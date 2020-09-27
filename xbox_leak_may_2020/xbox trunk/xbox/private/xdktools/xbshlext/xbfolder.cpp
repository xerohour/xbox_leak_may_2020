/*++

Copyright (c) Microsoft Corporation

Module Name:

    xbfolder.cpp

Abstract:

   Implementation of CXboxFolder and its derivatives.

Environment:

    Windows 2000 and Later 
    User Mode
    Compiles UNICODE, but uses many ANSI APIs explictly.

Revision History:
    
    06-29-2001 : created

--*/

#include "stdafx.h"

/********
*********  Organization of Code.  Rather than implementing every class in a different code
*********  file. All the methods declared in the base class as virtual are implemented for
*********  each inheriting class one after the other.  For the most part methods are
*********  implemented in the order they are declared.  They only deviations is that 
*********  in some cases, public, protected and private are interleaved.
********/

/*
** Construction:
**
**   CXboxFolder implements a default constructor
**   and an InitBaseClass method.
**
**   Every derived class that can be instantiated
**   implements: 
**
**      1) a private constructor
**      1) a public  static Create   method
**      2) a public  virtual Clone   method
**      3) a public  virtual Destroy method
**
**   EXCEPT CXboxRoot is special.  It is served
**      by the module's class factory and the
**      default c'tor works.  It is initialized with
**      
**
**   Create:
**      Constructs a class and initializes it to mirror
**      the desired item.  If it fails, there is no memory
**      allocation.
**
**      The arguments to Create a class specific.
**
**   Clone:
**   CloneSelection:
**      Constucts an duplicate of itself with a specified
**      subset of children.
**
**   Destroy:
**      Destroys the object completely cleaning up, the object
**      then deletes itself.  Destroy fails and ASSERTS during
**      recursion.
**
**   The also usually have a private c'tor that initialize
**   anything fail-safe.
*/


/*
** Constructors - initialize members to a known state
**                real construction is left for 
**                the Create methods (which use
**                CXboxFolder::InitBaseClass)
*/

CXboxFolder::CXboxFolder() : 
    m_pszPathName(NULL),
    m_pszName(NULL),
    m_ulShellAttributes(0),
    m_uPathDepth(0),
    m_uPathLen(0),
    m_pidlRoot(NULL),
    m_uRootPidlLen(0),
    m_fSelectionClone(FALSE),
    m_fChildrenValid(FALSE),
    m_uAllocatedChildCount(0),   
    m_uChildCount(0), 
    m_uLastChildIndex(0),
    m_rgpszChildNames(NULL),
    m_rgulChildShellAttributes(NULL)
{
    INIT_TRACKABLEOBJECT_NAME(CXboxFolder);
}

CXboxRoot::CXboxRoot() :  
  m_rgdwChildIPAddresses(NULL),
  m_rgpszActualNames(NULL),
  m_uDefaultConsole((UINT)-1)
{
    INIT_TRACKABLEOBJECT_NAME(CXboxRoot);  
    m_dwSelectionVerbs = ROOT_VERBS;
}

CXboxConsole::CXboxConsole() :
  m_pConnection(NULL),
  m_pszDisplayName(NULL),
  m_dwIPAddress(0),
  m_rgullChildFreeSpace(NULL),
  m_rgullChildTotalSpace(NULL),
  m_rguChildVolumeType(NULL)
{
  INIT_TRACKABLEOBJECT_NAME(CXboxConsole);
  m_dwSelectionVerbs = CONSOLE_VERBS;
}

CXboxFileSystemFolder::CXboxFileSystemFolder() :
  m_rgChildFileAttributes(NULL),
  m_rgszChildTypeNames(NULL)
{
    INIT_TRACKABLEOBJECT_NAME(CXboxConsole);
}

HRESULT 
CXboxFolder::InitBaseClass(
    LPCSTR          pszPathName,
    ULONG           ulShellAttributes,
    LPCITEMIDLIST   pidlRoot
    )
/*++
  Routine Description:
    Initializes the name, pidl, and ShellAttributes
    of a newly created CXboxFolder.
--*/
{
    _ASSERTE(pszPathName);
    LPCSTR pszParse = pszPathName;

    //
    //  Setup the name and path members
    //

    m_uPathLen = 0;
    m_uPathDepth = 1;
    while(*pszParse)
    {
        if(L'\\' == *pszParse)
        {
            m_uPathDepth++;
        }
        m_uPathLen++;
        pszParse++;
    }
    m_pszPathName = new CHAR[m_uPathLen+1];
    
    if(m_pszPathName)
    {
        //
        // Assume that memcpy is more efficient than
        // strcpy, since we already know the length
        //

        memcpy(m_pszPathName, pszPathName, m_uPathLen+1);

        //
        //  m_pszName is pointer to the simple name.
        //  If '\\' is not found than it is the same
        //  as m_pszPathName.
        //

        m_pszName = strrchr(m_pszPathName, '\\');
        if(m_pszName)
        {
          m_pszName++;
        }else
        {
          m_pszName = m_pszPathName;
        } 
        
    } else
    {
        return E_OUTOFMEMORY;
    }
    
    //
    // Setup pidl members
    //

    m_pidlRoot = CPidlUtils::Copy(pidlRoot);
    if(m_pidlRoot)
    {
        m_uRootPidlLen = CPidlUtils::GetLength(m_pidlRoot);
    } else
    {
        delete [] m_pszPathName;
        m_pszPathName = NULL;
        return E_OUTOFMEMORY;
    }

    //
    //  Copy the shell attributes
    //

    m_ulShellAttributes = ulShellAttributes;
    
    return S_OK;
}

HRESULT
CXboxConsole::Create(
    LPCSTR          pszResolveName,
    LPCITEMIDLIST   pidlRoot,
    IXboxConnection *pConnection,
    REFIID          riid,
    void            **ppv
    )
/*++
  Routine Description:
    Creates an object of type CXboxConsole.
  Arguments:
    pszResolveName  - name to use finding on the wire finding the box.
                      This name goes into the pidl.
    pidlRoot        - pidl of Xbox root.
    pConnection     - connection to talk to console.
    riid            - desired interface.
    ppv             - pointer to receive new CXboxConsole.
--*/
{
    HRESULT hr;
    CComObject<CXboxConsole> *pNewConsole;

    *ppv = NULL;

    hr = CComObject<CXboxConsole>::CreateInstance(&pNewConsole);
    if(SUCCEEDED(hr))
    {
        hr = pNewConsole->InitBaseClass(
                    pszResolveName,
                    CONSOLE_SHELL_ATTRIBUTES,
                    pidlRoot
                    );
        
        if(SUCCEEDED(hr))
        {
            // 
            // The name oif the box is the resolve name.  If the box is specified
            // by IP address, we only show the actual name in the property page.
            //
            UINT uDisplayNameLen = strlen(pszResolveName)+1;
            pNewConsole->m_pszDisplayName = new char[uDisplayNameLen];
            if(pNewConsole->m_pszDisplayName)
            {
                memcpy(pNewConsole->m_pszDisplayName, pszResolveName, uDisplayNameLen);
                hr = pConnection->HrResolveXboxName(&pNewConsole->m_dwIPAddress);
                if(SUCCEEDED(hr))
                {
                    pConnection->AddRef();
                    pNewConsole->m_pConnection = pConnection;
                    hr = pNewConsole->QueryInterface(riid, ppv);
                }
            } else
            {
                hr = E_OUTOFMEMORY;
            }
        }
        if(!SUCCEEDED(hr))
        {
            delete pNewConsole;
        }
    }
    return hr;
}

HRESULT 
CXboxVolume::Create(
    LPCSTR          pszPathName,
    LPCITEMIDLIST   pidlRoot,
    IXboxConnection *pConnection,
    REFIID          riid,
    void            **ppv
    )
/*++
  Routine Description:
    Creates an object of type CXboxVolume.
  Arguments:
    pszPathName  - name to use finding on the wire finding the box.
                   This name goes into the pidl.
    pidlRoot     - pidl of Xbox root.
    pConnection  - connection to talk to volume.
    riid         - desired interface.
    ppv          - pointer to receive new CXboxVolume.
--*/
{
    HRESULT hr = E_OUTOFMEMORY;
    CComObject<CXboxVolume> *pNewVolume;

    *ppv = NULL;
    
    hr = CComObject<CXboxVolume>::CreateInstance(&pNewVolume);
    if(SUCCEEDED(hr))
    {
        hr = pNewVolume->InitBaseClass(
                pszPathName,
                VOLUME_SHELL_ATTRIBUTES|SFGAO_CANRENAME,
                pidlRoot
                );

        if(SUCCEEDED(hr))
        {
            pConnection->AddRef();
            pNewVolume->m_pConnection = pConnection;
            hr = pNewVolume->QueryInterface(riid, ppv);
        }
        
        if(!SUCCEEDED(hr))
        {
            delete pNewVolume;
        }
    }

    return hr;
}

HRESULT 
CXboxDirectory::Create(
    LPCSTR          pszPathName,
    LPCITEMIDLIST   pidlRoot,
    IXboxConnection *pConnection,
    REFIID          riid,
    void            **ppv
    )
{
    HRESULT hr = E_OUTOFMEMORY;
    CComObject<CXboxDirectory> *pNewDirectory;

    *ppv = NULL;
    
    hr = CComObject<CXboxDirectory>::CreateInstance(&pNewDirectory);
    if(SUCCEEDED(hr))
    {
        hr = pNewDirectory->InitBaseClass(
                pszPathName,
                DIRECTORY_SHELL_ATTRIBUTES,
                pidlRoot
                );

        if(SUCCEEDED(hr))
        {
            pConnection->AddRef();
            pNewDirectory->m_pConnection = pConnection;
            hr = pNewDirectory->QueryInterface(riid, ppv);
        }
        
        if(!SUCCEEDED(hr))
        {
            delete pNewDirectory;
        }
    }
    return hr;
}

void CXboxFolder::Destroy()
{
    delete [] m_pszPathName;
    m_pszPathName = NULL;
    CPidlUtils::Free(m_pidlRoot);
    FreeChildren();
}

void CXboxConsole::Destroy()
{
    m_pConnection->Release();
    m_pConnection = NULL;
    delete m_pszDisplayName;
    m_pszDisplayName = NULL;
    CXboxFolder::Destroy();
}

void CXboxFileSystemFolder::Destroy()
{
    m_pConnection->Release();
    m_pConnection = NULL;
    CXboxFolder::Destroy();
}

/*
**  Clone - Makes a copy of a CXboxFolder, including:
**
**          Information about item.
**          Current Selection.
**
**          
*/

HRESULT CXboxFolder::BuildSelectionIndexList(UINT cidl, LPCITEMIDLIST *apidl, UINT **ppuIndexList)
{
    HRESULT hr;
    UINT *puIndexList = NULL;
    UINT uIndex;

    *ppuIndexList = NULL;
    
    _ASSERTE(cidl);
    _ASSERTE(apidl);

    //
    //  Build an index list.
    //
    puIndexList = new UINT[cidl];
    if(!puIndexList)
    {
        return E_OUTOFMEMORY;
    }

    //
    //  Walk the pidls and build a list of indices
    //
    for(uIndex = 0; uIndex < cidl; uIndex++)
    {
        hr = GetChildIndex((LPSTR)apidl[uIndex]->mkid.abID, puIndexList+uIndex);
        if(FAILED(hr))
        {
            delete [] puIndexList;    
            return hr;
        }
    }

    *ppuIndexList = puIndexList;
    return S_OK;
}

HRESULT CXboxFolder::DuplicateBaseMembers(CXboxFolder *pClone)
/*++
  Routine Description:

   Duplicates (for cloning purposes) the base CXBoxFolder members
   that do not need to be handled in a class specific way.  This is
   mostly:

   Name, Pidl and Shell Attribute related members.

  Argument:

   pClone - clone to be initialized.

  Return Value:
   S_OK           - on success.
   E_OUTOFMEMORY  - If an allocation failed.

--*/
{
    pClone->m_pszPathName  = new char[m_uPathLen+1];
    if(pClone->m_pszPathName)
    {
        memcpy(pClone->m_pszPathName, m_pszPathName, m_uPathLen+1);
        pClone->m_pszName = pClone->m_pszPathName + (m_pszName - m_pszPathName);
        pClone->m_pidlRoot = (LPITEMIDLIST)g_pShellMalloc->Alloc(m_uRootPidlLen);
        if(pClone->m_pidlRoot)
        {
            memcpy(pClone->m_pidlRoot, m_pidlRoot, m_uRootPidlLen);
        } else
        {
            delete [] pClone->m_pszPathName;
            return E_OUTOFMEMORY;
        }
    } else
    {
        return E_OUTOFMEMORY;
    }

    pClone->m_uPathLen = m_uPathLen;
    pClone->m_uPathDepth = m_uPathDepth;
    pClone->m_uRootPidlLen = m_uRootPidlLen;
    pClone->m_ulShellAttributes = m_ulShellAttributes;
    pClone->m_fSelectionClone = TRUE;
    pClone->m_fChildrenValid = TRUE;
    pClone->m_uAllocatedChildCount = 0;
    pClone->m_uChildCount = 0;
    pClone->m_uLastChildIndex = 0;
    pClone->m_rgpszChildNames = NULL;
    pClone->m_rgulChildShellAttributes = NULL;

    return S_OK;
}

HRESULT CXboxFolder::CloneSelection(UINT cidl, LPCITEMIDLIST *apidl, CXboxFolder **ppSelectionClone)
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
        hr = CloneSelection(cidl, puIndexList, ppSelectionClone);
        delete [] puIndexList;
    }
    return hr;
}


HRESULT CXboxFolder::Clone(CXboxFolder **ppSelectionClone)
{
    _ASSERTE(m_fSelectionClone);
    
    //
    //  Copy all the children
    //
    UINT *puIndexList = new UINT[m_uChildCount];
    if(!puIndexList)
    {
        return E_OUTOFMEMORY;
    }
    UINT uIndex;
    for(uIndex=0; uIndex < m_uChildCount; uIndex++) puIndexList[uIndex] = uIndex;
    
    HRESULT hr = CloneSelection(m_uChildCount, puIndexList, ppSelectionClone);
    delete [] puIndexList;
    return hr;
}


HRESULT CXboxRoot::CloneSelection(UINT uIndexCount, const UINT *puIndexList, CXboxFolder **ppSelectionClone)
{
    HRESULT hr = S_OK;
    CComObject<CXboxRoot> *pClone;

    *ppSelectionClone = NULL;

    //
    //  Create the new folder, unitialized
    //

    hr = CComObject<CXboxRoot>::CreateInstance(&pClone);
    if(FAILED(hr))
    {
        return hr;
    }

    pClone->AddRef();  //It needs a reference count, so we can delete with release.

    //
    //  Now copy all the common base guts
    //
    hr = DuplicateBaseMembers(pClone);
    
    //
    //  Now handle class specific and children
    //
    if(SUCCEEDED(hr))
    {
        pClone->m_rgdwChildIPAddresses = NULL;
        pClone->m_rgpszActualNames = NULL;
        pClone->m_uDefaultConsole = 0;
        if(uIndexCount)
        {
            UINT uIndex;
            hr = pClone->ReallocateChildren(uIndexCount);
            pClone->m_dwSelectionVerbs = CONSOLE_VERBS;
            for(uIndex = 0; uIndex < uIndexCount; uIndex++, pClone->m_uChildCount++)
            {
                //
                //  Copy the name.
                //

                UINT uNameLength = strlen(m_rgpszChildNames[puIndexList[uIndex]])+1;
                pClone->m_rgpszChildNames[uIndex] = new char[uNameLength];
                if(pClone->m_rgpszChildNames[uIndex])
                {
                   memcpy(pClone->m_rgpszChildNames[uIndex], m_rgpszChildNames[puIndexList[uIndex]], uNameLength);
                } else
                {
                   hr = E_OUTOFMEMORY;
                   break;
                }

                //
                //  If one of the items is the AddConsole Wizard, change the available verbs
                //
                if(*m_rgpszChildNames[puIndexList[uIndex]] == '?')
                {
                    pClone->m_dwSelectionVerbs = ADDCONSOLE_VERBS;
                }

                //
                //  Copy the shell attributes
                //

                pClone->m_rgulChildShellAttributes[uIndex] = m_rgulChildShellAttributes[puIndexList[uIndex]];

                //
                //  Copy the ip addresses
                //

                pClone->m_rgdwChildIPAddresses[uIndex] = m_rgdwChildIPAddresses[puIndexList[uIndex]];
                
                //  Copy the actual names
                if(m_rgpszActualNames[puIndexList[uIndex]])
                {
                    if(XBOX_USE_USER_PROVIDED_NAME == m_rgpszActualNames[puIndexList[uIndex]])
                    {
                        pClone->m_rgpszActualNames[uIndex] = XBOX_USE_USER_PROVIDED_NAME;
                    } else
                    {
                        UINT uNameLen = strlen(m_rgpszActualNames[puIndexList[uIndex]])+1;
                        pClone->m_rgpszActualNames[uIndex] = new char[uNameLen];
                        if(pClone->m_rgpszActualNames[uIndex])
                        {
                            //we know the length so memcpy is more efficient.
                            memcpy(pClone->m_rgpszActualNames[uIndex], m_rgpszActualNames[puIndexList[uIndex]], uNameLen);
                        }
                    }
                }
            }
        }
    }
    
    if(SUCCEEDED(hr))
    {
        *ppSelectionClone = pClone;
    } else
    {
        pClone->Release();
    }

    return hr;
}



HRESULT CXboxConsole::CloneSelection(UINT uIndexCount, const UINT *puIndexList, CXboxFolder **ppSelectionClone)
{
    HRESULT hr = S_OK;
    LPSTR   pszDriveLetters;
    CComObject<CXboxConsole> *pClone;

    *ppSelectionClone = NULL;

    //
    //  Create the new folder, unitialized
    //

    hr = CComObject<CXboxConsole>::CreateInstance(&pClone);
    if(FAILED(hr))
    {
        return hr;
    }

    pClone->AddRef();  //It needs a reference count, so we can delete with release.

    //
    //  Now copy all the common base guts
    //
    hr = DuplicateBaseMembers(pClone);
    
    //
    //  Now handle class specific and children
    //
    if(SUCCEEDED(hr))
    {
        UINT uNameLength = strlen(m_pszDisplayName)+1;
        pClone->m_pszDisplayName = new char[uNameLength];
        if(!pClone->m_pszDisplayName)
        {
            hr = E_OUTOFMEMORY;
        } else
        {
            memcpy(pClone->m_pszDisplayName, m_pszDisplayName, uNameLength);
            pClone->m_pConnection = m_pConnection;
            m_pConnection->AddRef();
            pClone->m_dwIPAddress = m_dwIPAddress;
            if(uIndexCount)
            {
                UINT uIndex;
                pClone->m_dwSelectionVerbs = VOLUME_VERBS;
                hr = pClone->ReallocateChildren(uIndexCount);
                pszDriveLetters = new char[2*uIndexCount];
                if(pszDriveLetters)
                {
                    for(uIndex = 0; uIndex < uIndexCount; uIndex++, pClone->m_uChildCount++)
                    {
                        //
                        //  Copy the name.
                        //

                        pClone->m_rgpszChildNames[uIndex] = pszDriveLetters;
                        *pszDriveLetters++ = *m_rgpszChildNames[puIndexList[uIndex]];
                        *pszDriveLetters++ = '\0';

                        //
                        //  Copy the shell attributes
                        //

                        pClone->m_rgulChildShellAttributes[uIndex] = m_rgulChildShellAttributes[puIndexList[uIndex]];

                        //
                        //  Copy the class specific information
                        //

                        pClone->m_rgullChildFreeSpace[uIndex]  = m_rgullChildFreeSpace[puIndexList[uIndex]];
                        pClone->m_rgullChildTotalSpace[uIndex] = m_rgullChildTotalSpace[puIndexList[uIndex]];
                        pClone->m_rguChildVolumeType[uIndex]   = m_rguChildVolumeType[puIndexList[uIndex]];
                    }
                }
            }
        }
    }
    
    if(SUCCEEDED(hr))
    {
        *ppSelectionClone = pClone;
    } else
    {
        //
        //  Should clean up everything we managed to set.
        //

        pClone->Release();
    }

    return hr;
}


HRESULT CXboxFileSystemFolder::CloneSelection(UINT uIndexCount, const UINT *puIndexList, CXboxFileSystemFolder *pClone)
{
    //
    //  Now copy all the common base guts
    //
    HRESULT hr = DuplicateBaseMembers(pClone);
    
    //
    //  Now handle class specific and children
    //
    if(SUCCEEDED(hr))
    {
        pClone->m_pConnection = m_pConnection;
        m_pConnection->AddRef();
        if(uIndexCount)
        {
            UINT uIndex;
            // Assume the maximum number of verbs.  As we go through actual items
            // we AND with the ones that actually apply.
            pClone->m_dwSelectionVerbs = DIRECTORY_VERBS|DIRECTORY_VERBS|FILE_VERBS|XBE_VERBS;
            hr = pClone->ReallocateChildren(uIndexCount);
            for(uIndex = 0; uIndex < uIndexCount; uIndex++, pClone->m_uChildCount++)
            {
                //
                //  Copy the name.
                //

                UINT uNameLength = strlen(m_rgpszChildNames[puIndexList[uIndex]])+1;
                pClone->m_rgpszChildNames[uIndex] = new char[uNameLength];
                if(pClone->m_rgpszChildNames[uIndex])
                {
                   memcpy(pClone->m_rgpszChildNames[uIndex], m_rgpszChildNames[puIndexList[uIndex]], uNameLength);
                } else
                {
                   hr = E_OUTOFMEMORY;
                   break;
                }

                //
                //  Copy the type name.
                //

                uNameLength = strlen(m_rgszChildTypeNames[puIndexList[uIndex]])+1;
                pClone->m_rgszChildTypeNames[uIndex] = new char[uNameLength];
                if(pClone->m_rgszChildTypeNames[uIndex])
                {
                   memcpy(pClone->m_rgszChildTypeNames[uIndex], m_rgszChildTypeNames[puIndexList[uIndex]], uNameLength);
                } else
                {
                   hr = E_OUTOFMEMORY;
                   break;
                }

                //
                //  Copy the shell attributes
                //

                pClone->m_rgulChildShellAttributes[uIndex] = m_rgulChildShellAttributes[puIndexList[uIndex]];

                //
                //  Copy the file attributes
                //

                memcpy(
                   &pClone->m_rgChildFileAttributes[uIndex],
                   &m_rgChildFileAttributes[puIndexList[uIndex]],
                   sizeof(DM_FILE_ATTRIBUTES)
                   );

                // Figure out the supported verbs
                if(pClone->m_rgChildFileAttributes[uIndex].Attributes&FILE_ATTRIBUTE_DIRECTORY)
                {
                    pClone->m_dwSelectionVerbs &= DIRECTORY_VERBS;
                } else
                {
                    //If it is an XBE file, AND with XBE verbs
                    if(IsXbeFile(pClone->m_rgpszChildNames[uIndex]))
                    {
                        pClone->m_dwSelectionVerbs &= XBE_VERBS;
                    } else
                    {
                        pClone->m_dwSelectionVerbs &= FILE_VERBS;
                    }
                }
            }
        }
    }
    
    return hr;
}


HRESULT CXboxVolume::CloneSelection(UINT uIndexCount, const UINT *puIndexList, CXboxFolder **ppSelectionClone)
{
    HRESULT hr = S_OK;
    CComObject<CXboxVolume> *pClone;

    *ppSelectionClone = NULL;

    //
    //  Create the new folder, unitialized
    //

    hr = CComObject<CXboxVolume>::CreateInstance(&pClone);
    if(FAILED(hr))
    {
        return hr;
    }

    pClone->AddRef();  //It needs a reference count, so we can delete with release.

    hr = CXboxFileSystemFolder::CloneSelection(uIndexCount, puIndexList, pClone);
    
    if(SUCCEEDED(hr))
    {
        pClone->m_uVolumeType = m_uVolumeType;
        *ppSelectionClone = pClone;
    } else
    {
        //
        //  Should clean up everything we managed to set.
        //

        pClone->Release();
    }

    return hr;
}


HRESULT CXboxDirectory::CloneSelection(UINT uIndexCount, const UINT *puIndexList, CXboxFolder **ppSelectionClone)
{
    HRESULT hr = S_OK;
    CComObject<CXboxDirectory> *pClone;

    *ppSelectionClone = NULL;

    //
    //  Create the new folder, unitialized
    //

    hr = CComObject<CXboxDirectory>::CreateInstance(&pClone);
    if(FAILED(hr))
    {
        return hr;
    }

    pClone->AddRef();  //It needs a reference count, so we can delete with release.

    hr = CXboxFileSystemFolder::CloneSelection(uIndexCount, puIndexList, pClone);
    
    if(SUCCEEDED(hr))
    {
        *ppSelectionClone = pClone;
    } else
    {
        //
        //  Should clean up everything we managed to set.
        //

        pClone->Release();
    }

    return hr;
}

UINT CXboxFolder::GetPidlLen(CPidlUtils::PIDLTYPE PidlType)
/*++
  Routine Description:
   Computes the length of a pidl from the name information.
  Arguments:
   PidlType - The type pidl for which to get the length.  Valid values are:

              CXboxFolder::PidlTypeSimple   - a single SHITEMID, i.e. relative to the immediate
                                            parent.

              CXboxFolder::PidlTypeRoot     - relative to Xbox Root, but not including the
                                            the portion from the Desktop to Xbox Root.
              CXboxFolder::PidlTypeAbsolute - the fully qualified pidl, including the Desktop.
  Return Value:
   The size of the desired pidl in bytes.
--*/
{
  //
  //  Special handling for root
  //
  if(0==m_uPathDepth)
  {
    return m_uRootPidlLen;
  }

  //
  // Each SHITEMID in the pidl is a NULL terminated
  // string corresponding exactly to the Name of the
  // item.
  //
  
  //
  // Start sizeof(USHORT) to termiante the pidl +
  // the sizeof(CHAR) for the last NULL.
  //
  
  UINT uPidlLen = sizeof(USHORT)+sizeof(CHAR);

  //
  //  Switch on the pidl type
  //
  switch (PidlType)
  {
    case CPidlUtils::PidlTypeSimple:
      //Add in the length of the simple name.
      uPidlLen += strlen(m_pszName);
      break;
    case CPidlUtils::PidlTypeAbsolute:
      uPidlLen += m_uRootPidlLen;
      uPidlLen -= sizeof(USHORT); //The root pidl was already terminated
      //Fall through
    case CPidlUtils::PidlTypeRoot:
      //Add in the path len, the extra '\0' are a wash
      //with missing '\'.
      uPidlLen += m_uPathLen;
      //Add in a USHORT for each cb of each SHITEMID.
      uPidlLen += (m_uPathDepth*sizeof(USHORT));
      break;
    default:
      _ASSERTE("PidlType Unknown" && FALSE);
  }

  return uPidlLen;
}

LPITEMIDLIST CXboxFolder::GetPidl(CPidlUtils::PIDLTYPE PidlType, UINT uExtraAllocation)
/*++
 Routine Description:
  Get a Pidl for the current XboxItem.   Allocate memory using
  the Shell IMalloc (i.e. from SHGetMalloc) and fills it in with desire pidl.
 Arguments:
  PidlType          - See GetPidlLen for allowed values.
  uExtraAllocation  - Extra bytes to allocate beyond those required.  Used for
                      tacking on extra SHITEMID by GetChildPidl.
 Return Value:
  On success - the desired pidl.
  On failure - NULL.  The only failure mode is an out of memory failure.
--*/
{
  UINT uPidlLen = GetPidlLen(PidlType);
  LPITEMIDLIST pidl = (LPITEMIDLIST)g_pShellMalloc->Alloc(uPidlLen+uExtraAllocation);
  LPITEMIDLIST pidlWalk = pidl;
  LPSTR pszPath = m_pszPathName;
  USHORT cb;
  if(pidl)
  {
    //
    //  If this is the root (m_uPathDepth is 0)
    //  just copy the m_pidlRoot.
    //
    if(!m_uPathDepth)
    {
        memcpy(pidlWalk, m_pidlRoot, m_uRootPidlLen);
    } else
    //
    //  Otherwise, we have more work.
    //
    {
        //
        // If we need an absolute pidl, start with the root.
        //

        if(PidlType==CPidlUtils::PidlTypeAbsolute)
        {
            // Copy root pidl (leaving off termination.
            memcpy(pidlWalk, m_pidlRoot, m_uRootPidlLen-sizeof(USHORT));
            pidlWalk = AdvancePtr(pidlWalk, m_uRootPidlLen-sizeof(USHORT));
        }
    
        //
        // Otherwise if it is simple, get only the last item id.
        //
    
        else if(PidlType==CPidlUtils::PidlTypeSimple)
        {
            pszPath = m_pszName;
        }
    
        //
        // Copy each item from the path.
        //

        while(*pszPath)
        {
            cb = 0;
            while(*pszPath && (*pszPath != '\\')) pidlWalk->mkid.abID[cb++] = *pszPath++;
            pidlWalk->mkid.abID[cb++] = '\0';
            if(*pszPath == '\\') pszPath++;
            pidlWalk->mkid.cb = cb+sizeof(USHORT);
            pidlWalk = AdvancePtr(pidlWalk, pidlWalk->mkid.cb);
        }

        //
        //  Terminate the pidl
        //

        pidlWalk->mkid.cb = 0;
    }
  }
  _ASSERTE(pidl);
  ATLTRACE("GetPidl returning:(0x%0.8x)",pidl);
  DEBUG_DUMP_PIDL(pidl);
  return pidl;
}

HRESULT CXboxFolder::CompareItemIDs(UINT uColumn, int iNameCmp, const SHITEMID *pItemId1, const SHITEMID *pItemId2)
/*++
 Routine Description:
    Compares two immediate children of a CXboxFolder based on an uColumn.  The Column
      ID is specific to the class type of the CXboxFolder.
      
  Arguments:
      uColumn  - index of column to sort bt.
      iNameCmp - result of case-insenstive comparison on builds carried out by caller.
      pItemId1 - simple SHITEMID of first ID to compare.
      pItemId2 - simple SHITEMID of second ID to compare.

  Return Value:
      S_EQUAL    - Items are the same.  This actually should never happen as sameness can
                   and is determined by CXboxFolder::CompareIDs, which is the only caller.
      S_LESS     - pItemId1 comes before pItemId2
      S_GREATER  - pItemId1 comes after  pItemId2
      E_EQUAL    - Comparison failed, however items should be considered equal.  In theory,
                   a random caller may get this, but this case is not expected to be hit.
      E_LESS     - Comparison failed, however, if must succeed pItemId1 should be
                   before pItemId2
      E_GREATER  - Comparison failed, however, if must succeed pItemId2 should be
                   before pItemId1

  Comments:

      This routine turns is not virtual.  It turns the pItemId1, and pItemId2 into indices
      into the list of children.  Then it calls 
      CompareItemIDs(UINT iColumn, int iNameCmp, UINT uChildIndex1, UINT uChildIndex2),
      which does this real class dependent work.

      If either of the children are not found, then error results.  In the case of an error:
       1) If pItemId1 and pItemId2 are both invalid, return FailFromShort(iNameCmp)
       2) If only one them is invalid put the valid one first.

--*/

{
    HRESULT hr, hr2;
    UINT    uChildIndex1;
    UINT    uChildIndex2;
    
    hr=GetChildIndex((LPCSTR)pItemId1->abID, &uChildIndex1);
    hr2=GetChildIndex((LPCSTR)pItemId2->abID, &uChildIndex2);
    
    if(SUCCEEDED(hr))
    {
        if(SUCCEEDED(hr2))
        {
            //
            //  If the indices match they are equal
            //
            if(uChildIndex1 == uChildIndex2)
            {
                return 0;
            }
            return CompareItemIDs(uColumn, iNameCmp, uChildIndex1, uChildIndex2);
        } else
        {
            //
            // pItemId1 is valid and pItemId2 is not.
            //
            return ResultFromCompare(-1, true);
        }
    }
    
    //
    //  If we are here pItemId1 is invalid
    //

    if(SUCCEEDED(hr2))
    {
        //
        // pItemId2 is valid and pItemId1 is not.
        //
        return ResultFromCompare(1, true);
    }

    //
    //  Neither is valid
    //
    return ResultFromCompare(iNameCmp, true);
}

/*
**  Method Description: CompareItemIDs(UINT uColumn, int iNameCmp, UINT uChildIndex1, UINT uChildIndex2)
**
**      Compares two immediate children of a CXboxFolder based on an uColumn.  The Column
**      ID is specific to the class type of the CXboxFolder.
**      
**  Arguments:
**      uColumn      - index of column to sort bt.
**      iNameCmp     - result of case-insenstive comparison on builds carried out by caller.
**      uChildIndex1 - index of first child to compare
**      uChildIndex2 - index of second child to compare
**
**  Implemented By:
**      CXboxRoot           - Its children are CXboxConsole and the "Add New Console Wizard"
**      CXboxConsole        - Its children are CXboxVolume
**      CXboxFileSystemFolder - Its children are CXboxDirOrFile.
**
**  Return Value:
**      S_EQUAL    - Items are the same.  This actually should never happen as sameness can and is
**                   determined by CXboxFolder::CompareIDs, which is the only caller.
**      S_LESS     - pFolderId1 comes before pFolderId2
**      S_GREATER  - pFolderId1 comes after  pFolderId2
**
**      Cannot Fail.
**
**  Comments:
**
**      Identical items are those that have the same case-insensitive name.  If uColumn
**      sorts by a criteria other than name,  a name comparison is used to break ties.
**
**  Sort Order:
**      The interpretation of uColumn is on a class by class basis. See comments on individual
**      implementations below for details.
*/

HRESULT CXboxRoot::CompareItemIDs(UINT uColumn, int iNameCmp, UINT uChildIndex1, UINT uChildIndex2)
/*++
 Routine Description:
  Root implemenation, see overview for CompareItemIDs above.

 Sort Order by uColumn:
    XBOX_CONSOLE_COLUMN_NAME (0):   Sort by the name of the console.
    XBOX_CONSOLE_COLUMN_IPADDR (1): Sort by the IP address of the console.
    
    Regardless of uColumn, the "Add New Console" wizard is always before
    any of the consoles.
  
  Implementation Notes:
  
    index 0 is always the "Add New Console" wizard.

    Two different pidls could refer to the same Xbox, one by name and the other
    by IP address.  After must dicussion it was decided that these will be treated
    as two different boxes for all purposes.  Matching up the boxes requires some
    twisted logic, and may have severe perfromance implications as well.  This will
    effect very few users, and the consequences of not matching them are exceedingly
    benign.
    
--*/    
{
    _ASSERTE(uChildIndex1!=uChildIndex2);   

    //
    // Check for "Add New Console" wizard
    //

    if(0==uChildIndex1)
    {
      return S_LESS;
    } 
            
    if(0==uChildIndex2)
    {
      return S_GREATER;
    }

    //
    //  Compare passed on uColumn
    //
    
    if(uColumn == XBOX_CONSOLE_COLUMN_IPADDR)
    {
      DWORD dwIpAddressChild1 = GetChildIpAddress(uChildIndex1);
      DWORD dwIpAddressChild2 = GetChildIpAddress(uChildIndex2);
      if(dwIpAddressChild1 < dwIpAddressChild2)
      {
        return S_LESS;
      } else if(dwIpAddressChild1 > dwIpAddressChild2)
      {
        return S_GREATER;
      }
    }
    
    return ResultFromCompare(iNameCmp);
}

HRESULT CXboxConsole::CompareItemIDs(UINT uColumn, int iNameCmp, UINT uChildIndex1, UINT uChildIndex2)
/*++
 Routine Description:
  Console implemenation, see overview for CompareItemIDs above.

  Sort Order by uColumn:
    XBOX_VOLUME_COLUMN_NAME (0):          Sort by the name (drive letter) of the volume.
    XBOX_VOLUME_COLUMN_TYPE (1):          Sort by the type of volume (MU versus HD).
    XBOX_VOLUME_COLUMN_FREE_CAPACITY (2): Sort by the free capacity of volume.
    XBOX_VOLUME_COLUMN_TOTAL_CAPACITY(3): Sort by the total capacity of volume.
    
--*/
{
    _ASSERTE(uChildIndex1!=uChildIndex2);   

    //
    //  Compare based on uColumn
    //
    switch (uColumn)
    {
      case XBOX_VOLUME_COLUMN_TYPE:
        if(m_rguChildVolumeType[uChildIndex1] < m_rguChildVolumeType[uChildIndex2])
        {
            return S_LESS;
        }
        if(m_rguChildVolumeType[uChildIndex1] > m_rguChildVolumeType[uChildIndex2])
        {
            return S_GREATER;
        }
        break;
      case XBOX_VOLUME_COLUMN_FREE_CAPACITY:
        if(m_rgullChildFreeSpace[uChildIndex1] < m_rgullChildFreeSpace[uChildIndex2])
        {
            return S_LESS;
        }
        if(m_rgullChildFreeSpace[uChildIndex1] > m_rgullChildFreeSpace[uChildIndex2])
        {
            return S_GREATER;
        }
        break;
      case XBOX_VOLUME_COLUMN_TOTAL_CAPACITY:
        if(m_rgullChildTotalSpace[uChildIndex1] < m_rgullChildTotalSpace[uChildIndex2])
        {
            return S_LESS;
        }
        if(m_rgullChildTotalSpace[uChildIndex1] > m_rgullChildTotalSpace[uChildIndex2])
        {
            return S_GREATER;
        }
        break;
      case XBOX_VOLUME_COLUMN_NAME:
      default:
          break;
    }
    //
    //  If they were equal by other qualities fall back on name.
    //
    return ResultFromCompare(iNameCmp);
}

HRESULT CXboxFileSystemFolder::CompareItemIDs(UINT uColumn, int iNameCmp, UINT uChildIndex1, UINT uChildIndex2)
/*++
 Routine Description:
  Console implemenation, see overview for CompareItemIDs above.

  Sort Order by uColumn:
    XBOX_FILE_COLUMN_NAME      (0): Sort by the name file or directory.
    XBOX_FILE_COLUMN_SIZE      (1): Sort by the size of the file.
    XBOX_FILE_COLUMN_TYPE      (2): Sort by the type of file.
    XBOX_FILE_COLUMN_MODIFIED  (3): Sort by the date modified.
    XBOX_FILE_COLUMN_ATTRIBUTE (4): Sort by the file attributes.
    
    Regardless of uColumn, directories always appear before files.
--*/
{
    _ASSERTE(uChildIndex1!=uChildIndex2);
    BOOL fBothDirectories = FALSE;

    //
    // If one is a Directory and the other is not, the directory
    // comes first.
    //

    if(m_rgChildFileAttributes[uChildIndex1].Attributes&FILE_ATTRIBUTE_DIRECTORY)
    {
      if(m_rgChildFileAttributes[uChildIndex2].Attributes&FILE_ATTRIBUTE_DIRECTORY)
      {
         fBothDirectories = TRUE;
      }
      else {
        return S_LESS;  
      }
    } else
    {
      if(m_rgChildFileAttributes[uChildIndex2].Attributes&FILE_ATTRIBUTE_DIRECTORY)
      {
        return S_GREATER;  
      }
    }

    //
    //  Compare based on uColumn
    //
    switch (uColumn)
    {
      case XBOX_FILE_COLUMN_ATTRIBUTE:
        if(m_rgChildFileAttributes[uChildIndex1].Attributes < m_rgChildFileAttributes[uChildIndex2].Attributes)
        {
            return S_LESS;
        }
        if(m_rgChildFileAttributes[uChildIndex1].Attributes > m_rgChildFileAttributes[uChildIndex2].Attributes)
        {
            return S_GREATER;
        }
        break;
      case XBOX_FILE_COLUMN_MODIFIED:
        if(m_rgChildFileAttributes[uChildIndex1].ChangeTime.dwHighDateTime < 
           m_rgChildFileAttributes[uChildIndex2].ChangeTime.dwHighDateTime)
        {
            return S_LESS;
        }
        if(m_rgChildFileAttributes[uChildIndex1].ChangeTime.dwHighDateTime > 
           m_rgChildFileAttributes[uChildIndex2].ChangeTime.dwHighDateTime)
        {
            return S_GREATER;
        }
        if(m_rgChildFileAttributes[uChildIndex1].ChangeTime.dwLowDateTime  < 
           m_rgChildFileAttributes[uChildIndex2].ChangeTime.dwLowDateTime)
        {
            return S_LESS;
        }
        if(m_rgChildFileAttributes[uChildIndex1].ChangeTime.dwLowDateTime  > 
           m_rgChildFileAttributes[uChildIndex2].ChangeTime.dwLowDateTime)
        {
            return S_GREATER;
        }
        break;

      case XBOX_FILE_COLUMN_SIZE:
        if(!fBothDirectories)
        {
            if(m_rgChildFileAttributes[uChildIndex1].SizeHigh < 
               m_rgChildFileAttributes[uChildIndex2].SizeHigh)
            {
                return S_LESS;
            }
            if(m_rgChildFileAttributes[uChildIndex1].SizeHigh > 
               m_rgChildFileAttributes[uChildIndex2].SizeHigh)
            {
                return S_GREATER;
            }
            if(m_rgChildFileAttributes[uChildIndex1].SizeLow  < 
               m_rgChildFileAttributes[uChildIndex2].SizeLow)
            {
                return S_LESS;
            }
            if(m_rgChildFileAttributes[uChildIndex1].SizeLow  > 
               m_rgChildFileAttributes[uChildIndex2].SizeLow)
            {
                return S_GREATER;
            }
        }
        break;
      case XBOX_FILE_COLUMN_TYPE:
        if(!fBothDirectories)
        {
            int iTypeCmp;
            iTypeCmp = _stricmp(m_rgszChildTypeNames[uChildIndex1], m_rgszChildTypeNames[uChildIndex2]);
            if(iTypeCmp)
            {
                return ResultFromCompare(iTypeCmp);
            }
        }
        break;
      case XBOX_FILE_COLUMN_NAME:
      default:
          break;
    }
    //
    //  If they were equal by other qualities fall back on name.
    //
    return ResultFromCompare(iNameCmp);
}

BOOL CXboxFolder::AreItemsIDsIdentical(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, BOOL *pfSameMachine)
/*++
 Routine Description:
   This routine compares two absolute pidls to determine 1) If they are identical, and 2)
   if they are on the same machine.

   It is static.  It is part of CXboxFolder purely to contain the pidl interpretation encapsulated in this class.
--*/
{
    //Check the first item id, the machine name
    if(_stricmp((char *)pidl1->mkid.abID, (char *)pidl2->mkid.abID))
    {
        *pfSameMachine = FALSE;
        return FALSE;
    }
    *pfSameMachine = TRUE;  //Same machine

    //Check the rest of the item ids.
    do
    {
        //Advance to comparing the next pidl's
        pidl1 = AdvancePtr(pidl1, pidl1->mkid.cb);
        pidl2 = AdvancePtr(pidl2, pidl2->mkid.cb);
        
        // Check loop termination conditions.
        if(0==pidl1->mkid.cb)
        {
            if(0==pidl2->mkid.cb)
            {
                // At the end of pidl1 and pidl2, these are the same.
                return TRUE;
            }
            //At end of pidl1, but not pidl2, these are different.
            return FALSE;
        } else if(0==pidl2->mkid.cb)
        {
            //At end of pidl2, but not pidl1, these are different.
            return FALSE;
        }
        // At the end of neither pidl1 or pidl2, so keep looping.
    } while(0==_stricmp((char *)pidl1->mkid.abID, (char *)pidl2->mkid.abID));

    //We fell out of the loop when a difference was found, these are different.
    return FALSE;
}

UINT CXboxFolder::GetChildCount(BOOL fGuesstimate)
/*++
  Routine Description:
    Gets the count of child items.  If the child list is invalid
    can guess or can force enumeration.
  Arguments:
    fGuesstimate - if true take a guess rather than force enumeration.
  Return Value:
    Child Count, or guess.
--*/
{
    //
    //  If the children are not valid, decide whether or 
    //  not to guess or refresh the children.
    //

    if(!m_fChildrenValid)
    {

        if(fGuesstimate)
        {
            //TODO: Performance tune this value, perhaps by class.
            return 15;
        } else
        {
            RefreshChildren();
        }
    }
    return m_uChildCount;
}

HRESULT CXboxFolder::GetChildIndex(LPCSTR pszChildName, UINT *puIndex)
/*++
  Routine Description:
    Searchs for a child by name and returns an index.

  Arguments:
    pszChildName - [IN]  name of child to search for.
    puIndex      - [OUT] pointer to receive index of child.
  
  Comments:
    1) Does NOT refresh children.  Fails if the
       child list is invalid.
    2) Must be an immediate child.
    3) Must handle trailing '\\' so we don't simply use strcmp
    4) Convert to upper case while comparing.

  Performance Tuning:

    This routine is called a lot.  As often as not, it is called for exactly the same item it was
    called for the previous time.  50% of the other items it is called for the very next item.
    So our search loop always starts on the previously returned index.
    
--*/
{
    //
    // Only search if the child list is valid.
    //

    if(m_fChildrenValid && m_uChildCount)
    {
        UINT uIndex;    

        //
        //  Loop over children, looking for a matching name
        //
        uIndex = m_uLastChildIndex;
        do
        {
            LPCSTR pszSource = m_rgpszChildNames[uIndex];
            LPCSTR pszTarget = pszChildName;
            BOOL   fMatch = TRUE;

            //
            //  check for a match
            //
            while(*pszSource && *pszTarget)
            {
                if(toupper(*pszSource++) != toupper(*pszTarget++))
                {   
                    fMatch = FALSE;
                    break;
                }
            }
            
            //
            //  All the common characters are the
            //  same, check to see if one is longer
            //  than the other.
            //
            if(fMatch)
            {
                //
                // If the sources is at the end, and the
                // target is at the end (including a trailing '\\'
                // then they match.
                //
                if(('\0'==*pszSource) && (('\0'==*pszTarget)||('\\'==*pszTarget)))
                {
                    *puIndex = m_uLastChildIndex = uIndex;
                    return S_OK;
                }
            }
            //Increment uIndex for the next loop
            uIndex = (uIndex+1)%m_uChildCount;
        } while(uIndex != m_uLastChildIndex);  //loop until we get back where we started.
    }

    //
    //  No match, :(
    //

    *puIndex = 0;
    return E_FAIL;
}

LPITEMIDLIST CXboxFolder::GetChildPidl(UINT uIndex, CPidlUtils::PIDLTYPE PidlType)
/*++
 Routine Description:
  Creates a pidl for a child.

 Arguments:
  uIndex   - index of child for which to create pidl.
  PidlType - See CPidlUtils for pidl types.

 Return Value:
  On success, pointer to requested pidl.
  On failure, NULL.

 Comment:
  If CPidlUtils::PidlTypeSimple it is all done here, otherwise GetPidl is invoked.
   
--*/
{
   LPITEMIDLIST pidlReturn = NULL;
   LPITEMIDLIST pidlWalk = NULL;

   //
   //  If the children are not valid, return NULL.
   //

   if(!m_fChildrenValid) return NULL;

   //
   // Calculate the length of the pidl
   //
   UINT uChildItemIdLen = strlen(m_rgpszChildNames[uIndex]) + sizeof(USHORT) + sizeof(char);
   
   //
   // If not simple add our pidl length to that of the
   // child.
   //

   if(PidlType != CPidlUtils::PidlTypeSimple)
   {
       //
       //  Get our pidl, and ask for space at end to tack on
       //  our child.
       //

       pidlReturn = GetPidl(PidlType, uChildItemIdLen);

       //
       //  Go to the end of the pidl.
       //
       if(pidlReturn)
       {
         pidlWalk = AdvancePtr(pidlReturn, GetPidlLen(PidlType)-2);
        _ASSERT(0==pidlWalk->mkid.cb);
       }
   } else
   //
   //   Otherwise, allocate space for a simple pidl
   //
   {
       pidlReturn = 
       pidlWalk = (LPITEMIDLIST)g_pShellMalloc->Alloc(uChildItemIdLen+sizeof(USHORT));
   }

   //
   //  If the allocation worked, we need to add the SHITEMID for our child.
   //

   if(pidlWalk)
   {
       LPSTR pszChildNameTarget = (LPSTR)pidlWalk->mkid.abID;
       LPSTR pszChildNameSource = m_rgpszChildNames[uIndex];
       while(*pszChildNameSource)
       {
          *pszChildNameTarget++ = *pszChildNameSource++;
       }
       *pszChildNameTarget++ = *pszChildNameSource++;
       pidlWalk->mkid.cb = (USHORT)uChildItemIdLen;
       pidlWalk = AdvancePtr(pidlWalk, uChildItemIdLen);
       pidlWalk->mkid.cb = 0;
   }
   _ASSERTE(pidlReturn);
   return pidlReturn;
}

void CXboxFolder::GetSelectShellAttributes(IN OUT ULONG *pulShellAttributes)
{
    _ASSERTE(m_fSelectionClone);
    
    if(m_uChildCount)
    {
        UINT uIndex;
        for(uIndex = 0; uIndex < m_uChildCount; uIndex++)
        {
            *pulShellAttributes &= m_rgulChildShellAttributes[uIndex];
        }
    } else
    {
        *pulShellAttributes &= m_ulShellAttributes;
    }
    return;
}

HRESULT CXboxRoot::RefreshChildren()
/*++
  Routine Description:
    RefreshChildren reads the list of known boxes from registry.

    1) Gets count of items from registry.
    2) Free's existing children.
    3) Allocate new children.
    4) Use CManageConsoles to handle all the registery stuff.

--*/
{
    HRESULT hr;
    UINT    uChildCount;
    char    szConsoleName[MAX_CONSOLE_NAME];
    DWORD   dwConsoleNameSize;
    DWORD   dwConsoleCountSize;
    
    //
    //  If the children are already valid then there is no reason
    //  to do this.
    //
    if(m_fChildrenValid)
    {
        return S_OK;
    }

    CManageConsoles consoleManager;

    //
    //  The extra child is for the Add New Console Wizard.
    //
    uChildCount = consoleManager.GetMaxCount() + 1;
    
    //
    //  Free Children
    //

    FreeChildren();
    _ASSERTE(0==m_uChildCount);

    //
    //  Allocate Children
    //
    hr = ReallocateChildren(uChildCount);
    if(FAILED(hr))
    {
        return hr;
    }
    
    //
    //  Initialize Add New Console Wizard entry
    //

    m_rgpszChildNames[m_uChildCount] = new char[sizeof(ADD_NEW_CONSOLE_PIDL_NAME)];
    if(m_rgpszChildNames[m_uChildCount])
    {
         memcpy(m_rgpszChildNames[m_uChildCount], ADD_NEW_CONSOLE_PIDL_NAME, sizeof(ADD_NEW_CONSOLE_PIDL_NAME));
         m_rgulChildShellAttributes[m_uChildCount] = ADDNEWCONSOLE_SHELL_ATTRIBUTES;
         m_rgpszActualNames[m_uChildCount] = XBOX_USE_USER_PROVIDED_NAME;
         m_uChildCount++;
    }

    //
    //  Enumerate Consoles
    //
    
    dwConsoleNameSize = sizeof(szConsoleName);
    while( (m_uChildCount < uChildCount) && consoleManager.GetNext(szConsoleName, &dwConsoleNameSize))
    {
        //
        //  Allocate space to store the console name
        //
        dwConsoleNameSize+=1;
        m_rgpszChildNames[m_uChildCount] = new char[dwConsoleNameSize];
        
     
        //
        //  If the allocation worked, setup the console.
        //
        if(m_rgpszChildNames[m_uChildCount])
        {
            if(consoleManager.IsDefault(szConsoleName))
            {   
                m_uDefaultConsole = m_uChildCount;
            }
            IXboxConnection *pConnection;
            memcpy(m_rgpszChildNames[m_uChildCount], szConsoleName, dwConsoleNameSize);
            m_rgulChildShellAttributes[m_uChildCount] = CONSOLE_SHELL_ATTRIBUTES;
            m_rgdwChildIPAddresses[m_uChildCount] = 0;
            m_rgpszActualNames[m_uChildCount] = NULL;
            
            /*
            hr = Utils::GetXboxConnection(szConsoleName, &pConnection);
            if(SUCCEEDED(hr))
            {
                m_rgdwChildIPAddresses[m_uChildCount] = 0;
                pConnection->HrResolveXboxName(&m_rgdwChildIPAddresses[m_uChildCount]);
                pConnection->Release();
            }
            */
            m_uChildCount++;
        }

        // For next iteration
        dwConsoleNameSize = sizeof(szConsoleName);
    }

    //
    //  Set the children valid is true here, otherwise the
    //  default Xbox will show up twice.
    m_fChildrenValid = TRUE;
    
    return S_OK;
}

DWORD CXboxRoot::GetChildIpAddress(UINT uChildIndex, BOOL fRetry)
{
    HRESULT hr = S_OK;
    _ASSERTE(uChildIndex <= m_uChildCount);
    
    //
    //  If the IP address is INVALID_HANDLE_VALUE, then we have tried
    //  to get it in the past and failed.  If fRetry, then
    //  we try the wire protocol again to get it, otherwise we
    //  return 0 for the IP address.
    //
    if(0xFFFFFFFF == m_rgdwChildIPAddresses[uChildIndex])
    {
        if(!fRetry) return 0;
        m_rgdwChildIPAddresses[uChildIndex] = 0;
    }

    //
    //  If the IP address is 0, then we have to go over the wire
    //  to get it.
    //
    if(!m_rgdwChildIPAddresses[uChildIndex])
    {
        IXboxConnection *pConnection;
        hr = Utils::GetXboxConnection(m_rgpszChildNames[uChildIndex], &pConnection);
        if(SUCCEEDED(hr))
        {
            hr = pConnection->HrResolveXboxName(&m_rgdwChildIPAddresses[uChildIndex]);
            pConnection->Release();
        }
        //
        //  If we failed getting the interface or resolving the Xbox name to an IP address
        //  then we mark the IP address as INVALID_HANDLE_VALUE, and return 0.
        //
        if(FAILED(hr))
        {
            m_rgdwChildIPAddresses[uChildIndex] = 0xFFFFFFFF;
            return 0;
        }
    }

    //
    //  Return what we have.
    //
    return m_rgdwChildIPAddresses[uChildIndex];
}

LPCSTR CXboxRoot::GetActualName(UINT uChildIndex)
{

    HRESULT hr;
    hr = S_OK;
    _ASSERTE(uChildIndex <= m_uChildCount);

    //
    //  If the actual name is available, just use it.
    //
    if(m_rgpszActualNames[uChildIndex])
    {
        //  If the name is XBOX_USE_USER_PROVIDED_NAME, it means use the
        //  name that is is in the registry, that the user provided.
        if(XBOX_USE_USER_PROVIDED_NAME == m_rgpszActualNames[uChildIndex])
        {
            return m_rgpszChildNames[uChildIndex];
        }
        // Otherwise, it is a pointer and we should return it.
        return m_rgpszActualNames[uChildIndex];
    }

    //
    // If the user provided the name, then just use it.
    //
    if(INADDR_NONE==inet_addr(m_rgpszChildNames[uChildIndex]))
    {
        m_rgpszActualNames[uChildIndex] = XBOX_USE_USER_PROVIDED_NAME;
        return m_rgpszChildNames[uChildIndex];
    }
    

    // If we are here, we need to go over the wire to get the name.
    // Don't try going over the wire, if we failed at our last attempt
    // to find the box, when getting the IP address.
    if(0xFFFFFFFF != m_rgdwChildIPAddresses[uChildIndex])
    {
        IXboxConnection *pConnection;
        hr = Utils::GetXboxConnection(m_rgpszChildNames[uChildIndex], &pConnection);
        if(SUCCEEDED(hr))
        {
            char  szName[60];
            DWORD dwNameLen;
            hr = pConnection->HrGetNameOfXbox(szName, &dwNameLen, FALSE);
            if(SUCCEEDED(hr))
            {
                m_rgpszActualNames[uChildIndex] = new char[++dwNameLen];
                if(m_rgpszActualNames[uChildIndex])
                {
                    memcpy(m_rgpszActualNames[uChildIndex], szName, dwNameLen);
                    return m_rgpszActualNames[uChildIndex];
                }
            }
            pConnection->Release();
        }
    }
    
    //
    // If we are here, then we couldn't get the actual name for some
    // reason, return whatever the user typed.
    //
    return m_rgpszChildNames[uChildIndex];
}

HRESULT CXboxConsole::RefreshChildren()
/*++
  Routine Description:
    RefreshChildren query for all the volumes
    and rebuild the list of children.

    1) Gets count of items from registry.
    2) Free's existing children.
    3) Allocate new children.
    4) populate the children from registry.
    5) get the default console, and if necessary
       add it to the registry.
    
  Comments:
    How many volumes can there be?  E, S, T, U, V, X, plus
    C and Y (internal only) + 8 MU's, + any aliases.
    Figure that we will never see more that 24.  That is small
    enough that we can just allocate them all.
    
--*/
{
    HRESULT         hr;
    char            szVolumeName[4];
    char            szDriveLetters[24];
    char            *pszDriveNames;
    DWORD           dwDriveCount = 24;
    DWORD           dwDriveIndex;
    ULARGE_INTEGER  uliBogus;

    if(m_fChildrenValid)
    {
        return S_OK;
    }
    
    //
    //  Get the list of drives
    //
    hr = m_pConnection->HrGetDriveList(szDriveLetters, &dwDriveCount);
    if(FAILED(hr))
    {
        return hr;
    }

    //
    //  Allocate space for all the names.
    //
    pszDriveNames = new char[2*dwDriveCount];
    if(!pszDriveNames)
    {
        return E_OUTOFMEMORY;
    }

    //
    //  Free Children
    //

    FreeChildren();
    _ASSERTE(0==m_uChildCount);

    //
    //  Allocate Children
    //
    hr = ReallocateChildren(dwDriveCount);
    
    //
    //  Will be needed as we go through the drive letters
    //
    szVolumeName[1]=':';
    szVolumeName[2]='\\';
    szVolumeName[3]='\0';

    //
    //  Loop over all the volumes and collect information.
    //

    for(dwDriveIndex = 0; dwDriveIndex < dwDriveCount; dwDriveIndex++)
    {
        szVolumeName[0] = szDriveLetters[dwDriveIndex];
        m_rgpszChildNames[m_uChildCount] = pszDriveNames;
        *pszDriveNames++ = *szVolumeName;
        *pszDriveNames++ = '\0';
        m_rgulChildShellAttributes[m_uChildCount] = VOLUME_SHELL_ATTRIBUTES;
        m_rguChildVolumeType[m_uChildCount] = CXboxVolume::GetVolumeType(*szVolumeName);
        
        //
        // Go over the wire to get the free space and total space.
        //
        
        m_pConnection->HrGetDiskFreeSpace(
                        szVolumeName,
                        &uliBogus,
                        (PULARGE_INTEGER)&m_rgullChildTotalSpace[m_uChildCount],
                        (PULARGE_INTEGER)&m_rgullChildFreeSpace[m_uChildCount]
                        );
        m_uChildCount++;
    }

    m_fChildrenValid = TRUE;
    return S_OK;
}

UINT CXboxVolume::GetVolumeType(char cDriveLetter)
{
    cDriveLetter = (char)toupper(cDriveLetter);
    if((cDriveLetter >= 'F') && (cDriveLetter<= 'M'))
    {
        return IDS_DRIVETYPE_MEMORY_UNIT;
    }

    switch(cDriveLetter)
    {
        case 'C':
            return IDS_DRIVETYPE_MAIN_ROOT;
        case 'D':
            return IDS_DRIVETYPE_BOOT; 
        case 'E':
            return IDS_DRIVETYPE_DEVELOPMENT; 
        case 'S':
            return IDS_DRIVETYPE_TITLE_ROOT; 
        case 'T':
            return IDS_DRIVETYPE_TITLE_CURRENT; 
        case 'U':
            return IDS_DRIVETYPE_SAVED_CURRENT; 
        case 'V':
            return IDS_DRIVETYPE_SAVED_ROOT;
        case 'X':
            return IDS_DRIVETYPE_SCRATCH;
        case 'Y':
            return IDS_DRIVETYPE_DASH;
    } 
    return IDS_DRIVETYPE_UNKNOWN;
}

HRESULT CXboxFileSystemFolder::RefreshChildren()
/*++
  Routine Description:
    Walks the directory and creates all the children.  All children
    are either directories or files.
--*/
{
    char szWireName[MAX_PATH];
    PDM_WALK_DIR pdmWalkDir;
    DWORD dwFileCount;
    HRESULT hr;

    if(m_fChildrenValid)
    {
        return S_OK;
    }
    
    //
    //  Get the wire name (for our self).
    //
    GetWireName(szWireName, NULL, m_pszPathName);

    //
    //  Get the first child, as well as a count.
    //
    hr = m_pConnection->HrOpenDir(&pdmWalkDir, szWireName, &dwFileCount);
    if(FAILED(hr))
    {
        return hr;
    }   

    //
    //  Free Children
    //

    FreeChildren();
    _ASSERTE(0==m_uChildCount);

    //
    //  Allocate Children
    //
    hr = ReallocateChildren(dwFileCount);
    if(SUCCEEDED(hr))
    {

        //
        //  Now fill in the children.
        //
        while(SUCCEEDED(m_pConnection->HrWalkDir(&pdmWalkDir, NULL, &m_rgChildFileAttributes[m_uChildCount])))
        {   
            //
            // Have the name point at the name in the DM_FILE_ATTRIBUTES
            //

            m_rgpszChildNames[m_uChildCount] = m_rgChildFileAttributes[m_uChildCount].Name;

            //
            //  Figure out the shell attributes and file type
            //  from the attributes.
            //

            if(m_rgChildFileAttributes[m_uChildCount].Attributes&FILE_ATTRIBUTE_DIRECTORY)
            {
              m_rgulChildShellAttributes[m_uChildCount] = DIRECTORY_SHELL_ATTRIBUTES;
              m_rgszChildTypeNames[m_uChildCount] = (LPSTR)WindowUtils::GetPreloadedString(IDS_PRELOAD_FOLDER_TYPE_NAME);
            } else
            {
              m_rgulChildShellAttributes[m_uChildCount] = FILE_SHELL_ATTRIBUTES;
              m_rgszChildTypeNames[m_uChildCount] = GetFileTypeName(&m_rgChildFileAttributes[m_uChildCount]);
            }
            if(m_rgChildFileAttributes[m_uChildCount].Attributes&FILE_ATTRIBUTE_READONLY)
            {
              m_rgulChildShellAttributes[m_uChildCount] |= SFGAO_READONLY;
            }
            if(m_rgChildFileAttributes[m_uChildCount].Attributes&FILE_ATTRIBUTE_HIDDEN)
            {
              m_rgulChildShellAttributes[m_uChildCount] |= (SFGAO_HIDDEN|SFGAO_GHOSTED);
            }
            m_uChildCount++;
        }
    }
    m_fChildrenValid = TRUE;
    return S_OK;
}


HRESULT CXboxRoot::ReallocateChildren(UINT uChildCount)
{
    
    if(uChildCount > m_uAllocatedChildCount)
    {
        LPSTR *rgpszChildNames;
        ULONG *rgulChildShellAttributes;
        DWORD *rgdwChildIPAddresses;
        LPSTR *rgpszActualNames;

        //
        // Allocate new arrays
        //
        rgpszChildNames          = new LPSTR[uChildCount];
        rgulChildShellAttributes = new ULONG[uChildCount];
        rgdwChildIPAddresses     = new DWORD[uChildCount];
        rgpszActualNames         = new LPSTR[uChildCount];

        if( !rgpszChildNames || !rgulChildShellAttributes || !rgdwChildIPAddresses || !rgpszActualNames)
        {
            delete [] rgpszChildNames;
            delete [] rgulChildShellAttributes;
            delete [] rgdwChildIPAddresses;
            delete [] rgpszActualNames;
            return E_OUTOFMEMORY;
        } else
        {
            if(m_uChildCount)
            {
                memcpy(rgpszChildNames,          m_rgpszChildNames,          sizeof(LPSTR)*m_uChildCount);
                memcpy(rgulChildShellAttributes, m_rgulChildShellAttributes, sizeof(ULONG)*m_uChildCount);
                memcpy(rgdwChildIPAddresses,     m_rgdwChildIPAddresses,     sizeof(DWORD)*m_uChildCount);
                memcpy(rgpszActualNames,         m_rgpszActualNames,         sizeof(LPSTR)*m_uChildCount);
            }
            
            delete [] m_rgpszChildNames;
            delete [] m_rgulChildShellAttributes;
            delete [] m_rgdwChildIPAddresses;
            delete [] m_rgpszActualNames;          
            
            m_rgpszChildNames          = rgpszChildNames;
            m_rgulChildShellAttributes = rgulChildShellAttributes;
            m_rgdwChildIPAddresses     = rgdwChildIPAddresses;
            m_rgpszActualNames         = rgpszActualNames;

            m_uAllocatedChildCount = uChildCount;
        }
    }
    return S_OK;
}

HRESULT CXboxConsole::ReallocateChildren(UINT uChildCount)
{

    if(uChildCount > m_uAllocatedChildCount)
    {
        LPSTR     *rgpszChildNames;
        ULONG     *rgulChildShellAttributes;
        ULONGLONG *rgullChildFreeSpace;
        ULONGLONG *rgullChildTotalSpace;
        UINT      *rguChildVolumeType;

        //
        // Allocate new arrays
        //

        rgpszChildNames          = new LPSTR[uChildCount];
        rgulChildShellAttributes = new ULONG[uChildCount];
        rgullChildFreeSpace      = new ULONGLONG[uChildCount];
        rgullChildTotalSpace     = new ULONGLONG[uChildCount];
        rguChildVolumeType       = new UINT[uChildCount];

        if( 
            !rgpszChildNames ||
            !rgulChildShellAttributes ||
            !rgullChildFreeSpace ||
            !rgullChildTotalSpace ||
            !rguChildVolumeType
        )
        {
            delete [] rgpszChildNames;
            delete [] rgulChildShellAttributes;
            delete [] rgullChildFreeSpace;
            delete [] rgullChildTotalSpace;
            delete [] rguChildVolumeType;
            return E_OUTOFMEMORY;

        } else
        {
            if(m_uChildCount)
            {
                memcpy(rgpszChildNames,          m_rgpszChildNames,          sizeof(LPSTR)*m_uChildCount);
                memcpy(rgulChildShellAttributes, m_rgulChildShellAttributes, sizeof(ULONG)*m_uChildCount);
                memcpy(rgullChildFreeSpace,      m_rgullChildFreeSpace,      sizeof(ULONGLONG)*m_uChildCount);
                memcpy(rgullChildTotalSpace,     m_rgullChildTotalSpace,     sizeof(ULONGLONG)*m_uChildCount);
                memcpy(rguChildVolumeType,       m_rguChildVolumeType,       sizeof(UINT)*m_uChildCount);
            }
            
            delete [] m_rgpszChildNames;
            delete [] m_rgulChildShellAttributes;
            delete [] m_rgullChildFreeSpace;
            delete [] m_rgullChildTotalSpace;
            delete [] m_rguChildVolumeType;
            
            m_rgpszChildNames          = rgpszChildNames;
            m_rgulChildShellAttributes = rgulChildShellAttributes;
            m_rgullChildFreeSpace      = rgullChildFreeSpace;
            m_rgullChildTotalSpace     = rgullChildTotalSpace;
            m_rguChildVolumeType       = rguChildVolumeType;

            m_uAllocatedChildCount = uChildCount;
        }
    }
    return S_OK;
}

HRESULT CXboxFileSystemFolder::ReallocateChildren(UINT uChildCount)
{
    
    if(uChildCount > m_uAllocatedChildCount)
    {
        LPSTR              *rgpszChildNames;
        ULONG              *rgulChildShellAttributes;
        DM_FILE_ATTRIBUTES *rgChildFileAttributes;
        LPSTR              *rgszChildTypeNames;

        //
        // Allocate new arrays
        //

        rgpszChildNames            = new LPSTR[uChildCount];
        rgulChildShellAttributes   = new ULONG[uChildCount];
        rgChildFileAttributes      = new DM_FILE_ATTRIBUTES[uChildCount];
        rgszChildTypeNames         = new LPSTR[uChildCount];

        if( 
            !rgpszChildNames ||
            !rgulChildShellAttributes ||
            !rgChildFileAttributes ||
            !rgszChildTypeNames
        )
        {
            delete [] rgpszChildNames;
            delete [] rgulChildShellAttributes;
            delete [] rgChildFileAttributes;
            delete [] rgszChildTypeNames;
            return E_OUTOFMEMORY;

        } else
        {
            if(m_uChildCount)
            {
                memcpy(rgpszChildNames,          m_rgpszChildNames,          sizeof(LPSTR)*m_uChildCount);
                memcpy(rgulChildShellAttributes, m_rgulChildShellAttributes, sizeof(ULONG)*m_uChildCount);

                memcpy(rgChildFileAttributes,    m_rgChildFileAttributes,    sizeof(DM_FILE_ATTRIBUTES)*m_uChildCount);
                memcpy(rgszChildTypeNames,       m_rgszChildTypeNames,       sizeof(LPSTR)*m_uChildCount);
            }
            
            delete [] m_rgpszChildNames;
            delete [] m_rgulChildShellAttributes;
            delete [] m_rgChildFileAttributes;
            delete [] m_rgszChildTypeNames;
            
            m_rgpszChildNames          = rgpszChildNames;
            m_rgulChildShellAttributes = rgulChildShellAttributes;
            m_rgChildFileAttributes    = rgChildFileAttributes;
            m_rgszChildTypeNames       = rgszChildTypeNames;

            m_uAllocatedChildCount = uChildCount;
        }
    }
    return S_OK;
}

void CXboxRoot::FreeChildren()
/*++
  Routine Description:
    Frees the data structues associate with children of the root.
--*/
{
    UINT uIndex;

    for(uIndex = 0; uIndex < m_uChildCount; uIndex++)
    {
        delete m_rgpszChildNames[uIndex];
        
        //XBOX_USE_USER_PROVIDED_NAME - means that the actual name is the same as the name
        //                       thus, it is not really a pointer, not is it NULL, so don't delete!
        if(XBOX_USE_USER_PROVIDED_NAME!=m_rgpszActualNames[uIndex])
            delete [] m_rgpszActualNames[uIndex];
    }

    delete [] m_rgpszChildNames;
    delete [] m_rgulChildShellAttributes;
    delete [] m_rgdwChildIPAddresses;
    delete [] m_rgpszActualNames;
    
    m_rgpszChildNames = NULL;
    m_rgulChildShellAttributes = NULL;
    m_rgdwChildIPAddresses = NULL;
    m_rgpszActualNames = NULL;
    
    m_uAllocatedChildCount = 0;
    m_uChildCount = 0;
    m_uLastChildIndex = 0;
}

void CXboxConsole::FreeChildren()
/*++
  Routine Description:
    Frees the data structues associate with children of a console.
--*/
{
    UINT uIndex;

    delete [] m_rgpszChildNames;
    delete [] m_rgulChildShellAttributes;
    
    delete [] m_rgullChildFreeSpace;
    delete [] m_rgullChildTotalSpace;
    delete [] m_rguChildVolumeType;
  
    m_rgpszChildNames          = NULL;
    m_rgulChildShellAttributes = NULL;
    m_rgullChildFreeSpace      = NULL;
    m_rgullChildTotalSpace     = NULL;
    m_rguChildVolumeType       = NULL;

    m_uAllocatedChildCount = 0;
    m_uChildCount = 0;
    m_uLastChildIndex = 0;
}

void CXboxFileSystemFolder::FreeChildren()
/*++
  Routine Description:
    Frees the data structues associate with children of the root.
--*/
{
    UINT uIndex;

    for(uIndex = 0; uIndex < m_uChildCount; uIndex++)
    {
        if(!(m_rgChildFileAttributes[uIndex].Attributes&FILE_ATTRIBUTE_DIRECTORY))
        {
            delete m_rgszChildTypeNames[uIndex];
        }
    }
    delete [] m_rgpszChildNames;
    delete [] m_rgulChildShellAttributes;
    delete [] m_rgChildFileAttributes;
    delete [] m_rgszChildTypeNames;

    m_rgpszChildNames          = NULL;
    m_rgulChildShellAttributes = NULL;
    m_rgChildFileAttributes    = NULL;
    m_rgszChildTypeNames       = NULL;

    m_uAllocatedChildCount = 0;
    m_uChildCount = 0;
    m_uLastChildIndex = 0;
}



HRESULT CXboxRoot::ValidateItem(LPCSTR pszPathName, UINT uRelativeDepth, DWORD *pdwAttributes)
/*++
  Routine Description:
    Validates a child item over the wire and gets its shell attributes.  It does not rely on
    any cached information and does not look at the child list.  It can also look down multiple
    levels.
  Arguments:
    pszPathName    - path relative to this item (the root)
    uRelativeDepth - number of filesystem elements in the relative path.
    pdwAttributes  - [IN]  desired shell attribute flags
                     [OUT] desired shell attribute flags that apply to item.
  Return Value:
    S_OK if item exists.
    various errors if it does not.
--*/
{
  HRESULT            hr;
  char               szNameBuffer[MAX_PATH];
  LPSTR              pszTarget;
  LPCSTR             pszSource;
  IXboxConnection    *pConnection = NULL;
  DM_FILE_ATTRIBUTES dmFileAttributes;

  //
  //  Is it the "Add New Console Wizard"
  //  TODO: Implement this.
  //


  //
  //  Get a console connection
  //
  pszTarget = szNameBuffer;
  pszSource = pszPathName;
  while(('\0'!=*pszSource) && ('\\'!=*pszSource))
  {
    *pszTarget++ = *pszSource++;
  }
  *pszTarget = '\0';
  hr = Utils::GetXboxConnection(szNameBuffer, &pConnection);
  if(FAILED(hr)) return hr;

  //
  //  If item is a console, we are done.
  //
  if(1==uRelativeDepth)
  {
    
    //
    //  Don't need to the box anymore.
    //

    pConnection->Release();

    //
    //  If we are going to succeed, and the attributes
    //  are requested, then fill out the attributes.
    //
    if(SUCCEEDED(hr) && pdwAttributes)
    {
        *pdwAttributes &= CONSOLE_SHELL_ATTRIBUTES;
    }
    return hr;
  }

  //
  //  Build the wire name, and try to get the file attributes.
  //

  GetWireName(szNameBuffer, NULL, pszPathName);
  hr = pConnection->HrGetFileAttributes(szNameBuffer, &dmFileAttributes);
  pConnection->Release(); //Done with the connection, success or not.
  if(SUCCEEDED(hr))
  {
    DWORD dwAttributes;
    
    //
    //  Is it a volume?
    //
    if(2==uRelativeDepth)
    {
        _ASSERTE(dmFileAttributes.Attributes&FILE_ATTRIBUTE_DIRECTORY);
        dwAttributes = VOLUME_SHELL_ATTRIBUTES;
    } else if(dmFileAttributes.Attributes&FILE_ATTRIBUTE_DIRECTORY)
    //
    //  Or a directory?
    //
    {
        dwAttributes = DIRECTORY_SHELL_ATTRIBUTES;
    } else
    //
    //  Then must be a file.
    //
    {
        dwAttributes = FILE_SHELL_ATTRIBUTES;
    }

    //
    //  OR in SFGAO_READONLY and SFGAO_HIDDEN if applicable.
    //

    if(dmFileAttributes.Attributes&FILE_ATTRIBUTE_READONLY)
    {
        dwAttributes |= SFGAO_READONLY;
    }
    if(dmFileAttributes.Attributes&FILE_ATTRIBUTE_HIDDEN)
    {
        dwAttributes |= (SFGAO_HIDDEN|SFGAO_GHOSTED);
    }

    //
    // Set the out parameter
    //
    *pdwAttributes &= dwAttributes;
  }
  return hr;
}

HRESULT CXboxConsole::ValidateItem(LPCSTR pszPathName, UINT uRelativeDepth, DWORD *pdwAttributes)
/*++
  Routine Description:
    Validates a child item over the wire and gets its shell attributes.  It does not rely on
    any cached information and does not look at the child list.  It can also look down multiple
    levels.
  Arguments:
    pszPathName    - path relative to this item (the root)
    uRelativeDepth - number of filesystem elements in the relative path.
    pdwAttributes  - [IN]  desired shell attribute flags
                     [OUT] desired shell attribute flags that apply to item.
  Return Value:
    S_OK if item exists.
    various errors if it does not.
--*/
{
  HRESULT            hr;
  char               szNameBuffer[MAX_PATH];
  DM_FILE_ATTRIBUTES dmFileAttributes;

  //
  //  Build the wire name, and try to get the file attributes.
  //

  GetWireName(szNameBuffer, m_pszPathName , pszPathName);
  hr = m_pConnection->HrGetFileAttributes(szNameBuffer, &dmFileAttributes);
  if(SUCCEEDED(hr))
  {
    DWORD dwAttributes;
    
    //
    //  Is it a volume?
    //
    if(1==uRelativeDepth)
    {
        _ASSERTE(dmFileAttributes.Attributes&FILE_ATTRIBUTE_DIRECTORY);
        dwAttributes = VOLUME_SHELL_ATTRIBUTES;
    } else if(dmFileAttributes.Attributes&FILE_ATTRIBUTE_DIRECTORY)
    //
    //  Or a directory?
    //
    {
        dwAttributes = DIRECTORY_SHELL_ATTRIBUTES;
    } else
    //
    //  Then must be a file.
    //
    {
        dwAttributes = FILE_SHELL_ATTRIBUTES;
    }

    //
    //  OR in SFGAO_READONLY and SFGAO_HIDDEN if applicable.
    //

    if(dmFileAttributes.Attributes&FILE_ATTRIBUTE_READONLY)
    {
        dwAttributes |= SFGAO_READONLY;
    }
    if(dmFileAttributes.Attributes&FILE_ATTRIBUTE_HIDDEN)
    {
        dwAttributes |= (SFGAO_HIDDEN|SFGAO_GHOSTED);
    }

    //
    // Set the out parameter
    //
    *pdwAttributes &= dwAttributes;
  }
  return hr;
}

HRESULT CXboxFileSystemFolder::ValidateItem(LPCSTR pszPathName, UINT, DWORD *pdwAttributes)
/*++
  Routine Description:
    Validates a child item over the wire and gets its shell attributes.  It does not rely on
    any cached information and does not look at the child list.  It can also look down multiple
    levels.
  Arguments:
    pszPathName    - path relative to this item (the root)
    uRelativeDepth - number of filesystem elements in the relative path.
    pdwAttributes  - [IN]  desired shell attribute flags
                     [OUT] desired shell attribute flags that apply to item.
  Return Value:
    S_OK if item exists.
    various errors if it does not.
--*/
{
  HRESULT            hr;
  char               szNameBuffer[MAX_PATH];
  DM_FILE_ATTRIBUTES dmFileAttributes;

  //
  //  Build the wire name, and try to get the file attributes.
  //

  GetWireName(szNameBuffer, m_pszPathName , pszPathName);
  hr = m_pConnection->HrGetFileAttributes(szNameBuffer, &dmFileAttributes);
  if(SUCCEEDED(hr))
  {
    DWORD dwAttributes;
    
    //
    //  Is it a directory?
    //
    if(dmFileAttributes.Attributes&FILE_ATTRIBUTE_DIRECTORY)
    {
        dwAttributes = DIRECTORY_SHELL_ATTRIBUTES;
    } else
    //
    //  Then must be a file.
    //
    {
        dwAttributes = FILE_SHELL_ATTRIBUTES;
    }

    //
    //  OR in SFGAO_READONLY and SFGAO_HIDDEN if applicable.
    //

    if(dmFileAttributes.Attributes&FILE_ATTRIBUTE_READONLY)
    {
        dwAttributes |= SFGAO_READONLY;
    }
    if(dmFileAttributes.Attributes&FILE_ATTRIBUTE_HIDDEN)
    {
        dwAttributes |= (SFGAO_HIDDEN|SFGAO_GHOSTED);
    }

    //
    // Set the out parameter
    //
    *pdwAttributes &= dwAttributes;
  }
  return hr;
}

HRESULT CXboxFileSystemFolder::ReceiveFile(LPSTR pszRelativeRemoteFile, LPSTR szLocalFile)
/*++
  Routine Description:
    Receives a file over the wire and stores it in szLocalFile.  This is designed only
    to be used with selection clone.
  Arguments:
    pszRelativeRemoteFile - the filename to get relative to the parent folder of the
                            selection.  Unless the parent is the selection.
    szLocalFile           - full path of the file to copy to on the local system.
--*/
{
    char szWireName[MAX_PATH];
    _ASSERT(m_fSelectionClone && m_fChildrenValid);
    //
    //  If this has children, then the path is relative to our main path
    //
    if(m_uChildCount)
    {
        GetWireName(szWireName, m_pszPathName, pszRelativeRemoteFile);
    } else
    //
    //  Otherwise, the first part of the relative path is just our name
    //  we should skip it.
    //
    {
        UINT uNameLen = strlen(m_pszName);
        _ASSERT(0==_strnicmp(pszRelativeRemoteFile, m_pszName, uNameLen));
        pszRelativeRemoteFile += uNameLen;
        _ASSERT('\\'==*pszRelativeRemoteFile);
        GetWireName(szWireName, m_pszPathName, ++pszRelativeRemoteFile);
    }
    return m_pConnection->HrReceiveFile(szLocalFile, szWireName);
}

void CXboxFolder::GetTargetWireName(LPSTR pszWireName, LPSTR pszRelativeRemoteFile)
/*++
  Routine Description:
    A helper for all of the file transfer methods where the CXboxFolder is the target.
    It gets the filename of a target file relative to the selection.

    There are three cases:
      1) This object is not a selection clone.  The filename is always relative to the folder.
      2) This object is a selection clone with no children.  Same as 1).
      3) This object is a selection clone with exactly one child.  The filename is relative to
         that only child.

    It is illegal to call this on a selection clone with more than one child, the method ASSERTs.
    
--*/
{
    BOOL fFirstChild = FALSE;
    if(m_fSelectionClone)
    {
        _ASSERT(m_fChildrenValid);
        _ASSERT(m_uChildCount < 2);
        if(1==m_uChildCount)
        {
            fFirstChild=TRUE;
        }
    }

    if(fFirstChild)
    {
        GetWireName(pszWireName, m_pszPathName, m_rgpszChildNames[0]);
        if(pszRelativeRemoteFile)
        {
            UINT uWirePos = strlen(pszWireName) - 1;
            if(pszWireName[uWirePos++]!='\\') pszWireName[uWirePos++] = '\\';
            strcpy(pszWireName+uWirePos, pszRelativeRemoteFile);
        }
    } else
    {
        if(pszRelativeRemoteFile)
        {
            GetWireName(pszWireName, m_pszPathName, pszRelativeRemoteFile);
        } else
        {
            GetWireName(pszWireName, NULL, m_pszPathName);
        }
    }
}

BOOL CXboxFolder::IsXbeFile(LPCSTR pszFileName)
{
    LPCSTR pszExtension = strrchr(pszFileName, '.');
    return pszExtension ? !_stricmp( ++pszExtension, "XBE") : FALSE;
}

LPITEMIDLIST CXboxFolder::GetTargetPidl(LPSTR pszRelativeRemoteFile)
/*++
  Routine Description:
    This pidls a target pidl relative to a selection.  This is called
    for drop and paste operation.  Generally, the target is a selection.
    Depending on how the operation progressed, the selection is either
    this object,  or one (and only one) of its children.  So this selection
    object will have a single child.

    This routine glues the pidl of this object, and its child (if necessary)
    to the pszRelativeRemoteFile (if non-NULL), and generate a full
    absolute pidl.
--*/
{
    BOOL fFirstChild = FALSE;
    UINT uFirstChildLen = 0;
    //If this is a selection clone (generally it will be), we need to make some checks.
    if(m_fSelectionClone)
    {
        //It better be a valid clone with a single child in the selection.
        _ASSERT(m_fChildrenValid);
        _ASSERT(m_uChildCount < 2);
        //If there is a child, the first child must be included in the pidl
        if(1==m_uChildCount)
        {
            fFirstChild=TRUE;
        }
    }

    // For starters, we don't need any extra bytes at the end our pidl
    UINT uExtraAllocation = 0;
    
    // Add in space for the first child if we need it.
    if(fFirstChild)
    {
        uFirstChildLen = strlen(m_rgpszChildNames[0])+1;
        uExtraAllocation = sizeof(USHORT)+uFirstChildLen;
    }
    
    // Compute the pidl size for the relative part.
    LPSTR pszWalk = pszRelativeRemoteFile;
    //For each path element, add a USHORT for the cb of the ItemID.
    while(pszWalk)
    {
        uExtraAllocation += sizeof(USHORT);
        pszWalk = strchr(pszWalk, '\\');
        if(pszWalk) pszWalk++;
    }

    //Add in the lenght of the actual path.  Basically, each '\\' will be replaced
    //by a '\0'.  We already accounted for the cb portions.  Add one for the final
    //'\0' which strlen leaves out.
    if(pszRelativeRemoteFile)
        uExtraAllocation += strlen(pszRelativeRemoteFile)+1;

    // Get our pidl, and ask for the extra allocation at the end that we need.
    LPITEMIDLIST pidlReturn = GetPidl(CPidlUtils::PidlTypeAbsolute, uExtraAllocation);
    if(pidlReturn)
    {
        LPSTR pszPidlWalk;
        LPITEMIDLIST pidlWalk;
        UINT uSimpleNameLength;

        // Walk to the end of our pidl.
        pidlWalk = pidlReturn;
        while(pidlWalk->mkid.cb)
        {
            pidlWalk = AdvancePtr(pidlWalk, pidlWalk->mkid.cb);
        }

        // If we need to include the first child, do it here.
        if(fFirstChild)
        {
            pidlWalk->mkid.cb=uFirstChildLen+sizeof(USHORT);
            memcpy(pidlWalk->mkid.abID, m_rgpszChildNames[0], uFirstChildLen);
            pidlWalk = AdvancePtr(pidlWalk, pidlWalk->mkid.cb);
            pidlWalk->mkid.cb = 0;
        }

        // Now add the relative portion, if there is one.
        if(pszRelativeRemoteFile)
        {
            pszWalk = pszRelativeRemoteFile;
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
    return pidlReturn;
}

LPITEMIDLIST CXboxFolder::GetSourcePidl(LPCSTR pszFolder, LPCSTR pszItem)
{
    UINT uAllocLength = m_uRootPidlLen;
    UINT uLoop;
    
    LPCSTR pszWalk = pszFolder; 
    for(uLoop=0;uLoop<2;uLoop++)
    {
        while(pszWalk)
        {
            uAllocLength += sizeof(USHORT);
            pszWalk = strchr(pszWalk, '\\');
            if(pszWalk) pszWalk++;
        }
        
        pszWalk = pszItem;
    }
    uAllocLength += strlen(pszFolder)+strlen(pszItem)+2;

    LPITEMIDLIST pidlOut = (LPITEMIDLIST) g_pShellMalloc->Alloc(uAllocLength);
    LPITEMIDLIST pidlWalk;
    LPSTR        pszPidlWalk;
    UINT         uSimpleNameLength;
    if(pidlOut)
    {
        pidlWalk = pidlOut;
        memcpy(pidlWalk, m_pidlRoot, m_uRootPidlLen);
        //Go to the end of the root
        while(pidlWalk->mkid.cb)
        {
            pidlWalk = AdvancePtr(pidlWalk, pidlWalk->mkid.cb);
        }

        pszWalk = pszFolder;
        for(uLoop=0;uLoop<2;uLoop++)
        {
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
            pszWalk = pszItem;
        }
        pidlWalk->mkid.cb = 0;
    }
    return pidlOut;
}

void CXboxFolder::GetConsoleName(LPSTR pszConsole)
{
    LPSTR pszParse = m_pszPathName;
    while(*pszParse!='\\' && *pszParse)
    {
        *pszConsole++ = *pszParse++;
    }
    *pszConsole = '\0';
}


UINT CXboxFolder::PathFromPidl(LPCITEMIDLIST pidl, LPSTR pszPathName)
/*++
 Routine Description:
   A utility that parse an Xbox pidl and turns it into a path name.
   It returns the depth of the path.
   
--*/
{
    LPSTR pszName = pszPathName;
    LPCITEMIDLIST pidlWalk = pidl;
    UINT uPathLength; //With NULL termination
    UINT uDepth = 0;

    while(pidlWalk->mkid.cb)
    {
        uDepth++;

        //
        // Safely copy the name
        //
        uPathLength = pidlWalk->mkid.cb-sizeof(USHORT);
        
        //If first character is '?' skip it.
        if('?'==pidl->mkid.abID[0])
        {
            uPathLength--;
            memcpy(pszName, pidlWalk->mkid.abID+1, uPathLength--);
        } else
        {
            memcpy(pszName, pidlWalk->mkid.abID, uPathLength--);
        }
        _ASSERT('\0'==pszName[uPathLength]);
        pszName += uPathLength;

        //
        //  Replace '\0' with a '\\' character
        //
        *pszName++ = '\\';

        //
        //  Move on to next item in pidl
        //
        pidlWalk = AdvancePtr(pidlWalk, pidlWalk->mkid.cb);
    }

    //
    //  Cut the '\\' off the end.  If the depth is zero there
    //  is no '\\' so don't do it.
    //
    if(uDepth) pszName--;
    
    //
    //  NULL terminate the string.
    //
    *pszName =  '\0';

    return uDepth;
}

LPITEMIDLIST
CXboxFolder::PidlFromPath(
    LPCWSTR pwszPathName,
    UINT *puPathLength,
    UINT *puPathDepth,
    BOOL *pfLeadingSlash,
    BOOL *pfTrailingSlash
    )
/*++
 Routine Description:
   Allocates and creates a pidl from a path.  Also computes display
   name length, and path.
   
--*/
{
    LPCWSTR      pwszParse = pwszPathName;
    UINT         uPathLength = 0;
    UINT         uRelativeDepth = 1;
    UINT         uPidlLength;
    LPITEMIDLIST pidl;
    LPITEMIDLIST pidlParse;
    UINT         uItemLength;    
    
    
    _ASSERTE(pfLeadingSlash);
    _ASSERTE(pfTrailingSlash);


    //
    //  Determine if it has a leading slash
    //

    if(L'\\' == *pwszParse)
    {
        uPathLength++;
        pwszParse++;
        *pfLeadingSlash = TRUE;
    } else
    {
        *pfLeadingSlash = FALSE;
    }

    //
    //  First Pass: Determine the length and depth
    //
    
    *pfTrailingSlash = FALSE;
    while(*pwszParse)
    {
        if(L'\\' == *pwszParse)
        {
            uPathLength++;
            pwszParse++;
            if(L'\0'==*pwszParse)
            {
                *pfTrailingSlash = TRUE;
                break;
            }
            uRelativeDepth++;
        }
        uPathLength++;
        pwszParse++;
    }

    //
    //  Restore the parse pointer
    //

    pwszParse = pwszPathName;

    //
    //  Calculate the pidl length = USHORT for each level, length of string.
    //  Then added a character if the path did not have trailing slash.
    //  Finally subtract one if there was a leading slash.
    //

    uPidlLength = ((uRelativeDepth+1)*sizeof(USHORT))+uPathLength;
    if(!*pfTrailingSlash) uPidlLength++;
    if(*pfLeadingSlash)
    {
        uPidlLength--;
        pwszParse++;  //while we are at it, skip the leading slash
    }

    //
    //  Allocate the pidl
    //

    pidl = (LPITEMIDLIST)g_pShellMalloc->Alloc(uPidlLength);
    
    //
    //  Create pidl if we managed to allocate it.
    //

    if(pidl)
    {
        
        //
        //  Second Pass: Copy the names from the path to the pidl
        //
        pidlParse = pidl;
        uItemLength = 0;
        while(*pwszParse)
        {
            if(L'\\' == *pwszParse)
            {
                pwszParse++;
                pidlParse->mkid.abID[uItemLength++] = '\0';
                pidlParse->mkid.cb = (uItemLength + sizeof(USHORT));
                pidlParse = AdvancePtr(pidlParse, pidlParse->mkid.cb);
                uItemLength = 0;
            } else
            {
                pidlParse->mkid.abID[uItemLength++] = (UCHAR)(*pwszParse++);
            }
        }

        //
        //  If there was not a trailing '\\', terminate the last item.
        //
        if(!*pfTrailingSlash)
        {
            pidlParse->mkid.abID[uItemLength++] = '\0';
            pidlParse->mkid.cb = (uItemLength + sizeof(USHORT));
            pidlParse = AdvancePtr(pidlParse, pidlParse->mkid.cb);
        }

        //
        //  Terminate the pidl
        //
        pidlParse->mkid.cb = 0;
    }
    
    //
    //  Fill out the OUT paramteres
    //
    if(puPathLength)
    {
        *puPathLength = uPathLength;
    }
    if(puPathDepth)
    {
        *puPathDepth = uRelativeDepth;
    }

    return pidl;
}

void CXboxFolder::GetWireName(LPSTR pszWireName, LPCSTR pszInitialPath, LPCSTR pszFinalPath)
/*++
  Routine Description:
    Gets the wire name from a full path.

  Arguments:
    pszWireName    - [OUT] buffer to receive wire name.
    pszInitialPath - [IN] initial part of full path.
    pszFinalPath   - [IN] final part of full path.

  Comments:
    Most clients of this method have two portions of the path.  The path from Xbox
    root (starting with the console name) to themselves, and the relative path
    from themselves to some child item (perhaps multiple levels down).  As a convenience,
    this routine accepts the path as two pieces.

    Some clients do not have the path in two pieces.  These should pass NULL for pszInitialPath,
    and pass the entire path in pszFilePath.

--*/
{
    LPCSTR pszNextElement;
    BOOL   fFinalPart;
    _ASSERTE(pszFinalPath);
    
    //
    //  Initialize pszNextElement
    //

    if(pszInitialPath)
    {
        pszNextElement = pszInitialPath;
        fFinalPart = FALSE;
    } else
    {
        pszNextElement = pszFinalPath;
        fFinalPart = TRUE;
    }

    //
    //  The first item is always the console.
    //  we just want to skip that.
    //

    pszNextElement = strchr(pszNextElement, '\\');
    if(!pszNextElement || ('\0' == *(++pszNextElement)))
    {
        _ASSERTE(!fFinalPart);  //Malformed path
        pszNextElement = pszFinalPath;
        fFinalPart = TRUE;   
    }
    
    //
    //  Now process the volume. Always one character.
    //
    *pszWireName++ = *pszNextElement++;
    *pszWireName++ = ':';
    *pszWireName++ = '\\';

    //
    //  If the next character is '\\' then skip it.
    //
    if('\\' == *pszNextElement)
    {
        pszNextElement++;
    }

    //
    //  Handle the case where the paths are split at the volume.
    //
    if('\0' == *pszNextElement)
    {
        if(fFinalPart)
        {
            *pszWireName = '\0';
            return;
        }
        pszNextElement = pszFinalPath;
        fFinalPart = TRUE; 
    }
    
    //
    //  Loop until a break;
    //
    while(TRUE)
    {
        //
        //  Copy a path element.
        //
        while(('\\' != *pszNextElement) && ('\0' != *pszNextElement))
        {
            *pszWireName++ = *pszNextElement++;
        }

        //
        //  Skip '\\' if we see one.
        //

        if('\\' == *pszNextElement)
        {
            pszNextElement++;
        }

        //
        //  Check if we have hit the end of the
        //  current string.

        if('\0' == *pszNextElement)
        {
            //
            //  If this was the final part, we
            //  are done.
            //

            if(fFinalPart) break;

            //
            //  Otherwise, move to the final part.
            //
            pszNextElement = pszFinalPath;
            fFinalPart = TRUE; 
        }

        //
        //  Looks like there is another path element so
        //  add a '\\' as a path delimitor
        //
        *pszWireName++ = '\\';
    }

    *pszWireName = '\0';
    //
    //  All Done :)
    //

    return;
}

LPSTR CXboxFileSystemFolder::GetFileTypeName(PDM_FILE_ATTRIBUTES pdmFileAttributes)
{

    SHFILEINFOA shellFileInfo;
    LPSTR       pszTypeName = NULL;
    
    if( SHGetFileInfoA(
          pdmFileAttributes->Name,
          pdmFileAttributes->Attributes,
          &shellFileInfo,
          sizeof(shellFileInfo),
          SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME 
          )
    ){
      UINT uTypeNameSize = strlen(shellFileInfo.szTypeName)+1;
      pszTypeName = new char[uTypeNameSize];
      if(pszTypeName)
      {
        memcpy(pszTypeName, shellFileInfo.szTypeName, uTypeNameSize);
      }
    }

    return pszTypeName;
}
