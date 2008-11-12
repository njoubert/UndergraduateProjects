<?php

    // Check the cookie signature
    if ( ! yck_cookie_check_signature() ) {
        error_log('Cookie signature check failed, redirecting...');
        include("bounce.php");
        exit();
    }

    // Check the cookie timestamp
    if ( ! yck_cookie_check_time_stamp() ) {
        error_log('Cookie timestamp check failed, redirecting...');
        include("bounce.php");
        exit();
    }   
    if ($_SERVER['QUERY_STRING']) {
    	header("Location: ./#".$_SERVER['QUERY_STRING']);
    	exit();
    }
    include('Happenings.html');
?>