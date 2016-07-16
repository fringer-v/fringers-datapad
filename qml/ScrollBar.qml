import QtQuick 2.0
import "../js/drawing.js" as Draw

Item {
	id: scrollBar

	// The properties that define the scrollbar's state.
	// position and pageSize are in the range 0.0 - 1.0.  They are relative to the
	// height of the page, i.e. a pageSize of 0.5 means that you can see 50%
	// of the height of the view.
	// orientation can be either Qt.Vertical or Qt.Horizontal
	property real position
	property real pageSize
	property variant orientation : Qt.Vertical

	// A light, semi-transparent background
	Rectangle {
		id: background
		anchors.fill: parent
		radius: orientation == Qt.Vertical ? (width/2 - 1) : (height/2 - 1)
		color: "black"
		opacity: 0.3
	}

	// Size the bar to the required size, depending upon the orientation.
	Rectangle {
		property real page: Math.min(1.0, scrollBar.pageSize)
		property real fullSize: scrollBar.position < 0 ? 1.0 - scrollBar.position : Math.max(1.0, scrollBar.position + page)
		property real actualSize: page / fullSize;
		property real actualPos: Math.max(0.0, scrollBar.position) / fullSize

		x: orientation == Qt.Vertical ? 1 : (actualPos * (scrollBar.width-2) + 1)
		y: orientation == Qt.Vertical ? (actualPos * (scrollBar.height-2) + 1) : 1
		width: orientation == Qt.Vertical ? (parent.width-2) : (actualSize * (scrollBar.width-2))
		height: orientation == Qt.Vertical ? (actualSize * (scrollBar.height-2)) : (parent.height-2)
		radius: orientation == Qt.Vertical ? (width/2 - 1) : (height/2 - 1)
		color: "black"
		opacity: 0.8
	}
}
