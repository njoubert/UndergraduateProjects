package org.red5.server.webapp.webzyncApp;

import java.util.HashMap;
import java.util.Map;

public class RemoteCall {

	private Map<String,Object> map = new HashMap<String,Object>();
	
	public RemoteCall(User user, String method) {
		map.put("userid", user.getUserId());
		map.put("displayname", user.getDisplayName());
		map.put("method", method);
	}
	
	/**
	 * Sets an argument in this remote call
	 * 
	 * @param argument
	 * @param value
	 */
	public void set(String argument, Object value) {
		map.put(argument,value);
	}

	/**
	 * Returns the map format of this remote call
	 * @return
	 */
	public Map<String, Object> getMap() {
		return map;
	}
	
	
	
}
