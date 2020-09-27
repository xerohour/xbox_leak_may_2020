// bldthread.h: interface for the CVCBuildThread class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <bldmarshal.h>

class CVCMarshallableEventHandler : 
	public IVCMarshallableEventHandler,
	public CComObjectRoot
{
public:
	CVCMarshallableEventHandler() {}
	~CVCMarshallableEventHandler() {}
	static HRESULT CreateInstance(IVCMarshallableEventHandler** ppHandler);

BEGIN_COM_MAP(CVCMarshallableEventHandler)
	COM_INTERFACE_ENTRY(IVCMarshallableEventHandler)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CVCMarshallableEventHandler) 

public:
	STDMETHOD(FireProjectBuildFinished)(IDispatch *pCfg, long errors, long warnings, VARIANT_BOOL bCancelled);
};

class CVCBuildThread : 
	public IVCBuildThread,
	public CComObjectRoot
{
public:
	CVCBuildThread();
	~CVCBuildThread();
	static HRESULT CreateInstance(IVCBuildThread** ppThread);
BEGIN_COM_MAP(CVCBuildThread)
	COM_INTERFACE_ENTRY(IVCBuildThread)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CVCBuildThread) 

// IVCBuildThread
public:
	STDMETHOD(InitializeForBuild)(IVCGenericConfiguration* pGenCfg, VCConfiguration* pProjCfg, ULONG celtFiles,
		VCFile* pFiles[], bldActionTypes bldType, BOOL bVerbose, BOOL bProvideBanner, IVCBuildEngine* pBuildEngine,
		IVCBuildErrorContext* pErrorContext);
	STDMETHOD(StartThread)();
	STDMETHOD(ExitThread)();
	STDMETHOD(Wait)();

public:
	void StopThread();
	void CleanupThread();

protected:
	static DWORD WINAPI BuildThread(LPVOID pvBldThread);

	DWORD m_dwThreadId;
	BOOL m_fThreadDie;

	bldActionTypes m_bldType;
	CComPtr<VCConfiguration> m_spProjCfg;
	VCFile** m_ppFiles;
	ULONG m_cFiles;
	BOOL m_bRecurse;
	BOOL m_bProvideBanner;

public:
	CComPtr<IVCGenericConfiguration> m_spGenCfg;
	CComPtr<IVCBuildErrorContext> m_spBuildErrorContext;
	CComPtr<IVCBuildEngine> m_spBuildEngine;
	CComPtr<IVCMarshallableEventHandler> m_spEventHandler;
	CBldMarshaller<IVCMarshallableEventHandler> m_marshalledEventHandler;

	HANDLE m_hThread;
	HANDLE m_hThreadExitEvent;
};
