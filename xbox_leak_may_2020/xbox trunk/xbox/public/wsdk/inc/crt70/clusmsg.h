/*++

Copyright (c) 1991-1996  Microsoft Corporation

Module Name:

    clusmsg.h

Abstract:

    This file contains the message definitions for the cluster service.

Author:

    davidp 19-Sep-1996

Revision History:

Notes:


--*/



#ifndef _CLUSMSG_H_
#define _CLUSMSG_H_

/*
 The next starting point is 5893.
 Error codes 5000 thru 5089 conflict with net\inc\apperr2.h error codes.
 If we could move these error codes up by 800, we'd be all set.
 The range 5800 to 5889 is temporarily reserved for the 5000 range.
*/
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: ERROR_CLUSTER_MEMBERSHIP_INVALID_STATE_OLD
//
// MessageText:
//
//  An operation was attempted that is incompatible 
//  with the current membership state of the node.
//
#define ERROR_CLUSTER_MEMBERSHIP_INVALID_STATE_OLD 5090L

//
// MessageId: ERROR_CLUSTER_QUORUMLOG_NOT_FOUND_OLD
//
// MessageText:
//
//  The quorum resource does not contain the quorum log.
//
#define ERROR_CLUSTER_QUORUMLOG_NOT_FOUND_OLD 5091L

/*
; DON'T INSERT NEW MSGS HERE
;*/
//
// MessageId: ERROR_CLUSTER_MEMBERSHIP_INVALID_STATE
//
// MessageText:
//
//  An operation was attempted that is incompatible with the current membership
//  state of the node.
//
#define ERROR_CLUSTER_MEMBERSHIP_INVALID_STATE 5890L

//
// MessageId: ERROR_CLUSTER_QUORUMLOG_NOT_FOUND
//
// MessageText:
//
//  The quorum resource does not contain the quorum log.
//
#define ERROR_CLUSTER_QUORUMLOG_NOT_FOUND 5891L

//
// MessageId: ERROR_CLUSTER_MEMBERSHIP_HALT
//
// MessageText:
//
//  The membership engine requested shutdown of the cluster service on this node.
//
#define ERROR_CLUSTER_MEMBERSHIP_HALT    5892L

/*
; INSERT NEW MSGS HERE
*/
//
// MessageId: CLUSTER_BUILD_PLACEHOLDER
//
// MessageText:
//
//  With no messages, build will continously try to rebuild clusmsg.h which causes
//  all sorts of things to get rebuilt.
//
#define CLUSTER_BUILD_PLACEHOLDER        5999L

#endif // _CLUSMSG_H_
