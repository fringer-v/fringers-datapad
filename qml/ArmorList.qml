import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util
import space.fringer.datapad 1.0

BaseList {
	id: armorList

	colTitles: [ "ARMOR", "SOAK", "MDEF", "RDEF", "ENCUM", "HE/EQ", "+/-", "FEATURES" ]
	colWidths: [ 240, 50, 50, 50, 63, 48, 44, 0 ]
	listDelegate: armorLine
	listModel: armorData
	listIndent: 2

	Component {
		id: armorLine
		ArmorLine {
		}
	}

	DatArmorList {
		id: armorData
	}
}
