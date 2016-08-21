import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw

Rectangle {
	id: shoplistline
	height: 25
	width: parent.width
	property int spacer: 10
	property int fontSize: 14

	Column {
		Row {
			Text {
				width: shoplistline.width - credits.width - spacer
				font.pixelSize: fontSize
				font.family: "Arial"
				clip: true
				text: name
			}

			Hspacer { size: spacer }

			Text {
				id: credits
				width: 60
				font.pixelSize: fontSize
				font.family: "Arial"
				clip: true
				text: price + " CR"
			}
		}

		Vspacer { size: 2 }

		Rectangle {
			height: 1
			width: shoplistline.width
			color: "darkgray"
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			var amount_correct = datapadItemDialog.amountCorrect();

			itemItemKey = key;
			datapadItemBoxItem.infoText = name;
			datapadItemBoxPrice.infoText = price + " CR";
			datapadItemBoxRarity.infoText = rarity;

			if (datapadItemDialogNumber.boxText === "")
				datapadItemDialogNumber.boxText = 1;
			if (amount_correct)
				datapadItemDialogAmount.boxText = Math.abs(parseInt(datapadItemDialogNumber.boxText) * price);
			searchTextField.focus = false;
			datapadShopList.visible = false;
		}
	}
}
