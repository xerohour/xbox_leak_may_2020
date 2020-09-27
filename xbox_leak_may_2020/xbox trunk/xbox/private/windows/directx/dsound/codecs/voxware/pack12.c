/*
 *  File:       PackRT29.c
 *
 *  Project:    RT29 codec.
 *          
 *  By:         Rob Zopf  09/12/95
 *
 *  Contains:   do quantization of all the parameters for codec for compression ID 51
 * 
 *  OS:         ANSI
 *
 *  Machine:    SPARC 5
 *      
 *		Voxware Proprietary Material
 *		Copyright 1996, Voxware, Inc.
 *		All Rights Resrved
 *
 *		DISTRIBUTION PROHIBITED without
 *		written authorization from Voxware
 *
 *  Modified and Rewritten by Wei Wang,  2/13/96
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/pack/Pack12.c_v   1.1   16 Feb 1998 15:47:50   bobd  $
 */ 
#include <string.h>
#include <assert.h>

#include "VoxMem.h"
#include "vLibPack.h" /* BitPackCBuffer(), BitUnpackCBuffer() */
#include "quanvr12.h"

#include "Quan12.h"

#include "Pack12.h"

#include "xvocver.h"

unsigned short VoxPackVR12(void *pvQuan_mblk, unsigned char *PacketBuffer,
                   unsigned short *ByteOffset,  unsigned char *BitOffset,
                   unsigned short BufferSize)
{
  QUAN_12 *Quan_mblk = (QUAN_12 *)pvQuan_mblk;
  int i;

  C_BUFFER_TYPE CBuffer; 

  CBuffer.wBufferSize = BufferSize;
  CBuffer.pBufferWriteByteOffset = ByteOffset;
  CBuffer.pBufferWriteBitOffset  = BitOffset;
  CBuffer.pBuffer = PacketBuffer;                                       


  BitPackCBuffer(&CBuffer, (unsigned char)Quan_mblk->Class, CLASS_BITS_VR12);

  if (Quan_mblk->Class==UV_FRAME)
  {
     for (i=0; i<NUMSTAGES_LSPVQ_VR12_UNVOICED; i++)
        BitPackCBuffer(&CBuffer, (unsigned char)Quan_mblk->Lsf_INDEX[i], TABLEBITS_LSPVQ_VR12);

     BitPackCBuffer(&CBuffer, (unsigned char)Quan_mblk->Energy_INDEX, GAIN_BITS_VR12_UNVOICED);
  }
  if (Quan_mblk->Class==MIX_FRAME)
  {
     BitPackCBuffer(&CBuffer, (unsigned char)Quan_mblk->Pv_INDEX, PV_BITS_VR12_MIXED);
     BitPackCBuffer(&CBuffer, (unsigned char)Quan_mblk->Pitch_INDEX, F0_BITS_VR12_MIXED);
     for (i=0; i<NUMSTAGES_LSPVQ_VR12_MIXED; i++)
        BitPackCBuffer(&CBuffer, (unsigned char)Quan_mblk->Lsf_INDEX[i], TABLEBITS_LSPVQ_VR12);

     BitPackCBuffer(&CBuffer, (unsigned char)Quan_mblk->Energy_INDEX, GAIN_BITS_VR12_MIXED);
  }

  if (Quan_mblk->Class==V_FRAME)
  {
     BitPackCBuffer(&CBuffer, (unsigned char)Quan_mblk->Pitch_INDEX, F0_BITS_VR12_VOICED);
     for (i=0; i<NUMSTAGES_LSPVQ_VR12_VOICED; i++)
        BitPackCBuffer(&CBuffer, (unsigned char)Quan_mblk->Lsf_INDEX[i], TABLEBITS_LSPVQ_VR12);

     BitPackCBuffer(&CBuffer, (unsigned char)Quan_mblk->Energy_INDEX, GAIN_BITS_VR12_VOICED);
  }



  return 0;
}

unsigned short VoxUnPackVR12(void *pvQuan_mblk, unsigned char *PacketBuffer,
				unsigned short *ByteOffset,  unsigned char *BitOffset,
            unsigned short BufferSize)
{
  QUAN_12 *Quan_mblk = (QUAN_12 *)pvQuan_mblk;
  unsigned char pC;
  int i;

  C_BUFFER_TYPE CBuffer;
  
  CBuffer.wBufferSize = BufferSize;
  CBuffer.pBufferReadByteOffset = ByteOffset;
  CBuffer.pBufferReadBitOffset  = BitOffset;
  CBuffer.pBuffer = PacketBuffer;                                       


  BitUnpackCBuffer(&CBuffer, &pC, CLASS_BITS_VR12);
  Quan_mblk->Class = (short)pC;
  if (Quan_mblk->Class==UV_FRAME)
  {
     for (i=0; i<NUMSTAGES_LSPVQ_VR12_UNVOICED;i++)
     {
        BitUnpackCBuffer(&CBuffer, &pC, TABLEBITS_LSPVQ_VR12);
        Quan_mblk->Lsf_INDEX[i] = (short)pC;
     }
     BitUnpackCBuffer(&CBuffer, &pC, GAIN_BITS_VR12_UNVOICED);
     Quan_mblk->Energy_INDEX = (short)pC;
  }
  if (Quan_mblk->Class==MIX_FRAME)
  {
     BitUnpackCBuffer(&CBuffer, &pC, PV_BITS_VR12_MIXED);
     Quan_mblk->Pv_INDEX = (short)pC;
     BitUnpackCBuffer(&CBuffer, &pC, F0_BITS_VR12_MIXED);
     Quan_mblk->Pitch_INDEX = (short)pC;
     for (i=0; i<NUMSTAGES_LSPVQ_VR12_MIXED;i++)
     {
        BitUnpackCBuffer(&CBuffer, &pC, TABLEBITS_LSPVQ_VR12);
        Quan_mblk->Lsf_INDEX[i] = (short)pC;
     }
     BitUnpackCBuffer(&CBuffer, &pC, GAIN_BITS_VR12_MIXED);
     Quan_mblk->Energy_INDEX = (short)pC;
  }

  if (Quan_mblk->Class==V_FRAME)
  {
     BitUnpackCBuffer(&CBuffer, &pC, F0_BITS_VR12_VOICED);
     Quan_mblk->Pitch_INDEX = (short)pC;
     for (i=0; i<NUMSTAGES_LSPVQ_VR12_VOICED;i++)
     {
        BitUnpackCBuffer(&CBuffer, &pC, TABLEBITS_LSPVQ_VR12);
        Quan_mblk->Lsf_INDEX[i] = (short)pC;
     }
     BitUnpackCBuffer(&CBuffer, &pC, GAIN_BITS_VR12_VOICED);
     Quan_mblk->Energy_INDEX = (short)pC;
  }


  return 0;
}
