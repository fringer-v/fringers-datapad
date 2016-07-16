import QtQuick 2.0
import "../js/drawing.js" as Draw

Rectangle {
	id: metaPanel
	color: "white"

	property string panelTitle: ""
	property string panelContent: ""

	property real leftMargin: 8
	property real rightMargin: 8
	property real topMargin: 10
	property real bottomMargin: 8
	property real contentAreaWidth: panels.width - leftMargin
	property real contentAreaHeight: panels.height - navigationBar.height - topMargin
	property real contentWidth: contentAreaWidth - rightMargin
	property real contentHeight: contentAreaHeight - bottomMargin

	Column {
		anchors.fill: parent

		NavigationBar {
			id: navigationBar
			width: metaPanel.width
			title: characterData.name
		}

		Vspacer { size: metaPanel.topMargin }

		Row {
			Hspacer { size: metaPanel.leftMargin }

			Loader {
				id: contentArea
				width: contentAreaWidth
				height: contentAreaHeight
				source: panelContent
			}
		}
	}
}
