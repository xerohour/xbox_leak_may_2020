/***************************************************************************
 *
 *  Copyright (C) 11/2/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       wavbndlr.h
 *  Content:    Wave Bundler definitions.
 *
 ****************************************************************************/

#ifndef __WAVBNDLR_H__
#define __WAVBNDLR_H__

//
// Wave bank entry compressed data format
//

#define WAVEBANKMINIFORMAT_TAG_PCM      0x0 // PCM data
#define WAVEBANKMINIFORMAT_TAG_ADPCM    0x1 // ADPCM data

#define WAVEBANKMINIFORMAT_BITDEPTH_4   0x0 // 4-bit data (ADPCM only)
#define WAVEBANKMINIFORMAT_BITDEPTH_8   0x0 // 8-bit data (PCM only)
#define WAVEBANKMINIFORMAT_BITDEPTH_16  0x1 // 16-bit data (PCM only)

typedef struct _WAVEBANKMINIWAVEFORMAT
{
    DWORD       wFormatTag      : 1;    // PCM vs. ADPCM
    DWORD       nChannels       : 3;    // Channel count (1 - 6)
    DWORD       nSamplesPerSec  : 27;   // Sampling rate
    DWORD       wBitsPerSample  : 1;    // Bits per sample (8 vs. 16, PCM only)
} WAVEBANKMINIWAVEFORMAT, *LPWAVEBANKMINIWAVEFORMAT;

typedef const WAVEBANKMINIWAVEFORMAT *LPCWAVEBANKMINIWAVEFORMAT;

// 
// Wave bank expanded wave format
//

typedef union _WAVEBANKUNIWAVEFORMAT
{
    WAVEFORMATEX        WaveFormatEx;
    XBOXADPCMWAVEFORMAT AdpcmWaveFormat;
} WAVEBANKUNIWAVEFORMAT, *LPWAVEBANKUNIWAVEFORMAT;

typedef const WAVEBANKUNIWAVEFORMAT *LPCWAVEBANKUNIWAVEFORMAT;

//
// Wave bank entry region indices
//

typedef struct _WAVEBANKENTRYREGION
{
    DWORD           dwStart;                // Starting byte offset
    DWORD           dwLength;               // Region length, in bytes
} WAVEBANKENTRYREGION, *LPWAVEBANKENTRYREGION;

typedef const WAVEBANKENTRYREGION *LPCWAVEBANKENTRYREGION;

//
// Wave bank entry meta-data
//

typedef struct _WAVEBANKENTRY
{
    WAVEBANKMINIWAVEFORMAT Format;         // Entry format
    WAVEBANKENTRYREGION    PlayRegion;     // Offsets from the start of the data segment that contains this entry
    WAVEBANKENTRYREGION    LoopRegion;     // Offests relative to the play region that contains the loop region
} WAVEBANKENTRY, *LPWAVEBANKENTRY;

typedef const WAVEBANKENTRY *LPCWAVEBANKENTRY;

//
// Wave bank flags
//

#define WAVEBANK_TYPE_BUFFER            0x00000000
#define WAVEBANK_TYPE_STREAMING         0x00000001
#define WAVEBANK_TYPE_MASK              0x00000001

//
// Wave bank file header
//

#define WAVEBANKHEADER_SIGNATURE        'DNBW'
#define WAVEBANKHEADER_VERSION          2
#define WAVEBANKHEADER_BANKNAME_LENGTH  16

typedef struct _WAVEBANKHEADER
{
    DWORD           dwSignature;                                // File signature
    DWORD           dwVersion;                                  // Version of the tool that created the file
    DWORD           dwFlags;                                    // Wave bank flags (currently unused)
    DWORD           dwEntryCount;                               // Number of entries in the bank
    DWORD           dwAlignment;                                // Entry alignment
    CHAR            szBankName[WAVEBANKHEADER_BANKNAME_LENGTH]; // Bank identifier string
} WAVEBANKHEADER, *LPWAVEBANKHEADER;

typedef const WAVEBANKHEADER *LPCWAVEBANKHEADER;

//
// Wave bank section data
//

typedef struct _WAVEBANKSECTIONDATA
{
    LPWAVEBANKHEADER    pHeader;            // File header
    LPWAVEBANKENTRY     paMetaData;         // Array of entry meta-data
    LPVOID              pvData;             // Wave data base address
    DWORD               dwDataSize;         // Wave data size, in bytes
} WAVEBANKSECTIONDATA, *LPWAVEBANKSECTIONDATA;

typedef const WAVEBANKSECTIONDATA *LPCWAVEBANKSECTIONDATA;

//
// Helper functions
//

EXTERN_C BOOL WaveBankExpandFormat(LPCWAVEBANKMINIWAVEFORMAT pwfxCompressed, LPWAVEBANKUNIWAVEFORMAT pwfxExpanded);
EXTERN_C BOOL WaveBankCompressFormat(LPCWAVEBANKUNIWAVEFORMAT pwfxExpanded, LPWAVEBANKMINIWAVEFORMAT pwfxCompressed);

#ifdef __cplusplus

//
// Wave bank reader object
//

class CWaveBankReader
{
private:
    LPVOID                  m_pvBaseAddress;    // Bank base address
    DWORD                   m_dwBankSize;       // Bank size, in bytes

public:
    CWaveBankReader(void);
    virtual ~CWaveBankReader(void);

public:
    // Initialization
    HRESULT Open(LPCSTR pszBankPath);
    void Flush(void);

    // Bank data
    void GetSectionData(LPWAVEBANKSECTIONDATA pSectionData);
};

#endif // __cplusplus

#endif // __WAVBNDLR_H__
