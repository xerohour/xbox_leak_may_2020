/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: Packet.hpp                                                                  *
* Description: this defines the Packet class which acts as an abstraction of a buffer   *
*              collection                                                               *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/21/2000    created                              *
*                       jbekmann      5/31/2000    made destructor virtual              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_PACKET_H__
#define __SPARTA_PACKET_H__

#include "sparta.h"
#include "packets.h"

// this is for checking the packet types

#define PACKET_TYPE_MEDIA        BUFFER_TYPE_MEDIA       
#define PACKET_TYPE_ARP          BUFFER_TYPE_ARP         
#define PACKET_TYPE_IP4          BUFFER_TYPE_IP4         
#define PACKET_TYPE_IP6          BUFFER_TYPE_IP6         
#define PACKET_TYPE_UDP          BUFFER_TYPE_UDP         
#define PACKET_TYPE_ICMP         BUFFER_TYPE_ICMP        
#define PACKET_TYPE_IGMPV1       BUFFER_TYPE_IGMPV1
#define PACKET_TYPE_IGMPV2       BUFFER_TYPE_IGMPV2
#define PACKET_TYPE_IGMPV3       BUFFER_TYPE_IGMPV3
#define PACKET_TYPE_TCP          BUFFER_TYPE_TCP         
#define PACKET_TYPE_USERDATA     BUFFER_TYPE_USERDATA

class CPacket
{
    
protected:
    PKT_PACKET          m_Packet;
    PKT_BUFFER          m_UserBuffer; // the Packet's payload (excludes all Headers)

/*
THIS CODE IS NO LONGER USED:

Reason: we maintain a buffer on a per-interface basis, rather on a per-packet basis since
the cost of allocating and freeing buffers on a per-packet basis is too much overhead.
The "dirty" code is thus optimized to be repeated sends of the same packet on the 
interface.

    PBYTE               m_pbLastSendBuffer;
    DWORD               m_dwLastSendBufferLength;
    CRITICAL_SECTION    m_SendBufferCriticalSection;


    LONG                m_lPacketIsDirty;

    VOID MarkPacketDirty();    // call this method whenever something touches the 
                            // data buffer. This is used to optimize for repeated
                            // sends
*/

public:
    virtual int GetPacketType();

    CPacket();
    CPacket(PKT_PACKET Packet);
    virtual ~CPacket();  // frees all resources used by this packet, note that the user buffer
                // needs to be explicitly freed by the user

    DWORD GetLength(); // returns length of physical buffer if packet were to be sent

    virtual DWORD GetMinimumPacketLength(); // override for subsequent packet types
                                            // primarily used for CMacPacket layer

    virtual DWORD GetMaximumPacketLength(); // override for subsequent packet types

    SPARTA_STATUS SetUserBuffer(PVOID pvBuffer, DWORD dwDataLength, BOOL lPerformCopy);
        // sets the user buffer which is the packet's "payload"
        // if lPerformCopy is TRUE, the buffer will be copied and freed when the packet is deleted.
        // otherwise it's the user's responsibility to free it after use.

    SPARTA_STATUS GetUserBuffer(PVOID *ppvBuffer, PDWORD pdwDataLength);
        // returns a pointer to the user buffer as well as the length


    PKT_PACKET GetPacketPointer(); // returns the pointer to the packet structure
                                     // for use by advanced users using the PKT_XXXX API

    SPARTA_STATUS BuildRawBuffer(PBYTE pbDestBuffer,
                                DWORD dwBufferLength,
                                DWORD *pdwBytesCopied);

    // transfers the packet data into a contiguous block of data.
    // calls the send handlers for all buffers prior to copying for advanced users.
    // it is not really recommended for normal use.


    virtual SPARTA_STATUS PreparePacketForSend();

    // This method is called when BuildRawBuffer is called. Typically this is
    // called by the Send method. Override this method to do any processing you
    // need prior to building the raw buffer. Note that this is a higher level
    // handler than the BufferSendHandler at the PKT_XXX API which only handles
    // PKT_BUFFER structures.

    virtual void PrintPacket();
    void PrintUserBuffer();
};

#endif // __SPARTA_PACKET_H__

