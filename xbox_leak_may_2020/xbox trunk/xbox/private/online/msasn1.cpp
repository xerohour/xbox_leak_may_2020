/* Copyright (C) Boris Nikolaus, Germany, 1996-1997. All rights reserved. */
/* Copyright (C) Microsoft Corporation, 1997-1998. All rights reserved. */

#include "xonp.h"
#include "xonver.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef ENABLE_DOUBLE
#include <float.h>
#include <math.h>

#if HAS_IEEEFP_H
#include <ieeefp.h>
#elif HAS_FLOAT_H
#include <float.h>
#endif
#endif

#define ASN1LIB
#define MULTI_LEVEL_ZONES

#define ENABLE_BER
#define ENABLE_COMPARE
#define XBOX_SPECIFIC_OPTIMIZATIONS

#include "msasn1.h"

//#include <msper.h>
extern ASN1_PUBLIC void ASN1API ASN1PEREncAlignment(ASN1encoding_t enc);

/* MS-Windows 95/MS-Windows NT */
#define THIRTYTWO_BITS  1
// #define HAS_SIXTYFOUR_BITS 1
#define HAS_FLOAT_H     1
#define HAS_FPCLASS     1
#define fpclass(_d)     _fpclass(_d)
#define finite(_d)      _finite(_d)
#define isnan(_d)       _isnan(_d)
#define FP_PINF         _FPCLASS_PINF
#define FP_NINF         _FPCLASS_NINF
// #define HAS_STRICMP     1
#define DBL_PINF        {0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x7f}
#define DBL_MINF        {0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0xff}

extern ASN1intx_t ASN1intx_0, ASN1intx_1, ASN1intx_2, ASN1intx_16, ASN1intx_256, ASN1intx_64K, ASN1intx_1G;
extern void ASN1API ASN1intx_addoctet(ASN1intx_t *, ASN1intx_t *, ASN1octet_t);
extern void ASN1API ASN1intx_suboctet(ASN1intx_t *, ASN1intx_t *, ASN1octet_t);
extern void ASN1API ASN1intx_muloctet(ASN1intx_t *, ASN1intx_t *, ASN1octet_t);
extern void ASN1API ASN1intx_inc(ASN1intx_t *);
extern void ASN1API ASN1intx_dec(ASN1intx_t *);
extern void ASN1API ASN1intx_neg(ASN1intx_t *, ASN1intx_t *);
extern ASN1uint32_t ASN1API ASN1intx_log2(ASN1intx_t *);
extern ASN1uint32_t ASN1API ASN1intx_log256(ASN1intx_t *);
extern ASN1int32_t ASN1API ASN1intx_cmp(ASN1intx_t *, ASN1intx_t *);
extern ASN1int32_t ASN1API ASN1intx_dup(ASN1intx_t *, ASN1intx_t *);
// extern void ASN1API ASN1intx_free(ASN1intx_t *);
extern void ASN1API ASN1intx_setuint32(ASN1intx_t *, ASN1uint32_t);
extern void ASN1API ASN1intx_setint32(ASN1intx_t *, ASN1int32_t);
extern int  ASN1API ASN1intxisuint64(ASN1intx_t *);
extern int  ASN1API ASN1intxisint64(ASN1intx_t *);
extern int  ASN1API ASN1intxisuint32(ASN1intx_t *);
extern int  ASN1API ASN1intxisint32(ASN1intx_t *);
extern int  ASN1API ASN1intxisuint16(ASN1intx_t *);
extern int  ASN1API ASN1intxisint16(ASN1intx_t *);
extern int  ASN1API ASN1intxisuint8(ASN1intx_t *);
extern int  ASN1API ASN1intxisint8(ASN1intx_t *);
#ifdef HAS_SIXTYFOUR_BITS
extern ASN1uint64_t ASN1API ASN1intx2uint64(ASN1intx_t *);
extern ASN1int64_t ASN1API ASN1intx2int64(ASN1intx_t *);
#endif
extern ASN1uint32_t ASN1API ASN1intx2uint32(ASN1intx_t *);
extern ASN1int32_t ASN1API ASN1intx2int32(ASN1intx_t *);
extern ASN1uint16_t ASN1API ASN1intx2uint16(ASN1intx_t *);
extern ASN1int16_t ASN1API ASN1intx2int16(ASN1intx_t *);
extern ASN1uint8_t ASN1API ASN1intx2uint8(ASN1intx_t *);
extern ASN1int8_t ASN1API ASN1intx2int8(ASN1intx_t *);
extern ASN1uint32_t ASN1API ASN1intx_octets(ASN1intx_t *);
extern ASN1uint32_t ASN1API ASN1uint32_log2(ASN1uint32_t);
extern ASN1uint32_t ASN1API ASN1uint32_log256(ASN1uint32_t);
extern ASN1uint32_t ASN1API ASN1uint32_octets(ASN1uint32_t);
extern ASN1uint32_t ASN1API ASN1uint32_uoctets(ASN1uint32_t);
extern ASN1uint32_t ASN1API ASN1int32_octets(ASN1int32_t);
extern double ASN1API ASN1intx2double(ASN1intx_t *);
extern double ASN1API ASN1real2double(ASN1real_t *);
extern double ASN1API ASN1double_minf();
extern double ASN1API ASN1double_pinf();
extern int ASN1API ASN1double_isminf(double);
extern int ASN1API ASN1double_ispinf(double);
extern int ASN1API ASN1generalizedtime2string(char *, ASN1generalizedtime_t *);
extern int ASN1API ASN1utctime2string(char *, ASN1utctime_t *);
extern int ASN1API ASN1string2generalizedtime(ASN1generalizedtime_t *, char *);
extern int ASN1API ASN1string2utctime(ASN1utctime_t *, char *);

/* ------ Comparison APIs ------ */

extern int ASN1API ASN1ztchar32string_cmp(ASN1ztchar32string_t, ASN1ztchar32string_t);
extern int ASN1API ASN1double_cmp(double, double);
extern int ASN1API ASN1real_cmp(ASN1real_t *, ASN1real_t *);
extern int ASN1API ASN1external_cmp(ASN1external_t *, ASN1external_t *);
extern int ASN1API ASN1embeddedpdv_cmp(ASN1embeddedpdv_t *, ASN1embeddedpdv_t *);
extern int ASN1API ASN1characterstring_cmp(ASN1characterstring_t *, ASN1characterstring_t *);
extern int ASN1API ASN1sequenceoflengthpointer_cmp(ASN1uint32_t, void *, ASN1uint32_t, void *, ASN1uint32_t, int (*)(void *, void *));
extern int ASN1API ASN1sequenceofsinglylinkedlist_cmp(void *, void *, ASN1uint32_t, int (*)(void *, void *));
extern int ASN1API ASN1sequenceofdoublylinkedlist_cmp(void *, void *, ASN1uint32_t, int (*)(void *, void *));
extern int ASN1API ASN1setoflengthpointer_cmp(ASN1uint32_t, void *, ASN1uint32_t, void *, ASN1uint32_t, int (*)(void *, void *));
extern int ASN1API ASN1setofsinglylinkedlist_cmp(void *, void *, ASN1uint32_t, int (*)(void *, void *));
extern int ASN1API ASN1setofdoublylinkedlist_cmp(void *, void *, ASN1uint32_t, int (*)(void *, void *));


#define ASN1BITSET(_val, _bitnr) \
    ((_val)[(_bitnr) >> 3] |= 0x80 >> ((_bitnr) & 7))
#define ASN1BITCLR(_val, _bitnr) \
    ((_val)[(_bitnr) >> 3] &= ~(0x80 >> ((_bitnr) & 7)))
#define ASN1BITTEST(_val, _bitnr) \
    ((_val)[(_bitnr) >> 3] & (0x80 >> ((_bitnr) & 7)))
    
// internal functions
int _BERDecConstructed(ASN1decoding_t dec, ASN1uint32_t len, ASN1uint32_t infinite, ASN1decoding_t *dd, ASN1octet_t **di);


#if ! defined(_DEBUG) && defined(TEST_CODER)
#undef TEST_CODER
#endif

typedef struct ASN1INTERNencoding_s *ASN1INTERNencoding_t;
typedef struct ASN1INTERNdecoding_s *ASN1INTERNdecoding_t;

// lonchanc: this is really bad to duplicate the definitions of ASN1encoding_s
// and ASN1decoding_s here. We sould simply use them as components in
// the following ASN1INTERNencoding_s and ASN1INTERNdecoding_s.

struct ASN1INTERNencoding_s {

    // public view of encoding structure
    struct ASN1encoding_s       info;

    // private portion of encoding structure
    ASN1INTERNencoding_t        parent;
    ASN1INTERNencoding_t        child;

#ifdef ENABLE_EXTRA_INFO
    ASN1uint32_t				memlength;
    ASN1uint32_t				memsize;
    void**					    mem;
    ASN1uint32_t				epilength;
    ASN1uint32_t				episize;
    ASN1embeddedpdv_identification_t**		epi;
    ASN1uint32_t				csilength;
    ASN1uint32_t				csisize;
    ASN1characterstring_identification_t**	csi;
#endif // ENABLE_EXTRA_INFO
};

struct ASN1INTERNdecoding_s {

    // public view of decoding structure
    struct ASN1decoding_s       info;

    // private portion of decoding structure
    ASN1INTERNdecoding_t        parent;
    ASN1INTERNdecoding_t        child;

#ifdef ENABLE_EXTRA_INFO
    ASN1uint32_t				memlength;
    ASN1uint32_t				memsize;
    void**					    mem;
    ASN1uint32_t				epilength;
    ASN1uint32_t				episize;
    ASN1embeddedpdv_identification_t**		epi;
    ASN1uint32_t				csilength;
    ASN1uint32_t				csisize;
    ASN1characterstring_identification_t**	csi;
#endif // ENABLE_EXTRA_INFO

    // decoded into an external buffer
    ASN1uint32_t                fExtBuf;
    void*                       lpOrigExtBuf;    // original buffer pointer
    ASN1uint32_t                cbOrigExtBufSize;// original buffer size
    ASN1uint8_t*                lpRemExtBuf;     // remaining buffer pointer
    ASN1uint32_t                cbRemExtBufSize; // remaining buffer size
    ASN1uint32_t                cbLinearBufSize; // linear buffer size to hold the data
};

extern ASN1_PUBLIC void ASN1API ASN1DecAbort(ASN1decoding_t dec);
extern ASN1_PUBLIC void ASN1API ASN1DecDone(ASN1decoding_t dec);
extern ASN1_PUBLIC void ASN1API ASN1EncAbort(ASN1encoding_t enc);
extern ASN1_PUBLIC void ASN1API ASN1EncDone(ASN1encoding_t enc);

#ifdef ENABLE_GENERALIZED_CHAR_STR
int ASN1EncSearchCharacterStringIdentification(ASN1INTERNencoding_t e, ASN1characterstring_identification_t *identification, ASN1uint32_t *index, ASN1uint32_t *flag);
int ASN1DecAddCharacterStringIdentification(ASN1INTERNdecoding_t d, ASN1characterstring_identification_t *identification);
ASN1characterstring_identification_t *ASN1DecGetCharacterStringIdentification(ASN1INTERNdecoding_t d, ASN1uint32_t index);
#endif // ENABLE_GENERALIZED_CHAR_STR

#ifdef ENABLE_EMBEDDED_PDV
int ASN1EncSearchEmbeddedPdvIdentification(ASN1INTERNencoding_t e, ASN1embeddedpdv_identification_t *identification, ASN1uint32_t *index, ASN1uint32_t *flag);
int ASN1DecAddEmbeddedPdvIdentification(ASN1INTERNdecoding_t d, ASN1embeddedpdv_identification_t *identification);
ASN1embeddedpdv_identification_t *ASN1DecGetEmbeddedPdvIdentification(ASN1INTERNdecoding_t d, ASN1uint32_t index);
#endif // ENABLE_EMBEDDED_PDV

int ASN1DecDupObjectIdentifier(ASN1decoding_t dec, ASN1objectidentifier_t *dst, ASN1objectidentifier_t *src);

ASN1uint32_t GetObjectIdentifierCount(ASN1objectidentifier_t val);
ASN1uint32_t CopyObjectIdentifier(ASN1objectidentifier_t dst, ASN1objectidentifier_t src);
ASN1objectidentifier_t DecAllocObjectIdentifier(ASN1decoding_t dec, ASN1uint32_t cObjIds);
void DecFreeObjectIdentifier(ASN1decoding_t dec, ASN1objectidentifier_t p);


/* ------ perencod.c ------ */

int ASN1EncCheck(ASN1encoding_t enc, ASN1uint32_t noctets);
__inline int ASN1PEREncCheck(ASN1encoding_t enc, ASN1uint32_t noctets)
{
    return ASN1EncCheck(enc, noctets);
}
__inline int ASN1BEREncCheck(ASN1encoding_t enc, ASN1uint32_t noctets)
{
    return ASN1EncCheck(enc, noctets);
}

/* ------ bit.c ------ */

int ASN1is32space(ASN1char32_t);
int ASN1str32len(ASN1char32_t *);
int ASN1is16space(ASN1char16_t);
int ASN1str16len(ASN1char16_t *);
int My_lstrlenA(char *p);
int My_lstrlenW(WCHAR *p);
void ASN1bitcpy(ASN1octet_t *dst, ASN1uint32_t dstbit, ASN1octet_t *src, ASN1uint32_t srcbit, ASN1uint32_t nbits);
void ASN1bitclr(ASN1octet_t *dst, ASN1uint32_t dstbit, ASN1uint32_t nbits);
void ASN1bitset(ASN1octet_t *dst, ASN1uint32_t dstbit, ASN1uint32_t nbits);
void ASN1bitput(ASN1octet_t *dst, ASN1uint32_t dstbit, ASN1uint32_t val, ASN1uint32_t nbits);
ASN1uint32_t ASN1bitgetu(ASN1octet_t *src, ASN1uint32_t srcbit, ASN1uint32_t nbits);
ASN1int32_t ASN1bitget(ASN1octet_t *src, ASN1uint32_t srcbit, ASN1uint32_t nbits);
ASN1uint32_t ASN1bitcount(ASN1octet_t *src, ASN1uint32_t srcbit, ASN1uint32_t nbits);
void ASN1octetput(ASN1octet_t *dst, ASN1uint32_t val, ASN1uint32_t noctets);
ASN1uint32_t ASN1octetget(ASN1octet_t *src, ASN1uint32_t noctets);

#define ARRAY_SIZE(arr)             (sizeof(arr) / sizeof(arr[0]))
// #define FIELD_OFFSET(type, field)   ((long)&(((type *)0)->field))   // from winnt.h
#define PARAMS_SIZE_N_ARRARY(arr)   ARRAY_SIZE(arr), arr

#define LPVOID_ADD(ptr,inc)  (LPVOID) ((ASN1octet_t *) (ptr) + (ASN1uint32_t) (inc))
#define LPVOID_SUB(ptr,dec)  (LPVOID) ((ASN1octet_t *) (ptr) - (ASN1uint32_t) (inc))

#define LPVOID_NEXT(ptr)     *(LPVOID FAR *) (ptr)

// the following constants is for calculating decoded data structure size
// we are conservative here and try to be 4-byte aligned due to Alpha platform.

#define ASN1_SIZE_ALIGNED(n)    (n) = ((((n) + 3) >> 2) << 2)

#ifdef ENABLE_BER
int My_memcmp(ASN1octet_t *pBuf1, ASN1uint32_t cbBuf1Size, ASN1octet_t *pBuf2, ASN1uint32_t cbBuf2Size);
#endif // ENABLE_BER

#define UNKNOWN_MODULE                  0

#define MemAllocEx(dec,cb,fZero)        (dec)->pScratchMemoryAllocator->Alloc((cb))
#define MemFree(lp)
#define MemReAllocEx(dec,lp,cb,fZero)   ((lp) ? \
            (dec)->pScratchMemoryAllocator->ReAlloc((lp),(cb)) : \
            (dec)->pScratchMemoryAllocator->Alloc((cb)))

#define _ModName(enc_dec)                   

LPVOID DecMemAlloc   ( ASN1decoding_t dec, ASN1uint32_t cbSize );
LPVOID DecMemReAlloc ( ASN1decoding_t dec, LPVOID lpData, ASN1uint32_t cbSize );

#define EncMemAlloc(enc,cb)             (enc)->pScratchMemoryAllocator->Alloc((cb))
#define EncMemReAlloc(enc,lp,cb)        (enc)->pScratchMemoryAllocator->ReAlloc((lp),(cb))
#define EncMemImmediateReAlloc(dec,lp,ocb,cb)   (dec)->pScratchMemoryAllocator->ImmediateReAlloc((lp),(ocb),(cb))

void   DecMemFree    ( ASN1decoding_t dec, LPVOID lpData );

#define EncMemFree(enc,lpData)              MemFree(lpData)

int IsDigit(char p);
unsigned int  DecimalStringToUINT(char * pcszString, ASN1uint32_t cch);
void * ms_bSearch (
        const void *key,
        const void *base,
        size_t num,
        size_t width,
        int (__cdecl *compare)(const void *, const void *)
        );

#define MyAssert(f)         
#ifdef _DEBUG
    void MyDebugBreak(void);
    __inline void EncAssert(ASN1encoding_t enc, int val)
    {
        if ((! (enc->dwFlags & ASN1FLAGS_NOASSERT)) && (! (val)))
        {
            MyDebugBreak();
        }
    }
    __inline void DecAssert(ASN1decoding_t dec, int val)
    {
        if ((! (dec->dwFlags & ASN1FLAGS_NOASSERT)) && (! (val)))
        {
            MyDebugBreak();
        }
    }
#else
    #define EncAssert(enc,f)   
    #define DecAssert(dec,f)   
#endif // _DEBUG

// making a magic number
#define MAKE_STAMP_ID(a,b,c,d)     MAKELONG(MAKEWORD(a,b),MAKEWORD(c,d))

/* magic number for ASN1encoding_t */
#define MAGIC_ENCODER       MAKE_STAMP_ID('E','N','C','D')

/* magic number for ASN1decoding_t */
#define MAGIC_DECODER       MAKE_STAMP_ID('D','E','C','D')


#ifdef TEST_CODER
typedef struct ASN1testcoder_s
{
    struct ASN1INTERNencoding_s     e;
    struct ASN1INTERNdecoding_s     d;
}   *ASN1testcoder_t;
#define ASN1_TEST_CODER_SIZE    (sizeof(struct ASN1testcoder_s))
int TestEnc_InitCoder(ASN1INTERNencoding_t e, ASN1module_t mod);
int TestDec_InitCoder(ASN1INTERNdecoding_t d, ASN1module_t mod);
int TestEnc_Compare(ASN1INTERNencoding_t e, ASN1uint32_t id, ASN1octet_t *pbBuf, ASN1uint32_t cbBufSize);
int TestDec_Compare(ASN1INTERNdecoding_t d, ASN1uint32_t id, void *valref, ASN1octet_t *pbBuf, ASN1uint32_t cbBufSize);
#else
#define ASN1_TEST_CODER_SIZE    0
#endif

#define ENCODE_BUFFER_INCREMENT         128
#define ENCODE_BUFFER_MAX_INCREMENT     256

static const ASN1uint8_t c_aBitMask2[] =
{
    0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
};
// moved from perdecod.c
/* check if sufficient data is in decoding buffer */
int ASN1PERDecCheck(ASN1decoding_t dec, ASN1uint32_t nbits)
{
    if ((dec->pos - dec->buf) * 8 + dec->bit + nbits <= dec->size * 8)
    {
        return 1;
    }
    ASN1DecSetError(dec, ASN1_ERR_EOD);
    return 0;
}
/* skip up to octet boundary */
void ASN1PERDecAlignment(ASN1decoding_t dec)
{
    if (dec->bit)
    {
        dec->bit = 0;
        dec->pos++;
    }
}

// moved from perdecod.c
/* end of decoding */
int ASN1PERDecFlush(ASN1decoding_t dec)
{
    /* complete broken byte */
    ASN1PERDecAlignment(dec);

    /* get zero-octet if encoding is empty bitstring */
    if (dec->buf == dec->pos)
    {
        if (ASN1PERDecCheck(dec, 8))
        {
            if (*dec->pos == 0)
            {
                dec->pos++;
            }
            else
            {
                ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }

    /* calculate length */
    dec->len = (ASN1uint32_t) (dec->pos - dec->buf);

    /* set WRN_NOEOD if data left */
    if (dec->len >= dec->size)
    {
        return 1;
    }
    ASN1DecSetError(dec, ASN1_WRN_NOEOD);
    return 1;
}
// moved from perencod.c
/* end of encoding */
int ASN1PEREncFlush(ASN1encoding_t enc)
{
    /* complete broken octet */
    ASN1PEREncAlignment(enc);

    /* allocate at least one octet */
    if (enc->buf)
    {
        /* fill in zero-octet if encoding is empty bitstring */
        if (enc->buf == enc->pos)
            *enc->pos++ = 0;

        /* calculate length */
        enc->len = (ASN1uint32_t) (enc->pos - enc->buf);

        return 1;
    }

    return ASN1PEREncCheck(enc, 1);
}

// moved from perencod.c
/* encode an octet alignment */
void ASN1PEREncAlignment(ASN1encoding_t enc)
{
    /* complete broken octet */
    if (enc->bit)
    {
        enc->pos++;
        enc->bit = 0;
    }
}

// moved from perencod.c
/* check for space in buffer for PER and BER. */
int ASN1EncCheck(ASN1encoding_t enc, ASN1uint32_t noctets)
{
    // any additional space required?
    if (noctets)
    {
        // buffer exists?
        if (NULL != enc->buf)
        {
            // buffer large enough?
            if (enc->size - (enc->pos - enc->buf) - ((enc->bit != 0) ? 1 : 0) >= noctets)
            {
                return 1;
            }

            // static buffer overflow?
            if (enc->dwFlags & ASN1ENCODE_SETBUFFER)
            {
                ASN1EncSetError(enc, ASN1_ERR_OVERFLOW);
                return 0;
            }
            else
            {
                // round up to next 256 byte boundary and resize buffer
                ASN1octet_t *oldbuf = enc->buf;
                ASN1uint32_t oldsize = enc->size;
                // enc->size = ((noctets + (enc->pos - oldbuf) + (enc->bit != 0) - 1) | 255) + 1;
                if (ASN1_PER_RULE & enc->eRule)
                {
                    enc->size += max(noctets, ENCODE_BUFFER_INCREMENT);
                }
                else
                {
                    //enc->size += max(noctets, enc->size);
                    enc->size += min(max(noctets, enc->size), ENCODE_BUFFER_MAX_INCREMENT);
                }
                enc->buf = (ASN1octet_t *)EncMemImmediateReAlloc(enc, enc->buf, oldsize, enc->size);
                if (NULL != enc->buf)
                {
                    enc->pos = enc->buf + (enc->pos - oldbuf);
                }
                else
                {
                    ASN1EncSetError(enc, ASN1_ERR_MEMORY);
                    return 0;
                }
            }
        }
        else
        {
            // no buffer exists, allocate new one.
            // round up to next 256 byte boundary and allocate buffer
            // enc->size = ((noctets - 1) | 255) + 1;
            enc->size = max(noctets + enc->cbExtraHeader, ENCODE_BUFFER_INCREMENT);
            enc->buf = (ASN1octet_t*) EncMemAlloc(enc, enc->size);
            if (NULL != enc->buf)
            {
                enc->pos = (ASN1octet_t *) (enc->buf + enc->cbExtraHeader);
            }
            else
            {
                enc->pos = NULL;
                ASN1EncSetError(enc, ASN1_ERR_MEMORY);
                return 0;
            }
        }
    }

    return 1;
}

// moved from perfn.c
int ASN1PEREncRemoveZeroBits(ASN1uint32_t *nbits, ASN1octet_t *val, ASN1uint32_t minlen)
{
    ASN1uint32_t n;
    int i;

    /* get value */
    n = *nbits;

    /* nothing to scan? */
    if (n > minlen)
    {
        /* let val point to last ASN1octet used */
        val += (n - 1) / 8;

        /* check if broken ASN1octet consist out of zero bits */
        if ((n & 7) && !(*val & c_aBitMask2[n & 7])) {
            n &= ~7;
            val--;
        }

        /* scan complete ASN1octets (memrchr missing ...) */
        if (!(n & 7)) {
            while (n > minlen && !*val) {
                n -= 8;
                val--;
            }
        }

        /* scan current octet bit after bit */
        if (n > minlen) {
            for (i = (n - 1) & 7; i >= 0; i--) {
                if (*val & (0x80 >> i))
                    break;
                n--;
            }
        }

        /* return real bitstring len */
        *nbits = n < minlen ? minlen : n;
    }
    return 1;
}

/* init an ASN1encoding_t */
ASN1error_e ASN1_CreateEncoder
(
    CScratchMemoryAllocator* pScratchMemoryAllocator,
    ASN1module_t        mod,
    ASN1encoding_t     *enc,
    ASN1octet_t        *pbBuf,
    ASN1uint32_t        cbBufSize,
    ASN1encoding_t      pParent
)
{
    if (NULL != mod && NULL != enc)
    {
        ASN1INTERNencoding_t    e;

        *enc = NULL;

        /* construct ASN1encoding_t */
        e = (ASN1INTERNencoding_t)pScratchMemoryAllocator->Alloc(sizeof(*e) + ASN1_TEST_CODER_SIZE);
        if (NULL != e)
        {
            RtlZeroMemory(e, sizeof(*e) + ASN1_TEST_CODER_SIZE);
            e->info.pScratchMemoryAllocator = pScratchMemoryAllocator;
            e->info.magic = MAGIC_ENCODER;
            e->info.err = ASN1_SUCCESS;
            // e->info.pos = e->info.buf = NULL;
            // e->info.size = e->info.len = e->info.bit = 0;
            e->info.dwFlags = mod->dwFlags;
            e->info.module = mod;
            // e->child = NULL;

            /* set buffer if given */
            if (NULL != pbBuf && cbBufSize)
            {
                e->info.dwFlags |= ASN1ENCODE_SETBUFFER;
                e->info.pos = e->info.buf = pbBuf;
                e->info.size = cbBufSize;
            }

            /* set parent if parented, otherwise, initialized to itself */
            if (NULL != pParent)
            {
                e->parent = (ASN1INTERNencoding_t) pParent;
                e->info.eRule = pParent->eRule;
            }
            else
            {
                e->parent = e;
                e->info.eRule = mod->eRule;
            }

            // e->mem = NULL;
            // e->memlength = 0;
            // e->memsize = 0;
            // e->epi = NULL;
            // e->epilength = 0;
            // e->episize = 0;
            // e->csi = NULL;
            // e->csilength = 0;
            // e->csisize = 0;

            if (! (e->info.dwFlags & ASN1ENCODE_SETBUFFER) && (NULL != pParent))
            {
                // lonchanc: make sure we have a minimum 256 bytes available.
                BOOL fRet = FALSE;
                if (ASN1_PER_RULE & e->info.eRule)
                {
                    // this is required for h245.
                    fRet = ASN1PEREncCheck((ASN1encoding_t) e, 1);
                }
#ifdef ENABLE_BER
                else
                if (ASN1_BER_RULE & e->info.eRule)
                {
                    // this is required for h245.
                    fRet = ASN1BEREncCheck((ASN1encoding_t) e, 1);
                }
#endif // ENABLE_BER
                else
                {
                    EncAssert((ASN1encoding_t) e, 0);
                    MemFree(e);
                    return ASN1_ERR_RULE;
                }
                if (fRet)
                {
                    // lonchanc: make sure the first byte is zeroed out, which
                    // is required for h245.
                    e->info.buf[0] = '\0';
                }
                else
                {
                    MemFree(e);
                    return ASN1_ERR_MEMORY;
                }
            }

#if defined(TEST_CODER) && defined(_DEBUG)
            TestEnc_InitCoder(e, mod);
#endif // defined(TEST_CODER) && defined(_DEBUG)

            if (NULL != pParent)
            {
                EncAssert((ASN1encoding_t) e, NULL == ((ASN1INTERNencoding_t) pParent)->child);
                ((ASN1INTERNencoding_t) pParent)->child = e;
            }

            *enc = (ASN1encoding_t) e;
            return ASN1_SUCCESS;
        }
        else
        {
            return ASN1_ERR_MEMORY;
        }
    }

    return ASN1_ERR_BADARGS;
}

/* encode a value */
ASN1error_e ASN1_Encode
(
    ASN1encoding_t      enc,
    void               *value,
    ASN1uint32_t        id,
    ASN1uint32_t        flags,
    ASN1octet_t        *pbBuf,
    ASN1uint32_t        cbBufSize
)
{
    if (NULL != enc)
    {
        ASN1INTERNencoding_t    e = (ASN1INTERNencoding_t)enc;

        /* check magic numbers */
        EncAssert(enc, MAGIC_ENCODER == enc->magic);

        /* clear error */
        ASN1EncSetError(enc, ASN1_SUCCESS);

        /* new buffer given? */
        if (flags & ASN1ENCODE_SETBUFFER)
        {
            e->info.dwFlags |= ASN1ENCODE_SETBUFFER;
            enc->pos = enc->buf = pbBuf;
            enc->size = cbBufSize;
            enc->len = enc->bit = 0;
        }
        /* use a new buffer? */
        else if ((e->info.dwFlags | flags) & ASN1ENCODE_ALLOCATEBUFFER)
        {
            e->info.dwFlags &= ~ASN1ENCODE_SETBUFFER;
            enc->pos = enc->buf = NULL;
            enc->size = enc->len = enc->bit = 0;
        }
        /* reuse buffer? */
        else if ((flags & ASN1ENCODE_REUSEBUFFER) || !((e->info.dwFlags | flags) & ASN1ENCODE_APPEND))
        {
            EncAssert(enc, NULL != enc->buf);
            enc->pos = enc->buf;
            enc->bit = enc->len = 0;
        }
        /* otherwise append to buffer */

        /* check id number */
        if (id < enc->module->cPDUs)
        {
            if (ASN1_PER_RULE & enc->eRule)
            {
                /* encode value */
                ASN1PerEncFun_t pfnPER;
                if (NULL != (pfnPER = enc->module->PER.apfnEncoder[id]))
                {
                    if ((*pfnPER)(enc, value))
                    {
                        ASN1PEREncFlush(enc);
                    }
                    else
                    {
                        // the error code must be an error
                        if (ASN1_SUCCEEDED(e->parent->info.err))
                        {
                            // cannot return here immediately because we need to do cleanup
                            ASN1EncSetError(enc, ASN1_ERR_CORRUPT);
                        }
                    }
                }
                else
                {
                    return ASN1EncSetError(enc, ASN1_ERR_BADPDU);
                }
            }
#ifdef ENABLE_BER
            else
            if (ASN1_BER_RULE & enc->eRule)
            {
                /* encode value */
                ASN1BerEncFun_t pfnBER;
                if (NULL != (pfnBER = enc->module->BER.apfnEncoder[id]))
                {
                    if ((*pfnBER)(enc, 0, value)) // lonchanc: tag is 0 to make it compiled
                    {
                        ASN1BEREncFlush(enc);
                    }
                    else
                    {
                        // the error code must be an error
                        if (ASN1_SUCCEEDED(e->parent->info.err))
                        {
                            // cannot return here immediately because we need to do cleanup
                            ASN1EncSetError(enc, ASN1_ERR_CORRUPT);
                        }
                    }
                }
                else
                {
                    return ASN1EncSetError(enc, ASN1_ERR_BADPDU);
                }
            }
#endif // ENABLE_BER
            else
            {
                return ASN1EncSetError(enc, ASN1_ERR_RULE);
            }

            /* call abort/done function for non-parented encoding stream */
            if (ASN1_SUCCEEDED(e->parent->info.err))
            {
                // not parented
                if (e == e->parent)
                {
#if defined(TEST_CODER) && defined(_DEBUG)
                    if (ASN1_PER_RULE & enc->eRule)
                    {
                        if (! TestEnc_Compare(e, id, enc->buf + enc->cbExtraHeader, enc->len - enc->cbExtraHeader))
                        {
                            MyDebugBreak();
                        }
                    }
#ifdef ENABLE_BER
                    else
                    if (ASN1_BER_RULE & enc->eRule)
                    {
                        if (! TestEnc_Compare(e, id, enc->buf + enc->cbExtraHeader, enc->len - enc->cbExtraHeader))
                        {
                            MyDebugBreak();
                        }
                    }
#endif // ENABLE_BER
#endif
                    ASN1EncDone(enc);
                }
            }
            else
            {
                ASN1INTERNencoding_t child, child2;

                // not parented
                if (e == e->parent)
                {
                    ASN1EncAbort(enc);
                }

                // clean up...
                if ((e->info.dwFlags | flags) & ASN1ENCODE_ALLOCATEBUFFER)
                {
                    ASN1_FreeEncoded(enc, enc->buf);
                    enc->pos = enc->buf = NULL;
                    enc->size = enc->len = enc->bit = 0;
                }
                for (child = e->child; child; child = child2)
                {
                    child2 = child->child;
                    // make sure it does not touch its parent which may already be freed
                    child->parent = child;
                    ASN1_CloseEncoder2((ASN1encoding_t) child);
                }
                e->child = NULL;
            }

            /* return error code */
            return e->parent->info.err;
        }
        else
        {
            return ASN1EncSetError(enc, ASN1_ERR_BADPDU);
        }
    }

    return ASN1_ERR_BADARGS;
}

/* control function for encoding */
ASN1error_e ASN1_SetEncoderOption
(
    ASN1encoding_t      enc,
    ASN1optionparam_t  *pOptParam
)
{
    if (NULL != enc && NULL != pOptParam)
    {
        ASN1INTERNencoding_t e = (ASN1INTERNencoding_t)enc;
        ASN1error_e rc = ASN1_SUCCESS;

        /* check magic number */
        EncAssert(enc, MAGIC_ENCODER == enc->magic);

        switch (pOptParam->eOption)
        {
        case ASN1OPT_CHANGE_RULE:
            enc->eRule = pOptParam->eRule;
            break;

        case ASN1OPT_NOT_REUSE_BUFFER:
            e->info.dwFlags &= ~ASN1ENCODE_SETBUFFER;
            enc->buf = enc->pos = NULL;
            enc->size = enc->bit = enc->len = 0;
            break;

        case ASN1OPT_REWIND_BUFFER:
            enc->pos = enc->buf;
            enc->bit = enc->len = 0;
            break;

        default:
            rc = ASN1_ERR_BADARGS;
            break;
        }

        return ASN1EncSetError(enc, rc);
    }

    return ASN1_ERR_BADARGS;
}

ASN1error_e ASN1_GetEncoderOption
(
    ASN1encoding_t      enc,
    ASN1optionparam_t  *pOptParam
)
{
    if (NULL != enc && NULL != pOptParam)
    {
        ASN1INTERNencoding_t e = (ASN1INTERNencoding_t)enc;
        ASN1error_e rc = ASN1_SUCCESS;

        /* check magic number */
        EncAssert(enc, MAGIC_ENCODER == enc->magic);

        switch (pOptParam->eOption)
        {
        case ASN1OPT_GET_RULE:
            pOptParam->eRule = enc->eRule;
            break;

        default:
            rc = ASN1_ERR_BADARGS;
            break;
        }

        return ASN1EncSetError(enc, rc);
    }

    return ASN1_ERR_BADARGS;
}


/* destroy encoding stream */
void ASN1_CloseEncoder
(
    ASN1encoding_t      enc
)
{
    if (NULL != enc)
    {
        ASN1INTERNencoding_t e = (ASN1INTERNencoding_t)enc;

        /* check magic number */
        EncAssert(enc, MAGIC_ENCODER == enc->magic);

        if (e != e->parent)
        {
            EncAssert(enc, e == e->parent->child);
            e->parent->child = NULL;
        }

        /* free encoding stream */
        MemFree(e);
    }
}

/* destroy encoding stream */
void ASN1_CloseEncoder2
(
    ASN1encoding_t      enc
)
{
    if (NULL != enc)
    {
        /* check magic number */
        EncAssert(enc, MAGIC_ENCODER == enc->magic);

        EncMemFree(enc, enc->buf);

        ASN1_CloseEncoder(enc);
    }
}

/* init an ASN1decoding_t */
ASN1error_e ASN1_CreateDecoder
(
    CScratchMemoryAllocator* pScratchMemoryAllocator,
    ASN1module_t        mod,
    ASN1decoding_t     *dec,
    ASN1octet_t        *pbBuf,
    ASN1uint32_t        cbBufSize,
    ASN1decoding_t      pParent
)
{
    if (NULL != mod && NULL != dec)
    {
        ASN1INTERNdecoding_t d;

        *dec = NULL;

        /* construct ASN1decoding_t */
        d = (ASN1INTERNdecoding_t)pScratchMemoryAllocator->Alloc(sizeof(*d) + ASN1_TEST_CODER_SIZE);
        if (NULL != d)
        {
            RtlZeroMemory(d, sizeof(*d) + ASN1_TEST_CODER_SIZE);
            d->info.pScratchMemoryAllocator = pScratchMemoryAllocator;
            d->info.magic = MAGIC_DECODER;
            d->info.err = ASN1_SUCCESS;
            d->info.dwFlags = mod->dwFlags;
            d->info.module = mod;
            // d->child = NULL;

            /* set buffer if given */
            // lonchanc: it is ok to have a zero buffer size here
            if (NULL != pbBuf)
            {
                d->info.dwFlags |= ASN1DECODE_SETBUFFER;
                d->info.buf = d->info.pos = pbBuf;
                d->info.size = cbBufSize;
                // d->info.len = d->info.bit = 0;
            }
            else
            {
                // d->info.buf = d->info.pos = NULL;
                // d->info.size = d->info.len = d->info.bit = 0;
            }

            /* set parent if parented */
            if (NULL != pParent)
            {
                d->parent = (ASN1INTERNdecoding_t) pParent;
                d->info.eRule = pParent->eRule;
            }
            else
            /* initialize otherwise */
            {
                d->parent = d;
                d->info.eRule = mod->eRule;
            }

            // d->mem = NULL;
            // d->memlength = 0;
            // d->memsize = 0;
            // d->epi = NULL;
            // d->epilength = 0;
            // d->episize = 0;
            // d->csi = NULL;
            // d->csilength = 0;
            // d->csisize = 0;

#if defined(TEST_CODER) && defined(_DEBUG)
            TestDec_InitCoder(d, mod);
#endif // defined(TEST_CODER) && defined(_DEBUG)

            if (NULL != pParent)
            {
                DecAssert((ASN1decoding_t) d, NULL == ((ASN1INTERNdecoding_t) pParent)->child);
                ((ASN1INTERNdecoding_t) pParent)->child = d;
            }

            *dec = (ASN1decoding_t) d;
            return ASN1_SUCCESS;
        }
        else
        {
            return ASN1_ERR_MEMORY;
        }
    }

    return ASN1_ERR_BADARGS;
}

/* decode a value */
ASN1error_e ASN1_Decode
(
    ASN1decoding_t      dec,
    void              **valref,
    ASN1uint32_t        id,
    ASN1uint32_t        flags,
    ASN1octet_t        *pbBuf,
    ASN1uint32_t        cbBufSize
)
{
    if (NULL != dec && NULL != valref)
    {
        ASN1INTERNdecoding_t    d = (ASN1INTERNdecoding_t)dec;

        /* check magic numbers */
        DecAssert(dec, MAGIC_DECODER == dec->magic);

        /* clear error */
        ASN1DecSetError(dec, ASN1_SUCCESS);

        /* new buffer given? */
        if (flags & ASN1DECODE_SETBUFFER)
        {
            if (NULL != pbBuf && 0 != cbBufSize)
            {
                dec->pos = dec->buf = pbBuf;
                dec->size = cbBufSize;
                dec->bit = dec->len = 0;
            }
            else
            {
                return ASN1DecSetError(dec, ASN1_ERR_BADARGS);
            }
        }
        /* rewind buffer? */
        else if ((flags & ASN1DECODE_REWINDBUFFER) ||
                 !((d->info.dwFlags | flags ) & ASN1DECODE_APPENDED))
        {
            dec->pos = dec->buf;
            dec->bit = dec->len = 0;
        }
        /* otherwise continue reading from last buffer */

        /* check id number */
        if (id < dec->module->cPDUs)
        {
            ASN1uint32_t cbTopLevelStruct;

            /* clear length of linear buffer required */
            d->cbLinearBufSize = 0;

            /* double check for the availability of destination buffer */
            if (d->lpOrigExtBuf == NULL || d->cbOrigExtBufSize == 0)
            {
                d->fExtBuf = FALSE;
            }

            cbTopLevelStruct = dec->module->acbStructSize[id];
            if (NULL != (*valref = DecMemAlloc(dec, cbTopLevelStruct)))
            {
                if (ASN1_PER_RULE & dec->eRule)
                {
                    ASN1PerDecFun_t pfnPER;
                    /* decode value */
                    if (NULL != (pfnPER = dec->module->PER.apfnDecoder[id]))
                    {
                        if ((*pfnPER)(dec, *valref))
                        {
                            ASN1PERDecFlush(dec);
                        }
                        else
                        {
                            // the error code must be an error
                            if (ASN1_SUCCEEDED(d->parent->info.err))
                            {
                                // cannot return here immediately because we need to do cleanup
                                ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
                            }
                        }
                    }
                    else
                    {
                        return ASN1DecSetError(dec, ASN1_ERR_BADPDU);
                    }
                }
#ifdef ENABLE_BER
                else
                if (ASN1_BER_RULE & dec->eRule)
                {
                    ASN1BerDecFun_t pfnBER;
                    /* decode value */
                    if (NULL != (pfnBER = dec->module->BER.apfnDecoder[id]))
                    {
                        if ((*pfnBER)(dec, 0, *valref)) // lonchanc: tag is 0 to make it compiled
                        {
                            ASN1BERDecFlush(dec);
                        }
                        else
                        {
                            // the error code must be an error
                            if (ASN1_SUCCEEDED(d->parent->info.err))
                            {
                                // cannot return here immediately because we need to do cleanup
                                ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
                            }
                        }
                    }
                    else
                    {
                        return ASN1DecSetError(dec, ASN1_ERR_BADPDU);
                    }
                }
#endif // ENABLE_BER
                else
                {
                    return ASN1DecSetError(dec, ASN1_ERR_RULE);
                }

                /* call abort/done function for non-parented decoding stream */
                if (ASN1_SUCCEEDED(d->parent->info.err))
                {
                    // not parented
                    if (d == d->parent)
                    {
#if defined(TEST_CODER) && defined(_DEBUG)
                        if (ASN1_PER_RULE & dec->eRule)
                        {
                            if (! TestDec_Compare(d, id, *valref, dec->buf, dec->len))
                            {
                                MyDebugBreak();
                            }
                        }
#ifdef ENABLE_BER
                        else
                        if (ASN1_BER_RULE & dec->eRule)
                        {
                            if (! TestDec_Compare(d, id, *valref, dec->buf, dec->len))
                            {
                                MyDebugBreak();
                            }
                        }
#endif // ENABLE_BER
#endif
                        ASN1DecDone(dec);
                    }
                }
                else
                {
                    ASN1INTERNdecoding_t child, child2;

                    // not parented
                    if (d == d->parent)
                    {
                        ASN1DecAbort(dec);
                    }

                    // clean up...
                    ASN1_FreeDecoded(dec ,*valref, id);
                    *valref = NULL;
                    for (child = d->child; child; child = child2)
                    {
                        child2 = child->child;
                        // make sure it does not touch its parent which may already be freed
                        child->parent = child;
                        ASN1_CloseDecoder((ASN1decoding_t) child);
                    }
                    d->child = NULL;
                }

                /* return error code */
                return d->parent->info.err;
            }
            else
            {
                return ASN1_ERR_MEMORY;
            }
        }
        else
        {
            return ASN1DecSetError(dec, ASN1_ERR_BADPDU);
        }
    }

    return ASN1_ERR_BADARGS;
}

/* control function for decoding */
ASN1error_e ASN1_SetDecoderOption
(
    ASN1decoding_t      dec,
    ASN1optionparam_t  *pOptParam
)
{
    if (NULL != dec)
    {
        ASN1INTERNdecoding_t d = (ASN1INTERNdecoding_t)dec;
        ASN1error_e rc = ASN1_SUCCESS;

        /* check magic number */
        DecAssert(dec, MAGIC_DECODER == dec->magic);

        switch (pOptParam->eOption)
        {
        case ASN1OPT_CHANGE_RULE:
            dec->eRule = pOptParam->eRule;
            break;

        case ASN1OPT_SET_DECODED_BUFFER:
            if (NULL != pOptParam->Buffer.pbBuf && 0 != pOptParam->Buffer.cbBufSize)
            {
                d->fExtBuf = TRUE;
                d->lpOrigExtBuf = pOptParam->Buffer.pbBuf;
                d->cbOrigExtBufSize = pOptParam->Buffer.cbBufSize;
                d->lpRemExtBuf = (ASN1uint8_t*) d->lpOrigExtBuf;
                d->cbRemExtBufSize = d->cbOrigExtBufSize;
            }
            else
            {
                rc = ASN1_ERR_BADARGS;
            }
            break;

        case ASN1OPT_DEL_DECODED_BUFFER:
            d->fExtBuf = FALSE;
            d->lpOrigExtBuf = NULL;
            d->cbOrigExtBufSize = 0;
            d->lpRemExtBuf = NULL;
            d->cbRemExtBufSize = 0;
            break;

        default:
            rc = ASN1_ERR_BADARGS;
            break;
        }

        return ASN1DecSetError(dec, rc);
    }

    return ASN1_ERR_BADARGS;
}


/* control function for decoding */
ASN1error_e ASN1_GetDecoderOption
(
    ASN1decoding_t      dec,
    ASN1optionparam_t  *pOptParam
)
{
    if (NULL != dec)
    {
        ASN1INTERNdecoding_t d = (ASN1INTERNdecoding_t)dec;
        ASN1error_e rc = ASN1_SUCCESS;

        /* check magic number */
        DecAssert(dec, MAGIC_DECODER == dec->magic);

        switch (pOptParam->eOption)
        {
        case ASN1OPT_GET_RULE:
            pOptParam->eRule = dec->eRule;
            break;

        case ASN1OPT_GET_DECODED_BUFFER_SIZE:
            pOptParam->cbRequiredDecodedBufSize = d->cbLinearBufSize;
            break;

        default:
            rc = ASN1_ERR_BADARGS;
            break;
        }

        return ASN1DecSetError(dec, rc);
    }

    return ASN1_ERR_BADARGS;
}


/* destroy decoding stream */
void ASN1_CloseDecoder
(
    ASN1decoding_t      dec
)
{
    if (NULL != dec)
    {
        ASN1INTERNdecoding_t d = (ASN1INTERNdecoding_t)dec;

        /* check magic number */
        DecAssert(dec, MAGIC_DECODER == dec->magic);

        if (d != d->parent)
        {
            DecAssert(dec, d == d->parent->child);
            d->parent->child = NULL;
        }

        /* free decoding stream */
        MemFree(d);
    }
}

/* free an encoded value */
void ASN1_FreeEncoded
(
    ASN1encoding_t      enc,
    void               *val
)
{
    if (NULL != enc)
    {
        /* check magic number */
        EncAssert(enc, MAGIC_ENCODER == enc->magic);

        EncMemFree(enc, val);
    }
}

/* free a unencoded value */
void ASN1_FreeDecoded
(
    ASN1decoding_t      dec,
    void               *val,
    ASN1uint32_t        id
)
{
    if (NULL != dec)
    {
        ASN1INTERNdecoding_t d = (ASN1INTERNdecoding_t)dec;

        /* check magic number */
        DecAssert(dec, MAGIC_DECODER == dec->magic);

        // same behavior of LocalFree
        if (val != NULL)
        {
            if (id != ASN1DECFREE_NON_PDU_ID)
            {
                ASN1FreeFun_t       pfnFreeMemory;

                /* free value */
                if (id < dec->module->cPDUs)
                {
                    if (NULL != (pfnFreeMemory = dec->module->apfnFreeMemory[id]))
                    {
                        (*pfnFreeMemory)(val);
                    }
                }
                else
                {
                    return;
                }
            }

            // free the top-level structure
            MemFree(val);
        }
    }
}

void ASN1_InitModule
(
    ASN1module_t            module,
    ASN1uint32_t            version,
    ASN1encodingrule_e      eEncodingRule,
    ASN1uint32_t            dwFlags,
    ASN1uint32_t            cPDUs,
    const ASN1GenericFun_t  apfnEncoder[],
    const ASN1GenericFun_t  apfnDecoder[],
    const ASN1FreeFun_t     apfnFreeMemory[],
    const ASN1uint32_t      acbStructSize[],
    ASN1magic_t             nModuleName
)
{
    module->nModuleName = nModuleName;
    module->eRule = eEncodingRule;
    module->dwFlags = dwFlags;
    module->cPDUs = cPDUs;

    module->apfnFreeMemory = apfnFreeMemory;
    module->acbStructSize = acbStructSize;

    if (ASN1_PER_RULE & eEncodingRule)
    {
        module->PER.apfnEncoder = (const ASN1PerEncFun_t *) apfnEncoder;
        module->PER.apfnDecoder = (const ASN1PerDecFun_t *) apfnDecoder;
    }
#ifdef ENABLE_BER
    else
    if (ASN1_BER_RULE & eEncodingRule)
    {
        module->BER.apfnEncoder = (const ASN1BerEncFun_t *) apfnEncoder;
        module->BER.apfnDecoder = (const ASN1BerDecFun_t *) apfnDecoder;
    }
#endif // ENABLE_BER
}

#ifdef TEST_CODER

static int MyMemCmp(ASN1octet_t *p1, ASN1octet_t *p2, ASN1uint32_t c)
{
    BYTE diff;
    while (c--)
    {
        if ((diff = *p1++ - *p2++) != 0)
            return (int) diff;
    }
    return 0;
}

__inline ASN1INTERNencoding_t TestEnc_GetEnc(ASN1INTERNencoding_t e)
    { return &(((ASN1testcoder_t) (e+1))->e); }
__inline ASN1INTERNdecoding_t TestEnc_GetDec(ASN1INTERNencoding_t e)
    { return &(((ASN1testcoder_t) (e+1))->d); }
__inline ASN1INTERNencoding_t TestDec_GetEnc(ASN1INTERNdecoding_t d)
    { return &(((ASN1testcoder_t) (d+1))->e); }
__inline ASN1INTERNdecoding_t TestDec_GetDec(ASN1INTERNdecoding_t d)
    { return &(((ASN1testcoder_t) (d+1))->d); }

static void Test_InitEnc(ASN1INTERNencoding_t e, ASN1module_t mod, ASN1encodingrule_e eRule)
{
    RtlZeroMemory(e, sizeof(*e));
    e->info.magic = MAGIC_ENCODER;
    e->info.err = ASN1_SUCCESS;
    e->info.module = mod;
    e->info.eRule = eRule;
    e->parent = e;
    e->child = NULL;
}

static void Test_InitDec(ASN1INTERNdecoding_t d, ASN1module_t mod, ASN1encodingrule_e eRule)
{
    RtlZeroMemory(d, sizeof(*d));
    d->info.magic = MAGIC_DECODER;
    d->info.err = ASN1_SUCCESS;
    d->info.module = mod;
    d->info.eRule = eRule;
    d->parent = d;
    d->child = NULL;
}

static int TestEnc_InitCoder(ASN1INTERNencoding_t e, ASN1module_t mod)
{
    ASN1INTERNencoding_t ee = TestEnc_GetEnc(e);
    ASN1INTERNdecoding_t ed = TestEnc_GetDec(e);
    Test_InitEnc(ee, mod, e->info.eRule);
    Test_InitDec(ed, mod, e->info.eRule);
    return 1;
}

static int TestDec_InitCoder(ASN1INTERNdecoding_t d, ASN1module_t mod)
{
    ASN1INTERNencoding_t de = TestDec_GetEnc(d);
    ASN1INTERNdecoding_t dd = TestDec_GetDec(d);
    Test_InitEnc(de, mod, d->info.eRule);
    Test_InitDec(dd, mod, d->info.eRule);
    return 1;
}

static int Test_Encode(ASN1INTERNencoding_t e, void *value, ASN1uint32_t id)
{
    ASN1encoding_t enc = (ASN1encoding_t) e;

    /* clear error */
    ASN1EncSetError(enc, ASN1_SUCCESS);

    // clean buffer
    enc->pos = enc->buf;
    enc->bit = enc->len = 0;

    if (ASN1_PER_RULE & enc->eRule)
    {
        /* encode value */
        ASN1PerEncFun_t pfnPER;
        if (NULL != (pfnPER = enc->module->PER.apfnEncoder[id]))
        {
            if ((*pfnPER)(enc, value))
            {
                ASN1PEREncFlush(enc);
            }
        }
        else
        {
            return ASN1EncSetError(enc, ASN1_ERR_BADPDU);
        }
    }
#ifdef ENABLE_BER
    else
    if (ASN1_BER_RULE & enc->eRule)
    {
        /* encode value */
        ASN1BerEncFun_t pfnBER;
        if (NULL != (pfnBER = enc->module->BER.apfnEncoder[id]))
        {
            if ((*pfnBER)(enc, 0, value)) // lonchanc: tag is 0 to make it compiled
            {
                ASN1BEREncFlush(enc);
            }
        }
        else
        {
            return ASN1EncSetError(enc, ASN1_ERR_BADPDU);
        }
    }
#endif // ENABLE_BER
    else
    {
        return ASN1EncSetError(enc, ASN1_ERR_RULE);
    }

    /* call abort/done function for non-parented encoding stream */
    if (e->parent->info.err >= 0)
    {
        if (e == e->parent)
        {
            ASN1EncDone(enc);
        }
    }
    else
    {
        ASN1INTERNencoding_t child, child2;

        if (e == e->parent)
        {
            ASN1EncAbort(enc);
        }

        // clean up...
        ASN1_FreeEncoded(enc, enc->buf);
        enc->pos = enc->buf = NULL;
        enc->size = enc->len = enc->bit = 0;
        for (child = e->child; child; child = child2)
        {
            child2 = child->child;
            // make sure it does not touch its parent which may already be freed
            child->parent = child;
            ASN1_CloseEncoder((ASN1encoding_t) child);
        }
        e->child = NULL;
    }

    /* return error code */
    return e->parent->info.err;
}

static int Test_Decode(ASN1INTERNdecoding_t d, void ** valref, ASN1uint32_t id, ASN1octet_t *pbBuf, ASN1uint32_t cbBufSize)
{
    ASN1decoding_t dec = (ASN1decoding_t) d;
    ASN1uint32_t cbTopLevelStruct;

    /* clear error */
    ASN1DecSetError(dec, ASN1_SUCCESS);

    // set up buffer containing encoded data
    dec->pos = dec->buf = pbBuf;
    dec->size = cbBufSize;
    dec->bit = dec->len = 0;

    /* clear length of linear buffer required */
    d->cbLinearBufSize = 0;
    d->fExtBuf = FALSE;

    cbTopLevelStruct = dec->module->acbStructSize[id];
    if (NULL != (*valref = DecMemAlloc(dec, cbTopLevelStruct)))
    {
        if (ASN1_PER_RULE & dec->eRule)
        {
            ASN1PerDecFun_t pfnPER;
            /* decode value */
            if (NULL != (pfnPER = dec->module->PER.apfnDecoder[id]))
            {
                if ((*pfnPER)(dec, *valref))
                {
                    ASN1PERDecFlush(dec);
                }
            }
            else
            {
                return ASN1DecSetError(dec, ASN1_ERR_BADPDU);
            }
        }
#ifdef ENABLE_BER
        else
        if (ASN1_BER_RULE & dec->eRule)
        {
            ASN1BerDecFun_t pfnBER;
            /* decode value */
            if (NULL != (pfnBER = dec->module->BER.apfnDecoder[id]))
            {
                if ((*pfnBER)(dec, 0, *valref))
                {
                    ASN1BERDecFlush(dec);
                }
            }
            else
            {
                return ASN1DecSetError(dec, ASN1_ERR_BADPDU);
            }
        }
#endif // ENABLE_BER
        else
        {
            return ASN1DecSetError(dec, ASN1_ERR_RULE);
        }

        /* call abort/done function for non-parented decoding stream */
        if (d->parent->info.err >= 0)
        {
            // not parented
            if (d == d->parent)
            {
                ASN1DecDone(dec);
            }
        }
        else
        {
            ASN1INTERNdecoding_t child, child2;

            // not parented
            if (d == d->parent)
            {
                ASN1DecAbort(dec);
            }

            // clean up...
            ASN1_FreeDecoded(dec ,*valref, id);
            *valref = NULL;
            for (child = d->child; child; child = child2)
            {
                child2 = child->child;
                // make sure it does not touch its parent which may already be freed
                child->parent = child;
                ASN1_CloseDecoder((ASN1decoding_t) child);
            }
            d->child = NULL;
        }
    }
    else
    {
        return ASN1_ERR_MEMORY;
    }

    /* return error code */
    return d->parent->info.err;
}

static void Test_CleanEnc(ASN1INTERNencoding_t e)
{
    if (e->info.buf)
    {
        EncMemFree((ASN1encoding_t) e, e->info.buf);
    }
    Test_InitEnc(e, e->info.module, e->info.eRule);
}

static void Test_CleanDec(ASN1INTERNdecoding_t d)
{
    Test_InitDec(d, d->info.module, d->info.eRule);
}

static int TestEnc_Compare(ASN1INTERNencoding_t e, ASN1uint32_t id, ASN1octet_t *pbBuf, ASN1uint32_t cbBufSize)
{
    ASN1INTERNencoding_t ee = TestEnc_GetEnc(e);
    ASN1INTERNdecoding_t ed = TestEnc_GetDec(e);
    int ret;
    void *val = NULL;
    int fRet = 0;

    ee->info.eRule = e->info.eRule;
    ed->info.eRule = e->info.eRule;

    ret = Test_Decode(ed, &val, id, pbBuf, cbBufSize);
    if (ret == ASN1_SUCCESS)
    {
        ret = Test_Encode(ee, val, id);
        if (ret == ASN1_SUCCESS)
        {
            if (ee->info.len == cbBufSize)
            {
                fRet = (MyMemCmp(pbBuf, ee->info.buf, cbBufSize) == 0);
            }
        }
    }

    if (val)
    {
        ASN1_FreeDecoded((ASN1decoding_t) ed, val, id);
    }

    Test_CleanEnc(ee);
    Test_CleanDec(ed);

    return fRet;
}

static int TestDec_Compare(ASN1INTERNdecoding_t d, ASN1uint32_t id, void *val, ASN1octet_t *pbBuf, ASN1uint32_t cbBufSize)
{
    ASN1INTERNencoding_t de = TestDec_GetEnc(d);
    int ret;
    int fRet = 0;

    de->info.eRule = d->info.eRule;

    ret = Test_Encode(de, val, id);
    if (ret == ASN1_SUCCESS)
    {
        if (de->info.len == cbBufSize)
        {
            fRet = (MyMemCmp(pbBuf, de->info.buf, cbBufSize) == 0);
        }
    }

    Test_CleanEnc(de);

    return fRet;
}
#endif

#ifdef ENABLE_BER

static const char bitmsk2[] =
{
    (const char) 0x00,
    (const char) 0x80,
    (const char) 0xc0,
    (const char) 0xe0,
    (const char) 0xf0,
    (const char) 0xf8,
    (const char) 0xfc,
    (const char) 0xfe
};


/* decode bit string value */
int _BERDecBitString(ASN1decoding_t dec, ASN1uint32_t tag, ASN1bitstring_t *val, ASN1uint32_t fNoCopy)
{
    ASN1uint32_t constructed, len, infinite;
    ASN1bitstring_t b;
    ASN1decoding_t dd;
    ASN1octet_t *di;

    /* skip tag */
    if (ASN1BERDecTag(dec, tag, &constructed))
    {
        /* get length */
        if (ASN1BERDecLength(dec, &len, &infinite))
        {
            if (constructed)
            {
                /* constructed? then start decoding of constructed value */
                val->length = 0;
                if (_BERDecConstructed(dec, len, infinite, &dd, &di))
                {
                    while (ASN1BERDecNotEndOfContents(dd, di))
                    {
                        if (_BERDecBitString(dd, 0x3, &b, fNoCopy))
                        {
                            if (b.length)
                            {
                                if (fNoCopy)
                                {
                                    *val = b;
                                    break; // break out the loop because nocopy cannot have multiple constructed streams
                                }

                                /* resize value */
                                val->value = (ASN1octet_t *)DecMemReAlloc(dd, val->value,
                                    (val->length + b.length + 7) / 8);
                                if (val->value)
                                {
                                    /* concat bit strings */
                                    ASN1bitcpy(val->value, val->length, b.value, 0, b.length);
                                    val->length += b.length;
                                    if (val->length & 7)
                                        val->value[val->length / 8] &= bitmsk2[val->length & 7];

                                    /* free unused bit string */
                                    DecMemFree(dec, b.value);
                                }
                                else
                                {
                                    return 0;
                                }
                            }
                        }
                    } // while
                    return ASN1BERDecEndOfContents(dec, dd, di);
                }
            }
            else
            {
                /* primitive? then copy value */
                if (!len)
                {
                    val->length = 0;
                    val->value = NULL;
                    return 1;
                }
                else
                {
                    if (*dec->pos < 8)
                    {
                        len--; // skip over the initial octet; len is now the actual length of octets
                        val->length = len * 8 - *dec->pos++;
                        if (fNoCopy)
                        {
                            val->value = dec->pos;
                            dec->pos += len;
                            return 1;
                        }
                        else
                        {
                            if (val->length)
                            {
                                val->value = (ASN1octet_t *)DecMemAlloc(dec, (val->length + 7) / 8);
                                if (val->value)
                                {
                                    RtlCopyMemory(val->value, dec->pos, len);
                                    if (val->length & 7)
                                        val->value[len - 1] &= bitmsk2[val->length & 7];
                                    dec->pos += len;
                                    return 1;
                                }
                            }
                            else
                            {
                                val->value = NULL;
                                return 1;
                            }
                        }
                    }
                    else
                    {
                        ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
                    }
                }
            }
        }
    }
    return 0;
}

/* decode bit string value, making copy */
int ASN1BERDecBitString(ASN1decoding_t dec, ASN1uint32_t tag, ASN1bitstring_t *val)
{
    return _BERDecBitString(dec, tag, val, FALSE);
}

/* decode bit string value, no copy */
int ASN1BERDecBitString2(ASN1decoding_t dec, ASN1uint32_t tag, ASN1bitstring_t *val)
{
    return _BERDecBitString(dec, tag, val, TRUE);
}

/* decode string value */
int ASN1BERDecCharString(ASN1decoding_t dec, ASN1uint32_t tag, ASN1charstring_t *val)
{
    ASN1uint32_t constructed, len, infinite;
    ASN1charstring_t c;
    ASN1decoding_t dd;
    ASN1octet_t *di;

    /* skip tag */
    if (ASN1BERDecTag(dec, tag, &constructed))
    {
        /* get length */
        if (ASN1BERDecLength(dec, &len, &infinite))
        {
            if (constructed)
            {
                /* constructed? then start decoding of constructed value */
                val->length = 0;
                if (_BERDecConstructed(dec, len, infinite, &dd, &di))
                {
                    while (ASN1BERDecNotEndOfContents(dd, di))
                    {
                        if (ASN1BERDecCharString(dd, 0x4, &c))
                        {
                            if (c.length)
                            {
                                /* resize value */
                                val->value = (char *)DecMemReAlloc(dd, val->value,
                                    val->length + c.length);
                                if (val->value)
                                {
                                    /* concat strings */
                                    RtlCopyMemory(val->value + val->length, c.value, c.length);
                                    val->length += c.length;

                                    /* free unused string */
                                    DecMemFree(dec, c.value);
                                }
                                else
                                {
                                    return 0;
                                }
                            }
                        }
                        else
                        {
                            return 0;
                        }
                    } // while
                    return ASN1BERDecEndOfContents(dec, dd, di);
                }
            }
            else
            {
                /* primitive? then copy value */
                val->length = len;
                if (len)
                {
                    val->value = (char *)DecMemAlloc(dec, len+1);
                    if (val->value)
                    {
                        RtlCopyMemory(val->value, dec->pos, len);
                        dec->pos += len;
                        val->value[len] = 0;
                        return 1;
                    }
                }
                else
                {
                    val->value = NULL;
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* decode 16 bit string value */
int ASN1BERDecChar16String(ASN1decoding_t dec, ASN1uint32_t tag, ASN1char16string_t *val)
{
    ASN1uint32_t constructed, len, infinite;
    ASN1char16string_t c;
    ASN1decoding_t dd;
    ASN1octet_t *di;
    ASN1uint32_t i;

    /* skip tag */
    if (ASN1BERDecTag(dec, tag, &constructed))
    {
        /* get length */
        if (ASN1BERDecLength(dec, &len, &infinite))
        {
            if (constructed)
            {
                /* constructed? then start decoding of constructed value */
                val->length = 0;
                if (_BERDecConstructed(dec, len, infinite, &dd, &di))
                {
                    while (ASN1BERDecNotEndOfContents(dd, di))
                    {
                        if (ASN1BERDecChar16String(dd, 0x4, &c))
                        {
                            if (c.length)
                            {
                                /* resize value */
                                val->value = (ASN1char16_t *)DecMemReAlloc(dd, val->value,
                                    (val->length + c.length) * sizeof(ASN1char16_t));
                                if (val->value)
                                {
                                    /* concat strings */
                                    RtlCopyMemory(val->value + val->length, c.value,
                                        c.length * sizeof(ASN1char16_t));
                                    val->length += c.length;

                                    /* free unused string */
                                    DecMemFree(dec, c.value);
                                }
                                else
                                {
                                    return 0;
                                }
                            }
                        }
                        else
                        {
                            return 0;
                        }
                    }
                    return ASN1BERDecEndOfContents(dec, dd, di);
                }
            }
            else
            {
                /* primitive? then copy value */
                DecAssert(dec, 2 * sizeof(ASN1octet_t) == sizeof(ASN1char16_t));
                len = len >> 1; // divided by 2
                val->length = len;
                if (len)
                {
                    val->value = (ASN1char16_t *)DecMemAlloc(dec, (len+1) * sizeof(ASN1char16_t));
                    if (val->value)
                    {
                        for (i = 0; i < len; i++)
                        {
                            val->value[i] = (*dec->pos << 8) | dec->pos[1];
                            dec->pos += 2;
                        }
                        val->value[len] = 0;
                        return 1;
                    }
                }
                else
                {
                    val->value = NULL;
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* decode 32 bit string value */
int ASN1BERDecChar32String(ASN1decoding_t dec, ASN1uint32_t tag, ASN1char32string_t *val)
{
    ASN1uint32_t constructed, len, infinite;
    ASN1char32string_t c;
    ASN1decoding_t dd;
    ASN1octet_t *di;
    ASN1uint32_t i;

    /* skip tag */
    if (ASN1BERDecTag(dec, tag, &constructed))
    {
        /* get length */
        if (ASN1BERDecLength(dec, &len, &infinite))
        {
            if (constructed)
            {
                /* constructed? then start decoding of constructed value */
                val->length = 0;
                if (_BERDecConstructed(dec, len, infinite, &dd, &di))
                {
                    while (ASN1BERDecNotEndOfContents(dd, di))
                    {
                        if (ASN1BERDecChar32String(dd, 0x4, &c))
                        {
                            if (c.length)
                            {
                                /* resize value */
                                val->value = (ASN1char32_t *)DecMemReAlloc(dd, val->value,
                                    (val->length + c.length) * sizeof(ASN1char32_t));
                                if (val->value)
                                {
                                    /* concat strings */
                                    RtlCopyMemory(val->value + val->length, c.value,
                                        c.length * sizeof(ASN1char32_t));
                                    val->length += c.length;

                                    /* free unused string */
                                    DecMemFree(dec, c.value);
                                }
                                else
                                {
                                    return 0;
                                }
                            }
                        }
                        else
                        {
                            return 0;
                        }
                    }
                    return ASN1BERDecEndOfContents(dec, dd, di);
                }
            }
            else
            {
                /* primitive? then copy value */
                DecAssert(dec, 4 * sizeof(ASN1octet_t) == sizeof(ASN1char32_t));
                len = len >> 2; // divided by 4
                val->length = len;
                if (len)
                {
                    val->value = (ASN1char32_t *)DecMemAlloc(dec, (len+1) * sizeof(ASN1char32_t));
                    if (val->value)
                    {
                        for (i = 0; i < len; i++)
                        {
                            val->value[i] = (*dec->pos << 24) | (dec->pos[1] << 16) |
                                (dec->pos[2] << 8) | dec->pos[3];;
                            dec->pos += 4;
                        }
                        val->value[len] = 0;
                        return 1;
                    }
                }
                else
                {
                    val->value = NULL;
                    return 1;
                }
            }
        }
    }
    return 0;
}

#ifdef ENABLE_GENERALIZED_CHAR_STR
/* decode character string value */
int ASN1BERDecCharacterString(ASN1decoding_t dec, ASN1uint32_t tag, ASN1characterstring_t *val)
{
    ASN1INTERNdecoding_t d = (ASN1INTERNdecoding_t) dec;
    ASN1uint32_t constructed, len, infinite;
    ASN1uint32_t index;
    ASN1characterstring_identification_t *identification;
    ASN1decoding_t dd, dd2, dd3;
    ASN1octet_t *di, *di2, *di3;

    /* skip tag */
    if (!ASN1BERDecTag(dec, tag, &constructed))
        return 0;

    if (constructed)
    {
        /* constructed? CS-A encoded: */
        /* get length */
        if (!ASN1BERDecLength(dec, &len, &infinite))
            return 0;

        /* start decoding of constructed value */
        if (! _BERDecConstructed(dec, len, infinite, &dd, &di))
            return 0;
        if (!ASN1BERDecU32Val(dd, 0x80000000, &index))
            return 0;
        if (index != d->parent->csilength)
        {
            ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
            return 0;
        }
        if (!ASN1BERDecExplicitTag(dd, 0x80000001, &dd2, &di2))
            return 0;
        if (!ASN1BERDecPeekTag(dd2, &tag))
            return 0;
        switch (tag)
        {
        case 0x80000000:
            val->identification.o =
                ASN1characterstring_identification_syntaxes_o;
            if (!ASN1BERDecExplicitTag(dd2, 0x80000000, &dd3, &di3))
                return 0;
            if (!ASN1BERDecObjectIdentifier(dd3, 0x80000000,
                &val->identification.u.syntaxes.abstract))
                return 0;
            if (!ASN1BERDecObjectIdentifier(dd3, 0x80000001,
                &val->identification.u.syntaxes.transfer))
                return 0;
            if (!ASN1BERDecEndOfContents(dd2, dd3, di3))
                return 0;
            break;
        case 0x80000001:
            val->identification.o = ASN1characterstring_identification_syntax_o;
            if (!ASN1BERDecObjectIdentifier(dd2, 0x80000001,
                &val->identification.u.syntax))
                return 0;
            break;
        case 0x80000002:
            val->identification.o =
                ASN1characterstring_identification_presentation_context_id_o;
            if (!ASN1BERDecU32Val(dd2, 0x80000002,
                &val->identification.u.presentation_context_id))
                return 0;
            break;
        case 0x80000003:
            val->identification.o =
                ASN1characterstring_identification_context_negotiation_o;
            if (!ASN1BERDecExplicitTag(dd2, 0x80000003, &dd3, &di3))
                return 0;
            if (!ASN1BERDecU32Val(dd3, 0x80000000, &val->
                identification.u.context_negotiation.presentation_context_id))
                return 0;
            if (!ASN1BERDecObjectIdentifier(dd3, 0x80000001,
                &val->identification.u.context_negotiation.transfer_syntax))
                return 0;
            if (!ASN1BERDecEndOfContents(dd2, dd3, di3))
                return 0;
            break;
        case 0x80000004:
            val->identification.o =
                ASN1characterstring_identification_transfer_syntax_o;
            if (!ASN1BERDecObjectIdentifier(dd2, 0x80000004,
                &val->identification.u.transfer_syntax))
                return 0;
            break;
        case 0x80000005:
            val->identification.o = ASN1characterstring_identification_fixed_o;
            if (!ASN1BERDecNull(dd2, 0x80000005))
                return 0;
            break;
        default:
            ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
            return 0;
        }
        if (!ASN1BERDecEndOfContents(dd, dd2, di2))
            return 0;
        if (!ASN1DecAddCharacterStringIdentification(d->parent,
            &val->identification))
            return 0;
        val->data_value.o = ASN1characterstring_data_value_encoded_o;
        if (!ASN1BERDecOctetString(dd, 0x80000003,
            &val->data_value.u.encoded))
            return 0;
        if (!ASN1BERDecEndOfContents(dec, dd, di))
            return 0;
    }
    else
    {
        /* primitive? CS-B encoded */
        /* get length */
        if (!ASN1BERDecLength(dec, &len, NULL))
            return 0;

        /* then copy value */
        if (!len)
        {
            ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
            return 0;
        }
        val->data_value.o = ASN1characterstring_data_value_encoded_o;
        val->data_value.u.encoded.length = len - 1;
        val->data_value.u.encoded.value = (ASN1octet_t *)DecMemAlloc(dec, len - 1);
        if (!val->data_value.u.encoded.value)
        {
            return 0;
        }
        index = *dec->pos++;
        RtlCopyMemory(val->data_value.u.encoded.value, dec->pos, len - 1);
        identification = ASN1DecGetCharacterStringIdentification(d->parent,
            index);
        if (!identification)
            return 0;
        val->identification.o = identification->o;
        switch (identification->o)
        {
        case ASN1characterstring_identification_syntaxes_o:
            if (!ASN1DecDupObjectIdentifier(dec, 
                &val->identification.u.syntaxes.abstract,
                &identification->u.syntaxes.abstract))
                return 0;
            if (!ASN1DecDupObjectIdentifier(dec,
                &val->identification.u.syntaxes.transfer,
                &identification->u.syntaxes.transfer))
                return 0;
            break;
        case ASN1characterstring_identification_syntax_o:
            if (!ASN1DecDupObjectIdentifier(dec,
                &val->identification.u.syntax,
                &identification->u.syntax))
                return 0;
            break;
        case ASN1characterstring_identification_presentation_context_id_o:
            val->identification.u.presentation_context_id =
                identification->u.presentation_context_id;
            break;
        case ASN1characterstring_identification_context_negotiation_o:
            val->identification.u.context_negotiation.presentation_context_id =
                identification->u.context_negotiation.presentation_context_id;
            if (!ASN1DecDupObjectIdentifier(dec,
                &val->identification.u.context_negotiation.transfer_syntax,
                &identification->u.context_negotiation.transfer_syntax))
                return 0;
            break;
        case ASN1characterstring_identification_transfer_syntax_o:
            if (!ASN1DecDupObjectIdentifier(dec,
                &val->identification.u.transfer_syntax,
                &identification->u.transfer_syntax))
                return 0;
            break;
        case ASN1characterstring_identification_fixed_o:
            break;
        }
    }
    return 1;
}
#endif // ENABLE_GENERALIZED_CHAR_STR

#ifdef ENABLE_DOUBLE
/* decode real value */
int ASN1BERDecDouble(ASN1decoding_t dec, ASN1uint32_t tag, double *val)
{
    ASN1uint32_t head;
    ASN1int32_t exponent;
    ASN1uint32_t baselog2;
    ASN1uint32_t len;
    ASN1uint32_t i;
    ASN1octet_t *p, *q;
    double v;
    char buf[256], *b;

    /* skip tag */
    if (!ASN1BERDecTag(dec, tag, NULL))
        return 0;

    /* get length */
    if (!ASN1BERDecLength(dec, &len, NULL))
        return 0;

    /* null length is 0.0 */
    if (!len)
    {
        *val = 0.0;
    }
    else
    {
        p = q = dec->pos;
        dec->pos += len;
        head = *p++;

        /* binary encoding? */
        if (head & 0x80)
        {
            /* get base */
            switch (head & 0x30)
            {
            case 0:
                /* base 2 */
                baselog2 = 1;
                break;
            case 0x10:
                /* base 8 */
                baselog2 = 3;
                break;
            case 0x20:
                /* base 16 */
                baselog2 = 4;
                break;
            default:
                ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
                return 0;
            }

            /* get exponent */
            switch (head & 0x03)
            {
            case 0:
                /* 8 bit exponent */
                exponent = (ASN1int8_t)*p++;
                break;
            case 1:
                /* 16 bit exponent */
                exponent = (ASN1int16_t)((*p << 8) | p[1]);
                p += 2;
                break;
            case 2:
                /* 24 bit exponent */
                exponent = ((*p << 16) | (p[1] << 8) | p[2]);
                if (exponent & 0x800000)
                    exponent -= 0x1000000;
                break;
            default:
                /* variable length exponent */
                exponent = (p[1] & 0x80) ? -1 : 0;
                for (i = 1; i <= *p; i++)
                    exponent = (exponent << 8) | p[i];
                p += *p + 1;
                break;
            }

            /* calculate remaining length */
            len -= (ASN1uint32_t) (p - q);

            /* get mantissa */
            v = 0.0;
            for (i = 0; i < len; i++)
                v = v * 256.0 + *p++;

            /* scale mantissa */
            switch (head & 0x0c)
            {
            case 0x04:
                /* scaling factor 1 */
                v *= 2.0;
                break;
            case 0x08:
                /* scaling factor 2 */
                v *= 4.0;
                break;
            case 0x0c:
                /* scaling factor 3 */
                v *= 8.0;
                break;
            }

            /* check sign */
            if (head & 0x40)
                v = -v;

            /* calculate value */
            *val = ldexp(v, exponent * baselog2);
        }
        else
        /* special real values? */
        if (head & 0x40)
        {
            switch (head)
            {
            case 0x40:
                /* PLUS-INFINITY */
                *val = ASN1double_pinf();
                break;
            case 0x41:
                /* MINUS-INFINITY */
                *val = ASN1double_minf();
                break;
            default:
                ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
                return 0;
            }
        }
        /* decimal encoding */
        else
        {
            RtlCopyMemory(buf, p, len - 1);
            buf[len - 1] = 0;
            b = strchr(buf, ',');
            if (b)
                *b = '.';
            *val = strtod((char *)buf, &b);
            if (*b)
            {
                ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
                return 0;
            }
        }
    }
    return 1;
}
#endif // ENABLE_DOUBLE

#ifdef ENABLE_REAL
int ASN1BERDecReal(ASN1decoding_t dec, ASN1uint32_t tag, ASN1real_t *val)
{
    ASN1uint32_t head;
    ASN1int32_t ex;
    // ASN1intx_t exponent;
    ASN1uint32_t baselog2;
    ASN1uint32_t len;
    ASN1uint32_t i;
    ASN1octet_t *p, *q;
    double v;
    ASN1intx_t help;

    if (!ASN1BERDecTag(dec, tag, NULL))
        return 0;
    if (!ASN1BERDecLength(dec, &len, NULL))
        return 0;

    // *val = 0.0;
    DecAssert(dec, 0 == (int) eReal_Normal);
    RtlZeroMemory(val, sizeof(*val));
    if (len)
    {
        p = q = dec->pos;
        dec->pos += len;
        head = *p++;

        /* binary encoding? */
        if (head & 0x80)
        {
            val->type = eReal_Normal;

            /* get base */
            switch (head & 0x30)
            {
            case 0:
                /* base 2 */
                baselog2 = 1;
                break;
            case 0x10:
                /* base 8 */
                baselog2 = 3;
                break;
            case 0x20:
                /* base 16 */
                baselog2 = 4;
                break;
            default:
                ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
                return 0;
            }

            /* get exponent */
            switch (head & 0x03)
            {
            case 0:
                /* 8 bit exponent */
                ex = (ASN1int8_t)*p++;
                ASN1intx_setint32(&val->exponent, ex);
                break;
            case 1:
                /* 16 bit exponent */
                ex = (ASN1int16_t)((*p << 8) | p[1]);
                p += 2;
                // ASN1intx_setint32_t(&exponent, ex);
                ASN1intx_setint32(&val->exponent, ex);
                break;
            case 2:
                /* 24 bit exponent */
                ex = ((*p << 16) | (p[1] << 8) | p[2]);
                if (ex & 0x800000)
                    ex -= 0x1000000;
                // ASN1intx_setint32_t(&exponent, ex);
                ASN1intx_setint32(&val->exponent, ex);
                break;
            default:
                /* variable length exponent */
                val->exponent.length = *p;
                val->exponent.value = (ASN1octet_t *)DecMemAlloc(dec, *p);
                if (!val->exponent.value)
                {
                    return 0;
                }
                RtlCopyMemory(val->exponent.value, p + 1, *p);
                p += *p + 1;
                break;
            }

            /* calculate remaining length */
            len -= (p - q);
            if (!len)
            {
                ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
                return 0;
            }

            /* get mantissa */
            val->mantissa.length = (*p & 0x80) ? len + 1 : len;
            val->mantissa.value = (ASN1octet_t *)DecMemAlloc(dec, val->mantissa.length);
            if (!val->mantissa.value)
            {
                return 0;
            }
            val->mantissa.value[0] = 0;
            RtlCopyMemory(val->mantissa.value + val->mantissa.length - len, p, len);

            /* scale mantissa */
            switch (head & 0x0c)
            {
            case 0x04:
                /* scaling factor 1 */
                ASN1intx_muloctet(&help, &val->mantissa, 2);
                ASN1intx_free(&val->mantissa);
                val->mantissa = help;
                break;
            case 0x08:
                /* scaling factor 2 */
                ASN1intx_muloctet(&help, &val->mantissa, 4);
                ASN1intx_free(&val->mantissa);
                val->mantissa = help;
                break;
            case 0x0c:
                /* scaling factor 3 */
                ASN1intx_muloctet(&help, &val->mantissa, 8);
                ASN1intx_free(&val->mantissa);
                val->mantissa = help;
                break;
            }

            /* check sign */
            if (head & 0x40)
            {
                ASN1intx_neg(&help, &val->mantissa);
                ASN1intx_free(&val->mantissa);
                val->mantissa = help;
            }
        }
        else
        /* special real values? */
        if (head & 0x40)
        {
            switch (head)
            {
            case 0x40:
                /* PLUS-INFINITY */
                val->type = eReal_PlusInfinity;
                break;
            case 0x41:
                /* MINUS-INFINITY */
                val->type = eReal_MinusInfinity;
                break;
            default:
                ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
                return 0;
            }
        }
        /* decimal encoding */
        else
        {
            char *b;
            char buf[256];

            DecAssert(dec, (head & 0xc0) == 0xc0); 
            RtlCopyMemory(buf, p, len - 1);
            buf[len - 1] = 0;
            ex = 0;
            b = strchr(buf, ',');
            if (b)
            {
                // move the decimal point to the right
                ex -= My_lstrlenA(b+1);
                lstrcpyA(b, b+1);
            }
            // skip leading zeros
            for (b = &buf[0]; '0' == *b; b++)
                ;
            val->type = eReal_Normal;
            val->base = 10;
            ASN1intx_setint32(&val->exponent, ex);
            /*XXX*/
            // missing code here!!!
            // need to set val->mantissa through the decimal digits string
            DecAssert(dec, 0);
            return 0;
        }
    }
    return 1;
}
#endif // ENABLE_REAL

#ifdef ENABLE_EMBEDDED_PDV
/* decode embedded pdv value */
int ASN1BERDecEmbeddedPdv(ASN1decoding_t dec, ASN1uint32_t tag, ASN1embeddedpdv_t *val)
{
    ASN1INTERNdecoding_t d = (ASN1INTERNdecoding_t) dec;
    ASN1uint32_t constructed, len, infinite;
    ASN1uint32_t index;
    ASN1embeddedpdv_identification_t *identification;
    ASN1decoding_t dd, dd2, dd3;
    ASN1octet_t *di, *di2, *di3;

    /* skip tag */
    if (!ASN1BERDecTag(dec, tag, &constructed))
        return 0;

    if (constructed)
    {
        /* constructed? EP-A encoded: */
        /* get length */
        if (!ASN1BERDecLength(dec, &len, &infinite))
            return 0;

        /* then start decoding of constructed value */
        if (! _BERDecConstructed(dec, len, infinite, &dd, &di))
            return 0;
        if (!ASN1BERDecU32Val(dd, 0x80000000, &index))
            return 0;
        if (index != d->parent->epilength)
        {
            ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
            return 0;
        }
        if (!ASN1BERDecExplicitTag(dd, 0x80000001, &dd2, &di2))
            return 0;
        if (!ASN1BERDecPeekTag(dd2, &tag))
            return 0;
        switch (tag)
        {
        case 0x80000000:
            val->identification.o = ASN1embeddedpdv_identification_syntaxes_o;
            if (!ASN1BERDecExplicitTag(dd2, 0x80000000, &dd3, &di3))
                return 0;
            if (!ASN1BERDecObjectIdentifier(dd3, 0x80000000,
                &val->identification.u.syntaxes.abstract))
                return 0;
            if (!ASN1BERDecObjectIdentifier(dd3, 0x80000001,
                &val->identification.u.syntaxes.transfer))
                return 0;
            if (!ASN1BERDecEndOfContents(dd2, dd3, di3))
                return 0;
            break;
        case 0x80000001:
            val->identification.o = ASN1embeddedpdv_identification_syntax_o;
            if (!ASN1BERDecObjectIdentifier(dd2, 0x80000001,
                &val->identification.u.syntax))
                return 0;
            break;
        case 0x80000002:
            val->identification.o =
                ASN1embeddedpdv_identification_presentation_context_id_o;
            if (!ASN1BERDecU32Val(dd2, 0x80000002,
                &val->identification.u.presentation_context_id))
                return 0;
            break;
        case 0x80000003:
            val->identification.o =
                ASN1embeddedpdv_identification_context_negotiation_o;
            if (!ASN1BERDecExplicitTag(dd2, 0x80000003, &dd3, &di3))
                return 0;
            if (!ASN1BERDecU32Val(dd3, 0x80000000, &val->
                identification.u.context_negotiation.presentation_context_id))
                return 0;
            if (!ASN1BERDecObjectIdentifier(dd3, 0x80000001,
                &val->identification.u.context_negotiation.transfer_syntax))
                return 0;
            if (!ASN1BERDecEndOfContents(dd2, dd3, di3))
                return 0;
            break;
        case 0x80000004:
            val->identification.o =
                ASN1embeddedpdv_identification_transfer_syntax_o;
            if (!ASN1BERDecObjectIdentifier(dd2, 0x80000004,
                &val->identification.u.transfer_syntax))
                return 0;
            break;
        case 0x80000005:
            val->identification.o = ASN1embeddedpdv_identification_fixed_o;
            if (!ASN1BERDecNull(dd2, 0x80000005))
                return 0;
            break;
        default:
            ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
            return 0;
        }
        if (!ASN1BERDecEndOfContents(dd, dd2, di2))
            return 0;
        if (!ASN1DecAddEmbeddedPdvIdentification(d->parent,
            &val->identification))
            return 0;
        val->data_value.o = ASN1embeddedpdv_data_value_encoded_o;
        if (!ASN1BERDecBitString(dd, 0x80000003,
            &val->data_value.u.encoded))
            return 0;
        if (!ASN1BERDecEndOfContents(dec, dd, di))
            return 0;
    }
    else
    {
        /* primitive? EP-B encoded: */
        if (!ASN1BERDecLength(dec, &len, NULL))
            return 0;

        /* then copy value */
        if (!len)
        {
            ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
            return 0;
        }
        val->data_value.o = ASN1embeddedpdv_data_value_encoded_o;
        val->data_value.u.encoded.length = 8 * (len - 1);
        val->data_value.u.encoded.value = (ASN1octet_t *)DecMemAlloc(dec, len - 1);
        if (!val->data_value.u.encoded.value)
        {
            return 0;
        }
        index = *dec->pos++;
        RtlCopyMemory(val->data_value.u.encoded.value, dec->pos, len - 1);
        identification = ASN1DecGetEmbeddedPdvIdentification(d->parent, index);
        if (!identification)
            return 0;
        val->identification.o = identification->o;
        switch (identification->o)
        {
        case ASN1embeddedpdv_identification_syntaxes_o:
            if (!ASN1DecDupObjectIdentifier(dec, 
                &val->identification.u.syntaxes.abstract,
                &identification->u.syntaxes.abstract))
                return 0;
            if (!ASN1DecDupObjectIdentifier(dec,
                &val->identification.u.syntaxes.transfer,
                &identification->u.syntaxes.transfer))
                return 0;
            break;
        case ASN1embeddedpdv_identification_syntax_o:
            if (!ASN1DecDupObjectIdentifier(dec,
                &val->identification.u.syntax,
                &identification->u.syntax))
                return 0;
            break;
        case ASN1embeddedpdv_identification_presentation_context_id_o:
            val->identification.u.presentation_context_id =
                identification->u.presentation_context_id;
            break;
        case ASN1embeddedpdv_identification_context_negotiation_o:
            val->identification.u.context_negotiation.presentation_context_id =
                identification->u.context_negotiation.presentation_context_id;
            if (!ASN1DecDupObjectIdentifier(dec,
                &val->identification.u.context_negotiation.transfer_syntax,
                &identification->u.context_negotiation.transfer_syntax))
                return 0;
            break;
        case ASN1embeddedpdv_identification_transfer_syntax_o:
            if (!ASN1DecDupObjectIdentifier(dec,
                &val->identification.u.transfer_syntax,
                &identification->u.transfer_syntax))
                return 0;
            break;
        case ASN1embeddedpdv_identification_fixed_o:
            break;
        }
    }
    return 1;
}
#endif // ENABLE_EMBEDDED_PDV

#ifdef ENABLE_EXTERNAL
/* decode external value */
int ASN1BERDecExternal(ASN1decoding_t dec, ASN1uint32_t tag, ASN1external_t *val)
{
    ASN1decoding_t dd;
    ASN1octet_t *di;
    ASN1objectidentifier_t id;
    ASN1octetstring_t os;

    /* decode explicit tag */
    if (!ASN1BERDecExplicitTag(dec, tag | 0x20000000, &dd, &di))
        return 0;

    /* peek tag of choice alternative */
    if (!ASN1BERDecPeekTag(dd, &tag))
        return 0;

    /* decode alternative */
    if (tag == 0x6)
    {
        if (!ASN1BERDecObjectIdentifier(dd, 0x6, &id))
            return 0;
        if (!ASN1BERDecPeekTag(dd, &tag))
            return 0;
        if (tag == 0x2)
        {
            val->identification.o =
                ASN1external_identification_context_negotiation_o;
            val->identification.u.context_negotiation.transfer_syntax = id;
            if (!ASN1BERDecU32Val(dd, 0x2, &val->
                identification.u.context_negotiation.presentation_context_id))
                return 0;
            if (!ASN1BERDecPeekTag(dd, &tag))
                return 0;
        }
        else
        {
            val->identification.o = ASN1external_identification_syntax_o;
            val->identification.u.syntax = id;
        }
    }
    else
    if (tag == 0x2)
    {
        val->identification.o =
            ASN1external_identification_presentation_context_id_o;
        if (!ASN1BERDecU32Val(dd, 0x2,
            &val->identification.u.presentation_context_id))
            return 0;
        if (!ASN1BERDecPeekTag(dd, &tag))
            return 0;
    }
    else
    {
        ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
        return 0;
    }

    /* decode optional data value descriptor if present */
    if (tag == 0x7)
    {
        if (!ASN1BERDecZeroCharString(dd, 0x7, &val->data_value_descriptor))
            return 0;
        if (!ASN1BERDecPeekTag(dd, &tag))
            return 0;
    }
    else
    {
        val->data_value_descriptor = NULL;
    }

    /* decode data value alternative */
    switch (tag)
    {
    case 0:
        val->data_value.o = ASN1external_data_value_notation_o;
        if (!ASN1BERDecOpenType(dd, &val->data_value.u.notation))
            return 0;
        break;
    case 1:
        val->data_value.o = ASN1external_data_value_encoded_o;
        if (!ASN1BERDecOctetString(dd, 0x4, &os))
            return 0;
        val->data_value.u.encoded.value = os.value;
        val->data_value.u.encoded.length = os.length * 8;
        break;
    case 2:
        val->data_value.o = ASN1external_data_value_encoded_o;
        if (!ASN1BERDecBitString(dd, 0x3, &val->data_value.u.encoded))
            return 0;
        break;
    default:
        ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
        return 0;
    }

    /* end of constructed (explicit tagged) value */
    if (!ASN1BERDecEndOfContents(dec, dd, di))
        return 0;

    return 1;
}
#endif // ENABLE_EXTERNAL

/* decode generalized time value */
int ASN1BERDecGeneralizedTime(ASN1decoding_t dec, ASN1uint32_t tag, ASN1generalizedtime_t *val)
{
    ASN1ztcharstring_t time;
    if (ASN1BERDecZeroCharString(dec, tag, &time))
    {
        int rc = ASN1string2generalizedtime(val, time);
        DecMemFree(dec, time);
        if (rc)
        {
            return 1;
        }
        ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
    }
    return 0;
}

/* decode multibyte string value */
int ASN1BERDecZeroMultibyteString(ASN1decoding_t dec, ASN1uint32_t tag, ASN1ztcharstring_t *val)
{
    return ASN1BERDecZeroCharString(dec, tag, val);
}

int ASN1BERDecMultibyteString(ASN1decoding_t dec, ASN1uint32_t tag, ASN1charstring_t *val)
{
    return ASN1BERDecCharString(dec, tag, val);
}

/* decode null value */
int ASN1BERDecNull(ASN1decoding_t dec, ASN1uint32_t tag)
{
    ASN1uint32_t len;
    if (ASN1BERDecTag(dec, tag, NULL))
    {
        if (ASN1BERDecLength(dec, &len, NULL))
        {
            if (! len)
            {
                return 1;
            }
            ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
        }
    }
    return 0;
}

/* decode object identifier value */
int ASN1BERDecObjectIdentifier(ASN1decoding_t dec, ASN1uint32_t tag, ASN1objectidentifier_t *val)
{
    if (ASN1BERDecTag(dec, tag, NULL))
    {
        ASN1uint32_t len, i, v;
        ASN1octet_t *data, *p;
        ASN1uint32_t nelem;
        ASN1objectidentifier_t q;

        if (ASN1BERDecLength(dec, &len, NULL))
        {
            data = dec->pos;
            dec->pos += len;
            nelem = 1;
            for (i = 0, p = data; i < len; i++, p++)
            {
                if (!(*p & 0x80))
                    nelem++;
            }
            *val = q = DecAllocObjectIdentifier(dec, nelem);
            if (q)
            {
                v = 0;
                for (i = 0, p = data; i < len; i++, p++)
                {
                    v = (v << 7) | (*p & 0x7f);
                    if (!(*p & 0x80))
                    {
                        if (q == *val)
                        { // first id
                            q->value = v / 40;
                            if (q->value > 2)
                                q->value = 2;
                            q->next->value = v - 40 * q->value;
                            q = q->next->next;
                        }
                        else
                        {
                            q->value = v;
                            q = q->next;
                        }
                        v = 0;
                    }
                }
                return 1;
            }
        }
    }
    return 0;
}

/* decode object identifier value */
int ASN1BERDecObjectIdentifier2(ASN1decoding_t dec, ASN1uint32_t tag, ASN1objectidentifier2_t *val)
{
    if (ASN1BERDecTag(dec, tag, NULL))
    {
        ASN1uint32_t len, i, v;
        ASN1octet_t *data, *p;
        //ASN1objectidentifier_t q;

        if (ASN1BERDecLength(dec, &len, NULL))
        {
            if (len <= 16) // lonchanc: hard-coded value 16 to be consistent with ASN1objectidentifier2_t
            {
                data = dec->pos;
                dec->pos += len;
                val->count = 0;
                v = 0;
                for (i = 0, p = data; i < len; i++, p++)
                {
                    v = (v << 7) | (*p & 0x7f);
                    if (!(*p & 0x80))
                    {
                        if (! val->count)
                        { // first id
                            val->value[0] = v / 40;
                            if (val->value[0] > 2)
                                val->value[0] = 2;
                            val->value[1] = v - 40 * val->value[0];
                            val->count = 2;
                        }
                        else
                        {
                            val->value[val->count++] = v;
                        }
                        v = 0;
                    }
                }
                return 1;
            }
            else
            {
                ASN1DecSetError(dec, ASN1_ERR_LARGE);
            }
        }
    }
    return 0;
}

/* decode integer into signed 8 bit value */
int ASN1BERDecS8Val(ASN1decoding_t dec, ASN1uint32_t tag, ASN1int8_t *val)
{
    /* skip tag */
    if (ASN1BERDecTag(dec, tag, NULL))
    {
        ASN1uint32_t len;
        /* get length */
        if (ASN1BERDecLength(dec, &len, NULL))
        {
            /* get value */
            if (1 == len)
            {
                *val = *dec->pos++;
                return 1;
            }
            ASN1DecSetError(dec, (len < 1) ? ASN1_ERR_CORRUPT : ASN1_ERR_LARGE);
        }
    }
    return 0;
}

/* decode integer into signed 16 bit value */
int ASN1BERDecS16Val(ASN1decoding_t dec, ASN1uint32_t tag, ASN1int16_t *val)
{
    /* skip tag */
    if (ASN1BERDecTag(dec, tag, NULL))
    {
        ASN1uint32_t len;
        /* get length */
        if (ASN1BERDecLength(dec, &len, NULL))
        {
            /* get value */
            switch (len)
            {
            case 1:
                *val = *dec->pos++;
                break;
            case 2:
                *val = (*dec->pos << 8) | dec->pos[1];
                dec->pos += 2;
                break;
            default:
                ASN1DecSetError(dec, (len < 1) ? ASN1_ERR_CORRUPT : ASN1_ERR_LARGE);
                return 0;
            }
            return 1;
        }
    }
    return 0;
}

const ASN1int32_t c_nSignMask[] = { 0xFFFFFF00, 0xFFFF0000, 0xFF000000, 0 };

/* decode integer into signed 32 bit value */
int ASN1BERDecS32Val(ASN1decoding_t dec, ASN1uint32_t tag, ASN1int32_t *val)
{
    /* skip tag */
    if (ASN1BERDecTag(dec, tag, NULL))
    {
        ASN1uint32_t len;
        /* get length */
        if (ASN1BERDecLength(dec, &len, NULL))
        {
            int fSigned = 0x80 & *dec->pos;

            /* get value */
            switch (len)
            {
            case 1:
                *val = *dec->pos++;
                break;
            case 2:
                *val = (*dec->pos << 8) | dec->pos[1];
                dec->pos += 2;
                break;
            case 3:
                *val = (*dec->pos << 16) | (dec->pos[1] << 8) | dec->pos[2];
                dec->pos += 3;
                break;
            case 4:
                *val = (*dec->pos << 24) | (dec->pos[1] << 16) |
                    (dec->pos[2] << 8) | dec->pos[3];
                dec->pos += 4;
                break;
            default:
                ASN1DecSetError(dec, (len < 1) ? ASN1_ERR_CORRUPT : ASN1_ERR_LARGE);
                return 0;
            }
            if (fSigned)
            {
                *val |= c_nSignMask[len-1];
            }
            return 1;
        }
    }
    return 0;
}

/* decode integer into intx value */
int ASN1BERDecSXVal(ASN1decoding_t dec, ASN1uint32_t tag, ASN1intx_t *val)
{
    /* skip tag */
    if (ASN1BERDecTag(dec, tag, NULL))
    {
        ASN1uint32_t len;
        /* get length */
        if (ASN1BERDecLength(dec, &len, NULL))
        {
            /* get value */
            if (len >= 1)
            {
                val->length = len;
                val->value = (ASN1octet_t *)DecMemAlloc(dec, len);
                if (val->value)
                {
                    RtlCopyMemory(val->value, dec->pos, len);
                    dec->pos += len;
                    return 1;
                }
            }
            else
            {
                ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
            }
        }
    }
    return 0;
}

/* decode integer into unsigned 8 bit value */
int ASN1BERDecU8Val(ASN1decoding_t dec, ASN1uint32_t tag, ASN1uint8_t *val)
{
    /* skip tag */
    if (ASN1BERDecTag(dec, tag, NULL))
    {
        ASN1uint32_t len;
        /* get length */
        if (ASN1BERDecLength(dec, &len, NULL))
        {
            /* get value */
            switch (len)
            {
            case 1:
                *val = *dec->pos++;
                return 1;
            case 2:
                if (0 == *dec->pos)
                {
                    *val = dec->pos[1];
                    dec->pos += 2;
                    return 1;
                }
                // intentionally fall through
            default:
                ASN1DecSetError(dec, (len < 1) ? ASN1_ERR_CORRUPT : ASN1_ERR_LARGE);
                break;
            }
        }
    }
    return 0;
}

/* decode integer into unsigned 16 bit value */
int ASN1BERDecU16Val(ASN1decoding_t dec, ASN1uint32_t tag, ASN1uint16_t *val)
{
    /* skip tag */
    if (ASN1BERDecTag(dec, tag, NULL))
    {
        ASN1uint32_t len;
        /* get length */
        if (ASN1BERDecLength(dec, &len, NULL))
        {
            /* get value */
            switch (len)
            {
            case 1:
                *val = *dec->pos++;
                return 1;
            case 2:
                *val = (*dec->pos << 8) | dec->pos[1];
                dec->pos += 2;
                return 1;
            case 3:
                if (0 == *dec->pos)
                {
                    *val = (dec->pos[1] << 8) | dec->pos[2];
                    dec->pos += 3;
                    return 1;
                }
                // intentionally fall through
            default:
                ASN1DecSetError(dec, (len < 1) ? ASN1_ERR_CORRUPT : ASN1_ERR_LARGE);
                break;
            }
        }
    }
    return 0;
}

/* decode utc time value */
int ASN1BERDecUTCTime(ASN1decoding_t dec, ASN1uint32_t tag, ASN1utctime_t *val)
{
    ASN1ztcharstring_t time;
    if (ASN1BERDecZeroCharString(dec, tag, &time))
    {
        int rc = ASN1string2utctime(val, time);
        DecMemFree(dec, time);
        if (rc)
        {
            return 1;
        }
        ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
    }
    return 0;
}

/* decode zero terminated string value */
int ASN1BERDecZeroCharString(ASN1decoding_t dec, ASN1uint32_t tag, ASN1ztcharstring_t *val)
{
    ASN1uint32_t constructed, len, infinite;
    ASN1ztcharstring_t c;
    ASN1decoding_t dd;
    ASN1octet_t *di;
    ASN1uint32_t lv, lc;

    /* skip tag */
    if (ASN1BERDecTag(dec, tag, &constructed))
    {
        /* get length */
        if (ASN1BERDecLength(dec, &len, &infinite))
        {
            if (constructed)
            {
                /* constructed? then start decoding of constructed value */
                *val = NULL;
                if (_BERDecConstructed(dec, len, infinite, &dd, &di))
                {
                    while (ASN1BERDecNotEndOfContents(dd, di))
                    {
                        if (ASN1BERDecZeroCharString(dd, 0x4, &c))
                        {
                            lv = My_lstrlenA(*val);
                            lc = My_lstrlenA(c);
                            if (lc)
                            {
                                /* resize value */
                                *val = (char *)DecMemReAlloc(dd, *val, lv + lc + 1);
                                if (*val)
                                {
                                    /* concat strings */
                                    RtlCopyMemory(*val + lv, c, lc + 1);

                                    /* free unused string */
                                    DecMemFree(dec, c);
                                }
                                else
                                {
                                    return 0;
                                }
                            }
                        }
                        else
                        {
                            return 0;
                        }
                    } // while
                    return ASN1BERDecEndOfContents(dec, dd, di);
                }
            }
            else
            {
                /* primitive? then copy value */
                *val = (char *)DecMemAlloc(dec, len + 1);
                if (*val)
                {
                    RtlCopyMemory(*val, dec->pos, len);
                    (*val)[len] = 0;
                    dec->pos += len;
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* decode zero terminated 16 bit string value */
int ASN1BERDecZeroChar16String(ASN1decoding_t dec, ASN1uint32_t tag, ASN1ztchar16string_t *val)
{
    ASN1uint32_t constructed, len, infinite;
    ASN1ztchar16string_t c;
    ASN1decoding_t dd;
    ASN1octet_t *di;
    ASN1uint32_t i;
    ASN1uint32_t lv, lc;

    /* skip tag */
    if (ASN1BERDecTag(dec, tag, &constructed))
    {
        /* get length */
        if (ASN1BERDecLength(dec, &len, &infinite))
        {
            if (constructed)
            {
                /* constructed? then start decoding of constructed value */
                *val = NULL;
                if (_BERDecConstructed(dec, len, infinite, &dd, &di))
                {
                    while (ASN1BERDecNotEndOfContents(dd, di))
                    {
                        if (ASN1BERDecZeroChar16String(dd, 0x4, &c))
                        {
                            lv = ASN1str16len(*val);
                            lc = ASN1str16len(c);
                            if (lc)
                            {
                                /* resize value */
                                *val = (ASN1char16_t *)DecMemReAlloc(dd, *val, (lv + lc + 1) * sizeof(ASN1char16_t));
                                if (*val)
                                {
                                    /* concat strings */
                                    RtlCopyMemory(*val + lv, c, (lc + 1) * sizeof(ASN1char16_t));

                                    /* free unused string */
                                    DecMemFree(dec, c);
                                }
                                else
                                {
                                    return 0;
                                }
                            }
                        }
                        else
                        {
                            return 0;
                        }
                    } // while
                    return ASN1BERDecEndOfContents(dec, dd, di);
                }
            }
            else
            {
                /* primitive? then copy value */
                *val = (ASN1char16_t *)DecMemAlloc(dec, (len + 1) * sizeof(ASN1char16_t));
                if (*val)
                {
                    for (i = 0; i < len; i++)
                    {
                        (*val)[i] = (*dec->pos << 8) | dec->pos[1];
                        dec->pos += 2;
                    }
                    (*val)[len] = 0;
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* decode zero terminated 32 bit string value */
int ASN1BERDecZeroChar32String(ASN1decoding_t dec, ASN1uint32_t tag, ASN1ztchar32string_t *val)
{
    ASN1uint32_t constructed, len, infinite;
    ASN1ztchar32string_t c;
    ASN1decoding_t dd;
    ASN1octet_t *di;
    ASN1uint32_t i;
    ASN1uint32_t lv, lc;

    /* skip tag */
    if (ASN1BERDecTag(dec, tag, &constructed))
    {
        /* get length */
        if (ASN1BERDecLength(dec, &len, &infinite))
        {
            if (constructed)
            {
                /* constructed? then start decoding of constructed value */
                *val = (ASN1char32_t *)DecMemAlloc(dec, sizeof(ASN1char32_t));
                if (*val)
                {
                    **val = 0;
                    if (_BERDecConstructed(dec, len, infinite, &dd, &di))
                    {
                        while (ASN1BERDecNotEndOfContents(dd, di))
                        {
                            if (ASN1BERDecZeroChar32String(dd, 0x4, &c))
                            {
                                lv = ASN1str32len(*val);
                                lc = ASN1str32len(c);
                                if (lc)
                                {
                                    /* resize value */
                                    *val = (ASN1char32_t *)DecMemReAlloc(dd, *val, (lv + lc + 1) * sizeof(ASN1char32_t));
                                    if (*val)
                                    {
                                        /* concat strings */
                                        RtlCopyMemory(*val + lv, c, (lc + 1) * sizeof(ASN1char32_t));

                                        /* free unused string */
                                        DecMemFree(dec, c);
                                    }
                                }
                            }
                            else
                            {
                                return 0;
                            }
                        }
                        return ASN1BERDecEndOfContents(dec, dd, di);
                    }
                }
            }
            else
            {
                /* primitive? then copy value */
                *val = (ASN1char32_t *)DecMemAlloc(dec, (len + 1) * sizeof(ASN1char32_t));
                if (*val)
                {
                    for (i = 0; i < len; i++)
                    {
                        (*val)[i] = (*dec->pos << 24) | (dec->pos[1] << 16) |
                                    (dec->pos[2] << 8) | dec->pos[3];;
                        dec->pos += 4;
                    }
                    (*val)[len] = 0;
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* skip a value */
int ASN1BERDecSkip(ASN1decoding_t dec)
{
    ASN1uint32_t tag;
    ASN1uint32_t constructed, len, infinite;
    ASN1decoding_t dd;
    ASN1octet_t *di;

    /* set warning flag */
    ASN1DecSetError(dec, ASN1_WRN_EXTENDED);

    /* read tag */
    if (ASN1BERDecPeekTag(dec, &tag))
    {
        if (ASN1BERDecTag(dec, tag, &constructed))
        {
            if (constructed)
            {
                /* constructed? then get length */
                if (ASN1BERDecLength(dec, &len, &infinite))
                {
                    if (!infinite)
                    {
                        /* skip value */
                        dec->pos += len;
                        // remove the above warning set previously
                        ASN1DecSetError(dec, ASN1_SUCCESS);
                        return 1;
                    } 

                    /* start skipping of constructed value */
                    if (_BERDecConstructed(dec, len, infinite, &dd, &di))
                    {
                        while (ASN1BERDecNotEndOfContents(dd, di))
                        {
                            if (ASN1BERDecSkip(dd))
                            {
                                continue;
                            }
                            return 0;
                        }
                        if (ASN1BERDecEndOfContents(dec, dd, di))
                        {
                            // remove the above warning set previously
                            ASN1DecSetError(dec, ASN1_SUCCESS);
                            return 1;
                        }
                        return 0;
                    }
                }
            }
            else
            {
                /* primitive? then get length */
                if (ASN1BERDecLength(dec, &len, NULL))
                {
                    /* skip value */
                    dec->pos += len;
                    // remove the above warning set previously
                    ASN1DecSetError(dec, ASN1_SUCCESS);
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* decode an open type value */
int _BERDecOpenType(ASN1decoding_t dec, ASN1open_t *val, ASN1uint32_t fNoCopy)
{
    ASN1uint32_t tag;
    ASN1uint32_t constructed, len, infinite;
    ASN1decoding_t dd;
    ASN1octet_t *di;
    ASN1octet_t *p;

    p = dec->pos;

    /* skip tag */
    if (ASN1BERDecPeekTag(dec, &tag))
    {
        if (ASN1BERDecTag(dec, tag, &constructed))
        {
            if (constructed)
            {
                /* constructed? then get length */
                if (ASN1BERDecLength(dec, &len, &infinite))
                {
                    if (!infinite)
                    {
                        /* skip value */
                        dec->pos += len;
                        goto MakeCopy;
                    } 

                    /* start decoding of constructed value */
                    if (_BERDecConstructed(dec, len, infinite, &dd, &di))
                    {
                        while (ASN1BERDecNotEndOfContents(dd, di))
                        {
                            if (ASN1BERDecSkip(dd))
                            {
                                continue;
                            }
                            return 0;
                        }
                        if (ASN1BERDecEndOfContents(dec, dd, di))
                        {
                            goto MakeCopy;
                        }
                    }
                }
                return 0;
            }
            else
            {
                /* primitive? then get length */
                if (ASN1BERDecLength(dec, &len, NULL))
                {
                    /* skip value */
                    dec->pos += len;
                }
                else
                {
                    return 0;
                }
            }

        MakeCopy:

            // clean up unused fields
            // val->decoded = NULL;
            // val->userdata = NULL;

            /* copy skipped value */
            val->length = (ASN1uint32_t) (dec->pos - p);
            if (fNoCopy)
            {
                val->encoded = p;
                return 1;
            }
            else
            {
                val->encoded = (ASN1octet_t *)DecMemAlloc(dec, val->length);
                if (val->encoded)
                {
                    RtlCopyMemory(val->encoded, p, val->length);
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* decode an open type value, making a copy */
int ASN1BERDecOpenType(ASN1decoding_t dec, ASN1open_t *val)
{
    return _BERDecOpenType(dec, val, FALSE);
}

/* decode an open type value, no copy */
int ASN1BERDecOpenType2(ASN1decoding_t dec, ASN1open_t *val)
{
    return _BERDecOpenType(dec, val, TRUE);
}

/* finish decoding */
int ASN1BERDecFlush(ASN1decoding_t dec)
{
    /* calculate length */
    dec->len = (ASN1uint32_t) (dec->pos - dec->buf);

    /* set WRN_NOEOD if data left */
    if (dec->len >= dec->size)
    {
        DecAssert(dec, dec->len == dec->size);
        return 1;
    }
    ASN1DecSetError(dec, ASN1_WRN_NOEOD);
    return 1;
}

#endif // ENABLE_BER


#ifdef ENABLE_BER

/* encode a string value */
int ASN1BEREncCharString(ASN1encoding_t enc, ASN1uint32_t tag, ASN1uint32_t len, ASN1char_t *val)
{
    /* encode tag */
    if (ASN1BEREncTag(enc, tag))
    {
        /* encode length */
        if (ASN1BEREncLength(enc, len))
        {
            RtlCopyMemory(enc->pos, val, len);
            enc->pos += len;
            return 1;
        }
    }
    return 0;
}

/* encode a string value (CER) */
int ASN1CEREncCharString(ASN1encoding_t enc, ASN1uint32_t tag, ASN1uint32_t len, ASN1char_t *val)
{
    ASN1uint32_t n;

    if (len <= 1000)
    {
        /* encode tag */
        if (ASN1BEREncTag(enc, tag))
        {
            /* encode length */
            if (ASN1BEREncLength(enc, len))
            {
                RtlCopyMemory(enc->pos, val, len);
                enc->pos += len;
                return 1;
            }
        }
    }
    else
    {
        ASN1uint32_t nLenOff;
        /* encode value as constructed, using segments of 1000 octets */
        if (ASN1BEREncExplicitTag(enc, tag, &nLenOff))
        {
            while (len)
            {
                n = len > 1000 ? 1000 : len;
                if (ASN1BEREncTag(enc, 0x4))
                {
                    if (ASN1BEREncLength(enc, n))
                    {
                        RtlCopyMemory(enc->pos, val, n);
                        enc->pos += n;
                        val += n;
                        len -= n;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    return 0;
                }
            }
            return ASN1BEREncEndOfContents(enc, nLenOff);
        }
    }
    return 0;
}

/* encode a 16 bit string value */
int ASN1BEREncChar16String(ASN1encoding_t enc, ASN1uint32_t tag, ASN1uint32_t len, ASN1char16_t *val)
{
    /* encode tag */
    if (ASN1BEREncTag(enc, tag))
    {
        /* encode length */
        if (ASN1BEREncLength(enc, len * sizeof(ASN1char16_t)))
        {
            while (len--)
            {
                *enc->pos++ = (ASN1octet_t)(*val >> 8);
                *enc->pos++ = (ASN1octet_t)(*val);
                val++;
            }
        }
        return 1;
    }
    return 0;
}

/* encode a 16 bit string value (CER) */
int ASN1CEREncChar16String(ASN1encoding_t enc, ASN1uint32_t tag, ASN1uint32_t len, ASN1char16_t *val)
{
    ASN1uint32_t n;

    if (len <= 1000 / sizeof(ASN1char16_t))
    {
        /* encode tag */
        if (ASN1BEREncTag(enc, tag))
        {
            /* encode length */
            if (ASN1BEREncLength(enc, len * sizeof(ASN1char16_t)))
            {
                while (len--)
                {
                    *enc->pos++ = (ASN1octet_t)(*val >> 8);
                    *enc->pos++ = (ASN1octet_t)(*val);
                    val++;
                }
                return 1;
            }
        }
    }
    else
    {
        ASN1uint32_t nLenOff;
        /* encode value as constructed, using segments of 1000 octets */
        if (ASN1BEREncExplicitTag(enc, tag, &nLenOff))
        {
            while (len)
            {
                n = len > 1000 / sizeof(ASN1char16_t) ?
                    1000 / sizeof(ASN1char16_t) : len;
                if (ASN1BEREncTag(enc, 0x4))
                {
                    if (ASN1BEREncLength(enc, n))
                    {
                        while (len--)
                        {
                            *enc->pos++ = (ASN1octet_t)(*val >> 8);
                            *enc->pos++ = (ASN1octet_t)(*val);
                            val++;
                        }
                        len -= n;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    return 0;
                }
            }
            return ASN1BEREncEndOfContents(enc, nLenOff);
        }
    }
    return 0;
}

/* encode a 32 bit string value */
int ASN1BEREncChar32String(ASN1encoding_t enc, ASN1uint32_t tag, ASN1uint32_t len, ASN1char32_t *val)
{
    /* encode tag */
    if (ASN1BEREncTag(enc, tag))
    {
        /* encode length */
        if (ASN1BEREncLength(enc, len * sizeof(ASN1char32_t)))
        {
            while (len--)
            {
                *enc->pos++ = (ASN1octet_t)(*val >> 24);
                *enc->pos++ = (ASN1octet_t)(*val >> 16);
                *enc->pos++ = (ASN1octet_t)(*val >> 8);
                *enc->pos++ = (ASN1octet_t)(*val);
                val++;
            }
            return 1;
        }
    }
    return 0;
}

/* encode a 32 bit string value (CER) */
int ASN1CEREncChar32String(ASN1encoding_t enc, ASN1uint32_t tag, ASN1uint32_t len, ASN1char32_t *val)
{
    ASN1uint32_t n;

    if (len <= 1000 / sizeof(ASN1char32_t))
    {
        /* encode tag */
        if (ASN1BEREncTag(enc, tag))
        {
            /* encode length */
            if (ASN1BEREncLength(enc, len * sizeof(ASN1char32_t)))
            {
                while (len--)
                {
                    *enc->pos++ = (ASN1octet_t)(*val >> 24);
                    *enc->pos++ = (ASN1octet_t)(*val >> 16);
                    *enc->pos++ = (ASN1octet_t)(*val >> 8);
                    *enc->pos++ = (ASN1octet_t)(*val);
                    val++;
                }
                return 1;
            }
        }
    }
    else
    {
        ASN1uint32_t nLenOff;
        /* encode value as constructed, using segments of 1000 octets */
        if (ASN1BEREncExplicitTag(enc, tag, &nLenOff))
        {
            while (len)
            {
                n = len > 1000 / sizeof(ASN1char32_t) ?
                    1000 / sizeof(ASN1char32_t) : len;
                if (ASN1BEREncTag(enc, 0x4))
                {
                    if (ASN1BEREncLength(enc, n))
                    {
                        while (len--)
                        {
                            *enc->pos++ = (ASN1octet_t)(*val >> 24);
                            *enc->pos++ = (ASN1octet_t)(*val >> 16);
                            *enc->pos++ = (ASN1octet_t)(*val >> 8);
                            *enc->pos++ = (ASN1octet_t)(*val);
                            val++;
                        }
                        len -= n;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    return 0;
                }
            } // while
            return ASN1BEREncEndOfContents(enc, nLenOff);
        }
    }
    return 0;
}

/* encode a bit string value */
int ASN1BEREncBitString(ASN1encoding_t enc, ASN1uint32_t tag, ASN1uint32_t len, ASN1octet_t *val)
{
    ASN1uint32_t noctets = (len + 7) / 8;

    /* encode tag */
    if (ASN1BEREncTag(enc, tag))
    {
        /* encode length */
        if (ASN1BEREncLength(enc, noctets + 1))
        {
            ASN1uint32_t cUnusedBits = (7 - ((len + 7) & 7));
            *enc->pos++ = (ASN1octet_t) cUnusedBits;
            RtlCopyMemory(enc->pos, val, noctets);
            enc->pos += noctets;
            EncAssert(enc, noctets >= 1);
            if (cUnusedBits)
            {
                EncAssert(enc, 8 >= cUnusedBits);
                *(enc->pos - 1) &= bitmsk2[8 - cUnusedBits];
            }
            return 1;
        }
    }
    return 0;
}

/* encode a bit string value (CER) */
int ASN1CEREncBitString(ASN1encoding_t enc, ASN1uint32_t tag, ASN1uint32_t len, ASN1octet_t *val)
{
    ASN1uint32_t noctets;
    ASN1uint32_t n;

    noctets = (len + 7) / 8;
    if (noctets + 1 <= 1000)
    {
        /* encode tag */
        if (ASN1BEREncTag(enc, tag))
        {
            /* encode length */
            if (ASN1BEREncLength(enc, noctets + 1))
            {
                *enc->pos++ = (ASN1octet_t) (7 - ((len + 7) & 7));
                RtlCopyMemory(enc->pos, val, noctets);
                enc->pos += noctets;
                return 1;
            }
        }
    }
    else
    {
        ASN1uint32_t nLenOff;
        /* encode value as constructed, using segments of 1000 octets */
        if (ASN1BEREncExplicitTag(enc, tag, &nLenOff))
        {
            while (noctets)
            {
                n = len > 999 ? 999 : len;
                if (ASN1BEREncTag(enc, 0x3))
                {
                    if (ASN1BEREncLength(enc, n + 1))
                    {
                        *enc->pos++ = (ASN1octet_t) (n < len ? 0 : 7 - ((len + 7) & 7));
                        RtlCopyMemory(enc->pos, val, n);
                        enc->pos += n;
                        val += n;
                        noctets -= n;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    return 0;
                }
            } // while
            return ASN1BEREncEndOfContents(enc, nLenOff);
        }
    }
    return 0;
}

#ifdef ENABLE_GENERALIZED_CHAR_STR
/* encode a character string value */
int ASN1BEREncCharacterString(ASN1encoding_t enc, ASN1uint32_t tag, ASN1characterstring_t *val)
{
    ASN1uint32_t index;
    ASN1uint32_t flag;

    /* search identification */
    if (!ASN1EncSearchCharacterStringIdentification(((ASN1INTERNencoding_t) enc)->parent,
        &val->identification, &index, &flag))
        return 0;
    if (index > 255)
        flag = 1;

    if (flag)
    {
        ASN1uint32_t nLenOff_, nLenOff0, nLenOff1;

        /* CS-A encoding: */
        /* encode as constructed value */
        if (!ASN1BEREncExplicitTag(enc, tag, &nLenOff_))
            return 0;

        /* encode index */
        if (!ASN1BEREncU32(enc, 0x80000000, index))
            return 0;

        /* encode tag of identification */
        if (!ASN1BEREncExplicitTag(enc, 0x80000001, &nLenOff0))
            return 0;

        /* encode identification */
        switch (val->identification.o)
        {
        case ASN1characterstring_identification_syntaxes_o:
            if (!ASN1BEREncExplicitTag(enc, 0x80000000, &nLenOff1))
                return 0;
            if (!ASN1BEREncObjectIdentifier(enc, 0x80000000,
                &val->identification.u.syntaxes.abstract))
                return 0;
            if (!ASN1BEREncObjectIdentifier(enc, 0x80000001,
                &val->identification.u.syntaxes.transfer))
                return 0;
            if (!ASN1BEREncEndOfContents(enc, nLenOff1))
                return 0;
            break;
        case ASN1characterstring_identification_syntax_o:
            if (!ASN1BEREncObjectIdentifier(enc, 0x80000001,
                &val->identification.u.syntax))
                return 0;
            break;
        case ASN1characterstring_identification_presentation_context_id_o:
            if (!ASN1BEREncU32(enc, 0x80000002,
                val->identification.u.presentation_context_id))
                return 0;
            break;
        case ASN1characterstring_identification_context_negotiation_o:
            if (!ASN1BEREncExplicitTag(enc, 0x80000003, &nLenOff1))
                return 0;
            if (!ASN1BEREncU32(enc, 0x80000000, val->
                identification.u.context_negotiation.presentation_context_id))
                return 0;
            if (!ASN1BEREncObjectIdentifier(enc, 0x80000001,
                &val->identification.u.context_negotiation.transfer_syntax))
                return 0;
            if (!ASN1BEREncEndOfContents(enc, nLenOff1))
                return 0;
            break;
        case ASN1characterstring_identification_transfer_syntax_o:
            if (!ASN1BEREncObjectIdentifier(enc, 0x80000004,
                &val->identification.u.transfer_syntax))
                return 0;
            break;
        case ASN1characterstring_identification_fixed_o:
            if (!ASN1BEREncNull(enc, 0x80000005))
                return 0;
            break;
        default:
            ASN1EncSetError(enc, ASN1_ERR_INTERNAL);
            return 0;
        }

        /* end of identification */
        if (!ASN1BEREncEndOfContents(enc, nLenOff0))
            return 0;

        /* encode data value */
        switch (val->data_value.o)
        {
        case ASN1characterstring_data_value_notation_o:
            if (!ASN1BEREncOctetString(enc, 0x80000002,
                val->data_value.u.notation.length,
                val->data_value.u.notation.encoded))
                return 0;
            break;
        case ASN1characterstring_data_value_encoded_o:
            if (!ASN1BEREncOctetString(enc, 0x80000002,
                val->data_value.u.encoded.length,
                val->data_value.u.encoded.value))
                return 0;
            break;
        default:
            ASN1EncSetError(enc, ASN1_ERR_INTERNAL);
            return 0;
        }

        /* end of character string */
        if (!ASN1BEREncEndOfContents(enc, nLenOff_))
            return 0;
    }
    else
    {
        /* CS-B encoding: */
        /* encode tag */
        if (!ASN1BEREncTag(enc, tag))
            return 0;

        /* encode data value */
        switch (val->data_value.o)
        {
        case ASN1characterstring_data_value_notation_o:
            if (!ASN1BEREncLength(enc,
                val->data_value.u.notation.length + 1))
                return 0;
            *enc->pos++ = (ASN1octet_t) index;
            RtlCopyMemory(enc->pos, val->data_value.u.notation.encoded,
                val->data_value.u.notation.length);
            enc->pos += val->data_value.u.notation.length;
            break;
        case ASN1characterstring_data_value_encoded_o:
            if (!ASN1BEREncLength(enc,
                val->data_value.u.encoded.length + 1))
                return 0;
            *enc->pos++ = (ASN1octet_t) index;
            RtlCopyMemory(enc->pos, val->data_value.u.encoded.value,
                val->data_value.u.encoded.length);
            enc->pos += val->data_value.u.encoded.length;
            break;
        default:
            ASN1EncSetError(enc, ASN1_ERR_INTERNAL);
            return 0;
        }
    }
    return 1;
}
#endif // ENABLE_GENERALIZED_CHAR_STR

#ifdef ENABLE_DOUBLE
/* encode a real value */
int ASN1BEREncDouble(ASN1encoding_t enc, ASN1uint32_t tag, double d)
{
    double mantissa;
    int exponent;
    ASN1uint32_t nmoctets;
    ASN1uint32_t neoctets;
    ASN1octet_t head;
    ASN1uint32_t sign;
    ASN1uint32_t len;
    ASN1octet_t mASN1octets[16]; /* should be enough */
    ASN1octet_t eASN1octets[16]; /* should be enough */

    /* encode tag */
    if (ASN1BEREncTag(enc, tag))
    {
        /* check for PLUS_INFINITY */
        if (ASN1double_ispinf(d))
        {
            /* encode length */
            if (ASN1BEREncLength(enc, 1))
            {
                /* encode value */
                *enc->pos++ = 0x40;
                return 1;
            }
        }
        else
        /* check for MINUS_INFINITY */
        if (ASN1double_isminf(d))
        {
            /* encode length */
            if (ASN1BEREncLength(enc, 1))
            {
                /* encode value */
                *enc->pos++ = 0x41;
                return 1;
            }
        }
        else
        /* check for bad real value */
        if (finite(d))
        {
            /* encode normal real value */

            /* split into mantissa and exponent */
            mantissa = frexp(d, &exponent);

            /* check for zero value */
            if (mantissa == 0.0 && exponent == 0)
            {
                /* encode zero length */
                return ASN1BEREncLength(enc, 0);
            }

            /* get sign bit */
            if (mantissa < 0.0)
            {
                sign = 1;
                mantissa = -mantissa;
            }
            else
            {
                sign = 0;
            }

            /* encode mantissa */
            nmoctets = 0;
            while (mantissa != 0.0 && nmoctets < sizeof(mASN1octets))
            {
                mantissa *= 256.0;
                exponent -= 8;
                mASN1octets[nmoctets++] = (int)mantissa;
                mantissa -= (double)(int)mantissa;
            }

            /* encode exponent and create head octet of encoded value */
            head = (ASN1octet_t) (0x80 | (sign << 6));
            if (exponent <= 0x7f && exponent >= -0x80)
            {
                eASN1octets[0] = (ASN1octet_t)(exponent);
                neoctets = 1;
            }
            else
            if (exponent <= 0x7fff && exponent >= -0x8000)
            {
                eASN1octets[0] = (ASN1octet_t)(exponent >> 8);
                eASN1octets[1] = (ASN1octet_t)(exponent);
                neoctets = 2;
                head |= 0x01;
            }
            else
            if (exponent <= 0x7fffff && exponent >= -0x800000)
            {
                eASN1octets[0] = (ASN1octet_t)(exponent >> 16);
                eASN1octets[1] = (ASN1octet_t)(exponent >> 8);
                eASN1octets[2] = (ASN1octet_t)(exponent);
                neoctets = 3;
                head |= 0x02;
            }
            else
            {
                eASN1octets[0] = 4; /* XXX does not work if ASN1int32_t != int */
                eASN1octets[1] = (ASN1octet_t)(exponent >> 24);
                eASN1octets[2] = (ASN1octet_t)(exponent >> 16);
                eASN1octets[3] = (ASN1octet_t)(exponent >> 8);
                eASN1octets[4] = (ASN1octet_t)(exponent);
                neoctets = 5;
                head |= 0x03;
            }

            /* encode length into first octet */
            len = 1 + neoctets + nmoctets;
            if (ASN1BEREncLength(enc, len))
            {
                /* put head octet, mantissa and exponent */
                *enc->pos++ = head;
                RtlCopyMemory(enc->pos, eASN1octets, neoctets);
                enc->pos += neoctets;
                RtlCopyMemory(enc->pos, mASN1octets, nmoctets);
                enc->pos += nmoctets;
                return 1;
            }
        }
        else
        {
            ASN1EncSetError(enc, ASN1_ERR_BADREAL);
        }
    }
    /* finished */
    return 0;
}
#endif // ENABLE_DOUBLE

#ifdef ENABLE_REAL
/* encode a real value */
int ASN1BEREncReal(ASN1encoding_t enc, ASN1uint32_t tag, ASN1real_t *val)
{
    ASN1intx_t mantissa;
    ASN1intx_t exponent;
    ASN1intx_t help;
    ASN1uint32_t nmoctets;
    ASN1uint32_t neoctets;
    ASN1octet_t head;
    ASN1uint32_t sign;
    ASN1uint32_t len;
    ASN1octet_t mASN1octets[256]; /* should be enough */
    ASN1octet_t eASN1octets[256]; /* should be enough */

    /* encode tag */
    if (ASN1BEREncTag(enc, tag))
    {
        /* check for PLUS_INFINITY */
        if (val->type == eReal_PlusInfinity)
        {
            /* encode length */
            if (ASN1BEREncLength(enc, 1))
            {
                /* encode value */
                *enc->pos++ = 0x40;
                return 1;
            }
        }
        else
        /* check for MINUS_INFINITY */
        if (val->type == eReal_MinusInfinity)
        {
            /* encode length */
            if (ASN1BEREncLength(enc, 1))
            {
                /* encode value */
                *enc->pos++ = 0x41;
                return 1;
            }
        }
        /* encode normal real value */
        else
        {
            /* check for zero value */
            if (!ASN1intx_cmp(&val->mantissa, &ASN1intx_0))
            {
                /* encode zero length */
                return ASN1BEREncLength(enc, 0);
            }

            /* get sign bit */
            if (val->mantissa.value[0] > 0x7f)
            {
                sign = 1;
                ASN1intx_neg(&mantissa, &val->mantissa);
            }
            else
            {
                sign = 0;
                if (! ASN1intx_dup(&mantissa, &val->mantissa))
                {
                    return 0;
                }
            }
            if (! ASN1intx_dup(&exponent, &val->exponent))
            {
                return 0;
            }

            /* encode mantissa */
            nmoctets = ASN1intx_uoctets(&mantissa);
            if (nmoctets < 256)
            {
                RtlCopyMemory(mASN1octets,
                    mantissa.value + mantissa.length - nmoctets,
                    nmoctets);
                ASN1intx_setuint32(&help, 8 * nmoctets);
                ASN1intx_sub(&exponent, &exponent, &help);
                ASN1intx_free(&mantissa);
                ASN1intx_free(&help);

                /* encode exponent and create head octet of encoded value */
                neoctets = ASN1intx_octets(&exponent);
                if (neoctets < 256)
                {
                    RtlCopyMemory(mASN1octets,
                        val->exponent.value + val->exponent.length - neoctets,
                        neoctets);
                    ASN1intx_free(&exponent);
                    head = (ASN1octet_t) (0x80 | (sign << 6) | (neoctets - 1));

                    /* encode length into first octet */
                    len = 1 + neoctets + nmoctets;
                    if (ASN1BEREncLength(enc, len))
                    {
                        /* put head octet, mantissa and exponent */
                        *enc->pos++ = head;
                        RtlCopyMemory(enc->pos, eASN1octets, neoctets);
                        enc->pos += neoctets;
                        RtlCopyMemory(enc->pos, mASN1octets, nmoctets);
                        enc->pos += nmoctets;
                        return 1;
                    }
                }
                else
                {
                    ASN1intx_free(&exponent);
                    ASN1EncSetError(enc, ASN1_ERR_LARGE);
                }
            }
            else
            {
                ASN1intx_free(&mantissa);
                ASN1intx_free(&help);
                ASN1EncSetError(enc, ASN1_ERR_LARGE);
            }
        }
    }
    /* finished */
    return 0;
}
#endif // ENABLE_REAL

#ifdef ENABLE_EMBEDDED_PDV
/* encode an embedded pdv value */
int ASN1BEREncEmbeddedPdv(ASN1encoding_t enc, ASN1uint32_t tag, ASN1embeddedpdv_t *val)
{
    ASN1uint32_t index;
    ASN1uint32_t flag;

    /* search identification */
    if (!ASN1EncSearchEmbeddedPdvIdentification(((ASN1INTERNencoding_t) enc)->parent,
        &val->identification, &index, &flag))
        return 0;
    if (index > 255 ||
            (val->data_value.o == ASN1embeddedpdv_data_value_encoded_o &&
        (val->data_value.u.encoded.length & 7))) {
        flag = 1;
    }
        
    if (flag)
    {
        ASN1uint32_t nLenOff_, nLenOff0, nLenOff1;

        /* EP-A encoding: */
        /* encode as construct value */
        if (!ASN1BEREncExplicitTag(enc, tag, &nLenOff_))
            return 0;

        /* encode index */
        if (!ASN1BEREncU32(enc, 0x80000000, index))
            return 0;

        /* encode tag of identification */
        if (!ASN1BEREncExplicitTag(enc, 0x80000001, &nLenOff0))
            return 0;

        /* encode identification */
        switch (val->identification.o)
        {
        case ASN1embeddedpdv_identification_syntaxes_o:
            if (!ASN1BEREncExplicitTag(enc, 0x80000000, &nLenOff1))
                return 0;
            if (!ASN1BEREncObjectIdentifier(enc, 0x80000000,
                &val->identification.u.syntaxes.abstract))
                return 0;
            if (!ASN1BEREncObjectIdentifier(enc, 0x80000001,
                &val->identification.u.syntaxes.transfer))
                return 0;
            if (!ASN1BEREncEndOfContents(enc, nLenOff1))
                return 0;
            break;
        case ASN1embeddedpdv_identification_syntax_o:
            if (!ASN1BEREncObjectIdentifier(enc, 0x80000001,
                &val->identification.u.syntax))
                return 0;
            break;
        case ASN1embeddedpdv_identification_presentation_context_id_o:
            if (!ASN1BEREncU32(enc, 0x80000002,
                val->identification.u.presentation_context_id))
                return 0;
            break;
        case ASN1embeddedpdv_identification_context_negotiation_o:
            if (!ASN1BEREncExplicitTag(enc, 0x80000003, &nLenOff1))
                return 0;
            if (!ASN1BEREncU32(enc, 0x80000000, val->
                identification.u.context_negotiation.presentation_context_id))
                return 0;
            if (!ASN1BEREncObjectIdentifier(enc, 0x80000001,
                &val->identification.u.context_negotiation.transfer_syntax))
                return 0;
            if (!ASN1BEREncEndOfContents(enc, nLenOff1))
                return 0;
            break;
        case ASN1embeddedpdv_identification_transfer_syntax_o:
            if (!ASN1BEREncObjectIdentifier(enc, 0x80000004,
                &val->identification.u.transfer_syntax))
                return 0;
            break;
        case ASN1embeddedpdv_identification_fixed_o:
            if (!ASN1BEREncNull(enc, 0x80000005))
                return 0;
            break;
        default:
            ASN1EncSetError(enc, ASN1_ERR_INTERNAL);
            return 0;
        }

        /* end of identification */
        if (!ASN1BEREncEndOfContents(enc, nLenOff0))
            return 0;

        /* encode data value */
        switch (val->data_value.o)
        {
        case ASN1embeddedpdv_data_value_notation_o:
            if (!ASN1BEREncBitString(enc, 0x80000002,
                val->data_value.u.notation.length * 8,
                val->data_value.u.notation.encoded))
                return 0;
            break;
        case ASN1embeddedpdv_data_value_encoded_o:
            if (!ASN1BEREncBitString(enc, 0x80000002,
                val->data_value.u.encoded.length,
                val->data_value.u.encoded.value))
                return 0;
            break;
        default:
            ASN1EncSetError(enc, ASN1_ERR_INTERNAL);
            return 0;
        }

        /* end of embedded pdv */
        if (!ASN1BEREncEndOfContents(enc, nLenOff_))
            return 0;
    }
    else
    {
        /* EP-B encoding: */
        /* encode tag */
        if (!ASN1BEREncTag(enc, tag))
            return 0;

        /* encode data value */
        switch (val->data_value.o)
        {
        case ASN1embeddedpdv_data_value_notation_o:
            if (!ASN1BEREncLength(enc,
                val->data_value.u.notation.length + 1))
                return 0;
            *enc->pos++ = (ASN1octet_t) index;
            RtlCopyMemory(enc->pos, val->data_value.u.notation.encoded,
                val->data_value.u.notation.length);
            enc->pos += val->data_value.u.notation.length;
            break;
        case ASN1embeddedpdv_data_value_encoded_o:
            if (!ASN1BEREncLength(enc,
                val->data_value.u.encoded.length / 8 + 1))
                return 0;
            *enc->pos++ = (ASN1octet_t) index;
            RtlCopyMemory(enc->pos, val->data_value.u.encoded.value,
                val->data_value.u.encoded.length / 8);
            enc->pos += val->data_value.u.encoded.length / 8;
            break;
        default:
            ASN1EncSetError(enc, ASN1_ERR_INTERNAL);
            return 0;
        }
    }
    return 1;
}
#endif // ENABLE_EMBEDDED_PDV


#ifdef ENABLE_EXTERNAL
/* encode an external value */
int ASN1BEREncExternal(ASN1encoding_t enc, ASN1uint32_t tag, ASN1external_t *val)
{
    ASN1uint32_t t;
    ASN1uint32_t nLenOff_, nLenOff0;

    if (!val->data_value_descriptor)
        val->o[0] &= ~0x80;

    /* encode tag */
    if (!ASN1BEREncExplicitTag(enc, tag, &nLenOff_))
        return 0;

    /* encode identification */
    switch (val->identification.o) {
    case ASN1external_identification_syntax_o:
        if (!ASN1BEREncObjectIdentifier(enc, 0x6,
            &val->identification.u.syntax))
            return 0;
        break;
    case ASN1external_identification_presentation_context_id_o:
        if (!ASN1BEREncU32(enc, 0x2,
            val->identification.u.presentation_context_id))
            return 0;
        break;
    case ASN1external_identification_context_negotiation_o:
        if (!ASN1BEREncObjectIdentifier(enc, 0x6,
            &val->identification.u.context_negotiation.transfer_syntax))
            return 0;
        if (!ASN1BEREncU32(enc, 0x2,
            val->identification.u.context_negotiation.presentation_context_id))
            return 0;
        break;
    default:
        ASN1EncSetError(enc, ASN1_ERR_INTERNAL);
        return 0;
    }

    /* encode data value descriptor if present */
    if (val->o[0] & 0x80) {
        t = My_lstrlenA(val->data_value_descriptor);
        if (!ASN1BEREncCharString(enc, 0x7, t, val->data_value_descriptor))
            return 0;
    }

    /* encode data value */
    switch (val->data_value.o)
    {
    case ASN1external_data_value_notation_o:
        if (!ASN1BEREncExplicitTag(enc, 0, &nLenOff0))
            return 0;
        if (!ASN1BEREncOpenType(enc, &val->data_value.u.notation))
            return 0;
        if (!ASN1BEREncEndOfContents(enc, nLenOff0))
            return 0;
        break;
    case ASN1external_data_value_encoded_o:
        if (!(val->data_value.u.encoded.length & 7))
        {
            if (!ASN1BEREncExplicitTag(enc, 1, &nLenOff0))
                return 0;
            if (!ASN1BEREncOctetString(enc, 0x4,
                val->data_value.u.encoded.length / 8,
                val->data_value.u.encoded.value))
                return 0;
            if (!ASN1BEREncEndOfContents(enc, nLenOff0))
                return 0;
        }
        else
        {
            if (!ASN1BEREncExplicitTag(enc, 2, &nLenOff0))
                return 0;
            if (!ASN1BEREncBitString(enc, 0x3,
                val->data_value.u.encoded.length,
                val->data_value.u.encoded.value))
                return 0;
            if (!ASN1BEREncEndOfContents(enc, nLenOff0))
                return 0;
        }
        break;
    default:
        ASN1EncSetError(enc, ASN1_ERR_INTERNAL);
        return 0;
    }

    /* end of external value */
    return ASN1BEREncEndOfContents(enc, nLenOff_);
}
#endif // ENABLE_EXTERNAL

/* encode a generalized time value */
int ASN1BEREncGeneralizedTime(ASN1encoding_t enc, ASN1uint32_t tag, ASN1generalizedtime_t *val)
{
    char time[32];
    time[0] = '\0';
    ASN1generalizedtime2string(time, val);
    return ASN1BEREncCharString(enc, tag, My_lstrlenA(time), time);
}

/* encode a generalized time value (CER) */
int ASN1CEREncGeneralizedTime(ASN1encoding_t enc, ASN1uint32_t tag, ASN1generalizedtime_t *val)
{
    char time[32];
    time[0] = '\0';
    ASN1generalizedtime2string(time, val);
    return ASN1CEREncCharString(enc, tag, My_lstrlenA(time), time);
}

/* encode a signed integer value */
int ASN1BEREncS32(ASN1encoding_t enc, ASN1uint32_t tag, ASN1int32_t val)
{
    if (ASN1BEREncTag(enc, tag))
    {
        if (val >= -0x8000 && val < 0x8000)
        {
            if (val >= -0x80 && val < 0x80)
            {
                if (ASN1BEREncLength(enc, 1))
                {
                    *enc->pos++ = (ASN1octet_t)(val);
                    return 1;
                }
            }
            else
            {
                if (ASN1BEREncLength(enc, 2))
                {
                    *enc->pos++ = (ASN1octet_t)(val >> 8);
                    *enc->pos++ = (ASN1octet_t)(val);
                    return 1;
                }
            }
        }
        else
        {
            if (val >= -0x800000 && val < 0x800000)
            {
                if (ASN1BEREncLength(enc, 3))
                {
                    *enc->pos++ = (ASN1octet_t)(val >> 16);
                    *enc->pos++ = (ASN1octet_t)(val >> 8);
                    *enc->pos++ = (ASN1octet_t)(val);
                    return 1;
                }
            }
            else
            {
                if (ASN1BEREncLength(enc, 4))
                {
                    *enc->pos++ = (ASN1octet_t)(val >> 24);
                    *enc->pos++ = (ASN1octet_t)(val >> 16);
                    *enc->pos++ = (ASN1octet_t)(val >> 8);
                    *enc->pos++ = (ASN1octet_t)(val);
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* encode a intx_t integer value */
int ASN1BEREncSX(ASN1encoding_t enc, ASN1uint32_t tag, ASN1intx_t *val)
{
    ASN1uint32_t cb;
    ASN1octet_t *p;

    /* encode tag */
    if (ASN1BEREncTag(enc, tag))
    {
        // strip out leading 0 and ff.
        for (cb = val->length, p = val->value; cb > 1; cb--, p++)
        {
			// break if not 00 nor FF
            if (*p && *p != 0xFF)
            {
                break;
            }
			// break if 00 FF
			if ((! *p) && (*(p+1) & 0x80))
			{
				break;
			}
			// break if FF 7F
			if (*p == 0xFF && (!(*(p+1) & 0x80)))
			{
				break;
			}
        }

        /* encode length */
        if (ASN1BEREncLength(enc, cb))
        {
            /* copy value */
            RtlCopyMemory(enc->pos, p, cb);
            enc->pos += cb;
            return 1;
        }
    }
    return 0;
}

/* encode a multibyte string value */
int ASN1BEREncZeroMultibyteString(ASN1encoding_t enc, ASN1uint32_t tag, ASN1ztcharstring_t val)
{
    return ASN1BEREncCharString(enc, tag, My_lstrlenA(val), val);
}

int ASN1BEREncMultibyteString(ASN1encoding_t enc, ASN1uint32_t tag, ASN1charstring_t *val)
{
    return ASN1BEREncCharString(enc, tag, val->length, val->value);
}

/* encode a multibyte string value (CER) */
int ASN1CEREncZeroMultibyteString(ASN1encoding_t enc, ASN1uint32_t tag, ASN1ztcharstring_t val)
{
    return ASN1CEREncCharString(enc, tag, My_lstrlenA(val), val);
}

int ASN1CEREncMultibyteString(ASN1encoding_t enc, ASN1uint32_t tag, ASN1charstring_t *val)
{
    return ASN1CEREncCharString(enc, tag, val->length, val->value);
}

/* encode a null value */
int ASN1BEREncNull(ASN1encoding_t enc, ASN1uint32_t tag)
{
    /* encode tag */
    if (ASN1BEREncTag(enc, tag))
    {
        /* encode zero length */
        return ASN1BEREncLength(enc, 0);
    }
    return 0;
}

// encode an oid node s to buffer pointed by p
ASN1octet_t *_BEREncOidNode(ASN1octet_t *p, ASN1uint32_t s)
{
    if (s < 0x80)
    {
        *p++ = (ASN1octet_t)(s);
    }
    else
    if (s < 0x4000)
    {
        *p++ = (ASN1octet_t)((s >> 7) | 0x80);
        *p++ = (ASN1octet_t)(s & 0x7f);
    }
    else
    if (s < 0x200000)
    {
        *p++ = (ASN1octet_t)((s >> 14) | 0x80);
        *p++ = (ASN1octet_t)((s >> 7) | 0x80);
        *p++ = (ASN1octet_t)(s & 0x7f);
    }
    else
    if (s < 0x10000000)
    {
        *p++ = (ASN1octet_t)((s >> 21) | 0x80);
        *p++ = (ASN1octet_t)((s >> 14) | 0x80);
        *p++ = (ASN1octet_t)((s >> 7) | 0x80);
        *p++ = (ASN1octet_t)(s & 0x7f);
    }
    else
    {
        *p++ = (ASN1octet_t)((s >> 28) | 0x80);
        *p++ = (ASN1octet_t)((s >> 21) | 0x80);
        *p++ = (ASN1octet_t)((s >> 14) | 0x80);
        *p++ = (ASN1octet_t)((s >> 7) | 0x80);
        *p++ = (ASN1octet_t)(s & 0x7f);
    }
    return p;
}

/* encode an object identifier value */
int ASN1BEREncObjectIdentifier(ASN1encoding_t enc, ASN1uint32_t tag, ASN1objectidentifier_t *val)
{
    /* encode tag */
    if (ASN1BEREncTag(enc, tag))
    {
        ASN1objectidentifier_t obj = *val;
        ASN1uint32_t i, s, l;
        ASN1octet_t *data, *p;

        l = GetObjectIdentifierCount(obj);
        if (l)
        {
            /* convert object identifier to octets */
            p = data = (ASN1octet_t *)EncMemAlloc(enc, l * 5); /* max. 5 octets/subelement */
            if (p)
            {
                int rc;
                for (i = 0; i < l; i++)
                {
                    s = obj->value;
                    obj = obj->next;
                    if (!i && l > 1)
                    {
                        s = s * 40 + obj->value;
                        obj = obj->next;
                        i++;
                    }
                    p = _BEREncOidNode(p, s);
                } // for

                /* encode length */
                rc = ASN1BEREncLength(enc, (ASN1uint32_t) (p - data));
                if (rc)
                {
                    /* copy value */
                    RtlCopyMemory(enc->pos, data, p - data);
                    enc->pos += p - data;
                }
                MemFree(data);
                return rc;
            }
            ASN1EncSetError(enc, ASN1_ERR_MEMORY);
            return 0;
        } // if (l)
        /* encode zero length */
        return ASN1BEREncLength(enc, 0);
    }
    return 0;
}

/* encode an object identifier value */
int ASN1BEREncObjectIdentifier2(ASN1encoding_t enc, ASN1uint32_t tag, ASN1objectidentifier2_t *val)
{
    /* encode tag */
    if (ASN1BEREncTag(enc, tag))
    {
        ASN1uint32_t i, s;
        ASN1octet_t *data, *p;

        if (val->count)
        {
            /* convert object identifier to octets */
            p = data = (ASN1octet_t *)EncMemAlloc(enc, val->count * 5); /* max. 5 octets/subelement */
            if (p)
            {
                int rc;
                for (i = 0; i < val->count; i++)
                {
                    s = val->value[i];
                    if (!i && val->count > 1)
                    {
                        i++;
                        s = s * 40 + val->value[i];
                    }
                    p = _BEREncOidNode(p, s);
                } // for

                /* encode length */
                rc = ASN1BEREncLength(enc, (ASN1uint32_t) (p - data));
                if (rc)
                {
                    /* copy value */
                    RtlCopyMemory(enc->pos, data, p - data);
                    enc->pos += p - data;
                }
                MemFree(data);
                return rc;
            }
            ASN1EncSetError(enc, ASN1_ERR_MEMORY);
            return 0;
        } // if (l)
        /* encode zero length */
        return ASN1BEREncLength(enc, 0);
    }
    return 0;
}

/* encode an octet string value (CER) */
int ASN1CEREncOctetString(ASN1encoding_t enc, ASN1uint32_t tag, ASN1uint32_t len, ASN1octet_t *val)
{
    ASN1uint32_t n;

    if (len <= 1000)
    {
        /* encode tag */
        if (ASN1BEREncTag(enc, tag))
        {
            /* encode length */
            if (ASN1BEREncLength(enc, len))
            {
                /* copy value */
                RtlCopyMemory(enc->pos, val, len);
                enc->pos += len;
                return 1;
            }
        }
    }
    else
    {
        ASN1uint32_t nLenOff;
        /* encode value as constructed, using segments of 1000 octets */
        if (ASN1BEREncExplicitTag(enc, tag, &nLenOff))
        {
            while (len)
            {
                n = len > 1000 ? 1000 : len;
                if (ASN1BEREncTag(enc, 0x4))
                {
                    if (ASN1BEREncLength(enc, n))
                    {
                        RtlCopyMemory(enc->pos, val, n);
                        enc->pos += n;
                        val += n;
                        len -= n;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    return 0;
                }
            }
            return ASN1BEREncEndOfContents(enc, nLenOff);
        }
    }
    return 0;
}

/* encode an open type value */
int ASN1BEREncOpenType(ASN1encoding_t enc, ASN1open_t *val)
{
    if (ASN1BEREncCheck(enc, val->length))
    {
        /* copy value */
        RtlCopyMemory(enc->pos, val->encoded, val->length);
        enc->pos += val->length;
        return 1;
    }
    return 0;
}

/* remove trailing zero bits from bit string */
int ASN1BEREncRemoveZeroBits(ASN1uint32_t *nbits, ASN1octet_t *val)
{
    ASN1uint32_t n;
    int i;

    /* get value */
    n = *nbits;

    /* let val point to last ASN1octet used */
    val += (n - 1) / 8;

    /* check if broken ASN1octet consist out of zero bits */
    if ((n & 7) && !(*val & bitmsk2[n & 7]))
    {
        n &= ~7;
        val--;
    }

    /* scan complete ASN1octets (memcchr missing ...) */
    if (!(n & 7))
    {
        while (n && !*val)
        {
            n -= 8;
            val--;
        }
    }

    /* scan current ASN1octet bit after bit */
    if (n)
    {
        for (i = (n - 1) & 7; i >= 0; i--)
        {
            if (*val & (0x80 >> i))
                break;
            n--;
        }
    }

    /* return real bitstring len */
    *nbits = n;
    return 1;
}

/* encode an utc time value */
int ASN1BEREncUTCTime(ASN1encoding_t enc, ASN1uint32_t tag, ASN1utctime_t *val)
{
    char time[32];
    time[0] = '\0';
    ASN1utctime2string(time, val);
    return ASN1BEREncCharString(enc, tag, My_lstrlenA(time), time);
}

/* encode an utc time value (CER) */
int ASN1CEREncUTCTime(ASN1encoding_t enc, ASN1uint32_t tag, ASN1utctime_t *val)
{
    char time[32];
    time[0] = '\0';
    ASN1utctime2string(time, val);
    return ASN1CEREncCharString(enc, tag, My_lstrlenA(time), time);
}

/* end of encoding */
int ASN1BEREncFlush(ASN1encoding_t enc)
{
    /* allocate at least one octet */
    if (enc->buf)
    {
        /* fill in zero-octet if encoding is empty bitstring */
        if (enc->buf == enc->pos)
            *enc->pos++ = 0;

        /* calculate length */
        enc->len = (ASN1uint32_t) (enc->pos - enc->buf);
        return 1;
    }
    return ASN1BEREncCheck(enc, 1);
}

#endif // ENABLE_BER

/* get the expected length based on the table */
ASN1uint32_t _BERGetLength(ASN1uint32_t val, const ASN1uint32_t Tbl[], ASN1uint32_t cItems)
{
    ASN1uint32_t i;
    for (i = 0; i < cItems; i++)
    {
        if (val < Tbl[i])
            return i+1;
    }
    return cItems+1;
}

static const ASN1uint32_t c_TagTable[] = { 31, 0x80, 0x4000, 0x200000, 0x10000000 };

/* encode a tag */
int ASN1BEREncTag(ASN1encoding_t enc, ASN1uint32_t tag)
{
    ASN1uint32_t tagclass, tagvalue, cbTagLength;

    tagclass = (tag >> 24) & 0xe0;
    tagvalue = tag & 0x1fffffff;

    cbTagLength = _BERGetLength(tagvalue, c_TagTable, ARRAY_SIZE(c_TagTable));
    if (ASN1BEREncCheck(enc, cbTagLength))
    {
        if (cbTagLength == 1)
        {
            *enc->pos++ = (ASN1octet_t)(tagclass | tagvalue);
        }
        else
        {
            *enc->pos++ = (ASN1octet_t)(tagclass | 0x1f);
            switch (cbTagLength)
            {
            case 6:
                *enc->pos++ = (ASN1octet_t)((tagvalue >> 28) | 0x80);
                // lonchanc: intentionally fall through
            case 5:
                *enc->pos++ = (ASN1octet_t)((tagvalue >> 21) | 0x80);
                // lonchanc: intentionally fall through
            case 4:
                *enc->pos++ = (ASN1octet_t)((tagvalue >> 14) | 0x80);
                // lonchanc: intentionally fall through
            case 3:
                *enc->pos++ = (ASN1octet_t)((tagvalue >> 7) | 0x80);
                // lonchanc: intentionally fall through
            case 2:
                *enc->pos++ = (ASN1octet_t)(tagvalue & 0x7f);
                break;
            }
        }
        return 1;
    }
    return 0;
}

/* put the length value */
void _BERPutLength(ASN1encoding_t enc, ASN1uint32_t len, ASN1uint32_t cbLength)
{
    if (cbLength > 1)
    {
        *enc->pos++ = (ASN1octet_t) (0x7f + cbLength); // 0x80 + cbLength - 1;
    }

    switch (cbLength)
    {
    case 5:
        *enc->pos++ = (ASN1octet_t)(len >> 24);
        // lonchanc: intentionally fall through
    case 4:
        *enc->pos++ = (ASN1octet_t)(len >> 16);
        // lonchanc: intentionally fall through
    case 3:
        *enc->pos++ = (ASN1octet_t)(len >> 8);
        // lonchanc: intentionally fall through
    default: // case 2: case 1:
        *enc->pos++ = (ASN1octet_t)len;
        break;
    }
}

static const ASN1uint32_t c_LengthTable[] = { 0x80, 0x100, 0x10000, 0x1000000 };

/* encode length */
int ASN1BEREncLength(ASN1encoding_t enc, ASN1uint32_t len)
{
    ASN1uint32_t cbLength = _BERGetLength(len, c_LengthTable, ARRAY_SIZE(c_LengthTable));

    if (ASN1BEREncCheck(enc, cbLength + len))
    {
        _BERPutLength(enc, len, cbLength);
        return 1;
    }
    return 0;
}

/* encode an octet string value */
int ASN1BEREncOctetString(ASN1encoding_t enc, ASN1uint32_t tag, ASN1uint32_t len, ASN1octet_t *val)
{
    /* encode tag */
    if (ASN1BEREncTag(enc, tag))
    {
        /* encode length */
        if (ASN1BEREncLength(enc, len))
        {
            /* copy value */
            RtlCopyMemory(enc->pos, val, len);
            enc->pos += len;
            return 1;
        }
    }
    return 0;
}

/* encode a boolean value */
int ASN1BEREncBool(ASN1encoding_t enc, ASN1uint32_t tag, ASN1bool_t val)
{
    if (ASN1BEREncTag(enc, tag))
    {
        if (ASN1BEREncLength(enc, 1))
        {
            *enc->pos++ = val ? 0xFF : 0;
            return 1;
        }
    }
    return 0;
}

static const ASN1uint32_t c_U32LengthTable[] = { 0x80, 0x8000, 0x800000, 0x80000000 };

/* encode a unsigned integer value */
int ASN1BEREncU32(ASN1encoding_t enc, ASN1uint32_t tag, ASN1uint32_t val)
{
    EncAssert(enc, tag != 0x01);
    if (ASN1BEREncTag(enc, tag))
    {
        ASN1uint32_t cbLength;
        cbLength = _BERGetLength(val, c_U32LengthTable, ARRAY_SIZE(c_U32LengthTable));
        if (ASN1BEREncLength(enc, cbLength))
        {
            switch (cbLength)
            {
            case 5:
                *enc->pos++ = 0;
                // lonchanc: intentionally fall through
            case 4:
                *enc->pos++ = (ASN1octet_t)(val >> 24);
                // lonchanc: intentionally fall through
            case 3:
                *enc->pos++ = (ASN1octet_t)(val >> 16);
                // lonchanc: intentionally fall through
            case 2:
                *enc->pos++ = (ASN1octet_t)(val >> 8);
                // lonchanc: intentionally fall through
            case 1:
                *enc->pos++ = (ASN1octet_t)(val);
                break;
            }
            return 1;
        }
    }
    return 0;
}


// THE FOLLOWING IS FROM BERDECOD.C


/* check if len octets are left in decoding stream */
int ASN1BERDecCheck(ASN1decoding_t dec, ASN1uint32_t len)
{
    if (dec->pos + len <= dec->buf + dec->size)
    {
        return 1;
    }

    ASN1DecSetError(dec, ASN1_ERR_EOD);
    return 0;
}

int _BERDecPeekCheck(ASN1decoding_t dec, ASN1uint32_t len)
{
    return ((dec->pos + len <= dec->buf + dec->size) ? 1 : 0);
}

/* start decoding of a constructed value */
int _BERDecConstructed(ASN1decoding_t dec, ASN1uint32_t len, ASN1uint32_t infinite, ASN1decoding_t *dd, ASN1octet_t **ppBufEnd)
{
    // safety net
    DecAssert(dec, NULL != dd);
    if (dd != NULL)
    {
        *dd = dec;
    }

#ifdef XBOX_SPECIFIC_OPTIMIZATIONS
    // lonchanc: this does not work because open type can be the last component and
    // the open type decoder needs to peek a tag. as a result, we may peek the tag
    // outside the buffer boundary.
    if (ppBufEnd && (! infinite))
    {
        *ppBufEnd = dec->pos + len;
        return 1;
    }
#endif

    /* initialize a new decoding stream as child of running decoding stream */
    if (ASN1_CreateDecoder(dec->pScratchMemoryAllocator, dec->module, dd,
        dec->pos, infinite ? dec->size - (ASN1uint32_t) (dec->pos - dec->buf) : len, dec) >= 0)
    {
        /* set pointer to end of decoding stream if definite length case selected */
        *ppBufEnd = infinite ? NULL : (*dd)->buf + (*dd)->size;
        return 1;
    }

    return 0;
}

/* decode a tag value; return constructed bit if desired */
int ASN1BERDecTag(ASN1decoding_t dec, ASN1uint32_t tag, ASN1uint32_t *constructed)
{
    ASN1uint32_t tagvalue, tagclass, c;

    /* get tag class and value */
    if (ASN1BERDecCheck(dec, 1))
    {
        tagclass = *dec->pos & 0xe0;
        tagvalue = *dec->pos++ & 0x1f;
        if (tagvalue == 0x1f)
        {
            tagvalue = 0;
            do {
                if (ASN1BERDecCheck(dec, 1))
                {
                    c = *dec->pos++;
                    if (! (tagvalue & 0xe0000000))
                    {
                        tagvalue = (tagvalue << 7) | (c & 0x7f);
                    }
                    else
                    {
                        ASN1DecSetError(dec, ASN1_ERR_BADTAG);
                        return 0;
                    }
                }
                else
                {
                    return 0;
                }
            } while (c & 0x80);
        }

        /* extract constructed bit if wanted */
        if (constructed)
        {
            *constructed = tagclass & 0x20;
            tagclass &= ~0x20;
        }

        /* check if tag equals desires */
        if (tag == ((tagclass << 24) | tagvalue))
        {
            return 1;
        }

        ASN1DecSetError(dec, ASN1_ERR_BADTAG);
    }
    return 0;
}

/* decode length */
int ASN1BERDecLength(ASN1decoding_t dec, ASN1uint32_t *len, ASN1uint32_t *infinite)
{
    // default is definite length
    if (infinite)
    {
        *infinite = 0;
    }

    /* get length and infinite flag */
    if (ASN1BERDecCheck(dec, 1))
    {
        ASN1uint32_t l = *dec->pos++;
        if (l < 0x80)
        {
            *len = l;
        }
        else
        if (l == 0x80)
        {
            *len = 0;
            if (infinite)
            {
                *infinite = 1;
            }
            else
            {
                ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
                return 0;
            }
        }
        else
        if (l <= 0x84)
        {
            ASN1uint32_t i = l - 0x80;
            if (ASN1BERDecCheck(dec, i))
            {
                l = 0;
                switch (i)
                {
                case 4:
                    l = *dec->pos++ << 24;
                    /*FALLTHROUGH*/
                case 3:
                    l |= *dec->pos++ << 16;
                    /*FALLTHROUGH*/
                case 2:
                    l |= *dec->pos++ << 8;
                    /*FALLTHROUGH*/
                case 1:
                    l |= *dec->pos++;
                    break;
                }
                *len = l;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
            return 0;
        }

        /* check if enough octets left if length is known */
        if (!infinite || !*infinite)
        {
            return ASN1BERDecCheck(dec, *len);
        }
        return 1;
    }

    return 0;
}

/* decode an explicit tag */
int ASN1BERDecExplicitTag(ASN1decoding_t dec, ASN1uint32_t tag, ASN1decoding_t *dd, ASN1octet_t **ppBufEnd)
{
    ASN1uint32_t len, infinite;

    // safety net
    if (dd)
    {
        *dd = dec;
    }

    /* skip the constructed tag */
    if (ASN1BERDecTag(dec, tag | 0x20000000, NULL))
    {
        /* get the length */
        if (ASN1BERDecLength(dec, &len, &infinite))
        {
            /* start decoding of constructed value */
            if (! dd)
            {
                *ppBufEnd = infinite ? NULL : dec->pos + len;
                return 1;
            }
            return _BERDecConstructed(dec, len, infinite, dd, ppBufEnd);
        }
    }
    return 0;
}

/* decode octet string value */
int _BERDecOctetString(ASN1decoding_t dec, ASN1uint32_t tag, ASN1octetstring_t *val, ASN1uint32_t fNoCopy)
{
    ASN1INTERNdecoding_t d = (ASN1INTERNdecoding_t)dec;
    ASN1uint32_t constructed, len, infinite;
    ASN1decoding_t dd;
    ASN1octet_t *di;

    if (ASN1BERDecTag(dec, tag, &constructed))
    {
        if (ASN1BERDecLength(dec, &len, &infinite))
        {
            if (! constructed)
            {
                val->length = len;
                if (fNoCopy)
                {
                    val->value = dec->pos;
                    dec->pos += len;
                    return 1;
                }
                else
                {
                    if (len)
                    {
#ifdef XBOX_SPECIFIC_OPTIMIZATIONS
                        val->value = dec->pos;
                        dec->pos += len;
                        return 1;
#else
                        val->value = (ASN1octet_t *)DecMemAlloc(dec, len);
                        if (val->value)
                        {
                            RtlCopyMemory(val->value, dec->pos, len);
                            dec->pos += len;
                            return 1;
                        }
#endif
                    }
                    else
                    {
                        val->value = NULL;
                        return 1;
                    }
                }
            }
            else
            {
                ASN1octetstring_t o;
                val->length = 0;
                val->value = NULL;
                if (_BERDecConstructed(dec, len, infinite, &dd, &di))
                {
                    while (ASN1BERDecNotEndOfContents(dd, di))
                    {
                        o.length = 0;
                        o.value = NULL;
                        if (_BERDecOctetString(dd, 0x4, &o, fNoCopy))
                        {
                            if (o.length)
                            {
                                if (fNoCopy)
                                {
                                    *val = o;
                                    break; // break out the loop because nocopy cannot have multiple constructed streams
                                }

                                /* resize value */
                                val->value = (ASN1octet_t *)DecMemReAlloc(dd, val->value,
                                                                val->length + o.length);
                                if (val->value)
                                {
                                    /* concat octet strings */
                                    RtlCopyMemory(val->value + val->length, o.value, o.length);
                                    val->length += o.length;

                                    /* free unused octet string */
                                    DecMemFree(dec, o.value);
                                }
                                else
                                {
                                    return 0;
                                }
                            }
                        }
                    }
                    return ASN1BERDecEndOfContents(dec, dd, di);
                }
            }
        }
    }
    return 0;
}

/* decode octet string value, making copy */
int ASN1BERDecOctetString(ASN1decoding_t dec, ASN1uint32_t tag, ASN1octetstring_t *val)
{
    return _BERDecOctetString(dec, tag, val, FALSE);
}

/* decode octet string value, no copy */
int ASN1BERDecOctetString2(ASN1decoding_t dec, ASN1uint32_t tag, ASN1octetstring_t *val)
{
    return _BERDecOctetString(dec, tag, val, TRUE);
}

/* peek the following tag without advancing the read position */
int ASN1BERDecPeekTag(ASN1decoding_t dec, ASN1uint32_t *tag)
{
    ASN1uint32_t tagvalue, tagclass, c;
    ASN1octet_t *p;

    *tag = 0; // invalid tag
    if (_BERDecPeekCheck(dec, 1))
    {
        p = dec->pos;

        /* get tagclass without primitive/constructed bit */
        tagclass = *dec->pos & 0xc0;

        /* get tag value */
        tagvalue = *dec->pos++ & 0x1f;
        if (tagvalue == 0x1f)
        {
            tagvalue = 0;
            do {
                if (_BERDecPeekCheck(dec, 1))
                {
                    c = *dec->pos++;
                    if (! (tagvalue & 0xe0000000))
                    {
                        tagvalue = (tagvalue << 7) | (c & 0x7f);
                    }
                    else
                    {
                        ASN1DecSetError(dec, ASN1_ERR_BADTAG);
                        return 0;
                    }
                }
                else
                {
                    return 0;
                }
            } while (c & 0x80);
        }

        /* return tag */
        *tag = ((tagclass << 24) | tagvalue);

        /* reset decoding position */
        dec->pos = p;
        return 1;
    }
    return 0;
}

/* decode boolean into ASN1boot_t */
int ASN1BERDecBool(ASN1decoding_t dec, ASN1uint32_t tag, ASN1bool_t *val)
{
    /* skip tag */
    if (ASN1BERDecTag(dec, tag, NULL))
    {
        /* get length */
        ASN1uint32_t len;
        if (ASN1BERDecLength(dec, &len, NULL))
        {
            if (len >= 1)
            {
                DecAssert(dec, len == 1);
                *val = *dec->pos ? 1 : 0;
                dec->pos += len; // self defensive
                return 1;
            }
        }
    }
    return 0;
}

/* decode integer into unsigned 32 bit value */
int ASN1BERDecU32Val(ASN1decoding_t dec, ASN1uint32_t tag, ASN1uint32_t *val)
{
    ASN1uint32_t len;

    /* skip tag */
    if (ASN1BERDecTag(dec, tag, NULL))
    {
        /* get length */
        if (ASN1BERDecLength(dec, &len, NULL))
        {
            /* get value */
            if (len >= 1)
            {
                switch (len)
                {
                case 1:
                    *val = *dec->pos++;
                    break;
                case 2:
                    *val = (*dec->pos << 8) | dec->pos[1];
                    dec->pos += 2;
                    break;
                case 3:
                    *val = (*dec->pos << 16) | (dec->pos[1] << 8) | dec->pos[2];
                    dec->pos += 3;
                    break;
                case 4:
                    *val = (*dec->pos << 24) | (dec->pos[1] << 16) |
                        (dec->pos[2] << 8) | dec->pos[3];
                    dec->pos += 4;
                    break;
                case 5:
                    if (! *dec->pos)
                    {
                        *val = (dec->pos[1] << 24) | (dec->pos[2] << 16) |
                            (dec->pos[3] << 8) | dec->pos[4];
                        dec->pos += 5;
                        break;
                    }
                    // intentionally fall through
                default:
                    ASN1DecSetError(dec, ASN1_ERR_LARGE);
                    return 0;
                }
                return 1;
            }
            else
            {
                ASN1DecSetError(dec, ASN1_ERR_CORRUPT);
                return 0;
            }
        }
    }
    return 0;
}

int ASN1BERDecEndOfContents(ASN1decoding_t dec, ASN1decoding_t dd, ASN1octet_t *pBufEnd)
{
    ASN1error_e err = ASN1_ERR_CORRUPT;

    if (! dd)
    {
        dd = dec;
    }

    DecAssert(dec, NULL != dd);

    if (pBufEnd)
    {
        /* end of definite length case: */
        /* check if decoded up to end of contents */
        if (dd->pos == pBufEnd)
        {
            dec->pos = pBufEnd;
            err = ASN1_SUCCESS;
        }
    }
    else
    {
        /* end of infinite length case: */
        /* expect end-of-contents octets */
        if (ASN1BERDecCheck(dd, 2))
        {
            if (0 == dd->pos[0] && 0 == dd->pos[1])
            {
                dd->pos += 2;
                if (dd != dec)
                {
                    /* finit child decoding stream and update parent decoding stream */
                    dec->pos = dd->pos;
                }
                err = ASN1_SUCCESS;
            }
        }
        else
        {
            err = ASN1_ERR_EOD;
        }
    }

    if (dd && dd != dec)
    {
        ASN1_CloseDecoder(dd);
    }

    if (ASN1_SUCCESS == err)
    {
        return 1;
    }

    ASN1DecSetError(dec, err);
    return 0;
}

/* check if end of contents (of a constructed value) has been reached */
int ASN1BERDecNotEndOfContents(ASN1decoding_t dec, ASN1octet_t *pBufEnd)
{
    return (pBufEnd ?
                (dec->pos < pBufEnd) :
                (ASN1BERDecCheck(dec, 2) && (dec->pos[0] || dec->pos[1])));
}


#ifdef ENABLE_BER

typedef struct
{
    ASN1octet_t        *pBuf;
    ASN1uint32_t        cbBufSize;
}
    CER_BLK_BUF;

typedef struct
{
    ASN1blocktype_e     eBlkType;
    ASN1encoding_t      encPrimary;
    ASN1encoding_t      encSecondary;
    ASN1uint32_t        nMaxBlkSize;
    ASN1uint32_t        nCurrBlkSize;
    CER_BLK_BUF        *aBlkBuf;
}
    CER_BLOCK;

#define MAX_INIT_BLK_SIZE   16

int ASN1CEREncBeginBlk(ASN1encoding_t enc, ASN1blocktype_e eBlkType, void **ppBlk_)
{
    CER_BLOCK *pBlk = (CER_BLOCK *) EncMemAlloc(enc, sizeof(CER_BLOCK));
    if (NULL != pBlk)
    {
        EncAssert(enc, ASN1_DER_SET_OF_BLOCK == eBlkType);
        pBlk->eBlkType = eBlkType;
        pBlk->encPrimary = enc;
        pBlk->encSecondary = NULL;
        pBlk->nMaxBlkSize = MAX_INIT_BLK_SIZE;
        pBlk->nCurrBlkSize = 0;
        pBlk->aBlkBuf = (CER_BLK_BUF *)EncMemAlloc(enc, MAX_INIT_BLK_SIZE * sizeof(CER_BLK_BUF));
        if (NULL != pBlk->aBlkBuf)
        {
            *ppBlk_ = (void *) pBlk;
            return 1;
        }
        EncMemFree(enc, pBlk);
    }
    return 0;
}

int ASN1CEREncNewBlkElement(void *pBlk_, ASN1encoding_t *enc2)
{
    CER_BLOCK *pBlk = (CER_BLOCK *) pBlk_;
    if (NULL == pBlk->encSecondary)
    {
        if (ASN1_SUCCESS == ASN1_CreateEncoder(pBlk->encPrimary->pScratchMemoryAllocator, pBlk->encPrimary->module,
                                               &(pBlk->encSecondary),
                                               NULL, 0, pBlk->encPrimary))
        {
            pBlk->encSecondary->eRule = pBlk->encPrimary->eRule;
            *enc2 = pBlk->encSecondary;
            return 1;
        }
    }
    else
    {
        ASN1INTERNencoding_t e = (ASN1INTERNencoding_t) (*enc2 = pBlk->encSecondary);

        RtlZeroMemory(e, sizeof(*e));
        e->info.magic = MAGIC_ENCODER;
        // e->info.err = ASN1_SUCCESS;
        // e->info.pos = e->info.buf = NULL;
        // e->info.size = e->info.len = e->info.bit = 0;
        // e->info.dwFlags = 0;
        e->info.module = pBlk->encPrimary->module;
        e->info.eRule = pBlk->encPrimary->eRule;

        ((ASN1INTERNencoding_t) pBlk->encPrimary)->child = e;
        e->parent = (ASN1INTERNencoding_t) pBlk->encPrimary;
        // e->child = NULL;

        // e->mem = NULL;
        // e->memlength = 0;
        // e->memsize = 0;
        // e->epi = NULL;
        // e->epilength = 0;
        // e->episize = 0;
        // e->csi = NULL;
        // e->csilength = 0;
        // e->csisize = 0;

        if (ASN1BEREncCheck((ASN1encoding_t) e, 1))
        {
            // lonchanc: make sure the first byte is zeroed out, which
            // is required for h245.
            e->info.buf[0] = '\0';
            return 1;
        }
    }

    *enc2 =  NULL;
    return 0;
}

int ASN1CEREncFlushBlkElement(void *pBlk_)
{
    CER_BLOCK *pBlk = (CER_BLOCK *) pBlk_;
    ASN1encoding_t enc = pBlk->encSecondary;
    ASN1uint32_t i;

    if (ASN1BEREncFlush(enc))
    {
        // make sure we have enough space...
        if (pBlk->nCurrBlkSize >= pBlk->nMaxBlkSize)
        {
            CER_BLK_BUF *aBlkBuf = (CER_BLK_BUF *)EncMemAlloc(pBlk->encPrimary, (pBlk->nMaxBlkSize << 1) * sizeof(CER_BLK_BUF));
            if (NULL != aBlkBuf)
            {
                RtlCopyMemory(aBlkBuf, pBlk->aBlkBuf, pBlk->nMaxBlkSize * sizeof(CER_BLK_BUF));
                EncMemFree(pBlk->encPrimary, pBlk->aBlkBuf);
                pBlk->aBlkBuf = aBlkBuf;
                pBlk->nMaxBlkSize <<= 1;
            }
            else
            {
                return 0;
            }
        }

        if (pBlk->encPrimary->eRule & (ASN1_BER_RULE_DER | ASN1_BER_RULE_CER))
        {
            // we need to sort these octet strings
            for (i = 0; i < pBlk->nCurrBlkSize; i++)
            {
                if (0 >= My_memcmp(enc->buf, enc->len, pBlk->aBlkBuf[i].pBuf, pBlk->aBlkBuf[i].cbBufSize))
                {
                    ASN1uint32_t cnt = pBlk->nCurrBlkSize - i;
                    ASN1uint32_t j;
                    for (j = pBlk->nCurrBlkSize; cnt--; j--)
                    {
                        pBlk->aBlkBuf[j] = pBlk->aBlkBuf[j-1];
                    }
                    // i is the place to hold the new one
                    break;
                }
            }
        }
        else
        {
            EncAssert(enc, ASN1_BER_RULE_BER == pBlk->encPrimary->eRule);
            i = pBlk->nCurrBlkSize;
        }

        // remeber the new one.
        pBlk->aBlkBuf[i].pBuf = enc->buf;
        pBlk->aBlkBuf[i].cbBufSize = enc->len;
        pBlk->nCurrBlkSize++;
        
        // clean up the encoder structure
        enc->buf = enc->pos = NULL;
        enc->size = enc->len = 0;
        return 1;
    }
    return 0;
}

int ASN1CEREncEndBlk(void *pBlk_)
{
    CER_BLOCK *pBlk = (CER_BLOCK *) pBlk_;
    ASN1encoding_t enc = pBlk->encPrimary;
    ASN1uint32_t cbTotalSize = 0;
    ASN1uint32_t i;
    int fRet = 0;

    // calculate the total size for all the buffers.
    for (i = 0; i < pBlk->nCurrBlkSize; i++)
    {
        cbTotalSize += pBlk->aBlkBuf[i].cbBufSize;
    }

    if (ASN1BEREncCheck(enc, cbTotalSize))
    {
        for (i = 0; i < pBlk->nCurrBlkSize; i++)
        {
            ASN1uint32_t cbBufSize = pBlk->aBlkBuf[i].cbBufSize;
            RtlCopyMemory(enc->pos, pBlk->aBlkBuf[i].pBuf, cbBufSize);
            enc->pos += cbBufSize;
        }
        fRet = 1;
    }

    // free these block buffers.
    for (i = 0; i < pBlk->nCurrBlkSize; i++)
    {
        EncMemFree(enc, pBlk->aBlkBuf[i].pBuf);
    }

    // free the block buffer array
    EncMemFree(enc, pBlk->aBlkBuf);

	// free the secondary encoder structure
	ASN1_CloseEncoder(pBlk->encSecondary);

    // free the block structure itself.
    EncMemFree(enc, pBlk);

    return fRet;
}

#endif // ENABLE_BER

/* encode explicit tag */
int ASN1BEREncExplicitTag(ASN1encoding_t enc, ASN1uint32_t tag, ASN1uint32_t *pnLenOff)
{
    /* encode tag */
    if (ASN1BEREncTag(enc, tag | 0x20000000))
    {
        /* encode infinite length */
        if (ASN1BEREncCheck(enc, 1))
        {
            if (ASN1_BER_RULE_CER != enc->eRule)
            {
                // BER and DER always use definite length form.
                /* return the place to hold the length */
                *pnLenOff = (ASN1uint32_t) (enc->pos++ - enc->buf);
            }
            else
            {
                // CER sub-rule always use indefinite length form.
                *enc->pos++ = 0x80;
                *pnLenOff = 0;
            }
            return 1;
        }
    }
    return 0;
}

/* encode definite length */
int ASN1BEREncEndOfContents(ASN1encoding_t enc, ASN1uint32_t nLenOff)
{
    if (ASN1_BER_RULE_CER != enc->eRule)
    {
        ASN1octet_t *pbLen = enc->buf + nLenOff;
        ASN1uint32_t len = (ASN1uint32_t) (enc->pos - pbLen - 1);
        ASN1uint32_t cbLength = _BERGetLength(len, c_LengthTable, ARRAY_SIZE(c_LengthTable));

        //ASN1uint32_t i;

        if (cbLength == 1)
        {
            *pbLen = (ASN1octet_t) len;
            return 1;
        }

        // we have to move the octets upward by cbLength-1
        // --cbLength;
        if (ASN1BEREncCheck(enc, cbLength-1))
        {
            // update pbLen because enc->buf may change due to realloc.
            pbLen = enc->buf + nLenOff;

            // move memory
            RtlMoveMemory(pbLen + cbLength, pbLen + 1, len);

            // put the length
            enc->pos = pbLen;
            _BERPutLength(enc, len, cbLength);
            EncAssert(enc, enc->pos == pbLen + cbLength);

            // set up new position pointer.
            // now enc->pos is at the beginning of contents.
            enc->pos += len;
            return 1;
        }
    }
    else
    {
        EncAssert(enc, 0 == nLenOff);
        if (ASN1BEREncCheck(enc, 2))
        {
            *enc->pos++ = 0;
            *enc->pos++ = 0;
            return 1;
        }
    }
    return 0;
}


// The following is for CryptoAPI

#ifdef ENABLE_BER

 // max num of octets, ceiling of 64 / 7, is 10
#define MAX_BYTES_PER_NODE      10

ASN1uint32_t _BEREncOidNode64(ASN1encoding_t enc, unsigned __int64 n64, ASN1octet_t *pOut)
{
    ASN1uint32_t Low32, i, cb;
    ASN1octet_t aLittleEndian[MAX_BYTES_PER_NODE];

    RtlZeroMemory(aLittleEndian, sizeof(aLittleEndian));
    for (i = 0; n64 != 0; i++)
    {
        Low32 = *(ASN1uint32_t *) &n64;
        aLittleEndian[i] = (ASN1octet_t) (Low32 & 0x7f);
        n64 = Int64ShrlMod32(n64, 7);
    }
    cb = i ? i : 1; // at least one byte for zero value
    EncAssert(enc, cb <= MAX_BYTES_PER_NODE);
    if (cb > MAX_BYTES_PER_NODE)
    {
        cb = MAX_BYTES_PER_NODE;
    }
    for (i = 0; i < cb; i++)
    {
        EncAssert(enc, 0 == (0x80 & aLittleEndian[cb - i - 1]));
        *pOut++ = (ASN1octet_t) (0x80 | aLittleEndian[cb - i - 1]);
    }
    *(pOut-1) &= 0x7f;
    return cb;
}


ASN1uint32_t _BERDecOidNode64(unsigned __int64 *pn64, ASN1octet_t *pIn)
{
    ASN1uint32_t c;
    *pn64 = 0;
    for (c = 1; TRUE; c++)
    {
        *pn64 = Int64ShllMod32(*pn64, 7) + (unsigned __int64) (*pIn & 0x7f);
        if (!(*pIn++ & 0x80))
        {
            return c;
        }
    }
    return 0;
}


/* encode an object identifier value */
int ASN1BEREncEoid(ASN1encoding_t enc, ASN1uint32_t tag, ASN1encodedOID_t *val)
{
    /* encode tag */
    if (ASN1BEREncTag(enc, tag))
    {
        /* encode length */
        int rc = ASN1BEREncLength(enc, val->length);
        if (rc)
        {
            /* copy value */
            RtlCopyMemory(enc->pos, val->value, val->length);
            enc->pos += val->length;
        }
        return rc;
    }
    return 0;
}

/* decode object identifier value */
int ASN1BERDecEoid(ASN1decoding_t dec, ASN1uint32_t tag, ASN1encodedOID_t *val)
{
    val->length = 0; // safety net
    if (ASN1BERDecTag(dec, tag, NULL))
    {
        ASN1uint32_t len;
        if (ASN1BERDecLength(dec, &len, NULL))
        {
            val->length = (ASN1uint16_t) len;
            if (len)
            {
                val->value = (ASN1octet_t *) DecMemAlloc(dec, len);
                if (val->value)
                {
                    RtlCopyMemory(val->value, dec->pos, len);
                    dec->pos += len;
                    return 1;
                }
            }
            else
            {
                val->value = NULL;
                return 1;
            }
        }
    }
    return 0;
}


void ASN1BEREoid_free(ASN1encodedOID_t *val)
{
    if (val)
    {
        MemFree(val->value);
    }
}

#endif // ENABLE_BER

static const ASN1uint8_t
c_aBitMask[] = {
    0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
};

static const ASN1uint8_t
c_aBitMask4[] = {
    0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00
};

static const ASN1int32_t
c_aBitMask5[] = {
    (ASN1int32_t)0xffffffff, (ASN1int32_t)0xfffffffe,
    (ASN1int32_t)0xfffffffc, (ASN1int32_t)0xfffffff8,
    (ASN1int32_t)0xfffffff0, (ASN1int32_t)0xffffffe0,
    (ASN1int32_t)0xffffffc0, (ASN1int32_t)0xffffff80,
    (ASN1int32_t)0xffffff00, (ASN1int32_t)0xfffffe00,
    (ASN1int32_t)0xfffffc00, (ASN1int32_t)0xfffff800,
    (ASN1int32_t)0xfffff000, (ASN1int32_t)0xffffe000,
    (ASN1int32_t)0xffffc000, (ASN1int32_t)0xffff8000,
    (ASN1int32_t)0xffff0000, (ASN1int32_t)0xfffe0000,
    (ASN1int32_t)0xfffc0000, (ASN1int32_t)0xfff80000,
    (ASN1int32_t)0xfff00000, (ASN1int32_t)0xffe00000,
    (ASN1int32_t)0xffc00000, (ASN1int32_t)0xff800000,
    (ASN1int32_t)0xff000000, (ASN1int32_t)0xfe000000,
    (ASN1int32_t)0xfc000000, (ASN1int32_t)0xf8000000,
    (ASN1int32_t)0xf0000000, (ASN1int32_t)0xe0000000,
    (ASN1int32_t)0xc0000000, (ASN1int32_t)0x80000000,
    (ASN1int32_t)0x00000000
};

static const ASN1uint8_t
c_aBitCount[] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

/* copy nbits bits from src/srcbit into dst/dstbit;
   src points to first octet containing bits to be copied
   srcbit names the first bit within the first octet to be copied (0=msb, 7=lsb)
   dst points to first octet to copy into
   dstbit names the first bit within the first octet to copy into (0=msb, 7=lsb)
   nbits is the number of bits to copy;
   assumes that bits of broken octet at dst/dstbit are cleared;
   bits of last octet behind dst/dstbit+nbits-1 will be cleared
*/
void ASN1bitcpy(ASN1octet_t *dst, ASN1uint32_t dstbit, ASN1octet_t *src, ASN1uint32_t srcbit, ASN1uint32_t nbits)
{
    ASN1uint32_t xsrcbit, xdstbit;

    if (!nbits)
        return;

    if (dstbit >= 8) {
        dst += dstbit / 8;
        dstbit &= 7;
    }
    if (srcbit >= 8) {
        src += srcbit / 8;
        srcbit &= 7;
    }

    /* check if we have to fill broken first octet */
    if (dstbit) {
        xdstbit = 8 - dstbit;

        /* enough bits to fill up broken octet? */
        if (nbits >= xdstbit) {
            if (srcbit < dstbit) {
                *dst++ |= (*src >> (dstbit - srcbit)) & c_aBitMask[xdstbit];
                nbits -= xdstbit;
                srcbit += xdstbit;
                dstbit = 0;
            } else if (srcbit == dstbit) {
                *dst++ |= *src++ & c_aBitMask[xdstbit];
                nbits -= xdstbit;
                srcbit = 0;
                dstbit = 0;
            } else {
                *dst++ |= ((*src & c_aBitMask[8 - srcbit]) << (srcbit - dstbit)) |
                    (src[1] >> (8 - (srcbit - dstbit)));
                nbits -= xdstbit;
                src++;
                srcbit -= dstbit;
                dstbit = 0;
            }

        /* less bits to fill than needed to fill up the broken octet */
        } else {
            if (srcbit <= dstbit) {
                *dst |= ((*src >> (8 - srcbit - nbits)) & c_aBitMask[nbits]) <<
                    (xdstbit - nbits);
            } else {
                *dst++ |= ((*src & c_aBitMask[8 - srcbit]) << (srcbit - dstbit)) |
                    ((src[1] >> (16 - srcbit - nbits)) << (xdstbit - nbits));
            }
            return;
        }
    }

    /* fill up complete octets */
    if (nbits >= 8) {
        if (!srcbit) {
            RtlCopyMemory(dst, src, nbits / 8);
            dst += nbits / 8;
            src += nbits / 8;
            nbits &= 7;
        } else {
            xsrcbit = 8 - srcbit;
            do {
                *dst++ = (*src << srcbit) | (src[1] >> (xsrcbit));
                src++;
                nbits -= 8;
            } while (nbits >= 8);
        }
    }

    /* fill bits into last octet */
    if (nbits)
        {
                *dst = (*src << srcbit) & c_aBitMask2[nbits];
                // lonchanc: made the following fix for the case that
                // src bits across byte boundary.
                if (srcbit + nbits > 8)
                {
                        xsrcbit = nbits - (8 - srcbit);
                        src++;
                        *dst |= ((*src & c_aBitMask2[xsrcbit]) >> (8 - srcbit));
                }
        }
}

/* clear nbits bits at dst/dstbit;
   bits of last octet behind dst/dstbit+nbits-1 will be cleared
*/
void ASN1bitclr(ASN1octet_t *dst, ASN1uint32_t dstbit, ASN1uint32_t nbits)
{
    ASN1uint32_t xdstbit;

    if (!nbits)
        return;

    if (dstbit >= 8) {
        dst += dstbit / 8;
        dstbit &= 7;
    }

    /* clear broken ASN1octet first */
    if (dstbit) {
        xdstbit = 8 - dstbit;
        *dst &= c_aBitMask2[xdstbit];
        if (xdstbit < nbits) {
            dst++;
            nbits -= xdstbit;
        } else {
            return;
        }
    }

    /* clear remaining bits */
    RtlZeroMemory(dst, (nbits + 7) / 8);
}

/* clear nbits bits at dst/dstbit;
   bits of last octet behind dst/dstbit+nbits-1 will be cleared
*/
void ASN1bitset(ASN1octet_t *dst, ASN1uint32_t dstbit, ASN1uint32_t nbits)
{
    ASN1uint32_t xdstbit;

    if (!nbits)
        return;

    if (dstbit >= 8) {
        dst += dstbit / 8;
        dstbit &= 7;
    }

    /* set broken ASN1octet first */
    if (dstbit) {
        xdstbit = 8 - dstbit;
        if (xdstbit < nbits) {
            *dst |= c_aBitMask4[xdstbit];
            dst++;
            nbits -= xdstbit;
        } else {
            *dst |= c_aBitMask4[nbits] << (xdstbit - nbits);
            return;
        }
    }

    /* set complete octets */
    if (nbits >= 8) {
        memset(dst, 0xff, nbits / 8);
        dst += nbits / 8;
        nbits &= 7;
    }

    /* set remaining bits */
    if (nbits)
        *dst |= c_aBitMask4[nbits] << (8 - nbits);
}

/* write nbits bits of val at dst/dstbit;
   assumes that bits of broken octet at dst/dstbit are cleared;
   bits of last octet behind dst/dstbit+nbits-1 will be cleared
*/
void ASN1bitput(ASN1octet_t *dst, ASN1uint32_t dstbit, ASN1uint32_t val, ASN1uint32_t nbits)
{
    ASN1uint32_t xdstbit;

    if (!nbits)
        return;

    if (dstbit >= 8) {
        dst += dstbit / 8;
        dstbit &= 7;
    }
    xdstbit = 8 - dstbit;

    /* fill up broken octet first */
    if (dstbit) {
        if (xdstbit <= nbits) {
            *dst++ |= val >> (nbits -= xdstbit);
        } else {
            *dst |= (val & c_aBitMask[nbits]) << (xdstbit - nbits);
            return;
        }
    }

    /* copy complete octets */
    while (nbits >= 8)
        *dst++ = (ASN1octet_t) (val >> (nbits -= 8));

    /* copy left bits */
    if (nbits)
        *dst = (ASN1octet_t) ((val & c_aBitMask[nbits]) << (8 - nbits));
}

/* read nbits bits of val at src/srcbit */
// lonchanc: the return value is independent of big or little endian
// because we use shift left within a long integer.
ASN1uint32_t ASN1bitgetu(ASN1octet_t *src, ASN1uint32_t srcbit, ASN1uint32_t nbits)
{
    ASN1uint32_t xsrcbit;
    ASN1uint32_t ret;

    if (!nbits)
        return 0;

    if (srcbit >= 8) {
        src += srcbit / 8;
        srcbit &= 7;
    }
    xsrcbit = 8 - srcbit;
    ret = 0;

    /* get bits from broken octet first */
    if (srcbit) {
        if (xsrcbit <= nbits) {
            ret = (*src++ & c_aBitMask[xsrcbit]) << (nbits -= xsrcbit);
        } else {
            return (*src >> (xsrcbit - nbits)) & c_aBitMask[nbits];
        }
    }

    /* get complete octets */
    while (nbits >= 8)
        ret |= *src++ << (nbits -= 8);

    /* get left bits */
    if (nbits)
        ret |= ((*src) >> (8 - nbits)) & c_aBitMask[nbits];
    return ret;
}

/* read nbits bits of val at src/srcbit */
ASN1int32_t ASN1bitget(ASN1octet_t *src, ASN1uint32_t srcbit, ASN1uint32_t nbits)
{
    ASN1uint32_t xsrcbit;
    ASN1int32_t ret;

    if (!nbits)
        return 0;

    if (srcbit >= 8) {
        src += srcbit / 8;
        srcbit &= 7;
    }
    xsrcbit = 8 - srcbit;
    if (*src & (0x80 >> srcbit))
        ret = c_aBitMask5[nbits];
    else
        ret = 0;

    /* get bits from broken octet first */
    if (srcbit) {
        if (xsrcbit <= nbits) {
            ret = *src++ << (nbits -= xsrcbit);
        } else {
            return (*src >> (xsrcbit - nbits)) & c_aBitMask[nbits];
        }
    }

    /* get complete octets */
    while (nbits >= 8)
        ret |= *src++ << (nbits -= 8);

    /* get left bits */
    if (nbits)
        ret |= ((*src) >> (8 - nbits)) & c_aBitMask[nbits];
    return ret;
}

/* get number of set bits in nbits bits at src/srcbit */
ASN1uint32_t ASN1bitcount(ASN1octet_t *src, ASN1uint32_t srcbit, ASN1uint32_t nbits)
{
    ASN1uint32_t xsrcbit;
    ASN1uint32_t ret;

    if (!nbits)
        return 0;

    if (srcbit >= 8) {
        src += srcbit / 8;
        srcbit &= 7;
    }
    xsrcbit = 8 - srcbit;

    /* count bits from broken octet first */
    if (srcbit) {
        if (xsrcbit <= nbits) {
            ret = c_aBitCount[*src++ & c_aBitMask4[srcbit]];
            nbits -= xsrcbit;
        } else {
            return c_aBitCount[(*src >> (xsrcbit - nbits)) & c_aBitMask[nbits]];
        }
    } else {
        ret = 0;
    }

    /* count bits in complete octets */
    while (nbits >= 8)
	{
        ret += c_aBitCount[*src++];
		nbits -= 8;
	}

    /* count left bits */
    if (nbits)
        ret += c_aBitCount[(*src) & c_aBitMask2[nbits]];
    return ret;
}

/* write noctets of val at dst */
void ASN1octetput(ASN1octet_t *dst, ASN1uint32_t val, ASN1uint32_t noctets)
{
    switch (noctets) {
    case 4:
        *dst++ = (ASN1octet_t)(val >> 24);
        /*FALLTHROUGH*/
    case 3:
        *dst++ = (ASN1octet_t)(val >> 16);
        /*FALLTHROUGH*/
    case 2:
        *dst++ = (ASN1octet_t)(val >> 8);
        /*FALLTHROUGH*/
    case 1:
        *dst++ = (ASN1octet_t)(val);
        break;
    default:
    break;
        MyAssert(0);
        /*NOTREACHED*/
    }
}

/* read noctets of val at dst */
ASN1uint32_t ASN1octetget(ASN1octet_t *src, ASN1uint32_t noctets)
{
    switch (noctets) {
    case 4:
        return (*src << 24) | (src[1] << 16) | (src[2] << 8) | src[3];
    case 3:
        return (*src << 16) | (src[1] << 8) | src[2];
    case 2:
        return (*src << 8) | src[1];
    case 1:
        return *src;
    default:
        MyAssert(0);
        return(0);
        /*NOTREACHED*/
    }
}

/* check if a 16 bit character is a space */
int ASN1is16space(ASN1char16_t c)
{
    return c == ' ' || c == '\t' || c == '\b' || c == '\f' || c == '\r' ||
	c == '\n' || c == '\v';
}

/* get length of a 16 bit string */
// lonchanc: lstrlenW()
int ASN1str16len(ASN1char16_t *p)
{
    
    int len;

    for (len = 0; *p; p++)
        len++;
    return len;
}

#ifdef USE_lstrlen
int My_lstrlenA(char *p)
{
    return (NULL != p) ? lstrlenA(p) : 0;
}

int My_lstrlenW(WCHAR *p)
{
    return (NULL != p) ? lstrlenW(p) : 0;
}
#else
int My_lstrlenA( char* lpString )
{
    if (lpString == NULL)
        return 0;
#ifdef EXCEPTIONS_ALLOWED
    __try
    {
        return strlen(lpString);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return 0;
    }
#else
    return strlen(lpString);
#endif
}

int My_lstrlenW( WCHAR* lpString )
{
    if (lpString == NULL)
        return 0;
#ifdef EXCEPTIONS_ALLOWED
    __try
    {
        return wcslen(lpString);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return 0;
    }
#else
    return wcslen(lpString);
#endif
}
#endif

/* check if a 32 bit character is a space */
int ASN1is32space(ASN1char32_t c)
{
    return c == ' ' || c == '\t' || c == '\b' || c == '\f' || c == '\r' ||
            c == '\n' || c == '\v';
}

/* get length of a 32 bit string */
int ASN1str32len(ASN1char32_t *p)
{
    int len;

    for (len = 0; *p; p++)
        len++;
    return len;
}

/* external use only, allocate memory for decoding */
LPVOID ASN1DecAlloc(ASN1decoding_t dec, ASN1uint32_t size)
{
    return DecMemAlloc(dec, size);
}

/* external use only, reallocate memory for decoding */
LPVOID ASN1DecRealloc(ASN1decoding_t dec, LPVOID ptr, ASN1uint32_t size)
{
    return DecMemReAlloc(dec, ptr, size);
}

/* external use only, free a memory block */
void ASN1Free(LPVOID ptr)
{
    MemFree(ptr);
}

// lonchanc: we need to re-visit this approach of aborting a decoding
/* abort decoding, free any memory allocated for decoding */
void ASN1DecAbort(ASN1decoding_t dec)
{
    ASN1INTERNdecoding_t d = ((ASN1INTERNdecoding_t)dec)->parent;

#ifdef ENABLE_EXTRA_INFO
    /* clear the lists */
    d->memlength = d->epilength = d->csilength = 0;
    d->memsize = d->episize = d->csisize = 0;
    MemFree(d->mem);
    MemFree(d->epi);
    MemFree(d->csi);
    d->mem = NULL;
    d->epi = NULL;
    d->csi = NULL;
#endif // ENABLE_EXTRA_INFO
}

// lonchanc: we need to re-visit this approach of aborting a decoding
/* finish decoding */
void ASN1DecDone(ASN1decoding_t dec)
{
    ASN1INTERNdecoding_t d = ((ASN1INTERNdecoding_t)dec)->parent;

#ifdef ENABLE_EXTRA_INFO
    /* clear the lists */
    d->memlength = d->epilength = d->csilength = 0;
    d->memsize = d->episize = d->csisize = 0;
    MemFree(d->mem);
    MemFree(d->epi);
    MemFree(d->csi);
    d->mem = NULL;
    d->epi = NULL;
    d->csi = NULL;
#endif // ENABLE_EXTRA_INFO
}

// lonchanc: we need to re-visit this approach of aborting a decoding
/* abort encoding, free any memory allocated for encoding */
void ASN1EncAbort(ASN1encoding_t enc)
{
    ASN1INTERNencoding_t e = ((ASN1INTERNencoding_t)enc)->parent;

#ifdef ENABLE_EXTRA_INFO
    /* clear the lists */
    e->memlength = e->epilength = e->csilength = 0;
    e->memsize = e->episize = e->csisize = 0;
    MemFree(e->mem);
    MemFree(e->epi);
    MemFree(e->csi);
    e->mem = NULL;
    e->epi = NULL;
    e->csi = NULL;
#endif // ENABLE_EXTRA_INFO
}

// lonchanc: we need to re-visit this approach of aborting a decoding
/* finish encoding */
void ASN1EncDone(ASN1encoding_t enc)
{
    ASN1INTERNencoding_t e = ((ASN1INTERNencoding_t)enc)->parent;

#ifdef ENABLE_EXTRA_INFO
    /* clear the lists */
    e->memlength = e->epilength = e->csilength = 0;
    e->memsize = e->episize = e->csisize = 0;
    MemFree(e->mem);
    MemFree(e->epi);
    MemFree(e->csi);
    e->mem = NULL;
    e->epi = NULL;
    e->csi = NULL;
#endif // ENABLE_EXTRA_INFO
}


/* search the identification of an embedded pdv */
#ifdef ENABLE_EMBEDDED_PDV
int ASN1EncSearchEmbeddedPdvIdentification(ASN1INTERNencoding_t e, ASN1embeddedpdv_identification_t *identification, ASN1uint32_t *index, ASN1uint32_t *flag)
{
    ASN1embeddedpdv_identification_t **id;

    /* search identification in indentification list */
    /* if found then reset flag (to indicate EP-B encoding) and return */
    for (*index = 0, id = e->epi; *index < e->epilength; (*index)++, id++) {
        if ((*id)->o == identification->o) {
            switch ((*id)->o) {
            case ASN1embeddedpdv_identification_syntaxes_o:
                if (!ASN1objectidentifier_cmp(&(*id)->u.syntaxes.abstract,
                    &identification->u.syntaxes.abstract) &&
                    !ASN1objectidentifier_cmp(&(*id)->u.syntaxes.transfer,
                    &identification->u.syntaxes.transfer)) {
                    *flag = 0;
                    return 1;
                }
                break;
            case ASN1embeddedpdv_identification_syntax_o:
                if (!ASN1objectidentifier_cmp(&(*id)->u.syntax,
                    &identification->u.syntax)) {
                    *flag = 0;
                    return 1;
                }
                break;
            case ASN1embeddedpdv_identification_presentation_context_id_o:
                if ((*id)->u.presentation_context_id ==
                    identification->u.presentation_context_id) {
                    *flag = 0;
                    return 1;
                }
                break;
            case ASN1embeddedpdv_identification_context_negotiation_o:
                if ((*id)->u.context_negotiation.presentation_context_id ==
                    identification->u.context_negotiation.
                    presentation_context_id &&
                    !ASN1objectidentifier_cmp(
                    &(*id)->u.context_negotiation.transfer_syntax,
                    &identification->u.context_negotiation.transfer_syntax)) {
                    *flag = 0;
                    return 1;
                }
                break;
            case ASN1embeddedpdv_identification_transfer_syntax_o:
                if (!ASN1objectidentifier_cmp(&(*id)->u.transfer_syntax,
                    &identification->u.transfer_syntax)) {
                    *flag = 0;
                    return 1;
                }
                break;
            case ASN1embeddedpdv_identification_fixed_o:
                *flag = 0;
                return 1;
            default:
                e->parent->info.err = ASN1_ERR_CORRUPT;
                return 0;
            }
        }
    }
    /* identification not found */

    /* add it into indentification array */
    if (e->epilength >= e->episize) {
        e->episize = e->episize ? 4 * e->episize : 16;
        e->epi = (ASN1embeddedpdv_identification_t **)MemReAlloc(e->epi,
            e->episize * sizeof(ASN1embeddedpdv_identification_t *), _ModName((ASN1encoding_t) e));
        if (!e->epi)
        {
            ASN1EncSetError((ASN1encoding_t) e, ASN1_ERR_MEMORY);
            return 0;
        }
    }
    e->epi[e->epilength++] = identification;

    /* return flag for EP-A encoding */
    *flag = 1;
    return 1;
}
#endif // ENABLE_EMBEDDED_PDV

/* search the identification of an character string */
#ifdef ENABLE_GENERALIZED_CHAR_STR
int ASN1EncSearchCharacterStringIdentification(ASN1INTERNencoding_t e, ASN1characterstring_identification_t *identification, ASN1uint32_t *index, ASN1uint32_t *flag)
{
    ASN1characterstring_identification_t **id;

    /* search identification in indentification list */
    /* if found then reset flag (to indicate CS-B encoding) and return */
    for (*index = 0, id = e->csi; *index < e->csilength; (*index)++, id++) {
        if ((*id)->o == identification->o) {
            switch ((*id)->o) {
            case ASN1characterstring_identification_syntaxes_o:
                if (!ASN1objectidentifier_cmp(&(*id)->u.syntaxes.abstract,
                    &identification->u.syntaxes.abstract) &&
                    !ASN1objectidentifier_cmp(&(*id)->u.syntaxes.transfer,
                    &identification->u.syntaxes.transfer)) {
                    *flag = 0;
                    return 1;
                }
                break;
            case ASN1characterstring_identification_syntax_o:
                if (!ASN1objectidentifier_cmp(&(*id)->u.syntax,
                    &identification->u.syntax)) {
                    *flag = 0;
                    return 1;
                }
                break;
            case ASN1characterstring_identification_presentation_context_id_o:
                if ((*id)->u.presentation_context_id ==
                    identification->u.presentation_context_id) {
                    *flag = 0;
                    return 1;
                }
                break;
            case ASN1characterstring_identification_context_negotiation_o:
                if ((*id)->u.context_negotiation.presentation_context_id ==
                    identification->u.context_negotiation.
                    presentation_context_id &&
                    !ASN1objectidentifier_cmp(
                    &(*id)->u.context_negotiation.transfer_syntax,
                    &identification->u.context_negotiation.transfer_syntax)) {
                    *flag = 0;
                    return 1;
                }
                break;
            case ASN1characterstring_identification_transfer_syntax_o:
                if (!ASN1objectidentifier_cmp(&(*id)->u.transfer_syntax,
                    &identification->u.transfer_syntax)) {
                    *flag = 0;
                    return 1;
                }
                break;
            case ASN1characterstring_identification_fixed_o:
                *flag = 0;
                return 1;
            default:
                e->parent->info.err = ASN1_ERR_CORRUPT;
                return 0;
            }
        }
    }
    /* identification not found */

    /* add it into indentification array */
    if (e->csilength >= e->csisize) {
        e->csisize = e->csisize ? 4 * e->csisize : 16;
        e->csi = (ASN1characterstring_identification_t **)MemReAlloc(e->csi,
            e->csisize * sizeof(ASN1characterstring_identification_t *), _ModName((ASN1encoding_t) e));
        if (!e->csi)
        {
            ASN1EncSetError((ASN1encoding_t) e, ASN1_ERR_MEMORY);
            return 0;
        }
    }
    e->csi[e->csilength++] = identification;

    /* return flag for CS-A encoding */
    *flag = 1;
    return 1;
}
#endif // ENABLE_GENERALIZED_CHAR_STR

/* allocate and copy an object identifier */
#if defined(ENABLE_GENERALIZED_CHAR_STR) || defined(ENABLE_EMBEDDED_PDV)
int ASN1DecDupObjectIdentifier(ASN1decoding_t dec, ASN1objectidentifier_t *dst, ASN1objectidentifier_t *src)
{
    ASN1INTERNdecoding_t d = ((ASN1INTERNdecoding_t)dec)->parent;
    ASN1uint32_t l = GetObjectIdentifierCount(*src);
    *dst = DecAllocObjectIdentifier(dec, l);
    if (! *dst)
    {
        ASN1DecSetError((ASN1decoding_t) d, ASN1_ERR_MEMORY);
        return 0;
    }
    CopyObjectIdentifier(*dst, *src);
    return 1;
}
#endif // defined(ENABLE_GENERALIZED_CHAR_STR) || defined(ENABLE_EMBEDDED_PDV)

/* add an embedded pdv identification to the list of identifications */
#ifdef ENABLE_EMBEDDED_PDV
int ASN1DecAddEmbeddedPdvIdentification(ASN1INTERNdecoding_t d, ASN1embeddedpdv_identification_t *identification)
{
    if (d->epilength >= d->episize) {
        d->episize = d->episize ? 4 * d->episize : 16;
        d->epi = (ASN1embeddedpdv_identification_t **)MemReAlloc(d->epi,
            d->episize * sizeof(ASN1embeddedpdv_identification_t *), _ModName((ASN1decoding_t) d));
        if (!d->epi)
        {
            ASN1DecSetError((ASN1decoding_t) d, ASN1_ERR_MEMORY);
            return 0;
        }
    }
    d->epi[d->epilength++] = identification;
    return 1;
}
#endif // ENABLE_EMBEDDED_PDV

/* get an embedded pdv identification from the list of identifications */
#ifdef ENABLE_EMBEDDED_PDV
ASN1embeddedpdv_identification_t *ASN1DecGetEmbeddedPdvIdentification(ASN1INTERNdecoding_t d, ASN1uint32_t index)
{
    if (index >= d->epilength)
    {
        ASN1DecSetError((ASN1decoding_t) d, ASN1_ERR_CORRUPT);
        return NULL;
    }
    return d->epi[index];
}
#endif // ENABLE_EMBEDDED_PDV

/* add a character string identification to the list of identifications */
#ifdef ENABLE_GENERALIZED_CHAR_STR
int ASN1DecAddCharacterStringIdentification(ASN1INTERNdecoding_t d, ASN1characterstring_identification_t *identification)
{
    if (d->csilength >= d->csisize) {
        d->csisize = d->csisize ? 4 * d->csisize : 16;
        d->csi = (ASN1characterstring_identification_t **)MemReAlloc(d->csi,
            d->csisize * sizeof(ASN1characterstring_identification_t *), _ModName((ASN1decoding_t) d));
        if (!d->csi)
        {
            ASN1DecSetError((ASN1decoding_t) d, ASN1_ERR_MEMORY);
            return 0;
        }
    }
    d->csi[d->csilength++] = identification;
    return 1;
}
#endif // ENABLE_GENERALIZED_CHAR_STR

/* get a character string identification from the list of identifications */
#ifdef ENABLE_GENERALIZED_CHAR_STR
ASN1characterstring_identification_t *ASN1DecGetCharacterStringIdentification(ASN1INTERNdecoding_t d, ASN1uint32_t index)
{
    if (index >= d->csilength)
    {
        ASN1DecSetError((ASN1decoding_t) d, ASN1_ERR_CORRUPT);
        return NULL;
    }
    return d->csi[index];
}
#endif // ENABLE_GENERALIZED_CHAR_STR

ASN1error_e ASN1EncSetError(ASN1encoding_t enc, ASN1error_e err)
{
    ASN1INTERNencoding_t e = (ASN1INTERNencoding_t) enc;
    EncAssert(enc, ASN1_SUCCESS <= err);
    while (e)
    {
        e->info.err = err;
        if (e == e->parent)
        {
            break;
        }
        e = e->parent;
    }
    return err;
}

ASN1error_e ASN1DecSetError(ASN1decoding_t dec, ASN1error_e err)
{
    ASN1INTERNdecoding_t d = (ASN1INTERNdecoding_t) dec;
    DecAssert(dec, ASN1_SUCCESS <= err);
    while (d)
    {
        d->info.err = err;
        if (d == d->parent)
        {
            break;
        }
        d = d->parent;
    }
    return err;
}

#ifdef _DEBUG
void MyDebugBreak(void)
{
    DebugBreak();
}
#endif // _DEBUG

// Memory manager for decoder
LPVOID DecMemAlloc ( ASN1decoding_t dec, ASN1uint32_t cbSize )
{
    LPVOID lp = NULL;
    ASN1INTERNdecoding_t lpInfo = (ASN1INTERNdecoding_t) dec;
    // make sure the size is 4-byte aligned
    ASN1_SIZE_ALIGNED(cbSize);
    // accumulate the total size
    lpInfo->cbLinearBufSize += cbSize;
    // allocate the buffer
    if (lpInfo->fExtBuf)
    {
        if (lpInfo->cbRemExtBufSize >= cbSize)
        {
            lp = lpInfo->lpRemExtBuf;
            lpInfo->lpRemExtBuf += cbSize;
            lpInfo->cbRemExtBufSize -= cbSize;
        }
        else
        {
            DecAssert(dec, FALSE);
        }
    }
    else
    {
        lp = MemAllocEx(dec, cbSize, TRUE); // zero memory
    }
    // make sure to propagate error
    if (lp == NULL)
    {
        ASN1DecSetError(dec, ASN1_ERR_MEMORY);
    }
    return lp;
}

void DecMemFree ( ASN1decoding_t dec, LPVOID lpData )
{
    if (! ((ASN1INTERNdecoding_t) dec)->fExtBuf)
    {
        MemFree(lpData);
    }
}

LPVOID DecMemReAlloc ( ASN1decoding_t dec, LPVOID lpData, ASN1uint32_t cbSize )
{
    LPVOID lp = NULL;
    // make sure the size is 4-byte aligned
    ASN1_SIZE_ALIGNED(cbSize);
    // accumulate the total size
    ((ASN1INTERNdecoding_t) dec)->cbLinearBufSize += cbSize;
    // allocate the buffer
    if (((ASN1INTERNdecoding_t) dec)->fExtBuf)
    {
        if (lpData == NULL)
        {
            lp = DecMemAlloc(dec, cbSize);
        }
        else
        {
            DecAssert(dec, FALSE);
        }
    }
    else
    {
        lp = MemReAllocEx(dec, lpData, cbSize, TRUE); // zero memory
    }
    // make sure to propagate error
    if (lp == NULL)
    {
        ASN1DecSetError(dec, ASN1_ERR_MEMORY);
    }
    return lp;
}



/***
*char *ms_bSearch() - do a binary search on an array
*
*Purpose:
*       Does a binary search of a sorted array for a key.
*
*Entry:
*       const char *key    - key to search for
*       const char *base   - base of sorted array to search
*       unsigned int num   - number of elements in array
*       unsigned int width - number of bytes per element
*       int (*compare)()   - pointer to function that compares two array
*               elements, returning neg when #1 < #2, pos when #1 > #2, and
*               0 when they are equal. Function is passed pointers to two
*               array elements.
*
*Exit:
*       if key is found:
*               returns pointer to occurrence of key in array
*       if key is not found:
*               returns NULL
*
*Exceptions:
*
*******************************************************************************/

void * ms_bSearch (
        const void *key,
        const void *base,
        size_t num,
        size_t width,
        int (__cdecl *compare)(const void *, const void *)
        )
{
        char *lo = (char *)base;
        char *hi = (char *)base + (num - 1) * width;
        char *mid;
        unsigned int half;
        int result;

        while (lo <= hi)
                if (half = num / 2)
                {
                        mid = lo + (num & 1 ? half : (half - 1)) * width;
                        if (!(result = (*compare)(key,mid)))
                                return(mid);
                        else if (result < 0)
                        {
                                hi = mid - width;
                                num = num & 1 ? half : half-1;
                        }
                        else    {
                                lo = mid + width;
                                num = half;
                        }
                }
                else if (num)
                        return((*compare)(key,lo) ? NULL : lo);
                else
                        break;

        return(NULL);
}


int IsDigit(char p)
{
      return ((p >= ('0')) && (p <=('9'))) ? 1 : 0 ;
}



// BUGBUG - This is a copy of the same routine in nmutil
// BUGBUG - Remove this when we convert our files to CPP and use nmutil
/*  D E C I M A L  S T R I N G  T O  U  I  N  T  */
/*-------------------------------------------------------------------------
    %%Function: DecimalStringToUINT
    
-------------------------------------------------------------------------*/
unsigned int  DecimalStringToUINT(char * pcszString, ASN1uint32_t cch)
{
	unsigned int uRet = 0;
	char * pszStr =  pcszString;
	while (cch-- && ('\0') != pszStr[0])
	{
		uRet = (10 * uRet) + (int) (pszStr[0] - ('0'));
		pszStr++; // NOTE: DBCS characters are not allowed!
	}
	return uRet;
}


#ifdef ENABLE_BER
int My_memcmp(ASN1octet_t *pBuf1, ASN1uint32_t cbBuf1Size, ASN1octet_t *pBuf2, ASN1uint32_t cbBuf2Size)
{
    int diff;
    ASN1uint32_t cbMinBufSize = (cbBuf1Size < cbBuf2Size) ? cbBuf1Size : cbBuf2Size;
    while (cbMinBufSize--)
    {
        diff = (int) (ASN1uint32_t) *pBuf1++ - (int) (ASN1uint32_t) *pBuf2++;
        if (0 != diff)
        {
            return diff;
        }
    }
    return ((int) cbBuf1Size - (int) cbBuf2Size);
}
#endif // ENABLE_BER

#ifdef ENABLE_DOUBLE

const ASN1octet_t ASN1double_pinf_octets[] = DBL_PINF;
const ASN1octet_t ASN1double_minf_octets[] = DBL_MINF;

/* get a positive infinite double value */
double ASN1double_pinf()
{
    double val;
    MyAssert(sizeof(val) == sizeof(ASN1double_pinf_octets));
    RtlCopyMemory(&val, ASN1double_pinf_octets, sizeof(ASN1double_pinf_octets));
    return val;
}

/* get a negative infinite double value */
double ASN1double_minf()
{
    double val;
    MyAssert(sizeof(val) == sizeof(ASN1double_minf_octets));
    RtlCopyMemory(&val, ASN1double_minf_octets, sizeof(ASN1double_minf_octets));
    return val;
}

/* check if double is plus infinity */
int ASN1double_ispinf(double d)
{
#if HAS_FPCLASS
    return !finite(d) && fpclass(d) == FP_PINF;
#elif HAS_ISINF
    return !finite(d) && isinf(d) && copysign(1.0, d) > 0.0;
#else
#error "cannot encode NaN fp values"
#endif
}

/* check if double is minus infinity */
int ASN1double_isminf(double d)
{
#if HAS_FPCLASS
    return !finite(d) && fpclass(d) == FP_NINF;
#elif HAS_ISINF
    return !finite(d) && isinf(d) && copysign(1.0, d) < 0.0;
#else
#error "cannot encode NaN fp values"
#endif
}

#endif

/* convert a real value into a double */
#ifdef ENABLE_REAL
double ASN1real2double(ASN1real_t *val)
{
    ASN1intx_t exp;
    ASN1int32_t e;
    double m;

    switch (val->type) {
    case eReal_Normal:
        m = ASN1intx2double(&val->mantissa);
        if (val->base == 10) {
            return m * pow(10.0, (double)ASN1intx2int32(&val->exponent));
        } else {
            if (val->base == 2) {
                if (! ASN1intx_dup(&exp, &val->exponent))
                {
                    return 0.0;
                }
            } else if (val->base == 8) {
                ASN1intx_muloctet(&exp, &val->exponent, 3);
            } else if (val->base == 16) {
                ASN1intx_muloctet(&exp, &val->exponent, 4);
            } else {
                return 0.0;
            }
            e = ASN1intx2int32(&exp);
            ASN1intx_free(&exp);
            return ldexp(m, e);
        }
    case eReal_PlusInfinity:
        return ASN1double_pinf();
    case eReal_MinusInfinity:
        return ASN1double_minf();
    default:
        return 0.0;
    }
}
#endif // ENABLE_REAL

/* free a real value */
#ifdef ENABLE_REAL
void ASN1real_free(ASN1real_t *val)
{
    ASN1intx_free(&val->mantissa);
    ASN1intx_free(&val->exponent);
}
#endif // ENABLE_REAL

/* free a bitstring value */
void ASN1bitstring_free(ASN1bitstring_t *val)
{
    if (val)
    {
        // lonchanc: no need to check length because zeroed decoded buffers.
        MemFree(val->value);
    }
}

/* free an octet string value */
void ASN1octetstring_free(ASN1octetstring_t *val)
{
    if (val)
    {
        // lonchanc: no need to check length because zeroed decoded buffers.
        MemFree(val->value);
    }
}

/* free an object identifier value */
void ASN1objectidentifier_free(ASN1objectidentifier_t *val)
{
    if (val)
    {
        // lonchanc: we allocate the entire object identifer as a chunk.
        // as a result, we free it only once as a chunk.
        MemFree(*val);
    }
}

/* free a string value */
#ifdef ENABLE_BER
void ASN1charstring_free(ASN1charstring_t *val)
{
    if (val)
    {
        // lonchanc: no need to check length because zeroed decoded buffers.
        MemFree(val->value);
    }
}
#endif // ENABLE_BER

/* free a 16 bit string value */
void ASN1char16string_free(ASN1char16string_t *val)
{
    if (val)
    {
        // lonchanc: no need to check length because zeroed decoded buffers.
        MemFree(val->value);
    }
}

/* free a 32 bit string value */
#ifdef ENABLE_BER
void ASN1char32string_free(ASN1char32string_t *val)
{
    if (val)
    {
        // lonchanc: no need to check length because zeroed decoded buffers.
        MemFree(val->value);
    }
}
#endif // ENABLE_BER

/* free a zero-terminated string value */
void ASN1ztcharstring_free(ASN1ztcharstring_t val)
{
    MemFree(val);
}

/* free a zero-terminated 16 bit string value */
#ifdef ENABLE_BER
void ASN1ztchar16string_free(ASN1ztchar16string_t val)
{
    MemFree(val);
}
#endif // ENABLE_BER

/* free a zero-terminated 32 bit string value */
#ifdef ENABLE_BER
void ASN1ztchar32string_free(ASN1ztchar32string_t val)
{
    MemFree(val);
}
#endif // ENABLE_BER

/* free an external value */
#ifdef ENABLE_EXTERNAL
void ASN1external_free(ASN1external_t *val)
{
    if (val)
    {
        switch (val->identification.o)
        {
        case ASN1external_identification_syntax_o:
            ASN1objectidentifier_free(&val->identification.u.syntax);
            break;
        case ASN1external_identification_presentation_context_id_o:
            break;
        case ASN1external_identification_context_negotiation_o:
            ASN1objectidentifier_free(
                &val->identification.u.context_negotiation.transfer_syntax);
            break;
        }
        ASN1ztcharstring_free(val->data_value_descriptor);
        switch (val->data_value.o)
        {
        case ASN1external_data_value_notation_o:
            ASN1open_free(&val->data_value.u.notation);
            break;
        case ASN1external_data_value_encoded_o:
            ASN1bitstring_free(&val->data_value.u.encoded);
            break;
        }
    }
}
#endif // ENABLE_EXTERNAL

/* free an embedded pdv value */
#ifdef ENABLE_EMBEDDED_PDV
void ASN1embeddedpdv_free(ASN1embeddedpdv_t *val)
{
    if (val)
    {
        switch (val->identification.o)
        {
        case ASN1embeddedpdv_identification_syntaxes_o:
            ASN1objectidentifier_free(&val->identification.u.syntaxes.abstract);
            ASN1objectidentifier_free(&val->identification.u.syntaxes.transfer);
            break;
        case ASN1embeddedpdv_identification_syntax_o:
            ASN1objectidentifier_free(&val->identification.u.syntax);
            break;
        case ASN1embeddedpdv_identification_presentation_context_id_o:
            break;
        case ASN1embeddedpdv_identification_context_negotiation_o:
            ASN1objectidentifier_free(
                &val->identification.u.context_negotiation.transfer_syntax);
            break;
        case ASN1embeddedpdv_identification_transfer_syntax_o:
            ASN1objectidentifier_free(&val->identification.u.transfer_syntax);
            break;
        case ASN1embeddedpdv_identification_fixed_o:
            break;
        }
        switch (val->data_value.o)
        {
        case ASN1embeddedpdv_data_value_notation_o:
            ASN1open_free(&val->data_value.u.notation);
            break;
        case ASN1embeddedpdv_data_value_encoded_o:
            ASN1bitstring_free(&val->data_value.u.encoded);
            break;
        }
    }
}
#endif // ENABLE_EMBEDDED_PDV

/* free a character string value */
#ifdef ENABLE_GENERALIZED_CHAR_STR
void ASN1characterstring_free(ASN1characterstring_t *val)
{
    if (val)
    {
        switch (val->identification.o)
        {
        case ASN1characterstring_identification_syntaxes_o:
            ASN1objectidentifier_free(&val->identification.u.syntaxes.abstract);
            ASN1objectidentifier_free(&val->identification.u.syntaxes.transfer);
            break;
        case ASN1characterstring_identification_syntax_o:
            ASN1objectidentifier_free(&val->identification.u.syntax);
            break;
        case ASN1characterstring_identification_presentation_context_id_o:
            break;
        case ASN1characterstring_identification_context_negotiation_o:
            ASN1objectidentifier_free(
                &val->identification.u.context_negotiation.transfer_syntax);
            break;
        case ASN1characterstring_identification_transfer_syntax_o:
            ASN1objectidentifier_free(&val->identification.u.transfer_syntax);
            break;
        case ASN1characterstring_identification_fixed_o:
            break;
        }
        switch (val->data_value.o)
        {
        case ASN1characterstring_data_value_notation_o:
            ASN1open_free(&val->data_value.u.notation);
            break;
        case ASN1characterstring_data_value_encoded_o:
            ASN1octetstring_free(&val->data_value.u.encoded);
            break;
        }
    }
}
#endif // ENABLE_GENERALIZED_CHAR_STR

/* free an open type value */
#ifdef ENABLE_BER
void ASN1open_free(ASN1open_t *val)
{
    if (val)
    {
        // lonchanc: no need to check length because zeroed decoded buffers.
        MemFree(val->encoded);
    }
}
#endif // ENABLE_BER

#ifdef ENABLE_BER
void ASN1utf8string_free(ASN1wstring_t *val)
{
    if (val)
    {
        // lonchanc: no need to check length because zeroed decoded buffers.
        MemFree(val->value);
    }
}
#endif // ENABLE_BER

/* convert a generalized time value into a string */
int ASN1generalizedtime2string(char *dst, ASN1generalizedtime_t *val)
{
    if (dst && val)
    {
        sprintf(dst, "%04d%02d%02d%02d%02d%02d",
            val->year, val->month, val->day,
            val->hour, val->minute, val->second);
        if (val->millisecond) {
            if (!(val->millisecond % 100))
                sprintf(dst + 14, ".%01d", val->millisecond / 100);
            else if (!(val->millisecond % 10))
                sprintf(dst + 14, ".%02d", val->millisecond / 10);
            else
                sprintf(dst + 14, ".%03d", val->millisecond);
        }
        if (val->universal)
            strcpy(dst + My_lstrlenA(dst), "Z");
        else if (val->diff > 0) {
            if (val->diff % 60) {
                sprintf(dst + My_lstrlenA(dst), "+%04d",
                    100 * (val->diff / 60) + (val->diff % 60));
            } else {
                sprintf(dst + My_lstrlenA(dst), "+%02d",
                    val->diff / 60);
            }
        } else if (val->diff < 0) {
            if (val->diff % 60) {
                sprintf(dst + My_lstrlenA(dst), "-%04d",
                    -100 * (val->diff / 60) - (val->diff % 60));
            } else {
                sprintf(dst + My_lstrlenA(dst), "-%02d",
                    -val->diff / 60);
            }
        }
        return 1;
    }
    return 0;
}

/* convert a utc time value into a string */
#ifdef ENABLE_BER
int ASN1utctime2string(char *dst, ASN1utctime_t *val)
{
    if (dst && val)
    {
        sprintf(dst, "%02d%02d%02d%02d%02d%02d",
            val->year, val->month, val->day,
            val->hour, val->minute, val->second);
        if (val->universal)
            strcpy(dst + My_lstrlenA(dst), "Z");
        else if (val->diff > 0) {
            if (val->diff % 60) {
                sprintf(dst + My_lstrlenA(dst), "+%04d",
                    100 * (val->diff / 60) + (val->diff % 60));
            } else {
                sprintf(dst + My_lstrlenA(dst), "+%02d",
                    val->diff / 60);
            }
        } else if (val->diff < 0) {
            if (val->diff % 60) {
                sprintf(dst + My_lstrlenA(dst), "-%04d",
                    -100 * (val->diff / 60) - (val->diff % 60));
            } else {
                sprintf(dst + My_lstrlenA(dst), "-%02d",
                    -val->diff / 60);
            }
        }
        return 1;
    }
    return 0;
}
#endif // ENABLE_BER

//
// scan the decimal fraction of a number based on the base
// base must be a power of 10
//
static DWORD scanfrac(char *p, char **e, DWORD base)
{
    DWORD ret = 0;

    while (IsDigit(*p)) {
        if (base != 1)
        {
            base /= 10;
            ret = (ret * 10) + (*p++ - '0');
        }
        else
        {
            p++;
        }
    }
    *e = p;
    return ret * base;
}

/* convert a string into a generalized time value */
int ASN1string2generalizedtime(ASN1generalizedtime_t *dst, char *val)
{
    if (dst && val)
    {
        int year, month, day, hour, minute, second, millisecond, diff, universal;
        char *p;
        DWORD frac;
        DWORD temp;

        millisecond = second = minute = universal = diff = 0;
        if (My_lstrlenA(val) < 10)
        {
            return 0;
        }
        year = DecimalStringToUINT(val, 4);
        month = DecimalStringToUINT((val+4), 2);
        day = DecimalStringToUINT((val+6), 2);
        hour = DecimalStringToUINT((val+8), 2);
        p = val + 10;
        if (*p == '.' || *p == ',')
        {
            const DWORD base = 10000000;
            frac = scanfrac(p+1, &p, base);
            temp = frac * 60;
            minute = (temp / base);
            frac = temp % base;
            temp = frac * 60;
            second = (temp / base);
            frac = temp % base;
            millisecond = (frac / (base / 1000) );
        }
        else if (IsDigit(*p))
        {
            minute = DecimalStringToUINT(p, 2);
            p += 2;
            if (*p == '.' || *p == ',')
            {
                const DWORD base = 100000;
                frac = scanfrac(p+1, &p, base);
                temp = frac * 60;
                second = (temp / base);
                frac = temp % base;
                millisecond = (frac / (base / 1000) );
            } else if (IsDigit(*p))
            {
                second = DecimalStringToUINT(p, 2);
                p += 2;
                if (*p == '.' || *p == ',')
                {
                    const DWORD base = 1000;
                    frac = scanfrac(p+1, &p, base);
                    millisecond = frac;
                }
            }
        }
        if (*p == 'Z')
        {
            universal = 1;
            p++;
        }
        else if (*p == '+')
        {
            const DWORD base = 10000;
            frac = scanfrac(p + 1, &p, base);
            diff = frac / (base/100) * 60 + frac % 100;
        }
        else if (*p == '-')
        {
            const DWORD base = 10000;
            frac = scanfrac(p + 1, &p, base);
            diff = -((int)(frac / (base/100) * 60 + frac % 100));
        }
        if (*p)
        {
            return 0;
        }
        dst->year = (ASN1uint16_t)year;
        dst->month = (ASN1uint8_t)month;
        dst->day = (ASN1uint8_t)day;
        dst->hour = (ASN1uint8_t)hour;
        dst->minute = (ASN1uint8_t)minute;
        dst->second = (ASN1uint8_t)second;
        dst->millisecond = (ASN1uint16_t)millisecond;
        dst->universal = (ASN1bool_t)universal;
        dst->diff = (ASN1uint16_t)diff;
        return 1;
    }
    return 0;
}

/* convert a string into a utc time value */
#ifdef ENABLE_BER
int ASN1string2utctime(ASN1utctime_t *dst, char *val)
{
    if (dst && val)
    {
        char *p;
        DWORD frac;

        if (My_lstrlenA(val) < 10)
            return 0;

        p = val;
        dst->year = (ASN1uint8_t) DecimalStringToUINT(p, 2);
        p += 2;
        dst->month = (ASN1uint8_t) DecimalStringToUINT(p, 2);
        p += 2;
        dst->day = (ASN1uint8_t) DecimalStringToUINT(p, 2);
        p += 2;
        dst->hour = (ASN1uint8_t) DecimalStringToUINT(p, 2);
        p += 2;
        dst->minute = (ASN1uint8_t) DecimalStringToUINT(p, 2);
        p += 2;

        if (IsDigit(*p))
        {
            dst->second = (ASN1uint8_t) DecimalStringToUINT(p, 2);
            p += 2;
        }
        else
        {
            dst->second = 0;
        }

        dst->universal = 0;
        dst->diff = 0;

        if (*p == 'Z')
        {
            dst->universal = 1;
            p++;
        }
        else if (*p == '+')
        {
            const DWORD base = 10000;
            frac = scanfrac(p + 1, &p, base);
            dst->diff = (int)(frac / (base/100) * 60 + frac % 100);
        }
        else if (*p == '-')
        {
            const DWORD base = 10000;
            frac = scanfrac(p + 1, &p, base);
            dst->diff = -((int)(frac / (base/100) * 60 + frac % 100));
        }
        return ((*p) ? 0 : 1);
    }
    return 0;
}
#endif // ENABLE_BER


ASN1uint32_t GetObjectIdentifierCount(ASN1objectidentifier_t val)
{
    ASN1uint32_t cObjIds = 0;
    while (val)
    {
        cObjIds++;
        val = val->next;
    }
    return cObjIds;
}

ASN1uint32_t CopyObjectIdentifier(ASN1objectidentifier_t dst, ASN1objectidentifier_t src)
{
    while (dst && src)
    {
        dst->value = src->value;
        dst = dst->next;
        src = src->next;
    }
    return ((! dst) && (! src));
}

ASN1objectidentifier_t DecAllocObjectIdentifier(ASN1decoding_t dec, ASN1uint32_t cObjIds)
{
    ASN1objectidentifier_t p, q;
    ASN1uint32_t i;
    p = (ASN1objectidentifier_t) DecMemAlloc(dec, cObjIds * sizeof(struct ASN1objectidentifier_s));
    if (p)
    {
        for (q = p, i = 0; i < cObjIds-1; i++)
        {
            q->value = 0;
            q->next = (ASN1objectidentifier_t) ((char *) q + sizeof(struct ASN1objectidentifier_s));
            q = q->next;
        }
        q->next = NULL;
    }
    return p;
}

void DecFreeObjectIdentifier(ASN1decoding_t dec, ASN1objectidentifier_t p)
{
    DecMemFree(dec, p);
}



