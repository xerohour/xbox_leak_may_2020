/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       buffer.hpp
 *  Content:    Class header the buffer base class; this class
 *              contains all the logic that is shared between
 *              the Index/Vertex/Command buffer types.
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

//----------------------------------------------------------------------------
// Helper to create an instance of a buffer.
//
HRESULT CreateVertexIndexOrPushBuffer(
    DWORD Type,
    DWORD Size, 
    void **ppBuffer);


} // end namespace
