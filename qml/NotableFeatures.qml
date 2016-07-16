import QtQuick 2.3
import "../js/drawing.js" as Draw

Rectangle {
	id: chararacterNotes
	height: 239
	width: 544
	property int horizontalMargin: 4
	property int verticalMargin: 10

	property string featuresNotes: characterData.features
	property string backgroundNotes: characterData.story

	onFeaturesNotesChanged: chararacterNotesCanvas.requestPaint()
	onBackgroundNotesChanged: chararacterNotesCanvas.requestPaint()

	NotesArea {
		width: chararacterNotes.width-verticalMargin*2
		height: chararacterNotes.height-31-10-horizontalMargin*2
		x: verticalMargin
		y: 31-2+horizontalMargin
		text: featuresNotes + (featuresNotes.length > 0 ? "[P]" : "") + backgroundNotes
	}

	Canvas {
		id: chararacterNotesCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = chararacterNotesCanvas.getContext("2d");

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);

			Draw.bevelRect(ctx, 0, 0, chararacterNotes.width, chararacterNotes.height, 1.5, 20, "black");
			Draw.bevelRect(ctx, 14, 6, chararacterNotes.width-28, 25, 1.5, 9, "black", "white");

			var title = "";

			if (featuresNotes.length > 0 || backgroundNotes.length == 0)
				title = "NOTABLE FEATURES";
			if (backgroundNotes.length > 0) {
				if (title.length > 0)
					title += "/";
				title += "BACKGROUND STORY";
			}
			Draw.textBox(ctx, title, 16, 6, chararacterNotes.width-32, 25, "bold 15px Arial", "black", 2, "center", "middle");

			Draw.exit(ctx);
		}
	}
}
