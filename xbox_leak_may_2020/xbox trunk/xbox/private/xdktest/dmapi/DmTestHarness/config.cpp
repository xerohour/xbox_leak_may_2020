//
// Debugger API - Config functions
//

#include "tests.h"
#include "logging.h"
#include <stdio.h>

static void testDmSetConfigValue();
static void testDmGetXboxName();
static void testDmSetXboxName();
static void testDmSetXboxNameNoRegister();

void testConfig()
{
	testDmSetConfigValue();
	testDmGetXboxName();
	testDmSetXboxName();
	testDmSetXboxNameNoRegister();
}

void testDmSetConfigValue()
{
#ifndef _XBOX
	HRESULT hr;
	// this is an undocumented function
	// VERIFY_HRESULT(DmSetConfigValue(0, 0, 0, 0), XBDM_NOERR);
#endif
}

void testDmGetXboxName()
{
	HRESULT			hr;
	unsigned char	szStoredName[256];
	char			buffer[260];
	DWORD			dwStoredSize;
	DWORD			Type, dwSize;
	HKEY			key;

#ifndef _XBOX
	// get name via registry

	dwStoredSize = sizeof szStoredName;
	RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\XboxSDK", 0, KEY_QUERY_VALUE, &key);
	RegQueryValueEx(key, "XboxName", NULL, &Type, szStoredName, &dwStoredSize);
	RegCloseKey(key);
	dwStoredSize = strlen(reinterpret_cast<const char *>(szStoredName));
#endif

	// Verify name matches registry value

	memset(buffer, 0, sizeof buffer);
	dwSize = sizeof buffer;
	VERIFY_HRESULT(DmGetXboxName(buffer, &dwSize), XBDM_NOERR);
#ifndef _XBOX
	VERIFY_TRUE(strcmp(buffer, (char *) szStoredName) == 0);
#else
	strcpy(szStoredName, buffer);
	dwStoredSize = dwSize;
#endif

	// Pass NULL for buf

	memset(buffer, 0, sizeof buffer);
	dwSize = sizeof buffer;
	VERIFY_HRESULT(DmGetXboxName(NULL, &dwSize), E_INVALIDARG);

	// Pass NULL for max

	memset(buffer, 0, sizeof buffer);
	dwSize = sizeof buffer;
 	VERIFY_HRESULT(DmGetXboxName(buffer, NULL), E_INVALIDARG);

	// Pass # < actual name for max

	if (dwStoredSize > 0)
	{
		memset(buffer, 0, sizeof buffer);
		dwSize = dwStoredSize - 1;
 		VERIFY_HRESULT(DmGetXboxName(buffer, &dwSize /* too small */), XBDM_BUFFER_TOO_SMALL);
	}

	// Pass # > actual name for max

	if (dwStoredSize > 0)
	{
		memset(buffer, 0, sizeof buffer);
		dwSize = dwStoredSize + 1;
 		VERIFY_HRESULT(DmGetXboxName(buffer, &dwSize /* too big */), XBDM_NOERR);
	}
}

void testDmSetXboxName()
{
	HRESULT		hr;
	char		szStoredName[256];
	char		szBuffer[260];
	DWORD		dwStoredSize;
	DWORD		i, dwIP;

	dwStoredSize = sizeof szStoredName;
	DmGetXboxName(szStoredName, &dwStoredSize);

	// Pass NULL for name

	VERIFY_HRESULT(DmSetXboxName(NULL), XBDM_NOERR);

	// Pass empty string for name

	VERIFY_HRESULT(DmSetXboxName(""), E_INVALIDARG);

	// Pass 1 char string for name

	VERIFY_HRESULT(DmSetXboxName("A"), XBDM_NOERR);

	// Pass numeric string for name (should resolve to IP)

	DmResolveXboxName(&dwIP);
	sprintf(szBuffer, "%d.%d.%d.%d", (dwIP>>24)&0xFF, (dwIP>>16)&0xFF, (dwIP>>8)&0xFF, dwIP&0xFF);
	VERIFY_HRESULT(DmSetXboxName(szBuffer /*IP*/), XBDM_NOERR);

	// Pass < 255 char string for name

	for (i=0; i<255; i++)
		szBuffer[i]=char('0'+((i+1)%10));
	szBuffer[255]='\0';
	hr = DmSetXboxName(szBuffer);
	VERIFY_HRESULT(DmSetXboxName(szBuffer /* < 255 char Name */), XBDM_NOERR);

	// Pass > 255 char string for name
	
	for (i=0; i<256; i++)
		szBuffer[i]=char('0'+((i+1)%10));
	szBuffer[256]='\0';
	VERIFY_HRESULT(DmSetXboxName(szBuffer /* > 255 char Name */), E_INVALIDARG);

	DmSetXboxName(szStoredName);
}

void testDmSetXboxNameNoRegister()
{
#ifndef _XBOX
	HRESULT		hr;
	char		szStoredName[256];
	char		szBuffer[260];
	DWORD		dwStoredSize;
	DWORD		i, dwIP;

	dwStoredSize = sizeof szStoredName;
	DmGetXboxName(szStoredName, &dwStoredSize);

	// Pass NULL for name

	VERIFY_HRESULT(DmSetXboxNameNoRegister(NULL), XBDM_NOERR);

	// Pass empty string for name

	VERIFY_HRESULT(DmSetXboxNameNoRegister(""), E_INVALIDARG);

	// Pass 1 char string for name

	VERIFY_HRESULT(DmSetXboxNameNoRegister("A"), XBDM_NOERR);

	// Pass numeric string for name (should resolve to IP)

	DmResolveXboxName(&dwIP);
	sprintf(szBuffer, "%d.%d.%d.%d", (dwIP>>24)&0xFF, (dwIP>>16)&0xFF, (dwIP>>8)&0xFF, dwIP&0xFF);
	VERIFY_HRESULT(DmSetXboxNameNoRegister(szBuffer /*IP*/), XBDM_NOERR);

	// Pass < 255 char string for name

	for (i=0; i<255; i++)
		szBuffer[i] = char('0'+((i+1)%10));
	szBuffer[255] = '\0';
	VERIFY_HRESULT(DmSetXboxNameNoRegister(szBuffer /* < 255 char Name */), XBDM_NOERR);

	// Pass > 255 char string for name
	
	for (i=0; i<256; i++)
		szBuffer[i] = char('0'+((i+1)%10));
	szBuffer[256] = '\0';
	VERIFY_HRESULT(DmSetXboxNameNoRegister(szBuffer /* > 255 char Name */), E_INVALIDARG);

	DmSetXboxName(szStoredName);
#endif
}
