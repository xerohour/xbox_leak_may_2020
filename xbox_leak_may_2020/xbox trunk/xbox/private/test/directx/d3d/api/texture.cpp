#include "d3dapi.hpp"

#include "swizzler.h"

#ifndef ROUNDUP
#	define ROUNDUP(a,b)    ((((a)+(b)-1)/(b))*(b))
#endif

#ifndef MIN
#	define MIN(A,B) ((A) <= (B) ? (A) : (B))
#endif

LINKME(0)

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

    D3DFMT_UYVY,
    D3DFMT_YUY2,
    D3DFMT_DXT1,
    D3DFMT_DXT2,
    D3DFMT_DXT3,
    D3DFMT_DXT4,
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

struct texturestuffstruct {
	D3DFORMAT f;
	UINT bitdepth;
	UINT width;
	UINT height;
	UINT count;
	UINT expected;
} Stuff[] = {
	{  D3DFMT_A8R8G8B8, 32,  64,   64,  0,  7},
	{  D3DFMT_A8R8G8B8, 32,  64,   64,  5,  5},
	{  D3DFMT_A8R8G8B8, 32,4096,  128,  0, 13},
	{  D3DFMT_A8R8G8B8, 32,4096,  128,  8,  8},
	{  D3DFMT_A8R8G8B8, 32,2048, 2048, 12, 12},
	{  D3DFMT_A8R8G8B8, 32,   4,    4,  0,  3},
	{  D3DFMT_A8R8G8B8, 32,   2,    2,  0,  2},
	{  D3DFMT_A8R8G8B8, 32,   1,    1,  0,  1},

	{      D3DFMT_DXT1,  4,  64,   64,  0,  7},
	{      D3DFMT_DXT1,  4,  64,   64,  5,  5},
	{      D3DFMT_DXT1,  4,4096,  128,  0, 13},
	{      D3DFMT_DXT1,  4,4096,  128,  8,  8},
	{      D3DFMT_DXT1,  4,4096, 4096, 11, 11},
	{      D3DFMT_DXT1,  4,   4,    4,  0,  3},

	{    D3DFMT_R5G6B5, 16,  64,   64,  5,  5},
	{        D3DFMT_A8,  8,  64,   64,  5,  5}
	
#if 1 // NUGOOP
	                                       ,
	{D3DFMT_LIN_R5G6B5, 16,  64,   64,  0,  1},
	{D3DFMT_LIN_R5G6B5, 16,1021, 1024,  0,  1},
	{D3DFMT_LIN_R5G6B5, 16,  64,  127,  0,  1},
	{D3DFMT_LIN_R5G6B5, 16, 381,  412,  1,  1},
	{D3DFMT_LIN_R5G6B5, 16,4096, 4096,  0,  1}
#endif
};

/*
//    HRESULT WINAPI CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture8 **ppTexture);
//    HRESULT WINAPI CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture8 **ppVolumeTexture);
//    HRESULT WINAPI CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture8 **ppCubeTexture);

    HRESULT WINAPI UpdateTexture(IDirect3DBaseTexture8 *pSourceTexture, IDirect3DBaseTexture8 *pDestinationTexture);

    HRESULT WINAPI BeginStateBlock();
    HRESULT WINAPI EndStateBlock(DWORD *pToken);
    HRESULT WINAPI ApplyStateBlock(DWORD Token);
    HRESULT WINAPI CaptureStateBlock(DWORD Token);
    HRESULT WINAPI DeleteStateBlock(DWORD Token);
    HRESULT WINAPI CreateStateBlock(D3DSTATEBLOCKTYPE Type,DWORD *pToken);

    HRESULT WINAPI GetTexture(DWORD Stage, IDirect3DBaseTexture8 **ppTexture);
    HRESULT WINAPI SetTexture(DWORD Stage, IDirect3DBaseTexture8 *pTexture);

    HRESULT WINAPI GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue);
    HRESULT WINAPI SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);

    HRESULT WINAPI SetCurrentTexturePalette(UINT PaletteNumber);
    HRESULT WINAPI GetCurrentTexturePalette(UINT *PaletteNumber);

//	HRESULT WINAPI IDirect3DTexture8_GetDevice(IDirect3DTexture8 *pThis, IDirect3DDevice8 **ppDevice) { return IDirect3DResource8_GetDevice((IDirect3DResource8 *)pThis, ppDevice); }
//	D3DRESOURCETYPE WINAPI IDirect3DTexture8_GetType(IDirect3DTexture8 *pThis) { return IDirect3DResource8_GetType((IDirect3DResource8 *)pThis); }
//	BOOLEAN WINAPI IDirect3DTexture8_IsBusy(IDirect3DTexture8 *pThis) { return IDirect3DResource8_IsBusy((IDirect3DResource8 *)pThis); }
//	DWORD   WINAPI IDirect3DTexture8_GetLevelCount(IDirect3DTexture8 *pThis) { return IDirect3DBaseTexture8_GetLevelCount((IDirect3DBaseTexture8 *)pThis); }

//	HRESULT WINAPI IDirect3DTexture8_GetLevelDesc(IDirect3DTexture8 *pThis, UINT Level, D3DSURFACE_DESC *pDesc);
//	HRESULT WINAPI IDirect3DTexture8_GetSurfaceLevel(IDirect3DTexture8 *pThis, UINT Level, IDirect3DSurface8 **ppSurfaceLevel);
//	HRESULT WINAPI IDirect3DTexture8_LockRect(IDirect3DTexture8 *pThis, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags);
//	HRESULT WINAPI IDirect3DTexture8_UnlockRect(IDirect3DTexture8 *pThis, UINT Level);

  in resource.cpp:
    HRESULT WINAPI IDirect3DTexture8_SetPrivateData(IDirect3DTexture8 *pThis, REFGUID refguid, CONST void *pData, DWORD SizeOfData, DWORD Flags) { return IDirect3DResource8_SetPrivateData((IDirect3DResource8 *)pThis, refguid, pData, SizeOfData, Flags); }
	HRESULT WINAPI IDirect3DTexture8_GetPrivateData(IDirect3DTexture8 *pThis, REFGUID refguid, void *pData, DWORD *pSizeOfData) { return IDirect3DResource8_GetPrivateData((IDirect3DResource8 *)pThis, refguid, pData, pSizeOfData); }
	HRESULT WINAPI IDirect3DTexture8_FreePrivateData(IDirect3DTexture8 *pThis, REFGUID refguid) { return IDirect3DResource8_FreePrivateData((IDirect3DResource8 *)pThis, refguid); }

*/


#pragma data_seg(".d3dapi$test100") 

BEGINTEST(testCreateTexture)
{
	IDirect3DTexture8* t;
	DWORD hr;
	int a, b, c, d;

	xStartVariation(hlog, "CreateTexture, Formats Loop");
	STARTLEAKCHECK();

	for(a = 0; a < COUNTOF(TextureTypes); a++) {
		hr = g_pd3dd8->CreateTexture(64, 64, 0, 0, TextureTypes[a], D3DPOOL_DEFAULT, &t);
		CHECKRESULT(hr == D3D_OK);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "x: %d, y: %d, Lev: %d, Usage: %d, Format: %d",
				64, 64, 0, 0, TextureTypes[a]);
			break;
		}
		hr = t->Release();
		CHECKRESULT(hr == 0);
	}

	CHECKLEAKS();
	xEndVariation(hlog);
	xStartVariation(hlog, "CreateTexture, Size & Levels Loop");
	STARTLEAKCHECK();

	for(b = 0; b < COUNTOF(Sizes); b++) {
	for(c = 0; c < COUNTOF(Levels); c++) {
	for(d = 0; d < COUNTOF(Usages); d++) {
		hr = g_pd3dd8->CreateTexture(Sizes[b].x, Sizes[b].y, 
			Levels[c], Usages[d], D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t);
		CHECKRESULT(hr == D3D_OK);
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "x: %d, y: %d, Lev: %d, Usage: %d, Format: %d",
				Sizes[b].x, Sizes[b].y,	Levels[c], Usages[d], D3DFMT_A8R8G8B8);
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

//ULONG Rnd() { return ::Rnd(); }
ULONG Rnd(int a) {
	if(a == 0) return 0;
	return ::rand() % abs(a);
}

void FillRect(void* pBits, void* pBits2, int x1, int y1, int x2, int y2, int pitch, int bpp) {
	int i, j, b;
	if(x1 == 0 && x2 == 0) {x1 = 0; x2 = pitch / bpp;}
	if(pBits2) {
		for(j = y1; j < y2; j++) {
			for(i = x1; i < x2; i++) {
				for(b = 0; b < bpp; b++) {
					((char*)pBits)[j * pitch + i*bpp + b] = (char)rand();
					((char*)pBits2)[j * pitch + i*bpp + b] = ((char*)pBits)[j * pitch + i*bpp + b];
				}
			}
		}
	} else {
		for(j = y1; j < y2; j++) {
			for(i = x1; i < x2; i++) {
				for(b = 0; b < bpp; b++) {
					((char*)pBits)[j * pitch + i*bpp + b] = (char)rand();
				}
			}
		}
	}
}

void CopyRect(void* pDest, const void*pSrc, int rx1, int ry1, int rx2, int ry2, int px, int py, int dpitch, int spitch, int bpp) {
	int i, j, b;
	int rpitch = rx2 * bpp - rx1 * bpp;
	for(j = ry1; j < ry2; j++) {
		for(i = 0; i < (rpitch & ~15); i += 16) {
			char* tempsrc = &((char*)pSrc)[(j)*spitch+ rx1*bpp + i];
			char* tempdst = &((char*)pDest)[(j - ry1 + py) * dpitch + px*bpp + i];
			_asm {
				mov esi, tempsrc
				mov edi, tempdst
				movups xmm0, [esi]
				movups [edi], xmm0
			}
		}
		for( ; i < rpitch; i++) {
			((char*)pDest)[(j - ry1 + py) * dpitch + px*bpp + i] = ((char*)pSrc)[(j)*spitch+ rx1*bpp + i];
		}
	}
}

bool FastCompare(void *pSource, void *pDest, DWORD Size)
{
    if((Size & 31) != 0) {
		_asm {int 3};
	}

    void *pSourceMax = (BYTE *)pSource + Size;
    ULONGLONG Temp;

    _asm 
    {
        mov esi, [pSource]
        mov edi, [pDest]
        mov edx, [pSourceMax]
        mov eax, 1

Start:

        ; Get the memory to compare.

        movq mm0, [esi]
        movq mm1, [esi + 8]
        movq mm2, [esi + 16]
        movq mm3, [esi + 24]
        movq mm4, [edi]
        movq mm5, [edi + 8]
        movq mm6, [edi + 16]
        movq mm7, [edi + 24]

        ; If any of these compares fails then the appopriate DWORD will be 
        ; set to zero, otherwise all bits will be set to one.

        pcmpeqd mm0, mm4
        pcmpeqd mm1, mm5
        pcmpeqd mm2, mm6
        pcmpeqd mm3, mm7

        ; Collapse all of the comparisons together.

        pand mm0, mm1
        pand mm0, mm2
        pand mm0, mm3

        ; AND the high and low DWORD of mm0.  If any of the compares
        ; failed then the result in eax will be zero.  

        movd      eax, mm0
        punpckhdq mm0, mm0
        movd      ecx, mm0
        and       eax, ecx

        jz Done

        add esi, 32
        add edi, 32

        cmp esi, edx
        jne Start

Done:
		emms
    }
}

BEGINTEST(testCopyRects)
{
    IDirect3DTexture8 *t[3] = {0,0,0};
    IDirect3DSurface8 *s[3] = {0,0,0};
	int Width[3], Height[3];
	RECT   Src[1000];
	RECT*  pSrc;
	POINT  Dest[1000];
	POINT* pDest;
	int i, NumRects, r, z, f;
	D3DLOCKED_RECT LockedRect[3];
	D3DFORMAT Formats[] = {D3DFMT_LIN_A8, D3DFMT_LIN_R5G6B5, D3DFMT_LIN_A8R8G8B8};
	D3DFORMAT SwizFormats[] = {D3DFMT_A8, D3DFMT_R5G6B5, D3DFMT_A8R8G8B8, D3DFMT_DXT1, D3DFMT_DXT2, D3DFMT_DXT4};
	int SwizBpp[] = {8,16,32,4,8,16};
//	D3DFORMAT DxtFormats[] = {};
	HRESULT hr;

/*
    g_pd3dd8->CreateTexture(32, 32, 0, 0, D3DFMT_A8R8G8B8, 0, &t[0]);
    g_pd3dd8->CreateTexture(32, 32, 0, 0, D3DFMT_A8R8G8B8, 0, &t[1]);
 
    D3DSURFACE_DESC         desc1;
    D3DSURFACE_DESC         desc2;
 
    t[0]->GetSurfaceLevel(5, &s[0]);
    t[1]->GetSurfaceLevel(5, &s[1]);
	s[0]->LockRect(&LockedRect[0],0,0);
	s[1]->LockRect(&LockedRect[1],0,0);
 
    g_pd3dd8->CopyRects(s[1], NULL, 0, s[0], NULL);

	s[0]->UnlockRect();
	s[0]->UnlockRect();
	
    s[0]->Release();
    s[1]->Release();
	t[0]->Release();
	t[1]->Release();
*/
	xStartVariation(hlog, "linear textures");
	for(i = 0; i < 3; i++) {
		for(f = 0; f < COUNTOF(Formats); f++) {
			do {
				Width[0] = Rnd(4096) + 1;
				Height[0] = Rnd(4096) + 1;
			} while (Width[0] * (1 << f) > 8128 || Width[0] * Height[0] * (1 << f) > 0x1000000);
			do {
				Width[1] = Rnd(4096) + 1;
				Height[1] = Rnd(4096) + 1;
			} while (Width[1] * (1 << f) > 8128 || Width[1] * Height[1] * (1 << f) > 0x1000000);
			Width[2] = Width[1]; Height[2] = Height[1];
			for(z = 0; z < 3; z++) {
				hr = g_pd3dd8->CreateTexture(Width[z], Height[z], 1, 0, Formats[f], D3DPOOL_DEFAULT, &t[z]);
				CHECKRESULT(hr == D3D_OK && "CreateTexture");
			}
			if(WASGOODRESULT()) {
				for(z = 0; z < 3; z++) {
					hr = t[z]->GetSurfaceLevel(0, &s[z]);
					CHECKRESULT(hr == D3D_OK && "CreateSurfaceLevel");
				}
			}
			if(WASGOODRESULT()) {
				for(z = 0; z < 3; z++) {
					hr = s[z]->LockRect(&LockedRect[z], 0, 0);
					CHECKRESULT(hr == D3D_OK && "LockRect");
				}
			}
			if(WASGOODRESULT()) {

				FillRect(LockedRect[0].pBits,                NULL, 0,0,0/*Width[0]*/,Height[0],LockedRect[0].Pitch, (1 << f));
				FillRect(LockedRect[1].pBits, LockedRect[2].pBits, 0,0,0/*Width[1]*/,Height[1],LockedRect[1].Pitch, (1 << f));
			
//				CHECKRESULT(!memcmp(LockedRect[1].pBits, LockedRect[2].pBits, Height[1] * LockedRect[1].Pitch) || !"Lin CopyRects Comparison");

				NumRects = Rnd(10);
				if(((Rnd(2) || (NumRects == 0)) && Width[0] <= Width[1]) && Height[0] <= Height[1]) {
					//null RECTs
					pSrc = NULL;

					if(Rnd(2)) {
						NumRects = 0;
						//null POINTs
						pDest = NULL;
						CopyRect(LockedRect[2].pBits, LockedRect[0].pBits, 0,0,Width[0],Height[0], 
							0,0,LockedRect[2].Pitch, LockedRect[0].Pitch, (1 << f));
					} else {
						pDest = Dest;
						for(r = 0; r < max(NumRects,1); r++) {
							Dest[r].x = Rnd(Width[1] - Width[0]);
							Dest[r].y = Rnd(Height[1] - Height[0]);
							CopyRect(LockedRect[2].pBits, LockedRect[0].pBits, 0,0,Width[0],Height[0], 
								Dest[r].x,Dest[r].y,LockedRect[2].Pitch, LockedRect[0].Pitch, (1 << f));
						}
					}
				} else {
					if(NumRects == 0) NumRects = 1;
					pSrc = Src;
					if(Rnd(2)) { 
						pDest = NULL;
						//null POINTs
						for(r = 0; r < max(NumRects,1); r++) {
							Src[r].top = Rnd(MIN(Height[0], Height[1]) - 1);
							Src[r].bottom = Src[r].top + Rnd(MIN(Height[0], Height[1]) - Src[r].top) + 1;
							Src[r].left = Rnd(MIN(Width[0], Width[1]) - 1);
							Src[r].right = Src[r].left + Rnd(MIN(Width[0], Width[1]) - Src[r].left) + 1;
							CopyRect(LockedRect[2].pBits, LockedRect[0].pBits, Src[r].left, Src[r].top, Src[r].right, Src[r].bottom, 
								Src[r].left, Src[r].top,LockedRect[2].Pitch, LockedRect[0].Pitch, (1 << f));

						}
					} else {
						pDest = Dest;
						for(r = 0; r < max(NumRects,1); r++) {
							Src[r].top = Rnd(Height[0] - 1);
							Src[r].bottom = 1 + Src[r].top + Rnd(MIN(Height[1], Height[0] - Src[r].top));
							Src[r].left = Rnd(Width[0] - 1);
							Src[r].right = 1 + Src[r].left + Rnd(MIN(Width[1], Width[0] - Src[r].left));
							Dest[r].x = Rnd(Width[1] - (Src[r].right - Src[r].left));
							Dest[r].y = Rnd(Height[1] - (Src[r].bottom - Src[r].top));
							CopyRect(LockedRect[2].pBits, LockedRect[0].pBits, Src[r].left, Src[r].top, Src[r].right, Src[r].bottom, 
								Dest[r].x, Dest[r].y,LockedRect[2].Pitch, LockedRect[0].Pitch, (1 << f));
						}
					}
				}

				xLog(hlog, XLL_INFO, "S1: %dx%d. S2: %dx%d. Rects:%d. Bpp%d. S%p D%p", Width[0],Height[0],Width[1],Height[1], NumRects, (1 << f), pSrc, pDest);
//				if (NumRects >= 2) {
//					for(r = 0; r < NumRects; r++) {
//						hr = g_pd3dd8->CopyRects(s[0], &pSrc[r], 1, s[1], &pDest[r]);
//					}
//				} else {
					hr = g_pd3dd8->CopyRects(s[0], pSrc, NumRects, s[1], pDest);
//				}
				CHECKRESULT(hr == D3D_OK && "CopyRects result");
				s[1]->BlockUntilNotBusy();
                CHECKRESULT(FastCompare(LockedRect[1].pBits, LockedRect[2].pBits, Height[1] * LockedRect[1].Pitch) || !"Lin CopyRects Comparison");
				if(WASBADRESULT()) { _asm {int 3} ; }
			} else {
				xLog(hlog, XLL_INFO, "S1: %dx%d. S2: %dx%d. Rects:%d. Bpp%d", Width[0],Height[0],Width[1],Height[1], NumRects, (1 << f));
				xLog(hlog, XLL_INFO, "copyrects: couldn't create the textures!");
			}
			for(z = 0; z < 3; z++) {
				if(s[z]) {
					s[z]->UnlockRect();
					s[z]->Release();
					t[z]->Release();
				} else if (t[z]) t[z]->Release();
				s[z]=0;t[z]=0;
			}
		}
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "Swizzled Textures");
	D3DSURFACE_DESC desc;
	for(i = 0; i < 50; i++) {
		for(f = 0; f < COUNTOF(SwizFormats); f++) {
			do {
				Width[1] = Width[0] = (1 << (rand() % 12));
				Height[1] = Height[0] = (1 << (rand() % 12));
				if(i == 0) {
					Width[1] = Width[0] = 1;
					Height[1] = Height[0] = 1;
				}
				if(i == 1) {
					Width[1] = Width[0] = 2;
					Height[1] = Height[0] = 1;
				}
				if(i == 2) {
					Width[1] = Width[0] = 1;
					Height[1] = Height[0] = 2;
				}
				if(i == 3) {
					Width[1] = Width[0] = 2;
					Height[1] = Height[0] = 2;
				}
			} while ((Width[0] * Height[0] * SwizBpp[f]) / 8 > 0x2000000);
			for(z = 0; z < 2; z++) {
				hr = g_pd3dd8->CreateTexture(Width[z], Height[z], 1, 0, SwizFormats[f], D3DPOOL_DEFAULT, &t[z]);
				CHECKRESULT(hr == D3D_OK);
			}
			if(WASGOODRESULT()) {
				for(z = 0; z < 2; z++) {
					hr = t[z]->GetSurfaceLevel(0, &s[z]);
					CHECKRESULT(hr == D3D_OK);
				}
			}

			t[0]->GetLevelDesc(0,&desc);

			if(WASGOODRESULT()) {
				for(z = 0; z < 2; z++) {
					hr = s[z]->LockRect(&LockedRect[z], 0, 0);
					CHECKRESULT(hr == D3D_OK);
				}
			}
			if(WASGOODRESULT()) {
				UINT asdf;
				for(asdf = 0; asdf < desc.Size; asdf++) {
					((char*)LockedRect[0].pBits)[asdf] = (char)rand();
				}
				
//				FillRect(LockedRect[0].pBits, NULL, 0,0,Width[0],Height[0],LockedRect[0].Pitch, (1 << f));

				xLog(hlog, XLL_INFO, "S1: %dx%d. S2: %dx%d. Rects:%d. Format:%d", Width[0],Height[0],Width[1],Height[1], NumRects, f);
				hr = g_pd3dd8->CopyRects(s[0], NULL, 0, s[1], NULL);
				CHECKRESULT(hr == D3D_OK || !"CopyRects Return Value");
				s[1]->BlockUntilNotBusy();
				if(desc.Size & 31) {
					CHECKRESULT(!memcmp(LockedRect[1].pBits, LockedRect[0].pBits, desc.Size /*Height[1] * LockedRect[1].Pitch*/) && 
					"Swiz CopyRects Comparison");
				} else {
					CHECKRESULT(FastCompare(LockedRect[1].pBits, LockedRect[0].pBits, desc.Size /*Height[1] * LockedRect[1].Pitch*/) && 
					"Swiz CopyRects Comparison");
				}
			} else {
				xLog(hlog, XLL_INFO, "S1: %dx%d. S2: %dx%d. Rects:%d. Format:%d", Width[0],Height[0],Width[1],Height[1], NumRects, f);
				xLog(hlog, XLL_INFO, "copyrects: couldn't create the textures!");
			}
			for(z = 0; z < 2; z++) {
				if(s[z]) {
					s[z]->UnlockRect();
					s[z]->Release();
					t[z]->Release();
				} else if (t[z]) t[z]->Release();
				s[z]=0;t[z]=0;
			}
		}
	}
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testTextureGetDevice)
{
	DWORD hr;
	IDirect3DTexture8* t;
	IDirect3DDevice8* d;

	xStartVariation(hlog, "TextureGetDevice");
	STARTLEAKCHECK();
	hr = g_pd3dd8->CreateTexture(64, 64, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t);
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

BEGINTEST(testTextureGetType)
{
	DWORD hr;
	IDirect3DTexture8* t;

	xStartVariation(hlog, "TextureGetType");
	STARTLEAKCHECK();
	hr = g_pd3dd8->CreateTexture(64, 64, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t);
	CHECKRESULT(hr == D3D_OK);
	CHECKRESULT(t->GetType() == D3DRTYPE_TEXTURE);
	CHECKRESULT(t->Release() == 0);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testTextureIsBusy)
{
	//#pragma TODO("test IsBusy")
}
ENDTEST()

BEGINTEST(testTextureGetLevelCount)
{
	DWORD hr;
	IDirect3DTexture8* t;
	int i;

	xStartVariation(hlog, "TextureGetLevelCount");
	STARTLEAKCHECK();

	for(i = 0; i < COUNTOF(Stuff); i++) {
		hr = g_pd3dd8->CreateTexture(Stuff[i].width, Stuff[i].height, Stuff[i].count, 0, Stuff[i].f, D3DPOOL_DEFAULT, &t);
		CHECKRESULT(hr == D3D_OK);
		if(WASGOODRESULT()) {
			hr = t->GetLevelCount();
			CHECKRESULT(hr == Stuff[i].expected /*GetLevelCount()*/);
			if(WASBADRESULT()) {
				xLog(hlog, XLL_INFO, "w:%d, h:%d, count:%d, format:%d, GetLevelCount():%d, expected:%d",
					Stuff[i].width, Stuff[i].height, Stuff[i].count, Stuff[i].f, hr, Stuff[i].expected);
			}
			CHECKRESULT(t->Release() == 0);
		}
	}
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

bool IsSwizzledD3DFORMAT(D3DFORMAT Format);
namespace D3D {
//bool IsSwizzledD3DFORMAT(D3DFORMAT Format);
bool IsCompressedD3DFORMAT(D3DFORMAT Format);
}

BEGINTEST(testTextureGetLevelDesc)
{
	DWORD hr;
	IDirect3DTexture8* t;
	D3DSURFACE_DESC d;
	int i;
	unsigned int j;
	UINT w, h, w4, h4;

	xStartVariation(hlog, "TextureGetLevelDesc");
	STARTLEAKCHECK();

	for(i = 0; i < COUNTOF(Stuff); i++) {
		hr = g_pd3dd8->CreateTexture(Stuff[i].width, Stuff[i].height, Stuff[i].count, 0, Stuff[i].f, D3DPOOL_DEFAULT, &t);
		w = Stuff[i].width; 
		h = Stuff[i].height;
		CHECKRESULT(hr == D3D_OK);
//		CHECKRESULT(t->GetLevelCount() == Stuff[i].expected);
		if(WASGOODRESULT()) { 
			for(j = 0; j < t->GetLevelCount(); j++) {
		//		if(Stuff[i].width == 381) {
		//			_asm int 3
		//		}

				hr = t->GetLevelDesc(j, &d);
				CHECKRESULT(hr == D3D_OK);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "GetLevelDesc returned %d", hr);
				CHECKRESULT(d.Width == w);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "d.Width = %d, should be %d", d.Width, w);
				CHECKRESULT(d.Height == h);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "d.Height = %d, should be %d", d.Height, h);
				if(Stuff[i].f == D3DFMT_DXT1 || Stuff[i].f == D3DFMT_DXT2 || Stuff[i].f == D3DFMT_DXT4) {
					if(w < 4) w4 = 4; else w4 = w;
					if(h < 4) h4 = 4; else h4 = h;
                } else if (!IsSwizzledD3DFORMAT(Stuff[i].f)) {
					w4 = ROUNDUP(w, D3DTEXTURE_PITCH_MIN * 8 / Stuff[i].bitdepth);
					h4 = h;
				} else {
					w4 = w;
					h4 = h;
				}
				
				CHECKRESULT(d.Size == w4 * h4 * Stuff[i].bitdepth /8);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "d.Size = %d, should be %d", d.Size, w4 * h4 * Stuff[i].bitdepth/8);
				CHECKRESULT(d.Format == Stuff[i].f);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "d.Format = %d, should be %d", d.Format, Stuff[i].f);
				w = (w >> 1) ? (w >> 1) : 1;
				h = (h >> 1) ? (h >> 1) : 1;
			}
			CHECKRESULT(t->Release() == 0);
		}
	}
	CHECKRESULT(d.Type == D3DRTYPE_TEXTURE);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testTextureGetSurfaceLevel)
{
	DWORD hr;
	IDirect3DTexture8* t;
	IDirect3DSurface8* s;
	D3DSURFACE_DESC d;
	int i;
	unsigned int j;
	UINT w, h, w4, h4;

	xStartVariation(hlog, "TextureGetSurfaceLevel");
	STARTLEAKCHECK();

	for(i = 0; i < COUNTOF(Stuff); i++) {
		hr = g_pd3dd8->CreateTexture(Stuff[i].width, Stuff[i].height, Stuff[i].count, 0, Stuff[i].f, D3DPOOL_DEFAULT, &t);
		w = Stuff[i].width; 
		h = Stuff[i].height;
		CHECKRESULT(hr == D3D_OK);
		if(WASGOODRESULT()) {
			for(j = 0; j < t->GetLevelCount(); j++) {
				t->GetSurfaceLevel(j, &s);
				s->GetDesc(&d);
	  			CHECKRESULT(d.Width == w);	// //basically already tested above
				if(WASBADRESULT()) {
					xLog(hlog, XLL_INFO, "width is %d, should be %d", d.Width, w);
				}
	  			CHECKRESULT(d.Height == h);
				if(WASBADRESULT()) {
					xLog(hlog, XLL_INFO, "height is %d, should be %d", d.Height, h);
				}

				if(Stuff[i].f == D3DFMT_DXT1 || Stuff[i].f == D3DFMT_DXT2 || Stuff[i].f == D3DFMT_DXT4) {
					if(w < 4) w4 = 4; else w4 = w;
					if(h < 4) h4 = 4; else h4 = h;
                } else if (!IsSwizzledD3DFORMAT(Stuff[i].f)) {
					w4 = ROUNDUP(w, D3DTEXTURE_PITCH_MIN * 8 / Stuff[i].bitdepth);
					h4 = h;
				} else {
					w4 = w;
					h4 = h;
				}
				
				CHECKRESULT(d.Size == w4 * h4 * Stuff[i].bitdepth/8);
				if(WASBADRESULT()) xLog(hlog, XLL_INFO, "size is: %d, should be %d. w=%d,h=%d,f=%d. i=%d,j=%d, bpp=%d", 
					d.Size, w4*h4*Stuff[i].bitdepth/8, w,h,Stuff[i].f,i,j, Stuff[i].bitdepth);
				CHECKRESULT(d.Format == Stuff[i].f);
				w = (w >> 1) ? (w >> 1) : 1;
				h = (h >> 1) ? (h >> 1) : 1;
				hr = s->Release();
				CHECKRESULT(hr == 0);
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
	BEGINTEST(testTextureLockAndUnlockRect)
	{
		/*
		This test does the following:
			create huge texture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
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
		IDirect3DTexture8* t[3] = {0,0,0};
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
		SWIZZLER b (2048, 256, 0);
		SWIZCOORD coords[32];

		SWIZNUM big_u = a.Convert(259, UCOORD);
		SWIZNUM big_v = a.Convert(273, VCOORD);

		xStartVariation(hlog, "Texture Lock Rect");
		STARTLEAKCHECK();

	//		create huge texture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
		hr = g_pd3dd8->CreateTexture(4096, 4096, 13, 0, D3DFMT_A8, D3DPOOL_DEFAULT, &t[0]);
		CHECKRESULT(hr == D3D_OK /*Create huge texture*/);
		if(WASBADRESULT()) {
			goto cleanup;
		}

		r.top = 0;
		r.left = 0;
		r.right = 4095;
		r.bottom = 4095;

	//		locks the top layer as RAW_DATA
		hr = t[0]->LockRect(0, &l, &r, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK /*lock top layer RAW*/);
		CHECKRESULT(l.Pitch == 4096 /*lock top layer RAW*/);
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "l.pitch = %d, should be %d", l.Pitch, 4096);
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
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "l.pitch = %d, should be %d", l.Pitch, 4096);

	//		checks if the pixels tweaked earlier are still set
		if(((BYTE*)l.pBits)[coords[0].i] != 0) isbad = true;
		if(((BYTE*)l.pBits)[coords[1].i] != 255) isbad = true;
		if(((BYTE*)l.pBits)[coords[2].i] != 31) isbad = true;
		if(((BYTE*)l.pBits)[coords[3].i] != 41) isbad = true;
		CHECKRESULT(isbad == false /*swizzle didn't work right*/);

		isbad = false;

	//		locks layer 3 (512x512), (RAW)
		hr = t[0]->LockRect(3, &l2, NULL, D3DLOCK_RAWDATA); //this should be a 512x512 texture (swizzled)
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
		CHECKRESULT(hr == D3D_OK /*swizzled texture unlock*/);

	//		locks top layer (RAW)
	//	_asm int 3; 
		hr = t[0]->LockRect(0, &l, &r, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK /*lock top layer RAW*/);
		CHECKRESULT(l.Pitch == 4096 /*lock top layer RAW*/);
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "l.pitch = %d, should be %d", l.Pitch, 4096);

	//		lock bottom layer (RAW)
		hr = t[0]->LockRect(12, &l3, NULL, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK /*lock bottom layer RAW*/);
		CHECKRESULT(l3.Pitch == 1 /*lock bottom layer RAW*/);
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "l3.pitch = %d, should be %d", l3.Pitch, 1);

	//		create another texture (256x512, 1 mip layer, 32bits/pixel == 128k)
		hr = g_pd3dd8->CreateTexture(2048, 256, 1, 0, D3DFMT_R8G8B8A8, D3DPOOL_DEFAULT, &t[1]);
		CHECKRESULT(hr == D3D_OK /*create second texture while first is unlocked*/);
		if(WASBADRESULT()) {
			goto cleanup;
		}

	//		lock second texture (unswizzled)
		hr = t[1]->LockRect(0, &l4, NULL, 0); 
		CHECKRESULT(hr == D3D_OK);
		CHECKRESULT(l4.Pitch == 2048 * sizeof(DWORD));
		if(WASBADRESULT()) {
			xLog(hlog, XLL_INFO, "l4.pitch = %d, should be %d", l4.Pitch, 2048*sizeof(DWORD));
		}

	//		write pixels to second texture
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

	//		set a pixel on bottom layer of first texture
		*((BYTE*)l3.pBits) = 255;

	//		unlock bottom layer of first texture(RAW)
		hr = t[0]->UnlockRect(12);
		CHECKRESULT(hr == D3D_OK /*bottom-layer mipmap unlock*/);

	//		unlock layer 3 of first texture(RAW)
		hr = t[0]->UnlockRect(3);
		CHECKRESULT(hr == D3D_OK /*layer 3 mipmap unlock*/);

	//		unlock second texture (unswizzled)
		hr = t[1]->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK /*second texture unlock*/);

	//		checks for corruption of pixels set on RAW top layer
		a.Set(0,VCOORD);
		for(j = 0; j < 4096; j += 273) {
			a.Set(0,UCOORD);
			for(i = 0; i < 4096; i += 259) {
				if(((BYTE*)l.pBits)[a.Get2()] != i + j) {isbad = true; break;}
				a.Add(big_u, UCOORD);
			}
			a.Add(big_v, VCOORD);
		}
		CHECKRESULT(isbad == false /*unswizzle of top layer didn't work right*/ );
		isbad = false;

	//		unlock top layer
		hr = t[0]->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK /*unlock top layer*/);

	//		locks top layer (unswizzled)
		hr = t[0]->LockRect(0, &l, &r, 0); 
		CHECKRESULT(hr == D3D_OK /*second unswizzling of top layer*/);
		CHECKRESULT(l.Pitch == 4096 /*lock top layer of first texture*/);
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "l.pitch = %d, should be %d", l.Pitch, 4096);

	//		unlock top layer
		hr = t[0]->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK /*second reswizzling of top layer*/);

	//		delete first texture
		hr = t[0]->Release();
		t[0] = 0;
		CHECKRESULT(hr == 0 /*release first texture*/);

	//		lock second texture (RAW)
		hr = t[1]->LockRect(0, &l4, NULL, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK /*lock second texture, RAW*/);
		CHECKRESULT(l4.Pitch == 2048 * sizeof(DWORD) /*second texture RAW*/);
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "l4.pitch = %d, should be %d", l4.Pitch, 2048*sizeof(DWORD));

	//		check for corrupted pixels of second texture
		if(((DWORD*)l4.pBits)[coords[10].c] != 0x12345678) isbad = true;
		if(((DWORD*)l4.pBits)[coords[11].c] != 0x11111111) isbad = true;
		if(((DWORD*)l4.pBits)[coords[12].c] != 0x33445566) isbad = true;
		if(((DWORD*)l4.pBits)[coords[13].c] != 0x91807f6e) isbad = true;
		if(((DWORD*)l4.pBits)[coords[14].c] != 0xfffefdfc) isbad = true;
		if(((DWORD*)l4.pBits)[coords[15].c] != 0x01020304) isbad = true;
		CHECKRESULT(isbad == false /*corrupted pixels of second texture*/);

		hr = t[1]->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);

	cleanup:
		for(i = 0; i < 3; i++) {
			if(t[i] != NULL) {
				hr = t[i]->Release();
				CHECKRESULT(hr == 0);
				t[i] = NULL;
			}
		}

	//check for leaks
		CHECKLEAKS();
		xEndVariation(hlog);
	}
	ENDTEST()


	BEGINTEST(testTextureUpdateTexture)
	{
		/*
		This test does the following:
			create huge texture map (4096x4096x8bit), full mips (22.3 mb)
			write random pixels to first texture (multiple layers)
			create second huge texture map (4096x4096x8bit), full mips
			use UpdateTexture to copy first texture to second
			delete first texture
			check random pixels in second texture (unswizzled)
			create 256x256 texture map, full mips
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

		IDirect3DTexture8* t[3] = {0,0,0};
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

		xStartVariation(hlog, "");
		STARTLEAKCHECK();

	//		create huge texture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
		hr = g_pd3dd8->CreateTexture(4096, 4096, 13, 0, D3DFMT_A8, D3DPOOL_DEFAULT, &t[0]);
		CHECKRESULT(hr == D3D_OK /*Create huge texture*/);
		if(WASBADRESULT()) {
			goto cleanup;
		}

		for(i = 0; i < COUNTOF(coords); i++) {
		//		lock layer
			r.bottom = 4096 / (1 << coords[i].layer) - 1;
			r.right = 4096 / (1 << coords[i].layer) - 1;
			r.left = 0;
			r.top = 0;

			hr = t[0]->LockRect(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//set byte
			((BYTE*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] = (BYTE)i;

		//		unlock layer
			hr = t[0]->UnlockRect(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);

		}

	//		create second huge texture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
		hr = g_pd3dd8->CreateTexture(4096, 4096, 13, 0, D3DFMT_A8, D3DPOOL_DEFAULT, &t[1]);
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
			hr = t[1]->LockRect(coords[i].layer, &l, &r, 0); 
			CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);

		//compare byte
			if(((BYTE*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != (BYTE)i) isbad = true;

		//		unlock layer
			hr = t[1]->UnlockRect(coords[i].layer);
			CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture*/);
		isbad = false;

	//		create smaller texture (256x256, full mips, 8bits/pixel), 
		hr = g_pd3dd8->CreateTexture(256, 256, 0, 0, D3DFMT_A8, D3DPOOL_DEFAULT, &t[2]);
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
				hr = t[1]->LockRect(coords[i].layer -4, &l, &r, 0); 
				CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
		
			//compare byte
			if(((BYTE*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != i) isbad = true;
		
			//		unlock layer
				hr = t[1]->UnlockRect(coords[i].layer -4);
				CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
			}
		}
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture to smaller texture*/);
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

		//		use UpdateTexture to copy second texture to third
		hr = g_pd3dd8->UpdateTexture(t[1], t[2]);
		CHECKRESULT(hr == D3D_OK /*Update huge->small again*/);

	//		check bytes in third texture again
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
		CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture to smaller texture*/);
		isbad = false;

	cleanup:
		for (i = 0; i < 3; i++)
		{
			if(t[i]) {
				hr = t[i]->Release();
				CHECKRESULT(hr == 0);
				t[i] = 0;
			}
		}
		
		CHECKLEAKS();
		xEndVariation(hlog);
	}
	ENDTEST()

	#if 0
		BEGINTEST(testTextureUpdateTextureNonSquare)
		{
			/*
			This test does the following:
				create huge texture map (4096x1024x16bit), full mips 
				write random pixels to first texture (multiple layers)
				create second huge texture map (4096x1024x16it), full mips
				use UpdateTexture to copy first texture to second
				delete first texture
				check random pixels in second texture (unswizzled)
				create 256x64x16bit texture map, full mips
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

			IDirect3DTexture8* t[3];
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

			xStartVariation(hlog, "");
			STARTLEAKCHECK();

		//		create huge texture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
			hr = g_pd3dd8->CreateTexture(4096, 1024, 13, 0, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &t[0]);
			CHECKRESULT(hr == D3D_OK /*Create huge texture*/);
			if(WASBADRESULT()) {
				goto cleanup;
			}

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

		//		create second huge texture (4096x4096, 13 mip layers, 8bits/pixel == 22.3 mb), 
			hr = g_pd3dd8->CreateTexture(4096, 1024, 13, 0, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &t[1]);
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

		//		check bytes in second texture
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
			CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture*/);
			isbad = false;

		//		create smaller texture (256x256, full mips, 8bits/pixel), 
			hr = g_pd3dd8->CreateTexture(256, 64, 0, 0, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &t[2]);
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
					hr = t[1]->LockRect(coords[i].layer -4, &l, &r, 0); 
					CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
			
				//compare byte
				if(((WORD*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != i) isbad = true;
			
				//		unlock layer
					hr = t[1]->UnlockRect(coords[i].layer -4);
					CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
				}
			}
			CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture to smaller texture*/);
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

			//		use UpdateTexture to copy second texture to third
			hr = g_pd3dd8->UpdateTexture(t[1], t[2]);
			CHECKRESULT(hr == D3D_OK /*Update huge->small again*/);

		//		check bytes in third texture again
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

		BEGINTEST(testTextureUpdateTextureNonSquare2)
		{
			/*
			This test does the following:
				create huge texture map (16x2048x32bit), 6 mip layers 
				write random pixels to first texture (multiple layers)
				create second huge texture map (2048x16x32it), 6 mip layers
				use UpdateTexture to copy first texture to second
				delete first texture
				check random pixels in second texture (unswizzled)
				create 1x64 texture map, 1 layer
				copy lowest mip layers from texture2 to texture3
				check texture3
				change pixels in texture2
				copy lowest mip layer from texture2 to texture3
				check texture3
				delete texture2, texture3

			this checks:
				big texture
				32-bit
				1-layer mip copy
				non-square texture
				multiple copy from same source
			*/
			
			DWORD hr;

			IDirect3DTexture8* t[3];
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

			xStartVariation(hlog, "Texture UpdateTexture");
			STARTLEAKCHECK();

		//		create huge texture (16x1024, 6 mip layers, 32bits/pixel), 
			hr = g_pd3dd8->CreateTexture(16, 2048, 6, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t[0]);
			CHECKRESULT(hr == D3D_OK /*Create huge texture*/);
			if(WASBADRESULT()) {
				goto cleanup;
			}

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

		//		create second huge texture (4096x4096, 6 mip layers, 32bits/pixel), 
			hr = g_pd3dd8->CreateTexture(16, 2048, 6, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t[1]);
			CHECKRESULT(hr == D3D_OK /*Create second huge texture*/);
			if(WASBADRESULT()) {
				goto cleanup;
			}

		//		use UpdateTexture to copy first texture to second
			hr = g_pd3dd8->UpdateTexture(t[0], t[1]);
			CHECKRESULT(hr == D3D_OK /*Update on huge texture*/);

		//		delete first texture
			hr = t[0]->Release();
			t[0] = 0;
			CHECKRESULT(hr == 0 /*release first huge texture*/);

		//		check bytes in second texture
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
			CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture*/);
			isbad = false;

		//		create smaller texture (256x256, 1 layer, 32bits/pixel), 
			hr = g_pd3dd8->CreateTexture(1, 64, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &t[2]);
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
					hr = t[1]->LockRect(coords[i].layer -4, &l, &r, 0); 
					CHECKRESULT(hr == D3D_OK /*locking layer coords[i].layer*/);
			
				//compare byte
				if(((BYTE*)l.pBits)[coords[i].u + l.Pitch * coords[i].v] != i) isbad = true;
			
				//		unlock layer
					hr = t[1]->UnlockRect(coords[i].layer -4);
					CHECKRESULT(hr == D3D_OK /*unlocking layer coords[i].layer*/);
				}
			}
			CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture to smaller texture*/);
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

			//		use UpdateTexture to copy second texture to third
			hr = g_pd3dd8->UpdateTexture(t[1], t[2]);
			CHECKRESULT(hr == D3D_OK /*Update huge->small again*/);

		//		check bytes in third texture again
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
			CHECKRESULT(isbad == false /*checking pixels copied by UpdateTexture to smaller texture*/);
			isbad = false;

		cleanup:
			for(i = 0; i < 3; i++) {
				if(t[i] != NULL) {
					hr = t[i]->Release();
					CHECKRESULT(hr == 0);
				}
			}

			CHECKLEAKS();
			xEndVariation(hlog);
		}
		ENDTEST()
	#endif

#endif

#pragma data_seg()
