import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: characterLine
	width: parent.width
	height: characterRow.height

	property int charMidSpace: 44
	property real charWidth: (characterLine.width - charMidSpace) / 2

	Column {
		id: characterRow

		Vspacer { size: index == 0 ? 0 : panels.lineSpace }

		Row {
			CharacterBox {
				width: charWidth
				charName: name_0
				charFile: file_0
				charPlayer: player_0
				charImage: portrait_0
				charTime: time_0
				charSelected: characterData.file.trim().length > 0 && characterData.file === file_0
			}

			Hspacer { size: charMidSpace }

			CharacterBox {
				visible: name_1 !== undefined
				width: charWidth
				charName: name_1 !== undefined ? name_1 : ""
				charFile: file_1 !== undefined ? file_1 : ""
				charPlayer: player_1 !== undefined ? player_1 : ""
				charImage: portrait_1 !== undefined ? portrait_1 : ""
				charTime: time_1 !== undefined ? time_1 : ""
				charSelected: characterData.file.trim().length > 0 && characterData.file === file_1
			}
		}
	}
}
