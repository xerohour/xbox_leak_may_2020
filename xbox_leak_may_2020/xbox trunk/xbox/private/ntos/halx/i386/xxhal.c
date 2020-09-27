/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    xxhal.c

Abstract:

    This module implements the initialization of the system dependent
    functions that define the Hardware Architecture Layer (HAL) for an
    x86 system.

--*/

#include "halp.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, HalInitSystemPhase0)
#endif

VOID
HalInitSystemPhase0(
    VOID
    )
/*++

Routine Description:

    This function initializes the Hardware Architecture Layer (HAL) for an
    x86 system.

Arguments:

    None.

Return Value:

    None.

--*/
{
    ULONGLONG ApicBits;

    _outp(0x61, 0x08);

    //
    // Disable the processor APIC (advanced programmable interrupt
    // controller).  The processor powers on with the APIC enabled.
    //

    HalPulseHardwareMonitorPin();
    ApicBits = RDMSR(APIC_BASE_MSR);
    WRMSR(APIC_BASE_MSR, (ApicBits & ~APIC_ENABLED));

    //
    // Initialize the 8259 programmable interrupt controller.  Note that
    // this will enable interrupts.
    //

    HalPulseHardwareMonitorPin();
    HalpInitializePICs();

    //
    // Initialize the real time clock.
    //

    HalPulseHardwareMonitorPin();
    HalpInitializeRealTimeClock();

    //
    // Note that HalpInitializeClock MUST be called after
    // HalpInitializeStallExecution, because HalpInitializeStallExecution
    // reprograms the timer.
    //

//  HalpInitializeStallExecution();

    //
    // Setup the clock.
    //

    HalPulseHardwareMonitorPin();
    HalpInitializeClock();

    //
    // Make sure the profile interrupt is disabled.
    //

    HalPulseHardwareMonitorPin();
    HalStopProfileInterrupt(0);

    //
    // Configure the devices built into the south bridge.
    //

    HalPulseHardwareMonitorPin();
    HalpSetupPCIDevices();

    //
    // Initialize the System Control Interrupt.
    //

    HalPulseHardwareMonitorPin();
    HalpInitializeSystemControlInterrupt();
}

VOID
HalInitSystemPhase0Quick(
    VOID
    )
/*++

Routine Description:

    This function initializes the Hardware Architecture Layer (HAL) for an
    x86 system.

Arguments:

    None.

Return Value:

    None.

--*/
{
    //
    // Initialize the 8259 programmable interrupt controller.  Note that
    // this will enable interrupts.
    //

    HalPulseHardwareMonitorPin();
    HalpInitializePICs();
}

VOID
HalInitSystemPhase1(
    VOID
    )
/*++

Routine Description:

    This function initializes the Hardware Architecture Layer (HAL) for an
    x86 system.

Arguments:

    None.

Return Value:

    None.

--*/
{
    //
    // Connect to the system interrupts.
    //

    HalPulseHardwareMonitorPin();
    KiSetHandlerAddressToIDT(CLOCK_VECTOR, KeClockInterrupt);
    HalEnableSystemInterrupt(CLOCK_VECTOR - PRIMARY_VECTOR_BASE, Latched);

    HalPulseHardwareMonitorPin();
    KiSetHandlerAddressToIDT(PROFILE_VECTOR, HalpProfileInterrupt);
    HalEnableSystemInterrupt(PROFILE_VECTOR - PRIMARY_VECTOR_BASE, Latched);

    HalPulseHardwareMonitorPin();
    KiSetHandlerAddressToIDT(SMBUS_VECTOR, HalpSMBusInterrupt);
    HalEnableSystemInterrupt(SMBUS_VECTOR - PRIMARY_VECTOR_BASE, LevelSensitive);

    //
    // The following actions need to be performed before servicing interrupts
    // from the SMC, but after connecting to the above interrupt vectors.
    //

    if (!KeHasQuickBooted) {
        HalPulseHardwareMonitorPin();
        HalpProcessSMCInitialState();
    }

    //
    // Connect to the System Control Interrupt.
    //

    HalPulseHardwareMonitorPin();
    KiSetHandlerAddressToIDT(SCI_VECTOR, HalpSystemControlInterrupt);
    HalEnableSystemInterrupt(SCI_VECTOR - PRIMARY_VECTOR_BASE, LevelSensitive);
}

VOID
HalPulseHardwareMonitorPin(
    VOID
    )
/*++

Routine Description:

    This function pulses the hardware monitor pin used to externally monitor the
    boot process.

Arguments:

    None.

Return Value:

    None.

--*/
{
    //
    // Don't pulse the hardware monitor pin if this is a quick boot or a shadow
    // boot.
    //

    if (!KeHasQuickBooted &&
        ((XboxBootFlags & XBOX_BOOTFLAG_SHADOW) == 0)) {

        _outp(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + 0xCF, 0x05);

        KeStallExecutionProcessor(5);

        _outp(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + 0xCF, 0x04);

        KeStallExecutionProcessor(5);
    }
}
