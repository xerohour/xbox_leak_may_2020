/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       math.hpp
 *  Content:    Common math routines
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#define F_ONE 0x3f800000
#define F_64 0x42800000
#define F_POS_INFINITY 0x7F800000
#define F_NEG_INFINITY 0xFF800000

float JBInvSqrt (const float x);
float nvSqrt (float x);
float nvInv (float x);
void ScaleVector3 (D3DVECTOR *out, CONST D3DVECTOR *v1, FLOAT scale);
void AddVectors3 (D3DVECTOR *out, CONST D3DVECTOR *v1, CONST D3DVECTOR *v2);
FLOAT SquareMagnitude3 (CONST D3DVECTOR *v);
void NormalizeVector3 (D3DVECTOR *v);
void XformBy4x3 (D3DVECTOR *res, CONST D3DVECTOR *v, FLOAT w, CONST D3DMATRIX *m);
void MatrixProduct4x4 (D3DMATRIX *res, CONST D3DMATRIX *a, CONST D3DMATRIX *b);
int Inverse4x4 (D3DMATRIX *inverse, CONST D3DMATRIX *src, BOOL bNormalize);
void Explut (float n, float *l, float *m);

//---------------------------------------------------------------------------
// reverse a 3-component vector

FORCEINLINE void ReverseVector3 (D3DVECTOR *out, CONST D3DVECTOR *in)
{
    out->x = -(in->x);
    out->y = -(in->y);
    out->z = -(in->z);
}

//------------------------------------------------------------------------------
// g_IdentityMatrix

D3DCONST D3DMATRIX g_IdentityMatrix = 
{ 
    1.0f, 0.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f 
};

//------------------------------------------------------------------------------
// Round
//
// Note that because 'FloatToLong' simply truncates, this routine doesn't
// properly round negative numbers - e.g., '-1.8f' will become '1', and
// '-1.2f' will become '0'.

FORCEINLINE DWORD Round(
    FLOAT f)
{
    return (DWORD) FloatToLong(f + 0.5f);
}

//------------------------------------------------------------------------------
// RoundSigned

FORCEINLINE DWORD RoundSigned(
    FLOAT f)
{
    DWORD result;

    if (f >= 0.0f)
    {
        result = FloatToLong(f + 0.5f);
    }
    else
    {
        result = -FloatToLong(-f + 0.5f);
    }

    return result;
}


} // end namespace
