// EncBldThrd.h: interface for the CEncBldThrd class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CEncBldThrd  
{
public:
	BOOL FStart();
	CEncBldThrd();
	virtual ~CEncBldThrd();
	BOOL Init(LPCOLESTR szCmdLine, LPCOLESTR szCurrentDir, IVCBuildErrorContext* pIVCBuildErrorContext,
		IVCBuildEngine* pBuildEngine, IENCBuildSpawnerCallback *pENCBuildSpawnerCallback, BOOL fUseConsoleCP);
	void Wait();

	DWORD GetErrorCount() 
	{ return (DWORD)m_lErrors; }

	DWORD GetWarningCount()
	{ return (DWORD)m_lWarnings; }

	HRESULT GetHResult()
	{ return m_hResult; }

protected:
	static DWORD WINAPI EncBuildThread(LPVOID *pvGenCfg );
    BOOL CreateThread();

	CComBSTR m_szCmdLine;
	CComBSTR m_szCurrentDir;
	BOOL m_fUseConsoleCP;

    HANDLE m_hThread;
	DWORD m_dwThreadId;

	long m_lErrors;
	long m_lWarnings;
	HRESULT m_hResult;

	CComPtr<IENCBuildSpawnerCallback> m_pENCBuildSpawnerCallback;
	CComQIPtr<IVCBuildEngineImpl> m_spBuildEngineImpl;
	CComPtr<IVCBuildErrorContext> m_spBuildErrorContext;
	IStream *m_pStreamENCBuildSpawnerCallback;
};

