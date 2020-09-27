/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    reboot.cpp

Abstract:

    This program contains routines to test the reboot API, only public
    API is tested currently.

--*/

#include <precomp.h>

typedef enum {
    RebootStateStartTest = 0x0000,
    RebootStateEndTest   = 0x0001,
    RebootStateHDDTest   = 0x1000,
    RebootStateHDDVerify = 0x1001,
    RebootStateDVDTest  = 0x2000,
    RebootStateDVDVerify = 0x2001
} RebootTestStates;

#define REBOOT_TEST_HDD_NT_PATH     OTEXT("\\??\\Z:\\reboot.xbe")
#define REBOOT_TEST_HDD_DOS_PATH    OTEXT("z:\\reboot.xbe")

#define REBOOT_TEST_LAUNCH_DATA     ((PLAUNCH_DATA)OTEXT("Reboot Test Launch Data"))

BOOL
SetRebootTestState(
    ULONG NewState,
    PULONG State OPTIONAL
    )
/*++

Routine Description:

    This routine stores state number to testini.ini before we perform a reboot

Arguments:

    State - 32-bit number

Return Value:

    TRUE/FALSE

--*/
{
    TCHAR Buffer[32];
    _stprintf( Buffer, TEXT("0x%x"), NewState );
    if ( ARGUMENT_PRESENT(State) ) {
        *State = NewState;
    }
    return WriteProfileString( TEXT("reboot"), TEXT("State"), Buffer );
}

ULONG
GetRebootTestState(
    VOID
    )
/*++

Routine Description:

    This routine gets cookie number from testini.ini

Arguments:

    None

Return Value:

    32-bit cookie number

--*/
{
    return GetProfileInt( TEXT("reboot"), TEXT("State"), ~0UL );
}

NTSTATUS
RebootTestCopyImage(
    PCOSTR Destination
    )
/*++

Routine Description:

    This routine copies the current test image to Destination

Arguments:

    Destination - full path name of destination

Return Value:

    TRUE/FALSE

--*/
{
    NTSTATUS Status = STATUS_NO_MEMORY;
    OBJECT_ATTRIBUTES ObjA;
    IO_STATUS_BLOCK IoStatusBlock;
    OBJECT_STRING DestFileName;
    HANDLE ImageFileHandle = INVALID_HANDLE_VALUE;
    HANDLE DestFileHandle = INVALID_HANDLE_VALUE;
    LPVOID Buffer = NULL;
    const ULONG BufferSize = 64 * 1024UL;

    __try {

        Buffer = VirtualAlloc( NULL, BufferSize, MEM_COMMIT, PAGE_READWRITE );
        if ( !Buffer ) {
            Status = STATUS_NO_MEMORY;
            __leave;
        }

        InitializeObjectAttributes(
            &ObjA,
            XeImageFileName,
            OBJ_CASE_INSENSITIVE,
            NULL,
            NULL
            );

        //
        // Open source current image file from loader structure
        //
        Status = NtOpenFile(
                    &ImageFileHandle,
                    SYNCHRONIZE | GENERIC_READ,
                    &ObjA,
                    &IoStatusBlock,
                    FILE_SHARE_READ,
                    FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE | \
                        FILE_SEQUENTIAL_ONLY
                    );

        if ( !NT_SUCCESS(Status) ) {
            __leave;
        }

        RtlInitObjectString( &DestFileName, Destination );

        InitializeObjectAttributes(
            &ObjA,
            &DestFileName,
            OBJ_CASE_INSENSITIVE,
            NULL,
            NULL
            );

        Status = NtCreateFile(
                    &DestFileHandle,
                    SYNCHRONIZE | GENERIC_WRITE,
                    &ObjA,
                    &IoStatusBlock,
                    NULL,
                    FILE_ATTRIBUTE_NORMAL,
                    FILE_SHARE_READ,
                    FILE_OVERWRITE_IF,
                    FILE_WRITE_THROUGH | FILE_SYNCHRONOUS_IO_NONALERT | \
                        FILE_SEQUENTIAL_ONLY
                    );

        if ( !NT_SUCCESS(Status) ) {
            __leave;
        }

        for (;;) {
            Status = NtReadFile(
                        ImageFileHandle,
                        NULL,
                        NULL,
                        NULL,
                        &IoStatusBlock,
                        Buffer,
                        BufferSize,
                        NULL
                        );

            if ( Status == STATUS_PENDING ) {
                //
                // Operation must complete before return and IoStatusBlock destroyed
                //
                Status = NtWaitForSingleObject( ImageFileHandle, FALSE, NULL );
                if ( NT_SUCCESS(Status) ) {
                    Status = IoStatusBlock.Status;
                }
            }

            if ( NT_SUCCESS(Status) ) {
                Status = NtWriteFile(
                            DestFileHandle,
                            NULL,
                            NULL,
                            NULL,
                            &IoStatusBlock,
                            Buffer,
                            IoStatusBlock.Information,
                            NULL
                            );

                if ( Status == STATUS_PENDING) {
                    //
                    // Operation must complete before return and IoStatusBlock destroyed
                    //
                    Status = NtWaitForSingleObject( DestFileHandle, FALSE, NULL );
                    if ( NT_SUCCESS(Status)) {
                        Status = IoStatusBlock.Status;
                    }
                }

                if ( !NT_SUCCESS(Status) ) {
                    __leave;
                }

            } else if ( Status == STATUS_END_OF_FILE ) {
                Status = STATUS_SUCCESS;
                __leave;
            } else {
                __leave;
            }
        }

    } __finally {

        if ( Buffer ) {
            VirtualFree( Buffer, 0, MEM_RELEASE );
        }

        if ( ImageFileHandle != INVALID_HANDLE_VALUE ) {
            NtClose( ImageFileHandle );
        }

        if ( DestFileHandle != INVALID_HANDLE_VALUE ) {
            NtClose( DestFileHandle );
        }
    }

    return Status;
}

BOOL
WINAPI
RebootTestVerifyLaunchData(
    VOID
    )
/*++

    This is a helper routine used to verify the launch data retreived from
    XGetLaunchInfo

Arguments:

    None

Return Value:

    TRUE if the launch data matches the data passed to XLaunchNewImage.
    FALSE otherwise

--*/
{
    DWORD Type;
    LAUNCH_DATA Data;

    XGetLaunchInfo( &Type, &Data );
    if ( Type == LDT_TITLE && !strcmp((PCSTR)REBOOT_TEST_LAUNCH_DATA, (PCSTR)&Data) ) {
        return TRUE;
    }

    KdPrint(( "REBOOT: invalid launch data\n" ));
__asm int 3;
    return FALSE;
}

VOID
WINAPI
RebootStartTest(
    HANDLE LogHandle
    )
/*++

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

--*/
{
    ULONG State;
    DWORD FileAttributes;
    DWORD ErrorCode;
    BOOL Done = FALSE;
    NTSTATUS Status;

    KdPrint(( "REBOOT: StartTest\n" ));
    xSetComponent( LogHandle, "xapi", "Reboot APIs" );

    State = GetRebootTestState();

    while ( !Done ) {

        switch ( State ) {

        case RebootStateHDDTest:
            Status = RebootTestCopyImage(REBOOT_TEST_HDD_NT_PATH);
            if ( NT_SUCCESS(Status) ) {
                SetRebootTestState( RebootStateHDDVerify, &State );
                ErrorCode = XLaunchNewImage( REBOOT_TEST_HDD_DOS_PATH,
                                             REBOOT_TEST_LAUNCH_DATA );

                //
                // If we get here that means we couldn't reboot
                //
                KdPrint(( "REBOOT: variation %x pass\n", State ));
                SetRebootTestState( RebootStateDVDTest, &State );
            } else {
                KdPrint(( "REBOOT: unable to copy image file (%s)\n",
                          NtStatusSymbolicName(Status) ));
                SetRebootTestState( RebootStateDVDTest, &State );
            }
            break;

        case RebootStateHDDVerify:
            //
            // If we get here that means we actually reboot
            //
            RebootTestVerifyLaunchData();
            KdPrint(( "REBOOT: variation %x failed\n", State ));
            SetRebootTestState( RebootStateDVDTest, &State );
            break;

        case RebootStateDVDTest:
            SetRebootTestState( RebootStateDVDVerify, &State );
            FileAttributes = GetFileAttributes(OTEXT("d:\\default.xbe"));
            if ( FileAttributes == (DWORD)-1 ) {
                KdPrint(( "REBOOT: test blocked, no d:\\default.xbe\n" ));
            } else {
                ErrorCode = XLaunchNewImage( OTEXT("d:\\default.xbe"),
                                             REBOOT_TEST_LAUNCH_DATA );
                if ( ErrorCode != ERROR_SUCCESS ) {
                    SetRebootTestState( RebootStateEndTest, &State );
                    KdPrint(( "REBOOT: variation %x XLaunchNewImage failed (%s)\n",
                              State, WinErrorSymbolicName(ErrorCode) ));
                }
            }
            break;

        case RebootStateDVDVerify:
            RebootTestVerifyLaunchData();
            KdPrint(( "REBOOT: variation %x pass\n", State ));
            SetRebootTestState( RebootStateEndTest, &State );
            break;

        case RebootStateStartTest:
            SetRebootTestState( RebootStateHDDTest, &State );
            break;

        case RebootStateEndTest:
            Done = TRUE;
            break;

        default:
            KdPrint(( "REBOOT: invalid state\n" ));
            SetRebootTestState( RebootStateStartTest, &State );
        }
    }
}

VOID
WINAPI
RebootEndTest(
    VOID
    )
/*++

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

--*/
{
    KdPrint(( "REBOOT: EndTest\n" ));
    SetRebootTestState( RebootStateStartTest, 0 );

    //
    // TODO: save log from .ini to xLog file
    //
}

void __cdecl main( void )
{
    RebootStartTest( INVALID_HANDLE_VALUE );
    RebootEndTest();
    XLaunchNewImage( 0, 0 );
}
