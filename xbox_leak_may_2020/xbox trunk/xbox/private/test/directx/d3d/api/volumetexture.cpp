#include "d3dapi.hpp"
#include "swizzler.h"

LINKME(9)

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
///  D3DFMT_DXT2,
  D3DFMT_DXT3,
  D3DFMT_DXT4,
///  D3DFMT_DXT5
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

static struct volumestuffstruct {
	D3DFORMAT f;
	UINT bitdepth;
	UINT width;
	UINT height;
	UINT depth;
	UINT count;
	UINT expected;
} VolumeStuff[] = {
	{  D3DFMT_A8R8G8B8, 32, 128,  128,  512,  0, 10}, //38mb
	{  D3DFMT_A8R8G8B8, 32, 512,  512,   32,  1,  1}, //32mb
	{  D3DFMT_A8R8G8B8, 32, 512,  512,    8,  0, 10},
	{  D3DFMT_A8R8G8B8, 32, 128,  128,  128,  4,  4},
	{  D3DFMT_A8R8G8B8, 32,   1,  512,    4,  0, 10},
	{  D3DFMT_A8R8G8B8, 32,   4,    4,    4,  0,  3},
	{  D3DFMT_A8R8G8B8, 32,   2,    2,    2,  0,  2},
	{  D3DFMT_A8R8G8B8, 32,   1,    1,    1,  0,  1},

	{    D3DFMT_R5G6B5, 16,  64,   64,   16,  5,  5},
	{    D3DFMT_R5G6B5, 16, 512,  128,  256,  0, 10},

	{        D3DFMT_A8,  8,   1,    1,    1,  1,  1},
	{        D3DFMT_A8,  8,   1,   64,    1,  0,  7},
	{        D3DFMT_A8,  8, 256,  256,  512,  2,  2},
	
};

#pragma data_seg(".d3dapi$test120") 



BEGINTEST(testCreateVolumeTexture)
{
	IDirect3DVolumeTexture8* t;
	DWORD hr;
	int a, b, c, d;

	xStartVariation(hlog, "CreateTexture, Formats Loop");
	STARTLEAKCHECK();

	for(a = 0; a < COUNTOF(TextureTypes); a++) {
		hr = g_pd3dd8->CreateVolumeTexture(64, 64, 64, 0, 0, TextureTypes[a], D3DPOOL_DEFAULT, &t);
		CHECKRESULT(hr == D3D_OK);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "x: %d, y: %d, z: %d, Lev: %d, Usage: %d, Format: %d",
				64, 64, 64, 0, 0, TextureTypes[a]);
			break;
		}
		hr = t->Release();
		CHECKRESULT(hr == 0);
	}

	CHECKLEAKS();
	xEndVariation(hlog);
	xStartVariation(hlog, "CreateVolumeTexture, Size & Levels Loop");
	STARTLEAKCHECK();

	for(b = 0; b < COUNTOF(Sizes); b++) {
	for(c = 0; c < COUNTOF(Levels); c++) {
	for(d = 0; d < COUNTOF(Usages); d++) {
		hr = g_pd3dd8->CreateVolumeTexture(Sizes[b].x, Sizes[b].y, Sizes[b].z, 
			Levels[c], Usages[d], D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t);
		CHECKRESULT(hr == D3D_OK);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "x: %d, y: %d, z: %d, Lev: %d, Usage: %d, Format: %d",
				Sizes[b].x, Sizes[b].y,	Sizes[b].z,	Levels[c], Usages[d], D3DFMT_A8R8G8B8);
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


BEGINTEST(testVolumeTextureGetDevice)
{
	DWORD hr;
	IDirect3DVolumeTexture8* t;
	IDirect3DDevice8* d;

	xStartVariation(hlog, " ");
	STARTLEAKCHECK();
	hr = g_pd3dd8->CreateVolumeTexture(64, 64, 64, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t);
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

BEGINTEST(testVolumeTextureGetType)
{
	DWORD hr;
	IDirect3DVolumeTexture8* t;

	xStartVariation(hlog, " ");
	STARTLEAKCHECK();
	hr = g_pd3dd8->CreateVolumeTexture(64, 64, 64, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t);
	CHECKRESULT(hr == D3D_OK);
	CHECKRESULT(t->GetType() == D3DRTYPE_VOLUMETEXTURE);
	CHECKRESULT(t->Release() == 0);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testVolumeTextureIsBusy)
{
	//#pragma TODO("test IsBusy")
}
ENDTEST()

BEGINTEST(testVolumeTextureGetLevelCount)
{
	DWORD hr;
	IDirect3DVolumeTexture8* t;
	int i;

	xStartVariation(hlog, " ");
	STARTLEAKCHECK();

	for(i = 0; i < COUNTOF(VolumeStuff); i++) {
		hr = g_pd3dd8->CreateVolumeTexture(VolumeStuff[i].width, VolumeStuff[i].height, VolumeStuff[i].depth, VolumeStuff[i].count, 0, VolumeStuff[i].f, D3DPOOL_DEFAULT, &t);
		CHECKRESULT(hr == D3D_OK);
		if(WASGOODRESULT()) {
			CHECKRESULT(t->GetLevelCount() == VolumeStuff[i].expected);
			if(WASBADRESULT()) xLog(hlog, XLL_INFO, "GetLevelCount returned %d, should be %d",
				t->GetLevelCount(), VolumeStuff[i].expected);
			CHECKRESULT(t->Release() == 0);
		}
	}
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()


BEGINTEST(testVolumeTextureGetLevelDesc)
{
	DWORD hr;
	IDirect3DVolumeTexture8* t;
	D3DVOLUME_DESC dd;
	int i;
	unsigned int j;
	UINT w, h, d;

	xStartVariation(hlog, " ");
	STARTLEAKCHECK();

	for(i = 0; i < COUNTOF(VolumeStuff); i++) {
		hr = g_pd3dd8->CreateVolumeTexture(VolumeStuff[i].width, VolumeStuff[i].height, VolumeStuff[i].depth, VolumeStuff[i].count, 0, VolumeStuff[i].f, D3DPOOL_DEFAULT, &t);
		w = VolumeStuff[i].width; 
		h = VolumeStuff[i].height;
		d = VolumeStuff[i].depth;
		CHECKRESULT(hr == D3D_OK);
		if(WASGOODRESULT()) {
	//		CHECKRESULT(t->GetLevelCount() == VolumeStuff[i].expected);
			for(j = 0; j < t->GetLevelCount(); j++) {
				t->GetLevelDesc(j, &dd);
				CHECKRESULT(dd.Width == w);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "width was: %d, should be %d", dd.Width, w);
				CHECKRESULT(dd.Height == h);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "height was: %d, should be %d", dd.Height, h);
				CHECKRESULT(dd.Depth == d);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "depth was: %d, should be %d", dd.Depth, d);
				CHECKRESULT(dd.Size == w * h * d * VolumeStuff[i].bitdepth/8);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "size was: %d, should be %d", dd.Size, w * h * d * VolumeStuff[i].bitdepth/8);
				CHECKRESULT(dd.Format == VolumeStuff[i].f);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "width was: %d, should be %d", dd.Format, VolumeStuff[i].f);
				w = (w >> 1) ? (w >> 1) : 1;
				h = (h >> 1) ? (h >> 1) : 1;
				d = (d >> 1) ? (d >> 1) : 1;
			}
			CHECKRESULT(t->Release() == 0);
		}
	}
	CHECKRESULT(dd.Type == D3DRTYPE_VOLUMETEXTURE);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testVolumeTextureGetVolumeLevel)
{
	DWORD hr;
	IDirect3DVolumeTexture8* t;
	IDirect3DVolume8* s;
	D3DVOLUME_DESC dd;
	int i;
	unsigned int j;
	UINT w, h, d;

	xStartVariation(hlog, " ");
	STARTLEAKCHECK();

	for(i = 0; i < COUNTOF(VolumeStuff); i++) {
		hr = g_pd3dd8->CreateVolumeTexture(VolumeStuff[i].width, VolumeStuff[i].height, VolumeStuff[i].depth, VolumeStuff[i].count, 0, VolumeStuff[i].f, D3DPOOL_DEFAULT, &t);
		w = VolumeStuff[i].width; 
		h = VolumeStuff[i].height;
		d = VolumeStuff[i].depth;
		CHECKRESULT(hr == D3D_OK);
		if(WASGOODRESULT()) {
			for(j = 0; j < t->GetLevelCount(); j++) {
				t->GetVolumeLevel(j, &s);
				s->GetDesc(&dd);
				CHECKRESULT(dd.Width == w);	// //basically already tested above
				CHECKRESULT(dd.Height == h);
				CHECKRESULT(dd.Size == w * h * d * VolumeStuff[i].bitdepth / 8);
				if(WASBADRESULT()) {
					xLog(hlog, XLL_INFO, "size was: %d, should have been: %d. i%d, w%d, h%d, d%d", 
						dd.Size, w * h * d * VolumeStuff[i].bitdepth / 8, i, w, h, d);
				}
				CHECKRESULT(dd.Format == VolumeStuff[i].f);
				w = (w >> 1) ? (w >> 1) : 1;
				h = (h >> 1) ? (h >> 1) : 1;
				d = (d >> 1) ? (d >> 1) : 1;
				hr = s->Release();
				CHECKRESULT(hr == 0);
			}
			CHECKRESULT(t->Release() == 0);
		}
	}
	CHECKRESULT(dd.Type == D3DRTYPE_VOLUME);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

#if 0
	BEGINTEST(testVolumeTextureLockAndUnlockBox)
	{
		/*
		This test does the following:
			create huge volume (256x256x256, 9 mip layers, 8bits/pixel == 19 mb), 
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
			create another texture (256x512, 1 mip layer, 32bits/pixel == 128k)
			lock second texture (unswizzled)
			write pixels to second texture
			set a pixel on bottom layer of first texture
			unlock bottom layer of first texture (RAW)
			unlock layer 3 of first texture(RAW)
			unlock second texture (unswizzled)
			checks for corruption of pixels set on RAW top layer
			unlock top layer
			lock top layer
			unlock top layer
			delete first texture
			lock second texture (RAW)
			check for corrupted pixels of second texture
			unlock second texture (RAW)
			delete second texture

		tests:
			huge textures, 
			multiple simultaneous unswizzling of texture levels
			swizzling
			unswizzling
			multiple simultaneous unswizzled textures
			1x1 mip layers
			non-square textures
			8bbp and 32bbp lock/unlock
		*/
		


		DWORD hr;
		IDirect3DVolumeTexture8* t[3] = {0,0,0};
		D3DBOX r;
		D3DLOCKED_BOX l;
		D3DLOCKED_BOX l2;
		D3DLOCKED_BOX l3;
		D3DLOCKED_BOX l4;
		int i, j, k;
		int u, v;
		int p;
		bool isbad = false;

		SWIZZLER a (256, 256, 256);
		SWIZZLER b (256, 512, 2);
		SWIZCOORD coords[32];

		SWIZNUM big_u = a.Convert(31, UCOORD);
		SWIZNUM big_v = a.Convert(19, VCOORD);
		SWIZNUM big_s = a.Convert(32, VCOORD);



		xStartVariation(hlog, "");
		STARTLEAKCHECK();

	//		create huge texture (256x256x256, 9 mip layers, 8bits/pixel == 19.1 mb), 
		hr = g_pd3dd8->CreateVolumeTexture(256, 256, 256, 9, 0, D3DFMT_A8, D3DPOOL_DEFAULT, &t[0]);
		CHECKRESULT(hr == D3D_OK /*Create huge texture*/);
		if(WASBADRESULT()) {
			goto cleanup;
		}

		r.Top = 0;
		r.Left = 0;
		r.Front = 0;
		r.Right = 255;
		r.Bottom = 255;
		r.Back = 255;

	//		locks the top layer as RAW_DATA
		hr = t[0]->LockBox(0, &l, &r, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK /*lock top layer RAW*/);
		CHECKRESULT(l.RowPitch == 256 /*lock top layer RAW*/);
		CHECKRESULT(l.SlicePitch == 256*256 /*lock top layer RAW*/);
		coords[0].Init(  2,   0,   0, &a);
		coords[1].Init(  3,   0,   0, &a);
		coords[2].Init(255, 254,   0, &a);
		coords[3].Init(255, 255,   0, &a);

	//		sets a few random pixels
		((BYTE*)l.pBits)[coords[0].c] = 0;
		((BYTE*)l.pBits)[coords[1].c] = 255;
		((BYTE*)l.pBits)[coords[2].c] = 31;
		((BYTE*)l.pBits)[coords[3].c] = 41;

	//		unlocks top layer
		hr = t[0]->UnlockBox(0);
		CHECKRESULT(hr == D3D_OK /*unlock top layer RAW*/);
		
	//		locks top layer (unswizzled)
		hr = t[0]->LockBox(0, &l, &r, 0); 
		CHECKRESULT(hr == D3D_OK /*lock top layer unswizzled*/);
		CHECKRESULT(l.RowPitch == 256 /*lock top layer unswizzled*/);
		CHECKRESULT(l.SlicePitch == 256 * 256 /*lock top layer unswizzled*/);

	//		checks if the pixels tweaked earlier are still set
		if(((BYTE*)l.pBits)[coords[0].i] != 0) isbad = true;
		if(((BYTE*)l.pBits)[coords[1].i] != 255) isbad = true;
		if(((BYTE*)l.pBits)[coords[2].i] != 31) isbad = true;
		if(((BYTE*)l.pBits)[coords[3].i] != 41) isbad = true;
		CHECKRESULT(isbad == false /*swizzle didn't work right*/);
		isbad = false;

	//		locks layer 3 (512x512), (RAW)
		hr = t[0]->LockBox(3, &l2, NULL, D3DLOCK_RAWDATA); //this should be a 512x512 texture (swizzled)
		CHECKRESULT(hr == D3D_OK /*second mipmap lock*/);

	//		sets a lot more pixels on top layer
		for(k = 0; k < 256; k += 32) {
			for(j = 0; j < 256; j += 19) {
				for(i = 0; i < 256; i += 31) {
					((BYTE*)l.pBits)[i + j * l.RowPitch + k * l.SlicePitch] = i + j * (k - 1);
				}
			}
		}

	//		sets some pixels on layer 3
		for(k = 0; k < 32; k++) {
			for(j = 0; j < 32; j++) {
				for(i = 0; i < 32; i++) {
					((BYTE*)l2.pBits)[i + j * l2.RowPitch + k * l2.SlicePitch] = 1;
				}
			}
		}

	//		unlocks top layer
		hr = t[0]->UnlockBox(0);
		CHECKRESULT(hr == D3D_OK /*swizzled texture unlock*/);

	//		locks top layer (RAW)
		hr = t[0]->LockBox(0, &l, &r, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK /*lock top layer RAW*/);
		CHECKRESULT(l.RowPitch == 256 /*lock top layer RAW*/);
		CHECKRESULT(l.SlicePitch == 65536 /*lock top layer RAW*/);

	//		lock bottom layer (RAW)
		hr = t[0]->LockBox(8, &l3, NULL, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK /*lock bottom layer RAW*/);
		CHECKRESULT(l3.RowPitch == 1 /*lock bottom layer RAW*/);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "RowPitch was %d, should be %d", l3.RowPitch, 1);
		}
		CHECKRESULT(l3.SlicePitch == 1 /*lock bottom layer RAW*/);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "SlicePitch was %d, should be %d", l3.SlicePitch, 1);
		}

	//		create another texture (256x512x2, 1 mip layer, 32bits/pixel == 128k)
		hr = g_pd3dd8->CreateVolumeTexture(256, 512, 2, 1, 0, D3DFMT_R8G8B8A8, D3DPOOL_DEFAULT, &t[1]);
		CHECKRESULT(hr == D3D_OK /*create second texture while first is unlocked*/);
		if(WASBADRESULT()) {
			goto cleanup;
		}

	//		lock second texture (unswizzled)
		hr = t[1]->LockBox(0, &l4, NULL, 0); 
		CHECKRESULT(hr == D3D_OK);
		CHECKRESULT(l.RowPitch == 256 * sizeof(DWORD));
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "RowPitch was %d, should be %d", l.RowPitch, 256*sizeof(DWORD));
		}
		CHECKRESULT(l.SlicePitch == 256 * 512 * sizeof(DWORD));
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "SlicePitch was %d, should be %d", l.SlicePitch, 256*512*sizeof(DWORD));
		}

	//		write pixels to second texture
		coords[10].Init(   0,    0, 0, &b);
		coords[11].Init(   1,    0, 1, &b);
		coords[12].Init(   2,    0, 0, &b);
		coords[13].Init( 211,  255, 1, &b);
		coords[14].Init(  30,  255, 0, &b);
		coords[15].Init(  31,  255, 0, &b);
		((DWORD*)l4.pBits)[coords[10].i] = 0x12345678;
		((DWORD*)l4.pBits)[coords[11].i] = 0x11111111;
		((DWORD*)l4.pBits)[coords[12].i] = 0x33445566;
		((DWORD*)l4.pBits)[coords[13].i] = 0x91807f6e;
		((DWORD*)l4.pBits)[coords[14].i] = 0xfffefdfc;
		((DWORD*)l4.pBits)[coords[15].i] = 0x01020304;

	//		set a pixel on bottom layer of first texture
		*((BYTE*)l3.pBits) = 255;

	//		unlock bottom layer of first texture(RAW)
		hr = t[0]->UnlockBox(8);
		CHECKRESULT(hr == D3D_OK /*bottom-layer mipmap unlock*/);

	//		unlock layer 3 of first texture(RAW)
		hr = t[0]->UnlockBox(3);
		CHECKRESULT(hr == D3D_OK /*layer 3 mipmap unlock*/);

	//		unlock second texture (unswizzled)
		hr = t[1]->UnlockBox(0);
		CHECKRESULT(hr == D3D_OK /*second texture unlock*/);

	//		checks for corruption of pixels set on RAW top layer

		a.Set(0,SCOORD);
		for(k = 0; k < 256; k += 32) {
			a.Set(0,VCOORD);
			for(j = 0; j < 256; j += 19) {
				a.Set(0,UCOORD);
				for(i = 0; i < 256; i += 31) {
					if(((BYTE*)l.pBits)[a.Get3()] != i + j * (k - 1)) {isbad = true; break;}
					a.Add(big_u, UCOORD);
				}
				a.Add(big_v, VCOORD);
			}
			a.Add(big_v, VCOORD);
		}
		CHECKRESULT(isbad == false /*unswizzle of top layer didn't work right*/ );
		isbad = false;

	//		unlock top layer
		hr = t[0]->UnlockBox(0);
		CHECKRESULT(hr == D3D_OK /*unlock top layer*/);

	//		locks top layer (unswizzled)
		hr = t[0]->LockBox(0, &l, &r, 0); 
		CHECKRESULT(hr == D3D_OK /*second unswizzling of top layer*/);
		CHECKRESULT(l.RowPitch == 256 /*lock top layer of first texture*/);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "RowPitch was %d, should be %d", l.RowPitch, 256);
		}
		CHECKRESULT(l.SlicePitch == 65536 /*lock top layer of first texture*/);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "Slicepitch was %d, should be %d", l.SlicePitch, 65536);
		}

	//		unlock top layer
		hr = t[0]->UnlockBox(0);
		CHECKRESULT(hr == D3D_OK /*second reswizzling of top layer*/);

	//		delete first texture
		hr = t[0]->Release();
		CHECKRESULT(hr == 0 /*release first texture*/);
		t[0] = 0;

	//		lock second texture (RAW)
		hr = t[1]->LockBox(0, &l4, NULL, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK /*lock second texture, RAW*/);
		CHECKRESULT(l.RowPitch == 256 * sizeof(DWORD) /*second texture RAW*/);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "RowPitch was %d, should be %d", l.RowPitch, 256*sizeof(DWORD));
		}
		CHECKRESULT(l.SlicePitch == 512 * 256 * sizeof(DWORD) /*second texture RAW*/);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "Slicepitch was %d, should be %d", l.SlicePitch, 256*512*sizeof(DWORD));
		}

	//		check for corrupted pixels of second texture
		if(((DWORD*)l4.pBits)[coords[10].c] != 0x12345678) isbad = true;
		if(((DWORD*)l4.pBits)[coords[11].c] != 0x11111111) isbad = true;
		if(((DWORD*)l4.pBits)[coords[12].c] != 0x33445566) isbad = true;
		if(((DWORD*)l4.pBits)[coords[13].c] != 0x91807f6e) isbad = true;
		if(((DWORD*)l4.pBits)[coords[14].c] != 0xfffefdfc) isbad = true;
		if(((DWORD*)l4.pBits)[coords[15].c] != 0x01020304) isbad = true;
		CHECKRESULT(isbad == false /*corrupted pixels of second texture*/);

		//		unlock second texture (RAW)
		hr = t[1]->UnlockBox(0);
		CHECKRESULT(hr == D3D_OK /*second texture unlock*/);

	cleanup:
		for(i = 0; i < 3; i++) {
			if(t[i] != NULL) {
				hr = t[i]->Release();
				CHECKRESULT(hr == 0);
				t[i] = 0;
			}
		}

	//check for leaks
		CHECKLEAKS();
		xEndVariation(hlog);
	}
	ENDTEST()



	BEGINTEST(testVolumeTextureUpdateTexture)
	{
		/*
		This test does the following:
			create huge volume map (256x256x256x8bit), full mips (19 mb)
			write random pixels to first texture (multiple layers)
			create second huge texture map (256x256x256x8bit), full mips
			use UpdateTexture to copy first texture to second
			delete first texture
			check random pixels in second texture (unswizzled)
			create 8x8x8 volume map, full mips
			copy lower mip layers from texture2 to texture3
			check texture3
			change pixels in texture2
			copy lower mip layers from texture2 to texture3
			check texture3
			delete texture2, texture3

		this checks:
			big texture
			full mip copy
			8-bit
			partial mip copy
			square texture
			multiple copy from same source
		*/

		DWORD hr;

		IDirect3DVolumeTexture8* t[3] = {0,0,0};
		D3DBOX r;
		D3DLOCKED_BOX l;

		SWIZZLER a (256, 256, 256);
		struct COORD {
			DWORD u;
			DWORD v;
			DWORD s;
			int layer;
		} coords [] = {
			{0, 0, 0, 0},
			{255, 255, 255, 0},
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{32, 32, 5, 1},
			{63, 63, 63, 2},
			{38, 23, 19, 3},
			{2, 3, 10, 4},
			{3, 3, 10, 4},
			{1, 0, 1, 7},
			{0, 0, 0, 8},
		};
		int i;
		bool isbad = false;

		xStartVariation(hlog, " ");
		STARTLEAKCHECK();

	//		create huge texture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
		hr = g_pd3dd8->CreateVolumeTexture(256, 256, 256, 9, 0, D3DFMT_A8, D3DPOOL_DEFAULT, &t[0]);
		CHECKRESULT(hr == D3D_OK /*Create huge texture*/);
		if(WASBADRESULT()) {
			goto cleanup;
		}

		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[0]->LockBox(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//set byte
			((BYTE*)l.pBits)[coords[i].u + l.RowPitch * coords[i].v + l.SlicePitch * coords[i].s] = (BYTE)i;

		//		unlock layer
			hr = t[0]->UnlockBox(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);

		}

	//		create second huge texture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
		hr = g_pd3dd8->CreateVolumeTexture(256, 256, 256, 9, 0, D3DFMT_A8, D3DPOOL_DEFAULT, &t[1]);
		CHECKRESULT(hr == D3D_OK /*Create second huge texture*/);
		if(WASBADRESULT()) {
			goto cleanup;
		}

	//		use UpdateTexture to copy first texture to second
		hr = g_pd3dd8->UpdateTexture(t[0], t[1]);
		CHECKRESULT(hr == D3D_OK /*Update on huge texture*/);

	//		delete first texture
		hr = t[0]->Release();
		CHECKRESULT(hr == 0 /*release first huge texture*/);
		t[0] = 0;

	//		check bytes in second texture
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[1]->LockBox(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//compare byte
			if(((BYTE*)l.pBits)[coords[i].u + l.RowPitch * coords[i].v + l.SlicePitch * coords[i].s] != (BYTE)i) isbad = true;

		//		unlock layer
			hr = t[1]->UnlockBox(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture*/);
		isbad = false;

	//		create smaller texture (256x256, full mips, 8bits/pixel), 
		hr = g_pd3dd8->CreateVolumeTexture(8, 8, 8, 0, 0, D3DFMT_A8, D3DPOOL_DEFAULT, &t[2]);
		CHECKRESULT(hr == D3D_OK /*Create 256x256 texture*/);
		if(WASBADRESULT()) {
			goto cleanup;
		}

	//		use UpdateTexture to copy second texture to third
		hr = g_pd3dd8->UpdateTexture(t[1], t[2]);
		CHECKRESULT(hr == D3D_OK /*Update huge->small*/);
		
	//		check bytes in third texture
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			if (i >= 5) {
				hr = t[1]->LockBox(coords[i].layer -5, &l, &r, 0); 
				CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
		
			//compare byte
			if(((BYTE*)l.pBits)[coords[i].u + l.RowPitch * coords[i].v + l.SlicePitch * coords[i].s] != i) isbad = true;
		
			//		unlock layer
				hr = t[1]->UnlockBox(coords[i].layer -5);
				CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
			}
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture to smaller texture*/);
		isbad = false;

		coords[ 0].u =   7; coords[ 0].v =   7; coords[ 0].s =   7; coords[ 0].layer = 5;
		coords[ 1].u =   6; coords[ 1].v =   7; coords[ 1].s =   7; coords[ 1].layer = 5;
		coords[ 2].u =   5; coords[ 2].v =   7; coords[ 2].s =   7; coords[ 2].layer = 5;
		coords[ 3].u =   0; coords[ 3].v =   1; coords[ 3].s =   4; coords[ 3].layer = 5;
		coords[ 4].u =   3; coords[ 4].v =   1; coords[ 4].s =   3; coords[ 4].layer = 5;
		coords[ 5].u =   3; coords[ 5].v =   1; coords[ 5].s =   1; coords[ 5].layer = 6;
		coords[ 6].u =   0; coords[ 6].v =   3; coords[ 6].s =   2; coords[ 6].layer = 6;
		coords[ 7].u =   1; coords[ 7].v =   1; coords[ 7].s =   1; coords[ 7].layer = 7;
		coords[ 8].u =   0; coords[ 8].v =   0; coords[ 8].s =   0; coords[ 8].layer = 7;
		coords[ 9].u =   1; coords[ 9].v =   0; coords[ 9].s =   1; coords[ 9].layer = 7;
		coords[10].u =   1; coords[10].v =   0; coords[10].s =   0; coords[10].layer = 7;
		coords[11].u =   0; coords[11].v =   0; coords[11].s =   0; coords[11].layer = 8;

		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[1]->LockBox(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//set byte
			((BYTE*)l.pBits)[coords[i].u + l.RowPitch * coords[i].v + l.SlicePitch * coords[i].s] = -i;

		//		unlock layer
			hr = t[1]->UnlockBox(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
		}

		//		use UpdateTexture to copy second texture to third
		hr = g_pd3dd8->UpdateTexture(t[1], t[2]);
		CHECKRESULT(hr == D3D_OK /*Update huge->small again*/);

	//		check bytes in third texture again
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			if (i >= 5) {
				hr = t[1]->LockBox(coords[i].layer -5, &l, &r, 0); 
				CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
		
			//compare byte
			if(((BYTE*)l.pBits)[coords[i].u + l.RowPitch * coords[i].v + l.SlicePitch * coords[i].s] != -i) isbad = true;
		
			//		unlock layer
				hr = t[1]->UnlockBox(coords[i].layer -5);
				CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
			}
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture to smaller texture*/);
		isbad = false;


	cleanup:
		for(i = 0; i < 3; i++) {
			if(t[i] != NULL) {
				hr = t[i]->Release();
				CHECKRESULT(hr == 0);
				t[i] = 0;
			}
		}

		CHECKLEAKS();
		xEndVariation(hlog);
	}
	ENDTEST()

	BEGINTEST(testVolumeTextureUpdateTextureNonSquare)
	{
		/*
		This test does the following:
			create huge texture map (512x8x1024x16bit), full mips 
			write random pixels to first texture (multiple layers)
			create second huge texture map (512x8x1024x16it), full mips
			use UpdateTexture to copy first texture to second
			delete first texture
			check random pixels in second texture (unswizzled)
			create 16x1x32 bit texture map, full mips
			copy lower mip layers from texture2 to texture3
			check texture3
			change pixels in texture2
			copy lower mip layers from texture2 to texture3
			check texture3
			delete texture2, texture3

		this checks:
			big texture
			full mip copy
			16-bit
			partial mip copy
			square texture
			multiple copy from same source
		*/
		DWORD hr;

		IDirect3DVolumeTexture8* t[3] = {0,0,0};
		D3DBOX r;
		D3DLOCKED_BOX l;

		SWIZZLER a (256, 4, 512);
		struct COORD {
			DWORD u;
			DWORD v;
			DWORD s;
			int layer;
		} coords [] = {
			{0, 0, 0, 0},
			{255, 3, 511, 0},
			{1, 0, 0, 0},
			{123, 1, 510, 0},
			{65, 3, 512, 0},
			{79, 1, 238, 1},
			{46, 0, 6, 1},
			{6, 0, 9, 3},
			{2, 0, 10, 5},
			{0, 0, 11, 8},
			{1, 0, 11, 9},
			{0, 0, 12, 10},
		};
		int i;
		bool isbad = false;

		xStartVariation(hlog, "Texture UpdateTexture");
		STARTLEAKCHECK();

	//		create huge texture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
		hr = g_pd3dd8->CreateVolumeTexture(256, 4, 512, 0, 0, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &t[0]);
		CHECKRESULT(hr == D3D_OK /*Create huge texture*/);
		if(WASBADRESULT()) {
			goto cleanup;
		}

		r.Front = r.Left = r.Top = 0;
		r.Bottom = r.Right = r.Back = 1;

		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer

			hr = t[0]->LockBox(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//set byte
			((WORD*)l.pBits)[coords[i].u + l.RowPitch * coords[i].v + l.SlicePitch * coords[i].s] = (WORD)i;

		//		unlock layer
			hr = t[0]->UnlockBox(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);

		}

	//		create second huge texture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
		hr = g_pd3dd8->CreateVolumeTexture(256, 4, 512, 0, 0, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &t[1]);
		CHECKRESULT(hr == D3D_OK /*Create second huge texture*/);
		if(WASBADRESULT()) {
			goto cleanup;
		}

	//		use UpdateTexture to copy first texture to second
		hr = g_pd3dd8->UpdateTexture(t[0], t[1]);
		CHECKRESULT(hr == D3D_OK /*Update on huge texture*/);

	//		delete first texture
		hr = t[0]->Release();
		CHECKRESULT(hr == 0 /*release first huge texture*/);
		t[0] = 0;

	//		check bytes in second texture
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[1]->LockBox(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//compare byte
			if(((WORD*)l.pBits)[coords[i].u + l.RowPitch * coords[i].v + l.SlicePitch * coords[i].s] != (WORD)i) isbad = true;

		//		unlock layer
			hr = t[1]->UnlockBox(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture*/);
		isbad = false;

	//		create smaller texture (256x256, full mips, 8bits/pixel), 
		hr = g_pd3dd8->CreateVolumeTexture(16, 1, 32, 0, 0, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &t[2]);
		CHECKRESULT(hr == D3D_OK /*Create 256x256 texture*/);
		if(WASBADRESULT()) {
			goto cleanup;
		}

	//		use UpdateTexture to copy second texture to third
		hr = g_pd3dd8->UpdateTexture(t[1], t[2]);
		CHECKRESULT(hr == D3D_OK /*Update huge->small*/);
		
	//		check bytes in third texture
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			if (i >= 4) {
				hr = t[1]->LockBox(coords[i].layer -4, &l, &r, 0); 
				CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
		
			//compare byte
			if(((WORD*)l.pBits)[coords[i].u + l.RowPitch * coords[i].v + l.SlicePitch * coords[i].s] != i) isbad = true;
		
			//		unlock layer
				hr = t[1]->UnlockBox(coords[i].layer -4);
				CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
			}
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture to smaller texture*/);
		isbad = false;

		coords[ 0].u =   0; coords[ 0].v =   0; coords[ 0].s =   0; coords[ 0].layer = 5;
		coords[ 1].u =  15; coords[ 1].v =   0; coords[ 1].s =  31; coords[ 1].layer = 5;
		coords[ 2].u =  14; coords[ 2].v =   0; coords[ 2].s =  31; coords[ 2].layer = 5;
		coords[ 3].u =  15; coords[ 3].v =   0; coords[ 3].s =  30; coords[ 3].layer = 5;
		coords[ 4].u =  15; coords[ 4].v =   0; coords[ 4].s =  29; coords[ 4].layer = 5;
		coords[ 5].u =   3; coords[ 5].v =   0; coords[ 5].s =   1; coords[ 5].layer = 6;
		coords[ 6].u =   7; coords[ 6].v =   0; coords[ 6].s =   2; coords[ 6].layer = 6;
		coords[ 7].u =   1; coords[ 7].v =   0; coords[ 7].s =   1; coords[ 7].layer = 7;
		coords[ 8].u =   0; coords[ 8].v =   0; coords[ 8].s =   3; coords[ 8].layer = 7;
		coords[ 9].u =   1; coords[ 9].v =   0; coords[ 9].s =   1; coords[ 9].layer = 7;
		coords[10].u =   1; coords[10].v =   0; coords[10].s =   0; coords[10].layer = 7;
		coords[11].u =   0; coords[11].v =   0; coords[11].s =   0; coords[11].layer = 8;
		coords[12].u =   0; coords[12].v =   0; coords[12].s =   0; coords[12].layer = 9;



		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			hr = t[1]->LockBox(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//set byte
			((WORD*)l.pBits)[coords[i].u + l.RowPitch * coords[i].v + l.SlicePitch * coords[i].s] = (WORD)(-i);

		//		unlock layer
			hr = t[1]->UnlockBox(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
		}

		//		use UpdateTexture to copy second texture to third
		hr = g_pd3dd8->UpdateTexture(t[1], t[2]);
		CHECKRESULT(hr == D3D_OK /*Update huge->small again*/);

	//		check bytes in third texture again
		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			if (i >= 4) {
				hr = t[1]->LockBox(coords[i].layer -4, &l, &r, 0); 
				CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
		
			//compare byte
			if(((WORD*)l.pBits)[coords[i].u + l.RowPitch * coords[i].v + l.SlicePitch * coords[i].s] != (WORD)(-i)) isbad = true;
		
			//		unlock layer
				hr = t[1]->UnlockBox(coords[i].layer -4);
				CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
			}
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture to smaller texture*/);
		isbad = false;

	cleanup:
		for(i = 0; i < 3; i++) {
			if(t[i] != NULL) {
				hr = t[i]->Release();
				CHECKRESULT(hr == 0);
				t[i] = NULL;
			}
		}

		CHECKLEAKS();
		xEndVariation(hlog);
	}
	ENDTEST()

#endif

#pragma data_seg()