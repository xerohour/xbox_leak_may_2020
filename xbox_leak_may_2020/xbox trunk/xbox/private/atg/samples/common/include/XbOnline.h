//-----------------------------------------------------------------------------
// File: XbOnline.h
//
// Desc: Shortcut macros and helper functions for the Xbox online samples.
//       Requires linking with XONLINE[D][S].LIB
//
// Hist: 10.11.01 - New for November XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBONLINE_H
#define XBONLINE_H

#pragma warning( disable: 4786 ) // ignore STL identifier truncation
#include <xtl.h>
#include <vector>
#include "xonline.h"
#include "XBResource.h"
#include "XBFont.h"
#include "XBHelp.h"
#include "XBStopWatch.h"




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_ERROR_STR = 1024;




//-----------------------------------------------------------------------------
// Name: XBOnline_GetUserList() / XBOnline_UpdateUserList
// Desc: Extract accounts from hard disk and MUs
//-----------------------------------------------------------------------------
typedef std::vector< XONLINE_USER > XBUserList;
HRESULT XBOnline_GetUserList( XBUserList& UserList );




//-----------------------------------------------------------------------------
// Name: class CXBPinEntry
// Desc: Xbox PIN entry
//-----------------------------------------------------------------------------
class CXBPinEntry
{

    XPININPUTHANDLE m_hPinInput;                    // PIN input handle
    BYTE            m_byPin[ XONLINE_PIN_LENGTH ];  // Current PIN
    DWORD           m_dwPinLength;                  // number of buttons entered

public:

    CXBPinEntry();
    ~CXBPinEntry();

    HRESULT BeginInput( const XINPUT_GAMEPAD& );
    HRESULT DecodeInput( const XINPUT_GAMEPAD& );
    DWORD   GetPinLength() const;
    BOOL    IsPinComplete() const;
    BOOL    IsPinValid( const BYTE* ) const;
    HRESULT EndInput();

};




//-----------------------------------------------------------------------------
// Name: class CXBOnlineUI
// Desc: UI class for standard online UI
//-----------------------------------------------------------------------------
class CXBOnlineUI
{
    CXBPackedResource   m_xprResource;               // app resources
    mutable CXBFont     m_Font;                      // game font
    CXBHelp             m_Help;                      // help screen
    LPDIRECT3DTEXTURE8  m_ptMenuSel;                 // menu selection image
    WCHAR               m_strError[ MAX_ERROR_STR ]; // generic err
    WCHAR*              m_strFrameRate;              // from CXBApp
    WCHAR*              m_strHeader;                 // header string
    CXBStopWatch        m_CaretTimer;                // for PIN entry

public:

    static const D3DCOLOR COLOR_HIGHLIGHT = 0xffffff00; // Yellow
    static const D3DCOLOR COLOR_GREEN     = 0xff00ff00; // Green
    static const D3DCOLOR COLOR_NORMAL    = 0xffffffff; // White

public:

    explicit CXBOnlineUI( WCHAR* strFrameRate, WCHAR* strHeader );

    HRESULT Initialize( DWORD dwNumResources, DWORD dwMenuSelectorOffset );

    // Accessors
    VOID SetErrorStr( const WCHAR*, va_list );

    // UI functions
    VOID RenderCreateAccount( BOOL bHasMachineAccount ) const;
    VOID RenderSelectAccount( DWORD, const XBUserList& ) const;
    VOID RenderGetPin( const CXBPinEntry&, const WCHAR* strUser ) const;
    VOID RenderLoggingOn() const;
    VOID RenderError() const;

    VOID RenderHeader() const;
    VOID RenderMenuSelector( FLOAT, FLOAT ) const;

    HRESULT DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, 
                      const WCHAR* strText, DWORD dwFlags=0L ) const;

private:

    // Disabled
    CXBOnlineUI();
    CXBOnlineUI( const CXBOnlineUI& );

};

#endif // XBONLINE_H
