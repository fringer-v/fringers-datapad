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

#include "Gear.h"
#include "DataAccess.h"
#include "DatUtil.h"
#include "Character.h"

// Gear -------------------------

Gear Gear::instance = Gear(QStringList() << "uuid" << "itemkey" << "gear" << "quantity" << "stored" << "encumbrance"
						   << "carry_state" << "special" << "notes");

Gear::Gear(QStringList columns) :
	ItemList(columns)
{
}

QVariant Gear::getValue(int row, const char* col)
{
	Item item = itemAt(row);

	if (strcmp(col, "gear") == 0)
		return item.name();

	if (strcmp(col, "encumbrance") == 0) {
		ShopItem shop = Shop::instance.getItem(item.itemkey);
		return QVariant(shop.encumbrance);
	}

	if (strcmp(col, "special") == 0) {
		ShopItem	shop = Shop::instance.getItem(item.itemkey);
		QString		special;
		QString		features;

		features = item.features();
		if (!features.isEmpty())
			DatUtil::appendToList(special, QString("[B]Features:[b] %1").arg(features), " ");
		if (!item.attachments.isEmpty())
			DatUtil::appendToList(special, QString("[B]Attachments:[b] %1").arg(item.attachments), " ");
		if (!item.notes.isEmpty() && item.notes != shop.description)
			DatUtil::appendToList(special, QString("[B]Notes:[b] %1").arg(item.notes), " ");

		DatUtil::addDescription(special, shop.description, shop.books);

		return special;
	}

	return ItemList::getValue(row, col);
}

// ShopGear -------------------------

void ShopGear::loadGear()
{
	QString path;
	QByteArray data;
	QString special;
	QString features;
	//int row;

	// Custom data overwrite standard data, so load last!
	path = DatUtil::getSystemDataFolder() + "Data/Gear.xml";
	data = DataAccess::readFile(path);
	//parse(data);
	readFromBuffer(data.constData(), data.length());

	if (!Character::instance->dataSet().isEmpty()) {
		path = DatUtil::getDataSetFolder() + Character::instance->dataSet() + "/Gear.xml";
		if (QFile::exists(path)) {
			data = DataAccess::readFile(path);
			//parse(data);
			readFromBuffer(data.constData(), data.length());
		}
	}

	/*
	for (i = 0; i < Gear::instance.rowCount(); i++) {
		Item item = Gear::instance.itemAt(i);
		if (Character::instance->Shop::instance.contains(item.key)) {
			ShopItem shop = Shop::instance.getItem(item.key);
			if (!i.value().loaded) {
				i.value().loaded = true;
				i.value().name = shop.name;
				i.value().encumbrance = shop.encumbrance;
				foreach (Mod mod, shop.modList) {
					i.value().addMod(mod);
					i.value().addQualityFromMod(mod);
				}
			}
			Item item = i.value();
			if (!DataList::gear.findSortedRow(row, "gear", item.name)) {
				special = shop.description;
				features = item.modifications();
				if (!features.isEmpty())
					DatUtil::appendToList(special, QString("[B]Features:[b] %1").arg(features), " ");
				if (!item.attachments.isEmpty())
					DatUtil::appendToList(special, QString("[B]Attachments:[b] %1").arg(item.attachments), " ");
				if (!item.notes.isEmpty())
					DatUtil::appendToList(special, QString("[B]Notes:[b] %1").arg(item.notes), " ");
				DataList::gear.insertRow(row);

				DataList::gear.setValue(row, "key", item.key);
				DataList::gear.setValue(row, "gear", item.name);
				DataList::gear.setValue(row, "special", special);
				DataList::gear.setValue(row, "quantity", item.quantity);
				DataList::gear.setValue(row, "encumbrance", item.encumbrance);
				DataList::gear.setValue(row, "held", item.held);
				DataList::gear.setValue(row, "equipped", item.equipped);
			}
		}
	}
	*/
}

void ShopGear::end()
{
}

QString ShopGear::grenBandKey;

bool ShopGear::xmlElement(const DatStringBuffer& path, const char* value)
{
	if (path.endsWith("/Gear/Key/"))
		iItem.clear(value);
	else if (path.endsWith("/Gear/Name/")) {
		iItem.name = value;
		if (strcmp(value, "Grenade Bandolier") == 0)
			grenBandKey = iItem.key;
	}
	else if (path.endsWith("/Gear/Description/")) {
		iItem.description = value; //value.trimmed();
		if (iItem.description.contains("[h3]"))
			iItem.description = DatUtil::right(iItem.description, "[h3]").trimmed();

		if (iItem.description.startsWith("Please see page")) {
			iItem.books = DatUtil::pageSummary(iItem.description);
			iItem.description.clear();
		}
	}
	else if (path.endsWith("/Gear/Encumbrance/"))
		iItem.encumbrance = toInt(value);
	else if (path.endsWith("/Gear/Price/"))
		iItem.price = toInt(value);
	else if (path.endsWith("/Gear/Rarity/"))
		iItem.rarity = toInt(value);
	else if (path.endsWith("/Gear/Type/")) {
		if (strcmp(value, "Cybernetics") == 0)
			iItem.gearType |= GEAR_TYPE_CYBERNETICS;
		else if (strcmp(value, "Carrying/Storage") == 0)
			iItem.gearType |= GEAR_TYPE_STORAGE;
		else if (strcmp(value, "Generic") == 0)
			iItem.description.clear();
	}
	else if (path.endsWith("/BaseMods/Mod/"))
		iMod.clear();
	else if (path.endsWith("/BaseMods/Mod/Key/"))
		iMod.clear(value);
	else if (path.endsWith("/BaseMods/Mod/Count/"))
		iMod.count = toInt(value);
	else if (path.endsWith("/BaseMods/Mod/MiscDesc/")) {
		iMod.miscDesc = QString::fromUtf8(value).trimmed(); // value.trimmed();
		// PACKMIL <MiscDesc>Cumbersome 2</MiscDesc>
		if (iMod.miscDesc.startsWith("Cumbersome ")) {
			if (iMod.key.isEmpty()) {
				iMod.key = "CUMBERSOME";
				iMod.count = DatUtil::right(iMod.miscDesc, "Cumbersome ").toInt();
			}
		}
	}
	else if (path.endsWith("/BaseMods/Mod/#end")) {
		if (!iMod.key.isEmpty()) {
			iItem.modList[iMod.key] = iMod;
			iItem.qualityList[iMod.key].key = iMod.key;
			iItem.qualityList[iMod.key].count = iMod.count * iMod.number;
			// Check PACKMIL, it has a second Mod with no key!
			iMod.clear();
		}
		else if (!iMod.miscDesc.isEmpty()) {
			// Add the MiscDesc to the description
			if (!iMod.miscDesc.endsWith(".") &&
				!iMod.miscDesc.endsWith(";") &&
				!iMod.miscDesc.endsWith(","))
				iMod.miscDesc += ".";
			DatUtil::appendToList(iItem.description, iMod.miscDesc, " ");
		}
	}
	else if (path.endsWith("/Gears/Gear/#end")) {
		iItem.type = "GEAR";
		Shop::instance.addItem(iItem);
	}

	return true;
}
