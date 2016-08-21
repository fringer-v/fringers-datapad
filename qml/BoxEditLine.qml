import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant

Rectangle {
	id: boxeditline
	width: 100
	height: 40
	border.width: 1
	border.color: "black"
	color: "lightgrey"

	property string placeHolderText: "0"
	property string boxText: ""
	property string format: "text" // "natural" (0, 1, ...) | "integer" (..., -1, 0, -1, ...)
	property string name: ""

	signal donePressed()
	signal tabPressed()

	TextField {
		id: boxtextfield
		width: parent.width - 2
		height: parent.height - 2
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		placeholderText: placeHolderText

		Keys.onEnterPressed: boxeditline.donePressed()
		Keys.onReturnPressed: boxeditline.donePressed()
		Keys.onTabPressed: boxeditline.tabPressed()
		onTextChanged: {
			var actual_text = actualText(text);
			boxText = actual_text;
			text = actual_text;
		}
	}

	onFormatChanged: {
		var actual_text = actualText(boxText);
		boxtextfield.text = actual_text;
		boxText = actual_text;
	}

	onBoxTextChanged: {
		var actual_text = actualText(boxText);
		boxtextfield.text = actual_text;
		boxText = actual_text;
	}

	onFocusChanged: {
		if (focus)
			boxtextfield.focus = focus;
	}

	function actualText(text)
	{
		var val;

		if (text !== "") {
			if (format !== "text")
				text = text.trim();
			if (format === "natural") {
				val = parseInt(text);

				if (isNaN(val))
					text = "";
				else if (val < 0)
					text = (-val).toString();
			}
			else if (format === "integer") {
				if (text !== "-") {
					val = parseInt(text);

					if (isNaN(val))
						text = "";
				}
			}
		}
		return text;
	}

}
