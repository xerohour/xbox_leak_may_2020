//-----------------------------------------------------------------------------
// File: DukeProd2.cpp
//
// Desc: General purpose test tool for input devices based on XBApplication Framework and
//		  Dukeprod
//
// Hist: Oct 01 2001 - Created
//
//
// Author:  Dennis Krueger <a-denkru>
//
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <process.h>
#include "dukeprod2.h"
//#include "controls.h"
#include "draw.h"
#include "TestControls.h"

//-----------------------------------------------------------------------------
// Name: class CInputTestApp
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CInputTestApp 
{
public:
    CInputTestApp();
	~CInputTestApp();

    virtual HRESULT Initialize(BYTE bPort);
	virtual void Run();

	CTestController  * m_pController;

	BOOL	m_fStopRender;
	BOOL	m_fStopRefresh;
	HANDLE  m_hSynchRender;
	HANDLE  m_hSynchRefresh;

};


DWORD WINAPI Render(void * pParam);
DWORD WINAPI Refresh(void * pParam);

//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    XInitDevices(0,NULL); // initialize the devices

	drInit();
	// find all attached controllers
	Sleep(2000);  // delay a couple seconds to allow emumeration to complete

	while(1) // run until rebooted
	{
		DWORD dwDeviceMask = XGetDevices( XDEVICE_TYPE_GAMEPAD );
		if(0 == dwDeviceMask)
		{
			// no devices connected, continue to spin
			drCls();
			drSetSize(50, 100, 0xffff0000, 0xff000000); //set the color
			drPrintf(200,150, L"No Controller Detected!");
			drShowScreen();

			continue;
		}
		BYTE bPort = 0;
		for(int i = 1; i < 9;i <<= 1)
		{
			if(dwDeviceMask & i)
			{
				// found the first slot supporting a controller, exit
				break;
			}
			bPort++;
		}

		CInputTestApp * xbApp = new CInputTestApp();
		if( FAILED( xbApp->Initialize(bPort) ) )
			return;
		drCls();
		drShowScreen();
		xbApp->Run();
		delete xbApp; // end of test for this controller

	}
}



//-----------------------------------------------------------------------------
// Name: CXBoxSample (constructor)
// Desc: Constructor for CInputTestApp class
//-----------------------------------------------------------------------------
CInputTestApp::CInputTestApp() 
{
;
}

CInputTestApp::~CInputTestApp()
{
	delete m_pController;
}


void
CInputTestApp::Run()
{
	HANDLE hResult;

// spawn a refresh thread so we don't need to maintain that either
	m_fStopRefresh = FALSE;

	hResult = CreateThread(NULL,0,Refresh,this,0,NULL);
	if(NULL == hResult)
	{
		DWORD dwResult = GetLastError();
		return;
	}
	Sleep(0); // allow refresh thread to run

// spawn the render thread so we don't need to maintain it
	m_fStopRender = FALSE;
	hResult = CreateThread(NULL,0,Render,this,0,NULL);
	if(NULL == hResult)
	{
		DWORD dwResult = GetLastError();
		return;
	}

	BOOL fResult = m_pController->Test();
	while(FALSE == m_pController->m_fRemoved)
	{
		Sleep(0); // release quantum
		// spin until/if removed
	}
	m_fStopRefresh = TRUE; // turn off refresh
	m_fStopRender = TRUE; // stop rendering

	// wait for refresh and render to quit
	WaitForSingleObject(m_hSynchRefresh,INFINITE);
	WaitForSingleObject(m_hSynchRender,INFINITE);
	
	return;



}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT
CInputTestApp::Initialize(BYTE bPort)
{

	m_fStopRender = FALSE; // init flags
	m_fStopRefresh = FALSE;
    // init drawing from cpxlib
    // locate the controller to initialize
	m_hSynchRefresh = CreateEvent(NULL,FALSE,FALSE,NULL);
	if(NULL == m_hSynchRefresh) return E_FAIL;
	m_hSynchRender = CreateEvent(NULL,FALSE,FALSE,NULL);
	if(NULL == m_hSynchRender) return E_FAIL;



	// found the first port with a controller, this is the one we'll test
	m_pController = new CTestController;
	DWORD dwResult = m_pController->InitTest(bPort);
	if(0 != dwResult)
	{
		// failed init
		return E_FAIL;
	}

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
DWORD
WINAPI Refresh(void * pThis)
{
	DWORD dwResult;
	
	CInputTestApp * pApp = (CInputTestApp *) pThis;


	DWORD dwAddDevice,dwRemoveDevice, dwMuMask;
	// make mask for mu test
	dwMuMask = (1 << pApp->m_pController->m_dwPort) | (0x10000 << pApp->m_pController->m_dwPort); // check upper and lower slots on our port
	while(FALSE == pApp->m_fStopRefresh)
	{
		if(XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD,&dwAddDevice, &dwRemoveDevice))
		{
			// a controller has been added or removed, all we care about is removal for the
			// ...controller under test
			if(dwRemoveDevice & 1 << pApp->m_pController->m_dwPort)
			{
				// this controller has been removed, close up shop
				pApp->m_pController->m_fRemoved = TRUE;
				pApp->m_fStopRender = TRUE;
				pApp->m_fStopRefresh = TRUE;
				continue;
			}
		}

		dwResult = pApp->m_pController->Refresh();  // keep values fresh
		if(ERROR_DEVICE_NOT_CONNECTED == dwResult)
		{
			pApp->m_pController->m_fRemoved = TRUE;
			pApp->m_fStopRefresh = TRUE;
			pApp->m_fStopRender = TRUE;
			continue; // drop out of while
		}


		// maintain MU status here
        if(XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &dwAddDevice, &dwRemoveDevice))
		{
			if(dwAddDevice & dwMuMask) // catch either upper or lower port
			{
				if(dwAddDevice & 0xffff0000)
				{
					pApp->m_pController->m_dwSlot[1] = TRUE;
				}
				if(dwAddDevice & 0x0000ffff)
				{
					pApp->m_pController->m_dwSlot[0] = TRUE;
				}
			}
			// do removals after adds, the same port could have been inserted and removed during same call
#ifdef NEVER  // don't record removals
			if(dwRemoveDevice & dwMuMask) // catch either upper or lower port
			{
				if(dwRemoveDevice & 0xffff0000)
				{
					pApp->m_pController->m_dwSlot[1] = FALSE;
				}
				if(dwRemoveDevice & 0x0000ffff)
				{
					pApp->m_pController->m_dwSlot[0] = FALSE;
				}
			}
#endif // 0
		}


		Sleep(0); // release quantum
	}
	SetEvent(pApp->m_hSynchRefresh); // inform main loop we're done

	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
DWORD WINAPI Render(void * pThis)
{
	CInputTestApp * pApp = (CInputTestApp *) pThis;
	
// setup frame rate

	UINT iFlasher = 0;

	while(FALSE == pApp->m_fStopRender)
	{
		Sleep(100); // refresh 10 times a second
		drCls();
		pApp->m_pController->Draw();
		drShowScreen();
		// toggle blink every .5 secs for display processes
		if(0 == ++iFlasher % 5) 
		{
			if(TRUE == pApp->m_pController->m_fFlash)
				 pApp->m_pController->m_fFlash = FALSE;
			else 
				pApp->m_pController->m_fFlash =  TRUE;
		}
		pApp->m_pController->DecCountDown(); // dec .1 sec count down timer for client test timeouts
	}
	pApp->m_pController->SetCountDown(0); // clear countdown timer to release any waiting tests
	SetEvent(pApp->m_hSynchRender); // inform main loop we're done
    return S_OK;
}

