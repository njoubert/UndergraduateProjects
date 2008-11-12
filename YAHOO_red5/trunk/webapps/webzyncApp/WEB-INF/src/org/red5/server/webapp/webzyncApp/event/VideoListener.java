package org.red5.server.webapp.webzyncApp.event;

public interface VideoListener {

	public void statusChanged(VideoEvent event);
	public void errorOccurred(VideoEvent event);
	public void transferredData(VideoEvent event);
	
}
