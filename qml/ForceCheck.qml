import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util

Rectangle {
	id: forceCheck
	width: checklist.width - 20
	height: infoLine.height * 4 + 8 + 24 + (characterData.checkAttachDesc !== "" ? attachmentText.height : 0)

	property int leftWidth: 220
	property int roghtWidth: 300
	property int spaceBetween: 10

	Canvas {
		id: forceCheckCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = forceCheckCanvas.getContext("2d");
			var lineSize = 1.5;

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.cornerRect(ctx, 0, 0, 60, 70, 7, Constant.DARK_GRAY, "top-left, top-right");
			Draw.cornerRect(ctx, 0, forceCheck.height-(70), 60, 70, 7, Constant.LIGHT_GRAY, "bottom-left, bottom-right");
			Draw.bevelRect(ctx, 10, 4, forceCheck.width-10, forceCheck.height-8, lineSize, 19, "black", "white");
			Draw.exit(ctx);
		}
	}

	Column {
		x: 24
		y: 8

		InfoLine {
			id: infoLine
			width: forceCheck.width - 24 - 6
			infoTitle: "POWER:"
			infoText: characterData.talentName
			infoType: "bold,check-item,clear2"
		}

		Row {
			InfoLine {
				width: leftWidth + spaceBetween + (forceCheck.width - 24 - 6)  - 2*spaceBetween - leftWidth - roghtWidth
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
				width: leftWidth
				infoTitle: "SKILL:"
				infoText: characterData.skillName
				infoType: "check-item,clear2"
			}

			Hspacer { size: spaceBetween }

			InfoLine {
				width: (forceCheck.width - 24 - 6)  - 2*spaceBetween - leftWidth - roghtWidth
				infoTitle: "STRENGTH:"
				infoText: characterData.checkPowerStr
				infoType: "check-item,right,clear,flow-text"
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

		Row {
			InfoLine {
				width: leftWidth
				infoTitle: "RANGE:"
				infoText: characterData.checkRange
				infoType: "check-item,clear2"
			}

			Hspacer { size: spaceBetween }

			InfoLine {
				width: (forceCheck.width - 24 - 6)  - 2*spaceBetween - leftWidth - roghtWidth
				infoTitle: "DURATION:"
				infoText: characterData.checkDuration
				infoType: "check-item,right,clear,flow-text"
			}

			Hspacer { size: spaceBetween }

			InfoLine {
				width: roghtWidth
				infoTitle: "MAGNITUDE:"
				infoText: "  "+characterData.checkQualMag
				infoType: "check-item,right,clear"
			}
		}

		Vspacer {
			size: 10
			visible: characterData.checkAttachDesc !== ""
		}

		FlowText {
			id: attachmentText
			width: forceCheck.width - 35
			text: characterData.checkAttachDesc
			clearColor: "white"
			visible: characterData.checkAttachDesc !== ""
		}
	}
}
