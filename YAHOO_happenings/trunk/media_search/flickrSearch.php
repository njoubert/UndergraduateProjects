<?php
/*
flickr image search
http://dev1.brl.corp.yahoo.com:8181/rahul/flickrSearch.php
http://prod1.yrb.scd.yahoo.com/8rlv2l5An3TL2Qs/happenings/flickrSearch.php
*/





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

$eventid = 151760;

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

if ($debug) {show_keys($eventDetails);};
//erase from here
/*
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
	$eventDate 	=	$eventDetails['rsp']['event']['start_date'];
	$eventVenue =	$eventDetails['rsp']['event']['venue_name'];
	$eventCity 	=	$eventDetails['rsp']['event']['venue_city'];
	$eventDescription 	=	$eventDetails['rsp']['event']['description'];

	//$XMLbody = '' . array_to_xmlCD($eventDetails['rsp']['event'], 2, 'event') . '';
	$outputArray = $eventDetails['rsp']['event'];


	$artists[]=$eventName;
	//$XMLbody .= '<artists>' . array_to_xmlCD($artists, 2) . '</artists>';
	$outputArray['artists'] = $artists;


	$keywords[]='music';
	$keywords[]='show';
	$keywords[]='band';

	//$XMLbody .= '<keywords>' . array_to_xmlCD($keywords, 2) . '</keywords>';
	$outputArray['keyword'] = $keywords;



	//
}


$output = array();
$output['event'] = $outputArray;
$eventInput = serialize($output);

*/
//erase till here


//show_keys($eventDetails);
$eventName 	=	$eventDetails['event']['name'];
$eventDate 	=	$eventDetails['event']['start_date'];
$eventVenue =	$eventDetails['event']['venue_name'];
$eventCity 	=	$eventDetails['event']['venue_city'];
$eventDescription 	=	$eventDetails['event']['description'];
$eventArtists		=	$eventDetails['event']['artists'];

//now look for photographs on flickr
//echo("Photos of $eventName at $eventVenue($eventCity) on $eventDate<br>");

$resultPhotos = array();

//look for photos taken (exif time) within 2 days of the event
foreach($eventArtists as $artist)
{
	if ($useDateTaken)
	{
		//make the base flickr query
		$params = array(
		'api_key'	=> '3d1360eba4ca3201377b0251b535fa7e',
		'extras'	=> 'date_upload, date_taken, owner_name, icon_server, original_format, last_update, geo, tags, machine_tags',
		'format'	=> 'php_serial',
		'method'	=> 'flickr.photos.search',
		'page'		=> $page,
		'per_page'	=> 500,
		'sort'		=> 'relevance',
		'text'	=> "$artist",
		'min_taken_date'	=> date('Y-m-d', strtotime($eventDate) - 86400 * 0.5),
		'max_taken_date'	=> date('Y-m-d', strtotime($eventDate) + 86400 * 1.5),
		);
		$rsp_obj = flickrCall($params);
		
		//score the photographs
		$resultPhotos = scorePhotos($rsp_obj, $resultPhotos);
		//show_keys ($resultPhotos);
	}
	
	
	//look for photos uploaded within the date range (1 day before and 8 days after) of the event
	if ($useDateUpload)
	{
		//make the base flickr query
		$params = array(
		'api_key'	=> '3d1360eba4ca3201377b0251b535fa7e',
		'extras'	=> 'date_upload, date_taken, owner_name, icon_server, original_format, last_update, geo, tags, machine_tags',
		'format'	=> 'php_serial',
		'method'	=> 'flickr.photos.search',
		'page'		=> $page,
		'per_page'	=> 500,
		'sort'		=> 'relevance',
		'text'		=> "$artist",
		'min_upload_date'	=> strtotime($eventDate) - 84600,
		'max_upload_date'	=> strtotime($eventDate) + 84600 * 8,
		);
		$rsp_obj = flickrCall($params);
	
		$resultPhotos = scorePhotos($rsp_obj, $resultPhotos);
		//show_keys ($resultPhotos);
	}
	
	
	//search for photos with the event name and the city name
	if ($useCity)
	{
		//make the base flickr query
		$params = array(
		'api_key'	=> '3d1360eba4ca3201377b0251b535fa7e',
		'extras'	=> 'date_upload, date_taken, owner_name, icon_server, original_format, last_update, geo, tags, machine_tags',
		'format'	=> 'php_serial',
		'method'	=> 'flickr.photos.search',
		'page'		=> $page,
		'per_page'	=> 500,
		'sort'		=> 'relevance',
		'text'		=> "$artist $eventCity",
		'min_upload_date'	=> strtotime($eventDate) - 84600,
		'max_upload_date'	=> strtotime($eventDate) + 84600 * 7 * 8,
		);
		$rsp_obj = flickrCall($params);
	
		$resultPhotos = scorePhotos($rsp_obj, $resultPhotos);
		//show_keys ($resultPhotos);
	}
	
	
	//search for photos with the event name and the venue name
	if ($useVenue)
	{
		//make the base flickr query
		$params = array(
		'api_key'	=> '3d1360eba4ca3201377b0251b535fa7e',
		'extras'	=> 'date_upload, date_taken, owner_name, icon_server, original_format, last_update, geo, tags, machine_tags',
		'format'	=> 'php_serial',
		'method'	=> 'flickr.photos.search',
		'page'		=> $page,
		'per_page'	=> 500,
		'sort'		=> 'relevance',
		'text'		=> "$artist $eventVenue",
		'min_upload_date'	=> strtotime($eventDate) - 84600,
		'max_upload_date'	=> strtotime($eventDate) + 84600 * 7 * 8,
		);
		$rsp_obj = flickrCall($params);
	
		$resultPhotos = scorePhotos($rsp_obj, $resultPhotos);
		//show_keys ($resultPhotos);
	}

}//end for each artist


//sort the photos based on how many of the above parameters are satisfied
//$resultPhotos = scoreSortPhotos($resultPhotos);

//sort photos by time
$resultPhotos = timeSortPhotos($resultPhotos);
$outputHTML = '';
$tempusers = array();
$users = array();
$tags = array();
$outputArray = array();

//count # of photos per flickr user
foreach($resultPhotos as $photo)
{
	$tempusers[$photo['ownername']] += 1;
	if (!$userColour[$photo['ownername']])
	{
		//set a random colour for each user
		$userColour[$photo['ownername']] = dechex(rand(0,255)) . dechex(rand(0,255)) . dechex(rand(0,255));
	} 
}


foreach($resultPhotos as $photo)
{
	//omit users with only 1 photo
	if ($tempusers[$photo['ownername']] > 1)
	{
		$outputArray['photos']['photo'][] = $photo;
		//set the colour of the photo border to match the user
		$outputHTML .= '<a href="http://flickr.com/photos/' . $photo['owner'] . '/' . $photo['flickrid'] . '/"  target="_blank"><img style=\'border-color:' . $userColour[$photo['ownername']] . '\' src="http://farm' . $photo['farm'] . '.static.flickr.com/' . $photo['server'] . '/' . $photo['flickrid'] . '_' . $photo['secret'] . '_s.jpg" alt=""></a>';
		$users[$photo['ownername']] += 1;
		if ($photo['tags'])
		{
			foreach ($photo['tags'] as $tag)
			{
				$tags[$tag] += 1;
			}
		}
		if ($photo['machine_tags'])
		{
			foreach ($photo['machine_tags'] as $tag)
			{
				$tags[$tag] += 1;
			}
		}
	}
	//echo($photoCount);
}






$photoCount = count($resultPhotos);
arsort($tags);
$tagCount = count($tags);
arsort($users);
$userCount = count($users);

/*
echo ("Showing $photoCount photos<br>");
if ($userCount > 0)
{
	echo("$userCount users<br><b>Top users:</b>");
	foreach ($users as $user => $count)
	{
		//list the flickr user names of each user (colour coded to photo border) along with the number of photos they took
		echo(" <font color='$userColour[$user]'>$user($count)</font>");
	}
	echo('<br>');
}
if ($tagCount > 0)
{
	//list the top tags at the event
	echo("$tagCount tags<br><b>Top tags:</b>");

	foreach ($tags as $tag => $count)
	{
		if ($count > 0) {echo(" <a href='$existingURL+" . urlencode($tag) . "'>$tag($count)</a>,");}
	}
	echo('<br>');
}
echo($outputHTML);
*/


echo(serialize($outputArray));

//show_keys(unserialize(serialize($outputArray)));


function timeSortPhotos($inputPhotos)
{
	$rank = 0;
	foreach ($inputPhotos as $photo)
	{
		$rank +=1;
		$resultPhotos[$rank] = $photo;
	}

	for ($i = 1; $i <= $rank-1; $i++)
	{
		for ($j = 1; $j <= $rank-1-$i; $j++)
		{
			if( $resultPhotos[$j+1]['datetaken'] < $resultPhotos[$j]['datetaken'] )
			{
				$temp = $resultPhotos[$j];
				$resultPhotos[$j] = $resultPhotos[$j+1];
				$resultPhotos[$j+1] = $temp;
			}
		}
	}

	return $resultPhotos;
}


function scoreSortPhotos($inputPhotos)
{
	$rank = 0;
	foreach ($inputPhotos as $photo)
	{
		$rank +=1;
		$resultPhotos[$rank] = $photo;
	}
	
	for ($i = 1; $i <= $rank-1; $i++)
	{
		for ($j = 1; $j <= $rank-1-$i; $j++)
		{
			if( $resultPhotos[$j+1]['score'] > $resultPhotos[$j]['score'] )
			{
				$temp = $resultPhotos[$j];
				$resultPhotos[$j] = $resultPhotos[$j+1];
				$resultPhotos[$j+1] = $temp;
			}
		}
	}
	
	return $resultPhotos;
}





function scorePhotos($rsp_obj, $resultPhotos)
{
	if ($rsp_obj['stat'] == 'ok')
	{
		$photoCount = $rsp_obj['photos']['total'];
		if ($photoCount > 500) {$photoCount = 500;}
	
		$counter = $photoCount;
		foreach($rsp_obj['photos']['photo'] as $photo)
		{
			$resultPhotos[$photo['id']]['flickrid'] = $photo['id'];
			$resultPhotos[$photo['id']]['farm'] = $photo['farm'];
			$resultPhotos[$photo['id']]['owner'] = $photo['owner'];
			$resultPhotos[$photo['id']]['server'] = $photo['server'];
			$resultPhotos[$photo['id']]['secret'] = $photo['secret'];
			$resultPhotos[$photo['id']]['ownername'] = $photo['ownername'];
			$resultPhotos[$photo['id']]['datetaken'] = $photo['datetaken'];
			$resultPhotos[$photo['id']]['dateupload'] = $photo['dateupload'];
			$resultPhotos[$photo['id']]['score'] += $counter;
				
			$counter -=1;
				
			$outputHTML .= '<a href="http://flickr.com/photos/' . $photo['owner'] . '/' . $photo['id'] . '/"><img src="http://farm' . $photo['farm'] . '.static.flickr.com/' . $photo['server'] . '/' . $photo['id'] . '_' . $photo['secret'] . '_s.jpg" alt=""></a>';
			$users[$photo['ownername']] += 1;
			if ($photo['tags'])
			{
				$taglist = explode(' ', $photo['tags']);
				foreach ($taglist as $tag)
				{
					$resultPhotos[$photo['id']]['tags'][$tag] = $tag;
					$tags[$tag] += 1;
				}
			}
			if ($photo['machine_tags'])
			{
				$taglist = explode(' ', $photo['machine_tags']);
				foreach ($taglist as $tag)
				{
					$resultPhotos[$photo['id']]['machine_tags'][$tag] = $tag;
					$tags[$tag] -= 1;
				}
			}
			//echo($photoCount);
		}//end for each photo
	}//endif ok
	
	return $resultPhotos;
}







	
	
function getLocationCoordinates($locationString, $server = 'geo.intl.yahoo.com', $port = 4080, $path = '/RPC2', $username = 'research', $password = 'severn')
{
	global $commondir;
	global $debug;
	include_once($commondir . 'xmlrpc-2.2/lib/xmlrpc.inc');
	include_once($commondir . "utf8_to_ascii/utf8_to_ascii.php");

	$client=new xmlrpc_client($path, $server, $port);
	$client->setCredentials('research', 'severn');

	$msg=new xmlrpcmsg('locationprobability.calculate', array(new xmlrpcval("any id", "string"), new xmlrpcval($locationString, "string")));

	//print "<pre>Sending the following request:\n\n" . htmlentities($msg->serialize()) . "\n\nDebug info of server data follows...\n\n</pre>";


	$resp=&$client->send($msg);

	if(!$resp->faultCode())
	{
		//print "value: <pre>" . htmlspecialchars($resp->serialize()) . '</pre>';
		$value=$resp->value();
		$phpval = php_xmlrpc_decode($value);
		//show_keys($phpval);
		$result = $phpval['contents']['0'];
		if (!$result){return false;}
		if ($debug==1) { show_keys($result);}
		
		$returnCoordinates['name'] = $result['name'];
		$returnCoordinates['type'] = $result['placetypename'];
		$returnCoordinates['city'] = $result['posttown'];
		$returnCoordinates['state'] = $result['state'];
		$returnCoordinates['country'] = $result['country'];
		$returnCoordinates['zipcode'] = $result['zip'];
		
		
		
		$returnCoordinates['latitude'] = $result['centroid']['latitude'];
		$returnCoordinates['longitude'] = $result['centroid']['longitude'];
			
		$returnCoordinates['startLat'] = $result['box']['swcorner']['latitude'];
		$returnCoordinates['endLat'] = $result['box']['necorner']['latitude'];
		$returnCoordinates['startLong'] = $result['box']['swcorner']['longitude'];
		$returnCoordinates['endLong'] = $result['box']['necorner']['longitude'];
			
		//show_keys($returnCoordinates);
		return $returnCoordinates;
	}
	else
	{
		//print "value: <pre>" . htmlspecialchars($resp->faultCode()) . '</pre>';
		//echo($resp->faultCode());
	}
	return false;
}
	


function escapeXML($string)	
{
		$string = str_replace("&", "&amp;", $string);
		$string = str_replace("'", "&apos;", $string);
		$string = str_replace('"', "&quot;", $string);
		$string = str_replace("<", "&lt;", $string);
		$string = str_replace(">", "&gt;", $string);
		
		return $string;
}


function flickrCall($params)
{
	global $debug;
	$encoded_params = array();
	$rawString = '';
	ksort($params);
	foreach ($params as $k => $v){
	
		$encoded_params[] = urlencode($k).'='.urlencode($v);
		$rawString .= $k.$v;
	}
	
	
	#
	# call the API and decode the response
	#
	
	$rawString = 'e3e425108ed8d540' . $rawString;

	$api_sig = md5($rawString);

	$url = "http://api.flickr.com/services/rest/?".implode('&', $encoded_params) . "&api_sig=$api_sig";
	if ($debug) {echo($url . '<br>');}
	//$rsp = returnURL($url);
	
	$user_agent = "Mozilla/5.0 (Macintosh; U; PPC Mac OSX; en-us)";
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, $url);
	curl_setopt($ch, CURLOPT_USERAGENT, $user_agent);
	curl_setopt ($ch, CURLOPT_FOLLOWLOCATION, 1);
	curl_setopt ($ch, CURLOPT_CONNECTTIMEOUT, 30);
	curl_setopt ($ch, CURLOPT_TIMEOUT, 90);
	curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
	$rsp = curl_exec ($ch);
	curl_close ($ch);
	
	$rsp_obj = unserialize($rsp);
	
	return $rsp_obj;
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

