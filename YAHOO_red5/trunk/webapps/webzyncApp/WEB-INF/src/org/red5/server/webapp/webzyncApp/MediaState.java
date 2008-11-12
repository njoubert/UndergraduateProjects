package org.red5.server.webapp.webzyncApp;

import java.util.HashMap;
import java.util.Map;

/**
 * Stores the stateful information of the current file playing in a room.
 * This class reads and created the data sent between the server and client. 
 * This is it's most important responsibility.
 * It also keeps track of the progression of time, to enable the zyncing of new clients.
 * @author njoubert, mbastaef
 *
 */
public class MediaState {

	private String filename;
	private double timestamp, duration;
	private double timestampModifiedAt;
	private boolean playing;
	
	
	/**
	 * Constructs a new 
	 * @param filename
	 * @param duration
	 */
	public MediaState(String filename, double duration) {
		this.filename = filename;
		this.duration = duration;
		setTimestamp(0,true);
	}

	/**
	 * Constructs a media state from a flash object
	 * @param data
	 */
	public MediaState(Map<String,Object> data) {
		this((String)data.get("name"),((Number)data.get("duration")).doubleValue());
		update(data);
	}

	/**
	 * Updates media state with new flash object data
	 * @param data
	 */
	public void update(Map<String,Object> data) {
		if (data.containsKey("time"))
			setTimestamp(((Number)data.get("time")).doubleValue(),(Boolean)data.get("playing"));
	}
	
	/**
	 * Changes the given timestamp while retaining the playing state.
	 * @param seekToTime
	 */
	public void seek(double seekToTime) {
		setTimestamp(seekToTime, playing);
	}
	
	
	private double getCurrentTime() {
		return System.currentTimeMillis() / 1000.0;
	}
	
	/**
	 * @return corrected current timestamp
	 */
	public double getTimestamp() {
		return timestamp + getOffset();
	}
	private double getOffset() {
		if (playing)
			return getCurrentTime() - timestampModifiedAt;
		return 0;
	}
	/**
	 * @param timestamp
	 * @param playing
	 */
	public void setTimestamp(double timestamp, boolean playing) {
		this.timestamp = timestamp;
		this.timestampModifiedAt = getCurrentTime();
		this.playing = playing;
	}
	/**
	 * @return the duration
	 */
	public double getDuration() {
		return duration;
	}
	/**
	 * @return playing
	 */
	public boolean isPlaying() {
		return playing;
	}
	/**
	 * @return filename
	 */
	public String getFilename() {
		return filename;
	}
	
	/**
	 * @param includeTimestamp
	 * @return Returns a flash object version of this media state
	 */
	public Map<String,Object> getMap(boolean includeTimestamp) {
		Map<String,Object> map = new HashMap<String, Object>();
		
		map.put("name", getFilename());
		map.put("duration", getDuration());
		if (includeTimestamp) {
			map.put("time", getTimestamp());
			map.put("playing", isPlaying());
		}
		return map;
	}
	public Map<String,Object> getMap() {
		return getMap(true);
	}

	public void setDuration(double d) {
		this.duration = d;
	}
	
}
