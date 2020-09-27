/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:   NspIIR.c                                                                    
*                                                                              
* Purpose:    IIR filter routine                                                                 
*                                                                              
* Functions:  VoxNspIIRFlt()                                                                
*                                                                              
* Author/Date: Xiangdong Fu(originated from Intel NSP Lib)  12/10/96                                                              
********************************************************************************
* Modifications:
*                   
* Comments:   1. Inplace filtering is available for order > 1
*             2. The first coefficient of Denominator should always
*                be normalized to 1.0 and be ignored when filter
*                coefficients are packed into the "taps" array                                                         
*             -- 
*             Xiangdong Fu  12/10/96
* Concerns:
*
*******************************************************************************/



#include "NspIIR.h"

#include "xvocver.h"
/******************************************************************************
*  Input:
*          inSamps       --       input signal
*          numIters      --       Length of IIR filtering
*          order         --       IIR filter order
*          taps          --       filter taps([NUM:DEN])
*          dlyl          --       filter delay line
*  OutPut:
*          outSamps      --       output signal
*******************************************************************************/

void VoxNspIIRFlt(float *inSamps, float *outSamps, int numIters,
		   float *taps, int order, float *dlyl)


{
	
  int    ni; 
  float  taps0;
  float  out, out1;


  {
	/* arbitrary filter 2-nd order unrolling algorithm */
    int                i, ordm1, ordm2;
    float              inp ,  inp1;
    float              taps1, tapsh1, tapsh2, tapsh0 , tapsh00;
    float              *tapsh;
    float              dl;  

    if (order > 1 ) {    /*  filter order  > 0 */
      taps0   = taps[0];
      taps1   = taps[1];
      tapsh   = &taps[order];
      tapsh1  = taps[order-1];
      tapsh2  = tapsh[order-1];
      tapsh0  = taps[order];
      tapsh00 = taps[2*order];
      ordm1   = order - 1;
      ordm2   = order - 2;
      if ( numIters > 1  ) {
        if (numIters % 2) {   /* Sample number is odd */
          for (ni = 0; ni < numIters-1; ni += 2) {
            inp  = inSamps[ni];
            inp1 = inSamps[ni+1];
            out  = taps0 * inp + dlyl[0];
            outSamps[ni] = out;
            out1 = taps0 * inp1 + taps1 * inp - tapsh[1]*out + dlyl[1];
            outSamps[ni+1]=out1;
            for (i = 2; i < order; i++)
              dlyl[i-2] = taps[i-1] * inp1 - tapsh[i-1] * out1
                        + taps[i]   * inp  - tapsh[i]   * out  + dlyl[i];
            dlyl[ordm2] = tapsh1 * inp1 - tapsh2  * out1 + tapsh0 * inp
                        - tapsh00 * out;
            dlyl[ordm1] = tapsh0 * inp1 - tapsh00 * out1;
          } /* end ni */
          outSamps[numIters-1] = taps0 * inSamps[numIters-1] + dlyl[0]; /* last sample */
          for (i = 1; i < order; i++)
            dlyl[i-1] = taps[i] * inSamps[numIters-1]
                      - tapsh[i] * outSamps[numIters-1] + dlyl[i];
          dlyl[ordm1] = tapsh0 * inSamps[numIters-1]
                     - tapsh00 * outSamps[numIters-1];
        } /* end  if odd */
        else {              /* Sample number is even */
          for (ni = 0; ni < numIters; ni+=2) {
            inp  = inSamps[ni];
            inp1 = inSamps[ni+1];
            out  = taps0 * inp + dlyl[0];
            out1 = taps0 * inp1 + taps1 * inp - tapsh[1] * out + dlyl[1];
            outSamps[ni]   = out;
            outSamps[ni+1] = out1;
            for (i = 2; i < order; i++){
              dl = taps[i-1] * inp1 - tapsh[i-1] * out1
                 + taps[i]   * inp  - tapsh[i]   * out + dlyl[i];
              dlyl[i-2] = dl;
            }/* end i */
            dlyl[ordm2] = tapsh1 * inp1 - tapsh2  * out1
                        + tapsh0 * inp  - tapsh00 * out;
            dlyl[ordm1] = tapsh0 * inp1 - tapsh00 * out1;
          } /* end ni */
        } /* end else even */ 
      }
      else  { /*  numIters = 1 */
        outSamps[0] = taps0 * inSamps[0] + dlyl[0]; 
        for (i = 1; i < order; i++)
          dlyl[i-1] = taps[i] * inSamps[0]
                    - tapsh[i] * outSamps[0] + dlyl[i];
        dlyl[ordm1] = tapsh0 * inSamps[0]
                    - tapsh00 * outSamps[0];
      }
    } /* end if order > 1 */

    else {
      if (order) {
        for (ni = 0; ni < numIters; ni++) {
          outSamps[ni] = taps[0] * inSamps[ni] + dlyl[0];
          dlyl[0] = taps[1] * inSamps[ni]
                  - taps[2] * outSamps[ni];
        }
      }
      else {
        for (ni = 0; ni < numIters; ni++)
        outSamps[ni] = taps[0] * inSamps[ni];
      }
    }
  }
}  /* end voxnspiirflt()  */
