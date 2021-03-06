import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util
import space.fringer.datapad 1.0

BaseList {
	id: checkListList

	headerHeight: 0
	colWidths: [ 0 ]
	listDelegate: checkListLine
	listModel: checkListListData
	listIndent: 0
	fadeHeaderHeight: 8

	Component {
		id: checkListLine
		CheckListLine {
		}
	}

	DatCheckList {
		id: checkListListData
	}

	onAdd: {
		positionViewAtEnd();
	}
}
