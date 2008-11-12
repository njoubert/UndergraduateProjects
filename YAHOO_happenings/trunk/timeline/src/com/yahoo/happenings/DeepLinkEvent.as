package com.yahoo.happenings
{
	import flash.events.Event;

	public class DeepLinkEvent extends Event
	{
		public static const CHANGE:String = "change";
		
		public function DeepLinkEvent(type:String, field:String, oldValue:Object, newValue:Object) {
			super(type);
			_field = field;
			_oldValue = oldValue;
			_newValue = newValue;
		}
		
		public function get field():String {
			return _field;
		}
		public function get oldValue():Object {
			return _oldValue;
		}
		public function get newValue():Object {
			return _newValue;
		}
		
		private var _field:String, _oldValue:Object, _newValue:Object;
		
		override public function clone():Event {
			return new DeepLinkEvent(this.type,this.field, this.oldValue, this.newValue);
		}
	}
}