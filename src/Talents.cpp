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

#include "Talents.h"
#include "DataAccess.h"
#include "DatUtil.h"
#include "Character.h"

QPointer<AllTalents> AllTalents::sInstance;

// CharTalentMap -------------------------

CharTalent CharTalentMap::iEmpty;
QStringList* CharTalentMap::iCodedTalents;

CharTalentMap::CharTalentMap()
{
	if (iCodedTalents == NULL)
		iCodedTalents = new QStringList();
	iCodedTalents->append("ARM");
	iCodedTalents->append("ARMIMP");
	iCodedTalents->append("DEADACC");
	iCodedTalents->append("DEDI");
	iCodedTalents->append("GRIT");
	iCodedTalents->append("JURY");
	iCodedTalents->append("LETHALBL");
	iCodedTalents->append("PHYSTRAIN");
	iCodedTalents->append("SKILLJOCK");
	iCodedTalents->append("TINK");
	iCodedTalents->append("TOUGH");
	iCodedTalents->append("KILL");
	iCodedTalents->append("INTIM");
	iCodedTalents->append("CONGENIAL");
	iCodedTalents->append("TRUEAIM");
	iCodedTalents->append("QUICKST");
	iCodedTalents->append("PRECAIM");
	iCodedTalents->append("TARGBL");
	iCodedTalents->append("SNIPSHOT");
	iCodedTalents->append("UNCANREAC");
	iCodedTalents->append("STRSMART");
	iCodedTalents->append("INTENSFOC");
	iCodedTalents->append("SIXSENSE");
	iCodedTalents->append("FORCERAT");
	iCodedTalents->append("BAL");
	iCodedTalents->append("DODGE");
	iCodedTalents->append("DISARMSMILE");
	iCodedTalents->append("TOUCH");
	iCodedTalents->append("INSIGHT");
	iCodedTalents->append("MASPIL");
	iCodedTalents->append("FULLTH");
	iCodedTalents->append("FULLTHIMP");
	iCodedTalents->append("FULLTHSUP");
	iCodedTalents->append("SLEIGHTMIND");
	iCodedTalents->append("HAWKSWOOP");
	iCodedTalents->append("UNCANSENS");
	iCodedTalents->append("PARRY");
	iCodedTalents->append("PARRYIMP");
	iCodedTalents->append("RAPREA");
	iCodedTalents->append("REFLECT");
	iCodedTalents->append("REFLECTIMP");
	iCodedTalents->append("SABERSW");
	iCodedTalents->append("RAINDEATH");
	iCodedTalents->append("ENDUR");
	iCodedTalents->append("HEAVYHITTER");
	iCodedTalents->append("BRA");
	iCodedTalents->append("BURLY");
	iCodedTalents->append("BAR");
	iCodedTalents->append("FERSTR");
	iCodedTalents->append("SENSDANG");
	iCodedTalents->append("POINTBL");
	iCodedTalents->append("SIDESTEP");
	iCodedTalents->append("DEFSTA");
	iCodedTalents->append("DURA");
	iCodedTalents->append("SENSEADV");
	iCodedTalents->append("KEENEYED");

	iCodedTalents->append("SENSECONTROL1");
	iCodedTalents->append("SENSECONTROL3");
	iCodedTalents->append("SENSESTRENGTH");
	iCodedTalents->append("SENSEDURATION");

	iCodedTalents->append("INFLUENCECONTROL1");
	iCodedTalents->append("INFLUENCECONTROL2");
	iCodedTalents->append("INFLUENCERANGE");
	iCodedTalents->append("INFLUENCEMAGNITUDE");
	iCodedTalents->append("INFLUENCESTRENGTH");
	iCodedTalents->append("INFLUENCEDURATION");

	iCodedTalents->append("ATARU");

	iCodedTalents->append("ENHANCEBASIC");
	iCodedTalents->append("ENHANCERANGE");
	iCodedTalents->append("ENHANCECONT0");
	iCodedTalents->append("ENHANCECONT1");
	iCodedTalents->append("ENHANCECONT2");
	iCodedTalents->append("ENHANCECONT3");
	iCodedTalents->append("ENHANCECONT4");
	iCodedTalents->append("ENHANCECONT5");
	//iCodedTalents->append("ENHANCECONT6");
	iCodedTalents->append("ENHANCECONT7");
	iCodedTalents->append("ENHANCECONT8");
	iCodedTalents->append("ENHANCECONT9");

	iCodedTalents->append("FORSEECONTROL1");
	iCodedTalents->append("FORSEECONTROL3");

	iCodedTalents->append("MOVEBASIC");
	iCodedTalents->append("MOVECONTROL1");
	iCodedTalents->append("MOVECONTROL2");
	iCodedTalents->append("MOVECONTROL3");
	iCodedTalents->append("MOVESTRENGTH");
	iCodedTalents->append("MOVERANGE");
	iCodedTalents->append("MOVEMAGNITUDE");

	iCodedTalents->append("MISDIRRANGE");
	iCodedTalents->append("MISDIRMAGNITUDE");
	iCodedTalents->append("MISDIRSTRENGTH");
	iCodedTalents->append("MISDIRCONTROL3");
	iCodedTalents->append("MISDIRDURATION");
}

int CharTalentMap::size(bool exclude_hidden)
{
	fillArrays();
	if (exclude_hidden)
		return iDisplayNonCoded.size();
	return iDisplayNonSpecies.size();
}

bool CharTalentMap::contains(const QString& key)
{
	return iTalents.contains(key);
}

int CharTalentMap::ranks(const QString& key)
{
	if (iTalents.contains(key))
		return iTalents[key].ranks;
	return 0;
}

QString CharTalentMap::range(const QString& key, int ranks)
{
	// Misdirect starts at short range
	if (key.startsWith("MISDIR"))
		ranks++;
	switch (ranks) {
		case 0: return "Engaged";
		case 1: return "Short range";
		case 2: return "Medium range";
		case 3: return "Long range";
	}
	return "Extreme range";
}

QString CharTalentMap::duration(const QString& key, int ranks)
{
	int r = ranks + 1;

	return QString("%1 Round%2 (%3 min)").arg(r).arg(r==1 ? "" : "s").arg(5*r);
}

QString CharTalentMap::magnitude(const QString& key, int ranks)
{
	if (key.startsWith("MISDIR"))
		ranks = ranks*Character::instance->presence();
	else
		ranks++;
	return QString("%1 Target%2").arg(ranks).arg(ranks==1 ? "" : "s");
}

QString CharTalentMap::forceUpgrades(const QString& key, int show, int cost)
{
	QString power;
	QString upgrades;
	QString costs;
	CharTalent t;

	if (key.startsWith("INFLUENCE"))
		power = "INFLUENCE";
	else if (key.startsWith("ENHANCE"))
		power = "ENHANCE";
	else if (key.startsWith("FORESEE"))
		power = "FORESEE";
	else if (key.startsWith("MOVE"))
		power = "MOVE";
	else if (key.startsWith("MISDIR"))
		power = "MISDIR";

	if (show & STR) {
		t = get(power + "STRENGTH");
		if (t.ranks > 0) {
			if (power == "INFLUENCE")
				upgrades = QString("Strain x2").arg(t.ranks);
			else if (power == "MOVE")
				upgrades = QString("Silhouette %1").arg(t.ranks);
			else if (power == "MISDIR")
				upgrades = QString("Silhouette %1").arg(t.ranks+1);
			else
				upgrades = QString("Strength +%1").arg(t.ranks);
		}
	}

	if (show & RAN) {
		t = get(power + "RANGE");
		if (t.ranks > 0)
			DatUtil::appendToList(costs, range(power + "RANGE", t.ranks), " or ");
	}

	if (show & MAG) {
		t = get(power + "MAGNITUDE");
		if (t.ranks > 0)
			DatUtil::appendToList(costs, magnitude(power + "MAGNITUDE", t.ranks), " or ");
	}

	if (show & DUR) {
		t = get(power + "DURATION");
		if (t.ranks > 0)
			DatUtil::appendToList(costs, duration(power + "DURATION", t.ranks), " or ");
	}

	if (!costs.isEmpty())
		costs = QString("spend %1 for ").arg(DatUtil::repeat("[LI]", cost))+costs;

	DatUtil::appendToList(upgrades, costs, ", ");

	return upgrades;
}

void CharTalentMap::clear()
{
	iTalents.clear();
	iDisplayNonSpecies.clear();
	iDisplayNonCoded.clear();
	iLightSaberChars.clear();
}

void CharTalentMap::addTalent(CharTalent talent)
{
	if (iCodedTalents->contains(talent.key))
		talent.talentType = TALENT_CODED;
	if (iTalents.contains(talent.key)) {
		iTalents[talent.key].ranks++;
		DatUtil::appendToList(iTalents[talent.key].aquisition, talent.aquisition, ", ");
	}
	else {
		iTalents[talent.key] = talent;
		iDisplayNonSpecies.clear();
		iDisplayNonCoded.clear();
	}
	Talent tdata = AllTalents::instance()->getTalent(talent.key);
	if (!tdata.lightSaberSkill.isEmpty())
		iLightSaberChars.append(tdata.lightSaberSkill);
}

CharTalent& CharTalentMap::at(int i, bool exclude_hidden)
{
	fillArrays();

	if (i < 0 || i >= iTalents.size()) {
		iEmpty.clear();
		return iEmpty;
	}

	if (exclude_hidden)
		return iTalents[iDisplayNonCoded.at(i)];
	return iTalents[iDisplayNonSpecies.at(i)];
}

CharTalent& CharTalentMap::get(const QString& key)
{
	if (iTalents.contains(key))
		return iTalents[key];
	iEmpty.clear();
	iEmpty.ranks = 0;
	return iEmpty;
}

QString CharTalentMap::getLightSaberChar()
{
	Character* character = Character::instance;
	QString ch = BRAWN;
	int ranks = character->brawn();
	foreach (QString lch, iLightSaberChars) {
		if (character->getAttribute(lch) >= ranks) {
			ch = lch;
			ranks = character->getAttribute(lch);
		}
	}
	return ch;
}

void CharTalentMap::fillArrays()
{
	if (iDisplayNonSpecies.isEmpty()) {
		for (QMap<QString, CharTalent>::iterator i = iTalents.begin(); i != iTalents.end(); i++) {
			if (i.value().talentType != TALENT_SPECIES)
				iDisplayNonSpecies.append(i.key());

			if (i.value().talentType == TALENT_REGULAR)
				iDisplayNonCoded.append(i.key());
		}
	}
}

// Talent -------------------------

void Talent::clear(QString k)
{
	key = k;
	name.clear();
	activation.clear();
	description.clear();
	books.clear();
	ranked = false;
	force = false;
	addDamagePerSkillRank = false;
	damageBonus = 0;
	damageBonusSkills.clear();
	dieModifiers.clear();
	lightSaberSkill.clear();
	soakValue = 0;
	defenseRanged = 0;
	defenseMelee = 0;
	requirementWearingArmor = false;
	requirementSoakAtLeast = 0;
	burly = 0;
}

// AllTalents -------------------------

AllTalents* AllTalents::instance()
{
	if (sInstance.isNull())
		sInstance = new AllTalents;
	return sInstance;
}

void AllTalents::loadTalents(const QString& dset)
{
	QByteArray	data;
	QString		path;

	iTalents.clear();
	path = DatUtil::getSystemDataFolder() + "Data/Talents.xml";
	data = DataAccess::readFile(path);
	readFromBuffer(data.constData(), data.length());

	path = DatUtil::getSystemDataFolder() + "Data/Force Abilities.xml";
	data = DataAccess::readFile(path);
	readFromBuffer(data.constData(), data.length());

	if (!dset.isEmpty()) {
		path = DatUtil::getDataSetFolder() + dset + "/Talents.xml";
		if (QFile::exists(path)) {
			data = DataAccess::readFile(path);
			readFromBuffer(data.constData(), data.length());
		}

		path = DatUtil::getDataSetFolder() + dset + "/Force Abilities.xml";
		if (QFile::exists(path)) {
			data = DataAccess::readFile(path);
			readFromBuffer(data.constData(), data.length());
		}
	}
}

bool AllTalents::xmlElement(const DatStringBuffer& path, const char* value)
{
	// qDebug() << "XML" << path.getCString() << value;
	if (path.endsWith("/Talent/Key/"))
		iTalent.clear(value);
	else if (path.endsWith("/Talent/Name/"))
		iTalent.name = value;
	else if (path.endsWith("/Talent/Description/")) {
		iTalent.description = value; // value.trimmed();
		if (iTalent.description.startsWith("Please see page")) {
			iTalent.books = DatUtil::pageSummary(iTalent.description);
			iTalent.description.clear();
		}
	}
	else if (path.endsWith("/Talent/Activation/"))
		iTalent.activation = value;
	else if (path.endsWith("/Talent/Ranked/"))
		iTalent.ranked = isTrue(value);
	else if (path.endsWith("/Talent/#end")) {
		iTalents[iTalent.key] = iTalent;
	}

	// Damage Bonus
	else if (path.endsWith("/Damage/Bonus/"))
		iTalent.damageBonus = toInt(value);
	else if (path.endsWith("/Damage/Skills/Key/"))
		DatUtil::appendToList(iTalent.damageBonusSkills, value, ",");

	// Die modifiers
	else if (path.endsWith("/DieModifier/SkillKey/"))
		iDieModifier.clear(value);
	else if (path.endsWith("/DieModifier/SetbackCount/"))
		iDieModifier.setbackCount = toInt(value);
	else if (path.endsWith("/DieModifier/ForceCount/"))
		iDieModifier.forceCount = toInt(value);
	else if (path.endsWith("/DieModifier/BoostCount/"))
		iDieModifier.boostCount = toInt(value);
	else if (path.endsWith("/DieModifier/#end"))
		iTalent.dieModifiers.append(iDieModifier);

	else if (path.endsWith("/SkillChars/SkillChar/CharKey/"))
		iTalent.lightSaberSkill = value;
	else if (path.endsWith("/SkillChoice/AddDamagePerSkillRank/"))
		iTalent.addDamagePerSkillRank = isTrue(value);

	else if (path.endsWith("/Attributes/SoakValue/"))
		iTalent.soakValue = toInt(value);
	else if (path.endsWith("/Attributes/DefenseRanged/"))
		iTalent.defenseRanged = toInt(value);
	else if (path.endsWith("/Attributes/DefenseMelee/"))
		iTalent.defenseMelee = toInt(value);
	else if (path.endsWith("/Attributes/Requirement/WearingArmor/"))
		iTalent.requirementWearingArmor = isTrue(value);
	else if (path.endsWith("/Attributes/Requirement/SoakAtLeast/"))
		iTalent.requirementSoakAtLeast = toInt(value);

	else if (path.endsWith("/ItemChange/EncumChange/"))
		iTalent.burly = toInt(value) * -1;

	// Force abilities:
	else if (path.endsWith("/ForceAbility/Key/")) {
		iTalent.clear(value);
		iTalent.force = true;
	}
	else if (path.endsWith("/ForceAbility/Name/"))
		iTalent.name = value;
	else if (path.endsWith("/ForceAbility/Description/")) {
		iTalent.description = value; // value.trimmed();
		if (iTalent.description.startsWith("Please see page")) {
			iTalent.books = DatUtil::pageSummary(iTalent.description);
			iTalent.description.clear();
		}
	}
	else if (path.endsWith("/ForceAbility/#end")) {
		iTalents[iTalent.key] = iTalent;
	}

	// DEADACC
	return true;
}

Talent AllTalents::getTalent(const QString& key)
{
	return iTalents.value(key);
}

void AllTalents::addTalent(const Talent& t)
{
	iTalents[t.key] = t;
}


// Talents -------------------------

Talents Talents::instance = Talents();

Talents::Talents() :
	AbstractDataList(QStringList() << "hidden" << "talent" << "ranks" << "activation"
		<< "acquisition" << "description")
{
}

int Talents::rowCount()
{
	bool exclude_hidden = Character::instance->currentData()->hideAutoTalents;
	return Character::instance->talents.size(exclude_hidden);
}

QVariant Talents::getValue(int row, int col)
{
	bool exclude_hidden = Character::instance->currentData()->hideAutoTalents;
	CharTalent& char_talent = Character::instance->talents.at(row, exclude_hidden);
	Talent talent = AllTalents::instance()->getTalent(char_talent.key);

	switch (col) {
		case 0:
			return char_talent.talentType;
		case 1:
			if (char_talent.key == "ENHANCECONT6")
				return "Enhance: Force Leap";
			return talent.name.isEmpty() ? char_talent.key : talent.name;
		case 2:
			return talent.ranked ? char_talent.ranks : 0;
		case 3:
			return talent.activation;
		case 4:
			if (talent.force)
				return "FORCE";
			return char_talent.aquisition;
		case 5: {
			QString desc;

			if (char_talent.selectedSkills.count() > 0) {
				QString list;
				foreach (QString skill_key, char_talent.selectedSkills) {
					DatUtil::appendToList(list, Skill::getSkill(skill_key)->name, ", ");
				}
				desc = QString("[B]Selected Skills:[b] ") + list;
			}

			if (!char_talent.selectedCharacteristics.isEmpty())
				DatUtil::appendToList(desc, QString("[B]Selected Characteristics:[b] ") + char_talent.selectedCharacteristics);

			DatUtil::addDescription(desc, talent.description, talent.books);

			if (char_talent.key == "INFLUENCEBASIC" || char_talent.key == "MOVEBASIC" ||
				char_talent.key == "MOVECONTROL1" || char_talent.key == "MOVECONTROL2" ||
				char_talent.key == "MOVECONTROL3" || char_talent.key == "MOVECONTROL4"
				)
				DatUtil::appendToList(desc, Character::instance->talents.forceUpgrades("INFLUENCEBASIC"), " [B]Upgrades:[b] ");
			else if (char_talent.key == "INFLUENCECONTROL1")
				DatUtil::appendToList(desc, Character::instance->talents.forceUpgrades("INFLUENCECONTROL1", RAN | MAG | DUR), " [B]Upgrades:[b] ");
			else if (char_talent.key == "ENHANCECONT6") {
				QString	text;
				QString action = "an Action";
				QString type = "horizontally";
				QString range = "Short";

				if (Character::instance->talents.contains("ENHANCECONT3"))
					type = "horizontally or vertically";
				if (Character::instance->talents.contains("ENHANCECONT0"))
					action = "a Maneuver";
				if (Character::instance->talents.contains("ENHANCERANGE"))
					range = "Medium";
				text = QString("As %1 jump %2 to any location in %3 range").arg(action).arg(type).arg(range);
				desc.clear();
				DatUtil::addDescription(desc, text, talent.books);
			}
			else if (char_talent.key == "MISDIRBASIC") {
				DatUtil::appendToList(desc, Character::instance->talents.forceUpgrades("MISDIRBASIC", RAN | MAG), " [B]Upgrades:[b] ");
				DatUtil::appendToList(desc, Character::instance->talents.forceUpgrades("MISDIRBASIC", STR, 2), ", ");
			}

			return desc;
		}
	}
	return QVariant();
}

/*
	// Talents:
	for (QMap<QString, CharTalent>::iterator i = Character::instance->talents.begin(); i != Character::instance->talents.end(); i++) {
		AllTalents::instance()->addCharacterTalent(i.value());
	}

void AllTalents::addCharacterTalent(CharTalent char_talent)
{
	Talent talent = iTalents.value(char_talent.key);
	int row;

	if (DataList::talents.findSortedRow(row, "talent", talent.name)) {
		qDebug() << "SHOULD NOT HAPPEN";
	}
	else {

		DataList::talents.insertRow(row);
		DataList::talents.setValue(row, "talent", talent.name.isEmpty() ? char_talent.key : talent.name);
		DataList::talents.setValue(row, "ranks", talent.ranked ? char_talent.ranks : 0);
		DataList::talents.setValue(row, "activation", talent.activation);
		DataList::talents.setValue(row, "acquisition", char_talent.aquisition);
		DataList::talents.setValue(row, "description", desc);
	}
}

*/
