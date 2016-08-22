.pragma library

.import "../js/constants.js" as Constant

var symbolCodes = [];

symbolCodes["[ABILITY]"] = 'A';
symbolCodes["[AB]"] = 'A';
symbolCodes["[ADVANTAGE]"] = 'a';
symbolCodes["[AD]"] = 'a';
symbolCodes["[BOOST]"] = 'B';
symbolCodes["[BO]"] = 'B';
symbolCodes["[CHALLENGE]"] = 'C';
symbolCodes["[CH]"] = 'C';
symbolCodes["[DESPAIR]"] = 'y';
symbolCodes["[DE]"] = 'y';
symbolCodes["[DIFFICULTY]"] = 'D';
symbolCodes["[DI]"] = 'D';
symbolCodes["[FAILURE]"] = 'f';
symbolCodes["[FA]"] = 'f';
symbolCodes["[PROFICIENCY]"] = 'P';
symbolCodes["[PR]"] = 'P';
symbolCodes["[SETBACK]"] = 'S';
symbolCodes["[SE]"] = 'S';
symbolCodes["[SUCCESS]"] = 's';
symbolCodes["[SU]"] = 's';
symbolCodes["[THREAT]"] = 't';
symbolCodes["[TH]"] = 't';
symbolCodes["[TRIUMPH]"] = 'x';
symbolCodes["[TR]"] = 'x';
symbolCodes["[LIGHT]"] = 'Z';
symbolCodes["[LI]"] = 'Z';
symbolCodes["[FP]"] = 'Z';
symbolCodes["[DARK]"] = 'z';
symbolCodes["[DA]"] = 'z';
symbolCodes["[REMSETBACK]"] = 'N';
symbolCodes["[RS]"] = 'N';
symbolCodes["[UPGABILITY]"] = 'U';
symbolCodes["[UA]"] = 'U';
symbolCodes["[UPGDIFFICULTY"] = 'u';
symbolCodes["[UD]"] = 'u';
symbolCodes["[DWNDIFFICULTY"] = 'd';
symbolCodes["[DD]"] = 'd';
symbolCodes["[FORCE]"] = 'F';
symbolCodes["[FO]"] = 'F';
symbolCodes["[REMFORCE"] = 'g';
symbolCodes["[RF]"] = 'g';
symbolCodes["[FORCEPOINT]"] = '.'; //'.';
symbolCodes["[FP]"] = '.'; //'.';
symbolCodes["[RESTRICTED]"] = '=';
symbolCodes["[RE]"] = '=';


function pixel(p)
{
	return p*1;
}

function enter(ctx, fixedWidth, fixedHeight, clip, backgroundStyle)
{
	if (clip === undefined)
		clip = false;

	ctx.save();

	// Setup a clip region which covers everything:
	if (clip) {
		ctx.beginPath();
		ctx.rect(0, 0, ctx.canvas.width, ctx.canvas.height);
		ctx.clip();
	}

	if (backgroundStyle !== undefined) {
		ctx.fillStyle = backgroundStyle;
		ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);
	}

	// If we want to draw on a fixed size canvas, then
	// then we scale accordingly
	if (fixedWidth !== undefined &&
		fixedHeight !== undefined) {
		ctx.scale(ctx.canvas.width / fixedWidth, ctx.canvas.height / fixedHeight);
	}

}

function exit(ctx)
{
	ctx.restore();
}

function beginClip(ctx, x, y, width, height)
{
	ctx.save();
	ctx.beginPath();
	ctx.rect(x, y, width, height);
	ctx.clip();
}

function endClip(ctx)
{
	ctx.restore();
}

function paintPath(ctx, x, y, width, height, lineSize, strokeStyle, fillStyle)
{
	if (fillStyle !== undefined) {
		if (typeof(fillStyle) === "object") {
			ctx.save();
			ctx.clip();
			placeImage(ctx, x, y, width, height, fillStyle);
			ctx.restore();
		}
		else {
			ctx.fillStyle = fillStyle;
			ctx.fill();
		}
	}
	if (strokeStyle !== undefined && lineSize > 0) {
		ctx.lineWidth = lineSize;
		ctx.strokeStyle = strokeStyle;
		ctx.stroke();
	}
}

function bevelRect(ctx, x, y, width, height, lineSize, bevelSize, strokeStyle, fillStyle) {
	var lof = lineSize/2;

	//console.log("bevel", x, y, width, height);
	ctx.beginPath();
	ctx.moveTo(x+bevelSize+lof, y+lof);
	ctx.lineTo(x+width-bevelSize-lof, y+lof);
	ctx.lineTo(x+width-lof, y+bevelSize+lof);
	ctx.lineTo(x+width-lof, y+height-bevelSize-lof);
	ctx.lineTo(x+width-bevelSize-lof, y+height-lof);
	ctx.lineTo(x+bevelSize+lof, y+height-lof);
	ctx.lineTo(x+lof, y+height-bevelSize-lof);
	ctx.lineTo(x+lof, y+bevelSize+lof);
	ctx.closePath();
	paintPath(ctx, x, y, width, height, lineSize, strokeStyle, fillStyle);
}

function pointyRect(ctx, x, y, width, height, lineSize, bevelSize, strokeStyle, fillStyle) {
	var lof = lineSize/2;

	//console.log("pointy", x, y, width, height);
	ctx.beginPath();
	ctx.moveTo(x+bevelSize+lof, y+lof);
	ctx.lineTo(x+width-bevelSize-lof, y+lof);
	ctx.lineTo(x+width-lof, y+height/2);
	ctx.lineTo(x+width-bevelSize-lof, y+height-lof);
	ctx.lineTo(x+bevelSize+lof, y+height-lof);
	ctx.lineTo(x+lof, y+height/2);
	ctx.closePath();
	paintPath(ctx, x, y, width, height, lineSize, strokeStyle, fillStyle);
}

// corner: top-left, top-right, bottom-left, bottom-right
function cornerRect(ctx, x, y, width, height, bevelSize, fillStyle, corner)
{
	ctx.beginPath();
	if (corner.indexOf("top-left") >= 0) {
		ctx.moveTo(x, y+bevelSize);
		ctx.lineTo(x+bevelSize, y);
	}
	else
		ctx.moveTo(x, y);
	if (corner.indexOf("top-right") >= 0) {
		ctx.lineTo(x+width-bevelSize, y);
		ctx.lineTo(x+width, y+bevelSize);
	}
	else
		ctx.lineTo(x+width, y);
	if (corner.indexOf("bottom-right") >= 0) {
		ctx.lineTo(x+width, y+height-bevelSize);
		ctx.lineTo(x+width-bevelSize, y+height);
	}
	else
		ctx.lineTo(x+width, y+height);
	if (corner.indexOf("bottom-left") >= 0) {
		ctx.lineTo(x+bevelSize, y+height);
		ctx.lineTo(x, y+height-bevelSize);
	}
	else
		ctx.lineTo(x, y+height);
	ctx.closePath();
	paintPath(ctx, x, y, width, height, undefined, undefined, fillStyle);
}

function diamondWidth(height)
{
	return height * 83 / 109; // Fixed proportions!
}

function diamond(ctx, x, y_mid, height, lineSize, strokeStyle, fillStyle)
{
	var lof = lineSize/2;
	var width = diamondWidth(height);
	var y = y_mid - height/2;

	ctx.beginPath();
	ctx.moveTo(x+width/2, y+lof);
	ctx.lineTo(x+width-lof, y+height/2);
	ctx.lineTo(x+width/2, y+height-lof);
	ctx.lineTo(x+lof, y+height/2);
	ctx.closePath();
	paintPath(ctx, x, y, width, height, lineSize, strokeStyle, fillStyle);
	return width;
}

function plus(ctx, x, y, height, lineSize, strokeStyle, fillStyle)
{
	var lof = lineSize/2;
	var width = height;
	var len = width / 3;

	ctx.beginPath();
	ctx.moveTo(x+lof, y+len);
	ctx.lineTo(x+len, y+len);
	ctx.lineTo(x+len, y+lof);
	ctx.lineTo(x+width-len, y+lof);
	ctx.lineTo(x+width-len, y+len);
	ctx.lineTo(x+width-lof, y+len);
	ctx.lineTo(x+width-lof, y+height-len);
	ctx.lineTo(x+width-len, y+height-len);
	ctx.lineTo(x+width-len, y+height-lof);
	ctx.lineTo(x+len, y+height-lof);
	ctx.lineTo(x+len, y+height-len);
	ctx.lineTo(x+lof, y+height-len);
	ctx.closePath();
	paintPath(ctx, x, y, width, height, lineSize, strokeStyle, fillStyle);
	return width;
}

function hexagonHeight(height)
{
	return height * 15 / 19;
}

function hexagonWidth(height)
{
	return hexagonHeight(height) * 107 / 93; // Scale width to fit height
}

function hexagon(ctx, x, y_mid, box_height, lineSize, strokeStyle, fillStyle)
{
	var lof = lineSize/2;
	var height = hexagonHeight(box_height); // Scale height so it fits diamond size of same height
	var width = hexagonWidth(box_height);
	var indent = width * 28 / 107; // Indent at the top

	ctx.beginPath();
	ctx.moveTo(x+lof, y_mid);
	ctx.lineTo(x+indent, y_mid-height/2+lof);
	ctx.lineTo(x+width-indent, y_mid-height/2+lof);
	ctx.lineTo(x+width-lof, y_mid);
	ctx.lineTo(x+width-indent, y_mid+height/2-lof);
	ctx.lineTo(x+indent, y_mid+height/2-lof);
	ctx.closePath();
	paintPath(ctx, x, y_mid-height/2, width, height, lineSize, strokeStyle, fillStyle);
	return width;
}

function squareWidth(height)
{
	return height * 13 / 19; // Scale height so it fits diamond size of same height
}

function circleWidth(height)
{
	return height * 16.5 / 19;
}

function symbolSpace(height)
{
	return height / 19;
}

function square(ctx, x, y_mid, box_height, lineSize, strokeStyle, fillStyle)
{
	var lof = lineSize/2;
	var width = squareWidth(box_height); // Scale height so it fits diamond size of same height
	var height = width; // duh!
	var y = y_mid - height/2;

	ctx.beginPath();
	ctx.moveTo(x+lof, y+lof);
	ctx.lineTo(x+width-lof, y+lof);
	ctx.lineTo(x+width-lof, y+height-lof);
	ctx.lineTo(x+lof, y+height-lof);
	ctx.closePath();
	paintPath(ctx, x, y, width, height, lineSize, strokeStyle, fillStyle);
	return width;
}

function circle(ctx, x, y_mid, box_height, lineSize, strokeStyle, fillStyle)
{
	var lof = lineSize/2;
	var width = circleWidth(box_height); // Scale height so it fits diamond size of same height
	var height = width; // duh!
	var y = y_mid - height/2;
	var x_mid = x + width/2;

	ctx.beginPath();
	ctx.arc(x_mid, y_mid, width/2-lof, 0, 2*Math.PI);
	paintPath(ctx, x, y, width, height, lineSize, strokeStyle, fillStyle);
	return width;
}

function halfCircle(ctx, x, y_mid, box_height, lineSize, strokeStyle, fillStyle)
{
	var lof = lineSize/2;
	var width = circleWidth(box_height);
	var height = width; // duh!
	var y = y_mid - height/2;
	var x_mid = x + width/2;

	ctx.beginPath();
	ctx.arc(x_mid, y_mid, width/2-lof, 0.5*Math.PI, 1.5*Math.PI);
	ctx.closePath();
	paintPath(ctx, x, y, width, height, lineSize, strokeStyle, fillStyle);
	return width;
}

function diceWidth(ctx, height, spacing, dice, pixelSize)
{
	var total_width = 0;
	var shape_width = 0;

	dice = dice.replaceAll("|", "");
	for (var i=0; i<dice.length; i++) {
		switch (dice[i]) {
			case "F":
			case "P":
			case "C":
			case "U":
			case "u":
			case "g":
			case "d": shape_width = hexagonWidth(height); break;
			case "A":
			case "D":
			case "E": shape_width = diamondWidth(height); break;
			case "B":
			case "S":
			case "N": shape_width = squareWidth(height); break;
			case ".":
			case "=": shape_width = circleWidth(height); break;
			default:
				ctx.save();
				if (pixelSize === undefined)
					pixelSize = height * 14.0 / 16.0;
				ctx.font = Math.round(pixelSize) + "px 'EotE Symbol'";
				shape_width = ctx.measureText(dice[i]).width;
				ctx.restore();
				break;
		}
		total_width += shape_width;
		switch (dice[i]) {
			case 'F':
			case 'P':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'S':
			case 'E':
			case 'N':
			case 'U':
			case 'u':
			case "g":
			case 'd':
				total_width += symbolSpace(height)*2;
				break;
		}
	}
	return total_width;
}

// Dice sequence:
// F=Force (white)
// P=Proficiency (yellow)
// A=Ability (green)
// B=Boost (blue)
// C=Challenge (red)
// D=Difficulty (purple)
// S=Setback (black)
// E=empty
// N=Negative Setback (black with red dash)
// U=Upgrade Ability
// u=Upgrade Difficulty

// Font:
// Z = white
// a = advantage
// f = failure
// s = success
// t = threat
// x = triumph
// y = despair
// z = black
function dice(ctx, x, y, width, height, spacing, dice, pixelSize)
{
	var i;
	var y_mid = y + height/2;
	var shape_width = 0;
	var line_width;
	var saveFillStyle = ctx.fillStyle;

	dice = dice.replaceAll("|", "");
	var total_width = diceWidth(ctx, height, spacing, dice, pixelSize);

	if (width > 0 && total_width > width) {
		height = height * width / total_width;
		spacing = spacing * width / total_width;
		total_width = width;
	}

	var start_x;
	if (width)
		start_x = x + (width - total_width) / 2;
	else
		start_x = x;
	x = start_x;

	if (height >= 40)
		line_width = 2;
	else if (height >= 28)
		line_width = 1.5;
	else if (height >= 18)
		line_width = 1.2;
	else
		line_width = 1;
	for (i=0; i<dice.length; i++) {
		switch (dice[i]) {
			case 'F':
			case 'P':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'S':
			case 'E':
			case 'N':
			case 'U':
			case 'u':
			case 'g':
			case 'd':
				x += symbolSpace(height);
				break;
		}
		switch (dice[i]) {
			case 'F': shape_width = hexagon(ctx, x, y_mid, height, line_width, "black", "white"); break;
			case 'P': shape_width = hexagon(ctx, x, y_mid, height, line_width, "black", Constant.DICE_YELLOW); break;
			case 'A': shape_width = diamond(ctx, x, y_mid, height, line_width, "black", Constant.DICE_GREEN); break;
			case 'B': shape_width = square(ctx, x, y_mid, height, line_width, "black", Constant.DICE_BLUE); break;
			case 'C': shape_width = hexagon(ctx, x, y_mid, height, line_width, "black", Constant.DICE_RED); break;
			case 'D': shape_width = diamond(ctx, x, y_mid, height, line_width, "black", Constant.DICE_PURPLE); break;
			case 'S': shape_width = square(ctx, x, y_mid, height, line_width, "black", "black"); break;
			case 'E': shape_width = diamond(ctx, x, y_mid, height, line_width, "black"); break;
			case 'N':
				shape_width = square(ctx, x, y_mid, height, line_width, "black", "black");
				drawBox(ctx, x + shape_width / 5.0, y_mid - (shape_width / 5.0) / 2.0, shape_width * 3.0 / 5.0, shape_width / 5.0, 0.5, "orange", "red");
				break;
			case '.':
				shape_width = circle(ctx, x, y_mid, height, 0, "black", "black");
				halfCircle(ctx, x, y_mid, height, 0, "black", "white");
				circle(ctx, x, y_mid, height, line_width, "black");
				break;
			case '=':
				shape_width = circle(ctx, x, y_mid, height, line_width, "black", "red");
				drawBox(ctx, x + shape_width / 5.0, y_mid - (shape_width / 5.0) / 2.0, shape_width * 3.0 / 5.0, shape_width / 5.0, 0.5, "white", "white");
				break;
			case 'U': // Upgrade
				shape_width = hexagon(ctx, x, y_mid, height, line_width, "black", Constant.DICE_YELLOW);
				var plus_width = shape_width * 3.0 / 5.0;
				plus(ctx, x + (shape_width - plus_width) / 2, y_mid - plus_width / 2.0, plus_width, 0.5, "white", "green");
				break;
			case 'u':
				shape_width = hexagon(ctx, x, y_mid, height, line_width, "black", Constant.DICE_RED);
				var plus_width = shape_width * 3.0 / 5.0;
				plus(ctx, x + (shape_width - plus_width) / 2, y_mid - plus_width / 2.0, plus_width, 0.5, "orange", "purple");
				break;
			case 'g':
				shape_width = hexagon(ctx, x, y_mid, height, line_width, "black", "white");
				drawBox(ctx, x + shape_width / 5.0, y_mid - (shape_width / 5.0) / 2.0, shape_width * 3.0 / 5.0, shape_width / 5.0, 0.5, "grey", "black");
				break;
			case 'd':
				shape_width = hexagon(ctx, x, y_mid, height, line_width, "black", Constant.DICE_RED);
				drawBox(ctx, x + shape_width / 5.0, y_mid - (shape_width / 5.0) / 2.0, shape_width * 3.0 / 5.0, shape_width / 5.0, 0.5, "orange", "purple");
				break;
			case 'Z':
			case 'a':
			case 'f':
			case 's':
			case 't':
			case 'x':
			case 'y':
			case 'z':
				ctx.save();
				if (pixelSize === undefined)
					pixelSize = height * 14.0 / 16.0;
				ctx.font = Math.round(pixelSize) + "px 'EotE Symbol'";
				ctx.textBaseline = "middle";
				ctx.fillStyle = "black";
				ctx.fillText(dice[i], x, y_mid);
				shape_width = ctx.measureText(dice[i]).width;
				ctx.restore();
				break;
			default:
				ctx.save();
				if (pixelSize === undefined)
					pixelSize = height * 14.0 / 16.0;
				ctx.font = Math.round(pixelSize) + "px Arial";
				ctx.textBaseline = "middle";
				ctx.fillStyle = "black";
				ctx.fillText(dice[i], x, y_mid+height/8.0);
				shape_width = ctx.measureText(dice[i]).width;
				ctx.restore();
				break;
		}
		x += shape_width;
		switch (dice[i]) {
			case 'F':
			case 'P':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'S':
			case 'E':
			case 'N':
			case 'U':
			case 'u':
			case 'g':
			case 'd':
				x += symbolSpace(height);
				break;
		}
	}

	ctx.fillStyle = saveFillStyle;
	return x - start_x;
}

function drawCircle(ctx, x, y, width, lineSize, strokeStyle, fillStyle) {
	var lof = lineSize/2;
	var radius = (width - lineSize) / 2;

	//console.log("cicle", x, y, width, width);
	ctx.beginPath();
	ctx.arc(x+radius+lof, y+radius+lof, radius, 0, 2*Math.PI);
	paintPath(ctx, x, y, width, width, lineSize, strokeStyle, fillStyle);
	//target(ctx, x+radius+lineSize/2, y+radius+lineSize/2, width);
}

function drawBox(ctx, x, y, width, height, lineSize, strokeStyle, fillStyle) {
	var lof = lineSize/2;

	//console.log("box", x, y, width, width);
	ctx.beginPath();
	ctx.rect(x+lof, y+lof, width-lineSize, height-lineSize);
	paintPath(ctx, x, y, width, height, lineSize, strokeStyle, fillStyle);
	//target(ctx, x+radius+lineSize/2, y+radius+lineSize/2, width);
}

function hLine(ctx, x, y, width, height, lineSize, strokeStyle, alignment) {
	var lof = lineSize/2;

	if (alignment === undefined)
		alignment = "middle";

	ctx.beginPath();
	if (alignment === "top") {
		ctx.moveTo(x, y+lof);
		ctx.lineTo(x+width, y+lof);
	}
	else if (alignment === "bottom") {
		ctx.moveTo(x, y+height-lof);
		ctx.lineTo(x+width, y+height-lof);
	}
	else {
		ctx.moveTo(x, y+height/2);
		ctx.lineTo(x+width, y+height/2);
	}
	paintPath(ctx, x, y, width, height, lineSize, strokeStyle);
}

function vLine(ctx, x, y, width, height, lineSize, strokeStyle) {
	ctx.beginPath();
	ctx.moveTo(x+width/2, y);
	ctx.lineTo(x+width/2, y+height);
	paintPath(ctx, x, y, width, height, lineSize, strokeStyle);
}

function bLine(ctx, x, y, width, height, lineSize, strokeStyle) {
	var lof = lineSize/2;

	ctx.beginPath();
	ctx.moveTo(x, y+height-lof);
	ctx.lineTo(x+width, y+height-lof);
	paintPath(ctx, x, y, width, height, lineSize, strokeStyle);
}

function drawEllipse(ctx, x, y, w, h, lineSize, strokeStyle, fillStyle) {
	var kappa = .5522848,
		ox = (w / 2) * kappa, // control point offset horizontal
		oy = (h / 2) * kappa, // control point offset vertical
		xe = x + w,           // x-end
		ye = y + h,           // y-end
		xm = x + w / 2,       // x-middle
		ym = y + h / 2;       // y-middle

	ctx.beginPath();
	ctx.moveTo(x, ym);
	ctx.bezierCurveTo(x, ym - oy, xm - ox, y, xm, y);
	ctx.bezierCurveTo(xm + ox, y, xe, ym - oy, xe, ym);
	ctx.bezierCurveTo(xe, ym + oy, xm + ox, ye, xm, ye);
	ctx.bezierCurveTo(xm - ox, ye, x, ym + oy, x, ym);
	paintPath(ctx, x, y, w, h, lineSize, strokeStyle, fillStyle);
}

function placeImage(ctx, x, y, width, height, image) {
	var w = image.width;
	var h = image.height;

	// Scale up if required
	if (w < width) {
		h = h * width / w;
		w = width;
	}
	if (h < height) {
		w = w * height / h;
		h = height;
	}
	if (w !== width) {
		var nh = h * width / w;
		if (nh >= height) {
			w = width;
			h = nh;
		}
	}
	if (w !== width && h !== height) {
		w = w * height / h;
		h = height;
	}
	var posx = width/2 - w/2;
	var posy = height/2 - h/2;
	ctx.drawImage(image, x+posx, y+posy, w, h);
}

function target(ctx, x, y, len)
{
	if (len === undefined)
		len = 10;
	ctx.beginPath();
	ctx.moveTo(x-len/2, y);
	ctx.lineTo(x+len/2, y);
	ctx.moveTo(x, y-len/2);
	ctx.lineTo(x, y+len/2);
	ctx.lineWidth = 1;
	ctx.strokeStyle = "red";
	ctx.stroke();
}

function getFontSize(str)
{
	var pos = str.indexOf("px");
	if (pos === -1)
		return 10;
	str = str.substr(0, pos);
	pos = str.lastIndexOf(" ");
	if (pos !== -1)
		str = str.substr(pos);
	return parseInt(str);
}

// Draw text in the box.
// halign = left/center/right
// valign = top/middle/bottom
// margin = value in pixels
function textBox(ctx, text, x, y, width, height, textFont, fillStyle, margin, halign, valign, clip) {
	if (textFont === undefined)
		textFont = "Arial";
	if (fillStyle === undefined)
		fillStyle = "black";
	if (margin === undefined)
		margin = 0;
	if (halign === undefined)
		halign = "center";
	if (valign === undefined)
		valign = "middle";
	if (clip === undefined)
		clip = false;

	if (clip) {
		ctx.save();
		ctx.beginPath();
		ctx.rect(x, y, width, height);
		ctx.clip();
	}

	if (halign === "left")
		x = x + margin;
	else if (halign === "right")
		x = x + width - margin;
	else {
		halign = "center";
		x = x + width / 2;
	}

	if (valign === "top")
		y = y + margin;
	else if (valign === "bottom")
		y = y + height - margin;
	else {
		valign = "middle"
		y = y + height / 2 + margin;
	}

	ctx.font = textFont; // e.g. "bold 30px Arial"
	ctx.textAlign = halign;
	ctx.textBaseline = valign;
	ctx.fillStyle = fillStyle;
	ctx.fillText(text, x, y);

	if (clip)
		ctx.restore();
}

function fontString(format)
{
	var str = "";

	if (format["i"] > 0)
		str += " italic";
	if (format["b"] > 0)
		str += " bold";
	str += " "+format["size"]+"px";
	str += " '"+format["font"]+"'";
	return str.trim();
}

function lineCount(ctx, text, width, lineHeight, font, pixelSize, bold, color)
{
	return setText(ctx, text, 0, 0, width, lineHeight, font, pixelSize, bold, color, 0, false, true);
}

function lineLength(ctx, text, lineHeight, font, pixelSize, bold, color)
{
	return setText(ctx, text, 0, 0, 0, lineHeight, font, pixelSize, bold, color, 0, false, false);
}

function flowText(ctx, text, x, y, width, height, lineHeight, font, pixelSize, bold, color, shiftText, clip)
{
	if (clip) {
		ctx.save();
		ctx.beginPath();
		ctx.rect(x, y, width, height);
		ctx.clip();
	}

	setText(ctx, text, x, y, width, lineHeight, font, pixelSize, bold, color, shiftText, true, true);

	if (clip)
		ctx.restore();
}

function setText(ctx, textData, x, y, width, lineHeight, font, pixelSize, bold, color, shiftText, draw, returnLines)
{
	var lineCount = 1;
	var format = [];
	var text = {
		data: textData
	}

	format["b"] = bold ? 1 : 0;
	format["i"] = 0;
	format["s"] = 0;
	format["u"] = 0;
	format["font"] = font;
	format["size"] = pixelSize;
	format["color"] = color;
	format["lineSize"] = 1;
	format["shiftText"] = shiftText;

	var f = fontString(format); // e.g. "bold 30px Arial"
	ctx.font = f;
	ctx.textBaseline = "middle";
	ctx.fillStyle = color;

	var tx = 0;
	var ty = 0;
	var word;
	var tw;
	var newl;
	while (text.data.length > 0) {
		word = nextWord(ctx, text, format);
		if (word.length === 0)
			break;
		tw = wordLength(ctx, word, lineHeight, format);
		if (width > 0 && tx + tw > width) {
			tx = 0;
			ty += lineHeight;
			lineCount++;
			while (word.length > 0 && isSpace(word)) {
				word = nextWord(ctx, text, format);
			}
			if (word.length === 0)
				break;
			tw = wordLength(ctx, word, lineHeight, font, pixelSize, bold);
			newl = true;
		}
		else
			newl = false;
		if (word === "[P]" || word === "[BR]") {
			tx = 0;
			if (!newl)
				ty += lineHeight;
			if (word === "[P]")
				ty += lineHeight;
			lineCount += newl ? 1 : 2;
			while (word.length > 0) {
				word = nextWord(ctx, text, format);
				if (word === "[P]" || word === "[BR]") {
					ty += lineHeight;
					lineCount++;
				}
				else if (!isSpace(word))
					break;
			}
			if (word.length === 0)
				break;
			tw = wordLength(ctx, word, lineHeight, font, pixelSize, bold);
		}
		if (draw)
			drawWord(ctx, word, x+tx, y+ty, lineHeight, format);
		tx += tw;
	}

	if (returnLines)
		return lineCount;
	return tx;
}

function wordLength(ctx, word, lineHeight, format)
{
	if (word.charAt(0) === '[') {
		var symb = symbolCodes[word];
		if (symb !== undefined)
			return diceWidth(ctx, lineHeight, 2, symb, undefined, format["size"]);
	}
	return ctx.measureText(word).width;
}

function drawWord(ctx, word, x, y, lineHeight, format)
{
	var length = 0;
	var symb;

	if (word.charAt(0) === '[' && (symb = symbolCodes[word]) !== undefined)
		length = dice(ctx, x, y - lineHeight/2, 0, lineHeight, 2, symb, format["size"]);
	else
		ctx.fillText(word, x, y+format["shiftText"]);
	if (format["u"] > 0) {
		if (length === 0)
			length = ctx.measureText(word).width;
		ctx.lineWidth = format["lineSize"];
		ctx.moveTo(x, y + format["size"] / 2 - format["lineSize"] / 2);
		ctx.lineTo(x + length, y + format["size"] / 2 - format["lineSize"] / 2);
		ctx.stroke();
	}
	else if (format["s"] > 0) {
		if (length === 0)
			length = ctx.measureText(word).width;
		ctx.lineWidth = format["lineSize"];
		ctx.moveTo(x, y);
		ctx.lineTo(x + length, y);
		ctx.stroke();
	}
}

var BOLD_OPEN = 1;

function isTag(word)
{
	if (word.length === 3) {
		if (word.charAt(0) === "[" && word.charAt(2) === "]") {
			switch (word.charAt(1)) {
				case 'B': return { open: true, type: "b" };
				case 'b': return { open: false, type: "b" };
				case 'I': return { open: true, type: "i" };
				case 'i': return { open: false, type: "i" };
				case 'U': return { open: true, type: "u" };
				case 'u': return { open: false, type: "u" };
				case 'P': return { open: true, type: "p" };
			}
		}
		return false;
	}
	if (word.length === 4) {
		if (word.charAt(0) === "[" && word.charAt(3) === "]") {
			switch (word.charAt(1)) {
				case 'H':
					switch (word.charAt(2)) {
						case '1': return { open: true, type: "1" };
						case '2': return { open: true, type: "2" };
						case '3': return { open: true, type: "3" };
						case '4': return { open: true, type: "4" };
					}
					break;
				case 'h':
					switch (word.charAt(2)) {
						case '1': return { open: false, type: "1" };
						case '2': return { open: false, type: "2" };
						case '3': return { open: false, type: "3" };
						case '4': return { open: false, type: "4" };
					}
					break;
				case 'B':
					if (word.charAt(2) === 'R')
						return { open: true, type: "r" };
					break;
			}
		}
	}
	return false;
}

function nextWord(ctx, text, format)
{
	var word;
	var tag;

	for (;;) {
		word = getWord(text);
		tag = isTag(word);
		if (tag === false)
			break;

		// Not supported (yet) tags:
		if (format[tag.type] === undefined)
			break;

		format[tag.type] += tag.open ? 1 : -1;
		ctx.font = fontString(format); // e.g. "bold 30px Arial"
	}
	return word;
}

function getWord(text)
{
	var ch;
	var pos;
	var word;

	if (text.data === undefined || text.data.length === 0)
		return "";

	if (isSpace(text.data.charAt(0))) {
		word = text.data.charAt(0);
		pos = 1;
		while (pos < text.data.length && isSpace(text.data.charAt(pos))) {
			word += text.data.charAt(pos);
			pos++;
		}
		text.data = text.data.substr(word.length);
		return " ";
	}

	if (text.data.charAt(0) === '[') {
		pos = 1;
		word = "[";
		for (;;) {
			ch = text.data.charAt(pos);
			if (!((ch >= "A" && ch <= "Z") || (ch >= "a" && ch <= "z") || ch === '/'))
				break;
			word += ch;
			pos++;
		}
		if (ch === "]") {
			word += ']';
			text.data = text.data.substr(word.length);
			return word;
		}
		text.data = text.data.substr(word.length);
		return word;
	}

	pos = 0;
	word = ""
	for (;;) {
		ch = text.data.charAt(pos);
		if (isSpace(ch) || ch === "[")
			break;
		word += ch;
		pos++;
	}
	text.data = text.data.substr(word.length);
	return word;
}

function isSpace(text)
{
	for (var i=0; i<text.length; i++) {
		var ch = text.charAt(i);
		if (ch !== " " && ch !== "\n" && ch !== "\t" && ch !== "\r")
			return false;
	}
	return true;
}
