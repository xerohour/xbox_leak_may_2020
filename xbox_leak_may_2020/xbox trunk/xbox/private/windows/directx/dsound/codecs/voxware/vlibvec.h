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
* File:         vLibVec.h
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
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/vlibvec.h_v   1.29   07 Apr 1998 18:02:18   weiwang  $
******************************************************************************/
 
#ifndef __VLIBVEC
#define __VLIBVEC

#ifdef __cplusplus
extern "C" {
#endif

/* AutoCor.h */
/*******************************************************************************
*
* Function:  AutoCor()
*
* Action:    Compute autocorrelation sequence.
*
* Input:     int          iOrder   -> number of autocorrelation samples to
*                                       compute
*            const float *pcfVecIn -> input sequence
*            int          iLength  -> length of input sequence
*
* Output:    float *pfVecOut       -> the scaled vector
*
* Globals:   none
*
* Return:    void
*******************************************************************************/
void AutoCor( int iOrder, const float *pcfVecIn, int iLength, float *pfVecOut);

/* DotProd.h" */
/******************************************************************************
*
* Function:  DotProd()
*
* Action:    Calculate the inner product of two vector
*
* Input:     const float *pfVecA  -> 1st input vector
*            const float *pfVecA  -> 2nd input vector
*            int iLength          -> the length of input vectors
*
* Globals:   none
*
* Return:    float fDotProduct    -> the inner product
*******************************************************************************/
float DotProd(const float *pcfVecA, const float *pcfVecB, int iLength);

/* FindMax.h */
/*******************************************************************************
*
* Function:  FindMaxValueFlt()
*
* Action:    Finds the maximum value (and its location) of a floating 
*              point vector.
*
* Input:     *pfX  -> the vector to be scaled
*            iLength -> the length of the vector
*
* Output:    *piMaxIndex -> index of maximum value
*            *pfXmax     -> maximum value
*
* Globals:   none
*
* Return:    void
********************************************************************************
* Modifications: Version 2.0, Originated from Intel NSP Lib
*                Modified by Xiangdong Fu 12/11/96
*
*
* Concerns:    This version is an optimized version on Pentium or IEEE 
*              floating point format machine. It assumes interger
*              comparision is faster than floating point comparision. 
*              This version is 3 times faster than version  1.0 on Pentium.
*              It is proved to work on Sun as well, but not faster(or slower)
*
*              Only float point and long are 32 bits, choose 'IEEE' pre-
*              definition.
*          
* Comments:    Input vector length >= 1
*
*******************************************************************************/
void FindMaxValueFlt( const float *pfX, int         iLength, 
                      int  *piMaxIndex, float       *pfXmax);


/* L2Scale.h */
/******************************************************************************
*
* Function:  L2Scale()
*
* Action:    Perform an L2 scaling of a vector.
*
* Input:     float *pfX       -> the vector to be scaled
*            int iLength      -> the length of the vector
*            float fEnergySqr -> the desired output energy of the vector
*
* Output:    float *pfX       -> the scaled vector
*
* Globals:   none
*
* Return:    void
******************************************************************************/
void L2Scale(float *pfX, int iLength, float fEnergySqr);

/* MagSq.h */
/*******************************************************************************
*
* Function: MagSq()
*
* Action:   Compute magnitude squared of a complex vector
*
* Input:    const float *pcfRe    -> real part of complex vector
*           const float *pcfIm    -> imaginary part of complex vector
*           int    iLength        -> length of vector
*
* Output:   float *pfMagSq        -> magnitude squared of the vector
*
* Globals:  none
*
* Return:   void
*******************************************************************************/
void MagSq(const float *pcfRe, const float *pcfIm, int iLength, float *pfMagSq);


/******************************************************************************
*
* Function: MagSqIL()
*
* Action:   Compute magnitude squared of a inter-leave formatted complex vector
*
* Input:    const float *pcfVec    -> inter-leave complex vector
*           int    iLength        -> length of vector
*
* Output:   float *pfMagSq        -> magnitude squared of the vector
*
* Globals:  none
*
* Return:   void
******************************************************************************/
void MagSqIL(const float *pcfVec, int iLength, float *pfMagSq);


/* oladd.h */
/*******************************************************************************
*
* Function:  OlAdd()
*
* Action:    a generalized trapezoidal overlap-add routine.
*
* Input:     const float *pfFadeOutBuf -> fade-out input buffer
*            const float *pfFadeInBuf  -> fade-in input buffer
*            int    iOverlapLen  -> length of overlap
*            int    iFrameLen    -> length of frame
*
* Output:    float       *pfOutBuf     -> output buffer
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
*           iFrameLen
* /-------------------------------------\
*               iOverlapLen
*           /-----------------\
*
* ----------\__             __/
*              \__       __/
*                 \__ __/
*                  __X__
*               __/     \__
*            __/           \__
*           /                 \----------
*
*   pfFadeOutBuf
* |---------------------------|
*
*             pfFadeInBuf
*           |---------------------------|
*
********************************************************************************
*
* Modifications: Removed some variables and changed type (short) in call
*                to type (int). Bod Dunn 7/24/97.
*
* Comments: IMPORTANT: this routine ACCUMULATES into the output buffer.  If
*                      needed, the output buffer should be set to zero before
*                      calling this routine.
*
* Concerns/TBD:
*******************************************************************************/
void OlAdd ( const float *pfFadeOutBuf, const float *pfFadeInBuf,
             int iOverlapLen, int iFrameLen, float *pfOutBuf );

/* ScaleVec.c */
/******************************************************************************
*
* Function:  ScaleVector()
*
* Action:    Multiply a vector by a scalar.
*
* Input:     float *pfIn      -> the vector to be scaled
*            int    iLength   -> the length of the vector
*            float  fScalar   -> the desired scale factor
*
* Output:    float *pfOut     -> the scaled vector
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Modifications:
*
* Comments:  In place and out of place operation are supported.
*
* Concerns/TBD:
******************************************************************************/
void ScaleVector( float *pfIn, int iLength, float fScalar, float *pfOut );


#ifdef __cplusplus
}
#endif

#endif /* __VLIBVEC */



