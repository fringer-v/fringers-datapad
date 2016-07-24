<?php
session_start();

require_once "functions.php";

if (isset($_POST["field6"])) {
	$error = register_user($_POST["field1"], $_POST["field2"], $_POST["field3"], $_POST["field4"], $_POST["field5"],
		$_POST["field7"], $_POST["field8"], $_POST["field9"]);
	if (empty($error)) {
		header("Location: files.php", true, 302);
		die();
	}
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>Fringer's Datapad - Register</title>
<link rel="stylesheet" type="text/css" href="style.css"/>
</head>

<body bgcolor="#000000">
<form method="post" action="register.php">
<table border="0" width="100%">
<tr><td align="center">

<table border="0" width="80%">
<tr><td class="main-text" align="center" colspan="2"><font size="+2">Fringer's Datapad</font></td></tr>
<tr><td class="main-text" align="center" colspan="2"><font size="+1">a Character Sheet for the Star Wars&trade; EotE, AoR and F&D Role Playing Games</font></td></tr>
<tr><td colspan="2"><img src="space.gif" width="1" height="1" border="0"/></td></tr>
<tr><td height="1" colspan="2"><hr width="100%" /></td></tr>
<tr><td colspan="2"><img src="space.gif" width="1" height="20" border="0"/></td></tr>

<tr><td colspan="2" align="center">

<table border="0" width="340">

<tr><td colspan="2" class="main-text"><font size="+1">Register</font></td></tr>
<tr><td colspan="2"><hr width="100%" /></td></tr>

<tr>
<td class="comic-text" align="left" nowrap="nowrap">Your Email Address</td>
<td class="comic-text" align="right"><input name="field1" type="text" value="<?php print $_POST['field1'];?>"/></td>
</tr>
<tr>
<td class="comic-text" align="left" nowrap="nowrap">Password</td>
<td class="comic-text" align="right"><input name="field2" type="password" value="<?php print $_POST['field2'];?>"/></td>
</tr>
<tr>
<td class="comic-text" align="left" nowrap="nowrap">Repeat Password</td>
<td class="comic-text" align="right"><input name="field3" type="password" value="<?php print $_POST['field3'];?>"/></td>
</tr>
<tr>
<td class="comic-text" align="left" nowrap="nowrap">Security Questions:</td>
<td class="comic-text" align="left" nowrap="nowrap"></td>
</tr>
<tr>
<td class="comic-text" align="left" colspan="2"><font size="-1">The answers to these questions are used to determine your identify, if you loose your password. Please enter values that you can remember!</font></td>
</tr>
<tr>
<td class="comic-text" align="left" nowrap="nowrap">Your Birth Date</td>
<td class="comic-text" align="left"><input name="field7" type="text" size="3" value="<?php print $_POST['field7'];?>"/> <select name="field8">
<option value="01" <?php if ($_POST['field8'] == '01') print 'selected';?>>Jan</option>
<option value="02" <?php if ($_POST['field8'] == '02') print 'selected';?>>Feb</option>
<option value="03" <?php if ($_POST['field8'] == '03') print 'selected';?>>Mar</option>
<option value="04" <?php if ($_POST['field8'] == '04') print 'selected';?>>Apr</option>
<option value="05" <?php if ($_POST['field8'] == '05') print 'selected';?>>May</option>
<option value="06" <?php if ($_POST['field8'] == '06') print 'selected';?>>Jun</option>
<option value="07" <?php if ($_POST['field8'] == '07') print 'selected';?>>Jul</option>
<option value="08" <?php if ($_POST['field8'] == '08') print 'selected';?>>Aug</option>
<option value="09" <?php if ($_POST['field8'] == '09') print 'selected';?>>Sep</option>
<option value="10" <?php if ($_POST['field8'] == '10') print 'selected';?>>Oct</option>
<option value="11" <?php if ($_POST['field8'] == '11') print 'selected';?>>Nov</option>
<option value="12" <?php if ($_POST['field8'] == '12') print 'selected';?>>Dec</option>
</select> <input name="field9" type="text" size="6" value="<?php print $_POST['field9'];?>"/</td>
</tr>
<tr>
<td class="comic-text" align="left" nowrap="nowrap">
<select name="field4">
<option value="pet" <?php if ($_POST['field4'] == 'pet') print 'selected';?>>The name of your first pet?</option>
<option value="street" <?php if ($_POST['field4'] == 'street') print 'selected';?>>The name of the street where you grew up?</option>
<option value="maiden" <?php if ($_POST['field4'] == 'maiden') print 'selected';?>>Your mother's maiden name?</option>
<option value="team" <?php if ($_POST['field4'] == 'team') print 'selected';?>>The name of your favourite sports team?</option>
<option value="nickname" <?php if ($_POST['field4'] == 'nickname') print 'selected';?>>What was your childhood nickname?</option>
<option value="teacher" <?php if ($_POST['field4'] == 'teacher') print 'selected';?>>The name of your favourite teacher?</option>
</select></td>
<td class="comic-text" align="right"><input name="field5" type="text" value="<?php print $_POST['field5'];?>"/></td>
</tr>
<tr>
<td class="comic-text" align="left"></td>
<td class="comic-text" align="right"><input name="field6" type="submit" value="Register"/></td>
</tr>
<tr><td colspan="2"><img src="space.gif" width="1" height="15" border="0"/></td></tr>
<tr><td colspan="2" class="comic-text" align="center"><font color="red"><?php print $error;?></font></td></tr>
<tr><td colspan="2"><img src="space.gif" width="1" height="5" border="0"/></td></tr>
<td class="comic-text" align="center" colspan="2"><a href="login.php">Back to Login...</a></td>
<tr><td colspan="2"><img src="space.gif" width="1" height="10" border="0"/></td></tr>
</table>

</td></tr>
</table>

</td></tr>
</table>
</form>
</body>
</html>