import QtQuick 2.3
import "../js/drawing.js" as Draw

Row {
	Hspacer { size: Draw.pixel(10) }

	Column {
		Row {
			InfoLine {
				width: (metaPanel.contentWidth - Draw.pixel(20)) / 2 + Draw.pixel(20)
				infoTitle: "CHARACTER NAME:"
				infoText: characterData.name
				infoType: "large-font,clear"
			}

			InfoLine {
				width: (metaPanel.contentWidth - Draw.pixel(20)) / 2 - Draw.pixel(20)
				infoTitle: "PLAYER NAME:"
				infoText: characterData.player
				infoType: "right,wide-row,clear"
			}
		}

		Row {
			InfoLine {
				width: (metaPanel.contentWidth - Draw.pixel(20)) / 2 + Draw.pixel(20)
				infoTitle: "SPECIES:"
				infoText: characterData.species
				infoType: "clear"
			}

			InfoLine {
				width: (metaPanel.contentWidth - Draw.pixel(20)) / 2 - Draw.pixel(20)
				infoTitle: "CAREER:"
				infoText: characterData.career
				infoType: "right,clear"
			}
		}

		InfoLine {
			width: metaPanel.contentWidth - Draw.pixel(20)
			infoTitle: "SPECIALIZATIONS:"
			infoText: characterData.specializations
			infoType: "clear"
		}
	}
}
