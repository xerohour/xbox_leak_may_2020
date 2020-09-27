//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
//#include "types.h"
#include "CStage.h"
#include "CTextureStage.h"
//#include "math.h"

extern CTextureStage* g_pTextureStage;

HRESULT CTextureStage::ValidateDevice8(DWORD *pdwPasses)
{
    HRESULT hRes=g_pTextureStage->m_pSrcDevice8->ValidateDevice(pdwPasses);

    if (FAILED(hRes))
    {
#if 0
        switch (hRes)
        {
            case D3DERR_CONFLICTINGTEXTUREFILTER:   g_TextureStage.WriteToLog("Validate Ref: Conflicting texture filter\n"); break;
            case D3DERR_CONFLICTINGTEXTUREPALETTE:  g_TextureStage.WriteToLog("Validate Ref: Conflicting texture palette\n"); break;
            case D3DERR_TOOMANYOPERATIONS:          g_TextureStage.WriteToLog("Validate Ref: Too many operations\n"); break;
            case D3DERR_UNSUPPORTEDALPHAARG:        g_TextureStage.WriteToLog("Validate Ref: Unsupported alpha arg\n"); break;
            case D3DERR_UNSUPPORTEDALPHAOPERATION:  g_TextureStage.WriteToLog("Validate Ref: Unsupported alpha op\n"); break;
            case D3DERR_UNSUPPORTEDCOLORARG:        g_TextureStage.WriteToLog("Validate Ref: Unsupported color arg\n"); break;
            case D3DERR_UNSUPPORTEDCOLOROPERATION:  g_TextureStage.WriteToLog("Validate Ref: Unsupported color op\n"); break;
            case D3DERR_UNSUPPORTEDFACTORVALUE:     g_TextureStage.WriteToLog("Validate Ref: Unsupported factor value\n"); break;
            case D3DERR_UNSUPPORTEDTEXTUREFILTER:   g_TextureStage.WriteToLog("Validate Ref: Unsupported texture filter\n"); break;
            case D3DERR_WRONGTEXTUREFORMAT:         g_TextureStage.WriteToLog("Validate Ref: Wrong texture format\n"); break;
        }
#endif
        return hRes;
    }

    hRes=g_pTextureStage->m_pSrcDevice8->ValidateDevice(pdwPasses);

    if (FAILED(hRes))
    {
#if 0
        switch (hRes)
        {
            case D3DERR_CONFLICTINGTEXTUREFILTER:  g_TextureStage.WriteToLog("Validate Src: Conflicting texture filter\n"); break;
            case D3DERR_CONFLICTINGTEXTUREPALETTE: g_TextureStage.WriteToLog("Validate Src: Conflicting texture palette\n"); break;
            case D3DERR_TOOMANYOPERATIONS:         g_TextureStage.WriteToLog("Validate Src: Too many operations\n"); break;
            case D3DERR_UNSUPPORTEDALPHAARG:       g_TextureStage.WriteToLog("Validate Src: Unsupported alpha arg\n"); break;
            case D3DERR_UNSUPPORTEDALPHAOPERATION: g_TextureStage.WriteToLog("Validate Src: Unsupported alpha op\n"); break;
            case D3DERR_UNSUPPORTEDCOLORARG:       g_TextureStage.WriteToLog("Validate Src: Unsupported color arg\n"); break;
            case D3DERR_UNSUPPORTEDCOLOROPERATION: g_TextureStage.WriteToLog("Validate Src: Unsupported color op\n"); break;
            case D3DERR_UNSUPPORTEDFACTORVALUE:    g_TextureStage.WriteToLog("Validate Src: Unsupported factor value\n"); break;
            case D3DERR_UNSUPPORTEDTEXTUREFILTER:  g_TextureStage.WriteToLog("Validate Src: Unsupported texture filter\n"); break;
            case D3DERR_WRONGTEXTUREFORMAT:        g_TextureStage.WriteToLog("Validate Src: Wrong texture format\n"); break;
        }
#endif
        return hRes;
    }

    return hRes;
}
