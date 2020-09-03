import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: gearLine
	width: parent === null ? 500 : parent.width // To avoid timing error: TypeError: Cannot read property of null
	height: Math.max(gearText.height, specialText.height) + topMargin + bottomMargin

	property var colWidths: adjustedlColWidths
	property real lineWidth: 1
	property int lineFontSize: 15
	property int descFontSize: 14
	property string lineColor: (index % 2) == 1 ? Constant.LIST_LINE_GRAY : "white"

	property int topMargin: 4
	property int bottomMargin: 3

	Row {
		Hspacer { size: listIndent; color: lineColor }

		Rectangle {
			width: colWidths[0]
			height: gearLine.height
			color: lineColor

			Text {
				id: gearText
				width: parent.width
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: lineFontSize
				font.family: "Arial"
				font.bold: true
				horizontalAlignment: Text.AlignLeft
				text: {
					if (quantity != 1) {
						var name = quantity +"x "+ gear;
						if (carry_state != 1 && stored > 0) {
							if (stored >= quantity)
								name = "1/"+name;
							else
								name = (quantity-stored)+"/"+name;
						}
						return name;
					}
					return gear;
				}
			}
		}

		Rectangle {
			width: colWidths[1]
			height: gearLine.height
			color: lineColor

			Text {
				width: parent.width
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: lineFontSize
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text: encumbrance
			}
		}

		HeldEquip {
			width: colWidths[2]
			height: gearLine.height
			color: lineColor
			isGear: true
			invUuid: uuid
			invName: gear
			invState: carry_state
			invQuantity: quantity
			invStored: stored
		}

		Rectangle {
			width: colWidths[3]
			height: parent.height
			color: lineColor

			Image {
				id: cart_icon
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				antialiasing: true
				smooth: true
				source: "qrc:/images/cart20.png"
			}

			MouseArea {
				anchors.fill: parent
				onClicked: gearDatapadDialog.newDatapadItem(itemkey, 3, gear); // ITEM_AMOUNT
			}
		}

		Rectangle {
			width: colWidths[4]
			height: gearLine.height
			color: lineColor

			/*
			Text {
				id: specialText
				width: parent.width
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: descFontSize
				font.family: "Arial"
				horizontalAlignment: Text.AlignLeft
				wrapMode: Text.Wrap
				text: special
			}
			*/
			FlowText {
				id: specialText
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				text: special
			}
		}

		Hspacer { size: listIndent; color: lineColor }
	}
}
