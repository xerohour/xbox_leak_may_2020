// SessionUser.cpp: implementation of the CSessionUser class.
//
//////////////////////////////////////////////////////////////////////

#include "SessionUser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSessionUser::CSessionUser()
{
	memset(&m_UserID, 0, sizeof(m_UserID));
	memset(m_szUserName, 0, sizeof(m_szUserName));
	memset(m_szUserKingdom, 0, sizeof(m_szUserKingdom));
}

CSessionUser::~CSessionUser()
{

}

CSessionUser::CSessionUser(const CSessionUser &SessionUser)
{
	memcpy(&m_UserID, &(SessionUser.m_UserID), sizeof(m_UserID));
	memcpy(m_szUserName, SessionUser.m_szUserName, sizeof(m_szUserName));
	memcpy(m_szUserKingdom, SessionUser.m_szUserKingdom, sizeof(m_szUserKingdom));
}

CSessionUser& CSessionUser::operator =(const CSessionUser& SessionUser)
{
	memcpy(&m_UserID, &(SessionUser.m_UserID), sizeof(m_UserID));
	memcpy(m_szUserName, SessionUser.m_szUserName, sizeof(m_szUserName));
	memcpy(m_szUserKingdom, SessionUser.m_szUserKingdom, sizeof(m_szUserKingdom));

	return *this;
}

bool CSessionUser::operator ==(CSessionUser &SessionUser)
{
	if(memcmp(&m_UserID, &(SessionUser.m_UserID), sizeof(m_UserID)))
		return false;

	if(strcmp(m_szUserName, SessionUser.m_szUserName))
		return false;

	if(strcmp(m_szUserKingdom, SessionUser.m_szUserKingdom))
		return false;

	return true;
}

bool CSessionUser::operator <(CSessionUser &SessionUser)
{
	return (m_UserID.qwUserID < SessionUser.m_UserID.qwUserID);
}

/****************************************************************************
 *
 *  SetUserInfo
 *
 *  Description:
 *      Sets all of the user information
 *
 *  Arguments:
 *      XUID                *pUserID            XUID to set for the user
 *      CHAR                *szUserName         Name to set for the user
 *      CHAR                *szUserKingdom      Kingdom to set for the user
 *
 *  Returns:  
 *      (none)
 *
 ****************************************************************************/
void CSessionUser::SetUserInfo(XUID *pUserID, CHAR *szUserName, CHAR *szUserKingdom)
{
	if(pUserID)
		memcpy(&m_UserID, pUserID, sizeof(m_UserID));

	if(szUserName && (strlen(szUserName) < XONLINE_NAME_SIZE))
		strcpy(m_szUserName, szUserName);

	if(szUserKingdom && (strlen(szUserKingdom) < XONLINE_KINGDOM_SIZE))
		strcpy(m_szUserKingdom, szUserKingdom);
}

/****************************************************************************
 *
 *  GetUserInfo
 *
 *  Description:
 *      Retrieves all of the user information
 *
 *  Arguments:
 *      XUID                *pUserID            XUID to get for the user
 *      CHAR                *szUserName         Name to get for the user
 *      CHAR                *szUserKingdom      Kingdom to get for the user
 *
 *  Returns:  
 *      (none)
 *
 ****************************************************************************/
void CSessionUser::GetUserInfo(XUID *pUserID, CHAR *szUserName, CHAR *szUserKingdom)
{
	if(pUserID)
		memcpy(pUserID, &m_UserID, sizeof(m_UserID));

	if(szUserName)
		strcpy(szUserName, m_szUserName);

	if(szUserKingdom)
		strcpy(szUserKingdom, m_szUserKingdom);
}

/****************************************************************************
 *
 *  PackIntoBuffer
 *
 *  Description:
 *      Packs all of the CSessionUser information into a buffer for transmission
 *      over the wire to another client
 *
 *  Arguments:
 *      CHAR                *pBuff              Buffer to hold the user information.  If this
 *                                              parameter is NULL and a non-NULL pdwBuffSize is
 *                                              provided, then the function will fill pdwBuffSize
 *                                              with the required size necessary
 *      DWORD               *pdwBuffSize        Holds the size of the provided buffer. On output
 *                                              this parameter will contain either the required size
 *                                              (if the provided size was insufficient) or the
 *                                              ammount of space that was actually used
 *
 *  Returns:  
 *      S_OK if the user was succesfully packed into the buffer
 *      S_FALSE if a buffer was supplied but it was too small to hold the data
 *      E_FAIL if bad parameters were passed in or a system error occured
 *
 ****************************************************************************/
HRESULT CSessionUser::PackIntoBuffer(CHAR *pBuff, DWORD *pdwBuffSize)
{
	PSGBVT_USER_HEADER pUserHeader = (PSGBVT_USER_HEADER) pBuff;
	DWORD dwRequiredSize = 0;

	if(!pdwBuffSize)
		return E_FAIL;

	dwRequiredSize = sizeof(SGBVT_USER_HEADER) + sizeof(m_UserID) + strlen(m_szUserName) + strlen(m_szUserKingdom);

	if(*pdwBuffSize < dwRequiredSize)
	{
		*pdwBuffSize = dwRequiredSize;
		return S_FALSE;
	}

	*pdwBuffSize = dwRequiredSize;

	pUserHeader->dwUserSize = dwRequiredSize;
	pUserHeader->dwNameSize = strlen(m_szUserName);
	pUserHeader->dwKingdomSize = strlen(m_szUserKingdom);
	
	memcpy(pBuff += sizeof(SGBVT_USER_HEADER), &m_UserID, sizeof(m_UserID));
	memcpy(pBuff += sizeof(m_UserID), m_szUserName, pUserHeader->dwNameSize);
	memcpy(pBuff += pUserHeader->dwNameSize, m_szUserKingdom, pUserHeader->dwKingdomSize);

	return S_OK;
}

/****************************************************************************
 *
 *  UnpackFromBuffer
 *
 *  Description:
 *      Unpacks a user information packet received over the wire into a new
 *      CSessionUser object
 *
 *  Arguments:
 *      CHAR                *pBuff              Buffer containing the received user information packet
 *      DWORD               *pdwBuffSize        On input, indicates the size of the received packet
 *                                              On output, indicates the size used to build the user object
 *
 *  Returns:  
 *      S_OK if the user object was succesfully unpacked from the buffer
 *      E_FAIL if bad parameters were passed in or a system error occured
 *
 ****************************************************************************/
HRESULT CSessionUser::UnpackFromBuffer(CHAR *pBuff, DWORD *pdwBuffSize)
{
	PSGBVT_USER_HEADER pUserHeader = (PSGBVT_USER_HEADER) pBuff;

	if(*pdwBuffSize < (sizeof(SGBVT_USER_HEADER) + sizeof(m_UserID)) || !pBuff)
		return E_FAIL;

	if(pUserHeader->dwUserSize > *pdwBuffSize)
		return E_FAIL;

	if((pUserHeader->dwNameSize + pUserHeader->dwKingdomSize + sizeof(SGBVT_USER_HEADER) + sizeof(m_UserID)) != pUserHeader->dwUserSize)
		return E_FAIL;

	*pdwBuffSize = pUserHeader->dwUserSize;

	memcpy(&m_UserID, pBuff += sizeof(SGBVT_USER_HEADER), sizeof(m_UserID));
	memset(m_szUserName, 0, sizeof(m_szUserName));
	memcpy(m_szUserName, pBuff += sizeof(m_UserID), pUserHeader->dwNameSize);
	memset(m_szUserName, 0, sizeof(m_szUserName));
	memcpy(m_szUserKingdom, pBuff += pUserHeader->dwNameSize, pUserHeader->dwKingdomSize);

	return S_OK;
}
