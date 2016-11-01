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

#ifndef __Skills_h__
#define __Skills_h__

#include "DatXML.h"
#include "Item.h"
#include "DataList.h"

#define NO_SKILL_TYPE	-1
#define GENERAL			0
#define COMBAT			1
#define KNOWLEDGE		2
#define SPECIAL			3

#define BRAWN		"BR"
#define AGILITY		"AG"
#define INTELLECT	"INT"
#define CUNNING		"CUN"
#define WILLPOWER	"WIL"
#define PRESENCE	"PR"
#define DEFENSE		"DE"

struct Skill {
	const char* key;
	const char* name;
	const char* shortName;
	const char* characteristic;
	int type;

	static Skill* getSkill(const QString& key);
	static Skill* getSkillByName(const QString& skill_name);
};

class Skills : public AbstractDataList {
public:
	Skills(int type);

	virtual int rowCount();
	virtual QVariant getValue(int row, int col);

private:
	QList<Skill> iSkillList;
};

class GeneralSkills : public Skills {
public:
	static GeneralSkills instance;

	GeneralSkills() :
		Skills(GENERAL) {
	}
};

class CombatSkills : public Skills {
public:
	static CombatSkills instance;

	CombatSkills() :
		Skills(COMBAT) {
	}
};

class KnowledgeSkills : public Skills {
public:
	static KnowledgeSkills instance;

	KnowledgeSkills() :
		Skills(KNOWLEDGE) {
	}
};

class SpecialSkills : public Skills {
public:
	static SpecialSkills instance;

	SpecialSkills() :
		Skills(SPECIAL) {
	}
};

#endif // __Skills_h__
