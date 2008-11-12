<?php
/*
flickr image search
http://dev1.brl.corp.yahoo.com:8181/rahul/youtubeSearch.php
http://prod1.yrb.scd.yahoo.com/8rlv2l5An3TL2Qs/happenings/youtubeSearch.php?debug=1
* */





include_once '/home/y/share/htdocs/ztcommon/utils.inc';
//error_reporting(E_ERROR | E_WARNING | E_PARSE);
error_reporting(0);

$eventInput		=	$_GET['eventInput'];
$eventid 		= 	yahoo_get_data(YIV_GET,'eventid',YIV_FILTER_NUMBER);
$useCity		= 	yahoo_get_data(YIV_GET,'useCity',YIV_FILTER_NUMBER);//use Location?
$useVenue 		= 	yahoo_get_data(YIV_GET,'useVenue',YIV_FILTER_NUMBER);//use date?
$useDateTaken 	= 	yahoo_get_data(YIV_GET,'useDateTaken',YIV_FILTER_NUMBER);//use Location?
$useDateUpload	= 	yahoo_get_data(YIV_GET,'useDateUpload',YIV_FILTER_NUMBER);//use Location?

$debug 			= 	yahoo_get_data(YIV_GET,'debug',YIV_FILTER_NUMBER);//debug bit
//$debug=1;

if ($debug ==1) {error_reporting(E_ERROR | E_WARNING | E_PARSE);}

$useCity		= 	1;
$useVenue 		= 	1;
$useDateTaken 	= 	1;
$useDateUpload	= 	1;
$useExtracted	= 	1;
if (!$eventid) { $eventid = 151760; }
if (!$output) { $output = 'php'; }



if (!$eventInput)
{
	$eventDataURL = 'http://prod1.yrb.scd.yahoo.com/8rlv2l5An3TL2Qs/happenings/eventInfo.php?eventid=' . $eventid;
	if ($debug ==1) {echo($eventDataURL);}
	$eventDetails = unserialize(downloadURL($eventDataURL));
}
else
{
	$eventInput = htmlspecialchars_decode($eventInput);
	$eventDetails = unserialize($eventInput);
}



if ($debug ==1) {show_keys($eventDetails);}

$eventName 	=	$eventDetails['event']['name'];
$eventDate 	=	$eventDetails['event']['start_date'];
$eventVenue =	$eventDetails['event']['venue_name'];
$eventCity 	=	$eventDetails['event']['venue_city'];
$eventDescription 	=	$eventDetails['event']['description'];
$eventArtists		=	$eventDetails['event']['artists'];


//get the youtube videos of the event
foreach($eventArtists as $artist)
{
	$page=1;
	$youtubeBaseURL = "http://www.youtube.com/api2_rest?method=youtube.videos.list_by_tag&dev_id=BTcmAmlosuY&page=$page&per_page=100&tag=";
	
	$videoList = returnParsedXML($youtubeBaseURL . urlencode($artist . ' ' . $eventCity));
	if ($debug ==1) {echo($youtubeBaseURL . urlencode($artist . ' ' . $eventCity) . '<br>');}
	
	//show_keys($videoList);
	
	$outputArray = array();
	foreach($videoList['ut_response']['video_list']['video'] as $video)
	{
		$outputArray['videos']['video'][] = $video;
		//echo ("<a href='" . $video['url'] . "' target='_blank'><img src='" . $video['thumbnail_url'] . "'>" . $video['title'] . " ($video[length_seconds])</a>. Uploaded on " . date('Y-m-d' , $video['upload_time']) . '<br>');
	}
	
	$totalVideos = $videoList['ut_response']['video_list']['total'];
	if ($totalVideos > 100)
	{
		for ($page=2; $page<=floor($totalVideos/100)+1; $page++)
		{
			$youtubeBaseURL = "http://www.youtube.com/api2_rest?method=youtube.videos.list_by_tag&dev_id=BTcmAmlosuY&page=$page&per_page=100&tag=";
			$videoList = returnParsedXML($youtubeBaseURL . urlencode($artist . ' ' . $eventCity));
			foreach($videoList['ut_response']['video_list']['video'] as $video)
			{
				$outputArray['videos']['video'][] = $video;
				//echo ("<a href='" . $video['url'] . "' target='_blank'><img src='" . $video['thumbnail_url'] . "'>" . $video['title'] . " ($video[length_seconds])</a>. Uploaded on " . date('Y-m-d' , $video['upload_time']) . '<br>');
			}
		}
	}
}



//show_keys($outputArray);
//echo(count($outputArray['videos']['video']));
echo(serialize($outputArray));

//show_keys(unserialize(serialize($outputArray)));



function escapeXML($string)	
{
		$string = str_replace("&", "&amp;", $string);
		$string = str_replace("'", "&apos;", $string);
		$string = str_replace('"', "&quot;", $string);
		$string = str_replace("<", "&lt;", $string);
		$string = str_replace(">", "&gt;", $string);
		
		return $string;
}

function downloadURL($url)
{
	$user_agent = "Mozilla/5.0 (Macintosh; U; PPC Mac OSX; en-us)";
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, $url);
	curl_setopt($ch, CURLOPT_USERAGENT, $user_agent);
	curl_setopt ($ch, CURLOPT_FOLLOWLOCATION, 1);
	curl_setopt ($ch, CURLOPT_CONNECTTIMEOUT, 30);
	curl_setopt ($ch, CURLOPT_TIMEOUT, 300);
	curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
	$returnedData = curl_exec ($ch);
	curl_close ($ch);

	return $returnedData;
}

?>

