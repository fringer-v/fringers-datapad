import QtQuick 2.0
import "../js/drawing.js" as Draw

Rectangle {
	id: hLine
	width: 10
	height: 5
	property real lineSize: 1.5
	property string strokeStyle: "black"
	property string alignment: "middle"

	Canvas {
		id: hLineCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = hLineCanvas.getContext("2d");

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.hLine(ctx, 0, 0, hLine.width, hLine.height, lineSize, strokeStyle, alignment);
			Draw.exit(ctx);
		}
	}
}
