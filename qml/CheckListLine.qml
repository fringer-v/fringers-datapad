import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.3
import "../js/drawing.js" as Draw

Rectangle {
	id: checklistline
	height: {
		var desc_heigth = descText.height;
		if (ref == 0)
			desc_heigth = add_button_rect.height;
		return (desc_heigth + 5) < minimumHeight ? minimumHeight : desc_heigth + 5;
	}
	width: parent.width

	property int minimumHeight: 25
	property int lastLineHeigth: 40
	property int checkWidth: minimumHeight - 5 + 10
	property int dicePoolWidth: 132
	property int spacer: 10
	property int fontSize: 14
	property bool boxChecked: checked
	property string checkDicePool: dice
	property string checkDescription: description

	Column {
		Row {
			visible: ref != 0
			Rectangle {
				width: checkWidth
				height: checklistline.height - 1

				Rectangle {
					width: minimumHeight - 5
					height: minimumHeight - 5
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
				height: minimumHeight - 1
				center: false
				anchors.verticalCenter: parent.verticalCenter
				pool: checkDicePool
				color: "white"
			}

			FlowText {
				id: descText
				width: checklistline.width - checkWidth - dicePoolWidth
				pixelSize: fontSize
				font: "Comic Sans MS"
				anchors.verticalCenter: parent.verticalCenter
				text: checkDescription
			}
		}

		Row {
			visible: ref != 0
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

		Row {
			visible: ref == 0
			Rectangle {
				id: add_button_rect
				width: checklistline.width
				height: add_button.height + 10

				Button {
					id: add_button
					y: 10
					width: add_text.width + 10
					anchors.verticalCenter: parent.verticalCenter

					Text {
						id: add_text
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
						font.pixelSize: 16
						font.family: "Comic Sans MS"
						text: checkDescription
					}
				}
			}
		}
	}

	MouseArea {
		width: checkWidth + dicePoolWidth;
		height: checklistline.height
		anchors.top: parent.top
		anchors.left: parent.left
		onClicked: {
			if (ref == 0) {
				checklist.checkItemRef = ref;
				checkListItemDialogDicePool.pool = "";
				checkListItemDialogText.boxText = "";
				checkListItemDialog.visible = true;
			}
			else
				characterData.checkItem(ref);
		}
	}

	MouseArea {
		width: checklistline.width - (checkWidth + dicePoolWidth)
		height: checklistline.height
		anchors.top: parent.top
		anchors.right: parent.right
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
