/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       VoicDeci.c                                                              
*                                                                              
* Purpose:        voice probability calculation                                                                     
*                                                                              
* Functions:      VOXErr VoxVocDecision(const int LL, const float Prr, 
*                                       void *Voicing_mblk, float *pPv);                                                             
*                                                                              
* Author/Date:                                                                 
********************************************************************************
* Modifications:Ilan Berci/ 10/96 Removed global dependencies
*                                 Modularized code segments (Encapsulation)
*                                 Removed scope changes
*                                 Changed error code procedure
*                                 Removed useless code segments
*                                 General optimization                                                                 
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/encode/VoicDeci.c_v   1.4   01 Dec 1998 12:07:14   zopf  $
*******************************************************************************/
#include <stdio.h>


#include <math.h>
#include <assert.h>
#include <stdlib.h>

#include "VoxMem.h"
#include "model.h"
#include "research.h"
#include "GetnHarm.h"

#include "VoicDeci.h"

#include "xvocver.h"

/** PDA structure **/
typedef struct
{
  /**** for pitch refinement and voicing *****/
  /***** static memories ******/
  short *prev_vuv;          /** previous voice/unvoice decision -- 60 (in voice decision) **/
  float Zmax;             /** maximum energy of long term (Zmax) **/
  void *myVAD_mblk;
  float P_v;               /** previous Pv **/
} Voicing;


#define MINCON         1.0e-6F    /* minimum constant */

/*******************************************************************************
* Function:      VOXErr VoxVocDecision(const int LL, const float Prr, 
*                                       void *Voicing_mblk, float *pPv);
*
* Action:         voice probability calculation
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:		  VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
unsigned short VoxVocDecision(const float fPitch, void *Voicing_mblk, float *pPv, 
                      float *pPower, float *pBandErr, unsigned int uiWinLen, short *Class)
{
  Voicing *pVoicing_mblk = (Voicing *)Voicing_mblk;
  float fNormScale;
  int i;
  float fLowBandEng, fHighBandEng, fTotalEng;
  float Pv;
  float fMaxEng = pVoicing_mblk->Zmax;
  float fBiasEng, fBiasL2H, fBiasP0, fBiasAll;
  float fThrdDelta;
  float fThreshold;
  short *iPrevVUV = pVoicing_mblk->prev_vuv;
  float fPrevPv = pVoicing_mblk->P_v;
  int iVHarm;
  unsigned short iClass=*Class;
  float Z02Zmax, Zlf2Zhf, M;
  int NHarm;

  /*************************************************************
    get number of harmonics 
    ************************************************************/
  NHarm = getnHarm(fPitch);

  /*************************************************************
    calculate energy 
    ************************************************************/
  fNormScale = 1.0F / ((float)NFFT_2 * (float)uiWinLen);

  fLowBandEng = MINCON;
  for (i = 0; i < NFFT_4; i++)  fLowBandEng += *pPower++;
  fLowBandEng *= fNormScale;

  fHighBandEng = MINCON;
  for (; i < NFFT_2; i++)   fHighBandEng += *pPower++;
  fHighBandEng *= fNormScale;

  fTotalEng = fLowBandEng + fHighBandEng;   /* leave for VAD */
  
  if (fTotalEng < MINCON) 
    Pv = 0.0F;
  else {
    /*************************************************************
      calculate the bias for voicing threshold 
      ************************************************************/
    /* Ratio related to maximum energy. For a low energy, voicing decision
       tends to unvoice. But this parameter seems have very little influence 
       to voicing calculation. */
    if (fTotalEng > fMaxEng)
      fMaxEng = 0.5F*(fTotalEng+fMaxEng);
    else
      fMaxEng = fMaxEng+0.01F*(fTotalEng-fMaxEng);

    if (fMaxEng > 40000.0F)          /* this energy is too small */
      fMaxEng = 40000.0F;

    fBiasEng = (0.005F*fMaxEng + fTotalEng)/(0.01F*fMaxEng+fTotalEng);

    /* ratio related to low band energy to high band energy. If high band
       energy is too high compare to low band energy, the speech is noise-like.
       However, this condition can never be over-used, because it also depends 
       on the channel condition. */
    fBiasL2H = (float)sqrt(fLowBandEng / (5.0F*fHighBandEng));
    if (fBiasL2H > 1.0F) fBiasL2H = 1.0F;
    

    /* ratio related to pitch value. When pitch is too large, the spectrum is 
       too smeared. It's due to bandwidth of harmonics is smaller than the window 
       bandwidth. For Kaiser window with beta = 6.0, the window bandwidth is 
       around 8. So I simplified the condition to: */
    fBiasP0 = 65.0F/fPitch;
    if (fBiasP0 > 1.0F) fBiasP0 = 1.0F;

    fBiasAll = fBiasP0 * fBiasL2H * fBiasEng;

    /* now we can make the decision based on the each harmonics */
    /* still use Suat's decision in this release until I find out
       more sophisticate classifier and threshold */
    fThrdDelta = fBiasAll * (float)CONSTANT4PV / fPitch;
    iVHarm = 0;
    for (i = 0; i < NHarm; i++) {
      fThreshold = fBiasAll - fThrdDelta * (float)i;
      /* make the threshold adaptive to previous decision */
      if(fPrevPv >= 0.85F) fThreshold *= 0.95F;
      else if (iPrevVUV[i]==1&&fPrevPv>=0.65F) fThreshold *= 0.9F;
      else if (iPrevVUV[i]==1&&fPrevPv>=0.5F)  fThreshold *= 0.85F;
      else if (iPrevVUV[i]==1)  fThreshold *= 0.75F;
      else     fThreshold *= 0.65F;

      /* make the decision by comparing the threshold and band error */
      if ( pBandErr[i] < fThreshold) {
	iPrevVUV[i] = 1;
	iVHarm ++;
      }
      else {
	iPrevVUV[i] = 0;
      }
    }

    for (; i < MAXHARM; i++)
      iPrevVUV[i] = 0;

    assert(NHarm > 0);
    

    Pv = getPv((short)iVHarm, (short)NHarm);   /* Pv=(float)((int)(iVHarm*NFFT/fPitch+0.5F))/(0.93F*NFFT_2); */

    /*************************************************************
      use Suat's decision
      ************************************************************/
    Z02Zmax = fTotalEng/fMaxEng;
    Zlf2Zhf = fLowBandEng/fHighBandEng;
    M = fBiasL2H * fBiasEng;
    if((Z02Zmax<0.0001F) ||
       (Z02Zmax<0.0005F && Zlf2Zhf<150.0F) ||
       (Z02Zmax<0.0012F && Zlf2Zhf<20.0F) ||
       (Z02Zmax<0.021F  && Zlf2Zhf<1.27F && M<0.35F && fPrevPv==0.0F) ||
       (Z02Zmax<0.02F   && Zlf2Zhf<1.13F && M<0.22F && fPrevPv==0.0F) ||
       (Z02Zmax<0.02F   && Zlf2Zhf<1.13F && M<0.28F && fPrevPv<0.12F) ||
       (Z02Zmax<0.023F  && Zlf2Zhf<1.0F  && M<0.19F && fPrevPv<0.12F) ||
       (Z02Zmax<0.041F  && Zlf2Zhf<0.3F  && M<0.15F && fPrevPv==0.0F) ||
       (Z02Zmax<0.115F  && Zlf2Zhf<0.62F && M<0.34F && fPrevPv==0.0F && fPitch>100.0F) ||
       (Z02Zmax<0.043F  && Zlf2Zhf<2.1F  && M<0.58F && fPrevPv==0.0F && fPitch>110.0F))
      {
	Pv=0.0F;
      }

    /*************************************************************
      use voicing activity classifier to find silence frames
      ************************************************************/
    if (iClass == 0) 
    {
      Pv = 0.0F;
    }
  }


  /*************************************************************
    save all the condition.
    ************************************************************/
  if (Pv == 0.0) {
    for (i = 0; i < MAXHARM; i++)
      iPrevVUV[i] = 0;
  }

  pVoicing_mblk->P_v = Pv;

  *pPv = Pv;

  pVoicing_mblk->Zmax = fMaxEng;

  pVoicing_mblk->P_v = Pv;

  return 0;
}

/*******************************************************************************
* Function:       VOXErr VoxInitVoicing(Voicing *myVoicing_mblk)
*
* Action:         initializes Voicing structure
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:	  VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
unsigned short VoxInitVoicing(void **Voicing_mblk)
{
  Voicing *myVoicing_mblk;
  /***** temporary memories *****/
  if(VOX_MEM_INIT(myVoicing_mblk=*Voicing_mblk,1,sizeof(Voicing))) 
      return 1;

  if(VOX_MEM_INIT(myVoicing_mblk->prev_vuv,MAXHARM,sizeof(short))) {
     VOX_MEM_FREE(myVoicing_mblk);
     return 1;
  }

  myVoicing_mblk->Zmax = 1000000.0F;    /*  was 20000.0 */
  myVoicing_mblk->P_v = 0.0F;

  return 0;

} /* initPDA() */

/*******************************************************************************
* Function:       VOXErr VoxFreeVoicing(Voicing *myVoicing_mblk)
*
* Action:         free's Voicing structure
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:         VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
unsigned short VoxFreeVoicing(void **hVoicing_mblk)
{
  Voicing *myVoicing_mblk=(Voicing*)*hVoicing_mblk;

  if (*hVoicing_mblk) {
    VOX_MEM_FREE(myVoicing_mblk->myVAD_mblk);

    VOX_MEM_FREE(myVoicing_mblk->prev_vuv);

    VOX_MEM_FREE(*hVoicing_mblk);
  }
  return 0;

} /* freeVoicing() */


