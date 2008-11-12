package org.red5.server.webapp.webzyncApp;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.httpclient.util.URIUtil;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.red5.io.flv.impl.FLVReader;
import org.red5.io.mp3.impl.MP3Reader;
import org.red5.server.api.IScope;
import org.red5.server.api.Red5;
import org.springframework.core.io.Resource;
/**
 * Handles all on-demand based video backend for WebZync.
 * @author njoubert, mbastaef
 *
 */
public class VideoUtils {

	protected static Log log = LogFactory.getLog(Application.class.getName());
    
	/**
	 * @return List of available movies
	 */
    public static List<Map<String, Object>> getMovies(Room room) {
		IScope scope = Red5.getConnectionLocal().getScope();
		List<Map<String, Object>> movies = new ArrayList<Map<String, Object>>();
		
		try {
			log.debug("getting the FLV files");
			String path = "streams/*.flv";
			Resource[] flvs = scope.getResources(path);
			if (flvs != null) {
				for (Resource flv : flvs) {
					try {
						FLVReader reader = new FLVReader(flv.getFile());
						movies.add(new MediaState(flv.getFile().getName(), reader.getDuration()).getMap(false));
					} catch (Exception ex) {
						ex.printStackTrace();
					}
				}
			}
		} catch (IOException e) {
			log.error(e);
		}
		if (log.isDebugEnabled())
			log.debug("movies="+movies);
		
		Collections.sort(movies, new Comparator<Map<String, Object>>() {
			@Override
			public int compare(Map<String, Object> arg0,
					Map<String, Object> arg1) {
				
				return ((String) arg0.get("name")).compareTo((String) arg1.get("name"));
			} });
		
		return movies;
	}
    
	/**
	 * @return List of available mp3s
	 */
    public static List<Map<String, Object>> getMp3s(Room room) {
		IScope scope = Red5.getConnectionLocal().getScope();
		List<Map<String, Object>> music = new ArrayList<Map<String, Object>>();
		
		try {
			log.debug("getting the MP3files");
			String path = "streams/*.mp3";
			Resource[] mp3s = scope.getResources(path);
			if (mp3s != null) {
				for (Resource mp3 : mp3s) {
					MP3Reader reader = new MP3Reader(mp3.getFile());
					music.add(new MediaState(mp3.getFile().getName(), reader.getDuration()).getMap(false));
				}
			}
		} catch (IOException e) {
			log.error(e);
		}
		if (log.isDebugEnabled())
			log.debug("mp3s="+music);

		Collections.sort(music, new Comparator<Map<String, Object>>() {
			@Override
			public int compare(Map<String, Object> arg0,
					Map<String, Object> arg1) {
				
				return ((String) arg0.get("name")).compareTo((String) arg1.get("name"));			} });
		
		return music;
	}
    
    /**
     * Pulls the given url's text.
     * @param url
     * @return String of HTML data
     * @throws IOException
     */
	public static String getURLContent(URL url) throws IOException {

		URLConnection conn = url.openConnection();
		
		InputStream is = conn.getInputStream();
		
		Reader reader = new InputStreamReader(is);
		
		StringBuffer buff = new StringBuffer();

		char buffer[] = new char[4096];
		while (true) {
			int length = reader.read(buffer,0,4096);
			if (length<0)
				return buff.toString();
			buff.append(buffer,0,length);
		}
	}
	/**
	 * @param s
	 * @return s.
	 */
	public static String unescape(String s) {
		return s;
	}
	/**
	 * @param id
	 * @param siteContents
	 * @return Youtube video url
	 */
	public static String getVideoURL(String id, String siteContents) {
		 // try youtube:
		 if (id.indexOf("yt:") == 0) {
			 Pattern p = Pattern.compile("(video_id=[^\"&]+)");
			 Matcher m = p.matcher(siteContents);
			 
			 if (m.find())
				 return "http://youtube.com/get_video.php?" + m.group(1);
		 }
		 if (id.indexOf("gv:") == 0) {
		     String r = siteContents;
		     String g = "googleplayer.swf?&videoUrl";     
		     String doc = r.substring(r.indexOf(g) + g.length());
		     doc = doc.substring(doc.indexOf("http"));
		     doc = doc.substring(0, doc.indexOf("http", 5) - 6);
		     // if (isIt()) alert(unescape(doc));
		     return unescape(doc);
		 }
		 if (id.indexOf("blip:") == 0) {
		     String r = siteContents;
		     String g = "setPrimaryMediaUrl(\"";
		     String doc = r.substring(r.indexOf(g) + g.length());
		     doc = doc.substring(0, doc.indexOf("\")"));
		     return doc;
		 }
		 return null;
	}
	
	/**
	 * @param u
	 * @return the video url inside this page.
	 * @throws IOException
	 */
	public static String getVideoURL(URL u) throws IOException {
		
		return getURLContent(new URL("http://cs2.berkeley.corp.yahoo.com/webzync/keepvid.php?url="+URIUtil.encodeQuery(u.toString())));

	}
	
	/**
	 * Test!
	 * @param args
	 * @throws Exception
	 */
	public static void main(String args[]) throws Exception {
		URL u = new URL("http://youtube.com/watch?v=nr9JxCGWIAM");
		
		String contents = getURLContent(u);
		
		System.out.println("result = "+getVideoURL(u));
		
		
	}
}
