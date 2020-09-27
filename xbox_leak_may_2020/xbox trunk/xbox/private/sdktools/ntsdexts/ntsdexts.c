/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    ntsdexts.c

Abstract:

    This function contains the default ntsd debugger extensions

Author:

    Mark Lucovsky (markl) 09-Apr-1991

Revision History:

--*/

#include "ntsdextp.h"

WINDBG_EXTENSION_APIS ExtensionApis;
HANDLE ExtensionCurrentProcess;


NTSTATUS
QueryTebAddress(
    HANDLE hCurrentThread,
    PULONGLONG Teb
    )
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    THREAD_BASIC_INFORMATION ThreadInformation;
    ULONGLONG Address;

    if (ExtensionApis.nSize >= FIELD_OFFSET(WINDBG_EXTENSION_APIS, lpIoctlRoutine)) {
        Address = 0;
        GetTebAddress(&Address);
        if (Address) {
            *Teb = Address;
            Status = STATUS_SUCCESS;
        }
    }

    if (!NT_SUCCESS(Status)) {
        Status = NtQueryInformationThread( hCurrentThread,
                                           ThreadBasicInformation,
                                           &ThreadInformation,
                                           sizeof( ThreadInformation ),
                                           NULL
                                         );

        if (NT_SUCCESS( Status )) {
            *Teb = (ULONGLONG)ThreadInformation.TebBaseAddress;
        }
    }
    return Status;
}

NTSTATUS
QueryPebAddress(
    HANDLE hCurrentThread,
    PULONGLONG Peb
    )
{
    PROCESS_BASIC_INFORMATION ProcessInformation;
    ULONGLONG Address;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    DWORD cb;
    TEB Teb;

    if (ExtensionApis.nSize >= FIELD_OFFSET(WINDBG_EXTENSION_APIS, lpIoctlRoutine)) {
        //
        // query the debugger instead of the system
        //
        Address = 0;
        QueryTebAddress(hCurrentThread, &Address);
        if (Address) {
            ReadMemory(Address, &Teb, sizeof(Teb), &cb);
            *Peb = (ULONGLONG)Teb.ProcessEnvironmentBlock;
            Status = STATUS_SUCCESS;
        }
    }

    if (!NT_SUCCESS(Status)) {

        Status = NtQueryInformationProcess( ExtensionCurrentProcess,
                                            ProcessBasicInformation,
                                            &ProcessInformation,
                                            sizeof( ProcessInformation ),
                                            NULL
                                          );
        if (NT_SUCCESS( Status )) {
            *Peb = (ULONGLONG)ProcessInformation.PebBaseAddress;
        }

    }
    return Status;
}

BOOL
QueryPeb(
    HANDLE hCurrentThread,
    PPEB ThePeb
    )
{
    BOOL b;
    NTSTATUS Status;
    PROCESS_BASIC_INFORMATION ProcessInformation;
    ULONGLONG PebAddress;

    Status = QueryPebAddress(hCurrentThread, &PebAddress);

    if (!NT_SUCCESS( Status )) {
        b = FALSE;
    } else {
        b = ReadMemory( PebAddress,
                        ThePeb,
                        sizeof(*ThePeb),
                        NULL
                      );
    }

    if ( !b ) {
        dprintf("    Unable to read Process PEB\n" );
        memset( ThePeb, 0, sizeof( *ThePeb ) );
    }

    return b;
}


VOID
DecodeError(
    PSTR    Banner,
    ULONG   Code,
    BOOL    TreatAsStatus
    )
{
    HANDLE Dll ;
    PSTR Source ;
    UCHAR Message[ 512 ];
    PUCHAR s;

    if ( !TreatAsStatus )
    {
        //
        // The value "type" is not known.  Try and figure out what it
        // is.
        //

        if ( (Code & 0xC0000000) == 0xC0000000 )
        {
            //
            // Easy:  NTSTATUS failure case
            //

            Dll = GetModuleHandle( "NTDLL.DLL" );

            Source = "NTSTATUS" ;

            TreatAsStatus = TRUE ;

        }
        else if ( ( Code & 0xF0000000 ) == 0xD0000000 )
        {
            //
            // HRESULT from NTSTATUS
            //

            Dll = GetModuleHandle( "NTDLL.DLL" );

            Source = "NTSTATUS" ;

            Code &= 0xCFFFFFFF ;

            TreatAsStatus = TRUE ;

        }
        else if ( ( Code & 0x80000000 ) == 0x80000000 )
        {
            //
            // Note, this can overlap with NTSTATUS warning area.  In that
            // case, force the NTSTATUS.
            //

            Dll = GetModuleHandle( "KERNEL32.DLL" );

            Source = "HRESULT" ;

        }
        else
        {
            //
            // Sign bit is off.  Explore some known ranges:
            //

            if ( (Code >= WSABASEERR) && (Code <= WSABASEERR + 1000 ))
            {
                Dll = LoadLibrary( "wsock32.dll" );

                Source = "Winsock" ;
            }
            else if ( ( Code >= NERR_BASE ) && ( Code <= MAX_NERR ) )
            {
                Dll = LoadLibrary( "netmsg.dll" );

                Source = "NetAPI" ;
            }
            else
            {
                Dll = GetModuleHandle( "KERNEL32.DLL" );

                Source = "Win32" ;
            }

        }
    }
    else
    {
        Dll = GetModuleHandle( "NTDLL.DLL" );

        Source = "NTSTATUS" ;
    }

    if (!FormatMessage(  FORMAT_MESSAGE_IGNORE_INSERTS |
                    FORMAT_MESSAGE_FROM_HMODULE,
                    Dll,
                    Code,
                    0,
                    Message,
                    sizeof( Message ),
                    NULL ) )
    {
        strcpy( Message, "No mapped error code" );
    }

    s = Message ;

    while (*s) {
        if (*s < ' ') {
            *s = ' ';
            }
        s++;
        }

    if ( !TreatAsStatus )
    {
        dprintf( "%s: (%s) %#x (%u) - %s\n",
                    Banner,
                    Source,
                    Code,
                    Code,
                    Message );

    }
    else
    {
        dprintf( "%s: (%s) %#x - %s\n",
                    Banner,
                    Source,
                    Code,
                    Message );

    }

}

DECLARE_API( error )
{
    ULONG err ;

    INIT_API();

    err = (ULONG) GetExpression( lpArgumentString );

    DecodeError( "Error code", err, FALSE );

}


DECLARE_API( gle )
{
    NTSTATUS Status;
    THREAD_BASIC_INFORMATION ThreadInformation;
    ULONGLONG Address;
    TEB Teb;

    INIT_API();

    Status = QueryTebAddress(hCurrentThread, &Address);

    if (NT_SUCCESS( Status )) {
        if (ReadMemory( Address,
                        &Teb,
                        sizeof(Teb),
                        NULL
                      )
           ) {

            DecodeError( "LastErrorValue", Teb.LastErrorValue, FALSE );

            DecodeError( "LastStatusValue", Teb.LastStatusValue, TRUE );

            return ;

            }

        }

    dprintf("Unable to read current thread's TEB\n" );
    return;
}

DECLARE_API( version )
{
    OSVERSIONINFOA VersionInformation;
    HKEY hkey;
    DWORD cb, dwType;
    CHAR szCurrentType[128];
    CHAR szCSDString[3+128];

    INIT_API();

    VersionInformation.dwOSVersionInfoSize = sizeof(VersionInformation);
    if (!GetVersionEx( &VersionInformation )) {
        dprintf("GetVersionEx failed - %u\n", GetLastError());
        return;
        }

    szCurrentType[0] = '\0';
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     "Software\\Microsoft\\Windows NT\\CurrentVersion",
                     0,
                     KEY_READ,
                     &hkey
                    ) == NO_ERROR
       ) {
        cb = sizeof(szCurrentType);
        if (RegQueryValueEx(hkey, "CurrentType", NULL, &dwType, szCurrentType, &cb ) != 0) {
            szCurrentType[0] = '\0';
            }
        }
    RegCloseKey(hkey);

    if (VersionInformation.szCSDVersion[0]) {
        sprintf(szCSDString, ": %s", VersionInformation.szCSDVersion);
        }
    else {
        szCSDString[0] = '\0';
        }

    dprintf("Version %d.%d (Build %d%s) %s\n",
          VersionInformation.dwMajorVersion,
          VersionInformation.dwMinorVersion,
          VersionInformation.dwBuildNumber,
          szCSDString,
          szCurrentType
         );
    return;
}

DECLARE_API( help )
{
    INIT_API();

    while (*lpArgumentString == ' ')
        lpArgumentString++;

    if (*lpArgumentString == '\0') {
        dprintf("ntsdexts help:\n\n");
        dprintf("!atom [atom]                 - Dump the atom or table(s) for the process\n");
        dprintf("!critSec csAddress           - Dump a critical section\n");
        dprintf("!cxr address                 - Dump a context record\n");
        dprintf("!dh [image base address]     - Dump image header\n");
        dprintf("!dlls [-h]                   - Dump loaded DLLS\n");
        dprintf("!dp [v] [pid | pcsr_process] - Dump CSR process\n");
        dprintf("!dreg -[d|w] <keyPath>[![<valueName> | *]]  - Dump registry information\n");
        dprintf("!dt [v] pcsr_thread          - Dump CSR thread\n");
        dprintf("!error value                 - Decode error value\n");
        dprintf("!exr address                 - Dump an exception record\n");
        dprintf("!gatom                       - Dump the global atom table\n");
        dprintf("!gflag [value]               - Dump the global atom table\n");
        dprintf("!gle                         - Dump GetLastError value for current thread\n");
        dprintf("!handle [handle]             - Dump handle information\n");
        dprintf("!heap [address]              - Dump heap\n");
        dprintf("!help [cmd]                  - Displays this list or gives details on command\n");
        dprintf("!igrep [pattern [addr]]      - Grep for disassembled pattern starting at addr\n");
        dprintf("!locks [-v]                  - Dump all Critical Sections in process\n");
        dprintf("!obja ObjectAddress          - Dump an object's attributes\n");
        dprintf("!peb [peb addr to dump]      - Dump the PEB structure\n");
        dprintf("!str AnsiStringAddress       - Dump an ANSI string\n");
        dprintf("!teb [teb addr to dump]      - Dump the TEB structure\n");
        dprintf("!ustr UnicodeStringAddress   - Dump a UNICODE string\n");
        dprintf("!version                     - Dump system version and build number\n");
        dprintf("!vprot [address]             - Dump the virtual protect settings\n");

    } else {
        if (*lpArgumentString == '!')
            lpArgumentString++;
        if (strcmp(lpArgumentString, "igrep") == 0) {
            dprintf("!igrep [pattern [addr]]     - Grep for disassembled pattern starting at addr\n");
            dprintf("       If no pattern, last pattern is used, if no address, last hit is used\n");
        } else if (strcmp( lpArgumentString, "handle") == 0) {
            dprintf("!handle [handle [flags [type]]] - Dump handle information\n");
            dprintf("       If no handle specified, all handles are dumped.\n");
            dprintf("       Flags are bits indicating greater levels of detail.\n");
            dprintf("If the handle is 0 or -1, all handles are scanned.  If the handle is not\n");
            dprintf("zero, that particular handle is examined.  The flags are as follows:\n");
            dprintf("    1   - Get type information (default)\n");
            dprintf("    2   - Get basic information\n");
            dprintf("    4   - Get name information\n");
            dprintf("    8   - Get object specific info (where available)\n");
            dprintf("\n");
            dprintf("If Type is specified, only object of that type are scanned.  Type is a\n");
            dprintf("standard NT type name, e.g. Event, Semaphore, etc.  Case sensitive, of\n");
            dprintf("course.\n");
            dprintf("\n");
            dprintf("Examples:\n");
            dprintf("\n");
            dprintf("    !handle     -- dumps the types of all the handles, and a summary table\n");
            dprintf("    !handle 0 0 -- dumps a summary table of all the open handles\n");
            dprintf("    !handle 0 f -- dumps everything we can find about a handle.\n");
            dprintf("    !handle 0 f Event\n");
            dprintf("                -- dumps everything we can find about open events\n");
        } else if (strcmp( lpArgumentString, "gflag") == 0) {
            dprintf("If a value is not given then displays the current bits set in\n");
            dprintf("NTDLL!NtGlobalFlag variable.  Otherwise value can be one of the\n");
            dprintf("following:\n");
            dprintf("\n");
            dprintf("    -? - displays a list of valid flag abbreviations\n");
            dprintf("    number - 32-bit number that becomes the new value stored into\n");
            dprintf("             NtGlobalFlag\n");
            dprintf("    +number - specifies one or more bits to set in NtGlobalFlag\n");
            dprintf("    +abbrev - specifies a single bit to set in NtGlobalFlag\n");
            dprintf("    -number - specifies one or more bits to clear in NtGlobalFlag\n");
            dprintf("    -abbrev - specifies a single bit to clear in NtGlobalFlag\n");
        } else {
            dprintf("Invalid command.  No help available\n");
        }
    }
}



PLIST_ENTRY
DumpCritSec(
    DWORD_PTR dwAddrCritSec,
    BOOLEAN bDumpIfUnowned
    )

/*++

Routine Description:

    This function is called as an NTSD extension to format and dump
    the contents of the specified critical section.

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    dwAddrCritSec - Supplies the address of the critical section to
        be dumped

    bDumpIfUnowned - TRUE means to dump the critical section even if
        it is currently unowned.

Return Value:

    Pointer to the next critical section in the list for the process or
    NULL if no more critical sections.

--*/

{
    USHORT i;
    CHAR Symbol[1024];
    DWORD_PTR Displacement;
    CRITICAL_SECTION CriticalSection;
    CRITICAL_SECTION_DEBUG DebugInfo;
    BOOL b;

    //
    // Read the critical section from the debuggees address space into our
    // own.

    b = ReadMemory( dwAddrCritSec,
                    &CriticalSection,
                    sizeof(CriticalSection),
                    NULL
                  );
    if ( !b ) {
        return NULL;
        }

    DebugInfo.ProcessLocksList.Flink = NULL;
    if (CriticalSection.DebugInfo != NULL) {
        b = ReadMemory( CriticalSection.DebugInfo,
                        &DebugInfo,
                        sizeof(DebugInfo),
                        NULL
                      );
        if ( !b ) {
            CriticalSection.DebugInfo = NULL;
            }
        }

    //
    // Dump the critical section
    //

    if ( CriticalSection.LockCount == -1 && !bDumpIfUnowned) {
        return DebugInfo.ProcessLocksList.Flink;
        }

    //
    // Get the symbolic name of the critical section
    //

    dprintf("\n");
    GetSymbol((LPVOID)dwAddrCritSec,Symbol,&Displacement);
    dprintf(
        "CritSec %s+%lx at %p\n",
        Symbol,
        Displacement,
        dwAddrCritSec
        );

    if ( CriticalSection.LockCount == -1) {
        dprintf("LockCount          NOT LOCKED\n");
        }
    else {
        dprintf("LockCount          %ld\n",CriticalSection.LockCount);
        }

    dprintf("RecursionCount     %ld\n",CriticalSection.RecursionCount);
    dprintf("OwningThread       %lx\n",CriticalSection.OwningThread);
    dprintf("EntryCount         %lx\n",DebugInfo.EntryCount);
    if (CriticalSection.DebugInfo != NULL) {
        dprintf("ContentionCount    %lx\n",DebugInfo.ContentionCount);
        if ( CriticalSection.LockCount != -1) {
            dprintf("*** Locked\n");
            }

        return DebugInfo.ProcessLocksList.Flink;
        }

    return NULL;
}

DECLARE_API( critsec )
{
    DWORD_PTR dwAddrCritSec;

    INIT_API();

    //
    // Evaluate the argument string to get the address of
    // the critical section to dump.
    //

    dwAddrCritSec = GetExpression(lpArgumentString);
    if ( !dwAddrCritSec ) {
        return;
        }

    DumpCritSec(dwAddrCritSec,TRUE);
}

DECLARE_API( igrep )
/*++

Routine Description:

    This function is called as an NTSD extension to grep the instruction
    stream for a particular pattern.

    Called as:

        !igrep [pattern [expression]]

    If a pattern is not given, the last pattern is used.  If expression
    is not given, the last hit address is used.

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - Supplies the pattern and expression for this
        command.


Return Value:

    None.

--*/

{
    DWORD_PTR dwNextGrepAddr;
    DWORD_PTR dwCurrGrepAddr;
    CHAR SourceLine[256];
    BOOL NewPc;
    DWORD d;
    LPSTR pc;
    LPSTR Pattern;
    LPSTR Expression;
    CHAR Symbol[1024];
    DWORD_PTR Displacement;

    INIT_API();

    if ( igrepLastPc && igrepLastPc == dwCurrentPc ) {
        NewPc = FALSE;
        }
    else {
        igrepLastPc = dwCurrentPc;
        NewPc = TRUE;
        }

    //
    // check for pattern.
    //

    pc = lpArgumentString;
    Pattern = NULL;
    Expression = NULL;
    if ( *pc ) {
        Pattern = pc;
        while (*pc > ' ') {
                pc++;
            }

        //
        // check for an expression
        //

        if ( *pc != '\0' ) {
            *pc = '\0';
            pc++;
            if ( *pc <= ' ') {
                while (*pc <= ' '){
                    pc++;
                    }
                }
            if ( *pc ) {
                Expression = pc;
                }
            }
        }

    if ( Pattern ) {
        strcpy(igrepLastPattern,Pattern);

        if ( Expression ) {
            igrepSearchStartAddress = GetExpression(Expression);
            if ( !igrepSearchStartAddress ) {
                igrepSearchStartAddress = igrepLastPc;
                return;
                }
            }
        else {
            igrepSearchStartAddress = igrepLastPc;
            }
        }

    dwNextGrepAddr = igrepSearchStartAddress;
    dwCurrGrepAddr = dwNextGrepAddr;
    d = Disasm(&dwNextGrepAddr,SourceLine,FALSE);
    while(d) {
        if (strstr(SourceLine,igrepLastPattern)) {
            igrepSearchStartAddress = dwNextGrepAddr;
            GetSymbol((LPVOID)dwCurrGrepAddr,Symbol,&Displacement);
            dprintf("%s",SourceLine);
            return;
            }
        if ((CheckControlC)()) {
            return;
            }
        dwCurrGrepAddr = dwNextGrepAddr;
        d = Disasm(&dwNextGrepAddr,SourceLine,FALSE);
        }
}

DECLARE_API( str )

/*++

Routine Description:

    This function is called as an NTSD extension to format and dump
    counted (ansi) string.

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - Supplies the asciiz string that describes the
        ansi string to be dumped.

Return Value:

    None.

--*/

{
    ANSI_STRING AnsiString;
    DWORD_PTR dwAddrString;
    CHAR Symbol[1024];
    LPSTR StringData;
    DWORD_PTR Displacement;
    BOOL b;

    INIT_API();

    //
    // Evaluate the argument string to get the address of
    // the string to dump.
    //

    dwAddrString = GetExpression(lpArgumentString);
    if ( !dwAddrString ) {
        return;
        }


    //
    // Get the symbolic name of the string
    //

    GetSymbol((LPVOID)dwAddrString,Symbol,&Displacement);

    //
    // Read the string from the debuggees address space into our
    // own.

    b = ReadMemory( dwAddrString,
                    &AnsiString,
                    sizeof(AnsiString),
                    NULL
                  );
    if ( !b ) {
        return;
        }

    StringData = (LPSTR)LocalAlloc(LMEM_ZEROINIT,AnsiString.Length+1);

    b = ReadMemory( AnsiString.Buffer,
                    StringData,
                    AnsiString.Length,
                    NULL
                  );
    if ( !b ) {
        LocalFree(StringData);
        return;
        }

    dprintf(
        "String(%d,%d) %s+%p at %p: %s\n",
        AnsiString.Length,
        AnsiString.MaximumLength,
        Symbol,
        Displacement,
        dwAddrString,
        StringData
        );

    LocalFree(StringData);
}

DECLARE_API( ustr )

/*++

Routine Description:

    This function is called as an NTSD extension to format and dump
    counted unicode string.

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - Supplies the asciiz string that describes the
        ansi string to be dumped.

Return Value:

    None.

--*/

{
    ANSI_STRING AnsiString;
    UNICODE_STRING UnicodeString;
    DWORD_PTR dwAddrString;
    CHAR Symbol[1024];
    LPSTR StringData;
    DWORD_PTR Displacement;
    BOOL b;

    INIT_API();

    //
    // Evaluate the argument string to get the address of
    // the string to dump.
    //

    dwAddrString = GetExpression(lpArgumentString);
    if ( !dwAddrString ) {
        return;
        }


    //
    // Get the symbolic name of the string
    //

    GetSymbol((LPVOID)dwAddrString,Symbol,&Displacement);

    //
    // Read the string from the debuggees address space into our
    // own.

    b = ReadMemory( dwAddrString,
                    &UnicodeString,
                    sizeof(UnicodeString),
                    NULL
                  );
    if ( !b ) {
        return;
        }

    StringData = (LPSTR)LocalAlloc(LMEM_ZEROINIT,UnicodeString.Length+sizeof(UNICODE_NULL));

    b = ReadMemory( UnicodeString.Buffer,
                    StringData,
                    UnicodeString.Length,
                    NULL
                  );
    if ( !b ) {
        LocalFree(StringData);
        return;
        }
    UnicodeString.Buffer = (PWSTR)StringData;
    UnicodeString.MaximumLength = UnicodeString.Length+(USHORT)sizeof(UNICODE_NULL);

    RtlUnicodeStringToAnsiString(&AnsiString,&UnicodeString,TRUE);
    LocalFree(StringData);

    dprintf(
        "String(%d,%d) %s+%p at %p: %s\n",
        UnicodeString.Length,
        UnicodeString.MaximumLength,
        Symbol,
        Displacement,
        dwAddrString,
        AnsiString.Buffer
        );

    RtlFreeAnsiString(&AnsiString);
}

DECLARE_API( obja )

/*++

Routine Description:

    This function is called as an NTSD extension to format and dump
    an object attributes structure.

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - Supplies the asciiz string that describes the
        ansi string to be dumped.

Return Value:

    None.

--*/

{
    UNICODE_STRING UnicodeString;
    DWORD_PTR dwAddrObja;
    OBJECT_ATTRIBUTES Obja;
    DWORD_PTR dwAddrString;
    CHAR Symbol[1024];
    LPSTR StringData;
    DWORD_PTR Displacement;
    BOOL b;

    INIT_API();

    //
    // Evaluate the argument string to get the address of
    // the Obja to dump.
    //

    dwAddrObja = GetExpression(lpArgumentString);
    if ( !dwAddrObja ) {
        return;
        }


    //
    // Get the symbolic name of the Obja
    //

    GetSymbol((LPVOID)dwAddrObja,Symbol,&Displacement);

    //
    // Read the obja from the debuggees address space into our
    // own.

    b = ReadMemory( dwAddrObja,
                    &Obja,
                    sizeof(Obja),
                    NULL
                  );
    if ( !b ) {
        return;
        }
    StringData = NULL;
    if ( Obja.ObjectName ) {
        dwAddrString = (DWORD_PTR)Obja.ObjectName;
        b = ReadMemory( dwAddrString,
                        &UnicodeString,
                        sizeof(UnicodeString),
                        NULL
                      );
        if ( !b ) {
            return;
            }

        StringData = (LPSTR)LocalAlloc(
                        LMEM_ZEROINIT,
                        UnicodeString.Length+sizeof(UNICODE_NULL)
                        );

        b = ReadMemory( UnicodeString.Buffer,
                        StringData,
                        UnicodeString.Length,
                        NULL
                      );
        if ( !b ) {
            LocalFree(StringData);
            return;
            }
        UnicodeString.Buffer = (PWSTR)StringData;
        UnicodeString.MaximumLength = UnicodeString.Length+(USHORT)sizeof(UNICODE_NULL);
    }

    //
    // We got the object name in UnicodeString. StringData is NULL if no name.
    //

    dprintf(
        "Obja %s+%p at %p:\n",
        Symbol,
        Displacement,
        dwAddrObja
        );
    if ( StringData ) {
        dprintf("\t%s is %ws\n",
            Obja.RootDirectory ? "Relative Name" : "Full Name",
            UnicodeString.Buffer
            );
        LocalFree(StringData);
        }
    if ( Obja.Attributes ) {
            if ( Obja.Attributes & OBJ_INHERIT ) {
                dprintf("\tOBJ_INHERIT\n");
                }
            if ( Obja.Attributes & OBJ_PERMANENT ) {
                dprintf("\tOBJ_PERMANENT\n");
                }
            if ( Obja.Attributes & OBJ_EXCLUSIVE ) {
                dprintf("\tOBJ_EXCLUSIVE\n");
                }
            if ( Obja.Attributes & OBJ_CASE_INSENSITIVE ) {
                dprintf("\tOBJ_CASE_INSENSITIVE\n");
                }
            if ( Obja.Attributes & OBJ_OPENIF ) {
                dprintf("\tOBJ_OPENIF\n");
                }
        }
}


DECLARE_API( locks )

/*++

Routine Description:

    This function is called as an NTSD extension to display all
    critical sections in the target process.

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - tbd.

Return Value:

    None.

--*/

{
    BOOL b;
    CRITICAL_SECTION_DEBUG DebugInfo;
    PVOID AddrListHead;
    LIST_ENTRY ListHead;
    PLIST_ENTRY Next;
    BOOLEAN Verbose;
    LPSTR p;

    INIT_API();

    Verbose = FALSE;
    p = lpArgumentString;
    while ( p != NULL && *p ) {
        if ( *p == '-' ) {
            p++;
            switch ( *p ) {
                case 'V':
                case 'v':
                    Verbose = TRUE;
                    p++;
                    break;

                case ' ':
                    goto gotBlank;

                default:
                    dprintf( "NTSDEXTS: !locks invalid option flag '-%c'\n", *p );
                    break;

                }
            }
        else {
gotBlank:
            p++;
            }
        }

    //
    // Locate the address of the list head.
    //

    AddrListHead = (PVOID)GetExpression("&ntdll!RtlCriticalSectionList");
    if ( !AddrListHead ) {
        return;
        }

    //
    // Read the list head
    //

    b = ReadMemory( AddrListHead,
                    &ListHead,
                    sizeof(ListHead),
                    NULL
                  );
    if ( !b ) {
        return;
        }

    Next = ListHead.Flink;

    //
    // Walk the list of critical sections
    //
    while ( Next != AddrListHead ) {
        b = ReadMemory( CONTAINING_RECORD( Next,
                                           RTL_CRITICAL_SECTION_DEBUG,
                                           ProcessLocksList
                                         ),
                        &DebugInfo,
                        sizeof(DebugInfo),
                        NULL
                      );
        if ( !b ) {
            return;
            }

        Next = DumpCritSec((DWORD_PTR)DebugInfo.CriticalSection, Verbose);
        if (Next == NULL) {
            break;
            }

        if ((CheckControlC)()) {
            break;
            }

        }

    return;
}


//
// Simple routine to convert from hex into a string of characters.
// Used by debugger extensions.
//
// by scottlu
//

char *
HexToString(
    ULONG_PTR dw,
    CHAR *pch
    )
{
    if (dw > 0xf) {
        pch = HexToString(dw >> 4, pch);
        dw &= 0xf;
    }

    *pch++ = ((dw >= 0xA) ? ('A' - 0xA) : '0') + (CHAR)dw;
    *pch = 0;

    return pch;
}


//
// dt == dump thread
//
// dt [v] pcsr_thread
// v == verbose (structure)
//
// by scottlu
//

DECLARE_API( dt )
{
    char chVerbose;
    CSR_THREAD csrt;
    ULONG dw;

    INIT_API();

    while (*lpArgumentString == ' ')
        lpArgumentString++;

    chVerbose = ' ';
    if (*lpArgumentString == 'v')
        chVerbose = *lpArgumentString++;

    dw = (ULONG)GetExpression(lpArgumentString);
    move(csrt, dw);

    //
    // Print simple thread info if the user did not ask for verbose.
    //
    if (chVerbose == ' ') {
        dprintf("Thread %08lx, Process %08lx, ClientId %lx.%lx, Flags %lx, Ref Count %lx\n",
                dw,
                csrt.Process,
                csrt.ClientId.UniqueProcess,
                csrt.ClientId.UniqueThread,
                csrt.Flags,
                csrt.ReferenceCount);
        return;
    }

    dprintf("PCSR_THREAD @ %08lx:\n"
            "\t+%04lx Link.Flink                %08lx\n"
            "\t+%04lx Link.Blink                %08lx\n"
            "\t+%04lx Process                   %08lx\n",
            dw,
            FIELD_OFFSET(CSR_THREAD, Link.Flink), csrt.Link.Flink,
            FIELD_OFFSET(CSR_THREAD, Link.Blink), csrt.Link.Blink,
            FIELD_OFFSET(CSR_THREAD, Process), csrt.Process);

    dprintf(
            "\t+%04lx WaitBlock                 %08lx\n"
            "\t+%04lx ClientId.UniqueProcess    %08lx\n"
            "\t+%04lx ClientId.UniqueThread     %08lx\n"
            "\t+%04lx ThreadHandle              %08lx\n",
            FIELD_OFFSET(CSR_THREAD, WaitBlock), csrt.WaitBlock,
            FIELD_OFFSET(CSR_THREAD, ClientId.UniqueProcess), csrt.ClientId.UniqueProcess,
            FIELD_OFFSET(CSR_THREAD, ClientId.UniqueThread), csrt.ClientId.UniqueThread,
            FIELD_OFFSET(CSR_THREAD, ThreadHandle), csrt.ThreadHandle);

    dprintf(
            "\t+%04lx Flags                     %08lx\n"
            "\t+%04lx ReferenceCount            %08lx\n"
            "\t+%04lx HashLinks.Flink           %08lx\n"
            "\t+%04lx HashLinks.Blink           %08lx\n",
            FIELD_OFFSET(CSR_THREAD, Flags), csrt.Flags,
            FIELD_OFFSET(CSR_THREAD, ReferenceCount), csrt.ReferenceCount,
            FIELD_OFFSET(CSR_THREAD, HashLinks.Flink), csrt.HashLinks.Flink,
            FIELD_OFFSET(CSR_THREAD, HashLinks.Blink), csrt.HashLinks.Blink);

    return;
}

//
// dp == dump process
//
// dp [v] [pid | pcsr_process]
//      v == verbose (structure + thread list)
//      no process == dump process list
//
// by scottlu
//

DECLARE_API( dp )
{
    PLIST_ENTRY ListHead, ListNext;
    char ach[80];
    char chVerbose;
    PCSR_PROCESS pcsrpT;
    CSR_PROCESS csrp;
    PCSR_PROCESS pcsrpRoot;
    PCSR_THREAD pcsrt;
    ULONG_PTR dwProcessId;
    ULONG_PTR dw;
    DWORD_PTR dwRootProcess;

    INIT_API();

    while (*lpArgumentString == ' ')
        lpArgumentString++;

    chVerbose = ' ';
    if (*lpArgumentString == 'v')
        chVerbose = *lpArgumentString++;

    dwRootProcess = GetExpression("&csrsrv!CsrRootProcess");
    if ( !dwRootProcess ) {
        return;
        }

    move(pcsrpRoot, dwRootProcess);

    //
    // See if user wants all processes. If so loop through them.
    //
    if (*lpArgumentString == 0) {
        ListHead = &pcsrpRoot->ListLink;
        move(ListNext, &ListHead->Flink);

        while (ListNext != ListHead) {
            pcsrpT = CONTAINING_RECORD(ListNext, CSR_PROCESS, ListLink);

            ach[0] = chVerbose;
            ach[1] = ' ';
            HexToString((ULONG_PTR)pcsrpT, &ach[2]);

            dp(hCurrentProcess, hCurrentThread, dwCurrentPc, lpExtensionApis,
                    ach);

            move(ListNext, &ListNext->Flink);
        }

        dprintf("---\n");
        return;
    }

    //
    // User wants specific process structure. Evaluate to find id or process
    // pointer.
    //
    dw = (ULONG)GetExpression(lpArgumentString);

    ListHead = &pcsrpRoot->ListLink;
    move(ListNext, &ListHead->Flink);

    while (ListNext != ListHead) {
        pcsrpT = CONTAINING_RECORD(ListNext, CSR_PROCESS, ListLink);
        move(ListNext, &ListNext->Flink);

        move(dwProcessId, &pcsrpT->ClientId.UniqueProcess);
        if (dw == dwProcessId) {
            dw = (ULONG_PTR)pcsrpT;
            break;
        }
    }

    pcsrpT = (PCSR_PROCESS)dw;
    move(csrp, pcsrpT);

    //
    // If not verbose, print simple process info.
    //
    if (chVerbose == ' ') {
        dprintf("Process %08lx, Id %p, Seq# %lx, Flags %lx, Ref Count %lx\n",
                pcsrpT,
                csrp.ClientId.UniqueProcess,
                csrp.SequenceNumber,
                csrp.Flags,
                csrp.ReferenceCount);
        return;
    }

    dprintf("PCSR_PROCESS @ %08lx:\n"
            "\t+%04lx ListLink.Flink            %08lx\n"
            "\t+%04lx ListLink.Blink            %08lx\n"
            "\t+%04lx Parent                    %08lx\n",
            pcsrpT,
            FIELD_OFFSET(CSR_PROCESS, ListLink.Flink), csrp.ListLink.Flink,
            FIELD_OFFSET(CSR_PROCESS, ListLink.Blink), csrp.ListLink.Blink,
            FIELD_OFFSET(CSR_PROCESS, Parent), csrp.Parent);

    dprintf(
            "\t+%04lx ThreadList.Flink          %08lx\n"
            "\t+%04lx ThreadList.Blink          %08lx\n"
            "\t+%04lx NtSession                 %08lx\n"
            "\t+%04lx ExpectedVersion           %08lx\n",
            FIELD_OFFSET(CSR_PROCESS, ThreadList.Flink), csrp.ThreadList.Flink,
            FIELD_OFFSET(CSR_PROCESS, ThreadList.Blink), csrp.ThreadList.Blink,
            FIELD_OFFSET(CSR_PROCESS, NtSession), csrp.NtSession,
            FIELD_OFFSET(CSR_PROCESS, ExpectedVersion), csrp.ExpectedVersion);

    dprintf(
            "\t+%04lx ClientPort                %08lx\n"
            "\t+%04lx ClientViewBase            %08lx\n"
            "\t+%04lx ClientViewBounds          %08lx\n"
            "\t+%04lx ClientId.UniqueProcess    %08lx\n",
            FIELD_OFFSET(CSR_PROCESS, ClientPort), csrp.ClientPort,
            FIELD_OFFSET(CSR_PROCESS, ClientViewBase), csrp.ClientViewBase,
            FIELD_OFFSET(CSR_PROCESS, ClientViewBounds), csrp.ClientViewBounds,
            FIELD_OFFSET(CSR_PROCESS, ClientId.UniqueProcess), csrp.ClientId.UniqueProcess);

    dprintf(
            "\t+%04lx ProcessHandle             %08lx\n"
            "\t+%04lx SequenceNumber            %08lx\n"
            "\t+%04lx Flags                     %08lx\n"
            "\t+%04lx DebugFlags                %08lx\n",
            FIELD_OFFSET(CSR_PROCESS, ProcessHandle), csrp.ProcessHandle,
            FIELD_OFFSET(CSR_PROCESS, SequenceNumber), csrp.SequenceNumber,
            FIELD_OFFSET(CSR_PROCESS, Flags), csrp.Flags,
            FIELD_OFFSET(CSR_PROCESS, DebugFlags), csrp.DebugFlags);

    dprintf(
            "\t+%04lx DebugUserInterface        %08lx\n"
            "\t+%04lx ReferenceCount            %08lx\n"
            "\t+%04lx ProcessGroupId            %08lx\n"
            "\t+%04lx ProcessGroupSequence      %08lx\n",
            FIELD_OFFSET(CSR_PROCESS, DebugUserInterface.UniqueProcess), csrp.DebugUserInterface.UniqueProcess,
            FIELD_OFFSET(CSR_PROCESS, ReferenceCount), csrp.ReferenceCount,
            FIELD_OFFSET(CSR_PROCESS, ProcessGroupId), csrp.ProcessGroupId,
            FIELD_OFFSET(CSR_PROCESS, ProcessGroupSequence), csrp.ProcessGroupSequence);

    dprintf(
            "\t+%04lx fVDM                      %08lx\n"
            "\t+%04lx ThreadCount               %08lx\n"
            "\t+%04lx PriorityClass             %08lx\n"
            "\t+%04lx ShutdownLevel             %08lx\n"
            "\t+%04lx ShutdownFlags             %08lx\n",
            FIELD_OFFSET(CSR_PROCESS, fVDM), csrp.fVDM,
            FIELD_OFFSET(CSR_PROCESS, ThreadCount), csrp.ThreadCount,
            FIELD_OFFSET(CSR_PROCESS, PriorityClass), csrp.PriorityClass,
            FIELD_OFFSET(CSR_PROCESS, ShutdownLevel), csrp.ShutdownLevel,
            FIELD_OFFSET(CSR_PROCESS, ShutdownFlags), csrp.ShutdownFlags);

    //
    // Now dump simple thread info for this processes' threads.
    //

    ListHead = &pcsrpT->ThreadList;
    move(ListNext, &ListHead->Flink);

    dprintf("Threads:\n");

    while (ListNext != ListHead) {
        pcsrt = CONTAINING_RECORD(ListNext, CSR_THREAD, Link);

        //
        // Make sure this pcsrt is somewhat real so we don't loop forever.
        //
        move(dwProcessId, &pcsrt->ClientId.UniqueProcess);
        if (dwProcessId != (DWORD_PTR)csrp.ClientId.UniqueProcess) {
            dprintf("Invalid thread. Probably invalid argument to this extension.\n");
            return;
        }

        HexToString((ULONG_PTR)pcsrt, ach);
        dt(hCurrentProcess, hCurrentThread, dwCurrentPc, lpExtensionApis, ach);

        move(ListNext, &ListNext->Flink);
    }

    return;
}

VOID
Reg64(
    LPSTR   Name,
    ULONG   HiPart,
    ULONG   LoPart,
    BOOL    ForceHi
    )
{
    dprintf("%4s=", Name);
    if (ForceHi || HiPart) {
        dprintf("%08lx", HiPart);
    }
    dprintf("%08lx   ", LoPart);
}

CONTEXT LastContext;
BOOL HaveContext = 0;

DECLARE_API( cxr )

/*++

Routine Description:

    This function is called as an NTSD extension to dump a context record

    Called as:

        !cxr address

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - Supplies the pattern and expression for this
        command.


Return Value:

    None.

--*/
{
    CONTEXT Context;
    DWORD_PTR Address;

    INIT_API();

    Address = GetExpression(lpArgumentString);
    if (!Address) {
        return;
    }

    move(Context, Address);

    LastContext = Context;
    HaveContext = TRUE;



#if i386

    dprintf("eax=%08lx ebx=%08lx ecx=%08lx edx=%08lx esi=%08lx edi=%08lx\n",
                Context.Eax,
                Context.Ebx,
                Context.Ecx,
                Context.Edx,
                Context.Esi,
                Context.Edi);
    dprintf("eip=%08lx esp=%08lx ebp=%08lx iopl=%1lx         "
        "%s %s %s %s %s %s %s %s\n",
                Context.Eip,
                Context.Esp,
                Context.Ebp,
                ((Context.EFlags >> 12) & 3),
        (Context.EFlags & 0x800) ? "ov" : "nv",
        (Context.EFlags & 0x400) ? "dn" : "up",
        (Context.EFlags & 0x200) ? "ei" : "di",
        (Context.EFlags & 0x80) ? "ng" : "pl",
        (Context.EFlags & 0x40) ? "zr" : "nz",
        (Context.EFlags & 0x10) ? "ac" : "na",
        (Context.EFlags & 0x4) ? "po" : "pe",
        (Context.EFlags & 0x1) ? "cy" : "nc");

    dprintf("cs=%04x  ss=%04x  ds=%04x  es=%04x  fs=%04x  gs=%04x"
            "             efl=%08lx\n",
                (USHORT)(Context.SegCs & 0xffff),
                (USHORT)(Context.SegSs & 0xffff),
                (USHORT)(Context.SegDs & 0xffff),
                (USHORT)(Context.SegEs & 0xffff),
                (USHORT)(Context.SegFs & 0xffff),
                (USHORT)(Context.SegGs & 0xffff),
                Context.EFlags);

#elif MIPS

    if (Context.ContextFlags & CONTEXT_EXTENDED_INTEGER) {
        dprintf("at=%08I64x v0=%08I64x v1=%08I64x\n",
                    Context.XIntAt,
                    Context.XIntV0,
                    Context.XIntV1);
        dprintf("a0=%08I64x a1=%08I64x a2=%08I64x\n",
                    Context.XIntA0,
                    Context.XIntA1,
                    Context.XIntA2);
        dprintf("a3=%08I64x t0=%08I64x t1=%08I64x\n",
                    Context.XIntA3,
                    Context.XIntT0,
                    Context.XIntT1);
        dprintf("t2=%08I64x t3=%08I64x t4=%08I64x\n",
                    Context.XIntT2,
                    Context.XIntT3,
                    Context.XIntT4);
        dprintf("t5=%08I64x t6=%08I64x t7=%08I64x\n",
                    Context.XIntT5,
                    Context.XIntT6,
                    Context.XIntT7);
        dprintf("s0=%08I64x s1=%08I64x s2=%08I64x\n",
                    Context.XIntS0,
                    Context.XIntS1,
                    Context.XIntS2);
        dprintf("s3=%08I64x s4=%08I64x s5=%08I64x\n",
                    Context.XIntS3,
                    Context.XIntS4,
                    Context.XIntS5);
        dprintf("s6=%08I64x s7=%08I64x t8=%08I64x\n",
                    Context.XIntS6,
                    Context.XIntS7,
                    Context.XIntT8);
        dprintf("t9=%08I64x k0=%08I64x k1=%08I64x\n",
                    Context.XIntT9,
                    Context.XIntK0,
                    Context.XIntK1);
        dprintf("gp=%08I64x sp=%08I64x s8=%08I64x\n",
                    Context.XIntGp,
                    Context.XIntSp,
                    Context.XIntS8);
        dprintf("ra=%08I64x lo=%08I64x hi=%08I64x\n",
                    Context.XIntRa,
                    Context.XIntLo,
                    Context.XIntHi);
        dprintf("fir=%08lx psr=%08lx\n",
                    Context.XFir,
                    Context.XPsr);
        dprintf("cu=%1lx%1lx%1lx%1lx intr(5:0)=%1lx%1lx%1lx%1lx%1lx%1lx sw(1:0)=%1lx%1lx ksu=%1lx erl=%1lx exl=%1lx ie=%1lx\n",
                    (Context.XPsr >> 31) & 0x1,
                    (Context.XPsr >> 30) & 0x1,
                    (Context.XPsr >> 29) & 0x1,
                    (Context.XPsr >> 28) & 0x1,

                    (Context.XPsr >> 15) & 0x1,
                    (Context.XPsr >> 14) & 0x1,
                    (Context.XPsr >> 13) & 0x1,
                    (Context.XPsr >> 12) & 0x1,
                    (Context.XPsr >> 11) & 0x1,
                    (Context.XPsr >> 10) & 0x1,

                    (Context.XPsr >> 9) & 0x1,
                    (Context.XPsr >> 8) & 0x1,

                    (Context.XPsr >> 3) & 0x3,
                    (Context.XPsr >> 2) & 0x1,
                    (Context.XPsr >> 1) & 0x1,
                    (Context.XPsr & 0x1));

    } else {
        dprintf("at=%08lx v0=%08lx v1=%08lx a0=%08lx a1=%08lx a2=%08lx\n",
                    Context.IntAt,
                    Context.IntV0,
                    Context.IntV1,
                    Context.IntA0,
                    Context.IntA1,
                    Context.IntA2);
        dprintf("a3=%08lx t0=%08lx t1=%08lx t2=%08lx t3=%08lx t4=%08lx\n",
                    Context.IntA3,
                    Context.IntT0,
                    Context.IntT1,
                    Context.IntT2,
                    Context.IntT3,
                    Context.IntT4);
        dprintf("t5=%08lx t6=%08lx t7=%08lx s0=%08lx s1=%08lx s2=%08lx\n",
                    Context.IntT5,
                    Context.IntT6,
                    Context.IntT7,
                    Context.IntS0,
                    Context.IntS1,
                    Context.IntS2);
        dprintf("s3=%08lx s4=%08lx s5=%08lx s6=%08lx s7=%08lx t8=%08lx\n",
                    Context.IntS3,
                    Context.IntS4,
                    Context.IntS5,
                    Context.IntS6,
                    Context.IntS7,
                    Context.IntT8);
        dprintf("t9=%08lx k0=%08lx k1=%08lx gp=%08lx sp=%08lx s8=%08lx\n",
                    Context.IntT9,
                    Context.IntK0,
                    Context.IntK1,
                    Context.IntGp,
                    Context.IntSp,
                    Context.IntS8);
        dprintf("ra=%08lx lo=%08lx hi=%08lx           fir=%08lx psr=%08lx\n",
                    Context.IntRa,
                    Context.IntLo,
                    Context.IntHi,
                    Context.Fir,
                    Context.Psr);

        dprintf("cu=%1lx%1lx%1lx%1lx intr(5:0)=%1lx%1lx%1lx%1lx%1lx%1lx sw(1:0)=%1lx%1lx ksu=%1lx erl=%1lx exl=%1lx ie=%1lx\n",
                    (Context.Psr >> 31) & 0x1,
                    (Context.Psr >> 30) & 0x1,
                    (Context.Psr >> 29) & 0x1,
                    (Context.Psr >> 28) & 0x1,

                    (Context.Psr >> 15) & 0x1,
                    (Context.Psr >> 14) & 0x1,
                    (Context.Psr >> 13) & 0x1,
                    (Context.Psr >> 12) & 0x1,
                    (Context.Psr >> 11) & 0x1,
                    (Context.Psr >> 10) & 0x1,

                    (Context.Psr >> 9) & 0x1,
                    (Context.Psr >> 8) & 0x1,

                    (Context.Psr >> 3) & 0x3,
                    (Context.Psr >> 2) & 0x1,
                    (Context.Psr >> 1) & 0x1,
                    (Context.Psr & 0x1));
    }

#elif ALPHA

#define R(N,R)  Reg64(N,(ULONG)(Context.R>>32),(ULONG)(Context.R&0xffffffff),0)
#define NL()    dprintf("\n")

    R("v0", IntV0); R("t0", IntT0); R("t1", IntT1); R("t2", IntT2); NL();
    R("t3", IntT3); R("t4", IntT4); R("t5", IntT5); R("t6", IntT6); NL();
    R("t7", IntT7); R("s0", IntS0); R("s1", IntS1); R("s2", IntS2); NL();
    R("s3", IntS3); R("s4", IntS4); R("s5", IntS5); R("fp", IntFp); NL();
    R("a0", IntA0); R("a1", IntA1); R("a2", IntA2); R("a3", IntA3); NL();
    R("a4", IntA4); R("a5", IntA5); R("t8", IntT8); R("t9", IntT9); NL();
    R("t10", IntT10); R("t11", IntT11); R("ra", IntRa); R("t12", IntT12); NL();
    R("at", IntAt); R("gp", IntGp); R("sp", IntSp); R("zero", IntZero); NL();

    Reg64("fpcr", (ULONG)(Context.Fpcr>>32), (ULONG)(Context.Fpcr&0xffffffff), 1);
    Reg64("softfpcr", (ULONG)(Context.SoftFpcr>>32), (ULONG)(Context.SoftFpcr&0xffffffff), 1);
    R("fir", Fir);
    NL();

    dprintf(" psr=%08lx\n", Context.Psr);
    dprintf("mode=%1x ie=%1x irql=%1x\n",
                        Context.Psr & 0x1,
                        (Context.Psr>>1) & 0x1,
                        (Context.Psr>>2) & 0x7);


#undef R
#undef NL

#elif PPC

#define R(N,R)  dprintf("%4s=%08lx", N, Context.R)
#define NL()    dprintf("\n")

    R("r0", Gpr0); R("r1", Gpr1); R("r2", Gpr2); R("r3", Gpr3); R("r4", Gpr4); R("r5", Gpr5); NL();
    R("r6", Gpr6); R("r7", Gpr7); R("r8", Gpr8); R("r9", Gpr9); R("r10", Gpr10); R("r11", Gpr11); NL();
    R("r12", Gpr12); R("r13", Gpr13); R("r14", Gpr14); R("r15", Gpr15); R("r16", Gpr16); R("r17", Gpr17); NL();
    R("r18", Gpr18); R("r19", Gpr19); R("r20", Gpr20); R("r21", Gpr21); R("r22", Gpr22); R("r23", Gpr23); NL();
    R("r24", Gpr24); R("r25", Gpr25); R("r26", Gpr26); R("r27", Gpr27); R("r28", Gpr28); R("r29", Gpr29); NL();
    R("r30", Gpr30); R("r31", Gpr31); R("cr", Cr); R("xer", Xer); R("msr", Msr); R("iar", Iar); NL();
    R("lr", Lr); R("ctr", Ctr); NL();

#undef R
#undef NL

#else
    dprintf("cxr code needed for cpu\n");
#endif

    return;

}


VOID
DumpExr(
    PEXCEPTION_RECORD Exr
    )
{
    ULONG   i;
    CHAR Buffer[80];
    DWORD_PTR displacement;

    GetSymbol((LPVOID)Exr->ExceptionAddress, Buffer, &displacement);

    if (*Buffer) {
        dprintf("ExceptionAddress: %08lx (%s+0x%x)\n",
                 Exr->ExceptionAddress,
                 Buffer,
                 displacement);
    } else {
        dprintf("ExceptionAddress: %08lx\n", Exr->ExceptionAddress);
    }
    //dprintf("   ExceptionCode: %08lx", Exr->ExceptionCode);
    DecodeError( "   ExceptionCode", Exr->ExceptionCode, TRUE );
    dprintf("  ExceptionFlags: %08lx\n", Exr->ExceptionFlags);

    dprintf("NumberParameters: %d\n", Exr->NumberParameters);
    if (Exr->NumberParameters > EXCEPTION_MAXIMUM_PARAMETERS) {
        Exr->NumberParameters = EXCEPTION_MAXIMUM_PARAMETERS;
    }
    for (i = 0; i < Exr->NumberParameters; i++) {
        dprintf("   Parameter[%d]: %08lx\n", i, Exr->ExceptionInformation[i]);
    }

    //
    // Known Exception processing:
    //

    switch ( Exr->ExceptionCode )
    {
        case STATUS_ACCESS_VIOLATION:
            if ( Exr->NumberParameters == 2 )
            {
                dprintf("Attempt to %s address %x\n",
                          (Exr->ExceptionInformation[0] ? "write to" : "read from"),
                          Exr->ExceptionInformation[1] );

            }
            break;

        case STATUS_IN_PAGE_ERROR:
            if ( Exr->NumberParameters == 3 )
            {
                dprintf("Inpage operation failed at %#x, due to I/O error %x\n",
                          Exr->ExceptionInformation[1],
                          Exr->ExceptionInformation[2] );
            }
            break;

        case STATUS_INVALID_HANDLE:
        case STATUS_HANDLE_NOT_CLOSABLE:
            dprintf( "Thread tried to close a handle that was invalid or illegal to close\n");
            break;

        case STATUS_POSSIBLE_DEADLOCK:
            if ( Exr->NumberParameters == 1 )
            {
                RTL_CRITICAL_SECTION CritSec ;
                ULONG Result ;

                GetSymbol( (LPVOID) Exr->ExceptionInformation[0], Buffer, &displacement );

                if ( *Buffer )
                {
                    dprintf("Critical section at %#x (%s+%#x)",
                            Exr->ExceptionInformation[0],
                            Buffer,
                            displacement );
                }
                else
                {
                    dprintf("Critical section at %#x",
                            Exr->ExceptionInformation[0] );

                }
                if ( ReadMemory( Exr->ExceptionInformation[0],
                                 &CritSec,
                                 sizeof( RTL_CRITICAL_SECTION ),
                                 &Result ) )
                {
                    if ( Result == sizeof( CritSec ))
                    {
                        dprintf("is owned by thread %x,\ncausing this thread to raise an exception",
                                CritSec.OwningThread );

                    }
                }
                dprintf("\n");

            }
            break;

        default:
            break;

    }
    return;
}

DECLARE_API( exr )

/*++

Routine Description:

    This function is called as an NTSD extension to dump an exception record

    Called as:

        !exr address

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - Supplies the pattern and expression for this
        command.


Return Value:

    None.

--*/

{
    ULONG_PTR Address;
    ULONG result;
    NTSTATUS status=0;
    EXCEPTION_RECORD    Exr;
    ULONG   i;

    INIT_API();

    Address = GetExpression(lpArgumentString);
    if (!Address) {
        return;
    }

    move(Exr, Address);

    DumpExr( &Exr );

    return;
}


VOID
DllsExtension(
    PCSTR lpArgumentString,
    PPEB ProcessPeb
    );

DECLARE_API( dlls )

/*++

Routine Description:

    This function is called as an NTSD extension to dump the loaded module data
    base for the debugged process.


Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - Supplies the pattern and expression for this
        command.


Return Value:

    None.

--*/

{
    BOOL b;
    NTSTATUS Status;
    PROCESS_BASIC_INFORMATION ProcessInformation;
    PEB ThePeb;

    INIT_API();

    QueryPeb(hCurrentThread, &ThePeb);

    DllsExtension( (PCSTR)lpArgumentString, &ThePeb );
}

#include "dllsext.c"

VOID
HeapExtension(
    IN PCSTR lpArgumentString,
    IN PPEB ProcessPeb,
    IN ULONG_PTR SystemRangeStart
    );

DECLARE_API( heap )

/*++

Routine Description:

    This function is called as an NTSD extension to dump a user mode heap

    Called as:

        !heap [address [detail]]

    If an address if not given or an address of 0 is given, then the
    process heap is dumped.  If the address is -1, then all the heaps of
    the process are dumped.  If detail is specified, it defines how much
    detail is shown.  A detail of 0, just shows the summary information
    for each heap.  A detail of 1, shows the summary information, plus
    the location and size of all the committed and uncommitted regions.
    A detail of 3 shows the allocated and free blocks contained in each
    committed region.  A detail of 4 includes all of the above plus
    a dump of the free lists.

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - Supplies the pattern and expression for this
        command.


Return Value:

    None.

--*/

{
    BOOL b;
    NTSTATUS Status;
    PROCESS_BASIC_INFORMATION ProcessInformation;
    ULONG_PTR SystemRangeStart;
    PEB ThePeb;

    INIT_API();

    QueryPeb(hCurrentThread, &ThePeb);

    Status = NtQuerySystemInformation(SystemRangeStartInformation,
                                      &SystemRangeStart,
                                      sizeof(SystemRangeStart),
                                      NULL
                                      );
    if (!NT_SUCCESS( Status )) {
        dprintf("    Unable to fetch SystemRangeStart\n" );
        SystemRangeStart = 0;
    }

    HeapExtension( (PCSTR)lpArgumentString, &ThePeb, SystemRangeStart );
}

#include "heapext.c"


VOID
AtomExtension(
    PCSTR lpArgumentString
    );



DECLARE_API( atom )

/*++

Routine Description:

    This function is called as an NTSD extension to dump a user mode atom table

    Called as:

        !atom [address]

    If an address if not given or an address of 0 is given, then the
    process atom table is dumped.

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - Supplies the pattern and expression for this
        command.


Return Value:

    None.

--*/

{
    INIT_API();

    AtomExtension( (PCSTR)lpArgumentString );
}

#include "atomext.c"

DECLARE_API( gatom )

/*++

Routine Description:

    This function is called as an NTSD extension to dump the global atom table
    kept in kernel mode

    Called as:

        !gatom

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - Supplies the pattern and expression for this
        command.


Return Value:

    None.

--*/

{
    NTSTATUS Status;
    ATOM_TABLE_INFORMATION TableInfo;
    PATOM_TABLE_INFORMATION pTableInfo;
    PATOM_BASIC_INFORMATION pBasicInfo;
    ULONG RequiredLength, MaxLength, i;

    INIT_API();

    dprintf("\nGlobal atom table ");
    Status = NtQueryInformationAtom( RTL_ATOM_INVALID_ATOM,
                                     AtomTableInformation,
                                     &TableInfo,
                                     sizeof( TableInfo ),
                                     &RequiredLength
                                   );
    if (Status != STATUS_INFO_LENGTH_MISMATCH) {
        dprintf( " - cant get information - %x\n", Status );
        return;
        }

    RequiredLength += 100 * sizeof( RTL_ATOM );
    pTableInfo = LocalAlloc( 0, RequiredLength );
    if (pTableInfo == NULL) {
        dprintf( " - cant allocate memory for %u atoms\n", RequiredLength / sizeof( RTL_ATOM ) );
        return;
        }

    Status = NtQueryInformationAtom( RTL_ATOM_INVALID_ATOM,
                                     AtomTableInformation,
                                     pTableInfo,
                                     RequiredLength,
                                     &RequiredLength
                                   );
    if (!NT_SUCCESS( Status )) {
        dprintf( " - cant get information about %x atoms - %x\n", RequiredLength / sizeof( RTL_ATOM ), Status );
        LocalFree( pTableInfo );
        return;
        }

    MaxLength = sizeof( *pBasicInfo ) + RTL_ATOM_MAXIMUM_NAME_LENGTH;
    pBasicInfo = LocalAlloc( 0, MaxLength );
    for (i=0; i<pTableInfo->NumberOfAtoms; i++) {
        Status = NtQueryInformationAtom( pTableInfo->Atoms[ i ],
                                         AtomBasicInformation,
                                         pBasicInfo,
                                         MaxLength,
                                         &RequiredLength
                                       );
        if (!NT_SUCCESS( Status )) {
            dprintf( "%hx *** query failed (%x)\n", Status );
            }
        else {
            dprintf( "%hx(%2d) = %ls (%d)%s\n",
                     pTableInfo->Atoms[ i ],
                     pBasicInfo->UsageCount,
                     pBasicInfo->Name,
                     pBasicInfo->NameLength,
                     pBasicInfo->Flags & RTL_ATOM_PINNED ? " pinned" : ""
                   );
            }
        }
}


VOID
KUserExtension(
    PCSTR lpArgumentString,
    KUSER_SHARED_DATA * const SharedData
    );


DECLARE_API( kuser )

/*++

Routine Description:

    This function is called as an NTSD extension to dump the shared user mode
    page (KUSER_SHARED_DATA)

    Called as:

        !kuser

Arguments:

    None

Return Value:

    None

--*/

{
    INIT_API();

    KUserExtension( (PCSTR)lpArgumentString, USER_SHARED_DATA );
}

#include "kuserext.c"

VOID
PebExtension(
    PCSTR lpArgumentString,
    PPEB pPeb
    );

DECLARE_API( peb )

/*++

Routine Description:

    This function is called as an NTSD extension to dump the PEB

    Called as:

        !peb

--*/

{
    NTSTATUS Status;
    ULONGLONG Address;

    INIT_API();

    Status = QueryPebAddress(hCurrentThread, &Address);
    if (!NT_SUCCESS(Status)) {
        dprintf("    Unable to query process PEB address (%x)\n", Status );
        return;
    }

    PebExtension( (PCSTR)lpArgumentString, (PPEB)Address );
}

VOID
TebExtension(
    PCSTR lpArgumentString,
    PTEB pTeb
    );

DECLARE_API( teb )

/*++

Routine Description:

    This function is called as an NTSD extension to dump the TEB

    Called as:

        !teb

--*/

{
    NTSTATUS Status;
    THREAD_BASIC_INFORMATION ThreadInformation;
    ULONGLONG Address;

    INIT_API();

    Status = QueryTebAddress(hCurrentThread, &Address);

    if (!NT_SUCCESS( Status )) {
        dprintf("    Unable to query thread TEB address (%x)\n", Status );
        return;
        }

    TebExtension( (PCSTR)lpArgumentString, (PTEB)Address);
}

#include "pebext.c"

VOID
ImageExtension(
    PSTR lpArgs
    );

DECLARE_API( dh )
{
    INIT_API();

    ImageExtension((PSTR)lpArgumentString);
    return;
}

#include "imageext.c"

///////////////////////////////////////////////////////////////////////////////

VOID
DebugPageHeapExtensionFind(
    PCSTR ArgumentString
    );

DECLARE_API( dphfind )
{
    INIT_API();
    DebugPageHeapExtensionFind( (PCSTR)lpArgumentString );
}

VOID
DebugPageHeapExtensionDump(
    PCSTR ArgumentString
    );

DECLARE_API( dphdump )
{
    INIT_API();
    DebugPageHeapExtensionDump( (PCSTR)lpArgumentString );
}

VOID
DebugPageHeapExtensionHogs(
    PCSTR ArgumentString
    );

DECLARE_API( dphhogs )
{
    INIT_API();
    DebugPageHeapExtensionHogs( (PCSTR)lpArgumentString );
}

VOID
DebugPageHeapExtensionFlags(
    PCSTR ArgumentString
    );

DECLARE_API( dphflags )
{
    INIT_API();
    DebugPageHeapExtensionFlags( (PCSTR)lpArgumentString );
}

#include "heappagx.c"

#include "hleak.c"

#include "secexts.c"



/*++

Routine Description:

    This function is called as an NTSD extension to mimic the !handle
    kd command.  This will walk through the debuggee's handle table
    and duplicate the handle into the ntsd process, then call NtQueryobjectInfo
    to find out what it is.

    Called as:

        !handle [handle [flags [Type]]]

    If the handle is 0 or -1, all handles are scanned.  If the handle is not
    zero, that particular handle is examined.  The flags are as follows
    (corresponding to secexts.c):
        1   - Get type information (default)
        2   - Get basic information
        4   - Get name information
        8   - Get object specific info (where available)

    If Type is specified, only object of that type are scanned.  Type is a
    standard NT type name, e.g. Event, Semaphore, etc.  Case sensitive, of
    course.

    Examples:

        !handle     -- dumps the types of all the handles, and a summary table
        !handle 0 0 -- dumps a summary table of all the open handles
        !handle 0 f -- dumps everything we can find about a handle.
        !handle 0 f Event
                    -- dumps everything we can find about open events

--*/
DECLARE_API( handle )
{
    HANDLE  hThere;
    DWORD   Type;
    PSTR    Args;
    DWORD   Mask;
    DWORD   HandleCount;
    NTSTATUS Status;
    DWORD   Total;
    DWORD   TypeCounts[TYPE_MAX];
    DWORD   Handle;
    DWORD   Hits;
    DWORD   Matches;
    DWORD   ObjectType;

    INIT_API();

    Mask = GHI_TYPE ;
    hThere = INVALID_HANDLE_VALUE;
    Type = 0;

    while (*lpArgumentString == ' ') {
        lpArgumentString++;
    }

    if ( strcmp( lpArgumentString, "-?" ) == 0 )
    {
        help(hCurrentProcess, hCurrentThread, dwCurrentPc,
                    lpExtensionApis, "handle" );

        return ;
    }

    hThere = (PVOID) GetExpression( lpArgumentString );

    while (*lpArgumentString && (*lpArgumentString != ' ') ) {
        lpArgumentString++;
    }
    while (*lpArgumentString == ' ') {
        lpArgumentString++;
    }

    if (*lpArgumentString) {
        Mask = (DWORD)GetExpression( lpArgumentString );
    }

    while (*lpArgumentString && (*lpArgumentString != ' ') ) {
        lpArgumentString++;
    }
    while (*lpArgumentString == ' ') {
        lpArgumentString++;
    }

    if (*lpArgumentString) {
        Type = GetObjectTypeIndex( lpArgumentString );
        if (Type == (DWORD) -1 ) {
            dprintf("Unknown type '%s'\n", lpArgumentString );
            return;
        }
    }

    //
    // if they specified 0, they just want the summary.  Make sure nothing
    // sneaks out.
    //

    if ( Mask == 0 ) {
        Mask = GHI_SILENT;
    }
    //
    // hThere of 0 indicates all handles.
    //
    if ((hThere == 0) || (hThere == INVALID_HANDLE_VALUE)) {
        Status = NtQueryInformationProcess( hCurrentProcess,
                                            ProcessHandleCount,
                                            &HandleCount,
                                            sizeof( HandleCount ),
                                            NULL );

        if ( !NT_SUCCESS( Status ) ) {
            return;
        }

        Hits = 0;
        Handle = 0;
        Matches = 0;
        ZeroMemory( TypeCounts, sizeof(TypeCounts) );

        while ( Hits < HandleCount ) {
            if ( Type ) {
                if (GetHandleInfo( hCurrentProcess,
                                   (HANDLE) Handle,
                                   GHI_TYPE | GHI_SILENT,
                                   &ObjectType ) ) {
                    Hits++;
                    if ( ObjectType == Type ) {
                        GetHandleInfo( hCurrentProcess,
                                        (HANDLE) Handle,
                                        Mask,
                                        &ObjectType );
                        Matches ++;
                    }

                }
            } else {
                if (GetHandleInfo(  hCurrentProcess,
                                    (HANDLE) Handle,
                                    GHI_TYPE | GHI_SILENT,
                                    &ObjectType) ) {
                    Hits++;
                    TypeCounts[ ObjectType ] ++;

                    GetHandleInfo(  hCurrentProcess,
                                    (HANDLE) Handle,
                                    Mask,
                                    &ObjectType );

                }
            }

            Handle += 4;
        }

        if ( Type == 0 ) {
            dprintf( "%d Handles\n", Hits );
            dprintf( "Type           \tCount\n");
            for (Type = 0; Type < TYPE_MAX ; Type++ ) {
                if (TypeCounts[Type]) {
                    dprintf("%-15ws\t%d\n", pszTypeNames[Type], TypeCounts[Type]);
                }
            }
        } else {
            dprintf("%d handles of type %ws\n", Matches, pszTypeNames[Type] );
        }


    } else {
        GetHandleInfo( hCurrentProcess, hThere, Mask, &Type );
    }

}


DECLARE_API( threadtoken )
{
    HANDLE hToken ;
    NTSTATUS Status ;

    INIT_API();

    Status = NtOpenThreadToken(
                    hCurrentThread,
                    TOKEN_READ,
                    TRUE,
                    &hToken );

    if ( !NT_SUCCESS( Status ) )
    {
        if ( Status == STATUS_ACCESS_DENIED )
        {
            //
            // Try to get around the ACL:
            //
        }

        if ( Status != STATUS_NO_TOKEN )
        {
            dprintf( "Can't open token, %d\n", RtlNtStatusToDosError( Status ) );
            return;
        }

        Status = NtOpenProcessToken(
                    hCurrentProcess,
                    TOKEN_READ,
                    &hToken );

        if ( !NT_SUCCESS( Status ) )
        {
            dprintf( "Can't open any token, %d\n", RtlNtStatusToDosError( Status ) );
            return ;
        }

        dprintf( "\n***Thread is not impersonating, using process token***\n" );
    }

    TokenInfo( hToken, 0xFFF );

    NtClose( hToken );

}



#define PAGE_ALL (PAGE_READONLY|\
                  PAGE_READWRITE|\
                  PAGE_WRITECOPY|\
                  PAGE_EXECUTE|\
                  PAGE_EXECUTE_READ|\
                  PAGE_EXECUTE_READWRITE|\
                  PAGE_EXECUTE_WRITECOPY|\
                  PAGE_NOACCESS)

VOID
printflags(
    DWORD Flags
    )
{
    switch (Flags & PAGE_ALL) {
        case PAGE_READONLY:
            dprintf("PAGE_READONLY");
            break;
        case PAGE_READWRITE:
            dprintf("PAGE_READWRITE");
            break;
        case PAGE_WRITECOPY:
            dprintf("PAGE_WRITECOPY");
            break;
        case PAGE_EXECUTE:
            dprintf("PAGE_EXECUTE");
            break;
        case PAGE_EXECUTE_READ:
            dprintf("PAGE_EXECUTE_READ");
            break;
        case PAGE_EXECUTE_READWRITE:
            dprintf("PAGE_EXECUTE_READWRITE");
            break;
        case PAGE_EXECUTE_WRITECOPY:
            dprintf("PAGE_EXECUTE_WRITECOPY");
            break;
        case PAGE_NOACCESS:
            if ((Flags & ~PAGE_NOACCESS) == 0) {
                dprintf("PAGE_NOACCESS");
                break;
            } // else fall through
        default:
            dprintf("*** Invalid page protection ***\n");
            return;
            break;
    }

    if (Flags & PAGE_NOCACHE) {
        dprintf(" + PAGE_NOCACHE");
    }
    if (Flags & PAGE_GUARD) {
        dprintf(" + PAGE_GUARD");
    }
    dprintf("\n");
}



DECLARE_API( vprot )
/*++

Routine Description:

    This debugger extension dumps the virtual memory info for the
    address specified.

Arguments:


Return Value:

--*/
{
    PVOID Address;
    DWORD ReturnLength;
    MEMORY_BASIC_INFORMATION mbi;

    INIT_API();

    while (*lpArgumentString == ' ') {
        lpArgumentString++;
    }

    Address = (PVOID)GetExpression( lpArgumentString );


    ReturnLength = VirtualQueryEx( hCurrentProcess, Address, &mbi, sizeof(mbi) );

    if (ReturnLength == 0) {
        dprintf("vprot: VirtualQueryEx failed, error = %d\n", GetLastError());
        return;
    }

    dprintf("BaseAddress:       %p\n",   mbi.BaseAddress);
    dprintf("AllocationBase:    %08x\n", mbi.AllocationBase);
    dprintf("AllocationProtect: %08x  ", mbi.AllocationProtect);
    printflags(mbi.AllocationProtect);

    dprintf("RegionSize:        %08x\n", mbi.RegionSize);
    dprintf("State:             %08x  ", mbi.State);
    switch (mbi.State) {
        case MEM_COMMIT:
            dprintf("MEM_COMMIT\n");
            break;
        case MEM_FREE:
            dprintf("MEM_FREE\n");
            break;
        case MEM_RESERVE:
            dprintf("MEM_RESERVE\n");
            break;
        default:
            dprintf("*** Invalid page state ***\n");
            break;
    }

    dprintf("Protect:           %08x  ", mbi.Protect);
    printflags(mbi.Protect);

    dprintf("Type:              %08x  ", mbi.Type);
    switch(mbi.Type) {
        case MEM_IMAGE:
            dprintf("MEM_IMAGE\n");
            break;
        case MEM_MAPPED:
            dprintf("MEM_MAPPED\n");
            break;
        case MEM_PRIVATE:
            dprintf("MEM_PRIVATE\n");
            break;
        default:
            dprintf("*** Invalid page type ***\n");
            break;
    }

}

VOID
GFlagExtension(
    PVOID pNtGlobalFlag,
    ULONG ValidBits,
    PCSTR lpArgumentString
    );


DECLARE_API( gflag )

/*++

Routine Description:

    This function is called as an NTSD extension to dump or modify
    the contents of the NtGlobalFlag variable in NTDLL

    Called as:

        !gflag [value]

    If a value is not given then displays the current bits set in
    NTDLL!NtGlobalFlag variable.  Otherwise value can be one of the
    following:

        -? - displays a list of valid flag abbreviations
        number - 32-bit number that becomes the new value stored into
                 NtGlobalFlag
        +number - specifies one or more bits to set in NtGlobalFlag
        +abbrev - specifies a single bit to set in NtGlobalFlag
        -number - specifies one or more bits to clear in NtGlobalFlag
        -abbrev - specifies a single bit to clear in NtGlobalFlag

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - Supplies the pattern and expression for this
        command.


Return Value:

    None.

--*/

{
    NTSTATUS Status;
    PROCESS_BASIC_INFORMATION ProcessInformation;
    ULONGLONG Address;

    INIT_API();

    Status = NtQueryInformationProcess( ExtensionCurrentProcess,
                                        ProcessBasicInformation,
                                        &ProcessInformation,
                                        sizeof( ProcessInformation ),
                                        NULL
                                      );

    Status = QueryPebAddress(hCurrentThread, &Address);

    if (!NT_SUCCESS( Status )) {
        dprintf("    Unable to query process PEB address (%x)\n", Status );
        return;
    }

    GFlagExtension( (PVOID)(Address + FIELD_OFFSET(PEB, NtGlobalFlag)),
                    FLG_USERMODE_VALID_BITS,
                    (PCSTR)lpArgumentString
                  );
}

#include "gflagext.c"

#include "leak.c"


#include "regexts.c"
/*++

Routine Description:

    This function is called as an NTSD extension to dump registry information

    Called as:

        !dreg -[d|w] <keyPath>[![<valueName> | *]]

Arguments:

    hCurrentProcess - Supplies a handle to the current process (at the
        time the extension was called).

    hCurrentThread - Supplies a handle to the current thread (at the
        time the extension was called).

    CurrentPc - Supplies the current pc at the time the extension is
        called.

    lpExtensionApis - Supplies the address of the functions callable
        by this extension.

    lpArgumentString - Supplies the pattern and expression for this
        command.


Return Value:

    None.

--*/

DECLARE_API( dreg )
{
    DWORD    opts = 1;

    INIT_API();


    // Skip past leading spaces
    while (*lpArgumentString == ' ')
    {
        lpArgumentString++;
    }

    if (*lpArgumentString == '-')
    {
        lpArgumentString++;
        switch (*lpArgumentString)
        {
        case 'd':
            opts = 4;
            break;
        case 'w':
            opts = 2;
            break;
        default:
            opts = 1;
            break;
        }

        if (*lpArgumentString)
        {
            // expect a space between options
            lpArgumentString++;

            // Skip past leading spaces
            while (*lpArgumentString == ' ')
            {
                lpArgumentString++;
            }
        }
    }

    Idreg(opts, lpArgumentString);
}

