#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <winsock.h>
#include <d3dx8.h>
#include "transprt.h"
#include "util.h"
#include "typetran.h"
#include "server.h"

//***********************************************************************************
extern "C" void __cdecl M_SWC8_AddRef(LPDIRECT3DSWAPCHAIN8 pd3dsw, ULONG* pulRet) {

    *pulRet = pd3dsw->AddRef();
}

//***********************************************************************************
extern "C" void __cdecl M_SWC8_Release(LPDIRECT3DSWAPCHAIN8 pd3dsw, ULONG* pulRet) {

    *pulRet = g_pServer->ReleaseObject(pd3dsw);
}

//***********************************************************************************
extern "C" void __cdecl M_SWC8_Present(LPDIRECT3DSWAPCHAIN8 pd3dsw, HRESULT* phr, RECT* prectSrc, RECT* prectDst, HWND hDstWindowOverride, RGNDATA* prgnDirtyRegion) {

    if (prectSrc->left == -1) {
        prectSrc = NULL;
    }
    if (prectDst->left == -1) {
        prectDst = NULL;
    }
    if (prgnDirtyRegion->rdh.nCount == 0) {
        prgnDirtyRegion = NULL;
    }
    *phr = pd3dsw->Present(prectSrc, prectDst, hDstWindowOverride, prgnDirtyRegion);
}

//***********************************************************************************
extern "C" void __cdecl M_SWC8_GetBackBuffer(LPDIRECT3DSWAPCHAIN8 pd3dsw, HRESULT* phr, UINT uBackBuffer, D3DBACKBUFFER_TYPE bbt, LPDIRECT3DSURFACE8* ppBackBuffer) {

    REMAPIN(D3DTI_D3DBACKBUFFER_TYPE, bbt);
    *phr = pd3dsw->GetBackBuffer(uBackBuffer, bbt, ppBackBuffer);
}
