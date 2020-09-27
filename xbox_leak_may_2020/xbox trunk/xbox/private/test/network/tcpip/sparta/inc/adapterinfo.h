/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: adapterinfo.h                                                               *
* Description: defines the structures used by the driver and user mode code to share    *
*              information about bound adapters.                                        *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/16/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __ADAPTERINFO_H__
#define __ADAPTERINFO_H__


#define TARDIS_802_ADDR_LENGTH 6
#define TARDIS_ADDR_LENGTH 6

typedef struct _ADAPTER
{
    ULONG AdapterIndex;
    UCHAR MacAddress[TARDIS_ADDR_LENGTH];
} ADAPTER, *PADAPTER;                                                       

typedef struct _ADAPTER_INFO
{
    ULONG   cAdapters;
    ULONG   cbAddressLength;
    ADAPTER Adapters[1];
} ADAPTER_INFO, *PADAPTER_INFO;


#endif __ADAPTERINFO_H__

