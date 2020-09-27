/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       waveldr.h
 *  Content:    Wave file loader.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  06/08/00    dereks  Created.
 *
 ****************************************************************************/

#ifndef __WAVELDR_H__
#define __WAVELDR_H__

#include "macros.h"

#pragma warning(disable:4200)
#pragma pack(push, 1)

//
// Supported file types
//

enum
{
    WAVELDR_FILETYPE_WAVE = 0,
    WAVELDR_FILETYPE_AIFF,
};

//
// Supported RIFF chunk identifiers
//

enum
{
    WAVELDR_FOURCC_RIFF         = 'FFIR',
    WAVELDR_FOURCC_WAVE         = 'EVAW',
    WAVELDR_FOURCC_FORMAT       = ' tmf',
    WAVELDR_FOURCC_DATA         = 'atad',
    WAVELDR_FOURCC_WAVE_SAMPLE  = 'pmsw',
};

//
// Supported AIFF/AIFF-C chunk identifiers
//

enum
{
    WAVELDR_FOURCC_FORM         = 'MROF',
    WAVELDR_FOURCC_AIFF         = 'FFIA',
    WAVELDR_FOURCC_AIFFC        = 'CFIA',
    WAVELDR_FOURCC_AIFF_VERSION = 'REVR',
    WAVELDR_FOURCC_COMM         = 'MMOC',
    WAVELDR_FOURCC_SOUND        = 'DNSS',
    WAVELDR_FOURCC_NONE         = 'ENON',
    WAVELDR_FOURCC_INSTRUMENT   = 'TSNI',
    WAVELDR_FOURCC_MARKER       = 'KRAM'
};

//
// Supported AIFF-C version
//

#define WAVELDR_AIFFC_VERSION   0xA2805140

//
// RIFF header
//

BEGIN_DEFINE_STRUCT()
    DWORD   dwChunkId;
    DWORD   dwDataSize;
END_DEFINE_STRUCT(RIFFHEADER);

//
// Wave instrument data
//

#define WAVESAMPLE_OPTIONS_NOTRUNCATION     0x00000001
#define WAVESAMPLE_OPTIONS_NOCOMPRESSION    0x00000002

#define WAVESAMPLE_LOOPTYPE_FORWARD         0x00000000
#define WAVESAMPLE_LOOPTYPE_RELEASE         0x00000001

BEGIN_DEFINE_STRUCT()
    DWORD   dwSize;
    DWORD   dwLoopType;
    DWORD   dwLoopStart;
    DWORD   dwLoopLength;
END_DEFINE_STRUCT(WAVESAMPLE_LOOP);

BEGIN_DEFINE_STRUCT()
    DWORD           dwSize;
    WORD            wUnityNote;
    SHORT           nFineTune;
    LONG            lGain;
    DWORD           dwOptions;
    DWORD           dwSampleLoops;
    WAVESAMPLE_LOOP aLoops[0];
END_DEFINE_STRUCT(WAVESAMPLE);

//
// AIFF/AIFF-C format data
//

BEGIN_DEFINE_STRUCT()
    WORD    nChannels;
    DWORD   dwSampleCount;
    WORD    wBitsPerSample;
    WORD    wFrequencyExponent;
    DWORD   dwFrequencyMantissa;
    WORD    wReserved;
    DWORD   dwCompression;
END_DEFINE_STRUCT(AIFFFORMAT);

//
// AIFF/AIFF-C sound data header
//

BEGIN_DEFINE_STRUCT()
    DWORD   dwOffset;
    DWORD   dwBlockSize;
END_DEFINE_STRUCT(AIFFSOUNDHDR);

//
// AIFF/AIFF-C instrument data
//

#define AIFFLOOP_MODE_NOLOOPING 0x0000
#define AIFFLOOP_MODE_FORWARD   0x0001
#define AIFFLOOP_MODE_BACKWARD  0x0002

BEGIN_DEFINE_STRUCT()
    WORD    wPlayMode;
    WORD    wStartMarker;
    WORD    wEndMarker;
END_DEFINE_STRUCT(AIFFLOOP);

BEGIN_DEFINE_STRUCT()
    BYTE        bBaseNote;
    BYTE        bDetune;
    BYTE        bLowNote;
    BYTE        bHighNote;
    BYTE        bLowVelocity;
    BYTE        bHighVelocity;
    SHORT       nGain;
    AIFFLOOP    SustainLoop;
    AIFFLOOP    ReleaseLoop;
END_DEFINE_STRUCT(AIFFINSTRUMENT);

//
// AIFF/AIFF-C marker data
//

BEGIN_DEFINE_STRUCT()
    WORD        wMarkerId;
    DWORD       dwPosition;
    BYTE        bNameLength;
    char        szMarkerName[1];
END_DEFINE_STRUCT(AIFFMARKER);

BEGIN_DEFINE_STRUCT()
    WORD        wMarkerCount;
END_DEFINE_STRUCT(AIFFMARKERHDR);

//
// Chunk flags
//

#define RIFFCHUNK_FLAGS_VALID       0x00000001      // Is the chunk valid?

//
// File stream flags
//

#define FILESTREAM_FLAGS_ATTACHED   0x00000001      // Do we own the file handle?

#pragma pack(pop)

#ifdef __cplusplus

//
// Standard file stream
//

namespace WaveLoader
{
    class CStdFileStream
    {
    protected:
        HANDLE                  m_hFile;            // File handle
        DWORD                   m_dwFlags;          // Flags

    public:
        CStdFileStream(void);
        ~CStdFileStream(void);

    public:
        // Initialization
        HRESULT Open(LPCSTR pszFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes);
        void Attach(HANDLE hFile);
        void Close(void);

        // I/O
        HRESULT Read(LPVOID pvBuffer, DWORD dwBufferSize, LPDWORD pdwRead = NULL);
        HRESULT Write(LPCVOID pvBuffer, DWORD dwBufferSize, LPDWORD pdwWritten = NULL);
        HRESULT Seek(LONG lOffset, DWORD dwOrigin, LPDWORD pdwAbsolute = NULL);

        // Properties
        HRESULT GetLength(LPDWORD pdwLength);

    public:
        operator HANDLE(void);
    };

    __inline CStdFileStream::operator HANDLE(void)
    {
        return m_hFile;
    }
}

//
// RIFF chunk utility class
//

namespace WaveLoader
{
    class CRiffChunk
    {
        friend class CWaveFile;

    protected:
        CRiffChunk *            m_pParentChunk;                 // Parent chunk
        CStdFileStream *        m_pStream;                      // File stream
        DWORD                   m_dwChunkId;                    // Chunk identifier
        DWORD                   m_dwDataOffset;                 // Chunk data offset
        DWORD                   m_dwDataSize;                   // Chunk data size
        DWORD                   m_dwFlags;                      // Chunk flags

    public:
        CRiffChunk(void);
        ~CRiffChunk(void);

    public:
        // Initialization
        HRESULT Open(CRiffChunk *pParentChunk, CStdFileStream *pStream, DWORD dwChunkId);

        // Data
        HRESULT Read(DWORD dwOffset, LPVOID pvBuffer, DWORD dwBufferSize, LPDWORD pdwRead = NULL);

        // Chunk information
        DWORD GetChunkId(void);
        DWORD GetDataOffset(void);
        DWORD GetDataSize(void);
        BOOL IsValid(void);
    };

    __inline DWORD CRiffChunk::GetChunkId(void)
    {
        return m_dwChunkId;
    }

    __inline BOOL CRiffChunk::IsValid(void)
    {
        return !!(m_dwFlags & RIFFCHUNK_FLAGS_VALID);
    }

    __inline DWORD CRiffChunk::GetDataOffset(void)
    {
        return m_dwDataOffset;
    }

    __inline DWORD CRiffChunk::GetDataSize(void)
    {
        return m_dwDataSize;
    }
}

//
// Wave file utility class
//

namespace WaveLoader
{
    class CWaveFile
    {
    protected:
        CStdFileStream          m_Stream;           // File stream
        CRiffChunk              m_ParentChunk;      // Top-level chunk
        CRiffChunk              m_DataChunk;        // Data chunk
        DWORD                   m_dwFileType;       // File type
        LPWAVEFORMATEX          m_pwfxFormat;       // File format
    
    public:
        CWaveFile(void);
        ~CWaveFile(void);

    public:
        // Initialization
        HRESULT Open(LPCSTR pszFileName);
        HRESULT Open(HANDLE hFile);
        void Close(void);

        // File format
        HRESULT GetFormat(LPWAVEFORMATEX pwfxFormat, DWORD dwFormatSize, LPDWORD pdwRequired = NULL);
        HRESULT GetFormat(LPCWAVEFORMATEX *ppwfxFormat);

        // File data
        HRESULT ReadSample(DWORD dwPosition, LPVOID pvBuffer, DWORD dwBufferSize, LPDWORD pdwRead = NULL);
        void ConvertAiffPcm(LPVOID pvSource, DWORD cbSource);
        static void ConvertAiffPcm(LPVOID pvSource, DWORD cbSource, DWORD dwBitDepth);

        // File properties
        HRESULT GetFileType(LPDWORD pdwType);
        HRESULT GetDataOffset(LPDWORD pdwDataOffset);
        HRESULT GetDuration(LPDWORD pdwDuration);
        HRESULT GetLoopRegion(LPDWORD pdwLoopStart, LPDWORD pdwLoopLength);

    protected:
        // Initialization
        HRESULT Open(LPCSTR pszFileName, HANDLE hFile);
    
        // File format
        HRESULT GetWaveFormat(LPWAVEFORMATEX pwfxFormat, DWORD dwFormatSize, LPDWORD pdwRequired);
        HRESULT GetAiffFormat(LPWAVEFORMATEX pwfxFormat, DWORD dwFormatSize, LPDWORD pdwRequired);

        // Loop points
        HRESULT GetWaveLoopRegion(LPDWORD pdwLoopStart, LPDWORD pdwLoopLength);
        HRESULT GetAiffLoopRegion(LPDWORD pdwLoopStart, LPDWORD pdwLoopLength);
    };

    __inline HRESULT CWaveFile::Open(LPCSTR pszFileName)
    {
        return Open(pszFileName, NULL);
    }

    __inline HRESULT CWaveFile::Open(HANDLE hFile)
    {
        return Open(NULL, hFile);
    }

    __inline HRESULT CWaveFile::GetFileType(LPDWORD pdwType)
    {
        *pdwType = m_dwFileType;
        return S_OK;
    }

    __inline HRESULT CWaveFile::GetDataOffset(LPDWORD pdwDataOffset)
    {
        *pdwDataOffset = m_DataChunk.GetDataOffset();
        return S_OK;
    }

    __inline HRESULT CWaveFile::GetDuration(LPDWORD pdwDuration)
    {
        *pdwDuration = m_DataChunk.GetDataSize();
        return S_OK;
    }

    __inline HRESULT CWaveFile::GetFormat(LPCWAVEFORMATEX *ppwfxFormat)
    {
        *ppwfxFormat = m_pwfxFormat;
        return S_OK;
    }
}

using namespace WaveLoader;

#endif // __cplusplus

#endif // __WAVELDR_H__
