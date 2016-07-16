import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: talentLine
	width: parent.width
	height: textHeight + topMargin + bottomMargin + 1

	property var colWidths: adjustedlColWidths
	property real lineWidth: 1
	property int lineFontSize: 15
	property int descFontSize: 14
	property string lineColor: (index % 2) == 1 ? "#F1F1F3" : "white"

	property int topMargin: 4
	property int bottomMargin: 2
	property int textHeight: Math.max(talentText.height, acquisitionText.height, descriptionText.height)

	Column {
		Rectangle {
			width: talentLine.width
			height: topMargin
			color: lineColor
		}

		Row {
			Hspacer { size: listIndent; color: lineColor }

			Rectangle {
				width: colWidths[0]
				height: textHeight
				color: lineColor

				Text {
					id: talentText
					width: parent.width
					anchors.verticalCenter: parent.verticalCenter
					font.pixelSize: lineFontSize
					font.family: "Arial"
					font.bold: true
					horizontalAlignment: Text.AlignLeft
					text: talent
				}
			}

			Rectangle {
				width: colWidths[1]
				height: textHeight
				color: lineColor

				Text {
					width: parent.width
					anchors.verticalCenter: parent.verticalCenter
					font.pixelSize: lineFontSize
					font.family: "Arial"
					horizontalAlignment: Text.AlignHCenter
					text: ranks == 0 ? "--" : ranks
				}
			}

			Rectangle {
				width: colWidths[2]
				height: textHeight
				color: lineColor

				Text {
					width: parent.width
					anchors.verticalCenter: parent.verticalCenter
					font.pixelSize: lineFontSize
					font.family: "Arial"
					horizontalAlignment: Text.AlignHCenter
					text: activation
				}
			}

			Rectangle {
				width: colWidths[3]
				height: textHeight
				color: lineColor

				Text {
					id: acquisitionText
					width: parent.width
					anchors.verticalCenter: parent.verticalCenter
					font.pixelSize: descFontSize
					font.family: "Arial"
					horizontalAlignment: Text.AlignLeft
					wrapMode: Text.Wrap
					text: acquisition
				}
			}

			Rectangle {
				id: textCol
				width: colWidths[4]
				height: textHeight
				color: lineColor

				 FlowText {
					id: descriptionText
					width: parent.width
					anchors.verticalCenter: parent.verticalCenter
					text: description
					clearColor: lineColor
				}
			}

			Hspacer { size: listIndent; color: lineColor }
		}

		Rectangle {
			width: talentLine.width
			height: bottomMargin
			color: lineColor
		}

		Rectangle {
			width: talentLine.width
			height: 1
			color: Constant.LIST_UNDERLINE_GRAY
		}
	}
}
