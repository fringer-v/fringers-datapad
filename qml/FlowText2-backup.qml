import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

// ================================ ORIGINAL FLOWTEXT:
Canvas {
	id: flowText
	height: lineCount === 0 ? 0 : lineCount*lineHeight+vMargin*2
	/*
	width: {
		if (!completed || !available || printText == undefined)
			return 0;
		var ctx = flowText.getContext("2d");
		return Draw.lineLength(ctx, printText, lineHeight, font, pixelSize, bold, color);
	}
	*/

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
	//property bool updated: false
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
			//updated = true;
		}
	}

	/*
	onAvailableChanged: {
		flowText.requestPaint();
	}

	// Hell of a draw/redraw problem lead to all this, which should not be required!
	onHeightChanged: {
		flowText.requestPaint();
	}

	onWidthChanged: {
		flowText.requestPaint();
	}
	*/

	Component.onCompleted: {
		printText = text;
		//updated = false;
		completed = true;
		flowText.requestPaint();
	}

	onTextChanged: {
		if (completed) {
			if (printText != text) {
				printText = text;
				//updated = false;
				flowText.requestPaint();
			}
		}
	}
}

/* ORIGINAL


import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Canvas {
	id: flowText
	height: lineCount === 0 ? 0 : lineCount*lineHeight+vMargin*2

	property int lineCount: {
		if (!available || printText == undefined || flowText.width === 0)
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
	property string printText: undefined
	property bool updated: false

	onPaint: {
		if (!available || printText == undefined)
			return;
		var ctx = flowText.getContext("2d");

		Draw.enter(ctx);
		var y = (flowText.height - ((lineCount-1)*lineHeight)) / 2;
		Draw.flowText(ctx, printText, hMargin, y, flowText.width-hMargin*2, flowText.height-vMargin*2,
			lineHeight, font, pixelSize, bold, color, clip);
		Draw.exit(ctx);
	}

	onAvailableChanged: {
		if (!updated && available && width > 0 && height > 0) {
			//flowText.markDirty(Qt.rect(0, 0, flowText.width, flowText.height));
			flowText.requestPaint();
			updated = true;
		}
	}

	// Hell of a draw/redraw problem lead to all this, which should not be required!
	onHeightChanged: {
		if (!updated && available && width > 0 && height > 0) {
			//flowText.markDirty(Qt.rect(0, 0, flowText.width, flowText.height));
			flowText.requestPaint();
			updated = true;
		}
	}

	onWidthChanged: {
		if (!updated && available && width > 0 && height > 0) {
			//flowText.markDirty(Qt.rect(0, 0, flowText.width, flowText.height));
			flowText.requestPaint();
			updated = true;
		}
	}

	Component.onCompleted: {
		printText = text;
		if (!updated && available && width > 0 && height > 0) {
			//flowText.markDirty(Qt.rect(0, 0, flowText.width, flowText.height));
			flowText.requestPaint();
			updated = true;
		}
	}
}
*/

/*
  =======================================================================================
import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
*/

/*
Rectangle {
	id: flowText
	//visible: false

	//property int lineCount: {
	//	if (width > 0) {
	//		var ctx = calcCanvas.getContext("2d");
	//		//var len = Draw.lineCount(ctx, text, width-hMargin*2, lineHeight, font, pixelSize, bold, "black");
	//		return 2;
	//	}
	//	return 0;
	//}
	property int lineHeight: 16
	property string font: "Arial"
	property int pixelSize: 14
	property bool bold: false
	property string textColor: "green"
	property bool clip: false
	property int vMargin: 0
	property int hMargin: 0
	property string text: ""
	property int whichIndex: 0

	Rectangle {
		id: heightRect
		visible: false;
		width: flowText.width
		height: {
			if (width === 0)
				return 0;
			var ctx = calcCanvas.getContext("2d");
			var lc = Draw.lineCount(ctx, text, width-hMargin*2, lineHeight, font, pixelSize, bold, "black");
			return lc*lineHeight;
		}

		Canvas {
			id: calcCanvas
			anchors.fill: parent
			visible: false;
		}
	}

	Canvas {
		id: flowTextCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = flowTextCanvas.getContext("2d");

			console.debug("---- PAINTING "+flowText.width+" "+height+" "+flowText.text);
			Draw.enter(ctx);
			var lc = Draw.lineCount(ctx, text, width-hMargin*2, lineHeight, font, pixelSize, bold, "black");
			var y = (flowText.height - ((lc-1)*flowText.lineHeight)) / 2;
			Draw.flowText(ctx, flowText.text, flowText.hMargin, y, flowText.width-flowText.hMargin*2, flowText.height-flowText.vMargin*2,
				flowText.lineHeight, flowText.font, flowText.pixelSize, flowText.bold, flowText.textColor, flowText.clip);
			Draw.exit(ctx);
		}
	}

	onHeightChanged: {
		console.debug(">>>> "+whichIndex+" height="+height+" width="+width);
		//if (width != 0) {
		//	visible = true;
		//	textColor = "black";
		//	flowTextCanvas.requestPaint();
		//}
	}
}
*/

/*
Canvas {
	id: flowText
	height: lineCount === 0 ? 0 : lineCount*lineHeight+vMargin*2

	property int lineCount: {
		if (flowText.width === 0)
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
	property string printText: ""
	property int shiftText: 0

	onPaint: {
		var ctx = flowText.getContext("2d");

		//ctx.canvas.markDirty(Qt.rect(0, 0, 500, 500));
		Draw.enter(ctx);
		//var lc = Draw.lineCount(ctx, text, flowText.width-hMargin*2, lineHeight, font, pixelSize, bold, color);
		var y = (flowText.height - ((lineCount-1)*lineHeight)) / 2;
		//console.debug("---- PAINTING "+whichIndex+" width="+width+" height="+height+" lines="+lineCount+" y="+y+" "+flowText.text);
		//console.debug("---- width="+ctx.canvas.width+" height="+ctx.canvas.height);
		//Draw.drawBox(ctx, 0, 0, width, height, 1, "yellow", "yellow");
		Draw.flowText(ctx, printText, hMargin, y, flowText.width-hMargin*2, flowText.height-vMargin*2,
			lineHeight, font, pixelSize, bold, color, shiftText, clip);
		Draw.exit(ctx);
	}

	Component.onCompleted: {
		printText = text;
	}
}
*/

/*
Canvas {
	id: flowText
	height: lineCount*lineHeight+topMargin+bottomMargin

	property int lineCount: {
		var ctx = flowText.getContext("2d");
		return Draw.lineCount(ctx, text, flowText.width-leftMargin-rightMargin, lineHeight, font, pixelSize, bold, color);
	}
	property int lineHeight: pixelSize * 16 / 14
	property string font: "Arial"
	property int pixelSize: 14
	property bool bold: false
	property string color: "black"
	property bool clip: false
	property int topMargin: margin
	property int bottomMargin: margin
	property int leftMargin: margin
	property int rightMargin: margin
	property int margin: 0
	property string text: ""

	onPaint: {
		var ctx = flowText.getContext("2d");

		Draw.enter(ctx);
		var x = leftMargin;
		var y = flowText.height - ((lineCount-1)*lineHeight) / 2;
		Draw.flowText(ctx, text, x, y, flowText.width-leftMargin-rightMargin, flowText.height-topMargin-bottomMargin,
			lineHeight, font, pixelSize, bold, color, clip);
		Draw.exit(ctx);
	}
}
*/
