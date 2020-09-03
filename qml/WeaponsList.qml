import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util
import space.fringer.datapad 1.0

BaseList {
	id: weaponsList

	colTitles: [ "WEAPON", "SKILL", "RANGE", "DAM", "CRIT", "DICE POOL", "HE/EQ", "+/-", "QUALITIES" ]
	colWidths: [ 240, 70, 88, 41, 41, 150, 48, 44, 0 ]
	listDelegate: weaponsLine
	listModel: weaponData
	listIndent: 2

	Component {
		id: weaponsLine
		WeaponsLine {
		}
	}

	DatWeaponsList {
		id: weaponData
	}
}
