#pragma once


#include <httpext.h>
#include <atlutil.h>

#define ATLSV_IS_TRANSFERRED_REQUEST "ATLSV_IS_TRANSFERRED_REQUEST"

// ECB used for transferring request on the server.
class CTransferECB : 
	public EXTENSION_CONTROL_BLOCK
{
public:
	CTransferECB()
	{
		m_pFromECB = NULL;
	}

	BOOL Initialize(EXTENSION_CONTROL_BLOCK *pFromECB,
					LPCSTR szNewURL)
	{
		ATLASSERT(pFromECB != NULL);
		ATLASSERT(szNewURL != NULL);
		m_pFromECB = pFromECB;
		m_TransferUrl.CrackUrl(szNewURL);
		ConnID = (HCONN)this;
		cbSize = pFromECB->cbSize;
		dwVersion = pFromECB->dwVersion;
		dwHttpStatusCode = pFromECB->dwHttpStatusCode;
		if (pFromECB->lpszLogData)
			strcpy(lpszLogData, pFromECB->lpszLogData);
		lpszMethod = pFromECB->lpszMethod;

		ReTranslatePath();
		
		lpszQueryString = (char*)(m_TransferUrl.GetExtraInfo()+1);
		lpszPathInfo = (LPSTR)m_TransferUrl.GetUrlPath();
		lpszPathTranslated = (LPSTR)m_szPathTranslated; // path translated's are the same.
												// this restricts transfer urls to the same vroot.
		cbTotalBytes = pFromECB->cbTotalBytes;;
		cbAvailable = pFromECB->cbAvailable;
		lpbData = pFromECB->lpbData;
		lpszContentType = pFromECB->lpszContentType;

		GetServerVariable = CTransferECB::GetServerVariableImpl;
		WriteClient = CTransferECB::WriteClientImpl;
		ReadClient = CTransferECB::ReadClientImpl;
		ServerSupportFunction = CTransferECB::ServerSupportFunctionImpl;
		return TRUE;
	}

    static BOOL WINAPI GetServerVariableImpl(
										HCONN       hConn,
                                        LPSTR       lpszVariableName,
                                        LPVOID      lpvBuffer,
                                        LPDWORD     lpdwSize)
	{
		EXTENSION_CONTROL_BLOCK *pECB = ((CTransferECB*)hConn)->m_pFromECB;
		ATLASSERT(pECB != NULL);

		// must special case some server variables to make the transfer work right.
		if (!strcmp(lpszVariableName, ATLSV_IS_TRANSFERRED_REQUEST ))
		{
			if (*lpdwSize != sizeof(int))
				return FALSE;
			*lpdwSize = sizeof(int);
			*((int*)lpvBuffer) = 1;
			return TRUE;
		}

		return pECB->GetServerVariable(
										pECB->ConnID,
										lpszVariableName,
										lpvBuffer,
										lpdwSize);
	}

    static BOOL WINAPI WriteClientImpl( HCONN      ConnID,
                                   LPVOID     Buffer,
                                   LPDWORD    lpdwBytes,
                                   DWORD      dwReserved )
	{
		EXTENSION_CONTROL_BLOCK *pECB = ((CTransferECB*)ConnID)->m_pFromECB;
		ATLASSERT(pECB != NULL);
		DWORD dwErr = 0;
		BOOL bRet =  pECB->WriteClient(
								 pECB->ConnID,
								 Buffer,
								 lpdwBytes,
								 dwReserved
								 );

		if (!bRet)
			dwErr = GetLastError();
		return bRet;

	}

    static BOOL WINAPI ReadClientImpl( HCONN      ConnID,
                                  LPVOID     lpvBuffer,
                                  LPDWORD    lpdwSize )
	{
  		EXTENSION_CONTROL_BLOCK *pECB = ((CTransferECB*)ConnID)->m_pFromECB;
		ATLASSERT(pECB != NULL);
		return pECB->ReadClient(
								pECB->ConnID,
								lpvBuffer,
								lpdwSize
								);
	}


    static BOOL WINAPI ServerSupportFunctionImpl( HCONN      hConn,
                                           DWORD      dwHSERequest,
                                           LPVOID     lpvBuffer,
                                           LPDWORD    lpdwSize,
                                           LPDWORD    lpdwDataType )
	{
		EXTENSION_CONTROL_BLOCK *pECB = ((CTransferECB*)hConn)->m_pFromECB;
		ATLASSERT(pECB != NULL);
		return pECB->ServerSupportFunction(
											pECB->ConnID,
											dwHSERequest,
											lpvBuffer,
											lpdwSize,
											lpdwDataType
											);
	}



	void ReTranslatePath()
	{
		char szNewPath[_MAX_PATH];
		strcpy(szNewPath, m_TransferUrl.GetUrlPath());

		char *pTemp = szNewPath;
		while (*pTemp)
		{
			if (*pTemp == '/')
				*pTemp = '\\';
			pTemp++;
		}

		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];

		_splitpath(m_pFromECB->lpszPathTranslated, drive, dir, NULL, NULL);
		pTemp = szNewPath + strlen(szNewPath);
		while (pTemp > szNewPath)
		{
			if (*pTemp == '\\')
				break;
			if (pTemp <= szNewPath)
			{
				pTemp = NULL;
				break;
			}
			pTemp--;
		}
		if (!pTemp)
			return;
		pTemp++;
		strcpy(m_szPathTranslated, drive);
		strcat(m_szPathTranslated, dir);
		strcat(m_szPathTranslated, pTemp);
	}


	char m_szPathTranslated[_MAX_PATH];
	CUrl m_TransferUrl;
	EXTENSION_CONTROL_BLOCK *m_pFromECB;
};