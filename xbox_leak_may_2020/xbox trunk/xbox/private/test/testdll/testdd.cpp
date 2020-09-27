#define NOD3D
#define NODSOUND
#include <xtl.h>
#undef  NOD3D
#undef  NODSOUND

#include <wingdi.h>
#include <ddraw.h>
#include "testdd.h"

void TestDirectDraw(void)
{
	HRESULT hr;
	LPDIRECTDRAW7 pDD;
    LPDIRECTDRAWSURFACE7 psurf = NULL, pback = NULL;
    DDSURFACEDESC2 ddsd;

    _asm int 3;
    hr = DirectDrawCreateEx(NULL, (VOID**)&pDD, IID_IDirectDraw7, NULL);
    if (pDD) {
        ZeroMemory(&ddsd, sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd); 
        ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT; 
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY |
                              DDSCAPS_FLIP | DDSCAPS_COMPLEX; 
        ddsd.dwBackBufferCount = 1;
        pDD->CreateSurface(&ddsd, &psurf, NULL);
        if (psurf) {
            DDSCAPS2 ddscaps;

            _asm int 3;
            ZeroMemory(&ddscaps, sizeof(ddscaps));
            ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
            hr = psurf->GetAttachedSurface(&ddscaps, &pback);

            if (pback) {
                pback->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

                DWORD *lpdw;

                lpdw = (DWORD*)ddsd.lpSurface;
                lpdw += 640 * 5;

                for (int i = 0; i < 50; i++)
                    *lpdw++ = -1;

                pback->Unlock(NULL);

                _asm int 3;
                for (i = 0; i < 10; i++) {
                    psurf->Flip(NULL, DDFLIP_WAIT);
                    Sleep(5000);
                }
                pback->Release();
            }
            psurf->Release();
        }
        pDD->Release();
    }
}