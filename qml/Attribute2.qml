import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Canvas {
	id: attribute2
	height: 92
	width: 167
	property string attrText: ""
	property int textFontSize: 14
	property string attrSubText1: "VALUE"
	property string attrSubText2: "THRESHOLD"
	property int subTextFontSize: 12
	property int subTextOffset: 2
	property int attrValue1: 6
	property int attrValue2: 6
	property int valueFontSize: 30

	onAttrValue1Changed: requestPaint()
	onAttrValue2Changed: requestPaint()

	onPaint: {
		var ctx = attribute2.getContext("2d");

		Draw.enter(ctx);

		Draw.bevelRect(ctx, 0, 31, attribute2.width, 61, 1, 11, undefined, Constant.LIGHT_GRAY);
		Draw.beginClip(ctx, 0, 31, attribute2.width/2, 61);
		Draw.bevelRect(ctx, 0, 31, attribute2.width, 61, 1, 11, undefined, Constant.DARK_GRAY);
		Draw.endClip(ctx);

		Draw.vLine(ctx, 0, 31, attribute2.width, 60, 1, "black");

		Draw.bevelRect(ctx, 7, 0, attribute2.width-14, 74, 1.5, 20, "black", "white");
		Draw.vLine(ctx, 7, 6+24, attribute2.width-14, 74-(6+24), 1.5, "black");
		Draw.pointyRect(ctx, 17, 6, attribute2.width-34, 24, 1.5, 12, "black");

		Draw.textBox(ctx, attrText, 17, 6, attribute2.width-34, 24, "bold "+textFontSize+"px Arial", "black", 1);
		Draw.textBox(ctx, attrValue1, 7, 28, (attribute2.width-14)/2, 74-28, "bold "+valueFontSize+"px Verdana", "black", 1);
		Draw.textBox(ctx, attrValue2, 7+(attribute2.width-14)/2, 28, (attribute2.width-14)/2, 74-28, "bold "+valueFontSize+"px Verdana", "black", 1);
		Draw.textBox(ctx, attrSubText1, 0, 31, attribute2.width/2, 61, "bold "+subTextFontSize+"px Arial", "black", subTextOffset, "center", "bottom");
		Draw.textBox(ctx, attrSubText2, attribute2.width/2, 31, attribute2.width/2, 61, "bold "+subTextFontSize+"px Arial", "black", subTextOffset, "center", "bottom");

		Draw.exit(ctx);
	}
}
