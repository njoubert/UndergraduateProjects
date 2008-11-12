package org.red5.io.composite.impl;

import java.net.MalformedURLException;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.red5.io.ITagReader;
import org.red5.io.flv.impl.FLVReader;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;

import org.red5.server.GlobalScope;
import org.red5.server.Scope;
import org.red5.server.WebScope;
import org.red5.server.api.IBasicScope;
import org.red5.server.api.IContext;
import org.red5.server.api.IScope;
import org.red5.server.api.ScopeUtils;
import org.red5.server.stream.IProviderService;
import org.red5.server.stream.provider.FileProvider;

import java.net.HttpURLConnection;
import java.net.URL;
import java.net.MalformedURLException;
import java.net.URLConnection;
import java.util.regex.*;

/**
 * This class provided services to go from an application path to an URL
 * on the TimeTags server, fetch that URL, and interpret an URL into a file
 * on our Red5 app's disk. Thus it provides all the functionality needed to 
 * integrate into the REST-based API and give us playlisting capability.
 * @author njoubert
 *
 */
public class UrlResolver {
	
	protected static Log log = LogFactory.getLog(UrlResolver.class.getName());
	
	/**
	 * The location of the REST API root call.
	 */
	private static String xmlServer;
	
	
	/**
	 * The RELATIVE path from the xmlServer root to the playlist call.
	 */
	private static String xmlPlaylistService;
	
	/**
	 * The last scope in the scope hierarchy that indicates
	 * a dynamic XML call.
	 */
	private static String scopeName;
	
	/**
	 * The regular expression String used to extract the parameters of the XML call.
	 */
	private static String playlistRegExp;
	
	public InputStream getXML(URL url) {
		try {
			if (log.isDebugEnabled())
				log.debug("Opening connection to URL " + url.toString());
		    HttpURLConnection connection = (HttpURLConnection) url.openConnection();
		    //BufferedReader d = new BufferedReader(new InputStreamReader(connection.getInputStream()));
		    return connection.getInputStream();
		    
		} catch (IOException e) {
			log.error("Error in pulling XML from server!");
			log.error(e);
			return null;
		}
	}
	
	/**
	 * We resolve a URL (from the XML Service) to a path in our current application.
	 * It is important to note that we completely ignore application from the incoming
	 * URL, but we expect the first part of the path to be the application, and strip it off.
	 * We then build up a custom scope from the rest of the path, and hand it to the filename generator
	 * defined for the application in which the query was called, so that the path is built up according to the
	 * FilenameGenerator defined for the app in which the query was called.
	 * @param urlString - we expect this to be "<protocol>://host<:post>/app/..."
	 * @return
	 */
	public static ITagReader resolveUrl(IScope scope, String urlString) {
		IContext context = scope.getContext();
		IProviderService providerService = (IProviderService) context
				.getBean(IProviderService.BEAN_NAME);
		
		//First we strip off the protocol://host<:port>/ part
		String protocolEnd = "//";
		int i = urlString.indexOf(protocolEnd);
		if (i != -1) {	//Does the address start with protocol://
			urlString = urlString.substring(urlString.indexOf('/', i + protocolEnd.length()) + 1);
		} else {
			urlString = urlString.substring(urlString.indexOf('/') + 1);
			
		}
		//Strip off app (doesnt do anything if there's no path left)
		urlString = urlString.substring(urlString.indexOf('/') + 1);
		try {
			
		//At the moment we go back to the most basic scope...
			scope = ScopeUtils.findApplication(scope);
			//And build up a custom scope hierarchy
			int j = urlString.indexOf('/');
			while (j != -1) {
				String scopeName = urlString.substring(0, j);
				Scope child = new Scope(scopeName);
				child.setParent(scope);
				scope.addChildScope(child);
				scope = child;
				urlString = urlString.substring(j+1);
				j = urlString.indexOf('/');
			}
			
			//Then we use the providerService to get a File and hand it to FileProvider
			File VODFile = providerService.getVODProviderFile(scope, urlString);
			
			if (VODFile == null) {
				log.error("Could not resolve URL to file on disk... ");
				return null;
			}
				
			
		//Then we get a reader from the FileProvider.
			FileProvider provider = new FileProvider(scope, VODFile);
		
			return provider.getReader();
		} catch (MalformedURLException e) {
			log.error("The URL provided was malformed! ", e);
			return null;
		} catch (IOException e) {
			
			log.error("Could not create reader! ", e);
			return null;
			
		}
	}
	
	/**
	 * Here we create an URL that returns the playlist segments to create this playlist.
	 * We also check the validity of the call being made.
	 * @param scope
	 * @param name
	 * @return
	 */
	public URL getXMLPlaylistUrl(IScope scope, String name) {
		//Check for a valid playlist URL:
		String id;
		String n = scope.getName();
		if (n.compareTo(scopeName) != 0)
			return null;
		
		Pattern p = Pattern.compile(playlistRegExp);
		Matcher m = p.matcher(name);
		if (m.matches()) {
			id = m.group(2);
			try {
				Integer.parseInt(id);	//We make sure that this is an integer!
			} catch (NumberFormatException e) {
				return null;
			}
		} else 
			return null;
		
		String urlS = xmlServer + xmlPlaylistService + id;
		
		try {
			URL playlistUrl = new URL(urlS);
			return playlistUrl;
		} catch (MalformedURLException e) {
			log.error("Could not create URL for XML server... ");
			log.error(e);
			return null;
		}
	}
	
	public void setXmlServer(String xml) {
		this.xmlServer = xml;
	}
	public void setXmlPlaylistService(String xml) {
		this.xmlPlaylistService = xml;
	}
	public void setScopeName(String xml) {
		this.scopeName = xml;
	}
	public void setPlaylistRegExp(String xml) {
		this.playlistRegExp = xml;
	}
	
}
