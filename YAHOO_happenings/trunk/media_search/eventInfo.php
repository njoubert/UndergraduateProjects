<?php
/*
 http://dev1.brl.corp.yahoo.com:8181/rahul/eventInfo.php
http://dev1.brl.corp.yahoo.com:9999/internal/rahul/eventInfo.php
http://prod1.yrb.scd.yahoo.com/8rlv2l5An3TL2Qs/happenings/eventInfo.php?eventid=126855
*/





include_once '/home/y/share/htdocs/ztcommon/utils.inc';
//error_reporting(E_ERROR | E_WARNING | E_PARSE);
error_reporting(0);

$eventid 		= 	yahoo_get_data(YIV_GET,'eventid',YIV_FILTER_NUMBER);//use Location?$_GET['eventid'];//search term
$output 		= 	$_GET['output'];

$debug 			= 	yahoo_get_data(YIV_GET,'debug',YIV_FILTER_NUMBER);//debug bit
//$debug=1;

if ($debug ==1) {error_reporting(E_ERROR | E_WARNING | E_PARSE);}

if (!$eventid) { $eventid = 151760; }
if (!$output) { $output = 'php'; }

//start by getting the upcoming information about the event
$upcomingBaseURL = 'http://upcoming.yahoo.com/services/rest/?api_key=98197e6705&method=event.getInfo&event_id=';

$eventDetails = returnParsedXML($upcomingBaseURL . $eventid);
//show_keys($eventDetails);

if ($eventDetails['rsp']['event']['name'])
{
	$outputArray = array();
	//show_keys($eventDetails['rsp']['event']);
	unset($eventDetails['rsp']['event']['_attributes']);
	unset($eventDetails['rsp']['event']['_name']);
	$eventName 	=	$eventDetails['rsp']['event']['name'];
	$eventName 	=	$eventDetails['rsp']['event']['name'];
	$eventDate 	=	$eventDetails['rsp']['event']['start_date'];
	$eventVenue =	$eventDetails['rsp']['event']['venue_name'];
	$eventCity 	=	$eventDetails['rsp']['event']['venue_city'];
	$eventDescription 	=	$eventDetails['rsp']['event']['description'];
	
	//$XMLbody = '' . array_to_xmlCD($eventDetails['rsp']['event'], 2, 'event') . '';
	$outputArray = $eventDetails['rsp']['event'];
	
	
	//$artists[]=$eventName;
	//$XMLbody .= '<artists>' . array_to_xmlCD($artists, 2) . '</artists>';

	
	//http://lwbe01.search.re2.yahoo.com/annotate
	//http://lwd1.search.corp.yahoo.com/annotate

	//extract event information
	//$liveWordsURL = 'http://lwd1.search.corp.yahoo.com/annotate?text=' . urlencode($eventName) . '&output=xml';
	//echo("<a href='$liveWordsURL'>title keywords</a><br>");
	//$liveWordsURL = 'http://lwd1.search.corp.yahoo.com/annotate?text=' . urlencode($eventDescription) . '&output=xml';
	//echo("<a href='$liveWordsURL'>description keywords</a><br>");
	$liveWordsURL = 'http://lwbe01.search.re2.yahoo.com/annotate?text=' . urlencode($eventName . "\n" . $eventDescription) . '&output=xml';
	//echo("<a href='$liveWordsURL'>description + title keywords</a><br>");
	
	$livewords = returnParsedXML($liveWordsURL);
	if ($debug) {show_keys($livewords);}
	foreach($livewords['result']['annotationSet']['entity'] as $entity)
	{
		if ($entity['categorys']['category'] <> 'PLACE' AND isArtist($entity['text']['_value']))
		{
			$artists[]=$entity['text']['_value'];
		}
		//show_keys($entity);
	}

/*	
	$url = 'http://search.yahooapis.com/ContentAnalysisService/V1/termExtraction';

	$postVars['appid'] = 'CaaOmv3V34HxgbIJ67YI0DFiW1tsNj8o4FqF22gKcnbRbQIQ1T8Bq9SYJC928ktZgeB0Lw--';
	//$postVars['context'] = urlencode($eventName . "\n" . $eventDescription);
	$postVars['context'] = urlencode($eventName);
	
	$livewords = returnParsedXMLPost($url, $postVars); 
	//show_keys($livewords);
	foreach($livewords['ResultSet']['Result'] as $term)
	{
		if (isArtist($term))
		{
			$artists[]=$term;
		}
	}
*/

	if (!$artists)
	{
		$artists[] = $eventName;
	}
	
	
	
	
	
	
	$outputArray['artists'] = $artists;
	
	
	
	$keywords[]='music';
	$keywords[]='show';
	$keywords[]='band';
	
	//$XMLbody .= '<keywords>' . array_to_xmlCD($keywords, 2) . '</keywords>';
	$outputArray['keyword'] = $keywords;
	
	//show_keys($outputArray);
	/*$XMLbody = '<?xml version="1.0"?><rsp stat="ok"><event>' . $XMLbody . '</event></rsp>';*/
	
	//
}
else
{
	/*$XMLbody = '<?xml version="1.0" encoding="ISO-8859-1"?><rsp stat="fail"><err code="1" msg="Invalid usertoken" /> </rsp>';*/
}

$output = array();
$output['event'] = $outputArray;
echo(serialize($output));
if ($debug ==1) {show_keys($output);}

//displayXML($XMLbody);

die();



//function to download and parse an XML document given a URL
function returnParsedXMLPost($docURL, $postVars)
{
	$url=$docURL;
	$user_agent = "Mozilla/5.0 (Macintosh; U; PPC Mac OSX; en-us)";
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, $url);
	curl_setopt($ch, CURLOPT_USERAGENT, $user_agent);
	curl_setopt ($ch, CURLOPT_FOLLOWLOCATION, 1);
	curl_setopt ($ch, CURLOPT_CONNECTTIMEOUT, 30);
	curl_setopt ($ch, CURLOPT_TIMEOUT, 60);
	curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
	curl_setopt($ch, CURLOPT_POST, 1);
	curl_setopt($ch, CURLOPT_POSTFIELDS, $postVars);
	
	$xmlResults = curl_exec ($ch);
	curl_close ($ch);


	$xml_parser = new xml(false, true, true);
	$parsed_response = $xml_parser->parse($xmlResults);
	return $parsed_response;
}// end returnParsedXML




function isArtist($text)
{
	$artists = returnParsedXML('http://wsapi.us.music.yahoo.com/artist/v1/list/search/artist/' . urlencode($text));
	if(strtolower($artists['Artists']['Artist'][0]['name']) == strtolower($text) or strtolower($artists['Artists']['Artist']['name']) == strtolower($text))
	{

		return true;
	}
	return false;
}

function displayXML($XMLbody)
{
	if ($XMLbody)
	//	echo(strlen($XMLbody) . "<br>");
	header("content-type: text/xml");
	header("Content-Length: ".strlen($XMLbody));
	echo($XMLbody);
	die();
}




function array_to_xmlCD($array, $level=1 , $root ='') {
	$xml = '';
	if ($level==1) {
		$xml .= '<?xml version="1.0" encoding="ISO-8859-1"?>';
		if ($root) {$xml .= "\n<$root>\n";}
	}
	foreach ($array as $key=>$value) {
		$key = strtolower($key);
		if (is_array($value)) {
			$multi_tags = false;
			foreach($value as $key2=>$value2) {
				if (is_array($value2)) {
					$xml .= str_repeat("\t",$level)."<$key>\n";
					$xml .= array_to_xmlCD($value2, $level+1);
					$xml .= str_repeat("\t",$level)."</$key>\n";
					$multi_tags = true;
				} else {
					$xml .= str_repeat("\t",$level).
					"<$key>" . htmlspecialchars ($value2, ENT_QUOTES) . "</$key>\n";
					$multi_tags = true;
				}
			}
			if (!$multi_tags and count($value)>0) {
				$xml .= str_repeat("\t",$level)."<$key>\n";
				$xml .= array_to_xmlCD($value, $level+1);
				$xml .= str_repeat("\t",$level)."</$key>\n";
			}
		} else {
			if (trim($value)!='') {
				$xml .= str_repeat("\t",$level).
				"<$key>" . htmlspecialchars ($value, ENT_QUOTES) . "</$key>\n";
			}
		}
	}
	if ($level==1) {
		if ($root) {$xml .= "</$root>\n";}
	}
	return $xml;
}
?>

