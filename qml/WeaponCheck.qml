import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: poolItem
	width: checklist.width - 20
	height: infoLine.height * 4 + 8 + 24 + (characterData.itemAttachments !== "" ? attachmentText.height : 0)

	property int leftWidth: 200
	property int roghtWidth: 300
	property int spaceBetween: 10

	Canvas {
		id: poolItemCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = poolItemCanvas.getContext("2d");
			var lineSize = 1.5;

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.cornerRect(ctx, 0, 0, 60, 70, 7, Constant.DARK_GRAY, "top-left, top-right");
			Draw.cornerRect(ctx, 0, poolItem.height-(70), 60, 70, 7, Constant.LIGHT_GRAY, "bottom-left, bottom-right");
			Draw.bevelRect(ctx, 10, 4, poolItem.width-10, poolItem.height-8, lineSize, 19, "black", "white");
			Draw.exit(ctx);
		}
	}

	Column {
		x: 24
		y: 8

		InfoLine {
			id: infoLine
			width: poolItem.width - 24 - 6
			infoTitle: "WEAPON:"
			infoText: characterData.itemName
			infoType: "bold,check-item,clear2"
		}

		Row {
			InfoLine {
				width: leftWidth
				infoTitle: "SKILL:"
				infoText: characterData.itemSkill
				infoType: "check-item,clear2"
			}

			Hspacer { size: spaceBetween }

			InfoLine {
				width: (poolItem.width - 24 - 6)  - 2*spaceBetween - leftWidth - roghtWidth
				infoTitle: "DICE POOL:"
				infoText: characterData.dicePool
				infoType: "check-item,right,dice,clear"
			}

			Hspacer { size: spaceBetween }

			InfoLine {
				width: roghtWidth
				infoTitle: "MANEUVERS:"
				infoText: characterData.itemManeuvers
				infoType: "check-item,right,clear"
			}
		}

		Row {
			InfoLine {
				width: leftWidth
				infoTitle: "RANGE:"
				infoText: characterData.itemRange
				infoType: "check-item,clear2"
			}

			Hspacer { size: spaceBetween }

			InfoLine {
				width: (poolItem.width - 24 - 6)  - 2*spaceBetween - leftWidth - roghtWidth
				infoTitle: "CRITICAL:"
				infoText: characterData.itemCritical
				infoType: "check-item,right,dice,clear"
			}

			Hspacer { size: spaceBetween }

			InfoLine {
				width: roghtWidth
				infoTitle: "STRAIN:"
				infoText: characterData.itemStrain
				infoType: "check-item,right,clear"
			}
		}

		Row {
			InfoLine {
				width: leftWidth
				infoTitle: "DAMAGE:"
				infoText: characterData.itemDamage
				infoType: "check-item,clear2"
			}

			Hspacer { size: 10 }

			InfoLine {
				width: (poolItem.width - 24 - 6)  - 10 - leftWidth
				infoTitle: "QUALITIES:"
				infoText: characterData.itemQualities
				infoType: "check-item,right,clear2"
			}
		}

		Vspacer {
			size: 10
			visible: characterData.itemAttachments !== ""
		}

		FlowText {
			id: attachmentText
			width: poolItem.width - 35
			text: characterData.itemAttachments
			clearColor: "white"
			visible: characterData.itemAttachments !== ""
		}
	}
}
