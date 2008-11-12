<?php

function getPage($pagevar) {

if ($pagevar['page'] == "") {
	return 'main.php';
} else {
	return $pagevar['page'] . '.php';
}

}




?>