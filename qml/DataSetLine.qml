import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: dataset
	width: parent.width
	height: topMargin + itemHeight
	color:  "white"

	property string dsName: name
	property string dsFile: file
	property string dsTime: time
	property bool dsSelected: characterData.dataSet === dsFile + "/" + dsName

	property int topMargin: index == 0 ? 0 : 2
	property int sideMargin: 4
	property int itemHeight: topLineHeight + 26 + 26
	property int partraitWidth: itemHeight - partraitFrame*2
	property int partraitFrame: 8
	property int topLineHeight: 26

	Image {
		source: dsSelected ? "qrc:/images/folder-icon-black.png" :  "qrc:/images/folder-icon-gray.png"
		width: partraitWidth
		height: partraitWidth
		x: sideMargin
		y: partraitFrame + topMargin
	}

	InfoLine {
		width: dataset.width - (partraitWidth + partraitFrame + sideMargin*2)
		height: topLineHeight
		x: sideMargin + partraitWidth + partraitFrame
		y: topMargin
		infoTitle: "FILE"
		infoText: dsFile
		infoType: "char-box,clear2"
		infoColor: dsSelected ? "black" : Constant.INFO_TITLE_GRAY
		infoBackground: "white"
	}

	InfoLine {
		width: dataset.width - (partraitWidth + partraitFrame + sideMargin*2)
		height: 26
		x: sideMargin + partraitWidth + partraitFrame
		y: topMargin + topLineHeight
		infoTitle: "NAME"
		infoText: dsName
		infoType: "char-box,clear"
		infoColor: dsSelected ? "black" : Constant.INFO_TITLE_GRAY
		infoBackground: "white"
	}

	InfoLine {
		width: dataset.width - (partraitWidth + partraitFrame + sideMargin*2)
		height: 26
		x: sideMargin + partraitWidth + partraitFrame
		y: topMargin + topLineHeight + 26
		infoTitle: "UPDATED"
		infoText: dsTime
		infoType: "char-box,clear"
		infoColor: dsSelected ? "black" : Constant.INFO_TITLE_GRAY
		infoBackground: "white"
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			characterData.dataSet = dsFile + "/" + dsName;
			gc();
		}
	}
}
