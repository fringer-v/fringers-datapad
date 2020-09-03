import QtQuick 2.3
import "../js/drawing.js" as Draw

Rectangle {
	width: 445
	height: 214
	property int crit_value: 6

	Canvas {
		id: criticalinjuries
		anchors.fill: parent

		onPaint: {
			var ctx = criticalinjuries.getContext("2d");

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);

			Draw.bevelRect(ctx, 0, 0, 445, 214, 1.5, 20, "black", "white");
			Draw.bevelRect(ctx, 14, 6, 417, 43, 1.5, 13, "black");

			Draw.textBox(ctx, "CRITICAL INJURIES", 0, 0, 445, 28, "bold 15px Arial", "black", 1, "center", "bottom");
			Draw.textBox(ctx, "RESULT", 0, 0, 181, 49, "bold 13px Arial", "black", 3, "right", "bottom");
			Draw.textBox(ctx, "SEVERITY", 0, 0, 406, 49, "bold 13px Arial", "black", 3, "right", "bottom");

			Draw.exit(ctx);
		}
	}

	InjuryList {
		x: 9
		y: 52 - 3
		width: 429
		height: 35*4 + 9
		fadeHeaderHeight: 3
		fadeFooterHeight: 6
	}

	/*
	Column {
		x: 9
		y: 52

		InjuryLine {
			width: 429
		}

		InjuryLine {
			width: 429
		}

		InjuryLine {
			width: 429
		}

		InjuryLine {
			width: 429
		}
	}
	*/
}
