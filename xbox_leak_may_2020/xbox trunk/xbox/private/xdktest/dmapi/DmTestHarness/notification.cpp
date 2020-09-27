//
// Debug API - Socket Functions
//

#include "tests.h"
#include "logging.h"

#define INVALID_DM_FLAGS (~(DM_PERSISTENT|DM_DEBUGSESSION|DM_ASYNCSESSION))

void testDmOpenNotificationSession();
void testDmCloseNotificationSession();
void testDmNotify();
void testDmRegisterNotificationProcessor();

void testNotification()
{
	testDmOpenNotificationSession();
	testDmCloseNotificationSession();
	testDmNotify();
	testDmRegisterNotificationProcessor();
}

DWORD __stdcall MyProcessor(LPCSTR szNotification)
{
	return XBDM_NOERR;
}

DWORD __stdcall MyHandler(ULONG dwNotification, DWORD dwParam)
{
	return XBDM_NOERR;
}

void testDmOpenNotificationSession()
{
	HRESULT			hr;
	PDMN_SESSION	pSession = NULL;

#define RESET_SESSION(p) if (p) { DmCloseNotificationSession(p); p = NULL; }

	VERIFY_HRESULT(DmOpenNotificationSession(0, NULL), E_INVALIDARG);
	RESET_SESSION(pSession);
	VERIFY_HRESULT(DmOpenNotificationSession(INVALID_DM_FLAGS, &pSession), E_INVALIDARG);
	RESET_SESSION(pSession);
	VERIFY_HRESULT(DmOpenNotificationSession(0, &pSession), XBDM_NOERR);
	RESET_SESSION(pSession);
	VERIFY_HRESULT(DmOpenNotificationSession(DM_PERSISTENT, &pSession), XBDM_NOERR);
	RESET_SESSION(pSession);

	// TODO: DmOpenNotificationSession - multiple notification sessions
}

void testDmCloseNotificationSession()
{
	HRESULT			hr;
	PDMN_SESSION	pSession;

	if (FAILED(DmOpenNotificationSession(0, &pSession)))
	{
		LogNote("Unable to open Connection to test DmCloseNotificationSession()");
		return;
	}

	VERIFY_HRESULT(DmCloseNotificationSession(NULL), E_INVALIDARG);
	VERIFY_HRESULT(DmCloseNotificationSession(pSession), XBDM_NOERR);

	// TODO: DmCloseNotificationSession - add more cases

	DmCloseNotificationSession(pSession);
}

void testDmNotify()
{
	HRESULT			hr;
	PDMN_SESSION	pSession = NULL;

	VERIFY_HRESULT(DmOpenNotificationSession(0, &pSession), XBDM_NOERR);
	VERIFY_HRESULT(DmRegisterNotificationProcessor(pSession, "DEFAULT", MyProcessor), XBDM_NOERR);
	VERIFY_HRESULT(DmNotify(pSession, DM_MODLOAD, MyHandler), XBDM_NOERR);
	VERIFY_HRESULT(DmNotify(NULL, DM_MODLOAD, MyHandler), E_INVALIDARG);
	VERIFY_HRESULT(DmNotify(pSession, INVALID_DM_FLAGS, MyHandler), E_INVALIDARG);
	VERIFY_HRESULT(DmNotify(pSession, DM_MODLOAD, NULL), E_INVALIDARG);
	VERIFY_HRESULT(DmCloseNotificationSession(pSession), XBDM_NOERR);
}

void testDmRegisterNotificationProcessor()
{
	HRESULT			hr;
	PDM_CONNECTION	pConnection;
	PDMN_SESSION	pSession;

	hr = DmOpenConnection(&pConnection);
	if (hr != XBDM_NOERR)
	{
		LogNote("Unable to open Connection to test DmRegisterNotificationProcessor()");
		return;
	}

	hr = DmOpenNotificationSession(0, &pSession);
	if (hr != XBDM_NOERR)
	{
		LogNote("Unable to open Session to test DmRegisterNotificationProcessor()");
		return;
	}

	VERIFY_HRESULT(DmRegisterNotificationProcessor(pSession, "DEFAULT", MyProcessor), XBDM_NOERR);
	VERIFY_HRESULT(DmRegisterNotificationProcessor(pSession, "", MyProcessor), XBDM_NOERR);
	VERIFY_HRESULT(DmRegisterNotificationProcessor(NULL, "DEFAULT", MyProcessor), E_INVALIDARG);
	VERIFY_HRESULT(DmRegisterNotificationProcessor(pSession, NULL, MyProcessor), E_INVALIDARG);
	VERIFY_HRESULT(DmRegisterNotificationProcessor(pSession, "DEFAULT", NULL), XBDM_NOERR);

	DmCloseNotificationSession(pSession);
	DmCloseConnection(pConnection);
}

