#ifndef _VPAGED_H_
#define _VPAGED_H_
/******** Operating System Interface for NVidia Hardware Simulator  ********\
*                                                                           *
* Module: VPAGED.H                                                          *
*       Header file for Virtual Paging device.                              *
*                                                                           *
*---------------------------------------------------------------------------*
*                                                                           *
* History:                                                                  *
*        David Schmenk (dschmenk)     05/08/95 - wrote it                   *
*                                                                           *
\*********************** Copyright 1994 NVidia, Inc. ***********************/

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------

#define PageSwap_Get_Version    0x0000
#define PageSwap_Test_Create    0x0001
#define PageSwap_Create         0x0002
#define PageSwap_Destroy        0x0003
#define PageSwap_In             0x0004
#define PageSwap_Out            0x0005
#define PageSwap_Test_IO_Valid  0x0006
#define PS_Zero_Init_Mask       0x0001
#define PS_Zero_Init_Bit        0x0000
#define PS_Fixed_Page_Mask      0x0008
#define PS_Fixed_Page_Bit       0x0003
#define PS_First_Page_In_Mask   0x0020
#define PS_First_Page_In_Bit    0x0005
#define PS_Dirty_Mask           0x0040
#define PS_Dirty_Bit            0x0006
#define PS_Ever_Dirty_Mask      0x8000
#define PS_Ever_Dirty_Bit       0x000F
#define PS_NoXchg_Mask          0x80000000
#define PS_NoXchg_Bit           0x001F     // DO NOT xchg phys addrs on PageSwap_Out
#define PS_No_Pager             0x01
#define PS_MSDOS_Pager          0x02
#define PS_HW_Pager             0x03
//
// Buffer descriptor for asynchronous I/O
//
#define SIZE_PS_BD_RESERVED     44
struct PageSwapBufferDesc
{
    DWORD PS_BD_Next;
    WORD  PS_BD_List;
    BYTE  PS_BD_Cmd;
    BYTE  PS_BD_Priority;
    DWORD PS_BD_Page_Number;
    DWORD PS_BD_Buffer_Ptr;
    DWORD PS_BD_File_Page;
    DWORD PS_BD_Call_Back;
    BYTE PS_BD_Reserved[SIZE_PS_BD_RESERVED];
};
//
// List values for PS_BD_List
//
#define PS_BDL_Free         1
#define PS_BDL_Pending_WB   2
#define PS_BDL_Submitted_RA 3
#define PS_BDL_Locked       4
#define PS_BDP_Invalid_Data -1
//
// VPAGED callback routines.
//
#pragma aux VPAGED_INT  modify [EAX EBX ECX EDX];
//
// Make functions to these services.
//
DWORD pagerGetVersion(VOID);
BYTE  pagerGetAccessType(VOID);

#pragma aux pagerGetVersion =                                   \
            CALL_VXD(PageSwap_Device_ID, PageSwap_Get_Version)  \
            value [EAX];

#pragma aux pagerGetAccessType =                                \
            CALL_VXD(PageSwap_Device_ID, PageSwap_Get_Version)  \
            value [BL];
#endif // _VPAGED_H_
