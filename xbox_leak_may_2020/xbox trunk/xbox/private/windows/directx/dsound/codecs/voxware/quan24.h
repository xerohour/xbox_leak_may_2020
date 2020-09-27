#ifndef		QUAN24_H
#define     QUAN24_H

#ifndef __VEM_KEY
#define __VEM_KEY
typedef void * VEM_KEY;
#endif /* __VEM_KEY */

typedef struct
{
  /***** quantize Pv ******/
  const float    *Pv_TABLE;
  unsigned short Pv_SIZE;
  unsigned short Pv_INDEX;

  /***** quantize pitch ******/
  float          *Pitch_TABLE;
  float          QuanPitch;
  unsigned short Pitch_SIZE;
  unsigned short Pitch_INDEX;

  /***** quantize energy ******/
  const float   *Energy_TABLE;
  unsigned short Energy_SIZE;
  unsigned short Energy_INDEX;
  
  /**** quantize multistage LSF VQ ****/
  const float    **Lsf_TABLE;
  unsigned short *Lsf_SIZE;
  unsigned short *Lsf_INDEX;
  const int      *levels;

  void           *pVemMem;
  VEM_KEY        vemKeyPostQuan;
  VEM_KEY        vemKeyPostUnQuan;

} QUAN_24;

unsigned short VoxQuan24(STACK_R void *pvQuanMemBlk, void *pvParamMemBlk) ;

unsigned short VoxUnQuan24(void *pvQuanMemBlk, void *pvParamMemBlk);

unsigned short VoxInitQuan24(void **hQuan_mblk, void *pVemMem);

unsigned short VoxFreeQuan24(void **Quan_mblk);

short VoxQuanPitch(float pitch);

float VoxLutPitch(short qP0);
#endif  /*ifndef QUAN24_H*/

