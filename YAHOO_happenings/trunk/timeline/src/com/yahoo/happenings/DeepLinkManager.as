package com.yahoo.happenings
{
		
	import mx.events.BrowserChangeEvent;
	import mx.managers.IBrowserManager;
	import mx.managers.BrowserManager;
	import mx.utils.URLUtil;
	import flash.events.EventDispatcher;
	import flash.events.Event;
	import mx.controls.Alert;
	public class DeepLinkManager extends EventDispatcher
	{
		private static var instance:DeepLinkManager;

	//--------------------------------------------------------------------------
	//
	//  Class methods
	//
	//--------------------------------------------------------------------------

		/**
		 *  Returns the sole instance of this singleton class,
		 *  creating it if it does not already exist.
	     */
		public static function getInstance():DeepLinkManager
		{
			if (!instance)
				instance = new DeepLinkManager();
	
			return instance;
		}

		
		private var browserManager:IBrowserManager;
	   
		public function DeepLinkManager(titleBase:String = "") {
			browserManager = BrowserManager.getInstance();
			browserManager.addEventListener(BrowserChangeEvent.BROWSER_URL_CHANGE, function(event:BrowserChangeEvent):void {
				runEvents();
			});
			browserManager.init("", titleBase);
			runEvents();
			this.titleBase = titleBase;
		}
		public function get url():String {
			return browserManager.url;
		}
		override public function addEventListener(type:String, listener:Function, useCapture:Boolean=false, priority:int=0, useWeakReference:Boolean=false):void {
			super.addEventListener(type,listener,useCapture,priority,useWeakReference);
			for (var x:String in lastObject)
				listener(new DeepLinkEvent(DeepLinkEvent.CHANGE,x,null,lastObject[x]));
		}
		public function runEvents():void {
			parsing = true;
			
			var o:Object = URLUtil.stringToObject(browserManager.fragment);
			trace("runEvents: "+browserManager.fragment);
			var x:String;
			for (x in o) {
				if (!x) continue;
				if (x && lastObject[x] != o[x]) {
					trace("dispatch "+x+": "+lastObject[x]+" -> " + o[x]);
					dispatchEvent(new DeepLinkEvent(DeepLinkEvent.CHANGE,x,lastObject[x],o[x]));
				}
				delete lastObject[x];
			}
			for (x in lastObject) {
				if (!x) continue;
				trace("dispatch (remove) "+x+": "+lastObject[x]+" -> " + null);
				dispatchEvent(new DeepLinkEvent(DeepLinkEvent.CHANGE,x,lastObject[x],null));
			}
			lastObject = o;
				
			parsing = false;	
		}
		
		public var titleBase:String;
		private var lastObject:Object = {};
		private var parsing:Boolean = false;
		
		/**
		 * Returns a parameter from the hash
		 */
		public function getParameter(name:String): Object {
			return lastObject[name];
		}
		/**
		 * Sets a parameter in the hash
		 */
		public function setParameter(name:String, value:Object):void {
			if (!parsing) {
				if (!name) return;
				trace("set "+name+" -> " + value);
				if (value==null)
					delete lastObject[name];
				else
					lastObject[name] = value;
				
				var s:String = URLUtil.objectToString(lastObject);
				browserManager.setFragment(s);
			}
		}
		/**
		 * Sets the browser page title
		 */
		public function setTitle(title:String):void {
			browserManager.setTitle(title ? titleBase + ": " + title : titleBase);
		}
		
	}
}