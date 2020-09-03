import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: money
	width: parent.width
	height: 199

	Row {
		Attribute {
			id: moneyValue
			y: money.height - moneyValue.height

			attrText: "CREDITS"
			attrValue: characterData.locked === 0 ? characterData.credits : ""
			attrValueFontSize: 24

			Image {
				width: 30
				height: 30
				y: 36
				anchors.horizontalCenter: parent.horizontalCenter
				//anchors.verticalCenter: parent.verticalCenter
				source: "qrc:/images/lock.png"
				visible: characterData.locked !== 0
			}

			MouseArea {
				anchors.fill: parent
				onClicked: gearDatapadDialog.newDatapadItem("", 5, "");
			}
		}

		Hspacer { size: 10 }

		DatapadBox {
			id: datapadbox
			height: money.height
			width: money.width - moneyValue.width - 10
		}
	}
}
