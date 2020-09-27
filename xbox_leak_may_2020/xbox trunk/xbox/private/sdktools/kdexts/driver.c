/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    driver.c

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

#include <time.h>

DumpImage(
    ULONG Base,
    BOOL DoHeaders,
    BOOL DoSections
    );

PUCHAR DispatchRoutineTable[]=
{
    "IRP_MJ_CREATE",
    "IRP_MJ_CREATE_NAMED_PIPE",
    "IRP_MJ_CLOSE",
    "IRP_MJ_READ",
    "IRP_MJ_WRITE",
    "IRP_MJ_QUERY_INFORMATION",
    "IRP_MJ_SET_INFORMATION",
    "IRP_MJ_QUERY_EA",
    "IRP_MJ_SET_EA",
    "IRP_MJ_FLUSH_BUFFERS",
    "IRP_MJ_QUERY_VOLUME_INFORMATION",
    "IRP_MJ_SET_VOLUME_INFORMATION",
    "IRP_MJ_DIRECTORY_CONTROL",
    "IRP_MJ_FILE_SYSTEM_CONTROL",
    "IRP_MJ_DEVICE_CONTROL",
    "IRP_MJ_INTERNAL_DEVICE_CONTROL",
    "IRP_MJ_SHUTDOWN",
    "IRP_MJ_LOCK_CONTROL",
    "IRP_MJ_CLEANUP",
    "IRP_MJ_CREATE_MAILSLOT",
    "IRP_MJ_QUERY_SECURITY",
    "IRP_MJ_SET_SECURITY",
    "IRP_MJ_POWER",
    "IRP_MJ_SYSTEM_CONTROL",
    "IRP_MJ_DEVICE_CHANGE",
    "IRP_MJ_QUERY_QUOTA",
    "IRP_MJ_SET_QUOTA",
    "IRP_MJ_PNP",
    NULL
} ;

//
// Change this value and update the above table if IRP_MJ_MAXIMUM_FUNCTION
// is increased.
//
#define IRP_MJ_MAXIMUM_FUNCTION_HANDLED 0x1b

DECLARE_API( drvobj )

/*++

Routine Description:

    Dump a driver object.

Arguments:

    args - the location of the driver object to dump.

Return Value:

    None

--*/

{
    ULONG driverToDump;
    ULONG Flags;
    char driverExprBuf[256] ;
    char *driverExpr ;

    //
    // !drvobj DriverAddress DumpLevel
    //    where DriverAddress can be an expression or driver name
    //    and DumpLevel is a hex mask
    //
    strcpy(driverExprBuf, "\\Driver\\") ;
    driverExpr = driverExprBuf+strlen(driverExprBuf) ;
    Flags = 1;
    driverToDump = 0 ;

    sscanf(args, "%s %lx", driverExpr, &Flags);

    //
    // The debugger will treat C0000000 as a symbol first, then a number if
    // no match comes up. We sanely reverse this ordering.
    //
    if (IsHexNumber(driverExpr)) {

        sscanf(driverExpr, "%lx", &driverToDump) ;

    } else if (driverExpr[0] == '\\') {

        driverToDump = (ULONG) FindObjectByName( driverExpr, NULL);
    
    } else if (isalpha(driverExpr[0])) {

        //
        // Perhaps it's an object. Try with \\Driver\\ prepended...
        // 
        driverToDump = (ULONG) FindObjectByName((PUCHAR) driverExprBuf, NULL);
    }
                      
    if (driverToDump == 0) {
           
        //
        // Last try, is it an expression to evaluate?
        //
        driverToDump = GetExpression( driverExpr ) ;
    }

    if(driverToDump == 0) {
        dprintf("Driver object %s not found\n", args);
        return;
    }

    dprintf("Driver object (%08lx) is for:\n", driverToDump);
    DumpDriver((PVOID) driverToDump, 0, Flags);
}

VOID
DumpDriver(
    PVOID DriverAddress,
    ULONG FieldWidth,
    ULONG Flags
    )

/*++

Routine Description:

    Displays the driver name and the list of device objects created by
    the driver.

Arguments:

    DriverAddress - addres of the driver object to dump.
    FieldWidth    - Width of printf field (eg %11s) for driver name. 
                    Use 0 for full display.
    Flags         - Bit 0, Dump out device objects owned by driver
                    Bit 1, Dump out dispatch routines for driver

Return Value:

    None

--*/

{
    DRIVER_OBJECT    driverObject;
    ULONG            result;
    ULONG            i,j;
    PUCHAR           buffer;
    PVOID            deviceAddress;
    DEVICE_OBJECT    deviceObject;
    UNICODE_STRING   unicodeString;
    ULONG            displacement;
    UCHAR            component[512];
    PUCHAR           *dispatchTableText ;
    LPVOID           *p;

    if ((!ReadMemory( (DWORD) DriverAddress,
                     &driverObject,
                     sizeof(driverObject),
                     &result)) || result < sizeof(driverObject)) {
        return;
    }

    if (Flags&0x2) {
           
        dprintf ("\nDispatch routines:\n");        
        dispatchTableText = DispatchRoutineTable ;
        for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION_HANDLED; i++) {
            
            GetSymbol((LPVOID)(driverObject.MajorFunction[i]), component, &displacement);
            
            //
            // Forms are:
            // [1b] IRP_MJ_PNP            C0000000  DispatchHandler+30
            // [1b] IRP_MJ_PNP            C0000000  DispatchHandler
            // [1b] ???                   C0000000  <either of above>  
            //
            if (*dispatchTableText) {
               dprintf("[%02x] %s", i, *dispatchTableText) ;
               j=strlen(*dispatchTableText) ;
            } else {
               dprintf("[%02x] ???") ;
               j=3 ;
            }

            while(j++<35) dprintf(" ") ;
            dprintf("%8.8x\t%s", driverObject.MajorFunction[i], component) ;
            
            if (displacement) {
          
                dprintf("+0x%x\n", displacement) ;
            } else {
     
                dprintf("\n") ;
            }  
            
            if (*dispatchTableText) {
               
                dispatchTableText++ ;
            }
        }
        dprintf("\n");
    }
}

UCHAR *PagedOut = {"Header Paged Out"};

DECLARE_API( drivers )

/*++

Routine Description:

    Displays physical memory usage by driver.

Arguments:

    None.

Return Value:

    None.

--*/

{
    LIST_ENTRY List;
    PLIST_ENTRY Next;
    ULONG ListHead;
    NTSTATUS Status = 0;
    ULONG Result;
    PLDR_DATA_TABLE_ENTRY DataTable;
    LDR_DATA_TABLE_ENTRY DataTableBuffer;
    WCHAR UnicodeBuffer[128];
    UNICODE_STRING BaseName;
    PIMAGE_NT_HEADERS NtHeader;
    PIMAGE_DOS_HEADER DosHeader;
    ULONG SizeOfData;
    ULONG SizeOfCode;
    ULONG SizeOfLocked;
    ULONG TotalCode = 0;
    ULONG TotalData = 0;
    ULONG TotalValid = 0;
    ULONG TotalTransition = 0;
    ULONG DosHeaderSize;
    ULONG TimeDateStamp;
    PUCHAR time;
    ULONG Flags;

    Flags = 0;
    sscanf(args,"%lx",&Flags);

    ListHead = GetNtDebuggerData( PsLoadedModuleList );

    if (!ListHead) {
        dprintf("Couldn't get offset of PsLoadedModuleListHead\n");
        return;
    } else {
        if ((!ReadMemory((DWORD)ListHead,
                         &List,
                         sizeof(LIST_ENTRY),
                         &Result)) || (Result < sizeof(LIST_ENTRY))) {
            dprintf("Unable to get value of PsLoadedModuleListHead\n");
            return;
        }
    }

    dprintf("Loaded System Driver Summary\n\n");
    if (Flags & 1) {
        dprintf("Base       Code Size       Data Size       Resident  Standby   Driver Name\n");
    } else if (Flags & 2) {
        dprintf("Base       Code  Data  Locked  Resident  Standby  Loader Entry  Driver Name\n");
    } else {
        dprintf("Base       Code Size       Data Size       Driver Name       Creation Time\n");
    }

    Next = List.Flink;
    if (Next == NULL) {
        dprintf("PsLoadedModuleList is NULL!\n");
        return;
    }

    while ((ULONG)Next != ListHead) {
        if (CheckControlC()) {
            return;
        }
        DataTable = CONTAINING_RECORD(Next,
                                      LDR_DATA_TABLE_ENTRY,
                                      InLoadOrderLinks);
        if ((!ReadMemory((DWORD)DataTable,
                         &DataTableBuffer,
                         sizeof(LDR_DATA_TABLE_ENTRY),
                         &Result)) || (Result < sizeof(LDR_DATA_TABLE_ENTRY))) {
            dprintf("Unable to read LDR_DATA_TABLE_ENTRY at %08lx - status %08lx\n",
                    DataTable,
                    Status);
            return;
        }

        //
        // Get the base DLL name.
        //
        if ((!ReadMemory((DWORD)DataTableBuffer.BaseDllName.Buffer,
                         UnicodeBuffer,
                         DataTableBuffer.BaseDllName.Length,
                         &Result)) || (Result < DataTableBuffer.BaseDllName.Length)) {
            dprintf("Unable to read name string at %08lx - status %08lx\n",
                    DataTable,
                    Status);
            return;
        }

        BaseName.Buffer = UnicodeBuffer;
        BaseName.Length = BaseName.MaximumLength = (USHORT)Result;

        DosHeader = (PIMAGE_DOS_HEADER)DataTableBuffer.DllBase;

        if ((!ReadMemory((DWORD)&DosHeader->e_lfanew,
                         &DosHeaderSize,
                         sizeof(ULONG),
                         &Result)) || (Result < sizeof(ULONG))) {
            //dprintf("Unable to read DosHeader at %08lx - status %08lx\n",
            //        &DosHeader->e_lfanew,
            //        Status);

            SizeOfCode = 0;
            SizeOfData = 0;
            SizeOfLocked = -1;
            time = PagedOut;
        } else {

            NtHeader = (PIMAGE_NT_HEADERS)((ULONG)DosHeader + DosHeaderSize);

            if ((!ReadMemory((DWORD)&(NtHeader->OptionalHeader.SizeOfCode),
                             &SizeOfCode,
                             sizeof(ULONG),
                             &Result)) || (Result < sizeof(ULONG))) {
                dprintf("Unable to read DosHeader at %08lx - status %08lx\n",
                        &(NtHeader->OptionalHeader.SizeOfCode),
                        Status);
                goto getnext;
            }

            if ((!ReadMemory((DWORD)&(NtHeader->OptionalHeader.SizeOfInitializedData),
                             &SizeOfData,
                             sizeof(ULONG),
                             &Result)) || (Result < sizeof(ULONG))) {
                dprintf("Unable to read DosHeader at %08lx - status %08lx\n",
                        &(NtHeader->OptionalHeader.SizeOfCode),
                        Status);
                goto getnext;
            }

            if ((!ReadMemory((DWORD)&(NtHeader->FileHeader.TimeDateStamp),
                             &TimeDateStamp,
                             sizeof(ULONG),
                             &Result)) || (Result < sizeof(ULONG))) {
                dprintf("Unable to read DosHeader at %08lx - status %08lx\n",
                        &(NtHeader->FileHeader.TimeDateStamp),
                        Status);
                goto getnext;
            }

            time = ctime((time_t *)&TimeDateStamp);
            time[strlen(time)-1] = 0;
        }

        if (Flags & 1) {
            PCHAR Va;
            PCHAR EndVa;
            ULONG States[3] = {0,0,0};

            Va = DataTableBuffer.DllBase;
            EndVa = Va + DataTableBuffer.SizeOfImage;

            while (Va < EndVa) {
                States[GetAddressState((PVOID)Va)] += PAGE_SIZE/1024;
                Va += PAGE_SIZE;
            }
            dprintf("%08lx %6lx (%4ld kb) %6lx (%4ld kb) (%5ld kb %5ld kb) %12wZ\n",
                     DataTableBuffer.DllBase,
                     SizeOfCode,
                     SizeOfCode / 1024,
                     SizeOfData,
                     SizeOfData / 1024,
                     States[ADDRESS_VALID],
                     States[ADDRESS_TRANSITION],
                     &BaseName);
            TotalValid += States[ADDRESS_VALID];
            TotalTransition += States[ADDRESS_TRANSITION];
        } else if (Flags & 2) {
            ULONG i;
            ULONG SizeToLock;
            PMMPTE PointerPte;
            PMMPTE LastPte;
            PVOID BaseAddress;
            PCHAR Va;
            PCHAR EndVa;
            ULONG States[3] = {0,0,0};
            PIMAGE_SECTION_HEADER NtSection;
            IMAGE_SECTION_HEADER NtSectionData;
            IMAGE_FILE_HEADER FileHeader;

            Va = DataTableBuffer.DllBase;
            EndVa = Va + DataTableBuffer.SizeOfImage;

            while (Va < EndVa) {
                States[GetAddressState((PVOID)Va)] += PAGE_SIZE/1024;
                Va += PAGE_SIZE;
            }

            SizeOfLocked = 0;

            //
            // Read the sections in the executable header to see which are
            // locked.  Don't bother looking for refcounted PFNs.
            //
        
            NtHeader = (PIMAGE_NT_HEADERS)((ULONG)DosHeader + DosHeaderSize);

            if ((!ReadMemory((DWORD)&(NtHeader->FileHeader),
                             &FileHeader,
                             sizeof(FileHeader),
                             &Result)) || (Result < sizeof(ULONG))) {
                dprintf("Unable to read FileHeader at %08lx - status %08lx\n",
                        &(NtHeader->FileHeader),
                        Status);
                goto getnext;
            }

            NtSection = (PIMAGE_SECTION_HEADER)((ULONG_PTR)NtHeader +
                                sizeof(ULONG) +
                                sizeof(IMAGE_FILE_HEADER) +
                                FileHeader.SizeOfOptionalHeader
                                );
        
            for (i = 0; i < FileHeader.NumberOfSections; i += 1) {
        
                if ((!ReadMemory((DWORD)NtSection,
                                 &NtSectionData,
                                 sizeof(NtSectionData),
                                 &Result)) || (Result < sizeof(ULONG))) {
                    dprintf("Unable to read NtSectionData at %08lx - status %08lx\n",
                            NtSection,
                            Status);
                    goto getnext;
                }

                if ((NtSectionData.NumberOfLinenumbers == 1) ||
                    (NtSectionData.NumberOfLinenumbers == 2)) {

                    BaseAddress = (PVOID)(NtSectionData.PointerToRelocations);
                    SizeToLock = NtSectionData.SizeOfRawData;
                    PointerPte = DbgGetPteAddress(BaseAddress);
                    LastPte = DbgGetPteAddress((PVOID)((PCHAR)BaseAddress + SizeToLock - 1));
                    SizeOfLocked += (LastPte - PointerPte + 1);
                }

                NtSection += 1;
            }

#if 0
        dprintf("Base       Code  Data  Locked  Resident  Standby  Loader Entry  Driver Name\n");
#endif

            dprintf("%08lx %6lx %6lx %6lx  %6lx   %6lx    %8lx      %12wZ\n",
                     DataTableBuffer.DllBase,
                     SizeOfCode,
                     SizeOfData,
                     SizeOfLocked,
                     States[ADDRESS_VALID],
                     States[ADDRESS_TRANSITION],
                     DataTable,
                     &BaseName);
            TotalValid += States[ADDRESS_VALID];
            TotalTransition += States[ADDRESS_TRANSITION];
        } else {
             dprintf("%08lx %6lx (%4ld kb) %5lx (%3ld kb) %12wZ  %s\n",
                      DataTableBuffer.DllBase,
                      SizeOfCode,
                      SizeOfCode / 1024,
                      SizeOfData,
                      SizeOfData / 1024,
                      &BaseName,
                      time);
        }

        if (Flags & 4) {
            DumpImage((ULONG)DataTableBuffer.DllBase,
                     (Flags & 2) == 2,
                     (Flags & 4) == 4
                     );
        }

        TotalCode += SizeOfCode;
        TotalData += SizeOfData;
getnext:
        Next = DataTableBuffer.InLoadOrderLinks.Flink;
    }

    dprintf("TOTAL:   %6lx (%4ld kb) %6lx (%4ld kb) (%5ld kb %5ld kb)\n",
            TotalCode,
            TotalCode / 1024,
            TotalData,
            TotalData / 1024,
            TotalValid,
            TotalTransition);

}
