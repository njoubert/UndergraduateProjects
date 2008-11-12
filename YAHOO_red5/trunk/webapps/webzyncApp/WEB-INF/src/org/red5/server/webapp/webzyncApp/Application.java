package org.red5.server.webapp.webzyncApp;

import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.red5.server.adapter.ApplicationAdapter;
import org.red5.server.api.IClient;
import org.red5.server.api.IConnection;
import org.red5.server.api.IScope;
import org.red5.server.api.Red5;
import org.red5.server.api.stream.IServerStream;
import org.red5.server.webapp.webzyncApp.db.DbLogger;

/**
 * The Webzync application syncs video streams across clients, giving a video-conferencing interface
 * to watch video with multiple people.
 * @author njoubert, mbastaef, Yahoo! Research Berkeley
 * @since Summer 2007
 *
 */
public class Application extends ApplicationAdapter {
	
	private IScope appScope;
	private IServerStream serverStream;
	private Remote remote;
	private String authUrl;
	
	private VideoDaemon videoDaemon;
	
	private static Map<String, Room> rooms;
	private Map<IClient,User> users;
	
	protected static Log log = LogFactory.getLog(Application.class.getName());
	protected DbLogger db;

	/** {@inheritDoc} */
    @Override
	public boolean appStart(IScope app) {
		appScope = app;
		
		remote = new Remote();
	    app.registerServiceHandler("remote", remote);
	    
	    try {
		    db = new DbLogger();
		    db.init();
	    } catch (Exception e) {
	    	log.error("We could not initialize the Database object for logging!");
	    	db = null;
	    }
	    
	    users = new HashMap<IClient,User>();
	    rooms = new HashMap<String, Room>();
	    try {
	    	videoDaemon = new VideoDaemon(app.getResource("streams").getFile());
	    } catch (IOException ex) {
	    	log.error(ex);
	    	//ex.printStackTrace();
	    }
		return true;
	}
    
    
    /**
     * The Remote class handles all the calls the client can make on the server.
     * @author njoubert, mbastaef
     *
     */
	public class Remote {
	
		/**
		 * The initialization handshake the client invokes to start his session.
		 * 
		 * @param cookie - If we can get the cookie from the user, we auth on this
		 * @return - The room, the file list and the room state in AS3.0 compatible format
		 */
		public Object initialize(String cookie, String roomName) {
			log.debug("User attempting to initialize connection...");
			log.debug("With cookie: " + cookie);
			
			String username = authUser(cookie);
			if (username == null || username == "")
				username = "guest#0"; //KEEP THE #

			log.info("User " + username + " logged into application");			
			
			User u = addUser(Red5.getConnectionLocal(), username);
			
			String video = null;
			
			if (roomName == null)
				roomName = "";
			
			int index = roomName.indexOf(';');
			if (index>=0 && index<roomName.length()-2) {
				video = roomName.substring(index+1);
				roomName = roomName.substring(0,index);
			}
			
			//HARD-CODED ROOM FOR NOW!
			Room room = getRoom(roomName);
			u.setRoom(room);

			if (room.getUsers().size()==1 && video!=null && video.length()>0)
				loadForeignContent(null, video);
			
			Map<String,Object> ret = new HashMap<String,Object>();
			List<Map<String, Object>> movies = VideoUtils.getMovies(room);
			List<Map<String, Object>> music = VideoUtils.getMp3s(room);
			ret.put("user", u.getUserId());
			ret.put("movies", movies);
			ret.put("music", music);
			ret.put("room", room.getName());
			ret.put("load", room.getRoomState().getMap(true));
			
			
			u.log("initialize", room.getRoomState());
			return ret;
			
		}
		
		
		/**
		 * Perform a seek on the room
		 * @param roomState - state of the room this client reports.
		 * @param seekToTime - time to seek to on this state.
		 */
		public void seek(Map<String, Object> roomState, double seekToTime) {
			User user = findUser( Red5.getConnectionLocal());
			user.getRoom().seek(user,roomState,seekToTime);
		}

		/**
		 * Perform a seek on the room
		 * @param roomState - state of the room this client reports.
		 * @param seekToTime - time to seek to on this state.
		 */
		public void join(String roomName) {
			User user = findUser( Red5.getConnectionLocal());
			int index = roomName.indexOf(';');
			if (index>0)
				roomName = roomName.substring(0,index);
			user.setRoom(getRoom(roomName));
		}

		/**
		 * Performs a play operation on the room
		 * @param roomState - state of the room this client reports.
		 */
		public void play(Map<String, Object> roomState) {
			User user = findUser( Red5.getConnectionLocal());
			user.getRoom().play(user,roomState);
		}
		
		/**
		 * Perform a pause operation on the room.
		 * @param roomState  - state of the room this client reports.
		 */
		public void pause(Map<String, Object> roomState) {
			User user = findUser( Red5.getConnectionLocal());
			user.getRoom().pause(user,roomState);
		}
		
		/**
		 * Loads the file with the relative path and filename "movie"
		 * @param roomState  - state of the room this client reports.
		 * @param movie - the movie to load (relative path and filename)
		 */
		public void load(Map<String, Object> roomState, String movie) {
			User user = findUser( Red5.getConnectionLocal());
			user.getRoom().load(user, roomState, movie);
		}
		
		/**
		 * Sends a chat message to everyone in the user's room.
		 * @param roomState - state of the room this client reports.
		 * @param message - chat message
		 */
		public void sendChat(Map<String, Object> roomState, String message) {
			User user = findUser( Red5.getConnectionLocal());
			user.getRoom().sendChat(user, roomState, message);
		}
		/**
		 * Issues a call to load Youtube, Google Video, Yahoo video or other video sites into Webzync.
		 * @param roomState - state of the room this client reports.
		 * @param url - url of page that has video on it.
		 */
		public void loadForeignContent(Map<String, Object> roomState, String url) {
			User user = findUser( Red5.getConnectionLocal());
			user.loadForeignContent(roomState, videoDaemon.getVideo(url));
		}
			
	}

	/**
	 * Validates a user's cookie and returns their username if successful.
	 * 
	 * @param cookie - Raw string of a user's cookies from the browser.
	 * @return Yahoo! username if successful, otherwise null or ""
	 */
	public String authUser(String cookie) {
		try {
			URL myUrl = new URL(authUrl);
			HttpURLConnection urlConn = (HttpURLConnection) myUrl.openConnection();
			urlConn.setRequestProperty("Cookie", cookie);
			urlConn.connect();
			InputStreamReader in = new InputStreamReader(urlConn.getInputStream());
			
			String user = "";
			int ch;
			while ((ch = in.read()) != -1)
				user += (char) ch;
			urlConn.disconnect();
			return user;
		} catch (MalformedURLException e) {
			return null;
		} catch (IOException e) {
			return null;
		}
	}

	/** {@inheritDoc} */
    @Override
	public boolean appConnect(IConnection conn, Object[] params) {
		return super.appConnect(conn, params);
	}

	/** {@inheritDoc} */
    @Override
	public void appDisconnect(IConnection conn) {
    	
		removeUser(conn);
		if (appScope == conn.getScope() && serverStream != null) {
			serverStream.close();
		}
		super.appDisconnect(conn);
	}
    
	/**
	 * Finds a user no matter which room they are in.
	 * @param conn
	 * @return
	 */
	protected User findUser(IConnection conn) {
		return users.get(conn.getClient());
	}
	
	/**
	 * Adds a user conditionally (if he doesn't exist) and returns it.  Note:
	 * returns null if the user already exists.
	 * @param connection
	 * @return the new user or null if the user already exists
	 */
	protected User addUser(IConnection connection, String username) {
		User user = findUser(connection);
		// Return null if we already added this user (counterintuitive?)
		if (user!=null) 
			return null;
		
		// Add new user
		this.users.put(connection.getClient(), user = new User(db, connection, username));
		user.log("connect",null);
		return user;
	}
	
	/**
	 * Remove a user based on connection
	 * @param connection
	 */
	protected void removeUser(IConnection connection) {
		User u = findUser(connection);
		if (u == null)
			return;

		if (u.getConnection()==connection)
			removeUser(u);
			
		
	}
	/**
	 * Remove a specific user
	 * @param user
	 */
	protected void removeUser(User user) {
		Room room = user.getRoom();
		if (room != null)
			room.removeUser(user);
		
		// Remove user from list
		users.remove(user.getClient());
		user.log("disconnect",user.getRoom().getRoomState());
	}
    
    protected Room getRoom(String name) {
    	if (rooms.containsKey(name))
    		return rooms.get(name);
    	Room room = new Room(this, name);
    	rooms.put(name, room);
    	return room;
    }
    
	protected void removeRoom(Room room) {
		if (room.isEmpty())
			rooms.remove(room.getName());
	}
	/**
	 * For bean initialization. Url of script that does cookie auth.
	 * @param url of auth script
	 */
    public void setAuthUrl(String url) {
    	this.authUrl = url;
    }

	public VideoDaemon getVideoDaemon() {
		return videoDaemon;
	}
    
    
}
