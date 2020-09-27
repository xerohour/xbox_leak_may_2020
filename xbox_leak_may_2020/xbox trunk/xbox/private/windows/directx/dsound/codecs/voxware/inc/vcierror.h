/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vciError.h
*
* Purpose:     Defines the Voxware Core Interface (VCI) error values
*
* Author/Date: Ilan Berci, Sept. 25, 1996
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   P:/r_and_d/archives/common/vci/vciError.h_v   1.4   28 Aug 1998 10:36:10   johnp  $
******************************************************************************/

#ifndef _VCI_ERROR_H_
#define _VCI_ERROR_H_

#ifndef _VCI_RETCODE_
#define _VCI_RETCODE_
typedef unsigned short VCI_RETCODE; /* Voxware Core Interface error type.     */
                                    /* This variable is also defined in vci.h */
#endif  /** #ifndef _VCI_RETCODE_ **/

#define VCI_NO_ERROR                      0
#define VCI_FAILED_MEM_ALLOC           6000
#define VCI_NOT_VCI_HANDLE             6100
#define VCI_INCOMPATIBLE_HANDLE        6105
#define VCI_NOT_ENC_HANDLE             6110
#define VCI_NOT_DEC_HANDLE             6115
#define VCI_NULL_IO_BLOCK              6120
#define VCI_NULL_PCM_BUFFER            6125
#define VCI_NULL_VOX_BUFFER            6130
#define VCI_INVALID_R_BIT_OFFSET       6135
#define VCI_INVALID_W_BIT_OFFSET       6140
#define VCI_INVALID_R_BYTE_OFFSET      6145
#define VCI_INVALID_W_BYTE_OFFSET      6150
#define VCI_INSUFFICIENT_VOX_DATA      6155
#define VCI_NULL_INFO_BLOCK            6160
#define VCI_ARG_OUT_OF_RANGE           6165
#define VCI_NULL_ARG                   6170
#define VCI_NO_EXTENSION_SUPPORT       6175
#define VCI_WRONG_PITCH_MODE           6180
#define VCI_CODEC_NOT_SUPPORTED        6500 
#define VCI_CODEC_OUT_OF_DATE          6501 
#define VCI_EXT_OUT_OF_DATE            6502
#define VCI_EXTENSION_NOT_ENABLED      6503 
#define VCI_BITSTREAM_CORRUPTED        6185
#define VCI_BITSTREAM_NOT_SUPPORTED    6186
#define VCI_BITSTREAM_INSUFFICIENT     6187
#define VCI_BITSTREAM_FULL             6189
#define VCI_RATE_SET_TO_MAX            6190
#define VCI_RATE_INVALID               6191
#define VCI_DYNAMIC_ARG_NOT_FOUND      6200
#define VCI_PLUS_NOT_INITIALIZED       6210

#define VCI_IO_BLK_PARAM_CHECK(a) \
   if(!a) \
      return VCI_NULL_ARG; \
   if(!a->pVoxBuffer) \
      return VCI_NULL_VOX_BUFFER; \
   if(!(a->pPCMBuffer)) \
      return VCI_NULL_PCM_BUFFER; \
   if(a->wVoxBufferReadByteOffset>=a->wVoxBufferSize) \
      return VCI_INVALID_R_BYTE_OFFSET; \
   if(a->wVoxBufferWriteByteOffset>=a->wVoxBufferSize) \
      return VCI_INVALID_W_BYTE_OFFSET; \
   if(a->chVoxBufferReadBitOffset>7) \
      return VCI_INVALID_R_BIT_OFFSET; \
   if(a->chVoxBufferWriteBitOffset>7) \
      return VCI_INVALID_W_BIT_OFFSET; 

#define VCI_EXTRACT_IO_BLK_PARAM_CHECK(a) \
   if(!a) \
      return VCI_NULL_ARG; \
   if(!a->pVoxBuffer) \
      return VCI_NULL_VOX_BUFFER; \
   if(a->wVoxBufferReadByteOffset>=a->wVoxBufferSize) \
      return VCI_INVALID_R_BYTE_OFFSET; \
   if(a->wVoxBufferWriteByteOffset>=a->wVoxBufferSize) \
      return VCI_INVALID_W_BYTE_OFFSET; \
   if(a->chVoxBufferReadBitOffset>7) \
      return VCI_INVALID_R_BIT_OFFSET; \
   if(a->chVoxBufferWriteBitOffset>7) \
      return VCI_INVALID_W_BIT_OFFSET; 

#define VCI_IO_BLK_PARAM_CHECK_ENC(a) \
   if(!a) \
      return VCI_NULL_ARG; \
   if(!a->pPCMBuffer) \
      return VCI_NULL_PCM_BUFFER; \
   if(!a->pVoxBuffer) \
      return VCI_NULL_VOX_BUFFER; \
   if(a->wVoxBufferWriteByteOffset>=a->wVoxBufferSize) \
      return VCI_INVALID_W_BYTE_OFFSET; \
   if(a->chVoxBufferWriteBitOffset>7) \
      return VCI_INVALID_W_BIT_OFFSET; 

#define VCI_IO_BLK_PARAM_CHECK_DEC(a) \
   if(!a) \
      return VCI_NULL_ARG; \
   if(!a->pPCMBuffer) \
      return VCI_NULL_PCM_BUFFER; \
   if(!a->pVoxBuffer) \
      return VCI_NULL_VOX_BUFFER; \
   if(a->wVoxBufferReadByteOffset>=a->wVoxBufferSize) \
      return VCI_INVALID_R_BYTE_OFFSET; \
   if(a->chVoxBufferReadBitOffset>7) \
      return VCI_INVALID_R_BIT_OFFSET;

#define VCI_SIL_IO_BLK_PARAM_CHECK_DEC(a) \
   if(!a) \
      return VCI_NULL_ARG; \
   if(!a->pPCMBuffer) \
      return VCI_NULL_PCM_BUFFER;

#endif /** #ifndef _VCI_ERROR_H_ **/

