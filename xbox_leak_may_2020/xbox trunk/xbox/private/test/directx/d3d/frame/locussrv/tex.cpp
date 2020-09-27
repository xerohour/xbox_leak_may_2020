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
extern "C" void __cdecl M_TEX8_GetLevelDesc(LPDIRECT3DTEXTURE8 pd3dt, HRESULT* phr, UINT uLevel, D3DSURFACE_DESC* pd3dsd) {

    *phr = pd3dt->GetLevelDesc(uLevel, pd3dsd);
    g_pServer->RemapStruct(FALSE, pd3dsd);
}

//***********************************************************************************
extern "C" void __cdecl M_TEX8_LockRect(LPDIRECT3DTEXTURE8 pd3dt, HRESULT* phr, UINT uLevel, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags) {

    if (prect->left == -1) {
        prect = NULL;
    }
    dwFlags &= ~XBOX_D3DLOCK_FLAGS;
    *phr = pd3dt->LockRect(uLevel, pd3dlr, prect, dwFlags);
}

//***********************************************************************************
extern "C" void __cdecl M_TEX8_UnlockRect(LPDIRECT3DTEXTURE8 pd3dt, HRESULT* phr, UINT uLevel, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags, LPBYTE pBits) {

    if (!(dwFlags & D3DLOCK_READONLY)) {

        D3DSURFACE_DESC d3dsd;
        DWORD           dwWidth, dwHeight, dwBitDepth;
        UINT            i;
        pd3dt->GetLevelDesc(uLevel, &d3dsd);
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

    *phr = pd3dt->UnlockRect(uLevel);
}

//***********************************************************************************
extern "C" void __cdecl M_TEX8_AddDirtyRect(LPDIRECT3DTEXTURE8 pd3dt, HRESULT* phr, RECT* prectDirty) {

    if (prectDirty->left == -1) {
        prectDirty = NULL;
    }
    *phr = pd3dt->AddDirtyRect(prectDirty);
}

//***********************************************************************************
extern "C" void __cdecl M_TEX8_GetSurfaceLevel(LPDIRECT3DTEXTURE8 pd3dt, HRESULT* phr, UINT uLevel, LPDIRECT3DSURFACE8* ppSurface) {

    *phr = pd3dt->GetSurfaceLevel(uLevel, ppSurface);
}

