import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: descBlock
	width: 243
	height: 239

	Canvas {
		id: descBlockCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = descBlockCanvas.getContext("2d");
			var lineSize = 1.5;

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.cornerRect(ctx, 0, 0, 60, 90, 7, Constant.DARK_GRAY, "top-left, top-right");
			Draw.cornerRect(ctx, 0, descBlock.height-(90), 60, 90, 7, Constant.LIGHT_GRAY, "bottom-left, bottom-right");
			Draw.bevelRect(ctx, 16, 4, descBlock.width-16, descBlock.height-8, lineSize, 19, "black", "white");
			Draw.exit(ctx);
		}
	}

	Column {
		x: 24
		y: 8

		InfoLine {
			width: descBlock.width - 24 - 6
			infoTitle: "GENDER:"
			infoText: characterData.gender
			infoType: "desc-box,clear2"
		}

		InfoLine {
			width: descBlock.width - 24 - 6
			infoTitle: "AGE:"
			infoText: characterData.age
			infoType: "desc-box,clear2"
		}

		InfoLine {
			width: descBlock.width - 24 - 6
			infoTitle: "HEIGHT:"
			infoText: characterData.height
			infoType: "desc-box,clear2"
		}

		InfoLine {
			width: descBlock.width - 24 - 6
			infoTitle: "BUILD:"
			infoText: characterData.build
			infoType: "desc-box,clear2"
		}

		InfoLine {
			width: descBlock.width - 24 - 6
			infoTitle: "HAIR:"
			infoText: characterData.hair
			infoType: "desc-box,clear2"
		}

		InfoLine {
			width: descBlock.width - 24 - 6
			infoTitle: "EYES:"
			infoText: characterData.eyes
			infoType: "desc-box,clear2"
		}
	}
}
