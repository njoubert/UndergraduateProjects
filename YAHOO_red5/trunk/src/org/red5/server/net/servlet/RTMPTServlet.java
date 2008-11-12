package org.red5.server.net.servlet;

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

import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;
import org.red5.server.net.rtmpt.RTMPTConnection;
import org.red5.server.net.rtmpt.RTMPTHandler;

/**
 * Servlet that handles all RTMPT requests.
 *
 * @author The Red5 Project (red5@osflash.org)
 * @author Joachim Bauch (jojo@struktur.de)
 */
public class RTMPTServlet extends HttpServlet {

	private static final long serialVersionUID = -3097083526127047079L;

	protected static Logger log = Logger.getLogger(RTMPTServlet.class);

	/**
	 * HTTP request method to use for RTMPT calls.
	 */
	private static final String REQUEST_METHOD = "POST";

	/**
	 * Content-Type to use for RTMPT requests / responses.
	 */
	private static final String CONTENT_TYPE = "application/x-fcs";

	/**
	 * URL that is called to start a new RTMPT session.
	 */
	@SuppressWarnings("unused") private static final String OPEN_REQUEST = "/open";

	/**
	 * URL that is called to close a RTMPT session.
	 */
	@SuppressWarnings("unused") private static final String CLOSE_REQUEST = "/close";

	/**
	 * URL that is called to send data through a RTMPT connection.
	 */
	@SuppressWarnings("unused")	private static final String SEND_REQUEST = "/send";

	/**
	 * URL that is called to poll RTMPT connection for new events / data.
	 */
	@SuppressWarnings("unused") private static final String IDLE_REQUEST = "/idle";

	/**
	 * Try to generate responses that contain at least 32768 bytes data.
	 * Increasing this value results in better stream performance, but
	 * also increases the latency.
	 */
	private static final int RESPONSE_TARGET_SIZE = 32768;

	/**
	 * Holds a map of client id -> client object.
	 */
	protected HashMap rtmptClients = new HashMap();

	/**
	 * Return an error message to the client.
	 *
	 * @param message
	 * @param resp
	 * @throws IOException
	 */
	protected void handleBadRequest(String message, HttpServletResponse resp)
		throws IOException {
		resp.setStatus(HttpServletResponse.SC_BAD_REQUEST);
		resp.setContentType("text/plain");
		resp.setContentLength(message.length());
		resp.getWriter().write(message);
		resp.flushBuffer();
	}

	/**
	 * Return a single byte to the client.
	 *
	 * @param message
	 * @param resp
	 * @throws IOException
	 */
	protected void returnMessage(byte message, HttpServletResponse resp)
		throws IOException {
		resp.setStatus(HttpServletResponse.SC_OK);
		resp.setHeader("Connection", "Keep-Alive");
		resp.setHeader("Cache-Control", "no-cache");
		resp.setContentType(CONTENT_TYPE);
		resp.setContentLength(1);
		resp.getWriter().write(message);
		resp.flushBuffer();
	}

	/**
	 * Return a message to the client.
	 *
	 * @param message
	 * @param resp
	 * @throws IOException
	 */
	protected void returnMessage(String message, HttpServletResponse resp)
		throws IOException {
		resp.setStatus(HttpServletResponse.SC_OK);
		resp.setHeader("Connection", "Keep-Alive");
		resp.setHeader("Cache-Control", "no-cache");
		resp.setContentType(CONTENT_TYPE);
		resp.setContentLength(message.length());
		resp.getWriter().write(message);
		resp.flushBuffer();
	}

	/**
	 * Return raw data to the client.
	 *
	 * @param client
	 * @param buffer
	 * @param resp
	 * @throws IOException
	 */
	protected void returnMessage(RTMPTConnection client, ByteBuffer buffer, HttpServletResponse resp)
		throws IOException {
		resp.setStatus(HttpServletResponse.SC_OK);
		resp.setHeader("Connection", "Keep-Alive");
		resp.setHeader("Cache-Control", "no-cache");
		resp.setContentType(CONTENT_TYPE);
		//this will prevent stringbuffers from being created when not in debug mode
		if (log.isDebugEnabled()) {
			log.debug("Sending " + buffer.limit() + " bytes.");
		}
		resp.setContentLength(buffer.limit() + 1);
		ServletOutputStream output = resp.getOutputStream();
		output.write(client.getPollingDelay());
		ServletUtils.copy(buffer.asInputStream(), output);
		buffer = null;
	}

	/**
	 * Return the client id from a url like /send/123456/12 -> 123456
	 */
	protected String getClientId(HttpServletRequest req) {
		String path = req.getPathInfo();
		if (path.equals("")) {
			return "";
		}

		if (path.charAt(0) == '/') {
			path = path.substring(1);
		}

		int endPos = path.indexOf('/');
		if (endPos != -1) {
			path = path.substring(0, endPos);
		}

		return path;
	}

	/**
	 * Get the RTMPT client for a session.
	 *
	 * @param req
	 * @return
	 */
	protected RTMPTConnection getClient(HttpServletRequest req) {
		String id = getClientId(req);
		if (StringUtils.isEmpty(id) || !rtmptClients.containsKey(id)) {
			//this will prevent stringbuffers from being created when not in debug mode
			if (log.isDebugEnabled()) {
				log.debug("Unknown client id: " + id);
			}
			return null;
		}

		return (RTMPTConnection) rtmptClients.get(id);
	}

	/**
	 * Skip data sent by the client.
	 *
	 * @param req
	 * @throws IOException
	 */
	protected void skipData(HttpServletRequest req) throws IOException {
		ByteBuffer data = ByteBuffer.allocate(req.getContentLength());
		ServletUtils.copy(req.getInputStream(), data.asOutputStream());
		data.flip();
		data = null;
	}

	/**
	 * Send pending messages to client.
	 *
	 * @param client
	 * @param resp
	 * @throws IOException
	 */
	protected void returnPendingMessages(RTMPTConnection client, HttpServletResponse resp)
		throws IOException {

		ByteBuffer data = client.getPendingMessages(RESPONSE_TARGET_SIZE);
		if (data == null) {
			// no more messages to send...
			returnMessage(client.getPollingDelay(), resp);
			return;
		}

		returnMessage(client, data, resp);
	}

	/**
	 * Start a new RTMPT session.
	 *
	 * @param req
	 * @param resp
	 * @throws ServletException
	 * @throws IOException
	 */
	protected void handleOpen(HttpServletRequest req, HttpServletResponse resp)
		throws ServletException, IOException {

		// Skip sent data
		skipData(req);

		// TODO: should we evaluate the pathinfo?

//		RTMPTHandler handler = (RTMPTHandler) getServletContext().getAttribute(RTMPTHandler.HANDLER_ATTRIBUTE);
//		RTMPTConnection client = new RTMPTConnection(handler);
//		synchronized (rtmptClients) {
//			rtmptClients.put(client.getId(), client);
//		}
//
//		// Return connection id to client
//		returnMessage(client.getId() + "\n", resp);
	}

	/**
	 * Close a RTMPT session.
	 *
	 * @param req
	 * @param resp
	 * @throws ServletException
	 * @throws IOException
	 */
	protected void handleClose(HttpServletRequest req, HttpServletResponse resp)
		throws ServletException, IOException {

		// Skip sent data
		skipData(req);

		RTMPTConnection client = getClient(req);
		if (client == null) {
			handleBadRequest("Unknown client.", resp);
			return;
		}

		synchronized (rtmptClients) {
			rtmptClients.remove(client.getId());
		}

		RTMPTHandler handler = (RTMPTHandler) getServletContext().getAttribute(RTMPTHandler.HANDLER_ATTRIBUTE);
		client.setServletRequest(req);
		handler.connectionClosed(client, client.getState());

		returnMessage((byte) 0, resp);
	}

	/**
	 * Add data for an established session.
	 *
	 * @param req
	 * @param resp
	 * @throws ServletException
	 * @throws IOException
	 */
	protected void handleSend(HttpServletRequest req, HttpServletResponse resp)
		throws ServletException, IOException {

		RTMPTConnection client = getClient(req);
		if (client == null) {
			handleBadRequest("Unknown client.", resp);
			return;
		}

		client.setServletRequest(req);

		// Put the received data in a ByteBuffer
		int length = req.getContentLength();
		ByteBuffer data = ByteBuffer.allocate(length);
		ServletUtils.copy(req.getInputStream(), data.asOutputStream());
		data.flip();

		// Decode the objects in the data
		List messages = client.decode(data);
		data = null;
		if (messages == null || messages.isEmpty()) {
			returnMessage(client.getPollingDelay(), resp);
			return;
		}

		// Execute the received RTMP messages
		RTMPTHandler handler = (RTMPTHandler) getServletContext().getAttribute(RTMPTHandler.HANDLER_ATTRIBUTE);
		Iterator it = messages.iterator();
		while (it.hasNext()) {
			try {
				handler.messageReceived(client, client.getState(), it.next());
			} catch (Exception e) {
				log.error("Could not process message.", e);
			}
		}

		// Send results to client
		returnPendingMessages(client, resp);
	}

	/**
	 * Poll RTMPT session for updates.
	 *
	 * @param req
	 * @param resp
	 * @throws ServletException
	 * @throws IOException
	 */
	protected void handleIdle(HttpServletRequest req, HttpServletResponse resp)
		throws ServletException, IOException {

		// Skip sent data
		skipData(req);

		RTMPTConnection client = getClient(req);
		if (client == null) {
			handleBadRequest("Unknown client.", resp);
			return;
		}

		client.setServletRequest(req);
		returnPendingMessages(client, resp);
	}

	/**
	 * Main entry point for the servlet.
	 */
	@Override
	protected void service(HttpServletRequest req, HttpServletResponse resp)
		throws ServletException, IOException {

		log.info("RTMPT service");

		if (!req.getMethod().equals(REQUEST_METHOD) ||
			req.getContentLength() == 0 ||
			req.getContentType() == null ||
			!req.getContentType().equals(CONTENT_TYPE)) {
			// Bad request - return simple error page
			handleBadRequest("Bad request, only RTMPT supported.", resp);
			return;
		}

		// XXX Paul: since the only current difference in the type of request
		// that we are interested in is the 'second' character, we can double
		// the speed of this entry point by using a switch on the second charater.
		char p = req.getServletPath().charAt(1);
		switch (p) {
			case 'o': //OPEN_REQUEST
				handleOpen(req, resp);
				break;
			case 'c': //CLOSE_REQUEST
				handleClose(req, resp);
				break;
			case 's': //SEND_REQUEST
				handleSend(req, resp);
				break;
			case 'i': //IDLE_REQUEST
				handleIdle(req, resp);
				break;
			default:
				handleBadRequest("RTMPT command " + p + " is not supported.", resp);
		}

	}

}
