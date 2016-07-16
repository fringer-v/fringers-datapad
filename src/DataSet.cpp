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

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QPointer>
#include <QSettings>
#include <QDebug>

#include "DataSet.h"
#include "DatUtil.h"
#include "DataAccess.h"

// DataSet ------------------------------------

DataSet DataSet::instance;

DataSet::DataSet() :
	careers(),
	species()
{
}

DataSet::~DataSet()
{
}


void DataSet::loadCareers(QString dset)
{
	bool loaded = false;

	if (!dset.isEmpty())
		loaded = loadCareerNames(DatUtil::getDataSetFolder() + dset + "/Species/");
	if (!loaded)
		loadCareerNames(DatUtil::getSystemDataFolder() + "Data/Careers/");
}

void DataSet::loadSpecies(QString dset)
{
	bool loaded = false;

	if (!dset.isEmpty())
		loaded = loadSpeciesKeys(DatUtil::getDataSetFolder() + dset + "/Species/");
	if (!loaded)
		loadSpeciesKeys(DatUtil::getSystemDataFolder() + "Data/Species/");
}

bool DataSet::loadCareerNames(const QString& dir)
{
	QStringList files = DataAccess::listDirectory(dir);
	QByteArray data;
	QString key;
	QString name;

	if (files.isEmpty())
		return false;

	foreach (QString file, files) {
		data = DataAccess::readFile(DatUtil::addDirChar(dir) + file);
		key = DataAccess::getTagValue(data, "/Career/Key/");
		name = DataAccess::getTagValue(data, "/Career/Name/");
		careers[key] = name;
	}
	return true;
}

bool DataSet::loadSpeciesKeys(const QString& dir)
{
	QStringList files = DataAccess::listDirectory(dir);
	QByteArray data;
	QString key;

	if (files.isEmpty())
		return false;

	foreach (QString file, files) {
		data = DataAccess::readFile(DatUtil::addDirChar(dir) + file);
		key = DataAccess::getTagValue(data, "/Species/Key/");
		species[key] = file;
	}
	return true;
}
