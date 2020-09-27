//-----------------------------------------------------------------------------
// File: Cubemap.cpp
//
// Desc: Contains the cubemap-specific logic for the bundler tool
//
// Hist: 04.11.01 - New for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Cubemap.h"
#include "Bundler.h"




//-----------------------------------------------------------------------------
// Name: CCubemap()
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CCubemap::CCubemap( CBundler* pBundler )
         :CBaseTexture( pBundler )
{
    m_pSurfaceXP            = NULL;
    m_pSurfaceXN            = NULL;
    m_pSurfaceYP            = NULL;
    m_pSurfaceYN            = NULL;
    m_pSurfaceZP            = NULL;
    m_pSurfaceZN            = NULL;

    m_strSourceXP[0]         = '\0';
    m_strSourceXN[0]         = '\0';
    m_strSourceYP[0]         = '\0';
    m_strSourceYN[0]         = '\0';
    m_strSourceZP[0]         = '\0';
    m_strSourceZN[0]         = '\0';
    m_strAlphaSourceXP[0]    = '\0';
    m_strAlphaSourceXN[0]    = '\0';
    m_strAlphaSourceYP[0]    = '\0';
    m_strAlphaSourceYN[0]    = '\0';
    m_strAlphaSourceZP[0]    = '\0';
    m_strAlphaSourceZN[0]    = '\0';
}




//-----------------------------------------------------------------------------
// Name: ~CCubemap()
// Desc: Performs any cleanup needed before the object is destroyed
//-----------------------------------------------------------------------------
CCubemap::~CCubemap()
{
    if( m_pSurfaceXP )
        m_pSurfaceXP->Release();
    if( m_pSurfaceXN )
        m_pSurfaceXN->Release();
    if( m_pSurfaceYP )
        m_pSurfaceYP->Release();
    if( m_pSurfaceYN )
        m_pSurfaceYN->Release();
    if( m_pSurfaceZP )
        m_pSurfaceZP->Release();
    if( m_pSurfaceZN )
        m_pSurfaceZN->Release();
}




//-----------------------------------------------------------------------------
// Name: SaveToBundle()
// Desc: Handles saving the appropriate data to the packed resource file
//-----------------------------------------------------------------------------
HRESULT CCubemap::SaveToBundle( DWORD* pcbHeader, DWORD* pcbData )
{
    HRESULT hr;

    hr = LoadCubemap();
    if( FAILED( hr ) )
        return hr;

    // Pad data file to proper alignment for the start of the texture
    hr = m_pBundler->PadToAlignment( D3DTEXTURE_ALIGNMENT );
    if( FAILED( hr ) )
        return hr;

    // Save resource header
    hr = SaveHeaderInfo( m_pBundler->m_cbData, pcbHeader );
    if( FAILED( hr ) )
        return hr;

    // Save cubemap data
    (*pcbData) = 0;
    SaveSurface( pcbData, m_dwLevels, m_pSurfaceXP );
    SaveSurface( pcbData, m_dwLevels, m_pSurfaceXN );
    SaveSurface( pcbData, m_dwLevels, m_pSurfaceYP );
    SaveSurface( pcbData, m_dwLevels, m_pSurfaceYN );
    SaveSurface( pcbData, m_dwLevels, m_pSurfaceZP );
    SaveSurface( pcbData, m_dwLevels, m_pSurfaceZN );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadCubemap()
// Desc: Loads the texture from the file, and sets any properties that were
//       not specified with values from the file (width, height, format, etc)
//-----------------------------------------------------------------------------
HRESULT CCubemap::LoadCubemap()
{
    D3DSURFACE_DESC desc;
    HRESULT hr;

    // Try to look up our format string
    m_nFormat = FormatFromString( m_strFormat );
    if( m_nFormat < -1 )
    {
        m_pBundler->ErrorMsg( "Error: Invalid texture format: %s", m_strFormat );
        return E_FAIL;
    }
    lstrcpyA( m_strFormat, g_TextureFormats[m_nFormat].strFormat );

    // Check for linear textures
    if( g_TextureFormats[m_nFormat].Type == FMT_LINEAR )
    {
        m_pBundler->ErrorMsg( "Error: Cubemaps cannot have linear formats", m_strFormat );
        return E_FAIL;
    }

    // If an alpha source was specified, make sure the format supports alpha
    if( m_strAlphaSourceXP[0] || m_strAlphaSourceXN[0] || 
        m_strAlphaSourceYP[0] || m_strAlphaSourceYN[0] || 
        m_strAlphaSourceZP[0] || m_strAlphaSourceZN[0] ) 
    {
        if( g_TextureFormats[m_nFormat].dwNumAlphaBits == 0 )
        {
            m_pBundler->ErrorMsg( "Warning: an ALPHASOURCE was specified, yet the " \
                                  "requested texture format, %s,  does not have " \
                                  "any alpha.\n", m_strFormat );
        }
    }

    // Load the image surfaces from the file (using default width, height, and a
    // A8R8G8B8 surface format)
    if( FAILED( hr = LoadSurface( m_strSourceXP, m_strAlphaSourceXP, &m_pSurfaceXP ) ) )
    {
		if( m_strAlphaSourceXP[0] )
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s> or <%s>\n", m_strSourceXP, m_strAlphaSourceXP );
		else
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s>\n", m_strSourceXP );
        return hr;
    }
    if( FAILED( hr = LoadSurface( m_strSourceXN, m_strAlphaSourceXN, &m_pSurfaceXN ) ) )
    {
		if( m_strAlphaSourceXN[0] )
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s> or <%s>\n", m_strSourceXN, m_strAlphaSourceXN );
		else
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s>\n", m_strSourceXN );
        return hr;
    }
    if( FAILED( hr = LoadSurface( m_strSourceYP, m_strAlphaSourceYP, &m_pSurfaceYP ) ) )
    {
		if( m_strAlphaSourceYP[0] )
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s> or <%s>\n", m_strSourceYP, m_strAlphaSourceYP );
		else
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s>\n", m_strSourceYP );
        return hr;
    }
    if( FAILED( hr = LoadSurface( m_strSourceYN, m_strAlphaSourceYN, &m_pSurfaceYN ) ) )
    {
		if( m_strAlphaSourceYN[0] )
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s> or <%s>\n", m_strSourceYN, m_strAlphaSourceYN );
		else
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s>\n", m_strSourceYN );
        return hr;
    }
    if( FAILED( hr = LoadSurface( m_strSourceZP, m_strAlphaSourceZP, &m_pSurfaceZP ) ) )
    {
		if( m_strAlphaSourceZP[0] )
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s> or <%s>\n", m_strSourceZP, m_strAlphaSourceZP );
		else
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s>\n", m_strSourceZP );
        return hr;
    }
    if( FAILED( hr = LoadSurface( m_strSourceZN, m_strAlphaSourceZN, &m_pSurfaceZN ) ) )
    {
		if( m_strAlphaSourceZN[0] )
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s> or <%s>\n", m_strSourceZN, m_strAlphaSourceZN );
		else
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s>\n", m_strSourceZN );
        return hr;
    }

    // Determine final width and height
    if( m_dwSize==0 )
    {
        hr = m_pSurfaceXP->GetDesc( &desc );

        // Enforce power-of-two dimensions for cubemap faces
        for( m_dwSize=1;  m_dwSize  < desc.Width;  m_dwSize<<=1 );
    }

    // Determine final number of miplevels
    DWORD dwLevels = 1; 
    while( (1UL<<(dwLevels-1)) < m_dwSize )
        dwLevels++;
        
    if( m_dwLevels < 1 || m_dwLevels > dwLevels )
        m_dwLevels = dwLevels;

    // Change the size of the surfaces
    hr = ResizeSurface( m_dwSize, m_dwSize, &m_pSurfaceXP );
    hr = ResizeSurface( m_dwSize, m_dwSize, &m_pSurfaceXN );
    hr = ResizeSurface( m_dwSize, m_dwSize, &m_pSurfaceYP );
    hr = ResizeSurface( m_dwSize, m_dwSize, &m_pSurfaceYN );
    hr = ResizeSurface( m_dwSize, m_dwSize, &m_pSurfaceZP );
    hr = ResizeSurface( m_dwSize, m_dwSize, &m_pSurfaceZN );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SaveHeaderInfo()
// Desc: Saves the appropriate data to the header file
//-----------------------------------------------------------------------------
HRESULT CCubemap::SaveHeaderInfo( DWORD dwStart, DWORD* pcbHeader )
{
    D3DTexture d3dtex;

    XGSetCubeTextureHeader( m_dwSize, m_dwLevels, 0,
                            (D3DFORMAT)g_TextureFormats[m_nFormat].dwXboxFormat,
                            D3DPOOL_DEFAULT, (IDirect3DCubeTexture8*)&d3dtex,
                            dwStart, 0 );

    // Write the resource header out
    if( FAILED( m_pBundler->WriteHeader( &d3dtex, sizeof( d3dtex ) ) ) )
        return E_FAIL;

    (*pcbHeader) = sizeof(d3dtex);

    return S_OK;
}




