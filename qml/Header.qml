import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Canvas {
	id: header
	height: Constant.HEADER_HEIGHT
	width: 500

	property string headText: "TEST"
	property int headLen: 224
	property int headBevel: 24

	onPaint: {
		var ctx = header.getContext("2d");

		Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
		Draw.hLine(ctx, 0, 0, header.width, header.height, 2, "black");
		Draw.pointyRect(ctx, (header.width - header.headLen) / 2, 0, header.headLen, header.height, 1, header.headBevel, "black", Constant.HEADER_COLOR);
		Draw.textBox(ctx, headText, (header.width - header.headLen) / 2, 0, header.headLen, header.height, "bold 15px Arial", "black", 1);

		Draw.exit(ctx);
	}
}
