import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: checklist

	property string checkListType: ""
	property int checkItemRef: 0
	//property real betweenButtons: 12.3
	property real buttonCount: 19
	property real betweenButtons: 5
	property real marginSize: 10
	property real buttonWidth: (checklist.width - 2*marginSize - ((buttonCount-1) * betweenButtons)) / buttonCount // 42.7

	// This area prevents to dialog from closing if you click wrong
	MouseArea {
		width: parent.width - 20
		height: parent.height - 20
		x: 0
		y: 20
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
	}

	Column {
		Row {
			Hspacer {
				size: marginSize
				height: 20
				color: "#00000000";
			}

			Column {
				Column {
					id: headerSection

					Vspacer { size: panels.lineSpace }

					SkillCheck {
						visible: checkListType === "skill"
					}

					WeaponCheck {
						visible: checkListType === "weapon"
					}

					Vspacer { size: panels.lineSpace }

				}

				Row {
					Column {
						Column {
							id: checkListHeader

							Header {
								width: checklist.width - 20 - negativePanel.width
								//width: checklist.width - 20
								headText: "CHECKLIST"
							}

							Vspacer { size: panels.lineSpace - 8 }
						}

						CheckListList {
							width: checklist.width - 20 - negativePanel.width
							//width: checklist.width - 20
							height: checklist.height - headerSection.height - checkListHeader.height
						}
					}

					Rectangle {
						id: negativePanel
						width: 160
						height: checklist.height - headerSection.height;
						//height: 20
						//color: "yellow"

						Rectangle {
							id: negativePanelBox
							x: 10
							y: 8
							width: parent.width - 10
							height: parent.height - 18
							border.width: 1.5
							border.color: "black"

							Column {
								Vspacer { size: panels.lineSpace }

								NegativePool {
									width: negativePanelBox.width - 10
									checkReference: 1
									checkDicePool: "S"
								}
								NegativePool {
									width: negativePanelBox.width - 10
									checkReference: 2
									checkDicePool: "SS"
								}
								NegativePool {
									width: negativePanelBox.width - 10
									checkReference: 4
									checkDicePool: "SSS"
								}
								NegativePool {
									width: negativePanelBox.width - 10
									checkReference: 8
									checkDicePool: "D"
								}
								NegativePool {
									width: negativePanelBox.width - 10
									checkReference: 16
									checkDicePool: "DD"
								}
								NegativePool {
									width: negativePanelBox.width - 10
									checkReference: 32
									checkDicePool: "DDD"
								}
								NegativePool {
									width: negativePanelBox.width - 10
									checkReference: 64
									checkDicePool: "DDDD"
								}
								NegativePool {
									width: negativePanelBox.width - 10
									checkReference: 128
									checkDicePool: "C"
								}
								NegativePool {
									width: negativePanelBox.width - 10
									checkReference: 256
									checkDicePool: "CC"
								}
								NegativePool {
									width: negativePanelBox.width - 10
									checkReference: 512
									checkDicePool: "CCC"
								}
								NegativePool {
									width: negativePanelBox.width - 10
									checkReference: 1024
									checkDicePool: "u"
								}
								NegativePool {
									width: negativePanelBox.width - 10
									checkReference: 2048
									checkDicePool: "uu"
								}
							}
						}
					}
				}
			}

			Hspacer {
				size: 10
				height: 20
				color: "#00000000";
			}
		}
	}

	function clearAll()
	{
		checkItemRef = 0;
		checkListItemDialogDicePool.pool = "";
		checkListItemDialogText.boxText = "";
	}

	onXChanged: {
		if (x == panels.x + (panels.width - checklist.width)/2) {
			characterData.fillCheckList();
		}
		else if (x > panels.x + (panels.width - checklist.width)/2 + checklist.width/2) {
			if (checkListItemDialog.visible) {
				checkListItemDialog.visible = false;
			}
		}
	}

	Dialog {
		id: removeCheckItemDialog
		width: 400
		title: "Remove Check Item"
		standardButtons: StandardButton.Yes | StandardButton.Cancel

		Column {
			anchors.horizontalCenter: parent.horizontalCenter

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 16
				font.family: "Arial"
				horizontalAlignment: Text.AlignHCenter
				text:
					"<b><font size=\"+1\">Remove Check Item</font></b><br><br>"+
					"Do you really want to remove this item?"
			}

			Vspacer { size: 10 }
		}

		onYes: {
			characterData.removeCheckItem(checklist.checkItemRef);
			clearAll();
			checkListItemDialog.visible = false;
		}
	}

	function symbolPadClicked(button)
	{
		if (button === "Cancel")
			checkListItemDialog.visible = false;
		else if (button === "DEL") {
			if (checkListItemDialogDicePool.pool.length > 0)
				checkListItemDialogDicePool.pool = checkListItemDialogDicePool.pool.substring(0, checkListItemDialogDicePool.pool.length-1);
		}
		else if (button === "Remove") {
			if (checkItemRef > 0)
				removeCheckItemDialog.open();
		}
		else if (button === "OK") {
			enterHitonDialog();
		}
		else
			checkListItemDialogDicePool.pool += button;
		Qt.inputMethod.hide();
	}

	function enterHitonDialog()
	{
		checkListItemDialog.visible = false;
		if (checkListItemDialogDicePool.pool.trim().length !== 0 && checkListItemDialogText.boxText.trim().length !== 0) {
			if (checkItemRef != 0)
				characterData.updateCheckItem(checkItemRef, checkListItemDialogDicePool.pool.trim(), checkListItemDialogText.boxText.trim());
			else
				characterData.appendCheckItem(checkListItemDialogDicePool.pool.trim(), checkListItemDialogText.boxText.trim());
		}
		clearAll();
		Qt.inputMethod.hide();
	}

	property int margin: 5

	Rectangle {
		id: checkListItemDialog
		anchors.fill: parent
		color: "#00000000"
		visible: false

		MouseArea {
			anchors.fill: parent

			onClicked: checkListItemDialog.visible = false;
		}

		Rectangle {
			id: boxSize
			width: margin*2 + 9*(50+margin*2)
			height: margin*2 + diceRow.height + margin*2 + buttonRow1.height + buttonRow2.height + margin*2 +
				checkListItemDialogText.height + margin*2 + buttonRow.height + margin*2;
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
			color: "#7F000000"
			radius: 10

			MouseArea {
				anchors.fill: parent
			}

			Column {
				Vspacer { size: margin*2 + 3 }

				Row {
					Hspacer { size: margin*2 }

					Rectangle {
						id: diceRow
						width: 200 // boxSize.width - margin*4
						height: 40
						border.width: 1
						border.color: "black"
						color: "white"

						Rectangle {
							width: parent.width - 2
							height: parent.height - 2
							anchors.horizontalCenter: parent.horizontalCenter
							anchors.verticalCenter: parent.verticalCenter
							border.width: 1
							border.color: "darkgrey"
							radius: 2
							color: "white"

							DicePool {
								id: checkListItemDialogDicePool
								width: parent.width - margin*2
								height: 34
								margin: 5
								spacing: 4
								center: false
								backgroundColor: "white"
								placeHolderText: "Dice Pool"
								anchors.horizontalCenter: parent.horizontalCenter
								anchors.verticalCenter: parent.verticalCenter
								pool: ""
							}
						}
					}

					Hspacer { size: margin*2 }
				}

				Vspacer { size: margin*2 }

				Row {
					Hspacer { size: margin }

					PadButton {
						id: buttonRow1
						buttonWidth: 50
						buttonHeigth: 40
						buttonBorder: margin
						buttonText: "DEL" // "←"
						fontSize: 16
						fontStyle: "Arial"
						fontBold: false
						onPressed: symbolPadClicked("DEL")
					}

					SymbolButton {
						buttonSymbol: "P"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "A"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "B"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "C"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "D"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "S"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "N"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "U"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					Hspacer { size: margin }
				}

				Row {
					Hspacer { size: margin }

					SymbolButton {
						id: buttonRow2
						buttonSymbol: "u"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "d"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "F"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "a"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "t"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "s"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "f"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "x"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					SymbolButton {
						buttonSymbol: "y"
						onPressed: symbolPadClicked(buttonSymbol)
					}

					Hspacer { size: margin }
				}

				Vspacer { size: margin*2 }

				Row {
					Hspacer { size: margin*2 }

					BoxEditLine {
						id: checkListItemDialogText
						width: boxSize.width - margin*4
						height: 40
						placeHolderText: "Description"
						onDonePressed: enterHitonDialog()
					}

					Hspacer { size: margin*2 }
				}

				Vspacer { size: margin*2 }

				Row {

					Hspacer { size: margin }

					Hspacer { size: boxSize.width - margin*2 - 3*(100+10) + (checkItemRef > 0 ? 0 : 100+10) }

					PadButton {
						id: buttonRow
						buttonWidth: 100
						buttonHeigth: 40
						buttonBorder: margin
						buttonText: "Cancel"
						onPressed: symbolPadClicked(buttonText)
					}

					PadButton {
						visible: checkItemRef > 0
						buttonWidth: 100
						buttonHeigth: 40
						buttonBorder: margin
						buttonText: "Remove"
						onPressed: symbolPadClicked(buttonText)
					}

					PadButton {
						buttonWidth: 100
						buttonHeigth: 40
						buttonBorder: margin
						buttonText: "OK"
						onPressed: symbolPadClicked(buttonText)
					}

					Hspacer { size: margin }
				}

				Vspacer { size: margin*2 }
			}
		}
	}
}
