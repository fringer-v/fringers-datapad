import QtQuick 2.3
import "../js/drawing.js" as Draw
import "../js/constants.js" as Constant
import "../js/utilities.js" as Util
import space.fringer.datapad 1.0

BaseList {
	id: skillList

	property string skillType: undefined
	property var skillData: undefined
	property bool customSkills: false

	headerHeight: 25
	headerColor: Constant.HEADER_COLOR
	headerFontSize: 14
	colTitles: [ skillType, "CAREER", "RANK",  "DICE POOL" ]
	colWidths: [ 0, 76, 49, 173 ] // 493
	listDelegate: skillLine
	listModel: skillData
	listIndent: 3

	Component {
		id: skillLine
		SkillLine {
		}
	}
}
