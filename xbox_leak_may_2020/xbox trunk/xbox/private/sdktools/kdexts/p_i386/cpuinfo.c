/*++

Copyright (c) 1999  Microsoft Corporation

Module Name:

    mps.c

Abstract:

    WinDbg Extension Api

Author:

    Peter Johnston (peterj) 19-April-1999

Environment:

    User Mode.

Revision History:

--*/

#include "precomp.h"
#pragma hdrstop

//
// xReadMemory is easier to use than ReadMemory and is
// defined in ..\devnode.c
//

BOOLEAN
xReadMemory(
    PVOID S,
    PVOID D,
    ULONG Len
    );


VOID
DumpCpuInfo(
    IN ULONG StartCpu,
    IN BOOLEAN DoMultipleCpus
)
{
    KPCR    pcr;
    KPRCB   prcb;
    ULONG   addr;
    ULONG   processor = StartCpu;
    BOOLEAN doNext = DoMultipleCpus;
    BOOLEAN doHead = TRUE;
    UCHAR   sigWarn1, sigWarn2;
    LARGE_INTEGER updateSignature;
    PROCESSORINFO pi;

    if (!Ioctl(IG_KD_CONTEXT, &pi, sizeof(pi))) {
        dprintf("Unable to get processor info, quitting\n");
        return;
    }

    do {
        if (!ReadPcr((USHORT)processor, &pcr, &addr, (HANDLE)0)) {
            if (doNext == FALSE) {

                //
                // if not doing a bunch of processors, user must have
                // asked for this one,... so we should tell them we
                // couldn't get to it.
                //

                dprintf("Unable to read PCR for Processor %u, quitting\n",
                        processor);
            }
            return;
        }

        //
        // Got the PCR, what we really want is the PRCB.
        //
        // Sanity?
        //

        if ((ULONG)pcr.Prcb != ((ULONG)pcr.SelfPcr + FIELD_OFFSET(KPCR, PrcbData))) {
            dprintf(
                "!sanity: PCR->Prcb (%p) != &PCR->PrcbData (%p), quitting.\n",
                pcr.Prcb,
                pcr.SelfPcr + FIELD_OFFSET(KPCR, PrcbData)
                );
            return;
        }

        if (!xReadMemory(pcr.Prcb, &prcb, sizeof(prcb))) {
            dprintf("Unable to read PRCB for processor %u, quitting.\n",
                    processor);
            return;
        }

        if ((ULONG)prcb.Number != processor) {

            //
            // Processor number isn't what I expected.  Bail out.
            // This will need revisiting at some stage in the future
            // when we support a discontiguous set of processor numbers.
            //

            dprintf("Processor %d mismatch with processor number in PRCB %d, quitting\n",
                    processor,
                    prcb.Number);
            return;
        }

        if (prcb.CpuID == 0) {

            //
            // This processor doesn't support CPUID,... not likely in
            // an MP environment but also means we don't have anything
            // useful to say.
            //

            dprintf("Processor %d doesn't support CPUID, quitting.\n",
                    processor);
        }

        //
        // If this is an Intel processor, family 6 (or, presumably
        // above family 6) read the current UpdateSignature from 
        // the processor rather than using what was there when we
        // booted,... it may have been updated.
        //
        // Actually, this can't be done unless we can switch processors
        // from within an extension.   So, mark the processor we did
        // it for (unless there's only one processor).
        //

        updateSignature = prcb.UpdateSignature;
        sigWarn1 = sigWarn2 = ' ';

        if ((!strcmp(prcb.VendorString, "GenuineIntel")) &&
            (prcb.CpuType >= 6)) {

            if (prcb.Number == pi.Processor) {
                READ_WRITE_MSR msr;

                msr.Msr = 0x8b;
                msr.Value = 0;

                if (Ioctl(IG_READ_MSR, &msr, sizeof(msr))) {
                    updateSignature.QuadPart = msr.Value;
                }
                if (pi.NumberProcessors != 1) {
                    sigWarn1 = '>';
                    sigWarn2 = '<';
                }
            }
        }

        //
        // This extension could pretty much be !PRCB but it's a 
        // subset,... perhaps we should have a !PRCB?
        //

        if (doHead) {

            //
            // Be pretty.
            //

            dprintf("CP F/M/S Manufacturer  MHz Update Signature Features\n");
            doHead = FALSE;
        }

        dprintf("%2d %d,%d,%d %12s%5d%c%08x%08x%c%08x\n",
                prcb.Number,
                prcb.CpuType,
                (prcb.CpuStep >> 8) & 0xff,
                prcb.CpuStep & 0xff,
                prcb.VendorString,
                prcb.MHz,
                sigWarn1,
                updateSignature.u.HighPart,
                updateSignature.u.LowPart,
                sigWarn2,
                prcb.FeatureBits);

        //
        // Done with this processor,... advance.  (Sanity check too,...
        // this is an x86 extension, we'll never support more than 32
        // processors on x86).
        //

        if (++processor > 31) {
            doNext = FALSE;
        }
    } while (doNext == TRUE);
}

DECLARE_API( cpuinfo )

/*++

Routine Description:

    Gather up any info we know is still in memory that we gleaned
    using the CPUID instruction,.... and a few other interesting
    tidbits as well.

Arguments:

    None

Return Value:

    None

--*/

{
    ULONG   processor;
    BOOLEAN doNext = FALSE;

#if 0

    //
    // Apply to whichever processor user asks for
    //

    if (strlen(args)) {
        if (strchr(args, '?')) {

            //
            // User wants to know the rules.
            //

            dprintf("usage: !cpuinfo      print cpuinfo info for current cpu.\n");
            dprintf("       !cpuinfo n    print cpuinfo info for cpu n.\n");
            dprintf("       !cpuinfo all  print cpuinfo for all processors.\n");
            return;
        }
        if (!strcmp(args, "all")) {
            doNext = TRUE;
            processor = 0;
        } else {
            processor = strtoul(args, NULL, 16);
        }
    } else {
        processor = dwProcessor;
    }

#else

    //
    // Default to all CPUs, always (ie no options).
    //

    doNext = TRUE;
    processor = 0;

#endif

    DumpCpuInfo(processor, doNext);
}
