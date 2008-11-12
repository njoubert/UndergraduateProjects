/**
 * 
 */
package org.red5.server.webapp.webzyncApp;

import java.util.Arrays;
import java.util.Map;

import org.apache.commons.codec.digest.DigestUtils;
import org.red5.io.flv.impl.FLVReader;
import org.red5.server.api.IClient;
import org.red5.server.api.IConnection;
import org.red5.server.api.service.IServiceCapableConnection;
import org.red5.server.webapp.webzyncApp.VideoDaemon.Video;
import org.red5.server.webapp.webzyncApp.db.DbLogger;
import org.red5.server.webapp.webzyncApp.event.VideoEvent;
import org.red5.server.webapp.webzyncApp.event.VideoListener;

class User {
		
		/**
		 * 
		 */
		private DbLogger db;
		private IServiceCapableConnection connection;
		private IClient client;
		private String yuid;
		private Room room = null;
		private String sessionId;
		private int instanceId;
		/**
		 * Creates class that represents one user, one session. Can be attached
		 * to multiple rooms during the session (one at a time), and has a specific file
		 * hooked to it.
		 * @param connection
		 * @param username
		 * @param application
		 */
		public User(DbLogger db, IConnection connection, String username) {
			this.db = db;
			this.connection = (IServiceCapableConnection)connection;
			this.client = connection.getClient();
			this.yuid = username;
			this.sessionId = DigestUtils.md5Hex(username + "" + System.currentTimeMillis());
			this.instanceId = 0;
		}
		
//		
//		public Map<String, Object> getCurrentFile() {
//			if (room==null)
//				return null;
//			return CurrentfileInterpreter.inferCurrentfile(room.getCurrentFile());
//		}

		/**
		 * Invokes a function on this specific client.
		 * @param method - method to call on client
		 * @param arguments - args to this method
		 */
		void invoke(String method, Object ...arguments) {
			Application.log.debug(this + ": invoking "+method+" with args "+Arrays.toString(arguments)+" on "+connection);
			connection.invoke(method, arguments);
		}
		/**
		 * Invokes a call on the client
		 * See client for listing of possible calls.
		 * @param call - the map as returned to getDefaultMap() that represents the specific call.
		 */
		public void invoke(RemoteCall call) {
			invoke("remote", call.getMap());
		}
		/**
		 * @return the connection this client is connected to.
		 */
		public IConnection getConnection() {
			return connection;
		}
		/**
		 * @return the specific client of this user.
		 */
		public IClient getClient() {
			return client;
		}
		/**
		 * @return The unique YUID the 
		 */
		public String getYUID() {
			return yuid;
		}
		/**
		 * @return The name to be publicly displayed by the client. a combination of instance id and username.
		 */
		public String getDisplayName() {
			if (instanceId > 0)
				return getUsername() + "." + instanceId;
			return getUsername();
		}
		/**
		 * @return The username part of the YUID
		 */
		public String getUsername() {
			int delimIndex = getYUID().indexOf('#');
			if (delimIndex > 0)
				return getYUID().substring(0, delimIndex);
			return getYUID();
		}
		/**
		 * @return Our internal userid - the yahoo id login along with the session id.
		 */
		public String getUserId() {
			return getUsername() + "#" + getSessionId();
		}
		/**
		 * @return The specific session of this user, changes every time he initializes.
		 */
		public String getSessionId() {
			return this.sessionId;
		}
		/**
		 * @return Returns a string of the userid.
		 */
		@Override
		public String toString() {
			return getUserId();
		}
		/**
		 * Attached this user to a specific room, and adds the user to the room.
		 * @param room
		 */
		public void setRoom(Room room) {
			if (this.room==room) return;
			if (this.room != null)
				this.room.removeUser(this);
			room.addUser(this);
			this.room = room;
		}
		/**
		 * @return room user is connected to.
		 */
		public Room getRoom() {
			return room;
		}
		/**
		 * @return Can we log this user?
		 */
		public boolean isOptedIn() {
			return true;
		}
		/**
		 * Logs a command
		 * @param command to log.
		 * @param currentFile - state of room this user reports.
		 */
		public void log(String command, MediaState currentFile) {
			log(command, currentFile, null);
		}
		
		/**
		 * Log an event - a remote client invoking a method on its peers or on us.
		 * @param command - the command sent
		 * @param currentFile -  state as reported by this user.
		 * @param arg - the (single) argument of the command.
		 */
		public void log(String command, MediaState currentFile, Object arg) {
			if (db == null) {
				Application.log.error("The Database object does not exist!");
				return;
			}
			try {
				String roomId = ((getRoom() != null) ? getRoom().getRoomId() : "");
				if (isOptedIn())
					db.log(getYUID(), getSessionId(), roomId, currentFile, command, arg);
				else
					db.log("anonymous", getSessionId(), getRoom().getRoomId(), currentFile, command, arg);
			} catch (Exception e) {
				//Logging failed - which probably means someone is spoofing us.
				Application.log.error("We tried to log, but an exception occured... Did we get spoofed?");
				Application.log.error(e);
				e.printStackTrace();
				
			}
		}
		/**
		 * @param instanceId - the nth instance of this user in some room.
		 */
		public void setInstanceId(int instanceId) {
			this.instanceId = instanceId;
		}

		public int getInstanceId() {
			return instanceId;
		}

		public void loadForeignContent(Map<String, Object> roomState, Video video) {
			if (video.isLoaded()) {
				getRoom().load(this, null, video);
			} else {
				video.addVideoListener(new VideoListener() {
					private long lastSend = 0;
					private boolean started = false;
					
					private void checkPlay(VideoEvent event) {
						if (!started && event.getVideo().canPlay()) {
 							getRoom().load(User.this, null, event.getVideo());
 							started = true;
						}
						
					}

					public void errorOccurred(VideoEvent event) {
						RemoteCall call = new RemoteCall(User.this, "error");
						call.set("message", "Error loading video: " + event.getVideo());
						invoke(call);
					}

					public void statusChanged(VideoEvent event) {
						RemoteCall call = new RemoteCall(User.this, "status");
						call.set("message", "Status update: " + event.getStatus());
						invoke(call);
						checkPlay(event);
						sendDuration(event);
					}

					private void sendDuration(VideoEvent event) {
						if (started && getRoom().getRoomState().getFilename().equals(event.getVideo().getFileName())) {
							// TODO: move this code somewhere else
							try {
								FLVReader flvReader = new FLVReader(event.getVideo().getFile());
								RemoteCall call = new RemoteCall(User.this, "newDuration");
								double duration = flvReader.getDuration();
								getRoom().getRoomState().setDuration(duration);
								call.set("duration",duration);
								getRoom().broadcast(call);
							} catch (Exception e) {
								e.printStackTrace();
							}
						}
					}
					
					public void transferredData(VideoEvent event) {
						if (lastSend==0 || System.currentTimeMillis()-lastSend>1000) {
							RemoteCall call = new RemoteCall(User.this, "status");
							call.set("message", "Transferred: " + event.getVideo()+" : " + event.getDownloadedBytes()*100/event.getFileSize()+"%");
							invoke(call);
							lastSend = System.currentTimeMillis();
						}
						checkPlay(event);
						sendDuration(event);
					}
				});
				video.load();
			}
		}

	}