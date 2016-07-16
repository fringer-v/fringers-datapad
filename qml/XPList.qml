import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util
import space.fringer.datapad 1.0

BaseList {
	id: xpList

	headerHeight: 0
	colWidths: [ 0 ]
	listDelegate: xpLine
	listModel: xpData
	listIndent: 0

	Component {
		id: xpLine
		XPLine {
		}
	}

	DatXPList {
		id: xpData
	}

	onAdd: {
		positionViewAtEnd();
	}
}
