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

#include <QStringListIterator>

#include <string.h>

#include "Character.h"
#include "Shop.h"

// Shop ------------------------------------

Shop Shop::instance = Shop(QStringList() << "key" << "name" << "price" << "rarity");

Shop::Shop(QStringList columns) :
	AbstractDataList(columns),
	iList(),
	iSortedKeys(),
	iSelection()
{
}

int Shop::rowCount()
{
	return iSelection.size();
}

QVariant Shop::getValue(int row, int col)
{
	if (col < 0 || col >= colCount())
		return QVariant();
	return getValue(row, iColumns[col].toUtf8().constData());
}

QVariant Shop::getValue(int row, const char* col)
{
	ShopItem item = itemAt(row);

	if (strcmp(col, "key") == 0)
		return item.key;
	if (strcmp(col, "name") == 0)
		return item.name;
	if (strcmp(col, "price") == 0)
		return item.price;
	if (strcmp(col, "rarity") == 0)
		return item.rarity;

	return QVariant();
}

void Shop::clear()
{
	iList.clear();
	iSortedKeys.clear();
	iSelection.clear();
}

void Shop::addItem(const ShopItem& item)
{
	iList[item.key] = item;
	iSortedKeys.clear();
	iSelection.clear();
}

ShopItem Shop::itemAt(int row)
{
	if (row < 0 || row >= rowCount())
		return ShopItem();
	return iList[iSelection[row]];
}

bool Shop::contains(const QString& key)
{
	return !key.isEmpty() && iList.contains(key);
}

ShopItem Shop::getItem(const QString& key)
{
	if (contains(key))
		return iList[key];
	return ShopItem();
}

void Shop::search(const QString& search)
{
	if (iSortedKeys.size() == 0) {
		QMapIterator<QString, ShopItem> i(iList);

		while (i.hasNext()) {
			i.next();
			addSortedItem(i.value());
		}
	}

	QStringListIterator i(iSortedKeys);
	const QString* key;

	setRowCountChanged();
	iSelection.clear();
	if (search.length() >= 3) {
		while (i.hasNext()) {
			key = &i.next();
			if (iList[*key].name.contains(search, Qt::CaseInsensitive)) {
				iSelection.append(*key);
			}
		}
	}
	makeClean();
}

void Shop::addSortedItem(const ShopItem& item)
{
	for (int i=0; i<iSortedKeys.size(); i++) {
		ShopItem b = iList[iSortedKeys[i]];
		if (b.name > item.name) {
			iSortedKeys.insert(i, item.key);
			return;
		}
	}
	iSortedKeys.append(item.key);
}
