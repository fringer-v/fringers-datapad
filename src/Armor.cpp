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

#include "Character.h"
#include "Armor.h"
#include "DataAccess.h"
#include "Talents.h"
#include "DatUtil.h"

// Armor -------------------------

Armor Armor::instance = Armor(QStringList() << "uuid" << "itemkey" << "armor" << "quantity" << "stored" << "soak"
							  << "mdef" << "rdef" << "encumbrance" << "carry_state" << "retricted" << "features"
							  << "attachments" << "notes" << "qualities");

Armor::Armor(QStringList columns) :
	AbstractDataList(columns)
{
}

int Armor::rowCount()
{
	return CurrentData::instance->armor.rowCount();
}

QVariant Armor::getValue(int row, int col)
{
	Item item;

	if (row < 0)
		return QVariant();
	if (row >= CurrentData::instance->armor.rowCount())
		return QVariant();

	item = CurrentData::instance->armor.itemAt(row);
	switch (col) {
		case 0: // uuid
			return item.uuid;
		case 1: // itemkey
			return item.itemkey;
		case 2: // armor
			return item.name();
		case 3: // quantity
			return item.quantity();
		case 4: // stored
			return item.stored();
		case 5: // soak
			return QVariant(item.soakVal());
		case 6: // mdef
			return QVariant(item.meleeDef());
		case 7: // rdef
			return QVariant(item.rangeDef());
		case 8: // encumbrance
			return item.encumbrance();
		case 9: // carry_state
			return item.state();
		case 10: // retricted
			return item.restricted();
		case 11: // features
			return item.features();
		case 12: // attachments
			return item.attachments;
		case 13: // notes
			return item.notes;
		case 14: // qualities
			return item.qualities();
	}
	return QVariant();
}

// ShopArmor -------------------------

void ShopArmor::loadArmor()
{
	QString path;
	QByteArray data;
	//int row;

	iItemModCount = 0;

	// Custom data overwrite standard data, so load last!
	path = DatUtil::getSystemDataFolder() + "Data/Armor.xml";
	data = DataAccess::readFile(path);
	readFromBuffer(data.constData(), data.length());

	if (!Character::instance->dataSet().isEmpty()) {
		path = DatUtil::getDataSetFolder() + Character::instance->dataSet() + "/Armor.xml";
		if (QFile::exists(path)) {
			data = DataAccess::readFile(path);
			readFromBuffer(data.constData(), data.length());
		}
	}
}

void ShopArmor::end()
{
}

bool ShopArmor::xmlElement(const DatStringBuffer& path, const char* value)
{
	//qDebug() << "XML" << path << value;
	if (path.endsWith("/Armor/Key/"))
		iItem.clear(value);
	else if (path.endsWith("/Armor/Name/"))
		iItem.name = value;
	else if (path.endsWith("/Armor/Soak/"))
		iItem.soak = toInt(value);
	else if (path.endsWith("/Armor/Defense/")) {
		iItem.mdef = toInt(value);
		iItem.rdef = toInt(value);
	}
	else if (path.endsWith("/Armor/Encumbrance/"))
		iItem.encumbrance = toInt(value);
	else if (path.endsWith("/Armor/Price/"))
		iItem.price = toInt(value);
	else if (path.endsWith("/Armor/Rarity/"))
		iItem.rarity = toInt(value);
	else if (path.endsWith("/BaseMods/Mod/#open"))
		iMod.clear();
	else if (path.endsWith("/BaseMods/Mod/Key/"))
		iMod.clear(value);
	else if (path.endsWith("/BaseMods/Mod/Count/"))
		iMod.count = toInt(value);
	else if (path.endsWith("/BaseMods/Mod/MiscDesc/"))
		iMod.miscDesc = value; //value.trimmed();
	else if (path.endsWith("/BaseMods/Mod/#end")) {
		if (iMod.key.isEmpty()) {
			iItemModCount++;
			iMod.key = QString("ARMOR_MOD_%1").arg(iItemModCount);
		}
		else {
			iItem.qualityList[iMod.key].key = iMod.key;
			iItem.qualityList[iMod.key].count = iMod.count * iMod.number;
		}
		iItem.modList[iMod.key] = iMod;
	}
	else if (path.endsWith("/Armors/Armor/#end")) {
		iItem.type = "ARMOR";
		Shop::instance.addItem(iItem);
	}
	return true;
}
