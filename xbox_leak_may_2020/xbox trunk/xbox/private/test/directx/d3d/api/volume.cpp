#include "d3dapi.hpp"
LINKME(3)

#ifndef ROUNDUP
#	define ROUNDUP(a,b)    ((((a)+(b)-1)/(b))*(b))
#endif

/*
static __inline ULONG   WINAPI IDirect3DVolume8_AddRef(IDirect3DVolume8 *pThis) { return IDirect3DResource8_AddRef((IDirect3DResource8 *)pThis); }
static __inline ULONG   WINAPI IDirect3DVolume8_Release(IDirect3DVolume8 *pThis) { return IDirect3DResource8_Release((IDirect3DResource8 *)pThis); }
//static __inline HRESULT WINAPI IDirect3DVolume8_GetDevice(IDirect3DVolume8 *pThis, IDirect3DDevice8 **ppDevice) { return IDirect3DResource8_GetDevice((IDirect3DResource8 *)pThis, ppDevice); }
//static __inline D3DRESOURCETYPE WINAPI IDirect3DVolume8_GetType(IDirect3DVolume8 *pThis) { return IDirect3DResource8_GetType((IDirect3DResource8 *)pThis); }
static __inline BOOLEAN WINAPI IDirect3DVolume8_IsBusy(IDirect3DVolume8 *pThis) { return IDirect3DResource8_IsBusy((IDirect3DResource8 *)pThis); }
static __inline VOID    WINAPI IDirect3DVolume8_MoveResourceMemory(IDirect3DVolume8 *pThis, D3DMEMORY where) { IDirect3DResource8_MoveResourceMemory((IDirect3DResource8 *)pThis, where); }
static __inline HRESULT WINAPI IDirect3DVolume8_SetPrivateData(IDirect3DVolume8 *pThis, REFGUID refguid, CONST void *pData, DWORD SizeOfData, DWORD Flags) { return IDirect3DResource8_SetPrivateData((IDirect3DResource8 *)pThis, refguid, pData, SizeOfData, Flags); }
static __inline HRESULT WINAPI IDirect3DVolume8_GetPrivateData(IDirect3DVolume8 *pThis, REFGUID refguid, void *pData, DWORD *pSizeOfData) { return IDirect3DResource8_GetPrivateData((IDirect3DResource8 *)pThis, refguid, pData, pSizeOfData); }
static __inline HRESULT WINAPI IDirect3DVolume8_FreePrivateData(IDirect3DVolume8 *pThis, REFGUID refguid) { return IDirect3DResource8_FreePrivateData((IDirect3DResource8 *)pThis, refguid); }

//HRESULT WINAPI IDirect3DVolume8_GetContainer(IDirect3DVolume8 *pThis, IDirect3DBaseTexture8 **ppBaseTexture);
//HRESULT WINAPI IDirect3DVolume8_GetDesc(IDirect3DVolume8 *pThis, D3DVOLUME_DESC *pDesc);
//HRESULT WINAPI IDirect3DVolume8_LockBox(IDirect3DVolume8 *pThis, D3DLOCKED_BOX *pLockedVolume, CONST D3DBOX *pBox, DWORD Flags);
//HRESULT WINAPI IDirect3DVolume8_UnlockBox(IDirect3DVolume8 *pThis);
*/


struct VSTUFF {
	int width;
	int height;
	int depth;
	D3DFORMAT f;
	int count;
	int bitdepth;
} vstuff[] = {
	{  64,  64,  64, D3DFMT_A8R8G8B8,  7, 32},
	{   4,   4,   4, D3DFMT_DXT1,      0,  4},
	{ 128, 128, 128, D3DFMT_R5G6B5,    4, 16},
	{  32, 128, 256, D3DFMT_A8R8G8B8,  2, 32},
	{   1,  64,  64, D3DFMT_A8R8G8B8,  7, 32},
	{  64,   1,  64, D3DFMT_A8R8G8B8,  7, 32},
	{  64,  64,   1, D3DFMT_A8R8G8B8,  7, 32},
};

#pragma data_seg(".d3dapi$test220") 


bool IsSwizzledD3DFORMAT(D3DFORMAT Format){
switch (Format)
{
case D3DFMT_A8R8G8B8:
case D3DFMT_R5G6B5:
case D3DFMT_A8:
return true;
case D3DFMT_DXT1:
case D3DFMT_LIN_A8R8G8B8:
case D3DFMT_LIN_R5G6B5:
case D3DFMT_LIN_A8:
return false;
default:
_asm {int 3};
}
}
namespace D3D {
//bool IsSwizzledD3DFORMAT(D3DFORMAT Format);
bool IsCompressedD3DFORMAT(D3DFORMAT Format);
}


BEGINTEST(testCreateVolume)
{
	DWORD hr;
	IDirect3DVolumeTexture8* t;
	IDirect3DVolume8* s;
	D3DVOLUME_DESC dd;
	int i; 
	unsigned int j;
	UINT w, h, d, w4,h4;

	xStartVariation(hlog, "VT::GetVolumeLevel");
	STARTLEAKCHECK();

	for(i = 0; i < COUNTOF(vstuff); i++) {
		hr = g_pd3dd8->CreateVolumeTexture(vstuff[i].width, vstuff[i].height, vstuff[i].depth, vstuff[i].count, 0, vstuff[i].f, D3DPOOL_DEFAULT, &t);
		w = vstuff[i].width; 
		h = vstuff[i].height;
		d = vstuff[i].depth;
		CHECKRESULT(hr == D3D_OK);
		for(j = 0; j < t->GetLevelCount(); j++) {
			t->GetVolumeLevel(j, &s);
			s->GetDesc(&dd);
			if(vstuff[i].f == D3DFMT_DXT1 || vstuff[i].f == D3DFMT_DXT2 || vstuff[i].f == D3DFMT_DXT4) {
				if(w < 4) w4 = 4; else w4 = w;
				if(h < 4) h4 = 4; else h4 = h;
            } else if (!IsSwizzledD3DFORMAT(vstuff[i].f)) {
				w4 = ROUNDUP(w, D3DTEXTURE_PITCH_MIN * 8 / vstuff[i].bitdepth);
				h4 = h;
			} else {
				w4 = w;
				h4 = h;
			}

			CHECKRESULT(dd.Size == w4 * h4 * d * vstuff[i].bitdepth / 8);
			CHECKRESULT(dd.Format == vstuff[i].f);
			w = (w >> 1) ? (w >> 1) : 1;
			h = (h >> 1) ? (h >> 1) : 1;
			d = (d >> 1) ? (d >> 1) : 1;
			hr = s->Release();
			CHECKRESULT(hr == 0);
		}
		CHECKRESULT(t->Release() == 0);
	}
	CHECKRESULT(dd.Type == D3DRTYPE_VOLUME);
	CHECKLEAKS();
	xEndVariation(hlog);

}
ENDTEST()

BEGINTEST(testVolumeGetContainer)
{
	DWORD hr;
	IDirect3DVolumeTexture8* t;
	IDirect3DVolume8* s;
	IDirect3DBaseTexture8* b;
	IDirect3DDevice8* de;
	int i;
	unsigned int j;
	UINT w, h, d;

	xStartVariation(hlog, " ");
	STARTLEAKCHECK();

	for(i = 0; i < COUNTOF(vstuff); i++) {
		hr = g_pd3dd8->CreateVolumeTexture(vstuff[i].width, vstuff[i].height, vstuff[i].depth, vstuff[i].count, 0, vstuff[i].f, D3DPOOL_DEFAULT, &t);
		w = vstuff[i].width; 
		h = vstuff[i].height;
		d = vstuff[i].depth;
		CHECKRESULT(hr == D3D_OK);
		for(j = 0; (j < t->GetLevelCount()); j++) {
			t->GetVolumeLevel(j, &s);
			s->GetContainer(&b);
			CHECKRESULT((void*)b == (void*)t);
			s->GetDevice(&de);
			CHECKRESULT(de == g_pd3dd8);
			de->Release();
			CHECKRESULT(D3DRTYPE_VOLUME == s->GetType());
			hr = s->Release();
			CHECKRESULT(hr == 0);
			hr = b->Release();
			CHECKRESULT(hr == 1);
		}
		CHECKRESULT(t->Release() == 0);
	}
//	CHECKRESULT(dd.Type == D3DRTYPE_VOLUMETEXTURE);
	CHECKLEAKS();
	xEndVariation(hlog);

}
ENDTEST()

/*
BEGINTEST(testVolumeGetDevice)
{
//see above
}
ENDTEST()
*/

/*
//see volumelock.cpp

BEGINTEST(testVolumeLockUnlock)
{
//create volume texture (width = x, height = y, depth = z, levels = l, format = f)
//grab random level from it
//getvolumelevel (that level)
//release that volume level
//lock (RAW)
//grab that volume level again
//set pixels in locked level
//unlock locked level
//lock volume level (unswizzled)
//check pixels
//write a few pixels
//unlock volume level
//lock texture (unswizzled)
//check for pixels
//unlock it
//release everything
	DWORD hr;
	IDirect3DVolumeTexture8* t;
	IDirect3DVolume8* s;
	IDirect3DBaseTexture8* b;
	IDirect3DDevice8* de;
	D3DVOLUME_DESC dd;
	int i;
	unsigned int j;
	UINT w, h, d;

	xStartVariation(hlog, " ");
	STARTLEAKCHECK();

	for(i = 0; i < COUNTOF(vstuff); i++) {
		hr = g_pd3dd8->CreateVolumeTexture(vstuff[i].width, vstuff[i].height, vstuff[i].depth, vstuff[i].count, 0, vstuff[i].f, D3DPOOL_DEFAULT, &t);
		w = vstuff[i].width; 
		h = vstuff[i].height;
		d = vstuff[i].depth;
		CHECKRESULT(hr == D3D_OK);
		hr = t->Release();
		CHECKRESULT(hr == 0);

	}

}
ENDTEST()
*/
#pragma data_seg()
