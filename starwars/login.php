<?php
session_start();

require_once "functions.php";

if (isset($_POST["field3"])) {
	$error = login_user($_POST["field1"], $_POST["field2"]);
	if (empty($error)) {
		header("Location: files.php", true, 302);
		die();
	}
}
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>Character Sheet Login</title>
<link rel="stylesheet" type="text/css" href="style.css"/>
</head>

<body bgcolor="#000000">
<form method="post" action="login.php">
<div data-role="main" class="ui-content">
<table border="0" width="100%">
<tr><td align="center">

<table border="0" width="80%">
<tr><td class="main-text" align="center" colspan="2"><font size="+2">Jabberwookiee's Star Wars&trade; Character Sheet</font></td></tr>
<tr><td colspan="2"><img src="space.gif" width="1" height="1" border="0"/></td></tr>
<tr><td height="1" colspan="2"><hr width="100%" /></td></tr>
<tr><td colspan="2"><img src="space.gif" width="1" height="20" border="0"/></td></tr>

<tr><td colspan="2" align="center">

<table border="0" width="300">

<tr><td colspan="2" class="main-text"><font size="+1">Login</font></td></tr>
<tr><td colspan="2"><hr width="100%" /></td></tr>

<tr>
<td class="comic-text" align="left">Email</td>
<td class="comic-text" align="right"><input name="field1" type="text"  value="<?php print $_POST['field1'];?>" size="30" /></td>
</tr>
<tr>
<td class="comic-text" align="left">Password</td>
<td class="comic-text" align="right"><input name="field2" type="password" value="<?php print $_POST['field2'];?>" size="30" /></td>
</tr>
<tr>
<td class="comic-text" align="left"></td>
<td class="comic-text" align="right"><input name="field3" type="submit" value="Login"/></td>
</tr>
<tr><td colspan="2"><img src="space.gif" width="1" height="15" border="0"/></td></tr>
<tr><td colspan="2" class="comic-text" align="center"><font color="red"><?php print $error;?></font></td></tr>
<tr><td colspan="2"><img src="space.gif" width="1" height="5" border="0"/></td></tr>
<td class="comic-text" align="center" colspan="2"><a href="register.php">Register...</a>&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;<a href="lost-pwd.php">Forgot password...</a></td>
<tr><td colspan="2"><img src="space.gif" width="1" height="10" border="0"/></td></tr>
</table>

</td></tr>
</table>

</td></tr>
<tr><td align="center">
<table border="0" width="520">
<tr><td class="comic-text" align="center">If you have not already done so, <a href="register.php"><u>register</u></a> and upload your character created using
  <a href="https://community.fantasyflightgames.com/topic/89135-another-character-generator/"><u><i>OggDude's Star Wars
  Character Generator</i></u></a>.<p>This can then be viewed using Jabberwookiee's Datapad App,
  on an Apple iPad. Custom "Data Sets" created with the <i>SW Data Editor</i>, can also be
  uploaded and used by the App.
  </td></tr>
</table>
</td></tr>
</table>
</div>
</form>
</body>
</html>