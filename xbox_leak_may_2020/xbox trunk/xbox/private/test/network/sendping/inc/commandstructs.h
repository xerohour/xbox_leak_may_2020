/*
 * File : CommandStructs.h
 * Author : Sandeep Prabhu
 * Revision History
 *      28/4/2000       sandeep           Created
 *
 * Structures used by the stubs in AutoSrv and Controller to send and receive data
 */

#ifndef __COMMANDSTRUCTS__H
#define __COMMANDSTRUCTS__H

#include <windows.h>
#include "common.h"
 
#define ANY_SIZE 1

#pragma pack(1)
/*
 *  New data types
 */                              
typedef struct
{
    BYTE bAddr[MAX_IPADDR_LEN];
    int iLength;
} IPADDR, *PIPADDR;

typedef struct
{
    BYTE bAddr[MAX_MACADDR_LEN];
    int iLength;
} MACADDR, *PMACADDR;  
                 
/*
 * Valid command codes supported by AutoSrv
 */               
typedef enum {
    CC_Cleanup,
    CC_Reboot,
    CC_Ping,
    CC_GetArpCache,
    CC_FlushArpCache,
    CC_DeleteArpCacheEntry,
    CC_AddArpCacheEntry,
    CC_CheckArpCacheEntry,
    CC_GetRouteTable,
    CC_DeleteRouteTableEntry,
    CC_AddRouteTableEntry,
    CC_GetNumInterfaces,
    CC_GetInterfaceIndex,
    CC_DeleteIP,
    CC_AddIP,
    CC_SendArp,
    CC_CreateRegistryKey, 
    CC_DeleteRegistryKey,
    CC_SetRegistryKeyValue,
    CC_GetRegistryKeyValue,
    CC_Socket,
    CC_BindEx,
    CC_Bind,
    CC_SetSocketOption,
    CC_ConnectEx,
    CC_AsyncConnectEx,
    CC_AcceptEx,
    CC_AsyncAcceptEx,
    CC_Connect,
    CC_Accept,
    CC_Send,
    CC_AsyncSend,
    CC_Receive,
    CC_AsyncReceive,
    CC_GetSessionStats,
    CC_GetAsyncStatus,
    CC_GetAsyncCompletionStatus,
    CC_Stop,
    CC_Close,
    CC_Reset,
    CC_Shutdown,
    CC_AsyncClose,
    CC_AsyncReset,
    CC_AsyncShutdown,
    CC_Unknown
} tCommandCode;

/*
 *  Response structure
 */
#define RESPONSE_HEADER_LENGTH (sizeof(tCommandCode)+3*sizeof(DWORD))

typedef struct _tGenericResponse {         
    tCommandCode CommandCode;
    DWORD dwTID;         
    DWORD dwStatus;
    DWORD dwDataSize;
    BYTE  abData[ANY_SIZE];
 } tGenericResponse;


/* 
 * Generic portion command to all requests sent out by Controller
 */
typedef struct {
    tCommandCode CommandCode;
    DWORD dwTID;
} tGenericStruct;


/*
 * Structures for specific commands
 */

typedef struct {
    tGenericStruct GenericPart;
} tArgs_Cleanup, tArgs_Reboot, tArgs_Ping, tArgs_GetArpCache, tArgs_GetRouteTable, tArgs_GetNumInterfaces;

typedef struct
{
    tGenericStruct GenericPart;
    DWORD dwAdapterIndex;
} tArgs_FlushArpCache;

typedef struct
{
    tGenericStruct GenericPart;
    DWORD dwAdapterIndex;
    IPADDR IPAddress;
    MACADDR MacAddress;
    DWORD dwType;
} tArgs_DeleteArpCacheEntry, tArgs_AddArpCacheEntry;

typedef struct
{
    tGenericStruct GenericPart;
    DWORD dwAdapterIndex;
    IPADDR IPAddress;
    MACADDR MacAddress;
} tArgs_CheckArpCacheEntry;

typedef struct
{
    tGenericStruct GenericPart;
    DWORD dwAdapterIndex;
    IPADDR TargetIP;
    IPADDR GWIP;
    IPADDR Mask;
    DWORD dwMetric;
    DWORD dwType;
} tArgs_DeleteRouteTableEntry, tArgs_AddRouteTableEntry;


typedef struct
{
    tGenericStruct GenericPart;
    TCHAR pszFriendlyName[MAX_INTERFACE_NAME_LENGTH];
} tArgs_GetInterfaceIndex;

typedef struct
{
    tGenericStruct GenericPart;
    ULONG ulNTEContext;
} tArgs_DeleteIP;

typedef struct
{
    tGenericStruct GenericPart;
    DWORD dwAdapterIndex;
    IPADDR IPAddress;
    IPADDR Mask;
} tArgs_AddIP;

typedef struct
{
    tGenericStruct GenericPart;
    IPADDR DestIP;
    IPADDR SrcIP;
} tArgs_SendArp;


typedef struct 
{
    tGenericStruct GenericPart;
    HKEY hMainKey;
    TCHAR aszSubKey[MAX_REG_SUBKEY_LENGTH];
    DWORD dwOptions;
    REGSAM Sam;
} tArgs_CreateRegistryKey;

typedef struct 
{
    tGenericStruct GenericPart;
    HKEY hMainKey;
    TCHAR aszSubKey[MAX_REG_SUBKEY_LENGTH];
} tArgs_DeleteRegistryKey;

typedef struct 
{
    tGenericStruct GenericPart;
    HKEY hMainKey;
    TCHAR aszSubKey[MAX_REG_SUBKEY_LENGTH];
    TCHAR aszValueName[MAX_REG_VALUENAME_LENGTH];
    DWORD dwType;
    BYTE abData[MAX_REG_VALUE_LENGTH];
    DWORD dwDataLen;
} tArgs_SetRegistryKeyValue;

typedef struct 
{
    tGenericStruct GenericPart;
    HKEY hMainKey;
    TCHAR aszSubKey[MAX_REG_SUBKEY_LENGTH];
    TCHAR aszValueName[MAX_REG_VALUENAME_LENGTH];
} tArgs_GetRegistryKeyValue;

/*
 * Session command structs
 */
typedef struct {
    tGenericStruct GenericPart;
    DWORD dwSessionId;    
} tGenericSessionStruct, tArgs_GetSessionStats, tArgs_Close, tArgs_Reset, tArgs_Shutdown;

typedef struct {
    tGenericStruct GenericPart;
    DWORD dwSessionId;
    INT iType;
} tArgs_Socket;

typedef struct {
    tGenericStruct GenericPart;
    DWORD dwSessionId;
    IPADDR SrcIP;
    USHORT usSrcPort;
} tArgs_BindEx;

typedef struct {
    tGenericStruct GenericPart;
    DWORD dwSessionId;
    INT iType;
    IPADDR SrcIP;
    USHORT usSrcPort;
} tArgs_Bind;

typedef struct {
    tGenericStruct GenericPart;
    DWORD dwSessionId;
    INT iOption;
    CHAR achValue[MAX_OPTION_SIZE];
    INT iSize;
} tArgs_SetSocketOption;

typedef struct {
    tGenericStruct GenericPart;
    DWORD dwSessionId;
    IPADDR DestIP;
    USHORT usDestPort;
} tArgs_ConnectEx;

typedef struct {
    tGenericStruct GenericPart;
    DWORD dwSessionId;
    INT iType;
    IPADDR SrcIP;
    USHORT usSrcPort;
    IPADDR DestIP;
    USHORT usDestPort;
} tArgs_Connect;

typedef struct {
    tGenericStruct GenericPart;
    DWORD dwSessionId;
    INT iType;
    IPADDR SrcIP;
    USHORT usSrcPort;
    INT iListenQ;
} tArgs_Accept;

typedef struct {
    tGenericStruct GenericPart;
    DWORD dwSessionId;
    INT iListenQ;
} tArgs_AcceptEx;

typedef struct {
    tGenericStruct GenericPart;
    DWORD dwSessionId;
    DWORD dwIterations;
    DWORD dwDelay;
    INT iBufferSize;
    BOOL fBufferSpecified;
    CHAR achBuffer[ANY_SIZE];
} tArgs_Send;

typedef struct {
    tGenericStruct GenericPart;
    DWORD dwSessionId;
    DWORD dwIterations;
    DWORD dwDelay;
    INT iBufferSize;
} tArgs_Receive;


typedef struct {
    tGenericStruct GenericPart;
    DWORD dwSessionId;
    DWORD dwAsyncTID;
} tArgs_GetAsyncStatus, tArgs_GetAsyncCompletionStatus, tArgs_Stop;

/*
 * Command specific responses
 */
typedef struct {
    DWORD dwBytesSent;
    DWORD dwBytesRcvd;
} tResponse_GetSessionStats;


typedef struct 
{
    DWORD dwType;
    DWORD dwDataLen;
    BYTE abValue[ANY_SIZE];
} tResponse_GetRegistryKeyValue;
#pragma pack()
      
#endif


