/*++

Copyright (c) 2000 Microsoft Corporation

    xppp.h

Abstract:

    Xbox PPP APIs.

--*/

#ifndef __XPPP_H__
#define __XPPP_H__

/*
 * Errors
 */
#define PPP_ERROR_BASE                  600
#define ERROR_PENDING                   (PPP_ERROR_BASE + 0)
#define ERROR_BUFFER_TOO_SMALL          (PPP_ERROR_BASE + 3)
#define ERROR_UNKNOWN                   (PPP_ERROR_BASE + 35)
#define ERROR_AUTHENTICATION_FAILURE    (PPP_ERROR_BASE + 91)
#define ERROR_PPP_INVALID_PACKET        (PPP_ERROR_BASE + 122)
#define ERROR_PPP_NOT_CONVERGING        (PPP_ERROR_BASE + 132)
#define ERROR_PPP_CP_REJECTED           (PPP_ERROR_BASE + 133)
#define ERROR_PPP_LOOPBACK_DETECTED     (PPP_ERROR_BASE + 137)

/*
 * API entry points
 */
BOOL PppConnect(CHAR *pszUsername, CHAR *pszPassword, CHAR *pszDomain, CHAR *pszNumber);

#endif // __XPPP_H__

