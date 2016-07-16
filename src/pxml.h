#ifndef __pxml__
#define __pxml__

/* pxml.h 23.3.01 Paul McCullagh */
/* Parse XML */
/* Entities understood by XML:
   &gt;		(>)
   &lt;		(<)
   &amp;	(&)
   &apos;	(')
   &quot;	(")

   Processing Instructions		<? ... ?>
   CDATA Sections				<![CDATA[ ... ]]>
   Document Type Definition		<!DOCTYPE ... [ ...markup... ] >
   Conditional Sections			<![ ... [ ...markup... ]]>
   
 */

#define UniChar						uint16_t
#define Word1						uint8_t
#define Word4						uint32_t
#define PUBLIC

#define ERR_NOT_IMP					-1

//#include "yv_defs.h"
//#include "convchar.h"
#include <inttypes.h>

#define XML_BEFORE_CDATA			0		/* XXX */
#define XML_IN_CDATA				1		/* XXX */

#define XML_LT						2		/* < */
#define XML_LT_BANG					3		/* <! */
#define XML_LT_BANG_DASH			4		/* <!- */
#define XML_LT_BANG_SQR				5		/* <![ */
#define XML_LT_BANG_SQR_IN_NAME		6
#define XML_LT_BANG_SQR_AFTER_NAME	7

#define XML_IN_TAG_NAME				8		/* abc */

#define XML_BEFORE_ATTR				9		/* ' ' */
#define XML_IN_ATTR					10		/* xyz */

#define XML_BEFORE_EQUAL			11		/* ' ' */
#define XML_AFTER_EQUAL				12		/* ' ' */

#define XML_QUOTE_BEFORE_VALUE		13		/* " or ' */
#define XML_IN_VALUE				14		/* ... */
#define XML_QUOTE_AFTER_VALUE		15		/* " or ' */

#define XML_SLASH					16		/* / */
#define XML_QMARK					17		/* ? */
#define XML_SQR						18		/* ] */

#define XML_IN_COMMENT				19		/* <!--... */
#define XML_IN_COMMENT_DASH			20		/* - */
#define XML_IN_COMMENT_DASH_DASH	21		/* -- */
#define XML_IN_COMMENT_3_DASH		22		/* --- */

#define XML_IN_CDATA_TAG			23		/* <![CDATA[... */
#define XML_IN_CDATA_TAG_SQR		24		/* ] */
#define XML_IN_CDATA_TAG_SQR_SQR	25		/* ]] */
#define XML_IN_CDATA_TAG_3_SQR		26		/* ]]] */

#define PARSE_BUFFER_SIZE			20
#define PARSE_STACK_SIZE			200

#define END_TAG_TYPE(x)				(x->nesting-1 < PARSE_STACK_SIZE ? x->end_type[x->nesting-1] : XML_OP_1_END_UNKNOWN_TAG)

#define		TO_LONG_CHAR(ch)	((unigned char) (ch))

#define XML_STEP_NONE				0
#define XML_STEP_TAG				1
#define XML_STEP_ATTR				2
#define XML_STEP_VALUE				3
#define XML_STEP_NESTED				4

typedef struct XMLParseStateRec {
	/* Internal information: */
	int				state;
	int				quote;
	int				step;

	/* Data: output is always in the buffer: */
	int				type;							/* Type of data in the buffer. */
	int				count;							/* Size of the buffer.  */
	UniChar			buffer[PARSE_BUFFER_SIZE];		/* Contains data to be added. */

	/* Signals: tag start and end: */
	int				nesting;						/* Tag nesting depth. */
	Word1			end_type[PARSE_STACK_SIZE];		/* Stack of tag types */
} XMLParseStateRec, *XMLParseStatePtr;

#define XML_OP_1_MASK					0x0000000F
#define XML_ERROR						0x00001000

#define XML_OP_1_NOOP					0x00000000
#define XML_OP_1_END_TAG				0x00000001		/* < ... >   */
#define XML_OP_1_END_CLOSE_TAG			0x00000002		/* </ ... >  */
#define XML_OP_1_END_EMPTY_TAG			0x00000003		/* < ... />  */
#define XML_OP_1_END_PI_TAG				0x00000004		/* <? ... ?> */
#define XML_OP_1_END_ENTITY_TAG			0x00000005		/* <! ... >  */
#define XML_OP_1_END_BRACKET_TAG		0x00000006		/* <![ ... ]> */
#define XML_OP_1_END_UNKNOWN_TAG		0x00000007		/* <_ ... > */
#define XML_OP_1_START_CDATA_TAG		0x00000008		/* <![CDATA[ ... */
#define XML_OP_1_START_COMMENT			0x00000009		/* <!-- ... */
#define XML_OP_1_START_TAG				0x0000000A		/* <... */
#define XML_OP_1_ADD_ATTR				0x0000000B
#define XML_OP_1_END_CDATA				0x0000000C
#define XML_OP_1_END_CDATA_TAG			0x0000000D		/* ... ]]> */
#define XML_OP_1_END_COMMENT			0x0000000E		/* ... --> */

#define XML_DATA_MASK					0x000000F0

#define XML_NO_DATA						0x00000000
#define XML_DATA_TAG					0x00000010
#define XML_DATA_ATTR					0x00000020
#define XML_DATA_CDATA					0x00000030
#define XML_DATA_CDATA_TAG				0x00000040
#define XML_COMMENT						0x00000050
#define XML_DATA_VALUE					0x00000060

#define XML_OP_2_MASK					0x00000F00

#define XML_OP_2_NOOP					0x00000000
#define XML_OP_2_END_TAG				0x00000100
#define XML_OP_2_END_CLOSE_TAG			0x00000200
#define XML_OP_2_END_EMPTY_TAG			0x00000300
#define XML_OP_2_END_PI_TAG				0x00000400
#define XML_OP_2_END_ENTITY_TAG			0x00000500
#define XML_OP_2_END_BRACKET_TAG		0x00000600
#define XML_OP_2_END_UNKNOWN_TAG		0x00000700
#define XML_OP_2_START_CDATA_TAG		0x00000800
#define XML_OP_2_START_COMMENT			0x00000900

#define XML_noop						(XML_OP_2_NOOP|XML_NO_DATA)

#define XML_CDATA_CH					(XML_DATA_CDATA)
#define XML_end_cdata_TAG_CH			(XML_OP_1_END_CDATA|XML_DATA_TAG)
#define XML_start_tag_TAG_CH			(XML_OP_1_START_TAG|XML_DATA_TAG)
#define XML_add_attr_TAG_CH				(XML_OP_1_ADD_ATTR|XML_DATA_TAG)
#define XML_TAG_CH						(XML_DATA_TAG)
#define XML_start_tag_ATTR_CH			(XML_OP_1_START_TAG|XML_DATA_ATTR)
#define XML_add_attr_ATTR_CH			(XML_OP_1_ADD_ATTR|XML_DATA_ATTR)
#define XML_ATTR_CH						(XML_DATA_ATTR)
#define XML_start_tag_VALUE_CH			(XML_OP_1_START_TAG|XML_DATA_VALUE)
#define XML_add_attr_VALUE_CH			(XML_OP_1_ADD_ATTR|XML_DATA_VALUE)
#define XML_VALUE_CH					(XML_DATA_VALUE)
#define XML_start_tag_end_tag(x)		(XML_OP_1_START_TAG|((x) << 8))
#define XML_add_attr_end_tag(x)			(XML_OP_1_ADD_ATTR|((x) << 8))
#define XML_end_tag(x)					(x)
#define XML_start_tag_end_empty_tag		XML_start_tag_end_tag(XML_OP_1_END_EMPTY_TAG)
#define XML_add_attr_end_empty_tag		XML_add_attr_end_tag(XML_OP_1_END_EMPTY_TAG)
#define XML_end_empty_tag				XML_end_tag(XML_OP_1_END_EMPTY_TAG)
#define XML_start_tag_end_pi_tag		XML_start_tag_end_tag(XML_OP_1_END_PI_TAG)
#define XML_add_attr_end_pi_tag			XML_add_attr_end_tag(XML_OP_1_END_PI_TAG)
#define XML_end_pi_tag					XML_end_tag(XML_OP_1_END_PI_TAG)

#define XML_end_cdata_start_cdata_tag	(XML_OP_1_END_CDATA|XML_OP_2_START_CDATA_TAG)
#define XML_start_tag_start_cdata_tag	(XML_OP_1_START_TAG|XML_OP_2_START_CDATA_TAG)
#define XML_add_attr_start_cdata_tag	(XML_OP_1_ADD_ATTR|XML_OP_2_START_CDATA_TAG)
#define XML_start_cdata_tag				(XML_OP_1_START_CDATA_TAG)
#define XML_CDATA_TAG_CH				(XML_DATA_CDATA_TAG)
#define XML_end_cdata_tag				(XML_OP_1_END_CDATA_TAG)

#define XML_end_cdata_start_comment		(XML_OP_1_END_CDATA|XML_OP_2_START_COMMENT)
#define XML_start_tag_start_comment		(XML_OP_1_START_TAG|XML_OP_2_START_COMMENT)
#define XML_add_attr_start_comment		(XML_OP_1_ADD_ATTR|XML_OP_2_START_COMMENT)
#define XML_start_comment				(XML_OP_1_START_COMMENT)
#define XML_COMMENT_CH					(XML_COMMENT)
#define XML_end_comment					(XML_OP_1_END_COMMENT)

#define MAX_XML_NAME_SIZE				48

/* Standard charsets are ISO-8879-1, US-ASCII or UNICODE. None
 * require conversion!
 */
#define CHARSET_STANDARD				0
#define CHARSET_UTF_8					1
#define CHARSET_TO_CONVERT_8_BIT		2

typedef struct XMLProcessStateRec {
	int					result;
	char				err_file[MAX_XML_NAME_SIZE];

	bool				ip;

	int					tlength;
	char				tag[MAX_XML_NAME_SIZE];
	int					nlength;
	char				name[MAX_XML_NAME_SIZE];
	int					vlength;
	char				value[MAX_XML_NAME_SIZE];

	//int					utf8_count;
	//int					utf8_length;
	//Word4				utf8_buffer[6];

	int					charset_type;
	//ConvCharTablePtr	conversion_table;
	char				charset[MAX_XML_NAME_SIZE];

	int					elength;
	char				entity[MAX_XML_NAME_SIZE];

	XMLParseStateRec	p;
} XMLProcessStateRec, *XMLProcessStatePtr;

/* This function processes a UNICODE character from an XML
 * document returns parsing instructions (operations).
 * Each instruction can consist of up to 3 operations. The
 * operations must be executed in the following order:
 * - Operation 1
 * - Data operation, record one of the following:
 *   - part of a tag name
 *   - part of an attribute name
 *   - part of an attribute value
 *   - part of CDATA
 * - Operation 2
 * Output for the data operation (if any) is placed in the buffer
 * in the state structure. The input state structure must be zeroed
 * before processing begins. Input characters may be 1 byte or
 * 2 byte. Output is always 2-byte UNICODE.
 */
int		ProcessXMLChar(XMLProcessStatePtr xml, UniChar ch);

/* A call to this function is required to free resources
 * allocated for character conversion.
 */
void	ProcessXMLDone(XMLProcessStatePtr xml);

void	InitXMLState(XMLProcessStatePtr xml);

#endif
