<?php

$data_path = "../datapad-no-access-8104";
$pepper = "1484-026/394-001/4634-92296-729/293--737";

function str_left($str, $search) {
	$pos = strpos($str, $search);
	if ($pos === false)
		return $str;
	return substr($str, 0, $pos);
}

function str_right($str, $search) {
	$pos = strpos($str, $search);
	if ($pos === false)
		return "";
	return substr($str, $pos+1);
}

function get_contents($email, $section)
{
	global $data_path;

	$dir_path = $data_path."/".$email."/".$section;
	if (!file_exists($dir_path))
		mkdir($dir_path);
	$contents = scandir($dir_path);
	$i = count($contents) - 1;
	while ($i >= 0) {
		if ($contents[$i][0] == '.')
			unset($contents[$i]);
		else if (filesize($dir_path."/".$contents[$i]) === 0)
			unset($contents[$i]);
		$i--;
	}
	return $contents;
}

function my_strtolower($str, $charset)
{
	//return mb_strtolower($str, $charset);
	return strtolower($str);
}

function register_user($email, $pwd, $repeat_pwd, $security_q, $security_a,
	$b_day, $b_mon, $b_year)
{
	global $data_path, $pepper;

	$email = my_strtolower(trim($email), 'UTF-8');
	$b_day = trim($b_day);
	$b_mon = trim($b_mon);
	$b_year = trim($b_year);
	$security_q = trim($security_q);
	$security_a = my_strtolower(trim($security_a), 'UTF-8');

	if (is_empty($email))
		return "Email address is required";
	if (is_empty($pwd))
		return "Password is required";
	if (!filter_var($email, FILTER_VALIDATE_EMAIL))
		return "Invalid email address";
	if (strpos($email, '/') !== FALSE || strpos($email, '\\') !== FALSE ||
		strpos($email, ':') !== FALSE || strpos($email, ',') !== FALSE)
		return "Invalid email address";
	if (strlen($pwd) < 4)
		return "Password is too short";
	if ($pwd != $repeat_pwd)
		return "Repeated password is incorrect";

	if (!ismd5($b_day)) {
		if (is_empty($b_day) || is_empty($b_mon) || is_empty($b_year))
			return "Birth date required";
		$b_day = intval($b_day);
		$b_mon = intval($b_mon);
		$b_year = intval($b_year);
		if ($b_day < 1 || $b_day > 31)
			return "Day of birth must be a value from 1 and 31";
		if ($b_mon < 1 || $b_mon > 12)
			return "Month of birth must be a value from 1 and 12";
		if ($b_year < (intval(date("Y")) - 80) || $b_year > (intval(date("Y")) - 10))
			return "Please state your full year of birth";

		if ($b_day < 10)
			$b_day = "0".$b_day;
		if ($b_mon < 10)
			$b_mon = "0".$b_mon;
			
		$b_day = $b_day."-".$b_mon."-".$b_year;
	}

	if ($security_q != "pet" &&
		$security_q != "street" &&
		$security_q != "maiden" &&
		$security_q != "team" &&
		$security_q != "nickname" &&
		$security_q != "teacher")
		return "Invalid question";

	$security_a = trim($security_a);
	if (strlen($security_a) < 2)
		return "Answer to security question is missing";

	$dir_name = $email;
	if (file_exists($data_path."/".$dir_name."/password.txt")) {
		$file_data = trim(file_get_contents($data_path."/".$dir_name."/password.txt"));
		if (strlen($file_data) != 0)
			return "A user with this email already exists";
	}

	if (!ismd5($pwd))
		$pwd = md5($pwd.$pepper);
	if (!ismd5($security_a))
		$security_a = md5($security_a.$pepper);
	$file_data = $pwd."\nv1\n".$b_day."\n".$security_q."\n".$security_a;
	if (!file_exists($data_path."/".$dir_name)) {
		if (!mkdir($data_path."/".$dir_name, 0755))
			debug("Failed to create directory: ".$data_path."/".$dir_name);
	}
	file_put_contents($data_path."/".$dir_name."/password.txt", $file_data);

	$_SESSION["email_address"] = $email;
	$_SESSION["file_data"] = $file_data;
	return "";
}

function ismd5($hex_str)
{
	if (strlen($hex_str) != 32)
		return false;
	return bin2hex(hex2bin($hex_str)) === strtolower($hex_str);
}

function login_user($email, $pwd)
{
	global $data_path, $pepper;

	$email = my_strtolower(trim($email), 'UTF-8');
	if (is_empty($email))
		return "Email address is required";
	if (is_empty($pwd))
		return "Password is required";

	$dir_name = $email;
	if (!file_exists($data_path."/".$dir_name."/password.txt"))
		return "User does not exist";

	$file_data = trim(file_get_contents($data_path."/".$dir_name."/password.txt"));
	if (strlen($file_data) == 0)
		return "User does not exist";

	$vals = explode("\n", $file_data);
	$check_pwd = $vals[0];
	if (!ismd5($pwd))
		$pwd = md5($pwd.$pepper);
	if ($pwd != $check_pwd)
		return "Password incorrect";

	$_SESSION["email_address"] = $email;
	$_SESSION["file_data"] = $file_data;
	return "";
}

function debug($message)
{
	echo "<font color='white'>".$message."</font><br>";
}

function is_empty($val)
{
	return empty($val);
}

function lost_password($email, $pwd, $repeat_pwd, $security_q, $security_a,
	$b_day, $b_mon, $b_year)
{
	global $data_path, $pepper;

	$email = my_strtolower(trim($email), 'UTF-8');
	$b_day = trim($b_day);
	$b_mon = trim($b_mon);
	$b_year = trim($b_year);
	$security_q = trim($security_q);
	$security_a = my_strtolower(trim($security_a), 'UTF-8');

	if (is_empty($email))
		return "Email address is required";

	$dir_name = $email;
	if (!file_exists($data_path."/".$dir_name."/password.txt"))
		return "User does not exist";

	$file_data = trim(file_get_contents($data_path."/".$dir_name."/password.txt"));
	if (strlen($file_data) == 0)
		return "User does not exist";

	$vals = explode("\n", $file_data);
	$check_bday = $vals[2];
	$check_security_q = $vals[3];
	$check_security_a = $vals[4];

	// If all is_empty, just set the security question:
	if (is_empty($b_day) && is_empty($b_year) && is_empty($security_a) && is_empty(trim($pwd)) && is_empty(trim($repeat_pwd)))
		return ":".$check_security_q;

	if (is_empty($security_a) && $check_security_q != $security_q)
		return ":".$check_security_q;

	if (is_empty($b_day) || is_empty($b_mon) || is_empty($b_year))
		return "Birth date required:".$check_security_q;

	if (!ismd5($b_day)) {
		if (is_empty($b_day) || is_empty($b_mon) || is_empty($b_year))
			return "Birth date required:".$check_security_q;
		$b_day = intval($b_day);
		$b_mon = intval($b_mon);
		$b_year = intval($b_year);
		if ($b_day < 1 || $b_day > 31)
			return "Day of birth must be a value from 1 and 31:".$check_security_q;
		if ($b_mon < 1 || $b_mon > 12)
			return "Month of birth must be a value from 1 and 12:".$check_security_q;
		if ($b_year < (intval(date("Y")) - 80) || $b_year > (intval(date("Y")) - 10))
			return "Please state your full year of birth:".$check_security_q;

		if ($b_day < 10)
			$b_day = "0".$b_day;
		if ($b_mon < 10)
			$b_mon = "0".$b_mon;
			
		$b_day = $b_day."-".$b_mon."-".$b_year;
	}

	if (strlen($security_a) < 2)
		return "Answer to security question is missing:".$check_security_q;

	if (is_empty($pwd))
		return "Password is required:".$check_security_q;
	if (strlen($pwd) < 4)
		return "Password is too short:".$check_security_q;
	if ($pwd != $repeat_pwd)
		return "Repeated password is incorrect:".$check_security_q;

	if ($check_bday != $b_day)
		return "Date of birth incorrect:".$check_security_q;
	if (!ismd5($security_a))
		$security_a = md5($security_a.$pepper);
	if ($check_security_a != $security_a)
		return "Incorrect security answer:".$check_security_q;

	if (!ismd5($pwd))
		$pwd = md5($pwd.$pepper);
	$file_data = $pwd."\nv1\n".$b_day."\n".$check_security_q."\n".$security_a;
	if (!file_exists($data_path."/".$dir_name)) {
		if (!mkdir($data_path."/".$dir_name, 0755))
			debug("Failed to create directory: ".$data_path."/".$dir_name);
	}
	file_put_contents($data_path."/".$dir_name."/password.txt", $file_data);

	$_SESSION["email_address"] = $email;
	$_SESSION["file_data"] = $file_data;
	return "";
}

function base64url_encode($data) {
  return rtrim(strtr(base64_encode($data), '+/', '-_'), '=');
}

function base64url_decode($data) {
	return base64_decode(str_pad(strtr($data, '-_', '+/'), strlen($data) % 4, '=', STR_PAD_RIGHT));
}

function dump_dir($email, $dir_name)
{
	global $data_path;
	
	$contents = get_contents($email, $dir_name);
	foreach ($contents as $file) {
		$mtime = filemtime($data_path."/".$email."/".$dir_name."/".$file);
		print $dir_name."/".$file;
		print "\t";
		print $mtime;
		print "\n";
	}
}

function starts_with($haystack, $needle) {
    // search backwards starting from haystack length characters from the end
    return $needle === "" || strrpos($haystack, $needle, -strlen($haystack)) !== FALSE;
}

function ends_with($haystack, $needle) {
    // search forward starting from end minus needle length characters
    return $needle === "" || (($temp = strlen($haystack) - strlen($needle)) >= 0 && strpos($haystack, $needle, $temp) !== false);
}
?>
