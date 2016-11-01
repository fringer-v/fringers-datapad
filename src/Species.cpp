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

#include "Species.h"
#include "DataAccess.h"
#include "Talents.h"
#include "DatUtil.h"
#include "Character.h"
#include "DataSet.h"

// Species -------------------------

QString Species::getName()
{
	QString n = iName;

	if (!iSubName.isEmpty())
		n = n + " (" + iSubName + ")";
	return n;
}

void Species::loadSpecies(const QString& nkey, const QString& sub_nkey, QMap<QString, QString>* choices)
{
	iKey = nkey;
	iSubKey = sub_nkey;
	iChoices = choices;
	iName.clear();
	iSubName.clear();
	iSubKeyMatched = false;

	QString file_name = DataSet::instance.species[nkey];
	QByteArray data = DataAccess::getData(Character::instance->dataSet(), "/Species/"+file_name);
	readFromBuffer(data.constData(), data.length());
}

void Species::end()
{
}

bool Species::xmlElement(const DatStringBuffer& path, const char* value)
{
	//qDebug() << "XML" << path << value;
	if (path.endsWith("/Species/Name/"))
		iName = value;
	else if (path.endsWith("/SubSpecies/Key/"))
		iSubKeyMatched = iSubKey == value;
	else if (path.endsWith("/SubSpecies/Name/")) {
		if (iSubKeyMatched)
			iSubName = value;
		iSubKeyMatched = false;
	}

	else if (path.endsWith("/TalentModifier/Key/")) {
		iCharTalent.clear(value);
		iCharTalent.aquisition = iName;
	}
	else if (path.endsWith("/TalentModifier/RankAdd/"))
		iCharTalent.ranks = toInt(value);
	else if (path.endsWith("/TalentModifier/#end")) {
		Character::instance->talents.addTalent(iCharTalent);
	}

	else if (path.endsWith("/OptionChoice/Key/")) {
		iChoiceKey = value;
		iOptionKey.clear();
	}
	else if (path.endsWith("/OptionChoice/Name/"))
		iOptionTitle = value;
	else if (path.endsWith("/Options/Option/Key/")) {
		iOptionKey = value;
		iOptionSubTitle.clear();
		iOptionContent.clear();
		iDieMod.clear();
		iDieModList.clear();
	}
	else if (path.endsWith("/Options/Option/Name/"))
		iOptionSubTitle = value;
	else if (path.endsWith("/Options/Option/Description/"))
		iOptionContent = value;

	else if (path.endsWith("/Options/Option/DieModifiers/DieModifier/#open"))
		iDieMod.clear();
	else if (path.endsWith("/Options/Option/DieModifiers/DieModifier/SkillKey/"))
		iDieMod.skillKey = value;
	else if (path.endsWith("/Options/Option/DieModifiers/DieModifier/SkillType/"))
		iDieMod.skillKey = (value == "Knowledge") ? KNOWLEDGE : NO_SKILL_TYPE;
	else if (path.endsWith("/Options/Option/DieModifiers/DieModifier/BoostCount/"))
		iDieMod.boostCount = toInt(value);
	else if (path.endsWith("/Options/Option/DieModifiers/DieModifier/SetbackCount/"))
		iDieMod.setbackCount = toInt(value);
	else if (path.endsWith("/Options/Option/DieModifiers/DieModifier/ForceCount/"))
		iDieMod.forceCount = toInt(value);
	else if (path.endsWith("/Options/Option/DieModifiers/DieModifier/AdvantageCount/"))
		iDieMod.advantageCount = toInt(value);
	else if (path.endsWith("/Options/Option/DieModifiers/DieModifier/ThreatCount/"))
		iDieMod.threatCount = toInt(value);
	else if (path.endsWith("/Options/Option/DieModifiers/DieModifier/AddSetbackCount/"))
		iDieMod.addSetbackCount = toInt(value);
	else if (path.endsWith("/Options/Option/DieModifiers/DieModifier/UpgradeAbilityCount/"))
		iDieMod.upgradeCount = toInt(value);
	else if (path.endsWith("/Options/Option/DieModifiers/DieModifier/#end"))
		iDieModList.addMod(iDieMod);

	else if (path.endsWith("/Options/Option/#end")) {
		if (!iOptionKey.isEmpty()) {
			if (iChoices->value(iChoiceKey) == iOptionKey) {
				iOptionsRow = DataList::speciesFeatures.appendRow();
				DataList::speciesFeatures.setValue(iOptionsRow, "title", iOptionTitle);
				DataList::speciesFeatures.setValue(iOptionsRow, "subtitle", iOptionSubTitle);
				DataList::speciesFeatures.setValue(iOptionsRow, "content", iOptionContent);

				if (iDieModList.count() > 0) {
					Talent t;
					t.clear(iOptionKey);
					t.dieModList = iDieModList;
					AllTalents::instance()->addTalent(t);
				}

				iCharTalent.clear(iOptionKey);
				iCharTalent.aquisition = iName;
				iCharTalent.talentType = TALENT_SPECIES;
				Character::instance->talents.addTalent(iCharTalent);
			}
		}
		iOptionKey.clear();
	}


	else if (path.endsWith("/WeaponModifiers/WeaponModifier/#open"))
		iSpecTalent.clear();
	else if (path.endsWith("/WeaponModifiers/WeaponModifier/AllSkillKey/"))
		iSpecTalent.damageBonusSkill = value;
	else if (path.endsWith("/WeaponModifiers/WeaponModifier/DamageAdd/"))
		iSpecTalent.damageBonus = toInt(value);
	else if (path.endsWith("/WeaponModifiers/WeaponModifier/Crit/"))
		iSpecTalent.weaponsCrit = toInt(value);
	else if (path.endsWith("/WeaponModifiers/WeaponModifier/#end"))
		Character::instance->speciesTalents.append(iSpecTalent);

	return true;
}
