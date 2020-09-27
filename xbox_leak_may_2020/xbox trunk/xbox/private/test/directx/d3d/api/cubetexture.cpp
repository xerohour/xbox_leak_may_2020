#include "d3dapi.hpp"
#include "swizzler.h"

LINKME(1)

static D3DFORMAT TextureTypes[] = {
//    D3DFMT_R8G8B8,
    D3DFMT_A8R8G8B8,
    D3DFMT_X8R8G8B8,
    D3DFMT_R5G6B5,
    D3DFMT_X1R5G5B5,
    D3DFMT_A1R5G5B5,
    D3DFMT_A4R4G4B4,
 //   D3DFMT_R3G3B2,
    D3DFMT_A8,
//    D3DFMT_A8R3G3B2,
//    D3DFMT_X4R4G4B4,

//    D3DFMT_A8P8,
    D3DFMT_P8,

    D3DFMT_L8,
    D3DFMT_A8L8,
//    D3DFMT_A4L4,

//    D3DFMT_UYVY,
//    D3DFMT_YUY2,
    D3DFMT_DXT1,
    D3DFMT_DXT2,
///    D3DFMT_DXT3,
///    D3DFMT_DXT4,
    D3DFMT_DXT5
};

static DWORD Usages[] = {
	0, D3DUSAGE_DEPTHSTENCIL, D3DUSAGE_RENDERTARGET, D3DUSAGE_DEPTHSTENCIL | D3DUSAGE_RENDERTARGET
};

static struct {UINT x; UINT y; UINT z;} Sizes[] = {
	{ 4,  4, 4},
	{ 8,  8, 8},
	{16, 16, 16},
	{32, 32, 32},
	{64, 64, 64},
	{ 4, 64, 32}
};

static DWORD Levels[] = {
	0, 1, 2
};

static struct cubestuffstruct {
	D3DFORMAT f;
	UINT bitdepth;
	UINT width;
//	UINT height;
	UINT count;
	UINT expected;
} CubeStuff[] = {
	{  D3DFMT_A8R8G8B8, 32,   64,  0,  7},
	{  D3DFMT_A8R8G8B8, 32,   64,  5,  5},
	{  D3DFMT_A8R8G8B8, 32,  128,  0,  8},
	{  D3DFMT_A8R8G8B8, 32,  128,  8,  8},
	{  D3DFMT_A8R8G8B8, 32,  512, 10, 10},
	{  D3DFMT_A8R8G8B8, 32,    4,  0,  3},
	{  D3DFMT_A8R8G8B8, 32,    2,  0,  2},
	{  D3DFMT_A8R8G8B8, 32,    1,  0,  1},

//	{      D3DFMT_DXT1,  4,   64,  0,  5},
//	{      D3DFMT_DXT1,  4,   64,  5,  5},
//	{      D3DFMT_DXT1,  4,  128,  0,  8},
//	{      D3DFMT_DXT1,  4,  128,  6,  6},
//	{      D3DFMT_DXT1,  4, 1024,  9,  9},
//	{      D3DFMT_DXT1,  4,    4,  0,  1},

	{    D3DFMT_R5G6B5, 16,   64,  5,  5},
	{        D3DFMT_A8,  8,   64,  0,  7},

//	{D3DFMT_LIN_R5G6B5, 16,   64,  0,  1},
//	{D3DFMT_LIN_R5G6B5, 16, 1024,  0,  1},
//	{D3DFMT_LIN_R5G6B5, 16,  128,  0,  1},
//	{D3DFMT_LIN_R5G6B5, 16,  512,  1,  1},
//	{D3DFMT_LIN_R5G6B5, 16, 2048,  0,  1}
};


#pragma data_seg(".d3dapi$test110") 


BEGINTEST(testCreateCubeTexture)
{
	IDirect3DCubeTexture8* t;
	DWORD hr;
	int a, b, c, d;

	xStartVariation(hlog, "Cube: Formats Loop");
	STARTLEAKCHECK();

	for(a = 0; a < COUNTOF(TextureTypes); a++) {
		hr = g_pd3dd8->CreateCubeTexture(64, 0, 0, TextureTypes[a], D3DPOOL_DEFAULT, &t);
		CHECKRESULT(hr == D3D_OK);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "len: %d, Lev: %d, Usage: %d, Format: %d",
				64, 0, 0, TextureTypes[a]);
			break;
		}
		hr = t->Release();
		CHECKRESULT(hr == 0);
	}

	CHECKLEAKS();
	xEndVariation(hlog);
	xStartVariation(hlog, "Cube: Size & Levels Loop");
	STARTLEAKCHECK();

	for(b = 0; b < COUNTOF(Sizes) - 1; b++) {
	for(c = 0; c < COUNTOF(Levels); c++) {
	for(d = 0; d < COUNTOF(Usages); d++) {
		hr = g_pd3dd8->CreateCubeTexture(Sizes[b].x, 
			Levels[c], Usages[d], D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t);
		CHECKRESULT(hr == D3D_OK);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "Len: %d, Lev: %d, Usage: %d, Format: %d",
				Sizes[b].x, Levels[c], Usages[d], D3DFMT_A8R8G8B8);
			break;
		}
		hr = t->Release();
		CHECKRESULT(hr == 0);
	}
	}
	}

	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()


BEGINTEST(testCubeTextureGetDevice)
{
	DWORD hr;
	IDirect3DCubeTexture8* t;
	IDirect3DDevice8* d;

	xStartVariation(hlog, " ");
	STARTLEAKCHECK();
	hr = g_pd3dd8->CreateCubeTexture(64, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t);
	CHECKRESULT(hr == D3D_OK);
	hr = t->GetDevice(&d);
	CHECKRESULT(hr == D3D_OK);
	CHECKRESULT(d == g_pd3dd8);
	d->Release();
	CHECKRESULT(t->Release() == 0);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testCubeTextureGetType)
{
	DWORD hr;
	IDirect3DCubeTexture8* t;

	xStartVariation(hlog, " ");
	STARTLEAKCHECK();
	hr = g_pd3dd8->CreateCubeTexture(64, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t);
	CHECKRESULT(hr == D3D_OK);
	CHECKRESULT(t->GetType() == D3DRTYPE_CUBETEXTURE);
	CHECKRESULT(t->Release() == 0);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testCubeTextureIsBusy)
{
	//#pragma TODO("test IsBusy")
}
ENDTEST()

BEGINTEST(testCubeTextureGetLevelCount)
{
	DWORD hr;
	IDirect3DCubeTexture8* t;
	int i;

	xStartVariation(hlog, " ");
	STARTLEAKCHECK();

	for(i = 0; i < COUNTOF(CubeStuff); i++) {
		hr = g_pd3dd8->CreateCubeTexture(CubeStuff[i].width, CubeStuff[i].count, 0, CubeStuff[i].f, D3DPOOL_DEFAULT, &t);
		CHECKRESULT(hr == D3D_OK);
		CHECKRESULT(t->GetLevelCount() == CubeStuff[i].expected);
		CHECKRESULT(t->Release() == 0);
	}
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()


BEGINTEST(testCubeTextureGetLevelDesc)
{
	DWORD hr;
	IDirect3DCubeTexture8* t;
	D3DSURFACE_DESC d;
	int i;
	unsigned int j;
	UINT w;

	xStartVariation(hlog, " ");
	STARTLEAKCHECK();

	for(i = 0; i < COUNTOF(CubeStuff); i++) {
		hr = g_pd3dd8->CreateCubeTexture(CubeStuff[i].width, CubeStuff[i].count, 0, CubeStuff[i].f, D3DPOOL_DEFAULT, &t);
		w = CubeStuff[i].width; 
		CHECKRESULT(hr == D3D_OK);
		if(WASGOODRESULT()) {
			for(j = 0; j < t->GetLevelCount(); j++) {
				t->GetLevelDesc(j, &d);
				CHECKRESULT(d.Width == w);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "d.Width is %d, should be %d", d.Width, w);
				CHECKRESULT(d.Height == w);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "d.Height is %d, should be %d", d.Height, w);
				CHECKRESULT(d.Size == w * w * CubeStuff[i].bitdepth/8);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "d.Size is %d, should be %d", d.Size, w*w*CubeStuff[i].bitdepth /8);
				CHECKRESULT(d.Format == CubeStuff[i].f);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "d.Format is %d, should be %d", d.Format, CubeStuff[i].f);
				w = (w >> 1) ? (w >> 1) : 1;
			}
			CHECKRESULT(t->Release() == 0);
		}
	}
	CHECKRESULT(d.Type == D3DRTYPE_CUBETEXTURE);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testCubeTextureGetSurfaceLevel)
{
	DWORD hr;
	IDirect3DCubeTexture8* t;
	IDirect3DSurface8* s;
	D3DSURFACE_DESC d;
	int i;
	unsigned int j;
	int f;
	UINT w;

	xStartVariation(hlog, " ");
	STARTLEAKCHECK();

	for(i = 0; i < COUNTOF(CubeStuff); i++) {
		hr = g_pd3dd8->CreateCubeTexture(CubeStuff[i].width, CubeStuff[i].count, 0, CubeStuff[i].f, D3DPOOL_DEFAULT, &t);
		CHECKRESULT(hr == D3D_OK);
		if(WASGOODRESULT()) {
			for(f = D3DCUBEMAP_FACE_POSITIVE_X; f <= D3DCUBEMAP_FACE_NEGATIVE_Z; f++) {
				w = CubeStuff[i].width; 
				for(j = 0; j < t->GetLevelCount(); j++) {
					t->GetCubeMapSurface((D3DCUBEMAP_FACES)f, j, &s);
					s->GetDesc(&d);
					CHECKRESULT(d.Width == w);
					if(WASBADRESULT()) {
						xLog(hlog, XLL_INFO, "d.Width is %d, should be %d", d.Width, w);
					}
					CHECKRESULT(d.Size == w * w * CubeStuff[i].bitdepth/8);
					if(WASBADRESULT()) {
						xLog(hlog, XLL_INFO, "d.Size is %d, should be %d. w=%d, bpp=%d", d.Size, w*w*CubeStuff[i].bitdepth/8, w, CubeStuff[i].bitdepth/8);
					}
					CHECKRESULT(d.Format == CubeStuff[i].f);
					w = (w >> 1) ? (w >> 1) : 1;
					hr = s->Release();
					CHECKRESULT(hr == 0);
				}
			}
			CHECKRESULT(t->Release() == 0);
		}
	}
	CHECKRESULT(d.Type == D3DRTYPE_SURFACE);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

#if 0
	BEGINTEST(testCubeTextureLockAndUnlockRect)
	{
		/*
		This test does the following:
			create huge CubeTexture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
			locks the top layer as RAW_DATA
			sets a few random pixels
			unlocks top layer
			locks top layer (unswizzled)
			checks if the pixels tweaked earlier are still set
			locks layer 3 (512x512), (RAW)
			sets a lot more pixels on top layer
			sets some pixels on layer 3
			unlock layer 3, lock layer 3 as unswizzled
			set more pixels on layer 3
			unlocks top layer
			locks top layer (RAW)
			lock bottom layer (RAW)
			create another CubeTexture (256x512, 1 mip layer, 32bits/pixel == 128k)
			lock second CubeTexture (unswizzled)
			write pixels to second CubeTexture
			set a pixel on bottom layer of first CubeTexture
			unlock bottom layer of first CubeTexture (RAW)
			unlock layer 3 of first CubeTexture(RAW)
			unlock second CubeTexture (unswizzled)
			checks for corruption of pixels set on RAW top layer
			unlock top layer
			lock top layer
			unlock top layer
			delete first CubeTexture
			lock second CubeTexture (RAW)
			check for corrupted pixels of second CubeTexture
			unlock second CubeTexture (RAW)
			delete second CubeTexture

		tests:
			huge CubeTextures, 
			multiple simultaneous unswizzling of CubeTexture levels
			swizzling
			unswizzling
			multiple simultaneous unswizzled CubeTextures
			1x1 mip layers
			non-square CubeTextures
			8bbp and 32bbp lock/unlock
		*/
		
		SWIZZLER2<DWORD> narf (4096, 4096, 0);


		DWORD hr;
		IDirect3DCubeTexture8* t[3];
		RECT r;
		D3DLOCKED_RECT l;
		D3DLOCKED_RECT l2;
		D3DLOCKED_RECT l3;
		D3DLOCKED_RECT l4;
		int i, j;
		int u, v;
		int p;
		bool isbad = false;

		SWIZZLER a (4096, 4096, 0);
		SWIZCOORD coords[32];

		xStartVariation(hlog, " ");
		STARTLEAKCHECK();

	//		create huge CubeTexture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
		hr = g_pd3dd8->CreateCubeTexture(4096, 13, 0, D3DFMT_A8, D3DPOOL_DEFAULT, &t[0]);
		CHECKRESULT(hr == D3D_OK /*Create huge CubeTexture*/);

		r.top = 0;
		r.left = 0;
		r.right = 4095;
		r.bottom = 4095;

	//		locks the top layer as RAW_DATA
		hr = t[0]->LockRect(0, &l, &r, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK /*lock top layer RAW*/);
		CHECKRESULT(l.Pitch == 4096 /*lock top layer RAW*/);
		coords[0].Init(   2,    0,    0, &a);
		coords[1].Init(   3,    0,    0, &a);
		coords[2].Init(4095, 4094,    0, &a);
		coords[3].Init(2047, 2047,    0, &a);

	//		sets a few random pixels
		((BYTE*)l.pBits)[coords[0].c] = 0;
		((BYTE*)l.pBits)[coords[1].c] = 255;
		((BYTE*)l.pBits)[coords[2].c] = 31;
		((BYTE*)l.pBits)[coords[3].c] = 41;

	//		unlocks top layer
		hr = t[0]->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK /*unlock top layer RAW*/);
		
	//		locks top layer (unswizzled)
		hr = t[0]->LockRect(0, &l, &r, 0); 
		CHECKRESULT(hr == D3D_OK /*lock top layer unswizzled*/);
		CHECKRESULT(l.Pitch == 4096 /*lock top layer unswizzled*/);

	//		checks if the pixels tweaked earlier are still set
		if(((BYTE*)l.pBits)[coords[0].i] != 0) isbad = true;
		if(((BYTE*)l.pBits)[coords[1].i] != 255) isbad = true;
		if(((BYTE*)l.pBits)[coords[2].i] != 31) isbad = true;
		if(((BYTE*)l.pBits)[coords[3].i] != 41) isbad = true;
		CHECKRESULT(isbad == false /*swizzle didn't work right*/);
		isbad = false;

	//		locks layer 3 (512x512), (RAW)
		hr = t[0]->LockRect(3, &l2, NULL, D3DLOCK_RAWDATA); //this should be a 512x512 CubeTexture (swizzled)
		CHECKRESULT(hr == D3D_OK /*second mipmap lock*/);

	//		sets a lot more pixels on top layer
		for(j = 0; j < 4096; j += 273) {
			for(i = 0; i < 4096; i += 259) {
				((BYTE*)l.pBits)[i + j * l.Pitch] = i + j;
			}
		}

	//		sets some pixels on layer 3
		for(i = 0; i < l2.Pitch; i++) {
			for(j = 0; j < l2.Pitch; j++) {
				((BYTE*)l2.pBits)[i + j * l2.Pitch] = 1;
			}
		}

	//		unlocks top layer
		hr = t[0]->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK /*swizzled CubeTexture unlock*/);

	//		locks top layer (RAW)
		hr = t[0]->LockRect(0, &l, &r, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK /*lock top layer RAW*/);
		CHECKRESULT(l.Pitch == 4096 /*lock top layer RAW*/);

	//		lock bottom layer (RAW)
		hr = t[0]->LockRect(12, &l3, NULL, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK /*lock bottom layer RAW*/);
		CHECKRESULT(l.Pitch == 1 /*lock bottom layer RAW*/);

	//		create another CubeTexture (256x512, 1 mip layer, 32bits/pixel == 128k)
		hr = g_pd3dd8->CreateCubeTexture(2048, 256, 1, 0, D3DFMT_R8G8B8A8, D3DPOOL_DEFAULT, &t[1]);
		CHECKRESULT(hr == D3D_OK /*create second CubeTexture while first is unlocked*/);
		SWIZZLER b (2048, 256, 0);

	//		lock second CubeTexture (unswizzled)
		hr = t[1]->LockRect(0, &l4, NULL, 0); 
		CHECKRESULT(hr == D3D_OK);
		CHECKRESULT(l.Pitch == 2048 * sizeof(DWORD));

	//		write pixels to second CubeTexture
		coords[10].Init(   0,    0, 0, &b);
		coords[11].Init(   1,    0, 0, &b);
		coords[12].Init(   2,    0, 0, &b);
		coords[13].Init( 767,  255, 0, &b);
		coords[14].Init(2047,  255, 0, &b);
		coords[15].Init(2046,  254, 0, &b);
		((DWORD*)l4.pBits)[coords[10].i] = 0x12345678;
		((DWORD*)l4.pBits)[coords[11].i] = 0x11111111;
		((DWORD*)l4.pBits)[coords[12].i] = 0x33445566;
		((DWORD*)l4.pBits)[coords[13].i] = 0x91807f6e;
		((DWORD*)l4.pBits)[coords[14].i] = 0xfffefdfc;
		((DWORD*)l4.pBits)[coords[15].i] = 0x01020304;

	//		set a pixel on bottom layer of first CubeTexture
		*((BYTE*)l3.pBits) = 255;

	//		unlock bottom layer of first CubeTexture(RAW)
		hr = t[0]->UnlockRect(12);
		CHECKRESULT(hr == D3D_OK /*bottom-layer mipmap unlock*/);

	//		unlock layer 3 of first CubeTexture(RAW)
		hr = t[0]->UnlockRect(3);
		CHECKRESULT(hr == D3D_OK /*layer 3 mipmap unlock*/);

	//		unlock second CubeTexture (unswizzled)
		hr = t[1]->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK /*second CubeTexture unlock*/);

	//		checks for corruption of pixels set on RAW top layer
		SWIZNUM big_u_259 = a.Convert(259, UCOORD);
		SWIZNUM big_v_273 = a.Convert(273, VCOORD);
		a.Set(0,UCOORD);
		for(j = 0; j < 2096; j += 273) {
			a.Set(0,VCOORD);
			for(i = 0; i < 4096; i += 259) {
				if(((BYTE*)l.pBits)[a.Get2()] != i + j) {isbad = true; break;}
				a.Add(big_u_259, UCOORD);
			}
			a.Add(big_v_273, VCOORD);
		}
		CHECKRESULT(isbad == false /*unswizzle of top layer didn't work right*/ );
		isbad = false;

	//		unlock top layer
		hr = t[0]->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK /*unlock top layer*/);

	//		locks top layer (unswizzled)
		hr = t[0]->LockRect(0, &l, &r, 0); 
		CHECKRESULT(hr == D3D_OK /*second unswizzling of top layer*/);
		CHECKRESULT(l.Pitch == 4096 /*lock top layer of first CubeTexture*/);

	//		unlock top layer
		hr = t[0]->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK /*second reswizzling of top layer*/);

	//		delete first CubeTexture
		hr = t[0]->Release();
		CHECKRESULT(hr == 0 /*release first CubeTexture*/);

	//		lock second CubeTexture (RAW)
		hr = t[1]->LockRect(0, &l4, NULL, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK /*lock second CubeTexture, RAW*/);
		CHECKRESULT(l.Pitch == 2048 * sizeof(DWORD) /*second CubeTexture RAW*/);

	//		check for corrupted pixels of second CubeTexture
		if(((DWORD*)l4.pBits)[coords[10].c] != 0x12345678) isbad = true;
		if(((DWORD*)l4.pBits)[coords[11].c] != 0x11111111) isbad = true;
		if(((DWORD*)l4.pBits)[coords[12].c] != 0x33445566) isbad = true;
		if(((DWORD*)l4.pBits)[coords[13].c] != 0x91807f6e) isbad = true;
		if(((DWORD*)l4.pBits)[coords[14].c] != 0xfffefdfc) isbad = true;
		if(((DWORD*)l4.pBits)[coords[15].c] != 0x01020304) isbad = true;
		CHECKRESULT(isbad == false /*corrupted pixels of second CubeTexture*/);

		//		unlock second CubeTexture (RAW)
		hr = t[1]->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK /*second CubeTexture unlock*/);

	//		delete second CubeTexture
		hr = t[1]->Release();
		CHECKRESULT(hr == 0);

	//check for leaks
		CHECKLEAKS();
		xEndVariation(hlog);
	}
	ENDTEST()



	BEGINTEST(testCubeTextureUpdateCubeTexture)
	{
		/*
		This test does the following:
			create huge CubeTexture map (4096x4096x8bit), full mips (22.3 mb)
			write random pixels to first CubeTexture (multiple layers)
			create second huge CubeTexture map (4096x4096x8bit), full mips
			use UpdateCubeTexture to copy first CubeTexture to second
			delete first CubeTexture
			check random pixels in second CubeTexture (unswizzled)
			create 256x256 CubeTexture map, full mips
			copy lower mip layers from CubeTexture2 to CubeTexture3
			check CubeTexture3
			change pixels in CubeTexture2
			copy lower mip layers from CubeTexture2 to CubeTexture3
			check CubeTexture3
			delete CubeTexture2, CubeTexture3

		this checks:
			big CubeTexture
			full mip copy
			8-bit
			partial mip copy
			square CubeTexture
			multiple copy from same source
		*/
		DWORD hr;

		IDirect3DCubeTexture8* t[3];
		RECT r;
		D3DLOCKED_RECT l;

		SWIZZLER a (4096, 4096, 0);
		struct COORD {
			DWORD u;
			DWORD v;
			int layer;
		} coords [] = {
			{0, 0, 0},
			{4095, 4095, 0},
			{2048, 2047, 0},
			{1022, 34, 2},
			{100, 4, 4},
			{32, 32, 5},
			{0, 15, 8},
			{6, 2, 9},
			{2, 3, 10},
			{0, 1, 11},
			{1, 0, 11},
			{0, 0, 12},
		};
		int i;
		bool isbad = false;

		xStartVariation(hlog, "CubeTexture UpdateCubeTexture");
		STARTLEAKCHECK();

	//		create huge CubeTexture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
		hr = g_pd3dd8->CreateCubeTexture(4096, 4096, 13, 0, D3DFMT_A8, D3DPOOL_DEFAULT, &t[0]);
		CHECKRESULT(hr == D3D_OK /*Create huge CubeTexture*/);

		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[0]->LockRect(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//set byte
			((BYTE*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] = (BYTE)i;

		//		unlock layer
			hr = t[0]->UnlockRect(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);

		}

	//		create second huge CubeTexture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
		hr = g_pd3dd8->CreateCubeTexture(4096, 4096, 13, 0, D3DFMT_A8, D3DPOOL_DEFAULT, &t[1]);
		CHECKRESULT(hr == D3D_OK /*Create second huge CubeTexture*/);

	//		use UpdateCubeTexture to copy first CubeTexture to second
		hr = g_pd3dd8->UpdateCubeTexture(t[0], t[1]);
		CHECKRESULT(hr == D3D_OK /*Update on huge CubeTexture*/);

	//		delete first CubeTexture
		hr = t[0]->Release();
		CHECKRESULT(hr == 0 /*release first huge CubeTexture*/);

	//		check bytes in second CubeTexture
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[1]->LockRect(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//compare byte
			if(((BYTE*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != (BYTE)i) isbad = true;

		//		unlock layer
			hr = t[1]->UnlockRect(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateCubeTexture*/);
		isbad = false;

	//		create smaller CubeTexture (256x256, full mips, 8bits/pixel), 
		hr = g_pd3dd8->CreateCubeTexture(256, 256, 0, 0, D3DFMT_A8, D3DPOOL_DEFAULT, &t[2]);
		CHECKRESULT(hr == D3D_OK /*Create 256x256 CubeTexture*/);

	//		use UpdateCubeTexture to copy second CubeTexture to third
		hr = g_pd3dd8->UpdateCubeTexture(t[1], t[2]);
		CHECKRESULT(hr == D3D_OK /*Update huge->small*/);
		
	//		check bytes in third CubeTexture
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			if (i >= 4) {
				hr = t[1]->LockRect(coords[i].layer -4, &l, &r, 0); 
				CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
		
			//compare byte
			if(((BYTE*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != i) isbad = true;
		
			//		unlock layer
				hr = t[1]->UnlockRect(coords[i].layer -4);
				CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
			}
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateCubeTexture to smaller CubeTexture*/);
		isbad = false;

		coords[ 0].u = 255; coords[ 0].v = 255; coords[ 0].layer = 4;
		coords[ 0].u = 254; coords[ 0].v = 255; coords[ 0].layer = 4;
		coords[ 0].u = 185; coords[ 0].v =  25; coords[ 0].layer = 4;
		coords[ 0].u = 120; coords[ 0].v =   0; coords[ 0].layer = 5;
		coords[ 0].u =   0; coords[ 0].v =   0; coords[ 0].layer = 5;
		coords[ 0].u =  63; coords[ 0].v =  63; coords[ 0].layer = 6;
		coords[ 0].u =   7; coords[ 0].v =   0; coords[ 0].layer = 9;
		coords[ 0].u =   0; coords[ 0].v =   3; coords[ 0].layer =10;
		coords[ 0].u =   1; coords[ 0].v =   1; coords[ 0].layer =11;
		coords[ 0].u = 125; coords[ 0].v = 128; coords[ 0].layer = 4;
		coords[ 0].u =   0; coords[ 0].v =   0; coords[ 0].layer =12;
		coords[ 0].u =  30; coords[ 0].v =  31; coords[ 0].layer = 7;

		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[1]->LockRect(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//set byte
			((BYTE*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] = -i;

		//		unlock layer
			hr = t[1]->UnlockRect(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
		}

		//		use UpdateCubeTexture to copy second CubeTexture to third
		hr = g_pd3dd8->UpdateCubeTexture(t[1], t[2]);
		CHECKRESULT(hr == D3D_OK /*Update huge->small again*/);

	//		check bytes in third CubeTexture again
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			if (i >= 4) {
				hr = t[1]->LockRect(coords[i].layer -4, &l, &r, 0); 
				CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
		
			//compare byte
			if(((BYTE*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != -i) isbad = true;
		
			//		unlock layer
				hr = t[1]->UnlockRect(coords[i].layer -4);
				CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
			}
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateCubeTexture to smaller CubeTexture*/);
		isbad = false;


		//		delete third CubeTexture
		hr = t[2]->Release();
		CHECKRESULT(hr == 0 /*release smaller CubeTexture*/);

	//		delete second CubeTexture
		hr = t[1]->Release();
		CHECKRESULT(hr == 0 /*release second big CubeTexture*/);

		CHECKLEAKS();
		xEndVariation(hlog);
	}
	ENDTEST()

	BEGINTEST(testCubeTextureUpdateCubeTextureNonSquare)
	{
		/*
		This test does the following:
			create huge CubeTexture map (4096x1024x16bit), full mips 
			write random pixels to first CubeTexture (multiple layers)
			create second huge CubeTexture map (4096x1024x16it), full mips
			use UpdateCubeTexture to copy first CubeTexture to second
			delete first CubeTexture
			check random pixels in second CubeTexture (unswizzled)
			create 256x64x16bit CubeTexture map, full mips
			copy lower mip layers from CubeTexture2 to CubeTexture3
			check CubeTexture3
			change pixels in CubeTexture2
			copy lower mip layers from CubeTexture2 to CubeTexture3
			check CubeTexture3
			delete CubeTexture2, CubeTexture3

		this checks:
			big CubeTexture
			full mip copy
			16-bit
			partial mip copy
			square CubeTexture
			multiple copy from same source
		*/
		DWORD hr;

		IDirect3DCubeTexture8* t[3];
		RECT r;
		D3DLOCKED_RECT l;

		SWIZZLER a (4096, 1024, 0);
		struct COORD {
			DWORD u;
			DWORD v;
			int layer;
		} coords [] = {
			{0, 0, 0},
			{4095, 1023, 0},
			{2048, 512, 0},
			{1022, 34, 2},
			{100, 4, 4},
			{32, 15, 5},
			{0,  3, 8},
			{6, 1, 9},
			{2, 0, 10},
			{0, 0, 11},
			{1, 0, 11},
			{0, 0, 12},
		};
		int i;
		bool isbad = false;

		xStartVariation(hlog, "CubeTexture UpdateCubeTexture");
		STARTLEAKCHECK();

	//		create huge CubeTexture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
		hr = g_pd3dd8->CreateCubeTexture(4096, 1024, 13, 0, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &t[0]);
		CHECKRESULT(hr == D3D_OK /*Create huge CubeTexture*/);

		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[0]->LockRect(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//set byte
			((WORD*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] = (WORD)i;

		//		unlock layer
			hr = t[0]->UnlockRect(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);

		}

	//		create second huge CubeTexture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
		hr = g_pd3dd8->CreateCubeTexture(4096, 1024, 13, 0, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &t[1]);
		CHECKRESULT(hr == D3D_OK /*Create second huge CubeTexture*/);

	//		use UpdateCubeTexture to copy first CubeTexture to second
		hr = g_pd3dd8->UpdateCubeTexture(t[0], t[1]);
		CHECKRESULT(hr == D3D_OK /*Update on huge CubeTexture*/);

	//		delete first CubeTexture
		hr = t[0]->Release();
		CHECKRESULT(hr == 0 /*release first huge CubeTexture*/);

	//		check bytes in second CubeTexture
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[1]->LockRect(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//compare byte
			if(((WORD*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != (WORD)i) isbad = true;

		//		unlock layer
			hr = t[1]->UnlockRect(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateCubeTexture*/);
		isbad = false;

	//		create smaller CubeTexture (256x256, full mips, 8bits/pixel), 
		hr = g_pd3dd8->CreateCubeTexture(256, 64, 0, 0, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &t[2]);
		CHECKRESULT(hr == D3D_OK /*Create 256x256 CubeTexture*/);

	//		use UpdateCubeTexture to copy second CubeTexture to third
		hr = g_pd3dd8->UpdateCubeTexture(t[1], t[2]);
		CHECKRESULT(hr == D3D_OK /*Update huge->small*/);
		
	//		check bytes in third CubeTexture
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			if (i >= 4) {
				hr = t[1]->LockRect(coords[i].layer -4, &l, &r, 0); 
				CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
		
			//compare byte
			if(((WORD*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != i) isbad = true;
		
			//		unlock layer
				hr = t[1]->UnlockRect(coords[i].layer -4);
				CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
			}
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateCubeTexture to smaller CubeTexture*/);
		isbad = false;

		coords[ 0].u = 255; coords[ 0].v =  63; coords[ 0].layer = 4;
		coords[ 0].u = 254; coords[ 0].v =  63; coords[ 0].layer = 4;
		coords[ 0].u = 185; coords[ 0].v =  25; coords[ 0].layer = 4;
		coords[ 0].u = 120; coords[ 0].v =   0; coords[ 0].layer = 5;
		coords[ 0].u =   0; coords[ 0].v =   0; coords[ 0].layer = 5;
		coords[ 0].u =  63; coords[ 0].v =  15; coords[ 0].layer = 6;
		coords[ 0].u =   7; coords[ 0].v =   0; coords[ 0].layer = 9;
		coords[ 0].u =   0; coords[ 0].v =   3; coords[ 0].layer =10;
		coords[ 0].u =   1; coords[ 0].v =   1; coords[ 0].layer =11;
		coords[ 0].u = 125; coords[ 0].v =  32; coords[ 0].layer = 4;
		coords[ 0].u =   0; coords[ 0].v =   0; coords[ 0].layer =12;
		coords[ 0].u =  30; coords[ 0].v =   5; coords[ 0].layer = 7;

		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[1]->LockRect(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//set byte
			((WORD*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] = (WORD)(-i);

		//		unlock layer
			hr = t[1]->UnlockRect(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
		}

		//		use UpdateCubeTexture to copy second CubeTexture to third
		hr = g_pd3dd8->UpdateCubeTexture(t[1], t[2]);
		CHECKRESULT(hr == D3D_OK /*Update huge->small again*/);

	//		check bytes in third CubeTexture again
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			if (i >= 4) {
				hr = t[1]->LockRect(coords[i].layer -4, &l, &r, 0); 
				CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
		
			//compare byte
			if(((WORD*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != (WORD)(-i)) isbad = true;
		
			//		unlock layer
				hr = t[1]->UnlockRect(coords[i].layer -4);
				CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
			}
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateCubeTexture to smaller CubeTexture*/);
		isbad = false;


		//		delete third CubeTexture
		hr = t[2]->Release();
		CHECKRESULT(hr == 0 /*release smaller CubeTexture*/);

	//		delete second CubeTexture
		hr = t[1]->Release();
		CHECKRESULT(hr == 0 /*release second big CubeTexture*/);

		CHECKLEAKS();
		xEndVariation(hlog);
	}
	ENDTEST()

	BEGINTEST(testCubeTextureUpdateCubeTextureNonSquare2)
	{
		/*
		This test does the following:
			create huge CubeTexture map (16x2048x32bit), 6 mip layers 
			write random pixels to first CubeTexture (multiple layers)
			create second huge CubeTexture map (2048x16x32it), 6 mip layers
			use UpdateCubeTexture to copy first CubeTexture to second
			delete first CubeTexture
			check random pixels in second CubeTexture (unswizzled)
			create 1x64 CubeTexture map, 1 layer
			copy lowest mip layers from CubeTexture2 to CubeTexture3
			check CubeTexture3
			change pixels in CubeTexture2
			copy lowest mip layer from CubeTexture2 to CubeTexture3
			check CubeTexture3
			delete CubeTexture2, CubeTexture3

		this checks:
			big CubeTexture
			32-bit
			1-layer mip copy
			non-square CubeTexture
			multiple copy from same source
		*/
		
		DWORD hr;

		IDirect3DCubeTexture8* t[3];
		RECT r;
		D3DLOCKED_RECT l;

		SWIZZLER a (16, 2048, 0);
		struct COORD {
			DWORD u;
			DWORD v;
			int layer;
		} coords [] = {
			{0, 0, 0},
			{15, 2047, 0},
			{7, 1821, 0},
			{5, 1000, 1},
			{1, 34, 2},
			{0, 4, 4},
			{0, 17, 5},
		};
		int i;
		bool isbad = false;

		xStartVariation(hlog, "CubeTexture UpdateCubeTexture");
		STARTLEAKCHECK();

	//		create huge CubeTexture (16x1024, 6 mip layers, 32bits/pixel), 
		hr = g_pd3dd8->CreateCubeTexture(16, 2048, 6, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t[0]);
		CHECKRESULT(hr == D3D_OK /*Create huge CubeTexture*/);

		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[0]->LockRect(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//set byte
			((DWORD*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] = (DWORD)i;

		//		unlock layer
			hr = t[0]->UnlockRect(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);

		}

	//		create second huge CubeTexture (4096x4096, 6 mip layers, 32bits/pixel), 
		hr = g_pd3dd8->CreateCubeTexture(16, 2048, 6, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t[1]);
		CHECKRESULT(hr == D3D_OK /*Create second huge CubeTexture*/);

	//		use UpdateCubeTexture to copy first CubeTexture to second
		hr = g_pd3dd8->UpdateCubeTexture(t[0], t[1]);
		CHECKRESULT(hr == D3D_OK /*Update on huge CubeTexture*/);

	//		delete first CubeTexture
		hr = t[0]->Release();
		CHECKRESULT(hr == 0 /*release first huge CubeTexture*/);

	//		check bytes in second CubeTexture
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[1]->LockRect(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//compare byte
			if(((DWORD*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != (DWORD)i) isbad = true;

		//		unlock layer
			hr = t[1]->UnlockRect(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateCubeTexture*/);
		isbad = false;

	//		create smaller CubeTexture (256x256, 1 layer, 32bits/pixel), 
		hr = g_pd3dd8->CreateCubeTexture(1, 64, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t[2]);
		CHECKRESULT(hr == D3D_OK /*Create 256x256 CubeTexture*/);

	//		use UpdateCubeTexture to copy second CubeTexture to third
		hr = g_pd3dd8->UpdateCubeTexture(t[1], t[2]);
		CHECKRESULT(hr == D3D_OK /*Update huge->small*/);
		
	//		check bytes in third CubeTexture
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			if (i >= 4) {
				hr = t[1]->LockRect(coords[i].layer -4, &l, &r, 0); 
				CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
		
			//compare byte
			if(((BYTE*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != i) isbad = true;
		
			//		unlock layer
				hr = t[1]->UnlockRect(coords[i].layer -4);
				CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
			}
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateCubeTexture to smaller CubeTexture*/);
		isbad = false;

		coords[ 0].u = 0; coords[ 0].v =  63; coords[ 0].layer = 6;
		coords[ 1].u = 0; coords[ 1].v =  21; coords[ 1].layer = 6;
		coords[ 2].u = 0; coords[ 2].v =   0; coords[ 2].layer = 6;
		coords[ 3].u = 0; coords[ 3].v =  13; coords[ 3].layer = 6;
		coords[ 4].u = 0; coords[ 4].v =  11; coords[ 4].layer = 6;
		coords[ 5].u = 0; coords[ 5].v =  62; coords[ 5].layer = 6;
		coords[ 6].u = 0; coords[ 6].v =  14; coords[ 6].layer = 6;

		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[1]->LockRect(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//set byte
			((DWORD*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] = (DWORD)(2*i);

		//		unlock layer
			hr = t[1]->UnlockRect(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
		}

		//		use UpdateCubeTexture to copy second CubeTexture to third
		hr = g_pd3dd8->UpdateCubeTexture(t[1], t[2]);
		CHECKRESULT(hr == D3D_OK /*Update huge->small again*/);

	//		check bytes in third CubeTexture again
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			if (i >= 4) {
				hr = t[1]->LockRect(coords[i].layer -6, &l, &r, 0); 
				CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
		
			//compare byte
			if(((BYTE*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != DWORD(2*i)) isbad = true;
		
			//		unlock layer
				hr = t[1]->UnlockRect(coords[i].layer -6);
				CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
			}
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateCubeTexture to smaller CubeTexture*/);
		isbad = false;


		//		delete third CubeTexture
		hr = t[2]->Release();
		CHECKRESULT(hr == 0 /*release smaller CubeTexture*/);

	//		delete second CubeTexture
		hr = t[1]->Release();
		CHECKRESULT(hr == 0 /*release second big CubeTexture*/);

		CHECKLEAKS();
		xEndVariation(hlog);
	}
	ENDTEST()

#endif //0

#pragma data_seg()
