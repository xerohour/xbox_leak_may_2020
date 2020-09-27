/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    coverage.cpp

Abstract:

    This module implements routines to collect coverage data from Xbox.
    It is loaded by XBDM on Xbox machine (devkit only).

--*/

#include "coveragep.h"

PDMN_SESSION DmNotifySession;


/*
Routine Description:

    Simple local version of atoi until the crt is no longer broken:

        libcntpr.lib(atox.obj) : error LNK2019: unresolved 
        external symbol __ismbcspace referenced in function _atol

Arguments:

    see atoi()

Return Value:

    see atoi()

*/
int myatoi(char *src)
    {
    if(!src) return 0;
    int value = 0;

    while(*src)
        {
        if(*src == ' ' || *src == '\t' || *src == '\r' || *src == '\n') break;
        value *= 10;
        value += *src - '0';
        ++src;
        }

    return value;
    }



HRESULT
CoverageSaveData(
    IN  PCSTR  szCmd,
    OUT PSTR   szResp,
    IN  SIZE_T cchResp
    )
/*++

--*/
{
    KdPrint(( "COVERAGE: save data\n" ));
    return XBDM_BUFFER_TOO_SMALL;
}

HRESULT
CoverageResetData(
    IN  PCSTR  szCmd,
    OUT PSTR   szResp,
    IN  SIZE_T cchResp
    )
/*++

Routine Description:

    Resets (discards) any existing coverage information that has not been
    polled and saved for one or all running coverage binaries.  This can
    be useful to "zero" any coverage information prior to running a
    specific test.

Arguments:

    szCommand - The string of the reset command.  The format for this
        command is in the following form

        "BBCOVER!Cmd=CmdId Only=<BinaryName>"

    szResponse - Buffer for response results

    cchResponse - Size of response buffer in character

Return Value:

    HRESULT of the operation

--*/
{
    NTSTATUS Status;
    PCSTR BinaryName;
    HRESULT hr;

    BinaryName = strstr( szCmd, "Only=" );
    if ( BinaryName ) {
        BinaryName += sizeof("Only=") - 1;
    }

    Status = CoverageFlushLogging( BinaryName );

    if ( NT_SUCCESS(Status) ) {
        hr = XBDM_NOERR;
    } else {
        SetResponseError( szResp, cchResp, ERROR_MOD_NOT_FOUND );
        hr = XBDM_NOMODULE;
    }

    return hr;
}

HRESULT
CoverageSetPeriodic(
    IN  PCSTR  szCmd,
    OUT PSTR   szResp,
    IN  SIZE_T cchResp
    )
/*++

Routine Description:

    Automatically polls and saves all current coverage information
    every <Frequency> seconds.  This option should rarely be used,
    typically only when anticipating a system crash before "covercmd
    /Save" can be issued.  Note that an application crash will not
    cause any coverage information to be lost.  The /Save and /Reset
    options cannot be used while periodic logging is active.

Arguments:

    szCommand - The string of the periodic command.  The format for this
        command is in the following form

        "BBCOVER!Cmd=CmdId Frequency=<freq>"

    szResponse - Buffer for response results

    cchResponse - Size of response buffer in character

Return Value:

    HRESULT of the operation

--*/
{
    KdPrint(( "COVERAGE: set periodic\n" ));
    return XBDM_BUFFER_TOO_SMALL;
}

HRESULT
CoverageListBinary(
    IN  PCSTR  szCmd,
    OUT PSTR   szResp,
    IN  SIZE_T cchResp
    )
/*++

Routine Description:

    Display the list of coverage binaries that are running or have
    run.  This can be useful to determine which binary names can be
    used with "/Only" for "/Save" or "/Reset".

Arguments:

    szCommand - The string of the list command.  The format for this
        command is in the following form

        "BBCOVER!Cmd=CmdId"

    szResponse - Buffer for response results

    cchResponse - Size of response buffer in character

Return Value:

    HRESULT of the operation

--*/
{
    PSTR CurrentBuffer = szResp;
    PREGISTERED_BINARY Node;
    SIZE_T BufferSize = 0;
    SIZE_T NodeCount = 0;
    HRESULT hr;

    //
    // Calculate size of return buffer for list of registered modules
    // The returned buffer is array of string pointer
    //

    AcquireRegistrationLock();

    Node = GetRegisteredBinaryList();
    while ( Node != GetRegisteredBinaryListHead() ) {

        //
        // The length of binary name itself plus the new line character
        //

        BufferSize += strlen(Node->BinaryName) + sizeof(' ');
        NodeCount++;
    }

    //
    // Including the last null terminator
    //

    if ( NodeCount ) {
        BufferSize += sizeof('\0');
    } else {
        BufferSize = sizeof("None");
    }

    if ( cchResp < BufferSize ) {

        SetResponseError( szResp, cchResp, ERROR_INSUFFICIENT_BUFFER );
        hr = XBDM_BUFFER_TOO_SMALL;

    } else {

        //
        // Loop over again and this time copy data to the output buffer
        //

        if ( !NodeCount ) {
            strcpy( CurrentBuffer, "None" );
        } else {

            Node = GetRegisteredBinaryList();
            while ( Node != GetRegisteredBinaryListHead() ) {
                strcpy( CurrentBuffer, Node->BinaryName );
                CurrentBuffer += strlen(CurrentBuffer);
                *CurrentBuffer++ = ' ';
            }

            *CurrentBuffer = '\0';
        }

        hr = XBDM_NOERR;
    }

    ReleaseRegistrationLock();

    return hr;
}

HRESULT
WINAPI
CoverageCmdProc(
    PCSTR  szCmd,
    PSTR   szResp,
    SIZE_T cchResp,
    PDM_CMDCONT /* pDmCc */
    )
/*++

Routine Description:

    This is a callback routine that process coverage commands sent from
    remote machine.  The command format is

    "BBCOVER!Cmd=id OptionalCmd=string ..."

Arguments:

    szCommand - The string of command mentioned above

    szResponse - Buffer for response results

    cchResponse - Size of response buffer in character

    pDmCc - Unused

Return Value:

    HRESULT

--*/
{
    PCSTR CmdIdPtr;
    ULONG CmdId;
    HRESULT hr = E_INVALIDARG;

    ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

    //
    // Looking for the command id in the command string and process it
    //

    CmdIdPtr = strstr( szCmd, COVERAGE_COMMAND_HEADER );

    if ( CmdIdPtr ) {

        CmdIdPtr += sizeof(COVERAGE_COMMAND_HEADER) - 1;
        CmdId = myatoi( (char*)CmdIdPtr );

        switch ( CmdId ) {

        case CoverageCmdSave:
            hr = CoverageSaveData( szCmd, szResp, cchResp );
            break;

        case CoverageCmdReset:
            hr = CoverageResetData( szCmd, szResp, cchResp );
            break;

        case CoverageCmdPeriodic:
            hr = CoverageSetPeriodic( szCmd, szResp, cchResp );
            break;

        case CoverageCmdList:
            hr = CoverageListBinary( szCmd, szResp, cchResp );
            break;

        default:
            KdPrint(( "COVERAGE: invalid command id (%u)", CmdId ));
        }

    } else {
        DbgPrint( "COVERAGE: invalid command '%s'\n", szCmd );
    }

    return hr;
}

DWORD
WINAPI
CoverageNotifyHandler(
    IN ULONG Notification,
    IN DWORD Parameter
    )
/*++

Routine Description:

    This is a notification routine called when module load/unload and
    execution event occur.  We monitor these events and save coverage
    information as needed.

Arguments:

    Notification - Type of notification.  This parameter can be
        DM_EXEC, DM_MODLOAD, DM_MODUNLOAD

    Parameter - Optional parameter based on type of notification

Return Value:

    Always zero

--*/
{
    PDMN_MODLOAD DmModLoad;

    ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

    switch ( Notification ) {

    case DM_MODLOAD:
        DmModLoad = (PDMN_MODLOAD)Parameter;
        RegisterNoImportCoverageBinary( DmModLoad->BaseAddress );
        break;

    case DM_EXEC:
        if ( Parameter == DMN_EXEC_REBOOT ) {
            KdPrint(( "COVERAGE: system reboot, saving coverage data...\n\n" ));
            CoverageInternalCloseLogFileAllBinaries();
        }
        break;

    }

    return 0;
}

EXTERN_C
VOID
WINAPI
CoverageAbsEntry(
    VOID
    )
/*++

Routine Description:

    This is an absolute entry point of the module.  It registers XBDM
    external command processor and registers notification event to see
    module load and unload notification.

Arguments:

    None

Return Value:

    None

--*/
{
    HRESULT hr;
    PDM_WALK_MODULES DmWalkModules;
    DMN_MODLOAD DmModLoad;

    //
    // Register XBDM external command processor for code coverage command and
    // register notification handler so that when the module is loaded, unload,
    // or system reboot, its coverage data will be saved.
    //

    hr = DmRegisterCommandProcessor( DM_COVERAGE_COMMAND, CoverageCmdProc );

    if ( SUCCEEDED(hr) ) {
        hr = DmOpenNotificationSession( DM_PERSISTENT, &DmNotifySession );
        if ( FAILED(hr) ) {
            KdPrint(( "COVERAGE: DmOpenNotificationSession failed\n" ));
        }
    }

    if ( SUCCEEDED(hr) ) {
        hr = DmNotify( DmNotifySession, DM_MODLOAD, CoverageNotifyHandler );
        if ( FAILED(hr) ) {
            KdPrint(( "COVERAGE: DmNotify(DM_MODLOAD) failed\n" ));
        }
    }

    if ( SUCCEEDED(hr) ) {
        hr = DmNotify( DmNotifySession, DM_MODUNLOAD, CoverageNotifyHandler );
        if ( FAILED(hr) ) {
            KdPrint(( "COVERAGE: DmNotify(DM_MODUNLOAD) failed\n" ));
        }
    }

    if ( SUCCEEDED(hr) ) {
        hr = DmNotify( DmNotifySession, DM_EXEC, CoverageNotifyHandler );
        if ( FAILED(hr) ) {
            KdPrint(( "COVERAGE: DmNotify(DM_EXEC) failed\n" ));
        }
    }

    if ( FAILED(hr) ) {
        DbgPrint( "COVERAGE: unable to load extension (hr=0x%x)\n", hr );
        return;
    }

    if ( !CoverageInitPoolBuffer() ) {
        DbgPrint( "COVERAGE: unable to allocate poll buffer\n" );
        return;
    }

    DbgPrint( "COVERAGE: coverage engine loaded\n" );

    //
    // Try to register all loaded modules.  Module will be registered if it
    // has .cover section in its PE image
    //

    DmWalkModules = NULL;

    while ( SUCCEEDED(DmWalkLoadedModules(&DmWalkModules, &DmModLoad)) ) {
        RegisterNoImportCoverageBinary( DmModLoad.BaseAddress );
    }

    if ( DmWalkModules ) {
        DmCloseLoadedModules( DmWalkModules );
    }
}

