package com.yahoo.happenings
{
	import flash.display.Loader;
	import flash.display.Shape;
	import flash.events.MouseEvent;
	import flash.net.URLRequest;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;
	
	import mx.core.UIComponent;
	import mx.events.ResizeEvent;
	import flash.geom.Point;
	import flash.display.DisplayObjectContainer;
	import flash.display.Graphics;

	public class GraphicsComponent extends UIComponent
	{
		public function GraphicsComponent()
		{
			super();
			/*
			
			var l:Loader = getImage("http://zoraw.cellosoft.com/media/sandstone/header.jpg");
			l.x = 10;
			l.y = 0;
			addChild(l);
			l = getImage("http://zoraw.cellosoft.com/media/sandstone/header.jpg");
			l.x = 10;
			l.y = 100;
			addChild(l);
			
			l.addEventListener(MouseEvent.MOUSE_OVER,function():void {
				l.x+=10;
			});
			l.addEventListener(MouseEvent.MOUSE_OUT,function():void {
				l.x-=10;
			});
			
			var label:TextField = new TextField();
            label.autoSize = TextFieldAutoSize.LEFT;
            label.selectable = false;

            var format:TextFormat = new TextFormat();
            format.font = "Verdana";
            format.color = 0x000000;
            format.size = 30;

            label.defaultTextFormat = format;
            label.text = "FOOOOOO";
            addChild(label);
            */
            addEventListener(ResizeEvent.RESIZE,function():void {
            	
            	if (maskRect!=null)
            		removeChild(maskRect);
				maskRect = new Shape();
				maskRect.graphics.beginFill(0xFFFFFF);
				maskRect.visible = false;
				maskRect.graphics.drawRect(0,0,width,height);//p.x, p.y, width, height);
				maskRect.graphics.endFill();
				addChild(maskRect);
				mask = maskRect;
				
				
				repaint();
	        });
		}
		private var maskRect:Shape = null;
		public function repaint():void {
			paint(graphics);
		}
		protected function paint(graphics:Graphics):void {
			
		}
		/*
		public function getXY():Point {
			var x:Number = 0,y:Number = 0;
			var o:DisplayObjectContainer = this;
			while (o!=null) {
				x+=o.x;
				y+=o.y;
				o = o.parent;
			}
			return new Point(x,y);
		}*/
		
		public static function getImage(url:String):Loader {
			var ldr:Loader = new Loader();
			ldr.load(new URLRequest(url));
			return ldr;
		}
		/*
		override public function get width():Number {
			var w:Number = 0;
			for (var i=0; i<numChildren; i++) {
				var x:Number = getChildAt(i).width+getChildAt(i).x;
				if (w<x)
					w=x;
			}
			return w;
		}
		override public function get height():Number {
			var h:Number = 0;
			for (var i=0; i<numChildren; i++) {
				var y:Number = getChildAt(i).height+getChildAt(i).y;
				if (h<y)
					h=y;
			}
			return h;
		}
		*/
	}
}