import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw

Rectangle {
	id: topBox
	width: woundWidth
	height: 213 * woundWidth / 376

	property int attributeHeight: 0
	property int woundWidth: 376
	property int leftAndRightIndent: 7
	property int topIndent: 6
	property int bottomIndent: 8
	property real frameWidth: width - leftAndRightIndent*2
	property real frameHeight: height - topIndent - bottomIndent
	property real textBorder: 10

	property real textWidth: frameWidth/2 - textBorder*2
	property real textHeight: frameHeight - (attributeHeight-topIndent) - textBorder*2 - 18/2 // 18=text box

	Canvas {
		id: woundandstrain
		anchors.fill: parent

		onPaint: {
			var ctx = woundandstrain.getContext("2d");

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);

			Draw.bevelRect(ctx, leftAndRightIndent, topIndent, frameWidth, frameHeight, 1.5, 20, "black", "white");
			Draw.vLine(ctx, leftAndRightIndent, topIndent, frameWidth, frameHeight, 1.5, "black");
			Draw.pointyRect(ctx, 135, 213-18, 106, 18, 1.5, 9, "black", "white");
			Draw.textBox(ctx, "CURRENT", 135, 213-18, 106, 18, "bold 12px Arial", "black", 1);

			Draw.exit(ctx);
		}
	}

	Text {
		x: leftAndRightIndent + textBorder
		y: attributeHeight
		width: textWidth
		height: 20
		color: "darkgrey"
		horizontalAlignment: Text.Right
		verticalAlignment: Text.Top
		font.pixelSize: 16
		font.family: "Comic Sans MS"
		text: woundHistory()
	}

	Text {
		x: leftAndRightIndent + textBorder
		y: attributeHeight + textBorder
		width: textWidth
		height: textHeight
		color: currentWounds() > (characterData.wound+woundDelta()) ? "red" : "black"
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		font.pixelSize: 42
		font.family: "Comic Sans MS"
		font.bold: true
		text: currentWounds() === 0 ? "" : currentWounds()
	}

	MouseArea {
		x: 0
		y: 0
		width: topBox.width/2
		height: attributeHeight
		onClicked: {
			//woundDelta.text = characterData.woundDelta === 0 ? "" : characterData.woundDelta;
			//woundDeltaDialog.open();
			setNumberPadSubject("wound-threshold");
		}
	}

	MouseArea {
		x: 0
		y: attributeHeight
		width: topBox.width/2
		height: topBox.height-attributeHeight
		onClicked: {
			//woundAdjustment.text = "";
			//woundsChangeDialog.open();
			setNumberPadSubject("adjust-wounds");
		}
	}

	Text {
		x: leftAndRightIndent + textBorder + textWidth + textBorder*2
		y: attributeHeight
		width: textWidth
		height: 20
		color: "darkgrey"
		horizontalAlignment: Text.Right
		verticalAlignment: Text.Top
		font.pixelSize: 16
		font.family: "Comic Sans MS"
		text: strainHistory()
	}

	Text {
		x: leftAndRightIndent + textBorder + textWidth + textBorder*2
		y: attributeHeight + textBorder
		width: textWidth
		height: textHeight
		color: currentStrain() > (characterData.strain+strainDelta()) ? "red" : "black"
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		font.pixelSize: 42
		font.family: "Comic Sans MS"
		font.bold: true
		text: currentStrain() === 0 ? "" : currentStrain()
	}

	MouseArea {
		x: topBox.width/2
		y: 0
		width: topBox.width/2
		height: attributeHeight
		onClicked: {
			//strainDelta.text = characterData.strainDelta === 0 ? "" : characterData.strainDelta;
			//strainDeltaDialog.open();
			setNumberPadSubject("strain-threshold");
		}
	}

	MouseArea {
		x: topBox.width/2
		y: attributeHeight
		width: topBox.width/2
		height: topBox.height-attributeHeight
		onClicked: {
			//strainAdjustment.text = "";
			//strainChangeDialog.open();
			setNumberPadSubject("adjust-strain");
		}
	}

	/*
	Dialog {
		id: woundsChangeDialog
		width: 400
		title: "Adjust Wounds"
		standardButtons: StandardButton.Ok | StandardButton.Cancel

		Column {
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 16
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text:
					"<b><font size=\"+1\">Adjust Wounds</font></b><br><br>"+
					"Enter Wound adjustment: e.g. 5 or +5 to<br>"+
					"add 5 Wounds, -5 to remove 5 Wounds."
			}

			Vspacer { size: 10 }

			TextField {
				id: woundAdjustment
				width: 120
				height: 32
				anchors.horizontalCenter: parent.horizontalCenter
				validator: IntValidator { }
				inputMethodHints: Qt.ImhFormattedNumbersOnly
				placeholderText: "+/- Wounds"

				Keys.onReturnPressed: {
					woundsChangeDialog.click(StandardButton.Ok);
				}
			}

			Vspacer { size: 10 }
		}

		onAccepted: {
			var val = woundAdjustment.text.trim();
			characterData.adjustWounds(parseInt(val));
			Qt.inputMethod.hide();
		}

		onRejected: {
			Qt.inputMethod.hide();
		}
	}

	Dialog {
		id: woundDeltaDialog
		width: 400
		title: "Adjust Wound Threshold"
		standardButtons: StandardButton.Ok | StandardButton.Cancel

		Column {
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 16
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text:
					"<b><font size=\"+1\">Adjust Wound Threshold</font></b><br><br>"+
					"Enter a Wound Threshold adjustment: e.g. -1 or -2<br>"+
					"to reduce the Wound Threshold. Enter 0 to reset."
			}

			Vspacer { size: 10 }

			TextField {
				id: woundDelta
				width: 120
				height: 32
				anchors.horizontalCenter: parent.horizontalCenter
				validator: IntValidator { }
				inputMethodHints: Qt.ImhFormattedNumbersOnly
				placeholderText: "+/- Threshold"

				Keys.onReturnPressed: {
					woundDeltaDialog.click(StandardButton.Ok);
				}
			}

			Vspacer { size: 10 }
		}

		onAccepted: {
			var val = woundDelta.text.trim();
			characterData.setWoundDelta(parseInt(val));
			Qt.inputMethod.hide();
		}

		onRejected: {
			Qt.inputMethod.hide();
		}
	}

	Dialog {
		id: strainChangeDialog
		width: 400
		title: "Adjust Strain"
		standardButtons: StandardButton.Ok | StandardButton.Cancel

		Column {
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 16
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text:
					"<b><font size=\"+1\">Adjust Strain</font></b><br><br>"+
					"Enter Strain adjustment: e.g. 5 or +5 to<br>"+
					"add 5 Strain, -5 to remove 5 Strain."
			}

			Vspacer { size: 10 }

			TextField {
				id: strainAdjustment
				width: 120
				height: 32
				anchors.horizontalCenter: parent.horizontalCenter
				validator: IntValidator { }
				inputMethodHints: Qt.ImhFormattedNumbersOnly
				placeholderText: "+/- Strain"

				Keys.onReturnPressed: {
					strainChangeDialog.click(StandardButton.Ok);
				}
			}

			Vspacer { size: 10 }
		}

		onAccepted: {
			var val = strainAdjustment.text.trim();
			characterData.adjustStrain(parseInt(val));
			Qt.inputMethod.hide();
		}

		onRejected: {
			Qt.inputMethod.hide();
		}
	}

	Dialog {
		id: strainDeltaDialog
		width: 400
		title: "Adjust Strain Threshold"
		standardButtons: StandardButton.Ok | StandardButton.Cancel

		Column {
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 16
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text:
					"<b><font size=\"+1\">Adjust Strain Threshold</font></b><br><br>"+
					"Enter a Strain Threshold adjustment: e.g. -1 or -2<br>"+
					"to reduce the Strain Threshold. Enter 0 to reset."
			}

			Vspacer { size: 10 }

			TextField {
				id: strainDelta
				width: 120
				height: 32
				anchors.horizontalCenter: parent.horizontalCenter
				validator: IntValidator { }
				inputMethodHints: Qt.ImhFormattedNumbersOnly
				placeholderText: "+/- Threshold"

				Keys.onReturnPressed: {
					strainDeltaDialog.click(StandardButton.Ok);
				}
			}

			Vspacer { size: 10 }
		}

		onAccepted: {
			var val = strainDelta.text.trim();
			characterData.setStrainDelta(parseInt(val));
			Qt.inputMethod.hide();
		}

		onRejected: {
			Qt.inputMethod.hide();
		}
	}
	*/
}
