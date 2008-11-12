package com.yahoo.happenings
{
	import flash.display.Loader;
	import flash.net.URLRequest;
	import flash.display.Sprite;
	import mx.controls.Label;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;
	import com.yahoo.timetags.IVisualMediaItem;
	import flash.display.Shape;
	import mx.controls.Image;
	import mx.controls.SWFLoader;
	import flash.display.DisplayObject;
		
	public class Photo extends Sprite {
		internal var view:TimelineView;
		internal var node:IConcertRelatedItem;
		internal var date:Date;
		internal var randY:Number;
		internal var scale:Number;
		public var owner:String;
		internal var imgWidth:Number,imgHeight:Number;
		internal var favorites:XML, info:String ="";
		internal var loader:Loader = new Loader();
		internal var pseudoTime:Number;
		internal var videoIcon:DisplayObject = null;
		internal var concert:Concert = null;
		public var untimedIndex:Number = 0;

		public function Photo(node:IConcertRelatedItem, concert:Concert) {
			super();
			this.node = node;
			this.date = node.captureTime;
			this.concert = concert;
			if (node is FlickrImage) {
				this.owner = (node as FlickrImage).ownerName;
			} else if (node is YouTubeVideo) {
				this.owner = (node as YouTubeVideo).uploader;
			}
			this.randY = Math.random();
		}
		
        [Embed(source="../../../../assets/video_icon.png")]
        [Bindable]
        public var videoImage:Class;
        
		public function loadImage():void {
			loader.load(new URLRequest(node.thumbnailMediaURL));
			loader.addEventListener("updateComplete",function():void {
				imgWidth=loader.content.width;
				imgHeight=loader.content.height;
				loader.width = imgWidth/2;
				loader.height = imgHeight/2;
			});
			addChild(loader);
			if (node is YouTubeVideo) {
				videoIcon = new videoImage();
				addChild(videoIcon);
			}
		}
		public function setTimestamp(time:String):void {
			var m:Array = time.match(/(\d+):(\d\d)/);
			if (date==null) {
				date = new Date(concert.startTime.time);
			}
			date.hours = m[1];
			date.minutes = m[2];
			node.captureTime = date;
		}
		public function getImage(size:String):String {
			switch (size) {
				case 's':
					if (node is FlickrImage)
						return (node as FlickrImage).squareMediaURL;
				case 'm':
					if (node is FlickrImage)
						return (node as FlickrImage).smallMediaURL;
				case 't':
					return node.thumbnailMediaURL;
			}
			return node.mediaURL;
		}
		/*
		public function getImage(size:String):String {
			
			return "http://farm" + node.@farm + ".static.flickr.com/" + 
								   node.@server + "/" +
								   node.@id + "_" + node.@secret + "_"+size+".jpg";
			return null;
		}*/
		public static function gaussian(x:Number,sigma:Number,mu:Number):Number {
			return Math.exp(-(x-mu)*(x-mu)/(2*sigma*sigma))
						/ (sigma*Math.sqrt(2 * Math.PI));
		}
		public static function hypot(x:Number, y:Number):Number {
			return Math.sqrt(x*x+y*y);
		}
		public var hover:Boolean = false;
		public function position(canvas:TimelineView):void {
			alpha = 1.0;
			
			if (date==null) {
				y = (canvas.untimedBarSize - height) / 2;
				visible = true;
			} else {
				//x = ((date.time-canvas.minTime.time)/(canvas.maxTime.time-canvas.minTime.time))*(canvas.width-100)+50;
				x = (pseudoTime+canvas.filterOffset)*canvas.width*canvas.filterScale/(canvas.pseudoLength);
				info = "\nx="+x;
				y = randY*(canvas.height-120-canvas.untimedBarSize)+20+canvas.untimedBarSize;
			}
			
			var dist:Number = hypot(canvas.lastMouseX-x,canvas.lastMouseY-y);
			var g:Number = gaussian(dist,100,0);
			scale = canvas.bubbleIntensity*g;
			if (date!=null) {
				// = imgWidth*(0.5+scale);
				//height = imgHeight*(0.5+scale);
				
				y -= height/2 + (canvas.lastMouseY-y)*scale*2;
				
			}
			x -= width/2 + (canvas.lastMouseX-x)*scale*2;
			//scaleX = scaleY = 0.5+scale;
			//loader.width = imgWidth * (0.5+scale);
			//loader.height = imgHeight * (0.5+scale);
			loader.scaleX = loader.scaleY = 0.5+scale;
			alpha = scale+scale+0.4;
			hover = canvas.lastMouseX>=x && canvas.lastMouseY>=y && canvas.lastMouseX<x+width && canvas.lastMouseY<y+height;
			
			if (videoIcon != null) {
				videoIcon.x = (width-videoIcon.width)/2;
				videoIcon.y = (height-videoIcon.height)/2;
				//setChildIndex(videoIcon,numChildren-1);
			}
			
			
			
			graphics.clear();
			/*
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
					//graphics.endFill();
				}
			} else {
				
			}*/
		}
		public function highlight(active:Boolean):void {
			graphics.lineStyle(active? 6 : 2,0xadea43,active ? 0.7 : 0.55);
			graphics.beginFill(0x000000,0.3);
			graphics.drawRect(0,0,width,height);
			graphics.endFill();
		}
	}
}