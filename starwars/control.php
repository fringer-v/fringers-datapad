<?php
require_once "functions.php";

//$email = base64url_decode($_GET["user"]);
$tver = $_GET["tver"];
$user = $_GET["user"];
$time = $_GET["time"];
$csum = $_GET["csum"];

$email = strtolower(base64url_decode($user));

//if ($tver < 200000) {
//		print "ERROR! Please update to 2.0.0";
//		die();
//}

// ?contents&user=base64(email)&time=1234&csum=hash(args, pwd)
if (isset($_GET["contents"])) {
	if (!file_exists($data_path."/".$email."/password.txt")) {
		print "ERROR! User does not exist: ".$email;
		die();
	}
	$file_data = trim(file_get_contents($data_path."/".$email."/password.txt"));
	$vals = explode("\n", $file_data);
	$args = "contents&user=".$user."&time=".$time;
	if (md5($args."-".$vals[0]) != $csum) {
		print "ERROR! Authorization failed: password incorrect";
		die();
	}
	print "contents:\n";
	dump_dir($email, "chars");
	dump_dir($email, "dsets");
	dump_dir($email, "cdata");
}

// ?download=bases64(file)&user=base64(email)&time=1234&csum=hash(args, pwd)
if (isset($_GET["download"])) {
	if (!file_exists($data_path."/".$email."/password.txt")) {
		header("HTTP/1.0 403 Forbidden");
		die();
	}
	$file_data = trim(file_get_contents($data_path."/".$email."/password.txt"));
	$vals = explode("\n", $file_data);
	$file = $_GET["download"];
	$args = "download=".$file."&user=".$user."&time=".$time;
	if (isset($_GET["tver"]))
		$args = $args."&tver=".$tver;
	if (md5($args."-".$vals[0]) != $csum) {
		header("HTTP/1.0 403 Forbidden");
		die();
	}

	$download = base64url_decode($file);
	$download_file = $data_path."/".$email."/".$download;
	if (file_exists($download_file)) {
		header('Content-Description: File Transfer');
		header('Content-Type: application/octet-stream');
		header('Content-Disposition: attachment; filename="'.basename($download_file).'"');
		header('Expires: 0');
		header('Cache-Control: must-revalidate');
		header('Pragma: no-cacheLogout');
		header('Content-Length: ' . filesize($download_file));
		readfile($download_file);
	}
	else {
		header("HTTP/1.0 404 Not Found");
		die();
	}
}

// ?upload=bases64(file)&user=base64(email)&time=1234&csum=hash(args, pwd)
if (isset($_GET["upload"])) {
	if (!file_exists($data_path."/".$email."/password.txt")) {
		print "ERROR! User does not exist: ".$email;
		die();
	}
	$file_data = trim(file_get_contents($data_path."/".$email."/password.txt"));
	$vals = explode("\n", $file_data);
	$file = $_GET["upload"];
	$modt = $_GET["modt"];
	$args = "upload=".$file."&user=".$user."&modt=".$modt."&time=".$time;
	if (isset($_GET["tver"]))
		$args = $args."&tver=".$tver;
	if (md5($args."-".$vals[0]) != $csum) {
		print "ERROR! Authorization failed: password incorrect";
		die();
	}

	$upload = base64url_decode($file);
	if (!starts_with($upload, "chars/") &&
		!starts_with($upload, "dsets/") &&
		!starts_with($upload, "cdata/")) {
		print "ERROR! Upload directory incorrect: ".$upload;
		die();
	}

	$base = substr($upload, strpos($upload, "/") + 1);
	if (strpos($base, '/') !== FALSE || strpos($base, '\\') !== FALSE ||
		strpos($base, '"') !== FALSE) {
		print "ERROR! Illegal character in file name: ".$upload;
		die();
	}

	$upload_file = $data_path."/".$email."/".$upload;
	$putdata = fopen("php://input", "r");
	$fp = fopen($upload_file, "w");
	while ($data = fread($putdata, 1024*256))
	  fwrite($fp, $data);

	fclose($fp);
	fclose($putdata);
	if (!touch($upload_file, $modt)) {
		print "ERROR! Unabled to touch file: ".$upload_file;
		die();
	}

	print "OK!";
}
?>