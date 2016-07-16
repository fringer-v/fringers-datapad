import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: acquiredXp
	width: 389
	height: 115

	Canvas {
		id: acquiredXpCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = acquiredXpCanvas.getContext("2d");

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.cornerRect(ctx, 0, 6, 26, 38, 4, Constant.LIGHT_GRAY, "top-left");
			Draw.cornerRect(ctx, 0, acquiredXp.height-38, 26, 38, 4, Constant.LIGHT_GRAY, "bottom-left");
			Draw.bevelRect(ctx, 7, 9, acquiredXp.width-7, acquiredXp.height - 9 - 3, 1.5, 13, "black", "white");
			Draw.bevelRect(ctx, 7 + (acquiredXp.width-7 - 300) / 2, 0, 300, 19, 1.5, 4, Constant.LIGHT_GRAY, Constant.LIGHT_GRAY);
			Draw.textBox(ctx, "EARNED & ACQUIRED", 7 + (acquiredXp.width-7 - 300) / 2, 0, 300, 19, "bold 13px Arial", "black", 1, "center", "middle");
			Draw.exit(ctx);
		}
	}

	XPList {
		x: 18
		y: 20
		width: parent.width - 30
		height: parent.height - 28
	}
}
