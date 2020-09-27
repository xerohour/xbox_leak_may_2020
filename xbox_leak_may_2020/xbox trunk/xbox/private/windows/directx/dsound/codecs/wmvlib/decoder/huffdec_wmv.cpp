//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       huffman.cpp
//
//--------------------------------------------------------------------------
#include "bldsetup.h"

#include "xplatform.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "stdio.h"
#include "huffman_wmv.hpp"
#include "strmdec_wmv.hpp"
#ifndef CHECK_ALLOC
#define CHECK_ALLOC(ptr) if ((ptr)==NULL_WMV) { vr=vrOutOfMemory; goto lerror; }
#endif
#define MAX_DEC_TABLES 1000

typedef struct {
  U32_WMV code;
  U32_WMV length;
  U32_WMV table;
} InitEncInfo;


Void_WMV Huffman_WMV_construct(Huffman_WMV * pThis)
{
  pThis->m_tableInfo     = NULL_WMV;
  pThis->m_initInfo      = NULL_WMV;
  pThis->m_encInfo       = NULL_WMV;
  pThis->m_decInfo       = NULL_WMV;

  pThis->m_alphabetSize  = 0;
  pThis->m_maxCodeLength = 0;
  pThis->m_allocDecEntries=0;
  pThis->m_allocTables=0;
}

Void_WMV Huffman_WMV_destruct(Huffman_WMV * pThis)
{
  if ( pThis->m_tableInfo != NULL_WMV ) {
      DEBUG_HEAP_SUB( g_iHeapLastClass, pThis->m_allocTables * sizeof(TableInfo) );
  }
  delete [] pThis->m_tableInfo;
//  delete [] m_initInfo; // deleted elsewhere
//  delete [] m_encInfo;
  if ( pThis->m_decInfo != NULL ) {
      DEBUG_HEAP_SUB( g_iHeapLastClass, pThis->m_allocDecEntries * sizeof(HuffDecInfo) );
  }
  delete [] pThis->m_decInfo;
}

VResult Huffman_WMV_findTables(Huffman_WMV * pThis, I32_WMV &totalTableNum, I32_WMV *maxBits, I32_WMV iMAX_STAGES);
VResult Huffman_WMV_allocTables(Huffman_WMV * pThis, I32_WMV numTables);
void Huffman_WMV_fillEntry(Huffman_WMV * pThis, I32_WMV cwd, I32_WMV length, I32_WMV tableNum,
                        I32_WMV index, HuffDecInfo *currDecTable);

extern "C"
ULONG
_cdecl
DbgPrint(
    PCH Format,
    ...
    );

VResult Huffman_WMV_init (Huffman_WMV * pThis, U32_WMV uiUserData, tPackHuffmanCode_WMV huffArray[], I32_WMV *maxBits, I32_WMV iMAX_STAGES)
{
    VResult vr;
    I32_WMV i, j, numTables;

    pThis->m_uiUserData = uiUserData;

//  assert(sizeof(I32_WMV) == sizeof(int32)); // GetMask is only 32 bits wide
    pThis->m_maxCodeLength = 0;
    pThis->m_alphabetSize = huffArray[0].code;
    huffArray = huffArray++;

    pThis->m_encInfo = huffArray;
    pThis->m_maxCodeLength = 0;
    for (j=0; j<pThis->m_alphabetSize; j++) {
      //assert(*huffArray != -1);
      if ((U32_WMV) pThis->m_encInfo[j].length > (U32_WMV)pThis->m_maxCodeLength)
          pThis->m_maxCodeLength = pThis->m_encInfo[j].length;
  }
  // Generate decoder table if necessary
  //vr = initDecTable(maxBits);

#ifndef _CASIO_VIDEO_
#ifdef SMALL_HUFFMAN
  assert(pThis->m_alphabetSize <= (1<<12)); // so we can use 12 bits for symbol field
#else
  assert(pThis->m_alphabetSize <= (1<<16));
#endif
#endif

  pThis->m_initInfo = new TableInitInfo[MAX_DEC_TABLES];
  CHECK_ALLOC(pThis->m_initInfo);
  DEBUG_HEAP_ADD( g_iHeapLastClass, MAX_DEC_TABLES * sizeof(TableInitInfo));

  // Find the # of decoder tables we will need
  vr = Huffman_WMV_findTables(pThis,numTables, maxBits, iMAX_STAGES);
//printf("%d\n",numTables);
  if (vr.failed()) goto lerror;

  // Allocate tables
  vr = Huffman_WMV_allocTables(pThis,numTables);
  if (vr.failed()) goto lerror;

  // Now we are done with table info and can delete it
  delete [] pThis->m_initInfo;
  pThis->m_initInfo=NULL_WMV;
  DEBUG_HEAP_SUB( g_iHeapLastClass, MAX_DEC_TABLES * sizeof(TableInitInfo) );

  for (i=0; i<pThis->m_alphabetSize; i++) {
    Huffman_WMV_fillEntry(pThis, pThis->m_encInfo[i].code , pThis->m_encInfo[i].length , 0, i, pThis->m_decInfo);
  }

#if 0 && DBG

  HuffDecInfo *pdecinfo;
  DWORD Count = numTables;

  DbgPrint("WORD g_Huffman_[] =\n{\n    // Tables");

  for (i = 0; i < numTables; i++)
  {
      if (i % 8 == 0)
      {
          DbgPrint("\n    ");
      }

      DbgPrint("0x%04X, ", Count | (pThis->m_tableInfo[i].bits << 12));

      Count += (1 << pThis->m_tableInfo[i].bits);
  }

  for (i = 0; i < numTables; i++)
  {
      pdecinfo = pThis->m_tableInfo[i].table;

      DWORD j;

      DbgPrint("\n    // Table %d", i);

      for (j = 0; j < (DWORD)(1 << pThis->m_tableInfo[i].bits); j++)
      {
          if (j % 8 == 0)
          {
              DbgPrint("\n    ");
          }

          DbgPrint("0x%04X, ", *(WORD *)pdecinfo);

          pdecinfo++;
      }

      DbgPrint("\n");
  }

  DbgPrint("\n};");

#endif

  return vrNoError;

 lerror:
  if (   pThis->m_initInfo != NULL_WMV ) {
      DEBUG_HEAP_SUB( g_iHeapLastClass, MAX_DEC_TABLES * sizeof(TableInitInfo) );
  }
  delete [] pThis->m_initInfo;
  pThis->m_initInfo=NULL_WMV;
  return vr;
}

VResult Huffman_WMV_findTables(Huffman_WMV * pThis, I32_WMV &totalTableNum, I32_WMV *maxBits, I32_WMV iMAX_STAGES)
{
  VResult vr;
  I32_WMV i, j, stage, tableNum, start, end, prefix, excessBits, nextTable;
  InitEncInfo *initEncInfo = NULL_WMV;
  I32_WMV myMaxBits[4];
  Bool_WMV found;
  // Set # of bits for each stage
  if (maxBits == NULL_WMV) {
    iMAX_STAGES = 3;
    myMaxBits[0] = BITS_STAGE1;
    myMaxBits[1] = BITS_STAGE2;
    myMaxBits[2] = pThis->m_maxCodeLength - (myMaxBits[0] + myMaxBits[1]);
  } else {
    for (i=0; i<iMAX_STAGES; i++) myMaxBits[i] = maxBits[i];
  }
#ifndef HITACHI
  initEncInfo = new InitEncInfo[pThis->m_alphabetSize];
#else
  initEncInfo =  (InitEncInfo *)wmvalloc(pThis->m_alphabetSize * (sizeof(InitEncInfo)));
#endif      
  CHECK_ALLOC(initEncInfo);
  DEBUG_HEAP_ADD( g_iHeapLastClass, pThis->m_alphabetSize * sizeof(InitEncInfo) );

#ifndef _CASIO_VIDEO_
  assert(MAX_DEC_TABLES <= (1<<16));
#endif

  for (i=0; i<pThis->m_alphabetSize; i++) {
    initEncInfo[i].code   = pThis->m_encInfo[i].code ;
    initEncInfo[i].length = pThis->m_encInfo[i].length ;
    initEncInfo[i].table  = 0;
  }
  pThis->m_initInfo[0].maxBits = pThis->m_maxCodeLength;
  for (i=1; i<MAX_DEC_TABLES; i++) {
    pThis->m_initInfo[i].maxBits = 0;
  }

  totalTableNum = 1;
  end           = 0;
  for (stage=0; stage<iMAX_STAGES; stage++) {
    start = end;
    end   = totalTableNum;
//    lprintf(2, "    At stage %d, we have %d tables", stage, end-start);
    for (tableNum=start; tableNum<end; tableNum++) {
      pThis->m_initInfo[tableNum].start = totalTableNum;
      pThis->m_initInfo[tableNum].end   = pThis->m_initInfo[tableNum].start;
      if (pThis->m_initInfo[tableNum].maxBits <= myMaxBits[stage]) {
        pThis->m_initInfo[tableNum].bits = pThis->m_initInfo[tableNum].maxBits;
        continue;
      } else {
        pThis->m_initInfo[tableNum].bits = myMaxBits[stage];
      }
      for (i=0; i<pThis->m_alphabetSize; i++) {
        if (initEncInfo[i].table == (U32_WMV)tableNum) {
          if (initEncInfo[i].length > (U32_WMV)pThis->m_initInfo[tableNum].bits) {
            excessBits = initEncInfo[i].length - pThis->m_initInfo[tableNum].bits;
            prefix = (initEncInfo[i].code >> excessBits);
            initEncInfo[i].length = excessBits;
#ifdef OPT_HUFFMAN_GET_WMV
            initEncInfo[i].code &= (((U32_WMV)0xffffffff)>>(32-excessBits));
#else
            initEncInfo[i].code &= GetMask_WMV[excessBits];
#endif
            found = false;
            for (j=pThis->m_initInfo[tableNum].start; j<pThis->m_initInfo[tableNum].end; j++) {
              if (pThis->m_initInfo[j].prefix == prefix) {
                found = true;
                if (excessBits > pThis->m_initInfo[j].maxBits) {
                  pThis->m_initInfo[j].maxBits = excessBits;
                }
                nextTable = j;
                break;
              }
            }
            if (!found) {
#ifndef _CASIO_VIDEO_
              assert(totalTableNum < MAX_DEC_TABLES);
#endif
              pThis->m_initInfo[tableNum].end++;
              pThis->m_initInfo[totalTableNum].prefix = prefix;
              pThis->m_initInfo[totalTableNum].maxBits = excessBits;
              nextTable = totalTableNum;
              totalTableNum++;
            }
            initEncInfo[i].table = nextTable;
          }
        }
      }
    }
  }

  if(initEncInfo != NULL_WMV) {
    DEBUG_HEAP_SUB( g_iHeapLastClass, pThis->m_alphabetSize * sizeof(InitEncInfo) );
  }
#ifndef HITACHI
  delete [] initEncInfo;
#else 
  if(initEncInfo != NULL_WMV)
    wmvfree(initEncInfo);
#endif  
  return vrNoError;

 lerror:
  if(initEncInfo != NULL_WMV) {
      DEBUG_HEAP_SUB( g_iHeapLastClass, pThis->m_alphabetSize * sizeof(InitEncInfo) );
  }
#ifndef HITACHI
  delete [] initEncInfo;
#else 
  if(initEncInfo != NULL_WMV)
    wmvfree(initEncInfo);

#endif  
  return vr;
}

VResult Huffman_WMV_allocTables(Huffman_WMV * pThis, I32_WMV numTables)
{
  VResult vr;
  I32_WMV i, j;

  // Allocate memory and set offsets
//  lprintf(2, "    Allocating %d bytes for next stage info",
//          numTables*sizeof(TableInfo));
  if (numTables > pThis->m_allocTables) {   // support reinitialisation
      if (pThis->m_tableInfo) {
          delete [] pThis->m_tableInfo;
          DEBUG_HEAP_SUB( g_iHeapLastClass, pThis->m_allocTables * sizeof(TableInfo) );
      }
      pThis->m_tableInfo = new TableInfo[pThis->m_allocTables = numTables];
      CHECK_ALLOC(pThis->m_tableInfo);
      DEBUG_HEAP_ADD( g_iHeapLastClass, pThis->m_allocTables * sizeof(TableInfo) );
  }

  pThis->m_numDecEntries = 0;
  for (i=0; i<numTables; i++) pThis->m_numDecEntries += 1<<pThis->m_initInfo[i].bits;
//  lprintf(2, "    Allocating %d bytes for huffman decoder table",
//          m_numDecEntries*sizeof(HuffDecInfo));
  if (pThis->m_numDecEntries > pThis->m_allocDecEntries) {  // support reinitialisation
      if (pThis->m_decInfo) {
          delete [] pThis->m_decInfo; 
          DEBUG_HEAP_SUB( g_iHeapLastClass, pThis->m_allocDecEntries * sizeof(HuffDecInfo) );
      }
      pThis->m_decInfo = new HuffDecInfo[pThis->m_allocDecEntries = pThis->m_numDecEntries];
      CHECK_ALLOC(pThis->m_decInfo);
      DEBUG_HEAP_ADD( g_iHeapLastClass, pThis->m_allocDecEntries * sizeof(HuffDecInfo) );

      // set everything to -1 for mpeg4 tables because there are illegal
      // entries in mpeg4 table, this way, we can detect bad data.
      for (i = 0; i < pThis->m_allocDecEntries; i++) {
          pThis->m_decInfo[i].length = -1;
      }
  }

  HUFFMANGET_DBG_STATUS(pThis->m_decInfo,numTables,pThis->m_numDecEntries)

  pThis->m_numDecEntries = 0;

  for (i=0; i<numTables; i++) {
    pThis->m_tableInfo[i].bits  = pThis->m_initInfo[i].bits;
    pThis->m_tableInfo[i].table = pThis->m_decInfo+pThis->m_numDecEntries;
    pThis->m_numDecEntries     += 1<<pThis->m_initInfo[i].bits;
  }

  for (i=0; i<numTables; i++) {
    for (j=pThis->m_initInfo[i].start; j<pThis->m_initInfo[i].end; j++) {
      (pThis->m_tableInfo[i].table)[pThis->m_initInfo[j].prefix].symbol = j;
      (pThis->m_tableInfo[i].table)[pThis->m_initInfo[j].prefix].length = 0;
    }
    if ( (unsigned)pThis->m_initInfo[i].end >= (1U<<HUFFDEC_SYMBOL_BITS) ) {
        assert( (unsigned)pThis->m_initInfo[i].end < (1U<<HUFFDEC_SYMBOL_BITS));
        vr = vrOutOfBounds;
        goto lerror;
    }
  }


  return vrNoError;

 lerror:
  return vr;
}

DEBUG_ONLY( I32_WMV giMaxSymbol_WMV = 0; )
DEBUG_ONLY( I32_WMV giMaxLength_WMV = 0; )

void Huffman_WMV_fillEntry(Huffman_WMV * pThis, I32_WMV cwd, I32_WMV length, I32_WMV tableNum,
                        I32_WMV index, HuffDecInfo *currDecTable)
{
  I32_WMV start, end, excessBits, j, prefix;

  if (length == 0) return;

  DEBUG_ONLY( I32_WMV iMaxSymbol = 0 );
  DEBUG_ONLY( I32_WMV iMaxLength = 0 );

  while (true) {
#ifndef _CASIO_VIDEO_
#ifdef SMALL_HUFFMAN
    assert(pThis->m_tableInfo[tableNum].bits < (1<<4)); // only 4 bits for length
                                                // one less for next stage
#else
    assert(pThis->m_tableInfo[tableNum].bits < (1<<16)); // should never happen
#endif
#endif
    if (length <= pThis->m_tableInfo[tableNum].bits) {
      excessBits = pThis->m_tableInfo[tableNum].bits - length;
      start      = cwd << excessBits;
      end        = start + (1 << excessBits);
      for (j=start; j<end; j++) {
        currDecTable[j].symbol = index;
        currDecTable[j].length = length;
        DEBUG_ONLY( if (iMaxSymbol<index) iMaxSymbol = index );
        DEBUG_ONLY( if (iMaxLength<length) iMaxLength = length );
      }
      // verify the indexes and lengths fit.
#ifndef _Embedded_x86
      DEBUG_ONLY( assert( iMaxSymbol < (1U<<HUFFDEC_SYMBOL_BITS) && iMaxLength < (1U<<HUFFDEC_SYMBOL_BITS) ) );
#endif
      DEBUG_ONLY( if (giMaxSymbol_WMV < iMaxSymbol) giMaxSymbol_WMV = iMaxSymbol );
      DEBUG_ONLY( if (giMaxLength_WMV < iMaxLength) giMaxLength_WMV = iMaxLength );
      return;
    } else {
      excessBits   = length - pThis->m_tableInfo[tableNum].bits;
      prefix       = cwd >> excessBits;
#ifndef _CASIO_VIDEO_
      assert(currDecTable[prefix].length == 0);
#endif
      tableNum     = currDecTable[prefix].symbol;
#ifdef OPT_HUFFMAN_GET_WMV
      cwd          = cwd & (((U32_WMV)0xffffffff)>>(32-excessBits)); // take excessBits LSB's
#else
      cwd          = cwd & GetMask_WMV[excessBits]; // take excessBits LSB's
#endif
      length       = excessBits;
      currDecTable = pThis->m_tableInfo[tableNum].table;
    }
  }
}

#ifndef OPT_HUFFMAN_GET_WMV

inline U32_WMV BS_showBitsInline(CInputBitStream_WMV * pThis, register U32_WMV dwNumBits){

//    inline I32_WMV igetMaxBits (register U32_WMV dwNumBits, I32_WMV* rgCodeSym, U8_WMV* rgCodeSize) {
        I32_WMV iBitPattern;
        I32_WMV iTmp;

//      //assert ( dwNumBits <=16);
        //if (m_fEof == FALSE && ((U32_WMV) (m_cbBuflen << 3) >= dwNumBits || m_dwBitsLeft + (m_cbBuflen << 3) >= dwNumBits)) {
        if (dwNumBits <= pThis->m_dwBitsLeft) {
            iBitPattern = ((pThis->m_dwDot >> (pThis->m_dwBitsLeft - dwNumBits)) & GetMask_WMV[dwNumBits]); //0x00000fff;
//            m_dwBitsLeft -= rgCodeSize[iBitPattern];
//          if (rgCodeSym [iBitPattern] == 255)
//              m_fStreamStaus = ILLEGAL_PATTERN;
            return iBitPattern;
        }
//
// The next most common case is when we have lots of data left in the buffer.
// and we can fully load (i.e., all 32-bits) our accumulator.  This is
// hard-wired to allow an optimizer to go crazy with all of the constants.
// Note that the data is byte-swapped on the way in.
//
        U32_WMV dwBitsLeft = pThis->m_dwBitsLeft;
        if (pThis->m_cbBuflen >= 2) {
            iTmp = pThis->m_dwDot << 16;
            //m_dwDot <<= 16;
            iTmp |= (U32_WMV)(pThis->m_pBuffer[1]);
            iTmp |= (U32_WMV)(pThis->m_pBuffer[0]) << 8;
//            m_pBuffer += 2;
//            m_cbBuflen -= 2;
            dwBitsLeft += 16;
            iBitPattern = ((iTmp >> (dwBitsLeft - dwNumBits)) & GetMask_WMV[dwNumBits]);
            return iBitPattern;
        }
//
//
        iTmp = pThis->m_dwDot; 
        if (pThis->m_cbBuflen) {
#ifndef _CASIO_VIDEO_
            assert(pThis->m_cbBuflen==1);
#endif
            iTmp <<= 8;
            iTmp |= *pThis->m_pBuffer;
            //--m_cbBuflen;
            dwBitsLeft += 8;
        }

        if (dwBitsLeft >= dwNumBits){ 
            iBitPattern = ((iTmp >> (dwBitsLeft - dwNumBits)) & GetMask_WMV[dwNumBits]);
            return iBitPattern;
        }
        else {
            iBitPattern = (iTmp & GetMask_WMV[dwBitsLeft]) << (dwNumBits - dwBitsLeft);
            return iBitPattern;
        }
}
inline Void_WMV BS_flushBitsInline(CInputBitStream_WMV * pThis, register U32_WMV dwNumBits){
//      //assert ( dwNumBits <=16);
        //if (m_fEof == FALSE && ((U32_WMV) (m_cbBuflen << 3) >= dwNumBits || m_dwBitsLeft + (m_cbBuflen << 3) >= dwNumBits)) {
//
#if DBG
g_TotalBits += dwNumBits;
#endif DBG

        if (dwNumBits <= pThis->m_dwBitsLeft) {
            pThis->m_dwBitsLeft -= dwNumBits;
            return; 
        }
//
// The next most common case is when we h ave lots of data left in the buffer.
// and we can fully load (i.e., all 32-bits) our accumulator.  This is
// hard-wired to allow an optimizer to go crazy with all of the constants.
// Note that the data is byte-swapped on the way in.
//
#ifdef __STREAMING_MODE_DECODE_
        if (pThis->m_cbBuflen <= 4 && pThis->m_bNotEndOfFrame) {
            if (!WMVDecCBGetDataWithPadding(pThis))
                return;
        }
#endif

        if (pThis->m_cbBuflen >= 2) {
            pThis->m_dwDot <<= 16;
            pThis->m_dwDot |= (U32_WMV)(pThis->m_pBuffer[1]);
            pThis->m_dwDot |= (U32_WMV)(pThis->m_pBuffer[0]) << 8;
            pThis->m_pBuffer += 2;
            pThis->m_cbBuflen -= 2;
            pThis->m_dwBitsLeft += 16;
//          iBitPattern = ((m_dwDot >> (m_dwBitsLeft - dwNumBits)) & GetMask[dwNumBits]);
            pThis->m_dwBitsLeft -= dwNumBits;
            return;
        }
//
//
        for (; pThis->m_cbBuflen;) {
            pThis->m_dwDot <<= 8;
            pThis->m_dwDot |= *pThis->m_pBuffer++;
            --pThis->m_cbBuflen;
            pThis->m_dwBitsLeft += 8;
        }
        if (pThis->m_dwBitsLeft >= dwNumBits){ 
//          iBitPattern = ((m_dwDot >> (m_dwBitsLeft - dwNumBits)) & GetMask[dwNumBits]);
            pThis->m_dwBitsLeft -= dwNumBits;
            return;
        }
        else {
//          iBitPattern = (m_dwDot & GetMask[m_dwBitsLeft]) << (dwNumBits - m_dwBitsLeft);
            pThis->m_fStreamStaus = END_OF_FILE;
            return;
//          return 255;
            }
}

#endif
I32_WMV getHuffman(CInputBitStream_WMV *bs, void * pHuffman);

#ifndef OPT_HUFFMAN_GET_WMV

I32_WMV Huffman_WMV_get(Huffman_WMV * pThis, CInputBitStream_WMV *bs)
{
  //if (m_maxCodeLength == 0) {
  //    return 0; // CHECKCHECK
  //}
// #define OPTIMIZE_HUFFMAN_GET

  I32_WMV index, tableNum;
  HuffDecInfo *decTable;
  HUFFMANGET_DEBUG_ONLY( I32_WMV cCount = 1 );
 
  tableNum = 0;
  decTable = pThis->m_decInfo;
  while (true) { 
    index = BS_showBitsInline(bs, pThis->m_tableInfo[tableNum].bits);
    HUFFMANGET_DBG_CACHE(decTable,index,sizeof(HuffDecInfo));
    if (decTable[index].length > 0) {
      BS_flushBitsInline(bs, decTable[index].length);

      HUFFMANGET_DBG_DMP((I32_WMV)decTable[index].symbol,decTable,index,cCount);
      assert((I32_WMV)decTable[index].symbol >= 0);
      return ((I32_WMV)decTable[index].symbol);
    }
    else if (decTable[index].length == 0) {
        BS_flushBitsInline(bs, pThis->m_tableInfo[tableNum].bits);
        tableNum = decTable[index].symbol;
        decTable = pThis->m_tableInfo[tableNum].table;
        HUFFMANGET_DEBUG_ONLY( cCount++ );
    }
    else {
        // bad node -- mpeg4 table is sparsely populated...
        BS_setInvalid(bs);
        return 0;
    }
  }
#ifndef _CASIO_VIDEO_
  assert(false);
#endif
  HUFFMANGET_DBG_DMP(0,decTable,0,0);
  return 0;

}

#endif

// #define  BS_mask      GetMask

#ifdef OPT_HUFFMAN_GET_WMV

//int dbg_cnt2,dbg_cnt3;

//I32_WMV gethuffman_end(CInputBitStream *bs, void *decTable, I32_WMV &nextbits_shift,I32_WMV & currBits,I32_WMV & numBits,I32_WMV & numBits2, I32_WMV & bitsOver, I32_WMV & wBitsLeft, I32_WMV & result, I32_WMV & index);

#ifndef _EMB_ASM_ARM_STRMHUFF_
U32_WMV getHuffman(CInputBitStream_WMV *bs,TableInfo *tableInfo, HuffDecInfo *decTable)
{
  I32_WMV index, tableNum, numBits, numBits2, bitsOver;  
  register I32_WMV wBitsLeft,   minus1=-1;
  wBitsLeft=bs->m_BMS8.m_wBitsLeft;
  numBits = tableInfo[0].bits;

EndlessLoop:   
  // showBits

 
  bitsOver = wBitsLeft - numBits ;
  if (bitsOver < 0) 
  {
    

      bs->m_BMS8.m_wBitsLeft=wBitsLeft;
      BS_updateCurrNext(bs);
      wBitsLeft=bs->m_BMS8.m_wBitsLeft;

      bitsOver = wBitsLeft - numBits ;
      if(bitsOver<0)
      {
        index=bs->m_currBits<<(-bitsOver);
        
      }
      else
        index = (bs->m_currBits >> (bitsOver));
  }
  else 
  {
     
     index = (bs->m_currBits >> (bitsOver)); 
  }

  index&=(((U32_WMV)0xffffffff)>>(32-numBits));
  
  numBits2 = decTable[index].length;
  if (numBits2 != 0) 
  {
    // Flush Bits

    bs->m_BMS8.m_wBitsLeft= wBitsLeft-numBits2;

    return (I32_WMV)decTable[index].symbol;
  }



  wBitsLeft-= numBits;
  
  tableNum  = decTable[index].symbol;
  decTable  = tableInfo[tableNum].table;
  numBits = tableInfo[tableNum].bits;
  goto EndlessLoop;



}

#endif
#endif




