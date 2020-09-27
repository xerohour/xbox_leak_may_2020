/*++

Copyright (c) Microsoft Corporation

Module Name:

    attrib.cpp

Abstract:

    Implementation of 


Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    06-11-2001 : created (mitchd)

--*/
#include "stdafx.h"

CGetAttributes::CGetAttributes(
    XBOX_MULTIFILE_ATTRIBUTES *pAttributes,
    bool fGetTypeName,
    GET_ATTRIBUTES_CB pfnCallback,
    PVOID pvCallback
    ) :
    m_pAttributes(pAttributes),
    m_fGetTypeName(fGetTypeName),
    m_pfnCallback(pfnCallback),
    m_pvCallback(pvCallback),
    m_fVariousFileTypes(false),
    m_fFirstItem(true)
{
   HRESULT hr;
   m_pAttributes->dwFileCount = 0;
   m_pAttributes->dwFolderCount = 0;
   m_pAttributes->dwValidAttributes = FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_HIDDEN;
   m_pAttributes->ullTotalSize = 0;
}

void
CGetAttributes::VisitFileOrDir(
    IXboxFileOrDirVisit *pFileOrDir, 
    DWORD *pdwFlags
)
{
    HRESULT hr;
    DM_FILE_ATTRIBUTES dmAttributes;
     
    //
    //  Even for a set operation, we need to get the existing
    //  attributes first.
    //

    pFileOrDir->GetFileAttributes(&dmAttributes);
    
    //
    //  Count files and folder.
    //  

    if(dmAttributes.Attributes&FILE_ATTRIBUTE_DIRECTORY)
    {
        m_pAttributes->dwFolderCount++;  
    } else
    {
        m_pAttributes->dwFileCount++;
    }

    //
    //  If this is the first item just copy everything.
    //
    if(m_fFirstItem)
    {
        m_fFirstItem = false;
        m_pAttributes->CreationTime = dmAttributes.CreationTime;
        m_pAttributes->ChangeTime = dmAttributes.ChangeTime;
        m_pAttributes->dwAttributes = dmAttributes.Attributes;
        if(m_fGetTypeName)
        {
            if(dmAttributes.Attributes&FILE_ATTRIBUTE_DIRECTORY)
            {
                strcpy(
                    m_pAttributes->szTypeName, 
                    WindowUtils::GetPreloadedString(IDS_PRELOAD_FOLDER_TYPE_NAME)
                    );
            }else
            {
                SHFILEINFOA shellFileInfo;
                pFileOrDir->GetName(shellFileInfo.szDisplayName);
                shellFileInfo.dwAttributes = dmAttributes.Attributes;
                if(SHGetFileInfoA(
                    shellFileInfo.szDisplayName,
                    dmAttributes.Attributes,
                    &shellFileInfo,
                    sizeof(shellFileInfo),
                    SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME
                    )
                )
                {
                    strcpy(m_pAttributes->szTypeName, shellFileInfo.szTypeName);                
                }
            }
        }

    } else
    {   
        
        //
        //  Clear the correct valid bits and OR the bits.
        //

        m_pAttributes->dwValidAttributes &= ~(m_pAttributes->dwAttributes^dmAttributes.Attributes);
        m_pAttributes->dwAttributes |= dmAttributes.Attributes;

        //
        //  Handle the type name.
        //

        if(m_fGetTypeName && !m_fVariousFileTypes)
        {
            if(dmAttributes.Attributes&FILE_ATTRIBUTE_DIRECTORY)
            {
                if(m_pAttributes->dwFileCount)
                {
                    m_fVariousFileTypes = true;
                    LoadStringA(
                        _Module.GetModuleInstance(),
                        IDS_FILETYPE_VARIOUS,
                        m_pAttributes->szTypeName,
                        MAX_PATH
                        );
                }
            }else
            {
                if(m_pAttributes->dwFolderCount)
                {
                    m_fVariousFileTypes = true;
                    LoadStringA(
                        _Module.GetModuleInstance(),
                        IDS_FILETYPE_VARIOUS,
                        m_pAttributes->szTypeName,
                        MAX_PATH
                        );
                } else
                {
                    SHFILEINFOA shellFileInfo;
                    pFileOrDir->GetName(shellFileInfo.szDisplayName);
                    shellFileInfo.dwAttributes = dmAttributes.Attributes;
                    if(SHGetFileInfoA(
                        shellFileInfo.szDisplayName,
                        dmAttributes.Attributes,
                        &shellFileInfo,
                        sizeof(shellFileInfo),
                        SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME
                        )
                    )
                    {
                        if(strcmp(m_pAttributes->szTypeName, shellFileInfo.szTypeName))
                        {
                            m_fVariousFileTypes = true;
                            LoadStringA(
                                _Module.GetModuleInstance(),
                                IDS_FILETYPE_VARIOUS,
                                m_pAttributes->szTypeName,
                                MAX_PATH
                                );
                        }
                    }

                }
            }
        }
    }
    
    //
    //  Sum in the size.
    //
    ULARGE_INTEGER uliSize;
    uliSize.HighPart = dmAttributes.SizeHigh;
    uliSize.LowPart = dmAttributes.SizeLow;
    m_pAttributes->ullTotalSize +=  uliSize.QuadPart;

    //
    //  Report Progress
    //

    if(m_pfnCallback)
    {
       if(!m_pfnCallback(m_pvCallback, m_pAttributes))
       {
           //
           //   Stop visiting if callback tells us to.
           //
           *pdwFlags = 0;
       }
    }
}

void CSetAttributes::VisitFileOrDir(IXboxFileOrDirVisit *pFileOrDir, DWORD *)
{
    DM_FILE_ATTRIBUTES dmFileAttributes;
    DWORD              dwOldAttributes;

    LPITEMIDLIST pidl = pFileOrDir->GetPidl(CPidlUtils::PidlTypeAbsolute);

    pFileOrDir->GetFileAttributes(&dmFileAttributes);

    dwOldAttributes = dmFileAttributes.Attributes;
    dmFileAttributes.Attributes &= ~m_dwClearAttributes;
    dmFileAttributes.Attributes |= m_dwSetAttributes;
    
    if(dmFileAttributes.Attributes != dwOldAttributes)
    {
        if(0==dmFileAttributes.Attributes)
        {
            dmFileAttributes.Attributes = FILE_ATTRIBUTE_NORMAL;
        }
        HRESULT hr=pFileOrDir->SetFileAttributes(&dmFileAttributes);
        if(SUCCEEDED(hr))
        {
            SHChangeNotify(SHCNE_ATTRIBUTES, SHCNF_FLUSH|SHCNF_IDLIST, pidl, 0);
            CPidlUtils::Free(pidl);
        }
    }
}