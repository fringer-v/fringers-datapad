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

#include "Character.h"
#include "CharacterList.h"

// ExperienceList -------------------------------------------

ExperienceList ExperienceList::instance = ExperienceList();

ExperienceList::ExperienceList() :
	AbstractDataList(QStringList() << "ref" << "type" << "key" << "name" << "desc" << "amount" << "base" << "ro" << "del")
{
}

int ExperienceList::rowCount()
{
	int count = Character::instance->currentData()->experienceLog.count();
	return count+1;
}

QVariant ExperienceList::getValue(int row, int col)
{
	ExpLogItem item;
	QString key;
	ExpLogTotal total;

	if (row < 0)
		return QVariant();
	if (row >= Character::instance->currentData()->experienceLog.count()) {
		switch (col) {
			case 0:
				return 0;
			case 1:
			case 2:
			case 3:
				break;
			case 4: {
				// XP, Duty, Obligation, Morality
				bool morality_applies = Character::instance->getAttribute(FORCE) > 0 ||
					Character::instance->morality() != 50;

				return QString("XP: %1, Obligation: %2, Duty: %3, Morality: %4")
					.arg(Character::instance->totalXP())
					.arg(ObligationList::instance.total())
					.arg(DutyList::instance.total())
					.arg(morality_applies ? QString::number(Character::instance->morality()) + "%" : "n/a");
			}
			case 5:
				return 0;
			case 6:
				return 0;
			case 7:
				return false;
			case 8:
				return false;
		}
		return QVariant();
	}

	item = Character::instance->currentData()->experienceLog[row];
	key = CurrentData::expTypeToString(item.type, item.key);
	total = Character::instance->currentData()->experienceTotal[key];

	switch (col) {
		case 0:
			return item.ref;
		case 1:
			return CurrentData::expTypeToString(item.type);
		case 2:
			return item.key;
		case 3:
			return item.name;
		case 4:
			return item.desc;
		case 5:
			return item.amount;
		case 6:
			return total.value;
		case 7:
			return row == total.firstItem;
		case 8:
			return total.itemCount == 1 || row != total.firstItem;
	}
	return QVariant();
}

// ObligationList -------------------------------------------

ObligationList ObligationList::instance = ObligationList();

ObligationList::ObligationList() :
	AbstractDataList(QStringList() << "key" << "name" << "size" << "notes")
{
}

int ObligationList::rowCount()
{
	return Character::instance->obligations.items.count();
}

QVariant ObligationList::getValue(int row, int col)
{
	CharItem item;

	if (row < 0)
		return QVariant();
	if (row >= Character::instance->obligations.items.count())
		return QVariant();

	item = Character::instance->obligations.items[row];

	switch (col) {
		case 0:
			return item.key;
		case 1:
			return item.name;
		case 2:
			if (Character::instance->currentData()->experienceTotal.contains(item.key))
				return Character::instance->currentData()->experienceTotal[item.key].value;
			return item.size;
		case 3:
			return item.notes;
	}
	return QVariant();
}

int ObligationList::total()
{
	int total = 0;

	foreach (CharItem item, Character::instance->obligations.items) {
		if (Character::instance->currentData()->experienceTotal.contains(item.key))
			total += Character::instance->currentData()->experienceTotal[item.key].value;
		else
			total += item.size;
	}
	return total;
}

// DutyList -------------------------------------------

DutyList DutyList::instance = DutyList();

DutyList::DutyList() :
	AbstractDataList(QStringList() << "key" << "name" << "size" << "notes")
{
}

int DutyList::rowCount()
{
	return Character::instance->duties.items.count();
}

QVariant DutyList::getValue(int row, int col)
{
	CharItem item;

	if (row < 0)
		return QVariant();
	if (row >= Character::instance->duties.items.count())
		return QVariant();

	item = Character::instance->duties.items[row];
	switch (col) {
		case 0:
			return item.key;
		case 1:
			return item.name;
		case 2:
			if (Character::instance->currentData()->experienceTotal.contains(item.key))
				return Character::instance->currentData()->experienceTotal[item.key].value;
			return item.size;
		case 3:
			return item.notes;
	}
	return QVariant();
}

int DutyList::total()
{
	int total = 0;

	foreach (CharItem item, Character::instance->duties.items) {
		if (Character::instance->currentData()->experienceTotal.contains(item.key))
			total += Character::instance->currentData()->experienceTotal[item.key].value;
		else
			total += item.size;
	}
	return total;
}


