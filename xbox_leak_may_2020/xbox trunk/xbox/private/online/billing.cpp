/*
 *  billing.cpp
 *
 *  XBOX Online Service.  Copyright (c) Microsoft Corporation.
 *  
 *  Author: Ben Zotto (benzotto), Steve Lamb (slamb)
 *  Created: 9/24/01
 *
 *  Implements the APIs:   XOnlineOfferingPurchase()
 *                         XOnlineOfferingCancel()
 *                         XOnlineOfferingGetDetails()
 *                          XOnlineOfferingGetDetailsGetResults()
 *                          [XOnlineOfferingPriceFormat() is elsewhere]
 *                         XOnlineOfferingVerify()
 */     

#include "xonp.h"
#include "xonlinep.h"
#include "xonver.h"
                                                     
// If the following #define is uncommented, the billing functions will always succeed
// with S_OK, without actually performing a network request.
//#define _XBILLING_DUMMY

#define     XOBILLING_TIMEOUT       60000

#define     XO_PURCHASE_BUFFER_SIZE 1024

// 64k is the most we allow in details blobs
#define     XO_DETAILS_MAX_TITLE_DATA_SIZE        65536

                                            
#define     XO_OFFERING_VERIFY_LICENSE_XRL  "/xbos/VerifyLicense.ashx"
#define     XO_OFFERING_VERIFY_BUFFER_SIZE 1024

#define     XO_OFFERING_PURCHASE_XRL "/xbos/OfferingPurchase.ashx"
#define     XO_OFFERING_CANCEL_XRL   "/xbos/OfferingCancel.ashx"
#define     XO_OFFERING_DETAILS_XRL  "/xbos/OfferingDetails.ashx"
//
// Define enumerated types that describe the states for purchasing.
//
typedef enum 
{
    xopurchWaitResponse = 0,
    xopurchDone
} XON_PURCHASE_STATES;

//
// Define enumerated types that describe the states for cancelling.
//
typedef enum 
{
    xocancelWaitResponse = 0,
    xocancelDone
} XON_CANCEL_STATES;


//
// Define enumerated types that describe the states for a details call.
//
typedef enum 
{
    xodetailsWaitResponse = 0,
    xodetailsParseResponse,
    xodetailsResultsAvail,
    xodetailsDone
} XON_DETAILS_STATES;


//
// Define enumerated types that describe the states for verify.
//
typedef enum 
{
    xoverifyCacheOpen = 0,
    xoverifyCacheOpenPump,
    xoverifyCacheLookup,
    xoverifyCacheLookupPump,
    xoverifyUpload,
    xoverifyUploadPump,
    xoverifyCacheUpdate,
    xoverifyCacheUpdatePump,
    xoverifyCacheClose,
    xoverifyCacheClosePump,
    xoverifyDone
} 
XON_OFFERING_VERIFY_STATES;


/* Wire Structs for communication with ISAPIs */

#pragma pack(push, 1)

typedef struct {
    DWORD        cbSize;
    ULONGLONG    qwUserPUID;
    DWORD        dwOfferingId;
} XO_PURCHASE_REQUEST;

typedef struct {
    DWORD       cbSize;
    ULONGLONG   qwUserPUID;
    DWORD       dwOfferingId;
} XO_CANCEL_REQUEST;

typedef struct {
    DWORD       cbSize;
    ULONGLONG   qwUserPUID;
    DWORD       dwOfferId;
    DWORD       dwDescriptionIndex;
    SHORT       sLanguageId;
} XO_DETAILS_REQUEST;
                            
typedef struct {
    DWORD cbSize;
    DWORD dwGrossWholePart;
    BYTE  bGrossFractionalPart;
    DWORD dwNetWholePart;
    BYTE  bNetFractionalPart;
    BYTE  bCurrencyFormat;
    CHAR  rgchISOCode[3];
    DWORD dwSizeOfBlob;
} XO_DETAILS_HEADER, *PXO_DETAILS_HEADER;

#pragma pack(pop)


//
// Cancel: the opaque task handle that the API consumer will hold.
//
struct XO_CANCEL_TASK
{
    XONLINETASK_CONTEXT         Context;            // Standard Task Pump context info
    XRL_ASYNC_EXTENDED      xrlULContext;       // subsumed context for upload
    
    XO_CANCEL_REQUEST       Request;            // cancel Request info
    HRESULT                 hrFinal;            // Final HRESULT
    DWORD                   dwState;            // Current state of op
    BYTE                    rgbWorkBuffer[XO_PURCHASE_BUFFER_SIZE];     // work buffer
};

struct  XO_CANCEL_TASK;
typedef XO_CANCEL_TASK * PXO_CANCEL_TASK;

//
// Purchase: the opaque task handle that the API consumer will hold.
//
struct XO_PURCHASE_TASK
{
    XONLINETASK_CONTEXT     Context;            // Standard Task Pump context info
    XRL_ASYNC_EXTENDED      xrlULContext;       // subsumed context for upload
    
    XO_PURCHASE_REQUEST     Request;            // Purchase Request info
    HRESULT                 hrFinal;            // Final HRESULT
    DWORD                   dwState;            // Current state of op
    BYTE                    rgbWorkBuffer[XO_PURCHASE_BUFFER_SIZE];     // work buffer
};

struct  XO_PURCHASE_TASK;
typedef XO_PURCHASE_TASK * PXO_PURCHASE_TASK;

//
// offering details task handle
//
struct XO_DETAILS_TASK 
{
    XONLINETASK_CONTEXT         Context;            // this task context
    XRL_ASYNC_EXTENDED          xrlULContext;       //  Upload context
    XO_DETAILS_REQUEST          Request;            // details request
    XONLINE_PRICE               GrossPrice;         // eventually holds a price struct
    XONLINE_PRICE               NetPrice;           // likewise
    HRESULT                     hrFinal;            // Final HRESULT
    DWORD                       dwState;            // Current state
};

struct XO_DETAILS_TASK;
typedef XO_DETAILS_TASK * PXO_DETAILS_TASK;


// licsense usage flags
//   defines what fields of a license should be enforced.
#define XO_LIC_TIMEREFRESH      0x0001          
#define XO_LIC_USECOUNT         0x0002          
#define XO_LIC_TIMEFROM         0x0004          
#define XO_LIC_TIMETO           0x0008
#define XO_LIC_USERID           0x0010


//
// acutal license format, retrieved from XBOS
// and stored in the local file cache.
//
#pragma pack(push, 1)
struct XO_OFFERING_LICENSE
{
   // Versioning
   DWORD cbSize;

   // Content Information
   DWORD dwTitleID;
   DWORD dwOfferingId;

   // Content Usage Conditions
   WORD       wConditionFlags;
   BYTE       byRightID;
   BYTE       byUseCount;
   FILETIME   filetimeRefresh;
   FILETIME   filetimeFrom;
   FILETIME   filetimeTo;
   ULONGLONG  qwPassportUserID;

   // signature to prevent tampering
   XCALCSIG_SIGNATURE Signature;
};
#pragma pack(pop)


// 
// index format to cached licenses
//
#pragma pack(push, 1)
struct XO_LICENSE_CACHE_INDEX
{
    DWORD dwOfferingId;
    ULONGLONG qwPuid;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct XO_VERIFY_LICENSE_REQUEST
{
    DWORD       cbSize;
    DWORD       dwTitleId;
    DWORD       dwOfferId;

    ULONGLONG   qwPuids[5];
};
#pragma pack(pop)

#define LIC_XRL_PADDING 15

struct XO_OFFERING_VERIFY_LICENSE_TASK
{
    XONLINETASK_CONTEXT         Context;            // Standard Task Pump context info
    
    XONLINETASK_HANDLE          hXrlTask;           // handle for xrl task
    XONLINETASK_HANDLE          hCacheTask;         // hanlde for caching task
    XO_LICENSE_CACHE_INDEX      index;              // current index data

    HANDLE                      hCache;             // cache handle
    XONLC_CONTEXT               cacheContext;       // cache context

    XO_VERIFY_LICENSE_REQUEST   Request;            // request info
    DWORD                       cbResults;          // response size

    XO_OFFERING_LICENSE         OfferingLicense;    // license info
    BYTE                        rgbyPadding[LIC_XRL_PADDING]; // padding buffer for xrl calls

    BYTE                        nPuidIndex;         // last puid to be examined        
    
    XON_OFFERING_VERIFY_STATES  State;              // Current state of op
    HRESULT                     hrFinal;            // Final HRESULT
};

/////////////////////////////////////////////////////////////////////////////
// XOnlineOfferingPurchase()
//
// User entry point to purchase an online offering.
// 

HRESULT
CXo::XOnlineOfferingPurchase( DWORD dwUserIndex, 
                              XONLINEOFFERING_ID OfferingId, 
                              HANDLE hWorkEvent,
                              XONLINETASK_HANDLE *phTask )
{
    XoEnter("XOnlineOfferingPurchase");
    XoCheck(dwUserIndex < 4);

    ULONGLONG           qwUserPUID = 0;
    HRESULT             hr         = S_OK;
    PXO_PURCHASE_TASK   pXoPurch   = NULL;
    DWORD               cbBuffer   = XO_PURCHASE_BUFFER_SIZE;   
    PXONLINE_USER       rgUsers    = NULL;
    XUID                xUser;

    PXONLINE_SERVICE_INFO pBillingServiceInfo; 

    //  create the task context
	pXoPurch = (PXO_PURCHASE_TASK)SysAllocZ(sizeof(XO_PURCHASE_TASK), PTAG_XO_PURCHASE_TASK);
    if( !pXoPurch ) {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    // set up first state to go to on pumping...
    pXoPurch->dwState = xopurchWaitResponse;

    TaskInitializeContext( &(pXoPurch->Context) );

    // the work function entry point.
    pXoPurch->Context.pfnContinue = DoPurchaseContinue;
    pXoPurch->Context.pfnClose = DoPurchaseClose;
    pXoPurch->Context.hEventWorkAvailable = hWorkEvent;

#ifndef _XBILLING_DUMMY

    // Get UserPUID from controller id
    rgUsers = XOnlineGetLogonUsers();
    if(rgUsers == NULL) {
        hr = XONLINE_E_NO_SESSION;
        goto Error;
    }
    
    xUser = rgUsers[dwUserIndex].xuid;
    qwUserPUID = xUser.qwUserID;
    if(qwUserPUID == 0) {
        hr = XONLINE_E_USER_NOT_LOGGED_ON;
        goto Error;
    }
    // block guests from purchasing stuff!! :)
    Assert( !XOnlineIsUserGuest(xUser.dwUserFlags) );
    if( XOnlineIsUserGuest(xUser.dwUserFlags) ) {
        hr = XONLINE_E_NO_GUEST_ACCESS;
        goto Error;
    }

#endif


    // fill in payload data for the purchase operation
    pXoPurch->Request.cbSize = sizeof(XO_PURCHASE_REQUEST);
    pXoPurch->Request.qwUserPUID = qwUserPUID;
    pXoPurch->Request.dwOfferingId = OfferingId;

    // launch the upload! 

#ifndef _XBILLING_DUMMY

    hr = UploadFromMemoryInternal( 
            XONLINE_BILLING_OFFERING_SERVICE,        // service for billing
            XO_OFFERING_PURCHASE_XRL,               
            pXoPurch->rgbWorkBuffer,
            cbBuffer,
            NULL,                                   // extra headers buffer
            0,                                      // .. and count for it.
            (PBYTE)&(pXoPurch->Request),
            sizeof(XO_PURCHASE_REQUEST),
            XOBILLING_TIMEOUT,
            hWorkEvent,                             // event handle.
            &(pXoPurch->xrlULContext)
        );

#endif

    if( hr != S_OK ) {
        goto Error;
    }       
            
    // pass back the uber-task-handle.  it's opaque and carries our extra data.
    *phTask = (XONLINETASK_HANDLE)pXoPurch;
    
    // we're OK, exit here
    goto Exit;

Error:
    // Failed setup, make sure we clean up everything
    if (pXoPurch)
        SysFree(pXoPurch);

Exit:   
    return(XoLeave(hr));
}


// DoPurchaseContinue()
//
// This is the entry point for the purchase task pump
//
HRESULT CXo::DoPurchaseContinue( XONLINETASK_HANDLE     hTask )
{

    HRESULT             hr       = S_OK;
    PXO_PURCHASE_TASK   pXoPurch = (PXO_PURCHASE_TASK)hTask;

    Assert(hTask != NULL);

    // The first and only working state, we're waiting for a response here.
    //
    if (pXoPurch->dwState == xopurchWaitResponse) {


#ifdef _XBILLING_DUMMY
    
        pXoPurch->hrFinal = S_OK;
        hr = XONLINETASK_S_SUCCEEDED;
        pXoPurch->dwState = xopurchDone;
        goto Exit;
#endif
        
        // all we do in this is pump on the upload.
        hr = XOnlineTaskContinue( (XONLINETASK_HANDLE)(&(pXoPurch->xrlULContext)));

        // done with the upload? 
        if( hr != XONLINETASK_S_RUNNING ) {
            
            // get final result (will be either S_OK aka XONLINETASK_S_SUCCEEDED, 
            // or from X-Err field on error).
            pXoPurch->hrFinal = hr;

            // we're done
            pXoPurch->dwState = xopurchDone;
        }
    }

    // When the next state is xopurchDone, we are done.
    // 
    if (pXoPurch->dwState == xopurchDone)
    {
        // Indicate that we are done and return the final code
        hr = pXoPurch->hrFinal;
        goto Exit;
    }

Exit:   
    return(hr);
}


// DoPurchaseClose()
//
// Performs an "new school" XOnlineTaskClose on a purchase task.
//
VOID CXo::DoPurchaseClose(XONLINETASK_HANDLE hTask)
{
    PXO_PURCHASE_TASK   pXoPurch = (PXO_PURCHASE_TASK)hTask;

    Assert(hTask != NULL);

    // Do we need to ensure that we've completed?
    Assert(pXoPurch->dwState == xopurchDone);

    // Close the upload task's handle first
    XRL_CleanupHandler(&(pXoPurch->xrlULContext.xrlasync));

    // ...And get rid of our own resources
    SysFree(pXoPurch);
}


/////////////////////////////////////////////////////////////////////////////
// XOnlineOfferingCancel()
//
// User entry point to cancel an online offering.
// 

HRESULT 
CXo::XOnlineOfferingCancel(  DWORD dwUserIndex, 
                             XONLINEOFFERING_ID OfferingId, 
                             HANDLE hWorkEvent,
                             XONLINETASK_HANDLE *phTask )
{
    XoEnter("XOnlineOfferingCancel");
    XoCheck(dwUserIndex < 4);

    ULONGLONG           qwUserPUID = 0;
    HRESULT             hr         = S_OK;
    PXO_CANCEL_TASK     pXoCancel  = NULL;
    PXONLINE_USER       rgUsers    = NULL;
    XUID                xUser;
    DWORD               cbBuffer   = XO_PURCHASE_BUFFER_SIZE;   
    
    PXONLINE_SERVICE_INFO pBillingServiceInfo; 

    //  create the task context
	pXoCancel = (PXO_CANCEL_TASK)SysAllocZ(sizeof(XO_CANCEL_TASK), PTAG_XO_CANCEL_TASK);
    if( !pXoCancel )
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    // set up first state to go to on pumping...
    pXoCancel->dwState = xocancelWaitResponse;

    TaskInitializeContext( &(pXoCancel->Context) );

    // the work function entry point.
    pXoCancel->Context.pfnContinue = DoCancelContinue;
    pXoCancel->Context.pfnClose = DoCancelClose;
    pXoCancel->Context.hEventWorkAvailable = hWorkEvent;


#ifndef _XBILLING_DUMMY

    // Get UserPUID from controller id
    rgUsers = XOnlineGetLogonUsers();
    if(rgUsers == NULL) {
        hr = XONLINE_E_NO_SESSION;
        goto Error;
    }

    xUser = rgUsers[dwUserIndex].xuid;
    qwUserPUID = xUser.qwUserID;
    if(qwUserPUID == 0) {
        hr = XONLINE_E_USER_NOT_LOGGED_ON;
        goto Error;
    }
    // block guests 
    Assert( !XOnlineIsUserGuest(xUser.dwUserFlags) );
    if( XOnlineIsUserGuest(xUser.dwUserFlags) ) {
        hr = XONLINE_E_NO_GUEST_ACCESS;
        goto Error;
    }
#endif


    // fill in payload data for the cancel operation
    pXoCancel->Request.cbSize = sizeof(XO_CANCEL_REQUEST);
    pXoCancel->Request.qwUserPUID = qwUserPUID;
    pXoCancel->Request.dwOfferingId = OfferingId;

    // launch the upload! 

#ifndef _XBILLING_DUMMY

    hr = UploadFromMemoryInternal( 
            XONLINE_BILLING_OFFERING_SERVICE,       // service for billing
            XO_OFFERING_CANCEL_XRL,             
            pXoCancel->rgbWorkBuffer,
            cbBuffer,
            NULL,                                   // extra headers buffer
            0,                                      // .. and count for it.
            (PBYTE)&(pXoCancel->Request),
            sizeof(XO_CANCEL_REQUEST),
            XOBILLING_TIMEOUT,
            hWorkEvent,                             // event handle.
            &(pXoCancel->xrlULContext)
        );

#endif

    if( hr != S_OK ) {
        goto Error;
    }       
            
    // pass back the uber-task-handle.  it's opaque and carries our extra data.
    *phTask = (XONLINETASK_HANDLE)pXoCancel;
    
    // we're OK, exit here
    goto Exit;

Error:
    // Failed setup, make sure we clean up everything
    if (pXoCancel)
        SysFree(pXoCancel);

Exit:   
    return(XoLeave(hr));
}


// DoCancelContinue()
//
// This is the entry point for the offer cancel task pump
//
HRESULT CXo::DoCancelContinue(XONLINETASK_HANDLE hTask)
{

    HRESULT             hr       = S_OK;
    PXO_CANCEL_TASK     pXoCancel = (PXO_CANCEL_TASK)hTask;

    Assert(hTask != NULL);

    // The first and only working state, we're waiting for a response here.
    //
    if (pXoCancel->dwState == xocancelWaitResponse) {


#ifdef _XBILLING_DUMMY
    
        pXoCancel->hrFinal = S_OK;
        hr = XONLINETASK_S_SUCCEEDED;
        pXoCancel->dwState = xocancelDone;
        goto Exit;
#endif
        
        // all we do in this is pump on the upload.
        hr = XOnlineTaskContinue( (XONLINETASK_HANDLE)(&(pXoCancel->xrlULContext)));

        // done with the upload? 
        if( hr != XONLINETASK_S_RUNNING ) {
            
            // get final result (will be either S_OK aka XONLINETASK_S_SUCCEEDED, 
            // or from X-Err field on error).
            pXoCancel->hrFinal = hr;

            // we're done
            pXoCancel->dwState = xocancelDone;
        }
    }

    // When the next state is xocancelDone, we are done.
    // 
    if (pXoCancel->dwState == xocancelDone)
    {
        // Indicate that we are done and return the final code
        hr = pXoCancel->hrFinal;
        goto Exit;
    }

Exit:   
    return(hr);
}


// DoCancelClose()
//
// Performs an "new school" XOnlineTaskClose on a cancel task.
//
VOID CXo::DoCancelClose(XONLINETASK_HANDLE hTask)
{
    PXO_CANCEL_TASK pXoCancel = (PXO_CANCEL_TASK)hTask;

    Assert(hTask != NULL);

    // Do we need to ensure that we've completed?
    Assert(pXoCancel->dwState == xocancelDone);

    // Close the upload task's handle first
    XRL_CleanupHandler(&(pXoCancel->xrlULContext.xrlasync));

    // ...And get rid of our own resources
    SysFree(pXoCancel);
}


/////////////////////////////////////////////////////////////////////////////
// XOnlineOfferingGetDetails()
//
// User entry point to get details for an online offering.
// 
HRESULT
CXo::XOnlineOfferingGetDetails ( DWORD                   dwUserIndex,
                                 XONLINEOFFERING_ID      OfferingId,
                                 WORD                    wLanguage,
                                 DWORD                   dwDescriptionIndex,
                                 PBYTE                   pbBuffer,
                                 DWORD                   cbBuffer,
                                 HANDLE                  hWorkEvent,
                                 XONLINETASK_HANDLE      *phTask )
{
    XoEnter("XOnlineOfferingGetDetails");
    XoCheck(dwUserIndex < 4);
    XoCheck(OfferingId != 0);
    XoCheck(cbBuffer != 0);

    HRESULT                     hr         = S_OK;
    ULONGLONG                   qwUserPUID = 0;
    PXO_DETAILS_TASK            pDetails   = NULL;
    PXONLINE_USER               rgUsers    = NULL;
    XUID                        xUser;
    CHAR                        szContentType[25];
    DWORD                       cbContentType;

    // Get UserPUID from controller id
    rgUsers = XOnlineGetLogonUsers();
    if(rgUsers == NULL) {
        hr = XONLINE_E_NO_SESSION;
        goto Error;
    }
    xUser = rgUsers[dwUserIndex].xuid;
    qwUserPUID = xUser.qwUserID;
    if(qwUserPUID == 0) {
        hr = XONLINE_E_USER_NOT_LOGGED_ON;
        goto Error;
    }
    // block guests 
    Assert( !XOnlineIsUserGuest(xUser.dwUserFlags) );
    if( XOnlineIsUserGuest(xUser.dwUserFlags) )
    {
        hr = XONLINE_E_NO_GUEST_ACCESS;
        goto Error;
    }

	// We are responsible for allocating the context and the work buffer
	// The work buffer are the specified bytes immediately following the
	// context structure
	pDetails = (PXO_DETAILS_TASK)SysAlloc(sizeof(PXO_DETAILS_TASK) + (pbBuffer ? 0 : cbBuffer), PTAG_XO_DETAILS_TASK);
	if (!pDetails)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

    // Initialize the context
    ZeroMemory(pDetails, sizeof(PXO_DETAILS_TASK));

    // Set up the buffer 
    if (!pbBuffer)
        pbBuffer = (PBYTE)(pDetails + 1);

    // the work function entry point.
    TaskInitializeContext(&(pDetails->Context));
    pDetails->Context.pfnContinue           = DoDetailsContinue;
    pDetails->Context.pfnClose              = DoDetailsClose;
    pDetails->Context.hEventWorkAvailable   = hWorkEvent;
    pDetails->dwState = xodetailsWaitResponse;
    
    // Load up the request infomation
    pDetails->Request.cbSize             = sizeof(XO_DETAILS_REQUEST);
    pDetails->Request.qwUserPUID         = qwUserPUID;
    pDetails->Request.dwOfferId          = OfferingId;
    pDetails->Request.dwDescriptionIndex = dwDescriptionIndex;
    if( wLanguage == LANGUAGE_CURRENT ) {
        pDetails->Request.sLanguageId = (short)XGetLanguage();    // use whatever the dash is set to now... 
    } else {
        pDetails->Request.sLanguageId = wLanguage;              // ...or override the dash's default
    }

    // Build the content type header
    cbContentType = sprintf(szContentType, 
                            "Content-type: xon/%x\r\n", XONLINE_BILLING_OFFERING_SERVICE);  

    hr = UploadFromMemoryInternal(
                XONLINE_BILLING_OFFERING_SERVICE,
                XO_OFFERING_DETAILS_XRL,
                pbBuffer,
                cbBuffer,
                (PBYTE)szContentType,
                cbContentType,
                (PBYTE)&(pDetails->Request),
                pDetails->Request.cbSize,
                XOBILLING_TIMEOUT,
                hWorkEvent,
                &(pDetails->xrlULContext));


    if( FAILED(hr) ) {
        goto Error;
    } else {
        
        hr = S_OK;
        // pass back the uber-task-handle.  it's opaque and carries our extra data.
        *phTask = (XONLINETASK_HANDLE)pDetails;
        goto Exit;
    }

Error:
    // since something messed up, we need to clean up the mess
    if( pDetails ) 
        SysFree( pDetails );
    
Exit:
    return hr;
}

// DoDetailsContinue()
//
// This is the entry point for the details task pump
//
HRESULT CXo::DoDetailsContinue( XONLINETASK_HANDLE      hTask )
{
    HRESULT             hr         = S_OK;
    PXO_DETAILS_TASK    pXoDetails = (PXO_DETAILS_TASK)hTask;

    Assert(hTask != NULL);

    switch(pXoDetails->dwState) {
        case xodetailsWaitResponse:

            // The first working state, we're waiting for a response here.
            //

            // all we do here is pump on the upload.
            hr = XOnlineTaskContinue( (XONLINETASK_HANDLE)(&(pXoDetails->xrlULContext)));

            // done with the upload? 
            if( hr != XONLINETASK_S_RUNNING ) {
        
                // we might have a real problem here.  in which case, we can jump right
                // to the done state, because there will be no results to process.
                if( hr != XONLINETASK_S_SUCCESS) {  
                    pXoDetails->hrFinal = hr;
                    pXoDetails->dwState = xodetailsDone;
                } else {
                    
                    // we're successfully done here, now we need to figure out the rest of it!
                    pXoDetails->hrFinal = XONLINETASK_S_SUCCESS;
                    pXoDetails->dwState = xodetailsParseResponse;
                    hr = XONLINETASK_S_RUNNING;         // so that *our* task can keep going.
                }
            }
            break;

        case xodetailsParseResponse:

            XOParsePrices(hTask);
            pXoDetails->dwState = xodetailsResultsAvail;
            hr = XONLINETASK_S_RESULTS_AVAIL;
            goto Exit;

        case xodetailsResultsAvail:
            
            // we STAY HERE until GetResults() is called. 
            hr = XONLINETASK_S_RESULTS_AVAIL;
            goto Exit;

        case xodetailsDone:

            // all done here, apparently.  use the upload HR, whether success or not.
            hr = pXoDetails->hrFinal;
            goto Exit;

        default:
            AssertSz(FALSE, "DoDetailsContinue: bad state");
    }

Exit:   
    return hr;
}


// DoDetailsClose()
//
// Performs an "new school" XOnlineTaskClose on a details task.
// Allows closures when results are waiting.
//
VOID CXo::DoDetailsClose(XONLINETASK_HANDLE hTask)
{
    PXO_DETAILS_TASK    pXoDetails = (PXO_DETAILS_TASK)hTask;

    Assert(hTask != NULL);

    // Do we need to ensure that we've completed?
    Assert(pXoDetails->dwState >= xodetailsResultsAvail);

    // Close the upload task's handle first
    XRL_CleanupHandler(&(pXoDetails->xrlULContext.xrlasync));

    // ...And get rid of our own resources
    SysFree(pXoDetails);
}

// XOParsePrices()
//
//  Helper function to unpack price data from details header into meaningful
//  price structs to help the client.
//
VOID CXo::XOParsePrices(XONLINETASK_HANDLE hTask)
{
    WCHAR                isoCode[3];
    PXO_DETAILS_TASK     pXoDetails = (PXO_DETAILS_TASK)hTask;
    
    Assert(pXoDetails != NULL);

    PXO_DETAILS_HEADER   header     = (PXO_DETAILS_HEADER)(pXoDetails->xrlULContext.xrlasync.pBuffer);


    // we're in trouble if the header size isn't what's expected.
    Assert(header->cbSize >= sizeof(XO_DETAILS_HEADER));
    Assert(header->cbSize == (sizeof(XO_DETAILS_HEADER)+(header->dwSizeOfBlob))); 

    // assign the ISO codes -- inelegant thunking (casting) from unicode to ascii is OK because
    //  the ISO codes are always in [A-Z] interval.
    Assert((header->rgchISOCode[0] >> 8) == 0x0);       // make sure thunking is OK
    Assert((header->rgchISOCode[1] >> 8) == 0x0);
    Assert((header->rgchISOCode[2] >> 8) == 0x0);
    pXoDetails->GrossPrice.rgchISOCurrencyCode[0] = pXoDetails->NetPrice.rgchISOCurrencyCode[0] = (CHAR)header->rgchISOCode[0];
    pXoDetails->GrossPrice.rgchISOCurrencyCode[1] = pXoDetails->NetPrice.rgchISOCurrencyCode[1] = (CHAR)header->rgchISOCode[1];
    pXoDetails->GrossPrice.rgchISOCurrencyCode[2] = pXoDetails->NetPrice.rgchISOCurrencyCode[2] = (CHAR)header->rgchISOCode[2];

    // currency format byte
    pXoDetails->GrossPrice.bCurrencyFormat = pXoDetails->NetPrice.bCurrencyFormat = header->bCurrencyFormat;

    // assign gross price
    pXoDetails->GrossPrice.dwWholePart = header->dwGrossWholePart;
    pXoDetails->GrossPrice.dwFractionalPart = header->bGrossFractionalPart;

    // assign net price
    pXoDetails->NetPrice.dwWholePart = header->dwNetWholePart;
    pXoDetails->NetPrice.dwFractionalPart = header->bNetFractionalPart;

}

/////////////////////////////////////////////////////////////////////////////
// XOnlineOfferingGetDetailsGetResults()
//
// User entry point to get results from the details.  All results are optional;
// pass in NULL and you get nothing, pass in a valid pointer and you'll get
// the results you want. 
// 
HRESULT
CXo::XOnlineOfferingDetailsGetResults    ( XONLINETASK_HANDLE      hTask,
                                           PBYTE                   *pDetailsBuffer,
                                           DWORD                   *pcbDetailsLength,
                                           XONLINE_PRICE           *pGrossPrice,
                                           XONLINE_PRICE           *pNetPrice )
{
    XoEnter("XOnlineOfferingDetailsGetResults");
    XoCheck(hTask != NULL);

    PXO_DETAILS_TASK    pXoDetails = (PXO_DETAILS_TASK)hTask;
    PXO_DETAILS_HEADER  pHeader;

    Assert(pXoDetails->dwState == xodetailsResultsAvail);
           
    // since the current state seems to check out OK, load up all the desired args.
    pHeader = (PXO_DETAILS_HEADER)(pXoDetails->xrlULContext.xrlasync.pBuffer);

    if( pcbDetailsLength ) {
        *pcbDetailsLength = pHeader->dwSizeOfBlob;
    }

    if( pDetailsBuffer ) {
        if(pHeader->dwSizeOfBlob) { 
            *pDetailsBuffer = (BYTE *)(pHeader + 1);
        } else {
            *pDetailsBuffer = NULL;
        }
    }

    if( pGrossPrice ) {
        *pGrossPrice = pXoDetails->GrossPrice;
    }

    if( pNetPrice ) {
        *pNetPrice = pXoDetails->NetPrice;
    }

    // since the results have done been gotten, we can allow the consumer to 
    // see the task as complete.
    pXoDetails->dwState = xodetailsDone;

    return S_OK;
}

//---------------------------------------------------------
//
// XOnlineOfferingGetDetailsMaxSize()
//
//      Returns the size of the buffer to pass into a details
//          request. Allows the user to specify a max cap for
//          data that this title uses (thus reducing the buffer
//          size needed).  Otherwise, pass in 0 and get the max
//          possible.  Synchronous.
// 

DWORD CXo::XOnlineOfferingDetailsMaxSize(DWORD cbTitleSpecificDataMaxSize)
{
    XoEnter_("XOnlineOfferingDetailsMaxSize", 0);

    if (!cbTitleSpecificDataMaxSize)
        cbTitleSpecificDataMaxSize = XO_DETAILS_MAX_TITLE_DATA_SIZE ;

    cbTitleSpecificDataMaxSize += sizeof(XO_DETAILS_HEADER);
    return(cbTitleSpecificDataMaxSize);
}



//---------------------------------------------------------
//
// XOnlineOfferingVerify()
//
//      User entry point to verify rights to an offering. 
// 

HRESULT
CXo::XOnlineOfferingVerifyLicense( DWORD dwOfferingId, 
                                   DWORD dwUserAccounts,
                                   HANDLE hWorkEvent,
                                   XONLINETASK_HANDLE *phTask )
{
    XoEnter("XOnlineOfferingVerify");
    XoCheck(phTask != NULL);
    HRESULT hr                                      = XONLINETASK_S_RUNNING;
    XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask    = NULL;
    PXONLINE_USER pUsers                            = NULL;

    //
    //  create the task context
    //

    pVerifyTask = (XO_OFFERING_VERIFY_LICENSE_TASK*)SysAllocZ(sizeof(XO_OFFERING_VERIFY_LICENSE_TASK),
        PTAG_XO_OFFERING_VERIFY_LICENSE_TASK);
    if( !pVerifyTask ) 
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

    TaskInitializeContext( &(pVerifyTask->Context) );
    pVerifyTask->Context.pfnContinue            = VerifyLicenseTaskContinue;
    pVerifyTask->Context.pfnClose               = VerifyLicenseTaskClose;
    pVerifyTask->Context.hEventWorkAvailable    = hWorkEvent;
    

    //
    // set up license specific stuff
    //
    pVerifyTask->Request.cbSize     = sizeof(XO_VERIFY_LICENSE_REQUEST);
    pVerifyTask->Request.dwOfferId  = dwOfferingId;
    pVerifyTask->Request.dwTitleId  = m_dwTitleId;
    pVerifyTask->cbResults          = sizeof(XO_OFFERING_LICENSE) + LIC_XRL_PADDING;
        
    // 
    // populate list of users.  guests don't count.
    //
    // TODO: set machine account. waiting for an api from tony.
    //
    pVerifyTask->Request.qwPuids[0] = 0xffffffffffffffff;
    
    pUsers = XOnlineGetLogonUsers();

    if (dwUserAccounts & XONLINE_LOGON_CONTROLLER1 && !XOnlineIsUserGuest(pUsers[0].xuid.dwUserFlags))
    {
        pVerifyTask->Request.qwPuids[1] = pUsers[0].xuid.qwUserID;
    }
    if (dwUserAccounts & XONLINE_LOGON_CONTROLLER2 && !XOnlineIsUserGuest(pUsers[1].xuid.dwUserFlags))
    {
        pVerifyTask->Request.qwPuids[2] = pUsers[1].xuid.qwUserID;
    }
    if (dwUserAccounts & XONLINE_LOGON_CONTROLLER3 && !XOnlineIsUserGuest(pUsers[2].xuid.dwUserFlags))
    {
        pVerifyTask->Request.qwPuids[3] = pUsers[2].xuid.qwUserID;
    }
    if (dwUserAccounts & XONLINE_LOGON_CONTROLLER4 && !XOnlineIsUserGuest(pUsers[3].xuid.dwUserFlags))
    {
        pVerifyTask->Request.qwPuids[4] = pUsers[3].xuid.qwUserID;
    }

    
    // BUGBUG: remove me!
    pVerifyTask->Request.qwPuids[0] = 23;
    pVerifyTask->Request.qwPuids[1] = 2;
    // END BUGBUG


    // 
    // start off by searching for the license in the cache
    //
    pVerifyTask->State = xoverifyCacheOpen;    
    
    pVerifyTask->index.dwOfferingId = dwOfferingId;
    pVerifyTask->cacheContext.pbIndexData = (PBYTE)&(pVerifyTask->index);
    
    pVerifyTask->cacheContext.pbRecordBuffer = (PBYTE)&(pVerifyTask->OfferingLicense);
    pVerifyTask->cacheContext.dwRecordBufferSize = sizeof(XO_OFFERING_LICENSE);

    
    hr = DoVerifyCacheOpen(pVerifyTask);
    
    *phTask = (XONLINETASK_HANDLE)pVerifyTask; 

Cleanup:    
    if (FAILED(hr) && pVerifyTask) 
    {
        SysFree(pVerifyTask);
        *phTask = NULL;
    }

    return(hr);
}

//---------------------------------------------------------
//
// VerifyLicenseTaskContinue()
//
//  Callback fn for the verify task pump
//
HRESULT CXo::VerifyLicenseTaskContinue(XONLINETASK_HANDLE hTask)
{
    HRESULT hr                                      = XONLINETASK_S_SUCCESS;
    DWORD dwUploadFlags                             = 0;
    XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask    = (XO_OFFERING_VERIFY_LICENSE_TASK*)hTask;

    Assert(hTask != NULL);


    switch (pVerifyTask->State) 
    {
    case xoverifyCacheOpen:
    case xoverifyCacheOpenPump:
        hr = DoVerifyCacheOpen(pVerifyTask);
        break;

    case xoverifyCacheLookup:
    case xoverifyCacheLookupPump:
        hr = DoVerifyCacheLookup(pVerifyTask);
        break;

    case xoverifyUpload:
    case xoverifyUploadPump:
        hr = DoVerifyServerRequest(pVerifyTask);            
        break;

    case xoverifyCacheUpdate:
    case xoverifyCacheUpdatePump:
        hr = DoVerifyCacheUpdate(pVerifyTask);            
        break;
    
    case xoverifyCacheClose:
    case xoverifyCacheClosePump:
        hr = DoVerifyCacheClose(pVerifyTask);            
        break;

    case xoverifyDone:
        hr = pVerifyTask->hrFinal;
        break;

    default:
        // unknown state!
        ASSERT(FALSE);
        break;

    } 

    
    if (hr != XONLINETASK_S_RUNNING)
    {
        pVerifyTask->State = xoverifyDone;
        pVerifyTask->hrFinal = hr;
    }

    return hr;
}

//---------------------------------------------------------
//
// VerifyLicenseTaskClose
//
//  Closes the handle associated with this task and
//  all sub-handles.
//
VOID CXo::VerifyLicenseTaskClose(XONLINETASK_HANDLE hTask)
{
    XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask    = (XO_OFFERING_VERIFY_LICENSE_TASK*)hTask;
    
    Assert(hTask != NULL);

    //
    // clean up cache handle
    //
    if (pVerifyTask->hCache)
    {
        pVerifyTask->hCache = NULL;
    }

    if (pVerifyTask->hCacheTask != NULL)
    {
        XOnlineTaskClose(pVerifyTask->hCacheTask);
        pVerifyTask->hCacheTask = NULL;
    }
    
    //
    // clean up xrl handle
    //
    if (pVerifyTask->hXrlTask != NULL)
    {
        XOnlineTaskClose(pVerifyTask->hXrlTask);
        pVerifyTask->hXrlTask = NULL;
    }
    
    
    // 
    // clean up our own task
    //
    SysFree(pVerifyTask);
}


//---------------------------------------------------------
//
// DoVerifyCacheOpen
//
//  Opens the cache.
//
HRESULT CXo::DoVerifyCacheOpen(XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask)
{
    HRESULT hr = XONLINETASK_S_RUNNING;

    if (pVerifyTask->State == xoverifyCacheOpen)
    {
        ASSERT(pVerifyTask->hCacheTask == NULL);
        pVerifyTask->State = xoverifyCacheOpenPump;
        hr = CacheOpen(XONLC_TYPE_LICENSING, NULL, pVerifyTask->Context.hEventWorkAvailable, &(pVerifyTask->hCache), &(pVerifyTask->hCacheTask));

        goto Cleanup;
    }

    ASSERT(pVerifyTask->State == xoverifyCacheOpenPump);
    
    hr = XOnlineTaskContinue(pVerifyTask->hCacheTask);

Cleanup:
    if (hr == XONLINETASK_S_SUCCESS)
    {
        // done opening, proceed to next task
        pVerifyTask->State = xoverifyCacheLookup;
        hr = XONLINETASK_S_RUNNING;
    }

    return hr;
}


//---------------------------------------------------------
//
// DoVerifyCacheLookup
//
//  Checks the local cache for any valid licenses.
//
HRESULT CXo::DoVerifyCacheLookup(XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask)
{
    HRESULT hr = XONLINETASK_S_RUNNING;
    int i;


    //
    // if there's an existing task, keep puming it.
    //
    if (pVerifyTask->State == xoverifyCacheLookupPump)
    {
        hr = XOnlineTaskContinue(pVerifyTask->hCacheTask);
        if (hr == XONLINETASK_S_RUNNING || FAILED(hr))
        {
            goto Cleanup;
        }

        pVerifyTask->State = xoverifyCacheLookup;

        // found the cached item!
        hr = VerifyRights(&(pVerifyTask->OfferingLicense), pVerifyTask->index.qwPuid);
        if (SUCCEEDED(hr))                                                                             
        {
            goto Cleanup;
        }
    }

    ASSERT(pVerifyTask->State == xoverifyCacheLookup);


    // 
    // search cache using the remaining puids
    //
    for(; pVerifyTask->nPuidIndex < 5; pVerifyTask->nPuidIndex++)
    {
        if (pVerifyTask->Request.qwPuids[pVerifyTask->nPuidIndex] == 0)
        {
            continue;
        }
        
        pVerifyTask->index.qwPuid = pVerifyTask->Request.qwPuids[pVerifyTask->nPuidIndex];
    
        
        hr = CacheLookup(pVerifyTask->hCache, &(pVerifyTask->cacheContext));
        if (FAILED(hr))
        {
            goto Cleanup;
        }

        if (!pVerifyTask->cacheContext.bValidDataInCache)
        {
            continue;
        }
        
        // found item, need to retrieve from disk?
        if (pVerifyTask->cacheContext.bCallRetrieveNext)
        {
            pVerifyTask->State = xoverifyCacheLookupPump;
            hr = CacheRetrieve(pVerifyTask->hCache, pVerifyTask->hCacheTask, &(pVerifyTask->cacheContext));
            if (hr == XONLINETASK_S_RUNNING || FAILED(hr))
            {
                goto Cleanup;
            }

            // retrieve succeeded, no need to pump
            pVerifyTask->State = xoverifyCacheLookup;
        }
        
        // is this really the right license?
        hr = VerifyRights(&(pVerifyTask->OfferingLicense), pVerifyTask->index.qwPuid);
        if (SUCCEEDED(hr))
        {
            goto Cleanup;
        }
    }

    // 
    // no cached license found, proceed to upload
    //
    //ASSERT(pVerifyTask->hCacheTask == NULL);
    pVerifyTask->State = xoverifyUpload;
    hr = XONLINETASK_S_RUNNING;
    
Cleanup:
    return hr;
}


//---------------------------------------------------------
// VerifyRights
//
//  checks if the given puid has rights according
//  to the data stored in a license.
//
HRESULT CXo::VerifyRights(XO_OFFERING_LICENSE* pLic, ULONGLONG puid)
{
    HRESULT hr = XONLINE_E_OFFERING_PERMISSION_DENIED;
    FILETIME ft; 
    FILETIME ftNow;
    
#ifdef XONLINE_FEATURE_XBOX 
    XCALCSIG_SIGNATURE Signature;
    HANDLE hSig;

    // check signature
    hSig = XCalculateSignatureBegin(XCALCSIG_FLAG_NON_ROAMABLE);
    XCalculateSignatureUpdate(hSig, (PBYTE)pLic, sizeof(XO_OFFERING_LICENSE) - sizeof(XCALCSIG_SIGNATURE));
    XCalculateSignatureEnd(hSig, &Signature);

    if (memcmp(&Signature, &(pLic->Signature), sizeof(XCALCSIG_SIGNATURE)) != 0)
    {
        goto Cleanup;
    }
#endif
       
    // check puid
    if (pLic->wConditionFlags & XO_LIC_USERID)
    {
        if (pLic->qwPassportUserID != puid)
        {
            goto Cleanup;
        }
    }
    
    GetSystemTimeAsFileTime(&ftNow);

    // check license refresh time
    if (pLic->wConditionFlags & XO_LIC_TIMEREFRESH)
    {
        if (CompareFileTime(&(pLic->filetimeRefresh), &ftNow) == -1)
        {
            goto Cleanup;
        }
    }

    // check when this license starts
    if (pLic->wConditionFlags & XO_LIC_TIMEFROM)
    {
        if (CompareFileTime(&(pLic->filetimeFrom), &ftNow) == 1)
        {
            goto Cleanup;
        }
    }

    // check whenthis license ends
    if (pLic->wConditionFlags & XO_LIC_TIMETO)
    {
        if (CompareFileTime(&(pLic->filetimeTo), &ftNow) == -1)
        {
            goto Cleanup;
        }
    }

    // all checks passed, they're good to go!
    hr = XONLINETASK_S_SUCCESS;

Cleanup:
    return hr;
}

//---------------------------------------------------------
// DoVerifyServerRequest
//
//  asks the server if the user has rights to this offering.
//  if it does, task->OfferingLicense is filled with the
//  license.
//
HRESULT CXo::DoVerifyServerRequest(XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask)
{
    HRESULT hr = XONLINETASK_S_RUNNING;

    #ifndef _XBILLING_DUMMY
    // TODO: handle this case.   They're most likely not logged in.
    Assert(SUCCEEDED(XOnlineGetServiceInfo(XONLINE_BILLING_OFFERING_SERVICE, NULL)));
    #endif

    //
    // if the task hasnt been opened, create it
    //
    if (pVerifyTask->State == xoverifyUpload)
    {
        pVerifyTask->State = xoverifyUploadPump;
        
        //
        // launch the upload
        //
        hr = XOnlineUploadFromMemory(
                XONLINE_BILLING_OFFERING_SERVICE,
                XO_OFFERING_VERIFY_LICENSE_XRL,
                (PBYTE)&(pVerifyTask->OfferingLicense),
                &(pVerifyTask->cbResults),
                NULL,
                0,
                (PBYTE)&(pVerifyTask->Request),
                sizeof(XO_VERIFY_LICENSE_REQUEST),
                XOBILLING_TIMEOUT,
                pVerifyTask->Context.hEventWorkAvailable,
                &(pVerifyTask->hXrlTask)
            );

        goto Cleanup;
    }
    
    ASSERT(pVerifyTask->State == xoverifyUploadPump);

    hr = XOnlineTaskContinue(pVerifyTask->hXrlTask);
    if (hr == XONLINETASK_S_SUCCESS)
    {
        // TODO: make sure the license is in the response

        // found a license -- add it to the cache.
        pVerifyTask->State = xoverifyCacheUpdate;
        hr = XONLINETASK_S_RUNNING;

        // note: even if adding it to the cache fails, license
        // check will succeed.
        pVerifyTask->hrFinal = S_OK;
        goto Cleanup;
    }


    
Cleanup:

    return hr;
}

//---------------------------------------------------------
// DoVerifyCacheUpdate
//
//  Updates the local cache with the new offering license.
//
HRESULT CXo::DoVerifyCacheUpdate(XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask)
{
    HRESULT hr;

    if (pVerifyTask->State == xoverifyCacheUpdate)
    {                                                  
#ifdef XONLINE_FEATURE_XBOX 
        //
        // set signature
        //
        XCALCSIG_SIGNATURE Signature;
        HANDLE hSig;
        
        hSig = XCalculateSignatureBegin(XCALCSIG_FLAG_NON_ROAMABLE);
        XCalculateSignatureUpdate(hSig, (PBYTE)&(pVerifyTask->OfferingLicense), sizeof(XO_OFFERING_LICENSE) - sizeof(XCALCSIG_SIGNATURE));
        XCalculateSignatureEnd(hSig, &(pVerifyTask->OfferingLicense.Signature));
#endif
        
        
        pVerifyTask->State = xoverifyCacheUpdatePump;

        // 
        // set index                                 y
        //
        pVerifyTask->index.dwOfferingId = pVerifyTask->OfferingLicense.dwOfferingId;
        if (pVerifyTask->OfferingLicense.wConditionFlags & XO_LIC_USERID)
        {
            // index on user puid
            pVerifyTask->index.qwPuid   = pVerifyTask->OfferingLicense.qwPassportUserID;
        }
        else
        {
            // index on machine puid
            pVerifyTask->index.qwPuid   = pVerifyTask->Request.qwPuids[0];
        }

        pVerifyTask->cacheContext.pbIndexData        = (PBYTE)&(pVerifyTask->index);
        pVerifyTask->cacheContext.pbRecordBuffer     = (PBYTE)&(pVerifyTask->OfferingLicense);
        pVerifyTask->cacheContext.dwRecordBufferSize = sizeof(XO_OFFERING_LICENSE);


        

        //
        // update cache
        //
        hr = CacheUpdate(pVerifyTask->hCache, pVerifyTask->hCacheTask, (PBYTE)&(pVerifyTask->index), (PBYTE)&(pVerifyTask->OfferingLicense));
        goto Cleanup;
    }

    ASSERT(pVerifyTask->State == xoverifyCacheUpdatePump);
    
    hr = XOnlineTaskContinue(pVerifyTask->hCacheTask);
    if (hr == XONLINETASK_S_SUCCESS)
    {
        pVerifyTask->State = xoverifyCacheClose;
        hr = XONLINETASK_S_RUNNING;
    }

Cleanup:
    return hr;
}

//---------------------------------------------------------
//
// DoVerifyCacheClose
//
//  Opens the cache.
//
HRESULT CXo::DoVerifyCacheClose(XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask)
{
    HRESULT hr = XONLINETASK_S_RUNNING;

    ASSERT(pVerifyTask->hCache != NULL);

    if (pVerifyTask->State == xoverifyCacheClose)
    {
        pVerifyTask->State = xoverifyCacheClosePump;
        hr = CacheClose(pVerifyTask->hCache);

        goto Cleanup;
    }

    ASSERT(pVerifyTask->State == xoverifyCacheClosePump);
    
    hr = XOnlineTaskContinue(pVerifyTask->hCacheTask);

Cleanup:
    if (hr == XONLINETASK_S_SUCCESS)
    {
        // all done
        pVerifyTask->State = xoverifyDone;
    }

    return hr;
}


