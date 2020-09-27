/*M*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1999 Intel Corporation. All Rights Reserved.
//
//  Purpose: Declarations of the required functions
//   
*M*/

/***********************************************************/
/* Only copy the function prototype of all the             */
/* functions you want from the release include files       */
/***********************************************************/
#ifdef __USERLIST__

// Used by audio encoder and decoder
#if defined(INTELFFT_AUDIO)
    NSPAPI(void,nspcFft,     (SCplx  *samps, int order, int flags))
#endif  // INTELFFT_AUDIO

// Used by audio encoder and speech (encoder and decoder)
#if (defined(INTELFFT_AUDIO) && defined(INTELFFT_ENCODER)) || defined(INTELFFT_SPEECH)
    NSPAPI(void,nspsCcsFft,  (float  *samps, int order, int flags))
#endif  // (defined(INTELFFT_AUDIO) && defined(INTELFFT_ENCODER)) || defined(INTELFFT_SPEECH)

// Used by speech (encoder and decoder)
#if defined(INTELFFT_SPEECH)
    NSPAPI(void,nspsRealFft, (float  *samps, int order,  int  flags))
    NSPAPI(void,nspsDct,     (const float* src, float* dst, int len, int flags))
#endif  // INTELFFT_SPEECH

#endif /* __USERLIST__ */
