<?php

/*
https://stackoverflow.com/questions/29972343/what-does-this-php-code-do-looks-like-hacking
*/

$starwars_user_count = 0;

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
	return substr($str, $pos+strlen($search));
}

function starts_with($haystack, $needle) {
    // search backwards starting from haystack length characters from the end
    return $needle === "" || strpos($haystack, $needle) === 0;
}

function ends_with($haystack, $needle) {
    // search forward starting from end minus needle length characters
    return $needle === "" || (($temp = strlen($haystack) - strlen($needle)) >= 0 && strpos($haystack, $needle, $temp) !== false);
}

function dump_sample($cont)
{
	echo "-----------<br>";
	echo "<pre>".htmlspecialchars(substr($cont, 0, 200))."</pre>";
	echo "-----------<br>";
}

function detect_embedded($cont, $newpath)
{
	if (!strpos($cont, "?><?php"))
		return false;

	$str = str_left($cont, "?><?php");
	if (strlen($str) > 200) {
		for ($i=0; $i<strlen($str); $i++) {
			if ($str[$i] == "\r" || $str[$i] == "\n")
				return false;
		}
	}
	$newcont = "<?php".str_right($cont, "?><?php");
	file_put_contents($newpath, $newcont);
	return true;
}

function list_dir($path, $file_count)
{
	global $starwars_user_count;

	if (strpos($path, "8104/")) {
		$below = str_right($path, "8104/");
		if (strpos($below, "/")) {
			$below = str_right($below, "/");
			if ($below != "chars" && $below != "dsets" && $below != "cdata") {
				echo "PATH NOT ALLOWED: ".$path, "<br>";
			}
		}
		else {
			if (!strpos($below, "@") || !strpos($below, ".")) {
				echo "PATH NOT ALLOWED (2): ".$path, "<br>";
			}
		}
	}

	foreach (scandir($path) as $key=>$file_name)
	{
		if ($file_name == "." || $file_name == "..")
			continue;

		if (ends_With($path, "8104") && strpos($file_name, "@"))
			$starwars_user_count++;

		$newpath = $path."/".$file_name;
		if (ends_with($newpath, ".php"))
			echo "PHP ".date ("d.m.Y H:i:s.", filemtime($newpath))." ".$newpath."<br>";

		if (is_dir($newpath)) {
			$file_count++;
			$file_count = list_dir($newpath, $file_count);
		}
		else {
			if (strpos($path, "8104/")) {
				if (ends_with($path, "/chars")) {
					if (!ends_with($file_name, ".xml")) {
						echo "(1) REMOVED INVALID FILE: ".$newpath, "<br>";
						rename($newpath, "./got-it/x~".$file_name."-".rand());
					}
				}
				else if (ends_with($path, "/dsets")) {
					if (!ends_with($file_name, ".zip")) {
						echo "(2) REMOVED INVALID FILE: ".$newpath, "<br>";
						rename($newpath, "./got-it/x~".$file_name."-".rand());
					}
				}
				else if (ends_with($path, "/cdata")) {
					if (!ends_with($file_name, ".xpad") && !ends_with($file_name, "tri")) {
						echo "(3) REMOVED INVALID FILE: ".$newpath, "<br>";
						rename($newpath, "./got-it/x~".$file_name."-".rand());
					}
				}
				else if ($file_name != "password.txt") {
					echo "(4) REMOVED INVALID FILE: ".$newpath, "<br>";
					rename($newpath, "./got-it/x~".$file_name."-".rand());
				}
			}
			else if (ends_with($file_name, ".php") && $file_name != "conf-8104.php") {
				if (starts_with($file_name, "index.php"))
					echo "(10) INDEX: ".$newpath, "<br>";
				$cont = file_get_contents($newpath);
				if (strpos($cont, '@include "\x')) {
					echo '(5) php FILE CONTAINS @include "\x: '.$newpath, "<br>";
					dump_sample($cont);
				}
				else if (starts_with($cont, '<?php ${"\x')) {
					echo '(6) php REMOVED FILE STARTS WITH @include ..php ${"\x '.$newpath, "<br>";
					rename($newpath, "./got-it/x~".$file_name."-".rand());
				}
				else if (strpos($cont, '//php_off')) {
					echo '(7) php REMOVED FILE CONTAINS @include //php_off '.$newpath, "<br>";
					rename($newpath, "./got-it/x~".$file_name."-".rand());
				}
				else if (strpos($cont, '"\x')) {
					if (detect_embedded($cont, $newpath)) {
						echo "(8) php REMOVED EMBEDDED CODE: ".$newpath, "<br>";
						dump_sample($cont);
					}
					else if (!starts_with($cont, "<?php\r/**\r * @version")) {
						echo "(9) php FILE CONTAINS \x: ".$newpath, "<br>";
						dump_sample($cont);
					}
				}
				else if ($newpath != "../starwars/".$file_name) {
					echo '(10) php INVALID PATH! '.$newpath, "<br>";
					rename($newpath, "./got-it/x~".$file_name."-".rand());
				}
				else if ($file_name != "control.php" &&
					$file_name != "files.php" &&
					$file_name != "functions.php" &&
					$file_name != "login.php" &&
					$file_name != "lost-pwd.php" &&
					$file_name != "register.php" &&
					$file_name != "test.php") {
					echo '(11) php INVALID NAME! '.$newpath, "<br>";
					rename($newpath, "./got-it/x~".$file_name."-".rand());
				}
			}
			else if (ends_with($file_name, ".suspected"))
				echo "(12) SUSPECTED: ".$newpath, "<br>";
				
			$file_count++;
		}
		if ($file_count > 16000)
			break;
	}
	return $file_count;
}

echo '<div style="font-family: Monaco; font-size: 12px;">';
$file_count = list_dir("..", 0);
echo "FILES: ", $file_count, "<br>";
echo "USERS: ", $starwars_user_count, "<br>";

echo "</div>";

?>
