import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

// Experience, Special Features, Talents
Rectangle {
	id: panel

	property int topSpacer: 40
	property int topSectionHeight: 235
	property int topSectionWidth: (metaPanel.contentWidth - topSpacer) / 2

	Column {
		Row {
			Column {
				id: xp
				width: topSectionWidth

				Header {
					width: xp.width
					headText: "EXPERIENCE"
				}

				Vspacer { size: panels.lineSpace }

				Experience {
					height: topSectionHeight - panels.lineSpace - Constant.HEADER_HEIGHT
				}
			}

			Hspacer { size: topSpacer }

			Column {
				id: species
				width: topSectionWidth

				Header {
					width: species.width
					headText: "SPECIES FEATURES"
				}

				Vspacer { size: panels.lineSpace }

				SpeciesFeatures {
					height: topSectionHeight - panels.lineSpace - Constant.HEADER_HEIGHT
				}
			}
		}

		Vspacer { size: panels.lineSpace }

		Header {
			width: metaPanel.contentWidth
			headText: "TALENTS"
		}

		Vspacer { size: panels.lineSpace }

		TalentList {
			width: metaPanel.contentWidth
			height: metaPanel.contentHeight - topSectionHeight - panels.lineSpace*2 - Constant.HEADER_HEIGHT
		}
	}

	onVisibleChanged: {
		if (visible)
			characterData.showTalents();
	}
}

