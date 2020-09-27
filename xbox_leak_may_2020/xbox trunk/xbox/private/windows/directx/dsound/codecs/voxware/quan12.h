#ifndef		QUAN12_H
#define     QUAN12_H

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
  const float   *Energy_TABLE_Voiced;
  unsigned short Energy_SIZE_Voiced;
  const float   *Energy_TABLE_UnVoiced;
  unsigned short Energy_SIZE_UnVoiced;

  unsigned short Energy_INDEX;
  
  /**** quantize multistage LSF VQ ****/
  const float    **Lsf_TABLE;
  unsigned short *Lsf_SIZE;
  unsigned short *Lsf_INDEX;

  const float    **Lsf_MSVQ_TABLE;

  /**** quantize amplitude VQ ****/
  short          Amp_NVec;
  const float    *Amp_TABLE;
  unsigned short Amp_INDEX;

  float          *Bark_pw_amp;

  void           *MSVQ_struct;

  const int      *levels;

  short          Class;
  short          PrevClass;
  float          *PrevLsf;
  float          *UVLsf;
  long           Seed;
  float          BckGrndEnergyEst;

  void           *pVemMem;
  VEM_KEY        vemKeyPostQuan;
  VEM_KEY        vemKeyPostUnQuan;

} QUAN_12;

unsigned short VoxQuanVR12(void *pvQuanMemBlk, void *pvParamMemBlk) ;

unsigned short VoxUnQuanVR12(void *pvQuanMemBlk, void *pvParamMemBlk);

unsigned short VoxInitQuanVR12(void **hQuan_mblk, void *pVemMem);

unsigned short VoxFreeQuanVR12(void **Quan_mblk);

short VoxQuanPitchVR12(float pitch);

float VoxLutPitchVR12(short qP0);
#endif  /*ifndef QUAN12_H*/

