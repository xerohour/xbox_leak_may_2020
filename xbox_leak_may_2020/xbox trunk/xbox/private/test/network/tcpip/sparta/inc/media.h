/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: media.h                                                                     *
* Description: typedefs for the media type and address structure                        *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/20/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_MEDIA_H__
#define __SPARTA_MEDIA_H__

#include "sparta.h"

typedef enum
{
    UNKNOWN = 0,
    ETHERNET_802_3,
    TOKENRING,
    ATM
} MAC_MEDIA_TYPE;


typedef struct
{
    WORD wAddressLength;
	BYTE *bpAddress;
    MAC_MEDIA_TYPE MediaType;
} tMAC_ADDRESS, *MAC_ADDRESS;


#endif // __SPARTA_MEDIA_H__


