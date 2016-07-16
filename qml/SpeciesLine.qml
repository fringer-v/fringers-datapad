import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: speciesLine
	width: parent.width
	height: 4 + 24 + contentData.height + lineWidth

	property real lineWidth: 1
	property real titleFontSize: 14
	property real fontSize: 14

	Column {
		Row {
			Hspacer { size: 4 }

			Text {
				id: titleBox
				width: 150
				height: 24 - lineWidth
				font.pixelSize: titleFontSize
				font.bold: true
				verticalAlignment: Text.AlignVCenter
				font.family: "Arial"
				text: title
			}

			Text {
				width: speciesLine.width - titleBox.width - 8
				height: 24 - lineWidth
				font.pixelSize: fontSize
				font.italic: true
				font.family: "Arial"
				verticalAlignment: Text.AlignVCenter
				text: subtitle
			}
		}

		Hline {
			width: speciesLine.width
			height: lineWidth + 2
			lineSize: lineWidth
			alignment: "top"
		}

		FlowText {
			id: contentData
			width: speciesLine.width
			pixelSize: fontSize
			//topMargin: 2
			//leftMargin: 4
			hMargin: 4
			text: content
		}

		Hline {
			width: speciesLine.width
			height: lineWidth + 2
			lineSize: lineWidth
			alignment: "bottom"
		}
	}
}
