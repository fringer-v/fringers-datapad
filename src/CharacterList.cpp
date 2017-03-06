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
	int count = CurrentData::instance->experienceLog.count();
	return count+1;
}

QVariant ExperienceList::getValue(int row, int col)
{
	ExpLogItem item;
	QString key;
	ExpLogTotal total;

	if (row < 0)
		return QVariant();
	if (row >= CurrentData::instance->experienceLog.count()) {
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

	item = CurrentData::instance->experienceLog[row];
	key = CurrentData::expTypeToString(item.type, item.key);
	total = CurrentData::instance->experienceTotal[key];

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
	return CurrentData::instance->obligations.items.count();
}

QVariant ObligationList::getValue(int row, int col)
{
	CharItem item;

	if (row < 0)
		return QVariant();
	if (row >= CurrentData::instance->obligations.items.count())
		return QVariant();

	item = CurrentData::instance->obligations.items[row];

	switch (col) {
		case 0:
			return item.key;
		case 1:
			return item.name;
		case 2:
			if (CurrentData::instance->experienceTotal.contains(item.key))
				return CurrentData::instance->experienceTotal[item.key].value;
			return item.size;
		case 3:
			return item.notes;
	}
	return QVariant();
}

int ObligationList::total()
{
	int total = 0;

	foreach (CharItem item, CurrentData::instance->obligations.items) {
		if (CurrentData::instance->experienceTotal.contains(item.key))
			total += CurrentData::instance->experienceTotal[item.key].value;
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
	return CurrentData::instance->duties.items.count();
}

QVariant DutyList::getValue(int row, int col)
{
	CharItem item;

	if (row < 0)
		return QVariant();
	if (row >= CurrentData::instance->duties.items.count())
		return QVariant();

	item = CurrentData::instance->duties.items[row];
	switch (col) {
		case 0:
			return item.key;
		case 1:
			return item.name;
		case 2:
			if (CurrentData::instance->experienceTotal.contains(item.key))
				return CurrentData::instance->experienceTotal[item.key].value;
			return item.size;
		case 3:
			return item.notes;
	}
	return QVariant();
}

int DutyList::total()
{
	int total = 0;

	foreach (CharItem item, CurrentData::instance->duties.items) {
		if (CurrentData::instance->experienceTotal.contains(item.key))
			total += CurrentData::instance->experienceTotal[item.key].value;
		else
			total += item.size;
	}
	return total;
}

// SpecialFeaturesList -------------------------------------------

SpecialFeaturesList SpecialFeaturesList::instance = SpecialFeaturesList();

SpecialFeaturesList::SpecialFeaturesList() :
	AbstractDataList(QStringList() << "title" << "subtitle" << "content")
{
}

int SpecialFeaturesList::rowCount()
{
	return CurrentData::instance->specialFeatures.count();
}

QVariant SpecialFeaturesList::getValue(int row, int col)
{
	SpecialFeatureItem item;

	if (row < 0)
		return QVariant();
	if (row >= CurrentData::instance->specialFeatures.count())
		return QVariant();

	item = CurrentData::instance->specialFeatures[row];
	switch (col) {
		case 0:
			return item.title;
		case 1:
			return item.subtitle;
		case 2:
			return item.content;
	}
	return QVariant();
}

// InjuryList -------------------------------------------

InjuryList InjuryList::instance = InjuryList();

InjuryList::InjuryList() :
	AbstractDataList(QStringList() << "ref" << "percent" << "type" << "title"
								   << "description" << "dice")
{
}

int InjuryList::rowCount()
{
	return CurrentData::instance->injuries.count();
}

QVariant InjuryList::getValue(int row, int col)
{
	InjuryItem item;

	if (row < 0)
		return QVariant();
	if (row >= CurrentData::instance->injuries.count())
		return QVariant();

	item = CurrentData::instance->injuries[row];
	Injury* injury = NULL;

	if (col >= 3)
		injury = DatUtil::getInjury(item.percent, item.type);
	switch (col) {
		case 0:
			return item.ref;
		case 1:
			return item.percent;
		case 2:
			return item.type;
		case 3:
			if (injury)
				return injury->title;
			break;
		case 4:
			if (injury)
				return injury->description;
			break;
		case 5:
			if (injury)
				return injury->dice;
			return "EEEE";
	}
	return QVariant();
}

// MotivationList -------------------------------------------

MotivationList MotivationList::instance = MotivationList();

MotivationList::MotivationList() :
	AbstractDataList(QStringList() << "name1" << "name2" << "notes")
{
}

int MotivationList::rowCount()
{
	return CurrentData::instance->motivations.count();
}

QVariant MotivationList::getValue(int row, int col)
{
	MotMorItem item;

	if (row < 0)
		return QVariant();
	if (row >= CurrentData::instance->motivations.count())
		return QVariant();

	item = CurrentData::instance->motivations[row];
	switch (col) {
		case 0:
			return item.name1;
		case 1:
			return item.name2;
		case 2:
			return item.notes;
	}
	return QVariant();
}

// MoralityList -------------------------------------------

MoralityList MoralityList::instance = MoralityList();

MoralityList::MoralityList() :
	AbstractDataList(QStringList() << "name1" << "name2" << "notes")
{
}

int MoralityList::rowCount()
{
	return CurrentData::instance->moralities.count();
}

QVariant MoralityList::getValue(int row, int col)
{
	MotMorItem item;

	if (row < 0)
		return QVariant();
	if (row >= CurrentData::instance->moralities.count())
		return QVariant();

	item = CurrentData::instance->moralities[row];
	switch (col) {
		case 0:
			return item.name1;
		case 1:
			return item.name2;
		case 2:
			return item.notes;
	}
	return QVariant();
}


