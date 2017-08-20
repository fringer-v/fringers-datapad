import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util
import space.fringer.datapad 1.0

BaseList {
	id: gearList

	colTitles: [ "GEAR", "ENCUM", "HE/EQ", "+/-", "SPECIAL" ]
	colWidths: [ 268, 64, 48, 44, 0 ]
	listDelegate: gearLine
	listModel: gearData

	Component {
		id: gearLine

		GearLine {
		}
	}

	DatGearList {
		id: gearData
	}
}
