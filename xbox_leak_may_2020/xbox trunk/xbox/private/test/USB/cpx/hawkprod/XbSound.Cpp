//-----------------------------------------------------------------------------
// File: XBSound.cpp
//
// Desc: Helper class for reading a .wav file and playing it in a DirectSound
//       buffer.
//
// Hist: 12.15.00 - New for December XDK release
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <stdio.h>
#include "XBSound.h"
#include "XBUtil.h"




//-----------------------------------------------------------------------------
// FourCC definitions
//-----------------------------------------------------------------------------
const DWORD FOURCC_RIFF   = 'FFIR';
const DWORD FOURCC_WAVE   = 'EVAW';
const DWORD FOURCC_FORMAT = ' tmf';
const DWORD FOURCC_DATA   = 'atad';




//-----------------------------------------------------------------------------
// Name: CRiffChunk()
// Desc: Object constructor.
//-----------------------------------------------------------------------------
CRiffChunk::CRiffChunk()
{
    // Initialize defaults
    m_fccChunkId   = 0;
    m_pParentChunk = NULL;
    m_hFile        = INVALID_HANDLE_VALUE;
    m_dwDataOffset = 0;
    m_dwDataSize   = 0;
    m_dwFlags      = 0;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initializes the object
//-----------------------------------------------------------------------------
VOID CRiffChunk::Initialize( FOURCC fccChunkId, const CRiffChunk* pParentChunk, 
                             HANDLE hFile )
{
    m_fccChunkId   = fccChunkId;
    m_pParentChunk = pParentChunk;
    m_hFile        = hFile;
}




//-----------------------------------------------------------------------------
// Name: Open()
// Desc: Opens an existing chunk.
//-----------------------------------------------------------------------------
HRESULT CRiffChunk::Open()
{
    RIFFHEADER rhRiffHeader;
    LONG       lOffset = 0;

    // Seek to the first byte of the parent chunk's data section
    if( m_pParentChunk )
    {
        lOffset = m_pParentChunk->m_dwDataOffset;

        // Special case the RIFF chunk
        if( FOURCC_RIFF == m_pParentChunk->m_fccChunkId )
            lOffset += sizeof(FOURCC);
    }
    
    // Read each child chunk header until we find the one we're looking for
    for( ;; )
    {
        if( INVALID_SET_FILE_POINTER == SetFilePointer( m_hFile, lOffset, NULL, FILE_BEGIN ) )
            return HRESULT_FROM_WIN32( GetLastError() );

        DWORD dwRead;
        if( 0 == ReadFile( m_hFile, &rhRiffHeader, sizeof(rhRiffHeader), &dwRead, NULL ) )
            return HRESULT_FROM_WIN32( GetLastError() );

        // Check if we found the one we're looking for
        if( m_fccChunkId == rhRiffHeader.fccChunkId )
        {
            // Save the chunk size and data offset
            m_dwDataOffset = lOffset + sizeof(rhRiffHeader);
            m_dwDataSize   = rhRiffHeader.dwDataSize;

            // Success
            m_dwFlags |= RIFFCHUNK_FLAGS_VALID;

            return S_OK;
        }

        lOffset += sizeof(rhRiffHeader) + rhRiffHeader.dwDataSize;
    }
}




//-----------------------------------------------------------------------------
// Name: Read()
// Desc: Reads from the file
//-----------------------------------------------------------------------------
HRESULT CRiffChunk::ReadData( LONG lOffset, VOID* pData, DWORD dwDataSize )
{
    // Seek to the offset
    DWORD dwPosition = SetFilePointer( m_hFile, m_dwDataOffset+lOffset, NULL, FILE_BEGIN );
    if( INVALID_SET_FILE_POINTER == dwPosition )
        return HRESULT_FROM_WIN32( GetLastError() );

    // Read from the file
    DWORD dwRead;
    if( 0 == ReadFile( m_hFile, pData, dwDataSize, &dwRead, NULL ) )
        return HRESULT_FROM_WIN32( GetLastError() );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile()
// Desc: Object constructor.
//-----------------------------------------------------------------------------
CWaveFile::CWaveFile()
{
    m_hFile = INVALID_HANDLE_VALUE;
}




//-----------------------------------------------------------------------------
// Name: ~CWaveFile()
// Desc: Object destructor.
//-----------------------------------------------------------------------------
CWaveFile::~CWaveFile()
{
    Close();
}




//-----------------------------------------------------------------------------
// Name: Open()
// Desc: Initializes the object.
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Open( const CHAR* strFileName )
{
    // If we're already open, close
    Close();
    
    // Open the file
    m_hFile = CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                          OPEN_EXISTING, 0L, NULL );
    if( INVALID_HANDLE_VALUE == m_hFile )
        return HRESULT_FROM_WIN32( GetLastError() );

    // Initialize the chunk objects
    m_RiffChunk.Initialize( FOURCC_RIFF, NULL, m_hFile );
    m_FormatChunk.Initialize( FOURCC_FORMAT, &m_RiffChunk, m_hFile );
    m_DataChunk.Initialize( FOURCC_DATA, &m_RiffChunk, m_hFile );

    HRESULT hr = m_RiffChunk.Open();
    if( FAILED(hr) )
        return hr;

    hr = m_FormatChunk.Open();
    if( FAILED(hr) )
        return hr;

    hr = m_DataChunk.Open();
    if( FAILED(hr) )
        return hr;

    // Validate the file type
    FOURCC fccType;
    hr = m_RiffChunk.ReadData( 0, &fccType, sizeof(fccType) );
    if( FAILED(hr) )
        return hr;

    if( FOURCC_WAVE != fccType )
        return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetFormat()
// Desc: Gets the wave file format
//-----------------------------------------------------------------------------
HRESULT CWaveFile::GetFormat( WAVEFORMATEX* pwfxFormat, DWORD dwFormatSize )
{
    DWORD dwValidSize = m_FormatChunk.GetDataSize();

    if( NULL == pwfxFormat || 0 == dwFormatSize )
        return E_INVALIDARG;

    // Read the format chunk into the buffer
    HRESULT hr = m_FormatChunk.ReadData( 0, pwfxFormat, min(dwFormatSize, dwValidSize) );
    if( FAILED(hr) )
        return hr;

    // Zero out remaining bytes, in case enough bytes were not read
    if( dwFormatSize > dwValidSize )
        ZeroMemory( (BYTE*)pwfxFormat + dwValidSize, dwFormatSize - dwValidSize );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ReadSample()
// Desc: Reads data from the audio file.
//-----------------------------------------------------------------------------
HRESULT CWaveFile::ReadSample( DWORD dwPosition, VOID* pBuffer, 
                               DWORD dwBufferSize, DWORD* pdwRead )
{                                   
    // Don't read past the end of the data chunk
    DWORD dwDuration;
    GetDuration( &dwDuration );

    if( dwPosition + dwBufferSize > dwDuration )
        dwBufferSize = dwDuration - dwPosition;

    HRESULT hr = S_OK;
    if( dwBufferSize )
        hr = m_DataChunk.ReadData( (LONG)dwPosition, pBuffer, dwBufferSize );

    if( pdwRead )
        *pdwRead = dwBufferSize;

    return hr;
}




//-----------------------------------------------------------------------------
// Name: Close()
// Desc: Closes the object
//-----------------------------------------------------------------------------
VOID CWaveFile::Close()
{
    if( m_hFile != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_hFile );
        m_hFile = INVALID_HANDLE_VALUE;
    }
}




//-----------------------------------------------------------------------------
// Name: CXBSound()
// Desc: 
//-----------------------------------------------------------------------------
CXBSound::CXBSound()
{
    m_pDSoundBuffer = NULL;
    m_dwBufferSize  = 0L;
}




//-----------------------------------------------------------------------------
// Name: ~CXBSound()
// Desc: 
//-----------------------------------------------------------------------------
CXBSound::~CXBSound()
{
    Destroy();
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Creates the sound. Sound is buffered to memory allocated internally
//       by DirectSound.
//-----------------------------------------------------------------------------
HRESULT CXBSound::Create( const CHAR* strFileName, DWORD dwFlags )
{
    // Find the media file
    CHAR strWavePath[512];
    HRESULT   hr;
    if( FAILED( hr = XBUtil_FindMediaFile( strWavePath, strFileName ) ) )
        return hr;

    // Open the .wav file
    CWaveFile waveFile;
    hr = waveFile.Open( strWavePath );
    if( FAILED(hr) )
        return hr;

    // Get the WAVEFORMAT structure for the .wav file
    hr = waveFile.GetFormat( &m_WaveFormat, sizeof(WAVEFORMATEX) );
    if( FAILED(hr) )
        return hr;

    // Get the size of the .wav file
    waveFile.GetDuration( &m_dwBufferSize );

    // Create the sound buffer
    hr = Create( &m_WaveFormat, dwFlags, NULL, m_dwBufferSize );
    if( FAILED(hr) )
        return hr;

    // Lock the buffer so it can be filled
    VOID* pLock1 = NULL;
    VOID* pLock2 = NULL;
    DWORD dwLockSize1 = 0L;
    DWORD dwLockSize2 = 0L;
    hr = m_pDSoundBuffer->Lock( 0L, m_dsbd.dwBufferBytes, &pLock1, &dwLockSize1, 
                                &pLock2, &dwLockSize2, 0L );
    if( FAILED(hr) )
        return hr;

    // Read the wave file data into the buffer
    hr = waveFile.ReadSample( 0L, pLock1, dwLockSize1, NULL );
    if( FAILED(hr) )
        return hr;

    // Unlock the buffer
    hr = m_pDSoundBuffer->Unlock( &pLock1, dwLockSize1, &pLock2, dwLockSize2 );
    if( FAILED(hr) )
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Creates the sound and tells DirectSound where the sound data will be
//       stored. If pBuffer is NULL, DirectSound handles buffer creation.
//-----------------------------------------------------------------------------
HRESULT CXBSound::Create( const WAVEFORMATEX* pwfxFormat, DWORD dwFlags,
                          const VOID* pBuffer, DWORD dwBytes )
{
    // Setup the sound buffer description
    ZeroMemory( &m_dsbd, sizeof(DSBUFFERDESC) );
    m_dsbd.dwSize      = sizeof(DSBUFFERDESC);
    m_dsbd.dwFlags     = dwFlags;
    m_dsbd.lpwfxFormat = (LPWAVEFORMATEX)pwfxFormat;

    // If pBuffer is non-NULL, dwBufferBytes will be zero, which informs
    // DirectSoundCreateBuffer that we will presently be using SetBufferData().
    // Otherwise, we set dwBufferBytes to the size of the WAV data, potentially
    // including alignment bytes.
    if( pBuffer == NULL )
    {
        m_dsbd.dwBufferBytes = ( 0 == m_WaveFormat.nBlockAlign ) ? dwBytes : 
                                 dwBytes - ( dwBytes % m_WaveFormat.nBlockAlign );
    }

    // Create the sound buffer. (Note: on Xbox, we do not need a DSound object)
    Destroy();
	DirectSoundUseLightHRTF();
    HRESULT hr = DirectSoundCreateBuffer( &m_dsbd, &m_pDSoundBuffer);
    if( FAILED(hr) )
        return hr;

    // If buffer specified, tell DirectSound to use it
    if( pBuffer != NULL )
    {
        hr = m_pDSoundBuffer->SetBufferData( (LPVOID)pBuffer, dwBytes );
        if( FAILED(hr) )
            return hr;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroys the resources used by the sound
//-----------------------------------------------------------------------------
VOID CXBSound::Destroy()
{
    SAFE_RELEASE( m_pDSoundBuffer );
}




//-----------------------------------------------------------------------------
// Name: Play()
// Desc: Plays the sound
//-----------------------------------------------------------------------------
HRESULT CXBSound::Play( DWORD dwFlags ) 
{
    if( NULL == m_pDSoundBuffer )
        return E_INVALIDARG;
        
    return m_pDSoundBuffer->Play( 0, 0, dwFlags );
}




//-----------------------------------------------------------------------------
// Name: Stop()
// Desc: Stops the sound
//-----------------------------------------------------------------------------
HRESULT CXBSound::Stop() 
{
    if( NULL == m_pDSoundBuffer )
        return E_INVALIDARG;
        
    return m_pDSoundBuffer->Stop();
}




//-----------------------------------------------------------------------------
// Name: SetPosition()
// Desc: Positions the sound
//-----------------------------------------------------------------------------
HRESULT CXBSound::SetPosition( const D3DXVECTOR3& v ) 
{
    if( NULL == m_pDSoundBuffer )
        return E_INVALIDARG;
        
    return m_pDSoundBuffer->SetPosition( v.x, v.y, v.z, DS3D_IMMEDIATE );
}




//-----------------------------------------------------------------------------
// Name: SetVelocity()
// Desc: Sets the sound's velocity
//-----------------------------------------------------------------------------
HRESULT CXBSound::SetVelocity( const D3DXVECTOR3& v ) 
{
    if( NULL == m_pDSoundBuffer )
        return E_INVALIDARG;
        
    return m_pDSoundBuffer->SetVelocity( v.x, v.y, v.z, DS3D_IMMEDIATE );
}



