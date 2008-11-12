<?php

// Parsing Yahoo! REST Web Service results using
// unserialize. PHP4/PHP5
// Author: Jason Levitt
// February 1, 2006

error_reporting(E_ALL);



$dbpassword = ysecure_get_key('ztroot');
$link = mysql_connect('localhost', 'ztroot', $dbpassword);
if (!$link) {
	die('Could not connect: ' . mysql_error());
}

$db_selected = mysql_select_db('happenings', $link);
if (!$db_selected) {
	die ('Cannnot use happenings : ' . mysql_error());
}

$eventid = "192988";
if(isset($_GET['eventid'])) $eventid = $_GET['eventid'];
$viewall = false;
if(isset($_GET['viewall'])) $viewall = true;

// output=php means that the request will return serialized PHP
$url =  "http://dev1.brl.corp.yahoo.com:8181/rahul/getEventMedia.php?eventid=$eventid";

$user_agent = "Mozilla/5.0 (Macintosh; U; PPC Mac OSX; en-us)";
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_USERAGENT, $user_agent);
curl_setopt ($ch, CURLOPT_FOLLOWLOCATION, 1);
curl_setopt ($ch, CURLOPT_CONNECTTIMEOUT, 30);
curl_setopt ($ch, CURLOPT_TIMEOUT, 30);
curl_setopt ($ch, CURLOPT_RETURNTRANSFER, CURLAUTH_BASIC);

$response = curl_exec ($ch);
curl_close ($ch);

if ($response === false) {
    die("Media crawler failed <a href='save_media.php?eventid=$eventid'>Try again</a>");
}

$phpobj = unserialize($response);


$name = $phpobj['event']['name'];
$desc = $phpobj['event']['description'];
$tags = $phpobj['event']['tags'];
$start_date = $phpobj['event']['start_date'];
$start_time = $phpobj['event']['start_time'];
$end_date = $phpobj['event']['end_date'];
$end_time = $phpobj['event']['end_time'];
$venue = $phpobj['event']['venue_name'];
$address = $phpobj['event']['venue_address'];
$city = $phpobj['event']['venue_city'];
$state = $phpobj['event']['venue_state_code'];
$artists = "";
foreach($phpobj['event']['artists'] as $artist) {
    $artists .= $artist." ";
}
$keywords = "";
foreach($phpobj['event']['keyword'] as $keyword) {
    $keywords .= $keyword." ";
}


$sql = "SELECT count(id) as count from events WHERE upcomingid='$eventid';";
$rs = mysql_query($sql);
if (!$rs) {
	echo mysql_error() . " query:$sql";
}
$row = mysql_fetch_assoc($rs);
$count = $row['count'];

echo "<p>Crawling media for $name $start_date $venue $city, $state</p>";

if($count == 0) {
	$sql = "INSERT INTO `events` (`upcomingid` , `name` , `description` , `tags` , `start_date` , `start_time` , `end_date` , `end_time` , `venue` , `venue_address` , `city` , `state` , `artists` , `keywords` ) VALUES ('$eventid', '$name', '$desc', '$tags', '$start_date', '$start_time', '$end_date', '$end_time', '$venue', '$address', '$city', '$state', '$artists', '$keywords');";
	mysql_query($sql);
	echo "New event, added to database";
} else {
	echo "This event is already in the database, checking for new media since last crawl...";
}

$tobepruned = 0;
$newphotos=0;
echo "<h2>Photos</h2>";
if(!isset($phpobj['media']['flickr']['photos']['photo'])) {
	echo "No photos available";
} else {
	$i=0;
	foreach($phpobj['media']['flickr']['photos']['photo'] as $photo) {
		$i++;
		$photo_id = $photo['flickrid'];
		$farm  = $photo['farm'];
		$owner_id = $photo['owner'];
		$server = $photo['server'];
		$secret = $photo['secret'];
		$owner_name = $photo['ownername'];
		$date_taken = $photo['datetaken'];
		$date_upload = $photo['dateupload'];
		$score = $photo['score'];
		$tags = "";
		if(isset($photo['tags']))
		foreach($photo['tags'] as $tag) {
			$tags .= $tag." ";
		}


		$sql = "SELECT count(id) as count from photos WHERE eventid='$eventid' and photo_id='$photo_id';";
		$rs = mysql_query($sql);
		if (!$rs) {
			echo mysql_error() . " query:$sql";
		}
		$row = mysql_fetch_assoc($rs);
		$count = $row['count'];

		if($count == 0) {
			$newphotos++;
			$sql = "INSERT INTO `photos` (`eventid` , `photo_id` , `farm` , `owner_id` , `server` , `secret` , `owner_name` , `date_taken` , `date_upload` , `score` , `tags` ) VALUES ($eventid, '$photo_id', $farm, '$owner_id', $server, '$secret', '$owner_name', '$date_taken', '$date_upload', $score, '$tags');";
			mysql_query($sql);
		}
	}
}

$sql = "SELECT count(id) as count from photos WHERE eventid='$eventid';";
$rs = mysql_query($sql);
if (!$rs) {
	echo mysql_error() . " query:$sql";
}
$row = mysql_fetch_assoc($rs);
$totalphotos = $row['count'];

$sql = "SELECT count(id) as count from photos WHERE eventid='$eventid' and prune=-1;";
$rs = mysql_query($sql);
if (!$rs) {
	echo mysql_error() . " query:$sql";
}
$row = mysql_fetch_assoc($rs);
$tobepruned = $row['count'];


$sql = "SELECT count(id) as count from photos WHERE eventid='$eventid' and prune>-1;";
$rs = mysql_query($sql);
if (!$rs) {
	echo mysql_error() . " query:$sql";
}
$row = mysql_fetch_assoc($rs);
$pruned = $row['count'];

echo "<p><b>Found $newphotos new photos. Now have $totalphotos total photos. $tobepruned still need to be pruned. $pruned have been pruned so far.</b></p>";

if($tobepruned > 0) {
	echo "<p><b><a href='prune_photos.php?eventid=$eventid'>Prune Photos</a></b></p>";
}

if($pruned > 0) {
	echo "<p><b><a href='prune_photos.php?eventid=$eventid&view_pruned=true'>View all pruned photos</a></b></p>";
}

$tobepruned = 0;
$newvideos=0;
echo "<h2>Videos</h2>";
if(!isset($phpobj['media']['youtube']['videos']['video'])) {
	echo "No videos available";
} else {
	$i=0;
	foreach($phpobj['media']['youtube']['videos']['video'] as $video) {
		if(isset($video['id']) && strlen($video['id']) > 1) {
			$i++;
			$id = $video['id'];
			$author = $video['author'];
			$title = mysql_real_escape_string($video['title']);
			$length = $video['length_seconds'];
			$rating = $video['rating_avg'];
			$rating_count = $video['rating_count'];
			$description = mysql_real_escape_string($video['description']);
			$view_count = $video['view_count'];
			$upload_time = $video['upload_time'];
			$comment_count = $video['comment_count'];
			$tags = mysql_real_escape_string($video['tags']);
			$url = $video['url'];
			$thumb = $video['thumbnail_url'];

			$sql = "SELECT count(id) as count from videos WHERE eventid='$eventid' and yt_id='$id';";
			$rs = mysql_query($sql);
			if (!$rs) {
				echo mysql_error() . " query:$sql";
			}
			$row = mysql_fetch_assoc($rs);
			$count = $row['count'];

			if($count == 0) {
				$newvideos++;
				$sql = "INSERT INTO `videos` (`eventid` , `yt_id` , `title` , `author` , `length_seconds` , `rating_avg` , `rating_count` , `description` , `view_count` , `upload_time` , `comment_count` , `tags` , `url` , `thumb` ) VALUES ($eventid, '$id', '$title', '$author', $length, $rating, $rating_count, '$description', $view_count, $upload_time, $comment_count, '$tags', '$url', '$thumb');";
				mysql_query($sql);
			}
		}
	}
}
$sql = "SELECT count(id) as count from videos WHERE eventid='$eventid';";
$rs = mysql_query($sql);
if (!$rs) {
	echo mysql_error() . " query:$sql";
}
$row = mysql_fetch_assoc($rs);
$totalvideos = $row['count'];

$sql = "SELECT count(id) as count from videos WHERE eventid='$eventid' and prune=-1;";
$rs = mysql_query($sql);
if (!$rs) {
	echo mysql_error() . " query:$sql";
}
$row = mysql_fetch_assoc($rs);
$tobepruned = $row['count'];

$sql = "SELECT count(id) as count from videos WHERE eventid='$eventid' and prune>-1;";
$rs = mysql_query($sql);
if (!$rs) {
	echo mysql_error() . " query:$sql";
}
$row = mysql_fetch_assoc($rs);
$pruned = $row['count'];

echo "<p><b>Found $newvideos new videos. Now have $totalvideos total videos. $tobepruned still need to be pruned. $pruned have been pruned so far.</b></p>";

if($tobepruned > 0) {
	echo "<p><b><a href='prune_videos.php?eventid=$eventid'>Prune Videos</a></b></p>";
}

if($pruned > 0) {
	echo "<p><b><a href='prune_videos.php?eventid=$eventid&view_pruned=true'>View all pruned videos</a></b></p>";
}

//echo '<pre>';
//print_r($phpobj);
//echo '</pre>';

?>