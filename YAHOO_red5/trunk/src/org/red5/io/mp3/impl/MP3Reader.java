package org.red5.io.mp3.impl;

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

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteOrder;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;

import javax.imageio.stream.FileImageInputStream;
import javax.imageio.stream.FileImageOutputStream;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.mina.common.ByteBuffer;
import org.red5.io.ISeekableFile;
import org.red5.io.IStreamableFile;
import org.red5.io.ITag;
import org.red5.io.ITagReader;
import org.red5.io.IoConstants;
import org.red5.io.amf.Output;
import org.red5.io.flv.impl.Tag;
import org.red5.io.index.IIndex;
import org.red5.io.index.IKeyFramePair;
import org.red5.io.object.Serializer;

import org.red5.io.mp3.impl.MP3LiteHeader;

/**
 * Read MP3 files
 */
public class MP3Reader implements ITagReader, ISeekableFile {
    /**
     * Logger
     */
	protected static Log log = LogFactory.getLog(MP3Reader.class.getName());

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
     * Memory-mapped buffer for file content
     */
	private MappedByteBuffer mappedFile;
    /**
     * Source byte buffer
     */
	private ByteBuffer in;
    /**
     * Last read tag object
     */
	private ITag tag;
    /**
     * Previous tag size
     */
	private int prevSize;
    /**
     * Current time
     */
	private double currentTime;

	private int dataRate;
    /**
     * Whether first frame is read
     */
	private boolean firstFrame;
    /**
     * File metadata
     */
	private ITag fileMeta;
    /**
     * File duration
     */
	private double duration;


	MP3Reader() {// Only used by the bean startup code to initialize the frame cache
	}
	
    /**
     * Creates reader from file input stream
     * @param stream          File input stream source
     */
    public MP3Reader(File file) throws FileNotFoundException {
    	this.file = file;
		fis = new FileInputStream(file);
        // Grab file channel and map it to memory-mapped byte buffer in read-only mode
        channel = fis.getChannel();
		try {
			mappedFile = channel.map(FileChannel.MapMode.READ_ONLY, 0, channel
					.size());
		} catch (IOException e) {
			log.error("MP3Reader :: MP3Reader ::>\n", e);
		}

        // Use Big Endian bytes order
        mappedFile.order(ByteOrder.BIG_ENDIAN);
        // Wrap mapped byte buffer to MINA buffer
        in = ByteBuffer.wrap(mappedFile);
		firstFrame = true;
		
        // Process ID3v2 header if present
		processID3v2Header();

        if (!initializeIndex()) {
        	log.warn("Could not initialize index for MP3 file " + file.getAbsolutePath());
        	seekingEnabled = false;
        }
        
        // Create file metadata object
        fileMeta = createFileMeta();
        
        // MP3 header is length of 32 bits, that is, 4 bytes
        // Read further if there's still data
        if (in.remaining() > 4) {

            MP3LiteHeader header = findNextFrame();
            // Set position
            in.position(in.position() - 4);
            // Check header
            if (header != null) {
				checkValidHeader(header);
			} else {
				throw new RuntimeException("No initial header found.");
			}
            
		}
	}

    /**
     * A MP3 stream never has video.
     * 
     * @return always returns <code>false</code>
     */
    public boolean hasVideo() {
    	return false;
    }
	
	/**
	 * Check if the file can be played back with Flash. Supported sample rates are
     * 44KHz, 22KHz, 11KHz and 5.5KHz
     * 
	 * @param header       Header to check
	 */
	private void checkValidHeader(MP3LiteHeader header) {
		switch (header.getSampleRate()) {
			case 44100:
			case 22050:
			case 11025:
			case 5513:
				break; // Supported sample rate
			default:
				throw new RuntimeException("Unsupported sample rate: " + header.getSampleRate());
		}
	}

    /**
     * Creates file metadata object
     * @return         Tag
     */
    private ITag createFileMeta() {
		// Create tag for onMetaData event
		ByteBuffer buf = ByteBuffer.allocate(1024);
		buf.setAutoExpand(true);
		Output out = new Output(buf);
		out.writeString("onMetaData");
		Map<Object, Object> props = new HashMap<Object, Object>();
		props.put("duration", (double) duration / 1000);
		props.put("audiocodecid", IoConstants.FLAG_FORMAT_MP3);
		if (dataRate > 0) {
			props.put("audiodatarate", dataRate);
		}
		props.put("canSeekToEnd", true);
		out.writeMap(props, new Serializer());
		buf.flip();

		ITag result = new Tag(IoConstants.TYPE_METADATA, 0, buf.limit(), null,
				prevSize);
		result.setBody(buf);
		return result;
	}

	/** {@inheritDoc} */
    public IStreamableFile getStreamableFile() {
		// TODO Auto-generated method stub
		return null;
	}

	/** {@inheritDoc} */
    public long getOffset() {
		// TODO Auto-generated method stub
		return 0;
	}

	/** {@inheritDoc} */
    public long getBytesRead() {
		return in.position();
	}

	/** {@inheritDoc} */
    public double getDuration() {
		return duration / 1000.0;
	}

	/** {@inheritDoc} */
    public boolean hasMoreTags() {
		MP3LiteHeader header = findNextFrame();
		if (header == null) {
			return false;
		}

		if (header.frameSize() == 0) {
			// TODO find better solution how to deal with broken files...
			// See APPSERVER-62 for details
			return false;
		}
		
		if (in.position() + header.frameSize() - 4 > in.limit()) {
			// Last frame is incomplete
			in.position(in.limit());
			return false;
		}

		in.position(in.position() - 4);
		return true;
	}

	/** {@inheritDoc} */
    public synchronized ITag readTag() {
		if (firstFrame) {
			// Return file metadata as first tag.
			firstFrame = false;
			return fileMeta;
		}

		MP3LiteHeader header = findNextFrame();
		if (header == null) {
			return null;
		}

		int frameSize = header.frameSize();
		if (frameSize == 0) {
			// TODO find better solution how to deal with broken files...
			// See APPSERVER-62 for details
			return null;
		}
		
		if (in.position() + frameSize - 4 > in.limit()) {
			// Last frame is incomplete
			in.position(in.limit());
			return null;
		}

		tag = new Tag(IoConstants.TYPE_AUDIO, (int) currentTime, frameSize + 1,
				null, prevSize);
		prevSize = frameSize + 1;
		currentTime += header.frameDuration();
		ByteBuffer body = ByteBuffer.allocate(tag.getBodySize());
		byte tagType = (IoConstants.FLAG_FORMAT_MP3 << 4)
				| (IoConstants.FLAG_SIZE_16_BIT << 1);
		switch (header.getSampleRate()) {
			case 44100:
				tagType |= IoConstants.FLAG_RATE_44_KHZ << 2;
				break;
			case 22050:
				tagType |= IoConstants.FLAG_RATE_22_KHZ << 2;
				break;
			case 11025:
				tagType |= IoConstants.FLAG_RATE_11_KHZ << 2;
				break;
			default:
				tagType |= IoConstants.FLAG_RATE_5_5_KHZ << 2;
		}
		tagType |= (header.isStereo() ? IoConstants.FLAG_TYPE_STEREO : IoConstants.FLAG_TYPE_MONO);
		body.put(tagType);
		final int limit = in.limit();
		body.putInt(header.getData());
		in.limit(in.position() + frameSize - 4);
		body.put(in);
		body.flip();
		in.limit(limit);

		tag.setBody(body);

		return tag;
	}
    
    

	/** {@inheritDoc} */
    public void close() {
		mappedFile.clear();
		if (in != null) {
			in.release();
			in = null;
		}
		try {
			fis.close();
			channel.close();
		} catch (IOException e) {
			log.error("MP3Reader :: close ::>\n", e);
		}
	}

	/** {@inheritDoc} */
    public void decodeHeader() {
	}


  
    private void processID3v2Header() {
    	if (in.remaining() <= 10)
    		// We need at least 10 bytes ID3v2 header + data
    		return;
    	
    	int start = in.position();
    	byte a, b, c;
    	a = in.get();
    	b = in.get();
    	c = in.get();
    	if (a != 'I' || b != 'D' || c != '3') {
    		// No ID3v2 header
    		in.position(start);
    		return;
    	}
    	
    	// Skip version and flags
    	in.skip(3);
    	int size = (in.get() & 0x7f) << 21 | (in.get() & 0x7f) << 14 | (in.get() & 0x7f) << 7 | (in.get() & 0x7f);
    	// Skip ID3v2 header for now
    	in.skip(size);
    }


    
    /*
     * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
     * 
     * FOLLOWING IS ALL THE METHODS AND MEMBERS THAT IS USED TO HANDLE THE INDEX FILE
     * THAT ENABLES SEEKING TO ARBITRARY TIMESTAMPS IN A FLV FILE
     * 
     * Terminology:
     * 
     * - "IndexFile" / "Index File" 	- the file on the hard drive storing the actual data.
     * - "index" / "Index" 					- the abstract idea of an index of keyframes 
     * 
     * IndexFile structure:
     * 
     * 	HEADER:	
     * 		version number		-	integer
     *  	last modified data	-	long
     *  	duration			-	long
     *  	flags				-	byte
     *  	datarate			-	integer
     * 	BODY:  (packets of:)
     * 		position			-	long
     * 		timestamp			-	integer
     *  
     *  FLAGS:
     *  	AudioOnly	-	Mask 0x1	(Least Significant bit indicates audio only FLV)
     *  	MP3 File	-	Mask 0x2	(Second Significant bit indicates MP3 File)
     *  
     * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
     */  
    
	//	 INDEXING CONSTANTS:
	
    public static final int HEADER_SIZE = 25;
    
    public static final int PACKET_SIZE = 16;
    
    public static final long bogus = 0xCAFEBABE;
    
    public static final long BOGUS_HEADER = (bogus << 32) | 0x2FADFACE;
    
    private static final byte AUDIO_ONLY_MASK = 0x1;
    
    private static final byte MP3_FILE_MASK = 0x2;
    
    // INDEXING VARIABLES:
    
	private static final int indexVersion = 2;
    
	public long indexLastModified;
    
	public byte flags = 0x2;	//Indicates that this is an MP3 File in the IDX header
    
	public boolean indexValid = false; //Shows whether we have a valid index file or not.
	
	public boolean seekingEnabled = false; //checkIndexValidity has to enable seeking
    
    public File indexFile;  //If Null, not yet initialized!
    
    // INDEXING HELPER CLASSES
    
    /**
     * IndexPair is an object that stores file position, timestamp pairs
     */
    public class IndexPair implements IKeyFramePair {
    	/**
    	 * Keyframe timestamp in milliseconds
    	 */
    	public double   timestamp = -1;
    	/**
    	 * Keyframe file position in bytes
    	 */
    	public long  position = -1;
    	
    	public double getTimestamp() {return timestamp;}
    	public long getPosition() {return position;}
    }
    
    /**
     * Initialized the indexfile when class gets instantiated.
     * Does NOT depend on presistance of file, only uses it to pull paths and filenames.
	 * Depends on "file" being set by constructor.
     * @return false if seeking / indexing is not supported.
     */
    private synchronized boolean initializeIndex() {
    	if (file == null)
    		return false;
    	
    	seekingEnabled = true;
    	currentTime = 0;
    	String filename = file.getAbsolutePath() + ".idx";
    	indexFile = new File(filename);  
    	try {
    		
    		if (!indexFile.exists()) {
    			if (indexFile.createNewFile())
    				seekingEnabled = indexValid = analyzeKeyFrames();			
    			else
    				log.error("Could not create index file!");
    		} else {
    			return checkIndexValidity();
    		}
    		return indexValid;
    	} catch (IOException err) {
    		log.error(err);
    		return false;
    	}
    }
    
    /**
     * Checks the IndexFile Validity, 
     * sets indexVersion, indexLastModified, duration, indexAudioOnly,indexValid
     * and returns value.
     * @return - value of indexValid variable after doing check.
     */
    private synchronized boolean checkIndexValidity() {
		if (!seekingEnabled)
			return false; //Don't seek if not enabled!
		
    	if (indexFile == null || !indexFile.exists())
    		indexValid = false;
    	try {
    		FileImageInputStream istream = new FileImageInputStream(indexFile);
			byte flags = 0x0;
    		long fileSize = istream.length();
    		
			// The file isn't big enough to get our information
			if ( fileSize < 41 ) {
				indexValid = false;
				return indexValid;
			}
			
			// Check for the correct version number
			int version = istream.readInt();
			if ( version != indexVersion ) {
				indexValid = analyzeKeyFrames();
			}
			
			// Make sure the file hasn't changed
			indexLastModified = istream.readLong();
			if ( indexLastModified != file.lastModified() ) {
				if (indexLastModified == BOGUS_HEADER) {
					log.warn("previous index file not completely generated! File: " + file.getAbsolutePath());
				}
				indexValid = analyzeKeyFrames();
				return indexValid;
			}
			
			duration = istream.readLong();
			
			flags = istream.readByte();
			if (((flags & MP3_FILE_MASK) <= 0) || ((flags & ~MP3_FILE_MASK) > 0)) {
				indexValid = false;	//This is not an MP3 file? CRAP!
				log.error("A non-MP3 Index is associated with an MP3 file, fail seeking! File: " + file.getAbsolutePath());
				return indexValid;
			}
			dataRate = istream.readInt();
			indexValid = true;
			
    	} catch (IOException err) {
    		log.error(err);
    		indexValid = false;
    	}
    	return indexValid;
    }

    public IKeyFramePair getIndex(double ts) {
    	return findPlaceFromTimestamp(ts);
    }
    
    /**
     * Looks up the closest keyframe to given timestamp in index file.
     * Checks for validity and returns given ts if failed.
     * @param ts
     * @return IndexPair closest to given timestamp (milliseconds)
     */
    private IndexPair findPlaceFromTimestamp(double ts) {
    	
    	IndexPair pair = new IndexPair();
    	if (!indexValid || !seekingEnabled) {
    		pair.timestamp = currentTime;
    		pair.position = in.position();
    	}
    	
		try {
			FileImageInputStream istream = new FileImageInputStream(indexFile);
			
			long approxBytePos = HEADER_SIZE;
			istream.seek(HEADER_SIZE);
			long first_ps = istream.readLong();
			double first_ts = istream.readDouble();
			istream.seek(istream.length() - PACKET_SIZE);
			long last_ps = istream.readLong();
			double last_ts = istream.readDouble();
			
			if (ts >= this.duration) {
				
				approxBytePos = istream.length() - PACKET_SIZE;
				
			} else if (ts < last_ts && ts > 0) {
				
				double ratio = ((double)ts) / (last_ts - first_ts);
				if (ratio > 1) {
					ratio = 1;
				}
				double guess = (ratio * (istream.length() - HEADER_SIZE));
				guess = guess - (guess % PACKET_SIZE) - PACKET_SIZE;
				approxBytePos = (long)guess + HEADER_SIZE;
				if (log.isDebugEnabled()) {	
					log.debug("First_TS: " + first_ts);
					log.debug("Last_TS: " + last_ts);
					log.debug("Ratio: " + ratio);
				}
			}
			
			if (log.isDebugEnabled())
				log.debug("About to seek to byte pointer: " + approxBytePos);
			
			istream.seek(approxBytePos);
			
			long oldguess_ps, guess_ps;
			double oldguess_ts, guess_ts;
			guess_ps = istream.readLong();
			guess_ts = istream.readDouble();
			if (log.isDebugEnabled())
				log.debug("Corresponds to position: " + guess_ps);
	
			if (ts >= this.duration || ts <= 0 || guess_ts == ts) {
				pair.position = guess_ps;
				pair.timestamp = guess_ts;
				istream.close();
				return pair;
			} else if (guess_ts > ts ) {
				
				//Seek up
				for (long i = approxBytePos; i > 0; i = i - 2*PACKET_SIZE) {
					istream.seek(i);
					oldguess_ps = guess_ps;
					oldguess_ts = guess_ts;
					guess_ps = istream.readLong();
					guess_ts = istream.readInt();
					if (guess_ts < ts) {
						pair.position = oldguess_ps;
						pair.timestamp = oldguess_ts;
						if (log.isDebugEnabled()) {
							log.debug("Seeked up. Found byte address: " + istream.getStreamPosition());
						}
						istream.close();
						return pair;
					}
				}
				
			} else if (guess_ts < ts) {
				
				//seek down
				for (long i = approxBytePos; i < istream.length() - PACKET_SIZE; i = i + PACKET_SIZE) {
					oldguess_ps = guess_ps;
					oldguess_ts = guess_ts;
					guess_ps = istream.readLong();
					guess_ts = istream.readInt();
					if (guess_ts > ts) {
						if (log.isDebugEnabled()) {
							log.debug("Seeked down. Found byte address: " + istream.getStreamPosition());
						}
						pair.position = oldguess_ps;
						pair.timestamp =  oldguess_ts;
						istream.close();
						return pair;
					}
				}
				
			}
				
		istream.close();
	} catch (IOException err) {
		log.error("could not load keyframe data for getClosestKeyFrame", err);
	}	
	return pair;
    }
    
    public IKeyFramePair getIndex(long pos) {
    	return findPlaceFromPosition(pos);
    }
    
    /**
     * Looks up the closest timestamp and position to the given position in index file.
     * Checks for validity and returns the given pos if failed.
     * @param pos - byte offset into mp3 file
     * @return IndexPair closest to given position (byte address)
     */
    private IndexPair findPlaceFromPosition(long pos) {
    	IndexPair pair = new IndexPair();
    	if (!indexValid || !seekingEnabled) {
    		pair.timestamp = currentTime;
    		pair.position = in.position();
    	}
    	
    	if (pos == Long.MAX_VALUE) {
    		// Seek at EOF
    		pair.position = in.limit();
    		pair.timestamp = duration;
    		return pair;
    	}
    	
		try {
			FileImageInputStream istream = new FileImageInputStream(indexFile);
			
			long approxBytePos = HEADER_SIZE;
			istream.seek(HEADER_SIZE);
			long first_ps = istream.readLong();
			double first_ts = istream.readDouble();
			istream.seek(istream.length() - PACKET_SIZE);
			long last_ps = istream.readLong();
			double last_ts = istream.readDouble();
			
			if (pos >= istream.length()) {
				
				approxBytePos = istream.length() - PACKET_SIZE;
				
			} else if (pos < last_ps && pos > 0) {
				
				double ratio = ((double)pos) / (last_ps - first_ps);
				if (ratio > 1) {
					ratio = 1;
				}
				double guess = (ratio * (istream.length() - HEADER_SIZE));
				guess = guess - (guess % PACKET_SIZE) - PACKET_SIZE;
				approxBytePos = (long)guess + HEADER_SIZE;
				if (log.isDebugEnabled()) {	
					log.debug("First_PS: " + first_ps);
					log.debug("Last_PS: " + last_ps);
					log.debug("Ratio: " + ratio);
				}
			}
			
			if (log.isDebugEnabled())
				log.debug("About to seek to byte pointer: " + approxBytePos);
			
			istream.seek(approxBytePos);
			
			long oldguess_ps, guess_ps;
			double oldguess_ts, guess_ts;
			guess_ps = istream.readLong();
			guess_ts = istream.readDouble();
			if (log.isDebugEnabled())
				log.debug("Corresponds to position: " + guess_ps);
	
			if (pos >= istream.length() || pos <= 0 || guess_ps == pos) {
				pair.position = guess_ps;
				pair.timestamp = guess_ts;
				istream.close();
				return pair;
			} else if (guess_ps > pos ) {
				
				//Seek up
				for (long i = approxBytePos; i > 0; i = i - 2*PACKET_SIZE) {
					istream.seek(i);
					oldguess_ps = guess_ps;
					oldguess_ts = guess_ts;
					guess_ps = istream.readLong();
					guess_ts = istream.readInt();
					if (guess_ps < pos) {
						pair.position = oldguess_ps;
						pair.timestamp = oldguess_ts;
						if (log.isDebugEnabled()) {
							log.debug("Seeked up. Found byte address: " + istream.getStreamPosition());
						}
						istream.close();
						return pair;
					}
				}
				
			} else if (guess_ps < pos) {
				
				//seek down
				for (long i = approxBytePos; i < istream.length() - PACKET_SIZE; i = i + PACKET_SIZE) {
					oldguess_ps = guess_ps;
					oldguess_ts = guess_ts;
					guess_ps = istream.readLong();
					guess_ts = istream.readInt();
					if (guess_ps > pos) {
						if (log.isDebugEnabled()) {
							log.debug("Seeked down. Found byte address: " + istream.getStreamPosition());
						}
						pair.position = oldguess_ps;
						pair.timestamp =  oldguess_ts;
						istream.close();
						return pair;
					}
				}
				
			}
				
		istream.close();
	} catch (IOException err) {
		log.error("could not load keyframe data for getClosestKeyFrame", err);
	}	
	return pair;
    	
    }

	/** {@inheritDoc} */
    private synchronized boolean analyzeKeyFrames() throws IOException {
		if (indexFile == null)
			throw new IOException("indexFile not set!");
		
    	Calendar cal = Calendar.getInstance();
    	long startTime = cal.getTimeInMillis();
    	
		FileImageOutputStream ostream;
		
		try {
			ostream = new FileImageOutputStream(indexFile);
		} catch (IOException err) {
			log.error("could not open stream to save keyframe data", err);
			return false;
		}

    	if (log.isDebugEnabled())
    		log.debug("Creating index for MP3 file!");
    	
		dataRate = 0;
		long rate = 0;
		int count = 0;
		int origPos = in.position();
		double time = 0;
		
		//Writing bogus header info 
		ostream.writeInt(indexVersion);
		ostream.writeLong(BOGUS_HEADER);
		ostream.writeLong(BOGUS_HEADER);
		ostream.writeByte(flags);
		ostream.writeInt(dataRate);

		in.position(0);
		processID3v2Header();
		MP3LiteHeader header;
		while ((header = findNextFrame()) != null) {

			if (header.frameSize() <= 0) {
				// TODO find better solution how to deal with broken files... See APPSERVER-62 for details
		    	if (log.isDebugEnabled())
		    		log.debug("Broke out of index creation cause we hit a header with 0 framesize");

				break;
			}
			
			int pos = in.position() - 4;
			
			if (pos + header.frameSize() - 4 > in.limit()) {	
		    	if (log.isDebugEnabled())
		    		log.debug("Broke out of index creation cause we hit the last incomplete frame, pos: " + pos + " ts: " + time);
				break;  // Last frame is incomplete
			}

			ostream.writeLong(pos);
			ostream.writeDouble(time);

			rate += header.getBitRate();
			time += header.frameDuration();
			in.position(pos + header.frameSize());
			count++;
		}
		// restore the pos
		in.position(origPos);
		
		indexLastModified = file.lastModified();
		currentTime = 0;
		duration = (long) (time);
		dataRate = (int) ((double) rate / (double) count);
		
		//Write read header
		ostream.seek(0);
		ostream.writeInt(indexVersion);
		ostream.writeLong(indexLastModified);
		ostream.writeLong((long) duration);
		ostream.writeByte(flags);
		ostream.writeInt(dataRate);
		//ostream.flush(); necessay?
		ostream.close();

		if (log.isDebugEnabled()) {
			cal = Calendar.getInstance();
			long endTime = cal.getTimeInMillis();
			long totalTime = endTime - startTime;
			log.debug("File index created in " + totalTime + " ms");
		}
		
		return true;
	}

    
    
    /**
     * Seeks this reader to the closest keyframe to the given timestamp 
     * Returns timestamp that reader was set to.
     * 
     * @param ts - timestamp in milliseconds to seek to
     * @return timestamp seeked to.
     */
	public synchronized int seekToTime(int ts) {
		if (!seekingEnabled)
			return ts; //Don't seek if not enabled!
			
		if (!indexValid)
			if (!checkIndexValidity()) 
				return ts;	//Index is not valid, skip it...
		
		if (log.isDebugEnabled())
			log.debug("Asked to seek to time " + ts);
		
		IndexPair pair = findPlaceFromTimestamp(ts);
		
		if (pair.timestamp < 0) 
			return ts;		//No valid timestamp found, skip it...
		
		//searchNextFrame(); 			//Should no be necessary
		if (pair.timestamp > -1) {
			currentTime = pair.timestamp;
			in.position((int) pair.position);
		} else {
			// Unknown frame position - this should never happen
			log.error("Found a negative timestamp value from index... File: " + file.getAbsolutePath());
			currentTime = 0;
		}
		return (int) pair.timestamp;
	}
	
    
	/** {@inheritDoc} */
    public void position(long pos) throws IOException {
    	if (!seekingEnabled)
    		return;
    	
		if (!indexValid)
			if (!checkIndexValidity()) 
				return;	//Index is not valid, skip it...

		// Make sure we can resolve file positions to timestamps 
		IndexPair place = findPlaceFromPosition(pos);
		
		// Advance to next frame
		//searchNextFrame();  		//Should not be necessary
		if (place.timestamp > -1) {
			currentTime = place.timestamp;
			in.position((int) place.position);
		} else {
			// Unknown frame position - this should never happen
			log.error("Found a negative timestamp value from index... File: " + file.getAbsolutePath());
			currentTime = 0;
		}
	}
    
    /*
     * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
     * 	MP3 FRAME HANDLING PROCEDURES
     * 
     *  We run through an MP3 File, looking for headers and creating an MP3LiteHeader class to represent it.
     *  
     *  THINGS TO NOTE:
     *  
     *  The findNextFrame() procedure leaves the file pointer to point to the first byte after the header,
     *  thus if you want to read the audio, you can call findNextFrame to get info, calculate the
     *  frame size, and read the frame from the current point in the frame buffer.
     *  
     *  Timing in this file is handles by successively counting the time as the frames are pulled.
     * 
     * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
     */
    
	public static final int HEADER_MASK = 0xfff00000;
	
	public static final int sampleRateTable[][] = 
	{	
		    // version 0
		    {22050, 24000, 16000, 0},
		    // version 1
		    {44100, 48000, 32000, 0}
	};
	
	public static final long bitRateTable[][][] = 
	{
	    // version 0
	    {
	        {0},
	        // one list per layer 1-3
	        {0, 32000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 144000,
	            160000, 176000, 192000, 224000, 256000, 0},
	        {0, 8000, 16000, 24000, 32000, 40000, 48000, 56000, 64000, 80000, 96000, 
	            112000, 128000, 144000, 160000, 0},
	        {0, 8000, 16000, 24000, 32000, 40000, 48000, 56000, 64000, 80000, 96000, 
	            112000, 128000, 144000, 160000, 0},
	    },
	    // version 1
	    {
	        {0},
	        // one list per layer 1-3
	        {0, 32000,  64000,  96000,  128000, 160000, 192000, 224000, 256000, 
	            288000, 320000, 352000, 384000, 416000, 448000, 0},
	        {0, 32000,  48000,  56000,   64000,  80000,  96000, 112000, 128000, 
	            160000, 192000, 224000, 256000, 320000, 384000, 0},
	        {0, 32000,  40000,  48000,   56000,  64000,  80000,  96000, 112000, 
	            128000, 160000, 192000, 224000, 256000, 320000, 0},
	    }
	};
	
	private int header;
	private int m_header;
	
	private boolean m_first_packet = true;
	private double  m_frame_size;
	private long    m_next_frame;
	private byte    m_layer;
	private byte    m_id;
	private int     m_channels;
	private int	    m_bitrate_index;
	private long    m_bitrate;
	private int	    m_samplerate_index;
	private long    m_samplerate;
	private byte	m_protection;
	private double  m_frame_duration; 
	private long    m_frame_time; 
    
	/**
	 * Searches through the file until it finds the next valid header in the "in" stream.
	 * When it finds that valid header, it created an MP3LiteHeader that represents it, and
	 * puts the "in" stream to the beginning of that frame (thus, just AFTER the header)
	 * @return
	 */
    private MP3LiteHeader findNextFrame() {
		boolean found = false;
		
		while ((in.limit() - in.position()) > 4 && !found ) {
			
				header = in.getInt();
				
				if ((header & HEADER_MASK) == HEADER_MASK) {
					
					//Read in what seems to be the header!		
					byte id = 		(byte) 	((header >>> 19) & 0x01);
					byte layer = 	(byte) 	(-(header >>> 17) & 0x03);
					byte protection =(byte) 	(~(header >>> 16) & 0x01);
					int bitrate_index = 			((header >>> 12) & 0x0f);
					int samplerate_index = 		((header >>> 10) & 0x03);
					int padding = 		((header >>> 9) & 0x01);
					//int privatebit = 	((header & 0x00000100) >>> 8);
					int mode = 				((header >>> 6) & 0x03);
					int mode_ext = 			((header >>> 4) & 0x03);
					int copy = 				((header & 0x00000008) >>> 3);
					int orig = 				((header & 0x00000004) >>> 2);
					int emph = 				((header & 0x00000003));

					//Now we need to check out data				
					if ( m_first_packet )
						m_first_packet = false;
					else {
						if (m_layer != layer || m_id != id || m_samplerate_index != samplerate_index) {
							//These should not be changing mid-stream...
							in.position(in.position() - 3);
							continue;
						}
					}
					
					long samplerate = sampleRateTable[id][samplerate_index];
					long bitrate = bitRateTable[id][layer][bitrate_index];
					
					int channels = 0;
					byte bound = 0;
					switch (mode) {
						case 0: //Stereo
							channels = 2;
							bound = 32;
							break;
						case 1: //Joint stereo
							channels = 2;
							bound = (byte) ((mode_ext + 1) << 2);
							break;
						case 2: //Dual channel
							channels = 2;
							bound = 32;
							break;
						case 3: //Mono
							channels = 1;
							bound = 0;
							break;
					}
					
					double frameSize = 0;
					int nsamples = 0;
					if ( bitrate == 0 ) {
						in.position(in.position() - 3);
						continue;
						//throw new IOException("NO SUPPORT FOR FREE BITRATES!");
					} else if ( layer == 1 ) {
						nsamples = 384;
						frameSize = (nsamples * bitrate / samplerate) / 8;
						if (padding > 0)
							frameSize += 4;
					} else {
						if (id == 0) {
							nsamples = 576;
						} else {
							nsamples = 1152;
						}
						
						frameSize = (nsamples * bitrate / samplerate) / 8;
						if (padding > 0) 
							frameSize += 1;
					}

					m_frame_size = frameSize;
					m_layer = layer;
					m_id = id;
					m_channels = channels;
					m_bitrate = bitrate;
					m_bitrate_index = bitrate_index;
					m_samplerate = samplerate;
					m_samplerate_index = samplerate_index;
					m_protection = protection;
					m_header = header;
					
					m_frame_duration = (double) (nsamples * 1000) / ((double) m_samplerate);
					
					found = true;
					
				} else {
					
					//NOT on header... Lets search on!
					in.position(in.position() - 3);
					continue;
					
				}
				
				if (found) {
					
					MP3LiteHeader retval = new MP3LiteHeader();
					retval.data = header;
					retval.bitRate = m_bitrate;
					retval.channels = m_channels;
					retval.duration = m_frame_duration;
					retval.protectionBit = (m_protection > 0);
					retval.samplingRate = m_samplerate;
					retval.size = (int) m_frame_size;
					try {
						//printHeader(true);
					} catch (Exception e) {
						
					}
					return retval;
				} else {
					
				}
				
		}
		return null;

    }
    
	public void printHeader(boolean full) throws IOException {
		System.out.printf("%x, RAW HEADER: %x \n",in.position(),m_header);
		if (!full)
			return;
		System.out.println("\tID: " + m_id);
		System.out.println("\tLayer: " + m_layer);
		System.out.println("\tCRCs: " + m_protection);
		System.out.println("\tBitrate: " + m_bitrate);
		System.out.println("\tSamplerate: " + m_samplerate);
		System.out.println("\tFrame size: " + m_frame_size);
		System.out.println("\tFrame duration: " + m_frame_duration);
		System.out.println("\tFrame timestamp: " + duration);
		
	}

	@Override
	public File getFile() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public IIndex getIndex() throws IOException {
		// TODO Auto-generated method stub
		return null;
	}
	
    
}
