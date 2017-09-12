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
#include "KeyMethods.h"

QPointer<AllTalents> AllTalents::sInstance;

// CharTalentMap -------------------------

CharTalent CharTalentMap::iEmpty;

CharTalentMap::CharTalentMap()
{
	// Implemented Talents:
	KeyMethod::instance.append("BINDBASIC", KM_BINDBASIC);
	KeyMethod::instance.append("BRI", KM_BRI);
	KeyMethod::instance.append("INFLUENCEBASIC", KM_INFLUENCEBASIC);
	KeyMethod::instance.append("INFLUENCECONTROL1", KM_INFLUENCECONTROL1);
	KeyMethod::instance.append("MISDIRBASIC", KM_MISDIRBASIC);
	KeyMethod::instance.append("MISDIRCONTROL1", KM_MISDIRCONTROL1);
	KeyMethod::instance.append("MISDIRCONTROL2", KM_MISDIRCONTROL2);
	KeyMethod::instance.append("MISDIRMASTERY", KM_MISDIRMASTERY);
	KeyMethod::instance.append("SENSEBASIC", KM_SENSEBASIC);
	KeyMethod::instance.append("SENSECONTROL2", KM_SENSECONTROL2);
	KeyMethod::instance.append("DONTSHOOT", KM_DONTSHOOT);
	KeyMethod::instance.append("FEARSOME", KM_FEARSOME);
	KeyMethod::instance.append("MOVEBASIC", KM_MOVEBASIC);
	KeyMethod::instance.append("MOVECONTROL1", KM_MOVECONTROL1);
	KeyMethod::instance.append("MOVECONTROL2", KM_MOVECONTROL2);
	KeyMethod::instance.append("MOVECONTROL3", KM_MOVECONTROL3);
	KeyMethod::instance.append("TECHAPT", KM_TECHAPT);
	KeyMethod::instance.append("INDIS", KM_INDIS);
	KeyMethod::instance.append("ENHANCECONT6", KM_ENHANCECONT6);
	KeyMethod::instance.append("QUICKDR", KM_QUICKDR);
	KeyMethod::instance.append("TRICK", KM_TRICK);
	KeyMethod::instance.append("WARFORBASIC", KM_WARFORBASIC);
	KeyMethod::instance.append("WARFORCONTROL1", KM_WARFORCONTROL1);
	KeyMethod::instance.append("JUSTKID", KM_JUSTKID);
	KeyMethod::instance.append("HERO", KM_HERO);
	KeyMethod::instance.append("HEROICRES", KM_HEROICRES);
	KeyMethod::instance.append("INTUITEVA", KM_INTUITEVA);
	KeyMethod::instance.append("NATBLADE", KM_NATBLADE);
	KeyMethod::instance.append("SEEKBASICPOWER", KM_SEEKBASIC);
	KeyMethod::instance.append("SEEKCONTROL3", KM_SEEKCONTROL3);
	KeyMethod::instance.append("SEEKSTRENGTH", KM_SEEKSTRENGTH);
	KeyMethod::instance.append("SEEKCONTROL1", KM_SEEKCONTROL1);
	KeyMethod::instance.append("SEEKMASTERY", KM_SEEKMASTERY);
	KeyMethod::instance.append("SEEKDURATION", KM_SEEKDURATION);
	KeyMethod::instance.append("BATMEDBASIC", KM_BATMEDBASIC);
	KeyMethod::instance.append("SUPPRESSBASIC", KM_SUPPRESSBASIC);
	KeyMethod::instance.append("SUPPRESSCONTROL1", KM_SUPPRESSCONTROL1);
	KeyMethod::instance.append("SUPPRESSCONTROL2", KM_SUPPRESSCONTROL2);
	KeyMethod::instance.append("SUPPRESSMASTERY", KM_SUPPRESSMASTERY);
	KeyMethod::instance.append("FARSIGHTBASIC", KM_FARSIGHTBASIC);
	KeyMethod::instance.append("FARSIGHTCONTROL1", KM_FARSIGHTCONTROL1);
	KeyMethod::instance.append("FARSIGHTCONTROL1", KM_FARSIGHTCONTROL1);
	KeyMethod::instance.append("FARSIGHTCONTROL2", KM_FARSIGHTCONTROL2);
	KeyMethod::instance.append("FARSIGHTCONTROL5", KM_FARSIGHTCONTROL5);
	KeyMethod::instance.append("FARSIGHTCONTROL5", KM_FARSIGHTCONTROL5);
	KeyMethod::instance.append("FARSIGHTCONTROL6", KM_FARSIGHTCONTROL6);
	KeyMethod::instance.append("FARSIGHTMASTERY", KM_FARSIGHTMASTERY);
	KeyMethod::instance.append("FORSEEBASIC", KM_FORSEEBASIC);

	// Coded Talents (must be last),
	KeyMethod::instance.append("NATMYSTIC", KM_NATMYSTIC);
	KeyMethod::instance.append("FORSEECONTROL1", KM_FORSEECONTROL1);
	KeyMethod::instance.append("FORSEECONTROL2", KM_FORSEECONTROL2);
	KeyMethod::instance.append("FORSEECONTROL3", KM_FORSEECONTROL3);
	KeyMethod::instance.append("FARSIGHTCONTROL3", KM_FARSIGHTCONTROL3);
	KeyMethod::instance.append("FARSIGHTCONTROL4", KM_FARSIGHTCONTROL4);
	KeyMethod::instance.append("FORCEALLY", KM_FORCEALLY);
	KeyMethod::instance.append("FORAG", KM_FORAG);
	KeyMethod::instance.append("EXTRACK", KM_EXTRACK);
	KeyMethod::instance.append("NOWYOUSEE", KM_NOWYOUSEE);
	KeyMethod::instance.append("SUPPRESSDURATION", KM_SUPPRESSDURATION);
	KeyMethod::instance.append("SUPPRESSCONTROL3", KM_SUPPRESSCONTROL3);
	KeyMethod::instance.append("MASSHAD", KM_MASSHAD);
	KeyMethod::instance.append("BATMEDCONTROL1", KM_BATMEDCONTROL1);
	KeyMethod::instance.append("BATMEDCONTROL2", KM_BATMEDCONTROL2);
	KeyMethod::instance.append("BATMEDMASTERY", KM_BATMEDMASTERY);
	KeyMethod::instance.append("BATMEDDURATION", KM_BATMEDDURATION);
	KeyMethod::instance.append("IRONBODY", KM_IRONBODY);
	KeyMethod::instance.append("ANAT", KM_ANAT);
	KeyMethod::instance.append("PLAUSDEN", KM_PLAUSDEN);
	KeyMethod::instance.append("CRIPV", KM_CRIPV);
	KeyMethod::instance.append("PREYWEAK", KM_PREYWEAK);
	KeyMethod::instance.append("SEEKCONTROL2", KM_SEEKCONTROL2);
	KeyMethod::instance.append("SEEKMAGNITUDE", KM_SEEKMAGNITUDE);
	KeyMethod::instance.append("SARSWEEP", KM_SARSWEEP);
	KeyMethod::instance.append("MULTOPP", KM_MULTOPP);
	KeyMethod::instance.append("GALMAP", KM_GALMAP);
	KeyMethod::instance.append("EXHPORT", KM_EXHPORT);
	KeyMethod::instance.append("CONF", KM_CONF);
	KeyMethod::instance.append("DEFTRAIN", KM_DEFTRAIN);
	KeyMethod::instance.append("COD", KM_COD);
	KeyMethod::instance.append("BYP", KM_BYP);
	KeyMethod::instance.append("CONDITIONED", KM_CONDITIONED);
	KeyMethod::instance.append("ARM", KM_ARM);
	KeyMethod::instance.append("ARMIMP", KM_ARMIMP);
	KeyMethod::instance.append("DEADACC", KM_DEADACC);
	KeyMethod::instance.append("DEDI", KM_DEDI);
	KeyMethod::instance.append("GRIT", KM_GRIT);
	KeyMethod::instance.append("JURY", KM_JURY);
	KeyMethod::instance.append("LETHALBL", KM_LETHALBL);
	KeyMethod::instance.append("PHYSTRAIN", KM_PHYSTRAIN);
	KeyMethod::instance.append("SKILLJOCK", KM_SKILLJOCK);
	KeyMethod::instance.append("TINK", KM_TINK);
	KeyMethod::instance.append("TOUGH", KM_TOUGH);
	KeyMethod::instance.append("KILL", KM_KILL);
	KeyMethod::instance.append("INTIM", KM_INTIM);
	KeyMethod::instance.append("CONGENIAL", KM_CONGENIAL);
	KeyMethod::instance.append("TRUEAIM", KM_TRUEAIM);
	KeyMethod::instance.append("QUICKST", KM_QUICKST);
	KeyMethod::instance.append("PRECAIM", KM_PRECAIM);
	KeyMethod::instance.append("TARGBL", KM_TARGBL);
	KeyMethod::instance.append("SNIPSHOT", KM_SNIPSHOT);
	KeyMethod::instance.append("UNCANREAC", KM_UNCANREAC);
	KeyMethod::instance.append("STRSMART", KM_STRSMART);
	KeyMethod::instance.append("INTENSFOC", KM_INTENSFOC);
	KeyMethod::instance.append("SIXSENSE", KM_SIXSENSE);
	KeyMethod::instance.append("FORCERAT", KM_FORCERAT);
	KeyMethod::instance.append("BAL", KM_BAL);
	KeyMethod::instance.append("DODGE", KM_DODGE);
	KeyMethod::instance.append("DISARMSMILE", KM_DISARMSMILE);
	KeyMethod::instance.append("TOUCH", KM_TOUCH);
	KeyMethod::instance.append("INSIGHT", KM_INSIGHT);
	KeyMethod::instance.append("MASPIL", KM_MASPIL);
	KeyMethod::instance.append("FULLTH", KM_FULLTH);
	KeyMethod::instance.append("FULLTHIMP", KM_FULLTHIMP);
	KeyMethod::instance.append("FULLTHSUP", KM_FULLTHSUP);
	KeyMethod::instance.append("SLEIGHTMIND", KM_SLEIGHTMIND);
	KeyMethod::instance.append("HAWKSWOOP", KM_HAWKSWOOP);
	KeyMethod::instance.append("UNCANSENS", KM_UNCANSENS);
	KeyMethod::instance.append("PARRY", KM_PARRY);
	KeyMethod::instance.append("PARRYIMP", KM_PARRYIMP);
	KeyMethod::instance.append("UNARMPARRY", KM_UNARMPARRY);
	KeyMethod::instance.append("RAPREA", KM_RAPREA);
	KeyMethod::instance.append("REFLECT", KM_REFLECT);
	KeyMethod::instance.append("REFLECTIMP", KM_REFLECTIMP);
	KeyMethod::instance.append("SABERSW", KM_SABERSW);
	KeyMethod::instance.append("RAINDEATH", KM_RAINDEATH);
	KeyMethod::instance.append("ENDUR", KM_ENDUR);
	KeyMethod::instance.append("HEAVYHITTER", KM_HEAVYHITTER);
	KeyMethod::instance.append("BRA", KM_BRA);
	KeyMethod::instance.append("BURLY", KM_BURLY);
	KeyMethod::instance.append("BAR", KM_BAR);
	KeyMethod::instance.append("FERSTR", KM_FERSTR);
	KeyMethod::instance.append("SENSDANG", KM_SENSDANG);
	KeyMethod::instance.append("POINTBL", KM_POINTBL);
	KeyMethod::instance.append("SIDESTEP", KM_SIDESTEP);
	KeyMethod::instance.append("DEFSTA", KM_DEFSTA);
	KeyMethod::instance.append("DURA", KM_DURA);
	KeyMethod::instance.append("SENSEADV", KM_SENSEADV);
	KeyMethod::instance.append("KEENEYED", KM_KEENEYED);
	KeyMethod::instance.append("SUPREF", KM_SUPREF);
	KeyMethod::instance.append("DEAD", KM_DEAD);
	KeyMethod::instance.append("DEADIMP", KM_DEADIMP);
	KeyMethod::instance.append("HARDHD", KM_HARDHD);
	KeyMethod::instance.append("HARDHDIMP", KM_HARDHDIMP);
	KeyMethod::instance.append("RESEARCH", KM_RESEARCH);
	KeyMethod::instance.append("SORESUTECH", KM_SORESUTECH);
	KeyMethod::instance.append("WELLROUND", KM_WELLROUND);

	KeyMethod::instance.append("SENSECONTROL1", KM_SENSECONTROL1);
	KeyMethod::instance.append("SENSECONTROL3", KM_SENSECONTROL3);
	KeyMethod::instance.append("SENSESTRENGTH", KM_SENSESTRENGTH);
	KeyMethod::instance.append("SENSEDURATION", KM_SENSEDURATION);
	KeyMethod::instance.append("SENSERANGE", KM_SENSERANGE);
	KeyMethod::instance.append("SENSEMAGNITUDE", KM_SENSEMAGNITUDE);

	KeyMethod::instance.append("INFLUENCECONTROL2", KM_INFLUENCECONTROL2);
	KeyMethod::instance.append("INFLUENCERANGE", KM_INFLUENCERANGE);
	KeyMethod::instance.append("INFLUENCEMAGNITUDE", KM_INFLUENCEMAGNITUDE);
	KeyMethod::instance.append("INFLUENCESTRENGTH", KM_INFLUENCESTRENGTH);
	KeyMethod::instance.append("INFLUENCEDURATION", KM_INFLUENCEDURATION);

	KeyMethod::instance.append("ATARU", KM_ATARU);

	KeyMethod::instance.append("ENHANCEBASIC", KM_ENHANCEBASIC);
	KeyMethod::instance.append("ENHANCERANGE", KM_ENHANCERANGE);
	KeyMethod::instance.append("ENHANCECONT0", KM_ENHANCECONT0);
	KeyMethod::instance.append("ENHANCECONT1", KM_ENHANCECONT1);
	KeyMethod::instance.append("ENHANCECONT2", KM_ENHANCECONT2);
	KeyMethod::instance.append("ENHANCECONT3", KM_ENHANCECONT3);
	KeyMethod::instance.append("ENHANCECONT4", KM_ENHANCECONT4);
	KeyMethod::instance.append("ENHANCECONT5", KM_ENHANCECONT5);
	//KeyMethod::instance.append("ENHANCECONT6", KM_);
	KeyMethod::instance.append("ENHANCECONT7", KM_ENHANCECONT7);
	KeyMethod::instance.append("ENHANCECONT8", KM_ENHANCECONT8);
	KeyMethod::instance.append("ENHANCECONT9", KM_ENHANCECONT9);

	KeyMethod::instance.append("MOVESTRENGTH", KM_MOVESTRENGTH);
	KeyMethod::instance.append("MOVERANGE", KM_MOVERANGE);
	KeyMethod::instance.append("MOVEMAGNITUDE", KM_MOVEMAGNITUDE);

	KeyMethod::instance.append("MISDIRRANGE", KM_MISDIRRANGE);
	KeyMethod::instance.append("MISDIRMAGNITUDE", KM_MISDIRMAGNITUDE);
	KeyMethod::instance.append("MISDIRSTRENGTH", KM_MISDIRSTRENGTH);
	KeyMethod::instance.append("MISDIRCONTROL3", KM_MISDIRCONTROL3);
	KeyMethod::instance.append("MISDIRDURATION", KM_MISDIRDURATION);

	KeyMethod::instance.append("WARFORRANGE", KM_WARFORRANGE);
	KeyMethod::instance.append("WARFORCONTROL2", KM_WARFORCONTROL2);
	KeyMethod::instance.append("WARFORCONTROL3", KM_WARFORCONTROL3);
	KeyMethod::instance.append("WARFORCONTROL4", KM_WARFORCONTROL4);
	KeyMethod::instance.append("WARFORMAGNITUDE", KM_WARFORMAGNITUDE);

	KeyMethod::instance.append("BINDRANGE", KM_BINDRANGE);
	KeyMethod::instance.append("BINDMAGNITUDE", KM_BINDMAGNITUDE);
	KeyMethod::instance.append("BINDSTRENGTH", KM_BINDSTRENGTH);
	KeyMethod::instance.append("BINDCONTROL1", KM_BINDCONTROL1);
	KeyMethod::instance.append("BINDDURATION", KM_BINDDURATION);
	KeyMethod::instance.append("BINDCONTROL2", KM_BINDCONTROL2);
	KeyMethod::instance.append("BINDMASTERY", KM_BINDMASTERY);
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
	return charTalentMap.contains(key);
}

int CharTalentMap::ranks(const QString& key)
{
	if (charTalentMap.contains(key))
		return charTalentMap[key].ranks;
	return 0;
}

QString CharTalentMap::range(const QString& key, int ranks)
{
	// Misdirect starts at short range
	if (key.startsWith("MISDIR") ||
		key.startsWith("WARFOR"))
		ranks++;
	if (key == "SENSEBASIC") {
		switch (ranks) {
			case 0: return "Engaged";
			case 1: return "Medium range (Short for emotions)";
			case 2: return "Long range (Medium for emotions)";
			case 3: return "Extreme range (Long for emotions)";
		}
	}
	else {
		switch (ranks) {
			case 0: return "Engaged";
			case 1: return "Short range";
			case 2: return "Medium range";
			case 3: return "Long range";
		}
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
	if (key.startsWith("SEEK"))
		return QString("%1 Additional Detail%2").arg(ranks-1).arg(ranks-1==1 ? "" : "s");
	return QString("%1 Target%2").arg(ranks).arg(ranks==1 ? "" : "s");
}

int CharTalentMap::forcePower(const QString& key, QString& power, QString& base)
{
	if (key.startsWith("INFLUENCE")) {
		power = "INFLUENCE";
		base = "Influence";
		return FP_INFLUENCE;
	}
	if (key.startsWith("ENHANCE")) {
		power = "ENHANCE";
		base = "Enhance";
		return FP_ENHANCE;
	}
	if (key.startsWith("FORSEE")) {
		power = "FORSEE";
		base = "Foresee";
		return FP_FORSEE;
	}
	if (key.startsWith("MOVE")) {
		power = "MOVE";
		base = "Move";
		return FP_MOVE;
	}
	if (key.startsWith("MISDIR")) {
		power = "MISDIR";
		base = "Misdirect";
		return FP_MISDIR;
	}
	if (key.startsWith("SENSE")) {
		power = "SENSE";
		base = "Sense";
		return FP_SENSE;
	}
	if (key.startsWith("BIND")) {
		power = "BIND";
		base = "Bind";
		return FP_BIND;
	}
	if (key.startsWith("HEALHARM")) {
		power = "HEALHARM";
		base = "Heal/Harm";
		return FP_HEALHARM;
	}
	if (key.startsWith("PROTUNL")) {
		power = "PROTUNL";
		base = "Protect/Unleash";
		return FP_PROTUNL;
	}
	if (key.startsWith("WARFOR")) {
		power = "WARFOR";
		base = "Warde's Foresight";
		return FP_WARFOR;
	}
	if (key.startsWith("SUPPRESS")) {
		power = "SUPPRESS";
		base = "Suppress";
		return FP_SUPPRESS;
	}
	if (key.startsWith("FARSIGHT")) {
		power = "FARSIGHT";
		base = "Farsight";
		return FP_FARSIGHT;
	}
	if (key.startsWith("MANIPULATE")) {
		power = "MANIPULATE";
		base = "Manipulate";
		return FP_MANIPULATE;
	}
	if (key.startsWith("SEEK")) {
		power = "SEEK";
		base = "Seek";
		return FP_SEEK;
	}
	if (key.startsWith("BATMED")) {
		power = "BATMED";
		base = "Battle Meditation";
		return FP_BATMED;
	}
	return 0;
}

void CharTalentMap::clear()
{
	charTalentMap.clear();
	iDisplayNonSpecies.clear();
	iDisplayNonCoded.clear();
	iLightSaberChars.clear();
}

void CharTalentMap::addTalent(CharTalent talent)
{
	if (talent.talentType == TALENT_REGULAR && KeyMethod::instance.isCodedTalent(talent.key))
		talent.talentType = TALENT_CODED;
	if (charTalentMap.contains(talent.key)) {
		charTalentMap[talent.key].ranks++;
		DatUtil::appendToList(charTalentMap[talent.key].aquisition, talent.aquisition, ", ");
	}
	else {
		charTalentMap[talent.key] = talent;
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

	if (i < 0 || i >= charTalentMap.size()) {
		iEmpty.clear();
		return iEmpty;
	}

	if (exclude_hidden)
		return charTalentMap[iDisplayNonCoded.at(i)];
	return charTalentMap[iDisplayNonSpecies.at(i)];
}

CharTalent& CharTalentMap::get(const QString& key)
{
	if (charTalentMap.contains(key))
		return charTalentMap[key];
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
		for (QMap<QString, CharTalent>::iterator i = charTalentMap.begin(); i != charTalentMap.end(); i++) {
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
	internalName.clear();
	activation.clear();
	description.clear();
	books.clear();
	ranked = false;
	force = false;
	addDamagePerSkillRank = false;
	damageBonus = 0;
	damageBonusSkills.clear();
	dieModList.clear();
	lightSaberSkill.clear();
	soakValue = 0;
	defenseRanged = 0;
	defenseMelee = 0;
	requirementWearingArmor = false;
	requirementSoakAtLeast = 0;
	burly = 0;
}

QString Talent::name()
{
	MethodID talent_id = KeyMethod::instance.getID(key);
	switch (talent_id) {
		case KM_ENHANCEBASIC:
			return "Enhance Basic Power: Atheletics";

		case KM_SENSECONTROL2:
			return "Sense: Read Thoughts";

		case KM_MISDIRCONTROL1:
			return "Misdirect: Change Appearance";
		case KM_MISDIRCONTROL2:
			return "Misdirect: Create Illusion";
		case KM_MISDIRMASTERY:
			return "Misdirect Mastery";
		default:
			QString power;
			QString base;

			if (CharTalentMap::forcePower(key, power, base)) {
				if (internalName.startsWith("Control: "))
					return base + ": " + DatUtil::right(internalName, "Control: ");
				if (!internalName.startsWith(base))
					return base + ": " + internalName;
			}
	}
	return internalName.isEmpty() ? key : internalName;
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
		iTalent.internalName = value;
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
	else if (path.endsWith("/DieModifier/#open"))
		iDieMod.clear();
	else if (path.endsWith("/DieModifier/SkillKey/"))
		iDieMod.skillKey = value;
	else if (path.endsWith("/DieModifier/SkillType/"))
		iDieMod.skillType = strcmp(value, "Knowledge") == 0 ? KNOWLEDGE : NO_SKILL_TYPE;
	else if (path.endsWith("/DieModifier/BoostCount/"))
		iDieMod.boostCount = toInt(value);
	else if (path.endsWith("/DieModifier/SetbackCount/"))
		iDieMod.setbackCount = toInt(value);
	else if (path.endsWith("/DieModifier/ForceCount/"))
		iDieMod.forceCount = toInt(value);
	else if (path.endsWith("/DieModifier/AdvantageCount/"))
		iDieMod.advantageCount = toInt(value);
	else if (path.endsWith("/DieModifier/ThreatCount/"))
		iDieMod.threatCount = toInt(value);
	else if (path.endsWith("/DieModifier/AddSetbackCount/"))
		iDieMod.addSetbackCount = toInt(value);
	else if (path.endsWith("/DieModifier/UpgradeAbilityCount/"))
		iDieMod.upgradeCount = toInt(value);
	else if (path.endsWith("/DieModifier/#end"))
		iTalent.dieModList.addMod(iDieMod);

	// Skill keys:
	else if (path.endsWith("/SkillKeys/Key/")) {
		iDieMod.clear();
		iDieMod.skillKey = value;
		iTalent.dieModList.addMod(iDieMod);
	}

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
		iTalent.internalName = value;
	else if (path.endsWith("/ForceAbility/Description/")) {
		iTalent.description = value; // value.trimmed();
		if (iTalent.description.startsWith("Please see page")) {
			iTalent.books = DatUtil::pageSummary(iTalent.description);
			iTalent.description.clear();
		}
	}
	else if (path.endsWith("/ForceAbility/#end")) {
		// "Correct" Daryl's data:
		if (iTalent.internalName == "Control: Fine Detail")
			iTalent.key = "FARSIGHTCONTROL6";
		else if (iTalent.internalName == "Control: Vigilance/Perception")
			iTalent.key = "FARSIGHTCONTROL3";
		else if (iTalent.internalName == "Control: 360 Degrees")
			iTalent.key = "FARSIGHTCONTROL5";
		else if (iTalent.internalName == "Control: Ongoing Effect")
			iTalent.key = "FARSIGHTCONTROL4";
		iTalents[iTalent.key] = iTalent;
	}

	// DEADACC
	return true;
}

bool AllTalents::isTalent(const QString& key)
{
	return iTalents.contains(key);
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
		<< "acquisition" << "description" << "talentkey")
{
}

int Talents::rowCount()
{
	bool exclude_hidden = Character::instance->hideCodedTalents();
	return CurrentData::instance->talents.size(exclude_hidden);
}

QVariant Talents::getValue(int row, int col)
{
	bool exclude_hidden = Character::instance->hideCodedTalents();
	CharTalent& char_talent = CurrentData::instance->talents.at(row, exclude_hidden);
	Talent talent = AllTalents::instance()->getTalent(char_talent.key);

	switch (col) {
		case 0:
			return char_talent.talentType;
		case 1:
			return talent.name();
		case 2:
			return talent.ranked ? char_talent.ranks : 0;
		case 3:
			if (talent.force)
				return "Action";
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

			/*
			if (char_talent.key == "ENHANCECONT6") {
				QString	text;
				QString action = "an Action";
				QString type = "horizontally";
				QString range = "Short";

				if (CurrentData::instance->talents.contains("ENHANCECONT3"))
					type = "horizontally or vertically";
				if (CurrentData::instance->talents.contains("ENHANCECONT0"))
					action = "a Maneuver";
				if (CurrentData::instance->talents.contains("ENHANCERANGE"))
					range = "Medium";
				text = QString("As %1 jump %2 to any location in %3 range").arg(action).arg(type).arg(range);
				desc.clear();
				DatUtil::addDescription(desc, text, talent.books);
			}
			*/

			return desc;
		}
		case 6:
			return char_talent.key;

	}
	return QVariant();
}

/*
	// Talents:
	for (QMap<QString, CharTalent>::iterator i = CurrentData::instance->talents.begin(); i != CurrentData::instance->talents.end(); i++) {
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
