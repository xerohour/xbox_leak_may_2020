// RegScan.cpp: implementation of the CRegistryScanner class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegScan.h"
#include "uniapi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


////////////////////////////////////////////////////////////////////////////
// Registry scanner class
CRegistryScanner::CRegistryScanner()
	: m_hKeyRoot(NULL), m_iSubKey(0)
{
}

CRegistryScanner::~CRegistryScanner()
{
	if (m_hKeyRoot != NULL)
		Close();
}

BOOL CRegistryScanner::Open(HKEY hKey, BSTR bstrSubKey)
{
	if (m_hKeyRoot)
		Close();

	m_bstrKey = bstrSubKey;

	// Try to open the key.
	return ::RegOpenKeyW(hKey, bstrSubKey, &m_hKeyRoot) == ERROR_SUCCESS;
}

void CRegistryScanner::Close()
{
	// Close the key.
	::RegCloseKey(m_hKeyRoot);
	m_hKeyRoot = NULL;
}

BOOL CRegistryScanner::EnumKey(BSTR *pbstrKeyName)
{
	BOOL fOK;
	wchar_t szBuffer[_MAX_REG_BUFFER + 1];
	fOK = ::RegEnumKeyW(m_hKeyRoot, m_iSubKey++, szBuffer, _MAX_REG_BUFFER) == ERROR_SUCCESS;
	if( fOK )
	{
		m_bstrKey = szBuffer;
		m_bstrKey.CopyTo(pbstrKeyName);
	}
	return fOK;
}

BOOL CRegistryScanner::GetValue(BSTR bstrKey, BSTR *pbstrValue)
{
	ATLASSERT(m_hKeyRoot != NULL);

	CComBSTR bstrSub = m_bstrKey;
	bstrSub.Append(bstrKey);

	wchar_t szBuffer[_MAX_REG_BUFFER + 1];
	LONG cchBuffer = _MAX_REG_BUFFER;

	LONG lr = ::RegQueryValueW(m_hKeyRoot, bstrSub, szBuffer, &cchBuffer);
	if( lr == ERROR_SUCCESS )
	{
		CComBSTR bstr = szBuffer;
		*pbstrValue = bstr.Detach();
	}
	return lr == ERROR_SUCCESS;
}

BOOL CRegistryScanner::QueryStringValue(BSTR bstrValueName, CStringW& rstrValue)
{
	ATLASSERT(m_hKeyRoot != NULL);

	DWORD dwType;
	DWORD cchBuffer = _MAX_REG_BUFFER;
	LONG lRes;
	
	wchar_t szBuffer[_MAX_REG_BUFFER + 1];
	lRes = ::RegQueryValueExW(m_hKeyRoot, bstrValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(szBuffer), &cchBuffer);
	rstrValue = szBuffer;

	if (lRes != ERROR_SUCCESS)
		return FALSE;
	if (dwType != REG_SZ)
		return FALSE;

	return TRUE;
}
