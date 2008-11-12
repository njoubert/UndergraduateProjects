package org.red5.io;

import java.io.File;
import java.io.IOException;

import org.red5.io.index.IKeyFramePair;
import org.red5.io.index.IIndex;

/*
 * RED5 Open Source Flash Server - http://www.osflash.org/red5
 * 
 * Copyright (c) 2006-2007 by respective authors (see below). All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free Software 
 * Foundation; either version 2.1 of the License, or (at your option) any later 
 * version. 
 * 
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along 
 * with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 */

/**
 * Any reader that can read FLV tags to send to the client.
 */
public interface ITagReader {

	/**
	 * Return the file that is loaded.
	 * 
	 * @return the file to be loaded
	 */
	public IStreamableFile getStreamableFile();
	
	public File getFile();

	/**
	 * Returns the offet length
	 * 
	 * @return int
	 */
	public long getOffset();

	/**
	 * Returns the amount of bytes read
	 * 
	 * @return long
	 */
	public long getBytesRead();

	/**
	 * Return length in seconds
     * @return duration as double in seconds.
     */
	public double getDuration();

	/**
	 * Decode the header of the stream;
	 * @throws IOException if the header could not be read
	 */
	public void decodeHeader() throws IOException;

	/**
	 * Move the reader pointer to given position in file.
	 * 
	 * @param pos
	 *            File position to move to
	 * @throws IOException TODO
	 */
	public void position(long pos) throws IOException;

	/**
	 * Returns a boolean stating whether the FLV has more tags
	 * 
	 * @return boolean
	 */
	public boolean hasMoreTags();

	/**
	 * Returns a Tag object
	 * 
	 * @return Tag
	 * @throws IOException if the tag could not be read
	 */
	public ITag readTag() throws IOException;

	/**
	 * Closes the reader and free any allocated memory.
	 */
	public void close();
	
	/**
	 * Check if the reader also has video tags.
	 * @return true if video tags exist
	 */
	public boolean hasVideo();
	
	/**
	 * Finds the timestamp and position closest to the given timestamp in the file. 
	 * No effect on reader's position.
	 * @param ts is milliseconds
	 * @return IIndexPair representing the given index point.
	 */
	public IKeyFramePair getIndex(double ts);
	
	/**
	 * Finds the timestamp and position closest to the given position in the file. 
	 * No effect on reader's position.
	 * @param pos - the position to look for (byte offset)
	 * @return IIndexPair representing the given index point.
	 */
	public IKeyFramePair getIndex(long pos);
	
	/**
	 * @return the class of the Index this media item uses 
	 * @throws IOException if index could not be created.
	 */
	public IIndex getIndex() throws IOException;

}
