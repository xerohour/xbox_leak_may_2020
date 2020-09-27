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
extern "C" void __cdecl M_D3D8_AddRef(LPDIRECT3D8 pd3d, ULONG* pulRet) {

    *pulRet = pd3d->AddRef();
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_Release(LPDIRECT3D8 pd3d, ULONG* pulRet) {

    *pulRet = g_pServer->ReleaseObject(pd3d);
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_GetAdapterCount(LPDIRECT3D8 pd3d, UINT* puRet) {

    *puRet = pd3d->GetAdapterCount();
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_GetAdapterDisplayMode(LPDIRECT3D8 pd3d, HRESULT* phr, UINT uAdapter, D3DDISPLAYMODE* pd3ddm) {

    *phr = pd3d->GetAdapterDisplayMode(uAdapter, pd3ddm);
    REMAPOUT(D3DTI_D3DFORMAT, pd3ddm->Format);
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_GetAdapterIdentifier(LPDIRECT3D8 pd3d, HRESULT* phr, UINT uAdapter, DWORD dwFlags, D3DADAPTER_IDENTIFIER8* pd3dai) {

    *phr = pd3d->GetAdapterIdentifier(uAdapter, dwFlags, pd3dai);
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_GetAdapterModeCount(LPDIRECT3D8 pd3d, UINT* puRet, UINT uAdapter) {

    *puRet = pd3d->GetAdapterModeCount(uAdapter);
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_GetAdapterMonitor(LPDIRECT3D8 pd3d, HMONITOR* phmon, UINT uAdapter) {

    *phmon = pd3d->GetAdapterMonitor(uAdapter);
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_GetDeviceCaps(LPDIRECT3D8 pd3d, HRESULT* phr, UINT uAdapter, D3DDEVTYPE d3ddt, D3DCAPS8* pd3dcaps) {

    REMAPIN(D3DTI_D3DDEVTYPE, d3ddt);
    *phr = pd3d->GetDeviceCaps(uAdapter, d3ddt, pd3dcaps);
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_EnumAdapterModes(LPDIRECT3D8 pd3d, HRESULT* phr, UINT uAdapter, UINT uMode, D3DDISPLAYMODE* pd3ddm) {

    *phr = pd3d->EnumAdapterModes(uAdapter, uMode, pd3ddm);
    REMAPOUT(D3DTI_D3DFORMAT, pd3ddm->Format);
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_RegisterSoftwareDevice(LPDIRECT3D8 pd3d, HRESULT* phr, void* pfnInitialize) {

    *phr = pd3d->RegisterSoftwareDevice(pfnInitialize);
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_CheckDeviceFormat(LPDIRECT3D8 pd3d, HRESULT* phr, UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtAdapter, DWORD dwUsage, D3DRESOURCETYPE d3drt, D3DFORMAT fmtCheck) {

    REMAPIN(D3DTI_D3DDEVTYPE, d3ddt);
    REMAPIN(D3DTI_D3DFORMAT, fmtAdapter);
    REMAPIN(D3DTI_D3DRESOURCETYPE, d3drt);
    REMAPIN(D3DTI_D3DFORMAT, fmtCheck);
    *phr = pd3d->CheckDeviceFormat(uAdapter, d3ddt, fmtAdapter, dwUsage, d3drt, fmtCheck);
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_CheckDeviceMultiSampleType(LPDIRECT3D8 pd3d, HRESULT* phr, UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtRenderTarget, BOOL bWindowed, D3DMULTISAMPLE_TYPE d3dmst) {

    REMAPIN(D3DTI_D3DDEVTYPE, d3ddt);
    REMAPIN(D3DTI_D3DFORMAT, fmtRenderTarget);
    REMAPIN(D3DTI_D3DMULTISAMPLE_TYPE, d3dmst);
    *phr = pd3d->CheckDeviceMultiSampleType(uAdapter, d3ddt, fmtRenderTarget, bWindowed, d3dmst);
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_CheckDeviceType(LPDIRECT3D8 pd3d, HRESULT* phr, UINT uAdapter, D3DDEVTYPE d3ddtCheck, D3DFORMAT fmtDisplayMode, D3DFORMAT fmtBackBuffer, BOOL bWindowed) {

    REMAPIN(D3DTI_D3DDEVTYPE, d3ddtCheck);
    REMAPIN(D3DTI_D3DFORMAT, fmtDisplayMode);
    REMAPIN(D3DTI_D3DFORMAT, fmtBackBuffer);
    *phr = pd3d->CheckDeviceType(uAdapter, d3ddtCheck, fmtDisplayMode, fmtBackBuffer, bWindowed);
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_CheckDepthStencilMatch(LPDIRECT3D8 pd3d, HRESULT* phr, UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtAdapter, D3DFORMAT fmtrRenderTarget, D3DFORMAT fmtdDepthStencil) {

    REMAPIN(D3DTI_D3DDEVTYPE, d3ddt);
    REMAPIN(D3DTI_D3DFORMAT, fmtAdapter);
    REMAPIN(D3DTI_D3DFORMAT, fmtrRenderTarget);
    REMAPIN(D3DTI_D3DFORMAT, fmtdDepthStencil);
    *phr = pd3d->CheckDepthStencilMatch(uAdapter, d3ddt, fmtAdapter, fmtrRenderTarget, fmtdDepthStencil);
}

//***********************************************************************************
extern "C" void __cdecl M_D3D8_CreateDevice(LPDIRECT3D8 pd3d, HRESULT* phr, UINT uAdapter, D3DDEVTYPE d3ddt, HWND hWndFocus, DWORD dwBehaviorFlags, D3DPRESENT_PARAMETERS* pd3dpp, LPDIRECT3DDEVICE8* pd3dd) {

    REMAPIN(D3DTI_D3DDEVTYPE, d3ddt);
    g_pServer->RemapStruct(TRUE, pd3dpp);
    pd3dpp->hDeviceWindow = g_hWndD3D;
    *phr = pd3d->CreateDevice(uAdapter, d3ddt, g_hWndD3D, dwBehaviorFlags, pd3dpp, pd3dd);
}
