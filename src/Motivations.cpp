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

#include "Motivations.h"
#include "DataAccess.h"

// Motivations -------------------------

Motivations Motivations::instance;

void Motivations::loadMotivations(QString dset)
{
	QByteArray data = DataAccess::getData(dset, "Motivations.xml");
	motivationList.clear();
	readFromBuffer(data.constData(), data.length());
	data = DataAccess::getData(dset, "SpecificMotivations.xml");
	readFromBuffer(data.constData(), data.length());
}

bool Motivations::xmlElement(const DatStringBuffer& path, const char* value)
{
	//qDebug() << "---- xmlElement" << path.getCString() << value;
	if (path.endsWith("/Motivation/Key/")) {
		iKey = value;
	}
	if (path.endsWith("/Motivation/Name/")) {
		motivationList[iKey] = value;
	}
	else if (path.endsWith("/SpecificMotivation/Key/")) {
		iKey = value;
	}
	else if (path.endsWith("/SpecificMotivation/Name/")) {
		motivationList[iKey] = value;
	}
	return true;
}
