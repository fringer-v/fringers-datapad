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

#include <QtXml>

#include "ItemDescriptors.h"
#include "DataAccess.h"
#include "Skills.h"
#include "DatUtil.h"

QPointer<ItemDescriptors> ItemDescriptors::sInstance;

// ItemDescriptor -------------------------

ItemDescriptor::ItemDescriptor()
{
	clear();
}

void ItemDescriptor::clear(const QString& k)
{
	key = k;
	modDesc.clear();
	qualDesc.clear();
	isQuality = false;
}

// ItemDescriptors -------------------------

ItemDescriptors* ItemDescriptors::instance()
{
	if (sInstance.isNull()) {
		sInstance = new ItemDescriptors;
	}
	return sInstance;
}

void ItemDescriptors::loadItemDescriptors(const QString& dset)
{
	QByteArray	data;
	QString		path;

	iItemDescriptors.clear();
	path = DatUtil::getSystemDataFolder() + "Data/ItemDescriptors.xml";
	data = DataAccess::readFile(path);
	readFromBuffer(data.constData(), data.length());

	if (!dset.isEmpty()) {
		path = DatUtil::getDataSetFolder() + dset + "/ItemDescriptors.xml";
		if (QFile::exists(path)) {
			data = DataAccess::readFile(path);
			readFromBuffer(data.constData(), data.length());
		}
	}

	ItemDescriptor desc;
	desc.clear(BRAWN);
	desc.modDesc = "Character (Brawn)";
	iItemDescriptors[BRAWN] = desc;
	desc.clear(AGILITY);
	desc.modDesc = "Character (Agility)";
	iItemDescriptors[AGILITY] = desc;
	desc.clear(INTELLECT);
	desc.modDesc = "Character (Intellect)";
	iItemDescriptors[INTELLECT] = desc;
	desc.clear(CUNNING);
	desc.modDesc = "Character (Cunning)";
	iItemDescriptors[CUNNING] = desc;
	desc.clear(WILLPOWER);
	desc.modDesc = "Character (Willpower)";
	iItemDescriptors[WILLPOWER] = desc;
	desc.clear(PRESENCE);
	desc.modDesc = "Character (Presence)";
	iItemDescriptors[PRESENCE] = desc;
}

bool ItemDescriptors::xmlElement(const DatStringBuffer& path, const char* value)
{
	//qDebug() << "XML" << path << value;
	if (path.endsWith("/ItemDescriptor/Key/")) {
		iItemDescriptor.clear();
		iItemDescriptor.key = value;
	}
	else if (path.endsWith("/ItemDescriptor/ModDesc/"))
		iItemDescriptor.modDesc = value;
	else if (path.endsWith("/ItemDescriptor/QualDesc/"))
		iItemDescriptor.qualDesc = value;
	else if (path.endsWith("/ItemDescriptor/IsQuality/"))
		iItemDescriptor.isQuality = isTrue(value);
	else if (path.endsWith("/ItemDescriptor/#end"))
		iItemDescriptors[iItemDescriptor.key] = iItemDescriptor;
	return true;
}

ItemDescriptor ItemDescriptors::descriptor(const QString& key)
{
	ItemDescriptor item;

	if (iItemDescriptors.contains(key))
		return iItemDescriptors[key];
	return item;
}
