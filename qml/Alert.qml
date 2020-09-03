import QtQuick 2.3
import "../js/drawing.js" as Draw

Rectangle {
	id: alert
	color: "#7F000000"
	anchors.fill: parent

	property string title: ""
	property string message: ""

	MouseArea {
		anchors.fill: parent
		onClicked: {
			alert.visible = false;
		}
	}

	Rectangle {
		id: box
		width: 360
		height: 200
		color: "white"
		border.color: "black"
		border.width: 1
		radius: 10
		anchors.verticalCenter: parent.verticalCenter
		anchors.horizontalCenter: parent.horizontalCenter

		Row {
			x: 8
			y: 5

			Image {
				id: pic
				width: 72
				height: 72
				source: "qrc:/images/alert72.png"
			}

			Text {
				width: box.width - pic.width - 8*2
				height: pic.height
				font.pixelSize: 14
				font.family: "Arial"
				font.bold: true
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				text: title
			}
		}

		Text {
			width: box.width - 40
			y: 5 + pic.height + 10
			anchors.horizontalCenter: parent.horizontalCenter
			font.pixelSize: 14
			font.family: "Arial"
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.Wrap
			text: message
		}
	}
}
