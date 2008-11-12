package com.yahoo.happenings
{
	import flash.display.Graphics;
	import flash.display.Shape;
	import mx.events.DragEvent;
	import flash.display.Sprite;
	import flash.events.MouseEvent;
	import flash.geom.Rectangle;
	import mx.events.ResizeEvent;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.text.TextFieldAutoSize;
	import mx.events.PropertyChangeEvent;
	import mx.controls.Alert;
	
	public class TimelineScrubber extends GraphicsComponent
	{
		
		private var deepLinkManager:DeepLinkManager;
		
		private var bars:Array = [];
		[Bindable]
		public var concert:Concert = null;
		public function TimelineScrubber() {
			deepLinkManager = DeepLinkManager.getInstance();
			
			scrubFill = new Sprite();
			addChild(scrubFill);
			
			scrubStart = new Sprite();
			scrubStart.graphics.beginFill(0xFFFFFF,0.5);
			scrubStart.graphics.drawRect(0,0,10,100);
			addChild(scrubStart);
			
			scrubEnd = new Sprite();
			scrubEnd.graphics.beginFill(0xFFFFFF,0.5);
			scrubEnd.graphics.drawRect(0,0,10,100);
			scrubEnd.x = -1;
			addChild(scrubEnd);
			
            number.autoSize = TextFieldAutoSize.CENTER;
            number.selectable = false;

            var format:TextFormat = new TextFormat();
            format.font = "Verdana";
            format.color = 0xFFFFFF;
            format.size = 10;

            number.defaultTextFormat = format;
            addChild(number);
            
            function readRange():void {
            	
            }
			
			function scrubFillFunc():void {
				scrubEnd.x = scrubFill.x + scrubFill.width;
				scrubStart.x = scrubFill.x - 10;
				updateScrubber();
			}
			scrubFill.addEventListener(MouseEvent.MOUSE_DOWN,function():void {
				if (!timeline.readMouse) return;
				scrubFill.startDrag(false,new Rectangle(10,0,width-scrubFill.width-20,0));
				systemManager.addEventListener(MouseEvent.MOUSE_MOVE,scrubFillFunc);
				systemManager.addEventListener(MouseEvent.MOUSE_UP, function():void {
					scrubFill.stopDrag();
					deepStart();
					deepEnd();
					systemManager.removeEventListener(MouseEvent.MOUSE_MOVE,scrubFillFunc);
					systemManager.removeEventListener(MouseEvent.MOUSE_UP,arguments.callee);
				});
			});
			scrubStart.addEventListener(MouseEvent.MOUSE_DOWN,function ():void {
				if (!timeline.readMouse) return;
				scrubStart.startDrag(false,new Rectangle(0,0,scrubEnd.x-10,0));
				systemManager.addEventListener(MouseEvent.MOUSE_MOVE,updateScrubber);
				systemManager.addEventListener(MouseEvent.MOUSE_UP, function():void {
					scrubStart.stopDrag();
					deepStart();
					systemManager.removeEventListener(MouseEvent.MOUSE_MOVE,updateScrubber);
					systemManager.removeEventListener(MouseEvent.MOUSE_UP,arguments.callee);
				});
			});
			scrubEnd.addEventListener(MouseEvent.MOUSE_DOWN,function ():void { 
				if (!timeline.readMouse) return;
				scrubEnd.startDrag(false,new Rectangle(scrubStart.x+10,0,width-20-scrubStart.x,0));
				systemManager.addEventListener(MouseEvent.MOUSE_MOVE,updateScrubber);
				systemManager.addEventListener(MouseEvent.MOUSE_UP, function():void {
					scrubEnd.stopDrag();
					deepEnd();
					systemManager.removeEventListener(MouseEvent.MOUSE_MOVE,updateScrubber);
					systemManager.removeEventListener(MouseEvent.MOUSE_UP,arguments.callee);
				});
			});
			
			addEventListener(ResizeEvent.RESIZE,function(event:ResizeEvent):void {
				if (scrubEnd.x<0) {
					scrubEnd.x = width-10;
				} else {
					var scaleX:Number = width / event.oldWidth;
					
					scrubStart.x *= scaleX;
					scrubEnd.x = (scrubEnd.x+10)*scaleX-10;
				}
				updateScrubber();
				layoutBars();
			});
			addEventListener(MouseEvent.MOUSE_MOVE, function():void {
				if (timeline.scrubDate != null) {
	    			var date:Number = mouseX * (timeline.maxTime.time-timeline.minTime.time) / width + timeline.minTime.time;
	    			for each (var segment:Object in timeline.segments) {
	    				if (date < segment.start.time + segment.duration) {
	    					timeline.scrubDate = new Date(date);
	    					timeline.scrubPseudoDate = segment.pseudoTime + segment.pseudoLength * (date - segment.start.time) / segment.duration;
	    					break;
	    				}
	    			}
	    			timeline.repaint();
	    		}
			});
			layoutBars();
		}
		
		internal function startListening():void {
			
			deepLinkManager.addEventListener(DeepLinkEvent.CHANGE,function(event:DeepLinkEvent):void {
				try {
				if (event.field=="start") {
					
					scrubStart.x = event.newValue ? ((event.newValue as Number) - timeline.minTime.time) * width / (timeline.maxTime.time-timeline.minTime.time)
									: 0;
					trace((event.newValue as Number)-timeline.minTime.time,timeline.minTime.time,timeline.maxTime.time-timeline.minTime.time,((event.newValue as Number) - timeline.minTime.time) * width / (timeline.maxTime.time-timeline.minTime.time));
					updateScrubber();
					timeline.repaint();
				} else if (event.field=="end") {
				
					scrubEnd.x = (event.newValue ? ((event.newValue as Number) - timeline.minTime.time) * width / (timeline.maxTime.time-timeline.minTime.time)
									: width) - 10;
					updateScrubber();
					timeline.repaint();	
				}
				} catch (ex:Error) {
					Alert.show(ex.toString());
				}
			});
		}
		
		private function layoutBars():void {
			for each (var b:Sprite in bars)
				removeChild(b);
			bars = [];
			if (timeline!=null && timeline.segments && timeline.maxTime!=null) {
				var segment:Object;
    			var max:Number = 0;
    			
    			var frac:Number = width/(timeline.maxTime.time-timeline.minTime.time);
    			for each (segment in timeline.segments) {
					var count:Number = segment.count;
					if (timeline.showPhoto!=null)
						for each (var p:Photo in segment.photos)
							if (p.owner != timeline.showPhoto.owner)
								count--;
    				if (count>max)
    					max = count;
    			}
    			
				max *= 1.05;
    			for each (segment in timeline.segments)
    				makeBar(segment,frac,max);
				
			}
		}
		private function makeBar(segment:Object, frac:Number, max:Number):void {
			var count:Number = segment.count;
			if (timeline.showPhoto!=null)
				for each (var p:Photo in segment.photos)
					if (p.owner != timeline.showPhoto.owner)
						count--;
			if (count==0) return;
			var bar:Sprite = new Sprite();
			bar.x = (segment.start.time-timeline.minTime.time)*frac;
			bar.y = height*(1-count/max);
			bar.graphics.lineStyle();
			bar.graphics.beginFill((timeline.showPhoto!=null) ? 0xadea42 : 0xd2560b);
			bar.graphics.drawRect(0,0,frac*segment.duration,height*(segment.count/max));
			bar.graphics.endFill();
			bar.alpha = 0.3;
			bar.addEventListener(MouseEvent.MOUSE_OVER,function():void {
				bar.alpha = 0.7;
				if (timeline.showPhoto == null) {
					number.text = "photos: " + segment.count + "\nusers: "+segment.userCount;
				} else {
					
    				var count:Number = segment.count;
    				
					if (timeline.showPhoto!=null)
						for each (var p:Photo in segment.photos)
							if (p.owner != timeline.showPhoto.owner)
								count--;
					number.text = "photos: " + count;
				}
				number.x = (segment.start.time-timeline.minTime.time)*frac;
				number.y = Math.max(3,height*(1-segment.count/max)-20);
				number.visible = true;
			});
			bar.addEventListener(MouseEvent.MOUSE_OUT,function():void {
				bar.alpha = 0.3;
				number.visible = false;
			});
			bar.addEventListener(MouseEvent.CLICK,function():void {
				if (timeline.showPhoto!=null) return;
				scrubStart.x = Math.max(0, bar.x-20);
				scrubEnd.x = Math.min(width-10,bar.x+bar.width+10);
				updateScrubber();
				deepStart();
				deepEnd();
				timeline.repaint();
			});
			addChild(bar);
			bars.push(bar);
			
		}
		protected function updateScrubber(...args):void {
			scrubFill.x = scrubStart.x + 10;
			scrubFill.graphics.clear();
			scrubFill.graphics.beginFill(0xFFFFFF,0.3);
			scrubFill.graphics.drawRect(0,0,scrubEnd.x-(scrubStart.x+10),height);
			scrubFill.width = scrubEnd.x-scrubStart.x-10;
			trace(scrubStart.x+10,0,scrubEnd.x-scrubStart.x-10,height);
			scrubFill.graphics.endFill();
			trace( width/(scrubEnd.x+10-scrubStart.x));
			if (timeline.maxTime != null) {
				var b:Boolean = timeline.readMouse;
				timeline.readMouse = true;
				timeline.filterStartTime = new Date((scrubStart.x / width) * (timeline.maxTime.time-timeline.minTime.time) + timeline.minTime.time);
				timeline.filterEndTime = new Date((scrubEnd.x+10) / width * (timeline.maxTime.time-timeline.minTime.time) + timeline.minTime.time);
				timeline.repaint();
				timeline.readMouse = b;
			}
		}
		private function deepStart():void {
			deepLinkManager.setParameter("start",timeline.filterStartTime.time);
		}
		private function deepEnd():void {
			deepLinkManager.setParameter("end",timeline.filterEndTime.time);
		}
		
		public var timeline:TimelineView = null;
		
		public var scrubStart:Sprite, scrubEnd:Sprite, scrubFill:Sprite;
		private var number:TextField = new TextField();
		
		override protected function paint(g:Graphics):void {
			g.clear();
			g.beginFill(0);
			g.drawRect(0,0,width,height);
			g.endFill();
			
		
    		if (timeline.minTime!=null && timeline.maxTime!=null) {
    			
    			var max:Number = 0;
    			
    			var frac:Number = width/(timeline.maxTime.time-timeline.minTime.time);
    			for each (var segment:Object in timeline.segments) {
	    			var x:Number = (segment.start.time-timeline.minTime.time)*frac;
	    			
    				g.lineStyle(2,0xFFFFFF,segment.importance);
    				g.moveTo(x,0);
    				g.lineTo(x,height);
    				
    				var count:Number = segment.count;
    				
					if (timeline.showPhoto!=null)
						for each (var p:Photo in segment.photos)
							if (p.owner != timeline.showPhoto.owner)
								count--;
    				if (count>max)
    					max = count;
    				/*
    				g.beginFill(0x303030,0.2);
    				g.lineStyle(0);
    				g.drawRect(x+2,height*(1-(segment.count/max)), x+frac*segment.duration-2,height);
    				g.endFill();*/
    			}
    			/*
    			//max = Math.log(max) * 1.05;
    			g.lineStyle(4,0xFFFFFF,0.5);
    			g.moveTo(0,height);
    			max *= 1.05;
    			//max = Math.log(max);
    			for each (segment in timeline.segments) {
	    			x = (segment.start.time-timeline.minTime.time)*frac;
	    			g.lineStyle(0,0,0);
	    			g.beginFill(0x808000,0.5);
	    			
    				count = segment.count;
    				
					if (timeline.showPhoto!=null)
						for each (var p:Photo in segment.photos)
							if (p.owner != timeline.showPhoto.owner)
								count--;
	    			g.drawRect(x,height*(1-count/max),frac*segment.duration,height*(segment.count/max));
	    			g.endFill();
    				//g.lineTo(x+frac*segment.duration/2,height*(1-segment.count/max));//(Math.log(segment.count) / max));
    			}
    			*/
    			if (timeline.scrubDate != null) {
	    			var realX:Number = (timeline.scrubDate.time-timeline.minTime.time)*width/(timeline.maxTime.time-timeline.minTime.time);
		    		g.lineStyle(2,0xFF0000,0.5);
		    		g.moveTo(realX,0);
	    			g.lineTo(realX,height);
	    		}
    		}
    		layoutBars();
		}
		
	}
}