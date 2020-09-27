#ifndef		QUAN28_H
#define     QUAN28_H

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

  /**** quantize amplitude VQ ****/
  short          Amp_NVec;
  const float    *Amp_TABLE;
  unsigned short Amp_INDEX;

  float          *Bark_pw_amp;

  const int      *levels;

  void           *pVemMem;
  VEM_KEY        vemKeyPostQuan;
  VEM_KEY        vemKeyPostUnQuan;

} QUAN_28;

unsigned short VoxQuanRT28(void *pvQuanMemBlk, void *pvParamMemBlk) ;

unsigned short VoxQuanAmpRT28(STACK_R void *pvQuanMemBlk, void *pvParamMemBlk);

unsigned short VoxUnQuanRT28(STACK_R void *pvQuanMemBlk, void *pvParamMemBlk);

unsigned short VoxInitQuanRT28(void **hQuan_mblk, void *pVemMem);

unsigned short VoxFreeQuanRT28(void **Quan_mblk);

short VoxQuanPitchRT28(float pitch);

float VoxLutPitchRT28(short qP0);
#endif  /*ifndef QUAN29_H*/

