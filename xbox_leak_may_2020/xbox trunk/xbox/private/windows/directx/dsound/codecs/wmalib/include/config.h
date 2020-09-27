#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else 
#define EXTERN_C extern 
#endif
#endif

// The XxxGetXxxXxx funstions below are used to enumerate supported parameter
// combinations.  ulIndex is a 0-based index, the funtions return FALSE if it
// is out of range.
//
// The XxxCheckXxxXxx functions return TRUE if the parameter combination is
// supported, FALSE otherwise.

typedef struct _WMAFormatInfo {
    // base WAVEFORMATEX
    U32 nSamplesPerSec;
    U32 nChannels;
    U32 nAvgBytesPerSec;
    U32 nBlockAlign;
#if defined (WMA_V9)
    U32 nBytePerSample;
    U16 nValidBitsPerSample;    /* bits of precision */
    U32 nChannelMask;         /* which channels are present in stream */
#endif // WMA_V9
    // extended WAVEFORMATES
    U32 nSamplesPerBlock;
    U32 dwSuperBlockAlign;
    U16 wEncodeOptions;

    // miscellaneous
    U32 nFramesPerPacket;
    U32 nSamplesPerFrame;
    U32 nMaxSamplesPerPacket;
    U32 nLookaheadSamples;
    U32 nSuperFrameSamples; // useless ?
    U32 ulOfficialBitrate;
} WMAFormatInfo;

#ifdef __cplusplus
extern "C" {
#endif

   // Checks if the codec can encode this PCM at all (at at least some bitrate)
Bool CheckPCMParams(U32 ulSamplingRate, U32 ulChannels);
// Enumerate all of the channels / sampling combinations supported at any bitrate
Bool GetPCMParamsByIndex(U32 ulIndex, U32 *pulSamplingRate, U32 *pulChannels);


//
// For the GetXxxXxx functions, the entire WMAFormatInfo is an output parameter.
// For CheckFormat, the WAVEFORMATEX stuff is input and the miscellaneous fields
// at the end are output.
//

// Enumerates all of the output formats, regardless if input.  Normal formats
// are enumerated first, followed by the special ASF AV Interleave versions of
// formats that cannot be interleaved in their normal form.
Bool GetFormatByIndex(U32 ulIndex, Bool fIncludeASFAVInterleaveSpecial, WMAFormatInfo* pFormat);
// Total number of formants enumerated by GetFormatByIndex
U32 TotalFormatCount(Bool fIncludeASFAVInterleaveSpecial);

// This is for when you know the input sampling rate and the number of channels
// and want to enumerate all possible output formats.  Normal formats are
// enumerated first, followed by the special ASF AV Interleave versions.
Bool GetFormatByPCMParamsAndIndex(U32 ulIndex, U32 ulSamplingRate, U32 ulChannels, Bool fIncludeASFAVInterleaveSpecial, WMAFormatInfo* pFormat);

// If you know the sampling rate / channels AND the approximate bitrate you
// want, call this to get the corresponding WMAFormatInfo parameters.
EXTERN_C Bool GetFormatByPCMParamsAndBitrate(U32 ulSamplingRate, 
                                             U32 ulChannels, 
#if defined (WMA_V9)
                                             U32 nBytePerSample,
                                             U16 nValidBitsPerSample,
                                             U32 nChannelMask,
#endif // WMA_V9
                                             U32 ulBitrate, Bool fASFAVInterleave, WMAFormatInfo* pFormat);

// Only the following members of pSrcFormat are used:
// nSamplesPerSec, nChannels, nAvgBytesPerSec, nBlockAlign,
// nSamplesPerBlock, dwSuperBlockAlign, wEncodeOptions
Bool GetTranscodeDstFormatByIndex(const WMAFormatInfo* pSrcFormat,
                                  const U32 ulIndex,
                                  WMAFormatInfo* pDstFormat);


#ifdef __cplusplus
}
#endif


#endif //__CONFIG_H__
