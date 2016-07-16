import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Canvas {
	id: notes
	height: 200
	width: 1008

	onPaint: {
		var ctx = notes.getContext("2d");
		var lineSize = 1.5;
		var offset = (25-lineSize)/2;
		var h_corner = Math.min(72, (notes.height-offset-4)/2);

		Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);

		//Draw.bevelRect(ctx, 0, 0, notes.width, notes.height, 2, 20, undefined, "");
		Draw.cornerRect(ctx, 0, offset-4, 80, h_corner, 8, Constant.DARK_GRAY, "top-left");
		Draw.cornerRect(ctx, notes.width-80, offset-4, 80, h_corner, 8, Constant.DARK_GRAY, "top-right");
		Draw.cornerRect(ctx, 0, notes.height-h_corner, 80, h_corner, 8, Constant.LIGHT_GRAY, "bottom-left");
		Draw.cornerRect(ctx, notes.width-80, notes.height-h_corner, 80, h_corner, 8, Constant.LIGHT_GRAY, "bottom-right");
		Draw.bevelRect(ctx, 20, offset, notes.width-40, notes.height-offset-4, lineSize, 20, "black", "white");
		Draw.bevelRect(ctx, (notes.width - 140)/2, 0, 140, 25, lineSize, 7, "black", "white");
		Draw.textBox(ctx, "NOTES", (notes.width - 140)/2, 0, 140, 25, "bold 16pt Arial", "black", 2);

		Draw.exit(ctx);
	}
}
