#include "usbtst.h"
#include <brainbox.h>

#define FAILURE_RED          0xff9f0000
#define PENDING_YELLOW       0xffffff00
#define UNKNOWN_ORANGE       0xffff7f00
#define SUCCESS_GREEN        0xff009f00
#define WAITING_WHITE        0xffffffff

class CBrainBox
{
  public:
    CBrainBox() : m_dwConnectedDevices(0),
               m_dwCurrentDevice(0xFFFFFFFF),
               m_hBrainBox(NULL),
               m_fOutstandingOutput(FALSE)
    {
        memset(&m_BrainBoxData, 0, sizeof(m_BrainBoxData));
        memset(m_LastKnownLEDs, 0, sizeof(m_LastKnownLEDs));
        memset(m_DesiredLEDs, 0, sizeof(m_DesiredLEDs));
    }
    void Init();
    void Run();

  private:

// Render Loop
    void CheckForHotplug();
    void PollData();
    void UpdateLEDs();
    void Render();

// Graphics Helpers
    CDraw   m_draw;
    CXBFont *m_pFont;

// Data
    DWORD                   m_dwConnectedDevices;
    DWORD                   m_dwCurrentDevice;
    HANDLE                  m_hBrainBox;
    XINPUT_BRAINBOX         m_BrainBoxData;
    WORD                    m_LastKnownLEDs[3];
    WORD                    m_DesiredLEDs[3];
    XINPUT_BRAINBOX_LEDS    m_BrainBoxLEDPacket;
    BOOL                    m_fOutstandingOutput;
};

//------------------------------------------------------------------------------
// TestMain
//------------------------------------------------------------------------------
void __cdecl main()
{
    CBrainBox brainBox;
    brainBox.Init();
    brainBox.Run();
}

void CBrainBox::Init()
{
    //Initialize core peripheral port support
    XInitDevices(0,NULL);

    m_pFont = m_draw.CreateFont("d:\\media\\font16.xpr");
}

void CBrainBox::Run()
{
    while(1)
    {
        CheckForHotplug();
        PollData();
        UpdateLEDs();
        Render();
    }
}


void CBrainBox::CheckForHotplug()
{
    DWORD dwInsertions, dwRemovals;
    if(XGetDeviceChanges(XDEVICE_TYPE_BRAINBOX, &dwInsertions, &dwRemovals))
    {
        //If the current device was removed, stop using it.
        if(m_hBrainBox && (1 << m_dwCurrentDevice)&dwRemovals)
        {   
            XInputClose(m_hBrainBox);
            m_hBrainBox=NULL;
            memset(&m_BrainBoxData, 0, sizeof(m_BrainBoxData));
            memset(m_LastKnownLEDs, 0, sizeof(m_LastKnownLEDs));
            memset(m_DesiredLEDs, 0, sizeof(m_DesiredLEDs));
            m_fOutstandingOutput = FALSE;
        }
        m_dwConnectedDevices &= ~dwRemovals;
        m_dwConnectedDevices |= dwInsertions;
        if(!m_hBrainBox && m_dwConnectedDevices)
        {
            DWORD dwMask=1;
            //Find first device
            m_dwCurrentDevice = 0;
            while(m_dwCurrentDevice<4)
            {
                if(dwMask&m_dwConnectedDevices)
                {
                    XINPUT_POLLING_PARAMETERS pollingParameters;
                    pollingParameters.fAutoPoll = TRUE;
                    pollingParameters.bInputInterval = 8;
                    pollingParameters.bOutputInterval = 8;
                    pollingParameters.fInterruptOut = TRUE;
                    pollingParameters.ReservedMBZ1 = 0;
                    pollingParameters.ReservedMBZ2 = 0;
                    m_hBrainBox = XInputOpen(XDEVICE_TYPE_BRAINBOX, m_dwCurrentDevice, 0, &pollingParameters);
                    if(m_hBrainBox)
                    {
                        return;
                    }
                }
                dwMask <<= 1;
                m_dwCurrentDevice++;
            }
        }
    }
}

void CBrainBox::PollData()
{
    if(m_hBrainBox)
    {
        XInputGetState(m_hBrainBox, (PXINPUT_STATE)&m_BrainBoxData);
        m_DesiredLEDs[0] = m_BrainBoxData.data.bmButtons[0];
        m_DesiredLEDs[1] = m_BrainBoxData.data.bmButtons[1];
        m_DesiredLEDs[2] = m_BrainBoxData.data.bmButtons[2]&0x00000;
    }
}


void CBrainBox::UpdateLEDs()
{
    if(m_hBrainBox)
    {
      //Check for completed I/O
      if(m_fOutstandingOutput && (ERROR_IO_PENDING!=m_BrainBoxLEDPacket.Header.dwStatus))
      {
         m_fOutstandingOutput = FALSE;
         if(ERROR_SUCCESS == m_BrainBoxLEDPacket.Header.dwStatus)
         { 
		    memcpy(m_LastKnownLEDs, m_BrainBoxLEDPacket.bmLEDs, sizeof(m_LastKnownLEDs));
         }	
      }

      //If there is no outstanding, update the LED state to be the same as the button state
      if(!m_fOutstandingOutput)
      {
          for(int i=0; i<3; i++)
          {
            if(m_LastKnownLEDs[i] != m_DesiredLEDs[i])
            {
		        memcpy(m_BrainBoxLEDPacket.bmLEDs, m_DesiredLEDs, sizeof(m_DesiredLEDs));
                m_BrainBoxLEDPacket.Header.dwStatus = ERROR_IO_PENDING;
                m_BrainBoxLEDPacket.Header.hEvent = 0;
                m_fOutstandingOutput = TRUE;
                XInputSetState(m_hBrainBox, (XINPUT_FEEDBACK *)&m_BrainBoxLEDPacket);
                return;
            }
          }
      }
    }
}


void CBrainBox::Render()
{
    WCHAR wszFormatBuffer[1024];
    m_draw.FillRect(0,0,640,480, PITCH_BLACK);
    if(!m_hBrainBox)
    {
        m_pFont->DrawText(70, 95, FAILURE_RED, L"Insert Device");
    } else
    {
        wsprintf(wszFormatBuffer, L"Device In Port %d", m_dwCurrentDevice);
        m_pFont->DrawText(70, 70, BRIGHT_BLUE, wszFormatBuffer);
    
        wsprintf(wszFormatBuffer, L"Buttons 0x%0.4x%0.4x%0.4x", 
          (DWORD)m_BrainBoxData.data.bmButtons[2],
          (DWORD)m_BrainBoxData.data.bmButtons[1],
          (DWORD)m_BrainBoxData.data.bmButtons[0]);
        m_pFont->DrawText(70, 95, BRIGHT_BLUE, wszFormatBuffer);

        wsprintf(wszFormatBuffer, L"Right Lever (0x%0.4x, 0x%0.4x)", (DWORD)m_BrainBoxData.data.wRightLeverX,(DWORD)m_BrainBoxData.data.wRightLeverY);
        m_pFont->DrawText(70, 120, BRIGHT_BLUE, wszFormatBuffer);
        
        wsprintf(wszFormatBuffer, L"Left Lever (0x%0.4x)", (DWORD)m_BrainBoxData.data.wLeftLeverX);
        m_pFont->DrawText(70, 145, BRIGHT_BLUE, wszFormatBuffer);

        wsprintf(wszFormatBuffer, L"Upper Left Lever (0x%0.4x, 0x%0.4x)", (DWORD)m_BrainBoxData.data.wUpperLeftLeverX, (DWORD)m_BrainBoxData.data.wUpperLeftLeverY);
        m_pFont->DrawText(70, 170, BRIGHT_BLUE, wszFormatBuffer);
        
        wsprintf(wszFormatBuffer, L"Pedals (0x%0.4x, 0x%0.4x, 0x%0.4x)",
            (DWORD)m_BrainBoxData.data.wFootPedal1,
            (DWORD)m_BrainBoxData.data.wFootPedal2,
            (DWORD)m_BrainBoxData.data.wFootPedal3);
        m_pFont->DrawText(70, 195, BRIGHT_BLUE, wszFormatBuffer);

        wsprintf(wszFormatBuffer, L"Tuner (0x%0.2x)", (DWORD)m_BrainBoxData.data.bTuner);
        m_pFont->DrawText(70, 220, BRIGHT_BLUE, wszFormatBuffer);

        wsprintf(wszFormatBuffer, L"Shift Lever (0x%0.2x)", (DWORD)m_BrainBoxData.data.bShiftLever);
        m_pFont->DrawText(70, 245, BRIGHT_BLUE, wszFormatBuffer);
    }
    m_draw.Present();
}
