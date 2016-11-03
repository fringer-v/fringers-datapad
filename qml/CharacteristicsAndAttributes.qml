import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util

Rectangle {
	id: panel

	property int middleMargin: 21
	property real medicationWidth: metaPanel.contentWidth - middleMargin - force.width

	Column {
		Column {
			id: main_data

			// INFO
			InfoHeader {
			}

			Vspacer { size: panels.lineSpace }

			// CHARACTERISTICS
			Header {
				width: metaPanel.contentWidth
				headText: "CHARACTERISTICS"
			}

			Vspacer { size: panels.lineSpace }

			Row {
				id: char_row
				property real charPadding: Math.max((metaPanel.contentWidth - charBrawn.width*6) / 5, 1)

				Characteristic {
					id: charBrawn
					charName: "BRAWN"
					charShortName: "Br"
					charValue: characterData.brawn + characterData.brawnDelta
					onClicked: {
						checklist.checkListType = "skill"
						characterData.itemUuid = "";
						characterData.itemItemKey = "";
						characterData.itemName = "";
						characterData.activeSkill = charName;
						characterData.dicePool = Util.repeat("A", charValue);
						characterData.showCheckList();
						controller.state = "checkout";
					}
				}

				Hspacer { size: char_row.charPadding }

				Characteristic {
					charName: "AGILITY"
					charShortName: "Ag"
					charValue: characterData.agility + characterData.agilityDelta
					onClicked: {
						checklist.checkListType = "skill"
						characterData.itemUuid = "";
						characterData.itemItemKey = "";
						characterData.itemName = "";
						characterData.activeSkill = charName;
						characterData.dicePool = Util.repeat("A", charValue);
						characterData.showCheckList();
						controller.state = "checkout";
					}
				}

				Hspacer { size: char_row.charPadding }

				Characteristic {
					charName: "INTELLECT"
					charShortName: "Int"
					charValue: characterData.intellect + characterData.intellectDelta
					onClicked: {
						checklist.checkListType = "skill"
						characterData.itemUuid = "";
						characterData.itemItemKey = "";
						characterData.itemName = "";
						characterData.activeSkill = charName;
						characterData.dicePool = Util.repeat("A", charValue);
						characterData.showCheckList();
						controller.state = "checkout";
					}
				}

				Hspacer { size: char_row.charPadding }

				Characteristic {
					charName: "CUNNING"
					charShortName: "Cun"
					charValue: characterData.cunning + characterData.cunningDelta
					onClicked: {
						checklist.checkListType = "skill"
						characterData.itemUuid = "";
						characterData.itemItemKey = "";
						characterData.itemName = "";
						characterData.activeSkill = charName;
						characterData.dicePool = Util.repeat("A", charValue);
						characterData.showCheckList();
						controller.state = "checkout";
					}
				}

				Hspacer { size: char_row.charPadding }

				Characteristic {
					charName: "WILLPOWER"
					charShortName: "Wil"
					charValue: characterData.willpower + characterData.willpowerDelta
					onClicked: {
						checklist.checkListType = "skill"
						characterData.itemUuid = "";
						characterData.itemItemKey = "";
						characterData.itemName = "";
						characterData.activeSkill = charName;
						characterData.dicePool = Util.repeat("A", charValue);
						characterData.showCheckList();
						controller.state = "checkout";
					}
				}

				Hspacer { size: char_row.charPadding }

				Characteristic {
					charName: "PRESENCE"
					charShortName: "Pr"
					charValue: characterData.presence + characterData.presenceDelta
					onClicked: {
						checklist.checkListType = "skill"
						characterData.itemUuid = "";
						characterData.itemItemKey = "";
						characterData.itemName = "";
						characterData.activeSkill = charName;
						characterData.dicePool = Util.repeat("A", charValue);
						characterData.showCheckList();
						controller.state = "checkout";
					}
				}
			}

			Vspacer { size: panels.lineSpace }

			// ATTRIBUTES
			Header {
				width: metaPanel.contentWidth
				headText: "ATTRIBUTES"
			}

			Vspacer { size: panels.lineSpace }

			Row {
				Rectangle {
					width: ws.width
					height: ws.height

					WoundAndStrain {
						id: ws
						anchors.left: parent.left
						anchors.top: parent.top
						attributeHeight: woundThreshold.height
					}

					Attribute {
						id: woundThreshold
						anchors.left: parent.left
						anchors.top: parent.top
						attrText: "WOUND"
						attrValue: characterData.wound
						attrDelta: woundDelta()
					}

					Attribute {
						anchors.right: parent.right
						anchors.top: parent.top
						attrText: "STRAIN"
						attrValue: characterData.strain
						attrDelta: strainDelta()
					}
				}

				Hspacer { size: Draw.pixel(10) }

				Column {
					Attribute {
						attrText: "SOAK"
						attrValue: characterData.soak
					}

					Rectangle { width: Draw.pixel(2); height: Draw.pixel(32) }

					Attribute2 {
						attrText: "DEFENSE"
						attrSubText1: "RANGED"
						attrSubText2: "MELEE"
						attrValue1: characterData.defenseRanged
						attrValue2: characterData.defenseMelee

						MouseArea {
							anchors.top: parent.top
							anchors.left: parent.left
							anchors.bottom: parent.bottom
							width: parent.width/2
							onClicked: {
								checklist.checkListType = "skill"
								characterData.itemItemKey = "";
								characterData.itemUuid = "";
								characterData.itemName = "";
								characterData.activeSkill = "DEFR";
								characterData.dicePool = "?";
								characterData.showCheckList();
								controller.state = "checkout";
							}
						}

						MouseArea {
							anchors.top: parent.top
							anchors.right: parent.right
							anchors.bottom: parent.bottom
							width: parent.width/2
							onClicked: {
								checklist.checkListType = "skill"
								characterData.itemUuid = "";
								characterData.itemItemKey = "";
								characterData.itemName = "";
								characterData.activeSkill = "DEFM";
								characterData.dicePool = "?";
								characterData.showCheckList();
								controller.state = "checkout";
							}
						}

					}
				}

				Hspacer { size: Draw.pixel(10) }

				CriticalInjuries {
				}
			}

			Rectangle { width: Draw.pixel(1); height: panels.lineSpace * 1.4 }
		}

		Row {
			Column {
				id: force
				width: moraltyLevels.width + 10 + 129

				Header {
					width: force.width
					headText: "FORCE"
				}

				Vspacer { size: panels.lineSpace }

				Row {
					Column {
						id: moraltyLevels

						MoralityAndConflict {
						}

						Vspacer { size: panels.lineSpace }

						MoralityLevel {
						}
					}

					Hspacer { size: 10 }

					Characteristic {
						charWidth: 129
						charName: "POINTS"
						charLineColor: characterData.force > 0 ? "black" : Constant.DARK_GRAY
						charValue: characterData.force
						charCurrentValue: characterData.forceCommitted === 0 ? -1 : (characterData.force - characterData.forceCommitted)
						onClicked: {
							if (characterData.force > 0) {
								checklist.checkListType = "skill"
								characterData.itemUuid = "";
								characterData.itemItemKey = "";
								characterData.itemName = "";
								characterData.activeSkill = "FORCE";
								characterData.dicePool = Util.repeat("F", characterData.force);
								characterData.showCheckList();
								controller.state = "checkout";
							}
						}
					}
				}
			}			

			Hspacer { size: middleMargin }

			Column {
				id: medication
				width: medicationWidth

				Header {
					width: medication.width
					headText: "MEDICATION"
				}

				Vspacer { size: panels.lineSpace }

				Row {
					Rectangle {
						width: 30
						height: 124
						border.color: Constant.DARK_GRAY
						border.width: 1
						color: Constant.HEADER_COLOR

						Text {
							anchors.fill: parent
							font.pixelSize: 16
							font.family: "Arial"
							font.bold: true
							horizontalAlignment: Text.AlignHCenter
							verticalAlignment: Text.AlignVCenter
							text: "R\nE\nS\nE\nT"
						}

						MouseArea {
							anchors.fill: parent

							onClicked: {
								characterData.useStimPack(-characterData.stimPacksUsed);
								characterData.useErp(-characterData.erpsUsed);
							}
						}
					}

					Hspacer { size: 10 }

					Medication {
						mediText: "STIMPACK"
						inventory: characterData.stimPacks
						used: characterData.stimPacksUsed
					}

					Hspacer { size: 10 }

					Medication {
						mediText: "ERP"
						inventory: characterData.erps
						used: characterData.erpsUsed
					}
				}
			}
		}

		//Notes {
		//	width: Math.max(metaPanel.contentWidth, 280)
		//	height: Math.max(metaPanel.contentHeight - main_data.height, 100)
		//}
	}

	property int spacer: 10
	property int bsize: 60
	property int numberPadBase: 12
	property int numberPadValue: 0
	property string numberPadSubject: ""

	function woundHistory()
	{
		if (numberPadSubject === "adjust-wounds" && numberPadValue != 0)
			return addHistory(characterData.woundHistory, characterData.currentWounds + numberPadValue);
		return characterData.woundHistory
	}

	function currentWounds()
	{
		if (numberPadSubject === "adjust-wounds")
			return characterData.currentWounds + numberPadValue;
		return characterData.currentWounds
	}

	function woundDelta()
	{
		if (numberPadSubject === "wound-threshold")
			return numberPadValue;
		return characterData.woundDelta
	}

	function strainHistory()
	{
		if (numberPadSubject === "adjust-strain" && numberPadValue != 0)
			return addHistory(characterData.strainHistory, characterData.currentStrain + numberPadValue);
		return characterData.strainHistory
	}

	function currentStrain()
	{
		if (numberPadSubject === "adjust-strain")
			return characterData.currentStrain + numberPadValue;
		return characterData.currentStrain
	}

	function strainDelta()
	{
		if (numberPadSubject === "strain-threshold")
			return numberPadValue;
		return characterData.strainDelta
	}

	function conflictValue()
	{
		if (characterData.force === 0)
			return "";
		var val = characterData.currentConflict;
		if (numberPadSubject === "conflict")
			val += numberPadValue;

		return val === 0 ? "" : val;
	}

	function addHistory(current, new_value)
	{
		var result;

		//current = Util.appendList(current, delta > 0 ? "+"+delta : String(delta), " ");
		current = Util.appendList(current, new_value, " ");
		if (current.length > 22)
			while (current.length  > 22)
				current = Util.right(current, " ");
		return current;
	}

	function setNumberPadSubject(subject)
	{
		numberPadValue = 0;
		numberPadSubject = subject;
		if (numberPadSubject == "adjust-wounds")
			numberPadBase = characterData.currentWounds;
		else if (numberPadSubject == "adjust-strain")
			numberPadBase = characterData.currentStrain;
		else if (numberPadSubject == "wound-threshold") {
			numberPadBase = characterData.wound;
			numberPadValue = characterData.woundDelta;
		}
		else if (numberPadSubject == "strain-threshold") {
			numberPadBase = characterData.strain;
			numberPadValue = characterData.strainDelta;
		}
		else if (numberPadSubject == "conflict")
			numberPadBase = characterData.currentConflict;

		numberPad.visible = numberPadSubject !== "";
	}

	function numberPadClicked(button)
	{
		if (button === "C")
			numberPadValue = 0;
		else if (button === "Cancel")
			setNumberPadSubject("");
		else if (button === "OK") {
			if (numberPadSubject == "adjust-wounds")
				characterData.adjustWounds(numberPadValue);
			else if (numberPadSubject == "adjust-strain")
				characterData.adjustStrain(numberPadValue);
			else if (numberPadSubject == "wound-threshold")
				characterData.setWoundDelta(numberPadValue);
			else if (numberPadSubject == "strain-threshold")
				characterData.setStrainDelta(numberPadValue);
			else if (numberPadSubject == "conflict")
				characterData.adjustConflict(numberPadValue);
			setNumberPadSubject("");
		}
		else
			numberPadValue += parseInt(button)
	}

	Rectangle {
		id: numberPad
		anchors.fill: parent
		color: "#00000000"
		visible: false

		MouseArea {
			anchors.fill: parent

			onClicked: setNumberPadSubject("");
		}

		Rectangle {
			height: spacer*6-spacer/2 + bsize*4
			width: height
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
			color: "#7F000000"
			radius: 10

			Rectangle {
				height: spacer*5 + bsize*4
				width: height
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				color: "#00000000"

				Column {
					Vspacer { size: spacer/2 }
					Row {
						Hspacer { size: spacer/2 }
						Rectangle {
							width: bsize*3 + spacer*3
							height: bsize + spacer
							color: "#00000000"

							Rectangle {
								width: bsize*3 + spacer*2
								height: bsize
								border.width: 1
								border.color: "black"
								anchors.verticalCenter: parent.verticalCenter
								anchors.horizontalCenter: parent.horizontalCenter
								color: "lightgrey"

								Text {
									width: parent.width - 20
									anchors.horizontalCenter: parent.horizontalCenter
									anchors.verticalCenter: parent.verticalCenter
									font.pixelSize: 24
									font.family: "Comic Sans MS"
									color: "grey"
									horizontalAlignment: Text.AlignLeft
									text: numberPadBase
								}

								Text {
									width: parent.width
									anchors.horizontalCenter: parent.horizontalCenter
									anchors.verticalCenter: parent.verticalCenter
									font.pixelSize: 24
									font.family: "Comic Sans MS"
									font.bold: true
									horizontalAlignment: Text.AlignHCenter
									text: (numberPadValue >= 0 ? "+" : "") + numberPadValue;
								}

								Text {
									width: parent.width - 20
									anchors.horizontalCenter: parent.horizontalCenter
									anchors.verticalCenter: parent.verticalCenter
									font.pixelSize: 24
									font.family: "Comic Sans MS"
									color: "grey"
									horizontalAlignment: Text.AlignRight
									text: "=" + (numberPadBase + numberPadValue)
								}
							}
						}
						PadButton {
							buttonWidth: bsize
							buttonBorder: spacer/2
							buttonText: "C"
							onPressed: numberPadClicked(buttonText)
						}
					}
					Row {
						Hspacer { size: spacer/2 }
						PadButton {
							buttonWidth: bsize
							buttonBorder: spacer/2
							buttonText: "+1"
							onPressed: numberPadClicked(buttonText)
						}
						PadButton {
							buttonWidth: bsize
							buttonBorder: spacer/2
							buttonText: "+2"
							onPressed: numberPadClicked(buttonText)
						}
						PadButton {
							buttonWidth: bsize
							buttonBorder: spacer/2
							onPressed: numberPadClicked(buttonText)
							buttonText: "+5"
						}
						PadButton {
							buttonWidth: bsize
							buttonBorder: spacer/2
							buttonText: "+10"
							onPressed: numberPadClicked(buttonText)
						}
					}
					Row {
						Hspacer { size: spacer/2 }
						PadButton {
							buttonWidth: bsize
							buttonBorder: spacer/2
							onPressed: numberPadClicked(buttonText)
							buttonText: "-1"
						}
						PadButton {
							buttonWidth: bsize
							buttonBorder: spacer/2
							buttonText: "-2"
							onPressed: numberPadClicked(buttonText)
						}
						PadButton {
							buttonWidth: bsize
							buttonBorder: spacer/2
							buttonText: "-5"
							onPressed: numberPadClicked(buttonText)
						}
						PadButton {
							buttonWidth: bsize
							buttonBorder: spacer/2
							buttonText: "-10"
							onPressed: numberPadClicked(buttonText)
						}
					}
					Row {
						Hspacer { size: spacer/2 }
						PadButton {
							buttonWidth: bsize*2 + spacer
							buttonHeigth: bsize
							buttonBorder: spacer/2
							buttonText: "Cancel"
							onPressed: numberPadClicked(buttonText)
						}
						PadButton {
							buttonWidth: bsize*2 + spacer
							buttonHeigth: bsize
							buttonBorder: spacer/2
							buttonText: "OK"
							onPressed: numberPadClicked(buttonText)
						}
					}
				}
			}
		}
	}

	onVisibleChanged: {
		if (visible)
			characterData.showCharacteristics();
	}
}

