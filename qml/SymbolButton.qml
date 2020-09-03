import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: symbolbutton
	width: buttonWidth + buttonBorder*2
	height: buttonHeight + buttonBorder*2
	color: "#00000000"

	property int buttonBorder: 5
	property int buttonWidth: 50
	property int buttonHeight: 40
	property string buttonSymbol: ""

	signal pressed();

	Rectangle {
		width: buttonWidth
		height: buttonHeight
		border.width: 1
		border.color: "black"
		anchors.verticalCenter: parent.verticalCenter
		anchors.horizontalCenter: parent.horizontalCenter
		color: "#00000000"

		Button {
			width: buttonWidth - 2
			height: buttonHeight - 2
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter

			onClicked: symbolbutton.pressed()
		}
	}

	DicePool {
		height: 24
		width: 30
		margin: 0
		color: "#00FFFFFF"
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter

		pool: buttonSymbol
	}
}
