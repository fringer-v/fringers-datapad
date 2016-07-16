import QtQuick 2.0
import "../js/drawing.js" as Draw

Rectangle {
	id: erps
	width: imageWidth * 5 + spacer * 4
	height: 57
	
	property int imageWidth: height * 129 / 96
	property int spacer: 10

	Row {
		Image {
			width: imageWidth
			height: erps.height
			source: "qrc:/images/erp.png"
		}

		Hspacer { size: spacer }

		Image {
			width: imageWidth
			height: erps.height
			source: "qrc:/images/erp.png"
		}

		Hspacer { size: spacer }

		Image {
			width: imageWidth
			height: erps.height
			source: "qrc:/images/erp.png"
		}

		Hspacer { size: spacer }

		Image {
			width: imageWidth
			height: erps.height
			source: "qrc:/images/erp.png"
		}

		Hspacer { size: spacer }

		Image {
			width: imageWidth
			height: erps.height
			source: "qrc:/images/erp.png"
		}
	}
}
