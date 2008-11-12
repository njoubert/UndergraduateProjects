package org.red5.io.index;

/**
 * The interface that classes expect for working with indexing pairs
 * from and Index file. Implemented by helper classes to pass around 
 * information that the Index file stores.
 * 
 * @author njoubert
 *
 */
public interface IKeyFramePair {

	/**
	 * @return Position (Byte Offset) in bytes
	 */
	public long getPosition();
	
	/**
	 * @return Timestamp in milliseconds
	 */
	public double getTimestamp();
	
}
