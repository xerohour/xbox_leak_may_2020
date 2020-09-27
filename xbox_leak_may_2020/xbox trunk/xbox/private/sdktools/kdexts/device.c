/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    device.c

Abstract:

    WinDbg Extension Api

Author:

    Wesley Witt (wesw) 15-Aug-1993

Environment:

    User Mode.

Revision History:

--*/


#include "precomp.h"
#pragma hdrstop

#define FLAG_NAME(flag)           {flag, #flag}

DECLARE_API( devobj )

/*++

Routine Description:

    Dump a device object.

Arguments:

    args - the location of the device object to dump.

Return Value:

    None

--*/

{
    ULONG deviceToDump ;
    char deviceExprBuf[256] ;
    char *deviceExpr ;

    //
    // !devobj DeviceAddress DumpLevel
    //    where DeviceAddress can be an expression or device name
    //    and DumpLevel is a hex mask
    //
    strcpy(deviceExprBuf, "\\Device\\") ;
    deviceExpr = deviceExprBuf+strlen(deviceExprBuf) ;
    deviceToDump = 0 ;

    strcpy(deviceExpr, args) ;
    //
    // sscanf(args, "%s %lx", deviceExpr, &Flags);
    //

    //
    // The debugger will treat C0000000 as a symbol first, then a number if
    // no match comes up. We sanely reverse this ordering.
    //
    if (IsHexNumber(deviceExpr)) {

        sscanf(deviceExpr, "%lx", &deviceToDump) ;

    } else if (deviceExpr[0] == '\\') {

        deviceToDump = (ULONG) FindObjectByName( deviceExpr, NULL);

    } else if (isalpha(deviceExpr[0])) {

        //
        // Perhaps it's an object. Try with \\Device\\ prepended...
        //
        deviceToDump = (ULONG) FindObjectByName((PUCHAR) deviceExprBuf, NULL);
    }

    if (deviceToDump == 0) {

        //
        // Last try, is it an expression to evaluate?
        //
        deviceToDump = GetExpression( deviceExpr ) ;
    }


    if(deviceToDump == 0) {
        dprintf("Device object %s not found\n", args);
        return;
    }

    DumpDevice((PVOID) deviceToDump, 0, TRUE);
}


VOID
DumpDevice(
    PVOID DeviceAddress,
    ULONG FieldLength,
    BOOLEAN FullDetail
    )

/*++

Routine Description:

    Displays the driver name for the device object if FullDetail == FALSE.
    Otherwise displays more information about the device and the device queue.

Arguments:

    DeviceAddress - address of device object to dump.
    FieldLength   - Width of printf field for driver name (eg %11s).
    FullDetail    - TRUE means the device object name, driver name, and
                    information about Irps queued to the device.

Return Value:

    None

--*/

{
    ULONG                      result;
    ULONG                      i;
    DEVICE_OBJECT              deviceObject;
    PLIST_ENTRY                nextEntry;
    PVOID                      queueAddress;
    PIRP                       irp;
    KDEVICE_QUEUE_ENTRY        queueEntry;
    CHAR                       symbol[256];
    ULONG                      displacement;

    if ((!ReadMemory( (DWORD)DeviceAddress,
                     &deviceObject,
                     sizeof(deviceObject),
                     &result)) || (result < sizeof(deviceObject))) {
        dprintf("%08lx: Could not read device object\n", DeviceAddress);
        return;
    }

    if (deviceObject.Type != IO_TYPE_DEVICE) {
        dprintf("%08lx: is not a device object\n", DeviceAddress);
        return;
    }

    if (FullDetail == TRUE) {

        //
        // Dump the device name if present.
        //
        dprintf("Device object (%08lx) is for:\n ", DeviceAddress);

        DumpObjectName(DeviceAddress) ;
    }

    DumpDriver((PVOID) deviceObject.DriverObject, FieldLength, 0);

    if (FullDetail == TRUE) {
        //
        // Dump Irps related to driver.
        //

        GetSymbol((LPVOID)deviceObject.DriverObject,symbol,&displacement);

        dprintf(" DriverObject %08lx %s\n", deviceObject.DriverObject, symbol);

        dprintf("Current Irp %08lx RefCount %d Type %08lx Flags %08lx\n",
                deviceObject.CurrentIrp,
                deviceObject.ReferenceCount,
                deviceObject.DeviceType,
                deviceObject.Flags);

        if (deviceObject.MountedOrSelfDevice != DeviceAddress) {
            dprintf("Mounted %08lx ", deviceObject.MountedOrSelfDevice);
        }

        dprintf("DevExt %08lx\n", deviceObject.DeviceExtension);

        if (deviceObject.DeviceQueue.Busy) {

            ULONG listHead = (ULONG) DeviceAddress;

            listHead += FIELD_OFFSET(DEVICE_OBJECT, DeviceQueue.DeviceListHead);

            if ((ULONG) deviceObject.DeviceQueue.DeviceListHead.Flink == listHead) {
                dprintf("Device queue is busy -- Queue empty.\n");
            } else if (IsListEmpty(&deviceObject.DeviceQueue.DeviceListHead)) {
                dprintf("Device queue is busy -- Queue empty\n");
            } else if(deviceObject.DeviceQueue.DeviceListHead.Flink ==
                 deviceObject.DeviceQueue.DeviceListHead.Blink) {
                dprintf("Device queue is busy - Queue flink = blink\n");
            } else {
                dprintf("DeviceQueue: ");
                nextEntry = deviceObject.DeviceQueue.DeviceListHead.Flink;
                i = 0;

                while ((PCH) nextEntry != (PCH)
                    ((PCH) DeviceAddress +
                         ((PCH) &deviceObject.DeviceQueue.DeviceListHead.Flink -
                              (PCH) &deviceObject))) {
                    queueAddress = CONTAINING_RECORD(nextEntry,
                                                     KDEVICE_QUEUE_ENTRY,
                                                     DeviceListEntry);

                    if ((!ReadMemory((DWORD)queueAddress,
                                     &queueEntry,
                                     sizeof(queueEntry),
                                     &result)) || (result < sizeof(queueEntry))) {
                        dprintf("%08lx: Could not read queue entry\n", DeviceAddress);
                        return;
                    }

                    nextEntry = queueEntry.DeviceListEntry.Flink;

                    irp = CONTAINING_RECORD(queueAddress,
                                            IRP,
                                            Tail.Overlay.DeviceQueueEntry);

                    dprintf("%08lx%s",
                            irp,
                            (i & 0x03) == 0x03 ? "\n\t     " : " ");
                    if (CheckControlC()) {
                        break;
                    }
                }
                dprintf("\n");
            }
        } else {
            dprintf("Device queue is not busy.\n");
        }
    }
}

VOID
DumpObjectName(
   PVOID ObjectAddress
   )
{
   POBJECT_HEADER_NAME_INFO   pNameInfo;
   OBJECT_HEADER_NAME_INFO    NameInfo;
   PUCHAR                     buffer;
   OBJECT_STRING              objectString;
   ULONG                      result;

   pNameInfo = OBJECT_TO_OBJECT_HEADER_NAME_INFO(ObjectAddress);
   if (ReadMemory((DWORD)pNameInfo,
                  &NameInfo,
                  sizeof(NameInfo),
                  &result) && (result == sizeof(NameInfo))) {
       buffer = LocalAlloc(LPTR, NameInfo.Name.MaximumLength);
       if (buffer != NULL) {
           objectString.MaximumLength = NameInfo.Name.MaximumLength;
           objectString.Length = NameInfo.Name.Length;
           objectString.Buffer = (POSTR)buffer;
           if (ReadMemory((DWORD)NameInfo.Name.Buffer,
                          buffer,
                          objectString.Length,
                          &result) && (result == objectString.Length)) {
               dprintf("%Z", &objectString);
           }
           LocalFree(buffer);
       }
   }
}

VOID
DumpFlags(
    ULONG Depth,
    LPSTR FlagDescription,
    ULONG Flags,
    PFLAG_NAME FlagTable
    )
{
    ULONG i;
    ULONG mask = 0;
    ULONG count = 0;

    UCHAR prolog[64];

    sprintf(prolog, "%s (%#010x)  ", FlagDescription, Flags);

    xdprintf(Depth, "%s", prolog);

    if (Flags == 0) {
        dprintf("\n");
        return;
    }

    memset(prolog, ' ', strlen(prolog));

    for (i = 0; FlagTable[i].Name != 0; i++) {

        PFLAG_NAME flag = &(FlagTable[i]);

        mask |= flag->Flag;

        if ((Flags & flag->Flag) == flag->Flag) {

            //
            // print trailing comma
            //

            if (count != 0) {

                dprintf(", ");

                //
                // Only print two flags per line.
                //

                if ((count % 2) == 0) {
                    dprintf("\n");
                    xdprintf(Depth, "%s", prolog);
                }
            }

            dprintf("%s", flag->Name);

            count++;
        }
    }

    dprintf("\n");

    if ((Flags & (~mask)) != 0) {
        xdprintf(Depth, "%sUnknown flags %#010lx\n", prolog, (Flags & (~mask)));
    }

    return;
}


BOOLEAN
xReadMemory(
    PVOID S,
    PVOID D,
    ULONG Len
    )
{
    ULONG result;

    return (ReadMemory((DWORD)S, D, Len, &result) && (result == Len));
}

static CCHAR DebugBuffer[300];

VOID
xdprintf(
    ULONG  Depth,
    PCCHAR S,
    ...
    )
{
    va_list ap;
    ULONG   i;

    for (i=0; i<Depth; i++) {
        dprintf ("  ");
    }

    va_start(ap, S);

    vsprintf(DebugBuffer, S, ap);

    dprintf(DebugBuffer);

    va_end(ap);
}


