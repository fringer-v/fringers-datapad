import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Canvas {
	id: attribute
	height: 92 * attrWidth / 167
	width: attrWidth
	property string attrText: ""
	property int attrWidth: 167
	property string attrValue: "0"
	property int attrDelta: 0
	property int attrValueFontSize: scaleHeight(30)
	property int attrDeltaFontSize: attrValueFontSize
	property real attrLineSize: scaleWidth(1.5) > 1 ? scaleWidth(1.5) : 1

	onAttrValueChanged: requestPaint()

	onPaint: {
		var ctx = attribute.getContext("2d");

		Draw.enter(ctx, 167, 92);

		Draw.bevelRect(ctx, 0, 31, 167, 61, 1, 11, undefined, Constant.HEADER_COLOR);
		Draw.bevelRect(ctx, 7, 0, 153, 75, attrLineSize, 20, "black", "white");
		Draw.pointyRect(ctx, 17, 6, 133, 24, attrLineSize, 12, "black");

		Draw.textBox(ctx, attrText, 17, 6, 133, 24, "bold 14px Arial", "black", 1);

		Draw.exit(ctx);
	}

	Text {
		x: scaleWidth(7)
		y: scaleHeight(28)
		width: scaleWidth(153)
		height: scaleHeight(75-28)
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		font.bold: true
		font.pixelSize: attrDelta === 0 ? attrValueFontSize : attrDeltaFontSize
		font.family: "Verdana"
		textFormat: Text.RichText
		clip: true
		text:  {
			if (attrDelta === 0)
				return attrValue;
			return "<font color='#444444'><s>"+attrValue+"</s></font> "+(parseInt(attrValue)+attrDelta);
		}
	}

	function scaleWidth(h)
	{
		return h * width / 167
	}

	function scaleHeight(h)
	{
		return h * height / 92
	}
}
