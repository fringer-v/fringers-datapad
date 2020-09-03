import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Dialog {
	id: carryItemsDialog
	width: 400
	standardButtons: StandardButton.Ok | StandardButton.Cancel

	property string itemUuid: ""
	property int itemTotal: 0
	property int itemStored: 0

	Column {
		anchors.horizontalCenter: parent.horizontalCenter

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			font.pixelSize: 16
			font.family: "Arial"
			horizontalAlignment: Text.AlignHCenter
			text:
				"<b><font size=\"+1\">"+carryItemsDialog.title+"</font></b><br><br>"+
				"You have <b>"+itemTotal+"</b> items in your inventory.<br><br>"+
				"Enter the number you wish to carry.";
		}

		Vspacer { size: 10 }

		Row {
			anchors.horizontalCenter: parent.horizontalCenter

			TextField {
				id: carryAmount
				width: 120
				height: 32
				validator: IntValidator { }
				inputMethodHints: Qt.ImhFormattedNumbersOnly
				placeholderText: "Number"
				text: itemTotal - itemStored <= 0 ? itemTotal : itemTotal - itemStored

				Keys.onReturnPressed: {
					carryItemsDialog.click(StandardButton.Ok);
				}
			}
		}

		Vspacer { size: 10 }
	}

	onAccepted: {
		if (carryAmount.text.trim().length > 0) {
			if (carryAmount.text.trim().length == 0)
				return;
			var carry = parseInt(carryAmount.text);
			if (carry < 0)
				carry = 0;
			if (carry > itemTotal)
				carry = itemTotal;
			characterData.changeEquipment(itemUuid, 2, itemTotal - carry);
		}
		Qt.inputMethod.hide();
	}

	onRejected: {
		Qt.inputMethod.hide();
	}
}
