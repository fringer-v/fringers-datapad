import QtQuick 2.3
import QtQuick.Window 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	height: lineCount === 0 ? 0 : lineCount*lineHeight+vMargin*2
	color: "#00000000"

	property int lineCount: {
		if (!completed || !flowText.available || printText == undefined || width === 0)
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
	property string text: ""
	property int whichIndex: 0
	property string clearColor: ""
	property int shiftText: 0

	// Private:
	property string printText: undefined
	property bool completed: false

	Canvas {
		id: flowText
		//anchors.top: parent.top
		//anchors.left: parent.left
		anchors.fill: parent
		//width: parent.width * ratio
		//height: parent.height * ratio
		//scale: 1/ratio

		property int ratio: Screen.devicePixelRatio

		onPaint: {
			if (completed && available && printText != undefined && width > 0 && height > 0) {
				var ctx = flowText.getContext("2d");

				ctx.save();
				//ctx.scale(ratio, ratio)
				var y = (flowText.height - ((lineCount-1)*lineHeight)) / 2;
				if (clearColor.length !== 0)
					Draw.drawBox(ctx, hMargin, vMargin, flowText.width-hMargin*2, flowText.height-vMargin*2, 1, clearColor, clearColor);
				Draw.flowText(ctx, printText, hMargin, y, flowText.width-hMargin*2, flowText.height-vMargin*2,
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

/*
ACTUALLY WORKING:

import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Canvas {
	id: flowText
	height: lineCount === 0 ? 0 : lineCount*lineHeight+vMargin*2

	property int lineCount: {
		if (!completed || !available || printText == undefined || flowText.width === 0)
			return 0;
		var ctx = flowText.getContext("2d");
		return Draw.lineCount(ctx, printText, flowText.width-hMargin*2, lineHeight, font, pixelSize, bold, color);
	}

	property int lineHeight: 16
	property string font: "Arial"
	property int pixelSize: 14
	property bool bold: false
	property string color: "black"
	property bool clip: false
	property int vMargin: 0
	property int hMargin: 0
	property string text: ""
	property int whichIndex: 0
	property string clearColor: ""
	property int shiftText: 0

	// Private:
	property string printText: undefined
	property bool completed: false

	onPaint: {
		if (completed && available && printText != undefined && width > 0 && height > 0) {
			var ctx = flowText.getContext("2d");

			Draw.enter(ctx);
			var y = (flowText.height - ((lineCount-1)*lineHeight)) / 2;
			if (clearColor.length !== 0)
				Draw.drawBox(ctx, hMargin, vMargin, flowText.width-hMargin*2, flowText.height-vMargin*2, 1, clearColor, clearColor);
			Draw.flowText(ctx, printText, hMargin, y, flowText.width-hMargin*2, flowText.height-vMargin*2,
				lineHeight, font, pixelSize, bold, color, shiftText, clip);
			Draw.exit(ctx);
		}
	}

	Component.onCompleted: {
		printText = text;
		completed = true;
		flowText.requestPaint();
	}

	onTextChanged: {
		if (completed) {
			if (printText != text) {
				printText = text;
				flowText.requestPaint();
			}
		}
	}
}

*/
