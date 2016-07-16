//#define MD5_TEST_DRIVER
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//================================================================
/* MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
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

//#include "yv_config.h"
//#include "sqldefs.h"
#include "md5.h"
//#include "calcconv.h"
//#include "yv_str_util.h"

/* Constants for MD5Transform routine.
 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21


static uint8_t PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (uint32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + (uint32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + (uint32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + (uint32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
  
  
/*======================================================================*/
/*						static FUNCTIONS								*/
/*======================================================================*/
/* Encodes input (uint32_t) into output (unsigned char). Assumes len is
  a multiple of 4.
 */
static void encode(uint8_t *output, uint32_t *input, uint32_t len)
{
	uint32_t i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (uint8_t)(input[i] & 0xff);
		output[j+1] = (uint8_t)((input[i] >> 8) & 0xff);
		output[j+2] = (uint8_t)((input[i] >> 16) & 0xff);
		output[j+3] = (uint8_t)((input[i] >> 24) & 0xff);
	}
}

/*======================================================================*/
/* Decodes input (unsigned char) into output (uint32_t). Assumes len is
  a multiple of 4.
 */
static void decode(uint32_t *output, uint8_t *input, uint32_t len)
{
uint32_t i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[i] = ((uint32_t)input[j]) |
					(((uint32_t)input[j+1]) << 8) |
					(((uint32_t)input[j+2]) << 16) |
					(((uint32_t)input[j+3]) << 24);
	}
}


/*======================================================================*/
/* MD5 basic transformation. Transforms state based on block. */
static void MD5Transform (uint32_t *state, uint8_t *block)
{
uint32_t a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	decode(x, block, 64);

	/* Round 1 */
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	/* Zeroize sensitive information.*/
	memset((void *)x, 0, sizeof (x));
}

/*======================================================================*/
/*						PUBLIC FUNCTIONS								*/
/*======================================================================*/
// MD5 initialization. Begins an MD5 operation, writing a new context.
void PB_MD5Init(Md5Ctx *context)
{
	context->count[0] = context->count[1] = 0;

	/* Load magic initialization constants.*/
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

/*======================================================================*/
/* MD5 block update operation. Continues an MD5 message-digest
  operation, processing another message block, and updating the
  context.
 */
void PB_MD5Update(Md5Ctx *context, uint8_t *input, uint32_t inputLen)
{
uint32_t i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (uint32_t)((context->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if ((context->count[0] += ((uint32_t)inputLen << 3)) < ((uint32_t)inputLen << 3)) {
		context->count[1]++;
	}
  
	context->count[1] += ((uint32_t)inputLen >> 29);

	partLen = 64 - index;

	// Transform as many times as possible.
	if (inputLen >= partLen) {
 		memcpy(&context->buffer[index], input, partLen);
		MD5Transform (context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
			MD5Transform (context->state, &input[i]);

		index = 0;
	} else
		i = 0;

	/* Buffer remaining input */
	memcpy((void *)&context->buffer[index], (void *)&input[i],inputLen-i);
}

/*======================================================================*/
/* MD5 finalization. Ends an MD5 message-digest operation, writing the
  the message digest and zeroizing the context.
 */
void PB_MD5Final(uint8_t *digest, Md5Ctx *context)
{
uint8_t bits[8];
uint32_t index, padLen;

	/* Save number of bits */
	encode(bits, context->count, 8);

	/* Pad out to 56 mod 64.*/
	index = (uint32_t)((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	PB_MD5Update (context, PADDING, padLen);

	/* Append length (before padding) */
	PB_MD5Update (context, bits, 8);

	/* Store state in digest */
	encode(digest, context->state, 16);

	/* Zeroize sensitive information.*/
	memset((void *)context, 0, sizeof(*context));
}

Md5::Md5()
{
	PB_MD5Init(&context);
}

void Md5::update(const char* input, uint32_t inputLen)
{
	PB_MD5Update(&context, (uint8_t*) input, inputLen);
}

const char* Md5::final()
{
	PB_MD5Final(digest, &context);
	return (char*) digest;
}

QString Md5::hash(const QByteArray& data)
{
	Md5 md5;
	const char* result;

	md5.update(data.constData(), data.length());
	result = md5.final();
	return QString::fromUtf8(QByteArray(result, 16).toHex());
}

#ifdef MD5_TEST_DRIVER
//================================================================
/* MDDRIVER.C - test driver for MD2, MD4 and MD5
 */

/* Copyright (C) 1990-2, RSA Data Security, Inc. Created 1990. All
rights reserved.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

/* Length of test block, number of test blocks.
 */
#define TEST_BLOCK_LEN 1000
#define TEST_BLOCK_COUNT 1000

//================================================================
/* Prints a message digest in hexadecimal. */
static void MDPrint (uint8_t *digest)
{
uint32_t i;

	for (i = 0; i < 16; i++)
		printf ("%02x", digest[i]);
}


//================================================================
/* Digests a string and prints the result.*/
static void MDString(char *string)
{
Md5Ctx context;
uint8_t digest[16];
uint32_t len = strlen(string);

	PB_MD5Init(&context);
	PB_MD5Update(&context, (uint8_t *)string, len);
	PB_MD5Final(digest, &context);

	printf("MD5(\"%s\") = ",string);
	MDPrint(digest);
	printf("\n");
}

//================================================================
/* Measures the time to digest TEST_BLOCK_COUNT TEST_BLOCK_LEN-byte
  blocks.
 */
static void MDTimeTrial (void)
{
Md5Ctx context;
time_t endTime, startTime;
uint8_t block[TEST_BLOCK_LEN], digest[16];
uint32_t i;

	printf("MD5 time trial. Digesting %d %d-byte blocks ...", 
		TEST_BLOCK_LEN, TEST_BLOCK_COUNT);

	/* Initialize block */
	for (i = 0; i < TEST_BLOCK_LEN; i++)
		block[i] = (unsigned char)(i & 0xff);

	/* Start timer */
	time(&startTime);

	/* Digest blocks */
	PB_MD5Init (&context);
	for (i = 0; i < TEST_BLOCK_COUNT; i++)
		PB_MD5Update (&context, block, TEST_BLOCK_LEN);
		
	PB_MD5Final (digest, &context);

	/* Stop timer */
	time (&endTime);

	printf (" done\n");
	printf ("Digest = ");
	MDPrint (digest);
	printf ("\nTime = %ld seconds\n", (long)(endTime-startTime));
	printf("Speed = %ld bytes/second\n", 
		(long)TEST_BLOCK_LEN * (long)TEST_BLOCK_COUNT/(endTime-startTime));
}

//================================================================
/* Digests a reference suite of strings and prints the results.
 */
static void MDTestSuite ()
{
	printf ("MD5 test suite:\n");

	MDString("");
	MDString("a");
	MDString("abc");
	MDString("message digest");
	MDString("abcdefghijklmnopqrstuvwxyz");
	MDString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	MDString("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
}

//================================================================
/* Digests a file and prints the result.*/
static void MDFile(char *filename)
{
FILE *file;
Md5Ctx context;
Int4 len;
uint8_t buffer[1024], digest[16];

	if ((file = fopen (filename, "rb")) == NULL)
		printf ("%s can't be opened\n", filename);
	else {
		PB_MD5Init(&context);
		while (len = fread (buffer, 1, 1024, file))
			PB_MD5Update (&context, buffer, len);
				
		PB_MD5Final(digest, &context);

		fclose(file);

		printf("PB_MD5 (%s) = ", filename);
		MDPrint(digest);
		printf("\n");
	}
}

//================================================================
/* Digests the standard input and prints the result.*/
static void MDFilter (void)
{
Md5Ctx context;
Int4 len;
uint8_t buffer[16], digest[16];

	PB_MD5Init(&context);
	while (len = fread (buffer, 1, 16, stdin))
		PB_MD5Update(&context, buffer, len);
		
	PB_MD5Final(digest, &context);

	MDPrint(digest);
	printf("\n");
}

//================================================================
//================================================================
/*
A.5 Test suite

   The MD5 test suite (driver option "-x") should print the following
   results:

MD5 test suite:
MD5 ("") = d41d8cd98f00b204e9800998ecf8427e
MD5 ("a") = 0cc175b9c0f1b6a831c399e269772661
MD5 ("abc") = 900150983cd24fb0d6963f7d28e17f72
MD5 ("message digest") = f96b697d7cb7938d525a2f31aaf161d0
MD5 ("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b
MD5 ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") =
d174ab98d277d9f5a5611c2c9f419d9f
MD5 ("123456789012345678901234567890123456789012345678901234567890123456
78901234567890") = 57edf4a22be3c955ac49da2e2107b67a
*/
/* Main driver.

Arguments (may be any combination):
  -sstring - digests string
  -t       - runs time trial
  -x       - runs test script
  filename - digests file
  (none)   - digests standard input
 */
int main (int argc, char *argv[])
{
int i;

#ifdef INTERACTIVE
	if (argc > 1) {
		for (i = 1; i < argc; i++) {
			if (argv[i][0] == '-' && argv[i][1] == 's')
				MDString (argv[i] + 2);
			else if (strcmp (argv[i], "-t") == 0)
				MDTimeTrial ();
			else if (strcmp (argv[i], "-x") == 0)
				MDTestSuite ();
			else
	 			MDFile (argv[i]);
	 	} 
	 } else
			MDFilter ();
#else
	MDTestSuite ();
#endif
	return (0);
}


#endif // MD5_TEST_DRIVER