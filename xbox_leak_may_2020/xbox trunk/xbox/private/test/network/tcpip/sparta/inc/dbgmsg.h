/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: dbgmsg.c                                                                    *
* Description: Enables the filtering and setting of various debug traces                *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/16/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_DBGMSG_H__
#define __SPARTA_DBGMSG_H__


#ifdef __cplusplus
extern "C" {
#endif

/*++

HACKED PORT TO USER MODE

--*/

extern UINT DbgSettings;

//
// This function MUST be provided by the application
// This string is always printed to the log.
//
void LogDebugMessage(TCHAR *string);

#if DBG


VOID DEBUGPRINT(TCHAR *szFormat, ...);


#define SETDEBUGMASK(x) (DbgSettings = x)

#define DEBUGZONE(n)  (DbgSettings & (0x00000001<<n))

// returns current call stack. Note that it's not thread-safe and
// you must check the thread id's in the stack trace yourself to see
// which thread is doing what. Caller's responsibility to free returned
// buffer.

TCHAR *DBGMSG_CreateStackStringCopy();


    
#define DEBUGMSG(dbgs,format) ((dbgs) ? printindenting format : 0)
    
void printindenting(TCHAR *szFormat, ...);
    
//
// debug levels for user mode code.
// zones 16-31 are reserved for the app calling this lib
//
// this is the mask set at driver initialization
#define INITIAL_DEBUG_MASK      0x0000c000
#define DEBUG_ALL               0x0000FFFF


#define DBG_INIT                DEBUGZONE(0)
#define DBG_SPARTA_PROTOCOLAPI  DEBUGZONE(1)
#define DBG_SPARTA_PACKETAPI    DEBUGZONE(2)
#define DBG_SPARTA_DRIVERAPI    DEBUGZONE(3)
#define DBG_SPARTA_MEMORYAPI    DEBUGZONE(4)
#define DBG_REF                 DEBUGZONE(12)
#define DBG_VERBOSE             DEBUGZONE(13)
#define DBG_WARN                DEBUGZONE(14)
#define DBG_ERROR               DEBUGZONE(15)


//
// debug levels for the driver.  Must be same as in driver\inc\dbgdrv.h
// to be used in calls to NI_SetDriverDebugMask
//
#define SYS_DBG_INIT                DEBUGZONE(0)

#define SYS_DBG_NDIS_OPEN           DEBUGZONE(1)
#define SYS_DBG_NDIS_BIND           DEBUGZONE(2)
#define SYS_DBG_NDIS_TX             DEBUGZONE(3)
#define SYS_DBG_NDIS_RX             DEBUGZONE(4)
#define SYS_DBG_NDIS_REQ            DEBUGZONE(5)
#define SYS_DBG_NDIS_PNP            DEBUGZONE(6)
#define SYS_DBG_NDIS_RESET          DEBUGZONE(7)
#define SYS_DBG_NDIS_STATUS         DEBUGZONE(8)


#define SYS_DBG_RX                  DEBUGZONE(16)
#define SYS_DBG_TX                  DEBUGZONE(17)
#define SYS_DBG_OPENCB              DEBUGZONE(18)
#define SYS_DBG_DEVIOCTL            DEBUGZONE(19)
#define SYS_DBG_OID                 DEBUGZONE(20)
#define SYS_DBG_RESET               DEBUGZONE(21)
#define SYS_DBG_ENUMERATE           DEBUGZONE(22)

#define SYS_DBG_REF                 DEBUGZONE(28)
#define SYS_DBG_VERBOSE             DEBUGZONE(29)
#define SYS_DBG_WARN                DEBUGZONE(30)
#define SYS_DBG_ERROR               DEBUGZONE(31)

//
// This function MUST be provided by the application
// String is written to debugger, or to log if flag set property (spartacom)
//
void PrintDebug(TCHAR *string);

#else

#define DEBUGMSG(d,f)   (0)

#endif


#ifdef __cplusplus
} // for the extern "C" declaration
#endif

#endif // __SPARTA_DBGMSG_H__

