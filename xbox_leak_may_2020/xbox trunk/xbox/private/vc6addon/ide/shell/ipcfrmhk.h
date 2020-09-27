// ipcfrmhk.h : interface of the CIPCompFrameHook class
//

#ifndef __IPCFRMHK_H__
#define __IPCFRMHK_H__

#include <oleipc.h>

extern UINT WM_RESETCOMMANDTARGET; // registered message sent to CIPCmpView

class CIPCompContainerItem;

/////////////////////////////////////////////////////////////////////////////
// CIPCompFrameHook 

class CIPCompFrameHook : public COleFrameHook
{

public:
	CIPCompFrameHook(CFrameWnd* pFrameWnd, CFrameWnd* pDocFrame, COleClientItem* pItem);
	~CIPCompFrameHook();

	virtual BOOL OnCompCmdMsg(GUID *pguid, DWORD cmdid, DWORD rgf,
		int nCode, void* pExtra);

	CFrameWnd *		m_pDocFrameWnd;
	
	IMsoCommandTarget * GetSupportingTarget(GUID * pguid, DWORD cmdid, DWORD rgf);
	void SetIPCComboEditText(CComboBox * pcombo, GUID * pguid, DWORD cmdid, DWORD rgf);
	void FillIPCCombo(CComboBox * pcombo, GUID * pguid, DWORD cmdid, DWORD rgf);
	void OnIPCComboSelect(CComboBox * pcombo, GUID * pguid, DWORD cmdid, DWORD rgf);
	void OnIPCComboEnter(CComboBox * pcombo, GUID * pguid, DWORD cmdid, DWORD rgf);

protected:
	COleRef<IOleInPlaceComponent> 	m_srpMainComp;
	COleRef<IMsoCommandTarget>		m_srpMainCmdTrgt;
	COleRef<IMsoCommandTarget>		m_srpActiveCmdTrgt;
	COleRef<IMsoCommandTarget>		m_srpPreContextMenuCmdTrgt;
	BOOL			m_fEnteringState;
	BOOL			m_fTopLevelContext;
	DWORD			m_dwCompRole;
	DWORD			m_dwRoleEnteringState;
	DWORD			m_grfComp;
	CLSID			m_clsidActive;
	
	//$UNDONE billmc -- only buttons supported so far, and no command
	// caching.
	//
	BOOL QueryCmdStatus(IMsoCommandTarget *pCmdTrgt, GUID *pguid, UINT cmdid, CCmdUI *pCmdUI, DWORD rgf);
	BOOL ExecCmd(IMsoCommandTarget *pCmdTrgt, GUID *pguid, UINT cmdid);
	void FillZoomCombo(CComboBox * pcombo, IMsoCommandTarget *pCmdTrgt);
	void FillFontNameCombo(CComboBox* pComboBox, IMsoCommandTarget* pCmdTrgt);
	void FillFontSizeCombo(CComboBox* pComboBox, LPCTSTR pszFontName);

protected:
	DECLARE_INTERFACE_MAP()

	// in addition, we need to implement IOleComponentUIManager, IOleInplaceComponentUIManager.
	BEGIN_INTERFACE_PART(OleCompUIMgr, IOleComponentUIManager)
		STDMETHOD(GetPredefinedStrings)(DISPID dispid, 
						CALPOLESTR *pcaStringsOut, CADWORD *pcaCookiesOut);
		STDMETHOD(GetPredefinedValue)(DISPID dispid, DWORD dwCookie, 
						VARIANT *pvarOut);
		STDMETHOD_(void, OnUIEvent)(DWORD dwCompRole, REFCLSID rclsidComp, 
						const GUID *pguidUIEventGroup, DWORD nUIEventId,
						DWORD dwUIEventStatus, 
						DWORD dwEventFreq, RECT *prcEventRegion, 
						VARIANT *pvarEventArg);
		STDMETHOD(OnUIEventProgress)(DWORD *pdwCookie, BOOL fInProgress,
						LPOLESTR pwszLabel, ULONG nComplete, ULONG nTotal);
		STDMETHOD(SetStatus)(LPCOLESTR pwszStatusText, 
						DWORD dwReserved);
		STDMETHOD(ShowContextMenu)(DWORD dwCompRole, REFCLSID rclsidActive,
	        			LONG nMenuId, POINTS &pos, 
						IMsoCommandTarget *pCmdTrgtActive);
		STDMETHOD(ShowHelp)(DWORD dwCompRole, REFCLSID rclsidComp,
						POINT posMouse, DWORD dwHelpCmd,
						LPOLESTR pszHelpFile, DWORD dwData);
		STDMETHOD(ShowMessage)(DWORD dwCompRole, REFCLSID rclsidComp,
						LPOLESTR pszTitle, LPOLESTR pszText,
						LPOLESTR pszHelpFile, DWORD dwHelpContextID,
						OLEMSGBUTTON msgbtn, OLEMSGDEFBUTTON msgdefbtn,
						OLEMSGICON msgicon, BOOL fSysAlert, LONG *pnResult);
	END_INTERFACE_PART(OleCompUIMgr)
	
	BEGIN_INTERFACE_PART(OleIPCompUIMgr, IOleInPlaceComponentUIManager)
	    STDMETHOD(UIActivateForMe)(DWORD dwCompRole, REFCLSID rclsidActive,
	        			IOleInPlaceActiveObject *pIPActObj, 
						IMsoCommandTarget *pCmdTrgtActive,
	        			ULONG cCmdGrpId, LONG *rgnCmdGrpId);
		STDMETHOD(UpdateUI)(DWORD dwCompRole, BOOL fImmediateUpdate, 
						DWORD dwReserved);
	    STDMETHOD(SetActiveUI)(DWORD dwCompRole, REFCLSID rclsid,
						ULONG cCmdGrpId, LONG *rgnCmdGrpId);
		STDMETHOD_(void,OnUIComponentEnterState)(DWORD dwCompRole, 
						DWORD dwStateId, DWORD dwReserved);
		STDMETHOD_(BOOL,FOnUIComponentExitState)(DWORD dwCompRole, 
						DWORD dwStateId, DWORD dwReserved);
		STDMETHOD_(BOOL,FUIComponentInState)(DWORD dwStateId);
		STDMETHOD_(BOOL,FContinueIdle)(THIS);
	END_INTERFACE_PART(OleIPCompUIMgr)
	
	BEGIN_INTERFACE_PART(ServiceProvider, IServiceProvider)
		STDMETHOD(QueryService) (REFGUID guidService, REFIID iid, void ** ppvObj);
	END_INTERFACE_PART(ServiceProvider)

public:
	virtual BOOL OnPreTranslateMessage(MSG* pMsg);
	virtual BOOL OnDocActivate(BOOL bActive);	// for OnDocWindowActivate
	
	IOleComponentUIManager * CIPCompFrameHook::GetIOleComponentUIManager();
	IOleInPlaceComponentUIManager * CIPCompFrameHook::GetIOleInPlaceComponentUIManager ();
	HRESULT SetInPlaceComponent(IUnknown * punkIPComp);
	void DoClose();

	// component manager related methods
	void OnEnterState(ULONG uStateId, BOOL fEnter);
	void OnActiveEnableModelss(ULONG uStateId, BOOL fEnter);
	void OnFrameActivate(BOOL fActivate);
	BOOL FDoIdle(DWORD grfidlef);
	BOOL FQueryClose(BOOL fPromptUser);

	BOOL ResetCommandTarget();
	
protected:
	//{{AFX_MSG(CIPCompFrameHook)
	//}}AFX_MSG
};

#endif	// __IPCFRMHK_H__
