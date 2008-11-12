<?php

if(isset($_GET['eventid'])) $eventid = $_GET['eventid'];

$view_pruned = false;
if(isset($_GET['view_pruned'])) {
	$view_pruned = true;
}

?>


<html>
<head>
</head>
<body>
<?php
include '/home/y/share/htdocs/ztcommon/commonvars.inc.php';

$log=fopen($logdir."log_happenings.txt", "a+");


$dbpassword = ysecure_get_key('ztroot');
if (!$dbpassword) {
    fwrite($log, "Could not get password");
}

$link = mysql_connect('localhost', 'ztroot', $dbpassword);
if (!$link) {
	fwrite($log,'$logID:Could not connect : ' . mysql_error()."\n\n");
	fclose($log);
	die('Could not connect: ' . mysql_error());
}

$db_selected = mysql_select_db('happenings', $link);
if (!$db_selected) {
	fwrite($log,'$logID:Cannnot use happenings : ' . mysql_error()."\n\n");
	fclose($log);
	die ('Cannnot use happenings : ' . mysql_error());
}

$sql = "SELECT count(yt_id) as count ".
				"FROM videos WHERE eventid = $eventid";
$rs = mysql_query($sql);
if (!$rs) {
    fwrite($log, mysql_error() . " query:$sql\n");
}
$row = mysql_fetch_assoc($rs);
$counttotalvideos = $row['count'];



if(isset($_GET["confirm"])) {

	$notselectedids = substr($_GET["notselectedids"], 0, -1);
	$idarr = explode(",", $notselectedids);
	foreach($idarr as $id) {
		$sql = "UPDATE videos SET prune = 0 WHERE yt_id = '$id' AND eventid = $eventid";
		mysql_query($sql);
	}
	echo "Your selections have been saved<p/>";
	echo "<a href='prune_videos.php?userid=$userid'>Go prune some more media!</a>";
	return;
}

$confirm=false;
if(isset($_GET["ids"])) {

        //$ids = split(",",$_GET["ids"]);
	$selectedids = substr($_GET["ids"], 0, -1);
	$idarr = explode(",", $selectedids);
	foreach($idarr as $id) {
		$sql = "UPDATE videos SET prune = 1 WHERE yt_id = '$id' AND eventid = $eventid";
		mysql_query($sql);
	}
	$notselectedids = substr($_GET["notselectedids"], 0, -1);
	$idarr = explode(",", $notselectedids);
	foreach($idarr as $id) {
		$sql = "UPDATE videos SET prune = 0 WHERE yt_id = '$id' AND eventid = $eventid";
		mysql_query($sql);
	}

	$sql2 = "SELECT count(yt_id) as countcomplete ".
			"FROM videos ".
			"WHERE prune != -1 and eventid = $eventid";
	$rs2 = mysql_query($sql2);
	$row2 = mysql_fetch_assoc($rs2);
	$countcomplete = $row2['countcomplete'];
	$percentcomplete = round(($countcomplete / $counttotalvideos) * 100, 2);

}

if(isset($_GET["ids"])) {
	echo "<p>Percent of photos for this event that have been pruned:<br/><img src='http://needles.guzzlingcakes.com/images/progress/progress.php?percent=".intval($percentcomplete)."' /></p>";
	echo "<p>Click to highlight videos that are not about this event</p>";

	if(!$view_pruned) {
		if($countcomplete == $counttotalvideos) {
			$confirm = true;
			echo "YOU ROCK!!! You have completed all the videos this event";
		} else {

			$praise = Array("Keep going, you can do it!",
											"Don't stop now, you've come this far!!!",
											"Come on, don't wimp out now!",
											"Your parents would be so proud right now if they know how many videos you have rated. Keep it up!",
											"Rah rah rah!!! You are the best photo clicker EVER!");

			echo "<p>Good job, you've pruned $countcomplete out of $counttotalvideos videos. That's $percentcomplete percent!!!</p>";
			$praisestr = $praise[rand(0,4)];
			echo "<p>$praisestr</p>";
		}
	}

} else {
	$sql2 = "SELECT count(yt_id) as countcomplete ".
			"FROM videos ".
			"WHERE prune != -1 and eventid = $eventid";
	$rs2 = mysql_query($sql2);
	$row2 = mysql_fetch_assoc($rs2);
	$countcomplete = $row2['countcomplete'];
	$percentcomplete = round(($countcomplete / $counttotalvideos) * 100, 2);
	echo "<p>Percent of photos for this event that have been pruned:<br/><img src='http://needles.guzzlingcakes.com/images/progress/progress.php?percent=".intval($percentcomplete)."' /></p>";
	echo "<p>Click to highlight videos that are not about this event</p>";
}

$sql = "SELECT count(yt_id) as count ".
				"FROM videos where eventid = $eventid";
$rs = mysql_query($sql);
if (!$rs) {
    fwrite($log, mysql_error() . " query:$sql\n");
}
$row = mysql_fetch_assoc($rs);
$counttotalvideos = $row['count'];

$sql = "SELECT count(yt_id) as count ".
				"FROM videos ".
				"WHERE eventid = $eventid";
$rs = mysql_query($sql);
if (!$rs) {
    fwrite($log, mysql_error() . " query:$sql\n");
}
$row = mysql_fetch_assoc($rs);
$count2 = $row['count'];

if($confirm) {

	$sql = "SELECT * ".
					"FROM videos ".
					"WHERE eventid = $eventid ".
					"AND prune = 1";
	//echo $sql;
	$rs = mysql_query($sql);
	if (!$rs) {
	    fwrite($log, mysql_error() . " query:$sql\n");
	}
	echo "<p>Here all all the videos you selected. Any changes?</p>";
	echo "<table><tr>";
	$col=0;
	$ids=array();
	while ($video = mysql_fetch_assoc($rs)) {
	    $col++;
	    $yt_id = $video['yt_id'];
	    array_push($ids, $yt_id);

		$author = $video['author'];
		$id = $video['id'];
		$title = $video['title'];
		$length = $video['length_seconds'];
		$rating = $video['rating_avg'];
		$rating_count = $video['rating_count'];
		$description = $video['description'];
		$description = substr($description, 0, 500);
		$view_count = $video['view_count'];
		$upload_time = $video['upload_time'];
		$comment_count = $video['comment_count'];
		$tags = $video['tags'];
		$url = $video['url'];
		$thumb = $video['thumb'];

		$embed = "<object width='425' height='350'><param name='movie' value='http://www.youtube.com/v/$yt_id'></param><param name='wmode' value='transparent'></param><embed src='http://www.youtube.com/v/$yt_id' type='application/x-shockwave-flash' wmode='transparent' width='425' height='350'></embed></object>";

		$videohtml = "<h4>$title</h4>";
		$videohtml .= "$embed<br/>";
		$videohtml .= "Description: $description<br/>";
		$videohtml .= "Tags: $tags<br/>";
		$videohtml .= "Views: $view_count<br/>";
		$videohtml .= "Comments: $comment_count<br/>";
		$videohtml .= "Author: $author<br/>";
		$videohtml .= "Length: $length seconds<br/>";
		$videohtml .= "Rating: $rating ($rating_count ratings)";


	    echo "<td width='450' onclick='toggle(\"$yt_id\")' id='$yt_id' style = 'border:5px solid'>$videohtml</td>";
	    if($col>1) {
		echo "</tr><tr>";
		$col=0;
	    }
	}
	echo "</tr></table>";

	echo "<script>\n";
	echo "var ids = new Array(".count($ids).");\n";
	foreach($ids as $id) {
		echo "ids[$id]=1;\n";
	}


} else {
	echo "<table><tr>";
	$ids=array();
	$col = 0;
	$limit = 10;// - $prunedcount;

	if($view_pruned) {
		$sql = "SELECT * ".
					"FROM videos WHERE prune = 0 ".
					"AND eventid = $eventid;";
	} else {
		$sql = "SELECT * ".
					"FROM videos WHERE prune = -1 ".
					"AND eventid = $eventid ".
					"LIMIT $limit";
	}
	//echo $sql;
	$rs = mysql_query($sql);
	if (!$rs) {
	    fwrite($log, mysql_error() . " query:$sql\n");
	}
	//echo "<table><tr>";
	//$col=0;
	$count = 0;
	while ($video = mysql_fetch_assoc($rs)) {
		$count++;
	    $col++;

	    $yt_id = $video['yt_id'];
	    array_push($ids, $yt_id);

		$author = $video['author'];
		$id = $video['id'];
		$title = $video['title'];
		$length = $video['length_seconds'];
		$rating = $video['rating_avg'];
		$rating_count = $video['rating_count'];
		$description = $video['description'];
		$description = substr($description, 0, 500);
		$view_count = $video['view_count'];
		$upload_time = $video['upload_time'];
		$comment_count = $video['comment_count'];
		$tags = $video['tags'];
		$url = $video['url'];
		$thumb = $video['thumb'];

		$embed = "<object width='425' height='350'><param name='movie' value='http://www.youtube.com/v/$yt_id'></param><param name='wmode' value='transparent'></param><embed src='http://www.youtube.com/v/$yt_id' type='application/x-shockwave-flash' wmode='transparent' width='425' height='350'></embed></object>";

		$videohtml = "<h4>$title</h4>";
		$videohtml .= "$embed<br/>";
		$videohtml .= "Description: $description<br/>";
		$videohtml .= "Tags: $tags<br/>";
		$videohtml .= "Views: $view_count<br/>";
		$videohtml .= "Comments: $comment_count<br/>";
		$videohtml .= "Author: $author<br/>";
		$videohtml .= "Length: $length seconds<br/>";
		$videohtml .= "Rating: $rating ($rating_count ratings)";

	    echo "<td width='450' onclick='toggle(\"$yt_id\")' id='$yt_id' style = 'border:5px solid'>$videohtml</td>";

	    if($col>1) {
		echo "</tr><tr>";
		$col=0;
	    }
	}
	echo "</tr></table>";

	if($count == 0) {
		echo "You're done!!! YOU ROCK!!! You have pruned all the media for this event<br>";
		return;
	}

	echo "<script>\n";
	echo "var ids = new Array(".count($ids).");\n";
	foreach($ids as $id) {
		echo "ids['$id']=0;\n";
	}
}




?>
function toggle(id) {
	var tblElt = document.getElementById(id);
	tblElt.cellSpacing = 0;
	if(ids[id]==1) {
		tblElt.style.border = "";
		ids[id]=0;
	} else {
		tblElt.style.border = "5px solid green";
		ids[id]=1;
	}
	//document.getElementById("yt_ids").innerHTML += id+",";
}

function validate() {
	var selectedids="";
	var notselectedids="";
	for(var id in ids) {
		if(ids[id]==1) {
			selectedids += id + ",";
		} else {
			notselectedids += id + ",";
		}

	}
	//alert(selectedids);
	document.forms.foo.ids.value=selectedids;
	document.forms.foo.notselectedids.value=notselectedids;
	return true;
}

function done() {
	window.location = "prune_videos.php?eventid=<?php echo $eventid; ?>";
}
</script>

<div id="yt_ids"></div>

<div align='center'>
<form name="foo"
action="prune_videos.php"
method="get"
onsubmit=" if (!validate()) { return false; }">
<input type="hidden" name="ids" value=""/>
<input type="hidden" name="notselectedids" value=""/>
<input type="hidden" name="eventid" value="<?php echo $eventid; ?>"/>
<?php if($view_pruned) { ?>
	<input type="hidden" name="view_pruned" value="true"/>
	<input type="button" value="Save Changes >>" onclick="if (!validate()) { return false; }
		else { document.forms.foo.submit(); }">
<?php } else { ?>
	<?php if($confirm) { ?>
		<input type="hidden" name="confirm" value="1">
		<input type="button" value="I have a few changes, save them please" onclick="if (!validate()) { return false; }
			else { document.forms.foo.submit(); }">
		<input type="button" value="I'm done, peace out" onclick="done();">
	<?php } else { ?>
		<input type="button" value="Next >>" onclick="if (!validate()) { return false; }
			else { document.forms.foo.submit(); }">
	<?php } ?>
<?php } ?>

</form>
</div>
</body>
</html>