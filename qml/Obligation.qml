import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: obligation
	width: parent.width
	height: Constant.OBLIGATION_HEIGHT // 100

	property string obligType: ""
	property string obligKey: ""
	property string obligName: ""
	property string obligSize: "0"
	property string obligNotes: ""
	property bool obligLeft: true
	property int diameter: obligation.height * 0.8
	property int obligCornerHeight: obligation.height * 0.4
	property string obligLineColor: obligName.length == 0 ? "lightgray" : "black"

	Canvas {
		id: obligationCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = obligationCanvas.getContext("2d");
			var lineSize = 1.5;
			var cornerBoxH = obligCornerHeight;
			var val = parseInt(obligSize);
			var text = obligSize;

			if (val > 100)
				text = val % 100;
			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.cornerRect(ctx, obligLeft ? 0 : obligation.width-32, 0, 32, cornerBoxH, 5, "#DFDFDF", obligLeft ? "top-left" : "top-right");
			Draw.cornerRect(ctx, obligLeft ? 0 : obligation.width-32, obligation.height-cornerBoxH, 32, cornerBoxH, 5, "#E1E1E1", obligLeft ? "bottom-left" : "bottom-right");
			Draw.bevelRect(ctx, obligLeft ? 6 : 0, 3, obligation.width-6, obligation.height-6, lineSize, 13, obligLineColor, "white");

			Draw.drawCircle(ctx, ((obligation.height-diameter)/2)+3 - (obligLeft ? 0 : 6), (obligation.height-diameter)/2, diameter, 5, obligLineColor, "white");
			Draw.drawCircle(ctx, ((obligation.height-diameter)/2)+3+7.5 - (obligLeft ? 0 : 6), (obligation.height-diameter)/2+7.5, diameter-15, 1.6, obligLineColor, "white");
			Draw.drawCircle(ctx, ((obligation.height-diameter)/2)+3+12 - (obligLeft ? 0 : 6), (obligation.height-diameter)/2+12, diameter-24, 0.9, obligLineColor, "white");
			Draw.textBox(ctx, text, ((obligation.height-diameter)/2)+3 - (obligLeft ? 0 : 6), (obligation.height-diameter)/2, diameter, diameter, "bold 24px Verdana", obligLineColor, 3);

			Draw.exit(ctx);
		}
	}

	onObligSizeChanged: {
		obligationCanvas.requestPaint();
	}

	InfoLine {
		width: obligation.width - obligation.height - (obligation.height-diameter)/2
		height: 30
		x: obligation.height - (obligLeft ? 0 : 6)
		y: 0
		infoTitle: {
			var val = parseInt(obligSize);
			var rank = "";

			if (val > 100)
				rank = " (Rank: "+(val/100)+")";

			return obligName.toUpperCase() + rank;
		}
		infoType: "obligation,clear2"
		infoColor: obligName.length == 0 ? "lightgray" : "black"
	}

	NotesArea {
		visible: obligNotes.length > 0
		width: obligation.width - obligation.height - (obligation.height-diameter)/2
		height: obligation.height - 30 - 8
		x: obligation.height - (obligLeft ? 0 : 6)
		y: 30
		text: obligNotes
	}

	MouseArea {
		width: obligation.height
		height: obligation.height
		x: 0
		y: 0

		onClicked: {
			experienceDialog.experienceDialogRef = 0;
			experienceDialog.experienceDialogType = obligType;
			experienceDialog.experienceDialogKey = obligKey;
			experienceDialog.experienceDialogName = obligName;
			experienceDialog.experienceDialogDesc = "";
			experienceDialog.experienceDialogBase = obligSize;
			experienceDialog.experienceDialogValue = 0;
			experienceDialog.experienceDialogMayDelete = false;
			experienceDialog.visible = true;
		}
	}
}

