#include "d3dapi.hpp"

LINKME(8)

#pragma data_seg(".d3dapi$test070") 

/*
	HRESULT WINAPI IDirect3DTexture8_SetPrivateData(IDirect3DTexture8 *pThis, REFGUID refguid, CONST void *pData, DWORD SizeOfData, DWORD Flags) { return IDirect3DResource8_SetPrivateData((IDirect3DResource8 *)pThis, refguid, pData, SizeOfData, Flags); }
	HRESULT WINAPI IDirect3DTexture8_GetPrivateData(IDirect3DTexture8 *pThis, REFGUID refguid, void *pData, DWORD *pSizeOfData) { return IDirect3DResource8_GetPrivateData((IDirect3DResource8 *)pThis, refguid, pData, pSizeOfData); }
	HRESULT WINAPI IDirect3DTexture8_FreePrivateData(IDirect3DTexture8 *pThis, REFGUID refguid) { return IDirect3DResource8_FreePrivateData((IDirect3DResource8 *)pThis, refguid); }
*/


#pragma data_seg()
