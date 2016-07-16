import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw

Rectangle {
	id: cashbook
	height: 22
	width: parent.width
	property int amountWidth: 110
	property int spacer: 10
	property int fontSize: 14

	Column {
		Row {
			FlowText {
				id: descText
				width: cashbook.width - amountWidth - spacer
				height: cashbook.height - 1
				pixelSize: fontSize
				font: "Comic Sans MS"
				text: {
					var t;
					var c;

					if (key == undefined || description == undefined ||
						type == undefined)
						return "";

					if (key.length === 0)
						return description;

					if (type === 4) // ITEM_ORIG_STOCK
						t = "[B]Original Stock:[b] ";
					else {
						if (count < 0) {
							if (amount > 0)
								t = "[B]Sold:[b] ";
							else
								t = "[B]Removed:[b] ";
						}
						else {
							if (amount < 0)
								t = "[B]Purchased:[b] ";
							else
								t = "[B]Added:[b] ";
						}
					}
					c = count < 0 ? -count : count;
					if (c !== 1)
						t += c + "x ";
					else if (type === 4) // ITEM_ORIG_STOCK
						t += "1x ";

					t += item;
					if (description.length !== 0)
						t += " (" + description + ")";
					return t;
				}
			}

			Hspacer { size: spacer }

			Text {
				id: amountText
				width: amountWidth
				height: cashbook.height-1
				font.pixelSize: fontSize
				font.family: "Comic Sans MS"
				font.bold: true
				horizontalAlignment: Text.AlignRight
				verticalAlignment: Text.AlignVCenter
				text: amount == undefined ? "" : (amount != 0 ? (((type === 3 && amount > 0) ? "+" + amount : amount) + " CR") : "")
			}
		}

		Row {
			Rectangle {
				height: 1
				width: descText.width
				color: "darkgray"
			}

			Hspacer { size: spacer }

			Rectangle {
				height: type === 1 ? 2 : 1
				width: amountText.width
				color: type === 1 || type === 2 ? "black" : "darkgray"
			}
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			if (type === 3 || (type === 4 && keycount <= 1)) // ITEM_AMOUNT || ITEM_ORIG_STOCK
				datapadItemDialog.editDataPadLine(ref, key, type, count, amount, description, item, price, rarity);
			else if (type == 5) // ITEM_ADD_ITEM
				datapadItemDialog.newDatapadItem("", 5, ""); // ITEM_ADD_ITEM
		}
	}
}
