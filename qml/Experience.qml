import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: experience
	width: parent.width
	height: 199

	property int xpBoxWidth: 137
	property int xpBoxFontSize: 26
	property int acquiredXPHeight: 0

	Column {
		Row {
			id: xpBox

			Attribute {
				attrText: "TOTAL XP"
				attrWidth: xpBoxWidth
				attrValue: characterData.locked === 0 ? characterData.totalXP : ""
				attrDelta: characterData.locked === 0 ? characterData.newXP : 0
				attrValueFontSize: xpBoxFontSize
				attrDeltaFontSize: xpBoxFontSize * 3 / 4

				Image {
					width: 24
					height: 24
					y: 32
					anchors.horizontalCenter: parent.horizontalCenter
					source: "qrc:/images/lock.png"
					visible: characterData.locked !== 0
				}

				MouseArea {
					anchors.fill: parent
					onClicked: {
						experienceDialog.experienceDialogRef = 0;
						experienceDialog.experienceDialogType = "XP";
						experienceDialog.experienceDialogKey = "";
						experienceDialog.experienceDialogName = "";
						experienceDialog.experienceDialogDesc = "";
						experienceDialog.experienceDialogBase = characterData.totalXP;
						experienceDialog.experienceDialogValue = 0;
						experienceDialog.experienceDialogMayDelete = false;
						experienceDialog.visible = true;
					}
				}
			}

			Hspacer { size: (experience.width - xpBoxWidth*3) / 2 }

			Attribute {
				attrText: "UNUSED XP"
				attrWidth: xpBoxWidth
				attrValue: characterData.locked === 0 ? (characterData.totalXP + characterData.newXP - characterData.usedXP) : ""
				attrValueFontSize: xpBoxFontSize

				Image {
					width: 24
					height: 24
					y: 32
					anchors.horizontalCenter: parent.horizontalCenter
					//anchors.verticalCenter: parent.verticalCenter
					source: "qrc:/images/lock.png"
					visible: characterData.locked !== 0
				}

				MouseArea {
					anchors.fill: parent
					onClicked: {
						experienceDialog.experienceDialogRef = 0;
						experienceDialog.experienceDialogType = "XP";
						experienceDialog.experienceDialogKey = "";
						experienceDialog.experienceDialogName = "";
						experienceDialog.experienceDialogDesc = "";
						experienceDialog.experienceDialogBase = characterData.totalXP;
						experienceDialog.experienceDialogValue = 0;
						experienceDialog.experienceDialogMayDelete = false;
						experienceDialog.visible = true;
					}
				}
			}

			Hspacer { size: (experience.width - xpBoxWidth*3) / 2 }

			Attribute {
				attrText: "USED XP"
				attrWidth: xpBoxWidth
				attrValue: characterData.locked === 0 ? characterData.usedXP : ""
				attrValueFontSize: xpBoxFontSize

				Image {
					width: 24
					height: 24
					y: 32
					anchors.horizontalCenter: parent.horizontalCenter
					//anchors.verticalCenter: parent.verticalCenter
					source: "qrc:/images/lock.png"
					visible: characterData.locked !== 0
				}

				MouseArea {
					anchors.fill: parent
					onClicked: {
						experienceDialog.experienceDialogRef = 0;
						experienceDialog.experienceDialogType = "XP";
						experienceDialog.experienceDialogKey = "";
						experienceDialog.experienceDialogName = "";
						experienceDialog.experienceDialogDesc = "";
						experienceDialog.experienceDialogBase = characterData.totalXP;
						experienceDialog.experienceDialogValue = 0;
						experienceDialog.experienceDialogMayDelete = false;
						experienceDialog.visible = true;
					}
				}
			}
		}

		Vspacer { size: 10 }

		Row {
			Hspacer { size: (experience.width - acquiredXP.width) / 2 }

			AcquiredXP {
				id: acquiredXP
				width: experience.width
				height: acquiredXPHeight === 0 ? experience.height - xpBox.height - 10 : acquiredXPHeight
			}
		}
	}

}
