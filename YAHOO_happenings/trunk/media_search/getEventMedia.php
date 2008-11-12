<?php
/*
event media search
http://dev1.brl.corp.yahoo.com:8181/rahul/getEventMedia.php
http://dev1.brl.corp.yahoo.com:9999/internal/rahul/getEventMedia.php
http://prod1.yrb.scd.yahoo.com/8rlv2l5An3TL2Qs/happenings/getEventMedia.php

ozzfest=62898
BFD 2007=168964
*/


error_reporting(0);

$eventid 		= 	yahoo_get_data(YIV_GET,'eventid',YIV_FILTER_NUMBER);//upcoming event id
$output			=	$_GET['output'];
$debug 			= 	yahoo_get_data(YIV_GET,'debug',YIV_FILTER_NUMBER);//debug bit set to 1 to see the workings of the script

if ($debug ==1) {error_reporting(E_ERROR | E_WARNING | E_PARSE);}
//set default event id to iron maiden
if (!$eventid) { $eventid = 151760; }
//set default output to serial php
if (!$output) { $output = 'php'; }


$baseHappeningsAPI = 'http://zonetag.research.yahoo.com/8rlv2l5An3TL2Qs/happenings/';

//start by getting the details of the event
$eventDataURL = $baseHappeningsAPI . 'eventInfo.php?eventid=' . $eventid;
if ($debug ==1) {echo($eventDataURL);}
$eventDetailsSerial = downloadURL($eventDataURL);
$eventDetails = unserialize($eventDetailsSerial);
if ($debug ==1) {show_keys($eventDetails);}



//get flickr pictures for the event
$flickrSearchURL = $baseHappeningsAPI . 'flickrSearch.php?eventInput=' . urlencode($eventDetailsSerial);
if ($debug ==1) {echo($flickrSearchURL);}
$flickrPhotos = downloadURL($flickrSearchURL);
$flickrPhotos = unserialize($flickrPhotos);
if ($debug ==1) {show_keys($flickrPhotos);}


//get youtube videos for the event
$youtubeSearchURL = $baseHappeningsAPI . 'youtubeSearch.php?eventInput=' . urlencode($eventDetailsSerial);
if ($debug ==1) {echo($youtubeSearchURL);}
$youtubeVideos = downloadURL($youtubeSearchURL);
$youtubeVideos = unserialize($youtubeVideos);

if ($debug ==1) {show_keys($youtubeVideos);}


$outputArray['event'] = $eventDetails['event'];
$outputArray['media']['flickr'] = $flickrPhotos;
$outputArray['media']['youtube'] = $youtubeVideos;


if ($debug ==1) {show_keys($outputArray);}

switch($output)
{
	case 'php':
		echo (serialize($outputArray));
		break;
	
	case 'json':
		echo (array2json($outputArray));
		break;
}


function array2json($arr) {
	$parts = array();
	$is_list = false;

	//Find out if the given array is a numerical array
	$keys = array_keys($arr);
	$max_length = count($arr)-1;
	if(($keys[0] == 0) and ($keys[$max_length] == $max_length)) {//See if the first key is 0 and last key is length - 1
		$is_list = true;
		for($i=0; $i<count($keys); $i++) { //See if each key correspondes to its position
			if($i != $keys[$i]) { //A key fails at position check.
				$is_list = false; //It is an associative array.
				break;
			}
		}
	}

	foreach($arr as $key=>$value) {
		if(is_array($value)) { //Custom handling for arrays
			if($is_list) $parts[] = array2json($value); /* :RECURSION: */
			else $parts[] = '"' . $key . '":' . array2json($value); /* :RECURSION: */
		} else {
			$str = '';
			if(!$is_list) $str = '"' . $key . '":';

			//Custom handling for multiple data types
			if(is_numeric($value)) $str .= $value; //Numbers
			elseif($value === false) $str .= 'false'; //The booleans
			elseif($value === true) $str .= 'true';
			else $str .= '"' . addslashes($value) . '"'; //All other things
			// :TODO: Is there any more datatype we should be in the lookout for? (Object?)

			$parts[] = $str;
		}
	}
	$json = implode(',',$parts);

	if($is_list) return '[' . $json . ']';//Return numerical JSON
	return '{' . $json . '}';//Return associative JSON
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

//function to expand an array and show_keys
function show_keys($ar)
{
	echo "<table width='100%' border='1' bordercolor='#6699CC' cellspacing='0' cellpadding='5'><tr valign='top'>";
	foreach ($ar as $k => $v )
	{
		echo "<td align='center' bgcolor='#EEEEEE'>
	       <table border='2' cellpadding='3'><tr><td bgcolor='#FFFFFF'><font face='verdana' size='1'>
		 " . $v . "(" . $k . ")
	       </font></td></tr></table>";
		if (is_array($ar[$k])) {
		 show_keys ($ar[$k]);
		}
		echo "</td>";
	}
	echo "</tr></table>";
}//end function show_keys
?>

