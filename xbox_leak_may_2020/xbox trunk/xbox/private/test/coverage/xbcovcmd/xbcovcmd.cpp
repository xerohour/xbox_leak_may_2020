/*++

Copyright (c) Microsoft Coroporation. All rights reserved.

Module Name:

    xbcovcmd.cpp

Abstract:

    This module implements routines to send commands to coverage debug
    extension (coverage.dxt).  It can be used for collecting code coverage
    data.

--*/

#include "xbcovcmdp.h"

VOID
TranslateDmResult(
    IN HRESULT hr
    )
/*++

Routine Description:

    This routine translates HRESULT returned from XboxDbg APIs into string
    and output to standard error

Auguments:

    hr - HRESULT returned from various XboxDbg APIs

Return Value:

    None

--*/
{
    CHAR Buffer[512];

    DmTranslateErrorA( hr, Buffer, sizeof(Buffer) );
    fprintf( stderr, Buffer );
}

VOID
VerifyDmResult(
    IN HRESULT hr,
    IN BOOL fTerminate = FALSE
    )
/*++

Routine Description:

    This routine translates HRESULT and output to standard error output if
    the result indicates failure code.  It also terminates the process if
    the parameter fTerminate is non-zero

Arguments:

    hr - HRESULT returned by various XboxDbg APIs

    fTerminate - Set to TRUE to terminate the process if the specified HRESULT
        indicates failure code

Return Value:

    None or terminate the process if specified hr is a failure code

--*/
{
    if ( FAILED(hr) ) {
        TranslateDmResult( hr );
        if ( fTerminate ) {
            exit( 1 );
        }
    }
}

VOID
VerifyWin32Error(
    IN DWORD ErrorCode,
    IN BOOL fTerminate = FALSE
    )
/*++

Routine Description:

    This routine translates Win32 error code and output to standard error
    output if the result indicates failure code.  It also terminates the
    process if the parameter fTerminate is non-zero

Arguments:

    ErrorCode - Win32 error code

    fTerminate - Set to TRUE to terminate the process if the specified HRESULT
        indicates failure code

Return Value:

    None or terminate the process if specified hr is a failure code

--*/
{
    PSTR lpMsgBuf;

    if ( ErrorCode == ERROR_SUCCESS ) {
        return;
    }

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | \
        FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0,
        NULL );

    fprintf( stderr, lpMsgBuf );

    LocalFree( lpMsgBuf );

    if ( fTerminate ) {
        exit( 1 );
    }
}

VOID
DisplayHelpMessage(
    VOID
    )
/*++

Routine Description:

    This routine displays help message of available parameters and terminate
    the process

Arguments:

    None

Return Value:

    None

--*/
{
    fputs( "Usage:  XBCOVCMD </Save, /Reset, /Periodic or /List] [/Machine:XboxName]\n"
           "\n"
           "\t  /Save  [/Only <BinaryName>]  [/As \"<TestName>\"]\n\n"
           "\t  Polls and saves current coverage information to the database file\n"
           "\t  %coverage%\\<BinaryName>\\<BinaryName>.covdata for one or all\n"
           "\t  running coverage binaries and optionally tags the polled coverage\n"
           "\t  information with a given test name for subsequent reporting.  The\n"
           "\t  current coverage information is automatically reset when polled.\n\n"
           "\t  It is not necessary to poll and save coverage information before\n"
           "\t  a binary is unloaded (process termination, etc).  All coverage\n"
           "\t  information is retained until covercmd \"/Save\" or \"/Reset\" is\n"
           "\t  executed.  Any coverage information that has not been saved when\n"
           "\t  system shutdown occurs is automatically saved during shutdown.\n"
           "\n"
           "\t/Reset [/Only <BinaryName>]\n"
           "\n"
           "\t   Resets (discards) any existing coverage information that has not\n"
           "\t   been polled and saved for one or all running coverage binaries.\n"
           "\t   This can be useful to \"zero\" any coverage information prior to\n"
           "\t   running a specific test.\n"
           "\n"
           "\t/Periodic <Frequency>\n"
           "\n"
           "\t   Automatically polls and saves all current coverage information\n"
           "\t   every <Frequency> seconds.  This option should rarely be used,\n"
           "\t   typically only when anticipating a system crash before \"xbcovcmd\n"
           "\t   /Save\" can be issued.  Note that an application crash will not\n"
           "\t   cause any coverage information to be lost.  The /Save and /Reset\n"
           "\t   options cannot be used while periodic logging is active.\n"
           "\n"
           "\t/Periodic /Stop\n"
           "\n"
           "\t   Stops periodic logging.\n"
           "\n"
           "\t/List\n"
           "\n"
           "\t   Display the list of coverage binaries that are running or have\n"
           "\t   run.  This can be useful to determine which binary names can be\n"
           "\n"
           "\t/Machine:XboxName\n"
           "\n"
           "\t   Specify the machine name of the Xbox\n",
           stderr );
    exit( 1 );
}

VOID
CoverageSetXboxName(
    IN PCSTR XboxName
    )
/*++

Routine Description:

    This routine set active Xbox name for future uses.  The configuration will
    be persistently saved and will affect most of XDK tools

Arguments:

    XboxName - Name or IP address of Xbox

Return Value:

    None

--*/
{
    VerifyDmResult( DmUseSharedConnection(TRUE), TRUE );
    VerifyDmResult( DmSetXboxName(XboxName), TRUE );
    DmUseSharedConnection( FALSE );
}

VOID
CoverageListBinary(
    VOID
    )
/*++

Routine Description:

    This routine sends command to coverage debug extension to get the list
    of binaries on Xbox machine.

Arguments:

    None

Return Value:

    None

--*/
{
    DWORD Error;
    PCHAR Buffer = NULL;
    SIZE_T BufferSize = 65536;

    VerifyDmResult( DmUseSharedConnection(TRUE), TRUE );

    //
    // Continue to send the command as long as we receive the error
    // indicating that the output buffer is not large enough
    //

    do {

        Buffer = (PCHAR)realloc( Buffer, BufferSize );
        if ( !Buffer ) {
            Error = ERROR_NOT_ENOUGH_MEMORY;
            break;
        }

        Error = CoverageSendCommand( CoverageCmdList, Buffer, &BufferSize, 0 );
        if ( Error == ERROR_INSUFFICIENT_BUFFER ) {
            BufferSize += 4096;
        }

    } while ( Error == ERROR_INSUFFICIENT_BUFFER );

    if ( Error == ERROR_SUCCESS ) {
        fprintf( stdout, "%s\n", GET_RESPONSE(Buffer) );
    } else {
        VerifyWin32Error( Error );
    }

    free( Buffer );

    DmUseSharedConnection( FALSE );
}

VOID
CoverageResetData(
    PCSTR BinaryName OPTIONAL
    )
/*++

Routine Description:

    This routine sends command to coverage debug extension to flush coverage
    data.  If BinaryName is specified, only coverage data for BinaryName
    will be flushed.

Arguments:

    BinaryName - Optional name of binary.  If not specified, all coverage
        data will be flushed

Return Value:

    None

--*/
{
    DWORD Error;

    Error = CoverageSendCommand( CoverageCmdReset, 0, 0, BinaryName ? \
                                 "Only=%s" : 0, BinaryName );

    if ( Error != ERROR_SUCCESS ) {
        VerifyWin32Error( Error );
    }
}

void __cdecl main( int argc, char **argv )
{
    int i;
    char *arg;
    DWORD dwFrequency = 0;
    BOOL bList = FALSE;
    BOOL bSave = FALSE;
    BOOL bReset = FALSE;
    BOOL bPeriodic = FALSE;
    BOOL bPeriodicStop = FALSE;
    PSTR pszMachine = NULL;
    PSTR pszTestName = NULL;
    PSTR pszOnlyBinary = NULL;

    for ( i=1; i<argc; i++) {
        arg = argv[i];

        if ( *arg == '/' || *arg == '-' ) {
            _strlwr( ++arg );

            if ( !strcmp(arg, "save") ) {
                bSave = TRUE;
            } else if ( !strcmp(arg, "reset") ) {
                bReset = TRUE;
            } else if ( !strcmp(arg, "list") ) {
                bList = TRUE;
            } else if ( strstr(arg, "machine") ) {
                pszMachine = strchr( arg, ':' ) + 1;
            } else if ( !strcmp(arg, "only") ) {
                pszOnlyBinary = argv[ ++i ];
            } else if ( !strcmp(arg, "as") ) {
                pszTestName = argv[ ++i ];
            } else if ( !strcmp(arg, "periodic") ) {
                bPeriodic = TRUE;
                if ( argv[i+1] && isdigit(argv[i+1][0]) ) {
                    dwFrequency = strtoul( argv[++i], NULL, 0 );
                }
            } else if ( !strcmp(arg,"stop") ) {
                bPeriodicStop = TRUE;
            } else {
                DisplayHelpMessage();
            }
        } else {
            DisplayHelpMessage();
        }
    }

    if (( ! ( bSave ^ bReset ^ bList ^ bPeriodic)) ||     // one and only one selected
        (( ! bPeriodic ) && ( bPeriodicStop || dwFrequency )) ||
        (( bPeriodicStop ) && ( dwFrequency )) ||
        (( ! bSave ) && ( pszTestName )) ||
        (( pszOnlyBinary ) && ( ! ( bSave || bReset))) ||
        (( pszOnlyBinary ) && ( ! ( *pszOnlyBinary )))) {
       DisplayHelpMessage();
    }

    if (( bPeriodic ) && ( ! bPeriodicStop ) && ( dwFrequency == 0 )) {
        dwFrequency = 30;
    }

    if ( pszMachine ) {
        CoverageSetXboxName( pszMachine );
    }

    if ( bSave ) {
        //CoverageSaveData( pszOnlyBinary, pszTestName );
    } else if ( bReset ) {
        CoverageResetData( pszOnlyBinary );
    } else if ( bList ) {
        CoverageListBinary();
    } else if ( bPeriodic ) {
        if ( bPeriodicStop ) {
            //CoverageStopPeriodic();
        } else {
            //CoverageSetPeriodic( dwFrequency );
        }
    }
}

