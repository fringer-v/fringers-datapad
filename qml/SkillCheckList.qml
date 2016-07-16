import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: checklist

	property int checkItemRef: 0

	// This area prevents to dialog from closing if you click wrong
	MouseArea {
		width: parent.width + 16
		height: parent.height + 16
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
	}

	Column {
		Row {
			Hspacer { size: 10; color: "#00000000"; }

			Column {
				Column {
					id: headerSection

					Vspacer { size: panels.lineSpace }

					TitleBox {
						width: checklist.width - 20
						height: 60
						boxTitle: "DICE POOL"
						boxText: characterData.dicePool
						boxCornerHeight: 20
						displayDicePool: true
					}

					Vspacer { size: panels.lineSpace }

					Header {
						width: checklist.width - 20
						headText: "CHECKLIST"
					}

					Vspacer { size: panels.lineSpace - 8 }
				}

				CheckListList {
					width: checklist.width - 20;
					height: checklist.height - headerSection.height - bottonSection.height - editRow.height;
				}

				Column {
					id: bottonSection

					Vspacer { size: panels.lineSpace - 8 }

					// Dice sequence:
					// P=Proficiency (yellow)
					// A=Ability (green)
					// B=Boost (blue)
					// C=Challenge (red)
					// D=Difficulty (purple)
					// S=Setback (black)
					// E=empty
					// N=Negative Setback (black with red dash)
					// U=Upgrade Ability
					// u=Upgrade Difficulty

					// Font:
					// Z = white
					// a = advantage
					// f = failure
					// s = success
					// t = threat
					// x = triumph
					// y = despair
					// z = black
					Row {
						SymbolButton {
							symbol: "P"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "A"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "B"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "C"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "D"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "S"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "N"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "U"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "u"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "a"
							textField: dicePoolField
						}
						Hspacer { size: 6 }

						SymbolButton {
							symbol: "t"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "Z"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "z"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "s"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "f"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "x"
							textField: dicePoolField
						}

						Hspacer { size: 6 }

						SymbolButton {
							symbol: "y"
							textField: dicePoolField
						}
					}

					//Vspacer { size: panels.lineSpace }
				}
			}

			Hspacer { size: 10; color: "#00000000"; }
		}

		Row {
			id: editRow

			Rectangle {
				id: leftSection
				width: 200
				height: 32 + 6 + panels.lineSpace

				MouseArea {
					anchors.fill: parent

					onClicked: {
						dicePoolField.focus = true;
					}
				}

				Rectangle {
					id: pad0
					height: 6
					width: parent.width
					anchors.top: parent.top
					anchors.left: parent.left
				}

				TextField {
					id: dicePoolField
					width: parent.width - 10
					height: 32
					anchors.top: pad0.bottom
					anchors.right: parent.right
					placeholderText: "Dice Pool"

					Keys.onReturnPressed: {
						if (dicePoolField.text.trim().length != 0 && diceDescField.text.trim().length !== 0) {
							if (checkItemRef != 0)
								characterData.updateCheckItem(checkItemRef, dicePoolField.text.trim(), diceDescField.text.trim());
							else
								characterData.addCheckItem(dicePoolField.text.trim(), diceDescField.text.trim());
						}
						clearAll();
						Qt.inputMethod.hide();
					}
				}
			}

			Rectangle {
				width: checklist.width - leftSection.width - rightSection.width
				height: 32 + 6 + panels.lineSpace

				MouseArea {
					anchors.fill: parent

					onClicked: {
						diceDescField.focus = true;
					}
				}

				Rectangle {
					id: pad1
					height: 6
					width: parent.width
					anchors.top: parent.top
					anchors.left: parent.left
				}

				TextField {
					id: diceDescField
					width: parent.width - 10
					height: 32
					anchors.top: pad1.bottom
					anchors.right: parent.right
					placeholderText: "Description"

					Keys.onReturnPressed: {
						if (dicePoolField.text.trim().length != 0 && diceDescField.text.trim().length !== 0) {
							if (checkItemRef != 0)
								characterData.updateCheckItem(checkItemRef, dicePoolField.text.trim(), diceDescField.text.trim());
							else
								characterData.addCheckItem(dicePoolField.text.trim(), diceDescField.text.trim());
						}
						clearAll();
						Qt.inputMethod.hide();
					}
				}
			}

			Column {
				id: rightSection

				Vspacer { size: 6 }

				Row {
					Hspacer { size: 10; color: "#00000000"; }

					Button {
						id: addButton
						width: 42
						height: 32
						text: "ADD"
						enabled: dicePoolField.text.trim().length != 0 && diceDescField.text.trim().length !== 0
						onClicked: {
							if (dicePoolField.text.length !== 0 && diceDescField.text.length !== 0)
								characterData.addCheckItem(dicePoolField.text.trim(), diceDescField.text.trim());
							clearAll();
							Qt.inputMethod.hide();
						}
					}

					Hspacer { size: 10; color: "#00000000"; }

					Button {
						id: setButton
						width: 42
						height: 32
						text: "SET"
						enabled: checkItemRef != 0
						onClicked: {
							if (dicePoolField.text.length !== 0 && diceDescField.text.length !== 0)
								characterData.updateCheckItem(checkItemRef, dicePoolField.text.trim(), diceDescField.text.trim());
							clearAll();
							Qt.inputMethod.hide();
						}
					}

					Hspacer { size: 10; color: "#00000000"; }

					Button {
						width: 42
						height: 32
						text: "DEL"
						enabled: checkItemRef != 0
						onClicked: {
							removeCheckItemDialog.open();
							Qt.inputMethod.hide();
						}
					}

					Hspacer { size: 10; color: "#00000000"; }
				}
			}
		}
	}

	function clearAll()
	{
		checkItemRef = 0;
		dicePoolField.text = "";
		diceDescField.text = "";
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
		}
	}
}
