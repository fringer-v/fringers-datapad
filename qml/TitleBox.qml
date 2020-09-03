import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: titleBox
	width: 389
	height: 115

	property string boxTitle: ""
	property string boxText: ""
	property string boxComment: ""
	property int textIndent: 0
	property int boxTitleWidth: 180 // 229
	property int boxCornerHeight: 38
	property bool displayDicePool: false
	property bool displayText: false

	Canvas {
		id: titleBoxCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = titleBoxCanvas.getContext("2d");

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.cornerRect(ctx, 0, 6, 26, boxCornerHeight, 4, Constant.DARK_GRAY, "top-left");
			Draw.cornerRect(ctx, 0, titleBox.height-boxCornerHeight, 26, boxCornerHeight, 4, Constant.LIGHT_GRAY, "bottom-left");
			Draw.bevelRect(ctx, 7, 9, titleBox.width-7, titleBox.height - 9 - 3, 1.5, 9, "black", "white");
			Draw.bevelRect(ctx, 7 + (titleBox.width-7 - boxTitleWidth) / 2, 0, boxTitleWidth, 19, 1.5, 4, Constant.LIGHT_GRAY, Constant.LIGHT_GRAY);
			Draw.textBox(ctx, boxTitle, 7 + (titleBox.width-7 - boxTitleWidth) / 2, 0, boxTitleWidth, 19, "bold 13px Arial", "black", 1, "center", "middle");
			Draw.exit(ctx);
		}
	}

	onBoxTitleChanged: titleBoxCanvas.requestPaint()

	Text {
		visible: displayText
		width: titleBox.width-30-textIndent
		height: titleBox.height-28
		x: 20+textIndent
		y: 26
		horizontalAlignment: Text.AlignHCenter
		font.bold: true
		font.pixelSize: 18
		font.family: "Verdana"
		textFormat: Text.RichText
		clip: true
		text: boxText
	}

	NotesArea {
		visible: !displayDicePool && !displayText
		width: titleBox.width-30-textIndent
		height: titleBox.height-28
		x: 20+textIndent
		y: 20
		text: boxText
	}

	DicePool {
		visible: displayDicePool && !displayText
		width: titleBox.width-30
		height: titleBox.height-28
		x: 20
		y: 20
		margin: 4
		spacing: 3
		pool: boxText
		color: "white"
		backgroundColor: "white"
	}
}
