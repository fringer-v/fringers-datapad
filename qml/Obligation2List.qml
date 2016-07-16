import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util
import space.fringer.datapad 1.0

BaseList {
	id: obligationList

	headerHeight: 0
	colWidths: [ 0 ]
	listDelegate: obligation2Line
	listModel: obligatonData
	listIndent: 0
	fadeHeaderHeight: 8

	Component {
		id: obligation2Line
		Obligation2Line {
		}
	}

	DatObligation2List {
		id: obligatonData
	}
}
