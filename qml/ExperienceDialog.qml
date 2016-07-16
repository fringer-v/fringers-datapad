import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util

Rectangle {
	id: experienceDialog
	anchors.fill: parent
	color: "#00000000"
	visible: false

	property int margin: 10
	property real button_height: 40
	property real text_height: 40
	property real button_width: 80
	property real cancel_button_width: 120

	property int experienceDialogRef: 0
	property string experienceDialogType: ""
	property string experienceDialogKey: ""
	property string experienceDialogName: ""
	property string experienceDialogDesc: ""
	property int experienceDialogBase: 0
	property int experienceDialogValue: 0
	property bool experienceDialogMayDelete: false

	onExperienceDialogDescChanged: descDisplay.boxText = experienceDialogDesc

	MouseArea {
		anchors.fill: parent

		onClicked: {
			experienceDialog.visible = false;
			Qt.inputMethod.hide();
		}
	}

	Rectangle {
		id: experienceDialogBox
		width: button_width*6+margin*7
		height: margin + button_height*4 + text_height + margin + margin*3  + margin
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		color: "#7F000000"
		radius: 10

		Column {
			Vspacer { size: margin }

			Row {
				Hspacer { size: margin }

				Rectangle {
					width: experienceDialogBox.width - (margin*3 + numberDisplay.width)
					height: button_height
					border.width: 1
					border.color: "black"
					color: "lightgrey"

					Text {
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
						font.pixelSize: 20
						font.family: "Comic Sans MS"
						horizontalAlignment: Text.AlignHCenter
						text: experienceTitle(experienceDialogType, experienceDialogName);
					}
				}

				Hspacer { size: margin }

				Rectangle {
					id: numberDisplay
					width: 200
					height: button_height
					border.width: 1
					border.color: "black"
					color: "lightgrey"

					Text {
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
						font.pixelSize: 20
						font.family: "Comic Sans MS"
						horizontalAlignment: Text.AlignHCenter
						text: {
							var result = experienceDialogBase + experienceDialogValue;
							return "" + experienceDialogBase + (experienceDialogValue >= 0 ? "  +" : "  ") + experienceDialogValue + "  =" + result;
						}
					}
				}

				Hspacer { size: margin }
			}

			Vspacer { size: margin }

			Row {
				id: button_row

				Hspacer { size: margin }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "+1"
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "+2"
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: button_height
					buttonBorder: 0
					onPressed: experienceDialogClicked(buttonText)
					buttonText: "+5"
				}

				Hspacer { size: margin }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "+10"
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "+20"
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "+50"
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin }
			}

			Vspacer { size: margin }

			Row {
				Hspacer { size: margin }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: button_height
					buttonBorder: 0
					onPressed: experienceDialogClicked(buttonText)
					buttonText: "-1"
				}

				Hspacer { size: margin }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "-2"
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "-5"
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "-10"
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "-20"
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "-50"
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin }
			}

			Vspacer { size: margin }

			Row {
				Hspacer { size: margin }

				BoxEditLine {
					id: descDisplay
					width: experienceDialogBox.width - margin*2
					height: text_height
					enabled: true
					placeHolderText: "Description"
					name: "desc"

					onDonePressed: experienceDialogClicked("OK")
				}

				Hspacer { size: margin }
			}

			Vspacer { size: margin }

			Row {
				Hspacer { size: margin }

				Hspacer { size: experienceDialogBox.width - margin*3 - cancel_button_width*2 - (experienceDialogMayDelete ? (margin+cancel_button_width) : 0) }

				PadButton {
					buttonWidth: cancel_button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "Cancel"
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin; visible: experienceDialogMayDelete }

				PadButton {
					buttonWidth: cancel_button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "Delete"
					visible: experienceDialogMayDelete
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin }

				PadButton {
					buttonWidth: cancel_button_width
					buttonHeigth: button_height
					buttonBorder: 0
					buttonText: "OK"
					onPressed: experienceDialogClicked(buttonText)
				}

				Hspacer { size: margin }
			}
		}
	}

	function experienceDialogClicked(button)
	{
		if (button === "C")
			experienceDialogValue = 0;
		else if (button === "Cancel") {
			experienceDialogType = "";
			experienceDialog.visible = false;
		}
		else if (button === "Delete") {
			if (experienceDialogRef)
				removeExperienceDialog.open();
		}
		else if (button === "OK") {
			if (experienceDialogType !== "" && experienceDialogValue !== 0) {
				if (experienceDialogRef)
					characterData.changeExperience(experienceDialogRef, descDisplay.boxText, experienceDialogValue);
				else
					characterData.addExperience(experienceDialogType, experienceDialogKey, experienceDialogName, descDisplay.boxText, experienceDialogValue);
				experienceDialogType = "";
				experienceDialog.visible = false;
			}
		}
		else
			experienceDialogValue += parseInt(button);
	}

	Dialog {
		id: removeExperienceDialog
		width: 400
		title: "Remove Experience"
		standardButtons: StandardButton.Yes | StandardButton.Cancel

		Column {
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 16
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text:
					"<b><font size=\"+1\">Remove Experience</font></b><br><br>"+
					"Do you wish to remove this experience item?"
			}

			Vspacer { size: 10 }
		}

		onYes: {
			characterData.removeExperience(experienceDialogRef);
			experienceDialog.visible = false;
		}
	}

	function openRemoveExperienceDialog(ref)
	{
		experienceDialogRef = ref;
		removeExperienceDialog.open();
	}

	function experienceTitle(type, name)
	{
		var val;

		if (type == "DUTYRANK")
			val = "Duty Rank";
		else if (type == "DUTY")
			val = "Duty: "+name;
		else if (type == "XP")
			val = "XP";
		else if (type == "OBLIGATION")
			val = "Obligation: "+name;
		else if (type == "MORALITY")
			val = "Morality";
		else
			return "";
		return val;
	}

	Dialog {
		id: addXPDialog
		width: 400
		title: "Add XP"
		standardButtons: StandardButton.Ok | StandardButton.Cancel

		Column {
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 16
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text:
					"<b><font size=\"+1\">Add Experience</font></b><br><br>"+
					"Enter a description and the amount of XP awarded.";
			}

			Vspacer { size: 10 }

			Row {
				anchors.horizontalCenter: parent.horizontalCenter

				TextField {
					id: descField
					width: 220
					height: 32
					placeholderText: "Description"

					Keys.onReturnPressed: {
						addXPDialog.click(StandardButton.Ok);
					}
				}

				Hspacer { size: 20 }

				TextField {
					id: amountField
					width: 120
					height: 32
					validator: IntValidator { }
					inputMethodHints: Qt.ImhFormattedNumbersOnly
					placeholderText: "XP Amount"

					Keys.onReturnPressed: {
						addXPDialog.click(StandardButton.Ok);
					}
				}
			}

			Vspacer { size: 10 }
		}

		onAccepted: {
			Qt.inputMethod.hide();
		}

		onRejected: {
			Qt.inputMethod.hide();
		}
	}

}
