import QtQuick 2.3
import "../js/drawing.js" as Draw

// Experience, Special Features, Talents
Rectangle {
	id: panel

	property real weaponsSectionHeight: metaPanel.contentHeight / 3 * 2
	property real armorSectionHeight: metaPanel.contentHeight - weaponsSectionHeight

	Column {
		// WEAPONS
		Header {
			id: weaponsHeader
			width: metaPanel.contentWidth
			headText: "WEAPONS"
		}

		Vspacer { size: panels.lineSpace }

		WeaponsList {
			width: metaPanel.contentWidth
			height: weaponsSectionHeight - panels.lineSpace*2 - weaponsHeader.height
		}

		Vspacer { size: panels.lineSpace }

		// ARMOR
		Header {
			id: armorHeader
			width: metaPanel.contentWidth
			headText: "ARMOR"
		}

		Vspacer { size: panels.lineSpace }

		ArmorList {
			width: metaPanel.contentWidth
			height: armorSectionHeight - panels.lineSpace - armorHeader.height
		}
	}

	onVisibleChanged: {
		if (visible)
			characterData.showWeaponsAndArmor();
	}
}

