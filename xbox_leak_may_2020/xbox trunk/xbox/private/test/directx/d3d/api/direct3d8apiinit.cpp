#include "d3dapi.hpp"

LINKME(5)

BEGINTEST(testDirect3D8_Release)
{
	xStartVariation(hlog, "Direct3d8Release");
//	__try {
		DWORD d = g_pd3d8->Release();
		CHECKRESULT(d == 0 || d == 1);
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "returned: %d", d);
//	} __except(1) {
//		xLog(hlog, XLL_EXCEPTION, "Exception in IDirect3d8::Release()");
//	}

	g_pd3d8 = NULL;
	xEndVariation(hlog);
}
ENDTEST()

#pragma data_seg(".d3dapi$test010")
BEGINTEST(testDirect3DCreate8)
{
	IDirect3D8 * pid;
	xStartVariation(hlog, "Direct3DCreate8(D3D_SDK_VERSION)");
	pid = Direct3DCreate8(D3D_SDK_VERSION);
	BLOCKRESULT(pid != NULL);

	if(WASBADRESULT()) {
		xLog(hlog, XLL_INFO, "Error %d", GetLastError());
	} else {
		g_pd3d8 = pid;
		RegisterDeinitFunc(ptestDirect3D8_Release);
	}
	xEndVariation(hlog);
}
ENDTEST()
#pragma data_seg()