/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Dummy version 2.0 Dash XBE to test autoupdate and autoreboot

Module Name:

    v2dash.cpp

--*/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"

#include "xboxp.h"

//======================== The main function
void __cdecl main()
{
    HRESULT		hr = S_OK;
    DWORD		dwError;
    DWORD		dwTitleId;
    WCHAR		szMessage[1024];

	DWORD				dwType;
	LD_LAUNCH_DASHBOARD ld;
	LD_LAUNCH_DASHBOARD ld2;
	PLD_FROM_TITLE_UPDATE pldftu = (PLD_FROM_TITLE_UPDATE)&ld;
	PLD_FROM_DASHBOARD pldfd = (PLD_FROM_DASHBOARD)&ld2;
	
	dwError = XGetLaunchInfo(&dwType, (PLAUNCH_DATA)&ld);
	if (dwError != ERROR_SUCCESS)
	{
		hr = HRESULT_FROM_WIN32(dwError);
		goto Error;
	}

	// Make a reference to xonline.lib so we link in the 
	// reboot logic to make sure we don't screw up in the reboot
	// logic
    dwTitleId = XeImageHeader()->Certificate->TitleID;

	// Initialize graphics stuff
	hr = InitializeGraphics();
	if (FAILED(hr))
	{
        DebugPrint("Cannot initialize graphics (%08x)\n", hr);
        RIP("Failed to initialize graphics");
        return;
	}

	// Initialize text font
	hr = InitializeFont(0xffffffff, 0xff000000, XFONT_OPAQUE);
	if (FAILED(hr))
	{
        DebugPrint("Cannot initialize text (%08x)\n", hr);
        RIP("Failed to initialize text");
        return;
	}

	BeginNewScene();

	wsprintf(szMessage,
			L"\nCongratulations!!!!! AutoReboot works for D A S H!!!\n"
			L"Title ID: %08x (%u)\n"
			L"Current Version: %08x (%u)\n\n\n",
			XeImageHeader()->Certificate->TitleID,
			XeImageHeader()->Certificate->TitleID,
			XeImageHeader()->Certificate->Version,
			XeImageHeader()->Certificate->Version);
	Xputs(szMessage);

	if (dwType == LDT_FROM_TITLE_UPDATE)
	{
		// We were just updated, boot back to the app in d:
		if (FAILED(pldftu->hr))
		{
			wsprintf(szMessage,
					L"\nHowever, autoupdate for D A S H failed (%08x)\n\n"
					L"Rebooting to d:\\default.xbe in f3 seconds ...", pldftu->hr);
			Xputs(szMessage);
		}
		else
		{
			wsprintf(szMessage,
					L"\nAutoupdate for D A S H also succeeded with context (%08x)\n\n"
					L"Rebooting to d:\\default.xbe in 3 seconds ...", 
					pldftu->dwContext);
			Xputs(szMessage);
		}
		
		PresentScene();

		Sleep(3000);

		pldfd->dwContext = pldftu->dwContext;
		XLaunchNewImage("d:\\default.xbe", (PLAUNCH_DATA)&ld2);
	}

	PresentScene();

Wait:
	while (1)
	{
		Sleep(1000);
	}

	// Break here
	_asm int 3;

	return;

Error:	

	BeginNewScene();

	wsprintf(szMessage,
			L"\nAutoReboot for D A S H failed (%08x)\n", hr);
			
	Xputs(szMessage);

	PresentScene();

	goto Wait;
}

