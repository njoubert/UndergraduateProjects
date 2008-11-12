package org.red5.server.webapp.webzyncApp.db;

import java.sql.DriverManager;
import java.sql.SQLException;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.red5.server.webapp.webzyncApp.MediaState;

import com.mysql.jdbc.Connection;
import com.mysql.jdbc.PreparedStatement;
import com.mysql.jdbc.ResultSet;

/**
 * Supplies server-side MySQL based logging for WebZync.
 * See the create.sql file in WEB-INF for db declerations.
 * @author njoubert
 *
 */
public class DbLogger {

	private static String username;
	private static String password;
	private static String dbUrl;
	private static boolean enable;
	private Connection conn;
	
	protected static Log log = LogFactory.getLog(DbLogger.class.getName());
	
	/**
	 * Tests the Db connection.
	 * @param args
	 */
	public static void main(String[] args) {
		DbLogger dd = new DbLogger("jdbc:mysql://localhost/webzync","root" ,"" );
		dd.init();
		System.out.println("DB Initialized.");
		dd.uinit();
		System.out.println("DB Closed.");
	}
	
	/**
	 * For Bean Init.
	 * 
	 */
	public DbLogger() {
		//Left empty
	}
	
	/**
	 * For Testing
	 * @param dbUrl - MySQL JDBC url to connect to
	 * @param username
	 * @param password
	 */
	public DbLogger(String dbUrl, String username, String password) {
		DbLogger.username = username;
		DbLogger.password = password;
		DbLogger.dbUrl = dbUrl;	
	}
	
	/**
	 * Initializes DB on object creation.
	 */
	public void init() {
		if (!DbLogger.enable)
			return;
		checkForConnection();
	}
	
	private void checkForConnection() {
		try {
			if (conn != null && !conn.isClosed() && conn.isValid(1))
				return;
			Class.forName("com.mysql.jdbc.Driver").newInstance();
			String url = dbUrl + "?user=" + username + "&password="+ password;
			conn = (Connection) DriverManager.getConnection(url);
		} catch( SQLException ex ){
		    log.error("SQLException: " + ex.getMessage());
		    log.error("SQLState: " + ex.getSQLState());
		    log.error("VendorError: " + ex.getErrorCode());
		} catch (Exception e) {
			log.error("Could not create database driver!");
		}
		if (conn == null)
			log.error("Could not get MySQL driver connection");	
		log.debug("Successfully created Database object.");
	}
	
	/**
	 * Call this to close all DB connections.
	 */
	public void uinit() {
		if (!DbLogger.enable)
			return;
	}
		

	
	
	/*
	 * ======================================
	 * HERE COMES THE ACTUAL LOGGING METHODS 
	 * ======================================
	 */
	
	/**
	 * Logs an event.
	 * @param yuid
	 * @param session_id
	 * @param roomid
	 * @param currentFile
	 * @param command
	 * @param args
	 * @throws Exception
	 */
	public void log(String yuid, String session_id, String roomid, MediaState currentFile, String command, Object args) throws Exception {
		
		if (!DbLogger.enable) {
			log.debug("Logging disabled.");
			return;
		}
		
		PreparedStatement stmt = null;
		int rs = -1;
		
		int mediaId = findOrCreateMedia(currentFile, yuid);
		
		try {
			log.debug("Creating statement for logging event!");
			stmt = (PreparedStatement) conn.prepareStatement("INSERT INTO events (yuid, session_id, roomid, timestamp, command, args, media, date) VALUES (?, ?, ?, ?, ?, ?, ?, NOW())");
			stmt.setString(1, yuid);
			stmt.setString(2, session_id);
			stmt.setString(3, roomid);
			if (currentFile != null)
				stmt.setDouble(4, currentFile.getTimestamp());
			else
				stmt.setDouble(4, -1);
			stmt.setString(5, command);
			if (args != null)
				stmt.setString(6, args.toString());	//This is suspect...
			else
				stmt.setString(6, "");
			stmt.setInt(7, mediaId);
			
			log.debug("Executing query: " + stmt);
		    rs = stmt.executeUpdate();
		    log.debug("Found resultset: " + rs);
		    log.info("Logged with returned id " + rs);
		    
		} catch (SQLException sx) {
			log.error(sx);
		} catch (Exception e) {
			log.error(e);

		} finally {

		    if (stmt != null) {
		        try {
		            stmt.close();
		        } catch (SQLException sqlEx) { 
		        	//Left empty
		        }
		        stmt = null;
		    }
		}
		
		
	}
	
	/*
	 * MEDIA TABLE STUFF
	 */
	
	
	/**
	 * Creates a new entry for some media item.
	 * @param url
	 * @param userId
	 * @return id
	 */
	private int newMedia(String url, String userId) {
		PreparedStatement stmt = null;
		int rs = -1;

		try {
			log.debug("Creating new media in database: " + url);
			stmt = (PreparedStatement) conn.prepareStatement("INSERT INTO media (url) VALUES (?)");
			stmt.setString(1, url);
			log.debug("Executing query: " + stmt);
		    rs = stmt.executeUpdate();
		    log.debug("Found resultset: " + rs);
		    
		} catch (SQLException sx) {
			log.error(sx);
		} catch (Exception e) {
			log.error(e);

		} finally {

		    if (stmt != null) {
		        try {
		            stmt.close();
		        } catch (SQLException sqlEx) {
		        	//Left Empty
		        }
		        stmt = null;
		    }
		}
		
		return rs;
	}
	
	private int findMediaByUrl(String url) {
		PreparedStatement stmt = null;
		ResultSet rs = null;
		int id = -1;
		try {
			log.debug("Finding media " + url);
			stmt = (PreparedStatement) conn.prepareStatement("SELECT * FROM media WHERE URL = ?");
			stmt.setString(1, url);
		    rs = (ResultSet) stmt.executeQuery();
		    if (rs.next()) {
		    	id = rs.getInt("id");
		    	log.debug("Found id " + id);
		    }
		} catch (SQLException sx) {
			//Left Empty
		} finally {

		    if (rs != null) {
		        try {
		            rs.close();
		        } catch (SQLException sqlEx) {
		        	//empty
		        }
		        rs = null;
		    }
		    if (stmt != null) {
		        try {
		            stmt.close();
		        } catch (SQLException sqlEx) {
		        	//empty
		        }
		        stmt = null;
		    }
		}

		return id;
	}
	
	private int findOrCreateMedia(MediaState currentFile, String yuid) {
		if (currentFile == null)
			return -1;
		String url = currentFile.getFilename();
		int id = findMediaByUrl(url);
		if (id < 1)
			id = newMedia(url, yuid);
		return id;
	}
	
	/*
	 * ======================================
	 * HERE ENDS THE ACTUAL LOGGING METHODS 
	 * ======================================
	 */
	
	/**
	 * For Bean
	 * @param username
	 */
	public void setUsername(String username) {
		DbLogger.username = username;
	}
	/**
	 * For Bean
	 * @param password
	 */
	public void setPassword(String password) {
		DbLogger.password = password;
	}
	/**
	 * For Bean
	 * @param dbUrl
	 */
	public void setDbUrl(String dbUrl) {
		DbLogger.dbUrl = dbUrl;
	}
	/**
	 * For Bean
	 * @param enable
	 */
	public void setEnable(String enable) {
		log.debug("Setting log to " + enable);
		DbLogger.enable = Boolean.parseBoolean(enable);
	}
	
}
