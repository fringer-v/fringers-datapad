import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: moralevel
	width: 320
	height: 22

	property string moralityText: {
		if (characterData.force === 0)
			return "Not force sensitive"
		if (characterData.morality > 70)
			return "Light-Side Paragon";
		if (characterData.morality < 30)
			return "Dark-Side Force User";
		return "Light-Side Force User";
	}

	Canvas {
		id: moralevelCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = moralevelCanvas.getContext("2d");

			Draw.enter(ctx);

			Draw.pointyRect(ctx, 0, 0, moralevelCanvas.width, moralevelCanvas.height, 1.5, 18, "black", "white");
			Draw.textBox(ctx, moralityText, 0, 0, moralevelCanvas.width, moralevelCanvas.height, "bold 14px Arial", "black", 1);

			Draw.exit(ctx);
		}
	}

	onMoralityTextChanged: {
		moralevelCanvas.requestPaint();
	}
}
