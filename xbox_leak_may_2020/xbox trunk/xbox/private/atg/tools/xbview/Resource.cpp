//-----------------------------------------------------------------------------
//  
//  File: Resource.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Resource handling for effect, skeleton, and animation.
//  
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xgmath.h>
#include <xgraphics.h>
#include "Resource.h"

//-----------------------------------------------------------------------------
// Name: SizeOfResource()
// Desc: Determines the size, in bytes, of the D3DResource pointed to by 
//       pRes.
//-----------------------------------------------------------------------------
static DWORD SizeOfResource( LPDIRECT3DRESOURCE8 pResource )
{
    switch( pResource->GetType() )
    {
        case D3DRTYPE_TEXTURE:
            return sizeof(D3DTexture);
        case D3DRTYPE_VOLUMETEXTURE:
            return sizeof(D3DVolumeTexture);
        case D3DRTYPE_CUBETEXTURE:
            return sizeof(D3DCubeTexture);
        case D3DRTYPE_VERTEXBUFFER:
            return sizeof(D3DVertexBuffer);
        case D3DRTYPE_INDEXBUFFER:
            return sizeof(D3DIndexBuffer);
        case D3DRTYPE_PALETTE:
            return sizeof(D3DPalette);
        default:
            return 0;
    }
}


//-----------------------------------------------------------------------------
// Name: Resource()
// Desc: Constructor
//-----------------------------------------------------------------------------
Resource::Resource()
{
	m_pd3dDevice    = NULL;
    m_cbHeaders     = 0;
	m_cbData        = 0;
	m_LoadingState = LOADING_NOTSTARTED;
}

//-----------------------------------------------------------------------------
// Name: ~Resource()
// Desc: Destructor
//-----------------------------------------------------------------------------
Resource::~Resource()
{
	Unload();
	if (m_pd3dDevice != NULL)
		m_pd3dDevice->Release();
}

//-----------------------------------------------------------------------------
// Name: PollLoadingState()
// Desc: Check for IO completion
//-----------------------------------------------------------------------------
LOADINGSTATE Resource::PollLoadingState()
{
    if ( m_LoadingState == LOADING_HEADER
		 || m_LoadingState == LOADING_DATA )
	{
		if ( HasOverlappedIoCompleted( &m_overlapped ) )
		{
			HRESULT hr = OnIOComplete();
			if (FAILED(hr))
				m_LoadingState = LOADING_FAILED;
		}
	}
	return m_LoadingState;
}


//-----------------------------------------------------------------------------
// Name: StartLoading()
// Desc: Loads all the resources from the given XBR. ppResources should 
//       be large enough to hold all the LPDIRECT3DRESOURCE8 pointers.  
//       The read is performed asynchronously, so the data isn't available
//       until OnIOComplete().
//-----------------------------------------------------------------------------
HRESULT Resource::StartLoading( LPDIRECT3DDEVICE8 pDevice, LPSTR strFileName )
{
    HRESULT     hr = S_OK;
    XPR_HEADER  xprh;
    HANDLE      hfHeader = INVALID_HANDLE_VALUE;
    DWORD       cb;

	// Make sure previous resources are unloaded
	Unload();

	// Set D3D device
	LPDIRECT3DDEVICE8 pDevicePrevious = m_pd3dDevice;
	m_pd3dDevice = pDevice;
	if (m_pd3dDevice)
		m_pd3dDevice->AddRef();
	if (pDevicePrevious != NULL)
		pDevicePrevious->Release();
	
    // Read out the headers first
    hfHeader = CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                           OPEN_EXISTING, 0, NULL );
    if( hfHeader == INVALID_HANDLE_VALUE )
    {
        hr = E_FAIL;
        goto Done;
    }

    // Verify the XPR magic header
    if( !ReadFile( hfHeader, &xprh, sizeof( XPR_HEADER), &cb, NULL ) )
	{
		hr = E_FAIL; 
		goto Done;
	}
    if( xprh.dwMagic != XPR_MAGIC_VALUE )
    {
        hr = E_INVALIDARG;
        goto Done;
    }

	// TODO: make header reading asynchronous, too, with LOADING_HEADER state
	
    // Allocate memory for the headers
    m_cbHeaders = xprh.dwHeaderSize - 3 * sizeof( DWORD );
    m_pSysMemData = new BYTE[m_cbHeaders];
    if( !m_pSysMemData )
    {
        hr = E_OUTOFMEMORY;
        goto Done;
    }

    // Read in the headers
    if( !ReadFile( hfHeader, m_pSysMemData, m_cbHeaders, &cb, NULL ) )
	{
		hr = E_FAIL; 
		goto Done;
	}
    CloseHandle( hfHeader );

    // Now read the data
    // File is opened with overlapped i/o and no buffering
    m_hfXPR = CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                          OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL );
    if( m_hfXPR == INVALID_HANDLE_VALUE )
    {
        hr = E_FAIL;
        goto Done;
    }

    // Allocate contiguous memory for the texture data
    m_cbData = xprh.dwTotalSize - xprh.dwHeaderSize;
    m_pVidMemData = (BYTE *)D3D_AllocContiguousMemory( m_cbData, D3DTEXTURE_ALIGNMENT );
    if( !m_pVidMemData )
    {
        hr = E_OUTOFMEMORY;
        goto Done;
    }

    // Set up our overlapped i/o struct
    ZeroMemory( &m_overlapped, sizeof( OVERLAPPED ) );
    m_overlapped.Offset = xprh.dwHeaderSize;

    // Start the read of the texture data
    if( !ReadFile( m_hfXPR, m_pVidMemData, m_cbData, &cb, &m_overlapped ) )
    {
        if( GetLastError() == ERROR_IO_PENDING )
			m_LoadingState = LOADING_DATA;
        else
        {
            // Error we weren't expecting
            hr = E_FAIL;
            goto Done;
        }
    }

Done:
    // Lots of cleanup to do
    if( INVALID_HANDLE_VALUE != hfHeader )
        CloseHandle( hfHeader );

    // If we had an error condition, we need to 
    // free memory and close the XPR file
    if( FAILED( hr ) )
    {
        m_LoadingState = LOADING_FAILED;
        if( INVALID_HANDLE_VALUE != m_hfXPR )
            CloseHandle( m_hfXPR );

        delete[] m_pSysMemData;
        if( m_pVidMemData )
            D3D_FreeContiguousMemory( m_pVidMemData );
    }

    return hr;
}


//-----------------------------------------------------------------------------
// Name: Unload()
// Desc: Cleans up resource state that is external to the bulk allocation.
//-----------------------------------------------------------------------------
HRESULT Resource::Unload()
{
	HRESULT hr = S_OK;

	if (m_LoadingState == LOADING_HEADER
		|| m_LoadingState == LOADING_DATA )
	{
		// Abort loading
		CloseHandle( m_hfXPR );
		m_hfXPR = INVALID_HANDLE_VALUE;
		m_LoadingState = LOADING_NOTSTARTED;
	}

    // Loop over resources, unloading as needed
    for( UINT i = 0; i < m_dwNumResources; i++ )
    {
		DWORD dwType = m_dwTypes[i];
		BYTE *pResource = m_ppResources[i];
		HRESULT hr2 = this->Cleanup(dwType, pResource);
		if (FAILED(hr2))
			hr = hr2;	// keep final result

	}
	Destroy();		// call CXBPackedResource cleanup routine
	return hr;
}

//-----------------------------------------------------------------------------
// Name: OnIOComplete()
// Desc: Called when async i/o is complete, so that we can copy the
//       texture data to video memory and register the resources.
//-----------------------------------------------------------------------------
HRESULT Resource::OnIOComplete()
{
	HRESULT hr = S_OK;
    BYTE      * pHeader;

    // Loop over resources, calling Register()
    pHeader = m_pSysMemData;
    for( int i = 0; i < MAX_NUM_RESOURCES; i++ )
    {
		DWORD dwType = *(DWORD*)pHeader;
		// Check for end token
		if ( dwType == 0xffffffff )
			break;
		// Check for userdata, which has size in header
		else if ( dwType & 0x80000000 )
		{
			DWORD dwSize = ((DWORD*)pHeader)[1];
			pHeader += 8;
			
			m_ppResources[i] = pHeader;	// pointer is to actual user data, not wrapper type and size
			m_dwTypes[i] = dwType;		// keep the type in a separate array
			
			// Special handling of user data, such as patching of file offsets to memory offsets, etc.
			hr = this->Patch(dwType, pHeader);
			if (FAILED(hr))
				break;
			pHeader += dwSize;
		}
		else
		{
			// Standard Resource
			m_dwTypes[i] = dwType & D3DCOMMON_TYPE_MASK;
			m_ppResources[i] = pHeader;
			hr = this->Patch(dwType, pHeader);
			if (FAILED(hr))
				break;
			pHeader += SizeOfResource( (LPDIRECT3DRESOURCE8)pHeader ); // standard resource types use standard sizes
		}
	}
    m_dwNumResources = i; // Number of loaded resources

    // Done with async XPR load
    CloseHandle( m_hfXPR );
    m_hfXPR = INVALID_HANDLE_VALUE;
	if (FAILED(hr))
	{
		m_LoadingState = LOADING_FAILED;
		return hr;
	}
	else
	{
		m_LoadingState = LOADING_DONE;
		return S_OK;
	}
}


//-----------------------------------------------------------------------------
// Name: OnIOComplete()
// Desc: Called when async i/o is complete, so that we can copy the
//       texture data to video memory and register the resources.
//-----------------------------------------------------------------------------
HRESULT Resource::Patch(DWORD dwType,	// resource type
						BYTE *pHeader)	// pointer to resource header
{
	if (!(dwType & 0x80000000))	// not user data
	{
		// Texture and VertexBuffer handling
		D3DResource *pResource = (D3DResource *)pHeader;
		pResource->Register( m_pVidMemData );
	}
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Called by Unload to release handles, etc. to cleanup the resource before unloading.
//-----------------------------------------------------------------------------
HRESULT Resource::Cleanup(DWORD dwType, BYTE *pHeader)
{
	// default does nothing
	return S_OK;
}
