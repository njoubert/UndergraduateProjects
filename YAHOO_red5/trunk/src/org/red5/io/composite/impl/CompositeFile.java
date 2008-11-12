package org.red5.io.composite.impl;

import java.io.File;
import java.io.IOException;
import java.net.URL;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.red5.io.ITagReader;
import org.red5.io.ITagWriter;
import org.red5.io.composite.ICompositeFile;
import org.red5.io.flv.impl.FLV;
import org.red5.io.composite.impl.UrlResolver;
import org.red5.server.api.IScope;

/**
 * CompositeFiles are an abstraction that makes a group of clips, 
 * made up of in and out points and files on disk, appear as a single, 
 * streamable FLV. The structure of a single composite file is built
 * from a database query into the TimeTags API's query, and Red5
 * parses this according to a specific naming convention in paths.
 * 
 * <h2> To access a composite file </h2>
 * <br>
 * The URL that Red5 will respond to to create a composite file 
 * looks like this:
 * <br>
 * <code> http://server/app/dynamic/query </code>
 * <br>
 * where query has to contain at least:
 * <br>
 * <code> playlist_id=id </code>
 * <br>
 * where <code>id</code> corresponds to a database id for a specific playlist.
 * 
 * Red5 will then pull the XML from the server defined in red5-common.xml under /conf and
 * parse the XML structure into Readers.
 * 
 * <h2>Defining a XML Server</h2>
 * The XML server has to correspond to the TimeTags API, at this time in Version 3.
 * The red5-common.xml config file holds the following keys and information:
 * <br>
 * <ul>
 * 	<li><b>xmlServer</b> - The root of the XML server to call. http://cs2.berkeley.corp.yahoo.com:85/</li>
 *  <li><b>xmlPlaylistService</b> - The path to the playlist service, including the query. eg: apis/timetags/V3/segments?playlist=</li>
 *  <li><b>scopeName</b> - The subdirectory inside the scope we expect for dynamic calls. eg: dynamic</li>
 *  <li><b>playlistRegExp</b> - The regular expression used to parse the incoming query to a playlist_id. </li>
 * </ul>
 * It is important to note that the Id that you pass will simply be tacked
 * onto the end of xmlPlaylistService.
 * <br>Also, the playlistRegExp uses the seconds (2nd) capture group as the id for the playlist to call.
 * 
 * @author njoubert
 *
 */
public class CompositeFile implements ICompositeFile {

	protected static Log log = LogFactory.getLog(FLV.class.getName());
	
	private File file;
	
	private URL url;
	
	private UrlResolver urlResolver;
	
	public CompositeFile(File file) {
		this.file = file;
	}
	
	public CompositeFile(URL url, UrlResolver resolver) {
		this.url = url;
		this.urlResolver = resolver;
	}
	
	public ITagWriter getAppendWriter() throws IOException {
		// TODO Auto-generated method stub
		throw new UnsupportedOperationException();
	}

	public ITagReader getReader() throws IOException {
		return getReader(null);
	}
	
	public ITagReader getReader(IScope scope) throws IOException {
		if (scope == null) {
			log.error("We do not support scope-less CompositeFileReaders!");
			return null;
		}
		CompositeFileReader reader;
		if (file == null && url != null)
			reader = new CompositeFileReader(url, urlResolver, scope);
		else
			reader = new CompositeFileReader(file, scope);
		return reader;
	}

	public ITagWriter getWriter() throws IOException {
		// TODO Auto-generated method stub
		throw new UnsupportedOperationException();
	}

}
