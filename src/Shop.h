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

#ifndef __Shop_h__
#define __Shop_h__

#include "DataList.h"

class ShopItem {
public:
	QString	key;
	QString type;
	QString	name;
	QString description;
	QString books;
	int price;
	int rarity;
	int encumbrance;
	QString	range;
	int critical;
	int addDamage;
	int damage;
	QString	skillKey;
	int mdef;
	int rdef;
	int soak;
	int gearType;
	QMap<QString, Mod> modList;
	QMap<QString, Quality> qualityList;

	ShopItem() {
		clear(QString());
	}

	void clear(const QString& k) {
		key = k;
		type.clear();
		name.clear();
		description.clear();
		books.clear();
		price = 0;
		rarity = 0;
		encumbrance = 0;
		range.clear();
		critical = 0;
		addDamage = 0;
		damage = 0;
		skillKey.clear();
		mdef = 0;
		rdef = 0;
		soak = 0;
		gearType = 0;
		modList.clear();
		qualityList.clear();
	}

	bool isMeleeWeapon() const {
		return skillKey == "LTSABER" || skillKey == "BRAWL" || skillKey == "MELEE";
	}
};

class Shop : public AbstractDataList
{
public:
	static Shop	instance;

	Shop(QStringList columns);

	virtual int rowCount();
	virtual QVariant getValue(int row, int col);
	virtual QVariant getValue(int row, const char* col);

	void clear();
	void addItem(const ShopItem item);
	ShopItem itemAt(int i);
	bool contains(const QString& key) const;
	const ShopItem getItem(const QString& key) const;
	void search(const QString& key);

protected:
	void addSortedItem(const ShopItem item);

	QMap<QString, ShopItem>	iList;
	QStringList				iSortedKeys;
	QStringList				iSelection;
	ShopItem				iEmptyItem;
};


#endif // __Shop_h__
