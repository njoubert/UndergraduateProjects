package org.red5.io;

import java.io.File;

public interface ISeekableFile {



	/**
	 * Sets file reader's position to the closest timestamp to given ts in milliseconds
	 * 
	 * @param ts - timestamp to seek to (milliseconds)
	 * @return timestamp seeked to.
	 */
	public int seekToTime(int ts);
	


}
