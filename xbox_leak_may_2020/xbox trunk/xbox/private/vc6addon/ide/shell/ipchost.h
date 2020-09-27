
// ipchost.h : main header file IMsoComponent and IMsoComponentHost
//

#ifndef __IPCHOST_H__
#define __IPCHOST_H__

#include <cmpmgr.h>

class CIPCompMainFrame : public CCmdTarget
{
public:
	CIPCompMainFrame();
	BOOL SetComponentManager(IMsoComponentManager * pCmpMgr);
	void ReleaseComponentManager();
	IMsoComponent * CIPCompMainFrame::GetIMsoComponent()
		{return &m_xMsoComponent; }

	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(MsoComponent, IMsoComponent)
	    MSODEBUGMETHODIMP
		MSOMETHOD_(BOOL, FPreTranslateMessage) (MSG *pMsg);
		MSOMETHOD_(void, OnEnterState) (ULONG uStateId, BOOL fEnter);
		MSOMETHOD_(void, OnAppActivate) (BOOL fActivate, DWORD dwOtherThreadID);
		MSOMETHOD_(void, OnLoseActivation) ();
		MSOMETHOD_(void, OnActivationChange) (
							IMsoComponent *pic, 
							BOOL fSameComponent,
							const MSOCRINFO *pcrinfo,
							BOOL fHostIsActivating,
							const MSOCHOSTINFO *pchostinfo,
							DWORD dwReserved);
		MSOMETHOD_(BOOL, FDoIdle) (DWORD grfidlef);
		MSOMETHOD_(BOOL, FContinueMessageLoop) (
							ULONG uReason,
							void* pvLoopData,
							MSG *pMsgPeeked);
		MSOMETHOD_(BOOL, FQueryTerminate) (BOOL fPromptUser);
		MSOMETHOD_(void, Terminate) ();
		MSOMETHOD_(HWND, HwndGetWindow) (DWORD dwWhich, DWORD dwReserved);
	END_INTERFACE_PART(MsoComponent)

public:
	COleRef<IMsoComponentManager> m_srpMsoCompMgr;
	DWORD m_dwComponentID;
};

class CIPCompHostApp : public CCmdTarget
{
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(MsoComponentHost, IMsoComponentHost)
		MSOMETHOD(QueryService) (REFGUID guidService, REFIID iid, void **ppvObj);

		MSODEBUGMETHODIMP
		MSOMETHOD_(void, OnComponentActivate) (IMsoComponent *pic, 
							const MSOCRINFO *pcrinfo, BOOL fTracking);
		MSOMETHOD_(void, OnComponentEnterState) (ULONG uStateId, BOOL fEnter);
		MSOMETHOD_(BOOL, FContinueIdle) ();
		MSOMETHOD_(BOOL, FPushMessageLoop) (ULONG uReason);
	END_INTERFACE_PART(MsoComponentHost)

public:
	CIPCompHostApp();
	IMsoComponentHost * CIPCompHostApp::GetIMsoComponentHost()
		{return &m_xMsoComponentHost; }
};

class CIPCompMessageFilter : public COleMessageFilter
{
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(ServiceProvider, IServiceProvider)
		STDMETHOD(QueryService) (REFGUID guidService, REFIID iid, void ** ppvObj);
	END_INTERFACE_PART(ServiceProvider)

public:
	CIPCompMessageFilter();
};

#endif //__IPCHOST_H__
