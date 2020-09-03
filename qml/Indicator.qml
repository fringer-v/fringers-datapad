import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Canvas {
	id: attribute2
	width: 64
	height: 28
	property string attrText: ""
	property int attrValue1: 0
	property int attrValue2: 0
	property int attrMaxValue1: 0
	property int attrMaxValue2: 0
	property int valueFontSize: 14
	property int picSize: 16
	property string picUrl: "qrc:/images/white-success17.png"

	onAttrValue1Changed: requestPaint()
	onAttrValue2Changed: requestPaint()
	onAttrMaxValue1Changed: requestPaint()
	onAttrMaxValue2Changed: requestPaint()

	Image {
		id: icon
		visible: false
		source: picUrl
	}

	onPaint: {
		var ctx = attribute2.getContext("2d");

		Draw.enter(ctx);

		Draw.bevelRect(ctx, 0, 0, width, height, 1.5, 6, "white", "#333333");

		Draw.textBox(ctx, attrValue1, 0, 0, (width-picSize)/2, height, "bold "+valueFontSize+"px Verdana", attrValue1 > attrMaxValue1 ? "red" : "white", 1);
		Draw.textBox(ctx, attrValue2, (width+picSize)/2, 0, (width-picSize)/2, height, "bold "+valueFontSize+"px Verdana", attrValue2 > attrMaxValue2 ? "red" : "white", 1);
		ctx.drawImage(icon, (width - picSize)/2, (height - picSize)/2, picSize, picSize, icon);

		Draw.exit(ctx);
	}
}
