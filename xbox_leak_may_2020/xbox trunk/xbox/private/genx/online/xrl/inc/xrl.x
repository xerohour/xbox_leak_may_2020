/*==========================================================================;
 *
 *  Copyright (C) 2000 - 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:      xrl.h
 *  Content:   Xbox online XRL include file
 ***************************************************************************/

//
// XRLs are relative paths.  All XRLs must be valid XBox filesystem paths.
// No protocol information is supplied or allowed.
//

#ifndef __XRL__
#define __XRL__

#ifdef __cplusplus
extern "C" {
#endif



typedef char *XRL;
#define XRL_MAX_LEN     400


// PARSE_HEADERS_KNOWN is used to tell the parse headers function what you want to get out. Initialize
// it to zero, then only pass in pointers to values you care about.
typedef struct
{
    DWORD               dwHTTPResponseCode;
    ULARGE_INTEGER*     pContentLength;
    FILETIME*           pLastModifiedTime;
    FILETIME*           pDate;
} XRL_KNOWN_HEADERS;




//--------------------------------------------------------------------------------------------------------
// Blocking functions

HRESULT XRL_DownloadFile( const XRL xrl, DWORD cbBuffer, PBYTE pBuffer,
                                  FILETIME *pftLastModified, LPCSTR szPath, XRL_KNOWN_HEADERS *pHTTPHeaders );

HRESULT XRL_UploadFile( const XRL xrl, DWORD cbBuffer, PBYTE pBuffer,
                                  LPCSTR szPath, XRL_KNOWN_HEADERS *pHTTPHeaders );

HRESULT XRL_DownloadToMemory( const XRL xrl, DWORD* pcbBuffer, PBYTE pBuffer, XRL_KNOWN_HEADERS *pHTTPHeaders );


//@@BEGIN_MSINTERNAL

// These 3 routines are used by the SSL download routine

HRESULT XRL_LookupXRLIPAddress( XRL xrl, SOCKADDR_IN *pAddress, PXAPPSERVICEINFO pServiceInfo );

HRESULT XRL_BuildGetRequest( char* pBuffer, DWORD* pcbBuffer, XRL xrl, FILETIME* pModifiedSince, PXAPPSERVICEINFO pServiceInfo );

HRESULT XRL_ParseHeaders( char* pHeaders, DWORD cbHeaderSize, XRL_KNOWN_HEADERS* pH, PXAPPSERVICEINFO pServiceInfo );

//@@END_MSINTERNAL

#ifdef __cplusplus
}
#endif

#endif  //__XRL__
