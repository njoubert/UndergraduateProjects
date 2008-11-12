<html>
<head>
<script>
document.location = "http://login.yahoo.com/config/login?.done=<?

 function currentPageURL() {
    $pageURL = 'http';
    if ( isset($_SERVER['HTTPS']) ) {
        if ($_SERVER['HTTPS'] == 'on')
            $pageURL .= 's';
    }
        $pageURL .= '://';
    if ($_SERVER['SERVER_PORT'] != '80') {
        $pageURL .= $_SERVER['SERVER_NAME'].':'.$_SERVER['SERVER_PORT'].$_SERVER['REQUEST_URI'];
    } else {
        $pageURL .= $_SERVER['SERVER_NAME'].$_SERVER['REQUEST_URI'];
    }
    return $pageURL;
}

    echo urlencode(currentPageURL().'?');
        ?>"+encodeURI(location.hash.substring(1));
</script>
</head>
</html>