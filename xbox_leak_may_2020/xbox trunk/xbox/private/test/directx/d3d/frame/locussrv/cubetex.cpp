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
extern "C" void __cdecl M_CTX8_GetLevelDesc(LPDIRECT3DCUBETEXTURE8 pd3dtc, HRESULT* phr, UINT uLevel, D3DSURFACE_DESC* pd3dsd) {

    *phr = pd3dtc->GetLevelDesc(uLevel, pd3dsd);
    g_pServer->RemapStruct(FALSE, pd3dsd);
}

//***********************************************************************************
extern "C" void __cdecl M_CTX8_LockRect(LPDIRECT3DCUBETEXTURE8 pd3dtc, HRESULT* phr, D3DCUBEMAP_FACES d3dcf, UINT uLevel, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags) {

    REMAPIN(D3DTI_D3DCUBEMAP_FACES, d3dcf);
    if (prect->left == -1) {
        prect = NULL;
    }
    dwFlags &= ~XBOX_D3DLOCK_FLAGS;
    *phr = pd3dtc->LockRect(d3dcf, uLevel, pd3dlr, prect, dwFlags);
}

//***********************************************************************************
extern "C" void __cdecl M_CTX8_UnlockRect(LPDIRECT3DCUBETEXTURE8 pd3dtc, HRESULT* phr, D3DCUBEMAP_FACES d3dcf, UINT uLevel, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags, LPBYTE pBits) {

    REMAPIN(D3DTI_D3DCUBEMAP_FACES, d3dcf);

    if (!(dwFlags & D3DLOCK_READONLY)) {

        D3DSURFACE_DESC d3dsd;
        DWORD           dwWidth, dwHeight, dwBitDepth;
        UINT            i;
        pd3dtc->GetLevelDesc(uLevel, &d3dsd);
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

    *phr = pd3dtc->UnlockRect(d3dcf, uLevel);
}

//***********************************************************************************
extern "C" void __cdecl M_CTX8_AddDirtyRect(LPDIRECT3DCUBETEXTURE8 pd3dtc, HRESULT* phr, D3DCUBEMAP_FACES d3dcf, RECT* prectDirty) {

    REMAPIN(D3DTI_D3DCUBEMAP_FACES, d3dcf);
    if (prectDirty->left == -1) {
        prectDirty = NULL;
    }
    *phr = pd3dtc->AddDirtyRect(d3dcf, prectDirty);
}

//***********************************************************************************
extern "C" void __cdecl M_CTX8_GetCubeMapSurface(LPDIRECT3DCUBETEXTURE8 pd3dtc, HRESULT* phr, D3DCUBEMAP_FACES d3dcf, UINT uLevel, LPDIRECT3DSURFACE8* ppSurface) {

    REMAPIN(D3DTI_D3DCUBEMAP_FACES, d3dcf);
    *phr = pd3dtc->GetCubeMapSurface(d3dcf, uLevel, ppSurface);
}

