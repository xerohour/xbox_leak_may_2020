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
* Filename:    fLog2.c
*
* Purpose:     approximations to log and inverse log base 2
*
* Functions:   fLog2() and fInvLog2()
*
* Author/Date: Bob Dunn 1/16/97
*
*******************************************************************************
*
* Modifications:  Xiaoqin Sun: speed up for IEEE floating point machine
*                 Wei Wang: clean up the code. And add define IEEE.
*
* Comments:       To speed up the code on other non-IEEE floating point machine,
*                 we just need to know the bits for mantissa and exponent.
*
* Concerns:     (1) Need choose LINEAR_LOG2 or QUAD_LOG2
*               (2) If choose pre-definition IEEE, float and long should be 32 bits
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/Log2.c_v   1.8   07 Apr 1998 17:55:52   weiwang  $
*
******************************************************************************/
#include <assert.h>
#include <math.h>
#include "vLibMath.h"

#include "xvocver.h"

#define LINEAR_LOG2         1
#define QUAD_LOG2           2

#define LOG2_TYPE        LINEAR_LOG2 

/*------------------------------------------------------------------
  If IEEE or WIN32 is defined then use hard-wired fast code to 
    break a floating point number into its exponent and mantissa,
    or to create a floating point number into its exponent and 
    mantissa.
------------------------------------------------------------------*/
#if ((defined(IEEE) == 1) || (defined(WIN32) == 1))

static float VoxfrExp(float fx, int *piExponent);
static float VoxldExp(float fMant, int iExponent);

/*------------------------------------------------------------------
  If IEEE or WIN32 is not defined then use frexp() break a floating 
    point number into its exponent and mantissa and use ldexp() 
    to create a floating point number into its exponent and mantissa.
------------------------------------------------------------------*/
#else

#define VoxfrExp(fx, piExponent)  (float)frexp((double)fx, piExponent)
#define VoxldExp(fMant, iExponent) (float)ldexp((double)fMant, iExponent)

#endif /* else of ((defined(IEEE) == 1) || (defined(WIN32) == 1)) */


/******************************************************************************
  Forward and inverse of the base 2 logarithm using linear interpolation.
    This is less accurate than quadratic interpolation but it is exactly
    invertable because a linear function has an exact linear inverse function.
******************************************************************************/
#if (LOG2_TYPE == LINEAR_LOG2)

/******************************************************************************
*
* function:  float fLog2 (float fx) 
*
* Action:    approximation to log_base_2
*
* Input:     fx -- input number 
*
* Output:    same as return
*
* Globals:   none
*
* Return:    approximation to log_base_2 value
*
*******************************************************************************
*
* Implementation/Detailed Description: 
*
*     The base 2 logarithm is approximated by LINEAR interpolation in
*  region (0.5 <= X < 1.0).  The floating point input number is normalized to 
*  this region by separating it into mantissa and exponent.  The integer
*  portion of the logarithm is the exponent and the fractional portion
*  of the logarithm is approximated by LINEAR interpolation the mantissa.
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
float fLog2 (float fx) 
{
  int    iExponent;
  float  fLogB2;
  float  fMant;

  /*------------------------------------------------------------
    Change fx to fMant * 2**iExponent,  with 0.5 <= fMant < 1.0.
  ------------------------------------------------------------*/
  fMant = (float)VoxfrExp(fx, &iExponent);  

  /*------------------------------------------------------------
    Do linear interpolation.
  ------------------------------------------------------------*/
  fLogB2 = (float)iExponent + 2.0F*(fMant - 1.0F);

  return (fLogB2);
}

/******************************************************************************
*
* Function:  float fInvLog2 (float fLogB2) 
*
* Action:    approximation to 2**log_b2
*
* Input:     fLogB2 -- input number 
*
* Output:    same as return
*
* Globals:   none
*
* Return:    inverse log2 value of fx
*
*******************************************************************************
*
* Implementation/Detailed Description:
*
*     The inverse of the base 2 logarithm is approximated by LINEAR 
*  interpolation in region (0.5 <= X < 1.0).  The integer part of the
*  floating point input number is loaded into the exponent of the result.
*  The fractional part of the input number is LINEARLY interpolated
*  to be in the region (0.5 <= X < 1.0) and is loaded into the 
*  mantissa portion of the result.
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
float fInvLog2 (float fLogB2) 
{
  int   iExponent;
  float fx, fLogMant, fMant;

  /*------------------------------------------------------------
    Find exponent value.
  ------------------------------------------------------------*/
  iExponent = (int)VoxFLOOR(fLogB2)+1;

  /*------------------------------------------------------------
    compute mantissa which between 0.5 to 1.0 
  ------------------------------------------------------------*/
  fLogMant = fLogB2 - iExponent;      /* -1.0 <= fLogMant < 0.0 */
  //assert((fLogMant < 0.0F) && (fLogMant >= -1.0F));

  fMant = 0.5F*fLogMant + 1.0F;       /* 0.5 <= fMant < 1.0 */
  //assert((fMant < 1.0F) && (fMant >= 0.5F));

  /*------------------------------------------------------------
    fx = fMant * 2**iExponent 
  ------------------------------------------------------------*/
  fx = (float)VoxldExp(fMant, iExponent);  
 
  return ( fx );
}

/******************************************************************************
  Forward and inverse of the base 2 logarithm using quadratic interpolation.
    This is more accurate than linear interpolation but it is not exactly
    invertable because the quadratic function does not have an exact 
    quadratic inverse function.
******************************************************************************/
#elif (LOG2_TYPE == QUAD_LOG2) /* else use quadratic interpolation */


/*------------------------------------------------------------
  Constants for quadratic interpolation of the base 2 
    logarithm on the interval (0.5 <= X < 1.0)
------------------------------------------------------------*/
static const float a_logb2 = -1.3594F;
static const float b_logb2 =  4.0391F;
static const float c_logb2 = -2.6797F;
static const float a_ilogb2 = 0.171572F;
static const float b_ilogb2 = 0.671572F;
static const float c_ilogb2 = 1.000000F;
 
/******************************************************************************
*
* function:  float fLog2 (float fx) 
*
* Action:    approximation to log_base_2
*
* Input:     fx -- input number 
*
* Output:    same as return
*
* Globals:   none
*
* Return:    approximation to log_base_2 value
*
*******************************************************************************
*
* Implementation/Detailed Description: 
*
*     The base 2 logarithm is approximated by QUADRATIC interpolation in
*  region (0.5 <= X < 1.0).  The floating point input number is normalized to 
*  this region by separating it into mantissa and exponent.  The integer
*  portion of the logarithm is the exponent and the fractional portion
*  of the logarithm is approximated by QUADRATIC interpolation the mantissa.
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
float fLog2 (float fx) 
{
  int    iExponent;
  float  fLogB2;
  float  fMant;

  /*------------------------------------------------------------
    Change fx to fMant * 2**iExponent,  with 0.5 <= fMant < 1.0.
  ------------------------------------------------------------*/
  fMant = (float)VoxfrExp(fx, &iExponent);  /* 0.5 <= fMant < 1.0 */

  /*------------------------------------------------------------
    Do quadratic approximation.
  ------------------------------------------------------------*/
  fLogB2  =  b_logb2 + a_logb2*fMant;
  fLogB2  =  c_logb2 + fLogB2*fMant;
  fLogB2 += (float)iExponent;

  return (fLogB2);
}

/******************************************************************************
*
* Function:  float fInvLog2 (float fLogB2) 
*
* Action:    approximation to 2**log_b2
*
* Input:     fLogB2 -- input number 
*
* Output:    same as return
*
* Globals:   none
*
* Return:    inverse log2 value of fx
*
*******************************************************************************
*
* Implementation/Detailed Description:
*
*     The inverse of the base 2 logarithm is approximated by QUADRATIC 
*  interpolation in region (0.5 <= X < 1.0).  The integer part of the
*  floating point input number is loaded into the exponent of the result.
*  The fractional part of the input number is QUADRATICLY interpolated
*  to be in the region (0.5 <= X < 1.0) and is loaded into the 
*  mantissa portion of the result.
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
float fInvLog2 (float fLogB2) 
{
  int   iExponent;
  float fx, fLogMant, fMant;

  /*------------------------------------------------------------
    Find exponent value.
  ------------------------------------------------------------*/
  iExponent = (int)VoxFLOOR(fLogB2)+1;

  /*------------------------------------------------------------
    compute mantissa which between 0.5 to 1.0 
  ------------------------------------------------------------*/
  fLogMant = fLogB2 - iExponent;      /* -1.0 < fLogMant < 0.0 */

  /* approximates 2**fLogMant */
  fx = b_ilogb2 + a_ilogb2*fLogMant;
  fMant = c_ilogb2 + fx*fLogMant;
  
  /*------------------------------------------------------------
    fx = fMant * 2**iExponent 
  ------------------------------------------------------------*/
  fx = (float)VoxldExp(fMant, iExponent);      /* fx = fMant * 2**iExponent */

  return ( fx );
}
#endif


#if ((defined(IEEE) == 1) || (defined(WIN32) == 1))

#define  MANT_BITS   23
#define  EXP_BITS    8
#define  EXP_MASK    (0x7F800000L)        /* ((1<<8)-1)<<23 */
#define  EXP_OFFSET  127               /* offset for exponent (2^(8-1)) */
#define  MANT_MASK   (0x007FFFFFL)        /* (1<<23-1) */
#define  EXP_SHIFT_MASK   ((long)EXP_MASK >> MANT_BITS)
#define  SHIFT_EXP_OFFSET  ((long)EXP_OFFSET << MANT_BITS)

typedef union {
  float f;
  long  l;
} UnionFltInt;


/******************************************************************************
*
* Function:  static float VoxfrExp(float fx, int *piExponent)
*
* Action:    find out exponent and mantissa of a floating number
*
* Input:     fx -- input number
*            *piExponent -- address to output exponent.
*
* Output:    *piExponent -- exponent of fx.
*
* Globals:   none
*
* Return:    mantissa of fx
*
*******************************************************************************/

static float VoxfrExp(float fx, int *piExponent)
{
  UnionFltInt ux;
  int iExponent;
  float fMant;

  /**** change the float to 32 bits int */
  ux.f = fx;
  /**** find the exponent by shifting out mantissa bits ****/
  iExponent = (int)((ux.l & EXP_MASK) >> MANT_BITS);
  iExponent -= EXP_OFFSET;
  
  /**** find the mantissa part by masking out the exponent bits ****/
  ux.l= (ux.l & MANT_MASK) | SHIFT_EXP_OFFSET;   /* 1.0 < ux.f < 2.0 */
  fMant = ux.f * 0.5F;   /*    0.5< fMant < 1.0 */
  iExponent ++;        /*   normalize iExponent   */

  *piExponent = iExponent;
  return fMant;
}


/******************************************************************************
*
* Function:  static float VoxldExp(float fMant, int iExponent)
*
* Action:    reconstruct a number from exponent and mantissa.
*
* Input:     fMant -- mantissa
*            iExponent -- exponent
*
* Output:    same as Return
*
* Globals:   none
*
* Return:    reconstructed number from fMant and iExponent.
*
*******************************************************************************/

static float VoxldExp(float fMant, int iExponent)
{
  UnionFltInt ux;
  float fx;
  long lExponent;

  /* reconstruct exponent part */
  iExponent --;
  lExponent = ((long)(iExponent+EXP_OFFSET)<<MANT_BITS) & EXP_MASK;

  /* reconstruct mantissa part */
  ux.f = fMant;
  ux.l = (ux.l & MANT_MASK);

  /* combine exponent and mantissa parts */
  ux.l |= lExponent;

  fx = ux.f;

  return fx;
}
#endif

