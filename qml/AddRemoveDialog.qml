import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Dialog {
	id: addRemoveItemsDialog
	width: 400
	standardButtons: StandardButton.Ok | StandardButton.Cancel

	property int price: 0
	property string countDeltaText: ""
	property string amountDeltaText: ""

	Column {
		anchors.horizontalCenter: parent.horizontalCenter

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			font.pixelSize: 16
			font.family: "Arial"
			horizontalAlignment: Text.AlignHCenter
			text:
				"<b><font size=\"+1\">"+addRemoveItemsDialog.title+"</font></b><br><br>"+
				"Enter the number of items to either add or remove<br>"+
				"(negative number). If the items are sold or<br>"+
				"purchased, enter a corresponding amount.<br><br>"+
				"Base price for this item is "+addRemoveItemsDialog.price+" CR."
		}

		Vspacer { size: 10 }

		Row {
			anchors.horizontalCenter: parent.horizontalCenter

			TextField {
				id: countDelta
				width: 120
				height: 32
				validator: IntValidator { }
				inputMethodHints: Qt.ImhFormattedNumbersOnly
				placeholderText: "+/- Number"
				text: countDeltaText

				Keys.onReturnPressed: {
					addRemoveItemsDialog.click(StandardButton.Ok);
				}
			}

			Hspacer { size: 20 }

			TextField {
				id: amountDelta
				width: 120
				height: 32
				validator: IntValidator { }
				inputMethodHints: Qt.ImhFormattedNumbersOnly
				placeholderText: "+/- Credits"
				text: amountDeltaText

				Keys.onReturnPressed: {
					addRemoveItemsDialog.click(StandardButton.Ok);
				}
			}
		}

		Vspacer { size: 10 }
	}

	onAccepted: {
		if (countDelta.text.trim().length > 0) {
			if (amountDelta.text.trim().length == 0)
				amountDelta.text = "0";
			characterData.addInvItem(parseInt(countDelta.text), key, "", parseInt(amountDelta.text));
		}
		Qt.inputMethod.hide();
	}

	onRejected: {
		Qt.inputMethod.hide();
	}
}
