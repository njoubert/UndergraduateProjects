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

$sql = "SELECT count(photo_id) as count ".
				"FROM photos WHERE eventid = $eventid";
$rs = mysql_query($sql);
if (!$rs) {
    fwrite($log, mysql_error() . " query:$sql\n");
}
$row = mysql_fetch_assoc($rs);
$counttotalphotos = $row['count'];



if(isset($_GET["confirm"])) {

	$notselectedids = substr($_GET["notselectedids"], 0, -1);
	$sql = "UPDATE photos SET prune = 0 WHERE photo_id IN ($notselectedids) AND eventid = $eventid";
	mysql_query($sql);
	echo "Your selections have been saved<p/>";
	echo "<a href='index.php?userid=$userid'>Go prune some more media!</a>";
	return;
}

$confirm=false;
if(isset($_GET["ids"])) {

        //$ids = split(",",$_GET["ids"]);
	$selectedids = substr($_GET["ids"], 0, -1);
	$notselectedids = substr($_GET["notselectedids"], 0, -1);
	$sql = "UPDATE photos SET prune = 1 WHERE photo_id IN ($selectedids) AND eventid = $eventid";
	mysql_query($sql);
	$sql = "UPDATE photos SET prune = 0 WHERE photo_id IN ($notselectedids) AND eventid = $eventid";
	mysql_query($sql);

	$sql2 = "SELECT count(photo_id) as countcomplete ".
			"FROM photos ".
			"WHERE prune != -1 and eventid = $eventid";
	$rs2 = mysql_query($sql2);
	$row2 = mysql_fetch_assoc($rs2);
	$countcomplete = $row2['countcomplete'];
	$percentcomplete = round(($countcomplete / $counttotalphotos) * 100, 2);

}

if(isset($_GET["ids"])) {

	echo "<p>Percent of photos for this event that have been pruned:<br/><img src='http://needles.guzzlingcakes.com/images/progress/progress.php?percent=".intval($percentcomplete)."' /></p>";
	echo "<p>Click to highlight photos that are not about this event</p>";

	if(!$view_pruned) {
		if($countcomplete == $counttotalphotos) {
			$confirm = true;
			echo "YOU ROCK!!! You have completed all the photos this event";
		} else {

			$praise = Array("Keep going, you can do it!",
											"Don't stop now, you've come this far!!!",
											"Come on, don't wimp out now!",
											"Your parents would be so proud right now if they know how many photos you have pruned. Keep it up!",
											"Rah rah rah!!! You are the best photo clicker EVER!");

			echo "<p>Good job, you've completed $countcomplete out of $counttotalphotos photos for this event. That's $percentcomplete percent!!!</p>";
			$praisestr = $praise[rand(0,4)];
			echo "<p>$praisestr</p>";
		}
	}

} else {
	$sql2 = "SELECT count(photo_id) as countcomplete ".
			"FROM photos ".
			"WHERE prune != -1 and eventid = $eventid";
	$rs2 = mysql_query($sql2);
	$row2 = mysql_fetch_assoc($rs2);
	$countcomplete = $row2['countcomplete'];
	$percentcomplete = round(($countcomplete / $counttotalphotos) * 100, 2);
	echo "<p>Percent of photos for this event that have been pruned:<br/><img src='http://needles.guzzlingcakes.com/images/progress/progress.php?percent=".intval($percentcomplete)."' /></p>";
	echo "<p>Click to highlight photos that are not about this event</p>";
}

$sql = "SELECT count(photo_id) as count ".
				"FROM photos where eventid = $eventid";
$rs = mysql_query($sql);
if (!$rs) {
    fwrite($log, mysql_error() . " query:$sql\n");
}
$row = mysql_fetch_assoc($rs);
$counttotalphotos = $row['count'];

$sql = "SELECT count(photo_id) as count ".
				"FROM photos ".
				"WHERE eventid = $eventid";
$rs = mysql_query($sql);
if (!$rs) {
    fwrite($log, mysql_error() . " query:$sql\n");
}
$row = mysql_fetch_assoc($rs);
$count2 = $row['count'];

if($confirm) {

	$sql = "SELECT photo_id, server, secret ".
					"FROM photos ".
					"WHERE eventid = $eventid ".
					"AND prune = 1";
	//echo $sql;
	$rs = mysql_query($sql);
	if (!$rs) {
	    fwrite($log, mysql_error() . " query:$sql\n");
	}
	echo "<p>Here all all the photos you selected. Any changes?</p>";
	echo "<table><tr>";
	$col=0;
	$ids=array();
	while ($row = mysql_fetch_assoc($rs)) {
	    $col++;
	    $photo_id = $row['photo_id'];
	    $server = $row['server'];
	    $secret = $row['secret'];
	    array_push($ids, $photo_id);

	    $url = "http://static.flickr.com/".$server."/".$photo_id."_".$secret."_t.jpg";
			//$fullurl = "http://static.flickr.com/".$server."/".$photo_id."_".$secret.".jpg";
			$fullurl = "http://www.flickr.com/photo.gne?id=".$photo_id;
	    echo "<td width=120 height=120><table id='$photo_id' style = 'border:5px solid green'><tr><td align='center'><a href='javascript:void(0)' onclick='toggle($photo_id)'><img border=0 src='$url'></a><br/><a href='$fullurl' target='_'><img border='0' src='http://us.i1.yimg.com/us.yimg.com/i/nt/ic/ut/bsc/srch16_1.gif'></a></td></tr></table></div></td>";
	    if($col>9) {
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
	$limit = 250;// - $prunedcount;

	if($view_pruned) {
		$sql = "SELECT photo_id, server, secret, tags, date_taken  ".
					"FROM photos WHERE prune = 0 ".
					"AND eventid = $eventid;";
	} else {
		$sql = "SELECT photo_id, server, secret, tags, date_taken ".
					"FROM photos WHERE prune = -1 ".
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
	while ($row = mysql_fetch_assoc($rs)) {
		$count++;
	    $col++;
	    $photo_id = $row['photo_id'];
	    $server = $row['server'];
	    $secret = $row['secret'];
	    $tags = $row['tags'];
	    $date_taken = $row['date_taken'];
	    array_push($ids, $photo_id);

		$tagarr = explode(" ", $tags);
		$tagstrunc = "";
		foreach($tagarr as $tag) {
			if(strlen($tag) < 20) {
				$tagstrunc .= $tag." ";
			}
		}

	    $url = "http://static.flickr.com/".$server."/".$photo_id."_".$secret."_t.jpg";
			//$fullurl = "http://static.flickr.com/".$server."/".$photo_id."_".$secret.".jpg";
			$fullurl = "http://www.flickr.com/photo.gne?id=".$photo_id;
	    echo "<td width='120' height='150' valign='top'><table id='$photo_id'><tr><td width='120' align='center'><a href='javascript:void(0)' onclick='toggle($photo_id)'><img border=0 src='$url'></a><br/><br/><a href='$fullurl' target='_'><img border='0' src='http://us.i1.yimg.com/us.yimg.com/i/nt/ic/ut/bsc/srch16_1.gif'></a></td></tr><tr><td><br/>Tags: $tagstrunc<br/>Date Taken: $date_taken<br/><br/><br/></td></tr><tr></table></div></td>";
	    if($col>9) {
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
		echo "ids[$id]=0;\n";
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
	//document.getElementById("photo_ids").innerHTML += id+",";
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
	window.location = "prune_photos.php?eventid=<?php echo $eventid; ?>";
}
</script>

<div id="photo_ids"></div>

<div align='center'>
<form name="foo"
action="prune_photos.php"
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