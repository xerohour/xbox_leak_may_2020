/*++

Copyright (c) Microsoft Corporation


Module Name:

    DataObj.h

Abstract:
    
    Declares the CXboxDataObject class

Environment:

    Windows 2000
    Uses ATL

Notes:

Revision History:

    April 2001 - created by Mitchell Dernis (mitchd)
    July  2001 - major overhaul (mitchd)

--*/
#ifndef __XBOXDATAOBJ_H__
#define __XBOXDATAOBJ_H__

//------------------------------------------------------------------------
//  Propertary clipboard format for Xbox files.
//
//  This clipboard format is an Xbox specific variation on
//  CFSTR_FILEDESCRIPTOR.  The primary change is the addition of the
//  machine name.  All the files described must be on the same machine.
//
//  There is enough information to obtain a CXboxItem in the target
//  process.
//
//  FORMATETC
//   cfFormat = CFXBOX_FILEDESCRIPTOR 
//   ptd      = NULL
//   dwAspect = DVASPECT_CONTENT
//   lindex   = 0
//   tymed    = TYMED_HGLOBAL
//
//  The contents of the HGLOBAL is CFXBOX_FILEORDIRECTORY.
//
//  
//------------------------------------------------------------------------
const TCHAR CFXBOX_FILEDESCRIPTOR[] = TEXT("CFXBOX_FILEDESCRIPTOR");
typedef struct _XBOXFILEGROUPDESCRIPTOR {
    UINT cItems;
    CHAR szFolderPath[MAX_PATH];
    FILEDESCRIPTORA fgd[1];         //Always ANSI
} XBOXFILEGROUPDESCRIPTOR, *PXBOXFILEGROUPDESCRIPTOR;

//--------------------------------------------------------------------------------
//  Forward Class Declarations
//--------------------------------------------------------------------------------
class CStoredDataFormat;
class CStoredDataFormatList;
class CXboxDataObject;

//--------------------------------------------------------------------------------
//  typedef a specialization of CComEnum for EnumFormatEtc.
//--------------------------------------------------------------------------------
typedef CComEnum<IEnumFORMATETC,&IID_IEnumFORMATETC,FORMATETC,_Copy<FORMATETC>,CComMultiThreadModel> CEnumFormatEtc;

//--------------------------------------------------------------------------------
//  Supported Formats Table
//--------------------------------------------------------------------------------

typedef HRESULT (CXboxDataObject::*PFN_GETDATA)(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fHere);
typedef struct _GET_DATA_FORMAT_TABLE_ENTRY
{
    FORMATETC   formatEtc;
    PFN_GETDATA pfnGetData;
} GET_DATA_FORMAT_TABLE_ENTRY, *PGET_DATA_FORMAT_TABLE_ENTRY;

typedef HRESULT (CXboxDataObject::*PFN_SETDATA)(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fRelease);
typedef struct _SET_DATA_FORMAT_TABLE_ENTRY
{
    FORMATETC   formatEtc;
    PFN_SETDATA pfnSetData;
} SET_DATA_FORMAT_TABLE_ENTRY, *PSET_DATA_FORMAT_TABLE_ENTRY;

//------------------------------------------------------------------------
// CStoredDataFormat - for storing generic formats
//------------------------------------------------------------------------
class CStoredDataFormat
{
  public:

    CStoredDataFormat *GetNext() {return m_pNext;}
    void               CopyFormat(FORMATETC *pFormatEtc);   //Only information about the format
    HRESULT            CopyTo(STGMEDIUM *pstgm);            //Retrieves the data.
    
  private:
    
    CStoredDataFormat():m_pNext(NULL){}
    ~CStoredDataFormat(){}

    FORMATETC m_fetc;
    STGMEDIUM m_stgm;
    
    CStoredDataFormat *m_pNext;
    friend class CStoredDataFormatList;    
};

class CStoredDataFormatList
{
 public:
  CStoredDataFormatList() : m_pHead(NULL), m_uCount(0){}
  ~CStoredDataFormatList();

  HRESULT           Store(FORMATETC *pFormatEtc, STGMEDIUM *pstgm, BOOL fRelease);
  CStoredDataFormat *FindMatch(FORMATETC *pfetc);
  UINT              GetCount() {return m_uCount;}
  CStoredDataFormat *GetFirst() {return m_pHead;}

 private:    
  CStoredDataFormat *m_pHead;
  UINT               m_uCount;
};

//------------------------------------------------------------------------
// CXboxDataObject
//------------------------------------------------------------------------
class ATL_NO_VTABLE CXboxDataObject : 
    public CComObjectRootEx<CComMultiThreadModel>,
    public IObjectWithSiteImpl<CXboxDataObject>,
    public IAsyncOperation,
    public IDataObject
{
 
  public:
    /*
    **  c'tor and d'tor
    */
    CXboxDataObject () : 
        m_pSelection(NULL),
        m_ulShellAttributes(0),
        m_fDidAsynchStart(FALSE),
        m_pXboxFileGroupDescriptor(NULL),
        m_uTopLevelItemCount(0),
        m_pUnkThread(NULL),
        m_dwPreferredDropEffect(0),
        m_dwPerformedDropEffect(0),
        m_dwLogicalPerformedDropEffect(0),
        m_dwPasteSucceeded(0),
        m_dwDropEffectFlags(0){}
    ~CXboxDataObject ();

    static HRESULT Create(UINT cidl, LPCITEMIDLIST * apidl, CXboxFolder *pParent, IDataObject **ppDataObject);
    static HRESULT Create(UINT cidl, CXboxFolder *pSelection, IDataObject **ppDataObject);

    /*
    ** ATL Interface Map
    */
    DECLARE_PROTECT_FINAL_CONSTRUCT()
    BEGIN_COM_MAP(CXboxDataObject)
     COM_INTERFACE_ENTRY_IID(IID_IObjectWithSite, IObjectWithSite)
     COM_INTERFACE_ENTRY_IID(IID_IDataObject, IDataObject)
     COM_INTERFACE_ENTRY_IID(IID_IAsyncOperation, IAsyncOperation)
    END_COM_MAP()

  public:
   
    /*
    ** IDataObject methods
    */
    STDMETHOD(GetData)(FORMATETC *pfmtetcIn, STGMEDIUM *pstgmed);
    STDMETHOD(GetDataHere)(FORMATETC *pfmtetc, STGMEDIUM *pstgpmed);
    STDMETHOD(QueryGetData)(FORMATETC *pfmtetc);
    STDMETHOD(GetCanonicalFormatEtc)(FORMATETC *pfmtetcIn, FORMATETC *pfmtetcOut);
    STDMETHOD(SetData)(FORMATETC *pfmtetc, STGMEDIUM *pstgmed, BOOL bRelease);
    STDMETHOD(EnumFormatEtc)(DWORD dwDirection, IEnumFORMATETC **ppienumFormatEtc);
    STDMETHOD(DAdvise)(FORMATETC *pfmtetc, DWORD dwAdviseFlags, IAdviseSink * piadvsink, DWORD * pdwConnection);
    STDMETHOD(DUnadvise)(DWORD dwConnection);
    STDMETHOD(EnumDAdvise)(IEnumSTATDATA **ppienumStatData);

    /*
    ** IAsyncOperation methods
    */
    STDMETHOD(EndOperation)(HRESULT hResult, IBindCtx *pbcReserved, DWORD dwEffects);
    STDMETHOD(SetAsyncMode)(BOOL fDoOpAsync){return E_NOTIMPL;}
    STDMETHOD(GetAsyncMode)(BOOL *pfIsOpAsync);
    STDMETHOD(InOperation)(BOOL *pfInAsyncOp);
    STDMETHOD(StartOperation)(IBindCtx *pbcReserved);

    //
    //  A generally useful utility function, ever so slightly customized with respect
    //  to CF_FILECONTENTS
    //
    static HRESULT AreFormatsEquivalent(FORMATETC *pFormatEtc1, FORMATETC *pFormatEtc2);

  protected:

    
    static HRESULT CopyToHGlobal(STGMEDIUM *pstgm, PVOID pvData, UINT cbSize, BOOL fHere);

    //
    //  Used by GetData, QueryGetData. 
    //

    HRESULT FindMatchingGetFormat(FORMATETC *pFormatEtc, PUINT puIndex = NULL);

    //
    //  Used by SetData.
    //

    HRESULT FindMatchingSetFormat(FORMATETC *pFormatEtc, PUINT puIndex = NULL);

    //
    //  Helpers for many of the GetXXX methods.
    //
    PXBOXFILEGROUPDESCRIPTOR GetXboxFileGroupDescriptor();
    bool IsValidFileContentsIndex(UINT uIndex);

    //
    //  Handle getting and setting data objects.
    //

    HRESULT GetXboxDescriptor(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fHere);
    HRESULT GetFileDescriptorA(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fHere);
    HRESULT GetFileDescriptorW(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fHere);
    HRESULT GetFileContents(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fHere);
    HRESULT GetShellIdList(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fHere);
    HRESULT SetPreferredDropEffect(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fRelease);
    HRESULT SetPerformedDropEffect(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fRelease);
    HRESULT SetLogicalPerformedDropEffect(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fRelease);
    HRESULT SetPasteSucceeded(FORMATETC *pfetc, STGMEDIUM *pstgm, BOOL fRelease);

    //
    //  Data
    //

    CXboxFolder             *m_pSelection;
    ULONG                    m_ulShellAttributes;
    BOOL                     m_fDidAsynchStart;
    PXBOXFILEGROUPDESCRIPTOR m_pXboxFileGroupDescriptor;
    UINT                     m_uTopLevelItemCount;
    CStoredDataFormatList    m_StoredFormats;
    IUnknown                *m_pUnkThread;
    
    DWORD   m_dwPreferredDropEffect;
    DWORD   m_dwPerformedDropEffect;
    DWORD   m_dwLogicalPerformedDropEffect;
    DWORD   m_dwPasteSucceeded;
    DWORD   m_dwDropEffectFlags;
    #define DROPEFFECT_PREFERERED_SET        0x01
    #define DROPEFFECT_PERFORMED_SET         0x02
    #define DROPEFFECT_LOGICALPERFORMED_SET  0x04
    #define DROPEFFECT_PASTESUCCEEDED_SET    0x08


    //
    //  static data
    //
    
    static GET_DATA_FORMAT_TABLE_ENTRY sm_GetDataSupportedFormats[];
    static UINT                        sm_uGetDataSupportedCount;

    static SET_DATA_FORMAT_TABLE_ENTRY sm_SetDataSupportedFormats[];
    static UINT                        sm_uSetDataSupportedCount;
};

#endif //__XBOXDATAOBJ_H__
