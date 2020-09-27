/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    systime.c

Abstract:

    This module implements the NT system time services.

--*/

#include "exp.h"

//
// Guards against multiple threads attempting to set the system time.
//
INITIALIZED_CRITICAL_SECTION(ExpTimeRefreshLock);

NTSTATUS
NtSetSystemTime (
    IN PLARGE_INTEGER SystemTime,
    OUT PLARGE_INTEGER PreviousTime OPTIONAL
    )

/*++

Routine Description:

    This function sets the current system time and optionally returns the
    previous system time.

Arguments:

    SystemTime - Supplies a pointer to the new value for the system time.

    PreviousTime - Supplies an optional pointer to a variable that receives
        the previous system time.

Return Value:

    STATUS_SUCCESS is returned if the service is successfully executed.

    STATUS_ACCESS_VIOLATION is returned if the input parameter for the
        system time cannot be read or the output parameter for the system
        time cannot be written.

    STATUS_INVALID_PARAMETER is returned if the input system time is negative.

--*/

{
    LARGE_INTEGER CurrentTime;
    LARGE_INTEGER NewTime;
    TIME_FIELDS TimeFields;

    PAGED_CODE();

    RtlEnterCriticalSectionAndRegion(&ExpTimeRefreshLock);

    //
    // Get the new system time and check to ensure that the value is
    // positive and resonable. If the new system time is negative, then
    // return an invalid parameter status.
    //

    NewTime = *SystemTime;

    if ((NewTime.HighPart < 0) || (NewTime.HighPart > 0x20000000)) {
        RtlLeaveCriticalSectionAndRegion(&ExpTimeRefreshLock);
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Set the system time, and capture the previous system time in a
    // local variable, then store the local variable in the previous time
    // variable if it is specified. This is required so that faults can
    // be prevented from happening in the set time routine.
    //

    KeSetSystemTime(&NewTime, &CurrentTime);

    RtlTimeToTimeFields(&NewTime, &TimeFields);
    HalSetRealTimeClock(&TimeFields);

    if (ARGUMENT_PRESENT(PreviousTime)) {
        *PreviousTime = CurrentTime;
    }

    //
    // Mark the CMOS data as valid.
    //

    HalMarkCmosValid();

    RtlLeaveCriticalSectionAndRegion(&ExpTimeRefreshLock);
    return STATUS_SUCCESS;
}
