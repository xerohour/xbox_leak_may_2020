#ifndef _VNV0D_H_
#define _VNV0D_H_
/******** Operating System Interface for NVidia Hardware Simulator  ********\
*                                                                           *
* Module: NVRM.H                                                            *
*       Header file for NV0 defines and service exports.                    *
*                                                                           *
*---------------------------------------------------------------------------*
*                                                                           *
* History:                                                                  *
*        David Schmenk (dschmenk)     06/21/94 - wrote it                   *
*                                                                           *
\*********************** Copyright 1994 NVidia, Inc. ***********************/

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------

//
// Version numbers.
//
#define NV0_Major_Version  1
#define NV0_Minor_Version  0
//
// Device IDs.
//
#define NV0_Device_ID      0x32DB
//
// Init order.
//
#define NV0_Init_Order     0x70000000
//
// Virtual device services.
//
#define NV0_Get_Version        0
#define NV0_Install_Handlers   1

//---------------------------------------------------------------------------
//
//  Types.
//
//---------------------------------------------------------------------------

//
// Shared communication buffer structure.
//
typedef DWORD CDECL (*RDPROC)(DWORD, DWORD);
typedef DWORD CDECL (*WRPROC)(DWORD, DWORD, DWORD);
typedef VOID  CDECL (*CTPROC)(VOID);
typedef struct
{
    RDPROC memRead;
    WRPROC memWrite;
    RDPROC ioRead;
    WRPROC ioWrite;
    RDPROC cfgRead;
    WRPROC cfgWrite;
    CTPROC Interrupt;
    DWORD  physBase;
    DWORD  linBase;
    DWORD  ufifoBase;
    DWORD  ufifoCount;
} NV0PROCS, *PNV0PROCS;

//---------------------------------------------------------------------------
//
//  Prototypes to call NV0 services.
//
//---------------------------------------------------------------------------

DWORD nvsimGetVersion(VOID);
LONG  nvsimInstallHandlers(DWORD, PNV0PROCS);
#pragma aux nvsimGetVersion =                           \
            CALL_VXD(NV0_Device_ID, NV0_Get_Version)    \
            parm caller []                              \
            value no8087 [EAX]                          \
            modify [EAX EBX ECX EDX];
#pragma aux nvsimInstallHandlers =                          \
            CALL_VXD(NV0_Device_ID, NV0_Install_Handlers)   \
            parm caller []                                  \
            value no8087 [EAX]                              \
            modify [EAX EBX ECX EDX];
#endif // _VNV0D_H_
