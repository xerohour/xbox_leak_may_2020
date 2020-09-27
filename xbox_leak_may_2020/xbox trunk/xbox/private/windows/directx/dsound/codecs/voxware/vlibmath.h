/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*                                                                              *
*******************************************************************************/
 
/*******************************************************************************
*
* File:          vLibMath.h
*
* Purpose:
*
* Functions:
*
* Author/Date:
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/vlibmath.h_v   1.24   22 Apr 1998 10:27:26   weiwang  $
******************************************************************************/
 
#ifndef __VLIBMATH
#define __VLIBMATH

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN64
#undef WIN32
#endif

#include <math.h>


/* Log2.h */
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
******************************************************************************
*
* Concern:   Need choose either 'IEEE' pre-definition or not. If both float and 
*            long are 32 bits and the platform support IEEE floating point
*            format, then choose 'IEEE' to speed up the code.
*          
******************************************************************************/
float fLog2 (float fx);

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
******************************************************************************
*
* Concern:   Need choose either 'IEEE' pre-definition or not. If both float and 
*            long are 32 bits and the platform support IEEE floating point
*            format, then choose 'IEEE' to speed up the code.
*          
******************************************************************************/

float fInvLog2 (float fLogB2);
 
/* nurand.h */

/** 1/(2**31-1): to normalize the random number at U[0,1] **/
#define NURAND_SCALE  4.656612875e-10F 

/******************************************************************************
*
* Function:  long NuRand(long *plSeed)
*
* Action:    Generate a long random number (31-bit unsigned long) from lSeed.
*
* Input:     plSeed -- address to the input seed for random number generator.
*
* Output:    plSeed -- address to the updated seed.
*
* Globals:   none
*
* Return:    New random number in unsigned long (31 bit)
*
*******************************************************************************
*
* Modifications:
*
* Comments:    
*
* Concerns/TBD:  The initial seed must be larger than 0 
*******************************************************************************/
long NuRand(long *plSeed);

/* VoxQR.h */
/******************************************************************************
*
* function:  float fQuadRoot(float fXval)
*
* Action:    approximation fourth root.
*
* Input:     fXvale -- input number 
*
* Output:    same as return
*
* Globals:   none
*
* Return:    approximation to fourth root of fXval
*
*******************************************************************************
*
* Comments:
*
*     WARNING: This code assumes that the input is between 0 and 1.
*              The output is meaningless for inputs greater than 1.
*              If the input is negative an infinite loop results.
*
********************************************************************************/
float fQuadRoot(float fXval);


/* Macros.h */

/*******************************************************************************
*
* Macros for the Pentium
*
*******************************************************************************/
#ifdef WIN32
/*----------------------------------------------------
   VoxROUND2pos() rounds a positive float.  This is 
      assembly code for the Pentium. XF
----------------------------------------------------*/
#define VoxROUND2pos(floatIn, IntegerOut)    \
                     _asm fld floatIn        \
                     _asm fistp IntegerOut   

/*----------------------------------------------------
   VoxROUND2posLong() return long -- same as 
   VoxROUND2pos()
----------------------------------------------------*/
#define VoxROUND2posLong(floatIn, IntegerOut) \
                    VoxROUND2pos(floatIn, IntegerOut) 


/*----------------------------------------------------
   VoxROUND2() rounds a float.  This is 
      assembly code for the Pentium. XF
----------------------------------------------------*/
#define VoxROUND2(floatIn, IntegerOut)    \
                  _asm fld floatIn        \
                  _asm fistp IntegerOut   


/*----------------------------------------------------
   VoxROUNDlong() return long -- same as 
   VoxROUND()
----------------------------------------------------*/
#define VoxROUND2long(floatIn, IntegerOut) \
                   VoxROUND2(floatIn, IntegerOut)

/*----------------------------------------------------
   VoxFLOOR() truncates a float towards minus infinity.  
      This is a compiler dependent macro.  It works with
      cc and gcc on the SUN and on the Pentium.
----------------------------------------------------*/
#define VoxFLOORpos(A) ((int) (A))

#define VoxFLOOR(A)   ((int)((A>=0.0F) ?  A : A-1.0F))

/*----------------------------------------------------
   VoxFLOORlong() -- return long
----------------------------------------------------*/
#define VoxFLOORlong(A)   ((long)((A>=0.0F)? A : A-1.0F)) 

#define VoxFLOORposLong(A) ((long) (A))


#endif /* ifdef WIN32 */

/*******************************************************************************
*
* Macros for the SparcStation
*
*******************************************************************************/
#if (defined(SPARCSTATION) == 1) || (defined(WIN16) == 1) || (defined(FAST_ROUND)==1)

/*----------------------------------------------------
   VoxROUND2pos() rounds a positive float.  This is 
      a compiler dependent macro.  It works with
      cc and gcc on the SUN. BD
----------------------------------------------------*/
#define VoxROUND2pos(A,B) B = ((int) ((A)+0.5F))


/*----------------------------------------------------
   VoxROUND2posLong() return long 
----------------------------------------------------*/
#define VoxROUND2posLong(A, B)  B = ((long) ((A)+0.5F))

/*----------------------------------------------------
   VoxROUND2() rounds a float.  This is 
      a compiler dependent macro.  It works with
      cc and gcc on the SUN. BD
----------------------------------------------------*/
#define VoxROUND2(A,B)  B = (((A)>0.0F) ? (int) ((A)+0.5F) : (int) ((A)-0.5F))

/*----------------------------------------------------
   VoxROUND2long() return long 
----------------------------------------------------*/
#define VoxROUND2long(A, B) \
            B = (((A)>0.0F) ? (long) ((A)+0.5F) : (long) ((A)-0.5F))

/*----------------------------------------------------
   VoxFLOOR() truncates a float towards minus infinity.  
      This is a compiler dependent macro.  It works with
      cc and gcc on the SUN and on the Pentium.
----------------------------------------------------*/
#define VoxFLOORpos(A) ((int) (A))

#define VoxFLOOR(A)   ((int)((A>=0.0F) ? A : A-1.0F))


/*----------------------------------------------------
   VoxFLOORlong() return long 
----------------------------------------------------*/
#define VoxFLOORlong(A)   ((long)((A>=0.0F) ? A : A-1.0F))

#define VoxFLOORposLong(A) ((long) (A))

#endif /* ifndef WIN32 */

/*******************************************************************************
*
* Machine independent macros
*
*******************************************************************************/
#if  (defined(WIN32) == 0) && (defined(SPARCSTATION) == 0) && (defined(WIN16) == 0) \
   && (defined(FAST_ROUND) == 0)

/*----------------------------------------------------
   VoxROUND2pos() rounds a positive float.  This is 
      a machine independent macro.  
----------------------------------------------------*/
#define VoxROUND2pos(A,B) B = ((int) floor((double) ((A)+0.5F)))

/*----------------------------------------------------
   VoxROUND2posLong() returns long
----------------------------------------------------*/
#define VoxROUND2posLong(A,B) B = ((long) floor((double) ((A)+0.5F)))

/*----------------------------------------------------
   VoxROUND2() rounds a float.  This is 
      a machine independent macro.  
----------------------------------------------------*/
#define VoxROUND2(A,B)  B = ( ((A)>0.0F) ?                         \
                              ((int) floor((double) ((A)+0.5F))) : \
                              ((int) ceil((double)  ((A)-0.5F))) )

/*----------------------------------------------------
   VoxROUND2long() returns long
----------------------------------------------------*/
#define VoxROUND2long(A,B)  B = ( ((A)>0.0F) ?                     \
                              ((long) floor((double) ((A)+0.5F))) : \
                              ((long) ceil((double)  ((A)-0.5F))) )


/*----------------------------------------------------
   VoxFLOOR() truncates a float towards minus infinity.  
      This is a machine independent macro.  
----------------------------------------------------*/
#define VoxFLOORpos(A) ((int) (floor( (double) (A))))

#define VoxFLOOR(A) ((int) (floor( (double) (A))))

/*----------------------------------------------------
   VoxFLOORlong() returns long
----------------------------------------------------*/
#define VoxFLOORlong(A) ((long) (floor( (double) (A))))

#define VoxFLOORposLong(A) ((long) (floor( (double) (A))))

#endif

/*----------------------------------------------------
   VoxABS() generates the absolute value of a float.
      This is many times faster than fabs() as it 
      doesn't do type conversions and it is a macro
      instead of a function.
----------------------------------------------------*/
#define VoxABS(A)  (((A)>0.0F) ? (A) : -(A))
#define VoxIABS(A)  (((A)>0) ? (A) : -(A))


#define VoxINTERP(A,B,FACTOR) ((A)+(FACTOR)*((B)-(A)))

#define VoxMAX(A,B) ( ((A)>(B)) ? (A) : (B) )

#define VoxMIN(A,B) ( ((A)>(B)) ? (B) : (A) )       

/*-----------------------------------------------------
  VoxATAN2():
 -----------------------------------------------------*/
#ifdef WIN16
#define VoxATAN2(x, y)    ((x==0.0F) ? (0.0F) : ((float)atan2(x,y)))
#else
#define VoxATAN2(x, y)    ((float)atan2(x,y))
#endif

#ifdef _WIN64
#define WIN32
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VLIBMATH */

