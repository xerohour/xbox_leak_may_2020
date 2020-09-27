#ifndef __SPARTA_ARP_H__
#define __SPARTA_ARP_H__



#pragma pack(push,origpack)

#pragma pack(1)

#define ETHERNET_ADDRESS_LENGTH  6

#define ARP_REQUEST 1
#define ARP_REPLY 2

typedef struct
{
    USHORT HardType;
    USHORT ProtType;
    UCHAR  HardSize;
    UCHAR  ProtSize;
    USHORT OpCode;
    UCHAR  SrcEth[6];
    unsigned long  SrcIP;
    UCHAR  DstEth[6];
    unsigned long  DstIP;
} ARP_PKT_TYPE, *pARP_PKT_TYPE;

#pragma pack(pop,origpack)


#endif // __SPARTA_ARP_H__

