// DmConnection.cpp: implementation of the CDmConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DmConnection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDmConnection::CDmConnection() :
	m_pdmc(NULL),
	m_sharedConnectionOpened(false)
{
}

CDmConnection::~CDmConnection()
{
	CloseConnection();

	if (m_sharedConnectionOpened) {
		DmUseSharedConnection(FALSE);
	}
}

HRESULT CDmConnection::SetXboxNameNoRegister(LPCSTR pszName)
{
	return DmSetXboxNameNoRegister(pszName);
}

HRESULT CDmConnection::SetXboxName(LPCSTR pszName)
{
	return DmSetXboxName(pszName);
}

HRESULT CDmConnection::GetXboxName(LPSTR pszName, LPDWORD pcch)
{
	return DmGetXboxName(pszName, pcch);
}

HRESULT CDmConnection::SendCommand(LPCSTR szCmd, LPSTR szResp, LPDWORD pcb)
{
	HRESULT hr;

	if (!m_pdmc && FAILED(hr = OpenConnection())) {
		return hr;
	}

	return DmSendCommand(m_pdmc, szCmd, szResp, pcb);
}

HRESULT CDmConnection::OpenConnection()
{
	if (m_pdmc) {
		return S_OK;
	}

	return DmOpenConnection(&m_pdmc);
}

HRESULT CDmConnection::CloseConnection()
{
	HRESULT hr = S_OK;

	if (m_pdmc) {
		hr = DmCloseConnection(m_pdmc);
		m_pdmc = NULL;
	}

	return hr;
}

HRESULT CDmConnection::UseSharedConnection()
{
	if (m_sharedConnectionOpened) {
		return S_OK;
	}

	HRESULT hr = DmUseSharedConnection(TRUE);

	if (SUCCEEDED(hr)) {
		m_sharedConnectionOpened = true;
	}

	return hr;
}

HRESULT CDmConnection::ReceiveSocketLine(LPSTR szResponse, LPDWORD pcbSize) const
{
	return DmReceiveSocketLine(m_pdmc, szResponse, pcbSize);
}

LPSTR CDmConnection::_stristr(LPCSTR str1, LPCSTR str2)
{
    LPSTR cp = (LPSTR)str1;
    LPSTR s1, s2;

    if (!*str2) {
		return (LPSTR)str1;
	}

    while (*cp) {
        s1 = cp;
        s2 = (LPSTR)str2;

        while (*s1 && *s2 && _tolower(*s1) == _tolower(*s2)) {
			s1++, s2++;
		}

        if (!*s2) {
			return cp;
		}

        cp++;
    }

    return NULL;
}

LPSTR CDmConnection::GetParamPointer(LPCSTR szResp, LPCSTR szName)
{
	LPSTR p = _stristr(szResp, szName);

	if (p && (p = strchr(p, '=')) != NULL) {
		p++;
		while (isspace(*p)) {
			p++;
		}
	}

	return p;
}

bool CDmConnection::GetParam(LPCSTR szResp, LPCSTR szName, LPDWORD pdwResult)
{
	LPSTR p = GetParamPointer(szResp, szName);

	if (p) {
		*pdwResult = strtoul(p, NULL, 0);
	}

	return p != NULL;
}

HRESULT CDmConnection::TranslateError(HRESULT hr, LPSTR Buffer, DWORD cb)
{
	return DmTranslateError(hr, Buffer, cb);
}
