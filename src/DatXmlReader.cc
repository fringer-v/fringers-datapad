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
 */

#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <QtXml>

#include "DatXmlReader.h"
#include "DatUtil.h"

DatXmlReader::DatXmlReader() :
	iCurrentChar(0),
	iFile(0),
	iBuffer(0),
	iBufPos(0),
	iBufLen(0),
	iPath(),
	iName(),
	iValue(),
	flags(0)
{
}

DatXmlReader::~DatXmlReader()
{
}

bool DatXmlReader::readFromFile(const char *file_iName, bool* file_not_found)
{
	iFile = fopen(file_iName, "rb");
	if (!iFile) {
		result = errno;
		if (file_not_found) {
			*file_not_found = result == ENOENT;
		}
		return false;
	}

	iPath.setLength(0);
	iName.setLength(0);
	iValue.setLength(0);
	iCurrentChar = 0;

	InitXMLState(&xml);
	start();
	bool ok = parseXML();
	end();
	ProcessXMLDone(&xml);

	fclose(iFile);
	iFile = NULL;
	yeildCount = 0;

	return ok;
}

bool DatXmlReader::readFromBuffer(const char* buffer, size_t length)
{
	iBuffer = buffer;
	iBufLen = length;
	iBufPos = 0;

	iPath.setLength(0);
	iName.setLength(0);
	iValue.setLength(0);
	iCurrentChar = 0;

	// Skip UTF-8 BOM:
	if (buffer[0] == (char) 239 &&
		buffer[1] == (char) 187 &&
		buffer[2] == (char) 191)
		iBufPos = 3;

	InitXMLState(&xml);
	start();
	bool ok = parseXML();
	end();
	ProcessXMLDone(&xml);

	iBuffer = NULL;
	iBufLen = 0;
	iBufPos = 0;
	yeildCount = 0;

	return ok;
}

void DatXmlReader::start()
{
}

void DatXmlReader::end()
{
}

bool DatXmlReader::openNode(const DatStringBuffer& path, const char* value)
{
	//qDebug() << "=====OPEN " << path << value;
	bool ok = true;

	if (path.endsWith("/")) {
		iValue.trim();
		value = iValue.getCString();
		if (*value)
			ok = xmlElement(path, value);
	}
	else {
		size_t len = iPath.length();
		iPath.append("/#open");
		ok = xmlElement(iPath, "");
		iPath.setLength(len);
	}
	return ok;
}

bool DatXmlReader::closeNode(const DatStringBuffer& path)
{
	//qDebug() << "----CLOSE " << path;
	if (path.endsWith("/"))
		return true;
	size_t len = iPath.length();
	iPath.append("/#end");
	bool ok = xmlElement(iPath, "");
	iPath.setLength(len);
	return ok;
}

bool DatXmlReader::addAttribute(const DatStringBuffer&, const char*, const char*)
{
	return true;
}

int DatXmlReader::toInt(const char* value) const
{
	return atoi(value);
}

bool DatXmlReader::isTrue(const char* value) const
{
	return strcmp(value, "true") == 0;
}

/*
bool DatXmlReader::readFromBuffer(const char *buffer)
{
	readFromBuffer(buffer, strlen(buffer));
	return parseXML();
}

bool DatXmlReader::readFromBuffer(const char* buffer, size_t length)
{
	return parseXML();
}
*/

bool DatXmlReader::addChars(DatStringBuffer* s, size_t size, UniChar *buffer, bool to_lower)
{
	UniChar ch;

	for (size_t i=0; i<size; i++) {
		ch = buffer[i];
		if (ch > 127)
			ch = '?';
		//if (to_lower)
		//	ch = tolower(ch);
		// Ignore preceeding space:
		if (s->length() > 0 || !isspace(ch)) {
			if (!s->append((char) ch))
				return false;
		}
	}
	return true;
}

bool DatXmlReader::addCString(DatStringBuffer* s, const char* string)
{
	return s->append(string);
}

void DatXmlReader::setEmpty(DatStringBuffer* s)
{
	s->setLength(0);
}

/* -------------------------------------------------------------------------------------------- */
/*												READ XML										*/
/* -------------------------------------------------------------------------------------------- */

#define IS_XML_CDATA				0
#define IS_XML_CDATA_TAG			1
#define IS_XML_TAG					2
#define IS_XML_CLOSE_TAG			3
#define IS_XML_COMMENT				4
#define IS_XML_DTD					5
#define IS_XML_PI					6
#define IS_XML_PI_XML				7
#define IS_XML_IN_EX				8
#define IS_XML_OPEN_BRACKET			9
#define IS_XML_CLOSE_BRACKET		10

int DatXmlReader::getNodeType(const char* pname)
{
	if (pname) {
		switch (*pname) {
			case 0:
				return(IS_XML_CDATA);
			case '[':
				if (strlen(pname) == 1)
					return(IS_XML_OPEN_BRACKET);
				break;
			case ']':
				if (strlen(pname) == 1)
					return(IS_XML_CLOSE_BRACKET);
				break;
			case '/':
				return(IS_XML_CLOSE_TAG);
			case '!':
				if (strlen(pname) > 1) {
					if (strcasecmp(pname, "!--") == 0)
						return(IS_XML_COMMENT);
					if (pname[1] == '[') {
						if (strcasecmp(pname, "![CDATA[") == 0)
							return(IS_XML_CDATA_TAG);
						return(IS_XML_IN_EX);
					}
				}
				return(IS_XML_DTD);
			case '?':
				if (strcasecmp(pname, "?xml") == 0)
					return(IS_XML_PI_XML);
				return(IS_XML_PI);
		}
		return(IS_XML_TAG);
	}
	return(IS_XML_CDATA);
}

bool DatXmlReader::doCloseNode(const char* pname, bool single)
{
	bool	done = single;
	char	*ptr;
	size_t	len;

	//qDebug() << "* CLOSE: " << pname<< iPath.getCString() << single;
	len = iPath.length();
	if (len > 0) {
		ptr = iPath.getBuffer() + len;
		do {
			iPath.getCString();
			len--;
			ptr--;
			while (len && *ptr != '/') {
				len--;
				ptr--;
			}
			if (strcmp(ptr, pname) == 0)
				done = true;
			if (!closeNode(iPath))
				return false;
			iPath.setLength(len);
		}
		while (!done && len);
	}
	return true;
}

bool DatXmlReader::doOpenNode(const char* pname)
{	
	//qDebug() << "*  OPEN: " << pname << iPath.getCString();
	//if (iPath.length() == 0)
	yeildCount++;
	if ((yeildCount % 1000) == 0)
		DatUtil::yield();

	if (!addCString(&iPath, "/"))
		return false;
	if (!addCString(&iPath, pname))
		return false;
	return openNode(iPath, iValue.getCString());
}

/*
FSTATIC void dump_op(int op, UniChar ch)
{
	switch (ch) {
		case '\n':
			printf("'\\n' ");
			break;
		case '\t':
			printf("'\\t' ");
			break;
		case '\r':
			printf("'\\r' ");
			break;
		default:
			if (isprint(ch))
				printf("'%c' ", (int) ch);
			else
				printf("0x%02X ", (int) ch);
	}
	switch (op & XML_OP_1_MASK) {
		case XML_OP_1_NOOP:				printf("-          "); break;
		case XML_OP_1_END_TAG:			printf("<...> }    "); break;
		case XML_OP_1_END_CLOSE_TAG:	printf("</...> }   "); break;
		case XML_OP_1_END_EMPTY_TAG:	printf("<.../> }   "); break;
		case XML_OP_1_END_PI_TAG:		printf("<?...?> }  "); break;
		case XML_OP_1_END_ENTITY_TAG:	printf("<!...> }   "); break;
		case XML_OP_1_END_BRACKET_TAG:	printf("<![...]> } "); break;
		case XML_OP_1_END_UNKNOWN_TAG:	printf("<_...> }   "); break;
		case XML_OP_1_START_CDATA_TAG:	printf("{ <![CDATA "); break;
		case XML_OP_1_START_COMMENT:	printf("{ <!--...  "); break;
		case XML_OP_1_START_TAG:		printf("{ <...     "); break;
		case XML_OP_1_ADD_ATTR:			printf("ADD ATTR   "); break;
		case XML_OP_1_END_CDATA:		printf("CDATA }    "); break;
		case XML_OP_1_END_CDATA_TAG:	printf("...]]> }   "); break;
		case XML_OP_1_END_COMMENT:		printf("...--> }   "); break;
	}
	switch (op & XML_DATA_MASK) {
		case XML_NO_DATA:		printf("-         "); break;
		case XML_DATA_TAG:		printf("TAG DATA  "); break;
		case XML_DATA_ATTR:		printf("ATTR DATA "); break;
		case XML_DATA_CDATA:	printf("CDATA     "); break;
		case XML_DATA_CDATA_TAG:printf("CDATA TAG "); break;
		case XML_COMMENT:		printf("comment   "); break;
		case XML_DATA_VALUE:	printf("VALU DATA "); break;
	}
	switch (op & XML_OP_2_MASK) {
		case XML_OP_2_NOOP:				printf("-          "); break;
		case XML_OP_2_END_TAG:			printf("<...> }    "); break;
		case XML_OP_2_END_CLOSE_TAG:	printf("</...> }   "); break;
		case XML_OP_2_END_EMPTY_TAG:	printf("<.../> }   "); break;
		case XML_OP_2_END_PI_TAG:		printf("<?...?> }  "); break;
		case XML_OP_2_END_ENTITY_TAG:	printf("<!...> }   "); break;
		case XML_OP_2_END_BRACKET_TAG:	printf("<![...]> } "); break;
		case XML_OP_2_END_UNKNOWN_TAG:	printf("<_...> }   "); break;
		case XML_OP_2_START_CDATA_TAG:	printf("{ <![CDATA "); break;
		case XML_OP_2_START_COMMENT:	printf("{ <!--...  "); break;
	}
	printf("\n");
}
*/

bool DatXmlReader::parseXML()
{
	bool	ok;
	bool	error_occurred;
	int		op;
	int		tagtype;

	if (!addChars(&iPath, 0, NULL, false))
		return false;
	if (!addChars(&iName, 0, NULL, false))
		return false;
	if (!addChars(&iValue, 0, NULL, false))
		return false;

	while (nextChar(&error_occurred)) {
		op = ProcessXMLChar(&xml, currentChar());
		switch (op & XML_OP_1_MASK) {
			case XML_OP_1_NOOP:
				break;
			case XML_OP_1_END_TAG:
				break;
			case XML_OP_1_END_CLOSE_TAG:
				break;
			case XML_OP_1_END_EMPTY_TAG:
				if (!doCloseNode("/>", true))
					return false;
				break;
			case XML_OP_1_END_PI_TAG:
				if (!doCloseNode("?>", true))
					return false;
				break;
			case XML_OP_1_END_ENTITY_TAG:
				if (!doCloseNode(">", true))
					return false;
				break;
			case XML_OP_1_END_BRACKET_TAG:
				if (!doCloseNode("]>", true))
					return false;
				break;
			case XML_OP_1_END_UNKNOWN_TAG:
				if (!doCloseNode(">", true))
					return false;
				break;
			case XML_OP_1_START_CDATA_TAG:
				break;
			case XML_OP_1_START_COMMENT:
				break;
			case XML_OP_1_START_TAG:
				if (getNodeType(iName.getCString()) == IS_XML_CLOSE_TAG)
					ok = doCloseNode(iName.getCString(), false);
				else
					ok = doOpenNode(iName.getCString());
				if (!ok)
					return false;
				setEmpty(&iName);
				setEmpty(&iValue);
				break;
			case XML_OP_1_ADD_ATTR:
				tagtype = getNodeType(iName.getCString());
				if (tagtype != IS_XML_OPEN_BRACKET && tagtype != IS_XML_CLOSE_BRACKET) {
					if (!addAttribute(iPath, iName.getCString(), iValue.getCString()))
						return false;
				}
				setEmpty(&iName);
				setEmpty(&iValue);
				break;
			case XML_OP_1_END_CDATA:
				if (iValue.length() || (flags & XML_KEEP_EMPTY_CDATA)) {
					if (!doOpenNode(""))
						return false;
					setEmpty(&iName);
					setEmpty(&iValue);
					if (!doCloseNode("", true))
						return false;
				}
				break;
			case XML_OP_1_END_CDATA_TAG:
				if (!doOpenNode("![CDATA["))
					return false;
				setEmpty(&iName);
				setEmpty(&iValue);
				if (!doCloseNode("]]>", true))
					return false;
				break;
			case XML_OP_1_END_COMMENT:
				if (!doOpenNode("!--"))
					return false;
				setEmpty(&iName);
				setEmpty(&iValue);
				if (!doCloseNode("-->", true))
					return false;
				break;
		}
		switch (op & XML_DATA_MASK) {
			case XML_DATA_TAG:
			case XML_DATA_ATTR:
				if (!addChars(&iName, xml.p.count, xml.p.buffer, true))
					return false;
				break;
			case XML_DATA_CDATA:
			case XML_DATA_CDATA_TAG:
			case XML_COMMENT:
			case XML_DATA_VALUE:
				if (!addChars(&iValue, xml.p.count, xml.p.buffer, false))
					return false;
				break;
		}
		switch (op & XML_OP_2_MASK) {
			case XML_OP_2_NOOP:
				break;
			case XML_OP_2_END_TAG:
				break;
			case XML_OP_2_END_CLOSE_TAG:
				break;
			case XML_OP_2_END_EMPTY_TAG:
				if (!doCloseNode("/>", true))
					return false;
				break;
			case XML_OP_2_END_PI_TAG:
				if (!doCloseNode("?>", true))
					return false;
				break;
			case XML_OP_2_END_ENTITY_TAG:
				if (!doCloseNode(">", true))
					return false;
				break;
			case XML_OP_2_END_BRACKET_TAG:
				if (!doCloseNode("]>", true))
					return false;
				break;
			case XML_OP_2_END_UNKNOWN_TAG:
				if (!doCloseNode(">", true))
					return false;
				break;
			case XML_OP_2_START_CDATA_TAG:
				break;
			case XML_OP_2_START_COMMENT:
				break;
		}
	}

	return !error_occurred;
}

bool DatXmlReader::nextChar(bool *error_occurred)
{
	char ch;

	if (iFile) {
		int fch = fgetc(iFile);
		if (fch == EOF) {
			if (errno) {
				result = errno;
				*error_occurred = true;
			}
			else {
				result = 0;
				*error_occurred = false;
			}
			iCurrentChar = 0;
			return false;
		}
		ch = (char) fch;
	}
	else if (iBuffer) {
		if (iBufPos >= iBufLen) {
			result = 0;
			*error_occurred = false;
			return false;
		}
		ch = iBuffer[iBufPos];
		iBufPos++;
	}
	else {
		*error_occurred = false;
		result = 0;
		return false;
	}

	iCurrentChar = (unsigned char) ch;
	return true;
}

UniChar DatXmlReader::currentChar()
{
	return iCurrentChar;
}

/* -------------------------------------------------------------------------------------------- */
/*												STRING BUFFER									*/
/* -------------------------------------------------------------------------------------------- */

DatStringBuffer::DatStringBuffer() :
	iStrBuf(0),
	iSize(0),
	myStrLen(0)
{
}

DatStringBuffer::~DatStringBuffer()
{
}

const char *DatStringBuffer::getCString() const
{
	if (!iStrBuf)
		return "";
	if (iStrBuf)
		iStrBuf[myStrLen] = 0;
	return iStrBuf;
}

bool DatStringBuffer::append(const char* string)
{
	size_t len = strlen(string);

	if (myStrLen+len+1 > iSize) {
		void* ptr = realloc((void*) iStrBuf, myStrLen + len + 100);
		if (!ptr)
			return false;
		iStrBuf = (char*) ptr;
		iSize = myStrLen + len + 100;
	}
	memcpy(iStrBuf + myStrLen, string, len);
	myStrLen += len;
	return true;
}

bool DatStringBuffer::append(char ch)
{
	if (myStrLen+1+1 > iSize) {
		void* ptr = realloc((void*) iStrBuf, myStrLen + 101);
		if (!ptr)
			return false;
		iStrBuf = (char*) ptr;
		iSize = myStrLen + 101;
	}
	iStrBuf[myStrLen] = ch;
	myStrLen++;
	return true;
}

bool DatStringBuffer::setLength(size_t len)
{
	if (len+1 > iSize) {
		void* ptr = realloc((void*) iStrBuf, len+1);
		if (!ptr)
			return false;
		iSize = len+1;
	}
	myStrLen = len;
	return true;
}

size_t DatStringBuffer::length() const
{
	return myStrLen;
}

char* DatStringBuffer::getBuffer() const
{
	if (iStrBuf)
		iStrBuf[myStrLen] = 0;
	return iStrBuf;
}

void DatStringBuffer::trim()
{
	if (!iStrBuf || !myStrLen)
		return;
	while (myStrLen > 0) {
		if (!isspace(iStrBuf[myStrLen-1]))
			break;
		myStrLen--;
	}
	iStrBuf[myStrLen] = 0;
}

bool DatStringBuffer::startsWith(const char* str) const
{
	size_t slen = strlen(str);

	if (!slen || myStrLen < slen)
		return false;

	if (!iStrBuf || !myStrLen)
		return false;

	iStrBuf[myStrLen] = 0;
	int r = strncmp(iStrBuf, str, slen) == 0;
	return r;
}

bool DatStringBuffer::endsWith(const char* str) const
{
	size_t slen = strlen(str);

	if (slen == 0 || slen > myStrLen)
		return false;
	iStrBuf[myStrLen] = 0;
	return strcmp(iStrBuf + myStrLen - slen, str) == 0;
}

bool DatStringBuffer::contains(const char* str) const
{
	size_t slen = strlen(str);

	if (!slen || myStrLen < slen)
		return false;

	if (!iStrBuf || !myStrLen)
		return false;

	iStrBuf[myStrLen] = 0;
	return strstr(iStrBuf, str) != 0;
}

int DatStringBuffer::toInt() const
{
	if (!iStrBuf || !myStrLen)
		return 0;
	iStrBuf[myStrLen] = 0;
	return atoi(iStrBuf);
}

