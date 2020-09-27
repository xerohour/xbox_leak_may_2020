/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       CImaAdpcmCodec::.cpp
 *  Content:    IMA ADPCM CODEC.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  10/26/00    dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"

DEFINEREFTYPE(IMAADPCMWAVEFORMAT);
DEFINELPCTYPE(BYTE);
DEFINELPTYPE(SHORT);

#define IMAADPCM_BITS_PER_SAMPLE    4
#define IMAADPCM_HEADER_LENGTH      4

//
// IMA ADPCM encoder function prototype
//

typedef BOOL (*LPFNIMAADPCMCONVERT)(LPBYTE pbSrc, LPBYTE pbDst, UINT cBlocks, UINT nBlockAlignment, UINT cSamplesPerBlock, LPINT pnStepIndexL, LPINT pnStepIndexR);

//
// IMA ADPCM CODEC
//

class CImaAdpcmCodec
{
protected:
    IMAADPCMWAVEFORMAT      m_wfxEncode;                // Encoded format description
    BOOL                    m_fEncoder;                 // Is the object acting as an encoder?
    INT                     m_nStepIndexL;              // Left-channel stepping index
    INT                     m_nStepIndexR;              // Right-channel stepping index

private:
    static const SHORT      m_asNextStep[16];           // Step increment array
    static const SHORT      m_asStep[89];               // Step value array

private:
    LPFNIMAADPCMCONVERT     m_pfnConvert;               // Conversion function

public:
    CImaAdpcmCodec(void);
    ~CImaAdpcmCodec(void);

public:
    // Initialization
    void Initialize(REFIMAADPCMWAVEFORMAT wfxEncode, BOOL fEncoder);

    // Size conversions
    WORD GetEncodeAlignment(void);
    WORD GetDecodeAlignment(void);

    // Data conversions
    BOOL Convert(LPCVOID pvSrc, LPVOID pvDst, UINT cBlocks);

    // Format descriptions
    static void CreatePcmFormat(WORD nChannels, DWORD nSamplesPerSec, LPWAVEFORMATEX pwfxFormat);
    static void CreateImaAdpcmFormat(WORD nChannels, DWORD nSamplesPerSec, WORD nSamplesPerBlock, WORD nAlignmentMultiplier, LPIMAADPCMWAVEFORMAT pwfxFormat);

private:
    // En/decoded data alignment
    static WORD CalculateEncodeAlignment(WORD nSamplesPerBlock, WORD nChannels, WORD nAlignmentMultiplier);
    
    // Data conversion functions
    static BOOL EncodeM16(LPBYTE pbSrc, LPBYTE pbDst, UINT cBlocks, UINT nBlockAlignment, UINT cSamplesPerBlock, LPINT pnStepIndexL, LPINT pnStepIndexR);
    static BOOL EncodeS16(LPBYTE pbSrc, LPBYTE pbDst, UINT cBlocks, UINT nBlockAlignment, UINT cSamplesPerBlock, LPINT pnStepIndexL, LPINT pnStepIndexR);
    static BOOL DecodeM16(LPBYTE pbSrc, LPBYTE pbDst, UINT cBlocks, UINT nBlockAlignment, UINT cSamplesPerBlock, LPINT pnStepIndexL, LPINT pnStepIndexR);
    static BOOL DecodeS16(LPBYTE pbSrc, LPBYTE pbDst, UINT cBlocks, UINT nBlockAlignment, UINT cSamplesPerBlock, LPINT pnStepIndexL, LPINT pnStepIndexR);

    static INT EncodeSample(INT nInputSample, INT *nPredictedSample, INT nStepSize);
    static INT DecodeSample(INT nInputSample, INT nPredictedSample, INT nStepSize);

    static INT NextStepIndex(INT nEncodedSample, INT nStepIndex);
    static BOOL ValidStepIndex(INT nStepIndex);
};

__inline INT CImaAdpcmCodec::NextStepIndex(INT nEncodedSample, INT nStepIndex)
{
    nStepIndex += m_asNextStep[nEncodedSample];

    if(nStepIndex < 0)
    {
        nStepIndex = 0;
    }
    else if(nStepIndex >= NUMELMS(m_asStep))
    {
        nStepIndex = NUMELMS(m_asStep) - 1;
    }

    return nStepIndex;
}

__inline BOOL CImaAdpcmCodec::ValidStepIndex(INT nStepIndex)
{
    return (nStepIndex >= 0) && (nStepIndex < NUMELMS(m_asStep));
}

//
// IMA ADPCM Media Object implementation
//

class CImaAdpcmMediaObject
    : public XMediaObject, private CImaAdpcmCodec
{
    STDNEWDELETE

protected:
    ULONG                   m_ulRefCount;       // Object reference count

public:
    CImaAdpcmMediaObject(void);
    virtual ~CImaAdpcmMediaObject(void);

public:
    // Initialization
    void STDMETHODCALLTYPE Initialize(REFIMAADPCMWAVEFORMAT wfxEncode, BOOL fEncoder);
    
    // IUnknown methods
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // XMediaObject methods
    virtual HRESULT STDMETHODCALLTYPE GetInfo(LPXMEDIAINFO pInfo);
    virtual HRESULT STDMETHODCALLTYPE GetStatus(LPDWORD pdwStatus);
    virtual HRESULT STDMETHODCALLTYPE Process(LPCXMEDIAPACKET pxmbInput, LPCXMEDIAPACKET pxmbOutput);
    virtual HRESULT STDMETHODCALLTYPE Discontinuity(void);
    virtual HRESULT STDMETHODCALLTYPE Flush(void);

protected:
    // Size conversions
    WORD GetInputAlignment(void);
    WORD GetOutputAlignment(void);
};    

__inline void CImaAdpcmMediaObject::Initialize(REFIMAADPCMWAVEFORMAT wfxEncode, BOOL fEncoder)
{
    CImaAdpcmCodec::Initialize(wfxEncode, fEncoder);
}

__inline WORD CImaAdpcmMediaObject::GetInputAlignment(void)
{
    return m_fEncoder ? GetDecodeAlignment() : GetEncodeAlignment();
}

__inline WORD CImaAdpcmMediaObject::GetOutputAlignment(void)
{
    return m_fEncoder ? GetEncodeAlignment() : GetDecodeAlignment();
}


/****************************************************************************
 *
 *  CImaAdpcmCodec
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::CImaAdpcmCodec"

//
// This array is used by NextStepIndex to determine the next step index to use.  
// The step index is an index to the m_asStep[] array, below.
//

const SHORT CImaAdpcmCodec::m_asNextStep[16] =
{
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
};

//
// This array contains the array of step sizes used to encode the ADPCM
// samples.  The step index in each ADPCM block is an index to this array.
//

const SHORT CImaAdpcmCodec::m_asStep[89] =
{
        7,     8,     9,    10,    11,    12,    13,
       14,    16,    17,    19,    21,    23,    25,
       28,    31,    34,    37,    41,    45,    50,
       55,    60,    66,    73,    80,    88,    97,
      107,   118,   130,   143,   157,   173,   190,
      209,   230,   253,   279,   307,   337,   371,
      408,   449,   494,   544,   598,   658,   724,
      796,   876,   963,  1060,  1166,  1282,  1411,
     1552,  1707,  1878,  2066,  2272,  2499,  2749,
     3024,  3327,  3660,  4026,  4428,  4871,  5358,
     5894,  6484,  7132,  7845,  8630,  9493, 10442,
    11487, 12635, 13899, 15289, 16818, 18500, 20350,
    22385, 24623, 27086, 29794, 32767
};

CImaAdpcmCodec::CImaAdpcmCodec
(
    void
)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CImaAdpcmCodec
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::~CImaAdpcmCodec"

CImaAdpcmCodec::~CImaAdpcmCodec
(
    void
)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      REFIMAADPCMWAVEFORMAT [in]: encoded data format.
 *      BOOL [in]: TRUE to initialize the object as an encoder.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::Initialize"

void
CImaAdpcmCodec::Initialize
(
    REFIMAADPCMWAVEFORMAT               wfxEncode, 
    BOOL                                fEncoder
)
{
    static const LPFNIMAADPCMCONVERT    apfnConvert[2][2] = 
    { 
        {
            DecodeM16,
            DecodeS16 
        },
        {
            EncodeM16,
            EncodeS16 
        }
    };
    
    DPF_ENTER();

    ASSERT(wfxEncode.wfx.nChannels <= 2);

    //
    // Save the format data
    //

    m_wfxEncode = wfxEncode;
    m_fEncoder = fEncoder;

    //
    // Set up the conversion function
    //

    m_pfnConvert = apfnConvert[!!fEncoder][wfxEncode.wfx.nChannels - 1];

    //
    // Initialize the stepping indeces
    //

    m_nStepIndexL = m_nStepIndexR = 0;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Convert
 *
 *  Description:
 *      Converts data from the source to destination format.
 *
 *  Arguments:
 *      LPCVOID [in]: source buffer.
 *      LPVOID [out]: destination buffer.
 *      UINT [in]: block count.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::Convert"

__inline
BOOL
CImaAdpcmCodec::Convert
(
    LPCVOID                 pvSrc,
    LPVOID                  pvDst,
    UINT                    cBlocks
)
{
    return m_pfnConvert((LPBYTE)pvSrc, (LPBYTE)pvDst, cBlocks, m_wfxEncode.wfx.nBlockAlign, m_wfxEncode.wSamplesPerBlock, &m_nStepIndexL, &m_nStepIndexR);
}


/****************************************************************************
 *
 *  GetEncodeAlignment
 *
 *  Description:
 *      Gets the alignment of an encoded buffer.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      WORD: alignment, in bytes.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::GetEncodeAlignment"

__inline WORD
CImaAdpcmCodec::GetEncodeAlignment
(
    void
)
{
    return m_wfxEncode.wfx.nBlockAlign;
}


/****************************************************************************
 *
 *  GetDecodeAlignment
 *
 *  Description:
 *      Gets the alignment of a decoded buffer.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: alignment, in bytes.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::GetDecodeAlignment"

__inline WORD
CImaAdpcmCodec::GetDecodeAlignment
(
    void
)
{
    return m_wfxEncode.wSamplesPerBlock * m_wfxEncode.wfx.nChannels * 2;
}


/****************************************************************************
 *
 *  CalculateEncodeAlignment
 *
 *  Description:
 *      Calculates an encoded data block alignment based on a PCM sample
 *      count and an alignment multiplier.
 *
 *  Arguments:
 *      WORD [in]: channel count.
 *      WORD [in]: PCM samples per block.
 *      WORD [in]: encoded data alignment multiplier, in bytes.
 *
 *  Returns:  
 *      WORD: alignment, in bytes.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::CalculateEncodeAlignment"

__inline WORD
CImaAdpcmCodec::CalculateEncodeAlignment
(
    WORD                    nChannels,
    WORD                    nSamplesPerBlock,
    WORD                    nAlignmentMultiplier
)
{
    const WORD              nEncodedSampleBits  = nChannels * IMAADPCM_BITS_PER_SAMPLE;
    const WORD              nHeaderBytes        = nChannels * IMAADPCM_HEADER_LENGTH;
    WORD                    nBlockAlign;

    //
    // Calculate the raw block alignment that nSamplesPerBlock dictates.  This
    // value may include a partial encoded sample, so be sure to round up.
    //
    // Start with the samples-per-block, minus 1.  The first sample is actually
    // stored in the header.
    //

    nBlockAlign = nSamplesPerBlock - 1;

    //
    // Convert to encoded sample size
    //

    nBlockAlign *= nEncodedSampleBits;
    nBlockAlign += 7;
    nBlockAlign /= 8;

    //
    // The stereo encoder requires that there be at least two DWORDs to process
    //

    nBlockAlign += 7;
    nBlockAlign /= 8;
    nBlockAlign *= 8;

    //
    // Add the header
    //

    nBlockAlign += nHeaderBytes;

    //
    // Align the alignment, again rounding up.  The added amount will be
    // treated as padding by the en/decode routines.
    //

    if(nAlignmentMultiplier > 1)
    {
        nBlockAlign += nAlignmentMultiplier - 1;
        nBlockAlign /= nAlignmentMultiplier;
        nBlockAlign *= nAlignmentMultiplier;
    }

    return nBlockAlign;
}


/****************************************************************************
 *
 *  CreatePcmFormat
 *
 *  Description:
 *      Creates a PCM format descriptor.
 *
 *  Arguments:
 *      WORD [in]: channel count.
 *      DWORD [in]: sampling rate.
 *      LPWAVEFORMATEX [out]: format descriptor.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::CreatePcmFormat"

__inline
void
CImaAdpcmCodec::CreatePcmFormat
(
    WORD                    nChannels, 
    DWORD                   nSamplesPerSec, 
    LPWAVEFORMATEX          pwfx
)
{
    pwfx->wFormatTag = WAVE_FORMAT_PCM;
    pwfx->nChannels = nChannels;
    pwfx->nSamplesPerSec = nSamplesPerSec;
    pwfx->nBlockAlign = nChannels * 2;
    pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
    pwfx->wBitsPerSample = 16;
    pwfx->cbSize = 0;
}


/****************************************************************************
 *
 *  CreateImaAdpcmFormat
 *
 *  Description:
 *      Creates an IMA ADPCM format descriptor.
 *
 *  Arguments:
 *      WORD [in]: channel count.
 *      DWORD [in]: sampling rate.
 *      LPIMAADPCMWAVEFORMAT [out]: format descriptor.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::CreateImaAdpcmFormat"

__inline
void
CImaAdpcmCodec::CreateImaAdpcmFormat
(
    WORD                    nChannels, 
    DWORD                   nSamplesPerSec, 
    WORD                    nSamplesPerBlock,
    WORD                    nAlignmentMultiplier,
    LPIMAADPCMWAVEFORMAT    pwfx
)
{
    pwfx->wfx.wFormatTag = WAVE_FORMAT_IMA_ADPCM;
    pwfx->wfx.nChannels = nChannels;
    pwfx->wfx.nSamplesPerSec = nSamplesPerSec;
    pwfx->wfx.nBlockAlign = CalculateEncodeAlignment(nChannels, nSamplesPerBlock, nAlignmentMultiplier);
    pwfx->wfx.nAvgBytesPerSec = nSamplesPerSec * pwfx->wfx.nBlockAlign / nSamplesPerBlock;
    pwfx->wfx.wBitsPerSample = IMAADPCM_BITS_PER_SAMPLE;
    pwfx->wfx.cbSize = sizeof(*pwfx) - sizeof(pwfx->wfx);
    pwfx->wSamplesPerBlock = nSamplesPerBlock;
}


/****************************************************************************
 *
 *  EncodeSample
 *
 *  Description:
 *      Encodes a sample.
 *
 *  Arguments:
 *      INT [in]: the sample to be encoded.
 *      LPINT [in/out]: the predicted value of the sample.
 *      INT [in]: the quantization step size used to encode the sample.
 *
 *  Returns:  
 *      INT: the encoded ADPCM sample.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::EncodeSample"

__inline INT
CImaAdpcmCodec::EncodeSample
(
    INT                 nInputSample,
    LPINT               pnPredictedSample,
    INT                 nStepSize
)
{
    INT                 nPredictedSample;
    LONG                lDifference;
    INT                 nEncodedSample;
    
    DPF_ENTER();

    nPredictedSample = *pnPredictedSample;

    lDifference = nInputSample - nPredictedSample;
    nEncodedSample = 0;

    if(lDifference < 0) 
    {
        nEncodedSample = 8;
        lDifference = -lDifference;
    }

    if(lDifference >= nStepSize)
    {
        nEncodedSample |= 4;
        lDifference -= nStepSize;
    }

    nStepSize >>= 1;

    if(lDifference >= nStepSize)
    {
        nEncodedSample |= 2;
        lDifference -= nStepSize;
    }

    nStepSize >>= 1;

    if(lDifference >= nStepSize)
    {
        nEncodedSample |= 1;
        lDifference -= nStepSize;
    }

    if(nEncodedSample & 8)
    {
        nPredictedSample = nInputSample + lDifference - (nStepSize >> 1);
    }
    else
    {
        nPredictedSample = nInputSample - lDifference + (nStepSize >> 1);
    }

    if(nPredictedSample > 32767)
    {
        nPredictedSample = 32767;
    }
    else if(nPredictedSample < -32768)
    {
        nPredictedSample = -32768;
    }

    *pnPredictedSample = nPredictedSample;
    
    DPF_LEAVE(nEncodedSample);
    
    return nEncodedSample;
}


/****************************************************************************
 *
 *  DecodeSample
 *
 *  Description:
 *      Decodes an encoded sample.
 *
 *  Arguments:
 *      INT [in]: the sample to be decoded.
 *      INT [in]: the predicted value of the sample.
 *      INT [i]: the quantization step size used to encode the sample.
 *
 *  Returns:  
 *      INT: the decoded PCM sample.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::DecodeSample"

__inline INT
CImaAdpcmCodec::DecodeSample
(
    INT                 nEncodedSample,
    INT                 nPredictedSample,
    INT                 nStepSize
)
{
    LONG                lDifference;
    LONG                lNewSample;

    DPF_ENTER();

    lDifference = nStepSize >> 3;

    if(nEncodedSample & 4) 
    {
        lDifference += nStepSize;
    }

    if(nEncodedSample & 2) 
    {
        lDifference += nStepSize >> 1;
    }

    if(nEncodedSample & 1) 
    {
        lDifference += nStepSize >> 2;
    }

    if(nEncodedSample & 8)
    {
        lDifference = -lDifference;
    }

    lNewSample = nPredictedSample + lDifference;

    if((LONG)(SHORT)lNewSample != lNewSample)
    {
        if(lNewSample < -32768)
        {
            lNewSample = -32768;
        }
        else
        {
            lNewSample = 32767;
        }
    }

    DPF_LEAVE(lNewSample);

    return (INT)lNewSample;
}


/****************************************************************************
 *
 *  Conversion Routines
 *
 *  Description:
 *      Converts a PCM buffer to ADPCM, or the reverse.
 *
 *  Arguments:
 *      LPBYTE [in]: source buffer.
 *      LPBYTE [out]: destination buffer.
 *      UINT [in]: block count.
 *      UINT [in]: block alignment of the ADPCM data, in bytes.
 *      UINT [in]: the number of samples in each ADPCM block (not used in
 *                 decoding).
 *      LPINT [in/out]: left-channel stepping index.
 *      LPINT [in/out]: right-channel stepping index.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::EncodeM16"

BOOL
CImaAdpcmCodec::EncodeM16
(
    LPBYTE                  pbSrc,
    LPBYTE                  pbDst,
    UINT                    cBlocks,
    UINT                    nBlockAlignment,
    UINT                    cSamplesPerBlock,
    LPINT                   pnStepIndexL,
    LPINT                   pnStepIndexR
)
{
    LPBYTE                  pbBlock;
    UINT                    cSamples;
    INT                     nSample;
    INT                     nStepSize;
    INT                     nEncSample1;
    INT                     nEncSample2;
    INT                     nPredSample;
    INT                     nStepIndex;

    //
    // Save a local copy of the step index so we're not constantly 
    // dereferencing a pointer.
    //
    
    nStepIndex = *pnStepIndexL;

    //
    // Enter the main loop
    //
    
    while(cBlocks--)
    {
        pbBlock = pbDst;
        cSamples = cSamplesPerBlock - 1;

        //
        // Block header
        //

        nPredSample = *(LPSHORT)pbSrc;
        pbSrc += sizeof(SHORT);

        *(LONG *)pbBlock = MAKELONG(nPredSample, nStepIndex);
        pbBlock += sizeof(LONG);

        //
        // We have written the header for this block--now write the data
        // chunk (which consists of a bunch of encoded nibbles).  Note
        // that if we don't have enough data to fill a complete byte, then
        // we add a 0 nibble on the end.
        //

        while(cSamples)
        {
            //
            // Sample 1
            //

            nSample = *(LPSHORT)pbSrc;
            pbSrc += sizeof(SHORT);
            cSamples--;

            nStepSize = m_asStep[nStepIndex];
            nEncSample1 = EncodeSample(nSample, &nPredSample, nStepSize);
            nStepIndex = NextStepIndex(nEncSample1, nStepIndex);

            //
            // Sample 2
            //

            if(cSamples)
            {
                nSample = *(LPSHORT)pbSrc;
                pbSrc += sizeof(SHORT);
                cSamples--;

                nStepSize = m_asStep[nStepIndex];
                nEncSample2 = EncodeSample(nSample, &nPredSample, nStepSize);
                nStepIndex = NextStepIndex(nEncSample2, nStepIndex);
            }
            else
            {
                nEncSample2 = 0;
            }

            //
            // Write out encoded byte.
            //

            *pbBlock++ = (BYTE)(nEncSample1 | (nEncSample2 << 4));
        }

        //
        // Skip padding
        //

        pbDst += nBlockAlignment;
    }

    //
    // Restore the value of the step index to be used on the next buffer.
    //

    *pnStepIndexL = nStepIndex;

    DPF_LEAVE(TRUE);
    
    return TRUE;
}


#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::EncodeS16"

BOOL
CImaAdpcmCodec::EncodeS16
(
    LPBYTE                  pbSrc,
    LPBYTE                  pbDst,
    UINT                    cBlocks,
    UINT                    nBlockAlignment,
    UINT                    cSamplesPerBlock,
    LPINT                   pnStepIndexL,
    LPINT                   pnStepIndexR
)
{
    LPBYTE                  pbBlock;
    UINT                    cSamples;
    UINT                    cSubSamples;
    INT                     nSample;
    INT                     nStepSize;
    DWORD                   dwLeft;
    DWORD                   dwRight;
    INT                     nEncSampleL;
    INT                     nPredSampleL;
    INT                     nStepIndexL;
    INT                     nEncSampleR;
    INT                     nPredSampleR;
    INT                     nStepIndexR;
    UINT                    i;

    //
    // Save a local copy of the step indeces so we're not constantly 
    // dereferencing a pointer.
    //
    
    nStepIndexL = *pnStepIndexL;
    nStepIndexR = *pnStepIndexR;

    //
    // Enter the main loop
    //
    
    while(cBlocks--)
    {
        pbBlock = pbDst;
        cSamples = cSamplesPerBlock - 1;

        //
        // LEFT channel block header
        //

        nPredSampleL = *(LPSHORT)pbSrc;
        pbSrc += sizeof(SHORT);

        *(LONG *)pbBlock = MAKELONG(nPredSampleL, nStepIndexL);
        pbBlock += sizeof(LONG);

        //
        // RIGHT channel block header
        //

        nPredSampleR = *(LPSHORT)pbSrc;
        pbSrc += sizeof(SHORT);

        *(LONG *)pbBlock = MAKELONG(nPredSampleR, nStepIndexR);
        pbBlock += sizeof(LONG);

        //
        // We have written the header for this block--now write the data
        // chunk.  This consists of 8 left samples (one DWORD of output)
        // followed by 8 right samples (also one DWORD).  Since the input
        // samples are interleaved, we create the left and right DWORDs
        // sample by sample, and then write them both out.
        //

        while(cSamples)
        {
            dwLeft = 0;
            dwRight = 0;

            cSubSamples = min(cSamples, 8);

            for(i = 0; i < cSubSamples; i++)
            {
                //
                // LEFT channel
                //

                nSample = *(LPSHORT)pbSrc;
                pbSrc += sizeof(SHORT);

                nStepSize = m_asStep[nStepIndexL];
                
                nEncSampleL = EncodeSample(nSample, &nPredSampleL, nStepSize);

                nStepIndexL = NextStepIndex(nEncSampleL, nStepIndexL);
                dwLeft |= (DWORD)nEncSampleL << (4 * i);

                //
                // RIGHT channel
                //

                nSample = *(LPSHORT)pbSrc;
                pbSrc += sizeof(SHORT);

                nStepSize = m_asStep[nStepIndexR];
                
                nEncSampleR = EncodeSample(nSample, &nPredSampleR, nStepSize);

                nStepIndexR = NextStepIndex(nEncSampleR, nStepIndexR);
                dwRight |= (DWORD)nEncSampleR << (4 * i);
            }

            //
            // Write out encoded DWORDs.
            //

            *(LPDWORD)pbBlock = dwLeft;
            pbBlock += sizeof(DWORD);

            *(LPDWORD)pbBlock = dwRight;
            pbBlock += sizeof(DWORD);

            cSamples -= cSubSamples;
        }

        //
        // Skip padding
        //

        pbDst += nBlockAlignment;
    }

    //
    // Restore the value of the step index to be used on the next buffer.
    //
    
    *pnStepIndexL = nStepIndexL;
    *pnStepIndexR = nStepIndexR;

    DPF_LEAVE(TRUE);
    
    return TRUE;

}


#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::DecodeM16"

BOOL
CImaAdpcmCodec::DecodeM16   
(
    LPBYTE                  pbSrc,
    LPBYTE                  pbDst,
    UINT                    cBlocks,
    UINT                    nBlockAlignment,
    UINT                    cSamplesPerBlock,
    LPINT                   pnStepIndexL,
    LPINT                   pnStepIndexR
)
{
    BOOL                    fSuccess    = TRUE;
    LPBYTE                  pbBlock;
    UINT                    cSamples;
    BYTE                    bSample;
    INT                     nStepSize;
    INT                     nEncSample;
    INT                     nPredSample;
    INT                     nStepIndex;
    DWORD                   dwHeader;

    DPF_ENTER();

    //
    // Enter the main loop
    //
    
    while(cBlocks--)
    {
        pbBlock = pbSrc;
        cSamples = cSamplesPerBlock - 1;
        
        //
        // Block header
        //

        dwHeader = *(LPDWORD)pbBlock;
        pbBlock += sizeof(DWORD);

        nPredSample = (INT)(SHORT)LOWORD(dwHeader);
        nStepIndex = (INT)(BYTE)HIWORD(dwHeader);

        if(!ValidStepIndex(nStepIndex))
        {
            //
            // The step index is out of range - this is considered a fatal
            // error as the input stream is corrupted.  We fail by returning
            // zero bytes converted.
            //

            DPF_ERROR("invalid step index %lu", nStepIndex);
            fSuccess = FALSE;
            break;
        }
        
        //
        // Write out first sample
        //

        *(LPSHORT)pbDst = (SHORT)nPredSample;
        pbDst += sizeof(SHORT);

        //
        // Enter the block loop
        //

        while(cSamples)
        {
            bSample = *pbBlock++;

            //
            // Sample 1
            //

            nEncSample = (bSample & (BYTE)0x0F);
            nStepSize = m_asStep[nStepIndex];
            nPredSample = DecodeSample(nEncSample, nPredSample, nStepSize);
            nStepIndex = NextStepIndex(nEncSample, nStepIndex);

            *(LPSHORT)pbDst = (SHORT)nPredSample;
            pbDst += sizeof(SHORT);

            cSamples--;

            //
            // Sample 2
            //

            if(cSamples)
            {
                nEncSample = (bSample >> 4);
                nStepSize = m_asStep[nStepIndex];
                nPredSample = DecodeSample(nEncSample, nPredSample, nStepSize);
                nStepIndex = NextStepIndex(nEncSample, nStepIndex);

                *(LPSHORT)pbDst = (SHORT)nPredSample;
                pbDst += sizeof(SHORT);

                cSamples--;
            }
        }

        //
        // Skip padding
        //

        pbSrc += nBlockAlignment;
    }

    DPF_LEAVE(fSuccess);

    return fSuccess;
}


#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmCodec::DecodeS16"

BOOL
CImaAdpcmCodec::DecodeS16
(
    LPBYTE                  pbSrc,
    LPBYTE                  pbDst,
    UINT                    cBlocks,
    UINT                    nBlockAlignment,
    UINT                    cSamplesPerBlock,
    LPINT                   pnStepIndexL,
    LPINT                   pnStepIndexR
)
{
    BOOL                    fSuccess    = TRUE;
    LPBYTE                  pbBlock;
    UINT                    cSamples;
    UINT                    cSubSamples;
    INT                     nStepSize;
    DWORD                   dwHeader;
    DWORD                   dwLeft;
    DWORD                   dwRight;
    INT                     nEncSampleL;
    INT                     nPredSampleL;
    INT                     nStepIndexL;
    INT                     nEncSampleR;
    INT                     nPredSampleR;
    INT                     nStepIndexR;
    UINT                    i;

    DPF_ENTER();    

    //
    // Enter the main loop
    //
    
    while(cBlocks--)
    {
        pbBlock = pbSrc;
        cSamples = cSamplesPerBlock - 1;

        //
        // LEFT channel header
        //

        dwHeader = *(LPDWORD)pbBlock;
        pbBlock += sizeof(DWORD);
        
        nPredSampleL = (INT)(SHORT)LOWORD(dwHeader);
        nStepIndexL = (INT)(BYTE)HIWORD(dwHeader);

        if(!ValidStepIndex(nStepIndexL)) 
        {
            //
            // The step index is out of range - this is considered a fatal
            // error as the input stream is corrupted.  We fail by returning
            // zero bytes converted.
            //

            DPF_ERROR("invalid step index %u (L)", nStepIndexL);
            fSuccess = FALSE;
            break;
        }
        
        //
        // RIGHT channel header
        //

        dwHeader = *(LPDWORD)pbBlock;
        pbBlock += sizeof(DWORD);
        
        nPredSampleR = (INT)(SHORT)LOWORD(dwHeader);
        nStepIndexR = (INT)(BYTE)HIWORD(dwHeader);

        if(!ValidStepIndex(nStepIndexR))
        {
            //
            // The step index is out of range - this is considered a fatal
            // error as the input stream is corrupted.  We fail by returning
            // zero bytes converted.
            //

            DPF_ERROR("invalid step index %u (R)",nStepIndexR);
            fSuccess = FALSE;
            break;
        }

        //
        // Write out first sample
        //

        *(LPDWORD)pbDst = MAKELONG(nPredSampleL, nPredSampleR);
        pbDst += sizeof(DWORD);

        //
        // The first DWORD contains 4 left samples, the second DWORD
        // contains 4 right samples.  We process the source in 8-byte
        // chunks to make it easy to interleave the output correctly.
        //

        while(cSamples)
        {
            dwLeft = *(LPDWORD)pbBlock;
            pbBlock += sizeof(DWORD);
            dwRight = *(LPDWORD)pbBlock;
            pbBlock += sizeof(DWORD);

            cSubSamples = min(cSamples, 8);
            
            for(i = 0; i < cSubSamples; i++)
            {
                //
                // LEFT channel
                //

                nEncSampleL = (dwLeft & 0x0F);
                nStepSize = m_asStep[nStepIndexL];
                nPredSampleL = DecodeSample(nEncSampleL, nPredSampleL, nStepSize);
                nStepIndexL = NextStepIndex(nEncSampleL, nStepIndexL);

                //
                // RIGHT channel
                //

                nEncSampleR = (dwRight & 0x0F);
                nStepSize = m_asStep[nStepIndexR];
                nPredSampleR = DecodeSample(nEncSampleR, nPredSampleR, nStepSize);
                nStepIndexR = NextStepIndex(nEncSampleR, nStepIndexR);

                //
                // Write out sample
                //

                *(LPDWORD)pbDst = MAKELONG(nPredSampleL, nPredSampleR);
                pbDst += sizeof(DWORD);

                //
                // Shift the next input sample into the low-order 4 bits.
                //

                dwLeft >>= 4;
                dwRight >>= 4;
            }

            cSamples -= cSubSamples;
        }

        //
        // Skip padding
        //

        pbSrc += nBlockAlignment;
    }

    DPF_LEAVE(fSuccess);

    return fSuccess;
}


/****************************************************************************
 *
 *  CImaAdpcmMediaObject
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmMediaObject::CImaAdpcmMediaObject"

CImaAdpcmMediaObject::CImaAdpcmMediaObject
(
    void
)
{
    DPF_ENTER();

    m_ulRefCount = 1;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CImaAdpcmMediaObject
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmMediaObject::~CImaAdpcmMediaObject"

CImaAdpcmMediaObject::~CImaAdpcmMediaObject
(
    void
)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  AddRef
 *
 *  Description:
 *      Increments the object reference count.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      ULONG: reference count.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundSecondaryBuffer::AddRef"

ULONG
CImaAdpcmMediaObject::AddRef
(
    void
)
{
    ULONG                   ulRefCount;

    DPF_ENTER();

    ASSERT(m_ulRefCount);
    
    ulRefCount = ++m_ulRefCount;

    DPF_LEAVE(ulRefCount);

    return ulRefCount;
}


/****************************************************************************
 *
 *  Release
 *
 *  Description:
 *      Decrements the object reference count.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      ULONG: reference count.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundSecondaryBuffer::Release"

ULONG
CImaAdpcmMediaObject::Release
(
    void
)
{
    ULONG                   ulRefCount;

    DPF_ENTER();

    ASSERT(m_ulRefCount);
    
    if(!(ulRefCount = --m_ulRefCount))
    {
        delete this;
    }

    DPF_LEAVE(ulRefCount);

    return ulRefCount;
}


/****************************************************************************
 *
 *  GetInfo
 *
 *  Description:
 *      Gets information about the data the object supports.
 *
 *  Arguments:
 *      LPXMEDIAINFO [in/out]: info structure.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmMediaObject::GetInfo"

HRESULT
CImaAdpcmMediaObject::GetInfo
(
    LPXMEDIAINFO            pInfo
)
{
    DPF_ENTER();

    ASSERT(pInfo);

    pInfo->dwFlags = XMO_STREAMF_WHOLE_SAMPLES | XMO_STREAMF_FIXED_SAMPLE_SIZE;
    pInfo->dwMaxLookahead = 0;
    pInfo->dwInputSize = GetInputAlignment();
    pInfo->dwOutputSize = GetOutputAlignment();

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  Discontinuity
 *
 *  Description:
 *      Indicates a discontinuity in the stream data>
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmMediaObject::Discontinuity"

HRESULT
CImaAdpcmMediaObject::Discontinuity
(
    void
)                                       
{                                           
    DPF_ENTER();
    
    //
    // Reset stepping indeces
    //

    m_nStepIndexL = m_nStepIndexR = 0;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  Flush
 *
 *  Description:
 *      Resets the stream to it's default state.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmMediaObject::Flush"

HRESULT
CImaAdpcmMediaObject::Flush
(
    void
)                                       
{                                           
    DPF_ENTER();
    
    //
    // Reset stepping indeces
    //

    m_nStepIndexL = m_nStepIndexR = 0;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  GetStatus
 *
 *  Description:
 *      Gets stream status.
 *
 *  Arguments:
 *      LPDWORD [out]: stream status.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmMediaObject::GetStatus"

HRESULT
CImaAdpcmMediaObject::GetStatus
(
    LPDWORD                 pdwStatus
)
{
    DPF_ENTER();

    ASSERT(pdwStatus);

    *pdwStatus = XMO_STATUSF_ACCEPT_INPUT_DATA | XMO_STATUSF_ACCEPT_OUTPUT_DATA;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  Process
 *
 *  Description:
 *      Submits buffers to the stream.
 *
 *  Arguments:
 *      LPCXMEDIAPACKET  [in]: input buffer.
 *      LPCXMEDIAPACKET  [in]: output buffer.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CImaAdpcmMediaObject::Process"

HRESULT
CImaAdpcmMediaObject::Process
(
    LPCXMEDIAPACKET         pxmbSource, 
    LPCXMEDIAPACKET         pxmbDest 
)
{
    DWORD                   dwInputAlignment;
    DWORD                   dwOutputAlignment;
    DWORD                   dwInputBlocks;
    DWORD                   dwOutputBlocks;
    DWORD                   dwBlocks;
    DWORD                   dwInputSize;
    DWORD                   dwOutputSize;
    HRESULT                 hr;
    
    DPF_ENTER();

    ASSERT(pxmbSource && pxmbDest);

    //
    // Initialize the packets' output parameters
    //

    XMOAcceptPacket(pxmbSource);
    XMOAcceptPacket(pxmbDest);

    //
    // Align buffer sizes and convert to a block count
    //

    dwInputAlignment = GetInputAlignment();
    dwOutputAlignment = GetOutputAlignment();

    dwInputBlocks = pxmbSource->dwMaxSize / dwInputAlignment;
    dwOutputBlocks = pxmbDest->dwMaxSize / dwOutputAlignment;

    dwBlocks = min(dwInputBlocks, dwOutputBlocks);

    //
    // Convert
    //

    if(Convert((LPBYTE)pxmbSource->pvBuffer, (LPBYTE)pxmbDest->pvBuffer, dwBlocks))
    {
        hr = DS_OK;
    }
    else
    {
        DPF_ERROR("CODEC failure");
        hr = DSERR_GENERIC;
    }

    //
    // Complete the packets
    //

    if(SUCCEEDED(hr))
    {
        XMOCompletePacket(pxmbSource, dwBlocks * dwInputAlignment);
        XMOCompletePacket(pxmbDest, dwBlocks * dwOutputAlignment);
    }
    else
    {
        XMOCompletePacket(pxmbSource, 0, NULL, NULL, XMEDIAPACKET_STATUS_FAILURE);
        XMOCompletePacket(pxmbDest, 0, NULL, NULL, XMEDIAPACKET_STATUS_FAILURE);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  AdpcmCreateEncoder
 *
 *  Description:
 *      Creates an ADPCM CODEC Media Object.
 *
 *  Arguments:
 *      LPCWAVEFORMATEX [in]: source format description.
 *      DWORD [in]: bytes of PCM data per block.  This value must be aligned
 *                  to 8 samples.
 *      LPIMAADPCMWAVEFORMAT [out]: destination format description.
 *      XMediaObject ** [out]: CODEC Media Object.  The caller is responsbile
 *                             for freeing this object with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "AdpcmCreateEncoder"

HRESULT
AdpcmCreateEncoder
(
    LPCWAVEFORMATEX         pwfxDecoded,
    DWORD                   dwDecodedBytesPerBlock,
    LPIMAADPCMWAVEFORMAT    pwfxEncoded,
    XMediaObject **         ppMediaObject
)
{
    CImaAdpcmMediaObject *  pMediaObject;
    IMAADPCMWAVEFORMAT      wfxEncoded;

#ifndef ASSUME_VALID_PARAMETERS

    BOOL                    fValidFormat;

#endif // ASSUME_VALID_PARAMETERS

    HRESULT                 hr;

    DPF_ENTER();

#ifndef ASSUME_VALID_PARAMETERS

    ASSERT(pwfxDecoded);
    ASSERT(ppMediaObject);

    fValidFormat = IsValidPcmFormat(pwfxDecoded);

    if(fValidFormat && (pwfxDecoded->nChannels > 2))
    {
        DPF_ERROR("The IMA ADPCM CODEC only supports MONO and STEREO");
        fValidFormat = FALSE;
    }            

    if(fValidFormat && (16 != pwfxDecoded->wBitsPerSample))
    {
        DPF_ERROR("The IMA ADPCM CODEC only supports 16-bit PCM data");
        fValidFormat = FALSE;
    }

    ASSERT(!(dwDecodedBytesPerBlock % pwfxDecoded->nBlockAlign));

#endif // ASSUME_VALID_PARAMETERS

    //
    // Create the destination format
    //

    CImaAdpcmCodec::CreateImaAdpcmFormat(pwfxDecoded->nChannels, pwfxDecoded->nSamplesPerSec, (WORD)(dwDecodedBytesPerBlock / pwfxDecoded->nBlockAlign), 1, &wfxEncoded);

    //
    // Create the CODEC object
    //

    hr = HRFROMP(pMediaObject = NEW(CImaAdpcmMediaObject));

    if(SUCCEEDED(hr))
    {
        pMediaObject->Initialize(wfxEncoded, TRUE);
    }

    //
    // Provide format data to the caller
    //

    if(SUCCEEDED(hr) && pwfxEncoded)
    {
        CopyMemory(pwfxEncoded, &wfxEncoded, sizeof(wfxEncoded));
    }

    //
    // Success
    //

    if(SUCCEEDED(hr))
    {
        *ppMediaObject = ADDREF(pMediaObject);
    }

    RELEASE(pMediaObject);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  AdpcmCreateDecoder
 *
 *  Description:
 *      Creates an ADPCM CODEC Media Object.
 *
 *  Arguments:
 *      LPCIMAADPCMWAVEFORMAT [in]: source format description.
 *      LPWAVEFORMATEX [out]: destination format description.
 *      XMediaObject ** [out]: CODEC Media Object.  The caller is responsbile
 *                             for freeing this object with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "AdpcmCreateDecoder"

HRESULT
AdpcmCreateDecoder
(
    LPCIMAADPCMWAVEFORMAT   pwfxEncoded,
    LPWAVEFORMATEX          pwfxDecoded,
    XMediaObject **         ppMediaObject
)
{
    CImaAdpcmMediaObject *  pMediaObject;
    HRESULT                 hr;

    DPF_ENTER();

#ifndef ASSUME_VALID_PARAMETERS

    ASSERT(pwfxEncoded);
    ASSERT(ppMediaObject);

    IsValidImaAdpcmFormat(pwfxEncoded);

#endif // ASSUME_VALID_PARAMETERS

    //
    // Create the CODEC object
    //

    hr = HRFROMP(pMediaObject = NEW(CImaAdpcmMediaObject));

    if(SUCCEEDED(hr))
    {
        pMediaObject->Initialize(*pwfxEncoded, FALSE);
    }

    //
    // Provide format data to the caller
    //

    if(SUCCEEDED(hr) && pwfxDecoded)
    {
        CImaAdpcmCodec::CreatePcmFormat(pwfxEncoded->wfx.nChannels, pwfxEncoded->wfx.nSamplesPerSec, pwfxDecoded);
    }

    //
    // Success
    //

    if(SUCCEEDED(hr))
    {
        *ppMediaObject = ADDREF(pMediaObject);
    }

    RELEASE(pMediaObject);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


