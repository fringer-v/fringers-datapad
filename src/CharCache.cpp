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
 * 2017-03-07 Paul McCullagh
 *
 * H&G2JCtL
 *
 */

#include <string.h>

#include <QtXml>

#include "CharCache.h"
#include "CharacterXML.h"

CharCache CharCache::instance;

CharCache::CharCache() :
	iPCs(0),
	iNPCs(0),
	iLRU(NULL),
	iMRU(NULL)
{
}

CurrentData* CharCache::loadCharFile(const QString& file)
{
	CurrentData* current_data;

	if (file.isEmpty()) {
		iCurrentFile.clear();
		return new CurrentData();
	}

	if ((current_data = removeChar(file))) {
		if (iCurrentFile != file) {
			addChar(current_data);
			iCurrentFile = file;
			return current_data;
		}
	}

	current_data = new CurrentData();
	current_data->characterFile = file;

	QByteArray data;
	CharacterXML char_xml(current_data);

	data = DataAccess::readFile(DatUtil::getCharacterFolder() + file);
	char_xml.readFromBuffer(data.constData(), data.length());

	addChar(current_data);
	iCurrentFile = file;
	return current_data;
}

void CharCache::clearCache()
{
	CurrentData* current_char = removeChar(iCurrentFile);
	deleteAll();
	if (current_char)
		addChar(current_char);
}

CurrentData* CharCache::removeChar(const QString& file)
{
	CurrentData* ptr = iLRU;
	CurrentData* pptr = NULL;

	while (ptr) {
		if (ptr->characterFile == file)
			goto found;
		pptr = ptr;
		ptr = ptr->iNextInCache;
	}
	return NULL;

	found:
	if (pptr)
		pptr->iNextInCache = ptr->iNextInCache;
	else
		iLRU = iLRU->iNextInCache;
	if (iMRU == ptr)
		iMRU = pptr;
	ptr->iNextInCache = NULL;
	if (ptr->npc)
		iNPCs--;
	else
		iPCs--;
	return ptr;
}

void CharCache::addChar(CurrentData* current_char)
{
	current_char->iNextInCache = NULL;
	if (iMRU)
		iMRU->iNextInCache = current_char;
	else
		iLRU = current_char;
	iMRU = current_char;
	if (current_char->npc)
		iNPCs++;
	else
		iPCs++;
	reduceCache();
}

CurrentData* CharCache::findChar(const QString& file)
{
	CurrentData* current_char = removeChar(file);
	if (current_char)
		addChar(current_char);
	return current_char;
}

void CharCache::deleteAll()
{
	CurrentData* ptr = iLRU;
	CurrentData* pptr;

	while (ptr) {
		pptr = ptr;
		ptr = ptr->iNextInCache;
		delete pptr;
	}
	iMRU = NULL;
	iMRU = NULL;
	iNPCs = 0;
	iPCs = 0;
}

void CharCache::reduceCache()
{
	CurrentData* ptr = iLRU;
	bool remove_npc;

	if (iNPCs > 5)
		remove_npc = true;
	else if (iPCs > 3)
		remove_npc = false;
	else
		return;

	while (ptr) {
		if ((remove_npc && ptr->npc) || (!remove_npc && !ptr->npc)) {
			removeChar(ptr->characterFile);
			return;
		}
		ptr = ptr->iNextInCache;
	}
}
