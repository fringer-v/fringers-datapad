import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw

/*
Canvas {
	id: injury
	height: 35
	width: 429

	onPaint: {
		var ctx = injury.getContext("2d");

		Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);

		Draw.bLine(ctx, 0, 0, 429, 35, 1, "black");
		Draw.diamond(ctx, 314, 35/2, 25, 1.5, "black", "white");
		Draw.diamond(ctx, 340, 35/2, 25, 1.5, "black", "white");
		Draw.diamond(ctx, 366, 35/2, 25, 1.5, "black", "white");
		Draw.diamond(ctx, 392, 35/2, 25, 1.5, "black", "white");

		Draw.exit(ctx);
	}
}
*/

Rectangle {
	id: injury
	height: 35
	width: 429
	property int leftMargin: 5
	property int percentWidth: 50
	property int spacer: 4
	property int rightMargin: 8

	Column {
		Row {
			Hspacer { size: leftMargin }

			Text {
				width: percentWidth
				height: injury.height-1
				font.pixelSize: 20
				font.family: "Arial"
				font.bold: true
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				text: percent > 0 ? percent + "%" : ""
			}

			Hspacer { size: spacer }

			Rectangle {
				width: injury.width - leftMargin - percentWidth - spacer - spacer - injuryDice.width - rightMargin
				height: injury.height-1

				FlowText {
					anchors.fill: parent
					vMargin: 2
					hMargin: 2
					text: percent > 0 ? "[B]" + title + ":[b] " + description : ""
				}
				/*
				Text {
					anchors.verticalCenter: parent.verticalCenter
					width: parent.width
					font.pixelSize: 14
					font.family: "Arial"
					textFormat: Text.RichText
					horizontalAlignment: Text.AlignLeft
					lineHeightMode: Text.FixedHeight
					lineHeight: 17
					wrapMode: Text.Wrap
					text: percent > 0 ? "<b>" + title + ":</b> " + description : ""
				}
				*/
			}

			Hspacer { size: spacer }

			Rectangle {
				id: injuryDice
				width: 26*4
				height: injury.height-1

				DicePool {
					anchors.verticalCenter: parent.verticalCenter
					width: 26*4
					height: 25
					margin: 0
					spacing: 6
					pool: dice
					color: "white"
				}
			}

			Hspacer { size: rightMargin }
		}

		Rectangle {
			height: 1
			width: injury.width
			color: "black"
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			if (ref == 0)
				addInjuryDialog.open();
			else
				removeInjuryDialog.open();
		}
	}

	Dialog {
		id: removeInjuryDialog
		width: 400
		title: "Remove Critical Injury"
		standardButtons: StandardButton.Yes | StandardButton.Cancel

		Column {
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 16
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text:
					"<b><font size=\"+1\">Remove Critical Injury</font></b><br><br>"+
					"Do you wish to remove this Critical Injury ("+percent+"%)?"
			}

			Vspacer { size: 10 }
		}

		onYes: {
			characterData.removeCriticalWound(ref);
		}
	}

	Dialog {
		id: addInjuryDialog
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
					"<b><font size=\"+1\">Add Critical Injury</font></b><br><br>"+
					"Enter the percentage and the d10 roll of<br>"+
					"the new Critical Injury.";
			}

			Vspacer { size: 10 }

			Row {
				anchors.horizontalCenter: parent.horizontalCenter

				TextField {
					id: injuryPercent
					width: 120
					height: 32
					validator: IntValidator { }
					inputMethodHints: Qt.ImhFormattedNumbersOnly
					placeholderText: "Injury %"

					Keys.onReturnPressed: {
						addInjuryDialog.click(StandardButton.Ok);
					}
				}

				Hspacer { size: 20 }

				TextField {
					id: injuryD10
					width: 120
					height: 32
					validator: IntValidator { }
					inputMethodHints: Qt.ImhFormattedNumbersOnly
					placeholderText: "Injury d10"

					Keys.onReturnPressed: {
						addInjuryDialog.click(StandardButton.Ok);
					}
				}
			}

			Vspacer { size: 10 }
		}

		onAccepted: {
			characterData.addCriticalWound(parseInt(injuryPercent.text), parseInt(injuryD10.text));
			Qt.inputMethod.hide();
		}

		onRejected: {
			Qt.inputMethod.hide();
		}
	}

}
