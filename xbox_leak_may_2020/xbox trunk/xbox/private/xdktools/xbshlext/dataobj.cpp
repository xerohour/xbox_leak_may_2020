/*++

Copyright (c) Microsoft Corporation

Module Name:

    dataobj.cpp

Abstract:

    Implementation of CXboxDataObject.  CXboxDataObject handles file transfer when the
    namespace extension is acting as a source.


Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    04-03-2001 : created (mitchd)
    04-25-2001 : implemented IDataObject methods (jeffsim)
    07-03-2001 : changed implementation to use CXboxSelectedItems and to recurse properly (mitchd)
    07-24-2001 : change implementation for new IXboxVisitor architecture, (mitchd)

--*/

#include "stdafx.h"

//-------------------------------------------------------------------------------------
// Supported data formats
//-------------------------------------------------------------------------------------
CLIPFORMAT CF_FILECONTENTS               = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILECONTENTS);
CLIPFORMAT CF_FILEDESCRIPTORA            = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILEDESCRIPTORA);
CLIPFORMAT CF_FILEDESCRIPTORW            = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW);
CLIPFORMAT CF_FILENAMEMAPA               = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILENAMEMAPA);
CLIPFORMAT CF_FILENAMEMAPW               = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILENAMEMAPW);
CLIPFORMAT CF_FILENAME                   = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILENAME);
CLIPFORMAT CF_PREFERREDDROPEFFECT        = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
CLIPFORMAT CF_PERFORMEDDDROPEFFECT       = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_PERFORMEDDROPEFFECT);
CLIPFORMAT CF_LOGICALPERFORMEDDROPEFFECT = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_LOGICALPERFORMEDDROPEFFECT);
CLIPFORMAT CF_PASTESUCCEEDED             = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_PASTESUCCEEDED);
CLIPFORMAT CF_SHELLIDLIST                = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_SHELLIDLIST);
CLIPFORMAT CF_XBOXFILEDESCRIPTOR         = (CLIPFORMAT)RegisterClipboardFormat(TEXT("XBOX_FILEDESCRIPTOR"));



#define FORMAT_XBOXFILEDESCRIPTOR          {CF_XBOXFILEDESCRIPTOR,        NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}
#define FORMAT_FILEDESCRIPTORA             {CF_FILEDESCRIPTORA,           NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}
#define FORMAT_FILEDESCRIPTORW             {CF_FILEDESCRIPTORW,           NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}
#define FORMAT_FILECONTENTS                {CF_FILECONTENTS,              NULL, DVASPECT_CONTENT, -1, TYMED_ISTREAM}
#define FORMAT_SHELLIDLIST                 {CF_SHELLIDLIST,               NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}
#define FORMAT_PREFERREDDROPEFFECT         {CF_PREFERREDDROPEFFECT,       NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}
#define FORMAT_PERFORMEDDDROPEFFECT        {CF_PERFORMEDDDROPEFFECT,      NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}
#define FORMAT_LOGICALPERFORMEDDROPEFFECT  {CF_LOGICALPERFORMEDDROPEFFECT,NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}
#define FORMAT_PASTESUCCEEDED              {CF_PASTESUCCEEDED,            NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}

GET_DATA_FORMAT_TABLE_ENTRY CXboxDataObject::sm_GetDataSupportedFormats[] = 
{
  {FORMAT_XBOXFILEDESCRIPTOR,  CXboxDataObject::GetXboxDescriptor},
  {FORMAT_FILEDESCRIPTORA,     CXboxDataObject::GetFileDescriptorA},
  {FORMAT_FILEDESCRIPTORW,     CXboxDataObject::GetFileDescriptorW},
  {FORMAT_FILECONTENTS,        CXboxDataObject::GetFileContents},
  {FORMAT_SHELLIDLIST,         CXboxDataObject::GetShellIdList}
};
UINT CXboxDataObject::sm_uGetDataSupportedCount = sizeof(CXboxDataObject::sm_GetDataSupportedFormats)/sizeof(GET_DATA_FORMAT_TABLE_ENTRY);

SET_DATA_FORMAT_TABLE_ENTRY CXboxDataObject::sm_SetDataSupportedFormats[] = 
{
  {FORMAT_PREFERREDDROPEFFECT,         CXboxDataObject::SetPreferredDropEffect},
  {FORMAT_PERFORMEDDDROPEFFECT,        CXboxDataObject::SetPerformedDropEffect},
  {FORMAT_LOGICALPERFORMEDDROPEFFECT,  CXboxDataObject::SetLogicalPerformedDropEffect},
  {FORMAT_PASTESUCCEEDED,              CXboxDataObject::SetPasteSucceeded}
};
UINT CXboxDataObject::sm_uSetDataSupportedCount = sizeof(CXboxDataObject::sm_SetDataSupportedFormats)/sizeof(SET_DATA_FORMAT_TABLE_ENTRY);


//-------------------------------------------------------------------------------------
// Creation and Destruction
//-------------------------------------------------------------------------------------
HRESULT 
CXboxDataObject::Create(
    UINT           cidl,
    LPCITEMIDLIST *apidl,
    CXboxFolder   *pParent,
    IDataObject   **ppDataObject
    )
/*++
 Routine Description:
  Creates an IDataObject for the selected items of a CXboxFolder.  This works
  for file items as well as other types (for links only).

 Arguments:

  cidl         - number of ItemIDs selected (can be zero, meaning the parent is the selection).
  apidl        - array of selected ItemIDs.
  pParent      - parent of the selected items (unless cidl is zero).
  ppDataObject - [OUT] get the data object on success

 Return Value:

  S_OK on success.
  E_NOINTERFACE if the object type is not supported.
  E_OUTOFMEMORY is always possible.
--*/
{
    ULONG ulShellAttributes = SFGAO_CANCOPY | SFGAO_CANMOVE | SFGAO_CANLINK;
    CXboxFolder *pSelection;
    HRESULT hr = pParent->GetAttributesOf(cidl, apidl, &ulShellAttributes);
    if(SUCCEEDED(hr))
    {
        if(!ulShellAttributes)
        {
            hr=E_NOINTERFACE;
        } else
        {
            hr = pParent->CloneSelection(cidl, apidl,&pSelection);
        }
    }

    if(SUCCEEDED(hr))
    {
        CComObject<CXboxDataObject> *pDataObject;
        hr = CComObject<CXboxDataObject>::CreateInstance(&pDataObject);
        if(SUCCEEDED(hr))
        {
            pDataObject->m_pSelection = pSelection;
            pDataObject->m_ulShellAttributes = ulShellAttributes;
            pDataObject->m_uTopLevelItemCount = cidl;
            pDataObject->QueryInterface(IID_PPV_ARG(IDataObject, ppDataObject));
            SHGetThreadRef(&pDataObject->m_pUnkThread);
            //Assume drop effect copy, it might get changed later.
            DataObjUtil::SetPreferredDropEffect(pDataObject, DROPEFFECT_COPY);
        } else
        {
            pSelection->Release();
        }
    }
    return hr;
}

HRESULT 
CXboxDataObject::Create(
    UINT           cidl,
    CXboxFolder   *pSelection,
    IDataObject   **ppDataObject
    )
/*++
 Routine Description:
  Creates an IDataObject for the selected items of a CXboxFolder.  This works
  for file items as well as other types (for links only).

 Arguments:

  cidl         - number of ItemIDs selected (can be zero, meaning the parent is the selection).
  pSelection   - Xbox Selection Clone
  ppDataObject - [OUT] get the data object on success

 Return Value:

  S_OK on success.
  E_NOINTERFACE if the object type is not supported.
  E_OUTOFMEMORY is always possible.
--*/
{
    ULONG ulShellAttributes = SFGAO_CANCOPY | SFGAO_CANMOVE | SFGAO_CANLINK;
    pSelection->GetSelectShellAttributes(&ulShellAttributes);
    if(!ulShellAttributes)
    {
        return E_NOINTERFACE;
    }

    CComObject<CXboxDataObject> *pDataObject;
    HRESULT hr = CComObject<CXboxDataObject>::CreateInstance(&pDataObject);
    if(SUCCEEDED(hr))
    {
        pSelection->AddRef();
        pDataObject->m_pSelection = pSelection;
        pDataObject->m_ulShellAttributes = ulShellAttributes;
        pDataObject->m_uTopLevelItemCount = cidl;
        pDataObject->QueryInterface(IID_PPV_ARG(IDataObject, ppDataObject));
        SHGetThreadRef(&pDataObject->m_pUnkThread);
        //Assume drop effect copy, it might get changed later.
        DataObjUtil::SetPreferredDropEffect(pDataObject, DROPEFFECT_COPY);
    } else
    {
        pSelection->Release();
    }
    
    return hr;
}

CXboxDataObject::~CXboxDataObject()
/*++
  Routine Description:
    As we go out of existance, we need to clean up anything
    we allocated along the way.

    This may also be the correct place to complete an unoptimized move.
--*/
{
    /*
    **  TODO: Hmm, this may be the time to complete
    **  unoptimized moves.
    */

    
    //
    //  We are done with out clone of the selection
    //
    if(m_pSelection) m_pSelection->Release();
    
    //
    //  Cleanup the m_pXboxFileGroupList, if it 
    //  was ever created.
    //
    free(m_pXboxFileGroupDescriptor);

    //
    //  If we are holding a reference count on the thread
    //  release it.
    //
    if(m_pUnkThread) m_pUnkThread->Release();
}

/*
**  IDataObject Methods
**
*/

STDMETHODIMP CXboxDataObject::GetData(FORMATETC *pfetc, STGMEDIUM *pstgm)
/*++

 Routine Description:

   Implements IDataObject::GetData.  Retrieves the data associated with a particular data format.

 Arguments:

   pfetc - [IN]  Information about the desired data format
   pstgm - [OUT] The data for the desired data format

 Return Value:
   S_OK             - success
   DV_E_LINDEX      - the index did not match.
   DV_E_FORMAT_ETC  - This object does not currently contain the desired clipboard format.
   DV_E_TYMED       - The tymed value was invalid for the desired clipboard format.
   DV_E_DVASPECT    - The dwAspect value was invalid for the desired clipboard format.
   STG_E_MEDIUMFULL - Memory allocation error related to the medium.
   E_OUTOFMEMORY    - Generic memory allocation error.
   E_INVALIDARG     - Caller passed a bad argument.

--*/
{
    HRESULT hr;
    UINT    uDataFormatIndex;

    ENTER_SPEW;
    
    // Validate parameters
    if (pfetc == NULL || pstgm == NULL)
        return E_INVALIDARG;

    //
    //  Dump the format name.
    //
    //DUMP_FORMAT_NAME(pfetc->cfFormat);

    //
    //   First, check our table for supported formats
    //
    hr = FindMatchingGetFormat(pfetc, &uDataFormatIndex);
    if(SUCCEEDED(hr))
    {
        hr = (this->*sm_GetDataSupportedFormats[uDataFormatIndex].pfnGetData)(pfetc, pstgm, false);
    } else
    //
    //  Perhaps someone stored a non-native format.  Check that list too.
    //  
    {
        CStoredDataFormat *pStoredFormat = m_StoredFormats.FindMatch(pfetc);
        if(pStoredFormat)
        {
            hr = pStoredFormat->CopyTo(pstgm);
        }
    }
    return hr;
}


STDMETHODIMP CXboxDataObject::SetData(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fRelease)
/*++
 Routine Description:

  Implements IDataObject::SetData.  There are a few formats that are really messages to us.  Those
  are found in our sm_SetDataSupportedFormats table and have special handlers.  The rest we store as
  generic data.  We rely on CStoredDataFormatList to handle this.

 Arguments:
   pfetc    - describes the format to store
   pstgm    - data to store in the specified format.
   fRelease - if true, we store the data, and we are responsible for freeing it when we are done.
              if false, we must store a copy and free it when we are done.

 Return Values:
   S_OK             - Success
   E_INVALIDARG     - Caller passed a bad argument or mutually incompatible arguments.
   STG_E_MEDIUMFULL - fRelease was FALSE, and the memory allocation failed.
   E_OUTOFMEMORY    - some other memory allocation failed.

--*/
{
    HRESULT hr;
    UINT uDataFormatIndex;
    
    //
    // Verify parameters
    //
    if (pfetc == NULL || pstgm == NULL)
    {
        return E_INVALIDARG;
    }

    //
    //  First, check our table for supported formats, and allow
    //  any special handling.
    //
    hr = FindMatchingSetFormat(pfetc, &uDataFormatIndex);
    if(SUCCEEDED(hr))
    {
        hr = (this->*sm_SetDataSupportedFormats[uDataFormatIndex].pfnSetData)(pfetc, pstgm, fRelease);
    } else
    //
    // Otherwise store it as a generic format.
    //
    {
        hr = m_StoredFormats.Store(pfetc, pstgm, fRelease);
    }

    return hr;
}

STDMETHODIMP CXboxDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppieftc)
/*++
 Routine Description:
   Implements IDataObject::EnumFormatEtc.
 Arguments:
   dwDirection - DATADIR_GET enumerates which formats may be retrieved via GetData.
   ppieftc     - DATADIR_SET enumerates which formats may be retrieved via SetData.
 Return Value:
   S_OK         - Success
   E_INVALIDARG - Caller passed ppieftc = NULL.
   Other OLE errors.
 Comments:
   We use ATLs standard CComEnum appropriately specialized to CComEnumFormatEtc
   (this is typedef'd in the dataobj.h).  In this routine, we build an array of
   supported formats (both native formats, and those that have been stored with SetData).
   Then initialize CComEnumFormatEtc with this array.

--*/
{
    HRESULT hr = S_OK;
    //
    //  In case we fail, NULL out the out parameter.
    //

    *ppieftc = NULL;

    //
    // Verify parameters
    //

    if (ppieftc == NULL) return E_INVALIDARG;
    if( (DATADIR_GET!=dwDirection) && (DATADIR_SET!=dwDirection))
    {
        _ASSERT(FALSE);
        return E_INVALIDARG;
    }

    //
    //  Build the array
    //
    UINT       uFormatCount;
    FORMATETC *pFirstFormat;
    FORMATETC *pNextFormat;
    if(DATADIR_GET == dwDirection)
    {
        //
        //  If the selected items may be copied then all of the
        //  formats are supported.  Otherwise, just CF_SHELLIDLIST
        //
        if(m_ulShellAttributes&SFGAO_CANCOPY)
        {
            uFormatCount = sm_uGetDataSupportedCount;
        } else
        {
            uFormatCount = 1;
        }
        //
        //  Add in any formats that may have been stored
        //  via SetData.
        //
        uFormatCount += m_StoredFormats.GetCount();
        
        //
        //  Allocate the array.
        //
        pFirstFormat = new FORMATETC[uFormatCount];

        if(pFirstFormat)
        {
            //
            //  Copy the native formats
            //
        
            UINT uIndex = (m_ulShellAttributes&SFGAO_CANCOPY) ? 0 : (sm_uGetDataSupportedCount-1);
            for(pNextFormat = pFirstFormat; uIndex < sm_uGetDataSupportedCount; uIndex++, pNextFormat++)
            {
                memcpy(pNextFormat, &sm_GetDataSupportedFormats[uIndex].formatEtc, sizeof(FORMATETC));
            }

            //
            //  Copy the stored formats.
            //

            CStoredDataFormat *pDataFormat = m_StoredFormats.GetFirst();
            while(pDataFormat)
            {
                pDataFormat->CopyFormat(pNextFormat++);
                pDataFormat = pDataFormat->GetNext();
            }
        }

    } else if(DATADIR_SET)
    {
        uFormatCount = sm_uSetDataSupportedCount;
        //
        //  Allocate the array.
        //
        pFirstFormat = new FORMATETC[uFormatCount];
        
        if(pFirstFormat)
        {
            //
            //  Copy the native formats
            //
        
            UINT uIndex = 0;
            for(pNextFormat = pFirstFormat; uIndex < sm_uSetDataSupportedCount; uIndex++, pNextFormat++)
            {
                memcpy(pNextFormat, &sm_SetDataSupportedFormats[uIndex].formatEtc, sizeof(FORMATETC));
            }
        }
    }

    //
    //  If we managed to get an array, then create an enumerator
    //  and initialize it,
    if(pFirstFormat)
    {
        CComObject<CEnumFormatEtc> *pEnumFormatEtc;
        hr = CComObject<CEnumFormatEtc>::CreateInstance(&pEnumFormatEtc);
        if(SUCCEEDED(hr))
        {
            hr = pEnumFormatEtc->Init(pFirstFormat, pNextFormat, NULL, AtlFlagTakeOwnership);
            if(SUCCEEDED(hr))
            {
                hr = pEnumFormatEtc->QueryInterface(IID_PPV_ARG(IEnumFORMATETC, ppieftc));
            }
            if(FAILED(hr))
            {   
                delete pEnumFormatEtc;
                delete [] pFirstFormat;
            }
        }
        else
        {
            delete [] pFirstFormat;
        }
    } else
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

STDMETHODIMP CXboxDataObject::QueryGetData(FORMATETC *pfetc)
/*++
  Routine Description:
    Checks to see if it is possible to get a given data object.
--*/
{
    HRESULT hr;
    ENTER_SPEW;
    
    // Validate parameters
    if (pfetc == NULL) return E_INVALIDARG;

    //
    //  Dump the format name.
    //
    //DUMP_FORMAT_NAME(pfetc->cfFormat);

    //
    //   First, check our table for supported formats
    //

    hr = FindMatchingGetFormat(pfetc);

    //
    //  Perhaps someone stored a non-native format.  Check that list too.
    //

    if(FAILED(hr))
    {
        CStoredDataFormat *pStoredFormat = m_StoredFormats.FindMatch(pfetc);
        if(pStoredFormat)
        {
            hr = S_OK;
        }
    }
    return hr;
}

STDMETHODIMP CXboxDataObject::GetCanonicalFormatEtc(FORMATETC *pfetcIn, FORMATETC *pfetcOut)
/*++
  Routine Description:
    Implements IDataObject::GetCanonicalFormatEtc.
    
    There is a big long discussion in the FTP shell extension code why this should be
    implemented thusly.  The arguments were all pretty generic.
--*/
{
    return DATA_S_SAMEFORMATETC;
}


/*
**  IDataObject methods that we do not (yet?) support
**
*/

STDMETHODIMP CXboxDataObject::DAdvise(FORMATETC *pfetc, DWORD dwAdviseFlags, IAdviseSink * piadvsink,
	                             DWORD * pdwConnection)
{
    return E_NOTIMPL;
}

STDMETHODIMP CXboxDataObject::DUnadvise(DWORD dwConnection)
{
    return E_NOTIMPL;
}

STDMETHODIMP CXboxDataObject::EnumDAdvise(IEnumSTATDATA **ppienumStatData)
{
	return E_NOTIMPL;
}

//
//  BugBug: I think that this is required for persistent storage on the clipboard after a copy and close.
//
STDMETHODIMP CXboxDataObject::GetDataHere(FORMATETC *pfetc, STGMEDIUM *pstgm)
{
    //_ASSERTE(FALSE);
    return E_NOTIMPL;
}

/**
***
*** IAsyncOperation Implementation
***
**/
STDMETHODIMP CXboxDataObject::EndOperation(HRESULT hResult, IBindCtx *pbcReserved, DWORD dwEffects)
{
    //
    //  At the end of an Async Move we need to delete all the items.
    //  In the non-Async case, we do this from the out IShellFolderViewCB
    //  implemented CXboxView.
    //
    
    if(SUCCEEDED(hResult) && (DROPEFFECT_MOVE == dwEffects))
    {
        CXboxDelete objDelete(NULL, m_pSelection->GetChildCount(), true);
        m_pSelection->VisitEach(&objDelete, IXboxVisitor::FlagContinue|IXboxVisitor::FlagRecurse);
    }
    m_fDidAsynchStart = FALSE;
    return S_OK;
}
STDMETHODIMP CXboxDataObject::GetAsyncMode(BOOL *pfIsOpAsync)
{
    if(m_pUnkThread || WindowUtils::IsMainShellProcess())
    {
        *pfIsOpAsync = TRUE;
    } else
    {
        *pfIsOpAsync = FALSE;
    }
    return S_OK;
}
STDMETHODIMP CXboxDataObject::InOperation(BOOL *pfInAsyncOp)
{
    *pfInAsyncOp = m_fDidAsynchStart;
    return S_OK;
}
STDMETHODIMP CXboxDataObject::StartOperation(IBindCtx *pbcReserved)
{
    HRESULT hr;
    _ASSERTE(!pbcReserved);
    
    m_fDidAsynchStart = TRUE;
    return S_OK;
}


/*
**  
**  Internal Implemenation of CXboxDataObject
**
*/


HRESULT CXboxDataObject::AreFormatsEquivalent(FORMATETC *pFormatEtc1, FORMATETC *pFormatEtc2)
/*++
  Routine Description:
   A static method to compare to FORTMATETC and determine equivalence.
   This should be sufficient for GetData, SetData, and QueryGetData.
   The only caveat is checking lindex.  For these, specific knowledge
   of the instance must be taken into account.  So for CF_FILECONTENT
   we just ignore it.
--*/
{
    if(pFormatEtc1->cfFormat != pFormatEtc2->cfFormat)
    {
        return DV_E_FORMATETC;
    }

    //
    //  CAVEAT: CF_FILECONTENTS can take a range of indices,
    //          we will FindMatchingGetFormat deals with this.
    //
    if(CF_FILECONTENTS!=pFormatEtc1->cfFormat)
    {
        if(pFormatEtc1->lindex != pFormatEtc2->lindex)
        {
            return DV_E_LINDEX;
        }
    }

    if(!(pFormatEtc1->tymed&pFormatEtc2->tymed))
    {
        return DV_E_TYMED;
        
    }
    if(pFormatEtc1->dwAspect != pFormatEtc2->dwAspect)
    {
        return DV_E_DVASPECT;
    }

    return S_OK;
}

HRESULT CXboxDataObject::FindMatchingGetFormat(FORMATETC *pFormatEtc, PUINT puIndex)
/*++
  Routine Description:
    Checks the sm_GetDataSupportedFormats for a matching FORMATETC.
--*/
{
    UINT uIndex = (m_ulShellAttributes&SFGAO_CANCOPY) ? 0 : (sm_uGetDataSupportedCount-1);
    HRESULT hrResult = S_OK;
    HRESULT hr;
    
    for(; uIndex < sm_uGetDataSupportedCount; uIndex++)
    {
        hr = AreFormatsEquivalent(&sm_GetDataSupportedFormats[uIndex].formatEtc, pFormatEtc);
        if(SUCCEEDED(hr))
        {
            //
            //  AreFormatsEquivalent cannot check the index for CF_FILECONTENTS
            //  so we do that here.
            //
            if(CF_FILECONTENTS==pFormatEtc->cfFormat)
            {
                if(!IsValidFileContentsIndex(pFormatEtc->lindex))
                {
                    return DV_E_LINDEX;
                }
            }
            if(puIndex)
            {
                *puIndex = uIndex;
            }
            hrResult = S_OK;
            break;
        } else
        {
            //
            //  This is a bit strange. If you look at winerror.h
            //  and the IDataObject::GetData or GetDataObject all the errors
            //  are not equavilent.  Presumable DV_E_FORMATETC implies that you
            //  do not support the format at all, whereas DV_E_ASPECT implies
            //  that the format is just fine, and it is just the you don't
            //  have the correct aspect ratio.  It turns out that the
            //  codes are laid out so that this works.
            //
            if(HRESULT_CODE(hr) > HRESULT_CODE(hrResult))
            {
                hrResult = hr;
            }
        }
    }
    return hrResult;
}
 

HRESULT CXboxDataObject::FindMatchingSetFormat(FORMATETC *pFormatEtc, PUINT puIndex)
/*++
  Routine Description:
    Checks the sm_SetDataSupportedFormats for a matching FORMATETC.
--*/
{
    UINT uIndex;
    HRESULT hrResult = S_OK;
    HRESULT hr;
    
    for(uIndex = 0; uIndex < sm_uSetDataSupportedCount; uIndex++)
    {
        hr = AreFormatsEquivalent(&sm_SetDataSupportedFormats[uIndex].formatEtc, pFormatEtc);
        if(SUCCEEDED(hr))
        {
            if(puIndex)
            {
                *puIndex = uIndex;
            }
            hrResult = S_OK;
            break;
        } else
        {
            if(HRESULT_CODE(hr) > HRESULT_CODE(hrResult))
            {
                hrResult = hr;
            }
        }
    }
    return hrResult;
}


class CCreateXboxFileGroupDesc : public IXboxVisitor
{
  public:
    virtual void VisitRoot         (IXboxVisit *pRoot,                DWORD *pdwFlags){_ASSERTE(FALSE);}
    virtual void VisitAddConsole   (IXboxVisit *pAddConsole,          DWORD *pdwFlags){_ASSERTE(FALSE);}
    virtual void VisitConsole      (IXboxConsoleVisit *pConsole,    DWORD *pdwFlags){_ASSERTE(FALSE);}
    virtual void VisitVolume       (IXboxVolumeVisit *pVolume,     DWORD *pdwFlags){_ASSERTE(FALSE);}
    virtual void VisitDirectoryPost (IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags){_ASSERTE(FALSE);}
    virtual void VisitFileOrDir    (IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags);

    PXBOXFILEGROUPDESCRIPTOR GetXboxGroupDesc(UINT uTopLevelCount, CXboxFolder *m_pSelection);
  
  private:
    struct XBOX_FILE_ITEM
    {
      DM_FILE_ATTRIBUTES dmFileAttributes;    
      XBOX_FILE_ITEM    *pNext;
    };

    UINT            m_uCount;
    XBOX_FILE_ITEM *m_pListHead;
    XBOX_FILE_ITEM *m_pListTail;
};


void CCreateXboxFileGroupDesc::VisitFileOrDir(IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags)
{
    XBOX_FILE_ITEM *pNewFileItem = new XBOX_FILE_ITEM;
    if(pNewFileItem)
    {
       pFileOrDir->GetFileAttributes(&pNewFileItem->dmFileAttributes);
       //
       //   The name in dmFileAttributes is the simple name, we want
       //   to replace it with the name relative to the point of recursion,
       //   which GetName returns.
       //
       pFileOrDir->GetName(pNewFileItem->dmFileAttributes.Name);

       pNewFileItem->pNext = NULL;
       if(!m_uCount)
       {
           m_pListHead = pNewFileItem;
       } else
       {
           m_pListTail->pNext = pNewFileItem; 
       }
       m_pListTail = pNewFileItem;
       m_uCount++;
    }
}

PXBOXFILEGROUPDESCRIPTOR CCreateXboxFileGroupDesc::GetXboxGroupDesc(UINT uTopLevelCount, CXboxFolder *pSelection)
{   
    PXBOXFILEGROUPDESCRIPTOR pXboxFileGroupDescriptor = NULL;
    UINT uIndex = 0;
    m_uCount = 0;
    m_pListHead = NULL;
    m_pListTail = NULL;

    //
    //  Build the list linked list of XBOX_FILE_ITEM's
    //
    pSelection->VisitEach(this, IXboxVisitor::FlagContinue|IXboxVisitor::FlagRecurse);

    //
    //  Allocate and fill out the XBOXFILEGROUPDESCRIPTOR
    //  
    if(m_pListHead)
    {
        //
        //  Calculate the size and allocate it.
        //
        UINT uSize = sizeof(XBOXFILEGROUPDESCRIPTOR) + (m_uCount-1)*sizeof(FILEDESCRIPTORA);
        pXboxFileGroupDescriptor = (PXBOXFILEGROUPDESCRIPTOR)malloc(uSize);
        if(pXboxFileGroupDescriptor)
        {
            //
            //  Fill in the header content
            //

            pXboxFileGroupDescriptor->cItems = m_uCount;
            pSelection->GetPath(pXboxFileGroupDescriptor->szFolderPath);
            if(!uTopLevelCount)
            {
                LPSTR pszParse = strrchr(pXboxFileGroupDescriptor->szFolderPath,'\\');
                *pszParse = '\0';
            }
        }

        //
        //  Go through our list of item, add them to our XBOXFILEGROUPDESCRIPTOR,
        //  if we managed to allocate it, and then free each item.
        //
        while(m_pListHead)
        {
            XBOX_FILE_ITEM *pFileItem;
            //
            //  Pop a file item off the list
            //
            pFileItem = m_pListHead;
            m_pListHead = pFileItem->pNext;
            if(pXboxFileGroupDescriptor)
            {
                pXboxFileGroupDescriptor->fgd[uIndex].dwFlags = FD_ATTRIBUTES|FD_CREATETIME|FD_FILESIZE|FD_WRITESTIME|FD_PROGRESSUI;
                strcpy(pXboxFileGroupDescriptor->fgd[uIndex].cFileName, pFileItem->dmFileAttributes.Name);
                pXboxFileGroupDescriptor->fgd[uIndex].dwFileAttributes = pFileItem->dmFileAttributes.Attributes;
                pXboxFileGroupDescriptor->fgd[uIndex].ftCreationTime = pFileItem->dmFileAttributes.CreationTime;
                pXboxFileGroupDescriptor->fgd[uIndex].ftLastWriteTime = pFileItem->dmFileAttributes.ChangeTime;
                pXboxFileGroupDescriptor->fgd[uIndex].nFileSizeHigh = pFileItem->dmFileAttributes.SizeHigh;
                pXboxFileGroupDescriptor->fgd[uIndex].nFileSizeLow = pFileItem->dmFileAttributes.SizeLow;
                uIndex++;
            }
            delete pFileItem;
        }
    }
    return pXboxFileGroupDescriptor;
}



PXBOXFILEGROUPDESCRIPTOR CXboxDataObject::GetXboxFileGroupDescriptor()
{
    //
    //  If the data is NOT already rendered, 
    //  use our helper class to create it.
    //
    if(!m_pXboxFileGroupDescriptor)
    {
        CCreateXboxFileGroupDesc createXboxFileGroupDesc;    
        m_pXboxFileGroupDescriptor = createXboxFileGroupDesc.GetXboxGroupDesc(
                                        m_uTopLevelItemCount,
                                        m_pSelection);
    }
    return m_pXboxFileGroupDescriptor;    
}


bool CXboxDataObject::IsValidFileContentsIndex(UINT uIndex)
/*++
  Routine Description:
    Checks if an index is valid for getting CF_FILECONTENTS

--*/
{
    PXBOXFILEGROUPDESCRIPTOR pXboxGroupFileDescriptor;
    pXboxGroupFileDescriptor = GetXboxFileGroupDescriptor();
    if(pXboxGroupFileDescriptor)
    {
        if(uIndex < pXboxGroupFileDescriptor->cItems)
        {
            if(!(pXboxGroupFileDescriptor->fgd[uIndex].dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
            {
                return true;
            }
        }
    }
    return false;
}

HRESULT CXboxDataObject::CopyToHGlobal(STGMEDIUM *pstgm, PVOID pvData, UINT cbSize, BOOL fHere)
{
    //
    // Allocate global memory to store the copy we are returning
    //
    if(fHere)
    {
        if(cbSize > GlobalSize(pstgm->hGlobal))
        {
            return STG_E_MEDIUMFULL;
        }
    } else
    {
        pstgm->hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, cbSize);
        if (pstgm->hGlobal == NULL)
        {
            return STG_E_MEDIUMFULL;
        }
        pstgm->tymed = TYMED_HGLOBAL;
        pstgm->pUnkForRelease = NULL;
    }

    //
    // Copy the Data Object's XBOXFILEGROUPDESCRIPTOR into the
    // global memory we just allocated.
    //
    PVOID pvGlobal = GlobalLock(pstgm->hGlobal);
    memcpy(pvGlobal, pvData, cbSize);
    GlobalUnlock(pstgm->hGlobal);
    return S_OK;
}

HRESULT CXboxDataObject::GetXboxDescriptor(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fHere)
{

    PXBOXFILEGROUPDESCRIPTOR pXboxGroupFileDescriptor = GetXboxFileGroupDescriptor();

    if(!pXboxGroupFileDescriptor)
    {
        return E_OUTOFMEMORY;
    }
    
    _ASSERT(pfetc->tymed&TYMED_HGLOBAL);
        
    //
    //  Calculate the size of the structure
    //

    UINT cItems = pXboxGroupFileDescriptor->cItems;
    UINT cbSize = sizeof(XBOXFILEGROUPDESCRIPTOR) + ((cItems-1)*sizeof(FILEDESCRIPTORA));

    //
    //  Copy the data to the stream.
    //

    return CopyToHGlobal(pstgm, (PVOID)pXboxGroupFileDescriptor, cbSize, fHere);
};

HRESULT CXboxDataObject::GetFileDescriptorA(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fHere)
{
    PXBOXFILEGROUPDESCRIPTOR pXboxGroupFileDescriptor = GetXboxFileGroupDescriptor();
    if(!pXboxGroupFileDescriptor)
    {
        return E_OUTOFMEMORY;
    }

    UINT uIndex;
    //
    //  Calculate the size of the structure
    //
    UINT cItems = pXboxGroupFileDescriptor->cItems;
    UINT cbSize = sizeof(FILEGROUPDESCRIPTORA) + ((cItems-1)*sizeof(FILEDESCRIPTORA));

    //
    //  Allocate the medium
    //
    if(fHere)
    {
        if(cbSize > GlobalSize(pstgm->hGlobal))
        {
            return STG_E_MEDIUMFULL;
        }
    } else
    {
        pstgm->hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, cbSize);
        if (pstgm->hGlobal == NULL)
        {
            return STG_E_MEDIUMFULL;
        }
        pstgm->tymed = TYMED_HGLOBAL;
        pstgm->pUnkForRelease = NULL;
    }
    
    //
    // Copy the Data Object's XBOXFILEGROUPDESCRIPTOR into the
    // global memory we just allocated.
    //
    FILEGROUPDESCRIPTORA *pfgda = (FILEGROUPDESCRIPTORA *)GlobalLock(pstgm->hGlobal);
    pfgda->cItems = cItems;
    UINT cbCopySize = cItems*sizeof(FILEDESCRIPTORA);
    memcpy(pfgda->fgd, pXboxGroupFileDescriptor->fgd, cbCopySize);
    GlobalUnlock(pstgm->hGlobal);
    
    return S_OK;

}
HRESULT CXboxDataObject::GetFileDescriptorW(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fHere)
{
    
    PXBOXFILEGROUPDESCRIPTOR pXboxGroupFileDescriptor = GetXboxFileGroupDescriptor();

    if(!pXboxGroupFileDescriptor)
    {
        return E_OUTOFMEMORY;
    }

    UINT uIndex;
    //
    //  Calculate the size of the structure
    //
    UINT cItems = pXboxGroupFileDescriptor->cItems;
    UINT cbSize = sizeof(FILEGROUPDESCRIPTORW) + ((cItems-1)*sizeof(FILEDESCRIPTORW));

    //
    //  Allocate the medium
    //
    if(fHere)
    {
        if(cbSize > GlobalSize(pstgm->hGlobal))
        {
            return STG_E_MEDIUMFULL;
        }
    } else
    {
        pstgm->hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, cbSize);
        if (pstgm->hGlobal == NULL)
        {
            return STG_E_MEDIUMFULL;
        }
        pstgm->tymed = TYMED_HGLOBAL;
        pstgm->pUnkForRelease = NULL;
    }
    
    //
    // Copy the Data Object's XBOXFILEGROUPDESCRIPTOR into the
    // global memory we just allocated.
    //
    FILEGROUPDESCRIPTORW *pfgdw = (FILEGROUPDESCRIPTORW *)GlobalLock(pstgm->hGlobal);
    pfgdw->cItems = cItems;
    for(uIndex = 0; uIndex < cItems; uIndex++)
    {
        pfgdw->fgd[uIndex].dwFlags          = pXboxGroupFileDescriptor->fgd[uIndex].dwFlags;
        pfgdw->fgd[uIndex].clsid            = pXboxGroupFileDescriptor->fgd[uIndex].clsid;
        pfgdw->fgd[uIndex].sizel            = pXboxGroupFileDescriptor->fgd[uIndex].sizel;
        pfgdw->fgd[uIndex].pointl           = pXboxGroupFileDescriptor->fgd[uIndex].pointl;
        pfgdw->fgd[uIndex].dwFileAttributes = pXboxGroupFileDescriptor->fgd[uIndex].dwFileAttributes;
        pfgdw->fgd[uIndex].ftCreationTime   = pXboxGroupFileDescriptor->fgd[uIndex].ftCreationTime;
        pfgdw->fgd[uIndex].ftLastAccessTime = pXboxGroupFileDescriptor->fgd[uIndex].ftLastAccessTime;
        pfgdw->fgd[uIndex].ftLastWriteTime  = pXboxGroupFileDescriptor->fgd[uIndex].ftLastWriteTime;
        pfgdw->fgd[uIndex].nFileSizeHigh    = pXboxGroupFileDescriptor->fgd[uIndex].nFileSizeHigh;
        pfgdw->fgd[uIndex].nFileSizeLow     = pXboxGroupFileDescriptor->fgd[uIndex].nFileSizeLow;
        wsprintfW(pfgdw->fgd[uIndex].cFileName, L"%hs", pXboxGroupFileDescriptor->fgd[uIndex].cFileName);
    }
    GlobalUnlock(pstgm->hGlobal);
    
    return S_OK;
}

HRESULT CXboxDataObject::GetFileContents(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fHere)
{
    PXBOXFILEGROUPDESCRIPTOR pXboxGroupFileDescriptor = GetXboxFileGroupDescriptor();
    
    if(!pXboxGroupFileDescriptor)
    {
        return E_OUTOFMEMORY;
    }
    if(fHere)
    {
        return E_NOTIMPL;
    }   
    
    HRESULT hr;
    char szTargetFile[MAX_PATH];
    
    //
    //  Get a temporary file name, since we pass false to open
    //  the file is not actually created, we just get the name.
    //  (The return value is zero for success, INVALID_HANDLE_VALUE
    //   on failure.)
    if(INVALID_HANDLE_VALUE==WindowUtils::CreateTempFile(szTargetFile, false))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    //
    //  Bring the file across the wire and save it to our temp file
    //
    hr = m_pSelection->ReceiveFile(pXboxGroupFileDescriptor->fgd[pfetc->lindex].cFileName, szTargetFile);
    if(SUCCEEDED(hr))
    {
        //
        //  The file is now in a temp file.  We just need
        //  to create a stream on top if it.
        //
        // Create the object that will handle the caller's streaming requests (read, etc).
        CXboxStream *pxboxstream = new CXboxStream;
        hr = pxboxstream->Initialize(szTargetFile);
        if (SUCCEEDED(hr))
        {
            // Point the storage medium at our newly created IStream derivative.
            pstgm->tymed = TYMED_ISTREAM;
            pstgm->pstm = pxboxstream;
            pstgm->pUnkForRelease = NULL;
        }
    } else
    // It is important to translate the error codes, such that windows gives
    // a sensible error code.
    {
        hr = Utils::XboxErrorToWindowsError(hr);
    }
    return hr;
}

class CCreateShellIdList : public IXboxVisitor
{
  public:
    virtual void VisitRoot         (IXboxVisit          *pItem, DWORD *pf){return Visit(pItem,pf);}
    virtual void VisitAddConsole   (IXboxVisit          *pItem, DWORD *pf){return Visit(pItem,pf);}
    virtual void VisitConsole      (IXboxConsoleVisit   *pItem, DWORD *pf){return Visit(pItem,pf);}
    virtual void VisitVolume       (IXboxVolumeVisit    *pItem, DWORD *pf){return Visit(pItem,pf);}
    virtual void VisitFileOrDir    (IXboxFileOrDirVisit *pItem, DWORD *pf){return Visit(pItem,pf);}
    virtual void VisitDirectoryPost (IXboxFileOrDirVisit *,DWORD *){_ASSERTE(FALSE);}

    void Visit(IXboxVisit *pItem, DWORD *pf);

    HRESULT GetShellIdList(UINT uTopLevelCount, CXboxFolder *pSelection, STGMEDIUM *pstgm, BOOL fHere);
  
  private:

    UINT             m_uCount;
    LPITEMIDLIST    *m_rgPidls;
    UINT             m_uTotalPidlSize;
    HRESULT          m_hr;
};

void CCreateShellIdList::Visit(IXboxVisit *pItem, DWORD *pf)
{
    m_rgPidls[m_uCount] = pItem->GetPidl(CPidlUtils::PidlTypeSimple);
    if(m_rgPidls[m_uCount])
    {
      m_uTotalPidlSize += CPidlUtils::GetLength(m_rgPidls[m_uCount]);
      m_uCount++;
    } else
    {
      *pf = 0;
      m_hr = E_OUTOFMEMORY;
    } 
}

HRESULT CCreateShellIdList::GetShellIdList(UINT uTopLevelCount, CXboxFolder *pSelection, STGMEDIUM *pstgm, BOOL fHere)
{
    CIDA *pCida = NULL;
    UINT  uIndex;
    UINT uPidlLen;
    UINT nNextPidlOffset;
    m_hr = S_OK;
    m_uCount = 0;
    m_uTotalPidlSize = 0;
    
    //
    //  Get the pidl of the parent folder
    //
    LPITEMIDLIST pParentPidl = pSelection->GetPidl(CPidlUtils::PidlTypeAbsolute);
    if(!pParentPidl)
    {
        return E_OUTOFMEMORY;
    }

    uPidlLen = CPidlUtils::GetLength(pParentPidl);
    m_uTotalPidlSize += uPidlLen;
    //
    //  If the pSelection is for itself (not one of its children
    //  then we need its parent.)
    //
    if(!uTopLevelCount)
    {
        //
        //  Do surgery on the parent pidl, lob off the last item.
        //
        LPITEMIDLIST pParsePidl = CPidlUtils::LastItem(pParentPidl);
        pParsePidl->mkid.cb = 0;
        uTopLevelCount = 1;
    }

    m_rgPidls = new LPITEMIDLIST[uTopLevelCount];
    if(m_rgPidls)
    {
        pSelection->VisitEach(this, IXboxVisitor::FlagContinue);
        if(SUCCEEDED(m_hr))
        {
            UINT cbSize = sizeof(CIDA) + (m_uCount)*sizeof(UINT);
            cbSize += m_uTotalPidlSize;

            //
            //  Allocate the storage medium
            //
            if(fHere)
            {
                if(cbSize > GlobalSize(pstgm->hGlobal))
                {
                    m_hr = STG_E_MEDIUMFULL;
                } else
                {
                    pCida = (CIDA *)GlobalLock(pstgm->hGlobal);
                }
            } else
            {
                pstgm->hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, cbSize);
                if (pstgm->hGlobal == NULL)
                {
                    m_hr = STG_E_MEDIUMFULL;
                } else
                {
                    pstgm->tymed = TYMED_HGLOBAL;
                    pstgm->pUnkForRelease = NULL;
                    pCida = (CIDA *)GlobalLock(pstgm->hGlobal);
                }
            }
        }
            
        //
        //  Copy the parent pidl.
        //
        if(pCida)
        {
            pCida->cidl = m_uCount;
            nNextPidlOffset = sizeof(CIDA) + m_uCount * sizeof(UINT);
            pCida->aoffset[0] = nNextPidlOffset;
            memcpy((BYTE*)pCida + nNextPidlOffset, pParentPidl, uPidlLen);
            nNextPidlOffset += uPidlLen;
        }
        //
        //  Free the parent pidl
        //  
        CPidlUtils::Free(pParentPidl);
        //
        //  Copy the other pidls, and free them.
        //
        for(uIndex = 0; uIndex < m_uCount; uIndex++)
        {
            if(pCida)
            {
                pCida->aoffset[uIndex+1] = nNextPidlOffset;
                uPidlLen = CPidlUtils::GetLength(m_rgPidls[uIndex]);
                memcpy((BYTE*)pCida + nNextPidlOffset, m_rgPidls[uIndex], uPidlLen);
                nNextPidlOffset += uPidlLen;
            }
            CPidlUtils::Free(m_rgPidls[uIndex]);
        }

        //
        //  Done with the storage medium.
        //
        if(pCida)
        {
            GlobalUnlock(pstgm->hGlobal);
        }

        //
        //  Done with the array for holding the pidls.
        //
        delete [] m_rgPidls;
    }
    return m_hr;
}


HRESULT CXboxDataObject::GetShellIdList(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fHere)
/*++
  Routine Description:
    Called to for CF_SHELLIDLIST.  It is much easier to do all this work a helper
    class.
--*/
{
    CCreateShellIdList createShellIdList;
    return createShellIdList.GetShellIdList(this->m_uTopLevelItemCount, m_pSelection, pstgm, fHere);
}

HRESULT CXboxDataObject::SetPreferredDropEffect(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fRelease)
{
    m_dwPreferredDropEffect = *((DWORD *)GlobalLock(pstgm->hGlobal));
    GlobalUnlock(pstgm->hGlobal);
    m_dwDropEffectFlags |= DROPEFFECT_PREFERERED_SET;
    return m_StoredFormats.Store(pfetc, pstgm, fRelease);
}

HRESULT CXboxDataObject::SetPerformedDropEffect(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fRelease)
{
    m_dwPerformedDropEffect = *((DWORD *)GlobalLock(pstgm->hGlobal));
    GlobalUnlock(pstgm->hGlobal);
    m_dwDropEffectFlags |= DROPEFFECT_PERFORMED_SET;
    return m_StoredFormats.Store(pfetc, pstgm, fRelease);
}

HRESULT CXboxDataObject::SetLogicalPerformedDropEffect(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fRelease)
{
    m_dwLogicalPerformedDropEffect = *((DWORD *)GlobalLock(pstgm->hGlobal));
    GlobalUnlock(pstgm->hGlobal);
    m_dwDropEffectFlags |= DROPEFFECT_LOGICALPERFORMED_SET;
    return m_StoredFormats.Store(pfetc, pstgm, fRelease);
}

HRESULT CXboxDataObject::SetPasteSucceeded(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fRelease)
{
    m_dwPerformedDropEffect = *((DWORD *)GlobalLock(pstgm->hGlobal));
    GlobalUnlock(pstgm->hGlobal);
    m_dwDropEffectFlags |= DROPEFFECT_PASTESUCCEEDED_SET;
    return m_StoredFormats.Store(pfetc, pstgm, fRelease);
}

/*
**  Implementation of CStoredDataFormatList
**
*/

HRESULT CStoredDataFormatList::Store(FORMATETC *pFormatEtc, STGMEDIUM *pstgm, BOOL fRelease)
/*++
  Routine Description:
    Stores data in a given format in our list.  If there is already data of that format,
    then it writes over it.
--*/
{
    //
    //  We can only store types that we can easily duplicate.
    //
    if(!(pstgm->tymed&(TYMED_HGLOBAL|TYMED_ISTREAM|TYMED_ISTORAGE)))
    {
        return DV_E_TYMED;
    }

    //
    //  Look for a match, if so we will need to overwrite it.
    //

    CStoredDataFormat *pDataFormat = FindMatch(pFormatEtc);
    if(pDataFormat)
    {
        ReleaseStgMedium(&pDataFormat->m_stgm);
    } else
    {
        //
        //  No match so allocate the item
        //
        pDataFormat = new CStoredDataFormat;
        if(pDataFormat)
        {
            pDataFormat->m_pNext = m_pHead;
            m_pHead = pDataFormat;
            m_uCount++;
        } else
        {
            return STG_E_MEDIUMFULL;
        }
    }
    
    //
    //  Blast the FORMATETC
    //
    memcpy(&pDataFormat->m_fetc, pFormatEtc, sizeof(FORMATETC));

    //
    //  If fRelease is set than just blast it over
    //
    if(fRelease)
    {
        memcpy(&pDataFormat->m_stgm, pstgm, sizeof(STGMEDIUM));
    }
    else
    //
    //  Otherwise, use CopyStgMedium, and be prepared
    //  for failure.
    {
        HRESULT hr = CopyStgMedium(pstgm, &pDataFormat->m_stgm);
        if(FAILED(hr))
        {
          //
          // Free the format
          //
          if(pDataFormat == m_pHead)
          {
            m_pHead = pDataFormat->m_pNext;
          } else
          {
            CStoredDataFormat *pPreviousFormat = m_pHead;
            while(pDataFormat != pPreviousFormat->m_pNext)
            {
                pPreviousFormat = pPreviousFormat->m_pNext;
            }
                pPreviousFormat->m_pNext = pDataFormat->m_pNext;
          }
          delete pDataFormat;
          m_uCount--;
          return hr;
        }
    }
    return S_OK;
}

CStoredDataFormat *CStoredDataFormatList::FindMatch(FORMATETC *pfetc)
/*++
  Routine Description:
    Finds stored data that matches the given format.
--*/
{
    CStoredDataFormat *pFormat = m_pHead;
    while(pFormat)
    {
        if(SUCCEEDED(CXboxDataObject::AreFormatsEquivalent(&pFormat->m_fetc, pfetc)))
        {
          break;
        }
        pFormat = pFormat->m_pNext;
    }
    return pFormat;
}

CStoredDataFormatList::~CStoredDataFormatList()
/*++
  Routine Description:
   Destroy the list of stored formats and their contents.
   CStoredDataFormat is really just a slightly glorified
   structure.  The list really owns destruction.
--*/
{
    while(m_pHead)
    {
        CStoredDataFormat *pFormat = m_pHead;
        m_pHead = pFormat->m_pNext;
        ReleaseStgMedium(&pFormat->m_stgm);
        delete pFormat;
    }
}

void CStoredDataFormat::CopyFormat(FORMATETC *pFormatEtc)
/*++
  Routine Description:
    This is used only when retrieving IEnumFORMATETC.  When
    building the array of available formats, the list of stored
    formats is walked, and this method is how the
    format is retrieved from the object.
  Arguments:
   pFormatEtc - buffer to receive format.
--*/
{
    memcpy(pFormatEtc, &m_fetc, sizeof(FORMATETC));
}

HRESULT CStoredDataFormat::CopyTo(STGMEDIUM *pstgm)
/*++
  Routine Description:
    Used by GetData to copy the data out of a stored storage medium
    into a new one.
--*/
{
    return CopyStgMedium(&m_stgm, pstgm);
}
