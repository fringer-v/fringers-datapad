import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: obligation
	width: 488
	height: 100

	property int obligRow: 0
	property string obligName: ""
	property int obligValue: 0
	property string obligText: ""
	property bool obligLeft: true

	Row {
		anchors.verticalCenter: parent.verticalCenter

		Row {
			id: leftSide
			visible: obligLeft

			Characteristic {
				charName: obligName
				charWidth: 100
				charValue: obligValue
				charNameFontSize: 13
				charNameMargin: 2
				charValueFontSize: 32
			}

			Hspacer { size: 10 }
		}

		Canvas {
			id: obligationCanvas
			width: obligation.width - (leftSide.visible ? leftSide.width : 0) - (rightSide.visible ? rightSide.width : 0)
			height: obligation.height

			onPaint: {
				var ctx = obligationCanvas.getContext("2d");
				var lineSize = 1.5;
				var cornerBoxH = 32;

				Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
				Draw.cornerRect(ctx, obligLeft ? 0 : obligationCanvas.width-32, 0, 32, cornerBoxH, 5, Constant.DARK_GRAY, obligLeft ? "top-left" : "top-right");
				Draw.cornerRect(ctx, obligLeft ? 0 : obligationCanvas.width-32, obligationCanvas.height-cornerBoxH, 32,
					cornerBoxH, 5, Constant.LIGHT_GRAY, obligLeft ? "bottom-left" : "bottom-right");
				Draw.bevelRect(ctx, obligLeft ? 6 : 0, 3, obligationCanvas.width-6, obligationCanvas.height-6, lineSize, 13, "black", "white");
				Draw.exit(ctx);
			}
		}

		Row {
			id: rightSide
			visible: !obligLeft

			Hspacer { size: 10 }

			Characteristic {
				charName: obligName
				charWidth: 100
				charValue: obligValue
				charNameFontSize: 13
				charNameMargin: 2
				charValueFontSize: 32
			}
		}
	}

}
