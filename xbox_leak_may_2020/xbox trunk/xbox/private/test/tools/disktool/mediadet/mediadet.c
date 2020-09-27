/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

   media detect - mediadet.c

Abstract:

   detect media, break if failed

Author:

   John Daly

Notes:


--*/

#include "mediadet.h"

HANDLE hDuke[XGetPortCount()] = { NULL, NULL, NULL, NULL};

void 
__cdecl 
main(
    void
    )
/*++

Routine Description:

    main

Arguments:

    void

Return Value:

    void
    
Note:

--*/ 
{
    ULARGE_INTEGER FreeBytesAvailable;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;
    HANDLE hFile;
    DWORD BytesRead;
    DWORD x = 0;
    ANSI_STRING ostDrive, ostPath;
    NTSTATUS status;

    XInitDevices(0, NULL);
    Sleep(2000); // Give the USB time to enumerate any attached devices

    //
    // look at the size in a:\
    // keep running log of sizes found, the person running the test has to 
    // determine what is a pass or fail detection...
    // keep 10 detection size slots open (probably only ever use 3 (0 for fail, 
    // size x for movie partition, size y for game partition))
    //

    xCreateConsole(NULL);
    xSetBackgroundImage(NULL);
    printf("        mediadet - does reboot and media detect\n");
    printf("        starting test\n");

    //
    // open the log file
    //

    hFile = CreateFileA("t:\\Counter", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        DebugBreak();
    }

    ReadFile(hFile, (LPVOID)&fbuffer, sizeof(fbuffer), &BytesRead, NULL);
    // on first read this will fail, maybe check to see if we need to create then check, or don't check ever
    //if (BytesRead != sizeof(fbuffer)) {
    //    DebugBreak();
    //}

    //
    // detect controller, select reboot type, or end test and restore box
    //

    //
    // map drive c:\
    //

    RtlInitObjectString( &ostDrive, "\\??\\c:" );
    RtlInitObjectString( &ostPath, "\\Device\\Harddisk0\\Partition1" );
    status = IoCreateSymbolicLink( &ostDrive, &ostPath );
    
    RebootType = RebootMenu();

    //
    // change reboot type if neccessary
    //

    if (RebootType) {
        fbuffer.RebootType = RebootType;
    }

    //
    // get the space, and look for the appropriate slot to increment
    //

    if (FALSE == GetDiskFreeSpaceEx("A:\\",                 // directory name
                                    &FreeBytesAvailable,    // bytes available to caller
                                    &TotalNumberOfBytes,    // bytes on disk
                                    &TotalNumberOfFreeBytes // free bytes on disk
                                   )) {
        //
        // failed to read case, I am not going to assume that TotalNumberOfBytes gets set to 0...
        //

        TotalNumberOfBytes.QuadPart = 0;

    } else {

        for (x = 0; x < DATASLOTS; x++) {

            if (fbuffer.SizesDetected == x) {

                //
                // we detected a new size
                //

                ++fbuffer.SizesDetected;
                fbuffer.DetectData[x].SizeDetected.QuadPart = TotalNumberOfBytes.QuadPart;
                fbuffer.DetectData[x].SizeDetectedCount = 1;
                break;
                
            } else {

                //
                // we detected a previously detected size
                //

                if (TotalNumberOfBytes.QuadPart == fbuffer.DetectData[x].SizeDetected.QuadPart) {
                    ++fbuffer.DetectData[x].SizeDetectedCount;
                    break;
                }
            }
        }
    }

    //
    // we are at the end of the buffer, we have not overwritten, but this
    // is very unexpected and needs to be debugged
    //

    if (x == DATASLOTS) {
        DebugBreak();
    }

    DumpStats();

    sprintf(buffer, "On this iteration, size detected : %#16.16I64x\n", TotalNumberOfBytes.QuadPart);
    printf(buffer);

    SetFilePointer(hFile,0L, NULL, FILE_BEGIN);
    ++fbuffer.iteration;
    WriteFile(hFile, (LPVOID)&fbuffer, sizeof(fbuffer), &BytesRead, NULL);
    CloseHandle(hFile);
    
    //
    // let IO complete - may be unneccessary, better safe than blowing a test run
    //

    Sleep(200);

    //
    // conditionally self-reboot
    //

    if (fbuffer.RebootType == HARD_REBOOT) {
        HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_RESET, SMC_RESET_ASSERT_POWERCYCLE);
    }

    if (fbuffer.RebootType == SOFT_REBOOT) {

        //
        // boot default title, which is this app
        //

        HalReturnToFirmware(HalRebootRoutine);
    }

    if (fbuffer.RebootType == SMC_RESET) {
        HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_RESET, SMC_RESET_ASSERT_RESET);
    }

    //
    // if we return, we get to hit an asster in the CRTs because main exited
    // so we sleep instead to get the SMC time to reboot us
    //

    while (TRUE);
}

DWORD 
__cdecl 
RebootMenu(
    void
    )
/*++

Routine Description:

    RebootMenu - detect controller, if controller found, display menu and stats, fix box if neccessary

Arguments:

    void

Return Value:

    SOFT_REBOOT
    HARD_REBOOT
    0 (no controller detected)
    
Note:

    Josh did the kick-ass USB part for this
    
    input:
        
        A - run Soft reboot
        B - run Hard reboot
        X - quit and restore box
        Y - continue running currently running test
        W (White) - reset all counters
        B (Black) - use SMC_RESET reboot

--*/ 
{
    HANDLE hFile;
    BOOL bRetVal;
    DWORD returnval;
    DWORD x = 0;
    DWORD addDevice, removeDevice;

    //
    // look for restore copy of dashboard.xbx, if it is not there, copy current version to it
    //

    hFile = CreateFileA("c:\\dashboard_backup.xbx", 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        bRetVal = CopyFileA("c:\\dashboard.xbx", "c:\\dashboard_backup.xbx", FALSE);

        //
        // check for failure just in case...
        //

        if (FALSE == bRetVal) {
            DebugBreak();
        }

    } else {
        CloseHandle(hFile);
    }

    //
    // look for our substitute dashboard.xbx, if it is not there, we don't run
    //

    hFile = CreateFileA("c:\\mediadet.xbx", 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {

        //
        // add instructions for running the test someday...
        //

        DebugBreak();

    } else {
        CloseHandle(hFile);
    }

    //
    // no USB detected, return 0
    //
    
    addDevice = XGetDevices(XDEVICE_TYPE_GAMEPAD);
    if (0 == addDevice) {
        return 0;
    }

    InputDukeInsertions(addDevice, 0);
    
    //
    // present menu and stats
    //

    sprintf(buffer, "controller detected, test paused, please read the menu\nCurrent Stats:\n");
    printf(buffer);

    DumpStats();

    sprintf(buffer, "Instructions:\n");
    printf(buffer);

    sprintf(buffer, "A - run Soft reboot\n");
    printf(buffer);
    sprintf(buffer, "B - run Hard reboot\n");
    printf(buffer);
    sprintf(buffer, "B (Black) - run SMC_RESET reboot\n");
    printf(buffer);
    sprintf(buffer, "X - quit and restore box\n");
    printf(buffer);
    sprintf(buffer, "Y - continue running currently running test\n");
    printf(buffer);
    sprintf(buffer, "W (White) - reset all counters\n");
    printf(buffer);

    //
    // get input and do the right thing
    //

    while (1) {

        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);

        //
        // if A
        //

        if (InputCheckButton(XINPUT_GAMEPAD_A)) {
            bRetVal = CopyFileA("c:\\mediadet.xbx", "c:\\dashboard.xbx", FALSE);
            if (FALSE == bRetVal) {
                DebugBreak();
            }
            return SOFT_REBOOT;
        }

        //
        // if B
        //

        else if (InputCheckButton(XINPUT_GAMEPAD_B)) {
            bRetVal = CopyFileA("c:\\mediadet.xbx", "c:\\dashboard.xbx", FALSE);
            if (FALSE == bRetVal) {
                DebugBreak();
            }
            return HARD_REBOOT;
        }

        //
        // if B (black)
        //

        else if (InputCheckButton(XINPUT_GAMEPAD_BLACK)) {
            bRetVal = CopyFileA("c:\\mediadet.xbx", "c:\\dashboard.xbx", FALSE);
            if (FALSE == bRetVal) {
                DebugBreak();
            }
            return SMC_RESET;
        }

        //
        // if Y
        //

        else if (InputCheckButton(XINPUT_GAMEPAD_Y)) {
            // don't change anything
            return 0;
        }

        //
        // if WHITE
        //

        else if (InputCheckButton(XINPUT_GAMEPAD_WHITE)) {

            //
            // clear the stats
            //

            fbuffer.iteration = 0;
            fbuffer.SizesDetected = 0;
            for (x = 0; x < DATASLOTS; x++) {
                fbuffer.DetectData[x].SizeDetected.QuadPart = 0;
                fbuffer.DetectData[x].SizeDetectedCount = 0;
            }

            //
            // don't exit from here - user can press white then another selection
            //

        }

        //
        // if X
        //

        else if (InputCheckButton(XINPUT_GAMEPAD_X)) {
            bRetVal = CopyFileA("c:\\dashboard_backup.xbx", "c:\\dashboard.xbx", FALSE);

            if (FALSE == bRetVal) {
                DebugBreak();
            }

            //
            // boot default title
            //

            XLaunchNewImage(NULL, NULL);
        }

        //
        // wait for further input    
        //

        Sleep(300);
    }

    //
    // won't get hit
    //

    return(0);
}

void
__cdecl 
DumpStats(
    void
    )
/*++

Routine Description:

    DumpStats - dump current test stats

Arguments:

    void

Return Value:

    void
        
Note:


--*/ 
{
    DWORD x = 0;

    sprintf(buffer,"results so far:\niterations: %8.8d\n", fbuffer.iteration);
    printf(buffer);

    for (x = 0; x < fbuffer.SizesDetected; x++) {
        sprintf(buffer,"Detected Size:  %#16.16I64x\n", fbuffer.DetectData[x].SizeDetected.QuadPart);
        printf(buffer);
        sprintf(buffer,"Number of times detected: %8.8d\n", fbuffer.DetectData[x].SizeDetectedCount);
        printf(buffer);
    }

}


static const DWORD USBPortMasks[] = {
    XDEVICE_PORT0_MASK,
    XDEVICE_PORT1_MASK,
    XDEVICE_PORT2_MASK,
    XDEVICE_PORT3_MASK
};

void
__cdecl 
InputDukeInsertions(
    DWORD add, 
    DWORD remove
    )
/*++

Routine Description:

    InputDukeInsertions - look for Duke

Arguments:

    DWORD add
    DWORD remove

Return Value:

    void
        
Note:


--*/ 
{
    unsigned port;

    for (port=0; port<XGetPortCount(); port++) {
        if (remove & USBPortMasks[port]) {
            if (hDuke[port]) XInputClose(hDuke[port]);
            hDuke[port] = NULL;
        }
        if (add & USBPortMasks[port]) {
            hDuke[port] = XInputOpen(XDEVICE_TYPE_GAMEPAD, port, 0, NULL);
        }
    }
}

BOOL
__cdecl 
InputCheckButton(
    int button
    )
/*++

Routine Description:

    InputCheckButton - look for pressed button

Arguments:

    int button

Return Value:

    void
        
Note:


--*/ 
{
    XINPUT_STATE state;
    unsigned port;

    for (port=0; port<XGetPortCount(); port++) {
        if (hDuke[port]) {
            XInputGetState(hDuke[port], &state);
            if (state.Gamepad.bAnalogButtons[button] > BUTTON_PRESS_VALUE) {
                return TRUE;
            }
        }
    }

    return FALSE;
}
