/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    tllp.c

Abstract:

    This implements the local transport layer for OSDebug versions
    2 and 4 on Win32.

Author:

    Jim Schaad (jimsch)
    Kent Forschmiedt (kentf)


--*/
#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>

#include <string.h>
#include <memory.h>



#include "odtypes.h"
#include "od.h"
#include "odp.h"
#include "odassert.h"
#include "emdm.h"

extern "C" LPDBF lpdbf = (LPDBF)0;         // the debugger helper functions

#include "tllp.h"
#include "tlutil.h"
#include <xboxdbg.h>
#include "resource.h"

extern "C" extern HINSTANCE hInstance;

#include "dbgver.h"
extern AVS Avs;

#ifndef CVWS
int _acrtused = 0;
#endif

int fReplyDM = FALSE;
int fReplyEM = FALSE;

HANDLE hevtReboot;
PDMN_SESSION psessTl;

char szLastError[512];

DWORD TlNotify(DWORD dwNotification, DWORD dwParam)
{
	if((dwNotification & DM_NOTIFICATIONMASK) == DM_EXEC)
		if(dwParam == DMN_EXEC_REBOOT)
			ResetEvent(hevtReboot);
		else
			SetEvent(hevtReboot);
	return 0;
}

#if DBG

ULONG
TEvent::Wait(
	ULONG	TimeOut	// = INFINITE
	)
/*++

Routine Description:

	Wait for the event to be signaled.  In the debug version, we do some
	deadlock checking (when waiting forever).  If we wait for longer than
	DEBUG_DEADLOCK_TIMEOUT we will assert.

--*/
{
	ULONG	ret;
	
	if (TimeOut == INFINITE)
	{
		ret = WaitForSingleObject (m_Event, DEBUG_DEADLOCK_TIMEOUT);

		assert (ret == WAIT_OBJECT_0);
	}

	return ret;
}


ULONG
TMutex::Wait(
	ULONG	TimeOut // = INFINITE
	)
/*++

Routine Description:

	See comments under TEvent::Wait ().

--*/
{
	ULONG	ret;

	if (TimeOut == INFINITE)
	{
		ret = WaitForSingleObject (m_hMutex, DEBUG_DEADLOCK_TIMEOUT);

		assert (ret == WAIT_OBJECT_0);
	}

	return ret;
}

#else

ULONG
TEvent::Wait(
	ULONG	TimeOut // = INFINITE
	)
{
	return WaitForSingleObject (m_Event, TimeOut);
}

ULONG
TMutex::Wait(
	ULONG	TimeOut // = INFINITE
	)
{
	return WaitForSingleObject (m_hMutex, TimeOut);
}

#endif

// debug monitor function definitions.

extern "C" {
void FAR PASCAL LOADDS DMInit (DMTLFUNCTYPE, LPVOID);
XOSD FAR PASCAL LOADDS TLFunc ( TLF, HPID, LPARAM, LPARAM);
XOSD FAR PASCAL LOADDS DMTLFunc ( TLF, HPID, LPARAM, LPARAM);
BOOL FAR PASCAL LOADDS DmDllInit (LPDBF);
VOID FAR PASCAL LOADDS DMFunc (DWORD, LPDBB);
}

LOCAL TLCALLBACKTYPE TLCallBack;    // central osdebug callback function

TEvent*	DmReplyEvent = NULL;
TMutex*	DmRequestMutex = NULL;
TEvent*	EmReplyEvent = NULL;
TMutex*	EmRequestMutex = NULL;


// these variables are static to prevent collisions with other TL's

static BOOL fConDM = FALSE;
static BOOL fConEM = FALSE;
static BOOL fConnected = FALSE;

static LPBYTE  	lpbDM;
static LONG 	ibMaxDM;
static DWORD	ibDM;

static LPBYTE 	lpbEM;
static LONG		ibMaxEM;
static DWORD	ibEM;


TLIS Tlis = {
    FALSE,                // fCanSetup
    0xffffffff,           // dwMaxPacket
    0xffffffff,           // dwOptPacket
    TLISINFOSIZE,         // dwInfoSize ?? what is this for ??
    FALSE,                // fRemote
#if defined(_M_IX86)
    mptix86,              // mpt
    mptix86,              // mptRemote
#elif defined(_M_MRX000)
    mptmips,              // mpt
    mptmips,              // mptRemote
#elif defined(_M_ALPHA)
    mptdaxp,              // mpt
    mptdaxp,              // mptRemote
#else
#error( "unknown target machine" );
#endif
    {  "Local Transport Layer (LOCAL:)" } // rgchInfo
};



/**** DBGVersionCheck                                                   ****
 *                                                                         *
 *  PURPOSE:                                                               *
 *                                                                         *
 *      To export our version information to the debugger.                 *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *      NONE.                                                              *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Returns - A pointer to the standard version information.           *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *      Just returns a pointer to a static structure.                      *
 *                                                                         *
 ***************************************************************************/

#ifdef DEBUGVER
DEBUG_VERSION('T','L',"TCP/IP Xbox Transport Layer (Debug)")
#else
RELEASE_VERSION('T','L',"TCP/IP Xbox Transport Layer")
#endif

DBGVERSIONCHECK()



BOOL
DllVersionMatch(
    HANDLE hMod,
    LPSTR pType
    )
{
    DBGVERSIONPROC  pVerProc;
    LPAVS           pavs;

    pVerProc = (DBGVERSIONPROC)GetProcAddress((HINSTANCE)hMod, DBGVERSIONPROCNAME);
    if (!pVerProc) {
        return(FALSE);  // no version entry point
    } else {
        pavs = (*pVerProc)();

        if ((pType[0] != pavs->rgchType[0] || pType[1] != pavs->rgchType[1]) ||
          (Avs.iApiVer != pavs->iApiVer) ||
          (Avs.iApiSubVer != pavs->iApiSubVer)) {
            return(FALSE);
        }
    }

    return(TRUE);
}



XOSD
WINAPI
TLFunc (
    TLF		wCommand,
    HPID	hpid,
    LPARAM	wParam,
    LPARAM	lParam
    )

/*++

Routine Description:

    This function contains the dispatch loop for commands comming into
    the transport layer.  The address of this procedure is exported to
    users of the DLL.

Arguments:

    wCommand    - Supplies the command to be executed.
    hpid        - Supplies the hpid for which the command is to be executed.
    wParam      - Supplies information about the command.
    lParam      - Supplies information about the command.

Return Value:

    XOSD error code.  xosdNone means that no errors occured.  Other error
    codes are defined in osdebug\include\od.h.

--*/

{
    XOSD xosd = xosdNone;

    Unreferenced( hpid );

    switch ( wCommand ) {

    case tlfInit:

        lpdbf = (LPDBF) wParam;
        TLCallBack = (TLCALLBACKTYPE) lParam;
        xosd = xosdNone;
        break;

    case tlfLoadDM:

        if (DmDllInit(lpdbf) == FALSE) {
            xosd = xosdUnknown;
            break;
        }

        DMInit (DMTLFunc, (LPVOID) ((LPLOADDMSTRUCT)lParam)->lpDmParams);

        break;

    case tlfDestroy:
        break;

    case tlfGetProc:
        *((TLFUNCTYPE FAR *) lParam) = TLFunc;
        break;

    case tlfConnect:
		/* Make sure we can connect to the Xbox */
		{
			HRESULT hr;
			char szHostName[256];


			if(GetHostName(mptUnknown, szHostName, sizeof szHostName,
					lpdbf->lpfnGetSet))
				DmSetXboxName(szHostName);
			DmUseSharedConnection(TRUE);
            /* Set our conversation timeout to 60 seconds */
            DmSetConnectionTimeout(0, 60000);
			/* Get a couple of things squared away */
			if(!hevtReboot)
				hevtReboot = CreateEvent(NULL, TRUE, FALSE, NULL);
			if(!psessTl)
                /* Pass in the secret VC flag to avoid leaks */
				hr = DmOpenNotificationSession(DM_PERSISTENT | 0x20000,
                    &psessTl);
			else
				hr = XBDM_NOERR;
			if(SUCCEEDED(hr))
				hr = DmNotify(psessTl, DM_EXEC, TlNotify);
			if(FAILED(hr))
			{
				hr = DmTranslateError(hr, szLastError, sizeof(szLastError));
                if(FAILED(hr) && hr != XBDM_NOERRORSTRING)
                {
                    szLastError[0]=0;
                }
				
				xosd = xosdGeneral;
				break;
			}
			/* If a reboot in progress, wait until it's complete */
			if(WaitForSingleObject(hevtReboot, 120000) == WAIT_TIMEOUT) {
				if(!LoadString(hInstance, IDS_ERR_BOOTTIMEOUT, szLastError,
						sizeof szLastError))
					szLastError[0] = 0;
				xosd = xosdGeneral;
				break;
			}
		}

        fConEM = TRUE;
        fConnected = fConDM;
        break;

    case tlfDisconnect:
		/* You'd think we need to close our notification session here -- or
		 * in DLL_PROCESS_DETACH -- but VC unloads and reloads the tl every
		 * time a new connection is initiated, and we need to keep persistent
		 * notifications alive.  Instead, we just disconnect the tl's
		 * notification functions and keep the session open (which means the
		 * notifier's resources will stay around forever, though this is
		 * apparently not anathema to the VC resource usage scheme) */
        DmNotify(psessTl, DM_NONE, NULL);
        DmUseSharedConnection(FALSE);

        fConDM = fConnected = FALSE;
        break;

	case tlfGetLastError:
		strncpy((char *)lParam, szLastError, wParam);
		break;

    case tlfSetBuffer:

        lpbDM = (LPBYTE) lParam;
        ibMaxDM = wParam;
        break;

    case tlfReply:

        if (!fConnected)
		{
			xosd = xosdLineNotConnected;
        }
		else
		{
            if (wParam <= ibMaxEM)
			{
                _fmemcpy ( lpbEM, (LPBYTE) lParam, wParam );
                ibEM = wParam;
            }
			else
			{
                ibEM = 0;
                xosd = xosdInvalidParameter;
            }

			EmReplyEvent->Set ();
        }
        break;


    case tlfDebugPacket:

        if ( !fConnected )
		{
            xosd = xosdLineNotConnected;
        }
        else
		{
            DMFunc ( wParam, (LPDBB) lParam );
        }
        break;

    case tlfRequest:

        if ( !fConnected )
		{
			xosd = xosdLineNotConnected;
        }
		else
		{
			EmRequestMutex->Wait ();
            ibDM = 0;
			DMFunc (wParam, (LPDBB) lParam);
			DmReplyEvent->Wait ();
			EmRequestMutex->Release ();

		}
        break;

    case tlfGetVersion:     // don't need to do remote version check
        *((AVS*)lParam) = Avs;
        xosd = xosdNotRemote;
        break;


    case tlfGetInfo:

        _fmemcpy((LPTLIS)lParam, &Tlis, sizeof(TLIS));
        break;

    case tlfSetup:
        break;

    default:

        assert ( FALSE );
        break;
    }

    return xosd;
}                               /* TLFunc() */

//
// DMTLFunc is what the debug monitor will call when it has something
// to do.
//

XOSD
WINAPI
DMTLFunc (
    TLF 	wCommand,
    HPID	hpid,
    LPARAM	wParam,
    LPARAM	lParam
    )
{
    XOSD xosd = xosdNone;

    switch ( wCommand ) {

        case tlfInit:

            break;

        case tlfDestroy:

            break;

        case tlfConnect:

            fConDM = TRUE;
            fConnected = fConEM;
            break;

        case tlfDisconnect:

            fConEM = fConnected = FALSE;
            break;

        case tlfSetBuffer:

            lpbEM = (LPBYTE) lParam;
            ibMaxEM = wParam;
            break;

        case tlfReply:
		
            if (!fConnected)
			{
                xosd = xosdLineNotConnected;
             }
			 else
			 {
                if ( wParam <= ibMaxDM )
				{
                    ibDM = wParam;
                    _fmemcpy ( lpbDM, (LPBYTE) lParam, wParam );
                }
				else
				{
                    ibDM = 0;
                }

				DmReplyEvent->Set ();
				
            }
            break;

        case tlfDebugPacket:
		
            if (!fConnected)
			{
                xosd = xosdLineNotConnected;
            }
			else
			{
                TLCallBack ( hpid, wParam, lParam );
            }
            break;

        case tlfRequest:
		
            if (!fConnected)
			{
                xosd = xosdLineNotConnected;
            }
			else
			{

				DmRequestMutex->Wait ();
                TLCallBack (hpid, wParam, lParam );
				EmReplyEvent->Wait ();
				DmRequestMutex->Release ();

                if (ibEM == 0)
				{
                    xosd = xosdInvalidParameter;
                }
            }
            break;

        default:

            assert ( FALSE );
            break;
    }

    return xosd;

}

extern "C"
int
WINAPI
TlDllMain(
    HINSTANCE	hModule,
    DWORD 		dwReason,
    DWORD 		dwReserved
    )
{
	switch (dwReason)
	{
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
                break;

        case DLL_PROCESS_ATTACH:

			DisableThreadLibraryCalls (hModule);

			DmReplyEvent = new TEvent;
			DmReplyEvent->Create ();
			
			DmRequestMutex = new TMutex;
			DmRequestMutex->Create ();
			
			EmReplyEvent = new TEvent ;
			EmReplyEvent->Create ();
			
			EmRequestMutex = new TMutex;
			EmRequestMutex->Create ();
			
            break;

        case DLL_PROCESS_DETACH:

			delete DmReplyEvent;
			delete DmRequestMutex;
			delete EmReplyEvent;
			delete EmRequestMutex;
            break;
    }
		
	return TRUE;
}



