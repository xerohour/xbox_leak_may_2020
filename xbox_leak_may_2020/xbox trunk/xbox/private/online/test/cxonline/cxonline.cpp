// ---------------------------------------------------------------------------------------
// conline.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include <windows.h>
#include <xbox.h>
#include <winsockx.h>
#include <winsockp.h>
#include <xonlinep.h>
#include <stdlib.h>
#include <stdio.h>

#include <OnlineAccounts.h>

__forceinline void DbgBreak()
{
    _asm int 3;
}

#define Verify(x) do { if (!(x)) DbgBreak(); } while (0)

#ifdef _XBOX
int __cdecl main(int argc, char * argv[])
#else
class CTestXOnline : public COnlineAccounts
{
private:
    CTestXOnline(char * pszXbox = NULL) : COnlineAccounts(pszXbox) {}

	static CTestXOnline* m_pInstance;

public:
    int Main(int argc, char * argv[]);

	static CTestXOnline* Instance( LPSTR szName = NULL ) {
		if ( NULL == m_pInstance )
		{
			m_pInstance = new CTestXOnline( szName );
			ASSERT( m_pInstance );
		}

		return m_pInstance;
	}

	~CTestXOnline( void ) { delete m_pInstance; }
		
};

CTestXOnline* CTestXOnline::m_pInstance = NULL;

int __cdecl main(int argc, char * argv[])
{
    int result = 0;
    char buffer[128];

    sprintf(buffer, "%s@Nic/0", getenv("COMPUTERNAME") );
    //CTestXOnline * pTestXOnline = new CTestXOnline(buffer);
    //pTestXOnline->Main(argc, argv);
	CTestXOnline::Instance(buffer)->Main( argc, argv );
    //delete pTestXOnline;
    return(result);
}

int CTestXOnline::Main(int argc, char * argv[])
#endif
{
    HANDLE hEventWorkAvailable;
    HRESULT hr, hrTask = XONLINETASK_S_SUCCESS;
    XONLINE_USER users[XONLINE_MAX_STORED_ONLINE_USERS];
    DWORD cUsers;
	XONLINETASK_HANDLE hTask;
    DWORD aServices[] = {
        XONLINE_BILLING_OFFERING_SERVICE,
        XONLINE_USER_ACCOUNT_SERVICE,
        XONLINE_AUTO_UPDATE_SERVICE,
        XONLINE_MATCHMAKING_SERVICE,
        };

    printf("Testing CTestXOnline");

    hr = XOnlineStartup(NULL);
    Verify(hr == S_OK);

	hEventWorkAvailable = CreateEvent(NULL, FALSE, FALSE, NULL);
	Verify(hEventWorkAvailable != NULL);

    RtlZeroMemory( users, sizeof(users) );

	hr = COnlineAccounts::Instance()->PopulateUserAccountsHD( NULL, 1 );
	Verify( hr == S_OK );
	
    hr = XOnlineGetUsers( &users[0], &cUsers );
    Verify(hr == S_OK);

    hr = XOnlineLogon(users, aServices, sizeof(aServices)/sizeof(DWORD), hEventWorkAvailable, &hTask);
    Verify(hr == S_OK);

    //
	// Pump until logon task completes
	//
	do
	{
	    Sleep(10);
	    
	    WaitForSingleObject(hEventWorkAvailable, INFINITE);

        ResetEvent( hEventWorkAvailable );
        
		hr = XOnlineTaskContinue(hTask);

	} while (hr == XONLINETASK_S_RUNNING);

    hrTask = hr;

    hr = XOnlineTaskClose(hTask);
    Verify(hr == S_OK);

    hr = XOnlineCleanup();
    Verify(hr == S_OK);

    if (FAILED(hrTask))
        printf(" [FAILED %08lX]\n", hrTask);
    else
        printf(" [OK]\n");

    return (FAILED(hrTask));
}
