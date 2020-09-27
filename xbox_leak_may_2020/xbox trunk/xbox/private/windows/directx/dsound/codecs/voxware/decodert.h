#ifndef DECODERT_H
#define DECODERT_H

#ifndef __VEM_KEY
#define __VEM_KEY
typedef void * VEM_KEY;
#endif /* __VEM_KEY */

/*---------------------------------------------------------------------
  Decoder Structure ...
  ---------------------------------------------------------------------*/
typedef struct tagDecode_mblk
{
  /* structure for VCI */
   void *    pFadeIOMemBlk;

  /* private structure */
   void*     Synthesis_mblk;
   void*     UVexcite_mblk;
   void*     Vexcite_mblk;
   void*     OutputBuffer_mblk;
   void*     SubFrameParam_mblk;
   void*     PrevFrameParam_mblk;
   void*     Param_mblk;
   void*     Sws_mblk;
   void*     myFHT_mblk;

   void*     Effects_mblk;
   
   float     fWarpedLengthFactor;
   short     drop;
   short     dropLeft;
   short     dropRate; 
   float     Warp;
   unsigned char writeFLAG;
   unsigned char unpackFLAG;

   void      *pVemMem;  /* attachment to Vem memory */
   VEM_KEY   vemKeyPreDecode;
   VEM_KEY   vemKeyPostDecode;
} DEC_MEM_BLK;

unsigned short VoxDecodeRT(STACK_R short *pPCMBuffer,void *pMemBlkD);

unsigned short VoxInitDecodeRT(void **ppMemBlkD, void *pVemMem);

unsigned short VoxFreeDecodeRT(void **hMemBlkD);

#endif /* DECODERT_H */

