#ifndef _SYNSPEC_H_
#define _SYNSPEC_H_

#define BIT_SHIFT         22
#define DOWN_SHIFT        6
#define REFINE_SHIFT      (BIT_SHIFT-DOWN_SHIFT)
#define FIX_SCALE         ((long)((long)1<<BIT_SHIFT))
#define FIX_05            ((long)((long)1<<(BIT_SHIFT-1)))
#define FIX_05_REFINE     ((long)((long)1<<(REFINE_SHIFT-1)))


#define REFINE_SPEC_SCALE   32                

#define SPEC_RESAMP_BASE   ((float)REFINE_SPEC_SCALE/(float)(FRAME_SIZE1-1))

#define MIN_REFINE_PITCH     8


#define NREFINEP01        5       /* number of pitch candidates for pitch refinement */
#define NREFINEP02        4       /* number of pitch candidates for pitch refinement */

#define SPEC_START        3       /* 50 Hz */
#define SPEC_END          64 	  /* was 236 (3.7kHz)  however now 64 (1kHz) for Low complexity*/

#define MAXCON            3.402823466E+38F

#define MINCON         1.0e-6F    /* minimum constant */

#define INV_NFFT          (1.0F/(float)NFFT)

#define MAX_BANDWDTH      20

#define SEEVOC_PEAK_ON

float CalcCmpxSpecErr(float fPitch, float *pSWR, float *pSWI, 
		      int uiWinLen, int iStartDFT, int iEndDFT);

int CalcBandErr(float fPitch, float *pSWR, float *pSWI,
		float *pPower, int uiWinLen, float *pBandErr);

int VoxSynSpecSeeVoc(float *pSWR, float *pSWI, float *pPower, int *iSeeVocFreq,
		     float pitch, float *pBandErr, int uiWinLen);

#endif
