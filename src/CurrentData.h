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

#include "DatXmlReader.h"
#include "DataList.h"
#include "Skills.h"
#include "Talents.h"
#include "Shop.h"

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

class Character;

class FindTag : public QObject, public DatXmlReader {
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
	QString key; // same is intSkillID
	MethodID skillID; // e.g. ICOOL, IVIG, REC, FDISC
	MethodID intSkillID; // e.g COOL, VIGIL, COOL, DISC
	int internalRanks;
	bool isCareer;

	CharSkill() {
		clear("");
	}

	void clear(QString k) {
		key = k;
		skillID = KM_UNKNOWN;
		intSkillID = KM_UNKNOWN;
		internalRanks = 0;
		isCareer = false;
	}

	int skillRanks();
	int skillAttribute(Character* charac, Skill* skill);
	QString getBasicPool(Character* charac);
	double poolRating(Character* charac);
	QString getDicePool();
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

class ChecklistItem {
public:
	ChecklistItem() :
		reference(0),
		checked(false),
		reduceStrain(),
		moveRequired(),
		consumable(),
		commitKey(),
		forceCommitCount(0),
		destinyPointCost(0),
		plusDamage(0),
		plusPierce(0),
		plusCrit(0),
		plusCritRating(0),
		plusRange(0),
		plusMagnitude(0),
		plusStrength(0),
		plusDuration(0),
		plusExtra(0)
	{
	}

	QString dice() {
		if (forceCommitCount > 0)
			return QString("F").repeated(forceCommitCount);
		return dicePool;
	}

	int reference;		// < 0 if Added automatically!
	bool checked;
	QString dicePool;
	QString description;
	int reduceStrain;	// Reduce strain by this amount when used /checked
	int moveRequired;
	QString consumable;
	QString commitKey;
	int forceCommitCount;
	int destinyPointCost;
	int plusDamage;
	int plusPierce;
	int plusCrit;
	int plusCritRating;
	int plusRange;
	int plusMagnitude;
	int plusStrength;
	int plusDuration;
	int plusExtra;
};

typedef enum {
	PLUS_DURATION,
	PLUS_MAGNITUDE,
	PLUS_STRENGTH
} PlusType;

class ChecklistData {
public:

	QString					skillKey;
	QList<ChecklistItem>	items;

	void clear();
	ChecklistItem* findItem(int reference);
	void checkAll(bool checked);
	void deleteItem(int reference);
	QString getCurrentPool();
	bool hasCustomItem() const;
	int strainUsed(QString& consumable) const;
	int movesUsed() const;
	void appendCustom(int ref, const QString& pool, const QString& desc);
	int append(ChecklistItem& item);

	void plusDamage(int ref, int count);
	void plusPierce(int ref, int count);
	void plusCrit(int ref, int count);
	void plusCritRating(int ref, int count);
	void plusRange(int ref, int count);
	void plusMagnitude(int ref, int count);
	void plusStrength(int ref, int count);
	void plusDuration(int ref, int count);
	void plusExtra(int ref, int count);

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
	friend class CharCache;

public:
	static CurrentData* instance;

	CurrentData() {
		clear();
	}

	QString fileName;       // Name of the current data file
	QString characterFile;	// Name of the character file

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
	QString name;			// Name of the character
	bool npc;
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
	QMap<QString, ShopItem> customItems;

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
	ChecklistData checklistItems;
	QMap<QString, ChecklistData> checkLists;
	int dutyRank;
	QList<ExpLogItem> experienceLog;
	QMap<QString, ExpLogTotal> experienceTotal; // EXP_TOT_XP, etc.
	QList<InjuryItem> injuries;

	void clear();

	int getAttribute(const QString& val);

	void adjustWounds(int delta);
	void adjustStrain(int delta);
	void adjustConflict(int delta);

	void useStimPack(Character* charac, int delta);
	void useErp(Character* charac, int delta);

	QString stimPacks();
	QString erps();

	int stimPackQuantity();
	int erpQuantity();

	bool setWoundDelta(int val);
	bool setStrainDelta(int val);

	void addExperience(Character* charac, const QString& type, const QString& key, const QString& name, const QString& desc, int amount);
	void changeExperience(Character* charac, int ref, const QString& desc, int amount);
	void removeExperience(Character* charac, int ref);

	void addCustomSkill(Character* charac, const QString& name, const QString& characteristic, int ranks);
	void removeCustomSkill(Character* charac, const QString& name);

	void addItem(Character* charac, int count, const QString& itemkey, const QString& desc, int amount);
	void updateItem(Character* charac, int ref, int count, const QString& desc, int amount);
	bool removeItem(Character* charac, int ref);
	void storeItem(const QString& uuid, const QString& itemkey, int count, int state, Item* item);

	void clearChecklist(Character* charac);
	static QString plurize(const QString& thing, int val);
	void setChecklist(Character* charac, QString skillKey, QString talentKey, Item& weapon);
	void exitChecklist(Character* charac);
	void appendCheckItem(Character* charac, const QString& pool, const QString& desc);
	void updateCheckItem(Character* charac, int ref, const QString& pool, const QString& desc);
	void removeCheckItem(Character* charac, int ref);
	void uncheckAllItem(Character* charac);
	void checklistRangeAdd(QString power);
	void checklistMultiForce(int pip_count, QString desc, QString sin, QString plu, int amount, PlusType type, bool select_one = false);
	int checklistAdd(const QString& pool, const QString& desc, int move, int strain, int dpoint = 0,
		const QString& consumable = QString(), const QString& commit_key = QString(), int force_cost = 0);
	void condChecklistAdd(const QString& talent, const QString& pool, const QString& desc, int move, int strain);
	void condChecklistAddRanked(const QString& talent, const QString& pool, const QString& desc, int move, int strain);
	void checklistSelected(const QString& pool, const QString& desc);
	void multiCommit(Character* charac, QString key, QString desc, QString arg, int extra_limit = 1000, bool commit_list = true);
	int checklistCommit(int commit_count, const QString& commit_key, const QString& desc, int move = 0, int strain = 0, bool commit_list = true);
	int checklistMayCommit(int commit_count, const QString& commit_key, const QString& desc, int duration = 1000);
	int checklistDamage(int damage, const QString& desc, int strain, int dpoint);
	int checklistAppend(ChecklistItem& item);
	bool checkItem(Character* charac, int ref, bool list_setup = true);
	void setupChecklistForce(int commit_count);

	int negativePool();
	void setNegativePool(Character* charac, int bit, const QString& skillKey);
	void negetiveDefence(int& r, int &m);

	int forceCommitCount();
	int isCommitted(const QString& key);
	int getCommitCount(const QString& key);

	static int expTypeToInt(const char* type);
	static QString expTypeToString(int type, const QString& key = QString());

	void addCriticalWound(Character* charac, int perc, int type);
	void removeCriticalWound(Character* charac, int ref);

	int nonCommitedForce(Character* charac);

	void setCharSkill(const CharSkill& char_skill);
	CharSkill getCharSkill(MethodID skill_id);

private:
	void setCriticalWound(int perc, int type);
	void appendEmptyWound();

	int findExpLogItem(int ref);
	void setExpLogItem(int type, const QDateTime& when, const QString& key, const QString& name, const QString& desc, int amount, bool loading);
	void addExpLogItem(int type, const QDateTime& when, const QString& key, const QString& name, const QString& desc, int amount);
	int findInvLogItem(int ref);
	QString setInvLogItem(int count, const QDateTime& create, const QDateTime& update, const QString& uuid, const QString& itemkey, const QString& desc, int amount, bool loading);
	void addInvLogItem(const QDateTime& create, const QDateTime& update, const QString& desc, int amount, int type);
	void inventoryChanged(Character* charac, const QString& uuid, const QString& itemkey, bool signal);
	void dicePoolChanged(Character* charac, ChecklistItem* item = NULL, bool list_setup = false);
	QString getCustomChecklistKey(Character* charac);
	void addCheckItem(const QString& skillKey, const QString& pool, const QString& desc);
	void writeCurrentData();
	void changeWounds(int delta);
	void changeStrain(int delta);
	void setTemporaryStrain(Character* charac, int value);

	QMap<QString, int> iForceCommitted;
	int iCommitCountMax;

	QMap<MethodID, CharSkill> iCharSkills;

	CurrentData* iNextInCache;
	int iNegativeCheck;
	int iNegMelee;
	int iNegRanged;

	// Counter used to generate unique IDs and references:
	static int iReferenceCounter;
};

class CurrentDataXML : public DatXmlReader {
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
	QString		iItemCheckSkill;

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
