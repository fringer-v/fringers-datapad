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

#ifndef __Weapons_h__
#define __Weapons_h__

#include "DatXmlReader.h"
#include "Item.h"
#include "DataList.h"
#include "Shop.h"

class Weapons : public AbstractDataList {
public:
	static Weapons instance;

	Weapons(QStringList columns);

	virtual int rowCount();
	virtual QVariant getValue(int row, int col);

	static QString toRangeText(int range);
	static QString toRangeText(int range, int range2);

	//virtual QVariant getValue(int row, const char* col);
};

class ShopWeapons : public QObject, public DatXmlReader {
	Q_OBJECT

public:
	void loadWeapons();

	virtual void end();
	virtual bool xmlElement(const DatStringBuffer& path, const char* value);

private:
	Quality		iQuality;
	ShopItem	iItem;
};

#endif // __Weapons_h__
