import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

// Experience, Special Features, Talents
Rectangle {
	id: panel

	property int topSectionHeight: 123

	Column {
		Row {
			Column {
				width: encumberance.width

				Header {
					width: encumberance.width
					headText: "ENCUMBERANCE"
				}

				Vspacer { size: panels.lineSpace }

				Encumberance {
					id: encumberance
					height: topSectionHeight - panels.lineSpace - Constant.HEADER_HEIGHT
				}
			}

			Hspacer { size: 20 }

			Column {
				id: inventory
				width: metaPanel.contentWidth - 20 - encumberance.width

				Header {
					width: inventory.width
					headText: "DATAPAD"
				}

				Vspacer { size: panels.lineSpace }

				InventoryBox {
					width: inventory.width
					height: topSectionHeight - panels.lineSpace - Constant.HEADER_HEIGHT
				}
			}
		}

		Vspacer { size: panels.lineSpace }

		Header {
			width: metaPanel.contentWidth
			headText: "GEAR"
		}

		Vspacer { size: panels.lineSpace }

		GearList {
			width: metaPanel.contentWidth
			height: metaPanel.contentHeight - topSectionHeight - panels.lineSpace*2 - Constant.HEADER_HEIGHT
		}
	}

	onVisibleChanged: {
		if (visible)
			characterData.showGear();
	}
}

