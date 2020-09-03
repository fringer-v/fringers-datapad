import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import space.fringer.datapad 1.0

Rectangle {
	id: panel

	property int sizeMargin: 100
	property real halfWidth: (metaPanel.contentWidth - 20) / 2
	property int portraitHeight: 239
	property int descriptionSectionHeight: Constant.HEADER_HEIGHT + panels.lineSpace + portraitHeight + panels.lineSpace

	Column {
		// DESCRIPTION
		Column {
			id: descriptionSection

			Header {
				width: metaPanel.contentWidth
				headText: "DESCRIPTION"
			}

			Vspacer { size: panels.lineSpace }

			Row {
				DescBlock {
					id: descBlock
					height: portraitHeight
				}

				Hspacer { size: 9 }

				Canvas {
					id: portrait
					antialiasing: true
					width: 200
					height: portraitHeight

					onPaint: {
						var ctx = portrait.getContext("2d");

						Draw.enter(ctx, ctx.canvas.width, ctx.canvas.height, true);
						Draw.drawBox(ctx, 0, 0, portrait.width, portrait.height, 2, "black", portraitData);
						Draw.exit(ctx);
					}
				}

				Hspacer { size: 9 }

				NotableFeatures {
					width: Math.max(metaPanel.contentWidth - portrait.width - descBlock.width - 18, 200)
					height: portraitHeight
				}
			}

			Vspacer { size: panels.lineSpace }
		}

		// MOTIVATIONS
		Column {
			id: motivationsSection

			/*
			Header {
				width: metaPanel.contentWidth
				headText: "MOTIVATIONS"
			}

			Vspacer { size: panels.lineSpace }

			Row {
				width: controller.width
				height: motivations.height

				Hspacer { size: (metaPanel.contentWidth - motivations.width) / 2 }

				Motivations {
					id: motivations
					height: Math.max(100, metaPanel.contentHeight - descriptionSection.height - obligationDutySection.height - Constant.HEADER_HEIGHT - panels.lineSpace*2)
					motivationNotes: characterData.motivationNotes
					motivation: characterData.motivation
					specificMotivation: characterData.specificMotivation
				}
			}

			Vspacer { size: panels.lineSpace }
			*/

			Row {
				Column {
					width: halfWidth

					Header {
						width: halfWidth
						headText: "MOTIVATIONS"
					}

					Vspacer { size: panels.lineSpace - 8 }

					MotivationList {
						width: halfWidth
						height: metaPanel.contentAreaHeight - descriptionSectionHeight - obligationDutySection.height - Constant.HEADER_HEIGHT - (panels.lineSpace - 8)*2
					}

					Vspacer { size: panels.lineSpace - 8 }
				}

				Hspacer { size: 20 }

				Column {
					width: halfWidth

					Header {
						width: halfWidth
						headText: "MORALITY"
					}

					Vspacer { size: panels.lineSpace - 8 }

					MoralityList {
						width: halfWidth
						height: metaPanel.contentAreaHeight - descriptionSectionHeight - obligationDutySection.height - Constant.HEADER_HEIGHT - (panels.lineSpace - 8)*2
					}

					Vspacer { size: panels.lineSpace - 8 }
				}
			}
		}

		// OBLIGATION / DUTY
		Column {
			id: obligationDutySection

			/*
			Column {
				visible: false

				Header {
					width: metaPanel.contentWidth
					headText: "OBLIGATIONS"
				}

				Vspacer { size: panels.lineSpace - 8 }

				Row {
					Hspacer { size: sizeMargin }

					Obligation2List {
						width: metaPanel.contentWidth - sizeMargin*2
						height: Constant.OBLIGATION_HEIGHT*2 + panels.lineSpace + 16 // 16 = fade area
					}
				}

				Vspacer { size: panels.lineSpace - 8 }
			}
			*/

			Row {
				visible: true

				Column {
					width: halfWidth

					Header {
						width: halfWidth
						headText: "OBLIGATIONS"
					}

					Vspacer { size: panels.lineSpace - 8 - 2 } // -2, because the top edge of the obligation box is "indented"

					ObligationList {
						width: halfWidth
						height: Constant.OBLIGATION_HEIGHT*2 + panels.lineSpace - 4 + 16 // 16 = fade area
					}

					Vspacer { size: panels.lineSpace - 8 }
				}

				Hspacer { size: 20 }

				Column {
					width: halfWidth

					Header {
						width: halfWidth
						headText: "DUTY"
					}

					Vspacer { size: panels.lineSpace - 8 - 2 } // -2, because the top edge of the obligation box is "indented"

					DutyList {
						width: halfWidth
						height: Constant.OBLIGATION_HEIGHT*2 + panels.lineSpace - 4 + 16 - 2 // 16 = fade area
					}

					Vspacer { size: panels.lineSpace - 8 }
				}
			}
		}
	}

	Component.onCompleted: {
		characterData.portraitChanged.connect(characterPortraitChanged);
		// Like the pictures in the character list, this does not want to
		// update correctly!
		characterData.imageProviderCountChanged.connect(characterPortraitChanged);
	}

	function characterPortraitChanged() {
		portrait.requestPaint();
	}

	onVisibleChanged: {
		if (visible)
			characterData.showDescription();
	}
}

