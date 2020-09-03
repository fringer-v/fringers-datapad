import QtQuick 2.3
import "../js/drawing.js" as Draw

Rectangle {
	id: stimpacks
	width: imageWidth * 5 + spacer * 4
	height: 57
	
	property int imageWidth: height * 129 / 96
	property int spacer: 10

	Row {
		Image {
			width: imageWidth
			height: stimpacks.height
			source: "qrc:/images/stimpack.png"
		}

		Hspacer { size: spacer }

		Image {
			width: imageWidth
			height: stimpacks.height
			source: "qrc:/images/stimpack.png"
		}

		Hspacer { size: spacer }

		Image {
			width: imageWidth
			height: stimpacks.height
			source: "qrc:/images/stimpack.png"
		}

		Hspacer { size: spacer }

		Image {
			width: imageWidth
			height: stimpacks.height
			source: "qrc:/images/stimpack.png"
		}

		Hspacer { size: spacer }

		Image {
			width: imageWidth
			height: stimpacks.height
			source: "qrc:/images/stimpack.png"
		}
	}
}
