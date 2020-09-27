/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       SynSpec.c
*
* Purpose:        Generate synthetic spectrum using SEEVOC peaks.
*                                                                   
* Functions: 
*
* Author/Date:
********************************************************************************
* Modifications:
*
* Comments:                                                                    
*
* Concerns:
*
* $Header:   G:/r_and_d/archives/realtime/encode/SynSpec.c_v   1.1   11 Mar 1998 13:33:52   weiwang  $
*******************************************************************************/
#include <assert.h>
#include <math.h>
#include "VoxMem.h"
#include "model.h"
#include "vLibMath.h"
#include "GetnHarm.h"
#include "SynSpec.h"

#include "xvocver.h"


/* spectrum window for pitch refinement -- center part (1024) of 16k point FFT of
   kaiser(221, 6) */
const float pWinR221[] =
{
#include "w576RT29.h"
};

const int iSpecTableLen = sizeof(pWinR221)/sizeof(float);

#define FABS(a)     (((float)a>0.0F)?(float)a:(float)-a)



/*
 * Function: calculate the error between the original spectrum and
 *           the synthetic spectrum.
 */
float CalcCmpxSpecErr(float fPitch, float *pSWR, float *pSWI, 
		      int uiWinLen, int iStartDFT, int iEndDFT)
{
  float F0, fBandWdth;
  int iHarmStart, iHarmEnd;
  float fTotalErr;
  int iLeftBound, iRightBound;
  float fAmpReal, fAmpImag;
  float fWin;
  int WinIndex;
  int i;
  int iHarm;
  float invF0;
  int iHarmF0;
  float *pWinSpecTab = (float *)pWinR221;
  long Fix_F0, Fix_fBandWdth;
  long Fix_fHarmF0;
  register float ftmp1, ftmp2;
  float ftmp;
  int iResampFactor;


  /*************************************************************
    calculate the resampling factor for synthesis spectrum 
	 ************************************************************/
  ftmp = SPEC_RESAMP_BASE*(float)(uiWinLen-1);
  VoxROUND2pos(ftmp, iResampFactor);

  /* adjust the resample factor */
  if (fPitch < (MIN_REFINE_PITCH+1))
    iResampFactor --;

  /* check the pitch range and reset ResampFactor */
  if (((float)iResampFactor * (float)NFFT_2) > (fPitch * (float)iSpecTableLen)) 
    iResampFactor = REFINE_SPEC_SCALE;


  /*************************************************************
    fundamental frequency in DFT domain 
    ************************************************************/
  F0 = (float)NFFT/fPitch;
  fBandWdth = F0 * 0.5F;

  ftmp = FIX_SCALE*F0;
  VoxROUND2posLong(ftmp, Fix_F0);
  ftmp = FIX_SCALE*fBandWdth;
  VoxROUND2posLong(ftmp, Fix_fBandWdth);

  /*************************************************************
    calculate the index of first harmonic and last harmonic 
    ************************************************************/
  assert(iStartDFT >= 0);
  assert(iEndDFT < NFFT_2);
  assert(iStartDFT < iEndDFT);
  invF0 = fPitch * INV_NFFT;
  ftmp = iStartDFT*invF0;
  VoxROUND2pos(ftmp, iHarmStart);

  ftmp = iEndDFT*invF0;
  VoxROUND2pos(ftmp, iHarmEnd);

  if (((iHarmEnd*Fix_F0+FIX_05)>>BIT_SHIFT) >= NFFT_2)
    iHarmEnd --;

  
  /*************************************************************
    Synthesis the spectrum and measure the errors between
    synthetic spectrum and original spectrum 
    ************************************************************/
  fTotalErr = MINCON;

  Fix_fHarmF0 = (long)(iHarmStart-1)*Fix_F0;
  iLeftBound = (int)((Fix_fHarmF0 + Fix_fBandWdth + FIX_05)>>BIT_SHIFT);


  if (iLeftBound < iStartDFT)
    iLeftBound = iStartDFT;


  for (iHarm = iHarmStart; iHarm <= iHarmEnd; iHarm++) {
    /* the center frequency of iHarm-th harmonic */
    Fix_fHarmF0 += Fix_F0;
    iRightBound = (int)((Fix_fHarmF0+Fix_fBandWdth+FIX_05)>>BIT_SHIFT);


    if (iRightBound > iEndDFT)
      iRightBound = iEndDFT;

    /* find the center amplitude */
    iHarmF0 = (int)((Fix_fHarmF0+FIX_05)>>BIT_SHIFT);

    assert(iHarmF0 < NFFT_2);

    fAmpReal = pSWR[iHarmF0];
    fAmpImag = pSWI[iHarmF0];

    /* synthesis the left part of one harmonic spectrum */
    WinIndex = -iResampFactor*iLeftBound;
    WinIndex+=(int)(((long)iResampFactor*((long)Fix_fHarmF0>>DOWN_SHIFT)
		     -(long)FIX_05_REFINE)>>REFINE_SHIFT);

    i = iLeftBound;
    while(WinIndex >= 0) {
      assert(WinIndex < iSpecTableLen && WinIndex >= 0);
      fWin = pWinSpecTab[WinIndex];
      ftmp1 = pSWR[i] - fAmpReal*fWin;
      ftmp2 = pSWI[i] - fAmpImag*fWin;
      fTotalErr += (float)FABS(ftmp1)+(float)FABS(ftmp2);
      i++;
      WinIndex -= iResampFactor;
    }

    /* synthesis the right part of one harmonic spectrum */
    WinIndex = -WinIndex;
    while (i < iRightBound) {
      assert(WinIndex < iSpecTableLen && WinIndex >= 0);
      fWin = pWinSpecTab[WinIndex];
      ftmp1 = pSWR[i] - fAmpReal*fWin;
      ftmp2 = pSWI[i] - fAmpImag*fWin;
      fTotalErr += (float)FABS(ftmp1)+(float)FABS(ftmp2);
      i++;
      WinIndex += iResampFactor;
    }

    /* save the rightBound as the leftBound for the next harmonic */
    iLeftBound = iRightBound;
  }

  return fTotalErr;
}


/*
 * Function: calculate the error between the original spectrum and
 *           the synthetic spectrum.
 */

int CalcBandErr(float fPitch, float *pSWR, float *pSWI,
		float *pPower, int uiWinLen, float *pBandErr)
{
  float F0, fBandWdth;
  int NHarm;
  int iLeftBound, iRightBound;
  float fAmpReal, fAmpImag;
  float fTotalErr, fTotalEng;
  int WinIndex;
  float fWin;
  int i;
  float fTmpSW, fTmpErr;
  int iHarm;
  int iHarmF0;
  float *pWinSpecTab = (float *)pWinR221;
  long Fix_F0, Fix_fBandWdth;
  long Fix_fHarmF0;
  float ftmp;
  int iResampFactor;


  /*************************************************************
    calculate the resampling factor for synthesis spectrum 
	 ************************************************************/
  ftmp = SPEC_RESAMP_BASE*(float)(uiWinLen-1);
  VoxROUND2pos(ftmp, iResampFactor);

  /* adjust the resample factor */
  if (fPitch < (MIN_REFINE_PITCH+1))
    iResampFactor --;

  /* check the pitch range and reset ResampFactor */
  if (((float)iResampFactor * (float)NFFT_2) > (fPitch * (float)iSpecTableLen)) 
    iResampFactor = REFINE_SPEC_SCALE;


  /*************************************************************
    fundamental frequency in DFT domain 
    ************************************************************/
  F0 = (float)NFFT/fPitch;
  fBandWdth = F0 * 0.5F;

  ftmp = FIX_SCALE*F0;
  VoxROUND2posLong(ftmp, Fix_F0);

  ftmp = FIX_SCALE*fBandWdth;
  VoxROUND2posLong(ftmp, Fix_fBandWdth);


  /*************************************************************
    calculate number of harmonics.
    ************************************************************/
  NHarm = getnHarm(fPitch);  

  /* make sure that number of harmonics won't out of range */
  if (NHarm > MAXHARM)    NHarm = MAXHARM;
  
  /*************************************************************
    Synthesis the spectrum and measure the errors between
    synthetic spectrum and original spectrum 
    ************************************************************/
  Fix_fHarmF0 = 0;

  iLeftBound = (int)((Fix_fHarmF0+Fix_fBandWdth+FIX_05)>>BIT_SHIFT);

  for (iHarm = 0; iHarm < NHarm; iHarm++) {
    /* the center frequency of iHarm-th harmonic */
    Fix_fHarmF0 += Fix_F0;         
    iRightBound = (int)((Fix_fHarmF0+Fix_fBandWdth+FIX_05)>>BIT_SHIFT);

    if (iRightBound >= NFFT_2)
      iRightBound = NFFT_2-1;

    /* find the center amplitude */
    iHarmF0 = (int)((Fix_fHarmF0+FIX_05)>>BIT_SHIFT);
    assert(iHarmF0 < NFFT_2);

    fAmpReal = pSWR[iHarmF0];
    fAmpImag = pSWI[iHarmF0];

    fTotalErr = MINCON;
    fTotalEng = MINCON;

    /* synthesis the left part of one harmonic spectrum */
    WinIndex = -iResampFactor*iLeftBound;
    WinIndex+=(int)(((long)iResampFactor*((long)Fix_fHarmF0>>DOWN_SHIFT)
		     -(long)FIX_05_REFINE)>>REFINE_SHIFT);
    i = iLeftBound;
    while(WinIndex >= 0) {
      assert(WinIndex < iSpecTableLen && WinIndex >= 0); 
      fWin = pWinSpecTab[WinIndex];

      fTmpSW = pSWR[i];
      fTmpErr = fTmpSW - fAmpReal*fWin;

      fTotalErr += fTmpErr*fTmpErr;

      fTmpSW = pSWI[i];
      fTmpErr = fTmpSW - fAmpImag*fWin;
      fTotalErr += fTmpErr*fTmpErr;
      fTotalEng += pPower[i];

      i++;
      WinIndex -= iResampFactor;
    }

    /* synthesis the right part of one harmonic spectrum */
    WinIndex = -WinIndex;
    while (i < iRightBound) {
      assert(WinIndex < iSpecTableLen && WinIndex >= 0); 
      fWin = pWinSpecTab[WinIndex];

      fWin = pWinSpecTab[WinIndex];
      fTmpSW = pSWR[i];
      fTmpErr = fTmpSW - fAmpReal*fWin;

      fTotalErr += fTmpErr*fTmpErr;
 
      fTmpSW = pSWI[i];
      fTmpErr = fTmpSW - fAmpImag*fWin;
      fTotalErr += fTmpErr*fTmpErr;
      fTotalEng += pPower[i];


      i++;
      WinIndex += iResampFactor;
    }

    /* save the rightBound as the leftBound for the next harmonic */
    iLeftBound = iRightBound;

    *pBandErr++ = fTotalErr/fTotalEng;
  }

  return NHarm;
}

int VoxSynSpecSeeVoc(float *pSWR, float *pSWI, float *pPower, int *iSeeVocFreq,
		     float fPitch, float *pBandErr, int uiWinLen)
{
  float F0, fBandWdth;
  int NHarm;
  int iLeftBound, iRightBound;
  float fAmpReal, fAmpImag;
  float fTotalErr, fTotalEng;
  int WinIndex;
  float fWin;
  int i;
  float fTmpSW, fTmpErr;
  int iHarm;
  int iHarmF0;
  int ResampFactor;
  float fHarmF0;
  int Ind_SV;
  int lastSVPeak;
  float ftmp;

  /*************************************************************
    calculate the resampling factor for synthesis spectrum 
	 ************************************************************/
  ftmp =  SPEC_RESAMP_BASE*(float)(uiWinLen-1);
  VoxROUND2pos(ftmp, ResampFactor);

  /* adjust the resample factor */
  if (fPitch < (MIN_REFINE_PITCH+1))
    ResampFactor --;

  /* check the pitch range and reset ResampFactor */
  if (((float)ResampFactor * (float)NFFT_2) > (fPitch * (float)iSpecTableLen)) 
    ResampFactor = REFINE_SPEC_SCALE;

  /*************************************************************
    fundamental frequency in DFT domain 
    ************************************************************/
  F0 = (float)NFFT/fPitch;
  fBandWdth = F0 * 0.5F;

  if (fBandWdth > (float)MAX_BANDWDTH)
    fBandWdth = (float)MAX_BANDWDTH;


  /*************************************************************
    calculate number of harmonics.
    ************************************************************/
  NHarm = getnHarm(fPitch);  

  /* make sure that number of harmonics won't out of range */
  if (NHarm > MAXHARM)    NHarm = MAXHARM;
  
  /*************************************************************
    Synthesis the spectrum and measure the errors between
    synthetic spectrum and original spectrum 
    ************************************************************/
  fHarmF0 = 0.0F;

  Ind_SV = 0;
  lastSVPeak = iSeeVocFreq[0];
  for (iHarm = 0; iHarm < NHarm; iHarm++) {
    /* the center frequency of iHarm-th harmonic */
	 fHarmF0 += F0;
	 ftmp = fHarmF0-fBandWdth;
	 VoxROUND2pos(ftmp, iLeftBound);
	 ftmp = fHarmF0 + fBandWdth;
	 VoxROUND2pos(ftmp, iRightBound);

    if (iRightBound >= NFFT_2)
      iRightBound = NFFT_2-1;

#ifdef SEEVOC_PEAK_ON

    while (fHarmF0 > iSeeVocFreq[Ind_SV]) {
      lastSVPeak = iSeeVocFreq[Ind_SV];
      Ind_SV++;
    }

    /* find the shortest distance of seevoc peaks */
    if ((iSeeVocFreq[Ind_SV]+lastSVPeak) > (fHarmF0+fHarmF0))
      iHarmF0 = lastSVPeak;
    else
      iHarmF0 = iSeeVocFreq[Ind_SV];

#else
	 /* find the center amplitude */
	 VoxROUND2pos(fHarmF0, iHarmF0);
#endif
    assert(iHarmF0 < NFFT_2);

    fAmpReal = pSWR[iHarmF0];
    fAmpImag = pSWI[iHarmF0];

    fTotalErr = MINCON;
    fTotalEng = MINCON;

	 /* synthesis the left part of one harmonic spectrum */
	 ftmp = ResampFactor * (fHarmF0-iLeftBound);
	 VoxROUND2pos(ftmp, WinIndex);
	 i = iLeftBound;
    while(WinIndex >= 0) {
      assert(WinIndex < iSpecTableLen && WinIndex >= 0); 
      fWin = pWinR221[WinIndex];

      fTmpSW = pSWR[i];
      fTmpErr = fTmpSW - fAmpReal*fWin;

      fTotalErr += fTmpErr*fTmpErr;

      fTmpSW = pSWI[i];
      fTmpErr = fTmpSW - fAmpImag*fWin;
      fTotalErr += fTmpErr*fTmpErr;
      fTotalEng += pPower[i];

      i++;
      WinIndex -= ResampFactor;
    }

    /* synthesis the right part of one harmonic spectrum */
    WinIndex = -WinIndex;
    while (i < iRightBound) {
      assert(WinIndex < iSpecTableLen && WinIndex >= 0); 
      fWin = pWinR221[WinIndex];

      fWin = pWinR221[WinIndex];
      fTmpSW = pSWR[i];
      fTmpErr = fTmpSW - fAmpReal*fWin;

      fTotalErr += fTmpErr*fTmpErr;

      fTmpSW = pSWI[i];
      fTmpErr = fTmpSW - fAmpImag*fWin;
      fTotalErr += fTmpErr*fTmpErr;
      fTotalEng += pPower[i];

      i++;
      WinIndex += ResampFactor;
    }

    *pBandErr++ = fTotalErr/fTotalEng;
  }

  return NHarm;
}

