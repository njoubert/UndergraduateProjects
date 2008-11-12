/**
 * 
 */
package org.red5.server.webapp.webzyncApp;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.commons.codec.digest.DigestUtils;
import org.red5.io.flv.impl.FLVReader;
import org.red5.server.api.IClient;
import org.red5.server.api.IConnection;
import org.red5.server.api.IScope;
import org.red5.server.api.Red5;
import org.red5.server.webapp.webzyncApp.VideoDaemon.Video;
import org.springframework.core.io.Resource;

public class Room {
	/**
	 * 
	 */
	private final Application application;
	private String roomid;
	private MediaState roomState;
	private Map<IClient,User> users = new HashMap<IClient,User>();
	private String name;
	private String path;
	/**
	 * Create a new room with the given name. The room gets a unique ID for logging and
	 * tracking purposes.
	 * @param name - Name of the room.
	 * @param application TODO
	 */
	public Room(Application application, String name) {
		this.application = application;
		roomid = DigestUtils.md5Hex("room" + System.currentTimeMillis());
		Application.log.debug("New room created. roomid="+roomid);
		this.name = name;
		if (name.length() > 0)
			this.path = name + "/";
		else
			this.path = "";
		roomState = new MediaState(VideoUtils.getMovies(this).get(0));
	}

	/**
	 * Performs a seek op on the room
	 * @param user - who performed the seek
	 * @param roomState - state of the room this client reports. 
	 * @param seekToTime
	 */
	public void seek(User user, Map<String, Object> roomState, double seekToTime) {
		setRoomState(roomState);
		user.log("seek", getRoomState(), seekToTime);
		Application.log.debug("Remote call: User "+user+" Seek " + seekToTime);
		RemoteCall call = new RemoteCall(user, "seek");
		call.set("time", seekToTime);
		broadcast(call);
		this.roomState.seek(seekToTime);
	}
	
	/**
	 * Plays the room's file
	 * @param user - who issues the play command
	 * @param roomState - state of the room this client reports.
	 */
	public void play(User user, Map<String, Object> roomState) {
		setRoomState(roomState);
		user.log("play", getRoomState());
		Application.log.debug("Remote call: User "+user+" Play");
		broadcast(new RemoteCall(user, "play"));
	}
	
	/**
	 * @param user - who issues the command
	 * @param roomState - state of the room this client reports.
	 */
	public void pause(User user, Map<String, Object> roomState) {
		setRoomState(roomState);
		user.log("pause",getRoomState());
		Application.log.debug("Remote call: User "+user+" Pause");
		broadcast(new RemoteCall(user, "pause"));
	}
	
	/**
	 * @param user - who issues the command
	 * @param roomState  - state of the room this client reports.
	 * @param movie
	 */
	public void load(User user, Map<String, Object> roomState, String movie) {
		setRoomState(roomState);
		Application.log.debug("Remote call: User "+user+" Load " + movie);
		try {
			IScope scope = Red5.getConnectionLocal().getScope();
			Resource[] flvs = scope.getResources("streams/" + movie);
			FLVReader flv = new FLVReader(flvs[0].getFile());
			MediaState newState = new MediaState(flvs[0].getFile().getName(), flv.getDuration());
			setRoomState(newState);
			user.log("load", getRoomState(), movie);
			RemoteCall call = new RemoteCall(user, "load");
			call.set("media", newState.getMap(true));
			broadcast(call);
		} catch (Exception e) {
			//Error!
		}
	}

	/**
	 * Loads a movie
	 * @param user
	 * @param roomState
	 * @param video
	 */
	public void load(User user, MediaState roomState, Video video) {
		//XXX:setRoomState(roomState);
		Application.log.debug("Remote call: User "+user+" Load " + video.getFileName());
		try {
			FLVReader flv = new FLVReader(video.getFile());
			MediaState newState = new MediaState(video.getFileName(), flv.getDuration());
			setRoomState(newState);
			user.log("load", getRoomState(), video.getFileName());
			RemoteCall call = new RemoteCall(user, "load");
			call.set("media", newState.getMap(true));
			broadcast(call);
		} catch (IOException ex) {
			// This shouldn't happen, we shouldn't be loading videos that aren't loaded
			ex.printStackTrace();
		}
	}
	/**
	 * @param user - who issues the command
	 * @param roomState - state of the room this client reports.
	 * @param message
	 */
	public void sendChat(User user, Map<String, Object> roomState, String message) {
		RemoteCall call = new RemoteCall(user, "messageReceived");
		call.set("message", message);
		broadcast(call);
	}
	
	/**
	 * @return the unique id of the room
	 */
	public String getRoomId() {
		return roomid;
	}
	
	protected Map<IClient,User> getUsers() {
		return users;
	}
	private void setRoomState(MediaState roomState) {
		if (roomState==null)
			return;
		this.roomState = roomState;
	}
	private void setRoomState(Map<String, Object> roomState) {
		if (roomState == null)
			return;
		setRoomState(new MediaState(roomState));
	}
	/**
	 * @return the mediastate object of the current file in this room.
	 */
	public MediaState getRoomState() {
		return this.roomState;
	}

	/**
	 * Find a user based on connection
	 * @param conn
	 * @return
	 */
	protected User findUser(IConnection conn) {
		return users.get(conn.getClient());
	}
	
	/**
	 * Send a message to all users
	 * @param method
	 * @param arguments
	 */
	/*
	protected void broadcast(String method, Object ...arguments) {
		for (User user : users.values())
			user.invoke(method, arguments);
	}
	*/
	
	protected void broadcast(RemoteCall call) {
		for (User user : users.values())
			user.invoke("remote", call.getMap());
	}

	/**
	 * Adds the given user to this room. Should be called from User.setRoom and NOT manually.
	 * @param user
	 */
	public void addUser(User user) {
		user.log("joined",getRoomState());


		RemoteCall call = new RemoteCall(user, "enter");
		call.set("room", getName());
		user.invoke(call);
		
		
		user.setInstanceId(0);
		
		Collection<User> userValues = getUsers().values();
		Iterator<User> iter = userValues.iterator();
		while (iter.hasNext()) {
			User u = iter.next();
			if (u.getYUID().equals(user.getYUID()) && u.getInstanceId() == user.getInstanceId()) {
				user.setInstanceId(user.getInstanceId() + 1);
				iter = userValues.iterator();
			}
		}
		

		// Send current users to new user and new user to current users
		for (User existingUser : users.values()) {
			user.invoke(new RemoteCall(existingUser, "userJoined"));
			existingUser.invoke(new RemoteCall(user, "userJoined"));
		}
		
		//Add to list
		this.users.put(user.getClient(),user);
		
	}

	/**
	 * Removes the given user from this room.
	 * @param user
	 */
	public void removeUser(User user) {
		user.log("left",getRoomState());

		RemoteCall call = new RemoteCall(user, "leave");
		call.set("room", getName());
		user.invoke(call);
		
		users.remove(user.getClient());
		
		broadcast(new RemoteCall(user, "userLeft"));
		
		if (isEmpty())
			application.removeRoom(this);
	}

	/**
	 * @return room name
	 */
	public String getName() {
		return name;
	}
	
	/**
	 * @return True if the room is empty.
	 */
	public boolean isEmpty() {
		return (getUsers().size() < 1);
	}

	public String getPath() {
		return path;
	}


	
}