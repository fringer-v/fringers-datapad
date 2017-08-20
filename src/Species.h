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

#ifndef __Species_h__
#define __Species_h__

#include <QObject>
#include <QMap>

#include "DatXmlReader.h"
#include "Character.h"

class AllTalents;

class SpeciesXML : public DatXmlReader {
public:
	SpeciesXML(CurrentData* current_data);

	QString loadSpecies(const QString& nkey, const QString& sub_nkey, QMap<QString, QString>* choices);

	virtual void end();
	virtual bool xmlElement(const DatStringBuffer& path, const char* value);

private:
	CurrentData* iCurrentData;

	QString iKey;
	QString	iSubKey;
	QString iName;
	QString	iSubName;
	DieMod iDieMod;
	DieModList iDieModList;
	CharTalent iCharTalent;
	QMap<QString, QString>*	iChoices;
	bool iSubKeyMatched;
	int iOptionsRow;

	QString iChoiceKey;
	QString iOptionKey;
	QString iOptionTitle;
	QString iOptionSubTitle;
	QString iOptionContent;

	SpeciesTalent iSpecTalent;
};

#endif // __Species_h__
