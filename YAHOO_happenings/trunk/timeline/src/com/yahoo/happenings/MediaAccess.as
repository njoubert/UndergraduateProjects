package com.yahoo.happenings
{
	import flash.net.URLRequest;
	import flash.net.URLVariables;
	import flash.net.URLRequestMethod;
	import flash.net.URLLoader;
	import com.adobe.serialization.json.JSON;
	
	public class MediaAccess
	{
		
	
		public static function getRequest(method:String, params:Object):URLRequest {
			var url:String = "http://api.flickr.com/services/rest/";
			var req:URLRequest = new URLRequest(url);
			var variables:URLVariables = new URLVariables();
			
			for (var key:String in params)
				variables[key] = params[key];
				
			variables.method = method;
			variables.api_key = "ffd6fd52b0cc95305f70e5a506af543d";
			
			req.data = variables;
			req.method = URLRequestMethod.POST;
			return req;
		}
		public static function request(method:String,params:Object,func:Function):void {
			var loader:URLLoader = new URLLoader();
			loader.load(getRequest(method,params));
			loader.addEventListener("complete",function():void {
				func(new XML(loader.data));
			});
		}
		public static function request2(func:Function):void {
			var url:String = "http://dev1.brl.corp.yahoo.com:8181/rahul/getEventMedia.php";
			var req:URLRequest = new URLRequest(url);
			var variables:URLVariables = new URLVariables();
			
			variables.output = 'json';
			
			req.data = variables;
			req.method = URLRequestMethod.GET;
			
			
			var loader:URLLoader = new URLLoader();
			loader.load(req);
			loader.addEventListener("complete",function():void {
				func(JSON.decode(loader.data));
			});
		}
	}
}