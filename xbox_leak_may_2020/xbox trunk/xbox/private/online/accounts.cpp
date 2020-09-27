/*
 *  accounts.cpp
 *
 *  XBOX Online Service.  Copyright (c) Microsoft Corporation.
 *  
 *  Author: Steve Lamb (slamb), Ben Zotto (benzotto)
 *  Created: 12/20/01
 *
 */     

#include "xonp.h"
#include "xonlinep.h"
#include "xonver.h"

#define XOACCTS_TIMEOUT 60000
#define XOACCTS_WORKBUFFER_SIZE 512

struct ACCT_XRL
{
    DWORD dwService;                  // Service ID number
    CHAR * szUrl;                     // relative URL of ISAPI front door
    CHAR * szContentType;             // Service content type
    DWORD cbContentType;              // Size of the service content type
};

const ACCT_XRL g_AccountXrls[] =
{
    { XONLINE_USER_ACCOUNT_SERVICE, "/xuacs/verifynickname.ashx",   "Content-Type: xon/5\r\n", 21 },
    { XONLINE_USER_ACCOUNT_SERVICE, "/xuacs/tempcreateacct.ashx",   "Content-Type: xon/5\r\n", 21 },
    { XONLINE_USER_ACCOUNT_SERVICE, "/xuacs/getlockoutinfo.ashx",   "Content-Type: xon/5\r\n", 21 },
    { XONLINE_USER_ACCOUNT_SERVICE, "/xuacs/gettags.ashx",          "Content-Type: xon/5\r\n", 21 },
    { XONLINE_USER_ACCOUNT_SERVICE, "/xuacs/reservename.ashx",      "Content-Type: xon/5\r\n", 21 },
    { XONLINE_USER_ACCOUNT_SERVICE, "/xuacs/createacct.ashx",       "Content-Type: xon/5\r\n", 21 }
};

enum XRLS
{
    verifyNickname,
    tempCreate,
    getLockoutInfo,
    getTags,
    reserveName,
    createAccount
};


HRESULT
CXo::XOnlineVerifyNickname( LPCWSTR lpNickname, 
                            HANDLE hWorkEvent,
                            XONLINETASK_HANDLE *phTask )
{
    return XONLINETASK_S_SUCCESS;
}




// ********************************************************
//
// _XOnlineAccountTempCreate
//
// ********************************************************


enum CREATE_TEMP_ACCT_STATE
{
    xoacctCreateUploadPump,
    xoacctSaveAcct,
    xoacctCreateDone
};


struct XO_CREATE_ACCT_TASK
{
    XONLINETASK_CONTEXT             Context;            // this task context
    XRL_ASYNC_EXTENDED              XrlContext;         // upload context
    
    CREATE_TEMP_ACCT_STATE          State;              // current task state

    XONLINEP_USER                   Request;	        // user to create
	XONLINEP_USER                   User;               // created user on success
    BYTE                            rgbBuffer[XOACCTS_WORKBUFFER_SIZE];

    HRESULT                         hrFinal;            // final hresult
};





//---------------------------------------------------------
// _XOnlineAccountTempCreate
//
//   temporary method to create xonline accounts.  
//   Only the users name/kingdom is used.   if the async
//   task ends successfully, call _XOnlineAccountTempGetResults
//   to retrive the new user struct. 
//
HRESULT
CXo::_XOnlineAccountTempCreate(PXONLINE_USER pUser, HANDLE hWorkEvent, XONLINETASK_HANDLE *phTask)
{
    XO_CREATE_ACCT_TASK* pTaskExt = NULL;

    
    XoEnter("_XOnlineAccountTempCreate");
    XoCheck(pUser != NULL);
    XoCheck(phTask != NULL);

    HRESULT hr = XONLINETASK_S_RUNNING;
  
    if (strlen(pUser->name) > XONLINE_MAX_NAME_LENGTH || strlen(pUser->kingdom) > XONLINE_MAX_KINGDOM_LENGTH)
    {
        hr = XONLINE_E_ACCOUNTS_INVALID_USER;
        goto Cleanup;
    }

    pTaskExt = (XO_CREATE_ACCT_TASK*)SysAllocZ(sizeof(XO_CREATE_ACCT_TASK), PTAG_XO_CREATE_ACCT_TASK);
    if (pTaskExt == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

    TaskInitializeContext(&(pTaskExt->Context));
    pTaskExt->Context.pfnContinue           = DoAcctTempCreateContinue;
    pTaskExt->Context.pfnClose              = DoAcctTempCreateClose;
    pTaskExt->Context.hEventWorkAvailable   = hWorkEvent;
    pTaskExt->State                         = xoacctCreateUploadPump;

    memcpy((PBYTE)&(pTaskExt->Request), pUser, sizeof(XONLINE_USER));
    
    hr = UploadFromMemoryInternal(
                g_AccountXrls[tempCreate].dwService,
                g_AccountXrls[tempCreate].szUrl,
                (PBYTE)&(pTaskExt->User),
                sizeof(XONLINE_USER) + XOACCTS_WORKBUFFER_SIZE,
                (PBYTE)(g_AccountXrls[tempCreate].szContentType),
                g_AccountXrls[tempCreate].cbContentType,
                (PBYTE)&(pTaskExt->Request),
                sizeof(XONLINE_USER),
                XOACCTS_TIMEOUT,
                hWorkEvent,
                &(pTaskExt->XrlContext));

    *phTask = (XONLINETASK_HANDLE)pTaskExt;
    
Cleanup:
    if (FAILED(hr) && pTaskExt != NULL)
    {
        SysFree(pTaskExt);
        *phTask = NULL;
    }
    
    return hr;
}

HRESULT
CXo::_XOnlineAccountTempCreateGetResults(XONLINETASK_HANDLE hTask, XONLINE_USER* pUser)
{
    XoEnter("_XOnlineAccountTempGetResults");
    XoCheck(hTask != NULL);

    XO_CREATE_ACCT_TASK*   pTaskExt   = (XO_CREATE_ACCT_TASK*)hTask;

    if (FAILED(pTaskExt->hrFinal))
    {
        goto Cleanup;
    }
    
    if (pUser != NULL)
    {
        memcpy(pUser, (PBYTE)&(pTaskExt->User), sizeof(XONLINE_USER));
        TimeStampOnlineUserStruct( (XC_ONLINE_USER_ACCOUNT_STRUCT*) pUser ); 
    }
    
Cleanup:
    return pTaskExt->hrFinal;
}


//---------------------------------------------------------
// DoAcctCreateTempContinue
//
//   _XOnlineAccountCreateTemp task pump
//
HRESULT CXo::DoAcctTempCreateContinue( XONLINETASK_HANDLE      hTask )
{
    HRESULT                hr         = XONLINETASK_S_RUNNING;
    XO_CREATE_ACCT_TASK*   pTaskExt   = (XO_CREATE_ACCT_TASK*)hTask;

    Assert(hTask != NULL);

    switch (pTaskExt->State)
    {
    case xoacctCreateUploadPump:
        hr = XOnlineTaskContinue((XONLINETASK_HANDLE)(&(pTaskExt->XrlContext)));
        if (hr != XONLINETASK_S_RUNNING)
        {
            /*
            now done on the server
            
            if (SUCCEEDED(hr))
            {
                // convert password to key.   
                // TODO: move this to the server
                if (!KerbPasswordToKey(pTaskExt->User.realm, (PBYTE)(pTaskExt->User.key)))
                {
                    hr = E_FAIL;
                    pTaskExt->State = xoacctCreateDone;
                    goto Cleanup;
                }

                ZeroMemory(pTaskExt->User.realm, XONLINE_REALM_NAME_SIZE);
                strcpy(pTaskExt->User.realm, "passport.net");
            }
            */

            pTaskExt->hrFinal = hr;
            pTaskExt->State = xoacctCreateDone;
        }

        break;
    
    case xoacctCreateDone:
        break;

    default:
        Assert(FALSE);
    }
    
    return hr;
}



//---------------------------------------------------------
// DoAcctTempCreateClose
//
//   _XOnlineAccountCreateTemp task close handler
//
VOID CXo::DoAcctTempCreateClose(XONLINETASK_HANDLE hTask)
{
    XO_CREATE_ACCT_TASK*   pTaskExt   = (XO_CREATE_ACCT_TASK*)hTask;
    
    XRL_CleanupHandler(&(pTaskExt->XrlContext.xrlasync));
    SysFree(pTaskExt);
}
                                    

// ********************************************************
//
// _XOnlineGetTags
//
// ********************************************************


#pragma pack(push, 1)
struct GET_TAGS_REQUEST
{
    WORD        wHeadSize;
    WORD        wCountryId;
    WORD        wMaxTags;
    WORD        cbName;
    char        szName[1];       // alloc'd to be the actual name size
};
#pragma pack(pop)

#pragma pack(push, 1)
struct GET_TAGS_RESPONSE
{
    WORD        wTagCount;
    BYTE        byTags[1];   // alloc'd to be the max name size
};
#pragma pack(pop)


enum GET_TAGS_STATE
{
    xoacctGetTagsUploadPump,
    xoacctGetTagsDone
};

struct GET_TAGS_TASK
{
	XONLINETASK_CONTEXT         Context;            // this task context
    XRL_ASYNC_EXTENDED          XrlContext;         // upload context
    
    GET_TAGS_STATE              State;              // current task state
    HRESULT                     hrFinal;            // final hresult

    GET_TAGS_REQUEST*           pRequest;	        // request buffer
    GET_TAGS_RESPONSE*          pResponse;          // response buffer

    LPWSTR                      pszTags;            // list of utf8-decoded tags.
};

//---------------------------------------------------------
// _XOnlineGetTags
//
//  Checks to see if the given name is good (non-offensive)
//  and returns any tags it's available in. 
// 
//  if pwsName is null, a list of all public tags is returned.
//  
//
HRESULT
CXo::_XOnlineGetTags(WORD wCountryId, WORD wMaxTags, LPCWSTR pwsName, HANDLE hWorkEvent, XONLINETASK_HANDLE *phTask)
{
    GET_TAGS_TASK* pTaskExt = NULL;
    DWORD cbName;
    DWORD cbRequest;
    DWORD cbResponse;

    XoEnter("_XOnlineGetTags");
    XoCheck(phTask != NULL);

    HRESULT hr = S_OK;
  
    pTaskExt = (GET_TAGS_TASK*)SysAllocZ(sizeof(GET_TAGS_TASK), PTAG_GET_TAGS_TASK);
    if (pTaskExt == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

    //
    // init task stuff
    //
    TaskInitializeContext(&(pTaskExt->Context));
    pTaskExt->Context.pfnContinue           = DoGetTagsContinue;
    pTaskExt->Context.pfnClose              = DoGetTagsClose;
    pTaskExt->Context.hEventWorkAvailable   = hWorkEvent;
    pTaskExt->State                         = xoacctGetTagsUploadPump;

    //
    // init request
    //
    if (pwsName == NULL)
    {
        cbName = 0;
    }
    else
    {
        cbName = WideCharToMultiByte( CP_UTF8, 0, pwsName, lstrlenW(pwsName), NULL, 0, NULL, NULL );
    }
    cbRequest = sizeof(GET_TAGS_REQUEST) + cbName;
    pTaskExt->pRequest = (GET_TAGS_REQUEST*)SysAlloc(cbRequest, PTAG_GET_TAGS_REQUEST);
    if (pTaskExt->pRequest == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }
    
    pTaskExt->pRequest->wHeadSize   = sizeof(GET_TAGS_REQUEST) - sizeof(char);
    pTaskExt->pRequest->wCountryId  = wCountryId;
    pTaskExt->pRequest->wMaxTags    = wMaxTags;
    pTaskExt->pRequest->cbName      = (WORD)cbName;
    pTaskExt->pRequest->szName[0]   = NULL;
    
    if (pwsName != NULL)
    {
        WideCharToMultiByte(CP_UTF8, 0, pwsName, lstrlenW(pwsName), pTaskExt->pRequest->szName, cbName, NULL, NULL);
        Assert(pTaskExt->pRequest->szName[0] != NULL);
    }

    // 
    // init return buff
    // 
    cbResponse = sizeof(GET_TAGS_RESPONSE) - sizeof(BYTE) + (wMaxTags * XONLINE_KINGDOM_SIZE) + XOACCTS_WORKBUFFER_SIZE;
    pTaskExt->pResponse = (GET_TAGS_RESPONSE*)SysAllocZ(cbResponse, PTAG_GET_TAGS_RESPONSE);
    if (pTaskExt->pResponse == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

    
	
	
	// 
    // launch the upload
    //
    hr = UploadFromMemoryInternal(
                g_AccountXrls[getTags].dwService,
                g_AccountXrls[getTags].szUrl,
                (PBYTE)(pTaskExt->pResponse),
                cbResponse,
                (PBYTE)(g_AccountXrls[getTags].szContentType),
                g_AccountXrls[getTags].cbContentType,
                (PBYTE)(pTaskExt->pRequest),
                cbRequest,
                XOACCTS_TIMEOUT,
                hWorkEvent,
                &(pTaskExt->XrlContext));

    *phTask = (XONLINETASK_HANDLE)pTaskExt;
    
Cleanup:
    if (FAILED(hr) && pTaskExt != NULL)
    {
        if (pTaskExt->pRequest != NULL)
        {
            SysFree(pTaskExt->pRequest);
        }

        if (pTaskExt->pResponse != NULL)
        {
            SysFree(pTaskExt->pResponse);
        }

        SysFree(pTaskExt);
        *phTask = NULL;
    }
    
    XoLeave(hr);
    return hr;
    
}

HRESULT 
CXo::DoGetTagsContinue( XONLINETASK_HANDLE hTask )
{
    HRESULT                hr         = XONLINETASK_S_RUNNING;
    GET_TAGS_TASK*   pTaskExt   = (GET_TAGS_TASK*)hTask;

    Assert(hTask != NULL);

    switch (pTaskExt->State)
    {
    case xoacctGetTagsUploadPump:
        hr = XOnlineTaskContinue((XONLINETASK_HANDLE)(&(pTaskExt->XrlContext)));
        if (hr != XONLINETASK_S_RUNNING)
        {
            pTaskExt->hrFinal = hr;
            pTaskExt->State = xoacctGetTagsDone;
            
            goto Cleanup;
        }

        break;

    case xoacctGetTagsDone:
        break;

    default:
        Assert(FALSE);
    }
Cleanup:
    pTaskExt->hrFinal = hr;
    return hr;
}


//---------------------------------------------------------
// _XOnlineGetTagsResults
//
//   returns the list of tags retrieved by _XOnlineGetTags.
//
HRESULT
CXo::_XOnlineGetTagsResults(XONLINETASK_HANDLE hTask, LPWSTR* pszTags, WORD* pwTagCount)
{
    GET_TAGS_TASK* pTaskExt = (GET_TAGS_TASK*)hTask;
    LPWSTR psz = pTaskExt->pszTags;
    PBYTE pby = pTaskExt->pResponse->byTags;
    WORD cb;
    HRESULT hr = S_OK;
    int res;

    *pwTagCount = 0;

    if (psz == NULL)
    {
        psz = (PWCHAR)SysAllocZ(sizeof(WCHAR) * XONLINE_KINGDOM_SIZE * pTaskExt->pResponse->wTagCount, PTAG_GET_TAGS_RESULTS);
        if (psz == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto Cleanup;
        }

        pTaskExt->pszTags = psz;

        for (WORD i = 0; i < pTaskExt->pResponse->wTagCount; i++)
        {
            cb = (WORD)*pby;
            if (cb == 0 || cb > XONLINE_KINGDOM_SIZE * sizeof(WCHAR))
            {
                hr = XONLINE_E_INTERNAL_ERROR;
                goto Cleanup;
            }

            pby += sizeof(WORD);

            res = MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pby, cb, psz, XONLINE_KINGDOM_SIZE );
            if (res == 0)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                goto Cleanup;
            }

            pby += cb;
            psz += XONLINE_KINGDOM_SIZE;
        }
    }

    *pszTags  = pTaskExt->pszTags;
    *pwTagCount = pTaskExt->pResponse->wTagCount;
    
Cleanup:
    if (FAILED(hr))
    {
        *pszTags = NULL;
    }
    return hr;
}



//---------------------------------------------------------
// DoGetTagsClose
//
//   _XOnlineGetTags task close handler
//
VOID CXo::DoGetTagsClose(XONLINETASK_HANDLE hTask)
{
    GET_TAGS_TASK*   pTaskExt   = (GET_TAGS_TASK*)hTask;
    
    if (pTaskExt != NULL)
    {
        
        XRL_CleanupHandler(&(pTaskExt->XrlContext.xrlasync));

        if (pTaskExt->pRequest != NULL)
        {
            SysFree(pTaskExt->pRequest);
        }

        if (pTaskExt->pResponse != NULL)
        {
            SysFree(pTaskExt->pResponse);
        }
        
        if (pTaskExt->pszTags != NULL)
        {
            SysFree(pTaskExt->pszTags);
        }
        
        SysFree(pTaskExt);
    }
}








// ********************************************************
//
// _XOnlineReserveName
//
// ********************************************************


#pragma pack(push, 1)
struct RESERVE_NAME_REQUEST
{
    ULONGLONG   qwMachineId;
    WORD        wCountryId;
    WORD        wMaxNames;

    BYTE        rgbData[1];       
};
#pragma pack(pop)

#pragma pack(push, 1)
struct RESERVE_NAME_RESPONSE
{
    WORD        wNameCount;
    BYTE        rgbData[1];   // alloc'd to be the max name size
};
#pragma pack(pop)


enum RESERVE_NAME_STATE
{
    xoacctReserveNameUploadPump,
    xoacctReserveNameDone
};

struct RESERVE_NAME_TASK
{
	XONLINETASK_CONTEXT         Context;            // this task context
    XRL_ASYNC_EXTENDED          XrlContext;         // upload context
    
    RESERVE_NAME_STATE         State;              // current task state
    HRESULT                     hrFinal;            // final hresult

    DWORD                      cbRequest;
    RESERVE_NAME_REQUEST*      pRequest;	        // request buffer

    DWORD                      cbResponse;
    RESERVE_NAME_RESPONSE*     pResponse;          // response buffer

    PUSER_XNAME                      pNames;           // list of utf8-decoded names.
};

//---------------------------------------------------------
// _XOnlineReserveName
//
//  Checks to see if the given name is good (non-offensive)
//  and returns any tags it's available in. 
// 
//  if pwsName is null, a list of all public tags is returned.
//  
//
HRESULT
CXo::_XOnlineReserveName(LPCWSTR pwsGamerName, LPCWSTR pwsRealm, WORD wMaxNames, WORD wCountryId, HANDLE hWorkEvent, XONLINETASK_HANDLE *phTask)
{
    RESERVE_NAME_TASK* pTaskExt = NULL;
    DWORD cbName;
    PBYTE pb;

    XoEnter("_XOnlineReserveName");
    XoCheck(pwsGamerName != NULL);
    XoCheck(phTask != NULL);

    HRESULT hr = S_OK;
  
    pTaskExt = (RESERVE_NAME_TASK*)SysAllocZ(sizeof(RESERVE_NAME_TASK), PTAG_RESERVE_NAME_TASK);
    if (pTaskExt == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

    //
    // init task stuff
    //
    TaskInitializeContext(&(pTaskExt->Context));
    pTaskExt->Context.pfnContinue           = DoReserveNameContinue;
    pTaskExt->Context.pfnClose              = DoReserveNameClose;
    pTaskExt->Context.hEventWorkAvailable   = hWorkEvent;
    pTaskExt->State                         = xoacctReserveNameUploadPump;

    //
    // init request
    //
    pTaskExt->cbRequest = sizeof(RESERVE_NAME_REQUEST) + XONLINE_NAME_SIZE + XONLINE_KINGDOM_SIZE + (2*sizeof(WORD));
    pTaskExt->pRequest = (RESERVE_NAME_REQUEST*)SysAlloc(pTaskExt->cbRequest, PTAG_RESERVE_NAME_REQUEST);
    if (pTaskExt->pRequest == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

    hr = _XOnlineGetMachineID(&(pTaskExt->pRequest->qwMachineId));
    if (FAILED(hr))
    {
        AssertSz1(FALSE, "_XOnlineGetMachineID Failed! 0x%08x", hr);
        goto Cleanup;
    }
        

    pTaskExt->pRequest->wCountryId  = wCountryId;
    pTaskExt->pRequest->wMaxNames   = wMaxNames;
    
    pb = pTaskExt->pRequest->rgbData;
    pb = WToUtf8((PWORD)pb, pb + sizeof(WORD), pwsGamerName);
    pb = WToUtf8((PWORD)pb, pb + sizeof(WORD), pwsRealm);

    Assert((pb - pTaskExt->pRequest->rgbData) < (int)(pTaskExt->cbRequest));
    
    //
    // init return buff
    // 
    //cbResponse = struct + wMaxNames * xname len/chars + workbuffer
    pTaskExt->cbResponse = sizeof(RESERVE_NAME_RESPONSE) - sizeof(BYTE) + 
                (wMaxNames * (XONLINE_NAME_SIZE + XONLINE_KINGDOM_SIZE + (2 * sizeof(WORD)))) + 
                XOACCTS_WORKBUFFER_SIZE;

    pTaskExt->pResponse = (RESERVE_NAME_RESPONSE*)SysAllocZ(pTaskExt->cbResponse, PTAG_RESERVE_NAME_RESPONSE);
    if (pTaskExt->pResponse == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

	
	// 
    // launch the upload
    //
    hr = UploadFromMemoryInternal(
                g_AccountXrls[reserveName].dwService,
                g_AccountXrls[reserveName].szUrl,
                (PBYTE)(pTaskExt->pResponse),
                pTaskExt->cbResponse,
                (PBYTE)(g_AccountXrls[reserveName].szContentType),
                g_AccountXrls[reserveName].cbContentType,
                (PBYTE)(pTaskExt->pRequest),
                pTaskExt->cbRequest,
                XOACCTS_TIMEOUT,
                hWorkEvent,
                &(pTaskExt->XrlContext));

    *phTask = (XONLINETASK_HANDLE)pTaskExt;
    
Cleanup:
    if (FAILED(hr) && pTaskExt != NULL)
    {
        if (pTaskExt->pRequest != NULL)
        {
            SysFree(pTaskExt->pRequest);
        }

        if (pTaskExt->pResponse != NULL)
        {
            SysFree(pTaskExt->pResponse);
        }

        SysFree(pTaskExt);
        *phTask = NULL;
    }
    
    XoLeave(hr);
    return hr;
    
}

HRESULT 
CXo::DoReserveNameContinue( XONLINETASK_HANDLE hTask )
{
    HRESULT                hr         = XONLINETASK_S_RUNNING;
    RESERVE_NAME_TASK*   pTaskExt   = (RESERVE_NAME_TASK*)hTask;

    Assert(hTask != NULL);

    switch (pTaskExt->State)
    {
    case xoacctReserveNameUploadPump:
        hr = XOnlineTaskContinue((XONLINETASK_HANDLE)(&(pTaskExt->XrlContext)));
        if (hr != XONLINETASK_S_RUNNING)
        {
            pTaskExt->hrFinal = hr;
            pTaskExt->State = xoacctReserveNameDone;
            
            goto Cleanup;
        }

        break;

    case xoacctReserveNameDone:
        break;

    default:
        Assert(FALSE);
    }
Cleanup:
    pTaskExt->hrFinal = hr;
    return hr;
}


//---------------------------------------------------------
// _XOnlineReserveNameResults
//
//   returns the list of tags retrieved by _XOnlineReserveName.
//
HRESULT
CXo::_XOnlineGetReserveNameResults(XONLINETASK_HANDLE hTask, PUSER_XNAME *ppNames, DWORD* pdwNameCount)
{
    XoEnter("_XOnlineGetReserveNameResults");
    XoCheck(pdwNameCount != NULL);
    XoCheck(ppNames != NULL);
    
    RESERVE_NAME_TASK* pTaskExt = (RESERVE_NAME_TASK*)hTask;
    PBYTE pby = pTaskExt->pResponse->rgbData;
    WORD cb;
    HRESULT hr = S_OK;
    int res;
    DWORD cbTotal;

    *pdwNameCount = pTaskExt->pResponse->wNameCount;
    *ppNames = pTaskExt->pNames;


    if (*ppNames == NULL && *pdwNameCount > 0)
    {
        cbTotal = sizeof(USER_XNAME) * (*pdwNameCount);
        *ppNames = (PUSER_XNAME)SysAllocZ(cbTotal, PTAG_RESERVE_NAME_RESULTS);
        if (*ppNames == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto Cleanup;
        }

        for (WORD i = 0; i < pTaskExt->pResponse->wNameCount; i++)
        {
            cb = (WORD)*pby;
            if (cb == 0 || cb > XONLINE_NAME_SIZE)
            {
                hr = XONLINE_E_INTERNAL_ERROR;
                goto Cleanup;
            }
            pby += sizeof(WORD);
            Assert((DWORD)(pby - pTaskExt->pResponse->rgbData) < pTaskExt->cbResponse);

            res = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pby, cb, (*ppNames)[i].wszNickname, XONLINE_NAME_SIZE);
            if (res == 0)
            {
                AssertSz(FALSE, "MultiByteToWideChar(nickname) failed!");
                hr = HRESULT_FROM_WIN32(GetLastError());
            }
            
            pby += cb;
            Assert((DWORD)(pby - pTaskExt->pResponse->rgbData) < pTaskExt->cbResponse);

            cb = (WORD)*pby;
            if (cb == 0 || cb > XONLINE_KINGDOM_SIZE)
            {
                hr = XONLINE_E_INTERNAL_ERROR;
                goto Cleanup;
            }
            pby += sizeof(WORD);
            Assert((DWORD)(pby - pTaskExt->pResponse->rgbData) < pTaskExt->cbResponse);

            res = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pby, cb, (*ppNames)[i].wszRealm, XONLINE_KINGDOM_SIZE);
            if (res == 0)
            {
                AssertSz(FALSE, "MultiByteToWideChar(nickname) failed!");
                hr = HRESULT_FROM_WIN32(GetLastError());
            }

            pby += cb;
            Assert((DWORD)(pby - pTaskExt->pResponse->rgbData) < pTaskExt->cbResponse);
        }
    }

    *pdwNameCount = pTaskExt->pResponse->wNameCount;
    pTaskExt->pNames = *ppNames;

Cleanup:
    if (FAILED(hr))
    {
        if (*ppNames != NULL)
        {
            SysFree(*ppNames);
        }

        *ppNames = NULL;
        *pdwNameCount = 0;
    }
    
    XoLeave(hr);
    return hr;
}



//---------------------------------------------------------
// DoReserveNameClose
//
//   _XOnlineGetTags task close handler
//
VOID CXo::DoReserveNameClose(XONLINETASK_HANDLE hTask)
{
    RESERVE_NAME_TASK*   pTaskExt   = (RESERVE_NAME_TASK*)hTask;
    
    if (pTaskExt != NULL)
    {
        
        XRL_CleanupHandler(&(pTaskExt->XrlContext.xrlasync));

        if (pTaskExt->pRequest != NULL)
        {
            SysFree(pTaskExt->pRequest);
        }

        if (pTaskExt->pResponse != NULL)
        {
            SysFree(pTaskExt->pResponse);
        }
        
        if (pTaskExt->pNames != NULL)
        {
            SysFree(pTaskExt->pNames);
        }
        
        SysFree(pTaskExt);
    }
}



// ********************************************************
//
// _XOnlineCreateAccount
//
// ********************************************************


#pragma pack(push, 1)
struct CREATE_ACCT_REQUEST
{
    BYTE        bCountryId;
    BYTE        bCardTypeId;
    FILETIME    ftCardExpiration;

    // string data, alloc'd to be the actual size
    BYTE byData[1]; 
};
#pragma pack(pop)

#pragma pack(push, 1)

struct CREATE_ACCT_RESPONSE
{
    ULONGLONG   qwUserID;
    BYTE        rgbKey[XONLINE_KEY_LENGTH];
    CHAR        szDomain[XONLINE_USERDOMAIN_SIZE];
    CHAR        szRealm[XONLINE_REALM_NAME_SIZE];
};
#pragma pack(pop)


enum CREATE_ACCT_STATE
{
    xoacctCreateAcctUploadPump,
    xoacctCreateAcctDone
};

struct CREATE_ACCT_TASK
{
	XONLINETASK_CONTEXT         Context;            // this task context
    XRL_ASYNC_EXTENDED          XrlContext;         // upload context
    
    CREATE_ACCT_STATE           State;              // current task state
    HRESULT                     hrFinal;            // final hresult

    CREATE_ACCT_REQUEST*        pRequest;	        // request buffer
    CREATE_ACCT_RESPONSE        Response;           // response buffer
    BYTE                        rgbBuf[XOACCTS_WORKBUFFER_SIZE]; // extra work buffer 

    XONLINEP_USER               User;
};



//---------------------------------------------------------
// _XOnlineCreateAccount
//
//  Checks to see if the given name is good (non-offensive)
//  and returns any tags it's available in. 
// 
//  if pwsName is null, a list of all public tags is returned.
//  
//
HRESULT
CXo::_XOnlineCreateAccount(USER_ACCOUNT_INFO* pAccountInfo, HANDLE hWorkEvent, XONLINETASK_HANDLE *phTask)
{
    CREATE_ACCT_TASK* pTaskExt = NULL;
    DWORD cbRequest;
    DWORD cbResponse;
    DWORD cbUsed;
    CREATE_ACCT_REQUEST* pRequest;
    PBYTE pby;
    WORD n = 0;

    XoEnter("_XOnlineCreateAccount");
    XoCheck(phTask != NULL);

    HRESULT hr = S_OK;
  
    pTaskExt = (CREATE_ACCT_TASK*)SysAllocZ(sizeof(CREATE_ACCT_TASK), PTAG_CREATE_ACCT_TASK);
    if (pTaskExt == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

    //
    // init task stuff
    //
    TaskInitializeContext(&(pTaskExt->Context));
    pTaskExt->Context.pfnContinue           = DoCreateAccountContinue;
    pTaskExt->Context.pfnClose              = DoCreateAccountClose;
    pTaskExt->Context.hEventWorkAvailable   = hWorkEvent;
    pTaskExt->State                         = xoacctCreateAcctUploadPump;

    //
    // init request
    //
    cbRequest = sizeof(USER_ACCOUNT_INFO) * 3;
    pRequest = (CREATE_ACCT_REQUEST*)SysAlloc(cbRequest, PTAG_CREATE_ACCT_REQUEST);
    if (pRequest == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

    
    pRequest->bCountryId = pAccountInfo->bCountryId;
	pRequest->bCardTypeId = pAccountInfo->bCardTypeId;
    pRequest->ftCardExpiration = pAccountInfo->ftCardExpiration;

    
    pby = pRequest->byData;
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszNickname);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszKingdom);
    
    // count pin bytes
    for (n = 0; n < XONLINE_PIN_LENGTH && pAccountInfo->rgbPin[n] != 0; n++);
    *(PWORD)(pby) = n;
	pby += sizeof(WORD);

    // copy pin
	if (n > 0)
	{
		memcpy(pby + sizeof(WORD), pAccountInfo->rgbPin, n);
		pby += n;
	}

    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszFirstName);

    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszFirstName);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszLastName);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszStreet1);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszStreet2);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszStreet3);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszCity);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszState);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszPostalCode);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszPhonePrefix);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszPhoneNumber);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszPhoneExtension);
    
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszCardHolder);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszCardNumber);
    
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszEmail);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszBankCode);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszBranchCode);
    pby = WToUtf8((PWORD)pby, pby + sizeof(WORD), pAccountInfo->wszCheckDigits);

	cbUsed = pby - pRequest->byData;     
    pTaskExt->pRequest = pRequest;
    cbResponse = sizeof(CREATE_ACCT_RESPONSE) + XOACCTS_WORKBUFFER_SIZE;

    // 
    // copy known data to user struct
    //
    WToUtf8(NULL, (LPBYTE)pTaskExt->User.name, pAccountInfo->wszNickname);
    WToUtf8(NULL, (LPBYTE)pTaskExt->User.kingdom, pAccountInfo->wszKingdom);
    strcpy(pTaskExt->User.realm, "passport.net");
    memcpy(pTaskExt->User.pin, pAccountInfo->rgbPin, XONLINE_PIN_LENGTH);
    

	// 
    // launch the upload
    //
    hr = UploadFromMemoryInternal(
                g_AccountXrls[createAccount].dwService,
                g_AccountXrls[createAccount].szUrl,
                (PBYTE)&(pTaskExt->Response),
                cbResponse,
                (PBYTE)(g_AccountXrls[createAccount].szContentType),
                g_AccountXrls[createAccount].cbContentType,
                (PBYTE)(pTaskExt->pRequest),
                cbRequest,
                XOACCTS_TIMEOUT,
                hWorkEvent,
                &(pTaskExt->XrlContext));

    
    
    *phTask = (XONLINETASK_HANDLE)pTaskExt;
    
Cleanup:
    if (FAILED(hr) && pTaskExt != NULL)                     
    {
        if (pTaskExt->pRequest != NULL)
        {
            SysFree(pTaskExt->pRequest);
        }

        SysFree(pTaskExt);
        *phTask = NULL;
    }
    
    XoLeave(hr);
    return hr;
    
}

// 
// WToUtf8
//  Converts the given wide str to a UTF8 encoded str, writing 
//  the length to the (optional) pwLen param, and the 
//  encoded string to the pbData param, returning a pointer
//  to the byte following the encoded string.
//
//  NOTE: pbData is assuemed to have enough buffer space to 
//        to fit the encoded string.
//
LPBYTE
CXo::WToUtf8(PWORD pwLen, LPBYTE pbData, LPCWSTR pwszStr)
{
    Assert(pbData != NULL);
    Assert(pwszStr != NULL);
    WORD wLen;
    
    int cchStr = lstrlenW(pwszStr);
    int ret;


    if (*pwszStr == NULL)
    {
        wLen = 0;
        goto Cleanup;
    }
    
    ret = WideCharToMultiByte( CP_UTF8, 0, pwszStr, cchStr, (LPSTR)pbData, max(cchStr * 3, 0xffff), NULL, NULL );
    if (ret == 0)
    {
        AssertSz1(FALSE, "WideCharToMultiByte failed! (0x%08x)", GetLastError());
        
        wLen = 0;
        goto Cleanup;
    }
    
    if (ret > 0xffff)
    {
        AssertSz1(FALSE, "WideCharToMultiByte returned value out of range! <%d>", ret);
        
        wLen = 0;
        goto Cleanup;
    }

	wLen = (WORD)ret;

Cleanup:
    if (pwLen != NULL)
    {
        *pwLen = wLen;
    }

    return pbData + wLen;
}


HRESULT 
CXo::DoCreateAccountContinue( XONLINETASK_HANDLE hTask )
{
    HRESULT             hr         = XONLINETASK_S_RUNNING;
    CREATE_ACCT_TASK*   pTaskExt   = (CREATE_ACCT_TASK*)hTask;

    Assert(hTask != NULL);

    switch (pTaskExt->State)
    {
    case xoacctCreateAcctUploadPump:
        hr = XOnlineTaskContinue((XONLINETASK_HANDLE)(&(pTaskExt->XrlContext)));
        if (hr != XONLINETASK_S_RUNNING)
        {
            if (SUCCEEDED(hr))
            {
                pTaskExt->User.xuid.qwUserID = pTaskExt->Response.qwUserID;
                memcpy(pTaskExt->User.domain, pTaskExt->Response.szDomain, XONLINE_USERDOMAIN_SIZE);
                memcpy(pTaskExt->User.key, pTaskExt->Response.rgbKey, XONLINE_KEY_LENGTH);

                TimeStampOnlineUserStruct( (XC_ONLINE_USER_ACCOUNT_STRUCT*) &(pTaskExt->User) ); 

                hr = _XOnlineAddUserToHD((PXONLINE_USER)&(pTaskExt->User));
				hr = (FAILED(hr) ? hr : XONLINETASK_S_SUCCESS);
				
                // REVIEW: what if a failure happens here??  Do we try to roll back?
            }

            pTaskExt->hrFinal = hr;
            pTaskExt->State = xoacctCreateAcctDone;

            goto Cleanup;
        }

        break;

    case xoacctCreateAcctDone:


        break;

    default:
        Assert(FALSE);
    }
    Cleanup:
    pTaskExt->hrFinal = hr;
    return hr;
}


//---------------------------------------------------------
// _XOnlineCreateAccountResults
//
//   returns the list of tags retrieved by _XOnlineCreateAccount.
//
HRESULT
CXo::_XOnlineGetCreateAccountResults(XONLINETASK_HANDLE hTask, XONLINE_USER* pUser)
{
    XoEnter("_XOnlineGetCreateAccountResults");
    XoCheck(hTask != NULL);

    CREATE_ACCT_TASK*   pTaskExt   = (CREATE_ACCT_TASK*)hTask;

    if (FAILED(pTaskExt->hrFinal))
    {
        goto Cleanup;
    }
    
    if (pUser != NULL)
    {
        memcpy(pUser, (PBYTE)&(pTaskExt->User), sizeof(XONLINE_USER));
    }
    
Cleanup:
    XoLeave(pTaskExt->hrFinal);
    return pTaskExt->hrFinal;
    
}



//---------------------------------------------------------
// DoCreateAccountClose
//
//   _XOnlineCreateAccount task close handler
//
VOID CXo::DoCreateAccountClose(XONLINETASK_HANDLE hTask)
{
    CREATE_ACCT_TASK*   pTaskExt   = (CREATE_ACCT_TASK*)hTask;
    
    if (pTaskExt != NULL)
    {
        XRL_CleanupHandler(&(pTaskExt->XrlContext.xrlasync));

        if (pTaskExt->pRequest != NULL)
        {
            SysFree(pTaskExt->pRequest);
        }

        SysFree(pTaskExt);
    }
}













#if 0
/* THE LOCKOUT FUNCTIONALITY HAS BEEN CUT FROM THE CURRENT IMPLEMENTATION.
   TO BE RESURRECTED AT AN APPROPRIATE TIME                                 
   
   THIS IS UNTESTED CODE!                                                       */



// LockoutInfo structures

enum ACCT_LOCKOUT_STATE
{
    xoacctLockoutUploadPump,
    xoacctLockoutDone
};

#pragma pack(push, 1)
typedef struct 
{
     DWORD      cbSize;
     ULONGLONG  qwUserId;
} 
XO_LOCKOUT_INFO_REQUEST, *PXO_LOCKOUT_INFO_REQUEST;

typedef struct
{
    DWORD      cbSize;
    WORD       wTotalDays;
    WORD       wDaysRemaining;
    BYTE       bHoursRemaining;
} 
XO_LOCKOUT_INFO, *PXO_LOCKOUT_INFO;
#pragma pack(pop)

typedef struct
{
    XONLINETASK_CONTEXT         Context;            // this task context
    XRL_ASYNC_EXTENDED          XrlContext;         // upload context
    BYTE                        rgbWorkBuffer[XOACCTS_WORKBUFFER_SIZE];     // work buffer
    DWORD                       State;              // current task state

    XO_LOCKOUT_INFO_REQUEST     Request;	        // user to get info on
    HRESULT                     hrFinal;            // final hresult
}
XO_LOCKOUT_INFO_TASK, *PXO_LOCKOUT_INFO_TASK;

// SHOULD BE MOVED TO PUBLIC AREA OF XONLINE.X
typedef struct
{
    DWORD       dwTotalDays;
    DWORD       dwDaysRemaining;
    DWORD       dwHoursRemaining;
} XONLINE_LOCKOUT_INFORMATION;



//---------------------------------------------------------
// XOnlineGetLockoutInfo
//
//   Goes to the server to determine the remaining length of time
//   a user has to spend in lockout mode.  Call the GetResults
//   function to get the results.
//
HRESULT
CXo::XOnlineGetLockoutInfo(DWORD dwUserIndex, HANDLE hWorkEvent, XONLINETASK_HANDLE *phTask)
{
    XoEnter("XOnlineGetLockoutInfo");
    XoCheck(dwUserIndex < 4);

    HRESULT                     hr         = XONLINETASK_S_RUNNING;
    ULONGLONG                   qwUserPUID = 0;
    PXO_LOCKOUT_INFO_TASK       pTaskExt   = NULL;
    PXONLINE_USER               rgUsers    = NULL;
    XUID                        xUser;
    CHAR                        szContentType[25];
    DWORD                       cbContentType;
  
    pTaskExt = (PXO_LOCKOUT_INFO_TASK)SysAllocZ(sizeof(XO_LOCKOUT_INFO_TASK), PTAG_XO_LOCKOUT_INFO_TASK);
    if (pTaskExt == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

    TaskInitializeContext(&(pTaskExt->Context));
    pTaskExt->Context.pfnContinue           = DoLockoutInfoContinue;
    pTaskExt->Context.pfnClose              = DoLockoutInfoClose;
    pTaskExt->Context.hEventWorkAvailable   = hWorkEvent;
    pTaskExt->State                         = xoacctLockoutUploadPump;

   
    // Get UserPUID from controller id
    rgUsers = XOnlineGetLogonUsers();
    if(rgUsers == NULL) {
        hr = XONLINE_E_NO_SESSION;
        goto Cleanup;
    }
    xUser = rgUsers[dwUserIndex].xuid;
    qwUserPUID = xUser.qwUserID;
    if(qwUserPUID == 0) {
        hr = XONLINE_E_USER_NOT_LOGGED_ON;
        goto Cleanup;
    }
    
    // fill in request    
    pTaskExt->Request.cbSize    = sizeof(XO_LOCKOUT_INFO_REQUEST);
    pTaskExt->Request.qwUserId  = qwUserPUID;
    
    hr = UploadFromMemoryInternal(
                g_AccountXrls[acctGetLockoutInfo].dwService,
                g_AccountXrls[acctGetLockoutInfo].szUrl,
                pTaskExt->rgbWorkBuffer,
                XOACCTS_WORKBUFFER_SIZE,
                (PBYTE)(g_AccountXrls[acctGetLockoutInfo].szContentType),
                g_AccountXrls[acctGetLockoutInfo].cbContentType,
                (PBYTE)&(pTaskExt->Request),
                sizeof(XO_LOCKOUT_INFO_REQUEST),
                XOACCTS_TIMEOUT,
                hWorkEvent,
                &(pTaskExt->XrlContext));

    *phTask = (XONLINETASK_HANDLE)pTaskExt;
    
Cleanup:
    if (FAILED(hr) && pTaskExt != NULL)
    {
        SysFree(pTaskExt);
        *phTask = NULL;
    }
    
    return (XOLeave(hr));
}


//---------------------------------------------------------
// XOnlineGetLockoutInfoGetResults
//
//   ...gets the results!
//
HRESULT
CXo::XOnlineGetLockoutInfoGetResults(XONLINETASK_HANDLE hTask, XONLINE_LOCKOUT_INFORMATION * pLockoutInfo)
{
    XoEnter("XOnlineGetLockoutInfoGetResults");
    XoCheck(hTask != NULL);

    PXO_LOCKOUT_INFO_TASK           pTaskExt        = (PXO_LOCKOUT_INFO_TASK)hTask;
    PXO_LOCKOUT_INFO                pResponse       = (PXO_LOCKOUT_INFO)&(pTaskExt->rgbWorkBuffer);
    XONLINE_LOCKOUT_INFORMATION     LockoutTemp;
    
    if (FAILED(pTaskExt->hrFinal))
    {
        goto Cleanup;
    }
    
    if (pLockoutInfo != NULL)
    {
        Assert(pResponse != NULL);

        pLockoutInfo->dwTotalDays         = pResponse->wTotalDays;
        pLockoutInfo->dwDaysRemaining     = pResponse->wDaysRemaining;
        pLockoutInfo->dwHoursRemaining    = pResponse->bHoursRemaining;
    }
    
Cleanup:
    return pTaskExt->hrFinal;
}



//---------------------------------------------------------
// DoLockoutContinue
//
//   XOnlineGetLockoutInfo task pump
//
HRESULT CXo::DoLockoutContinue( XONLINETASK_HANDLE      hTask )
{
    HRESULT                hr         = XONLINETASK_S_RUNNING;
    PXO_LOCKOUT_INFO_TASK  pTaskExt   = (PXO_LOCKOUT_INFO_TASK)hTask;

    Assert(hTask != NULL);

    switch (pTaskExt->State)
    {
        case xoacctLockoutUploadPump:
            hr = XOnlineTaskContinue((XONLINETASK_HANDLE)(&(pTaskExt->XrlContext)));
            if (hr != XONLINETASK_S_RUNNING)
            {
                pTaskExt->State = xoacctLockoutDone;
			    pTaskExt->hrFinal = hr;							
                goto Cleanup;
            }
            else 
            {
                Assert(pTaskExt->XrlContext.xrlasync.dwCurrent == ((PXO_LOCKOUT_INFO)(pTaskExt->rgbWorkBuffer))->cbSize);
            }
            break;

        case xoacctLockoutDone:
            hr = pTaskExt->hrFinal;
            break;

        default:
            Assert(FALSE);
    }
    
Cleanup:
    return hr;
}


//---------------------------------------------------------
// DoLockoutClose
//
//   XOnlineGetLockoutInfo task close handler
//
VOID CXo::DoLockoutClose(XONLINETASK_HANDLE hTask)
{
    PXO_LOCKOUT_INFO_TASK   pTaskExt   = (PXO_LOCKOUT_INFO_TASK)hTask;
    
    XRL_CleanupHandler(&(pTaskExt->XrlContext.xrlasync), 0);
    SysFree(pTaskExt);
}

#endif /* lockout section */
