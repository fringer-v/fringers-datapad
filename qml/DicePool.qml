import QtQuick 2.3
import QtQuick.Window 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	color: "#00000000"

	property real margin: 2.5
	property real spacing: 3
	property string pool: "EEEEE"
	property bool center: true
	property string backgroundColor: ""
	property string placeHolderText: ""

	property int pixelRatio: Screen.devicePixelRatio

	Canvas {
		id: diceCanvas
		anchors.top: parent.top
		anchors.left: parent.left
		x: 0
		y: 0
		width: parent.width * pixelRatio
		height: parent.height * pixelRatio
		antialiasing: true
		smooth: true

		transform: Scale {
			xScale: 1/pixelRatio
			yScale: 1/pixelRatio
		}

		onPaint: {
			var ctx = diceCanvas.getContext("2d");
			var ht = parent.height - margin*2;
			var pixelSize = ht * 14.0 / 16.0;

			ctx.save();
			ctx.scale(pixelRatio, pixelRatio)
			if (backgroundColor != undefined && backgroundColor.length != 0)
				Draw.drawBox(ctx, 0, 0, parent.width, parent.height, 1, backgroundColor, backgroundColor);
			//if (pool.substring(0, 1) == "@") {
			//	Draw.textBox(ctx, pool.substring(1), 0, margin, parent.width, ht, Math.round(pixelSize) + "px Arial", "black", 0, center ? "center": "left", "bottom");
			//}
			//else {
			//}
			if (pool.substring(0, 1) == "@")
				pool = pool.substring(1);
			if (pool === "" && placeHolderText !== "") {
				Draw.textBox(ctx, placeHolderText, 0, margin, parent.width, ht, Math.round(pixelSize) + "px Arial", "darkgrey", 0, center ? "center": "left", "bottom");
			}
			else {
				if (center)
					Draw.dice(ctx, 0, margin, parent.width, ht, spacing, pool);
				else {
					var w = Draw.diceWidth(ctx, ht, spacing, pool);
					Draw.dice(ctx, 0, margin, w, ht, spacing, pool);
				}
			}
			ctx.restore();
		}
	}

	onPoolChanged: {
		diceCanvas.requestPaint();
	}
}
