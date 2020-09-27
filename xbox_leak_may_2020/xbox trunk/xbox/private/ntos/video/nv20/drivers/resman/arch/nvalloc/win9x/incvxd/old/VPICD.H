#ifndef _VPICD_H_
#define _VPICD_H_
/******** Operating System Interface for NVidia Hardware Simulator  ********\
*                                                                           *
* Module: VPICD.H                                                           *
*       Header file for Virtual PIC device.                                 *
*                                                                           *
*---------------------------------------------------------------------------*
*                                                                           *
* History:                                                                  *
*        David Schmenk (dschmenk)     10/12/94 - wrote it                   *
*                                                                           *
\*********************** Copyright 1994 NVidia, Inc. ***********************/

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------

//
// Output of picGetStatus()
//
#define VPICD_Stat_IRET_Pending     0x01
#define VPICD_Stat_IRET_Pending_Bit 0
#define VPICD_Stat_In_Service	    0x02
#define VPICD_Stat_In_Service_Bit   1
#define VPICD_Stat_Phys_Mask	    0x04
#define VPICD_Stat_Phys_Mask_Bit    2
#define VPICD_Stat_Phys_In_Serv     0x08
#define VPICD_Stat_Phys_In_Serv_Bit 3
#define VPICD_Stat_Virt_Mask	    0x10
#define VPICD_Stat_Virt_Mask_Bit    4
#define VPICD_Stat_Virt_Req         0x20
#define VPICD_Stat_Virt_Req_Bit     5
#define VPICD_Stat_Phys_Req	        0x40
#define VPICD_Stat_Phys_Req_Bit     6
#define VPICD_Stat_Virt_Dev_Req     0x80
#define VPICD_Stat_Virt_Dev_Req_Bit 7
//
// Options in IRQ Descriptor
//
#define VPICD_Opt_Read_Hw_IRR       0x01
#define VPICD_Opt_Read_Hw_IRR_Bit   0
#define VPICD_Opt_Can_Share         0x02
#define VPICD_Opt_Can_Share_Bit     1
//
// IRQ descriptor.
//
struct VPICD_IRQ_Descriptor
{
    WORD  VID_IRQ_Number;
    WORD  VID_Options;
    DWORD VID_Hw_Int_Proc;
    DWORD VID_Virt_Int_Proc;
    DWORD VID_EOI_Proc;
    DWORD VID_Mask_Change_Proc;
    DWORD VID_IRET_Proc;
    DWORD VID_IRET_Time_Out;
};
//
// VPICD service defines.
//
#define VPICD_Get_Version               0x0000
#define VPICD_Virtualize_IRQ            0x0001
#define VPICD_Set_Int_Request           0x0002
#define VPICD_Clear_Int_Request         0x0003
#define VPICD_Phys_EOI                  0x0004
#define VPICD_Get_Complete_Status       0x0005
#define VPICD_Get_Status                0x0006
#define VPICD_Test_Phys_Request         0x0007
#define VPICD_Physically_Mask           0x0008
#define VPICD_Physically_Unmask         0x0009
#define VPICD_Set_Auto_Masking          0x000A
#define VPICD_Get_IRQ_Complete_Status   0x000B
#define VPICD_Convert_Handle_To_IRQ     0x000C
#define VPICD_Convert_IRQ_To_Int        0x000D
#define VPICD_Convert_Int_To_IRQ        0x000E
#define VPICD_Call_When_Hw_Int          0x000F
#define VPICD_Force_Default_Owner       0x0010
#define VPICD_Force_Default_Behavior    0x0011
#define VPICD_Auto_Mask_At_Inst_Swap    0x0012
#define VPICD_Begin_Inst_Page_Swap      0x0013
#define VPICD_End_Inst_Page_Swap        0x0014
//
// VID callback routines.
//
#pragma aux VPICD_INT   parm [EAX] [EBX];
//
// Make functions to these services.
//
WORD  picGetVersion(VOID);
DWORD picVirtualizeIRQ(struct VPICD_IRQ_Descriptor *);
VOID  picSetIntRequest(DWORD, DWORD);
VOID  picClearIntRequest(DWORD, DWORD);
VOID  picPhysEOI(DWORD);
DWORD picGetCompleteStatus(DWORD, DWORD);
DWORD picGetStatus(DWORD, DWORD);
DWORD picTestPhysRequest(DWORD);
VOID  picPhysicallyMask(DWORD);
VOID  picPhysicallyUnmask(DWORD);
VOID  picSetAutoMasking(DWORD);
DWORD picGetIRQCompleteStatus(DWORD, DWORD);
DWORD picGetIRQVirtualizedStatus(DWORD, DWORD);
DWORD picConvertHandleToIRQ(DWORD);
DWORD picConvertIRQToInt(DWORD, DWORD);
DWORD picConvertntToIRQ(DWORD);
DWORD picCallWhenHwInt(DWORD);
DWORD picForceDefaultOwner(DWORD, DWORD);
VOID  picForceDefaultBehavior(DWORD);

#pragma aux picGetVersion =                                 \
            CALL_VXD(VPICD_Device_ID, VPICD_Get_Version)    \
            value [AX];

#pragma aux picVirtualizeIRQ =                              \
            CALL_VXD(VPICD_Device_ID, VPICD_Virtualize_IRQ) \
            parm [EDI]                                      \
            value [EAX];

#pragma aux picSetIntRequest =                              \
            CALL_VXD(VPICD_Device_ID, VPICD_Set_Int_Request)\
            parm [EAX] [EBX];

#pragma aux picClearIntRequest =                            \
            CALL_VXD(VPICD_Device_ID, VPICD_Clear_Int_Request)\
            parm [EAX] [EBX];

#pragma aux picPhysEOI =                                    \
            CALL_VXD(VPICD_Device_ID, VPICD_Phys_EOI)       \
            parm [EAX];

#pragma aux picGetCompleteStatus =                          \
            CALL_VXD(VPICD_Device_ID, VPICD_Get_Complete_Status)\
            parm [EAX] [EBX]                                \
            value [ECX];

#pragma aux picGetStatus =                                  \
            CALL_VXD(VPICD_Device_ID, VPICD_Get_Status)     \
            parm [EAX] [EBX]                                \
            value [ECX];

#pragma aux picTestPhysRequest =                            \
            CALL_VXD(VPICD_Device_ID, VPICD_Test_Phys_Request)\
            parm [EAX]                                      \
            value [EAX];

#pragma aux picPhysicallyMask =                             \
            CALL_VXD(VPICD_Device_ID, VPICD_Physically_Mask)\
            parm [EAX];

#pragma aux picPhysicallyUnmask =                           \
            CALL_VXD(VPICD_Device_ID, VPICD_Physically_Unmask)\
            parm [EAX];

#pragma aux picSetAutoMasking =                             \
            CALL_VXD(VPICD_Device_ID, VPICD_Set_Auto_Masking)\
            parm [EAX];

#pragma aux picGetIRQCompleteStatus =                       \
            CALL_VXD(VPICD_Device_ID, VPICD_Get_IRQ_Complete_Status)\
            parm [EAX] [EBX]                                 \
            value [ECX];

#pragma aux picGetIRQVirtualizedStatus =                       \
            CALL_VXD(VPICD_Device_ID, VPICD_Get_IRQ_Complete_Status)\
            "sbb    EAX, EAX"                               \
            parm [EAX] [EBX]                                 \
            value [EAX];

#pragma aux picConvertHandleToIRQ =                         \
            CALL_VXD(VPICD_Device_ID, VPICD_Convert_Handle_To_IRQ)\
            parm [EAX]                                      \
            value [ESI];

#pragma aux picConvertIRQToInt =                            \
            CALL_VXD(VPICD_Device_ID, VPICD_Convert_IRQ_To_Int)\
            parm [EAX] [EBX]                                \
            value [EAX];
            
#pragma aux picConvertIntToIRQ =                            \
            CALL_VXD(VPICD_Device_ID, VPICD_Convert_Int_To_IRQ)\
            parm [EAX]                                      \
            value [EAX];

#pragma aux picCallWhenHwInt =                              \
            CALL_VXD(VPICD_Device_ID, VPICD_Call_When_Hw_Int)\
            parm [ESI]                                      \
            value [ESI];

#pragma aux picForceDefaultOwner =                          \
            CALL_VXD(VPICD_Device_ID, VPICD_Force_Default_Owner)\
            parm [EAX] [EBX]                                \
            value [EAX];

#pragma aux picForceDefaultBehavior =                      \
            CALL_VXD(VPICD_Device_ID, VPICD_Force_Default_Behavior)\
            parm [EAX];

#endif // _VPICD_H_
