/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:     enhance.c
*                                                                              
* Purpose:      Frequency-domain based perceptual weighting of the 
*		          harmonic amplitudes based upon the spectrum of the LPC
*		          coefficients.
*                                                                              
* Functions:    VOXErr VoxEnhance(float *lpc, float Pv, float PitchLag, 
*                                 int nHarm, float *HarmAmp, 
*                                 ENHANCE *EnhancePTR);
*               VOXErr VoxInitEnhance(ENHANCE *EnhancePTR);
*               VOXErr VoxFreeEnhance(ENHANCE *EnhancePTR);
*                                                                              
* Author/Date:  Rob Zopf (originally from Suat)                                                              
********************************************************************************
* Modifications:Ilan Berci/ 10/96 Removed global dependencies
*                                 Modularized code segments (Encapsulation)
*                                 Removed scope changes
*                                 Changed error code procedure
*                                 Removed useless code segments
*                                 General optimization
*
*               XF    1/15/97     Add comments and explanations for optimization
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/enhance.c_v   1.6   20 Apr 1998 14:41:34   weiwang  $
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include <assert.h>

#include "vLibTran.h" /* VoxRealFft() */

#include "model.h"
#include "VoxMem.h"

#ifdef _WIN64
#undef WIN32
#endif

#ifdef WIN32
static float myPower(float x, float y);
#endif

#include "enhance.h"

#include "xvocver.h"

/*******************************************************************************
* Function:   VOXErr VoxEnhance(float *lpc, float Pv, float PitchLag, int nHarm,
*                               float *HarmAmp, ENHANCE *EnhancePTR)
*
* Action:     Perceptual weighting filter
*
* Input:      lpc		- the lpc coefficients
*		        Pv		- voicing probability
*		        PitchLag	- time-domain pitch lag
*		        nHarm		- number of harmonics to be weighted
*		        HarmAmp	- pointer to the harmonic amplitudes
*		        EnhancePTR	- pointer to the Enhance structure
*
*
* Output:     HarmAmp	- weighted harmonic amplitudes
*
* Globals:    none
*
* Return:     VOVErr : Voxware return code
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

#define SMALL_NUMBER                      0.000001F

#if (USE_POWER_ESTIMATE==1)
const float Poly125[] = 
{
#include "pow125.h"
};

const float Poly150[] =
{
#include "pow150.h"
};
#endif


unsigned short VoxEnhance(STACK_R float *lpc, float Pv, float PitchLag, int nHarm, float *HarmAmp)
{
   STACK_INIT
   float HWR[ENHANCE_FFT+2];
   float amp[MAXHARM_2];
   short freq[MAXHARM_2];
   STACK_INIT_END

   STACK_ATTACH(float*,HWR)
   STACK_ATTACH(float*,amp)
   STACK_ATTACH(short*,freq)
   float *TT = HWR;
   float slope, lf;
   float TTt;
   float scale;

   float XK;
   register int i = 0,k;
   int PeakPos = 1;
	long Fi, Fo;
	int FiInt;

#if USE_POWER_ESTIMATE==0
   float POWFactor; 
#endif

   STACK_START
   /********************************************************
    Check if LPCs are zero since this leads to trouble later
    ********************************************************/
   while ((i<LPC_ORDER) && (lpc[i+1]!=0.0F))
   {
      i++;
   }/*i==LPC_ORDER is check passes, no LPC is zero*/
   
   if ((i==LPC_ORDER) && (nHarm!=0))
   {

   /********************************************************
    			Calculate LPC Filter Response
    ********************************************************/

      memcpy( HWR, lpc, (LPC_ORDER+1)*sizeof(float) );
      memset( HWR+LPC_ORDER+1, 0, (ENHANCE_FFT-LPC_ORDER-1)*sizeof(float) );

      VoxRealFft( HWR, ENHANCE_FFT_REAL_ORDER );

      /** calculate the power spectrum of the LPCs **/
      for (i=0; i<ENHANCE_FFT2; i++)
         TT[i] = (HWR[2*i]*HWR[2*i] + HWR[2*i+1]*HWR[2*i+1] + SMALL_NUMBER);
         /* add small number to prevent divide by 0 */

   /********************************************************
    			Search For Formant Peaks
    ********************************************************/

   /*********************************************************
   *  Locate the peaks of 1/TT(or valleys of TT) and        *
   *        find  1/sqrt(TT) at those values                *
   *********************************************************/


      for (i=1; i < ENHANCE_FFT2-1; i++)
      {
     	  if ((TT[i]<TT[i-1]) && (TT[i]<TT[i+1]))
    	  { 
              amp[PeakPos] = (float)sqrt((double)(1.0F/TT[i]));
              freq[PeakPos++] = (short)i; /* Typically never exceeds 6 */
              i++;        /* can't hit another peak right away */
          }
      }
      
      amp[0] = (float)sqrt((double)(1.0F/TT[0])); 
      freq[0] = 0;

      amp[PeakPos] = (float)sqrt((double)(1.0F/TT[ENHANCE_FFT2-1]));
      freq[PeakPos++] = ENHANCE_FFT2-1;

    /********************************************************
     multiply by -0.5 instead of taking square root
     and then division, used the following two identities 
     to eliminate sqrt and div operation
     (sqrt(y))^k = y^(k/2)
     (1/y)^x = y^(-x)

     since interpolations are only needed at the harmonics, 
     there is no need to calculate the whole XK[] array, the
     trick here is : just calculate XK at the harmonics
    ********************************************************/
#if USE_POWER_ESTIMATE
      pwrTbl = (Pv<=0.1F)?Poly125:Poly150;
#else
#if 0 
      POWFactor = (Pv<=0.25F)?(SUPPRESSION_FACTOR2*-0.5F):(float)(SUPPRESSION_FACTOR*-0.5F);
#else
/* Pitch adaptive Post filter AGUILAR 2/26/97 */
if(Pv<=0.25){
	POWFactor=SF_0*(-0.5F);
  }
  else
 {
	if(PitchLag<=SF_P1){
		POWFactor=SF_1*(-0.5F);
	}
	else
	{
		if(PitchLag>SF_P1 && PitchLag <=SF_P2 ){
			POWFactor=SF_2*(-0.5F);
		}
		else
		{
			if(PitchLag>SF_P2)
				POWFactor=SF_3*(-0.5F);
		}
	}

 }
#endif
#endif

    /************************************************
      Fi = Fo = (float)(ENHANCE_FFT)/PitchLag;
     ************************************************/
      Fi = (long)((((long)1)<<(F_QFACTOR))*(((float)ENHANCE_FFT)/PitchLag));
      Fo = Fi;

      for (i=0,k=0; i<PeakPos-1; i++)
      {
         slope = (amp[i+1]-amp[i])/(float)(freq[i+1]-freq[i]); 
       /***********************************************
         FiInt = (int)Fi;
        ***********************************************/
			FiInt = (int)(Fi>>F_QFACTOR);

         while ((k<nHarm)&&(FiInt<freq[i+1]))
         {
            /*calculation the interpolation*/
#if (INTERPOLATE_BETWEEN_FFT_BINS==1)
            XK = amp[i] + slope*(float)((Fi*F_TO_FLOAT)-freq[i]);
#else
            XK = amp[i] + slope*(float)(((Fi+( ((long)1) <<(F_QFACTOR-1) ))>>F_QFACTOR)-freq[i]);
#endif

            /* calculate the weighting factor */
            /********************************************************************************
             Since the FFT size may be reduced, we can increase precision by interpolating
             between FFT bins to the frequency of the k'th harmonic instead of just
             selecting the nearest bin.  Ideally, we would like to do this interpolation
             on the magnitude spectrum of the inverse lpc spectrum, but to reduce complexity,
             we interpolate on the PSD of the lpc's, and leave the sqrt and the divide to 
             the power function which will handle it all together.  Interpolating the PSD
             of the lpc's will perform no worse than just selecting the nearest bin, since
             the interpolation is still monotonic, it's just not linear in the 1/MAG domain.

             Note that (Fi&(F_FRACTION_MASK))*F_TO_FLOAT = the fractional part of the FFT
                                                           frequency bin.
             ********************************************************************************/
#if (INTERPOLATE_BETWEEN_FFT_BINS==1)
            TTt = TT[FiInt] + (TT[FiInt+1]-TT[FiInt])*((Fi&(F_FRACTION_MASK))*F_TO_FLOAT);

           /*************************************************************************************
            This commented code is the optimal way to do the interpolation.
            -------------------------------------------------------------------------------------
            
            TTt  = 1.0/( (1.0/sqrt(TT[FiInt])) +  ((1.0/sqrt(TT[FiInt+1]))-(1.0/sqrt(TT[FiInt])))*
                   ((Fi&(F_FRACTION_MASK))*F_TO_FLOAT));
            TTt *= TTt;
            -------------------------------------------------------------------------------------
            *************************************************************************************/
#else
           /**************************************************************************************
            Just select the closest FFT bin 
            **************************************************************************************/
            TTt = TT[(Fi+( ((long)1) <<(F_QFACTOR-1) ))>>F_QFACTOR];
#endif
            scale = XK*XK*TTt;
            if (scale > 1.0)
            {
#if (USE_POWER_ESTIMATE==1)
               lf = powerpoly(scale, pwrTbl);  /* this function could be inlined */
#else
#ifndef WIN32
               lf = (float)pow((double)(scale), (double)POWFactor);
#else  /*WIN32 */
               lf = myPower(scale, POWFactor);
#endif /*WIN32 */
#endif
               if (lf < 0.3F) 
                  lf = 0.3F;

               /* weight the harmonic amplitudes */
               HarmAmp[k] *= lf;
            }
            Fi+=Fo;
            FiInt = (int)(Fi>>F_QFACTOR);
            k++;
         }          
      }
   } /* if fail */

   STACK_END
   return 0;
}

#ifdef WIN32

/** assmbly power **/
static float myPower(float x, float y)     /* result = x^^y*/   													 
{
	int IntPow;	
	float result;
   _asm												 		
   {											
      fld1					/*load 1*/				 
      fld		y			/*load y*/				 
      fld		x		    /*load x*/               
      fyl2x                /* y*log2(x)*/           
      fist     IntPow      /*save the integer part*/
      fisub    IntPow      /*fraction part left   */
      f2xm1                /* 2^^st(0)-1*/          
      faddp    st(1), st(0) /*2^^st(0), add 1*/     
      fstp     result                               
   }												 	
   if(IntPow>=0)   result *=(1<<IntPow);				 
   else   	    result /=(1<<(-IntPow));				 	
   return result;
}

#endif

#ifdef _WIN64
#define WIN32
#endif
