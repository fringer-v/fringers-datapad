import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: padbutton;
	width: buttonWidth + buttonBorder*2
	height: buttonHeigth + buttonBorder*2
	color: "#00000000"

	property int buttonBorder: 5
	property int buttonWidth: 60
	property int buttonHeigth: buttonWidth
	property string buttonText: ""
	property int fontSize: 20
	property string fontStyle: "Comic Sans MS"
	property bool fontBold: false

	signal pressed();

	Rectangle {
		width: buttonWidth
		height: buttonHeigth
		border.width: 1
		border.color: "black"
		anchors.verticalCenter: parent.verticalCenter
		anchors.horizontalCenter: parent.horizontalCenter
		color: "#00000000"
		radius: 4

		Button {
			width: buttonWidth - 2
			height: buttonHeigth - 2
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: fontSize
				font.family: fontStyle
				font.bold: fontBold
				horizontalAlignment: Text.AlignHCenter
				text: buttonText
			}

			onClicked: padbutton.pressed()
		}
	}
}

