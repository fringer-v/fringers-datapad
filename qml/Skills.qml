import QtQuick 2.3
import "../js/drawing.js" as Draw
import space.fringer.datapad 1.0

Rectangle {
	id: panel

	property int skillListWidth: 493

	Column {
		id: topSection
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right

		// INFO
		InfoHeader {
		}

		Vspacer { size: panels.lineSpace }

		// SKILLS
		Header {
			width: metaPanel.contentWidth
			headText: "SKILLS"
		}

		Vspacer { size: panels.lineSpace }
	}

	Row {
		anchors.top: topSection.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		//SkillList {
		//	width: skillListWidth
		//	height: metaPanel.contentHeight - topSection.height + 4 // This is half of the fade area!
		//	skillType: "GENERAL SKILLS"
		//	skillData: generalSkills
		//}
		GeneralSkillList {
			width: skillListWidth
			height: metaPanel.contentHeight - topSection.height + 4 // This is half of the fade area!
		}

		Hspacer {
			// PMC - For some reason a slight overhang?! (so -1)
			size: metaPanel.contentWidth - skillListWidth*2
		}

		Column {
			SkillList {
				width: skillListWidth
				height: 25 + 147 + 4
				skillType: "COMBAT SKILLS"
				skillData: combatSkills
			}

			Vspacer {
				size: 12 - 4
			}

			SkillList {
				width: skillListWidth
				height: 25 + 171 + 4
				skillType: "KNOWLEDGE SKILLS"
				skillData: knowledgeSkills
			}

			Vspacer {
				size: 12 - 4
			}

			/*
			SkillList {
				width: skillListWidth
				height: 25 + 147 + 4
				skillType: "CUSTOM SKILLS"
				skillData: customSkills
				customSkills: true
			}
			*/

			SkillList {
				width: skillListWidth
				height: 25 + 147 + 4
				skillType: "SPECIAL SKILLS"
				skillData: specialSkills
			}
		}
	}

	//Component.onCompleted: {
	//}
	//DatGeneralSkillsList {
	//	id: generalSkills
	//}

	DatCombatSkillsList {
		id: combatSkills
	}

	DatKnowledgeSkillsList {
		id: knowledgeSkills
	}

	/*
	DatCustomSkillsList {
		id: customSkills
	}
	*/

	DatSpecialSkillsList {
		id: specialSkills
	}

	onVisibleChanged: {
		if (visible)
			characterData.showSkills();
	}
}

