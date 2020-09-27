/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineAuthUtil.cpp
 *  Content:    utility functions
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/22/01    danrose Created to test Xbox Online Auth apis
 *  01/05/02    davidhef Added IsUserDataEqual() and WaitForTask()
 *
 ****************************************************************************/

#include "XOnlineAuthUtil.h"

/****************************************************************************
 *
 *  CloseNetwork
 *
 *  Description:
 *      Utility function to close up the network
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT CloseNetwork( void )
{
	HRESULT				hr				= S_OK;

	RUN( XOnlineCleanup() );

	return hr;
}

/****************************************************************************
 *
 *  StartNetwork
 *
 *  Description:
 *      Utility function to start up the network
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT StartNetwork( void )
{
	HRESULT				hr				= S_OK;
	INT					iVal			= 0;
	DWORD				dwResult		= 0;
	WSADATA				wsaData;
	XNADDR				xnaddr;
	XNetStartupParams   xnsp;

	CHECKRUN( XOnlineStartup( NULL ) );

	//
	// wait for DNS to succeed
	//
	do
	{
		dwResult = XNetGetTitleXnAddr( &xnaddr );  	
	} 
	while ( XNET_GET_XNADDR_PENDING == dwResult );

	return hr;
}

/****************************************************************************
 *
 *  WaitForTask
 *
 *  Description:
 *      Utility function to wait for a task to finish
 *
 *  Arguments:
 *      (XONLINETASK_HANDLE taskHandle)
 *
 *  Returns:  
 *      HRESULT returned by XOnlineTaskContinue
 *
 ****************************************************************************/

HRESULT WaitForLogon( XONLINETASK_HANDLE taskHandle, HANDLE hEvent )
{
    HRESULT hr = S_OK;
    DWORD dwWait = 0;

    // Assume that the event passed in is auto-reset!
    
    do
    {   
        //
        // wait for data to be ready for processing
        //
        dwWait = WaitForSingleObject( hEvent, INFINITE );
        
        if ( WAIT_OBJECT_0 != dwWait )
        {
            hr = E_FAIL;

            if (!BVTMode())
            {
                ASSERT( SUCCEEDED( hr ) || !"wait failed!" );
            }

            return hr;
        }
        
        //
        // continue pumping the handle
        //
        hr = XOnlineTaskContinue( taskHandle );
    } while ( XONLINETASK_S_RUNNING == hr );
    
    return hr;
}


/****************************************************************************
 *
 *  IsUserDataEqual
 *
 *  Description:
 *      Utility function to compare two users' data
 *
 *  Arguments:
 *      (XONLINE_USER const& user1, XONLINE_USER const& user2)
 *
 *  Returns:  
 *      bool 
 *
 ****************************************************************************/

bool IsUserDataEqual(XONLINEP_USER const *pUser1, XONLINEP_USER const *pUser2)
{
    // We don't need to compare all elements.
    //
    if (pUser1->xuid == pUser2->xuid)
    {
        // Theoretically, we don't need to compare name or kingdom, but we 
        // do it because this is a test routine.
        if (memcmp((PVOID) &(pUser1->name), (PVOID) &(pUser2->name), sizeof(pUser1->name)) == 0)
        {
            if (memcmp((PVOID) &(pUser1->kingdom), (PVOID) &(pUser2->kingdom), sizeof(pUser1->kingdom)) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

HRESULT DeleteAllUsersFromHD( void )
{
    XONLINE_USER users[XONLINE_MAX_STORED_ONLINE_USERS];
    HRESULT hr = S_OK;
    DWORD dwcUsers;
    DWORD dwcMUUsers = 0;
    
    ZeroMemory(users, sizeof(users));
    
    // Get the existing users.
    hr = XOnlineGetUsers(users, &dwcUsers);
    
    // Remove the existing users.
    for (DWORD i = 0; i < XONLINE_MAX_STORED_ONLINE_USERS && SUCCEEDED(hr); ++i)
    {
        if (users[ i ].xuid.qwUserID == (ULONGLONG) 0)
        {
            // Skip empty slots.
            continue;
        }

        if (users[ i ].dwUserOptions & XONLINE_USER_OPTION_CAME_FROM_MU)
        {
            // Skip user accounts from MUs.
            ++dwcMUUsers;
            continue;
        }

        hr = _XOnlineRemoveUserFromHD( users + i );
    }
    
    // Verify that there are no users.
    if (SUCCEEDED(hr))
    {
        hr = XOnlineGetUsers(users, &dwcUsers);
        hr = (SUCCEEDED(hr) && (0 == dwcUsers - dwcMUUsers)) ? S_OK : E_FAIL;
    }
    
    return hr;
}

bool BVTMode( void )
{
    return true;
}

CXOnlineTaskManager::CXOnlineTaskManager(DWORD dwTicks)
:
    m_defaultTicks(dwTicks)
{
    InitializeCriticalSection(&m_critSec);
}

CXOnlineTaskManager::~CXOnlineTaskManager()
{
    DeleteCriticalSection(&m_critSec);
}

HRESULT CXOnlineTaskManager::ManageTask(XONLINETASK_HANDLE taskHandle, HANDLE hEvent)
{
    TaskRec task(taskHandle, hEvent, m_defaultTicks);

    EnterCriticalSection(&m_critSec);
    m_taskList.push_back(task);
    LeaveCriticalSection(&m_critSec);

    return S_OK;
}

DWORD CountMUAccounts(XONLINE_USER const *pUsers)
{
    DWORD dwCount = 0;

    XONLINE_USER const *pEnd = pUsers + XONLINE_MAX_STORED_ONLINE_USERS;
    XONLINE_USER const *pCurrentUser = pUsers;

    for (; pCurrentUser < pEnd; ++pCurrentUser)
    {
        if (IsMUAccount(pCurrentUser))
            ++dwCount;
    }

    return dwCount;
}



