/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineAuthUtil.h
 *  Content:    utility functions
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/22/01    danrose Created to test Xbox Online Auth apis
 *  01/05/02    davidhef Added WaitForTask()
 *  01/05/02    davidhef Added IsUserDataEqual() and operators
 *
 ****************************************************************************/

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xonlinep.h>
#include <list>


//
// functions to start and close the network
//

HRESULT StartNetwork( void );
HRESULT CloseNetwork( void );

HRESULT DeleteAllUsersFromHD( void );

inline bool IsMUAccount(XONLINE_USER const *pUser)
{
    return (pUser->dwUserOptions && XONLINE_USER_OPTION_CAME_FROM_MU);
}

inline bool IsHDAccount(XONLINE_USER const *pUser)
{
    return !IsMUAccount(pUser);
}

DWORD CountMUAccounts(XONLINE_USER const *pUsers);

//
// Function that lets logon finish its initial work.
//

HRESULT WaitForLogon( XONLINETASK_HANDLE taskHandle, HANDLE hEvent );

// Compare user data
bool IsUserDataEqual(XONLINEP_USER const *pUser1, XONLINEP_USER const *pUser2);

// Useful operators
inline bool operator==( XUID const& lhs, XUID const& rhs )
{
    // Only compare the actual IDs, not the flags.
    return (lhs.qwUserID == rhs.qwUserID);
}

inline bool operator!=( XUID const& lhs, XUID const& rhs )
{
    return !(lhs == rhs);
}

inline bool operator==( XUID const& lhs, ULONGLONG rhs )
{
    // Only compare the actual IDs, not the flags.
    return (lhs.qwUserID == rhs);
}

inline bool operator!=( XUID const& lhs, ULONGLONG rhs )
{
    return !(lhs == rhs);
}

inline bool operator==(XONLINE_USER const& lhs, XONLINE_USER const& rhs)
{
    return (lhs.xuid == rhs.xuid);
}

inline bool operator!=(XONLINE_USER const& lhs, XONLINE_USER const& rhs)
{
    return !(lhs == rhs);
}

bool BVTMode( void );


class CXOnlineTaskManager
{
public:

    CXOnlineTaskManager(DWORD dwDefaultTickCount = 0);

    ~CXOnlineTaskManager();

    HRESULT ManageTask(XONLINETASK_HANDLE taskHandle, HANDLE hEvent);

private:

    CRITICAL_SECTION        m_critSec;
    DWORD                   m_defaultTicks;

    struct taskRec
    {
        struct taskRec() { ; }
        struct taskRec(XONLINETASK_HANDLE taskHandle, HANDLE hEvent, DWORD dwTicks)
            :   taskHandle(taskHandle),
                hEvent(hEvent),
                dwTicks(dwTicks)
        {
        }

        XONLINETASK_HANDLE  taskHandle;
        HANDLE              hEvent;
        DWORD               dwTicks;
    };

    typedef struct taskRec TaskRec;

    std::list<TaskRec>         m_taskList;
};







