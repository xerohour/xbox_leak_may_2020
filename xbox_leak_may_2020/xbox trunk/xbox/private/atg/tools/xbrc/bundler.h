//-----------------------------------------------------------------------------
// File: Bundler.h
//
// Desc: Header file for the bundler tool
//
// Hist: 2001.02.06 - New for March XDK release
//       2002.01.31 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef BUNDLER_H
#define BUNDLER_H

#include <assert.h>
#include <windows.h>
#include <d3dx8.h>

#include "xbrc.h"

// Default extensions for output files
#define XPREXT ".xbr"	/* use .xbr instead of .xpr for packed resource files that contain vertex shaders, etc. */
#define HDREXT ".h"
#define ERREXT ".err"

// $BUGBUG 11551
#define MAX_RESOURCES 10000

#define TOKEOF ((CHAR)0xff)

class CBundler;

/*
  typedef HRESULT (CBundler::* TOKENHANDLERFN)();


// Enums for all of our known tokens
typedef enum 
{ 
//    TOKEN_RESOURCE_OPENBRACE             = 0x00000001,
//    TOKEN_RESOURCE_CLOSEBRACE            = 0x00000002,
    TOKEN_RESOURCE_EOF                   = 0x00000003,
    TOKEN_RESOURCE_OUT_PACKEDRESOURCE    = 0x00000004,
    TOKEN_RESOURCE_OUT_HEADER            = 0x00000005, 
    TOKEN_RESOURCE_OUT_PREFIX            = 0x00000006, 
    TOKEN_RESOURCE_OUT_ERROR             = 0x00000007,
    TOKEN_RESOURCE_TEXTURE               = 0x00000010,
    TOKEN_RESOURCE_CUBEMAP               = 0x00000030,
    TOKEN_RESOURCE_VOLUMETEXTURE         = 0x00000050,
    TOKEN_RESOURCE_VERTEXBUFFER          = 0x00000100,
    TOKEN_RESOURCE_USERDATA              = 0x00000200,

    // Properties for a resource have the resource
    // value shifted over by 16 bits
    TOKEN_PROPERTY_TEXTURE                = 0x00100000,
    TOKEN_PROPERTY_TEXTURE_SOURCE         = 0x00100001,
    TOKEN_PROPERTY_TEXTURE_ALPHASOURCE    = 0x00100002,
    TOKEN_PROPERTY_TEXTURE_FILTER         = 0x00100003,
    TOKEN_PROPERTY_TEXTURE_FORMAT         = 0x00100004,
    TOKEN_PROPERTY_TEXTURE_WIDTH          = 0x00100005,
    TOKEN_PROPERTY_TEXTURE_HEIGHT         = 0x00100006,
    TOKEN_PROPERTY_TEXTURE_LEVELS         = 0x00100007,

    TOKEN_PROPERTY_CUBEMAP                = 0x00300000,
    TOKEN_PROPERTY_CUBEMAP_SOURCE_XP      = 0x00300001,
    TOKEN_PROPERTY_CUBEMAP_SOURCE_XN      = 0x00300002,
    TOKEN_PROPERTY_CUBEMAP_SOURCE_YP      = 0x00300003,
    TOKEN_PROPERTY_CUBEMAP_SOURCE_YN      = 0x00300004,
    TOKEN_PROPERTY_CUBEMAP_SOURCE_ZP      = 0x00300005,
    TOKEN_PROPERTY_CUBEMAP_SOURCE_ZN      = 0x00300006,
    TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_XP = 0x00300011,
    TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_XN = 0x00300012,
    TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_YP = 0x00300013,
    TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_YN = 0x00300014,
    TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_ZP = 0x00300015,
    TOKEN_PROPERTY_CUBEMAP_ALPHASOURCE_ZN = 0x00300016,
    TOKEN_PROPERTY_CUBEMAP_SIZE           = 0x00300022,

    TOKEN_PROPERTY_VOLUMETEXTURE             = 0x00500000,
//    TOKEN_PROPERTY_VOLUMETEXTURE_SOURCE      = 0x00500001,
//    TOKEN_PROPERTY_VOLUMETEXTURE_ALPHASOURCE = 0x00500002,
    TOKEN_PROPERTY_VOLUMETEXTURE_DEPTH       = 0x00500003,

    TOKEN_PROPERTY_VB                     = 0x01000000,
    TOKEN_PROPERTY_VB_VERTEXDATA          = 0x01000001,
    TOKEN_PROPERTY_VB_VERTEXFORMAT        = 0x01000002,

    TOKEN_PROPERTY_USERDATA               = 0x02000000,
    TOKEN_PROPERTY_USERDATA_DATAFILE      = 0x02000001,

} TOKEN_ID;
*/

// Types of tokens for validation
typedef enum
{
    TT_ANY = 0,
    TT_IDENTIFIER,
    TT_HEXNUMBER,
    TT_INTEGER,
    TT_FLOAT,
    TT_FILENAME,
} TOKENTYPE;


/*
// Token struct that is passed around
typedef struct _BUNDLERTOKEN
{
    char *          strKeyword;
    TOKEN_ID        eType;
    TOKENTYPE       propType;
    TOKENHANDLERFN  pfnHandler;
} BUNDLERTOKEN;
*/


// Resource struct for tracking
#define BUNDLER_IDENTIFIER_MAX_LENGTH 128
typedef struct 
{
    char    strIdentifier[BUNDLER_IDENTIFIER_MAX_LENGTH];
    DWORD   dwOffset;
} RESOURCE_STRUCT;




//-----------------------------------------------------------------------------
// Name: class CBundler
// Desc: Main application class for driving the bundler.  Handles data 
//       management, resource tracking, file management, and tokenizing
//-----------------------------------------------------------------------------
class CBundler
{
public:
    CBundler();
    ~CBundler();

    HRESULT Begin(CHAR *strBase);	// default base name of resource, header, and error files
	
    HRESULT SetOutPackedResourceFile(CHAR *strResourceFile);
	HRESULT SetOutHeaderFile(CHAR *strHeaderFile);
    HRESULT SetOutPrefix(CHAR *strPrefix);
    HRESULT SetOutErrorFile(CHAR *strErrorFile);
	
    HRESULT InitD3D();	// used only for image types not handled by LoadImage
    HRESULT WriteHeader( void * pbBuff, DWORD cb );
    HRESULT WriteData( void * pbBuff, DWORD cb );
    HRESULT PadToAlignment( DWORD dwAlign );	// data alignment

    HRESULT End();	// flush output, write files
	
    DWORD   FilterFromString( char* strFilter );

    // D3D stuff for manipulating textures
    LPDIRECT3D8 m_pd3d;
    LPDIRECT3DDEVICE8 m_pd3ddev;
	
    // Routine to call w/ error messages - prints to stderr and file
    void ErrorMsg( char * strFmt, ... );

    BOOL IsExistingIdentifier( char * strIdentifier, DWORD *pIndex = NULL );

    DWORD m_cbHeader;
    DWORD m_cbData;

public:

    HRESULT OpenFiles();            // Open output files
    HRESULT CloseFiles();           // Close all files
    HRESULT FlushDataFile();        // Flush from buffer to output file
    HRESULT WriteSymbolTable();     // Add a symbol table resource to the header
    HRESULT WriteHeaderFile();      // Create .h header file
	
    HRESULT ValidateType( CHAR* strToken, TOKENTYPE tt );

    // Filenames
	CHAR   m_strPath[MAX_PATH];
    CHAR   m_strRDF[MAX_PATH];
    CHAR   m_strXPR[MAX_PATH];
    CHAR   m_strHDR[MAX_PATH];
    CHAR   m_strERR[MAX_PATH];

	// Prefix for header file constants
    CHAR   m_strPrefix[MAX_PATH];

    // File handles
    HANDLE m_hfRDF;
    HANDLE m_hfXPR;
    HANDLE m_hfHDR;
    HANDLE m_hfERR;

    // Data buffer management
    BYTE*  m_pbHeaderBuff;
    DWORD  m_cbHeaderAlloc;
    BYTE*  m_pbDataBuff;
    DWORD  m_cbDataAlloc;

    // Resource tracking
    RESOURCE_STRUCT  m_aResources[MAX_RESOURCES];
    DWORD  m_nResourceCount;

    // For specifying output files and prefix on cmd line vs. in RDF
    BOOL   m_bExplicitXPR;
    BOOL   m_bExplicitHDR;
    BOOL   m_bExplicitERR;
	BOOL   m_bExplicitPrefix;

    // Indicates a single-texture conversion
    BOOL   m_bSingleTexture;
};


#endif // BUNDLER_H
