import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: encumberance
	width: encumValues.width + spaceBetween + cumbValues.width + spaceBetween + encumberanceCanvas.width
	height: 93

	property int spaceBetween: 16

	Row {
		Attribute2 {
			id: encumValues
			y: encumberance.height - encumValues.height

			attrText: "ENCUMBERANCE"
			textFontSize: 12
			attrSubText1: "VALUE"
			attrSubText2: "THRESHOLD"
			subTextFontSize: 10
			subTextOffset: 3
			attrValue1: characterData.encValue
			attrValue2: characterData.encThreshold
			valueFontSize: 24
		}

		Hspacer { size: spaceBetween }

		Attribute2 {
			id: cumbValues
			height: 92
			width: 143
			y: encumberance.height - cumbValues.height

			attrText: "CUMBERSOME"
			textFontSize: 12
			attrSubText1: "VALUE"
			attrSubText2: "BRAWN"
			subTextFontSize: 10
			subTextOffset: 3
			attrValue1: characterData.cumbValue
			attrValue2: characterData.cumbThreshold
			valueFontSize: 24
		}

		Hspacer { size: spaceBetween }

		Canvas {
			id: encumberanceCanvas
			width: 280
			height: encumberance.height

			onPaint: {
				var ctx = encumberanceCanvas.getContext("2d");

				Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
				Draw.cornerRect(ctx, 0, 0, 38, 34, 6, Constant.DARK_GRAY, "top-left,top-right");
				Draw.cornerRect(ctx, 0, ctx.canvas.height-34, 38, 34, 6, Constant.LIGHT_GRAY, "bottom-left,bottom-right");
				Draw.bevelRect(ctx, 9, 2, ctx.canvas.width-9, ctx.canvas.height-4, 1.5, 11, "black", "white");
				Draw.exit(ctx);
			}
		}
	}

	NotesArea {
		width: encumberanceCanvas.width-30
		height: encumberanceCanvas.height-16
		x: encumberanceCanvas.x + 20
		y: encumberanceCanvas.y + 8
		text: characterData.encText
	}
}
