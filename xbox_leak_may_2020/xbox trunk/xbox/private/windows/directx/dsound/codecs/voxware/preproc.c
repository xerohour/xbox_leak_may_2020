/******************************************************************************
*
*                        Voxware Proprietary Material
*
*                        Copyright 1997, Voxware, Inc
*
*                            All Rights Reserved
*
*
*                       DISTRIBUTION PROHIBITED without
*
*                      written authorization from Voxware
*
******************************************************************************/
 
/******************************************************************************
*
* Filename:    PreProc.c
*
* Purpose:    Preprocess input speech to remove the DC component. 
*
* Functions: VoxPreProcess()
*            VoxInitPreProc()
*            VoxFreePreProc()
*
* Author/Date: Wei Wang, 1995.
*
*******************************************************************************
*
* Modifications: 11/25/97, 
*                According to Raymond's suggestion, check the first value
*                of each frame. If it's too small, set it to 0.
*
*                11/24/97, 
*                Wei Wang, only add the noise floor to the first sample of 
*                each frame.
*
* Comments: 
*
* Concerns: (1) need convert short to float.
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/PreProc.c_v   1.3   06 Mar 1998 16:57:46   weiwang  $
******************************************************************************/

#include <stdlib.h>
#include <assert.h>
#include "vLibFilt.h"
#include "vLibMacr.h"
#include "vLibMath.h"

#include "xvocver.h"

/***** preprocessing structure (local structure)******/
typedef struct
{
  float fInX;                 /* direct-form IIR filter internal memory */
  float fOutY;                /* direct-form IIR filter internal memory */
  float cPP_Coef;            /* IIR filter coefficient, which is close to 1 */
} PRE_PROC_mblk;


/***** Definition to prevent under-flow *****/
static const float minValue = 1.0e-5F;  


/******************************************************************************
*
* Function:  VoxPreProcess()
*
* Action:    First-order high-pass filter to remove DC.
*
* Input:     piIn -- the input speech signal (16-bit PCM)
*            pfOut -- the point for the output speech
*            N -- number of samples for both input and output signal
*            hPreProc_mblk  -- internal memory for the processing
*
* Output:    pfOut -- the output speech after the preprocessing.
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
*    first order IIR filter:  pfOut[n]=(piIn[n]-piIn[n-1])+C*pfOut[n-1],
*    where C is the filter coefficient.
*******************************************************************************
*
* Modifications:
*
* Comments: 
*
* Concerns/TBD:  (1) Need to convert from short to float.
*                (2) For some machine, like SPARC, under-flow problem may occur
*                    for floating point operation. So a very small offset is 
*                    updated every frame to prevent it. For fix point 
*                    implementation, just set this value to 0.
*                    For example, if x[0] = 1 at first frame, then all 0s. For
*                    minimum floating number 1.175494351E-38, only 8690 samples 
*                    0 input will cause under-flow problem.
******************************************************************************/

void VoxPreProcess(const short *psIn, float *pfOut, int N, void *hPreProcMblk)
{
  float x1, x, y;
  float Coef = ((PRE_PROC_mblk *)hPreProcMblk)->cPP_Coef;

  /***** error checking *****/
  assert((psIn != NULL) && (pfOut != NULL));
  assert(hPreProcMblk != NULL);

  /***** get the memory from the previous frame *****/
  x = ((PRE_PROC_mblk *)hPreProcMblk)->fInX;
  y = ((PRE_PROC_mblk *)hPreProcMblk)->fOutY;

  if (VoxABS(y) < minValue)
     y =  0.0F;

  /***** filter the signal sample-by-sample *****/
  while ((N--) > 0)
    {
      x1 = (float)*psIn++;
      y = x1 - x + Coef * y;
      x = x1;
      *pfOut++ = y;
    }
  
  /***** update the internal memory *****/ 
  ((PRE_PROC_mblk *)hPreProcMblk)->fInX = x;
  ((PRE_PROC_mblk *)hPreProcMblk)->fOutY = y;
}



/******************************************************************************
*
* Function:  VoxInitPreProc()
*
* Action:    initialize the preprocessing structure.
*
* Input:     hPreProcMblk -- pointer to the preprocess structure (NULL pointer).
*            cPP_Coef -- filter coefficient
*
* Output:    hPreProcMblk -- memory allocated preprocess structure.
*
* Globals:   none
*
* Return:    0 : succeed
*            1 : fail
*******************************************************************************/
unsigned short VoxInitPreProc(void **hPreProcMblk, const float cPP_Coef)
{
  unsigned short RetFlag = 0;

  *hPreProcMblk = (void *)calloc(1, sizeof(PRE_PROC_mblk));
  if (*hPreProcMblk == NULL)
      RetFlag = 1;
  else {
     /* initialize the internal memories */
    ((PRE_PROC_mblk *)*hPreProcMblk)->fInX = 0.0F;
    ((PRE_PROC_mblk *)*hPreProcMblk)->fOutY = 0.0F;
    ((PRE_PROC_mblk *)*hPreProcMblk)->cPP_Coef = cPP_Coef;
  }

  return RetFlag;
}

/******************************************************************************
*
* Function:  VoxFreePreProc()
*
* Action:    free the preprocessing structure.
*
* Input:     hPreProcMblk -- the pointer of the preprocess structure 
*
* Output:    hPreProcMblk -- NULL preprocess structure
*
* Globals:   none
*
* Return:    none
*******************************************************************************/

void VoxFreePreProc(void **hPreProcMblk)
{
  SafeFree(*hPreProcMblk);
}

