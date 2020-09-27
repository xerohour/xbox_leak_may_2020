/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
*
* Filename:   nurand.c
*
* Purpose:    Compute the random number in long (31 bit).
*
* Functions:  long NuRand(long *plSeed)
*
* Author/Date: Rob Zopf, 1996.
*
*******************************************************************************
*
* Modifications: 
*
* Comments:      
*
* Concerns:   The initial seed must be larger than 0 
*
* $Header:   G:/pvcsproj/voxlib.prj/nurand.c_v   1.6   10 Mar 1998 16:29:32   weiwang  $
******************************************************************************/

#include <stdlib.h>
#include <assert.h>
#include "vLibMath.h"

#include "xvocver.h"

/******************************************************************************
*
* Function:  long NuRand(long *plSeed)
*
* Action:    Generate a long random number (31-bit long) from lSeed.
*
* Input:     plSeed -- address to the input seed for random number generator.
*
* Output:    plSeed -- address to the updated seed.
*
* Globals:   none
*
* Return:    New random number in long (31 bit)
*
*******************************************************************************
*
* Implementation/Detailed Description:
*     The sequence used is x' = (A*x) mod M,  (A = 16807, M = 2^31 - 1).  
*     This is the "minimal standard" generator from CACM Oct 1988, p. 1192.
*     The implementation is based on an algorithm using 2 31-bit registers 
*     to represent the product (A*x), from CACM Jan 1990, p. 87.           
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:    
*
* Concerns/TBD:  The initial seed must be larger than 0 
******************************************************************************/

long NuRand(long *plSeed)
{
  const long A = 16807;
  const unsigned long LONG_MASK = 0xFFFFFFFFL;
  unsigned long x0 = ((*plSeed<< 16)&LONG_MASK)>>16;  /* 16 LSBs OF SEED  */
  unsigned long x1 = *plSeed >> 16;                  /* 16 MSBs OF SEED   */
  unsigned long p, q;                                /* MSW, LSW OF PRODUCT */

  assert(plSeed != NULL);
  assert(*plSeed != 0);

  /*---------------------------------------------------------------------*/
  /* COMPUTE THE PRODUCT (A * lSeed) USING CROSS MULTIPLICATION OF        */
  /* 16-BIT HALVES OF THE INPUT VALUES.  THE RESULT IS REPRESENTED AS 2  */
  /* 31-BIT VALUES.  SINCE 'A' FITS IN 15 BITS, ITS UPPER HALF CAN BE    */
  /* DISREGARDED.  USING THE NOTATION val[m::n] TO MEAN "BITS n THROUGH  */
  /* m OF val", THE PRODUCT IS COMPUTED AS:                              */
  /*   q = (A * x)[0::30]  = ((A * x1)[0::14] << 16) + (A * x0)[0::30]   */
  /*   p = (A * x)[31::60] =  (A * x1)[15::30]       + (A * x0)[31]  + C */
  /* WHERE C = q[31] (CARRY BIT FROM q).  NOTE THAT BECAUSE A < 2^15,    */
  /* (A * x0)[31] IS ALWAYS 0.                                           */
  /*---------------------------------------------------------------------*/
  q = ((((A * x1) << 17) & LONG_MASK) >> 1) + (A * x0);
  q = q & LONG_MASK;
  p = ((A * x1) >> 15) + (q >> 31);
  p = p & LONG_MASK;
  q = ((q << 1) & LONG_MASK) >> 1;             /* CLEAR CARRY */

  /*---------------------------------------------------------------------*/
  /* IF (p + q) < 2^31, RESULT IS (p + q).  OTHERWISE, RESULT IS         */
  /* (p + q) - 2^31 + 1.                                                 */
  /*---------------------------------------------------------------------*/
  p += q;
  p = p & LONG_MASK;

  /* ADD CARRY, THEN CLEAR IT */
  *plSeed = (((p + (p >> 31)) << 1) & LONG_MASK) >> 1;

  return *plSeed;
}
