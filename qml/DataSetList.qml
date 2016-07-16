import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util
import space.fringer.datapad 1.0

BaseList {
	id: datasets

	headerHeight: 0
	colWidths: [ 0 ]
	listDelegate: dataSetLine
	listModel: dataSetList
	listIndent: 0
	fadeHeaderHeight: 8

	Component {
		id: dataSetLine
		DataSetLine {
		}
	}
}
