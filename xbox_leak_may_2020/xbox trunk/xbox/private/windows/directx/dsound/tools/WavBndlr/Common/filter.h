/***************************************************************************
 *
 *  Copyright (C) 11/27/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       filter.h
 *  Content:    Filter classes.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/27/2001   dereks  Created.
 *
 ****************************************************************************/

#ifndef __FILTER_H__
#define __FILTER_H__

//
// Public filter types
//

#define WBFILTER_ADPCM  0x00000001
#define WBFILTER_8BIT   0x00000002
#define WBFILTER_MASK   0x00000003

#ifdef __cplusplus

//
// Xbox ADPCM filter
//

class CXboxAdpcmFilter
    : protected CImaAdpcmCodec
{
public:
    CXboxAdpcmFilter(void);
    virtual ~CXboxAdpcmFilter(void);

public:
    // Filter name
    static LPCSTR GetName(void);
    
    // Initialization
    virtual BOOL Initialize(LPCWAVEBANKMINIWAVEFORMAT pwfxSource);

    // Format
    static BOOL IsValidFormat(LPCWAVEBANKMINIWAVEFORMAT pwfxFormat);
    static BOOL ConvertFormat(LPWAVEBANKMINIWAVEFORMAT pwfxFormat);

    // Sizes
    static DWORD GetSourceAlignment(DWORD nChannels);
    virtual DWORD GetSourceAlignment(void);
    static DWORD GetDestinationAlignment(DWORD nChannels);
    virtual DWORD GetDestinationAlignment(void);
    static DWORD GetSize(DWORD cbSource, DWORD nChannels, BOOL fRoundUp = FALSE);
    virtual DWORD GetSize(DWORD cbSource, BOOL fRoundUp = FALSE);

    // Data conversion
    virtual BOOL Convert(LPCVOID pvSource, LPDWORD pcbRead, LPVOID pvDest, LPDWORD pcbWritten);
};

__inline LPCSTR CXboxAdpcmFilter::GetName(void)
{
    return "ADPCM";
}

__inline DWORD CXboxAdpcmFilter::GetSourceAlignment(DWORD nChannels)
{
    return nChannels * 2 * XBOX_ADPCM_SAMPLES_PER_BLOCK;
}

__inline DWORD CXboxAdpcmFilter::GetSourceAlignment(void)
{
    return m_wfxEncode.wfx.nChannels * 2 * XBOX_ADPCM_SAMPLES_PER_BLOCK;
}

__inline DWORD CXboxAdpcmFilter::GetDestinationAlignment(DWORD nChannels)
{
    return nChannels * XBOX_ADPCM_ENCODED_BLOCK_SIZE;
}

__inline DWORD CXboxAdpcmFilter::GetDestinationAlignment(void)
{
    return m_wfxEncode.wfx.nChannels * XBOX_ADPCM_ENCODED_BLOCK_SIZE;
}

__inline DWORD CXboxAdpcmFilter::GetSize(DWORD cbSource, DWORD nChannels, BOOL fRoundUp)
{
    const DWORD             cbSrcAlign  = GetSourceAlignment(nChannels);
    const DWORD             cbDstAlign  = GetDestinationAlignment(nChannels);
    
    if(fRoundUp)
    {
        cbSource += cbSrcAlign - 1;
    }

    cbSource /= cbSrcAlign;
    cbSource *= cbDstAlign;
    
    return cbSource;
}

__inline DWORD CXboxAdpcmFilter::GetSize(DWORD cbSource, BOOL fRoundUp)
{
    return GetSize(cbSource, m_wfxEncode.wfx.nChannels, fRoundUp);
}

//
// 16- to 8-bit filter
//

class C8BitFilter
{
protected:
    DWORD                   m_nChannels;    // Channel count

public:
    C8BitFilter(void);
    virtual ~C8BitFilter(void);

public:
    // Filter name
    static LPCSTR GetName(void);
    
    // Initialization
    virtual BOOL Initialize(LPCWAVEBANKMINIWAVEFORMAT pwfxSource);

    // Format
    static BOOL IsValidFormat(LPCWAVEBANKMINIWAVEFORMAT pwfxFormat);
    static BOOL ConvertFormat(LPWAVEBANKMINIWAVEFORMAT pwfxFormat);

    // Sizes
    static DWORD GetSourceAlignment(DWORD nChannels);
    virtual DWORD GetSourceAlignment(void);
    static DWORD GetDestinationAlignment(DWORD nChannels);
    virtual DWORD GetDestinationAlignment(void);
    static DWORD GetSize(DWORD cbSource, DWORD nChannels);
    virtual DWORD GetSize(DWORD cbSource);

    // Data conversion
    virtual BOOL Convert(LPCVOID pvSource, LPDWORD pcbRead, LPVOID pvDest, LPDWORD pcbWritten);
};
    
__inline LPCSTR C8BitFilter::GetName(void)
{
    return "8-bit";
}

__inline DWORD C8BitFilter::GetSourceAlignment(DWORD nChannels)
{
    return nChannels * 2;
}

__inline DWORD C8BitFilter::GetSourceAlignment(void)
{
    return m_nChannels * 2;
}

__inline DWORD C8BitFilter::GetDestinationAlignment(DWORD nChannels)
{
    return nChannels;
}

__inline DWORD C8BitFilter::GetDestinationAlignment(void)
{
    return m_nChannels;
}

__inline DWORD C8BitFilter::GetSize(DWORD cbSource, DWORD nChannels)
{
    return cbSource / (nChannels * 2) * nChannels;
}

__inline DWORD C8BitFilter::GetSize(DWORD cbSource)
{
    return cbSource / (m_nChannels * 2) * m_nChannels;
}

#endif // __cplusplus

#endif // __FILTER_H__
