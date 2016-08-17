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

#ifndef __CharacterList_h__
#define __CharacterList_h__

#include <QObject>

#include "DataList.h"

class ExperienceList : public AbstractDataList {
public:
	static ExperienceList instance;

	ExperienceList();

	virtual int rowCount();
	virtual QVariant getValue(int row, int col);
};

class ObligationList : public AbstractDataList {
public:
	static ObligationList instance;

	ObligationList();

	virtual int rowCount();
	virtual QVariant getValue(int row, int col);

	int total();
};


class DutyList : public AbstractDataList {
public:
	static DutyList instance;

	DutyList();

	virtual int rowCount();
	virtual QVariant getValue(int row, int col);

	int total();
};

#endif // __CharacterList_h__
