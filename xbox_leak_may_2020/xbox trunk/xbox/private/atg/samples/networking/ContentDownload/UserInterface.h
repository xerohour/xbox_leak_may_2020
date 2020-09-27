//-----------------------------------------------------------------------------
// File: UserInterface.h
//
// Desc: ContentDownload rendering functions
//
// Hist: 09.07.01 - Updated for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef CONTENTDOWNLOAD_UI_H
#define CONTENTDOWNLOAD_UI_H

#include "Common.h"
#include "XBOnline.h"




//-----------------------------------------------------------------------------
// Name: class UserInterface
// Desc: Main UI class for ContentDownload sample
//-----------------------------------------------------------------------------
class UserInterface
{
    CXBOnlineUI m_UI;

    static const D3DCOLOR COLOR_NORMAL    = CXBOnlineUI::COLOR_NORMAL;
    static const D3DCOLOR COLOR_HIGHLIGHT = CXBOnlineUI::COLOR_HIGHLIGHT;
    static const D3DCOLOR COLOR_GREEN     = CXBOnlineUI::COLOR_GREEN;

public:

    explicit UserInterface( WCHAR* );

    HRESULT Initialize();

    // Accessors
    VOID __cdecl SetErrorStr( const WCHAR*, ... );

    // UI functions
    VOID RenderCreateAccount( BOOL bHasMachineAccount ) const;
    VOID RenderSelectAccount( DWORD, const XBUserList& ) const;
    VOID RenderGetPin( const CXBPinEntry&, const WCHAR* ) const;
    VOID RenderSelectDevice( DWORD ) const;
    VOID RenderEnumContent() const;
    VOID RenderSelectContent( XONLINEOFFERING_ENUM_DEVICE, 
                              const ContentList&, DWORD, DWORD ) const;
    VOID RenderContentInfo( XONLINEOFFERING_ENUM_DEVICE, 
                            const ContentInfo& ) const;
    VOID RenderInstallContent( FLOAT, DWORD, DWORD ) const;
    VOID RenderVerifyContent() const;
    VOID RenderConfirmRemove( const ContentInfo&, DWORD ) const;
    VOID RenderRemoveContent() const;
    VOID RenderLoggingOn() const;
    VOID RenderCancel() const;
    VOID RenderError() const;

private:

    // Disabled
    UserInterface();
    UserInterface( const UserInterface& );

    VOID RenderProgress( const D3DXVECTOR4&, FLOAT ) const;

};

#endif // CONTENTDOWNLOAD_UI_H
