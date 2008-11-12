package org.red5.server.webapp.webzyncApp.event;

import org.red5.server.webapp.webzyncApp.VideoDaemon;
import org.red5.server.webapp.webzyncApp.VideoDaemon.Status;
import org.red5.server.webapp.webzyncApp.VideoDaemon.Video;

public class VideoEvent {

	private VideoDaemon.Video video;
	private VideoDaemon.Status status;
	private long downloadedBytes = 0;
	private long fileSize = -1;
	
	public VideoEvent(Video video, Status status) {
		this.video = video;
		this.status = status;
	}
	
	
	public VideoEvent(Video video, Status status, long downloadedBytes, long fileSize) {
		this.video = video;
		this.status = status;
		this.downloadedBytes = downloadedBytes;
		this.fileSize = fileSize;
	}


	public long getDownloadedBytes() {
		return downloadedBytes;
	}


	public long getFileSize() {
		return fileSize;
	}


	public VideoDaemon.Status getStatus() {
		return status;
	}
	public VideoDaemon.Video getVideo() {
		return video;
	}
	
}
