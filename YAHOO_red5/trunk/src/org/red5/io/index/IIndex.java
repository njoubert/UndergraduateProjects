package org.red5.io.index;

import java.io.IOException;

/**
 * Represents the index file of some media we can play.
 * @author njoubert
 *
 */
public interface IIndex {
	
	/**
	 * Queries the Index for the information of a KeyFrame closest to a given timestamp.
	 * Thus giving a timestamp longer than the file will return to the last keyframe.
	 * @param ts - the ideal timestamp to find information of.
	 * @return - the KeyFramePair closest to the given timestamp. Null if we can't find it.
	 * @throws InterruptedException 
	 * @throws IOException 
	 */
	public IKeyFramePair getKeyFrameFromTimestamp(double ts) throws IOException;
	
	/**
	 * Queries the Index for the information of a KeyFrame closest to the given byte offset
	 * in the original FLV. Thus giving a position bigger than the file (in bytes) will return
	 * the last keyframe.
	 * @param pos - the byte offset into the original FLV.
	 * @return - the KeyFramePair closest to the given byte offset. Null if we can't find it.
	 * @throws InterruptedException 
	 * @throws IOException 
	 */
	public IKeyFramePair getKeyFrameFromPosition(long pos) throws IOException;
	
	/**
	 * This calculates the file duration from the index - thus, the last timestamp
	 * plus the duration of the last frame.
	 * @return The duration in seconds of the file the Index knows about. -1 if we dont know.
	 * @throws InterruptedException 
	 * @throws IOException 
	 */
	public double getDuration() throws IOException;
	
}
