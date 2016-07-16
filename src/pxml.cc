/* pxml.c 23.3.01 Paul McCullagh */
/* Parse XML */

#include <string.h>
#include <stdio.h>

#include "pxml.h"

#define ISSPACE(ch)			(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
#define ISSINGLE(ch)		(ch == '*' || ch == '+' || ch == '(' || ch == ')' || ch == ',' || ch == '|' || ch == '[' || ch == ']' || ch == '?' || ch == '/')

#define SET_CHAR(x, ch)		{ x->buffer[0] = ch; x->count = 1; }
#define ADD_CHAR(x, ch)		{ if (x->count < PARSE_BUFFER_SIZE) { x->buffer[x->count] = ch; x->count++; } else x->buffer[PARSE_BUFFER_SIZE-1] = ch; }

static bool
match_string(XMLParseStatePtr xml, const char *ch)
{
	int i;
	
	for (i=0; i<xml->count; i++) {
		if (xml->buffer[i] != *ch)
			return(false);
		ch++;
	}
	if (*ch)
		return(false);
	return(i == xml->count);
}

static void
increment_nesting(XMLParseStatePtr xml, UniChar ch)
{
	if (xml->nesting < PARSE_STACK_SIZE) {
		switch (ch) {
			case '/':
				xml->end_type[xml->nesting] = XML_OP_1_END_CLOSE_TAG;
				break;
			case '?':
				xml->end_type[xml->nesting] = XML_OP_1_END_PI_TAG;
				break;
			case '!':
				xml->end_type[xml->nesting] = XML_OP_1_END_ENTITY_TAG;
				break;
			case '[':
				xml->end_type[xml->nesting] = XML_OP_1_END_BRACKET_TAG;
				break;
			default:
				if (ISSPACE(ch))
					xml->end_type[xml->nesting] = XML_OP_1_END_UNKNOWN_TAG;
				else
					xml->end_type[xml->nesting] = XML_OP_1_END_TAG;
				break;
		}
	}
	xml->nesting++;
}

static int
xml_parser(XMLParseStatePtr xml, UniChar ch)
/* This function does the actual work of parsing. It is expects 
 * "complete" characters as input. This could be 4 byte characters
 * as long as it is able to recognize the characters that are
 * relevant to parsing.
 * The function outputs processing instructions, and indicates
 * how the output data is to be understood.
 */
{
	switch (xml->state) {
		case XML_BEFORE_CDATA:
			xml->nesting = 0;
			/* This is the initial state! */
			if (ch == '<') {
				xml->state = XML_LT;
				xml->type = XML_noop;
			}
			else {
				xml->state = XML_IN_CDATA;
				xml->type = XML_CDATA_CH;
			}
			SET_CHAR(xml, ch);
			break;
		case XML_IN_CDATA:
			if (ch == '<') {
				xml->state = XML_LT;
				xml->type = XML_noop;
			}
			else
				xml->type = XML_CDATA_CH;
			SET_CHAR(xml, ch);
			break;
		case XML_LT:
			if (ISSPACE(ch)) {
				if (xml->nesting) {
					xml->state = XML_BEFORE_ATTR;
					if (xml->step == XML_STEP_TAG)
						xml->type = XML_start_tag_TAG_CH;
					else if (xml->step == XML_STEP_NESTED)
						xml->type = XML_TAG_CH;
					else if (xml->step == XML_STEP_NONE)
						xml->type = XML_end_cdata_TAG_CH;
					else
						xml->type = XML_add_attr_TAG_CH;
					xml->step = XML_STEP_TAG;
					increment_nesting(xml, ch);
					xml->count = 0;
				}
				else {
					xml->state = XML_IN_CDATA;
					xml->type = XML_CDATA_CH;
					ADD_CHAR(xml, ch);
				}
			}
			else if (ch == '!') {
				xml->state = XML_LT_BANG;
				xml->type = XML_noop;
				ADD_CHAR(xml, ch);
			}
			else {
				xml->state = XML_IN_TAG_NAME;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_TAG_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_TAG_CH;
				else if (xml->step == XML_STEP_NONE)
					xml->type = XML_end_cdata_TAG_CH;
				else
					xml->type = XML_add_attr_TAG_CH;
				xml->step = XML_STEP_TAG;
				increment_nesting(xml, ch);
				SET_CHAR(xml, ch);
			}
			break;
		case XML_LT_BANG:
			if (ch == '-') {
				xml->state = XML_LT_BANG_DASH;
				xml->type = XML_noop;
			}
			else if (ch == '[') {
				xml->state = XML_LT_BANG_SQR;
				xml->type = XML_noop;
			}
			else {
				xml->state = XML_IN_TAG_NAME;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_TAG_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_TAG_CH;
				else if (xml->step == XML_STEP_NONE)
					xml->type = XML_end_cdata_TAG_CH;
				else
					xml->type = XML_add_attr_TAG_CH;
				xml->step = XML_STEP_TAG;
				increment_nesting(xml, '!');
				SET_CHAR(xml, '!');
			}
			ADD_CHAR(xml, ch);
			break;
		case XML_LT_BANG_DASH:
			if (ch == '-') {
				xml->state = XML_IN_COMMENT;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_start_comment;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_start_comment;
				else if (xml->step == XML_STEP_NONE)
					xml->type = XML_end_cdata_start_comment;
				else
					xml->type = XML_add_attr_start_comment;
				increment_nesting(xml, ' ');
			}
			else {
				xml->state = XML_IN_CDATA;
				xml->type = XML_CDATA_CH;
				ADD_CHAR(xml, ch);
			}
			break;
		case XML_LT_BANG_SQR:
			if (ISSPACE(ch))
				xml->type = XML_noop;
			else if (ch == '[') {
				xml->state = XML_BEFORE_ATTR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_TAG_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_TAG_CH;
				else if (xml->step == XML_STEP_NONE)
					xml->type = XML_end_cdata_TAG_CH;
				else
					xml->type = XML_add_attr_TAG_CH;
				xml->step = XML_STEP_TAG;
				increment_nesting(xml, '[');
				SET_CHAR(xml, '!');
				ADD_CHAR(xml, '[');
			}
			else {
				xml->state = XML_LT_BANG_SQR_IN_NAME;
				xml->type = XML_noop;
				SET_CHAR(xml, '!');
				ADD_CHAR(xml, '[');
				ADD_CHAR(xml, ch);
			}
			break;
		case XML_LT_BANG_SQR_IN_NAME:
			if (ISSPACE(ch)) {
				xml->state = XML_LT_BANG_SQR_AFTER_NAME;
				xml->type = XML_noop;
			}
			else if (ch == '[') {
				if (match_string(xml, "![CDATA")) {
					xml->state = XML_IN_CDATA_TAG;
					if (xml->step == XML_STEP_TAG)
						xml->type = XML_start_tag_start_cdata_tag;
					else if (xml->step == XML_STEP_NESTED)
						xml->type = XML_start_cdata_tag;
					else if (xml->step == XML_STEP_NONE)
						xml->type = XML_end_cdata_start_cdata_tag;
					else
						xml->type = XML_add_attr_start_cdata_tag;
					xml->step = XML_STEP_TAG;
					increment_nesting(xml, '[');
				}
				else {
					xml->state = XML_BEFORE_ATTR;
					if (xml->step == XML_STEP_TAG)
						xml->type = XML_start_tag_TAG_CH;
					else if (xml->step == XML_STEP_NESTED)
						xml->type = XML_TAG_CH;
					else if (xml->step == XML_STEP_NONE)
						xml->type = XML_end_cdata_TAG_CH;
					else
						xml->type = XML_add_attr_TAG_CH;
					xml->step = XML_STEP_TAG;
					increment_nesting(xml, '[');
				}
			}
			else {
				xml->type = XML_noop;
				ADD_CHAR(xml, ch);
			}
			break;
		case XML_LT_BANG_SQR_AFTER_NAME:
			if (ch == '[') {
				if (match_string(xml, "![CDATA")) {
					xml->state = XML_IN_CDATA_TAG;
					if (xml->step == XML_STEP_TAG)
						xml->type = XML_start_tag_start_cdata_tag;
					else if (xml->step == XML_STEP_NESTED)
						xml->type = XML_start_cdata_tag;
					else if (xml->step == XML_STEP_NONE)
						xml->type = XML_end_cdata_start_cdata_tag;
					else
						xml->type = XML_add_attr_start_cdata_tag;
					increment_nesting(xml, '[');
				}
				else {
					xml->state = XML_BEFORE_ATTR;
					if (xml->step == XML_STEP_TAG)
						xml->type = XML_start_tag_TAG_CH;
					else if (xml->step == XML_STEP_NESTED)
						xml->type = XML_TAG_CH;
					else if (xml->step == XML_STEP_NONE)
						xml->type = XML_end_cdata_TAG_CH;
					else
						xml->type = XML_add_attr_TAG_CH;
					xml->step = XML_STEP_TAG;
					increment_nesting(xml, '[');
				}
			}
			else
				/* Ignore data until the '['!!! */
				xml->type = XML_noop;
			break;
		case XML_IN_TAG_NAME:
			if (ISSPACE(ch)) {
				xml->state = XML_BEFORE_ATTR;
				xml->type = XML_noop;
			}
			else if (ch == '<') {
				xml->state = XML_LT;
				xml->type = XML_noop;
			}
			else if (ch == '>') {
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_end_tag(END_TAG_TYPE(xml));
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_end_tag(END_TAG_TYPE(xml));
				else
					xml->type = XML_add_attr_end_tag(END_TAG_TYPE(xml));
				xml->nesting--;
				if (xml->nesting) {
					xml->step = XML_STEP_NESTED;
					xml->state = XML_BEFORE_ATTR;
				}
				else {
					xml->step = XML_STEP_NONE;
					xml->state = XML_IN_CDATA;
				}
			}
			else if (ch == '"' || ch == '\'') {
				xml->state = XML_QUOTE_BEFORE_VALUE;
				xml->quote = ch;
				xml->type = XML_noop;
			}
			else if (ch == '/' && (END_TAG_TYPE(xml) == XML_OP_1_END_TAG)) {
				xml->state = XML_SLASH;
				xml->type = XML_noop;
			}
			else if (ch == '?' && (END_TAG_TYPE(xml) == XML_OP_1_END_PI_TAG)) {
				xml->state = XML_QMARK;
				xml->type = XML_noop;
			}
			else if (ch == ']' && (END_TAG_TYPE(xml) == XML_OP_1_END_BRACKET_TAG)) {
				xml->state = XML_SQR;
				xml->type = XML_noop;
			}
			else if (ISSINGLE(ch)) {
				xml->state = XML_BEFORE_ATTR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_ATTR_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_ATTR_CH;
				else
					xml->type = XML_add_attr_ATTR_CH;
				xml->step = XML_STEP_ATTR;
				SET_CHAR(xml, ch);
			}
			else {
				xml->type = XML_TAG_CH;
				SET_CHAR(xml, ch);
			}
			break;
		case XML_BEFORE_ATTR:
			if (ISSPACE(ch))
				xml->type = XML_noop;
			else if (ch == '<') {
				xml->state = XML_LT;
				xml->type = XML_noop;
			}
			else if (ch == '>') {
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_end_tag(END_TAG_TYPE(xml));
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_end_tag(END_TAG_TYPE(xml));
				else
					xml->type = XML_add_attr_end_tag(END_TAG_TYPE(xml));
				xml->nesting--;
				if (xml->nesting) {
					xml->step = XML_STEP_NESTED;
					xml->state = XML_BEFORE_ATTR;
				}
				else {
					xml->step = XML_STEP_NONE;
					xml->state = XML_IN_CDATA;
				}
			}
			else if (ch == '"' || ch == '\'') {
				xml->state = XML_QUOTE_BEFORE_VALUE;
				xml->quote = ch;
				xml->type = XML_noop;
			}
			else if (ch == '/' && (END_TAG_TYPE(xml) == XML_OP_1_END_TAG)) {
				xml->state = XML_SLASH;
				xml->type = XML_noop;
			}
			else if (ch == '?' && (END_TAG_TYPE(xml) == XML_OP_1_END_PI_TAG)) {
				xml->state = XML_QMARK;
				xml->type = XML_noop;
			}
			else if (ch == ']' && (END_TAG_TYPE(xml) == XML_OP_1_END_BRACKET_TAG)) {
				xml->state = XML_SQR;
				xml->type = XML_noop;
			}
			else if (ISSINGLE(ch)) {
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_ATTR_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_ATTR_CH;
				else
					xml->type = XML_add_attr_ATTR_CH;
				xml->step = XML_STEP_ATTR;
				SET_CHAR(xml, ch);
			}
			else {
				xml->state = XML_IN_ATTR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_ATTR_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_ATTR_CH;
				else
					xml->type = XML_add_attr_ATTR_CH;
				xml->step = XML_STEP_ATTR;
				SET_CHAR(xml, ch);
			}
			break;
		case XML_IN_ATTR:
			if (ISSPACE(ch)) {
				xml->state = XML_BEFORE_EQUAL;
				xml->type = XML_noop;
			}
			else if (ch == '<') {
				xml->state = XML_LT;
				xml->type = XML_noop;
			}
			else if (ch == '>') {
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_end_tag(END_TAG_TYPE(xml));
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_end_tag(END_TAG_TYPE(xml));
				else
					xml->type = XML_add_attr_end_tag(END_TAG_TYPE(xml));
				xml->nesting--;
				if (xml->nesting) {
					xml->step = XML_STEP_NESTED;
					xml->state = XML_BEFORE_ATTR;
				}
				else {
					xml->step = XML_STEP_NONE;
					xml->state = XML_IN_CDATA;
				}
			}
			else if (ch == '"' || ch == '\'') {
				xml->state = XML_QUOTE_BEFORE_VALUE;
				xml->quote = ch;
				xml->type = XML_noop;
			}
			else if (ch == '/' && (END_TAG_TYPE(xml) == XML_OP_1_END_TAG)) {
				xml->state = XML_SLASH;
				xml->type = XML_noop;
			}
			else if (ch == '?' && (END_TAG_TYPE(xml) == XML_OP_1_END_PI_TAG)) {
				xml->state = XML_QMARK;
				xml->type = XML_noop;
			}
			else if (ch == ']' && (END_TAG_TYPE(xml) == XML_OP_1_END_BRACKET_TAG)) {
				xml->state = XML_SQR;
				xml->type = XML_noop;
			}
			else if (ISSINGLE(ch)) {
				xml->state = XML_BEFORE_ATTR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_ATTR_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_ATTR_CH;
				else
					xml->type = XML_add_attr_ATTR_CH;
				xml->step = XML_STEP_ATTR;
				SET_CHAR(xml, ch);
			}
			else if (ch == '=') {
				xml->state = XML_AFTER_EQUAL;
				xml->type = XML_noop;
			}
			else {
				xml->type = XML_ATTR_CH;
				SET_CHAR(xml, ch);
			}
			break;
		case XML_BEFORE_EQUAL:
			if (ISSPACE(ch))
				xml->type = XML_noop;
			else if (ch == '<') {
				xml->state = XML_LT;
				xml->type = XML_noop;
			}
			else if (ch == '>') {
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_end_tag(END_TAG_TYPE(xml));
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_end_tag(END_TAG_TYPE(xml));
				else
					xml->type = XML_add_attr_end_tag(END_TAG_TYPE(xml));
				xml->nesting--;
				if (xml->nesting) {
					xml->step = XML_STEP_NESTED;
					xml->state = XML_BEFORE_ATTR;
				}
				else {
					xml->step = XML_STEP_NONE;
					xml->state = XML_IN_CDATA;
				}
			}
			else if (ch == '"' || ch == '\'') {
				xml->state = XML_QUOTE_BEFORE_VALUE;
				xml->quote = ch;
				xml->type = XML_noop;
			}
			else if (ch == '/' && (END_TAG_TYPE(xml) == XML_OP_1_END_TAG)) {
				xml->state = XML_SLASH;
				xml->type = XML_noop;
			}
			else if (ch == '?' && (END_TAG_TYPE(xml) == XML_OP_1_END_PI_TAG)) {
				xml->state = XML_QMARK;
				xml->type = XML_noop;
			}
			else if (ch == ']' && (END_TAG_TYPE(xml) == XML_OP_1_END_BRACKET_TAG)) {
				xml->state = XML_SQR;
				xml->type = XML_noop;
			}
			else if (ISSINGLE(ch)) {
				xml->state = XML_BEFORE_ATTR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_ATTR_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_ATTR_CH;
				else
					xml->type = XML_add_attr_ATTR_CH;
				xml->step = XML_STEP_ATTR;
				SET_CHAR(xml, ch);
			}
			else if (ch == '=') {
				xml->state = XML_AFTER_EQUAL;
				xml->type = XML_noop;
			}
			else {
				xml->state = XML_IN_ATTR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_ATTR_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_ATTR_CH;
				else
					xml->type = XML_add_attr_ATTR_CH;
				xml->step = XML_STEP_ATTR;
				SET_CHAR(xml, ch);
			}
			break;
		case XML_AFTER_EQUAL:
			if (ISSPACE(ch)) {
				xml->state = XML_AFTER_EQUAL;
				xml->type = XML_noop;
			}
			else if (ch == '<') {
				xml->state = XML_LT;
				xml->type = XML_noop;
			}
			else if (ch == '>') {
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_end_tag(END_TAG_TYPE(xml));
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_end_tag(END_TAG_TYPE(xml));
				else
					xml->type = XML_add_attr_end_tag(END_TAG_TYPE(xml));
				xml->nesting--;
				if (xml->nesting) {
					xml->step = XML_STEP_NESTED;
					xml->state = XML_BEFORE_ATTR;
				}
				else {
					xml->step = XML_STEP_NONE;
					xml->state = XML_IN_CDATA;
				}
			}
			else if (ch == '"' || ch == '\'') {
				xml->state = XML_QUOTE_BEFORE_VALUE;
				xml->quote = ch;
				xml->type = XML_noop;
			}
			else if (ch == '/' && (END_TAG_TYPE(xml) == XML_OP_1_END_TAG)) {
				xml->state = XML_SLASH;
				xml->type = XML_noop;
			}
			else if (ch == '?' && (END_TAG_TYPE(xml) == XML_OP_1_END_PI_TAG)) {
				xml->state = XML_QMARK;
				xml->type = XML_noop;
			}
			else if (ch == ']' && (END_TAG_TYPE(xml) == XML_OP_1_END_BRACKET_TAG)) {
				xml->state = XML_SQR;
				xml->type = XML_noop;
			}
			else if (ISSINGLE(ch)) {
				xml->state = XML_BEFORE_ATTR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_ATTR_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_ATTR_CH;
				else
					xml->type = XML_add_attr_ATTR_CH;
				xml->step = XML_STEP_ATTR;
				SET_CHAR(xml, ch);
			}
			else {
				xml->state = XML_IN_VALUE;
				xml->quote = 0;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_VALUE_CH;
				else if (xml->step == XML_STEP_VALUE)
					xml->type = XML_add_attr_VALUE_CH;
				else
					xml->type = XML_VALUE_CH;
				xml->step = XML_STEP_VALUE;
				SET_CHAR(xml, ch);
			}
			break;
		case XML_QUOTE_BEFORE_VALUE:
			if (ch == xml->quote) {
				xml->state = XML_QUOTE_AFTER_VALUE;
				// Empty string:
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_VALUE_CH;
				else if (xml->step == XML_STEP_VALUE)
					xml->type = XML_add_attr_VALUE_CH;
				else
					xml->type = XML_VALUE_CH;
				xml->step = XML_STEP_VALUE;
				xml->count = 0;
			}
			else {
				xml->state = XML_IN_VALUE;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_VALUE_CH;
				else if (xml->step == XML_STEP_VALUE)
					xml->type = XML_add_attr_VALUE_CH;
				else
					xml->type = XML_VALUE_CH;
				xml->step = XML_STEP_VALUE;
				SET_CHAR(xml, ch);
			}
			break;
		case XML_IN_VALUE:
			if (xml->quote) {
				if (ch == xml->quote) {
					xml->state = XML_QUOTE_AFTER_VALUE;
					xml->type = XML_noop;
				}
				else {
					xml->type = XML_VALUE_CH;
					SET_CHAR(xml, ch);
				}
			}
			else {
				/* A value without quotes (for HTML!) */
				if (ISSPACE(ch)) {
					xml->state = XML_BEFORE_ATTR;
					xml->type = XML_noop;
				}
				else if (ch == '<') {
					xml->state = XML_LT;
					xml->type = XML_noop;
				}
				else if (ch == '>') {
					if (xml->step == XML_STEP_TAG)
						xml->type = XML_start_tag_end_tag(END_TAG_TYPE(xml));
					else if (xml->step == XML_STEP_NESTED)
						xml->type = XML_end_tag(END_TAG_TYPE(xml));
					else
						xml->type = XML_add_attr_end_tag(END_TAG_TYPE(xml));
					xml->nesting--;
					if (xml->nesting) {
						xml->step = XML_STEP_NESTED;
						xml->state = XML_BEFORE_ATTR;
					}
					else {
						xml->step = XML_STEP_NONE;
						xml->state = XML_IN_CDATA;
					}
				}
				else if (ch == '"' || ch == '\'') {
					xml->state = XML_QUOTE_BEFORE_VALUE;
					xml->quote = ch;
					xml->type = XML_noop;
				}
				else {
					xml->type = XML_VALUE_CH;
					SET_CHAR(xml, ch);
				}
			}
			break;
		case XML_QUOTE_AFTER_VALUE:
			if (ISSPACE(ch)) {
				xml->state = XML_BEFORE_ATTR;
				xml->type = XML_noop;
			}
			else if (ch == '<') {
				xml->state = XML_LT;
				xml->type = XML_noop;
			}
			else if (ch == '>') {
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_end_tag(END_TAG_TYPE(xml));
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_end_tag(END_TAG_TYPE(xml));
				else
					xml->type = XML_add_attr_end_tag(END_TAG_TYPE(xml));
				xml->nesting--;
				if (xml->nesting) {
					xml->step = XML_STEP_NESTED;
					xml->state = XML_BEFORE_ATTR;
				}
				else {
					xml->step = XML_STEP_NONE;
					xml->state = XML_IN_CDATA;
				}
			}
			else if (ch == '"' || ch == '\'') {
				xml->state = XML_QUOTE_BEFORE_VALUE;
				xml->quote = ch;
				xml->type = XML_noop;
			}
			else if (ch == '/' && (END_TAG_TYPE(xml) == XML_OP_1_END_TAG)) {
				xml->state = XML_SLASH;
				xml->type = XML_noop;
			}
			else if (ch == '?' && (END_TAG_TYPE(xml) == XML_OP_1_END_PI_TAG)) {
				xml->state = XML_QMARK;
				xml->type = XML_noop;
			}
			else if (ch == ']' && (END_TAG_TYPE(xml) == XML_OP_1_END_BRACKET_TAG)) {
				xml->state = XML_SQR;
				xml->type = XML_noop;
			}
			else if (ISSINGLE(ch)) {
				xml->state = XML_BEFORE_ATTR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_ATTR_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_ATTR_CH;
				else
					xml->type = XML_add_attr_ATTR_CH;
				xml->step = XML_STEP_ATTR;
				SET_CHAR(xml, ch);
			}
			else {
				xml->state = XML_IN_ATTR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_ATTR_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_ATTR_CH;
				else
					xml->type = XML_add_attr_ATTR_CH;
				xml->step = XML_STEP_ATTR;
				SET_CHAR(xml, ch);
			}
			break;
		case XML_SQR:
			SET_CHAR(xml, ']');
			goto cont;
		case XML_SLASH:
			SET_CHAR(xml, '/');
			goto cont;
		case XML_QMARK:
			SET_CHAR(xml, '?');
			cont:
			if (ISSPACE(ch)) {
				xml->state = XML_BEFORE_ATTR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_TAG_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_TAG_CH;
				else if (xml->step == XML_STEP_NONE)
					xml->type = XML_end_cdata_TAG_CH;
				else
					xml->type = XML_add_attr_TAG_CH;
				xml->step = XML_STEP_ATTR;
			}
			else if (ch == '<') {
				xml->state = XML_LT;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_TAG_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_TAG_CH;
				else if (xml->step == XML_STEP_NONE)
					xml->type = XML_end_cdata_TAG_CH;
				else
					xml->type = XML_add_attr_TAG_CH;
				xml->step = XML_STEP_TAG;
			}
			else if (ch == '>') {
				if (xml->state == XML_SLASH) {
					if (xml->step == XML_STEP_TAG)
						xml->type = XML_start_tag_end_empty_tag;
					else if (xml->step == XML_STEP_NESTED)
						xml->type = XML_end_empty_tag;
					else
						xml->type = XML_add_attr_end_empty_tag;
				}
				else if (xml->state == XML_SQR) {
					if (xml->step == XML_STEP_TAG)
						xml->type = XML_start_tag_end_tag(XML_OP_1_END_BRACKET_TAG);
					else if (xml->step == XML_STEP_NESTED)
						xml->type = XML_end_tag(XML_OP_1_END_BRACKET_TAG);
					else
						xml->type = XML_add_attr_end_tag(XML_OP_1_END_BRACKET_TAG);
				}
				else {
					if (xml->step == XML_STEP_TAG)
						xml->type = XML_start_tag_end_pi_tag;
					else if (xml->step == XML_STEP_NESTED)
						xml->type = XML_end_pi_tag;
					else
						xml->type = XML_add_attr_end_pi_tag;
				}
				xml->nesting--;
				if (xml->nesting) {
					xml->step = XML_STEP_NESTED;
					xml->state = XML_BEFORE_ATTR;
				}
				else {
					xml->step = XML_STEP_NONE;
					xml->state = XML_IN_CDATA;
				}
			}
			else if (ch == '"' || ch == '\'') {
				xml->state = XML_QUOTE_BEFORE_VALUE;
				xml->quote = ch;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_TAG_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_TAG_CH;
				else if (xml->step == XML_STEP_NONE)
					xml->type = XML_end_cdata_TAG_CH;
				else
					xml->type = XML_add_attr_TAG_CH;
				xml->step = XML_STEP_ATTR;
			}
			else if (ch == '/' && (END_TAG_TYPE(xml) == XML_OP_1_END_TAG)) {
				xml->state = XML_SLASH;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_TAG_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_TAG_CH;
				else if (xml->step == XML_STEP_NONE)
					xml->type = XML_end_cdata_TAG_CH;
				else
					xml->type = XML_add_attr_TAG_CH;
				xml->step = XML_STEP_ATTR;
			}
			else if (ch == '?' && (END_TAG_TYPE(xml) == XML_OP_1_END_PI_TAG)) {
				xml->state = XML_QMARK;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_TAG_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_TAG_CH;
				else if (xml->step == XML_STEP_NONE)
					xml->type = XML_end_cdata_TAG_CH;
				else
					xml->type = XML_add_attr_TAG_CH;
				xml->step = XML_STEP_ATTR;
			}
			else if (ch == ']' && (END_TAG_TYPE(xml) == XML_OP_1_END_BRACKET_TAG)) {
				xml->state = XML_SQR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_TAG_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_TAG_CH;
				else if (xml->step == XML_STEP_NONE)
					xml->type = XML_end_cdata_TAG_CH;
				else
					xml->type = XML_add_attr_TAG_CH;
				xml->step = XML_STEP_ATTR;
			}
			else if (ISSINGLE(ch)) {
				xml->state = XML_BEFORE_ATTR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_TAG_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_TAG_CH;
				else if (xml->step == XML_STEP_NONE)
					xml->type = XML_end_cdata_TAG_CH;
				else
					xml->type = XML_add_attr_TAG_CH;
				xml->step = XML_STEP_ATTR;
				ADD_CHAR(xml, ch);
			}
			else {
				xml->state = XML_IN_ATTR;
				if (xml->step == XML_STEP_TAG)
					xml->type = XML_start_tag_TAG_CH;
				else if (xml->step == XML_STEP_NESTED)
					xml->type = XML_TAG_CH;
				else if (xml->step == XML_STEP_NONE)
					xml->type = XML_end_cdata_TAG_CH;
				else
					xml->type = XML_add_attr_TAG_CH;
				xml->step = XML_STEP_ATTR;
				ADD_CHAR(xml, ch);
			}
			break;
		case XML_IN_COMMENT:
			if (ch == '-') {
				xml->state = XML_IN_COMMENT_DASH;
				xml->type = XML_noop;
			}
			else
				xml->type = XML_COMMENT_CH;
			SET_CHAR(xml, ch);
			break;
		case XML_IN_COMMENT_DASH:
			if (ch == '-') {
				xml->state = XML_IN_COMMENT_DASH_DASH;
				xml->type = XML_noop;
			}
			else {
				xml->state = XML_IN_COMMENT;
				xml->type = XML_COMMENT_CH;
			}
			ADD_CHAR(xml, ch);
			break;
		case XML_IN_COMMENT_DASH_DASH:
			if (ch == '-') {
				xml->state = XML_IN_COMMENT_3_DASH;
				xml->type = XML_COMMENT_CH;
				SET_CHAR(xml, ch);
			}
			else if (ch == '>') {
				xml->type = XML_end_comment;
				xml->nesting--;
				if (xml->nesting) {
					xml->step = XML_STEP_NESTED;
					xml->state = XML_BEFORE_ATTR;
				}
				else {
					xml->step = XML_STEP_NONE;
					xml->state = XML_IN_CDATA;
				}
			}
			else {
				xml->state = XML_IN_COMMENT;
				xml->type = XML_COMMENT_CH;
				ADD_CHAR(xml, ch);
			}
			break;
		case XML_IN_COMMENT_3_DASH:
			if (ch == '-') {
				xml->type = XML_COMMENT_CH;
				SET_CHAR(xml, ch);
			}
			else if (ch == '>') {
				xml->type = XML_end_comment;
				xml->nesting--;
				if (xml->nesting) {
					xml->step = XML_STEP_NESTED;
					xml->state = XML_BEFORE_ATTR;
				}
				else {
					xml->step = XML_STEP_NONE;
					xml->state = XML_IN_CDATA;
				}
			}
			else {
				xml->state = XML_IN_COMMENT;
				xml->type = XML_COMMENT_CH;
				SET_CHAR(xml, '-');
				ADD_CHAR(xml, '-');
				ADD_CHAR(xml, ch);
			}
			break;
		case XML_IN_CDATA_TAG:
			if (ch == ']') {
				xml->state = XML_IN_CDATA_TAG_SQR;
				xml->type = XML_noop;
			}
			else
				xml->type = XML_CDATA_TAG_CH;
			SET_CHAR(xml, ch);
			break;
		case XML_IN_CDATA_TAG_SQR:
			if (ch == ']') {
				xml->state = XML_IN_CDATA_TAG_SQR_SQR;
				xml->type = XML_noop;
			}
			else {
				xml->state = XML_IN_CDATA_TAG;
				xml->type = XML_CDATA_TAG_CH;
			}
			ADD_CHAR(xml, ch);
			break;
		case XML_IN_CDATA_TAG_SQR_SQR:
			if (ch == ']') {
				xml->state = XML_IN_CDATA_TAG_3_SQR;
				xml->type = XML_CDATA_TAG_CH;
				SET_CHAR(xml, ch);
			}
			else if (ch == '>') {
				xml->type = XML_end_cdata_tag;
				xml->nesting--;
				if (xml->nesting) {
					xml->step = XML_STEP_NESTED;
					xml->state = XML_BEFORE_ATTR;
				}
				else {
					xml->step = XML_STEP_NONE;
					xml->state = XML_IN_CDATA;
				}
			}
			else {
				xml->state = XML_IN_CDATA_TAG;
				xml->type = XML_CDATA_TAG_CH;
				ADD_CHAR(xml, ch);
			}
			break;
		case XML_IN_CDATA_TAG_3_SQR:
			if (ch == ']') {
				xml->type = XML_CDATA_TAG_CH;
				SET_CHAR(xml, ch);
			}
			else if (ch == '>') {
				xml->type = XML_end_cdata_tag;
				xml->nesting--;
				if (xml->nesting) {
					xml->step = XML_STEP_NESTED;
					xml->state = XML_BEFORE_ATTR;
				}
				else {
					xml->step = XML_STEP_NONE;
					xml->state = XML_IN_CDATA;
				}
			}
			else {
				xml->state = XML_IN_CDATA_TAG;
				xml->type = XML_CDATA_TAG_CH;
				SET_CHAR(xml, ']');
				ADD_CHAR(xml, ']');
				ADD_CHAR(xml, ch);
			}
			break;
	}
	return(xml->type);
}

// Private use are: E000 - F8FF

static int
capture_initializer(XMLProcessStatePtr xml, UniChar ch)
/* We capture tag and attribute data for the parsing purposes.
 * The buffers are initialized here (at the lowest level)
 * of processing after parsing.
 */
{
	int op;

	op = xml_parser(&xml->p, ch);
	switch (op & XML_OP_1_MASK) {
		case XML_OP_1_START_TAG:
			xml->tlength = 0;
			break;
		case XML_OP_1_ADD_ATTR:
			xml->nlength = 0;
			xml->vlength = 0;
			break;
	}
	return(op);
}

static int
entity_translator(XMLProcessStatePtr xml, UniChar ch)
/* This function handles entities.
 * Certain entities are translated into UNICODE characters.
 * Strictly speaking, these enties are only recognised by HTML.
 * The few entities that are recognised by XML are first translated
 * into some reserved characters for the parser. This is to ensure
 * that the parser does not recognize them as characters with special
 * meaning! This includes '&', '<' and '>'.
 */
{
	int op;

	op = capture_initializer(xml, ch);
	return(op);
}

static int
charset_transformer(XMLProcessStatePtr xml, UniChar ch)
/*
 * This function translates the input character stream into UNICODE.
 */
{
	int op;

	// Do transformation according to the charset.
	/*
	switch (xml->charset_type) {
		case CHARSET_UTF_8:
			if (ch > 127 && ch < 256) {
				Word4 utf_value;
				Word1 utf_ch = ch;

				if ((utf_ch & 0xC0) != 0x80)
					xml->utf8_count = 0;
				if ((utf_ch & 0x80) == 0x00)
					xml->utf8_length = 1;
				else if ((utf_ch & 0xE0) == 0xC0)
					xml->utf8_length = 2;
				else if ((utf_ch & 0xF0) == 0xE0)
					xml->utf8_length = 3;
				else if ((utf_ch & 0xF8) == 0xF0)
					xml->utf8_length = 4;
				else if ((utf_ch & 0xFC) == 0xF8)
					xml->utf8_length = 5;
				else if ((utf_ch & 0xFE) == 0xFC)
					xml->utf8_length = 6;
				xml->utf8_buffer[xml->utf8_count] = (Word4) utf_ch;
				xml->utf8_count++;
				if (xml->utf8_count < xml->utf8_length) {
					// I need more bytes!
					xml->p.type = XML_noop;
					return(XML_noop);
				}
				utf_value = 0;
				switch (xml->utf8_length) {
					case 1:
						utf_value = xml->utf8_buffer[0] & 0x0000007F;
						break;
					case 2:
						utf_value = ((xml->utf8_buffer[0] & 0x0000001F) << 6) |
									(xml->utf8_buffer[1] & 0x0000003F);
						if (utf_value < 0x00000080)
							utf_value = '?';
						break;
					case 3:
						utf_value = ((xml->utf8_buffer[0] & 0x0000000F) << 12) |
									((xml->utf8_buffer[1] & 0x0000003F) << 6) |
									(xml->utf8_buffer[2] & 0x0000003F);
						if (utf_value < 0x000000800)
							utf_value = '?';
						break;
					case 4:
						utf_value = ((xml->utf8_buffer[0] & 0x00000007) << 18) |
									((xml->utf8_buffer[1] & 0x0000003F) << 12) |
									((xml->utf8_buffer[2] & 0x0000003F) << 6) |
									(xml->utf8_buffer[3] & 0x0000003F);
						if (utf_value < 0x00010000)
							utf_value = '?';
						break;
					case 5:
						utf_value = ((xml->utf8_buffer[0] & 0x00000003) << 24) |
									((xml->utf8_buffer[1] & 0x0000003F) << 18) |
									((xml->utf8_buffer[2] & 0x0000003F) << 12) |
									((xml->utf8_buffer[3] & 0x0000003F) << 6) |
									(xml->utf8_buffer[4] & 0x0000003F);
						if (utf_value < 0x00200000)
							utf_value = '?';
						break;
					case 6:
						utf_value = ((xml->utf8_buffer[0] & 0x00000001) << 30) |
									((xml->utf8_buffer[1] & 0x0000003F) << 24) |
									((xml->utf8_buffer[2] & 0x0000003F) << 18) |
									((xml->utf8_buffer[3] & 0x0000003F) << 12) |
									((xml->utf8_buffer[4] & 0x0000003F) << 6) |
									(xml->utf8_buffer[5] & 0x0000003F);
						if (utf_value < 0x04000000)
							utf_value = '?';
						break;
				}
				if (utf_value > 0x0000FFFF)
					ch = '?';
				else
					ch = utf_value;
			}
			break;
		case CHARSET_TO_CONVERT_8_BIT:
			//if (ch > 127 && xml->conversion_table && ch < 256)
			//	ch = ConvertToUni(xml->conversion_table, (char) ch);
			break;
	}
	*/

	op = entity_translator(xml, ch);

	// Determine the characters set:
	switch (op & XML_OP_1_MASK) {
		case XML_OP_1_START_TAG:
			if (strcmp(xml->tag, "?xml") == 0)
				xml->ip = true;
			else
				xml->ip = false;
			break;
		case XML_OP_1_ADD_ATTR:
			if (xml->ip) {
				if (strcmp(xml->name, "encoding") == 0) {
					strcpy(xml->charset, xml->value);
					if (strstr(xml->charset, "utf-8") || strstr(xml->charset, "utf8"))
						xml->charset_type = CHARSET_UTF_8;
					else if (strstr(xml->charset, "ucs-2") ||
						strstr(xml->charset, "ucs-4") ||
						strstr(xml->charset, "unicode"))
						xml->charset_type = CHARSET_STANDARD;
					else {
						xml->result = ERR_NOT_IMP;
						op |= XML_ERROR;
						xml->p.type = op;
					}
				}
			}
			break;
	}
	return(op);
}

PUBLIC void
ProcessXMLDone(XMLProcessStatePtr xml)
{
	//if (xml->conversion_table) {
	//	ReleaseConversionTable(xml->conversion_table);
	//	xml->conversion_table = NULL;
	//}
}

PUBLIC int
ProcessXMLChar(XMLProcessStatePtr xml, UniChar ch)
{
	int		op;
	int		i;
	UniChar	*ptr;

	op = charset_transformer(xml, ch);

	/*
	 * Capture output tag and attribute data.
	 * This must be done at the highest level, after
	 * parsing.
	 */
	switch (op & XML_DATA_MASK) {
		case XML_DATA_TAG:
			ptr = xml->p.buffer;
			for (i=0; i < xml->p.count; i++) {
				if (xml->tlength < MAX_XML_NAME_SIZE-1) {
					if (*ptr > 255)
						xml->tag[xml->tlength] = '?';
					else
						xml->tag[xml->tlength] = *ptr;
					xml->tlength++;
					ptr++;
					xml->tag[xml->tlength] = 0;
				}
			}
			break;
		case XML_DATA_ATTR:
			ptr = xml->p.buffer;
			for (i=0; i < xml->p.count; i++) {
				if (xml->nlength < MAX_XML_NAME_SIZE-1) {
					if (xml->p.buffer[i] > 255)
						xml->name[xml->nlength] = '?';
					else
						xml->name[xml->nlength] = *ptr;
					xml->nlength++;
					ptr++;
					xml->name[xml->nlength] = 0;
				}
			}
			break;
		case XML_DATA_VALUE:
			ptr = xml->p.buffer;
			for (i=0; i < xml->p.count; i++) {
				if (xml->vlength < MAX_XML_NAME_SIZE-1) {
					if (xml->p.buffer[i] > 255)
						xml->value[xml->vlength] = '?';
					else
						xml->value[xml->vlength] = *ptr;
					xml->vlength++;
					xml->value[xml->vlength] = 0;
				}
			}
			break;
	}
	return(op);
}

PUBLIC void
InitXMLState(XMLProcessStatePtr xml)
{
	memset(xml, 0, sizeof(XMLProcessStateRec));
	xml->charset_type = CHARSET_UTF_8;
}

