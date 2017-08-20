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
									<< "encumbrance" << "carry_state" << "restricted" << "attachments" << "critplus"
									<< "pierce" << "notes" << "features");

Weapons::Weapons(QStringList columns) :
	AbstractDataList(columns)
{
}

int Weapons::rowCount()
{
	return CurrentData::instance->weapons.rowCount();
}

QVariant Weapons::getValue(int row, int col)
{
	Item item;

	if (row < 0)
		return QVariant();
	if (row >= CurrentData::instance->weapons.rowCount())
		return QVariant();

	item = CurrentData::instance->weapons.itemAt(row);
	switch (col) {
		case 0: // uuid
			return item.uuid;
		case 1: // itemkey
			return item.itemkey;
		case 2: // weapon
			return item.name();
		case 3: { // skill
			Skill* skill = Skill::getSkill(item.shopItem().skillKey);
			return skill ? skill->shortName : "?";
		}
		case 4: // range
			return item.shopItem().range;
		case 5: // damage
			return item.damageTotal();
		case 6: // critical
			return item.critTotal();
		case 7: // dicePool
			return item.dicePool();
		case 8: // qualities
			return item.qualities();
		case 9: // quantity
			return item.quantity();
		case 10: // stored
			return item.stored();
		case 11: // encumbrance
			return item.shopItem().encumbrance;
		case 12: // carry_state
			return item.state();
		case 13: // retricted
			return item.restricted();
		case 14: // attachments
			return item.attachments;
		case 15: // critplus
			return item.critPlus();
		case 16: // pierce
			return item.pierce();
		case 17: // notes
			return item.notes;
		case 18: { // features
			const ShopItem shop = item.shopItem();
			QString features;

			features = item.features();
			DatUtil::appendToList(features, shop.description, " ");
			return features;
		}
	}
	return QVariant();
}

QString Weapons::toRangeText(int range)
{
	// Engaged, short, medium, long, extreme.
	switch (range) {
		case 0:
			return "Engaged";
		case 1:
			return "Short";
		case 2:
			return "Medium";
		case 3:
			return "Long";
	}
	if (range < 0)
		return "Engaged";
	return "Extreme";
}

QString Weapons::toRangeText(int range, int range2)
{
	if (range2 < 0)
		return toRangeText(range);
	return QString("%1 (%2)").arg(toRangeText(range)).arg(toRangeText(range2));
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
		iItem.encumbrance = toInt(value);
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
