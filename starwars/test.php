<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>jQuery UI Button - Default functionality</title>
<link rel="stylesheet" href="http://code.jquery.com/ui/1.11.2/themes/vader/jquery-ui.css">
<script src="http://code.jquery.com/jquery-1.10.2.js"></script>
<script src="http://code.jquery.com/ui/1.11.2/jquery-ui.js"></script>
<link rel="stylesheet" href="/resources/demos/style.css">
<script>
$(function() {
$( "input[type=submit], a, button" )
.button()
.click(function( event ) {
event.preventDefault();
});
});
</script>
</head>
<body>
<button>A button element</button>
<input class="ui-button ui-button-text-only" type="submit" value="A submit button">
<a href="#">An anchor</a>
</body>
</html>