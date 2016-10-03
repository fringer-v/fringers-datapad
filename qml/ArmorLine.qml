import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: armorLine
	width: parent.width
	height: row1.height + ((attachmentsVisible && attachments.length > 0) ? row2.height : 0)
	color: lineColor

	property var colWidths: adjustedlColWidths
	property real lineWidth: 1
	property int minLineHeight: 24
	property int lineFontSize: 15
	property int descFontSize: 14
	property int bottomTextMargin: 3
	property string lineColor: (index % 2) == 1 ? Constant.LIST_LINE_GRAY : "white"
	property bool attachmentsVisible: false

	Row {
		id: row1
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: Math.max(Math.max(armorText.height, specialText.height), minLineHeight);

		Hspacer { size: listIndent; color: lineColor }

		Rectangle {
			width: colWidths[0]
			height: parent.height
			color: lineColor

			Text {
				id: armorText
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				font.pixelSize: lineFontSize
				font.family: "Arial"
				font.bold: true
				horizontalAlignment: Text.AlignLeft
				text:  {
					if (quantity != 1) {
						var name = quantity +"x "+ armor;
						if (carry_state != 1 && stored > 0) {
							if (stored >= quantity)
								name = "1/"+name;
							else
								name = (quantity-stored)+"/"+name;
						}
						return name;
					}
					return armor;
				}
			}
		}

		Rectangle {
			width: colWidths[1]
			height: parent.height
			color: lineColor

			Text {
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				font.pixelSize: lineFontSize
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text: soak
			}
		}

		Rectangle {
			width: colWidths[2]
			height: parent.height
			color: lineColor

			Text {
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				font.pixelSize: lineFontSize
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text: mdef
			}
		}

		Rectangle {
			width: colWidths[3]
			height: parent.height
			color: lineColor

			Text {
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				font.pixelSize: lineFontSize
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text: rdef
			}
		}

		Rectangle {
			width: colWidths[4]
			height: parent.height
			color: lineColor

			Text {
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				font.pixelSize: lineFontSize
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text: encumbrance
			}
		}

		HeldEquip {
			width: colWidths[5]
			height: parent.height
			color: lineColor
			invUuid: uuid
			invName: armor
			invState: carry_state
			invQuantity: quantity
			invStored: stored
		}

		Rectangle {
			width: colWidths[6]
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
				onClicked: gearDatapadDialog.newDatapadItem(itemkey, 3, armor); // ITEM_AMOUNT
			}
		}

		Rectangle {
			width: colWidths[7]
			height: parent.height
			color: lineColor

			/*
			Text {
				id: specialText
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				font.pixelSize: descFontSize
				font.family: "Arial"
				horizontalAlignment: Text.AlignLeft
				wrapMode: Text.Wrap
				text: features
			}
			*/
			FlowText {
				id: specialText
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				//text: features
				text: {
					if (attachments.length === 0)
						return features;
					if (features.length === 0)
						return "[U]Attachments...[u]";
					return features + ", [U]Attachments...[u]";
				}
			}

			MouseArea {
				anchors.fill: parent

				onClicked: attachmentsVisible = !attachmentsVisible
			}
		}

		Hspacer { size: listIndent; color: lineColor }
	}

	Row {
		id: row2
		visible: attachmentsVisible && attachments.length > 0
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		height: attachText.height + bottomTextMargin

		Hspacer { size: listIndent; color: lineColor }

		Column {
			/*
			Text {
				id: attachText
				width: armorLine.width - listIndent*2
				font.pixelSize: descFontSize
				font.family: "Arial"
				horizontalAlignment: Text.AlignLeft
				wrapMode: Text.Wrap
				text: "[U]Attachments:[u] " + attachments
			}
			*/
			FlowText {
				id: attachText
				width: armorLine.width - listIndent*2
				text: "[U]Attachments:[u] " + attachments
			}

			Vspacer { size: bottomTextMargin; color: lineColor }
		}

		Hspacer { size: listIndent; color: lineColor }
	}
}
