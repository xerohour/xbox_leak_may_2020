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
extern "C" void __cdecl M_SUR8_AddRef(LPDIRECT3DSURFACE8 pd3ds, ULONG* pulRet) {

    *pulRet = pd3ds->AddRef();
}

//***********************************************************************************
extern "C" void __cdecl M_SUR8_Release(LPDIRECT3DSURFACE8 pd3ds, ULONG* pulRet) {

    *pulRet = g_pServer->ReleaseObject(pd3ds);
}

//***********************************************************************************
extern "C" void __cdecl M_SUR8_GetContainer(LPDIRECT3DSURFACE8 pd3ds, HRESULT* phr, LPGUID pguid, void** ppContainer) {

    *phr = pd3ds->GetContainer(*pguid, ppContainer);
}

//***********************************************************************************
extern "C" void __cdecl M_SUR8_GetDevice(LPDIRECT3DSURFACE8 pd3ds, HRESULT* phr, LPDIRECT3DDEVICE8* ppDevice) {

    *phr = pd3ds->GetDevice(ppDevice);
}

//***********************************************************************************
extern "C" void __cdecl M_SUR8_GetDesc(LPDIRECT3DSURFACE8 pd3ds, HRESULT* phr, D3DSURFACE_DESC* pd3dsd) {

    *phr = pd3ds->GetDesc(pd3dsd);
    g_pServer->RemapStruct(FALSE, pd3dsd);
}

//***********************************************************************************
extern "C" void __cdecl M_SUR8_LockRect(LPDIRECT3DSURFACE8 pd3ds, HRESULT* phr, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags) {

    if (prect->left == -1) {
        prect = NULL;
    }
    dwFlags &= ~XBOX_D3DLOCK_FLAGS;
    *phr = pd3ds->LockRect(pd3dlr, prect, dwFlags);
}

//***********************************************************************************
extern "C" void __cdecl M_SUR8_UnlockRect(LPDIRECT3DSURFACE8 pd3ds, HRESULT* phr, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags, LPBYTE pBits) {

    if (!(dwFlags & D3DLOCK_READONLY)) {

        D3DSURFACE_DESC d3dsd;
        DWORD           dwWidth, dwHeight, dwBitDepth;
        UINT            i;
        pd3ds->GetDesc(&d3dsd);
        if (prect->left == -1) {
            dwWidth = d3dsd.Width;
            dwHeight = d3dsd.Height;
        }
        else {
            dwWidth = prect->right - prect->left;
            dwHeight = prect->bottom - prect->top;
        }
        dwBitDepth = FormatToBitDepth(d3dsd.Format);
        if (IsCompressedFormat(d3dsd.Format)) {
            memcpy(pd3dlr->pBits, pBits, dwWidth * dwHeight * dwBitDepth / 8);
        }
        else {
            for (i = 0; i < dwHeight; i++) {
                memcpy((LPBYTE)pd3dlr->pBits + i * pd3dlr->Pitch, pBits + i * dwWidth * dwBitDepth / 8, dwWidth * dwBitDepth / 8);
            }
        }
    }

    *phr = pd3ds->UnlockRect();
}

//***********************************************************************************
extern "C" void __cdecl M_SUR8_GetPrivateData(LPDIRECT3DSURFACE8 pd3ds, HRESULT* phr, GUID* prefguid, void* pvData, DWORD* pdwSizeOfData) {

    *phr = pd3ds->GetPrivateData(*prefguid, pvData, pdwSizeOfData);
}

//***********************************************************************************
extern "C" void __cdecl M_SUR8_SetPrivateData(LPDIRECT3DSURFACE8 pd3ds, HRESULT* phr, GUID* prefguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

    *phr = pd3ds->SetPrivateData(*prefguid, pvData, dwSizeOfData, dwFlags);
}

//***********************************************************************************
extern "C" void __cdecl M_SUR8_FreePrivateData(LPDIRECT3DSURFACE8 pd3ds, HRESULT* phr, GUID* prefguid) {

    *phr = pd3ds->FreePrivateData(*prefguid);
}

