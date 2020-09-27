/*++

Copyright (c) Microsoft Corporation

Module Name:

    xbicon.cpp

Abstract:

    Implementation of CXboxExtractIcon

Environment:

    Windows 2000 and Later 
    User Mode
    ATL

Revision History:
    
    03-13-2001 : created (mitchd)

--*/

#include "stdafx.h"

void CXboxExtractIcon::VisitRoot(IXboxVisit *pRoot, DWORD *pdwFlags)
{    
    if(m_uCount)
    {
        if(CXboxExtractIcon::Root != m_eIconType)
        {
            m_hr = E_FAIL;
            *pdwFlags = 0;
        }
    }
    m_eIconType = CXboxExtractIcon::Root;
    m_uCount++;
}
void CXboxExtractIcon::VisitAddConsole(IXboxVisit *pAddConsole, DWORD *pdwFlags)
{
    if(m_uCount)
    {
        if(CXboxExtractIcon::AddConsole != m_eIconType)
        {
            m_hr = E_FAIL;
            *pdwFlags = 0;
        }
    }
    m_eIconType = CXboxExtractIcon::AddConsole;
    m_uCount++;
}

void CXboxExtractIcon::VisitConsole(IXboxConsoleVisit *pConsole, DWORD *pdwFlags)
{
    if(m_uCount)
    {
        if(CXboxExtractIcon::Console != m_eIconType)
        {
            m_hr = E_FAIL;
            *pdwFlags = 0;
        }
    }

    // Set the icon according to whether or not it is the default.
    m_eIconType = (pConsole->GetDefault()) ? CXboxExtractIcon::ConsoleDefault : CXboxExtractIcon::Console;
    m_uCount++;
}
void CXboxExtractIcon::VisitVolume(IXboxVolumeVisit *pVolume, DWORD *pdwFlags)
{
    if(m_uCount)
    {
        if(CXboxExtractIcon::Volume != m_eIconType)
        {
            m_hr = E_FAIL;
            *pdwFlags = 0;
        }
    }
    m_eIconType = CXboxExtractIcon::Volume;
    m_uCount++;
}

void CXboxExtractIcon::VisitFileOrDir(IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags)
{
    DM_FILE_ATTRIBUTES dmFileAttributes;
    pFileOrDir->GetFileAttributes(&dmFileAttributes);
    if(dmFileAttributes.Attributes&FILE_ATTRIBUTE_DIRECTORY)
    {
        if(m_uCount)
        {
            if(CXboxExtractIcon::Folder != m_eIconType)
            {
                m_hr = E_FAIL;
                *pdwFlags = 0;
            }
        }
        m_eIconType = CXboxExtractIcon::Folder;
    } else
    {
        
        if(CXboxFolder::IsXbeFile(dmFileAttributes.Name)) 
        {
            if(m_uCount)
            {
                if(CXboxExtractIcon::Xbe != m_eIconType)
                {
                    m_hr = E_FAIL;
                    *pdwFlags = 0;
                }
            }
            m_eIconType = CXboxExtractIcon::Xbe;
        } else
        {
            m_eIconType = CXboxExtractIcon::File;
            m_dwAttributes = dmFileAttributes.Attributes;
            strcpy(m_szFilename, dmFileAttributes.Name);
        }
    }
    m_uCount++;
}

HRESULT
CXboxExtractIcon::GetIconLocationImpl(
    UINT uFlags,
    LPSTR szIconFile,
    UINT cchMax,
    LPINT piIndex,
    UINT *pwFlags
    )
{
    DWORD dwResult = 0;
    //
    // Handle case of File Icon.  We defer this to the 
    // case to SHGetFileInfoA so that the icon is whatever
    // was registered for that file type.
    //
    if(CXboxExtractIcon::File==m_eIconType)
    { 
        strcpy(szIconFile, m_szFilename);
        SHFILEINFOA ShellFileInfo;
        if(SHGetFileInfoA(
                    m_szFilename,
                    m_dwAttributes,
                    &ShellFileInfo,
                    sizeof(ShellFileInfo),
                    SHGFI_USEFILEATTRIBUTES|SHGFI_SYSICONINDEX
                    ))
        {
            *piIndex = ShellFileInfo.iIcon;
            *pwFlags = GIL_NOTFILENAME;
            strcpy(szIconFile,"*");
            return S_OK;
        }
        return E_FAIL;
    } else
    {
        //
        //  All other icons are resources in this dll, so the path is to this DLL
        //
        if(GetModuleFileNameA(_Module.GetModuleInstance(), szIconFile, cchMax))
        {
            *pwFlags = 0;
            //
            // Assuming that we got the path, switch on the IconType to get the index
            //
            switch(m_eIconType)
            {
                case CXboxExtractIcon::AddConsole:
                    *piIndex = ICON_INDEX(IDI_ADD_CONSOLE);
                    break;
                case CXboxExtractIcon::Root:
                    *piIndex = ICON_INDEX(IDI_MAIN);
                    break;
                case CXboxExtractIcon::Console:
                    *piIndex = ICON_INDEX(IDI_CONSOLE);
                    break;
                case CXboxExtractIcon::ConsoleDefault:
                    *piIndex = ICON_INDEX(IDI_CONSOLE_DEFAULT);
                    break;
                case CXboxExtractIcon::Volume:
                    *piIndex = ICON_INDEX(IDI_VOLUME);
                    break;
                case CXboxExtractIcon::Folder:
                    *piIndex = ICON_INDEX(IDI_FOLDER);
                    break;
                case CXboxExtractIcon::Xbe:
                    *piIndex = ICON_INDEX(IDI_XBE);
                    break;
                default:
                    _ASSERT(FALSE);
            }
        } else
        {
            dwResult = GetLastError();
        }
    }
    return HRESULT_FROM_WIN32(dwResult);
}

HRESULT
CXboxExtractIcon::GetIconLocation(
    UINT uFlags,
    LPSTR szIconFile,
    UINT cchMax,
    LPINT piIndex,
    UINT *pwFlags
    )
{
    return GetIconLocationImpl(uFlags,szIconFile,cchMax,piIndex,pwFlags);
}
    
HRESULT
CXboxExtractIcon::GetIconLocation(
    UINT uFlags,
    LPWSTR szIconFile,
    UINT cchMax,
    LPINT piIndex,
    UINT *pwFlags
    )
{
    CHAR szIconFileBuffer[MAX_PATH];
    if(cchMax > MAX_PATH) cchMax = MAX_PATH;
    HRESULT hr = GetIconLocationImpl(uFlags,szIconFileBuffer,cchMax,piIndex,pwFlags);
    if(SUCCEEDED(hr))
    {
        wsprintfW(szIconFile, L"%hs", szIconFileBuffer);
    }
    return hr;
}

