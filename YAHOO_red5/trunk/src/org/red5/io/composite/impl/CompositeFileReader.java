package org.red5.io.composite.impl;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.mina.common.ByteBuffer;
import org.red5.io.ISeekableFile;
import org.red5.io.IStreamableFile;
import org.red5.io.ITag;
import org.red5.io.ITagReader;
import org.red5.io.IoConstants;
import org.red5.server.api.IScope;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.net.URL;

import org.red5.io.amf.Output;
import org.red5.io.flv.impl.Tag;
import org.red5.io.index.IIndex;
import org.red5.io.index.IKeyFramePair;
import org.red5.io.object.Serializer;

import java.io.File;
import java.io.IOException;
import org.red5.io.composite.impl.UrlResolver;

public class CompositeFileReader implements IoConstants, ITagReader, ISeekableFile {
	
	protected static Log log = LogFactory.getLog(CompositeFileReader.class.getName());

	private File file;
	
	private URL xmlSource;
	
	private UrlResolver urlResolver;
	
	private CompositeElement currentReader;
	
	private int index = 0;
	
	private List<CompositeElement> elements;
	
	private long duration = 0;
	
	private int tagPosition = 0;
	
	private IScope scope;
	
	private boolean generateMetadata = true;
	
	public CompositeFileReader(URL xmlSource, UrlResolver resolver, IScope scope) throws IOException {
		this.xmlSource = xmlSource;
		this.urlResolver = resolver;
		this.scope = scope;
		index = 0;
		init();
	}
	
	public CompositeFileReader(File file,IScope scope) throws IOException {
		this.file = file;
		this.scope = scope;
		index = 0;
		init();
	}
	
	synchronized private void init() {
		ArrayList<CompositeElement> elements = new ArrayList<CompositeElement>();
		CompositeMapCreator mapCreator = new CompositeMapCreator(urlResolver, scope);
		if (file == null && xmlSource != null) {
			if (!mapCreator.readElementsFromURL(xmlSource, elements) && elements.size() < 1) {
				log.error("Tried to read a playlist from the server and got 0 elements");
				return;
			}
		} else {
			if (!mapCreator.readElementsFile(file, elements) && elements.size() < 1) {
				log.error("Tried to read a composite element file and got 0 elements");
				return;
			}
		}
		index = 0;
		currentReader = elements.get(index);
		this.elements = elements;
		duration = 0;
		for (CompositeElement element : elements) 
			duration += (element.getOutPointTime() - element.getInPointTime());
		
		if (duration < 0)
			log.error("Misformed XML or negative duration in XML file...");
		
		if (log.isDebugEnabled())
			log.debug("Instantiated CompositeFileReader. Imported " + elements.size() + " Elements. Found duration to be " + duration / 1000.0 + " seconds");
	}
	
	synchronized private void nextReader() {

		index += 1;
		if (index < elements.size()) {
			currentReader = elements.get(index);
			currentReader.reset();
			if (log.isDebugEnabled())
				log.debug("Flipping from reader " + (index - 1) + " to " + index );
		} else
			log.error("Tried to move to the next reader when we were already at the last reader");
	}
	
	/**
	 * Sets the given reader (at the given index position) to be the current reader.
	 * @param rd - reader to become current reader
	 * @param i - index of reader
	 */
	synchronized private void setReader(CompositeElement rd, int i) {
		if (log.isDebugEnabled())
			log.debug("Setting reader to element " + i + " with in-point " + 
					(rd.getInPointTime() + rd.getCorrection()) + " and out-point " + (rd.getOutPointTime() + rd.getCorrection()) + 
					" that corresponds to wanted timeslot.");
		index = i;
		currentReader = rd;
		rd.reset();
	}

	/**
     * @return Return length in seconds
     */
	public double getDuration() {
		return duration / 1000.0;
	}

	/**
	 * Decode the header of the stream;
	 *
	 */
	public void decodeHeader() throws IOException {
		for (CompositeElement element : elements)
			element.decodeHeader();
	}


	/**
	 * Returns a boolean stating whether the CompositeFile has more tags
	 * @return boolean
	 */
	synchronized public boolean hasMoreTags() {
		boolean indices = ((index + 1) < elements.size());
		boolean rd = currentReader.hasMoreTags();
		return  indices || rd;
	}

    /**
     * Create tag for metadata event
     * @return         Metadata event tag
     */
    private ITag createMeta() {
		// Create tag for onMetaData event
		ByteBuffer buf = ByteBuffer.allocate(1024);
		buf.setAutoExpand(true);
		Output out = new Output(buf);

        // Duration property
		out.writeString("onMetaData");
		Map<Object, Object> props = new HashMap<Object, Object>();
		props.put("duration", duration / 1000.0);
		
		// TODO: Somehow send out the correct videocodecid and audiocodecid
		// See FLVReader
		
/*		((FLVReader) currentReader.reader).
		if (firstVideoTag != -1) {
			long old = getCurrentPosition();
			setCurrentPosition(firstVideoTag);
			readTagHeader();
			fillBuffer(1);
			byte frametype = in.get();
            // Video codec id
			props.put("videocodecid", frametype & MASK_VIDEO_CODEC);
			setCurrentPosition(old);
		}
		if (firstAudioTag != -1) {
			long old = getCurrentPosition();
			setCurrentPosition(firstAudioTag);
			readTagHeader();
			fillBuffer(1);
			byte frametype = in.get();
            // Audio codec id
            props.put("audiocodecid", (frametype & MASK_SOUND_FORMAT) >> 4);
			setCurrentPosition(old);
		}*/
		props.put("canSeekToEnd", true);
		out.writeMap(props, new Serializer());
		buf.flip();

		ITag result = new Tag(IoConstants.TYPE_METADATA, 0, buf.limit(), null,
				0);
		result.setBody(buf);
		return result;
	}
    
	/**
	 * Returns a Tag object
	 * 
	 * @return Tag
	 */
	synchronized public ITag readTag() throws IOException {
		
		if (tagPosition == 0 && generateMetadata) {
			tagPosition++;
			return createMeta();
		} 
		
		if (currentReader.hasMoreTags()) {
			tagPosition++;
			return currentReader.readTag();
		} else if (index < elements.size()) {
			nextReader();
			tagPosition++;
			return currentReader.readTag();
		} else {
			return null;
		}
	}

	
	/**
	 * Check if the reader also has video tags.
	 * @return
	 */
	public boolean hasVideo() {
		//TODO: At this point in time we only support video playlists, this should change in the future!
		return true;
	}
	
	/**
	 * Sets file reader's position to the closest timestamp to given ts in milliseconds
	 * 
	 * @param ts - timestamp to seek to (milliseconds)
	 * @return timestamp seeked to.
	 */
	public synchronized int seekToTime(int ts) {
		if (log.isDebugEnabled())
			log.debug("SEEK to Timestamp " + ts + " recieved.");
		
		if (ts < 0 || ts > this.getDuration() * 1000.0) {
			ts = 0;
			log.error("Seek to negative timestamp rewritten to zero...");
		}

		CompositeElement el = null;
		int i = index;
		
		if (currentReader.getOutPointTime() + currentReader.getCorrection() < ts) {
			//Seek forward
			if (log.isDebugEnabled())
				log.debug("Seeking forward through readers.");
			
			for (i = index + 1; i < elements.size(); i++) {
				el = elements.get(i);
				if ((el.getOutPointTime() + el.getCorrection()) > ts) {
					setReader(el, i);
					return el.seekToTime(ts);
				}
			}
			
		} else if (currentReader.getInPointTime() + currentReader.getCorrection() > ts) {
			//Seek backwards
			if (log.isDebugEnabled())
				log.debug("Seeking backwards through readers.");
			
			for (i = index; i >= 0; i--) {
				el = elements.get(i);
				if ((el.getInPointTime() + el.getCorrection()) < ts) {
					setReader(el, i);
					return el.seekToTime(ts);
				}
			}
			
		} else {
			//Seeking within current reader.
			if (log.isDebugEnabled())
				log.debug("No seeking through readers needed, seek within current reader only!");		
			
			return currentReader.seekToTime(ts);
		}

		return 0;
	}
	
	public void position(long pos) throws IOException {
		log.error("Setting the position of a CompositeFile is not implemented...");
	}
	
	public IStreamableFile getStreamableFile() {
		return null;
	}

	public long getOffset() {
		return (int) getBytesRead();
	}

	public long getBytesRead() {
		return 0L;
	}

	public void close() {
		for (CompositeElement el : elements)
			el.close();
		
	}
	
	public IKeyFramePair getIndex(double ts) { throw new UnsupportedOperationException(); }
	public IKeyFramePair getIndex(long pos) { throw new UnsupportedOperationException(); }

	public IIndex getIndex() {
		throw new UnsupportedOperationException();
	}

	@Override
	public File getFile() {
		// TODO Auto-generated method stub
		return null;
	}
}
