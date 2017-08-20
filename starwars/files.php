<?php
session_start();

require_once "functions.php";

if (!isset($_SESSION["email_address"])) {
	header("Location: login.php", true, 302);
	die();
}

if (isset($_POST["uploaded_type"]) && !empty($_FILES["uploaded_file"]["tmp_name"])) {
	$target_file = basename($_FILES["uploaded_file"]["name"]);
	$temp_file = $_FILES["uploaded_file"]["tmp_name"];
	$upload_dir = $_POST["uploaded_type"];
	$ext = pathinfo($target_file, PATHINFO_EXTENSION);
	if (strpos($target_file, '/') !== FALSE || strpos($target_file, '\\') !== FALSE ||
		strpos($target_file, '"') !== FALSE)
		$error = "Illegal character in file name: ".$target_file;
	else if ($upload_dir == "chars" && $ext != "xml")
		$error = "Characters files must have a .xml extension";
	else if ($upload_dir == "dsets" && $ext != "zip")
		$error = "DataSet files must have a .zip extension";
	else if ($upload_dir == "cdata" && $ext != "xpad")
		$error = "Current Data files must have a .xpad extension";
	else {
		$to_file = $data_path."/".$_SESSION["email_address"]."/".$upload_dir."/".$target_file;
		if (file_exists($to_file))
			unlink($to_file);
		move_uploaded_file($temp_file, $to_file);
	}
}

if (isset($_POST["delete_file"])) {
	$delete_file = $data_path."/".$_SESSION["email_address"]."/".$_POST["delete_file"];
	if (file_exists($delete_file))
		unlink($delete_file);
}

if (isset($_GET["download_file"])) {
	$download_file = $data_path."/".$_SESSION["email_address"]."/".base64_decode($_GET["download_file"]);
	if (file_exists($download_file)) {
		header('Content-Description: File Transfer');
		header('Content-Type: application/octet-stream');
		header('Content-Disposition: attachment; filename="'.basename($download_file).'"');
		header('Expires: 0');
		header('Cache-Control: must-revalidate');
		header('Pragma: no-cacheLogout');
		header('Content-Length: ' . filesize($download_file));
		readfile($download_file);
		exit;
	}
}

if (isset($_GET["logout"])) {
	session_destroy();
	header("Location: login.php", true, 302);
	die();
}

$characters = get_contents($_SESSION["email_address"], "chars");
$datasets = get_contents($_SESSION["email_address"], "dsets");
$current_data = get_contents($_SESSION["email_address"], "cdata");

?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>Fringer's Datapad - Files</title>
<link rel="stylesheet" type="text/css" href="style.css"/>
<link rel="stylesheet" href="js/jquery-ui-1.11.2/jquery-ui.css">
<script src="js/jquery-1.11.2.min.js"></script>
<script src="js/jquery-ui-1.11.2/jquery-ui.js"></script>
<script language="javascript">
	function uploadDialog(type) {
		document.getElementById("uploaded_type").value = type;
		$("#upload").dialog({
			dialogClass: "no-close",
			minWidth: 440,
			minHeight: 200,
			buttons: [
				{ text: "OK", click: function() { $("#upload_form").submit(); $(this).dialog("close");  } },
				{ text: "Cancel", click: function() { $( this ).dialog( "close" );  } }
		]
		});
	}
	function deleteDialog(file) {
		console.debug(file);
		document.getElementById("file_to_delete").innerHTML = "Do you really want to delete "+file.substr(file.search("/")+1)+"?";
		document.getElementById("delete_file").value = file;
		$("#delete").dialog({
			dialogClass: "no-close",
			buttons: [
				{ text: "OK", click: function() { $("#delete_form").submit(); $(this).dialog("close");  } },
				{ text: "Cancel", click: function() { $( this ).dialog( "close" );  } }
		]
		});
	}
	function setUpPage() {
		$("#upload").hide();
		$("#delete").hide();
	}
</script>
</head>
<body bgcolor="#000000" onLoad='setUpPage()'>
<table border="0" width="100%">
<tr><td align="center">
<table border="0" width="80%">
<tr><td class="main-text" align="center" colspan="+2"><font size="+2">Fringer's Datapad</font></td></tr>
<td class="comic-text" align="left"><font size="+1"><?php print $_SESSION["email_address"];?></font></td>
<td class="comic-text" align="right"><font size="+1"><a href="?logout">Logout...</a></font></td>
</tr>
<tr><td height="1" colspan="2"><hr width="100%" /></td></tr>
<?php
if (!empty($error)) {
	print '<tr><td colspan="2" class="comic-text" align="center"><font color="red">'.$error.'</font></td></tr>';
}
?>
<tr><td colspan="2"><img src="space.gif" width="1" height="20" border="0"/></td></tr>

<tr><td colspan="2" align="center">

<table border="0" width="80%">

<tr><td colspan="2" class="main-text"><font size="+1"><b>Characters</b></font></td>
<td class="main-text" align="center" width="30"><a href="#" onclick="uploadDialog('chars');"><img src="add.png" height="20" width="20" /></a></td></tr>
<tr><td colspan="3"><hr width="100%" /></td></tr>

<?php
	if (count($characters) == 0) {
		print '<tr>';
		print '<td class="comic-text" align="center" colspan="3">No characters uploaded.<br>
		Character files are <i>.xml</i> files exported from OggDude\'s Star Wars Character Generator.<br>
		For example: <i>Yubookoo - Wookiee Hired Gun.xml</i></td>';
		print '</tr>';
	}
	else {
		foreach ($characters as $file) {
			$mtime = date ("Y-m-d H:i:s", filemtime($data_path."/".$_SESSION["email_address"]."/chars/".$file));
			$download_file = base64_encode("chars/".$file);
			print '<tr>';
			print '<td class="code-text" align="left"><a href="?download_file='.$download_file.'">'.$file.'</a></td>';
			print '<td class="code-text" align="right">'.$mtime.'</td>';
			print '<td class="comic-text" align="center" width="30"><a href="#" onclick="deleteDialog(\'chars/'.$file.'\');">'.
				'<img src="remove2.png" height="20" width="20" /></a></td>';
			print '</tr>';
		}
	}
?>

<tr><td colspan="3"><img src="space.gif" width="1" height="20" border="0"/></td></tr>
<tr><td colspan="2" class="main-text"><font size="+1"><b>Data Sets</b></font></td>
<td class="main-text" align="center" width="30"><a href="#" onclick="uploadDialog('dsets');"><img src="add.png" height="20" width="20" /></a></td></tr>
<tr><td colspan="3"><hr width="100%" /></td></tr>

<?php
	if (count($datasets) == 0) {
		print '<tr>';
		print '<td class="comic-text" align="center" colspan="3">No Data Sets uploaded.<br>
		Data Set files are <i>.zip</i> files exported from OggDude\'s Star Wars Data Editor.<br>
		For example: <i>SWDataSet-Example 27Mar2016 1659.zip</i></td>';
		print '</tr>';
	}
	else {
		foreach ($datasets as $file) {
			$mtime = date ("Y-m-d H:i:s", filemtime($data_path."/".$_SESSION["email_address"]."/dsets/".$file));
			$download_file = base64_encode("dsets/".$file);
			print '<tr>';
			print '<td class="code-text" align="left"><a href="?download_file='.$download_file.'">'.$file.'</a></td>';
			print '<td class="code-text" align="right">'.$mtime.'</td>';
			print '<td class="comic-text" align="center" width="30"><a href="#" onclick="deleteDialog(\'dsets/'.$file.'\');">'.
				'<img src="remove2.png" height="20" width="20" /></a></td>';
			print '</tr>';
		}
	}
?>

<tr><td colspan="3"><img src="space.gif" width="1" height="20" border="0"/></td></tr>
<tr><td colspan="2" class="main-text"><font size="+1"><b>Current Data</b></font></td>
<td class="comic-text" align="center" width="30"><a href="#" onclick="uploadDialog('cdata');"><img src="add.png" height="20" width="20" /></a></td></tr>
<tr><td colspan="3"><hr width="100%" /></td></tr>

<?php
	if (count($current_data) == 0) {
		print '<tr>';
		print '<td class="comic-text" align="center" colspan="3">'."Current Data files will be uploaded by Fringer's Datapad App.".'<br>
		<font size="-1">Current Data (<i>.xpad</i>) files are XML files that contain changes to your characters made by the App.<br>
		Any modifications you make to the files here (including deletion) will be transferred to the App when you "Sync with Server".</font></td>';
		print '</tr>';
	}
	else {
		foreach ($current_data as $file) {
			$mtime = date ("Y-m-d H:i:s", filemtime($data_path."/".$_SESSION["email_address"]."/cdata/".$file));
			$download_file = base64_encode("cdata/".$file);
			print '<tr>';
			print '<td class="code-text" align="left"><a href="?download_file='.$download_file.'">';
			if (ends_with($file, ".tri"))
				print '<font color="red">';
			print $file;
			if (ends_with($file, ".tri"))
				print '</font>';
			print '</a></td>\n';
			print '<td class="code-text" align="right">';
			if (ends_with($file, ".tri"))
				print '<font color="red">';
			print $mtime;
			if (ends_with($file, ".tri"))
				print '</font>';
			print '</td>\n';
			print '<td class="comic-text" align="center" width="30"><a href="#" onclick="deleteDialog(\'cdata/'.$file.'\');">'.
				'<img src="remove2.png" height="20" width="20" /></a></td>\n';
			print '</tr>';
		}
	}
?>

</table>

</td></tr>
</table>

</td></tr>
</table>
<div id="upload" title="Select a File to Upload" class="comic-text">

<!--
<form action="files.php" method="post" enctype="multipart/form-data">
    Select image to upload:
    <input type="file" name="fileToUpload" id="fileToUpload">
    <input type="submit" value="Upload Image" name="submit">
</form>
-->

<form id="upload_form" action="files.php" method="post" enctype="multipart/form-data">
<table border="0">
<tr><td><img src="space.gif" width="1" height="10" border="0"/></td></tr>
<tr><td><input type="file" name="uploaded_file" id="uploaded_file" /><td></tr>
<tr><td><img src="space.gif" width="1" height="10" border="0"/></td></tr>
</table>
<input type="hidden" name="uploaded_type" id="uploaded_type" value="x" />
</form>

</div>
<div id="delete" title="Delete File" class="comic-text">
<form id="delete_form" action="files.php" method="post">
<table border="0">
<tr><td><img src="space.gif" width="1" height="10" border="0"/></td></tr>
<tr>
<td id="file_to_delete">text<td>
</tr>
<tr><td><img src="space.gif" width="1" height="10" border="0"/></td></tr>
</table>
<input type="hidden" name="delete_file" id="delete_file" value="x" />
</form>
</div>
</body>
</html>