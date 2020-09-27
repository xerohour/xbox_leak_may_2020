/*++

Copyright (c) Microsoft Corporation

Module Name:

    visit.cpp

Abstract:

  Implements the visitor model for CXboxFolder items.

  This uses a number of helper classes all defined and
  implemented here.  These are all mutual friends of the
  appropriate CXboxFolder class and should be considered
  part of their private implementation.

  In general, the Visit member of the various CXboxFolder
  instantiates one of these helpers on the stack, updates
  its internal context and then calls the appropriate
  member of IVisitor.

Environment:

    Windows 2000 and Later 
    User Mode
    Compiles UNICODE, but explictly most things are ANSI,
    since the Xbox filesystem is all ANSI.

Revision History:
    
    07-13-01 Created by Mitchell Dernis (mitchd)

--*/
#include <stdafx.h>

//-------------------------------------------------------------------
//  CFolderVisit - base class implements IXboxVisit for everone that
//                 is not 'FromSelf' or 'Recurse'
//-------------------------------------------------------------------
class CFolderVisit : public IXboxVisit
{
 public:
   virtual void         GetName(OUT LPSTR pszItemName)
                            {strcpy(pszItemName, m_pFolder->m_rgpszChildNames[m_uChildIndex]);}

   virtual LPITEMIDLIST GetPidl(CPidlUtils::PIDLTYPE PidlType)
                            {return m_pFolder->GetChildPidl(m_uChildIndex, PidlType);}

   virtual ULONG        GetShellAttributes()
                            {return m_pFolder->m_rgulChildShellAttributes[m_uChildIndex];}

   virtual void         GetConsoleName(OUT LPSTR pszConsoleName);
   
   //   Context 
   //
   CXboxFolder *m_pFolder;
   UINT         m_uChildIndex;
};

void CFolderVisit::GetConsoleName(OUT LPSTR pszConsoleName)
{
    LPSTR pszPathName;
    
    if(m_pFolder->m_uPathDepth)
    {
        pszPathName = m_pFolder->m_pszPathName;
    } else
    {
        pszPathName = m_pFolder->m_rgpszChildNames[this->m_uChildIndex];
    }
    while(*pszPathName && (*pszPathName!='\\'))
    {
        *pszConsoleName++ = *pszPathName++;
    }
    *pszConsoleName = '\0';
}

//-------------------------------------------------------------------
//  CFolderVisitFromSelf - base class implements IXboxVisit for everone that
//                         is 'FromSelf'
//-------------------------------------------------------------------
class CFolderVisitFromSelf : public IXboxVisit
{
 public:
   virtual void         GetName(OUT LPSTR pszItemName)
                            {m_pFolder->GetName(pszItemName);}

   virtual LPITEMIDLIST GetPidl(CPidlUtils::PIDLTYPE PidlType)
                            {return m_pFolder->GetPidl(PidlType);}

   virtual ULONG        GetShellAttributes()
                            {return m_pFolder->GetShellAttributes();}
   
   virtual void         GetConsoleName(OUT LPSTR pszConsoleName);
   //
   //   Context 
   //
   CXboxFolder *m_pFolder;
};

void CFolderVisitFromSelf::GetConsoleName(OUT LPSTR pszConsoleName)
{
    LPSTR pszPathName = m_pFolder->m_pszPathName;
    if(m_pFolder->m_uPathDepth)
    {
        while(*pszPathName && (*pszPathName!='\\'))
        {
            *pszConsoleName++ = *pszPathName++;
        }
    }
    *pszConsoleName = '\0';
}

//-------------------------------------------------------------------
//  MACRO to help everyone inheriting from CFolderVisit or CFolderVisitFromSelf
//-------------------------------------------------------------------
#define USE_BASE_VISITIMPL(_BaseType_)\
    virtual void GetName(OUT LPSTR pszItemName){_BaseType_::GetName(pszItemName);}\
    virtual LPITEMIDLIST GetPidl(CPidlUtils::PIDLTYPE PidlType){return _BaseType_::GetPidl(PidlType);}\
    virtual ULONG GetShellAttributes() {return _BaseType_::GetShellAttributes();}\
    virtual void GetConsoleName(OUT LPSTR pszConsoleName) {_BaseType_::GetConsoleName(pszConsoleName);}


//-------------------------------------------------------------------
// CConsoleVisit - implements IXboxConsoleVisit for CXboxRoot
//-------------------------------------------------------------------
class CConsoleVisit : public CFolderVisit, public IXboxConsoleVisit
{
  public:
   virtual DWORD        GetIPAddress()
                            {return m_pRoot->GetChildIpAddress(m_uChildIndex, TRUE);}
   virtual void         SetDefault(BOOL fMakeDefault);
   virtual BOOL         GetDefault();

   //
   //   Context 
   //
   CXboxRoot    *m_pRoot;
   USE_BASE_VISITIMPL(CFolderVisit)
   
};

void CConsoleVisit::SetDefault(BOOL fMakeDefault)
{
    CManageConsoles manageConsoles;
    manageConsoles.SetDefault(fMakeDefault ? m_pFolder->m_rgpszChildNames[m_uChildIndex] : NULL);
}
BOOL CConsoleVisit::GetDefault()
{
    CManageConsoles manageConsoles;
    return manageConsoles.IsDefault(m_pFolder->m_rgpszChildNames[m_uChildIndex]);
}

//-------------------------------------------------------------------
// CConsoleVisit - implements IXboxConsoleVisit for CXboxConsole
//-------------------------------------------------------------------
class CConsoleVisitFromSelf : public CFolderVisitFromSelf, public IXboxConsoleVisit
{
  public:
   virtual DWORD        GetIPAddress()
                            {return m_pConsole->m_dwIPAddress;}
   virtual void         SetDefault(BOOL fMakeDefault);
   virtual BOOL         GetDefault();

   //
   //   Context 
   //
   CXboxConsole        *m_pConsole;
    USE_BASE_VISITIMPL(CFolderVisitFromSelf)
};

void CConsoleVisitFromSelf::SetDefault(BOOL fMakeDefault)
{
    CManageConsoles manageConsoles;
    manageConsoles.SetDefault(fMakeDefault ? m_pFolder->m_pszPathName : NULL);
}
BOOL CConsoleVisitFromSelf::GetDefault()
{
    CManageConsoles manageConsoles;
    return manageConsoles.IsDefault(m_pFolder->m_pszPathName);
}

//-------------------------------------------------------------------
// CVolumeVisit - implements IXboxVolumeVisit for CXboxConsole
//-------------------------------------------------------------------
class CVolumeVisit :  public CFolderVisit, public IXboxVolumeVisit
{
  public:
   virtual int   GetVolumeType()
                    {return m_pConsole->m_rguChildVolumeType[m_uChildIndex];}
   virtual void  GetDiskCapacity(PULONGLONG pullTotalCapacity, PULONGLONG pullFreeCapacity)
                    {*pullTotalCapacity = m_pConsole->m_rgullChildTotalSpace[m_uChildIndex];
                     *pullFreeCapacity  = m_pConsole->m_rgullChildFreeSpace[m_uChildIndex];}
  
   //
   //   Context 
   //
   CXboxConsole *m_pConsole;
   USE_BASE_VISITIMPL(CFolderVisit)
};

//-------------------------------------------------------------------
// CVolumeVisitFromSelf - implements IXboxVolumeVisit for CXboxVolume
//-------------------------------------------------------------------
class CVolumeVisitFromSelf : public CFolderVisitFromSelf, public IXboxVolumeVisit
{
  public:
   virtual int          GetVolumeType()
                            {return m_pVolume->m_uVolumeType;}
   virtual void         GetDiskCapacity(PULONGLONG pullTotalCapacity, PULONGLONG pullFreeCapacity);
  
   //
   //   Context
   //
   CXboxVolume *m_pVolume;
   USE_BASE_VISITIMPL(CFolderVisitFromSelf)
};

void  CVolumeVisitFromSelf::GetDiskCapacity(PULONGLONG pullTotalCapacity, PULONGLONG pullFreeCapacity)
{
    char szWireName[MAX_PATH];
    CXboxFolder::GetWireName(
        szWireName,
        NULL,
        m_pVolume->m_pszPathName
        );
    ULARGE_INTEGER uliBogus;
    m_pVolume->m_pConnection->HrGetDiskFreeSpace(
                szWireName,
                (PULARGE_INTEGER)pullFreeCapacity,
                (PULARGE_INTEGER)pullTotalCapacity,
                &uliBogus
                );

}
//---------------------------------------------------------------------
// CFileOrDirVisit - implements IXboxFileOrDirVisit for a parent CXboxFileSystemFolder
//---------------------------------------------------------------------
class CFileOrDirVisit: public CFolderVisit, public IXboxFileOrDirVisit
{
  public:
   virtual void    GetFileAttributes(PDM_FILE_ATTRIBUTES pdmFileAttributes);
   virtual HRESULT SetFileAttributes(PDM_FILE_ATTRIBUTES pdmFileAttributes);
   virtual HRESULT SetName(HWND hWnd, LPSTR pszNewName);
   virtual HRESULT Delete();
   virtual void    GetWireName(LPSTR pszWireName);
   //
   //   Context
   //
   CXboxFileSystemFolder *m_pFileSystemFolder;
   USE_BASE_VISITIMPL(CFolderVisit)
};

void  CFileOrDirVisit::GetFileAttributes(PDM_FILE_ATTRIBUTES pDmFileAttributes)
{
    memcpy(
      pDmFileAttributes,
      &m_pFileSystemFolder->m_rgChildFileAttributes[m_uChildIndex],
      sizeof(DM_FILE_ATTRIBUTES)
      );
}

HRESULT CFileOrDirVisit::SetFileAttributes(PDM_FILE_ATTRIBUTES pdmFileAttributes)
{
    char szWireName[MAX_PATH];
    CXboxFolder::GetWireName(
        szWireName,
        m_pFileSystemFolder->m_pszPathName,
        m_pFileSystemFolder->m_rgpszChildNames[m_uChildIndex]
        );
    if(!pdmFileAttributes->Attributes) pdmFileAttributes->Attributes = FILE_ATTRIBUTE_NORMAL;
    HRESULT hr=m_pFileSystemFolder->m_pConnection->HrSetFileAttributes(szWireName, pdmFileAttributes);
    if(SUCCEEDED(hr))
    {
        m_pFileSystemFolder->m_rgChildFileAttributes[m_uChildIndex].Attributes = pdmFileAttributes->Attributes;
        if(pdmFileAttributes->Attributes&FILE_ATTRIBUTE_READONLY)
        {
            m_pFileSystemFolder->m_rgulChildShellAttributes[m_uChildIndex] |= SFGAO_READONLY;
        } else
        {
            m_pFileSystemFolder->m_rgulChildShellAttributes[m_uChildIndex] &= ~SFGAO_READONLY;
        }
        
        if(pdmFileAttributes->Attributes&FILE_ATTRIBUTE_HIDDEN)
        {
            m_pFileSystemFolder->m_rgulChildShellAttributes[m_uChildIndex] |= (SFGAO_HIDDEN|SFGAO_GHOSTED);
        } else
        {
            m_pFileSystemFolder->m_rgulChildShellAttributes[m_uChildIndex] &= ~(SFGAO_HIDDEN|SFGAO_GHOSTED);
        }
    }
    return hr;
}

HRESULT CFileOrDirVisit::SetName(HWND hWnd, LPSTR pszNewName)
{
    if(strlen(pszNewName) != _mbstrlen(pszNewName))
    {
        return XBDM_BADFILENAME;;
    }

    HRESULT hr = S_OK;
    char szWireNameOld[MAX_PATH];
    char szWireNameNew[MAX_PATH];
    LPSTR pszNewNameCopy  = new char[strlen(pszNewName)+1];
    if(pszNewNameCopy)
    {
        strcpy(pszNewNameCopy, pszNewName);
        CXboxFolder::GetWireName(
            szWireNameOld,
            m_pFileSystemFolder->m_pszPathName,
            m_pFileSystemFolder->m_rgpszChildNames[m_uChildIndex]
            );
    
        CXboxFolder::GetWireName(
            szWireNameNew,
            m_pFileSystemFolder->m_pszPathName,
            pszNewNameCopy
            );
    
        LPITEMIDLIST oldPidl = GetPidl(CPidlUtils::PidlTypeAbsolute);

        if(oldPidl)
        {
            hr = m_pFileSystemFolder->SetName(hWnd, szWireNameOld, szWireNameNew);
            if(SUCCEEDED(hr))
            {
                delete m_pFileSystemFolder->m_rgpszChildNames[m_uChildIndex];
                m_pFileSystemFolder->m_rgpszChildNames[m_uChildIndex] = pszNewNameCopy;
                LPITEMIDLIST newPidl = GetPidl(CPidlUtils::PidlTypeAbsolute);
                if(newPidl)
                {
                    DWORD dwEvent;
                    if(m_pFileSystemFolder->m_rgulChildShellAttributes[m_uChildIndex]&SFGAO_FOLDER)
                    {
                        dwEvent = SHCNE_RENAMEFOLDER;
                    } else
                    {
                        dwEvent = SHCNE_RENAMEITEM;
                    }
                    SHChangeNotify(dwEvent, SHCNF_FLUSH|SHCNF_IDLIST, oldPidl, newPidl);
                    CPidlUtils::Free(newPidl);
                }
            } else
            {
                delete [] pszNewNameCopy;
            }
            CPidlUtils::Free(oldPidl);
        }
    }
    return hr;
}

HRESULT CFileOrDirVisit::Delete()
{
    char szWireName[MAX_PATH];
    HRESULT hr;
    IXboxConnection *pConnection = m_pFileSystemFolder->m_pConnection;
    BOOL fIsFolder = (m_pFileSystemFolder->m_rgulChildShellAttributes[m_uChildIndex]&SFGAO_FOLDER) ? TRUE : FALSE;

    //
    //  It is probably pretty bad to do this, if this is not
    //  a selection clone.  Infact, the selection code, itself
    //  is toast after deleting everything.  One additionally hopes
    //  that any none clones are going to get refreshed.
    //
    _ASSERTE(m_pFileSystemFolder->m_fSelectionClone);

    CXboxFolder::GetWireName(
        szWireName,
        m_pFileSystemFolder->m_pszPathName,
        m_pFileSystemFolder->m_rgpszChildNames[m_uChildIndex]
        );

    hr = pConnection->HrDeleteFile(szWireName, fIsFolder);

    if(hr==XBDM_CANNOTACCESS)
    {
        HRESULT hrLoc;
        DM_FILE_ATTRIBUTES dmFileAttributes;
        DWORD dwOldAttributes;

        hrLoc = pConnection->HrGetFileAttributes(szWireName, &dmFileAttributes);
        if(SUCCEEDED(hrLoc))
        {
            dwOldAttributes = dmFileAttributes.Attributes;
            if(dwOldAttributes&FILE_ATTRIBUTE_READONLY)
            {
                dmFileAttributes.Attributes = FILE_ATTRIBUTE_NORMAL;
                hrLoc = pConnection->HrSetFileAttributes(szWireName, &dmFileAttributes);
                if(SUCCEEDED(hrLoc))
                {
                    hr = pConnection->HrDeleteFile(szWireName, fIsFolder);
                    if(FAILED(hr))
                    {
                        dmFileAttributes.Attributes = dwOldAttributes;
                        pConnection->HrSetFileAttributes(szWireName, &dmFileAttributes);
                    }
                }
            }
        }
    }
    return hr;
}

void CFileOrDirVisit::GetWireName(LPSTR pszWireName)
{
    CXboxFolder::GetWireName(
        pszWireName,
        m_pFileSystemFolder->m_pszPathName,
        m_pFileSystemFolder->m_rgpszChildNames[m_uChildIndex]
        );
}

//---------------------------------------------------------------------
// CFileOrDirVisit - implements IXboxFileOrDirVisit for CXboxDirectory
//---------------------------------------------------------------------
class CDirectoryVisitFromSelf: public CFolderVisitFromSelf, public IXboxFileOrDirVisit
{
  public:
   virtual void         GetFileAttributes(PDM_FILE_ATTRIBUTES pDmFileAttributes);
   virtual HRESULT      SetFileAttributes(PDM_FILE_ATTRIBUTES pdmFileAttributes);
   virtual HRESULT      SetName(HWND hWnd, LPSTR pszNewName);
   virtual HRESULT      Delete();
   virtual void         GetWireName(LPSTR pszWireName);
   //
   //   Context
   //
   CXboxFileSystemFolder *m_pFileSystemFolder;
   USE_BASE_VISITIMPL(CFolderVisitFromSelf)
};

void CDirectoryVisitFromSelf::GetFileAttributes(PDM_FILE_ATTRIBUTES pDmFileAttributes)
{
    char szWireName[MAX_PATH];
    m_pFileSystemFolder->GetWireName(szWireName, NULL, m_pFileSystemFolder->m_pszPathName);
    m_pFileSystemFolder->m_pConnection->HrGetFileAttributes(szWireName, pDmFileAttributes);
    strcpy(pDmFileAttributes->Name, m_pFileSystemFolder->m_pszName);
}
HRESULT CDirectoryVisitFromSelf::SetFileAttributes(PDM_FILE_ATTRIBUTES pdmFileAttributes)
{
    HRESULT hr;
    char szWireName[MAX_PATH];
    m_pFileSystemFolder->GetWireName(szWireName, NULL, m_pFileSystemFolder->m_pszPathName);
    if(!pdmFileAttributes->Attributes) pdmFileAttributes->Attributes = FILE_ATTRIBUTE_NORMAL;
    hr = m_pFileSystemFolder->m_pConnection->HrSetFileAttributes(szWireName, pdmFileAttributes);
    if(SUCCEEDED(hr))
    {
        if(pdmFileAttributes->Attributes&FILE_ATTRIBUTE_READONLY)
        {
            m_pFileSystemFolder->m_ulShellAttributes |= SFGAO_READONLY;
        } else
        {
            m_pFileSystemFolder->m_ulShellAttributes &= ~SFGAO_READONLY;
        }
        
        if(pdmFileAttributes->Attributes&FILE_ATTRIBUTE_HIDDEN)
        {
            m_pFileSystemFolder->m_ulShellAttributes |= (SFGAO_HIDDEN|SFGAO_GHOSTED);
        } else
        {
            m_pFileSystemFolder->m_ulShellAttributes &= ~(SFGAO_HIDDEN|SFGAO_GHOSTED);
        }
    }
    return hr;
}

HRESULT CDirectoryVisitFromSelf::SetName(HWND hWnd, LPSTR pszNewName)
{
    UINT uNewNameLen = strlen(pszNewName);
    if(uNewNameLen != _mbstrlen(pszNewName))
    {
        return XBDM_BADFILENAME;
    }

    HRESULT hr = S_OK;
    char  szWireNameOld[MAX_PATH];
    char  szWireNameNew[MAX_PATH];
    LPSTR pszParse;
    CXboxFolder::GetWireName(szWireNameOld, NULL, m_pFileSystemFolder->m_pszPathName);
    strcpy(szWireNameNew, szWireNameOld);
    pszParse = strrchr(szWireNameNew, '\\');
    strcpy(++pszParse, pszNewName);

    LPITEMIDLIST oldPidl = GetPidl(CPidlUtils::PidlTypeAbsolute);
    if(oldPidl)
    {
        hr = m_pFileSystemFolder->SetName(hWnd, szWireNameOld, szWireNameNew);
        if(SUCCEEDED(hr))
        {
            LPITEMIDLIST newPidl = CPidlUtils::Copy(oldPidl, uNewNameLen);
            if(newPidl)
            {
                DWORD dwEvent;
                LPITEMIDLIST lastItem = CPidlUtils::LastItem(newPidl);
                lastItem->mkid.cb = uNewNameLen+sizeof(UCHAR)+sizeof(USHORT);
                strcpy((LPSTR)lastItem->mkid.abID,pszNewName);
                lastItem = AdvancePtr(lastItem, lastItem->mkid.cb);
                lastItem->mkid.cb = 0;
            
                if(m_pFileSystemFolder->m_ulShellAttributes&SFGAO_FOLDER)
                {
                    dwEvent = SHCNE_RENAMEFOLDER;
                } else
                {
                    dwEvent = SHCNE_RENAMEITEM;
                }
                SHChangeNotify(dwEvent, SHCNF_FLUSH|SHCNF_IDLIST, oldPidl, newPidl);
                CPidlUtils::Free(newPidl);
            }
        }
        CPidlUtils::Free(oldPidl);
    }
    return hr;
}
HRESULT CDirectoryVisitFromSelf::Delete()
{
    _ASSERTE(FALSE);
    return E_NOTIMPL;
}

void CDirectoryVisitFromSelf::GetWireName(LPSTR pszWireName)
{
    CXboxFolder::GetWireName(pszWireName, NULL, m_pFileSystemFolder->m_pszPathName);
}

//---------------------------------------------------------------------
// CFileOrDirVisit - implements IXboxFileOrDirVisit while recursing directories
//---------------------------------------------------------------------
class CFileOrDirVisitRecurse: public IXboxFileOrDirVisit
{
  public:
   virtual void         GetName(OUT LPSTR pszItemName);
   virtual LPITEMIDLIST GetPidl(CPidlUtils::PIDLTYPE PidlType);
   virtual ULONG        GetShellAttributes();
   virtual void         GetConsoleName(OUT LPSTR pszConsoleName);
   virtual void         GetFileAttributes(PDM_FILE_ATTRIBUTES pdmFileAttributes);
   virtual HRESULT      SetFileAttributes(PDM_FILE_ATTRIBUTES pdmFileAttributes);
   virtual HRESULT      SetName(HWND hWnd, LPSTR pszNewName){return E_NOTIMPL;}
   virtual HRESULT      Delete();
   virtual void         GetWireName(LPSTR pszWireName);
   

   //
   //   Actually perform recursion.
   //
   static void Go(
       CXboxFileSystemFolder *pDirectory,
       LPCSTR pszRelativePath,
       IXboxVisitor *pVisitor,
       DWORD* pdwFlags
       );
   
   void Recurse(DWORD *pdwFlags);
   
   //
   //   Recurse Context
   //
   char m_szWireName[MAX_PATH];
   IXboxVisitor *m_pVisitor;
   IXboxConnection *m_pConnection;
   CXboxFileSystemFolder *m_pDirectory;
   PDM_FILE_ATTRIBUTES m_pdmFileAttributes;
   LPSTR m_pszName;
};

void 
CFileOrDirVisitRecurse::Go(
   CXboxFileSystemFolder *pDirectory,
   LPCSTR pszRelativePath,
   IXboxVisitor *pVisitor,
   DWORD* pdwFlags
   )
{
    CFileOrDirVisitRecurse recurseObject;
    recurseObject.m_pConnection = pDirectory->m_pConnection;
    recurseObject.m_pDirectory = pDirectory;
    recurseObject.m_pVisitor = pVisitor;
    if(pszRelativePath)
    {
      CXboxFolder::GetWireName(recurseObject.m_szWireName, pDirectory->m_pszPathName, pszRelativePath);
      recurseObject.m_pszName = strrchr(recurseObject.m_szWireName, '\\') + 1;
    } else
    {
      CXboxFolder::GetWireName(recurseObject.m_szWireName, NULL, pDirectory->m_pszPathName);
      recurseObject.m_pszName = recurseObject.m_szWireName + strlen(recurseObject.m_szWireName);
    }
    recurseObject.Recurse(pdwFlags);
}
   
void
CFileOrDirVisitRecurse::Recurse(DWORD *pdwFlags)
{
    PDM_WALK_DIR        pdmWalkDir = NULL;
    DM_FILE_ATTRIBUTES  dmFileAttributes;
    LPSTR               pszNextChar;
    DWORD               dwCount;
    if(SUCCEEDED(m_pConnection->HrOpenDir(&pdmWalkDir, m_szWireName, &dwCount)))
    {
        pszNextChar = m_szWireName + strlen(m_szWireName);
        *pszNextChar++ = '\\';
        while(SUCCEEDED(m_pConnection->HrWalkDir(&pdmWalkDir, NULL, &dmFileAttributes)))
        {
            m_pdmFileAttributes = &dmFileAttributes;
            strcpy(pszNextChar, dmFileAttributes.Name);
            m_pVisitor->VisitFileOrDir(this, pdwFlags);
            if(*pdwFlags&IXboxVisitor::FlagRecurse)
            {
                Recurse(pdwFlags);
            }
            if((dmFileAttributes.Attributes&FILE_ATTRIBUTE_DIRECTORY)&&(*pdwFlags&IXboxVisitor::FlagCallPost))
            {
                m_pdmFileAttributes = &dmFileAttributes;
                m_pVisitor->VisitDirectoryPost(this, pdwFlags);
            }
        }
        m_pConnection->HrCloseDir(pdmWalkDir);
        *(--pszNextChar) = '\0';
    }
}


void CFileOrDirVisitRecurse::GetName(OUT LPSTR pszItemName)
/*++
  Routine Description:
    Should return the name relative to where we are recursing from.
--*/
{
    strcpy(pszItemName, m_pszName);
}   

LPITEMIDLIST CFileOrDirVisitRecurse::GetPidl(CPidlUtils::PIDLTYPE PidlType)
/*++
  Routine Description:
    
--*/
{
    UINT uRelativeDepth = 1;
    UINT uRelativeLength = 0;
    LPSTR pszWalk = m_pszName;
    LPSTR pszSimpleName = pszWalk;
    UINT  uSimpleNameLength = 0;
    LPITEMIDLIST pidlOut;
    LPITEMIDLIST pidlWalk;
    

    while(*pszWalk)
    {
        uRelativeLength++;
        uSimpleNameLength++;
        if(*pszWalk++ == '\\')
        {
            uRelativeDepth++;
            uSimpleNameLength=0;
            pszSimpleName = pszWalk;
        }
    }

    if(PidlType == CPidlUtils::PidlTypeSimple)
    {
        pidlOut = (LPITEMIDLIST)g_pShellMalloc->Alloc(uSimpleNameLength+sizeof(CHAR)+2*sizeof(USHORT));
        if(pidlOut)
        {
            pidlOut->mkid.cb = uSimpleNameLength+sizeof(CHAR)+sizeof(USHORT);
            memcpy(pidlOut->mkid.abID, pszSimpleName, uSimpleNameLength+1);
            pidlWalk = AdvancePtr(pidlOut, pidlOut->mkid.cb);
            pidlWalk->mkid.cb = 0;
        }
    } else
    {
        //
        //  Calculate how many extra bytes we need for the relative
        //  part of the pidl after whatever we get for the directory.
        //
        UINT uExtraAllocation = uRelativeLength + sizeof(USHORT)*(uRelativeDepth+1)+sizeof(UCHAR);
        //
        //  Get the directories portion of the pidl.
        //
        pidlOut = m_pDirectory->GetPidl(PidlType, uExtraAllocation);

        //
        //  Add our relative poriton of the pidl.
        //
        if(pidlOut)
        {
            LPSTR pszPidlWalk;
            pidlWalk = pidlOut;
            while(pidlWalk->mkid.cb)
            {
                pidlWalk = AdvancePtr(pidlWalk, pidlWalk->mkid.cb);
            }
            pszWalk = m_pszName;
            pszPidlWalk = (LPSTR)pidlWalk->mkid.abID;
            uSimpleNameLength = 0;
            while(*pszWalk)
            {
                uSimpleNameLength++;
                if(*pszWalk != '\\')
                {
                    *pszPidlWalk++ = *pszWalk;
                    
                } else
                {
                    *pszPidlWalk = '\0';
                    pidlWalk->mkid.cb = uSimpleNameLength+sizeof(USHORT);
                    pidlWalk = AdvancePtr(pidlWalk, pidlWalk->mkid.cb);
                    uSimpleNameLength = 0;
                    pszPidlWalk = (LPSTR)pidlWalk->mkid.abID;
                }
                pszWalk++;
            }
            *pszPidlWalk = '\0';
            pidlWalk->mkid.cb = uSimpleNameLength+sizeof(UCHAR)+sizeof(USHORT);
            pidlWalk = AdvancePtr(pidlWalk, pidlWalk->mkid.cb);
            pidlWalk->mkid.cb = 0;
        }
    }
    return pidlOut;
}

ULONG CFileOrDirVisitRecurse::GetShellAttributes()
{
  ULONG ulShellAttributes;
  if(m_pdmFileAttributes->Attributes&FILE_ATTRIBUTE_DIRECTORY)
  {
    ulShellAttributes = DIRECTORY_SHELL_ATTRIBUTES;
  } else
  {
    ulShellAttributes  = FILE_SHELL_ATTRIBUTES;
  }
  if(m_pdmFileAttributes->Attributes&FILE_ATTRIBUTE_READONLY)
  {
    ulShellAttributes  |= SFGAO_READONLY;
  }
  if(m_pdmFileAttributes->Attributes&FILE_ATTRIBUTE_HIDDEN)
  {
    ulShellAttributes  |= (SFGAO_HIDDEN|SFGAO_GHOSTED);
  }
  return ulShellAttributes;
}

void CFileOrDirVisitRecurse::GetConsoleName(OUT LPSTR pszConsoleName)
{
    LPSTR pszPathName = m_pDirectory->m_pszPathName;
    while(*pszPathName && (*pszPathName!='\\'))
    {
        *pszConsoleName++ = *pszPathName++;
    }
    *pszConsoleName = '\0';
}

void CFileOrDirVisitRecurse::GetFileAttributes(PDM_FILE_ATTRIBUTES pDmFileAttributes)
{
    memcpy(pDmFileAttributes, m_pdmFileAttributes, sizeof(DM_FILE_ATTRIBUTES));
}

HRESULT CFileOrDirVisitRecurse::SetFileAttributes(PDM_FILE_ATTRIBUTES pdmFileAttributes)
{
    HRESULT hr;
    DWORD dwOldAttributes = m_pdmFileAttributes->Attributes;
    if(!pdmFileAttributes->Attributes)
    {
        pdmFileAttributes->Attributes = FILE_ATTRIBUTE_NORMAL;
    }
    hr = m_pConnection->HrSetFileAttributes(m_szWireName, pdmFileAttributes);
    if(SUCCEEDED(hr))
    {
        m_pdmFileAttributes->Attributes = pdmFileAttributes->Attributes;
    }
    return hr;
}


HRESULT CFileOrDirVisitRecurse::Delete()
{
    HRESULT hr;
    BOOL fIsFolder = m_pdmFileAttributes->Attributes&FILE_ATTRIBUTE_DIRECTORY ? TRUE : FALSE;
    hr = m_pConnection->HrDeleteFile(m_szWireName, fIsFolder);
    if(hr==XBDM_CANNOTACCESS)
    {
        HRESULT hrLoc;
        
        DWORD dwOldAttributes;
        
        dwOldAttributes = m_pdmFileAttributes->Attributes;
        if(dwOldAttributes&FILE_ATTRIBUTE_READONLY)
        {
            m_pdmFileAttributes->Attributes = FILE_ATTRIBUTE_NORMAL;
            hrLoc = m_pConnection->HrSetFileAttributes(m_szWireName, m_pdmFileAttributes);
            if(SUCCEEDED(hrLoc))
            {
                hr = m_pConnection->HrDeleteFile(m_szWireName, fIsFolder);
                if(FAILED(hr))
                {
                    m_pdmFileAttributes->Attributes = dwOldAttributes;
                    m_pConnection->HrSetFileAttributes(m_szWireName, m_pdmFileAttributes);
                }
            }
        }
    }
    return hr;
}

void CFileOrDirVisitRecurse::GetWireName(LPSTR pszWireName)
{
    strcpy(pszWireName, m_szWireName);
}

//---------------------------------------------------------------------
// Various Implementations of visit.
//---------------------------------------------------------------------
void CXboxRoot::Visit(UINT uIndexCount, const UINT *puIndexList, IXboxVisitor *pVisitor, DWORD dwFlags)
{
    //
    //  You cannot call without FlagContinue.
    //

    dwFlags |= IXboxVisitor::FlagContinue;
    if(0==uIndexCount)
    {
        CFolderVisitFromSelf visit;
        visit.m_pFolder = this;
        pVisitor->VisitRoot(&visit, &dwFlags);
    } else
    {
        UINT uLoop;
        CConsoleVisit consoleVisit;      // All of our children excepting the "Add Console" is a console, so ready this.
        consoleVisit.m_pFolder = this;
        consoleVisit.m_pRoot = this;
        for(uLoop = 0; uLoop < uIndexCount; uLoop++)
        {
            //
            //  Check for the "Add New Console" whose first character 
            //  is always '?', this is the only item for which this
            //  can be true.
            //
            if('?'==*m_rgpszChildNames[puIndexList[uLoop]])
            {
                CFolderVisit visitFolder;
                visitFolder.m_pFolder = this;
                visitFolder.m_uChildIndex = puIndexList[uLoop];
                pVisitor->VisitAddConsole(&visitFolder, &dwFlags);
            } else
            {
                consoleVisit.m_uChildIndex = puIndexList[uLoop];
                pVisitor->VisitConsole(&consoleVisit, &dwFlags);
            }
            if(!(dwFlags&IXboxVisitor::FlagContinue))
            {
                break;
            }
        }
    }
}


void CXboxConsole::Visit(UINT uIndexCount, const UINT *puIndexList, IXboxVisitor *pVisitor, DWORD dwFlags)
{
    //
    //  You cannot call without FlagContinue.
    //
    m_pConnection->HrUseSharedConnection(TRUE);
    dwFlags |= IXboxVisitor::FlagContinue;
    if(0==uIndexCount)
    {
        CConsoleVisitFromSelf visitConsole;
        visitConsole.m_pFolder = this;
        visitConsole.m_pConsole = this;
        pVisitor->VisitConsole(&visitConsole, &dwFlags);
    } else
    {
        UINT uLoop;
        CVolumeVisit volumeVisit;      // All of our children are volumes
        volumeVisit.m_pFolder = this;
        volumeVisit.m_pConsole = this;
        for(uLoop = 0; uLoop < uIndexCount; uLoop++)
        {
            volumeVisit.m_uChildIndex = puIndexList[uLoop];
            pVisitor->VisitVolume(&volumeVisit, &dwFlags);
            if(!(dwFlags&IXboxVisitor::FlagContinue))
            {
                break;
            }
        }
    }
    m_pConnection->HrUseSharedConnection(FALSE);
}


void CXboxVolume::Visit(UINT uIndexCount, const UINT *puIndexList, IXboxVisitor *pVisitor, DWORD dwFlags)
{
    //
    //  You cannot call without FlagContinue.
    //
    m_pConnection->HrUseSharedConnection(TRUE);
    dwFlags |= IXboxVisitor::FlagContinue;
    if(0==uIndexCount)
    {
        CVolumeVisitFromSelf visitVolume;
        visitVolume.m_pFolder = this;
        visitVolume.m_pVolume = this;
        pVisitor->VisitVolume(&visitVolume, &dwFlags);
    } else
    {
        UINT uLoop;
        CFileOrDirVisit fileOrDirVisit;      // All of our children are files or directories
        fileOrDirVisit.m_pFolder = this;
        fileOrDirVisit.m_pFileSystemFolder = this;
        for(uLoop = 0; uLoop < uIndexCount; uLoop++)
        {
            fileOrDirVisit.m_uChildIndex = puIndexList[uLoop];
            pVisitor->VisitFileOrDir(&fileOrDirVisit, &dwFlags);
            if(!(dwFlags&IXboxVisitor::FlagContinue))
            {
                break;
            }

            if(
                (dwFlags&IXboxVisitor::FlagRecurse) && 
                (m_rgChildFileAttributes[puIndexList[uLoop]].Attributes&FILE_ATTRIBUTE_DIRECTORY)
            )
            {
                CFileOrDirVisitRecurse::Go(this, m_rgpszChildNames[puIndexList[uLoop]], pVisitor, &dwFlags);
                if(dwFlags&IXboxVisitor::FlagCallPost)
                {
                    pVisitor->VisitDirectoryPost(&fileOrDirVisit, &dwFlags);
                    if(!(dwFlags&IXboxVisitor::FlagContinue))
                    {
                        break;
                    }
                }
            }
        }
    }
    m_pConnection->HrUseSharedConnection(FALSE);
}

void CXboxDirectory::Visit(UINT uIndexCount, const UINT *puIndexList, IXboxVisitor *pVisitor, DWORD dwFlags)
{
    //
    //  You cannot call without FlagContinue.
    //
    m_pConnection->HrUseSharedConnection(TRUE);
    dwFlags |= IXboxVisitor::FlagContinue;
    if(0==uIndexCount)
    {
        CDirectoryVisitFromSelf visitDirectory;
        visitDirectory.m_pFolder = this;
        visitDirectory.m_pFileSystemFolder = this;
        pVisitor->VisitFileOrDir(&visitDirectory, &dwFlags);
        if(dwFlags&IXboxVisitor::FlagRecurse)
        {
            if(dwFlags&IXboxVisitor::FlagRecurse)
            {
                CFileOrDirVisitRecurse::Go(this, NULL, pVisitor, &dwFlags);
                if(dwFlags&IXboxVisitor::FlagCallPost)
                {
                    pVisitor->VisitFileOrDir(&visitDirectory, &dwFlags);
                }
            }

        }
    } else
    {
        UINT uLoop;
        CFileOrDirVisit fileOrDirVisit;      // All of our children are files or directories
        fileOrDirVisit.m_pFolder = this;
        fileOrDirVisit.m_pFileSystemFolder = this;
        for(uLoop = 0; uLoop < uIndexCount; uLoop++)
        {
            fileOrDirVisit.m_uChildIndex = puIndexList[uLoop];
            pVisitor->VisitFileOrDir(&fileOrDirVisit, &dwFlags);
            if(!(dwFlags&IXboxVisitor::FlagContinue))
            {
                break;
            }

            if(dwFlags&IXboxVisitor::FlagRecurse)
            {
                if(
                    (dwFlags&IXboxVisitor::FlagRecurse) && 
                    (m_rgChildFileAttributes[puIndexList[uLoop]].Attributes&FILE_ATTRIBUTE_DIRECTORY)
                )
                {
                    CFileOrDirVisitRecurse::Go(this, m_rgpszChildNames[puIndexList[uLoop]], pVisitor, &dwFlags);
                    if(dwFlags&IXboxVisitor::FlagCallPost)
                    {
                        pVisitor->VisitDirectoryPost(&fileOrDirVisit, &dwFlags);
                        if(!(dwFlags&IXboxVisitor::FlagContinue))
                        {
                            break;
                        }
                    }
                }
            }
        }
    }
    m_pConnection->HrUseSharedConnection(FALSE);
}


void CXboxFolder::VisitEach(IXboxVisitor *pVisitor, DWORD dwFlags)
/*++
  Routine Description:
    
--*/
{
    //
    //  Allocate an index list.
    //
    UINT *puIndexList = NULL;
    if(m_uChildCount)
    {
        puIndexList = new UINT[m_uChildCount];
        if(!puIndexList) return;
    }
    
    UINT uIndex;
    //
    //  Populate for all of the children.
    //
    for(uIndex=0; uIndex < m_uChildCount; uIndex++) puIndexList[uIndex] = uIndex;
    //
    //  Do the visits
    //
    Visit(m_uChildCount, puIndexList, pVisitor, dwFlags);
    delete [] puIndexList;

}

void CXboxFolder::VisitThese(UINT cidl, LPCITEMIDLIST *apidl, IXboxVisitor *pVisitor, DWORD dwFlags)
/*++
  Routine Description:
        
--*/
{
    HRESULT hr = S_OK;
    UINT *puIndexList = NULL;

    if(cidl)
    {
        _ASSERTE(apidl);
        hr = BuildSelectionIndexList(cidl, apidl, &puIndexList);
    } else
    {
        _ASSERTE(!apidl);
    }

    //
    //  Let the class specific portion finish.
    //
    if(SUCCEEDED(hr))
    {
        Visit(cidl, puIndexList, pVisitor, dwFlags);
        delete [] puIndexList;
    }
}



