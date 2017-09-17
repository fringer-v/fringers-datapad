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

#ifndef __DatUtil_h__
#define __DatUtil_h__

#include <QMap>
#include <QString>
#include <QByteArray>

#include "KeyMethods.h"

// Some useful connection Macros:
#define CONNECT(fobj, ffunc, tobj, tfunc)	\
	do { \
		bool connect_result = connect(fobj, ffunc, tobj, tfunc); \
		if (!connect_result) \
			Q_ASSERT(connect_result); \
	} while (0)

#define DISCONNECT(fobj, ffunc, tobj, tfunc)	\
	do { \
		bool connect_result = disconnect(fobj, ffunc, tobj, tfunc); \
		if (!connect_result) \
			Q_ASSERT(connect_result); \
	} while (0)

#define CONNECT_TO(fobj, ffunc, tobj, tfunc)		CONNECT(fobj, SIGNAL(ffunc), tobj, SLOT(tfunc))
#define DISCONNECT_FROM(fobj, ffunc, tobj, tfunc)	DISCONNECT(fobj, SIGNAL(ffunc), tobj, SLOT(tfunc))

// Send a signal to the switchboard:
#define SENT_BY(tobj, fsig)							CONNECT(tobj, SIGNAL(fsig), &G_::switchBoard, SIGNAL(fsig))
#define SENDS(fsig)									SENT_BY(this, fsig)

// Receive a signal from the switchboard
#define RECEIVED_BY(fsig, tobj, tslot)				CONNECT(&G_::switchBoard, SIGNAL(fsig), tobj, SLOT(tslot))

#define RECEIVES(fsig, tslot)						RECEIVED_BY(fsig, this, tslot)

#define SPECIES		"SPEC"
#define SOAK		"SOA"
#define WOUND		"WOU"
#define STRAIN		"STR"
#define DRANGED		"DRA"
#define DMELEE		"DME"
#define FORCE		"FOR"
#define XP			"XP"
#define NEWXP		"NEWXP"   // Add to XP to get actual total
#define USEDXP		"USEDXP"
#define MORALITY	"MOR"

struct Injury {
	int			fromPerc;
	int			toPerc;
	int			d10;
	const char* title;
	const char* description;
	const char* dice;
};

class Character;

class DatUtil
{
public:
	static int d(int x);
	static QString timeNow();
	static uint timeNowInSecs();
	static int random();
	static int d20() { return d(20); }
	static const char *cstr(QString s);
	static QString genUuid();

	static QByteArray toBytes(const QString& value);
	static QByteArray stringToBytes(QString value);
	static QString fromBytes(QByteArray& value);
	static QByteArray nextLine(const QByteArray& bytes, int& pos);
	static bool isValidLine(QByteArray line, bool& empty_line, bool& last_line);
	static bool lineAttributes(QByteArray line, QString &name, QString &value);
	static void appendToList(QString& list, const QString& item, const QString& delim = ", ");
	static void makeList(QString& list, QString item, const QString& delim = ",");
	static void appendT(QString& list, const QString& item, const QString& delim = ", ");
	static QString repeat(const QString& list, int count);

	static int firstNumber(QString x);
	static int htmlLength(QString x);
	static bool isNumber(const QString& x);
	static bool isSpace(const QString& x);
	static bool contains(QString x, QString y);
	static QString left(QString x, QString y);
	static QString right(QString x, QString y);
	static QString lastLeft(QString x, QString y);
	static QString lastRight(const QString& x, const QString& y);
	static bool copyDir(const QString& fileName, const QString& newPath, const QString& newName);
	static QString lineBreak(QString message, int length = 60);
	static QString capitalize(QString x);
	static QString unCapitalize(QString x);
	static QByteArray toStandardLineEnding(QByteArray& from);
	static QByteArray toWindowsLineEndings(QByteArray& from);
	static QString poolText(QString x);
	static QString poolText(QString x, int arg);
	static QString unpoolText(QString text);

	static QString addDirChar(QString path);
	static QString lastNameOfPath(const QString& path);
	static QString fileExtension(const QString& path);

	static QString getApplicationData();
	static QString getTempFolder();
	static QString getCharacterFolder();
	static QString getDataSetFolder();
	static QString getCurrentFolder();
	static QString getSystemDataFolder();

	static QString getBasicDicePool(int ranks, int char_value);
	static QString addDiceToText(const QString& path);

	static Injury* getInjury(int perc, int d10);

	static QString normalizeDice(const QString& dice);
	static int betterThan(const QString& skill1, const QString& skill2, Character* charac);
	static int betterThan(MethodID skill1, MethodID skill2, Character* charac);
	static QString base64(const QString& text);
	static void yield();
	static QString pageSummary(QString page_desc);
	static void addDescription(QString& desc, QString descriptio, QString books);

	private:
	static QString getSymbolToken(QString& str);
};

#endif // __DatUtil_h__
