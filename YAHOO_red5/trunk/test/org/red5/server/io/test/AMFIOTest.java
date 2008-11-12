package org.red5.server.io.test;

/*
 * RED5 Open Source Flash Server - http://www.osflash.org/red5
 * 
 * Copyright � 2006 by respective authors. All rights reserved.
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
 * 
 * @author The Red5 Project (red5@osflash.org)
 * @author Luke Hubbard, Codegent Ltd (luke@codegent.com)
 */

import org.apache.mina.common.ByteBuffer;
import org.red5.io.amf.Input;
import org.red5.io.amf.Output;
import org.red5.io.utils.HexDump;

public class AMFIOTest extends AbstractIOTest {

	ByteBuffer buf;
	
	/** {@inheritDoc} */
    @Override
	void setupIO() {
		buf = ByteBuffer.allocate(0); // 1kb
		buf.setAutoExpand(true);
		in = new Input(buf);
		out = new Output(buf);
	}

	/** {@inheritDoc} */
    @Override
	void dumpOutput() {
		buf.flip();
		System.err.println(HexDump.formatHexDump(buf.getHexDump()));
	}

	/** {@inheritDoc} */
    @Override
	void resetOutput() {
		 setupIO();
	}

}
