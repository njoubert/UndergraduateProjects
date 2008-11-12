package org.red5.server.net.rtmp;

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

import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.util.ArrayList;
import java.util.Collections;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.red5.server.net.rtmp.message.Packet;

public class RTMPMinaConnection extends RTMPConnection {
    /**
     * Logger
     */
	protected static Log log = LogFactory.getLog(RTMPMinaConnection.class
			.getName());

    /**
     * MINA I/O session, connection between two endpoints
     */
    private IoSession ioSession;

	/** Constructs a new RTMPMinaConnection. */
    RTMPMinaConnection() {
		super(PERSISTENT);
	}

	/**
     * Setter for MINA I/O session (connection)
     *
     * @param protocolSession  Protocol session
     */
    void setIoSession(IoSession protocolSession) {
		SocketAddress remote = protocolSession.getRemoteAddress();
		if (remote instanceof InetSocketAddress) {
			remoteAddress = ((InetSocketAddress) remote).getAddress()
					.getHostAddress();
			remotePort = ((InetSocketAddress) remote).getPort();
		} else {
			remoteAddress = remote.toString();
			remotePort = -1;
		}
		remoteAddresses = new ArrayList<String>();
		remoteAddresses.add(remoteAddress);
		remoteAddresses = Collections.unmodifiableList(remoteAddresses);
		this.ioSession = protocolSession;
	}
	
	/**
     * Return MINA I/O session
     *
     * @return MINA O/I session, connection between two endpoints
     */
    public IoSession getIoSession() {
		return ioSession;
	}

	/** {@inheritDoc} */
    @Override
	public void rawWrite(ByteBuffer out) {
		ioSession.write(out);
	}

	/** {@inheritDoc} */
    @Override
	public void write(Packet out) {
		writingMessage(out);
		ioSession.write(out);
	}

	/** {@inheritDoc} */
    @Override
	public boolean isConnected() {
		return super.isConnected() && ioSession.isConnected();
	}

	/** {@inheritDoc} */
    @Override
	public long getReadBytes() {
		return ioSession.getReadBytes();
	}

	/** {@inheritDoc} */
    @Override
	public long getWrittenBytes() {
		return ioSession.getWrittenBytes();
	}

	/** {@inheritDoc} */
    @Override
	public long getPendingMessages() {
		return ioSession.getScheduledWriteRequests();
	}

	/** {@inheritDoc} */
    @Override
	public void close() {
		super.close();
		ioSession.close();
	}
	
	/** {@inheritDoc} */
    @Override
	protected void onInactive() {
		this.close();
	}
}
