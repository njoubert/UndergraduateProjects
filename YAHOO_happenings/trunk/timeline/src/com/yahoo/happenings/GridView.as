package com.yahoo.happenings
{
	import mx.containers.Canvas;
	import flash.events.MouseEvent;
	import flash.events.Event;
	import mx.events.ResizeEvent;
	import flash.utils.Timer;
	import flash.events.TimerEvent;
	import com.yahoo.happenings.OverviewPhoto;
	import flash.display.Sprite;
	import flash.display.SpreadMethod;
	import mx.events.PropertyChangeEvent;
	import com.yahoo.timetags.IVisualMediaItem;
	import flash.text.TextField;
	import caurina.transitions.Tweener;
	

	public class GridView extends GraphicsComponent
	{
		private var $photos:Array;
		private var $timer:Timer;
		private var $holder:GraphicsComponent;
		private var $itemLength:Number;
		private var $itemSize:Number;
		private var $scrollDelta:Number;
		private var $rolledOverPhoto:OverviewPhoto;
		private var $photoCursor:Number;
		private var $activePhoto:OverviewPhoto;
		private var $originalWidth:Number;
		private var $originalHeight:Number;
		private var $thumbnail:PhotoThumbnail;
			
		[Bindable]
		public var concert:Concert = null;
		
		public function GridView()
		{
			super();
			$originalWidth = $originalHeight = 600;
			
			$photos = new Array();
			
			addEventListener("propertyChange",function(event:PropertyChangeEvent):void {
				if (event.property=="concert") {
					for each (var _item:IConcertRelatedItem in concert.relatedItems) {
						var _photo:OverviewPhoto = new OverviewPhoto(_item,concert);
						$photos.push(_photo);
	   				}
	   				//find the number of photos in the stack, and divide by two. this is the number of rows/cols we'll be using.
	   				if ($photos.length > 0) {
	   					$itemLength = Math.ceil(Math.sqrt($photos.length));
	   					$itemSize = ($originalWidth / $itemLength);
	   					$startPhotoLoad();
	   				}
   					
				}
			});
			$thumbnail = new PhotoThumbnail();
			$thumbnail.pseudoParent = this;
			$thumbnail.addEventListener(PhotoThumbEvent.CLOSE,function():void {
				$thumbnail.hide();
			});
			
		}

		
		/**
		 * Begins photo load iteration.
		 **/
		private function $startPhotoLoad():void {			
			$photoCursor = -1;
			$holder = new GraphicsComponent();
		
			addChild($holder);
			$holder.addChild($thumbnail);
			$thumbnail.addEventListener(PhotoThumbEvent.CLOSE,function():void {
				$thumbnail.hide();
			});
			$loadPhoto();
		}
		
		private function $loadPhoto():void {
			if (++$photoCursor < $photos.length) {
				var _p:OverviewPhoto = $photos[$photoCursor];
				_p.name = 'p_'+$photoCursor;
				_p.size = $itemSize;
				_p.addEventListener(Event.COMPLETE, $onPhotoLoaded);
				$holder.addChild(_p);				
				_p.load();
				
			} else {
				trace('media objects loaded.');
			}
			
		}
		
		private function $onPhotoLoaded(_e:Event):void {
			updateLayout();
			var _p:OverviewPhoto = _e.target as OverviewPhoto;
			_p.addEventListener(MouseEvent.ROLL_OVER, $photoRollOver);
			_p.addEventListener(MouseEvent.ROLL_OUT, $photoRollOut);
			_p.addEventListener(MouseEvent.CLICK, $photoClick);
			$loadPhoto();
		}
		
		/**
		 * OverviewPhoto event handlers
		**/
		
		private function $photoRollOver(_e:MouseEvent):void {
			if (!$thumbnail.visible) {
				var _p:OverviewPhoto = _e.currentTarget as OverviewPhoto;
				$rolledOverPhoto = _p;
				_p.rollover();
				$holder.setChildIndex(_p, $holder.numChildren-1);
			}
		}
		private function $photoRollOut(_e:MouseEvent):void {
			var _p:OverviewPhoto = _e.currentTarget as OverviewPhoto;
			_p.rollout();
			$rolledOverPhoto = null;
	
		}
		
		
		private function $photoClick(_e:MouseEvent):void {
			$photoRollOut(_e);
			var _p:OverviewPhoto = _e.currentTarget as OverviewPhoto;
			$thumbnail.photo = _p;			
			$thumbnail.show();
		}
		
		
		

		
		
		public function updateLayout():void {
			for(var _i:Number = 0; _i<$photoCursor+1; _i++) {
				var _y:Number = Math.floor(_i/$itemLength) * ($itemSize);
				var _p:OverviewPhoto = $holder.getChildByName('p_'+_i) as OverviewPhoto;
				
				if (_p != null) {
					$holder.setChildIndex(_p, _i);
					_p.x = Math.floor(_i%$itemLength) * ($itemSize);
					_p.y = _y;
	
				}
			}
			if ($rolledOverPhoto != null) {
				$holder.setChildIndex($rolledOverPhoto, $holder.numChildren-1);
			} else {
				$holder.setChildIndex($thumbnail, $holder.numChildren-1);
			}
		}		
	}
}