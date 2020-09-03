import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: currentdata
	width: parent.width
	height: topMargin + itemHeight
	color: "white"

	property int topMargin: index == 0 ? 0 : 8
	property int sideMargin: 4
	property int itemHeight: 52
	property int partraitWidth: itemHeight
	property int partraitFrame: 8
	property int topLineHeight: 23

	Image {
		source: "qrc:/images/anonymous-user.png"
		width: partraitWidth
		height: partraitWidth
		x: sideMargin
		y: topMargin
	}

	InfoLine {
		width: currentdata.width - (partraitWidth + partraitFrame + sideMargin*2)
		height: topLineHeight
		x: sideMargin + partraitWidth + partraitFrame
		y: topMargin
		infoTitle: "FILE"
		infoText: file
		infoType: "char-box,clear2"
		infoColor: Constant.INFO_TITLE_GRAY
		infoBackground: "white"
	}

	/*
	InfoLine {
		width: currentdata.width - (partraitWidth + partraitFrame + sideMargin*2)
		height: 26
		x: sideMargin + partraitWidth + partraitFrame
		y: topMargin + topLineHeight
		infoTitle: "NAME"
		infoText: name
		infoType: "char-box,clear"
		infoColor: Constant.INFO_TITLE_GRAY
		infoBackground: "white"
	}
	*/

	InfoLine {
		width: currentdata.width - (partraitWidth + partraitFrame + sideMargin*2)
		height: 26
		x: sideMargin + partraitWidth + partraitFrame
		y: topMargin + topLineHeight
		infoTitle: "UPDATED"
		infoText: time
		infoType: "char-box,clear"
		infoColor: Constant.INFO_TITLE_GRAY
		infoBackground: "white"
	}
}
