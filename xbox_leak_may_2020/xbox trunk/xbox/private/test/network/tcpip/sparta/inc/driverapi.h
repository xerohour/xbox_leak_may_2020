/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: driverapi.h                                                                 *
* Description: this code provides an user mode API to the tardis/sparta packet driver   *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/10/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_DRIVERAPI_H__
#define __SPARTA_DRIVERAPI_H__

#include "sparta.h"
#include "media.h"
#include "adapterinfo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define     MAXIMUM_POSSIBLE_MEDIA_ADDRESS_LENGTH   12 // picked arbitrary large number
#define     MAXIMUM_POSSIBLE_NUMBER_OF_ADAPTERS     100
#define     MAXIMUM_POSSIBLE_PACKET_LENGTH          4096 // arbitrary large number

typedef struct
{
   HANDLE         hAdapterHandle;
   MAC_MEDIA_TYPE MediaType;
   MAC_ADDRESS    MacAddress;
//    PADAPTER_INFO pAdapterInfo;
} tNI_INTERFACE, *NI_INTERFACE;

typedef struct
{
   DWORD dwOffset;
   DWORD dwLength;
   PVOID pvPattern;
} tNI_PATTERN, *NI_PATTERN;

#ifdef   INCLUDE_UNUSED
typedef struct
{
   NI_INTERFACE   Interface;
   DWORD          dwSelectStatus;
} tNI_SELECT_STRUCTURE, *NI_SELECT_STRUCTURE;
#endif


NI_INTERFACE NI_OpenInterfaceByName(const TCHAR *szInterfaceDescriptor);
// currently the name of the interface is the same as its media address

NI_INTERFACE NI_OpenInterfaceByIndex(DWORD dwInterfaceIndex);
// this index corresponds to the index in the adapter enumeration

SPARTA_STATUS NI_EnumerateAdapters(OUT PADAPTER_INFO pAdapterInfo, IN OUT PDWORD pcbAdapterInfo);

SPARTA_STATUS NI_CloseInterface(NI_INTERFACE Interface);

MAC_MEDIA_TYPE NI_GetMediaType(NI_INTERFACE Interface);

#ifdef   INCLUDE_UNUSED
MAC_ADDRESS NI_GetMediaAddress(NI_INTERFACE Interface);
// returns the media address of the NIC
#endif

NI_PATTERN NI_CreatePattern(DWORD dwOffset, DWORD dwLength, PVOID pvBuffer);
SPARTA_STATUS NI_DeletePattern(NI_PATTERN);

SPARTA_STATUS NI_AddPatternFilter(NI_INTERFACE Interface, NI_PATTERN Pattern);
SPARTA_STATUS NI_RemovePatternFilter(NI_INTERFACE Interface, NI_PATTERN Pattern);

SPARTA_STATUS NI_EnablePatternFiltering(NI_INTERFACE Interface);
SPARTA_STATUS NI_DisablePatternFiltering(NI_INTERFACE Interface);

//
// Warning: be very careful when you put the adapter into promiscuous mode
// the filters will be very busy and you put additional load on the CPU since
// the netork card will not do the address filtering for the driver
//
// receive modes are based on the NDIS modes. Use the following constants (can be or'd together):
// NDIS_PACKET_TYPE_PROMISCUOUS, NDIS_PACKET_TYPE_DIRECTED,
// NDIS_PACKET_TYPE_BROADCAST, NDIS_PACKET_TYPE_MULTICAST
//
// NOTE: DUE TO AN ARCHITECTUAL ISSUE, THE INTERFACE MODE WILL CURRENTLY APPLY TO *ALL*
// OPEN INTERFACES, SO TAKE CARE IN MANIPULATING THE INTERFACE MODE. IF YOU NEED TO MAKE
// THE RECEIVE MODE RESTRICTIVE, YOU MIGHT CONSIDER USING FILTERS

SPARTA_STATUS NI_SetInterfaceMode(NI_INTERFACE Interface, DWORD dwMode);

#ifdef   INCLUDE_UNUSED
SPARTA_STATUS NI_AddAddressFilter(NI_INTERFACE Interface, MAC_ADDRESS MediaAddress);

SPARTA_STATUS NI_AddMulticastAddress(NI_INTERFACE Interface, MAC_ADDRESS MediaAddress);

SPARTA_STATUS NI_RemoveMulticastAddress(NI_INTERFACE Interface, MAC_ADDRESS MediaAddress);
#endif

// The "raw" variations take a buffer rather than a MediaAddress struct

SPARTA_STATUS NI_AddMulticastAddressRaw(NI_INTERFACE Interface, WORD wLength, PBYTE Buffer);

SPARTA_STATUS NI_RemoveMulticastAddressRaw(NI_INTERFACE Interface, WORD wLength, PBYTE Buffer);

SPARTA_STATUS NI_StartListening(NI_INTERFACE Interface);
SPARTA_STATUS NI_StopListening(NI_INTERFACE Interface);

// The start and stop listening is added for convenience: when manipulating the 
// pattern filters, receive modes and multicast addresses, it is useful to stop
// the driver from receiving packets temporarily (so that the filter manipulation 
// can act as an atomic operation). After an interface is opened, it will be in
// "not listening" mode, so call start listening after setting up your filters etc.. 

#ifdef   INCLUDE_UNUSED
SPARTA_STATUS NI_FlushQueue(NI_INTERFACE Interface);
#endif


// The SPARTA driver supports asynchronous I/O so all that needs to be changed 
// are the NI_Send and NI_Receive calls to be async, maybe add functions to
// such as NI_AsyncReceive and NI_AsyncSend

SPARTA_STATUS NI_Send(NI_INTERFACE Interface,PVOID buffer, DWORD length);
// blocking call to send the data

#ifdef   INCLUDE_UNUSED
SPARTA_STATUS NI_Select(DWORD dwTimeout, NI_SELECT_STRUCTURE, ...);
#endif

SPARTA_STATUS NI_Receive(NI_INTERFACE Interface, PVOID pvBuffer, DWORD dwLength, 
                            DWORD *pdwBytesRead, HANDLE hTimerEventHandle, DWORD dwTimeout);
// blocking call to receive data, will unblock if timerevent fires or the specified timeout
// expires. in both cases, ERROR_TIMEOUT will be returned
// timerobject handle may be NULL (i.e. it's optional)
// dwTimeout is optional (0 is interpreted as an infinite timeout)


// UTILITY FUNCTION FOR DEBUGGING - sets the debugging mask for the kernel mode driver
// output goes to the kernel debugger if present
// see dbgmsg.h for mask types

DWORD NI_SetDriverDebugMask(IN ULONG ulDebugMask);


#ifdef __cplusplus
} // for the extern "C" declaration
#endif

#endif // __SPARTA_DRIVERAPI_H__

