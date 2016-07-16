/* MD5.H - header file for MD5C.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

//#include "yv_defs.h"

#include <inttypes.h>

#include <QString>
#include <QByteArray>

/* MD5 context. */
typedef struct {
  uint32_t state[4];                                   /* state (ABCD) */
  uint32_t count[2];        /* number of bits, modulo 2^64 (lsb first) */
  uint8_t buffer[64];                         /* input buffer */
} Md5Ctx;

void PB_MD5Init(Md5Ctx *context);
void PB_MD5Update(Md5Ctx *context, uint8_t *input, uint32_t inputLen);
void PB_MD5Final(uint8_t *digest, Md5Ctx *context);
//void PB_MD5(uint32_t *digest, char *string);

//#define  encryp_str pb_strndup
//char *encryp_str(char *str);

class Md5 {
public:
	Md5();

	void update(const char* input, uint32_t inputLen);
	const char* final();

	static QString hash(const QByteArray& data);

private:
	Md5Ctx	context;
	uint8_t	digest[16];
};



