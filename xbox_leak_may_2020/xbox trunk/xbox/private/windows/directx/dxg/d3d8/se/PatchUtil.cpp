/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       PatchUtil.cpp
 *  Content:    High order surface implementation.
 *
 ****************************************************************************/

#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#pragma warning (disable:4244)
#pragma warning (disable:4102)

unsigned int RetessFDtag[2][2] = {0x3f800000, 0, 0x3f800000, 0};
int RetessFDMRU = 0;
FDMatrix *RetessFDCache[2] = {NULL,NULL};

extern float *bezBasis[16];
extern float *catmullRomBasis[16];
extern float *bsplineBasis[16];
extern float *invBezBasis[16];
extern float *powerToFD[16];
extern float *inversePowerToFD[16];

const float cachedBinomialCoefficients[11][11] = {
  { 1.000000000000000e+000f, 1.000000000000000e+000f, 5.000000000000000e-001f, 1.666666666666667e-001f, 4.166666666666666e-002f,
    8.333333333333333e-003f, 1.388888888888889e-003f, 1.984126984126984e-004f, 2.480158730158730e-005f, 2.755731922398589e-006f, 2.755731922398589e-007f},
  { 1.000000000000000e+000f, 1.000000000000000e+000f, 5.000000000000000e-001f, 1.666666666666667e-001f, 4.166666666666666e-002f,
    8.333333333333333e-003f, 1.388888888888889e-003f, 1.984126984126984e-004f, 2.480158730158730e-005f, 2.755731922398589e-006f, 2.755731922398589e-007f},
  { 1.000000000000000e+000f, 2.000000000000000e+000f, 1.000000000000000e+000f, 3.333333333333333e-001f, 8.333333333333333e-002f,
    1.666666666666667e-002f, 2.777777777777778e-003f, 3.968253968253968e-004f, 4.960317460317460e-005f, 5.511463844797179e-006f, 5.511463844797179e-007f},
  { 1.000000000000000e+000f, 3.000000000000000e+000f, 3.000000000000000e+000f, 1.000000000000000e+000f, 2.500000000000000e-001f,
    5.000000000000000e-002f, 8.333333333333333e-003f, 1.190476190476190e-003f, 1.488095238095238e-004f, 1.653439153439153e-005f, 1.653439153439153e-006f},
  { 1.000000000000000e+000f, 4.000000000000000e+000f, 6.000000000000000e+000f, 4.000000000000000e+000f, 1.000000000000000e+000f,
    2.000000000000000e-001f, 3.333333333333333e-002f, 4.761904761904762e-003f, 5.952380952380952e-004f, 6.613756613756613e-005f, 6.613756613756613e-006f},
  { 1.000000000000000e+000f, 5.000000000000000e+000f, 1.000000000000000e+001f, 1.000000000000000e+001f, 5.000000000000000e+000f,
    1.000000000000000e+000f, 1.666666666666667e-001f, 2.380952380952381e-002f, 2.976190476190476e-003f, 3.306878306878307e-004f, 3.306878306878306e-005f},
  { 1.000000000000000e+000f, 6.000000000000000e+000f, 1.500000000000000e+001f, 2.000000000000000e+001f, 1.500000000000000e+001f,
    6.000000000000000e+000f, 1.000000000000000e+000f, 1.428571428571429e-001f, 1.785714285714286e-002f, 1.984126984126984e-003f, 1.984126984126984e-004f},
  { 1.000000000000000e+000f, 7.000000000000000e+000f, 2.100000000000000e+001f, 3.500000000000000e+001f, 3.500000000000000e+001f,
    2.100000000000000e+001f, 7.000000000000000e+000f, 1.000000000000000e+000f, 1.250000000000000e-001f, 1.388888888888889e-002f, 1.388888888888889e-003f},
  { 1.000000000000000e+000f, 8.000000000000000e+000f, 2.800000000000000e+001f, 5.600000000000000e+001f, 7.000000000000000e+001f,
    5.600000000000000e+001f, 2.800000000000000e+001f, 8.000000000000000e+000f, 1.000000000000000e+000f, 1.111111111111111e-001f, 1.111111111111111e-002f},
  { 1.000000000000000e+000f, 9.000000000000000e+000f, 3.600000000000000e+001f, 8.400000000000000e+001f, 1.260000000000000e+002f,
    1.260000000000000e+002f, 8.400000000000000e+001f, 3.600000000000000e+001f, 9.000000000000000e+000f, 1.000000000000000e+000f, 1.000000000000000e-001f},
  { 1.000000000000000e+000f, 1.000000000000000e+001f, 4.500000000000000e+001f, 1.200000000000000e+002f, 2.100000000000000e+002f,
    2.520000000000000e+002f, 2.100000000000000e+002f, 1.200000000000000e+002f, 4.500000000000000e+001f, 1.000000000000000e+001f, 1.000000000000000e+000f},
};

#define EV_GUARD_ATTR(index)        (((index) == NV_PATCH_ATTRIB_NORMAL) || ((index) == NV_PATCH_ATTRIB_POSITION))
#define EV_GUARD_BITS               ((1 << NV_PATCH_ATTRIB_NORMAL) | (1 << NV_PATCH_ATTRIB_POSITION))
#define EV_GUARD_BIT_VERTEX         ((1 << NV_PATCH_ATTRIB_POSITION))
#define EV_GUARD_BIT_NORMAL         ((1 << NV_PATCH_ATTRIB_NORMAL))

// Some of the ATTR bit names are used so much, and are so long, they were making line lengths in excess of 140 characters.
// Rather than split the lines, these names have abbreviations
#define ATTR_V  NV_PATCH_ATTRIB_POSITION
#define ATTR_N  NV_PATCH_ATTRIB_NORMAL

// note: conventions for specifying indices.
// For matrices, the ordering is [row][col] or [Vcoord][Ucoord].
// This matches how the control points are specified.
// For function arguments, it's tempting to use (Ucoord, Vcoord) since that
// is similar to (X, Y), but this can be confusing (matrices vs routines).
// Therefore, routines also take arguments in the (V, U) order.

// For the HW interface, integer step patches,
// the curve for the outside determines the direction of drawing the stitch.
// The inside curve may be evaluated in the reverse order. In these cases
// the n points are saved away in order to match the outside direction,
// then the stitch is drawn in the outside direction.

// Normally, matrices and curves are calculated with small step sizes.

// When swathing, the matrices are calculated with big steps, and 
// converted during swathing. The low level routine DrawIntPatchGrid()
// receives matrices with small steps.
// Guard curves are calculated with big steps in nvEvalPatch(), then
// stepped and converted to small steps in DrawIntPatchSwaths()
// DrawIntStitch() receives a set of small step curves.

/*****************************************************************************/
#define EV_SWAP(a, b, t) { t = a; a = b; b = t; }

// Count as maximum patch order of 5 for the inner loop
int CountLoopPushes(NV_PATCH_INFO *info)
{
    int index, pushes = 4;

    for (index = 0; index < info->maxAttr; index++) {
        if (info->evalEnables & (1 << index)) {
            pushes += 5 * info->maxOrder;
        }
    }

    return pushes;
}

// index into curve*[]. For given attrib, and term of the curve equation, point to the 'X' of the attribute
inline int idxCurve(int attribIndex, int orderTerm)
{
    return (attribIndex*NV_PATCH_MAX_ORDER + orderTerm)*4;
}

inline float *pRawData(NV_PATCH_INFO *info, NV_PATCH_MAP_INFO *map, float *rawData, int vCoord, int uCoord)
{
    if (info->flipV) {
        vCoord = map->vorder - 1 - vCoord;
    }
    if (info->flipU) {
        uCoord = map->uorder - 1 - uCoord;
    }
    return (rawData + vCoord * map->vfStride + uCoord * map->ufStride);
}

inline void getRawData(NV_PATCH_INFO *info, NV_PATCH_MAP_INFO *map, float *rawData, int vCoord, int uCoord, float *pOut)
{
    float *data = pRawData(info, map, rawData, vCoord, uCoord);
    pOut[0] = data[0]; pOut[1] = data[1]; pOut[2] = data[2]; pOut[3] = data[3];

}

inline void CopyPoint(float *dst, float *src)
// This will generate better code if dst and/or src are expressions or fancy structures.
// The reason is that dst and src are evaluated once.
// If done by hand in-line, the compiler may not optimize for the similarity of expressions.
// Looking at a few tests, without the inline function took 4*12=48 instructions,
// vs only 21 using CopyPoint
// Also, and just as important, I think it makes the code easier to read because
// it's more compact, not expanding x4 for each simple copy.
// And, as a bonus, if you are "watching" dst in the debugger, you may optionally
// step into this routine to see the values being copied. Otherwise, it may be a more
// laborious procedure to see what just happened.
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
}

inline int PointsEqual(float *src, float *src2)
{
    return(EV_AS_INT(src[0]) == EV_AS_INT(src2[0])
        && EV_AS_INT(src[1]) == EV_AS_INT(src2[1])
        && EV_AS_INT(src[2]) == EV_AS_INT(src2[2])
        && EV_AS_INT(src[3]) == EV_AS_INT(src2[3]));
}

inline void AddPoint(float *dst, float *src)
{
    dst[0] += src[0];
    dst[1] += src[1];
    dst[2] += src[2];
    dst[3] += src[3];
}

inline void CopyCoeffs(NV_PATCH_INFO *info, NV_PATCH_CURVE_COEFFS *dst, NV_PATCH_CURVE_COEFFS *src)
// copy curve coeffs. Does not copy 'order'. May be used to copy matrix rows to curve coeffs.
{
    memcpy(dst, src, 4*info->maxOrder*sizeof(float));
}

void CopyCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *dst, NV_PATCH_CURVE_INFO *src)
// copy curve, including order
{
    float *srcptr, *dstptr;
    switch(src->order) {
    case 2:
        dst->order = src->order;
        srcptr = (float*)(&src->coeffs[0][0]); dstptr = (float*)(&dst->coeffs[0][0]);
        dstptr[0] = srcptr[0]; dstptr[1] = srcptr[1]; dstptr[2] = srcptr[2]; dstptr[3] = srcptr[3];  //v0-xyzw
        dstptr[4] = srcptr[4]; dstptr[5] = srcptr[5]; dstptr[6] = srcptr[6]; dstptr[7] = srcptr[7];  //v1-xyzw
        break;
    case 4:
        dst->order = src->order;
        srcptr = (float*)(&src->coeffs[0][0]); dstptr = (float*)(&dst->coeffs[0][0]);
        dstptr[0] = srcptr[0]; dstptr[1] = srcptr[1]; dstptr[2] = srcptr[2]; dstptr[3] = srcptr[3];  //v0-xyzw
        dstptr[4] = srcptr[4]; dstptr[5] = srcptr[5]; dstptr[6] = srcptr[6]; dstptr[7] = srcptr[7];  //v1-xyzw
        dstptr[8] = srcptr[8]; dstptr[9] = srcptr[9]; dstptr[10] = srcptr[10]; dstptr[11] = srcptr[11];  //v2-xyzw
        dstptr[12] = srcptr[12]; dstptr[13] = srcptr[13]; dstptr[14] = srcptr[14]; dstptr[15] = srcptr[15];  //v3-xyzw
        break;
    case 6:
        dst->order = src->order;
        srcptr = (float*)(&src->coeffs[0][0]); dstptr = (float*)(&dst->coeffs[0][0]);
        dstptr[0] = srcptr[0]; dstptr[1] = srcptr[1]; dstptr[2] = srcptr[2]; dstptr[3] = srcptr[3];  //v0-xyzw
        dstptr[4] = srcptr[4]; dstptr[5] = srcptr[5]; dstptr[6] = srcptr[6]; dstptr[7] = srcptr[7];  //v1-xyzw
        dstptr[8] = srcptr[8]; dstptr[9] = srcptr[9]; dstptr[10] = srcptr[10]; dstptr[11] = srcptr[11];  //v2-xyzw
        dstptr[12] = srcptr[12]; dstptr[13] = srcptr[13]; dstptr[14] = srcptr[14]; dstptr[15] = srcptr[15];  //v3-xyzw
        dstptr[16] = srcptr[16]; dstptr[17] = srcptr[17]; dstptr[18] = srcptr[18]; dstptr[19] = srcptr[19];  //v4-xyzw
        dstptr[20] = srcptr[20]; dstptr[21] = srcptr[21]; dstptr[22] = srcptr[22]; dstptr[23] = srcptr[23];  //v5-xyzw
        break;
     default:
        memcpy(dst, src, PATCH_CURVE_HEADER_SIZE + 4*info->maxOrder*sizeof(float));
    }
}

inline void CopyCurveSet(NV_PATCH_INFO *info, int evalEnables, FDCurveAttrSet *dst, FDCurveAttrSet *src)
{
    int index;
    for (index = 0; index < info->maxAttr; index++) {
        if (evalEnables & (1 << index)) {
            CopyCurve(info, (*dst)[index], (*src)[index]);
        }
    }
}


// copy one matrix
inline void CopyMatrix(NV_PATCH_INFO *info, FDMatrix *dst, FDMatrix *src)
{
    int i;
    float *srcptr, *dstptr;
    switch(src->rows) {
    case 2:
        dst->rows = src->rows;
        dst->columns = src->columns;
        srcptr = (float*)(&src->data[0][0][0]);
        dstptr = (float*)(&dst->data[0][0][0]);
        dstptr[0] = srcptr[0]; dstptr[1] = srcptr[1]; dstptr[2] = srcptr[2]; dstptr[3] = srcptr[3];  //v0-xyzw
        dstptr[4] = srcptr[4]; dstptr[5] = srcptr[5]; dstptr[6] = srcptr[6]; dstptr[7] = srcptr[7];  //v1-xyzw
        dstptr+=16*4; srcptr+=16*4;                                                                  //skip to next row
        dstptr[0] = srcptr[0]; dstptr[1] = srcptr[1]; dstptr[2] = srcptr[2]; dstptr[3] = srcptr[3];  //v2-xyzw
        dstptr[4] = srcptr[4]; dstptr[5] = srcptr[5]; dstptr[6] = srcptr[6]; dstptr[7] = srcptr[7];  //v3-xyzw
        break;
    case 4:
        dst->rows = src->rows;
        dst->columns = src->columns;
        srcptr = (float*)(&src->data[0][0][0]);
        dstptr = (float*)(&dst->data[0][0][0]);
        for(i=0; i < 4; i++) {
            dstptr[0] = srcptr[0]; dstptr[1] = srcptr[1]; dstptr[2] = srcptr[2]; dstptr[3] = srcptr[3];  //v0-xyzw
            dstptr[4] = srcptr[4]; dstptr[5] = srcptr[5]; dstptr[6] = srcptr[6]; dstptr[7] = srcptr[7];  //v1-xyzw
            dstptr[8] = srcptr[8]; dstptr[9] = srcptr[9]; dstptr[10] = srcptr[10]; dstptr[11] = srcptr[11];  //v2-xyzw
            dstptr[12] = srcptr[12]; dstptr[13] = srcptr[13]; dstptr[14] = srcptr[14]; dstptr[15] = srcptr[15];  //v3-xyzw
            dstptr+=16*4; srcptr+=16*4;                                                                  //skip to next row
        }
        break;
    case 6:
        dst->rows = src->rows;
        dst->columns = src->columns;
        srcptr = (float*)(&src->data[0][0][0]);
        dstptr = (float*)(&dst->data[0][0][0]);
        for(i=0; i < 6; i++) {
            dstptr[0] = srcptr[0]; dstptr[1] = srcptr[1]; dstptr[2] = srcptr[2]; dstptr[3] = srcptr[3];  //v0-xyzw
            dstptr[4] = srcptr[4]; dstptr[5] = srcptr[5]; dstptr[6] = srcptr[6]; dstptr[7] = srcptr[7];  //v1-xyzw
            dstptr[8] = srcptr[8]; dstptr[9] = srcptr[9]; dstptr[10] = srcptr[10]; dstptr[11] = srcptr[11];  //v2-xyzw
            dstptr[12] = srcptr[12]; dstptr[13] = srcptr[13]; dstptr[14] = srcptr[14]; dstptr[15] = srcptr[15];  //v3-xyzw
            dstptr[16] = srcptr[16]; dstptr[17] = srcptr[17]; dstptr[18] = srcptr[18]; dstptr[19] = srcptr[19];  //v4-xyzw
            dstptr[20] = srcptr[20]; dstptr[21] = srcptr[21]; dstptr[22] = srcptr[22]; dstptr[23] = srcptr[23];  //v5-xyzw
            dstptr+=16*4; srcptr+=16*4;                                                                  //skip to next row
        }
        break;
    default:
        memcpy(dst, src, sizeof(FDMatrix));
    }
}

inline void CopyMatrixTranspose(NV_PATCH_INFO *info, FDMatrix *dst, FDMatrix *src)
{
    int i,j;
    //THIS NEEDS TO BE IMPROVED!!!
    dst->rows = src->rows;
    dst->columns = src->columns;
    for(i=0; i < src->rows; i++) {
        for(j = 0; j < src->columns; j++) {
            dst->data[j][i][0] = src->data[i][j][0];
            dst->data[j][i][1] = src->data[i][j][1];
            dst->data[j][i][2] = src->data[i][j][2];
            dst->data[j][i][3] = src->data[i][j][3];
        }
    }
}

inline void CopyMatrixSet(NV_PATCH_INFO *info, int evalEnables, FDMatrixAttrSet *dst, FDMatrixAttrSet *src)
// copy set of matrices
{
    int index;
    for (index = 0; index < info->maxAttr; index++) {
        if (!(evalEnables & (1 << index))) {
            continue;
        }
        CopyMatrix(info, (*dst)[index], (*src)[index]);
    }
}

/*****************************************************************************/
// Compare two vertices and return an edge direction
// a->b is 1, b->a is 0
int CompareVertices(const float *a, const float *b)
{
    // Compare X
    if (a[0] > b[0]) {
        return 0;
    } else if (a[0] < b[0]) {
        return 1;
    }

    // Compare Y
    if (a[1] > b[1]) {
        return 0;
    } else if (a[1] < b[1]) {
        return 1;
    }

    // Compare Z
    if (a[2] > b[2]) {
        return 0;
    } else if (a[2] < b[2]) {
        return 1;
    }

    // Compare W
    if (a[3] > b[3]) {
        return 0;
    } else if (a[3] < b[3]) {
        return 1;
    }

    // actually, if the two points are equal, the "right" thing to do
    // would be to examine the interior control points to determine
    // a sorting.

    // Arbitrary case
    return 1;
}

// Compute the directions of the four edges of a patch
void ComputeEdgeDirections(NV_PATCH_INFO *info, int tri, NV_PATCH_MAP_INFO *map,
                           int *u0Dir, int *u1Dir, int *v0Dir, int *v1Dir)
{
    int uorder = map->uorder;
    int vorder = map->vorder;
    float *rawData = map->rawData;
    const float *p00, *p10, *p01, *p11; //p[x][y]

    if (rawData == NULL) {
        //assuming that unenabled maps have a null pointer
        //if we don't have a position punt for now and assume no reversals
        *u0Dir = *u1Dir = *v0Dir = *v1Dir = 1;
        return;
    }

    p00 = pRawData(info, map, rawData, 0,        0);
    p10 = pRawData(info, map, rawData, 0,        uorder-1);
    p01 = pRawData(info, map, rawData, vorder-1, 0       );
    p11 = pRawData(info, map, rawData, vorder-1, uorder-1);

    if (tri) {
        *u0Dir = *u1Dir = CompareVertices(p00, p10);
        *v0Dir = CompareVertices(p00, p01);
        *v1Dir = CompareVertices(p10, p01);
    } else {
        *u0Dir = CompareVertices(p00, p10);
        *u1Dir = CompareVertices(p01, p11);
        *v0Dir = CompareVertices(p00, p01);
        *v1Dir = CompareVertices(p10, p11);
    }
}

void MatrixMult(NV_PATCH_INFO *info, FDMatrix *c, const FDMatrix *a, const FDMatrix *b)
{
    int row, col, term;

    c->rows    = a->rows;
    c->columns = b->columns;
    
    for (row = 0; row < c->rows; row++) {
        for (col = 0; col < c->columns; col++) {
            c->data[row][col][0] = 0.0f;
            c->data[row][col][1] = 0.0f;
            c->data[row][col][2] = 0.0f;
            c->data[row][col][3] = 0.0f;
            for (term = 0; term < a->columns; term++) {
                c->data[row][col][0] += a->data[row][term][0] * b->data[term][col][0];
                c->data[row][col][1] += a->data[row][term][1] * b->data[term][col][1];
                c->data[row][col][2] += a->data[row][term][2] * b->data[term][col][2];
                c->data[row][col][3] += a->data[row][term][3] * b->data[term][col][3];
            }
        }
    }
}

// Load m with a Bezier basis matrix, or the inverse of a Bezier basis matrix
void ComputeBasis(int order, int inverse, FDMatrix *m)
{
    int row, col;
    float *matrix = NULL;

    // Inverse matrices have been pulled out for now, as the current code does not need them
    nvAssert(inverse == 0);

    // nxn matrix where n = order
    m->rows = m->columns = order;

    // Select the appropriate matrix
    matrix = bezBasis[order-1];

    if (!matrix) {  nvAssert(0); return; }

    // Load matrix
    for (row = 0; row < order; row++) {
        for (col = 0; col < order; col++) {
            float coeff = *matrix++;
            m->data[row][col][0] = coeff;
            m->data[row][col][1] = coeff;
            m->data[row][col][2] = coeff;
            m->data[row][col][3] = coeff;
        }
    }
}

// Compute the matrix to convert from a power basis to an FD basis
void ComputePowerToFD(int order, FDMatrix *m, int inverse)
{
    int row, col;
    float *matrix;

    // nxn matrix where n = order
    m->rows = m->columns = order;

    if (inverse) {
        matrix = inversePowerToFD[order-1];
    } else {
        matrix = powerToFD[order-1];
    }

    // Load matrix
    for (row = 0; row < order; row++) {
        for (col = 0; col < order; col++) {
            float coeff = *matrix++;
            m->data[row][col][0] = coeff;
            m->data[row][col][1] = coeff;
            m->data[row][col][2] = coeff;
            m->data[row][col][3] = coeff;
        }
    }
}

// Compute the matrix 'P' for FD coefficients, as defined by Henry's paper
void ComputeP(int order, float step, FDMatrix *m)
{
    int row, col, i;
    float f;

    // nxn matrix where n = order
    m->rows = m->columns = order;

    // Fill with zeros
    for (row = 0; row < order; row++) {
        for (col = 0; col < order; col++) {
            m->data[row][col][0] = 0.0f;
            m->data[row][col][1] = 0.0f;
            m->data[row][col][2] = 0.0f;
            m->data[row][col][3] = 0.0f;
        }
    }

    // Fill in diagonal
    for (i = order-1, f = 1.0f; i >= 0; i--, f *= step) {
        m->data[i][i][0] = f;
        m->data[i][i][1] = f;
        m->data[i][i][2] = f;
        m->data[i][i][3] = f;
    }
}

/*****************************************************************************/
// Fill in a column vector matrix using a sequence of strided xyzw values
void FillColumnVectorMatrix(int n, int stride, const float *data, FDMatrix *m, int col)
{
    int i;

    // nx1 matrix
    m->rows = n;    
    //this implicitly assumes that  we are filline this one column at a time
    //such that we have the right value when we are done (i.e. the calls are made)
    //col = 1... n whe col=n is the LAST call.
    m->columns = (col+1);       
    for (i = 0; i < n; i++) {
        m->data[i][col][0] = data[0]; m->data[i][col][1] = data[1];
        m->data[i][col][2] = data[2]; m->data[i][col][3] = data[3];
        data = (const float *)((const char *)data + stride);
    }
}

// Empty a column vector matrix into a sequence of strided xyzw values
void EmptyColumnVectorMatrix(int n, const FDMatrix *m, int stride, float *data, int col)
{
    int i;

    // Verify that it is a nx1 matrix
    nvAssert(m->rows == n);
    nvAssert(col < m->columns);

    // Empty data
    for (i = 0; i < n; i++) {
        // Copy data
        data[0] = m->data[i][col][0];
        data[1] = m->data[i][col][1];
        data[2] = m->data[i][col][2];
        data[3] = m->data[i][col][3];

        // Step pointer
        data = (float *)((char *)data + stride);
    }
}

/*****************************************************************************/
// Compare the U curve from a FDMatrix to a curve
void CompareUCurveMatrix(NV_PATCH_CURVE_INFO *dst, FDMatrix *m)
{
    int i, stride = 4*sizeof(float);
    float *data = dst->coeffs[0];

    nvAssert(dst->order == m->columns);
    // Empty data
    for (i = 0; i < dst->order; i++) {
        // Compare data
        nvAssert(EV_AS_INT(data[0]) == EV_AS_INT(m->data[0][i][0]));
        nvAssert(EV_AS_INT(data[1]) == EV_AS_INT(m->data[0][i][1]));
        nvAssert(EV_AS_INT(data[2]) == EV_AS_INT(m->data[0][i][2]));
        nvAssert(EV_AS_INT(data[3]) == EV_AS_INT(m->data[0][i][3]));

        // Step pointer
        data = (float *)((char *)data + stride);
    }
}

// Extract the U curve from a FDMatrix into a curve
void ExtractUCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *dst, const FDMatrix *m)
{
    dst->order = m->columns;
    memcpy(dst->coeffs[0], m->data[0][0], 4*m->columns*sizeof(float));
}

void ExtractUCurveSet(NV_PATCH_INFO *info, int evalEnables, FDCurveAttrSet *dst, FDMatrixAttrSet *pSet)
{
    int index;

    for (index = 0; index < info->maxAttr; index++) {
        if (evalEnables & (1 << index)) {
            ExtractUCurve(info, (*dst)[index], (*pSet)[index]);
        }
    }
}

/*****************************************************************************/
// Compare the V curve from a FDMatrix to a curve
void CompareVCurveMatrix(NV_PATCH_CURVE_INFO *dst, FDMatrix *m)
{
    int i, stride = 4*sizeof(float);
    float *data = dst->coeffs[0];

    nvAssert(dst->order == m->rows);
    // Empty data
    for (i = 0; i < dst->order; i++) {
        // Compare data
        nvAssert(EV_AS_INT(data[0]) == EV_AS_INT(m->data[i][0][0]));
        nvAssert(EV_AS_INT(data[1]) == EV_AS_INT(m->data[i][0][1]));
        nvAssert(EV_AS_INT(data[2]) == EV_AS_INT(m->data[i][0][2]));
        nvAssert(EV_AS_INT(data[3]) == EV_AS_INT(m->data[i][0][3]));

        // Step pointer
        data = (float *)((char *)data + stride);
    }
}

// Extract the V curve from a FDMatrix into a curve
void ExtractVCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *dst, FDMatrix *m)
{
    int i, stride = 4*sizeof(float);
    float *data = dst->coeffs[0];

    dst->order = m->rows;
    for (i = 0; i < dst->order; i++) {
        // Copy data
        CopyPoint(data, m->data[i][0]);

        // Step pointer
        data = (float *)((char *)data + stride);
    }
}

void ExtractVCurveSet(NV_PATCH_INFO *info, int evalEnables, FDCurveAttrSet *dst, FDMatrixAttrSet *pSet)
{
    int index;

    for (index = 0; index < info->maxAttr; index++) {
        if (evalEnables & (1 << index)) {
            ExtractVCurve(info, (*dst)[index], (*pSet)[index]);
        }
    }
}

/*****************************************************************************/
// Check the retessllation cache to see if we've cached some work here
int CheckRetessellationCache(NV_PATCH_INFO *info, unsigned int tag1, unsigned int tag2, FDMatrix **m){
        //m should be null because it will be overwritten here.
        nvAssert(*m==NULL);

        if (RetessFDtag[0][0] == tag1 && RetessFDtag[0][1] == tag2) {
            *m = RetessFDCache[0];
            RetessFDMRU = 0;
        } else if (RetessFDtag[1][0] == tag1 && RetessFDtag[1][1] == tag2){
            *m = RetessFDCache[1];
            RetessFDMRU = 1;
        } else {
            *m = (FDMatrix *)MemAlloc(sizeof(FDMatrix));

            if (!*m)
            {
                DXGRIP("Unable to alloc matrix -- out of memory.");
            }

            //replace cache slot with newly computed value
            RetessFDMRU = (1-RetessFDMRU);
            if (RetessFDCache[RetessFDMRU]) MemFree(RetessFDCache[RetessFDMRU]);
            RetessFDCache[RetessFDMRU] = *m;
            RetessFDtag[RetessFDMRU][0] = tag1;
            RetessFDtag[RetessFDMRU][1] = tag2;
            return 0;
        }
        
        return 1;
}

/*****************************************************************************/
// Compute FD coefficients from curve control points
void ComputeFDCurveCoeffsFlip(NV_PATCH_INFO *info, int order, float step, float *coeffs,
                const float *points, int stride, int dir, int flipped)
{
    FDMatrix *basis, *m, *p;
    FDMatrix *temp[3];
    EV_ALLOC_DECL();
    unsigned int tag1, tag2;

    EV_ALLOC_INIT_WITH_CACHE(info->context, 5 * sizeof(FDMatrix), computeFDCurveCoeffsEnd, info->pCache[EV_CACHE_9FDMATRICES]);
    EV_ALLOC(basis, info->context, sizeof(FDMatrix));
    EV_ALLOC(m, info->context, sizeof(FDMatrix));
    EV_ALLOC(p, info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[0], info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[1], info->context, sizeof(FDMatrix));
    EV_ALLOC_FULL(info->context);

    if (flipped) {
        stride = -stride;
    }
    // Reverse direction of points if necessary
    if (!dir) {
        points = (const float *)((const char *)points + (order-1)*stride);
        stride = -stride;
    }

    temp[2] = NULL;
    tag1 = (0xFFFF0000 | order);
    tag2 = *(unsigned int *)(&step);
    if(!CheckRetessellationCache(info, tag1, tag2, &temp[2])){
        // Compute matrices for the calculation
        ComputeBasis(order, 0, basis);
        ComputePowerToFD(order, m, 0);
        ComputeP(order, step, p);
        MatrixMult(info, temp[1], p, basis);
        MatrixMult(info, temp[2], m, temp[1]);

    }
    // Compute FD curve coefficients
    FillColumnVectorMatrix(order, stride, points, temp[0], 0);
    MatrixMult(info, temp[1], temp[2], temp[0]);
    EmptyColumnVectorMatrix(order, temp[1], 4*sizeof(float), coeffs, 0);

computeFDCurveCoeffsEnd:
    EV_FREE(basis, info->context);
    EV_FREE(m, info->context);
    EV_FREE(p, info->context);
    EV_FREE(temp[0], info->context);
    EV_FREE(temp[1], info->context);
}

void ComputeFDCurveCoeffs(NV_PATCH_INFO *info, int order, float step, float *coeffs,
                          const float *points, int stride, int dir)
{
    ComputeFDCurveCoeffsFlip(info, order, step, coeffs, points, stride, dir, 0);
}

/*****************************************************************************/
// Compute FD coefficients from curve control points
void RetessellateFDCurveInOut(NV_PATCH_INFO *info, float stepOld, float stepNew,
                    NV_PATCH_CURVE_INFO *curve, NV_PATCH_CURVE_INFO *curveOut)
{
    FDMatrix *m, *p, *mInv;
    FDMatrix *temp[3];
    EV_ALLOC_DECL();
    float ratio;
    unsigned int tag1, tag2;

    EV_ALLOC_INIT_WITH_CACHE(info->context, 5 * sizeof(FDMatrix), computeFDCurveCoeffsEnd, info->pCache[EV_CACHE_9FDMATRICES]);
    EV_ALLOC(m, info->context, sizeof(FDMatrix));
    EV_ALLOC(p, info->context, sizeof(FDMatrix));
    EV_ALLOC(mInv, info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[0], info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[1], info->context, sizeof(FDMatrix));
    EV_ALLOC_FULL(info->context);

    ratio = stepNew / stepOld;
    temp[2] = NULL;
    tag1 = (*(unsigned int*)(&ratio));
    tag2 = (unsigned int)(curve->order);
    if(ratio != 1.0){
        if(!CheckRetessellationCache(info, tag1, tag2, &temp[2])){
            // Compute matrices for the calculation
            ComputePowerToFD(curve->order, m, 0);
            ComputeP(curve->order, ratio, p);
            ComputePowerToFD(curve->order, mInv, 1);
            MatrixMult(info, temp[1], p, mInv);
            MatrixMult(info, temp[2], m, temp[1]);       
        }

        // Compute FD curve coefficients
        FillColumnVectorMatrix(curve->order, 4*sizeof(float), &curve->coeffs[0][0], temp[0], 0);
        MatrixMult(info, temp[1], temp[2], temp[0]);
        EmptyColumnVectorMatrix(curve->order, temp[1], 4*sizeof(float), &curveOut->coeffs[0][0], 0);
        curveOut->order = curve->order;
    }

computeFDCurveCoeffsEnd:
    EV_FREE(m, info->context);
    EV_FREE(p, info->context);
    EV_FREE(mInv, info->context);
    EV_FREE(temp[0], info->context);
    EV_FREE(temp[1], info->context);
}

// Compute FD coefficients from curve control points
unsigned int RetessellateFDCurveSetInOut(NV_PATCH_INFO *info, int evalEnables,
                    float stepOld, float stepNew,
                    FDCurveAttrSet *pIn, FDCurveAttrSet *pOut)
{
    int index;

    for (index = 0; index < info->maxAttr; index++) {
        if (evalEnables & (1 << index)) {
            RetessellateFDCurveInOut(info, stepOld, stepNew, (*pIn)[index], (*pOut)[index]);
        }
    }
    return 0;
}

void RetessellateFDCurve(NV_PATCH_INFO *info, float stepOld, float stepNew,
                                NV_PATCH_CURVE_INFO *curve)
{
    RetessellateFDCurveInOut(info, stepOld, stepNew, curve, curve);
}

/*****************************************************************************/

void OffsetCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *curve, int n)
{
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < curve->order-1; j++) {
            AddPoint(curve->coeffs[j], curve->coeffs[j+1]);
        }
    }
}

void OffsetCurveSet(NV_PATCH_INFO *info, int evalEnables, FDCurveAttrSet *pSet, int n)
{
    int index;

    for (index = 0; index < info->maxAttr; index++) {
        if (evalEnables & (1 << index)) {
            OffsetCurve(info, (*pSet)[index], n);
        }
    }
}

// Offset curve coefficients by n steps
void OffsetCoeffs(float *coeffs, int order, int n)
{
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < order-1; j++) {
            coeffs[4*j+0] += coeffs[4*(j+1)+0];
            coeffs[4*j+1] += coeffs[4*(j+1)+1];
            coeffs[4*j+2] += coeffs[4*(j+1)+2];
            coeffs[4*j+3] += coeffs[4*(j+1)+3];
        }
    }
}

/*****************************************************************************/
// Evaluate along a curve
void EvalCurve(const float *curve, int order, int n, float *outputs, int stride, int revIdx)
{
    int i, j;
    float temp[NV_PATCH_MAX_ORDER][4];

    if (revIdx) {
        outputs = (float *)((char *)outputs + (n-1)*stride);
        stride = -stride;
    }

    // Initialize FD
    for (j = 0; j < order; j++) {
        temp[j][0] = curve[4*j + 0];
        temp[j][1] = curve[4*j + 1];
        temp[j][2] = curve[4*j + 2];
        temp[j][3] = curve[4*j + 3];
    }

    for (i = 0; i < n; i++) {
        // Output a vertex
        outputs[0] = temp[0][0];
        outputs[1] = temp[0][1];
        outputs[2] = temp[0][2];
        outputs[3] = temp[0][3];
        outputs = (float *)((char *)outputs + stride);

        // Step FD
        for (j = 0; j < order-1; j++) {
            AddPoint(temp[j], temp[j+1]);
        }
    }
}

// Evaluate all the enabled curves along a strip in either direction
void EvalStrip(NV_PATCH_INFO *info,
                       FDCurveAttrSet *curves,
                       int n, NV_PATCH_EVAL_OUTPUT *pOutput, int revIdx)
{
    int index;

    for (index = 0; index < info->maxAttr; index++) {
        if (info->evalEnables & (1 << index)) {
            EvalCurve((*curves)[index]->coeffs[0], (*curves)[index]->order,
                n, &pOutput->vertexAttribs[0][index][0],
                4*sizeof(float)*NV_PATCH_NUMBER_OF_ATTRIBS, revIdx);
        }
    }
}

/*****************************************************************************/
#define EVAL_IWRAP(serial) ((serial) % NV_PATCH_MAX_SW_VERTICES)

// Evaluate attr0 along a curve
void EvalCurveIdx(int attr, int order, const float *curve, int n, NV_PATCH_EVAL_OUTPUT *pOutput, int startIdx, int revIdx)
{
    int i, j, sign = 1, idx;
    float temp[NV_PATCH_MAX_ORDER][4];

    if (revIdx) {
        startIdx = startIdx + n - 1;
        sign = -1;
    }

    // Initialize FD
    for (j = 0; j < order; j++) {
        temp[j][0] = curve[4*j + 0];
        temp[j][1] = curve[4*j + 1];
        temp[j][2] = curve[4*j + 2];
        temp[j][3] = curve[4*j + 3];
    }

    for (i = 0; i < n; i++) {
        // Output a vertex
        nvAssert((startIdx + i*sign) >= 0);
        idx = EVAL_IWRAP(startIdx + i*sign);
        CopyPoint(pOutput->vertexAttribs[idx][0], temp[0]);

        // Step FD
        for (j = 0; j < order-1; j++) {
            AddPoint(temp[j], temp[j+1]);
        }
    }
}

// Evaluate all the enabled curves along a strip in either direction
void EvalStripIdx(NV_PATCH_INFO *info,
                       FDCurveAttrSet *curves,
                       int n, NV_PATCH_EVAL_OUTPUT *pOutput, int startIdx,
                       int revIdx)
{
    int index;

    for (index = 0; index < info->maxAttr; index++) {
        if (info->evalEnables & (1 << index)) {
            // call pretending current attr is attr0
            EvalCurveIdx(index, (*curves)[index]->order, (*curves)[index]->coeffs[0],
                n, (NV_PATCH_EVAL_OUTPUT *)&pOutput->vertexAttribs[0][index], startIdx, revIdx);
        }
    }
}

/*****************************************************************************/
// Compute a FD matrix
void ComputeFDMatrixFlip(NV_PATCH_INFO *info, NV_PATCH_MAP_INFO *map, int uorder, int vorder, float ustep, float vstep,
                     FDMatrix *m,
                     const float *points, int vCoord, int uCoord,
                     int ufStride, int vfStride)
{
    FDMatrix *uCoeffs;
    FDMatrix *basisU, *basisV;
    FDMatrix *mU, *mV, *pU, *pV;
    FDMatrix *temp[3];
    int i, j;
    unsigned int tag1, tag2;
    EV_ALLOC_DECL();

    EV_ALLOC_INIT_WITH_CACHE(info->context, 9 * sizeof(FDMatrix), computeFDMatrixEnd, info->pCache[EV_CACHE_9FDMATRICES]);
    EV_ALLOC(uCoeffs, info->context, sizeof(FDMatrix));
    EV_ALLOC(basisU, info->context, sizeof(FDMatrix));
    EV_ALLOC(basisV, info->context, sizeof(FDMatrix));
    EV_ALLOC(mU, info->context, sizeof(FDMatrix));
    EV_ALLOC(mV, info->context, sizeof(FDMatrix));
    EV_ALLOC(pU, info->context, sizeof(FDMatrix));
    EV_ALLOC(pV, info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[0], info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[1], info->context, sizeof(FDMatrix));
    EV_ALLOC_FULL(info->context);

    if (map) {
        if (info->flags & NV_PATCH_FLAG_FLIPPED_U) {
            uCoord = (uorder - 1 - uCoord);
            ufStride = -ufStride;
        }
        if (info->flags & NV_PATCH_FLAG_FLIPPED_V) {
            vCoord = (vorder - 1 - vCoord);
            vfStride = -vfStride;
        }
        points += vCoord * map->vfStride + uCoord * map->ufStride;
    }

    // The matrix will be mxn, where n = uorder and m = vorder
    m->rows = vorder;
    m->columns = uorder;
    uCoeffs->rows = vorder;
    uCoeffs->columns = uorder;
    
        // Compute u-direction FD curve coefficients
    for (j = 0; j < vorder; j++) { FillColumnVectorMatrix(uorder, ufStride * sizeof(float), points + j*vfStride, temp[0], j); }
    temp[2] = NULL;
    tag1 = (0xFFFF0000 | uorder);
    tag2 = *(unsigned int *)(&ustep);
    if(!CheckRetessellationCache(info, tag1, tag2, &temp[2])){
        // Compute matrices in each direction
        ComputeBasis(uorder, 0, basisU);
        ComputePowerToFD(uorder, mU, 0);
        ComputeP(uorder, ustep, pU);

        MatrixMult(info, temp[1], pU, basisU);
        MatrixMult(info, temp[2], mU, temp[1]);
    }
    MatrixMult(info, temp[1], temp[2], temp[0]);
    for (j = 0; j < vorder; j++) { EmptyColumnVectorMatrix(uorder, temp[1], 4*sizeof(float), &uCoeffs->data[j][0][0], j);}

    // Compute v-direction FD curve coefficients
    for (i = 0; i < uorder; i++) { FillColumnVectorMatrix(vorder, 4*sizeof(float)*NV_PATCH_MAX_ORDER, &uCoeffs->data[0][i][0], temp[0], i); }
    temp[2] = NULL;
    tag1 = (0xFFFF0000 | vorder);
    tag2 = *(unsigned int *)(&vstep);
    if(!CheckRetessellationCache(info, tag1, tag2, &temp[2])){
        // Compute matrices in each direction
        ComputePowerToFD(vorder, mV, 0);
        ComputeBasis(vorder, 0, basisV);    
        ComputeP(vorder, vstep, pV);

        MatrixMult(info, temp[1], pV, basisV);
        MatrixMult(info, temp[2], mV, temp[1]);
    }
    MatrixMult(info, temp[1], temp[2], temp[0]);
    for (i = 0; i < uorder; i++) { EmptyColumnVectorMatrix(vorder, temp[1], 4*sizeof(float)*NV_PATCH_MAX_ORDER, &m->data[0][i][0], i); }
    
computeFDMatrixEnd:
    EV_FREE(uCoeffs, info->context);
    EV_FREE(basisU, info->context);
    EV_FREE(basisV, info->context);
    EV_FREE(mU, info->context);
    EV_FREE(mV, info->context);
    EV_FREE(pU, info->context);
    EV_FREE(pV, info->context);
    EV_FREE(temp[0], info->context);
    EV_FREE(temp[1], info->context);
}

void ComputeFDMatrix(NV_PATCH_INFO *info, int uorder, int vorder, float ustep, float vstep,
                             FDMatrix *m, const float *points, int ustride, int vstride)
// wrapper for now, eventually change all calls to be direct
{
    ComputeFDMatrixFlip(info, 0, uorder, vorder, ustep, vstep, m,
                (const float *)points, 0, 0, ustride, vstride);
}

// Offset an FD matrix by a steps in u and b steps in v
// Not particularly fast -- try to step along a curve when possible!
void OffsetFDMatrix(NV_PATCH_INFO *info, FDMatrix *m, int a, int b)
{
    int i, row, col;
    // Step in u
    for (i = 0; i < a; i++) {
        for (row = 0; row < m->rows; row++) {
            for (col = 0; col < m->columns-1; col++) {
                AddPoint(m->data[row][col], m->data[row][col+1]);
            }
        }
    }

    // Step in v
    for (i = 0; i < b; i++) {
        for (row = 0; row < m->rows-1; row++) {
            for (col = 0; col < m->columns; col++) {
                AddPoint(m->data[row][col], m->data[row+1][col]);
            }
        }
    }
}

void OffsetFDMatrix_Extract_Discard(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *dst, int dir, FDMatrix *m, int a, int b) {
    CopyMatrix(info, &info->tempMatrix, m);
    OffsetFDMatrix(info, &info->tempMatrix, a, b);
    if (dir) ExtractVCurve(info, dst, &info->tempMatrix);
    else ExtractUCurve(info, dst, &info->tempMatrix);
}

void OffsetFDMatrixSet(NV_PATCH_INFO *info, int evalEnables, FDMatrixAttrSet *pSet, int a, int b)
{
    int attr;

    for (attr = 0;  attr < info->maxAttr;  attr++) {
        if (!(evalEnables & (1 << attr))) {
            continue;
        }
        OffsetFDMatrix(info, (*pSet)[attr], a, b);
    }
}

// Compute FD coefficients from curve control points
void RetessellateFDMatrix(NV_PATCH_INFO *info, float stepOldU, float stepOldV,
                                 float stepNewU, float stepNewV, FDMatrix *matrix)
{
    const int uorder = matrix->columns;
    const int vorder = matrix->rows;
    float ratio;
    FDMatrix *m, *p, *mInv;
    FDMatrix *temp[3];
    int freeTemp2 = 1;

    EV_ALLOC_DECL();

    EV_ALLOC_INIT_WITH_CACHE(info->context, 5 * sizeof(FDMatrix), RetessellateFDMatrixEnd, info->pCache[EV_CACHE_9FDMATRICES]);
    EV_ALLOC(m, info->context, sizeof(FDMatrix));
    EV_ALLOC(p, info->context, sizeof(FDMatrix));
    EV_ALLOC(mInv, info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[0], info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[1], info->context, sizeof(FDMatrix));
    EV_ALLOC_FULL(info->context);

    // Compute matrices for the calculation
    ratio = stepNewU / stepOldU;
    if (ratio != 1.0) {
        temp[2] = NULL;
        if(!CheckRetessellationCache(info, (*(unsigned int*)(&ratio)), uorder, &temp[2])){
            //cache miss calculate the step matrix
            ComputePowerToFD(uorder, m, 0);
            ComputeP(uorder, ratio, p);
            ComputePowerToFD(uorder, mInv, 1);
            MatrixMult(info, temp[0], p, mInv);
            MatrixMult(info, temp[2], m, temp[0]);

        }
        CopyMatrixTranspose(info, temp[0], matrix);
        MatrixMult(info, temp[1], temp[2], temp[0]);
        CopyMatrixTranspose(info, matrix, temp[1]);
    }

    ratio = stepNewV / stepOldV;
    if (ratio != 1.0) {
        temp[2] = NULL;
        if(!CheckRetessellationCache(info, *(unsigned int*)(&ratio), vorder, &temp[2])){
            //cache miss calculate the step matrix
            ComputePowerToFD(vorder, m, 0);
            ComputeP(vorder, ratio, p);
            ComputePowerToFD(vorder, mInv, 1);

            MatrixMult(info, temp[0], p, mInv);
            MatrixMult(info, temp[2], m, temp[0]);
        }

        //don't need a copy cause its not a transpose...
        MatrixMult(info, temp[1], temp[2], matrix);
        CopyMatrix(info, matrix, temp[1]);
    }

RetessellateFDMatrixEnd:
    EV_FREE(m, info->context);
    EV_FREE(p, info->context);
    EV_FREE(mInv, info->context);
    EV_FREE(temp[0], info->context);
    EV_FREE(temp[1], info->context);
}
#undef RATIO
#undef ORDER

unsigned int RetessellateFDMatrixSet(NV_PATCH_INFO *info, int evalEnables,
                    float stepOldU, float stepOldV,
                    float stepNewU, float stepNewV, FDMatrixAttrSet *pSet)
{
    int attr;

    for (attr = 0;  attr < info->maxAttr;  attr++) {
        if (!(evalEnables & (1 << attr))) {
            continue;
        }
        RetessellateFDMatrix(info, stepOldU, stepOldV, stepNewU, stepNewV, (*pSet)[attr]);
    }
    return 0;
}

/*****************************************************************************/
// Compute a Bezier curve's midpoint given its control points
// This uses the de Casteljau algorithm.

void ComputeCurvePoint(int order, float *point, const float *points,
                       int stride, float t)
{
    float temp[NV_PATCH_MAX_ORDER][4];
    int i, j;

    // Copy points
    for (i = 0; i < order; i++) {
        temp[i][0] = points[0]; temp[i][1] = points[1];
        temp[i][2] = points[2]; temp[i][3] = points[3];
        points = (const float *)((const char *)points + stride);
    }

    // Compute midpoint
    for (i = order-1; i >= 1; i--) {
        for (j = 0; j < i; j++) {
            temp[j][0] = (1-t) * temp[j][0] + (t)*temp[j+1][0];
            temp[j][1] = (1-t) * temp[j][1] + (t)*temp[j+1][1];
            temp[j][2] = (1-t) * temp[j][2] + (t)*temp[j+1][2];
            temp[j][3] = (1-t) * temp[j][3] + (t)*temp[j+1][3];
        }
    }

    //note that this is 'point' variable and not 'points'
    point[0] = temp[0][0];
    point[1] = temp[0][1];
    point[2] = temp[0][2];
    point[3] = temp[0][3];
}

void ComputePatchPoint(int uorder, int vorder,
                  float *point, const float *points,
                  int ustride, int vstride,
                  float u, float v) {

    int j;
    float curvePoints[NV_PATCH_MAX_ORDER][4];

    //calculate offset curve in v direction.
    for (j = 0; j < uorder; j++) {
        ComputeCurvePoint(vorder, &curvePoints[j][0], points + 4*j, vstride, v);
    }
    ComputeCurvePoint(uorder, point, &curvePoints[0][0], 4*sizeof(float), u);
}

void ComputeTriPatchCenter(int order, float *point, const float *points,
                           int ustride, int vstride)
{
    float temp[NV_PATCH_MAX_ORDER][NV_PATCH_MAX_ORDER][4];
    const float *ptemp;
    int i, j, k;

    // Copy points
    for (j = 0; j < order; j++) {
        ptemp = (const float *)((const char *)points + j*vstride);
        for (i = 0; i < order; i++) {
            temp[j][i][0] = ptemp[0];
            temp[j][i][1] = ptemp[1];
            temp[j][i][2] = ptemp[2];
            temp[j][i][3] = ptemp[3];
            ptemp = (const float *)((const char *)ptemp + ustride);
        }
    }

    // Compute midpoint
    for (i = order-1; i >= 1; i--) {
        for (j = 0; j < i; j++) {
            for (k = 0; k < i-j; k++) {
                temp[k][j][0] = (temp[k][j][0] + temp[k][j+1][0] + temp[k+1][j][0]) / 3.0f;
                temp[k][j][1] = (temp[k][j][1] + temp[k][j+1][1] + temp[k+1][j][1]) / 3.0f;
                temp[k][j][2] = (temp[k][j][2] + temp[k][j+1][2] + temp[k+1][j][2]) / 3.0f;
                temp[k][j][3] = (temp[k][j][3] + temp[k][j+1][3] + temp[k+1][j][3]) / 3.0f;
            }
        }
    }

    //note that this is 'point' variable and not 'points'
    point[0] = temp[0][0][0];
    point[1] = temp[0][0][1];
    point[2] = temp[0][0][2];
    point[3] = temp[0][0][3];
}

const float triTo3QuadDomains[3][4][2] = {
    {
        {0, 0},
        {0.5f, 0},
        {0, 0.5f},
        {1/3.0f, 1/3.0f},
    },
    {
        {1, 0},
        {0.5f, 0},
        {0.5f, 0.5f},
        {1/3.0f, 1/3.0f},
    },
    {
        {0, 1},
        {0.5f, 0.5f},
        {0, 0.5f},
        {1/3.0f, 1/3.0f},
    },
};

const float triTo1QuadDomain[4][2] = {
    {0, 0},
    {1.0f, 0},
    {0, 1.0f},
    {0, 1.0f},
};

FDMatrix *ReduceTriPatch(NV_PATCH_INFO *info, int level, FDMatrix *p, float w0, float w1)
{
    FDMatrix *rp;
    int deg = p->rows-2;
    int i, j, k;
    float w2 = 1.0f - w0 - w1;

    rp = info->reduceTri[level];
    if (!rp) {
        return NULL;
    }
    rp->rows    = deg+1;
    rp->columns = deg+1;

    for (i = 0; i <= deg; i++) {
        for (j = 0; j <= deg-i; j++) {
            for (k = 0; k < 4; k++) {
                rp->data[i][j][k]  = w0*p->data[i+1][j][k];
                rp->data[i][j][k] += w1*p->data[i][j+1][k];
                rp->data[i][j][k] += w2*p->data[i][j][k];
            }
        }
    }
    return rp;
}

unsigned int ComputeTriToQuad(NV_PATCH_INFO *info, int level, FDMatrix *f, FDMatrix *g, FDMatrix *h,
                                    float **weights, int n, int mini,
                                    int si, int sj, float c, int mu)
{
    FDMatrix *fn;
    int indexi, indexj, firsti, i;
    unsigned int retVal;
    float bc[4];

    if (n == 0) {
        for (i = 0; i < 4; i++) {
            h->data[sj][si][i] += c * f->data[0][0][i];
        }
        weights[sj][si] += c;
        return 0;
    }

    firsti = 1;
    for (indexi = mini; indexi <= 1; indexi++) {
        if (firsti) {
            mu++;
            firsti = 0;
        } else {
            mu = 1;
        }
        for (indexj = 0; indexj <= 1; indexj++) {
            bc[0] = g->data[1 - indexi][indexj][0];
            bc[1] = g->data[1 - indexi][indexj][1];
            fn = ReduceTriPatch(info, level, f, bc[1], bc[0]);
            if (!fn) {
                return NV_PATCH_EVAL_ERR_OUT_OF_MEMORY;
            }
            retVal = ComputeTriToQuad(info, level+1, fn, g, h,
                                    weights, n-1, indexi,
                                    si + 1 - indexi,
                                    sj + indexj,
                                    c*n/mu,
                                    mu);
            if (retVal) {
                return retVal;
            }
        }
    }
    return 0;
}

unsigned int ReparameterizeTriPatch(NV_PATCH_INFO *info, FDMatrix *in,
                                           FDMatrix *out, int domain, int frac)
{
    FDMatrix domainMatrix;
    float *weights[NV_PATCH_MAX_ORDER], invWeight;
    int i, j;
    EV_ALLOC_DECL();

    EV_ALLOC_INIT_WITH_CACHE(info->context, NV_PATCH_MAX_ORDER*NV_PATCH_MAX_ORDER*sizeof(float)
                                            + (in->rows-1) * sizeof(FDMatrix),
                        reparameterizeTriPatchOutOfMemory, info->pCache[EV_CACHE_TRI_WEIGHTS]);

    for (i = 0; i < NV_PATCH_MAX_ORDER; i++) {
        EV_ALLOC(weights[i], info->context, NV_PATCH_MAX_ORDER*sizeof(float));
        for (j = 0; j < NV_PATCH_MAX_ORDER; j++) {
            out->data[i][j][0] = 0.0f;
            out->data[i][j][1] = 0.0f;
            out->data[i][j][2] = 0.0f;
            out->data[i][j][3] = 0.0f;
            weights[i][j] = 0.0f;
        }
    }
    for (i = 0;  i < in->rows-1;  i++) {
        EV_ALLOC(info->reduceTri[i], info->context, sizeof(FDMatrix));
    }
    EV_ALLOC_FULL(info->context);

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (frac) {
                domainMatrix.data[i][j][0] = triTo3QuadDomains[domain][j*2+i][0];
                domainMatrix.data[i][j][1] = triTo3QuadDomains[domain][j*2+i][1];
            } else {
                domainMatrix.data[i][j][0] = triTo1QuadDomain[j*2+i][0];
                domainMatrix.data[i][j][1] = triTo1QuadDomain[j*2+i][1];
            }
        }
    }

    info->retVal |= ComputeTriToQuad(info, 0, in, &domainMatrix, out,
                                    weights, in->rows-1, 0,
                                    0, 0, 1.0f, 0);

    out->rows = in->rows;
    out->columns = in->columns;
    for (i = 0; i < out->rows; i++) {
        for (j = 0; j < out->columns; j++) {
            invWeight = 1.0f / weights[i][j];
            out->data[i][j][0] *= invWeight;
            out->data[i][j][1] *= invWeight;
            out->data[i][j][2] *= invWeight;
            out->data[i][j][3] *= invWeight;
        }
    }

reparameterizeTriPatchOutOfMemory:
    for (i = 0; i < NV_PATCH_MAX_ORDER; i++) {
        if (weights[i]) {
            EV_FREE(weights[i], info->context);
        }
    }

    return info->retVal;
}

void SendTransitionEndPoints(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo,
                    FDCurveAttrSet **ppCurvesInnerList, int revTran, int nCurveIn,
                    int gridIdxV, int gridIdxU, int cornerIdxV, int cornerIdxU, int doFrac, int transitionFlag)
{
    int index;
    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

    // SEND THE OUTER ENDPOINT
    HOS_NOTE("Send Outer Transition endpoint"); //always send this...
    pPush = pDevice->StartPush(CountLoopPushes(info));
    Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_OUTER_END_PT);
    pPush += 2;
    for (index = 0; index < info->maxAttr; index++) {
        if (info->evalEnables & (1 << index)) {
            HOS_NOTE("Set Curve Coefficients");
            PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
            *(pPush + 1) = VIEW_AS_DWORD(quadInfo->pCorners[index][cornerIdxV][cornerIdxU][0]);
            *(pPush + 2) = VIEW_AS_DWORD(quadInfo->pCorners[index][cornerIdxV][cornerIdxU][1]);
            *(pPush + 3) = VIEW_AS_DWORD(quadInfo->pCorners[index][cornerIdxV][cornerIdxU][2]);
            *(pPush + 4) = VIEW_AS_DWORD(quadInfo->pCorners[index][cornerIdxV][cornerIdxU][3]);
            pPush += 5;
        }
    }
    Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
    pDevice->EndPush(pPush + 2);

    if (!doFrac) {
        // SEND THE INNER ENDPOINT -- integer case
        // this should only happen in degenerate transitions cases
        //ffif (quadInfo->nv0 == 1 && quadInfo->nv1 == 1)
        {
            float *pPoint;

            //HACK HACK HACK  -- just testing out a theory on the degenerate case Fred gave me...
            //Fix this up.
            HOS_NOTE("Send Inner Transition endpoint");
            pPush = pDevice->StartPush();
            Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_INNER_END_PT);
            pDevice->EndPush(pPush + 2);
            //ffif (transitionFlag == LAST_ROW || transitionFlag == FIRST_ROW)
            {
                for (index = 0; index < info->maxAttr; index++) {
                    if (info->evalEnables & (1 << index)) {
                        //in degenerate case use patch corners not grid corners
                        // XXX gridCorner is not set up for some degen cases. WHY?
                        //ffpPoint = info->gridCorner[index][gridIdxV][gridIdxU];

                        if (transitionFlag == FIRST_ROW) {
                            //ffpPoint = quadInfo->pCorners[index][cornerIdxV][cornerIdxU];
                            pPoint = quadInfo->pCorners[index][quadInfo->vMaxDir][quadInfo->uMaxDir];

                        } else if (transitionFlag == LAST_ROW) {
                            pPoint = quadInfo->pCorners[index][1-quadInfo->vMaxDir][quadInfo->uMaxDir];

                        } else if (transitionFlag == FIRST_COL) {
                            pPoint = quadInfo->pCorners[index][quadInfo->vMaxDir][quadInfo->uMaxDir];

                        } else if (transitionFlag == LAST_COL) {
                            pPoint = quadInfo->pCorners[index][quadInfo->vMaxDir][1-quadInfo->uMaxDir];

                        } else {
                            pPoint = info->gridCorner[index][gridIdxV][gridIdxU];
                        }
                        HOS_NOTE("Set Curve Coefficients");
                        pPush = pDevice->StartPush(5);
                        PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
                        *(pPush + 1) = VIEW_AS_DWORD(pPoint[0]);
                        *(pPush + 2) = VIEW_AS_DWORD(pPoint[1]);
                        *(pPush + 3) = VIEW_AS_DWORD(pPoint[2]);
                        *(pPush + 4) = VIEW_AS_DWORD(pPoint[3]);
                        pDevice->EndPush(pPush + 5);
                    }
                }
            }
        }

    } else {
        // SEND THE INNER ENDPOINT -- fractional case
        if (transitionFlag == FIRST_ROW) {
            float *pPoint;
            HOS_NOTE("Send Inner Transition endpoint");
            pPush = pDevice->StartPush(CountLoopPushes(info));
            Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_INNER_END_PT);
            pPush += 2;
            for (index = 0; index < info->maxAttr; index++) {
                if (info->evalEnables & (1 << index)) {
                    if (quadInfo->uMaxSegs > 1) {
                        //ffpPoint = info->gridCorner[index][0][1];
                        pPoint = info->gridCorner[index][gridIdxV][gridIdxU];
                    } else {
                        //in degenerate case use patch corners not grid corners
                        //ffpPoint = quadInfo->pCorners[index][1][1];
                        //ffpPoint = quadInfo->pCorners[index][cornerIdxV][cornerIdxU];
                        pPoint = quadInfo->pCorners[index][quadInfo->vMaxDir][cornerIdxU];
                    }
                    HOS_NOTE("Set Curve Coefficients");
                    PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
                    *(pPush + 1) = VIEW_AS_DWORD(pPoint[0]);
                    *(pPush + 2) = VIEW_AS_DWORD(pPoint[1]);
                    *(pPush + 3) = VIEW_AS_DWORD(pPoint[2]);
                    *(pPush + 4) = VIEW_AS_DWORD(pPoint[3]);
                    pPush += 5;
                }
            }
            Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
            pDevice->EndPush(pPush + 2);
            HOS_FLUSH();

        } else if (transitionFlag == FIRST_COL) {
            float *pPoint;
            HOS_NOTE("Send Inner Transition endpoint");
            pPush = pDevice->StartPush(CountLoopPushes(info));
            Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_INNER_END_PT);
            pPush += 2;
            for (index = 0; index < info->maxAttr; index++) {
                if (info->evalEnables & (1 << index)) {
                    if (doFrac && (quadInfo->vMaxSegs == 1)) {
                        CopyCurve(info, &info->tempCurve, (*ppCurvesInnerList[!revTran ? 0 : nCurveIn-1])[index]);
                        OffsetCurve(info, &info->tempCurve,1);
                        pPoint = (float*)&info->tempCurve.coeffs[0];
                    } else if (quadInfo->vMaxSegs > 1) {
                        //ffpPoint = info->gridCorner[index][1][0];
                        pPoint = info->gridCorner[index][gridIdxV][gridIdxU];
                    } else {
                        //in degenerate case use patch corners not grid corners
                        //ffpPoint = quadInfo->pCorners[index][1][1];
                        pPoint = quadInfo->pCorners[index][cornerIdxV][cornerIdxU];
                    }
                    HOS_NOTE("Set Curve Coefficients");
                    PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
                    *(pPush + 1) = VIEW_AS_DWORD(pPoint[0]);
                    *(pPush + 2) = VIEW_AS_DWORD(pPoint[1]);
                    *(pPush + 3) = VIEW_AS_DWORD(pPoint[2]);
                    *(pPush + 4) = VIEW_AS_DWORD(pPoint[3]);
                    pPush += 5;
                }
            }
            Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
            pDevice->EndPush(pPush + 2);
            HOS_FLUSH();
        }
    }
}

/*****************************************************************************/
// where do the transition curves start?
// for INT cases, they all start on the swatch boundary
// for FRAC cases, curves are rooted as follows:
//  TOP:
//      LAST ROW outer, at actual UL corner that starts the stitch
//      LAST ROW inner, at left of swatch, like INT stitch top
//  RIGHT:
//      LAST COL outer, at actual LR corner that starts the stitch
//      LAST COL inner, at bot of swatch, like INT stitch right
//  BOT:
//      FIRST ROW outer, at LL of swatch, like INT stitch bot
//      FIRST ROW inner, at left of swatch, like INT stitch bot
//  LEFT:
//      FIRST COL outer, at LL of swatch, like INT stitch left
//      FIRST COL inner, at bot of swatch, like INT stitch left
//
// so, the normal "exceptions" are the TOP/RIGHT outer curves
// but, for degenerates:
// but, when there is no FRAC TOP stitch,   then the BOT FIRST ROW inner starts at actual UL of stitch (like TOP LAST ROW outer)
// and, when there is no FRAC RIGHT stitch, either nInnerU > nInnerV and there's no FRAC TOP, or transpose to get same case

void SendFirstInnerTransitionCurve(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo,
                    FDCurveAttrSet **ppCurvesInnerList, int revTran, int nCurveIn, int doFrac, int transitionFlag)
{
    int index, i;
    NV_PATCH_CURVE_INFO *pCurve;
    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

    // SEND THE FIRST INNER TRANSITION CURVE
    HOS_NOTE("Begin Curve: Send Inner Transition");
    pPush = pDevice->StartPush(CountLoopPushes(info));
    Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_INNER_TRANSITION_CURVE);
    pPush += 2;
    for (index = 0; index < info->maxAttr; index++) {
        if (info->evalEnables & (1 << index)) {
            pCurve = (*ppCurvesInnerList[!revTran ? 0 : nCurveIn-1])[index];
            if (doFrac && (quadInfo->vMaxSegs == 1) && (transitionFlag == FIRST_ROW)) {
                CopyCurve(info, &info->tempCurve, pCurve);
                OffsetCurve(info, &info->tempCurve,1);
                pCurve = &info->tempCurve;
            }

            // if this is a not a new swath... do we pre adjust that row??? or just not send the last row in the previous patch?
            for (i = 0; i < pCurve->order; i++) {
                HOS_NOTE("Set Curve Coefficients");
                PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
                *(pPush + 1) = VIEW_AS_DWORD(pCurve->coeffs[i][0]);
                *(pPush + 2) = VIEW_AS_DWORD(pCurve->coeffs[i][1]);
                *(pPush + 3) = VIEW_AS_DWORD(pCurve->coeffs[i][2]);
                *(pPush + 4) = VIEW_AS_DWORD(pCurve->coeffs[i][3]);
                pPush += 5;
            }
        }
    }
    Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
    pDevice->EndPush(pPush + 2);
    HOS_FLUSH();
}

void SendFirstOuterTransitionCurve(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, FDCurveAttrSet *pCurvesOuter,
                    int doFrac, int transitionFlag)
{
    int index, i;
    NV_PATCH_CURVE_INFO *pCurve;
    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

    // SEND THE FIRST OUTER TRANSITION CURVE
    HOS_NOTE("Send Outer Transition");
    pPush = pDevice->StartPush(CountLoopPushes(info));
    Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_OUTER_TRANSITION_CURVE);
    pPush += 2;
    for (index = 0; index < info->maxAttr; index++) {
        if (info->evalEnables & (1 << index)) {
            pCurve = (*pCurvesOuter)[index];
            //if this is a not a new swath... do we pre adjust that row??? or just not send the last row in the previous patch?
            if (doFrac && (transitionFlag == LAST_ROW || transitionFlag == LAST_COL)) {
                CopyCurve(info, &info->tempCurve, pCurve);
                OffsetCurve(info, &info->tempCurve,1);
                pCurve = &info->tempCurve;
            }

            for (i = 0; i < pCurve->order; i++) {
                HOS_NOTE("Set Curve Coefficients");
                PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
                *(pPush + 1) = VIEW_AS_DWORD(pCurve->coeffs[i][0]);
                *(pPush + 2) = VIEW_AS_DWORD(pCurve->coeffs[i][1]);
                *(pPush + 3) = VIEW_AS_DWORD(pCurve->coeffs[i][2]);
                *(pPush + 4) = VIEW_AS_DWORD(pCurve->coeffs[i][3]);
                pPush += 5;
            }
        }
    }
    Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
    pDevice->EndPush(pPush + 2);
    HOS_FLUSH();
}

/*****************************************************************************/
// Draw stitch for INT or FRAC cases
// gridIdx* are indices for inside endpoint:
//      info->gridCorner[index][gridIdxV][gridIdxU]
// cornerIdx* are indices for outside endpoint:
//      quadInfo->pCorners[index][cornerIdxV][cornerIdxU]

void DrawIntStitch(int stripU, NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo,
                    FDCurveAttrSet **ppCurvesOuter, FDCurveAttrSet **ppCurvesInnerList,
                    int outSegs, int inSegs,
                    NV_PATCH_EVAL_OUTPUT *pDataOut, NV_PATCH_EVAL_OUTPUT *pDataIn,
                    int gridIdxV, int gridIdxU,
                    int cornerIdxV, int cornerIdxU,
                    int stitchLowOut, int stitchHighOut,
                    int stitchLowIn, int stitchHighIn,
                    int qDir, int qOtherDir, int qMaxDir,
                    int reverse, int ifEqualStepInner, int transitionFlag)
{
    // World's messiest state machine.  I wish this was cleaner. -- MJC
    // ... and now perhaps messier, but does swathing. -- FBF
    // and may even match the HW
    int dist, in, out, i, first;
    int inStrip, lastWasIn;
    int fanCenter, fanFirst, fanLast;
    int idxInnerEnd;
    int index;
    FDCurveAttrSet *pCurves;
    int doFrac = !!(info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL);
    //ff// calc swaths WITHOUT including the ending FRAC-only stitches
    //ffint nSwatchU = (quadInfo->uMaxSegs - 1 - doFrac) / info->maxSwatch + 1;
    //ffint nSwatchV = (quadInfo->vMaxSegs - 1 - doFrac) / info->maxSwatch + 1;
    int nCurveOut;
    int nCurveIn; // = stripU ? nSwatchU : nSwatchV;
    int revTran = qDir != qMaxDir;          // qDir is from outerDir
    int revInner = qOtherDir != qMaxDir;    // qOtherDir
    int totalToDoOut, totalToDoIn;
    int onCurveOut = 1, onCurveIn = 1;
    int oneToDoOut, oneToDoIn;
    int onLowIn, onHighIn;
    int innerCurve;         // temp to hold curve number to select from ppCurvesInnerList
    int lastInsideCurve;    // means last in list. Not necessarily last drawn.
    int lastOutsideCurve;   // last drawn
    //ffint lastFracInner = 0;  // for special case where last frac curve actually consumes 2 curves
    int lastFrontFace;

    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

    nCurveOut = (outSegs - 1) / info->maxSwatch + 1;
    nCurveIn =  (inSegs - 1 - doFrac) / info->maxSwatch + 1;
    nvAssert(qOtherDir == qMaxDir);
    nvAssert((outSegs <= inSegs)  ||  doFrac);

    ifEqualStepInner ^= info->flipUV;

    out = 0;
    in = 0;
    dist = inSegs - outSegs;
    totalToDoOut = 1+outSegs; totalToDoIn = 1+inSegs;

    if (totalToDoOut > 1+info->maxSwatch) {
        oneToDoOut = info->maxSwatch;   // doesn't fit in one curve
        lastOutsideCurve = 0;
    } else {
        oneToDoOut = totalToDoOut;
        lastOutsideCurve = 1;
    }

    if (totalToDoIn > 1+info->maxSwatch) {
        // doesn't fit in one curve
        if (revTran) {
            // reversed inner curve
            oneToDoIn = inSegs % info->maxSwatch;
            ASSERT(oneToDoIn >= 0);
            if ((oneToDoIn - stitchHighIn) <= 0) {
                if ((oneToDoIn == 1)  &&  stitchHighIn /* && (nCurveIn == 2)*/) {
                    nCurveIn--;
                }
                oneToDoIn += info->maxSwatch; // + stitchHighIn;
            }
            oneToDoIn++;
            lastInsideCurve = 1;
            onLowIn = 0;
            onHighIn = stitchHighIn;
        } else {
            if (doFrac && (nCurveIn == 1) && (totalToDoIn == (2+info->maxSwatch))) {
                oneToDoIn = 2+info->maxSwatch;
                lastInsideCurve = 1;
            } else {
                oneToDoIn = info->maxSwatch;
                lastInsideCurve = 0;
            }
            onLowIn = stitchLowIn;
            onHighIn = 0;
        }

    } else {
        // fits in one curve
        oneToDoIn = totalToDoIn;
        lastInsideCurve = 1;
        onLowIn = stitchLowIn;
        onHighIn = stitchHighIn;
    }

    if (revTran) {
        idxInnerEnd = in;
    } else {
        nvAssert(qDir == qOtherDir);
        idxInnerEnd = oneToDoIn-1;
    }

    //setup transition
    unsigned long TControl[3], order;
    unsigned long numcoeffs = 0x0;
    int hwAttr;
    TControl[0] = TControl[1] = TControl[2] = 0x0;
    for (hwAttr = 0; hwAttr < 7; hwAttr++) {                //TPControl0
        if (info->evalEnables & (1 << hwAttr)) {
            order = (*ppCurvesOuter[0])[hwAttr]->order;
            ASSERT((int)order == (*ppCurvesInnerList[0])[hwAttr]->order);
            TControl[0] |= ((order-1) << (hwAttr<<2));      //hwAttr*4
            numcoeffs += order;
        }
    }
    for (hwAttr = 8; hwAttr < 15; hwAttr++) {               //TPControl1
        if (info->evalEnables & (1 << hwAttr)) {
            order = (*ppCurvesOuter[0])[hwAttr]->order;
            ASSERT((int)order == (*ppCurvesInnerList[0])[hwAttr]->order);
            TControl[1] |= ((order-1) << (hwAttr<<2));
            numcoeffs += order;
        }
    }

    TControl[2] = (numcoeffs << 24) | (outSegs << 10) | inSegs;
    HOS_NOTE("Set Begin Transition");
    pPush = pDevice->StartPush(4);
    PushCount(pPush, NV097_SET_BEGIN_TRANSITION0, 3);
    *(pPush + 1) = TControl[0];
    *(pPush + 2) = TControl[1];
    *(pPush + 3) = TControl[2];
    pDevice->EndPush(pPush + 4);

    //SEND Transition endpoints as necessary
    SendTransitionEndPoints(info, quadInfo, ppCurvesInnerList, revTran, nCurveIn,
                            gridIdxV, gridIdxU, cornerIdxV, cornerIdxU, doFrac, transitionFlag);

    //SEND Beginning Transition Curves
    SendFirstInnerTransitionCurve(info, quadInfo, ppCurvesInnerList, revTran, nCurveIn, doFrac, transitionFlag);
    SendFirstOuterTransitionCurve(info, quadInfo, ppCurvesOuter[0], doFrac, transitionFlag);

    if (inSegs != outSegs) {
        // if we have a reversal, bias the stitching direction so it comes
        // out the same as the non reversed case.
        int lastTrans = (transitionFlag == LAST_ROW  || transitionFlag == LAST_COL) ? 1 : 0;
        int rowTrans =  (transitionFlag == FIRST_ROW || transitionFlag == LAST_ROW) ? 1 : 0;
        int fudge;
        if (rowTrans) {
            fudge = !(revTran ^ (lastTrans)); // row
        } else {
            fudge = !(revTran ^ (!lastTrans)); // col
        }
        dist += fudge;
    } else {
        // when segs equal, initial dist == 0. if dist==0 means to select
        // inner edge first, then adjust 'dist' to step inner edge first.
        if (ifEqualStepInner) {
            dist += 1;
        }
    }

    // The algorithm in the Unit test (and presumably the actual HW) assumes
    // the first outside and inside points have been sent (accounted for), even
    // though the starting 'totalToDoIn' and 'oneToDoIn' do not reflect this.
    // Similarly for 'oneToDoOut' and 'totalToDoOut'. That's why they start out
    // with the total counts.
    // Here we fudge an "extra" loop to avoid duplicating the code to send
    // and inside curve (by setting 'first'). To compensate, adjust the
    // various counts.
    first = 1;
    dist += 2*outSegs;
    in--;
    oneToDoIn++;
    totalToDoIn++;
    inStrip = 1;
    lastWasIn = 0;

    // The FD unit test algorithm does "extra" steps at the end which don't
    // matter for HW, but cause extra lines for SW emulation.
    // That's why this code checks for "> 1" instead of "> 0"

    if (doFrac) {
        // fudge things to get alternating out-in-out-in-out ... out
        // NOTE: first 'out' already sent, so start with 'in' below.
        dist = 1;       // set up to alternate in-out-in-out ...
        outSegs = inSegs = 1;       // from now on, only used by 'dist' adjustment
    }

    //tag:stst
    while (totalToDoOut > 1  ||  totalToDoIn > 1) {
        while (first || ((totalToDoIn > 1) && (dist > 0))) {
            // step and use an inside point
            first = 0;
            dist -= 2*outSegs;
            in++;
            oneToDoIn--;
            totalToDoIn--;

            if (oneToDoIn == 0  &&  totalToDoIn > 1) {
                // need another inside curve
                if (totalToDoIn > (1+info->maxSwatch+doFrac)) {
                    oneToDoIn = info->maxSwatch;
                    onLowIn = 0;
                    onHighIn = 0;
                    lastInsideCurve = 0;
                } else {
                    oneToDoIn = totalToDoIn;
                    onLowIn = stitchLowIn && revTran;
                    onHighIn = stitchHighIn && !revTran;
                    if (!revTran) {
                        lastInsideCurve = 1;
                    } else {
                        lastInsideCurve = 0;
                    }
                    //ffif (doFrac) {
                        //ffif (totalToDoIn == (2+info->maxSwatch)) {
                            //ff// this special case effectively consumes the 2 last curves at once
                            //fflastFracInner = 1;
                        //ff}
                    //ff}
                }

                innerCurve = !revTran ? onCurveIn : nCurveIn-1-onCurveIn;
                pCurves = ppCurvesInnerList[innerCurve];

                // SEND NEXT INNER TRANSITION CURVE
                HOS_NOTE("Send Inner Transition");
                pPush = pDevice->StartPush(CountLoopPushes(info));
                Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_INNER_TRANSITION_CURVE);
                pPush += 2;
                for (index = 0; index < info->maxAttr; index++) {
                    if (info->evalEnables & (1 << index)) {
                        //if this is a not a new swath... do we pre adjust that row??? or just not send the last row in the previous patch?
                        //fffor (i = 0; i < (*ppCurvesInnerList[!revTran ? 0 : nCurveIn-1])[index]->order; i++) { // }
                        for (i = 0; i < (*pCurves)[index]->order; i++) {
                            HOS_NOTE("Set Curve Coefficients");
                            PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
                            *(pPush + 1) = VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][0]);
                            *(pPush + 2) = VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][1]);
                            *(pPush + 3) = VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][2]);
                            *(pPush + 4) = VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][3]);
                            pPush += 5;
                        }
                    }
                }
                Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
                pDevice->EndPush(pPush + 2);
                HOS_FLUSH();

                onCurveIn++;
            }

            if (doFrac) {
                if (totalToDoOut == 1) {
                    dist = 0xDAD;       // complete stitch with fan of inside pts
                }
            }
        }

        if (totalToDoOut > 1) {
            dist += 2*inSegs;
            out++;
            oneToDoOut--;
            totalToDoOut--;

            if (oneToDoOut == 0  &&  totalToDoOut > 1) {
                // need another outside curve
                if (totalToDoOut > 1+info->maxSwatch) {
                    oneToDoOut = info->maxSwatch;   // doesn't fit in one curve
                    lastOutsideCurve = 0;
                } else {
                    oneToDoOut = totalToDoOut;
                    lastOutsideCurve = 1;
                }

                pCurves = ppCurvesOuter[onCurveOut];

                // SEND THE NEXT OUTER TRANSITION CURVE
                HOS_NOTE("Send Outer Transition");
                pPush = pDevice->StartPush(CountLoopPushes(info));
                Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_OUTER_TRANSITION_CURVE);
                pPush += 2;
                for (index = 0; index < info->maxAttr; index++) {
                    if (info->evalEnables & (1 << index)) {
                        // if this is a not a new swath... do we pre adjust that row???
                        // or just not send the last row in the previous patch?
                        for (i = 0; i < (*pCurves)[index]->order; i++) {
                            HOS_NOTE("Set Curve Coefficients");
                            PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
                            *(pPush + 1) = VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][0]);
                            *(pPush + 2) = VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][1]);
                            *(pPush + 3) = VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][2]);
                            *(pPush + 4) = VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][3]);
                            pPush += 5;
                        }
                    }
                }
                Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
                pDevice->EndPush(pPush + 2);
                HOS_FLUSH();

                onCurveOut++;
            }

            //ffif (doFrac) {
                if (totalToDoIn == 1) {
                    dist = -0xDAD;      // complete stitch with fan of outside pts
                }
            //ff}
        }
    }
    nvAssert(totalToDoOut == 1);
    nvAssert(totalToDoIn == 1);
    nvAssert(onCurveOut == nCurveOut);
    //ffnvAssert((onCurveIn + lastFracInner) == nCurveIn);
    nvAssert(onCurveIn == nCurveIn);

    HOS_NOTE("END Transition");
    pPush = pDevice->StartPush();
    Push1(pPush, NV097_SET_END_TRANSITION, 0);
    pDevice->EndPush(pPush + 2);
    HOS_FLUSH();
}


/*****************************************************************************/
unsigned int DrawIntPatch(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, FDMatrix **matrices);
unsigned int DrawIntPatchGrid(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, FDMatrix **matrices);

unsigned int DrawFracPatch(NV_PATCH_INFO *info,
                           int botOutSegs, int botInSegs,
                           int leftOutSegs, int leftInSegs,
                           int reverse, FDMatrix **matrices)
{
    int index;
    unsigned int retVal;
    NV_PATCH_QUAD_INFO *quadInfo = info->quadInfo;

    // layer on top of DrawIntPatch() cases
    quadInfo->nu0 = botOutSegs;
    quadInfo->nu1 = botInSegs;
    quadInfo->nv0 = leftOutSegs;
    quadInfo->nv1 = leftInSegs;

    quadInfo->uMaxSegs = botInSegs;
    quadInfo->vMaxSegs = leftInSegs;
    quadInfo->uMinSegs = botOutSegs;
    quadInfo->vMinSegs = leftOutSegs;
    quadInfo->needUInner = 1;
    quadInfo->needVInner = 1;

    quadInfo->stitchLeft = 1;
    quadInfo->stitchRight = 0;
    quadInfo->stitchTop = 0;
    quadInfo->stitchBottom = 1;

    quadInfo->stitchUBegin = 1;
    quadInfo->stitchUEnd = 0;
    quadInfo->stitchVBegin = 1;
    quadInfo->stitchVEnd = 0;

    quadInfo->u0Dir = 1;
    quadInfo->v0Dir = 1;
    quadInfo->u1Dir = 1;
    quadInfo->v1Dir = 1;
    quadInfo->uMaxDir = 1;
    quadInfo->vMaxDir = 1;

    quadInfo->du0 = 1.f / quadInfo->nu0;
    quadInfo->du1 = 1.f / quadInfo->nu1;
    quadInfo->dv0 = 1.f / quadInfo->nv0;
    quadInfo->dv1 = 1.f / quadInfo->nv1;
    quadInfo->duMax = 1.f / quadInfo->uMaxSegs;
    quadInfo->dvMax = 1.f / quadInfo->vMaxSegs;

    for (index = 0; index < info->maxAttr; index++) {
        if (!(info->evalEnables & (1<<index))) {
            continue;
        }
// XXX maybe do this in lower routine ?
CopyMatrix(info, &info->tempMatrix, matrices[index]);
if (info->swatchFlags & NV_PATCH_SWATCH) {
    RetessellateFDMatrix(info, 1.f, quadInfo->dvMax * info->maxSwatch, 1.f, quadInfo->dvMax, &info->tempMatrix);    // big to small
}
        OffsetFDMatrix_Extract_Discard(info, quadInfo->pUInner[index], 0, &info->tempMatrix, 0, 1);

// XXX maybe do this in lower routine ?
CopyMatrix(info, &info->tempMatrix, matrices[index]);
if (info->swatchFlags & NV_PATCH_SWATCH) {
    RetessellateFDMatrix(info, quadInfo->duMax * info->maxSwatch, 1.f, quadInfo->duMax, 1.f, &info->tempMatrix);    // big to small
}
        OffsetFDMatrix_Extract_Discard(info, quadInfo->pVInner[index], 1, &info->tempMatrix, 1, 0);
    }

    //             *-*-*    the set frontface call will adjust for this
    // winding on  |X|O|
    // subpatches  *-*-*
    // X=CW        |O|X|
    // O=CCW       *-*-*

    info->reverse ^= reverse;
    retVal = DrawIntPatch(info, quadInfo, matrices);
    info->reverse ^= reverse;

    return retVal;
}

/*****************************************************************************/
/*****************************************************************************/
unsigned int
FillIntCornerLL(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, NV_PATCH_QUAD_INFO *quadInfoOrig,
            int index, float *pPoint)
{
    NV_PATCH_CURVE_INFO *tempCurve = &info->tempCurve;

    if (quadInfo->stitchVBegin) {
        nvAssert(!quadInfo->stitchVEnd);
        if (quadInfo->vMaxSegs > 1) {
            CopyCurve(info, tempCurve, (*quadInfo->pSwatchVBegin)[index]);
            OffsetCurve(info, tempCurve, 1);
            CopyPoint(pPoint, tempCurve->coeffs[0]);
            return 0;
        }
        if (quadInfo->stitchUBegin) {
            // must step along ending patch edge
            CopyCurve(info, tempCurve, (*quadInfo->pSwatchUEnd)[index]);
            OffsetCurve(info, tempCurve, 1);
            CopyPoint(pPoint, tempCurve->coeffs[0]);
            return 0;
        }
        CopyPoint(pPoint, quadInfoOrig->pCorners[index][  quadInfo->vMaxDir][1-quadInfo->uMaxDir]);
        return 0;

    } else if (quadInfo->stitchUBegin) {
        nvAssert(!quadInfo->stitchUEnd);
        // must step along edge, even though (*quadInfo->pSwatchVBegin)[index]->coeffs[0] is about right
        CopyCurve(info, tempCurve, (*quadInfo->pSwatchUBegin)[index]);
        OffsetCurve(info, tempCurve, 1);
        CopyPoint(pPoint, tempCurve->coeffs[0]);
        // Since the outside edge takes priority, we tweak start of pSwatchVBegin to be on the outside edge
        CopyPoint((*quadInfo->pSwatchVBegin)[index]->coeffs[0], pPoint);

    } else {
        CopyPoint(pPoint, quadInfoOrig->pCorners[index][1-quadInfo->vMaxDir][1-quadInfo->uMaxDir]);
    }

    return 0;
}

/*****************************************************************************/
unsigned int
FillIntCornerLR(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, NV_PATCH_QUAD_INFO *quadInfoOrig,
            int index, float *pPoint)
{
    NV_PATCH_CURVE_INFO *tempCurve = &info->tempCurve;

    if (quadInfo->stitchVBegin) {
        nvAssert(!quadInfo->stitchVEnd);
        if (quadInfo->vMaxSegs > 1) {
            CopyCurve(info, tempCurve, (*quadInfo->pSwatchVEnd)[index]);
            OffsetCurve(info, tempCurve, 1);
            CopyPoint(pPoint, tempCurve->coeffs[0]);
            return 0;
        }
        if (quadInfo->stitchUEnd) {
            CopyCurve(info, tempCurve, (*quadInfo->pSwatchUEnd)[index]);
            OffsetCurve(info, tempCurve, quadInfo->uMaxSegs - 1);
            CopyPoint(pPoint, tempCurve->coeffs[0]);
            return 0;
        }
        CopyPoint(pPoint, quadInfoOrig->pCorners[index][  quadInfo->vMaxDir][quadInfo->uMaxDir]);
        return 0;

    } else if (quadInfo->stitchUEnd) {
        nvAssert(!quadInfo->stitchUBegin);
        CopyPoint(pPoint, (*quadInfo->pSwatchVEnd)[index]->coeffs[0]);

    } else {
        CopyPoint(pPoint, quadInfoOrig->pCorners[index][1-quadInfo->vMaxDir][quadInfo->uMaxDir]);
    }

    return 0;
}

/*****************************************************************************/
unsigned int
FillIntCornerUL(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, NV_PATCH_QUAD_INFO *quadInfoOrig,
            int index, float *pPoint)
{
    NV_PATCH_CURVE_INFO *tempCurve = &info->tempCurve;

    if (quadInfo->stitchVEnd) {
        nvAssert(!quadInfo->stitchVBegin);
        if (quadInfo->vMaxSegs > 1) {
            CopyCurve(info, tempCurve, (*quadInfo->pSwatchVBegin)[index]);
            OffsetCurve(info, tempCurve, quadInfo->vMaxSegs - 1);
            CopyPoint(pPoint, tempCurve->coeffs[0]);
            return 0;
        }
        if (quadInfo->stitchUBegin) {
            CopyCurve(info, tempCurve, (*quadInfo->pSwatchUBegin)[index]);
            OffsetCurve(info, tempCurve, 1);
            CopyPoint(pPoint, tempCurve->coeffs[0]);
            return 0;
        }
        CopyPoint(pPoint, quadInfoOrig->pCorners[index][1-quadInfo->vMaxDir][1-quadInfo->uMaxDir]);
        return 0;

    } else if (quadInfo->stitchUBegin) {
        nvAssert(!quadInfo->stitchUEnd);
        CopyCurve(info, tempCurve, (*quadInfo->pSwatchUEnd)[index]);
        OffsetCurve(info, tempCurve, 1);
        CopyPoint(pPoint, tempCurve->coeffs[0]);

    } else {
        CopyPoint(pPoint, quadInfoOrig->pCorners[index][quadInfo->vMaxDir][1-quadInfo->uMaxDir]);
    }
    return 0;
}

/*****************************************************************************/
unsigned int
FillIntCornerUR(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, NV_PATCH_QUAD_INFO *quadInfoOrig,
            int index, float *pPoint)
{
    NV_PATCH_CURVE_INFO *tempCurve = &info->tempCurve;
    int doFrac = !!(info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL);
    int adjust = doFrac && (info->swatchFlags & NV_PATCH_SWATCH);

    if (quadInfo->stitchVEnd  ||  doFrac) {
        nvAssert(!quadInfo->stitchVBegin  ||  doFrac);
        if ((quadInfo->vMaxSegs > 1)  ||  doFrac) {
            CopyCurve(info, tempCurve, (*quadInfo->pSwatchVEnd)[index]);
            OffsetCurve(info, tempCurve, quadInfo->vMaxSegs - 1 + adjust);
            CopyPoint(pPoint, tempCurve->coeffs[0]);
            return 0;
        }
        if (quadInfo->stitchUEnd  ||  doFrac) {
            CopyCurve(info, tempCurve, (*quadInfo->pSwatchUBegin)[index]);
            OffsetCurve(info, tempCurve, quadInfo->uMaxSegs - 1 + adjust);
            CopyPoint(pPoint, tempCurve->coeffs[0]);
            return 0;
        }
        nvAssert(!(info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL));
        CopyPoint(pPoint, quadInfoOrig->pCorners[index][1-quadInfo->vMaxDir][quadInfo->uMaxDir]);
        return 0;

    } else if (quadInfo->stitchUEnd) {
        nvAssert(!quadInfo->stitchUBegin);
        CopyCurve(info, tempCurve, (*quadInfo->pSwatchUEnd)[index]);
        OffsetCurve(info, tempCurve, quadInfo->uMaxSegs - 1);
        CopyPoint(pPoint, tempCurve->coeffs[0]);

    } else {
        CopyPoint(pPoint, quadInfoOrig->pCorners[index][quadInfo->vMaxDir][quadInfo->uMaxDir]);
    }

    return 0;
}

/*****************************************************************************/
// routines for integer tessellations
// NOTE: tempVBegin and tempVEnd, are scratch storage.
// This means that the guards may be stepping without a copy.
// The guard setup is handled by the caller, which allows the caller
// to make intelligent choices about the setup and avoid double copies.
// However, the matrix doesn't have any options, so it's better to have
// this routine do the copy from 'matrices' to ppMatrixSetSS0
// since an attr loop is required here anyways.
// So, 'matrices' data must not be altered, and ppMatrixSetSS0[*] are scratch.

// For SW emulation, I tried a strategy of having the formation of the regular
// grid generate curves and guards in preparation for the transitions.
// This can work.
// One of the problems is that a swatch UR guard point is required
// by the HW, so it's not possible to simply write the "top" UInner guard
// from the matrix and just use it, there would still need to be
// a UR guard point to match the swatch to the right.
// In SW, we could save the results from EvalStrip() and feed that into
// the starting point for the next UInner transition (for both the
// FIRST_ROW and LAST_ROW), but not for the inner swatches which naturally get
// guard points and curves from the big step matrices and curves obtained
// while doing swaths.
// In SW, it's required to save all the UInner and VInner curves for later
// doing the transitions, so the storage issue is identical, but with HW it's
// not feasible to store arbitrary amounts of data for later use during the
// transitions.
// So, the "top" UEnd is calculated as a series of guard curves, and the
// guard UR comes from the next curve to the right of the current swatch.
// This could be optimized for SW emulation, but is not done in order to have
// SW emulation more closely match and test requirements for HW drawing.
// Similar reasoning applies to the swatch LR corner guard.
// Also, the VBegin and UBegin inner curves could be totally obtained from
// the matrix at appropriate times, feeding UL and LR to the next
// swatch curve along the outer boundaries of the regular grid.
// Finally, LL could always be obtained from the starting point of the matrix,
// or after one step to the regular grid (as needed for EvalStrip)

unsigned int
DrawIntPatchGrid(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, FDMatrix **matrices)
{
    int ix, iy, degen, ylimit, ymax, index, useVBegin, useVEnd;
    NV_PATCH_CURVE_INFO **tempVBegin, **tempVEnd, *tempCurve;
    FDMatrix **ppMatrixSetSS0;
    FDCurveAttrSet *pCurvesTop, *pCurvesBot, *pCurvesTemp;
    NV_PATCH_EVAL_OUTPUT *pOut1, *pOut2, *pTemp;
    int fixUL = 0, fixUR = 0;
    int fixLL = 0, fixLR = 0;
    int iEvalRight = quadInfo->uMaxSegs - quadInfo->stitchUEnd;
    unsigned int retVal = NV_PATCH_EVAL_OK;
    int doFrac = !!(info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL);
    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

    tempVBegin = info->tempVBegin;
    tempVEnd = info->tempVEnd;
    tempCurve = &info->tempCurve;
    ppMatrixSetSS0 = info->ppMatrixSetSS0;
    pCurvesTop = quadInfo->pCurvesTop;
    pCurvesBot = quadInfo->pCurvesBot;
    pOut1 = quadInfo->pOut1;
    pOut2 = quadInfo->pOut2;

    // tag:iii
    // setup the swatch
    degen = 0;//(quadInfo->vMaxSegs == 1); // && (quadInfo->stitchVBegin || quadInfo->stitchVEnd);
    ymax = quadInfo->vMaxSegs - quadInfo->stitchVEnd - quadInfo->stitchVBegin;// + degen;

    // check for degenerate regular grid.
    if (!doFrac) {
        if ((ymax <= 0)  ||  ((iEvalRight - quadInfo->stitchUBegin) <= 0)) {
            return retVal;
        }
    } else {
        // this test is normal against <= 2 but we've already pre-subtracted
        // 1 from each before calling here assuming that the regular grid
        // has 1 less segment due to the additional 2 transitions in frac.
        if ((ymax <= 0)  ||  ((iEvalRight - quadInfo->stitchUBegin) <= 0)) {
            return retVal;
        }
    }

    // Note: the 'use' vars are for non-std-guard attributes
    useVBegin = 0;
    useVEnd = doFrac && (info->swatchFlags & NV_PATCH_SWATCH_LAST_COL);

    for (index = 0; index < info->maxAttr; index++) {
        if (info->evalEnables & (1 << index)) {

            // copy and step the forward differencing matrix to the first row of regular grid (as for HW)
            CopyMatrix(info, ppMatrixSetSS0[index], matrices[index]);

            OffsetFDMatrix(info, ppMatrixSetSS0[index], 0, quadInfo->stitchVBegin);
        }
    }

    //BEGIN_END_SWATCH/SWATH?
    unsigned long dwFormat;
    dwFormat = NV097_SET_BEGIN_END_SWATCH_SWATCH_CMD_BEGIN;
    dwFormat |= (((info->swatchFlags  & NV_PATCH_SWATCH_FIRST_ROW) ? NV097_SET_BEGIN_END_SWATCH_NEW_SWATH_NEW : NV097_SET_BEGIN_END_SWATCH_NEW_SWATH_CONTINUE)   << 4);
    dwFormat |= ((quadInfo->stitchVBegin ? NV097_SET_BEGIN_END_SWATCH_SKIP_FIRST_ROW_TRUE : NV097_SET_BEGIN_END_SWATCH_SKIP_FIRST_ROW_FALSE) << 8);
    dwFormat |= ((quadInfo->stitchUBegin ? NV097_SET_BEGIN_END_SWATCH_SKIP_FIRST_COL_TRUE : NV097_SET_BEGIN_END_SWATCH_SKIP_FIRST_COL_FALSE) << 12);
    dwFormat |= (((quadInfo->vMaxSegs - quadInfo->stitchVEnd < info->maxSwatch) ?  NV097_SET_BEGIN_END_SWATCH_SHORT_SWATCH_PARTIAL_HEIGHT : NV097_SET_BEGIN_END_SWATCH_SHORT_SWATCH_FULL_HEIGHT) << 16);
    dwFormat |= (((quadInfo->uMaxSegs - quadInfo->stitchUEnd < info->maxSwatch) ?  NV097_SET_BEGIN_END_SWATCH_NARROW_SWATCH_PARTIAL_WIDTH : NV097_SET_BEGIN_END_SWATCH_NARROW_SWATCH_FULL_WIDTH) << 20);

    HOS_NOTE("Begin Swatch");
    pPush = pDevice->StartPush();
    Push1(pPush, NV097_SET_BEGIN_END_SWATCH, dwFormat);
    pDevice->EndPush(pPush + 2);

    //send guard curves!!!!
    SendGuardCurve(NV097_SET_BEGIN_END_CURVE_CMD_LEFT_GUARD_CURVE,  info, quadInfo, info->swatchFlags );
    SendGuardCurve(NV097_SET_BEGIN_END_CURVE_CMD_RIGHT_GUARD_CURVE, info, quadInfo, info->swatchFlags );

    // prep INT patch
    if (info->swatchFlags & NV_PATCH_SWATCH_FIRST_ROW) {
        for (index = 0; index < info->maxAttr; index++) {
            if (info->evalEnables & (1 << index)) {
                if (quadInfo->stitchVBegin  &&  quadInfo->needUInner) {
                    // cache off first curve for use in transitions later
                    ExtractUCurve(info, (*quadInfo->pSwatchUBegin)[index], ppMatrixSetSS0[index]);
                }
            }
        }

        int i;
        HOS_NOTE("Begin Curve");
        pPush = pDevice->StartPush(CountLoopPushes(info));
        Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_STRIP_CURVE);
        pPush += 2;
        for (index = 0; index < info->maxAttr; index++) {
            if (info->evalEnables & (1 << index)) {
                //if this is a not a new swath... do we pre adjust that row??? or just not send the last row in the previous patch?
                for (i = 0; i < ppMatrixSetSS0[index]->columns; i++) {
                    HOS_NOTE("Set Curve Coefficients");
                    PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
                    *(pPush + 1) = VIEW_AS_DWORD(ppMatrixSetSS0[index]->data[0][i][0]);
                    *(pPush + 2) = VIEW_AS_DWORD(ppMatrixSetSS0[index]->data[0][i][1]);
                    *(pPush + 3) = VIEW_AS_DWORD(ppMatrixSetSS0[index]->data[0][i][2]);
                    *(pPush + 4) = VIEW_AS_DWORD(ppMatrixSetSS0[index]->data[0][i][3]);
                    pPush += 5;
                }
            }
        }
        Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
        pDevice->EndPush(pPush + 2);
    }

    if (doFrac) {
        // for FRAC patches, check after writing UBegin
        if ((ymax <= 0)  ||  ((iEvalRight - quadInfo->stitchUBegin) <= 0)) {
            return retVal;
        }
    }

    // tag:rrr
    // loop thru regular grid, plus 1 for top stitch to init info for "bot" of top stitch
    ylimit = ymax - 1;
    for (iy = 0; iy < ymax; iy++) {
        for (index = 0; index < info->maxAttr; index++) {
            if (info->evalEnables & (1 << index)) {
                OffsetFDMatrix(info, ppMatrixSetSS0[index], 0, 1);
                if (EV_GUARD_ATTR(index)  ||  useVBegin) {
                    OffsetCurve(info, tempVBegin[index], 1);
                }
                if (EV_GUARD_ATTR(index)  ||  useVEnd) {
                    OffsetCurve(info, tempVEnd[index], 1);
                }

                if ((iy == ylimit) && (info->swatchFlags & NV_PATCH_SWATCH_LAST_ROW)) {
                    if (doFrac  ||  (quadInfo->stitchVEnd  &&  quadInfo->needUInner)) {
                        // save UInner curves for doing transition later
                        if (EV_GUARD_ATTR(index) && !quadInfo->stitchUBegin  &&  !doFrac) {
                            // snap to pre-calculated guard points
                            if (index == ATTR_V) {
                                CopyPoint(ppMatrixSetSS0[index]->data[0][0], info->pSwatchCorner[ATTR_V][1][0]);
                            } else {
                                CopyPoint(ppMatrixSetSS0[index]->data[0][0], info->pSwatchCorner[ATTR_N][1][0]);
                            }
                        }
                        ExtractUCurve(info, (*quadInfo->pSwatchUEnd)[index], ppMatrixSetSS0[index]);

                    }
                }
            }
        }

        // avoid strip when degenerate regular grid (have only bottom stitch)
        // and avoid degenerate strip at top of regular grid
        if (!degen) {

            int i;
            HOS_NOTE("Begin Curve");
            pPush = pDevice->StartPush(CountLoopPushes(info));
            Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_STRIP_CURVE);
            pPush += 2;
            for (index = 0; index < info->maxAttr; index++) {
                if (info->evalEnables & (1 << index)) {
                    //if this is a not a new swath... do we pre adjust that row??? or just not send the last row in the previous patch?
                    for (i = 0; i < ppMatrixSetSS0[index]->columns; i++) {
                        HOS_NOTE("Set Curve Coefficients");
                        PushCount(pPush, NV097_SET_CURVE_COEFFICIENTS(0), 4);
                        *(pPush + 1) = VIEW_AS_DWORD(ppMatrixSetSS0[index]->data[0][i][0]);
                        *(pPush + 2) = VIEW_AS_DWORD(ppMatrixSetSS0[index]->data[0][i][1]);
                        *(pPush + 3) = VIEW_AS_DWORD(ppMatrixSetSS0[index]->data[0][i][2]);
                        *(pPush + 4) = VIEW_AS_DWORD(ppMatrixSetSS0[index]->data[0][i][3]);
                        pPush += 5;
                    }
                }
            }
            Push1(pPush, NV097_SET_BEGIN_END_CURVE, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
            pDevice->EndPush(pPush + 2);
        }
        pTemp = pOut1;
        pOut1 = pOut2;
        pOut2 = pTemp;
        pCurvesTemp = pCurvesTop;
        pCurvesTop = pCurvesBot;
        pCurvesBot = pCurvesTemp;
    }

    HOS_NOTE("End Swatch");
    pPush = pDevice->StartPush();
    Push1(pPush, NV097_SET_BEGIN_END_SWATCH, NV097_SET_BEGIN_END_SWATCH_SWATCH_CMD_END);
    pDevice->EndPush(pPush + 2);

    quadInfo->pCurvesTop = pCurvesTop;
    quadInfo->pCurvesBot = pCurvesBot;
    quadInfo->pOut1 = pOut1;
    quadInfo->pOut2 = pOut2;

    return retVal;
}


void DrawAllPatchTransitions(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo,
            FDCurveAttrSet **ppSetU0, FDCurveAttrSet **ppSetU1, FDCurveAttrSet **ppSetUInner, FDCurveAttrSet **ppSetUInnerFrac,
            FDCurveAttrSet **ppSetV0, FDCurveAttrSet **ppSetV1, FDCurveAttrSet **ppSetVInner, FDCurveAttrSet **ppSetVInnerFrac)
{
    FDCurveAttrSet **ppCurvesOut, **ppCurvesInner;

    if (info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL) {
        int index;

        for (index = 0; index < info->maxAttr; index++) {
            if (!(info->evalEnables & (1 << index))) {
                continue;
            }

            //These are INNER TRANSITION ENDPOINTS -- we are storing them in the grid corners
            //slot because they are free and the original data there won't be needed anymore
            //ffCopyCurve(info, &info->tempCurve, quadInfo->pU1[index]);
            CopyCurve(info, &info->tempCurve, (*ppSetU1[0])[index]);
            OffsetCurve(info, &info->tempCurve, 1);
            CopyPoint(info->gridCorner[index][1][0], (float*)&info->tempCurve.coeffs[0]);

            //ffCopyCurve(info, &info->tempCurve, quadInfo->pV1[index]);
            CopyCurve(info, &info->tempCurve, (*ppSetV1[0])[index]);
            OffsetCurve(info, &info->tempCurve, 1);
            CopyPoint(info->gridCorner[index][0][1], (float*)&info->tempCurve.coeffs[0]);
        }

        // do initial stitches for FRAC : RIGHT/TOP/LEFT
        nvAssert(quadInfo->v1Dir == quadInfo->vMaxDir);
        nvAssert(quadInfo->u1Dir == quadInfo->uMaxDir);

        // TOP/RIGHT/BOTTOM/LEFT
        // DCR HW must      *---*-----------*
        // render FRAC      |   |    0     /|
        // stitches in      |   *---------* |
        // order TOP /      |   |         | |
        // RIGHT / BOTTOM   | 3 |         |1|
        // LEFT             |   |         | |
        //                  |  _*---------*-*
        //                  |_/      2      |
        //                  *---------------*
        if (quadInfo->nu1 < quadInfo->nv1) {
ASSERT(0);
            // this should not happen because we call MaybeTransposeFlipQuad() to
            // make nu1 >= nv1
            // Mostly this is an optimization, but it may be that
            // HW requires a transpose of the transitions when we are in a degenerate grid case.
        }

        {
            // TOP
            {
                if (quadInfo->uMaxSegs > 1 && quadInfo->vMaxSegs > 1) {
                    ppCurvesOut = ppSetU1;
                    ppCurvesInner = ppSetUInnerFrac;
                    DrawIntStitch(1, info, quadInfo, ppCurvesOut, ppCurvesInner,
                        quadInfo->uMaxSegs, quadInfo->uMaxSegs,
                        quadInfo->pOut1, quadInfo->pOut2,
                        1, quadInfo->uMaxDir,  // for forcing regular grid corners
                        1, quadInfo->u1Dir,    // for guard corners[][]
                        0, 1,                  // stitchLowOut, stitchHighOut,
                        0, 1,                  // stitchLowIn, stitchHighIn,
                        quadInfo->u1Dir, quadInfo->u0Dir, quadInfo->uMaxDir,
                        quadInfo->u1Dir, 1 ^ quadInfo->u1Dir, LAST_ROW);
                }
            }

            // RIGHT
            {
                if (quadInfo->uMaxSegs > 1 && quadInfo->vMaxSegs > 1) {
                    ppCurvesOut = ppSetV1;
                    ppCurvesInner = ppSetVInnerFrac;
                    //don't render a bottom transition if degenerate regular grid

                    DrawIntStitch(0, info, quadInfo, ppCurvesOut, ppCurvesInner, quadInfo->vMaxSegs, quadInfo->vMaxSegs,
                        quadInfo->pOut1, quadInfo->pOut2,
                        quadInfo->vMaxDir, 1,   // for forcing regular grid corners
                        quadInfo->v1Dir, 1,     // for guard corners[][]
                        0, 1,                   // stitchLowOut, stitchHighOut,
                        0, 1,                   // stitchLowIn, stitchHighIn,
                        quadInfo->v1Dir, quadInfo->v0Dir, quadInfo->vMaxDir,
                        1 ^ quadInfo->v1Dir, 1 ^ quadInfo->v1Dir, LAST_COL);
                }
            }

            // BOT
            if (quadInfo->stitchBottom) {
                ppCurvesOut = ppSetU0;
                if (quadInfo->vMaxSegs != 2) {
                    ppCurvesInner = ppSetUInner;
                } else {
                    ppCurvesInner = ppSetUInnerFrac;
                }

                DrawIntStitch(1, info, quadInfo, ppCurvesOut, ppCurvesInner, quadInfo->uMinSegs, quadInfo->uMaxSegs,
                    quadInfo->pOut1, quadInfo->pOut2,
                    0, quadInfo->uMaxDir,   // for forcing regular grid corners
                    0, quadInfo->u0Dir,     // for guard corners[][]
                    quadInfo->u0Dir == quadInfo->uMaxDir ? quadInfo->stitchUBegin : quadInfo->stitchUEnd,   // stitchLowOut
                    quadInfo->u0Dir == quadInfo->uMaxDir ? quadInfo->stitchUEnd : quadInfo->stitchUBegin,   // stitchHighOut
                    quadInfo->stitchUBegin, quadInfo->stitchUEnd,   // stitchLowIn, stitchHighIn,
                    quadInfo->u0Dir, quadInfo->u1Dir, quadInfo->uMaxDir,
                    1 ^ quadInfo->u0Dir, quadInfo->u0Dir, FIRST_ROW);
            }

            // LEFT
            //ffif (quadInfo->stitchLeft && ((quadInfo->vMaxSegs > 1) || (quadInfo->uMaxSegs == 1)))
            if (quadInfo->stitchLeft)
            {
                ppCurvesOut = ppSetV0;
                ppCurvesInner = ppSetVInner;

                DrawIntStitch(0, info, quadInfo, ppCurvesOut, ppCurvesInner, quadInfo->vMinSegs, quadInfo->vMaxSegs,
                    quadInfo->pOut1, quadInfo->pOut2,
                    quadInfo->vMaxDir, 0,   // for forcing regular grid corners
                    quadInfo->v0Dir, 0,     // for guard corners[][]
                    quadInfo->v0Dir == quadInfo->vMaxDir ? quadInfo->stitchVBegin : quadInfo->stitchVEnd,   // stitchLowOut
                    quadInfo->v0Dir == quadInfo->vMaxDir ? quadInfo->stitchVEnd : quadInfo->stitchVBegin,   // stitchHighOut
                    quadInfo->stitchVBegin, quadInfo->stitchVEnd,   // stitchLowIn, stitchHighIn,
                    quadInfo->v0Dir, quadInfo->v1Dir, quadInfo->vMaxDir,
                    quadInfo->v0Dir, quadInfo->v0Dir, FIRST_COL );
            }
        }

        return;
    }

    // HW INT Transitions must be rendered in order TOP/RIGHT/BOTTOM/LEFT
    if (quadInfo->stitchVEnd) {
        if (quadInfo->stitchTop) {
            ppCurvesOut = ppSetU1;
            ppCurvesInner = ppSetUInner;
            DrawIntStitch(1, info, quadInfo, ppCurvesOut, ppCurvesInner, quadInfo->uMinSegs, quadInfo->uMaxSegs,
                quadInfo->pOut1, quadInfo->pOut2,
                1, quadInfo->uMaxDir,   // for forcing regular grid corners
                1, quadInfo->u1Dir,     // for guard corners[][]
                quadInfo->u1Dir == quadInfo->uMaxDir ? quadInfo->stitchUBegin : quadInfo->stitchUEnd,   // stitchLowOut
                quadInfo->u1Dir == quadInfo->uMaxDir ? quadInfo->stitchUEnd : quadInfo->stitchUBegin,   // stitchHighOut
                quadInfo->stitchUBegin, quadInfo->stitchUEnd,   // stitchLowIn, stitchHighIn,
                quadInfo->u1Dir, quadInfo->u0Dir, quadInfo->uMaxDir,
                quadInfo->u1Dir, 1 ^ quadInfo->u1Dir, LAST_ROW);
        } else {
            ppCurvesOut = ppSetU0;
            ppCurvesInner = ppSetUInner;
            DrawIntStitch(1, info, quadInfo, ppCurvesOut, ppCurvesInner, quadInfo->uMinSegs, quadInfo->uMaxSegs,
                quadInfo->pOut1, quadInfo->pOut2,
                0, quadInfo->uMaxDir,   // for forcing regular grid corners
                0, quadInfo->u0Dir,     // for guard corners[][]
                quadInfo->u0Dir == quadInfo->uMaxDir ? quadInfo->stitchUBegin : quadInfo->stitchUEnd,   // stitchLowOut
                quadInfo->u0Dir == quadInfo->uMaxDir ? quadInfo->stitchUEnd : quadInfo->stitchUBegin,   // stitchHighOut
                quadInfo->stitchUBegin, quadInfo->stitchUEnd,   // stitchLowIn, stitchHighIn,
                quadInfo->u0Dir, quadInfo->u1Dir, quadInfo->uMaxDir,
                1 ^ quadInfo->u0Dir, quadInfo->u0Dir, LAST_ROW);
        }
    }

    if (quadInfo->stitchUEnd) {
        if (quadInfo->stitchRight) {
            ppCurvesOut = ppSetV1;
            ppCurvesInner = ppSetVInner;
            DrawIntStitch(0, info, quadInfo, ppCurvesOut, ppCurvesInner, quadInfo->vMinSegs, quadInfo->vMaxSegs,
                quadInfo->pOut1, quadInfo->pOut2,
                quadInfo->vMaxDir, 1,   // for forcing regular grid corners
                quadInfo->v1Dir, 1,     // for guard corners[][]
                quadInfo->v1Dir == quadInfo->vMaxDir ? quadInfo->stitchVBegin : quadInfo->stitchVEnd,   // stitchLowOut
                quadInfo->v1Dir == quadInfo->vMaxDir ? quadInfo->stitchVEnd : quadInfo->stitchVBegin,   // stitchHighOut
                quadInfo->stitchVBegin, quadInfo->stitchVEnd,   // stitchLowIn, stitchHighIn,
                quadInfo->v1Dir, quadInfo->v0Dir, quadInfo->vMaxDir,
                1 ^ quadInfo->v1Dir, 1 ^ quadInfo->v1Dir, LAST_COL);
        } else {
            ppCurvesOut = ppSetV0;
            ppCurvesInner = ppSetVInner;
            DrawIntStitch(0, info, quadInfo, ppCurvesOut, ppCurvesInner, quadInfo->vMinSegs, quadInfo->vMaxSegs,
                quadInfo->pOut1, quadInfo->pOut2,
                quadInfo->vMaxDir, 0,   // for forcing regular grid corners
                quadInfo->v0Dir, 0,     // for guard corners[][]
                quadInfo->v0Dir == quadInfo->vMaxDir ? quadInfo->stitchVBegin : quadInfo->stitchVEnd,   // stitchLowOut
                quadInfo->v0Dir == quadInfo->vMaxDir ? quadInfo->stitchVEnd : quadInfo->stitchVBegin,   // stitchHighOut
                quadInfo->stitchVBegin, quadInfo->stitchVEnd,   // stitchLowIn, stitchHighIn,
                quadInfo->v0Dir, quadInfo->v1Dir, quadInfo->vMaxDir,
                quadInfo->v0Dir, quadInfo->v0Dir, LAST_COL);
        }
    }

    if (quadInfo->stitchVBegin) {
        if (quadInfo->stitchBottom) {
            ppCurvesOut = ppSetU0;
            ppCurvesInner = ppSetUInner;
            DrawIntStitch(1, info, quadInfo, ppCurvesOut, ppCurvesInner, quadInfo->uMinSegs, quadInfo->uMaxSegs,
                quadInfo->pOut1, quadInfo->pOut2,
                0, quadInfo->uMaxDir,   // for forcing regular grid corners
                0, quadInfo->u0Dir,     // for guard corners[][]
                quadInfo->u0Dir == quadInfo->uMaxDir ? quadInfo->stitchUBegin : quadInfo->stitchUEnd,   // stitchLowOut
                quadInfo->u0Dir == quadInfo->uMaxDir ? quadInfo->stitchUEnd : quadInfo->stitchUBegin,   // stitchHighOut
                quadInfo->stitchUBegin, quadInfo->stitchUEnd,   // stitchLowIn, stitchHighIn,
                quadInfo->u0Dir, quadInfo->u1Dir, quadInfo->uMaxDir,
                1 ^ quadInfo->u0Dir, quadInfo->u0Dir, FIRST_ROW);
        } else {
            ppCurvesOut = ppSetU1;
            ppCurvesInner = ppSetUInner;
            DrawIntStitch(1, info, quadInfo, ppCurvesOut, ppCurvesInner, quadInfo->uMinSegs, quadInfo->uMaxSegs,
                quadInfo->pOut1, quadInfo->pOut2,
                1, quadInfo->uMaxDir,   // for forcing regular grid corners
                1, quadInfo->u1Dir,     // for guard corners[][]
                quadInfo->u1Dir == quadInfo->uMaxDir ? quadInfo->stitchUBegin : quadInfo->stitchUEnd,   // stitchLowOut
                quadInfo->u1Dir == quadInfo->uMaxDir ? quadInfo->stitchUEnd : quadInfo->stitchUBegin,   // stitchHighOut
                quadInfo->stitchUBegin, quadInfo->stitchUEnd,   // stitchLowIn, stitchHighIn,
                quadInfo->u1Dir, quadInfo->u0Dir, quadInfo->uMaxDir,
                quadInfo->u1Dir, 1 ^ quadInfo->u1Dir, FIRST_ROW);
        }
    }

    if (quadInfo->stitchUBegin) {
        if (quadInfo->stitchLeft) {
            ppCurvesOut = ppSetV0;
            ppCurvesInner = ppSetVInner;
            DrawIntStitch(0, info, quadInfo, ppCurvesOut, ppCurvesInner, quadInfo->vMinSegs, quadInfo->vMaxSegs,
                quadInfo->pOut1, quadInfo->pOut2,
                quadInfo->vMaxDir, 0,   // for forcing regular grid corners
                quadInfo->v0Dir, 0,     // for guard corners[][]
                quadInfo->v0Dir == quadInfo->vMaxDir ? quadInfo->stitchVBegin : quadInfo->stitchVEnd,   // stitchLowOut
                quadInfo->v0Dir == quadInfo->vMaxDir ? quadInfo->stitchVEnd : quadInfo->stitchVBegin,   // stitchHighOut
                quadInfo->stitchVBegin, quadInfo->stitchVEnd,   // stitchLowIn, stitchHighIn,
                quadInfo->v0Dir, quadInfo->v1Dir, quadInfo->vMaxDir,
                quadInfo->v0Dir, quadInfo->v0Dir, FIRST_COL);
        } else {
            ppCurvesOut = ppSetV1;
            ppCurvesInner = ppSetVInner;
            DrawIntStitch(0, info, quadInfo, ppCurvesOut, ppCurvesInner, quadInfo->vMinSegs, quadInfo->vMaxSegs,
                quadInfo->pOut1, quadInfo->pOut2,
                quadInfo->vMaxDir, 1,   // for forcing regular grid corners
                quadInfo->v1Dir, 1,     // for guard corners[][]
                quadInfo->v1Dir == quadInfo->vMaxDir ? quadInfo->stitchVBegin : quadInfo->stitchVEnd,   // stitchLowOut
                quadInfo->v1Dir == quadInfo->vMaxDir ? quadInfo->stitchVEnd : quadInfo->stitchVBegin,   // stitchHighOut
                quadInfo->stitchVBegin, quadInfo->stitchVEnd,   // stitchLowIn, stitchHighIn,
                quadInfo->v1Dir, quadInfo->v0Dir, quadInfo->vMaxDir,
                1 ^ quadInfo->v1Dir, 1 ^ quadInfo->v1Dir, FIRST_COL);
        }
    }
}

/*****************************************************************************/
// For given GuardSet ptr, and attrib, point to the curve for the attribute
inline NV_PATCH_CURVE_INFO *
pGuardSetCurve(NV_PATCH_INFO *info, FDCurveAttrSet *pBase, int attribIndex)
{
    nvAssert(attribIndex < info->maxAttr);
    return (*pBase)[attribIndex];
}

// draw series of sub-patches in swaths.
// General algorithm:
//  1) high level code computes for "normal" step sizes. This is to optimize
//      for smaller cases where swathing is not needed.
//      XXX TODO: have step size be a parameter to high-level setup and
//      XXX prepare directly to required step sizes.
//  2) This routine converts matrices and curves to stepping by 'maxSwatch'
//  3) guard curves and other info is set up stepping around the patch
//      using the larger maxSwatch steps.
//  4) curves and matrices are converted to the smaller step size as needed
//      and passed to DrawIntPatchGrid()
//  5) DrawAllPatchTransitions() is used to draw all transitions
//
// swaths are drawn in col-major order
//
// swatch stepping algorithm:
//          FOR EACH BIG COL
//              horz step BB0 to BB1
//              cvt BB0 to BS0 (in place, BB0 not needed any more)
//              extract BV1 from BB1
//
//              pUL points into BS0 (will be correct when ready for EACH SMALL ROW)
//              pUR points into BV1 (will be correct when ready for EACH SMALL ROW)
//
//              FOR EACH BIG ROW
//                  extract LL from BS0
//                  cvt BS0 to SS0
//                  extract SU0 from SS0
//                  extract SV0 from SS0
//
//                  step BS0 vert (whole matrix, but on last row may just step COL to get UL point)
//                  // note: pUL now points to UL
//
//                  extract LR from BV1
//                  cvt BV1 to SV1
//                  step BV1
//                  // note: pUR now points to UR
//
//                  FOR EACH SMALL ROW
//                      extract SH0 row from SS0 (send to HW, or EvalStrip for SW emulation)
//                      if not last row then step SS0 vert
//

unsigned int DrawIntPatchSwaths(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, FDMatrix **matrices)
{
    NV_PATCH_QUAD_INFO  sQuadInfo = *quadInfo;      // swatch quad info
    NV_PATCH_QUAD_INFO  *quadInfoOrig = quadInfo;
    unsigned int retVal = NV_PATCH_EVAL_OK;
    int doFrac = !!(info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL);
    int onRow, onCol, index;
    int nSwatchU, nSwatchV;
    int stitch_nSwatchU, stitch_nSwatchV;

    // each original guard curve is stepped 'maxSwatch' steps,
    // and intermediate big step curves are stored in a "GuardSet"
    // These get used once while drawing the regular grid,
    // and again when drawing the transition stitches.
    // Each GuardSet is an array of ptrs to info for each big step.
    // pp means ptr to ptr
    FDCurveAttrSet **ppGuardSetU0;  // pp to curves per attribute
    FDCurveAttrSet **ppGuardSetU1;  // [maxAttr][maxOrder][4]
    FDCurveAttrSet **ppGuardSetV0;  // ...
    FDCurveAttrSet **ppGuardSetV1;
    FDCurveAttrSet **ppGuardSetUInner, **ppGuardSetVInner;
    FDCurveAttrSet **ppGuardSetUInnerFrac, **ppGuardSetVInnerFrac;
    FDCurveAttrSet **ppGuardSetUBegin, **ppGuardSetUEnd;    // at begin/end of regular grid
    FDCurveAttrSet **ppGuardSetVBegin, **ppGuardSetVEnd;    // at begin/end of regular grid
    FDCurveAttrSet **ppStitchUInner = NULL, **ppStitchVInner = NULL; // inner curves for stitching
    FDCurveAttrSet **ppStitchUInnerFrac = NULL, **ppStitchVInnerFrac = NULL; // inner curves for stitching FRAC patches

    // for doing one swatch
    FDCurveAttrSet guardSetUBegin, guardSetUEnd;
    FDCurveAttrSet guardSetVBegin, guardSetVEnd;

    // variables for stepping around the regular grid in col-major order
    //      U1-->
    //      UL      UR
    //      +-------+
    //      |       |
    //    ^ |       | ^     <--- one swatch
    //    | |       | |
    //    | |       | |
    //   V0 +-------+ V1
    //      LL      LR
    //      U0-->

    // matrix names have letters B and S to indicate big vs small steps in Vert and Horz directions
    FDMatrixAttrSet *pBB0;          // ptr to FD matrixSet at LL, Big steps Up and Right
    FDMatrixAttrSet *pBB1;          // ptr to FD matrixSet at LR, Big steps Up and Right
    FDMatrixAttrSet *pTempMatrix;   // for ptr swap
    FDMatrixAttrSet matrix0, matrix1;   // actual set of ptrs to matrix set
    FDMatrixAttrSet *pBS0;              // FD matrix at LL, Big steps Up, Small steps Right
    FDMatrixAttrSet SS0;                // ptrs to inner loop FD Matrices, small steps Up and Right

    int needBV0;
    FDCurveAttrSet  BV0;    // curve from LL to UL, for UInner curve list, big steps
    FDCurveAttrSet  SV0;    // curve from LL to UL, for UInner curve list, small steps
    FDCurveAttrSet  BV1;    // curve from LR to UR, for UInner curve list, big steps
    FDCurveAttrSet  SV1;    // curve from LR to UR, for UInner curve list, small steps
    FDCurveAttrSet  *pBV1;  // ptr to current BV1 (maybe really SV1, but only used to get start point)
    FDCurveAttrSet  *pSV1;  // ptr to current SV1

    float LL[4], LR[4];                         // vertex: for extracting from BS0 and BV1
    float *pLL, *pLR, *pUL, *pUR;               // vertex: ptr to swatch corner points
    float LLnor[4], LRnor[4];                   // normal: for extracting from BS0 and BV1
    float *pLLnor, *pLRnor, *pULnor, *pURnor;   // ptr to swatch corner points

    int bytesGuardCurve = EV_ROUND(PATCH_CURVE_HEADER_SIZE + (info->maxOrder * 4) * sizeof(float));   // truncated NV_PATCH_CURVE_INFO
    int bytesGuardCurveAllAttr = info->nAttr * bytesGuardCurve;                 // just the set of curve info

    EV_ALLOC_DECL();
    int nBytes;

    if (doFrac) {
        // The max for nSwatchU/V is not the whole story. The minSegs for the transitions
        // may actually have more steps in the frac case.
        // This is also complicated by the fact that for TRI FRAC, when drawing the 
        // third sub-QUAD-FRAC, the U and V get transposed.
        // So, recalc here according to min/max
        int uMax = MAX(quadInfo->uMinSegs, quadInfo->uMaxSegs);
    
        int vMax = MAX(quadInfo->vMinSegs, quadInfo->vMaxSegs);

        // calc values including end stitch
        stitch_nSwatchU = (uMax - 1) / info->maxSwatch + 1;
        stitch_nSwatchV = (vMax - 1) / info->maxSwatch + 1;
    } else {
        stitch_nSwatchU = info->nSwatchU;
        stitch_nSwatchV = info->nSwatchV;
    }

    // use local quadInfo
    quadInfo = &sQuadInfo;
    quadInfo->needUInner = quadInfoOrig->needUInner;
    quadInfo->needVInner = quadInfoOrig->needVInner;

    // allocating full 16 attr's, 16 order, 1000x1000 steps, 16 maxSwatch, 2 Grid FDMatrix columns --> 9.4 Mbytes
    // 4 order --> 8.3 Mbytes
    // and 2 attr --> 1.0 Mbytes

    nBytes =  EV_ROUND(info->maxAttr * sizeof(NV_PATCH_CURVE_INFO *)); // for ppGuardSet's, for U and V   
    nBytes += bytesGuardCurveAllAttr;                                                   
    nBytes *= (stitch_nSwatchU+1 + stitch_nSwatchV+1);
    nBytes += EV_ROUND((stitch_nSwatchU+1)*sizeof(FDCurveAttrSet *));
    nBytes += EV_ROUND((stitch_nSwatchV+1)*sizeof(FDCurveAttrSet *));
    nBytes *= (doFrac ? 4 : 3);    
    nBytes += 6 * bytesGuardCurveAllAttr;                                                           // guards for current swatch
    nBytes += info->nAttr * sizeof(FDMatrix) * (3);                                                 // matrix0,1 and SS0
    nBytes += info->nAttr * bytesGuardCurve * (4);                                                  // BV0, BV1, SV0, SV1
    
    EV_ALLOC_INIT_WITH_CACHE(info->context, nBytes, drawIntPatchSwathsEnd, info->pCache[EV_CACHE_SWATHING]);

    nBytes = EV_ROUND((stitch_nSwatchU+1) * sizeof(FDCurveAttrSet *));
    EV_ALLOC(ppGuardSetU0,     info->context, nBytes );
    EV_ALLOC(ppGuardSetU1,     info->context, nBytes);
    EV_ALLOC(ppGuardSetUInner, info->context, nBytes);
    if (doFrac) {
        EV_ALLOC(ppGuardSetUInnerFrac, info->context, nBytes);
    }
    
    nBytes = EV_ROUND((stitch_nSwatchV+1) * sizeof(FDCurveAttrSet *));
    EV_ALLOC(ppGuardSetV0,     info->context, nBytes);
    EV_ALLOC(ppGuardSetV1,     info->context, nBytes);
    EV_ALLOC(ppGuardSetVInner, info->context, nBytes);
    if (doFrac) {
        EV_ALLOC(ppGuardSetVInnerFrac, info->context, nBytes);
    }

    for (index = 0; index < info->maxAttr; index++) {
        if (!(info->evalEnables & (1 << index))) {
            continue;
        }
        EV_ALLOC(quadInfo->pU0[index], info->context, bytesGuardCurve);
        EV_ALLOC(quadInfo->pU1[index], info->context, bytesGuardCurve);
        EV_ALLOC(quadInfo->pV0[index], info->context, bytesGuardCurve);
        EV_ALLOC(quadInfo->pV1[index], info->context, bytesGuardCurve);
        EV_ALLOC(quadInfo->pUInner[index], info->context, bytesGuardCurve);
        EV_ALLOC(quadInfo->pVInner[index], info->context, bytesGuardCurve);

        EV_ALLOC(matrix0[index], info->context, sizeof(FDMatrix));
        EV_ALLOC(matrix1[index], info->context, sizeof(FDMatrix));
        EV_ALLOC(SS0[index], info->context, sizeof(FDMatrix));

        EV_ALLOC(BV0[index], info->context, bytesGuardCurve);
        EV_ALLOC(BV1[index], info->context, bytesGuardCurve);
        EV_ALLOC(SV0[index], info->context, bytesGuardCurve);
        EV_ALLOC(SV1[index], info->context, bytesGuardCurve);
    }
    quadInfo->pSwatchVBegin = &guardSetVBegin;
    quadInfo->pSwatchVEnd = &guardSetVEnd;

    // set up sets of guard curves for each big step
    // init pointers to guard sub-curves for each big step
    nBytes = EV_ROUND(info->maxAttr * sizeof(NV_PATCH_CURVE_INFO *));
    for (onCol = 0;  onCol <= stitch_nSwatchU;  onCol++) {
        EV_ALLOC(ppGuardSetU0[onCol], info->context, nBytes);
        EV_ALLOC(ppGuardSetU1[onCol], info->context, nBytes);
        EV_ALLOC(ppGuardSetUInner[onCol], info->context, nBytes);
        if (doFrac) {
            EV_ALLOC(ppGuardSetUInnerFrac[onCol], info->context, nBytes);
        }

        for (index = 0; index < info->maxAttr; index++) {
            if (!(info->evalEnables & (1 << index))) {
                continue;
            }
            EV_ALLOC((*ppGuardSetU0[onCol])[index], info->context, bytesGuardCurve);
            EV_ALLOC((*ppGuardSetU1[onCol])[index], info->context, bytesGuardCurve);
            EV_ALLOC((*ppGuardSetUInner[onCol])[index], info->context, bytesGuardCurve);
            if (doFrac) {
                EV_ALLOC((*ppGuardSetUInnerFrac[onCol])[index], info->context, bytesGuardCurve);
                if (!onCol) {
                    // XXX maybe just point to this
                    CopyCurve(info, (*ppGuardSetUInnerFrac[0])[index], (*info->ppGuardSetUInnerFrac[0])[index]);
                }
            }
        }
    }

    // again for V
    nBytes = EV_ROUND(info->maxAttr * sizeof(NV_PATCH_CURVE_INFO *));
    for (onRow = 0;  onRow <= stitch_nSwatchV;  onRow++) {
        EV_ALLOC(ppGuardSetV0[onRow], info->context, nBytes);
        EV_ALLOC(ppGuardSetV1[onRow], info->context, nBytes);
        EV_ALLOC(ppGuardSetVInner[onRow], info->context, nBytes);
        if (doFrac) {
            EV_ALLOC(ppGuardSetVInnerFrac[onRow], info->context, nBytes);
        }

        for (index = 0; index < info->maxAttr; index++) {
            if (!(info->evalEnables & (1 << index))) {
                continue;
            }
            EV_ALLOC((*ppGuardSetV0[onRow])[index], info->context, bytesGuardCurve);
            EV_ALLOC((*ppGuardSetV1[onRow])[index], info->context, bytesGuardCurve);
            EV_ALLOC((*ppGuardSetVInner[onRow])[index], info->context, bytesGuardCurve);
            if (doFrac) {
                EV_ALLOC((*ppGuardSetVInnerFrac[onRow])[index], info->context, bytesGuardCurve);
                if (!onRow) {
                    // XXX maybe just point to this
                    CopyCurve(info, (*ppGuardSetVInnerFrac[0])[index], (*info->ppGuardSetVInnerFrac[0])[index]);
                }
            }
        }
    }
    EV_ALLOC_FULL(info->context);

    /*****************************************************************************/
    // This code is analogous to code in DrawIntPatch() to calc quadInfo->guard[i]->pGuardXXX
    // BEGIN SIMILAR CODE
    if (quadInfo->vMaxDir == 1) {
        if (quadInfo->stitchTop) {
            ppGuardSetUBegin = ppGuardSetU0;
            if (quadInfo->vMaxSegs > 1) {
                ppGuardSetUEnd = ppGuardSetUInner;
                ppStitchUInner = ppGuardSetUInner;
            } else {
                ppGuardSetUEnd = ppGuardSetU0;
                ppStitchUInner = ppGuardSetU0;
            }
        } else if (quadInfo->stitchBottom) {
            if (quadInfo->vMaxSegs > 1) {
                ppGuardSetUBegin = ppGuardSetUInner;
                ppStitchUInner = ppGuardSetUInner;
            } else {
                ppGuardSetUBegin = ppGuardSetU1;
                ppStitchUInner = ppGuardSetU1;
            }
            ppGuardSetUEnd = ppGuardSetU1;
        } else {
            ppGuardSetUBegin = ppGuardSetU0;
            ppStitchUInner = NULL;
            ppGuardSetUEnd = ppGuardSetU1;
        }
    } else {
        if (quadInfo->stitchTop) {
            if (quadInfo->vMaxSegs > 1) {
                ppGuardSetUBegin = ppGuardSetUInner;
                ppStitchUInner = ppGuardSetUInner;
            } else {
                ppGuardSetUBegin = ppGuardSetU0;
                ppStitchUInner = ppGuardSetU0;
            }
            ppGuardSetUEnd = ppGuardSetU0;
        } else if (quadInfo->stitchBottom) {
            ppGuardSetUBegin = ppGuardSetU1;
            if (quadInfo->vMaxSegs > 1) {
                ppGuardSetUEnd = ppGuardSetUInner;
                ppStitchUInner = ppGuardSetUInner;
            } else {
                ppGuardSetUEnd = ppGuardSetU1;
                ppStitchUInner = ppGuardSetU1;
            }
        } else {
            ppGuardSetUBegin = ppGuardSetU1;
            ppStitchUInner = NULL;
            ppGuardSetUEnd = ppGuardSetU0;
        }
    }

    if (quadInfo->uMaxDir == 1) {
        if (quadInfo->stitchRight) {
            ppGuardSetVBegin = ppGuardSetV0;
            if (quadInfo->uMaxSegs > 1) {
                ppGuardSetVEnd = ppGuardSetVInner;
                ppStitchVInner = ppGuardSetVInner;
            } else {
                ppGuardSetVEnd = ppGuardSetV0;
                ppStitchVInner = ppGuardSetV0;
            }
        } else if (quadInfo->stitchLeft) {
            if (quadInfo->uMaxSegs > 1) {
                ppGuardSetVBegin = ppGuardSetVInner;
                ppStitchVInner = ppGuardSetVInner;
            } else {
                ppGuardSetVBegin = ppGuardSetV1;
                ppStitchVInner = ppGuardSetV1;
            }
            ppGuardSetVEnd = ppGuardSetV1;
        } else {
            ppGuardSetVBegin = ppGuardSetV0;
            ppStitchVInner = NULL;
            ppGuardSetVEnd = ppGuardSetV1;
        }
    } else {
        if (quadInfo->stitchRight) {
            if (quadInfo->uMaxSegs > 1) {
                ppGuardSetVBegin = ppGuardSetVInner;
                ppStitchVInner = ppGuardSetVInner;
            } else {
                ppGuardSetVBegin = ppGuardSetV0;
                ppStitchVInner = ppGuardSetV0;
            }
            ppGuardSetVEnd = ppGuardSetV0;
        } else if (quadInfo->stitchLeft) {
            ppGuardSetVBegin = ppGuardSetV1;
            if (quadInfo->uMaxSegs > 1) {
                ppGuardSetVEnd = ppGuardSetVInner;
                ppStitchVInner = ppGuardSetVInner;
            } else {
                ppGuardSetVEnd = ppGuardSetV1;
                ppStitchVInner = ppGuardSetV1;
            }
        } else {
            ppGuardSetVBegin = ppGuardSetV1;
            ppStitchVInner = NULL;
            ppGuardSetVEnd = ppGuardSetV0;
        }
    }
    if (doFrac) {
        ppStitchUInnerFrac = ppGuardSetUEnd = ppGuardSetUInnerFrac;
        ppStitchVInnerFrac = ppGuardSetVEnd = ppGuardSetVInnerFrac;
    }
    // END SIMILAR CODE

    /*****************************************************************************/
    // calc initial big step U guard curves
    // For now, use BV0, BV1, SV0, SV1  for BU0, BU1, BUInner, BUInnerFrac
    // XXX TODO: one of the outside curves may not require as many steps
    for (index = 0; index < info->maxAttr; index++) {
        if (!(info->evalEnables & (1 << index))) {
            continue;
        }
        CopyCurve(info, BV0[index], quadInfoOrig->pU0[index]);
        RetessellateFDCurveInOut(info, quadInfo->du0 * info->maxSwatch, quadInfo->du0,
                            quadInfoOrig->pU0[index], pGuardSetCurve(info, ppGuardSetU0[0], index));
        CopyCurve(info, BV1[index], quadInfoOrig->pU1[index]);
        RetessellateFDCurveInOut(info, quadInfo->du0 * info->maxSwatch, quadInfo->du0,
                            quadInfoOrig->pU1[index], pGuardSetCurve(info, ppGuardSetU1[0], index));

        if (quadInfo->needUInner) {
            CopyCurve(info, SV0[index], quadInfoOrig->pUInner[index]);
            RetessellateFDCurveInOut(info, quadInfo->du0 * info->maxSwatch, quadInfo->du0,
                            quadInfoOrig->pUInner[index], pGuardSetCurve(info, ppGuardSetUInner[0], index));
        }
        //ffif (doFrac  &&  EV_GUARD_ATTR(index)) { // }
        if (doFrac) {
            CopyCurve(info, SV1[index], pGuardSetCurve(info, ppGuardSetUInnerFrac[0], index));
            RetessellateFDCurveInOut(info, quadInfo->duMax * info->maxSwatch, quadInfo->duMax, SV1[index],
                                        pGuardSetCurve(info, ppGuardSetUInnerFrac[0], index));
        }
    }

    // make big steps and cvt to small for later
    for (onCol = 1;  onCol <= stitch_nSwatchU;  onCol++) {
        // copy all prev attrs
        memcpy(pGuardSetCurve(info, ppGuardSetU0[onCol], info->firstAttr),
                                pGuardSetCurve(info, ppGuardSetU0[onCol-1], info->firstAttr), 3*bytesGuardCurveAllAttr);
        for (index = 0;  index < info->maxAttr;  index++) {
            if (!(info->evalEnables & (1 << index))) {
                continue;
            }
            OffsetCurve(info, BV0[index], 1);
            RetessellateFDCurveInOut(info, quadInfo->du0 * info->maxSwatch, quadInfo->du0, BV0[index],
                            pGuardSetCurve(info, ppGuardSetU0[onCol], index));
            OffsetCurve(info, BV1[index], 1);
            RetessellateFDCurveInOut(info, quadInfo->du1 * info->maxSwatch, quadInfo->du1, BV1[index],
                            pGuardSetCurve(info, ppGuardSetU1[onCol], index));
            if (quadInfo->needUInner && quadInfo->stitchVEnd && (EV_GUARD_ATTR(index))) {
                OffsetCurve(info, SV0[index], 1);
                RetessellateFDCurveInOut(info, quadInfo->duMax * info->maxSwatch, quadInfo->duMax, SV0[index],
                            pGuardSetCurve(info, ppGuardSetUInner[onCol], index));
            }
            //ffif (doFrac  &&  EV_GUARD_ATTR(index)) { // }
            if (doFrac) {
                OffsetCurve(info, SV1[index], 1);
                RetessellateFDCurveInOut(info, quadInfo->duMax * info->maxSwatch, quadInfo->duMax, SV1[index],
                            pGuardSetCurve(info, ppGuardSetUInnerFrac[onCol], index));
            }
        }
    }

    /*****************************************************************************/
    /*****************************************************************************/
    // calc initial big step V guard curves
    // For now, use BV0, BV1, SV0, SV1  for BV0, BV1, BVInner, BVInnerFrac
    // XXX TODO: one of the outside curves may not require as many steps
    for (index = 0; index < info->maxAttr; index++) {
        if (!(info->evalEnables & (1 << index))) {
            continue;
        }
        CopyCurve(info, BV0[index], quadInfoOrig->pV0[index]);
        RetessellateFDCurveInOut(info, quadInfo->dv0 * info->maxSwatch, quadInfo->dv0,
                            quadInfoOrig->pV0[index], pGuardSetCurve(info, ppGuardSetV0[0], index));
        CopyCurve(info, BV1[index], quadInfoOrig->pV1[index]);
        RetessellateFDCurveInOut(info, quadInfo->dv0 * info->maxSwatch, quadInfo->dv0,
                            quadInfoOrig->pV1[index], pGuardSetCurve(info, ppGuardSetV1[0], index));

        if (quadInfo->needVInner && quadInfo->stitchUEnd) {
            CopyCurve(info, SV0[index], quadInfoOrig->pVInner[index]);
            RetessellateFDCurveInOut(info, quadInfo->dv0 * info->maxSwatch, quadInfo->dv0,
                            quadInfoOrig->pVInner[index], pGuardSetCurve(info, ppGuardSetVInner[0], index));
        }
        if (doFrac) {
            CopyCurve(info, SV1[index], pGuardSetCurve(info, ppGuardSetVInnerFrac[0], index));
            RetessellateFDCurveInOut(info, quadInfo->dvMax * info->maxSwatch, quadInfo->dvMax, SV1[index],
                                        pGuardSetCurve(info, ppGuardSetVInnerFrac[0], index));
        }
    }

    // make big steps and cvt to small for later
    for (onRow = 1;  onRow <= stitch_nSwatchV;  onRow++) {
        // copy all prev attrs
        memcpy(pGuardSetCurve(info, ppGuardSetV0[onRow], info->firstAttr),
                                pGuardSetCurve(info, ppGuardSetV0[onRow-1], info->firstAttr), 3*bytesGuardCurveAllAttr);
        for (index = 0;  index < info->maxAttr;  index++) {
            if (!(info->evalEnables & (1 << index))) {
                continue;
            }
            OffsetCurve(info, BV0[index], 1);
            RetessellateFDCurveInOut(info, quadInfo->dv0 * info->maxSwatch, quadInfo->dv0, BV0[index],
                            pGuardSetCurve(info, ppGuardSetV0[onRow], index));
            OffsetCurve(info, BV1[index], 1);
            RetessellateFDCurveInOut(info, quadInfo->dv1 * info->maxSwatch, quadInfo->dv1, BV1[index],
                            pGuardSetCurve(info, ppGuardSetV1[onRow], index));
            if (quadInfo->needVInner && quadInfo->stitchUEnd) {
                OffsetCurve(info, SV0[index], 1);
                RetessellateFDCurveInOut(info, quadInfo->dvMax * info->maxSwatch, quadInfo->dvMax, SV0[index],
                            pGuardSetCurve(info, ppGuardSetVInner[onRow], index));
            }
            if (doFrac) {
                OffsetCurve(info, SV1[index], 1);
                RetessellateFDCurveInOut(info, quadInfo->dvMax * info->maxSwatch, quadInfo->dvMax, SV1[index],
                            pGuardSetCurve(info, ppGuardSetVInnerFrac[onRow], index));
            }
        }
    }

    /*****************************************************************************/
    // work on regular grid

    // calc swaths WITHOUT including ending stitch
    index = 1 + doFrac; // temp // adjust to not draw FRAC RIGHT and TOP stitch
    nSwatchU = (quadInfo->uMaxSegs - index - quadInfo->stitchUEnd) / info->maxSwatch + 1;
    nSwatchV = (quadInfo->vMaxSegs - index - quadInfo->stitchVEnd) / info->maxSwatch + 1;
    if (!doFrac) {
        // note: for FRAC cases, the quadInfo->uMaxSegs used at this point can be
        // quite different than the full MAX calculated in evalCommonSetup()
        // Therefore, the following is only generally true for INT cases.
        nvAssert((stitch_nSwatchU - nSwatchU) <= 1);
    }

    // convert small to big steps for starting matrix BB0
    pBB0 = &matrix0;
    pBB1 = &matrix1;

    for (index = 0; index < info->maxAttr; index++) {
        if (!(info->evalEnables & (1 << index))) {
            continue;
        }
        CopyMatrix(info, (*pBB0)[index], matrices[index]);
    }

    // tag:sss swathing loop
    for (onCol = 0;  onCol < nSwatchU;  onCol++) {
        int rem, makeSV1 = 0;

        pLL = LL;
        pLR = LR;
        pLLnor = LLnor;
        pLRnor = LRnor;

        CopyMatrixSet(info, info->evalEnables, pBB1, pBB0);     // horz step BB0 to BB1
        OffsetFDMatrixSet(info, info->evalEnables, pBB1, 1, 0); // ...

        // cvt BB0 to BS0 (in place)
        pBS0 = pBB0;
        RetessellateFDMatrixSet(info, info->evalEnables,
                                        quadInfo->du0 * info->maxSwatch, 1.f,
                                        quadInfo->du0,                   1.f, pBS0);

        // if no left stitch, then BV0 may be extracted from BS0,
        // otherwise must create and step BV0 and SV0
        needBV0 = !onCol && quadInfoOrig->stitchUBegin;
        if (needBV0) {
            CopyMatrixSet(info, info->evalEnables, (FDMatrixAttrSet *)info->ppMatrixSetSS0, pBS0);
            // XXX: steps of '1'-and-never-more can be done with short-cut
            OffsetFDMatrixSet(info, info->evalEnables, (FDMatrixAttrSet *)info->ppMatrixSetSS0, 1, 0);
            ExtractVCurveSet(info, info->evalEnables, &BV0, (FDMatrixAttrSet *)info->ppMatrixSetSS0);
        }

        if (onCol != (nSwatchU-1)) {
            makeSV1 = 1;
            ExtractVCurveSet(info, info->evalEnables, &BV1, pBB1);  // extract BV1 from BB1
            pBV1 = &BV1;
            pSV1 = &SV1;
        } else {
            makeSV1 = 0;
            pBV1 = pSV1 = ppGuardSetVEnd[0];
        }

        if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
            pUL = (*pBS0)[ATTR_V]->data[0][0];
        }
        if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
            pULnor = (*pBS0)[ATTR_N]->data[0][0];
        }

        for (onRow = 0;  onRow < nSwatchV;  onRow++) {
            int needUL = 0;
            int needLL = 0;

            // prepare matrices and curves for this swatch
            CopyMatrixSet(info, info->evalEnables, &SS0, pBS0);
            RetessellateFDMatrixSet(info, info->evalEnables,
                                        1.f, quadInfo->dv0 * info->maxSwatch,
                                        1.f, quadInfo->dv0,                   &SS0);

            OffsetFDMatrixSet(info, info->evalEnables, pBS0, 0, 1); // up to next row
            // now pUL points to UL

            if (makeSV1) {
                retVal |= RetessellateFDCurveSetInOut(info, info->evalEnables,
                                        quadInfo->dv0 * info->maxSwatch, quadInfo->dv0, &BV1, &SV1);
                OffsetCurveSet(info, info->evalEnables & EV_GUARD_BITS, &BV1, 1);
                // now pUR points to UR

            } else {
                pBV1 = ppGuardSetVEnd[onRow+1];
                pSV1 = ppGuardSetVEnd[onRow];
            }

            if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
                pLR = (*pSV1)[ATTR_V]->coeffs[0];
                pUR = (*pBV1)[ATTR_V]->coeffs[0];
            }
            if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
                pLRnor = (*pSV1)[ATTR_N]->coeffs[0];
                pURnor = (*pBV1)[ATTR_N]->coeffs[0];
            }

            if (needBV0) {
                // BV0-->SV0 will be saved away for VInner transition curve set
                retVal |= RetessellateFDCurveSetInOut(info, info->evalEnables,
                                        quadInfo->dv0 * info->maxSwatch, quadInfo->dv0, &BV0, &SV0);
                OffsetCurveSet(info, info->evalEnables, &BV0, 1);
                CopyCurveSet(info, info->evalEnables, ppGuardSetVBegin[onRow], &SV0);
                if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
                    guardSetVBegin[ATTR_V] = (*ppGuardSetVBegin[onRow])[ATTR_V];
                }
                if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
                    guardSetVBegin[ATTR_N] = (*ppGuardSetVBegin[onRow])[ATTR_N];
                }
            } else if (onCol) {
                // other columns extract SV0 from SS0
                ExtractVCurveSet(info, info->evalEnables & EV_GUARD_BITS, &SV0, &SS0);
                if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
                    guardSetVBegin[ATTR_V] = SV0[ATTR_V];
                }
                if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
                    guardSetVBegin[ATTR_N] = SV0[ATTR_N];
                }
            } else {
                // first column, no transition, get from original V0 curves
                if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
                    guardSetVBegin[ATTR_V] = (*ppGuardSetVBegin[onRow])[ATTR_V];
                }
                if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
                    guardSetVBegin[ATTR_N] = (*ppGuardSetVBegin[onRow])[ATTR_N];
                }
            }
            if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
                pLL = guardSetVBegin[ATTR_V]->coeffs[0];
            }
            if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
                pLLnor = guardSetVBegin[ATTR_N]->coeffs[0];
            }

            // ***** prepare guard information
            if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
                guardSetUBegin[ATTR_V] = NULL;     // assume not used
                //ffguardSetUEnd[ATTR_V] = NULL;       // assume not used
                //ffguardSetVBegin[ATTR_V] = NULL;     // assume not used
                guardSetVEnd[ATTR_V] = NULL;       // assume not used
            }
            if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
                guardSetUBegin[ATTR_N] = NULL;       // assume not used
                //ffguardSetUEnd[ATTR_N] = NULL;         // assume not used
                //ffguardSetVBegin[ATTR_N] = NULL;       // assume not used
                guardSetVEnd[ATTR_N] = NULL;         // assume not used
            }

            info->swatchFlags = NV_PATCH_SWATCH;
            info->setGridCorner = 0;

            if (!onCol) {
                info->swatchFlags |= NV_PATCH_SWATCH_FIRST_COL;
                quadInfo->stitchUBegin = quadInfoOrig->stitchUBegin;
                if (!onRow) {
                    info->setGridCorner |= NV_PATCH_SET_GRID_CORNER00;
                }
            } else {
                quadInfo->stitchUBegin = 0;
            }

            for (index = 0; index < info->maxAttr; index++) {
                if (!(info->evalEnables & (1 << index))) {
                    continue;
                }
                //ffif (!EV_GUARD_ATTR(index)  &&  !doFrac) {
                    //ffcontinue;
                //ff}
                guardSetVEnd[index] = (*pSV1)[index];
            }

            if (onCol == (nSwatchU-1)) {
                info->swatchFlags |= NV_PATCH_SWATCH_LAST_COL;
                quadInfo->stitchUEnd = quadInfoOrig->stitchUEnd;

                if (!onRow) {
                    info->setGridCorner |= NV_PATCH_SET_GRID_CORNER01;
                }
            } else {
                quadInfo->stitchUEnd = 0;
            }
// ...
            quadInfo->pSwatchUBegin = NULL;

            if (!onRow) {
                info->swatchFlags |= NV_PATCH_SWATCH_FIRST_ROW;
                quadInfo->stitchVBegin = quadInfoOrig->stitchVBegin;
                quadInfo->pSwatchUBegin = ppGuardSetUBegin[onCol];

            } else {
                quadInfo->stitchVBegin = 0;
            }

            quadInfo->pSwatchUEnd = NULL;
            if (onRow == (nSwatchV-1)) {
                info->swatchFlags |= NV_PATCH_SWATCH_LAST_ROW;
                quadInfo->stitchVEnd = quadInfoOrig->stitchVEnd;
                quadInfo->pSwatchUEnd = ppGuardSetUEnd[onCol];
                if (!onCol) {
                    info->setGridCorner |= NV_PATCH_SET_GRID_CORNER10;
                    // pUL will be set below
                }

                if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
                    pUL = (*ppGuardSetUEnd[onCol  ])[ATTR_V]->coeffs[0];
                }
                if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
                    pULnor = (*ppGuardSetUEnd[onCol  ])[ATTR_N]->coeffs[0];
                }

                if (onCol==(nSwatchU-1)) {
                    info->setGridCorner |= NV_PATCH_SET_GRID_CORNER11;
                    // UR is obtained below after computing uMaxSegs and vMaxSegs (for FillIntCornerUR)
                } else {
                    if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
                        pUR = (*ppGuardSetUEnd[onCol+1])[ATTR_V]->coeffs[0];
                    }
                    if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
                        pURnor = (*ppGuardSetUEnd[onCol+1])[ATTR_N]->coeffs[0];
                    }
                }
            } else {
                quadInfo->stitchVEnd = 0;
                // extract top guard curve for sub-patch UEnd, and point to it
                if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
                    ExtractUCurve(info, quadInfo->pUInner[ATTR_V], (*pBS0)[ATTR_V]);
                    guardSetUEnd[ATTR_V] = quadInfo->pUInner[ATTR_V];
                }
                if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
                    ExtractUCurve(info, quadInfo->pUInner[ATTR_N], (*pBS0)[ATTR_N]);
                    guardSetUEnd[ATTR_N] = quadInfo->pUInner[ATTR_N];
                }
                quadInfo->pSwatchUEnd = &guardSetUEnd;

                if (quadInfo->stitchUBegin) {
                    // UL is not at the swatch corner, calc below when safe to assign info->pSwatchCorner[ATTR_V][1][0]
                    needUL = 1;
                }
            }

            quadInfo->uMaxSegs = info->maxSwatch;
            quadInfo->vMaxSegs = info->maxSwatch;
            rem = quadInfoOrig->uMaxSegs - doFrac - quadInfo->stitchUEnd - (onCol + 1) * info->maxSwatch;
            if (rem < 0) {
                quadInfo->uMaxSegs += rem;
            }
            rem = quadInfoOrig->vMaxSegs - doFrac - quadInfo->stitchVEnd - (onRow + 1) * info->maxSwatch;
            if (rem < 0) {
                quadInfo->vMaxSegs += rem;
            }
            quadInfo->uMaxSegs += quadInfo->stitchUEnd;
            quadInfo->vMaxSegs += quadInfo->stitchVEnd;

            info->pSwatchCorner[ATTR_V][0][0] = pLL;
            info->pSwatchCorner[ATTR_V][0][1] = pLR;
            info->pSwatchCorner[ATTR_V][1][0] = pUL;
            info->pSwatchCorner[ATTR_V][1][1] = pUR;

            info->pSwatchCorner[ATTR_N][0][0] = pLLnor;
            info->pSwatchCorner[ATTR_N][0][1] = pLRnor;
            info->pSwatchCorner[ATTR_N][1][0] = pULnor;
            info->pSwatchCorner[ATTR_N][1][1] = pURnor;

            if (info->setGridCorner & NV_PATCH_SET_GRID_CORNER00) {
                if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
                    FillIntCornerLL(info, quadInfo, quadInfoOrig, ATTR_V,
                                    info->gridCorner[ATTR_V][  1-quadInfo->vMaxDir][ 1-quadInfo->uMaxDir]);
                }
                if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
                    FillIntCornerLL(info, quadInfo, quadInfoOrig, ATTR_N,
                                    info->gridCorner[ATTR_N][  1-quadInfo->vMaxDir][ 1-quadInfo->uMaxDir]);
                }
            }
            if (info->setGridCorner & NV_PATCH_SET_GRID_CORNER01) {
                for (index = 0; index < info->maxAttr; index++) {
                    if (!(info->evalEnables & (1 << index))) {
                        continue;
                    }
                    if (!EV_GUARD_ATTR(index)  &&  !doFrac) {
                        continue;
                    }
                    FillIntCornerLR(info, quadInfo, quadInfoOrig, index,
                                info->gridCorner[index][  1-quadInfo->vMaxDir][   quadInfo->uMaxDir]);
                }
            }

            if (needUL) {
                if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
                    // extract top guard curve for sub-patch UEnd, and point to it
                    ExtractUCurve(info, quadInfo->pUInner[ATTR_V], (*pBS0)[ATTR_V]);
                    OffsetCurve(info, quadInfo->pUInner[ATTR_V], 1);
                    info->pSwatchCorner[ATTR_V][1][0] = quadInfo->pUInner[ATTR_V]->coeffs[0];
                }
                if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
                    // extract top guard curve for sub-patch UEnd, and point to it
                    ExtractUCurve(info, quadInfo->pUInner[ATTR_N], (*pBS0)[ATTR_N]);
                    OffsetCurve(info, quadInfo->pUInner[ATTR_N], 1);
                    info->pSwatchCorner[ATTR_N][1][0] = quadInfo->pUInner[ATTR_N]->coeffs[0];
                }
            }
            if (info->setGridCorner & NV_PATCH_SET_GRID_CORNER10) {
                for (index = 0; index < info->maxAttr; index++) {
                    if (!(info->evalEnables & (1 << index))) {
                        continue;
                    }
                    if (!EV_GUARD_ATTR(index)  &&  !doFrac) {
                        continue;
                    }
                    if (quadInfo->stitchUBegin) {
                        // UL is not at the swatch corner
                        info->pSwatchCorner[index][1][0] =
                                    info->gridCorner[index][    quadInfo->vMaxDir][ 1-quadInfo->uMaxDir];
                        FillIntCornerUL(info, quadInfo, quadInfoOrig, index, info->pSwatchCorner[index][1][0]);
                    } else {
                        if (index == ATTR_V) {
                            CopyPoint(info->gridCorner[ATTR_V][  quadInfo->vMaxDir][ 1-quadInfo->uMaxDir], pUL);
                        }
                        if (index == ATTR_N) {
                            CopyPoint(info->gridCorner[ATTR_N][  quadInfo->vMaxDir][ 1-quadInfo->uMaxDir], pULnor);
                        }
                    }
                }
            }
            if (info->setGridCorner & NV_PATCH_SET_GRID_CORNER11) {
                // UR may not be swatch corner
                for (index = 0; index < info->maxAttr; index++) {
                    if (!(info->evalEnables & (1 << index))) {
                        continue;
                    }
                    //ffif (!EV_GUARD_ATTR(index)  &&  !doFrac) {
                        //ffcontinue;
                    //ff}
                    info->pSwatchCorner[index][1][1] =
                                    info->gridCorner[index][    quadInfo->vMaxDir][   quadInfo->uMaxDir];
                    FillIntCornerUR(info, quadInfo, quadInfoOrig, index, info->pSwatchCorner[index][1][1]);
                }
            }

            // Copy V guard curves
            // Note: must be done after FillIntCorner*() because the start values for
            // the pSwatch curves may be tweaked to match between the regular grid and stitches.
            for (index = 0; index < info->maxAttr; index++) {
                if (!EV_GUARD_ATTR(index)  &&  !doFrac) {
                    continue;
                }
                if (!(info->evalEnables & (1 << index))) {
                    continue;
                }
                CopyCurve(info, info->tempVEnd[index], (*quadInfo->pSwatchVEnd)[index]);
                if (!EV_GUARD_ATTR(index)) {
                    continue;
                }
                CopyCurve(info, info->tempVBegin[index], (*quadInfo->pSwatchVBegin)[index]);
            }

            // call to draw regular grid for each small row
            {
                //cache a ptr to the global patch data inside the subpatch structure.
                if (/* ff (info->flags & NV_PATCH_FLAG_AUTO_NORMAL)    || */
                        (info->evalEnables & 1<<ATTR_N)) { info->swatchFlags |= NV_PATCH_HAS_NORMAL; };

                if ((info->swatchFlags & NV_PATCH_SWATCH_FIRST_ROW) && (info->swatchFlags & NV_PATCH_SWATCH_FIRST_COL)) {
                    ComputeKelvinHOSControlWords(info, quadInfoOrig, info->swatchFlags);
                }

                retVal |= DrawIntPatchGrid(info, quadInfo, &SS0[0]);
            }
        }

        if (needBV0) {
            // complete generation of any inner curves
            for (onRow = nSwatchV;  onRow < stitch_nSwatchV;  onRow++) {
                retVal |= RetessellateFDCurveSetInOut(info, info->evalEnables,
                                        quadInfo->dv0 * info->maxSwatch, quadInfo->dv0, &BV0, &SV0);
                OffsetCurveSet(info, info->evalEnables, &BV0, 1);
                CopyCurveSet(info, info->evalEnables, ppGuardSetVBegin[onRow], &SV0);
            }
        }

        // swap ptrs for next big column
        pTempMatrix = pBB0; pBB0 = pBB1; pBB1 = pTempMatrix;
    }

    if (quadInfoOrig->stitchVBegin) { // now done in U1 setup || (quadInfo->stitchVEnd && (info->evalEnables & ~EV_GUARD_BITS))) { // }
        // last set of curves from regular grid was not written
        // because the regular grid is multiple of swatch size
        int lEnables = info->evalEnables;

        if (quadInfo->stitchVEnd) {
            lEnables = info->evalEnables & ~EV_GUARD_BITS;
        }
        for (onCol = nSwatchU;  onCol < stitch_nSwatchU;  onCol++) {
            //ffCopyMatrixSet(info, lEnables, pBB1, pBB0);     // horz step BB0 to BB1
            //ffOffsetFDMatrixSet(info, lEnables, pBB1, 1, 0); // ...

            // cvt BB0 to BS0 (in place)
            pBS0 = pBB0;
            RetessellateFDMatrixSet(info, lEnables,
                                        quadInfo->du0 * info->maxSwatch, 1.f,
                                        quadInfo->du0,                   1.f, pBS0);

            // prepare matrices for swatch row 0
            CopyMatrixSet(info, lEnables, &SS0, pBS0);
            RetessellateFDMatrixSet(info, lEnables,
                                        1.f, quadInfo->dv0 * info->maxSwatch,
                                        1.f, quadInfo->dv0,                   &SS0);

            if (quadInfoOrig->stitchVBegin) {
                OffsetFDMatrixSet(info, lEnables, &SS0, 0, 1);
                ExtractUCurveSet(info, lEnables, ppGuardSetUBegin[onCol], &SS0);
            } else {
                OffsetFDMatrixSet(info, lEnables, &SS0, 0, quadInfo->vMaxSegs-1);
                ExtractUCurveSet(info, lEnables, ppGuardSetUEnd[onCol], &SS0);
            }

            // swap ptrs for next big column
            //ffpTempMatrix = pBB0; pBB0 = pBB1; pBB1 = pTempMatrix;
        }
    }

    // but restore everything else
    quadInfo = quadInfoOrig;

    DrawAllPatchTransitions(info, quadInfo,
                                    ppGuardSetU0, ppGuardSetU1, ppStitchUInner, ppStitchUInnerFrac,
                                    ppGuardSetV0, ppGuardSetV1, ppStitchVInner, ppStitchVInnerFrac);

    if ((info->swatchFlags & NV_PATCH_SWATCH_LAST_ROW) && (info->swatchFlags & NV_PATCH_SWATCH_LAST_COL)) {
        HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

        HOS_NOTE("End Patch");
        pPush = pDevice->StartPush();
        Push1(pPush, NV097_SET_END_PATCH, 0);
        pDevice->EndPush(pPush + 2);
    }

drawIntPatchSwathsEnd:
    EV_FREE(ppGuardSetU0, info->context);
    EV_FREE(ppGuardSetU1, info->context);
    EV_FREE(ppGuardSetUInner, info->context);
    EV_FREE(ppGuardSetV0, info->context);
    EV_FREE(ppGuardSetV1, info->context);
    EV_FREE(ppGuardSetVInner, info->context);

    return retVal;
}

unsigned int
DrawIntPatch(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, FDMatrix **matrices)
{
    unsigned int retVal = NV_PATCH_EVAL_OK;
    int doFrac = !!(info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL);
    int index;

    FDCurveAttrSet curves1, curves2;
    FDCurveAttrSet guardSetU0, guardSetU1, guardSetUInner;
    FDCurveAttrSet guardSetV0, guardSetV1, guardSetVInner;

    FDCurveAttrSet *ppGuardSetU0[1];  // pp to curves per attribute
    FDCurveAttrSet *ppGuardSetU1[1];  // [maxAttr][maxOrder][4]
    FDCurveAttrSet *ppGuardSetV0[1];  // ...
    FDCurveAttrSet *ppGuardSetV1[1];
    FDCurveAttrSet *ppGuardSetUInner[1];
    FDCurveAttrSet *ppGuardSetVInner[1];

    FDCurveAttrSet **ppGuardSetUBegin, **ppGuardSetUEnd;    // at begin/end of regular grid
    FDCurveAttrSet **ppGuardSetVBegin, **ppGuardSetVEnd;    // at begin/end of regular grid
    FDCurveAttrSet **ppStitchUInner = NULL, **ppStitchVInner = NULL; // inner curves for stitching

    EV_ALLOC_DECL();
    int nBytes;
    int bytesGuardCurve = EV_ROUND(PATCH_CURVE_HEADER_SIZE + (info->maxOrder * 4) * sizeof(float));   // truncated NV_PATCH_CURVE_INFO
    int bytesGuardCurveAllAttr = info->nAttr * bytesGuardCurve;                 // just the set of curve info
    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();


    ppGuardSetU0[0] = &guardSetU0;  // pp to curves per attribute
    ppGuardSetU1[0] = &guardSetU1;  // [maxAttr][maxOrder][4]
    ppGuardSetV0[0] = &guardSetV0;  // ...
    ppGuardSetV1[0] = &guardSetV1;
    ppGuardSetUInner[0] = &guardSetUInner;
    ppGuardSetVInner[0] = &guardSetVInner;

    if (!doFrac) {
        info->reverse = info->reverse ^ (quadInfo->uMaxDir ^ quadInfo->vMaxDir);
    }
    setFrontFace(info, 1 ^ info->reverse);

    nBytes = 0;
    if (nBytes) {
        EV_ALLOC_INIT_WITH_CACHE(info->context, nBytes, drawIntPatchEnd, info->pCache[EV_CACHE_DRAW_SW]);
    }

    for (index = 0; index < info->maxAttr; index++) {
        if (!(info->evalEnables & (1<<index))) {
            continue;
        }

        if (info->swatchFlags & NV_PATCH_SWATCH) {
            continue;
        }

        guardSetU0[index]     = quadInfo->pU0[index];
        guardSetU1[index]     = quadInfo->pU1[index];
        guardSetUInner[index] = quadInfo->pUInner[index];
        guardSetV0[index]     = quadInfo->pV0[index];
        guardSetV1[index]     = quadInfo->pV1[index];
        guardSetVInner[index] = quadInfo->pVInner[index];
    }
    if (nBytes) {
        EV_ALLOC_FULL(info->context);
    }

    if (doFrac) {
        for (index = 0; index < info->maxAttr; index++) {
            if (!(info->evalEnables & (1 << index))) {
                continue;
            }

            //ffif (EV_GUARD_ATTR(index))
            {
                // Calc UInnerFrac
// XXX maybe do this in lower routine ?
CopyMatrix(info, &info->tempMatrix, matrices[index]);
if (info->swatchFlags & NV_PATCH_SWATCH) {
    RetessellateFDMatrix(info, 1.f, quadInfo->dvMax * info->maxSwatch, 1.f, quadInfo->dvMax, &info->tempMatrix);    // big to small
}
                OffsetFDMatrix_Extract_Discard(info, info->guardSetUInnerFrac[index], 0, &info->tempMatrix, 0, quadInfo->vMaxSegs-1);
            }

            // Calc VInnerFrac
// XXX maybe do this in lower routine ?
CopyMatrix(info, &info->tempMatrix, matrices[index]);
if (info->swatchFlags & NV_PATCH_SWATCH) {
    RetessellateFDMatrix(info, quadInfo->duMax * info->maxSwatch, 1.f, quadInfo->duMax, 1.f, &info->tempMatrix);    // big to small
}
            OffsetFDMatrix_Extract_Discard(info, info->guardSetVInnerFrac[index], 1, &info->tempMatrix, quadInfo->uMaxSegs-1, 0);
        }
    }

    /*****************************************************************************/
    // This code is analogous to code in DrawIntPatchSwaths() to calc ppGuardXXX
    // BEGIN SIMILAR CODE
    if (quadInfo->vMaxDir == 1) {
        if (quadInfo->stitchTop) {
            ppGuardSetUBegin = ppGuardSetU0;
            if (quadInfo->vMaxSegs > 1) {
                ppGuardSetUEnd = ppGuardSetUInner;
                ppStitchUInner = ppGuardSetUInner;
            } else {
                ppGuardSetUEnd = ppGuardSetU0;
                ppStitchUInner = ppGuardSetU0;
            }
        } else if (quadInfo->stitchBottom) {
            if (quadInfo->vMaxSegs > 1) {
                ppGuardSetUBegin = ppGuardSetUInner;
                ppStitchUInner = ppGuardSetUInner;
            } else {
                ppGuardSetUBegin = ppGuardSetU1;
                ppStitchUInner = ppGuardSetU1;
            }
            ppGuardSetUEnd = ppGuardSetU1;
        } else {
            ppGuardSetUBegin = ppGuardSetU0;
            ppStitchUInner = NULL;
            ppGuardSetUEnd = ppGuardSetU1;
        }
    } else {
        if (quadInfo->stitchTop) {
            if (quadInfo->vMaxSegs > 1) {
                ppGuardSetUBegin = ppGuardSetUInner;
                ppStitchUInner = ppGuardSetUInner;
            } else {
                ppGuardSetUBegin = ppGuardSetU0;
                ppStitchUInner = ppGuardSetU0;
            }
            ppGuardSetUEnd = ppGuardSetU0;
        } else if (quadInfo->stitchBottom) {
            ppGuardSetUBegin = ppGuardSetU1;
            if (quadInfo->vMaxSegs > 1) {
                ppGuardSetUEnd = ppGuardSetUInner;
                ppStitchUInner = ppGuardSetUInner;
            } else {
                ppGuardSetUEnd = ppGuardSetU1;
                ppStitchUInner = ppGuardSetU1;
            }
        } else {
            ppGuardSetUBegin = ppGuardSetU1;
            ppStitchUInner = NULL;
            ppGuardSetUEnd = ppGuardSetU0;
        }
    }

    if (quadInfo->uMaxDir == 1) {
        if (quadInfo->stitchRight) {
            ppGuardSetVBegin = ppGuardSetV0;
            if (quadInfo->uMaxSegs > 1) {
                ppGuardSetVEnd = ppGuardSetVInner;
                ppStitchVInner = ppGuardSetVInner;
            } else {
                ppGuardSetVEnd = ppGuardSetV0;
                ppStitchVInner = ppGuardSetV0;
            }
        } else if (quadInfo->stitchLeft) {
            if (quadInfo->uMaxSegs > 1) {
                ppGuardSetVBegin = ppGuardSetVInner;
                ppStitchVInner = ppGuardSetVInner;
            } else {
                ppGuardSetVBegin = ppGuardSetV1;
                ppStitchVInner = ppGuardSetV1;
            }
            ppGuardSetVEnd = ppGuardSetV1;
        } else {
            ppGuardSetVBegin = ppGuardSetV0;
            ppStitchVInner = NULL;
            ppGuardSetVEnd = ppGuardSetV1;
        }
    } else {
        if (quadInfo->stitchRight) {
            if (quadInfo->uMaxSegs > 1) {
                ppGuardSetVBegin = ppGuardSetVInner;
                ppStitchVInner = ppGuardSetVInner;
            } else {
                ppGuardSetVBegin = ppGuardSetV0;
                ppStitchVInner = ppGuardSetV0;
            }
            ppGuardSetVEnd = ppGuardSetV0;
        } else if (quadInfo->stitchLeft) {
            ppGuardSetVBegin = ppGuardSetV1;
            if (quadInfo->uMaxSegs > 1) {
                ppGuardSetVEnd = ppGuardSetVInner;
                ppStitchVInner = ppGuardSetVInner;
            } else {
                ppGuardSetVEnd = ppGuardSetV1;
                ppStitchVInner = ppGuardSetV1;
            }
        } else {
            ppGuardSetVBegin = ppGuardSetV1;
            ppStitchVInner = NULL;
            ppGuardSetVEnd = ppGuardSetV0;
        }
    }
    if (doFrac) {
        ppGuardSetUEnd = /*ppGuardSetUInnerFrac =*/ info->ppGuardSetUInnerFrac;
        ppGuardSetVEnd = /*ppGuardSetVInnerFrac =*/ info->ppGuardSetVInnerFrac;
    }
    // END SIMILAR CODE

    quadInfo->pCurvesTop = &curves1;
    quadInfo->pCurvesBot = &curves2;
    quadInfo->pOut1 = NULL;
    quadInfo->pOut2 = NULL;

    info->setGridCorner = NV_PATCH_SET_GRID_CORNER_ALL;

    if (info->swatchFlags & NV_PATCH_SWATCH) {
        retVal |= DrawIntPatchSwaths(info, quadInfo, matrices);

    } else {
        // set up ptrs for writing U curves from matrix
        quadInfo->pSwatchUBegin = ppGuardSetUBegin[0];
        quadInfo->pSwatchUEnd = ppGuardSetUEnd[0];
        quadInfo->pSwatchVBegin = ppGuardSetVBegin[0];
        quadInfo->pSwatchVEnd = ppGuardSetVEnd[0];

        for (index = 0; index < info->maxAttr; index++) {
            if (!EV_GUARD_ATTR(index)  &&  !doFrac) {
                continue;
            }
            if (!(info->evalEnables & (1 << index))) {
                continue;
            }
            info->pSwatchCorner[index][0][0] = info->gridCorner[index][1-quadInfo->vMaxDir][ 1-quadInfo->uMaxDir];
            FillIntCornerLL(info, quadInfo, quadInfo, index, info->pSwatchCorner[index][0][0]);

            info->pSwatchCorner[index][0][1] = info->gridCorner[index][1-quadInfo->vMaxDir][   quadInfo->uMaxDir];
            FillIntCornerLR(info, quadInfo, quadInfo, index, info->pSwatchCorner[index][0][1]);

            info->pSwatchCorner[index][1][0] = info->gridCorner[index][  quadInfo->vMaxDir][ 1-quadInfo->uMaxDir];
            FillIntCornerUL(info, quadInfo, quadInfo, index, info->pSwatchCorner[index][1][0]);

            info->pSwatchCorner[index][1][1] = info->gridCorner[index][  quadInfo->vMaxDir][   quadInfo->uMaxDir];
            FillIntCornerUR(info, quadInfo, quadInfo, index, info->pSwatchCorner[index][1][1]);

            // Copy U and V guard curves
            // note: be sure to do after Fill*() routines in case pSwatchV* curves are tweaked.
            CopyCurve(info, info->tempVBegin[index], (*quadInfo->pSwatchVBegin)[index]);
            CopyCurve(info, info->tempVEnd[index], (*quadInfo->pSwatchVEnd)[index]);
        }

        if (/* ff (info->flags & NV_PATCH_FLAG_AUTO_NORMAL)    || */
            (info->evalEnables & (1 << ATTR_N))) {
                info->swatchFlags |= NV_PATCH_HAS_NORMAL;
        };

        ComputeKelvinHOSControlWords(info, quadInfo, info->swatchFlags);

        quadInfo->uMaxSegs -= doFrac;
        quadInfo->vMaxSegs -= doFrac;
        retVal |= DrawIntPatchGrid(info, quadInfo, matrices);
        quadInfo->uMaxSegs += doFrac;
        quadInfo->vMaxSegs += doFrac;

        DrawAllPatchTransitions(info, quadInfo,
                            ppGuardSetU0, ppGuardSetU1, ppStitchUInner, info->ppGuardSetUInnerFrac,
                            ppGuardSetV0, ppGuardSetV1, ppStitchVInner, info->ppGuardSetVInnerFrac);

        HOS_NOTE("End Patch");
        pPush = pDevice->StartPush();
        Push1(pPush, NV097_SET_END_PATCH, 0);
        pDevice->EndPush(pPush + 2);
    }

drawIntPatchEnd:
    return retVal;
}

// Determine if a patch is rational (i.e. w != 1)
int IsPatchRational(NV_PATCH_INFO *info, int attrib)
{
    NV_PATCH_MAP_INFO *map = &info->maps[attrib];
    unsigned int i, j;

/*    switch(type) {
    case NV_PATCH_VERTEX_FORMAT_FLOAT_4:*/
        if (info->flags & NV_PATCH_FLAG_TYPE_TRIANGULAR) {
            for (j = 0; j < map->vorder; j++) {
                for (i = 0; i < map->uorder - j; i++) {
                    if (map->rawData[4*(j*map->uorder + i) + 3] != 1.0f) {
                        return 1;
                    }
                }
            }
        } else {
            for (j = 0; j < map->vorder; j++) {
                for (i = 0; i < map->uorder; i++) {
                    if (map->rawData[4*(j*map->uorder + i) + 3] != 1.0f) {
                        return 1;
                    }
                }
            }
        }
/*        break;
    case NV_PATCH_VERTEX_FORMAT_FLOAT_1:
    case NV_PATCH_VERTEX_FORMAT_FLOAT_2:
    case NV_PATCH_VERTEX_FORMAT_FLOAT_3:
        break;  // fall through we are guaranteed non-rational by our expansion methods
    default:
        nvAssert(0);
    }
*/
    return 0;
}

float BinomialCoefficient(int n, int r)
{
    float x;
    if(n<=10 && r <= 10){
        x = cachedBinomialCoefficients[n][r];
    }
    else {
        int i;
        x = 1.0f;
        for (i = 2; i <= n; i++) {
            x *= i;
        }
        for (i = 2; i <= r; i++) {
            x /= i;
        }
        for (i = 2; i <= n-r; i++) {
            x /= i;
        }
    }

    return x;
}

float TrinomialCoefficient(int n, int r, float s)
{
    float x = 1.0f;
    int i;

    for (i = 2; i <= n; i++) {
        x *= i;
    }
    for (i = 2; i <= r; i++) {
        x /= i;
    }
    for (i = 2; i <= s; i++) {
        x /= i;
    }
    for (i = 2; i <= n-r-s; i++) {
        x /= i;
    }
    return x;
}

void ComputeNormalPatch(NV_PATCH_INFO *info, float *normalPatch,
                                int uorder, int vorder, int rational)
{
    NV_PATCH_MAP_INFO *map = &info->maps[info->srcNormal];
    float *rawData = map->rawData;
    int pitch = map->pitch;
    int ufStride = 4;
    int vfStride = 4 * map->pitch;
    int i, j;
    FDMatrix *derivU, *derivV, *derivUTemp = NULL, *derivVTemp = NULL;
    FDMatrix *temp[6], *pFDMatrix;
    int normalUOrder, normalVOrder;
    int uRow, uCol, vRow, vCol;
    EV_ALLOC_DECL();

    EV_ALLOC_INIT_WITH_CACHE(info->context, 8 * sizeof(FDMatrix), computeNormalPatchEnd, info->pCache[EV_CACHE_9FDMATRICES]);
    EV_ALLOC(derivU, info->context, sizeof(FDMatrix));
    EV_ALLOC(derivV, info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[0], info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[1], info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[2], info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[3], info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[4], info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[5], info->context, sizeof(FDMatrix));
    EV_ALLOC_FULL(info->context);

    if (info->flags & NV_PATCH_FLAG_TYPE_TRIANGULAR) {
        if (rational) {
            normalUOrder = normalVOrder = 4*uorder - 5;
        } else {
            normalUOrder = normalVOrder = 2*uorder - 3;
        }

        // Compute derivatives in u
        derivU->columns = uorder-1;
        derivU->rows = uorder-1;
        for (j = 0; j < uorder-1; j++) {
            FillColumnVectorMatrix(uorder-j, ufStride*sizeof(float), pRawData(info, map, rawData, j, 0), temp[0], 0);
            for (i = 0; i < uorder-1-j; i++) {
                derivU->data[j][i][0] = temp[0]->data[i+1][0][0] - temp[0]->data[i][0][0];
                derivU->data[j][i][1] = temp[0]->data[i+1][0][1] - temp[0]->data[i][0][1];
                derivU->data[j][i][2] = temp[0]->data[i+1][0][2] - temp[0]->data[i][0][2];
                derivU->data[j][i][3] = temp[0]->data[i+1][0][3] - temp[0]->data[i][0][3];
            }
        }

        // Compute derivatives in v
        derivV->columns = uorder-1;
        derivV->rows = uorder-1;
        for (i = 0; i < uorder; i++) {
            FillColumnVectorMatrix(uorder-i, vfStride * sizeof(float), pRawData(info, map, rawData, 0, i), temp[0], 0);
            for (j = 0; j < uorder-1-i; j++) {
                derivV->data[j][i][0] = temp[0]->data[j+1][0][0] - temp[0]->data[j][0][0];
                derivV->data[j][i][1] = temp[0]->data[j+1][0][1] - temp[0]->data[j][0][1];
                derivV->data[j][i][2] = temp[0]->data[j+1][0][2] - temp[0]->data[j][0][2];
                derivV->data[j][i][3] = temp[0]->data[j+1][0][3] - temp[0]->data[j][0][3];
            }
        }

        if (rational) {
            int derivRow, derivCol, vertRow, vertCol;

            // This code makes the assumption that type == NV_PATCH_VERTEX_FORMAT_FLOAT_4.
            // Seeing as the patch is rational, this is probably safe, unless people start
            // using unsigned byte types for rational patches.

            derivUTemp = temp[2];
            derivVTemp = temp[3];

            // Initialize matrices
            derivUTemp->columns = 2*uorder - 2;
            derivUTemp->rows    = 2*uorder - 2;
            derivVTemp->columns = 2*uorder - 2;
            derivVTemp->rows    = 2*uorder - 2;
            for (derivRow = 0; derivRow < derivUTemp->rows; derivRow++) {
                for (derivCol = 0; derivCol < derivUTemp->columns; derivCol++) {
                    derivUTemp->data[derivRow][derivCol][0] = 0.0f;
                    derivUTemp->data[derivRow][derivCol][1] = 0.0f;
                    derivUTemp->data[derivRow][derivCol][2] = 0.0f;
                    derivUTemp->data[derivRow][derivCol][3] = 0.0f;
                }
            }
            for (derivRow = 0; derivRow < derivVTemp->rows; derivRow++) {
                for (derivCol = 0; derivCol < derivVTemp->columns; derivCol++) {
                    derivVTemp->data[derivRow][derivCol][0] = 0.0f;
                    derivVTemp->data[derivRow][derivCol][1] = 0.0f;
                    derivVTemp->data[derivRow][derivCol][2] = 0.0f;
                    derivVTemp->data[derivRow][derivCol][3] = 0.0f;
                }
            }

            // derivU <-- derivU.xyz * vertices.w - derivU.w * vertices.xyz
            for (derivRow = 0; derivRow < derivU->rows; derivRow++) {
                for (derivCol = 0; derivCol < derivU->columns-derivRow; derivCol++) {
                    float derivPoint[4];

                    derivPoint[0] = derivU->data[derivRow][derivCol][0];
                    derivPoint[1] = derivU->data[derivRow][derivCol][1];
                    derivPoint[2] = derivU->data[derivRow][derivCol][2];
                    derivPoint[3] = derivU->data[derivRow][derivCol][3];

                    for (vertRow = 0; vertRow < vorder; vertRow++) {
                        for (vertCol = 0; vertCol < uorder-vertRow; vertCol++) {
                            float vert[4], *result;
                            float factor;

                            vert[0] = rawData[4*(vertRow*pitch + vertCol) + 0];
                            vert[1] = rawData[4*(vertRow*pitch + vertCol) + 1];
                            vert[2] = rawData[4*(vertRow*pitch + vertCol) + 2];
                            vert[3] = rawData[4*(vertRow*pitch + vertCol) + 3];

                            factor  = TrinomialCoefficient(derivU->rows-1, derivRow, derivCol);
                            factor *= TrinomialCoefficient(uorder-1, vertRow, vertCol);
                            factor /= TrinomialCoefficient(derivU->rows+uorder-2, derivRow+vertRow, derivCol+vertCol);

                            result = &derivUTemp->data[derivRow+vertRow][derivCol+vertCol][0];

                            // Accumulate
                            result[0] += factor*(derivPoint[0]*vert[3] - derivPoint[3]*vert[0]);
                            result[1] += factor*(derivPoint[1]*vert[3] - derivPoint[3]*vert[1]);
                            result[2] += factor*(derivPoint[2]*vert[3] - derivPoint[3]*vert[2]);
                        }
                    }
                }
            }

            // derivV <-- derivV.xyz * vertices.w - derivV.w * vertices.xyz
            for (derivRow = 0; derivRow < derivV->rows; derivRow++) {
                for (derivCol = 0; derivCol < derivV->columns-derivRow; derivCol++) {
                    float derivPoint[4];

                    derivPoint[0] = derivV->data[derivRow][derivCol][0];
                    derivPoint[1] = derivV->data[derivRow][derivCol][1];
                    derivPoint[2] = derivV->data[derivRow][derivCol][2];
                    derivPoint[3] = derivV->data[derivRow][derivCol][3];

                    for (vertRow = 0; vertRow < vorder; vertRow++) {
                        for (vertCol = 0; vertCol < uorder-vertRow; vertCol++) {
                            float vert[4], *result;
                            float factor;

                            vert[0] = rawData[4*(vertRow*pitch + vertCol) + 0];
                            vert[1] = rawData[4*(vertRow*pitch + vertCol) + 1];
                            vert[2] = rawData[4*(vertRow*pitch + vertCol) + 2];
                            vert[3] = rawData[4*(vertRow*pitch + vertCol) + 3];

                            factor  = TrinomialCoefficient(derivV->rows-1, derivRow, derivCol);
                            factor *= TrinomialCoefficient(uorder-1, vertRow, vertCol);
                            factor /= TrinomialCoefficient(derivV->rows+uorder-2, derivRow+vertRow, derivCol+vertCol);

                            result = &derivVTemp->data[derivRow+vertRow][derivCol+vertCol][0];

                            // Accumulate
                            result[0] += factor*(derivPoint[0]*vert[3] - derivPoint[3]*vert[0]);
                            result[1] += factor*(derivPoint[1]*vert[3] - derivPoint[3]*vert[1]);
                            result[2] += factor*(derivPoint[2]*vert[3] - derivPoint[3]*vert[2]);
                        }
                    }
                }
            }

            EV_SWAP(derivU, temp[2], pFDMatrix);
            EV_SWAP(derivV, temp[3], pFDMatrix);
        }

        for (uRow = 0; uRow < derivU->rows; uRow++) {
            for (uCol = 0; uCol < derivU->columns-uRow; uCol++) {
                float uPoint[3];

                uPoint[0] = derivU->data[uRow][uCol][0];
                uPoint[1] = derivU->data[uRow][uCol][1];
                uPoint[2] = derivU->data[uRow][uCol][2];

                for (vRow = 0; vRow < derivV->rows; vRow++) {
                    for (vCol = 0; vCol < derivV->columns-vRow; vCol++) {
                        float vPoint[3];
                        int normalIndex = 4*(normalUOrder*(uRow+vRow) + uCol + vCol);
                        float factor;

                        vPoint[0] = derivV->data[vRow][vCol][0];
                        vPoint[1] = derivV->data[vRow][vCol][1];
                        vPoint[2] = derivV->data[vRow][vCol][2];

                        factor  = TrinomialCoefficient(derivU->rows-1, uRow, uCol);
                        factor *= TrinomialCoefficient(derivV->rows-1, vRow, vCol);
                        factor /= TrinomialCoefficient(derivU->rows+derivV->rows-2, uRow+vRow, uCol+vCol);

                        // Accumulate x: uy*vz - uz*vy
                        normalPatch[normalIndex + 0] += factor*(uPoint[1]*vPoint[2] - uPoint[2]*vPoint[1]);
                        // Accumulate y: uz*vx - ux*vz
                        normalPatch[normalIndex + 1] += factor*(uPoint[2]*vPoint[0] - uPoint[0]*vPoint[2]);
                        // Accumulate z: ux*vy - uy*vx
                        normalPatch[normalIndex + 2] += factor*(uPoint[0]*vPoint[1] - uPoint[1]*vPoint[0]);
                    }
                }
            }
        }
    } else {
        if (rational) {
            normalUOrder = 4*uorder - 4;
            normalVOrder = 4*vorder - 4;
        } else {
            normalUOrder = 2*uorder - 2;
            normalVOrder = 2*vorder - 2;
        }

        // Compute derivatives in u
        derivU->columns = uorder-1;
        derivU->rows = vorder;
        for (j = 0; j < vorder; j++) {
            FillColumnVectorMatrix(uorder, ufStride*sizeof(float), pRawData(info, map, rawData, j, 0), temp[0], 0);
            for (i = 0; i < uorder-1; i++) {
                derivU->data[j][i][0] = temp[0]->data[i+1][0][0] - temp[0]->data[i][0][0];
                derivU->data[j][i][1] = temp[0]->data[i+1][0][1] - temp[0]->data[i][0][1];
                derivU->data[j][i][2] = temp[0]->data[i+1][0][2] - temp[0]->data[i][0][2];
                derivU->data[j][i][3] = temp[0]->data[i+1][0][3] - temp[0]->data[i][0][3];
            }
        }

        // Compute derivatives in v
        derivV->columns = uorder;
        derivV->rows = vorder-1;
        for (i = 0; i < uorder; i++) {
            FillColumnVectorMatrix(vorder, vfStride * sizeof(float), pRawData(info, map, rawData, 0, i), temp[0], 0);
            for (j = 0; j < vorder-1; j++) {
                derivV->data[j][i][0] = temp[0]->data[j+1][0][0] - temp[0]->data[j][0][0];
                derivV->data[j][i][1] = temp[0]->data[j+1][0][1] - temp[0]->data[j][0][1];
                derivV->data[j][i][2] = temp[0]->data[j+1][0][2] - temp[0]->data[j][0][2];
                derivV->data[j][i][3] = temp[0]->data[j+1][0][3] - temp[0]->data[j][0][3];
            }
        }

        if (rational) {
            int derivRow, derivCol, vertRow, vertCol;

            // This code makes the assumption that type == NV_PATCH_VERTEX_FORMAT_FLOAT_4.
            // Seeing as the patch is rational, this is probably safe, unless people start
            // using unsigned byte types for rational patches.

            derivUTemp = temp[4];
            derivVTemp = temp[5];

            // Initialize matrices
            derivUTemp->columns = 2*uorder - 2;
            derivUTemp->rows    = 2*vorder - 1;
            derivVTemp->columns = 2*uorder - 1;
            derivVTemp->rows    = 2*vorder - 2;
            for (derivRow = 0; derivRow < derivUTemp->rows; derivRow++) {
                for (derivCol = 0; derivCol < derivUTemp->columns; derivCol++) {
                    derivUTemp->data[derivRow][derivCol][0] = 0.0f;
                    derivUTemp->data[derivRow][derivCol][1] = 0.0f;
                    derivUTemp->data[derivRow][derivCol][2] = 0.0f;
                    derivUTemp->data[derivRow][derivCol][3] = 0.0f;
                }
            }
            for (derivRow = 0; derivRow < derivVTemp->rows; derivRow++) {
                for (derivCol = 0; derivCol < derivVTemp->columns; derivCol++) {
                    derivVTemp->data[derivRow][derivCol][0] = 0.0f;
                    derivVTemp->data[derivRow][derivCol][1] = 0.0f;
                    derivVTemp->data[derivRow][derivCol][2] = 0.0f;
                    derivVTemp->data[derivRow][derivCol][3] = 0.0f;
                }
            }

            // derivU <-- derivU.xyz * vertices.w - derivU.w * vertices.xyz
            for (derivRow = 0; derivRow < derivU->rows; derivRow++) {
                for (derivCol = 0; derivCol < derivU->columns; derivCol++) {
                    float derivPoint[4];

                    derivPoint[0] = derivU->data[derivRow][derivCol][0];
                    derivPoint[1] = derivU->data[derivRow][derivCol][1];
                    derivPoint[2] = derivU->data[derivRow][derivCol][2];
                    derivPoint[3] = derivU->data[derivRow][derivCol][3];

                    for (vertRow = 0; vertRow < vorder; vertRow++) {
                        for (vertCol = 0; vertCol < uorder; vertCol++) {
                            float vert[4], *result;
                            float factor;
                            int idx;
                            idx = 4*(vertRow*pitch + vertCol);
                            vert[0] = rawData[idx + 0];
                            vert[1] = rawData[idx + 1];
                            vert[2] = rawData[idx + 2];
                            vert[3] = rawData[idx + 3];

                            factor  = BinomialCoefficient(derivU->rows-1, derivRow);
                            factor /= BinomialCoefficient(derivU->rows+vorder-2, derivRow+vertRow);
                            factor *= BinomialCoefficient(derivU->columns-1, derivCol);
                            factor *= BinomialCoefficient(vorder-1, vertRow);
                            factor /= BinomialCoefficient(derivU->columns+uorder-2, derivCol+vertCol);
                            factor *= BinomialCoefficient(uorder-1, vertCol);

                            result = &derivUTemp->data[derivRow+vertRow][derivCol+vertCol][0];

                            // Accumulate
                            result[0] += factor*(derivPoint[0]*vert[3] - derivPoint[3]*vert[0]);
                            result[1] += factor*(derivPoint[1]*vert[3] - derivPoint[3]*vert[1]);
                            result[2] += factor*(derivPoint[2]*vert[3] - derivPoint[3]*vert[2]);
                        }
                    }
                }
            }

            // derivV <-- derivV.xyz * vertices.w - derivV.w * vertices.xyz
            for (derivRow = 0; derivRow < derivV->rows; derivRow++) {
                for (derivCol = 0; derivCol < derivV->columns; derivCol++) {
                    float derivPoint[4];

                    derivPoint[0] = derivV->data[derivRow][derivCol][0];
                    derivPoint[1] = derivV->data[derivRow][derivCol][1];
                    derivPoint[2] = derivV->data[derivRow][derivCol][2];
                    derivPoint[3] = derivV->data[derivRow][derivCol][3];

                    for (vertRow = 0; vertRow < vorder; vertRow++) {
                        for (vertCol = 0; vertCol < uorder; vertCol++) {
                            float vert[4], *result;
                            float factor;

                            vert[0] = rawData[4*(vertRow*pitch + vertCol) + 0];
                            vert[1] = rawData[4*(vertRow*pitch + vertCol) + 1];
                            vert[2] = rawData[4*(vertRow*pitch + vertCol) + 2];
                            vert[3] = rawData[4*(vertRow*pitch + vertCol) + 3];

                            factor  = BinomialCoefficient(derivV->rows-1, derivRow);
                            factor *= BinomialCoefficient(derivV->columns-1, derivCol);
                            factor *= BinomialCoefficient(vorder-1, vertRow);
                            factor *= BinomialCoefficient(uorder-1, vertCol);
                            factor /= BinomialCoefficient(derivV->rows+vorder-2, derivRow+vertRow);
                            factor /= BinomialCoefficient(derivV->columns+uorder-2, derivCol+vertCol);

                            result = &derivVTemp->data[derivRow+vertRow][derivCol+vertCol][0];

                            // Accumulate
                            result[0] += factor*(derivPoint[0]*vert[3] - derivPoint[3]*vert[0]);
                            result[1] += factor*(derivPoint[1]*vert[3] - derivPoint[3]*vert[1]);
                            result[2] += factor*(derivPoint[2]*vert[3] - derivPoint[3]*vert[2]);
                        }
                    }
                }
            }

            EV_SWAP(derivU, temp[4], pFDMatrix);
            EV_SWAP(derivV, temp[5], pFDMatrix);
        }

        for (uRow = 0; uRow < derivU->rows; uRow++) {
            for (uCol = 0; uCol < derivU->columns; uCol++) {
                float uPoint[3];

                uPoint[0] = derivU->data[uRow][uCol][0];
                uPoint[1] = derivU->data[uRow][uCol][1];
                uPoint[2] = derivU->data[uRow][uCol][2];

                for (vRow = 0; vRow < derivV->rows; vRow++) {
                    for (vCol = 0; vCol < derivV->columns; vCol++) {
                        float vPoint[3];
                        int normalIndex = 4*(normalUOrder*(uRow+vRow) + uCol+vCol);
                        float factor;

                        vPoint[0] = derivV->data[vRow][vCol][0];
                        vPoint[1] = derivV->data[vRow][vCol][1];
                        vPoint[2] = derivV->data[vRow][vCol][2];

                        factor  = BinomialCoefficient(derivU->rows-1, uRow);
                        factor *= BinomialCoefficient(derivV->rows-1, vRow);
                        factor *= BinomialCoefficient(derivU->columns-1, uCol);
                        factor *= BinomialCoefficient(derivV->columns-1, vCol);
                        factor /= BinomialCoefficient(derivU->rows+derivV->rows-2, uRow+vRow);
                        factor /= BinomialCoefficient(derivU->columns+derivV->columns-2, uCol+vCol);

                        // Accumulate x: uy*vz - uz*vy
                        normalPatch[normalIndex + 0] += factor*(uPoint[1]*vPoint[2] - uPoint[2]*vPoint[1]);

                        // Accumulate y: uz*vx - ux*vz
                        normalPatch[normalIndex + 1] += factor*(uPoint[2]*vPoint[0] - uPoint[0]*vPoint[2]);

                        // Accumulate z: ux*vy - uy*vx
                        normalPatch[normalIndex + 2] += factor*(uPoint[0]*vPoint[1] - uPoint[1]*vPoint[0]);
                    }
                }
            }
        }
    }

computeNormalPatchEnd:
    EV_FREE(derivU, info->context);
    EV_FREE(derivU, info->context);
    EV_FREE(temp[0], info->context);
    EV_FREE(temp[1], info->context);
    EV_FREE(temp[2], info->context);
    EV_FREE(temp[3], info->context);
    EV_FREE(temp[4], info->context);
    EV_FREE(temp[5], info->context);
}

const struct {
    int uSide;
    int vSide;
    int uDir;
    int vDir;
    int reverse;
    int centerCurve;
} TriPatchLayout[3] = {
    { 0, 2, 0, 1, 0, 0 },
    { 0, 1, 1, 0, 1, 1 },
    { 1, 2, 1, 0, 1, 1 },
};

// Compute the step size and number of segments in fractional tessellation
void ComputeFracStepInfo(float steps, float *du, int *segs)
{
    *du = 1.0f / steps;
    if (segs) {
        *segs = (int)(steps/2.0f + 0.999f);
    }
}

/*****************************************************************************/
// needs adjustment for 64-bit machines
#define SIZEOF_NV_PATCH_QUAD_INFO EV_ROUND(sizeof(NV_PATCH_QUAD_INFO))

int nvAllocInfoScratch(NV_PATCH_INFO *info)
{
    unsigned int retVal = NV_PATCH_EVAL_OK;
    int doFrac = !!(info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL);
    int attr, bytes;
    EV_ALLOC_DECL();

    bytes = info->nAttr * (sizeof(FDMatrix) + 2*info->bytesGuardCurve);

    if (doFrac) {
        bytes += info->nAttr * (2*info->bytesGuardCurve);
    }

    EV_ALLOC_INIT_WITH_CACHE(info->context, bytes, nvAllocInfoScratchEnd, info->pCache[EV_CACHE_INFO_SCRATCH]);
    info->pScratchBase = evMemory.pBase;

    for (attr = 0; attr < info->maxAttr; attr++){
        if (!(info->evalEnables & (1 << attr))) {
            continue;
        }

        EV_ALLOC(info->ppMatrixSetSS0[attr], info->context, sizeof(FDMatrix));

        // for guard curves at begin or end of matrix stepping
        EV_ALLOC(info->tempVBegin[attr], info->context, info->bytesGuardCurve);
        EV_ALLOC(info->tempVEnd[attr], info->context, info->bytesGuardCurve);

        if (doFrac) {
            EV_ALLOC(info->guardSetUInnerFrac[attr], info->context, info->bytesGuardCurve);
            EV_ALLOC(info->guardSetVInnerFrac[attr], info->context, info->bytesGuardCurve);
            continue;
        }
    }
    EV_ALLOC_FULL(info->context);

    info->ppGuardSetUInnerFrac1[0] = &info->guardSetUInnerFrac;
    info->ppGuardSetVInnerFrac1[0] = &info->guardSetVInnerFrac;

    info->ppGuardSetUInnerFrac = &info->ppGuardSetUInnerFrac1[0];
    info->ppGuardSetVInnerFrac = &info->ppGuardSetVInnerFrac1[0];

nvAllocInfoScratchEnd:
    return retVal;
}

// alloc FDMatrices and other things for imm mode rendering
int nvAllocFDMatricesEtc(NV_PATCH_INFO *info)
{
    unsigned int retVal = NV_PATCH_EVAL_OK;
    int doFrac = !!(info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL);
    int attr, matrixBytes, totalBytes;
    NV_PATCH_QUAD_INFO *quadInfo;
    EV_ALLOC_DECL();

    // all styles need quadInfo
    totalBytes = SIZEOF_NV_PATCH_QUAD_INFO;

    if (info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL) {
        if (info->flags & NV_PATCH_FLAG_TYPE_TRIANGULAR) {
            matrixBytes = 3*sizeof(FDMatrix) + sizeof(NV_PATCH_FRAC_TRI_GUARD_INFO);
        } else {
            matrixBytes = 4*sizeof(FDMatrix) + sizeof(NV_PATCH_FRAC_QUAD_GUARD_INFO);
        }
    } else {
        matrixBytes = 1*sizeof(FDMatrix);
        totalBytes += 4 * info->bytesGuardCurve * info->nAttr;
    }
    matrixBytes = EV_ROUND(matrixBytes);

    // always alloc Inner guard curves
    totalBytes += 2 * info->bytesGuardCurve * info->nAttr;

    // all styles need matrices and guardInfo
    totalBytes += matrixBytes * info->nAttr;

    EV_ALLOC_INIT_WITH_CACHE(info->context, totalBytes, nvAllocFDMatricesEnd, info->pCache[EV_CACHE_QUADINFO_ETC]);

    //THIS MUST ALWAYS BE THE FIRST ALLOCATION FROM THIS GROUP!!!!!!!!!
    EV_ALLOC(info->quadInfo, info->context, SIZEOF_NV_PATCH_QUAD_INFO);
    quadInfo = info->quadInfo;

    for (attr = 0; attr < info->maxAttr; attr++) {
        if (!(info->evalEnables & (1 << attr))) {
            continue;
        }

        quadInfo->m00[attr] = quadInfo->m01[attr] = quadInfo->m10[attr] = quadInfo->m11[attr] = NULL;
        EV_ALLOC(quadInfo->m00[attr], info->context, matrixBytes);

        // alloc Inner for all cases
        EV_ALLOC(quadInfo->pUInner[attr], info->context, info->bytesGuardCurve);
        EV_ALLOC(quadInfo->pVInner[attr], info->context, info->bytesGuardCurve);

        if (info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL) {
            quadInfo->m01[attr] = &quadInfo->m00[attr][1];
            quadInfo->m10[attr] = &quadInfo->m00[attr][2];
            if (info->flags & NV_PATCH_FLAG_TYPE_TRIANGULAR) {
                quadInfo->guardTF[attr] = (NV_PATCH_FRAC_TRI_GUARD_INFO *)&quadInfo->m00[attr][3];
            } else {
                quadInfo->m11[attr] = &quadInfo->m00[attr][3];
                quadInfo->guardQF[attr] = (NV_PATCH_FRAC_QUAD_GUARD_INFO *)&quadInfo->m00[attr][4];
            }
            continue;
        }

        // INT cases
        EV_ALLOC(quadInfo->pU0[attr], info->context, info->bytesGuardCurve);
        EV_ALLOC(quadInfo->pU1[attr], info->context, info->bytesGuardCurve);
        EV_ALLOC(quadInfo->pV0[attr], info->context, info->bytesGuardCurve);
        EV_ALLOC(quadInfo->pV1[attr], info->context, info->bytesGuardCurve);

        quadInfo->pCorners[attr][0][0] = quadInfo->cornAttr[attr].corners[0][0];
        quadInfo->pCorners[attr][0][1] = quadInfo->cornAttr[attr].corners[0][1];
        quadInfo->pCorners[attr][1][0] = quadInfo->cornAttr[attr].corners[1][0];
        quadInfo->pCorners[attr][1][1] = quadInfo->cornAttr[attr].corners[1][1];
    }
    EV_ALLOC_FULL(info->context);

nvAllocFDMatricesEnd:
    return retVal;
}

/*****************************************************************************/
int bFirstTimeInit = 0x1;              //global value to indicate if we've built dynamic inner loops yet or not.
float fracFudge = .01f;
unsigned int nvEvalPatchSetup(NV_PATCH_INFO *info)
{
    int ii;
    unsigned int retVal = NV_PATCH_EVAL_OK;
    float *normalPatch = NULL;
    float *UVPatch = NULL;
    float segmin = 1.f;
    EV_ALLOC_DECL();

    info->reverse = 0;

    //INITIALIZE HERE BECAUSE Compute Normal Patch will read these.
    info->flipT = 0;
    info->flipU = 0;
    info->flipV = 0;
    info->flipTUV = 0;
    info->flipUV = 0;
    info->nAttr = 0;
    info->maxAttr = -1;
    info->maxOrder = 0;
    info->firstAttr = -1;

    if (info->flags & NV_PATCH_FLAG_TYPE_TRIANGULAR) {
        // enforce minimums as required
        if (info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL) { segmin = 2.f + fracFudge; }
        info->tess.tri.n1 = MAX(info->tess.tri.n1, segmin);
        info->tess.tri.n2 = MAX(info->tess.tri.n2, segmin);
        info->tess.tri.n3 = MAX(info->tess.tri.n3, segmin);

    } else {
        // enforce minimums as required
        if (info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL) {
            segmin = 2.f;
            if ((info->tess.tensor.nu0 == 2.f  &&  info->tess.tensor.nu1 == 2.f)
                    ||  (info->tess.tensor.nv0 == 2.f  &&  info->tess.tensor.nv1 == 2.f)) {
                segmin += fracFudge;
            }
        }
        info->tess.tensor.nu0 = MAX(info->tess.tensor.nu0, segmin);
        info->tess.tensor.nu1 = MAX(info->tess.tensor.nu1, segmin);
        info->tess.tensor.nv0 = MAX(info->tess.tensor.nv0, segmin);
        info->tess.tensor.nv1 = MAX(info->tess.tensor.nv1, segmin);
    }

    nvAssert(16 == NV_PATCH_NUMBER_OF_ATTRIBS); // fix all 16's if we hit this... fundamental assumption has changed

    if (info->flags & NV_PATCH_FLAG_AUTO_NORMAL) {
        NV_PATCH_MAP_INFO *map;
        int uorder, vorder;
        int normalUOrder, normalVOrder;

        // init for ComputeNormalPatch
        map = &info->maps[info->srcNormal];
        uorder = map->uorder;
        vorder = map->vorder;
        map->ufStride = 4;
        map->vfStride = 4 * map->pitch;

        nvAssert(info->srcNormal >= 0 && info->srcNormal <= NV_PATCH_NUMBER_OF_ATTRIBS);
        nvAssert(info->dstNormal >= 0 && info->dstNormal <= NV_PATCH_NUMBER_OF_ATTRIBS);
        if (info->maps[info->srcNormal].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
            info->rational = IsPatchRational(info, info->srcNormal);
        }

        if (info->flags & NV_PATCH_FLAG_TYPE_TRIANGULAR) {
            if (info->rational) {
                normalUOrder = normalVOrder = 4*uorder - 5;
            } else {
                normalUOrder = normalVOrder = 2*uorder - 3;
            }
        } else {
            if (info->rational) {
                normalUOrder = 4*uorder - 4;
                normalVOrder = 4*vorder - 4;
            } else {
                normalUOrder = 2*uorder - 2;
                normalVOrder = 2*vorder - 2;
            }
        }

        // If control points are dirty, recalculate the actual normal patch
        if (info->maps[info->srcNormal].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
            int bytes;

            //init some storage
            if (normalUOrder == 1) {
                //degree lifting -- HW doesn't allow for a order1/degree0/constant function in the FD unit,
                //so allocate extra space here so I can lift it to linear later
                bytes = 4*(normalUOrder+1)*(normalVOrder+1)*sizeof(float);
            } else {
                bytes = 4*normalUOrder*normalVOrder*sizeof(float);
            }
            EV_ALLOC_INIT_WITH_CACHE(info->context, bytes, nvPatchSetupEnd, info->pCache[EV_CACHE_AUTO_NORMAL_SETUP]);
            EV_ALLOC(info->normalPatch, info->context, bytes);
            normalPatch = info->normalPatch;
            for (ii = 0; ii < 4*normalUOrder*normalVOrder; ii++) {
                if ((ii & 3) != 3) {
                    normalPatch[ii] = 0.0f;
                } else {
                    normalPatch[ii] = 1.0f;
                }
            }

            // XXXmjc We aren't handling constant position in one direction properly yet
            nvAssert(uorder > 1);
            nvAssert(vorder > 1);
            ComputeNormalPatch(info, normalPatch, uorder, vorder, info->rational); //type passed here is the data src type
            if (normalUOrder == 1) {
                float pt[4];
                //degree lifting -- HW doesn't allow for a order1/degree0/constant function in the FD unit,
                //expand the normal patch data here.
                pt[0] = normalPatch[0]; pt[1] = normalPatch[1]; pt[2] = normalPatch[2];
                for (ii=4; ii < 12; ii+=4) {
                    normalPatch[ii+0] = pt[0];
                    normalPatch[ii+1] = pt[1];
                    normalPatch[ii+2] = pt[2];
                    normalPatch[ii+3] = 1.f;
                }
            }
        }

        if (normalUOrder == 1) {
            //if the order is actually 1 then bump it up one because I lifted the degree
            //when the control pts were dirty.
            normalUOrder++;
            normalVOrder++;
        }

        // Force evaluation of the normal
        info->evalEnables |= (1 << info->dstNormal);
        info->maps[info->dstNormal].uorder = normalUOrder;
        info->maps[info->dstNormal].vorder = normalVOrder;
        info->maps[info->dstNormal].rawData = normalPatch;
        //ffinfo->maps[info->dstNormal].stride = 4*sizeof(float);
        info->maps[info->dstNormal].pitch  = normalUOrder;
    }

    if (info->flags & NV_PATCH_FLAG_AUTO_UV) {
        int order = 2;
        int bytes = 0;
        for (ii=0; ii < 8; ii++) {
            if (info->dstUV[ii] != 0xFFFFffff) {
                if (info->maps[info->dstUV[ii]].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
                    if(bytes == 0){
                        //if this is the first UV coord build patch data for it
                        bytes = 4*order*order*sizeof(float);
                        EV_ALLOC_INIT_WITH_CACHE2(info->context, bytes, nvPatchSetupEnd, info->pCache[EV_CACHE_UV_SETUP]);
                        EV_ALLOC(UVPatch , info->context, bytes);
                        if (info->flags & NV_PATCH_FLAG_TYPE_TRIANGULAR) {
                            // DO THIS BECAUSE THERE IS AN IMPLICIT MIRROR IN THE
                            // FORMATS OF D3D/OGL        D3D= 0      OGL=2
                            //                                |\         |\
                            //                                1-2        0-1
                            // texture coordinates need to be mirrored in y.
                            // likewise winding needs to be inverted
                            UVPatch[0] = info->endu;   UVPatch[1] = info->endv;     UVPatch[2] = 0.f;   UVPatch[3] = 1.f;
                            UVPatch[4] = info->startu; UVPatch[5] = info->endv;     UVPatch[6] = 0.f;   UVPatch[7] = 1.f;
                            UVPatch[8] = info->startu; UVPatch[9] = info->startv;   UVPatch[10] = 0.f;  UVPatch[11] = 1.f;
                            UVPatch[12] = 0x0;         UVPatch[13] = 0x0;           UVPatch[14] = 0.f;  UVPatch[15] = 1.f;
                        } else {
                            UVPatch[0] = info->startu; UVPatch[1] = info->startv;   UVPatch[2] = 0.f;   UVPatch[3] = 1.f;
                            UVPatch[4] = info->endu;   UVPatch[5] = info->startv;   UVPatch[6] = 0.f;   UVPatch[7] = 1.f;
                            UVPatch[8] = info->startu; UVPatch[9] = info->endv;     UVPatch[10] = 0.f;  UVPatch[11] = 1.f;
                            UVPatch[12] = info->endu;   UVPatch[13] = info->endv;   UVPatch[14] = 0.f;  UVPatch[15] = 1.f;
                        }
                    }
                    info->UVPatch = UVPatch;
                }

                // Force evaluation of the texturecoord
                info->evalEnables |= (1 << info->dstUV[ii]);
                info->maps[info->dstUV[ii]].uorder = info->maps[info->dstUV[ii]].vorder = order;
                info->maps[info->dstUV[ii]].rawData = UVPatch;
                //ffinfo->maps[info->dstUV[ii]].stride = 4*sizeof(float);
                info->maps[info->dstUV[ii]].pitch = order;
            }
        }
    }

    // ensure some flags are initialized
    info->flags &= ~(NV_PATCH_FLAG_SOME_DIRTY_TESS | NV_PATCH_FLAG_TRANSPOSE
                        | NV_PATCH_FLAG_FLIPPED_U | NV_PATCH_FLAG_FLIPPED_V);
    for (ii = 0;  ii < NV_PATCH_NUMBER_OF_ATTRIBS;  ii++) {
        if (info->evalEnables & (1 << ii)) {
            if (!(info->maps[ii].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS)) {
                if (info->maps[ii].dirtyFlags & NV_PATCH_DIRTY_TESSELLATION) {
                    info->flags |= NV_PATCH_FLAG_SOME_DIRTY_TESS;
                }
            }

            info->maps[ii].ufStride = 4;
            info->maps[ii].vfStride = 4 * info->maps[ii].pitch;

            // calc MAX values
            info->nAttr++;
            if (info->firstAttr == -1) {
                info->firstAttr = ii;
            }
            if (ii > info->maxAttr) {
                info->maxAttr = ii;
            }
            if (info->maps[ii].uorder > info->maxOrder) {
                info->maxOrder = info->maps[ii].uorder;
            }
            if (info->maps[ii].vorder > info->maxOrder) {
                info->maxOrder = info->maps[ii].vorder;
            }
        }
    }
    info->maxAttr = info->maxAttr + 1;

    info->bytesGuardCurve = EV_ROUND(PATCH_CURVE_HEADER_SIZE + (info->maxOrder * 4) * sizeof(float)); // truncated NV_PATCH_CURVE_INFO
    info->bytesGuardCurveAllAttr = info->nAttr * info->bytesGuardCurve;                   // just the set of curve info

    if (!(info->flags & NV_PATCH_FLAG_TYPE_TRIANGULAR)) {
        // setup QUADRILATERAL PATCH

        if (info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL) {
        } else {    // setup QUADRILATERAL INTEGER TESSELLATION
        }

    } else {
        // setup TRIANGULAR PATCH
        if (info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL) {
        } else {    // setup TRIANGULAR INTEGER TESSELLATION
        }
    }

    return retVal;

nvPatchSetupEnd:
    nvEvalPatchCleanup(info);
    return retVal;
}

unsigned int nvEvalPatchCleanup(NV_PATCH_INFO *info)
{
    int ii;

    if (info->normalPatch && !info->pCache[EV_CACHE_AUTO_NORMAL_SETUP]) {
        MemFree(info->normalPatch);
        info->normalPatch = NULL;
    }
    if (info->UVPatch && !info->pCache[EV_CACHE_UV_SETUP]) {
        MemFree(info->UVPatch);
        info->UVPatch = NULL;
    }

    // indicate that all cache's are not in use
    for (ii = 0;  ii < MAX_EV_CACHE;  ii++) {
        if (info->pCache[ii]) {
            info->pCache[ii]->inUse = 0;
        }
    }

    setFrontFace(info, 0);
    return info->retVal;
}

/*****************************************************************************/
int noflip = 0;

int MaybeTransposeFlipQuad(NV_PATCH_INFO *info)
{
    NV_PATCH_QUAD_INFO *quadInfo = info->quadInfo;
    int doFrac = !!(info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL);
    int index, ii;
    int stitchLeft;
    int stitchBottom;
    float fnuMax, fnvMax, ftemp;

    quadInfo->nu0 = (int)(info->tess.tensor.nu0 + 0.999f);
    quadInfo->nu1 = (int)(info->tess.tensor.nu1 + 0.999f);
    quadInfo->nv0 = (int)(info->tess.tensor.nv0 + 0.999f);
    quadInfo->nv1 = (int)(info->tess.tensor.nv1 + 0.999f);
    if (!doFrac) {
        fnuMax = MAX(quadInfo->nu0, quadInfo->nu1);
        fnvMax = MAX(quadInfo->nv0, quadInfo->nv1);
    } else {
        fnuMax = (info->tess.tensor.nu0 + info->tess.tensor.nu1) / 2.0f;
        fnvMax = (info->tess.tensor.nv0 + info->tess.tensor.nv1) / 2.0f;
    }

if (noflip) {
    quadInfo->uMaxSegs = MAX(quadInfo->nu0, quadInfo->nu1);
    quadInfo->vMaxSegs = MAX(quadInfo->nv0, quadInfo->nv1);
    quadInfo->uMinSegs = MIN(quadInfo->nu0, quadInfo->nu1);
    quadInfo->vMinSegs = MIN(quadInfo->nv0, quadInfo->nv1);

    return 1;
}
    if (fnvMax > fnuMax) {
        info->flipT = 1;
        info->flags |= NV_PATCH_FLAG_TRANSPOSE;
        EV_SWAP(quadInfo->nu0, quadInfo->nv0, ii);
        EV_SWAP(quadInfo->nu1, quadInfo->nv1, ii);
        EV_SWAP(info->tess.tensor.nu0, info->tess.tensor.nv0, ftemp);
        EV_SWAP(info->tess.tensor.nu1, info->tess.tensor.nv1, ftemp);
        for (index = 0; index < info->maxAttr; index++) {
            if (info->evalEnables & (1 << index)) {
                EV_SWAP(info->maps[index].uorder, info->maps[index].vorder, ii);
                EV_SWAP(info->maps[index].vfStride, info->maps[index].ufStride, ii);
            }
        }
    }

    stitchLeft = (quadInfo->nv0 < quadInfo->nv1);
    if (stitchLeft  &&  !doFrac) {
        info->flags |= NV_PATCH_FLAG_FLIPPED_U;
        info->flipU = 1;
        EV_SWAP(quadInfo->nv0, quadInfo->nv1, ii);
        EV_SWAP(info->tess.tensor.nv0, info->tess.tensor.nv1, ftemp);
    }

    stitchBottom = (quadInfo->nu0 < quadInfo->nu1);
    if (stitchBottom  &&  !doFrac) {
        info->flags |= NV_PATCH_FLAG_FLIPPED_V;
        info->flipV = 1;
        EV_SWAP(quadInfo->nu0, quadInfo->nu1, ii);
        EV_SWAP(info->tess.tensor.nu0, info->tess.tensor.nu1, ftemp);
    }

    info->flipUV = info->flipU ^ info->flipV;
    info->flipTUV = info->flipU ^ info->flipV ^ !!(info->flags & NV_PATCH_FLAG_TRANSPOSE);

    info->reverse ^= info->flipTUV;

    quadInfo->uMaxSegs = MAX(quadInfo->nu0, quadInfo->nu1);
    quadInfo->vMaxSegs = MAX(quadInfo->nv0, quadInfo->nv1);
    quadInfo->uMinSegs = MIN(quadInfo->nu0, quadInfo->nu1);
    quadInfo->vMinSegs = MIN(quadInfo->nv0, quadInfo->nv1);

    return 1;
}


int MaybeTransposeFlipTri(NV_PATCH_INFO *info)
{
    NV_PATCH_QUAD_INFO *quadInfo = info->quadInfo;
    int index, ii;
    int stitchLeft;
    float ftemp;

    quadInfo->nu0 = (int)(info->tess.tri.n1 + 0.999f);
    quadInfo->nu1 = 1;
    quadInfo->nv0 = (int)(info->tess.tri.n3 + 0.999f);
    quadInfo->nv1 = (int)(info->tess.tri.n2 + 0.999f);

    quadInfo->uMaxSegs = MAX(quadInfo->nu0, quadInfo->nu1);
    quadInfo->vMaxSegs = MAX(quadInfo->nv0, quadInfo->nv1);
    quadInfo->uMinSegs = MIN(quadInfo->nu0, quadInfo->nu1);
    quadInfo->vMinSegs = MIN(quadInfo->nv0, quadInfo->nv1);

return 1;
    if (quadInfo->nv0 > quadInfo->nu0) {
        info->flipT = 1;
        info->flags |= NV_PATCH_FLAG_TRANSPOSE;
        EV_SWAP(quadInfo->nv0, quadInfo->nu0, ii);
        EV_SWAP(info->tess.tri.n3, info->tess.tri.n1, ftemp);
        for (index = 0; index < info->maxAttr; index++) {
            if (info->evalEnables & (1 << index)) {
                ASSERT(info->maps[index].uorder == info->maps[index].vorder);
                EV_SWAP(info->maps[index].uorder, info->maps[index].vorder, ii);
                EV_SWAP(info->maps[index].vfStride, info->maps[index].ufStride, ii);
            }
        }
    }

    stitchLeft = (quadInfo->nv0 < quadInfo->nv1);
    if (stitchLeft) {
        //ffinfo->flags |= NV_PATCH_FLAG_FLIPPED_U;
        //ffinfo->flipU = 1;
        EV_SWAP(quadInfo->nv0, quadInfo->nu0, ii);
        EV_SWAP(info->tess.tri.n3, info->tess.tri.n1, ftemp);
        for (index = 0; index < info->maxAttr; index++) {
            if (info->evalEnables & (1 << index)) {
                NV_PATCH_MAP_INFO *map = &info->maps[index];
                float *rawData = map->rawData, *dst, *src;
                int uorder = map->uorder;
                int vorder = map->vorder;
                int row, col;
                float diag[4];

                ASSERT(info->maps[index].uorder == info->maps[index].vorder);
                for (row = 0; row < vorder; row++) {
                    src = rawData + row*uorder*4;
                    dst = rawData + ((vorder-1)*uorder + (uorder-1-row)) * 4;
                    for (col = 0; col < uorder - row - 1; col++) {
                        CopyPoint(dst, src);
                        src += 4;
                        dst -= uorder;
                    }
                    CopyPoint(diag, src);
                    CopyPoint(src, dst);
                    CopyPoint(dst, diag);
                }

                map->rawData = rawData + (vorder*uorder - 1) * 4;
                info->maps[index].ufStride = -info->maps[index].ufStride;
                info->maps[index].vfStride = -info->maps[index].vfStride;
            }
        }
    }

    info->flipUV = info->flipU ^ info->flipV;
    info->flipTUV = info->flipU ^ info->flipV ^ !!(info->flags & NV_PATCH_FLAG_TRANSPOSE);

    if (info->flipT) {
        info->reverse ^= 1;
    }

    quadInfo->uMaxSegs = MAX(quadInfo->nu0, quadInfo->nu1);
    quadInfo->vMaxSegs = MAX(quadInfo->nv0, quadInfo->nv1);
    quadInfo->uMinSegs = MIN(quadInfo->nu0, quadInfo->nu1);
    quadInfo->vMinSegs = MIN(quadInfo->nv0, quadInfo->nv1);

    return 1;
}

/*****************************************************************************/
// common setup code for patches
void evalCommonSetup(NV_PATCH_INFO *info, float uMaxSegs, float  vMaxSegs)
{
    // determine if swathing is required

    if ((uMaxSegs > info->maxSwatch)  ||  (vMaxSegs > info->maxSwatch)) {
        info->swatchFlags = NV_PATCH_SWATCH;    // for lower level routine to know "swatch mode"
        // calc swatches including the Begin and End stitches
        info->nSwatchU = (uMaxSegs - 1) / info->maxSwatch + 1;
        info->nSwatchV = (vMaxSegs - 1) / info->maxSwatch + 1;

    } else {
        info->swatchFlags = (NV_PATCH_SWATCH_FIRST_ROW | NV_PATCH_SWATCH_LAST_ROW | NV_PATCH_SWATCH_FIRST_COL | NV_PATCH_SWATCH_LAST_COL);
        info->nSwatchU = 1;
        info->nSwatchV = 1;
    }
}

/*****************************************************************************/
unsigned int nvEvalPatch(NV_PATCH_INFO *info)
{
    int i, j;
    unsigned int retVal = NV_PATCH_EVAL_OK;
    NV_PATCH_QUAD_INFO *quadInfo = info->quadInfo;

    if ((info->evalEnables & EV_GUARD_BIT_VERTEX) != EV_GUARD_BIT_VERTEX) {
        nvAssert(0);    //HW absolutely requires position 0 to be active -- if we don't have one bail.
        return NV_PATCH_EVAL_ERR_NOT_HANDLED;

    }

    if (!(info->flags & NV_PATCH_FLAG_TYPE_TRIANGULAR)) {
        // QUAD FRACTIONAL TESSELLATION

        // Tensor product patch
        if (info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL) {
            int transpose = MaybeTransposeFlipQuad(info);
            float nu0 = info->tess.tensor.nu0;
            float nv0 = info->tess.tensor.nv0;
            float nu1 = info->tess.tensor.nu1;
            float nv1 = info->tess.tensor.nv1;
            float nuMid = (nu0 + nu1) / 2.0f;
            float nvMid = (nv0 + nv1) / 2.0f;
            float nu0Old = info->originaltess.tensor.nu0;
            float nv0Old = info->originaltess.tensor.nv0;
            float nu1Old = info->originaltess.tensor.nu1;
            float nv1Old = info->originaltess.tensor.nv1;
            float nuMidOld = (nu0Old + nu1Old) / 2.0f;
            float nvMidOld = (nv0Old + nv1Old) / 2.0f;
            float du0, du1, dv0, dv1, duMid, dvMid;
            float du0Old, du1Old, dv0Old, dv1Old, duMidOld, dvMidOld;
            int u0Segs, u1Segs, v0Segs, v1Segs, uMidSegs, vMidSegs;
            float curvePoints[NV_PATCH_MAX_ORDER][4];

            ComputeFracStepInfo(nu0, &du0, &u0Segs);
            ComputeFracStepInfo(nu1, &du1, &u1Segs);
            ComputeFracStepInfo(nv0, &dv0, &v0Segs);
            ComputeFracStepInfo(nv1, &dv1, &v1Segs);

            ComputeFracStepInfo(nuMid, &duMid, &uMidSegs);
            ComputeFracStepInfo(nvMid, &dvMid, &vMidSegs);

            ComputeFracStepInfo(nu0Old, &du0Old, NULL);
            ComputeFracStepInfo(nu1Old, &du1Old, NULL);
            ComputeFracStepInfo(nv0Old, &dv0Old, NULL);
            ComputeFracStepInfo(nv1Old, &dv1Old, NULL);

            ComputeFracStepInfo(nuMidOld, &duMidOld, NULL);
            ComputeFracStepInfo(nvMidOld, &dvMidOld, NULL);

            evalCommonSetup(info, MAX(u0Segs, u1Segs), MAX(v0Segs, v1Segs));      // various common init for patches

            if (info->swatchFlags & NV_PATCH_SWATCH) {
                // scale to init all matrices and curves to big steps
                du0 *= info->maxSwatch;
                du1 *= info->maxSwatch;
                dv0 *= info->maxSwatch;
                dv1 *= info->maxSwatch;
                duMid *= info->maxSwatch;
                dvMid *= info->maxSwatch;
            }
            if (MAX(u0Segs,u1Segs) > info->maxSwatch || MAX(v0Segs,v1Segs) > info->maxSwatch){
                du0Old *= info->maxSwatch;
                du1Old *= info->maxSwatch;
                dv0Old *= info->maxSwatch;
                dv1Old *= info->maxSwatch;
                duMidOld *= info->maxSwatch;
                dvMidOld *= info->maxSwatch;
            }   

            // Compute FD matrices in each of the 4 orientations
            for (i = 0; i < info->maxAttr; i++) {
                if (info->evalEnables & (1 << i)) {
                    NV_PATCH_MAP_INFO *map = &info->maps[i];
                    float *rawData = map->rawData;
                    int uorder = map->uorder;
                    int vorder = map->vorder;
                    int ufStride = map->ufStride;
                    int vfStride = map->vfStride;

                    nvAssert(quadInfo->m00[i]);
                    nvAssert(quadInfo->m10[i]);
                    nvAssert(quadInfo->m01[i]);
                    nvAssert(quadInfo->m11[i]);
                    nvAssert(quadInfo->guardQF[i]);

                    if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
                        // Compute FD matrix for each subpatch
                        ComputeFDMatrixFlip(info, map, uorder, vorder, duMid, dvMid, quadInfo->m00[i],
                                                rawData, 0, 0, info->maps[i].ufStride, info->maps[i].vfStride);
                        ComputeFDMatrixFlip(info, map, uorder, vorder, duMid, dvMid, quadInfo->m10[i],
                                                rawData, 0, uorder-1, -info->maps[i].ufStride, info->maps[i].vfStride);
                        ComputeFDMatrixFlip(info, map, uorder, vorder, duMid, dvMid, quadInfo->m01[i],
                                                rawData, vorder-1, 0, info->maps[i].ufStride, -info->maps[i].vfStride);
                        ComputeFDMatrixFlip(info, map, uorder, vorder, duMid, dvMid, quadInfo->m11[i],
                                                rawData, vorder-1, uorder-1, -info->maps[i].ufStride, -info->maps[i].vfStride);

                        // Compute guard curves /

                        //  |-u10-->|<--u11-|
                        //  |       |       |
                        // v01 m01  |  m11 v11
                        //  |       |       |
                        // \|/      |      \|/
                        //  --------+--------
                        // /|\      |      /|\
                        //  |       |       |
                        // v00 m00  |  m10 v10
                        //  |       |       |
                        //  |-u00-->|<--u01-|
                        //

                        // u00, u01
                        quadInfo->guardQF[i]->guardU00.order = uorder;
                        quadInfo->guardQF[i]->guardU01.order = uorder;
                        ComputeFDCurveCoeffsFlip(info, uorder, du0, &quadInfo->guardQF[i]->guardU00.coeffs[0][0],
                            pRawData(info, map, rawData, 0, 0), info->maps[i].ufStride * sizeof(float), 1, info->flipU);
                        ComputeFDCurveCoeffsFlip(info, uorder, du0, &quadInfo->guardQF[i]->guardU01.coeffs[0][0],
                            pRawData(info, map, rawData, 0, 0), info->maps[i].ufStride * sizeof(float), 0, info->flipU);

                        // u10, u11
                        quadInfo->guardQF[i]->guardU10.order = uorder;
                        quadInfo->guardQF[i]->guardU11.order = uorder;
                        ComputeFDCurveCoeffsFlip(info, uorder, du1, &quadInfo->guardQF[i]->guardU10.coeffs[0][0],
                            pRawData(info, map, rawData, vorder-1, 0), info->maps[i].ufStride * sizeof(float), 1, info->flipU);
                        ComputeFDCurveCoeffsFlip(info, uorder, du1, &quadInfo->guardQF[i]->guardU11.coeffs[0][0],
                            pRawData(info, map, rawData, vorder-1, 0), info->maps[i].ufStride * sizeof(float), 0, info->flipU);

                        // v00, v01
                        quadInfo->guardQF[i]->guardV00.order = vorder;
                        quadInfo->guardQF[i]->guardV01.order = vorder;
                        ComputeFDCurveCoeffsFlip(info, vorder, dv0, &quadInfo->guardQF[i]->guardV00.coeffs[0][0],
                            pRawData(info, map, rawData, 0, 0), info->maps[i].vfStride * sizeof(float), 1, info->flipV);
                        ComputeFDCurveCoeffsFlip(info, vorder, dv0, &quadInfo->guardQF[i]->guardV01.coeffs[0][0],
                            pRawData(info, map, rawData, 0, 0), info->maps[i].vfStride * sizeof(float), 0, info->flipV);

                        // v10, v11
                        quadInfo->guardQF[i]->guardV10.order = vorder;
                        quadInfo->guardQF[i]->guardV11.order = vorder;
                        ComputeFDCurveCoeffsFlip(info, vorder, dv1, &quadInfo->guardQF[i]->guardV10.coeffs[0][0],
                            pRawData(info, map, rawData, 0, uorder-1), info->maps[i].vfStride * sizeof(float), 1, info->flipV);
                        ComputeFDCurveCoeffsFlip(info, vorder, dv1, &quadInfo->guardQF[i]->guardV11.coeffs[0][0],
                            pRawData(info, map, rawData, 0, uorder-1), info->maps[i].vfStride * sizeof(float), 0, info->flipV);

                        // uCenter0, uCenter1
                        quadInfo->guardQF[i]->guardUCenter0.order = uorder;
                        quadInfo->guardQF[i]->guardUCenter1.order = uorder;
                        for (j = 0; j < uorder; j++) {
                            ComputeCurvePoint(vorder, &curvePoints[j][0], pRawData(info, map, rawData, 0, j), vfStride * sizeof(float), .5);
                        }
                        ComputeFDCurveCoeffsFlip(info, uorder, duMid, &quadInfo->guardQF[i]->guardUCenter0.coeffs[0][0],
                            &curvePoints[0][0], 4 * sizeof(float), 1, 0);
                        ComputeFDCurveCoeffsFlip(info, uorder, duMid, &quadInfo->guardQF[i]->guardUCenter1.coeffs[0][0],
                            &curvePoints[0][0], 4 * sizeof(float), 0, 0);

                        // vCenter0, vCenter1
                        quadInfo->guardQF[i]->guardVCenter0.order = vorder;
                        quadInfo->guardQF[i]->guardVCenter1.order = vorder;
                        for (j = 0; j < vorder; j++) {
                            ComputeCurvePoint(uorder, &curvePoints[j][0], pRawData(info, map, rawData, j, 0),
                                                ufStride*sizeof(float),.5);
                        }
                        ComputeFDCurveCoeffsFlip(info, vorder, dvMid, &quadInfo->guardQF[i]->guardVCenter0.coeffs[0][0],
                            &curvePoints[0][0], 4 * sizeof(float), 1, 0);
                        ComputeFDCurveCoeffsFlip(info, vorder, dvMid, &quadInfo->guardQF[i]->guardVCenter1.coeffs[0][0],
                            &curvePoints[0][0], 4 * sizeof(float), 0, 0);

                        // Compute points
                        // DCR seems to me we computed these already above!!! why duplicate it here?
                        // u0Mid
                        ComputeCurvePoint(uorder, quadInfo->guardQF[i]->u0Mid, pRawData(info, map, rawData, 0, 0),
                                                ufStride * sizeof(float), .5);

                        // u1Mid
                        ComputeCurvePoint(uorder, quadInfo->guardQF[i]->u1Mid, pRawData(info, map, rawData, vorder-1, 0),
                                                ufStride * sizeof(float), .5);

                        // v0Mid
                        ComputeCurvePoint(vorder, quadInfo->guardQF[i]->v0Mid, pRawData(info, map, rawData, 0, 0),
                                                vfStride * sizeof(float), .5);

                        // v1Mid
                        ComputeCurvePoint(vorder, quadInfo->guardQF[i]->v1Mid, pRawData(info, map, rawData, 0, uorder-1),
                                                vfStride * sizeof(float), .5);

                        // center
                        for (j = 0; j < uorder; j++) {
                            ComputeCurvePoint(vorder, &curvePoints[j][0], pRawData(info, map, rawData, 0, j),
                                                vfStride * sizeof(float), .5);
                        }
                        ComputeCurvePoint(uorder, quadInfo->guardQF[i]->center, &curvePoints[0][0],
                                                4*sizeof(float), .5);

                        info->originaltess = info->tess;
                        info->maps[i].dirtyFlags = 0;

                    } else if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_TESSELLATION) {
                        RetessellateFDMatrix(info, duMidOld, dvMidOld, duMid, dvMid, quadInfo->m00[i]);
                        RetessellateFDMatrix(info, duMidOld, dvMidOld, duMid, dvMid, quadInfo->m10[i]);
                        RetessellateFDMatrix(info, duMidOld, dvMidOld, duMid, dvMid, quadInfo->m01[i]);
                        RetessellateFDMatrix(info, duMidOld, dvMidOld, duMid, dvMid, quadInfo->m11[i]);

                        RetessellateFDCurve(info, du0Old, du0, &quadInfo->guardQF[i]->guardU00);
                        RetessellateFDCurve(info, du0Old, du0, &quadInfo->guardQF[i]->guardU01);
                        RetessellateFDCurve(info, du1Old, du1, &quadInfo->guardQF[i]->guardU10);
                        RetessellateFDCurve(info, du1Old, du1, &quadInfo->guardQF[i]->guardU11);
                        RetessellateFDCurve(info, dv0Old, dv0, &quadInfo->guardQF[i]->guardV00);
                        RetessellateFDCurve(info, dv0Old, dv0, &quadInfo->guardQF[i]->guardV01);
                        RetessellateFDCurve(info, dv1Old, dv1, &quadInfo->guardQF[i]->guardV10);
                        RetessellateFDCurve(info, dv1Old, dv1, &quadInfo->guardQF[i]->guardV11);

                        RetessellateFDCurve(info, duMidOld, duMid, &quadInfo->guardQF[i]->guardUCenter0);
                        RetessellateFDCurve(info, duMidOld, duMid, &quadInfo->guardQF[i]->guardUCenter1);
                        RetessellateFDCurve(info, dvMidOld, dvMid, &quadInfo->guardQF[i]->guardVCenter0);
                        RetessellateFDCurve(info, dvMidOld, dvMid, &quadInfo->guardQF[i]->guardVCenter1);

                        info->originaltess = info->tess;
                        info->maps[i].dirtyFlags &= ~NV_PATCH_DIRTY_TESSELLATION;
                    }
                }
            }

            // DCR: should we be guard curving ALL attributes?  HW doesn't.
            // FBF: the names are mis-leading. They are actually full attribute curves
            // used when doing the regular grid and transitions. But, they are also "guards"
            // in the sense that they are used to provide a consistent set of values
            // for position and normal along the various sub-patch directions.
            for (i = 0; i < info->maxAttr; i++) {
                if (info->evalEnables & (1 << i)) {
                    // Curves
                    quadInfo->pU0[i] = &quadInfo->guardQF[i]->guardU00;
                    quadInfo->pV0[i] = &quadInfo->guardQF[i]->guardV00;
                    quadInfo->pU1[i] = &quadInfo->guardQF[i]->guardUCenter0;
                    quadInfo->pV1[i] = &quadInfo->guardQF[i]->guardVCenter0;

                    // Points
                    quadInfo->pCorners[i][0][0] = quadInfo->m00[i]->data[0][0];
                    quadInfo->pCorners[i][0][1] = quadInfo->guardQF[i]->u0Mid;
                    quadInfo->pCorners[i][1][0] = quadInfo->guardQF[i]->v0Mid;
                    quadInfo->pCorners[i][1][1] = quadInfo->guardQF[i]->center;
                }
            }

            retVal |= DrawFracPatch(info, u0Segs, uMidSegs, v0Segs, vMidSegs, 0, quadInfo->m00);

            for (i = 0; i < info->maxAttr; i++) {
                if (info->evalEnables & (1 << i)) {
                    // Curves
                    quadInfo->pU0[i] = &quadInfo->guardQF[i]->guardU01;
                    quadInfo->pV0[i] = &quadInfo->guardQF[i]->guardV10;
                    quadInfo->pU1[i] = &quadInfo->guardQF[i]->guardUCenter1;
                    quadInfo->pV1[i] = &quadInfo->guardQF[i]->guardVCenter0;

                    // Points
                    quadInfo->pCorners[i][0][0] = quadInfo->m10[i]->data[0][0];
                    quadInfo->pCorners[i][0][1] = quadInfo->guardQF[i]->u0Mid;
                    quadInfo->pCorners[i][1][0] = quadInfo->guardQF[i]->v1Mid;
                    quadInfo->pCorners[i][1][1] = quadInfo->guardQF[i]->center;
                }
            }

            retVal |= DrawFracPatch(info, u0Segs, uMidSegs, v1Segs, vMidSegs, 1, quadInfo->m10);

            for (i = 0; i < info->maxAttr; i++) {
                if (info->evalEnables & (1 << i)) {
                    // Curves
                    quadInfo->pU0[i] = &quadInfo->guardQF[i]->guardU10;
                    quadInfo->pV0[i] = &quadInfo->guardQF[i]->guardV01;
                    quadInfo->pU1[i] = &quadInfo->guardQF[i]->guardUCenter0;
                    quadInfo->pV1[i] = &quadInfo->guardQF[i]->guardVCenter1;

                    // Points
                    quadInfo->pCorners[i][0][0] = quadInfo->m01[i]->data[0][0];
                    quadInfo->pCorners[i][0][1] = quadInfo->guardQF[i]->u1Mid;
                    quadInfo->pCorners[i][1][0] = quadInfo->guardQF[i]->v0Mid;
                    quadInfo->pCorners[i][1][1] = quadInfo->guardQF[i]->center;
                }
            }

            retVal |= DrawFracPatch(info, u1Segs, uMidSegs, v0Segs, vMidSegs, 1, quadInfo->m01);

            for (i = 0; i < info->maxAttr; i++) {
                if (info->evalEnables & (1 << i)) {
                    // Curves
                    quadInfo->pU0[i] = &quadInfo->guardQF[i]->guardU11;
                    quadInfo->pV0[i] = &quadInfo->guardQF[i]->guardV11;
                    quadInfo->pU1[i] = &quadInfo->guardQF[i]->guardUCenter1;
                    quadInfo->pV1[i] = &quadInfo->guardQF[i]->guardVCenter1;

                    // Points
                    quadInfo->pCorners[i][0][0] = quadInfo->m11[i]->data[0][0];
                    quadInfo->pCorners[i][0][1] = quadInfo->guardQF[i]->u1Mid;
                    quadInfo->pCorners[i][1][0] = quadInfo->guardQF[i]->v1Mid;
                    quadInfo->pCorners[i][1][1] = quadInfo->guardQF[i]->center;
                }
            }

            retVal |= DrawFracPatch(info, u1Segs, uMidSegs, v1Segs, vMidSegs, 0, quadInfo->m11);

        /*****************************************************************************/
        } else {    // QUAD INTEGER TESSELLATION
            int transpose = MaybeTransposeFlipQuad(info);
            int stitchBottom = (quadInfo->nu0 < quadInfo->nu1);
            int stitchTop = (quadInfo->nu0 > quadInfo->nu1);
            int stitchLeft = (quadInfo->nv0 < quadInfo->nv1);
            int stitchRight = (quadInfo->nv0 > quadInfo->nv1);

            float du0 = 1.0f / quadInfo->nu0;
            float du1 = 1.0f / quadInfo->nu1;
            float dv0 = 1.0f / quadInfo->nv0;
            float dv1 = 1.0f / quadInfo->nv1;

            float duMax = 1.0f / quadInfo->uMaxSegs;
            float dvMax = 1.0f / quadInfo->vMaxSegs;

            int nu0Old;
            int nv0Old;
            int nu1Old;
            int nv1Old;
            int nuMaxOld;
            int nvMaxOld;
            float du0Old;
            float du1Old;
            float dv0Old;
            float dv1Old;
            float duMaxOld;
            float dvMaxOld;
            int uMaxDir, vMaxDir;
            int stitchUBegin, stitchUEnd, stitchVBegin, stitchVEnd;

            if (info->maps[ATTR_V].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
                ComputeEdgeDirections(info, 0, &info->maps[ATTR_V],
                            &quadInfo->u0Dir, &quadInfo->u1Dir, &quadInfo->v0Dir, &quadInfo->v1Dir);
                if (quadInfo->uMaxSegs == 1) {
                    // then reverse transitions in U do not matter
                    quadInfo->u0Dir = quadInfo->u1Dir = 1;
                }
                if (quadInfo->vMaxSegs == 1) {
                    // then reverse transitions in V do not matter
                    quadInfo->v0Dir = quadInfo->v1Dir = 1;
                }
            }

            // Fill in directions of max and min edges
            if (quadInfo->nu0 >= quadInfo->nu1) {
                uMaxDir = quadInfo->u0Dir;
            } else {
                uMaxDir = quadInfo->u1Dir;
            }
            if (quadInfo->nv0 >= quadInfo->nv1) {
                vMaxDir = quadInfo->v0Dir;
            } else {
                vMaxDir = quadInfo->v1Dir;
            }

            if (info->flags & NV_PATCH_FLAG_SOME_DIRTY_TESS) {
                // compute the du/dvMaxOld for retessellations
                nu0Old = (int)(info->originaltess.tensor.nu0 + 0.999f);
                nv0Old = (int)(info->originaltess.tensor.nv0 + 0.999f);
                nu1Old = (int)(info->originaltess.tensor.nu1 + 0.999f);
                nv1Old = (int)(info->originaltess.tensor.nv1 + 0.999f);
                nuMaxOld = MAX(nu0Old,nu1Old);
                nvMaxOld = MAX(nv0Old,nv1Old);
                du0Old = 1.0f / nu0Old;
                du1Old = 1.0f / nu1Old;
                dv0Old = 1.0f / nv0Old;
                dv1Old = 1.0f / nv1Old;
                duMaxOld = 1.0f / nuMaxOld;
                dvMaxOld = 1.0f / nvMaxOld;
            }

            info->originaltess = info->tess;
            quadInfo->uMaxDir = uMaxDir;
            quadInfo->vMaxDir = vMaxDir;
            quadInfo->du0 = du0;
            quadInfo->du1 = du1;
            quadInfo->dv0 = dv0;
            quadInfo->dv1 = dv1;
            quadInfo->duMax = duMax;
            quadInfo->dvMax = dvMax;

            evalCommonSetup(info, quadInfo->uMaxSegs, quadInfo->vMaxSegs);      // various common init for patches

            if (info->swatchFlags & NV_PATCH_SWATCH) {
                // scale to init all matrices and curves to big steps
                du0 *= info->maxSwatch;
                du1 *= info->maxSwatch;
                dv0 *= info->maxSwatch;
                dv1 *= info->maxSwatch;
                duMax *= info->maxSwatch;
                dvMax *= info->maxSwatch;
            }
            if(nuMaxOld > info->maxSwatch || nvMaxOld > info->maxSwatch){
                du0Old *= info->maxSwatch;
                du1Old *= info->maxSwatch;
                dv0Old *= info->maxSwatch;
                dv1Old *= info->maxSwatch;
                duMaxOld *= info->maxSwatch;
                dvMaxOld *= info->maxSwatch;
            }

            // We always tessellate in the "MaxDir" direction
            for (i = 0; i < info->maxAttr; i++) {
                if (info->evalEnables & (1 << i)) {
                    NV_PATCH_MAP_INFO *map = &info->maps[i];
                    float *rawData = map->rawData;
                    int uorder = map->uorder;
                    int vorder = map->vorder;

                    nvAssert(quadInfo->m00[i]);
                    nvAssert(quadInfo->pU0[i]);
                    nvAssert(quadInfo->pU1[i]);
                    nvAssert(quadInfo->pV0[i]);
                    nvAssert(quadInfo->pV1[i]);
                    nvAssert(quadInfo->pVInner[i]);
                    nvAssert(quadInfo->pVInner[i]);

                    if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
                        // compute FDMatrix given the modified origin according to u/vDir
                        if (vMaxDir == 1) {
                            if (uMaxDir == 1) {
                                ComputeFDMatrixFlip(info, map, uorder, vorder, duMax, dvMax, quadInfo->m00[i],
                                    rawData, 0, 0, info->maps[i].ufStride, info->maps[i].vfStride);
                            } else {
                                ComputeFDMatrixFlip(info, map, uorder, vorder, duMax, dvMax, quadInfo->m00[i],
                                    rawData, 0, uorder-1, -info->maps[i].ufStride, info->maps[i].vfStride);
                            }
                        } else {
                            if (uMaxDir == 1) {
                                ComputeFDMatrixFlip(info, map, uorder, vorder, duMax, dvMax, quadInfo->m00[i],
                                    rawData, vorder-1, 0, info->maps[i].ufStride, -info->maps[i].vfStride);
                            } else {
                                ComputeFDMatrixFlip(info, map, uorder, vorder, duMax, dvMax, quadInfo->m00[i],
                                    rawData, vorder-1, uorder-1, -info->maps[i].ufStride, -info->maps[i].vfStride);
                            }
                        }

                    } else if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_TESSELLATION) {
                        RetessellateFDMatrix(info, duMaxOld, dvMaxOld, duMax, dvMax, quadInfo->m00[i]);
                    }
                }
            }

            // Reverse transitions may cause stitches
            // If dir's are not equal, AND there are no stitches for other reasons,
            // AND it's not degenerate in this direction, then ...
            if ((quadInfo->u0Dir != quadInfo->u1Dir) && !stitchBottom && !stitchTop && (quadInfo->nu0 > 1)) {
                nvAssert(quadInfo->nu0 == quadInfo->nu1);
                stitchTop = 1;          // make a stitch for the reversal
            }
            if ((quadInfo->v0Dir != quadInfo->v1Dir) && !stitchLeft && !stitchRight && (quadInfo->nv0 > 1)) {
                nvAssert(quadInfo->nv0 == quadInfo->nv1);
                stitchRight = 1;        // make a stitch for the reversal
            }

            // Decide where the stitching occurs w.r.t. the regular region
            if (uMaxDir == 1) {
                stitchUBegin = stitchLeft;
                stitchUEnd = stitchRight;
            } else {
                stitchUBegin = stitchRight;
                stitchUEnd = stitchLeft;
            }
            if (vMaxDir == 1) {
                stitchVBegin = stitchBottom;
                stitchVEnd = stitchTop;
            } else {
                stitchVBegin = stitchTop;
                stitchVEnd = stitchBottom;
            }

            quadInfo->stitchLeft   = stitchLeft;
            quadInfo->stitchRight  = stitchRight;
            quadInfo->stitchBottom = stitchBottom;
            quadInfo->stitchTop    = stitchTop;
            quadInfo->stitchUBegin = stitchUBegin;
            quadInfo->stitchUEnd   = stitchUEnd;
            quadInfo->stitchVBegin = stitchVBegin;
            quadInfo->stitchVEnd   = stitchVEnd;

            // calc if U/V inner guards required
            quadInfo->needUInner = (quadInfo->vMaxSegs > 1) && (stitchBottom || stitchTop);
            quadInfo->needVInner = (quadInfo->uMaxSegs > 1) && (stitchLeft || stitchRight);

            for (i = 0; i < info->maxAttr; i++) {
                if (info->evalEnables & (1 << i)) {
                    NV_PATCH_MAP_INFO *map = &info->maps[i];
                    float *rawData = map->rawData;
                    int uorder = map->uorder;
                    int vorder = map->vorder;

                    if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
                        // Store the corner points
                        getRawData(info, &info->maps[i], rawData, 0,               0, quadInfo->pCorners[i][0][0]);
                        getRawData(info, &info->maps[i], rawData, 0,        uorder-1, quadInfo->pCorners[i][0][1]);
                        getRawData(info, &info->maps[i], rawData, vorder-1, 0,        quadInfo->pCorners[i][1][0]);
                        getRawData(info, &info->maps[i], rawData, vorder-1, uorder-1, quadInfo->pCorners[i][1][1]);

                        // Compute U boundary curves
                        quadInfo->pU0[i]->order = uorder;
                        quadInfo->pU1[i]->order = uorder;
                        ComputeFDCurveCoeffsFlip(info, uorder, du0, &quadInfo->pU0[i]->coeffs[0][0],
                            pRawData(info, map, rawData, 0, 0),
                            info->maps[i].ufStride * sizeof(float), quadInfo->u0Dir, info->flipU);
                        ComputeFDCurveCoeffsFlip(info, uorder, du1, &quadInfo->pU1[i]->coeffs[0][0],
                            pRawData(info, map, rawData, vorder-1, 0),
                            info->maps[i].ufStride * sizeof(float), quadInfo->u1Dir, info->flipU);

                        // Compute V boundary curves
                        quadInfo->pV0[i]->order = vorder;
                        quadInfo->pV1[i]->order = vorder;
                        ComputeFDCurveCoeffsFlip(info, vorder, dv0, &quadInfo->pV0[i]->coeffs[0][0],
                            pRawData(info, map, rawData, 0, 0),
                            info->maps[i].vfStride * sizeof(float), quadInfo->v0Dir, info->flipV);
                        ComputeFDCurveCoeffsFlip(info, vorder, dv1, &quadInfo->pV1[i]->coeffs[0][0],
                            pRawData(info, map, rawData, 0, uorder-1),
                            info->maps[i].vfStride * sizeof(float), quadInfo->v1Dir, info->flipV);

                        info->maps[i].dirtyFlags = 0;

                    } else if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_TESSELLATION) {
                        // Recompute U boundary curves
                        RetessellateFDCurve(info, du0Old, du0, quadInfo->pU0[i]);
                        RetessellateFDCurve(info, du1Old, du1, quadInfo->pU1[i]);

                        // Recompute V boundary curves
                        RetessellateFDCurve(info, dv0Old, dv0, quadInfo->pV0[i]);
                        RetessellateFDCurve(info, dv1Old, dv1, quadInfo->pV1[i]);

                        info->maps[i].dirtyFlags &= ~NV_PATCH_DIRTY_TESSELLATION;
                    }

                    // If we're stitching on the top or bottom, do a U inner guard
                    if (quadInfo->needUInner) {
// XXX maybe do this in lower routine ?
CopyMatrix(info, &info->tempMatrix, quadInfo->m00[i]);
RetessellateFDMatrix(info, 1.f, dvMax, 1.f, quadInfo->dvMax, &info->tempMatrix);    // big to small
                        if (stitchVBegin) {
                            OffsetFDMatrix_Extract_Discard(info, quadInfo->pUInner[i], 0,  &info->tempMatrix, 0, 1);
                        } else {
                            nvAssert(stitchVEnd);
                            OffsetFDMatrix_Extract_Discard(info, quadInfo->pUInner[i], 0,  &info->tempMatrix, 0, quadInfo->vMaxSegs-1);
                        }
                    }

                    // If we're stitching on the left or right, do a V inner guard
                    if (quadInfo->needVInner) {
// XXX maybe do this in lower routine ?
CopyMatrix(info, &info->tempMatrix, quadInfo->m00[i]);
RetessellateFDMatrix(info, duMax, 1.f, quadInfo->duMax, 1.f, &info->tempMatrix);    // big to small
                        if (stitchUBegin) {
                            OffsetFDMatrix_Extract_Discard(info, quadInfo->pVInner[i], 1,  &info->tempMatrix, 1, 0);
                        } else {
                            OffsetFDMatrix_Extract_Discard(info, quadInfo->pVInner[i], 1,  &info->tempMatrix, quadInfo->uMaxSegs-1, 0);
                        }
                    }
                }
            }

            retVal |= DrawIntPatch(info, quadInfo, quadInfo->m00);
        }

    /*****************************************************************************/
    } else {
        // TRI FRAC TESSELLATION
        if (info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL) {
            int transpose = MaybeTransposeFlipTri(info);
            float n1 = info->tess.tri.n1;
            float n2 = info->tess.tri.n2;
            float n3 = info->tess.tri.n3;
            float nMid = (n1 + n2 + n3)/3.0f;
            float n1Old = info->originaltess.tri.n1;
            float n2Old = info->originaltess.tri.n2;
            float n3Old = info->originaltess.tri.n3;
            float nMidOld = (n1 + n2 + n3)/3.0f;
            float duv[3], duvMid, duvOld[3], duvMidOld;
            int segs[3], midSegs, vMax;
            FDMatrix *m[3][NV_PATCH_NUMBER_OF_ATTRIBS];

            //guard = NULL;
            for (i = 0; i < info->maxAttr; i++) {
                m[0][i] = quadInfo->m00[i];
                m[1][i] = quadInfo->m10[i];
                m[2][i] = quadInfo->m01[i];
            }

            ComputeFracStepInfo(n1, &duv[0], &segs[0]);
            ComputeFracStepInfo(n2, &duv[1], &segs[1]);
            ComputeFracStepInfo(n3, &duv[2], &segs[2]);

            ComputeFracStepInfo(nMid, &duvMid, &midSegs);
            duvMid *= 2;

            ComputeFracStepInfo(n1Old, &duvOld[0], NULL);
            ComputeFracStepInfo(n2Old, &duvOld[1], NULL);
            ComputeFracStepInfo(n3Old, &duvOld[2], NULL);

            ComputeFracStepInfo(nMidOld, &duvMidOld, NULL);
            duvMidOld *= 2;

            vMax = MAX(segs[1], segs[2]);
            evalCommonSetup(info, MAX(segs[0], midSegs), MAX(vMax, midSegs));        // various common init for patches

            if (info->swatchFlags & NV_PATCH_SWATCH) {
                // scale to init all matrices and curves to big steps
                duv[0] *= info->maxSwatch;
                duv[1] *= info->maxSwatch;
                duv[2] *= info->maxSwatch;
                duvMid *= info->maxSwatch;
            }

            // Compute FD matrices in each of the 3 orientations
            for (i = 0; i < info->maxAttr; i++) {
                if (info->evalEnables & (1 << i)) {
                    NV_PATCH_MAP_INFO *map = &info->maps[i];
                    float *rawData = map->rawData;
                    int order = map->uorder;
                    int ufStride = map->ufStride;
                    int vfStride = map->vfStride;
                    int k, l;

                    nvAssert(info->maps[i].uorder == info->maps[i].vorder);

                    // Compute FD matrix for each subpatch
                    if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
                        for (j = 0; j < 3; j++) {
                            info->reparam.rows = info->reparam.columns = order;
                            for (l = 0; l < order; l++) {       // V order
                                for (k = 0; k < order-l; k++) { // U order
                                    getRawData(info, &info->maps[i], rawData, l, k, info->reparam.data[l][k]);
                                }
                            }
                            ReparameterizeTriPatch(info, &info->reparam, m[j][i], j, 1);

                            quadInfo->guardTF[i]->guardCenter[j].order = order;
                            if (TriPatchLayout[j].centerCurve) {
                                ComputeFDCurveCoeffsFlip(info, order, duvMid, &quadInfo->guardTF[i]->guardCenter[j].coeffs[0][0],
                                    &m[j][i]->data[order-1][0][0],
                                    4 * sizeof(float), 1, 0);
                            } else {
                                ComputeFDCurveCoeffsFlip(info, order, duvMid, &quadInfo->guardTF[i]->guardCenter[j].coeffs[0][0],
                                    &m[j][i]->data[0][order-1][0],
                                    4*NV_PATCH_MAX_ORDER*sizeof(float), 1, 0);
                            }

                            ComputeFDMatrixFlip(info, 0, order, order, duvMid, duvMid, m[j][i],
                                &m[j][i]->data[0][0][0], 0, 0, 4, 4*NV_PATCH_MAX_ORDER);
                        }

                        // Compute guard curves /

                        //  |\
                        //  | \
                        //  |  \
                        // 20   11
                        //  |     \
                        //  |      \|
                        // \|/     -\
                        //  +        +
                        // /|\-    _/ \-
                        //  |  \-+/   |\
                        //  |     \     \
                        // 21      |    10
                        //  |      \      \
                        //  |       |      \
                        //  |--00-->|<---01-|
                        //
                        // 00, 01
                        quadInfo->guardTF[i]->guard[0][0].order = order;
                        quadInfo->guardTF[i]->guard[0][1].order = order;

                        ComputeFDCurveCoeffsFlip(info, order, duv[0], &quadInfo->guardTF[i]->guard[0][0].coeffs[0][0],
                            pRawData(info, map, rawData, 0, 0),
                            ufStride * sizeof(float), 1, info->flipU);
                        ComputeFDCurveCoeffsFlip(info, order, duv[0], &quadInfo->guardTF[i]->guard[0][1].coeffs[0][0],
                            pRawData(info, map, rawData, 0, 0),
                            ufStride * sizeof(float), 0, info->flipU);

                        // 10, 11
                        quadInfo->guardTF[i]->guard[1][0].order = order;
                        quadInfo->guardTF[i]->guard[1][1].order = order;
                        ComputeFDCurveCoeffsFlip(info, order, duv[1], &quadInfo->guardTF[i]->guard[1][0].coeffs[0][0],
                            pRawData(info, map, rawData, 0, order-1),
                            (vfStride-4) * sizeof(float), 1, info->flipU);
                        ComputeFDCurveCoeffsFlip(info, order, duv[1], &quadInfo->guardTF[i]->guard[1][1].coeffs[0][0],
                            pRawData(info, map, rawData, 0, order-1),
                            (vfStride-4) * sizeof(float), 0, info->flipU);

                        // 20, 21
                        quadInfo->guardTF[i]->guard[2][0].order = order;
                        quadInfo->guardTF[i]->guard[2][1].order = order;
                        ComputeFDCurveCoeffsFlip(info, order, duv[2], &quadInfo->guardTF[i]->guard[2][0].coeffs[0][0],
                            pRawData(info, map, rawData, 0, 0),
                            vfStride * sizeof(float), 0, info->flipU);
                        ComputeFDCurveCoeffsFlip(info, order, duv[2], &quadInfo->guardTF[i]->guard[2][1].coeffs[0][0],
                            pRawData(info, map, rawData, 0, 0),
                            vfStride * sizeof(float), 1, info->flipU);

                        // Compute points
                        // mid0,1,2
                        ComputeCurvePoint(order, quadInfo->guardTF[i]->mid[0],
                                            pRawData(info, map, rawData, 0, 0),
                                            ufStride * sizeof(float), .5);
                        ComputeCurvePoint(order, quadInfo->guardTF[i]->mid[1],
                                            pRawData(info, map, rawData, 0, order-1),
                                            (vfStride-4) * sizeof(float), .5);
                        ComputeCurvePoint(order, quadInfo->guardTF[i]->mid[2],
                                            pRawData(info, map, rawData, 0, 0),
                                            vfStride * sizeof(float), .5);

                        // center
                        ComputeTriPatchCenter(order, quadInfo->guardTF[i]->center, pRawData(info, map, rawData, 0, 0),
                            ufStride * sizeof(float), vfStride * sizeof(float));

                        info->maps[i].dirtyFlags = 0;
                    } else if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_TESSELLATION) {
                        for (j = 0; j < 3; j++) {
                            RetessellateFDMatrix(info, duvMidOld, duvMidOld, duvMid, duvMid, m[j][i]);

                            RetessellateFDCurve(info, duvOld[j], duv[j], &quadInfo->guardTF[i]->guard[j][0]);
                            RetessellateFDCurve(info, duvOld[j], duv[j], &quadInfo->guardTF[i]->guard[j][1]);

                            RetessellateFDCurve(info, duvMidOld, duvMid, &quadInfo->guardTF[i]->guardCenter[j]);
                        }

                        info->maps[i].dirtyFlags &= ~NV_PATCH_DIRTY_TESSELLATION;
                    }
                }
            }
            info->originaltess = info->tess;

            for (j = 0; j < 3; j++) {
                int uSide   = TriPatchLayout[j].uSide;
                int vSide   = TriPatchLayout[j].vSide;
                int uDir    = TriPatchLayout[j].uDir;
                int vDir    = TriPatchLayout[j].vDir;
                int reverse = TriPatchLayout[j].reverse;

                for (i = 0; i < info->maxAttr; i++) {
                    if (info->evalEnables & (1 << i)) {
                        // Curves
                        quadInfo->pU0[i] = &quadInfo->guardTF[i]->guard[uSide][uDir];
                        quadInfo->pV0[i] = &quadInfo->guardTF[i]->guard[vSide][vDir];
                        quadInfo->pU1[i] = &quadInfo->guardTF[i]->guardCenter[vSide];
                        quadInfo->pV1[i] = &quadInfo->guardTF[i]->guardCenter[uSide];

                        // Points
                        quadInfo->pCorners[i][0][0] = m[j][i]->data[0][0];
                        quadInfo->pCorners[i][0][1] = quadInfo->guardTF[i]->mid[uSide];
                        quadInfo->pCorners[i][1][0] = quadInfo->guardTF[i]->mid[vSide];
                        quadInfo->pCorners[i][1][1] = quadInfo->guardTF[i]->center;
                    }
                }

                retVal |= DrawFracPatch(info, segs[uSide], midSegs, segs[vSide], midSegs, reverse, m[j]);
            }

        /*****************************************************************************/
        } else {    // TRI INTEGER TESSELLATION
            int transpose = MaybeTransposeFlipTri(info);

            float du0 = 1.0f / quadInfo->nu0;
            float du1 = 1.0f;
            float dv0 = 1.0f / quadInfo->nv0;
            float dv1 = 1.0f / quadInfo->nv1;
            float duMax = 1.0f / quadInfo->uMaxSegs;
            float dvMax = 1.0f / quadInfo->vMaxSegs;

            int n1Old = (int)(info->originaltess.tri.n1 + 0.999f);
            int n2Old = (int)(info->originaltess.tri.n2 + 0.999f);
            int n3Old = (int)(info->originaltess.tri.n3 + 0.999f);
            int nuMaxOld = n1Old;
            int nvMaxOld = MAX(n2Old,n3Old);

            float du0Old = 1.0f / n1Old;
            float du1Old = 1.0f;
            float dv0Old = 1.0f / n3Old;
            float dv1Old = 1.0f / n2Old;
            float duMaxOld = 1.0f / nuMaxOld;
            float dvMaxOld = 1.0f / nvMaxOld;

            int uMaxDir, vMaxDir;
            int stitchBottom = 0;
            int stitchTop = (quadInfo->nu0 > 1);
            int stitchLeft = (quadInfo->nv0 < quadInfo->nv1);
            int stitchRight = (quadInfo->nv0 > quadInfo->nv1);
            int stitchUBegin, stitchUEnd, stitchVBegin, stitchVEnd;

            if (info->maps[ATTR_V].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
                //only compute edge directions when I have some map info around
                ComputeEdgeDirections(info, 1, &info->maps[ATTR_V],
                        &quadInfo->u0Dir, &quadInfo->u1Dir, &quadInfo->v0Dir, &quadInfo->v1Dir);
                if (quadInfo->uMaxSegs == 1) {
                    // then reverse transitions in U do not matter
                    quadInfo->u0Dir = quadInfo->u1Dir = 1;
                }
                if (quadInfo->vMaxSegs == 1) {
                    // then reverse transitions in V do not matter
                    quadInfo->v0Dir = quadInfo->v1Dir = 1;
                }
            }

            // Fill in directions of max and min edges
            uMaxDir = quadInfo->u0Dir;
            if (quadInfo->nv0 >= quadInfo->nv1) {
                vMaxDir = quadInfo->v0Dir;
            } else {
                vMaxDir = quadInfo->v1Dir;
            }

            info->originaltess = info->tess;
            quadInfo->uMaxDir = uMaxDir;
            quadInfo->vMaxDir = vMaxDir;
            quadInfo->du0 = du0;
            quadInfo->du1 = du1;
            quadInfo->dv0 = dv0;
            quadInfo->dv1 = dv1;
            quadInfo->duMax = duMax;
            quadInfo->dvMax = dvMax;

            evalCommonSetup(info, quadInfo->uMaxSegs, quadInfo->vMaxSegs);      // various common init for patches

            if (info->swatchFlags & NV_PATCH_SWATCH) {
                // scale to init all matrices and curves to big steps
                du0 *= info->maxSwatch;
                du1 *= info->maxSwatch;
                dv0 *= info->maxSwatch;
                dv1 *= info->maxSwatch;
                duMax *= info->maxSwatch;
                dvMax *= info->maxSwatch;
            }
            if(nuMaxOld > info->maxSwatch || nvMaxOld > info->maxSwatch){      // various common init for patches
                du0Old *= info->maxSwatch;
                du1Old *= info->maxSwatch;
                dv0Old *= info->maxSwatch;
                dv1Old *= info->maxSwatch;
                duMaxOld *= info->maxSwatch;
                dvMaxOld *= info->maxSwatch;
            }

            // Compute FD matrices in each of the 3 orientations
            for (i = 0; i < info->maxAttr; i++) {
                if (info->evalEnables & (1 << i)) {
                    NV_PATCH_MAP_INFO *map = &info->maps[i];
                    float *rawData = map->rawData;
                    int order = map->uorder;
                    int ufStride = map->ufStride;
                    int vfStride = map->vfStride;
                    int k, l;

                    nvAssert(info->maps[i].uorder == info->maps[i].vorder);

                    if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
                        // Compute FD matrix for each subpatch
                        info->reparam.rows = info->reparam.columns = order;
                        for (l = 0; l < order; l++) {
                            for (k = 0; k < order-l; k++) {
                                getRawData(info, &info->maps[i], rawData, l, k, info->reparam.data[l][k]);
                            }
                        }

                        ReparameterizeTriPatch(info, &info->reparam, quadInfo->m00[i], 0, 0);

                        if (vMaxDir == 1) {
                            if (uMaxDir == 1) {
                                ComputeFDMatrixFlip(info, 0, order, order, duMax, dvMax, quadInfo->m00[i],
                                    &quadInfo->m00[i]->data[0][0][0], 0, 0, 4, 4 * NV_PATCH_MAX_ORDER);
                            } else {
                                ComputeFDMatrixFlip(info, 0, order, order, duMax, dvMax, quadInfo->m00[i],
                                    &quadInfo->m00[i]->data[0][order-1][0], 0, 0, -4, 4 * NV_PATCH_MAX_ORDER);
                            }
                        } else {
                            if (uMaxDir == 1) {
                                ComputeFDMatrixFlip(info, 0, order, order, duMax, dvMax, quadInfo->m00[i],
                                    &quadInfo->m00[i]->data[order-1][0][0], 0, 0, 4, -4 * NV_PATCH_MAX_ORDER);
                            } else {
                                ComputeFDMatrixFlip(info, 0, order, order, duMax, dvMax, quadInfo->m00[i],
                                    &quadInfo->m00[i]->data[order-1][order-1][0], 0, 0, -4, -4 * NV_PATCH_MAX_ORDER);
                            }
                        }

                    } else if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_TESSELLATION) {
                        RetessellateFDMatrix(info, duMaxOld, dvMaxOld, duMax, dvMax, quadInfo->m00[i]);
                    }

                }
            }

            // Reverse transitions may cause stitches
            // If dir's are not equal, AND there are no stitches for other reasons,
            // AND it's not degenerate in this direction, then ...
            if ((quadInfo->u0Dir != quadInfo->u1Dir) && !stitchBottom && !stitchTop && (quadInfo->nu0 > 1)) {
                stitchTop = 1;          // make a stitch for the reversal
            }
            if ((quadInfo->v0Dir != quadInfo->v1Dir) && !stitchLeft && !stitchRight && (quadInfo->nv1 > 1)) {
                nvAssert(quadInfo->nv1 == quadInfo->nv0);
                stitchRight = 1;        // make a stitch for the reversal
            }

            // Decide where the stitching occurs w.r.t. the regular region
            if (uMaxDir == 1) {
                stitchUBegin = stitchLeft;
                stitchUEnd = stitchRight;
            } else {
                stitchUBegin = stitchRight;
                stitchUEnd = stitchLeft;
            }
            if (vMaxDir == 1) {
                stitchVBegin = stitchBottom;
                stitchVEnd = stitchTop;
            } else {
                stitchVBegin = stitchTop;
                stitchVEnd = stitchBottom;
            }

            quadInfo->stitchLeft   = stitchLeft;
            quadInfo->stitchRight  = stitchRight;
            quadInfo->stitchBottom = stitchBottom;
            quadInfo->stitchTop    = stitchTop;
            quadInfo->stitchUBegin = stitchUBegin;
            quadInfo->stitchUEnd   = stitchUEnd;
            quadInfo->stitchVBegin = stitchVBegin;
            quadInfo->stitchVEnd   = stitchVEnd;

            // calc if U/V inner guards required
            quadInfo->needUInner = (quadInfo->vMaxSegs > 1) && (stitchBottom || stitchTop);
            quadInfo->needVInner = (quadInfo->uMaxSegs > 1) && (stitchLeft || stitchRight);

            for (i = 0; i < info->maxAttr; i++) {
                if (info->evalEnables & (1 << i)) {
                    NV_PATCH_MAP_INFO *map = &info->maps[i];
                    float *rawData = map->rawData;
                    int uorder = map->uorder;
                    int vorder = map->vorder;

                    if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
                        // Store the corner points
                        getRawData(info, &info->maps[i], rawData, 0,        0,        quadInfo->pCorners[i][0][0]);
                        getRawData(info, &info->maps[i], rawData, 0,        uorder-1, quadInfo->pCorners[i][0][1]);
                        getRawData(info, &info->maps[i], rawData, vorder-1, 0,        quadInfo->pCorners[i][1][0]);

                        CopyPoint(quadInfo->pCorners[i][1][1], quadInfo->pCorners[i][1][0]);

                        // Compute U boundary curves
                        quadInfo->pU0[i]->order = uorder;
                        quadInfo->pU1[i]->order = uorder;
                        ComputeFDCurveCoeffsFlip(info, uorder, du0, &quadInfo->pU0[i]->coeffs[0][0],
                            pRawData(info, map, rawData, 0, 0),
                            info->maps[i].ufStride * sizeof(float),quadInfo->u0Dir, info->flipU);
                        ComputeFDCurveCoeffsFlip(info, uorder, du1, &quadInfo->pU1[i]->coeffs[0][0],
                            pRawData(info, map, rawData, vorder-1, 0),
                            0 /* stride at top */, quadInfo->u1Dir, info->flipU);

                        // Compute V boundary curves
                        quadInfo->pV0[i]->order = vorder;
                        quadInfo->pV1[i]->order = vorder;
                        ComputeFDCurveCoeffsFlip(info, vorder, dv0, &quadInfo->pV0[i]->coeffs[0][0],
                            pRawData(info, map, rawData, 0, 0),
                            info->maps[i].vfStride * sizeof(float), quadInfo->v0Dir, info->flipV);
                        ComputeFDCurveCoeffsFlip(info, vorder, dv1, &quadInfo->pV1[i]->coeffs[0][0],
                            pRawData(info, map, rawData, 0, uorder-1),
                            (info->maps[i].vfStride-4) * sizeof(float), quadInfo->v1Dir, info->flipV);

                        info->maps[i].dirtyFlags = 0;

                    } else if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_TESSELLATION) {
                        // Compute U boundary curves
                        RetessellateFDCurve(info, du0Old, du0, quadInfo->pU0[i]);
                        RetessellateFDCurve(info, du1Old, du1, quadInfo->pU1[i]);
                        // Compute V boundary curves
                        RetessellateFDCurve(info, dv0Old, dv0, quadInfo->pV0[i]);
                        RetessellateFDCurve(info, dv1Old, dv1, quadInfo->pV1[i]);

                        info->maps[i].dirtyFlags &= ~NV_PATCH_DIRTY_TESSELLATION;
                    }

                    // If we're stitching on the top or bottom, do a U inner guard
                    if (quadInfo->needUInner) {
// XXX maybe do this in lower routine ?
CopyMatrix(info, &info->tempMatrix, quadInfo->m00[i]);
RetessellateFDMatrix(info, 1.f, dvMax, 1.f, quadInfo->dvMax, &info->tempMatrix);    // big to small
                        if (stitchVBegin) {
                            OffsetFDMatrix_Extract_Discard(info, quadInfo->pUInner[i], 0,  &info->tempMatrix, 0, 1);
                        } else {
                            nvAssert(stitchVEnd);
                            OffsetFDMatrix_Extract_Discard(info, quadInfo->pUInner[i], 0,  &info->tempMatrix, 0, quadInfo->vMaxSegs-1);
                        }
                    }

                    // If we're stitching on the left or right, do a V inner guard
                    if (quadInfo->needVInner) {
// XXX maybe do this in lower routine ?
CopyMatrix(info, &info->tempMatrix, quadInfo->m00[i]);
RetessellateFDMatrix(info, duMax, 1.f, quadInfo->duMax, 1.f, &info->tempMatrix);    // big to small
                        if (stitchUBegin) {
                            OffsetFDMatrix_Extract_Discard(info, quadInfo->pVInner[i], 1,  &info->tempMatrix, 1, 0);
                        } else {
                            OffsetFDMatrix_Extract_Discard(info, quadInfo->pVInner[i], 1,  &info->tempMatrix, quadInfo->uMaxSegs-1, 0);
                        }
                    }
                }
            }

            retVal |= DrawIntPatch(info, quadInfo, quadInfo->m00);
        }
    }

    return retVal;
}

/*****************************************************************************/
int evAllocInitWithCache(NV_PATCH_INFO *info, void *context, NV_PATCH_ALLOC_CONTROL *pevMemory,
                            int tBytes, NV_PATCH_ALLOC_CACHE *pCache)
{
    nvAssert(!(tBytes & (sizeof(void *) - 1)));
    if (pCache && pCache->pBaseCache) {
        if (pCache->bytesAllocCache >= tBytes) {
            // requested allocation fit in cached allocation
            nvAssert(!pCache->inUse);
            nvAssert(pCache->contextCache == context);
            pCache->inUse = 1;
            pCache->countFreeCheck = 0;
            pevMemory->context = context;
            pevMemory->pBase = pevMemory->pAvail = pCache->pBaseCache;
            pevMemory->bytesAlloc = tBytes;
            pevMemory->bytesUsed = 0;
            pevMemory->pCache = pCache;
            return 0;
        }
        // need new alloc, free up the old
        MemFree(pCache->pBaseCache);
    }

    // must do new alloc
    pevMemory->context = context;
    pevMemory->pBase = MemAlloc(tBytes);
    pevMemory->pAvail = pevMemory->pBase;
    pevMemory->bytesAlloc = tBytes;
    pevMemory->bytesUsed = 0;
    pevMemory->pCache = pCache;

    if (pCache) {
        pCache->contextCache = context;
        pCache->pBaseCache = pevMemory->pBase;
        pCache->bytesAllocCache = tBytes;
        pCache->inUse = 1;
        pCache->countFreeCheck = 0;
    }
    if (pevMemory->pBase) {
        return 0;
    }
    return 1;
}

const int attrHwToSw[NV_PATCH_NUMBER_OF_ATTRIBS] = {
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15,
    };

const int attrSwToHw[NV_PATCH_NUMBER_OF_ATTRIBS] = {
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15,
    };

} // end of namespace
