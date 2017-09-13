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

#ifndef __Talents_h__
#define __Talents_h__

#include <QString>

#include "DatXmlReader.h"
#include "DataList.h"

//#define STR						1
//#define RAN						2
//#define MAG						4
//#define DUR						8

#define TALENT_REGULAR			0
#define TALENT_SPECIES			1
#define TALENT_CODED			2

#define FP_INFLUENCE			1
#define FP_ENHANCE				2
#define FP_FORSEE				3
#define FP_MOVE					4
#define FP_MISDIR				5
#define FP_WARFOR				6
#define FP_SENSE				7
#define FP_BIND					8
#define FP_HEALHARM				9
#define FP_PROTUNL				10
#define FP_SUPPRESS				11
#define FP_FARSIGHT				12
#define FP_MANIPULATE			13
#define FP_SEEK					14
#define FP_BATMED				15

class CharTalent {
public:
	int		talentType;
	QString key;
	int		ranks;
	QString	aquisition;
	QList<QString> selectedSkills;
	QString selectedCharacteristics;

	void clear(QString k = QString()) {
		talentType = TALENT_REGULAR;
		key = k;
		ranks = 1;
		aquisition.clear();
		selectedSkills.clear();
		selectedCharacteristics.clear();
	}
};

class CharTalentMap {
public:
	CharTalentMap();

	int size(bool exclude_hidden = false);
	void clear();
	bool contains(const QString& key);
	int ranks(const QString& key);
	QString range(const QString& key, int ranks);
	QString duration(const QString& key, int ranks);
	QString magnitude(const QString& key, int ranks);
	void addTalent(CharTalent talent);
	CharTalent& at(int i, bool exclude_hidden = false);
	CharTalent& get(const QString& key);
	QString getLightSaberChar();

	static int forcePower(const QString& key, QString& power, QString& base);

private:
	void fillArrays();

	QList<QString> iDisplayNonSpecies;
	QList<QString> iDisplayNonCoded;
	static CharTalent iEmpty;
	QStringList iLightSaberChars;

public:
	QMap<QString, CharTalent> charTalentMap;
};

class Talent {
public:
	QString key;
	QString internalName;
	QString activation;
	QString description;
	QString books;
	bool ranked;
	bool force;
	bool addDamagePerSkillRank;

	int damageBonus;
	QString damageBonusSkills;

	DieModList dieModList;

	QString lightSaberSkill;

	int		soakValue;
	int		defenseRanged;
	int		defenseMelee;
	bool	hasRequirement;
	bool	requirementWearingArmor;
	int		requirementSoakAtLeast;

	void clear(QString key);
	QString name();
};

class AllTalents : public QObject, public DatXmlReader {
	Q_OBJECT

public:
	static AllTalents* instance();

	void loadTalents(const QString& dset);

	virtual bool xmlElement(const DatStringBuffer& path, const char* value);

	bool isTalent(const QString& key);
	Talent getTalent(const QString& key);
	void addTalent(const Talent& t);

private:
	static QPointer<AllTalents> sInstance;

	Talent iTalent;
	QMap<QString, Talent> iTalents;
	DieMod iDieMod;
};

class Talents : public AbstractDataList {
public:
	static Talents instance;

	Talents();

	virtual int rowCount();
	virtual QVariant getValue(int row, int col);

private:
	QString getBooks(const QString& desc);
};

class SpeciesTalent {
public:
	QString damageBonusSkill;
	int damageBonus;
	int weaponsCrit;

	DieMod dieMod;

	void clear() {
		damageBonusSkill.clear();
		damageBonus = 0;
		weaponsCrit = 0;

		dieMod.clear();
	}
};

#endif // __Talents_h__
