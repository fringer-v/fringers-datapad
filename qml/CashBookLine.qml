import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw

Rectangle {
	id: cashbook
	height:  {
		var desc_heigth = desc_text.height;
		if (type == 5)
			desc_heigth = add_button_rect.height;
		return (desc_heigth + 5) < minimumHeight ? minimumHeight : desc_heigth + 5;
	}
	width: parent.width

	property int minimumHeight: 25
	property int amountWidth: 110
	property int spacer: 10
	property int fontSize: 14

	Column {
		Row {
			visible: type != 5
			FlowText {
				id: desc_text
				width: cashbook.width - amountWidth - spacer
				pixelSize: fontSize
				font: "Comic Sans MS"
				anchors.verticalCenter: parent.verticalCenter
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

			Hspacer { height: cashbook.height-1; size: spacer }

			Text {
				id: amount_text
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
			visible: type != 5
			Rectangle {
				height: 1
				width: desc_text.width
				color: "darkgray"
			}

			Hspacer { size: spacer }

			Rectangle {
				height: type === 1 ? 2 : 1
				width: amount_text.width
				color: type === 1 || type === 2 ? "black" : "darkgray"
			}
		}

		Row {
			visible: type == 5
			Rectangle {
				id: add_button_rect
				width: cashbook.width
				height: add_button.height + 10

				Button {
					id: add_button
					y: 10
					width: add_text.width + 10
					anchors.verticalCenter: parent.verticalCenter

					Text {
						id: add_text
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
						font.pixelSize: 16
						font.family: "Comic Sans MS"
						text: description
					}
				}
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
