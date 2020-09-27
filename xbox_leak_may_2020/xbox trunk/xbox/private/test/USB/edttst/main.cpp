/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    edttst.cpp	

Abstract:
    
    Test App to exercise high-level keyboard support.
    
    
Environment:

    Designed for XBOX.

Notes:

    
    
Revision History:

    12-26-00 created by Mitchell Dernis (mitchd)

--*/

#include <xtl.h>
#include "draw.h"
#include "edit.h"
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

//
//  Useful Macro
//

//------------------------------------------------------------------------------
// TestMain
//------------------------------------------------------------------------------

void __cdecl main()
{
    int iFrameCount=0;
    int iState = 0;
    HRESULT hr;
    DebugPrint("Edit Test.\nBuilt on %s at %s\n", __DATE__, __TIME__);
    
    //Initialize core peripheral port support
    XInitDevices(0,NULL);

    CDraw draw;
    /*
    hr = draw.CreateFont("Font16.xpr");
    if(FAILED(hr))
    {
        DebugPrint("Edit Test.\nCouldn't find font.\n", __DATE__, __TIME__);
    }*/

    CEditBox editBox(0,250,640,230, draw);
    draw.FillRect(0, 0, 640, 480, BACKDROP_BLUE); 
    draw.DrawText(L"Edit Test Application", 300,  1, LABEL_WHITE);
    draw.Present();
    draw.DrawText(L"Edit Test Application", 300,  1, LABEL_WHITE);
    
    CKeyboards *pKeyboards = NULL;
    pKeyboards = new CKeyboards(NULL);
    while(1)
    {
        pKeyboards->CheckForHotplugs();
        //pKeyboards->UpdateState();
        editBox.ProcessInput(draw);
        pKeyboards->Draw(draw);
        editBox.Draw(draw);
        draw.Present();
        Sleep(16);
    }
}

