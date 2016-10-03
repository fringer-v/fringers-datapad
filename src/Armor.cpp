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
							  << "attachments" << "notes");

Armor::Armor(QStringList columns) :
	ItemList(columns)
{
}

QVariant Armor::getValue(int row, const char* col)
{
	Item item = itemAt(row);

	if (strcmp(col, "armor") == 0)
		return item.name();

	if (strcmp(col, "features") == 0) {
		return QVariant(item.features());
	}

	if (strcmp(col, "soak") == 0)
		return QVariant(item.soakVal());

	if (strcmp(col, "mdef") == 0)
		return QVariant(item.meleeDef());

	if (strcmp(col, "rdef") == 0)
		return QVariant(item.rangeDef());

	if (strcmp(col, "encumbrance") == 0)
		return QVariant(item.encArmorValue());

	return ItemList::getValue(row, col);
}

// ShopArmor -------------------------

void ShopArmor::loadArmor()
{
	QString path;
	QByteArray data;
	//int row;

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

	/*
	QMap<QString, Item>::iterator i;
	for (i = Character::instance->armor.begin(); i != Character::instance->armor.end(); i++) {
		if (Shop::instance.contains(i.key())) {
			ShopItem shop = Character::instance->shopItems[i.key()];
			if (!i.value().loaded) {
				i.value().loaded = true;
				i.value().name = shop.name;
				//i.value().encumbrance = shop.encumbrance;
				//foreach (Mod mod, shop.modList) {
				//	i.value().addMod(mod);
				//	i.value().addQualityFromMod(mod);
				//}
				//i.value().mdef = shop.mdef;
				//i.value().rdef = shop.rdef;
				//i.value().soak = shop.soak;
			}
			Item item = i.value();
			if (!DataList::armor.findSortedRow(row, "armor", item.name)) {
				DataList::armor.insertRow(row);
				DataList::armor.setValue(row, "key", item.key);
				DataList::armor.setValue(row, "armor", item.name);
				//DataList::armor.setValue(row, "features", item.modifications());
				//DataList::armor.setValue(row, "quantity", item.quantity);
				//DataList::armor.setValue(row, "soak", item.soakVal());
				//DataList::armor.setValue(row, "mdef", item.meleeDef());
				//DataList::armor.setValue(row, "rdef", item.rangeDef());
				//DataList::armor.setValue(row, "encumbrance", item.encArmorValue());
				//DataList::armor.setValue(row, "held", item.held);
				//DataList::armor.setValue(row, "equipped", item.equipped);
				//DataList::armor.setValue(row, "restricted", item.restricted);
				//DataList::armor.setValue(row, "attachments", item.attachments);
				//qDebug() << item.name << item.attachments;
			}
		}
	}
	*/
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
	else if (path.endsWith("/BaseMods/Mod/"))
		iMod.clear();
	else if (path.endsWith("/BaseMods/Mod/Key/"))
		iMod.clear(value);
	else if (path.endsWith("/BaseMods/Mod/Count/"))
		iMod.count = toInt(value);
	else if (path.endsWith("/BaseMods/Mod/MiscDesc/"))
		iMod.miscDesc = value; //value.trimmed();
	else if (path.endsWith("/BaseMods/Mod/#end")) {
		iItem.modList[iMod.key] = iMod;
		iItem.qualityList[iMod.key].key = iMod.key;
		iItem.qualityList[iMod.key].count = iMod.count * iMod.number;
	}
	else if (path.endsWith("/Armors/Armor/#end")) {
		iItem.type = "ARMOR";
		Shop::instance.addItem(iItem);
	}
	return true;
}
