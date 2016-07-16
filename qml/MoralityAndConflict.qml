import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: moraconf
	width: 320
	height: 92

	property int mainBoxWidth: moraconf.width-7
	property int mainBoxHeight: moraconf.height - 9 - 3
	property int textBoxWidth: 108

	Canvas {
		id: moraconfCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = moraconfCanvas.getContext("2d");

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.cornerRect(ctx, 0, 6, 26, 38, 4, Constant.LIGHT_GRAY, "top-left");
			Draw.cornerRect(ctx, 0, moraconf.height-38, 26, 38, 4, Constant.LIGHT_GRAY, "bottom-left");
			Draw.bevelRect(ctx, 7, 9, mainBoxWidth, mainBoxHeight, 1.5, 9, "black", "white");
			Draw.bevelRect(ctx, 7 + (mainBoxWidth - textBoxWidth) / 2 - textBoxWidth / 2 - 15, 0, textBoxWidth, 19, 1.5, 4, Constant.LIGHT_GRAY, Constant.LIGHT_GRAY);
			Draw.textBox(ctx, "MORALITY", 7 + (mainBoxWidth - textBoxWidth) / 2 - textBoxWidth / 2 - 15, 0, textBoxWidth, 19, "bold 13px Arial", "black", 1, "center", "middle");
			Draw.bevelRect(ctx, 7 + (mainBoxWidth - textBoxWidth) / 2 + textBoxWidth / 2 + 15, 0, textBoxWidth, 19, 1.5, 4, Constant.LIGHT_GRAY, Constant.LIGHT_GRAY);
			Draw.textBox(ctx, "CONFLICT", 7 + (mainBoxWidth - textBoxWidth) / 2 + textBoxWidth / 2 + 15, 0, textBoxWidth, 19, "bold 13px Arial", "black", 1, "center", "middle");
			Draw.vLine(ctx, 7, 9, mainBoxWidth, mainBoxHeight, 1.5, "black");
			Draw.exit(ctx);
		}
	}

	Text {
		x: 7
		y: 9
		width: mainBoxWidth / 2
		height: mainBoxHeight
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		font.bold: true
		font.pixelSize: 24
		font.family: "Verdana"
		textFormat: Text.RichText
		color: characterData.force > 0 ? "black" : Constant.DARK_GRAY
		clip: true
		text: characterData.force > 0 ? characterData.morality + "%" : "n/a"
	}

	MouseArea {
		x: 7
		y: 9
		width: mainBoxWidth / 2
		height: mainBoxHeight
		onClicked: {
			experienceDialog.experienceDialogRef = 0;
			experienceDialog.experienceDialogType = "MORALITY";
			experienceDialog.experienceDialogKey = "";
			experienceDialog.experienceDialogName = "";
			experienceDialog.experienceDialogDesc = "";
			experienceDialog.experienceDialogBase = characterData.morality;
			experienceDialog.experienceDialogValue = 0;
			experienceDialog.experienceDialogMayDelete = false;
			experienceDialog.visible = true;
		}
	}

	Text {
		x: 7 + mainBoxWidth / 2
		y: 9
		width: mainBoxWidth / 2
		height: mainBoxHeight
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		font.pixelSize: 24
		font.family: "Comic Sans MS"
		font.bold: true
		text: conflictValue()
	}

	MouseArea {
		x: 7 + mainBoxWidth / 2
		y: 9
		width: mainBoxWidth / 2
		height: mainBoxHeight
		onClicked: {
			if (characterData.force > 0)
				setNumberPadSubject("conflict");
		}
	}

	/*
	Dialog {
		id: conflictDialog
		width: 400
		title: "Adjust Conflict"
		standardButtons: StandardButton.Ok | StandardButton.Cancel

		Column {
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 16
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text:
					"<b><font size=\"+1\">Adjust Conflict</font></b><br><br>"+
					"Enter Conflict adjustment: e.g. 1 or +1 to<br>"+
					"add 1 Conflict, -1 to remove 1 Conflict.";
			}

			Vspacer { size: 10 }

			TextField {
				id: conflictAdjustment
				width: 120
				height: 32
				anchors.horizontalCenter: parent.horizontalCenter
				validator: IntValidator { }
				inputMethodHints: Qt.ImhFormattedNumbersOnly
				placeholderText: "+/- Conflct"

				Keys.onReturnPressed: {
					conflictDialog.click(StandardButton.Ok);
				}
			}

			Vspacer { size: 10 }
		}

		onAccepted: {
			var val = conflictAdjustment.text.trim();
			characterData.adjustConflict(parseInt(val));
			Qt.inputMethod.hide();
		}

		onRejected: {
			Qt.inputMethod.hide();
		}
	}
	*/
}
