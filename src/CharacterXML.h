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

#ifndef __CreatureXML_h__
#define __CreatureXML_h__

#include <QPointer>

#include "DatXML.h"
#include "DatXMLReader.h"
#include "Item.h"
#include "Character.h"
#include "Talents.h"
#include "Weapons.h"
#include "Armor.h"
#include "Gear.h"
#include "Species.h"
#include "Moralities.h"
#include "Talents.h"
#include "Shop.h"

class CharacterXML : public DatXMLReader {
public:
	CharacterXML();

	void start();
	void end();
	//virtual bool xmlElement(const QString& path, const QString& value);
	virtual bool xmlElement(const DatStringBuffer& path, const char* value);

private:
	QPointer<Species>		iSpecies;

	bool					iNPC;
	QString					iSpecialization;
	QString					iSpecializations;
	QString					iAttribute;
	int						iAttrValue;

	int						iMotivationRow;
	int						iMoralityRow;

	QString					iSpeciesKey;
	QString					iSubSpeciesKey;
	QString					iChoiceKey;
	QMap<QString, QString>	iChoices;

	bool					iPurchased;
	CharTalent				iCharTalent;
	CharSkill				iCharSkill;
	Talent					iTalent;
	CharItem				iCharItem;

	Item					iItem;
	Mod						iMod;
	QString					iAttachment;
	ModList					iModList;
	DieMod					iDieMod;
	ShopItem				iShopItem;
};

class ListCharXML : public DatXMLReader {
public:
	void load(const QByteArray& data, const QString& file);
	virtual bool xmlElement(const DatStringBuffer& path, const char* value);

	QString file;
	QString name;
	QString player;
	QString portrait;

	QString lastChanged();

private:
	QString iLastChanged;
};

#endif // CREATUREXML_H