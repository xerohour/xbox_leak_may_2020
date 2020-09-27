/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    genoff.c

Abstract:

    This module implements a program which generates structure offset
    definitions for kernel structures that are accessed in assembly code.

Author:

    Bryan M. Willman (bryanwi) 16-Oct-90


To build:

    in ke\up do a "nmake UMAPPL=geni386"
    watch out for precompiled headers


Revision History:

    Forrest Foltz (forrestf) 24-Jan-1998

        Modified format to use new obj-based procedure.

--*/


#include "ki.h"
#pragma hdrstop

#include "nturtl.h"
#include "ntdbg.h"
#include "xdisk.h"
#include "bldr.h"
#include "xpcicfg.h"

include(`..\genxx.h')

#define KS386 KERNEL
#define HAL386 HAL

STRUC_ELEMENT ElementList[] = {

    START_LIST

  EnableInc(KS386)

    #include "genxx.inc"

    //
    // Generate architecture dependent definitions.
    //

    genCom("Apc Record Structure Offset Definitions")

    genDef(Ar, KAPC_RECORD, NormalRoutine)
    genDef(Ar, KAPC_RECORD, NormalContext)
    genDef(Ar, KAPC_RECORD, SystemArgument1)
    genDef(Ar, KAPC_RECORD, SystemArgument2)
    genVal(ApcRecordLength, sizeof(KAPC_RECORD))
    genSpc()

  EnableInc(HAL386)
    genCom("Processor Control Registers Structure Offset Definitions")

    genTxt("PCR equ dword ptr [_KiPCR]\n")
    genTxt("PCRB equ byte ptr [_KiPCR]\n")

    genAlt(PcExceptionList, KPCR,  NtTib.ExceptionList)
    genAlt(PcStackBase, KPCR, NtTib.StackBase)
    genAlt(PcStackLimit, KPCR, NtTib.StackLimit)
    genDef(Pc, KPCR, SelfPcr)
    genDef(Pc, KPCR, Prcb)
    genAlt(PcTeb, KPCR, NtTib.Self)
    genDef(Pc, KPCR, Irql)
    genDef(Pc, KPCR, PrcbData)
  DisableInc (HAL386)
    genVal(ProcessorControlRegisterLength, sizeof(KPCR))

  EnableInc (HAL386)
    genCom("Tss Structure Offset Definitions")

    genDef(Tss, KTSS, Ss0)
    genDef(Tss, KTSS, Esp0)
    genDef(Tss, KTSS, Eip)
    genDef(Tss, KTSS, EFlags)
    genDef(Tss, KTSS, Esp)
    genDef(Tss, KTSS, CR3)
    genDef(Tss, KTSS, Cs)
    genDef(Tss, KTSS, Ss)
    genDef(Tss, KTSS, Ds)
    genDef(Tss, KTSS, Es)
    genDef(Tss, KTSS, Fs)
    genDef(Tss, KTSS, Gs)
    genDef(Tss, KTSS, IoMapBase)
    genDef(Tss, KTSS, IoMaps)
    genVal(TssLength, sizeof(KTSS))
  DisableInc (HAL386)

  EnableInc (HAL386)
    genCom("Gdt Descriptor Offset Definitions")

    genNam(KGDT_R0_CODE)
    genNam(KGDT_R0_DATA)
    genNam(KGDT_R0_PCR)
    genNam(KGDT_TSS)
  DisableInc (HAL386)
    genNam(KGDT_DF_TSS)
    genNam(KGDT_NMI_TSS)

  EnableInc (HAL386)
    genCom("GdtEntry Offset Definitions")

    genDef(Kgdt, KGDTENTRY, BaseLow)
    genAlt(KgdtBaseMid, KGDTENTRY, HighWord.Bytes.BaseMid)
    genAlt(KgdtBaseHi, KGDTENTRY, HighWord.Bytes.BaseHi)
    genAlt(KgdtFlags1, KGDTENTRY, HighWord.Bytes.Flags1)
    genAlt(KgdtLimitHi, KGDTENTRY, HighWord.Bytes.Flags2)
    genDef(Kgdt, KGDTENTRY, LimitLow)
    genVal(KgdtEntryLength, (sizeof(KGDTENTRY)))
    genSpc()

    genCom("IdtEntry Offset Definitions")

    genDef(Kidt, KIDTENTRY, Offset)
    genDef(Kidt, KIDTENTRY, Selector)
    genDef(Kidt, KIDTENTRY, Access)
    genDef(Kidt, KIDTENTRY, ExtendedOffset)
    genVal(KidtEntryLength, (sizeof(KIDTENTRY)))
    genSpc()

    //
    // Processor block structure definitions.
    //

    genCom("Processor Block Structure Offset Definitions")

    genDef(Pb, KPRCB, CurrentThread)
    genDef(Pb, KPRCB, NextThread)
    genDef(Pb, KPRCB, IdleThread)

#ifdef DEVKIT
    genDef(Pb, KPRCB, DebugHaltThread)
    genDef(Pb, KPRCB, DebugMonitorData)
    genDef(Pb, KPRCB, DebugDoubleFault)
#endif

  DisableInc (HAL386)

    genDef(Pb, KPRCB, NpxThread)
    genDef(Pb, KPRCB, InterruptCount)
    genDef(Pb, KPRCB, DpcTime)
    genDef(Pb, KPRCB, InterruptTime)
    genDef(Pb, KPRCB, DebugDpcTime)
    genAlt(PbContextSwitches, KPRCB, KeContextSwitches)
    genDef(Pb, KPRCB, DpcInterruptRequested)
    genDef(Pb, KPRCB, DpcListHead)
    genDef(Pb, KPRCB, DpcRoutineActive)
    genDef(Pb, KPRCB, DpcStack)
    genDef(Pb, KPRCB, QuantumEnd)
    genDef(Pb, KPRCB, NpxSaveArea)
    genVal(ProcessorBlockLength, ((sizeof(KPRCB) + 15) & ~15))

  EnableInc (HAL386)
    genCom("Time Fields (TIME_FIELDS) Structure Offset Definitions")
    genDef(Tf, TIME_FIELDS, Second)
    genDef(Tf, TIME_FIELDS, Minute)
    genDef(Tf, TIME_FIELDS, Hour)
    genDef(Tf, TIME_FIELDS, Weekday)
    genDef(Tf, TIME_FIELDS, Day)
    genDef(Tf, TIME_FIELDS, Month)
    genDef(Tf, TIME_FIELDS, Year)
    genDef(Tf, TIME_FIELDS, Milliseconds)
    genSpc()
  DisableInc (HAL386)

  EnableInc (HAL386)
    genCom("constants for system irql and IDT vector conversion")

    genNam(MAXIMUM_IDTVECTOR)
    genNam(MAXIMUM_PRIMARY_VECTOR)
    genNam(PRIMARY_VECTOR_BASE)

    genCom("Flags in the CR0 register")

    genNam(CR0_PG)
    genNam(CR0_ET)
    genNam(CR0_TS)
    genNam(CR0_EM)
    genNam(CR0_MP)
    genNam(CR0_PE)
    genNam(CR0_CD)
    genNam(CR0_NW)
    genNam(CR0_AM)
    genNam(CR0_WP)
    genNam(CR0_NE)

    genCom("Flags in the CR4 register")

    genNam(CR4_VME)
    genNam(CR4_PVI)
    genNam(CR4_TSD)
    genNam(CR4_DE)
    genNam(CR4_PSE)
    genNam(CR4_PAE)
    genNam(CR4_MCE)
    genNam(CR4_PGE)
    genNam(CR4_FXSR)
    genNam(CR4_XMMEXCPT)

    genCom("Miscellaneous Definitions")

    genNam(MAXIMUM_PROCESSORS)
    genNam(INITIAL_STALL_COUNT)
    genNam(IRQL_NOT_GREATER_OR_EQUAL)
    genNam(IRQL_NOT_LESS_OR_EQUAL)
    genNam(MUTEX_ALREADY_OWNED)
    genNam(THREAD_NOT_MUTEX_OWNER)
  DisableInc (HAL386)
    genNam(BASE_PRIORITY_THRESHOLD)
    genNam(LOW_REALTIME_PRIORITY)
    genNam(KERNEL_STACK_SIZE)
    genNam(DOUBLE_FAULT_STACK_SIZE)
    genNam(EFLAG_SELECT)
    genNam(BREAKPOINT_BREAK )
    genNam(CLOCK_QUANTUM_DECREMENT)
    genNam(THREAD_QUANTUM)
    genNam(WAIT_QUANTUM_DECREMENT)
    genNam(ROUND_TRIP_DECREMENT_COUNT)

    //
    // Print trap frame offsets relative to sp.
    //

  EnableInc (HAL386)
    genCom("Trap Frame Offset Definitions and Length")

    genDef(Ts, KTRAP_FRAME, ExceptionList)
    genDef(Ts, KTRAP_FRAME, Edi)
    genDef(Ts, KTRAP_FRAME, Esi)
    genDef(Ts, KTRAP_FRAME, Ebp)
    genDef(Ts, KTRAP_FRAME, Ebx)
    genDef(Ts, KTRAP_FRAME, Edx)
    genDef(Ts, KTRAP_FRAME, Ecx)
    genDef(Ts, KTRAP_FRAME, Eax)
    genDef(Ts, KTRAP_FRAME, ErrCode)
    genDef(Ts, KTRAP_FRAME, Eip)
    genDef(Ts, KTRAP_FRAME, SegCs)
    genAlt(TsEflags, KTRAP_FRAME, EFlags)
    genDef(Ts, KTRAP_FRAME, HardwareEsp)
    genDef(Ts, KTRAP_FRAME, HardwareSegSs)
    genDef(Ts, KTRAP_FRAME, TempSegCs)
    genDef(Ts, KTRAP_FRAME, TempEsp)
    genDef(Ts, KTRAP_FRAME, DbgEbp)
    genDef(Ts, KTRAP_FRAME, DbgEip)
    genDef(Ts, KTRAP_FRAME, DbgArgMark)
    genDef(Ts, KTRAP_FRAME, DbgArgPointer)
    genNam(KTRAP_FRAME_LENGTH)
    genNam(KTRAP_FRAME_ALIGN)
    genNam(FRAME_EDITED)
    genNam(EFLAGS_ALIGN_CHECK)
    genNam(EFLAGS_V86_MASK)
    genNam(EFLAGS_INTERRUPT_MASK)
    genNam(EFLAGS_VIF)
    genNam(EFLAGS_VIP)
    genNam(EFLAGS_USER_SANITIZE)

    genCom("Context Frame Offset and Flag Definitions")

    genNam(CONTEXT_FULL)
    genNam(CONTEXT_DEBUG_REGISTERS)
    genNam(CONTEXT_CONTROL)
    genNam(CONTEXT_FLOATING_POINT)
    genNam(CONTEXT_INTEGER)
    genNam(CONTEXT_SEGMENTS)
    genSpc()

    //
    // Print context frame offsets relative to sp.
    //

    genDef(Cs, CONTEXT, ContextFlags)
    genDef(Cs, CONTEXT, FloatSave)
    genDef(Cs, CONTEXT, Edi)
    genDef(Cs, CONTEXT, Esi)
    genDef(Cs, CONTEXT, Ebp)
    genDef(Cs, CONTEXT, Ebx)
    genDef(Cs, CONTEXT, Edx)
    genDef(Cs, CONTEXT, Ecx)
    genDef(Cs, CONTEXT, Eax)
    genDef(Cs, CONTEXT, Eip)
    genDef(Cs, CONTEXT, SegCs)
    genAlt(CsEflags, CONTEXT, EFlags)
    genDef(Cs, CONTEXT, Esp)
    genDef(Cs, CONTEXT, SegSs)
    genVal(ContextFrameLength, ROUND_UP(sizeof(CONTEXT), 16))
    genNam(DR6_LEGAL)
    genNam(DR7_LEGAL)
    genNam(DR7_ACTIVE)

    //
    // Print Registration Record Offsets relative to base
    //

    genDef(Err, EXCEPTION_REGISTRATION_RECORD, Handler)
    genDef(Err, EXCEPTION_REGISTRATION_RECORD, Next)

    //
    // Print floating point field offsets relative to Context.FloatSave
    //

    genCom("Floating save area field offset definitions")

    genDef(Fx, FLOATING_SAVE_AREA, ControlWord)
    genDef(Fx, FLOATING_SAVE_AREA, StatusWord)
    genDef(Fx, FLOATING_SAVE_AREA, ErrorOffset)
    genDef(Fx, FLOATING_SAVE_AREA, ErrorSelector)
    genDef(Fx, FLOATING_SAVE_AREA, DataOffset)
    genDef(Fx, FLOATING_SAVE_AREA, DataSelector)
    genDef(Fx, FLOATING_SAVE_AREA, MXCsr)
    genDef(Fp, FLOATING_SAVE_AREA, Cr0NpxState)

    genSpc()
    genVal(NPX_FRAME_LENGTH, sizeof(FX_SAVE_AREA))

    //
    // Processor State Frame offsets relative to base
    //

    genCom("Processor State Frame Offset Definitions")

    genDef(Ps, KPROCESSOR_STATE, ContextFrame)
    genDef(Ps, KPROCESSOR_STATE, SpecialRegisters)
    genDef(Sr, KSPECIAL_REGISTERS, Cr0)
    genDef(Sr, KSPECIAL_REGISTERS, Cr2)
    genDef(Sr, KSPECIAL_REGISTERS, Cr3)
    genDef(Sr, KSPECIAL_REGISTERS, Cr4)
    genDef(Sr, KSPECIAL_REGISTERS, KernelDr0)
    genDef(Sr, KSPECIAL_REGISTERS, KernelDr1)
    genDef(Sr, KSPECIAL_REGISTERS, KernelDr2)
    genDef(Sr, KSPECIAL_REGISTERS, KernelDr3)
    genDef(Sr, KSPECIAL_REGISTERS, KernelDr6)
    genDef(Sr, KSPECIAL_REGISTERS, KernelDr7)
    genAlt(SrGdtr, KSPECIAL_REGISTERS, Gdtr.Limit)

    genAlt(SrIdtr, KSPECIAL_REGISTERS, Idtr.Limit)
    genDef(Sr, KSPECIAL_REGISTERS, Tr)
    genDef(Sr, KSPECIAL_REGISTERS, Ldtr)

    genDef(PsNT5, X86_NT5_KPROCESSOR_STATE, ContextFrame)
    genDef(PsNT5, X86_NT5_KPROCESSOR_STATE, SpecialRegisters)
    genDef(SrNT5, X86_KSPECIAL_REGISTERS, Cr0)
    genDef(SrNT5, X86_KSPECIAL_REGISTERS, Cr2)
    genDef(SrNT5, X86_KSPECIAL_REGISTERS, Cr3)
    genDef(SrNT5, X86_KSPECIAL_REGISTERS, Cr4)
    genDef(SrNT5, X86_KSPECIAL_REGISTERS, KernelDr0)
    genDef(SrNT5, X86_KSPECIAL_REGISTERS, KernelDr1)
    genDef(SrNT5, X86_KSPECIAL_REGISTERS, KernelDr2)
    genDef(SrNT5, X86_KSPECIAL_REGISTERS, KernelDr3)
    genDef(SrNT5, X86_KSPECIAL_REGISTERS, KernelDr6)
    genDef(SrNT5, X86_KSPECIAL_REGISTERS, KernelDr7)
    genAlt(SrNT5Gdtr, X86_KSPECIAL_REGISTERS, Gdtr.Limit)

    genAlt(SrNT5Idtr, X86_KSPECIAL_REGISTERS, Idtr.Limit)
    genDef(SrNT5, X86_KSPECIAL_REGISTERS, Tr)
    genDef(SrNT5, X86_KSPECIAL_REGISTERS, Ldtr)

    genSpc()
    genNam(PAGE_SIZE)

    genSpc()
    genNam(CLOCK_ROLLOVER_COUNT)
    genNam(CLOCK_TIME_INCREMENT)

    genSpc()
    genNam(PAGE_DIRECTORY_PHYSICAL_ADDRESS)

    genSpc()
    genNam(ROM_SHADOW_PHYSICAL_ADDRESS)
    genNam(ROM_SHADOW_SIZE)

    genSpc()
    genNam(XDISK_UNPARTITIONED_SECTORS)

    genSpc()
    genVal(MzXdataSectionHeader, ((ULONG_PTR)&((PIMAGE_DOS_HEADER)PsNtosImageBase)->e_res2))
    genDef(Xdsh, XDATA_SECTION_HEADER, SizeOfUninitializedData)
    genDef(Xdsh, XDATA_SECTION_HEADER, SizeOfInitializedData)
    genDef(Xdsh, XDATA_SECTION_HEADER, PointerToRawData)
    genDef(Xdsh, XDATA_SECTION_HEADER, VirtualAddress)

    genSpc()
    genDef(Mudb, MICROCODE_UPDATE_DATA_BLOCK, Processor)
    genDef(Mudb, MICROCODE_UPDATE_DATA_BLOCK, UpdateData)
    genVal(MudbStructureLength, sizeof(MICROCODE_UPDATE_DATA_BLOCK))

    genNam(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0)
    genNam(XPCICFG_LPCBRIDGE_IO_REGISTER_LENGTH_0)
    genNam(XPCICFG_SMBUS_IO_REGISTER_BASE_1)
    genNam(XPCICFG_SMBUS_IO_REGISTER_LENGTH_1)
    genNam(XPCICFG_GPU_MEMORY_REGISTER_BASE_0)
    genNam(XPCICFG_GPU_MEMORY_REGISTER_LENGTH_0)

  DisableInc (HAL386)

    END_LIST
};
