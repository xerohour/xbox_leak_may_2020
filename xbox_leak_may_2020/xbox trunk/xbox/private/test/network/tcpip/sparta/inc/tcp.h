//* Definitions for header flags.
#ifndef __SPARTA_TCP_H__
#define __SPARTA_TCP_H__


#pragma pack(push,origpack)

#pragma pack(1)
#define	PROTOCOL_TCP	06	// TCP protocol number
#define	TCP_MAX_HEADER_LENGTH	20	// TCP max header lenght
#define  TCP_DEFAULT_WINDOWSIZE    17520


#define TCP_FIN    0x1
#define TCP_SYN    0x2
#define TCP_RST    0x4
#define TCP_PUSH   0x8
#define TCP_ACK    0x10
#define TCP_URG    0x20


#define TCP_OPTION_EOO   0
#define TCP_OPTION_NOOP  1
#define TCP_OPTION_MSS   2
#define TCP_OPTION_WS    3
#define TCP_OPTION_SACKP 4
#define TCP_OPTION_SACK  5
#define TCP_OPTION_TS    8
#define TCP_OPTION_RAW   99


#define MAX_NO_TCP_OPTIONS  40
#define MAX_TCP_HDR_SIZE    60



typedef struct _tagTCPT_Header {
    USHORT              tcp_src;        // Source port.
    USHORT              tcp_dest;       // Destination port.
    ULONG               tcp_seq;        // Sequence number.
    ULONG               tcp_ack;        // Ack number.
    USHORT              tcp_flags;      // Flags and data offset.
    USHORT              tcp_window;     // Window offered.
    USHORT              tcp_xsum;       // Checksum.
    USHORT              tcp_urgent;     // Urgent pointer.

} TCP_HDR_TYPE, *pTCP_HDR_TYPE;




typedef struct _tagTCPOpion_NoOp{
  UCHAR  Kind;
} TCPO_NoOp_TYPE, *pTCPO_NoOp_TYPE;



typedef struct _tagTCPOpion_MSS{
  UCHAR  Kind;
  UCHAR  Length;
  USHORT MSS;

} TCPO_MSS_TYPE, *pTCPO_MSS_TYPE;


typedef struct _tagTCPOpion_WS{
  UCHAR  Kind;
  UCHAR  Length;
  UCHAR  WS;

} TCPO_WS_TYPE, *pTCPO_WS_TYPE;



typedef struct _tagTCPOpion_TS{
  UCHAR  Kind;
  UCHAR  Length;
  ULONG  TSValue;
  ULONG  TSEcho;

} TCPO_TS_TYPE, *pTCPO_TS_TYPE;


typedef struct _tagTCPSACKP_TYPE{
  UCHAR  Kind;
  UCHAR  Length;
} TCPO_SACKP_TYPE, *pTCPO_SACKP_TYPE;


typedef struct _tagTCPSACK_TYPE{
  UCHAR  Kind;
  UCHAR  Length;
  ULONG  Block[10];
} TCPO_SACK_TYPE, *pTCPO_SACK_TYPE;

typedef struct _tagTCPRAW_TYPE{
  UCHAR  Kind;
  UCHAR  Length;
  UCHAR  Block[MAX_NO_TCP_OPTIONS];
} TCPO_RAW_TYPE, *pTCPO_RAW_TYPE;



#pragma pack(pop,origpack)

#endif
