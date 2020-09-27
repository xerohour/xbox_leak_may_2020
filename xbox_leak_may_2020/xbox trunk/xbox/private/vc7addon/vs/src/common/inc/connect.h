/////////////////////////////////////////////////////////////////////////////
// Connect.h -- COM connection helper classes

#pragma once

#include <atlbase.h>

/////////////////////////////////////////////////////////////////////////////
// CConnection

template <const IID* piid>
class CConnection
{
public:
	CConnection() : m_dwCookie(0) {}
	HRESULT Connect(IUnknown* pSource, IUnknown* pSink) { return AtlAdvise(pSource, pSink, *piid, &m_dwCookie); }
	HRESULT Disconnect(IUnknown* pSource) { HRESULT hr = AtlUnadvise(pSource, *piid, m_dwCookie); m_dwCookie = 0; return hr; }
	BOOL IsConnected() { return m_dwCookie != 0; }
	DWORD m_dwCookie;
};
