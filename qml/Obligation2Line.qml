import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: obligation2Line
	width: parent.width
	height: obligationRow.height

	property int obligMidSpace: 30
	property real obligWidth: (obligation2Line.width - obligMidSpace) / 2

	Column {
		id: obligationRow

		Vspacer { size: index == 0 ? 0 : panels.lineSpace }

		Row {
			Obligation {
				width: obligWidth
				obligLeft: true
				obligName: name_0
				obligSize: size_0
				obligNotes: notes_0
			}

			Hspacer { size: obligMidSpace }

			Obligation {
				width: obligWidth
				visible: name_1 != undefined
				obligLeft: false
				obligName: name_1 == undefined ? "" : name_1
				obligSize: size_1 == undefined ? "" : size_1
				obligNotes: notes_1 == undefined ? "" : notes_1
			}
		}
	}
}
