// ---------------------------------------------------------------------------------------
// conline.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#if DBG
#define XNET_FEATURE_TRACE 1
#endif

#include <xapip.h>
#include <xtl.h>
//#include <windows.h>
//#include <xbox.h>
//#include <winsockx.h>
//#include <winsockp.h>
#include <xonlinep.h>
#include <stdlib.h>
#include <stdio.h>
#include <xn.h>
#include <OnlineAccounts.h>

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
    HRESULT hrLogon;
    DWORD i,j;
    XONLINE_USER users[XONLINE_MAX_STORED_ONLINE_USERS];
    DWORD cUsers;
	XONLINETASK_HANDLE hTask;
    XONLINETASK_HANDLE  hGetTagsTask;
    LPWSTR rgszTags = NULL;
    WORD  wTagCount = 0;
    DWORD aServices[] = {
        XONLINE_BILLING_OFFERING_SERVICE,
        XONLINE_USER_ACCOUNT_SERVICE,
        XONLINE_AUTO_UPDATE_SERVICE,
        XONLINE_MATCHMAKING_SERVICE,
        };
    DWORD dwDestTickCount;
    
#ifdef XNET_FEATURE_TRACE
    Tag(Verbose) = TAG_ENABLE;
    Tag(Warning) = TAG_ENABLE;

    //ExternTag(ScratchMemoryVerbose);
    //ExternTag(ScratchMemoryAlloc);
    
    //Tag(ScratchMemoryVerbose) = TAG_ENABLE;
    //Tag(ScratchMemoryAlloc) = TAG_ENABLE;
#endif

    //
    // Initialize core peripheral port support
    //
    XInitDevices(0,NULL);

    //
    // Sleep a while to make sure the USB is ready
    //
    Sleep (1000);

    printf("Testing CTestXOnline");

    hr = XOnlineStartup(NULL);
    Verify(hr == S_OK);

	hEventWorkAvailable = CreateEvent(NULL, FALSE, FALSE, NULL);
	Verify(hEventWorkAvailable != NULL);

    RtlZeroMemory( users, sizeof(users) );

#if 0
	hr = COnlineAccounts::Instance()->PopulateUserAccountsHD( NULL, 4 );
	Verify( hr == S_OK );
#endif

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

    hr = XOnlineStartup(NULL);
    Verify(hr == S_OK);

    hr = XOnlineCleanup();
    Verify(hr == S_OK);

    hr = XOnlineStartup(NULL);
    Verify(hr == S_OK);

    hr = XOnlineCleanup();
    Verify(hr == S_OK);

    hr = XOnlineStartup(NULL);
    Verify(hr == S_OK);

    hr = XOnlineGetUsers( &users[0], &cUsers );
    Verify(hr == S_OK);

    hr = XOnlineCleanup();
    Verify(hr == S_OK);

    for (j=0; j<5; ++j)
    {
        hr = XOnlineStartup(NULL);
        Verify(hr == S_OK);

        hr = XOnlineGetUsers( &users[0], &cUsers );
        Verify(hr == S_OK);

        for (i=0; i<cUsers; ++i)
        {
            TraceSz2( Verbose, "XOnlineGetUsers returned user %s with options 0x%08X", users[i].name, users[i].dwUserOptions);
        }
        
#if 0
#if 0
        users[1].xuid.qwUserID = 0;
        RtlCopyMemory( &users[3], &users[0], sizeof(XONLINE_USER) );
        SetAsGuestNumber(users[3].xuid.dwUserFlags, 1);
#else
        RtlZeroMemory( users, sizeof(users) );
#endif
#endif
        RtlZeroMemory( users, sizeof(users[0]) * j );

        hr = XOnlineLogon( &users[0], aServices, sizeof(aServices)/sizeof(DWORD), hEventWorkAvailable, &hTask );
        Verify(hr == S_OK);

        do
        {
            WaitForSingleObject(hEventWorkAvailable, INFINITE);

            hr = XOnlineTaskContinue(hTask);

        } while (hr == XONLINETASK_S_RUNNING);

        hrLogon = XOnlineLogonTaskGetResults( hTask );

        Verify(SUCCEEDED(hrLogon));

		hr = _XOnlineGetTags(103, 5, NULL, hEventWorkAvailable, &hGetTagsTask);

        Verify(SUCCEEDED(hr));

        do
        {
            WaitForSingleObject(hEventWorkAvailable, INFINITE);

            hr = XOnlineTaskContinue(hTask);

            Verify(SUCCEEDED(hr));

            hr = XOnlineTaskContinue(hGetTagsTask);
        
            Verify(SUCCEEDED(hr));

        } while (hr == XONLINETASK_S_RUNNING);

        if (SUCCEEDED(hr))
        {
            hr = _XOnlineGetTagsResults(hGetTagsTask, &rgszTags, &wTagCount);
            Verify(SUCCEEDED(hr));

            for (int i = 0; i < wTagCount; i++)
            {
                TraceSz2( Verbose, "_XOnlineGetTagsResults returned name %d: %ls", i, rgszTags + i*XONLINE_KINGDOM_SIZE);
            }
        }

        hr = XOnlineTaskClose(hGetTagsTask);
        Verify(hr == S_OK);    

        hr = XOnlineTaskClose( hTask );
        Verify(hr == S_OK);    

        hr = XOnlineCleanup();
        Verify(hr == S_OK);
    }

    #if 0
    hr = XOnlineStartup(NULL);
    Verify(hr == S_OK);

    RtlZeroMemory( users, sizeof(users) );

    hr = XOnlineLogon( &users[0], aServices, sizeof(aServices)/sizeof(DWORD), hEventWorkAvailable, &hTask );
    Verify(hr == S_OK);

    do
    {
        WaitForSingleObject(hEventWorkAvailable, INFINITE);

        hr = XOnlineTaskContinue(hTask);

    } while (hr == XONLINETASK_S_RUNNING);

    dwDestTickCount = GetTickCount()+3000;
    do
    {
        WaitForSingleObject(hEventWorkAvailable, INFINITE);

        XOnlineTaskContinue(hTask);

    } while (GetTickCount() < dwDestTickCount);

    Verify(SUCCEEDED(hr));

    hr = XOnlineLogonTaskGetResults( hTask );
    Verify(SUCCEEDED(hr));

    hr = XOnlineTaskClose( hTask );
    Verify(hr == S_OK);    

    hr = XOnlineGetUsers( &users[0], &cUsers );
    Verify(hr == S_OK);

    for (i=0; i<cUsers; ++i)
    {
        TraceSz2( Verbose, "XOnlineGetUsers returned user %s with options 0x%08X", users[i].name, users[i].dwUserOptions);
    }
    
    hr = XOnlineLogon( &users[0], aServices, sizeof(aServices)/sizeof(DWORD), hEventWorkAvailable, &hTask );
    Verify(hr == S_OK);

    do
    {
        WaitForSingleObject(hEventWorkAvailable, INFINITE);

        hr = XOnlineTaskContinue(hTask);

    } while (hr == XONLINETASK_S_RUNNING);

    dwDestTickCount = GetTickCount()+3000;
    do
    {
        WaitForSingleObject(hEventWorkAvailable, INFINITE);

        XOnlineTaskContinue(hTask);

    } while (GetTickCount() < dwDestTickCount);

    Verify(SUCCEEDED(hr));

    hr = XOnlineLogonTaskGetResults( hTask );
    Verify(SUCCEEDED(hr));

    hr = XOnlineTaskClose( hTask );
    Verify(hr == S_OK);    

    hr = XOnlineCleanup();
    Verify(hr == S_OK);
    #endif
    
    TraceSz( Verbose, "Xbox online auth test passed!");
    
    return 0;
}


HRESULT CheckPIN()
{
    HRESULT hr = E_FAIL;
    DWORD err;
    DWORD i,c;
    DWORD devices;
    XPININPUTHANDLE hPinInputHandle;
    XINPUT_STATE inputState;
    HANDLE hInputDevice;
    BYTE pinByte;
    
    //
    //  Initialize core peripheral port support
    //
    XInitDevices(0,NULL);

    //
    // Get all connected gamepads.
    //
    c = XGetPortCount ();
    devices = XGetDevices (XDEVICE_TYPE_GAMEPAD);
    hInputDevice = 0;
    
    for (i = 0; i < c; i++) {
        if ((1 << i) & devices) {
            //
            // Open the device.
            //
            TraceSz1( Verbose, "Gamepad found on port %u. Opening...", i);
            hInputDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0 + i, XDEVICE_NO_SLOT, NULL);
            if (hInputDevice)
            {
                TraceSz2( Verbose, "Gamepad successfully opened. (%u/%08x)", i, hInputDevice);
                break;
            }
            else
            {
                TraceSz1( Warning, "Gamepad failed opened. (%u)", i );
            }
        }
    }
    if ( hInputDevice == 0 )
    {
        TraceSz( Warning, "Can't find a Gamepad device");
        goto Cleanup;
    }
    
    err = XInputGetState( hInputDevice, &inputState );
    if ( err )
    {
        TraceSz1( Warning, "XInputGetState failed with %X", err);
        goto Cleanup;
    }

    hPinInputHandle = XOnlinePINStartInput( &inputState );
    if ( hPinInputHandle == 0 )
    {
        TraceSz( Warning, "XOnlinePINStartInput failed");
        goto Cleanup;
    }

    //
    // Get 4 key presses
    //
    i = 0;
    while ( i < 4 )
    {
        err = XInputGetState( hInputDevice, &inputState );
        if ( err )
        {
            TraceSz1( Warning, "XInputGetState failed with %X", err);
            goto Cleanup;
        }

        hr = XOnlinePINDecodeInput( hPinInputHandle, &inputState, &pinByte );
        if ( FAILED( hr ) )
        {
            TraceSz1( Warning, "XOnlinePINDecodeInput failed with %X", hr);
            goto Cleanup;
        }
        if ( hr == S_OK )
        {
            TraceSz1( Verbose, "Got PIN byte %d", pinByte );
            ++i;
        }
    }

    XOnlinePINEndInput( hPinInputHandle );
    
    XInputClose( hInputDevice );

    hr = S_OK;

Cleanup:

    return hr;
}
