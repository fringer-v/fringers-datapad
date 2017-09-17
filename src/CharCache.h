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

#ifndef __CharCache_h__
#define __CharCache_h__

#include <QObject>

#include "CurrentData.h"

class CharCache {
public:
	static CharCache instance;

	CharCache();

	CurrentData* loadCharFile(const QString& file);
	void clearCache();

private:
	CurrentData* removeChar(const QString& file);
	void addChar(CurrentData* current_char);
	CurrentData* findChar(const QString& file);
	void deleteCharFile(const QString& file);
	void deleteAll();
	void reduceCache();

	QString iCurrentFile;
	int iPCs;
	int iNPCs;
	CurrentData* iLRU;
	CurrentData* iMRU;
};

#endif // __CharCache_h__
