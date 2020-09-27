#include "usbtest.h"
#include "draw.h"
#include "devices.h"


// Hack! DbgPrint is much nicer than OutputDebugString so JonT exported it from xapi.lib
// (as DebugPrint) but the prototype isn't in the public headers...
extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}

CGameController::~CGameController()
{
    
}

void CGameController::Init(DWORD dwPort, INT iLeft, INT iTop)
{
    // Record this the basic stuff
    m_dwPort = dwPort;
    m_iLeft  = iLeft;
    m_iTop   = iTop;

    //
    //  Initialize test state details
    //
    m_iState        = DEVICE_STATE_NOT_INSERTED;
    m_iReenumCount  = 0;
    m_fDisconnectGamepadHub = FALSE;

    //
    //  Initialize the synchronization stuff.
    //
    InitializeCriticalSection(&m_CriticalSection);
    m_hSignalEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    //
    //  Start the worker thread
    //
    HANDLE hThread = CreateThread(NULL, 65535, (LPTHREAD_START_ROUTINE)WorkerThreadEntry, (LPVOID)this, 0, NULL);
    CloseHandle(hThread);
}

void CGameController::Inserted()
{
    EnterCriticalSection(&m_CriticalSection);
    m_iState = DEVICE_STATE_INSERTED;
    LeaveCriticalSection(&m_CriticalSection);
    SetEvent(m_hSignalEvent);
}
void CGameController::Removed(){} //Does nothing for now

DWORD WINAPI CGameController::WorkerThreadEntry(CGameController *pGameController)
{
    pGameController->WorkerThread();
    return 0;
}
void CGameController::WorkerThread()
{
    HANDLE      hDevice;
    IUsbDevice  *pParentHub;;
    IUsbDevice  *pXidDevice;
    IUsbDevice  *pRootHub;
    UCHAR       ucHubPort;
    UCHAR       ucHubPortInRoot;
    KIRQL       oldIrql;
    DWORD       dwWaitReason;

    //
    //  Wait for device insertion (no timeout)
    //
    WaitForSingleObject(m_hSignalEvent, INFINITE);

    //
    //  Assume that the state DEVICE_STATE_INSERTED
    //
    ASSERT(DEVICE_STATE_INSERTED == m_iState);

    //
    //  Open the device and get the handle
    //
    while(1)
    {
        Sleep(200); 
        //
        //  Reinit all the IUsbDevice pointers
        //
        pXidDevice = NULL;
        pParentHub = NULL;
        pRootHub = NULL;
       
        //
        //  We need to:
        //      1) Open the device.
        //      2) Sleep for 200 ms, while it polls 25 times or so
        //      3) Close the device.
        //      4) Get IUsbDevice interface for a gamepad in our port
        //      5) Get its parent's IUsbDevice interface (the internal hub).
        //      6) Get its parent's parent's IUsbDevice interface (for the root hub, or the daughter
        //         card hub.
        //      5) Report the device has not responding.
        //  If we managed to get to stop 5) without a failure then we set the status to 
        //  DEVICE_STATE_REENUMERATING.  Otherwise, we drop through to the next section.
        //

        HANDLE hDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, m_dwPort, XDEVICE_NO_SLOT, NULL);
        if(hDevice)
        {
            Sleep(200);
            XInputClose(hDevice);
        }

        oldIrql = KeRaiseIrqlToDpcLevel();
        pXidDevice = GetXidDeviceInterface(XDEVICE_TYPE_GAMEPAD, m_dwPort, 0);
        if(pXidDevice)
        {
            ucHubPort = GetHubPort(pXidDevice);
            pParentHub = GetParentInterface(pXidDevice);
            if(pParentHub)
            {
                ucHubPortInRoot = GetHubPort(pParentHub);
                pRootHub = GetParentInterface(pParentHub);
                
                if(pRootHub)
                {
                    KeLowerIrql(oldIrql);
                    if(m_fDisconnectGamepadHub)
                    {
                        m_fDisconnectGamepadHub = FALSE;
                        DisableHubPortSync(pRootHub, ucHubPortInRoot);
                        oldIrql = KeRaiseIrqlToDpcLevel();
                    } else
                    {
                        //
                        //  For disconnecting at the gamepad case we have to
                        //  hit reset too.  Why?  The XID driver will only
                        //  detect a device that doesn't respond if the device
                        //  is open.  Since we don't open the device,
                        //  well it won't come back automagically.
                        //
                        m_fDisconnectGamepadHub = TRUE;
                        DisableHubPortSync(pParentHub, ucHubPort);
                        oldIrql = KeRaiseIrqlToDpcLevel();
                        ResetDevice(pXidDevice, 0);
                    }
                    m_iState = DEVICE_STATE_REENUMERATING;
                    m_iReenumCount++;
                }
            }
        }
        KeLowerIrql(oldIrql);

        //
        //  Wait for the device to come back.  This presumes that we got to step 5) above.
        //  If not we will certainly timeout.  If the device fails enumeration (5 times)
        //  we will also timeout.  We do not distinguish.
        //
        //  In either case we query the root hub to see if there is still a device in the
        //  port.  If there is then somehow the device (or our API) failed.  We mark it
        //  a failure.  Otherwise, we simply record that there is no device attached.
        //  
        dwWaitReason = WaitForSingleObject(m_hSignalEvent, 2000); //Up to two seconds
        if(WAIT_TIMEOUT==dwWaitReason)
        {
            EnterCriticalSection(&m_CriticalSection);
            if(pRootHub && IsDeviceAttached(pRootHub, ucHubPortInRoot))
            {
                m_iState = DEVICE_STATE_FAILED;
            } else
            {
                m_iState = DEVICE_STATE_NOT_INSERTED;
            }
            LeaveCriticalSection(&m_CriticalSection);
            WaitForSingleObject(m_hSignalEvent, INFINITE);  //Wait for next insertion
            m_iReenumCount=0;
        }
    }
}

void CGameController::Draw(CDraw& draw)
{
    //
    //  Sync with display info
    //
    INT iState, iReenumCount;

    EnterCriticalSection(&m_CriticalSection);
    iState = m_iState;
    iReenumCount = m_iReenumCount;
    LeaveCriticalSection(&m_CriticalSection);
    
    //
    //  Clear area
    //
    draw.FillRect(m_iLeft, m_iTop, 150, 400, BACKGROUND_BLUE);
    //
    //  Draw Stop Light background
    //
    draw.FillCircle(m_iLeft+75, m_iTop+75, 60, PITCH_BLACK);
    draw.FillCircle(m_iLeft+75, m_iTop+305, 60, PITCH_BLACK);
    draw.FillRect(m_iLeft+15, m_iTop+75, 120, 230, PITCH_BLACK);

    WCHAR FormatBuffer[80];
    INT redColor; 
    INT yellowColor;
    INT greenColor;
    INT textXPos;
    INT textYPos;

    //
    //  Draw Red Light
    //
    if(DEVICE_STATE_FAILED == iState)
    {
        redColor = BRIGHT_RED;
        wsprintf(FormatBuffer, L"Failed Try %d", iReenumCount);
        textXPos = m_iLeft+40;
        textYPos = m_iTop+75;
    } else
    {
        redColor = DARK_RED;
    }
    draw.FillCircle(m_iLeft+75, m_iTop+75, 50, redColor);

    //
    //  Draw Yellow Light
    //
    if(DEVICE_STATE_NOT_INSERTED == iState)
    {
        yellowColor = BRIGHT_YELLOW;
        wsprintf(FormatBuffer, L"Insert Device");
        textXPos = m_iLeft+40;
        textYPos = m_iTop+195;
    } else
    {
        yellowColor = DARK_YELLOW;
    }
    draw.FillCircle(m_iLeft+75, m_iTop+195, 50, yellowColor);

    //
    //  Draw Green Light
    //
    if( 
        (DEVICE_STATE_INSERTED == iState) ||
        (DEVICE_STATE_REENUMERATING == iState)
    )
    {
        greenColor = BRIGHT_GREEN;
        wsprintf(FormatBuffer, L"Succeeded Try %d ", iReenumCount);
        textXPos = m_iLeft+40;
        textYPos = m_iTop+305;
    } else
    {
        greenColor = DARK_GREEN;
    }
    draw.FillCircle(m_iLeft+75, m_iTop+305, 50, greenColor);
    
    //
    //  Draw Text
    //
    draw.DrawText(FormatBuffer, textXPos, textYPos, PITCH_BLACK);
}


CGameControllers::CGameControllers()
{
    int i;
    for(i=0; i < 4; i++)
    {
        m_pGameControllers[i] = new CGameController();
        if(!m_pGameControllers[i])
        {
            DebugPrint("Failed to allocate a game controller!");
        } else
        {
            m_pGameControllers[i]->Init(i, 40+i*150, 40);
        }
    }
    DWORD dwDevices;
    dwDevices = XGetDevices(XDEVICE_TYPE_GAMEPAD);
    for(i=0; i < 4; i++)
    {
        if(dwDevices&(1 << i)) m_pGameControllers[i]->Inserted();
    }
}

CGameControllers::~CGameControllers()
{
    int i;
    for(i=0; i < 4; i++)
    {
        delete m_pGameControllers[i];
    }
}

void CGameControllers::CheckForHotplugs()
{
    DWORD dwInsertions, dwRemovals;
    if(XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals))
    {
        int i;
        for(i=0; i < 4; i++)
        {
            if(dwRemovals&(1 << i))
            {
                m_pGameControllers[i]->Removed();
            }
            if(dwInsertions&(1 << i))
            {
                m_pGameControllers[i]->Inserted();
            }
        }
    }
}
void CGameControllers::Draw(CDraw& draw)
{
    for(int i=0; i < 4; i++) m_pGameControllers[i]->Draw(draw);
}
