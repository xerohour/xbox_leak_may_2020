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
extern "C" void __cdecl M_VTX8_GetLevelDesc(LPDIRECT3DVOLUMETEXTURE8 pd3dtv, HRESULT* phr, UINT uLevel, D3DVOLUME_DESC* pd3dvd) {

    *phr = pd3dtv->GetLevelDesc(uLevel, pd3dvd);
    g_pServer->RemapStruct(FALSE, pd3dvd);
}

//***********************************************************************************
extern "C" void __cdecl M_VTX8_LockBox(LPDIRECT3DVOLUMETEXTURE8 pd3dtv, HRESULT* phr, UINT uLevel, D3DLOCKED_BOX* pd3dlb, D3DBOX* pbox, DWORD dwFlags) {

    if (pbox->Left == -1) {
        pbox = NULL;
    }
    dwFlags &= ~XBOX_D3DLOCK_FLAGS;
    *phr = pd3dtv->LockBox(uLevel, pd3dlb, pbox, dwFlags);
}

//***********************************************************************************
extern "C" void __cdecl M_VTX8_UnlockBox(LPDIRECT3DVOLUMETEXTURE8 pd3dtv, HRESULT* phr, UINT uLevel, D3DLOCKED_BOX* pd3dlb, D3DBOX* pbox, DWORD dwFlags, LPBYTE pBits) {

    if (!(dwFlags & D3DLOCK_READONLY)) {

        D3DVOLUME_DESC  d3dvd;
        DWORD           dwWidth, dwHeight, dwDepth, dwBitDepth;
        UINT            i, j;
        pd3dtv->GetLevelDesc(uLevel, &d3dvd);
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

    *phr = pd3dtv->UnlockBox(uLevel);
}

//***********************************************************************************
extern "C" void __cdecl M_VTX8_AddDirtyBox(LPDIRECT3DVOLUMETEXTURE8 pd3dtv, HRESULT* phr, D3DBOX* pboxDirty) {

    if (pboxDirty->Left == -1) {
        pboxDirty = NULL;
    }
    *phr = pd3dtv->AddDirtyBox(pboxDirty);
}

//***********************************************************************************
extern "C" void __cdecl M_VTX8_GetVolumeLevel(LPDIRECT3DVOLUMETEXTURE8 pd3dtv, HRESULT* phr, UINT uLevel, LPDIRECT3DVOLUME8* ppVolume) {

    *phr = pd3dtv->GetVolumeLevel(uLevel, ppVolume);
}

