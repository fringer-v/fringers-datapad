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

#include "ItemAttachments.h"
#include "DataAccess.h"
#include "DatUtil.h"

QPointer<ItemAttachments> ItemAttachments::sInstance;

// ItemAttachment -------------------------

void ItemAttachment::clear()
{
	key.clear();
	name.clear();
}

// ItemAttachments -------------------------

ItemAttachments* ItemAttachments::instance()
{
	if (sInstance.isNull()) {
		sInstance = new ItemAttachments;
	}
	return sInstance;
}

void ItemAttachments::loadItemAttachments(const QString& dset)
{
	QByteArray	data;
	QString		path;

	iItemAttachments.clear();
	path = DatUtil::getSystemDataFolder() + "Data/ItemAttachments.xml";
	data = DataAccess::readFile(path);
	readFromBuffer(data.constData(), data.length());

	if (!dset.isEmpty()) {
		path = DatUtil::getDataSetFolder() + dset + "/ItemAttachments.xml";
		if (QFile::exists(path)) {
			data = DataAccess::readFile(path);
			readFromBuffer(data.constData(), data.length());
		}
	}
}

bool ItemAttachments::xmlElement(const DatStringBuffer& path, const char* value)
{
	//qDebug() << "XML" << path << value;
	if (path.endsWith("/ItemAttachment/Key/")) {
		iItemAttachment.clear();
		iItemAttachment.key = value;
	}
	else if (path.endsWith("/ItemAttachment/Name/"))
		iItemAttachment.name = value;
	else if (path.endsWith("/ItemAttachment/#end"))
		iItemAttachments[iItemAttachment.key] = iItemAttachment;
	return true;
}


ItemAttachment ItemAttachments::attachment(const QString& key)
{
	return iItemAttachments[key];
}
