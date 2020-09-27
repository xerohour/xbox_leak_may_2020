// SessionCreation.h: interface for the CSessionCreation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SESSIONCREATION_H)
#define SESSIONCREATION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "xmatchp.h"
#include "connection.h"
#include "httprequest.h"
#include "mmcommon.h"

DWORD CreateDeleteModifySession(LPTSTR, LPTSTR, XMATCH_SESSION *, DWORD);

class CSessionCreation  
{
public:
	BOOL SetHostAddress(LPBYTE pHostAddress);
	DWORD m_dwBufferLen;
	BOOL AddAttribute(DWORD dwAttribID, DWORD dwAttribLen, LPBYTE pBuffer);
	BOOL AddPlayerAttribute(DWORD dwAttribID, LONGLONG qwPlayerID, DWORD dwAttribLen, LPBYTE pBuffer);
	DWORD SendRequest(LPTSTR szServer, LPTSTR szObject);
	DWORD SendRawRequest(LPTSTR szServer, LPTSTR szObject);
	BOOL SetNumAttributes(DWORD dwNumAttributes);
	BOOL SetPrivateAvailable(DWORD dwPrivateAvailable);
	BOOL SetPublicAvailable(DWORD dwPublicAvailable);
	BOOL SetSessionID(DWORD dwSessionID);
	BOOL SetTitleID(DWORD dwTitleID);
	BOOL SetMessageLength(DWORD dwMessageLength);
	BOOL SetProtocolVersion(DWORD dwProtocolVersion);
	BOOL Initialize(DWORD dwBufferLen);
	LPBYTE m_pBuffer;
	CSessionCreation();
	virtual ~CSessionCreation();

private:
	PATTRIB_LIST m_pAttribList;
	DWORD m_dwTotalAttribs;
	DWORD m_dwTotalAttribLen;
	BOOL m_fNumAttribOverride;
	BOOL m_fMsgLenOverride;
};

#endif // !defined(AFX_SESSIONCREATION_H__0DB786AB_F69D_4CD6_BC61_C671A234C602__INCLUDED_)
