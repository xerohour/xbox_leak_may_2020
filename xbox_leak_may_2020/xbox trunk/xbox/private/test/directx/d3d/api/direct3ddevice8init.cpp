#include "d3dapi.hpp"
#pragma warning(disable:4035)
__inline DWORD OneIfZero(DWORD num) {
	_asm {
		mov eax, [num]
		cmp eax, 1
		adc eax, 0
	}
}
#pragma warning(default:4035)

LINKME(7)

BEGINTEST(testDirect3dDevice8Release)
{
	DWORD dr;
	xStartVariation(hlog, "Create Device");

	if(g_pd3dd8 == NULL) {
		_asm {int 3}
	}
	dr = g_pd3dd8->Release();
	CHECKRESULT(dr == 0);
	if(WASBADRESULT()) {
		xLog(hlog, XLL_INFO, "release returned %d, should be 0", dr);
	}
	xEndVariation(hlog);
}
ENDTEST()

int gDeviceTypeIndex = 0;

#pragma data_seg(".d3dapi$test025") //goes after Direct3d8

BEGINTEST(testDirect3dCreateDevice1)
{
	D3DDISPLAYMODE DisplayModes[] = 
	{
	    { 640,  480, 60, 0,D3DFMT_LIN_R5G6B5 },
	    { 640,  480, 60, 0,D3DFMT_LIN_A8R8G8B8 },
	};
	
	int RefreshRates[] = {
		60, 
		D3DPRESENT_RATE_DEFAULT, 
		D3DPRESENT_RATE_UNLIMITED
	};

	int BackBufferCounts[] = {
		0, 
		1, 
		2, 
//		3
	};

	struct MULTISAMPLES {
		D3DSWAPEFFECT se;
		D3DMULTISAMPLE_TYPE mst;
	} MultiSamples[] = {
		{D3DSWAPEFFECT_DISCARD, D3DMULTISAMPLE_NONE},
		{D3DSWAPEFFECT_FLIP, D3DMULTISAMPLE_NONE},
//		{D3DSWAPEFFECT_COPY, D3DMULTISAMPLE_NONE},
//		{D3DSWAPEFFECT_COPY_VSYNC, D3DMULTISAMPLE_NONE}
	};
	UINT Intervals[] = {
		D3DPRESENT_INTERVAL_IMMEDIATE,
		D3DPRESENT_INTERVAL_DEFAULT,
		D3DPRESENT_INTERVAL_ONE,
	};

	DWORD Flags[] = {
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
//        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
//        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE,
	};
	
	DWORD PpFlags[] = {
		0, 
		D3DPRESENTFLAG_LOCKABLE_BACKBUFFER
	};

	struct STENCILS {
		BOOL u;
		D3DFORMAT s;
	} Stencils[] = {
		{FALSE, D3DFMT_D16_LOCKABLE},
//		{TRUE, D3DFMT_D15S1},
		{TRUE, D3DFMT_D16_LOCKABLE},
//		{TRUE, D3DFMT_D24S8},
//		{TRUE, D3DFMT_D32},
//		{TRUE, D3DFMT_D16},
//		{TRUE, D3DFMT_D24X4S4},
        {TRUE, D3DFMT_D24S8}
	};

	DWORD dr;
	DWORD flags;
	D3DPRESENT_PARAMETERS pp;

	#define SETCDPARAMS(a,b,c,d,e,f,g,h)						\
		xLog(hlog, XLL_INFO, "Params: width%d height%d format%d"\
			"backbuffercount%d multisampletype%d multisamples%d"\
			"enable stencils%d stencil format%d ppflags%08x rr%d"\
			"presentation interval%08x Flags%08x",				\
			pp.BackBufferWidth = DisplayModes[a].Width,			\
			pp.BackBufferHeight = DisplayModes[a].Height,		\
			pp.BackBufferFormat = DisplayModes[a].Format,		\
			pp.BackBufferCount = BackBufferCounts[c],			\
			pp.MultiSampleType = MultiSamples[d].mst,			\
			pp.SwapEffect = MultiSamples[d].se,					\
			pp.EnableAutoDepthStencil = Stencils[h].u,			\
			pp.AutoDepthStencilFormat = Stencils[h].s,			\
			pp.Flags = PpFlags[g],								\
			pp.FullScreen_RefreshRateInHz = RefreshRates[b],	\
			pp.FullScreen_PresentationInterval = Intervals[e],	\
			flags = Flags[f]									\
		);														\
		pp.hDeviceWindow = NULL;								\
		pp.Windowed = FALSE;								

//	int a,b,c,d,e,f,g,h,i,j;

	xStartVariation(hlog, "Create Device");
	switch (gDeviceTypeIndex) {
	case 0:
		SETCDPARAMS(0,0,0,0,LASTOF(Intervals),0,0,0);    //Minimum requrements
		break;
	case 1:
        SETCDPARAMS(LASTOF(DisplayModes),1,2,1,0,1,1,2); //maximum requirements
		break;
	}

	gDeviceTypeIndex++;
	if(gDeviceTypeIndex > 1) {
		gDeviceTypeIndex = 0;
	} else {
		RegisterDoAgain(0);
	}

//	__try {
		dr = g_pd3d8->CreateDevice(0, D3DDEVTYPE_HAL, 0, flags, &pp, &g_pd3dd8);
//	} __except(1) {
//		xLog(hlog, XLL_EXCEPTION, "Exception.");
//		dr = D3DERR_DRIVERINTERNALERROR;
//	}

	BLOCKRESULT(dr == D3D_OK);
	if(WASGOODRESULT()) {
		RegisterDeinitFunc(ptestDirect3dDevice8Release);
	} else {
		xLog(hlog, XLL_INFO, "CreateDevice returned %d", dr);
	}

	xEndVariation(hlog);
}
ENDTEST()

#pragma data_seg()
