import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: preferences

	property string xHost: characterData.host
	property bool active: false
	property bool wasActive: false
	property bool hideTalents: {
		// PMC: This should not be necessary, but the checkbox is
		// not responding, otherwise!?
		hideTalentsCheckbox.checked = (characterData.hideCodedTalents === 1);
		return characterData.hideCodedTalents === 1;
	}

	// This area prevents to dialog from closing if you click wrong
	MouseArea {
		width: parent.width + 16
		height: parent.height + 16
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
	}

	Image {
		width: 28
		height: 28
		x: /*parent.width - 38*/ 10
		y: 30
		source: "qrc:/images/gear-icon-black.png"
	}

	Row {
		Hspacer { size: 10 }

		Column {
			Column {
				id: headerSection

				Vspacer { size: panels.lineSpace }

				Header {
					width: preferences.width - 20
					headText: "SERVER ACCESS"
				}

				Vspacer { size: 20 }
			}

			Rectangle {
				id: hostAccess
				width: 520
				height: 160
				anchors.horizontalCenter: parent.horizontalCenter

				Canvas {
					id: hostAccessCanvas
					anchors.fill: parent

					onPaint: {
						var ctx = hostAccessCanvas.getContext("2d");

						Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height);
						Draw.cornerRect(ctx, 0, 6, 26, 38, 4, Constant.LIGHT_GRAY, "top-left");
						Draw.cornerRect(ctx, 0, hostAccess.height-38, 26, 38, 4, Constant.LIGHT_GRAY, "bottom-left");
						Draw.bevelRect(ctx, 7, 9, hostAccess.width-7, hostAccess.height - 9 - 3, 1.5, 13, "black", "white");
						Draw.bevelRect(ctx, 7 + (hostAccess.width-7 - 240) / 2, 0, 240, 19, 1.5, 4, Constant.LIGHT_GRAY, Constant.LIGHT_GRAY);
						Draw.textBox(ctx, "LOGIN DETAILS", 7 + (hostAccess.width-7 - 240) / 2, 0, 240, 19, "bold 13px Arial", "black", 1, "center", "middle");
						Draw.exit(ctx);
					}
				}

				Grid {
					x: 32
					y: 32
					width: parent.width - 30
					height: parent.height - 28
					columns: 2
					spacing: 10

					Text {
						height: 32
						font.pixelSize: 16
						font.family: "Arial"
						verticalAlignment: Text.AlignVCenter
						text: "Host:"
					}

					TextField {
						id: hostField
						width: 320
						height: 32
						inputMethodHints: Qt.ImhNoAutoUppercase
						placeholderText: "Host name / IP Address"
						text: characterData.host

						Keys.onTabPressed: emailField.focus = true;

						Keys.onReturnPressed: {
							if (hostField.text.trim() != "") {
								if (emailField.text.trim() != "")
									syncWithServer();
								else
									emailField.focus = true;
							}
						}
					}

					Text {
						height: 32
						font.pixelSize: 16
						font.family: "Arial"
						verticalAlignment: Text.AlignVCenter
						text: "Email:"
					}

					TextField {
						id: emailField
						width: 320
						height: 32
						inputMethodHints: Qt.ImhNoAutoUppercase
						placeholderText: "Email Address"
						text: characterData.email

						Keys.onTabPressed: passwordField.focus = true;

						Keys.onReturnPressed: {
							if (emailField.text.trim() != "") {
								if (hostField.text.trim() != "")
									syncWithServer();
								else
									hostField.focus = true;
							}
						}
					}

					Text {
						height: 32
						font.pixelSize: 16
						font.family: "Arial"
						verticalAlignment: Text.AlignVCenter
						text: "Password:"
					}

					TextField {
						id: passwordField
						width: 200
						height: 32
						inputMethodHints: Qt.ImhNoAutoUppercase
						placeholderText: "Password"
						text: ""
						echoMode: TextInput.Password

						Keys.onTabPressed: hostField.focus = true;

						Keys.onReturnPressed: syncWithServer()
					}
				}
			}

			Vspacer { size: panels.lineSpace }

			Rectangle {
				width: 240 + 10 + 240
				height: 40
				anchors.horizontalCenter: parent.horizontalCenter

				Row {
					Button {
						width: 240
						height: 40

						Text {
							anchors.verticalCenter: parent.verticalCenter
							anchors.horizontalCenter: parent.horizontalCenter
							font.pixelSize: 16
							font.family: "Arial"
							horizontalAlignment: Text.AlignHCenter
							text: "Character List"
						}

						onClicked: {
							panels.setVisible(characters);
							controller.state = "menuin";
						}
					}

					Hspacer { size: 10 }

					Button {
						width: 240
						height: 40

						Text {
							anchors.verticalCenter: parent.verticalCenter
							anchors.horizontalCenter: parent.horizontalCenter
							font.pixelSize: 16
							font.family: "Arial"
							horizontalAlignment: Text.AlignHCenter
							text: "Sync with Server Now"
						}

						onClicked: syncWithServer()
					}
				}
			}

			Vspacer { size: 20 }

			Rectangle {
				width: 540
				height: desc_text.height
				anchors.horizontalCenter: parent.horizontalCenter

				Text {
					id: desc_text
					width: parent.width
					anchors.top: parent.top
					wrapMode: Text.Wrap
					horizontalAlignment: Text.AlignHCenter
					text: "If you have no login, go to <u>http://fringer.space</u> where you can register an account. "+
						"Created your character sheet using <i>OggDude's Star Wars Character Generator</i>, "+
						"and upload it to the web-site. Then enter your login details here. After clicking " +
						"\"Sync with Server Now\" you will be able to view your chararacer in this App."
				}
			}

			Vspacer { size: 20 }

			Rectangle {
				id: auto_talent
				width: 540
				height: 100
				anchors.horizontalCenter: parent.horizontalCenter

				Column {
					CheckBox {
						id: hideTalentsCheckbox
						text: "Hide Talents that are automatically added to a Checklist or summed"
						checked: hideTalents
						onCheckedChanged: characterData.hideCodedTalents = (checked ? 1 : 0)
					}

					Vspacer { size: 10 }

					Text {
						width: auto_talent.width
						wrapMode: Text.Wrap
						horizontalAlignment: Text.AlignHCenter
						text: "A Checklist is displayed when you, for example, click on a Skill. The Checklists help "+
							"you to remember what Talents may apply in a situtation. Some Talents are added "+
							"automatically to checklists or contribute to the sum of values such as Soak, Wound or Strain. "+
							"This option allows you to hide those Talents in the Talent list."
					}
				}
			}

			Vspacer { size: panels.lineSpace }
		}

		Hspacer { size: 10 }
	}

	Rectangle {
		anchors.left: parent.left
		anchors.bottom: parent.bottom
		height: versionText.height + 10
		width: versionText.width + 10

		Text {
			id: versionText
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
			font.family: "Arial"
			color: "darkgray"
			text: "Fringer's Datapad 1.0.10, compatible with Oggdude's SWCharGen 1.7.0"
		}
	}

	//onXHostChanged: {
	//	hostField.text = xHost;
	//}

	onActiveChanged: {
		if (active)
			wasActive = true;
		else {
			if (wasActive)
				setPreferences();
			wasActive = false;
		}
	}

	function setPreferences() {
		characterData.host = hostField.text.trim();
		if (emailField.text.trim() != "")
			characterData.email = emailField.text.trim();
		if (passwordField.text.trim() != "")
			characterData.password = passwordField.text.trim();
	}

	function syncWithServer() {
		screen.loadError = false;
		setPreferences();
		Qt.inputMethod.hide();
		characterData.syncWithServer();
	}

	BusyIndicator {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		running: characterData.loading
	}
}
