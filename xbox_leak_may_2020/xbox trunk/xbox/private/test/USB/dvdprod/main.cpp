/* ++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    main.cpp

Abstract:

    initializes all global variables, contains main loop, init. The module is using remote controller to control duke's motor.
	there is a file called buttoncontrol.ini to specify all codes. it can be changed to satisfy different requirement. This 
	application supports two dukes right now, it can be modified to support more than two dukes easily.

Author:

     Bing Li (a-bingli) 2001 May
     Mitchell Dernis (mitchd) 2001 July - Rewrite

-- */

#include <xtl.h>
#include <xboxp.h>
#include <time.h>
#include <stdio.h>
#include <draw.h>


//
//  The kernel exports this, it is just not in a public header
//
extern "C" 
{
    ULONG DebugPrint(PCHAR Format,...);
}

//---------------------------------------------------
// Filenames
//---------------------------------------------------
const char INI_FILENAME[]                = "t:\\buttoncontrol.ini";
const char DVD_REFERENCE_CODE_FILENAME[] = "d:\\dvdsource.lib";
const char DVD_DOWNLOAD_CODE_FILENAME[]  = "t:\\dvdcode.lib";

//--------------------------------------------------------------
//  Colors
//--------------------------------------------------------------
#define COLOR_BLACK   0xFF000000
#define COLOR_RED     0xFFFF0000
#define COLOR_GREEN   0xFF00FF00
#define COLOR_BLUE    0xFF0000FF
#define COLOR_ORANGE  0xFFFFFF00
#define COLOR_MAGENTA 0xFFFF00FF
#define COLOR_CYAN    0xFF00FFFF
#define COLOR_WHITE   0xFFFFFFFF

//--------------------------------------------------------------
//  Messages
//--------------------------------------------------------------
const char WAIT_FOR_DEVICE_INSERT[] = "Insert DVD IR Remote Receiver";

//--------------------------------------------------------------
// Remote Code Table - this table drives the remote code testing
//--------------------------------------------------------------
#define BUTTON_PRESS_TIMEOUT 10000 //Timeout for pressing a button (in ms)
#define MAX_BUTTON_TEST_ENTRIES 40
struct BUTTON_TEST_TABLE
{
    LPSTR  Name;       // Name of the button on the remote controller
    WORD   ButtonCode; // Remote Code of the button
	WORD   MotorCode;  // Motor(s) that operate the remote button
};
BUTTON_TEST_TABLE g_ButtonTestTable[MAX_BUTTON_TEST_ENTRIES+1] = {0};

//----------------------------------------------------------------
// Image Buffers
//----------------------------------------------------------------
#define MAX_IMAGE_SIZE 2000000

DWORD g_dwReferenceImageLength;                  // Size of Reference Image
BYTE  g_rgbReferenceImageBuffer[MAX_IMAGE_SIZE]; // Buffer for the Reference ROM image
BYTE  g_rgbDownloadImageBuffer[MAX_IMAGE_SIZE];  // Buffer for the Downloaded ROM image

//----------------------------------------------------------------
// Forward Declarations - Implemented in order of declaration
//----------------------------------------------------------------
void  ReadButtonTestTable(LPCSTR pszIniFilename);
LPSTR SkipWhitespace(LPSTR pszString);
void  LoadDvdReferenceImage(LPCSTR pszFilename);
DWORD WaitForDvdReceiverInsertion();
BOOL  DownLoadAndVerifyCode(DWORD dwPort);
void  WriteImageToFile(LPCSTR pszFilename, PVOID pvBuffer, DWORD dwBytes);
BOOL  RunButtonTest(DWORD dwPort);
void  OperateMotors(WORD wMotorCode, BOOL fOn);
void  DisplayMessage(DWORD dwColor, LPCSTR pszMessage,...);
void  DisplayFailure(LPCSTR pszMessage,...);
void  DisplaySuccess();
void  CriticalError(LPSTR pszErrorString,...);


//------------------------------------------------------------------------------
// Main Loop
//------------------------------------------------------------------------------
void __cdecl main()
/*++
 Routine Description:
    This operates the main loop of the test.  The following steps are
    performed.

    1) Wait for DVD Remote to be inserted.
    2) Download Code and Compare to Existing Image.
    3) Walk through button test.  (operating motors)
  
    Each of the routines are responsible for display
    status as they go.

--*/
{
    //
    //  Initialize core peripheral port support
    //

    XInitDevices(0,NULL);

    //
    //  Initialize the Drawing Library
    //

    drInit();

    //
    //  Load the remote code table.
    //

    ReadButtonTestTable(INI_FILENAME);

    //
    //  Load the test DVD code image.
    //
    LoadDvdReferenceImage(DVD_REFERENCE_CODE_FILENAME);

    //
    //  Show the insert device screen.
    //

    DisplayMessage(COLOR_CYAN, WAIT_FOR_DEVICE_INSERT);

    //
    //  Loop Forever.
    //

    while(TRUE)
    {
        DWORD dwPort;

        //
        //  Wait for a device insertion
        //  

        dwPort = WaitForDvdReceiverInsertion();

        //
        //  Download the code and verify it.
        //

        if(DownLoadAndVerifyCode(dwPort))
        {
            //
            //  If verification Passed, run the button test.
            //

            if(RunButtonTest(dwPort))
            {
                //
                //  If RunButtonTest also passed
                //  display success.
                //

                DisplaySuccess();
            }
        }
    }
}

void ReadButtonTestTable(LPCSTR pszIniFilename)
/*++
 Routine Description:
  Reads the .ini file to fill the remote code table.

 File Format:
  Each line consists of a button name, a remote code, and a motor code.
  All Items must be on the same line, and are comma separated
  
  button name: a quoted string.
  remote code: numerical value of string. 0xHHHH, or may be decimal DDDDD
  motor  code: bit field for motors to operate, either hexadecimal or
               decimal.
--*/
{
    char lineBuffer[256];
    char *pStartPhrase, *pEndPhrase;
    int  iLineNumber, iNameLen;
    int  iButtonEntry = 0;

    FILE *hIniFile = fopen(pszIniFilename, "rt");
    if(!hIniFile)
    {
      CriticalError("Could not open \'%s\'", pszIniFilename);
    }
    
    DebugPrint("Processing \'%s\':\n", pszIniFilename);

    //
    //  Process One line at a time
    //

    iLineNumber = 0;
    while(fgets(lineBuffer, sizeof(lineBuffer), hIniFile))
    {
        LPSTR pszButtonName;
        DWORD dwButtonCode;
        DWORD dwMotorCode;

        //
        // Increment Line Number
        //
        iLineNumber++;

        //
        // Skip comment lines
        //

        if(0==strncmp("//", lineBuffer, 2)) continue;

        //
        // Skip blank lines
        //
        pStartPhrase = SkipWhitespace(lineBuffer);
        if(!pStartPhrase)
        {
            continue;
        }
        
        //
        // Isolate Button Name in line
        //
        if(*pStartPhrase++ != '\"')
        {
            DebugPrint("ERROR (line %d): Name of button must be quoted. Skipping line.\n", iLineNumber);
            continue;
        }
        pEndPhrase = strchr(pStartPhrase, '\"');
        if(!pEndPhrase)
        {
            DebugPrint("ERROR (line %d): Cannot find closing quote for button name. Skipping line.\n", iLineNumber);
            continue;
        }
        pszButtonName = pStartPhrase;
        *pEndPhrase++ = '\0';

        //
        //  Now, get the button code.
        //
        pStartPhrase = SkipWhitespace(pEndPhrase);
        if(!pStartPhrase || *pStartPhrase++!=',')
        {
            DebugPrint("ERROR (line %d): Expecting comma after name of button. Skipping line.\n", iLineNumber);
            continue;   
        }
        pStartPhrase = SkipWhitespace(pStartPhrase);
        dwButtonCode = strtoul(pStartPhrase, &pEndPhrase, 0);
        if(pStartPhrase==pEndPhrase)
        {
            DebugPrint("ERROR (line %d): Expecting button code after name of button. Skipping line.\n", iLineNumber);
            continue;   
        }
        if(dwButtonCode > 0xFFFF)
        {
            DebugPrint("ERROR (line %d): Button code must be less than or equal 0xFFFF. Skipping line.\n", iLineNumber);
            continue;   
        }

        //
        //  Finally, get the motor code.
        //

        pStartPhrase = SkipWhitespace(pEndPhrase);
        if(!pStartPhrase || *pStartPhrase++!=',')
        {
            DebugPrint("ERROR (line %d): Expecting comma after button code. Skipping line.\n", iLineNumber);
            continue;   
        }
        pStartPhrase = SkipWhitespace(pStartPhrase);
        dwMotorCode = strtoul(pStartPhrase, &pEndPhrase, 0);
        if(pStartPhrase==pEndPhrase)
        {
            DebugPrint("ERROR (line %d): Expecting motor code after button code. Skipping line.\n", iLineNumber);
            continue;   
        }
        if(dwMotorCode > 0xFFFF)
        {
            DebugPrint("ERROR (line %d): Motor code must be less than or equal 0xFFFF. Skipping line.\n", iLineNumber);
            continue;
        }
        
        //
        //  Scan to end of line, and make sure that it is only white
        //

        pStartPhrase = SkipWhitespace(pEndPhrase);
        if(pStartPhrase)
        {
            DebugPrint("WARNING (line %d): Unexpected characters \'%s\' at end of line. Process line anyway\n", iLineNumber, pStartPhrase);
            continue;   
        }

        //
        //  Are there entries left in the table?
        //

        if(iButtonEntry >= MAX_BUTTON_TEST_ENTRIES)
        {
            DebugPrint("WARNING (line %d): Too many commands in ini file. Increase MAX_BUTTON_TEST_ENTRIES and recompile.\n", iLineNumber, pStartPhrase);
            continue;   
        }

        //
        //  Copy the entry
        //

        iNameLen = strlen(pszButtonName)+1;
        g_ButtonTestTable[iButtonEntry].Name = new char[iNameLen];
        if(NULL==g_ButtonTestTable[iButtonEntry].Name)
        {
            CriticalError("Couldn't Allocate Memory For Button Test Entry");
        }
        strcpy(g_ButtonTestTable[iButtonEntry].Name, pszButtonName);
        g_ButtonTestTable[iButtonEntry].ButtonCode =  (WORD)dwButtonCode;
        g_ButtonTestTable[iButtonEntry].MotorCode = (WORD)dwMotorCode;

        //
        //  Spew, what we did.
        //
        DebugPrint("%d: \'%s\',wButtonCode = 0x%0.4x,wMotorCode = 0x%0.4x\n",
                    iButtonEntry,
                    g_ButtonTestTable[iButtonEntry].Name,
                    g_ButtonTestTable[iButtonEntry].ButtonCode,
                    g_ButtonTestTable[iButtonEntry].MotorCode
                    );

        //
        //  Ready for next entry
        //
        iButtonEntry++;
    }
    fclose(hIniFile);
}

LPSTR SkipWhitespace(LPSTR pszString)
/*++
  Routine Description:
   Skips over whitespace in a string.
--*/
{
    while(pszString && *pszString)
    {
        if(
            (*pszString == '\t') ||
            (*pszString == ' ')  ||
            (*pszString == '\n') ||
            (*pszString == '\r')
        )
        {
            pszString++;
        } else
        {
            return pszString;
        }
    }
    return NULL;
}

void LoadDvdReferenceImage(LPCSTR pszFilename)
/*++
  Routine Description:
    Read in the DVD Reference Image.
--*/
{
    HANDLE hFile;
    DWORD  dwBogus;
    char   szFormatBuffer[1024];
    
    //
    //  Open the file
    //

    hFile = CreateFile(pszFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if(FAILED(hFile))
    {
        CriticalError("Couldn't open DVD code reference file, \'%s\'", pszFilename);
    }

    //
    //  Get the image size
    //

    g_dwReferenceImageLength = GetFileSize(hFile, &dwBogus);
    if(MAX_IMAGE_SIZE < g_dwReferenceImageLength)
    {
        CriticalError("Image buffer is not big enough for reference image.");
    }

    //
    //  Read the reference image
    //

    if(!ReadFile(hFile, g_rgbReferenceImageBuffer, g_dwReferenceImageLength, &dwBogus, NULL))
    {
        CriticalError("Could not read DVD code reference file, \'%s\'", pszFilename);
    }
    
    if(dwBogus != g_dwReferenceImageLength)
    {
        CriticalError("Could not read entire DVD code reference file, \'%s\'", pszFilename);
    }

    //
    // Close the file
    //

    CloseHandle(hFile);

    return;
}

DWORD WaitForDvdReceiverInsertion()
/*++
  Routine Description:
    Waits for a DVD dongle.  Notice that if multiple devices are inserted between calls
    to XGetDeviceChanges we only test one of them.  This spins tight, but since we
    don't have other threads, we have nothing else to do.  There is no incentive sleep.
--*/
{
  static DWORD dwCurrentDongle = 0;
  DWORD dwInsertions, dwRemovals;
  DWORD dwRetVal;
    
  while(TRUE)
  {
    if(XGetDeviceChanges(XDEVICE_TYPE_DVD_CODE_SERVER, &dwInsertions, &dwRemovals))
    {
      //
      //  Handle Removals First
      //
      if(dwCurrentDongle&dwRemovals)
      {
          //DisplayMessage(COLOR_CYAN, WAIT_FOR_DEVICE_INSERT);
          dwCurrentDongle = 0;
      }
    
      //
      //  Look for newly inserted device.
      //
      if(dwInsertions)
      {
        for(dwRetVal = 0, dwCurrentDongle = 1; dwRetVal < 4; dwRetVal++, dwCurrentDongle <<= 1)
        {
           if(dwInsertions&dwCurrentDongle)
           {
             return dwRetVal;
           }
        }
      }
    }
  }            
}

BOOL DownLoadAndVerifyCode(DWORD dwPort)
/*++
  Routine Description:
    Reads the code from the DVD IR receiver
    and checks it against the reference image.
  Return Value:
    TRUE  - On Success
    FALSE - On Failure
--*/
{
    DWORD dwError;
    DWORD dwDeviceInstance;
    XDCS_DVD_CODE_INFORMATION xdcsCodeInformation;
    DWORD dwBytesRead;
    
    //
    //  Get the image size and version
    //
    dwError = XDCSGetInformation(dwPort, &dwDeviceInstance, &xdcsCodeInformation);
    
    //
    //  If error, then display and return FALSE
    //

    if(ERROR_SUCCESS != dwError)
    {
        DisplayFailure("Getting Device Information");
        return FALSE;
    }

    //
    //  Check image size against our buffer
    //
    
    if(xdcsCodeInformation.dwCodeLength > MAX_IMAGE_SIZE)
    {
        DisplayFailure("ROM Image Too Large (%d bytes)", xdcsCodeInformation.dwCodeLength);
        return FALSE;
    }

    //
    //  Download code,
    //

    dwError = XDCSDownloadCode(dwDeviceInstance, g_rgbDownloadImageBuffer, 0, xdcsCodeInformation.dwCodeLength, &dwBytesRead);

    //
    //  Check for errors
    //

    if(ERROR_SUCCESS != dwError)
    {
        DisplayFailure("Downloading ROM");
        return FALSE;
    }

    //
    //  Were all the bytes read?
    //

    if(dwBytesRead != xdcsCodeInformation.dwCodeLength)
    {
        DisplayFailure("Couldn't Download all bytes");
        return FALSE;
    }

    //
    //  Write out the image to file (before comparing it to the
    //  reference image.)
    //
    WriteImageToFile(DVD_DOWNLOAD_CODE_FILENAME, g_rgbDownloadImageBuffer, dwBytesRead);

    //
    //  Now compare with reference image.
    //
    
    if(g_dwReferenceImageLength != dwBytesRead)
    {
        DisplayFailure("ROM Length (%d bytes) incorrect", dwBytesRead);
        return FALSE;
    }

    //
    //  Do memory comparison
    //
    if(memcmp(g_rgbDownloadImageBuffer, g_rgbReferenceImageBuffer, dwBytesRead))
    {
        DisplayFailure("ROM not identical to reference code.");
        return FALSE;
    }

    return TRUE;
}

void  WriteImageToFile(LPCSTR pszFilename, PVOID pvBuffer, DWORD dwBytes)
/*++
  Routine Description:
    Writes pvBuffer out to pszFilename, blasting over an existing file.
--*/
{
    DWORD dwBytesWritten;
    HANDLE hFile = CreateFile(
                        pszFilename,
                        GENERIC_WRITE,
                        0,
                        NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL
                        );
    if(INVALID_HANDLE_VALUE == hFile)
    {
        DebugPrint("WARN: Could not create file \'%s\' to dump image to.\n", pszFilename);
        return;
    }

    if(!WriteFile(hFile, pvBuffer, dwBytes, &dwBytesWritten, NULL))
    {
        DebugPrint("WARN: Write dumping image failed.\n");
    }

    if(dwBytesWritten != dwBytes)
    {
        DebugPrint("WARN: Write only wrote some of the bytes.\n");
    }

    CloseHandle(hFile);
}



BOOL RunButtonTest(DWORD dwPort)
/*++
  Routine Description:
    1) Open IR Remote Device
    
    2) Goes through buttons in the INI file and
       a) Turns on motor(s) to activate.
       b) Display message to press button
       c) Waits for button press
       d) Fails if timeout on button press.
       e) Turn off motor(s).

    3) Close IR Remote Device
--*/
{
    int                   iButtonIndex = 0;
    XINPUT_STATE_INTERNAL InputState;
    DWORD                 dwError;
    HANDLE                hRemote;
    int                   iEndTick;
    BOOL                  fTimeout;
    

    //
    //  Give it a second the input portion, it should not
    //  take longer than that.
    //
    //  This is not really a hack.  It is perfectly legitimate
    //  to give it 200 ms and no more to find the input portion
    //  after finding the code server.  I would NEVER suggest
    //  this for a game.  However, we are testing that the device
    //  behaves reasonably.
    //

    Sleep(200);

    //
    //  Open the remote device
    //
    hRemote = XInputOpen(XDEVICE_TYPE_IR_REMOTE, dwPort, XDEVICE_NO_SLOT, NULL);
    if(!hRemote)
    {
        DisplayFailure("Opening IR Remote Input.");
        return FALSE;
    }
    
    //
    //  Loop over all button tests
    //

    while(g_ButtonTestTable[iButtonIndex].Name)
    {
        //
        //  Turn on the motors
        //

        OperateMotors(g_ButtonTestTable[iButtonIndex].MotorCode, TRUE);

        //
        //  Tell operator to press button.
        //

        DisplayMessage(COLOR_GREEN, "Press \'%s\' Button", g_ButtonTestTable[iButtonIndex].Name);

        //
        //  Wait for button press or timeout
        //  (use signed compare for timeout to handle wrap)
        //
        iEndTick = ((int)GetTickCount()) + BUTTON_PRESS_TIMEOUT;
        fTimeout = TRUE; //assume timeout
        while( (iEndTick - ((int)GetTickCount())) > 0)
        {
            dwError = XInputGetState(hRemote, (PXINPUT_STATE)&InputState);
            if(ERROR_SUCCESS != dwError)
            {
                DisplayFailure("\'%s\' Button", g_ButtonTestTable[iButtonIndex].Name);
                XInputClose(hRemote);
				OperateMotors(g_ButtonTestTable[iButtonIndex].MotorCode, FALSE);
                return FALSE;
            }
            
            if(InputState.IrRemote.wKeyCode == g_ButtonTestTable[iButtonIndex].ButtonCode)
            {
                // Not a timeout
                fTimeout = FALSE;
                break;
            }
        }

        //
        //  Turn off the motors
        //
        
        OperateMotors(g_ButtonTestTable[iButtonIndex].MotorCode, FALSE);

        //
        //  If we timed out, then display error, return FALSE.
        //

        if(fTimeout)
        {
            XInputClose(hRemote);
            DisplayFailure("Timeout Waiting for \'%s\' button", g_ButtonTestTable[iButtonIndex].Name);
            return FALSE;
        }

        //
        //  Increment iButtonIndex for next loop
        //

        iButtonIndex++;
    }

    //
    //  Close the handle
    //

    XInputClose(hRemote);
    
    //
    //  We finished the button test 
    //

    return TRUE;
}

void OperateMotors(WORD wMotorCode, BOOL fOn)
/*++
  Routine Description:
    Turns on the motors specified in the wMotorCode.
    
    Only the lower byte of wMotorCode is used.
    Here is the interpretation:

        Bit Position:        76543210  
        =============================
        Left(L) or Right(R): LRLRLRLR  
        Controller Number:   33221100

    Any number of bits may be set.

--*/
{
    XINPUT_FEEDBACK feedback;
    DWORD           dwPort;
    DWORD           dwError;
    HANDLE          hGamepad;
    BOOL            fSuccess;

    //
    //  Fill out the feedback header
    //
    memset(&feedback,0, sizeof(feedback));
    feedback.Header.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if(!feedback.Header.hEvent)
    {
        DebugPrint("WARN: Could not create feedback wait event.");
        return;
    }
    
    //
    //  Loop over ports
    //
    for(dwPort = 0; dwPort < 4; dwPort++)
    {
        if(wMotorCode&3)
        {
            DebugPrint(
                "Turning on gamepad %d motors(%s%s%s)\n",
                dwPort,
                (wMotorCode&2) ? "LEFT" : "",
                (3 == (wMotorCode&3)) ? " and " : "",
                (wMotorCode&1) ? "RIGHT" : ""
                );
            fSuccess = FALSE;
            hGamepad = XInputOpen(XDEVICE_TYPE_GAMEPAD, dwPort, XDEVICE_NO_SLOT, NULL);
            if(hGamepad)
            {
                if(fOn)
                {
                    feedback.Rumble.wLeftMotorSpeed = (wMotorCode&2) ? 0xFFF0 : 0;
                    feedback.Rumble.wRightMotorSpeed = (wMotorCode&1) ? 0xFFF0 : 0;
                } else
                {
                    feedback.Rumble.wLeftMotorSpeed = 0;
                    feedback.Rumble.wRightMotorSpeed = 0;
                }

                ResetEvent(feedback.Header.hEvent);
                dwError = XInputSetState(hGamepad, &feedback);
                if(ERROR_IO_PENDING == dwError)
                {
                    // Wait for 1 second max
                    WaitForSingleObject(feedback.Header.hEvent, 1000);
                }
                XInputClose(hGamepad);
                if(ERROR_SUCCESS == feedback.Header.dwStatus)
                {
                    fSuccess = TRUE;
                }
            }
            if(!fSuccess)
            {
                DebugPrint("WARN: Command to motors failed.");
            }
        }
        //
        //  Get ready for next iteration.
        //
        wMotorCode >>= 2;
    }
}

void  DisplayMessage(DWORD dwColor, LPCSTR pszMessage,...)
/*++
  Routine Description:
    Displays a message while tests are running to indicate testing
    progress, and whether user action is required.
  Arguments:
   pszMessage - format string for message to display.
  
   See printf - for use of variable arguments.
--*/
{
    char    szBuffer[512];
    WCHAR   wszBuffer[512];
    
    //Format string
    va_list args;
    va_start(args, pszMessage);
    vsprintf(szBuffer, pszMessage, args);
    va_end(args);

    //Convert to WCHAR
    wsprintfW(wszBuffer, L"%hs", szBuffer);

    //Set Font
    drSetSize(40.0, 42.0, dwColor, COLOR_BLACK);

    //Clear Screen
    drCls();

    //Print Header
    drPrintf(40,50,L"Xbox DVD Dongle Test Version 1.00");

    //Print Message
    drPrintf(40,150,wszBuffer);
    
    // Show the display
    drShowScreen();

    //Send to Debug Monitor
    DebugPrint("%s\n", szBuffer);
}   

void  DisplayFailure(LPCSTR pszMessage,...)
/*++
  Routine Description:
    Similar to DisplayMessage, but is accompanied by a BIG RED X.
    This indicates that some test failed.
  Arguments:
   pszMessage - format string for message to display.
  
   See printf - for use of variable arguments.
--*/
{
    char    szBuffer[512];
    WCHAR   wszBuffer[512];
    
    //Format string
    va_list args;
    va_start(args, pszMessage);
    vsprintf(szBuffer, pszMessage, args);
    va_end(args);

    //Convert to WCHAR
    wsprintfW(wszBuffer, L"FAILED: %hs", szBuffer);
    
    //Set Font
    drSetSize(40.0, 42.0, COLOR_RED, COLOR_BLACK);

    //Clear Screen
    drCls();

    //Print Message
    drPrintf(40,50, wszBuffer);

    //Draw RED X
    drQuad(200, 425, COLOR_RED, 550,  75, COLOR_RED, 600, 125, COLOR_RED, 250, 475, COLOR_RED);
	drQuad(600, 425, COLOR_RED, 250,  75, COLOR_RED, 200, 125, COLOR_RED, 550, 475, COLOR_RED);

    // Show the display
    drShowScreen();

    //Send to Debug Monitor
    DebugPrint("FAILED: %s\n", szBuffer);
}


void DisplaySuccess()
/*++
  Routine Description:
    Draws a big green check
--*/
{
    drSetSize(40.0, 42.0, COLOR_GREEN, COLOR_BLACK);

    //Clear Screen
    drCls();

    //Print Message
    drPrintf(40, 50,L"Test Passed");

    //Draw GREEN Check
    drQuad(250, 350, COLOR_GREEN, 550,  50, COLOR_GREEN, 600, 100, COLOR_GREEN, 300, 400, COLOR_GREEN);
	drQuad(250, 450, COLOR_GREEN, 300, 400, COLOR_GREEN, 150, 250, COLOR_GREEN, 100, 300, COLOR_GREEN);

    // Show the display
    drShowScreen();

    //Send to Debug Monitor
    DebugPrint("Test Passed\n");
}

void  CriticalError(LPSTR pszErrorString,...)
/*++
  Routine Description:
    Called when the test system hits an unexpected error that prevents testing.
--*/
{
    char    szBuffer[512];
    WCHAR   wszBuffer[512];
    
    //Format string
    va_list args;
    va_start(args, pszErrorString);
    vsprintf(szBuffer, pszErrorString, args);
    va_end(args);

    //Convert to WCHAR
    wsprintfW(wszBuffer, L"CRITICAL ERROR: %hs", szBuffer);

    //Set Font
    drSetSize(40.0, 42.0, COLOR_RED, COLOR_BLACK);

    //Clear Screen
    drCls();

    //Print Header
    drPrintf(40,50,L"Xbox DVD Dongle Test Version 1.00");

    //Print Message
    drPrintf(40,150,wszBuffer);
    drPrintf(40,200,L"Reboot System and Correct Problem");
    
    // Show the display
    drShowScreen();

    //Send to Debug Monitor
    DebugPrint("Critical Error: %s\n", szBuffer);
    DebugPrint("Reboot System and Correct Problem\n");

    //Spin forever
    while(TRUE);
}