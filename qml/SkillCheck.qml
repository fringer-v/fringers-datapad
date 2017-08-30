import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util

Rectangle {
	id: skillCheck
	width: checklist.width - 20
	height: infoLine.height * 2 + 8 + 24

	property int leftWidth: 220
	property int rightWidth: 220
	property int spaceBetween: 10

	Canvas {
		id: skillCheckCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = skillCheckCanvas.getContext("2d");
			var lineSize = 1.5;

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.cornerRect(ctx, 0, 0, 60, 70, 7, Constant.DARK_GRAY, "top-left, top-right");
			Draw.cornerRect(ctx, 0, skillCheck.height-(70), 60, 70, 7, Constant.LIGHT_GRAY, "bottom-left, bottom-right");
			Draw.bevelRect(ctx, 10, 4, skillCheck.width-10, skillCheck.height-8, lineSize, 19, "black", "white");
			Draw.exit(ctx);
		}
	}

	Column {
		x: 24
		y: 8

		InfoLine {
			id: infoLine
			width: skillCheck.width - 24 - 6
			infoTitle: "SKILL:"
			infoText: characterData.skillName.toUpperCase()
			infoType: "bold,check-item,right,clear2"
		}

		Row {
			InfoLine {
				width: (skillCheck.width - 24 - 6)  - 2*spaceBetween - leftWidth - rightWidth
				infoTitle: "DICE POOL:"
				infoText: characterData.dicePool
				infoType: "check-item,right,dice,clear"
			}

			Hspacer { size: spaceBetween }

			InfoLine {
				width: leftWidth
				infoTitle: "MANEUVERS:"
				infoText: "  "+Util.left(characterData.itemManeuvers, "|")
				infoComment: Util.right(characterData.itemManeuvers, "|")+"  "
				infoType: "check-item,right,clear"
			}

			Hspacer { size: spaceBetween }

			InfoLine {
				width: rightWidth
				infoTitle: "STRAIN:"
				infoText: "  "+Util.left(characterData.itemStrain, "|")
				infoComment: Util.right(characterData.itemStrain, "|")+"  "
				infoType: "check-item,clear"
			}

		}
	}
}
