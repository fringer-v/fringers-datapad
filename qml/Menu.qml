import QtQuick 2.3
import "../js/drawing.js" as Draw

Rectangle {
	color: "black"
	height: parent.height
	width: Draw.pixel(280)

	property string portraitUrl: "qrc:/images/xiaan.png" // "qrc:images/anonymous-user.png"
	property real portraitBevel: Draw.pixel(10)
	property real lineWidth: Draw.pixel(2)

	Behavior on x {
		PropertyAnimation { easing.type: Easing.InOutQuad; duration: 500 }
	}

	Column {
		spacing: Draw.pixel(6)
		anchors.fill: parent
		anchors.topMargin: Draw.pixel(12)

		Text {
			font.pointSize: Draw.pixel(18)
			font.bold: true
			font.family: comicRegular.name
			text: characterData.name
			anchors.horizontalCenter: parent.horizontalCenter
			color: "#777"
		}

		Canvas {
			id: portrait
			antialiasing: true
			width: Draw.pixel(120)
			height: Draw.pixel(140)
			anchors.horizontalCenter: parent.horizontalCenter

			MouseArea {
				anchors.fill: parent
				onClicked: {
					panels.setVisible(characters);
					controller.state = "menuin";
				}
			}

			onPaint: {
				var ctx = portrait.getContext("2d");

				Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height, true);
				Draw.bevelRect(ctx, 0, 0, portrait.width, portrait.height, menu.lineWidth, menu.portraitBevel, "#777", portraitData, true);
				Draw.exit(ctx);
			}
		}

		MenuItem {
			menuPanel: characters
		}

		MenuItem {
			menuPanel: characteristics
		}

		MenuItem {
			menuPanel: description
		}

		MenuItem {
			menuPanel: skills
		}

		MenuItem {
			menuPanel: experience
		}

		MenuItem {
			menuPanel: weapons
		}

		MenuItem {
			menuPanel: inventory
		}
	}

	Component.onCompleted: {
		characterData.portraitChanged.connect(characterPortraitChanged);
		// Like the pictures in the character list, this does not want to
		// update correctly!
		characterData.imageProviderCountChanged.connect(characterPortraitChanged);
	}

	function characterPortraitChanged() {
		portrait.requestPaint();
	}
}
