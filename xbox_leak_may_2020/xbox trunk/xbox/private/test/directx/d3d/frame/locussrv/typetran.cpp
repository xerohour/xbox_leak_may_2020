#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <winsock.h>
#include <d3dx8.h>
#include "transprt.h"
#include "util.h"
#include "typetran.h"
#include "server.h"

#define SETTYPE(x)      dwType = x;
#define MAPTYPE(x)      pTypeMapTable[dwType].Add(XBOX_##x, x);
#define MAPTYPES(x, y)  pTypeMapTable[dwType].Add(x, y);

//***********************************************************************************
void InitTypeTranslationMaps(CMap32* pTypeMapTable) {

    DWORD dwType;

    SETTYPE(D3DTI_D3DBACKBUFFER_TYPE)
    MAPTYPE(D3DBACKBUFFER_TYPE_MONO)
    MAPTYPE(D3DBACKBUFFER_TYPE_LEFT)
    MAPTYPE(D3DBACKBUFFER_TYPE_RIGHT)

    SETTYPE(D3DTI_D3DBLEND)
    MAPTYPE(D3DBLEND_ZERO)
    MAPTYPE(D3DBLEND_ONE)
    MAPTYPE(D3DBLEND_SRCCOLOR)
    MAPTYPE(D3DBLEND_INVSRCCOLOR)
    MAPTYPE(D3DBLEND_SRCALPHA)
    MAPTYPE(D3DBLEND_INVSRCALPHA)
    MAPTYPE(D3DBLEND_DESTALPHA)
    MAPTYPE(D3DBLEND_INVDESTALPHA)
    MAPTYPE(D3DBLEND_DESTCOLOR)
    MAPTYPE(D3DBLEND_INVDESTCOLOR)
    MAPTYPE(D3DBLEND_SRCALPHASAT)
//    MAPTYPE(D3DBLEND_BOTHSRCALPHA)
//    MAPTYPE(D3DBLEND_BOTHINVSRCALPHA)

    SETTYPE(D3DTI_D3DBLENDOP)
    MAPTYPE(D3DBLENDOP_ADD)
    MAPTYPE(D3DBLENDOP_SUBTRACT)
    MAPTYPE(D3DBLENDOP_REVSUBTRACT)
    MAPTYPE(D3DBLENDOP_MIN)
    MAPTYPE(D3DBLENDOP_MAX)

    SETTYPE(D3DTI_D3DCMPFUNC)
    MAPTYPE(D3DCMP_NEVER)
    MAPTYPE(D3DCMP_LESS)
    MAPTYPE(D3DCMP_EQUAL)
    MAPTYPE(D3DCMP_LESSEQUAL)
    MAPTYPE(D3DCMP_GREATER)
    MAPTYPE(D3DCMP_NOTEQUAL)
    MAPTYPE(D3DCMP_GREATEREQUAL)
    MAPTYPE(D3DCMP_ALWAYS)

    SETTYPE(D3DTI_D3DCUBEMAP_FACES)
    MAPTYPE(D3DCUBEMAP_FACE_POSITIVE_X)
    MAPTYPE(D3DCUBEMAP_FACE_NEGATIVE_X)
    MAPTYPE(D3DCUBEMAP_FACE_POSITIVE_Y)
    MAPTYPE(D3DCUBEMAP_FACE_NEGATIVE_Y)
    MAPTYPE(D3DCUBEMAP_FACE_POSITIVE_Z)
    MAPTYPE(D3DCUBEMAP_FACE_NEGATIVE_Z)

    SETTYPE(D3DTI_D3DCULL)
    MAPTYPE(D3DCULL_NONE)
    MAPTYPE(D3DCULL_CW)
    MAPTYPE(D3DCULL_CCW)

    SETTYPE(D3DTI_D3DDEBUGMONITORTOKENS)
    MAPTYPE(D3DDMT_ENABLE)
    MAPTYPE(D3DDMT_DISABLE)

    SETTYPE(D3DTI_D3DDEVTYPE)
    MAPTYPE(D3DDEVTYPE_HAL)
    MAPTYPE(D3DDEVTYPE_REF)
    MAPTYPE(D3DDEVTYPE_SW)

    SETTYPE(D3DTI_D3DFILLMODE)
    MAPTYPE(D3DFILL_POINT)
    MAPTYPE(D3DFILL_WIREFRAME)
    MAPTYPE(D3DFILL_SOLID)

    SETTYPE(D3DTI_D3DFOGMODE)
    MAPTYPE(D3DFOG_NONE)
    MAPTYPE(D3DFOG_EXP)
    MAPTYPE(D3DFOG_EXP2)
    MAPTYPE(D3DFOG_LINEAR)

    SETTYPE(D3DTI_D3DFORMAT)
    MAPTYPE(D3DFMT_UNKNOWN)
//    MAPTYPE(D3DFMT_R8G8B8)
    MAPTYPE(D3DFMT_A8R8G8B8)
    MAPTYPE(D3DFMT_X8R8G8B8)
    MAPTYPE(D3DFMT_R5G6B5)
    MAPTYPE(D3DFMT_X1R5G5B5)
    MAPTYPE(D3DFMT_A1R5G5B5)
    MAPTYPE(D3DFMT_A4R4G4B4)
//    MAPTYPE(D3DFMT_R3G3B2)
    MAPTYPE(D3DFMT_A8)
//    MAPTYPE(D3DFMT_A8R3G3B2)
//    MAPTYPE(D3DFMT_X4R4G4B4)
//    MAPTYPE(D3DFMT_A8P8)
    MAPTYPE(D3DFMT_P8)
    MAPTYPE(D3DFMT_L8)
    MAPTYPE(D3DFMT_A8L8)
//    MAPTYPE(D3DFMT_A4L4)
//    MAPTYPE(D3DFMT_V8U8)        // Equivalent to XBOX_D3DFMT_G8B8 under Xbox
//    MAPTYPE(D3DFMT_L6V5U5)
////    MAPTYPE(D3DFMT_X8L8V8U8)  // Equivalent to XBOX_D3DFMT_X8R8G8B8 under Xbox
////    MAPTYPE(D3DFMT_Q8W8V8U8)  // Equivalent to XBOX_D3DFMT_A8B8G8R8 under Xbox
//    MAPTYPE(D3DFMT_V16U16)
////    MAPTYPE(D3DFMT_W11V11U10)
    MAPTYPE(D3DFMT_X8L8V8U8)
    MAPTYPE(D3DFMT_L6V5U5)
    MAPTYPE(D3DFMT_V8U8)
    MAPTYPE(D3DFMT_Q8W8V8U8)
    MAPTYPE(D3DFMT_V16U16)
    MAPTYPES(XBOX_D3DFMT_LIN_X8L8V8U8, D3DFMT_X8L8V8U8)  // Equivalent to XBOX_D3DFMT_X8R8G8B8 under Xbox
    MAPTYPES(XBOX_D3DFMT_LIN_L6V5U5, D3DFMT_L6V5U5)
    MAPTYPES(XBOX_D3DFMT_LIN_V8U8, D3DFMT_V8U8)        // Equivalent to XBOX_D3DFMT_G8B8 under Xbox
    MAPTYPES(XBOX_D3DFMT_LIN_Q8W8V8U8, D3DFMT_Q8W8V8U8)  // Equivalent to XBOX_D3DFMT_A8B8G8R8 under Xbox
    MAPTYPES(XBOX_D3DFMT_LIN_V16U16, D3DFMT_V16U16)
    MAPTYPE(D3DFMT_UYVY)
    MAPTYPE(D3DFMT_YUY2)
    MAPTYPE(D3DFMT_DXT1)
    MAPTYPE(D3DFMT_DXT2)
//    MAPTYPE(D3DFMT_DXT3)      // Equivalent to XBOX_D3DFMT_DXT2 under Xbox
    MAPTYPE(D3DFMT_DXT4)
//    MAPTYPE(D3DFMT_DXT5)      // Equivalent to XBOX_D3DFMT_DXT4 under Xbox
//    MAPTYPE(D3DFMT_D16_LOCKABLE)
    MAPTYPE(D3DFMT_D16)
//    MAPTYPE(D3DFMT_D32)
//    MAPTYPE(D3DFMT_D15S1)
    MAPTYPE(D3DFMT_D24S8)
//    MAPTYPES(XBOX_D3DFMT_D16, D3DFMT_D16_LOCKABLE)  // XBOX_D3DFMT_D16 and XBOX_D3DFMT_D16_LOCKABLE equivalent under Xbox
//    MAPTYPE(D3DFMT_D24X8)
//    MAPTYPE(D3DFMT_D24X4S4)
    MAPTYPE(D3DFMT_VERTEXDATA)
    MAPTYPE(D3DFMT_INDEX16)
    MAPTYPE(D3DFMT_INDEX32)
    MAPTYPES(XBOX_D3DFMT_LIN_A1R5G5B5, D3DFMT_A1R5G5B5)
    MAPTYPES(XBOX_D3DFMT_LIN_A4R4G4B4, D3DFMT_A4R4G4B4)
    MAPTYPES(XBOX_D3DFMT_LIN_A8, D3DFMT_A8)
    MAPTYPES(XBOX_D3DFMT_LIN_A8R8G8B8, D3DFMT_A8R8G8B8)
    MAPTYPES(XBOX_D3DFMT_LIN_R5G6B5, D3DFMT_R5G6B5)
    MAPTYPES(XBOX_D3DFMT_LIN_X1R5G5B5, D3DFMT_X1R5G5B5)
    MAPTYPES(XBOX_D3DFMT_LIN_X8R8G8B8, D3DFMT_X8R8G8B8)
    MAPTYPES(XBOX_D3DFMT_LIN_A8L8, D3DFMT_A8L8)
    MAPTYPES(XBOX_D3DFMT_LIN_L8, D3DFMT_L8)
    MAPTYPES(XBOX_D3DFMT_LIN_D24S8, D3DFMT_D24S8)
    MAPTYPES(XBOX_D3DFMT_LIN_F24S8, D3DFMT_D24S8)
    MAPTYPES(XBOX_D3DFMT_LIN_D16, D3DFMT_D16)
    MAPTYPES(XBOX_D3DFMT_LIN_F16, D3DFMT_D16)

    SETTYPE(D3DTI_D3DBASISTYPE)
    MAPTYPE(D3DBASIS_BEZIER)
    MAPTYPE(D3DBASIS_BSPLINE)
    MAPTYPE(D3DBASIS_INTERPOLATE)

    SETTYPE(D3DTI_D3DLIGHTTYPE)
    MAPTYPE(D3DLIGHT_POINT)
    MAPTYPE(D3DLIGHT_SPOT)
    MAPTYPE(D3DLIGHT_DIRECTIONAL)

    SETTYPE(D3DTI_D3DMATERIALCOLORSOURCE)
    MAPTYPE(D3DMCS_MATERIAL)
    MAPTYPE(D3DMCS_COLOR1)
    MAPTYPE(D3DMCS_COLOR2)

    SETTYPE(D3DTI_D3DMULTISAMPLE_TYPE)
    MAPTYPE(D3DMULTISAMPLE_NONE)
    MAPTYPE(D3DMULTISAMPLE_2_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_3_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_4_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_5_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_6_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_7_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_8_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_9_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_10_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_11_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_12_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_13_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_14_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_15_SAMPLES)
    MAPTYPE(D3DMULTISAMPLE_16_SAMPLES)

    SETTYPE(D3DTI_D3DORDERTYPE)
    MAPTYPE(D3DORDER_LINEAR)
    MAPTYPE(D3DORDER_CUBIC)
    MAPTYPE(D3DORDER_QUINTIC)

    SETTYPE(D3DTI_D3DPATCHEDGESTYLE)
    MAPTYPE(D3DPATCHEDGE_DISCRETE)
    MAPTYPE(D3DPATCHEDGE_CONTINUOUS)

    SETTYPE(D3DTI_D3DPRIMITIVETYPE)
    MAPTYPE(D3DPT_POINTLIST)
    MAPTYPE(D3DPT_LINELIST)
    MAPTYPE(D3DPT_LINESTRIP)
    MAPTYPE(D3DPT_TRIANGLELIST)
    MAPTYPE(D3DPT_TRIANGLESTRIP)
    MAPTYPE(D3DPT_TRIANGLEFAN)

    SETTYPE(D3DTI_D3DRENDERSTATETYPE)
    MAPTYPE(D3DRS_ZENABLE)
    MAPTYPE(D3DRS_FILLMODE)
    MAPTYPE(D3DRS_SHADEMODE)
//    MAPTYPE(D3DRS_LINEPATTERN)
    MAPTYPE(D3DRS_ZWRITEENABLE)
    MAPTYPE(D3DRS_ALPHATESTENABLE)
//    MAPTYPE(D3DRS_LASTPIXEL)
    MAPTYPE(D3DRS_SRCBLEND)
    MAPTYPE(D3DRS_DESTBLEND)
    MAPTYPE(D3DRS_CULLMODE)
    MAPTYPE(D3DRS_ZFUNC)
    MAPTYPE(D3DRS_ALPHAREF)
    MAPTYPE(D3DRS_ALPHAFUNC)
    MAPTYPE(D3DRS_DITHERENABLE)
    MAPTYPE(D3DRS_ALPHABLENDENABLE)
    MAPTYPE(D3DRS_FOGENABLE)
    MAPTYPE(D3DRS_SPECULARENABLE)
//    MAPTYPE(D3DRS_ZVISIBLE)
    MAPTYPE(D3DRS_FOGCOLOR)
    MAPTYPE(D3DRS_FOGTABLEMODE)
    MAPTYPE(D3DRS_FOGSTART)
    MAPTYPE(D3DRS_FOGEND)
    MAPTYPE(D3DRS_FOGDENSITY)
    MAPTYPE(D3DRS_EDGEANTIALIAS)
    MAPTYPE(D3DRS_ZBIAS)
    MAPTYPE(D3DRS_RANGEFOGENABLE)
    MAPTYPE(D3DRS_STENCILENABLE)
    MAPTYPE(D3DRS_STENCILFAIL)
    MAPTYPE(D3DRS_STENCILZFAIL)
    MAPTYPE(D3DRS_STENCILPASS)
    MAPTYPE(D3DRS_STENCILFUNC)
    MAPTYPE(D3DRS_STENCILREF)
    MAPTYPE(D3DRS_STENCILMASK)
    MAPTYPE(D3DRS_STENCILWRITEMASK)
    MAPTYPE(D3DRS_TEXTUREFACTOR)
    MAPTYPE(D3DRS_WRAP0)
    MAPTYPE(D3DRS_WRAP1)
    MAPTYPE(D3DRS_WRAP2)
    MAPTYPE(D3DRS_WRAP3)
//    MAPTYPE(D3DRS_WRAP4)
//    MAPTYPE(D3DRS_WRAP5)
//    MAPTYPE(D3DRS_WRAP6)
//    MAPTYPE(D3DRS_WRAP7)
//    MAPTYPE(D3DRS_CLIPPING)
    MAPTYPE(D3DRS_LIGHTING)
    MAPTYPE(D3DRS_AMBIENT)
//    MAPTYPE(D3DRS_FOGVERTEXMODE)
    MAPTYPE(D3DRS_COLORVERTEX)
    MAPTYPE(D3DRS_LOCALVIEWER)
    MAPTYPE(D3DRS_NORMALIZENORMALS)
    MAPTYPE(D3DRS_DIFFUSEMATERIALSOURCE)
    MAPTYPE(D3DRS_SPECULARMATERIALSOURCE)
    MAPTYPE(D3DRS_AMBIENTMATERIALSOURCE)
    MAPTYPE(D3DRS_EMISSIVEMATERIALSOURCE)
    MAPTYPE(D3DRS_VERTEXBLEND)
//    MAPTYPE(D3DRS_CLIPPLANEENABLE)
//    MAPTYPE(D3DRS_SOFTWAREVERTEXPROCESSING)
    MAPTYPE(D3DRS_POINTSIZE)
    MAPTYPE(D3DRS_POINTSIZE_MIN)
    MAPTYPE(D3DRS_POINTSPRITEENABLE)
    MAPTYPE(D3DRS_POINTSCALEENABLE)
    MAPTYPE(D3DRS_POINTSCALE_A)
    MAPTYPE(D3DRS_POINTSCALE_B)
    MAPTYPE(D3DRS_POINTSCALE_C)
    MAPTYPE(D3DRS_MULTISAMPLEANTIALIAS)
    MAPTYPE(D3DRS_MULTISAMPLEMASK)
    MAPTYPE(D3DRS_PATCHEDGESTYLE)
    MAPTYPE(D3DRS_PATCHSEGMENTS)
//    MAPTYPE(D3DRS_DEBUGMONITORTOKEN)
    MAPTYPE(D3DRS_POINTSIZE_MAX)
//    MAPTYPE(D3DRS_INDEXEDVERTEXBLENDENABLE)
    MAPTYPE(D3DRS_COLORWRITEENABLE)
//    MAPTYPE(D3DRS_TWEENFACTOR)
    MAPTYPE(D3DRS_BLENDOP)

    SETTYPE(D3DTI_D3DRESOURCETYPE)
    MAPTYPE(D3DRTYPE_SURFACE)
    MAPTYPE(D3DRTYPE_VOLUME)
    MAPTYPE(D3DRTYPE_TEXTURE)
    MAPTYPE(D3DRTYPE_VOLUMETEXTURE)
    MAPTYPE(D3DRTYPE_CUBETEXTURE)
    MAPTYPE(D3DRTYPE_VERTEXBUFFER)
    MAPTYPE(D3DRTYPE_INDEXBUFFER)

    SETTYPE(D3DTI_D3DSHADEMODE)
    MAPTYPE(D3DSHADE_FLAT)
    MAPTYPE(D3DSHADE_GOURAUD)
//    MAPTYPE(D3DSHADE_PHONG)   // Equivalent to D3DSHADE_GOURAUD under Xbox

    SETTYPE(D3DTI_D3DSTATEBLOCKTYPE)
    MAPTYPE(D3DSBT_ALL)
    MAPTYPE(D3DSBT_PIXELSTATE)
    MAPTYPE(D3DSBT_VERTEXSTATE)

    SETTYPE(D3DTI_D3DSTENCILOP)
    MAPTYPE(D3DSTENCILOP_KEEP)
    MAPTYPE(D3DSTENCILOP_ZERO)
    MAPTYPE(D3DSTENCILOP_REPLACE)
    MAPTYPE(D3DSTENCILOP_INCRSAT)
    MAPTYPE(D3DSTENCILOP_DECRSAT)
    MAPTYPE(D3DSTENCILOP_INVERT)
    MAPTYPE(D3DSTENCILOP_INCR)
    MAPTYPE(D3DSTENCILOP_DECR)

    SETTYPE(D3DTI_D3DSWAPEFFECT)
    MAPTYPE(D3DSWAPEFFECT_DISCARD)
    MAPTYPE(D3DSWAPEFFECT_FLIP)
    MAPTYPE(D3DSWAPEFFECT_COPY)
    MAPTYPE(D3DSWAPEFFECT_COPY_VSYNC)

    SETTYPE(D3DTI_D3DTEXTUREADDRESS)
    MAPTYPE(D3DTADDRESS_WRAP)
    MAPTYPE(D3DTADDRESS_MIRROR)
    MAPTYPE(D3DTADDRESS_CLAMP)
    MAPTYPE(D3DTADDRESS_BORDER)
//    MAPTYPE(D3DTADDRESS_MIRRORONCE)

    SETTYPE(D3DTI_D3DTEXTUREFILTERTYPE)
    MAPTYPE(D3DTEXF_NONE)
    MAPTYPE(D3DTEXF_POINT)
    MAPTYPE(D3DTEXF_LINEAR)
    MAPTYPE(D3DTEXF_ANISOTROPIC)
    MAPTYPE(D3DTEXF_FLATCUBIC)
    MAPTYPE(D3DTEXF_GAUSSIANCUBIC)

    SETTYPE(D3DTI_D3DTEXTUREOP)
    MAPTYPE(D3DTOP_DISABLE)
    MAPTYPE(D3DTOP_SELECTARG1)
    MAPTYPE(D3DTOP_SELECTARG2)
    MAPTYPE(D3DTOP_MODULATE)
    MAPTYPE(D3DTOP_MODULATE2X)
    MAPTYPE(D3DTOP_MODULATE4X)
    MAPTYPE(D3DTOP_ADD)
    MAPTYPE(D3DTOP_ADDSIGNED)
    MAPTYPE(D3DTOP_ADDSIGNED2X)
    MAPTYPE(D3DTOP_SUBTRACT)
    MAPTYPE(D3DTOP_ADDSMOOTH)
    MAPTYPE(D3DTOP_BLENDDIFFUSEALPHA)
    MAPTYPE(D3DTOP_BLENDTEXTUREALPHA)
    MAPTYPE(D3DTOP_BLENDFACTORALPHA)
    MAPTYPE(D3DTOP_BLENDTEXTUREALPHAPM)
    MAPTYPE(D3DTOP_BLENDCURRENTALPHA)
    MAPTYPE(D3DTOP_PREMODULATE)
    MAPTYPE(D3DTOP_MODULATEALPHA_ADDCOLOR)
    MAPTYPE(D3DTOP_MODULATECOLOR_ADDALPHA)
    MAPTYPE(D3DTOP_MODULATEINVALPHA_ADDCOLOR)
    MAPTYPE(D3DTOP_MODULATEINVCOLOR_ADDALPHA)
    MAPTYPE(D3DTOP_BUMPENVMAP)
    MAPTYPE(D3DTOP_BUMPENVMAPLUMINANCE)
    MAPTYPE(D3DTOP_DOTPRODUCT3)
    MAPTYPE(D3DTOP_MULTIPLYADD)
    MAPTYPE(D3DTOP_LERP)

    SETTYPE(D3DTI_D3DTEXTURESTAGESTATETYPE)
    MAPTYPE(D3DTSS_COLOROP)
    MAPTYPE(D3DTSS_COLORARG0)
    MAPTYPE(D3DTSS_COLORARG1)
    MAPTYPE(D3DTSS_COLORARG2)
    MAPTYPE(D3DTSS_ALPHAOP)
    MAPTYPE(D3DTSS_ALPHAARG0)
    MAPTYPE(D3DTSS_ALPHAARG1)
    MAPTYPE(D3DTSS_ALPHAARG2)
    MAPTYPE(D3DTSS_BUMPENVMAT00)
    MAPTYPE(D3DTSS_BUMPENVMAT01)
    MAPTYPE(D3DTSS_BUMPENVMAT10)
    MAPTYPE(D3DTSS_BUMPENVMAT11)
    MAPTYPE(D3DTSS_TEXCOORDINDEX)
    MAPTYPE(D3DTSS_ADDRESSU)
    MAPTYPE(D3DTSS_ADDRESSV)
    MAPTYPE(D3DTSS_BORDERCOLOR)
    MAPTYPE(D3DTSS_MAGFILTER)
    MAPTYPE(D3DTSS_MINFILTER)
    MAPTYPE(D3DTSS_MIPFILTER)
    MAPTYPE(D3DTSS_MIPMAPLODBIAS)
    MAPTYPE(D3DTSS_MAXMIPLEVEL)
    MAPTYPE(D3DTSS_MAXANISOTROPY)
    MAPTYPE(D3DTSS_BUMPENVLSCALE)
    MAPTYPE(D3DTSS_BUMPENVLOFFSET)
    MAPTYPE(D3DTSS_TEXTURETRANSFORMFLAGS)
    MAPTYPE(D3DTSS_ADDRESSW)
    MAPTYPE(D3DTSS_COLORARG0)
    MAPTYPE(D3DTSS_ALPHAARG0)
    MAPTYPE(D3DTSS_RESULTARG)

    SETTYPE(D3DTI_D3DTEXTURETRANSFORMFLAGS)
    MAPTYPE(D3DTTFF_DISABLE)
    MAPTYPE(D3DTTFF_COUNT1)
    MAPTYPE(D3DTTFF_COUNT2)
    MAPTYPE(D3DTTFF_COUNT3)
    MAPTYPE(D3DTTFF_COUNT4)
    MAPTYPE(D3DTTFF_PROJECTED)

    SETTYPE(D3DTI_D3DTRANSFORMSTATETYPE)
    MAPTYPE(D3DTS_VIEW)
    MAPTYPE(D3DTS_PROJECTION)
    MAPTYPE(D3DTS_TEXTURE0)
    MAPTYPE(D3DTS_TEXTURE1)
    MAPTYPE(D3DTS_TEXTURE2)
    MAPTYPE(D3DTS_TEXTURE3)
    MAPTYPES(XBOX_D3DTS_WORLD, D3DTS_WORLDMATRIX(0))
    MAPTYPES(XBOX_D3DTS_WORLD1, D3DTS_WORLDMATRIX(1))
    MAPTYPES(XBOX_D3DTS_WORLD2, D3DTS_WORLDMATRIX(2))
    MAPTYPES(XBOX_D3DTS_WORLD3, D3DTS_WORLDMATRIX(3))

    SETTYPE(D3DTI_D3DVERTEXBLENDFLAGS)
    MAPTYPE(D3DVBF_DISABLE)
    MAPTYPE(D3DVBF_1WEIGHTS)
    MAPTYPE(D3DVBF_2WEIGHTS)
    MAPTYPE(D3DVBF_3WEIGHTS)
//    MAPTYPE(D3DVBF_TWEENING)
//    MAPTYPE(D3DVBF_0WEIGHTS)

    SETTYPE(D3DTI_D3DZBUFFERTYPE)
    MAPTYPE(D3DZB_FALSE)
    MAPTYPE(D3DZB_TRUE)
    MAPTYPE(D3DZB_USEW)

    SETTYPE(D3DTI_D3DPOOL)
    MAPTYPES(0, D3DPOOL_MANAGED)
    MAPTYPES(1, D3DPOOL_MANAGED)
    MAPTYPES(2, D3DPOOL_MANAGED)


}
