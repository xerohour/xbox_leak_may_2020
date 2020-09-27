/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:      PackSC.c
*
* Purpose:       Bit packing for SC36 codecs
*
* Functions:     VoxPackSC3, VoxPackSC6, VoxUnPackSC3, VoxUnPackSC6
*
* Author/Date:   Bob Dunn (scarfed code from Zopf/Wang)
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
*    WARNING: This code is currently hard wired and will not work with 
*               various multiple quan_files!
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pack/PackSC.c_v   1.7   13 Apr 1998 16:17:36   weiwang  $
*
******************************************************************************/
#include <string.h>
#include <assert.h>

#include "vLibPack.h"
#include "VoxMem.h"
#include "codec.h"
#include "quansc36.h"

#include "CodeStr.h"

#include "PackSC.h"

#include "xvocver.h"


const static short BitTable_SC3[TOTAL_SC3_QUAN_INDICES] = {
BITS_PITCH_SC3, BITS_GAIN_SC3, BITS_PV_SC3,
STAGE1_BITS_ASKS_0_1_SC36, STAGE2_BITS_ASKS_0_1_SC36,
STAGE1_BITS_ASKS_2_3_SC36, STAGE2_BITS_ASKS_2_3_SC36,
STAGE1_BITS_ASKS_4_5_SC36, STAGE2_BITS_ASKS_4_5_SC36,
STAGE1_BITS_ASKS_6_7_SC36, STAGE2_BITS_ASKS_6_7_SC36,
STAGE1_BITS_ASKS_8_9_SC36, STAGE2_BITS_ASKS_8_9_SC36,
STAGE1_BITS_ASKS_10_11_SC36, STAGE2_BITS_ASKS_10_11_SC36,
RESERVED_BITS_SC3, VCI_FLAG_BITS_SC3
};

const static short BitTable_SC6[EXTRA_SC6_QUAN_INDICES] = {
RESIDUAL_BITS_ASKS_8_9_SC6,
RESIDUAL_BITS_ASKS_10_11_SC6,
BITS_PHASE_PAIR_SC6, BITS_PHASE_PAIR_SC6, 
BITS_PHASE_PAIR_SC6, BITS_PHASE_PAIR_SC6, 
BITS_PHASE_PAIR_SC6, BITS_PHASE_PAIR_SC6, 
BITS_PHASE_PAIR_SC6, BITS_PHASE_PAIR_SC6,
RESERVED_BITS_SC6, VCI_FLAG_BITS_SC6
};

#ifndef DECODE_ONLY
/******************************************************************************
*
* Function:  VoxPackSC3
*
* Action:    Pack quantization indices for SC3
*
* Input:     piQuanIndex -- quantization indices
*            pchPacketBuffer -- start pointer to the packet buffer
*            psByteOffset -- the pointer to the write byte offset
*            pchBitOffset -- the pointer to the write bit offset
*            sBufferSize -- packet buffer size
*            sLongerFrameFlag -- continuous flag
*
* Output:    pchPacketBuffer -- bit packed buffer
*            psByteOffset -- updated write byte offset
*            pchBitOffset -- updated write bit offset
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
******************************************************************************/

void VoxPackSC3(int *piQuanIndex, unsigned char  *pchPacketBuffer,
                unsigned short *psByteOffset, unsigned char  *pchBitOffset,
                unsigned short  sBufferSize, unsigned short  sLongerFrameFlag)
{
  C_BUFFER_TYPE  CBuffer; 
  int i;

  /*-------------------------------------------------------------------
    Error checking 
  --------------------------------------------------------------------*/
  assert(piQuanIndex != NULL && pchPacketBuffer != NULL);
  assert(psByteOffset != NULL && pchBitOffset != NULL);


  CBuffer.wBufferSize = sBufferSize;
  CBuffer.pBufferWriteByteOffset = psByteOffset;
  CBuffer.pBufferWriteBitOffset  = pchBitOffset;
  CBuffer.pBuffer = pchPacketBuffer;                                       

  /*---------------------------------------------------
    Set for reserved bit
    --------------------------------------------------*/
   piQuanIndex[INDEX_RESERVE_BIT_SC3] = 0;

  /*---------------------------------------------------
    Set for longer frame flag
    --------------------------------------------------*/
   piQuanIndex[INDEX_FLAG_BIT_SC3] = sLongerFrameFlag;

  /*--------------------------------------------------------------------
    Pack All
  --------------------------------------------------------------------*/
  for (i = 0; i < TOTAL_SC3_QUAN_INDICES; i++) 
    BitPackCBuffer(&CBuffer, (unsigned char)piQuanIndex[i], BitTable_SC3[i]);
}


/******************************************************************************
*
* Function:  VoxPackSC6
*
* Action:    Pack quantization indices for SC6
*
* Input:     piQuanIndex -- quantization indices
*            pchPacketBuffer -- start pointer to the packet buffer
*            psByteOffset -- the pointer to the write byte offset
*            pchBitOffset -- the pointer to the write bit offset
*            sBufferSize -- packet buffer size
*            sLongerFrameFlag -- continuous flag
*
* Output:    pchPacketBuffer -- bit packed buffer
*            psByteOffset -- updated write byte offset
*            pchBitOffset -- updated write bit offset
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
******************************************************************************/

void VoxPackSC6(int *piQuanIndex, unsigned char  *pchPacketBuffer,
                unsigned short *psByteOffset, unsigned char  *pchBitOffset,
                unsigned short  sBufferSize, unsigned short  sLongerFrameFlag)
{
  C_BUFFER_TYPE  CBuffer; 
  int i;

  /*-------------------------------------------------------------------
    Error checking 
  --------------------------------------------------------------------*/
  assert(piQuanIndex != NULL && pchPacketBuffer != NULL);
  assert(psByteOffset != NULL && pchBitOffset != NULL);

  CBuffer.wBufferSize = sBufferSize;
  CBuffer.pBufferWriteByteOffset = psByteOffset;
  CBuffer.pBufferWriteBitOffset  = pchBitOffset;
  CBuffer.pBuffer = pchPacketBuffer;                                       


  /*---------------------------------------------------
    Set for reserved bit
    --------------------------------------------------*/
   piQuanIndex[INDEX_RESERVE_BIT_SC6] = 0;

  /*---------------------------------------------------
    Set for longer frame flag
    --------------------------------------------------*/
   piQuanIndex[INDEX_FLAG_BIT_SC6] = sLongerFrameFlag;

  /*--------------------------------------------------------------------
    Pack Phase VQ indices 
  --------------------------------------------------------------------*/
  for (i = 0; i < EXTRA_SC6_QUAN_INDICES; i++)
    BitPackCBuffer(&CBuffer, (unsigned char)piQuanIndex[i], BitTable_SC6[i]);

}
#endif

/******************************************************************************
*
* Function:  VoxUnPackSC3
*
* Action:    Unpack quantization indices for SC3
*
* Input:     piQuanIndex -- pointer to the quantization indices
*            pchPacketBuffer -- packet buffer
*            psByteOffset -- the pointer to the read byte offset
*            pchBitOffset -- the pointer to the read bit offset
*            sBufferSize -- packet buffer size
*            psLongerFrameFlag -- pointer to the continuous flag
*
* Output:    piQuanIndex -- unpacked quantization indices
*            psByteOffset -- updated read byte offset
*            pchBitOffset -- updated read bit offset
*            psLongerFrameFlag -- continuous flag
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
******************************************************************************/

void VoxUnPackSC3(int *piQuanIndex, unsigned char *pchPacketBuffer,
                  unsigned short *psByteOffset, unsigned char *pchBitOffset,
                  unsigned short sBufferSize, unsigned short *psLongerFrameFlag)
{
  unsigned char pC;
  int i;

  C_BUFFER_TYPE CBuffer;

  /*-------------------------------------------------------------------
    Error checking 
  --------------------------------------------------------------------*/
  assert(piQuanIndex != NULL && pchPacketBuffer != NULL);
  assert(psByteOffset != NULL && pchBitOffset != NULL);
  assert(psLongerFrameFlag != NULL);
  
  CBuffer.wBufferSize = sBufferSize;
  CBuffer.pBufferReadByteOffset = psByteOffset;
  CBuffer.pBufferReadBitOffset  = pchBitOffset;
  CBuffer.pBuffer = pchPacketBuffer;                                       

  /*--------------------------------------------------------------------
    unpack all
  --------------------------------------------------------------------*/
  for (i = 0; i < TOTAL_SC3_QUAN_INDICES; i++) {
    BitUnpackCBuffer(&CBuffer, &pC, BitTable_SC3[i]);
    piQuanIndex[i] = (int)pC;
  }


  /*---------------------------------------------------
    Set for longer frame flag
    --------------------------------------------------*/
   *psLongerFrameFlag = (unsigned short)piQuanIndex[INDEX_FLAG_BIT_SC3];
}

/******************************************************************************
*
* Function:  VoxUnPackSC6
*
* Action:    Unpack quantization indices for SC6
*
* Input:     piQuanIndex -- pointer to the quantization indices
*            pchPacketBuffer -- packet buffer
*            psByteOffset -- the pointer to the read byte offset
*            pchBitOffset -- the pointer to the read bit offset
*            sBufferSize -- packet buffer size
*            psLongerFrameFlag -- pointer to the continuous flag
*
* Output:    piQuanIndex -- unpacked quantization indices
*            psByteOffset -- updated read byte offset
*            pchBitOffset -- updated read bit offset
*            psLongerFrameFlag -- continuous flag
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
******************************************************************************/

void VoxUnPackSC6(int *piQuanIndex, unsigned char *pchPacketBuffer,
                  unsigned short *psByteOffset, unsigned char *pchBitOffset,
                  unsigned short sBufferSize, unsigned short *psLongerFrameFlag)
{
  unsigned char pC;
  int i;
  C_BUFFER_TYPE CBuffer;

  /*-------------------------------------------------------------------
    Error checking 
  --------------------------------------------------------------------*/
  assert(piQuanIndex != NULL && pchPacketBuffer != NULL);
  assert(psByteOffset != NULL && pchBitOffset != NULL);
  assert(psLongerFrameFlag != NULL);

  CBuffer.wBufferSize = sBufferSize;
  CBuffer.pBufferReadByteOffset = psByteOffset;
  CBuffer.pBufferReadBitOffset  = pchBitOffset;
  CBuffer.pBuffer = pchPacketBuffer;                                       

  /*--------------------------------------------------------------------
    Unpack all
  --------------------------------------------------------------------*/
  for (i = 0; i < EXTRA_SC6_QUAN_INDICES; i++)
    {
      BitUnpackCBuffer(&CBuffer, &pC, BitTable_SC6[i]);
      piQuanIndex[i] = (int)pC;
    }

  *psLongerFrameFlag = (unsigned short)piQuanIndex[INDEX_FLAG_BIT_SC6];
}
