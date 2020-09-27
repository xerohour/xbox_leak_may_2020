#ifndef __mrouter_packet_h
#define __mrouter_packet_h

#include <assert.h>

typedef	struct	PACKET_BUFFER		PACKET_BUFFER;
typedef	struct	PACKET_LIST_ENTRY	PACKET_LIST_ENTRY;

typedef void (*PACKET_COMPLETION_FUNC) (PACKET_BUFFER *);

struct	PACKET_LIST_ENTRY
{
	LIST_ENTRY		ListEntry;
	PACKET_BUFFER *	PacketBuffer;
};

#define	PACKET_BUFFER_ADDRESS_MAX	16

struct	PACKET_BUFFER
{
	LPBYTE				Data;				// storage for packet data
	LPBYTE				Start;				// where the data begins, must be offset from Data
	DWORD				Length;				// how long the data is, relative to Start
	DWORD				Max;				// the maximum length of buffer, within Data
	union {
	DWORD				Context;			// for use by the originator of the packet
	PVOID				ContextPointer;		// for use by the originator, but safe for 64-bit
	};
	WORD				Protocol;			// see below
	WORD				AddressLength;
	BYTE				Address		[PACKET_BUFFER_ADDRESS_MAX];
	PACKET_COMPLETION_FUNC	CompletionFunc;
	LONG				ReferenceCount;		// use interlocked access, do not access directly
	LIST_ENTRY			ListEntry;			// for use only by the originator of the packet
};

#define	PACKET_BUFFER_PROTOCOL_IP			0x0000
#define	PACKET_BUFFER_PROTOCOL_VBI_RAW		0x0001
#define	PACKET_BUFFER_PROTOCOL_DSS_ARP		0x0002
#define	PACKET_BUFFER_PROTOCOL_DSS_RAW		0x0003
#define	PACKET_BUFFER_PROTOCOL_DSS_MPT		0x0004

// PACKET_BUFFER_PROTOCOL_IP
// packet body is a full IP packet, including header and body
// IP address should be extracted from the message body
// address field is not used

// PACKET_BUFFER_PROTOCOL_DSS_RAW
// the address field is not used
// the packet length must be 130 bytes.
// the packet is a standard DSS frame: 3 bytes of header
// and 127 bytes of payload

// PACKET_BUFFER_PROTOCOL_DSS_MPT
// the address field may or may not be specified
// the address field is the sub-SCID ID
// if the address is not specified, the output driver should build the
// sub-SCID like an Ethernet MAC address.  this only works for multicast packets,
// which have a fixed, deterministic IP address to MAC address mapping.
// if the address field exists (length is not zero), then the
// address length must be 6 and the address must contain an MPT sub-SCID ID.

static __inline void PacketBufferComplete (PACKET_BUFFER * packet)
{
	assert (packet);
	assert (packet -> CompletionFunc);

	(*packet -> CompletionFunc) (packet);
}

static __inline void PacketBufferAddRef (PACKET_BUFFER * packet) {
	assert (packet);
	InterlockedIncrement (&packet -> ReferenceCount);
}

static __inline void PacketBufferRelease (PACKET_BUFFER * packet) {
	assert (packet);
	if (!InterlockedDecrement (&packet -> ReferenceCount))
		PacketBufferComplete (packet);
}

#endif // __mrouter_packet_h