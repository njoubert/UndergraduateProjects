package org.red5.io;

import java.io.IOException;


/**
 * Gets thrown if an FLV is incomplete. Thus the reader can
 * play the file, but only to a point, and the last frame is incomplete.
 * This exception can occur specifically when a file being downloaded is
 * opened.
 * 
 * @author njoubert
 *
 */

public class IncompleteFileException extends IOException {
	
	private static final long serialVersionUID = 1L;
	
	private ITagReader reader;
	
	/**
	 * 
	 * @param reader
	 * @param message
	 */
	public IncompleteFileException(ITagReader reader, String message) {
		this.reader = reader;
	}

	/**
	 * @return The damaged FLV Reader that 
	 */
	public ITagReader getReader() {
		return reader;
	}

	/**
	 * @param reader - the TagReader to set as this error's broken reader.
	 */
	public void setReader(ITagReader reader) {
		this.reader = reader;
	}
	
}
