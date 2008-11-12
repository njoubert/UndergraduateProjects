<?php

// Parsing Yahoo! REST Web Service results using
// unserialize. PHP4/PHP5
// Author: Jason Levitt
// February 1, 2006

error_reporting(E_ALL);
$query = "";
$year = "2007";
if(isset($_GET['query'])) $query = urlencode($_GET['query']);
if(isset($_GET['year'])) $year = $_GET['year'];

?>

Search past events:
<form action="event_search.php" method="get">
<input type="text" name="query" size="50" value="<?php echo urldecode($query); ?>">
Year:<select name="year">
  <option <?php if($year=="2007") echo "selected"; ?> value="2007">2007</option>
  <option <?php if($year=="2006") echo "selected"; ?>value="2006">2006</option>
  <option <?php if($year=="2005") echo "selected"; ?>value="2005">2005</option>
</select>

<input type="submit" value="search">
</form>


<?php
if($query != "") {
		switch($year) {
			case("2007"):
				$min_date = "2007-01-01";
				$max_date = date("Y-m-d");
				break;
			case("2006"):
				$min_date = "2006-01-01";
				$max_date = "2006-12-31";
				break;
			case("2005"):
				$min_date = "2005-01-01";
				$max_date = "2005-12-31";
				break;
		}

		// output=php means that the request will return serialized PHP
        $url =  "http://upcoming.yahooapis.com/services/rest/?api_key=cc9bffc500&method=event.search&search_text=$query&min_date=$min_date&max_date=$max_date";

        $user_agent = "Mozilla/5.0 (Macintosh; U; PPC Mac OSX; en-us)";
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_USERAGENT, $user_agent);
        curl_setopt ($ch, CURLOPT_FOLLOWLOCATION, 1);
        curl_setopt ($ch, CURLOPT_CONNECTTIMEOUT, 30);
        curl_setopt ($ch, CURLOPT_TIMEOUT, 30);
        curl_setopt ($ch, CURLOPT_RETURNTRANSFER, CURLAUTH_BASIC);

        $xmlstr = curl_exec ($ch);
        $xml = new SimpleXMLElement($xmlstr);

		$i=0;
        foreach($xml as $event) {
        	$i++;
            $eventid = $event['id'];
            $name = $event['name'];
            $descr = $event['description'];
            $date = $event['start_date'];
            $time = $event['start_time'];
            $venue = $event['venue_name'];
            $address = $event['venue_address'];
            $city = $event['venue_city'];
            $state = $event['venue_state_code'];
            echo "<h2>$name $date</h2>$venue $address $city, $state $time<br/>";
            echo substr($descr, 0, 200)."<br/>";
            echo "<a href='save_media.php?eventid=$eventid'>Get Media</a><br/><br/>";
        }
        if($i == 0) echo "No results";
}
?>