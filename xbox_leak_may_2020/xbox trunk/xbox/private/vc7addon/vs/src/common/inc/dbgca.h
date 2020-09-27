#include <msi.h>
#include "caintf.h"

#include <stdio.h>
#include <tchar.h>
#include <wchar.h>

#ifndef __DBGCA_H__
#define __DBGCA_H__

// setup errors
#define E_DBGCA_INITIALIZE_FAILED                  (MAKE_HRESULT(1, FACILITY_ITF, 0x900))
#define E_DBGCA_GETCOMPONENTSTATE_FAILED           (MAKE_HRESULT(1, FACILITY_ITF, 0x901))
#define E_DBGCA_NO_DARWININST                      (MAKE_HRESULT(1, FACILITY_ITF, 0x902))
#define E_DBGCA_NOT_INSTALL_OR_UNINSTALL           (MAKE_HRESULT(1, FACILITY_ITF, 0x903))

// ----------------------------------------------------------------------------
// Darwin setup support

typedef DWORD (*DARWININSTALLER)(CDarwinInst* pDarwinInst);

class CDebuggerCustomAction
{
// ctor/dtor/initialization
public:
	CDebuggerCustomAction(MSIHANDLE hInstance, LPTSTR pszFriendlyName)
	: m_hInstance(hInstance),
	  m_pszFriendlyName(pszFriendlyName)
	{
		m_pDarwinInst = new CDarwinInst(m_hInstance);
		ASSERT(m_pDarwinInst);
		if (!m_pDarwinInst)
		{
			// Log an error
			PMSIHANDLE hRec = MsiCreateRecord(1);
			if (hRec)
			{
				MsiRecordSetString(hRec, 0, _T("Unable to create a CDarwinInst interface."));
				MsiProcessMessage(m_hInstance, INSTALLMESSAGE_INFO, hRec);
			}
		}
	}
	~CDebuggerCustomAction()
	{
		delete m_pDarwinInst;
	}
// operations
public:
	DWORD Execute(DARWININSTALLER pInstall)
	{
		DWORD dwRet = NOERROR;
		HRESULT hr = S_OK;

		// Log that we are going to call the custom action
		PMSIHANDLE hRec = MsiCreateRecord(1);
		if (hRec)
		{
			TCHAR tcsBuffer[255];
			_stprintf(tcsBuffer, _T("%s::Install() called"), GetFriendlyName());

			MsiRecordSetString(hRec, 0, tcsBuffer);
			MsiProcessMessage(m_hInstance, INSTALLMESSAGE_INFO, hRec);
		}

		if (m_pDarwinInst)
		{
			if (m_pDarwinInst->Initialize(m_pszFriendlyName))
			{
				dwRet = pInstall(m_pDarwinInst);
			}
			else
			{
				hr = E_DBGCA_INITIALIZE_FAILED;
			}
		}
		else
		{
			hr = E_DBGCA_NO_DARWININST;
		}

		// Log the result of calling the custom action
		hRec = MsiCreateRecord(3);
		if (hRec)
		{
			TCHAR tcsBuffer[255];
			_stprintf(tcsBuffer, _T("%s::Install() finished; hr=%08x, dwRet=%08x"), GetFriendlyName(), hr, dwRet);

			MsiRecordSetString(hRec, 0, tcsBuffer);
			MsiProcessMessage(m_hInstance, INSTALLMESSAGE_INFO, hRec);
		}

		return hr;
	}
private:

	LPCTSTR GetFriendlyName() const
	{
		// make sure the log name is not null, and is a reasonable size
		if ( m_pszFriendlyName && _tcslen(m_pszFriendlyName) < 150 )
		{
			return m_pszFriendlyName;
		}
		else
		{
			return _T("<Invalid friendly name>");
		}
	}

// data
private:
	MSIHANDLE m_hInstance;
	CDarwinInst* m_pDarwinInst;
	LPTSTR m_pszFriendlyName;
};

#endif // __DBGCA_H__
