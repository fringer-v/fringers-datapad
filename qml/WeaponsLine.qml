import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util

Rectangle {
	id: weaponsLine
	width: parent.width
	height: row1.height + ((attachmentsVisible && (features.length > 0 || attachments.length > 0 || notes.length)) ? row2.height : 0)
	color: lineColor

	property var colWidths: adjustedlColWidths
	property real lineWidth: 1
	property int minLineHeight: 24
	property int lineFontSize: 15
	property int descFontSize: 14
	property int bottomTextMargin: 3
	property string lineColor: (index % 2) == 1 ? Constant.LIST_LINE_GRAY : "white"
	property bool attachmentsVisible: false

	Row {
		id: row1
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: Math.max(Math.max(weaponText.height, specialText.height), minLineHeight);

		Hspacer { size: listIndent; color: lineColor }

		Rectangle {
			width: colWidths[0]
			height: parent.height
			color: lineColor

			Text {
				id: weaponText
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				font.pixelSize: lineFontSize
				font.family: "Arial"
				font.bold: true
				horizontalAlignment: Text.AlignLeft
				text: {
					if (quantity != 1) {
						var name = quantity +"x "+ weapon;
						if (carry_state != 1 && stored > 0) {
							if (stored >= quantity)
								name = "1/"+name;
							else
								name = (quantity-stored)+"/"+name;
						}
						return name;
					}
					return weapon;
				}
			}

			MouseArea {
				anchors.fill: parent
				onClicked: useWeapon()
			}
		}

		Rectangle {
			width: colWidths[1]
			height: parent.height
			color: lineColor

			Text {
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				font.pixelSize: lineFontSize
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text: skill
			}

			MouseArea {
				anchors.fill: parent
				onClicked: useWeapon()
			}
		}

		Rectangle {
			width: colWidths[2]
			height: parent.height
			color: lineColor

			Text {
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				font.pixelSize: lineFontSize
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text: range
			}

			MouseArea {
				anchors.fill: parent
				onClicked: useWeapon()
			}
		}

		Rectangle {
			width: colWidths[3]
			height: parent.height
			color: lineColor

			Text {
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				font.pixelSize: lineFontSize
				font.family: "Arial"
				font.bold: true
				horizontalAlignment: Text.AlignHCenter
				text: damage
			}

			MouseArea {
				anchors.fill: parent
				onClicked: useWeapon()
			}
		}

		Rectangle {
			width: colWidths[4]
			height: parent.height
			color: lineColor

			/*DicePool {
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				height: minLineHeight
				color: lineColor
				margin: 2
				spacing: 2
				pool: critical
			}
			FlowText {
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				lineHeight: parent.height
				pixelSize: 15
				shiftText: 1
				text: critical
			}
			*/
			Text {
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				font.pixelSize: lineFontSize
				font.family: "Arial"
				font.bold: true
				horizontalAlignment: Text.AlignHCenter
				text: (critical === 0) ? "" : critical
			}

			MouseArea {
				anchors.fill: parent
				onClicked: useWeapon()
			}
		}

		Rectangle {
			width: colWidths[5]
			height: parent.height
			color: lineColor

			DicePool {
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				height: minLineHeight
				color: lineColor
				backgroundColor: lineColor
				margin: 2
				spacing: 2
				pool: dicePool
			}

			MouseArea {
				anchors.fill: parent
				onClicked: useWeapon()
			}
		}

		HeldEquip {
			width: colWidths[6]
			height: parent.height
			color: lineColor
			invUuid: uuid
			invName: weapon
			invState: carry_state
			invQuantity: quantity
			invStored: stored
			enabled: itemkey !== "UNARMED"
		}

		Rectangle {
			width: colWidths[7]
			height: parent.height
			color: lineColor

			Image {
				id: cart_icon
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				antialiasing: true
				smooth: true
				visible: itemkey !== "UNARMED"
				source: "qrc:/images/cart20.png"
			}

			MouseArea {
				anchors.fill: parent
				onClicked: {
					if (itemkey !== "UNARMED")
						gearDatapadDialog.newDatapadItem(itemkey, 3, weapon); // ITEM_AMOUNT
				}
			}
		}

		Rectangle {
			width: colWidths[8]
			height: parent.height
			color: lineColor

			FlowText {
				id: specialText
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				//font.pixelSize: descFontSize
				//font.family: "Arial"
				//horizontalAlignment: Text.AlignLeft
				//wrapMode: Text.Wrap
				text: {
					var result = "";

					if (features.length > 0)
						result = "[U]Features...[u]";
					else if (attachments.length > 0)
						result = "[U]Attachments...[u]";
					else if (notes.length > 0)
						result = "[U]Notes...[u]";

					if (qualities.length > 0) {
						if (result.length > 0)
							result = qualities + ", " + result;
						else
							result = qualities;
					}
					return result;
				}
			}

			MouseArea {
				anchors.fill: parent

				onClicked: attachmentsVisible = !attachmentsVisible
			}
		}

		Hspacer { size: listIndent; color: lineColor }
	}

	Row {
		id: row2
		visible: attachmentsVisible && (features.length > 0 || attachments.length > 0 || notes.length > 0)
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		height: attachText.height + bottomTextMargin

		Hspacer { size: listIndent; color: lineColor }

		Column {
			/*
			Text {
				id: attachText
				width: weaponsLine.width - listIndent*2
				font.pixelSize: descFontSize
				font.family: "Arial"
				horizontalAlignment: Text.AlignLeft
				wrapMode: Text.Wrap
				text: "[U]Attachments:[u] " + attachments
			}
			*/
			FlowText {
				id: attachText
				width: weaponsLine.width - listIndent*2
				text: {
					var result = "";

					if (features.length > 0)
						result = "[B]Features:[b] " + features;
					if (attachments.length > 0)
						result = Util.appendList(result, "[B]Attachments:[b] " + attachments, " ");
					if (notes.length > 0)
						result = Util.appendList(result, "[B]Notes:[b] " + notes, " ");
					return result;
				}
			}

			Vspacer { size: bottomTextMargin; color: lineColor }
		}

		Hspacer { size: listIndent; color: lineColor }
	}

	function useWeapon()
	{
		checklist.checkListType = "weapon"

		var desc = "";

		if (features.length > 0)
			desc = "[B]Features:[b] " + features;
		if (attachments.length > 0)
			desc = Util.appendList(desc, "[B]Attachments:[b] " + attachments, " ");
		if (notes.length > 0)
			desc = Util.appendList(desc, "[B]Notes:[b] " + notes, " ");

		characterData.itemUuid = uuid;
		characterData.itemItemKey = itemkey;
		characterData.itemName = weapon;
		characterData.itemRange = range;
		characterData.itemSkill = skill;
		characterData.itemDamage = damage;
		characterData.itemCritLevel = critical;
		characterData.itemQualMag = qualities;
		characterData.itemAttachDesc = desc;
		characterData.itemManeuvers = "";
		characterData.itemStrain = "";
		characterData.itemCritPlus = critplus;
		characterData.itemPierce = pierce;
		characterData.showCheckList(skill, "", "", dicePool);
		controller.state = "checkout";
	}
}
