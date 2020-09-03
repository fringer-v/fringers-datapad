import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: notesArea
	width: 640
	height: 480
	//color: "red"

	property string text: ""
	property real fadeArea: 8

	// Create a flickable to view a large image.
	Flickable {
		id: view
		anchors.fill: parent
		//contentWidth: printArea.contentWidth
		//contentHeight: printArea.contentHeight+upperSpace.height+lowerSpace.height
		contentWidth: printArea.width
		contentHeight: printArea.height+upperSpace.height+lowerSpace.height
		clip: true
		flickableDirection: Flickable.VerticalFlick

		Column {

			Rectangle {
				id: upperSpace
				width: notesArea.width
				height: fadeArea*3/4
			}

			/*
			Text {
				id: printArea
				width: notesArea.width
				//font.family: "Comic Sans MS"
				font.family: "Arial"
				//font.family: "EotE Symbol"
				//font.pixelSize: 14
				wrapMode: Text.WordWrap
				text: notesArea.text
			}
			*/
			FlowText {
				id: printArea
				width: notesArea.width
				clearColor: "white"
				//font.family: "Comic Sans MS"
				//font.family: "Arial"
				//font.family: "EotE Symbol"
				//font.pixelSize: 14
				//wrapMode: Text.WordWrap
				text: notesArea.text
			}

			Rectangle {
				id: lowerSpace
				width: notesArea.width
				height: fadeArea
			}
		}

		// Only show the scrollbars when the view is moving.
		states: State {
			name: "ShowBars"
			when: view.movingVertically
			PropertyChanges { target: verticalScrollBar; opacity: 1 }
		}

		transitions: Transition {
			NumberAnimation { properties: "opacity"; duration: 400 }
		}
	}

	Rectangle {
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: fadeArea
		gradient: Gradient {
			GradientStop { position: 0.0; color: "#FFFFFFFF" }
			GradientStop { position: 1.0; color: "#00FFFFFF" }
		}
	}

	Rectangle {
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		height: fadeArea
		gradient: Gradient {
			GradientStop { position: 0.0; color: "#00FFFFFF" }
			GradientStop { position: 1.0; color: "#FFFFFFFF" }
		}
	}

	// Attach scrollbars to the right:
	ScrollBar {
		id: verticalScrollBar
		width: 10
		y: fadeArea/2
		height: view.height - fadeArea
		anchors.right: view.right
		opacity: 0
		orientation: Qt.Vertical
		position: view.visibleArea.yPosition
		pageSize: view.visibleArea.heightRatio
	}
}
