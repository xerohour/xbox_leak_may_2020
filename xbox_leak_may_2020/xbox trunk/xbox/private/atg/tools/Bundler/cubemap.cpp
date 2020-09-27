//-----------------------------------------------------------------------------
// File: Cubemap.cpp
//
// Desc: Contains the cubemap-specific logic for the bundler tool
//
// Hist: 04.11.01 - New for May XDK release
//       01.29.02 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Cubemap.h"
#include "Bundler.h"
#include "LoadImage.h"



//-----------------------------------------------------------------------------
// Name: CCubemap()
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CCubemap::CCubemap( CBundler* pBundler )
         :CBaseTexture( pBundler )
{
    m_pImageXP              = NULL;
    m_pImageXN              = NULL;
    m_pImageYP              = NULL;
    m_pImageYN              = NULL;
    m_pImageZP              = NULL;
    m_pImageZN              = NULL;

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
    delete m_pImageXP;
    delete m_pImageXN;
    delete m_pImageYP;
    delete m_pImageYN;
    delete m_pImageZP;
    delete m_pImageZN;
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

    SaveImage( pcbData, m_dwLevels, m_pImageXP );
    SaveImage( pcbData, m_dwLevels, m_pImageXN );
    SaveImage( pcbData, m_dwLevels, m_pImageYP );
    SaveImage( pcbData, m_dwLevels, m_pImageYN );
    SaveImage( pcbData, m_dwLevels, m_pImageZP );
    SaveImage( pcbData, m_dwLevels, m_pImageZN );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadCubemap()
// Desc: Loads the texture from the file, and sets any properties that were
//       not specified with values from the file (width, height, format, etc)
//-----------------------------------------------------------------------------
HRESULT CCubemap::LoadCubemap()
{
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
    if( FAILED( hr = LoadImage( m_strSourceXP, m_strAlphaSourceXP, &m_pImageXP ) ) )
    {
		if( m_strAlphaSourceXP[0] )
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s> or <%s>\n", m_strSourceXP, m_strAlphaSourceXP );
		else
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s>\n", m_strSourceXP );
        return hr;
    }
    if( FAILED( hr = LoadImage( m_strSourceXN, m_strAlphaSourceXN, &m_pImageXN ) ) )
    {
		if( m_strAlphaSourceXN[0] )
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s> or <%s>\n", m_strSourceXN, m_strAlphaSourceXN );
		else
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s>\n", m_strSourceXN );
        return hr;
    }
    if( FAILED( hr = LoadImage( m_strSourceYP, m_strAlphaSourceYP, &m_pImageYP ) ) )
    {
		if( m_strAlphaSourceYP[0] )
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s> or <%s>\n", m_strSourceYP, m_strAlphaSourceYP );
		else
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s>\n", m_strSourceYP );
        return hr;
    }
    if( FAILED( hr = LoadImage( m_strSourceYN, m_strAlphaSourceYN, &m_pImageYN ) ) )
    {
		if( m_strAlphaSourceYN[0] )
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s> or <%s>\n", m_strSourceYN, m_strAlphaSourceYN );
		else
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s>\n", m_strSourceYN );
        return hr;
    }
    if( FAILED( hr = LoadImage( m_strSourceZP, m_strAlphaSourceZP, &m_pImageZP ) ) )
    {
		if( m_strAlphaSourceZP[0] )
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s> or <%s>\n", m_strSourceZP, m_strAlphaSourceZP );
		else
			m_pBundler->ErrorMsg( "Cubemap: Couldn't load source file <%s>\n", m_strSourceZP );
        return hr;
    }
    if( FAILED( hr = LoadImage( m_strSourceZN, m_strAlphaSourceZN, &m_pImageZN ) ) )
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
        // Enforce power-of-two dimensions for cubemap faces
        for( m_dwSize=1;  m_dwSize  < m_pImageXP->m_Width;  m_dwSize<<=1 );
    }

    // Determine final number of miplevels
    DWORD dwLevels = 1; 
    while( (1UL<<(dwLevels-1)) < m_dwSize )
        dwLevels++;
        
    if( m_dwLevels < 1 || m_dwLevels > dwLevels )
        m_dwLevels = dwLevels;

    // Change the size of the surfaces
    hr = ResizeImage( m_dwSize, m_dwSize, &m_pImageXP );
    hr = ResizeImage( m_dwSize, m_dwSize, &m_pImageXN );
    hr = ResizeImage( m_dwSize, m_dwSize, &m_pImageYP );
    hr = ResizeImage( m_dwSize, m_dwSize, &m_pImageYN );
    hr = ResizeImage( m_dwSize, m_dwSize, &m_pImageZP );
    hr = ResizeImage( m_dwSize, m_dwSize, &m_pImageZN );

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




