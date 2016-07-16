import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: skillLine
	width: parent.width
	height: (index % 2) == 1 ? 25 : 24

	property var colWidths: adjustedlColWidths
	property int skillFontSize: 16
	property int descFontSize: 13
	property real dicePoolMargin: 2.34
	property real dicePoolSpacing: 3.24
	property string lineColor: (index % 2) == 1 ? Constant.LIST_LINE_GRAY : "white"

	MouseArea {
		anchors.fill: parent
		onClicked: {
			if (customSkills) {
				if (key.length === 0)
					addCustomSkillDialog.open();
				else
					removeCustomSkillDialog.open();
			}
		}
	}

	Row {
		Hspacer { size: listIndent; color: lineColor }

		Rectangle {
			width: colWidths[0]
			height: skillLine.height
			color: lineColor

			Text {
				anchors.fill: parent
				font.family: "Arial"
				font.pointSize: skillFontSize
				verticalAlignment: Text.AlignVCenter
				color: "black"
				text: skill
			}
		}

		// Career
		Rectangle {
			width: colWidths[1]
			height: skillLine.height
			color: lineColor

			Image {
				visible: career
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				source: "qrc:/images/tick.png"
			}
		}

		// Ranks
		Rectangle {
			width: colWidths[2]
			height: skillLine.height
			color: lineColor
			Text {
				anchors.fill: parent
				font.family: "Arial"
				font.pointSize: skillFontSize
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				color: "black"
				text: rank
			}
		}

		// Dice Pool
		DicePool {
			width: colWidths[3]
			height: skillLine.height
			color: lineColor
			backgroundColor: lineColor
			margin: dicePoolMargin
			spacing: dicePoolSpacing
			pool: dice

			MouseArea {
				anchors.fill: parent
				onClicked: {
					characterData.itemKey = "";
					characterData.itemName = "";
					characterData.activeSkill = skill;
					characterData.dicePool = dice;
					characterData.showCheckList();
					controller.state = "checkout";
				}
			}
		}

		Hspacer { size: listIndent; color: lineColor }
	}

	Dialog {
		id: removeCustomSkillDialog
		width: 400
		title: "Remove Custom Skill"
		standardButtons: StandardButton.Yes | StandardButton.Cancel

		Column {
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 16
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text:
					"<b><font size=\"+1\">Remove Custom Skill</font></b><br><br>"+
					"Do you wish to remove this Custom Skill?"
			}

			Vspacer { size: 10 }
		}

		onYes: {
			characterData.removeCustomSkill(key);
		}
	}

	Dialog {
		id: addCustomSkillDialog
		width: 400
		title: "Add Critical Injury"
		standardButtons: StandardButton.Ok | StandardButton.Cancel

		Column {
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 16
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text:
					"<b><font size=\"+1\">Add Custom Skill</font></b><br><br>"+
					"Enter the name, select the characteristic, and specify<br>"+
					"the rank of the new Custom Skill.";
			}

			Vspacer { size: 10 }

			Row {
				anchors.horizontalCenter: parent.horizontalCenter

				TextField {
					id: skillName
					width: 160
					height: 32
					placeholderText: "Name"

					Keys.onReturnPressed: {
						addCustomSkillDialog.click(StandardButton.Ok);
					}
				}

				Hspacer { size: 20 }

				ComboBox {
					id: skillChar
					width: 100
					height: 32
					currentIndex: 0
					model: ListModel {
						id: skillCharModel
						ListElement { text: ""; charac: "" }
						ListElement { text: "Brawn"; charac: "BR" }
						ListElement { text: "Agility"; charac: "AG" }
						ListElement { text: "Intelect"; charac: "INT" }
						ListElement { text: "Cunning"; charac: "CUN" }
						ListElement { text: "Willpower"; charac: "WIL" }
						ListElement { text: "Presence"; charac: "PR" }
					}
				}

				Hspacer { size: 20 }

				TextField {
					id: skillRank
					width: 60
					height: 32
					validator: IntValidator { bottom: 0; top: 6; }
					inputMethodHints: Qt.ImhFormattedNumbersOnly
					placeholderText: "Rank"

					Keys.onReturnPressed: {
						addCustomSkillDialog.click(StandardButton.Ok);
					}
				}
			}

			Vspacer { size: 10 }
		}

		onAccepted: {
			var charac = skillCharModel.get(skillChar.currentIndex).charac;
			characterData.addCustomSkill(skillName.text, charac, parseInt(skillRank.text));
			Qt.inputMethod.hide();
		}

		onRejected: {
			Qt.inputMethod.hide();
		}
	}
}
