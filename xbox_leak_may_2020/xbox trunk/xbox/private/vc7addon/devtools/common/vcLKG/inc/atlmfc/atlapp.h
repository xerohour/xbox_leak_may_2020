//NONSHIP

// This is a part of the Active Template Library.
// Copyright (C) 1996-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLAPP_H__
#define __ATLAPP_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLBASE_H__
	#error atlapp.h requires atlbase.h to be included first
#endif

namespace ATL
{

/////////////////////////////////////////////////////////////////////////////
// Forward declarations

__interface CMessageFilter;
__interface CIdleHandler;
class CMessageLoop;
#if (_ATL_VER >= 0x0400)
class CAppModule;
#endif //(_ATL_VER >= 0x0400)


/////////////////////////////////////////////////////////////////////////////
// CMessageFilter - Interface for message filter support

__interface CMessageFilter
{
	BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////
// CIdleHandler - Interface for idle processing

__interface CIdleHandler
{
	BOOL OnIdle();
};

/////////////////////////////////////////////////////////////////////////////
// CMessageLoop - message loop implementation

class CMessageLoop
{
public:
	CSimpleArray<CMessageFilter*> m_aMsgFilter;
	CSimpleArray<CIdleHandler*> m_aIdleHandler;
	MSG m_msg;

// Message filter operations
	BOOL AddMessageFilter(CMessageFilter* pMessageFilter)
	{
		return m_aMsgFilter.Add(pMessageFilter);
	}
	BOOL RemoveMessageFilter(CMessageFilter* pMessageFilter)
	{
		return m_aMsgFilter.Remove(pMessageFilter);
	}
// Idle handler operations
	BOOL AddIdleHandler(CIdleHandler* pIdleHandler)
	{
		return m_aIdleHandler.Add(pIdleHandler);
	}
	BOOL RemoveIdleHandler(CIdleHandler* pIdleHandler)
	{
		return m_aIdleHandler.Remove(pIdleHandler);
	}
// message loop
	int Run()
	{
		BOOL bDoIdle = TRUE;
		int nIdleCount = 0;
		BOOL bRet;

		for(;;)
		{
			while(!::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE) && bDoIdle)
			{
				if(!OnIdle(nIdleCount++))
					bDoIdle = FALSE;
			}

			bRet = ::GetMessage(&m_msg, NULL, 0, 0);

			if(bRet == -1)
			{
				ATLTRACE2(atlTraceWindowing, 0, _T("::GetMessage returned -1 (error)\n"));
				continue;	// error, don't process
			}
			else if(!bRet)
			{
				ATLTRACE2(atlTraceWindowing, 0, _T("CMessageLoop::Run - exiting\n"));
				break;		// WM_QUIT, exit message loop
			}

			if(!PreTranslateMessage(&m_msg))
			{
				::TranslateMessage(&m_msg);
				::DispatchMessage(&m_msg);
			}

			if(IsIdleMessage(&m_msg))
			{
				bDoIdle = TRUE;
				nIdleCount = 0;
			}
		}

		return m_msg.wParam;
	}

	static BOOL IsIdleMessage(MSG* pMsg)
	{
		// These messages should NOT cause idle processing
		switch(pMsg->message)
		{
		case WM_MOUSEMOVE:
#ifndef UNDER_CE
		case WM_NCMOUSEMOVE:
#endif //!UNDER_CE
		case WM_PAINT:
		case 0x0118:	// WM_SYSTIMER (caret blink)
			return FALSE;
		}

		return TRUE;
	}

// Overrideables
	// Override to change message filtering
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		// loop backwards
		for(int i = m_aMsgFilter.GetSize() - 1; i >= 0; i--)
		{
			CMessageFilter* pMessageFilter = m_aMsgFilter[i];
			if(pMessageFilter != NULL && pMessageFilter->PreTranslateMessage(pMsg))
				return TRUE;
		}
		return FALSE;	// not translated
	}
	// override to change idle processing
	virtual BOOL OnIdle(int /*nIdleCount*/)
	{
		for(int i = 0; i < m_aIdleHandler.GetSize(); i++)
		{
			CIdleHandler* pIdleHandler = m_aIdleHandler[i];
			if(pIdleHandler != NULL)
				pIdleHandler->OnIdle();
		}
		return FALSE;	// don't continue
	}
};


#if (_ATL_VER >= 0x0400)
//class CAppModule : public CExeModule
class CAppModule : public CComExeModule<CAppModule>
{
public:
	CSimpleMap<DWORD, CMessageLoop*>* m_pMsgLoopMap;

// Overrides of CExeModule::Init and Term
	HRESULT Init(_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID = NULL)
	{
//		HRESULT hr = CExeModule::Init(pObjMap, hInstance, pLibID);
		HRESULT hr = CComExeModule<CAppModule>::Init(pObjMap, hInstance, pLibID);
		if (FAILED(hr))
			return hr;

		typedef CSimpleMap<DWORD, CMessageLoop*>	mapClass;
		m_pMsgLoopMap = NULL;
		ATLTRY(m_pMsgLoopMap = new mapClass);
		if (m_pMsgLoopMap == NULL)
			return E_OUTOFMEMORY;

		return hr;
	}
	void Term()
	{
		delete m_pMsgLoopMap;
//		CExeModule::Term();
		CComExeModule<CAppModule>::Term();
	}

// Message loop map methods
	BOOL AddMessageLoop(CMessageLoop* pMsgLoop)
	{
		ATLASSERT(pMsgLoop != NULL);
		ATLASSERT(m_pMsgLoopMap->Lookup(::GetCurrentThreadId()) == NULL);	// not in map yet
		return m_pMsgLoopMap->Add(::GetCurrentThreadId(), pMsgLoop);
	}
	BOOL RemoveMessageLoop()
	{
		return m_pMsgLoopMap->Remove(::GetCurrentThreadId());
	}
	CMessageLoop* GetMessageLoop(DWORD dwThreadID = ::GetCurrentThreadId()) const
	{
		return m_pMsgLoopMap->Lookup(dwThreadID);
	}
};
#endif //(_ATL_VER >= 0x0400)

}; //namespace ATL

#endif // __ATLAPP_H__
