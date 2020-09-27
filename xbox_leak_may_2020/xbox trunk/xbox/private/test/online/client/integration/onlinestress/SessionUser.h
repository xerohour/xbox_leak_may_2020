// SessionUser.h: interface for the CSessionUser class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <winsockx.h>
#include <stdio.h>
#include <xtestlib.h>
#include <xlog.h>
#include <xonlinep.h>

#pragma pack(push, 1)

// This structure frames individual users that are sent on the wire
typedef struct
{
	DWORD dwUserSize;
	DWORD dwNameSize;
	DWORD dwKingdomSize;
} XONSTRESS_USER_HEADER, *PXONSTRESS_USER_HEADER;


#pragma pack(pop)

class CSessionUser  
{
public:
	CSessionUser& operator = (const CSessionUser&);
	bool operator ==(CSessionUser &);
	bool operator <(CSessionUser &);
	HRESULT UnpackFromBuffer(CHAR *pBuff, DWORD *pdwBuffSize);
	HRESULT PackIntoBuffer(CHAR *pBuff, DWORD *pdwBuffSize);
	void GetUserInfo(XUID *pUserID, CHAR *szUserName, CHAR *szUserKingdom);
	void SetUserInfo(XUID *pUserID, CHAR *szUserName, CHAR *szUserKingdom);
	CSessionUser(const CSessionUser &SessionUser);
	CSessionUser();
	virtual ~CSessionUser();

protected:
	CHAR m_szUserKingdom[XONLINE_KINGDOM_SIZE];
	XUID m_UserID;
	CHAR m_szUserName[XONLINE_NAME_SIZE];
};
