import QtQuick 2.0
import QtQuick.Window 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	height: lineCount === 0 ? 0 : lineCount*lineHeight+vMargin*2
	color: "#00000000"

	property int lineCount: {
		if (!completed || !flowText.available || printText == "*undef*" || width === 0)
			return 0;
		var ctx = flowText.getContext("2d");
		return Draw.lineCount(ctx, printText, width-hMargin*2, lineHeight, font, pixelSize, bold, textColor);
	}

	property int lineHeight: 16
	property string font: "Arial"
	property int pixelSize: 14
	property bool bold: false
	property string textColor: "black"
	property bool clip: false
	property int vMargin: 0
	property int hMargin: 0
	property int whichIndex: 0
	property string clearColor: ""
	property int shiftText: 0
	property string text: ""

	// Private:
	property int pixelRatio: Screen.devicePixelRatio
	property string printText: "*undef*"
	property bool completed: false

	Canvas {
		id: flowText
		anchors.top: parent.top
		anchors.left: parent.left
		x: 0
		y: 0
		width: parent.width * pixelRatio
		height: parent.height * pixelRatio
		//scale: 1/pixelRatio

		transform: Scale {
			xScale: 1/pixelRatio
			yScale: 1/pixelRatio
		}

		onPaint: {
			if (completed && available && printText != "*undef*" && width > 0 && height > 0) {
				var ctx = flowText.getContext("2d");

				ctx.save();
				ctx.scale(pixelRatio, pixelRatio)
				var y = (parent.height - ((lineCount-1)*lineHeight)) / 2;
				if (clearColor.length !== 0)
					Draw.drawBox(ctx, hMargin, vMargin, parent.width-hMargin*2, parent.height-vMargin*2, 1, clearColor, clearColor);
				Draw.flowText(ctx, printText, hMargin, y, parent.width-hMargin*2, parent.height-vMargin*2,
					lineHeight, font, pixelSize, bold, textColor, shiftText, clip);
				ctx.restore();
			}
		}
	}

	onTextChanged: {
		if (completed) {
			if (printText != text) {
				printText = text;
				flowText.requestPaint();
			}
		}
	}

	Component.onCompleted: {
		printText = text;
		completed = true;
		flowText.requestPaint();
	}
}

