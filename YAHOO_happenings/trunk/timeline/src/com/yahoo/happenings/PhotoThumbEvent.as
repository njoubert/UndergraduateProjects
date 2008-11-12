package com.yahoo.happenings
{
	import flash.events.Event;
	public class PhotoThumbEvent extends Event
	{
	public static const TIMECHANGE:String = "timeChange";
    public static const CLOSE:String = "close";
    
    public var photo:Photo;
    
    public function PhotoThumbEvent(type:String)
    {
        super(type);
    }
    
    override public function clone():Event
    {
        return new PhotoThumbEvent(type);
    }
	}
}