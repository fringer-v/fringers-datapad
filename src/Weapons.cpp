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

#include "Weapons.h"
#include "DataAccess.h"
#include "DatUtil.h"
#include "ItemDescriptors.h"
#include "Character.h"

// Weapons -------------------------

Weapons Weapons::instance = Weapons(QStringList() << "uuid" << "itemkey" << "weapon" << "skill" << "range"
									<< "damage" << "critical" << "dicePool" << "qualities" << "quantity" << "stored"
									<< "encumberance" << "carry_state" << "restricted" << "attachments" << "critplus" << "pierce" << "notes");

Weapons::Weapons(QStringList columns) :
	ItemList(columns)
{
}

QVariant Weapons::getValue(int row, const char* col)
{
	Item item = itemAt(row);

	if (strcmp(col, "weapon") == 0)
		return item.name();

	if (strcmp(col, "skill") == 0) {
		Skill* skill = Skill::getSkill(Shop::instance.getItem(item.itemkey).skillKey);
		return skill ? skill->shortName : "?";
	}

	if (strcmp(col, "encumberance") == 0)
		return Shop::instance.getItem(item.itemkey).encumberance;

	if (strcmp(col, "range") == 0)
		return Shop::instance.getItem(item.itemkey).range;

	if (strcmp(col, "damage") == 0)
		return item.damageTotal();

	if (strcmp(col, "critical") == 0)
		return item.critTotal();

	if (strcmp(col, "dicePool") == 0)
		return item.dicePool();

	if (strcmp(col, "qualities") == 0)
		return item.qualities();

	if (strcmp(col, "critplus") == 0)
		return item.critPlus();

	if (strcmp(col, "pierce") == 0)
		return item.pierce();

	return ItemList::getValue(row, col);
}

// ShopWeapons -------------------------

void ShopWeapons::loadWeapons()
{
	QString path;
	QByteArray data;

	// Custom data overwrite standard data, so load last!
	path = DatUtil::getSystemDataFolder() + "Data/Weapons.xml";
	data = DataAccess::readFile(path);
	//parse(data);
	readFromBuffer(data.constData(), data.length());

	if (!Character::instance->dataSet().isEmpty()) {
		path = DatUtil::getDataSetFolder() + Character::instance->dataSet() + "/Weapons.xml";
		if (QFile::exists(path)) {
			data = DataAccess::readFile(path);
			//parse(data);
			readFromBuffer(data.constData(), data.length());
		}
	}
}

void ShopWeapons::end()
{
	ShopItem item;
	Quality q;

	// Unarmed
	item.clear("UNARMED");
	item.name = "Unarmed";
	item.range = "Engaged";
	item.critical = 5;
	item.skillKey = "BRAWL";
	q.clear("DISORIENT");
	q.count = 1;
	item.qualityList[q.key] = q;
	q.clear("KNOCKDOWN");
	item.qualityList[q.key] = q;
	q.clear("STUNSETTING");
	item.qualityList[q.key] = q;
	// Disorient 1, Knockdown, Stun Setting
	Shop::instance.addItem(item);
}

bool ShopWeapons::xmlElement(const DatStringBuffer& path, const char* value)
{
	//qDebug() << "XML" << path.getCString() << value;
	if (path.endsWith("/Weapon/Key/"))
		iItem.clear(value);
	else if (path.endsWith("/Weapon/Name/")) {
		iItem.name = value;
		if (strstr(value, "Grenade"))
			iItem.gearType |= GEAR_TYPE_GRENADE;
	}
	else if (path.endsWith("/Weapon/SkillKey/")) {
		iItem.skillKey = value;
		if (strcmp(value, "RANGLT") == 0 || strcmp(value, "RANGHV") == 0 || strcmp(value, "GUNN") == 0)
			iItem.gearType |= GEAR_TYPE_RANGED;
	}
	else if (path.endsWith("/Weapon/Damage/")) {
		if (toInt(value) > 0)
			iItem.damage = toInt(value);
	}
	else if (path.endsWith("/Weapon/DamageAdd/")) {
		if (toInt(value) > 0)
			iItem.addDamage = toInt(value);
	}
	else if (path.endsWith("/Weapon/Crit/"))
		iItem.critical = toInt(value);
	else if (path.endsWith("/Weapon/RangeValue/")) {
		if (value[0] == 'w' && value[1] == 'r')
			value += 2;
		iItem.range = value;
	}
	else if (path.endsWith("/Weapon/Range/")) {
		if (*value && iItem.range.isEmpty())
			iItem.range = value;
	}
	else if (path.endsWith("/Categories/Category/")) {
		if (strcmp(value, "Ranged") == 0)
			iItem.gearType |= GEAR_TYPE_RANGED;
	}
	else if (path.endsWith("/Weapon/Encumbrance/"))
		iItem.encumberance = toInt(value);
	else if (path.endsWith("/Weapon/Price/"))
		iItem.price = toInt(value);
	else if (path.endsWith("/Weapon/Rarity/"))
		iItem.rarity = toInt(value);
	else if (path.endsWith("/Quality/Key/"))
		iQuality.clear(value);
	else if (path.endsWith("/Quality/Count/"))
		iQuality.count = toInt(value);
	else if (path.endsWith("/Weapon/Type/"))
		iItem.type = value;
	else if (path.endsWith("/Quality/#end"))
		iItem.qualityList[iQuality.key] = iQuality;
	else if (path.endsWith("/Weapons/Weapon/#end")) {
		if (iItem.type != "Vehicle")
			Shop::instance.addItem(iItem);
	}

	return true;
}
