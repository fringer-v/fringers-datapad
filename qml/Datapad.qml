import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util

Rectangle {
	id: inventory

	// This area prevents to dialog from closing if you click wrong
	MouseArea {
		width: parent.width + 2
		height: parent.height + 2
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
	}

	Column {
		Row {
			Hspacer { size: 10 }

			Column {
				Column {
					id: headerSection

					Vspacer { size: panels.lineSpace }

					Header {
						width: inventory.width - 20
						headText: "INVENTORY"
					}

					Vspacer { size: panels.lineSpace - 8 }
				}

				CashBookList {
					width: inventory.width - 20;
					height: inventory.height - headerSection.height;
				}
			}
		}
	}

	onXChanged: {
		if (x > panels.x + (panels.width - inventory.width)/2 + inventory.width/2)
			datapadItemDialog.closeDialog();
	}

	DatapadDialog {
		id: datapadItemDialog
	}
}
