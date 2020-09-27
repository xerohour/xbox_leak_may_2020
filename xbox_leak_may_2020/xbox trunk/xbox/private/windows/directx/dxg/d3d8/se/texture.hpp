/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       texture.hpp
 *  Content:    Base class for all texture objects. Texture management is
 *              done at this level.
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{
 
//----------------------------------------------------------------------------
// Helper to create an instance of any texture.
//
HRESULT CreateTexture(
    DWORD Width,
    DWORD Height,
    DWORD Depth,
    DWORD Levels,
    DWORD Usage,
    D3DFORMAT Format,
    bool isCubeMap,
    bool isVolumeTexture,
    D3DBaseTexture **ppTexture
    );

} // end namespace
