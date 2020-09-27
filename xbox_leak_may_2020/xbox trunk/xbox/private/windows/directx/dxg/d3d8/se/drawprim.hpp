/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       drawprim.hpp
 *  Content:    DrawPrimitive common defines
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#define FVF_TEXCOORD_NUMBER(dwFVF) \
    (((dwFVF) & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT)

//---------------------------------------------------------------------
// Entry is texture count. Clears all texture format bits in the FVF DWORD,
// that correspond to the texture count for this count

D3DCONST DWORD g_TextureFormatMask[9] = 
{
    ~0x0000FFFF,
    ~0x0003FFFF,
    ~0x000FFFFF,
    ~0x003FFFFF,
    ~0x00FFFFFF,
    ~0x03FFFFFF,
    ~0x0FFFFFFF,
    ~0x3FFFFFFF,
    ~0xFFFFFFFF
};

} // end namespace
