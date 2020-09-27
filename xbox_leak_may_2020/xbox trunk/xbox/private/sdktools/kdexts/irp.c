/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    irp.c

Abstract:

    WinDbg Extension Api

Author:

    Ramon J San Andres (ramonsa) 5-Nov-1993

Environment:

    User Mode.

Revision History:

--*/


#include "precomp.h"
#include "irpverif.h"
#pragma hdrstop

typedef struct _POOL_BLOCK_HEAD {
    POOL_HEADER Header;
    LIST_ENTRY List;
} POOL_BLOCK_HEAD, *PPOOL_BLOCK_HEADER;

typedef struct _POOL_HACKER {
    POOL_HEADER Header;
    ULONG Contents[8];
} POOL_HACKER;

typedef
BOOLEAN
(WINAPI *IRP_FILTER_ROUTINE)(
    PIRP Irp,
    PVOID FilterContext
    );

typedef struct _IRP_FILTER {
    IRP_FILTER_ROUTINE FilterRoutine;
    PVOID              FilterContext;
} IRP_FILTER, *PIRP_FILTER;

typedef struct _SEARCH_CONTEXT {
    ULONG FirstTime;
    IRP_FILTER Filter;
} SEARCH_CONTEXT, *PSEARCH_CONTEXT;

#define TAG 0
#define NONPAGED_ALLOC 1
#define NONPAGED_FREE 2
#define PAGED_ALLOC 3
#define PAGED_FREE 4
#define NONPAGED_USED 5
#define PAGED_USED 6

#define IRPBUFSIZE  (sizeof(IRP) + (5 * sizeof(IO_STACK_LOCATION)))

VOID
DumpIrp(
    PVOID IrpToDump,
    ULONG DumpLevel
    );

BOOLEAN
IrpFilterUserEvent(
    PIRP Irp,
    PVOID FilterContext
    );

BOOLEAN
IrpFilterDevice(
    PIRP Irp,
    PVOID FilterContext
    );

BOOLEAN
IrpFilterFileObject(
    PIRP Irp,
    PVOID FilterContext
    );

BOOLEAN
IrpFilterThread(
    PIRP Irp,
    PVOID FilterContext
    );

BOOLEAN
IrpFilterMdlProcess(
    PIRP Irp,
    PVOID FilterContext
    );

BOOLEAN
IrpFilterArg(
    PIRP Irp,
    PVOID FilterContext
    );

DECLARE_API( irp )

/*++

Routine Description:

   Dumps the specified Irp

Arguments:

    args - Address

Return Value:

    None

--*/

{
    ULONG irpToDump;
    ULONG dumpLevel = 0 ;
    BOOLEAN isCheckedBuild ;
    char irpExprBuf[256] ;
    char dumpLevelBuf[256] ;

    if (!*args) {
        irpToDump = EXPRLastDump;
    } else {

        //
        // !Irp IrpAddress DumpLevel
        //    where IrpAddress can be an expression
        //    and DumpLevel is a decimal level of any non-decimal string for 1
        irpExprBuf[0] = '\0' ;
        dumpLevelBuf[0] = '\0' ;

        sscanf(args, "%s %s", irpExprBuf, dumpLevelBuf);

        if (irpExprBuf) {

            if (IsHexNumber(irpExprBuf)) {

               sscanf(irpExprBuf, "%lx", &irpToDump) ;
            } else {

               irpToDump = GetExpression( irpExprBuf ) ;
               if (irpToDump==0) {

                  dprintf("An error occured trying to evaluate the expression\n") ;
                  return ;
               }
            }

            if (IsDecNumber(dumpLevelBuf)) {

               sscanf(dumpLevelBuf, "%d", &dumpLevel) ;
            } else if (dumpLevelBuf[0]) {

               dumpLevel = 1 ;
            } else {

               dumpLevel = 0 ;
            }
        }
    }

    if (irpToDump == 0) {

       if (IsCheckedBuild(&isCheckedBuild) && isCheckedBuild) {

          //DumpAllTrackedIrps() ;

       } else {

          dprintf("Free build - use !irpfind to scan memory for any active IRPs\n") ;
       }

    } else {

       DumpIrp((PUCHAR)irpToDump, (ULONG) dumpLevel);
    }
}


DECLARE_API( irpzone )

/*++

Routine Description:

    Dumps both the small irp zone and the large irp zone.  Only irps that
    are currently allocated are dumped.  "args" controls the type of dump.
    If "args" is present then the Irp is sent to the DumpIrp routine to be
    disected.  Otherwise, only the irp, its thread and the driver holding the
    irp (i.e. the driver of the last stack) is printed.

Arguments:

    args - a string pointer.  If anything is in the string it indicates full
           information (i.e. call DumpIrp).

Return Value:

    None.

--*/

{
    ULONG   listAddress;
    BOOLEAN fullOutput = FALSE;

    dprintf("irpzone is no longer supported.  Use irpfind to search "   \
            "nonpaged pool for active Irps\n");

    return;

}



VOID
DumpIrp(
    PVOID IrpToDump,
    ULONG DumpLevel
    )

/*++

Routine Description:

    This routine dumps an Irp.  It does not check to see that the address
    supplied actually locates an Irp.  This is done to allow for dumping
    Irps post mortem, or after they have been freed or completed.

Arguments:

    IrpToDump - the address of the irp.
    DumpLevel - 0 Summary
                1 Extended information
                2 Debug tracking info iff available

Return Value:

    None

--*/

{
    IO_STACK_LOCATION   irpStack;
    PCHAR               buffer;
    ULONG               irpStackAddress;
    ULONG               result;
    IRP                 irp;
    CCHAR               irpStackIndex;
    LARGE_INTEGER       runTime ;
    BOOLEAN             isCheckedBuild, delayed ;

    if ( !ReadMemory( (DWORD) IrpToDump,
                      &irp,
                      sizeof(irp),
                      &result) ) {
        dprintf("%08lx: Could not read Irp\n", IrpToDump);
        return;
    }

    if (irp.Type != IO_TYPE_IRP) {
        dprintf("IRP signature does not match, probably not an IRP\n");
        return;
    }

    dprintf("Irp is active with %d stacks %d is current (= %#08lx)\n",
            irp.StackCount,
            irp.CurrentLocation,
            irp.Tail.Overlay.CurrentStackLocation);

    if ((irp.UserBuffer != NULL) && (irp.Type == IO_TYPE_IRP)) {
        dprintf(" UserBuffer = %08lx ", irp.UserBuffer);
    } else {
        dprintf(" No UserBuffer ");
    }

    dprintf("Thread %08lx:  ", irp.Tail.Overlay.Thread);

    if (irp.StackCount > 30) {
        dprintf("Too many Irp stacks to be believed (>30)!!\n");
        return;
    } else {
        if (irp.CurrentLocation > irp.StackCount) {
            dprintf("Irp is completed.  ");
        } else {
            dprintf("Irp stack trace.  ");
        }
    }

    if (irp.PendingReturned) {
        dprintf("Pending has been returned\n");
    } else {
        dprintf("\n");
    }

    if (DumpLevel>0)
    {
        dprintf("Flags = %08lx\n", irp.Flags);
        dprintf("ThreadListEntry.Flink = %08lx\n", irp.ThreadListEntry.Flink);
        dprintf("ThreadListEntry.Blink = %08lx\n", irp.ThreadListEntry.Blink);
        dprintf("IoStatus.Status = %08lx\n", irp.IoStatus.Status);
        dprintf("IoStatus.Information = %08lx\n", irp.IoStatus.Information);
        dprintf("Cancel = %02lx\n", irp.Cancel);
        dprintf("UserIosb = %08lx\n", irp.UserIosb);
        dprintf("UserEvent = %08lx\n", irp.UserEvent);
        dprintf("Overlay.AsynchronousParameters.UserApcRoutine = %08lx\n", irp.Overlay.AsynchronousParameters.UserApcRoutine);
        dprintf("Overlay.AsynchronousParameters.UserApcContext = %08lx\n", irp.Overlay.AsynchronousParameters.UserApcContext);
        dprintf(
            "Overlay.AllocationSize = %08lx - %08lx\n",
            irp.Overlay.AllocationSize.HighPart,
            irp.Overlay.AllocationSize.LowPart);
        dprintf("UserBuffer = %08lx\n", irp.UserBuffer);
        dprintf("&Tail.Overlay.DeviceQueueEntry = %08lx\n", &irp.Tail.Overlay.DeviceQueueEntry);
        dprintf("Tail.Overlay.Thread = %08lx\n", irp.Tail.Overlay.Thread);
        dprintf("Tail.Overlay.ListEntry.Flink = %08lx\n", irp.Tail.Overlay.ListEntry.Flink);
        dprintf("Tail.Overlay.ListEntry.Blink = %08lx\n", irp.Tail.Overlay.ListEntry.Blink);
        dprintf("Tail.Overlay.CurrentStackLocation = %08lx\n", irp.Tail.Overlay.CurrentStackLocation);
        dprintf("Tail.Overlay.OriginalFileObject = %08lx\n", irp.Tail.Overlay.OriginalFileObject);
        dprintf("Tail.Apc = %08lx\n", irp.Tail.Apc);
        dprintf("Tail.CompletionKey = %08lx\n", irp.Tail.CompletionKey);
    }

    irpStackAddress = (ULONG)IrpToDump + sizeof(irp);

    buffer = LocalAlloc(LPTR, 256);
    if (buffer == NULL) {
        dprintf("Can't allocate 256 bytes\n");
        return;
    }

    dprintf("     cmd  flg cl Device   File     Completion-Context\n");
    for (irpStackIndex = 1; irpStackIndex <= irp.StackCount; irpStackIndex++) {

        if ( !ReadMemory( (DWORD) irpStackAddress,
                          &irpStack,
                          sizeof(irpStack),
                          &result) ) {
            dprintf("%08lx: Could not read IrpStack\n", irpStackAddress);
            goto exit;
        }

        dprintf("%c[%3x,%2x]  %2x %2x %08lx %08lx %08lx-%08lx %s %s %s %s\n",
                irpStackIndex == irp.CurrentLocation ? '>' : ' ',
                irpStack.MajorFunction,
                irpStack.MinorFunction,
                irpStack.Flags,
                irpStack.Control,
                irpStack.DeviceObject,
                irpStack.FileObject,
                irpStack.CompletionRoutine,
                irpStack.Context,
                (irpStack.Control & SL_INVOKE_ON_SUCCESS) ? "Success" : "",
                (irpStack.Control & SL_INVOKE_ON_ERROR)   ? "Error"   : "",
                (irpStack.Control & SL_INVOKE_ON_CANCEL)  ? "Cancel"  : "",
                (irpStack.Control & SL_PENDING_RETURNED)  ? "pending"  : "");

        if (irpStack.DeviceObject != NULL) {
            dprintf("\t      ");
            DumpDevice(irpStack.DeviceObject, 0, FALSE);
        }

        if (irpStack.CompletionRoutine != NULL) {

            GetSymbol((LPVOID)irpStack.CompletionRoutine, buffer, &result);
            dprintf("\t%s\n", buffer);
        } else {
            dprintf("\n");
        }

        dprintf("\t\t\tArgs: %08lx %08lx %08lx %08lx\n",
                irpStack.Parameters.Others.Argument1,
                irpStack.Parameters.Others.Argument2,
                irpStack.Parameters.Others.Argument3,
                irpStack.Parameters.Others.Argument4);
        irpStackAddress += sizeof(irpStack);
        if (CheckControlC()) {
           goto exit;
        }
    }

    if (DumpLevel>=2) {

       if (IsCheckedBuild(&isCheckedBuild) && isCheckedBuild) {

       } else {

          dprintf("Extra information not available in free build.\n") ;
       }
    }

exit:
    LocalFree(buffer);
}


//+---------------------------------------------------------------------------
//
//  Function:   CheckForIrp
//
//  Synopsis:   Matches pool chunk against an irp
//
//  Arguments:  [Tag]            -- 
//              [Filter]         -- 
//              [Flags]          -- 0 nonpaged pool 1 paged pool 2 special pool 4 dump irp
//              [PoolTrackTable] -- 
//              [PoolHeader]     -- 
//              [BlockSize]      -- 
//              [Data]           -- 
//
//  Returns:    
//
//  History:    7-28-1999   benl   Created
//
//  Notes:
//
//----------------------------------------------------------------------------

BOOLEAN WINAPI CheckForIrp(
    PCHAR Tag,
    PCHAR Filter,
    ULONG Flags,
    PPOOL_HEADER PoolHeader,
    ULONG BlockSize,
    PVOID Data,
    PVOID Context
    )
{
    PIRP Irp;
    ULONG Result;
    PIO_STACK_LOCATION irpSp;
    PSEARCH_CONTEXT SearchContext = (PSEARCH_CONTEXT)Context;

    Irp = (PIRP) malloc( BlockSize );

    if(Irp == NULL) {
        dprintf("Unable to allocate irp sized buffer\n");
        return FALSE;
    }


    //
    //  If its a big pool page (no header) or special pool (Flags == 0x2) or
    //  its in regular pool and not free then try to dump 
    //  

    if ( ((PoolHeader == NULL) || 
          (Flags & 0x2) || 
          (PoolHeader->PoolType != 0)) 
        
        && (CheckSingleFilter( Tag, Filter ))) {

        if(ReadMemory((ULONG_PTR)Data,
                      Irp,
                      sizeof(DWORD),
                      &Result)) {

            if(Irp->Type == IO_TYPE_IRP)    {

                if (Flags & 0x4) {
                    if (SearchContext->FirstTime) {

                        dprintf("  Irp    [ Thread ] irpStack: (Mj,Mn)   DevObj  [Driver]\n");
                        SearchContext->FirstTime = FALSE;
                    }
                    dprintf("%08lx: ", Data);
                    DumpIrp((PUCHAR)Data, 0);
                    dprintf("\n");
                } else {
                    if((ReadMemory((ULONG_PTR)Data,
                                   Irp,
                                   BlockSize,
                                   &Result)) &&
                       (SearchContext->Filter.FilterRoutine == NULL) ||
                       (SearchContext->Filter.FilterRoutine(Irp, SearchContext->Filter.FilterContext))) {

                        if (SearchContext->FirstTime) {

                            dprintf("  Irp    [ Thread ] irpStack: (Mj,Mn)   DevObj  [Driver]\n");
                            SearchContext->FirstTime = FALSE;
                        }

                        irpSp = (PIO_STACK_LOCATION)
                                    (((PCHAR) Irp + sizeof(IRP)) +
                                    (Irp->CurrentLocation - 1) *
                                    sizeof(IO_STACK_LOCATION));

                        dprintf("%08lx [%08lx] ", Data,
                                Irp->Tail.Overlay.Thread);

                        if(Irp->CurrentLocation > Irp->StackCount) {
                            dprintf("Irp is complete (CurrentLocation "
                                    "%d > StackCount %d)",
                                    Irp->CurrentLocation,
                                    Irp->StackCount);
                        } else {
                            dprintf("irpStack: (%2x,%2x)",
                                    irpSp->MajorFunction,
                                    irpSp->MinorFunction);

                            dprintf("  %08lx [", irpSp->DeviceObject);
                            DumpDevice(irpSp->DeviceObject, 0, FALSE);
                            dprintf("]");
                        }

                        dprintf("\n");
                    }
                }
            } else {
                // dprintf("%08lx (size %04lx) uninitialized or overwritten IRP\n",
                //         irpAddress,
                //         PoolBlock.Header.BlockSize << POOL_BLOCK_SHIFT);
            }
        } else {
            dprintf("Possible IRP @ %lx - unable to read addr\n", Data );
        }

        free( Irp );
        return TRUE;

    } else {
#ifdef SHOW_PROGRESS
        dprintf("%c", turnTable[turn]);
        turn = (turn + 1) % 4;
#endif
    }
    
    free( Irp );
    return FALSE;
} // CheckForIrp


DECLARE_API(irpfind)

/*++

Routine Description:

    finds Irps in non-paged pool

Arguments:

    args -

Return Value:

    None

--*/


{
    PCHAR       RestartAddr = 0;
    ULONG       TagName;
    UCHAR       Field[20];
    ULONG       Match=0;
    SEARCH_CONTEXT Context;

    Context.FirstTime = TRUE;
    Context.Filter.FilterRoutine = NULL;
    Field[0] = '\0';
    if (args) {
        sscanf(args, "%x %19s %x", &RestartAddr, &Field, &Match);
    }
    if ((_stricmp(Field, "userevent") == 0) &&
        (Match != 0)) {
        Context.Filter.FilterRoutine = IrpFilterUserEvent;
        Context.Filter.FilterContext = (PVOID)Match;
        dprintf("Looking for IRP with UserEvent == %08lx\n",Match);
    } else if ((_stricmp(Field, "device") == 0) &&
        (Match != 0)) {
        Context.Filter.FilterRoutine = IrpFilterDevice;
        Context.Filter.FilterContext = (PVOID)Match;
        dprintf("Looking for IRPs with device object == %08lx\n",Match);
    } else if ((_stricmp(Field, "fileobject") == 0) &&
        (Match != 0)) {
        Context.Filter.FilterRoutine = IrpFilterFileObject;
        Context.Filter.FilterContext = (PVOID)Match;
        dprintf("Looking for IRPs with file object == %08lx\n",Match);
    } else if ((_stricmp(Field, "thread") == 0) &&
        (Match != 0)) {
        Context.Filter.FilterRoutine = IrpFilterThread;
        Context.Filter.FilterContext = (PVOID)Match;
        dprintf("Looking for IRPs with thread == %08lx\n",Match);
    } else if ((_stricmp(Field, "arg") == 0) &&
        (Match != 0)) {
        Context.Filter.FilterRoutine = IrpFilterArg;
        Context.Filter.FilterContext = (PVOID)Match;
        dprintf("Looking for IRPs with arg == %08lx\n",Match);
    }

    TagName = '?prI';

    SearchPool( TagName, (ULONG)RestartAddr, &CheckForIrp, &Context );
    return;
}


BOOLEAN
IrpFilterUserEvent(
    IN PIRP Irp,
    IN PVOID FilterContext
    )
/*++

Routine Description:

    Checks to see if the userevent field of an IRP matches the supplied
    parameter

Arguments:

    Irp - Supplies the irp to filter

    FilterContext - supplies the user event

Return Value:

    TRUE if the specified irp has userevent == FilterContext
    FALSE otherwise

--*/

{
    PKEVENT pEvent = (PKEVENT)FilterContext;

    if (Irp->UserEvent == pEvent) {
        return(TRUE);
    } else {
        return(FALSE);
    }
}


BOOLEAN
IrpFilterDevice(
    IN PIRP Irp,
    IN PVOID FilterContext
    )
/*++

Routine Description:

    Checks to see if the specified IRP matches the supplied
    device object

Arguments:

    Irp - Supplies the irp to filter

    FilterContext - supplies the device object

Return Value:

    TRUE if the specified irp has a device == FilterContext
    FALSE otherwise

--*/

{
    PIO_STACK_LOCATION IrpStack = (PIO_STACK_LOCATION)(Irp+1);
    LONG i;

    if (Irp->StackCount > 30) {
        return(FALSE);
    }

    for (i=0; i<Irp->StackCount; i++) {
        if (IrpStack[i].DeviceObject == FilterContext) {
            return(TRUE);
        }
    }
    return(FALSE);
}


BOOLEAN
IrpFilterFileObject(
    IN PIRP Irp,
    IN PVOID FilterContext
    )
/*++

Routine Description:

    Checks to see if the Tail.Overlay.OriginalFileObject field of an IRP matches the 
    supplied parameter

Arguments:

    Irp - Supplies the irp to filter

    FilterContext - supplies the file object

Return Value:

    TRUE if the specified irp has userevent == FilterContext
    FALSE otherwise

--*/

{
    PFILE_OBJECT pFile = (PFILE_OBJECT)FilterContext;

    if (Irp->Tail.Overlay.OriginalFileObject == pFile) {
        return(TRUE);
    } else {
        return(FALSE);
    }
}


BOOLEAN
IrpFilterThread(
    IN PIRP Irp,
    IN PVOID FilterContext
    )
/*++

Routine Description:

    Checks to see if the Tail.Overlay.OriginalFileObject field of an IRP matches the 
    supplied parameter

Arguments:

    Irp - Supplies the irp to filter

    FilterContext - supplies the file object

Return Value:

    TRUE if the specified irp has userevent == FilterContext
    FALSE otherwise

--*/

{
    PETHREAD pThread = (PETHREAD)FilterContext;

    if (Irp->Tail.Overlay.Thread == pThread) {
        return(TRUE);
    } else {
        return(FALSE);
    }
}


BOOLEAN
IrpFilterArg(
    IN PIRP Irp,
    IN PVOID FilterContext
    )
/*++

Routine Description:

    Checks to see if the specified IRP matches the supplied
    argument

Arguments:

    Irp - Supplies the irp to filter

    FilterContext - supplies the argument to match

Return Value:

    TRUE if the specified irp has argument == FilterContext
    FALSE otherwise

--*/

{
    PIO_STACK_LOCATION IrpStack = (PIO_STACK_LOCATION)(Irp+1);
    LONG i;

    if (Irp->StackCount > 30) {
        return(FALSE);
    }

    for (i=0; i<Irp->StackCount; i++) {
        if ((IrpStack[i].Parameters.Others.Argument1 == FilterContext) ||
            (IrpStack[i].Parameters.Others.Argument2 == FilterContext) ||
            (IrpStack[i].Parameters.Others.Argument3 == FilterContext) ||
            (IrpStack[i].Parameters.Others.Argument4 == FilterContext)) {
            return(TRUE);
        }
    }
    return(FALSE);
}
