/* Copyright (c) 2011, Paul McCullagh (Fringer)
 *
 * Datapad
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, please go to
 * http://www.gnu.org/licenses/gpl-2.0.html for further details.
 *
 * 2011-12-07 Paul McCullagh
 *
 * H&G2JCtL
 *
 */

#include <string.h>

#include <QtXml>

#include "Skills.h"
#include "DataAccess.h"
#include "Character.h"

// Skill -------------------------

Skill skills[] = {
	{ KM_ASTRO, "ASTRO", "Astrogation", "Astrogation", INTELLECT, GENERAL },
	{ KM_ATHL, "ATHL", "Athletics", "Athletics", BRAWN, GENERAL },
	{ KM_BRAWL, "BRAWL", "Brawl", "Brawl", BRAWN, COMBAT },
	{ KM_CHARM, "CHARM", "Charm", "Charm", PRESENCE, GENERAL },
	{ KM_COERC, "COERC", "Coercion", "Coercion", WILLPOWER, GENERAL },
	{ KM_COMP, "COMP", "Computers", "Computers", INTELLECT, GENERAL },
	{ KM_COOL, "COOL", "Cool", "Cool", PRESENCE, GENERAL },
	{ KM_COORD, "COORD", "Coordination", "Coordination", AGILITY, GENERAL },
	{ KM_CORE, "CORE", "Core Worlds", "Core Worlds", INTELLECT, KNOWLEDGE },
	{ KM_DECEP, "DECEP", "Deception", "Deception", CUNNING, GENERAL },
	{ KM_DISC, "DISC", "Discipline", "Discipline", WILLPOWER, GENERAL },
	{ KM_EDU, "EDU", "Education", "Education", INTELLECT, KNOWLEDGE },
	{ KM_GUNN, "GUNN", "Gunnery", "Gunnery", AGILITY, COMBAT },
	{ KM_LEAD, "LEAD", "Leadership", "Leadership", PRESENCE, GENERAL },
	{ KM_LTSABER, "LTSABER", "Lightsaber", "Saber", BRAWN, COMBAT },
	{ KM_LORE, "LORE", "Lore", "Lore", INTELLECT, KNOWLEDGE },
	{ KM_MECH, "MECH", "Mechanics", "Mechanics", INTELLECT, GENERAL },
	{ KM_MED, "MED", "Medicine", "Medicine", INTELLECT, GENERAL },
	{ KM_MELEE, "MELEE", "Melee", "Melee", BRAWN, COMBAT },
	{ KM_NEG, "NEG", "Negotiation", "Negotiation", PRESENCE, GENERAL },
	{ KM_OUT, "OUT", "Outer Rim", "Outer Rim", INTELLECT, KNOWLEDGE },
	{ KM_PERC, "PERC", "Perception", "Perception", CUNNING, GENERAL },
	{ KM_PILOTPL, "PILOTPL", "Piloting - Planetary", "Planetary", AGILITY, GENERAL },
	{ KM_PILOTSP, "PILOTSP", "Piloting - Space", "Space", AGILITY, GENERAL },
	{ KM_RANGHVY, "RANGHVY", "Ranged - Heavy", "Heavy", AGILITY, COMBAT },
	{ KM_RANGLT, "RANGLT", "Ranged - Light", "Light", AGILITY, COMBAT },
	{ KM_RESIL, "RESIL", "Resilience", "Resilience", BRAWN, GENERAL },
	{ KM_SKUL, "SKUL", "Skulduggery", "Skulduggery", CUNNING, GENERAL },
	{ KM_STEAL, "STEAL", "Stealth", "Stealth", AGILITY, GENERAL },
	{ KM_SW, "SW", "Streetwise", "Streetwise", CUNNING, GENERAL },
	{ KM_SURV, "SURV", "Survival", "Survival", CUNNING, GENERAL },
	{ KM_UND, "UND", "Underworld", "Underworld", INTELLECT, KNOWLEDGE },
	{ KM_VIGIL, "VIGIL", "Vigilance", "Vigilance", WILLPOWER, GENERAL },
	{ KM_XEN, "XEN", "Xenology", "Xenology", INTELLECT, KNOWLEDGE },
	{ KM_WARF, "WARF", "Warfare", "Warfare", INTELLECT, KNOWLEDGE },
	{ KM_ICOOL, "ICOOL", "Initiative - Cool", "Init Cool", PRESENCE, SPECIAL },
	{ KM_IVIG, "IVIG", "Initiative - Vigilance", "Init Vig", WILLPOWER, SPECIAL },
	{ KM_DEFM, "DEFM", "Defense - Melee", "Def Melee", "", SPECIAL },
	{ KM_DEFR, "DEFR", "Defense - Ranged", "Def Ranged", "", SPECIAL },
	{ KM_REC, "REC", "Recovery - Cool", "Recovery", PRESENCE, SPECIAL },
	{ KM_FDISC, "FDISC", "Fear - Discipline", "Fear Disc", WILLPOWER, SPECIAL },
	{ KM_FCOOL, "FCOOL", "Fear - Cool", "Fear Cool", PRESENCE, SPECIAL },
	// Track commited force dice, show force pool when click on force rating, subtract committed force dice from other features!

	{ KM_UNKNOWN, NULL, NULL, NULL, 0, 0 }
};

/*
QString Skill::underlyingSkill()
{
	switch (method_id) {
		case KM_ICOOL:
			return "COOL";
		case KM_IVIG:
			return "VIGIL";
		case KM_REC:
			return "COOL";
		case KM_FDISC:
			return "DISC";
		case KM_FCOOL:
			return "COOL";
		default:
			break;
	}
	return key;
}
*/

Skill* Skill::getSkill(const QString& key)
{
	MethodID method_id = KeyMethod::instance.getID(key);
	return getSkill(method_id);
}

Skill* Skill::getSkill(MethodID skill_id)
{
	if (KeyMethod::isSkillID(skill_id))
		return &skills[(int) skill_id];
	return NULL;
}

Skill* Skill::getSkillByName(const QString& skill_name)
{
	QString sk = skill_name;

	if (sk.contains("("))
		sk = DatUtil::left(sk, "(").trimmed();

	const char* ckey = sk.toUtf8().constData();

	for (int i=0; skills[i].key; i++) {
		if (strcmp(skills[i].name, ckey) == 0 ||
			strcmp(skills[i].shortName, ckey) == 0) {
			return &skills[i];
		}
	}
	return NULL;
}

// Skills -------------------------

GeneralSkills GeneralSkills::instance = GeneralSkills();
CombatSkills CombatSkills::instance = CombatSkills();
KnowledgeSkills KnowledgeSkills::instance = KnowledgeSkills();
SpecialSkills SpecialSkills::instance = SpecialSkills();

Skills::Skills(int type) :
	AbstractDataList(QStringList() << "key" << "skill" << "career" << "rank" << "dice")
{
	for (int i=0; skills[i].key; i++) {
		if (skills[i].type == type) {
			iSkillList.append(skills[i]);
		}
	}
}

int Skills::rowCount()
{
	return iSkillList.size();
}

QVariant Skills::getValue(int row, int col)
{
	Skill		skill = iSkillList.at(row);
	CharSkill	char_skill;
	QString		ch;

	char_skill = CurrentData::instance->getCharSkill(skill.method_id);
	switch (col) {
		case 0:
			return skill.key;
		case 1:
			if (ch.isEmpty())
				return skill.name;
			return QString("%1 (%2)").arg(skill.name).arg(DatUtil::capitalize(ch));
		case 2:
			return char_skill.isCareer;
		case 3:
			return char_skill.skillRanks();
		case 4:
			return char_skill.getDicePool();
	}
	return QVariant();
}

void Skills::init()
{
	for (int i=0; skills[i].key; i++) {
		KeyMethod::instance.append(skills[i].key, skills[i].method_id);
	}
}
