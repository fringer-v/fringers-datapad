import QtQuick 2.0
import "../js/drawing.js" as Draw

Rectangle {
	id: motivationLine
	width: parent.width
	height: motivationRow.height

	property string motivationNotes: notes
	property string motivation: name1
	property string specificMotivation: name2

	property int horizontalMargin: 3
	property int verticalMargin: 9

	onMotivationNotesChanged: motivationsCanvas.requestPaint()
	onMotivationChanged: motivationsCanvas.requestPaint()
	onSpecificMotivationChanged: motivationsCanvas.requestPaint()

	Column {
		id: motivationRow

		Vspacer { size: index == 0 ? 0 : panels.lineSpace }

		Rectangle {
			width: motivationLine.width
			height: 143

			Rectangle {
				id: motivations
				width: 471
				height: parent.height
				anchors.horizontalCenter: parent.horizontalCenter

				NotesArea {
					width: motivations.width-verticalMargin*2
					height: motivations.height-28-10-horizontalMargin*2
					x: verticalMargin
					y: 28-2+horizontalMargin
					text: motivationNotes
				}

				Canvas {
					id: motivationsCanvas
					anchors.fill: parent

					onPaint: {
						var ctx = motivationsCanvas.getContext("2d");

						Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);

						Draw.bevelRect(ctx, 0, 0, motivations.width, motivations.height, 1.5, 20, "black");
						Draw.pointyRect(ctx, 13, 7, 210, 21, 1.5, 10, "black", "white");
						Draw.textBox(ctx, motivation.toUpperCase(), 13, 7, 210, 21, "bold 15px Arial", "black", 2, "center", "middle");
						Draw.pointyRect(ctx, motivations.width-13-210, 7, 210, 21, 1.5, 10, "black", "white");
						Draw.textBox(ctx, specificMotivation.toUpperCase(), motivations.width-13-210, 7, 211, 21, "bold 15px Arial", "black", 2, "center", "middle");

						Draw.exit(ctx);
					}
				}
			}
		}
	}
}

