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

#ifndef __CurrentData_h__
#define __CurrentData_h__

#include <QObject>
#include <QDateTime>

#include "DatXMLReader.h"
#include "DataList.h"
#include "Skills.h"
#include "Talents.h"

#define MAX_HISTORY_LEN			22

#define ITEM_LOADED				-1
#define ITEM_ADD_XP				0
#define ITEM_TOTAL				1
#define ITEM_START				2
#define ITEM_AMOUNT				3
#define ITEM_ORIG_STOCK			4
#define ITEM_ADD_ITEM			5

#define NEG_CHECK_1_BLACK		1
#define NEG_CHECK_2_BLACK		2
#define NEG_CHECK_3_BLACK		4
#define NEG_CHECK_1_PURPLE		8
#define NEG_CHECK_2_PURPLE		16
#define NEG_CHECK_3_PURPLE		32
#define NEG_CHECK_4_PURPLE		64
#define NEG_CHECK_1_RED			128
#define NEG_CHECK_2_RED			256
#define NEG_CHECK_3_RED			512
#define NEG_CHECK_1_UPGRADE		1024
#define NEG_CHECK_2_UPGRADE		2048

#define NEG_PURPLE(x)			((x) & (NEG_CHECK_1_PURPLE | NEG_CHECK_2_PURPLE | NEG_CHECK_3_PURPLE | NEG_CHECK_4_PURPLE))
#define NEG_PURPLE_COUNT(x)		((((x) & NEG_CHECK_1_PURPLE) ? 1 : 0) + \
								(((x) & NEG_CHECK_2_PURPLE) ? 2 : 0) + \
								(((x) & NEG_CHECK_3_PURPLE) ? 3 : 0) + \
								(((x) & NEG_CHECK_4_PURPLE) ? 4 : 0))

/*
#define EXTRA_MAN				-100
#define AIM						-101
#define AIM_TWICE				-102
#define CALLED_SHOT				-103
#define CALLED_SHOT_AIMED		-104
#define QUICK_STRIKE			-105
#define TARGET_BLOW				-106
#define SNIP_SHOT				-107
#define TELE_OP_SIGHT			-108
#define TARGET_COMP				-109
#define SETTRIGGER				-110
#define INTENSE_FOCUS			-111
#define DISARMING_SMILE			-112
#define INFLUENCE_SKILLS		-113
#define FULL_THROTTLE			-114
#define FULL_THROTTLE_IMP		-115
#define JUST_KIDDING			-114
#define DONT_SHOOT				-115
#define DC_1					-116
#define DC_2					-117
#define DC_3					-118
#define GTA_1					-119
#define GTA_2					-120
#define GTA_3					-121
#define GTA_4					-122
#define CP						-123
#define BS						-124
#define ENHANCEBASIC			-125
#define ENHANCECONT1			-126
#define ENHANCECONT2			-127
#define ENHANCECONT4			-128
#define ENHANCECONT7			-129
#define ENHANCECONT5			-130

// Ranked, so the need "space"
#define PREC_AIM				-1000
#define DODGE					-1100
#define CONGENIAL				-1200
#define INTIMIDATING			-1300
*/

class Character;

class FindTag : public QObject, public DatXMLReader {
	Q_OBJECT

public:
	QString search(const QByteArray& bytes, const QString& search_path);
	virtual bool xmlElement(const DatStringBuffer& path, const char* value);

private:
	const char* iSearchPath;
	QString iResultValue;
};

class CharSkill {
public:
	QString key;
	int		ranks;
	bool	isCareer;

	CharSkill() {
		clear("");
	}

	void clear(QString k) {
		key = k;
		ranks = 0;
		isCareer = false;
	}

	QString getDicePool(Skill* skill = NULL, QString ch = QString());
};

class CharItem {
public:
	QString key;
	QString name;
	int		size;
	QString notes;

	void clear() {
		key.clear();
		name.clear();
		size = 0;
		notes.clear();
	}
};

class CharItemList {
public:
	QList<CharItem> items;

	CharItem findItem(const QString& key);
};

class MotMorItem {
public:
	QString name1;
	QString name2;
	QString notes;

	void clear() {
		name1.clear();
		name2.clear();
		notes.clear();
	}
};

class SpecialFeatureItem {
public:
	QString title;
	QString subtitle;
	QString content;

	void clear() {
		title.clear();
		subtitle.clear();
		content.clear();
	}
};

#define UNKNOWN_QUANTITY		-2000000000
#define UNKNOWN_STORED			-1

class InvModItem {
public:
	QString uuid;
	QString itemkey;
	int quantity; // UNKNOWN_QUANTITY, or actual stock
	int stored; // UNKNOWN_STORED, or a value >= 0
	int state; // UNDEFINED, NOT_CARRIED, IS_HELD, IS_EQUIPPED
	int rowCount;

	InvModItem() :
		uuid(),
		itemkey(),
		quantity(UNKNOWN_QUANTITY),
		stored(UNKNOWN_STORED),
		state(UNDEFINED),
		rowCount(0) {
	}
};

class InvLogItem {
public:
	int ref;
	int count;
	QDateTime create;
	QDateTime update;
	QString uuid;
	QString itemkey;
	QString desc;
	int amount;
	int type;

	InvLogItem() :
		ref(0),
		count(0),
		create(),
		update(),
		uuid(),
		itemkey(),
		desc(),
		amount(0),
		type(0) {
	}

	InvLogItem(int r, int c, const QDateTime& cr, const QDateTime& up, const QString& u, const QString& k, const QString& d, int a, int t) :
		ref(r),
		count(c),
		create(cr),
		update(up),
		uuid(u),
		itemkey(k),
		desc(d),
		amount(a),
		type(t) {
	}
};

#define EXP_UNKNOWN			0
#define EXP_XP				1
#define EXP_DUTY			2
#define EXP_DUTYRANK		3
#define EXP_MORALITY		4
#define EXP_OBLIGATION		5

#define EXP_TOT_XP			"XP"
#define EXP_TOT_DUTY		"DUTY"
#define EXP_TOT_DUTYRANK	"DUTYRANK"
#define EXP_TOT_MORALITY	"MORALITY"
#define EXP_TOT_OBLIGATION	"OBLIGATION"

class ExpLogItem {
public:
	int ref;
	int type;		// EXP_XP, EXP_DUTY, EXP_DUTY_RANK, EXP_MORALITY, EXP_OBLIGATION
	QDateTime when;
	QString key;	// Key of Duty or Obligation
	QString name;	// Name of Duty or Obligation
	QString desc;
	int amount;

	ExpLogItem() :
		ref(0),
		type(EXP_UNKNOWN),
		when(),
		key(),
		name(),
		desc(),
		amount(0) {
	}

	ExpLogItem(int r, int t, const QDateTime& w, const QString& k, const QString& n, const QString& d, int a) :
		ref(r),
		type(t),
		when(w),
		key(k),
		name(n),
		desc(d),
		amount(a) {
	}

	void clear() {
		ref = 0;
		type = EXP_UNKNOWN;
		when = QDateTime();
		key.clear();
		name.clear();
		desc.clear();
		amount = 0;
	}
};

class ExpLogTotal {
public:
	int value;
	int firstItem;
	int lastItem;
	int itemCount;

	ExpLogTotal() :
		value(0),
		firstItem(-1),
		lastItem(-1),
		itemCount(0) {
	}
};

class CustomSkill {
public:
	CustomSkill() :
		rank(0) {
	}

	QString name;
	QString characteristic;
	int rank;
};

class CheckListItem {
public:
	CheckListItem() :
		reference(0),
		checked(false),
		reduceStrain(),
		moveRequired(),
		consumable(),
		commitKey(),
		forceCost(1) {
	}

	int reference;		// < 0 if Added automatically!
	bool checked;
	QString dice;
	QString description;
	int reduceStrain;	// Reduce strain by this amount when used /checked
	int moveRequired;
	QString consumable;
	QString commitKey;
	int forceCost;
};

class CheckListData {
public:
	QString					skillKey;
	QList<CheckListItem>	items;

	void clear();
	CheckListItem* findItem(int reference);
	void checkAll(bool checked);
	void deleteItem(int reference);
	QString getCurrentPool();
	bool hasCustomItem() const;
	int strainUsed(QString& consumable) const;
	int movesUsed() const;
	int plus(const QString& pool, const QString& desc, int move, int strain,
		int dpoint = 0, const QString& consumable = QString(), const QString& commit_key = QString(), int force_cost = 0);
	void plusCustom(int ref, const QString& pool, const QString& desc);

private:
	int iRefCount;
};

class InjuryItem {
public:
	int ref;
	int percent;
	int type;

	void clear() {
		ref = 0;
		percent = 0;
		type = 0;
	}
};

class CurrentData {
	friend class CurrentDataXML;

public:
	static CurrentData* instance;

	CurrentData() {
		clear();
	}

	// DYNAMIC DATA:
	// Changes to attributes because of inventory
	CharMods attributeMods;

	// Current encumbrance and cumbersome values, change because of inventory changes
	int encumbranceValue;
	int encumbranceThreshold;
	int cumbersomeValue;
	int cumbersomeThreshold;
	QString encumbranceText;

	// CHARACTER XML DATA
	QString name;
	QString player;
	QString gender;
	QString age;
	QString height;
	QString build;
	QString hair;
	QString eyes;
	QString features;
	QString story;
	QString portrait;
	int credits;
	int originalCredits;
	QString career;
	QString specializations;
	QMap<QString, int> attributes;
	QMap<QString, CharSkill> skills;
	CharItemList obligations;
	CharItemList duties;
	QList<MotMorItem> motivations;
	QList<MotMorItem> moralities;
	CharTalentMap talents;
	ItemList weapons;
	ItemList armor;
	ItemList gear;
	QString species;
	QList<SpeciesTalent> speciesTalents;
	QList<SpecialFeatureItem> specialFeatures;

	// CURRENT DATA
	int wounds;
	int strain;
	int temporaryStrain;
	int conflict;
	QString woundHistory;
	QString strainHistory;
	QString conflictHistory;
	int woundDelta;
	int strainDelta;
	QMap<QString, InvModItem> invMod;
	QList<InvLogItem> inventoryLog;
	int stimPacksUsed;
	int erpsUsed;
	QList<CustomSkill> customSkills;
	CheckListData autoCheckItems;
	QMap<QString, CheckListData> checkLists;
	QMap<QString, int> forceCommitted;
	int dutyRank;
	QList<ExpLogItem> experienceLog;
	QMap<QString, ExpLogTotal> experienceTotal; // EXP_TOT_XP, etc.
	QList<InjuryItem> injuries;

	void clear();
	void loadCurrentData();

	QString getFile();

	void adjustWounds(int delta);
	void adjustStrain(int delta);
	void adjustConflict(int delta);

	void useStimPack(int delta);
	void useErp(int delta);

	QString stimPacks();
	QString erps();

	int stimPackQuantity();
	int erpQuantity();

	bool setWoundDelta(int val);
	bool setStrainDelta(int val);

	void addExperience(const QString& type, const QString& key, const QString& name, const QString& desc, int amount);
	void changeExperience(int ref, const QString& desc, int amount);
	void removeExperience(int ref);

	void addCustomSkill(const QString& name, const QString& charac, int ranks, bool loading = false);
	void removeCustomSkill(const QString& name);

	void addItem(int count, const QString& itemkey, const QString& desc, int amount);
	void updateItem(int ref, int count, const QString& desc, int amount);
	bool removeItem(int ref);
	void storeItem(const QString& uuid, const QString& itemkey, int count, int state, Item* item);

	void clearAutoCheckItems(const QString& skillKey);
	void setupAutoCheckItems(const QString& skillKey, const QString& uuid);
	void exitAutoCheckItems(const QString& skillKey);
	void addCheckItem(const QString& skillKey, const QString& pool, const QString& desc);
	void updateCheckItem(int ref, const QString& skillKey, const QString& pool, const QString& desc);
	void removeCheckItem(int ref, const QString& skillKey);
	void uncheckAllItem(const QString& skillKey);
	void checkItem(int ref, const QString& skillKey, bool list_setup);

	int negativePool();
	void setNegativePool(int bit, const QString& skillKey);
	void negetiveDefence(int& r, int &m);

	int commitCount();
	int isCommitted(const QString& key);

	static int expTypeToInt(const char* type);
	static QString expTypeToString(int type, const QString& key = QString());

	void addCriticalWound(int perc, int type);
	void removeCriticalWound(int ref);

	int nonCommitedForce();

private:
	void setCriticalWound(int perc, int type);
	void appendEmptyWound();

	int findExpLogItem(int ref);
	void setExpLogItem(int type, const QDateTime& when, const QString& key, const QString& name, const QString& desc, int amount, bool loading);
	void addExpLogItem(int type, const QDateTime& when, const QString& key, const QString& name, const QString& desc, int amount);
	int findInvLogItem(int ref);
	QString setInvLogItem(int count, const QDateTime& create, const QDateTime& update, const QString& uuid, const QString& itemkey, const QString& desc, int amount, bool loading);
	void addInvLogItem(const QDateTime& create, const QDateTime& update, const QString& desc, int amount, int type);
	void inventoryChanged(const QString& uuid, const QString& itemkey, bool signal);
	void setCheckItem(const QString& skillKey, const QString& pool, const QString& desc);
	void addCheckItem(const QString& skillKey, int reference, const QString& pool, const QString& desc);
	void checkChecked(const QString& skillKey);
	void writeCurrentData();
	void changeWounds(int delta);
	void changeStrain(int delta);
	void setTemporaryStrain(int value);

	QString		iFileName;

	int			iNegativeCheck;
	int			iNegMelee;
	int			iNegRanged;

	// Counter used to generate unique IDs and references:
	int			iReferenceCounter;
};

class CurrentDataXML : public DatXMLReader {
public:
	CurrentDataXML(CurrentData* current_data);

	virtual bool xmlElement(const DatStringBuffer& path, const char* value);

private:
	CurrentData*iCurrentData;
	int			iInjuryPercent;

	ExpLogItem	iExpItem;

	int			iItemCount;
	QString		iItemUuid;
	QDateTime	iItemCreate;
	QDateTime	iItemUpdate;
	QString		iItemKey;
	QString		iItemDesc;
	int			iItemAmount;

	QString		iStoreItemUuid;
	QString		iStoreItemKey;
	int			iStoreItemAmount;
	int			iStoreItemState; // UNDEFINED, NOT_CARRIED, IS_HELD, IS_EQUIPPED

	QString		iItemPool;
	QString		iItemSkill;

	QString		iSkillName;
	QString		iSkillChar;
	int			iSkillRank;

};

class CustomSkills : public AbstractDataList {
public:
	static CustomSkills instance;

	CustomSkills();

	virtual int rowCount();
	virtual QVariant getValue(int row, int col);
};

class CheckList : public AbstractDataList {
public:
	static CheckList instance;

	CheckList();

	virtual int rowCount();
	virtual QVariant getValue(int row, int col);
};

class InventoryLog : public AbstractDataList {
public:
	static InventoryLog instance;

	InventoryLog();

	virtual int rowCount();
	virtual QVariant getValue(int row, int col);
};

#endif // __CurrentData_h__
