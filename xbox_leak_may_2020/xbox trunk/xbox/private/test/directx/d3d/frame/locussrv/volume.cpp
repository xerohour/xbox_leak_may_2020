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
extern "C" void __cdecl M_VOL8_AddRef(LPDIRECT3DVOLUME8 pd3dv, ULONG* pulRet) {

    *pulRet = pd3dv->AddRef();
}

//***********************************************************************************
extern "C" void __cdecl M_VOL8_Release(LPDIRECT3DVOLUME8 pd3dv, ULONG* pulRet) {

    *pulRet = g_pServer->ReleaseObject(pd3dv);
}

//***********************************************************************************
extern "C" void __cdecl M_VOL8_GetContainer(LPDIRECT3DVOLUME8 pd3dv, HRESULT* phr, LPGUID pguid, void** ppContainer) {

    *phr = pd3dv->GetContainer(*pguid, ppContainer);
}

//***********************************************************************************
extern "C" void __cdecl M_VOL8_GetDevice(LPDIRECT3DVOLUME8 pd3dv, HRESULT* phr, LPDIRECT3DDEVICE8* ppDevice) {

    *phr = pd3dv->GetDevice(ppDevice);
}

//***********************************************************************************
extern "C" void __cdecl M_VOL8_GetDesc(LPDIRECT3DVOLUME8 pd3dv, HRESULT* phr, D3DVOLUME_DESC* pd3dvd) {

    *phr = pd3dv->GetDesc(pd3dvd);
    g_pServer->RemapStruct(FALSE, pd3dvd);
}

//***********************************************************************************
extern "C" void __cdecl M_VOL8_LockBox(LPDIRECT3DVOLUME8 pd3dv, HRESULT* phr, D3DLOCKED_BOX* pd3dlb, D3DBOX* pbox, DWORD dwFlags) {

    if (pbox->Left == -1) {
        pbox = NULL;
    }
    dwFlags &= ~XBOX_D3DLOCK_FLAGS;
    *phr = pd3dv->LockBox(pd3dlb, pbox, dwFlags);
}

//***********************************************************************************
extern "C" void __cdecl M_VOL8_UnlockBox(LPDIRECT3DVOLUME8 pd3dv, HRESULT* phr, D3DLOCKED_BOX* pd3dlb, D3DBOX* pbox, DWORD dwFlags, LPBYTE pBits) {

    if (!(dwFlags & D3DLOCK_READONLY)) {

        D3DVOLUME_DESC  d3dvd;
        DWORD           dwWidth, dwHeight, dwDepth, dwBitDepth;
        UINT            i, j;
        pd3dv->GetDesc(&d3dvd);
        if (pbox->Left == -1) {
            dwWidth = d3dvd.Width;
            dwHeight = d3dvd.Height;
            dwDepth = d3dvd.Depth;
        }
        else {
            dwWidth = pbox->Right - pbox->Left;
            dwHeight = pbox->Bottom - pbox->Top;
            dwDepth = pbox->Back - pbox->Front;
        }
        dwBitDepth = FormatToBitDepth(d3dvd.Format);
        if (IsCompressedFormat(d3dvd.Format)) {
            memcpy(pd3dlb->pBits, pBits, dwWidth * dwHeight * dwDepth * dwBitDepth / 8);
        }
        else {
            for (j = 0; j < dwDepth; j++) {
                for (i = 0; i < dwHeight; i++) {
                    memcpy((LPBYTE)pd3dlb->pBits + j * pd3dlb->SlicePitch + i * pd3dlb->RowPitch, pBits + j * dwHeight * dwWidth * dwBitDepth / 8 + i * dwWidth * dwBitDepth / 8, dwWidth * dwBitDepth / 8);
                }
            }
        }
    }

    *phr = pd3dv->UnlockBox();
}

//***********************************************************************************
extern "C" void __cdecl M_VOL8_GetPrivateData(LPDIRECT3DVOLUME8 pd3dv, HRESULT* phr, GUID* prefguid, void* pvData, DWORD* pdwSizeOfData) {

    *phr = pd3dv->GetPrivateData(*prefguid, pvData, pdwSizeOfData);
}

//***********************************************************************************
extern "C" void __cdecl M_VOL8_SetPrivateData(LPDIRECT3DVOLUME8 pd3dv, HRESULT* phr, GUID* prefguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

    *phr = pd3dv->SetPrivateData(*prefguid, pvData, dwSizeOfData, dwFlags);
}

//***********************************************************************************
extern "C" void __cdecl M_VOL8_FreePrivateData(LPDIRECT3DVOLUME8 pd3dv, HRESULT* phr, GUID* prefguid) {

    *phr = pd3dv->FreePrivateData(*prefguid);
}

