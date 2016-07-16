import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw

Rectangle {
	id: checklistline
	height: 25
	width: parent.width
	property int checkWidth: checklistline.height - 5 + 10
	property int dicePoolWidth: 120
	property int spacer: 10
	property int fontSize: 14
	property bool boxChecked: checked
	property string checkDicePool: dice
	property string checkDescription: description

	Column {
		Row {
			Rectangle {
				width: checkWidth
				height: checklistline.height - 1

				Rectangle {
					width: checklistline.height - 5
					height: checklistline.height - 5
					anchors.left: parent.left
					anchors.verticalCenter: parent.verticalCenter
					border.width: 1
					border.color: "darkgray"

					Image {
						visible: boxChecked
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
						source: "qrc:/images/tick.png"
					}
				}
			}

			DicePool {
				width: dicePoolWidth
				height: checklistline.height - 1
				center: false
				pool: checkDicePool
				color: "white"
			}

			FlowText {
				id: descText
				width: checklistline.width - checkWidth - dicePoolWidth
				height: checklistline.height - 1
				pixelSize: fontSize
				font: "Comic Sans MS"
				text: checkDescription
			}

		}

		Row {
			Rectangle {
				height: 1
				width: checkWidth
				color: "white"
			}

			Rectangle {
				height: 1
				width: checklistline.width - checkWidth
				color: "darkgray"
			}
		}
	}

	MouseArea {
		width: checkWidth + dicePoolWidth
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.bottom: parent.bottom
		onClicked: {
			characterData.checkItem(ref);
		}
	}

	MouseArea {
		width: checklistline.width - (checkWidth + dicePoolWidth)
		anchors.top: parent.top
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		onClicked: {
			checklist.checkItemRef = ref;
			if (ref == 0) {
				checkListItemDialogDicePool.pool = "";
				checkListItemDialogText.boxText = "";
			}
			else {
				checkListItemDialogDicePool.pool = checkDicePool;
				checkListItemDialogText.boxText = checkDescription;
			}
			if (ref >= 0)
				checkListItemDialog.visible = true;
		}
	}
}
