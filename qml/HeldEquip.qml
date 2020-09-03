import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	property bool isGear: false
	property string invUuid: ""
	property string invName: ""
	property int invState: 1
	property int invQuantity: 1
	property int invStored: 0

	Image {
		id: menu_icon
		anchors.verticalCenter: parent.verticalCenter
		anchors.horizontalCenter: parent.horizontalCenter
		antialiasing: true
		smooth: true
		source: {
			if (invState == 3) // IS_EQUIPPED
				return "qrc:/images/tools20.png";
			if (invState == 2) // IS_HELD
				return "qrc:/images/hand20.png";
			return "";
		}
	}

	MouseArea {
		anchors.fill: parent

		onClicked: {
			var new_state = invState + 1;
			var new_stored = invStored;

			if (new_state == 2 && invQuantity > 1) {
				carryItemsDialog.itemUuid = invUuid;
				carryItemsDialog.itemTotal = invQuantity;
				carryItemsDialog.itemStored = invStored;
				carryItemsDialog.open();
				new_stored = carryItemsDialog.itemsStored;
			}
			else
				characterData.changeEquipment(invUuid, new_state, new_stored);
		}
	}

	CarryDialog {
		id: carryItemsDialog
		title: "Carry "+invName
	}
}
