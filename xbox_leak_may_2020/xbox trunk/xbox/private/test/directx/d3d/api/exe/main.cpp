/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    main.cpp

Description:

    Direct3D Build Verification Tests.

*******************************************************************************/

#include <xtl.h>
#include <xlog.h>

extern "C" void D3DAPISTART(HANDLE h);
extern "C" void DebugPrint(char*, ...);


#ifndef UNDER_XBOX
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow) 
#else
int __cdecl main()
#endif // UNDER_XBOX
{
	HANDLE log;
	WEB_SERVER_INFO wsi = {
		L"",
		L"",
		L"",
		L"",
		L"",
	};

	log = xCreateLog(L"t:\\d3dapi.log", NULL, INVALID_SOCKET, XLL_LOGALL, XLO_DEBUG | XLO_CONFIG | XLO_STATE | XLO_REFRESH);
	DebugPrint("%p\n", log);
	D3DAPISTART(log);
	xCloseLog(log);
    return 0;
}
