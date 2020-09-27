// DmConnection.h: interface for the CDmConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DMCONNECTION_H__D4ACF6C7_4C30_45D9_BF7B_85ADF1128503__INCLUDED_)
#define AFX_DMCONNECTION_H__D4ACF6C7_4C30_45D9_BF7B_85ADF1128503__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDmConnection  
{
public:
	CDmConnection();
	virtual ~CDmConnection();

	HRESULT OpenConnection();
	HRESULT CloseConnection();
	HRESULT SendCommand(LPCSTR szCmd, LPSTR szResp = 0, LPDWORD pcb = 0);
	HRESULT UseSharedConnection();
	HRESULT ReceiveSocketLine(LPSTR szResponse, LPDWORD pcbSize) const;

	static LPSTR _stristr(LPCSTR str1, LPCSTR str2);
	static bool GetParam(LPCSTR szResp, LPCSTR szName, LPDWORD pdwResult);

	static HRESULT GetXboxName(LPSTR pszName, LPDWORD pcch);
	static HRESULT SetXboxName(LPCSTR pszName);
	static HRESULT SetXboxNameNoRegister(LPCSTR pszName);
	static HRESULT TranslateError(HRESULT hr, LPSTR Buffer, DWORD cb);

protected:
	static LPSTR GetParamPointer(LPCSTR szResp, LPCSTR szName);
	bool m_sharedConnectionOpened;
	PDM_CONNECTION m_pdmc;
};

#endif // !defined(AFX_DMCONNECTION_H__D4ACF6C7_4C30_45D9_BF7B_85ADF1128503__INCLUDED_)
