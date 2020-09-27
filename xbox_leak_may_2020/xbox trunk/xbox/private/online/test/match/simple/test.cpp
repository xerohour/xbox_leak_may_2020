/***************************************************************************
 *
 *  Copyright (c) Microsoft Corporation.  All rights reserved
 *
 ****************************************************************************/

#include <xtl.h>
#include <xonline.h>
#include <xdbg.h>

HRESULT CreateSession( VOID );
HRESULT SearchSession( VOID );
HRESULT WorkUntilDone(XONLINETASK_HANDLE hTask);

XONLINETASK_HANDLE   g_hLogon;

void __cdecl main()
{
    HRESULT      hr                       = S_OK;
    DWORD        cUsers                   = 0;
    DWORD        rgServices[1] = { XONLINE_MATCHMAKING_SERVICE };
    XONLINE_USER Users[XONLINE_MAX_STORED_ONLINE_USERS];

    XInitDevices(0,NULL);

    hr = XOnlineStartup(NULL);
    ASSERT(SUCCEEDED(hr));
    
    hr = XOnlineGetUsers( &Users[0], &cUsers );
    ASSERT(SUCCEEDED(hr));
    ASSERT(cUsers == 4);

    hr = XOnlineLogon( Users, rgServices, 1, NULL, &g_hLogon );
    ASSERT(SUCCEEDED(hr));
 
    do
    {
        hr = XOnlineTaskContinue(g_hLogon);
        if ( hr != XONLINETASK_S_RUNNING )
        {
            if (hr == XONLINE_S_LOGON_CONNECTION_ESTABLISHED)
            {
                break;
            }
            else if (hr == XONLINE_E_LOGON_SERVICE_UNAVAILABLE)
            {
                __asm int 3;
            }
            else if (hr == XONLINE_E_LOGON_UPDATE_REQUIRED)
            {
                __asm int 3;
            }
            else
            {
                __asm int 3;
            }
        }
    } while (1);

    __asm int 3;

    hr = CreateSession();
    ASSERT(SUCCEEDED(hr));

    hr = SearchSession();
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineCleanup();
    ASSERT(SUCCEEDED(hr));

    __asm int 3;
}

HRESULT CreateSession( VOID )
{
    HRESULT            hr             = S_OK;
    XONLINETASK_HANDLE hTask          = NULL;
    XNKID              SessionID      = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    XNKEY              KeyExchangeKey;
    XONLINE_ATTRIBUTE  Attribs[20];

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

    hr = XOnlineMatchSessionCreate( 1, 2, 3, 4, 7, Attribs, NULL, &hTask );
    ASSERT(SUCCEEDED(hr));

    hr = WorkUntilDone(hTask);
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineMatchSessionGetInfo(hTask, &SessionID, &KeyExchangeKey);
    ASSERT(SUCCEEDED(hr));

    XOnlineTaskClose(hTask);

    hr = XOnlineMatchSessionUpdate( SessionID, 1, 2, 3, 4, 7, Attribs, NULL, &hTask );
    ASSERT(SUCCEEDED(hr));

    hr = WorkUntilDone(hTask);
    ASSERT(SUCCEEDED(hr));

    XOnlineTaskClose(hTask);

/*
    XOnlineMatchSessionFindFromID( SessionID, NULL, &hTask );
    
    hr = WorkUntilDone(hTask);
    if (FAILED(hr))
        __asm int 3;

    XOnlineTaskClose(hTask);

    hr = XOnlineMatchSessionDelete( SessionID, NULL, &hTask );
    if (FAILED(hr))
        __asm int 3;

    hr = WorkUntilDone(hCreate);
    if (FAILED(hr))
        __asm int 3;

    XOnlineTaskClose(hTask);
*/

    return hr;
}


HRESULT SearchSession( VOID )
{
    HRESULT              hr              = S_OK;
    PXMATCH_SEARCHRESULT *ppSearchResult = NULL;
    DWORD                dwNumResults    = 0;
    XONLINETASK_HANDLE   hTask           = NULL;
    DWORD                i               = 0;
    XONLINE_ATTRIBUTE    Attribs[20];

    Attribs[0].dwAttributeID = X_ATTRIBUTE_DATATYPE_INTEGER;
    Attribs[0].info.integer.qwValue = 1234;

    Attribs[1].dwAttributeID = X_ATTRIBUTE_DATATYPE_STRING;
    Attribs[1].info.string.pwszValue = L"MYSTRING111";

    Attribs[2].dwAttributeID = X_ATTRIBUTE_DATATYPE_BLOB;
    Attribs[2].info.blob.pvValue = (VOID *) "MYBLOB111";
    Attribs[2].info.blob.dwLength = 9;

    Attribs[3].dwAttributeID = X_ATTRIBUTE_DATATYPE_STRING;
    Attribs[3].info.string.pwszValue = L"MYSTRING222";

    hr = XOnlineMatchSearch( 1, 50, 4, Attribs, 5000, NULL, &hTask );
    ASSERT(SUCCEEDED(hr));

    hr = WorkUntilDone(hTask);
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineMatchSearchGetResults(hTask, &ppSearchResult, &dwNumResults);
    ASSERT(SUCCEEDED(hr));

    for (i = 0; i < dwNumResults; i += 1)
    {
        /*
        hr = XOnlineMatchSearchParse( IN PXMATCH_SEARCHRESULT pSearchResult,
        IN DWORD dwNumSessionAttributes,
        IN PXONLINE_ATTRIBUTE_SPEC pSessionAttributeSpec,
        OUT PVOID pQuerySession
        );
        if (FAILED(hr))
            __asm int 3;
        */
    }

    XOnlineTaskClose(hTask);
    
    return hr;
}


HRESULT WorkUntilDone(XONLINETASK_HANDLE hTask)
{
    HRESULT hr          = S_OK;

    while (1)
    {
        hr = XOnlineTaskContinue(hTask);

        if (hr != XONLINETASK_S_RUNNING)
        {
            ASSERT(SUCCEEDED(hr));

            break;
        }

        if (FAILED(XOnlineTaskContinue(g_hLogon)))
        {
            __asm int 3;
        }
    }

    return hr;
}
