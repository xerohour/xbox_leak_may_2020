/*++

Copyright (c) Microsoft Corporation


Module Name:

    drop.h

Abstract:
    
    Declares the CXboxDropTarget class

Environment:

    Windows 2000
    Uses ATL

Notes:

Revision History:

    April 2001 - created by Mitchell Dernis (mitchd)
    July  2001 - major overhaul (mitchd)

--*/
#ifndef __XBOXDROP_H__
#define __XBOXDROP_H__

//------------------------------------------------------------------------
// CXboxDropTarget
//------------------------------------------------------------------------
class ATL_NO_VTABLE CXboxDropTarget : 
	public CComObjectRootEx<CComMultiThreadModel>,
    public CShellObjectWithSite,
    public IDropTarget
{
  public:

    /*
    ** c'tor and d'tor
    */
    CXboxDropTarget() : m_pSelection(NULL), m_hWnd(NULL), m_dwDefaultEffect(DROPEFFECT_NONE), m_grfKeyState(0) {}
    ~CXboxDropTarget(){if(m_pSelection) m_pSelection->Release(); m_pSelection = NULL;}
    static HRESULT Create(HWND hWndParent, UINT cidl, LPCITEMIDLIST * apidl, CXboxFolder *pParent, IDropTarget **ppDropTarget);
    
    /*
    **  ATL COM INTERFACE MAP
    */
    DECLARE_PROTECT_FINAL_CONSTRUCT()
    BEGIN_COM_MAP(CXboxDropTarget)
        COM_INTERFACE_ENTRY_IID(IID_IObjectWithSite, CShellObjectWithSite)
        COM_INTERFACE_ENTRY_IID(IID_IDropTarget, IDropTarget)
    END_COM_MAP()
  
  public:
    /*
    ** IDropTarget methods
    */
    STDMETHOD(DragEnter)(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
    STDMETHOD(DragOver)(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
    STDMETHOD(DragLeave)();
    STDMETHOD(Drop)(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
    
  protected:

    //
    //  Helper functions
    //
    DWORD DisplayDropPopUp(DWORD dwEffect, POINTL pt);
    DWORD GetEffect(DWORD grfKeyState, POINTL pt);
    BOOL  SetDefaultEffect(IDataObject *pDataObject);
    
    //
    //  Initialized by create
    //
    CXboxFolder *m_pSelection;  //The drop target selection.
    HWND         m_hWnd;        //The windw we are being dropped on.

    //
    //  The state of the drop target
    //
    DWORD     m_dwDefaultEffect; // Set on DragEnter.  This is the default drop effect for the source on the target
                                 // before taking into account keys and mouse buttons.  DROPEFFECT_NONE if the
                                 // the source and target are not compatible.
    DWORD     m_grfKeyState;   // We must cache this from DragEnter and DragOver, as Drop always gets
                               // grfKeyState = 0, I would say that this is probably a bug a in CDefaultView
                               // or some such.
};



struct HDROPFILE
{
    LPSTR pszFile;
    LPSTR pszRelativeFileName;
    HDROPFILE *pNext;
};
typedef struct HDROPFILE *PHDROPFILE; 

typedef struct NAMEMAPPING *PNAMEMAPPING;
struct NAMEMAPPING
{
    STGMEDIUM stgMedium;
    LPSTR     pszNames;
    LPWSTR    pwszNames;
};

#define FILE_PROGRESS_FUDGE_BYTES 65535 // Basically it means that the overhead of each file (or dir) is about equivalent to transfering 64 kbytes.
                                        // This was a trial and error value from looking at the initial time estimate displayed and increasing the
                                        // value until initial value was in the correct ballpark.  As I jacked this number up, the initial estimate
                                        // would get better and better.
class CDropOperation
{
  public:

    CDropOperation(
        CXboxFolder *pSelection,
        IDataObject *pDataObject,
        DWORD dwEffect,
        HWND hWnd
        ) : 
       m_pSelection(pSelection),   
       m_pDataObject(pDataObject),
       m_dwDesiredEffect(dwEffect),
       m_hWnd(hWnd),
       m_hParentWnd(hWnd),
       m_fAsync(FALSE),
       m_pMarshallingStream(0),
       m_pProgressDialog(NULL),
       m_pTargetConnection(NULL),
       m_uConfirmedYesToAll(0),
       m_uConfirmedNoToAll(0),
       m_fCancelled(FALSE),
       m_dwEffectPerformed(0)
       {
            m_pSelection->AddRef();
            m_pDataObject->AddRef();
       }
    ~CDropOperation();
    DWORD StartTransfer();
 
    static CLIPFORMAT GetDropFormat(IDataObject *pDataObject);
    static HRESULT GetDropData(IDataObject *pDataObject, FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium);

  private:

    //  
    //  Track which confirmation dialogs
    //  have been displayed with YesToAll
    //  or NoToAll.
    //
    enum CONFIRM_FLAGS
    {
        ConfirmFlagFileReplace           = 0x01,
        ConfirmFlagReadOnlyFileReplace   = 0x02,
        ConfirmFlagFolderReplace         = 0x04,
        ConfirmFlagReadOnlyFolderReplace = 0x08,
        ConfirmFlagFileMove              = 0x10,
        ConfirmFlagFolderMove            = 0x20
    };
    
    ///
    //  For Async
    //
    static DWORD ThreadProc(PVOID pvThis) {return reinterpret_cast<CDropOperation *>(pvThis)->TransferThread();}
    DWORD TransferThread();

    //
    //  The four main transfer types.
    //
    DWORD DoTransfer(); 
    void  DoXboxFileGroupDescriptorTransfer();
    void  DoHDropTransfer();
    void  DoFileGroupDescriptorWTransfer();
    void  DoFileGroupDescriptorATransfer();

    //
    //  Progress Dialogs
    //
    void StartProgressDialog();
    void SetProgressTarget();
    void UpdateCopyProgress(LPSTR pszFileName);
    void StopProgressDialog();
    BOOL QueryCancel();
    inline void ResetProgress() {if(m_pProgressDialog) m_pProgressDialog->Timer(PDTIMER_RESET,NULL);}

    //
    //  Confirm Dialogs
    //
    bool ConfirmFolderMove(LPCSTR pszFolderName);
    bool ConfirmFileMove(LPCSTR pszFileName);
    bool ConfirmFolderReplace(LPCSTR pszTargetWireName);
    bool ConfirmFileReplace(LPCSTR pszTargetWireName, FILEDESCRIPTORA *pFileDescriptor, bool *pfOverWrite);
    bool ConfirmFileReplace(LPCSTR pszTargetWireName, LPSTR pszFileName, WIN32_FILE_ATTRIBUTE_DATA *pFileAttributeData, bool *pfOverWrite);
    bool ProcessConfirmResponse(UINT uDialogResponse, CDropOperation::CONFIRM_FLAGS eConfirmFlag);

    //
    //  Prompts user that transfer failed for whatever reason, gives opportunity to cancel.
    //
    void HandleTransferFailed(UINT uResourceId, HRESULT hr, LPCSTR pszFilename);

    //
    //  File Transfer Help
    //
    bool MakeTargetDirectory(LPCSTR pszTargetWireName, FILEDESCRIPTORA *pFileDescriptor);
    bool MakeTargetDirectory(LPCSTR pszTargetWireName, DM_FILE_ATTRIBUTES *pSourceFileAttributes);


    //
    //  Converting and validating destination names for CF_HDROP
    //
    void InitNameMapping(IDataObject *pDataObject, PNAMEMAPPING pNameMapping);
    bool GetDestination(PNAMEMAPPING pNameMapping, PHDROPFILE pDropFile);
    void CleanupNameMapping(PNAMEMAPPING pNameMapping);
    void HDropRecurse(LPSTR pszTargetWireName, LPSTR szFullSource, DM_FILE_ATTRIBUTES *pdmFileAttributes, bool fMove);
    void CountDirContents(LPSTR pszDir, WIN32_FIND_DATAA *pFindData);
    void CalculateHDropWork(PHDROPFILE pHDropFiles);
    
    //
    //  Data members passed to the constructor
    //
    CXboxFolder     *m_pSelection;
    IDataObject     *m_pDataObject;
    DWORD            m_dwDesiredEffect;
    HWND             m_hWnd;
    HWND             m_hParentWnd;

    //
    //  Data members for do Async transfers
    //
    BOOL      m_fAsync; 
    IStream  *m_pMarshallingStream;

    //
    //  Progress Dialog
    //
    IProgressDialog *m_pProgressDialog;
    DWORD            m_dwFileCount;
    ULONGLONG        m_ullBytesCompleted;
    ULONGLONG        m_ullBytesTotal;
    //
    //  The data associated with the top level clipboard format:
    //  a.k.a. CF_HDROP, CF_FILEDESCRIPTOR, CF_XBOXFILEDESCRIPTOR, or
    //  CF_FILENAME.  Not CF_FILECONTENT.
    //
    STGMEDIUM m_stgMedium;
    
    //
    //  Connection for talking to the target box.
    //
    IXboxConnection *m_pTargetConnection;
    
    UINT m_uConfirmedYesToAll;  //Use the _CONFIRM_FLAGS
    UINT m_uConfirmedNoToAll;   //Use the _CONFIRM_FLAGS
    BOOL m_fCancelled;

    //
    //  This is filled out by the pfnPrepareCopy method.
    //
    DWORD           m_dwEffectPerformed;

    //
    //  Supported Format Table
    //
    static  const FORMATETC  sm_SupportedFormats[];
    static  const UINT       sm_uSupportedFormatCount;

};

#endif //__XBOXDROP_H__