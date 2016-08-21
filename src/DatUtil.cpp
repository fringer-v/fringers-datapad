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

#include <QtGui>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "DatUtil.h"
#include "ImageProvider.h"

// WindowsAndDialogs -------------------------

int DatUtil::d(int x)
{
	return (rand() % x) + 1;
}

QString DatUtil::timeNow()
{
	QString str;

	str = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
	return str;
}

uint DatUtil::timeNowInSecs()
{
	return QDateTime::currentDateTimeUtc().toTime_t();
}

int DatUtil::random()
{
	return rand();
}

const char *DatUtil::cstr(QString s)
{
	return s.toUtf8().constData();
}

int DatUtil::firstNumber(QString x)
{
	int i=0;

	while (i<x.size() && x[i].isSpace())
		i++;

	while (i<x.size() && (x[i] == QChar::fromLatin1('-') || x[i] == QChar::fromLatin1('+')))
		i++;

	while (i<x.size() && x[i].isDigit())
		i++;

	return x.left(i).toInt();
}

int DatUtil::htmlLength(QString x)
{
	int i = 0;
	int len = 0;

	while (i<x.size()) {
		if (x[i] == QChar::fromLatin1('<')) {
			i++;
			while (i<x.size() && x[i] != QChar::fromLatin1('>'))
				i++;
			if (i == x.size())
				break;
			i++;
		}
		else {
			i++;
			len++;
		}
	}
	return len;
}

bool DatUtil::isNumber(const QString& x)
{
	QChar ch;
	for (int i=0; i<x.size(); i++) {
		ch = x[i];
		if (ch.isDigit())
			continue;
		if (ch == QChar::fromLatin1(',') || ch == QChar::fromLatin1('-') || ch == QChar::fromLatin1('+') || ch == QChar::fromLatin1('.'))
			continue;
		return false;
	}
	return true;
}

bool DatUtil::isSpace(const QString& x)
{
	QChar ch;

	for (int i=0; i<x.size(); i++) {
		ch = x[i];
		if (!ch.isSpace())
			return false;
	}
	return true;
}

bool DatUtil::contains(QString x, QString y)
{
	return x.indexOf(y) >= 0;
}

QString DatUtil::left(QString x, QString y)
{
	int i;

	i = x.indexOf(y);
	if (i >= 0)
		return x.left(i);

	return x;
}

QString DatUtil::right(QString x, QString y)
{
	int i;

	i = x.indexOf(y);
	if (i >= 0)
		return x.right(x.size() - i - y.size());

	return "";
}

QString DatUtil::lastLeft(QString x, QString y)
{
	int i;

	i = x.lastIndexOf(y);
	if (i >= 0)
		return x.left(i);

	return "";
}

QString DatUtil::lastRight(const QString& x, const QString& y)
{
	int i;

	i = x.lastIndexOf(y);
	if (i >= 0)
		return x.right(x.size() - i - y.size());

	return x;
}

bool DatUtil::copyDir(const QString& dirName, const QString& newDirPath, const QString& newDirName)
{
	QDir			dir;
	QFileInfo		fileInfo;
	QFileInfoList	list = dir.entryInfoList();
	QString			source;
	QString			dest;

	dir = QDir(dirName);
	if (!dir.exists())
		return false;
	list = dir.entryInfoList();
	if (list.size() == 0)
		return false;

	dir = QDir(newDirPath);
	if (!dir.exists(newDirName) && !dir.mkdir(newDirName))
		return false;
	dir.cd(newDirName);
	dest = dir.path();

	foreach (fileInfo, list) {
		if (!fileInfo.fileName().startsWith(".")) {
			source = dirName + QDir::separator() + fileInfo.fileName();
			if (fileInfo.isDir()) {
				if (!copyDir(source, dest, fileInfo.fileName()))
					return false;
			}
			else {
				QString dest_file;

				dest_file = dest + QDir::separator() + fileInfo.fileName();
				if (!QFile::exists(dest_file) && !QFile::copy(source, dest_file))
					return false;
			}
		}
	}
	return true;
}

#define POS_SPACE			1
#define POS_SLASH			2
#define POS_PARAGRAPH		3

struct LinePos {
	int type;
	int offset;
	int letterCount;
};

QString DatUtil::lineBreak(QString message, int length)
{
	QString			buffer;
	int				offset;
	int				letterCount;
	LinePos			pos;
	QList<LinePos>	pos_list;

	offset = 0;
	letterCount = 0;
	while (offset < message.size()) {
		// This loop for each letter
		if (message[offset] == QChar::fromLatin1('<')) {
			QString tag;
			
			pos.offset = buffer.size();
			pos.letterCount = letterCount;

			tag += message[offset];
			offset++;
			while (offset < message.size()) {
				tag += message[offset];
				if (message[offset] == QChar::fromLatin1('>'))
					break;
				offset++;
			}
			offset++;

			if (tag == "<br>") {
				buffer += QChar::fromLatin1(' ');
				letterCount++;
			}
			else {
				buffer += tag;
				if (tag == "<p>") {
					pos.type = POS_PARAGRAPH;
					pos_list.append(pos);
				}
			}	
		}
		else if (message[offset] == QChar::fromLatin1('&')) {
			QString tag;
			
			tag += message[offset];
			offset++;
			while (offset < message.size()) {
				tag += message[offset];
				if (message[offset] == QChar::fromLatin1(';'))
					break;
				offset++;
			}
			offset++;

			buffer += tag;
			letterCount++;
		}
		else if (message[offset].isSpace()) {
			pos.offset = buffer.size();
			pos.letterCount = letterCount;
			pos.type = POS_SPACE;
			pos_list.append(pos);

			buffer += QChar::fromLatin1(' ');
			offset++;
			letterCount++;
			
			// Ignore addition spaces, after the first:
			while (offset < message.size()) {
				if (!message[offset].isSpace())
					break;
				offset++;
			}
		}
		else if (message[offset] == QChar::fromLatin1('/') || message[offset] == QChar::fromLatin1('\\')) {
			pos.offset = buffer.size();
			pos.letterCount = letterCount;
			pos.type = POS_SLASH;
			pos_list.append(pos);

			buffer += message[offset];
			offset++;
			letterCount++;
		}
		else {
			buffer += message[offset];
			offset++;
			letterCount++;
		}
	}

	QString result;

	int lastLetterCount = 0;
	int lastOffset = 0;
	for (int i=0; i<pos_list.size(); i++) {
		if (pos_list[i].type == POS_PARAGRAPH) {
			result += buffer.midRef(lastOffset, pos_list[i].offset - lastOffset + 3);

			lastLetterCount = pos_list[i].letterCount;
			lastOffset = pos_list[i].offset+3;
		}
		else {
			if (pos_list[i].letterCount - lastLetterCount >= length) {
				int diff = (pos_list[i].letterCount - lastLetterCount) - length;
				if (i > 0) {
					int diff2 = length - (pos_list[i-1].letterCount - lastLetterCount);
					if (diff2 < diff) {
						//diff = diff2;
						i--; 
					}
				}
				result += buffer.midRef(lastOffset, pos_list[i].offset - lastOffset);
				result += "<br>";
		
				lastLetterCount = pos_list[i].letterCount;
				lastOffset = pos_list[i].offset;
				if (pos_list[i].type == POS_SPACE) {
					lastLetterCount++;
					lastOffset++;
				}
			}
		}
	}
	if (lastOffset < buffer.size())
		result += buffer.midRef(lastOffset, buffer.size() - lastOffset);

	return result;
}

QString DatUtil::capitalize(QString x)
{
	QChar ch = x[0];

	return x.toLower().replace(0, 1, ch.toUpper());
}

QString DatUtil::unCapitalize(QString x)
{
	QChar ch = x[0];

	return x.replace(0, 1, ch.toLower());
}

QString DatUtil::genUuid()
{
	static uint counter = rand();
	counter++;
	return QString("#%1-%2-%3").arg(time(NULL) % 10000000).arg(rand() % 1000000).arg(counter % 100000);
}

QByteArray DatUtil::toBytes(const QString& value)
{
	QByteArray from;
	QByteArray to;

	from = value.toUtf8().constData();
	for (int i=0; i<from.size(); i++) {
		switch ((char) from[i]) {
			case '\n':
				to.append("\\n");
				break;
			case '\r':
				to.append("\\r");
				break;
			case '\\':
				to.append("\\\\");
			default:
				to.append(from[i]);
		}
	}
	return to;
}

QByteArray DatUtil::stringToBytes(QString value)
{
	return toBytes(value);
}

QString DatUtil::fromBytes(QByteArray& from)
{
	QByteArray to;

	for (int i=0; i<from.size(); i++) {
		if (from[i] == '\\') {
			i++;
			switch (from[i]) {
				case 'n':
					to.append("\n");
					break;
				case 'r':
					to.append("\r");
					break;
				default:
					to.append(from[i]);
					break;
			}
		}
		else
			to.append(from[i]);
	}
	return QString::fromUtf8(to);
}

QByteArray DatUtil::nextLine(const QByteArray& bytes, int& pos)
{
	QByteArray line;

	while (pos < bytes.size() && (bytes[pos] == '\n' || bytes[pos] == '\r'))
		pos++;
	if (pos == bytes.size())
		return line;
	while (pos < bytes.size() && bytes[pos] != '\n' && bytes[pos] != '\r') {
		line.append(bytes[pos]);
		pos++;
	}
	return line;
}

bool DatUtil::isValidLine(QByteArray line, bool& empty_line, bool& last_line)
{
	int i=0;

	empty_line = true;
	last_line = true;

	while (i < line.size() && isspace(line[i]))
		i++;

	if (i == line.size() || line[i] == '#')
		// Empty line or comment
		return true;

	empty_line = false;

	int ch = line[i];
	if (ch == '=')
		return false;

	while (i < line.size()) {
		ch = line[i];
		if (ch == '=') {
			last_line = false;
			return true;
		}
		if ((ch < 'a' || ch > 'z') && ch != QChar::fromLatin1('-') && ch != '/')
			return false;
		i++;
	}

	// Last line (i.e. valid name, but cut off)
	return false;
}

/*
 * Return false if this line is invalid
 */
bool DatUtil::lineAttributes(QByteArray line, QString &ret_name, QString &ret_value)
{
	int i=0;

	QByteArray name;
	QByteArray value;

	while (i < line.size() && isspace(line[i]))
		i++;

	if (i == line.size() || line[i] == '#')
		goto exit_ok;

	while (i < line.size() && line[i] != '=') {
		name.append(line[i]);
		i++;
	}

	if (line[i] != '=') {
#ifdef EQUAL_REQUIRED
		if (name.size() == 0)
			// Empty line:
			goto exit_ok;
		/* Currently, '=' is required, because we
		 * we have no attributes without values.
		 * Although the value may be the empty string.
		 */
		return false;
#else
		/* Basically I plan to ignore bad lines. */
		goto exit_ok;
#endif
	}
	i++;

	// Names are never this long!
	if (name.size() == 0 || name.size() > 400)
		return false;

	while (i < line.size()) {
		value.append(line[i]);
		i++;
	}

	exit_ok:
	ret_name = fromBytes(name).trimmed();
	ret_value = fromBytes(value).trimmed();
	return true;
}

void DatUtil::appendToList(QString& list, const QString& item, const QString& delim)
{
	if (!item.isEmpty()) {
		if (!list.isEmpty())
			list += delim;
		list += item;
	}
}

QString DatUtil::repeat(const QString& v, int count)
{
	QString str;

	for (int i=0; i<count; i++)
		str.append(v);
	return str;
}

QByteArray DatUtil::toStandardLineEnding(QByteArray& from)
{
	QByteArray to;

	for (int i=0; i<from.size(); i++) {
		if (from[i] == '\r') {
			if (i+1<from.size() && from[i+1] == '\n')
				i++;
			to.append('\n');
		}
		else
			to.append(from[i]);
	}
	return to;
}

QByteArray DatUtil::toWindowsLineEndings(QByteArray& data)
{
	QByteArray	converted;

	int i = 0;
	while (i < data.size()) {
		if (data[i] == '\r') {
			i++;
			if (i < data.size() && data[i] == '\n')
				i++;
			converted.append('\r');
			converted.append('\n');
		}
		else if (data[i] == '\n') {
			i++;
			converted.append('\r');
			converted.append('\n');
		}
		else {
			converted.append(data[i]);
			i++;
		}
	}
	return converted;
}

QString DatUtil::addDirChar(QString path)
{
	if (!path.endsWith("/"))
		return path + "/";
	return path;
}

QString DatUtil::lastNameOfPath(const QString& path)
{
	return lastRight(path, "/");
}

QString DatUtil::fileExtension(const QString& path)
{
	QString file = lastNameOfPath(path);

	return lastRight(path, ".");
}

QString DatUtil::getApplicationData()
{
	QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
	return addDirChar(dirs[0]);
}

QString DatUtil::getTempFolder()
{
	QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::TempLocation);
	return addDirChar(dirs[0]);
}

// Contains a list of .xml file which contain the data for each character.
QString DatUtil::getCharacterFolder()
{
	return getApplicationData() + "Characters/";
}

// Contains a list of folders that are custom data sets.
// The folders name are the names of the data sets.
// "DataSet_..." at the start of the name is ignored.
QString DatUtil::getDataSetFolder()
{
	return getApplicationData() + "DataSets/";
}

QString DatUtil::getCurrentFolder()
{
	return getApplicationData() + "CurrentData/";
}

// The System Data folder contains one directory called "Data" which contains
// the standard data.
QString DatUtil::getSystemDataFolder()
{
	return ":/resources/";
}

// Dice sequence:
// p=Proficiency (yellow)
// a=Ability (green)
// b=Boost (blue)
// c=Challenge (red)
// d=Difficulty (purple)
// s=Setback (black)
// e=empty
QString DatUtil::getBasicDicePool(int ranks, int char_value)
{
	QString pool;

	if (ranks > char_value) {
		int tmp = char_value;
		char_value = ranks;
		ranks = tmp;
	}
	for (int i=0; i<ranks; i++)
		pool += "P";
	for (int i=0; i<char_value-ranks; i++)
		pool += "A";
	return pool;
}

QString DatUtil::addDiceToText(const QString& str)
{
	QString	in = str;
	QString	out;
	QString	token;

	for (;;) {
		token = getSymbolToken(in);
		if (token.isEmpty())
			break;
		if (token.startsWith("[")) {
			QString	sym_codes;

			while (token.startsWith("[") && ImageProvider::isSymbolToken(token)) {
				sym_codes.append(ImageProvider::getSymbolCode(token));
				token = getSymbolToken(in);
				// Discard space:
				if (isSpace(token))
					token = getSymbolToken(in);
			}

			if (!sym_codes.isEmpty()) {
				while (out.endsWith(" "))
					out.chop(1);
				out += ImageProvider::getSymbolLink(sym_codes);
			}
			while (token.startsWith(" "))
				token = token.mid(1);
			out += token;
		}
		else
			out += token;
	}

	return out;
}

QString DatUtil::getSymbolToken(QString& str)
{
	QString token;

	if (str.startsWith("[")) {
		if (str.contains("]")) {
			token = left(str, "]") + "]";
			str = right(str, "]");
		}
		else {
			token = str;
			str = QString();
		}
	}
	else if (str.contains("[")) {
		token = left(str, "[");
		str = "[" + right(str, "[");
	}
	else {
		token = str;
		str = QString();
	}
	return token;
}

Injury injuries[] = {
	{ 1, 5, 10, "Minor Nick", "Suffer -1 to Strain", "DEEE" },
	{ 6, 10, 10, "Slowed Down", "May only act in last allied initiative slot in next round", "DEEE" },
	{ 11, 15, 10, "Sudden Jolt", "Drop whatever is in hand", "DEEE" },
	{ 16, 20, 10, "Distracted", "Cannot perform free maneuver during next turn", "DEEE" },
	//{ 21, 25, 10, "Off-Balance", "Add [SETBACK] to next skill check", "DEEE" },
	{ 21, 25, 10, "Off-Balance", "Add [AD] [SU] [FA] to next [TR] skill check [TH] [DE]", "DEEE" },
	{ 26, 30, 10, "Discouraging Wound", "Spend one Destiny Point", "DEEE" },
	{ 31, 35, 10, "Stunned", "Staggered until the end of the encounter", "DEEE" },
	{ 36, 40, 10, "Stinger", "Increase difficulty of next check by 1", "DEEE" },
	{ 41, 45, 10, "Bowled Over", "Knocked prone and suffer 1 Strain", "DDEE" },
	{ 46, 50, 10, "Head Ringer", "Int and Cun checks +1 difficulty until end of encounter", "DDEE" },
	{ 51, 55, 10, "Fearsome Wound", "Pr and Will checks +1 difficulty until end of encounter", "DDEE" },
	{ 56, 60, 10, "Agonizing Wound", "Br and Ag checks +1 difficulty until end of encounter", "DDEE" },
	{ 61, 65, 10, "Slightly Dazed", "Disoriented until end of the encounter", "DDEE" },
	{ 66, 70, 10, "Scattered Senses", "Remove all [BOOST] from all checks until end of encounter", "DDEE" },
	{ 71, 75, 10, "Hamstrung", "Lose free maneuver until end of encounter", "DDEE" },
	{ 76, 80, 10, "Overpowered", "Attacker may repeat last attack", "DDEE" },
	{ 81, 85, 10, "Winded", "Cannot voluntarily suffer Strain until end of encounter", "DDEE" },
	{ 86, 90, 10, "Compromised", "Increase difficulty of all checks by 1 until end of encounter", "DDEE" },
	{ 91, 95, 10, "At the Brink", "Suffer 1 strain on each action", "DDDE" },
	{ 96, 100, 0, "Crippled", "A limb is crippled, increase difficulty of checks by 1", "DDDE" },
	{ 96, 100, 3, "Crippled", "Left leg is crippled, increase difficulty of checks by 1", "DDDE" },
	{ 96, 100, 6, "Crippled", "Right leg is crippled, increase difficulty of checks by 1", "DDDE" },
	{ 96, 100, 8, "Crippled", "Left arm is crippled, increase difficulty of checks by 1", "DDDE" },
	{ 96, 100, 10, "Crippled", "Right leg is crippled, increase difficulty of checks by 1", "DDDE" },
	{ 101, 105, 0, "Maimed", "A limb is permanently lost, all actions gain [SETBACK]", "DDDE" },
	{ 101, 105, 3, "Maimed", "Left leg permanently lost, all actions gain [SETBACK]", "DDDE" },
	{ 101, 105, 6, "Maimed", "Right leg permanently lost, all actions gain [SETBACK]", "DDDE" },
	{ 101, 105, 8, "Maimed", "Left arm permanently lost, all actions gain [SETBACK]", "DDDE" },
	{ 101, 105, 10, "Maimed", "Right arm permanently lost,all actions gain [SETBACK]", "DDDE" },
	{ 126, 130, 0, "Horrific Injury", "Temporary -1 to one Characteristic", "DDDE" },
	{ 126, 130, 3, "Horrific Injury", "Temporary -1 to Brawn", "DDDE" },
	{ 126, 130, 6, "Horrific Injury", "Temporary -1 to Agility", "DDDE" },
	{ 126, 130, 7, "Horrific Injury", "Temporary -1 to Intellect", "DDDE" },
	{ 126, 130, 8, "Horrific Injury", "Temporary -1 to Cunning", "DDDE" },
	{ 126, 130, 9, "Horrific Injury", "Temporary -1 to Presence", "DDDE" },
	{ 126, 130, 10, "Horrific Injury", "Temporary -1 to Willpower", "DDDE" },
	{ 111, 115, 10, "Temporarily Lame", "Cannot perform more than 1 maneuver per turn", "DDDE" },
	{ 116, 120, 10, "Blinded", "Upgrade all checks x2, upgrade Perception and Vigilance checks x3", "DDDE" },
	{ 121, 125, 10, "Knocked Senseless", "Staggered until the end of the encounter", "DDDE" },
	{ 126, 130, 0, "Gruesome Injury", "Permanent -1 to one Characteristic", "DDDD" },
	{ 126, 130, 3, "Gruesome Injury", "Permanent -1 to Brawn", "DDDD" },
	{ 126, 130, 6, "Gruesome Injury", "Permanent -1 to Agility", "DDDD" },
	{ 126, 130, 7, "Gruesome Injury", "Permanent -1 to Intellect", "DDDD" },
	{ 126, 130, 8, "Gruesome Injury", "Permanent -1 to Cunning", "DDDD" },
	{ 126, 130, 9, "Gruesome Injury", "Permanent -1 to Presence", "DDDD" },
	{ 126, 130, 10, "Gruesome Injury", "Permanent -1 to Willpower", "DDDD" },
	{ 131, 140, 10, "Bleeding Out", "-1 Wound and -1 Strain per round, 1 Crit per 5 beyond W-Thr", "DDDD" },
	{ 141, 150, 10, "The End is Nigh", "Die at the end of the next round", "DDDD" },
	{ 151, 999, 10, "Dead", "Complete, obliterated, death", "DDDD" },
	{ 0, 0, 0, NULL, NULL, NULL }
};

Injury* DatUtil::getInjury(int perc, int d10)
{
	if (perc < 0)
		perc = 1;
	if (perc > 999)
		perc = 999;
	if (d10 < 0)
		d10 = 0;
	if (d10 > 10)
		d10 = 10;
	for (int i=0; injuries[i].title; i++) {
		if (perc >= injuries[i].fromPerc && perc <= injuries[i].toPerc && d10 <= injuries[i].d10)
			return &injuries[i];
	}
	return NULL;
}

#define KNOWN_DICE(ch) (ch == 'F' || ch == 'P' || ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D' || \
	ch == 'S' || ch == 'N' || ch == 'U' || ch == 'u' ||  ch == 'd' || ch == 'a' || ch == 't')

QString DatUtil::normalizeDice(const QString& dice)
{
	int yellow = 0;
	int green = 0;
	int blue = 0;
	int upgrade_abil = 0;
	int advantage = 0;
	int success = 0;
	int red = 0;
	int purple = 0;
	int black = 0;
	int upgrade_diff = 0;
	int downgrade_diff = 0;
	//int threat = 0;
	int white = 0;
	QString additional;
	QString new_dice;
	bool neg = false;
	bool neg_set;

	for (int i=0; i<dice.length(); i++) {
		neg_set = false;
		switch (dice[i].unicode()) {
			case 'P': neg ? upgrade_abil-- : yellow++; break;
			case 'A': neg ? green-- : green++; break;
			case 'B': neg ? blue-- : blue++; break;
			case 'C': neg ? upgrade_diff-- : red++; break;
			case 'D': neg ? purple-- : purple++; break;
			case 'S': neg ? black-- : black++; break;
			case 'E': break;
			case 'N': neg ? black++ : black--; break;
			case 'U': neg ? upgrade_abil-- : upgrade_abil++; break;
			case 'u': neg ? upgrade_diff-- : upgrade_diff++; break;
			case 'd': neg ? downgrade_diff-- : downgrade_diff++; break;
			case 'a': neg ? advantage-- : advantage++; break;
			case 't': neg ? advantage++ : advantage--; break; // threat
			case 's': neg ? success-- : success++; break;
			case 'f': neg ? success++ : success--; break;
			case 'F': neg ? white-- : white++; break;
			case 'g': neg ? white++ : white--; break;
			case ' ': break;
			case '-':
				if (i+1<dice.length()) {
					ushort ch = dice[i+1].unicode();
					if (KNOWN_DICE(ch)) {
						neg_set = true;
						break;
					}
				}
			case '+':
				if (i+1<dice.length()) {
					ushort ch = dice[i+1].unicode();
					if (KNOWN_DICE(ch))
						break;
				}
			case 'Z':
			case 'x':
			case 'y':
			case 'z':
				additional += dice[i];
				break;
			default:
				break;
		}
		neg = neg_set;
	}

	if (upgrade_abil >= 0) {
		for (int i=0; i<upgrade_abil; i++) {
			if (green) {
				green--;
				yellow++;
			}
			else
				green++;
		}
	}
	else {
		for (int i=0; i<-upgrade_abil; i++) {
			if (green)
				green--;
			else if (yellow) {
				yellow--;
				green++;
			}
			else
				break;
		}
	}

	if (upgrade_diff >= 0) {
		for (int i=0; i<upgrade_diff; i++) {
			if (purple) {
				purple--;
				red++;
			}
			else
				purple++;
		}
	}
	else {
		for (int i=0; i<-upgrade_diff; i++) {
			if (purple)
				purple--;
			else if (red) {
				red--;
				purple++;
			}
			else
				break;
		}
	}

	if (downgrade_diff >= 0) {
		int i;

		for (i=0; i<downgrade_diff; i++) {
			if (red) {
				red--;
				purple++;
			}
			else
				break;
		}
		downgrade_diff -= i;
	}
	else {
		for (int i=0; i<-downgrade_diff; i++) {
			if (purple) {
				red++;
				purple--;
			}
			else
				break;
		}
	}

	new_dice += QString("P").repeated(yellow);
	new_dice += QString("A").repeated(green);
	new_dice += QString("B").repeated(blue);
	if (white < 0)
		new_dice += QString("g").repeated(-white);
	else
		new_dice += QString("F").repeated(white);
	if (black < 0)
		new_dice += QString("N").repeated(-black);
	if (success > 0)
		new_dice += QString("s").repeated(success);
	if (advantage > 0)
		new_dice += QString("a").repeated(advantage);
	new_dice += QString("C").repeated(red);
	new_dice += QString("D").repeated(purple);
	new_dice += QString("S").repeated(black);
	if (success < 0)
		new_dice += QString("f").repeated(-success);
	if (advantage < 0)
		new_dice += QString("t").repeated(-advantage);
	if (downgrade_diff > 0)
		new_dice += QString("d").repeated(downgrade_diff);
	return new_dice + additional;
}

QString DatUtil::base64(const QString& text)
{
	return text.toUtf8().toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}

void DatUtil::yield()
{
	QEventLoop events;

#if defined(Q_OS_IOS)
	events.processEvents(QEventLoop::AllEvents);
#else
	events.processEvents(QEventLoop::ExcludeUserInputEvents);
#endif
}

QString DatUtil::pageSummary(QString page_desc)
{
	QString short_page_desc;
	QString page;
	QString book;
	QString number;

	while (page_desc.contains(" page ") and page_desc.contains(" of the ")) {
		page = left(page_desc, ",");
		page_desc = right(page_desc, ",");
		book = right(page, " of the ").trimmed();
		book = left(book, " for details").trimmed();
		if (book.endsWith(" Core Rulebook"))
			book = left(book, " Core Rulebook").trimmed();
		else if (book.endsWith(" Beta Rulebook"))
			book = left(book, " Rulebook").trimmed();
		else if (book.endsWith(" Sourcebook"))
			book = left(book, " Sourcebook").trimmed();
		else if (book.endsWith(" Rulebook")) {
			book = left(book, " Rulebook").trimmed();
			if (book.endsWith(" Core"))
				book = left(book, " Core").trimmed();
		}
		number = right(left(page, " of the "), " page ").trimmed();
		if (!short_page_desc.isEmpty())
			short_page_desc += ", ";
		if (book == "Age of Rebellion")
			book = "AoR";
		else if (book == "Edge of the Empire")
			book = "EotE";
		else if (book == "Force and Destiny")
			book = "F&D";
		else if (book == "Force and Destiny Beta")
			book = "F&D Beta";
		short_page_desc += book + " p"+number;
	}

	return short_page_desc;
}

void DatUtil::addDescription(QString& desc, QString description, QString books)
{
	if (!description.isEmpty()) {
		if (desc.isEmpty())
			desc = description;
		else
			desc = description + " " + desc;
	}

	if (!books.isEmpty()) {
		if (desc.isEmpty())
			desc = books;
		else
			desc = desc + " (" + books + ")";
	}
}
