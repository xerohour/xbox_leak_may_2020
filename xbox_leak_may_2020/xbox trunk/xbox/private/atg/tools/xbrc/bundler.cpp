//-----------------------------------------------------------------------------
// File: Bundler.cpp
//
// Desc: Bundles up a list of resources into an Xbox Packed Resource (xpr)
//       file.
//
// Hist: 2001.02.06 - New for March XDK release
//       2002.01.31 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Bundler.h"
#include "Texture.h"
#include "Cubemap.h"
#include "VolumeTexture.h"
#include "VB.h"
#include "UserData.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdafx.h"

#define INITIAL_HEADER_ALLOC 1024
#define INITIAL_DATA_ALLOC 65536

//-----------------------------------------------------------------------------
// Name: CBundler()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
CBundler::CBundler()
{
    m_pd3d = NULL;
    m_pd3ddev = NULL;
    m_cbHeader = m_cbData   = 0;
    m_strRDF[0] = m_strXPR[0] = m_strHDR[0] = m_strERR[0] = 0;
    m_strPrefix[0] =  0;
    m_hfRDF    = m_hfXPR    = m_hfHDR    = m_hfERR    = INVALID_HANDLE_VALUE;
    m_bExplicitPrefix = FALSE;
    m_pbHeaderBuff = m_pbDataBuff   = NULL;
    m_nResourceCount = 0;
    m_bExplicitHDR = m_bExplicitXPR = m_bExplicitERR = FALSE;
    m_bSingleTexture = FALSE;
}




//-----------------------------------------------------------------------------
// Name: ~CBundler()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
CBundler::~CBundler()
{
    CloseFiles();
    if( m_pbHeaderBuff )
    {
        free( m_pbHeaderBuff );
        m_pbHeaderBuff = NULL;
    }
    
    if( m_pbDataBuff )
    {
        free( m_pbDataBuff );
        m_pbDataBuff = NULL;
    }
    
    if( m_pd3ddev != NULL )
    {
        m_pd3ddev->Release();
        m_pd3d->Release();
    }
}



//-----------------------------------------------------------------------------
// Name: ErrorMsg()
// Desc: Prints an error message to stderr, and writes to the error file
//-----------------------------------------------------------------------------
void CBundler::ErrorMsg( CHAR* strFmt, ... )
{
    DWORD cb;
    va_list arglist;
    const int TempSize = 2000;
    char strTemp[TempSize];

    if( INVALID_HANDLE_VALUE == m_hfERR )
    {   
        m_hfERR = CreateFile( m_strERR, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
    }

    va_start( arglist, strFmt );

    _vsnprintf( strTemp, TempSize - 1, strFmt, arglist );
    strTemp[TempSize-1] = 0;    // make sure string is null terminated for strlen call
    WriteFile( m_hfERR, strTemp, strlen( strTemp ), &cb, NULL );
    
    // Instead of sending the message to stderr, use the XBContent error handling
    //  vfprintf( stderr, strFmt, arglist );
    extern XBContent *g_pXBContent;
    if (g_pXBContent == NULL)
        fputs( strTemp, stderr );
    else
    {
        WCHAR wstrTemp[TempSize];
        size_t count = mbstowcs( wstrTemp, strTemp, TempSize );
        if (count == -1)
            fputs( strTemp, stderr );
        else
        {
            g_pXBContent->error(g_pXBContent->m_pLocator, wstrTemp, E_FAIL);
            g_pXBContent->m_bErrorSuppress = true;
        }
    }

    va_end( arglist );
}




//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes D3D for texture gunk
//-----------------------------------------------------------------------------
HRESULT CBundler::InitD3D()
{
    HRESULT hr;
    D3DDISPLAYMODE dispMode;
    D3DPRESENT_PARAMETERS presentParams;

    // If we've already init'ed D3D, then we're good to go
    if( m_pd3ddev != NULL )
        return S_FALSE;

    m_pd3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (m_pd3d == NULL)
    {
        ErrorMsg("Couldn't create Direct3d - is it installed?\n");
        return E_FAIL;
    }

    m_pd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispMode);

    ZeroMemory(&presentParams, sizeof(presentParams));
    presentParams.Windowed = TRUE;
    presentParams.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
    presentParams.BackBufferWidth = 8;
    presentParams.BackBufferHeight = 8;
    presentParams.BackBufferFormat = ( ( dispMode.Format == D3DFMT_R8G8B8 ) ? D3DFMT_R5G6B5 : dispMode.Format );

    hr = m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, GetDesktopWindow(),
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &presentParams, &m_pd3ddev);
    if (FAILED(hr))
    {
        ErrorMsg("Couldn't create reference device.  Error: %x\n", hr);
        m_pd3d->Release();
        m_pd3d = NULL;
        return E_FAIL;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Begin
// Desc: Sets up writing 
//-----------------------------------------------------------------------------
HRESULT CBundler::Begin( CHAR *strBase )
{
/*  
    HRESULT hr;

    int n;
    int nBaseFileLen;
    char * pch;
    BOOL bHaveRDF = FALSE;

    for(n=1; n<argc; n++)
    {
        // step through each command line parameter
        if((argv[n][0] == '/') || (argv[n][0] == '-'))
        {
            if((n+1) == argc)
                return E_INVALIDARG;

            // found an option
            if(lstrcmpi("o", &(argv[n][1])) == 0)
            {
                // Output file
                lstrcpyA( m_strXPR, argv[n+1] );
                m_bExplicitXPR = TRUE;
            }
            else if(lstrcmpi("h", &(argv[n][1])) == 0)
            {
                // Header file
                lstrcpyA( m_strHDR, argv[n+1] );
                m_bExplicitHDR = TRUE;
            }
            else if(lstrcmpi("p", &(argv[n][1])) == 0)
            {
                // Prefix for header file constants
                lstrcpyA( m_strPrefix, argv[n+1] );
                m_bExplicitPrefix = TRUE;
            }
            else if(lstrcmpi("e", &(argv[n][1])) == 0)
            {
                // Error file
                lstrcpyA( m_strERR, argv[n+1] );
                m_bExplicitERR = TRUE;
            }
            else if(lstrcmpi("?", &(argv[n][1])) == 0)
                return E_INVALIDARG;    // causes usage to be displayed
            else
                return E_INVALIDARG;
            
            n++;    // skip two args
        }
        else
        {
            bHaveRDF = TRUE;
        }
    }

    if(!bHaveRDF)
        return E_INVALIDARG;
*/  
    // Grab the RDF filename
    lstrcpyA( m_strRDF, strBase );
    
    // Find where the filename ends and the extension begins
    CHAR *pch;
    pch = m_strRDF;
    while( pch && strchr( pch, '.' ) )
    {
        pch = strchr( pch, '.' ) + 1;
    }
    
    // If we never found a '.', use the whole filename
    if( pch == m_strRDF )
        pch = m_strRDF + lstrlen( m_strRDF ) + 1;
    
    // Must have something for a base filename
    INT nBaseFileLen = pch - m_strRDF - 1;
    if( nBaseFileLen <= 0 )
        return E_INVALIDARG;
    
    // Check to see if we are doing a quick single-texture conversion
    if((_stricmp(pch, "bmp") == 0) ||
       (_stricmp(pch, "jpg") == 0) ||
       (_stricmp(pch, "tga") == 0) ||
       (_stricmp(pch, "dds") == 0) ||
       (_stricmp(pch, "png") == 0))
    {
        m_bSingleTexture = TRUE;
    }
    
    // Copy the base filename + appropriate extension to each string
    strncpy( m_strXPR, m_strRDF, nBaseFileLen );
    m_strXPR[nBaseFileLen] = 0;
    strcat( m_strXPR, XPREXT );
    strncpy( m_strHDR, m_strRDF, nBaseFileLen );
    m_strHDR[nBaseFileLen] = 0;
    strcat( m_strHDR, HDREXT );
    strncpy( m_strERR, m_strRDF, nBaseFileLen );
    m_strERR[nBaseFileLen] = 0;
    strcat( m_strERR, ERREXT );
    
    strcpy( m_strPath, m_strXPR );
    if( strrchr( m_strPath, '\\' ) )
        strrchr( m_strPath, '\\' )[1] = 0;
    else
        m_strPath[0] = 0;

    // Creating a D3D device sets the floating point control
    // word to single precision.  Now that we only create the
    // D3D device on demand, we _could_ use double precision,
    // but set to single precision to maintain bit-for-bit
    // output compatibility.
    _controlfp( _PC_24, _MCW_PC );

    // Allocate buffers for storing data
    m_pbHeaderBuff = (BYTE *)malloc( INITIAL_HEADER_ALLOC );
    if( !m_pbHeaderBuff )
        return E_OUTOFMEMORY;
    m_cbHeaderAlloc = INITIAL_HEADER_ALLOC;

    m_pbDataBuff = (BYTE *)malloc( INITIAL_DATA_ALLOC );
    if( !m_pbDataBuff )
        return E_OUTOFMEMORY;
    m_cbDataAlloc = INITIAL_DATA_ALLOC;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: OpenFiles()
// Desc: Opens file handles for all our output files
//-----------------------------------------------------------------------------
HRESULT CBundler::OpenFiles()
{
    m_hfXPR = CreateFile( m_strXPR, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
    if( INVALID_HANDLE_VALUE == m_hfXPR )
    {
        ErrorMsg( "Couldn't open file <%s>", m_strXPR );
        return E_FAIL;
    }

    m_hfHDR = CreateFile( m_strHDR, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
    if( INVALID_HANDLE_VALUE == m_hfHDR )
    {
        ErrorMsg( "Couldn't open header file <%s>", m_strHDR );
        return E_FAIL;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CloseFiles()
// Desc: Closes file handles for all our input/output files
//-----------------------------------------------------------------------------
HRESULT CBundler::CloseFiles()
{
    if( INVALID_HANDLE_VALUE != m_hfRDF )
        CloseHandle( m_hfRDF );
    if( INVALID_HANDLE_VALUE != m_hfXPR )
        CloseHandle( m_hfXPR );
    if( INVALID_HANDLE_VALUE != m_hfHDR )
        CloseHandle( m_hfHDR );
    if( INVALID_HANDLE_VALUE != m_hfERR )
        CloseHandle( m_hfERR );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: End()
// Desc: Handles any necessary end-of-input processing, such as patching up
//       header information
//-----------------------------------------------------------------------------
HRESULT CBundler::End()
{
    if( FAILED( OpenFiles() ) )
        return E_FAIL;

    if( FAILED( WriteSymbolTable() ) )
        return E_FAIL;

    // write one last header entry to terminate the list
    DWORD tmp = 0xffffffff;
    WriteHeader( &tmp, sizeof( tmp ) );
    m_cbHeader += sizeof( tmp );

    if( FAILED( FlushDataFile() ) )
        return E_FAIL;

    if( FAILED( WriteHeaderFile() ) )
        return E_FAIL;
    
    return S_FALSE;
}


//-----------------------------------------------------------------------------
// Name: FilterFromString()
// Desc: Returns a D3DXFILTER from the given string
//-----------------------------------------------------------------------------
DWORD CBundler::FilterFromString( char* strFilter )
{
    DWORD filter  = D3DX_FILTER_TRIANGLE; // The default
    DWORD address = 0;
    DWORD dither  = 0;

    if( strFilter[0] )
    {
        // Scan string
        CHAR* strToken = strtok( strFilter, "|" );
        while( strToken != NULL )
        {
            if( !lstrcmpi( strToken, "NONE" ) )      filter  = D3DX_FILTER_NONE;
            if( !lstrcmpi( strToken, "POINT" ) )     filter  = D3DX_FILTER_POINT;
            if( !lstrcmpi( strToken, "LINEAR" ) )    filter  = D3DX_FILTER_LINEAR;
            if( !lstrcmpi( strToken, "TRIANGLE" ) )  filter  = D3DX_FILTER_TRIANGLE;
            if( !lstrcmpi( strToken, "BOX" ) )       filter  = D3DX_FILTER_BOX;
            if( !lstrcmpi( strToken, "WRAP" ) )      address = 0;
            if( !lstrcmpi( strToken, "CLAMP" ) )     address = D3DX_FILTER_MIRROR;
            if( !lstrcmpi( strToken, "DITHER" ) )    dither  = D3DX_FILTER_DITHER;

            strToken = strtok( NULL, "|" );
        }
    }

    return (filter|address|dither);
}

/*


//-----------------------------------------------------------------------------
// Name: HandleTextureToken()
// Desc: Takes over to handle a texture token. Reads all tokens up to and
//       including the closing brace
//-----------------------------------------------------------------------------
HRESULT CBundler::HandleTextureToken()
{
    HRESULT hr;
    BUNDLERTOKEN tok;
    char strBuff[MAX_PATH];
    CTexture2D tex(this);
    BOOL bDone = FALSE;
    DWORD cbHeader, cbData;

    // This is the texture name.
    hr = GetNextTokenString( m_aResources[m_nResourceCount].strIdentifier, TT_IDENTIFIER );
    if( FAILED( hr ) )
        return hr;

    // Check to see if it's a repeat identifier
    if( IsExistingIdentifier( m_aResources[m_nResourceCount].strIdentifier ) )
    {
        ErrorMsg( "Second usage of identifier <%s>", m_aResources[m_nResourceCount].strIdentifier );
        return E_FAIL;
    }

    // This should be the open brace
    hr = GetNextToken( &tok );
    if( FAILED( hr ) )
        return hr;

    if( tok.eType != TOKEN_RESOURCE_OPENBRACE )
    {
        ErrorMsg( "Texture name should be followed by an open brace\n" );
        return E_FAIL;
    }

    // Loop over all the properties
    while( !bDone )
    {
        hr = GetNextToken( &tok );
        if( FAILED( hr ) )
            return hr;

        // Any property token needs a value
        if( tok.eType & TOKEN_PROPERTY_TEXTURE )
        {
            hr = GetNextTokenString( strBuff, tok.propType );
            if( FAILED( hr ) )
                return hr;
        }

        switch( tok.eType )
        {
            case TOKEN_PROPERTY_TEXTURE_SOURCE:
                strcpy( tex.m_strSource, strBuff );
                break;
            case TOKEN_PROPERTY_TEXTURE_ALPHASOURCE:
                strcpy( tex.m_strAlphaSource, strBuff );
                break;
            case TOKEN_PROPERTY_TEXTURE_FILTER:
                tex.m_dwFilter = FilterFromString( strBuff );
                break;
            case TOKEN_PROPERTY_TEXTURE_FORMAT:
                strcpy( tex.m_strFormat, strBuff );
                break;
            case TOKEN_PROPERTY_TEXTURE_WIDTH:
                tex.m_dwWidth = atoi( strBuff );
                break;
            case TOKEN_PROPERTY_TEXTURE_HEIGHT:
                tex.m_dwHeight = atoi( strBuff );
                break;
            case TOKEN_PROPERTY_TEXTURE_LEVELS:
                tex.m_dwLevels = atoi( strBuff );
                break;
            case TOKEN_RESOURCE_CLOSEBRACE:
                bDone = TRUE;
                break;
            default:
                ErrorMsg("<%s> is not a texture property.\n", tok.strKeyword );
        }
    }

    // Save the texture to the bundle
    hr = tex.SaveToBundle( &cbHeader, &cbData );
    if( FAILED( hr ) )
        return hr;

    printf("Texture: Wrote %s out in format %s (%d bytes)\n", 
        m_aResources[m_nResourceCount].strIdentifier, tex.m_strFormat, cbData );
    printf("     %d x %d, %d levels\n\n", tex.m_dwWidth, tex.m_dwHeight, tex.m_dwLevels);

    m_aResources[m_nResourceCount++].dwOffset = m_cbHeader;
    m_cbHeader += cbHeader;
    //m_cbData += cbData;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: HandleCubemapToken()
// Desc: Takes over to handle a cubemap token. Reads all tokens up to and
//       including the closing brace
//-----------------------------------------------------------------------------
HRESULT CBundler::HandleCubemapToken()
{
    HRESULT      hr;
    BUNDLERTOKEN token;
    char         strBuffer[MAX_PATH];
    CCubemap     tex(this);
    BOOL         bDone = FALSE;
    DWORD        cbHeader;
    DWORD        cbData;

    // This is the texture name.
    hr = GetNextTokenString( m_aResources[m_nResourceCount].strIdentifier, TT_IDENTIFIER );
    if( FAILED( hr ) )
        return hr;

    // Check to see if it's a repeat identifier
    if( IsExistingIdentifier( m_aResources[m_nResourceCount].strIdentifier ) )
    {
        ErrorMsg( "Second usage of identifier <%s>", m_aResources[m_nResourceCount].strIdentifier );
        return E_FAIL;
    }

    // This should be the open brace
    hr = GetNextToken( &token );
    if( FAILED( hr ) )
        return hr;

    if( token.eType != TOKEN_RESOURCE_OPENBRACE )
    {
        ErrorMsg( "Cubemap name should be followed by an open brace\n" );
        return E_FAIL;
    }

    // Loop over all the properties
    while( !bDone )
    {
        hr = GetNextToken( &token );
        if( FAILED( hr ) )
            return hr;

        // Any property token needs a value
        if( token.eType & (TOKEN_PROPERTY_CUBEMAP|TOKEN_PROPERTY_TEXTURE) )
        {
            hr = GetNextTokenString( strBuffer, token.propType );
            if( FAILED( hr ) )
                return hr;
        }

        switch( token.eType )
        {
            case TOKEN_PROPERTY_CUBEMAP_SOURCE_XP:
                strcpy( tex.m_strSourceXP, strBuffer );
                break;
            case TOKEN_PROPERTY_CUBEMAP_SOURCE_XN:
                strcpy( tex.m_strSourceXN, strBuffer );
                break;
            case TOKEN_PROPERTY_CUBEMAP_SOURCE_YP:
                strcat( tex.m_strSourceYP, strBuffer );
                break;
            case TOKEN_PROPERTY_CUBEMAP_SOURCE_YN:
                strcpy( tex.m_strSourceYN, strBuffer );
                break;
            case TOKEN_PROPERTY_CUBEMAP_SOURCE_ZP:
                strcpy( tex.m_strSourceZP, strBuffer );
                break;
            case TOKEN_PROPERTY_CUBEMAP_SOURCE_ZN:
                strcpy( tex.m_strSourceZN, strBuffer );
                break;
            case TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_XP:
                strcpy( tex.m_strAlphaSourceXP, strBuffer );
                break;
            case TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_XN:
                strcpy( tex.m_strAlphaSourceXN, strBuffer );
                break;
            case TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_YP:
                strcpy( tex.m_strAlphaSourceYP, strBuffer );
                break;
            case TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_YN:
                strcpy( tex.m_strAlphaSourceYN, strBuffer );
                break;
            case TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_ZP:
                strcpy( tex.m_strAlphaSourceZP, strBuffer );
                break;
            case TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_ZN:
                strcpy( tex.m_strAlphaSourceZN, strBuffer );
                break;
            case TOKEN_PROPERTY_TEXTURE_FILTER:
                tex.m_dwFilter = FilterFromString( strBuffer );
                break;
            case TOKEN_PROPERTY_TEXTURE_FORMAT:
                strcpy( tex.m_strFormat, strBuffer );
                break;
            case TOKEN_PROPERTY_CUBEMAP_SIZE:
                tex.m_dwSize = atoi( strBuffer );
                break;
            case TOKEN_PROPERTY_TEXTURE_LEVELS:
                tex.m_dwLevels = atoi( strBuffer );
                break;
            case TOKEN_RESOURCE_CLOSEBRACE:
                bDone = TRUE;
                break;
            default:
                ErrorMsg("<%s> is not a cubemap property.\n", token.strKeyword );
        }
    }

    // Save the texture to the bundle
    hr = tex.SaveToBundle( &cbHeader, &cbData );
    if( FAILED( hr ) )
        return hr;

    printf("Cubemap: Wrote %s out in format %s (%d bytes)\n", 
        m_aResources[m_nResourceCount].strIdentifier, tex.m_strFormat, cbData );
    printf("     6 x %d x %d, %d levels\n\n", tex.m_dwSize, tex.m_dwSize, tex.m_dwLevels);

    m_aResources[m_nResourceCount++].dwOffset = m_cbHeader;
    m_cbHeader += cbHeader;
    //m_cbData += cbData;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: HandleVolumeTextureToken()
// Desc: Takes over to handle a volume texture token. Reads all tokens up to
//       and including the closing brace
//-----------------------------------------------------------------------------
HRESULT CBundler::HandleVolumeTextureToken()
{
    HRESULT        hr;
    BUNDLERTOKEN   token;
    char           strBuffer[MAX_PATH];
    CVolumeTexture tex(this);
    BOOL           bDone = FALSE;
    DWORD          cbHeader;
    DWORD          cbData;
    DWORD          srcnum = 0;
    DWORD          alphasrcnum = 0;

    // This is the texture name.
    hr = GetNextTokenString( m_aResources[m_nResourceCount].strIdentifier, TT_IDENTIFIER );
    if( FAILED( hr ) )
        return hr;

    // Check to see if it's a repeat identifier
    if( IsExistingIdentifier( m_aResources[m_nResourceCount].strIdentifier ) )
    {
        ErrorMsg( "Second usage of identifier <%s>", m_aResources[m_nResourceCount].strIdentifier );
        return E_FAIL;
    }

    // This should be the open brace
    hr = GetNextToken( &token );
    if( FAILED( hr ) )
        return hr;

    if( token.eType != TOKEN_RESOURCE_OPENBRACE )
    {
        ErrorMsg( "Cubemap name should be followed by an open brace\n" );
        return E_FAIL;
    }

    // Loop over all the properties
    while( !bDone )
    {
        hr = GetNextToken( &token );
        if( FAILED( hr ) )
            return hr;

        // Any property token needs a value
        if( token.eType & (TOKEN_PROPERTY_CUBEMAP|TOKEN_PROPERTY_TEXTURE) )
        {
            hr = GetNextTokenString( strBuffer, token.propType );
            if( FAILED( hr ) )
                return hr;
        }

        switch( token.eType )
        {
            case TOKEN_PROPERTY_TEXTURE_SOURCE:
                if( tex.m_dwDepth )
                {
                    if( srcnum <tex.m_dwDepth )
                        strcpy( tex.m_astrSource[srcnum++], strBuffer );
                    else
                        srcnum++;
                }
                else
                    ErrorMsg("Must specify volume texture DEPTH before SOURCE.\n" );
                break;
            case TOKEN_PROPERTY_TEXTURE_ALPHASOURCE:
                if( tex.m_dwDepth )
                {
                    if( alphasrcnum <tex.m_dwDepth )
                        strcpy( tex.m_astrAlphaSource[alphasrcnum++], strBuffer );
                    else
                        alphasrcnum++;
                }
                else
                    ErrorMsg("Must specify volume texture DEPTH before ALPHASOURCE.\n" );
                break;
            case TOKEN_PROPERTY_TEXTURE_FILTER:
                tex.m_dwFilter = FilterFromString( strBuffer );
                break;
            case TOKEN_PROPERTY_TEXTURE_FORMAT:
                strcpy( tex.m_strFormat, strBuffer );
                break;
            case TOKEN_PROPERTY_TEXTURE_WIDTH:
                tex.m_dwWidth = atoi( strBuffer );
                break;
            case TOKEN_PROPERTY_TEXTURE_HEIGHT:
                tex.m_dwHeight = atoi( strBuffer );
                break;
            case TOKEN_PROPERTY_VOLUMETEXTURE_DEPTH:
                if( 0 == tex.m_dwDepth )
                {   
                    tex.m_dwDepth         = atoi( strBuffer );
                    tex.m_astrSource      = new FILENAME[tex.m_dwDepth];
                    tex.m_astrAlphaSource = new FILENAME[tex.m_dwDepth];
                    ZeroMemory( tex.m_astrSource,      sizeof(FILENAME) * tex.m_dwDepth );
                    ZeroMemory( tex.m_astrAlphaSource, sizeof(FILENAME) * tex.m_dwDepth );
                }
                else
                    ErrorMsg("VolumeTexture DEPTH already specified.\n" );
                break;
            case TOKEN_PROPERTY_TEXTURE_LEVELS:
                tex.m_dwLevels = atoi( strBuffer );
                break;
            case TOKEN_RESOURCE_CLOSEBRACE:
                bDone = TRUE;
                break;
            default:
                ErrorMsg("<%s> is not a volume texture property.\n", token.strKeyword );
        }
    }

    // Check that we have a good number of sources
    if( ( srcnum != tex.m_dwDepth ) || ( alphasrcnum > 0  && alphasrcnum != tex.m_dwDepth ) )
    {   
        ErrorMsg("Volume texture must have same number of sources as the depth.\n" );
        return E_FAIL;
    }

    // Save the texture to the bundle
    hr = tex.SaveToBundle( &cbHeader, &cbData );
    if( FAILED( hr ) )
        return hr;

    printf("Volume texture: Wrote %s out in format %s (%d bytes)\n", 
        m_aResources[m_nResourceCount].strIdentifier, tex.m_strFormat, cbData );
    printf("     %d x %d x %d, %d levels\n\n", tex.m_dwWidth, tex.m_dwHeight, tex.m_dwDepth, tex.m_dwLevels);

    m_aResources[m_nResourceCount++].dwOffset = m_cbHeader;
    m_cbHeader += cbHeader;
    //m_cbData += cbData;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Match()
// Desc: Returns TRUE if it successfuly reads a token of the specified type.
//-----------------------------------------------------------------------------
BOOL CBundler::Match(INT TokType, BUNDLERTOKEN *pTok)
{
    HRESULT hr;
    hr = GetNextToken( pTok );
    if( FAILED( hr ) )
        return FALSE;

    if( pTok->eType != TokType )
        return FALSE;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: HandleVertexBuffer()
// Desc: Takes over to handle a VertexBuffer token.  Reads all tokens up to and
//       including the closing brace
//-----------------------------------------------------------------------------
HRESULT CBundler::HandleVertexBufferToken()
{
    HRESULT hr;
    BUNDLERTOKEN tok;
    char strBuff[MAX_PATH], *pEnd;
    BOOL bDone = FALSE;
    DWORD cbHeader, cbData;

    // This is the vertex buffer name.
    hr = GetNextTokenString( m_aResources[m_nResourceCount].strIdentifier, TT_IDENTIFIER );
    if( FAILED( hr ) )
        return hr;

    // Check to see if it's a repeat identifier
    if( IsExistingIdentifier( m_aResources[m_nResourceCount].strIdentifier ) )
    {
        ErrorMsg( "Second usage of identifier <%s>", m_aResources[m_nResourceCount].strIdentifier );
        return E_FAIL;
    }

    if(!Match(TOKEN_RESOURCE_OPENBRACE, &tok))
    {
        ErrorMsg( "Vertexbuffer name should be followed by an open brace\n" );
        return E_FAIL;
    }

    CVertexBuffer *vb = new CVertexBuffer(this);
    
    // Loop over all the properties
    while( !bDone )
    {
        hr = GetNextToken( &tok );
        if( FAILED( hr ) )
            return hr;

        switch( tok.eType )
        {
            case TOKEN_PROPERTY_VB_VERTEXDATA:
                if(!Match(TOKEN_RESOURCE_OPENBRACE, &tok))
                {
                    ErrorMsg( "VertexData property must begin with an open brace.\n" );
                    return E_FAIL;
                }
                while(TRUE)
                {
                    if(FAILED(hr = GetNextTokenString( strBuff, TT_ANY ))) return hr;
                
                    if((strBuff[0] == '}') || (strBuff[0] == TOKEOF))
                        break;
                    else if(ValidateType(strBuff, TT_HEXNUMBER) == S_OK)
                        vb->AddVertexData((DOUBLE)strtoul(strBuff, &pEnd, 16));
                    else if(ValidateType(strBuff, TT_INTEGER) == S_OK)
                        vb->AddVertexData(atof(strBuff));
                    else if(ValidateType(strBuff, TT_FLOAT) == S_OK)
                        vb->AddVertexData(atof(strBuff));
                }
                break;
            case TOKEN_PROPERTY_VB_VERTEXFORMAT:
                if(!Match(TOKEN_RESOURCE_OPENBRACE, &tok))
                {
                    ErrorMsg( "VertexFormat property must begin with an open brace.\n" );
                    return E_FAIL;
                }
                while(TRUE)
                {
                    if(FAILED(hr = GetNextTokenString( strBuff, TT_ANY ))) return hr;
                
                    if((strBuff[0] == '}') || (strBuff[0] == TOKEOF))
                        break;
                    else
                    {
                        if((strcmp(strBuff, "D3DVSDT_FLOAT1") == 0) ||
                           (strcmp(strBuff, "FLOAT1") == 0))
                            vb->AddVertexFormat(XD3DVSDT_FLOAT1);
                        else if((strcmp(strBuff, "D3DVSDT_FLOAT2") == 0) ||
                                (strcmp(strBuff, "FLOAT2") == 0))
                            vb->AddVertexFormat(XD3DVSDT_FLOAT2);
                        else if((strcmp(strBuff, "D3DVSDT_FLOAT3") == 0) ||
                                (strcmp(strBuff, "FLOAT3") == 0))
                            vb->AddVertexFormat(XD3DVSDT_FLOAT3);
                        else if((strcmp(strBuff, "D3DVSDT_FLOAT4") == 0) ||
                                (strcmp(strBuff, "FLOAT4") == 0))
                            vb->AddVertexFormat(XD3DVSDT_FLOAT4);
                        else if((strcmp(strBuff, "D3DVSDT_D3DCOLOR") == 0) ||
                                (strcmp(strBuff, "D3DCOLOR") == 0))
                            vb->AddVertexFormat(XD3DVSDT_D3DCOLOR);
                        else if((strcmp(strBuff, "D3DVSDT_SHORT2") == 0) ||
                                (strcmp(strBuff, "SHORT2") == 0))
                            vb->AddVertexFormat(XD3DVSDT_SHORT2);
                        else if((strcmp(strBuff, "D3DVSDT_SHORT4") == 0) ||
                                (strcmp(strBuff, "SHORT4") == 0))
                            vb->AddVertexFormat(XD3DVSDT_SHORT4);
                        else if((strcmp(strBuff, "D3DVSDT_NORMSHORT1") == 0) ||
                                (strcmp(strBuff, "NORMSHORT1") == 0))
                            vb->AddVertexFormat(XD3DVSDT_NORMSHORT1);
                        else if((strcmp(strBuff, "D3DVSDT_NORMSHORT2") == 0) ||
                                (strcmp(strBuff, "NORMSHORT2") == 0))
                            vb->AddVertexFormat(XD3DVSDT_NORMSHORT2);
                        else if((strcmp(strBuff, "D3DVSDT_NORMSHORT3") == 0) ||
                                (strcmp(strBuff, "NORMSHORT3") == 0))
                            vb->AddVertexFormat(XD3DVSDT_NORMSHORT3);
                        else if((strcmp(strBuff, "D3DVSDT_NORMSHORT4") == 0) ||
                                (strcmp(strBuff, "NORMSHORT4") == 0))
                            vb->AddVertexFormat(XD3DVSDT_NORMSHORT4);
                        else if((strcmp(strBuff, "D3DVSDT_NORMPACKED3") == 0) ||
                                (strcmp(strBuff, "NORMPACKED3") == 0))
                            vb->AddVertexFormat(XD3DVSDT_NORMPACKED3);
                        else if((strcmp(strBuff, "D3DVSDT_SHORT1") == 0) ||
                                (strcmp(strBuff, "SHORT1") == 0))
                            vb->AddVertexFormat(XD3DVSDT_SHORT1);
                        else if((strcmp(strBuff, "D3DVSDT_SHORT3") == 0) ||
                                (strcmp(strBuff, "SHORT3") == 0))
                            vb->AddVertexFormat(XD3DVSDT_SHORT3);
                        else if((strcmp(strBuff, "D3DVSDT_PBYTE1") == 0) ||
                                (strcmp(strBuff, "PBYTE1") == 0))
                            vb->AddVertexFormat(XD3DVSDT_PBYTE1);
                        else if((strcmp(strBuff, "D3DVSDT_PBYTE2") == 0) ||
                                (strcmp(strBuff, "PBYTE2") == 0))
                            vb->AddVertexFormat(XD3DVSDT_PBYTE2);
                        else if((strcmp(strBuff, "D3DVSDT_PBYTE3") == 0) ||
                                (strcmp(strBuff, "PBYTE3") == 0))
                            vb->AddVertexFormat(XD3DVSDT_PBYTE3);
                        else if((strcmp(strBuff, "D3DVSDT_PBYTE4") == 0) ||
                                (strcmp(strBuff, "PBYTE4") == 0))
                            vb->AddVertexFormat(XD3DVSDT_PBYTE4);
                        else if((strcmp(strBuff, "D3DVSDT_FLOAT2H") == 0) ||
                                (strcmp(strBuff, "FLOAT2H") == 0))
                            vb->AddVertexFormat(XD3DVSDT_FLOAT2H);
                        else
                        {
                            ErrorMsg( "Unrecognized attribute format: %s\n", strBuff );
                            return E_FAIL;
                        }
                    }
                }
                break;

            case TOKEN_RESOURCE_CLOSEBRACE:
                bDone = TRUE;
                break;
            default:
                ErrorMsg("<%s> is not a vertexbuffer property.\n", tok.strKeyword );
        }
    }

    // Save the vertexbuffer to the bundle
    hr = vb->SaveToBundle( &cbHeader, &cbData );
    if( FAILED( hr ) )
        return hr;

    printf("VB:  Wrote %s: %d bytes per vertex, %d vertices (%d bytes)\n\n", 
           m_aResources[m_nResourceCount].strIdentifier, vb->m_cBytesPerVertex,
           vb->m_cVertices, cbData );

    m_aResources[m_nResourceCount++].dwOffset = m_cbHeader;
    m_cbHeader += cbHeader;
    //m_cbData += cbData;

    delete vb;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: HandleUserDataToken()
// Desc: Takes over to handle a UserData token.  Reads all tokens up to and
//       including the closing brace
//-----------------------------------------------------------------------------
HRESULT CBundler::HandleUserDataToken()
{
    BUNDLERTOKEN token;
    CUserData    ud(this);
    CHAR         strBuff[MAX_PATH];
    BOOL         bDone = FALSE;
    DWORD        cbHeader, cbData;
    HRESULT      hr;

    // This is the texture name.
    hr = GetNextTokenString( m_aResources[m_nResourceCount].strIdentifier, TT_IDENTIFIER );
    if( FAILED( hr ) )
        return hr;

    // Check to see if it's a repeat identifier
    if( IsExistingIdentifier( m_aResources[m_nResourceCount].strIdentifier ) )
    {
        ErrorMsg( "Second usage of identifier <%s>", m_aResources[m_nResourceCount].strIdentifier );
        return E_FAIL;
    }

    // This should be the open brace
    hr = GetNextToken( &token );
    if( FAILED( hr ) )
        return hr;

    if( token.eType != TOKEN_RESOURCE_OPENBRACE )
    {
        ErrorMsg( "Texture name should be followed by an open brace\n" );
        return E_FAIL;
    }

    // Loop over all the properties
    while( !bDone )
    {
        hr = GetNextToken( &token );
        if( FAILED( hr ) )
            return hr;

        // Any property token needs a value
        if( token.eType & TOKEN_PROPERTY_USERDATA )
        {
            hr = GetNextTokenString( strBuff, token.propType );
            if( FAILED( hr ) )
                return hr;
        }

        switch( token.eType )
        {
            case TOKEN_PROPERTY_USERDATA_DATAFILE:
                strcpy( ud.m_strSource, strBuff );
                break;
            case TOKEN_RESOURCE_CLOSEBRACE:
                bDone = TRUE;
                break;
            default:
                ErrorMsg("<%s> is not a userdata property.\n", token.strKeyword );
        }
    }

    // Save the UserData to the bundle
    hr = ud.SaveToBundle( &cbHeader, &cbData );
    if( FAILED( hr ) )
        return hr;

    printf( "UserData: Wrote %ld bytes.\n", cbHeader );

    m_aResources[m_nResourceCount++].dwOffset = m_cbHeader;
    m_cbHeader += cbHeader;
    //m_cbData += cbData;

    return S_OK;
}

*/


//-----------------------------------------------------------------------------
// Name: FlushDataFile()
// Desc: Writes out all the buffered data to the data file, including
//       appropriate padding
//-----------------------------------------------------------------------------
HRESULT CBundler::FlushDataFile()
{
    BOOL bWriteOK;
    DWORD cb;
    XPR_HEADER xprh;
    BYTE * pbTemp;
    DWORD cbFill = 0;

    // Pad the data buffer out to be a multiple of the DVD sector size (2k).
    // This is needed in order to DMA the file off the DVD drive on the
    // Xbox.
    PadToAlignment( 2048 );

    xprh.dwMagic = XPR_MAGIC_VALUE;
    xprh.dwHeaderSize = m_cbHeader + sizeof( XPR_HEADER );

    // We may need to pad the file to a sector multiple for the 
    // start of the data block
    if( xprh.dwHeaderSize % 2048 )
    {
        cbFill = 2048 - ( xprh.dwHeaderSize % 2048 );
        xprh.dwHeaderSize += cbFill;
    }
    xprh.dwTotalSize = xprh.dwHeaderSize + m_cbData;

    // Write out header stuff
    if( !WriteFile( m_hfXPR, &xprh, sizeof( XPR_HEADER ), &cb, NULL ) )
        return E_FAIL;
    if( !WriteFile( m_hfXPR, m_pbHeaderBuff, m_cbHeader, &cb, NULL ) )
        return E_FAIL;

    if( cbFill )
    {
        // Allocate a buffer for fill data
        pbTemp = (BYTE *)malloc( cbFill );
        memset( pbTemp, 0xDEAD, cbFill );

        // Write it out
        bWriteOK = WriteFile( m_hfXPR, pbTemp, cbFill, &cb, NULL );
        free( pbTemp );
        if( !bWriteOK )
            return E_FAIL;
    }

    if( !WriteFile( m_hfXPR, m_pbDataBuff, m_cbData, &cb, NULL ) )
        return E_FAIL;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: RESOURCE_STRUCT_cmp
// Desc: Helper file for sorting list of resources
//-----------------------------------------------------------------------------
int __cdecl RESOURCE_STRUCT_cmp(const void *elem1, const void *elem2 )
{
    RESOURCE_STRUCT *p1 = (RESOURCE_STRUCT *)elem1;
    RESOURCE_STRUCT *p2 = (RESOURCE_STRUCT *)elem2;
    return strcmp(p1->strIdentifier, p2->strIdentifier);
}

//-----------------------------------------------------------------------------
// Name: WriteSymbolTable
// Desc: Write a symbol table resource
//-----------------------------------------------------------------------------
HRESULT CBundler::WriteSymbolTable()
{
    // sort list of resources
    qsort(m_aResources, m_nResourceCount, sizeof(RESOURCE_STRUCT), RESOURCE_STRUCT_cmp);

    // Add symbol table resource at the end
    CHAR *strIdentifier = m_aResources[m_nResourceCount].strIdentifier;
    ZeroMemory(strIdentifier, BUNDLER_IDENTIFIER_MAX_LENGTH);
    strcpy(strIdentifier, "SYMBOLTABLE");
    DWORD cbHeader0 = m_aResources[m_nResourceCount].dwOffset = m_cbHeader; // offset to UserData struct
    m_nResourceCount++;
    
    // get the size of all the resource strings
    DWORD StringSize = 0;
    for (DWORD i = 0; i < m_nResourceCount; i++)
        StringSize += strlen(m_aResources[i].strIdentifier) + 1;

    // get size of header
    DWORD HeaderSize = sizeof(DWORD)                // number of symbols
        + m_nResourceCount * 2 * sizeof(DWORD);     // (name, offset) pairs

    // Helper writing macro
#define WRITE(POINTER, SIZE)                    \
    if (FAILED(WriteHeader( POINTER, SIZE )))   \
        return E_FAIL;                          \
    m_cbHeader += SIZE; // TODO: this should be done in WriteHeader

    // write header to bundle
    struct UserData {
        DWORD dwType;               // XBRC_SYMBOLTABLE
        DWORD dwSize;               // total size (not including this header)
    } SymbolTableUserData;
    SymbolTableUserData.dwType = XBRC_SYMBOLTABLE;
    SymbolTableUserData.dwSize = HeaderSize + StringSize;
    WRITE( &SymbolTableUserData, sizeof(UserData) );

    // write count of symbols
    WRITE( &m_nResourceCount, sizeof(DWORD) );
        
    // write (name, offset) tags to header file
    DWORD cbStringOffset = cbHeader0 + sizeof(UserData) + HeaderSize;
    for (i = 0; i < m_nResourceCount; i++)
    {
        WRITE( &cbStringOffset, sizeof(DWORD) );    // offset to list of strings at end
        cbStringOffset += strlen(m_aResources[i].strIdentifier) + 1;
        WRITE( &m_aResources[i].dwOffset, sizeof(DWORD) );
    }

    // write the strings to the header file
    for (i = 0; i < m_nResourceCount; i++)
    {
        size_t length = strlen(m_aResources[i].strIdentifier) + 1;
        WRITE( m_aResources[i].strIdentifier, length ); // includes NULL
    }
    
#undef WRITE        
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: WriteHeaderFile()
// Desc: Writes out a .h file with useful definitions:
//       * NUM_RESOURCES
//       * XXX_OFFSET for each resource
//-----------------------------------------------------------------------------
HRESULT CBundler::WriteHeaderFile()
{
    DWORD i;
    DWORD cb;
    char strBuff[256];

    sprintf( strBuff, "// Automatically generated by the xbrc tool from %s\n\n", m_strRDF );
    if( !WriteFile( m_hfHDR, strBuff, strlen(strBuff), &cb, NULL ) )
        return E_FAIL;

    // strip root filename out of m_strRDF;
    CHAR strPrefix[MAX_PATH];

    if( m_strPrefix[0] )
    {
        strcpy( strPrefix, m_strPrefix );
    }
    else
    {
        // Use the resource name as a prefix, after removing prepending path
        // and file extension.
        CHAR drive[_MAX_DRIVE];
        CHAR dir[_MAX_DIR];
        CHAR name[_MAX_PATH];
        CHAR ext[_MAX_EXT];
        _splitpath( m_strRDF, drive, dir, name, ext );
        strcpy(strPrefix, name);
    }

    // Write out the number of resources
    sprintf( strBuff, "#define %s_NUM_RESOURCES %dUL\n\n", strPrefix, m_nResourceCount );
    if( !WriteFile( m_hfHDR, strBuff, strlen(strBuff), &cb, NULL ) )
        return E_FAIL;

    // Make the output pretty
    CHAR *strSpaces = "                                                    ";
    UINT MaxLength = 0;
    
    // Write out the offset of each resource
    if( m_bSingleTexture )
    {
        sprintf( strBuff, "#define %s_OFFSET %dUL\n", strPrefix, 
                          m_aResources[0].dwOffset );
        if( !WriteFile( m_hfHDR, strBuff, strlen(strBuff), &cb, NULL ) )
            return E_FAIL;
    }
    else
    {
        for( i = 0; i < m_nResourceCount; i++ )
        {
            UINT Length = strlen(m_aResources[i].strIdentifier);
            if (Length > MaxLength) MaxLength = Length;
        }
        for( i = 0; i < m_nResourceCount; i++ )
        {
            sprintf( strBuff, "#define %s_%s_OFFSET %.*s%dUL\n",
                     strPrefix, m_aResources[i].strIdentifier,
                     MaxLength - strlen(m_aResources[i].strIdentifier), strSpaces,
                     m_aResources[i].dwOffset );
            if( !WriteFile( m_hfHDR, strBuff, strlen(strBuff), &cb, NULL ) )
                return E_FAIL;
        }
    }

    // Write out debug map of names to resources
    strcpy( strBuff, "\n#ifdef _DEBUG\n" );
    if( !WriteFile( m_hfHDR, strBuff, strlen(strBuff), &cb, NULL ) )
        return E_FAIL;
    sprintf( strBuff, "XBRESOURCE %s_ResourceTags[%s_NUM_RESOURCES] = {\n", strPrefix, strPrefix );
    if( !WriteFile( m_hfHDR, strBuff, strlen(strBuff), &cb, NULL ) )
        return E_FAIL;
    if( m_bSingleTexture )
    {
        sprintf( strBuff, "\t{ \"%s\",\t%s_OFFSET },\n", strPrefix, strPrefix );
        if( !WriteFile( m_hfHDR, strBuff, strlen(strBuff), &cb, NULL ) )
            return E_FAIL;
    }
    else
    {
        // write list of resources
        for( i = 0; i < m_nResourceCount; i++ )
        {
            sprintf( strBuff, "\t{ \"%s_%s\", %.*s%s_%s_OFFSET },\n",
                     strPrefix, m_aResources[i].strIdentifier,
                     MaxLength - strlen(m_aResources[i].strIdentifier), strSpaces,
                     strPrefix, m_aResources[i].strIdentifier);
            if( !WriteFile( m_hfHDR, strBuff, strlen(strBuff), &cb, NULL ) )
                return E_FAIL;
        }
    }
    strcpy( strBuff, "};\n#endif\n" );
    if( !WriteFile( m_hfHDR, strBuff, strlen(strBuff), &cb, NULL ) )
        return E_FAIL;
    
    
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SetOutPackedResourceFile
// Desc: Takes over to handle the out_packedresource token.  
//-----------------------------------------------------------------------------
HRESULT CBundler::SetOutPackedResourceFile(CHAR *strResourceFile)
{
    HRESULT hr = ValidateType( strResourceFile, TT_FILENAME );
    if (FAILED(hr))
        return hr;
    
    if( m_nResourceCount > 0 )
    {
        ErrorMsg( "Warning: output files must be specified before resources\n" );
    }
    else if( !m_bExplicitXPR )
    {
        if( strchr( strResourceFile, ':' ) )
            strcpy( m_strXPR, strResourceFile );
        else
        {
            strcpy( m_strXPR, m_strPath );
            strcat( m_strXPR, strResourceFile );
        }
    }
    else
    {
        ErrorMsg( "Warning: out_packedresource overriden by command line\n" );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetOutHeaderFile
// Desc: Takes over to handle the out_header token.  
//-----------------------------------------------------------------------------
HRESULT CBundler::SetOutHeaderFile(CHAR *strHeaderFile)
{
    HRESULT hr = ValidateType( strHeaderFile, TT_FILENAME );
    if (FAILED(hr))
        return hr;

    if( m_nResourceCount > 0 )
    {
        ErrorMsg( "Warning: output files must be specified before resources\n" );
    }
    else if( !m_bExplicitHDR )
    {
        if( strchr( strHeaderFile, ':' ) )
            strcpy( m_strHDR, strHeaderFile );
        else
        {
            strcpy( m_strHDR, m_strPath );
            strcat( m_strHDR, strHeaderFile );
        }
    }
    else
    {
        ErrorMsg( "Warning: out_header overriden by command line\n" );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetOutPrefix
// Desc: Set the output prefix
//-----------------------------------------------------------------------------
HRESULT CBundler::SetOutPrefix(CHAR *strPrefix)
{
    HRESULT hr = ValidateType( strPrefix, TT_ANY );
    if (FAILED(hr))
        return hr;

    if( m_nResourceCount > 0 )
    {
        ErrorMsg( "Warning: output prefix must be specified before resources\n" );
    }
    else if( !m_bExplicitPrefix )
    {
        strcpy( m_strPrefix, strPrefix );
    }
    else
    {
        ErrorMsg( "Warning: out_prefix overriden by command line\n" );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetOutErrorFile()
// Desc: Takes over to handle the out_error token.  
//-----------------------------------------------------------------------------
HRESULT CBundler::SetOutErrorFile(CHAR *strErrorFile)
{
    HRESULT hr = ValidateType( strErrorFile, TT_FILENAME );
    if (FAILED(hr))
        return hr;

    if( m_nResourceCount > 0 )
    {
        ErrorMsg( "Warning: output files must be specified before resources\n" );
    }
    else if( !m_bExplicitERR )
    {
        if( strchr( strErrorFile, ':' ) )
            strcpy( m_strERR, strErrorFile );
        else
        {
            strcpy( m_strERR, m_strPath );
            strcat( m_strERR, strErrorFile );
        }
    }
    else
    {
        ErrorMsg( "Warning: out_error overriden by command line\n" );
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: WriteHeader()
// Desc: Copies resource header info to the header buffer.  Grows the header
//       buffer by multiples of 2.
//-----------------------------------------------------------------------------
HRESULT CBundler::WriteHeader( void * pbBuff, DWORD cb )
{
    DWORD cbNew = m_cbHeaderAlloc;

    // Find the next power-of-2 size that is big enough to hold the data
    while( m_cbHeader + cb > cbNew )
        cbNew <<= 1;

    // Realloc if necessary
    if( cbNew > m_cbHeaderAlloc )
    {
        BYTE * pbTemp = (BYTE *)realloc( m_pbHeaderBuff, cbNew );
        if( !pbTemp )
            return E_OUTOFMEMORY;
        m_pbHeaderBuff = pbTemp;
        m_cbHeaderAlloc = cbNew;
    }

    // Copy over the data
    memcpy( m_pbHeaderBuff + m_cbHeader, pbBuff, cb );
    // m_pBundler->m_cbHeader += cb;    // Why don't we do this here?

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: WriteData()
// Desc: Copies data to the data buffer.  Grows our data buffer by multiples
//       of 2
//-----------------------------------------------------------------------------
HRESULT CBundler::WriteData( void * pbBuff, DWORD cb )
{
    DWORD cbNew = m_cbDataAlloc;

    //
    // Find the next power-of-2 size that is 
    // big enough to hold the data
    //
    while( m_cbData + cb > cbNew )
        cbNew <<= 1;
    
    // Realloc if necessary
    if( cbNew > m_cbDataAlloc )
    {
        BYTE * pbTemp = (BYTE *)realloc( m_pbDataBuff, cbNew );
        if( !pbTemp )
            return E_OUTOFMEMORY;
        m_pbDataBuff = pbTemp;
        m_cbDataAlloc = cbNew;
    }

    // Copy over the data
    memcpy( m_pbDataBuff + m_cbData, pbBuff, cb );
    m_cbData += cb;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: PadToAlignment()
// Desc: Pads the data buffer to the specified alignment
//-----------------------------------------------------------------------------
HRESULT CBundler::PadToAlignment( DWORD dwAlign )
{
    HRESULT hr = S_OK;
    BYTE * pbTemp = NULL;
    DWORD cbFill = 0;

    if( m_cbData % dwAlign )
        cbFill = dwAlign - ( m_cbData % dwAlign );

    if( cbFill )
    {
        pbTemp = (BYTE *)malloc( cbFill );
        memset( pbTemp, 0xDEAD, cbFill );

        hr = WriteData( pbTemp, cbFill );
        
        free( pbTemp );
    }

    return hr;
}


//-----------------------------------------------------------------------------
// Name: IsExistingIdentifier()
// Desc: Returns TRUE if we've already processed a resource with the given
//       identifier
//-----------------------------------------------------------------------------
BOOL CBundler::IsExistingIdentifier( char * strIdentifier, DWORD *pdwIndex )
{
    DWORD dwIndex;
    DWORD dwStrLen = strlen( strIdentifier );

    for( dwIndex = 0; dwIndex < m_nResourceCount; dwIndex++ )
    {
        if( strncmp( strIdentifier, m_aResources[ dwIndex ].strIdentifier, dwStrLen ) == 0
            && strlen( m_aResources[ dwIndex ].strIdentifier ) == dwStrLen)
        {
            if (pdwIndex)
                *pdwIndex = dwIndex + 1;    // use base-one indices to reserve 0 for NULL
            return TRUE;
        }
    }

    return FALSE;
}


/*

//-----------------------------------------------------------------------------
// Name: Process()
// Desc: Processes the resource definition file and processes it to create our
//       Xbox Packed Resource (xpr) file and header file
//-----------------------------------------------------------------------------
HRESULT CBundler::Process()
{
    HRESULT hr;
    BUNDLERTOKEN tok;

    // Special case: single-texture conversion
    if( m_bSingleTexture )
    {
        CTexture2D tex(this);
        DWORD cbHeader, cbData;
        
        if( strrchr( m_strRDF, '\\' ) )
            strcpy( tex.m_strSource, strrchr( m_strRDF, '\\' )+1 );
        else
            strcpy( tex.m_strSource, m_strRDF );
        
        // Save the texture to the bundle
        hr = tex.SaveToBundle( &cbHeader, &cbData );
        if( FAILED( hr ) )
            return hr;
    
        printf( "Texture: Wrote %s out in format %s (%d bytes)\n", 
                 tex.m_strSource, tex.m_strFormat, cbData );
        printf( "     %d x %d, %d levels\n\n", tex.m_dwWidth, tex.m_dwHeight, tex.m_dwLevels );
    
        m_aResources[m_nResourceCount++].dwOffset = m_cbHeader;
        m_cbHeader += cbHeader;

        HandleEOFToken();

        return S_OK;
    }
    
    // Open the file
    m_hfRDF = CreateFile( m_strRDF, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
    if( INVALID_HANDLE_VALUE == m_hfRDF )
    {
        ErrorMsg( "Error: Can't open <%s>\n", m_strRDF );
        return E_FAIL;
    }

    // Iterate over top-level tokens (Texture, out_xxxx, EOF, etc)
    for( ; ; )
    {
        hr = GetNextToken( &tok );
        if( FAILED( hr ) )
            return hr;

        hr = (this->*tok.pfnHandler)();
        if( FAILED( hr ) )
            return hr;
        else if( hr == S_FALSE  )
            break;

    }

    return S_OK;
}

*/

/*

//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program
//-----------------------------------------------------------------------------
int _cdecl main( int argc, char * argv[] )
{
    HRESULT hr;
    CBundler bundler;

    hr = bundler.Initialize( argc, argv );
    if(hr == E_INVALIDARG)
    {
        bundler.PrintUsage();
        return 0;
    }
    else if(FAILED(hr))
    {
        bundler.ErrorMsg( "ERROR: D3D Initialization failed.\n" );
        return 0;
    }

    hr = bundler.Process();
    if( FAILED( hr ) )
        return 0;

    return 0;
}

*/

