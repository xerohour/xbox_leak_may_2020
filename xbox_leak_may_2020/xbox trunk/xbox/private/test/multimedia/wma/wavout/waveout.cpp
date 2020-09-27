///////////////////////////////////////////////////////////////////////////////
//
// File: waveout.cpp
//
// Date: 2/16/2001
//
// Author: danrose
//
// Purpose: Implementation of the Wave File Writer XMO
//
///////////////////////////////////////////////////////////////////////////////

#include "waveout.h"

///////////////////////////////////////////////////////////////////////////////
//
// Function: XWaveOutCreateMediaObject
// 
// Input: pszFileName   - the name of the file that will be created and written
//        pwfxFormat    - the format of the wavedata in memory
//        ppMediaObject - the created WaveOut XMO
//
// Output: a standard HRESULT
//
// Purpose: to make creation of the WaveOut XMO very easy
//
///////////////////////////////////////////////////////////////////////////////

HRESULT XWaveOutCreateMediaObject( LPCSTR pszFileName, LPCWAVEFORMATEX pwfxFormat, XMediaObject** ppMediaObject )
{
	// Check the inputted parameters

	if ( NULL == pszFileName || NULL == pwfxFormat || NULL == ppMediaObject )
		return E_POINTER;

	// Allocate the return HR and the XMO

    CWaveOutMediaObject* pMediaObject = NULL;
    HRESULT hr = S_OK;
    
	// Create the XMO, check for failures

    pMediaObject = new CWaveOutMediaObject();

	if ( NULL == pMediaObject )
		hr = E_OUTOFMEMORY;

	// Initalize the XMO with the inputted file name and wave format

    if( SUCCEEDED( hr ) )
    {
        hr = pMediaObject->Initialize( pszFileName, pwfxFormat );
    }

	// Set the output XMO pointer to the created one

    if( SUCCEEDED( hr ) )
    {
        *ppMediaObject = pMediaObject;
    }

	// If an error occured but the XMO was created, release it

	if ( FAILED( hr ) && NULL != pMediaObject )
	{
		pMediaObject->Release();
	}

    // Return the final Hr

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: CWaveOut XMO Constructor
//
// Input: None
//
// Output: None
//
// Purpose: allocate all stack variables, and zero them out
//
///////////////////////////////////////////////////////////////////////////////

CWaveOutMediaObject::CWaveOutMediaObject( void )
	: m_wfx(),
	  m_ulRefCount( 1 ),
	  m_dwBufferBytesWritten( 0 ),
	  m_dwChunkSize( 0 ),
	  m_hFile( NULL )
{
	ZeroMemory( &m_wfx, sizeof( WAVEFORMATEX ) );
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: CWaveOut XMO Destructor
//
// Input: None
//
// Output: None
//
// Purpose: Close the allocated file handle if no errors had occured
//
///////////////////////////////////////////////////////////////////////////////
    
CWaveOutMediaObject::~CWaveOutMediaObject( void )
{
	if ( NULL != m_hFile && INVALID_HANDLE_VALUE != m_hFile )
	{
		CloseHandle( m_hFile );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: CWaveOut XMO Initialize
//
// Input: pszFileName - the name of the wave file to be created
//        pwfxFormat  - the wave format of the buffer that will be passed in
//
// Output: a standard HRESULT
//
// Purpose: to set up the Wave out xmo by saving the waveformat, creating the
//          underlying file handle, and writing out the wave header to the file
//
///////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE CWaveOutMediaObject::Initialize( LPCSTR pszFileName, LPCWAVEFORMATEX pwfxFormat )
{
	// Check the parameters passed in

	if ( NULL == pszFileName || NULL == pwfxFormat )
		return E_POINTER;

	HRESULT hr = S_OK;
	
	// Copy over the wave format

	memcpy( &m_wfx, pwfxFormat, sizeof( WAVEFORMATEX ) );

	// create the file handle, check for errors

	m_hFile = CreateFile( pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( INVALID_HANDLE_VALUE == m_hFile )
	{
		hr = E_FAIL;
	}

	// Write out the wave header

	if ( SUCCEEDED( hr ) )
	{
		hr = WriteWaveHeader();
	}

	// Close the file handle if an error occured

	if ( FAILED( hr ) || NULL == m_hFile || INVALID_HANDLE_VALUE == m_hFile )
	{
		CloseHandle( m_hFile );
	}

	// return the final hr

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: CWaveOut XMO AddRef
//
// Input: None
//
// Output: the new reference count
//
// Purpose: to reference count the XMO
//
///////////////////////////////////////////////////////////////////////////////

__inline ULONG CWaveOutMediaObject::AddRef( void )
{
    return ++m_ulRefCount;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: CWaveOut XMO Release
//
// Input: None
//
// Output: the new reference count
//
// Purpose: to reference count the XMO
//
///////////////////////////////////////////////////////////////////////////////

__inline ULONG CWaveOutMediaObject::Release( void )
{
    if( m_ulRefCount )
    {
        if( !--m_ulRefCount )
        {
            delete this;
            return 0;
        }
    }

    return m_ulRefCount;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: CWaveOut XMO Discontinuity
//
// Input: None
//
// Output: always S_OK
//
// Purpose: not needed, just a stub
//
///////////////////////////////////////////////////////////////////////////////

__inline HRESULT CWaveOutMediaObject::Discontinuity( void )
{
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: CWaveOut XMO Flush
//
// Input: None
//
// Output: always S_OK
//
// Purpose: not needed, just a stub
//
///////////////////////////////////////////////////////////////////////////////

__inline HRESULT CWaveOutMediaObject::Flush( void )
{
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: CWaveOut XMO GetStatus
//
// Input: pdwStatus - a pointer to a dword that will contain the status
//
// Output: either E_POINTER or S_OK
//
// Purpose: to find out the status of the XMO
//
///////////////////////////////////////////////////////////////////////////////

__inline HRESULT CWaveOutMediaObject::GetStatus( LPDWORD pdwStatus )
{

	// Check for invalid params

	if ( NULL == pdwStatus )
		return E_POINTER;

	// set the status to be acccept input data since this is a 
	// rendering sync XMO

    *pdwStatus = XMO_STATUSF_ACCEPT_INPUT_DATA;

	// always return S_OK if we've made it here

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: CWaveOut XMO GetInfo
//
// Input: pInfo - a pointer ot the XMO info structure
//
// Output: either S_OK or E_POINTER
//
// Purpose: to get properties of the xmo to build a valid XMO graph
//
///////////////////////////////////////////////////////////////////////////////

HRESULT CWaveOutMediaObject::GetInfo( LPXMEDIAINFO pInfo )
{

	// Check for bad params

	if ( NULL == pInfo )
		return E_POINTER;

	// setup the info structure. We only take WHOLE, FIXED size samples
	// there is no lookahead
	// no input size
	// and the outputsize is whatever the block alignment of the waveform is

    pInfo->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE;
    pInfo->dwMaxLookahead = 0;
    pInfo->dwInputSize = 0;
    pInfo->dwOutputSize = m_wfx.nBlockAlign;

	// always return S_OK if the param was good

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: CWaveOut XMO Process
//
// Input: pxmbInput  - the input media packet
//        pxmbOutput - the output media packet, always NULL
//
// Output: a valid HRESULT
//
// Purpose: the actual work horse function. Process is called every time
//          new data is ready to be written out to the wave file
//
///////////////////////////////////////////////////////////////////////////////

HRESULT CWaveOutMediaObject::Process( LPCXMEDIAPACKET pxmbInput, LPCXMEDIAPACKET pxmbOutput )
{
	// Check the paramters

	if ( NULL != pxmbOutput || NULL == pxmbInput || NULL == pxmbInput->pvBuffer )
		return E_INVALIDARG;
		
	// allocate stack variables

	HRESULT hr = S_OK;

	BOOL bMoved = TRUE;
	
	DWORD dwBytesWritten = 0;
	DWORD dwWritten = 0;

	// write the inputted buffer to the file, check for failure

	DWORD dwWrite = WriteFile( m_hFile, pxmbInput->pvBuffer, pxmbInput->dwMaxSize, &dwBytesWritten, NULL );

	if ( 0 == dwWrite )
	{
		hr = E_FAIL;
	}

	// update the number of bytes written and the chunk size

	m_dwBufferBytesWritten += dwBytesWritten;
    m_dwChunkSize += dwBytesWritten;

	// Move the file pointer to the location in the header to update the data chunk size
	// and check for failure
	
	if ( SUCCEEDED( hr ) )
	{
		bMoved = SetFilePointer( m_hFile, 4, NULL, FILE_BEGIN );
	}

	if ( !bMoved )
	{
		hr = E_FAIL;
	}

	// Write out the new data chunk size, chekc for any failures

	if ( SUCCEEDED( hr ) )
	{
		dwWrite = WriteFile( m_hFile, &m_dwBufferBytesWritten, sizeof( DWORD ), &dwWritten, NULL );
	}

	if ( 0 == dwWrite )
	{
		hr = E_FAIL;
	}

	// more the file pointer to the location of the WAVE chunk size, and check for failure

	if ( SUCCEEDED( hr ) )
	{
		bMoved = SetFilePointer( m_hFile, 40, NULL, FILE_BEGIN );
	}

	if ( !bMoved )
	{
		hr = E_FAIL;
	}

	// write the new chunk size to the file, and check for failure

	if ( SUCCEEDED( hr ) )
	{
		dwWrite = WriteFile( m_hFile, &m_dwChunkSize, sizeof( DWORD ), &dwWritten, NULL );
	}

	if ( 0 == dwWrite )
	{
		hr = E_FAIL;
	}

	// move the file pointer to the end of the file, for the next call to process, chekc for failure

	if ( SUCCEEDED( hr ) )
	{
		bMoved = SetFilePointer( m_hFile, 0, NULL, FILE_END );
	}

	if ( !bMoved )
	{
		hr = E_FAIL;
	}

	// if the completed size, status and / or completion event are desired, act on them

	if ( pxmbInput->pdwCompletedSize )
	{
		*pxmbInput->pdwCompletedSize = dwBytesWritten;
	}

	if ( pxmbInput->pdwStatus )
	{
		*pxmbInput->pdwStatus = SUCCEEDED( hr ) ? XMEDIAPACKET_STATUS_SUCCESS : XMEDIAPACKET_STATUS_FAILURE;
	}

	if ( pxmbInput->hCompletionEvent )
	{
		SetEvent( pxmbInput->hCompletionEvent );
	}

	// return the final hr

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: CWaveOut XMO WriteWaveHeader
//
// Input: None
//
// Output: a valid HRESULT
//
// Purpose: to calculate and write out the wave file header
//
///////////////////////////////////////////////////////////////////////////////

HRESULT CWaveOutMediaObject::WriteWaveHeader( void )
{

// set up the compiler to pack on byte boundries

#pragma pack( push )
#pragma pack( 1 )

	// A local variable that represnts the header of a wave file

    struct
    {

		// first the Riff Chunk

        struct
        {
            DWORD           dwChunkId;
            DWORD           dwChunkSize;
            DWORD           dwRiffType;
        } RiffChunk;

		// then the format chunk

        struct
        {
            DWORD           dwChunkId;
            DWORD           dwChunkSize;
            
			// witch has an embedded wave format

            struct
            {
                WORD        wFormatTag;
                WORD        nChannels;
                DWORD       nSamplesPerSec;
                DWORD       nAvgBytesPerSec;
                WORD        nBlockAlign;
                WORD        wBitsPerSample;
            } PcmWaveFormat;
                
        } FormatChunk;

		// and finaly the data chunk

        struct
        {
            DWORD           dwChunkId;
            DWORD           dwChunkSize;
        } DataChunk;

    } RiffHeader;

#pragma pack( pop )

	// Set up the Data chunk

    RiffHeader.DataChunk.dwChunkId = 'atad';
    RiffHeader.DataChunk.dwChunkSize = 0;

	// then the Format Chunk preamble

    RiffHeader.FormatChunk.dwChunkId = ' tmf';
    RiffHeader.FormatChunk.dwChunkSize = sizeof( RiffHeader.FormatChunk.PcmWaveFormat );

	// then the wave format

    RiffHeader.FormatChunk.PcmWaveFormat.wFormatTag = m_wfx.wFormatTag;
    RiffHeader.FormatChunk.PcmWaveFormat.nChannels = m_wfx.nChannels;
    RiffHeader.FormatChunk.PcmWaveFormat.nSamplesPerSec = m_wfx.nSamplesPerSec;
    RiffHeader.FormatChunk.PcmWaveFormat.nAvgBytesPerSec = m_wfx.nAvgBytesPerSec;
    RiffHeader.FormatChunk.PcmWaveFormat.nBlockAlign = m_wfx.nBlockAlign;
    RiffHeader.FormatChunk.PcmWaveFormat.wBitsPerSample = m_wfx.wBitsPerSample;

	// then the riff chunk

    RiffHeader.RiffChunk.dwChunkId = 'FFIR';
    RiffHeader.RiffChunk.dwChunkSize = sizeof( RiffHeader.RiffChunk.dwRiffType ) + sizeof( RiffHeader.FormatChunk ) + RiffHeader.FormatChunk.dwChunkSize + sizeof( RiffHeader.DataChunk ) + RiffHeader.DataChunk.dwChunkSize;
	RiffHeader.RiffChunk.dwRiffType = 'EVAW';

	// write out the header to the file

	DWORD dwWritten = 0;
    DWORD dwWrite = WriteFile( m_hFile, &RiffHeader, sizeof( RiffHeader ), &dwWritten, NULL );

	// and decide if everything went ok

	return 0 == dwWrite ? E_FAIL : S_OK;
}



