package org.red5.server.webapp.webzyncApp;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.apache.commons.codec.digest.DigestUtils;
import org.red5.server.webapp.webzyncApp.event.VideoEvent;
import org.red5.server.webapp.webzyncApp.event.VideoListener;

public class VideoDaemon {

	/**
	 * Constructs a new VideoDaemon
	 * 
	 * @param storeDirectory
	 */
	public VideoDaemon(File storeDirectory) {
		this.storeDirectory = storeDirectory;
	}
	
	private File storeDirectory;
	private Map<String,Video> videos = new HashMap<String,Video>();
	
	/**
	 * Returns a Video object based on site URL given by user.  If this video
	 * is already loaded, you can access it immediately, otherwise you will need
	 * to add listeners and call "load"
	 * @param siteURL
	 * @return
	 */
	public Video getVideo(String siteURL) {
		Video video = videos.get(siteURL);
		if (video == null) {
			video = new Video(siteURL);
			videos.put(siteURL,video);
		}
		return video;
	}
	
	

	/**
	 * 
	 * 
	 */
	public enum Status {
		NOTSTARTED,
		INIT,
		GETTING_URL,
		TRANSFERRING,
		TRANSCODING,
		GENERATING_INDEX,
		DONE,
		ERROR
	};
	
	public class Video implements Runnable {
		public Video(String siteURL) {
			this.siteURL = siteURL;
		}
		
		private List<VideoListener> listeners = new LinkedList<VideoListener>();
		
		public void addVideoListener(VideoListener listener) {
			synchronized (listeners) {
				listeners.add(listener);
			}
		}
		public void removeVideoListner(VideoListener listener) {
			synchronized (listeners) {
				listeners.remove(listener);
			}
		}
		
		public void run() {
			try {
				setStatus(Status.GETTING_URL);
				String flvURL = VideoUtils.getVideoURL(new URL(siteURL));
				fileName = "offsite/"+DigestUtils.md5Hex(siteURL);
				if (!fileName.endsWith(".flv"))
					fileName += ".flv";

				setStatus(Status.TRANSFERRING);
				URLConnection conn = new URL(flvURL).openConnection();
				conn.connect();
				
				fileSize = conn.getContentLength();
				
				file = new File(storeDirectory.getAbsolutePath()+"/"+fileName);
				if (!file.exists() || !file.canRead() || file.length() < fileSize) {
					InputStream is = conn.getInputStream();
					FileOutputStream os = new FileOutputStream(file);
					
					byte bytes[] = new byte[1024 * 100];
					int i;
					while ((i = is.read(bytes))>=0) {
						os.write(bytes,0,i);
						downloadedBytes += i;
						VideoEvent e = getEvent();
						synchronized (listeners) {
							for (VideoListener l : listeners)
								l.transferredData(e);
						}
					}
					is.close();
					os.close();
				}
				setStatus(Status.DONE);
			} catch (IOException ex) {
				status = Status.ERROR;
				VideoEvent e = getEvent();

				synchronized (listeners) {
					for (VideoListener l : listeners)
						l.errorOccurred(e);
				}
			}
		}
		
		/**
		 * Starts the video loading if necessary.  (
		 */
		public void load() {
			if (status==Status.NOTSTARTED) {
				setStatus(Status.INIT);
				Thread t = new Thread(this);
				t.start();
			}
		}
		
		private void setStatus(Status status) {
			this.status = status;
			VideoEvent e = getEvent();
			for (VideoListener l : listeners)
				l.statusChanged(e);
		}
		private VideoEvent getEvent() {
			return new VideoEvent(this,status,downloadedBytes,fileSize);
		}
		private String siteURL;
		private Status status = Status.NOTSTARTED;
		private File file;
		private String fileName;
		
		private long fileSize = -1;
		private long downloadedBytes = 0;
		
		public String getFileName() {
			return fileName;
		}
		public File getFile() {
			return file;
		}
		public String getSiteURL() {
			return siteURL;
		}
		
		public long getFileSize() {
			return fileSize;
		}
		public long getDownloadedBytes() {
			return downloadedBytes;
		}
		
		public boolean canPlay() {
			return isLoaded() || downloadedBytes>102400;
		}
		public boolean isLoading() {
			return false;// status.isLoading();
		}
		public boolean isLoaded() {
			return status == Status.DONE;
		}
		public Status getStatus() {
			return status;
		}

		public String toString() {
			return "Video["+getSiteURL()+"]";
		}
	}
}
