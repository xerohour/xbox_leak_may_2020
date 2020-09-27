#include "d3dapi.hpp"
#include <stdio.h>

LINKME(4)

INT gNumModes = 0;

D3DDISPLAYMODE gDisplayModes[100];

D3DFORMAT format[] = {
//	D3DFMT_R8G8B8,
	D3DFMT_A8R8G8B8,
	D3DFMT_X8R8G8B8,
	D3DFMT_R5G6B5,
	D3DFMT_X1R5G5B5,
//	D3DFMT_A1R5G5B5,
//	D3DFMT_A4R4G4B4,
//	D3DFMT_R3G3B2,
//	D3DFMT_A8,
//	D3DFMT_A8R3G3B2,
//	D3DFMT_X4R4G4B4,

//	D3DFMT_A8P8,
//	D3DFMT_P8,

//	D3DFMT_L8,
//	D3DFMT_A8L8,
//	D3DFMT_A4L4,

/*	D3DFMT_V8U8,
	D3DFMT_L6V5U5,
	D3DFMT_X8L8V8U8,
	D3DFMT_Q8W8V8U8,
	D3DFMT_V16U16,
	*/
//	D3DFMT_W11V11U10,

/*
	D3DFMT_UYVY,
	D3DFMT_YUY2,
	D3DFMT_DXT1,
	D3DFMT_DXT2,
	D3DFMT_DXT3,
	D3DFMT_DXT4,
	D3DFMT_DXT5,
*/
//	D3DFMT_D16_LOCKABLE,
//	D3DFMT_D32,
//	D3DFMT_D15S1,
//	D3DFMT_D24S8,
//	D3DFMT_D16,
 //   D3DFMT_D24S8,
//	D3DFMT_D24X4S4,

//	D3DFMT_VERTEXDATA,
//	D3DFMT_INDEX16,
//  D3DFMT_INDEX32
};

#pragma data_seg(".d3dapi$test005")

BEGINTEST(testDirect3DCreate8Release)
{
	IDirect3D8 * pid;
	xStartVariation(hlog, "Direct3DCreate8(D3D_SDK_VERSION)");
	STARTLEAKCHECK();
	pid = Direct3DCreate8(D3D_SDK_VERSION);
	BLOCKRESULT(pid != NULL);
	DWORD d;

	if(WASBADRESULT()) {
		xLog(hlog, XLL_INFO, "Error %d", GetLastError());
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "D3dRelease(no device)");
	__try {
		d = pid->Release();
		CHECKRESULT(d == 0 || d == 1);
	} __except(1) {
		xLog(hlog, XLL_EXCEPTION, "Exception raised in IDirect3d8::Release()");
	}

	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

#pragma data_seg(".d3dapi$test020")

BEGINTEST(testDirect3DGetAdapterCount)
{
	DWORD d;
	xStartVariation(hlog, "IDirect3d8::GetAdapterCount");
	STARTLEAKCHECK();
	d = g_pd3d8->GetAdapterCount();
	CHECKRESULT(d == 1);
	if(WASBADRESULT()) {
		xLog(hlog, XLL_INFO, "returned %d, Error %d", d,GetLastError());
	}
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testDirect3DGetAdapterIdentifier)
{
	D3DADAPTER_IDENTIFIER8 dai8;
	DWORD d;

	xStartVariation(hlog, "GetAdapterIdentifier(flags=0)");
	STARTLEAKCHECK();
	d = g_pd3d8->GetAdapterIdentifier(0, 0, &dai8);
	CHECKRESULT(d == D3D_OK);
	xEndVariation(hlog);

	xStartVariation(hlog, "GetAdapterIdentifier(flags=D3DENUM_NO_WHQL_LEVEL)");
	d = g_pd3d8->GetAdapterIdentifier(0, D3DENUM_NO_WHQL_LEVEL, &dai8);
	CHECKRESULT(d == D3D_OK);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testDirect3DGetAdapterModeCount)
{
	UINT d;

	xStartVariation(hlog, "IDirect3d8::GetAdapterModeCount(0)");
	STARTLEAKCHECK();
	d = g_pd3d8->GetAdapterModeCount(0);
	BLOCKRESULT(d != 0);
	xLog(hlog, XLL_INFO, "returned %d", d);
	CHECKLEAKS();
	xEndVariation(hlog);
	gNumModes = d;
}
ENDTEST()

BEGINTEST(testGetAdapterDisplayMode)
{
	D3DDISPLAYMODE m;
	DWORD d;

	xStartVariation(hlog, "IDirect3d8::GetAdapterDisplayMode");
	STARTLEAKCHECK();
	d = g_pd3d8->GetAdapterDisplayMode(0, &m);
	CHECKRESULT(d == D3D_OK);
	if(WASBADRESULT()) {
		xLog(hlog, XLL_INFO, "returned %d", d);
	}
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testDirect3DCheckDeviceType)
{
	DWORD d;
	D3DFORMAT types[] = {
//		D3DFMT_R8G8B8,
		D3DFMT_X8R8G8B8,
		D3DFMT_X8R8G8B8,
		D3DFMT_R5G6B5,
		D3DFMT_X1R5G5B5,
//		D3DFMT_A1R5G5B5,
//		D3DFMT_X4R4G4B4,
//		D3DFMT_R3G3B2,
//		D3DFMT_P8
//		D3DFMT_R3G3B2,
//		D3DFMT_X4R4G4B4
	};
	D3DFORMAT backtypes[] = {
//		D3DFMT_R8G8B8,
		D3DFMT_A8R8G8B8,
		D3DFMT_X8R8G8B8,
		D3DFMT_R5G6B5,
		D3DFMT_X1R5G5B5,
//		D3DFMT_A1R5G5B5,
//		D3DFMT_A4R4G4B4,
//		D3DFMT_R3G3B2,
//		D3DFMT_P8
//		D3DFMT_A8R3G3B2,
//		D3DFMT_X4R4G4B4
	};
	int i;

	xStartVariation(hlog, "D3DCheckDeviceType(0,1,x,x,0)");
	STARTLEAKCHECK();
	for (i = 0; i < sizeof(backtypes) / sizeof(backtypes[0]); i++)
	{
		d = g_pd3d8->CheckDeviceType(0, D3DDEVTYPE_HAL, types[i], backtypes[i], FALSE);
		CHECKRESULTNOLOG((d == D3D_OK) || (d == D3DERR_INVALIDDEVICE));
		xLog(hlog, GETRESULT(), "CheckDeviceType(0, 1, %d, %d, 0)", types[i],backtypes[i]);
	}
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()


BEGINTEST(testDirect3DCheckDeviceFormat)
{
	DWORD d;
	

	DWORD usage[] = {
		0, 
		D3DUSAGE_DEPTHSTENCIL,
		D3DUSAGE_RENDERTARGET,
		D3DUSAGE_DEPTHSTENCIL | D3DUSAGE_RENDERTARGET
	};

	D3DRESOURCETYPE resourcetype[] = {
		D3DRTYPE_SURFACE,
		D3DRTYPE_VOLUME,
		D3DRTYPE_TEXTURE,
		D3DRTYPE_VOLUMETEXTURE,
		D3DRTYPE_CUBETEXTURE,
		D3DRTYPE_VERTEXBUFFER,
		D3DRTYPE_INDEXBUFFER
	};
	int indeces[5];

	xStartVariation(hlog, "CheckDeviceFormat(rnd)");
	STARTLEAKCHECK();
	for(int i = 0; i < 50; i++) {
		d = g_pd3d8->CheckDeviceFormat(0, D3DDEVTYPE_HAL,
			format[indeces[0] = rnd() % COUNTOF(format)],
			usage[indeces[1] = rnd() % COUNTOF(usage)],
			resourcetype[indeces[2] = rnd() % COUNTOF(resourcetype)],
			format[indeces[3] = rnd() % COUNTOF(format)]);
		CHECKRESULT((d == D3D_OK) || (d == D3DERR_NOTAVAILABLE));
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, 
				"CheckDeviceFormat(0, D3DDEVTYPE_HAL, %d, %d, %d, %d)",
				indeces[0], indeces[1], indeces[2], indeces[3]);
		}
	}
	CHECKLEAKS();
	xEndVariation(hlog);
/*
	for(int formatindex = 0; formatindex < sizeof(format)/sizeof(format[0]);formatindex++) {
		for(int usageindex = 0; usageindex < sizeof(usage)/sizeof(usage[0]);usageindex++) {
			for(int rindex = 0; rindex < sizeof(resourcetype)/sizeof(resourcetype[0]);rindex++) {
				g_pd3d8->CheckDeviceFormat(0, D3DDEVTYPE_HAL, 
					format[formatindex],usage[usageindex],
					resourcetype[rindex],
			}
		}

	}
*/
}
ENDTEST()


BEGINTEST(testDirect3DCheckDeviceMultiSampleType)
{
	DWORD d;
	int type;
	xStartVariation(hlog, "CheckDeviceMultiSampleType");
	STARTLEAKCHECK();
	for (int i = 0; i < COUNTOF(format); i++) {
//        switch(i & 3) {
  //      case 0:
			type = 0;
//            break;
//        case 1:
//            type = 2;
//            break;
//        case 2:
//            type = 16;
//            break;
//        case 3:
//            type = rnd() % 13 + 3;
//            break;
//        }
		d = g_pd3d8->CheckDeviceMultiSampleType(0,D3DDEVTYPE_HAL,format[i],0,(D3DMULTISAMPLE_TYPE)type);
		CHECKRESULT((d == D3D_OK) || (d == D3DERR_NOTAVAILABLE));
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, 
				"CheckDeviceMultiSampleType(0, D3DDEVTYPE_HAL, %d, 0, %d)",
				i, type);
		}
	}
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testDirect3DCheckDepthStencilMatch)
{
	DWORD d;
	D3DFORMAT types[] = {
//		D3DFMT_R8G8B8,
		D3DFMT_A8R8G8B8,
		D3DFMT_X8R8G8B8,
		D3DFMT_R5G6B5,
		D3DFMT_X1R5G5B5,
//		D3DFMT_A1R5G5B5,
//		D3DFMT_X4R4G4B4,
//		D3DFMT_R3G3B2,
//		D3DFMT_P8
//		D3DFMT_R3G3B2,
//		D3DFMT_X4R4G4B4
	};
	D3DFORMAT backtypes[] = {
//		D3DFMT_R8G8B8,
		D3DFMT_A8R8G8B8,
		D3DFMT_X8R8G8B8,
		D3DFMT_R5G6B5,
		D3DFMT_X1R5G5B5,
//		D3DFMT_A1R5G5B5,
//		D3DFMT_A4R4G4B4,
//		D3DFMT_R3G3B2,
//		D3DFMT_P8
//		D3DFMT_A8R3G3B2,
//		D3DFMT_X4R4G4B4
	};

	D3DFORMAT depths[] = {
		D3DFMT_D16_LOCKABLE,
//		D3DFMT_D32,
//		D3DFMT_D15S1,
//		D3DFMT_D24S8,
//		D3DFMT_D16,
        D3DFMT_D24S8
//		D3DFMT_D24X4S4
	};

	xStartVariation(hlog, "CheckDeviceMultiSampleType");
	STARTLEAKCHECK();
	for(int i = 0; i < COUNTOF(types); i++) {
		for (int j = 0; j < 1; j++) {
			d = g_pd3d8->CheckDepthStencilMatch(0, D3DDEVTYPE_HAL,
				types[i], backtypes[i], 
				depths[(i + j) % COUNTOF(depths)]);
			CHECKRESULT((d == D3D_OK) || (d == D3DERR_NOTAVAILABLE));
			if(WASBADRESULT()) {
				xLog(hlog, XLL_INFO, 
					"CheckDepthStencilMatch(0, D3DDEVTYPE_HAL, %d, %d, %d)",
					types[i], backtypes[i], depths[(i + j) % COUNTOF(depths)]);
			}
		}
	}
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testDirect3DGetDeviceCaps)
{
	D3DCAPS8 c;
	DWORD d;

	xStartVariation(hlog, "IDirect3d8::GetDeviceCaps");
	STARTLEAKCHECK();
	d = g_pd3d8->GetDeviceCaps(0, D3DDEVTYPE_HAL, &c);
	CHECKRESULT(d == D3D_OK);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()


BEGINTEST(testDirect3DEnumAdapterModes)
{
	D3DDISPLAYMODE m;
	DWORD d;
	INT mode;

	xStartVariation(hlog, "EnumAdapterModes");
	STARTLEAKCHECK();

	for(mode = 0; mode < gNumModes; mode++) {
		d = g_pd3d8->EnumAdapterModes(0, mode, &m);
		CHECKRESULT(d == D3D_OK);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "EnumAdapterModes(0, %d, %p) returned %d", mode, &m, d);
		} else {
			gDisplayModes[mode] = m;
		}
	}

	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()


void DrawStuff() 
{
	//draw various numbers of triangles here.
}


BEGINTEST(testDirect3DCreateDevice)
{
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
	};
	UINT Intervals[] = {
		D3DPRESENT_INTERVAL_IMMEDIATE,
//		D3DPRESENT_INTERVAL_TWO,
		D3DPRESENT_INTERVAL_ONE,
	};

	DWORD Flags[] = {
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
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
//		{TRUE, D3DFMT_D16_LOCKABLE},
//		{TRUE, D3DFMT_D24S8},
//		{TRUE, D3DFMT_D32},
//		{TRUE, D3DFMT_D16},
//		{TRUE, D3DFMT_D24X4S4},
        {TRUE, D3DFMT_D24S8}
	};

	DWORD dr;
	DWORD flags;
	D3DPRESENT_PARAMETERS pp;

	#define SETCDPARAMS(a,b,c,d,e,f,g,h)						 \
		xLog(hlog, XLL_INFO, "wid%d hgt%d fmt%d "\
			"BkBufCt%d MultiSamType%d MSs%d "\
			"Stencils?%d StFmt%d ppflags%08x "    \
			"RefRate%d PresInt%08x Flags%08x",\
			pp.BackBufferWidth = gDisplayModes[a].Width,			 \
			pp.BackBufferHeight = gDisplayModes[a].Height,		 \
			pp.BackBufferFormat = gDisplayModes[a].Format,		 \
			pp.BackBufferCount = BackBufferCounts[c],			 \
			pp.MultiSampleType = MultiSamples[d].mst,			 \
			pp.SwapEffect = MultiSamples[d].se,					 \
			pp.EnableAutoDepthStencil = Stencils[h].u,			 \
			pp.AutoDepthStencilFormat =							 \
                (gDisplayModes[a].Format == D3DFMT_LIN_A8R8G8B8 ? \
				  (Stencils[h].s == D3DFMT_LIN_D16 ?             \
				    D3DFMT_LIN_D24S8 : Stencils[h].s)            \
				: Stencils[h].s),                                \
			pp.Flags = PpFlags[g],								 \
			pp.FullScreen_RefreshRateInHz = RefreshRates[b],	 \
			pp.FullScreen_PresentationInterval = Intervals[e],	 \
			flags = Flags[f]									 \
		);														 \
		pp.hDeviceWindow = NULL;								 \
		pp.Windowed = FALSE;								

/*
	#define CDDOIT																	\
		__try {																		\
			dr = g_pd3d8->CreateDevice(0, D3DDEVTYPE_HAL, 0, flags, &pp, &g_pd3dd8);\
			CHECKRESULT(dr == D3D_OK);												\
			if(dr == D3D_OK) {														\
				dr = g_pd3dd8->AddRef();											\
				CHECKRESULT(dr == 2);												\
				dr = g_pd3dd8->Release();											\
				CHECKRESULT(dr == 1);												\
				if(WASBADRESULT()) {												\
					xLog(hlog, XLL_INFO, "Release returned %d", dr);				\
				} else {															\
					dr = g_pd3dd8->Release();										\
					CHECKRESULT(dr == 0);											\
					if(WASBADRESULT()) xLog(hlog, XLL_INFO, "Release returned %d", dr);\
				}																	\
			}																		\
			else {																	\
				xLog(hlog, XLL_INFO, "CreateDevice returned %d", dr);				\
			}																		\
		} __except(1) {																\
			xLog(hlog, XLL_EXCEPTION, "Exception.");								\
		}
*/
	#define CDDOIT																	   \
			dr = g_pd3d8->CreateDevice(0, D3DDEVTYPE_HAL, 0, flags, &pp, &g_pd3dd8);   \
			CHECKRESULT(dr == D3D_OK);												   \
			if(dr == D3D_OK) {														   \
				dr = g_pd3dd8->AddRef();											   \
				CHECKRESULT(dr == 2);												   \
				dr = g_pd3dd8->Release();											   \
				CHECKRESULT(dr == 1);												   \
				if(WASBADRESULT()) {												   \
					xLog(hlog, XLL_INFO, "Release returned %d", dr);				   \
				} else {																\
                    DrawStuff();                                                        \
					dr = g_pd3dd8->Release();										   \
					CHECKRESULT(dr == 0);											   \
					if(WASBADRESULT()) xLog(hlog, XLL_INFO, "Release returned %d", dr);\
				}																	   \
			}																		   \
			else {																	   \
				xLog(hlog, XLL_INFO, "CreateDevice returned %d", dr);				   \
			}																		   \


	int a,b,c,d,e,f,g,h,i,j;

	xStartVariation(hlog, "CreateRelease for leak checking");
	SETCDPARAMS(0,0,0,0,0,0,0,0);
	CDDOIT;
	xEndVariation(hlog);

	xStartVariation(hlog, "Display Modes loop");
	STARTLEAKCHECK();
	for (i = 0; i < gNumModes; i++) {
		for(j = 0; j < COUNTOF(RefreshRates); j++) {
			SETCDPARAMS(i,j,0,0,0,0,0,0);
			CDDOIT;
		}
	}
	xEndVariation(hlog);
//    RESETRESULT();

	xStartVariation(hlog, "BackBuffer count loop");
	for (i = 0; i < COUNTOF(BackBufferCounts); i++) {
		SETCDPARAMS(0,0,i,0,0,0,0,0);
		CDDOIT;
	}
	xEndVariation(hlog);
//    RESETRESULT();

	xStartVariation(hlog, "Multisamples count loop");
	for (i = 0; i < COUNTOF(MultiSamples); i++) {
		SETCDPARAMS(0,0,0,i,0,0,0,0);
		CDDOIT;
	}
	xEndVariation(hlog);
//    RESETRESULT();

	xStartVariation(hlog, "Intervals loop");
	for (i = 0; i < COUNTOF(Intervals); i++) {
		SETCDPARAMS(0,0,0,0,i,0,0,0);
		CDDOIT;
	}
	xEndVariation(hlog);
//    RESETRESULT();
	
	xStartVariation(hlog, "Flags loop");
	for (i = 0; i < COUNTOF(Flags); i++) {
		for (j = 0; j < COUNTOF(PpFlags); j++) {
			SETCDPARAMS(0,0,0,0,0,i,j,0);
			CDDOIT;
		}
	}
	xEndVariation(hlog);
//    RESETRESULT();

	xStartVariation(hlog, "Stencils loop");
	for (i = 0; i < COUNTOF(Stencils); i++) {
		SETCDPARAMS(0,0,0,0,0,0,0,i);
		CDDOIT;
	}
	xEndVariation(hlog);
//    RESETRESULT();

	xStartVariation(hlog, "Extrema loop");
	for (a = 0; a < gNumModes;     a += 1)     { //2
    for (b = 0; b < COUNTOF(RefreshRates);     b += 1/*COUNTOF(RefreshRates)*/)     { //2 =4
	for (c = 0; c < COUNTOF(BackBufferCounts); c += 1/*LASTOF(BackBufferCounts)*/) { //2 =8
	for (d = 0; d < COUNTOF(MultiSamples);     d += 1/*3*/)                        { //3 =24
	for (e = 0; e < COUNTOF(Intervals);        e += 1/*COUNTOF(Intervals)*/)        { //2 =48
	for (i = 0; i < COUNTOF(Flags) * COUNTOF(PpFlags); i += 1/*3*/);               { //3 =144
	f = (i % COUNTOF(Flags));
	g = (i / COUNTOF(Flags));
	for (h = 0; h < COUNTOF(Stencils);         h += 1/*2*/)                        { //4 =576
		SETCDPARAMS(a,b,c,d,e,f,g,h);
		CDDOIT;
	}
	}
	}
	}
	}
	}
	}
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()


#pragma data_seg()
