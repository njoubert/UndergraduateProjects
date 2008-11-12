package com.yahoo.happenings
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
	import com.adobe.serialization.json.JSON;
	import flash.geom.Rectangle;
	import mx.core.UIComponent;
	import flash.display.DisplayObjectContainer;
	import com.yahoo.happenings.MediaAccess;
	import flash.display.Loader;
	import flash.text.TextFormat;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextField;
	import flash.display.Graphics;
	import com.yahoo.timetags.IVisualMediaItem;
	import mx.controls.Alert;
	import flash.utils.setTimeout;

	public class TimelineView extends GraphicsComponent
	{
		public var minTime:Date= null, maxTime:Date = null;
		
		public var photos:Array = new Array();
		private var timeLabel:TextField = new TextField();
		private var timeStartLabel:TextField = new TextField();
		private var timeEndLabel:TextField = new TextField();
		private var infoLabel:TextField = new TextField();
		public var pseudoLength:Number = 0;
		
		public var scrubber:TimelineScrubber = null;
		public var thumbnail:PhotoThumbnail = new PhotoThumbnail();
		
		[Bindable]
		public var concert:Concert = null;
	
		public static function sqlDateToDate(date:String):Date {
			var matches:Array = date.match(/(\d\d\d\d)-(\d\d)-(\d\d) (\d\d):(\d\d):(\d\d)/);
			if (matches != null)
				return new Date(matches[1], matches[2], matches[3],
								matches[4], matches[5], matches[6]);
			return null;
		}
		public function TimelineView() {
			addEventListener(MouseEvent.MOUSE_MOVE,function(ev:MouseEvent):void {
				scrubPseudoDate = (mouseX*pseudoLength)/width/filterScale-filterOffset;
	    		repaint();	
			});
			addEventListener(ResizeEvent.RESIZE,function():void {
				repaint();
			});
			
			addEventListener(MouseEvent.MOUSE_OVER,function():void {
				bubbleIntensity = Math.min(width,height) * 100 / 500;
			});
			addEventListener(MouseEvent.MOUSE_OUT,function(): void {
				bubbleIntensity = 1;
			});
			addEventListener(MouseEvent.CLICK,function(event:Event): void {
				if (!readMouse) {
					thumbnail.hide();
    				DeepLinkManager.getInstance().setParameter("tlmedia", null);
					readMouse = true;
					repaint();
				}
				if (readMouse && activePhoto!=null) {
					
    				thumbnail.photo = activePhoto;
					thumbnail.show();
    				DeepLinkManager.getInstance().setParameter("tlmedia", activePhoto.node.id);
					readMouse = false;
					//showOnly = !showOnly;
					//repaint();
				}
				event.stopPropagation();
			});
			
			addChild(timeLabel);
			addChild(timeStartLabel);
			addChild(timeEndLabel);
			addChild(infoLabel);
			addChild(thumbnail);
			thumbnail.addEventListener(MouseEvent.MOUSE_OVER,function():void {
				readMouse = false;
			});
			thumbnail.addEventListener(PhotoThumbEvent.TIMECHANGE,function():void {
				updatePhotoTimes();
			});
			thumbnail.addEventListener(PhotoThumbEvent.CLOSE,function():void {
				readMouse = true;
				thumbnail.hide();
			
	    		repaint();
			});
			thumbnail.filterAction = function(filter:Boolean):void {
				showPhoto = filter ? thumbnail.photo : null;
				repaint();
			}
			
            timeLabel.autoSize = TextFieldAutoSize.LEFT;
            timeLabel.selectable = false;
			
            timeStartLabel.autoSize = TextFieldAutoSize.LEFT;
            timeStartLabel.selectable = false;
			
            timeEndLabel.autoSize = TextFieldAutoSize.RIGHT;
            timeEndLabel.selectable = false;

            var format:TextFormat = new TextFormat();
            format.font = "Verdana";
            format.color = 0xFFFFFF;
            format.size = 20;

            timeLabel.defaultTextFormat = format;
            timeStartLabel.defaultTextFormat = format;
            timeEndLabel.defaultTextFormat = format;
            
            infoLabel.autoSize = TextFieldAutoSize.LEFT;
            infoLabel.selectable = false;
            
            format = new TextFormat();
            format.font = "Verdana";
            format.color = 0xFFFFFF;
            format.size = 15;
            infoLabel.defaultTextFormat = format;
            
            
            
            
			addEventListener("propertyChange",function(event:PropertyChangeEvent):void {
				if (event.property=="concert") {
					trace(concert.startTime);
					for each (var item:IConcertRelatedItem in concert.relatedItems) {
						var photo:Photo = new Photo(item,concert);
						if (photo.date !=null && (photo.date.time < concert.startTime.time - 1000 * 60 * 60 * 10 ||
							photo.date.time > concert.startTime.time + 1000 * 60 * 60 * 10)) {
							trace("photo "+photo.node.id+" is out of date range: "+photo.date);
							continue;
						}
						photos.push(photo);
						addChild(photo);
						photo.loadImage();
					}
	   				updatePhotoTimes();
   			
		   			scrubber.startListening();
		   			
					DeepLinkManager.getInstance().addEventListener(DeepLinkEvent.CHANGE,function(event:DeepLinkEvent):void {
						switch (event.field) {
							case "tlmedia":
								if (event.newValue==null) {
									thumbnail.hide();
									readMouse = true;
									repaint();
									
								} else
									for each (var p:Photo in photos)
										if (p.node.id == event.newValue) {
											activePhoto = p;
						    				thumbnail.photo = activePhoto;
											thumbnail.show();
											readMouse = false;
											repaint();
											break;
										}
								break;
						}
					});
				}
			});
		}
		private var untimedPhotos:Array;
		public function get untimedBarSize():Number {
			return untimedPhotos.length == 0 ? 0 : 100;
		}
    	public function updatePhotoTimes(): void {
    		untimedPhotos = [];
   			for each (var photo:Photo in photos) {
				if (photo.date == null) {
    				photo.untimedIndex = untimedPhotos.length;
    				untimedPhotos.push(photo);
    			}
				if (photo.date != null && (minTime==null || photo.date.time < minTime.time))
					filterStartTime = minTime = photo.date;
				if (photo.date != null && (maxTime==null || photo.date.time > maxTime.time))
					filterEndTime = maxTime = photo.date;
   			}
   			
   			var step:Number = 15*60*1000; // 15 minutes
   			var photoCounts:Array = [];
   			pseudoLength = 0;
   			segments = [];
   			var startTime:Date = new Date(minTime.fullYear,minTime.month,minTime.date,minTime.hours,0,0,0);
   			for (var t:Number = startTime.time; t<maxTime.time; t+=step) {
   				var actualDate:Date = new Date(t);
   				photoCounts = [];
   				for each (var p:Photo in photos)
   					if (p.date!=null && p.date.time>=t && p.date.time < t+step)
   						photoCounts.push(p);
   				var count:Number = photoCounts.length;
   				var pseudoSegmentLength:Number = lerp(1,count/10,0.75);//0.75);
   				var userList:Object = {};
   				var userCount:Number = 0;
   				for each (p in photoCounts) {
   					p.pseudoTime = pseudoLength +
   					 pseudoSegmentLength * (p.date.time - t) 
   					 / step;
   					if (!userList[p.owner]) {
   						userCount++;
   						userList[p.owner] = true;
   					}
   				}
   				if (count>maxSegmentPhotos)
   					maxSegmentPhotos = count;
   				segments.push({
   					importance: (actualDate.minutes==0 ? 0.3 : 0.1),
   					start: actualDate,
   					duration: step, 
   					photos:photoCounts, 
   					count:count, 
   					userCount:userCount,
   					pseudoTime:pseudoLength, 
   					pseudoLength:pseudoSegmentLength
   				});
   				pseudoLength += pseudoSegmentLength;
   			}
			loaded = true;
   			repaint();
   			
		}
   		public function lerp(a:Number, b:Number, t:Number):Number {
   			return a * (1-t) + b * t;
   		}
		public var segments:Array = [];
		internal var activePhoto:Photo = null;
		internal var showPhoto:Photo = null;
		internal var showOnly:Boolean = false;
		internal var readMouse:Boolean = true;
		public var maxSegmentPhotos:Number = 0;
  		public var scrubDate:Date = null;
  		public var scrubPseudoDate:Number = 0;
  		public var filterStartTime:Date = null, filterEndTime:Date = null;
		public var filterStartPseudoTime:Number = 0;
		public var filterEndPseudoTime:Number = 1;
		public var filterScale:Number = 1;
		public var filterOffset:Number = 0;
		
		public var bubbleIntensity:Number = 1;
		
		private var loaded:Boolean = false;
		
		internal var lastMouseX:Number,lastMouseY:Number;
  		override protected function paint(g:Graphics): void {
  		
  			if (!loaded) return;
  			//if (!readMouse) return;
  			if (readMouse) {
  				lastMouseX = mouseX;
  				lastMouseY = mouseY;
  			}
			var minDist:Number;
			//if (activePhoto!=null)
			//	activePhoto.alpha = 1;
    		if (untimedPhotos.length>0) {
    			var x:Number = 0;
    			for each (var p:Photo in untimedPhotos)
    				if (p.visible) {
    					p.x = x;
    					x += 50;
    				}
    		}
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
			activePhoto = null;
    		for each (photo in photos) {
    			setChildIndex(photo,i++);
    			if (photo.hover && photo.visible)
    				activePhoto = photo;
    		}
    		
			var visiblePhotos:Number = 0;
			for each (photo in photos) {
				if (activePhoto!=null && photo.owner == activePhoto.owner)
					photo.highlight(photo == activePhoto);
				if (showPhoto!=null)
					photo.visible = photo.owner == showPhoto.owner;
				else
					photo.visible = true;
    			if (photo.visible && photo.date!=null && photo.date.time >= filterStartTime.time && photo.date.time < filterEndTime.time)
    				visiblePhotos++;
			}
			
    		photos.sort(function(a:Photo,b:Photo):int {
    			if (a.date!=null && b.date!=null) {
    				if (a.date.time<b.date.time)
	    				return -1;
	    			if (a.date.time>b.date.time)
	    				return 1;
	    		}
    			if (a.node.id<b.node.id)
    				return -1;
    			if (a.node.id>b.node.id)
    				return 1;
    			return 0;
    		});
			var hideStep:Number = (100 / visiblePhotos);
			var hideStepIndex:Number = 0;
			for each (photo in photos) {
				if (photo.date==null) continue;
				hideStepIndex += hideStep;
				if (hideStepIndex>=1) {
					hideStepIndex--;
				} else {
					photo.visible = false;
				}
			}
			
    		//while (visiblePhotos-->30) {
    		//	photos[Math.floor(Math.random()*photos.length)].visible = false;
    		//}
  
    		g.clear();
    		g.beginFill(0);
    		g.drawRect(0,0,width,height);
    		g.endFill();
    		if (minTime!=null && maxTime!=null) {
    			
    			var distance:Number = 40;

    			var segment:Object;
    			
    			var firstSegment:int = 0;
    			var lastSegment:int = 0;
    			var foundFirst:Boolean = false;

				var index:int;
				
				var filterStartPseudoTime:Number = 0;
				var filterEndPseudoTime:Number = 0;
				
				
			
				timeStartLabel.text = dateToString(filterStartTime);
				timeEndLabel.text = dateToString(filterEndTime);
				
				timeStartLabel.x = 2;
				timeEndLabel.x = width - timeEndLabel.width - 2;
				timeStartLabel.y = 2 + untimedBarSize;
				timeEndLabel.y = 2 + untimedBarSize;
				
				
				// find segments within range
    			for (index = 0; index<segments.length; index++) {
    				segment = segments[index];
    				if (!foundFirst && segment.start.time >= filterStartTime.time) {
    					firstSegment = index as int;
    					foundFirst = true;
    					filterStartPseudoTime = segment.pseudoTime + segment.pseudoLength * (filterStartTime.time - segment.start.time) / segment.duration;
    				}
    				if (segment.start.time <= filterEndTime.time) {
    					lastSegment = index as int;
    					filterEndPseudoTime = segment.pseudoTime + segment.pseudoLength * (filterEndTime.time - segment.start.time) / segment.duration;
    				} else {
    					break;
    				}
    			}
    			
    			filterScale = pseudoLength / (filterEndPseudoTime - filterStartPseudoTime);
    			filterOffset = -filterStartPseudoTime;
    			
    			var realX:Number;
    			    		
    			// draw vertical lines
    			for (index=firstSegment; index<=lastSegment; index++) {
    				segment = segments[index];
	    			var b:Boolean = true;
    				var lxx:Number,lyy:Number;
    				if (segment.importance < 0.2)
    					continue;
    				g.lineStyle(2,0xFFFFFF,segment.importance * (2*segment.count/maxSegmentPhotos+0.4));
	    			for (var y:Number = -10; y<=height; y+=30) {
						var xx:Number = (segment.pseudoTime+filterOffset)*(width)*filterScale/pseudoLength;
						var yy:Number = y;
						var localscale:Number = bubbleIntensity*Photo.gaussian(Photo.hypot(lastMouseX-xx,lastMouseY-yy),100,0);
						xx -= (lastMouseX-xx)*localscale*2;
						yy -= (lastMouseY-yy)*localscale*2;
						if (b) {
							g.moveTo(xx,yy);
							b=false;
						} else {
							if (yy>height-distance) {
								// calculate intersection 
								if (xx!=lxx) {
									var m:Number = (yy-lyy) / (xx-lxx);
									yy = height-distance;
									xx = (yy-lyy) / m + lxx;
								} else {
									yy = height-distance;
								}
								g.lineTo(xx,yy);
								break;
							}
							g.lineTo(xx,yy);
						}
						lxx = xx;
						lyy = yy;
	    			}
	    			//graphics.lineTo((segment.start.time-minTime.time)*width/(maxTime.time-minTime.time),height);
	    			realX = (segment.start.time-minTime.time)*width/(maxTime.time-minTime.time);
	    			//g.curveTo(xx,height-distance,,height);
	    			g.curveTo(xx,height-distance*0.75,lerp(realX,xx,0.5),height-distance/2);
	    			g.curveTo(realX,height-distance*0.25,realX,height);
    			}
	    		scrubDate = null;
	    		// Loop through segments
	    		for each (segment in segments) {
	    			if (scrubPseudoDate < segment.pseudoTime + segment.pseudoLength) {
	    				// the real time this segment starts, 
	    				//  find the fraction of the mouse pos within this segment
	    				//  multiply by the real duration
	    				scrubDate = new Date(segment.start.time + segment.duration * (scrubPseudoDate - segment.pseudoTime)/segment.pseudoLength);
	    				break;
	    			}
	    		}
	    		if (scrubDate!=null) {
	    			realX = (scrubDate.time-minTime.time)*width/(maxTime.time-minTime.time);
		    		timeLabel.x = realX+3;
		    		timeLabel.y = height - timeLabel.height;
    				timeLabel.text = dateToString(scrubDate);
		    		g.lineStyle(2,0xFF0000,0.5);
					var scrubX:Number = (scrubPseudoDate+filterOffset)*(width)*filterScale/pseudoLength;
		    		g.moveTo(scrubX,0);
		    		g.lineTo(scrubX,height-distance);
	    			g.curveTo(scrubX,height-distance*0.75,lerp(realX,scrubX,0.5),height-distance/2);
	    			g.curveTo(realX,height-distance*0.25,realX,height);
    			}
    			
    			if (scrubber != null)
    				scrubber.repaint();
    		}
    		
    		// Add info label
    		if (activePhoto!=null) {
    			setChildIndex(infoLabel,photos.length);
    			infoLabel.x = activePhoto.x+activePhoto.width+3;
    			infoLabel.y = activePhoto.y;
    			infoLabel.width = activePhoto.width;
    			infoLabel.height = activePhoto.height;
    			infoLabel.text = dateToString(activePhoto.date);//+"\n"+activePhoto.pseudoTime+activePhoto.info;
    			infoLabel.visible = true;
    		} else {
    			infoLabel.visible = false;
    		}
    		if (thumbnail.visible)
    			thumbnail.setSize();
		}
		
		private function dateToString(d:Date):String {
			if (d==null)
				return "";
			if (d.hours==0)
				return "12:"+pad(d.minutes)+"am";
			if (d.hours<13)
				return d.hours+":"+pad(d.minutes)+"am";
			return (d.hours-12)+":"+pad(d.minutes)+"pm";
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
