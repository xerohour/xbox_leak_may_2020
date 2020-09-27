/*************************************************************************

Copyright (C) 2001  Microsoft Corporation

Module Name:

    strmdecdbg.hpp

Abstract:

    Decoder BitStream Debugging & Performancer Analysis

Author:

    Sil Sanders (sils@microsoft.com) 14-Feb-2001

Revision History:

*************************************************************************/

#ifndef __STRMDECDBG_HPP_
#define __STRMDECDBG_HPP_

// #define OPT_HUFFMAN_DBGDMP
// #define OPT_HUFFMAN_GET

#ifdef __STREAMING_MODE_DECODE_
//#undef OPT_HUFFMAN_GET
#endif

#ifndef UNDER_CE
    // The follow defines help model CE behavior on the desktop x86
    // define HUFFMANGET_DEBUG_DUMP to get info about each huffman decode
    // warning the resulting file can be large.  e.g. 30 seconds cif => 50MB.
    //#   define HUFFMANGET_DEBUG_DUMP

    // define HUFFMANGET_DEBUG_DUMP_CACHE to dump info about each table reference in each huffman decode
    // warning the resulting file can be large.  e.g. 30 seconds cif => 50MB.
    //#   define HUFFMANGET_DEBUG_DUMP_CACHE

    // define HUFFMANGET_DEBUG_CACHE to dump frame-by-frame summary info about cache accessed based on the cache model below.
    //#   define HUFFMANGET_DEBUG_CACHE
#endif

#ifdef HUFFMANGET_DEBUG_CACHE
    // Select a cache model
#   if 1    // 8k suitable for ARM Intel SA1100 and SA1110
#       define DBG_DCACHE_LINE_SIZE 32
#       define DBG_DCACHE_WAYS 32
#       define DBG_DCACHE_WAYLINES 8
#       define DBG_CACHE_CONFLICT 64
#       define DBG_DPAGTBL_WAYS 32
#       define DBG_DPAGTBL_WAYENTRIES 1
#       define DBG_DPAGTBL_CONFLICTS 0
#   elif 1  // 16k suitable for SH3-7709a (HP540)
            // This is an I/D cache- either reduce # ways below 4 to reflect instruction use or raise conflict
#       define DBG_DCACHE_LINE_SIZE 16
#       define DBG_DCACHE_WAYS 2        // 4 -> 2 pretend other half used by instructions :O(...
#       define DBG_DCACHE_WAYLINES 256
#       define DBG_CACHE_CONFLICT 0
#       define DBG_DPAGTBL_WAYS 4
#       define DBG_DPAGTBL_WAYENTRIES 32
#       define DBG_DPAGTBL_CONFLICTS 0
#   elif 1  // 16k one-way data cache on the SH4 
#       define DBG_DCACHE_LINE_SIZE 32
#       define DBG_DCACHE_WAYS 1
#       define DBG_DCACHE_WAYLINES 512
#       define DBG_CACHE_CONFLICT 0
#       define DBG_DPAGTBL_WAYS 64
#       define DBG_DPAGTBL_WAYENTRIES 1
#       define DBG_DPAGTBL_CONFLICTS 0
#   elif 1  // 16k one-way data cache on the MIPS VR4122 (Casio E-125).  
            // The Vr4121 (E-115) is 8k (16 x 1 x 512)
#       define DBG_DCACHE_LINE_SIZE 16
#       define DBG_DCACHE_WAYS 1
#       define DBG_DCACHE_WAYLINES 1024
#       define DBG_CACHE_CONFLICT 3
#       define DBG_DPAGTBL_WAYS 32
#       define DBG_DPAGTBL_WAYENTRIES 1
#       define DBG_DPAGTBL_CONFLICTS 0
#   else   // 32k two-way data cache on the MIPS VR5432 (confirm params)
#       define DBG_DCACHE_LINE_SIZE 32
#       define DBG_DCACHE_WAYS 2
#       define DBG_DCACHE_WAYLINES 512
#       define DBG_CACHE_CONFLICT 3
#       define DBG_DPAGTBL_WAY 48
#       define DBG_DPAGTBL_WAYENTRIES 1
#       define DBG_DPAGTBL_CONFLICTS 0
#   endif
#       define DBG_DPAGTBL_PAGESIZE 4096
#endif // HUFFMANGET_DEBUG_CACHE

#ifdef OPT_HUFFMAN_DBGDMP
    // be careful about the size file you decode with this option - unprotected overflow.
    extern U32 opt_huffman_dbgdmp[0x20000];
    extern int opt_huffman_dbgdmp_idx;
#   define HUFFMANGET_DBG_DMP(x,t,i,c) opt_huffman_dbgdmp[opt_huffman_dbgdmp_idx++]=(x)
#   define HUFFMANGET_DBG_GLOBALS
#   define HUFFMANGET_DBG_CACHE(t,i,c)
#   define HUFFMANGET_DBG_STATUS(a,b,c)
#   define HUFFMANGET_DBG_HEADER(a,b)
#   define HUFFMANGET_DEBUG_ONLY(x)
#elif defined(HUFFMANGET_DEBUG_CACHE)
    // Model a RISC cache system
    extern FILE* fpHuffmanDump;
    extern int giDbgDCacheWays[DBG_DCACHE_WAYS*DBG_DCACHE_WAYLINES];
    extern int giDbgDCacheHits, giDbgDCacheMisses, giDbgDCacheFills, giDbgFillWay, giDbgIdx, giDbgLineAdr, giDbgModAdr;
    extern char gszDbgHeader[];
#   define HUFFMANGET_DBG_GLOBALS                                   \
    int giDbgDCacheWays[DBG_DCACHE_WAYS*DBG_DCACHE_WAYLINES] = {0}; \
    int giDbgDCacheHits = 0;                                        \
    int giDbgDCacheMisses = 0;                                      \
    int giDbgDCacheFills = 0;                                       \
    int giDbgFillWay = 0;                                           \
    int giDbgIdx, giDbgLineAdr, giDbgModAdr;                        \
    char gszDbgHeader[30] = {0};
#   define HUFFMANGET_DBG_DMP(x,t,i,c)
#   define HUFFMANGET_DBG_CACHE(t,i,c)                                                          \
        giDbgLineAdr = (int(t)+i*c)/DBG_DCACHE_LINE_SIZE;                                       \
        giDbgModAdr  = giDbgLineAdr%DBG_DCACHE_WAYLINES;                                        \
        for( giDbgIdx = 0; giDbgIdx < DBG_DCACHE_WAYS; giDbgIdx++ ) {                           \
            if (giDbgDCacheWays[giDbgIdx*DBG_DCACHE_WAYLINES+giDbgModAdr] == giDbgLineAdr)      \
                break;                                                                          \
        }                                                                                       \
        if (giDbgIdx < DBG_DCACHE_WAYS)                                                         \
            giDbgDCacheHits++;                                                                  \
        else {                                                                                  \
            giDbgDCacheMisses++;                                                                \
            if ( (++giDbgFillWay) >= DBG_DCACHE_WAYS )                                          \
                giDbgFillWay = 0;                                                               \
            if ( giDbgDCacheWays[giDbgFillWay*DBG_DCACHE_WAYLINES+giDbgModAdr]==0 )             \
                giDbgDCacheFills++;                                                             \
            assert( giDbgDCacheFills <= (DBG_DCACHE_WAYS*DBG_DCACHE_WAYLINES)  );               \
            giDbgDCacheWays[giDbgFillWay*DBG_DCACHE_WAYLINES+giDbgModAdr] = giDbgLineAdr;       \
        }                                                                                       \
        /* model others using the cache */                                                      \
        for( giDbgIdx = 0; giDbgIdx < DBG_CACHE_CONFLICT; giDbgIdx++ ) {                        \
            giDbgLineAdr = rand()%(DBG_DCACHE_WAYS*DBG_DCACHE_WAYLINES);                        \
            if ( giDbgDCacheWays[giDbgLineAdr] != 0 )                                           \
                giDbgDCacheWays[giDbgLineAdr] = -1;                                             \
        }
#   define HUFFMANGET_DBG_STATUS(a,b,c)
#   define HUFFMANGET_DBG_HEADER(s,t)                                                   \
        if (fpHuffmanDump!=NULL) {                                                      \
            if ( (giDbgDCacheHits+giDbgDCacheMisses+giDbgDCacheFills)!=0 )              \
                fprintf(fpHuffmanDump,"%s# %d %d %d\n",                                 \
                    gszDbgHeader,giDbgDCacheHits,giDbgDCacheMisses,giDbgDCacheFills );  \
            strncpy( gszDbgHeader, s, 29 );                                             \
        }                                                                               \
        memset(giDbgDCacheWays,0,DBG_DCACHE_WAYS*DBG_DCACHE_WAYLINES*sizeof(int));      \
        giDbgDCacheHits = giDbgDCacheMisses = giDbgDCacheFills = 0;
#   define HUFFMANGET_DEBUG_ONLY(x) x
#   define HUFFMANGET_DEBUG
#elif defined(HUFFMANGET_DEBUG_DUMP) || defined(HUFFMANGET_DEBUG_DUMP_CACHE)
    // usage:
    // to get a list of gets per table, use:
    //   _sort -k 1,8 HuffmanGet.txt | uniq -c -w 8
    // where column 1 is the count and column 2 is the table address
    // to get a list of the tables and indexes used:
    //   _sort -k 1,8 -u HuffmanGet.txt 
    // where column 1 is the table, column 2 is the index and column 3 is the value returned.
	// to get a list of tables and their sizes
	//   _grep -i -e "  " -e "_x8" HuffmanGet.txt
    extern FILE* fpHuffmanDump;
#   define HUFFMANGET_DBG_GLOBALS
#   if defined(HUFFMANGET_DEBUG_DUMP)
#     define HUFFMANGET_DBG_DMP(x,t,i,c) if (fpHuffmanDump!=NULL) { fprintf(fpHuffmanDump,"%08x_ %d %d %d\r\n",((int)t),(i),(x),(c)); }
#   else
#     define HUFFMANGET_DBG_DMP(x,t,i,c)
#   endif
#   if defined(HUFFMANGET_DEBUG_DUMP_CACHE)
#     define HUFFMANGET_DBG_CACHE(t,i,c) if (fpHuffmanDump!=NULL) { fprintf(fpHuffmanDump,"%08x~ %d %d\r\n",((int)t),(i),(c)); }
#   else
#     define HUFFMANGET_DBG_CACHE(t,i,c)
#   endif
#   define HUFFMANGET_DBG_STATUS(a,b,c)  if (fpHuffmanDump!=NULL) { fprintf(fpHuffmanDump,"%08x  %d %x\r\n",((int)a),(b),(c)); }
#   define HUFFMANGET_DBG_HEADER(s,t)    if (fpHuffmanDump!=NULL) { fprintf(fpHuffmanDump,"%s\r\n",(s) ); }
#   define HUFFMANGET_DEBUG_ONLY(x) x
#   define HUFFMANGET_DEBUG
#else
#   define HUFFMANGET_DBG_DMP(x,t,i,c)
#   define HUFFMANGET_DBG_GLOBALS
#   define HUFFMANGET_DBG_CACHE(t,i,c)
#   define HUFFMANGET_DBG_STATUS(a,b,c)
#   if defined(PROFILE) && defined(PROFILE_FRAMES)
        extern void TypeFrameProfile( int iType );
#       define HUFFMANGET_DBG_HEADER(a,t) TypeFrameProfile(t);
#   elif defined(PROFILE_FRAMES)
#       define HUFFMANGET_DBG_HEADER(a,t) printf("%s\r\n",a);
#   else
#       define HUFFMANGET_DBG_HEADER(a,t)
#   endif
#   define HUFFMANGET_DEBUG_ONLY(x)
#endif

#ifndef DEBUG_ONLY
#   if defined(DEBUG) || defined(_DEBUG)
#       define DEBUG_ONLY(a) a
#   else
#       define DEBUG_ONLY(a)
#   endif
#endif // DEBUG_ONLY

#endif // __STRMDECDBG_HPP_

