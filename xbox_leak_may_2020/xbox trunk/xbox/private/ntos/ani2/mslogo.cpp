#ifdef STARTUPANIMATION

#include "ntos.h"

#include "precomp.h"
#include "mslogo.h"
#include "renderer.h"
#include "xbs_app.h"
#include "xbs_math.h"

#pragma pack(1)

typedef struct _IMAGE_RUN1 {
    UINT fOne : 1;
    UINT Size : 3;
    UINT Intensity : 4;
} IMAGE_RUN1;

typedef struct _IMAGE_RUN2 {
    UINT fOne : 1;
    UINT fTwo : 1;
    UINT Size : 10;
    UINT Intensity : 4;
} IMAGE_RUN2;

#pragma pack()

// MS Logo sync event
extern "C" {
INITIALIZED_KEVENT(g_EventLogo, NotificationEvent, FALSE);
INITIALIZED_KEVENT(g_EventLogoWaiting, NotificationEvent, FALSE);
}

IDirect3DVertexBuffer8 *g_pVBMicrosoftLogo;
IDirect3DTexture8* g_pTexMicrosoftLogo;
PBYTE g_ImageLogo;
UINT g_ImageLogoSize;

typedef struct _TheVerts {
    float x,y,z,w;
    float u, v;
} TheVerts;

const TheVerts Verts[] =
{
    {270.0f,  400.0f, 0.5f, 1.0f,   0.0f,   0.0f},
    {370.0f,  417.0f, 0.5f, 1.0f, 100.0f,  17.0f},
    {270.0f,  417.0f, 0.5f, 1.0f,   0.0f,  17.0f},
    {370.0f,  400.0f, 0.5f, 1.0f, 100.0f,   0.0f},
    {370.0f,  417.0f, 0.5f, 1.0f, 100.0f,  17.0f},
    {270.0f,  400.0f, 0.5f, 1.0f,   0.0f,   0.0f},
};

BOOL InitVB(IDirect3DDevice8 *pDev)
{
    IDirect3DVertexBuffer8 *pVB;
    void *pVerts;

    if (FAILED(IDirect3DDevice8_CreateVertexBuffer(pDev,
            sizeof(Verts),
            D3DUSAGE_WRITEONLY,
            D3DFVF_XYZRHW | D3DFVF_TEX1,
            D3DPOOL_MANAGED,
            &pVB)))
    {
        return FALSE;
    }

    g_pVBMicrosoftLogo = pVB;

    IDirect3DVertexBuffer8_Lock(pVB, 0, sizeof(Verts), (BYTE **)(&pVerts), 0);
    memcpy((void*)pVerts, (void*)Verts, sizeof(Verts));
    IDirect3DVertexBuffer8_Unlock(pVB);

    return TRUE;
}

const float F_15 = 15.0f;
const float F_P_5 = 0.5f;

COLORREF ColorFromIntensity(UINT intensity4bits)
{
    UINT Base = 0xCC;

    UINT cr = (UINT)((Base * intensity4bits) / F_15 + F_P_5);

    return D3DCOLOR_ARGB(0xFF, cr, cr, cr);
}

BOOL InitTexture(IDirect3DDevice8 *pDev)
{
    IDirect3DTexture8* pTex;
    D3DLOCKED_RECT lock;
    UINT x = 0, i, j;
    DWORD dwLine;
    DWORD dwAddr;
    IMAGE_RUN1 *pRun1;
    IMAGE_RUN2 *pRun2;
    UINT Size;
    UINT Intensity;

    if (FAILED(IDirect3DDevice8_CreateTexture(pDev,
            100,
            17,
            1,
            0,
            D3DFMT_LIN_X8R8G8B8,
            0,
            &pTex)))
    {
        return FALSE;
    }

    g_pTexMicrosoftLogo = pTex;

    IDirect3DTexture8_LockRect(pTex, 0, &lock, NULL, 0);
    dwLine = (DWORD)lock.pBits;
    dwAddr = dwLine;
    i = 0;

    while(i < g_ImageLogoSize)
    {
        Size = 0;
        Intensity = 0;

        pRun1 = (IMAGE_RUN1*)&g_ImageLogo[i];
        if (pRun1->fOne)
        {
            Size = pRun1->Size;
            Intensity = pRun1->Intensity;
            i += 1;
        }
        else
        {
            pRun2 = (IMAGE_RUN2*)&g_ImageLogo[i];
            if (pRun2->fTwo)
            {
                Size = pRun2->Size;
                Intensity = pRun2->Intensity;
                i += 2;
            }
        }

        for (j = 0; j < Size; j++)
        {
            if (Intensity == 0)
            {
                *(DWORD*)dwAddr = 0xFF000000;
            }
            else
            {
                *(DWORD*)dwAddr = ColorFromIntensity(Intensity);
            }
            if (x >= 99)
            {
                x = 0;
                dwLine += lock.Pitch;
                dwAddr = dwLine;
            }
            else
            {
                x++;
                dwAddr += sizeof(DWORD);
            }
        }
    }

    IDirect3DTexture8_UnlockRect(pTex, 0);

    return TRUE;
}

void UnInitMicrosoftLogo()
{
    if (g_ImageLogo != NULL)
    {
        MemFree(g_ImageLogo);
    }

    if (g_pVBMicrosoftLogo != NULL)
    {
        g_pVBMicrosoftLogo->Release();
    }

    if (g_pTexMicrosoftLogo != NULL)
    {
        g_pTexMicrosoftLogo->Release();
    }
}

void RenderMicrosoftLogo()
{
    if (g_ImageLogo == NULL)
    {
        return;
    }

    if (!InitVB(gpd3dDev) || !InitTexture(gpd3dDev))
    {
        UnInitMicrosoftLogo();
        return;
    }

    IDirect3DDevice8 *pDev = gpd3dDev;
    IDirect3DSurface8 *pSurface;
    pDev->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pSurface);
    pDev->SetRenderTarget(pSurface, NULL);

    const DWORD RenderStates[] =
    {
        D3DRS_FILLMODE,         D3DFILL_SOLID,
        D3DRS_BACKFILLMODE,     D3DFILL_SOLID,
        D3DRS_CULLMODE,         D3DCULL_NONE,
        D3DRS_DITHERENABLE,     TRUE,
        D3DRS_ALPHATESTENABLE,  FALSE,
        D3DRS_ALPHABLENDENABLE, FALSE,
        D3DRS_FOGENABLE,        FALSE,
        D3DRS_EDGEANTIALIAS,    FALSE,
        D3DRS_STENCILENABLE,    FALSE,
        D3DRS_LIGHTING,         FALSE,
        D3DRS_MULTISAMPLEMASK,  0xffffffff,
        D3DRS_LOGICOP,          D3DLOGICOP_NONE,
        D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL,
        D3DRS_YUVENABLE,        FALSE,
    };
    
    const DWORD TextureStates[] =
    {
        D3DTSS_COLOROP,         D3DTOP_SELECTARG1,
        D3DTSS_COLORARG1,       D3DTA_TEXTURE,
        D3DTSS_ALPHAOP,         D3DTOP_DISABLE,
        D3DTSS_TEXCOORDINDEX,   0,
        D3DTSS_ADDRESSU,        D3DTADDRESS_CLAMP,
        D3DTSS_ADDRESSV,        D3DTADDRESS_CLAMP,
        D3DTSS_COLORKEYOP,      D3DTCOLORKEYOP_DISABLE,
        D3DTSS_COLORSIGN,       0,
        D3DTSS_ALPHAKILL,       D3DTALPHAKILL_DISABLE,
        D3DTSS_MINFILTER,       D3DTEXF_LINEAR,
        D3DTSS_MAGFILTER,       D3DTEXF_LINEAR,
    };

    int i;

    for (i = 0; i < sizeof(RenderStates) / (sizeof(DWORD) * 2); i++)
    {
        pDev->SetRenderState((D3DRENDERSTATETYPE)RenderStates[2*i],
                RenderStates[2*i + 1]);
    }

    for (i = 0; i < sizeof(TextureStates) / (sizeof(DWORD) * 2); i++)
    {
        pDev->SetTextureStageState(0,
                (D3DTEXTURESTAGESTATETYPE)TextureStates[2*i],
                TextureStates[2*i + 1]);
    }

    IDirect3DDevice8_SetStreamSource(pDev, 0, g_pVBMicrosoftLogo, sizeof(Verts[0]));
    IDirect3DDevice8_SetVertexShader(pDev, D3DFVF_XYZRHW | D3DFVF_TEX1);
    IDirect3DDevice8_SetTexture(pDev, 0, (D3DBaseTexture*)g_pTexMicrosoftLogo);

    IDirect3DDevice8_DrawPrimitive(pDev,
            D3DPT_TRIANGLELIST,
            0,
            2);

    pSurface->Release();
    UnInitMicrosoftLogo();
    pDev->PersistDisplay();
}

extern "C"
VOID AniSetLogo(PVOID pv, ULONG Size)
{
    if (KeReadStateEvent(&g_EventLogo) == 0)
    {
        if (gpd3dDev != NULL && pv != NULL && Size <= PAGE_SIZE)
        {
            g_ImageLogo = (PBYTE)MemAlloc(Size);
            if (g_ImageLogo != NULL)
            {
                memcpy(g_ImageLogo, pv, Size);
                g_ImageLogoSize = Size;
            }
        }

        KeSetEvent(&g_EventLogo, EVENT_INCREMENT, FALSE);
    }
}

void WaitOnMicrosoftLogo()
{
    // Signal the initialization thread that we're waiting for the logo.  Note
    // that this returns with the dispatcher database lock held, which the
    // following wait call will release.  This avoids an unnecessary context
    // switch.
    KeSetEvent(&g_EventLogoWaiting, EVENT_INCREMENT, TRUE);

    KeWaitForSingleObject(&g_EventLogo, Executive, KernelMode, FALSE, NULL);
    RenderMicrosoftLogo();
}

#endif // STARTUPANIMATION

