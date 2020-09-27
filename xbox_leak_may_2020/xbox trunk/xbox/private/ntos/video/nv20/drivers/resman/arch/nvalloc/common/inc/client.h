#ifndef _CLIENT_H_
#define _CLIENT_H_
/******************************************************************************
*
*   File:  client.h
*
*   Description:
*       This file contains the data structures needed to describe all
*   client information.
*
*
*   THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
*   NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
*   IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
*
******************************************************************************/

#define NUM_CLIENTS 160
#define NUM_DEVICES 8

typedef struct _def_client_memory_info
{
    PHWINFO             pDev;
    U032                Handle;
    U032                Parent;
    U032                Class;
    VOID*               Address;
    U032                Length;
    U032                Type;
    U032                Physicality;
    U032                Coherency;
    U032                Instance;
    U032                version;   // used to keep track of how an instance of class NV01_MEMORY_LOCAL_USER was allocated - C.Moidel
    U032                HeapOwner; /* if version > 0 then HeapOwner is the handle to a valid instance of NV04_HEAP_OWNER
                                                     This means that both the memory and HeapOwner were allocated with RmAlloc()
                                      if version = 0 then HeapOwner is a non-unique user created ID and there 
                                                     is NOT a HeapOwner object behind that ID.  Do not register and unregister with the HeapOwner!
                                                     This means that the memory was allocated with either RmAllocMemory() or RmArchHeap()  - C.Moidel*/  
    VOID*               MemData;   // private per-alloc data
    struct _def_client_memory_info* Next;
    struct _def_client_memory_info* Prev;

} CLI_MEMORY_INFO, *PCLI_MEMORY_INFO;

// FIFO channel info
typedef struct _def_client_fifo_info
{
    U032                Handle;
    U032                Device;
    U032                Class;
    U032                ErrorDmaContext;
    U032                DataDmaContext;
    VOID*               FifoPtr;
    U032                DevID;
    U032                InUse;
    U032                ChID;
    U032                AppID;
    void               *AppHandle;
    U032                Selector;
    VOID*               Flat;
    U032                heventWait;
    V032                hwndNotify;
    V032                hwndError;
    U032                msgNotify;
    U032                msgError;
    VOID*               lpSysCallback;
    U032                eventNotify;
    VOID*               pDmaObject;

} CLI_FIFO_INFO, *PCLI_FIFO_INFO;

// device information
typedef struct _def_client_device_info
{
    U032                Handle;
    U032                Client;
    U032                Class;
    U032                InUse;
    U032                AppID;
    U032                DevID;
    void               *AppHandle;
    PCLI_FIFO_INFO      DevFifoList;
    PCLI_MEMORY_INFO    DevMemoryList;

} CLI_DEVICE_INFO, *PCLI_DEVICE_INFO;

typedef struct _def_client_event_user
{
    U032                Handle;
    U032                Channel;
    struct _def_client_event_user* Next;
    struct _def_client_event_user* Prev;

} CLI_EVENT_USER, *PCLI_EVENT_USER;

typedef struct _def_client_event_info
{
    U032                Handle;
    U032                Class;
    PCLI_EVENT_USER     UserList;
    struct _def_client_event_info* Next;
    struct _def_client_event_info* Prev;

} CLI_EVENT_INFO, *PCLI_EVENT_INFO;


//Heap Owner info structure - added by Chuck Moidel
/* TODO: Create a heap_owner data structure that stores device & handle information about allocations
   that it "ownes" rather than doing a search throughout the data structres. */

typedef struct _def_client_heap_owner_info
{
    U032                Handle;
    U032                Client;
    U032                Class;
    U032                BlockReferenceCount;
//  PCLI_MEMORY_INFO    HeapOwnerMemoryList; 
//  PCLI_DEVICE_INFO    CliDeviceList;
    struct _def_client_heap_owner_info* Next;
    struct _def_client_heap_owner_info* Prev;

} CLI_HEAP_OWNER_INFO, *PCLI_HEAP_OWNER_INFO;

// client information
typedef struct _def_client_info
{
    U032                 Handle;
    U032                 Class;
    U032                 InUse;
    U032                 AppID;
    U032                 ProcID;
    void                *AppHandle;
    U032                 ProcContext;
    U032                 FifoCount;
    PCLI_DMA_INFO        CliDmaList;
    PCLI_DEVICE_INFO     CliDeviceList;
    PCLI_MEMORY_INFO     CliMemoryList;
    PCLI_EVENT_INFO      CliEventList;
    PCLI_HEAP_OWNER_INFO CliHeapOwnerList;   // modified by Chuck Moidel

} CLIENTINFO, *PCLIENTINFO;

//
// FIFO channel info.
//

typedef struct _def_channel_info
{
    U032  InUse;
    U032  ChID;
    U032  AppID;
    VOID *AppHandle;
    U032  Selector;
    VOID* Flat;
    U032  heventWait;
    V032  hwndNotify;
    V032  hwndError;
    U032  msgNotify;
    U032  msgError;
    VOID* lpSysCallback;
} CHANNELINFO, *PCHANNELINFO;


RM_STATUS           CliAddClient            (U032*, U032);
BOOL                CliDelClient            ();
BOOL                CliGetClientHandle      (U032*);
BOOL                CliGetClientIndex       (U032, U032*);
BOOL                CliGetClientAppHandle   (U032, void**);
BOOL                CliSetClientContext     (U032);
BOOL                CliSetClientAppHandle   (U032, void*);
BOOL                CliSetClientInstance    (U032);
RM_STATUS           CliGetClientHandleFromChID(PHWINFO, U032, U032*);
RM_STATUS           CliAddDevice            (U032, U032, U032);
BOOL                CliDelDevice            (U032);
BOOL                CliMakeDeviceList       (U032, U032, PCLI_DEVICE_INFO*);
BOOL                CliGetDeviceInfo        (U032, PCLI_DEVICE_INFO*);
PCLI_DEVICE_INFO    CliGetDeviceList        ();
BOOL                CliGetDeviceHandle      (U032, U032*);
BOOL                CliGetDeviceAppHandle   (U032, void**);
BOOL                CliSetDeviceContext     (U032, PHWINFO*);
BOOL                CliSetDeviceAppHandle   (U032, void*);
BOOL                CliGetDeviceClassString (U032, char*);
RM_STATUS           CliAddDeviceFifo        (PHWINFO, U032, U032, U032, U032, U032, U032, U032, U032, VOID*);
BOOL                CliDelDeviceFifo        (U032, U032);
BOOL                CliMakeDeviceFifoList   (U032, U032, U032, PCLI_FIFO_INFO*);
BOOL                CliGetDeviceFifoInfo    (U032, U032, PCLI_FIFO_INFO*);
BOOL                CliGetDeviceFifoList    (U032, PCLI_FIFO_INFO*);
RM_STATUS           CliAddDeviceMemory      (U032, U032, U032, VOID*, U032, U032, U032, U032);
BOOL                CliDelDeviceMemory      (U032, U032);
BOOL                CliDelDeviceHeapOwner   (U032, U032);
BOOL                CliGetDeviceMemoryInfo  (U032, U032, PCLI_MEMORY_INFO*);
PCLI_MEMORY_INFO    CliGetDeviceMemoryList  (U032);
RM_STATUS           CliAddDma               (U032, U032, U032, U032, U032, VOID*, U032, PCLI_DMA_INFO*);
BOOL                CliDelDma               (U032);
BOOL                CliGetDmaInfo           (U032, PCLI_DMA_INFO*);
PCLI_DMA_INFO       CliGetDmaList           ();
RM_STATUS           CliAddMemory            (PHWINFO, U032, U032, U032, U032, VOID*, U032, VOID*);
BOOL                CliDelMemory            (U032);
BOOL                CliGetMemoryInfo        (U032, PCLI_MEMORY_INFO*);
BOOL                CliGetMemoryInfoForLinAddr(VOID *, PCLI_MEMORY_INFO *);
PCLI_MEMORY_INFO    CliGetMemoryList        ();
RM_STATUS           CliAddEvent             (U032, U032, U032, U032, PCLI_EVENT_INFO*);
BOOL                CliDelEvent             (U032);
BOOL                CliDelEventUser         (U032, U032);
U032                CliGetEventUserHandle   (PCLI_EVENT_USER, U032);
BOOL                CliGetEventInfo         (U032, PCLI_EVENT_INFO*);
PCLI_EVENT_INFO     CliGetEventList         ();
BOOL                CliFindObject           (U032, POBJECT*, U032*);
BOOL                CliGetClass             (U032, U032, U032*);
BOOL                CliIsUniqueHandle       (U032, BOOL);
BOOL                CliGetPhysicalDevice    (U032, U032*);

#endif // _CLIENT_H_

