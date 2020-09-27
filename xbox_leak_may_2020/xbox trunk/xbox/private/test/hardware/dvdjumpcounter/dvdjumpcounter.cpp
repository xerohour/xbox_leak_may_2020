
#include "xbapp.h"
#include "xmenu.h"

#include <devioctl.h>
#include <scsi.h>
#include <ntddscsi.h>

//
// Macros to swap the byte order of a USHORT or ULONG at compile time.
//

#define IdexConstantUshortByteSwap(ushort) \
    ((((USHORT)ushort) >> 8) + ((((USHORT)ushort) & 0x00FF) << 8))

#define IdexConstantUlongByteSwap(ulong) \
    ((((ULONG)ulong) >> 24) + ((((ULONG)ulong) & 0x00FF0000) >> 8) + \
    ((((ULONG)ulong) & 0x0000FF00) << 8) + ((((ULONG)ulong) & 0x000000FF) << 24))


// Program option struct
struct OPTIONS
{
	BOOL DisplayCounter;
	BOOL ResetCounter;
};

// Global options
OPTIONS     g_Options;

//-----------------------------------------------------------------------------
// Name: DisplayJumpCounter()
// Desc: Menu callback for first menu item select
//-----------------------------------------------------------------------------
DWORD SetDisplayJumpCounter(DWORD cmd, XMenuItem *pmi)
{
	OutputDebugStringA("SetDisplayJumpCounter\n");
	g_Options.DisplayCounter = TRUE;
    return MROUTINE_DIE;
}


//-----------------------------------------------------------------------------
// Name: ResetJumpCounter()
// Desc: Menu callback for second menu item select
//-----------------------------------------------------------------------------
DWORD SetResetJumpCounter(DWORD cmd, XMenuItem *pmi)
{
	OutputDebugStringA("SetResetJumpCounter\n");
	g_Options.ResetCounter = TRUE;
    return MROUTINE_DIE;
}


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication {
private:
    // Font for rendering stats and help
    FLOAT           m_fFontHeight;
    FLOAT           m_fFontWidth;
    CXBFont         m_Font;
	
	// Menu system
    DWORD           m_dwMenuCommand;
    XMenu*          m_pMainMenu;

	// Display flag and strings
	BOOL			m_TextDisplayed;
	WCHAR			m_szDisplayLine1[64];
	WCHAR			m_szDisplayLine2[64];
	
	// Menu functions
	VOID DisplayJumpCounter();
	VOID ResetJumpCounter();
	VOID DisplayOutputStrings();

	VOID LogSenseData(SENSE_DATA SenseData);
	VOID ReadLBA(HANDLE hDevice, ULONG LBA, USHORT Sectors);

public:
    HRESULT Initialize();
    HRESULT Render();
    HRESULT FrameMove();

    CXBoxSample();
};

//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}

//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample():CXBApplication()
{
	m_pMainMenu = NULL;

    m_d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	m_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize all dependencies and states
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
	HRESULT             hr;

	// Clear options
	g_Options.DisplayCounter = FALSE;
	g_Options.ResetCounter = FALSE;
	m_TextDisplayed = FALSE;

	// Font
    if( FAILED( hr = m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    m_Font.GetTextExtent( L"M", &m_fFontWidth, &m_fFontHeight, TRUE );

	// Disable antialiasing by default
    m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);

    // Display initial wait screen
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL , 0x00000000, 0.0f, 0L);
	m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL , 0x00000000, 0.0f, 0L);
    m_pd3dDevice->Present(NULL, NULL, NULL, NULL);

	// Menu
    XMenu_SetFont(&m_Font);

    m_pMainMenu = XMenu_Init(320.0f, 100.0f, 2, MENU_NOBACK, NULL);
    m_pMainMenu->topcolor = 0xffffffff;
    m_pMainMenu->bottomcolor = 0xffffffff;
    m_pMainMenu->itemcolor = 0xff000000;
    m_pMainMenu->seltopcolor = 0xffcedfad;
    m_pMainMenu->selbotcolor = 0xff84b642;

	XMenu_SetTitle(m_pMainMenu, L"DVD Seek Jump Counter", 0xff000000);

    XMenu_AddItem(m_pMainMenu, MITEM_ROUTINE, L"Display", SetDisplayJumpCounter);  
	XMenu_AddItem(m_pMainMenu, MITEM_ROUTINE, L"Reset", SetResetJumpCounter);  

	XMenu_Activate(m_pMainMenu);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{

	// Process options
	if(g_Options.DisplayCounter) 
	{
		DisplayJumpCounter();
	} 
	else if(g_Options.ResetCounter)
	{
		ResetJumpCounter();
	}

	// Check for 'Back' button press
	if(m_DefaultGamepad.wPressedButtons&XINPUT_GAMEPAD_BACK)
	{
		OutputDebugStringA("Back Button Press\n");
		g_Options.DisplayCounter = FALSE;
		g_Options.ResetCounter = FALSE;
		m_TextDisplayed = FALSE;
		m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL , 0x00000000, 0.0f, 0L);
		m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL , 0x00000000, 0.0f, 0L);
		m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		XMenu_Activate(m_pMainMenu);
	}

    if(XMenu_IsActive())
	{
		m_dwMenuCommand = XMenu_GetCommand(&m_DefaultGamepad);
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
	if(XMenu_IsActive())
	{
		XMenu_Routine(m_dwMenuCommand); 
	} else 
	{
		// m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DisplayJumpCounter
// Desc: Load output strings with contents of DVD Seek Jump Counter
//-----------------------------------------------------------------------------
VOID CXBoxSample::DisplayJumpCounter()
{
	char cText[64];

	BOOL bReturn, bSuccess = TRUE;

	HANDLE hDevice;
    DWORD cbBytesReturned;
	SENSE_DATA SenseData;
    SCSI_PASS_THROUGH_DIRECT PassThrough;
	UCHAR Counter[8];
	ULONG i;
	
	if(!m_TextDisplayed)
	{
		// Open DVD device
		hDevice = CreateFile("cdrom0:",	GENERIC_READ, 0, NULL, OPEN_EXISTING, 
								FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL);
		if(hDevice == INVALID_HANDLE_VALUE) 
		{
			OutputDebugStringA("DisplayJumpCounter: CreateFile FAILED\n");
			bSuccess = FALSE;
		}

		// Do some jumps
		/*
		ULONG LBA = 0;
		for(i = 0; i < 5; i++)
		{
			ReadLBA(hDevice, LBA, 1);

			// Toggle LBA
			if(LBA == 0)
			{
				LBA = 333109;
			}
			else
			{
				LBA = 0;
			}
		}
		*/


		// Get counter from DVD
		ZeroMemory(Counter, sizeof(Counter));
		ZeroMemory(&PassThrough, sizeof(PassThrough));
		PassThrough.Length = sizeof(PassThrough);
		PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
		PassThrough.DataBuffer = Counter;
		PassThrough.DataTransferLength = sizeof(Counter);
		// PassThrough.CdbLength = 12;
		PassThrough.Cdb[0] =0x25;
		PassThrough.Cdb[1] =0xFF;

		ZeroMemory(&SenseData, sizeof(SenseData));
		cbBytesReturned = 0;

		bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
									&PassThrough, sizeof(PassThrough), 
									&SenseData, sizeof(SenseData),
									&cbBytesReturned, NULL);

		if((!bReturn) || (cbBytesReturned > 0)) 
		{
			if(!bReturn)
			{
				OutputDebugStringA("DisplayJumpCounter: DeviceIoControl returned FALSE\n");
				wsprintfA(cText, "DisplayJumpCounter: LastError = %i\n", GetLastError());
				OutputDebugStringA(cText);
			}

			if(cbBytesReturned > 0)
			{
				OutputDebugStringA("DisplayJumpCounter: DeviceIoControl cbBytesReturned not 0\n");
				LogSenseData(SenseData);
			}
			bSuccess = FALSE;

		}

		CloseHandle(hDevice);

		// Load Display text
		if(bSuccess)
		{
			wsprintfW(m_szDisplayLine1, L"Seek Jump Counter = 0x%.2x%.2x%.2x%.2x\n",
				Counter[0], Counter[1], Counter[2], Counter[3]);

			for(i = 0; i < 8; i++)
			{
				wsprintfA(cText, "Counter[%d] = %#x\n", i, Counter[i]);
				OutputDebugStringA(cText);
			}
		}
		else
		{
			wsprintfW(m_szDisplayLine1, L"Display Seek Jump Counter FAILED");
		}
		wsprintfW(m_szDisplayLine2, L"Press 'Back' button to continue");
		DisplayOutputStrings();		
		m_TextDisplayed = TRUE;
	}
}


//-----------------------------------------------------------------------------
// Name: ResetJumpCounter
// Desc: Reset DVD Seek Jump Counter
//-----------------------------------------------------------------------------
VOID CXBoxSample::ResetJumpCounter()
{
	char cText[64];

	BOOL bReturn, bSuccess = TRUE;

	HANDLE hDevice;
    DWORD cbBytesReturned;
	SENSE_DATA SenseData;
    SCSI_PASS_THROUGH_DIRECT PassThrough;
    
	if(!m_TextDisplayed)
	{
		// Reset DVD counter
		hDevice = CreateFile("cdrom0:",	GENERIC_READ, 0, NULL, OPEN_EXISTING, 
								FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL);
		if(hDevice == INVALID_HANDLE_VALUE) 
		{
			OutputDebugStringA("ResetJumpCounter: CreateFile FAILED\n");
			bSuccess = FALSE;
		}

		ZeroMemory(&PassThrough, sizeof(PassThrough));
		PassThrough.Length = sizeof(PassThrough);
		PassThrough.DataIn = SCSI_IOCTL_DATA_OUT;
		PassThrough.DataBuffer = NULL;
		PassThrough.DataTransferLength = 0;
		PassThrough.CdbLength = 12;
		PassThrough.Cdb[0] =0x1B;
		PassThrough.Cdb[4] =0xFF;

		ZeroMemory(&SenseData, sizeof(SenseData));
		cbBytesReturned = 0;

		bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
									&PassThrough, sizeof(PassThrough), 
									&SenseData, sizeof(SenseData),
									&cbBytesReturned, NULL);

		if((!bReturn) || (cbBytesReturned > 0)) 
		{
			if(!bReturn)
			{
				OutputDebugStringA("ResetJumpCounter: DeviceIoControl returned FALSE\n");
				wsprintfA(cText, "ResetJumpCounter: LastError = %i\n", GetLastError());
				OutputDebugStringA(cText);
			}

			if(cbBytesReturned > 0)
			{
				OutputDebugStringA("ResetJumpCounter: DeviceIoControl cbBytesReturned not 0\n");
				LogSenseData(SenseData);
			}
			bSuccess = FALSE;

		}

		CloseHandle(hDevice);

		// Load Display text
		if(bSuccess)
		{
			wsprintfW(m_szDisplayLine1, L"Seek Jump Counter Reset");
		}
		else
		{
			wsprintfW(m_szDisplayLine1, L"Seek Jump Counter Reset FAILED");
		}
		wsprintfW(m_szDisplayLine2, L"Press 'Back' button to continue");
		DisplayOutputStrings();
		m_TextDisplayed = TRUE;
	}
}

//-----------------------------------------------------------------------------
// Name: DisplayOutputStrings
// Desc: Display output strings
//-----------------------------------------------------------------------------
VOID CXBoxSample::DisplayOutputStrings()
{
	OutputDebugStringA("DisplayOutputStrings\n");
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL , 0x00000000, 0.0f, 0L);
	m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL , 0x00000000, 0.0f, 0L);
	m_Font.Begin();
	m_Font.DrawText(50.0, 50.0, 0xffffffff, m_szDisplayLine1);
	m_Font.DrawText(50.0, (float)(50 + m_Font.m_dwFontHeight), 0xffffffff, m_szDisplayLine2);
	m_Font.End();
	m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

VOID CXBoxSample::LogSenseData(SENSE_DATA SenseData)
{
	char cText[64];

	// Log SenseData

	wsprintfA(cText, "SenseData ErrorCode:  0x%.2x\n", SenseData.ErrorCode);	
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData Valid:  0x%.2x\n", SenseData.Valid);	
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData SegmentNumber:  0x%.2x\n", SenseData.SegmentNumber);
	OutputDebugStringA(cText);

    wsprintfA(cText, "SenseData SenseKey:  0x%.2x\n", SenseData.SenseKey);
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData Reserved:  0x%.2x\n", SenseData.Reserved);	
	OutputDebugStringA(cText);

    wsprintfA(cText, "SenseData IncorrectLength:  0x%.2x\n", SenseData.IncorrectLength);
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData Reserved:  0x%.2x\n", SenseData.Reserved);
	OutputDebugStringA(cText);
	
    wsprintfA(cText, "SenseData IncorrectLength:  0x%.2x\n", SenseData.IncorrectLength);	
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData EndOfMedia:  0x%.2x\n", SenseData.EndOfMedia);	
	OutputDebugStringA(cText);

    wsprintfA(cText, "SenseData FileMark:  0x%.2x\n", SenseData.FileMark);	
	OutputDebugStringA(cText);

    wsprintfA(cText, "SenseData Information[0]:  0x%.2x\n", SenseData.Information[0]);
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData Information[1]:  0x%.2x\n", SenseData.Information[1]);
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData Information[2]:  0x%.2x\n", SenseData.Information[2]);
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData Information[3]:  0x%.2x\n", SenseData.Information[3]);	
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData AdditionalSenseLength:  0x%.2x\n", SenseData.AdditionalSenseLength);	
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData CommandSpecificInformation[0]:  0x%.2x\n", SenseData.CommandSpecificInformation[0]);	
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData CommandSpecificInformation[1]:  0x%.2x\n", SenseData.CommandSpecificInformation[1]);	
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData CommandSpecificInformation[2]:  0x%.2x\n", SenseData.CommandSpecificInformation[2]);	
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData CommandSpecificInformation[3]:  0x%.2x\n", SenseData.CommandSpecificInformation[3]);	
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData AdditionalSenseCode:  0x%.2x\n", SenseData.AdditionalSenseCode);	
	OutputDebugStringA(cText);

    wsprintfA(cText, "SenseData AdditionalSenseCodeQualifier:  0x%.2x\n", SenseData.AdditionalSenseCodeQualifier);	
	OutputDebugStringA(cText);

    wsprintfA(cText, "SenseData FieldReplaceableUnitCode:  0x%.2x\n", SenseData.FieldReplaceableUnitCode);	
	OutputDebugStringA(cText);

    wsprintfA(cText, "SenseData SenseKeySpecific[0]:  0x%.2x\n", SenseData.SenseKeySpecific[0]);	
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData SenseKeySpecific[1]:  0x%.2x\n", SenseData.SenseKeySpecific[1]);	
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData SenseKeySpecific[2]:  0x%.2x\n", SenseData.SenseKeySpecific[2]);	
	OutputDebugStringA(cText);

	wsprintfA(cText, "SenseData SenseKeySpecific[3]:  0x%.2x\n", SenseData.SenseKeySpecific[3]);	
	OutputDebugStringA(cText);
}


VOID CXBoxSample::ReadLBA(HANDLE hDevice, ULONG LBA, USHORT Sectors) {
	SCSI_PASS_THROUGH_DIRECT PassThrough;
	SENSE_DATA SenseData;
	DWORD cbBytesReturned;
	PCDB Cdb;
	BOOL bReturn;
	UCHAR Buf[2048];
	char cText[64];


    RtlZeroMemory(&PassThrough, sizeof(PassThrough));

    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
    PassThrough.DataBuffer = Buf;
    PassThrough.DataTransferLength = sizeof(Buf);

	Cdb = (PCDB)&PassThrough.Cdb;
    Cdb->CDB10.OperationCode = SCSIOP_READ;
	Cdb->CDB10.ForceUnitAccess = 1;
	Cdb->CDB10.LogicalBlock = IdexConstantUlongByteSwap(LBA);
	Cdb->CDB10.TransferBlocks = IdexConstantUshortByteSwap(Sectors);

    bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
								&PassThrough, sizeof(PassThrough), 
								&SenseData, sizeof(SenseData),
								&cbBytesReturned, NULL);

	if((!bReturn) || (cbBytesReturned > 0)) 
	{
		if(!bReturn)
		{
			OutputDebugStringA("ReadLBA: DeviceIoControl returned FALSE\n");
			wsprintfA(cText, "ReadLBA: LastError = %i\n", GetLastError());
			OutputDebugStringA(cText);
		}

		if(cbBytesReturned > 0)
		{
			OutputDebugStringA("ReadLBA: DeviceIoControl cbBytesReturned not 0\n");
			LogSenseData(SenseData);
		}

	}
}
