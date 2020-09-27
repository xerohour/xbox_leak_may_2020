/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       waveldr.cpp
 *  Content:    Wave file loader.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  06/08/00    dereks  Created.
 *
 ****************************************************************************/

#include "dscommon.h"

// #define WAVELDR_ENABLE_ERRORS
#ifndef WAVELDR_ENABLE_ERRORS

#undef DPF_ERROR
#undef DPF_WARNING

#ifdef DEBUG

#define DPF_ERROR DPF_INFO
#define DPF_WARNING DPF_INFO

#else // DEBUG

#define DPF_ERROR(a)
#define DPF_WARNING(a)

#endif // DEBUG

#endif // WAVELDR_ENABLE_ERRORS

#pragma warning(disable:4035)

static unsigned short __fastcall EndianSwapWord(unsigned short n)
{
    __asm
    {
        mov     eax, ecx
        sar     eax, 8
        and     eax, 0ffh

        mov     ebx, ecx
        shl     ebx, 8
        and     ebx, 0ff00h
        or      eax, ebx
    }
}

static unsigned long __fastcall EndianSwapDword(unsigned long n)
{
    __asm
    {
        mov     eax, ecx
        sar     eax, 24
        and     eax, 0ffh

        mov     ebx, ecx
        sar     ebx, 8
        and     ebx, 0ff00h
        or      eax, ebx

        mov     ebx, ecx
        shl     ebx, 8
        and     ebx, 0ff0000h
        or      eax, ebx

        mov     ebx, ecx
        shl     ebx, 24
        and     ebx, 0ff000000h
        or      eax, ebx
    }
}


/***************************************************************************
 *
 *  CStdFileStream
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
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CStdFileStream::CStdFileStream"

CStdFileStream::CStdFileStream
(
    void
)
{
    DPF_ENTER();

    //
    // Initialize defaults
    //

    m_hFile = NULL;
    m_dwFlags = 0;

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  ~CStdFileStream
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
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CStdFileStream::~CStdFileStream"

CStdFileStream::~CStdFileStream
(
    void
)
{
    DPF_ENTER();

    Close();

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  Open
 *
 *  Description:
 *      Opens the stream.
 *
 *  Arguments:
 *      LPCSTR [in]: file name.
 *      DWORD [in]: access mask.
 *      DWORD [in]: share mask.
 *      DWORD [in]: creation distribution.
 *      DWORD [in]: attribute mask.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CStdFileStream::Open"

HRESULT
CStdFileStream::Open
(
    LPCSTR                  pszFileName, 
    DWORD                   dwDesiredAccess, 
    DWORD                   dwShareMode, 
    DWORD                   dwCreationDisposition, 
    DWORD                   dwFlagsAndAttributes
)
{
    HRESULT                 hr  = S_OK;

    DPF_ENTER();

    ASSERT(!IS_VALID_HANDLE_VALUE(m_hFile));
    
    m_hFile = CreateFile(pszFileName, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL);

    if(INVALID_HANDLE_VALUE == m_hFile)
    {
        m_hFile = NULL;
    }

    if(!m_hFile)
    {
        DPF_ERROR("Error %lu occurred trying to open %s", GetLastError(), pszFileName);
        hr = E_FAIL;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  Attach
 *
 *  Description:
 *      Attaches the stream to an open file handle.
 *
 *  Arguments:
 *      HANDLE [in]: file handle.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CStdFileStream::Attach"

void
CStdFileStream::Attach
(
    HANDLE                  hFile
)
{
    DPF_ENTER();

    ASSERT(!IS_VALID_HANDLE_VALUE(m_hFile));
    
    m_hFile = hFile;
    m_dwFlags |= FILESTREAM_FLAGS_ATTACHED;

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  Close
 *
 *  Description:
 *      Closes the object.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CStdFileStream::Close"

void
CStdFileStream::Close
(
    void
)
{
    DPF_ENTER();
    
    if(m_dwFlags & FILESTREAM_FLAGS_ATTACHED)
    {
        m_hFile = NULL;
        m_dwFlags &= ~FILESTREAM_FLAGS_ATTACHED;
    }
    else
    {
        CLOSE_HANDLE(m_hFile);
    }

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  Read
 *
 *  Description:
 *      Reads from the stream.
 *
 *  Arguments:
 *      LPVOID [in]: read buffer.
 *      DWORD [in]: size of above buffer, in bytes.
 *      LPDWORD [out]: amount read, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CStdFileStream::Read"

HRESULT
CStdFileStream::Read
(
    LPVOID                  pvBuffer,
    DWORD                   dwBufferSize,
    LPDWORD                 pdwRead
)
{
    HRESULT                 hr      = S_OK;
    DWORD                   dwRead;

    DPF_ENTER();
    
    ASSERT(IS_VALID_HANDLE_VALUE(m_hFile));
    
    if(!ReadFile(m_hFile, pvBuffer, dwBufferSize, &dwRead, NULL))
    {
        DPF_ERROR("Error %lu occurred reading from the file", GetLastError());
        hr = E_FAIL;
    }

    if(SUCCEEDED(hr))
    {
        if(pdwRead)
        {
            *pdwRead = dwRead;
        }
        else if(dwRead != dwBufferSize)
        {
            DPF_ERROR("Not enough data was read from the file");
            hr = E_FAIL;
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  Write
 *
 *  Description:
 *      Writes to the stream.
 *
 *  Arguments:
 *      LPCVOID [in]: write buffer.
 *      DWORD [in]: size of above buffer, in bytes.
 *      LPDWORD [out]: amount written, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CStdFileStream::Write"

HRESULT
CStdFileStream::Write
(
    LPCVOID                 pvBuffer,
    DWORD                   dwBufferSize,
    LPDWORD                 pdwWritten
)
{
    HRESULT                 hr          = S_OK;
    DWORD                   dwWritten;

    DPF_ENTER();
    
    ASSERT(IS_VALID_HANDLE_VALUE(m_hFile));
    
    if(!WriteFile(m_hFile, pvBuffer, dwBufferSize, &dwWritten, NULL))
    {
        DPF_ERROR("Error %lu occurred writing to the file", GetLastError());
        hr = E_FAIL;
    }

    if(SUCCEEDED(hr))
    {
        if(pdwWritten)
        {
            *pdwWritten = dwWritten;
        }
        else if(dwWritten != dwBufferSize)
        {
            DPF_ERROR("Not enough data was written from the file");
            hr = E_FAIL;
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  Seek
 *
 *  Description:
 *      Sets the current read or write position in the stream.
 *
 *  Arguments:
 *      LONG [in]: relative offset.
 *      DWORD [in]: offset origin.
 *      LPDWORD [out]: absolute stream position.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CStdFileStream::Seek"

HRESULT
CStdFileStream::Seek
(
    LONG                    lOffset,
    DWORD                   dwOrigin,
    LPDWORD                 pdwAbsolute
)
{
    HRESULT                 hr          = S_OK;
    DWORD                   dwAbsolute;
    
    DPF_ENTER();

    ASSERT(IS_VALID_HANDLE_VALUE(m_hFile));
    
    if((dwAbsolute = SetFilePointer(m_hFile, lOffset, NULL, dwOrigin)) == INVALID_SET_FILE_POINTER)
    {
        DPF_ERROR("Error %lu occurred seeking the file", GetLastError());
        hr = E_FAIL;
    }

    if(SUCCEEDED(hr) && pdwAbsolute)
    {
        *pdwAbsolute = dwAbsolute;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  GetLength
 *
 *  Description:
 *      Gets the file size, in bytes.
 *
 *  Arguments:
 *      LPDWORD [out]: file size, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CStdFileStream::GetLength"

HRESULT
CStdFileStream::GetLength
(
    LPDWORD                 pdwLength
)
{
    HRESULT                 hr  = S_OK;
    
    DPF_ENTER();

    ASSERT(IS_VALID_HANDLE_VALUE(m_hFile));
    
    if((*pdwLength = GetFileSize(m_hFile, NULL)) == -1)
    {
        DPF_ERROR("Error %lu attempting to get the file size", GetLastError());
        hr = E_FAIL;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  CRiffChunk
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      DWORD [in]: chunk identifier.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRiffChunk::CRiffChunk"

CRiffChunk::CRiffChunk
(
    void
)
{
    DPF_ENTER();

    //
    // Initialize defaults
    //

    m_pParentChunk = NULL;
    m_pStream = NULL;
    m_dwChunkId = 0;
    m_dwDataOffset = 0;
    m_dwDataSize = 0;
    m_dwFlags = 0;

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  ~CRiffChunk
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
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRiffChunk::~CRiffChunk"

CRiffChunk::~CRiffChunk
(
    void
)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  Open
 *
 *  Description:
 *      Opens an existing chunk.
 *
 *  Arguments:
 *      CRiffChunk * [in]: parent chunk.
 *      CStdFileStream * [in]: stream.
 *      DWORD [in]: chunk identifier.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRiffChunk::Open"

HRESULT
CRiffChunk::Open
(
    CRiffChunk *            pParentChunk, 
    CStdFileStream *        pStream,
    DWORD                   dwChunkId
)
{
    LONG                    lOffset;
    RIFFHEADER              rhRiffHeader;
    HRESULT                 hr;

    DPF_ENTER();
    
    m_pParentChunk = pParentChunk;
    m_pStream = pStream;

    //
    // Seek to the first byte of the parent chunk's data section
    //

    if(m_pParentChunk)
    {
        lOffset = m_pParentChunk->m_dwDataOffset;

        //
        // Special case the RIFF chunk
        //

        if((WAVELDR_FOURCC_RIFF == m_pParentChunk->m_dwChunkId) || (WAVELDR_FOURCC_FORM == m_pParentChunk->m_dwChunkId))
        {
            lOffset += sizeof(DWORD);
        }
    }
    else
    {
        lOffset = 0;
    }

    //
    // If this is the top level chunk, just read the first 8 bytes.  If not,
    // search the parent chunk until we find the one we're looking for.
    //

    do
    {
        hr = m_pStream->Seek(lOffset, FILE_BEGIN, NULL);

        if(SUCCEEDED(hr))
        {
            hr = m_pStream->Read(&rhRiffHeader, sizeof(rhRiffHeader), NULL);
        }

        if(SUCCEEDED(hr) && m_pParentChunk)
        {
            if(WAVELDR_FOURCC_FORM == m_pParentChunk->m_dwChunkId)
            {
                rhRiffHeader.dwDataSize = EndianSwapDword(rhRiffHeader.dwDataSize);
            }
        }

        if(SUCCEEDED(hr) && (dwChunkId == rhRiffHeader.dwChunkId))
        {
            break;
        }

        if(SUCCEEDED(hr) && !m_pParentChunk)
        {
            hr = E_FAIL;
        }

        if(SUCCEEDED(hr))
        {
            lOffset += sizeof(rhRiffHeader) + rhRiffHeader.dwDataSize;
        }
    }
    while(SUCCEEDED(hr));

    //
    // Success
    //

    if(SUCCEEDED(hr))
    {
        m_dwChunkId = dwChunkId;
        m_dwDataOffset = lOffset + sizeof(rhRiffHeader);
        m_dwDataSize = rhRiffHeader.dwDataSize;
        m_dwFlags |= RIFFCHUNK_FLAGS_VALID;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  Read
 *
 *  Description:
 *      Reads data from the chunk.
 *
 *  Arguments:
 *      DWORD [in]: chunk data position.
 *      LPVOID [out]: data buffer.
 *      DWORD [in]: data size.
 *      LPDWORD [out]: amount read.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRiffChunk::Read"

HRESULT
CRiffChunk::Read
(
    DWORD                   dwOffset, 
    LPVOID                  pvBuffer, 
    DWORD                   dwBufferSize, 
    LPDWORD                 pdwRead
)
{
    HRESULT                 hr  = S_OK;

    DPF_ENTER();

    if(dwOffset + dwBufferSize > m_dwDataSize)
    {
        if(dwOffset >= m_dwDataSize)
        {
            dwBufferSize = 0;
        }
        else
        {
            dwBufferSize = m_dwDataSize - dwOffset;
        }

        if(pdwRead)
        {
            *pdwRead = dwBufferSize;
        }
        else
        {
            DPF_ERROR("Attempted to read past the end of the chunk");
            hr = E_FAIL;
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pStream->Seek(m_dwDataOffset + dwOffset, FILE_BEGIN, NULL);
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pStream->Read(pvBuffer, dwBufferSize, NULL);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  CWaveFile
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
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFile::CWaveFile"

CWaveFile::CWaveFile
(
    void
)
{
    DPF_ENTER();

    m_dwFileType = -1;
    m_pwfxFormat = NULL;

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  ~CWaveFile
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
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFile::~CWaveFile"

CWaveFile::~CWaveFile
(
    void
)
{
    DPF_ENTER();

    Close();

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  Open
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      LPCSTR [in]: file name.
 *      HANDLE [in]: file handle.
 *
 *  Returns:  
 *      HRESULT: result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFile::Open"

HRESULT
CWaveFile::Open
(
    LPCSTR                  pszFileName,
    HANDLE                  hFile
)
{
    HRESULT                 hr              = S_OK;
    DWORD                   dwType;
    CRiffChunk              VersionChunk;
    DWORD                   dwVersion;
    DWORD                   dwFormatSize;
    AIFFSOUNDHDR            AiffSoundHeader;

    DPF_ENTER();

    ASSERT((pszFileName || hFile) && !(pszFileName && hFile));
    
    //
    // If we're already open, close
    //

    Close();

    //
    // Open the file
    //

    if(pszFileName)
    {
        hr = m_Stream.Open(pszFileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, 0);
    }
    else
    {
        m_Stream.Attach(hFile);
    }

    //
    // Determine the file type by opening the top-level chunk
    //

    if(SUCCEEDED(hr))
    {
        hr = m_ParentChunk.Open(NULL, &m_Stream, WAVELDR_FOURCC_RIFF);

        if(SUCCEEDED(hr))
        {
            m_dwFileType = WAVELDR_FILETYPE_WAVE;
        }
        else
        {
            hr = m_ParentChunk.Open(NULL, &m_Stream, WAVELDR_FOURCC_FORM);

            if(SUCCEEDED(hr))
            {
                m_dwFileType = WAVELDR_FILETYPE_AIFF;
            }
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = m_ParentChunk.Read(0, &dwType, sizeof(dwType));

        if(SUCCEEDED(hr))
        {
            if(WAVELDR_FILETYPE_WAVE == m_dwFileType)
            {
                if(WAVELDR_FOURCC_WAVE != dwType)
                {
                    DPF_ERROR("The file type is not WAVE");
                    hr = E_FAIL;
                }
            }
            else
            {
                if((WAVELDR_FOURCC_AIFF != dwType) && (WAVELDR_FOURCC_AIFFC != dwType))
                {
                    DPF_ERROR("The file type is not AIFF/AIFF-C");
                    hr = E_FAIL;
                }
            }
        }
    }

    if(SUCCEEDED(hr) && (WAVELDR_FILETYPE_AIFF == m_dwFileType) && (WAVELDR_FOURCC_AIFFC == dwType))
    {
        hr = VersionChunk.Open(&m_ParentChunk, &m_Stream, WAVELDR_FOURCC_AIFF_VERSION);

        if(SUCCEEDED(hr))
        {
            hr = VersionChunk.Read(0, &dwVersion, sizeof(dwVersion));
        }

        if(SUCCEEDED(hr) && (WAVELDR_AIFFC_VERSION != dwVersion))
        {
            DPF_ERROR("The file's AIFF-C version is not supported");
            hr = E_FAIL;
        }
    }

    //
    // Initialize the required chunk objects
    //

    if(SUCCEEDED(hr))
    {
        hr = m_DataChunk.Open(&m_ParentChunk, &m_Stream, (WAVELDR_FILETYPE_WAVE == m_dwFileType) ? WAVELDR_FOURCC_DATA : WAVELDR_FOURCC_SOUND);
    }

    //
    // Load the file format
    //

    if(SUCCEEDED(hr))
    {
        if(WAVELDR_FILETYPE_WAVE == m_dwFileType)
        {
            hr = GetWaveFormat(NULL, 0, &dwFormatSize);
        }
        else
        {
            hr = GetAiffFormat(NULL, 0, &dwFormatSize);
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = HRFROMP(m_pwfxFormat = (LPWAVEFORMATEX)MEMALLOC(BYTE, dwFormatSize));
    }

    if(SUCCEEDED(hr))
    {
        if(WAVELDR_FILETYPE_WAVE == m_dwFileType)
        {
            hr = GetWaveFormat(m_pwfxFormat, dwFormatSize, NULL);
        }
        else
        {
            hr = GetAiffFormat(m_pwfxFormat, dwFormatSize, NULL);
        }
    }

    //
    // If this is an AIFF file, offset the start of the wave data
    //

    if(SUCCEEDED(hr) && (WAVELDR_FILETYPE_AIFF == m_dwFileType))
    {
        hr = m_DataChunk.Read(0, &AiffSoundHeader, sizeof(AiffSoundHeader));

        if(SUCCEEDED(hr))
        {
            m_DataChunk.m_dwDataOffset += sizeof(AiffSoundHeader) + AiffSoundHeader.dwOffset;
            m_DataChunk.m_dwDataSize -= sizeof(AiffSoundHeader) + AiffSoundHeader.dwOffset;
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  GetFormat
 *
 *  Description:
 *      Gets the wave file format.
 *
 *  Arguments:
 *      LPWAVEFORMATEX [out]: file format.
 *      DWORD [in]: file format buffer size.
 *      LPDWORD [out]: bytes required.
 *
 *  Returns:  
 *      HRESULT: result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFile::GetFormat"

HRESULT
CWaveFile::GetFormat
(
    LPWAVEFORMATEX          pwfxFormat, 
    DWORD                   dwFormatSize, 
    LPDWORD                 pdwRequiredSize
)
{
    DWORD                   dwValidSize;
    HRESULT                 hr;

    DPF_ENTER();

    ASSERT(m_pwfxFormat);
    
    dwValidSize = sizeof(*m_pwfxFormat) + m_pwfxFormat->cbSize;

    if(pwfxFormat && dwFormatSize)
    {
        CopyMemory(pwfxFormat, m_pwfxFormat, min(dwFormatSize, dwValidSize));

        if(dwFormatSize > dwValidSize)
        {
            ZeroMemory((LPBYTE)pwfxFormat + dwValidSize, dwFormatSize - dwValidSize);
        }
    }

    if(pdwRequiredSize)
    {
        *pdwRequiredSize = dwValidSize;
    }

    DPF_LEAVE_HRESULT(S_OK);

    return S_OK;
}


/***************************************************************************
 *
 *  GetWaveFormat
 *
 *  Description:
 *      Gets the wave file format.
 *
 *  Arguments:
 *      LPWAVEFORMATEX [out]: file format.
 *      DWORD [in]: file format buffer size.
 *      LPDWORD [out]: bytes required.
 *
 *  Returns:  
 *      HRESULT: result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFile::GetWaveFormat"

HRESULT
CWaveFile::GetWaveFormat
(
    LPWAVEFORMATEX          pwfxFormat, 
    DWORD                   dwFormatSize, 
    LPDWORD                 pdwRequiredSize
)
{
    CRiffChunk              FormatChunk;
    DWORD                   dwValidSize;
    HRESULT                 hr;

    DPF_ENTER();

    ASSERT(WAVELDR_FILETYPE_WAVE == m_dwFileType);

    //
    // Read the format chunk into the buffer.  Make sure to fill in the full
    // WAVEFORMATEX structure, even if the data in the file isn't enough.
    //

    hr = FormatChunk.Open(&m_ParentChunk, &m_Stream, WAVELDR_FOURCC_FORMAT);

    if(SUCCEEDED(hr))
    {
        dwValidSize = FormatChunk.GetDataSize();

        hr = FormatChunk.Read(0, pwfxFormat, min(dwValidSize, dwFormatSize));
    }

    if(SUCCEEDED(hr) && (dwFormatSize > dwValidSize))
    {
        ZeroMemory((LPBYTE)pwfxFormat + dwValidSize, dwFormatSize - dwValidSize);
    }

    if(SUCCEEDED(hr) && pdwRequiredSize)
    {
        *pdwRequiredSize = max(dwValidSize, sizeof(*pwfxFormat));
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  GetAiffFormat
 *
 *  Description:
 *      Gets the wave file format.
 *
 *  Arguments:
 *      LPWAVEFORMATEX [out]: file format.
 *      DWORD [in]: file format buffer size.
 *      LPDWORD [out]: bytes required.
 *
 *  Returns:  
 *      HRESULT: result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFile::GetAiffFormat"

HRESULT
CWaveFile::GetAiffFormat
(
    LPWAVEFORMATEX          pwfxFormat, 
    DWORD                   dwFormatSize, 
    LPDWORD                 pdwRequiredSize
)
{
    HRESULT                 hr          = S_OK;
    CRiffChunk              FormatChunk;
    DWORD                   dwValidSize;
    AIFFFORMAT              AiffFormat;
    WAVEFORMATEX            WaveFormat;

    DPF_ENTER();

    ASSERT(WAVELDR_FILETYPE_AIFF == m_dwFileType);

    if(pwfxFormat && dwFormatSize)
    {
        //
        // Open the format chunk
        //

        hr = FormatChunk.Open(&m_ParentChunk, &m_Stream, WAVELDR_FOURCC_COMM);
        
        //
        // Read the base format data
        //

        if(SUCCEEDED(hr))
        {
            dwValidSize = FormatChunk.GetDataSize();

            hr = FormatChunk.Read(0, &AiffFormat, min(sizeof(AiffFormat), dwValidSize));
        }

        if(SUCCEEDED(hr) && (sizeof(AiffFormat) > dwValidSize))
        {
            ZeroMemory((LPBYTE)&AiffFormat + dwValidSize, sizeof(AiffFormat) - dwValidSize);
        }

        //
        // If no compression type is specified, assume PCM
        //

        if(SUCCEEDED(hr) && !AiffFormat.dwCompression)
        {
            AiffFormat.dwCompression = WAVELDR_FOURCC_NONE;
        }

        //
        // Currently, only PCM is supported
        //

        if(SUCCEEDED(hr) && (WAVELDR_FOURCC_NONE != AiffFormat.dwCompression))
        {
            DPF_ERROR("Unsupported AIFF compression type");
            hr = E_FAIL;
        }

        //
        // Convert AIFF format to WAVE format
        //

        if(SUCCEEDED(hr))
        {
            AiffFormat.nChannels = EndianSwapWord(AiffFormat.nChannels);
            AiffFormat.wBitsPerSample = EndianSwapWord(AiffFormat.wBitsPerSample);
            AiffFormat.wFrequencyExponent = EndianSwapWord(AiffFormat.wFrequencyExponent);
            AiffFormat.dwFrequencyMantissa = EndianSwapDword(AiffFormat.dwFrequencyMantissa);

            XAudioCreatePcmFormat(AiffFormat.nChannels, AiffFormat.dwFrequencyMantissa >> (16414 - (AiffFormat.wFrequencyExponent & 0x7FFF)), AiffFormat.wBitsPerSample, &WaveFormat);
        }

        if(SUCCEEDED(hr))
        {
            CopyMemory(pwfxFormat, &WaveFormat, min(sizeof(WaveFormat), dwFormatSize));
        }

        if(SUCCEEDED(hr) && (dwFormatSize > sizeof(WaveFormat)))
        {
            ZeroMemory((LPBYTE)pwfxFormat + sizeof(WaveFormat), dwFormatSize - sizeof(WaveFormat));
        }
    }
    
    if(SUCCEEDED(hr) && pdwRequiredSize)
    {
        *pdwRequiredSize = sizeof(WaveFormat);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  GetLoopRegion
 *
 *  Description:
 *      Gets the loop region, if it exists.
 *
 *  Arguments:
 *      LPDWORD [out]: loop start offset, in bytes.
 *      LPDWORD [out]: loop length, in bytes.
 *
 *  Returns:  
 *      HRESULT: result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CStdFileStream::GetLoopRegion"

HRESULT
CWaveFile::GetLoopRegion
(
    LPDWORD                 pdwLoopStart,
    LPDWORD                 pdwLoopLength
)
{
    HRESULT                 hr;

    DPF_ENTER();

    ASSERT(m_pwfxFormat);

    //
    // Read loop region data in samples
    //

    if(WAVELDR_FILETYPE_WAVE == m_dwFileType)
    {
        hr = GetWaveLoopRegion(pdwLoopStart, pdwLoopLength);
    }
    else
    {
        hr = GetAiffLoopRegion(pdwLoopStart, pdwLoopLength);
    }

    //
    // Convert to bytes
    //

    if(SUCCEEDED(hr))
    {
        if(WAVE_FORMAT_XBOX_ADPCM == m_pwfxFormat->wFormatTag)
        {
            *pdwLoopStart /= m_pwfxFormat->nChannels * XBOX_ADPCM_SAMPLES_PER_BLOCK;
            *pdwLoopStart *= m_pwfxFormat->nChannels * XBOX_ADPCM_ENCODED_BLOCK_SIZE;

            *pdwLoopLength /= m_pwfxFormat->nChannels * XBOX_ADPCM_SAMPLES_PER_BLOCK;
            *pdwLoopLength *= m_pwfxFormat->nChannels * XBOX_ADPCM_ENCODED_BLOCK_SIZE;
        }
        else
        {
            *pdwLoopStart *= m_pwfxFormat->nBlockAlign;
            *pdwLoopLength *= m_pwfxFormat->nBlockAlign;
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  GetWaveLoopRegion
 *
 *  Description:
 *      Gets the loop region.
 *
 *  Arguments:
 *      LPDWORD [out]: loop start offset, in samples.
 *      LPDWORD [out]: loop length, in samples.
 *
 *  Returns:  
 *      HRESULT: result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CStdFileStream::GetWaveLoopRegion"

HRESULT
CWaveFile::GetWaveLoopRegion
(
    LPDWORD                 pdwLoopStart,
    LPDWORD                 pdwLoopLength
)
{
    CRiffChunk              InstrumentChunk;
    WAVESAMPLE              wsamp;
    WAVESAMPLE_LOOP         loop;
    DWORD                   dwOffset;
    HRESULT                 hr;

    DPF_ENTER();

    ASSERT(m_pwfxFormat);

    //
    // Open the instrument chunk
    //

    hr = InstrumentChunk.Open(&m_ParentChunk, &m_Stream, WAVELDR_FOURCC_WAVE_SAMPLE);

    //
    // Read the WAVESAMPLE header and first loop region
    //

    if(SUCCEEDED(hr))
    {
        hr = InstrumentChunk.Read(0, &wsamp, sizeof(wsamp));
    }

    //
    // Scan loop regions until we find one that's FORWARD or RELEASE
    //

    if(SUCCEEDED(hr))
    {
        dwOffset = sizeof(wsamp);
    
        while(SUCCEEDED(hr) && wsamp.dwSampleLoops)
        {
            hr = InstrumentChunk.Read(dwOffset, &loop, sizeof(loop));

            if(SUCCEEDED(hr))
            {
                if((WAVESAMPLE_LOOPTYPE_FORWARD == loop.dwLoopType) || (WAVESAMPLE_LOOPTYPE_RELEASE == loop.dwLoopType))
                {
                    break;
                }
            }
        
            dwOffset += sizeof(loop);

            wsamp.dwSampleLoops--;
        }
    }

    if(SUCCEEDED(hr))
    {
        if(!wsamp.dwSampleLoops)
        {
            DPF_ERROR("No FORWARD loop regions found");
            hr = E_FAIL;
        }
        else if(wsamp.dwSampleLoops > 1)
        {
            DPF_WARNING("Only the first FORWARD loop will be used");
        }
    }

    //
    // Success
    //

    if(SUCCEEDED(hr))
    {
        *pdwLoopStart = loop.dwLoopStart;
        *pdwLoopLength = loop.dwLoopLength;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  GetAiffLoopRegion
 *
 *  Description:
 *      Gets the loop region.
 *
 *  Arguments:
 *      LPDWORD [out]: loop start offset, in samples.
 *      LPDWORD [out]: loop length, in samples.
 *
 *  Returns:  
 *      HRESULT: result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CStdFileStream::GetAiffLoopRegion"

HRESULT
CWaveFile::GetAiffLoopRegion
(
    LPDWORD                 pdwLoopStart,
    LPDWORD                 pdwLoopLength
)
{
    LPAIFFMARKERHDR         pMarkers            = NULL;
    CRiffChunk              InstrumentChunk;
    AIFFINSTRUMENT          inst;
    LPAIFFLOOP              pLoop;
    CRiffChunk              MarkerChunk;
    DWORD                   dwMarkerSize;
    AIFFMARKERHDR           MarkerHeader;
    LPAIFFMARKER            pStartMarker;
    LPAIFFMARKER            pEndMarker;
    HRESULT                 hr;
    WORD                    i;

    DPF_ENTER();

    //
    // Open the instrument chunk
    //

    hr = InstrumentChunk.Open(&m_ParentChunk, &m_Stream, WAVELDR_FOURCC_INSTRUMENT);

    //
    // Read the instrument data
    //

    if(SUCCEEDED(hr))
    {
        hr = InstrumentChunk.Read(0, &inst, sizeof(inst));
    }

    //
    // Use the first loop that's FORWARD
    //

    if(SUCCEEDED(hr))
    {
        if(AIFFLOOP_MODE_FORWARD == EndianSwapWord(inst.SustainLoop.wPlayMode))
        {
            pLoop = &inst.SustainLoop;
        }
        else if(AIFFLOOP_MODE_FORWARD == EndianSwapWord(inst.ReleaseLoop.wPlayMode))
        {
            pLoop = &inst.ReleaseLoop;
        }
        else
        {
            DPF_ERROR("Neither loop is FORWARD");
            hr = E_FAIL;
        }
    }

    //
    // Open the marker chunk
    //

    if(SUCCEEDED(hr))
    {
        hr = MarkerChunk.Open(&m_ParentChunk, &m_Stream, WAVELDR_FOURCC_MARKER);
    }

    // 
    // Read marker data
    //

    if(SUCCEEDED(hr))
    {
        dwMarkerSize = MarkerChunk.GetDataSize();
        
        hr = HRFROMP(pMarkers = (LPAIFFMARKERHDR)MEMALLOC(BYTE, dwMarkerSize));
    }

    if(SUCCEEDED(hr))
    {
        hr = MarkerChunk.Read(0, pMarkers, dwMarkerSize);
    }

    if(SUCCEEDED(hr))
    {
        pMarkers->wMarkerCount = EndianSwapWord(pMarkers->wMarkerCount);
    }

    //
    // Find the loop markers
    //

    if(SUCCEEDED(hr))
    {
        pStartMarker = (LPAIFFMARKER)(pMarkers + 1);
        
        for(i = 0; i < pMarkers->wMarkerCount; i++)
        {
            ASSERT((DWORD)pStartMarker < (DWORD)pMarkers + dwMarkerSize);
            
            if(pStartMarker->wMarkerId == pLoop->wStartMarker)
            {
                break;
            }

            pStartMarker = (LPAIFFMARKER)((LPBYTE)pStartMarker + sizeof(*pStartMarker) + pStartMarker->bNameLength);
        }

        if(i >= pMarkers->wMarkerCount)
        {
            DPF_ERROR("Can't find loop start marker");
            hr = E_FAIL;
        }
    }

    if(SUCCEEDED(hr))
    {
        pEndMarker = (LPAIFFMARKER)(pMarkers + 1);
        
        for(i = 0; i < pMarkers->wMarkerCount; i++)
        {
            ASSERT((DWORD)pEndMarker < (DWORD)pMarkers + dwMarkerSize);
            
            if(pEndMarker->wMarkerId == pLoop->wEndMarker)
            {
                break;
            }

            pEndMarker = (LPAIFFMARKER)((LPBYTE)pEndMarker + sizeof(*pEndMarker) + pEndMarker->bNameLength);
        }

        if(i >= pMarkers->wMarkerCount)
        {
            DPF_ERROR("Can't find loop end marker");
            hr = E_FAIL;
        }
    }

    //
    // Double-check the marker validity
    //

    if(SUCCEEDED(hr))
    {
        pStartMarker->dwPosition = EndianSwapDword(pStartMarker->dwPosition);
        pEndMarker->dwPosition = EndianSwapDword(pEndMarker->dwPosition);

        if(pStartMarker->dwPosition >= pEndMarker->dwPosition)
        {
            DPF_ERROR("Loop end point less-than-or-equal-to start point");
            hr = E_FAIL;
        }
    }

    //
    // Success
    //

    if(SUCCEEDED(hr))
    {
        *pdwLoopStart = pStartMarker->dwPosition;
        *pdwLoopLength = pEndMarker->dwPosition - pStartMarker->dwPosition;
    }

    //
    // Clean up
    //
    
    MEMFREE(pMarkers);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  ReadSample
 *
 *  Description:
 *      Reads data from the audio file.
 *
 *  Arguments:
 *      DWORD [in]: file position, in bytes.
 *      LPVOID [out]: data.
 *      DWORD [in]: data size.
 *      LPDWORD [out]: amount read.
 *
 *  Returns:
 *      HRESULT: result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFile::ReadSample"

HRESULT  
CWaveFile::ReadSample
(
    DWORD                   dwPosition, 
    LPVOID                  pvBuffer, 
    DWORD                   dwBufferSize, 
    LPDWORD                 pdwRead
)
{                                   
    HRESULT                 hr;

    DPF_ENTER();

    hr = m_DataChunk.Read(dwPosition, pvBuffer, dwBufferSize, &dwBufferSize);

    if(SUCCEEDED(hr) && (WAVELDR_FILETYPE_AIFF == m_dwFileType))
    {
        ConvertAiffPcm(pvBuffer, dwBufferSize);
    }

    if(SUCCEEDED(hr) && pdwRead)
    {
        *pdwRead = dwBufferSize;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  Close
 *
 *  Description:
 *      Closes the object.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFile::Close"

void
CWaveFile::Close
(
    void
)
{
    DPF_ENTER();

    m_Stream.Close();

    MEMFREE(m_pwfxFormat);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ConvertAiffPcm
 *
 *  Description:
 *      Converts AIFF PCM to standard PCM.
 *
 *  Arguments:
 *      LPVOID [in]: wave data.
 *      DWORD [in]: wave data buffer size, in bytes.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFile::ConvertAiffPcm"

void
CWaveFile::ConvertAiffPcm
(
    LPVOID                  pvSource,
    DWORD                   cbSource
)
{
    BYTE *                  pbSource;
    BYTE                    bTemp;

    DPF_ENTER();

    ASSERT(m_pwfxFormat);
    ASSERT(WAVE_FORMAT_PCM == m_pwfxFormat->wFormatTag);

    ConvertAiffPcm(pvSource, cbSource, m_pwfxFormat->wBitsPerSample);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ConvertAiffPcm
 *
 *  Description:
 *      Converts AIFF PCM to standard PCM.
 *
 *  Arguments:
 *      LPCSOUNDBANKMINIWAVEFORMAT [in]: format.
 *      LPVOID [in]: wave data.
 *      DWORD [in]: wave data buffer size, in bytes.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFile::ConvertAiffPcm"

void
CWaveFile::ConvertAiffPcm
(
    LPVOID                  pvSource,
    DWORD                   cbSource,
    DWORD                   dwBitDepth
)
{
    BYTE *                  pbSource;
    WORD *                  pwSource;
    BYTE                    bTemp;

    DPF_ENTER();

    if(16 == dwBitDepth)
    {
        cbSource /= 2;
        
        for(pwSource = (WORD *)pvSource; cbSource; cbSource--, pwSource++)
        {
            *pwSource = EndianSwapWord(*pwSource);
        }
    }
    else
    {
        ASSERT(8 == dwBitDepth);
        
        for(pbSource = (BYTE *)pvSource; cbSource; cbSource--, pbSource++)
        {
            *pbSource = *pbSource + 0x80;
        }
    }

    DPF_LEAVE_VOID();
}


