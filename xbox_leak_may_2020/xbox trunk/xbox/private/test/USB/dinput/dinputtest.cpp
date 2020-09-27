/*

Copyright (c) 2000  Microsoft Corporation

Module Name:

    usbtest.cpp

Abstract:

    The DLL exports two of the mandatory APIs: StartTest and EndTest.
    TESTINI.INI must be modified correctly so that the
    harness will load and call StartTest/EndTest.

    Please see "x-box test harness.doc" for more information.

Author:

    jpoley

Environment:

    XBox

Revision History:

*/


#include "stdafx.h"
#include <initguid.h>
#include <dinput.h>
#include <input.h>
#include <xlog.h>
#include "ditestAPI.h"
#include "commontest.h"

#include "dinputtest.h"

//
// Thread ID in multiple-thread configuration (not a Win32 thread ID)
//
// You can specify how many threads you want to run your test by
// setting the key NumberOfThreads=n under your DLL section. The
// default is only one thread.
//

ULONG ThreadID = 0;


//
// Heap handle from HeapCreate
//
HANDLE HeapHandle;


BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
    {
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(lpReserved);

    //
    // IMPORTANT:
    //
    // There are tons of threads in the harness process. If you don't have
    // anything to do for thread creation/destroy notification, you should
    // call DisableThreadLibraryCalls to reduce the size of the working set
    //
    // TODO: There is no DisableThreadLibraryCalls export from XAPI now,
    //         remove the comment below when it is ready.
    //

    if(fdwReason == DLL_PROCESS_ATTACH)
        {
        //DisableThreadLibraryCalls(hInstance);
        HeapHandle = HeapCreate(0, 0, 0);

        if(!HeapHandle)
            {
            OutputDebugString(L"USBTEST: Unable to create heap\n");
            }
        }
    else if(fdwReason == DLL_PROCESS_DETACH)
        {
        if(HeapHandle)
            {
            HeapDestroy(HeapHandle);
            }
        }

    return TRUE;
    }


/*

Routine Description:


Arguments:

    HANDLE hLog - to the logging object
    DWORD threadNumber - sequential number of the thread
    DWORD loops - # of times to run test
    WCHAR* testname - specific name of a test to run

Return Value:

    DWORD

*/
DWORD RunAll(HANDLE hLog, DWORD threadNumber, DWORD loops, WCHAR *testname)
    {
    Sleep(threadNumber*200); // stagger the thread's execution a bit

    struct APIFUNCT
        {
        APITEST_FUNC f;
        WCHAR *name;
        };
    #define BUILTAPISTRUCT(n) { n , L#n }

    // Define all the API tests
    APIFUNCT testlist[] =
        {
        BUILTAPISTRUCT(DeviceDump_Test),
        /*
        BUILTAPISTRUCT(Acquire_Test),
        */
        /*
        BUILTAPISTRUCT(CreateDevice_Test),
        BUILTAPISTRUCT(DirectInputCreate_Test),
        BUILTAPISTRUCT(EnumDevices_Test),
        BUILTAPISTRUCT(GetCapabilities_Test),
        BUILTAPISTRUCT(GetDeviceStatus_Test),
        */
        };

    bool runAll = wcscmp(testname, L"(all)") == 0 ? true : false;

    // Run the suite of tests
    for(DWORD i=0; i<loops; i++)
        {
        // run each test
        for(unsigned j=0; j<ARRAYSIZE(testlist); j++)
            {
            if(runAll || wcsstr(testlist[j].name, testname) != NULL)
                {
                xLog(hLog, XLL_INFO, "%ws() ******************************************", testlist[j].name);
                LogResourceStatus(hLog, true);
                DebugPrint("DITEST(%d): Entering %ws()\n", threadNumber, testlist[j].name);
                testlist[j].f(hLog, threadNumber);
                }
            }
        }

    return 0;
    }




/*

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

*/
VOID WINAPI StartTest(HANDLE hLog)
    {
    ULONG ID = 0;

    //
    // One way to run multiple threads without creating threads is to
    // set NumberOfThreads=n under the section [sample]. Everytime
    // the harness calls StartTest, ThreadID will get increment by one
    // after InterlockedIncrement call below. If no NumberOfThreads
    // specified, only RunTest1 will be run
    //

    ID = InterlockedIncrement((LPLONG) &ThreadID);

    // test settings
    const int buffLen = 1024;
    WCHAR testname[buffLen];
    DWORD loops;

    // Pull out internal INI file settings
    GetProfileString(L"dinputtest", L"testname", L"(all)", testname, buffLen);
    loops = GetProfileInt(L"dinputtest", L"loops", 1);

    xSetComponent(hLog, "DirectX", "DirectInput");
    DebugPrint("DITEST(%d): Entering StartTest()\n", ID);
    DebugPrint("DITEST(%d): Internal INI Settings:\n", ID);
    DebugPrint("  loops=%lu\n", loops);
    DebugPrint("  testname=%ws\n", testname);

    switch(ThreadID)
        {
        case 0:
        case 1:
        default:
            RunAll(hLog, ID, loops, testname);
            break;
        }

    DebugPrint("DITEST(%d): Leaving StartTest()\n", ID);
    }


/*

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

*/
VOID WINAPI EndTest(void)
    {
    ThreadID = 0;
    }



BOOL CALLBACK EnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
    {
    /*
    static BOOL fCreate = TRUE;
    CDIDevice *pDevice;

    if(pvRef == ENUM_DEVICE_REMOVE_REF)
        {
        pDevice = g_pDITest->FindDevice(lpddi->guidInstance);
        g_pDITest->RemoveDevice(pDevice, FALSE);
        delete pDevice;
        }

    if(pvRef == ENUM_DEVICE_INSERT_REF)
        {
        pDevice = new CDIDevice(lpddi->guidInstance);
        if(pDevice)
            {
            pDevice->CreateDID8(g_pDITest->GetDI8(), fCreate);
            fCreate = !fCreate;
            g_pDITest->AddDevice(pDevice);
            }
        }
*/
    return TRUE;
    }


/*
void DeviceChanges(PCINPUT_DEVICE_INFORMATION DeviceInformation, BOOL fAdd, BOOL, CXidDeviceCollection *pDeviceCollection)
    {
    if(fAdd)
        {
        pDeviceCollection->Add(DeviceInformation);
        }
    else
        {
        pDeviceCollection->Remove(DeviceInformation);
        }
    }


CXidDeviceCollection::CXidDeviceCollection() : m_pDevices(NULL), m_iDeviceCount(0)
    {
    }

CXidDeviceCollection::~CXidDeviceCollection()
    {
    //
    //  Remove all the devices
    //
    CXidDevice *pDevice;
    while(m_pDevices)
        {
        pDevice = m_pDevices;
        m_pDevices = pDevice->m_pNextDevice;
        //
        //  neglect the backward links, we are going to
        //  delete everything anyway.
        //
        pDevice->Close();
        m_iDeviceCount--;
        delete pDevice;
        }

    if(0!=m_iDeviceCount)
        {
        DebugPrint("USBTEST: Somewhere m_iDeviceCount got hosed, it is now %d, should be 0\n", m_iDeviceCount);
        }
    }

void CXidDeviceCollection::Add(PCINPUT_DEVICE_INFORMATION pDeviceInfo)
    {
    CXidDevice *pNewDevice = new CXidDevice(pDeviceInfo);

    //
    //  Open the device
    //
    pNewDevice->Open();

    //
    //  Add the device to the head of the list
    //
    pNewDevice->m_pPrevDevice = NULL;
    pNewDevice->m_pNextDevice = m_pDevices;
    if(m_pDevices) m_pDevices->m_pPrevDevice = pNewDevice;
    m_pDevices = pNewDevice;
    m_iDeviceCount++;
    }

void CXidDeviceCollection::Remove(PCINPUT_DEVICE_INFORMATION pDeviceInfo)
    {
    //
    //  Walk the list loop for the device
    //
    CXidDevice *pDevice = m_pDevices;
    while(pDevice)
        {
        if(pDevice->IsDevice(pDeviceInfo))
            {
            if(pDevice->m_pPrevDevice)
                {
                pDevice->m_pPrevDevice->m_pNextDevice = pDevice->m_pNextDevice;
                }
            else
                {
                m_pDevices = pDevice->m_pNextDevice;
                }
            if(pDevice->m_pNextDevice)
                {
                pDevice->m_pNextDevice->m_pPrevDevice = pDevice->m_pPrevDevice;
                }
            pDevice->Close();
            delete pDevice;
            m_iDeviceCount--;
            return;
            }
        pDevice = pDevice->m_pNextDevice;
        }
    DebugPrint("USBTEST: Attempt to remove a device not found:\n");
    DumpDeviceInfo(pDeviceInfo);
    }

void CXidDeviceCollection::PollDevices()
    {
    CXidDevice *pDevice = m_pDevices;
    while(pDevice)
        {
        pDevice->PollDevice();
        pDevice = pDevice->m_pNextDevice;
        }
    }

CXidDevice::CXidDevice(PCINPUT_DEVICE_INFORMATION pDeviceInfo) :
    m_dwDeviceHandle(0), m_pNextDevice(NULL), m_pPrevDevice(NULL),
    m_dwPacketNumber(0), m_dwIdlePollCount(0)
    {
    m_DeviceInfo = *pDeviceInfo;
    m_InputBuffer.bReportId = 0;
    m_InputBuffer.bSize = sizeof(INPUT_FORMAT_GAME);
    m_InputBufferLast.bReportId = 0;
    m_InputBufferLast.bSize = sizeof(INPUT_FORMAT_GAME);
    DebugPrint("USBTEST: DeviceAdded:\n");
    DumpDeviceInfo(&m_DeviceInfo);
    }

CXidDevice::~CXidDevice()
    {
    if(m_dwDeviceHandle)
        {
        Close();
        }
    DebugPrint("USBTEST: DeviceRemoved:\n");
    DumpDeviceInfo(&m_DeviceInfo);
    }

void CXidDevice::Open()
    {
    DWORD errorCode = InputOpenDevice(m_DeviceInfo.bDeviceID, false, &m_dwDeviceHandle);
    if(ERROR_SUCCESS != errorCode)
        {
        DebugPrint("USBTEST: InputOpenDevice failed with errorCode 0x%0.8x\n", errorCode);
        if(m_dwDeviceHandle != 0)
            {
            DebugPrint("USBTEST: InputOpenDevice failed and did not zero out the device handle!!!");
            }
        }
    }

void CXidDevice::Close()
    {
    if(m_dwDeviceHandle)
        {
        InputCloseDevice(m_dwDeviceHandle);
        m_dwDeviceHandle = 0;
        }
    }

void CXidDevice::PollDevice()
    {
    DWORD               dwError;
    DWORD               dwPreviousPacketNumber = m_dwPacketNumber;
    dwError = InputGetDeviceState(m_dwDeviceHandle, (PINPUT_FORMAT)&m_InputBuffer, &m_dwPacketNumber);
    if(ERROR_SUCCESS == dwError)
        {
        //
        //  check to see if the device state changed
        //
        if(dwPreviousPacketNumber != m_dwPacketNumber)
            {
            if(ThresholdCheck())
                {
                m_dwIdlePollCount = 0;
                DebugPrint("USBTEST: Device %d changed state:\n", (DWORD)m_DeviceInfo.bDeviceID);
                DumpDeviceState(&m_InputBuffer);
                }
            else
                {
                m_dwIdlePollCount = 0;
                DebugPrint(".");
                }
            }
        else
            {
            //
            //  Display an idle message about every 5 seconds, assuming
            //  this function gets called 60 times a seconds.
            //
            if(0==++m_dwIdlePollCount%600)
                {
                DebugPrint("USBTEST: Device %d has been idle about %d seconds.\n", (DWORD)m_DeviceInfo.bDeviceID, m_dwIdlePollCount/60);
                }
            }
        }
    else
        {
        DebugPrint("USBTEST: Poll of device %d failed.\n", (DWORD)m_DeviceInfo.bDeviceID);
        }
    }

BOOL CXidDevice::ThresholdCheck()
/*++
    ThresholdCheck is necessary because the POC has a lot of jitter.
    Since we don't have graphics yet, the test program outputs to the
    debug monitor, and we need to keep the spew down.
--/
{
    int i;
    BOOL Threshold = FALSE;
    for(i=0; i<8; i++)
    {
        //
        //  check analog buttons (look for a change of 32 or greater)
        //
        if((m_InputBufferLast.rgbAnalogButtons[i]^m_InputBuffer.rgbAnalogButtons[i])&0xE0)
        {
            m_InputBufferLast.rgbAnalogButtons[i] = m_InputBuffer.rgbAnalogButtons[i];
            Threshold = TRUE;
        }
        //
        //  check digitial buttons
        //
        if(m_InputBufferLast.wButtons != m_InputBuffer.wButtons)
        {
            m_InputBufferLast.wButtons = m_InputBuffer.wButtons;
            Threshold = TRUE;
        }
        //
        //  Check DPAD
        //
        if(m_InputBufferLast.wDPad != m_InputBuffer.wDPad)
        {
            m_InputBufferLast.wDPad = m_InputBuffer.wDPad;
            Threshold = TRUE;
        }
        //
        //  check axes (Look for a change of 32 or creater
        //
        if((m_InputBufferLast.wThumb1X^m_InputBuffer.wThumb1X)&0xFFE0)
        {
            m_InputBufferLast.wThumb1X=m_InputBuffer.wThumb1X;
            Threshold = TRUE;
        }
        if((m_InputBufferLast.wThumb1Y^m_InputBuffer.wThumb1Y)&0xFFE0)
        {
            m_InputBufferLast.wThumb1Y=m_InputBuffer.wThumb1Y;
            Threshold = TRUE;
        }
        if((m_InputBufferLast.wThumb2X^m_InputBuffer.wThumb2X)&0xFFE0)
        {
            m_InputBufferLast.wThumb2X=m_InputBuffer.wThumb2X;
            Threshold = TRUE;
        }
        if((m_InputBufferLast.wThumb2Y^m_InputBuffer.wThumb2Y)&0xFFE0)
        {
            m_InputBufferLast.wThumb2Y=m_InputBuffer.wThumb2Y;
            Threshold = TRUE;
        }
        /*
        if((m_InputBufferLast.wTiltX^m_InputBuffer.wTiltX)&0xFFE0)
        {
            m_InputBufferLast.wTiltX=m_InputBuffer.wTiltX;
            Threshold = TRUE;
        }
        if((m_InputBufferLast.wTiltY^m_InputBuffer.wTiltY)&0xFFE0)
        {
            m_InputBufferLast.wTiltY=m_InputBuffer.wTiltY;
            Threshold = TRUE;
        }*


    }
    return Threshold;
}

BOOL CXidDevice::IsDevice(PCINPUT_DEVICE_INFORMATION pDeviceInfo)
    {
    if(m_DeviceInfo.bDeviceID == pDeviceInfo->bDeviceID)
        {
        return TRUE;
        }
    return FALSE;
    }


LPCSTR GetDeviceTypeString(BYTE bType)
    {
    switch(bType)
        {
        case INPUT_DEVTYPE_GAMECONTROLLER:
            return "INPUT_DEVTYPE_GAMECONTROLLER";
        case INPUT_DEVTYPE_KEYBOARD:
            return "INPUT_DEVTYPE_KEYBOARD";
        case INPUT_DEVTYPE_MOUSE:
            return "INPUT_DEVTYPE_MOUSE";
        }
    return "UNKNOWN";
    }

LPCSTR GetDeviceSubTypeString(BYTE bType, BYTE bSubType)
    {
    switch(bType)
        {
        case INPUT_DEVTYPE_GAMECONTROLLER:
            switch(bSubType)
                {
                case INPUT_DEVSUBTYPE_GAMEPAD:
                    return "INPUT_DEVSUBTYPE_GAMEPAD";
                case INPUT_DEVSUBTYPE_JOYSTICK:
                    return "INPUT_DEVSUBTYPE_JOYSTICK";
                case INPUT_DEVSUBTYPE_WHEEL:
                    return "INPUT_DEVSUBTYPE_WHEEL";
                case INPUT_DEVSUBTYPE_FISHINGROD:
                    return "INPUT_DEVSUBTYPE_FISHINGROD";
                case INPUT_DEVSUBTYPE_DANCEPAD:
                    return "INPUT_DEVSUBTYPE_DANCEPAD";
                case INPUT_DEVSUBTYPE_LIGHTGUN:
                    return "INPUT_DEVSUBTYPE_LIGHTGUN";
                default:
                    return "UNKNOWN";
                }

        case INPUT_DEVTYPE_KEYBOARD:
            switch(bSubType)
                {
                case INPUT_DEVSUBTYPE_KEYBOARD:
                    return "INPUT_DEVSUBTYPE_KEYBOARD";
                default:
                    return "UNKNOWN";
                }

        case INPUT_DEVTYPE_MOUSE:
            switch(bSubType)
                {
                case INPUT_DEVSUBTYPE_MOUSE:
                    return "INPUT_DEVSUBTYPE_MOUSE";
                case INPUT_DEVSUBTYPE_TRACKBALL:
                    return "INPUT_DEVSUBTYPE_TRACKBALL";
                case INPUT_DEVSUBTYPE_TOUCHPAD:
                    return "INPUT_DEVSUBTYPE_TOUCHPAD";
                case INPUT_DEVSUBTYPE_THUMBSTICK:
                    return "INPUT_DEVSUBTYPE_THUMBSTICK";
                default:
                    return "UNKNOWN";
                }

        }
    return "UNKNOWN";
    }

void DumpDeviceInfo(PCINPUT_DEVICE_INFORMATION pDeviceInfo)
    {
    DebugPrint("USBTEST: Device Information:\n");
    DebugPrint("   dwSize:         %d\n", pDeviceInfo->dwSize);
    DebugPrint("   bDeviceID:      %d\n", (DWORD)pDeviceInfo->bDeviceID);
    DebugPrint("   bDeviceType:    %s\n", GetDeviceTypeString(pDeviceInfo->bDeviceType));
    DebugPrint("   bDeviceSubType: %s\n", GetDeviceSubTypeString(pDeviceInfo->bDeviceType, pDeviceInfo->bDeviceSubtype));
    DebugPrint("   bPlayerSlot:    %d\n", (DWORD)pDeviceInfo->bPlayerSlot);
    }

void DumpDeviceState(PINPUT_FORMAT_GAME pGamePacket)
    {
    DebugPrint("USBTEST: Device State:\n");
    DebugPrint("   bSize:                       %d\n", pGamePacket->bSize);
    DebugPrint("   bReportId:                   %d\n", pGamePacket->bReportId);
    DebugPrint("   Analog Buttons:              ");
    for(int i=0; i<8; i++)
        {
        DebugPrint("%d ",(DWORD)pGamePacket->rgbAnalogButtons[i]);
        }
    DebugPrint("\n   Digital Button (bitfield):   0x%0.4x\n",(DWORD)pGamePacket->wButtons);
    DebugPrint("   DPAD(like POV):              0x%0.4x\n",(DWORD)pGamePacket->wDPad);
    DebugPrint("   Left Thumb Stick:            (%d,%d)\n",(DWORD)pGamePacket->wThumb1X, (DWORD)pGamePacket->wThumb1Y);
    DebugPrint("   Right Thumb Stick:           (%d,%d)\n",(DWORD)pGamePacket->wThumb2X, (DWORD)pGamePacket->wThumb2Y);
    DebugPrint("   Tilt Axes:                   (%d,%d)\n\n",(DWORD)pGamePacket->wTiltX, (DWORD)pGamePacket->wTiltY);
    }
*/