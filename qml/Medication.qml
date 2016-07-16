import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: medication
	width: 239
	height: 124

	property string mediText: "STIMPACK"
	property int imageWidth: imageHeight * 129 / 96
	property int imageHeight: 58
	property string usageColor: Constant.LIST_LINE_GRAY
	property string inventory: "0"
	property int used: 0

	Canvas {
		id: medicationCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = medicationCanvas.getContext("2d");

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.cornerRect(ctx, 0, 6, 26, 38, 4, Constant.LIGHT_GRAY, "top-left");
			Draw.cornerRect(ctx, 0, medication.height-38, 26, 38, 4, Constant.LIGHT_GRAY, "bottom-left");
			Draw.bevelRect(ctx, 7, 9, medication.width-7, medication.height - 9 - 3, 1.5, 13, "black", "white");
			Draw.bevelRect(ctx, 7 + (medication.width-7 - 180) / 2, 0, 180, 19, 1.5, 4, Constant.LIGHT_GRAY, Constant.LIGHT_GRAY);
			Draw.textBox(ctx, mediText, 7 + (medication.width-7 - 180) / 2, 0, 180, 19, "bold 13px Arial", "black", 1, "center", "middle");
			Draw.drawBox(ctx, 115, 28, 16, 80, 1, usageColor, usageColor);
			Draw.drawBox(ctx, 115+24, 28, 16, 80, 1, usageColor, usageColor);
			Draw.drawBox(ctx, 115+24+24, 28, 16, 80, 1, usageColor, usageColor);
			Draw.drawBox(ctx, 115+24+24+24, 28, 16, 80, 1, usageColor, usageColor);
			Draw.drawBox(ctx, 115+24+24+24+24, 28, 16, 80, 1, usageColor, usageColor);
			var x = 115;
			for (var i=0; i<used; i++) {
				Draw.drawBox(ctx, x, 28, 16, 80, 1, "black", usageColor);
				x += 24;
			}
			Draw.exit(ctx);
		}
	}

	onUsedChanged: {
		medicationCanvas.requestPaint();
	}

	MouseArea {
		x: 115
		y: 28
		width: 24*4+16
		height: 80

		onClicked: {
			if (mediText == "STIMPACK")
				characterData.useStimPack(1);
			else
				characterData.useErp(1);
		}
	}

	Image {
		x: 7 + 15
		y: 9 + 15
		width: imageWidth
		height: imageHeight
		source: mediText == "STIMPACK" ? "qrc:/images/stimpack.png" : "qrc:/images/erp.png"
	}

	Text {
		x: 7 + 15
		y: 9 + 15 + imageHeight
		width: imageWidth
		height: medication.height - (9 + 15 + imageHeight) - 8
		font.pixelSize: 18
		font.family: "Arial"
		font.bold: true
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		text: inventory + "x"
	}
}
