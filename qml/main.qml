import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.3
import space.fringer.datapad 1.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Window {
	id: screen
	visible: true
	width: Constant.SCREEN_WIDTH
	height: Constant.SCREEN_HEIGHT

	property bool landscape: {
		var v;

		v = width > height;
		return v;
	}
	property bool loadError: true

	DatCurrentDataList {
		id: currentDataList
	}

	DatDataSetList {
		id: dataSetList
	}

	DatCharacterList {
		id: characterList
	}

	DatShopList {
		id: shopListData
	}

	Character {
		id: characterData

		onAlert: {
			alertDialog.title = title;
			alertDialog.message = message;
			alertDialog.visible = true;
			loadError = true;
		}

		onLoadingChanged: {
			if (!loading && !loadError) {
				if (controllerLoader.item !== null)
					controllerLoader.item.gotoPreferences();
			}
		}
	}

	//MessageDialog {
	//	id: alertDialog
	//	width: 360
	//	height: 200
	//}
	property bool isIOS: characterData.isIOS

	FontLoader { id: comicRegular; source: "qrc:/etc/Comic-Sans-MS.ttf" }
	FontLoader { id: comicBold; source: "qrc:/etc/Comic-Sans-MS-Bold.ttf" }
	FontLoader { id: eoteRegular; source: "qrc:/etc/EotE_Symbol-Regular_v1.otf" }

	Image {
		id: portraitData
		visible: false;
		source: {
			if (characterData.portrait.length > 0)
				return "image://datapad-images/"+characterData.portrait;
			return "qrc:/images/anonymous-user.png"
		}
	}

	Item {
		id: splashScreen
		x: 0
		y: isIOS ? - Constant.STATUS_BAR_HEIGHT : 0
		width: parent.width
		height: parent.height + (isIOS ? Constant.STATUS_BAR_HEIGHT : 0)

		Image {
			id: splashScreenImage
			anchors.fill: parent
			source: "qrc:/images/nightsky.png"
		}

		Rectangle {
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
			width: subtitle.width
			height: welcome.height + subtitle.height
			color: "transparent"

			Column {
				Text {
					id: welcome
					anchors.horizontalCenter: parent.horizontalCenter
					font.pixelSize: 40
					font.family: "Arial"
					color: "white"
					text: "Welcome to Fringer's Datapad"
				}

				Vspacer { size: 20 }

				Text {
					id: subtitle
					anchors.horizontalCenter: parent.horizontalCenter
					font.pixelSize: 24
					font.family: "Arial"
					color: "white"
					text: "a Character Sheet for the Star Wars\u2122 EotE, AoR and F&D Role Playing Games"
				}
			}
		}
	}

	// Simulated Status Bar for "normal" app:
	Rectangle {
		id: statusBar
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: isIOS ? 0 : Constant.STATUS_BAR_HEIGHT
		color: "black"
	}

	Flickable {
		id: scrollArea
		anchors.top: statusBar.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		contentWidth: landscape ? Constant.SCREEN_WIDTH : Constant.SCREEN_HEIGHT
		contentHeight: landscape ? Constant.SCREEN_HEIGHT - Constant.STATUS_BAR_HEIGHT : Constant.SCREEN_WIDTH
		boundsBehavior: Flickable.StopAtBounds

		Loader {
			id: controllerLoader
			width: Constant.SCREEN_WIDTH
			height: landscape ? Constant.SCREEN_HEIGHT - Constant.STATUS_BAR_HEIGHT : Constant.SCREEN_HEIGHT

			onLoaded: {
				splashScreen.visible = false;
				splashScreenImage.source = "";
			}
		}
	}

	Alert {
		id: alertDialog
		visible: false
	}

	Timer {
		id: loadTimer
		interval: 500

		onTriggered: {
			characterData.init();
			controllerLoader.source = "Controller.qml";
		}
	}

	Component.onCompleted: {
		//screen.allowedOrientation = Screen.Landscape | Screen.LandscapeInverted;
		loadTimer.start();
	}

	onHeightChanged: setOrientation();
	onWidthChanged: setOrientation();

	function setOrientation()
	{
		var local_landscape = width > height;
		if (local_landscape) {
			controllerLoader.transform = [ norot, notra ];
			scrollArea.contentY = 0;
		}
		else {
			controllerLoader.transform = [ rotme, trame ];
			scrollArea.contentY = Constant.STATUS_BAR_HEIGHT;
		}
	}

	Rotation { id: norot; origin.x: 0; origin.y: 0; angle: 0 }
	Translate { id: notra; x: 0; y: 0 }
	Rotation { id: rotme; angle: 270 }
	Translate { id: trame; x: 0; y: 1024 }
}
