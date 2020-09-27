//-----------------------------------------------------------------------------
// File: MsgClient.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "xonp.h"
#include "xonver.h"

typedef DWORD   XONMSGCLIENT_MSGID;
typedef USHORT  XONMSGCLIENT_LOCALE;

#define     XONMSGCLIENT_SERVER_XRL        "/MsgServer/MsgServer.ashx"
#define     XONMSGCLIENT_TIMEOUT           60000

#define     XONMSGCLIENT_WORKBUFFER_SIZE   200

#define     DEFAULT_STRING                 "NO TITLE NAME!"
#define     DEFAULT_STRING_SIZE            14

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
// InitializeMsgClientContext - initializes the custom task handle
//
//----------------------------------------------------------------------------
VOID CXo::InitializeMsgClientContext( 
            IN XONLINETASK_MSGCLIENT   hMsgTask,
            IN HANDLE                  hWorkEvent
        )
{
    // Fill in the blanks
    ZeroMemory(hMsgTask, sizeof(XONLINECTXT_MSGCLIENT));
    
    TaskInitializeContext(&(hMsgTask->xonTask));
    (hMsgTask->xonTask).pfnContinue = CXo::DoMsgClientContinue;
    (hMsgTask->xonTask).pfnClose  = CXo::DoMsgClientTaskClose;
    (hMsgTask->xonTask).hEventWorkAvailable = hWorkEvent;

    hMsgTask->hrFinal = XONLINETASK_S_RUNNING;
}

//----------------------------------------------------------------------------
//
// MsgClientInternal -  assumes that the request comes into one big buffer that 
// starts with a XONMSGCLIENT_HEADER structure and then has the records alligned
//
//----------------------------------------------------------------------------
HRESULT CXo::MsgClientInternal(
             IN XONLINETASK_MSGCLIENT              hMsgTask,
             IN PXONMSGCLIENT_REQUEST              pRequest,
             IN PXONMSGCLIENT_RESPONSE             pResponse
             )
{
    HRESULT                     hr = S_OK;
    PXONMSGCLIENT_HEADER        pReqHeader = (PXONMSGCLIENT_HEADER)(pRequest);

    Assert(NULL != hMsgTask);
    Assert(NULL != pRequest);
    Assert(NULL != pResponse);

    Assert(0 != (pRequest->Header).wRecordsCount);
    Assert(NULL != pResponse->pbBuffer);

    DWORD dwRequestSize = sizeof(XONMSGCLIENT_HEADER) + 
                          pReqHeader->wRecordsCount * sizeof(XONMSGCLIENT_REQUEST_RECORD);

    /*
	XONLINE_SERVICE_INFO       msgServiceInfo;
    msgServiceInfo.dwServiceID = XONLINE_MESSAGE_SERVICE;
    msgServiceInfo.serviceIP.s_addr = inet_addr("157.56.13.213");
    msgServiceInfo.wServicePort = 80;
	*/

    //
    // Start async work 
    //
    hMsgTask->hrFinal = XONLINETASK_S_RUNNING;
    return ( UploadFromMemoryInternal( 
                    XONLINE_MESSAGE_SERVICE,                            // service ID
                    XONMSGCLIENT_SERVER_XRL,                            // xrl
                    pResponse->pbBuffer,                                // response buffer
                    pResponse->cbBuffer,                                // size of response buffer
                    NULL,                                               // extra headers buffer
                    0,                                                  // .. and count for it.
                    (PBYTE)(pRequest),                                  // request buffer
                    dwRequestSize,                                      // size of request buffer
                    XONMSGCLIENT_TIMEOUT,                               // timeout (?)
                    (hMsgTask->xonTask).hEventWorkAvailable,            // event handle.
                    &(hMsgTask->xonTaskUpload)                          // task handle
                    ));
}


//----------------------------------------------------------------------------
//
// Do work function for msg client operations
//
//----------------------------------------------------------------------------
HRESULT CXo::DoMsgClientContinue(XONLINETASK_HANDLE hTask)
{
    HRESULT                     hr = S_OK;
    PXRL_ASYNC                  pXrlAsync;
        
    Assert(hTask != NULL);

    XONLINETASK_MSGCLIENT       hMsgTask = (XONLINETASK_MSGCLIENT)hTask;

	if( XONLINETASK_S_RUNNING != hMsgTask->hrFinal )
    {
        // One more loop in dowork ... nothing to be done anymore
        return (hMsgTask->hrFinal);
    }

    // We will pump until the task returns
    hr = XOnlineTaskContinue((XONLINETASK_HANDLE)(&(hMsgTask->xonTaskUpload)));

    if(XONLINETASK_S_RUNNING != hr)
    {

        pXrlAsync = &(hMsgTask->xonTaskUpload.xrlasync);
        
        // Just skip this if the HTTP response is a failure
        if (XONLINEUPLOAD_EXSTATUS_ANYERROR(pXrlAsync->dwExtendedStatus))
        {
            hMsgTask->hrFinal = pXrlAsync->hrXErr;
        }
        else
        {
            // Parse and validate the msg server data here; this 
			// handles the case where there are no results.
			hr = ParseMsgServerResults(pXrlAsync);

			if (FAILED(hr))
			{
				hMsgTask->hrFinal = hr;
			}
			else
			{
				hMsgTask->hrFinal = pXrlAsync->hrFinal;
			}
        }
    }

    return (hMsgTask->hrFinal);
}

//----------------------------------------------------------------------------
//
// Function to parse server results
//
//----------------------------------------------------------------------------
HRESULT CXo::ParseMsgServerResults(
			PXRL_ASYNC	pXrlAsync
			)
{
	HRESULT						    hr = S_OK;
	PBYTE						    pbBuffer;
	DWORD						    cbReceived;
	DWORD						    cbCurrent = 0;
	
	PXONMSGCLIENT_HEADER	        pRespHeader;
	PXONMSGCLIENT_RESPONSE_RECORD	pRespRecord;
	DWORD                           cbCurrentRecordSize = 0;
	ULONGLONG                       cbContentLength = 0;

	Assert(pXrlAsync != NULL);

	pbBuffer = pXrlAsync->pBuffer;
	cbReceived = pXrlAsync->dwCurrent;

	//
    // Get content length
    //
	cbContentLength = pXrlAsync->uliContentLength.QuadPart;

	// If content length is zero, then there are zero results
	if (!cbReceived)
	{
		if (cbContentLength == 0)
			goto Cleanup;
		else
			goto InvalidData;
	}

	// Sanity check
	if ((cbReceived < sizeof(XONMSGCLIENT_HEADER)) ||
		(cbReceived > pXrlAsync->cbBuffer))
		goto InvalidData;

	// Initialize our pointers
	pRespRecord = (PXONMSGCLIENT_RESPONSE_RECORD)pbBuffer;

	// Verify information
	if (cbReceived != cbContentLength)
		goto InvalidData;
	
	//
	// Process records
	//
	while (cbCurrent < cbContentLength)
	{

		cbCurrentRecordSize = pRespRecord->cbBuffer + sizeof(pRespRecord->cbBuffer);

		if (cbContentLength < (cbCurrent + cbCurrentRecordSize))
			goto InvalidData;

		cbCurrent += cbCurrentRecordSize;
		if (cbContentLength < cbCurrent)
			goto InvalidData;

		pbBuffer += cbCurrentRecordSize;
		pRespRecord = (PXONMSGCLIENT_RESPONSE_RECORD)pbBuffer;
	}

Cleanup:
	return(hr);

InvalidData:
	hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
	goto Cleanup;
}

//----------------------------------------------------------------------------
//
// Close function for MsgClient
//
//----------------------------------------------------------------------------
VOID CXo::DoMsgClientTaskClose(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);
    SysFree(hTask);
}


//////////////////////////////////////////////////////////////////////////////
//
// Title Cache Implementation
//
//////////////////////////////////////////////////////////////////////////////

//
// Define the title data task states
//
enum XONTITLECACHE_STATE
{
    XONTITLECACHE_STATE_INITIALIZE = 0,
    XONTITLECACHE_STATE_DONE,
    XONTITLECACHE_STATE_OPEN,
    XONTITLECACHE_STATE_RETRIEVE,
	XONTITLECACHE_STATE_START_SERVER,
    XONTITLECACHE_STATE_SERVER,
    XONTITLECACHE_STATE_UPDATE,
    XONTITLECACHE_STATE_CLOSE
};

//----------------------------------------------------------------------------
//
// TitleCacheInit - API to initialize the title cache and return a task handle
//
//----------------------------------------------------------------------------
HRESULT CXo::TitleCacheInit(
				IN  HANDLE              hWorkEvent,
                OUT PXONLINETASK_HANDLE phTask
                )
{
    HRESULT hr = S_OK;

    Assert(NULL != phTask);
    
    XONLINETASK_TITLECACHE hTitleCacheTask = NULL;
        
    do
    {
		*phTask = NULL;

        // Allocate the task context
        hTitleCacheTask = (XONLINETASK_TITLECACHE)SysAlloc(sizeof(XONLINECTXT_TITLECACHE), PTAG_XONLINECTXT_TITLECACHE);
        if (!hTitleCacheTask)
        {
            hr  = E_OUTOFMEMORY;
            break;
        }

		
        // Initialize the context
        InitializeTitleCacheContext(hTitleCacheTask, hWorkEvent);

		//
        // Allocate work buffers
        //
		DWORD dwRequestSize = sizeof(XONMSGCLIENT_HEADER) + sizeof(XONMSGCLIENT_REQUEST_RECORD);
		hTitleCacheTask->pMsgServerReq = (PXONMSGCLIENT_REQUEST)SysAlloc(dwRequestSize, PTAG_XONMSGCLIENT_REQUEST);

		if( NULL ==  hTitleCacheTask->pMsgServerReq )
        {
			hr  = E_OUTOFMEMORY;
			break;
        }

		hTitleCacheTask->cbMsgServerResp = XONMSGCLIENT_WORKBUFFER_SIZE;
		hTitleCacheTask->pbMsgServerResp = (BYTE *)SysAlloc(XONMSGCLIENT_WORKBUFFER_SIZE, PTAG_XONMSGCLIENT_WORKBUFFER);

		if( NULL ==  hTitleCacheTask->pbMsgServerResp )
        {
			hr  = E_OUTOFMEMORY;
			break;
        }

	    //
        // Open the cache
        //
		hr = CacheOpen(XONLC_TYPE_TITLEDATA,
			                  NULL,
							  hWorkEvent,
							  &(hTitleCacheTask->hCache),
							  &(hTitleCacheTask->hTaskLocalCache));

		if( FAILED( hr ) )
        {
			break;
        }

		hTitleCacheTask->hSubtask = hTitleCacheTask->hTaskLocalCache;
		hTitleCacheTask->state = XONTITLECACHE_STATE_OPEN;

		// Return the opaque handle
        *phTask = (XONLINETASK_HANDLE)hTitleCacheTask;
    }
    while( FALSE );

	if( FAILED( hr ) )
    {
		//
        // Free allocated memory
        //
		if( NULL !=  hTitleCacheTask->pMsgServerReq )
        {
			SysFree(hTitleCacheTask->pMsgServerReq);
			hTitleCacheTask->pMsgServerReq = NULL;

        }

		if( NULL !=  hTitleCacheTask->pbMsgServerResp )
        {
			SysFree(hTitleCacheTask->pbMsgServerResp);
			hTitleCacheTask->pbMsgServerResp = NULL;
        }

		//
        // Deallocate task handle
        //
		SysFree(hTitleCacheTask);
		hTitleCacheTask = NULL;

    }
    
    return( hr );

}

//----------------------------------------------------------------------------
//
// Function to initialize the context
//
//----------------------------------------------------------------------------
VOID CXo::InitializeTitleCacheContext( 
            IN XONLINETASK_TITLECACHE  hTitleCacheTask, 
            IN HANDLE                  hWorkEvent
        )
{
    // Fill in the blanks
    ZeroMemory(hTitleCacheTask, sizeof(XONLINECTXT_TITLECACHE));
    
    TaskInitializeContext(&(hTitleCacheTask->xonTask));
    hTitleCacheTask->xonTask.pfnContinue = CXo::DoTitleCacheContinue;
    hTitleCacheTask->xonTask.pfnClose  = CXo::DoTitleCacheTaskClose;
	hTitleCacheTask->xonTask.hEventWorkAvailable = hWorkEvent;

	hTitleCacheTask->hrFinal = XONLINETASK_S_RUNNING;

	InitializeMsgClientContext(&(hTitleCacheTask->xonTaskMsgClient) , hWorkEvent);
}

//----------------------------------------------------------------------------
//
// TitleCacheRetrieve - API to retrieve the Title Name (either from
//                             local cache or from from the server)
//                           - If none succeeds, returns DEFAULT_STRING.
//
//----------------------------------------------------------------------------
BOOL CXo::TitleCacheRetrieve(
				IN XONLINETASK_HANDLE    hTask,
                IN DWORD                 dwTitleID,
                OUT PXONLINE_TITLEDATA   pTitleData
)
{
    Assert(NULL != hTask);
	Assert(NULL != pTitleData);

	XONLINETASK_TITLECACHE hTitleCacheTask = (XONLINETASK_TITLECACHE)hTask;
	

	//
    // Memorize the input data
    //
	hTitleCacheTask->dwTitleID = dwTitleID;
	hTitleCacheTask->pTitleData = pTitleData;


	if( XONTITLECACHE_STATE_OPEN == hTitleCacheTask->state )
    {
		//
        // Tells the client to pump the task
        //
		return FALSE;
    }

	Assert(NULL != hTitleCacheTask->hCache);
	Assert(NULL != hTitleCacheTask->hTaskLocalCache);

	return (DoTitleCacheRetrieveBegin(hTitleCacheTask));

}

//----------------------------------------------------------------------------
//
// DoTitleCacheRetrieveBegin - work function for TitleCacheRetrieve
//
//----------------------------------------------------------------------------
BOOL CXo::DoTitleCacheRetrieveBegin(
		  IN XONLINETASK_TITLECACHE   hTitleCacheTask
		  )
{
	HRESULT               hr = S_OK;

	//
    // First tries to retrieve data from the local cache
    //
	hr = StartLocalCacheRetrieve(hTitleCacheTask);

	if( SUCCEEDED( hr ) && (XONTITLECACHE_STATE_DONE == hTitleCacheTask->state))
    {
		//
        // Finished retrieving the title name - it was in MRU list
        //
		return (TRUE);

    }

	//
    // The data is not in the cache
    //
	if( FAILED( hr ) || (XONTITLECACHE_STATE_START_SERVER == hTitleCacheTask->state))
    {
		//
        // Try on the server
        //
		hr = StartMsgServerRetrieve(hTitleCacheTask);

		if( FAILED( hr ) )
        {
			//
            // Default string 
            //
			memcpy(hTitleCacheTask->pTitleData , DEFAULT_STRING , DEFAULT_STRING_SIZE );
			return TRUE;
        }
    }
	
	//
    // The client has to pump the task handle
    //
	return FALSE;


}

//----------------------------------------------------------------------------
//
// StartLocalCacheRetrieve - starts retrieving the title name from the local cache
//
//----------------------------------------------------------------------------
HRESULT CXo::StartLocalCacheRetrieve(
		  IN XONLINETASK_TITLECACHE   hTitleCacheTask
		  )
{
	HRESULT               hr = S_OK;

	PXONLC_CONTEXT        pContext = &(hTitleCacheTask->context);

	memset(pContext , 0 , sizeof(XONLC_CONTEXT));

	pContext->pbIndexData = (LPBYTE)(&(hTitleCacheTask->dwTitleID));
	pContext->pbRecordBuffer = (LPBYTE)(hTitleCacheTask->pTitleData);

	memset(hTitleCacheTask->pTitleData , 0 , sizeof(XONLINE_TITLEDATA));
	pContext->dwRecordBufferSize = sizeof(XONLINE_TITLEDATA);

	do
	{
		// 
		// Lookup the key in the local cache
		//
		hr = CacheLookup(hTitleCacheTask->hCache , pContext);

		if( FAILED( hr ) )
        {
			TraceSz2(Warning, "Lookup FAILED on key = %x, hr = %x\n" , hTitleCacheTask->dwTitleID , hr);
			break;
        }

		//
        // If data is in the local cache
        //
		if( pContext->bValidDataInCache )
		{
			if( pContext->bCallRetrieveNext )
			{
				//
                // It has to be retrieved asynchronously from the cache
                //
				hr = CacheRetrieve( hTitleCacheTask->hCache, hTitleCacheTask->hTaskLocalCache, pContext);

				if(FAILED(hr))
				{
					TraceSz2(Warning, "Retrieve FAILED on key = %x, hr = %x\n" , hTitleCacheTask->dwTitleID , hr);
					break;
				}

				hTitleCacheTask->hSubtask = hTitleCacheTask->hTaskLocalCache;
				hTitleCacheTask->state = XONTITLECACHE_STATE_RETRIEVE;
				hTitleCacheTask->hrFinal = XONLINETASK_S_RUNNING;
			}
			else
			{
				//
				// Data already in pTitleData
				//
				hTitleCacheTask->hSubtask = NULL;
				hTitleCacheTask->state = XONTITLECACHE_STATE_DONE;
				hTitleCacheTask->hrFinal = XONLINETASK_S_SUCCESS;
			}
		}
		else
		{
			//
            // Try next step: retrieving from the server
            //
			hTitleCacheTask->state = XONTITLECACHE_STATE_START_SERVER;
		}
			
		
	}
	while(FALSE);

	if( FAILED( hr ) )
    {
		hTitleCacheTask->hSubtask = NULL;
		hTitleCacheTask->state = XONTITLECACHE_STATE_DONE;
		hTitleCacheTask->hrFinal = hr;
    }

	return (hr);

}

//----------------------------------------------------------------------------
//
// StartMsgServerRetrieve - starts retrieving the title name from the server
//
//----------------------------------------------------------------------------
HRESULT CXo::StartMsgServerRetrieve(
		  IN XONLINETASK_TITLECACHE   hTitleCacheTask
		  )
{
	HRESULT               hr = S_OK;

	XONMSGCLIENT_RESPONSE   msgServerResp;

	//
    // If the cache path didn't lead anywhere, start a request to a server
    //
	hTitleCacheTask->pMsgServerReq->Header.wRecordsCount = 1;
	
	//
    // Create input data fro the server
    //
	PXONMSGCLIENT_REQUEST_RECORD pReqRecords = (PXONMSGCLIENT_REQUEST_RECORD)((PBYTE)(hTitleCacheTask->pMsgServerReq) + sizeof(XONMSGCLIENT_HEADER));

	pReqRecords[0].wType = XONMSGCLIENT_TYPE_TITLEID;
	pReqRecords[0].dwID = hTitleCacheTask->dwTitleID;
	pReqRecords[0].wLocale = 0;

	//
    // Create output data for the server
    //
	XONMSGCLIENT_RESPONSE   resp;

	resp.cbBuffer = hTitleCacheTask->cbMsgServerResp;
	resp.pbBuffer = hTitleCacheTask->pbMsgServerResp;

	memset(resp.pbBuffer , 0 , resp.cbBuffer);

	XONLINETASK_MSGCLIENT hSubtask = &(hTitleCacheTask->xonTaskMsgClient);

	//
    // Call the internal API for the message server
    //
	hr = MsgClientInternal(hSubtask, hTitleCacheTask->pMsgServerReq, &resp);
                          
	if(FAILED(hr))
	{
		TraceSz1(Warning, "MsgClientInternal FAILED hr = %x \n" , hr);

		hTitleCacheTask->hSubtask = NULL;
		hTitleCacheTask->state = XONTITLECACHE_STATE_DONE;
		hTitleCacheTask->hrFinal = hr;
	}
	else
	{
		hTitleCacheTask->hSubtask = (XONLINETASK_HANDLE)hSubtask;
		hTitleCacheTask->state = XONTITLECACHE_STATE_SERVER;
		hTitleCacheTask->hrFinal = XONLINETASK_S_RUNNING;
	}

	return (hr);
}

//----------------------------------------------------------------------------
//
// StartLocalCacheUpdate - starts updating the local cache with the data from the server
//
//----------------------------------------------------------------------------
HRESULT CXo::StartLocalCacheUpdate(
		  IN XONLINETASK_TITLECACHE   hTitleCacheTask
		  )
{
	HRESULT               hr = S_OK;


	//
    // Start async update of the local cache
    //
	hr = CacheUpdate(hTitleCacheTask->hCache , 
		                    hTitleCacheTask->hTaskLocalCache,
							(PBYTE)(&(hTitleCacheTask->dwTitleID)),
							(PBYTE)(hTitleCacheTask->pTitleData)
							);

	if( FAILED( hr ) )
    {
		hTitleCacheTask->hSubtask = NULL;
		hTitleCacheTask->state = XONTITLECACHE_STATE_DONE;
		hTitleCacheTask->hrFinal = hr;
    }
	else
	{
		hTitleCacheTask->hSubtask = hTitleCacheTask->hTaskLocalCache;
		hTitleCacheTask->state = XONTITLECACHE_STATE_UPDATE;
		hTitleCacheTask->hrFinal = XONLINETASK_S_RUNNING;
	}

	return (hr);
}

//----------------------------------------------------------------------------
//
// Do work function for title data operations
//
//----------------------------------------------------------------------------
HRESULT CXo::DoTitleCacheContinue(XONLINETASK_HANDLE hTask)
{
    HRESULT hr = S_OK;
            
    Assert(hTask != NULL);

    XONLINETASK_TITLECACHE       hTitleCacheTask = (XONLINETASK_TITLECACHE)hTask;

	if( XONTITLECACHE_STATE_DONE == hTitleCacheTask->state )
    {
        // One more loop in dowork ... nothing to be done anymore
		Assert(NULL == hTitleCacheTask->hSubtask);
        return (hTitleCacheTask->hrFinal);
    }

	Assert(NULL != hTitleCacheTask->hSubtask);

	do
	{
		// We will pump until the task returns
		hTitleCacheTask->hrFinal = XOnlineTaskContinue(hTitleCacheTask->hSubtask);

		if(XONLINETASK_S_RUNNING != hTitleCacheTask->hrFinal)
		{
			if( FAILED( hTitleCacheTask->hrFinal ) )
            {
				break;
            }

			//
            // Reuse task handle
            //
            hTitleCacheTask->hSubtask = NULL;

			if( XONTITLECACHE_STATE_OPEN == hTitleCacheTask->state )
			{
				//
				// Finished opening cache - start retrieve operation
				//
				if( DoTitleCacheRetrieveBegin(hTitleCacheTask) )
                {
					//
                    // Found key in cache
                    //
					hTitleCacheTask->state = XONTITLECACHE_STATE_DONE;
		
                }

				break;
			}

			if( XONTITLECACHE_STATE_RETRIEVE == hTitleCacheTask->state )
            {
				//
                // Key found in the local cache
                //

				hTitleCacheTask->state = XONTITLECACHE_STATE_DONE;

				break;
            }

			if( XONTITLECACHE_STATE_SERVER ==  hTitleCacheTask->state )
            {
				//
                // We got the answer from the server
                //
				DWORD dwRespSize = *(DWORD*)(hTitleCacheTask->pbMsgServerResp);

				memset(hTitleCacheTask->pTitleData , 0 , XONLINE_TITLEDATA_SIZE);

				if( (0 < dwRespSize ) && (dwRespSize < XONLINE_TITLEDATA_SIZE) )
                {
					//
                    // Copy the contents from the work buffer; skip size of buffer
                    //
					memcpy(hTitleCacheTask->pTitleData , hTitleCacheTask->pbMsgServerResp + sizeof(DWORD) , dwRespSize );

                    //
                    // Write data into the local cache
                    //
					StartLocalCacheUpdate(hTitleCacheTask);
                }
				else
				{
					//
                    // Default string 
                    //
					memcpy(hTitleCacheTask->pTitleData , DEFAULT_STRING , DEFAULT_STRING_SIZE );
				}

            }
			else //UPDATE
			{
				hTitleCacheTask->state = XONTITLECACHE_STATE_DONE;
			}
		}
       
    }
	while(FALSE);

	if( FAILED( hr ) )
    {
		hTitleCacheTask->hrFinal = hr;
        hTitleCacheTask->state = XONTITLECACHE_STATE_DONE;
    }

	if(XONTITLECACHE_STATE_DONE == hTitleCacheTask->state)
    {
        hTitleCacheTask->hSubtask = NULL;
    }

    return (hTitleCacheTask->hrFinal);
}

//----------------------------------------------------------------------------
//
// Close function for TitleCache
//
//----------------------------------------------------------------------------
VOID CXo::DoTitleCacheTaskClose(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

	XONLINETASK_TITLECACHE       hTitleCacheTask = (XONLINETASK_TITLECACHE)hTask;

	HRESULT hr = S_OK;

	//
    // Try to close the cache, if failed, pump the task until it's finished
    //
	hr = CacheClose(hTitleCacheTask->hCache);

	if( FAILED( hr ) )
    {
		//
        // Some subtask inlolving the cache is in progress
        //
		hr = XONLINETASK_S_RUNNING;
		while(XONLINETASK_S_RUNNING == hr)
		{
			hr = XOnlineTaskContinue(hTitleCacheTask->hSubtask);
		};

		hr = CacheClose(hTitleCacheTask->hCache);

		Assert(SUCCEEDED(hr));
    }

	//
    // Free allocated memory
    //
	if( NULL !=  hTitleCacheTask->pMsgServerReq )
    {
		SysFree(hTitleCacheTask->pMsgServerReq);
		hTitleCacheTask->pMsgServerReq = NULL;
    }

	if( NULL !=  hTitleCacheTask->pbMsgServerResp )
    {
		SysFree(hTitleCacheTask->pbMsgServerResp);
		hTitleCacheTask->pbMsgServerResp = NULL;
    }

	//
    // Deallocate task handle
    //
	SysFree(hTitleCacheTask);
}
