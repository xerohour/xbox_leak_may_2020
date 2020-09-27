/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       enum.hpp
 *  Content:    Class for the enumerator object.
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

class CEnum : public Direct3D
{
public:

    D3DFORMAT MapUnknownFormat(
        UINT         iAdapter,
        DWORD        Usage,
        D3DFORMAT    Format,
        D3DDEVTYPE   Type,
        D3DFORMAT    DisplayFormat) const;
}; 


} // end namespace
