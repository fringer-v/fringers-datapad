import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw

Rectangle {
	id: negpoolline
	height: 25
	width: parent.width
	property int checkWidth: negpoolline.height - 5 + 10
	property int dicePoolWidth: 120
	property int spacer: 10
	property int fontSize: 14
	property int checkReference: 0
	property string checkDicePool: ""
	color: "#00000000"

	Column {
		Row {
			Hspacer { size: 10 }

			Rectangle {
				width: checkWidth
				height: negpoolline.height - 1

				Rectangle {
					width: negpoolline.height - 5
					height: negpoolline.height - 5
					anchors.left: parent.left
					anchors.verticalCenter: parent.verticalCenter
					border.width: 1
					border.color: "darkgray"

					Image {
						visible: (characterData.negativePool & checkReference) != 0
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
						source: "qrc:/images/tick.png"
					}
				}
			}

			DicePool {
				width: negpoolline.width - 10 - checkWidth
				height: negpoolline.height - 1
				center: false
				pool: checkDicePool
				color: "white"
			}
		}

		Row {
			Hspacer { size: 10; color: "#00000000";}

			Rectangle {
				height: 1
				width: checkWidth
				color: "white"
			}

			Rectangle {
				height: 1
				width: negpoolline.width - 10 - checkWidth
				color: "darkgray"
			}
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			characterData.negativePool = checkReference;
		}
	}
}
