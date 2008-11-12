package org.red5.io.flv.impl;

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

import java.util.Calendar;
import java.io.EOFException;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.nio.BufferUnderflowException;
import java.nio.channels.FileChannel;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import java.io.File;
import java.io.IOException;
import javax.imageio.stream.FileImageInputStream;
import javax.imageio.stream.FileImageOutputStream;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.mina.common.ByteBuffer;
import org.red5.io.IStreamableFile;
import org.red5.io.ITag;
import org.red5.io.ITagReader;
import org.red5.io.ISeekableFile;
import org.red5.io.IncompleteFileException;
import org.red5.io.IoConstants;
import org.red5.io.amf.Output;
import org.red5.io.flv.FLVHeader;
import org.red5.io.index.IKeyFramePair;
import org.red5.io.index.IIndex;
import org.red5.io.index.IndexManager;
import org.red5.io.object.Serializer;
import org.red5.io.utils.IOUtils;

/**
 * A Reader is used to read the contents of a FLV file.
 * NOTE: This class is not implemented as threading-safe. The caller
 * should make sure the threading-safety.
 *
 * @author The Red5 Project (red5@osflash.org)
 * @author Dominick Accattato (daccattato@gmail.com)
 * @author Luke Hubbard, Codegent Ltd (luke@codegent.com)
 * @author Paul Gregoire, (mondain@gmail.com)
 * @author Niels Joubert, (niels@berkeley.edu)
 */

public class FLVReader implements IoConstants, ITagReader,
	ISeekableFile {

    /**
     * Logger
     */
    private static Log log = LogFactory.getLog(FLVReader.class.getName());

    /**
     * File
     */
    private File file;
    
    /**
     * File input stream
     */
    private FileInputStream fis;

    /**
     * File channel
     */
    private FileChannel channel;

    /**
     * Input byte buffer
     */
    private ByteBuffer in;

	/** Set to true to generate metadata automatically before the first tag. */
	private boolean generateMetadata;

	/** Position of first video tag. */
	private long firstVideoTag = -1;

	/** Position of first audio tag. */
	private long firstAudioTag = -1;

	/** Current tag. */
	private int tagPosition;

	/** Duration in milliseconds. */
	private long duration;

	/** Buffer type / style to use **/
	private static BufferType bufferType = BufferType.AUTO;

	private static int bufferSize = 1024;
	
	/** Use load buffer */
	private boolean useLoadBuf;
	
	/** The header of this FLV file. */
	private FLVHeader header;
	
	private IIndex index;
	
	
	/** Constructs a new FLVReader. */
    FLVReader() {
    	//Left empty for Bean Insertion.
	}
    
    /**
     * Creates FLV reader from file input stream
     * @param f         File
     * @throws FileNotFoundException
     */
    public FLVReader(File f) throws IOException {
		this(f, false);
	}
    


    /**
     * Creates FLV reader from file input stream, sets up metadata generation flag
     * @param f                    File input stream
     * @param generateMetadata     <code>true</code> if metadata generation required, <code>false</code> otherwise
     * @throws FileNotFoundException
     */
    public FLVReader(File f, boolean generateMetadata) throws IOException {
    	this(f, generateMetadata, true);
	}
    
    /**
     * Creates FLV reader from file input stream, sets up metadata generation flag,
     * and controls whether we actually initiate an Index. This is mainly for the Index
     * creation in FLVIndex that uses its own reader to read through the file and build an index.
     * @param f                    File input stream
     * @param generateMetadata     <code>true</code> if metadata generation required, <code>false</code> otherwise
     * @param initializeIndex - if false, we never request an FLVIndex and we do not enable any index-related methods.
     * @throws FileNotFoundException
     */
    public FLVReader(File f, boolean generateMetadata, boolean initializeIndex) throws IOException {
    	this.file = f;
		this.fis = new FileInputStream(f);
		this.generateMetadata = generateMetadata;
		channel = fis.getChannel();
		
		in = null;
		fillBuffer();
		
		postInitialize(initializeIndex);
    }

    /**
	 * Accepts mapped file bytes to construct internal members.
	 *
	 * @param generateMetadata         <code>true</code> if metadata generation required, <code>false</code> otherwise
     * @param buffer                   Byte buffer
	 */
	public FLVReader(ByteBuffer buffer, boolean generateMetadata) throws IOException {
		this.generateMetadata = generateMetadata;
		in = buffer;

		postInitialize(true);
	}

    /**
	 * Get the remaining bytes that could be read from a file or ByteBuffer
	 * @return          Number of remaining bytes
	 */
	private long getRemainingBytes() {
		if (!useLoadBuf) {
			return in.remaining();
		}

		try {
			return channel.size() - channel.position() + in.remaining();
		} catch (IOException e) {
			log.error("Error getRemainingBytes", e);
			return 0;
		}
	}

	/**
	 * Get the total readable bytes in a file or ByteBuffer
	 * @return          Total readable bytes
	 */
	public long getTotalBytes() {
		if (!useLoadBuf) {
			return in.capacity();
		}

		try {
			return channel.size();
		} catch (Exception e) {
			log.error("Error getTotalBytes", e);
			return 0;
		}
	}

	/**
	 * Get the current position in a file or ByteBuffer
	 * @return           Current position in a file
	 */
	private long getCurrentPosition() {
		long pos;

		if (!useLoadBuf) {
			return in.position();
		}

		try {
			if (in != null) {
				pos = (channel.position() - in.remaining());
			} else {
				pos = channel.position();
			}
			return pos;
		} catch (Exception e) {
			log.error("Error getCurrentPosition", e);
			return 0;
		}
	}

	/**
     * Modifies current position
     *
     * @param pos  Current position in file
     */
    private void setCurrentPosition(long pos) throws IOException {
		if (pos == Long.MAX_VALUE) {
			pos = file.length();
		}
		if (!useLoadBuf) {
			in.position((int) pos);
			return;
		}

			if (pos >= (channel.position() - in.limit())
					&& pos < channel.position()) {
				in.position((int) (pos - (channel.position() - in.limit())));
			} else {
				channel.position(pos);
				fillBuffer(bufferSize, true);
			}

	}

    /**
     * Loads whole buffer from file channel, with no reloading (that is, appending)
     */
    private void fillBuffer() {
		try {
			fillBuffer(0, false);
		} catch (IOException e) {
			log.error("Initializing the buffer with 0 bytes failed!");
			log.error(e);
		}
	}

	/**
	 * Loads data from channel to buffer
	 * @param amount         Amount of data to load with no reloading
	 */
	private boolean fillBuffer(long amount) throws IOException {
		return fillBuffer(amount, false);
	}

	/**
	 * Load enough bytes from channel to buffer.
	 * After the loading process, the caller can make sure the amount
	 * in buffer is of size 'amount' if we haven't reached the end of channel.
	 * @param amount The amount of bytes in buffer after returning,
	 * no larger than bufferSize
	 * @param reload Whether to reload or append.
	 */
	private boolean fillBuffer(long amount, boolean reload) throws IOException {
			if (amount > bufferSize)
				amount = bufferSize;
			boolean canFullyRead = true;
			
			// Read all remaining bytes if the requested amount reach the end
			// of channel.
			if (channel.size() - channel.position() < amount) {
				amount = channel.size() - channel.position();
				canFullyRead = false;
			}

			if (in == null) {
				switch (bufferType) {
					case HEAP:
						in = ByteBuffer.allocate(bufferSize, false);
						break;
					case DIRECT:
						in = ByteBuffer.allocate(bufferSize, true);
						break;
					case AUTO:
						in = ByteBuffer.allocate(bufferSize);
						break;
					default:
						in = ByteBuffer.allocate(bufferSize);
				}
				channel.read(in.buf());
				in.flip();
				useLoadBuf = true;
			}
			
			

			if (!useLoadBuf) {
				return false;
			}

			if (reload || in.remaining() < amount) {
				if (!reload) {
					in.compact();
				} else {
					in.clear();
				}
				channel.read(in.buf());
				in.flip();
			}
			
			return canFullyRead;

	}

    /**
     * Post-initialization hook, reads keyframe metadata and decodes header (if any)
     */
    private void postInitialize(boolean initializeIndex) throws IOException {
		if (log.isDebugEnabled()) {
			log.debug("FLVReader 1 - Buffer size: " + getTotalBytes()
					+ " position: " + getCurrentPosition() + " remaining: "
					+ getRemainingBytes());
		}
		
		decodeHeader();
		
		if (file != null && initializeIndex) 
			initializeIndex();
	}
    
    private void initializeIndex() {
    	IndexManager manager = IndexManager.getInstance();
    	index = manager.getIndex(this);
    }

	/**
     * Getter for buffer type (auto, direct or heap)
     *
     * @return Value for property 'bufferType'.
     */
    public static String getBufferType() {
		switch (bufferType) {
			case AUTO:
				return "auto";
			case DIRECT:
				return "direct";
			case HEAP:
				return "heap";
			default:
				return null;
		}
	}

	/**
     * Setter for buffer type
     *
     * @param bufferType Value to set for property 'bufferType'.
     */
    public static void setBufferType(String bufferType) {
		int bufferTypeHash = bufferType.hashCode();
		 switch (bufferTypeHash) {
			 case 3198444: //heap
				 //Get a heap buffer from buffer pool
				 FLVReader.bufferType = BufferType.HEAP;
				 break;
			 case -1331586071: //direct
				 //Get a direct buffer from buffer pool
				 FLVReader.bufferType = BufferType.DIRECT;
				 break;
			 case 3005871: //auto
				 //Let MINA choose
			 default:
				 FLVReader.bufferType = BufferType.AUTO;
			 }
	}

	/**
     * Getter for buffer size
     *
     * @return Value for property 'bufferSize'.
     */
    public static int getBufferSize() {
		return bufferSize;
	}

	/**
     * Setter for property 'bufferSize'.
     *
     * @param bufferSize Value to set for property 'bufferSize'.
     */
    public static void setBufferSize(int bufferSize) {
		// make sure buffer size is no less than 1024 bytes.
		if (bufferSize < 1024) {
			bufferSize = 1024;
		}
		FLVReader.bufferSize = bufferSize;
	}

	/**
	 * Returns the file buffer.
	 * 
	 * @return  File contents as byte buffer
	 */
	public ByteBuffer getFileData() {
		// TODO as of now, return null will disable cache
		// we need to redesign the cache architecture so that
		// the cache is layed underneath FLVReader not above it,
		// thus both tag cache and file cache are feasible.
		return null;
	}

	/** {@inheritDoc} */
    public void decodeHeader() throws IOException {
		// XXX check signature?
		// SIGNATURE, lets just skip
		fillBuffer(9);
		header = new FLVHeader();
		in.skip(3);
		header.setVersion(in.get());
		header.setTypeFlags(in.get());
		header.setDataOffset(in.getInt());
		if (log.isDebugEnabled()) {
			log.debug("Header: " + header.toString());
		}
	}

	/** {@inheritDoc}
	 */
	public IStreamableFile getStreamableFile() {
		return null;
	}
	
	public File getFile() {
		return file;
	}

	/** {@inheritDoc}
	 */
	public long getOffset() {
		return getCurrentPosition();
	}

	/** {@inheritDoc}
	 */
	public long getBytesRead() {
		// XXX should summarize the total bytes read or
		// just the current position?
		return getCurrentPosition();
	}

	/** {@inheritDoc} */
    public double getDuration() {
    	
    	try {
			if (index != null)
				return index.getDuration() / 1000.0;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			log.error(e.getStackTrace());
		}
		return 0;
	}

	/** {@inheritDoc}
	 */
	public boolean hasMoreTags() {
		return getRemainingBytes() > 15;
	}

    /**
     * Create tag for metadata event
     * @return         Metadata event tag
     */
    private ITag createFileMeta() throws IOException {
		// Create tag for onMetaData event
		ByteBuffer buf = ByteBuffer.allocate(1024);
		buf.setAutoExpand(true);
		Output out = new Output(buf);

        // Duration property
		out.writeString("onMetaData");
		Map<Object, Object> props = new HashMap<Object, Object>();
		props.put("duration", getDuration());
		if (firstVideoTag != -1) {
			long old = getCurrentPosition();
			setCurrentPosition(firstVideoTag);
			if (readTagHeader() != null) {	
				fillBuffer(1);
				byte frametype = in.get();
	            // Video codec id
				props.put("videocodecid", frametype & MASK_VIDEO_CODEC);
				setCurrentPosition(old);
			}
		}
		if (firstAudioTag != -1) {
			long old = getCurrentPosition();
			setCurrentPosition(firstAudioTag);
			if (readTagHeader() != null) {
				fillBuffer(1);
				byte frametype = in.get();
	            // Audio codec id
	            props.put("audiocodecid", (frametype & MASK_SOUND_FORMAT) >> 4);
				setCurrentPosition(old);
			}
		}
		props.put("canSeekToEnd", (index != null));
		out.writeMap(props, new Serializer());
		buf.flip();

		ITag result = new Tag(IoConstants.TYPE_METADATA, 0, buf.limit(), null, 0);
		result.setBody(buf);
		return result;
	}

	/** {@inheritDoc}
	 */
    public synchronized ITag readTag() throws IOException {
		long oldPos = getCurrentPosition();
		ITag tag = readTagHeader();
		if (tag == null)
			throw new EOFException("your pipe aint big enough. l0ser.");
		
		if (tagPosition == 0 && tag.getDataType() != TYPE_METADATA
				&& generateMetadata) {
			setCurrentPosition(oldPos);
			tagPosition++;
			return createFileMeta();
		}

		ByteBuffer body = ByteBuffer.allocate(tag.getBodySize());

		// XXX Paul: this assists in 'properly' handling damaged FLV files		
		long newPosition = getCurrentPosition() + tag.getBodySize();
		if (newPosition <= getTotalBytes()) {
			int limit;
			while (getCurrentPosition() < newPosition) {
				fillBuffer(newPosition - getCurrentPosition());
				if (getCurrentPosition() + in.remaining() > newPosition) {
					limit = in.limit();
					in.limit((int) (newPosition - getCurrentPosition()) + in.position());
					body.put(in);
					in.limit(limit);
				} else {
					body.put(in);
				}
			}

			body.flip();
			tag.setBody(body);
			tagPosition++;
		}

		return tag;
	}
    
    /**
     * TODO: Make this more robust
     * Only call at the beginning of a frame!!!
     * @return Frametype.
     */
    public byte getFrameType() throws IOException {
		fillBuffer(1);
		byte frametype = in.get();
		return (byte) ((frametype & MASK_VIDEO_FRAMETYPE) >> 4); 	
    }

	/** {@inheritDoc}
	 */
	public void close() {
		log.debug("Reader close");
		if (in != null) {
			in.release();
			in = null;
		}
		if (channel != null) {
			try {
				channel.close();
				fis.close();
			} catch (IOException e) {
				log.error("FLVReader :: close ::>\n", e);
			}
		}
	}


	/**
	 * Put the current position to pos.
	 * The caller must ensure the pos is a valid one
	 * (eg. not sit in the middle of a frame)
	 * @param pos         New position in file. Pass <code>Long.MAX_VALUE</code> to seek to end of file.
	 */
	public void position(long pos) throws IOException {
		setCurrentPosition(pos);
		if (pos == Long.MAX_VALUE) {
			//tagPosition = posTagMap.size()+1;
			return;
		}
	}

	/**
	 * Read only header part of a tag
	 *
	 * @return              Tag header
	 */
	public ITag readTagHeader() throws IOException {
		// PREVIOUS TAG SIZE
		fillBuffer(15);
		
		if (getRemainingBytes() < 15)
			return null;
		
		int previousTagSize = in.getInt();

		// START OF FLV TAG
		byte dataType = in.get();

		// The next two lines use a utility method which reads in
		// three consecutive bytes but stores them in a 4 byte int.
		// We are able to write those three bytes back out by using
		// another utility method which strips off the last byte
		// However, we will have to check into this during optimization
		int bodySize = IOUtils.readUnsignedMediumInt(in);
		int timestamp = IOUtils.readUnsignedMediumInt(in);
		// reserved
		in.getInt();

		return new Tag(dataType, timestamp, bodySize, null, previousTagSize);
	}
	
	/**
	 * @return true if FLV file contains video.
	 */
	public boolean hasVideo() {
		return (!header.getFlagAudio() && header.getFlagVideo());
	}
	

    /**
     * Buffer types (auto, direct or heap)
     */
    public enum BufferType {
		/**
		 * 
		 */
		AUTO,
		/**
		 * 
		 */
		DIRECT,
		/**
		 * 
		 */
		HEAP
	}
    
   
	/** {@inheritDoc}
	 */
    public IKeyFramePair getIndex(double ts) { 
		if (index == null)
			return null;
		
		try {
			
			FLVIndex.FLVKeyFramePair pair = (FLVIndex.FLVKeyFramePair) index.getKeyFrameFromTimestamp(ts);
			return pair;
			
		} catch (IOException e) {
			log.error("could not find keyframe");
			log.error(e.getStackTrace());
			return null;
		}
    }
    
	/** {@inheritDoc}
	 */
    public IKeyFramePair getIndex(long pos) { 
    	throw new UnsupportedOperationException(); 
    }
    

    /**
     * Seeks this reader to the closest keyframe to the given timestamp 
     * Returns timestamp that reader was set to.
     * 
     * @param ts - timestamp in milliseconds to seek to
     * @return timestamp seeked to.
     */
	public synchronized int seekToTime(int ts) {

		//TODO: change to double?
		IKeyFramePair pair = getIndex((double) ts);
		
		if (pair == null || pair.getTimestamp() < 0) 
			return ts;		//No valid timestamp found, skip it...
		
		try {
			this.position(pair.getPosition());
			return (int) pair.getTimestamp();
		} catch (IOException e) {
			log.error("Seeking to a valid position according to index caused an exception.");
			log.error(e);
			log.error(Arrays.toString(e.getStackTrace()));
			return ts;
		}
		
	}

	/**
	 * @see org.red5.io.ITagReader#getIndex()
	 */
	@Override
	public IIndex getIndex() throws IOException {
		return new FLVIndex(file);
	}

	public FLVHeader getHeader() {
		return header;
	}
	

    /*
     * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
     * 
     * END OF KEYFRAME INDEXING FOR SEEK ABILITY
     * 
     * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
     */

}
