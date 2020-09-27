/*
 *  Microsoft Confidential
 *  Copyright (C) Microsoft Corporation 1993,1994,1996
 *  All Rights Reserved.
 *
 *  NFMCOMP.H -- definitions for NFMCOMP.C
 *
 *  History:
 *      13-Feb-1994     msliger     revised type names, ie, UINT16 -> UINT.
 *                                  normalized MCI_MEMORY type.
 *      23-Feb-1994     msliger     changed how LGM defines work
 *      24-Feb-1994     msliger     Changed MCI_MEMORY to MI_MEMORY.
 *      15-Mar-1994     msliger     Changes for 32 bits.
 *      22-Mar-1994     msliger     Changed !INT32 to BIT16.
 *                                  Changed interface USHORT -> UINT.
 *      12-May-1994     msliger     ifdef'd 1's complement LARGE_STORED_BLOCKS.
 *      26-Sep-1994     msliger     Changed pcbResult to a FAR pointer.
 *      12-Jun-1995     msliger     Found cases to increase MAX_GROWTH.
 *      10-Sep-1996     jhiggins    Added support for multiple contexts.
 */

#ifndef NO_LGM
#define LGM
#endif

/* --- types -------------------------------------------------------------- */

#ifndef DIAMONDAPI
#define DIAMONDAPI __cdecl
#endif

#ifndef _BYTE_DEFINED
#define _BYTE_DEFINED
typedef unsigned char  BYTE;
#endif

#ifndef _UINT_DEFINED
#define _UINT_DEFINED
typedef unsigned int UINT;
#endif

#ifndef _ULONG_DEFINED
#define _ULONG_DEFINED
typedef unsigned long  ULONG;
#endif

#ifndef FAR
#ifdef BIT16
#define FAR far
#else
#define FAR
#endif
#endif

#ifndef HUGE
#ifdef BIT16
#define HUGE huge
#else
#define HUGE
#endif
#endif

#ifndef _MI_MEMORY_DEFINED
#define _MI_MEMORY_DEFINED
typedef void HUGE *  MI_MEMORY;
#endif


/***    PFNALLOC - Memory allocation function for MCI
 *
 *  Entry:
 *      cb - Size in bytes of memory block to allocate
 *
 *  Exit-Success:
 *      Returns !NULL pointer to memory block
 *
 *  Exit-Failure:
 *      Returns NULL; insufficient memory
 */
#ifndef _PFNALLOC_DEFINED
#define _PFNALLOC_DEFINED
typedef MI_MEMORY (__cdecl FAR DIAMONDAPI *PFNALLOC)(ULONG cb);       /* pfnma */
#endif


/***    PFNFREE - Free memory function for MCI
 *
 *  Entry:
 *      pv - Memory block allocated by matching PFNALLOC function
 *
 *  Exit:
 *      Memory block freed.
 */
#ifndef _PFNFREE_DEFINED
#define _PFNFREE_DEFINED
typedef void (__cdecl FAR DIAMONDAPI *PFNFREE)(MI_MEMORY pv);          /* pfnmf */
#endif

/* --- warnings ----------------------------------------------------------- */

/* ATTENTION   There is a known problem where the compressor may become unstable
 *          if cbDest is too small.  (last_lit will not be reset, amongst
 *          others.)  If the current block fails because it would exceed
 *          cbDest, additional blocks are likely to be mis-compresssed, and
 *          likely to fault in the compressor.
 */

/* --- prototypes --------------------------------------------------------- */

extern int NFMcompress_init(void FAR *ctx, MI_MEMORY buf1,MI_MEMORY buf2);

extern int NFMcompress(void FAR *ctx, BYTE FAR *bfSrc, UINT cbSrc,
        BYTE FAR *bfDest, UINT cbDest,
        MI_MEMORY bfWrk1, MI_MEMORY bfWrk2,
#ifdef LGM
        MI_MEMORY bfWrk3, MI_MEMORY bfWrk4,
#endif
        char fhistory, UINT FAR *pcbDestRet);

extern void FAR *NFMcomp_create(PFNALLOC NFMalloc);

extern void NFMcomp_destroy(void FAR *ctx, PFNFREE NFMfree);

/* --- constants ---------------------------------------------------------- */

/* return codes */

#define     NFMsuccess      0       /* successful completion */
#define     NFMdestsmall    1       /* destination buffer is too small */
#define     NFMoutofmem     2       /* alloc returned an error (NULL) */
#define     NFMinvalid      3       /* source buffer invalid for operation */


#ifdef LARGE_STORED_BLOCKS
#define     MAX_GROWTH      12      /* maximum growth of a block */
#else
#define     MAX_GROWTH      8       /* maximum growth of a block */
#endif

#define     LIT_BUFSIZE     0x8000  /* literal buffer */
#define     DIST_BUFSIZE    0x8000  /* distance buffer */

/* ------------------------------------------------------------------------ */
