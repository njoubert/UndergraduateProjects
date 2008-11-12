<?php

	$savedir = "../streams/"

	function handleUploadedFile() {
		if (!isset($_FILES['uploadedFile'])) {
			echo 'No file uploaded!\n';
			print_r($_FILES);
			return;
		}
		
		$uploadfile = $savedir . basename($_FILES['uploadedFile']['name'])
		
		if (!move_uploaded_file($_FILES['uploadedFile']['tmp_name'], $uploadfile)) {
			echo 'File could not be moved\n';
			print_r($_FILES);
			return;
		}
			
	}
	
	handleUploadedFile();
?>
