//-----------------------------------------------------------------------------
// File: UserInterface.h
//
// Desc: Auth rendering functions
//
// Hist: 10.12.01 - New for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef AUTH_UI_H
#define AUTH_UI_H

#include "Common.h"
#include "XBOnline.h"




//-----------------------------------------------------------------------------
// Name: class UserInterface
// Desc: Main UI class for Auth sample
//-----------------------------------------------------------------------------
class UserInterface
{
    CXBOnlineUI m_UI;

    static const D3DCOLOR COLOR_NORMAL = CXBOnlineUI::COLOR_NORMAL;

public:

    explicit UserInterface( WCHAR* );

    HRESULT Initialize();

    // Accessors
    VOID __cdecl SetErrorStr( const WCHAR*, ... );

    // UI functions
    VOID RenderCreateAccount( BOOL bHasMachineAccount ) const;
    VOID RenderSelectAccount( DWORD, const XBUserList& ) const;
    VOID RenderGetPin( const CXBPinEntry&, const WCHAR* ) const;
    VOID RenderLoggingOn() const;
    VOID RenderSuccess( const ServiceInfoList& ) const;
    VOID RenderError() const;

private:

    // Disabled
    UserInterface();
    UserInterface( const UserInterface& );

};

#endif // CONTENTDOWNLOAD_UI_H
