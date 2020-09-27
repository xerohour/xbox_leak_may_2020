//-----------------------------------------------------------------------------
// File: UserInterface.cpp
//
// Desc: ContentDownload rendering functions
//
// Hist: 09.04.01 - Updated for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "UserInterface.h"
#include "Resource.h"
#include "XBApp.h"
#include "XBConfig.h"
#include "XBStorageDevice.h"
#include <cassert>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_CONTENT_DISPLAYED = 5;    // Number to show on screen

const D3DCOLOR COLOR_PROGRESS  = 0xff00ff00; // Green
const D3DCOLOR COLOR_BARBORDER = 0xff000000; // Black

const DWORD FVF_BARVERTEX = D3DFVF_XYZRHW;




//-----------------------------------------------------------------------------
// Name: UserInterface()
// Desc: Constructor
//-----------------------------------------------------------------------------
UserInterface::UserInterface( WCHAR* strFrameRate )
:
m_UI( strFrameRate, L"ContentDownload" )
{
}




//-----------------------------------------------------------------------------
// Name: SetErrorStr()
// Desc: Set error string
//-----------------------------------------------------------------------------
VOID __cdecl UserInterface::SetErrorStr( const WCHAR* strFormat, ... )
{
    va_list pArgList;
    va_start( pArgList, strFormat );
    
    m_UI.SetErrorStr( strFormat, pArgList );
    
    va_end( pArgList );
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects
//-----------------------------------------------------------------------------
HRESULT UserInterface::Initialize()
{
    return m_UI.Initialize( Resource_NUM_RESOURCES, Resource_MenuSelect_OFFSET );
}




//-----------------------------------------------------------------------------
// Name: RenderCreateAccount()
// Desc: Allow player to launch account creation tool
//-----------------------------------------------------------------------------
VOID UserInterface::RenderCreateAccount( BOOL bHasMachineAccount ) const
{
    m_UI.RenderCreateAccount( bHasMachineAccount );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectAccount()
// Desc: Display list of accounts
//-----------------------------------------------------------------------------
VOID UserInterface::RenderSelectAccount( DWORD dwCurrItem, 
                                        const XBUserList& UserList ) const
{
    m_UI.RenderSelectAccount( dwCurrItem, UserList );
}




//-----------------------------------------------------------------------------
// Name: RenderGetPin()
// Desc: Display PIN entry screen
//-----------------------------------------------------------------------------
VOID UserInterface::RenderGetPin( const CXBPinEntry& PinEntry, 
                                 const WCHAR* strUser ) const
{
    m_UI.RenderGetPin( PinEntry, strUser );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectDevice()
// Desc: Display available content enumeration devices
//-----------------------------------------------------------------------------
VOID UserInterface::RenderSelectDevice( DWORD dwCurrItem ) const
{
    m_UI.RenderHeader();
    
    m_UI.DrawText( 320, 140, COLOR_NORMAL, L"Select Content Device",
        XBFONT_CENTER_X );
    
    const WCHAR* const strDevice[] =
    {
        L"DVD",
            L"Hard Disk",
            L"Online Game Content Server"
    };
    
    const FLOAT fYtop = 200.0f;
    const FLOAT fYdelta = 50.0f;
    
    for( DWORD i = 0; i < XONLINEOFFERING_ENUM_DEVICE_MAX; ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;
        m_UI.DrawText( 260, fYtop + (fYdelta * i), dwColor, strDevice[i] );
    }
    
    // Show selected item with little triangle
    m_UI.RenderMenuSelector( 220.0f, fYtop + (fYdelta * dwCurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderEnumContent()
// Desc: Content enumeration progress
//-----------------------------------------------------------------------------
VOID UserInterface::RenderEnumContent() const
{
    m_UI.RenderHeader();
    
    m_UI.DrawText( 320, 140, COLOR_NORMAL, L"Enumerating content", 
        XBFONT_CENTER_X );
    m_UI.DrawText( 320, 200, COLOR_NORMAL, L"Press B to cancel", 
        XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectContent()
// Desc: Display content list
//-----------------------------------------------------------------------------
VOID UserInterface::RenderSelectContent( XONLINEOFFERING_ENUM_DEVICE contentDevice, 
                                        const ContentList& contentList,
                                        DWORD dwCurrItem, DWORD dwTopItem ) const
{
    m_UI.RenderHeader();
    
    if( contentList.empty() )
    {
        m_UI.DrawText( 320, 140, COLOR_NORMAL, L"No content available",
            XBFONT_CENTER_X );
        m_UI.DrawText( 320, 180, COLOR_NORMAL, L"Press A to continue", 
            XBFONT_CENTER_X );
        return;
    }
    
    m_UI.DrawText( 320, 100, COLOR_NORMAL, L"Content List",
        XBFONT_CENTER_X );
    
    m_UI.DrawText( 100, 140, COLOR_NORMAL, L"Press A for detailed info" );
    
    switch( contentDevice )
    {
    case XONLINEOFFERING_ENUM_DEVICE_ONLINE:
    case XONLINEOFFERING_ENUM_DEVICE_DVD:
        m_UI.DrawText( 360, 140, COLOR_NORMAL, L"Press Y to install" );
        break;
    case XONLINEOFFERING_ENUM_DEVICE_HD:
        m_UI.DrawText( 360, 140, COLOR_NORMAL, L"Press X to remove" );
        break;
    default:
        assert( FALSE );
        break;
    }
    
    const FLOAT fYtop = 200.0f;
    const FLOAT fYdelta = 40.0f;
    
    DWORD j = 0;
    for( DWORD i = dwTopItem; i < contentList.size() &&
        j < MAX_CONTENT_DISPLAYED; ++i, ++j )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;
        if( dwCurrItem == i )
        {
            // Show selected item with little triangle
            m_UI.RenderMenuSelector( 160.0f, fYtop + (fYdelta * j ) );
        }
        
        WCHAR strOfferingId[ 64 ];
        wsprintfW( strOfferingId, L"ID: %08X", contentList[i].GetId() );
        
        m_UI.DrawText( 200, fYtop + (fYdelta * j), dwColor, strOfferingId );
    }
    
    // Show ultra-sexy scroll arrows
    BOOL bShowTopArrow = dwTopItem > 0;
    BOOL bShowBtmArrow = dwTopItem + MAX_CONTENT_DISPLAYED < contentList.size();
    if( bShowTopArrow )
        m_UI.DrawText( 170, 170, COLOR_GREEN, L"/\\" );
    if( bShowBtmArrow )
        m_UI.DrawText( 170, 390, COLOR_GREEN, L"\\/" );
}




//-----------------------------------------------------------------------------
// Name: RenderContentInfo()
// Desc: Display detailed information about selected content
//-----------------------------------------------------------------------------
VOID UserInterface::RenderContentInfo( XONLINEOFFERING_ENUM_DEVICE contentDevice, 
                                      const ContentInfo& content ) const
{
    m_UI.RenderHeader();
    
    // Extract content data
    WCHAR strOfferingType[ 64 ] = L"";
    WCHAR strBitFlags[ 12 ] = L"";
    WCHAR strRating[ 12 ] = L"";
    
    if( contentDevice == XONLINEOFFERING_ENUM_DEVICE_HD )
    {
        lstrcpynW( strOfferingType, L"Metadata not stored in this release", 64 );
    }
    else
    {
        wsprintfW( strOfferingType, L"0x%08X", content.GetOfferingType() );
        wsprintfW( strBitFlags, L"0x%08X", content.GetBitFlags() );
        wsprintfW( strRating, L"%u", content.GetRating() );
    }
    
    // Format date and time information
    WCHAR strDate[32];
    WCHAR strTime[32];
    FILETIME ftCreationTime = content.GetCreationDate();
    CXBConfig::FormatDateTime( ftCreationTime, strDate, strTime );
    
    // Convert bytes to blocks
    const DWORD dwBlockSize = CXBStorageDevice::GetBlockSize();
    DWORD dwPackageBytes = content.GetPackageSize();
	DWORD dwInstallBlocks = content.GetInstallSize( );
    DWORD dwPackageBlocks = ( dwPackageBytes + (dwBlockSize-1) ) / dwBlockSize;
    
    // Format blocks
    CHAR strPackageBlocks[32];
    CHAR strInstallBlocks[32];
    CXBConfig::FormatInt( dwPackageBlocks, strPackageBlocks );
    CXBConfig::FormatInt( dwInstallBlocks, strInstallBlocks );
    WCHAR strPackageBlocksW[32];
    WCHAR strInstallBlocksW[32];
    XBUtil_GetWide( strPackageBlocks, strPackageBlocksW, 32 );
    XBUtil_GetWide( strInstallBlocks, strInstallBlocksW, 32 );
    lstrcatW( strPackageBlocksW, L" blocks" );
    lstrcatW( strInstallBlocksW, L" blocks" );
    
    // Column 1
    m_UI.DrawText( 100, 100, COLOR_NORMAL, L"Offering Type" );
    m_UI.DrawText( 100, 130, COLOR_NORMAL, L"Bit Flags" );
    m_UI.DrawText( 100, 160, COLOR_NORMAL, L"Creation Date" );
    m_UI.DrawText( 100, 190, COLOR_NORMAL, L"Rating" );
    m_UI.DrawText( 100, 220, COLOR_NORMAL, L"Package Size" );
    m_UI.DrawText( 100, 250, COLOR_NORMAL, L"Install Size" );
    
    // Column 2
    m_UI.DrawText( 240, 100, COLOR_GREEN, strOfferingType );
    m_UI.DrawText( 240, 130, COLOR_GREEN, strBitFlags );
    m_UI.DrawText( 240, 160, COLOR_GREEN, strDate );
    m_UI.DrawText( 240, 190, COLOR_GREEN, strRating );
    m_UI.DrawText( 240, 220, COLOR_GREEN, strPackageBlocksW );
    m_UI.DrawText( 240, 250, COLOR_GREEN, strInstallBlocksW );
    
    switch( contentDevice )
    {
    case XONLINEOFFERING_ENUM_DEVICE_ONLINE:
    case XONLINEOFFERING_ENUM_DEVICE_DVD:
        m_UI.DrawText( 320, 360, COLOR_NORMAL, L"Press Y to install",
            XBFONT_CENTER_X );
        break;
    case XONLINEOFFERING_ENUM_DEVICE_HD:
        m_UI.DrawText( 320, 360, COLOR_NORMAL, L"Press X to remove",
            XBFONT_CENTER_X );
        break;
    default:
        assert( FALSE );
        break;
    }
    
    m_UI.DrawText( 320, 400, COLOR_NORMAL, L"Press A or B to return to list",
        XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderInstallContent()
// Desc: Display download/installation progress
//-----------------------------------------------------------------------------
VOID UserInterface::RenderInstallContent( FLOAT fPercentComplete, 
                                         DWORD dwBlocksInstalled, 
                                         DWORD dwBlocksTotal ) const
{
    m_UI.RenderHeader();
    
    m_UI.DrawText( 320, 140, COLOR_NORMAL, L"Installing content", 
        XBFONT_CENTER_X );
    m_UI.DrawText( 320, 200, COLOR_NORMAL, L"Press B to cancel", 
        XBFONT_CENTER_X );
    
    // Display visual progress indicator
    D3DXVECTOR4 v4Progress = D3DXVECTOR4( 100, 260, 540, 280 );
    RenderProgress( v4Progress, fPercentComplete );
    
    // Format blocks
    CHAR strBlocksInstalled[32];
    CHAR strBlocksTotal[32];
    CXBConfig::FormatInt( dwBlocksInstalled, strBlocksInstalled );
    CXBConfig::FormatInt( dwBlocksTotal, strBlocksTotal );
    
    // Display block progress indicator
    WCHAR strProgress[ 128 ];
    wsprintfW( strProgress, L"%.*hs of %.*hs blocks", 32, strBlocksInstalled,
        32, strBlocksTotal );
    m_UI.DrawText( 320, 320, COLOR_NORMAL, strProgress, XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderVerifyContent()
// Desc: Display verification progress
//-----------------------------------------------------------------------------
VOID UserInterface::RenderVerifyContent() const
{
    m_UI.RenderHeader();
    
    m_UI.DrawText( 320, 140, COLOR_NORMAL, L"Verifying content", 
        XBFONT_CENTER_X );
    m_UI.DrawText( 320, 200, COLOR_NORMAL, L"Press B to cancel", 
        XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderConfirmRemove()
// Desc: Display removal confirmation
//-----------------------------------------------------------------------------
VOID UserInterface::RenderConfirmRemove( const ContentInfo& content,
                                        DWORD dwCurrItem ) const
{
    m_UI.RenderHeader();
    
    const DWORD NAME_MAX = 128;
    WCHAR strConfirm[ NAME_MAX + 128 ];
    
    /* TODO when content name available
    wsprintfW( strConfirm, L"Are you sure you want to remove\n"
    L"'%.*s' from the hard disk?", NAME_MAX, content.GetName() );
    */
    lstrcpynW( strConfirm, L"Are you sure you want to remove\n"
        L"content from the hard disk?", 128);
    
    m_UI.DrawText( 320, 140, COLOR_NORMAL, strConfirm, XBFONT_CENTER_X );
    
    const WCHAR* const strMenu[] =
    {
        L"Yes",
            L"No",
    };
    
    const FLOAT fYtop = 240.0f;
    const FLOAT fYdelta = 50.0f;
    
    for( DWORD i = 0; i < CONFIRM_MAX; ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : 
    COLOR_NORMAL;
    m_UI.DrawText( 280, fYtop + (fYdelta * i), dwColor, strMenu[i] );
    }
    
    // Show selected item with little triangle
    m_UI.RenderMenuSelector( 240.0f, fYtop + (fYdelta * dwCurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderRemoveContent()
// Desc: Display removal progress
//-----------------------------------------------------------------------------
VOID UserInterface::RenderRemoveContent() const
{
    m_UI.RenderHeader();
    
    m_UI.DrawText( 320, 140, COLOR_NORMAL, L"Removing content", 
        XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderLogginOn()
// Desc: Display login message
//-----------------------------------------------------------------------------
VOID UserInterface::RenderLoggingOn() const
{
    m_UI.RenderLoggingOn();
}




//-----------------------------------------------------------------------------
// Name: RenderError()
// Desc: Display error message
//-----------------------------------------------------------------------------
VOID UserInterface::RenderError() const
{
    m_UI.RenderError();
}




//-----------------------------------------------------------------------------
// Name: RenderProgress
// Desc: Renders a progress bar at the given rect, fPercent full
//-----------------------------------------------------------------------------
VOID UserInterface::RenderProgress( const D3DXVECTOR4& vecBar,
                                   FLOAT fPercentComplete ) const
{
    struct BARVERTEX
    { 
        D3DXVECTOR4 p;
    };
    
    LPDIRECT3DVERTEXBUFFER8 pvbTemp;
    g_pd3dDevice->CreateVertexBuffer( 9 * sizeof( BARVERTEX ), D3DUSAGE_WRITEONLY,
        FVF_BARVERTEX, D3DPOOL_MANAGED, &pvbTemp );
    
    // Create vertices for the filled-in bar and for the outer border
    BARVERTEX* pVertices;
    pvbTemp->Lock( 0, 0, (BYTE **)&pVertices, 0L );
    
    // Quad for filled-in section
    pVertices[0].p = D3DXVECTOR4( vecBar.x - 0.5f, vecBar.w - 0.5f, 1.0f, 1.0f );
    pVertices[1].p = D3DXVECTOR4( vecBar.x - 0.5f, vecBar.y - 0.5f, 1.0f, 1.0f );
    pVertices[2].p = D3DXVECTOR4( vecBar.x + fPercentComplete*(vecBar.z - vecBar.x) - 0.5f,
        vecBar.w - 0.5f, 1.0f, 1.0f );
    pVertices[3].p = D3DXVECTOR4( vecBar.x + fPercentComplete*(vecBar.z - vecBar.x) - 0.5f,
        vecBar.y - 0.5f, 1.0f, 1.0f );
    
    // Line-strip rectangle for border
    pVertices[4].p = D3DXVECTOR4( vecBar.x, vecBar.w, 1.0f, 1.0f );
    pVertices[5].p = D3DXVECTOR4( vecBar.x, vecBar.y, 1.0f, 1.0f );
    pVertices[6].p = D3DXVECTOR4( vecBar.z, vecBar.y, 1.0f, 1.0f );
    pVertices[7].p = D3DXVECTOR4( vecBar.z, vecBar.w, 1.0f, 1.0f );
    pVertices[8].p = D3DXVECTOR4( vecBar.x, vecBar.w, 1.0f, 1.0f );
    pvbTemp->Unlock();
    
    g_pd3dDevice->SetVertexShader( FVF_BARVERTEX );
    g_pd3dDevice->SetStreamSource( 0, pvbTemp, sizeof( BARVERTEX ) );
    g_pd3dDevice->SetTexture( 0, NULL );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    
    // First render the filled-in-section
    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, COLOR_PROGRESS );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    
    // Then render the linestrip border
    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, COLOR_BARBORDER );
    g_pd3dDevice->DrawPrimitive( D3DPT_LINESTRIP, 4, 4 );
    
    pvbTemp->Release();
}
