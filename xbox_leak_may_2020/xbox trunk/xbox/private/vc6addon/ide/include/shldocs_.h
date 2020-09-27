/////////////////////////////////////////////////////////////////////////////
//	SHLDOCS_.H
//		Microsoft only document and view base classes for packages.

#ifndef __SHLDOCS__H__
#define __SHLDOCS__H__

#ifndef __OLEREF_H__
#include "oleref.h"
#endif

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

#ifndef _IPC_DOCUMENTS_DISABLED

/////////////////////////////////////////////////////////////////////////////
//	Classes defined in this header file

//	class CMultiDocTemplate;
//		class CPartTemplate;
			class CIPCompDocTemplate;
//	class COleDocument;
//		class CPartDoc;
			class CIPCompDoc;

/////////////////////////////////////////////////////////////////////////////
//	In-place component document classes
//		For Microsoft package use only.

class CIPCompContainerItem;
class CIPCompView;
class CIPCompSlob;
class COleUndoManager;
class COleParentUndoUnit;
interface IMsoCommandTarget;
interface IPropertyPageUndoString;
interface ISelectionContainer;

// These flag is an addition to the OLECMDROUTEFLAG set
// defined in oleipc.h.  They are returned in the pgrf
// parameter to OleCmdFromId with the routing flags.
//
// OLECMDROUTEFLAG_PACKAGEUPDATE indicates that the document
// wants to perform additional command UI updating if the in-place
// component responds that the command is enabled.
//
// OLECMDROUTEFLAG_SINGLESTATE indicates that this command is a single
// state button, i.e. that it is never checked or latched.

#define OLECMDROUTEFLAG_PACKAGEUPDATE 0x80000000
#define OLECMDROUTEFLAG_SINGLESTATE	  0x40000000

#define	ITS_STANDARD_SUPPORT	0x0001
#define	ITS_CUSTOM_SUPPORT		0x0002

static const DWORD MULTI_SELECT_COOKIE = 0x0001;

void ShowErrorForInterface(REFIID riid, IUnknown * pUnk, HRESULT hr);

/////////////////////////////////////////////////////////////////////////////
//	CIPCompDocTemplate
//		Document template base class for in-place component document support
//		in packages.

class CIPCompDocTemplate : public CPartTemplate
{
	DECLARE_DYNAMIC(CIPCompDocTemplate)

public:
	CIPCompDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CPackage* pPackage, CPacket* pPacket, REFCLSID clsid);

	virtual void InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc,
		BOOL bMakeVisible = TRUE);
};

/////////////////////////////////////////////////////////////////////////////
//	CIPCompDoc
//		Document base class for in-place component document support in
//		packages.

class CIPCompDoc : public CPartDoc
{
	DECLARE_DYNCREATE(CIPCompDoc)
	friend CIPCompView;

public:
	//{{AFX_VIRTUAL(CIPCompDoc)
	public:
	//}}AFX_VIRTUAL
	virtual BOOL NewDocumentHook();
	virtual BOOL OpenDocumentHook(LPCTSTR lpszPathName);
	virtual void CloseDocumentHook();

	virtual HRESULT OnAmbientInvoke(DISPID dispidMember,REFIID riid,LCID lcid,
	      				  unsigned short wFlags,DISPPARAMS *pdispparams,
	      				  VARIANT *pvarResult,EXCEPINFO *pexcepinfo,
	      				  unsigned int *puArgErr) 
						  {return NOERROR;}
						  
	//$CONSIDER -- do these methods logically belong on CPackage?
	//That would make it easier to eliminate the package's implementation
	//of a document class.  The drawback would be that it introduces
	//in-place component specific implementation into CPackage, which
	//doesn't know about in-place components.
	//
	virtual BOOL OleCmdFromId(UINT nID, GUID **ppguid, DWORD *pcmdid, DWORD *prgf);
	virtual POPDESC * PopupDescFromGuidId(REFGUID rguid, UINT nMenuId);
		
	CIPCompDoc ();
	virtual ~CIPCompDoc ();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	CIPCompContainerItem * GetCtnrItem()
		{return m_pItem;}
		
	IOleObject * GetOleObject();
	IStorage * GetStorage();

	CSlob*	GetSlob(); 
	void	SetSlob(CIPCompSlob *pIpCompSlob)
			{ m_pIpCompSlob = pIpCompSlob; }
	IUnknown *GetTrackSelection();
	IUnknown *GetUndoActionManager();
	LRESULT	GetUndoRedoString(WPARAM nLevel, LPARAM lpstr, UINT cchBuf, BOOL fUndo);

	// Override this to display Context menu if you component(forms96) doesnt 
	// display context menu in response to VK_F10.  
	virtual void ShowContextPopupMenu(CPoint pt) { };
	
	// Service Provider override
	virtual HRESULT	GetService(REFGUID guidService, REFIID riid, void **ppvObj);
	// allow the doc to disallow a request for a service
	// S_OK == disallow, S_FALSE == its OK to let them have it
	virtual HRESULT VetoService(REFGUID guidService, REFIID riid)
					{ return S_FALSE; }
	
	// Builder Wizard Manager overrides
    virtual HRESULT DoesBuilderExist(REFGUID rguidBuilder)
					{ return E_NOINTERFACE; }
	virtual HRESULT GetBuilder(REFGUID rguidBuilder, DWORD grfGetOpt, 
					HWND hwndPromptOwner, IDispatch **ppdispApp,
					HWND *pwndBuilderOwner, REFIID riidBuilder,
					IUnknown **ppunkBuilder)
					{ return E_NOINTERFACE; }

	virtual HRESULT ShowIPCHelp(DWORD dwCompRole, REFCLSID rclsidComp,
								POINT posMouse, DWORD dwHelpCmd,
								LPOLESTR pwszHelpFile, DWORD dwData);
	virtual void UpdateIPCToolbarControl(GUID * pguid, DWORD cmdid, DWORD rgf) {};

	void BeginUndo(LPCTSTR szDescription);
	BOOL SetUndoStringMgr(IPropertyPageUndoString *pPPUS);
	void EndUndo(BOOL bAbort = FALSE);

	virtual void OnDocActivate(BOOL bActivate)
		{ return; }
	void SetDocObjViewActivate(BOOL fActivate)
		{ m_fDocObjViewUIActive = fActivate; }
		
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void DoesSupportInternalSelect(DWORD* pgrfSupport)
					{
						*pgrfSupport = 0;
					}
	virtual HRESULT GetInternalSelectList(ISelectionContainer* pISC,
						CALPOLESTR* pcaStringsOut, CADWORD* pcaCookiesOut)
					{
						ASSERT(pISC);
						ASSERT(pcaStringsOut);
						ASSERT(pcaCookiesOut);

						// be a good citizen, initialize out params
						if (pcaStringsOut != NULL)
						{
							pcaStringsOut->cElems = 0;
							pcaStringsOut->pElems = NULL;
						}
						if (pcaCookiesOut != NULL)
						{
							pcaCookiesOut->cElems = 0;
							pcaCookiesOut->pElems = NULL;
						}

						if (NULL == pISC || NULL == pcaStringsOut ||
							NULL == pcaCookiesOut)
							return E_INVALIDARG;

						return E_NOTIMPL;
					}

public:
	//
	//	HRESULT OnPosRectChange
	//
	//	Description:
	//		Allows Document to resize itself, before resizing the OleClientItem.  
	//		Called from CIPCompContainerItem::XMyOleIPSite::OnPosRectChange.
	//		Returning an error, prevents the OleClientItem from being resized.
	//		Document should modify the rect if invalid.  This modified rect
	//		will be used from now on.
	//
	//	Arguments:
	//		LPRECT pPosRect:	IN  - suggested new size.  
	//							OUT - actuall new size.  
	//
	//	Return (HRESULT): S_OK if resized, error otherwise
	//
	//
	virtual HRESULT OnPosRectChange(LPRECT pPosRect) { return S_OK; };

	//
	//	void GetMinMaxInfo
	//
	//	Description:
	//		Allows document to specify MinMax info for the frame.
	//
	//	Arguments:
	//		MINMAXINFO * pMinMaxInfo: MINMAXINFO for frame
	//
	//	Return (void): none
	//
	virtual void GetMinMaxInfo(MINMAXINFO * pMinMaxInfo) {  };

	//
	//
	//	void OnWindowPosChanged
	//
	//	Description:
	//		Routes OnWindowPosChanged message to IP document
	//
	//	Arguments:
	//		WINDOWPOS FAR* lpwndpos:
	//
	//	Return (void):
	//
	virtual void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) {};
	
protected:
	CLSID m_clsid;	// the CLSID of the thing we are hosting
	CIPCompSlob	* m_pIpCompSlob;
	CIPCompContainerItem * m_pItem;	// the OleClientItem which provides the container site

	BOOL m_fDocObjViewUIActive;		// whether we have succeeded in create the doc object view
	
	COleRef<COleUndoManager>	 m_srpUndoMgr;
	COleRef<IPropertyPageUndoString> m_srpUndoStrMgr;
	CTypedPtrList<CPtrList, COleParentUndoUnit *> m_UndoStack;

	virtual BOOL InitOrLoadPersistence(IStorage * pStg, BOOL fNew);
	virtual BOOL NewOrLoadStorage(LPCTSTR pszPathName, IStorage ** ppStg, BOOL fNew);
	virtual void DoClose();

	// These functions are simply wrappers to similar functions
	// provided by the CIPCompFrameHook.  They give derived classes
	// access to those functions.
	//
	IMsoCommandTarget * GetSupportingTarget(GUID * pguid, DWORD cmdid, DWORD rgf);
	void SetIPCComboEditText(CComboBox * pcombo, GUID * pguid, DWORD cmdid, DWORD rgf);
	void FillIPCCombo(CComboBox * pcombo, GUID * pguid, DWORD cmdid, DWORD rgf);
	void OnIPCComboSelect(CComboBox * pcombo, GUID * pguid, DWORD cmdid, DWORD rgf);
	void OnIPCComboEnter(CComboBox * pcombo, GUID * pguid, DWORD cmdid, DWORD rgf);
	
protected:
	//{{AFX_MSG(CIPCompDoc)
	//}}AFX_MSG

	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);

	

	DECLARE_MESSAGE_MAP()
};
#endif

#undef AFX_DATA
#define AFX_DATA NEAR

#endif // __SHLDOCS__H__
