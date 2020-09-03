import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import space.fringer.datapad 1.0

Rectangle {
	id: speciesFeatures
	width: parent.width

	property real fadeArea: 10

	Canvas {
		id: speciesFeaturesCanvas
		anchors.fill: parent

		onPaint: {
			var ctx = speciesFeaturesCanvas.getContext("2d");

			Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
			Draw.cornerRect(ctx, 0, 0, 61, 52, 6, Constant.DARK_GRAY, "top-left,top-right");
			Draw.cornerRect(ctx, 0, speciesFeatures.height-52, 61, 52, 6, Constant.LIGHT_GRAY, "bottom-left,bottom-right");
			Draw.bevelRect(ctx, 16, 4, speciesFeatures.width-16, speciesFeatures.height-8, 1.5, 20, "black", "white");
			Draw.exit(ctx);
		}
	}

	ListView {
		id: list
		width: speciesFeatures.width - 26 - 10
		height: speciesFeatures.height - 26 - 26 + fadeArea*2
		x: 26
		y: 26 - fadeArea
		clip: true

		model: speciesData
		delegate: SpeciesLine { }
		header: fadeSpace
		footer: fadeSpace
	}

	Rectangle {
		width: list.width
		height: fadeArea
		anchors.top: list.top
		anchors.left: list.left
		gradient: Gradient {
			GradientStop { position: 0.0; color: "#FFFFFFFF" }
			GradientStop { position: 1.0; color: "#00FFFFFF" }
		}
	}

	Rectangle {
		width: list.width
		height: fadeArea
		anchors.bottom: list.bottom
		anchors.left: list.left
		gradient: Gradient {
			GradientStop { position: 0.0; color: "#00FFFFFF" }
			GradientStop { position: 1.0; color: "#FFFFFFFF" }
		}
	}

	Component {
		id: fadeSpace

		Rectangle {
			width: list.width
			height: fadeArea
			color: "white"
		}
	}

	DatSpeciesFeaturesList {
		id: speciesData
	}
}
