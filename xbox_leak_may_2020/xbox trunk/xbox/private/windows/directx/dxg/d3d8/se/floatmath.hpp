/*==========================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       floatmath.hpp
 *  Content:    Private impementation of floating-point math routines.
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{
    // Converts a floating point value to a long.
    long FloatToLong(float f);

    // Cosine.
    float Cos(float e);

    // Exponent
    float Exp(float e);

    // Log
    float Log(float e);

} // end namespace
