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
	{ "ASTRO", "Astrogation", "Astrogation", INTELLECT, GENERAL },
	{ "ATHL", "Athletics", "Athletics", BRAWN, GENERAL },
	{ "BRAWL", "Brawl", "Brawl", BRAWN, COMBAT },
	{ "CHARM", "Charm", "Charm", PRESENCE, GENERAL },
	{ "COERC", "Coercion", "Coercion", WILLPOWER, GENERAL },
	{ "COMP", "Computers", "Computers", INTELLECT, GENERAL },
	{ "COOL", "Cool", "Cool", PRESENCE, GENERAL },
	{ "COORD", "Coordination", "Coordination", AGILITY, GENERAL },
	{ "CORE", "Core Worlds", "Core Worlds", INTELLECT, KNOWLEDGE },
	{ "DECEP", "Deception", "Deception", CUNNING, GENERAL },
	{ "DISC", "Discipline", "Discipline", WILLPOWER, GENERAL },
	{ "EDU", "Education", "Education", INTELLECT, KNOWLEDGE },
	{ "GUNN", "Gunnery", "Gunnery", AGILITY, COMBAT },
	{ "LEAD", "Leadership", "Leadership", PRESENCE, GENERAL },
	{ "LTSABER", "Lightsaber", "Saber", BRAWN, COMBAT },
	{ "LORE", "Lore", "Lore", INTELLECT, KNOWLEDGE },
	{ "MECH", "Mechanics", "Mechanics", INTELLECT, GENERAL },
	{ "MED", "Medicine", "Medicine", INTELLECT, GENERAL },
	{ "MELEE", "Melee", "Melee", BRAWN, COMBAT },
	{ "NEG", "Negotiation", "Negotiation", PRESENCE, GENERAL },
	{ "OUT", "Outer Rim", "Outer Rim", INTELLECT, KNOWLEDGE },
	{ "PERC", "Perception", "Perception", CUNNING, GENERAL },
	{ "PILOTPL", "Piloting - Planetary", "Planetary", AGILITY, GENERAL },
	{ "PILOTSP", "Piloting - Space", "Space", AGILITY, GENERAL },
	{ "RANGHVY", "Ranged - Heavy", "Heavy", AGILITY, COMBAT },
	{ "RANGLT", "Ranged - Light", "Light", AGILITY, COMBAT },
	{ "RESIL", "Resilience", "Resilience", BRAWN, GENERAL },
	{ "SKUL", "Skulduggery", "Skulduggery", CUNNING, GENERAL },
	{ "STEAL", "Stealth", "Stealth", AGILITY, GENERAL },
	{ "SW", "Streetwise", "Streetwise", CUNNING, GENERAL },
	{ "SURV", "Survival", "Survival", CUNNING, GENERAL },
	{ "UND", "Underworld", "Underworld", INTELLECT, KNOWLEDGE },
	{ "VIGIL", "Vigilance", "Vigilance", WILLPOWER, GENERAL },
	{ "XEN", "Xenology", "Xenology", INTELLECT, KNOWLEDGE },
	{ "WARF", "Warfare", "Warfare", INTELLECT, KNOWLEDGE },

	{ "ICOOL", "Initiative - Cool", "Init Cool", PRESENCE, SPECIAL },
	{ "IVIG", "Initiative - Vigilance", "Init Vig", WILLPOWER, SPECIAL },
	{ "DEFM", "Defense - Melee", "Def Melee", "", SPECIAL },
	{ "DEFR", "Defense - Ranged", "Def Ranged", "", SPECIAL },
	{ "REC", "Recovery - Cool", "Recovery", PRESENCE, SPECIAL },
	{ "FDISC", "Fear - Discipline", "Fear Disc", WILLPOWER, SPECIAL },
	{ "FCOOL", "Fear - Cool", "Fear Cool", PRESENCE, SPECIAL },
	// Track commited force dice, show force pool when click on force rating, subtract committed force dice from other features!

	{ NULL, NULL, NULL, 0, 0 }
};

Skill* Skill::getSkill(const QString& key)
{
	const char* ckey = key.toUtf8().constData();

	for (int i=0; skills[i].key; i++) {
		if (strcmp(skills[i].key, ckey) == 0) {
			return &skills[i];
		}
	}
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
		if (skills[i].type == type)
			iSkillList.append(skills[i]);
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
	QString		key;

	ch = skill.characteristic;

	if (strcmp(skill.key, "ICOOL") == 0)
		key = "COOL";
	else if (strcmp(skill.key, "IVIG") == 0)
		key = "VIGIL";
	else if (strcmp(skill.key, "DEFM") == 0)
		char_skill.key = "DEFM";
	else if (strcmp(skill.key, "DEFR") == 0)
		char_skill.key = "DEFR";
	else if (strcmp(skill.key, "REC") == 0)
		key = "COOL";
	else if (strcmp(skill.key, "FDISC") == 0)
		key = "DISC";
	else if (strcmp(skill.key, "FCOOL") == 0)
		key = "COOL";
	else {
		key = skill.key;
		if (strcmp(skill.key, "LTSABER") == 0)
			ch = Character::instance->talents.getLightSaberChar();
	}

	if (Character::instance->skills.contains(key))
		char_skill = Character::instance->skills[key];

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
			return char_skill.ranks;
		case 4:
			return char_skill.getDicePool(&skill, ch);
	}
	return QVariant();
}
