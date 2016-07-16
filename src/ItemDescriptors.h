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

#ifndef __ItemDescriptors_h__
#define __ItemDescriptors_h__

#include <QObject>
#include <QMap>

#include "DatXMLReader.h"

class ItemDescriptor {
public:
	QString key;
	QString modDesc;
	QString qualDesc;
	bool isQuality;

	ItemDescriptor();
	void clear(const QString& k = QString());
};

class ItemDescriptors : public QObject, public DatXMLReader {
	Q_OBJECT

public:
	static ItemDescriptors* instance();

	void loadItemDescriptors(const QString& dset);

	virtual bool xmlElement(const DatStringBuffer& path, const char* value);

	ItemDescriptor descriptor(const QString& key);

private:
	static QPointer<ItemDescriptors> sInstance;

	ItemDescriptor				iItemDescriptor;
	QMap<QString, ItemDescriptor> iItemDescriptors;
	QString							iItemDescriptorKey;
};

#endif // __ItemDescriptors_h__
