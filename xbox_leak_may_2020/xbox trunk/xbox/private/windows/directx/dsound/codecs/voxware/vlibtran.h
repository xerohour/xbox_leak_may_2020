/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/*******************************************************************************
*
* File:          vLibTran.h
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
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/vlibtran.h_v   1.36   21 Apr 1998 13:10:22   bobd  $
******************************************************************************/
 
#ifndef __VLIBTRAN
#define __VLIBTRAN

#ifdef __cplusplus
extern "C" {
#endif


/* as2lspe.c */
/*******************************************************************************
* Function:  VoxAsToLspEven()
*
* Action:    Compute LSPs from predictor coefficients for an even ordered
*              all-pole model.
*
* Input:     float *pfAs          --> predictor coefficients
*            int    iOrder        --> all-pole model order
*            float  fScaleFactor  --> scale factor to apply to LSPs (if the
*                                       factor is 1.0, the LSPs range from
*                                       0 to 0.5).
*
* Output:    float *pfLSP         --> LSPs
*
* Globals:   none
*
* Return:    int        (0/1) (success/fail)
********************************************************************************
* Modifications:
*
* Comments:    The inverse filter is defined as:
*
*                      A(Z) = 1 + SUM(i=1...p) A_i Z^(-i)
*
*              The LSPs are normalized in frequency and range
*                from 0 to 0.5.  Since many codecs require LSPs in
*                Hertz, fScaleFactor is included to allow the conversion
*                to Hertz.  If fScaleFactor is the sampling rate, the
*                LPSs will be in Hertz.  If fScaleFactor is 1.0 the LPSs
*                will range from 0 to 0.5.
*
* Concerns/TBD:
*******************************************************************************/
int VoxAsToLspEven( float *pfAs, int iOrder, float fScaleFactor, float *pfLSP );

/* lsp2ase.h */
/*******************************************************************************
* Function:       void VoxLspToAsEven()
*
* Action:         Compute order predictor coefficients from LSPs for
*                   even ordered all-pole models.
*
* Input:          const float *pfLSP        -> LSPs
*                 int          iOrder       -> all-pole model order
*                 float        fScaleFactor -> scale factor to apply to LSPs
*                                               (when multiplied by fScaleFactor
*                                               the LSPs should range from
*                                               0 to 0.5).
*
* Output:         float       *pfAs         -> 10th order predictor coefficients
*                                                where pfAs[0] = A_0 = 1.0
*
* Globals:        none
*
* Return:         void
********************************************************************************
* Modifications:
*
* Comments:    The inverse filter is defined as:
*
*                      A(Z) = 1 + SUM(1...p) A_p Z^(-p)
*
*              The LSPs are normalized in frequency and should range
*                from 0 to 0.5.  Since many codecs require LSPs in
*                Hertz, fScaleFactor is included to allow the conversion
*                from Hertz to normalized frequency.  If the input LSPs are
*                in Hertz, fScaleFactor should be set to one over the
*                sampling rate.  If the LSPs are normalized in frequency
*                (ranging from 0 to 0.5) fScaleFactor should be set to 1.0.
*
* Concerns/TBD:
*******************************************************************************/
void VoxLspToAsEven( const float *pfLSP, int iOrder,
                     float fScaleFactor, float *pfAs );

/* AsToCs.h */
/******************************************************************************
*
* Function:  asToCs()
*
* Action:    Convert the prediction coefficients to cepstrum coefficients
*
* Input:     fGain       -- residual energy in the log domain
*            pfAs        -- prediction coefficients in the format [1 a1 a2 ...]
*            iOrder_As   -- the order of prediction coefficients
*                             (or the length of pfAs plus 1).
*            pfCs        -- the pointer to the cepstrum coefficients.
*            iLength_Cs  -- the length of pfCs (or the order of cepstrum
*                             coefficients plus 1).
*
* Output:    pfCs     -- the output cepstrum coefficients
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Comments:     The format for pfAs[] is:
*                  A(z) = 1 + SUM(1...p) a_p z^(-p) where pfAs[0]=1.
*
******************************************************************************/
void VoxAsToCs( const float fGain, const float *pfAs, const int iOrder_As,
                float *pfCs, const int iLength_Cs);

/* AsToKs.c */
/******************************************************************************
*
* Function:  AsToKs()
*
* Action:    Convert the prediction coefficients (LPC) to
*              reflection coefficients.
*
* Input:     pfAS[] -> predictor coefficients
*            iOrder -> all pole model order
*
* Output:    pfKS[] -> reflection coefficients
*
* Globals:   none
*
* Return:    iClipFlag (1 for clipped pfKS[], 0 otherwise)
*******************************************************************************
*
* Comments:     This recursion for computing the reflection coefficients
*                 can become unstable in single precision floating point
*                 when the magnitude of a reflection coefficient gets close
*                 to 1.  If this happens, the magnitude of one or more
*                 reflection coefficients will be set to
*                 KS_OVERFLOW_THRESHOLD and iClipFlag will be set to 1.
*
*               The format for pfAs[] is:
*                  A(z) = 1 + SUM(1...p) a_p z^(-p) where pfAs[0]=1.
*
* Concerns/TBD: (1) Need scratch memory at least 2*MAX_LPC_ORDER-1
******************************************************************************/
int AsToKs( const float *pfAS, float *pfKS, int iOrder);

/* CsToEnv.h */
/******************************************************************************
*
* Function:  VoxCsToEnv()
*
* Action:    Compute the cepstral log-amplitude and phase envelopes (using
*            interleave-formated FFT)
*
* Input:     float *pfCs           --> Cepstral coefficients
*            int iOrderCepstral    --> number of Cepstral coefficients
*            int loglfft           --> Log (base 2) of FFT length
*
* Output:    float *pfSW           --> interleaved output array. The real part
*                                          (odd indices) is the Log (base 2)
*                                          magnitude of spectrum (0 to lfft/2)
*                                          The imaginary part (even indices)
*                                          is the unwrapped phase (in radians)
*                                          of spectrum (0 to lfft/2). The length
*                                          of *pfSW must be (1<<loglfft)+2).
*
* Globals:   none
*
* Return:    void
*******************************************************************************/
void VoxCsToEnv( const float *pfCs,  int    iOrderCepstral, 
                 int    loglfft,     float       *pfSW);


/* DeMltplx.h */
/*******************************************************************************
*
* Function:  VoxDeMultiplex()
*
* Action:    Demultiplex the real and imaginary part of the spectrum
*
* Input:     const float *pfSW    --> interleaved spectrum
*            int          iLength --> length of output arrays
*
* Output:    float *pfRE          --> real part of spectrum
*            float *pfIM          --> imaginary part of spectrum
*
* Globals:   none
*
* Return:    void
*******************************************************************************/
void VoxDemultiplex( const float *pfSW, float *pfRE, float *pfIM, int iLength);

/* fft_cent.h */
/******************************************************************************
*
* Function:  VoxFftCenter()
*
* Action:    Compute the FFT of a REAL signal with the signal centered at
*              FFT sample 0.  Calculate the real FFT using inter-leaved
*              formated FFT.
*
* Input:     pfSig     -- input signal buffer
*            iLength   -- the length of the input signal
*            pfSW      -- pointer to the output spectrum (need length N+2)
*            iFFTorder -- the order of FFT
*
* Output:    pfSW      -- output spectrum (inter-leaved)
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Concerns: If pfSig and pfSW share the same buffer, their lengths must
*           satisfy the condition:  iNfft - iLength >= (iLength>>1)
******************************************************************************/
void VoxFftCenter( const float *pfSig, int          iLength,
                   float *       pfSW, int           iFFTorder);

/* FHTkrnl.h */
/******************************************************************************
*
* Function:  VoxFHTkrnl()
*
* Action:    A fast harmonic sum of sinusoids algorithm (the fastest 
*            known to mankind as of 10/22/96 :-) ).
*            Returns a single pitch cycle of speech
*
* Input:     pfAmps       - array of harmonic amplitudes
*            piPhaseIndex - array of harmonic phase indexes (indexes into
*                             pfSinTbl[] and pfCosTbl[])
*            iHarm        - number of harmonics for synthesis
*            pfSig        - pointer to buffer for speech accumulation
*            iFFTorder    - the FFT order for FHT
*            pfFHTWeights - table of FHT weights
*            pfSinTbl     - pointer to sine table
*            pfCosTbl     - pointer to cosine table
*
* Output:    pfSig        - single pitch cycle of speech.
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Modifications: remove scaling factor at end
*
* Comments:
*
* Concerns/TBD: For speed, this routine currently uses table 
*               lookup for the sine and cosine of the phase.
*               The table size is variable and the phase indexes
*               are assumed to be within the table's limits.
*
*               For inter-leaved formatted inverse FFT, the size for pfSig has 
*               to be (1<<iFFTorder)+2.
*
******************************************************************************/

void VoxFHTkrnl( const float *pfAmps, 
                 const int   *piPhaseIndex, 
                 int   iHarm,    
                 float   *pfSig, 
                 int   iFFTorder, 
                 const float *pfFHTWeights,
                 const float *pfSinTbl,
                 const float *pfCosTbl);

/* K2ASK.h */
/******************************************************************************
*
* Function:  K2ASK()
*
* Action:    Convert reflection coefficients to Arcsin reflection coefficients.
*
* Input:     iLPCorder -- the order or the length of the coefficients.
*            pfk       -- reflection coefficients.
*            pfASK     -- the pointer for the output Arcsin reflection 
*                           coefficients.
*
* Output:    pfASK     -- Arcsin reflection coefficients 
*
* Globals:   none
*
* Return:    void
*******************************************************************************/
void K2ASK (int iLPCorder, const float *pfk, float *pfASK);


/******************************************************************************
*
* Function:  ASK2K()
*
* Action:    Convert Arcsin reflection coefficients to reflection coefficients.
*
* Input:     iLPCorder -- the order or the length of the coefficients.
*            pfASK     -- Arcsin reflection coefficients.
*            pfk       -- the pointer for the output reflection coefficients.
*
* Output:    pfk       -- reflection coefficients 
*
* Globals:   none
*
* Return:    void
*******************************************************************************/
void ASK2K (int iLPCorder, const float *pfASK, float *pfk);


/* KsToAs.h */
/******************************************************************************
*
* Function:  VoxKsToAs()
*
* Action:    Convert the reflection coefficients to prediction
*              coefficients (LPC).
*
* Input:     pfKS        -- the input reflection coefficients.
*            pfAS        -- the pointer for the output LPC coefficients.
*            iOrder      -- the order of both reflection coefficients and LPCs.
*
* Output:    pfAS     -- the output prediction coefficients.
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Comments:     The format for pfAs[] is:
*                  A(z) = 1 + SUM(1...p) a_p z^(-p) where pfAs[0]=1.
*
* Concerns/TBD: (1) Need scratch memory at least MAX_LPC_ORDER+1 (19).
*
******************************************************************************/
void VoxKsToAs( const float *pfKS, float *pfAS, int iOrder);

/* RealFFT.h */

#ifndef WIN32

/******************************************************************************
*
* Function:  RealFft()
*
* Action:    Fast-conversion of real signal to conjugate-symmetric spectrum.
*            The output is inter-leaved.
*
*            float *pfInOut        --> pfInOut real signal/output complex signal
*            int    log2N          --> the order for FFT
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Modifications:
*
**          - This is the umbrella function. It calls out the other functions
**          that do the actual work.
**          - Frequency domain signal is interleaved, and has 2 extra locations.
**          See data format discussion above for more info.
**          - "Packing" algorithm, based on manipulating the output of
**          a complex FFT of half the size of the real FFT.
**          Not the most efficient algorithm, but good by most standards.
* Comments:  pfInOut length N and output length N+2.
*
* Concerns/TBD: 
******************************************************************************/
void RealFft(float *pfInOut, int log2N);

/******************************************************************************
*
* Function:  RealInverseFft()
*
* Action:    Fast-conversion of conjugate-symmetric spectrum to real signal.
*            The pfInOut is inter-leaved.
*
*            float *pfInOut        --> pfInOut conjugate-symmetric spectrum
*                                      output real time signal
*            int    log2N          --> the order for FFT
*            int    iFlags         --> controls special functions
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Modifications:
*
**          - This is the umbrella function. It calls out the other functions
**          that do the actual work.
**          - Frequency domain signal is interleaved, and has 2 extra locations.
**          See data format discussion above for more info.
**          - "Packing" algorithm, based on manipulating the output of
**          a complex FFT of half the size of the real FFT.
**          Not the most efficient algorithm, but good by most standards.
*
* Comments:  pfInOut length N+2 and output length N.
*
* Concerns/TBD: 
******************************************************************************/
void RealInverseFft(float *pfInOut, int log2N, int iFlags);
#endif

#ifdef WIN32
typedef struct _SCplx 
{
    float        re;
    float        im;
} SCplx;

#define NSP_Forw         1
#define NSP_Inv          2
#define NSP_Init         4
#define NSP_NoScale      8
#define NSP_NoBitRev    16
#define NSP_InBitRev    32
#define NSP_OutBitRev   64
#define NSP_OutRCPack  128
#define NSP_OutRCPerm  256
#define NSP_InRCPack   512
#define NSP_InRCPerm  1024
#define NSP_RADIX     4096

#define  FFT_OrderSplit          11       /* max order for split algorithm  */

void vox_nspsRealFft(float *samps,int order, int flags);
void vox_nspsCcsFft(float *samps,int order, int flags);
void vox_nspcFft( SCplx *samps, int order,int flags);

void nspFreeFftResources();

#define P5_OPT    0
#endif

/***********************************************************************
*
* Macros to combine the RealFft(), RealInverseFft and NSP FFTs 
*
***********************************************************************/
#ifdef WIN32
#define VoxRealFft(pfInOut, log2N)   vox_nspsRealFft(pfInOut, log2N, NSP_Forw)
#define VoxRealInverseFft(pfInOut, log2N, flags)  \
                                     vox_nspsCcsFft(pfInOut, log2N, flags)
#define IFFT_NO_SCALE_FLAG            NSP_NoScale  /* 8 */
#define IFFT_SCALE_FLAG               NSP_Inv      /* 2 */

#else
#define VoxRealFft(pfInOut, log2N)   RealFft(pfInOut, log2N)
#define VoxRealInverseFft(pfInOut, log2N, flags)   \
                                     RealInverseFft(pfInOut, log2N, flags)
#define IFFT_NO_SCALE_FLAG            8      /* consistent with NSP library */
#define IFFT_SCALE_FLAG               2      /* consistent with NSP library */
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VLIBTRAN */


