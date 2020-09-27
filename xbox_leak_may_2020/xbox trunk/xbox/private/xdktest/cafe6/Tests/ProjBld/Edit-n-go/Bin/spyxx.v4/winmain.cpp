// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"

#ifdef AFX_CORE1_SEG
#pragma code_seg(AFX_CORE1_SEG)
#endif

/////////////////////////////////////////////////////////////////////////////
// Standard WinMain implementation
//  Can be replaced as long as 'AfxWinInit' is called first

#ifndef _USRDLL
#ifdef _MAC
extern "C" int PASCAL
#else
extern "C" int WINAPI
#endif
_tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	ASSERT(hPrevInstance == NULL);

	int nReturnCode = -1;
	CWinApp* pApp = AfxGetApp();

	// AFX internal initialization
	if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
		goto InitFailure;

	// App global initializations (rare)
	ASSERT_VALID(pApp);
	if (!pApp->InitApplication())
		goto InitFailure;
	ASSERT_VALID(pApp);

	// Perform specific initializations
	if (!pApp->InitInstance())
	{
		if (pApp->m_pMainWnd != NULL)
		{
			TRACE0("Warning: Destroying non-NULL m_pMainWnd\n");
			pApp->m_pMainWnd->DestroyWindow();
		}
		nReturnCode = pApp->ExitInstance();
		goto InitFailure;
	}
	ASSERT_VALID(pApp);

	nReturnCode = pApp->Run();
	ASSERT_VALID(pApp);

InitFailure:
	AfxWinTerm();
	return nReturnCode;
}

#else
// _USRDLL library initialization

extern "C" BOOL WINAPI
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	CWinApp* pApp = AfxGetApp();
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// initialize MFC exception handling
#ifndef _AFX_OLD_EXCEPTIONS
		set_terminate(&AfxStandardTerminate);
#endif
		_set_new_handler(&AfxNewHandler);

		// initialize DLL's instance(/module) not the app's
		if (!AfxWinInit(hInstance, NULL, &afxChNil, 0))
		{
			AfxWinTerm();
			return FALSE;   // Init Failed
		}

		// initialize the single instance DLL
		if (pApp != NULL && !pApp->InitInstance())
		{
			pApp->ExitInstance();
			AfxWinTerm();
			return FALSE;   // Init Failed
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		if (pApp != NULL)
			pApp->ExitInstance();

#ifdef _DEBUG
		// check for missing AfxLockTempMap calls
		if (AfxGetThreadState()->m_nTempMapLock != 0)
			TRACE1("Warning: Temp map lock count non-zero (%ld).\n",
				AfxGetThreadState()->m_nTempMapLock);
#endif
		// terminate the library before destructors are called
		AfxWinTerm();

		// free safety pool buffer
		AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
		if (pThreadState->m_pSafetyPoolBuffer != NULL)
		{
			free(pThreadState->m_pSafetyPoolBuffer);
			pThreadState->m_pSafetyPoolBuffer = NULL;
		}
		// clean up map objects before it is too late
		pThreadState->m_mapHWND.CHandleMap::~CHandleMap();
		pThreadState->m_mapHMENU.CHandleMap::~CHandleMap();
		pThreadState->m_mapHDC.CHandleMap::~CHandleMap();
		pThreadState->m_mapHGDIOBJ.CHandleMap::~CHandleMap();

#ifdef _DEBUG
		// trace any memory leaks that may have occurred
		AfxDumpMemoryLeaks();
#endif
	}
	return TRUE;    // ok
}

// Note: need to initialize _pRawDllMain to RawDllMain so it gets called
extern "C" BOOL WINAPI RawDllMain(HINSTANCE, DWORD dwReason, LPVOID);
extern "C" BOOL (WINAPI* _pRawDllMain)(HINSTANCE, DWORD, LPVOID) = &RawDllMain;
#endif //!_USRDLL

/////////////////////////////////////////////////////////////////////////////
// Common DLL initialization

#ifdef _WINDLL
extern BOOL _afxSharedData; // set to TRUE if running Win32s
extern DWORD _afxAppTlsIndex;

extern "C" BOOL WINAPI RawDllMain(HINSTANCE, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// make sure we have enough memory to attempt to start (8kb)
		void* pMinHeap = LocalAlloc(NONZEROLPTR, 0x2000);
		if (pMinHeap == NULL)
			return FALSE;   // fail if memory alloc fails
		LocalFree(pMinHeap);

		// cache Win32s version info
		if (_afxSharedData == (BOOL)-1)
		{
			DWORD dwVersion = ::GetVersion();
			_afxSharedData = (dwVersion & 0x80000000) && (BYTE)dwVersion <= 3;
		}

		// allocate initial thread local storage index
		if (_afxThreadTlsIndex == NULL_TLS)
		{
			_afxThreadTlsIndex = TlsAlloc();
			if (_afxThreadTlsIndex == NULL_TLS)
				return FALSE;   // failure
		}

		// initialize thread state for before constructors run
		AFX_THREAD_STATE* pThreadState = new AFX_THREAD_STATE;
		if (pThreadState == NULL)
			return FALSE;

		// initialize process state before constructors run
		AFX_APP_STATE* pAppState;
		if (_afxSharedData)
		{
			// Win32s: allocate thread local storage index if necessary
			if (_afxAppTlsIndex == NULL_TLS)
			{
				_afxAppTlsIndex = TlsAlloc();
				if (_afxAppTlsIndex == NULL_TLS)
					return FALSE;   // failure
			}

			// allocate AFX_APP_STATE structure for this process
			ASSERT(TlsGetValue(_afxAppTlsIndex) == NULL);
			pAppState = new AFX_APP_STATE;
			ASSERT(TlsGetValue(_afxAppTlsIndex) == pAppState);
			if (pAppState == NULL)
				return FALSE;   // failure
		}
		else
		{
			// Win32: use global buffer for app state instead
			pAppState = new AFX_APP_STATE;
			ASSERT(pAppState != NULL);
		}

		// make sure everything worked
		ASSERT(AfxGetAppState() == pAppState);
		ASSERT(AfxGetThreadState() == pThreadState);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		// free the thread state (for primary thread)
		AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
		delete pThreadState;

		// free the process state
		AFX_APP_STATE* pAppState = AfxGetAppState();
		delete pAppState;
	}
	return TRUE;    // ok
}
#endif //_WINDLL

/////////////////////////////////////////////////////////////////////////////
