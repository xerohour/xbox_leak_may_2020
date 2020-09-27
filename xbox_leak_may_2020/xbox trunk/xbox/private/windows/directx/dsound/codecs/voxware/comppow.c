/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
/*******************************************************************************
* Filename:     CompPow.c
*                                                                              
* Purpose:      approximate pow(x, 1/8) using polynomial evaluation.
*                                                                              
* Functions:    powerpoly_125()
*                                                                              
* Author/Date:  Wei Wang, 07/08/97. (powerpoly_125 is modified from RZ's 
*               code in enhance.c of RT29).
********************************************************************************
* Modifications:
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/CompPow.c_v   1.2   02 Apr 1998 14:46:54   weiwang  $
*******************************************************************************/

#include <assert.h>
#include "CompPow.h"
#include "vLibType.h"

#include "xvocver.h"

#define POLY_ORDER_p1  6
#define N_REGION       6

const static float cpfBoundary[N_REGION+1] = {
1.0F, 1.0e-1F, 1.0e-2F, 1.0e-3F, 1.0e-4F, 1.0e-5F, 1.0e-6F
};

/* scale factor is 1.0/Boundary */
const static float cpfScaleFactor[N_REGION] = {
1.0F, 1.0e+1F, 1.0e+2F, 1.0e+3F, 1.0e+4F, 1.0e+5F
};

const static float cpfPowTab_125[N_REGION*POLY_ORDER_p1] = {
#include "pow_125.h"
}; 

/*******************************************************************************
* Function:        powerpoly_125()
*
* Action:         calculate fX^(1/8)
*
* Input:          fX -- floating number which should be between [0 1].
*
* Output:         none
*
* Globals:        none
*
* Return:         fX^(0.125)
********************************************************************************
* Implementation/Detailed Description:
*     - routine is for a 5th order approximation.
*     - polynomial should be :
*         z = y(0).x^5 + y(1).x^4 ... + y(4).x + y(5)
*
*     - x^y is broken into 4 parts accroding to x (x always > 1)
*           poly1 : 1         <= x < BOUNDARY1
*           poly2 : BOUNDARY1 <= x < BOUNDARY2
*           poly3 : BOUNDARY2 <= x < BOUNDARY3
*           poly4 : BOUNDARY3 <= x < BOUNDARYMAX
*
*     - y should be ordered poly1, poly2, poly3, poly4
*
* References:
*     - used Matlab :=> polyfit, polyval
* RZ - 01/17/97
********************************************************************************
* Modifications:  Wei Wang: x is broken into 6 parts. And regenerate the 
*                 coefficients for x.^(1/8).   07/08/97.
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

float powerpoly_125(float fX)
{
   float fRes;
   float *pfTab;
   int i;
   Uint32 *uiBoundary = (Uint32 *)cpfBoundary;
   UnionFltLng   union_x;

   union_x.f = fX;

 
   /***** we can only afford 0 =< x <= 1 *****/
   if (union_x.l <= uiBoundary[6])   /* truncate the limite */
     union_x.l = uiBoundary[6];

   assert(union_x.l <= uiBoundary[0]);  /* make sure number between 0 to 1. */


   /***** find out which region fX falls in *****/
   if (union_x.l > uiBoundary[3]) {
     if (union_x.l > uiBoundary[2]) {
       if (union_x.l > uiBoundary[1]) 
         i = 0;
       else
         i = 1;
     }
     else
       i = 2;
   }
   else {
     if (union_x.l <= uiBoundary[4]) {
       if (union_x.l <= uiBoundary[5]) 
         i = 5;
       else
         i = 4;
     }
     else
       i = 3;
   }

   /***** find out the table *****/
   pfTab = (float *) &(cpfPowTab_125[i*POLY_ORDER_p1]);

   /***** scale-up the input *****/
   union_x.f *= cpfScaleFactor[i];

   /***** evaluate the polynomial in fast way ******/
   fRes  = union_x.f* *pfTab++;
   fRes += *pfTab++;
   fRes *= union_x.f;
   fRes += *pfTab++;
   fRes *= union_x.f;
   fRes += *pfTab++;
   fRes *= union_x.f;
   fRes += *pfTab++;
   fRes *= union_x.f;
   fRes += *pfTab;
   
   return(fRes);
}


