/*++

Copyright (c) Microsoft Corporation

Module Name:

    shellfolder.cpp

Abstract:

   Implementation of Interface for CXboxFolder:

       IPersistFolder
       IShellFolder
       IBrowserFrameOptions

Environment:

    Windows 2000 and Later 
    User Mode
    Compiles UNICODE, but uses many ANSI APIs explictly.

Revision History:
    
    06-29-2001 : created

--*/
#include "stdafx.h"

//-----------------------------------------------------------------------------------------------------
//  IPersist\IPersistFolder
//-----------------------------------------------------------------------------------------------------
HRESULT
CXboxFolder::GetClassID(
    LPCLSID lpClassID
    )
/*++
  Routine Description:
    Should return the CLSID of our IShellFolder implementation.
    This is probably used by clients to make short-cuts.
--*/
{
    if(lpClassID)
    {
        *lpClassID = __uuidof(XboxFolder);
        return S_OK;
    }
    return E_FAIL;
}


HRESULT
CXboxFolder::Initialize(
    LPCITEMIDLIST pidl
    )
/*++
  Routine Description:
    Only CXboxRoot supports Initialize so we return E_NOTIMPL and override this
    in CXboxRoot.
--*/
{
    _ASSERTE("IPersistFolder::Initialize on non-root item" && FALSE);
    return E_NOTIMPL;
}

HRESULT
CXboxRoot::Initialize(
    LPCITEMIDLIST pidlRoot
    )
/*++
  Routine Description:
    Implements IPersistFolder::Initialize for CXboxRoot.
    
    The hard part is getting the name of Xbox root, since the
    name can be changed by the user.  We use SHGetFileInfo

  Arguments:
    pidlRoot - pidl of Xbox root.
  
--*/
{
    HRESULT hr;
    SHFILEINFOA shellFileInfo;
    ENTER_SPEW
    DEBUG_DUMP_PIDL(pidlRoot);

    //
    //  SHGetFileInfoA should be able to get our root name from the registry.
    //
    
    if(SHGetFileInfoA((LPSTR)pidlRoot, 0, &shellFileInfo, sizeof(shellFileInfo), SHGFI_PIDL|SHGFI_DISPLAYNAME))
    {

        //
        //  Call the base initialization.
        //

        hr = InitBaseClass(
              shellFileInfo.szDisplayName,
              ROOT_SHELL_ATTRIBUTES,
              pidlRoot);
        m_uPathDepth = 0;
    } else
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }
    
    return hr;
}


//-----------------------------------------------------------------------------------------------------
//  IShellFolder
//-----------------------------------------------------------------------------------------------------

HRESULT CXboxFolder::BindToObject(LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv){return E_NOTIMPL;}

HRESULT CXboxRoot::BindToObject(LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv)
/*++
  Routine Description:
  
    Implements IShellFolder::BindToObject.  Gets an IShellFolder * for any in the namespace.
    This may be an immediate child or any number of levels down.

  Arguments:
  
    pidl - pidl of child to get.
    pbc  - a binding context, we just ignore this.
    riid - interface to get.
    ppv  - out parameter for interface.

  Result:
    
    S_OK on success.
    E_INVALIDARG - if the pidl does not represent a valid child.
    E_NOINTERFACE - if the desired interface does not exist.
    
  Comments:
    
    The pidl must be relative.  The first SHITEM ID will always be the machine name.
    We can us this to a connection interface.

    PathFromPidl will get the whole path from the pidl AND tell us the depth (i.e.
    the count of SHITEMIDs in the pidl).

    If Depth is 1, we need a console.
    If Depth is 2, we need a volume.
    If Depth is greater than 2, we need a directory.

    The silly shell tries to bind to the Add Xbox wizard, when resolving a short-cut.  This despite the
    fact that it doesn't have attributes that might suggest that this is possible.  So we need to
    detect this case and simply fail.  Then it tries to open it without binding.

--*/
{
    char  szPathName[MAX_XBOX_PATH];
    UINT  uDepth;
    IXboxConnection *pConnection;
    HRESULT hr;

    ENTER_SPEW
    DEBUG_DUMP_IID(riid);
    ATLTRACE("\n");
    DEBUG_DUMP_PIDL(pidl);

    
    uDepth = PathFromPidl(pidl, szPathName);
    
    //
    //  It is not legal to send a NULL pidl.
    //
    if(0==uDepth)
    {
        _ASSERTE(FALSE);
        return E_INVALIDARG;
    }

    if('?' == *pidl->mkid.abID)
    {
        return E_FAIL;
    }

    //
    //  Get a connection interface
    //

    hr = Utils::GetXboxConnection((LPSTR)pidl->mkid.abID, &pConnection);
    if(FAILED(hr))
    {
        if(pbc)
        {
            char szError[60];
            FormatUtils::XboxErrorString(hr, szError, sizeof(szError));

            //
            //  BUGBUG: No parent hwnd for this message box (chrispi fixed a build break here)
            //

            WindowUtils::MessageBoxResource(NULL, IDS_ERROR_OPENNING_FOLDER, IDS_ERROR_OPENNING_FOLDER_CAPTION, MB_OK|MB_ICONSTOP, szPathName, szError);
        }
        RETURN(hr);
    }

    if(1==uDepth)
    {
        hr = CXboxConsole::Create(szPathName, m_pidlRoot, pConnection, riid, ppv);
    } else if(2==uDepth)
    {
        hr = CXboxVolume::Create(szPathName, m_pidlRoot, pConnection, riid, ppv);
    } else
    {
        hr = CXboxDirectory::Create(szPathName, m_pidlRoot, pConnection, riid, ppv);
    }
    RETURN(hr);
}

HRESULT CXboxConsole::BindToObject(LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv)
/*++
  Routine Description:
  
    Same as CXboxRoot::BindToObject with a couple of differences in the implementation:
    
    1) We already have a connection it is one of our members.

    2) We need to concatanate the path with our own.

    3) If Depth is 1, we need a Volume.
       If Depth is greater than 1, we need a Directory.
--*/
{
    char  szPathName[MAX_XBOX_PATH];
    LPSTR pszPathName;
    UINT  uDepth;
    IXboxConnection *pConnection;
    HRESULT hr;

    ENTER_SPEW
    DEBUG_DUMP_IID(riid);
    ATLTRACE("\n");
    DEBUG_DUMP_PIDL(pidl);

    //
    //  Initialize the path buffer with the path to use.
    //
    GetPath(szPathName);
    pszPathName = szPathName + GetPathLen();

    //
    //  Append a '\\'
    //
    *pszPathName++ = '\\';

    //
    //  Append the path from the relative pidl
    //
    uDepth = PathFromPidl(pidl, pszPathName);
    
    //
    //  It is not legal to send a NULL pidl.
    //
    if(0==uDepth)
    {
        _ASSERTE(FALSE);
        RETURN(E_INVALIDARG);
    }

    //
    //  Create the correct object depending on the depth.
    //
    if(1==uDepth)
    {
        hr = CXboxVolume::Create(szPathName, m_pidlRoot, m_pConnection, riid, ppv);
    } else
    {
        hr = CXboxDirectory::Create(szPathName, m_pidlRoot, m_pConnection, riid, ppv);
    }
    RETURN(hr);
}

HRESULT CXboxFileSystemFolder::BindToObject(LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv)
/*++
  Routine Description:
  
    Same as CXboxConsole::BindToObject with a couple of differences in the implementation:
    
    Always Create a directory.
--*/
{
    char  szPathName[MAX_XBOX_PATH];
    LPSTR pszPathName;
    UINT  uDepth;
    IXboxConnection *pConnection;
    HRESULT hr;
    
    ENTER_SPEW
    DEBUG_DUMP_IID(riid);
    ATLTRACE("\n");
    DEBUG_DUMP_PIDL(pidl);

    //
    //  Initialize the path buffer with the path to use.
    //
    GetPath(szPathName);
    pszPathName = szPathName + GetPathLen();

    //
    //  Append a '\\'
    //
    *pszPathName++ = '\\';

    //
    //  Append the path from the relative pidl
    //
    uDepth = PathFromPidl(pidl, pszPathName);
    
    //
    //  It is not legal to send a NULL pidl.
    //
    if(0==uDepth)
    {
        _ASSERTE(FALSE);
        RETURN(E_INVALIDARG);
    }

    hr = CXboxDirectory::Create(szPathName, m_pidlRoot, m_pConnection, riid, ppv);
    
    RETURN(hr);
}


HRESULT CXboxFolder::BindToStorage(LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, void **ppv)
/*++
  Routine Description:
    We don't implement IStorage interfaces for each of our items, so we do not support
    this at the present time.  I am not 100% what this storage would like.
--*/
{
    ENTER_SPEW
    return E_NOTIMPL;
}

HRESULT CXboxFolder::CompareIDs(LPARAM lParam, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
/*++
  Routine Description:
   Implements IShellFolder::CompareIDs.  Compares two pidls for the purpose of sorting or
   for determining identity.

  Arguments:
   lParam  - Use SHCIDS_COLUMNMASK (lower sixteen bits) to get column IDs.
             SCHIDS_ALLFIELDS masks a bit in the high word.
   pidl1   - first item id.
   pidl2   - second item id.

  Return Value:
    S_EQUAL   - if they refer to the same object.
    S_GREATER - if pidl1 is greater than pidl2
    S_LESS    - if pidl1 is before pidl2.
    
    Failure codes (E_EQUAL, E_GREATER, E_LESS) in some situations, see caveats.

 Caveats:

   This appears to be a simple idea, but I found this to be one of the most deceptively
   sophisticated methods in the shell API.  Even small bugs cause really strange behavior.
   
   All sorts of layers use this to determine pidl identity.  Within the shell only the owning
   namespace extension can properly say whether two pidls refer to the same object.  This is how
   you ask.

   It is not acceptable to fail an identity check just because the pidl may no longer be valid
   (i.e. the xbox is not reachable, or the item has been deleted).  This is especially true
   as the shell's CDefView calls this to find the correct item to remove from the view after
   it is notified of an item being deleted.

   Note that all Xbox SHITEMID's are just null terminated upper-case strings containing the
   item name, with cb set correctly for pidls.  To sort by something other than name we must
   access the item over the wire (or from a member variable if we recently cached the information).

   So here are the rules:

   1) Pidls are identical if a case insensitive compare of each SHITEMID is 0.  We do not and
      must not talk over the wire for this!  Return 0 immediately.
   2) If pidls are identical to a point, but then one is shorter than the other, the shorter
      one comes first.
   3) If pidls are different at any SHITEMID besides the last one (for both pidls), then the
      the alphabetically earlier one is first.
   4) We only support sorting by anything other than name if each pidl is a simple pid.  In this
      case we call CompareItemIDs, which considers the column ID.  Otherwise, we just compare
      by name only.**
   5) If CompareItemIDs fails (an item doesn't exist or cannot be reached), we still cannot fail
      if SHCIDS_ALLFIELDS is set. In this case, an invalid item appears after a valid item.  Two
      invalid items are sorted by name.

   ** Whether this is sufficient remains to be seen.  The difficulty is that it is really inefficient
   to compare by criteria other than name, if we need to look down more than one level.  Additionally,
   the code to do the comparison would be completely different than anything that exists now.

--*/
{
    HRESULT hr;
    LPCITEMIDLIST pidl1Temp = pidl1;
    LPCITEMIDLIST pidl2Temp = pidl2;
    int iNameCmp;
    USHORT cbTotal=0;
    BOOL fCannotFail = (0==lParam) ? TRUE : FALSE;
    
    ATLTRACE("CompareIDs: %d\n", lParam);
    DEBUG_DUMP_PIDL(pidl1);
    DEBUG_DUMP_PIDL(pidl2);

    //
    // Loop over all SHITEMID's
    //
    while (pidl1Temp->mkid.cb && pidl2Temp->mkid.cb)
    {
        //
        //  Case-insensitve compare one level of SHITEMIDs
        //

        iNameCmp = _stricmp((char *)pidl1Temp->mkid.abID, (char *)pidl2Temp->mkid.abID);

        //
        //  If they were different, we have stuff to do.
        //

        if (iNameCmp)
        {
            // Check if this was the end of the pidl.
            //
            LPCITEMIDLIST pidl1End = AdvancePtr(pidl1Temp, pidl1Temp->mkid.cb);
            LPCITEMIDLIST pidl2End = AdvancePtr(pidl2Temp, pidl2Temp->mkid.cb);

            if (pidl1End->mkid.cb || pidl2End->mkid.cb)
            //
            //  Not the end so just return the result of the last string compare.
            //
            { 
                hr = ResultFromCompare(iNameCmp);
            }
            else
            //
            //  It was the end of the pidl, call the proper CompareItemIDs
            //
            {
                //
                //  If the common parent is not us, we do not support support
                //  anything but name based ordering.
                //
                if(cbTotal)
                {

                    ATLTRACE("Inefficient CompareItemIDs.");
                    _ASSERTE(0==(lParam&SHCIDS_COLUMNMASK));
                    hr = ResultFromCompare(iNameCmp);

                } else
                //
                //  The common parent is us.  That is good, that we probably won't have to
                //  hit the wire.  (When the sort criteria was set, we refreshed our children).
                //
                {
                    hr = CompareItemIDs(lParam&SHCIDS_COLUMNMASK, iNameCmp, &pidl1Temp->mkid, &pidl2Temp->mkid);
                }
                
                //
                // Check if the common parent failed the comparison, and if we cannot fail.
                //

                if(FAILED(hr) && fCannotFail)
                {
                    //Get rid of the severity and facility, and just keep the result
                    //stored by CompareItemIDs
                    hr = LOWORD(hr); 
                }
            }
            RETURN(hr);
        } else
        {
          //
          // Tracking the pidl length as we go will help, when
          // we need to get a CXboxFolder.
          //
          cbTotal += pidl1Temp->mkid.cb;
        }
        pidl1Temp = AdvancePtr(pidl1Temp, pidl1Temp->mkid.cb);
        pidl2Temp = AdvancePtr(pidl2Temp, pidl2Temp->mkid.cb);
    }

    // If we're here, either:
    //  1)  the pidls are identical to the end of one, but the other has a greater depth
    //  2)  the pidls are identical
    //
    if (pidl1Temp->mkid.cb)
    {
        _ASSERTE(!pidl2Temp->mkid.cb); // Sanity check.
        hr = S_GREATER;
    }
    else if (pidl2Temp->mkid.cb)
    {
        _ASSERTE(!pidl1Temp->mkid.cb); // Sanity check.
        hr = S_LESS;
    }
    else
    {
        hr = S_EQUAL;
    }
    RETURN(hr);
}


HRESULT CXboxFolder::CreateViewObject(HWND hwnd, REFIID riid, void **ppv)
/*++
  Routine Description:
    Implements IShellFolder::CreateViewObject.

    This routine has two purposes really.

    1) Get a view (IShellView or IShellView2).  This we handle here.
    
    2) It can also ask for anything that GetUIObjectOf can ask for.
       This we handle by calling GetUIObjectOf with cidl set to 0, and
       apidl set to NULL.  See GetUIObjectOf comments to understand
       why this works this way.
--*/
{
   HRESULT hr;
   ENTER_SPEW
   DEBUG_DUMP_IID(riid);

   //
   //   If it is not looking for a shell view, just defer
   //   to GetUIObjectOf.
   //
   if( (IID_IShellView==riid) || (IID_IShellView2==riid) )
   {
       //
       //   Instantiate our view callback.
       //
       return CXboxViewCB::CreateShellView(this, hwnd, riid, ppv);
   }
      
   return GetUIObjectOf(hwnd, 0, NULL, riid, NULL, ppv);
}

HRESULT CXboxFolder::EnumObjects(HWND hwnd, DWORD grfFlags, IEnumIDList **ppEnumIDList)
/*++
  Routine Description:
    Implements IShellFolder::EnumObjects.
    
    We can implement this all here since all we need are the simple pidls and the
    shell attributes. 
--*/
{
    HRESULT hr;
    CComObject<CEnumIdList> *pEnumerator = NULL;
    UINT uChildIndex, uFilteredChildCount;
    DWORD dwXorMask, dwAndMask;
    
    ENTER_SPEW

    //
    // Null the out parameter in case we need to fail.
    //
    *ppEnumIDList = NULL;  

    //  BUILD FILTER MASK FOR CHILDREN
    //
    //  The efficient filter based on bits is to create an
    //  XOR mask and an AND mask.  If the result is zero the
    //  the item is accepted.
    //
    //  With this scheme any bit can force a reject, but every
    //  bit must accept for the item to be accepted.
    //
    //  IN  ^ XORMASK & ANDMASK = OUT  | Comment
    //================================================================================
    //   X  |    X         0    =  0   | If ANDMASK clear, always accept
    //--------------------------------------------------------------------------------  
    //   1  |    1         1    =  0   | ANDMASK set, XORMASK set then
    //   0  |    1         1    =  1   | Accept if IN is set
    //--------------------------------------------------------------------------------  
    //   1  |    0         1    =  1   | ANDMASK set, XORMASK clear then
    //   0  |    0         1    =  0   | Accept if IN is clear
    //--------------------------------------------------------------------------------  
    //
    dwXorMask = 0;
    dwAndMask = 0;
    if(!(grfFlags&SHCONTF_INCLUDEHIDDEN)) 
    {
        dwAndMask |= SFGAO_HIDDEN;
    }

    //
    //  Switch on the state of SHCONTF_NONFOLDERS and SHCONTF_FOLDERS flag.
    //

    DWORD dwFolderBits = (SHCONTF_NONFOLDERS|SHCONTF_FOLDERS)&grfFlags;
    switch(dwFolderBits)
    {
        case SHCONTF_FOLDERS:
          dwXorMask |= SFGAO_FOLDER;
          //fall through to set the AND mask
        case SHCONTF_NONFOLDERS:
          dwAndMask |= SFGAO_FOLDER;
          break;
        case 0:
          // Someone is asking for neither folders nor non-folders.
          // What is the point of even calling?  The logical response
          // is to enumerate nothing, but give them everything anyway.
          //
          // Fall through to the case of both, after asserting.
          _ASSERTE(FALSE);
        case (SHCONTF_NONFOLDERS|SHCONTF_FOLDERS):
          break;
    };

    //
    // Refresh our child list.
    //

    hr = RefreshChildren();
    if(FAILED(hr))
    {
        return hr;
    }
    
    //
    //  Allocate array of PIDLs for all our children, even though
    //  some might not make it through the filter, we shouldn't
    //  over allocate by much.
    //

    LPITEMIDLIST *enumPidlArray = new LPITEMIDLIST[m_uChildCount];
    if(NULL == enumPidlArray) return E_OUTOFMEMORY;
    memset(enumPidlArray, 0, m_uChildCount*sizeof(LPITEMIDLIST));

    //
    //  Walk our children.  Filter them based on grfFlags and
    //  add them to enumPidlArray.
    //

    for(uChildIndex = 0, uFilteredChildCount = 0; uChildIndex < m_uChildCount; uChildIndex++)
    {
        if(0 == ((m_rgulChildShellAttributes[uChildIndex]^dwXorMask)&dwAndMask))
        {
            enumPidlArray[uFilteredChildCount] = GetChildPidl(uChildIndex, CPidlUtils::PidlTypeSimple);
            if(enumPidlArray[uFilteredChildCount])
            {
                uFilteredChildCount++;
            }
        }
    }
    
    //
    //  Create an enumerator object
    //
    hr = CComObject<CEnumIdList>::CreateInstance(&pEnumerator);

    //
    //  Initialize the enumerator object with our child array
    //

    if(SUCCEEDED(hr))
    {
        //
        //  We know that these next two operations cannot fail based on the implemenation
        //  of ATL's enumerator.
        //
        hr = pEnumerator->Init(enumPidlArray, enumPidlArray+uFilteredChildCount, NULL, AtlFlagTakeOwnership);
        _ASSERTE(SUCCEEDED(hr));
        
        hr = pEnumerator->QueryInterface(IID_IEnumIDList, (void **)ppEnumIDList);
        _ASSERTE(SUCCEEDED(hr));

    } else
    {
      //
      //  If we could not create an enumerator, we need to free
      //  the array ourselves.
      //

      for(uChildIndex = 0; uChildIndex < uFilteredChildCount; uChildIndex++)
      {
        g_pShellMalloc->Free(enumPidlArray[uChildIndex]);
      }
    }
       
    return hr;
}


HRESULT CXboxFolder::GetAttributesOf(UINT cidl, LPCITEMIDLIST * apidl, ULONG * drgfInOut)
/*++
  Routine Description:
    Implements IShellFolder::GetAttributesOf.

    Gets the shell attributes of a subset of children.

    1) Make sure we have a child list.
    
    2) Loop over all the simple pidls.

    3) For each, get our child index, then and its attributes with *drgfInOut.

  Arguments:
    cidl      - count of SHITEMIDs in apild.
    apidl     - array of cidl simple pidls (i.e. a single SHITEMID per pidl).
    drgfInOut - IN  = attributes the caller wants.
                OUT = union (bitwise AND) of attributes wanted by caller and supported by all the specified
                      children.
  Return Value:
   S_OK   - on success
   E_FAIL - if any of the pidls are not valid items

  Remarks:
   Sometimes cidl is 0, this is not documented.  In this case, we just return our own attributes.

--*/             
{
    UINT uItemIdIndex;
    UINT uChildIndex;
    HRESULT hr;

    ENTER_SPEW

    //
    //  Handle the query self case.
    //
    if(0==cidl)
    {
        *drgfInOut &= m_ulShellAttributes;
        *drgfInOut &= ~SFGAO_CANLINK;
        RETURN(S_OK);
    }

    if(*drgfInOut&SFGAO_VALIDATE)
    {
        m_fChildrenValid = FALSE;
    }
    hr = RefreshChildren();
    if(FAILED(hr))
    {
        return hr;
    }

    for(uItemIdIndex = 0; uItemIdIndex < cidl; uItemIdIndex++)
    {
        //
        //  If this is our pidl, apidl[uItemIdIndex]->mkid.abID is
        //  then name of the child.  If it is not our child, it is illegal, but
        //  still not that dangerous, we won't run over the end of the buffer
        //  or anything as the pidl itself is double NULL terminated.
        //

        hr = GetChildIndex((LPSTR)apidl[uItemIdIndex]->mkid.abID, &uChildIndex);
 
        //
        //  bitwise-AND in the flags.
        //

        if(FAILED(hr))
        {
            *drgfInOut = 0;
            return hr;
        } else
        {
            *drgfInOut &= m_rgulChildShellAttributes[uChildIndex];
        }
    }

    return S_OK;
}

HRESULT CXboxFolder::GetDisplayNameOf(LPCITEMIDLIST pidl, DWORD uFlags, STRRET *psr)
/*++
  Routine Description:
    Implements IShellFolder::GetDisplayNameOf.  Gets the display name of a child item.

  Arguments:
    pidl   - Relative pidl of item.
    uFlags - one or more of [SHGDN_NORMAL|SHGDN_INFOLDER] & [SHGDN_FOREDITING|SHGDN_FORADDRESSBAR|SHGDN_FORPARSING]
    psr    - reurn string value.


  Return Value:
   S_OK   - on success
   E_FAIL - if any of the pidls are not valid items

 Comments:

   ARRGH:  The documents big time suck on what to do with the flags!  The set of five example cases in the SHGNO
   entry of the docs is an extended subset of what callers will actually do.  In other words, the five examples
   are marginally useful.  Basically, you can refer to the docs with following ammendments:  (these cases are exact
   matches for uFlags, other combinations are of course possible)

  SHGFN_NORMAL - This name is definately what ends up as short-cut names.  It seems to be called for other purposes too, but
  I never see this UI show up, so who knows.  The docs say the path should be relative to the desktop.  Not really, rather it
  
  SHGDN_FORADDRESSBAR - The shell sends this to get the title of the window, when the user has NOT turned on the full-path name in title bar option.

  SHGDN_FORADDRESSBAR|SHGDN_FORPARSING - The shell sends this to get the title of the window, when the user has turned on the full-path name in title bar option.
  
--*/             
{
    LPCITEMIDLIST pidlWalk;
    UINT          uCharacterCount;
    LPCSTR        pszRootName;
    UINT          uRootLength;
    UINT          uParentLength;
    UINT          uChildLength;
    UINT          uTotalLength;
    SHFILEINFOA   shellFileInfo;
    char          szPidlPath[MAX_XBOX_PATH];

    ENTER_SPEW
    DEBUG_DUMP_PIDL(pidl);

    pidlWalk = AdvancePtr(pidl, pidl->mkid.cb);

    //  The normal display for the root, or for a console, is just the simple
    //  short name as return by SHGDN_INFOLDER.
    if(SHGDN_NORMAL==uFlags && (m_uPathDepth<1))
    {
        uFlags = SHGDN_INFOLDER;
    }

    //
    //  The condition was originally SHGDN_INFOLDER, however, this was not sufficient.
    //  The code that updates the title always passes SHGDN_FORADDRESSBAR, if it wants the full
    //  path it also sets SHGDN_FORPARSING.  The documentation on the *actual* use of the flags
    //  by the shell is wrong, but by now we expect nothing more.
    //
    if(uFlags&SHGDN_INFOLDER || ((uFlags&SHGDN_FORADDRESSBAR) && !(SHGDN_FORPARSING&uFlags)))
    {
        //
        //  We are guaranteed to be a simple pidl, if the
        //  caller plays by the rules.  Make sure the caller
        //  is playing by the rules!
        //

        if(pidlWalk->mkid.cb)
        {
            return E_FAIL;
        }

        //
        //  The name is simply pidl->mkid.abID, just
        //  after cb, which is of size sizeof(USHORT).
        //
        
        psr->uType = STRRET_OFFSET;
        psr->uOffset = sizeof(USHORT);
        
       
        //If first character is '?' skip it.
        if('?'==pidl->mkid.abID[0])
        {
            psr->uOffset += 1;
        }
        return S_OK;
    }
    
    // If we are normal, then we have a nicely formated display.
    if(SHGDN_NORMAL==uFlags)
    {   
        //The name is <item> on <machine>.
        char *pszItem = (LPSTR)pidl->mkid.abID;
        char szConsoleName[MAX_XBOX_PATH];
        //Handle Add Xbox wizard
        if('?'==*pszItem) pszItem++;
        GetConsoleName(szConsoleName);
        psr->uType = STRRET_CSTR;
        WindowUtils::rsprintf(psr->cStr, IDS_NORMAL_NAME_FORMAT, pszItem, szConsoleName);
        return S_OK;
    }

    // Get the name of our root.
    if(SHGetFileInfoA((LPSTR)m_pidlRoot, 0, &shellFileInfo, sizeof(shellFileInfo), SHGFI_PIDL|SHGFI_DISPLAYNAME))
    {
        pszRootName = shellFileInfo.szDisplayName;
        uRootLength = 
        uTotalLength = strlen(shellFileInfo.szDisplayName);
        uTotalLength += 1; // the one is for '\\'
    } else
    {
        RETURN(HRESULT_FROM_WIN32(GetLastError()));
    }

    //
    // If we are not the root, add in our path length
    //
    if(0 != m_uPathDepth)
    {
        uParentLength = GetPathLen(); 
        uTotalLength += uParentLength + 1;  // the one is for '\\'
    } else
    {
        uParentLength = 0;
    }
    
    //
    // Path from pidl.
    //
    PathFromPidl(pidl, szPidlPath);
    uChildLength = strlen(szPidlPath)+1;  // the one is for '\0'
    uTotalLength += uChildLength;

    //
    //  Now we have easy access to three pieces:
    //  the root name, our name (if we are not
    //  the root), and the pidl name relative to us.
    //


    //
    //  The total name may be longer than MAX_PATH, so be careful
    //  with psr

    if(MAX_PATH >= uTotalLength)
    {
        LPSTR pszPosition;

        //
        //  Use cStr, copy one element at a time.
        //

        psr->uType = STRRET_CSTR;
        pszPosition = psr->cStr;

        //
        //  The root name, use memcpy, since we
        //  already know the length.
        //

        memcpy(pszPosition, pszRootName, uRootLength);
        pszPosition += uRootLength;

        //
        //  Add a '\\'
        //

        *pszPosition++ = '\\';

        //
        //  If we are not the root, add the parent 
        //  path (that would be this object's path)
        //

        if(uParentLength)
        {
            GetPath(pszPosition);
            pszPosition += uParentLength;
            *pszPosition++ = '\\';
        }

        //
        //  Add the path we got from the pidl.
        //

        memcpy(pszPosition, szPidlPath, uChildLength);
        ATLTRACE("DisplayName = %s\n", psr->cStr);



    } else

    //
    //  Too long STRRET_CSTR, use STRRET_WSTR.
    //
    {
        //
        //  Allocate a buffer for the string with the shell's IMalloc
        //
        LPWSTR pwszPosition = (LPOLESTR)g_pShellMalloc->Alloc(uTotalLength*sizeof(WCHAR));
        if(pwszPosition)
        {
            //
            //  Exactly the same as the cStr case, except
            //  we convert to WCHAR as we copy.
            //

            psr->uType = STRRET_WSTR;
            psr->pOleStr = pwszPosition;
            Utils::CopyAtoW(pwszPosition, pszRootName);
            pwszPosition += uRootLength;
            *pwszPosition++ = L'\\';
            if(uParentLength)
            {
                GetPath(pwszPosition);
                pwszPosition += uParentLength*sizeof(WCHAR);
                *pwszPosition++ = L'\\';
            }
            Utils::CopyAtoW(pwszPosition, szPidlPath);
            ATLTRACE("DisplayName = %ls\n", psr->pOleStr);

        } else
        {
            //
            //  Couldn't allocate buffer/
            //

            return E_OUTOFMEMORY;
        }
    }

    

    return S_OK;
}


HRESULT CXboxFolder::GetUIObjectOf(HWND hwnd, UINT cidl, LPCITEMIDLIST * apidl, REFIID riid, UINT * prgfInOut, void **ppv)
/*++
  Routine Description:
    Implements IShellFolder::GetUIObjectOf.

    Can serve up the following object types:

       Interface     |   C++ Class
       ==========================
       IContextMenu  |  CXboxMenu 
       IDataObject   |  CXboxDataObject
       IDropTarget   |  CXboxDropTarget
       IExtractIcon  |  CXboxIcon

   Arguments:
    hwnd      - if any UI needs to be presented.
    cidl      - number of items items in apidl.
    apidl     - array of child pidls for which to get an object.
    riid      - interface ID desired for object.
    prgfInOut - reserved.
    ppv       - receives pointer.

--*/
{
    HRESULT hr = E_OUTOFMEMORY;

    ENTER_SPEW
    DEBUG_DUMP_IID(riid);

    if(cidl)
    {
        //
        //  If asking for child's object, we need to refresh the list
        //
        hr = RefreshChildren();
        if(FAILED(hr))
        {
            return hr;
        }
    }
    
    //
    //  NULL ppv in case we fail.
    //

    *ppv = NULL;

    //
    //  IExtractIcon is a special case, handle it first.
    //

    if(IID_IExtractIcon == riid)
    {
        CComObject<CXboxExtractIcon> *pExtractIcon = new CComObject<CXboxExtractIcon>;
        if(pExtractIcon)
        {
            VisitThese(cidl, apidl, pExtractIcon, IXboxVisitor::FlagContinue);
            hr = pExtractIcon->GetHResult();
            if(SUCCEEDED(hr))
            {
                hr = pExtractIcon->QueryInterface(IID_IExtractIcon, ppv);
                _ASSERTE(SUCCEEDED(hr));
            } else
            {
                delete pExtractIcon;
            }
        }
        RETURN(hr);
    }

    //
    //  Create the desired object type
    //
    hr = E_NOINTERFACE;

    if(IID_IContextMenu == riid)
    {
        hr = CXboxMenu::Create(cidl, apidl, this, (IContextMenu **)ppv);
    } else if(IID_IDataObject == riid)
    {
        hr = CXboxDataObject::Create(cidl, apidl, this, (IDataObject **)ppv);
    } else if (IID_IDropTarget == riid)
    {
        hr = CXboxDropTarget::Create(hwnd, cidl, apidl, this, (IDropTarget **)ppv);
    } else
    {
        hr = E_NOINTERFACE;
    }
    
    RETURN(hr);
}


HRESULT CXboxFolder::ParseDisplayName(HWND hwnd, LPBC pbc, LPOLESTR pszDisplayName, ULONG *pchEaten, LPITEMIDLIST *ppidl, ULONG *pdwAttributes)
/*++
  Routine Description:
    Implements IShellFolder::ParseDisplayName

    Get a pidl from a display name.  The pidl must be child (not necessarily immediate) of this object.

  Arguments:

    hwnd           - If we decide to display some UI.
    pbc            - a bind context (we ignore it)
    pszDisplayName - a name to parse (UNICODE)
    pchEaten       - [OUT OPTIONAL] how many characters did we eat.
    ppidl          - [OUT] the pidl that matches the display na,e
    pdwAttributes  - [IN OUT OPTIONAL] the desired attributes.

  Return Value:

    On Success - S_OK.
    On Failure - OLE defined error.

  Comments:

    if(pdwAttributes && ((*pdwAttributes)&SFGAO_VALIDATE)) we must make sure the item exists.  If it does not
    we must return an error.  In general, if pdwAttributes, we need to get the attributes.  If the attributes
    selected are determined by class only (i.e. NOT SFGAO_READONLY, SFGAO_HIDDEN, or in most cases SFGAO_FOLDER)
    we can often answer with a canned response.  Otherwise, we have to go over the wire anyway, which validates
    
    We always parse to the end, so *pchEaten is always _wcslen(pszDisplayName) if pchEaten.
--*/
{
    HRESULT  hr = S_OK;
    BOOL     fLeadingSlash;
    BOOL     fTrailingSlash;
    UINT     uDisplayNameLen;
    UINT     uRelativeDepth;
    BOOL     fStrippedProtocol = FALSE;
    
    ENTER_SPEW
    ATLTRACE("DisplayName = %ls", pszDisplayName);

    //
    //  Handle the case were this is the xbox:// protocol
    //
    if(0==wcsncmp(pszDisplayName, L"xbox://", 7))
    {
        pszDisplayName += 7;
        LPWSTR pwszParse = pszDisplayName;
        while(*pwszParse)
        {
            // convert '/' to '\\'
            if(L'/'==*pwszParse) *pwszParse = L'\\';
            pwszParse++;
        }
        if(L'\\' == *(pwszParse-1)) *(pwszParse-1) = L'\0';  //strip '/' or '\\' at the end.
        fStrippedProtocol = TRUE;
    }
    
    //
    //  Get a pidl
    //

    *ppidl =  PidlFromPath(
                   pszDisplayName,
                   &uDisplayNameLen,
                   &uRelativeDepth,
                   &fLeadingSlash,
                   &fTrailingSlash
                   );
    
    //
    //  If we didn't get a pidl there is one reason,
    //  out of memory.
    //

    if(!*ppidl)
    {
        return E_OUTOFMEMORY;
    }

    //
    //  We really don't expect to see pidls with a leading slash.
    //

    _ASSERTE(FALSE == fLeadingSlash);
    

    //
    //  We ate everything. 
    //

    if(pchEaten)
    {
        *pchEaten = uDisplayNameLen;
        if(fStrippedProtocol) *pchEaten += 7;
    }

    //
    //  If attributes are desired, we need to the parent
    //  or the item.
    //

    if(pdwAttributes)
    {
        LPSTR pszPathName = (LPSTR)_alloca(uDisplayNameLen+1);
        Utils::CopyWtoA(pszPathName, fLeadingSlash ? pszDisplayName+1 : pszDisplayName);

        //
        //  If the relative depth is one, it is an immediate child.
        //  As long as SFGAO_VALIDATE is not requested, we can often
        //  just fill out the attributes from our child cache.
        //

        if((1 == uRelativeDepth) && m_fChildrenValid)
        {
            if(!(*pdwAttributes&SFGAO_VALIDATE))
            {
                UINT uChildIndex;
                hr = GetChildIndex(pszPathName, &uChildIndex);
                if(SUCCEEDED(hr))
                {
                    *pdwAttributes &= m_rgulChildShellAttributes[uChildIndex];

                    //
                    //  If the path had a trailing slash and SFGAO_FOLDER
                    //  is not set, it is an illegal path.
                    //

                    if(fTrailingSlash && !(*pdwAttributes&SFGAO_FOLDER))
                    {
                        g_pShellMalloc->Free(*ppidl);
                        *ppidl = NULL;
                        _ASSERT(FALSE);//For early debug purposes.
                        return E_FAIL;
                    }
                    ATLTRACE("Pidl Not verified: ");
                    DEBUG_DUMP_PIDL(*ppidl);
                    return S_OK;
                }
            }
        }
        
        //
        //  We weren't able to get the attributes the quick and dirty way
        //  (either SFGAO_VALIDATE or we had now cached attributes).  So now
        //  force a wire transaction.
        //
        
        hr = ValidateItem(pszPathName, uRelativeDepth, pdwAttributes);
        if(FAILED(hr))
        {
            g_pShellMalloc->Free(*ppidl);
            *ppidl = NULL; 
        }
    }
    if(SUCCEEDED(hr))
    {
        ATLTRACE("Pidl verified: ");
        DEBUG_DUMP_PIDL(*ppidl);
    }
    RETURN(hr);
}


HRESULT CXboxFolder::SetNameOf(HWND hWnd, LPCITEMIDLIST pidl, LPCOLESTR pszName, DWORD uFlags, LPITEMIDLIST *ppidl)
/*++
  Routine Description:
    Does not implement IShellFolder::SetNameOf for consoles or volumes.  This is currently not supported.

    Proper filtering of menu's should prevent ever hitting this routine.
--*/
{
    _ASSERT(FALSE);
    return E_NOTIMPL;
}

HRESULT CXboxFileSystemFolder::SetNameOf(HWND hWnd, LPCITEMIDLIST pidl, LPCOLESTR pszName, DWORD uFlags, LPITEMIDLIST *ppidl)
/*++
  Routine Description:
    Implements IShellFolder::SetNameOf

    Renames a directory or file on an Xbox.

  Arguments:

    hWnd           - If we need to display some UI.
    pidl           - Exactly one SHITEMID.
    pszName        - new name of item.
    uFlags         - type of name.
    ppidl          - [OUT] new pidl of item after rename.

  Return Value:

    On Success - S_OK.
    On Failure - OLE defined error.

  Comments:

    This routine is responsible for sending the rename over the wire.  If hWnd is valid, errors
    are displayed to the user.

    There are four principle reasons that this can fail:

    1) Xbox is unreachable.
    2) Source file does not exist.
    3) Name collision: i.e. new name is already taken.
    4) New name is not a valid item name (too long, illegal character, etc.)

    The first conversion of UNICODE to ANSI will susbstitute '#' for anything with
    the high byte set.  This catchs some illegal character problems.

    For all of these, we hit the wire, and display (and return) an error, based on the
    return value of IXboxConnection::Rename
--*/
{
    HRESULT      hr;
    char         *pszNewName;
    char         szWireNameSource[MAX_PATH];
    char         szWireNameDest[MAX_PATH];
    UINT         uIllegalCharacterCount;
    UINT         uFailResource;
    LPITEMIDLIST pidlAbsolute;
    UINT         uChildIndex;
    UINT         uNewNameLen;
    BOOL         fReturnSuccess = TRUE;
    
    if(ppidl) *ppidl = NULL;

    //
    //  Allocate space for new name
    //

    uNewNameLen = wcslen(pszName)+1;
    pszNewName = new char[uNewNameLen];
    if(!pszNewName)
    {
        return E_OUTOFMEMORY;
    }

    //
    //  Unicode to ANSI conversion
    //

    uIllegalCharacterCount = Utils::CopyWtoA(pszNewName, pszName);
    if(uIllegalCharacterCount)
    {
        if(hWnd)
        {
            WindowUtils::MessageBoxResource(hWnd, IDS_RENAME_ERROR_ILLEGAL_CHARACTERS, IDS_RENAME_ERROR_CAPTION, MB_ICONSTOP|MB_OK);
        }
        delete [] pszNewName;
        return E_INVALIDARG;
    }

    //
    //  It had better be a direct child, find it in our child list.
    //

    RefreshChildren();
    hr = GetChildIndex((LPSTR)pidl->mkid.abID, &uChildIndex);
    if(FAILED(hr))
    {   
        char szError[60];
        FormatUtils::XboxErrorString(hr, szError, sizeof(szError));
        WindowUtils::MessageBoxResource(hWnd, IDS_RENAME_XBDM_ERROR, IDS_RENAME_ERROR_CAPTION, MB_OK|MB_ICONSTOP, szError);
        delete [] pszNewName;
        return hr;
    }

    //
    //  We will need an absolute pidl later to send a notification
    //

    pidlAbsolute = GetChildPidl(uChildIndex, CPidlUtils::PidlTypeAbsolute);

    //
    //  Get the wire names
    //

    GetWireName(szWireNameSource, m_pszPathName, (LPSTR)pidl->mkid.abID);
    GetWireName(szWireNameDest, m_pszPathName, pszNewName);

    hr = SetName(hWnd, szWireNameSource, szWireNameDest);
    if(FAILED(hr))
    {
        char szError[60];
        FormatUtils::XboxErrorString(hr, szError, sizeof(szError));
        if(hWnd)
        {
            WindowUtils::MessageBoxResource(hWnd, IDS_RENAME_XBDM_ERROR, IDS_RENAME_ERROR_CAPTION, MB_OK|MB_ICONSTOP, szError);
        }
    }

    //
    //  After success, update child name
    //  

    if(SUCCEEDED(hr))
    {
        LPITEMIDLIST newPidl;

        //
        //  Update the childname, and file type
        //

        strcpy(m_rgChildFileAttributes[uChildIndex].Name, pszNewName);
        if(!(m_rgChildFileAttributes[uChildIndex].Attributes&FILE_ATTRIBUTE_DIRECTORY))
        {
            delete m_rgszChildTypeNames[uChildIndex];
            m_rgszChildTypeNames[uChildIndex] = GetFileTypeName(&m_rgChildFileAttributes[uChildIndex]);
        }

        //
        //  Get the new full pidl
        //

        newPidl = GetChildPidl(uChildIndex, CPidlUtils::PidlTypeAbsolute);
        if(newPidl)
        {
            if(pidlAbsolute)
            {
                //
                //  Send the notification
                //
                
                SHChangeNotify( 
                    (m_rgulChildShellAttributes[uChildIndex]&SFGAO_FOLDER) ? SHCNE_RENAMEFOLDER : SHCNE_RENAMEITEM,
                    SHCNF_IDLIST|SHCNF_FLUSH,
                    pidlAbsolute,
                    newPidl
                    );

                //
                //  Get pidl for out parameter
                //
            }
            
            g_pShellMalloc->Free(newPidl);
             
            if(ppidl)
            {
                *ppidl = GetChildPidl(uChildIndex, CPidlUtils::PidlTypeSimple);
            }
            
        }
    }
    
    //
    //  Done with the new name, we are not going to use it.
    //
    delete [] pszNewName;

    //
    //  If we managed to allocate the original pidlAbsolute, we need to free it now.
    //

    if(pidlAbsolute)
    {
        g_pShellMalloc->Free(pidlAbsolute);
    }

    if(fReturnSuccess)
    {
        return S_OK;
    }
    return hr;
}


HRESULT CXboxFileSystemFolder::SetName(HWND hWnd, LPSTR szWireNameOld, LPSTR szWireNameNew)
{

    HRESULT hr;

    //
    //  Work-around for bug 10218.  FATX doesn't properly handle renaming
    //  a file to itself perhaps with different capitalization. Since this is
    //  in the kernel ROM and it is not at all important for games, it will
    //  never get fixed.  Basically, we do not support change the capitalization
    //  of the name, we succeed and don't change it.
    if(0==_stricmp(szWireNameOld, szWireNameNew))
    {
            return S_OK;
    }
  
    hr = m_pConnection->HrRenameFile(szWireNameOld, szWireNameNew);
    if(FAILED(hr))
    {
        HRESULT hrLoc;
        DM_FILE_ATTRIBUTES dmFileAttributes;
        DWORD dwOldAttributes;

        //
        //  The return value was XBDM_CANNOTACCESS, see if it was read only,
        //  

        if(hr==XBDM_CANNOTACCESS)
        {
            hrLoc = m_pConnection->HrGetFileAttributes(szWireNameOld, &dmFileAttributes);
            if(SUCCEEDED(hrLoc))
            {
                dwOldAttributes = dmFileAttributes.Attributes;
                if(dwOldAttributes&FILE_ATTRIBUTE_READONLY)
                {
                    // We need to confirm renaming when it is on a read only file.  If we were provided
                    // with an hWnd, and it is not a directory, confirm
                    if(hWnd && !(dwOldAttributes&FILE_ATTRIBUTE_DIRECTORY))
                    {
                        LPSTR pszNewFriendlyName = strrchr(szWireNameNew, '\\');
                        pszNewFriendlyName++;
                        LPSTR pszOldFriendlyName = strrchr(szWireNameOld, '\\');
                        pszOldFriendlyName++;
                        //Confirm renaming of read-only file.
                        if(IDYES != WindowUtils::MessageBoxResource(
                                                    hWnd,
                                                    IDS_CONFIRM_RENAME_RO_FILE,
                                                    IDS_CONFIRM_RENAME_CAPTION,
                                                    MB_YESNO|MB_ICONQUESTION,
                                                    pszOldFriendlyName, pszNewFriendlyName)
                                                    ){
                            // The user didn't want to rename, just return success.
                            // Anything else causes the shell to pop of an error dialog.                           
                            return S_OK;
                        }

                    }
                    dmFileAttributes.Attributes = FILE_ATTRIBUTE_NORMAL;
                    hrLoc = m_pConnection->HrSetFileAttributes(szWireNameOld, &dmFileAttributes);
                    if(SUCCEEDED(hrLoc))
                    {
                        hr = m_pConnection->HrRenameFile(szWireNameOld, szWireNameNew);
                        //Set the attributes back
                        dmFileAttributes.Attributes = dwOldAttributes;
                        m_pConnection->HrSetFileAttributes(SUCCEEDED(hr) ? szWireNameNew : szWireNameOld, &dmFileAttributes);
                    }
                }
            }
        }
    }
    return hr;
}
