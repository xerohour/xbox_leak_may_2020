/*--
Copyright (c) Microsoft Corporation

Module Name:

    avp.h

Abstract:

    This module contains the private data structures and procedure
    prototypes to identify AV packs and program TV encoder

--*/

#ifndef _AVP_H
#define _AVP_H

#include "ntos.h"
#include <av.h>
#include <smcdef.h>
#include <xtl.h>
#include "xboxp.h"
#include "xconfig.h"

#include "nv_ref_2a.h"
#include "nv_ref_plus.h"

#define DEFINE_REGISTERS

#include <avmode.h>

//
// I2C constants
//

#define I2C_PORT_PRIMARY            0
#define I2C_PORT_SECONDARY          1

#define I2C_SRCK                    0x20    // Serial Clock write
#define I2C_SRD                     0x10    // Serial Data  write
#define I2C_SRCK_IN                 0x04    // Serial Clock read
#define I2C_SRD_IN                  0x08    // Serial Data  read
#define I2C_ENABLE                  0x01    // Enable Serial Port Function

VOID
AvpReadDeviceReg(
    PVOID RegisterBase,
    UCHAR SMBaddr,
    UCHAR Reg,
    UCHAR* ReadData
    );

UCHAR
AvpWriteDeviceReg(
    PVOID RegisterBase,
    UCHAR SMBaddr,
    UCHAR Reg,
    UCHAR WriteValue
    );

VOID
AvpDelay(
    PVOID RegisterBase,
    ULONG nanosec
    );

#if DBG

extern ULONG AvpDump;

#endif 

//
// Macros/functions to access hardware registers
//

FORCEINLINE ULONG
REG_RD32(VOID* Ptr, ULONG Addr)
{
    return *((volatile ULONG*)((UCHAR*)(Ptr) + (Addr)));
}

FORCEINLINE VOID
REG_WR32(VOID* Ptr, ULONG Addr, ULONG Val)
{
    *((volatile ULONG*)((UCHAR*)(Ptr) + (Addr))) = (ULONG)(Val);

#if DBG
    if (AvpDump)
    {
        DbgPrint("%08X = %08X\n", Addr, Val);
    }
#endif
}

FORCEINLINE VOID
REG_OR32(VOID* Ptr, ULONG Addr, ULONG Val)
{
    ULONG Value;
    
#if DBG
    ULONG Store = AvpDump;
    AvpDump = FALSE;
#endif

    Value = REG_RD32(Ptr, Addr);
    REG_WR32(Ptr, Addr, Value | Val);

#if DBG
    AvpDump = Store;
    if (AvpDump)
    {
        DbgPrint("%08X = %08X (read modify write)\n", Addr, Val);
    }
#endif
}

FORCEINLINE UCHAR
REG_RD08(VOID* Ptr, ULONG Addr)
{
    return *((volatile UCHAR*)((UCHAR*)(Ptr) + (Addr)));
}

FORCEINLINE VOID
REG_WR08(VOID* Ptr, ULONG Addr, UCHAR Val)
{
    *((volatile UCHAR*)((UCHAR*)(Ptr) + (Addr))) = (UCHAR)(Val);

#if DBG
    if (AvpDump)
    {
        DbgPrint("%08X = %08X\n", Addr, Val);
    }
#endif
}

#define REG_RD_DRF(Ptr,d,r,f)       (((REG_RD32(Ptr,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

#define REG_WR_DRF_NUM(Ptr,d,r,f,n) REG_WR32(Ptr, NV ## d ## r, DRF_NUM(d,r,f,n))
#define REG_WR_DRF_DEF(Ptr,d,r,f,c) REG_WR32(Ptr, NV ## d ## r, DRF_DEF(d,r,f,c))
#define FLD_WR_DRF_NUM(Ptr,d,r,f,n) REG_WR32(Ptr, NV##d##r,(REG_RD32(Ptr, NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define FLD_WR_DRF_DEF(Ptr,d,r,f,c) REG_WR32(Ptr, NV##d##r,(REG_RD32(Ptr, NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))

FORCEINLINE VOID
CRTC_WR(VOID* Ptr, UCHAR i, UCHAR d)
{
#if DBG
    ULONG Store = AvpDump;
    AvpDump = FALSE;
#endif

    REG_WR08(Ptr, NV_PRMCIO_CRX__COLOR, i);
    REG_WR08(Ptr, NV_PRMCIO_CR__COLOR, d);

#if DBG
    AvpDump = Store;
    if (AvpDump)
    {
        DbgPrint("CR%02X = %02X\n", i, d);
    }
#endif
}

FORCEINLINE UCHAR
CRTC_RD(VOID* Ptr, UCHAR i)            
{
    UCHAR Value;

#if DBG
    ULONG Store = AvpDump;
    AvpDump = FALSE;
#endif

    REG_WR08(Ptr, NV_PRMCIO_CRX__COLOR, i);
    Value = REG_RD08(Ptr, NV_PRMCIO_CR__COLOR);

#if DBG
    AvpDump = Store;
#endif

    return Value;
}

FORCEINLINE UCHAR
SMB_RD(VOID * Ptr, UCHAR Reg)
{
    ULONG Value;

#if DBG
    ULONG Store = AvpDump;
    AvpDump = FALSE;
#endif

    HalReadSMBusByte(TV_ENCODER_ID, Reg, &Value);

#if DBG
    AvpDump = Store;
#endif

    return (UCHAR)Value;
}

FORCEINLINE VOID
SMB_WR(VOID *Ptr, UCHAR Reg, UCHAR Val)
{
#if DBG
    ULONG Store = AvpDump;
    AvpDump = FALSE;
#endif

    HalWriteSMBusByte(TV_ENCODER_ID, Reg, Val);

#if DBG
    AvpDump = Store;
    if (AvpDump)
    {
        DbgPrint("TV%02X = %02X\n", Reg, Val);
    }
#endif
}

FORCEINLINE USHORT
SMB_RDW(VOID * Ptr, UCHAR Reg)
{
    ULONG Value;

#if DBG
    ULONG Store = AvpDump;
    AvpDump = FALSE;
#endif

    HalReadSMBusWord(TV_ENCODER_ID, Reg, &Value);

#if DBG
    AvpDump = Store;
#endif

    return (USHORT)Value;
}

FORCEINLINE VOID
SMB_WRW(VOID *Ptr, UCHAR Reg, USHORT Val)
{
#if DBG
    ULONG Store = AvpDump;
    AvpDump = FALSE;
#endif

    HalWriteSMBusWord(TV_ENCODER_ID, Reg, Val);

#if DBG
    AvpDump = Store;
    if (AvpDump)
    {
        DbgPrint("TV%04X = %04X\n", Reg, Val);
    }
#endif
}


FORCEINLINE VOID
SRX_WR(VOID *Ptr, UCHAR i, UCHAR d)            
{
#if DBG
    ULONG Store = AvpDump;
    AvpDump = FALSE;
#endif

    REG_WR08(Ptr, NV_PRMVIO_SRX, i);
    REG_WR08(Ptr, NV_PRMVIO_SR_RESET, (d));

#if DBG
    AvpDump = Store;
    if (AvpDump)
    {
        DbgPrint("SR%02X = %02X\n", i, d);
    }
#endif
}

FORCEINLINE VOID
GRX_WR(VOID *Ptr, UCHAR i, UCHAR d)            
{
#if DBG
    ULONG Store = AvpDump;
    AvpDump = FALSE;
#endif

    REG_WR08(Ptr, NV_PRMVIO_GRX, i);
    REG_WR08(Ptr, NV_PRMVIO_GX_SR, (d));

#if DBG
    AvpDump = Store;
    if (AvpDump)
    {
        DbgPrint("GR%02X = %02X\n", i, d);
    }
#endif
}

FORCEINLINE VOID
ARX_WR(VOID *Ptr, UCHAR i, UCHAR d)            
{
#if DBG
    ULONG Store = AvpDump;
    AvpDump = FALSE;
#endif

    REG_WR08(Ptr, NV_PRMCIO_ARX, i);
    REG_WR08(Ptr, NV_PRMCIO_ARX, (d));

#if DBG
    AvpDump = Store;
    if (AvpDump)
    {
        DbgPrint("AR%02X = %02X\n", i, d);
    }
#endif
}

#if DBG
#define AvDbgPrint(x)               DbgPrint x
#else
#define AvDbgPrint(x)
#endif

#endif // _AVP_H

