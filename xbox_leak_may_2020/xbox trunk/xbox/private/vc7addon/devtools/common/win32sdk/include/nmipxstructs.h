//=============================================================================
//  Microsoft (R) Network Monitor (tm). 
//  Copyright (C) 1992-1999. All rights reserved.
//
//  MODULE: nmipxstructs.h
//
//  IPX structures & #defines
//=============================================================================

#ifndef NMIPXSTRUCTS_H
#define NMIPXSTRUCTS_H

#ifdef __cplusplus
extern "C"
{
#endif

//  IPX
typedef struct {
   UCHAR ha_address[6];
} HOST_ADDRESS;

typedef struct {
   ULONG  netnumber;
   HOST_ADDRESS hostaddr;
   USHORT socket;
} NET_ADDRESS;
typedef NET_ADDRESS UNALIGNED * ULPNETADDRESS;

typedef struct {
   USHORT ipx_checksum;
   USHORT ipx_length;
   UCHAR  ipx_xport_control;
   UCHAR  ipx_packet_type;
   NET_ADDRESS idp_dest;
   NET_ADDRESS idp_source;
} IPX_HDR;
typedef IPX_HDR UNALIGNED * ULPIPX_HDR;

//  SPX
typedef struct _SPX_HDR {       // Sequenced Packet Protocol
    IPX_HDR spx_idp_hdr;
    UCHAR   spx_conn_ctrl;      // bits 0-3 defined (SPX_CTRL_xxx)
    UCHAR   spx_data_type;      // 0 (defined to be used by higher layers)
    USHORT  spx_src_conn_id;    // b.e.
    USHORT  spx_dest_conn_id;   // b.e.
    USHORT  spx_sequence_num;   // sequence number (b.e.).
    USHORT  spx_ack_num;        // acknowledge number (b.e.)
    USHORT  spx_alloc_num;      // allocation (b.e.)
} SPX_HDR;
typedef SPX_HDR UNALIGNED *PSPX_HDR;

#ifdef __cplusplus
}
#endif

#endif