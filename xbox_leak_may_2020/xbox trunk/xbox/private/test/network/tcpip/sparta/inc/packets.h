/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: packets.h                                                                   *
* Description: buffer management routines for packet manipulation                       *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      1/24/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_PACKETS_H__
#define __SPARTA_PACKETS_H__


#ifdef __cplusplus // this file is also included by C++ files
extern "C"
{
#endif 



#define BUFFER_TYPE_MEDIA         0x01000000
#define BUFFER_TYPE_ARP           0x02000000
#define BUFFER_TYPE_IP4           0x03000000
#define BUFFER_TYPE_IP6           0x04000000
#define BUFFER_TYPE_UDP           0x05000000
#define BUFFER_TYPE_ICMP          0x06000000
#define BUFFER_TYPE_TCP           0x07000000
#define BUFFER_TYPE_TCP_OPTION    0x08000000
#define BUFFER_TYPE_USERDATA      0x09000000
#define BUFFER_TYPE_IGMPV1        0x0A000000
#define BUFFER_TYPE_IGMPV2        0x0B000000
#define BUFFER_TYPE_IGMPV3        0x0c000000
#define BUFFER_TYPE_MASK_OFF_MSB  0x00ffffff


#define net_short(x) ((((x)&0xff) << 8) | (((x)&0xff00) >> 8))


#define PHXSUM(s,d,p,l) (UINT)( (UINT)*(USHORT *)&(s) + \
                        (UINT)*(USHORT *)((UCHAR *)&(s) + sizeof(USHORT)) + \
                        (UINT)*(USHORT *)&(d) + \
                        (UINT)*(USHORT *)((UCHAR *)&(d) + sizeof(USHORT)) + \
                        (UINT)((USHORT)net_short((p))) + \
                        (UINT)((USHORT)net_short((USHORT)(l))) )

   


   

#ifdef   UNUSED

typedef struct 
{
   DWORD dwRefCount;    // buffers referring to this pool
   PVOID pvMemory;      // pool pointer
   DWORD dwLength;      // actual length
   BOOL  lAutoFree;     // whether this buffer will be freed automatically
} tPKT_BUFFER_POOL,*PKT_BUFFER_POOL;



typedef SPARTA_STATUS (*PKT_SEND_HANDLER)(struct _tagPKT_BUFFER *);
// the send handler is called for each header in a packet
// when BuildSendData is called. If the function returns anything
// other than ERROR_SUCCESS the call to BuildSendData will fail
// with the error code given.

#endif


struct _tagPKT_PACKET;  // forward declaration

typedef struct _tagPKT_BUFFER 
{
   DWORD                   dwAllocLength;
   DWORD                   dwActualLength;
   PVOID                   pvData;
   struct _tagPKT_BUFFER   *pNext;
   struct _tagPKT_BUFFER   *pPrev;
   
   DWORD                   dwBufferType;
   struct _tagPKT_PACKET   *pPacket;          
   // pointer to the packet (if assigned) that this buffer belongs to
#ifdef   INCLUDE_UNUSED        
   PKT_BUFFER_POOL AllocPool; 
   // if NULL, then pvData is the actual allocation received from the Memory Manager
   // if it's not NULL, then this is the pool that this buffer was allocated from.
#endif
   BOOL                    lAutoFree;    
   // the memory is automatically freed.
   // note that if there is a buffer pool, then the memory in the pool can only be freed with a call
   // to freebufferpool.
#ifdef   INCLUDE_UNUSED
   PKT_SEND_HANDLER        pfnSendHandler; // access to a single buffer only
   PVOID                   pvUserContext;
#endif
} tPKT_BUFFER, *PKT_BUFFER;


typedef struct _tagPKT_PACKET 
{
   WORD              wBufferCount; // number of buffers in this packet
   PKT_BUFFER        pHead;
   PKT_BUFFER        pTail;
   CRITICAL_SECTION  csPacketCritialSection;
} tPKT_PACKET, *PKT_PACKET;


//
// NOTE: it is recommended that BufferPools not be used unless you know exactly what they do.
//

#ifdef   INCLUDE_UNUSED
PKT_BUFFER_POOL  PKT_InitializeBufferPool(PVOID pvMemory, DWORD dwLength, BOOL lAutoFree);
// initializes buffer pool structure
// if lAutoFree is TRUE, the pool will
// automatically be freed when
// all associated buffers are freed
#endif

PKT_PACKET PKT_AllocatePacket();                                                        
// allocates a Packet Structure
SPARTA_STATUS PKT_AttachBuffer(PKT_PACKET Packet, PKT_BUFFER NewBuffer);
// Attaches a buffer to the end of the buffer list
SPARTA_STATUS PKT_InsertAfterBuffer(PKT_PACKET Packet, PKT_BUFFER PreviousBuffer, PKT_BUFFER NewBuffer);
// inserts a buffer AFTER the given buffer
// if given buffer is NULL, will become
// new head of list
SPARTA_STATUS PKT_UnchainBuffer(PKT_BUFFER Buffer); 
// unchains the specified buffer from it's packet

DWORD PKT_GetPacketLength(PKT_PACKET Packet); 
// returns length of physical buffer (uses dwActualLength of the buffers)

#ifdef   INCLUDE_UNUSED
DWORD PKT_GetBufferCount(PKT_PACKET Packet);  
// returns number of buffers in the packet
#endif

SPARTA_STATUS PKT_BuildSendData(PKT_PACKET Packet,PBYTE pbDestBuffer,DWORD dwBufferLength, DWORD *pdwBytesCopied);
// transfers the packet data into a contiguous block of data
// calls the send handlers for all
// packets prior to copying

SPARTA_STATUS PKT_UnchainAndFreeAllPacketBuffers(PKT_PACKET Packet);
// explicitly frees all the buffers in the packet list
SPARTA_STATUS PKT_FreePacket(PKT_PACKET Packet);
// frees the packet - only possible if the packet list
// is empty  - see UnchainAndFreeAllPacketBuffers

PKT_BUFFER PKT_AllocateBuffer(DWORD dwInitialLength, DWORD dwMaximumLength);
// allocate a buffer with the Maximum length.
// the ActualLength is set to InitialLength
// allocated buffers are freed automatically

PKT_BUFFER PKT_MapBuffer(PVOID pvExistingBuffer, DWORD dwCurrentLength, DWORD dwMaxBufferLength, BOOL lAutoFree);
// create a buffer structure
// and initialize the data area with a pointer to
// the existing buffer.
// Use instead of AllocateBuffer if you already have
// an allocated buffer.
// MaxBufferLength is the total memory allocated
// CurrentLength is the length to set the current
// buffer to.
// It is the caller's responsibility to free memory
// pointed to by a mapped buffer if lAutoFree is FALSE

#ifdef   INCLUDE_UNUSED
PKT_BUFFER PKT_MapPooledBuffer(PKT_BUFFER_POOL BufferPool,
                                     PVOID pvExistingBuffer,
                                     DWORD dwCurrentLength,
                                     DWORD dwMaxBufferLength);
#endif


SPARTA_STATUS PKT_SetBufferType(PKT_BUFFER Buffer, DWORD dwBufferType);
// sets the buffer type

PKT_PACKET PKT_GetBuffersPacket(PKT_BUFFER Buffer); 
// returns the packet which this buffer is part of

DWORD PKT_GetBufferType(PKT_BUFFER Buffer); 
// returns the buffer type
// the Buffer type is opague to the packet API or the driver. It was added for easier
// parsing and identification on the users part. Use as desired.

#ifdef   INCLUDE_UNUSED
PVOID PKT_GetUserContext(PKT_BUFFER Buffer); 
// sets the user context
SPARTA_STATUS PKT_SetUserContext(PKT_BUFFER Buffer, PVOID pvUserContext); 
// returns the user context
#endif

PVOID PKT_GetBufferData(PKT_BUFFER Buffer); 
// returns a pointer to the buffer's data

#ifdef   INCLUDE_UNUSED
SPARTA_STATUS PKT_SetBufferSendHandler(PKT_BUFFER Buffer, PKT_SEND_HANDLER pfnSendHandler);
// sets the send handler for this buffer
// see notes above

SPARTA_STATUS PKT_ClearBufferSendHandler(PKT_BUFFER Buffer);
// clears the send handler
#endif

DWORD PKT_GetBufferLength(PKT_BUFFER Buffer);                   
// get Buffer length
DWORD PKT_GetBufferMaxLength(PKT_BUFFER Buffer); 
// returns length of allocated buffer (uses dwAllocLength)

SPARTA_STATUS PKT_SetBufferLength(PKT_BUFFER Buffer, DWORD dwNewLength);
// set Buffer length
// does NOT grow buffer

SPARTA_STATUS PKT_FreeBuffer(PKT_BUFFER Buffer);
// frees the given buffer
// NOTE: if the buffer was mapped then the
// data pointer will not be freed !!

USHORT xsum( UINT Phxsum,PVOID Buffer,INT Size);


#ifdef __cplusplus // this file is also included by C++ files, we declared it as extern "C"
}
#endif 

#endif // __SPARTA_PACKETS_H__


