/*

    This second implementation of MD4 entry points is the newer style,
    optimized implementation, compatible with newer code expecting these
    entry point names.

    Modelled after MD5 and SHA-1.

    Scott Field (sfield)    21-Oct-97

*/


#include "xonp.h"
#include "xonver.h"
#include "md4.h"
#include "uint4.h"


#ifndef RSA32API 
#define RSA32API
#endif

/* Compile-time macro declarations for MD4.
*/

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

void MD4Transform (UINT4 state[4], unsigned char block[64]);


#define S11 3
#define S12 7
#define S13 11
#define S14 19
#define S21 3
#define S22 5
#define S23 9
#define S24 13
#define S31 3
#define S32 9
#define S33 11
#define S34 15


static const unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


/* F, G and H are basic MD4 functions.
 */

#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z) (((x) & (y)) | ((z) & ((x) | (y))))
#define H(x, y, z) ((x) ^ (y) ^ (z))


/* FF, GG and HH are MD4 transformations for rounds 1, 2 and 3 */
/* Rotation is separate from addition to prevent recomputation */
#define FF(a, b, c, d, x, s) \
  {(a) += F ((b), (c), (d)) + (x); \
   (a) = ROTATE_LEFT ((a), (s));}
#define GG(a, b, c, d, x, s) \
  {(a) += G ((b), (c), (d)) + (x) + (UINT4)013240474631; \
   (a) = ROTATE_LEFT ((a), (s));}
#define HH(a, b, c, d, x, s) \
  {(a) += H ((b), (c), (d)) + (x) + (UINT4)015666365641; \
   (a) = ROTATE_LEFT ((a), (s));}


#ifdef KMODE_RSA32
#pragma alloc_text(PAGER32C, MD4Init)
#pragma alloc_text(PAGER32C, MD4Update)
#pragma alloc_text(PAGER32C, MD4Transform)
#pragma alloc_text(PAGER32C, MD4Final)
#endif  // KMODE_RSA32

/* MD4 initialization. Begins an MD4 operation, writing a new context.
 */
void
RSA32API
MD4Init (
    MD4_CTX *context                                         /* context */
    )
{
    context->count[0] = 0;
    context->count[1] = 0;

    /* Load magic initialization constants.
     */

    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

/* MD4 block update operation. Continues an MD4 message-digest
     operation, processing another message block, and updating the
     context.
 */

void
RSA32API
MD4Update (
    MD4_CTX *context,                                        /* context */
    unsigned char *input,                                /* input block */
    unsigned int inputLen                      /* length of input block */
    )
{

  unsigned int bufferLen;

  /* Compute number of bytes mod 64 */
  bufferLen = (unsigned int)((context->count[0] >> 3) & 0x3F);

  /* Update number of bits */
  if ((context->count[0] += ((DWORD)inputLen << 3)) < ((DWORD)inputLen << 3))
    context->count[1]++;

  context->count[1] += ((DWORD)inputLen >> 29);

  /* If previous input in buffer, buffer new input and transform if
       possible.
   */
  if (bufferLen > 0 && bufferLen + inputLen >= 64) {
    memcpy(context->buffer+bufferLen, input, 64-bufferLen);
    input += (64-bufferLen);
    inputLen -= (64-bufferLen);
    MD4Transform (context->state, context->buffer);
    bufferLen = 0;
  }

    if( (DWORD_PTR)input & (sizeof(DWORD_PTR) - 1) ) {
        /* Copy input to aligned temporary buffer
         */
        while (inputLen >= 64) {
            memcpy( context->buffer, input, 64 );
            MD4Transform (context->state, context->buffer);
            input += 64;
            inputLen -= 64;
        }
    } else {
        /* Transform directly from input.
         */
        while (inputLen >= 64) {
            MD4Transform (context->state, input);
            input += 64;
            inputLen -= 64;
        }
    }

  /* Buffer remaining input */
  if (inputLen)
    memcpy(context->buffer+bufferLen, input, inputLen);

}

/* MD4 finalization. Ends an MD4 message-digest operation, writing the
     message digest and zeroizing the context.
 */
void
RSA32API
MD4Final (
    MD4_CTX *context                                         /* context */
    )
{
    unsigned int index, padLen;

    /* Save number of bits */

#if !defined(BIGENDIAN) || !BIGENDIAN
    ((unsigned long *)context->digest)[0] = context->count[0];
    ((unsigned long *)context->digest)[1] = context->count[1];
#else
  DWORDToLittleEndian (context->digest, context->count, 8);
#endif

    /* Pad out to 56 mod 64. */

    index = (unsigned int)((context->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    MD4Update (context, (unsigned char *)PADDING, padLen);

    /* Append length (before padding) */

    MD4Update (context, context->digest, 8);

    /* Store state in digest */

#if !defined(BIGENDIAN) || !BIGENDIAN
    ((unsigned long *)context->digest)[0] = context->state[0];
    ((unsigned long *)context->digest)[1] = context->state[1];
    ((unsigned long *)context->digest)[2] = context->state[2];
    ((unsigned long *)context->digest)[3] = context->state[3];
#else
  DWORDToLittleEndian (context->digest, context->state, 16);
#endif

}

/* MD4 basic transformation. Transforms state based on block.
 */

//#ifndef _X86_ // use optimized .asm version of MD4Transform on x86

void
MD4Transform (
    UINT4 state[4],
    unsigned char block[64]
    )
{
  UINT4 a = state[0], b = state[1], c = state[2], d = state[3];

#if !defined(BIGENDIAN) || !BIGENDIAN
  unsigned long *x = (unsigned long*)block;
#else
  unsigned long x[16];
  DWORDFromLittleEndian (x, (unsigned char *)block, 64);
#endif

  /* Round 1 */
  FF (a, b, c, d, x[ 0], S11); /* 1 */
  FF (d, a, b, c, x[ 1], S12); /* 2 */
  FF (c, d, a, b, x[ 2], S13); /* 3 */
  FF (b, c, d, a, x[ 3], S14); /* 4 */
  FF (a, b, c, d, x[ 4], S11); /* 5 */
  FF (d, a, b, c, x[ 5], S12); /* 6 */
  FF (c, d, a, b, x[ 6], S13); /* 7 */
  FF (b, c, d, a, x[ 7], S14); /* 8 */
  FF (a, b, c, d, x[ 8], S11); /* 9 */
  FF (d, a, b, c, x[ 9], S12); /* 10 */
  FF (c, d, a, b, x[10], S13); /* 11 */
  FF (b, c, d, a, x[11], S14); /* 12 */
  FF (a, b, c, d, x[12], S11); /* 13 */
  FF (d, a, b, c, x[13], S12); /* 14 */
  FF (c, d, a, b, x[14], S13); /* 15 */
  FF (b, c, d, a, x[15], S14); /* 16 */

  /* Round 2 */
  GG (a, b, c, d, x[ 0], S21); /* 17 */
  GG (d, a, b, c, x[ 4], S22); /* 18 */
  GG (c, d, a, b, x[ 8], S23); /* 19 */
  GG (b, c, d, a, x[12], S24); /* 20 */
  GG (a, b, c, d, x[ 1], S21); /* 21 */
  GG (d, a, b, c, x[ 5], S22); /* 22 */
  GG (c, d, a, b, x[ 9], S23); /* 23 */
  GG (b, c, d, a, x[13], S24); /* 24 */
  GG (a, b, c, d, x[ 2], S21); /* 25 */
  GG (d, a, b, c, x[ 6], S22); /* 26 */
  GG (c, d, a, b, x[10], S23); /* 27 */
  GG (b, c, d, a, x[14], S24); /* 28 */
  GG (a, b, c, d, x[ 3], S21); /* 29 */
  GG (d, a, b, c, x[ 7], S22); /* 30 */
  GG (c, d, a, b, x[11], S23); /* 31 */
  GG (b, c, d, a, x[15], S24); /* 32 */

  /* Round 3 */
  HH (a, b, c, d, x[ 0], S31); /* 33 */
  HH (d, a, b, c, x[ 8], S32); /* 34 */
  HH (c, d, a, b, x[ 4], S33); /* 35 */
  HH (b, c, d, a, x[12], S34); /* 36 */
  HH (a, b, c, d, x[ 2], S31); /* 37 */
  HH (d, a, b, c, x[10], S32); /* 38 */
  HH (c, d, a, b, x[ 6], S33); /* 39 */
  HH (b, c, d, a, x[14], S34); /* 40 */
  HH (a, b, c, d, x[ 1], S31); /* 41 */
  HH (d, a, b, c, x[ 9], S32); /* 42 */
  HH (c, d, a, b, x[ 5], S33); /* 43 */
  HH (b, c, d, a, x[13], S34); /* 44 */
  HH (a, b, c, d, x[ 3], S31); /* 45 */
  HH (d, a, b, c, x[11], S32); /* 46 */
  HH (c, d, a, b, x[ 7], S33); /* 47 */
  HH (b, c, d, a, x[15], S34); /* 48 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
}


//+-------------------------------------------------------------------------

BOOL
KerbPasswordToKey(
    IN LPSTR pszPassword,
    OUT PUCHAR Key
    )
{
    BOOL fSucceed = FALSE;
    MD4_CTX md4Ctx;
    UNICODE_STRING Password;
    DWORD dwPasswordLen;
    DWORD dwResult;
    WCHAR* pwcUnicodePassword;

    dwPasswordLen = strlen(pszPassword);

    pwcUnicodePassword = (WCHAR*)_alloca( (dwPasswordLen + 1) * sizeof(WCHAR) );
    if (pwcUnicodePassword == NULL)
    {
        goto Cleanup;
    }
    
    dwResult = MultiByteToWideChar(
         CP_ACP,
         MB_PRECOMPOSED,
         pszPassword,
         dwPasswordLen,
         pwcUnicodePassword,
         dwPasswordLen * sizeof(WCHAR)
         );

    if (dwResult == 0) {
        goto Cleanup;
    }
    
    MD4Init( &md4Ctx );
    MD4Update( &md4Ctx, (unsigned char *)pwcUnicodePassword, dwPasswordLen * sizeof(WCHAR) );
    MD4Final( &md4Ctx );

    RtlCopyMemory( Key, md4Ctx.digest, MD4_LEN );
    
    fSucceed = TRUE;

Cleanup:
    if (!fSucceed)
    {
        TraceSz( Warning, "KerbPasswordToKey failed" );
    }
    return(fSucceed);
}

//#endif // !_X86_

