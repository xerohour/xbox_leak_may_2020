/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Dummy version 2.0 XBE to test autoupdate and autoreboot

Module Name:

    v2title.cpp

--*/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"

//======================== The main function
void __cdecl main()
{
    HRESULT		hr = S_OK;
    DWORD		dwTitleId;
    WCHAR		szMessage[1024];

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
			L"\nCongratulations!!!!! AutoReboot works for T I T L E S!!!\n"
			L"Title ID: %08x (%u)\n"
			L"Current Version: %08x (%u)\n",
			XeImageHeader()->Certificate->TitleID,
			XeImageHeader()->Certificate->TitleID,
			XeImageHeader()->Certificate->Version,
			XeImageHeader()->Certificate->Version);
			
	Xputs(szMessage);

	PresentScene();

	while (1)
	{
		Sleep(1000);
	}

	// Break here
	_asm int 3;

	return;
}

