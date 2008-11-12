package org.red5.io.index;

import java.io.File;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.HashMap;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.red5.io.IStreamableFile;
import org.red5.io.ITagReader;

/**
 * A singleton class that takes care of index files
 * for Readers that require indexing for seeking, duration
 * calculation, etc.
 * @author njoubert
 *
 */
public class IndexManager {

	private final static IndexManager REF = new IndexManager();
	private HashMap<File, WeakReference<IIndex>> map;
	private static Log log = LogFactory.getLog(IndexManager.class.getName());
	
	private IndexManager() {
		this.map = new HashMap<File, WeakReference<IIndex>>();
	}
	
	/**
	 * @return The IndexManager reference.
	 */
	public static IndexManager getInstance() {
		return REF;
	}
	
	/**
	 * @throws CloneNotSupportedException if this object is cloned.
	*/
	@Override
	public Object clone() throws CloneNotSupportedException {
		throw new CloneNotSupportedException();
	}
	
	/**
	 * This returns the Index object for some reader, so that
	 * the calling reader can have a reference to the object
	 * that represents the index of the file it represents.
	 * @param mediaFile
	 * @return Index object for the mediaFile
	 */
	public synchronized IIndex getIndex(ITagReader mediaFile) {
		File file = mediaFile.getFile();
		
		if (file == null)
			return null;
		
		WeakReference<IIndex> indexRef = map.get(file);
		
		if (indexRef == null || indexRef.get() == null) {
			try {
				
				IIndex index = mediaFile.getIndex();
				map.put(file, new WeakReference<IIndex>(index));
				log.debug("Index file instantiated for " + file.getAbsolutePath());
				return index;
				
			} catch (IOException e) {
				
				log.error("Could not instantiate Index object for " + mediaFile);
				log.error(e);
				
			}
			return null;
		}
		log.debug("Index already existed and returned for " + file.getAbsolutePath());
		return indexRef.get();	
	}
}
