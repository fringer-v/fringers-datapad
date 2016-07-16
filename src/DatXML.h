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

#ifndef __DatXML_h__
#define __DatXML_h__

#include <QObject>
#include <QXmlDefaultHandler>
#include <QXmlAttributes>

class DatXML : public QXmlDefaultHandler
{
//	Q_OBJECT

public:
	DatXML();
	virtual ~DatXML();

	void parse(const QByteArray& bytes);

	virtual void start();
	virtual void end();
	virtual bool xmlElement(const QString& path, const QString& value);
	virtual void setError(const QString& error);

protected:
	bool startDocument();
	bool endDocument();
	bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts);
	bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
	bool characters(const QString &ch);
	//QString errorString();

private:
	void push(QString name);
	void pop(QString name);

	QString path;
	QString error;
	bool	tagStarted;
};

#endif // __DatXML_h__
