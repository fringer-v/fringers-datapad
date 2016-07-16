import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw

Rectangle {
	id: xp
	height: 22
	width: parent.width
	property int amountWidth: 60
	property int spacer: 10
	property int fontSize: 14

	Column {
		Row {
			Text {
				id: descText
				width: xp.width - amountWidth - spacer
				height: xp.height - 1
				font.pixelSize: fontSize
				font.family: "Comic Sans MS"
				font.bold: false
				horizontalAlignment: Text.AlignLeft
				verticalAlignment: Text.AlignVCenter
				text: {
					var val = experienceDialog.experienceTitle(type, name);
					if (val === "")
						return desc;
					if (desc !== "")
						val += " - "+ desc;
					return val;
				}
			}

			Hspacer { size: spacer }

			Text {
				id: amountText
				width: amountWidth
				height: xp.height-1
				font.pixelSize: fontSize
				font.family: "Comic Sans MS"
				font.bold: true
				horizontalAlignment: Text.AlignRight
				verticalAlignment: Text.AlignVCenter
				text: ref === 0 ? "" : amount
			}
		}

		Row {
			Rectangle {
				height: 1
				width: descText.width
				color: "darkgray"
			}

			Hspacer { size: spacer }

			Rectangle {
				height: 1
				width: amountText.width
				color: "darkgray"
			}
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			if (ref === 0) {
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
			else if (del) {
				if (ro) {
					experienceDialog.openRemoveExperienceDialog(ref)
				}
				else {
					experienceDialog.experienceDialogRef = ref;
					experienceDialog.experienceDialogType = type;
					experienceDialog.experienceDialogKey = key;
					experienceDialog.experienceDialogName = name;
					experienceDialog.experienceDialogDesc = desc;
					experienceDialog.experienceDialogBase = base - amount;
					experienceDialog.experienceDialogValue = amount;
					experienceDialog.experienceDialogMayDelete = true;
					experienceDialog.visible = true;
				}
			}
		}
	}

}
