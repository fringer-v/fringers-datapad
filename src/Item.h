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

#ifndef __Item_h__
#define __Item_h__

#include <QString>
#include <QMap>

#define V_BR		0
#define V_AG		1
#define V_INT		2
#define V_CUN		3
#define V_WIL		4
#define V_PR		5
#define V_SOAK		6
#define V_DRANGED	7
#define V_DMELEE	8
#define V_DUMMY		9
#define V_COUNT		10

#define BR_BIT		1
#define AG_BIT		2
#define INT_BIT		4
#define CUN_BIT		8
#define WIL_BIT		16
#define PR_BIT		32

#define SOAK_BIT	64
#define DRANGED_BIT	128
#define DMELEE_BIT	256

#define UNDEFINED	0
#define NOT_CARRIED	1
#define IS_HELD		2
#define IS_EQUIPPED	3

class Character;

class Quality {
public:
	QString	key;
	int count;

	Quality();
	void clear(const QString& k = QString());
	QString quality();
};

class Mod {
public:
	QString	key;
	int count;
	int number;
	QString miscDesc;

	Mod();
	void clear(const QString& k = QString());
	QString modText();
};

class QualityList {
public:
	void clear();
	int count();
	bool contains(const QString& key);
	Quality itemAt(int i);
	Quality get(const QString& key);
	void addItem(Quality qual);
	//void addMod(Mod mod);

private:
	QMap<QString, Quality>	iModMap;
	QList<QString>			iOrder;
};

class DieMod {
public:
	QString	skillKey;
	int boostCount;
	int setbackCount;
	int forceCount;
	int advantageCount;
	int threatCount;
	int addSetbackCount;
	int upgradeCount;

	DieMod();
	void clear(const QString& k = QString());
};

class DieModList {
public:
	void clear();
	int count();
	bool contains(const QString& skill);
	DieMod get(const QString& key);
	void addMod(DieMod mod);

	QMap<QString, DieMod> iDieModMap;
};

class ModList {
public:
	void clear();
	int count() const;
	bool contains(const QString& key);
	Mod itemAt(int i) const;
	Mod get(const QString& key);
	QString addItem(Mod mod);
	void attachments(QString& list);

private:
	QMap<QString, Mod>	iModMap;
	QList<QString>		iOrder;
};

class CharMods {
public:
	void clear()
	{
		for (int i=0; i<V_COUNT; i++)
			value[i] = 0;
	}

	int get(int ch) const {
		return value[ch];
	}

	int get(const QString& ch) const {
		return get(map(ch));
	}

	bool set(int ch, int v)
	{
		if (value[ch] != v) {
			value[ch] = v;
			return true;
		}
		return false;
	}

	bool set(const QString& ch, int v)
	{
		int c = map(ch);
		if (value[c] != v) {
			value[c] = v;
			return true;
		}
		return false;
	}

	int set(const CharMods& mods) {
		int changed = 0;

		for (int i=0; i<V_COUNT; i++) {
			if (value[i] != mods.value[i]) {
				value[i] = mods.value[i];
				changed |= (1 << i);
			}
		}
		return changed;
	}

	bool inc(int ch, int v)
	{
		if (!v)
			return false;
		return set(ch, value[ch] + v);
	}

private:
	int map(const QString& ch) const {
		switch ((ch[0].unicode() << 8) + ch[1].unicode()) {
			case ('b' << 8) + 'r': return V_BR;
			case ('B' << 8) + 'R': return V_BR;
			case ('a' << 8) + 'g': return V_AG;
			case ('A' << 8) + 'G': return V_AG;
			case ('i' << 8) + 'n': return V_INT;
			case ('I' << 8) + 'N': return V_INT;
			case ('c' << 8) + 'u': return V_CUN;
			case ('C' << 8) + 'U': return V_CUN;
			case ('w' << 8) + 'i': return V_WIL;
			case ('W' << 8) + 'I': return V_WIL;
			case ('p' << 8) + 'r': return V_PR;
			case ('P' << 8) + 'R': return V_PR;
			case ('s' << 8) + 'o': return V_SOAK;
			case ('S' << 8) + 'O': return V_SOAK;
			case ('d' << 8) + 'r': return V_DRANGED;
			case ('D' << 8) + 'R': return V_DRANGED;
			case ('d' << 8) + 'm': return V_DMELEE;
			case ('D' << 8) + 'M': return V_DMELEE;
		}
		return V_DUMMY;
	}
	int value[V_COUNT];
};

class Item {
public:
	QString		uuid;
	QString		itemkey;
	QString		rename;
	QString		notes;
	QString		attachments;
	bool		shown;
	DieModList	dieModList;
	QStringList	attachList;
	int			originalQuantity;
	int			originalStored;
	int			originalState;

	QString name() const;
	bool unmodified();
	bool restricted();
	QString damageTotal();
	QString dicePool();
	int critPlus();
	int pierce();
	int removeBurly(int val, int burly);
	int cumbersome(int burly);
	QString qualities();
	QString features();
	int encCarriedVal(int burly);
	int encWornValue();
	QString encArmorValue();
	int soakVal();
	int meleeDef();
	int rangeDef();
	void characteristicDelta(CharMods& mods);
	int critTotal();
	bool held();
	bool equipped();
	bool carried();
	int state();
	int quantity();
	int carriedQuantity();
	int stored();
	bool isGrenade();
	bool canBeWorn();

	void addMod(Mod mod);
	void addMod(const ModList& mod_list);
	bool hasMod(const QString& qkey);
	Mod getMod(const QString& key);

	bool hasQuality(const QString& key);
	Quality getQuality(const QString& key);

	int encumbrance();

	void clear() {
		uuid.clear();
		itemkey.clear();
		rename.clear();
		notes.clear();
		attachments.clear();
		shown = true;
		dieModList.clear();
		attachList.clear();

		originalQuantity = 0;
		originalStored = 0;
		originalState = NOT_CARRIED;

		iModList.clear();
	}

private:
	void storageData(int& quantity, int& stored, int& state);

	ModList	iModList;
};

#define INV_TYPE_WEAPON			0
#define INV_TYPE_ARMOR			1
#define INV_TYPE_GEAR			2

#define GEAR_TYPE_CYBERNETICS	1
#define GEAR_TYPE_STORAGE		2
#define GEAR_TYPE_GRENADE		4 // Grenade in name
#define GEAR_TYPE_RANGED		8 // Category = Ranged

#endif // __Item_h__
