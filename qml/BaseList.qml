import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util
import space.fringer.datapad 1.0

Rectangle {
	id: basicList
	width: 500

	property int headerHeight: Constant.LIST_HEADER_HEIGHT
	property string headerColor: Constant.LIST_HEADER_COLOR
	property int headerFontSize: 13
	property var colTitles: []
	property var colWidths: [] // 1 col width may be zero!
	property var listDelegate: undefined
	property var listModel: undefined
	property int listIndent: 4
	property int fadeHeaderHeight: 0
	property int fadeFooterHeight: 8

	signal add();

	function positionViewAtEnd()
	{
		list.positionViewAtEnd();
	}

	// Private:
	property var adjustedlColWidths: {
		var i;
		var tot = 0;

		for (i=0; i<colWidths.length; i++)
			tot += colWidths[i];

		var adjWidths = [];
		for (i=0; i<colWidths.length; i++) {
			if (colWidths[i] === 0)
				adjWidths[i] = basicList.width - listIndent*2 - tot;
			else
				adjWidths[i] = colWidths[i];
		}

		return adjWidths;
	}

	Rectangle {
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: headerHeight
		color: headerColor
	}

	ListView {
		id: list
		anchors.fill: parent
		clip: true

		model: listModel
		delegate: listDelegate
		header: headerFadeSpace
		footer: footerFadeSpace
	}

	Component {
		id: headerFadeSpace

		Rectangle {
			width: list.width
			height: headerHeight + fadeHeaderHeight
			color: "transparent"
		}
	}

	Component {
		id: footerFadeSpace

		Rectangle {
			width: list.width
			height: fadeFooterHeight
			color: "transparent"
		}
	}

	ListHeader {
		id: header
		visible: headerHeight > 0
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: headerHeight
		headerIndent: listIndent
		headerFontSize: headerFontSize
		backgroundColor: headerColor
		titles: colTitles
		widths: adjustedlColWidths
	}

	Rectangle {
		anchors.top: header.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		height: fadeHeaderHeight
		gradient: Gradient {
			GradientStop { position: 0.0; color: "#FFFFFFFF" }
			GradientStop { position: 1.0; color: "#00FFFFFF" }
		}
	}

	Rectangle {
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		height: fadeFooterHeight
		gradient: Gradient {
			GradientStop { position: 0.0; color: "#00FFFFFF" }
			GradientStop { position: 1.0; color: "#FFFFFFFF" }
		}
	}

	Component.onCompleted: {
		list.countChanged.connect(add);
	}
}
