/*M*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1996 Intel Corporation. All Rights Reserved.
//
//      $Workfile: nspfft.h $
//      $Revision: 3 $
//      $Modtime: Dec 16 1996 17:47:06  $
//
//  Purpose: NSP Fourier Transforms 
*M*/
#ifdef __cplusplus
extern "C" {
#endif
#if !defined (_NSPFFT_H) || defined (_OWN_BLDPCS)

    #define _NSPFFT_H

/* ======================================================================== */

    #if !defined (_OWN_BLDPCS)

        #define  NSP_Forw              1
        #define  NSP_Inv               2
        #define  NSP_Init              4
        #define  NSP_Free              8
        #define  NSP_NoScale          16
        #define  NSP_NoBitRev         32
        #define  NSP_InBitRev         64
        #define  NSP_OutBitRev       128
        #define  NSP_OutRCPack       256
        #define  NSP_OutRCPerm       512
        #define  NSP_InRCPack       1024
        #define  NSP_InRCPerm       4096
        #define  NSP_DoIntCore      8192
        #define  NSP_DoFloatCore   16384
        #define  NSP_DoFastMMX     32768

    #endif

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                                  Dft                                     */
/*                                                                          */
/* Compute the forward  or inverse discrete Fourier transform  (DFT)        */
/* of a complex signal.                                                     */
/*                                                                          */
/* ------------------------------------------------------------------------ */

    NSPAPI(void,nspvDft,(const WCplx *inSamps,  WCplx  *outSamps,
                         int    length,   int     flags,
                         int    scaleMode,int    *scaleFactor))
    NSPAPI(void,nspcDft,(const SCplx *inSamps,  SCplx *outSamps,
                         int    length,   int    flags))
    NSPAPI(void,nspzDft,(const DCplx *inSamps,  DCplx *outSamps,
                         int    length,   int    flags))

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                                  Fft                                     */
/*                                                                          */
/* Compute  the forward  or inverse   fast Fourier  transform (FFT)         */
/* of a complex signal.                                                     */
/*                                                                          */
/* ------------------------------------------------------------------------ */

    NSPAPI(void,nspvFft,(WCplx  *samps, int order, int flags,
                         int     scaleMode,        int *scaleFactor))

    NSPAPI(void,nspcFft,(SCplx  *samps, int order, int flags))
    NSPAPI(void,nspzFft,(DCplx  *samps, int order, int flags))


    NSPAPI(void,nspvFftNip,(const WCplx *inSamps,  WCplx  *outSamps,
                            int    order,    int     flags,
                            int    scaleMode,int   *scaleFactor))

    NSPAPI(void,nspcFftNip,(const SCplx *inSamps,  SCplx  *outSamps,
                            int    order,    int     flags))

    NSPAPI(void,nspzFftNip,(const DCplx *inSamps,  DCplx  *outSamps,
                            int    order,    int     flags))


    NSPAPI(void,nspvrFft,(short  *reSamps, short  *imSamps, int order, int flags,
                          int    scaleMode,int    *scaleFactor))

    NSPAPI(void,nspcrFft,(float  *reSamps, float  *imSamps, int order, int flags))

    NSPAPI(void,nspzrFft,(double *reSamps, double *imSamps, int order, int flags))


    NSPAPI(void,nspvrFftNip,(const short *reInSamps,  const short *imInSamps,
                             short *reOutSamps,       short *imOutSamps,
                             int    order,            int    flags,
                             int    scaleMode,        int   *scaleFactor))

    NSPAPI(void,nspcrFftNip,(const float  *reInSamps, const float  *imInSamps,
                             float  *reOutSamps,      float  *imOutSamps,
                             int     order,           int     flags))

    NSPAPI(void,nspzrFftNip,(const double *reInSamps, const double *imInSamps,
                             double *reOutSamps,      double *imOutSamps,
                             int     order,           int     flags))

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                         RealFftl, RealFftlNip                            */
/*                                                                          */
/* Compute the  forward or inverse FFT  of a real signal  using RCPack or   */
/* RCPerm format.                                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */

    NSPAPI(void,nspwRealFftl,(short  *samps, int order, int  flags,
                              int     scaleMode,        int *scaleFactor))

    NSPAPI(void,nspsRealFftl,(float  *samps, int order, int  flags))

    NSPAPI(void,nspdRealFftl,(double *samps, int order, int  flags))


    NSPAPI(void,nspwRealFftlNip,(const short  *inSamps,  short  *outSamps,
                                 int     order,    int     flags,
                                 int     scaleMode,int    *scaleFactor))

    NSPAPI(void,nspsRealFftlNip,(const float  *inSamps,  float  *outSamps,
                                 int     order,    int     flags))

    NSPAPI(void,nspdRealFftlNip,(const double *inSamps,  double *outSamps,
                                 int     order,    int     flags))

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                            CcsFftl, CcsFftlNip                           */
/*                                                                          */
/* Compute the  forward or inverse  FFT of a  complex conjugate-symmetric   */
/* (CCS) signal using RCPack or RCPerm format.                              */
/*                                                                          */
/* ------------------------------------------------------------------------ */

    NSPAPI(void,nspwCcsFftl,(short  *samps,    int  order, int flags,
                             int     scaleMode,int *scaleFactor))

    NSPAPI(void,nspsCcsFftl,(float  *samps,    int  order, int flags))

    NSPAPI(void,nspdCcsFftl,(double *samps,    int  order, int flags))


    NSPAPI(void,nspwCcsFftlNip,(const short *inSamps,  short  *outSamps,
                                int    order,    int     flags,
                                int    scaleMode,int    *scaleFactor))

    NSPAPI(void,nspsCcsFftlNip,(const float  *inSamps, float  *outSamps,
                                int     order,   int     flags))

    NSPAPI(void,nspdCcsFftlNip,(const double *inSamps, double *outSamps,
                                int     order,   int     flags))

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                       MpyRCPack2, MpyRCPack3                             */
/*                                                                          */
/* Multiply two vectors stored in RCPack format.                            */
/*                                                                          */
/* ------------------------------------------------------------------------ */

    NSPAPI(void,nspwMpyRCPack2,(const short *src,  short  *dst, int order,
                                int    scaleMode,         int *scaleFactor))

    NSPAPI(void,nspsMpyRCPack2,(const float  *src, float  *dst, int order))

    NSPAPI(void,nspdMpyRCPack2,(const double *src, double *dst, int order))


    NSPAPI(void,nspwMpyRCPack3,(const short  *srcA,const short  *srcB,short  *dst,
                                int     order,
                                int     scaleMode, int    *scaleFactor))

    NSPAPI(void,nspsMpyRCPack3,(const float  *srcA,const float  *srcB,float  *dst,
                                int     order))

    NSPAPI(void,nspdMpyRCPack3,(const double *srcA,const double *srcB,double *dst,
                                int     order))

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                        MpyRCPerm2, MpyRCPerm3                            */
/*                                                                          */
/* Multiply two vectors stored in RCPerm format.                            */
/*                                                                          */
/* ------------------------------------------------------------------------ */

    NSPAPI(void,nspwMpyRCPerm2,(const short  *src, short  *dst, int order,
                                int     scaleMode,        int *scaleFactor))

    NSPAPI(void,nspsMpyRCPerm2,(const float  *src, float  *dst, int order))

    NSPAPI(void,nspdMpyRCPerm2,(const double *src, double *dst, int order))


    NSPAPI(void,nspwMpyRCPerm3,(const short  *srcA,const short  *srcB,short  *dst,
                                int     order,
                                int     scaleMode, int    *scaleFactor))

    NSPAPI(void,nspsMpyRCPerm3,(const float  *srcA,const float  *srcB,float  *dst,
                                int     order))

    NSPAPI(void,nspdMpyRCPerm3,(const double *srcA,const double *srcB,double *dst,
                                int     order))
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                        RealFft,  RealFftNip                              */
/*                                                                          */
/* Compute the forward or inverse FFT of a real signal.                     */
/*                                                                          */
/* ------------------------------------------------------------------------ */

    NSPAPI(void,nspwRealFft,(short  *samps, int order,  int  flags,
                             int     scaleMode,         int *scaleFactor))

    NSPAPI(void,nspsRealFft,(float  *samps, int order,  int  flags))
    NSPAPI(void,nspdRealFft,(double *samps, int order,  int  flags))


    NSPAPI(void,nspwRealFftNip,(const short  *inSamps,  WCplx *outSamps,
                                int     order,    int    flags,
                                int     scaleMode,int   *scaleFactor))

    NSPAPI(void,nspsRealFftNip,(const float  *inSamps,  SCplx *outSamps,
                                int     order,    int    flags))

    NSPAPI(void,nspdRealFftNip,(const double *inSamps,  DCplx *outSamps,
                                int     order,    int    flags))

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                            CcsFft, CcsFftNip                             */
/*                                                                          */
/* Compute the  forward or inverse  FFT of a  complex conjugate-symmetric   */
/* (CCS) signal.                                                            */
/*                                                                          */
/* ------------------------------------------------------------------------ */

    NSPAPI(void,nspwCcsFft,(short  *samps, int order, int flags,
                            int     scaleMode,        int *scaleFactor))

    NSPAPI(void,nspsCcsFft,(float  *samps, int order, int flags))
    NSPAPI(void,nspdCcsFft,(double *samps, int order, int flags))


    NSPAPI(void,nspwCcsFftNip,(const WCplx *inSamps,  short *outSamps,
                               int    order,    int    flags,
                               int    scaleMode,int   *scaleFactor))

    NSPAPI(void,nspsCcsFftNip,(const SCplx *inSamps, float  *outSamps,
                               int    order,   int     flags))

    NSPAPI(void,nspdCcsFftNip,(const DCplx *inSamps, double *outSamps,
                               int    order,   int     flags))

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                    nsp?Real2Fft, nsp?Real2FftNip                         */
/*                                                                          */
/* Compute the forward or inverse FFT of two real signals.                  */
/*                                                                          */
/* ------------------------------------------------------------------------ */

    NSPAPI(void,nspwReal2Fft,(short *xSamps, short *ySamps, int order, int flags,
                              int scaleMode, int *scaleFactor))

    NSPAPI(void,nspsReal2Fft,(float *xSamps, float *ySamps, int order, int flags))

    NSPAPI(void,nspdReal2Fft,(double *xSamps, double *ySamps, int order, int flags))


    NSPAPI(void,nspwReal2FftNip,(const short *xInSamps, WCplx  *xOutSamps,
                                 const short *yInSamps, WCplx  *yOutSamps,
                                 int order, int flags,
                                 int scaleMode, int *scaleFactor))

    NSPAPI(void,nspsReal2FftNip,(const float *xInSamps, SCplx  *xOutSamps,
                                 const float *yInSamps, SCplx  *yOutSamps,
                                 int order, int flags))

    NSPAPI(void,nspdReal2FftNip,(const double *xInSamps, DCplx *xOutSamps,
                                 const double *yInSamps, DCplx *yOutSamps,
                                 int order, int flags))

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                    nsp?Ccs2Fft, nsp?Ccs2FftNip                           */
/*                                                                          */
/* Compute the forward or reverse  FFT of two complex conjugate-symmetric   */
/* (CCS) signals.                                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */

    NSPAPI(void,nspwCcs2Fft,(short  *xSamps, short  *ySamps, int order,int flags,
                             int     scaleMode, int *scaleFactor))

    NSPAPI(void,nspsCcs2Fft,(float  *xSamps, float  *ySamps, int order,int flags))

    NSPAPI(void,nspdCcs2Fft,(double *xSamps, double *ySamps, int order,int flags))


    NSPAPI(void,nspwCcs2FftNip,(const WCplx *xInSamps, short  *xOutSamps,
                                const WCplx *yInSamps, short  *yOutSamps,
                                int order, int flags,
                                int scaleMode, int *scaleFactor))

    NSPAPI(void,nspsCcs2FftNip,(const SCplx *xInSamps, float  *xOutSamps,
                                const SCplx *yInSamps, float  *yOutSamps,
                                int order, int flags))

    NSPAPI(void,nspdCcs2FftNip,(const DCplx *xInSamps, double *xOutSamps,
                                const DCplx *yInSamps, double *yOutSamps,
                                int order, int flags))

/* ======================================================================== */

#endif                                                         /* _NSPFFT_H */
#ifdef __cplusplus
}
#endif
