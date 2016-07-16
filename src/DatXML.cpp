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

#include "DatXML.h"
#include "DatUtil.h"
#include "Character.h"

// DatXML -------------------------

DatXML::DatXML() :
	//QObject(),
	QXmlDefaultHandler(),
	tagStarted(false)
{
}

DatXML::~DatXML()
{
}

void DatXML::parse(const QByteArray& bytes)
{
	QXmlSimpleReader	xmlReader;
	QXmlInputSource		*source = new QXmlInputSource();

	tagStarted = false;

	xmlReader.setContentHandler(this);
	xmlReader.setErrorHandler(this);

	source->setData(bytes);
	xmlReader.parse(source);
	if (bytes.isEmpty())
		end();

	delete source;
}

void DatXML::start()
{
}

void DatXML::end()
{
}

bool DatXML::xmlElement(const QString& path, const QString& value)
{
	Q_UNUSED(path);
	Q_UNUSED(value);
	return true;
}

void DatXML::setError(const QString& e)
{
	error = e;
}

bool DatXML::startDocument()
{
	push("/");
	start();
	return true;
}

bool DatXML::endDocument()
{
	end();
	pop("/");
	return true;
}

bool DatXML::startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts)
{
	bool	ok = true;
	QString	name;

	Q_UNUSED(namespaceURI);
	Q_UNUSED(localName);

	DatUtil::yield();

	if (tagStarted) {
		if (!xmlElement(path, QString()))
			return false;
	}
	push(qName + "/");
	tagStarted = true;
	for (int i=0; i<atts.count() && ok; i++) {
		name = atts.qName(i);
		push(name);
		ok = xmlElement(path, atts.value(i));
		pop(name);
	}
	return ok;
}

bool DatXML::endElement(const QString& namespaceURI, const QString& localName, const QString& qName)
{
	Q_UNUSED(namespaceURI);
	Q_UNUSED(localName);

	if (tagStarted)
		xmlElement(path, QString());
	xmlElement(path + "#end", "");
	tagStarted = false;
	pop(qName + "/");
	return true;
}

bool DatXML::characters(const QString &ch)
{
	QString value = ch.trimmed();

	if (value.isEmpty() && !tagStarted)
		return true;
	tagStarted = false;
	return xmlElement(path, value);
}

/*
 * QString DatXML::errorString()
{
	return error;
}
*/

void DatXML::push(QString name)
{
	path += name;
}

void DatXML::pop(QString name)
{
	Q_ASSERT(path.endsWith(name));
	path = DatUtil::lastLeft(path, name);
}
