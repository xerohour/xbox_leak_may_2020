//-----------------------------------------------------------------------------
// File: XbOnline.cpp
//
// Desc: Shortcut macros and helper functions for the Xbox online samples.
//       Requires linking with XONLINE[D][S].LIB.
//
// Hist: 10.11.01 - New for November XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "XBOnline.h"
#include "XBMemUnit.h"
#include "XBApp.h"
#include <cassert>
#include <algorithm>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_MEMORY_UNITS  = 2 * XGetPortCount();

// How often (per second) the caret blinks during PIN entry
const FLOAT fCARET_BLINK_RATE = 1.0f;

// During the blink period, the amount the caret is visible. 0.5 equals
// half the time, 0.75 equals 3/4ths of the time, etc.
const FLOAT fCARET_ON_RATIO = 0.75f;




//-----------------------------------------------------------------------------
// Name: XBOnline_GetUserList()
// Desc: Extract all accounts from hard disk and MUs
//-----------------------------------------------------------------------------
HRESULT XBOnline_GetUserList( XBUserList& UserList )
{
    // On input, the list must have room for XONLINE_MAX_STORED_ONLINE_USERS
    // accounts
    XONLINE_USER pUserList[ XONLINE_MAX_STORED_ONLINE_USERS ];

    // Get accounts stored on the hard disk
    DWORD dwUsers;
    HRESULT hr = XOnlineGetUsers( pUserList, &dwUsers );
    if( SUCCEEDED(hr) )
    {
        for( DWORD i = 0; i < dwUsers; ++i )
            UserList.push_back( pUserList[i] );
    }

    return hr;
}





//-----------------------------------------------------------------------------
// Name: CXBPinEntry()
// Desc: Create PIN entry object
//-----------------------------------------------------------------------------
CXBPinEntry::CXBPinEntry()
{
    m_hPinInput = NULL;
    m_dwPinLength = 0;

    for( DWORD i = 0; i < XONLINE_PIN_LENGTH; ++i )
        m_byPin[i] = 0;
}




//-----------------------------------------------------------------------------
// Name: ~CXBPinEntry()
// Desc: Destry PIN entry object
//-----------------------------------------------------------------------------
CXBPinEntry::~CXBPinEntry()
{
    EndInput();
}




//-----------------------------------------------------------------------------
// Name: BeginInput()
// Desc: Start PIN entry input
//-----------------------------------------------------------------------------
HRESULT CXBPinEntry::BeginInput( const XINPUT_GAMEPAD& DefaultGamepad )
{
    EndInput();

    m_dwPinLength = 0;
    assert( m_hPinInput == NULL );

    // The samples application framework, although it saves the full
    // XINPUT_STATE struct for each gamepad, does not track the "current"
    // or "primary" gamepad, but rather merges all input together
    // in one gamepad struct called the default gamepad.

    // The PIN functions want the full XINPUT_STATE, so we create a local
    // version. Your game should use the actual XINPUT_STATE from whichever
    // gamepad is doing the PIN entry.
    XINPUT_STATE xis;
    CopyMemory( &xis.Gamepad, &DefaultGamepad, sizeof(DefaultGamepad) );

    m_hPinInput = XOnlinePINStartInput( &xis );
    if( m_hPinInput == NULL )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DecodeInput()
// Desc: Continue PIN entry input. If input is B or BACK, ends input and
//       returns E_ABORT;
//-----------------------------------------------------------------------------
HRESULT CXBPinEntry::DecodeInput( const XINPUT_GAMEPAD& DefaultGamepad )
{
    assert( m_hPinInput != NULL );

    // Ignore A and START
    if( DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_A ] )
        return S_OK;
    if( DefaultGamepad.wButtons & XINPUT_GAMEPAD_START )
        return S_OK;

    // "Cancel" buttons
    if( ( DefaultGamepad.wButtons & XINPUT_GAMEPAD_BACK ) ||
        ( DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_B ] ) )
    {
        EndInput();
        return E_ABORT;
    }

    // The samples application framework, although it saves the full
    // XINPUT_STATE struct for each gamepad, does not track the "current"
    // or "primary" gamepad, but rather merges all input together
    // in one gamepad struct called the default gamepad.

    // The PIN functions want the full XINPUT_STATE, so we create a local
    // version. Your game should use the actual XINPUT_STATE from whichever
    // gamepad is doing the PIN entry.
    XINPUT_STATE xis;
    CopyMemory( &xis.Gamepad, &DefaultGamepad, sizeof(DefaultGamepad) );

    // For all other controller input, decode PIN input
    BYTE byPin;
    HRESULT hr = XOnlinePINDecodeInput( m_hPinInput, &xis, &byPin );
    if( hr == S_OK )
    {
        m_byPin[ m_dwPinLength ] = byPin;
        ++m_dwPinLength;
    }

    // If no code byte entered this cycle, we're done
    if( hr == S_FALSE )
        return S_OK;

    // Error
    return hr;
}




//-----------------------------------------------------------------------------
// Name: GetPinLength()
// Desc: Returns the number of button presses that have been registered
//-----------------------------------------------------------------------------
DWORD CXBPinEntry::GetPinLength() const
{
    return m_dwPinLength;
}




//-----------------------------------------------------------------------------
// Name: IsPinComplete()
// Desc: TRUE if the entered PIN has enough digits
//-----------------------------------------------------------------------------
BOOL CXBPinEntry::IsPinComplete() const
{
    return( m_dwPinLength == XONLINE_PIN_LENGTH );
}




//-----------------------------------------------------------------------------
// Name: IsPinValid()
// Desc: TRUE if the PIN is valid. Must only be called on complete PINs.
//-----------------------------------------------------------------------------
BOOL CXBPinEntry::IsPinValid( const BYTE* pinCompare ) const
{
    assert( IsPinComplete() );
    assert( pinCompare != NULL );
    return( memcmp( m_byPin, pinCompare, XONLINE_PIN_LENGTH ) == 0 );
}




//-----------------------------------------------------------------------------
// Name: EndInput()
// Desc: Clean up PIN entry
//-----------------------------------------------------------------------------
HRESULT CXBPinEntry::EndInput()
{
    if( m_hPinInput != NULL )
    {
        HRESULT hr = XOnlinePINEndInput( m_hPinInput );
        assert( SUCCEEDED(hr) );
        m_hPinInput = NULL;
        m_dwPinLength = 0;
        return hr;
    }
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CXBOnlineUI()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBOnlineUI::CXBOnlineUI( WCHAR* strFrameRate, WCHAR* strHeader )
:
    m_strFrameRate( strFrameRate ),
    m_strHeader   ( strHeader ),
    m_CaretTimer  ( TRUE )
{
    m_ptMenuSel = NULL;
    *m_strError = 0;
}




//-----------------------------------------------------------------------------
// Name: SetErrorStr()
// Desc: Set error string
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::SetErrorStr( const WCHAR* strFormat, va_list pArglist )
{
    INT iChars = wvsprintfW( m_strError, strFormat, pArglist );
    assert( iChars < MAX_ERROR_STR );
    (VOID)iChars; // avoid compiler warning
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects
//-----------------------------------------------------------------------------
HRESULT CXBOnlineUI::Initialize( DWORD dwNumResources, DWORD dwMenuSelectorOffset )
{
    // Create a font
    if( FAILED( m_Font.Create( g_pd3dDevice, "Font.xpr" ) ) )
    {
        OUTPUT_DEBUG_STRING( "Failed to load fonts\n" );
        return XBAPPERR_MEDIANOTFOUND;
    }

    // Initialize the help system
    if( FAILED( m_Help.Create( g_pd3dDevice, "Gamepad.xpr" ) ) )
    {
        OUTPUT_DEBUG_STRING( "Failed to load help\n" );
        return XBAPPERR_MEDIANOTFOUND;
    }

    // Load our textures
    if( FAILED( m_xprResource.Create( g_pd3dDevice, "Resource.xpr", dwNumResources ) ) )
    {
        OUTPUT_DEBUG_STRING( "Failed to load textures\n" );
        return XBAPPERR_MEDIANOTFOUND;
    }

    // Set up texture ptrs
    m_ptMenuSel = m_xprResource.GetTexture( dwMenuSelectorOffset );

    // Set projection transform
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 0.1f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set view position
    D3DXMATRIX matView;
    D3DXMatrixTranslation( &matView, 0.0f, 0.0f, 40.0f);
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderCreateAccount()
// Desc: Allow player to launch account creation tool
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderCreateAccount( BOOL bHasMachineAccount ) const
{
    RenderHeader();

    WCHAR* strInfo;
    if( bHasMachineAccount )
    {
        strInfo = L"No online accounts exist on this Xbox.\n\n"
                  L"Run the Xbox Online Setup Tool to create accounts.\n\n"
                  L"Press A to continue.";
    }
    else
    {
        strInfo = L"This Xbox does not have a machine account.\n\n"
                  L"Run the Xbox Online Setup Tool to create accounts.\n\n"
                  L"Press A to continue.";
    }

    m_Font.DrawText( 320, 140, COLOR_NORMAL, strInfo, XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectAccount()
// Desc: Display list of accounts
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderSelectAccount( DWORD dwCurrItem, 
                                       const XBUserList& UserList ) const
{
    assert( !UserList.empty() );

    RenderHeader();

    m_Font.DrawText( 320, 140, COLOR_NORMAL, L"Select an account",
                     XBFONT_CENTER_X );

    FLOAT fYtop = 220.0f;
    FLOAT fYdelta = 30.0f;

    // Show list of user accounts
    for( DWORD i = 0; i < UserList.size(); ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;

        // Convert user name to WCHAR string
        WCHAR strUserName[ XONLINE_NAME_SIZE ];
        XBUtil_GetWide( UserList[i].name, strUserName, XONLINE_NAME_SIZE );

        m_Font.DrawText( 160, fYtop + (fYdelta * i), dwColor, strUserName );
    }

    // Show selected item with little triangle
    RenderMenuSelector( 120.0f, fYtop + (fYdelta * dwCurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderGetPin()
// Desc: Display PIN entry screen
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderGetPin( const CXBPinEntry& PinEntry, 
                                  const WCHAR* strUser ) const
{
    RenderHeader();

    WCHAR strMsg[ 256 + XONLINE_USERNAME_SIZE ];
    wsprintfW( strMsg, L"Please enter the four-sequence pass code for\n"
                       L"'%.*s'\n"
                       L"using the directional pad, X, Y, black or\n"
                       L"white buttons, or left and right triggers.\n"
                       L"\n\n"
                       L"Press B to cancel",
               XONLINE_USERNAME_SIZE, strUser );
    m_Font.DrawText( 320, 120, COLOR_GREEN, strMsg, XBFONT_CENTER_X );

    // Build PIN text string (stars)
    WCHAR strPIN[ XONLINE_PIN_LENGTH * 2 + 1 ] = { 0 };
    for( DWORD i = 0; i < PinEntry.GetPinLength() * 2; i += 2 )
    {
        strPIN[ i   ] = L'*';
        strPIN[ i+1 ] = L' ';
    }

    // Determine caret location
    FLOAT fWidth;
    FLOAT fHeight;
    m_Font.GetTextExtent( strPIN, &fWidth, &fHeight );

    // Display text "cursor"
    if( fmod( m_CaretTimer.GetElapsedSeconds(), fCARET_BLINK_RATE ) <
        fCARET_ON_RATIO )
    {
        m_Font.DrawText( 300.0f + fWidth - 2.0f, 300.0f, COLOR_HIGHLIGHT, L"|" );
    }

    // Display "PIN"
    m_Font.DrawText( 300, 300, COLOR_NORMAL, strPIN );
}




//-----------------------------------------------------------------------------
// Name: RenderLoggingOn()
// Desc: Display "logging on" animation
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderLoggingOn() const
{
    RenderHeader();
    m_Font.DrawText( 320, 200, COLOR_NORMAL, L"Authenticating Xbox Account", 
                     XBFONT_CENTER_X );
    m_Font.DrawText( 320, 260, COLOR_NORMAL, L"Press B to cancel", 
                     XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderError()
// Desc: Display error (or any other) message
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderError() const
{
    RenderHeader();
    m_Font.DrawText( 320, 200, COLOR_NORMAL, m_strError, XBFONT_CENTER_X );
    m_Font.DrawText( 320, 300, COLOR_NORMAL, L"Press A to continue", 
                     XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderHeader()
// Desc: Display standard text
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderHeader() const
{
    m_Font.DrawText(  64, 50, COLOR_NORMAL, m_strHeader );
    m_Font.DrawText( 450, 50, COLOR_HIGHLIGHT, m_strFrameRate );
}




//-----------------------------------------------------------------------------
// Name: RenderMenuSelector()
// Desc: Display menu selector
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderMenuSelector( FLOAT fLeft, FLOAT fTop ) const
{
    D3DXVECTOR4 rc( fLeft, fTop, fLeft + 20.0f, fTop + 20.0f );

    // Show selected item
    struct TILEVERTEX
    {
        D3DXVECTOR4 p;
        D3DXVECTOR2 t;
    };
    TILEVERTEX* pVertices;

    LPDIRECT3DVERTEXBUFFER8 pvbTemp;
    g_pd3dDevice->CreateVertexBuffer( 4 * sizeof( TILEVERTEX ), 
                                      D3DUSAGE_WRITEONLY, 
                                      D3DFVF_XYZRHW | D3DFVF_TEX1, 
                                      D3DPOOL_MANAGED, &pvbTemp );

    // Create a quad for us to render our texture on
    pvbTemp->Lock( 0, 0, (BYTE **)&pVertices, 0L );
    pVertices[0].p = D3DXVECTOR4( rc.x - 0.5f, rc.w - 0.5f, 1.0f, 1.0f );  pVertices[0].t = D3DXVECTOR2( 0.0f, 1.0f ); // Bottom Left
    pVertices[1].p = D3DXVECTOR4( rc.x - 0.5f, rc.y - 0.5f, 1.0f, 1.0f );  pVertices[1].t = D3DXVECTOR2( 0.0f, 0.0f ); // Top    Left
    pVertices[2].p = D3DXVECTOR4( rc.z - 0.5f, rc.w - 0.5f, 1.0f, 1.0f );  pVertices[2].t = D3DXVECTOR2( 1.0f, 1.0f ); // Bottom Right
    pVertices[3].p = D3DXVECTOR4( rc.z - 0.5f, rc.y - 0.5f, 1.0f, 1.0f );  pVertices[3].t = D3DXVECTOR2( 1.0f, 0.0f ); // Top    Right
    pvbTemp->Unlock();

    // Set up our state
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetStreamSource( 0, pvbTemp, sizeof( TILEVERTEX ) );

    // Render the quad with our texture
    g_pd3dDevice->SetTexture( 0, m_ptMenuSel );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    g_pd3dDevice->SetTexture( 0, NULL );
    pvbTemp->Release();
}




//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Display text using UI font
//-----------------------------------------------------------------------------
HRESULT CXBOnlineUI::DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, 
                               const WCHAR* strText, DWORD dwFlags ) const
{
    return m_Font.DrawText( sx, sy, dwColor, strText, dwFlags );
}
