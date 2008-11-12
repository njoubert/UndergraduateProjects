package org.red5.samples.publisher.command 
{
	/**
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
	 	
	import com.adobe.cairngorm.commands.ICommand;
	import com.adobe.cairngorm.control.CairngormEvent;
	
	import org.red5.samples.publisher.business.NetStreamDelegate;
	import org.red5.samples.publisher.events.PublishStreamEvent;
	import org.red5.samples.publisher.model.*;
	
	/**
	 * Setup the <code>nsPublish</code> NetStream for publishing.
	 * <p>The NetStream is stored in the Model.</p>
	 * 
	 * @see org.red5.samples.publisher.model.Media#nsPublish nsPublish
	 * @author Thijs Triemstra
	 */	
	public class PublishStreamCommand implements ICommand 
	{
		/**
		* 
		*/			
		private var model : ModelLocator = ModelLocator.getInstance();
	 	
	 	/**
	 	* 
	 	*/	 	
	 	private var monitorTransaction : MonitorTransaction = model.monitorTransaction;
	 	
	 	/**
	 	* 
	 	*/	 	
	 	private var netStreamDelegate : NetStreamDelegate = monitorTransaction.ns_delegate;
	 
	 	/**
	 	 * 
	 	 * @param cgEvent
	 	 */	 	 	
	 	public function execute( cgEvent : CairngormEvent ) : void
	    { 
	    	var event : PublishStreamEvent = PublishStreamEvent( cgEvent );
	    	var publishMode : String = event.publishMode;
	    	var streamName : String = event.streamName;
			// Use Delegate to publish the NetStream.
	      	netStreamDelegate.startPublish( publishMode, streamName );
		}
				
	}
}