<?php include 'pagestart.php' ?>

<?php 

if ($_GET['page'] == "") {
	include 'main.php';
} else {
	include $_GET['page'] . '.php';
}

?>

<?php include 'pageend.php' ?>
