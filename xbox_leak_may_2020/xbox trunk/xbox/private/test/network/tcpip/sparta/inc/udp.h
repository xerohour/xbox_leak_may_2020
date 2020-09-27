

#ifndef __SPARTA_UDP_H__
#define __SPARTA_UDP_H__

#pragma pack(push,origpack)

#pragma pack(1)

#define	PROTOCOL_UDP	17	// UDP protocol number
#define	UDP_MAX_HEADER_LENGTH	8	// UDP max header lenght

typedef struct  _tagUDPT_Header
{
	USHORT		uh_src;				// Source port.
	USHORT		uh_dest;			// Destination port.
	USHORT		uh_length;			// Length
	USHORT		uh_xsum;			// Checksum.
} UDP_HDR_TYPE, *pUDP_HDR_TYPE; //UDPHeader 


#pragma pack(pop,origpack)


#endif // __SPARTA_UDP_H__


