import QtQuick 2.0
import "../js/drawing.js" as Draw

Rectangle {
	id: checker
	width: checklist.width - 20
	height: 60

	Row {
		TitleBox {
			height: checker.height
			width: checker.width - 400 - 20
			boxTitle: characterData.activeSkill.toUpperCase()
			boxText: characterData.dicePool
			boxTitleWidth: 240
			boxCornerHeight: 20
			displayDicePool: true
		}

		Hspacer { size: 10 }

		TitleBox {
			height: checker.height
			width: 200
			boxTitle: "MANEUVERS"
			boxText: characterData.itemManeuvers
			boxTitleWidth: 100
			boxCornerHeight: 20
			displayText: true
		}

		Hspacer { size: 10 }

		TitleBox {
			height: checker.height
			width: 200
			boxTitle: "STRAIN"
			boxText: characterData.itemStrain
			boxTitleWidth: 100
			boxCornerHeight: 20
			displayText: true
		}
	}
}
