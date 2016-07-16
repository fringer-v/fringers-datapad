import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: character
	width: 200
	height: Constant.CHARACTER_HEIGHT // 100

	property string charName: ""
	property string charFile: ""
	property string charPlayer: ""
	property string charImage: ""
	property string charTime: ""
	property bool charSelected: false

	property string charLineColor: charFile.length == 0 ? "lightgray" : "black"
	property int partraitWidth: character.height * 0.6
	property int partraitFrame: 16

	property int imageProviderCount: characterData.imageProviderCount

	Image {
		id: portrait
		visible: false;
		asynchronous: false
		source: {
			if (charImage.length > 0)
				return "image://datapad-images/"+charImage;
			return "qrc:/images/anonymous-user.png"
		}
	}

	Canvas {
		id: characterCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = characterCanvas.getContext("2d");
			var lineSize = 1.5;

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.bevelRect(ctx, 0, 0, character.width, character.height, lineSize, 13, charLineColor, charSelected ? Constant.LIGHT_GRAY : "white");
			Draw.drawBox(ctx, partraitFrame, partraitFrame, partraitWidth, character.height-partraitFrame*2, 2, charLineColor, portrait);
			Draw.exit(ctx);
		}
	}

	onCharSelectedChanged: characterCanvas.requestPaint()

	onCharImageChanged: characterCanvas.requestPaint()

	InfoLine {
		id: line1
		width: character.width - (partraitWidth + partraitFrame*3)
		height: 40
		x: partraitWidth + partraitFrame*2
		y: 0
		infoTitle: "FILE"
		infoText: charFile
		infoType: "char-box,clear2"
		infoColor: charFile.length == 0 ? "lightgray" : "black"
		infoBackground: charSelected ? Constant.LIGHT_GRAY : "white"
	}

	InfoLine {
		id: line2
		width: character.width - (partraitWidth + partraitFrame*3)
		height: 30
		x: partraitWidth + partraitFrame*2
		y: 40
		infoTitle: "NAME"
		infoText: charName
		infoType: "char-box,clear"
		infoColor: charLineColor
		infoBackground: charSelected ? Constant.LIGHT_GRAY : "white"
	}

	InfoLine {
		id: line3
		width: character.width - (partraitWidth + partraitFrame*3)
		height: 30
		x: partraitWidth + partraitFrame*2
		y: 70
		infoTitle: "PLAYER"
		infoText: charPlayer
		infoType: "char-box,clear"
		infoColor: charFile.length == 0 ? "lightgray" : "black"
		infoBackground: charSelected ? Constant.LIGHT_GRAY : "white"
	}

	InfoLine {
		id: line4
		width: character.width - (partraitWidth + partraitFrame*3)
		height: 30
		x: partraitWidth + partraitFrame*2
		y: 100
		infoTitle: "UPDATED"
		infoText: charTime
		infoType: "char-box,clear"
		infoColor: charFile.length == 0 ? "lightgray" : "black"
		infoBackground: charSelected ? Constant.LIGHT_GRAY : "white"
	}

	onImageProviderCountChanged: {
		// Seems like an overkill, but I just cannot get this list to
		// update and display EVERYTHING correctly if I dont redraw
		// all the time!
		line1.redraw();
		line2.redraw();
		line3.redraw();
		line4.redraw();
		characterCanvas.requestPaint();
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			characterData.file = charFile;
			gc();
		}
	}
}
