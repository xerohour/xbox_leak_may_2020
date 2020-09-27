/*++

Copyright (c) Microsoft Corporation.  All rights reserved

Description:
    Module implementing Xbox Matchmaking APIs

Module Name:

    match.cpp

--*/

#include "xonp.h"
#include "xonver.h"

//
// Server configuration information for matchmaking POST requests
// to front doors.
//
typedef struct
{
    DWORD dwService;                  // Service ID number
    CHAR * szUrl;                   // relative URL of ISAPI front door
    CHAR * szContentType;           // Service content type
    DWORD dwContentType;              // Size of the service content type
} XPRESENCE_SERVERS, *PXPRESENCE_SERVERS;

const XPRESENCE_SERVERS g_MatchServers[3] =
{
    { XONLINE_MATCHMAKING_SERVICE, "/xmatch/xmatchhost.srf",       "Content-Type: xon/6\r\n", 21 },
    { XONLINE_MATCHMAKING_SERVICE, "/xmatch/xmatchclient.srf",     "Content-Type: xon/6\r\n", 21 },
    { XONLINE_MATCHMAKING_SERVICE, "/xmatch/xmatchhostdelete.srf", "Content-Type: xon/6\r\n", 21 }
};

enum SERVER_TYPE
{
    serverMatchHost = 0,
    serverMatchClient,
    serverMatchHostDelete
};

//
// Timeout for POST requests.  After 30 seconds, requests will timeout with a failure.
//
#define DW_POST_TIMEOUT         30000

//
// Size of the send buffer -- including headers.
//
#define DW_LEN_SENDBUFFER 175

//
// Defines for attribute information.  Makes it easier to incorporate changes to these types.
//
#define ATTRIBUTE_OFFSET_TYPE        DWORD
#define ATTRIBUTE_ID_TYPE            DWORD
#define ATTRIBUTE_STRING_LENGTH_TYPE WORD
#define ATTRIBUTE_BLOB_LENGTH_TYPE   WORD

//
// Type of matchmaking request
//
enum XMATCH_TYPE
{
    xmatchCreate = 0,                           // Session creation/advertisment
    xmatchSearch,                               // Session search
    xmatchDelete                                // Session delete
};

//
// Extended task handle structure
//
struct XMATCH_EXTENDED_HANDLE
{
    XONLINETASK_CONTEXT  XMatchTaskHandle;      // Base task handle
    XONLINETASK_HANDLE   UploadTaskHandle;      // Task handle for XRL Upload
#if DBG
    DWORD                dwSignature;           // Debug signature for verification
#endif
    XMATCH_TYPE          Type;                  // Type of matchmaking request
    XNKID                SessionID;             // Session ID of new session                               
    XNKEY                KeyExchangeKey;        // Key of session
    DWORD                dwResultsLength;       // Length of results returned from search request
    DWORD                dwResponseCode;        // Response from server
    DWORD                dwMaxSearchResults;    // Maximum number of search results
    DWORD                dwActualSearchResults; // Actual search results returned
    PXMATCH_SEARCHRESULT *rgpSearchResults;     // Array of pointers to search results
    BYTE                 *pbResultsBuffer;      // Buffer of search results
    UINT                  cbResultsBufferAlloc; // Allocated size of results buffer
    BYTE                 *pbBuffer;             // Send buffer
};

//
// Write an Attribute Offset and increment the pointer to next Attribute offset
//
#define WriteAndIncAttributeOffset(pdwOffset, pbOffset) *pdwOffset = (DWORD) pbOffset; pdwOffset += 1;

//
// Write an Attribute ID and increment the pointer to next Attribute ID
//
#define WriteAndIncAttributeID(pByte, dwAttributeID)    *((DWORD *)pByte) = dwAttributeID; pByte += sizeof(DWORD);

//
// Write an Integer Attribute and increment the pointer to next Integer Attribute
//
#define WriteAndIncInt(pByte, qwInt)                    *((ULONGLONG *)pByte) = qwInt; pByte += sizeof(ULONGLONG);

//
// Write an Attribute String Length and increment the pointer to next Attribute String Length
//
#define WriteAndIncStringLen(pByte, cLength)            *((ATTRIBUTE_STRING_LENGTH_TYPE *)pByte) = cLength; pByte += sizeof(ATTRIBUTE_STRING_LENGTH_TYPE);

//
// Write an Attribute Blob Length and increment the pointer to next Attribute Blob Length
//
#define WriteAndIncBlobLen(pByte, cLength)              *((ATTRIBUTE_BLOB_LENGTH_TYPE *)pByte) = cLength; pByte += sizeof(ATTRIBUTE_BLOB_LENGTH_TYPE);

//
// Retrieve the Attribute ID
//
#define GetAttributeID(pByte)                           *((DWORD *) pByte)

//
// Retrieve the Attribute String Size
//
#define GetAttributeStringSize(pByte)                   *((ATTRIBUTE_STRING_LENGTH_TYPE *) (pByte+sizeof(DWORD)))

//
// Retrieve the Attribute Blob Size
//
#define GetAttributeBlobSize(pByte)                     *((ATTRIBUTE_BLOB_LENGTH_TYPE *) (pByte+sizeof(DWORD)))

//
// Return TRUE if the SessionID specified is not Zero
//
// Note:  This assumes that XNKID will always be a ULONGLONG type.  If this
// ever changes, we have to modify this.
//
#define IsSessionIDAvailable(SessionID)              (*((ULONGLONG *) &SessionID) != 0)

//
// Debug signature for extra validation
//
#define XMATCH_SIGNATURE                             'XMAT'
#define IsValidXMatchHandle(TaskHandle)             (((PXMATCH_EXTENDED_HANDLE)TaskHandle)->dwSignature == XMATCH_SIGNATURE)              

//---------------------------------------------------------------------------
//
// XOnlineMatchSessionCreate
//
// Public API
//
// Function to create/advertize a session
//
HRESULT
CXo::XOnlineMatchSessionCreate(
    IN DWORD dwPublicCurrent,
    IN DWORD dwPublicAvailable,
    IN DWORD dwPrivateCurrent,
    IN DWORD dwPrivateAvailable,
    IN DWORD dwNumAttributes,
    IN PXONLINE_ATTRIBUTE pAttributes,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    XoEnter("XOnlineMatchSessionCreate");
    XoCheck((dwNumAttributes == 0) == (pAttributes == NULL));
    XoCheck(phTask != NULL);

    DWORD i              = 0;
    XNKID SessionIDDummy = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // When creating a session, ID must be zero

    FixupAttributeIDs(dwNumAttributes, pAttributes);

    XoCheck(ValidateAttributeIDs(dwNumAttributes, pAttributes, TRUE));

    //
    // Since this is a session CREATE request, the fChanged flag
    // is set for every attribute so that all attributes get sent
    // up with the create request.
    //
    for (i = 0; i < dwNumAttributes; i += 1)
    {
        pAttributes[i].fChanged = TRUE;
    }

    //
    // Call private function that creates and/or updates session
    //

    return(XoLeave(SessionCreate( SessionIDDummy, dwPublicCurrent, dwPublicAvailable, dwPrivateCurrent, dwPrivateAvailable, dwNumAttributes, pAttributes, hWorkEvent, phTask)));
}

//---------------------------------------------------------------------------
//
// XOnlineMatchSessionUpdate
//
// Public API
//
// Function to update a session advertisement
//
HRESULT
CXo::XOnlineMatchSessionUpdate(
    IN XNKID SessionID,
    IN DWORD dwPublicCurrent,
    IN DWORD dwPublicAvailable,
    IN DWORD dwPrivateCurrent,
    IN DWORD dwPrivateAvailable,
    IN DWORD dwNumAttributes,
    IN PXONLINE_ATTRIBUTE pAttributes,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    XoEnter("XOnlineMatchSessionUpdate");
    XoCheck((dwNumAttributes == 0) == (pAttributes == NULL));
    XoCheck(IsSessionIDAvailable(SessionID));
    XoCheck(phTask != NULL);

    FixupAttributeIDs(dwNumAttributes, pAttributes);

    XoCheck(ValidateAttributeIDs(dwNumAttributes, pAttributes, TRUE));

    //
    // Call private function that creates and/or updates session
    //

    return(XoLeave(SessionCreate( SessionID, dwPublicCurrent, dwPublicAvailable, dwPrivateCurrent, dwPrivateAvailable, dwNumAttributes, pAttributes, hWorkEvent, phTask )));
}


//---------------------------------------------------------------------------
//
// XOnlineMatchSessionGetInfo 
//
// Public API
//
// Function to retrieve the session ID that was created in the
// create session call.  It will return zero until the session
// creation has been completed.
//
HRESULT
CXo::XOnlineMatchSessionGetInfo(
    IN XONLINETASK_HANDLE hTask,
    OUT XNKID *pSessionID,
    OUT XNKEY *pKeyExchangeKey
    )
{
    XoEnter("XOnlineMatchSessionGetInfo");
    XoCheck(hTask != NULL && IsValidXMatchHandle(hTask));
    XoCheck(pSessionID != NULL);
    XoCheck(pKeyExchangeKey != NULL);

    PXMATCH_EXTENDED_HANDLE pExtendedHandle = (PXMATCH_EXTENDED_HANDLE) hTask;

#if DBG
    if (!IsSessionIDAvailable(pExtendedHandle->SessionID))
    {
        TraceSz(Warning, "XOnlineMatchSessionGetInfo: Send session description has not completed successfully.  No Session ID to return.");
    }
#endif

    //
    // Copy session ID and key exchange key into buffer
    //
    memcpy(pSessionID, &pExtendedHandle->SessionID, sizeof(pExtendedHandle->SessionID));
    memcpy(pKeyExchangeKey, &pExtendedHandle->KeyExchangeKey, sizeof(pExtendedHandle->KeyExchangeKey));

    return(XoLeave(S_OK));
}


//---------------------------------------------------------------------------
//
// XOnlineMatchSessionDelete
//
// Public API
//
// Function to delete session.
//
HRESULT
CXo::XOnlineMatchSessionDelete(
    IN XNKID SessionID,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    XoEnter("XOnlineMatchSessionDelete");
    XoCheck(IsSessionIDAvailable(SessionID));
    XoCheck(phTask != NULL);

    HRESULT                 hr              = S_OK;
    PXMATCH_EXTENDED_HANDLE pExtendedHandle = NULL;
    PXMATCH_SESSION_DELETE  pHeader         = NULL;
    DWORD                   dwBufferLen     = DW_LEN_SENDBUFFER;

    //
    // Initialize task handle to NULL in case of error
    //
    *phTask = NULL;

    //
    // Create and fill in the Extended Task Handle structure and allocate any required
    // buffers
    //
    hr = CreateContext( &pExtendedHandle, sizeof(XMATCH_SESSION_DELETE), 0, SessionID, hWorkEvent, 0, xmatchDelete );

    if (SUCCEEDED(hr))
    {
        //
        // Fill in the delete request protocol structure
        //
        pHeader = (PXMATCH_SESSION_DELETE) pExtendedHandle->pbBuffer;
    
        pHeader->dwMessageLength   = sizeof(XMATCH_SESSION_DELETE);
        pHeader->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
        pHeader->dwTitleID         = m_dwTitleId;

        memcpy(&pHeader->SessionID, &SessionID, sizeof(SessionID));

        *phTask = (XONLINETASK_HANDLE) pExtendedHandle;
    }

    //
    // Post delete request to the server.  Timeout after 30 seconds.
    //
    hr = XOnlineUploadFromMemory( g_MatchServers[serverMatchHostDelete].dwService, g_MatchServers[serverMatchHostDelete].szUrl,
                                  NULL, &dwBufferLen,
                                  (PBYTE)g_MatchServers[serverMatchHostDelete].szContentType, g_MatchServers[serverMatchHostDelete].dwContentType,
                                  pExtendedHandle->pbBuffer, pHeader->dwMessageLength,
                                  DW_POST_TIMEOUT,
                                  hWorkEvent, &pExtendedHandle->UploadTaskHandle );

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineMatchSearch
//
// Public API.
//
// Function to search for a session
//
HRESULT
CXo::XOnlineMatchSearch(
    IN DWORD dwProcedureIndex,
    IN DWORD dwNumResults,
    IN DWORD dwNumAttributes,
    IN PXONLINE_ATTRIBUTE pAttributes,
    IN DWORD dwResultsLen,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    XoEnter("XOnlineMatchSearch");
    XoCheck(dwProcedureIndex != X_SSINDEX_GET_SESSION);
    XoCheck(dwNumResults > 0);
    XoCheck((dwNumAttributes == 0) == (pAttributes == NULL));
    XoCheck(dwResultsLen > 0);
    XoCheck(phTask != NULL);
    XoCheck(ValidateAttributeIDs(dwNumAttributes, pAttributes, FALSE));

    //
    // Call private search function that is used for both general searches and FindFromID()
    //
    return(XoLeave(MatchSearch(dwProcedureIndex, dwNumResults, dwNumAttributes, pAttributes, dwResultsLen, hWorkEvent, phTask)));
}


//---------------------------------------------------------------------------
//
// XOnlineMatchSessionFindFromID
//
// Public API.
//
// Function to search for a particular session ID.
//
HRESULT
CXo::XOnlineMatchSessionFindFromID(
    IN XNKID SessionID,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    XoEnter("XOnlineMatchSessionFindFromID");
    XoCheck(IsSessionIDAvailable(SessionID));
    XoCheck(phTask != NULL);

    XONLINE_ATTRIBUTE Attribute;

    //
    // Create the one parameter we have to pass into the FindFromID stored procedure.
    //
    // Note:  This assumes that XNKID will always be a ULONGLONG type.  If this
    // ever changes, we have to modify this to append a blob instead.
    //
    Attribute.info.integer.qwValue = *((ULONGLONG*) &SessionID);
    Attribute.dwAttributeID = X_ATTRIBUTE_DATATYPE_INTEGER;

    return(XoLeave(MatchSearch( X_SSINDEX_GET_SESSION, 1, 1, &Attribute, DW_LEN_SENDBUFFER, hWorkEvent, phTask )));
}


//---------------------------------------------------------------------------
//
// XOnlineMatchSearchGetResults
//
// Public API.
//
// Function to retrieve the search results from the
// search request.
//
// Note: Callers will NOT have to free rgpSearchResults when
// they are done, but they must call XOnlineTaskFree(hSearch)
// AFTER they are done with prgpSearchResults.
//
// prgpSearchResults returns a pointer to a vector of pointers to 
// PXMATCH_SEARCHRESULT structures (vs. pointer to an array of structs).
// i.e. ((*pprgpSearchResults)[i])->SessionID is the ID of the ith sessions
// returned.
//
HRESULT CXo::XOnlineMatchSearchGetResults(
    IN XONLINETASK_HANDLE hTask,
    OUT PXMATCH_SEARCHRESULT **prgpSearchResults,
    OUT DWORD *pdwReturnedResults
    )
{
    XoEnter("XOnlineMatchSearchGetResults");
    XoCheck(hTask != NULL && IsValidXMatchHandle(hTask));
    XoCheck(prgpSearchResults != NULL);
    XoCheck(pdwReturnedResults != NULL);

    PXMATCH_EXTENDED_HANDLE pExtendedHandle    = (PXMATCH_EXTENDED_HANDLE) hTask;
    DWORD                   i                  = 0;
    PBYTE                   pbSearchResult     = NULL;
    PBYTE                   pbNextSearchResult = NULL;
    PBYTE                   pbEndBuffer        = NULL;
    HRESULT                 hr                 = S_OK;

    //
    // Initialize number of search results to zero in case of error
    //
    *pdwReturnedResults = 0;

    //
    // If a search was not completed successfully, return an error to client
    //
    if (pExtendedHandle->dwResponseCode != 200)
    {
        hr = E_FAIL; // BUGBUG: maybe pick a different error code here.
        goto Cleanup;
    }

    //
    // Parse through search results buffer and collect pointers to beginning
    // of each session.
    //
    pbSearchResult = pExtendedHandle->pbResultsBuffer;
    pbEndBuffer = pExtendedHandle->pbResultsBuffer + pExtendedHandle->dwResultsLength;

    for ( i = 0; i < pExtendedHandle->dwMaxSearchResults; i += 1 )
    {
        //
        // If there is enough room left in the buffer for a full search results structure, we can continue.
        // Otherwise, there is an incomplete search result and we end.
        //
        if ((pbSearchResult + sizeof(XMATCH_SEARCHRESULT)) <= pbEndBuffer)
        {
            //
            // Set pointer to next search result
            //
            pbNextSearchResult = pbSearchResult + ((PXMATCH_SEARCHRESULT) pbSearchResult)->dwResultLength;

            //
            // If the new pointer is beyond the end of our buffer, it means this search result
            // is not complete.  Otherwise, it is complete and we update our array of pointers
            // to include this search result.
            //
            if (pbNextSearchResult <= pbEndBuffer)
            {
                //
                // Add this pointer to the array of search result pointers
                //
                pExtendedHandle->rgpSearchResults[i] = (PXMATCH_SEARCHRESULT) pbSearchResult;

                //
                // Increment the number of search results we have
                //
                (*pdwReturnedResults) += 1;
            }

            //
            // Continue with next search result
            //
            pbSearchResult = pbNextSearchResult;
        }
        else // It's not a complete search result structure
        {
            break;
        }
    }

    //
    // Return the array of search result pointers
    //
    *prgpSearchResults = pExtendedHandle->rgpSearchResults;
    pExtendedHandle->dwActualSearchResults = *pdwReturnedResults;

Cleanup:

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineMatchSearchParse
//
// Public API
//
// Function to parse individual sessions in a set of search results.
//
HRESULT CXo::XOnlineMatchSearchParse(
    IN PXMATCH_SEARCHRESULT pSearchResult,
    IN DWORD dwNumSessionAttributes,
    IN PXONLINE_ATTRIBUTE_SPEC pSessionAttributeSpec,
    OUT PVOID pQuerySession
    )
{
    XoEnter("XOnlineMatchSearchParse");
    XoCheck(pSearchResult != NULL);
    XoCheck(pSearchResult->dwNumAttributes == dwNumSessionAttributes);
    XoCheck(pSessionAttributeSpec != NULL);
    XoCheck(pQuerySession != NULL);

    ATTRIBUTE_OFFSET_TYPE   *pAttributeOffset = NULL;
    PBYTE                   pbAttribute       = NULL;
    HRESULT                 hr                = S_OK;
    DWORD                   i                 = 0;
    DWORD                   dwAttributeID     = 0;
    PBYTE                   pbCopyTo          = (PBYTE) pQuerySession;

    //
    // Loop through all attributes for this particular session
    //
    for (i = 0; i < pSearchResult->dwNumAttributes; i += 1)
    {
        //
        // Set pointers to the offset of the attribute and the attribute itself.
        //
        pAttributeOffset = ((ATTRIBUTE_OFFSET_TYPE *) (((PBYTE) pSearchResult) + sizeof(XMATCH_SEARCHRESULT))) + i;
        pbAttribute = ((PBYTE) pSearchResult) + *pAttributeOffset;

        //
        // Get the ID of the attribute
        //
        dwAttributeID = GetAttributeID(pbAttribute);

        //
        // Retrieve attribute and copy it into the user-supplied buffer
        //
        switch (dwAttributeID)
        {
            case X_ATTRIBUTE_DATATYPE_INTEGER:
            {
                //
                // Verify that the Attribute Spec matches the attribute we're retrieving
                //
                Assert(pSessionAttributeSpec[i].type == X_ATTRIBUTE_DATATYPE_INTEGER);

                //
                // Write the integer attribute to the user-supplied buffer
                //
                WriteAndIncInt(pbCopyTo, *((ULONGLONG *) (pbAttribute + sizeof(DWORD))));
                break;
            }

            case X_ATTRIBUTE_DATATYPE_STRING:
            {
                //
                // Verify that the Attribute Spec matches the attribute we're retrieving
                //
                Assert(pSessionAttributeSpec[i].type == X_ATTRIBUTE_DATATYPE_STRING);

                //
                // Verify that the size of the supplied string attribute buffer is big enough to hold the attribute
                //
                Assert(pSessionAttributeSpec[i].length >= GetAttributeStringSize(pbAttribute));

                //
                // Convert the UTF8 string to Unicode and copy into the user-supplied buffer.
                //
                MultiByteToWideChar( CP_UTF8, 0, (LPSTR) (pbAttribute + sizeof(DWORD) + sizeof(ATTRIBUTE_STRING_LENGTH_TYPE)), GetAttributeStringSize(pbAttribute), (LPWSTR) pbCopyTo, pSessionAttributeSpec[i].length / sizeof(WCHAR) );

                //
                // Move pointer to beginning of next attribute in user-supplied buffer
                //
                pbCopyTo += pSessionAttributeSpec[i].length;
                break;
            }

            case X_ATTRIBUTE_DATATYPE_BLOB:
            {
                //
                // Verify that the Attribute Spec matches the attribute we're retrieving
                //
                Assert(pSessionAttributeSpec[i].type == X_ATTRIBUTE_DATATYPE_BLOB);

                //
                // Verify that the size of the supplied blob attribute buffer is big enough to hold the attribute
                //
                Assert(pSessionAttributeSpec[i].length >= GetAttributeBlobSize(pbAttribute));

                //
                // Copy the blob size into the supplied attribute buffer
                //
                WriteAndIncBlobLen(pbCopyTo, GetAttributeBlobSize(pbAttribute));

                //
                // Copy the blob into the supplied attribute buffer
                //
                memcpy(pbCopyTo, pbAttribute + sizeof(DWORD) + sizeof(ATTRIBUTE_BLOB_LENGTH_TYPE), GetAttributeBlobSize(pbAttribute));

                //
                // Move pointer to beginning of next attribute in user-supplied buffer
                //
                pbCopyTo += pSessionAttributeSpec[i].length;
                break;
            }

            case X_ATTRIBUTE_DATATYPE_NULL:
            {
                //
                // Skip this attribute.  We assume that the developer set this buffer section to a known value, and
                // they should check against this known value to know if it's a NULL attribute or not.
                //

                //
                // For a blob type, write zero to the blob size first
                //
                if (pSessionAttributeSpec[i].type == X_ATTRIBUTE_DATATYPE_BLOB)
                {
                    WriteAndIncBlobLen(pbCopyTo, 0);
                }

                pbCopyTo += pSessionAttributeSpec[i].length;
                break;
            }
        }
    }

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// SessionCreate
//
// Private API.  Called by MatchSessionCreate() and MatchSessionUpdate().
//
// Function to open a session creation request.
//
HRESULT CXo::SessionCreate(
    IN XNKID SessionID,
    IN DWORD dwPublicCurrent,
    IN DWORD dwPublicAvailable,
    IN DWORD dwPrivateCurrent,
    IN DWORD dwPrivateAvailable,
    IN DWORD dwNumAttributes,
    IN PXONLINE_ATTRIBUTE pAttributes,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    HRESULT                 hr                     = S_OK;
    PXMATCH_EXTENDED_HANDLE pExtendedHandle        = NULL;
    PXMATCH_SESSION         pHeader                = NULL;
    DWORD                   dwMessageLength        = 0;
    DWORD                   dwResultsLen           = DW_LEN_SENDBUFFER;
    DWORD                   dwNumUpdatedAttributes = 0;
    DWORD                   i                      = 0;

    //
    // Initialize task handle in case of error
    //
    *phTask = NULL;

#if DBG

    AssertSz(!m_fSessionCreateInProgress, "Can only create one match session at a time");

    //
    // Set global flag to indicate that a SessionCreate is in progress.  Only one SessionCreate
    // can be in progress at a time.
    //

    m_fSessionCreateInProgress = TRUE;

#endif

    //
    // Count the number of attributes with the fChanged flag set to TRUE.
    //
    for (i = 0; i < dwNumAttributes; i += 1)
    {
        if (pAttributes[i].fChanged == TRUE)
        {
            dwNumUpdatedAttributes += 1;
        }
    }

    //
    // Determine how big the message buffer should be to hold complete post
    // request.  Also figure out the number of real attributes, since PRIVATE_AVAILABLE
    // and PUBLIC_AVAILABLE are not really attributes.
    //
    dwMessageLength = sizeof(XMATCH_SESSION) + TotalAttributeSize(dwNumUpdatedAttributes, pAttributes);

    //
    // Create and fill in extended task handle structure and allocate any required buffers
    //
    hr = CreateContext( &pExtendedHandle, dwMessageLength, 0, SessionID, hWorkEvent, 0, xmatchCreate );

    if (SUCCEEDED(hr))
    {
        //
        // Fill in session create protocol structure
        //
        pHeader = (PXMATCH_SESSION) pExtendedHandle->pbBuffer;

        //
        // Retrieve IP address of Xbox
        //
        XNetGetTitleXnAddr(&pHeader->HostAddress);

        pHeader->dwMessageLength    = dwMessageLength;
        pHeader->dwProtocolVersion  = X_MATCH_PROTOCOL_VERSION;
        pHeader->dwTitleID          = m_dwTitleId;

        //
        // Initially set public and private available slots to zero.
        //
        pHeader->dwPublicAvailable  = dwPublicAvailable;
        pHeader->dwPrivateAvailable = dwPrivateAvailable;
        pHeader->dwPublicCurrent    = dwPublicCurrent;
        pHeader->dwPrivateCurrent   = dwPrivateCurrent;
        pHeader->dwNumAttributes    = dwNumUpdatedAttributes;
        memcpy(&pHeader->SessionID, &SessionID, sizeof(SessionID));

        //
        // Call function that will parse session attributes and add them to our POST request buffer
        //
        WriteAttributes(pExtendedHandle->pbBuffer, dwNumAttributes, pAttributes, (DWORD *) (pExtendedHandle->pbBuffer + sizeof(XMATCH_SESSION)), pExtendedHandle->pbBuffer + sizeof(XMATCH_SESSION) + (dwNumAttributes * sizeof(ATTRIBUTE_OFFSET_TYPE)));

        //
        // POST our session creation request.  Timeout after 30 seconds.
        //
        hr = XOnlineUploadFromMemory( g_MatchServers[serverMatchHost].dwService,
                                      g_MatchServers[serverMatchHost].szUrl,
                                      pExtendedHandle->pbResultsBuffer, &dwResultsLen,
                                      (PBYTE)g_MatchServers[serverMatchHost].szContentType, g_MatchServers[serverMatchHost].dwContentType,
                                      pExtendedHandle->pbBuffer, pHeader->dwMessageLength,
                                      DW_POST_TIMEOUT,
                                      hWorkEvent, &pExtendedHandle->UploadTaskHandle );

        //
        // Return the task handle
        //
        *phTask = (XONLINETASK_HANDLE) pExtendedHandle;
    }

    return(hr);
}


//---------------------------------------------------------------------------
//
// MatchSearch
//
// Private API.  Called from MatchSearch() and MatchFindFromID()
//
// Search for a matching session.
//
HRESULT CXo::MatchSearch(
    IN DWORD dwProcedureIndex,
    IN DWORD dwNumResults,
    IN DWORD dwNumAttributes,
    IN PXONLINE_ATTRIBUTE pAttributes,
    IN DWORD dwResultsLen,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    HRESULT                 hr                  = S_OK;
    PXMATCH_EXTENDED_HANDLE pExtendedHandle     = NULL;
    PXMATCH_SEARCH          pHeader             = NULL;
    XNKID                   SessionIDDummy      = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    DWORD                   dwMessageLength     = 0;
    DWORD                   i                   = 0;

    //
    // Initialize task handle to NULL in case of error
    //
    *phTask = NULL;

    //
    // Since this is a session SEARCH request, the fChanged flag
    // is set for every attribute so that all attributes get sent
    // up with the search request.
    //
    for (i = 0; i < dwNumAttributes; i += 1)
    {
        pAttributes[i].fChanged = TRUE;
    }

    //
    // Determine required size of message buffer
    //
    dwMessageLength = sizeof(XMATCH_SEARCH) + TotalAttributeSize(dwNumAttributes, pAttributes);

    //
    // Create and fill in extended task handle structure and allocate any required buffers
    //
    hr = CreateContext( &pExtendedHandle, dwMessageLength, dwResultsLen, SessionIDDummy, hWorkEvent, dwNumResults, xmatchSearch );

    if (SUCCEEDED(hr))
    {
        //
        // Fill in session search protocol structure
        //
        pHeader = (PXMATCH_SEARCH) pExtendedHandle->pbBuffer;
    
        pHeader->dwMessageLength   = dwMessageLength;
        pHeader->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
        pHeader->dwTitleID         = m_dwTitleId;
        pHeader->dwProcedureIndex  = dwProcedureIndex;
        pHeader->dwNumParameters   = dwNumAttributes;

        //
        // Call function that will parse session attributes and add them to our POST request buffer
        //
        WriteAttributes(pExtendedHandle->pbBuffer, dwNumAttributes, pAttributes, (DWORD *) (pExtendedHandle->pbBuffer + sizeof(XMATCH_SEARCH)), pExtendedHandle->pbBuffer + sizeof(XMATCH_SEARCH) + (dwNumAttributes * sizeof(ATTRIBUTE_OFFSET_TYPE)));

        //
        // POST our session creation request.  Timeout after 30 seconds.
        //
        hr = XOnlineUploadFromMemory( g_MatchServers[serverMatchClient].dwService,
                                      g_MatchServers[serverMatchClient].szUrl,
                                      pExtendedHandle->pbResultsBuffer, &dwResultsLen,
                                      (PBYTE)g_MatchServers[serverMatchClient].szContentType, g_MatchServers[serverMatchClient].dwContentType,
                                      pExtendedHandle->pbBuffer, pHeader->dwMessageLength,
                                      DW_POST_TIMEOUT,
                                      hWorkEvent, &pExtendedHandle->UploadTaskHandle );

        //
        // Return the task handle
        //
        *phTask = (XONLINETASK_HANDLE) pExtendedHandle;
    }

    return hr;
}


//---------------------------------------------------------------------------
//
// CreateContext
//
// Private API.
//
// Create an extended task handle structure for matchmaking.  Fill in default
// values in this structure.  Allocate any buffers that are required for
// the particular matchmaking request. 
//
HRESULT CXo::CreateContext(
    OUT PXMATCH_EXTENDED_HANDLE *ppExtendedHandle,  // Pointer to extended task handle structure
    IN DWORD dwMsgLen,                              // Length of POST request
    IN DWORD dwResultsLen,                          // Length of expected response
    IN XNKID SessionID,                             // ID of session
    IN HANDLE hWorkEvent,                           // Event to set when work is available
    IN DWORD dwNumResults,                          // Number of results we ask for
    IN XMATCH_TYPE Type                             // Type of request
    )
{
    HRESULT hr = S_OK;
    
    //
    // Allocate mem for the extended handle
    //
    (*ppExtendedHandle) = (PXMATCH_EXTENDED_HANDLE)SysAllocZ(sizeof(XMATCH_EXTENDED_HANDLE), PTAG_XMATCH_EXTENDED_HANDLE);
    if ((*ppExtendedHandle) == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    //
    // Initialize the matchmaking task handle and set the handler functions and event.
    //
    TaskInitializeContext(&((*ppExtendedHandle)->XMatchTaskHandle));
    (*ppExtendedHandle)->XMatchTaskHandle.pfnContinue = MatchContinue;
    (*ppExtendedHandle)->XMatchTaskHandle.pfnClose = MatchClose;
    (*ppExtendedHandle)->XMatchTaskHandle.hEventWorkAvailable = hWorkEvent;

    //
    // Allocate a buffer for the POST request
    //
    (*ppExtendedHandle)->pbBuffer = (BYTE *) SysAllocZ(dwMsgLen, PTAG_XMATCH_POST_BUFFER);
    if ((*ppExtendedHandle)->pbBuffer == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    //
    // If we expect a response from the server, allocate memory to hold the response
    //
    if (dwResultsLen != 0)
    {
        (*ppExtendedHandle)->pbResultsBuffer = (BYTE *) SysAlloc(dwResultsLen, PTAG_XMATCH_RESULTS_BUFFER);
        if ((*ppExtendedHandle)->pbResultsBuffer == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto Error;
        }
        (*ppExtendedHandle)->cbResultsBufferAlloc = dwResultsLen;
    }
    else
    {
        (*ppExtendedHandle)->pbResultsBuffer = NULL;
        (*ppExtendedHandle)->cbResultsBufferAlloc = 0;
    }

    //
    // If it's a search request, allocate an array of pointers to search results
    //
    if (dwNumResults != 0)
    {
        (*ppExtendedHandle)->rgpSearchResults = (PXMATCH_SEARCHRESULT *) SysAlloc(dwNumResults * sizeof(PXMATCH_SEARCHRESULT),
            PTAG_XMATCH_SEARCHRESULT);
        if ((*ppExtendedHandle)->rgpSearchResults == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto Error;
        }
    }
    else
    {
        (*ppExtendedHandle)->rgpSearchResults = NULL;
    }

    //
    // Fill in the remaining fields of the extended task handle
    //
    (*ppExtendedHandle)->Type               = Type;
    (*ppExtendedHandle)->dwResultsLength    = 0;
    (*ppExtendedHandle)->UploadTaskHandle   = NULL;
    (*ppExtendedHandle)->dwResponseCode     = 0;
    (*ppExtendedHandle)->dwMaxSearchResults = dwNumResults;
    memcpy(&(*ppExtendedHandle)->SessionID, &SessionID, sizeof(SessionID));

#if DBG
    //
    // Debug signature to do extra validation
    //
    (*ppExtendedHandle)->dwSignature            = XMATCH_SIGNATURE;
#endif

Cleanup:
    return(hr);

Error:

    if ((*ppExtendedHandle)->pbBuffer != NULL)
    {
        SysFree( (*ppExtendedHandle)->pbBuffer );
    }

    if ((*ppExtendedHandle)->pbResultsBuffer != NULL)
    {
        SysFree( (*ppExtendedHandle)->pbResultsBuffer );
    }

    if ((*ppExtendedHandle)->rgpSearchResults != NULL)
    {
        SysFree( (*ppExtendedHandle)->rgpSearchResults );
    }

    if ((*ppExtendedHandle) != NULL)
    {
        SysFree(*ppExtendedHandle);
    }

    goto Cleanup;
}



//---------------------------------------------------------------------------
//
// FixupAttributeIDs
//
// Private API.
//
// Add Session flag to all attribute IDs.  User attributes are no longer
// supported, so we have to manually add the Session flag to all
// attribute IDs.
//
VOID CXo::FixupAttributeIDs(
    IN  DWORD dwNumAttributes,
    IN  PXONLINE_ATTRIBUTE pAttributes
    )
{
    DWORD i = 0;

    //
    // Loop through all attributes and set SESSION flag.
    //
    for (i = 0; i < dwNumAttributes; i += 1)
    {
        pAttributes[i].dwAttributeID |= X_ATTRIBUTE_TYPE_SESSION;
    }

    return;
}


//---------------------------------------------------------------------------
//
// TotalAttributeSize
//
// Private API.
//
// Calculate size of all attributes, including attribute offsets and
// attribute IDs.
//
DWORD CXo::TotalAttributeSize(
    IN  DWORD dwNumAttributes,
    IN  PXONLINE_ATTRIBUTE pAttributes
    )
{
    DWORD i = 0;
    DWORD dwSize = dwNumAttributes * ( sizeof(ATTRIBUTE_OFFSET_TYPE) + sizeof(ATTRIBUTE_ID_TYPE) );

    //
    // Loop through all attributes and add size of attribute to dwSize.
    //
    for (i = 0; i < dwNumAttributes; i += 1)
    {
        switch (pAttributes[i].dwAttributeID & X_ATTRIBUTE_DATATYPE_MASK)
        {
            case X_ATTRIBUTE_DATATYPE_INTEGER:
            {
                //
                // Integer attributes are just a ULONGLONG
                //
                dwSize += sizeof(ULONGLONG);

                break;
            }

            case X_ATTRIBUTE_DATATYPE_STRING:
            {
                //
                // String attributes are string length + UTF8 string, including NULL.
                //
                dwSize += sizeof(ATTRIBUTE_STRING_LENGTH_TYPE) + WideCharToMultiByte( CP_UTF8, 0, pAttributes[i].info.string.pwszValue, lstrlenW(pAttributes[i].info.string.pwszValue)+1, NULL, 0, NULL, NULL );

                break;
            }

            case X_ATTRIBUTE_DATATYPE_BLOB:
            {
                //
                // Blob attributes are blob length + size of blob
                //
                dwSize += sizeof(ATTRIBUTE_BLOB_LENGTH_TYPE) + pAttributes[i].info.blob.dwLength;

                break;
            }

            default:
            {
                // BUGBUG: Get rid of this case.  Do validation at top of every public function
                Assert(FALSE);
                break;
            }
        }
    }

    return dwSize;
}


//---------------------------------------------------------------------------
//
// WriteAttributes
//
// Private API.
//
// Function to write attributes to message buffer.  It assumes that the
// message buffer is big enough to hold all attributes.  It returns the
// actual number of attributes that had the fChanged flag set to TRUE.
//
VOID CXo::WriteAttributes(
    IN PBYTE pbBufferStart,               // Beginning of buffer, used to calculate offsets
    IN DWORD dwNumAttributes,             // Number of attributes
    IN PXONLINE_ATTRIBUTE pAttributes,    // Array of attribute structures
    IN DWORD *pdwAttributeOffset,         // Pointer to beginning of attribute offsets
    IN PBYTE pbAttribute                  // Pointer to beginning of attributes
    )
{
    DWORD i                      = 0;
    DWORD dwUTF8Length           = 0;

    //
    // Loop through all attributes
    //
    for (i = 0; i < dwNumAttributes; i += 1)
    {
        //
        // Only write an attribute to the buffer if it has changed (fChanged == TRUE).
        // In the case of a session CREATE request, all of the fChanged flags are set to
        // true before calling this function.
        //
        if (pAttributes[i].fChanged == TRUE)
        {
            //
            // Reset the fChanged flag so we know which flags have changed since the last update.
            //
            pAttributes[i].fChanged = FALSE;

            //
            // Write the actual attributes to the buffer
            //
            switch (pAttributes[i].dwAttributeID & X_ATTRIBUTE_DATATYPE_MASK)
            {
                case X_ATTRIBUTE_DATATYPE_INTEGER:
                {
                    //
                    // Write the attribute offset and increment the pointer to the next attribute offset
                    //
                    WriteAndIncAttributeOffset(pdwAttributeOffset, (pbAttribute - pbBufferStart));

                    //
                    // Write the integer attribute ID and the integer.  Increment
                    // the pointer to the next attribute.
                    //
                    WriteAndIncAttributeID(pbAttribute, pAttributes[i].dwAttributeID);
                    WriteAndIncInt(pbAttribute, pAttributes[i].info.integer.qwValue);

                    break;
                }

                case X_ATTRIBUTE_DATATYPE_STRING:
                {
                    Assert(pAttributes[i].info.string.pwszValue != NULL);
                    Assert(lstrlenW(pAttributes[i].info.string.pwszValue) <= X_MAX_STRING_ATTRIBUTE_LEN);

                    //
                    // Write the attribute offset and increment the pointer to the next attribute offset
                    //
                    WriteAndIncAttributeOffset(pdwAttributeOffset, (pbAttribute - pbBufferStart));

                    //
                    // Calculate the length of the UTF8 string.
                    //
                    dwUTF8Length = WideCharToMultiByte( CP_UTF8, 0, pAttributes[i].info.string.pwszValue, lstrlenW(pAttributes[i].info.string.pwszValue)+1, NULL, 0, NULL, NULL );

                    //
                    // Write the string attribute ID
                    //
                    WriteAndIncAttributeID(pbAttribute, pAttributes[i].dwAttributeID);

                    //
                    // Write the string length
                    //
                    WriteAndIncStringLen(pbAttribute, (ATTRIBUTE_STRING_LENGTH_TYPE) (dwUTF8Length));

                    //
                    // Write the UTF8 string
                    //
                    WideCharToMultiByte( CP_UTF8, 0, pAttributes[i].info.string.pwszValue, lstrlenW(pAttributes[i].info.string.pwszValue)+1, (LPSTR)pbAttribute, dwUTF8Length, NULL, NULL );

                    //
                    // Increment the pointer to the next attribute
                    //
                    pbAttribute += dwUTF8Length;

                    break;
                }
                                              
                case X_ATTRIBUTE_DATATYPE_BLOB:
                {
                    Assert(!((pAttributes[i].info.blob.dwLength > 0) && (pAttributes[i].info.blob.pvValue == NULL)));
                    Assert(pAttributes[i].info.blob.dwLength <= X_MAX_BLOB_ATTRIBUTE_LEN);

                    //
                    // Write the attribute offset and increment the pointer to the next attribute offset
                    //
                    WriteAndIncAttributeOffset(pdwAttributeOffset, (pbAttribute - pbBufferStart));

                    //
                    // Write the blob attribute ID
                    //
                    WriteAndIncAttributeID(pbAttribute, pAttributes[i].dwAttributeID);

                    //
                    // Write the blob length
                    //
                    WriteAndIncBlobLen(pbAttribute, (ATTRIBUTE_BLOB_LENGTH_TYPE) pAttributes[i].info.blob.dwLength);

                    //
                    // If the blob specified is not NULL, write the blob and increment the pointer to the next attribute
                    //
                    if (pAttributes[i].info.blob.pvValue != NULL)
                    {
                        memcpy( pbAttribute, pAttributes[i].info.blob.pvValue, pAttributes[i].info.blob.dwLength );
                        pbAttribute += pAttributes[i].info.blob.dwLength;
                    }

                    break;
                }

                default:
                {
                    // BUGBUG: Should never get here since the attribute list has been validated already
                    break;
                }
            }
        }
    }

    return;
}


//---------------------------------------------------------------------------
//
// MatchContinue()
//
// Public API (sort of)
//
// This is called when user calls TaskContinue with any matchmaking task
// handle.
//
HRESULT CXo::MatchContinue(XONLINETASK_HANDLE hTask)
{
    HRESULT                 hr               = XONLINETASK_S_RUNNING;
    PXMATCH_EXTENDED_HANDLE pExtendedHandle  = (PXMATCH_EXTENDED_HANDLE)hTask;
    PXMATCH_SESSION         pHeader          = NULL;
    PBYTE                   pbUpload         = NULL;
    DWORD                   dwStatus         = 0;
    DWORD                   cbUpload         = 0;
    ULARGE_INTEGER          uliContentLength;
    ULARGE_INTEGER          uliTotalReceived;
    BOOL                    fCleanHandle     = FALSE;

    Assert(hTask != NULL);
    Assert(IsValidXMatchHandle(hTask));

    //
    // If there is no XRL Upload task handle, this task must be complete
    //
    if (pExtendedHandle->UploadTaskHandle == NULL)
    {
        hr = XONLINETASK_S_SUCCESS;
        goto Error;
    }

    //
    // Tell the XRL Upload task handle to continue doing work
    //
    hr = XOnlineTaskContinue(pExtendedHandle->UploadTaskHandle);

    //
    // If the task is complete, error or success...
    //
    if (hr != XONLINETASK_S_RUNNING)
    {
        fCleanHandle = TRUE;

        //
        // If the task failed, return the error
        //
        if (FAILED(hr))
        {
            goto Error;
        }

        //
        // If the task is not successful, return the error.  This should never happen though, since we should have
        // caught it in the conditional above.
        //
        if (hr != XONLINETASK_S_SUCCESS)
        {
            // BUGBUG: Should never get here
            Assert(FALSE);
            __asm int 3;
            goto Error;
        }

        //
        // Get the results of the XRL Upload
        //
        hr = XOnlineUploadGetResults(pExtendedHandle->UploadTaskHandle, &pbUpload, &cbUpload, &uliTotalReceived, &uliContentLength, &dwStatus, NULL);

        //
        // Store the response code so the search parse functions know if it was
        // successful or not.
        //
        pExtendedHandle->dwResponseCode = dwStatus;

        //
        // If it wasn't a successful server code, return an error
        //
        if (pbUpload == NULL || dwStatus != 200)
        {
            hr = E_FAIL; // BUGBUG: find a better error code
            goto Error;
        }

        //
        // If it was successful, handle the results
        //
        if (SUCCEEDED(hr))
        {
            //
            // If it was a session create request, copy the new session info into our extended task handle
            //
            if ( pExtendedHandle->Type == xmatchCreate )
            {
                if (cbUpload > 0)
                {
                    pHeader = (PXMATCH_SESSION) pExtendedHandle->pbBuffer;

                    //
                    // Copy the new session ID and key exchange key into our extended task handle
                    //
                    memcpy(&pExtendedHandle->SessionID, &((PXMATCH_SESSION_INFO)pbUpload)->SessionID, sizeof(pExtendedHandle->SessionID));
                    memcpy(&pExtendedHandle->KeyExchangeKey, &((PXMATCH_SESSION_INFO)pbUpload)->KeyExchangeKey, sizeof(pExtendedHandle->KeyExchangeKey));
                }
            }
            else if ( pExtendedHandle->Type == xmatchDelete ) // Delete request
            {
                //
                // Do nothing if it was a session delete request.  No data comes back from these requests.
                //
            }
            else // Search request
            {
                //
                // Store the size of the response in our extended task handle.  This will later
                // be used to parse the search results.
                //
                if (uliContentLength.LowPart < pExtendedHandle->cbResultsBufferAlloc)
                {
                    pExtendedHandle->dwResultsLength = uliContentLength.LowPart;
                }
                else
                {
                    pExtendedHandle->dwResultsLength = pExtendedHandle->cbResultsBufferAlloc;
                }
            }
        }
        else
        {
            goto Error;
        }
    }

  Error:

    if (fCleanHandle == TRUE && pExtendedHandle->UploadTaskHandle != NULL)
    {
        //
        // Close the XRL Upload task handle
        //
        XOnlineTaskClose(pExtendedHandle->UploadTaskHandle);
        pExtendedHandle->UploadTaskHandle = NULL;
    }

    //
    // If the task so far has been successful but not complete (ie.  XONLINETASK_S_SUCCESS), return S_RUNNING.
    // Otherwise, return an error or XONLINE_S_SUCCESS if it's complete and successful.
    //
    if (SUCCEEDED(hr) && hr != XONLINETASK_S_SUCCESS)
    {
        hr = XONLINETASK_S_RUNNING;
    }

    return hr;
}                


//---------------------------------------------------------------------------
//
// MatchClose()
//
// Public API (sort of)
//
// Close the matchmaking task handle.
//
VOID CXo::MatchClose(XONLINETASK_HANDLE hTask)
{
    HRESULT                 hr              = S_OK;
    PXMATCH_EXTENDED_HANDLE pExtendedHandle = (PXMATCH_EXTENDED_HANDLE)hTask;

    Assert(hTask != NULL);
    Assert(IsValidXMatchHandle(hTask));

#if DBG

    //
    // Reset global flag indicating a SessionCreate request is in progress.
    //
    if ( pExtendedHandle->Type == xmatchCreate )
    {
        m_fSessionCreateInProgress = FALSE;
    }

#endif

    //
    // If the XRL Upload handle is still open, close it.
    //
    if (pExtendedHandle->UploadTaskHandle != NULL)
    {
        XOnlineTaskClose(pExtendedHandle->UploadTaskHandle);
        pExtendedHandle->UploadTaskHandle = NULL;
    }

    //
    // Free all buffers that have been allocated
    //
    if (pExtendedHandle->pbBuffer != NULL)
    {
        SysFree(pExtendedHandle->pbBuffer);
    }

    if (pExtendedHandle->pbResultsBuffer != NULL)
    {
        SysFree(pExtendedHandle->pbResultsBuffer);
    }

    if (pExtendedHandle->rgpSearchResults != NULL)
    {
        SysFree(pExtendedHandle->rgpSearchResults);
    }

    //
    // Free the matchmaking task handle
    //
    SysFree(pExtendedHandle);
}


#if DBG

//---------------------------------------------------------------------------
//
// ValidateAttributeIDs
//
// Private API.
//
// Validate that attribute IDs are correct
//
BOOL CXo::ValidateAttributeIDs(
    IN DWORD dwNumAttributes,
    IN PXONLINE_ATTRIBUTE pAttributes,
    IN BOOL fSessionCreate
    )
{
    DWORD i = 0;

    //
    // Loop through all attributes and verify that the attribute ID is correct.
    //
    for (i = 0; i < dwNumAttributes; i += 1)
    {
        if (IsValidAttributeID(pAttributes[i].dwAttributeID, fSessionCreate) == FALSE)
        {
            return FALSE;
        }
    }

    return TRUE;
}


//---------------------------------------------------------------------------
//
// IsValidAttributeID()
//
// Private API
//
// Validate attribute ID.
//
BOOL CXo::IsValidAttributeID(DWORD dwAttributeID, BOOL fSessionCreate)
{
    if (fSessionCreate == TRUE)
    {
        //
        // bit 31     Global or Title Specific.
        // Can be zero or one for session create
        //

        //
        // bit 24-27  Used to be Attribute Type, but now must be 0001 for a session create
        //   0000 = user attribute
        //   0001 = game session attribute
        //   0010-1111 = reserved for future use
        //
        if ((dwAttributeID & X_ATTRIBUTE_RESERVED3_MASK) != X_ATTRIBUTE_TYPE_SESSION)
        {
            return FALSE;
        }

        //
        // bit 0-15  Attribute Index
        // Can be any value for session create
        //
    }
    else
    {
        //
        // bit 31     Global or Title Specific.
        // Must be zero for session search
        //
        if ((dwAttributeID & X_ATTRIBUTE_SCOPE_MASK) != 0)
        {
            return FALSE;
        }

        //
        // bit 24-27  Used to be Attribute Type, but now must be zero for a session search
        //   0000 = user attribute
        //   0001 = game session attribute
        //   0010-1111 = reserved for future use
        //
        if ((dwAttributeID & X_ATTRIBUTE_RESERVED3_MASK) != 0)
        {
            return FALSE;
        }

        //
        // bit 0-15  Attribute Index
        // Must be zero for session search
        //
        if ((dwAttributeID & X_ATTRIBUTE_ID_MASK) != 0)
        {
            return FALSE;
        }
    }

    //
    // bit 28-30  Reserved for future use, must be zero
    //
    if ((dwAttributeID & X_ATTRIBUTE_RESERVED1_MASK) != 0)
    {
        return FALSE;
    }

    //
    // bit 20-23  Attribute Data Type
    //   0000 = integer
    //   0001 = string
    //   0010 = binary blob
    //   0011-1111 = reserved for future use
    //
    if ((dwAttributeID & X_ATTRIBUTE_DATATYPE_MASK) > X_ATTRIBUTE_DATATYPE_BLOB)
    {
        return FALSE;
    }

    //
    // bit 16-19  Reserved for future use, must be zero
    //
    if ((dwAttributeID & X_ATTRIBUTE_RESERVED2_MASK) != 0)
    {
        return FALSE;
    }

    return TRUE;
}
#endif
