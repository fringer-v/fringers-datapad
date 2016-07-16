/* Copyright (c) 2014 PrimeBase Systems GmbH
 *
 * yvva
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA
 *
 * 2014-01-29 Paul McCullagh
 *
 * H&G2JCtL
 *
 * A simple XML parser.
 */

#ifndef __yvvad_xmlreader_h__
#define __yvvad_xmlreader_h__

#include <unistd.h>
#include <stdio.h>

#include "pxml.h"

#define XML_KEEP_EMPTY_CDATA	1

class DatStringBuffer {
public:
	DatStringBuffer();
	virtual ~DatStringBuffer();

	const char *getCString() const;
	bool append(const char* string);
	bool append(char ch);
	bool setLength(size_t len);
	size_t length() const;
	char* getBuffer() const;
	void trim();
	bool startsWith(const char* str) const;
	bool endsWith(const char* string) const;
	int toInt() const;

private:
	char *iStrBuf;
	size_t iSize;
	size_t myStrLen;
};

class DatXMLReader {
public:
	DatXMLReader();
	virtual ~DatXMLReader();

	bool readFromFile(const char* file_name, bool* file_not_found = NULL);
	bool readFromBuffer(const char* buffer, size_t length);
	//bool readFromBuffer(const char* buffer);

	virtual void start();
	virtual void end();
	virtual bool xmlElement(const DatStringBuffer& path, const char* value) = 0;

	virtual bool openNode(const DatStringBuffer& path, const char* value);
	virtual bool closeNode(const DatStringBuffer& path);
	virtual bool addAttribute(const DatStringBuffer& path, const char* name, const char* value);

	int toInt(const char* value) const;
	bool isTrue(const char* value) const;

private:
	bool addChars(DatStringBuffer* s, size_t size, UniChar *buffer, bool to_lower);
	bool addCString(DatStringBuffer* s, const char* string);
	void setEmpty(DatStringBuffer* s);
	int getNodeType(const char* name);
	bool doCloseNode(const char* name, bool single);
	bool doOpenNode(const char* name);
	bool parseXML();

	bool nextChar(bool *error_occurred);
	UniChar currentChar();

	UniChar				iCurrentChar;

	FILE*				iFile;
	const char*			iBuffer;
	size_t				iBufPos;
	size_t				iBufLen;

	int					result;
	DatStringBuffer		iPath;
	DatStringBuffer		iName;
	DatStringBuffer		iValue;
	Word4				flags;
	XMLProcessStateRec	xml;

	int					yeildCount;
};

/* -------------------------------------------------------------------------------------------- */
/* 										PUBLIC FUNCTIONS										*/
/* -------------------------------------------------------------------------------------------- */

#endif
