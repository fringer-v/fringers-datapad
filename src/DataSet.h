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

#ifndef __DataSet_h__
#define __DataSet_h__

#include "DataSet.h"

class DataSet {
public:
	DataSet();
	virtual ~DataSet();

	static DataSet instance;

	void clear() {
		careers.clear();
		species.clear();
	}

	void loadCareers(QString dset);
	void loadSpecies(QString dset);

	QMap<QString, QString>	careers;
	QMap<QString, QString>	species;

private:
	bool loadCareerNames(const QString& dir);
	bool loadSpeciesKeys(const QString& dir);
};

#endif // __Shop_h__
