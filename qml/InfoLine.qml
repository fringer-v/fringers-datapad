import QtQuick 2.3
import QtQuick.Window 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: infoline
	height: getInfoHeight()
	width: 516
	color: "#00000000"

	property string infoTitle: ""
	property string infoText: ""
	property string infoComment: ""
	property string infoType: "" // left|right [ ,large-font ] [ ,wide-row ] [,desc-box] [, obligation ] [,clear]
	property string infoColor: "black"
	property string infoBackground: "white"

	property int infoLeftMargin: getInfoLeftMargin()
	property string infoTextFont: getInfoTextFont()
	property int infoTextMargin: infoType.indexOf("large-font") >= 0 ? -1 : 2
	property int infoTextIndent: getInfoTextIndent()
	property string infoTitleColor: getInfoTitleColor()
	property string infoTitleFont: getInfoTitleFont()
	property int infoTitleMargin: 3
	property real margin: 2.5
	property real spacing: 3
	property int diceHeight: 18

	property int pixelRatio: Screen.devicePixelRatio

	Canvas {
		id: infolingcanvas
		anchors.top: parent.top
		anchors.left: parent.left
		x: 0
		y: 0
		width: parent.width * pixelRatio
		height: parent.height * pixelRatio

		transform: Scale {
			xScale: 1/pixelRatio
			yScale: 1/pixelRatio
		}

		onPaint: {
			var ctx = infolingcanvas.getContext("2d");

			ctx.save();
			ctx.scale(pixelRatio, pixelRatio)
			if (infoType.indexOf("clear2") >= 0)
				Draw.drawBox(ctx, 0, parent.height/2-1, parent.width, parent.height/2+1,
					0, undefined, infoBackground);
			else if (infoType.indexOf("clear") >= 0)
				Draw.drawBox(ctx, 0, 0, parent.width, parent.height, 0, undefined, infoBackground);
			Draw.bLine(ctx, 0, 0, parent.width, parent.height, 1.5, infoColor);
			Draw.textBox(ctx, infoTitle, infoLeftMargin, 0, parent.width-infoLeftMargin, parent.height,
				infoTitleFont, infoTitleColor, infoTitleMargin, "left", "bottom");
			if (infoType.indexOf("dice") >= 0)
				Draw.dice(ctx, infoLeftMargin+infoTextIndent, parent.height - diceHeight - 3,
					/*parent.width-infoLeftMargin-infoTextIndent*/0, diceHeight, spacing, infoText);
			else {
				if (infoType.indexOf("flow-text") >= 0) {
					//var hMargin=0;
					var vMargin=0;
					var lineCount=1;
					var font="Comic Sans MS";
					var pixelSize=13;
					var lineHeight=pixelSize+4;
					var bold=false;
					var textColor="black";
					var shiftText=0;
					var clip=false;
					var y = parent.height - lineHeight / 2 - 1;
					Draw.flowText(ctx, infoText, infoLeftMargin+infoTextIndent, y, parent.width, parent.height-vMargin*2,
						lineHeight, font, pixelSize, bold, textColor, shiftText, clip);
				}
				else {
					Draw.textBox(ctx, infoText, infoLeftMargin+infoTextIndent, 0, parent.width-infoLeftMargin-infoTextIndent,
						parent.height, infoTextFont, infoColor, infoTextMargin, "left", "bottom");
				}
				if (infoComment !== "") {
					Draw.textBox(ctx, infoComment, infoLeftMargin+infoTextIndent, 0, parent.width-infoLeftMargin-infoTextIndent,
						parent.height, infoTextFont, "darkgrey", infoTextMargin, "right", "bottom");
				}
			}
			ctx.restore();
		}
	}

	onInfoTextChanged: infolingcanvas.requestPaint();
	onInfoBackgroundChanged: infolingcanvas.requestPaint();

	function redraw()
	{
		infolingcanvas.requestPaint();
	}

	function getInfoTitleColor()
	{
		if (infoType.indexOf("desc-box") >= 0 || infoType.indexOf("obligation") >= 0)
			return infoColor;
		return Constant.INFO_TITLE_GRAY;
	}

	function getInfoHeight()
	{
		if (infoType.indexOf("char-box") >= 0)
			return 30;
		if (infoType.indexOf("obligation") >= 0)
			return 30;
		if (infoType.indexOf("check-item") >= 0)
			return 30;
		if (infoType.indexOf("desc-box") >= 0)
			return 35;
		if (infoType.indexOf("large-font") >= 0 || infoType.indexOf("wide-row") >= 0)
			return 32;
		return 28;
	}

	function getInfoTextFont()
	{
		var bold = "";

		if (infoType.indexOf("bold") >= 0 || infoType.indexOf("large-font") >= 0)
			bold = "bold ";
		if (infoType.indexOf("char-box") >= 0)
			return bold + "13px 'Comic Sans MS'";
		if (infoType.indexOf("obligation") >= 0)
			return bold + "13px 'Comic Sans MS'";
		if (infoType.indexOf("check-item") >= 0)
			return bold + "13px 'Comic Sans MS'";
		if (infoType.indexOf("desc-box") >= 0)
			return bold + "14px 'Comic Sans MS'";
		if (infoType.indexOf("large-font") >= 0)
			return "26px 'Comic Sans MS'";
		return bold + "17px 'Comic Sans MS'";
	}

	function getInfoTextIndent()
	{
		if (infoType.indexOf("char-box") >= 0)
			return 85;
		if (infoType.indexOf("obligation") >= 0)
			return 120;
		if (infoType.indexOf("check-item") >= 0) {
			if (infoType.indexOf("right") >= 0)
				return 100;
			return 80;
		}
		if (infoType.indexOf("desc-box") >= 0)
			return 80;
		if (infoType.indexOf("right") >= 0)
			return 150;
		return 190;
	}

	function getInfoLeftMargin()
	{
		if (infoType.indexOf("char-box") >= 0)
			return -infoTitleMargin;
		if (infoType.indexOf("obligation") >= 0)
			return 0;
		if (infoType.indexOf("check-item") >= 0)
			return 3;
		if (infoType.indexOf("desc-box") >= 0)
			return 5;
		return 6;
	}

	function getInfoTitleFont()
	{
		if (infoType.indexOf("char-box") >= 0)
			return "bold 14px Arial";
		if (infoType.indexOf("obligation") >= 0)
			return "bold 14px Arial";
		if (infoType.indexOf("check-item") >= 0)
			return "bold 14px Arial";
		if (infoType.indexOf("desc-box") >= 0)
			return "bold 15px Arial";
		return "bold 17px Arial"
	}
}
