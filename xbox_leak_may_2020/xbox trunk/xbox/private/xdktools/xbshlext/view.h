/*++

Copyright (c) Microsoft Corporation


Module Name:

    view.h

Abstract:
    
    Declaration of the various classes which implement the folder views.

Environment:

    Windows 2000
    Uses ATL
    Depends on Private shell32 and shdocvw APIs

Notes:

Revision History:

    March 2001 - created by Mitchell Dernis (mitchd)

--*/
#ifndef __XBOXVIEW_H__
#define __XBOXVIEW_H__

//-------------------------------------------------------------------------------
//  Declaration of CXboxViewCB
//-------------------------------------------------------------------------------
class ATL_NO_VTABLE CXboxViewCB : 
	public CComObjectRootEx<CComMultiThreadModel>,
    public CShellObjectWithSite,
    public IShellFolderViewCB
    CTRACKABLEOBJECT
{
 public:

  /*
  ** ATL COM INTERFACE MAP
  */

  DECLARE_PROTECT_FINAL_CONSTRUCT()
  BEGIN_COM_MAP(CXboxViewCB)
    COM_INTERFACE_ENTRY_IID(IID_IObjectWithSite, CShellObjectWithSite)
    COM_INTERFACE_ENTRY_IID(IID_IShellFolderViewCB, IShellFolderViewCB)
  END_COM_MAP()

 public:

  /*
  **  IShellFolderViewCB Method (one and only)
  */
 
  STDMETHOD(MessageSFVCB)(UINT uMsg, WPARAM wParam, LPARAM lParam);

  /*
  **
  **  IShellFolderViewCB is nothing more than dispatch type interface wrapping
  **  windows messages.  The implementation of MessageSFVCB cracks the applicable
  **  messages and calls the appropriate virtual functions listed below, some of
  **  which are pure virtual.
  */
 
  HRESULT OnDefItemCount(LPINT pi);
  HRESULT OnGetHelpText(int ici, LPWSTR pszHelpText, int cChar);
  HRESULT OnGetPane(DWORD dwPaneID, DWORD *pdwPane);
  HRESULT OnRefresh(BOOL fReload);
  HRESULT OnDidDragDrop(DWORD dwDropEffect, IDataObject * pdto);
  HRESULT OnGetDetailsOf(UINT ici, PDETAILSINFO pdi);
  HRESULT OnInvokeCommand(UINT idc);
  HRESULT OnGetNotify(LPITEMIDLIST * ppidl, LONG * lEvents);
  HRESULT OnFSNotify(LPITEMIDLIST  pidl, LONG lEvent);
  HRESULT OnThisIDList(LPITEMIDLIST * ppidl);
  HRESULT OnSetISFV(IShellFolderView *pShellFolderView);
  HRESULT OnColumnClick(UINT ici);
  HRESULT OnUpdateStatusBar(void);
  //NYI - but we really don't want the default behavior
  HRESULT OnAddPropertyPages(SFVM_PROPPAGE_DATA * pData) {return E_NOTIMPL;}
  HRESULT OnSize(LONG x, LONG y) {return SetStatusParts();}
  
  HRESULT OnMergeMenu(LPQCMINFO pqcm){return S_OK;}
  HRESULT OnUnMergeMenu(HMENU hMenu){return S_OK;}


  /*
  **  Utility Methods
  */
  HRESULT SetStatusParts();
  

  /*
  ** Construction related methods
  */

  static HRESULT CreateShellView(CXboxFolder *pXboxFolder, HWND hWnd, REFIID riid, void **ppv);
  CXboxViewCB() : m_pXboxFolder(NULL) {INIT_TRACKABLEOBJECT_NAME(CXboxViewCB);}
  ~CXboxViewCB()
  {
      if(m_pXboxFolder) m_pXboxFolder->Release();
  }
  
  protected:
    CXboxFolder       *m_pXboxFolder;    // The CXboxItem which this view is displaying
    HWND               m_hWnd;           // The Window of IShellView
};

#endif //__XBOXVIEW_H__