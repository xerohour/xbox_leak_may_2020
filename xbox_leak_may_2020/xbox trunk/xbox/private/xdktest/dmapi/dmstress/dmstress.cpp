// DMAPI stress
//

#include <windows.h>
#include <stdio.h>
#include <crtdbg.h>
#include "xboxdbg.h"

#define DMSTRESSMSG(str) printf(str "\n")
#define DMSTRESSMSGWAIT printf(".")

DWORD __stdcall XboxNotificationHandler(ULONG dwNotification, DWORD dwParam);
DWORD __stdcall XboxNotificationProcessor(LPCSTR szNotification);
void ReconnectToXbox();
void XboxRerun(char *, char *);

// global variables
PDM_CONNECTION g_connection = NULL;
PDMN_SESSION g_session = NULL;
HANDLE g_hevtReboot;
int g_reconnect = 0;
HRESULT hr;

void __cdecl main(int argc, char *argv[])
{
    int count = 0;

	try
	{
		ReconnectToXbox();
		
		g_hevtReboot = CreateEvent(NULL, FALSE, FALSE, NULL);

		while (1)
		{	if (argc == 3)
				XboxRerun(argv[1], argv[2]);
			else
			{
				printf("USAGE: DMSTRESS [path] [xbe]");
				printf("defaulting to: E:\\samples\\fire fire.xbe");
				XboxRerun("E:\\samples\\fire", "fire.xbe");
			}
            printf("DmStress: [%d]\n", count);
            count++;
			Sleep(100);
		}
	}

	catch(...)
	{
        printf("*** Catch block invoked ***\n");
		CloseHandle(g_hevtReboot);

		DmNotify(g_session, DM_NONE, NULL);
		DmCloseNotificationSession(g_session);
		DmCloseConnection(g_connection);
	}
}

#define REPORTIFFAILED(expr) \
{ \
	hr = expr; \
	if (FAILED(hr)) \
	{ \
        char szErr[1024], szBuf[1024]; \
		DmTranslateErrorA(hr, szErr, 1024); \
		sprintf(szBuf, "FAILED: %s [%s]\n", #expr, szErr); \
		printf(szBuf); \
		Sleep(3000); \
	} \
    else \
		printf("SUCCEEDED: " #expr "\n"); \
}

void ReconnectToXbox()
{
	if (g_reconnect)
        printf("Calls to ReconnectToXbox() = %d\n", g_reconnect);

	g_reconnect++;

	if (g_session)
	{
		REPORTIFFAILED(DmNotify(g_session, DM_NONE, NULL));
		REPORTIFFAILED(DmCloseNotificationSession(g_session));
		g_session = NULL;
	}
	if (g_connection)
	{
		REPORTIFFAILED(DmCloseConnection(g_connection));
		g_connection = NULL;
	}

	REPORTIFFAILED(DmOpenConnection(&g_connection));
	REPORTIFFAILED(DmUseSharedConnection(TRUE));
	REPORTIFFAILED(DmOpenNotificationSession(DM_PERSISTENT, &g_session));
	//REPORTIFFAILED(DmNotify(g_session, DM_DEBUGSTR, XboxNotificationHandler));
	REPORTIFFAILED(DmNotify(g_session, DM_EXCEPTION, XboxNotificationHandler));
	//REPORTIFFAILED(DmNotify(g_session, DM_ASSERT, XboxNotificationHandler));
	//REPORTIFFAILED(DmNotify(g_session, DM_RIP, XboxNotificationHandler));
	REPORTIFFAILED(DmNotify(g_session, DM_EXEC, XboxNotificationHandler));
	//REPORTIFFAILED(DmRegisterNotificationProcessor(g_session, "Gravity", XboxNotificationProcessor));

	REPORTIFFAILED(DmConnectDebugger(TRUE));
}


DWORD __stdcall XboxNotificationHandler(ULONG dwNotification, DWORD dwParam)
{
	// Make sure there's no way we get in here twice.
	static bool inHere = false;
	if (inHere)
		return 0;
	inHere = true;
	
	switch (dwNotification & DM_NOTIFICATIONMASK)
	{
		case DM_DEBUGSTR:
		{
			DMSTRESSMSG("XboxNotificationHandler: DM_DEBUGSTR");
			DMN_DEBUGSTR* info = (DMN_DEBUGSTR*) dwParam;
			break;
		}

		case DM_EXCEPTION:
		{
			DMSTRESSMSG("XboxNotificationHandler: DM_EXCEPTION");
			break;
		}

		case DM_ASSERT:
		{
			DMSTRESSMSG("XboxNotificationHandler: DM_ASSERT");
			if (dwNotification & DM_STOPTHREAD)
			{
				/*
				int ret = MessageBox(str, MB_ABORTRETRYIGNORE);
				if (ret == IDABORT)
				{
					RebootXbox(true);
				}
				else
				{
					DmGo();
				}
				*/
			}
			break;
		}
		case DM_RIP:
		{
			DMSTRESSMSG("XboxNotificationHandler: DM_RIP");
			break;
		}

		case DM_EXEC:
		{
			switch (dwParam)
			{
			case DMN_EXEC_STOP :
				DMSTRESSMSG("XboxNotificationHandler: DM_EXEC(DMN_EXEC_STOP)");
				break;
			case DMN_EXEC_START :
				DMSTRESSMSG("XboxNotificationHandler: DM_EXEC(DMN_EXEC_START)");
				break;
			case DMN_EXEC_REBOOT :
				DMSTRESSMSG("XboxNotificationHandler: DM_EXEC(DMN_EXEC_REBOOT)");
				break;
			case DMN_EXEC_PENDING :
				DMSTRESSMSG("XboxNotificationHandler: DM_EXEC(DMN_EXEC_PENDING)");
				break;
			default :
				 DMSTRESSMSG("XboxNotificationHandler: DM_EXEC(???)");
				break;
			}
			//if (dwParam != DMN_EXEC_REBOOT)
			if (dwParam == DMN_EXEC_PENDING)
				SetEvent(g_hevtReboot);
			break;
		}
	}

	inHere = false;
	return 0;
}


DWORD __stdcall XboxNotificationProcessor(LPCSTR szNotification)
{
	// do nothing
	return 0;
}


bool RebootXbox(bool rebootToTitleLauncher)
{
	ResetEvent(g_hevtReboot);
	
	bool rebootCompleted = false;
	while (true)
	{
		HRESULT hr = DmReboot(DMBOOT_WARM | (rebootToTitleLauncher ? 0 : DMBOOT_WAIT));
		if (SUCCEEDED(hr))
		{
			DMSTRESSMSG("Waiting for g_hevtReboot event");
			while (true)
			{
				DMSTRESSMSGWAIT;
				if (WaitForSingleObject(g_hevtReboot, 0) == WAIT_OBJECT_0)
				{
					return true;
				}
				Sleep(250);
			}
		}
		else if (hr == XBDM_CONNECTIONLOST)
		{
			DMSTRESSMSG("RebootXbox() : DmReboot() returned XBDM_CONNECTIONLOST, calling ReconnectToXbox()");
			ReconnectToXbox();
		}
	}

	return false;
}

void XboxRerun(char *szXBEDir, char *szXBEName)
{
	_ASSERT(_CrtCheckMemory());

	if (RebootXbox(false))
	{	
		while (true)
		{
			DMSTRESSMSG("XboxRerun() : calling DmSetTitle()");
			REPORTIFFAILED(DmSetTitle(szXBEDir, szXBEName, ""));
			if (SUCCEEDED(hr))
			{
				while (true)
				{
					DMSTRESSMSG("XBoxRerun() : DmGo()...");
					REPORTIFFAILED(DmGo());
					if (SUCCEEDED(hr))
						goto Done;
					else if (hr == XBDM_CONNECTIONLOST)
					{
						DMSTRESSMSG("XBoxRerun() : DmGo() returned XBDM_CONNECTIONLOST, calling ReconnectToXbox()");
						Sleep(250);
						ReconnectToXbox();
					}
					else
					{
						DMSTRESSMSG("XBoxRerun() : DmGo() failed but not XBDM_CONNECTIONLOST");
                        Sleep(3000);
					}
				}
			}
			else if (hr == XBDM_CONNECTIONLOST)
			{
				DMSTRESSMSG("XBoxRerun() : DmSetTitle() returned XBDM_CONNECTIONLOST, calling ReconnectToXbox()");
				Sleep(250);
				ReconnectToXbox();
			}
			else
			{
				DMSTRESSMSG("XBoxRerun() : DmSetTitle() failed but not XBDM_CONNECTIONLOST");
                Sleep(3000);
			}
		}
	}

Done:
	return;
}
