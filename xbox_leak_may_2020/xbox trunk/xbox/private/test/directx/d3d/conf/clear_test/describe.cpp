#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "clear_test.h"

void CClearTest::DescribeFormat(char * pBuf, int nTexFormat, int nZFormat)
{
    char tbuf[256],zbuf[256];

    if (m_dwVersion >= 0x0800)
    {
        D3DFORMAT TexFmt = m_fmtCommon[nTexFormat];
        D3DFORMAT ZFmt = m_fmtdCommon[nZFormat];

        //
        // Texture format
        //

        switch (TexFmt)
        {
            case D3DFMT_R8G8B8:     sprintf(tbuf, "D3DFMT_R8G8B8");       break;
            case D3DFMT_A8R8G8B8:   sprintf(tbuf, "D3DFMT_A8R8G8B8");     break;
            case D3DFMT_X8R8G8B8:   sprintf(tbuf, "D3DFMT_X8R8G8B8");     break;
            case D3DFMT_R5G6B5:     sprintf(tbuf, "D3DFMT_R5G6B5");       break;
            case D3DFMT_X1R5G5B5:   sprintf(tbuf, "D3DFMT_X1R5G5B5");     break;
            case D3DFMT_A1R5G5B5:   sprintf(tbuf, "D3DFMT_A1R5G5B5");     break;
            case D3DFMT_A4R4G4B4:   sprintf(tbuf, "D3DFMT_A4R4G4B4");     break;
            case D3DFMT_R3G3B2:     sprintf(tbuf, "D3DFMT_R3G3B2");       break;
            case D3DFMT_A8:         sprintf(tbuf, "D3DFMT_A8");           break;
            case D3DFMT_A8R3G3B2:   sprintf(tbuf, "D3DFMT_A8R3G3B2");     break;
            case D3DFMT_X4R4G4B4:   sprintf(tbuf, "D3DFMT_X4R4G4B4");     break;
            case D3DFMT_A8P8:       sprintf(tbuf, "D3DFMT_A8P8");         break;
            case D3DFMT_P8:         sprintf(tbuf, "D3DFMT_P8");           break;
            case D3DFMT_L8:         sprintf(tbuf, "D3DFMT_L8");           break;
            case D3DFMT_A8L8:       sprintf(tbuf, "D3DFMT_A8L8");         break;
            case D3DFMT_A4L4:       sprintf(tbuf, "D3DFMT_A4L4");         break;
            case D3DFMT_V8U8:       sprintf(tbuf, "D3DFMT_V8U8");         break;
            case D3DFMT_L6V5U5:     sprintf(tbuf, "D3DFMT_L6V5U5");       break;
#ifndef UNDER_XBOX
            case D3DFMT_X8L8V8U8:   sprintf(tbuf, "D3DFMT_X8L8V8U8");     break;
#endif // !UNDER_XBOX
            case D3DFMT_Q8W8V8U8:   sprintf(tbuf, "D3DFMT_Q8W8V8U8");     break;
            case D3DFMT_V16U16:     sprintf(tbuf, "D3DFMT_V16U16");       break;
            case D3DFMT_W11V11U10:  sprintf(tbuf, "D3DFMT_W11V11U10");    break;
            case D3DFMT_UYVY:       sprintf(tbuf, "D3DFMT_UYVY");         break;
            case D3DFMT_YUY2:       sprintf(tbuf, "D3DFMT_YUY2");         break;
            case D3DFMT_DXT1:       sprintf(tbuf, "D3DFMT_DXT1");         break;
            case D3DFMT_DXT2:       sprintf(tbuf, "D3DFMT_DXT2");         break;
#ifndef UNDER_XBOX
            case D3DFMT_DXT3:       sprintf(tbuf, "D3DFMT_DXT3");         break;
#endif // !UNDER_XBOX
            case D3DFMT_DXT4:       sprintf(tbuf, "D3DFMT_DXT4");         break;
#ifndef UNDER_XBOX
            case D3DFMT_DXT5:       sprintf(tbuf, "D3DFMT_DXT5");         break;
#endif // !UNDER_XBOX
            default:                sprintf(tbuf, "Unknown");             break;
        };
                
                
        //
        // Depth buffer
        //

        if (m_ModeOptions.fZBuffer)
        {
            switch (ZFmt)
            {
#ifndef UNDER_XBOX
                case D3DFMT_D16_LOCKABLE:   sprintf(zbuf, "D3DFMT_D16_LOCKABLE"); break;
#endif // !UNDER_XBOX
                case D3DFMT_D32:            sprintf(zbuf, "D3DFMT_D32");          break;
                case D3DFMT_D15S1:          sprintf(zbuf, "D3DFMT_D15S1");        break;
                case D3DFMT_D24S8:          sprintf(zbuf, "D3DFMT_D24S8");        break;
                case D3DFMT_D16:            sprintf(zbuf, "D3DFMT_D16");          break;
#ifndef UNDER_XBOX
                case D3DFMT_D24X8:          sprintf(zbuf, "D3DFMT_D24X8");        break;
#endif // !UNDER_XBOX
                case D3DFMT_D24X4S4:        sprintf(zbuf, "D3DFMT_D24X4S4");      break;
                default:                    sprintf(zbuf, "Unknown");             break;
            };
        }
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



void CClearTest::DescribeTestcase(char * pBuf, int nTestcase)
{
	switch(nTestcase)
	{
		case 0:
			sprintf(pBuf, "Clear RGB full screen test.");
			break;
		case 1:
		case 2:
			sprintf(pBuf, "Clear Alpha full screen test.");
			break;
		case 3:
		case 4:
			sprintf(pBuf, "Clear Z full screen test.");
			break;
		case 5:
		case 6:
			sprintf(pBuf, "Clear Stencil full screen test.");
			break;
		case 7:
			sprintf(pBuf, "Clear Z affect stencil.");
			break;
		case 8:
			sprintf(pBuf, "Clear stencil affect Z.");
			break;
		case 9:
			sprintf(pBuf, "Clear render-target alone affect Z or stencil.");
			break;
		case 10:
			sprintf(pBuf, "Clear RGB test - list of rects.");
			break;
		case 11:
			sprintf(pBuf, "Clear alpha test - list of rects.");
			break;
		case 12:
			sprintf(pBuf, "Clear Z test - list of rects.");
			break;
		case 13:
			sprintf(pBuf, "Clear Stencil test - list of rects.");
			break;
		default:
			sprintf(pBuf, "DCT test error!");
			break;
	}

}