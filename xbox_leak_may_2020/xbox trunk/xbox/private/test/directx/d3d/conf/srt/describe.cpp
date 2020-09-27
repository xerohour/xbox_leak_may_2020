#include "d3dlocus.h"
#include "cd3dtest.h"
#include "srt.h"

void CSRT::DescribeTest(char * pBuf, int nTexFormat, int nZFormat)
{
    char tbuf[256],zbuf[256];

    if (m_dwVersion >= 0x0800)
    {
        //
        // Texture format
        //
        sprintf(tbuf, "%s", D3DFmtToString(m_fmtCommon[nTexFormat]));

        //
        // Depth buffer
        //
        if (m_ModeOptions.fZBuffer)
            sprintf(zbuf, "%s", D3DFmtToString(m_fmtdCommon[nZFormat]));
        else
            sprintf(zbuf, "Disabled"); 

    }
/*
    // m_dwVerion <= 0x0700
    else
    {
        PIXELFORMAT *pTexFmt = &(m_pCommonTextureFormats[nTexFormat].ddpfPixelFormat);
        PIXELFORMAT *pZFmt = &(m_pCommonZBufferFormats[nZFormat].ddpfPixelFormat);

        //
        // Texture format
        //

        if (pTexFmt->dwFlags & (PF_PALETTEINDEXED8 | PF_PALETTEINDEXED4 | PF_PALETTEINDEXED2 | PF_PALETTEINDEXED1 | PF_PALETTEINDEXEDTO8))
        {
            _stprintf(tbuf, "%d-bit Palettized", pTexFmt->dwRGBBitCount);
        } 

        if (pTexFmt->dwFlags & PF_RGB) 
        {
            _stprintf(tbuf, "%d-bit A:%08X R:%08X G:%08X B:%08X", 
                                pTexFmt->dwRGBBitCount, 
                                (pTexFmt->dwFlags & PF_ALPHAPIXELS) ? pTexFmt->dwRGBAlphaBitMask : 0,
                                pTexFmt->dwRBitMask, 
                                pTexFmt->dwGBitMask, 
                                pTexFmt->dwBBitMask);
        }
    
        if (pTexFmt->dwFlags & PF_BUMPDUDV)
        {
            _stprintf(tbuf, "%d-bit dU:%08X dV:%08X L:%08X",
                                pTexFmt->dwBumpBitCount,
                                pTexFmt->dwBumpDuBitMask,
                                pTexFmt->dwBumpDvBitMask,
                                (pTexFmt->dwFlags & PF_BUMPLUMINANCE) ? pTexFmt->dwBumpLuminanceBitMask : 0);
        }

        if (pTexFmt->dwFlags & (PF_LUMINANCE | PF_ALPHA))
        {
            _stprintf(tbuf, "%d-bit L:%08X A:%08X",
                                pTexFmt->dwLuminanceBitCount,
                                (pTexFmt->dwFlags & PF_LUMINANCE) ? pTexFmt->dwLuminanceBitMask : 0,
                                (pTexFmt->dwFlags & PF_ALPHA) ? pTexFmt->dwLuminanceAlphaBitMask : 0);
                    
        }

        if (pTexFmt->dwFlags & PF_FOURCC)
        {
            _stprintf(tbuf, "FourCC: %C%C%C%C", 
                            (pTexFmt->dwFourCC) & 0xFF,
                            ((pTexFmt->dwFourCC) & (0xFF<<8)) >> 8,
                            ((pTexFmt->dwFourCC) & (0xFF<<16)) >> 16,
                            ((pTexFmt->dwFourCC) & (0xFF<<24)) >> 24);
        }

        //
        // Depth buffer
        //

        if (m_ModeOptions.fZBuffer)
        {
            _stprintf(zbuf, "%d-bit, z%ds%d", 
                                pZFmt->dwZBufferBitDepth, 
                                pZFmt->dwZBufferBitDepth - pZFmt->dwStencilBitDepth,
                                pZFmt->dwStencilBitDepth);
        }
        else
            _stprintf(zbuf, "Disabled"); 
    }
*/
    sprintf(pBuf, "Tex(%d): %s Z(%d): %s", nTexFormat, tbuf, nZFormat, zbuf);
}

