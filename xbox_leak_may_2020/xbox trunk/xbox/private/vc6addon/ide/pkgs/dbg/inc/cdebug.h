#ifndef _CDEBUG_H_
#define _CDEBUG_H_

#include <bldapi.h>

#if !(defined (OSDEBUG4))
#include <od.h>
#else
#include <od4.h>
#endif

enum {
	DLL_SH = 0,
	DLL_TL,
	DLL_EM,
	DLL_EE,
    DLL_NM
};

// Maximum number of NM DLLs we can load at once
#define MAX_NM_DLL	4


class CDebug : public CObject
{
public:
	CDebug ();
	~CDebug ();
	BOOL Init(BOOL fSrcInit);
	void UpdateMetrics (HPID hpid);
	void OnDebugStart (HPID hpid);
	void OnConfigChange ();
	void ConfigChange (uniq_platform upID);  
	BOOL GetComponents( const TCHAR *szNM, const TCHAR**szTL, const TCHAR **szEM, const TCHAR**szSH, const TCHAR**szEE );

private:
	BOOL	m_fInitialized;
	BOOL	m_fMtrcOleRpc;
	BOOL	m_fMtrcNativeDebugger;
	BOOL	m_fMtrcRemoteTL;
	BOOL	m_fMtrcHasThreads;
	BOOL	m_fMtrcExceptionHandling;
	BOOL	m_fMtrcMultipleInstances;
	BOOL	m_fQuickRestart;
	BOOL	m_fEditedByENC;
	END		m_endMtrcEndian;
	BPTS	m_bptsMtrcBreakPoints;
	ASYNC	m_asyncMtrcAsync;

	CString	m_strExeExtension;

	UINT	m_platform;

	HINSTANCE m_hInstTL;
	HINSTANCE m_hInstEM;
	HINSTANCE m_hInstSH;
	HINSTANCE m_hInstEE;
	HINSTANCE m_rghInstNM [MAX_NM_DLL];

	const TCHAR* m_szTLName;
	const TCHAR* m_szEMName;
	const TCHAR* m_szSHName;
	const TCHAR* m_szEEName;
	const TCHAR* m_rgszNMName [MAX_NM_DLL];

	UINT m_nNMCount;

public:
	BOOL	MtrcOleRpc ()			{ return m_fMtrcOleRpc; }
	BOOL	MtrcNativeDebugger ()	{ return m_fMtrcNativeDebugger; }
	BOOL	MtrcRemoteTL ()			{ return m_fMtrcRemoteTL; }
	BOOL	MtrcHasThreads ()		{ return m_fMtrcHasThreads; }
	BOOL	MtrcFirstChanceExc ()	{ return m_fMtrcExceptionHandling; }
	BOOL	MtrcMultipleInstances()	{ return m_fMtrcMultipleInstances; }
	END		MtrcEndian ()			{ return m_endMtrcEndian; }
	BPTS	MtrcBreakPoints ()		{ return m_bptsMtrcBreakPoints; }
	ASYNC	MtrcAsync ()			{ return m_asyncMtrcAsync; }

	CString	GetExeExtension ()		{ return m_strExeExtension; }

	UINT	GetPlatform()			{ return m_platform; }

	BOOL	SupportsQuickRestart() const;
	void	SetQuickRestart (BOOL	f) { m_fQuickRestart = f; }
	BOOL	IsQuickRestart () { return m_fQuickRestart && !m_fEditedByENC; };
	void	SetEditedByENC (BOOL	f) { m_fEditedByENC = f; }
	BOOL	IsEditedByENC () { return m_fEditedByENC; }

	BOOL	LoadComponent(int DLLtype, int nWhichOne = -1);
	BOOL	UnLoadComponent(int DLLtype, int nWhichOne = -1);

	BOOL	InitializeComponent(int DLLtype, int nWhichOne = -1);
	BOOL	UnInitializeComponent(int DLLtype, int nWhichOne = -1);

	BOOL	InitOSD(VOID);
	BOOL	UnInitOSD(VOID);

	BOOL	InitDBF();
	BOOL	UnInitDBF();
	

	void	SetTLName(const TCHAR* szNewTLName)
									{ m_szTLName = szNewTLName; }
	
	BOOL	IsMacintosh() const;
	BOOL	IsJava() const;
};

extern CDebug *pDebugCurr;

BOOL
IsCrashDump(
	);

enum {
	FUNCTION_NORMAL,
	FUNCTION_NOSTEPINTO
};


class CExecutionExts
{
	THashedMapOf <CString, DWORD>	m_map;

  public:

	BOOL
	Enter(
		LPCSTR	str,
		DWORD	status
		)
	{
		return m_map.Insert (str, status);
	}
	
	BOOL
	IsNoStepInto(
		CString	strFunctionName
		)
	{
		DWORD	dw = 0;
		int		index = -1;

		//
		// Search for the full name: Foo::Bar
		//
		
		if (m_map.Find (strFunctionName, &dw)) {
			return (dw == FUNCTION_NOSTEPINTO);
		}

		//
		// Also check for all methods of this class: Foo::*
		//
		
		index = strFunctionName.Find ("::");

		if (index != -1) {
			strFunctionName = strFunctionName.Left (index + 2);
			strFunctionName += '*';

			if (m_map.Find (strFunctionName, &dw)) {
				return (dw == FUNCTION_NOSTEPINTO);
			}
		}

		return FALSE;
	}

};


extern CExecutionExts		g_ExecutionExts;
	

#endif	// _CDEBUG_H_


