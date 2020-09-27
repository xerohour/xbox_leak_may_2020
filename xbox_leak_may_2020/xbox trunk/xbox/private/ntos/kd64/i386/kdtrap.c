/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    kdtrap.c

Abstract:

    This module contains code to implement the target side of the portable
    kernel debugger.

Author:

    Bryan M. Willman (bryanwi) 25-Sep-90

Revision History:

--*/

#include "kdp.h"

VOID
KdpContextToNt5Context(
    IN PCONTEXT Context,
    OUT PX86_NT5_CONTEXT Nt5Context
    );

VOID
KdpContextFromNt5Context(
    OUT PCONTEXT Context,
    IN PX86_NT5_CONTEXT Nt5Context
    );

BOOLEAN
KdpCheckTracePoint(
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN OUT PCONTEXT ContextRecord
    );

BOOLEAN
KdpTrap (
    IN PKTRAP_FRAME TrapFrame,
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PCONTEXT ContextRecord,
    IN BOOLEAN SecondChance
    )

/*++

Routine Description:

    This routine is called whenever a exception is dispatched and the kernel
    debugger is active.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame that describes the
        trap.

    ExceptionFrame - Supplies a pointer to a exception frame that describes
        the trap.

    ExceptionRecord - Supplies a pointer to an exception record that
        describes the exception.

    ContextRecord - Supplies the context at the time of the exception.

    SecondChance - Supplies a boolean value that determines whether this is
        the second chance (TRUE) that the exception has been raised.

Return Value:

    A value of TRUE is returned if the exception is handled. Otherwise a
    value of FALSE is returned.

--*/

{

    BOOLEAN Completion = FALSE;
    BOOLEAN Enable;
    BOOLEAN UnloadSymbols = FALSE;
    ULONG   RetValue;
    STRING  String, AssertString, ReplyString;
    PUCHAR  Buffer;
    PKD_SYMBOLS_INFO SymbolInfo;
    PVOID   SavedEsp;
    ULONG   OldEip;
	char	AssertBuffer[2];

    _asm {
        //
        // Save esp on ebp frame so c-runtime registers are restored correctly
        //

        mov     SavedEsp, esp
    }

    //
    // Print, Prompt, Load symbols, Unload symbols, are all special
    // cases of STATUS_BREAKPOINT
    //

    if ((ExceptionRecord->ExceptionCode == STATUS_BREAKPOINT) &&
        (ExceptionRecord->ExceptionInformation[0] != BREAKPOINT_BREAK)) {

        //
        // We have one of the support functions.
        //

        if (KdDebuggerNotPresent  &&
            ExceptionRecord->ExceptionInformation[0] != BREAKPOINT_PROMPT &&
            ExceptionRecord->ExceptionInformation[0] != BREAKPOINT_RTLASSERT &&
            ExceptionRecord->ExceptionInformation[0] != BREAKPOINT_RIP) {
            ContextRecord->Eip++;
            return(TRUE);
        }


        //
        // Since some of these functions can be entered from user mode,
        // we hold off entering the debugger until the user mode buffers
        // are copied.  (Because they may not be present in memory, and
        // they must be paged in before we raise Irql to the
        // Highest level.)
        //
        //

        OldEip = ContextRecord->Eip;

        switch (ExceptionRecord->ExceptionInformation[0]) {

            //
            //  ExceptionInformation[1] is PSTRING to print
            //

            case BREAKPOINT_KDPRINT:
            case BREAKPOINT_PRINT:
                String = *((PSTRING)ExceptionRecord->ExceptionInformation[1]);

                KdLogDbgPrint(&String);

                if ((KdGlobalFlag & FLG_DISABLE_DBGPRINT) == 0) {
                    Enable = KdEnterDebugger(TrapFrame, ExceptionFrame);
                    if (KdpPrintString(&String)) {
                        ContextRecord->Eax = (ULONG)(STATUS_BREAKPOINT);
                    } else {
                        ContextRecord->Eax = STATUS_SUCCESS;
                    }
                    KdExitDebugger(Enable);
                }

                Completion = TRUE;
                break;

			//
			// ExceptionInformation[1] is the string to print as the assert
			//   message
			//

            case BREAKPOINT_RTLASSERT:
                String = *((PSTRING)ExceptionRecord->ExceptionInformation[1]);
				RtlInitAnsiString(&AssertString,
					"Break, Ignore, or Terminate Thread (bit)? ");
				ReplyString.Buffer = AssertBuffer;
				ReplyString.MaximumLength = 2;

				Enable = KdEnterDebugger(TrapFrame, ExceptionFrame);
				KdpPrintString(&String);
				do {
					RetValue = KdpPromptString(&AssertString, &ReplyString);
				} while (RetValue == TRUE);

				ContextRecord->Eax = AssertBuffer[0];
				KdExitDebugger(Enable);

                Completion = TRUE;
                break;

			//
			// ExceptionInformation[1] is the null-terminated string to
			// send
			//

			case BREAKPOINT_RIP:
				{
				char szRipText[] = "\n***RIP: ";
				char szNL[] = "\n";

				String.Buffer = szRipText;
				String.Length = sizeof szRipText - 1;
				String.MaximumLength = sizeof szRipText;
				KdpPrintString(&String);

				String.Buffer = (LPSTR)ExceptionRecord->ExceptionInformation[1];
				String.Length = (USHORT)strlen(String.Buffer);
				String.MaximumLength = String.Length + 1;
				KdpPrintString(&String);

				String.Buffer = szNL;
				String.Length = 1;
				String.MaximumLength = 2;
				KdpPrintString(&String);

				// now we stop as if at a breakpoint
	            Enable = KdEnterDebugger(TrapFrame, ExceptionFrame);
				goto RipBreak;
				}

            //
            //  ExceptionInformation[1] is prompt string,
            //  ExceptionInformation[2] is return string
            //

            case BREAKPOINT_PROMPT:
                String = *((PSTRING)ExceptionRecord->ExceptionInformation[1]);
                ReplyString = *((PSTRING)ExceptionRecord->ExceptionInformation[2]);

                //
                // Prompt, keep prompting until no breakin seen.
                //

                KdLogDbgPrint(&String);

                Enable = KdEnterDebugger(TrapFrame, ExceptionFrame);
                do {
                    RetValue = KdpPromptString(&String, &ReplyString);
                } while (RetValue == TRUE);

                ContextRecord->Eax = ReplyString.Length;
                KdExitDebugger(Enable);

                Completion = TRUE;
                break;

            //
            //  ExceptionInformation[1] is file name of new module
            //  ExceptionInformaiton[2] is the base of the dll
            //

            case BREAKPOINT_UNLOAD_SYMBOLS:
                UnloadSymbols = TRUE;

                //
                // Fall through
                //

            case BREAKPOINT_LOAD_SYMBOLS:

                Enable = KdEnterDebugger(TrapFrame, ExceptionFrame);

                //
                // Save and restore the processor context in case the
                // kernel debugger has been configured to stop on dll
                // loads.
                //

                KdpSaveProcessorControlState(&KdProcessorState);
                KdpContextToNt5Context(ContextRecord, &KdProcessorState.ContextFrame);

                SymbolInfo = (PKD_SYMBOLS_INFO)ExceptionRecord->ExceptionInformation[2];
                Completion =
                    KdpReportLoadSymbolsStateChange((PSTRING)ExceptionRecord->ExceptionInformation[1],
                                                    SymbolInfo,
                                                    UnloadSymbols,
                                                    &KdProcessorState.ContextFrame);

                KdpContextFromNt5Context(ContextRecord, &KdProcessorState.ContextFrame);
                KdpRestoreProcessorControlState(&KdProcessorState);

                KdExitDebugger(Enable);
                break;

            case BREAKPOINT_LOAD_XESECTION:
            case BREAKPOINT_UNLOAD_XESECTION:
            case BREAKPOINT_CREATE_FIBER:
            case BREAKPOINT_DELETE_FIBER:
                // we don't actually pay attention to these
                Completion = TRUE;
                break;

            //
            //  Unknown command
            //

            default:
                // return FALSE
                break;
        }
        //
        // If the kernel debugger did not update the EIP, then increment
        // past the breakpoint instruction.
        //

        if (ContextRecord->Eip == OldEip) {
            ContextRecord->Eip++;
        }


    } else {

        if  ((ExceptionRecord->ExceptionCode == STATUS_BREAKPOINT) ||
             (ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP)  ||
             (KdGlobalFlag & FLG_STOP_ON_EXCEPTION) ||
             SecondChance) {

            if (!SecondChance &&
                (ExceptionRecord->ExceptionCode == STATUS_PORT_DISCONNECTED ||
                 NT_SUCCESS( ExceptionRecord->ExceptionCode )
                )
               ) {
                //
                // User does not really want to see these either.
                // so do NOT report it to debugger.
                //

                return FALSE;
                }

            //
            // Report state change to kernel debugger on host
            //

            Enable = KdEnterDebugger(TrapFrame, ExceptionFrame);

            Completion = KdpCheckTracePoint(ExceptionRecord,ContextRecord);
            if (!Completion) {

RipBreak:
                KdpSaveProcessorControlState(&KdProcessorState);
                KdpContextToNt5Context(ContextRecord, &KdProcessorState.ContextFrame);

                Completion =
                    KdpReportExceptionStateChange(ExceptionRecord,
                                                  &KdProcessorState.ContextFrame,
                                                  SecondChance);

                KdpContextFromNt5Context(ContextRecord, &KdProcessorState.ContextFrame);
                KdpRestoreProcessorControlState(&KdProcessorState);
            }

            KdExitDebugger(Enable);

            KdpControlCPressed = FALSE;

        } else {

            //
            // This is real exception that user doesn't want to see,
            // so do NOT report it to debugger.
            //

            // return FALSE;
        }
    }

    _asm {
        mov     esp, SavedEsp
    }
    return Completion;
}

BOOLEAN
KdpStub (
    IN PKTRAP_FRAME TrapFrame,
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PCONTEXT ContextRecord,
    IN BOOLEAN SecondChance
    )

/*++

Routine Description:

    This routine provides a kernel debugger stub routine to catch debug
    prints in a checked system when the kernel debugger is not active.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame that describes the
        trap.

    ExceptionFrame - Supplies a pointer to a exception frame that describes
        the trap.

    ExceptionRecord - Supplies a pointer to an exception record that
        describes the exception.

    ContextRecord - Supplies the context at the time of the exception.

    SecondChance - Supplies a boolean value that determines whether this is
        the second chance (TRUE) that the exception has been raised.

Return Value:

    A value of TRUE is returned if the exception is handled. Otherwise a
    value of FALSE is returned.

--*/

{
    PULONG  SymbolArgs;
    //
    // If the breakpoint is a debug print, then return TRUE. Otherwise,
    // return FALSE.
    //

    if ((ExceptionRecord->ExceptionCode == STATUS_BREAKPOINT) &&
        (ExceptionRecord->NumberParameters > 0) &&
        ((ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_LOAD_SYMBOLS)||
         (ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_UNLOAD_SYMBOLS)||
         (ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_LOAD_XESECTION)||
         (ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_UNLOAD_XESECTION)||
         (ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_KDPRINT)||
         (ExceptionRecord->ExceptionInformation[0] == BREAKPOINT_PRINT))) {

        ContextRecord->Eip++;
        return(TRUE);
    } else if (KdDebuggerEnabled == FALSE) {
        return(FALSE);
    } else {
        return(KdpCheckTracePoint(ExceptionRecord,ContextRecord));
    }
}

#define NUMBER_OF_FP_REGISTERS  8
#define BYTES_PER_FP_REGISTER   10
#define BYTES_PER_FX_REGISTER   16

VOID
KdpContextToNt5Context(
    IN PCONTEXT Context,
    OUT PX86_NT5_CONTEXT Nt5Context
    )
{
    ULONG i;

    Nt5Context->ContextFlags = Context->ContextFlags;

    //
    // CONTEXT_CONTROL
    //

    Nt5Context->Ebp = Context->Ebp;
    Nt5Context->Eip = Context->Eip;
    Nt5Context->SegCs = Context->SegCs;
    Nt5Context->EFlags = Context->EFlags;
    Nt5Context->Esp = Context->Esp;
    Nt5Context->SegSs = Context->SegSs;

    //
    // CONTEXT_SEGMENTS
    //

    Nt5Context->SegGs = 0;
    Nt5Context->SegFs = KGDT_R0_PCR;
    Nt5Context->SegEs = KGDT_R0_DATA;
    Nt5Context->SegDs = KGDT_R0_DATA;

    //
    // CONTEXT_INTEGER
    //

    Nt5Context->Edi = Context->Edi;
    Nt5Context->Esi = Context->Esi;
    Nt5Context->Ebx = Context->Ebx;
    Nt5Context->Edx = Context->Edx;
    Nt5Context->Ecx = Context->Ecx;
    Nt5Context->Eax = Context->Eax;

    //
    // CONTEXT_EXTENDED_REGISTERS
    //

    RtlCopyMemory(Nt5Context->ExtendedRegisters, &Context->FloatSave,
        MAXIMUM_SUPPORTED_EXTENSION);

    //
    // CONTEXT_FLOATING_POINT
    //

    Nt5Context->FloatSave.ControlWord = (USHORT)Context->FloatSave.ControlWord;
    Nt5Context->FloatSave.StatusWord = (USHORT)Context->FloatSave.StatusWord;
    Nt5Context->FloatSave.TagWord = (USHORT)Context->FloatSave.TagWord;
    Nt5Context->FloatSave.ErrorOffset = Context->FloatSave.ErrorOffset;
    Nt5Context->FloatSave.ErrorSelector = Context->FloatSave.ErrorSelector;
    Nt5Context->FloatSave.DataOffset = Context->FloatSave.DataOffset;
    Nt5Context->FloatSave.DataSelector = Context->FloatSave.DataSelector;
    Nt5Context->FloatSave.Cr0NpxState = Context->FloatSave.Cr0NpxState;

    for (i = 0; i < NUMBER_OF_FP_REGISTERS; i++) {
        RtlCopyMemory(&Nt5Context->FloatSave.RegisterArea[i*BYTES_PER_FP_REGISTER],
            &Context->FloatSave.RegisterArea[i*BYTES_PER_FX_REGISTER],
            BYTES_PER_FP_REGISTER);
    }
}

VOID
KdpContextFromNt5Context(
    OUT PCONTEXT Context,
    IN PX86_NT5_CONTEXT Nt5Context
    )
{
    ULONG i;

    Context->ContextFlags = Nt5Context->ContextFlags;

    //
    // CONTEXT_CONTROL
    //

    Context->Ebp = Nt5Context->Ebp;
    Context->Eip = Nt5Context->Eip;
    Context->SegCs = Nt5Context->SegCs;
    Context->EFlags = Nt5Context->EFlags;
    Context->Esp = Nt5Context->Esp;
    Context->SegSs = Nt5Context->SegSs;

    //
    // CONTEXT_INTEGER
    //

    Context->Edi = Nt5Context->Edi;
    Context->Esi = Nt5Context->Esi;
    Context->Ebx = Nt5Context->Ebx;
    Context->Edx = Nt5Context->Edx;
    Context->Ecx = Nt5Context->Ecx;
    Context->Eax = Nt5Context->Eax;

    //
    // CONTEXT_EXTENDED_REGISTERS
    //

    RtlCopyMemory(&Context->FloatSave, Nt5Context->ExtendedRegisters,
        MAXIMUM_SUPPORTED_EXTENSION);

    //
    // CONTEXT_FLOATING_POINT
    //

    Context->FloatSave.ControlWord = (USHORT)Nt5Context->FloatSave.ControlWord;
    Context->FloatSave.StatusWord = (USHORT)Nt5Context->FloatSave.StatusWord;
    Context->FloatSave.TagWord = (USHORT)Nt5Context->FloatSave.TagWord;
    Context->FloatSave.ErrorOffset = Nt5Context->FloatSave.ErrorOffset;
    Context->FloatSave.ErrorSelector = Nt5Context->FloatSave.ErrorSelector;
    Context->FloatSave.DataOffset = Nt5Context->FloatSave.DataOffset;
    Context->FloatSave.DataSelector = Nt5Context->FloatSave.DataSelector;
    Context->FloatSave.Cr0NpxState = Nt5Context->FloatSave.Cr0NpxState;

    for (i = 0; i < NUMBER_OF_FP_REGISTERS; i++) {
        RtlCopyMemory(&Context->FloatSave.RegisterArea[i*BYTES_PER_FX_REGISTER],
            &Nt5Context->FloatSave.RegisterArea[i*BYTES_PER_FP_REGISTER],
            BYTES_PER_FP_REGISTER);
    }
}
