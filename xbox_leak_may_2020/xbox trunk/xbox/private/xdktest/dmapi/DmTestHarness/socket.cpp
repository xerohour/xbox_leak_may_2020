//
// Debug API - Socket Functions
//

#include "tests.h"
#include "logging.h"

void testDmOpenConnection();
void testDmCloseConnection();

void testSocketCommands()
{
	LogNote("*** testSecurity not ready for primetime ***");
	return;

	testDmOpenConnection();
	testDmCloseConnection();
}

void testDmOpenConnection()
{
	HRESULT			hr;
	PDM_CONNECTION	pConnection, apConnection[256];

	// Pass NULL

	VERIFY_HRESULT(DmOpenConnection(NULL), E_INVALIDARG);

	// Open with valid pConnection

	if (VERIFY_HRESULT(DmOpenConnection(&pConnection), XBDM_NOERR))
		hr = DmCloseConnection(pConnection);

	// Open till Max Connections

	for (int i=0; i<256; i++)
	{
		if (! VERIFY_HRESULT_ALT(DmOpenConnection(&apConnection[i]), XBDM_NOERR, XBDM_MAXCONNECT))
			break;
	}

	// Clean up open connections
	for (int j=0; j<i; j++)
		DmCloseConnection(apConnection[j]);
}


void testDmCloseConnection()
{
	HRESULT			hr;
	PDM_CONNECTION	pConnection;

	// using Null

	VERIFY_HRESULT(DmCloseConnection(NULL), E_INVALIDARG);

	// using valid pointer

	hr = DmOpenConnection(&pConnection);
	VERIFY_HRESULT(DmCloseConnection(pConnection), XBDM_NOERR);
}


void testDmUseSharedConnection()
{
	HRESULT hr;

	// Call with true after call with false

	hr = DmUseSharedConnection(false);
	VERIFY_HRESULT(DmUseSharedConnection(true), XBDM_NOERR);

	// Call with true after call with true

	hr = DmUseSharedConnection(true);
	VERIFY_HRESULT(DmUseSharedConnection(true), XBDM_NOERR);

	// Call with false after call with true

	hr = DmUseSharedConnection(true);
	VERIFY_HRESULT(DmUseSharedConnection(false), XBDM_NOERR);

	// Call with false after call with false

	hr = DmUseSharedConnection(false);
	VERIFY_HRESULT(DmUseSharedConnection(false), XBDM_NOERR);
}


void testDmResolveXboxName()
{
	HRESULT			hr;
	unsigned char	storedName[256];
	char			buffer[1024];
	DWORD			storedSize = 256;

	DWORD			Type;
	HKEY			key;
	DWORD			IP, oldIP;

	// Get Registry entry for XboxName, just in case
	
	LogNote("Obtaining XboxName stored in registry");

	RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\XboxSDK", 0, KEY_QUERY_VALUE, &key);
	RegQueryValueEx(key, "XboxName", NULL, &Type, storedName, &storedSize);
	RegCloseKey(key);
	
	storedSize = strlen(reinterpret_cast<const char *>(storedName));
	wsprintf(buffer, "Registry XboxName: %s", storedName);
	LogNote(buffer);

	// Pass NULL

	VERIFY_HRESULT(DmResolveXboxName(NULL), E_INVALIDARG);

	// Verify IP address matches XboxName in registry

	hr = DmResolveXboxName(&oldIP);
	wsprintf(buffer, "%d.%d.%d.%d", (oldIP>>24)&0xFF, (oldIP>>16)&0xFF, (oldIP>>8)&0xFF, oldIP&0xFF);
	hr = DmSetXboxName(buffer);
	IP = 0;
	hr = DmResolveXboxName(&IP);
	if (IP != oldIP)
		LogTestResult("DmResolveXboxName(&IP) returns valid IP", false, "value != actual IP");
	else
		LogTestResult("DmResolveXboxName(&IP) returns valid IP", true, "");

	// Set XboxName to bogus value

	IP = 0;
	hr = DmSetXboxName("La Di Da");
	VERIFY_HRESULT(DmResolveXboxName(&IP), XBDM_CANNOTCONNECT);

	// Restore name to original value

	LogNote("Restoring XboxName registry to original value");

	hr = DmSetXboxName(reinterpret_cast<const char *>(storedName));
}


void testDmReceiveBinary()
{
	HRESULT			hr;
	PDM_CONNECTION	pConnection;
	char			buffer[1024];
	DWORD			size;

	// Pass valid pConnection
	// Pass cb == 0
	// Pass cb == 1
	// Pass cb < data being sent
	// Pass cb > data being sent

	hr = DmOpenConnection(&pConnection);
	if (hr != XBDM_NOERR)
	{
		LogNote("Unable to open Connection to test DmReceiveBinary()");
		return;
	}

	size = 0;
	memset(buffer, 0, sizeof buffer);

	// Pass NULL for pConnection

	VERIFY_HRESULT(DmReceiveBinary(NULL, buffer, sizeof buffer, &size), E_INVALIDARG);

	// Pass NULL for pw

	VERIFY_HRESULT(DmReceiveBinary(pConnection, NULL, sizeof buffer, &size), E_INVALIDARG);

#if 0	// Bug 3545
	// Pass NULL for pcbRet

	VERIFY_HRESULT(DmReceiveBinary(pConnection, buffer, sizeof buffer, NULL), E_INVALIDARG);
#endif

	// Pass valid pConnection

	hr = DmSendBinary(pConnection, "SYSTIME\n\r", 9);
	VERIFY_HRESULT(DmReceiveBinary(pConnection, buffer, sizeof buffer, &size), XBDM_NOERR);

	// Pass cb == 0

	hr = DmSendBinary(pConnection, "SYSTIME\n\r", 9);
	VERIFY_HRESULT(DmReceiveBinary(pConnection, buffer, 0, &size), XBDM_NOERR);

	// Pass cb == -1
	// Pass cb == data being sent

	hr = DmCloseConnection(pConnection);
}

void testDmReceiveSocketLine()
{
	HRESULT			hr;
	PDM_CONNECTION	pConnection;
	char			buffer[1024];
	DWORD			size;


	hr = DmOpenConnection(&pConnection);
	if (hr != XBDM_NOERR)
	{
		LogNote("Unable to open Connection to test DmReceiveSocketLine()");
		return;
	}

	size = sizeof buffer;
	memset(buffer, 'X', size);

	// Pass NULL for pConnection

	hr = DmSendBinary(NULL, "SYSTIME\n\r", 9);
	VERIFY_HRESULT(DmReceiveSocketLine(NULL, buffer, &size), E_INVALIDARG);

	// Pass NULL for szResponse

	hr = DmSendBinary(pConnection, "SYSTIME\n\r", 9);
	VERIFY_HRESULT(DmReceiveSocketLine(pConnection, NULL, &size), E_INVALIDARG);

	// Pass NULL for lpdwResponseSize

	hr = DmSendBinary(pConnection, "SYSTIME\n\r", 9);
	VERIFY_HRESULT(DmReceiveSocketLine(pConnection, buffer, NULL), E_INVALIDARG);

	// Pass valid pConnection

	hr = DmSendBinary(pConnection, "SYSTIME\n\r", 9);
	VERIFY_HRESULT(DmReceiveSocketLine(pConnection, buffer, &size), XBDM_NOERR);

	// Pass size == 0

	hr = DmSendBinary(pConnection, "SYSTIME\n\r", 9);
	VERIFY_HRESULT(DmReceiveSocketLine(pConnection, buffer, 0), E_INVALIDARG);

	// Pass size > incoming data
	// Pass size < incoming data
	// Pass size == 1

	hr = DmCloseConnection(pConnection);
}

void testDmReceiveStatusResponse()
{
	HRESULT			hr;
	PDM_CONNECTION	pConnection;
	char			buffer[1024];
	DWORD			size;

	hr = DmOpenConnection(&pConnection);
	if (hr != XBDM_NOERR)
	{
		LogNote("Unable to open Connection to test DmReceiveStatusResponse()");
		return;
	}

	size = sizeof buffer;
	memset(buffer, 'X', size);

	// Pass NULL for pConnection

	hr = DmSendBinary(NULL, "SYSTIME\n\r", 9);
	VERIFY_HRESULT(DmReceiveStatusResponse(NULL, buffer, &size), E_INVALIDARG);

	// Pass NULL for szResponse
/* ******* NEED TO LOOK INTO THIS *******
	TEST_BEGIN("DmReceiveStatusResponse(pConnection, NULL, &size)");
	hr = DmSendBinary(pConnection, "SYSTIME\n\r", 9);
	hr = DmReceiveStatusResponse(pConnection, NULL, &size);
	VERIFY_HRESULT("DmReceiveStatusResponse(pConnection, NULL, &size)", hr);
	TEST_END("DmReceiveStatusResponse(pConnection, NULL, &size)");
*/
	// Pass NULL for lpdwResponseSize

	hr = DmSendBinary(pConnection, "SYSTIME\n\r", 9);
	VERIFY_HRESULT(DmReceiveStatusResponse(pConnection, buffer, NULL), E_INVALIDARG);

	// Pass valid pConnection

	hr = DmSendBinary(pConnection, "SYSTIME\n\r", 9);
	VERIFY_HRESULT(DmReceiveStatusResponse(pConnection, buffer, &size), XBDM_NOERR);

	// Pass size == 0

	hr = DmSendBinary(pConnection, "SYSTIME\n\r", 9);
	VERIFY_HRESULT(DmReceiveStatusResponse(pConnection, buffer, 0), E_INVALIDARG);

	// Pass size > incoming data
	// Pass size < incoming data
	// Pass size == 1

	hr = DmCloseConnection(pConnection);
}

void testDmSendBinary()
{
	HRESULT			hr;
	PDM_CONNECTION	pConnection;
	char			buffer[1024];
	DWORD			size;


	hr = DmOpenConnection(&pConnection);
	if (hr != XBDM_NOERR)
	{
		LogNote("Unable to open Connection to test DmSendBinary()");
		return;
	}

	size = sizeof buffer;
	memset(buffer, 'X', size);

	// Pass NULL for pConnection

	VERIFY_HRESULT(DmSendBinary(NULL, buffer, size), E_INVALIDARG);

	// Pass NULL for pw

	VERIFY_HRESULT(DmSendBinary(pConnection, NULL, size), E_INVALIDARG);

	// Pass valid pConnection

	VERIFY_HRESULT(DmSendBinary(pConnection, buffer, size), XBDM_NOERR);

	// Pass cb == 0

	VERIFY_HRESULT(DmSendBinary(pConnection, buffer, 0), XBDM_NOERR);

	// Pass cb == -1
	// Pass cb == data being sent

	hr = DmCloseConnection(pConnection);
}

void testDmSendCommand()
{
	HRESULT			hr;
	PDM_CONNECTION	pConnection;
	char			buffer[1024];
	DWORD			size;

	hr = DmOpenConnection(&pConnection);
	if (hr != XBDM_NOERR)
	{
		LogNote("Unable to open Connection to test DmSendCommand()");
		return;
	}

	DmUseSharedConnection(false);
	// Pass NULL for pConnection

	size = sizeof buffer;
	VERIFY_HRESULT(DmSendCommand(NULL, "SYSTIME", buffer, &size), E_INVALIDARG);

	// Pass NULL for szCommand

	size = sizeof buffer;
	VERIFY_HRESULT(DmSendCommand(pConnection, NULL, buffer, &size), E_INVALIDARG);

	// Pass NULL for szResponse

	size = sizeof buffer;
	VERIFY_HRESULT(DmSendCommand(pConnection, "SYSTIME", NULL, &size), XBDM_NOERR);

	// Pass NULL for lpdwResponseSize

	size = sizeof buffer; 
	VERIFY_HRESULT(DmSendCommand(pConnection, "SYSTIME", buffer, NULL), E_INVALIDARG);

	// Pass valid pConnection

	size = sizeof buffer;
	VERIFY_HRESULT(DmSendCommand(pConnection, "SYSTIME", buffer, &size), XBDM_NOERR);

	// Pass empty string for szCommand

	size = sizeof buffer;
	VERIFY_HRESULT(DmSendCommand(pConnection, "", buffer, &size), XBDM_INVALIDCMD);

	// Pass 1 char for szCommand
	// Pass Command, but no handler for szCommand
	// Pass long string for szCommand
	// Pass 0 for size
	// Pass 1 for size
	// Pass size of buffer for size

	hr = DmCloseConnection(pConnection);
}
