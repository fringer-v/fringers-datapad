import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: obligationLine
	width: parent.width
	height: obligationRow.height

	property string obligLineType: ""
	property int obligMargin: 20
	property real obligWidth: obligationLine.width - obligMargin * 2

	Column {
		id: obligationRow

		Vspacer { size: index == 0 ? 0 : panels.lineSpace - 4 } // -4 because we align top lines of box

		Row {
			Hspacer { size: obligMargin }

			Obligation {
				width: obligWidth
				obligLeft: true
				obligType: obligLineType
				obligKey: key
				obligName: name
				obligSize: size
				obligNotes: notes
			}
		}
	}
}
