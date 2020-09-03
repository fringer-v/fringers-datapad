import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util

Item {
	id: listHeader
	height: Constant.LIST_HEADER_HEIGHT

	property int headerIndent: 4
	property string backgroundColor: Constant.LIST_HEADER_COLOR
	property int headerFontSize: 13
	property string headerFont: "Arial"
	property int fadeHeaderHeight: 12
	property var titles: undefined
	property var widths: undefined

	Column {
		Rectangle {
			id: headerTextLine
			width: listHeader.width
			height: listHeader.height - 1
			gradient: Gradient {
				GradientStop { position: (headerTextLine.height - fadeHeaderHeight) / headerTextLine.height; color: Util.makeTransparent("FF", backgroundColor) }
				GradientStop { position: 1.0; color: Util.makeTransparent("80", backgroundColor) }
			}
		}

		Rectangle {
			width: listHeader.width
			height: 1
			color: backgroundColor
		}
	}

	Row {
		height: parent.height - 2
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		property int adjust: 1

		Hspacer { size: headerIndent; color: "transparent" }

		Text {
			visible: widths.length > 0
			width: widths.length > 0 ? widths[0] : 0
			height: parent.height
			font.pixelSize: headerFontSize
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			font.family: headerFont
			text: titles.length > 0 ? titles[0] : ""
		}

		Text {
			visible: widths.length > 1
			width: widths.length > 1 ? widths[1] : 0
			height: parent.height
			font.pixelSize: headerFontSize
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			font.family: headerFont
			text: titles.length > 1 ? titles[1] : ""
		}

		Text {
			visible: widths.length > 2
			width: widths.length > 2 ? widths[2] : 0
			height: parent.height
			font.pixelSize: headerFontSize
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			font.family: headerFont
			text: titles.length > 2 ? titles[2] : ""
		}

		Text {
			visible: widths.length > 3
			width: widths.length > 3 ? widths[3] : 0
			height: parent.height
			font.pixelSize: headerFontSize
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			font.family: headerFont
			text: titles.length > 3 ? titles[3] : ""
		}

		Text {
			visible: widths.length > 4
			width: widths.length > 4 ? widths[4] : 0
			height: parent.height
			font.pixelSize: headerFontSize
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			font.family: headerFont
			text: titles.length > 4 ? titles[4] : ""
		}

		Text {
			visible: widths.length > 5
			width: widths.length > 5 ? widths[5] : 0
			height: parent.height
			font.pixelSize: headerFontSize
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			font.family: headerFont
			text: titles.length > 5 ? titles[5] : ""
		}

		Text {
			visible: widths.length > 6
			width: widths.length > 6 ? widths[6] : 0
			height: parent.height
			font.pixelSize: headerFontSize
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			font.family: headerFont
			text: titles.length > 6 ? titles[6] : ""
		}

		Text {
			visible: widths.length > 7
			width: widths.length > 7 ? widths[7] : 0
			height: parent.height
			font.pixelSize: headerFontSize
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			font.family: headerFont
			text: titles.length > 7 ? titles[7] : ""
		}

		Hspacer { size: headerIndent; color: "transparent" }
	}
}
