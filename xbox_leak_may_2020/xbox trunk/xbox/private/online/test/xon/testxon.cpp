// ----------------------------------------------------------------------------
// testxon.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved
// ----------------------------------------------------------------------------

#include <windows.h>
#include <xbox.h>
#include <winsockx.h>
#include <winsockp.h>
#include <xonlinep.h>
#include <stdlib.h>
#include <stdio.h>
#include <vlan.h>
#include <OnlineAccounts2.h>

// ---------------------------------------------------------------------------------------
// Utilities
// ---------------------------------------------------------------------------------------

__forceinline void DbgBreak() {
    _asm int 3;
}

#define Verify(x)       do { if (!(x)) DbgBreak(); } while (0)

//
// Standard 30 second timeout.  Used as a parameter to DoWork().
//
#define STANDARD_TIMEOUT 30000

//
// Maximum number of tasks in the array that DoWork() will pump
//
#define NUM_TASKS        10

//
// Helper structure to tie a task with an HRESULT
//
struct ONLINETASK
{
    XONLINETASK_HANDLE hTask;
    HRESULT            hr;
};

//
// Progress interval:  time between output of dots.
//
#define PROGRESS_INTERVAL 2000

// ---------------------------------------------------------------------------------------
// CTestXbox
// ---------------------------------------------------------------------------------------

//
// Inherit from COnlineAccounts for account creation support
//
// BUGBUG: COnlineAccounts does not handle more than one instance of itself.  Need to
// fix it so we can simulate multiple Xboxes.
//
class CTestXbox : public COnlineAccounts
{

public:
    CTestXbox(char *pszXbox) : COnlineAccounts(pszXbox) {};

    //
    // Initialization of CTestXbox class.  It takes a pointer to the second CTestXbox
    // class so it can access the member variables such as user list and hresults.
    //
    virtual void StartupTest(CTestXbox *pOtherXbox, HANDLE hEvent1, HANDLE hEvent2)
    {
        DWORD i = 0;

        m_pOtherXbox = pOtherXbox;
        m_hEvent1    = hEvent1;
        m_hEvent2    = hEvent2;

        //
        // Initialize all tasks and create events for each
        //
        for (i = 0; i < NUM_TASKS; i += 1)
        {
            m_Tasks[i].hTask = NULL;
            m_Tasks[i].hr = S_OK;
            Verify((m_hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);
        }

        //
        // Zero out the user list
        //
        RtlZeroMemory( m_pUsers, sizeof(m_pUsers) );

        Verify(XOnlineStartup(NULL) == S_OK);
    };

    //
    // Termination of CTestXbox class.  Closes the logon task handle and XOnline.
    //
    virtual void TermTest()
    {
        DWORD i = 0;

        //
        // Close logon task handle
        //
        if (m_Tasks[0].hTask != NULL)
        {
            Verify(XOnlineTaskClose(m_Tasks[0].hTask) == S_OK);
        }

        Verify(XOnlineCleanup() == S_OK);

        //
        // Cleanup all task events
        //
        for (i = 0; i < NUM_TASKS; i += 1)
        {
            Verify(CloseHandle(m_hEvents[i]));
        }
    };

    //
    // =============================================================================
    // =============================================================================
    // Test case list - Add test cases here
    //
    virtual void Test_Account_Create() = 0;
    virtual void Test_Friends() = 0;
    virtual void Test_Matchmaking() = 0;
    //
    // End of test case list
    // =============================================================================
    // =============================================================================
    //

    //
    // Pump an array of task handles for some amount of time.  If hTaskToComplete is
    // NULL, then all of the tasks are just pumped as needed for dwTime.  If
    // hTaskToComplete is a valid task handle in the m_Tasks array, DoWork() will
    // exit early as soon as that task is complete (ie. did not return
    // XONLINETASK_S_RUNNING).  HRESULTS for every task are stored in the m_Tasks
    // structures.  A task is pumped until it returns something other than
    // XONLINETASK_S_RUNNING.  On the next call to DoWork(), however, all HRESULTS
    // are reset and all tasks are pumped again.
    //
    void DoWork(XONLINETASK_HANDLE hTaskToComplete, DWORD dwTime)
    {
        DWORD        dwEndTime  = GetTickCount() + dwTime;
        DWORD        i          = 0;
        static DWORD dwLastTime = 0;
        static BOOL  fDidWork   = FALSE;

        //
        // Reset HR of every task so they are pumped at least once.  If you need
        // the HR, be sure to use it before calling DoWork() again.
        //
        for (i = 0; i < NUM_TASKS; i += 1)
        {
            m_Tasks[i].hr = XONLINETASK_S_RUNNING;
        }

        //
        // Loop until we pass the timeout time.
        //
        while (GetTickCount() < dwEndTime)
        {
            //
            // Wait for any of our tasks to signal that work is required.  If no tasks are signalled
            // after PROGRESS_INTERVAL time, display a dot, and wait again (or leave it we hit the
            // timeout dwEndTime)
            //
            if (WaitForMultipleObjects(NUM_TASKS, m_hEvents, FALSE, PROGRESS_INTERVAL) != WAIT_TIMEOUT)
            {
                for (i = 0; i < NUM_TASKS; i += 1)
                {
                    //
                    // If the last TaskContinue() call returned XONLINETASK_S_RUNNING, then
                    // pump it again this time.  Otherwise skip it, so the client of DoWork()
                    // can look at any interesting HRESULTS.
                    //
                    if (m_Tasks[i].hr == XONLINETASK_S_RUNNING && m_Tasks[i].hTask != NULL)
                    {
                        fDidWork = TRUE;

                        m_Tasks[i].hr = XOnlineTaskContinue(m_Tasks[i].hTask);

                        //
                        // If the task has signalled an important state (something other than
                        // XONLINETASK_S_RUNNING) and the client of DoWork() is waiting for this
                        // task to complete (by passing in the task handle as the hTaskToComplete
                        // parameter), exit this function.
                        //
                        if (m_Tasks[i].hr != XONLINETASK_S_RUNNING && m_Tasks[i].hTask == hTaskToComplete)
                        {
                            goto Done;
                        }
                    }
                }
            }

            //
            // Output a dot to signal that the program is still running.  Only display a dot
            // as often as PROGRESS_INTERVAL.
            //
            if ((GetTickCount() - dwLastTime) > PROGRESS_INTERVAL)
            {
                dwLastTime = GetTickCount();

                if (fDidWork == TRUE)
                {
                    printf("*");
                    fDidWork = FALSE;
                }
                else
                {
                    printf(".");
                }
            }

            //
            // BUGBUG: Pending bug 11521, we need to Sleep so that vlan.sys has a chance to do work
            // and keep up with net traffic.
            //
            Sleep(1);
        }
      Done:;
    };

    //
    // Helper function that returns true if all task HRESULTS are success.  Useful to call
    // this after calling DoWork().
    //
    BOOL NoTaskErrors()
    {
        for (DWORD i = 0; i < NUM_TASKS; i += 1)
        {
            if (FAILED(m_Tasks[i].hr))
            {
                return FALSE;
            }
        }
        return TRUE;
    };

    HANDLE             m_hEvent1;            // Synchronization event to keep unit test threads running together
    HANDLE             m_hEvent2;            // Synchronization event to keep unit test threads running together
    XONLINE_USER       m_pUsers[XONLINE_MAX_STORED_ONLINE_USERS]; // Array of logged in users
    DWORD              m_cUsers;             // Number of logged in users
    ONLINETASK         m_Tasks[NUM_TASKS];   // Array of task handles to pump
    HANDLE             m_hEvents[NUM_TASKS]; // Array of events to signal task handles
    CTestXbox          *m_pOtherXbox;        // Pointer to other Xbox so we can access its member variables
};


//
// Helper function for Friends test.  Checks if a particular user is in the friends list.
//
BOOL IsInFriendsList(XUID xuid, PXONLINE_FRIEND pFriends, DWORD dwNumFriends)
{
    DWORD i = 0;

    for (i = 0; i < dwNumFriends; i += 1)
    {
        if (*((ULONGLONG *)&xuid) == *((ULONGLONG *)&(pFriends[i].xuid)))
            return TRUE;
    }
    return FALSE;
}

//
// First simulated Xbox.  This will contain unit tests that only require one simulated Xbox in addition
// to tests that require both simulated Xboxes.
//
class CTestXbox1 : public CTestXbox
{
public:
    CTestXbox1(char *pszXbox) : CTestXbox(pszXbox) {};

    //
    // *****************************************************************************
    // *****************************************************************************
    // Test_Account_Create - test account creation and logon
    // *****************************************************************************
    // *****************************************************************************
    //
    virtual void Test_Account_Create()
    {
        HRESULT hr;

        //
        // =============================================================================
        // =============================================================================
        // Add required services here
        //
        DWORD aServices[] = {
            XONLINE_BILLING_OFFERING_SERVICE,
            XONLINE_USER_ACCOUNT_SERVICE,
            XONLINE_AUTO_UPDATE_SERVICE,
            XONLINE_MATCHMAKING_SERVICE,
            };
        //
        // =============================================================================
        // =============================================================================
        //

        printf("Testing Account_Create ");

        //
        // Populate four users onto the hard drive.  This tests logon with machine account,
        // creation of four users, and storing those users to the hard disk.
        //
        Verify(PopulateUserAccountsHD( NULL, 4 ) == S_OK);

        //
        // Retrieve users from hard drive
        //
        Verify(XOnlineGetUsers( &m_pUsers[0], &m_cUsers ) == S_OK);

        //
        // Set user data for a couple of users.  This tests friend list functionality
        // but is included here since it behaves differently when called before logon,
        // and we want to test that.
        //
        Verify(SUCCEEDED(XOnlineNotificationSetUserData( 0, 4, (PBYTE) "ABC", NULL, NULL)));
        Verify(SUCCEEDED(XOnlineNotificationSetUserData( 1, 4, (PBYTE) "XYZ", NULL, NULL)));

        //
        // Logon with four users
        //
        Verify(XOnlineLogon(m_pUsers, aServices, sizeof(aServices)/sizeof(DWORD), m_hEvents[0], &m_Tasks[0].hTask) == S_OK);
        DoWork(m_Tasks[0].hTask, STANDARD_TIMEOUT);                                                              
        Verify(m_Tasks[0].hr == XONLINE_S_LOGON_CONNECTION_ESTABLISHED);

        //
        // Wait for the second simulated Xbox to finish this unit test
        //
        WaitForSingleObject(m_hEvent2, INFINITE);
        ResetEvent(m_hEvent2);
        SetEvent(m_hEvent1);

        printf(" [OK]\n");
    };

    //
    // *****************************************************************************
    // *****************************************************************************
    // Test_Friends - test buddy list stuff
    // *****************************************************************************
    // *****************************************************************************
    //
    virtual void Test_Friends()
    {
        DWORD                       i                 = 0;
        PXONLINE_FRIEND             pFriends          = NULL;
        DWORD                       dwNumFriends      = 0;
        PXONLINE_LOCKOUTUSER        pLockoutlist      = NULL;
        DWORD                       dwNumLockoutUsers = 0;
        XNKID                       SessionID         = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        XONLINE_ACCEPTED_GAMEINVITE GameInvite;
        DWORD                       dwEndTime;

        printf("Testing Friends ");

        //
        // Allocate memory for friend and block list
        //
        Verify((pFriends = (PXONLINE_FRIEND) LocalAlloc(0, MAX_FRIENDS * sizeof(XONLINE_FRIEND))) != NULL);
        Verify((pLockoutlist = (PXONLINE_LOCKOUTUSER) LocalAlloc(0, MAX_LOCKOUTUSERS * sizeof(XONLINE_LOCKOUTUSER))) != NULL);

        //
        // Start the friends and lockoutlist tasks.
        //
        Verify(SUCCEEDED(XOnlineFriendsStartup( m_hEvents[1], &m_Tasks[1].hTask )));
        Verify(SUCCEEDED(XOnlineLockoutlistStartup( m_hEvents[2], &m_Tasks[2].hTask )));

        //
        // Set state of user
        //
        Verify(SUCCEEDED(XOnlineNotificationSetState(0, XONLINE_FRIENDSTATE_FLAG_ONLINE, SessionID, 0, NULL)));
        Verify(SUCCEEDED(XOnlineNotificationSetState(1, XONLINE_FRIENDSTATE_FLAG_ONLINE, SessionID, 0, NULL)));
        Verify(SUCCEEDED(XOnlineNotificationSetState(2, XONLINE_FRIENDSTATE_FLAG_ONLINE, SessionID, 0, NULL)));
        Verify(SUCCEEDED(XOnlineNotificationSetState(3, XONLINE_FRIENDSTATE_FLAG_ONLINE, SessionID, 0, NULL)));

        //
        // Have the first user invite other three users to be friends.
        //
        Verify(SUCCEEDED(XOnlineFriendsRequest( 0, m_pUsers[1].xuid )));
        Verify(SUCCEEDED(XOnlineFriendsRequest( 0, m_pUsers[2].xuid )));
        Verify(SUCCEEDED(XOnlineFriendsRequest( 0, m_pUsers[3].xuid )));

        //
        // Start enumerating all friend lists
        //
        Verify(SUCCEEDED(XOnlineFriendsEnumerate( 0, m_hEvents[3], &m_Tasks[3].hTask )));
        Verify(SUCCEEDED(XOnlineFriendsEnumerate( 1, m_hEvents[4], &m_Tasks[4].hTask )));
        Verify(SUCCEEDED(XOnlineFriendsEnumerate( 2, m_hEvents[5], &m_Tasks[5].hTask )));
        Verify(SUCCEEDED(XOnlineFriendsEnumerate( 3, m_hEvents[6], &m_Tasks[6].hTask )));

        //
        // Work until all users have correct friends in their lists, or hit the
        // timeout.
        //
        dwEndTime = GetTickCount() + STANDARD_TIMEOUT;
        while (GetTickCount() < dwEndTime)
        {
            DoWork(NULL, 500);
            Verify(NoTaskErrors());
            Verify(m_Tasks[1].hr != XONLINE_S_NOTIFICATION_FRIEND_RESULT);

            //
            // Verify that all users have correct friends in their lists.  If not
            // iterate the loop and work some more.
            //
            dwNumFriends = XOnlineFriendsGetLatest( 0, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 3) continue;
            if (!IsInFriendsList(m_pUsers[1].xuid, pFriends, dwNumFriends)) continue;
            if (!IsInFriendsList(m_pUsers[2].xuid, pFriends, dwNumFriends)) continue;
            if (!IsInFriendsList(m_pUsers[3].xuid, pFriends, dwNumFriends)) continue;

            dwNumFriends = XOnlineFriendsGetLatest( 1, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 1) continue;
            if (!IsInFriendsList(m_pUsers[0].xuid, pFriends, dwNumFriends)) continue;

            dwNumFriends = XOnlineFriendsGetLatest( 2, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 1) continue;
            if (!IsInFriendsList(m_pUsers[0].xuid, pFriends, dwNumFriends)) continue;

            dwNumFriends = XOnlineFriendsGetLatest( 3, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 1) continue;
            if (!IsInFriendsList(m_pUsers[0].xuid, pFriends, dwNumFriends)) continue;

            break;
        }
        Verify(GetTickCount() < dwEndTime);

        //
        // Have one user reply with yes, one reply with no and last user doesn't reply at all.
        //
        Verify(SUCCEEDED(XOnlineFriendsAnswerRequest( 1, &pFriends[0], XONLINE_REQUEST_YES )));
        Verify(SUCCEEDED(XOnlineFriendsAnswerRequest( 2, &pFriends[0], XONLINE_REQUEST_NO )));

        //
        // Work until all users have correct friends in their lists, or hit the
        // timeout.
        //
        dwEndTime = GetTickCount() + STANDARD_TIMEOUT;
        while (GetTickCount() < dwEndTime)
        {
            DoWork(NULL, 500);
            Verify(NoTaskErrors());
            Verify(m_Tasks[1].hr != XONLINE_S_NOTIFICATION_FRIEND_RESULT);

            //
            // Verify that all users have correct friends in their lists.  If not
            // iterate the loop and work some more.
            //
            dwNumFriends = XOnlineFriendsGetLatest( 0, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 2) continue;
            if (!IsInFriendsList(m_pUsers[1].xuid, pFriends, dwNumFriends)) continue;
            if (!IsInFriendsList(m_pUsers[3].xuid, pFriends, dwNumFriends)) continue;
            if (pFriends[0].friendState & XONLINE_FRIENDSTATE_FLAG_CLOAKED) continue;

            dwNumFriends = XOnlineFriendsGetLatest( 1, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 1) continue;
            if (!IsInFriendsList(m_pUsers[0].xuid, pFriends, dwNumFriends)) continue;
            if (!(pFriends[0].friendState & XONLINE_FRIENDSTATE_FLAG_ONLINE)) continue;
            if (pFriends[0].friendState & XONLINE_FRIENDSTATE_FLAG_CLOAKED) continue;

            dwNumFriends = XOnlineFriendsGetLatest( 2, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 0) continue;

            dwNumFriends = XOnlineFriendsGetLatest( 3, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 1) continue;
            if (!IsInFriendsList(m_pUsers[0].xuid, pFriends, dwNumFriends)) continue;

            break;
        }
        Verify(GetTickCount() < dwEndTime);

        //
        // Check if we receive friend errors correctly.  Attempt to add a friend
        // that is already in our friend list.
        //
        Verify(SUCCEEDED(XOnlineFriendsRequest( 0, m_pUsers[1].xuid )));

        //
        // Work until we receive the error code from TaskContinue that indicates
        // there is a friend error.
        //
        dwEndTime = GetTickCount() + STANDARD_TIMEOUT;
        while (GetTickCount() < dwEndTime)
        {
            DoWork(NULL, 500);
            if (m_Tasks[1].hr != XONLINE_S_NOTIFICATION_FRIEND_RESULT) continue;
            Verify(NoTaskErrors());

            DWORD dwUserIndex = 0;
            XUID  xuidTargetUser;

            //
            // Verify that the friend error is correct
            //
            Verify(XOnlineFriendsGetResults(&dwUserIndex, &xuidTargetUser) == 0x80070524);
            Verify(dwUserIndex == 0);
            Verify(*((ULONGLONG *)&xuidTargetUser) == *((ULONGLONG *)&m_pUsers[1].xuid));

            break;
        }
        Verify(GetTickCount() < dwEndTime);

        //
        // Add two users to the lockout list.  Work for a bit to make sure those lockoutlist
        // adds are sent to the server.
        //
        Verify(SUCCEEDED(XOnlineLockoutlistAdd( 0, m_pUsers[1].xuid, m_pUsers[1].name )));
        Verify(SUCCEEDED(XOnlineLockoutlistAdd( 0, m_pUsers[2].xuid, m_pUsers[2].name )));
        DoWork(NULL, 3000);
        Verify(NoTaskErrors());

        //
        // Remove one user from the lockout list and work for a bit to send it
        // to the server.
        //
        Verify(SUCCEEDED(XOnlineLockoutlistRemove( 0, m_pUsers[1].xuid )));
        DoWork(NULL, 3000);
        Verify(NoTaskErrors());

        //
        // Get the lockout list for the first user and verify that it contains one user.
        //
        Verify((dwNumLockoutUsers = XOnlineLockoutlistGet( 0, MAX_LOCKOUTUSERS, pLockoutlist )) == 1);
        
        //
        // Set the state and user data for the first user and work so it gets sent
        // to the server.
        //
        Verify(SUCCEEDED(XOnlineNotificationSetState(0, XONLINE_FRIENDSTATE_FLAG_ONLINE | XONLINE_FRIENDSTATE_FLAG_PLAYING, SessionID, 4, (PBYTE) "DEF")));
        Verify(SUCCEEDED(XOnlineNotificationSetUserData( 0, 4, (PBYTE) "GHI", m_hEvents[7], &m_Tasks[7].hTask)));
        DoWork(m_Tasks[7].hTask, STANDARD_TIMEOUT);
        Verify(m_Tasks[7].hr == XONLINETASK_S_SUCCESS);
        Verify(NoTaskErrors());
        XOnlineTaskClose(m_Tasks[7].hTask);
        m_Tasks[7].hTask = NULL;

        //
        // Work until the state is updated correctly in the friend list, or hit the
        // timeout.
        //
        dwEndTime = GetTickCount() + STANDARD_TIMEOUT;
        while (GetTickCount() < dwEndTime)
        {
            DoWork(NULL, 500);
            Verify(NoTaskErrors());

            //
            // Verify that the state is correct
            //
            dwNumFriends = XOnlineFriendsGetLatest( 1, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 1) continue;
            if (!IsInFriendsList(m_pUsers[0].xuid, pFriends, dwNumFriends)) continue;
            if (!(pFriends[0].friendState & XONLINE_FRIENDSTATE_FLAG_PLAYING)) continue;
            if (pFriends[0].StateData[0] != 'D' || pFriends[0].StateData[1] != 'E' || pFriends[0].StateData[2] != 'F') continue;
            if (pFriends[0].UserData[0] != 'G' || pFriends[0].UserData[1] != 'H' || pFriends[0].UserData[2] != 'I') continue;

            break;
        }
        Verify(GetTickCount() < dwEndTime);

        //
        // Retrieve the user's latest friend list so we can invite a friend
        // to join our game session.
        //
        dwNumFriends = XOnlineFriendsGetLatest( 0, MAX_FRIENDS, pFriends );

        //
        // Find User #1 in our friend list.
        //
        for (i = 0; i < dwNumFriends; i += 1)
        {
            if (*((ULONGLONG *)&pFriends[i].xuid) == *((ULONGLONG *)&m_pUsers[1].xuid))
            {
                break;
            }
        }

        //
        // Invite User #2 to a game session.
        //
        Verify(SUCCEEDED(XOnlineFriendsGameInvite( 0, SessionID, 1, &pFriends[i] )));

        //
        // Work until the state is updated correctly in the friend list, or hit the
        // timeout.
        //
        dwEndTime = GetTickCount() + STANDARD_TIMEOUT;
        while (GetTickCount() < dwEndTime)
        {
            DoWork(NULL, 500);
            Verify(NoTaskErrors());

            //
            // Verify that all users have correct buddies in their lists
            //
            dwNumFriends = XOnlineFriendsGetLatest( 0, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 2) continue;
            if (!IsInFriendsList(m_pUsers[1].xuid, pFriends, dwNumFriends)) continue;
            if (!(pFriends[i].friendState & XONLINE_FRIENDSTATE_FLAG_SENTINVITE)) continue;
            if (pFriends[i].friendState & XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE) continue;
            if (pFriends[i].friendState & XONLINE_FRIENDSTATE_FLAG_INVITEACCEPTED) continue;
            if (pFriends[i].friendState & XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED) continue;

            dwNumFriends = XOnlineFriendsGetLatest( 1, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 1) continue;
            if (!IsInFriendsList(m_pUsers[0].xuid, pFriends, dwNumFriends)) continue;
            if (!(pFriends[0].friendState & XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE)) continue;
            if (pFriends[0].friendState & XONLINE_FRIENDSTATE_FLAG_SENTINVITE) continue;
            if (pFriends[0].friendState & XONLINE_FRIENDSTATE_FLAG_INVITEACCEPTED) continue;
            if (pFriends[0].friendState & XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED) continue;

            break;
        }
        Verify(GetTickCount() < dwEndTime);

        //
        // Retrieve the second user's latest buddy list and answer the game invite
        // that was sent.  Then close the enumeration task handle and pump the friends
        // task handle to send the game invite response to the other user.
        //
        Verify(SUCCEEDED(XOnlineFriendsAnswerGameInvite( 1, &pFriends[0], XONLINE_GAMEINVITE_NO)));

        //
        // Invite User #1 to a game session.
        //
        Verify(SUCCEEDED(XOnlineFriendsGameInvite(1, SessionID, 1, &pFriends[0] )));

        dwEndTime = GetTickCount() + STANDARD_TIMEOUT;
        while (GetTickCount() < dwEndTime)
        {
            DoWork(NULL, 500);
            Verify(NoTaskErrors());

            //
            // Verify that the state is correct
            //
            dwNumFriends = XOnlineFriendsGetLatest( 0, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 2) continue;
            if (!IsInFriendsList(m_pUsers[1].xuid, pFriends, dwNumFriends)) continue;
            if (!(pFriends[i].friendState & XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED)) continue;
            if (!(pFriends[i].friendState & XONLINE_FRIENDSTATE_FLAG_SENTINVITE)) continue;
            if (!(pFriends[i].friendState & XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE)) continue;
            if (pFriends[i].friendState & XONLINE_FRIENDSTATE_FLAG_INVITEACCEPTED) continue;

            dwNumFriends = XOnlineFriendsGetLatest( 1, MAX_FRIENDS, pFriends );
            if (dwNumFriends != 1) continue;
            if (!IsInFriendsList(m_pUsers[0].xuid, pFriends, dwNumFriends)) continue;
            if (!(pFriends[0].friendState & XONLINE_FRIENDSTATE_FLAG_SENTINVITE)) continue;
            if (pFriends[0].friendState & XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE) continue;
            if (pFriends[0].friendState & XONLINE_FRIENDSTATE_FLAG_INVITEACCEPTED) continue;
            if (pFriends[0].friendState & XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED) continue;

            break;
        }
        Verify(GetTickCount() < dwEndTime);

        //
        // Retrieve the accepted game invite from the hard disk.
        // BUGBUG: This currently isn't a really valid test since the game invite won't be written
        // to the hard disk if it came from in-game.  The API that changes the title ID should
        // be called with a different title ID in order to simulate this functionality.
        //
        Verify(SUCCEEDED(XOnlineFriendsGetAcceptedGameInvite( m_hEvents[7], &m_Tasks[7].hTask )));
        
        dwEndTime = GetTickCount() + STANDARD_TIMEOUT;
        while (GetTickCount() < dwEndTime)
        {
            DoWork(m_Tasks[7].hTask, 500);
            Verify(NoTaskErrors());

            if (m_Tasks[7].hr != XONLINETASK_S_SUCCESS) continue;

            break;
        }
        Verify(GetTickCount() < dwEndTime);

        Verify(SUCCEEDED(XOnlineFriendsGetAcceptedGameInviteResult( m_Tasks[7].hTask, &GameInvite )));
        XOnlineTaskClose(m_Tasks[7].hTask);
        m_Tasks[7].hTask = NULL;

        //
        // Close all opened task handles, except logon task handle at index 0
        //
        for (i = 1; i < NUM_TASKS; i += 1)
        {
            if (m_Tasks[i].hTask != NULL)
            {
                XOnlineTaskClose(m_Tasks[i].hTask);
                m_Tasks[i].hTask = NULL;
            }
        }

        //
        // Free the buddy list and block list memory
        //
        LocalFree((HLOCAL) pFriends);
        LocalFree((HLOCAL) pLockoutlist);

        //
        // Wait for the second simulated Xbox to finish this unit test
        //
        WaitForSingleObject(m_hEvent2, INFINITE);
        ResetEvent(m_hEvent2);
        SetEvent(m_hEvent1);

        printf(" [OK]\n");
    };

    //
    // *****************************************************************************
    // *****************************************************************************
    // Test_Matchmaking - test matchmaking session create and search
    // *****************************************************************************
    // *****************************************************************************
    //
    virtual void Test_Matchmaking()
    {
        XNKEY                KeyExchangeKey;
        XONLINE_ATTRIBUTE    Attribs[20];
        XNKID                SessionID       = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        PXMATCH_SEARCHRESULT *ppSearchResult = NULL;
        DWORD                dwNumResults    = 0;
        DWORD                i               = 0;

        printf("Testing Matchmaking ");

        //
        // Create a set of attributes to advertise with the new session
        //
        Attribs[0].dwAttributeID = X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000000;
        Attribs[0].info.integer.qwValue = 1234;
        Attribs[1].dwAttributeID = X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_DATATYPE_STRING | 0x00000001;
        Attribs[1].info.string.pwszValue = L"MYSTRING111";
        Attribs[2].dwAttributeID = X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_DATATYPE_BLOB | 0x00000002;
        Attribs[2].info.blob.pvValue = (VOID *) "MYBLOB111";
        Attribs[2].info.blob.dwLength = 9;
        Attribs[3].dwAttributeID = X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_DATATYPE_STRING | 0x00000003;
        Attribs[3].info.string.pwszValue = L"MYSTRING222";
        Attribs[4].dwAttributeID = X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_DATATYPE_STRING | 0x00000004;
        Attribs[4].info.string.pwszValue = L"MYSTRING333";
        Attribs[5].dwAttributeID = X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_DATATYPE_BLOB | 0x00000005;
        Attribs[5].info.blob.pvValue = (VOID *) "MYBLOB222";
        Attribs[5].info.blob.dwLength = 9;
        Attribs[6].dwAttributeID = X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000006;
        Attribs[6].info.integer.qwValue = 5678;

        //
        // Create a new session and send it to the server.  Then get the SessionID and KeyExchangeKey for that
        // new session.
        //
        Verify(SUCCEEDED(XOnlineMatchSessionCreate( 1, 2, 3, 4, 7, Attribs, m_hEvents[1], &m_Tasks[1].hTask )));
        DoWork(m_Tasks[1].hTask, STANDARD_TIMEOUT);
        Verify(NoTaskErrors());

        Verify(SUCCEEDED(XOnlineMatchSessionGetInfo(m_Tasks[1].hTask, &SessionID, &KeyExchangeKey)));
        XOnlineTaskClose(m_Tasks[1].hTask);
        m_Tasks[1].hTask = NULL;

        //
        // Update the session that was just created
        //
        Verify(SUCCEEDED(XOnlineMatchSessionUpdate( SessionID, 1, 2, 3, 4, 7, Attribs, m_hEvents[1], &m_Tasks[1].hTask )));
        DoWork(m_Tasks[1].hTask, STANDARD_TIMEOUT);
        Verify(NoTaskErrors());

        XOnlineTaskClose(m_Tasks[1].hTask);
        m_Tasks[1].hTask = NULL;

        //
        // Create a set of parameters to pass up to the session search request.
        //
        Attribs[0].dwAttributeID = X_ATTRIBUTE_DATATYPE_INTEGER;
        Attribs[0].info.integer.qwValue = 1234;
        Attribs[1].dwAttributeID = X_ATTRIBUTE_DATATYPE_STRING;
        Attribs[1].info.string.pwszValue = L"MYSTRING111";
        Attribs[2].dwAttributeID = X_ATTRIBUTE_DATATYPE_BLOB;
        Attribs[2].info.blob.pvValue = (VOID *) "MYBLOB111";
        Attribs[2].info.blob.dwLength = 9;
        Attribs[3].dwAttributeID = X_ATTRIBUTE_DATATYPE_STRING;
        Attribs[3].info.string.pwszValue = L"MYSTRING222";

        //
        // Create a new session search request and send it to the server.
        //
        Verify(SUCCEEDED(XOnlineMatchSearch( 1, 50, 4, Attribs, 5000, m_hEvents[1], &m_Tasks[1].hTask )));
        DoWork(m_Tasks[1].hTask, STANDARD_TIMEOUT);
        Verify(NoTaskErrors());

        //
        // Get the results of the search request.
        //
        Verify(SUCCEEDED(XOnlineMatchSearchGetResults(m_Tasks[1].hTask, &ppSearchResult, &dwNumResults)));
        XOnlineTaskClose(m_Tasks[1].hTask);
        m_Tasks[1].hTask = NULL;

        //
        // Wait for the second simulated Xbox to finish this unit test
        //
        WaitForSingleObject(m_hEvent2, INFINITE);
        ResetEvent(m_hEvent2);
        SetEvent(m_hEvent1);

        printf(" [OK]\n");
    };
};

//
// Second simulated Xbox.  This will only contain unit tests that require two simulated Xboxes.
// Tests that don't require two simulated Xboxes should use the same function template as
// Test_Matchmaking.
//
class CTestXbox2 : public CTestXbox
{
public:
    CTestXbox2(char *pszXbox) : CTestXbox(pszXbox) {};

    //
    // *****************************************************************************
    // *****************************************************************************
    // Test_Account_Create - test account creation and logon
    // *****************************************************************************
    // *****************************************************************************
    //
    virtual void Test_Account_Create()
    {
        //
        // Second simulated Xbox is not currently supported.  COnlineAccounts contains
        // bugs that prevent it from being instantiated more than once.
        //

        //
        // Tell first simulated Xbox that we're done and wait for it to respond
        // by setting its event.
        //
        SetEvent(m_hEvent2);
        WaitForSingleObject(m_hEvent1, INFINITE);
        ResetEvent(m_hEvent1);
    };

    //
    // *****************************************************************************
    // *****************************************************************************
    // Test_Friends - test buddy list stuff
    // *****************************************************************************
    // *****************************************************************************
    //
    virtual void Test_Friends()
    {
        //
        // Second simulated Xbox is not currently supported.  COnlineAccounts contains
        // bugs that prevent it from being instantiated more than once.
        //

        //
        // Tell first simulated Xbox that we're done and wait for it to respond
        // by setting its event.
        //
        SetEvent(m_hEvent2);
        WaitForSingleObject(m_hEvent1, INFINITE);
        ResetEvent(m_hEvent1);
    };

    //
    // *****************************************************************************
    // *****************************************************************************
    // Test_Matchmaking - test matchmaking session create and search
    // *****************************************************************************
    // *****************************************************************************
    //
    virtual void Test_Matchmaking()
    {
        //
        // Second simulated Xbox is not currently supported.  COnlineAccounts contains
        // bugs that prevent it from being instantiated more than once.
        //

        //
        // Tell first simulated Xbox that we're done and wait for it to respond
        // by setting its event.
        //
        SetEvent(m_hEvent2);
        WaitForSingleObject(m_hEvent1, INFINITE);
        ResetEvent(m_hEvent1);
    };
};

int     g_argc;
char ** g_argv;
BOOL    g_fNicSpecified;

//
// Helper function to see if unit test should be run or not.  If unit tests
// are specified on the command line, only those tests will run.
//
BOOL RunTest(char * pszName)
{
    if (g_argc == 1 || (g_argc == 2 && g_fNicSpecified == TRUE))
    {
        return(TRUE);
    }

    for (int i = 1; i < g_argc; i += 1)
    {
        if (g_argv[i] && lstrcmpiA(g_argv[i], pszName) == 0)
            return(TRUE);
    }

    return(FALSE);
}

//
// Helper function to get the Nic number to use.  If a number is
// specified on the command line, use that Nic.  If not, read it
// from the registry, if it exists.  Write it to the registry
// afterwards.
//
VOID GetNicNumber(DWORD *pdwNic)
{
    DWORD cbNicNum = sizeof(*pdwNic);
    HKEY  hKey1    = NULL;
    HKEY  hKey2    = NULL;

    *pdwNic = 0;
    g_fNicSpecified = FALSE;

    //
    // Look for a single digit specified on the command line.  If
    // one exists, use it as the Nic number.
    //
    for (int i = 1; i < g_argc; i += 1)
    {
        if (g_argv[i] && g_argv[i][0] >= '0' && g_argv[i][0] <= '9')
        {
            *pdwNic = g_argv[i][0] - '0';
            g_fNicSpecified = TRUE;
        }
    }

    //
    // Open the registry key and create the XboxSDK key if it doesn't
    // exist.
    //
    RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\Microsoft", 0, KEY_ALL_ACCESS, &hKey1);
    RegCreateKeyEx( hKey1, L"XboxSDK", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey2, NULL);
    RegCloseKey(hKey1);

    //
    // If a Nic was not specified on the command line, try getting it from the
    // registry
    //
    if (g_fNicSpecified == FALSE)
    {
        RegQueryValueEx( hKey2, L"SmokeTestNicNum", NULL, NULL, (LPBYTE) pdwNic, &cbNicNum);
    }
    
    //
    // Write Nic number to the registry
    //
    RegSetValueEx( hKey2, L"SmokeTestNicNum", 0, REG_DWORD, (BYTE *)pdwNic, sizeof(DWORD));
    RegCloseKey(hKey2);
}

//
// Thread proc that will run the unit tests on each of the two simulated Xboxes.
//
DWORD WINAPI TestThreadProc(void *pv)
{
    CTestXbox *pXbox = (CTestXbox *) pv;

    //
    // =============================================================================
    // =============================================================================
    // Test case list - Add test cases here
    //
    // Account_Create test must always run.  Other tests should be conditional
    // on command line parameters.  If no command line parameters are specified
    // then it defaults to all tests.
    //
                                          pXbox->Test_Account_Create();
    if (RunTest("Friends"))               pXbox->Test_Friends();
    if (RunTest("Matchmaking"))           pXbox->Test_Matchmaking();
    //
    // End of test case list
    // =============================================================================
    // =============================================================================
    //

    return(0);
}

//
// Main.  Sets up two simulated Xboxes and starts the threads that run through
// the unit tests.
//
int __cdecl main(int argc, char * argv[])
{
    HANDLE hThread1 = NULL;
    HANDLE hThread2 = NULL;
    HANDLE hEvent1 = NULL;
    HANDLE hEvent2 = NULL;
    char szXboxName1[128];
    char szXboxName2[128];
    CTestXbox1 *pXbox1;
    CTestXbox2 *pXbox2;
    DWORD dwNic = 0;

    g_argc = argc;
    g_argv = argv;

    //
    // Initialize VLan
    //
    Verify(VLanInit());

    Verify((hEvent1 = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL); ResetEvent(hEvent1);
    Verify((hEvent2 = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL); ResetEvent(hEvent2);

    //
    // Get Nic number from command line or registry, if one is specified
    //
    GetNicNumber(&dwNic);

    //
    // Create name of two simulated Xboxes.  It's based on the computer name of the system
    // you are running the test on.
    //
    sprintf(szXboxName1, "%s_1@Nic/%d", getenv("COMPUTERNAME"), dwNic);
    sprintf(szXboxName2, "%s_2@Nic/%d", getenv("COMPUTERNAME"), dwNic);

    printf("Xbox 1: '%s'\nXbox 2: '%s'\n\n", szXboxName1, szXboxName2);

    //
    // Instantiate two simulated Xbox classes
    //
    pXbox1 = new CTestXbox1(szXboxName1);
    pXbox2 = new CTestXbox2(szXboxName2);

    //
    // Initialize the Xboxes
    //
    pXbox1->StartupTest(pXbox2, hEvent1, hEvent2);
    pXbox2->StartupTest(pXbox1, hEvent1, hEvent2);

    //
    // Create the two threads that will run through the unit tests
    //
    Verify((hThread1 = CreateThread(NULL, 0, TestThreadProc, pXbox1, 0, NULL)) != NULL);
    Verify((hThread2 = CreateThread(NULL, 0, TestThreadProc, pXbox2, 0, NULL)) != NULL);

    //
    // Wait for them to complete.
    //
    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    //
    // Call cleanup functions on the two simulated Xboxes.
    //
    pXbox1->TermTest();
    pXbox2->TermTest();

    CloseHandle(hEvent1);
    CloseHandle(hEvent2);
    CloseHandle(hThread1);
    CloseHandle(hThread2);

    VLanTerm();

    return(0);
}
