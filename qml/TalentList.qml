import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util
import space.fringer.datapad 1.0

BaseList {
	id: talentList

	colTitles: [ "TALENT", "RANK", "ACTIVATION", "ACQUISITION", "DESCRIPTION" ]
	colWidths: [ 233, 41, 117, 155, 0 ]
	listDelegate: talentLine
	listModel: talentData
	listIndent: 2

	Component {
		id: talentLine
		TalentLine {
		}
	}

	DatTalentList {
		id: talentData
	}
}

