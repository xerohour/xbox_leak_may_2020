/*++

Copyright (c) Microsoft Corporation

Module Name:

    xboxpidl.cpp

Abstract:

    Implementation of CXboxPidl class

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    02-05-2000 : created

--*/

#include "priv.h"

//------------------------------------------------------------
// Definitions for display names
//------------------------------------------------------------
const WCHAR  XboxNameSpaceString[] = L"xbox://";
const size_t XboxNameSpaceStringCount = ((sizeof(XboxNameSpaceString)/ sizeof(WCHAR))-1);
#define MAX_XBOX_PATH 512

//------------------------------------------------------------
// Definitions of the internal format for Xbox PIDLs
//------------------------------------------------------------
#include <PSHPACK1.H>
typedef struct _XBOX_ITEMID
{
    USHORT cb;
    USHORT usItemType;
    CHAR   szItemData[1];
} XBOX_ITEMID, *PXBOX_ITEMID;
#include <POPPACK.H>
#define XBOX_ITEMTYPE_NOTXBOX   (0)
#define XBOX_ITEMTYPE_ROOT      (1)
#define XBOX_ITEMTYPE_MACHINE   (2)
#define XBOX_ITEMTYPE_VOLUME    (3)
#define XBOX_ITEMTYPE_FILEORDIR (4)


//------------------------------------------------------------
// Declare and Initialize static members of CXboxPidl
//------------------------------------------------------------
IMalloc *CXboxPidl::sm_pMalloc = NULL;
LONG    CXboxPidl::sm_lClassReference = 0;

HRESULT CXboxPidl::InitClass()
{
  ASSERT(NULL==sm_pMalloc);
  HRESULT hr = SHGetMalloc(&sm_pMalloc);
  ASSERT(SUCCEEDED(hr));
  return hr;
}
void CXboxPidl::DestroyClass()
{
  ASSERT(sm_pMalloc);
  sm_pMalloc->Release();
  sm_pMalloc = NULL;
}


//------------------------------------------------------------
// Destructor
//------------------------------------------------------------
CXboxPidl::~CXboxPidl()
/*++
    Destorys a PIDL.  Releases instance reference count on the class.
--*/
{
    DerefClass();
    if(m_Pidl)
    {
        sm_pMalloc->Free(m_Pidl);
        m_Pidl = NULL;
    }
}


HRESULT CXboxPidl::InitAsRoot()
/*++
    Routine Description:
        Initializes a CXboxPidl to point to the Xbox root.

  Return Value:
        S_OK - on success
        E_OUTOFMEMORY - the only possible failure now.
--*/
{
    //Assume that the class was successfully initialized
    ASSERT(sm_pMalloc);

    //Don't use this to reinit, only to init.
    ASSERT(NULL == m_Pidl);
    
    // Our root PIDL is fixed size
    ULONG cb = sizeof(XBOX_ITEMID) + 1;
    
    // Allocate memory for the root.
    m_Pidl = (LPITEMIDLIST) sm_pMalloc->Alloc(cb);
    if(NULL==m_Pidl) return E_OUTOFMEMORY;
    
    //Fill out the root item ID
    m_dwFirstXboxIdOffset = 0;
    PXBOX_ITEMID pRootItemId = (PXBOX_ITEMID)m_Pidl;
    pRootItemId->cb = sizeof(XBOX_ITEMID) - 1;
    pRootItemId->usItemType = XBOX_ITEMTYPE_ROOT;

    //Fill out the item type
    m_usItemType = XBOX_ITEMTYPE_ROOT;

    //Null Terminate the list
    AdvancePtr(pRootItemId,pRootItemId->cb)->cb = 0;
    return S_OK;
}

HRESULT CXboxPidl::InitWithPidl(LPCITEMIDLIST pidl, DWORD dwXboxIdOffset)
/*++
  Routine Description:
    Initializes an CXboxPidl with an existing pidl.

  Arguments:
    
    pidl           - pidl to initialize with.

    dwXboxIdOffset - offset to first ITEMID that is part of the Xbox namespace

  Return Value:
        S_OK - on success
        E_OUTOFMEMORY - the only possible failure now.
        
--*/
{
    //Assume that the class was successfully initialized
    ASSERT(sm_pMalloc);

    //Don't use this to reinit, only to init.
    ASSERT(NULL == m_Pidl);
    
    // Get the size
    ULONG cb = GetPidlSize(pidl);
    
    // Allocate memory for the pidl
    m_Pidl = (LPITEMIDLIST) sm_pMalloc->Alloc(cb);
    if(NULL==m_Pidl) return E_OUTOFMEMORY;

    //copy the parent pidl
    memcpy(m_Pidl, pidl, cb);

    //Set the offset to the xbox portion
    m_dwFirstXboxIdOffset = dwXboxIdOffset;

    //figure out the item type
    m_usItemType = XBOX_ITEMTYPE_NOTXBOX;
    PXBOX_ITEMID pXboxItemId = (PXBOX_ITEMID) AdvancePtr(m_Pidl, cb-2);
    while(pXboxItemId->cb)
    {
        m_usItemType = pXboxItemId->usItemType;
        pXboxItemId = AdvancePtr(pXboxItemId, pXboxItemId->cb);
    }
        
    return S_OK;
}

HRESULT CXboxPidl::InitWithDisplayName(CXboxPidl *pidlParent, LPCOLESTR pwszDisplayName)
/*++
  Routine Description:
    Initializes an CXboxPidl with a display name.

  Arguments:
    
    pidl            - pidl of the parent folder from the Xbox root.                      
    pwszDisplayName - Xbox path used to initialize pidl.

  Return Value:
        S_OK - on success
        E_OUTOFMEMORY - the only possible failure now.
  
  Comments:
    Whether or not the PIDL is absolute or relative is determine by whether the
    name starts with "xbox://"
        
--*/
{
   CHAR         pszBuffer[MAX_XBOX_PATH];
   DWORD        dwLength = MAX_XBOX_PATH;
   PXBOX_ITEMID pXboxItemIdList = NULL;
   DWORD        dwTokenCount;
   BOOL         fAbsolute;
   USHORT       cbPidl;
   USHORT       usLastItemIdType;
   HRESULT      hr;

   //Assume that the class was successfully initialized
   ASSERT(sm_pMalloc);

   //Don't use this to reinit, only to init.
   ASSERT(NULL == m_Pidl);
      
   // Preprocess the display name.
   //   1) Converts to MBCS.
   //   2) Throw out "xbox://" portion if present and set fAbsolute appropriately.
   //   3) Tokenize the items separating them with '\0'
   //   4) Count number of tokens (a.k.a. directories) 
   //   5) Return the new length.
   //
   //   hr will be failure if any of the characters are invalid
   hr = PreprocessDisplayName(pwszDisplayName, pszBuffer, &dwLength, &dwTokenCount, &fAbsolute);
   if(FAILED(hr))
   {
       return hr;
   }
   
   //each token requires an XBOX_ITEMID (which has space for the cb, the item type, and NULL termination
   //of the string), plus one character per character, + a two byte termination.
   cbPidl = (USHORT)(dwTokenCount * sizeof(XBOX_ITEMID) + dwLength + 2);

   if(fAbsolute)
   {
       // absolute path: prepend it with a root

       cbPidl += (sizeof(XBOX_ITEMID)-1);
       m_Pidl = (LPITEMIDLIST)
                (pXboxItemIdList = (PXBOX_ITEMID) sm_pMalloc->Alloc(cbPidl));
       if(pXboxItemIdList)
       {
           usLastItemIdType =
           pXboxItemIdList->usItemType = XBOX_ITEMTYPE_ROOT;
           pXboxItemIdList->cb = (sizeof(XBOX_ITEMID)-1);
           pXboxItemIdList = AdvancePtr(pXboxItemIdList, pXboxItemIdList->cb);
       }
   } else
   {
      // relative path: prepend it with a the pidl passed in
      
      // Get the parent pidl size sans termination
      USHORT cbPidlParent = (USHORT)(pidlParent->GetPidlSize() - 2); 
      cbPidl += cbPidlParent;
      m_Pidl = (LPITEMIDLIST)
               (pXboxItemIdList = (PXBOX_ITEMID)sm_pMalloc->Alloc(cbPidl));
      if(pXboxItemIdList)
      {
          memcpy(pXboxItemIdList, pidlParent->m_Pidl, cbPidlParent);
          usLastItemIdType = pidlParent->GetItemIdType();
          pXboxItemIdList = AdvancePtr(pXboxItemIdList, cbPidlParent);
      }
   }
  
   // Fail here if we didn't manage to allocate a pidl
   if(!pXboxItemIdList)
   {
      return E_OUTOFMEMORY;
   }

   //
   //   If we got here:
   //       m_Pidl is unterminated:
   //       pxboxItemIdList points to the next byte to write in m_Pidl
   //       pszBuffer should parse relative to m_Pidl
   //       usLastItemIdType contains the type of the last item in m_Pidl
   //
   USHORT cbItem;
   CHAR  *pszPos = pszBuffer;
   for(USHORT i=0; i < dwTokenCount; i++)
   {
       // Increment the item ID type, unless we are already at the file/folder
       // level
       if(XBOX_ITEMTYPE_FILEORDIR != usLastItemIdType) usLastItemIdType++;
       //assign the item id type
       pXboxItemIdList->usItemType =  usLastItemIdType;
       //copy the string - including the null
       cbItem = 0;
       do{
         pXboxItemIdList->szItemData[cbItem++] = *pszPos++;
       } while(*pszPos);
       // Add the XBOX_ITEMID structure to the cbItem
       cbItem += (sizeof(XBOX_ITEMID)-1); //the -1 is for the character is because one character
                                          //of the string is already in XBOX_ITEMID structure
       pXboxItemIdList->cb = cbItem;
       pXboxItemIdList = AdvancePtr(pXboxItemIdList, cbItem);
   }
   //   Terminate the PIDL
   pXboxItemIdList->cb = 0;

   // m_Pidl should now conform to syntax.
   return S_OK;
}

CXboxPidl& CXboxPidl::operator=(const CXboxPidl& rhs)
{
    //Only perform the copy if the rhs and lhs are different
    if(&rhs != this)
    {
        if(rhs.m_Pidl)
        {
            
            ULONG cb = rhs.GetPidlSize();
            //If we do not have a PIDL yet, then allocate one.
            if(!m_Pidl)
            {
                m_Pidl = (LPITEMIDLIST) sm_pMalloc->Alloc(cb);
            } else
            // realloc iff it is not big enough
            {
                if(cb > sm_pMalloc->GetSize(m_Pidl))
                {
                  //realloc can fail.  If it does, it does not free
                  //the original memory.  Hence,
                  //  "m_Pidl = (LPITEMIDLIST)m_pMalloc->Realloc(m_Pidl, cb);"
                  // could leak memory, so we need to write the next several lines
                  // of code.
                  void *reallocMemory = sm_pMalloc->Realloc(m_Pidl, cb);
                  if(reallocMemory)
                  {
                    if(reallocMemory)
                    {
                      m_Pidl = (LPITEMIDLIST)reallocMemory;
                    } else
                    {
                      sm_pMalloc->Free(m_Pidl);
                      m_Pidl = NULL;
                    }
                  }
                }
            }
            //
            //  After all that is possible that we don't have any memory
            //
            if(m_Pidl)
            {
                memcpy(m_Pidl, rhs.m_Pidl, cb);
                rhs.m_dwFirstXboxIdOffset;
            }
        } else if(m_Pidl)
        // if copying a blank PIDL, just make sure we are blank
        {
            sm_pMalloc->Free(m_Pidl);        
            m_Pidl = NULL;
        }
    }
    // In all cases, return a reference to ourselves.
    return *this;
}

HRESULT CXboxPidl::Append(const CXboxPidl& rhs)
{
    //appending a PIDL to itself is guaranteed to generate
    //a bad PIDL
    if(&rhs != this) return E_INVALIDARG;

    //If we are not initialized yet, then call our
    //assignment operator.  If we are still unitialized
    //fail.
    if(!m_Pidl)
    {
        *this = rhs;
        if(!m_Pidl)
        {
            return E_FAIL;
        }
        return S_OK;
    }
    
    //
    //  If the rhs is NULL just return success
    //
    if(!rhs.m_Pidl) return S_OK;

    //
    //  Xbox PIDLs do not support junctions so
    //  so the resulting PIDL better not end up
    //  with a section of non-Xbox PIDLs under
    //  Xbox PIDLs.
    //
    if( 
        rhs.m_dwFirstXboxIdOffset &&
        (m_dwFirstXboxIdOffset !=  GetPidlSize())
    )
    {
        return E_INVALIDARG;
    }

    //
    //  We will also check after the fact that the
    //  resulting PIDL is legal.
    //
    ULONG cbThis, cbRhs, cbTotal;
    cbThis = GetPidlSize() - 2; //subtract the NULL off cbThis, since it won't be preserved.
    cbRhs = rhs.GetPidlSize();
    cbTotal = cbThis + cbRhs;
    
    //
    //  Allocate memory if necessary to append.
    //
    if(cbTotal > sm_pMalloc->GetSize(m_Pidl))
    {
      //realloc can fail.  We will simply abort the append
      //in that case.
      void *reallocMemory = sm_pMalloc->Realloc(m_Pidl, cbTotal);
      if(reallocMemory)
      {
        if(reallocMemory)
        {
          m_Pidl = (LPITEMIDLIST)reallocMemory;
        } else
        {
          return E_OUTOFMEMORY;
        }
      }
    }
    
    //
    //  There is now enough memory, so lets just append
    //
    LPITEMIDLIST pidlAppendPoint = AdvancePtr(m_Pidl, cbThis);
    memcpy(pidlAppendPoint, rhs.m_Pidl, cbRhs); 
  
    //
    //  Now validate the PDL
    //
    if(!IsValid())
    {
        //reterminate at the append point - don't worry about trying to reshrink
        //the buffer, it is not a big deal.
        pidlAppendPoint->mkid.cb = 0;
        return E_INVALIDARG;
    }

    return S_OK;
} 


//gets a copy of the real PIDL
LPITEMIDLIST  CXboxPidl::GetPidl() const
{
    LPITEMIDLIST pidlResult = NULL;
    //Assume that the class was successfully initialized
    ASSERT(sm_pMalloc);

    // There had better be a PIDL to get.
    if(m_Pidl)
    {
        // Get the size
        ULONG cb = GetPidlSize();
        // Allocate memory for the pidl
        pidlResult = (LPITEMIDLIST) sm_pMalloc->Alloc(cb);
        if(pidlResult)
        {
            //copy pidl to the result
            memcpy(pidlResult, m_Pidl, cb);        
        }
    }
    return pidlResult;
}

HRESULT CXboxPidl::GetDisplayName(DWORD uFlags, LPSTRRET lpName) const
/*++
    Routine Description:
        The implementation of IShellFolder::GetDisplayNameOf uses this method
        to extract a name from a fully qualified name.

    Arguments:
        uFlags - same flags passed to IShellFolder::GetDisplayNameOf see MSDN
        lpName - structure for returning the display name

    Comments:
        1) The parsing names and the display are the same.
        2) For relative paths we use the cStr member of the return path.
           Otherwise, the path could exceed MAX_PATH due to the addition of the
           local path to the machine name.
        3) For now at least, do not strip the extension from edit and display names.
        
--*/
{
    
    PXBOX_ITEMID pXboxItemIdList;
    PCHAR   pWritePosition = lpName->cStr;
    size_t  strLength = 0;
    lpName->uType = STRRET_CSTR;

    //If in folder is request, skip everything accept the last PIDL
    if(uFlags&SHGDN_INFOLDER)
    {
        //skip to the end.
        pXboxItemIdList = (PXBOX_ITEMID)GetLastItemId(m_Pidl);
    } else
    {
        pXboxItemIdList = (PXBOX_ITEMID)AdvancePtr(m_Pidl, m_dwFirstXboxIdOffset);
    }

    //walk the whole path.
    while(pXboxItemIdList->cb)
    {   
        //BUGBUG: For now assume that "xbox://<machinename>/" will not push over MAX_PATH
        //Eventually we can fix this, by checking if this item would do it.  If it would
        //then we copy cStr into an OLESTR allocate with pMalloc, and continue
        //writing to cStr at the beginning, then before we returning we paste cStr onto
        //the end of the OLESTR and return the OLESTR instead of the cStr.  BUT DO THAT
        //LATER
        switch(pXboxItemIdList->usItemType)
        {
            case XBOX_ITEMTYPE_ROOT:
              // If the name is for parsing prepend "xbox://"
              if(uFlags&SHGDN_FORPARSING)
              {
                strcpy(pWritePosition, "xbox://");
                strLength += sizeof("xbox://")-1;
                pWritePosition = AdvancePtr(pWritePosition, sizeof("xbox://")-1);
              } 
              //
              // There is one case that we need to include the root in the display
              // or edit name.  That is when the root is the only Pidl in the item list:
              // i.e. this item is the root
              else if(XBOX_ITEMTYPE_ROOT == m_usItemType)
              {
                strcpy(pWritePosition, "Xboxes");
                strLength += sizeof("Xboxes")-1;
                pWritePosition = AdvancePtr(pWritePosition, sizeof("Xboxes")-1);
              }
              pXboxItemIdList = AdvancePtr(pXboxItemIdList, pXboxItemIdList->cb);
              break; 
              //The next three all work the same, they just copy the text
              //from the PIDL
            case XBOX_ITEMTYPE_MACHINE:
            case XBOX_ITEMTYPE_VOLUME:
            case XBOX_ITEMTYPE_FILEORDIR:
              *pWritePosition++ = '\\';
              memcpy(
                  pWritePosition,
                  pXboxItemIdList->szItemData,
                  pXboxItemIdList->cb - sizeof(XBOX_ITEMID) + 1 //Copy the NULL termination too.
                  );
              //Advance the pointer without the NULL terminator
              pWritePosition += (pXboxItemIdList->cb - sizeof(XBOX_ITEMID)); 
              //Increment the bytes written for the '\\' and the item text
              strLength += (pXboxItemIdList->cb - sizeof(XBOX_ITEMID) + 1);
              //Move on to next item in PIDL
              pXboxItemIdList = AdvancePtr(pXboxItemIdList, pXboxItemIdList->cb);
              break; 
        }
    }

    //At this time, this cannot fail.
    return S_OK;
}

// get information necessary to communicate with an Xbox
HRESULT CXboxPidl::GetXboxName(LPSTR pMachineName, DWORD *pdwLength)
{
    //Get the first Xbox item
    PXBOX_ITEMID pXboxItemIdList;
    pXboxItemIdList = (PXBOX_ITEMID)AdvancePtr(m_Pidl, m_dwFirstXboxIdOffset);
    if(!pXboxItemIdList->cb) return E_FAIL;

    while(XBOX_ITEMTYPE_MACHINE != pXboxItemIdList->usItemType)
    {
        pXboxItemIdList = (PXBOX_ITEMID)AdvancePtr(m_Pidl, pXboxItemIdList->cb);
        if(!pXboxItemIdList->cb) return E_FAIL;
    }
    
    //Check that the buffer is large enough.
    if(*pdwLength < (pXboxItemIdList->cb - sizeof(XBOX_ITEMID) + 1))
    {
       return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }

    *pdwLength = (pXboxItemIdList->cb - sizeof(XBOX_ITEMID) + 1);
    memcpy(pMachineName, pXboxItemIdList->szItemData, *pdwLength);

    return S_OK;
}
HRESULT CXboxPidl::GetXboxFilePath(LPSTR pFilePath, DWORD *pdwLength)
{
    return E_NOTIMPL;
}

HRESULT
CXboxPidl::PreprocessDisplayName(
    LPCOLESTR pwszDisplayName,
    LPSTR     pszBuffer,
    DWORD    *pdwLength,
    DWORD    *pdwTokenCount,
    BOOL     *pfAbsolute
)
/*++
  Routine Description:
    Converts display names into something we can start
    to parse.
--*/
{
   LPSTR pszStr;
   DWORD dwLength;
   
   *pdwTokenCount = 0;
   
   //
   // Determine whether the display name is absolute or relative.
   //
   if(0==_wcsnicmp(pwszDisplayName, XboxNameSpaceString, XboxNameSpaceStringCount))
   // absolute path
   {
       pwszDisplayName += XboxNameSpaceStringCount; //skip the "xbox://" part
       *pfAbsolute = TRUE; //this is an absolute path
   } else
   // relative path
   {
       *pfAbsolute = FALSE;
   }

   //Make sure there is a string after "xbox://"
   if(L'\0' == *pwszDisplayName)
   {
        return 0;
   } 

   // if the first character is '/' or '\' strip it off
   if( (L'\\' == *pwszDisplayName) || (L'/' == *pwszDisplayName) )
   {
       pwszDisplayName++;
   }

   // Run it through WideCharToMultiByte to do the initial conversion
   dwLength = WideCharToMultiByte(
                    CP_OEMCP,
                    0,
                    pwszDisplayName,
                    -1,
                    pszBuffer,
                    *pdwLength,
                    NULL,
                    NULL
                    );

   if(0 == dwLength)
   {
      *pdwLength = 0;
      return HRESULT_FROM_WIN32(GetLastError());
   }

   // There is at least one token
   *pdwTokenCount = 1;
   
   //Now walk through it ourselves looking for '\' and '/' 
   //convert to '\0', and count them, don't count it though
   //if it is the last character.
   pszStr = pszBuffer;
   while(*pszStr)
   {
       if( ('\\' == *pszStr) || ('/' == *pszStr) )
       {
         *pszStr++ = '\0';
         if('\0' == *pszStr)
         {
             dwLength--; //truncate the length by one.
             break;
         }
         // Two token delimiter in a row is a syntax
         // error.
         if( ('\\' == *pszStr) || ('/' == *pszStr) )
         {
            return HRESULT_FROM_WIN32(ERROR_INVALID_NAME);
         }
         *pdwTokenCount++;
       }
       pszStr++;
   }
   *pdwLength = dwLength;
   return S_OK;
}


/***********************************************************
 * IsValid() performs two checks:
 *
 * 1) Has this been initialized so that it contains
 *    any PIDL at all.
 *
 * 2) Are the hierarchy contraints met, namely:
 *
 *    NON-XBOX\ROOT\VOLUME\[DIR[\DIR]\]FILE
 *
 *              XBOX:// 
 ***********************************************************/
bool CXboxPidl::IsValid() const
{
    if(!m_Pidl) return false;
    // Skip the non-Xbox part
    USHORT usPrevItemType = XBOX_ITEMTYPE_NOTXBOX;
    PXBOX_ITEMID pXboxItemId = (PXBOX_ITEMID) AdvancePtr(m_Pidl, m_dwFirstXboxIdOffset);
    while(pXboxItemId->cb)
    {
        switch(usPrevItemType)
        {
            case XBOX_ITEMTYPE_NOTXBOX:
                if( m_dwFirstXboxIdOffset &&
                   (XBOX_ITEMTYPE_ROOT != pXboxItemId->usItemType)
                  ) return false;
            case XBOX_ITEMTYPE_ROOT:
                if(XBOX_ITEMTYPE_MACHINE != pXboxItemId->usItemType)
                    return false;
            case XBOX_ITEMTYPE_MACHINE:
                if(XBOX_ITEMTYPE_VOLUME != pXboxItemId->usItemType)
                    return false;
            case XBOX_ITEMTYPE_VOLUME: 
            case XBOX_ITEMTYPE_FILEORDIR:
                if(XBOX_ITEMTYPE_FILEORDIR != pXboxItemId->usItemType)
                    return false;
        }
        usPrevItemType = pXboxItemId->usItemType;
        pXboxItemId = AdvancePtr(pXboxItemId, pXboxItemId->cb);
    }
    return true;
}


/*****************************
 Walk a Pidl to get its length
*****************************/
ULONG CXboxPidl::GetPidlSize(LPCITEMIDLIST pidl)
{
    if(!pidl) return 0;
    ULONG    cbItemList = 2; //We know there are two bytes on the end
    USHORT   cbItem = pidl->mkid.cb;
    while(cbItem)
    {
        pidl = AdvancePtr(pidl, cbItem);
        cbItemList += cbItem;
        cbItem = pidl->mkid.cb;
    }
    return cbItemList;
}

LPCITEMIDLIST CXboxPidl::GetNextItemId(LPCITEMIDLIST pidl)
{
    pidl = AdvancePtr(pidl, pidl->mkid.cb);
    return pidl->mkid.cb ? pidl : NULL;
}

LPITEMIDLIST CXboxPidl::GetNextItemId(LPITEMIDLIST pidl)
{
    pidl = AdvancePtr(pidl, pidl->mkid.cb);
    return pidl->mkid.cb ? pidl : NULL;
}

LPCITEMIDLIST CXboxPidl::GetLastItemId(LPCITEMIDLIST pidl)
{
    LPCITEMIDLIST pidlPrevious;
    do
    {
        pidlPrevious = pidl;
        pidl = GetNextItemId(pidl);
    } while(pidl);
    return pidlPrevious;
}

LPITEMIDLIST CXboxPidl::CopyFirstItemId(LPCITEMIDLIST pidl)
{
    ULONG cbCopy = pidl->mkid.cb + 2;
    LPITEMIDLIST pidlLast;
    LPITEMIDLIST pidlCopy = (LPITEMIDLIST) sm_pMalloc->Alloc(cbCopy);
    if(pidlCopy)
    {
        memcpy(pidlCopy, pidl, pidl->mkid.cb);
        pidlLast = GetNextItemId(pidlCopy);
        pidlLast->mkid.cb = 0;
    }
    return pidlCopy;
}
   