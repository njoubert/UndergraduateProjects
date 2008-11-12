package com.yahoo.happenings.timeline
{
	import mx.containers.Canvas;
	import mx.controls.Image;
	import mx.controls.Label;
	import flash.net.URLRequest;
	import flash.net.URLRequestMethod;
	import flash.net.URLVariables;
	import flash.net.URLLoader;
	import mx.core.Container;
	import flash.events.MouseEvent;
	import mx.events.ResizeEvent;
	import flash.events.Event;
	import flash.display.Shape;
	import mx.formatters.PhoneFormatter;
	import flash.geom.Point;

	public class TimelineView extends Canvas
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
		
		public var minTime:Date= null, maxTime:Date = null;
		
		public var photos:Array = new Array();
		private var timeLabel:Label = new Label();
		private var infoLabel:Label = new Label();
		
	
		public static function sqlDateToDate(date:String):Date {
			var matches:Array = date.match(/(\d\d\d\d)-(\d\d)-(\d\d) (\d\d):(\d\d):(\d\d)/);
			if (matches != null)
				return new Date(matches[1], matches[2], matches[3],
								matches[4], matches[5], matches[6]);
			return null;
		}
		public function TimelineView() {
			request("flickr.photos.search", {
				tags:"arcade fire", 
				sort:"date_taken_asc", 
				per_page:500, 
				extras:"date_taken", 
				min_taken_date: "2007-06-01", 
				max_taken_date: "2007-06-03"
			},function(xml:XML):void {
				var cx:Number = 0;
				for each (var node:XML in xml.photos.children()) {
					var photo:Photo = new Photo(node);
					
					photos.push(photo);
					
					addChild(photo);
					if (minTime==null || photo.date.time < minTime.time)
						minTime = photo.date;
					if (maxTime==null || photo.date.time > maxTime.time)
						maxTime = photo.date;
						
   				}
				
			});
			addEventListener(MouseEvent.MOUSE_MOVE,updatePositions);
			addEventListener(ResizeEvent.RESIZE,updatePositions);
			
			addChild(timeLabel);
		}
		internal var activePhoto:Photo = null;
		protected function updatePositions(ev:Event):void {
			var minDist:Number;
			//if (activePhoto!=null)
			//	activePhoto.alpha = 1;
			activePhoto = null;
    		for each (var photo:Photo in photos) {
    			photo.position(this);
    		}
    		
    		photos.sort(function(a:Photo,b:Photo):int {
    			if (a.scale<b.scale)
    				return -1;
    			if (a.scale>b.scale)
    				return 1;
    			return 0;
    		});
    		var i:int = 0;
    		for each (photo in photos) {
    			setChildIndex(photo,i++);
    			if (photo.hover)
    				activePhoto = photo;
    		}
    		
    		if (activePhoto!=null) {
    			
    		}
     		var s:Shape = new Shape();
    		s.graphics.beginFill(0xFFFFFF);
    		s.graphics.drawRect(x,y,width,height);
    		s.graphics.endFill();
    		mask = s;
    		
    		graphics.clear();
    		graphics.beginFill(0);
    		graphics.drawRect(0,0,width,height);
    		graphics.endFill();
    		graphics.lineStyle(2,0xFFFFFF,0.2);
    		if (minTime!=null && maxTime!=null) {
    			var duration:Number = maxTime.time - minTime.time;
	    		for (var t:Number = minTime.time; t<=maxTime.time; t+=1000*60*60) {
	    			var b:Boolean = true;
	    			for (var y:Number = -10; y<=height+10; y+=20) {
	    				var xx:Number = (t-minTime.time)*width/(maxTime.time-minTime.time);
						var yy:Number = y;
						var scale:Number = 200*Photo.gaussian(Photo.hypot(mouseX-xx,mouseY-yy),100,0);
						xx -= (mouseX-xx)*scale*2;
						yy -= (mouseY-yy)*scale*2;
						if (b) {
							graphics.moveTo(xx,yy);
							b=false;
						} else {
							graphics.lineTo(xx,yy);
						}
	    			}
	    		}
	    		timeLabel.x = mouseX+3;
	    		timeLabel.y = height - 20;
    			var d:Date = new Date((mouseX*(maxTime.time - minTime.time)/width)+minTime.time);
    			timeLabel.text = d.fullYear+"."+pad(d.month)+"."+pad(d.day)+" "+pad(d.hours)+":"+pad(d.minutes);
	    		graphics.lineStyle(2,0xFFFFFF,0.5);
	    		graphics.moveTo(mouseX,0);
	    		graphics.lineTo(mouseX,height);
    		}
    		if (activePhoto!=null) {
    			graphics.lineStyle(3,0xFFFFFF,0.5);
    			graphics.beginFill(0x000000,0.3);
    			graphics.drawRect(activePhoto.x-10,activePhoto.y-10,activePhoto.width+20,activePhoto.height+20);
    			graphics.endFill();
    			infoLabel.x = activePhoto.x+activePhoto.width+3;
    			infoLabel.y = activePhoto.y;
    			infoLabel.width = activePhoto.width;
    			infoLabel.height = activePhoto.height;
    			if (activePhoto.info != null)
    				infoLabel.text = activePhoto.info.toString();
    		}
    		
		}
		
	
		private function pad(i:int):String {
			if (i<10)
				return "0"+i;
			return i.toString();
		}
		
		
		/*
		override protected function updateDisplayList(unscaledWidth:Number, unscaledHeight:Number):void {
			super.updateDisplayList(unscaledWidth,unscaledHeight);
    		for each (var photo:Photo in photos) {
				photo.x = Math.random()*(unscaledWidth-photo.width);
    			photo.y = Math.random()*(unscaledHeight-photo.height);
    		}
		}*/
		
	}
}

import flash.display.Loader;
import flash.net.URLRequest;
import mx.containers.Canvas;
import mx.core.IUIComponent;
import mx.controls.Image;
import com.yahoo.happenings.timeline.TimelineView;
import mx.controls.Label;
import mx.core.Container;
dynamic class Photo extends Image {
	internal var view:TimelineView;
	internal var node:XML;
	internal var date:Date;
	internal var randY:Number;
	internal var scale:Number;
	internal var imgWidth:Number,imgHeight:Number;
	internal var favorites:XML, info:XML;
	public function Photo(node:XML) {
		super();
		this.node = node;
		this.date = TimelineView.sqlDateToDate(node.@datetaken);
		load("http://farm" + node.@farm + ".static.flickr.com/" + 
							   node.@server + "/" +
							   node.@id + "_" + node.@secret + "_t.jpg");
		this.randY = Math.random();
		addEventListener("complete",function():void {
			imgWidth=contentWidth;
			imgHeight=contentHeight;
		});
		TimelineView.request("flickr.photos.getFavorites", {
			photo_id: node.@id,
			per_page: 100
		},function(xml:XML):void {
			favorites = xml;
		});
		
		TimelineView.request("flickr.photos.getInfo", {
			photo_id: node.@id
		},function(xml:XML):void {
			info = xml;
		});
		
	}
	public static function gaussian(x:Number,sigma:Number,mu:Number):Number {
		return Math.exp(-(x-mu)*(x-mu)/(2*sigma*sigma))
					/ (sigma*Math.sqrt(2 * Math.PI));
	}
	public static function hypot(x:Number, y:Number):Number {
		return Math.sqrt(x*x+y*y);
	}
	public var hover:Boolean = false;
	public function position(canvas:TimelineView):void {
		x = ((date.time-canvas.minTime.time)/(canvas.maxTime.time-canvas.minTime.time))*(canvas.width-100)+50;
		y = randY*(canvas.height-100)+50;
		var dist:Number = hypot(canvas.mouseX-x,canvas.mouseY-y);
		var g:Number = gaussian(dist,100,0);
		scale = 200*g;
		width = imgWidth*(0.5+scale);
		height = imgHeight*(0.5+scale);
		x -= width/2 + (canvas.mouseX-x)*scale*2;
		y -= height/2 + (canvas.mouseY-y)*scale*2;
		
		hover = canvas.mouseX>=x && canvas.mouseY>=y && canvas.mouseX<x+width && canvas.mouseY<y+height;
		alpha = scale+scale+0.4;
		
		
		graphics.clear();
		if (hover) {
			
			graphics.lineStyle(10,0xFFFFFF,0.5);
			graphics.beginFill(0x000000,0.3);
			graphics.drawRect(-10,-10,width+20,height+20);
			graphics.endFill();
			
		}
		
			var inf:XML = this.info;
			if (info != null) {
				var comments:int = info.photo.comments;
				trace(comments);
				
				for (var i:int = 0; i<comments; i++) {
					var xi:Number = width/2;
					var yi:Number = height/2;
					graphics.lineStyle(5*(scale+0.5),0xFFFFFF);
					graphics.endFill();
					graphics.moveTo(xi,yi);
					var theta:Number = i*Math.PI*2/comments;
					graphics.lineTo(xi+Math.cos(theta)*100*(scale+0.5),
									yi+Math.sin(theta)*100*(scale+0.5));
					
					//graphics.beginFill(0x808080);
					graphics.drawEllipse(xi+Math.cos(theta)*110*(scale+0.5)-10,yi+Math.sin(theta)*110*(scale+0.5)-10,20,20);
					//graphics.endFill();*/
				}
			} else {
				
			}
	}
	
}