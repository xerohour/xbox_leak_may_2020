#include "usbtst.h"

#define NO_XDCS_INSTANCE    ((DWORD)(-1))
#define IR_CODE_BUFFER_SIZE 8


#define FAILURE_RED          0xff9f0000
#define PENDING_YELLOW       0xffffff00
#define UNKNOWN_ORANGE       0xffff7f00
#define SUCCESS_GREEN        0xff009f00
#define WAITING_WHITE        0xffffffff

struct REMOTE_CODE_TABLE
{
    LPCWSTR Name;
    WORD Code;
};

REMOTE_CODE_TABLE RemoteCodeTable[] = {
    {L"POWER",    0xAD5},
    {L"REVERSE",  0xAE2},
    {L"PLAY",     0xAEA},
    {L"FORWARD",  0xAE3},
    {L"SKIP -",   0xADD},
    {L"STOP",     0xAE0},
    {L"PAUSE",    0xAE6},
    {L"SKIP +",   0xADF},
    {L"GUIDE",    0xAE5},
    {L"INFO",     0xAC3},
    {L"MENU",     0xAF7},
    {L"REPLAY",   0xAD8},
    {L"UP",       0xAA6},
    {L"LEFT",     0xAA9},
    {L"OK",       0xA0B},
    {L"RIGHT",    0xAA8},
    {L"DOWN",     0xAA7},
    {L"DIGIT 0",  0xACF},
    {L"DIGIT 1",  0xACE},
    {L"DIGIT 2",  0xACD},
    {L"DIGIT 3",  0xACC},
    {L"DIGIT 4",  0xACB},
    {L"DIGIT 5",  0xACA},
    {L"DIGIT 6",  0xAC9},
    {L"DIGIT 7",  0xAC8},
    {L"DIGIT 8",  0xAC7},
    {L"DIGIT 9",  0xAC6},
    {NULL, 0}
};

LPCWSTR GetCodeName(WORD wCode)
{
    if(0xF000 == (0xF000&wCode))
    {
        return L"ERROR";
    }
    REMOTE_CODE_TABLE *pRemoteEntry = RemoteCodeTable;
    while(pRemoteEntry->Code)
    {
        if(pRemoteEntry->Code == wCode)
        {
            return pRemoteEntry->Name;
        }
        pRemoteEntry++;
    }
    if(wCode)
    {
        return L"UNKNOWN";
    } else
    {
        return NULL;
    }
}

#define XDCS_STATUS_NO_DONGLE                   0
#define XDCS_STATUS_DOWNLOADING                 1
#define XDCS_STATUS_DOWNLOAD_VERIFYING          2
#define XDCS_STATUS_DOWNLOAD_UNKNOWN_VERSION    3
#define XDCS_STATUS_DOWNLOAD_CODE_ERRORS        4
#define XDCS_STATUS_DOWNLOAD_SUCCEEDED          5
#define XDCS_STATUS_DOWNLOAD_FAILED             6
#define XDCS_STATUS_FILE_WRITE_FAILED           7

DWORD VerifyAndGetRegion(XDCS_DVD_CODE_INFORMATION *pXdcsInfo, PVOID pvBytes, DWORD dwSize, DWORD *pdwRegion, BOOL *pfDebug);

const CHAR  *szRomImageDirTemplate    = "e:\\tests\\DVDROMIMAGES\\%x.%0.2x";     //version directory
const CHAR  *szRomImageNameTemplate   = "%s\\dvdkey%d.bin";               //release images
const CHAR  *szRomImageNameTemplateD  = "%s\\dvdkey%dd.bin";              //debug images


//------------------------------------------------------------------------------
// TestMain
//------------------------------------------------------------------------------
void __cdecl main()
{
    XDCS_DVD_CODE_INFORMATION XdcsInfo;
    DWORD dwXdcsInstance=NO_XDCS_INSTANCE;
    DWORD dwDownloadStatus = XDCS_STATUS_NO_DONGLE;
    DWORD dwRegion = 0;
    BOOL  fDebugRom = FALSE;
    DWORD dwConnectedDevices = 0;
    DWORD dwPortInUse = 0;
    XDCS_ASYNC_DOWNLOAD_REQUEST downloadRequest;
    DWORD dwPercentComplete = 0;
    DWORD dwDownloadStartTick, dwCurrentTick;
    double fDownloadTime = 0;
    PVOID pvBuffer;
    DWORD dwInserstions, dwRemovals;
    CHAR  szFileName[]="t:\\dvdcode.lib";
    HANDLE hRemote = NULL;
    DWORD  dwRemotePort=0;
    XINPUT_STATE_INTERNAL IrCodeBuffer[IR_CODE_BUFFER_SIZE] = {0};
    DWORD dwIrCodeBufferPos=0;
    DWORD dwPacketNumber=0;

    test_MapEDrive();

    CDraw draw;
    CXBFont *pFont = draw.CreateFont("e:\\tests\\media\\font16.xpr");
   

    WCHAR FormatBuffer[256];

    LPWSTR StatusText[] = 
    {
        L"INSERT DONGLE",
        L"PLEASE WAIT: Downloading . . .",
        L"PLEASE WAIT: Verifying Code . . .",
        L"INCONCLUSIVE: Cannot verify this ROM Version.",
        L"FAILURE: Code Contains Errors",
        L"SUCCESS: Region %d%s",
        L"FAILURE: Could Not Download Code",
        L"INCONCLUSIVE: Internal test error, could not save image",

    };

    int StatusColor[] = 
    {
        WAITING_WHITE,
        PENDING_YELLOW,
        PENDING_YELLOW,
        UNKNOWN_ORANGE,
        FAILURE_RED,
        SUCCESS_GREEN,
        FAILURE_RED,
        UNKNOWN_ORANGE
    };

    int ProgressColor[] = 
    {
        INVALID_GRAY,
        PENDING_YELLOW,
        PENDING_YELLOW,
        UNKNOWN_ORANGE,
        FAILURE_RED,
        SUCCESS_GREEN,
        FAILURE_RED,
        UNKNOWN_ORANGE
    };
    
    int InfoColor[] = 
    {
        INVALID_GRAY,
        PENDING_YELLOW,
        PENDING_YELLOW,
        UNKNOWN_ORANGE,
        FAILURE_RED,
        SUCCESS_GREEN,
        FAILURE_RED,
        UNKNOWN_ORANGE
    };

    
    //Initialize core peripheral port support
    XInitDevices(0,NULL);

    //Loop Forever Printing out the Library information
    while(1)
    {
        /**
        *** Poll the XDCS Portion
        ***
        ***
        **/
        
        //
        //  Update download progress
        //
        if(XDCS_STATUS_DOWNLOADING == dwDownloadStatus)
        {
            //Update download time
            dwCurrentTick = GetTickCount();
            fDownloadTime = ((LONG)dwCurrentTick - (LONG)dwDownloadStartTick)/1000.0;
            //Update percent complete
            dwPercentComplete = (downloadRequest.ulBytesRead * 100) / downloadRequest.ulLength; 
            // Check status
            if(ERROR_SUCCESS == downloadRequest.ulStatus)
            {
                HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
                if(INVALID_HANDLE_VALUE != hFile)
                {
                    DWORD dwBytesWritten;
                    if(WriteFile(hFile, downloadRequest.pvBuffer, downloadRequest.ulBytesRead, &dwBytesWritten, NULL))
                    {
                        dwDownloadStatus = VerifyAndGetRegion(
                                                &XdcsInfo,
                                                downloadRequest.pvBuffer,
                                                downloadRequest.ulBytesRead,
                                                &dwRegion,
                                                &fDebugRom
                                                );
                    } else
                    {
                        dwDownloadStatus = XDCS_STATUS_FILE_WRITE_FAILED;
                    }
                    CloseHandle(hFile);
                } else
                {
                    dwDownloadStatus = XDCS_STATUS_FILE_WRITE_FAILED;
                }
                free(downloadRequest.pvBuffer);
            } else if(ERROR_IO_PENDING != downloadRequest.ulStatus)
            {
                dwDownloadStatus = XDCS_STATUS_DOWNLOAD_FAILED;
                free(downloadRequest.pvBuffer);
            }
        }

        //
        //  Look for the insertion or removal of devices
        //
        if(XGetDeviceChanges(XDEVICE_TYPE_DVD_CODE_SERVER, &dwInserstions, &dwRemovals))
        {
            if((1<<dwPortInUse)&dwRemovals)
            {   
                dwDownloadStatus = XDCS_STATUS_NO_DONGLE;
            }
            dwConnectedDevices &= dwRemovals;       //Handle removals
            dwConnectedDevices |= dwInserstions;    //Then additions

            //If there currently is no dongle, start using the first device.
            if(XDCS_STATUS_NO_DONGLE == dwDownloadStatus)
            {
                if(dwInserstions)
                {
                    DWORD dwError;
                    dwPortInUse = 0;
                    while(!(dwConnectedDevices&(1<<dwPortInUse))) dwPortInUse++;
                    dwError = XDCSGetInformation(dwPortInUse, &dwXdcsInstance, &XdcsInfo);
                    if(dwError)
                    {
                        dwDownloadStatus = XDCS_STATUS_DOWNLOAD_FAILED;
                    } else
                    {
                        dwDownloadStatus = XDCS_STATUS_DOWNLOADING;
                        downloadRequest.dwDeviceInstance = dwXdcsInstance;
                        downloadRequest.pvBuffer = malloc(XdcsInfo.dwCodeLength);
                        downloadRequest.ulOffset = 0;
                        downloadRequest.ulLength = XdcsInfo.dwCodeLength;
                        downloadRequest.ulBytesRead = 0;
                        downloadRequest.hCompleteEvent = NULL;
                        dwDownloadStartTick = GetTickCount();
                        if(!downloadRequest.pvBuffer)
                        {
                            dwDownloadStatus = XDCS_STATUS_DOWNLOAD_FAILED;
                        } else if(ERROR_IO_PENDING!=XDCSDownloadCodeAsync(&downloadRequest))
                        {
                            dwDownloadStatus = XDCS_STATUS_DOWNLOAD_FAILED;
                        }
                    }
                }
            }
        }        
        
        draw.FillRect(0,0,640,480, PITCH_BLACK);
        //Draw the status.
        if(XDCS_STATUS_DOWNLOAD_SUCCEEDED == dwDownloadStatus)
        {
            swprintf(FormatBuffer, StatusText[dwDownloadStatus], dwRegion, fDebugRom ? L", DEBUG" : L"");
            pFont->DrawText(70, 70, StatusColor[dwDownloadStatus],FormatBuffer);
        } else
        {
            pFont->DrawText(70, 70, StatusColor[dwDownloadStatus],StatusText[dwDownloadStatus]);
        }
        //Draw the progress
        swprintf(FormatBuffer, L"%d percent complete.", dwPercentComplete);
        pFont->DrawText(70, 95, ProgressColor[dwDownloadStatus], FormatBuffer);
        swprintf(FormatBuffer, L"%0.2lf seconds", fDownloadTime);
        pFont->DrawText(70, 120, ProgressColor[dwDownloadStatus], FormatBuffer);
        //Draw Info
        swprintf(FormatBuffer, L"Version %x.%0.2x", (ULONG)((XdcsInfo.bcdVersion&0xff00)>>8), (ULONG)(XdcsInfo.bcdVersion&0x00ff));
        pFont->DrawText(70, 145, InfoColor[dwDownloadStatus], FormatBuffer);
        swprintf(FormatBuffer, L"Code Length =  %d bytes", XdcsInfo.dwCodeLength);
        pFont->DrawText(70, 170, InfoColor[dwDownloadStatus], FormatBuffer);
        swprintf(FormatBuffer, L"FileName =  \'%S\'", szFileName);
        pFont->DrawText(70, 195, InfoColor[dwDownloadStatus], FormatBuffer);


        /**
        *** Poll the XID Remote part.
        *** Just because we don't have lot's of creative ways to
        *** abuse the XInput API, we will be clever here, and use the API in a valid,
        *** but less than optimal ways.  It had better work too!
        **/
        if(!hRemote) {
            DWORD dwDevices= (XGetDevices(XDEVICE_TYPE_IR_REMOTE)&0x0F); 
            if(dwDevices)
            {
                //Find the first bit set and open it.
                dwRemotePort = 0;
                while(!(dwDevices&(1<<dwRemotePort))) dwRemotePort++;
                hRemote = XInputOpen(XDEVICE_TYPE_IR_REMOTE, dwRemotePort, XDEVICE_NO_SLOT, NULL);
                dwPacketNumber = 0;
            }
        }
        if(hRemote)
        {
            if(ERROR_SUCCESS == XInputGetState(hRemote, (PXINPUT_STATE)(IrCodeBuffer+dwIrCodeBufferPos)))
            {
                //Don't move for duplicates
                if(dwPacketNumber != IrCodeBuffer[dwIrCodeBufferPos].dwPacketNumber)
                {
                    dwPacketNumber = IrCodeBuffer[dwIrCodeBufferPos].dwPacketNumber;
                    dwIrCodeBufferPos = (dwIrCodeBufferPos+1)%IR_CODE_BUFFER_SIZE;
                }
                
                swprintf(FormatBuffer, L"Port = %d", dwRemotePort);
                pFont->DrawText(70, 230, BRIGHT_BLUE, FormatBuffer);
                pFont->DrawText(70, 250, BRIGHT_BLUE, L"Command");
                pFont->DrawText(280, 250, BRIGHT_BLUE, L"Relative Time");
                if(dwPacketNumber)
                {
                    DWORD dwCodeBufferDisplayPos = (dwIrCodeBufferPos+1)%IR_CODE_BUFFER_SIZE;
                    DWORD dwDisplayYPos = 250 + (IR_CODE_BUFFER_SIZE*18);
                    LPCWSTR pwszCodeName;
                    for(int i=1; i < IR_CODE_BUFFER_SIZE; i++)
                    {
                        pwszCodeName = GetCodeName(IrCodeBuffer[dwCodeBufferDisplayPos].IrRemote.wKeyCode);
                        if(pwszCodeName)
                        {
                            swprintf(FormatBuffer, L"%s(0x%0.4x)", pwszCodeName, (DWORD)IrCodeBuffer[dwCodeBufferDisplayPos].IrRemote.wKeyCode);
                            pFont->DrawText(70, (float)dwDisplayYPos, BRIGHT_BLUE, FormatBuffer);
                            swprintf(FormatBuffer, L"%d ms", (DWORD)IrCodeBuffer[dwCodeBufferDisplayPos].IrRemote.wTimeDelta);
                            pFont->DrawText(280, (float)dwDisplayYPos, BRIGHT_BLUE, FormatBuffer);
                        }
                        dwDisplayYPos -= 18;
                        dwCodeBufferDisplayPos = (dwCodeBufferDisplayPos+1)%IR_CODE_BUFFER_SIZE;
                    }
                }
            } else
            {   
                XInputClose(hRemote);
                hRemote = NULL;
                memset(IrCodeBuffer, 0, sizeof(IrCodeBuffer));
                pFont->DrawText(70, 230, BRIGHT_BLUE, L"No IR Remote Receiver");
            }
        } else
        {
            pFont->DrawText(70, 230, BRIGHT_BLUE, L"No IR Remote Receiver");
        }
        //Preset
        draw.Present();
    }
}

BYTE rgbRomImageBuffer[500000]; //Buffer for the ROM image
DWORD VerifyAndGetRegion(XDCS_DVD_CODE_INFORMATION *pXdcsInfo, PVOID pvBytes, DWORD dwSize, DWORD *pdwRegion, BOOL *pfDebug)
/*++
  Routine Description:
--*/
{
    HANDLE hRomImage;
    DWORD  dwRomImageSize, dwBogus;
    DWORD  dwRegion;
    BOOL   fDebug;
    char   szRomImageDirectory[1024];
    char   szRomImagePath[1024];
    const char *pszNameTemplate = szRomImageNameTemplate;

    //
    //  Verify that the correct version of the ROM is available for the test
    //
    wsprintfA(
        szRomImageDirectory,
        szRomImageDirTemplate,
        (ULONG)((pXdcsInfo->bcdVersion&0xff00)>>8),
        (ULONG)(pXdcsInfo->bcdVersion&0x00ff)
        );
    if((DWORD)-1 == GetFileAttributes(szRomImageDirectory))
    {
        return XDCS_STATUS_DOWNLOAD_UNKNOWN_VERSION;
    }
    
    // Loop over all the possible images

    for(fDebug = 0; fDebug <= 1; fDebug++)
    {
        for(dwRegion = 1; dwRegion < 6; dwRegion++)
        {
            wsprintfA(szRomImagePath, pszNameTemplate, szRomImageDirectory, dwRegion);
            hRomImage= CreateFile(szRomImagePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
            if(INVALID_HANDLE_VALUE!=hRomImage)
            {
               dwRomImageSize = GetFileSize(hRomImage, &dwBogus);
               if(dwRomImageSize==dwSize)
               {
                    if(ReadFile(hRomImage, rgbRomImageBuffer, dwRomImageSize, &dwBogus, NULL))
                    {
                        if(0==memcmp(rgbRomImageBuffer, pvBytes, dwRomImageSize))
                        {
                            CloseHandle(hRomImage);
                            *pdwRegion = dwRegion;
                            *pfDebug = fDebug;
                            return XDCS_STATUS_DOWNLOAD_SUCCEEDED;
                        }
                    }
               }
               CloseHandle(hRomImage);
            }
        }
        pszNameTemplate = szRomImageNameTemplateD;
    }
    return XDCS_STATUS_DOWNLOAD_CODE_ERRORS;
}
