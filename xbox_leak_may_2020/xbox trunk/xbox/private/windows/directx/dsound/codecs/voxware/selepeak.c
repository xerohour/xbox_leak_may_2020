/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:       SelePeak.c
*                                                                              
* Purpose:        Choose "good" peaks for frequency-domain pitch estimator.
*                                                                              
* Functions:      SelectPeaks()
*
* Author/Date:    
*
********************************************************************************
* Modifications:
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/SelePeak.c_v   1.3   02 Apr 1998 14:47:00   weiwang  $
******************************************************************************/
/**** system include files ****/
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

/**** Vox Lib include files ****/
#include "vLibTran.h"
#include "vLibSpch.h"
#include "vLibVec.h"
#include "vLibMath.h"

/**** Codec related include files ****/
#include "VoxMem.h"
#include "LDPDAdef.h"
#include "SelePeak.h"

#include "xvocver.h"


/**** local definitions ****/
#define MAX_LPC_MASKING_ORDER   7 
#define MASKING_ATTENUATION     0.2F 
#define LPC_MASKING_ORDER       5 
#define EXPANSION_FACTOR        0.98F

#define MAX_PEAK_THRESHOLD    6.5536e-4F  

#define DFFT_ORDER  6
#define DFFT        (1<<DFFT_ORDER)
#define DFFT_2      (DFFT>>1)
#define INV_DFFT_2  (1.0F/(float)(DFFT_2))

#define  N_SHIFT  15
#define  CONSTSCALE     (float)(1<<N_SHIFT)
#define  ROUND_OFFSET   (int)(1<<(N_SHIFT-1))

/* fix-point implementation of round(fScale*i) */
/* !!!iscale has to be pre-calculated. */
#define ConvertIndex(iScale, i)   ((iScale*i+ROUND_OFFSET)>>N_SHIFT) 


/**** private functions ****/
static 
void CalcRSbyPower( STACK_R const float *pfPower, int iBaseBand, 
                    float *pfRs, int iLPCorder );

static 
void CalcMaskEnvelope( STACK_R const float *pfAlpc, int iLPCorder, 
                       float *pfMaskEnv, int iNBand );



/******************************************************************************
* Function:  SelectPeaks()
*
* Action:    Select peaks from the power spectrum and eliminate small peaks
*            due to background-noise.
*
* Input:     pfPower    -- input power spectrum. 
*            iBaseBand  -- the length of input power spectrum .
*            piPeakFreq -- the address of the output peak indecies.
*
* Output:    piPeakFreq -- the output peak indecies.
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*
******************************************************************************/

void SelectPeaks( STACK_R 
                  const float *pfPower, 
                  int         iBaseBand, 
                  int         *piPeakFreq, 
                  float       fMaxPower
                )
{

  /***** local varibles ******/
  int   i,k;
  int   iIndex;
  int   iNPeaks;
  float fGain;
  float fPowerThrd;
  int   iLPCorder;

  /***** STACK memory ******/
STACK_INIT
  float pfAlpc[MAX_LPC_MASKING_ORDER];
STACK_INIT_END 

  STACK_ATTACH(float *, pfAlpc)
STACK_START

  assert(pfPower != NULL);
  assert(piPeakFreq != NULL);

  /*----------------------------------------------------------------------
    (1) Pick the peaks regularly from the power spectrum.
    ----------------------------------------------------------------------*/
  PeakPick( pfPower, 0, EXTEND_CUT_LC+1, MAX_NPEAKS,
            piPeakFreq, &iNPeaks);

  /*----------------------------------------------------------------------
    (2) Compute LPC coefficients.
    ----------------------------------------------------------------------*/

  /***  Find the lpc order. ***/
  iLPCorder = (int)ceil((float)LPC_MASKING_ORDER*(float)iBaseBand/
                       (float)EXTEND_CUT_LC);
  if (iLPCorder > MAX_LPC_MASKING_ORDER)
    iLPCorder = MAX_LPC_MASKING_ORDER;

  /*** Compute RS and LPC ***/
  {
STACK_INIT 
  float pfRs[MAX_LPC_MASKING_ORDER+1];
  float pfPARCOR[MAX_LPC_MASKING_ORDER];
STACK_INIT_END

   STACK_ATTACH(float *, pfRs)
   STACK_ATTACH(float *, pfPARCOR)

STACK_START
  /*
   * Compute autocorrelation coefficients from power spectrum.
   * And the autocorrelation coefficients are the inverse FFT of power spectrum.
   */
  CalcRSbyPower(STACK_S pfPower, iBaseBand, pfRs, iLPCorder);


  /***  Calculate the LPC coefficients. ***/
  pfRs[0] *= 1.000001F;
  VoxDurbin(pfRs, iLPCorder, pfAlpc, pfPARCOR, &fGain);
  BandExpand(pfAlpc, pfAlpc, iLPCorder, EXPANSION_FACTOR);
STACK_END
  }

  /*----------------------------------------------------------------------
   (3) Get masking envelope and apply it to eliminate small peaks.
   ----------------------------------------------------------------------*/
  {
STACK_INIT
  float pfMaskEnv[EXTEND_CUT_LC+1];
STACK_INIT_END

  STACK_ATTACH(float *, pfMaskEnv)

STACK_START
  /*** Compute the Masking Envelope from the inverse all-pole filter. ***/
  CalcMaskEnvelope(STACK_S pfAlpc, iLPCorder, pfMaskEnv, iBaseBand);

  /*** Scale the residue energy.***/
  fGain *= MASKING_ATTENUATION;

  /*** Reject the peaks below the LPC masking envelope and the global
    threshold. ***/
  fPowerThrd = fMaxPower * MAX_PEAK_THRESHOLD;
  k = 0;
  for (i = 0; i < iNPeaks; i++) 
  {
    iIndex = piPeakFreq[i];

    if ((iIndex<iBaseBand) && ((pfPower[iIndex]*pfMaskEnv[iIndex]) >= fGain)
          && (pfPower[iIndex] > fPowerThrd))
      {
        piPeakFreq[k] = piPeakFreq[i];
        k++;
      }
  }

  piPeakFreq[k] = SHRT_MAX;   /* a big number for ending */
STACK_END
  }

STACK_END
}


/******************************************************************************
* Function:  CalcRSbyPower
*
* Action:    compute autocorrelation coefficients from power spectrum.
*
* Input:     pfPower    -- input power spectrum
*            iNBand     -- the length of input power spectrum
*            pfRs       -- address for the output autocorrelation coefficients
*            iLPCorder  -- LPC order
*
* Output:    pfRs       -- autocorrelation coefficients
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*
******************************************************************************/

static 
void CalcRSbyPower( STACK_R 
                    const float *pfPower, 
                    int         iNBand, 
                    float       *pfRs,
                    int         iLPCorder
                  )
{
  int   iScale, i, j;
  float fScale;
  float ftmp;

STACK_INIT
  float pfMaskPower[DFFT+2];
STACK_INIT_END

STACK_ATTACH(float *, pfMaskPower)

STACK_START

  assert(pfPower != NULL);
  assert(pfRs != NULL);

  /*----------------------------------------------------------------------
    (1) Rescale the power spectrum of "DFFT" size. 
    Use fix-point index.
    ----------------------------------------------------------------------*/
  fScale = (float)iNBand/(float)DFFT_2;
  ftmp = fScale*CONSTSCALE;
  VoxROUND2(ftmp, iScale);

  j = 0;
  pfMaskPower[j++] = 0.0F;
  pfMaskPower[j++] = 0.0F;

  for (i = 1; i < DFFT_2; i++)
  {
    pfMaskPower[j++] = pfPower[ConvertIndex(iScale, i)];
    pfMaskPower[j++] = 0.0F;
  }
  pfMaskPower[j++] = 0.0F;
  pfMaskPower[j++] = 0.0F;

  /*----------------------------------------------------------------------
    (2) Do inverse FFT 
    ----------------------------------------------------------------------*/
  VoxRealInverseFft(pfMaskPower, DFFT_ORDER, IFFT_SCALE_FLAG);

  /*----------------------------------------------------------------------
    (3) copy to RS array.
    ----------------------------------------------------------------------*/
  memcpy(pfRs, pfMaskPower, (iLPCorder+1)*sizeof(float));

STACK_END
}


/******************************************************************************
* Function:  CalcMaskEnvelope()
*
* Action:    calculate masking envelope.
*
* Input:     pfAlpc     -- LPC coefficients
*            iLPCorder  -- LPC order
*            pfMaskEnv  -- address for the output masking envelope
*            iNBand     -- length for masking envelope
*
* Output:    pfMaskEnv  -- masking envelope
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*
******************************************************************************/

static 
void CalcMaskEnvelope( STACK_R 
                       const float *pfAlpc, 
                       int         iLPCorder, 
                       float       *pfMaskEnv, 
                       int         iNBand
                     )
{
  int i, j;
  int iScale;
  float ftmp;
  float fScale;

STACK_INIT  
  float pfMaskPower[DFFT+2];
STACK_INIT_END

STACK_ATTACH(float *, pfMaskPower)

STACK_START

  assert(pfAlpc != NULL);
  assert(pfMaskEnv != NULL);

  /*----------------------------------------------------------------------
    (1) Compute the inverse all-pole envelope.
    ----------------------------------------------------------------------*/
  memcpy(pfMaskPower, pfAlpc, (iLPCorder+1)*sizeof(float));

  i = iLPCorder+1;
  memset(&(pfMaskPower[i]), 0, (DFFT+2-i)*sizeof(float));

  VoxRealFft(pfMaskPower, DFFT_ORDER);
  
  /*----------------------------------------------------------------------
    (2) compute the power spectrum of the all-pole envelope.
    And rescale it to [0, iNBand].
    ----------------------------------------------------------------------*/
  fScale = (float)DFFT_2/(float)iNBand;
  ftmp = fScale * CONSTSCALE;
  VoxROUND2(ftmp,iScale);

  for (i = 0; i < (iNBand-1); i++) /*for interval from [0 iNBand-2]*/
  {
    j = 2*ConvertIndex(iScale, i);
    pfMaskEnv[i] = pfMaskPower[j]*pfMaskPower[j];
    j++;
    pfMaskEnv[i] += pfMaskPower[j]*pfMaskPower[j];
  }

  j = 2*ConvertIndex(iScale, i);
  if (j >= DFFT)
    pfMaskEnv[i] = pfMaskEnv[i-1];
  else
  {
    pfMaskEnv[i] = pfMaskPower[j]*pfMaskPower[j];
    j++;
    pfMaskEnv[i] += pfMaskPower[j]*pfMaskPower[j];
  }

STACK_END
}

