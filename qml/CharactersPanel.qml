import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import space.fringer.datapad 1.0

// Select Characters
Rectangle {
	id: panel

	property int sizeMargin: 48

	Column {
		Column {
			id: characterSection

			Header {
				width: metaPanel.contentWidth
				headText: "CHARACTERS"
			}

			Vspacer { size: panels.lineSpace - 8 }

			Row {
				Hspacer { size: sizeMargin }

				CharacterList {
					width: metaPanel.contentWidth - sizeMargin*2
					height: Constant.CHARACTER_HEIGHT*2.5 + panels.lineSpace*2 + 16 // 16 = fade area
				}
			}

			Vspacer { size: panels.lineSpace - 8 }
		}

		Row {
			id: dataSetSection

			// DATASETS
			Column {
				Header {
					id: dataSetHeader
					width: (metaPanel.contentWidth - 20 - 100 - 20) / 2
					headText: "DATASETS"
				}

				Vspacer { size: panels.lineSpace - 8 }

				DataSetList {
					width: dataSetHeader.width
					height: metaPanel.contentHeight - characterSection.height - dataSetHeader.height - (panels.lineSpace - 8)*2
				}
			}

			Hspacer { size: 20 }

			Column {
				Vspacer { size: 40 }

				Button {
					width: 100
					height: 100
					//border.color: "black"
					//border.width: 1
					//color: "lightgrey"
					//text: "<center>Sync with<br>Server Now<center>"

					Text {
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
						font.pixelSize: 16
						font.family: "Arial"
						horizontalAlignment: Text.AlignHCenter
						text: "Sync with\nServer Now"
					}

					onClicked: {
						characterData.syncWithServer();
					}
				}
			}

			Hspacer { size: 20 }

			// CURRENT DATA
			Column {
				Header {
					id: currentDataHeader
					width: (metaPanel.contentWidth - 20 - 100 - 20) / 2
					headText: "CURRENT DATA"
				}

				Vspacer { size: panels.lineSpace - 8 }

				CurrentDataList {
					width: currentDataHeader.width
					height: metaPanel.contentHeight - characterSection.height - dataSetHeader.height - (panels.lineSpace - 8)*2
				}
			}
		}
	}

	BusyIndicator {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		running: characterData.loading
	}
}
