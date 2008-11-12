package org.red5.io.composite.impl;



import java.io.File;
import java.io.IOException;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import org.red5.io.IStreamableFile;
import org.red5.io.ITag;
import org.red5.io.ITagReader;
import org.red5.io.ISeekableFile;
import org.red5.io.index.IIndex;
import org.red5.io.index.IKeyFramePair;

public class CompositeElement implements ITagReader, ISeekableFile {
	
	protected static Log log = LogFactory.getLog(CompositeElement.class.getName());

	public ITagReader reader;
	
	private long correction = 0;
	
	private long start_ts = 0;
	
	private long start_pos = 0;
	
	private long end_ts = 0;
	
	private long end_pos = 0;
	
	public CompositeElement(ITagReader reader) {
		this(reader, 0L, (long) reader.getDuration() * 1000);
	}
	
	/**
	 * @param reader - A reader for the file, assume it is not seeked to the correct position.
	 * @param start_ts - In point in milliseconds
	 * @param end_ts - Out point in milliseconds
	 */
	public CompositeElement(ITagReader reader, long start_ts, long end_ts) {
		IKeyFramePair pair;
		
		this.reader = reader;
		
		pair = reader.getIndex((double) start_ts);
		this.start_ts = (long) pair.getTimestamp();
		this.start_pos = pair.getPosition();
		
		pair = reader.getIndex((double) end_ts);
		this.end_ts = (long) pair.getTimestamp();
		this.end_pos = pair.getPosition();
		
		try {
			reader.position(start_pos);
		} catch (IOException e) {
			log.error("Could not move reader to correct position!");
			log.error(e.getStackTrace());
		}
	}

	/**
	 * @return the time in milliseconds that the clip starts in the file it is part of.
	 */
	public long getInPointTime() {
		return start_ts;
	}
	
	/**
	 * @return the time in milliseconds that the clip ends in the file it is part of.
	 */
	public long getOutPointTime() {
		return end_ts;
	}
	
	/**
	 * set the time in milliseconds that the clip starts in the file it is part of.
	 */
	public void setInPointTime(long ts) {
		start_ts = ts;
	}
	
	/**
	 * sets the time in milliseconds that the clip ends in the file it is part of.
	 */
	public void setOutPointTime(long ts) {
		end_ts = ts;
	}
	
	/**
	 * This sets the current reader and moves it to the start_pos position. Thus, 
	 * start_pos MUST be correct in this clip's position.
	 * @param rd
	 */
	public void setReader(ITagReader rd) {
		if (reader != null)
			reader.close();		
		reader = rd;
	}
	
	/**
	 * This sets the correction time to add to this reader's timestamps 
	 * to get the correct timestamps in the global timeline
	 * @param ts time in milliseconds
	 */
	public void setCorrection(long ts) {
		this.correction = ts;
	}
	
	public long getCorrection() {
		return this.correction;
	}
	
	/**
	 * Resets this reader to its starting position.
	 * @throws IOException TODO
	 */
	public void reset() {
		try {
			reader.position(start_pos);
		} catch (IOException e) {
			log.error("Could not reset reader!");
			log.error(e.getStackTrace());
		}
	}
	
	/**
	 * @return true if this is a valid element.
	 */
	public boolean isValid() {
		if (reader == null)
			return false;
		
		if (start_ts < 0 || start_ts > reader.getDuration() * 1000)
			return false;
		
		if (end_ts < 0 || end_ts > reader.getDuration() * 1000)
			return false;
		
		if (start_ts > end_ts)
			return false;
		
		return true;
	}
	
	public String getFileLocation() {
		//TODO: Implement some way to get the URL of the files
		throw new UnsupportedOperationException();
	}
	/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	 * What follows is the implementation of ITagReader
	 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	 */
	
	public IStreamableFile getStreamableFile() {
		return null;
	}

	/**
	 * Returns the offet length. This is the offset in the file itself.
	 * @return int
	 */
	public long getOffset() {
		return (int) getBytesRead();
	}

	/**
	 * Returns the amount of bytes read. This is the bytes read from the beginning of the clip.
	 * @return long
	 */
	public long getBytesRead() {
		return reader.getBytesRead() - this.start_pos;
	}

	/**
	 * Return length in seconds
     * @return
     */
	public double getDuration() {
		long duration = (end_ts - start_ts) / 1000;
		if (duration > reader.getDuration())
			return reader.getDuration();
		else if (duration > 0)
			return duration;
		else
			return 0;
	}

	/**
	 * Decode the header of the stream;
	 *
	 */
	public void decodeHeader() throws IOException {
		reader.decodeHeader();
	}

	/**
	 * Move the reader pointer to given position in file.
	 * 
	 * @param pos
	 *            File position to move to
	 */
	public void position(long pos) throws IOException {
		reader.position(pos);
	}

	/**
	 * Returns a boolean stating whether the FLV has more tags
	 * 
	 * @return boolean
	 */
	public boolean hasMoreTags() {
		return (getBytesRead() < (this.end_pos - this.start_pos)) && (reader.hasMoreTags());
	}


    
	/**
	 * Returns a Tag object
	 */
	public ITag readTag() throws IOException {
		ITag tag = reader.readTag();
		int newT = (int) (tag.getTimestamp() + this.getCorrection());
		if (log.isDebugEnabled()) 	//This causes a LOT of logging!
		//	log.debug("Rewriting timestamp " + tag.getTimestamp() + " to " + newT);
		if (newT < 0)
			log.error("Rewrote a timestamp to a negative value, this is illegal!");

		tag.setTimestamp(newT);	//Correct for the Timestamp change.
		return tag;
	}

	/**
	 * Closes the reader and free any allocated memory.
	 */
	public void close() {
		reader.close();
	}
	
	/**
	 * Check if the reader also has video tags.
	 */
	public boolean hasVideo() {
		return reader.hasVideo();
	}
	
	public IKeyFramePair getIndex(double ts) { return reader.getIndex(ts); }
	public IKeyFramePair getIndex(long pos) { return reader.getIndex(pos); }
	
	public IIndex getIndex() throws IOException {
		return reader.getIndex();
	}
	
	/**
	 * Seeks to this point in "global" time, thus we correct for where this clip is in the total timeline.
	 */
	public synchronized int seekToTime(int ts) {
		int in_ts = (int) (ts - this.getCorrection());
		int out_ts = ((ISeekableFile) reader).seekToTime(in_ts);
		out_ts = (int) (out_ts + this.getCorrection());
		if (log.isDebugEnabled())
			log.debug("Global timestamp " + ts + " corresponding to local file timestamp " + in_ts + " seeked to global timestamp " + out_ts);
		return out_ts;
	}
	
	public String toString() {
		return "CompositeElement " + "start time: " + (start_ts / 1000.0) + " (pos " + start_pos + "), end time: " + (end_ts / 1000.0) + " (pos " + end_pos + "), correction: " + (correction / 1000.0);
	}

	@Override
	public File getFile() {
		// TODO Auto-generated method stub
		return null;
	}
	
}
