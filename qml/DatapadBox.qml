import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

TitleBox {
	id: acquiredXp
	boxTitle: "INVENTORY"
	boxTitleWidth: 120

	Image {
		// parent.width - 60
		width: 129
		// sourceSize.height * ((parent.width - 60) / sourceSize.width)
		height: 78
		x: 30
		y: 24
		source: "qrc:/images/datapad129x78.png"
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			characterData.showInventory();
			controller.state = "cashout";
		}
	}
}
