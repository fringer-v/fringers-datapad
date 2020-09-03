import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

/*
Rectangle {
	id: flowText
	height: heightRect.height+vMargin*2
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

Canvas {
	id: canvas
	visible: false
	height: lineCount*lineHeight+vMargin*2

	property int lineCount: {
		if (canvas.width)
			return 0;
		var ctx = canvas.getContext("2d");
		return Draw.lineCount(ctx, text, canvas.width-hMargin*2, lineHeight, font, pixelSize, bold, "black");
	}
	property int lineHeight: 16
	property string font: "Arial"
	property int pixelSize: 14
	property bool bold: false
	property int vMargin: 0
	property int hMargin: 0
	property string text: ""
}

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
