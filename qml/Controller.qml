import QtQuick 2.0
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Item {
	id: controller
	state: "menuin"

	property var currentPanel: undefined
	property int dialogBorder: 72

	function gotoPreferences()
	{
		panels.setVisible(characters);
		controller.state = "menuin";
	}

	Menu {
		id: menu
		height: controller.height
	}

	Item {
		id: panels
		width: controller.width
		height: controller.height
		anchors.left: menu.right

		property real lineSpace: Draw.pixel(10)

		function setVisible(panel)
		{
			// Indicate that we are about to change skills so that the lists can be
			// refreshed if necessary!
			if (controller.state == "checkout")
				characterData.hideCheckList();
			characteristics.visible = false;
			description.visible = false;
			skills.visible = false;
			experience.visible = false;
			weapons.visible = false;
			inventory.visible = false;
			characters.visible = false;
			panel.visible = true;
			controller.currentPanel = panel;
			controller.state = "menuin";
			Qt.inputMethod.hide();
		}

		Panel {
			id: characteristics
			anchors.fill: parent
			panelTitle: "Characteristics & Attributes"
			panelContent: "CharacteristicsAndAttributes.qml"
		}

		Panel {
			id: skills
			anchors.fill: parent
			panelTitle: "Skills"
			panelContent: "Skills.qml"

			property int nameColSize: 200
			property int careerColSize: 62
			property int rankColSize: 38
			property int dicePoolColSize: 155
			property string rowColor: Constant.ROW_COLOR
		}

		Panel {
			id: description
			anchors.fill: parent
			panelTitle: "Description & Background"
			panelContent: "DescriptionAndBackground.qml"
		}

		// Experience, Special Features, Talents
		Panel {
			id: experience
			anchors.fill: parent
			panelTitle: "Experience & Talents"
			panelContent: "ExperienceAndTalents.qml"
		}

		// Weapons, Armor
		Panel {
			id: weapons
			anchors.fill: parent
			panelTitle: "Weapons & Armor"
			panelContent: "WeaponsAndArmor.qml"
		}

		// Encumbrance, Money, Gear
		Panel {
			id: inventory
			anchors.fill: parent
			panelTitle: "Inventory & Gear"
			panelContent: "InventoryAndGear.qml"
		}

		Panel {
			id: characters
			anchors.fill: parent
			panelTitle: "Character List"
			panelContent: "CharactersPanel.qml"
		}

		Rectangle {
			id: overlay
			anchors.fill: parent
			color: "#77000000"

			MouseArea {
				anchors.fill: parent
				onClicked: {
					panels.setVisible(controller.currentPanel);
				}
			}
		}
	}

	Preferences {
		id: preferences

		x: panels.x + panels.width
		y: panels.y + (panels.height - preferences.height)/2
		width: panels.width - dialogBorder
		height: panels.height - dialogBorder
		radius: 10
		//color: "yellow"

		Behavior on x {
			PropertyAnimation { easing.type: Easing.InOutQuad; duration: 500 }
		}
	}

	Datapad {
		id: cashbook
		x: panels.x + panels.width
		y: panels.y + (panels.height - cashbook.height)/2
		width: panels.width - dialogBorder
		height: panels.height - dialogBorder
		radius: 10
		//color: "yellow"

		Behavior on x {
			PropertyAnimation { easing.type: Easing.InOutQuad; duration: 500 }
		}
	}

	CheckList {
		id: checklist
		x: panels.x + panels.width
		y: panels.y + (panels.height - checklist.height)/2
		width: panels.width - dialogBorder
		height: panels.height - dialogBorder
		radius: 10
		//color: "yellow"

		Behavior on x {
			PropertyAnimation { easing.type: Easing.InOutQuad; duration: 500 }
		}
	}

	DatapadDialog {
		id: gearDatapadDialog
	}

	ExperienceDialog {
		id: experienceDialog
	}

	states : [
		State {
			name: "menuin"
			PropertyChanges { target: menu; x: -menu.width }
			PropertyChanges { target: overlay; visible: false }
			PropertyChanges { target: preferences; x: panels.x + panels.width; active: false }
			PropertyChanges { target: cashbook; x: panels.x + panels.width }
			PropertyChanges { target: checklist; x: panels.x + panels.width }
		},
		State {
			name: "menuout"
			PropertyChanges { target: menu; x: 0 }
			PropertyChanges { target: overlay; visible: true }
			PropertyChanges { target: preferences; x: panels.x + panels.width; active: false }
			PropertyChanges { target: cashbook; x: panels.x + panels.width }
			PropertyChanges { target: checklist; x: panels.x + panels.width }
		},
		State {
			name: "prefout"
			PropertyChanges { target: menu; x: -menu.width }
			PropertyChanges { target: overlay; visible: true }
			PropertyChanges { target: preferences; x: panels.x + (panels.width - preferences.width)/2; active: true }
			PropertyChanges { target: cashbook; x: panels.x + panels.width }
			PropertyChanges { target: checklist; x: panels.x + panels.width }
		},
		State {
			name: "cashout"
			PropertyChanges { target: menu; x: -menu.width }
			PropertyChanges { target: overlay; visible: true }
			PropertyChanges { target: preferences; x: panels.x + panels.width; active: false }
			PropertyChanges { target: cashbook; x: panels.x + (panels.width - cashbook.width)/2 }
			PropertyChanges { target: checklist; x: panels.x + panels.width }
		},
		State {
			name: "checkout"
			PropertyChanges { target: menu; x: -menu.width }
			PropertyChanges { target: overlay; visible: true }
			PropertyChanges { target: preferences; x: panels.x + panels.width; active: false }
			PropertyChanges { target: cashbook; x: panels.x + panels.width }
			PropertyChanges { target: checklist; x: panels.x + (panels.width - checklist.width)/2 }
		}
	]

	Component.onCompleted: {
		if (characterData.name.length === 0) {
			if (characterData.characterCount === 0) {
				panels.setVisible(characteristics);
				controller.state = "prefout";
			}
			else
				panels.setVisible(characters);
		}
		else
			panels.setVisible(characteristics);

	}
}
