import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util
import space.fringer.datapad 1.0

BaseList {
	id: injuryList

	headerHeight: 0
	colWidths: [ 0 ]
	listDelegate: injuryLine
	listModel: injuryData
	listIndent: 0

	Component {
		id: injuryLine
		InjuryLine {
		}
	}

	DatInjuryList {
		id: injuryData
	}
}
