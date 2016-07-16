import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: header
	height: Draw.pixel(32)
	color: "#333333"

	property string title: ""
	property int iconMargin: 20
	property int iconGap: 32
	property int iconSize: 22

	Rectangle {
		id: menuIcon
		width: iconGap + iconSize + iconMargin
		height: header.height
		anchors.left: parent.left
		color: "#333333"

		Rectangle {
			id: leftZ
			width: iconMargin
			height: parent.height
			anchors.left: parent.left
			color: parent.color
		}

		Image {
			width: iconSize
			height: iconSize
			anchors.left: leftZ.right
			anchors.verticalCenter: parent.verticalCenter
			source: "qrc:/images/menu-bars.png"
		}

		MouseArea {
			anchors.fill: parent

			onClicked: {
				controller.state = "menuout";
			}
		}
	}

	Indicator {
		id: wsIcon
		width: 84
		height: 24
		anchors.left: menuIcon.right
		anchors.verticalCenter: parent.verticalCenter
		attrValue1: characterData.currentWounds
		attrValue2: characterData.currentStrain
		attrMaxValue1: characterData.wound+characterData.woundDelta
		attrMaxValue2: characterData.strain+characterData.strainDelta
		picSize: 17
		picUrl: "qrc:/images/white-success17.png"

		MouseArea {
			anchors.fill: parent

			onClicked: {
				panels.setVisible(characteristics);
			}
		}
	}

	Rectangle {
		id: skillIcon
		width: iconGap / 2 + iconSize+2 + iconGap
		height: header.height
		anchors.left: wsIcon.right
		//color: "#333333"
		color: "#333333"

		Image {
			width: iconSize+2
			height: iconSize+2
			anchors.left: leftY.right
			anchors.verticalCenter: parent.verticalCenter
			source: "qrc:/images/hex64.png"
		}

		Rectangle {
			id: leftY
			width: iconGap
			height: parent.height
			anchors.left: parent.left
			color: parent.color
		}

		MouseArea {
			anchors.fill: parent

			onClicked: panels.setVisible(skills)
		}

	}

	Rectangle {
		id: talentIcon
		width: iconGap / 2 + iconSize + iconGap / 2
		height: header.height
		anchors.left: skillIcon.right
		color: "#333333"

		Image {
			width: iconSize+2
			height: iconSize+2
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
			//source: "qrc:/images/desc-icon22.png"
			source: "qrc:/images/portrait24.png"
		}

		MouseArea {
			anchors.fill: parent

			onClicked: panels.setVisible(description);
		}
	}

	Rectangle {
		width: iconGap / 2 + iconSize + iconGap / 2
		height: header.height
		anchors.left: talentIcon.right
		color: "#333333"

		Image {
			width: iconSize
			height: iconSize
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
			source: characterData.locked ? "qrc:/images/locked.png" : "qrc:/images/unlock.png"
		}

		MouseArea {
			anchors.fill: parent

			onClicked: characterData.locked = !characterData.locked
		}
	}

	Text {
		anchors.verticalCenter: parent.verticalCenter
		anchors.horizontalCenter: parent.horizontalCenter
		color: "white"
		text: header.title.trim() == "" ? "<u>Character List</u>" : header.title
		font.pixelSize: 20
		font.family: "Comic Sans MS"
		font.bold: true
		//bold 26px

		MouseArea {
			anchors.fill: parent

			onClicked: panels.setVisible(characters);
		}
	}

	Rectangle {
		width: iconGap / 2 + iconSize + iconGap / 2
		height: header.height
		anchors.right: invIcon.left
		color: "#333333"

		Image {
			width: iconSize+2
			height: iconSize+2
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
			source: "qrc:/images/award-white24.png"
		}

		MouseArea {
			anchors.fill: parent

			onClicked: panels.setVisible(experience)
		}
	}

	Rectangle {
		id: invIcon
		width: iconGap / 2 + iconSize + iconGap / 2
		height: header.height
		anchors.right: targetIcon.left
		color: "#333333"

		Image {
			width: iconSize
			height: iconSize
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
			source: "qrc:/images/inv-icon-white22.png"
		}

		MouseArea {
			anchors.fill: parent

			onClicked: {
				characterData.showInventory();
				controller.state = "cashout";
			}
		}
	}

	Rectangle {
		id: targetIcon
		width: iconGap / 2 + iconSize + iconGap
		height: header.height
		anchors.right: encIcon.left
		color: "#333333"

		Image {
			width: iconSize
			height: iconSize
			anchors.right: rightY.left
			anchors.verticalCenter: parent.verticalCenter
			source: "qrc:/images/target.png"
		}

		Rectangle {
			id: rightY
			width: iconGap
			height: parent.height
			anchors.right: parent.right
			color: parent.color
		}

		MouseArea {
			anchors.fill: parent

			onClicked:panels.setVisible(weapons);
		}
	}

	Indicator {
		id: encIcon
		width: 84
		height: 24
		anchors.right: prefsIcons.left
		anchors.verticalCenter: parent.verticalCenter
		attrValue1: characterData.encValue
		attrValue2: characterData.cumbValue
		attrMaxValue1: characterData.encThreshold
		attrMaxValue2: characterData.cumbThreshold
		picSize: 16
		picUrl: "qrc:/images/white-tools16.png"

		MouseArea {
			anchors.fill: parent

			onClicked: {
				panels.setVisible(inventory);
			}
		}
	}

	Rectangle {
		id: prefsIcons
		width: iconGap + iconSize + iconMargin
		height: header.height
		anchors.right: parent.right
		color: "#333333"

		Image {
			width: iconSize
			height: iconSize
			anchors.right: rightZ.left
			anchors.verticalCenter: parent.verticalCenter
			source: "qrc:/images/gear-icon.png"
		}

		Rectangle {
			id: rightZ
			width: iconMargin
			height: parent.height
			anchors.right: parent.right
			color: parent.color
		}

		MouseArea {
			anchors.fill: parent

			onClicked: {
				controller.state = "prefout";
			}
		}
	}
}
