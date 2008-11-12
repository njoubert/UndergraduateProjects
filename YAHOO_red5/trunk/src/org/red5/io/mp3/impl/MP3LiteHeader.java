package org.red5.io.mp3.impl;

/*
 * RED5 Open Source Flash Server - http://www.osflash.org/red5
 * 
 * Copyright (c) 2006-2007 by respective authors (see below). All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free Software 
 * Foundation; either version 2.1 of the License, or (at your option) any later 
 * version. 
 * 
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along 
 * with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 */

/**
 * Header of a MP3 frame.
 * This is only a package for variables, no logic! Everyhing happens in MP3Reader.
 * 
 * @author The Red5 Project (red5@osflash.org)
 * @author Niels Joubert, Yahoo! Research Berkeley (njoubert@yahoo-inc.com)
 * @see <a href="http://mpgedit.org/mpgedit/mpeg_format/mpeghdr.htm">File format</a>
 */

public class MP3LiteHeader {
	/**
     * Frame sync data
     */
    public int data;

    /**
     * Protection bit
     */
    public boolean protectionBit;

    /**
     * Bitrate used (index in array of bitrates)
     */
    public long bitRate;

    /**
     * Sampling rate used (index in array of sample rates)
     */
    public long samplingRate;
    
    public double duration;
    
    public long timestamp;
    
    public int size;
    
    public long channels;

    /**
     * Creates MP3 header from frame sync value
     */
    public MP3LiteHeader() {
		
	}

	/**
     * Getter for frame sync word data
     *
     * @return  Frame sync word data
     */
    public int getData() {
		return data;
	}

	/**
     * Whether stereo playback mode is used
     *
     * @return  <code>true</code> if stereo mode is used, <code>false</code> otherwise
     */
    public boolean isStereo() {
		return (channels == 2);
	}

	/**
     * Whether MP3 has protection bit
     *
     * @return  <code>true</code> if MP3 has protection bit, <code>false</code> otherwise
     */
    public boolean isProtected() {
		return protectionBit;
	}

	/**
     * Getter for bitrate
     *
     * @return  File bitrate
     */
    public int getBitRate() {
		return (int) bitRate;
	}

	/**
     * Getter for sample rate
     *
     * @return  Sampling rate
     */
    public int getSampleRate() {
		return (int) samplingRate;
	}

	/**
	 * Calculate the size of a MP3 frame for this header.
	 * 
	 * @return size of the frame including the header
	 */
	public int frameSize() {
		return size;
	}

	/**
	 * Return the duration of the frame for this header.
	 * 
	 * @return The duration in milliseconds
	 */
	public double frameDuration() {
		return duration;
	}

}
