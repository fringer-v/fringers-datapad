import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util

Rectangle {
	id: talentCheck
	width: checklist.width - 20
	height: infoLine.height * 3 + 8 + 24 + (characterData.checkAttachDesc !== "" ? attachmentText.height : 0)

	property int leftWidth: 220
	property int roghtWidth: 300
	property int spaceBetween: 10

	Canvas {
		id: talentCheckCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = talentCheckCanvas.getContext("2d");
			var lineSize = 1.5;

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.cornerRect(ctx, 0, 0, 60, 70, 7, Constant.DARK_GRAY, "top-left, top-right");
			Draw.cornerRect(ctx, 0, talentCheck.height-(70), 60, 70, 7, Constant.LIGHT_GRAY, "bottom-left, bottom-right");
			Draw.bevelRect(ctx, 10, 4, talentCheck.width-10, talentCheck.height-8, lineSize, 19, "black", "white");
			Draw.exit(ctx);
		}
	}

	Column {
		x: 24
		y: 8

		InfoLine {
			id: infoLine
			width: talentCheck.width - 24 - 6
			infoTitle: "TALENT:"
			infoText: characterData.talentName
			infoType: "bold,check-item,clear2"
		}

		Row {
			InfoLine {
				width: leftWidth + spaceBetween + (talentCheck.width - 24 - 6)  - 2*spaceBetween - leftWidth - roghtWidth
				infoTitle: "DICE POOL:"
				infoText: characterData.dicePool
				infoType: "check-item,right,dice,clear"
			}

			Hspacer { size: spaceBetween }

			InfoLine {
				width: roghtWidth
				infoTitle: "MANEUVERS:"
				infoText: "  "+Util.left(characterData.checkManeuvers, "|")
				infoComment: Util.right(characterData.checkManeuvers, "|")+"  "
				infoType: "check-item,right,clear"
			}
		}

		Row {
			InfoLine {
				width: leftWidth + spaceBetween + (talentCheck.width - 24 - 6)  - 2*spaceBetween - leftWidth - roghtWidth
				infoTitle: "SKILL:"
				infoText: characterData.skillName
				infoType: "check-item,clear2"
			}

			Hspacer { size: spaceBetween }

			InfoLine {
				width: roghtWidth
				infoTitle: "STRAIN:"
				infoText: "  "+Util.left(characterData.checkStrain, "|")
				infoComment: Util.right(characterData.checkStrain, "|")+"  "
				infoType: "check-item,right,clear"
			}
		}

		Vspacer {
			size: 10
			visible: characterData.checkAttachDesc !== ""
		}

		FlowText {
			id: attachmentText
			width: talentCheck.width - 35
			text: characterData.checkAttachDesc
			clearColor: "white"
			visible: characterData.checkAttachDesc !== ""
		}
	}
}
