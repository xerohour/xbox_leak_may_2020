//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       huffman.hpp
//
//--------------------------------------------------------------------------

/*  -*-c++-*-
 *  Sanjeev Mehrotra
 *  6/19/1998
 *  Chuang Gu
 *  2/1/2000
 */
#ifndef _HUFFMAN_H
#define _HUFFMAN_H
#include "typedef.hpp"
//extern U32_WMV getMask [33];
#define BITSTREAM_READ 1
//class CInputBitStream_WMV;
typedef enum tagVResultCode
{
  vrNoError        = 0,
  vrFail           = 1,
  vrNotFound       = 2,
  vrExists         = 3,
  vrEof            = 4,
  vrOutOfMemory    = 5,
  vrOutOfResource  = 6,
  vrOutOfBounds    = 7,
  vrBadParam       = 8,
  vrBadInput       = 9,
  vrIOError        = 10,
  vrInterrupted    = 11,
  vrNotSupported   = 12,
  vrNotImplemented = 13,
  vrDropped        = 14,
  vr_ENDOFALLERRORS
} VResultCode;

class VResult
{
private:
  VResultCode m_vrCode;

public:
  VResult() : m_vrCode(vrFail) {}
  VResult(VResultCode vrCode) : m_vrCode(vrCode) {}
  operator VResultCode() { return m_vrCode; }
#if !defined(_Embedded_x86) || defined(_XBOX)
  Bool_WMV failed() { return m_vrCode != vrNoError; }
#else
  Bool_WMV failed(void);   
#endif
  char* explanation();
};

#define MAX_STAGES 3

#if !defined(UNDER_CE) && !defined(MIMIC_CE_ON_DESKTOP)
    // define stages assuming large memory and cache
#   define BITS_STAGE1 10
#   define BITS_STAGE2 11
#else
    // define stages assuming small memory and cache
#   define BITS_STAGE1 6
#   define BITS_STAGE2 15
#endif

// Mode            Entry Size   Max # Symbols   Max codeword length
// ----------------------------------------------------------------
// LARGE_HUFFMAN    8 bytes         Large             32 bits
// MED_HUFFMAN      4 bytes         65536             32 bits
// SMALL_HUFFMAN    2 bytes          4096             32 bits

//#define LARGE_HUFFMAN
#ifndef LARGE_HUFFMAN
#define SMALL_HUFFMAN
#endif

#if defined(OPT_HUFFMAN_GET_WMV) 
  typedef struct _HuffDecInfo {

#   if (defined(UNDER_CE) && 1) || (defined(MIMIC_CE_ON_DESKTOP) && 1)
        // The largest symbol is 1099 because we put indexes, not the real symbol in these tables
        // By using three stages, we 
#       define HUFFDEC_SYMBOL_BITS 12
#       define HUFFDEC_LENGTH_BITS 4
        U16_WMV symbol : HUFFDEC_SYMBOL_BITS;
        U16_WMV length : HUFFDEC_LENGTH_BITS;
#   else
        // The following comment does not seem to be true any longer
            //MIPS implementation rely on the specific offsets of these structs. so DON'T CHANGE THEM. 
            // - weidongz Nov.3, 2000
#       define HUFFDEC_SYMBOL_BITS 31
#       define HUFFDEC_LENGTH_BITS 31
        U32_WMV symbol;
        U32_WMV length;
#   endif
  } HuffDecInfo;

  typedef struct _TableInfo {
    I32_WMV bits;
    HuffDecInfo *table;
  } TableInfo;

#else
  typedef struct _HuffDecInfo {
#     define HUFFDEC_SYMBOL_BITS 12
#     define HUFFDEC_LENGTH_BITS 4
      U16_WMV symbol : 12;
      U16_WMV length : 4;
  } HuffDecInfo;

  typedef struct _TableInfo {
    I32_WMV bits;
    HuffDecInfo *table;
  } TableInfo;

#endif

typedef struct _TableInitInfo {
  I32_WMV prefix;     // the prefix that got you there
  I32_WMV start, end; // start, end tableNums of children tables
  I32_WMV bits;       // the bits for this table
  I32_WMV maxBits;    // the maximum # of bits of things entering this table
} TableInitInfo;



//class CVideoObjectDecoder ;
//class CDCTTableInfo_Dec;
typedef struct _Huffman_WMV 
{


//  static U32_WMV m_mask[33]; // 32 bit dependency

  // It is critical that encTable be 32 bits and decTable be 16 bits for 
  // the given huffman routines to work
    U32_WMV m_uiUserData;
    I32_WMV m_alphabetSize;
    I32_WMV m_maxCodeLength;
    I32_WMV m_numDecEntries, m_allocDecEntries;
    I32_WMV m_allocTables;
    tPackHuffmanCode_WMV *m_encInfo; 
  //HuffEncInfo   *m_encInfo;

#if defined( OPT_HUFFMAN_GET_WMV)
  // it is critical for the mips implementation to work that the order and 
  // the types of these members are exactly as shown here.
  // -weidongz Nov.9.2000
  union
  {
    HuffDecInfo   *m_decInfo;
	int OPT_HUFFMAN_GET_Hbase;
  };
  TableInfo     *m_tableInfo;
#else
  HuffDecInfo   *m_decInfo;
  TableInfo     *m_tableInfo;
#endif

  TableInitInfo *m_initInfo;

}Huffman_WMV;

//-----------------------------------------------------------------------------
// Inline definitions
/*
inline I32_WMV Huffman::length(I32_WMV inpSym) {
#ifndef PROFILE
  Assert(inpSym >= 0 && inpSym < m_alphabetSize);
#endif
  return (I32_WMV)m_encInfo[inpSym].length;
}
*/
#endif

