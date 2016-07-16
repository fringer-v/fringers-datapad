import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Canvas {
	id: characteristic
	width: charWidth
	height: 142 * charWidth / 148
	property string charName: ""
	property string charShortName: ""
	property real charWidth: 148
	property int charValue: 6
	property real charNameFontSize: 18
	property real charNameMargin: 2
	property real charValueFontSize: 40
	property string charLineColor: "black"

	signal clicked();

	onCharValueChanged: requestPaint()

	onPaint: {
		var ctx = characteristic.getContext("2d");

		Draw.enter(ctx, 148, 142);

		// Now assume fixed width: 142 and fixed height: 148
		Draw.bevelRect(ctx, 0, 142-63, 148, 63, 1.5, 11, charLineColor, Constant.HEADER_COLOR);
		Draw.bevelRect(ctx, 6, 111, 136, 27, 1.5, 6, charLineColor, "white");
		Draw.drawCircle(ctx, 20, 0, 108, 6, charLineColor, "white");
		Draw.drawCircle(ctx, 29, 9, 90, 1.7, charLineColor);
		Draw.drawCircle(ctx, 33.5, 13.5, 81, 1, charLineColor);

		Draw.textBox(ctx, charName, 6, 111, 136, 27, "bold "+charNameFontSize+"px Arial", charLineColor, charNameMargin);
		Draw.textBox(ctx, charValue === 0 ? "-" : charValue.toString(), 20, 0, 108, 108, "bold "+charValueFontSize+"px Verdana", charLineColor, 3);

		Draw.exit(ctx);
	}

	MouseArea {
		id: mousy
		anchors.fill: parent
		onClicked: characteristic.clicked();
	}

}
