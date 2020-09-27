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
* File:         vLibSqPr.h
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
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/vlibsgpr.h_v   1.14   02 Mar 1998 18:23:54   bobd  $
******************************************************************************/
 
#ifndef __VLIBSGPR
#define __VLIBSGPR

#ifdef __cplusplus
extern "C" {
#endif

/* SpecInt.h */
/*******************************************************************************
* Function: SpecIntLn()
*
* Action: Interpolate the harmonic spectrum from one pitch to another using
*         linear interpolation.
*
* Input:  fPitch          - the original pitch lag (time-domain)
*         fPitchInt       - the pitch lag to interpolate the spectrum to.
*         iIntHarms       - the number of harmonics corresponding to fPitchInt
*         pfAmp           - pointer to the original harmonics
*         pfAmpInt        - pointer to a vector to put the interpolated spectrum
*         iMaxHarm        - maximum number of harmonics in fpAmpInt
*
* Output: pfAmpInt        - the interpolated harmonics
*
* Return:  None.
*
* Modifications:
*
* Comments: The pfAmp vector is zero padded up to iMaxHarm for unused
*             harmonics.
*
*******************************************************************************/
void SpecIntLn( float           fPitch,    float           fPitchInt,  
                int             iIntHarms, const float     *pfAmp,  
                float           *pfAmpInt,  unsigned int    iMaxHarm);


/* HWin.h */
/******************************************************************************
*
* Function:   HammingWindowNormSC()
*
* Action:     Window a signal with a Hamming window.  This window was suppose
*              to have unit area, but the area is really 1.08.  The function
*              is kept around because SC36 codebooks are currently trained
*              on it.
*
* Input:      float *pfX      the input signal
*             int   iLength   the length of the input signal
*
* Output:     float *pfX      the output signal
*
* Globals:    none
*
* Return:     none
******************************************************************************/
void HammingWindowNormSC( float *pfX, int iLength );

/******************************************************************************
*
* Function:   HammingWindowNorm()
*
* Action:     Window a signal with a Hamming window that has unit area.
*
* Input:      float *pfX      the input signal
*             int   iLength   the length of the input signal
*
* Output:     float *pfX      the output signal
*
* Globals:    none
*
* Return:     none
******************************************************************************/
void HammingWindowNorm( float *pfX, int iLength );

/******************************************************************************
*
* Function:   HanningWindowNorm()
*
* Action:     Window a signal with a Hanning window that has unit area.
*
* Input:      float *pfX      the input signal
*             int   iLength   the length of the input signal
*
* Output:     float *pfX      the output signal
*
* Globals:    none
*
* Return:     none
******************************************************************************
*
* Concerns:   The first and the last samples are zeros
*
******************************************************************************/
void HanningWindowNorm( float *pfX, int iLength ); 



/******************************************************************************
*
* Function:   WindowKernel()
*
* Action:     Window a signal (Hamming or Hanning)
*
* Input:      float *x        input signal
*             int   length    length of signal
*             float a         window constant A
*             float b         window constant B
*
* Output:     float *x        output windowed signal
*
* Globals:    none
*
* Return:     none
******************************************************************************/
void WindowKernel(float *x, int length, float a, float b);


/******************************************************************************
*
* Function:  KaiserWindow()
*
* Action:    Compute the Kaiser window coefficients. Only return left side
*            of window.
*
* Input:     fWindow -- pointer for Kaiser window
*            iWindowSize -- the length of full size window
*            fBeta -- the beta factor for Kaiser window.
*
* Output:    fWindow -- the left half side of Kaiser window.
*
* Globals:   none.
*
* Return:    none.
*
*******************************************************************************
*
* Modifications:
*
* Comments: All the normalization should call another function : 
*           NormSymWindowByPower() or NormSymWindowByArea().
*
* Concerns/TBD:
******************************************************************************/ 
void KaiserWindow(float *fWindow, int iWindowSize, float fBeta);

/******************************************************************************
*
* Function:  InitKaiserWindow()
*
* Action:    Allocate memory and compute the Kaiser window coefficients.
*            !!! The array *pfWindow only has the half of the window length.
*
* Input:     pfWindow -- pointer to Kaiser window buffer.
*            iWindowSize -- the length of full size window
*            fBeta -- the beta factor for Kaiser window.
*
* Output:    pfWindow -- the return address of the left side of Kaiser window.
*
* Globals:   none.
*
* Return:    1 : fail to allocate the memory.
*            0:  succeed.
*
*******************************************************************************
* Modifications:
*
* Comments: All the normalization should call another function : 
*           NormSymWindowByPower() or NormSymWindowByArea().
*
* Concerns/TBD:
******************************************************************************/
unsigned short InitKaiserWindow(float **pfWindow, int iWindowSize, float fBeta);

/******************************************************************************
*
* Function:  FreeKaiserWindow()
*
* Action:    Free the Kaiser window memory.
*
* Input:     pfWindow -- pointer to Kaiser window buffer.
*
* Output:    
*
* Globals:   none.
*
* Return:    none
*******************************************************************************/
void FreeKaiserWindow(float **pfWindow);

/* SymWinSp.h */
/*******************************************************************************
* 
* Function: SymWindowSignal()
*
* Action: considering the symmetricity of window, the signal is windowed by 
*         only the half size of window coefficients.
*
* Input:  pIn:    input signal.
*         pWin:   window data (only left half size == ((length+1)>>1)).
*         length: data length.
*
* Output: pOut:   output signal.
*
* Return: None.
*******************************************************************************
* Modifications:
*
* Comments: In-place operation is supported.
*
* Concerns/TBD:
******************************************************************************/
void SymWindowSignal(const float *pIn, const float *pWin, float *pOut,
                      int length);


/* NormWin.h */
/*******************************************************************************
* 
* Function: NormSymWindowByPower()
*
* Action: Normalize the symmetric window by power
*
* Input:  pfWindow: non-normalized window data (either left half or whole size).
*         iWindowSize : the length of window.
*
* Output: pfWindow: normalized window data.
*
* return: None.
*
******************************************************************************/
void NormSymWindowByPower(float *pfWindow, int iWindowSize);

/*******************************************************************************
* 
* Function: NormSymWindowByArea()
*
* Action: Normalize the symmetric window by area function.
*
* Input:  pfWindow: non-normalized window data (either left half or whole size).
*         iWindowSize : the length of window.
*
* Output: pfWindow: normalized window data.
*
* return: None.
*******************************************************************************/
void NormSymWindowByArea(float *pfWindow, int iWindowSize);


#ifdef __cplusplus
}
#endif

#endif /* __VLIBSGPR */
