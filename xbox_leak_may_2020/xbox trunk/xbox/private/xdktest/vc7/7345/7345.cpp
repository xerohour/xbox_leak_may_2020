#include <xtl.h>

static struct
{
    struct
    {
        IDirect3DTexture8 TestTex;
        IDirect3DTexture8 *pTex;
    } TexList[100];
} glb;

void xbGfxCloseTexture(long texture)
{
    IDirect3DTexture8 *pTexture = glb.TexList[texture].pTex;
    static long foo_bar = pTexture ? pTexture->UnlockRect(0) : 0;
}

void __cdecl main()
{
    xbGfxCloseTexture(0);
    xbGfxCloseTexture(1);
}
