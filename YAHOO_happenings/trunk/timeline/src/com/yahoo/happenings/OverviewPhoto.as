package com.yahoo.happenings
{
	import flash.display.Loader;
	import flash.net.URLRequest;
	import flash.display.Sprite;
	import mx.controls.Label;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;
	import caurina.transitions.Tweener;
	import flash.events.MouseEvent;
	import flash.events.Event;
	import com.yahoo.timetags.IVisualMediaItem;
	import flash.display.Shape;
	
	public class OverviewPhoto extends Photo {
		private var $mask:Shape;
		private var $border:Shape;
		private var $size:Number;
		private var $originalX:Number;
		private var $originalY:Number;
		
		public function OverviewPhoto(_node:IConcertRelatedItem,concert:Concert) {
			super(_node,concert);
			alpha = 0;
		}
		
		public function load():void {
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, $loadComplete);
			loader.load(new URLRequest(getImage("s")));
			addChild(loader);
		}
		
			
		private function $loadComplete(_e:Event):void {
			alpha = 0;
			//what side is the longest?
			var _longest_side:Number = Math.max(width, height);
			//what percentage is the longest side from $size?
		
			var _per:Number = _longest_side / $size;
			loader.scaleX = loader.scaleY = 1/_per;
			//trace(1/_per);
			$mask = new Shape();
			$mask.graphics.beginFill(0xFFFFFF);
			$mask.graphics.drawRect(-1,-1,$size+1,$size+1);
			addChild($mask);
			mask = $mask;
			Tweener.addTween(this, {alpha:1, time:.3});
			dispatchEvent(_e);
			buttonMode = true;
			$border = new Shape();
			$border.graphics.lineStyle(1,0xFFFF00,0,false,"none");
			$border.graphics.beginFill(0x000000,0);
			$border.graphics.drawRect(0,0,$size-2,$size-2);
			addChild($border);
			$border.visible = false;
		}
		
		public function set size(_arg:Number):void {
			$size = _arg;
		}
		public function get size():Number {
			return $size;
		}
		
		override public function set x(_arg:Number):void {
			super.x = _arg;
			$originalX = _arg;
		}
		
		override public function set y(_arg:Number):void {
			super.y = _arg;
			$originalY = _arg;
		}
		
		public function rollover():void {
			
			$border.visible = true;
			scaleX = scaleY = 1/loader.scaleX;
			x -= width/2;
			y -= height/2;
		}
		public function rollout():void {
			$border.visible = false;
			scaleX = scaleY = 1;
			x = $originalX;
			y = $originalY;
		}
		
	}
}