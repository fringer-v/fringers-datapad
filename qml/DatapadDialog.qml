import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util

Rectangle {
	id: datapadItemDialog
	anchors.fill: parent
	color: "#00000000"
	visible: false

	property int itemRef: 0
	property string itemItemKey: ""
	property int itemType: 0

	property int margin: 5
	property real button_width: 60

	function updatePos(item_orig, item_dest) {
		var pos_abs = datapadItemDialog.mapFromItem (item_orig.parent, item_orig.x, item_orig.y);
		return datapadItemDialog.mapToItem (item_dest.parent, pos_abs.x, pos_abs.y);
	}

	function editDataPadLine(ref, itemkey, type, count, amount, desc, item, price, rarity)
	{
		itemRef = ref;
		itemItemKey = itemkey;
		itemType = type;
		if (count === 0) {
			datapadItemDialogNumber.boxText = "";
			datapadItemDialogAmount.boxText = amount === 0 ? "" : amount;
		}
		else {
			datapadItemDialogNumber.boxText = count;
			datapadItemDialogAmount.boxText = amount === 0 ? "" : Math.abs(amount);
		}
		datapadItemDialogDesc.boxText = desc;
		datapadItemBoxItem.infoText = item;
		datapadItemBoxPrice.infoText = price === 0 ? "" : price + " CR";
		datapadItemBoxRarity.infoText = rarity === 0 ? "" : rarity;
		searchTextField.text = "";
		datapadItemDialog.visible = true
		datapadItemDialogNumber.focus = true;
	}

	function newDatapadItem(itemkey, type, item)
	{
		itemRef = 0;
		itemItemKey = itemkey;
		itemType = type;
		datapadItemDialogNumber.boxText = "";
		datapadItemDialogAmount.boxText = "";
		datapadItemDialogDesc.boxText = "";
		datapadItemBoxItem.infoText = item;
		datapadItemBoxPrice.infoText = itemkey === "" ? "" : characterData.getPrice(itemkey) + " CR";
		datapadItemBoxRarity.infoText = itemkey === "" ? "" : characterData.getRarity(itemkey);
		searchTextField.text = "";
		datapadItemDialog.visible = true
		searchTextField.focus = true;
	}

	function closeDialog()
	{
		if (datapadItemDialog.visible)
			datapadItemDialog.visible = false;
	}

	function negate(val_str)
	{
		if (val_str === "" || val_str === "+")
			return "-";
		if (val_str === "-")
			return "";
		var val = parseInt(val_str);
		if (isNaN(val))
			return "";
		return -val;
	}

	function addto(val_str, add_str)
	{
		var val = parseInt(val_str);
		if (isNaN(val))
			val = 0;
		val = val + parseInt(add_str);
		if (isNaN(val) || val === 0)
			return "";
		return val;
	}

	function setAmount(perc_str)
	{
		if (datapadItemBoxPrice.infoText.trim() !== "") {
			if (datapadItemDialogNumber.boxText.trim() === "")
				datapadItemDialogNumber.boxText = 1;
			else if (datapadItemDialogNumber.boxText.trim() === "-")
				datapadItemDialogNumber.boxText = -1;

			var number = parseInt(datapadItemDialogNumber.boxText);
			var price = parseInt(Util.left(datapadItemBoxPrice.infoText.trim(), " "));
			var perc =  parseInt(Util.left(perc_str, "%"));
			var val = Math.round(Math.abs(number * price * perc / 100.0));

			if (isNaN(val))
				return;
			if (val === 0)
				val = "";
			datapadItemDialogAmount.boxText = val;
		}
	}

	function datapadClicked(button)
	{
		if (button === "Cancel") {
			datapadItemDialog.visible = false;
			Qt.inputMethod.hide();
		}
		else if (button === "Remove") {
			if (itemRef > 0)
				removeItemDialog.open();
			Qt.inputMethod.hide();
		}
		else if (button === "OK")
			datapadClickedOK();
		else if (button === "pm1")
			datapadItemDialogNumber.boxText = negate(datapadItemDialogNumber.boxText);
		else if (button === "pm2")
			datapadItemDialogAmount.boxText = negate(datapadItemDialogAmount.boxText);
		else if (Util.endsWith(button, "%"))
			setAmount(button);
		else
			datapadItemDialogNumber.boxText = addto(datapadItemDialogNumber.boxText, button);
	}

	function datapadClickedOK()
	{
		var count = 0;
		var amount = 0;

		datapadItemDialog.visible = false;

		if (datapadItemDialogNumber.boxText.trim().length !== 0) {
			count = parseInt(datapadItemDialogNumber.boxText);
			if (isNaN(count))
				count = 0;
		}
		if (datapadItemDialogAmount.boxText.trim().length !== 0) {
			amount = parseInt(datapadItemDialogAmount.boxText);
			if (isNaN(amount))
				amount = 0;
		}

		var item = itemItemKey.trim();
		var desc = datapadItemDialogDesc.boxText.trim();

		if (item.length !== 0 || desc.length !== 0) {
			if (itemRef > 0)
				characterData.updateInvItem(itemRef, count, desc, amount);
			else
				characterData.addInvItem(count, item, desc, amount);
		}
		Qt.inputMethod.hide();
	}

	function amountCorrect(number_text)
	{
		if (number_text === undefined)
			number_text = datapadItemDialogNumber.boxText.trim();
		if (number_text !== "" &&
			datapadItemDialogAmount.boxText.trim() !== "" &&
			datapadItemBoxPrice.infoText.trim() !== "") {
			var number = Math.abs(parseInt(number_text));
			var price = Math.abs(parseInt(Util.left(datapadItemBoxPrice.infoText.trim(), " ")));
			var amount = Math.abs(parseInt(datapadItemDialogAmount.boxText));
			var val;

			val = Math.round(number * price * 100.0 / 100.0);
			if (val === amount)
				return "100%";
			val = Math.round(number * price * 75.0 / 100.0);
			if (val === amount)
				return "75%";
			val = Math.round(number * price * 50.0 / 100.0);
			if (val === amount)
				return "50%";
			val = Math.round(number * price * 25.0 / 100.0);
			if (val === amount)
				return "25%";
		}
		return "";
	}

	MouseArea {
		anchors.fill: parent

		onClicked: {
			datapadItemDialog.visible = false;
			Qt.inputMethod.hide();
		}
	}

	Rectangle {
		id: datapadItemDialogBox
		width: button_width*8+margin*2*9
		height: margin*2 + datapadItemDialogNumber.height + margin*2 + button_row.height + margin*2 + itemBox.height + margin*2 +
			datapadItemDialogDesc.height + margin*2 + apadbutton.height + margin
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		color: "#7F000000"
		radius: 10

		MouseArea {
			anchors.fill: parent
			onClicked: searchTextField.focus = false;
		}

		Column {
			Vspacer { size: margin*2 }

			Row {
				Hspacer { size: margin*2 }

				PadButton {
					id: plus_minus1
					buttonWidth: button_width
					buttonHeigth: 40
					buttonBorder: 0
					buttonText: "+/-"
					onPressed:datapadClicked("pm1");
				}

				Hspacer { size: margin*2 }

				BoxEditLine {
					id: datapadItemDialogNumber
					width: (datapadItemDialogBox.width - margin*6)/2 - button_width - margin*2
					height: 40
					enabled: itemType === 3 || itemType === 5 // ITEM_AMOUNT || ITEM_ADD_ITEM
					placeHolderText: "Number"
					format: "integer"
					name: "number"

					property string prevNumber: ""

					onDonePressed: datapadClickedOK()
					onBoxTextChanged: {
						if (boxText.trim() === "")
							datapadItemDialogAmount.boxText = "";
						else if (datapadItemBoxPrice.infoText.trim() !== "") {
							var perc = amountCorrect(prevNumber);
							if (perc !== "")
								setAmount(perc);
						}
						prevNumber = boxText.trim();
					}
					onTabPressed: {
						datapadItemDialogAmount.focus = true;
					}
				}

				Hspacer { size: margin*2 }

				PadButton {
					id: plus_minus2
					buttonWidth: button_width
					buttonHeigth: 40
					buttonBorder: 0
					buttonText: "+/-"
					onPressed: datapadClicked("pm2")
				}

				Hspacer { size: margin*2 }

				BoxEditLine {
					id: datapadItemDialogAmount
					width: (datapadItemDialogBox.width - margin*6)/2 - button_width - margin*2
					height: 40
					enabled: itemType === 3 || itemType === 5 // ITEM_AMOUNT || ITEM_ADD_ITEM
					placeHolderText: "Price"
					format: itemItemKey === "" ? "integer" : "natural"
					name: "amount"

					onDonePressed: datapadClickedOK()
					onTabPressed: {
						if (searchTextField.enabled)
							searchTextField.focus = true;
						else
							datapadItemDialogDesc.focus = true;
					}
				}

				Hspacer { size: margin*2 }
			}

			Vspacer { size: margin*2 }

			Row {
				Hspacer { size: margin*2 }

				PadButton {
					id: button_row
					buttonWidth: button_width
					buttonHeigth: 40
					buttonBorder: 0
					buttonText: "+1"
					onPressed: datapadClicked(buttonText)
				}

				Hspacer { size: margin*2 }

				PadButton {
					id: button_row2
					buttonWidth: button_width
					buttonHeigth: 40
					buttonBorder: 0
					buttonText: "+2"
					onPressed: datapadClicked(buttonText)
				}

				Hspacer { size: margin*2 }

				PadButton {
					id: button_row3
					buttonWidth: button_width
					buttonHeigth: 40
					buttonBorder: 0
					buttonText: "-1"
					onPressed: datapadClicked(buttonText)
				}

				Hspacer { size: margin*2 }

				PadButton {
					id: button_row4
					buttonWidth: button_width
					buttonHeigth: 40
					buttonBorder: 0
					buttonText: "-2"
					onPressed: datapadClicked(buttonText)
				}

				Hspacer { size: margin*2 }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: 40
					buttonBorder: 0
					buttonText: "25%"
					fontSize: 18
					onPressed: datapadClicked(buttonText)
				}

				Hspacer { size: margin*2 }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: 40
					buttonBorder: 0
					buttonText: "50%"
					fontSize: 18
					onPressed: datapadClicked(buttonText)
				}

				Hspacer { size: margin*2 }

				PadButton {
					buttonWidth: button_width
					buttonHeigth: 40
					buttonBorder: 0
					buttonText: "75%"
					fontSize: 18
					onPressed: datapadClicked(buttonText)
				}

				Hspacer { size: margin*2 }

				PadButton {
					id: button_row8
					buttonWidth: button_width
					buttonHeigth: 40
					buttonBorder: 0
					buttonText: "100%"
					fontSize: 18
					onPressed: datapadClicked(buttonText)
				}

				Hspacer { size: margin*2 }
			}

			Vspacer { size: margin*2 }

			Row {
				Hspacer { size: margin*2 }

				Rectangle {
					id: itemBox
					width: datapadItemDialogBox.width - margin*4
					height: datapadItemBoxItem.height + margin*6 + datapadItemBoxRarity.height
					border.color: "black"
					border.width: 1

					Row {
						Hspacer { size: margin*2 }

						Column {
							Vspacer { size: margin*2 }

							InfoLine {
								id: datapadItemBoxItem
								width: itemBox.width - margin*6 - 140 - 15 // -15 is required but I don't know why!
								infoTitle: ""
								infoText: ""
								infoType: "clear,char-box"
								infoLeftMargin: 0
								infoTextIndent: 0
								infoTextFont: "15px 'Comic Sans MS'"
							}

							Vspacer { size: margin*2 }

							TextField {
								id: searchTextField
								width: itemBox.width - margin*6 - 140
								height: 32
								enabled: itemType === 5 // ITEM_ADD_ITEM

								Keys.onEnterPressed: datapadClickedOK()
								Keys.onReturnPressed: datapadClickedOK()
								Keys.onTabPressed: datapadItemDialogDesc.focus = true;
								onTextChanged: editStatusChanged()
								onFocusChanged: editStatusChanged()

								function editStatusChanged()
								{
									if (itemType === 5 && focus) { // ITEM_ADD_ITEM
										if (text.length >= 3) {
											datapadShopList.visible = true;
											characterData.searchShop(text);
										}
										else {
											datapadShopList.visible = false;
											if (text.length == 0) {
												itemItemKey = "";
												datapadItemBoxItem.infoText = "";
												datapadItemBoxPrice.infoText = "";
												datapadItemBoxRarity.infoText = "";
											}
										}
									}
									else
										datapadShopList.visible = false;
								}
							}

							Vspacer { size: margin*2 }
						}

						Hspacer { size: margin*2 }

						Column {
							Vspacer { size: margin*2 }

							InfoLine {
								id: datapadItemBoxPrice
								width: 140
								infoTitle: "PRICE:"
								infoText: "1234 CR"
								infoType: "clear,char-box"
								infoLeftMargin: 0
								infoTextIndent: 65
								infoTextFont: "15px 'Comic Sans MS'"
							}

							Vspacer { size: margin*2 }

							InfoLine {
								id: datapadItemBoxRarity
								width: 140
								infoTitle: "RARITY:"
								infoText: ""
								infoType: "clear,char-box"
								infoLeftMargin: 0
								infoTextIndent: 65
								infoTextFont: "15px 'Comic Sans MS'"
							}

							Vspacer { size: margin*2 }
						}

						Hspacer { size: margin*2 }
					}
				}

				Hspacer { size: margin*2 }
			}

			Vspacer { size: margin*2 }

			Row {
				Hspacer { size: margin*2 }

				BoxEditLine {
					id: datapadItemDialogDesc
					width: datapadItemDialogBox.width - margin*4
					height: 40
					enabled: itemType === 3 || itemType === 5 // ITEM_AMOUNT || ITEM_ADD_ITEM
					placeHolderText: "Description"
					name: "desc"

					onDonePressed: datapadClickedOK()
					onTabPressed: datapadItemDialogNumber.focus = true;
				}

				Hspacer { size: margin*2 }
			}

			Vspacer { size: margin*2 }

			Row {

				Hspacer { size: margin }

				Hspacer { size: datapadItemDialogBox.width - margin*2 - 3*(100+10) + (itemRef > 0 ? 0 : 100+10) +  (itemType === 3 || itemType === 5 ? 0 : 100+10) }

				PadButton {
					id: apadbutton
					buttonWidth: 100
					buttonHeigth: 40
					buttonBorder: margin
					buttonText: "Cancel"
					onPressed: datapadClicked(buttonText)
				}

				PadButton {
					visible: itemRef > 0 // ITEM_AMOUNT || ITEM_ORIG_STOCK
					buttonWidth: 100
					buttonHeigth: 40
					buttonBorder: margin
					buttonText: "Remove"
					onPressed: datapadClicked(buttonText)
				}

				PadButton {
					visible: itemType === 3 || itemType === 5 // ITEM_AMOUNT || ITEM_ADD_ITEM
					buttonWidth: 100
					buttonHeigth: 40
					buttonBorder: margin
					buttonText: "OK"
					onPressed: datapadClicked(buttonText)
				}

				Hspacer { size: margin }
			}

			Vspacer { size: margin }
		}

		Rectangle {
			id: datapadShopList
			visible: false
			width: searchTextField.width
			height: 260
			x: updatePos(searchTextField, datapadShopList).x
			y: updatePos(searchTextField, datapadShopList).y
			border.width: 1
			border.color: "grey"
			radius: 5

			onVisibleChanged: {
				x = updatePos(searchTextField, datapadShopList).x;
				y = updatePos(searchTextField, datapadShopList).y + searchTextField.height + 2;
			}

			ShopList {
				width: parent.width - 14
				height: parent.height - 2
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
			}
		}
	}

	Dialog {
		id: removeItemDialog
		width: 400
		title: "Remove Item"
		standardButtons: StandardButton.Yes | StandardButton.Cancel

		Column {
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 16
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text:
					"<b><font size=\"+1\">Remove Item</font></b><br><br>"+
					"Do you really want to remove this item?"
			}

			Vspacer { size: 10 }
		}

		onYes: {
			characterData.removeInvItem(itemRef);
			datapadItemDialog.visible = false;
		}
	}

}
