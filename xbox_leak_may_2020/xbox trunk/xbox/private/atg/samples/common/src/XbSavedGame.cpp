//-----------------------------------------------------------------------------
// File: XbSavedGame.cpp
//
// Desc: Saved game container
//
// Hist: 02.05.01 - New for March XDK release
//       03.14.01 - Access image.xbx for game image (April XDK)
//       04.13.01 - GetSize() accounts for directory and cluster overhead
//       05.01.01 - Image.xbx changed to SaveImage.xbx to match specs/docs
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "XbSavedGame.h"
#include "Xbapp.h"
#include "Xbutil.h"
#include <xgraphics.h>
#include <cassert>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const CHAR* const strUSER_REGION_HD = "U:\\";
const CHAR* const strGAME_IMAGE = "SaveImage.xbx";
const DWORD IMAGE_HDR_SIZE = 2048;             // 2K
const DWORD IMAGE_WH = 64;                     // width and height
const DWORD IMAGE_DATA_SIZE = (IMAGE_WH * IMAGE_WH) / 2; // DXT1 is 4 bits per pixel
const DWORD BLOCK_SIZE = 16 * 1024; // TCR 3-11 Space Display
                                    




//-----------------------------------------------------------------------------
// Layout of SaveImage.xbx saved game image file
//-----------------------------------------------------------------------------
//
// File is XPR0 format. Since the XPR will always contain only a single
// 256x256 DXT1 image, we know exactly what the header portion will look like

struct XprImageHeader
{
    XPR_HEADER        xpr;           // Standard XPR struct
    IDirect3DTexture8 txt;           // Standard D3D texture struct
    DWORD             dwEndOfHeader; // 0xFFFFFFFF
};

struct XprImage
{
    XprImageHeader hdr;
    CHAR           strPad[ IMAGE_HDR_SIZE - sizeof( XprImageHeader ) ];
    BYTE           pBits[ IMAGE_DATA_SIZE ];     // data bits
};




//-----------------------------------------------------------------------------
// Helper class to wrap FindFirstFile HANDLE
//-----------------------------------------------------------------------------
class FindHandle
{

    HANDLE m_hFind;

public:

    explicit FindHandle( HANDLE h = NULL )
    : 
        m_hFind( h )
    {
    }

    ~FindHandle()
    {
        if( m_hFind != INVALID_HANDLE_VALUE )
            FindClose( m_hFind );
    }

    operator HANDLE() const
    {
        return m_hFind;
    }

};




//-----------------------------------------------------------------------------
// Name: CXBSavedGame()
// Desc: Construct empty saved game
//-----------------------------------------------------------------------------
CXBSavedGame::CXBSavedGame()
:
    m_strName(),
    m_strDir(),
    m_dwSize( DWORD(-1) ),
    m_pImage( NULL ),
    m_bIsValidImage( TRUE ),
    m_qwLastWriteTime( ULONGLONG(0) )
{
    Init();
}




//-----------------------------------------------------------------------------
// Name: CXBSavedGame()
// Desc: Construct from other save
//-----------------------------------------------------------------------------
CXBSavedGame::CXBSavedGame( const CXBSavedGame& rhs )
:
    m_strName        (),
    m_strDir         (),
    m_dwSize         ( rhs.m_dwSize ),
    m_pImage         ( NULL ),
    m_bIsValidImage  ( rhs.m_bIsValidImage ),
    m_qwLastWriteTime( rhs.m_qwLastWriteTime )
{
    lstrcpyW( m_strName, rhs.m_strName );
    lstrcpyA( m_strDir,  rhs.m_strDir );
    if( rhs.m_pImage != NULL )
    {
        m_pImage = rhs.m_pImage;
        m_pImage->AddRef();
    }
}




//-----------------------------------------------------------------------------
// Name: operator =()
// Desc: Copy from other save
//-----------------------------------------------------------------------------
CXBSavedGame& CXBSavedGame::operator =( const CXBSavedGame& rhs )
{
    lstrcpyW( m_strName, rhs.m_strName );
    lstrcpyA( m_strDir,  rhs.m_strDir );

    m_dwSize          = rhs.m_dwSize;
    m_bIsValidImage   = rhs.m_bIsValidImage;
    m_qwLastWriteTime = rhs.m_qwLastWriteTime;

    SAFE_RELEASE( m_pImage );

    if( rhs.m_pImage != NULL )
    {
        m_pImage = rhs.m_pImage;
        m_pImage->AddRef();
    }
    return *this;
}




//-----------------------------------------------------------------------------
// Name: ~CXBSavedGame()
// Desc: Tear down save game
//-----------------------------------------------------------------------------
CXBSavedGame::~CXBSavedGame()
{
    SAFE_RELEASE( m_pImage );
}




//-----------------------------------------------------------------------------
// Name: CXBSavedGame()
// Desc: Construct saved game from XGAME_FIND_DATA (see XFindFirstSaveGame())
//-----------------------------------------------------------------------------
CXBSavedGame::CXBSavedGame( const XGAME_FIND_DATA& XFindData )
:
    m_strName(),
    m_strDir(),
    m_dwSize( DWORD(-1) ),
    m_pImage( NULL ),
    m_bIsValidImage( TRUE ),
    m_ftLastWriteTime( XFindData.wfd.ftLastWriteTime )
{
    lstrcpynW( m_strName, XFindData.szSaveGameName, MAX_GAMENAME );
    lstrcpynA( m_strDir, XFindData.szSaveGameDirectory, MAX_PATH );
}




//-----------------------------------------------------------------------------
// Name: CXBSavedGame()
// Desc: Construct saved game
//-----------------------------------------------------------------------------
CXBSavedGame::CXBSavedGame( const WCHAR* strSaveGameName, 
                            const CHAR* strSaveGameDir )
:
    m_strName(),
    m_strDir(),
    m_dwSize( DWORD(-1) ),
    m_pImage( NULL ),
    m_bIsValidImage( TRUE ),
    m_qwLastWriteTime( ULONGLONG(0) )
{
    assert( strSaveGameName != NULL );
    assert( strSaveGameDir != NULL );
    lstrcpynW( m_strName, strSaveGameName, MAX_GAMENAME );
    lstrcpynA( m_strDir, strSaveGameDir, MAX_PATH );
}




//-----------------------------------------------------------------------------
// Name: CreateGame()
// Desc: TRUE if game container successfully created on the device
//-----------------------------------------------------------------------------
BOOL CXBSavedGame::CreateGame( CHAR chDestDrive, const WCHAR* strSaveGameName )
{
    return OpenGame( chDestDrive, strSaveGameName, CREATE_NEW );
}




//-----------------------------------------------------------------------------
// Name: OpenGame()
// Desc: TRUE if game container exists on the device
//-----------------------------------------------------------------------------
BOOL CXBSavedGame::OpenGame( CHAR chDestDrive, const WCHAR* strSaveGameName,
                             DWORD dwOpenDisposition )
{
    assert( strSaveGameName != NULL );
    Init();

    CHAR strRootPath[4] = "x:\\";
    *strRootPath = chDestDrive;

    // TCR 3-7 Saved Game Management
    DWORD dwSuccess = XCreateSaveGame( strRootPath, strSaveGameName, 
                                       dwOpenDisposition, 0, m_strDir, 
                                       MAX_PATH );
    if( dwSuccess != ERROR_SUCCESS )
        return FALSE;

    lstrcpynW( m_strName, strSaveGameName, MAX_GAMENAME );
    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: DeleteGame()
// Desc: TRUE if game container exists and successfully deleted
//-----------------------------------------------------------------------------
BOOL CXBSavedGame::DeleteGame()
{
    assert( !IsEmpty() );
    CHAR strRootPath[4] = "x:\\";
    *strRootPath = GetDrive();

    // TCR 3-7 Saved Game Management
    DWORD dwSuccess = XDeleteSaveGame( strRootPath, m_strName );
    Init();
    return( dwSuccess == ERROR_SUCCESS );
}




//-----------------------------------------------------------------------------
// Name: IsEmpty()
// Desc: TRUE if object doesn't represent an actual saved game
//-----------------------------------------------------------------------------
BOOL CXBSavedGame::IsEmpty() const
{
    return( *m_strName == 0 );
}




//-----------------------------------------------------------------------------
// Name: GetDrive()
// Desc: Saved game drive
//-----------------------------------------------------------------------------
CHAR CXBSavedGame::GetDrive() const
{
    return *m_strDir;
}




//-----------------------------------------------------------------------------
// Name: GetSize()
// Desc: Number of bytes used by save game container and data.
//       The value returned is cluster based, so it includes the total number
//       of bytes allocated by the device, and is always evenly divisible by
//       the cluster size of the device.
//-----------------------------------------------------------------------------
DWORD CXBSavedGame::GetSize() const
{
    if( IsEmpty() )
        return 0;

    // Cache the save game container size if we don't have it already
    if( m_dwSize == DWORD(-1) )
    {
        CHAR strRootPath[4] = "x:\\";
        *strRootPath = GetDrive();

        DWORD dwBlocks = XGetDisplayBlocks( m_strDir );
        m_dwSize = dwBlocks * BLOCK_SIZE;
    }

    return m_dwSize;
}




//-----------------------------------------------------------------------------
// Name: GetLastWriteTime()
// Desc: Time/date when saved game container folder was last written
//-----------------------------------------------------------------------------
FILETIME CXBSavedGame::GetLastWriteTime() const
{
    if( IsEmpty() )
        return FILETIME();

    // Cache the last write time if we don't have it already
    if( m_qwLastWriteTime == ULONGLONG(0) )
    {
        // FindFirstFile() doesn't want trailing backslash
        CHAR strSaveGameDir[ MAX_PATH ];
        lstrcpynA( strSaveGameDir, m_strDir, lstrlenA( m_strDir ) );

        // Get folder last write time
        WIN32_FIND_DATA FolderData;
        FindHandle hFolder( FindFirstFile( strSaveGameDir, &FolderData ) );
        if( hFolder != INVALID_HANDLE_VALUE )
        {
            m_ftLastWriteTime.dwHighDateTime = FolderData.ftLastWriteTime.dwHighDateTime;
            m_ftLastWriteTime.dwLowDateTime = FolderData.ftLastWriteTime.dwLowDateTime;
        }
    }
    return m_ftLastWriteTime;
}




//-----------------------------------------------------------------------------
// Name: GetLastWriteQword()
// Desc: Last write info as ULONGLONG
//-----------------------------------------------------------------------------
ULONGLONG CXBSavedGame::GetLastWriteQword() const
{
    FILETIME ftLastWriteTime = GetLastWriteTime();
    return( *(ULONGLONG*)( &ftLastWriteTime ) );
}




//-----------------------------------------------------------------------------
// Name: GetName()
// Desc: Saved game name
//-----------------------------------------------------------------------------
const WCHAR* CXBSavedGame::GetName() const
{
    return m_strName;
}




//-----------------------------------------------------------------------------
// Name: GetDirectory()
// Desc: Saved game container folder
//-----------------------------------------------------------------------------
const CHAR* CXBSavedGame::GetDirectory() const
{
    return m_strDir;
}




//-----------------------------------------------------------------------------
// Name: GetImage()
// Desc: Returns pointer to texture for game image. Caller is responsible for
//       calling Release() on the pointer.
//-----------------------------------------------------------------------------
BOOL CXBSavedGame::GetImage( LPDIRECT3DTEXTURE8* ppImage ) const
{
    if( IsEmpty() )
        return FALSE;

    // If we already have the image cached, just update the ref count
    if( m_pImage != NULL )
    {
        m_pImage->AddRef();
        *ppImage = m_pImage;
        return TRUE;
    }

    // If we already failed to load the image, bail out
    if( !m_bIsValidImage )
        return FALSE;

    // We don't have the image yet

    // See if the image file for this saved game exists
    CHAR strFile[ MAX_PATH ];
    lstrcpyA( strFile, m_strDir );
    lstrcatA( strFile, strGAME_IMAGE );
    HANDLE hFile = CreateFile( strFile, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                               0, NULL );
    if( hFile == INVALID_HANDLE_VALUE )
    {
        // TCR 2-14a Saved Game Representative Image
        // No specific image found; see if the default save image exists
        // (always on root of user data region on hard disk)
        lstrcpyA( strFile, strUSER_REGION_HD );
        lstrcatA( strFile, strGAME_IMAGE );
        hFile = CreateFile( strFile, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                            0, NULL );
        if( hFile == INVALID_HANDLE_VALUE )
        {
            m_bIsValidImage = FALSE;
            return FALSE;
        }
    }

    // Read the image header from disk
    XprImageHeader XprHeader;
    DWORD dwBytesRead;
    BOOL bSuccess = ReadFile( hFile, &XprHeader, sizeof( XprImageHeader ),
                              &dwBytesRead, NULL );

    // Validate the image
    bSuccess &= dwBytesRead == sizeof( XprImageHeader ) &&
                XprHeader.xpr.dwMagic == XPR_MAGIC_VALUE &&
                XprHeader.xpr.dwTotalSize == IMAGE_HDR_SIZE + IMAGE_DATA_SIZE &&
                XprHeader.xpr.dwHeaderSize == IMAGE_HDR_SIZE &&
                XprHeader.dwEndOfHeader == 0xFFFFFFFF;

    // If image looks good, store the bits in a texture
    if( bSuccess )
    {
        HRESULT hr = g_pd3dDevice->CreateTexture( IMAGE_WH, IMAGE_WH, 
                                                  1, 0, D3DFMT_DXT1,
                                                  D3DPOOL(), &m_pImage );
        bSuccess = SUCCEEDED(hr);
        if( bSuccess )
        {
            D3DLOCKED_RECT lr;
            m_pImage->LockRect( 0, &lr, NULL, D3DLOCK_READONLY );

            // Copy the bits from the file to the texture
            SetFilePointer( hFile, IMAGE_HDR_SIZE, NULL, FILE_BEGIN );
            bSuccess = ReadFile( hFile, lr.pBits, IMAGE_DATA_SIZE, 
                                 &dwBytesRead, NULL );
            bSuccess &= ( dwBytesRead == IMAGE_DATA_SIZE );
            m_pImage->UnlockRect( 0 );

            if( bSuccess )
            {
                m_pImage->AddRef();
                *ppImage = m_pImage;
            }
        }
    }
    else
    {
        m_bIsValidImage = FALSE;
    }

    CloseHandle( hFile );
    return( bSuccess );
}




//-----------------------------------------------------------------------------
// Name: SaveImage()
// Desc: Generates a meta data image file for the save game. Overwrites
//       any existing image. Automatically converts the incoming texture
//       to the proper meta data image format.
//-----------------------------------------------------------------------------
BOOL CXBSavedGame::SaveImage( const LPDIRECT3DTEXTURE8 pSrcTexture )
{
    if( IsEmpty() || pSrcTexture == NULL )
        return FALSE;

    // Determine size of incoming texture
    D3DSURFACE_DESC SrcDesc;
    HRESULT hr = pSrcTexture->GetLevelDesc( 0, &SrcDesc );
    if( FAILED(hr) )
        return FALSE;

    // Generate a 64x64 DXT1 surface
    LPDIRECT3DSURFACE8 pDxt1GameImageSurface = NULL;
    hr = g_pd3dDevice->CreateImageSurface( IMAGE_WH, IMAGE_WH, D3DFMT_DXT1,
                                           &pDxt1GameImageSurface );
    if( FAILED(hr) )
        return FALSE;

    // Copy the source into the DXT1 surface
    LPDIRECT3DSURFACE8 pSrcSurface = NULL;
    hr = pSrcTexture->GetSurfaceLevel( 0, &pSrcSurface );
    hr = D3DXLoadSurfaceFromSurface( pDxt1GameImageSurface, NULL, NULL, 
                                     pSrcSurface, NULL, NULL,
                                     D3DX_DEFAULT, D3DCOLOR( 0 ) );
    SAFE_RELEASE( pSrcSurface );
    if( FAILED(hr) )
    {
        SAFE_RELEASE( pDxt1GameImageSurface );
        return FALSE;
    }

    // Generate the save game image name
    CHAR strMetaDataFile[ MAX_PATH ];
    lstrcpyA( strMetaDataFile, m_strDir );
    lstrcatA( strMetaDataFile, strGAME_IMAGE );

    // Write the compressed texture out to an .XPR file
    hr = XGWriteSurfaceOrTextureToXPR( pDxt1GameImageSurface, strMetaDataFile, TRUE );

    SAFE_RELEASE( pDxt1GameImageSurface );

    if( FAILED(hr) )
        return FALSE;

    m_bIsValidImage = TRUE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Clear saved game data
//-----------------------------------------------------------------------------
VOID CXBSavedGame::Init()
{
    *m_strName = 0;
    *m_strDir = 0;
    m_qwLastWriteTime = ULONGLONG(0);
    m_dwSize = DWORD(-1);
    SAFE_RELEASE( m_pImage );
    m_bIsValidImage = TRUE;
}
