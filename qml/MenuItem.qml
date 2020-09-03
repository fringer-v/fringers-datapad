import QtQuick 2.3
import "../js/drawing.js" as Draw

Rectangle {
	id: menu_item
	width: parent.width
	height: Draw.pixel(40)

	property var menuPanel: undefined
	property string menuColor: controller.currentPanel == menuPanel ? "white" : "#777"
	property string menuBackgroundColor: "black"

	MouseArea {
		anchors.fill: parent
		onClicked: {
			panels.setVisible(menuPanel);
		}
	}

	Rectangle {
		id: spacer
		anchors.left: parent.left
		width: 10
		height: parent.height
		color: menuBackgroundColor
	}

	Rectangle {
		anchors.left: spacer.right
		anchors.right: parent.right
		height: parent.height
		color: menuBackgroundColor

		Text {
			anchors.bottom: line.top
			anchors.margins: 4
			font.pixelSize: Draw.pixel(18)
			text: menuPanel.panelTitle
			color: menuColor
		}

		Rectangle {
			id: line
			anchors.bottom: parent.bottom
			width: parent.width - Draw.pixel(10)
			height: Draw.pixel(1.5)
			color: menuColor
		}
	}
}


