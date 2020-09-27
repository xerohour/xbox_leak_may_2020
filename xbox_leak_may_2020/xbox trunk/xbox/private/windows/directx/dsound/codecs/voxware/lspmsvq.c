/*
 * File:     MSVQlpc.c
 *
 * Project:  RT24-HQ
 * 
 * Written by Rob Zopf.
 *
 * Modified by Xiangdong Fu
 *
 * Copyright 1996  Voxware, Inc.
 *
 */
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "VoxMem.h"
#include "LspMSVQ.h"
#include "model.h"

#include "xvocver.h"

#ifdef _WIN64
#undef WIN32
#endif

#ifdef WIN32
static float CalcWeightedMSE(float *weight,float *x, float *y, int Len);
#endif
/* *******************************************************************
   NAME: VQstfCoeffs

   DESCRIPTION:
      Vector quantizes a set of short term filter coefficients
      using a multi-stage M-L tree search algorithm.

   INPUTS:
         vectin	     : vector for quantization [0...n-1]
         lpcin       : lpc coeffs for weighting (if quantizing lsf's) [0...n-1]
         myMSVQ_mblk : pointer to MSVQ structure
         numStages   : number of codebook stages
         numCand     : number of search candidates
         dim         : the dimension of vectin
         tableSize   : number of vectors in each MSVQ table
         cb          : codebooks;   cb[1..numStages]
         DoLSF       : 0 = general input, 1 = LSF input
   OUTPUTS:
         vectin      : quantized coefficients
         cbNdx       : codebook indeces; cbNdx[1..numStages]

   ***********************************************************************/
unsigned short VoxLspMSVQ (float vectin[], float lpcin[], unsigned short cbNdx[], MSVQ_mblk *myMSVQ_mblk,
                  short numStages, short numCand, int dim, short tableSize, 
                  const float **cb)
{
   short    c, s, entry, ncPrev,  i, r;
   float       d;
   short    *newIndex=myMSVQ_mblk->newIndex,*tempshort;
   float    *tempfloat,*u;
   float	*newresidue = myMSVQ_mblk->newresidue; 
   float	*dMin = myMSVQ_mblk->dMin;
   float	*residue = myMSVQ_mblk->cand;
   float	*cand = myMSVQ_mblk->cand;
   short	*index = myMSVQ_mblk->index;
   short	*nextIndex = myMSVQ_mblk->nextIndex;
   float	*uHat;
   float	*tbl;
   float	weight[LPC_ORDER];
   short	bestIndex, candBAD;   
   MTHT_complex      hw, dhw;
   float             dm_sq, cs, sn, tmp;
   int               k,j;


   for (r=0;r<dim;r++)  
 	  residue[r] =  vectin[r]/4000.0F;
    
   for(j=0; j <dim; j++) {
     cs = (float)cos((double)(residue[j]*PI));
     sn = -(float)sin((double)(residue[j]*PI));

     dhw.re = cs*dim*(-lpcin[dim-1]);
     dhw.im = sn*dim*(-lpcin[dim-1]);

     hw.re = cs*(-lpcin[dim-1]);
     hw.im = sn*(-lpcin[dim-1]);  
    
     for(i=dim-2; i >= 0; i--) {
        /* add a[i] */
        dhw.re += (float)(i+1)*(-lpcin[i]);
        hw.re += -lpcin[i];

        /* multiply by exp(-jw) */
        dhw.im = cs*(tmp=dhw.im) + sn*dhw.re;
        dhw.re = cs*dhw.re - sn*tmp;

        /* multiply by exp(-jw) */
        hw.im = cs*(tmp=hw.im) + sn*hw.re;
        hw.re = cs*hw.re - sn*tmp;

     }
     /* scale result by j and add 1 */
     tmp = dhw.re;
     dhw.re = -dhw.im;
     dhw.im = tmp;

     hw.re = 1.0F - hw.re;
     hw.im = -hw.im;

     dm_sq = hw.re*hw.re + hw.im*hw.im;

     /* catch divide by zero (zero on or very near unit circle)
        -1 indicates to calling routine that error (or something funny
            occurred) */
     if (dm_sq <= (float)FLT_MIN)
       weight[j] = 1.0F;  
     else if (dm_sq >= (float)FLT_MAX)
        weight[j] = (float)(dim+1)*D_RATIO;
        
     else  {
       weight[j] = (float)dim + 1.0F + 2.0F*(dhw.im*hw.re - dhw.re*hw.im)/dm_sq;

       if (weight[j] > D_MAX)
         weight[j] = 1.0F;
       else if (weight[j] > D_CRIT)
         weight[j] = (float)sqrt((double)weight[j]/D_MAX);
       else
         weight[j] *= D_RATIO;
     }
     if (residue[j] > W_CRIT)
         weight[j] *= 1.0F-2.0F*(residue[j]-W_CRIT)/3.0F;

   }

   

/*********************************************************************
   Initialize the data before starting the tree search.
      - the number of candidates from the "previous" stage is set
        to 1 since there is no previous stage!
      - the candidate vector from the previous stage is set to zero
      - the list of indeces for each candidate is set to 1
  *********************************************************************/
   for (c=0; c<numCand; c++) {
       for (s=0; s<numStages; s++)
         index[c*numStages+s] = nextIndex[c*numStages+s] = 0;
   }
   ncPrev = 1;
   
/* ********************************************************************
   Now we start the search:
      For each stage
         For each candidate from the previous stage
            For each entry in the current stage codebook
               * add the codebook vector to the current candidate
               * compute the distortion with the target
               * retain as a candidate if it is one of the best so far

   ******************************************************************** */
   for (s=0; s<numStages; s++) {
      /* set the distortions to huge values */
      for (c=0; c<numCand; c++)
         dMin[c] = (float)FLT_MAX;

	   /* Loop for each previous candidate selected, and try each entry */
     
      for (c=0, u=residue; c<ncPrev; u+=dim, c++) {   
         for (entry=0,uHat=(float *)cb[s]; entry <tableSize; entry++, uHat+=dim) {
#ifndef WIN32
            for(i=dim-1,d=0.0F;i>=0;i--)
 	            d += weight[i]*(MTH_SQR(uHat[i] - u[i]));
#else  /* WIN32 */
            d=CalcWeightedMSE(weight,uHat,u,dim);
#endif /* WIN32 */
/* ********************************************************************
   If the error for this entry is less than the worst retained
   candidate so far, keep it. Note that the error list is maintained
   in order of best (min error) to worst.
   ******************************************************************** */
            if (d < dMin[numCand-1]) { 
               for(k=numCand-1;k>0&&d<=dMin[k-1];k--) {
		            dMin[k] = dMin[k-1];
		            newIndex[k+k] = newIndex[k+k-2];
		            newIndex[k+k+1] = newIndex[k+k-1];
		         }
               dMin[k] = d;
               newIndex[k+k] = c;
               newIndex[k+k+1] = entry;
            }

         } /* for each entry */
      } /* Try out each previous candidate codevector */
    
/* ********************************************************************
   Compute the number of candidate vectors which we kept for the next
   stage. Note that if the size of the stages is less than the number
   of candidates, we build them up using all entries until we have
   kept numCand candidates.
   ******************************************************************** */
      ncPrev = MTH_MIN (ncPrev*tableSize, numCand);

/* ********************************************************************
   We now have the  best indeces for the stage just completed, so
   compute the new candidate vectors for the next stage...
   ******************************************************************** */
 	  for (c=0; c<ncPrev; c++) {
         register short newcand;
         newcand = newIndex[c+c]; 
         nextIndex[c*numStages+s] = newIndex[c+c+1];
         for (i=0; i<s; i++)
            nextIndex[c*numStages+i] = index[newcand*numStages+i];
         tbl = (float *)cb[s]+newIndex[c+c+1]*dim;
		   for (i =0; i<dim; i++)
			   newresidue[c*dim+i] = residue[newcand*dim+i]-tbl[i];
     }
     tempshort = index;
	  tempfloat = residue;
	  index = nextIndex;
	  residue = newresidue;
      nextIndex = tempshort;
      newresidue = tempfloat;
   } /* for each stage */
   for (c=0; c<numCand; c++) {
      for(i = 0; i<dim; i++) 
         cand[c*dim+i] = 0.0F;
         for (i=0; i<numStages; i++) {
            tbl = (float *)cb[i]+index[c*numStages+i]*dim;
            u = cand+c*dim;
            for (r=0; r<dim; r++)
                *u++ += *tbl++;
         }
   }

   i=1;    
   while (i<dim&&cand[i]>=cand[i-1]) i++;
   if (i<dim&&cand[i]<cand[i-1]) {
   for (j=2;j<=dim;j++) {
      i=j-1;
      while (i>0 && cand[i-1] > cand[j-1]) {
         cand[i]=cand[i-1];
         i--;
      }
      cand[i]=cand[j-1];
   }
   }
   for (c=0; c<numCand; c++){
      candBAD=0;
      for (i=1;i<dim;i++){
         if ((cand[c*dim+i]-cand[c*dim+i-1]) <LSPDIFF_THRESHOLD){
            candBAD=1; 
         }
      }
      if (candBAD==0){
         bestIndex=c;
         break;
      }
   }
   if ((c>=numCand)&&(candBAD==1)){
      bestIndex=0;
      for (i=1;i<dim;i++){
         if ((cand[c*dim+i]-cand[c*dim+i-1]) <LSPDIFF_THRESHOLD){
            cand[i] = cand[i-1]+LSPDIFF_THRESHOLD;
         }
      }
   }
   for (r=0; r<dim; r++)
      vectin[r] = cand[bestIndex*dim+r]*4000.0F;
   
   for (s=0; s<numStages; s++)
      cbNdx[s] = index[bestIndex*numStages+s];

   return 0;
} /* VQstfCoeffs */

/* *******************************************************************
   NAME: DecodeMSVQcoeffs

   DESCRIPTION:
         Decode the quantized vector using the MSVQ structure and
         the quantized indices.

   ********************************************************************/
unsigned short VoxDecLspMSVQ (unsigned short index[], float vect[], const float **cb, short dim, 
                       short numStages)
{
   int         i,j, s, ndx;
   float	*tbl;

   for (i=0; i<dim;i++)
         vect[i] = 0.0F;

   for (s=0; s<numStages; s++) {
       ndx = index[s];
       tbl = (float *)cb[s]+(ndx)*dim;
       for (i=0; i<dim; i++) 
           vect[i] += tbl[i];
   }


   /* ********************************************************************
      Note that we check to see if the quantized LSPs are stable, and if
      not, we sort them so that they are stable.  The this "trick" is
      (of course!) also used in the encoder.
      ******************************************************************** */
   i=1;    
   while (i<dim&&vect[i]>=vect[i-1]) i++;
   if (i<dim&&vect[i]<vect[i-1]) {
     for (j=2;j<=dim;j++) {
         i=j-1;
         while (i>0 && vect[i-1] > vect[j-1]) {
           vect[i]=vect[i-1];
           i--;
         }
         vect[i]=vect[j-1];
     }
   }
                  
     for (i=1;i<dim;i++){
         if ((vect[i]-vect[i-1]) < LSPDIFF_THRESHOLD)
             vect[i] = vect[i-1]+LSPDIFF_THRESHOLD;
     }

     /* un-normalize */
     for (i=0; i<dim; i++)
         vect[i] *= (4000.0F);

     return 0;

} /* DecodeMSVQcoeffs */


unsigned short VoxInitMSVQ(void** hMSVQ_mblk)
{
   MSVQ_mblk *pMSVQ_mblk;

   if (VOX_MEM_INIT(pMSVQ_mblk=*hMSVQ_mblk, 1, sizeof(MSVQ_mblk))) 
      return 1;


   if(VOX_MEM_INIT(pMSVQ_mblk->dMin,MAX_NUMCAND_MSVQ,sizeof(float))) 
      return 1;

   if(VOX_MEM_INIT(pMSVQ_mblk->cand,MAX_NUMCANDxTABLEDIM_MSVQ,sizeof(float))) 
      return 1;

   if(VOX_MEM_INIT(pMSVQ_mblk->index,MAX_NUMCANDxNUMSTAGES,sizeof(short))) 
      return 1;

   if(VOX_MEM_INIT(pMSVQ_mblk->nextIndex,MAX_NUMCANDxNUMSTAGES,sizeof(short))) 
      return 1;

   if(VOX_MEM_INIT(pMSVQ_mblk->u,MAX_TABLEDIM_MSVQ,sizeof(float))) 
      return 1;

   if(VOX_MEM_INIT(pMSVQ_mblk->lpc,MAX_TABLEDIM_MSVQ,sizeof(float))) 
      return 1;
  
   if(VOX_MEM_INIT(pMSVQ_mblk->newIndex,MAX_NUMCAND_MSVQ<<1,sizeof(short))) 
      return 1;

   if(VOX_MEM_INIT(pMSVQ_mblk->newresidue,MAX_NUMCANDxTABLEDIM_MSVQ,sizeof(float))) 
      return 1;

   return 0;
}


unsigned short VoxFreeMSVQ(void **hMSVQ_mblk)
{
   MSVQ_mblk *myMSVQ_mblk = (MSVQ_mblk *) *hMSVQ_mblk;

   VOX_MEM_FREE(myMSVQ_mblk->dMin);
   VOX_MEM_FREE(myMSVQ_mblk->cand);
   VOX_MEM_FREE(myMSVQ_mblk->index);
   VOX_MEM_FREE(myMSVQ_mblk->nextIndex);
   VOX_MEM_FREE(myMSVQ_mblk->lpc);
   VOX_MEM_FREE(myMSVQ_mblk->u);

   VOX_MEM_FREE(myMSVQ_mblk->newIndex);
   VOX_MEM_FREE(myMSVQ_mblk->newresidue);

   VOX_MEM_FREE(*hMSVQ_mblk);

   return 0;
}

#ifdef WIN32
#pragma warning(disable:4035)
static float CalcWeightedMSE(float *weight,float *x, float *y, int Len)
{
  _asm
  {
     mov ecx, Len;
     mov esi, x;
   
     mov ebx, weight;
     mov edi, y;
   
     fldz;
   
     fld st(0);

loop1:

     faddp st(1), st(0);

     /** difference **/
     fld DWORD PTR[esi];
   
     fsub DWORD PTR[edi];
   
     fld DWORD PTR[esi+4];
	  
	  fsub DWORD PTR[edi+4];
	  
	  fld DWORD PTR[esi+8];

     fsub DWORD PTR[edi+8];
 
     fld DWORD PTR[esi+12];
	  
	  fsub DWORD PTR[edi+12];
	  
	  fld DWORD PTR[esi+16];

     fsub DWORD PTR[edi+16];

     /** square **/
     fxch st(4);
     fmul st(0), st(0);   /* 1 ~4 ~3 ~2 ~5 */

     fxch st(3);
     fmul st(0), st(0);  /* 2,~4,~3 1, ~5 */

     fxch st(2);
     fmul st(0), st(0);  /* 3 ~4, 2, 1, ~5 */

     fxch st(1);
     fmul st(0), st(0);  /* 4, 3, 2, 1, ~5 */

     fxch st(4);
     fmul st(0), st(0);  /* 5, 3, 2, 1, 4 */

     /** multiple the weights **/
     fxch st(3);         /* ~1, 3, 2, 5, 4 */
     fmul DWORD PTR[ebx];

     fxch st(2);         /* ~2, 3, ~1, 5, 4 */
     fmul DWORD PTR[ebx+4];

     fxch st(1);
     fmul DWORD PTR[ebx+8]; /* ~3, ~2, ~1, 5, 4 */

     fxch st(4);
     fmul DWORD PTR[ebx+12];/* ~4, ~2, ~1, 5, ~3 */

     fxch st(3);
     fmul DWORD PTR[ebx+16];/* ~5, ~2, ~1, ~4, ~3 */

     fxch st(2);
     faddp st(5), st(0); /* 2, 5, 4, 3, 1+sum */

     faddp st(3), st(0); /* 5,4, 3+2 1+sum */

     faddp st(1), st(0); /* 4+5, 3+2, 1+sum */

     add ebx, 20;
     add esi, 20;

     add edi, 20;
     sub ecx, 5;

     fxch st(2);         /* 1+sum, 3+2 , 4+5 */
     faddp st(1), st(0); /* 1+sum+3+2, 4+5 */

     jnz    loop1;

     faddp st(1), st(0); /* add to one term */
  }
}

#ifdef _WIN64
#define WIN32
#endif
#endif
