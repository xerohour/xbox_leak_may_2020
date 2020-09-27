// SurgeEngine.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f SurgeEngineps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "SurgeEngine.h"
#include "md5.h"

#include "SurgeEngine_i.c"
#include "CSurgeEngine.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_SurgeEngine, CSurgeEngine)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
    {
    if (dwReason == DLL_PROCESS_ATTACH)
        {
        _Module.Init(ObjectMap, hInstance, &LIBID_SURGEENGINELib);
        DisableThreadLibraryCalls(hInstance);
        }
    else if (dwReason == DLL_PROCESS_DETACH)
        {
        _Module.Term();
        }
    return TRUE;    // ok
    }

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
    {
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
    }

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
    {
    return _Module.GetClassObject(rclsid, riid, ppv);
    }

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
    {
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
    }

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
    {
    return _Module.UnregisterServer(TRUE);
    }

/////////////////////////////////////////////////////////////////////////////
// main
int __cdecl main(int, char **)
    {
    DllRegisterServer();
    return 0;
    }


/*++

Routine Description:

    GetToken
    returns the location of the first and second tokens and the length of the first token
    
Arguments:

    char *str           - [in] string buffer
    char **tokenStart   - [out] start of the first token
    size_t *tokenLen    - [out] length of the first token
    char **nextToken    - [out] start of the next token

Return Value:

    char * - same as tokenStart

Notes:

    Example:
        GetToken("\t    abc  def ghij", &tokenStart, &tokenLen, &nextToken);

        printf(tokenStart) -> "abc  def ghij"
        printf(nextToken)  -> "def ghij"
        tokenLen == 3

--*/
char* GetToken(char *str, char **tokenStart, size_t *tokenLen, char **nextToken)
    {
    size_t leadingBlank = strspn(str, " \t\n");
    str += leadingBlank;
    size_t len = strcspn(str, " \t\n");
    size_t trailingBlank = strspn(str + len, " \t\n");

    *tokenStart = str;
    *tokenLen = len;
    *nextToken = str+len+trailingBlank;

    return str;
    }

char* MD5Hash(char *inSource, char *outStr)
    {
    long lResult;
    unsigned char achDigest[20];
    
    lResult = MD5((unsigned char*)inSource, strlen(inSource), (unsigned char*)achDigest);
    if(lResult != ERROR_SUCCESS)
        {
        return NULL;
        }

    ToHex(achDigest, 16, outStr);

    return outStr;
    }