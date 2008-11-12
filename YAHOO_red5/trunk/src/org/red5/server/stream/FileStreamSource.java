package org.red5.server.stream;

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

import java.io.IOException;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.red5.io.ITag;
import org.red5.io.ITagReader;
import org.red5.io.ISeekableFile;
import org.red5.server.net.rtmp.event.AudioData;
import org.red5.server.net.rtmp.event.IRTMPEvent;
import org.red5.server.net.rtmp.event.Invoke;
import org.red5.server.net.rtmp.event.Notify;
import org.red5.server.net.rtmp.event.Unknown;
import org.red5.server.net.rtmp.event.VideoData;
import org.red5.server.net.rtmp.message.Constants;

/**
 * Represents stream source that is file
 */
public class FileStreamSource implements ISeekableStreamSource, Constants {
    /**
     * Logger
     */
	protected static Log log = LogFactory.getLog(FileStreamSource.class
			.getName());
    /**
     * Tag reader
     */
	private ITagReader reader;
    /**
     * Key frame metadata
     */


    /**
     * Creates file stream source with tag reader
     * @param reader    Tag reader
     */
	public FileStreamSource(ITagReader reader) {
		this.reader = reader;
	}

    /**
     * Closes tag reader
     */
    public void close() {
		reader.close();
	}

    /**
     * Get tag from queue and convert to message
     * @return  RTMP event
     */
    public IRTMPEvent dequeue() {

		if (!reader.hasMoreTags()) {
			return null;
		}
		try {
			
			ITag tag = reader.readTag();

		IRTMPEvent msg;
		switch (tag.getDataType()) {
			case TYPE_AUDIO_DATA:
				msg = new AudioData(tag.getBody());
				break;
			case TYPE_VIDEO_DATA:
				msg = new VideoData(tag.getBody());
				break;
			case TYPE_INVOKE:
				msg = new Invoke(tag.getBody());
				break;
			case TYPE_NOTIFY:
				msg = new Notify(tag.getBody());
				break;
			default:
				log.warn("Unexpected type? " + tag.getDataType());
				msg = new Unknown(tag.getDataType(), tag.getBody());
				break;
		}
		msg.setTimestamp(tag.getTimestamp());
		//msg.setSealed(true);
		return msg;
		
		} catch (IOException e) {
			log.error(e);
			return null;
		}
	}

	/** {@inheritDoc} */
    public boolean hasMore() {
		return reader.hasMoreTags();
	}
	
	/** {@inheritDoc} */
    public synchronized int seek(int ts) {
    	return ((ISeekableFile) reader).seekToTime(ts);
		
	}
}
