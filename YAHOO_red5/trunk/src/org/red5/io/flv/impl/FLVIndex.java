package org.red5.io.flv.impl;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.Calendar;

import org.red5.io.IoConstants;
import javax.imageio.stream.FileImageInputStream;
import javax.imageio.stream.FileImageOutputStream;
import javax.imageio.stream.ImageInputStream;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.red5.io.ITag;
import org.red5.io.IoConstants;
import org.red5.io.flv.FLVHeader;
import org.red5.io.index.IIndex;
import org.red5.io.index.IKeyFramePair;

/**
 * Responsible for managing index files with meta data on disk.
 * @author njoubert
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 * 
 * FOLLOWING IS ALL THE METHODS AND MEMBERS THAT IS USED TO HANDLE THE KEYFRAME INDEX FILE
 * THAT ENABLES SEEKING TO ARBITRARY TIMESTAMPS IN A FLV FILE
 * 
 * 
 * FLVIndex file structure:
 * 
 * 	HEADER:	
 * 		0	-	version number		-	integer
 *  	4	-	last modified date	-	long (millisecs)
 *  	12	-	last known size  	-   long (in bytes)
 *  	20	-	duration			-	long (millisecs)
 *  	28	-	flags				-	byte
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

public class FLVIndex implements IIndex, IoConstants  {

    private static Log log = LogFactory.getLog(FLVIndex.class.getName());

    private File flvFile;
	private File indexFile;
	private FileImageInputStream in;
	private IndexBuilder indexBuilder;
	private Status status;
	private FLVIndexHeader header;
    
    private static final int INDEX_VERSION = 3;

	/**
	 * Creates a new FLVIndex for the given FLV file.
	 * @param flvFile - the actualy FLV file on disk we want to index.
	 * @throws IOException - if the file does not exist and cannot be created, or the FLV file does not exist.
	 */
	public FLVIndex(File flvFile) throws IOException {
		
		if (flvFile == null || !flvFile.exists())
			throw new IOException("File does not exist. Can't make index.");
		
		this.flvFile = flvFile;
		indexFile = new File(flvFile.getAbsolutePath() + ".idx");

		if (!indexFile.exists())
			if (!indexFile.createNewFile())
				throw new IOException("Could not create index file on disk. Permission error, possibly?");

		setStatus(Status.THREAD_NONE);
		updateIndex();
		
	}

	/**
	 * @see org.red5.io.index.IIndex#getDuration()
	 */
	@Override
	public double getDuration() throws IOException {
		log.info("getDuration on index called.");
		requireIndex();
		
		if (header != null) {
			log.info("Duration: " + header.getDuration());
			return header.getDuration();
		}
		
		log.error("There was no header to get the duration from! Even though requireIndex was called.");
		return 0;
	}

	/**
	 * @see org.red5.io.index.IIndex#getKeyFrameFromPosition(long)
	 */
	@Override
	public IKeyFramePair getKeyFrameFromPosition(long pos) throws IOException {
		//requireIndex();
		throw new UnsupportedOperationException();
	}

	/**
	 * @see org.red5.io.index.IIndex#getKeyFrameFromTimestamp(double)
	 */
	@Override
	public IKeyFramePair getKeyFrameFromTimestamp(double ts) throws IOException {
		requireIndex();
		
		if (status == Status.THREAD_ERROR)
			return null;
		
		FileImageInputStream istream = new FileImageInputStream(indexFile);;
		FLVKeyFramePair pair = new FLVKeyFramePair();
		
		double duration = getDuration();
		
		long approxBytePos = FLVIndexHeader.HEADER_SIZE;
		istream.seek(approxBytePos);
		istream.readLong(); //Read first position.
		int first_ts = istream.readInt();
		istream.seek(istream.length() - FLVKeyFramePair.KEYFRAME_SIZE);
		istream.readLong(); //Read last position
		long last_ts = istream.readInt();
		
		if (ts >= duration) { //Setting the byte position to move to
			
			approxBytePos = istream.length() - FLVKeyFramePair.KEYFRAME_SIZE;
			
		} else if (ts < last_ts && ts > 0) {
			
			double ratio = ts / (last_ts - first_ts);
			if (ratio > 1)
				ratio = 1;
			
			double guess = (ratio * (istream.length() - FLVIndexHeader.HEADER_SIZE));
			guess = guess - (guess % FLVKeyFramePair.KEYFRAME_SIZE) - FLVKeyFramePair.KEYFRAME_SIZE;
			approxBytePos = (long)guess + FLVIndexHeader.HEADER_SIZE;
			
			if (log.isDebugEnabled()) {	
				log.debug("First_TS: " + first_ts);
				log.debug("Last_TS: " + last_ts);
				log.debug("Ratio: " + ratio);
			}
			
		}

		log.debug("About to seek to byte pointer: " + approxBytePos);
		istream.seek(approxBytePos);
		
		long oldguess_ps, guess_ps = istream.readLong();
		int oldguess_ts, guess_ts = istream.readInt();

		log.debug("Corresponds to position in FLV: " + guess_ps);

		if (ts >= duration || ts <= 0 || guess_ts == ts) {
			
			pair.setPosition(guess_ps);
			pair.setTimestamp(guess_ts);
			istream.close();
			return pair;
			
		} else if (guess_ts > ts ) { //Seek up
			
			for (long i = approxBytePos; i > 0; i = i - 2*FLVKeyFramePair.KEYFRAME_SIZE) {
				istream.seek(i);
				oldguess_ps = guess_ps;
				oldguess_ts = guess_ts;
				guess_ps = istream.readLong();
				guess_ts = istream.readInt();
				if (guess_ts < ts) {
					pair.setPosition(oldguess_ps);
					pair.setTimestamp(oldguess_ts);
					log.debug("Seeked down. Found byte address: " + oldguess_ps + " with timestamp " + oldguess_ts);
					istream.close();
					return pair;
				}
			}
			
		} else { //Seek down
			
			for (long i = approxBytePos; i < istream.length() - FLVKeyFramePair.KEYFRAME_SIZE; i += FLVKeyFramePair.KEYFRAME_SIZE) {
				oldguess_ps = guess_ps;
				oldguess_ts = guess_ts;
				guess_ps = istream.readLong();
				guess_ts = istream.readInt();
				if (guess_ts > ts) {
					log.debug("Seeked down. Found byte address: " + oldguess_ps + " with timestamp " + oldguess_ts);
					pair.position = oldguess_ps;
					pair.timestamp =  oldguess_ts;
					istream.close();
					return pair;
				}
			}
		}
		istream.close();
		return null; //Should never happen.
	
	}

	protected boolean isIndexValid() {
		try {
			FileImageInputStream in = new FileImageInputStream(indexFile);
			FLVIndexHeader current = readHeader(in);
			if (current == null)
				return false;
			FLVIndexHeader expected = new FLVIndexHeader(INDEX_VERSION, 
					flvFile.lastModified(), flvFile.length(), current.getDuration(), current.isAudioOnly());
			header = current;
			
			return current.equals(expected);
			
		} catch (IOException e) {
			log.error(Arrays.toString(e.getStackTrace()));
		}
		log.info("Index input could not be created.");
		return false;
		
	}
	
	private void buildIndex() throws IOException {
		
    	Calendar cal = Calendar.getInstance();
    	long startTime = cal.getTimeInMillis();
		log.debug("Starting indexing.");
		
		FLVReader reader = new FLVReader(getFlvFile(), false, false);
		FLVHeader flvHeader = reader.getHeader();
		
		if (!flvHeader.getFlagAudio() && !flvHeader.getFlagVideo()) {
			log.debug("This is a metadata-only FLV, so we dont need an index.");
			throw new IOException("This file only contains Metadata");
		}
		
		FileImageOutputStream out = new FileImageOutputStream(indexFile);
		FLVIndexHeader header = new FLVIndexHeader();
		header.write(out); //We write a bogus header.
		header.setAudioOnly(flvHeader.getFlagAudio() && !flvHeader.getFlagVideo());

		long origPos = reader.getOffset();
		
		while (reader.hasMoreTags()) {
			
			long pos = reader.getOffset();
            ITag tmpTag = reader.readTagHeader();
            if (tmpTag == null)
            	break;
			header.setDuration(tmpTag.getTimestamp());
			
			if (tmpTag.getDataType() == IoConstants.TYPE_VIDEO) {
				
				if (header.isAudioOnly()) {
					
					log.error("Found a video frame in an audio-only file " + flvFile.getAbsoluteFile());
					header.setAudioOnly(false);
					
					out.close();
					indexFile.delete();
					out = new FileImageOutputStream(indexFile);
					
					header.write(out);
					reader.position(origPos);
					
				} else if (reader.getFrameType() == FLAG_FRAMETYPE_KEYFRAME) {
					
					out.writeLong(pos);
					out.writeInt(tmpTag.getTimestamp());
					
				}

			} else if (tmpTag.getDataType() == IoConstants.TYPE_AUDIO && header.isAudioOnly()) {
				
					out.writeLong(pos);
					out.writeInt(tmpTag.getTimestamp());
					
			}
			
			long newPosition = pos + tmpTag.getBodySize() + 15;
			
			if (newPosition >= reader.getTotalBytes()) {
				log.info("New position exceeds limit");
				if (log.isDebugEnabled()) {
					log.debug("Keyframe analysis:");
					log.debug(" data type=" + tmpTag.getDataType() + " bodysize=" + tmpTag.getBodySize());
					log.debug(" remaining=" + reader.getOffset() + " limit=" + reader.getTotalBytes() + " new pos=" + newPosition);
					log.debug(" pos=" + pos);
				}
				break;
			}		
			reader.position(newPosition);
		}
		
		header.setDate(flvFile.lastModified());
		header.setSize(flvFile.length());
		header.write(out);

		this.header = header;
		
		reader.close();
		out.close();
		
		//if (log.isDebugEnabled()) {
			cal = Calendar.getInstance();
			log.info("File index created in " + (cal.getTimeInMillis() - startTime) + " ms for " + flvFile.getAbsolutePath().toString());
		//}

	}
	
	private synchronized void requireIndex() throws IOException {
		updateIndex();
		
		if (indexBuilder != null)
			try {
				indexBuilder.join();
			} catch (InterruptedException e) {
				throw new IOException(e);
			}
		
	}
	
	/**
	 * Will update the index file if it is out of sync with the FLV file.
	 */
	public synchronized void updateIndex() {
		if (isIndexValid())
			return;
		
		if (indexBuilder == null || !indexBuilder.isAlive()) {
			indexBuilder = new IndexBuilder(this);
			indexBuilder.start();
			log.info("Starting indexing thread.");
		}

	}
	
	/**
	 * Reads the header and returns you to the same place in the file.
	 * @return a header if we could read it, or null if there were none.
	 * @throws IOException
	 */
	public FLVIndexHeader readHeader(ImageInputStream in) throws IOException {
		if (in == null)
			throw new IOException("File is not big enough to have a header!");
		if (in.length() < FLVIndexHeader.HEADER_SIZE)
			return null;
		
		FLVIndexHeader header = new FLVIndexHeader();
		
		long pos = in.getStreamPosition();
		in.seek(0);
		header.setVersion(in.readInt());
		header.setDate(in.readLong());
		header.setSize(in.readLong());
		header.setDuration(in.readLong());
		header.setAudioOnly(in.readBoolean());
		in.seek(pos);
		
		return header;
	}
	//THREAD METHODS
	
	protected File getIndexFile() {
		return this.indexFile;
	}
	protected File getFlvFile() {
		return this.flvFile;
	}
	
	protected void setStatus(Status change) {
		log.info("IndexBuilder thread changed to "+change);
		this.status = change;
	}
	
	/**
	 * Status of index file, as used by IndexBuilder.
	 * @author njoubert
	 *
	 */
	public enum Status {
		THREAD_NONE,
		THREAD_READY,
		THREAD_BUSY,
		THREAD_DONE,
		THREAD_ERROR
	}
	
	/**
	 * Thread used to build index.
	 * @author njoubert
	 */
	private class IndexBuilder extends Thread {

		private FLVIndex listener;
		
		/**
		 * Creates an IndexBuilder.
		 * @param listener - the index file object to build for and to notify of changes.
		 */
		public IndexBuilder(FLVIndex listener) {
			this.listener = listener;
			setStatus(Status.THREAD_READY);
		}
		
		private void setStatus(Status change) {
			listener.setStatus(change);
		}
		
		/**
		 * @see java.lang.Runnable#run()
		 */
		@Override
		public void run() {
			try {
				
				setStatus(Status.THREAD_BUSY);
				buildIndex();
				setStatus(Status.THREAD_DONE);
				
			} catch (IOException e) {
				log.error("Could not build index. " + e.getStackTrace());
				setStatus(Status.THREAD_ERROR);
			}
		}
		
	}
	
	//KEYFRAME
	
	/**
	 * KeyFramePair is an object that stores file position, timestamp pairs
	 */
	public class FLVKeyFramePair implements IKeyFramePair {
		
		/**
		 * Size of one keyframe in the index file  in bytes.
		 */
		public static final int KEYFRAME_SIZE = 12;
		
		private int   timestamp = -1;
		private long  position = -1;
		
		/**
		 * @return timestamp in milliseconds.
		 */
		public double getTimestamp() {
			return timestamp;
		}
		/**
		 * @return position in byte offset.
		 */
		public long getPosition() {
			return position;
		}
		/**
		 * @param timestamp Timestamp in milliseconds corresponding to byte position.
		 */
		public void setTimestamp(int timestamp) {
			this.timestamp = timestamp;
		}
		/**
		 * @param position Byte Offset in FLV corresponding to this timestamp.
		 */
		public void setPosition(long position) {
			this.position = position;
		}
	}
	
	//HEADER

	/**
	 * FLVHeader is an object that stores and represents the header in the index file.
	 */
	public class FLVIndexHeader {
		
	    private static final long bogus = 0xCAFEBABE;
	    private static final long BOGUS_HEADER = (bogus << 32) | 0x2FADFACE;
	    /**
	     * Size in Bytes of header when written to file.
	     */
	    public static final int HEADER_SIZE = 29;
	    private static final byte AUDIO_ONLY_MASK = 0x1;
	    private static final byte MP3_FILE_MASK = 0x2;
	    	
	    private int version;
	    private long date;
	    private long size;
	    private long duration;
	    private boolean audioOnly;
		
		/**
		 * Create a header with the default bogus info.
		 */
		public FLVIndexHeader() {
			this(INDEX_VERSION, BOGUS_HEADER, BOGUS_HEADER, BOGUS_HEADER, false);
		}
		/**
		 * Creates a header with the given parameters.
		 * @param version 
		 * @param lastModified - 
		 * @param size
		 * @param duration
		 * @param audioOnly
		 */
		public FLVIndexHeader(int version, long lastModified, long size, long duration, boolean audioOnly) {
			this.version = version;
			this.date = lastModified;
			this.size = size;
			this.duration = duration;
			this.audioOnly = audioOnly;
		}
		
		/**
		 * Writes the current header to the beginning of the given stream.
		 * @param out - stream to write to.
		 * @throws IOException - if something goes wrong
		 */
		public void write(FileImageOutputStream out) throws IOException {
			out.seek(0);
			out.writeInt(getVersion());
			out.writeLong(getLastModified());
			out.writeLong(getSize());
			out.writeLong(getDuration());
			out.writeBoolean(isAudioOnly());
		}
		
		/** Tests for equality between two IndexHeaders
		 * @param o
		 * @return true if equal.
		 */
		public boolean equals(FLVIndexHeader o) {
			if (o.version == this.version &&
					o.date == this.date &&
					o.size == this.size &&
					o.duration == this.duration &&
					o.audioOnly == this.audioOnly)
				return true;
			return false;
		}

		public int getVersion() {
			return version;
		}

		public void setVersion(int version) {
			this.version = version;
		}

		public long getLastModified() {
			return date;
		}

		public void setDate(long date) {
			this.date = date;
		}

		public long getSize() {
			return size;
		}

		public void setSize(long size) {
			this.size = size;
		}

		public long getDuration() {
			return duration;
		}

		public void setDuration(long duration) {
			this.duration = duration;
		}

		public boolean isAudioOnly() {
			return audioOnly;
		}

		public void setAudioOnly(boolean audioOnly) {
			this.audioOnly = audioOnly;
		}
		
	}



	
}
