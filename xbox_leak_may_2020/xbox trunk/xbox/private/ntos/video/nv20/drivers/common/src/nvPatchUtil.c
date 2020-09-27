
/*
 * nvPatchUtil.c
 *
 * Software emulation for curved surfaces.
 *
 * Copyright (c) 2000, Nvidia Corporation.  All rights reserved.
 */

// Constants defined in nvPatchConsts.c
extern float *bezBasis[16];
extern float *catmullRomBasis[16];
extern float *bsplineBasis[16];
extern float *invBezBasis[16];
extern float *powerToFD[16];
extern float *inversePowerToFD[16];
extern float cachedBinomialCoefficients[11][11];
static double factorial[1+NV_PATCH_MAX_RETESS];
static double oofactorial[1+NV_PATCH_MAX_RETESS];

#define DCR_COMPUTEFDMATRIXFLIP_OPT
#define DCR_COMPUTEFDCURVEFLIP_OPT
#define DCR_HOS_OPT
#if !defined(IS_OPENGL)
    //#define DCR_HOS_MATRIX_OPT
    #define DCR_HOS_RETESSCURVE_OPT     // XXX not yet thread safe for OpenGL
    #define DCR_HOS_RETESS_OPT          // XXX not yet thread safe for OpenGL
#endif

#if !defined(IS_OPENGL)
    #undef  NV_TRACE_COND
    #define NV_TRACE_COND(class, level, code)
    #undef  NV_TRACE_CODE
    #define NV_TRACE_CODE(code)
    #define TPRINTF(xx)
    #define EV_DRAW(code)
#ifdef MAX
#undef MAX
#endif
#ifdef MIN
#undef MIN
#endif
    #define MAX max
    #define MIN min

#else
    #define nvAssert assert
    #if defined(COMPILE_TRACE_LIBRARY)
        #define ENABLE_DEBUG_REVERSE_TRANSITIONS  // for testing reverse transitions
        #define ENABLE_DEBUG_SWATH // for testing swathing. I'll remove this when things are more stable. Thanks. Fred.
        static int evalTrace = 0;
        static int skipQuads = 0;   // set to draw only 1 quad of frac cases to simplify debugging
        static int retessit = 0;
        static int drawRef = 0;
        #define EV_DRAW(code) \
                if (drawRef  &&  (nvDebugLevel >= 2)) { \
                    { code; } \
                } \

    #else
        #define EV_DRAW(code)
    #endif
#endif

#define EV_GUARD_ATTR(index)        (((index) == NV_PATCH_ATTRIB_NORMAL) || ((index) == NV_PATCH_ATTRIB_POSITION))
#define EV_GUARD_BITS               ((1 << NV_PATCH_ATTRIB_NORMAL) | (1 << NV_PATCH_ATTRIB_POSITION))
#define EV_GUARD_BIT_VERTEX         ((1 << NV_PATCH_ATTRIB_POSITION))
#define EV_GUARD_BIT_NORMAL         ((1 << NV_PATCH_ATTRIB_NORMAL))

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

#define ASSERT_CURVE(PCURVE) nvAssert((PCURVE)->order > 0  &&  (PCURVE)->order < NV_PATCH_MAX_ORDER)
#define ASSERT_MATRIX(PMATRIX) nvAssert((PMATRIX)->rows > 0  &&  (PMATRIX)->rows < NV_PATCH_MAX_ORDER \
                                     && (PMATRIX)->cols > 0  &&  (PMATRIX)->cols < NV_PATCH_MAX_ORDER)

/*****************************************************************************/
#define EV_SWAP(a, b, t) { t = a; a = b; b = t; }

// index into curve*[]. For given attrib, and term of the curve equation, point to the 'X' of the attribute
static NV_INLINE int idxCurve(int attribIndex, int orderTerm)
{
    return (attribIndex*NV_PATCH_MAX_ORDER + orderTerm)*4;
}

static NV_INLINE float *pRawData(NV_PATCH_INFO *info, NV_PATCH_MAP_INFO *map, float *rawData, int vCoord, int uCoord)
{
    int uOrder = map->uorder;//, ii;
    int vOrder = map->vorder;
    int ufStride = map->ufStride;
    int vfStride = map->vfStride;

    //ffif (info->flipT) {
        //ffEV_SWAP(uOrder, vOrder, ii);
        //ffEV_SWAP(ufStride, vfStride, ii);
    //ff}
    if (info->flipV) {
        vCoord = vOrder - 1 - vCoord;
    }
    if (info->flipU) {
        uCoord = uOrder - 1 - uCoord;
    }
    return (rawData + vCoord * vfStride + uCoord * ufStride);
}

static NV_INLINE void getRawData(NV_PATCH_INFO *info, NV_PATCH_MAP_INFO *map, float *rawData, int vCoord, int uCoord, float *pOut)
{
    float *data = pRawData(info, map, rawData, vCoord, uCoord);
    pOut[0] = data[0]; pOut[1] = data[1]; pOut[2] = data[2]; pOut[3] = data[3];

}

/*****************************************************************************/
#if defined(COMPILE_TRACE_LIBRARY)

void TraceMatrix(NV_PATCH_INFO *info, const FDMatrix *m);

static float approxOK = 1E-5;
#define EV_ABS(x)       ((x) >=  0  ? (x) : -(x))
#define EV_MIN(x, y)    ((x) < (y) ? (x) : (y))
#define EV_MAX(x, y)    ((x) > (y) ? (x) : (y))

static NV_INLINE int FloatsIdentical(float src, float src2)
{
    if (EV_AS_INT(src) == EV_AS_INT(src2)) {
        return 1;
    }

    TPRINTF(("src=%g=x%x src2=%g=x%x\n", src, *(int *)&src, src2, *(int *)&src2));
    nvAssert(("FLOATS NOT IDENTICAL", 0));
    return 0;
}

static NV_INLINE int DoublesIdentical(double src, double src2)
{
    int *pSrc = (int *)&src;
    int *pSrc2 = (int *)&src2;

    if (pSrc[0] == pSrc2[0]  &&  pSrc[1] == pSrc2[1]) {
        return 1;
    }

    TPRINTF(("src=%g=x%X x%08X src2=%g=x%X x%08X\n", src, pSrc[0], pSrc[1], src2, pSrc2[0], pSrc2[1]));
    nvAssert(("DOUBLES NOT IDENTICAL", 0));
    return 0;
}

static NV_INLINE int FloatsEqual(float src, float src2)
{
    float fmin = EV_MIN(EV_ABS(src), EV_ABS(src2));
    float fmax = EV_MAX(EV_ABS(src), EV_ABS(src2));
    float fok = approxOK * fmax;

    if (src == 0.f) {
        if (fmax < approxOK) {
            return 1;
        }
        return 0;
    }
    if (src2 == 0.f) {
        if (fmax < approxOK) {
            return 1;
        }
        return 0;
    }
    if (EV_ABS(src - src2) <= fok) {
        return 1;
    }
    TPRINTF(("src=%g=x%x src2=%g=x%x\n", src, *(int *)&src, src2, *(int *)&src2));
    nvAssert(("FLOATS NOT EQUAL", 0));
    return 0;
}

static NV_INLINE int PointsEqual(float *src, float *src2)
{
    int ret = FloatsEqual(src[0], src2[0]);

    ret = ret && FloatsEqual(src[1], src2[1]);
    ret = ret && FloatsEqual(src[2], src2[2]);
    ret = ret && FloatsEqual(src[3], src2[3]);
    return ret;
}

// Compare the U curve from a FDMatrix to a curve
void CompareUCurveMatrix(NV_PATCH_CURVE_INFO *dst, FDMatrix *m)
{
    int i, stride = 4*sizeof(float);
    float *data = dst->coeffs[0];

    nvAssert(dst->order == m->columns);
    // Empty data
    for (i = 0; i < dst->order; i++) {
        // Compare data
        PointsEqual(data, m->data[0][i]);

        // Step pointer
        data = (float *)((char *)data + stride);
    }
}

static NV_INLINE void CompareCurves(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *src, NV_PATCH_CURVE_INFO *src2)
{
    int i;

    nvAssert(src->order == src2->order);
    for (i = 0;  i < src->order;  i++) {
        PointsEqual(src->coeffs[i], src2->coeffs[i]);
    }
}

static NV_INLINE int CompareMatrices(NV_PATCH_INFO *info, FDMatrix *src, FDMatrix *src2)
{
    int i, j, ret = 1;

    nvAssert(src->rows == src2->rows);
    nvAssert(src->columns == src2->columns);
    for (i=0; i < src->rows; i++) {
        for (j = 0; j < src->columns; j++) {
            ret = ret && FloatsEqual(src->data[i][j][0], src2->data[i][j][0]);
            ret = ret && FloatsEqual(src->data[i][j][1], src2->data[i][j][1]);
            ret = ret && FloatsEqual(src->data[i][j][2], src2->data[i][j][2]);
            ret = ret && FloatsEqual(src->data[i][j][3], src2->data[i][j][3]);
        }
    }
    if (!ret) {
        TPRINTF(("BAD MATRIX COMPARE\n"));
        TraceMatrix(info, src);
        TraceMatrix(info, src2);
    }
    return ret;
}

void CheckCache(NV_PATCH_INFO *info, FDMatrix *src)
{
    int i, j;
    double sum;

    for (i=0; i < src->rows; i++) {
        sum = 0.;
        for (j = 0; j < src->columns; j++) {
            FloatsIdentical(src->data[i][j][0], src->data[i][j][1]);
            FloatsIdentical(src->data[i][j][0], src->data[i][j][2]);
            FloatsIdentical(src->data[i][j][0], src->data[i][j][3]);
            sum += src->data[i][j][0];
        }
        TPRINTF(("CheckCache %2d) sum=%g\n", i, sum));
        if (!i) {
            FloatsEqual(sum, 1.f);
        } else {
            FloatsEqual(sum, 0.f);
        }
    }
}

void CheckCached(NV_PATCH_INFO *info, FDMatrixd *src)
{
    int i, j;
    double sum;

    for (i=0; i < src->rows; i++) {
        sum = 0.;
        for (j = 0; j < src->columns; j++) {
            DoublesIdentical(src->data[i][j][0], src->data[i][j][1]);
            DoublesIdentical(src->data[i][j][0], src->data[i][j][2]);
            DoublesIdentical(src->data[i][j][0], src->data[i][j][3]);
            sum += src->data[i][j][0];
        }
        TPRINTF(("CheckCache %2d) sum=%g\n", i, sum));
        if (!i) {
            //ffFloatsEqual(sum, 1.f);
            assert(sum == 1.);
        } else {
            //ffFloatsEqual(sum, 0.f);
            assert(sum == 0.);
        }
    }
}

#endif  // defined(COMPILE_TRACE_LIBRARY)

/*****************************************************************************/
// Print a matrix for tracing purposes
static int traceHex = 0;
static int traceWrap = 1;
void TraceMatrix(NV_PATCH_INFO *info, const FDMatrix *m)
{
    int row, col;
#if !defined(IS_OPENGL)
    DPF_LEVEL(NVDBG_LEVEL_HOS_INFO, "%dx%d matrix:\n", m->rows, m->columns);
    for (row = 0; row < m->rows; row++) {
        DPF_LEVEL(NVDBG_LEVEL_HOS_INFO, "  ");
        for (col = 0; col < m->columns; col++) {
            DPF_LEVEL(NVDBG_LEVEL_HOS_INFO, "[%f %f %f %f] ",
                m->data[row][col][0], m->data[row][col][1],
                m->data[row][col][2], m->data[row][col][3]);
        }
        DPF_LEVEL(NVDBG_LEVEL_HOS_INFO, "\n");
    }
#else
    NV_TRACE(TR_EVAL, 25, ("%dx%d matrix:\n", m->rows, m->columns));
    for (row = 0; row < m->rows; row++) {
        NV_TRACE(TR_EVAL, 25, ("row%d:", row));
        for (col = 0; col < m->columns; col++) {
            NV_TRACE(TR_EVAL, 25, ("%s%2d) [%13g %13g %13g %13g]%s",
                col && (m->columns > traceWrap) ? "      " : " ", col,
                m->data[row][col][0], m->data[row][col][1],
                m->data[row][col][2], m->data[row][col][3],
                m->columns > traceWrap ? "\n" : ""));
            if (traceHex) {
                NV_TRACE(TR_EVAL, 85, ("%2d) [x%08X x%08X x%08X x%08X] ", col,
                    *(const int *)&m->data[row][col][0], *(const int *)&m->data[row][col][1],
                    *(const int *)&m->data[row][col][2], *(const int *)&m->data[row][col][3]));
            }
        }
        NV_TRACE(TR_EVAL, 25, ("\n"));
    }
#endif
}

void TraceCurve(NV_PATCH_INFO *info, const NV_PATCH_CURVE_INFO *pCurve, char *pMess)
{
    int i;

    TPRINTF(("Curve '%s':\n", pMess));

    for (i = 0;  i < pCurve->order;  i++) {
        TPRINTF(("%2d) [%13g %13g %13g %13g]\n", i, pCurve->coeffs[i][0],
                pCurve->coeffs[i][1], pCurve->coeffs[i][2], pCurve->coeffs[i][3]));
    }
}

/*****************************************************************************/
NV_INLINE void CopyPoint(float *dst, const float *src)
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

static NV_INLINE void AddPoint(float *dst, float *src)
{
    dst[0] += src[0];
    dst[1] += src[1];
    dst[2] += src[2];
    dst[3] += src[3];
}

static NV_INLINE void AddPoint3(float *dst, float *src1, float *src2)
{
    dst[0] = src1[0] + src2[0];
    dst[1] = src1[1] + src2[1];
    dst[2] = src1[2] + src2[2];
    dst[3] = src1[3] + src2[3];
}

/*****************************************************************************/
static NV_INLINE void CopyCoeffs(NV_PATCH_INFO *info, NV_PATCH_CURVE_COEFFS *dst, NV_PATCH_CURVE_COEFFS *src)
// copy curve coeffs. Does not copy 'order'. May be used to copy matrix rows to curve coeffs.
{
    __NV_MEMCPY(dst, src, 4*info->maxOrder*sizeof(float));
}

void CopyCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *dst, NV_PATCH_CURVE_INFO *src)
// copy curve, including order
{
#ifdef DCR_HOS_OPT
    float *srcptr, *dstptr;

    ASSERT_CURVE(src);
    switch (src->order) {
    case 2:
        dst->order = src->order;
        srcptr = (float*)(&src->coeffs[0][0]); dstptr = (float*)(&dst->coeffs[0][0]);
        dstptr[0] = srcptr[0]; dstptr[1] = srcptr[1]; dstptr[2] = srcptr[2]; dstptr[3] = srcptr[3];  //v0-xyzw
        dstptr[4] = srcptr[4]; dstptr[5] = srcptr[5]; dstptr[6] = srcptr[6]; dstptr[7] = srcptr[7];  //v1-xyzw
        return;
    case 4:
        dst->order = src->order;
        srcptr = (float*)(&src->coeffs[0][0]); dstptr = (float*)(&dst->coeffs[0][0]);
        dstptr[0] = srcptr[0]; dstptr[1] = srcptr[1]; dstptr[2] = srcptr[2]; dstptr[3] = srcptr[3];  //v0-xyzw
        dstptr[4] = srcptr[4]; dstptr[5] = srcptr[5]; dstptr[6] = srcptr[6]; dstptr[7] = srcptr[7];  //v1-xyzw
        dstptr[8] = srcptr[8]; dstptr[9] = srcptr[9]; dstptr[10] = srcptr[10]; dstptr[11] = srcptr[11];  //v2-xyzw
        dstptr[12] = srcptr[12]; dstptr[13] = srcptr[13]; dstptr[14] = srcptr[14]; dstptr[15] = srcptr[15];  //v3-xyzw
        return;
    case 6:
        dst->order = src->order;
        srcptr = (float*)(&src->coeffs[0][0]); dstptr = (float*)(&dst->coeffs[0][0]);
        dstptr[0] = srcptr[0]; dstptr[1] = srcptr[1]; dstptr[2] = srcptr[2]; dstptr[3] = srcptr[3];  //v0-xyzw
        dstptr[4] = srcptr[4]; dstptr[5] = srcptr[5]; dstptr[6] = srcptr[6]; dstptr[7] = srcptr[7];  //v1-xyzw
        dstptr[8] = srcptr[8]; dstptr[9] = srcptr[9]; dstptr[10] = srcptr[10]; dstptr[11] = srcptr[11];  //v2-xyzw
        dstptr[12] = srcptr[12]; dstptr[13] = srcptr[13]; dstptr[14] = srcptr[14]; dstptr[15] = srcptr[15];  //v3-xyzw
        dstptr[16] = srcptr[16]; dstptr[17] = srcptr[17]; dstptr[18] = srcptr[18]; dstptr[19] = srcptr[19];  //v4-xyzw
        dstptr[20] = srcptr[20]; dstptr[21] = srcptr[21]; dstptr[22] = srcptr[22]; dstptr[23] = srcptr[23];  //v5-xyzw
        return;
     default:
        ;   // fall thru to C code
    }
#endif

    ASSERT_CURVE(src);
    __NV_MEMCPY(dst, src, PATCH_CURVE_HEADER_SIZE + 4*info->maxOrder*sizeof(float));
}

static NV_INLINE void CopyCurveSet(NV_PATCH_INFO *info, int evalEnables, FDCurveAttrSet *dst, FDCurveAttrSet *src)
{
    int index;
    for (index = 0; index < info->maxAttr; index++) {
        if (evalEnables & (1 << index)) {
            CopyCurve(info, (*dst)[index], (*src)[index]);
        }
    }
}

/*****************************************************************************/
// copy one matrix
static NV_INLINE void CopyMatrix(NV_PATCH_INFO *info, FDMatrix *dst, const FDMatrix *src)
{
#ifdef DCR_HOS_OPT
    int i;
    float *srcptr, *dstptr;
#if !defined(IS_OPENGL)
    switch (src->rows) {
#else
    switch (MAX(src->rows, src->columns)) { // }
#endif
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
        return;
    case 4:
        dst->rows = src->rows;
        dst->columns = src->columns;
        srcptr = (float*)(&src->data[0][0][0]);
        dstptr = (float*)(&dst->data[0][0][0]);
        for (i=0; i < 4; i++) {
            dstptr[0] = srcptr[0]; dstptr[1] = srcptr[1]; dstptr[2] = srcptr[2]; dstptr[3] = srcptr[3];  //v0-xyzw
            dstptr[4] = srcptr[4]; dstptr[5] = srcptr[5]; dstptr[6] = srcptr[6]; dstptr[7] = srcptr[7];  //v1-xyzw
            dstptr[8] = srcptr[8]; dstptr[9] = srcptr[9]; dstptr[10] = srcptr[10]; dstptr[11] = srcptr[11];  //v2-xyzw
            dstptr[12] = srcptr[12]; dstptr[13] = srcptr[13]; dstptr[14] = srcptr[14]; dstptr[15] = srcptr[15];  //v3-xyzw
            dstptr+=16*4; srcptr+=16*4;                                                                  //skip to next row
        }
        return;
    case 6:
        dst->rows = src->rows;
        dst->columns = src->columns;
        srcptr = (float*)(&src->data[0][0][0]);
        dstptr = (float*)(&dst->data[0][0][0]);
        for (i=0; i < 6; i++) {
            dstptr[0] = srcptr[0]; dstptr[1] = srcptr[1]; dstptr[2] = srcptr[2]; dstptr[3] = srcptr[3];  //v0-xyzw
            dstptr[4] = srcptr[4]; dstptr[5] = srcptr[5]; dstptr[6] = srcptr[6]; dstptr[7] = srcptr[7];  //v1-xyzw
            dstptr[8] = srcptr[8]; dstptr[9] = srcptr[9]; dstptr[10] = srcptr[10]; dstptr[11] = srcptr[11];  //v2-xyzw
            dstptr[12] = srcptr[12]; dstptr[13] = srcptr[13]; dstptr[14] = srcptr[14]; dstptr[15] = srcptr[15];  //v3-xyzw
            dstptr[16] = srcptr[16]; dstptr[17] = srcptr[17]; dstptr[18] = srcptr[18]; dstptr[19] = srcptr[19];  //v4-xyzw
            dstptr[20] = srcptr[20]; dstptr[21] = srcptr[21]; dstptr[22] = srcptr[22]; dstptr[23] = srcptr[23];  //v5-xyzw
            dstptr+=16*4; srcptr+=16*4;                                                                  //skip to next row
        }
        return;
    default:
        ;   // fall thru to C code
    }
#endif

    __NV_MEMCPY(dst, src, sizeof(FDMatrix));
}

static NV_INLINE void CopyMatrixTranspose(NV_PATCH_INFO *info, FDMatrix *dst, FDMatrix *src)
{
    int i,j;
    //THIS NEEDS TO BE IMPROVED!!!
    dst->rows = src->columns;
    dst->columns = src->rows;
    for (i=0; i < src->rows; i++) {
        for (j = 0; j < src->columns; j++) {
            dst->data[j][i][0] = src->data[i][j][0];
            dst->data[j][i][1] = src->data[i][j][1];
            dst->data[j][i][2] = src->data[i][j][2];
            dst->data[j][i][3] = src->data[i][j][3];
        }
    }
}

static NV_INLINE void CopyMatrixSet(NV_PATCH_INFO *info, int evalEnables, FDMatrixAttrSet *dst, FDMatrixAttrSet *src)
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
void MatrixMult(NV_PATCH_INFO *info, FDMatrix *c, const FDMatrix *a, const FDMatrix *b)
{
    int row, col, term;

    NV_TRACE_COND(TR_EVAL, 55,
        TPRINTF(("MatrixMult:\n"));
        TraceMatrix(info, a);
        TraceMatrix(info, b);
    );

    c->rows    = a->rows;
    c->columns = b->columns;
    nvAssert(a->columns == b->rows);

#ifdef DCR_HOS_MATRIX_OPT
    if (g_FDGlobalData.cpuType) {
        switch (a->columns) {
        ??? what is this next line? Fred.
        PFMM p;
        case 4:
            p=(PFMM)(g_FDGlobalData.HOSInnerLoops[OFFSET_MATRIX_MULT_4x4]);
            p(c, (FDMatrix*)a, (FDMatrix*)b);
            return;
        case 6:
            p=(PFMM)(g_FDGlobalData.HOSInnerLoops[OFFSET_MATRIX_MULT_6x6]);
            p(c, (FDMatrix*)a, (FDMatrix*)b);
            return;
        case 2:
            //OFFSET_MATRIX_MULT_2x2
        default:
            //OFFSET_MATRIX_MULT_DEF
            ;   // fall thru to C code
        }
    }
#endif
    {
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
    NV_TRACE_COND(TR_EVAL, 45,
        TPRINTF(("... result:\n"));
        TraceMatrix(info, c);
    );
}

/*****************************************************************************/
// Fill in a column vector matrix using a sequence of strided xyzw values
void FillColumnVectorMatrix(int n, int stride, const float *data, FDMatrix *m, int col)
{
    int i;

    // nx1 matrix
    m->rows = n;
    //this implicitly assumes that  we are filling this one column at a time
    //such that we have the right value when we are done (i.e. the calls are made)
    //col = 1... n whe col=n is the LAST call.
    nvAssert(!col  ||  (col == m->columns));
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
static FDMatrix bigToSmallMatrix[NV_PATCH_MAX_RETESS];
static FDMatrix smallToBigMatrix[NV_PATCH_MAX_RETESS];
static int retessGuard = 0xfeed1;

/*****************************************************************************/
// Compute FD coefficients from curve control points
void RetessCurveInOut(NV_PATCH_INFO *info, int smallToBig, NV_PATCH_CURVE_INFO *curve, NV_PATCH_CURVE_INFO *curveOut)
{
    FDMatrix *temp[3];

    ASSERT_CURVE(curve);
    BEGIN_USING_TEMP_MATRIX(info->tempMatrix_m0);
    BEGIN_USING_TEMP_MATRIX(info->tempMatrix_m1);
    temp[0] = &info->tempMatrix_m0;
    temp[1] = &info->tempMatrix_m1;

    if (smallToBig) {
        temp[2] = &smallToBigMatrix[curve->order];
    } else {
        temp[2] = &bigToSmallMatrix[curve->order];
    }

    // Compute FD curve coefficients
    FillColumnVectorMatrix(curve->order, 4*sizeof(float), &curve->coeffs[0][0], temp[0], 0);
    MatrixMult(info, temp[1], temp[2], temp[0]);
    EmptyColumnVectorMatrix(curve->order, temp[1], 4*sizeof(float), &curveOut->coeffs[0][0], 0);

    curveOut->order = curve->order;
    END_USING_TEMP_MATRIX(info->tempMatrix_m0);
    END_USING_TEMP_MATRIX(info->tempMatrix_m1);
}

// Compute FD coefficients from curve control points
void RetessCurveSetInOut(NV_PATCH_INFO *info, int smallToBig, int evalEnables, FDCurveAttrSet *pIn, FDCurveAttrSet *pOut)
{
    int attr;

    for (attr = 0;  evalEnables;  attr++, evalEnables >>=1 ) {
        if (evalEnables & 1) {
            RetessCurveInOut(info, smallToBig, (*pIn)[attr], (*pOut)[attr]);
        }
    }
}

void RetessCurve(NV_PATCH_INFO *info, int smallToBig, NV_PATCH_CURVE_INFO *curve)
{
    RetessCurveInOut(info, smallToBig, curve, curve);
}

/*****************************************************************************/
void RetessMatrixU(NV_PATCH_INFO *info, int smallToBig, FDMatrix *matrix)
{
    FDMatrix *temp[3];

    BEGIN_USING_TEMP_MATRIX(info->tempMatrix_m0);
    BEGIN_USING_TEMP_MATRIX(info->tempMatrix_m1);
    temp[0] = &info->tempMatrix_m0;
    temp[1] = &info->tempMatrix_m1;

    if (smallToBig) {
        temp[2] = &smallToBigMatrix[matrix->columns];
    } else {
        temp[2] = &bigToSmallMatrix[matrix->columns];
    }

    // note: if we had MultTransposeMatrix then we could use the identity
    // transpose(AB) = transpose(B) * transpose(A)
    // and avoid a copy step
    CopyMatrixTranspose(info, temp[0], matrix);
    MatrixMult(info, temp[1], temp[2], temp[0]);
    CopyMatrixTranspose(info, matrix, temp[1]);
    END_USING_TEMP_MATRIX(info->tempMatrix_m0);
    END_USING_TEMP_MATRIX(info->tempMatrix_m1);
}

void RetessMatrixV(NV_PATCH_INFO *info, int smallToBig, FDMatrix *matrix)
{

    FDMatrix *temp[3];

    BEGIN_USING_TEMP_MATRIX(info->tempMatrix_m0);
    BEGIN_USING_TEMP_MATRIX(info->tempMatrix_m1);
    temp[0] = &info->tempMatrix_m0;
    temp[1] = &info->tempMatrix_m1;
    if (smallToBig) {
        temp[2] = &smallToBigMatrix[matrix->rows];
    } else {
        temp[2] = &bigToSmallMatrix[matrix->rows];
    }

    // don't need a copy cause its not a transpose...
    MatrixMult(info, temp[1], temp[2], matrix);
    CopyMatrix(info, matrix, temp[1]);
    END_USING_TEMP_MATRIX(info->tempMatrix_m0);
    END_USING_TEMP_MATRIX(info->tempMatrix_m1);
}

void RetessMatrixUV(NV_PATCH_INFO *info, int smallToBig, FDMatrix *matrix)
{
    RetessMatrixU(info, smallToBig, matrix);
    RetessMatrixV(info, smallToBig, matrix);
}

void RetessMatrixSetU(NV_PATCH_INFO *info, int smallToBig, int evalEnables, FDMatrixAttrSet *pSet)
{
    int attr;

    for (attr = 0;  evalEnables;  attr++, evalEnables >>=1 ) {
        if (evalEnables & 1) {
            RetessMatrixU(info, smallToBig, (*pSet)[attr]);
        }
    }
}

void RetessMatrixSetV(NV_PATCH_INFO *info, int smallToBig, int evalEnables, FDMatrixAttrSet *pSet)
{
    int attr;

    for (attr = 0;  evalEnables;  attr++, evalEnables >>=1 ) {
        if (evalEnables & 1) {
            RetessMatrixV(info, smallToBig, (*pSet)[attr]);
        }
    }
}

void RetessMatrixSetUV(NV_PATCH_INFO *info, int smallToBig, int evalEnables, FDMatrixAttrSet *pSet)
{
    int attr;

    for (attr = 0;  evalEnables;  attr++, evalEnables >>=1 ) {
        if (evalEnables & 1) {
            RetessMatrixU(info, smallToBig, (*pSet)[attr]);
            RetessMatrixV(info, smallToBig, (*pSet)[attr]);
        }
    }
}

/*****************************************************************************/
/*****************************************************************************/
// Compare two vertices and return an edge direction
// a->b is 1, b->a is 0
static int CompareVertices(const float *a, const float *b)
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

#if defined(ENABLE_DEBUG_REVERSE_TRANSITIONS)
void
setDebugColors(int tri, NV_PATCH_INFO *info, NV_PATCH_MAP_INFO *map, int *u0Dir, int *u1Dir, int *v0Dir, int *v1Dir)
{
    int uorder = map->uorder;
    int vorder = map->vorder;
    int ufStride = map->ufStride;
    int vfStride = map->vfStride;
    float *rawData = map->rawData;
    float *pf;

    // test to force reverse transitions whether colors altered or not
    if (nvDebugOptions & TRO_TEMP1) {
        *u0Dir ^= 1;
    }
    if (nvDebugOptions & TRO_TEMP2) {
        *u1Dir ^= 1;
    }
    if (nvDebugOptions & TRO_TEMP3) {
        *v0Dir ^= 1;
    }
    if (nvDebugOptions & TRO_TEMP4) {
        *v1Dir ^= 1;
    }

    if (uorder < 4  ||  vorder < 4) {
        return;
    }

    if (nvDebugOptions & TRO_TEMP1) {
        pf = pRawData(info, map, rawData, 0, 1);
        pf[0] = 9.f; pf[1] = 0.f; pf[2] = 0.f; pf[3] = 1.f;
        pf += ufStride;
        pf[0] = 9.f; pf[1] = 0.f; pf[2] = 0.f; pf[3] = 1.f;
    }

    if (nvDebugOptions & TRO_TEMP2) {
        pf = pRawData(info, map, rawData, vorder-1, 1);
        pf[0] = 0.f; pf[1] = 9.f; pf[2] = 0.f; pf[3] = 1.f;
        pf += ufStride;
        pf[0] = 0.f; pf[1] = 9.f; pf[2] = 0.f; pf[3] = 1.f;
    }

    if (nvDebugOptions & TRO_TEMP3) {
        pf = pRawData(info, map, rawData, 1, 0);
        pf[0] = 0.f; pf[1] = 0.f; pf[2] = 9.f; pf[3] = 1.f;
        pf += vfStride;
        pf[0] = 0.f; pf[1] = 0.f; pf[2] = 9.f; pf[3] = 1.f;
    }

    if (nvDebugOptions & TRO_TEMP4) {
        pf = pRawData(info, map, rawData, 1, uorder-1-tri);
        pf[0] = 9.f; pf[1] = 0.f; pf[2] = 9.f; pf[3] = 1.f;
        pf += vfStride;
        if (tri) {
            pf -= 4;
        }
        pf[0] = 9.f; pf[1] = 0.f; pf[2] = 9.f; pf[3] = 1.f;
    }
}
#endif

// Compute the directions of the four edges of a patch
void ComputeEdgeDirections(NV_PATCH_INFO *info, int tri, NV_PATCH_MAP_INFO *map,
                           int *u0Dir, int *u1Dir, int *v0Dir, int *v1Dir)
{
    int uorder = map->uorder;
    int vorder = map->vorder;
    float *rawData = map->rawData;
    const float *p00, *p10, *p01, *p11; //p[x][y]

    if (rawData == NULL) {
        // assuming that unenabled maps have a null pointer
        // if we don't have a position punt for now and assume no reversals
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

#if defined(ENABLE_DEBUG_REVERSE_TRANSITIONS)
    NV_TRACE_COND(TR_EVAL, 1,
        setDebugColors(tri, info, &map[3], u0Dir, u1Dir, v0Dir, v1Dir);
    );
    NV_TRACE_COND(TR_EVAL, 20,
        if (nvDebugOptions & TRO_TEMP1) { TPRINTF(("Change u0dir to %d\n", *u0Dir)); }
        if (nvDebugOptions & TRO_TEMP2) { TPRINTF(("Change u1dir to %d\n", *u1Dir)); }
        if (nvDebugOptions & TRO_TEMP3) { TPRINTF(("Change v0dir to %d\n", *v0Dir)); }
        if (nvDebugOptions & TRO_TEMP4) { TPRINTF(("Change v1dir to %d\n", *v1Dir)); }
    );
#endif
}

void ComputeEdgeDirectionsFromCorners(NV_PATCH_INFO *info, int tri,
                           int *u0Dir, int *u1Dir, int *v0Dir, int *v1Dir)
{
    const float *p00, *p10, *p01, *p11; //p[x][y]

    //cornAttr[attrib][v][u][xyzw]
    p00 = &(info->quadInfo->cornAttr[0].corners[0][0][0]); //    p00 = pRawData(info, map, rawData, 0,        0);
    p10 = &(info->quadInfo->cornAttr[0].corners[0][1][0]); //    p10 = pRawData(info, map, rawData, 0,        uorder-1);
    p01 = &(info->quadInfo->cornAttr[0].corners[1][0][0]); //    p01 = pRawData(info, map, rawData, vorder-1, 0       );
    p11 = &(info->quadInfo->cornAttr[0].corners[1][1][0]); //    p11 = pRawData(info, map, rawData, vorder-1, uorder-1);

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

#if defined(ENABLE_DEBUG_REVERSE_TRANSITIONS)
    nvAssert(0); //shouldn't ever really get this in OGL without Cached patches which they don't have
/*
    NV_TRACE_COND(TR_EVAL, 1,
        setDebugColors(tri, info, &map[3], u0Dir, u1Dir, v0Dir, v1Dir);
    );
    NV_TRACE_COND(TR_EVAL, 20,
        if (nvDebugOptions & TRO_TEMP1) { TPRINTF(("Change u0dir to %d\n", *u0Dir)); }
        if (nvDebugOptions & TRO_TEMP2) { TPRINTF(("Change u1dir to %d\n", *u1Dir)); }
        if (nvDebugOptions & TRO_TEMP3) { TPRINTF(("Change v0dir to %d\n", *v0Dir)); }
        if (nvDebugOptions & TRO_TEMP4) { TPRINTF(("Change v1dir to %d\n", *v1Dir)); }
    );
*/
#endif
}


#if defined(ENABLE_DEBUG_SWATH) && defined(IS_OPENGL)
/*****************************************************************************/
int onColor = 0;
float evalColors[8][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
        {0, 1, 1},
        {1, 0, 1},
        {1, 1, 0},
        {1, 1, 1},
        {0, 0, 0},
    };
void evalDrawCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *pCurve, int scaleDown)
{
    int ii, lColor;
    NV_PATCH_CURVE_INFO curve;

    if (scaleDown & 8) {
        scaleDown = (info->swatchFlags & NV_PATCH_SWATCH);
    }
    glColor3fv(evalColors[(onColor++) & 0x7]);
    CopyCurve(info, &curve, pCurve);
    if ((info->swatchFlags & NV_PATCH_SWATCH)  &&  scaleDown) {
        RetessCurveInOut(info, 0, &curve, &curve);
    }
    glBegin(GL_LINE_STRIP);
    for (ii = 0;  ii < 4;  ii++) {
        glVertex3fv(curve.coeffs[0]);
        OffsetCurve(info, &curve, 1);
    }
    glEnd();

    CopyCurve(info, &curve, pCurve);
    if (scaleDown) {
        RetessCurveInOut(info, 0, &curve, &curve);
    }
    lColor = 0;
    glPointSize(8.f);
    glBegin(GL_POINTS);
    for (ii = 0;  ii < 4;  ii++) {
        glColor3fv(evalColors[(lColor++) & 0x7]);
        glVertex3fv(curve.coeffs[0]);
        OffsetCurve(info, &curve, 1);
    }
    glEnd();
}

void evalDrawMatrix(NV_PATCH_INFO *info, const FDMatrix *m, int bigU, int bigV)
{
    NV_PATCH_CURVE_INFO curveU, curveV;
    FDMatrix lm;
    int row;

    ExtractUCurve(info, &curveU, m);
    ExtractVCurve(info, &curveV, m);

    evalDrawCurve(info, &curveU, bigU);

    evalDrawCurve(info, &curveV, bigV);

    CopyMatrix(info, &lm, m);

    if (bigV & 8) {
        bigV = (info->swatchFlags & NV_PATCH_SWATCH);
    }
    if (bigV) {
        RetessMatrixV(info, 0, &lm);
    }
    for (row = 0;  row < 3;  row++) {
        OffsetFDMatrix(info, &lm, 0, 1);
        ExtractUCurve(info, &curveU, &lm);
        evalDrawCurve(info, &curveU, bigU);
    }
}
#endif  // defined(ENABLE_DEBUG_SWATH) && defined(IS_OPENGL)

/*****************************************************************************/
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

    if (!matrix) { nvAssert(0); return; }

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

// Compute the matrix to convert from a power basis to an FD basis
void ComputePowerToFD(int order, FDMatrix *m, int flags)
{
    int row, col;
    float *matrix;

    // nxn matrix where n = order
    m->rows = m->columns = order;

    if (flags & 1) {
        matrix = inversePowerToFD[order-1];
    } else {
        matrix = powerToFD[order-1];
    }

    // Load matrix
    for (row = 0; row < order; row++) {
        for (col = 0; col < order; col++) {
            float coeff = *matrix++;
            if (flags & 2) {
                coeff *= oofactorial[row];
            }
            m->data[row][col][0] = coeff;
            m->data[row][col][1] = coeff;
            m->data[row][col][2] = coeff;
            m->data[row][col][3] = coeff;
        }
    }
}

/*****************************************************************************/
// Extract the U curve from a FDMatrix into a curve
void ExtractUCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *dst, const FDMatrix *m)
{
    dst->order = m->columns;
    __NV_MEMCPY(dst->coeffs[0], m->data[0][0], 4*m->columns*sizeof(float));
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
void ExtractVCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *dst, const FDMatrix *m)
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
extern const double *evalRetessTables[NV_PATCH_MAX_RETESS];
void evalFillPower(double r, double *prPower)
{
    prPower[1] = r;
    prPower[2] = r*r;
    prPower[3] = prPower[2] * r;
    prPower[4] = prPower[2] * prPower[2];
    prPower[5] = prPower[4] * r;
    prPower[6] = prPower[4] * prPower[2];
    prPower[7] = prPower[4] * prPower[3];
    prPower[8] = prPower[4] * prPower[4];
    prPower[9] = prPower[8] * r;
    prPower[10] = prPower[8] * prPower[2];
    prPower[11] = prPower[8] * prPower[3];
    prPower[12] = prPower[8] * prPower[4];
    prPower[13] = prPower[8] * prPower[5];
    prPower[14] = prPower[8] * prPower[6];
    prPower[15] = prPower[8] * prPower[7];
    //ffprPower[16] = prPower[8] * prPower[8];
    //ffprPower[17] = prPower[16] * r;
    //ffprPower[18] = prPower[16] * prPower[2];
}

void evalFillOneRetessMatrix(NV_PATCH_INFO *info, double *prPower, int order, FDMatrix *dst)
{
    int elem, idx, row, col;
    double dd;
    const double *prTable;

    (*dst).rows = order;
    (*dst).columns = order;
    for (row = 0;  row < order;  row++) {
        for (col = 0;  col < order;  col++) {
            idx = order * (order * row + col);
            prTable = &evalRetessTables[order][idx];
            dd = 0.;
            for (elem = 0;  elem < order;  elem++) {
                dd += prPower[elem] * prTable[elem];
            }
            (*dst).data[row][col][0] = dd;
            (*dst).data[row][col][1] = dd;
            (*dst).data[row][col][2] = dd;
            (*dst).data[row][col][3] = dd;
        }
    }
    NV_TRACE_COND(TR_EVAL, 55,
        TPRINTF(("calc retess matrix r=%g order=%d\n", prPower[1], order));
        TraceMatrix(info, dst);
    );
}

void evalFillOnePowerAndRetessMatrix(NV_PATCH_INFO *info, int order, float newOverOld, FDMatrix *dst)
{
    double rPower[NV_PATCH_MAX_RETESS] = {1.};

    evalFillPower(newOverOld, rPower);
    evalFillOneRetessMatrix(info, rPower, order, dst);
}

void evalFillAllRetessMatrices(NV_PATCH_INFO *info, float newOverOld, FDMatrix *dst)
{
    int order;
    double rPower[NV_PATCH_MAX_RETESS] = {1.};

    evalFillPower(newOverOld, rPower);
    for (order = 2;  order < NV_PATCH_MAX_RETESS;  order++) {
        evalFillOneRetessMatrix(info, rPower, order, &dst[order]);
    }
}

void evalFillRetessMatrices(NV_PATCH_INFO *info)
// note: order = (degree + 1)
{
    int degree;

    factorial[0] = 1.;
    factorial[1] = 1.;
    for (degree = 2;  degree <= NV_PATCH_MAX_RETESS;  degree++) {
        factorial[degree] = degree * factorial[degree-1];
    }
    for (degree = 0;  degree <= NV_PATCH_MAX_RETESS;  degree++) {
        oofactorial[degree] = 1. / factorial[degree];
    }

    evalFillAllRetessMatrices(info, info->maxSwatch,      smallToBigMatrix);
    evalFillAllRetessMatrices(info, 1. / info->maxSwatch, bigToSmallMatrix);
    nvAssert(retessGuard == 0xfeed1);   // attempt to catch init over-runs
}

/*****************************************************************************/
// Check the retessllation cache to see if we've cached some work here
int CheckRetessellationCache(NV_PATCH_INFO *info, unsigned int tag1, unsigned int tag2, FDMatrix **m)
{
    //m should be null because it will be overwritten here.
    nvAssert(*m==NULL);

    if (g_FDGlobalData.RetessFDtag[0][0] == tag1 && g_FDGlobalData.RetessFDtag[0][1] == tag2) {
        *m = g_FDGlobalData.RetessFDCache[0];
        g_FDGlobalData.RetessFDMRU = 0;
    } else if (g_FDGlobalData.RetessFDtag[1][0] == tag1 && g_FDGlobalData.RetessFDtag[1][1] == tag2) {
        *m = g_FDGlobalData.RetessFDCache[1];
        g_FDGlobalData.RetessFDMRU = 1;
    } else {
        *m = (FDMatrix *)__NV_MALLOC(info->context, sizeof(FDMatrix));

        //replace cache slot with newly computed value
        g_FDGlobalData.RetessFDMRU = (1-g_FDGlobalData.RetessFDMRU);
        if (g_FDGlobalData.RetessFDCache[g_FDGlobalData.RetessFDMRU]) {
            __NV_FREE(info->context, g_FDGlobalData.RetessFDCache[g_FDGlobalData.RetessFDMRU]);
        }
        g_FDGlobalData.RetessFDCache[g_FDGlobalData.RetessFDMRU] = *m;
        g_FDGlobalData.RetessFDtag[g_FDGlobalData.RetessFDMRU][0] = tag1;
        g_FDGlobalData.RetessFDtag[g_FDGlobalData.RetessFDMRU][1] = tag2;
        return 0;
    }

    return 1;
}

/*****************************************************************************/
// Compute FD coefficients from curve control points
void ComputeFDCurveFlip(NV_PATCH_INFO *info, int order, float step, NV_PATCH_CURVE_INFO *pCurve,
                const float *points, int stride, int dir, int flipped)
{
    float *coeffs = pCurve->coeffs[0];
    FDMatrix *basis, *m, *p;
    FDMatrix *temp[3];
    EV_ALLOC_DECL();
#ifdef DCR_COMPUTEFDCURVEFLIP_OPT
    unsigned int tag1, tag2;
#endif
    int mbytes = EV_ROUND(sizeof(FDMatrixd));

    EV_ALLOC_INIT_WITH_CACHE(info->context, /*16 + 2* */6 * mbytes, computeFDCurveCoeffsEnd, info->pCache[EV_CACHE_9FDMATRICES]);
#if 0
if (*(int *)evMemory.pBase & 8) {
    EV_ALLOC(basis, info->context, 8);
}

EV_ALLOC(ptemp, info->context, mbytes);
EV_ALLOC(ptemp, info->context, mbytes);
EV_ALLOC(ptemp, info->context, mbytes);
#endif
    EV_ALLOC(basis, info->context, mbytes);
    EV_ALLOC(m, info->context, mbytes);
    EV_ALLOC(p, info->context, mbytes);
    EV_ALLOC(temp[0], info->context, mbytes);
    EV_ALLOC(temp[1], info->context, mbytes);
    EV_ALLOC(temp[2], info->context, mbytes);
//ffEV_ALLOC(ptemp, info->context, mbytes);
//ffEV_ALLOC(ptemp, info->context, mbytes);
//ffEV_ALLOC(ptemp, info->context, mbytes);
    EV_ALLOC_FULL(info->context);

    if (flipped) {
        stride = -stride;
    }
    // Reverse direction of points if necessary
    if (!dir) {
        points = (const float *)((const char *)points + (order-1)*stride);
        stride = -stride;
    }

#if 0
if (order == 6 && dotime) {
dotime = 0;
tprintf("\ntime curve info: order %d\n", order);
PrintInt(mbytes);
PrintInt(basis);
PrintInt(p);
PrintInt(m);
PrintInt(temp[0]);
PrintInt(temp[1]);
PrintInt(temp[2]);

    ComputeBasisd(order, 0, (FDMatrixd *)basis);
    ComputePd(order, step, (FDMatrixd *)p);
    ComputePowerToFDd(order, (FDMatrixd *)m, 0);

    FillMatrixd(info, 0, (FDMatrixd *)basis);
    FillMatrixd(info, 0, (FDMatrixd *)p);
    FillMatrixd(info, 0, (FDMatrixd *)m);
    MatrixMultd(info, (FDMatrixd *)temp[0], (FDMatrixd *)p, (FDMatrixd *)basis);
    MatrixMultd(info, (FDMatrixd *)temp[1], (FDMatrixd *)p, (FDMatrixd *)basis);
    MatrixMultd(info, (FDMatrixd *)temp[2], (FDMatrixd *)m, (FDMatrixd *)temp[1]);
    FillMatrixd(info, 0, (FDMatrixd *)temp[0]);
    FillMatrixd(info, 0, (FDMatrixd *)temp[1]);
    FillMatrixd(info, 0, (FDMatrixd *)temp[2]);

//ffTIME_CODE_REPEAT(nRepeat, 1, "double 1 P B",
    //ffMatrixMultd(info, (FDMatrixd *)temp[1], (FDMatrixd *)p, (FDMatrixd *)basis);
//ff); 
//ffTIME_CODE_REPEAT(nRepeat, 1, "double 2 M 1",
    //ffMatrixMultd(info, (FDMatrixd *)temp[2], (FDMatrixd *)m, (FDMatrixd *)temp[1]);
//ff);

VtResumeSampling();
TIME_CODE_REPEAT(nRepeat, 1, "double P M B",
    MatrixMultd(info, (FDMatrixd *)p, (FDMatrixd *)m, (FDMatrixd *)basis);
);
VtPauseSampling();

TIME_CODE_REPEAT(nRepeat, 1, "double 0 M B",
    MatrixMultd(info, (FDMatrixd *)temp[0], (FDMatrixd *)m, (FDMatrixd *)basis);
);
#if 0
TIME_CODE_REPEAT(nRepeat, 1, "double 1 M B",
    MatrixMultd(info, (FDMatrixd *)temp[1], (FDMatrixd *)m, (FDMatrixd *)basis);
);
TIME_CODE_REPEAT(nRepeat, 1, "double 2 M B",
    MatrixMultd(info, (FDMatrixd *)temp[2], (FDMatrixd *)m, (FDMatrixd *)basis);
);
//...
TIME_CODE_REPEAT(nRepeat, 1, "double P M B rev",
    MatrixMultd(info, (FDMatrixd *)p, (FDMatrixd *)basis, (FDMatrixd *)m);
);
TIME_CODE_REPEAT(nRepeat, 1, "double 0 M B rev",
    MatrixMultd(info, (FDMatrixd *)temp[0], (FDMatrixd *)basis, (FDMatrixd *)m);
);
TIME_CODE_REPEAT(nRepeat, 1, "double 1 M B rev",
    MatrixMultd(info, (FDMatrixd *)temp[1], (FDMatrixd *)basis, (FDMatrixd *)m);
);
TIME_CODE_REPEAT(nRepeat, 1, "double 2 M B rev",
    MatrixMultd(info, (FDMatrixd *)temp[2], (FDMatrixd *)basis, (FDMatrixd *)m);
);

TIME_CODE_REPEAT(nRepeat, 1, "double 0 P B",
    MatrixMultd(info, (FDMatrixd *)temp[0], (FDMatrixd *)p, (FDMatrixd *)basis);
);
TIME_CODE_REPEAT(nRepeat, 1, "double 1 P B",
    MatrixMultd(info, (FDMatrixd *)temp[1], (FDMatrixd *)p, (FDMatrixd *)basis);
);
TIME_CODE_REPEAT(nRepeat, 1, "double 1 P B",
    MatrixMultd(info, (FDMatrixd *)temp[1], (FDMatrixd *)p, (FDMatrixd *)basis);
);
//...
TIME_CODE_REPEAT(nRepeat, 1, "double 0 P B rev",
    MatrixMultd(info, (FDMatrixd *)temp[0], (FDMatrixd *)basis, (FDMatrixd *)p);
);
TIME_CODE_REPEAT(nRepeat, 1, "double 1 P B rev",
    MatrixMultd(info, (FDMatrixd *)temp[1], (FDMatrixd *)basis, (FDMatrixd *)p);
);
TIME_CODE_REPEAT(nRepeat, 1, "double 1 P B rev",
    MatrixMultd(info, (FDMatrixd *)temp[1], (FDMatrixd *)basis, (FDMatrixd *)p);
);

TIME_CODE_REPEAT(nRepeat, 1, "double 0 P B",
    MatrixMultd(info, (FDMatrixd *)temp[1], (FDMatrixd *)temp[0], (FDMatrixd *)basis);
);
TIME_CODE_REPEAT(nRepeat, 1, "double 0 P B",
    MatrixMultd(info, (FDMatrixd *)temp[2], (FDMatrixd *)temp[0], (FDMatrixd *)basis);
);
//...
TIME_CODE_REPEAT(nRepeat, 1, "double 0 P B rev",
    MatrixMultd(info, (FDMatrixd *)temp[1], (FDMatrixd *)basis, (FDMatrixd *)temp[0]);
);
TIME_CODE_REPEAT(nRepeat, 1, "double 0 P B rev",
    MatrixMultd(info, (FDMatrixd *)temp[2], (FDMatrixd *)basis, (FDMatrixd *)temp[0]);
);
#endif

}
#endif

#ifdef DCR_COMPUTEFDCURVEFLIP_OPT
    temp[2] = NULL;
    tag1 = (0xFFFF0000 | order);
    tag2 = *(unsigned int *)(&step);
    if (!CheckRetessellationCache(info, tag1, tag2, &temp[2])) 
    {
        // Compute matrices for the calculation
        ComputeBasis(order, 0, basis);
        ComputeP(order, step, p);
        ComputePowerToFD(order, m, 0);
        MatrixMult(info, temp[1], p, basis);
        MatrixMult(info, temp[2], m, temp[1]);
    }
    // Compute FD curve coefficients
    FillColumnVectorMatrix(order, stride, points, temp[0], 0);
    MatrixMult(info, temp[1], temp[2], temp[0]);
    EmptyColumnVectorMatrix(order, temp[1], 4*sizeof(float), coeffs, 0);
#else
    // Compute matrices for the calculation
    ComputeBasis(order, 0, basis);
    ComputeP(order, step, p);
    ComputePowerToFD(order, m, 0);

    // Compute FD curve coefficients
    FillColumnVectorMatrix(order, stride, points, temp[0], 0);
    MatrixMult(info, temp[1], basis, temp[0]);
    MatrixMult(info, temp[0], p, temp[1]);
    MatrixMult(info, temp[1], m, temp[0]);
    EmptyColumnVectorMatrix(order, temp[1], 4*sizeof(float), coeffs, 0);
#endif

    NV_TRACE_COND(TR_EVAL, 35,
        int i;

        TPRINTF(("FDCurve order=%d  stride=%d  dir=%d\n", order, stride, dir));
        for (i = 0;  i < order;  i++) {
            TPRINTF(("\t%d) [%7.5f %7.5f %7.5f %7.5f]\n", i, coeffs[i*4+0], coeffs[i*4+1], coeffs[i*4+2], coeffs[i*4+3]));
        }
        TPRINTF(("\n"));
    );
computeFDCurveCoeffsEnd:
    EV_FREE(basis, info->context);
    EV_FREE(m, info->context);
    EV_FREE(p, info->context);
    EV_FREE(temp[0], info->context);
    EV_FREE(temp[1], info->context);
}

#define ONE_HALF 0.5
#define ONE_SIXTH 1.0f/6.0f
#define ONE_ONE_TWENTIETH 1.0f/120.0f
#define ONE_TWENTY_FOURTH 1.0f/24.0f
#define ONE_TWELFTH 1.0f/12.0f
#define ONE_FOURTH 1.0f/4.0f
void ComputeReversedRetessellation(NV_PATCH_INFO *info, int order, float a, float b, FDMatrix *temp[3], FDMatrix *m, FDMatrix *p, FDMatrix *mInv)
{
    float ib,ib2,ib3,ib4,ib5,b2,b3,b4;
    float r,r2,r3,r4,r5;    
    float ftemp;   
    FDMatrix *outMatrix = temp[2];
    r=a/b;
    switch(order-1) {                
    case 3: //cubic
        ib = 1/b; ib2 = ib*ib; ib3 = ib2*ib; r2 = r*r; r3 = r2*r;
        outMatrix->rows = outMatrix->columns = order;        
        outMatrix->data[0][0][3] = outMatrix->data[0][0][2] = outMatrix->data[0][0][1] = outMatrix->data[0][0][0] = 1;
        outMatrix->data[0][1][3] = outMatrix->data[0][1][2] = outMatrix->data[0][1][1] = outMatrix->data[0][1][0] = ib;    ftemp = (1-b)*ONE_HALF*ib2;
        outMatrix->data[0][2][3] = outMatrix->data[0][2][2] = outMatrix->data[0][2][1] = outMatrix->data[0][2][0] = ftemp; ftemp = (-1+b)*(-1+2*b)*ONE_SIXTH*ib3;
        outMatrix->data[0][3][3] = outMatrix->data[0][3][2] = outMatrix->data[0][3][1] = outMatrix->data[0][3][0] = ftemp;    

        outMatrix->data[1][0][3] = outMatrix->data[1][0][2] = outMatrix->data[1][0][1] = outMatrix->data[1][0][0] = 0;
        outMatrix->data[1][1][3] = outMatrix->data[1][1][2] = outMatrix->data[1][1][1] = outMatrix->data[1][1][0] = -r;    ftemp = r*(-2+b+b*r)*ONE_HALF*ib;
        outMatrix->data[1][2][3] = outMatrix->data[1][2][2] = outMatrix->data[1][2][1] = outMatrix->data[1][2][0] = ftemp; ftemp = -r*(3+b*(2+r)*(-3+b+b*r))*ONE_SIXTH*ib2;
        outMatrix->data[1][3][3] = outMatrix->data[1][3][2] = outMatrix->data[1][3][1] = outMatrix->data[1][3][0] = ftemp;    

        outMatrix->data[2][0][3] = outMatrix->data[2][0][2] = outMatrix->data[2][0][1] = outMatrix->data[2][0][0] = 0;
        outMatrix->data[2][1][3] = outMatrix->data[2][1][2] = outMatrix->data[2][1][1] = outMatrix->data[2][1][0] = 0;
        outMatrix->data[2][2][3] = outMatrix->data[2][2][2] = outMatrix->data[2][2][1] = outMatrix->data[2][2][0] = r2;    ftemp = -r2*(-1+b+b*r)*ib;
        outMatrix->data[2][3][3] = outMatrix->data[2][3][2] = outMatrix->data[2][3][1] = outMatrix->data[2][3][0] = ftemp;    

        outMatrix->data[3][0][3] = outMatrix->data[3][0][2] = outMatrix->data[3][0][1] = outMatrix->data[3][0][0] = 0;
        outMatrix->data[3][1][3] = outMatrix->data[3][1][2] = outMatrix->data[3][1][1] = outMatrix->data[3][1][0] = 0;
        outMatrix->data[3][2][3] = outMatrix->data[3][2][2] = outMatrix->data[3][2][1] = outMatrix->data[3][2][0] = 0;
        outMatrix->data[3][3][3] = outMatrix->data[3][3][2] = outMatrix->data[3][3][1] = outMatrix->data[3][3][0] = -r3;       
        return;        
    case 5:
        ib = 1/b; ib2 = ib*ib; ib3 = ib2*ib; ib4 = ib2*ib2; ib5 = ib4*ib; 
        r2 = r*r; r3 = r2*r, r4=r2*r2; r5=r3*r2;
        b2 = b*b; b3 = b2*b; b4 = b2*b2;
        outMatrix->rows = outMatrix->columns = order;        
        outMatrix->data[0][0][3] = outMatrix->data[0][0][2] = outMatrix->data[0][0][1] = outMatrix->data[0][0][0] = 1;
        outMatrix->data[0][1][3] = outMatrix->data[0][1][2] = outMatrix->data[0][1][1] = outMatrix->data[0][1][0] = ib;    ftemp = (1-b)*ONE_HALF*ib2;
        outMatrix->data[0][2][3] = outMatrix->data[0][2][2] = outMatrix->data[0][2][1] = outMatrix->data[0][2][0] = ftemp; ftemp = (-1+b)*(-1+2*b)*ONE_SIXTH*ib3;
        outMatrix->data[0][3][3] = outMatrix->data[0][3][2] = outMatrix->data[0][3][1] = outMatrix->data[0][3][0] = ftemp; ftemp = (1+b*(-6+(11-6*b)*b))*ONE_TWENTY_FOURTH*ib4;    
        outMatrix->data[0][4][3] = outMatrix->data[0][4][2] = outMatrix->data[0][4][1] = outMatrix->data[0][4][0] = ftemp; ftemp = (-1+b)*(-1+2*b)*(-1+3*b)*(-1+4*b)*ONE_ONE_TWENTIETH*ib5;
        outMatrix->data[0][5][3] = outMatrix->data[0][5][2] = outMatrix->data[0][5][1] = outMatrix->data[0][5][0] = ftemp;    

        outMatrix->data[1][0][3] = outMatrix->data[1][0][2] = outMatrix->data[1][0][1] = outMatrix->data[1][0][0] = 0;
        outMatrix->data[1][1][3] = outMatrix->data[1][1][2] = outMatrix->data[1][1][1] = outMatrix->data[1][1][0] = -r;    ftemp = r*(-2+b+b*r)*ONE_HALF*ib;
        outMatrix->data[1][2][3] = outMatrix->data[1][2][2] = outMatrix->data[1][2][1] = outMatrix->data[1][2][0] = ftemp; ftemp = -r*(3+b*(2+r)*(-3+b+b*r))*ONE_SIXTH*ib2;
        outMatrix->data[1][3][3] = outMatrix->data[1][3][2] = outMatrix->data[1][3][1] = outMatrix->data[1][3][0] = ftemp; ftemp = r*(2+b2*(1+r)*(2+r)-2*b*(3+r))*(-2+b*(3+r))*ONE_TWENTY_FOURTH*ib3;
        outMatrix->data[1][4][3] = outMatrix->data[1][4][2] = outMatrix->data[1][4][1] = outMatrix->data[1][4][0] = ftemp; ftemp = r*(-5+10*b*(4+r)-b4*(1+r)*(2+r)*(3+r)*(4+r)+5*b3*(4+r)*(5+r*(4+r))-5*b2*(21+2*r*(6+r)))*ONE_ONE_TWENTIETH*ib4;
        outMatrix->data[1][5][3] = outMatrix->data[1][5][2] = outMatrix->data[1][5][1] = outMatrix->data[1][5][0] = ftemp;    

        outMatrix->data[2][0][3] = outMatrix->data[2][0][2] = outMatrix->data[2][0][1] = outMatrix->data[2][0][0] = 0;
        outMatrix->data[2][1][3] = outMatrix->data[2][1][2] = outMatrix->data[2][1][1] = outMatrix->data[2][1][0] = 0;
        outMatrix->data[2][2][3] = outMatrix->data[2][2][2] = outMatrix->data[2][2][1] = outMatrix->data[2][2][0] = r2;    ftemp = -r2*(-1+b+b*r)*ib;
        outMatrix->data[2][3][3] = outMatrix->data[2][3][2] = outMatrix->data[2][3][1] = outMatrix->data[2][3][0] = ftemp; ftemp = r2*(6-6*b*(3+2*r)+b2*(1+r)*(11+7*r))*ib2*ONE_TWELFTH;
        outMatrix->data[2][4][3] = outMatrix->data[2][4][2] = outMatrix->data[2][4][1] = outMatrix->data[2][4][0] = ftemp; ftemp = -r2*(-1+b*(2+r))*(2-4*b*(2+r)+b2*(1+r)*(5+3*r))*ONE_TWELFTH*ib3;
        outMatrix->data[2][5][3] = outMatrix->data[2][5][2] = outMatrix->data[2][5][1] = outMatrix->data[2][5][0] = ftemp;    

        outMatrix->data[3][0][3] = outMatrix->data[3][0][2] = outMatrix->data[3][0][1] = outMatrix->data[3][0][0] = 0;
        outMatrix->data[3][1][3] = outMatrix->data[3][1][2] = outMatrix->data[3][1][1] = outMatrix->data[3][1][0] = 0;
        outMatrix->data[3][2][3] = outMatrix->data[3][2][2] = outMatrix->data[3][2][1] = outMatrix->data[3][2][0] = 0;
        outMatrix->data[3][3][3] = outMatrix->data[3][3][2] = outMatrix->data[3][3][1] = outMatrix->data[3][3][0] = -r3;   ftemp = r3*(-2+3*b*(1+r))*ONE_HALF*ib;     
        outMatrix->data[3][4][3] = outMatrix->data[3][4][2] = outMatrix->data[3][4][1] = outMatrix->data[3][4][0] = ftemp; ftemp = -r3*(2-2*b*(4+3*r)+b2*(1+r)*(7+5*r))*ONE_FOURTH*ib2;
        outMatrix->data[3][5][3] = outMatrix->data[3][5][2] = outMatrix->data[3][5][1] = outMatrix->data[3][5][0] = ftemp;       

        outMatrix->data[4][0][3] = outMatrix->data[4][0][2] = outMatrix->data[4][0][1] = outMatrix->data[4][0][0] = 0;
        outMatrix->data[4][1][3] = outMatrix->data[4][1][2] = outMatrix->data[4][1][1] = outMatrix->data[4][1][0] = 0;
        outMatrix->data[4][2][3] = outMatrix->data[4][2][2] = outMatrix->data[4][2][1] = outMatrix->data[4][2][0] = 0;
        outMatrix->data[4][3][3] = outMatrix->data[4][3][2] = outMatrix->data[4][3][1] = outMatrix->data[4][3][0] = 0;   
        outMatrix->data[4][4][3] = outMatrix->data[4][4][2] = outMatrix->data[4][4][1] = outMatrix->data[4][4][0] = r4;    ftemp = r4*(1-2*b*(1+r))*ib;
        outMatrix->data[4][5][3] = outMatrix->data[4][5][2] = outMatrix->data[4][5][1] = outMatrix->data[4][5][0] = ftemp;       

        outMatrix->data[5][0][3] = outMatrix->data[5][0][2] = outMatrix->data[5][0][1] = outMatrix->data[5][0][0] = 0;
        outMatrix->data[5][1][3] = outMatrix->data[5][1][2] = outMatrix->data[5][1][1] = outMatrix->data[5][1][0] = 0;
        outMatrix->data[5][2][3] = outMatrix->data[5][2][2] = outMatrix->data[5][2][1] = outMatrix->data[5][2][0] = 0;
        outMatrix->data[5][3][3] = outMatrix->data[5][3][2] = outMatrix->data[5][3][1] = outMatrix->data[5][3][0] = 0;
        outMatrix->data[5][4][3] = outMatrix->data[5][4][2] = outMatrix->data[5][4][1] = outMatrix->data[5][4][0] = 0;
        outMatrix->data[5][5][3] = outMatrix->data[5][5][2] = outMatrix->data[5][5][1] = outMatrix->data[5][5][0] = -r5;
        return;
    default: 
    case 1:
    case 9:
        //THESE CONVERSION MATRICES NEED TO BE ADDED.
        nvAssert(0);
        break;
    }

    // Compute matrices for the calculation
    ComputePowerToFD(order, m, 0);
    ComputeP(order, r, p);
    ComputePowerToFD(order, mInv, 1);
    MatrixMult(info, temp[1], p, mInv);
    MatrixMult(info, temp[2], m, temp[1]);
    return;
}

/*****************************************************************************/
// Compute FD coefficients from curve control points
void RetessellateFDCurveInOut(NV_PATCH_INFO *info, float stepOld, float stepNew,
                    NV_PATCH_CURVE_INFO *curve, NV_PATCH_CURVE_INFO *curveOut, int reverseDirection)
{
    FDMatrix *m, *p, *mInv;
    FDMatrix *temp[3];
    EV_ALLOC_DECL();
    float ratio;
#ifdef DCR_HOS_RETESSCURVE_OPT
    unsigned int tag1, tag2;
#endif

    EV_ALLOC_INIT_WITH_CACHE(info->context, 5 * sizeof(FDMatrix), computeFDCurveCoeffsEnd, info->pCache[EV_CACHE_9FDMATRICES]);
    EV_ALLOC(m, info->context, sizeof(FDMatrix));
    EV_ALLOC(p, info->context, sizeof(FDMatrix));
    EV_ALLOC(mInv, info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[0], info->context, sizeof(FDMatrix));
    EV_ALLOC(temp[1], info->context, sizeof(FDMatrix));
    EV_ALLOC_FULL(info->context);

#ifdef DCR_HOS_RETESSCURVE_OPT
    ratio = stepNew / stepOld;
    temp[2] = NULL;
    tag1 = (*(unsigned int*)(&ratio));
    tag2 = (0x0000FFFF & (unsigned int)(curve->order)) | reverseDirection << 20;    
    if (ratio != 1.0) {
        if (!CheckRetessellationCache(info, tag1, tag2, &temp[2])) {
            if (reverseDirection) {
                //the direciton of the curve is reversed so we need an uglier
                //retessellation matrix to 'reverse' the control point order here
                ComputeReversedRetessellation(info, curve->order, stepNew, stepOld, temp, m, p, mInv);
            } else {
                // Compute matrices for the calculation
                ComputePowerToFD(curve->order, m, 0);
                ComputeP(curve->order, ratio, p);
                ComputePowerToFD(curve->order, mInv, 1);
                MatrixMult(info, temp[1], p, mInv);
                MatrixMult(info, temp[2], m, temp[1]);
            }
        }

        // Compute FD curve coefficients
        FillColumnVectorMatrix(curve->order, 4*sizeof(float), &curve->coeffs[0][0], temp[0], 0);
        MatrixMult(info, temp[1], temp[2], temp[0]);
        EmptyColumnVectorMatrix(curve->order, temp[1], 4*sizeof(float), &curveOut->coeffs[0][0], 0);
        curveOut->order = curve->order;
    }
#else
    ratio = stepNew / stepOld;
    // Compute matrices for the calculation
    ComputePowerToFD(curve->order, mInv, 1);
    ComputeP(curve->order, ratio, p);
    ComputePowerToFD(curve->order, m, 0);

    // Compute FD curve coefficients
    FillColumnVectorMatrix(curve->order, 4*sizeof(float), &curve->coeffs[0][0], temp[0], 0);
    MatrixMult(info, temp[1], mInv, temp[0]);
    MatrixMult(info, temp[0], p, temp[1]);
    MatrixMult(info, temp[1], m, temp[0]);
    EmptyColumnVectorMatrix(curve->order, temp[1], 4*sizeof(float), &curveOut->coeffs[0][0], 0);
    curveOut->order = curve->order;
#endif

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
            RetessellateFDCurveInOut(info, stepOld, stepNew, (*pIn)[index], (*pOut)[index], 0);
        }
    }
    return 0;
}

void RetessellateFDCurve(NV_PATCH_INFO *info, float stepOld, float stepNew,
                                NV_PATCH_CURVE_INFO *curve, int reverseDirection)
{
    RetessellateFDCurveInOut(info, stepOld, stepNew, curve, curve, reverseDirection);
}

/*****************************************************************************/

void OffsetCurve(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *curve, int n)
{
    ASSERT_CURVE(curve);
#ifdef DCR_HOS_OPT
    if (g_FDGlobalData.cpuType) {
        PFOFSC p;
        switch (curve->order) {
        case 4:
            p=(PFOFSC)(g_FDGlobalData.HOSInnerLoops[OFFSET_CURVE_4x4]);
            p(curve,n);
            return;
        case 6:
            p=(PFOFSC)(g_FDGlobalData.HOSInnerLoops[OFFSET_CURVE_6x6]);
            p(curve,n);
            return;
        case 2:
           /* p=(PFOFSC)(g_FDGlobalData.HOSInnerLoops[OFFSET_CURVE_2x2]);
            p(curve,n);
            break;*/
        default:
            ;   // fall thru to C code
        }
    }
#endif
    {
        int i, j;

        for (i = 0; i < n; i++) {
            for (j = 0; j < curve->order-1; j++) {
                AddPoint(curve->coeffs[j], curve->coeffs[j+1]);
            }
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
static void EvalCurve(const float *curve, int order, int n, float *outputs, int stride, int revIdx)
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
        NV_TRACE_COND(TR_EVAL, 35,
            static int cnt = 0;
            TPRINTF(("EvalCurve#%d %d-->%d) %9g %9g %9g %9g\n", cnt++, i, revIdx ? n-i : i,
                        outputs[0], outputs[1], outputs[2], outputs[3]));
        );
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
            NV_TRACE_COND(TR_EVAL, 35,
                TPRINTF(("attr %d\n", index));
            );
            ASSERT_CURVE((*curves)[index]);
            EvalCurve((*curves)[index]->coeffs[0], (*curves)[index]->order,
                n, &pOutput->vertexAttribs[0][index][0],
                4*sizeof(float)*NV_PATCH_NUMBER_OF_ATTRIBS, revIdx);
        }
    }
}

/*****************************************************************************/
#define EVAL_IWRAP(serial) ((serial) % NV_PATCH_MAX_SW_VERTICES)

// Evaluate attr0 along a curve
static void EvalCurveIdx(int attr, int order, const float *curve, int n, NV_PATCH_EVAL_OUTPUT *pOutput, int startIdx, int revIdx)
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
        NV_TRACE_COND(TR_EVAL, 35,
            static int cnt = 0;
            TPRINTF(("EvalCurveIdx#%d/%d %d-->%d) %9g %9g %9g %9g\n", cnt++, attr, i, idx,
                        temp[0][0], temp[0][1], temp[0][2], temp[0][3]));
        );

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
#ifdef DCR_COMPUTEFDMATRIXFLIP_OPT
    unsigned int tag1, tag2;
#endif
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

#ifdef DCR_COMPUTEFDMATRIXFLIP_OPT
    // Compute u-direction FD curve coefficients
    for (j = 0; j < vorder; j++) { FillColumnVectorMatrix(uorder, ufStride * sizeof(float), points + j*vfStride, temp[0], j); }
    temp[2] = NULL;
    tag1 = (0xFFFF0000 | uorder);
    tag2 = *(unsigned int *)(&ustep);
    if (!CheckRetessellationCache(info, tag1, tag2, &temp[2])) {
        // Compute matrices in each direction
        ComputeBasis(uorder, 0, basisU);
        ComputeP(uorder, ustep, pU);
        ComputePowerToFD(uorder, mU, 0);

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
    if (!CheckRetessellationCache(info, tag1, tag2, &temp[2])) {
        // Compute matrices in each direction
        ComputeBasis(vorder, 0, basisV);
        ComputeP(vorder, vstep, pV);
        ComputePowerToFD(vorder, mV, 0);

        MatrixMult(info, temp[1], pV, basisV);
        MatrixMult(info, temp[2], mV, temp[1]);
    }
    MatrixMult(info, temp[1], temp[2], temp[0]);
    for (i = 0; i < uorder; i++) { EmptyColumnVectorMatrix(vorder, temp[1], 4*sizeof(float)*NV_PATCH_MAX_ORDER, &m->data[0][i][0], i); }
#else
    // Compute matrices in each direction
    ComputeBasis(uorder, 0, basisU);
    ComputeP(uorder, ustep, pU);
    ComputePowerToFD(uorder, mU, 0);

    // Compute u-direction FD curve coefficients
    for (j = 0; j < vorder; j++) {
        FillColumnVectorMatrix(uorder, ufStride * sizeof(float), points + j*vfStride, temp[0], 0);
        MatrixMult(info, temp[1], basisU, temp[0]);
        MatrixMult(info, temp[0], pU, temp[1]);
        MatrixMult(info, temp[1], mU, temp[0]);
        EmptyColumnVectorMatrix(uorder, temp[1], 4*sizeof(float), &uCoeffs->data[j][0][0], 0);
    }
    NV_TRACE_COND(TR_EVAL, 25,
        TraceMatrix(info, temp[1]);
    );

    // Compute matrices in each direction
    ComputeBasis(vorder, 0, basisV);
    ComputeP(vorder, vstep, pV);
    ComputePowerToFD(vorder, mV, 0);

    // Compute v-direction FD curve coefficients
    for (i = 0; i < uorder; i++) {
        FillColumnVectorMatrix(vorder, 4*sizeof(float)*NV_PATCH_MAX_ORDER, &uCoeffs->data[0][i][0], temp[0], 0);
        MatrixMult(info, temp[1], basisV, temp[0]);
        MatrixMult(info, temp[0], pV, temp[1]);
        MatrixMult(info, temp[1], mV, temp[0]);
        EmptyColumnVectorMatrix(vorder, temp[1], 4*sizeof(float)*NV_PATCH_MAX_ORDER, &m->data[0][i][0], 0);
    }
#endif

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

    NV_TRACE_COND(TR_EVAL, 25,
        TraceMatrix(info, m);
    );
}

void ComputeFDMatrix(NV_PATCH_INFO *info, int uorder, int vorder, float ustep, float vstep,
                             FDMatrix *m, const float *points, int ustride, int vstride)
// wrapper for now, eventually change all calls to be direct
{
    ComputeFDMatrixFlip(info, 0, uorder, vorder, ustep, vstep, m,
                (const float *)points, 0, 0, ustride, vstride);
}

// Offset an FD matrix by 'a' steps in u and 'b' steps in v
// Not particularly fast -- try to step along a curve when possible!
void OffsetFDMatrix(NV_PATCH_INFO *info, FDMatrix *m, int a, int b)
{
#ifdef DCR_HOS_OPT
#if !defined(IS_OPENGL)
    int order = m->rows;
#else
    int order = (m->rows == m->columns) ? m->rows : 0;
#endif
    PFOFSTM p;
    if (g_FDGlobalData.cpuType) {
        switch (order) {
        case 4:
            p=(PFOFSTM)(g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_4x4]);
            p(m, a, b);
            return;
        case 6:
            p=(PFOFSTM)(g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_6x6]);
            p(m, a, b);
            return;
        case 2:
            //OFFSET_FDMATRIX_2x2
        default:
            //OFFSET_FDMATRIX_DEF
            ;   // fall thru to C code
        }
    }
#endif
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
}

void OffsetFDMatrix_Extract_Discard(NV_PATCH_INFO *info, NV_PATCH_CURVE_INFO *dst, int dir, FDMatrix *m, int a, int b)
{
#ifdef DCR_HOS_OPT
#if !defined(IS_OPENGL)
    int order = m->rows;
#else
    int order = (m->rows == m->columns) ? m->rows : 0;
#endif
    PFOFSTMED p;
    if (g_FDGlobalData.cpuType) {
        switch (order) {
        case 4:
            p=(PFOFSTMED)(g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_ED_4x4]);
            p(dst, dir, m, a, b);
            return;
        case 2:
            /*p=(PFOFSTMED)(g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_ED_2x2]);
            p(dst, dir, m, a, b);
            return;
        case 6:
            /*p=(PFOFSTMED)(g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_ED_6x6]);
            p(dst, dir, m, a, b);
            break;*/
        default:
            ;   // fall thru to C code
        }
    }
#endif
    {
        BEGIN_USING_TEMP_MATRIX(info->tempMatrix);
        CopyMatrix(info, &info->tempMatrix, m);
        OffsetFDMatrix(info, &info->tempMatrix, a, b);
        if (dir) ExtractVCurve(info, dst, &info->tempMatrix);
        else ExtractUCurve(info, dst, &info->tempMatrix);
        END_USING_TEMP_MATRIX(info->tempMatrix);
    }
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
#ifndef DCR_HOS_RETESS_OPT
    int i,j;
#else
    unsigned int tag1, tag2;
    int reverseDirection;
    float ratio;
#endif
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

#ifdef DCR_HOS_RETESS_OPT
    // Compute matrices for the calculation
    ratio = stepNewU / stepOldU;
    if (ratio != 1.0) {
        temp[2] = NULL;
        reverseDirection = (info->quadInfo->uMaxDir != info->quadInfo->olduMaxDir ? 1 : 0);
        tag1 = (*(unsigned int*)(&ratio));
        tag2 = (0x0000FFFF & uorder) | reverseDirection << 20;
        if (!CheckRetessellationCache(info, tag1, tag2, &temp[2])) {            
            //cache miss calculate the step matrix
            if (reverseDirection) {
                ComputeReversedRetessellation(info, uorder, stepNewU, stepOldU, temp, m, p, mInv);
            } else {
                ComputePowerToFD(uorder, m, 0);
                ComputeP(uorder, ratio, p);
                ComputePowerToFD(uorder, mInv, 1);
                MatrixMult(info, temp[0], p, mInv);
                MatrixMult(info, temp[2], m, temp[0]);
            }
        }
        CopyMatrixTranspose(info, temp[0], matrix);
        MatrixMult(info, temp[1], temp[2], temp[0]);
        CopyMatrixTranspose(info, matrix, temp[1]);
    }

    ratio = stepNewV / stepOldV;
    if (ratio != 1.0) {        
        temp[2] = NULL;
        reverseDirection = (info->quadInfo->vMaxDir != info->quadInfo->oldvMaxDir ? 1 : 0);
        tag1 = (*(unsigned int*)(&ratio));
        tag2 = (0x0000FFFF & vorder) | reverseDirection << 20;
        if (!CheckRetessellationCache(info, tag1, tag2, &temp[2])) {
            //cache miss calculate the step matrix
            if (reverseDirection) {
                ComputeReversedRetessellation(info, uorder, stepNewV, stepOldV, temp, m, p, mInv);
            } else {
                ComputePowerToFD(vorder, m, 0);
                ComputeP(vorder, ratio, p);
                ComputePowerToFD(vorder, mInv, 1);
                MatrixMult(info, temp[0], p, mInv);
                MatrixMult(info, temp[2], m, temp[0]);
            }
        }

        //don't need a copy cause its not a transpose...
        MatrixMult(info, temp[1], temp[2], matrix);
        CopyMatrix(info, matrix, temp[1]);
    }

#else
    // Compute matrices for the calculation
    ComputePowerToFD(uorder, mInv, 1);
    ComputeP(uorder, stepNewU / stepOldU, p);
    ComputePowerToFD(uorder, m, 0);

    // Recompute in u direction
    for (j = 0; j < vorder; j++) {
        FillColumnVectorMatrix(uorder, 4*sizeof(float), &matrix->data[j][0][0], temp[0], 0);
        MatrixMult(info, temp[1], mInv, temp[0]);
        MatrixMult(info, temp[0], p, temp[1]);
        MatrixMult(info, temp[1], m, temp[0]);
        EmptyColumnVectorMatrix(uorder, temp[1], 4*sizeof(float), &matrix->data[j][0][0], 0);
    }

    // Compute matrices for the calculation
    ComputePowerToFD(vorder, mInv, 1);
    ComputeP(vorder, stepNewV / stepOldV, p);
    ComputePowerToFD(vorder, m, 0);

    // Recompute in v direction
    for (i = 0; i < uorder; i++) {
        FillColumnVectorMatrix(vorder, 4*sizeof(float)*NV_PATCH_MAX_ORDER, &matrix->data[0][i][0], temp[0], 0);
        MatrixMult(info, temp[1], mInv, temp[0]);
        MatrixMult(info, temp[0], p, temp[1]);
        MatrixMult(info, temp[1], m, temp[0]);
        EmptyColumnVectorMatrix(vorder, temp[1], 4*sizeof(float)*NV_PATCH_MAX_ORDER, &matrix->data[0][i][0], 0);
    }
#endif

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

const static float triTo3QuadDomains[3][4][2] = {
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

const static float triTo1QuadDomain[4][2] = {
    {0, 0},
    {1.0f, 0},
    {0, 1.0f},
    {0, 1.0f},
};

static FDMatrix *ReduceTriPatch(NV_PATCH_INFO *info, int level, FDMatrix *p, float w0, float w1)
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

// NOTE: this is a recursive routine
static unsigned int ComputeTriToQuad(NV_PATCH_INFO *info, int level, FDMatrix *f, FDMatrix *g, FDMatrix *h,
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

static unsigned int ReparameterizeTriPatch(NV_PATCH_INFO *info, FDMatrix *in,
                                           FDMatrix *out, int domain, int frac)
{
    FDMatrix *domainMatrix = &info->tempMatrix;
    float *weights[NV_PATCH_MAX_ORDER], invWeight;
    int i, j;
    EV_ALLOC_DECL();

    BEGIN_USING_TEMP_MATRIX(info->tempMatrix);
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
                domainMatrix->data[i][j][0] = triTo3QuadDomains[domain][j*2+i][0];
                domainMatrix->data[i][j][1] = triTo3QuadDomains[domain][j*2+i][1];
            } else {
                domainMatrix->data[i][j][0] = triTo1QuadDomain[j*2+i][0];
                domainMatrix->data[i][j][1] = triTo1QuadDomain[j*2+i][1];
            }
        }
    }

    info->retVal |= ComputeTriToQuad(info, 0, in, domainMatrix, out,
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
    END_USING_TEMP_MATRIX(info->tempMatrix);

    return info->retVal;
}

NV_INLINE void SendTransitionEndPoints(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo,
                    FDCurveAttrSet **ppCurvesInnerList, int revTran, int nCurveIn,
                    int gridIdxV, int gridIdxU, int cornerIdxV, int cornerIdxU, int doFrac, int transitionFlag)
{
    int index;
    int dwCount;
    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

    // SEND THE OUTER ENDPOINT
    HOS_NOTE("Send Outer Transition endpoint"); // always send this...
    HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
    HOS_PUSH(1,  NV097_SET_BEGIN_END_CURVE_CMD_OUTER_END_PT);
    HOS_PUSH_ADJUST(2);
    for (index = 0; index < info->maxAttr; index++) {
        dwCount = 0;
        if (info->evalEnables & (1 << index)) {
            HOS_NOTE("Set Curve Coefficients");
            HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
            HOS_PUSHF(dwCount+1, VIEW_AS_DWORD(quadInfo->pCorners[index][cornerIdxV][cornerIdxU][0]));
            HOS_PUSHF(dwCount+2, VIEW_AS_DWORD(quadInfo->pCorners[index][cornerIdxV][cornerIdxU][1]));
            HOS_PUSHF(dwCount+3, VIEW_AS_DWORD(quadInfo->pCorners[index][cornerIdxV][cornerIdxU][2]));
            HOS_PUSHF(dwCount+4, VIEW_AS_DWORD(quadInfo->pCorners[index][cornerIdxV][cornerIdxU][3]));
            dwCount+=5;
        }
        HOS_PUSH_ADJUST(dwCount);
    }
    HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
    HOS_PUSH(1,  NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
    HOS_PUSH_ADJUST(2);

    if (!doFrac) {
        // SEND THE INNER ENDPOINT -- integer case
        // this should only happen in degenerate transitions cases
        //ffif (quadInfo->nv0 == 1 && quadInfo->nv1 == 1)
        {
            float *pPoint;

            //HACK HACK HACK  -- just testing out a theory on the degenerate case Fred gave me...
            //Fix this up.
            HOS_NOTE("Send Inner Transition endpoint");
            HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
            HOS_PUSH(1,  NV097_SET_BEGIN_END_CURVE_CMD_INNER_END_PT);
            HOS_PUSH_ADJUST(2);
            //ffif (transitionFlag == LAST_ROW || transitionFlag == FIRST_ROW)
            {
                for (index = 0; index < info->maxAttr; index++) {
                    dwCount = 0;
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
                        HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
                        HOS_PUSH4F(dwCount+1, pPoint);
                        dwCount+=5;
                    }
                    HOS_PUSH_ADJUST(dwCount);
                }
                HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
                HOS_PUSH(1,  NV097_SET_BEGIN_END_CURVE_CMD_INNER_END_PT);
                HOS_PUSH_ADJUST(2);
            }
        }

    } else {
        // SEND THE INNER ENDPOINT -- fractional case
        if (transitionFlag == FIRST_ROW) {
            float *pPoint;
            HOS_NOTE("Send Inner Transition endpoint");
            HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
            HOS_PUSH(1,  NV097_SET_BEGIN_END_CURVE_CMD_INNER_END_PT);
            HOS_PUSH_ADJUST(2);
            for (index = 0; index < info->maxAttr; index++) {
                dwCount = 0;
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
                    HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
                    HOS_PUSHF(dwCount+1, VIEW_AS_DWORD(pPoint[0]));
                    HOS_PUSHF(dwCount+2, VIEW_AS_DWORD(pPoint[1]));
                    HOS_PUSHF(dwCount+3, VIEW_AS_DWORD(pPoint[2]));
                    HOS_PUSHF(dwCount+4, VIEW_AS_DWORD(pPoint[3]));
                    dwCount+=5;
                }
                HOS_PUSH_ADJUST(dwCount);
            }
            HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
            HOS_PUSH(1,  NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
            HOS_PUSH_ADJUST(2);
        } else if (transitionFlag == FIRST_COL) {
            float *pPoint;
            HOS_NOTE("Send Inner Transition endpoint");
            HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
            HOS_PUSH(1,  NV097_SET_BEGIN_END_CURVE_CMD_INNER_END_PT);
            HOS_PUSH_ADJUST(2);
            for (index = 0; index < info->maxAttr; index++) {
                dwCount = 0;
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
                    HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
                    HOS_PUSHF(dwCount+1, VIEW_AS_DWORD(pPoint[0]));
                    HOS_PUSHF(dwCount+2, VIEW_AS_DWORD(pPoint[1]));
                    HOS_PUSHF(dwCount+3, VIEW_AS_DWORD(pPoint[2]));
                    HOS_PUSHF(dwCount+4, VIEW_AS_DWORD(pPoint[3]));
                    dwCount+=5;
                }
                HOS_PUSH_ADJUST(dwCount);
            }
            HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
            HOS_PUSH(1,  NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
            HOS_PUSH_ADJUST(2);
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

NV_INLINE void SendFirstInnerTransitionCurve(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo,
                    FDCurveAttrSet **ppCurvesInnerList, int revTran, int nCurveIn, int doFrac, int transitionFlag)
{
    int index, i;
    int dwCount;
    NV_PATCH_CURVE_INFO *pCurve;
    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

    // SEND THE FIRST INNER TRANSITION CURVE
    HOS_NOTE("Begin Curve: Send Inner Transition");
    HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
    HOS_PUSH(1, NV097_SET_BEGIN_END_CURVE_CMD_INNER_TRANSITION_CURVE);
    HOS_PUSH_ADJUST(2);
    for (index = 0; index < info->maxAttr; index++) {
        if (info->evalEnables & (1 << index)) {
            dwCount = 0;
            pCurve = (*ppCurvesInnerList[!revTran ? 0 : nCurveIn-1])[index];
            if (doFrac && (quadInfo->vMaxSegs == 1) && (transitionFlag == FIRST_ROW)) {
                CopyCurve(info, &info->tempCurve, pCurve);
                OffsetCurve(info, &info->tempCurve,1);
                pCurve = &info->tempCurve;
            }

            // if this is a not a new swath... do we pre adjust that row??? or just not send the last row in the previous patch?
            for (i = 0; i < pCurve->order; i++) {
                HOS_NOTE("Set Curve Coefficients");
                HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
                HOS_PUSHF(dwCount+1, VIEW_AS_DWORD(pCurve->coeffs[i][0]));
                HOS_PUSHF(dwCount+2, VIEW_AS_DWORD(pCurve->coeffs[i][1]));
                HOS_PUSHF(dwCount+3, VIEW_AS_DWORD(pCurve->coeffs[i][2]));
                HOS_PUSHF(dwCount+4, VIEW_AS_DWORD(pCurve->coeffs[i][3]));
                dwCount+=5;
            }
            HOS_PUSH_ADJUST(dwCount);
        }
    }
    HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
    HOS_PUSH(1, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
    HOS_PUSH_ADJUST(2);
    HOS_FLUSH();
}

NV_INLINE void SendFirstOuterTransitionCurve(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, FDCurveAttrSet *pCurvesOuter,
                    int doFrac, int transitionFlag)
{
    int index, i;
    int dwCount;
    NV_PATCH_CURVE_INFO *pCurve;
    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

    // SEND THE FIRST OUTER TRANSITION CURVE
    HOS_NOTE("Send Outer Transition");
    HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
    HOS_PUSH(1, NV097_SET_BEGIN_END_CURVE_CMD_OUTER_TRANSITION_CURVE);
    HOS_PUSH_ADJUST(2);
    for (index = 0; index < info->maxAttr; index++) {
        if (info->evalEnables & (1 << index)) {
            dwCount = 0;
            pCurve = (*pCurvesOuter)[index];
            //if this is a not a new swath... do we pre adjust that row??? or just not send the last row in the previous patch?
            if (doFrac && (transitionFlag == LAST_ROW || transitionFlag == LAST_COL)) {
                CopyCurve(info, &info->tempCurve, pCurve);
                OffsetCurve(info, &info->tempCurve,1);
                pCurve = &info->tempCurve;
            }

            for (i = 0; i < pCurve->order; i++) {
                HOS_NOTE("Set Curve Coefficients");
                HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
                HOS_PUSHF(dwCount+1, VIEW_AS_DWORD(pCurve->coeffs[i][0]));
                HOS_PUSHF(dwCount+2, VIEW_AS_DWORD(pCurve->coeffs[i][1]));
                HOS_PUSHF(dwCount+3, VIEW_AS_DWORD(pCurve->coeffs[i][2]));
                HOS_PUSHF(dwCount+4, VIEW_AS_DWORD(pCurve->coeffs[i][3]));
                dwCount+=5;
            }
            HOS_PUSH_ADJUST(dwCount);
        }
    }
    HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
    HOS_PUSH(1, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
    HOS_PUSH_ADJUST(2);
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
                    int gridIdxVBegin, int gridIdxUBegin,   // for inner beginpoint from regular grid corner
                    int gridIdxV, int gridIdxU,             // for inner endpoint from regular grid corner
                    int cornerIdxV, int cornerIdxU,         // for outer endpoint from patch corners[][]
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
    int usingHW = info->usingHW; // identifies places that probably need code for HW version
    int dwCount;
    int lastFrontFace;

    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

    nCurveOut = (outSegs - 1) / info->maxSwatch + 1;
    nCurveIn =  (inSegs - 1 - doFrac) / info->maxSwatch + 1;
    NV_TRACE_COND(TR_EVAL, 30,
        TPRINTF(("DrawIntStitch out=%d in=%d stitchLowOut=%d stitchHighOut=%d\n",
                outSegs, inSegs, stitchLowOut, stitchHighOut));
        TPRINTF(("\t stitchLowIn=%d stitchHighIn=%d\n", stitchLowIn, stitchHighIn));
        TPRINTF(("\trev=%d ifEqualStepInner=%d\n", reverse, ifEqualStepInner));
    );
    nvAssert(qOtherDir == qMaxDir);
    nvAssert((outSegs <= inSegs)  ||  doFrac);

    if (!usingHW) {
        reverse ^= info->flipUV;
    }
    ifEqualStepInner ^= info->flipUV;

    out = 0;
    in = 0;
    if (stitchLowOut && (!usingHW /*|| doFrac*/)) {
        // these cases start the 'dist' algorithm at the first inner point
        in = 1;
        dist = inSegs - 3*outSegs;  // start with inside step
    } else {
        dist = inSegs - outSegs;
    }
    totalToDoOut = 1+outSegs; totalToDoIn = 1+inSegs;

    if (totalToDoOut > 1+info->maxSwatch) {
        oneToDoOut = info->maxSwatch;   // doesn't fit in one curve
        lastOutsideCurve = 0;
    } else {
        oneToDoOut = totalToDoOut;
        lastOutsideCurve = 1;
    }

    if (!usingHW) EvalStrip(info, ppCurvesOuter[0], oneToDoOut, pDataOut, 0); // outer

    if (totalToDoIn > 1+info->maxSwatch) {
        // doesn't fit in one curve
        if (revTran) {
            // reversed inner curve
            if (!usingHW) {
                oneToDoIn = inSegs % info->maxSwatch;
                assert(oneToDoIn >= 0);
                if (oneToDoIn <= 0) {
                    oneToDoIn += info->maxSwatch;
                }
            } else {
                oneToDoIn = inSegs % info->maxSwatch;
                assert(oneToDoIn >= 0);
                if ((oneToDoIn - stitchHighIn) <= 0) {
                    if ((oneToDoIn == 1)  &&  stitchHighIn /* && (nCurveIn == 2)*/) {
                        nCurveIn--;
                    }
                    oneToDoIn += info->maxSwatch; // + stitchHighIn;
                }
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

    if (!usingHW) {
        EvalStripIdx(info, ppCurvesInnerList[!revTran ? 0 : nCurveIn-1], oneToDoIn, pDataIn, 0, revTran); // inner
        if (onHighIn ) {
            //ffif (stitchHighIn) {
                oneToDoIn--;
                totalToDoIn--;
            //ff}
        }
    }

    if (revTran) {
        idxInnerEnd = in;
    } else {
        nvAssert(qDir == qOtherDir);
        idxInnerEnd = oneToDoIn-1;
    }
    if (!usingHW) {
        if (onLowIn) {
            oneToDoIn--;
            totalToDoIn--;
        }
    }

    if (usingHW) {
        //setup transition
        unsigned long TControl[3], order;
        unsigned long numcoeffs = 0x0;
        int hwAttr;
        TControl[0] = TControl[1] = TControl[2] = 0x0;
        for (hwAttr = 0; hwAttr <= 7; hwAttr++) {               //TPControl0
            if (info->evalEnables & (1 << hwAttr)) {
                order = (*ppCurvesOuter[0])[hwAttr]->order;
                assert((int)order == (*ppCurvesInnerList[0])[hwAttr]->order);
                TControl[0] |= ((order-1) << (hwAttr<<2));      //hwAttr*4
                numcoeffs += order;
            }
        }
        for (hwAttr = 8; hwAttr <= 15; hwAttr++) {              //TPControl1
            if (info->evalEnables & (1 << hwAttr)) {
                order = (*ppCurvesOuter[0])[hwAttr]->order;
                assert((int)order == (*ppCurvesInnerList[0])[hwAttr]->order);
                TControl[1] |= ((order-1) << ((hwAttr-8)<<2));
                numcoeffs += order;
            }
        }

        TControl[2] = (numcoeffs << 24) | (outSegs << 10) | inSegs;
        HOS_NOTE("Set Begin Transition");
        HOS_PUSH(0, ((0x3 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_TRANSITION0)));
        HOS_PUSH(1, TControl[0]);
        HOS_PUSH(2, TControl[1]);
        HOS_PUSH(3, TControl[2]);
        HOS_PUSH_ADJUST(4);

        //SEND Transition endpoints as necessary
        SendTransitionEndPoints(info, quadInfo, ppCurvesInnerList, revTran, nCurveIn,
                                gridIdxV, gridIdxU, cornerIdxV, cornerIdxU, doFrac, transitionFlag);

        //SEND Beginning Transition Curves
        SendFirstInnerTransitionCurve(info, quadInfo, ppCurvesInnerList, revTran, nCurveIn, doFrac, transitionFlag);
        SendFirstOuterTransitionCurve(info, quadInfo, ppCurvesOuter[0], doFrac, transitionFlag);

    } else {
        // Force first and last point
        for (index = 0; index < info->maxAttr; index++) {
            if (!EV_GUARD_ATTR(index)  &&  !doFrac) {
                continue;
            }
            if (!(info->evalEnables & (1 << index))) {
                continue;
            }
            if (lastOutsideCurve) {
                CopyPoint(pDataOut->vertexAttribs[oneToDoOut-1][index], quadInfo->pCorners[index][cornerIdxV][cornerIdxU]);
            }
            if (!doFrac) {
                // for starting grid corner
                if (revTran) {
                    if (lastInsideCurve && (nCurveIn == 1)) {
                        CopyPoint(pDataIn->vertexAttribs[oneToDoIn-onLowIn][index], info->gridCorner[index][gridIdxVBegin][gridIdxUBegin]);
                    }
                } else {
                    CopyPoint(pDataIn->vertexAttribs[in][index], info->gridCorner[index][gridIdxVBegin][gridIdxUBegin]);
                }
            }
            if (lastInsideCurve) {
                CopyPoint(pDataIn->vertexAttribs[idxInnerEnd][index], info->gridCorner[index][gridIdxV][gridIdxU]);
            }
        }

        if (doFrac  &&  stitchHighOut) {
            out++;
            oneToDoOut--;
            totalToDoOut--;

            in++;
            oneToDoIn--;
            totalToDoIn--;
        }

        info->backend->setFrontFace(info, lastFrontFace = reverse ^ info->reverse);
        info->backend->beginPrimitive(info, NV_PATCH_PRIMITIVE_TSTRIP);
        info->backend->sendVertex(info, pDataOut, out);
    }

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
        if (!usingHW) {
            dist += fudge ^ quadInfo->uMaxDir ^ quadInfo->vMaxDir ^ info->flipUV;
        } else {
            dist += fudge;
        }
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
        if (stitchLowOut && usingHW) {
            // for Bot and Left, adjust since first inner point skipped
            oneToDoIn--;
            totalToDoIn--;
        }
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
                    lastInsideCurve = 1;
                }

                innerCurve = !revTran ? onCurveIn : nCurveIn-1-onCurveIn;
                pCurves = ppCurvesInnerList[innerCurve];

                if (usingHW) {
                    // SEND NEXT INNER TRANSITION CURVE
                    HOS_NOTE("Send Inner Transition");
                    HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
                    HOS_PUSH(1, NV097_SET_BEGIN_END_CURVE_CMD_INNER_TRANSITION_CURVE);
                    HOS_PUSH_ADJUST(2);
                    for (index = 0; index < info->maxAttr; index++) {
                        if (info->evalEnables & (1 << index)) {
                            dwCount = 0;
                            //if this is a not a new swath... do we pre adjust that row??? or just not send the last row in the previous patch?
                            //fffor (i = 0; i < (*ppCurvesInnerList[!revTran ? 0 : nCurveIn-1])[index]->order; i++) { // }
                            for (i = 0; i < (*pCurves)[index]->order; i++) {
                                HOS_NOTE("Set Curve Coefficients");
                                HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
                                HOS_PUSHF(dwCount+1, VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][0]));
                                HOS_PUSHF(dwCount+2, VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][1]));
                                HOS_PUSHF(dwCount+3, VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][2]));
                                HOS_PUSHF(dwCount+4, VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][3]));
                                dwCount+=5;
                            }
                            HOS_PUSH_ADJUST(dwCount);
                        }
                    }
                    HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
                    HOS_PUSH(1, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
                    HOS_PUSH_ADJUST(2);
                    HOS_FLUSH();
                } else {
                    EvalStripIdx(info, pCurves, oneToDoIn, pDataIn, in, revTran); // inner
                    if (onHighIn) {
                        if (stitchHighIn) {
                            oneToDoIn--;
                            totalToDoIn--;
                        }
                    }
                    if (revTran) {
                        //ffidxInnerEnd = in+0;
                        idxInnerEnd = in+oneToDoIn-1-onLowIn;
                    } else {
                        idxInnerEnd = in+oneToDoIn-1;
                    }
                    if (onLowIn) {
                        oneToDoIn--;
                        totalToDoIn--;
                    }

                    // Force last point
                    for (index = 0; index < info->maxAttr; index++) {
                        if (!EV_GUARD_ATTR(index)  &&  !doFrac) {
                            continue;
                        }
                        if (!(info->evalEnables & (1 << index))) {
                            continue;
                        }
                        if (lastInsideCurve) {
                            if (revTran) {
                                CopyPoint(pDataIn->vertexAttribs[EVAL_IWRAP(idxInnerEnd)][index],
                                        info->gridCorner[index][gridIdxVBegin][gridIdxUBegin]);
                            } else {
                                CopyPoint(pDataIn->vertexAttribs[EVAL_IWRAP(idxInnerEnd)][index],
                                        info->gridCorner[index][gridIdxV][gridIdxU]);
                            }
                        }
                    }
                }
                onCurveIn++;
            }

            if (!usingHW) {
                if (inStrip && lastWasIn) {
                    info->backend->endPrimitive(info);
                    fanCenter = out;
                    fanFirst = in-1;
                    inStrip = 0;
                    if (lastFrontFace != (reverse ^ info->reverse)) {
                        info->backend->setFrontFace(info, lastFrontFace = !lastFrontFace);
                    }
                    info->backend->beginPrimitive(info, NV_PATCH_PRIMITIVE_TFAN);
                    info->backend->sendVertex(info, pDataOut, EVAL_IWRAP(fanCenter));
                    info->backend->sendVertex(info, pDataIn, EVAL_IWRAP(fanFirst));

                } else if (!inStrip && !lastWasIn) {
                    info->backend->endPrimitive(info);
                    info->backend->beginPrimitive(info, NV_PATCH_PRIMITIVE_TSTRIP);
                    info->backend->sendVertex(info, pDataIn, EVAL_IWRAP(in-1));
                    info->backend->sendVertex(info, pDataOut, EVAL_IWRAP(out));
                    inStrip = 1;
                }
                if (inStrip) {
                    info->backend->sendVertex(info, pDataIn, EVAL_IWRAP(in));
                } else {
                    fanLast = in;
                    info->backend->sendVertex(info, pDataIn, EVAL_IWRAP(fanLast));
                }
                lastWasIn = 1;
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

                if (usingHW) {
                    // SEND THE NEXT OUTER TRANSITION CURVE
                    HOS_NOTE("Send Outer Transition");
                    HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
                    HOS_PUSH(1, NV097_SET_BEGIN_END_CURVE_CMD_OUTER_TRANSITION_CURVE);
                    HOS_PUSH_ADJUST(2);
                    for (index = 0; index < info->maxAttr; index++) {
                        if (info->evalEnables & (1 << index)) {
                            // if this is a not a new swath... do we pre adjust that row???
                            // or just not send the last row in the previous patch?
                            dwCount = 0;
                            for (i = 0; i < (*pCurves)[index]->order; i++) {
                                HOS_NOTE("Set Curve Coefficients");
                                HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
                                HOS_PUSHF(dwCount+1, VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][0]));
                                HOS_PUSHF(dwCount+2, VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][1]));
                                HOS_PUSHF(dwCount+3, VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][2]));
                                HOS_PUSHF(dwCount+4, VIEW_AS_DWORD((*pCurves)[index]->coeffs[i][3]));
                                dwCount+=5;
                            }
                            HOS_PUSH_ADJUST(dwCount);
                        }
                    }
                    HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
                    HOS_PUSH(1, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
                    HOS_PUSH_ADJUST(2);
                    HOS_FLUSH();
                } else {
                    EvalStripIdx(info, pCurves, oneToDoOut, pDataOut, out, 0); // outer

                    // Force last point
                    if (lastOutsideCurve) {
                        for (index = 0; index < info->maxAttr; index++) {
                            if (!EV_GUARD_ATTR(index)  &&  !doFrac) {
                                continue;
                            }
                            if (info->evalEnables & (1 << index)) {
                                CopyPoint(pDataOut->vertexAttribs[EVAL_IWRAP(out+oneToDoOut-1)][index],
                                          quadInfo->pCorners[index][cornerIdxV][cornerIdxU]);
                            }
                        }
                    }

                }

                onCurveOut++;
            }

            if (!usingHW) {
                if (inStrip && !lastWasIn) {
                    info->backend->endPrimitive(info);
                    fanCenter = in;
                    fanFirst = out-1;
                    inStrip = 0;
                    if (lastFrontFace != (1 ^ reverse ^ info->reverse)) {
                        info->backend->setFrontFace(info, lastFrontFace = !lastFrontFace);
                    }
                    info->backend->beginPrimitive(info, NV_PATCH_PRIMITIVE_TFAN);
                    info->backend->sendVertex(info, pDataIn, EVAL_IWRAP(fanCenter));
                    info->backend->sendVertex(info, pDataOut, EVAL_IWRAP(fanFirst));

                } else if (!inStrip && lastWasIn) {
                    info->backend->endPrimitive(info);
                    info->backend->beginPrimitive(info, NV_PATCH_PRIMITIVE_TSTRIP);
                    info->backend->sendVertex(info, pDataOut, EVAL_IWRAP(out-1));
                    info->backend->sendVertex(info, pDataIn, EVAL_IWRAP(in));
                    inStrip = 1;
                }
                if (inStrip) {
                    info->backend->sendVertex(info, pDataOut, EVAL_IWRAP(out));
                } else {
                    fanLast = out;
                    info->backend->sendVertex(info, pDataOut, EVAL_IWRAP(fanLast));
                }
                lastWasIn = 0;
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
    nvAssert(onCurveIn == nCurveIn);

    if (usingHW) {
        HOS_NOTE("END Transition");
        HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_END_TRANSITION)));
        HOS_PUSH(1, 0);
        HOS_PUSH_ADJUST(2);
        HOS_FLUSH();
    } else {
        info->backend->endPrimitive(info);

        info->backend->setFrontFace(info, 0);
    }
}


/*****************************************************************************/
unsigned int DrawIntPatch(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, FDMatrix **matrices);
unsigned int DrawIntPatchGrid(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, FDMatrix **matrices);

unsigned int DrawFracPatch(NV_PATCH_INFO *info,
                           int botOutSegs, int botInSegs,
                           int leftOutSegs, int leftInSegs,
                           int reverse, FDMatrix **matrices)
{
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
void
evalPrepareInnerCurves(NV_PATCH_INFO *info, int bigStep, FDMatrix **matrices)
// XXX for swathing, calc the inner curves at the appropriate swatch
{
    NV_PATCH_QUAD_INFO *quadInfo = info->quadInfo;
    int doFrac = !!(info->flags & NV_PATCH_FLAG_TESS_FRACTIONAL);
    FDMatrix *pMatrix;
    int attr, needRetess;

    BEGIN_USING_TEMP_MATRIX(info->tempMatrix_m2);
    if (!(info->swatchFlags & NV_PATCH_SWATCH)) {
        needRetess = 0;
    } else {
        needRetess = 1;
    }

    for (attr = 0;  attr < info->maxAttr;  attr++) {
        if (!(info->evalEnables & (1 << attr))) {
            continue;
        }

        // If we're stitching on the top or bottom, do a U inner guard
        if (quadInfo->needUInner) {
            if (needRetess) {
                pMatrix = &info->tempMatrix_m2;
                CopyMatrix(info, pMatrix, matrices[attr]);
                // big to small, only in V so extract produces a big-step curve
                RetessMatrixV(info, 0, pMatrix);
            } else {
                pMatrix = matrices[attr];
            }
            if (quadInfo->stitchVBegin) {
                OffsetFDMatrix_Extract_Discard(info, quadInfo->pUInner[attr], 0, pMatrix, 0, 1);
            } else {
                nvAssert(quadInfo->stitchVEnd);
                OffsetFDMatrix_Extract_Discard(info, quadInfo->pUInner[attr], 0, pMatrix, 0, quadInfo->vMaxSegs-1);
            }
            if (doFrac) {
                // Calc UInnerFrac
                OffsetFDMatrix_Extract_Discard(info, info->guardSetUInnerFrac[attr], 0, pMatrix, 0, quadInfo->vMaxSegs-1);
            }
        }

        // If we're stitching on the left or right, do a V inner guard
        if (quadInfo->needVInner) {
            if (needRetess) {
                pMatrix = &info->tempMatrix_m2;
                CopyMatrix(info, pMatrix, matrices[attr]);
                // big to small, only in U so extract produces a big-step curve
                RetessMatrixU(info, 0, pMatrix);
            } else {
                pMatrix = matrices[attr];
            }
            if (quadInfo->stitchUBegin) {
                OffsetFDMatrix_Extract_Discard(info, quadInfo->pVInner[attr], 1, pMatrix, 1, 0);
            } else {
                nvAssert(quadInfo->stitchUEnd);
                OffsetFDMatrix_Extract_Discard(info, quadInfo->pVInner[attr], 1, pMatrix, quadInfo->uMaxSegs-1, 0);
            }
            if (doFrac) {
                // Calc VInnerFrac
                OffsetFDMatrix_Extract_Discard(info, info->guardSetVInnerFrac[attr], 1, pMatrix, quadInfo->uMaxSegs-1, 0);
            }
        }
    }
    END_USING_TEMP_MATRIX(info->tempMatrix_m2);
}

/*****************************************************************************/
/*****************************************************************************/
static unsigned int
FillIntCornerLL(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, NV_PATCH_QUAD_INFO *quadInfoOrig,
            int index, float *pPoint)
{
    NV_PATCH_CURVE_INFO *tempCurve = &info->tempCurve;

    if (quadInfo->stitchVBegin) {
        nvAssert(!quadInfo->stitchVEnd);
        if (quadInfo->vMaxSegs > 1) {
            if (quadInfo->stitchUBegin) {
                CopyCurve(info, tempCurve, (*quadInfo->pSwatchUBegin)[index]);
                OffsetCurve(info, tempCurve, 1);
                CopyPoint(pPoint, tempCurve->coeffs[0]);
                return 0;
            } else {
                // since outside edge takes priority, step along VBegin, and tweak pSwatchUBegin
                CopyCurve(info, tempCurve, (*quadInfo->pSwatchVBegin)[index]);
                OffsetCurve(info, tempCurve, 1);
                CopyPoint(pPoint, tempCurve->coeffs[0]);
                CopyPoint((*quadInfo->pSwatchUBegin)[index]->coeffs[0], pPoint);
                return 0;
            }
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
static unsigned int
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
static unsigned int
FillIntCornerUL(NV_PATCH_INFO *info, NV_PATCH_QUAD_INFO *quadInfo, NV_PATCH_QUAD_INFO *quadInfoOrig,
            int index, float *pPoint)
{
    NV_PATCH_CURVE_INFO *tempCurve = &info->tempCurve;

    if (quadInfo->stitchVEnd) {
        nvAssert(!quadInfo->stitchVBegin);
        if (quadInfo->vMaxSegs > 1) {
            if (quadInfo->stitchUBegin) {
                CopyCurve(info, tempCurve, (*quadInfo->pSwatchUEnd)[index]);
                OffsetCurve(info, tempCurve, 1);
                CopyPoint(pPoint, tempCurve->coeffs[0]);
            } else {
                CopyPoint(pPoint, (*quadInfo->pSwatchUEnd)[index]->coeffs[0]);
            }
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
static unsigned int
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
    NV_PATCH_CURVE_COEFFS *pCoeffs[NV_PATCH_NUMBER_OF_ATTRIBS];
    HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();
    int usingHW = info->usingHW;

    tempVBegin = info->tempVBegin;
    tempVEnd = info->tempVEnd;
    tempCurve = &info->tempCurve;
    ppMatrixSetSS0 = info->ppMatrixSetSS0;
    pCurvesTop = quadInfo->pCurvesTop;
    pCurvesBot = quadInfo->pCurvesBot;
    pOut1 = quadInfo->pOut1;
    pOut2 = quadInfo->pOut2;

#if defined(ENABLE_DEBUG_SWATH) && defined(IS_OPENGL)
    if (!usingHW && nvDebugLevel >= 3 && (evalTrace & 1)) {
        // draw red X through each sub-patch using corner points

        glDisable(GL_TEXTURE_2D);
        glPushMatrix();
        glTranslatef(0, 0, .1);
        glBegin(GL_LINE_STRIP);
            glColor3f(1, 0, 0); glVertex3fv(info->pSwatchCorner[0][0][0]);
            glColor3f(1, 0, 0); glVertex3fv(info->pSwatchCorner[0][0][1]);
            glColor3f(0, 1, 0); glVertex3fv(info->pSwatchCorner[0][1][0]); 
            glColor3f(0, 1, 0); glVertex3fv(info->pSwatchCorner[0][1][1]);
            glColor3f(1, 0, 0); glVertex3fv(info->pSwatchCorner[0][0][0]);
        glEnd();

        glPopMatrix();
        glColor3f(1, 1, 0);
        if (info->evalEnables & (1 << NV_PATCH_ATTRIB_TEXCOORD0)) {
            glEnable(GL_TEXTURE_2D);
        }
    }
#endif

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
    useVEnd = 0; //doFrac && (info->swatchFlags & NV_PATCH_SWATCH_LAST_COL);

    for (index = 0; index < info->maxAttr; index++) {
        if (info->evalEnables & (1 << index)) {

            // copy and step the forward differencing matrix to the first row of regular grid (as for HW)
            CopyMatrix(info, ppMatrixSetSS0[index], matrices[index]);

#if defined(IS_OPENGL) && defined(COMPILE_TRACE_LIBRARY)
            if ((1 << index) & EV_GUARD_BITS) {
                if (!quadInfo->stitchVBegin) {
                    nvAssert(PointsEqual(tempVBegin[index]->coeffs[0], info->pSwatchCorner[index][0][0]));
                    nvAssert(PointsEqual(tempVEnd[index]->coeffs[0],   info->pSwatchCorner[index][0][1]));
                }

                if ((info->swatchFlags & NV_PATCH_SWATCH_FIRST_ROW) && !quadInfo->stitchVBegin
                            &&  !quadInfo->stitchUBegin) { // && !quadInfo->needUInner ?
                    // the guard start should match pSwatchUBegin
                    nvAssert(PointsEqual(tempVBegin[index]->coeffs[0], (*quadInfo->pSwatchUBegin)[index]->coeffs[0]));
                    // tempVBegin should be extracted from matrix
                    nvAssert(PointsEqual(tempVBegin[index]->coeffs[0], ppMatrixSetSS0[index]->data[0][0]));
                }

                if (0 && nvDebugLevel >= 4) {
                    if (info->swatchFlags & NV_PATCH_SWATCH_FIRST_ROW) {
                        if (!(quadInfo->stitchVBegin)) {
                            CompareUCurveMatrix((*quadInfo->pSwatchUBegin)[index], matrices[index]);
                        }
                    }
                    if (info->swatchFlags & NV_PATCH_SWATCH_FIRST_COL) {
                        if (!(quadInfo->stitchUBegin)) {
                            CompareVCurveMatrix((*quadInfo->pSwatchVBegin)[index], matrices[index]);
                        }
                    }
                }

            }
#endif

            OffsetFDMatrix(info, ppMatrixSetSS0[index], 0, quadInfo->stitchVBegin);

            if (usingHW) {
                continue;
            }
            if (!EV_GUARD_ATTR(index)/*  &&  !doFrac*/) {
                continue;
            }

#if defined(IS_OPENGL) && defined(COMPILE_TRACE_LIBRARY)
            // test code for guard curve
            if (0 && nvDebugLevel >= 4) {
                if ((*quadInfo->pSwatchUBegin)[index]) {
                    CopyCurve(info, tempCurve, (*quadInfo->pSwatchUBegin)[index]);
                    OffsetCurve(info, tempCurve, quadInfo->stitchUBegin);
                    CompareUCurveMatrix(tempCurve, ppMatrixSetSS0[index]);
                }

                if ((*quadInfo->pSwatchVBegin)[index]) {
                    CopyCurve(info, tempCurve, (*quadInfo->pSwatchVBegin)[index]);
                    OffsetCurve(info, tempCurve, quadInfo->stitchVBegin);
                    CompareVCurveMatrix(tempCurve, ppMatrixSetSS0[index]);
                }
            }
#endif

            // Step V Begin/End curves
            if (quadInfo->stitchVBegin) {
                OffsetCurve(info, tempVBegin[index], 1);
                OffsetCurve(info, tempVEnd[index], 1);
                if (!quadInfo->stitchUBegin) {
                    // tweak UInner curve to start at guard locations
                    CopyPoint((*quadInfo->pSwatchUBegin)[index]->coeffs[0], tempVBegin[index]->coeffs[0]);
                }
            }
        }
    }

    if (usingHW) {
        //BEGIN_END_SWATCH/SWATH?
        unsigned long dwFormat;
        dwFormat = NV097_SET_BEGIN_END_SWATCH_SWATCH_CMD_BEGIN;
        dwFormat |= (((info->swatchFlags  & NV_PATCH_SWATCH_FIRST_ROW) ? NV097_SET_BEGIN_END_SWATCH_NEW_SWATH_NEW : NV097_SET_BEGIN_END_SWATCH_NEW_SWATH_CONTINUE)   << 4);
        dwFormat |= ((quadInfo->stitchVBegin ? NV097_SET_BEGIN_END_SWATCH_SKIP_FIRST_ROW_TRUE : NV097_SET_BEGIN_END_SWATCH_SKIP_FIRST_ROW_FALSE) << 8);
        dwFormat |= ((quadInfo->stitchUBegin ? NV097_SET_BEGIN_END_SWATCH_SKIP_FIRST_COL_TRUE : NV097_SET_BEGIN_END_SWATCH_SKIP_FIRST_COL_FALSE) << 12);
        dwFormat |= (((quadInfo->vMaxSegs - quadInfo->stitchVEnd < info->maxSwatch) ?  NV097_SET_BEGIN_END_SWATCH_SHORT_SWATCH_PARTIAL_HEIGHT : NV097_SET_BEGIN_END_SWATCH_SHORT_SWATCH_FULL_HEIGHT) << 16);
        dwFormat |= (((quadInfo->uMaxSegs - quadInfo->stitchUEnd < info->maxSwatch) ?  NV097_SET_BEGIN_END_SWATCH_NARROW_SWATCH_PARTIAL_WIDTH : NV097_SET_BEGIN_END_SWATCH_NARROW_SWATCH_FULL_WIDTH) << 20);

        HOS_NOTE("Begin Swatch");
        HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_SWATCH)));
        HOS_PUSH(1, dwFormat);
        HOS_PUSH_ADJUST(2);

        //send guard curves!!!!
        SendGuardCurve(NV097_SET_BEGIN_END_CURVE_CMD_LEFT_GUARD_CURVE,  info, quadInfo, info->swatchFlags );
        SendGuardCurve(NV097_SET_BEGIN_END_CURVE_CMD_RIGHT_GUARD_CURVE, info, quadInfo, info->swatchFlags );
    }

    // prep INT patch
    if (info->swatchFlags & NV_PATCH_SWATCH_FIRST_ROW) {
        for (index = 0; index < info->maxAttr; index++) {
            if (info->evalEnables & (1 << index)) {
                pCoeffs[index] = &(*quadInfo->pSwatchUBegin)[index]->coeffs;

                if (!usingHW) {
                    // no UInner curve, the last curve should be taken from the patch edge
                    CopyCoeffs(info, &(*pCurvesBot)[index]->coeffs, pCoeffs[index]);
                }
            }
        }

        if (usingHW) {
            unsigned long dwCount;  //precount the first two methods.
            int i;
            HOS_NOTE("Begin Curve");
            HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
            HOS_PUSH(1, NV097_SET_BEGIN_END_CURVE_CMD_STRIP_CURVE);
            HOS_PUSH_ADJUST(2);
            for (index = 0; index < info->maxAttr; index++) {
                if (info->evalEnables & (1 << index)) {
                    //if this is a not a new swath... do we pre adjust that row??? or just not send the last row in the previous patch?
                    dwCount = 0;
                    for (i = 0; i < ppMatrixSetSS0[index]->columns; i++) {
                        HOS_NOTE("Set Curve Coefficients");
                        HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
                        HOS_PUSHF(dwCount+1, VIEW_AS_DWORD((*pCoeffs[index])[i][0]));
                        HOS_PUSHF(dwCount+2, VIEW_AS_DWORD((*pCoeffs[index])[i][1]));
                        HOS_PUSHF(dwCount+3, VIEW_AS_DWORD((*pCoeffs[index])[i][2]));
                        HOS_PUSHF(dwCount+4, VIEW_AS_DWORD((*pCoeffs[index])[i][3]));
                        dwCount+=5;
                    }
                    HOS_PUSH_ADJUST(dwCount);
                }
            }
            HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
            HOS_PUSH(1, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
            HOS_PUSH_ADJUST(2);

        } else {
            // first strip of regular grid. Will become first bot strip.
            EvalStrip(info, pCurvesBot, 1+quadInfo->uMaxSegs - quadInfo->stitchUEnd, pOut2, 0);
            for (index = 0; index < info->maxAttr; index++) {
                if (info->evalEnables & (1 << index)) {
                    if (EV_GUARD_ATTR(index) || useVBegin) {
                        if (info->swatchFlags & NV_PATCH_SWATCH_FIRST_COL) {
                            // Force last point from grid corner
                            CopyPoint(pOut2->vertexAttribs[quadInfo->stitchUBegin][index],
                                                            info->gridCorner[index][1-quadInfo->vMaxDir][1-quadInfo->uMaxDir]);
                        } else {
                            // Force first point from guard curve
                            CopyPoint(pOut2->vertexAttribs[quadInfo->stitchUBegin][index], tempVBegin[index]->coeffs[0]);
                        }
                    }
                    if (EV_GUARD_ATTR(index) || useVEnd) {
                        if (info->swatchFlags & NV_PATCH_SWATCH_LAST_COL) {
                            // Force last point from grid corner
                            CopyPoint(pOut2->vertexAttribs[iEvalRight][index],
                                                            info->gridCorner[index][1-quadInfo->vMaxDir][  quadInfo->uMaxDir]);
                        } else {
                            // Force last point from guard curve
                            CopyPoint(pOut2->vertexAttribs[iEvalRight][index], tempVEnd[index]->coeffs[0]);
                        }
                    }
                }
            }
        }

    } else {
        // not first row
        for (index = 0; index < info->maxAttr; index++) {
            if (info->evalEnables & (1 << index)) {
                // tweak 
            }
        }
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
                    if (quadInfo->stitchVEnd) {
                        float t4[4];
                        int doTweak = !quadInfo->stitchUBegin && EV_GUARD_ATTR(index);

                        if (doTweak) {
                            // save curve, but preserve starting point
                            // copy since swatchCorner may be the same as pSwatchUEnd
                            CopyPoint(t4, info->pSwatchCorner[index][1][0]);
                            ExtractUCurve(info, (*quadInfo->pSwatchUEnd)[index], ppMatrixSetSS0[index]);
                        }

                        if (doTweak) {
                            // but tweak to swatchCorner set up by higher level routine
                            CopyPoint((*quadInfo->pSwatchUEnd)[index]->coeffs[0], t4);
                        }
                    }
                    pCoeffs[index] = &(*quadInfo->pSwatchUEnd)[index]->coeffs;
                } else {
                    // "inside" regular grid
                    pCoeffs[index] = &ppMatrixSetSS0[index]->data[0];
                }

                if (!usingHW) {
                    CopyCoeffs(info, &(*pCurvesTop)[index]->coeffs, pCoeffs[index]);
                }
            }
        }

        if (iy == ylimit  &&  !doFrac  &&  (info->swatchFlags & NV_PATCH_SWATCH_LAST_ROW)) {
            //ffif (quadInfo->stitchVEnd && !quadInfo->stitchUBegin) {
            if (quadInfo->stitchVEnd) {
                // tweak UInner curve to start at guard locations
                //ffCopyPoint((*quadInfo->pSwatchUEnd)[ATTR_V]->coeffs[0], tempVBegin[ATTR_V]->coeffs[0]);
                //ff(*quadInfo->pSwatchUEnd)[ATTR_V]->coeffs[0][1] += .05;
            }
        }

        if (!usingHW) {
            // eval "top" strip
            EvalStrip(info, pCurvesTop, 1+quadInfo->uMaxSegs - quadInfo->stitchUEnd, pOut1, 0);

            if (iy == ylimit  &&  !doFrac) {
                // force first and last point from swatch corner
                for (index = 0; index < info->maxAttr; index++) {
                    if (!(info->evalEnables & (1 << index))) {
                        continue;
                    }
                    if (EV_GUARD_ATTR(index)) {
                        CopyPoint(pOut1->vertexAttribs[quadInfo->stitchUBegin][index], info->pSwatchCorner[index][1][0]);
                        CopyPoint(pOut1->vertexAttribs[iEvalRight][index], info->pSwatchCorner[index][1][1]);
                        continue;
                    }
                    if (!doFrac) {
                        continue;
                    }
                    if (!useVEnd) {
                        continue;
                    }
                    // Force first point from guard curve
                    //ffCopyPoint(pOut1->vertexAttribs[quadInfo->stitchUBegin][index], tempVBegin[index]->coeffs[0]);

                    if (!(info->swatchFlags & NV_PATCH_SWATCH_LAST_ROW)) {
                        // Force last point from guard curve
                        //ffCopyPoint(pOut1->vertexAttribs[iEvalRight][index], tempVEnd[index]->coeffs[0]);
                    } else {
                        //ff// get first point from corner
                        //ffCopyPoint(pOut1->vertexAttribs[quadInfo->stitchUBegin][index], info->pSwatchCorner[index][1][0]);
                        // get end point from corner
                        CopyPoint(pOut1->vertexAttribs[iEvalRight][index], info->pSwatchCorner[index][1][1]);
                    }
                }

            } else {
                for (index = 0; index < info->maxAttr; index++) {
                    if (info->evalEnables & (1 << index)) {
                        if (EV_GUARD_ATTR(index)  ||  useVBegin) {
                            // Force first point from guard curve
                            CopyPoint(pOut1->vertexAttribs[quadInfo->stitchUBegin][index], tempVBegin[index]->coeffs[0]);
                        }
                        if (EV_GUARD_ATTR(index)  ||  useVEnd) {
                            // Force last point from guard curve
                            CopyPoint(pOut1->vertexAttribs[iEvalRight][index], tempVEnd[index]->coeffs[0]);
                        }
                    }
                }
            }
        }

        // avoid strip when degenerate regular grid (have only bottom stitch)
        // and avoid degenerate strip at top of regular grid
        if (!degen) {
            if (usingHW) {
                unsigned long dwCount;  //precount the first two methods.
                int i;
                HOS_NOTE("Begin Curve");
                HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
                HOS_PUSH(1, NV097_SET_BEGIN_END_CURVE_CMD_STRIP_CURVE);
                HOS_PUSH_ADJUST(2);
                for (index = 0; index < info->maxAttr; index++) {
                    if (info->evalEnables & (1 << index)) {
                        dwCount = 0;
                        //if this is a not a new swath... do we pre adjust that row??? or just not send the last row in the previous patch?
                        for (i = 0; i < ppMatrixSetSS0[index]->columns; i++) {
                            HOS_NOTE("Set Curve Coefficients");
                            HOS_PUSH(dwCount, ((0x4 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_CURVE_COEFFICIENTS(0))));
                            HOS_PUSHF(dwCount+1, VIEW_AS_DWORD((*pCoeffs[index])[i][0]));
                            HOS_PUSHF(dwCount+2, VIEW_AS_DWORD((*pCoeffs[index])[i][1]));
                            HOS_PUSHF(dwCount+3, VIEW_AS_DWORD((*pCoeffs[index])[i][2]));
                            HOS_PUSHF(dwCount+4, VIEW_AS_DWORD((*pCoeffs[index])[i][3]));
                            dwCount+=5;
                        }
                        HOS_PUSH_ADJUST(dwCount);
                    }
                }
                HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_CURVE)));
                HOS_PUSH(1, NV097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA);
                HOS_PUSH_ADJUST(2);

            } else {
                // output a strip of the regular grid
                info->backend->beginPrimitive(info, NV_PATCH_PRIMITIVE_TSTRIP);
                if (!(quadInfo->uMaxDir ^ quadInfo->vMaxDir ^ info->flipUV)) {
                    // "normal", start with top-bot-top
                    for (ix = quadInfo->stitchUBegin; ix <= quadInfo->uMaxSegs - quadInfo->stitchUEnd; ix++) {
                        info->backend->sendVertex(info, pOut1, ix);
                        info->backend->sendVertex(info, pOut2, ix);
                    }
                } else {
                    // flipped case
                    for (ix = quadInfo->stitchUBegin; ix <= quadInfo->uMaxSegs - quadInfo->stitchUEnd; ix++) {
                        info->backend->sendVertex(info, pOut2, ix);
                        info->backend->sendVertex(info, pOut1, ix);
                    }
                }
                info->backend->endPrimitive(info);
            }
        }
        pTemp = pOut1;
        pOut1 = pOut2;
        pOut2 = pTemp;
        pCurvesTemp = pCurvesTop;
        pCurvesTop = pCurvesBot;
        pCurvesBot = pCurvesTemp;
    }

    if (usingHW) {
        HOS_NOTE("End Swatch");
        HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_BEGIN_END_SWATCH)));
        HOS_PUSH(1, NV097_SET_BEGIN_END_SWATCH_SWATCH_CMD_END);
        HOS_PUSH_ADJUST(2);
    }

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

    NV_TRACE_COND(TR_EVAL, 25,
        TPRINTF(("DrawAllPatchTransitions nU0=%d nU1=%d nV0=%d nV1=%d\n",
                quadInfo->nu0, quadInfo->nu1, quadInfo->nv0, quadInfo->nv1));
        TPRINTF(("\tdir U0=%d U1=%d V0=%d V1=%d\n",
                quadInfo->u0Dir, quadInfo->u1Dir, quadInfo->v0Dir, quadInfo->v1Dir));
        TPRINTF(("\tmaxDir U=%d V=%d\n", quadInfo->uMaxDir, quadInfo->vMaxDir));
    );

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
assert(0);
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
                        1, 1-quadInfo->uMaxDir,  // for inner beginpoint from regular grid corner
                        1, quadInfo->uMaxDir,  // for inner endpoint from regular grid corner
                        1, quadInfo->u1Dir,    // for outer endpoint from patch corners[][]
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
                        1-quadInfo->vMaxDir, 1,   // for inner beginpoint from regular grid corner
                        quadInfo->vMaxDir, 1,   // for inner endpoint from regular grid corner
                        quadInfo->v1Dir, 1,     // for outer endpoint from patch corners[][]
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
                    0, 1-quadInfo->uMaxDir,   // for inner beginpoint from regular grid corner
                    0, quadInfo->uMaxDir,   // for inner endpoint from regular grid corner
                    0, quadInfo->u0Dir,     // for outer endpoint from patch corners[][]
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
                    1-quadInfo->vMaxDir, 0,   // for inner beginpoint from regular grid corner
                    quadInfo->vMaxDir, 0,   // for inner endpoint from regular grid corner
                    quadInfo->v0Dir, 0,     // for outer endpoint from patch corners[][]
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
                1, 1-quadInfo->uMaxDir,   // for inner beginpoint from regular grid corner
                1, quadInfo->uMaxDir,   // for inner endpoint from regular grid corner
                1, quadInfo->u1Dir,     // for outer endpoint from patch corners[][]
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
                0, 1-quadInfo->uMaxDir,   // for inner beginpoint from regular grid corner
                0, quadInfo->uMaxDir,   // for inner endpoint from regular grid corner
                0, quadInfo->u0Dir,     // for outer endpoint from patch corners[][]
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
                1-quadInfo->vMaxDir, 1,   // for inner beginpoint from regular grid corner
                quadInfo->vMaxDir, 1,   // for inner endpoint from regular grid corner
                quadInfo->v1Dir, 1,     // for outer endpoint from patch corners[][]
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
                1-quadInfo->vMaxDir, 0,   // for inner beginpoint from regular grid corner
                quadInfo->vMaxDir, 0,   // for inner endpoint from regular grid corner
                quadInfo->v0Dir, 0,     // for outer endpoint from patch corners[][]
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
                0, 1-quadInfo->uMaxDir,   // for inner beginpoint from regular grid corner
                0, quadInfo->uMaxDir,   // for inner endpoint from regular grid corner
                0, quadInfo->u0Dir,     // for outer endpoint from patch corners[][]
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
                1, 1-quadInfo->uMaxDir,   // for inner beginpoint from regular grid corner
                1, quadInfo->uMaxDir,   // for inner endpoint from regular grid corner
                1, quadInfo->u1Dir,     // for outer endpoint from patch corners[][]
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
                1-quadInfo->vMaxDir, 0,   // for inner beginpoint from regular grid corner
                quadInfo->vMaxDir, 0,   // for inner endpoint from regular grid corner
                quadInfo->v0Dir, 0,     // for outer endpoint from patch corners[][]
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
                1-quadInfo->vMaxDir, 1,   // for inner beginpoint from regular grid corner
                quadInfo->vMaxDir, 1,   // for inner endpoint from regular grid corner
                quadInfo->v1Dir, 1,     // for outer endpoint from patch corners[][]
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
static NV_INLINE NV_PATCH_CURVE_INFO *
pGuardSetCurve(NV_PATCH_INFO *info, FDCurveAttrSet *pBase, int attribIndex)
{
    nvAssert(attribIndex < info->maxAttr);
    return (*pBase)[attribIndex];
}

// draw series of sub-patches in swaths.
// General algorithm:
//  1) high level code computes for "normal" step sizes. This is to optimize
//      for smaller cases where swathing is not needed.
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
    int usingHW = info->usingHW;

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
    //ffFDCurveAttrSet lguardSetUBegin;
    //ffFDCurveAttrSet lguardSetUEnd;
    //ffFDCurveAttrSet lguardSetVBegin, lguardSetVEnd;

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
    int enablesSU0, enablesSU1, enablesSV0, enablesSV1;
    FDCurveAttrSet  BV0;        // curve from LL to UL, for UInner curve list, big steps
    FDCurveAttrSet  SV0;        // curve from LL to UL, for UInner curve list, small steps
    FDCurveAttrSet  BV1;        // curve from LR to UR, for UInner curve list, big steps
    FDCurveAttrSet  SV1;        // curve from LR to UR, for UInner curve list, small steps
    FDCurveAttrSet  *pSU0;      // ptr to SU0 or ppGuardSetUBegin[onCol]
    FDCurveAttrSet  *pSU1;      // ptr to SU1 or ppGuardSetUEnd[onCol]
    FDCurveAttrSet  *pSV0;      // ptr to SV0 or ppGuardSetVBegin[onRow]
    FDCurveAttrSet  *pSU1Next;  // ptr to next pSU1
    FDCurveAttrSet  *pSV0Next;  // ptr to next pSV0
    FDCurveAttrSet  *pSV1Next;  // ptr to next pSV1
    //ffFDCurveAttrSet  *pBV1;      // ptr to current BV1 (maybe really SV1, but only used to get start point)
    FDCurveAttrSet  *pSV1;      // ptr to current SV1 or ppGuardSetVEnd[onRow]
    //ffFDCurveAttrSet  *pTempCurveSet;

    float LL[NV_PATCH_NUMBER_OF_ATTRIBS][4];
    float LR[NV_PATCH_NUMBER_OF_ATTRIBS][4];
    float UL[NV_PATCH_NUMBER_OF_ATTRIBS][4];
    float UR[NV_PATCH_NUMBER_OF_ATTRIBS][4];        // vertex: for extracting from BS0 and BV1
    float *pLL[NV_PATCH_NUMBER_OF_ATTRIBS];
    float *pLR[NV_PATCH_NUMBER_OF_ATTRIBS];
    float *pUL[NV_PATCH_NUMBER_OF_ATTRIBS];
    float *pUR[NV_PATCH_NUMBER_OF_ATTRIBS];         // vertex: ptr to swatch corner points

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

    evalPrepareInnerCurves(info, 1, matrices);      // prepare the *Inner and *InnerFrac curves

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
    //ffquadInfo->pSwatchVBegin = &lguardSetVBegin;
    //ffquadInfo->pSwatchVEnd = &lguardSetVEnd;

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
        RetessCurveInOut(info, 0, quadInfoOrig->pU0[index], pGuardSetCurve(info, ppGuardSetU0[0], index));
        CopyCurve(info, BV1[index], quadInfoOrig->pU1[index]);
        RetessCurveInOut(info, 0, quadInfoOrig->pU1[index], pGuardSetCurve(info, ppGuardSetU1[0], index));

        if (quadInfo->needUInner) {
            CopyCurve(info, SV0[index], quadInfoOrig->pUInner[index]);
            RetessCurveInOut(info, 0, quadInfoOrig->pUInner[index], pGuardSetCurve(info, ppGuardSetUInner[0], index));
        }
        //ffif (doFrac  &&  EV_GUARD_ATTR(index)) { // }
        if (doFrac) {
            CopyCurve(info, SV1[index], pGuardSetCurve(info, ppGuardSetUInnerFrac[0], index));
            RetessCurveInOut(info, 0, SV1[index], pGuardSetCurve(info, ppGuardSetUInnerFrac[0], index));
        }
    }

    // make big steps and cvt to small for later
    for (onCol = 1;  onCol <= stitch_nSwatchU;  onCol++) {
        // copy all prev attrs
        __NV_MEMCPY(pGuardSetCurve(info, ppGuardSetU0[onCol], info->firstAttr),
                    pGuardSetCurve(info, ppGuardSetU0[onCol-1], info->firstAttr), 3*bytesGuardCurveAllAttr);
        for (index = 0;  index < info->maxAttr;  index++) {
            if (!(info->evalEnables & (1 << index))) {
                continue;
            }
            OffsetCurve(info, BV0[index], 1);
            RetessCurveInOut(info, 0, BV0[index], pGuardSetCurve(info, ppGuardSetU0[onCol], index));
            OffsetCurve(info, BV1[index], 1);
            RetessCurveInOut(info, 0, BV1[index], pGuardSetCurve(info, ppGuardSetU1[onCol], index));
            //ffif (quadInfo->needUInner && quadInfo->stitchVEnd && (EV_GUARD_ATTR(index) || !usingHW)) { // }
            if (quadInfo->needUInner) {
                OffsetCurve(info, SV0[index], 1);
                RetessCurveInOut(info, 0, SV0[index], pGuardSetCurve(info, ppGuardSetUInner[onCol], index));
            }
            //ffif (doFrac  &&  EV_GUARD_ATTR(index)) { // }
            if (doFrac) {
                OffsetCurve(info, SV1[index], 1);
                RetessCurveInOut(info, 0, SV1[index], pGuardSetCurve(info, ppGuardSetUInnerFrac[onCol], index));
            }
        }
    }

#if defined(ENABLE_DEBUG_SWATH)
    if (!usingHW && nvDebugLevel >= 4 && (evalTrace & 4)) {
        // red lines from "outside" to swatch boundaries for U0
        // grn lines from "outside" to swatch boundaries for U1
        // blue lines from center to swatch boundaries for UInner
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_LINES);
        for (onCol = 0;  onCol <= stitch_nSwatchU;  onCol++) {
            if (info->evalEnables & (1 << 3)) {
                glColor3fv(pGuardSetCurve(info, ppGuardSetU0[onCol], 3)->coeffs[0]);
            } else {
                glColor3f(1, 0, 0);
            }
            glVertex3f(.5, -1, 0); glVertex3fv(pGuardSetCurve(info, ppGuardSetU0[onCol], 0)->coeffs[0]);

            if (info->evalEnables & (1 << 3)) {
                glColor3fv(pGuardSetCurve(info, ppGuardSetU1[onCol], 3)->coeffs[0]);
            } else {
                glColor3f(0, 1, 0);
            }
            glVertex3f(.5, 2, 0); glVertex3fv(pGuardSetCurve(info, ppGuardSetU1[onCol], 0)->coeffs[0]);

            if (quadInfo->needUInner) {
                if (info->evalEnables & (1 << 3)) {
                    glColor3fv(pGuardSetCurve(info, ppGuardSetUInner[onCol], 3)->coeffs[0]);
                } else {
                    glColor3f(0, 0, 1);
                }
                glVertex3f(.5, .5, 0); glVertex3fv(pGuardSetCurve(info, ppGuardSetUInner[onCol], 0)->coeffs[0]);
            }
        }
        glEnd();
        if (!(info->evalEnables & (1 << 3))) {
            glColor3f(.5, .5, .5);
        }
        if (info->evalEnables & (1 << NV_PATCH_ATTRIB_TEXCOORD0)) {
            glEnable(GL_TEXTURE_2D);
        }
    }
#endif

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
        RetessCurveInOut(info, 0, quadInfoOrig->pV0[index], pGuardSetCurve(info, ppGuardSetV0[0], index));
        CopyCurve(info, BV1[index], quadInfoOrig->pV1[index]);
        RetessCurveInOut(info, 0, quadInfoOrig->pV1[index], pGuardSetCurve(info, ppGuardSetV1[0], index));

        //ffif (quadInfo->needVInner && quadInfo->stitchUEnd) {
        if (quadInfo->needVInner) {
            CopyCurve(info, SV0[index], quadInfoOrig->pVInner[index]);
            RetessCurveInOut(info, 0, quadInfoOrig->pVInner[index], pGuardSetCurve(info, ppGuardSetVInner[0], index));
        }
        if (doFrac) {
            CopyCurve(info, SV1[index], pGuardSetCurve(info, ppGuardSetVInnerFrac[0], index));
            RetessCurveInOut(info, 0, SV1[index], pGuardSetCurve(info, ppGuardSetVInnerFrac[0], index));
        }
    }

    // make big steps and cvt to small for later
    for (onRow = 1;  onRow <= stitch_nSwatchV;  onRow++) {
        // copy all prev attrs
        __NV_MEMCPY(pGuardSetCurve(info, ppGuardSetV0[onRow], info->firstAttr),
                    pGuardSetCurve(info, ppGuardSetV0[onRow-1], info->firstAttr), 3*bytesGuardCurveAllAttr);
        for (index = 0;  index < info->maxAttr;  index++) {
            if (!(info->evalEnables & (1 << index))) {
                continue;
            }
            OffsetCurve(info, BV0[index], 1);
            RetessCurveInOut(info, 0, BV0[index], pGuardSetCurve(info, ppGuardSetV0[onRow], index));
            OffsetCurve(info, BV1[index], 1);
            RetessCurveInOut(info, 0, BV1[index], pGuardSetCurve(info, ppGuardSetV1[onRow], index));
            //ffif (quadInfo->needVInner && quadInfo->stitchUEnd) {
            if (quadInfo->needVInner) {
                OffsetCurve(info, SV0[index], 1);
                RetessCurveInOut(info, 0, SV0[index], pGuardSetCurve(info, ppGuardSetVInner[onRow], index));
            }
            if (doFrac) {
                OffsetCurve(info, SV1[index], 1);
                RetessCurveInOut(info, 0, SV1[index], pGuardSetCurve(info, ppGuardSetVInnerFrac[onRow], index));
            }
        }
    }

#if defined(ENABLE_DEBUG_SWATH)
    if (!usingHW && nvDebugLevel >= 4 && (evalTrace & 8)) {
        // red lines from "outside" to swatch boundaries for V0
        // grn lines from "outside" to swatch boundaries for V1
        // violet lines from center to swatch boundaries for VInner
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_LINES);
        for (onRow = 0;  onRow < stitch_nSwatchV;  onRow++) {
            if (info->evalEnables & (1 << 3)) {
                glColor3fv(pGuardSetCurve(info, ppGuardSetV0[onRow], 3)->coeffs[0]);
            } else {
                glColor3f(1, 0, 0);
            }
            glVertex3f(-1, .5, 0); glVertex3fv(pGuardSetCurve(info, ppGuardSetV0[onRow], 0)->coeffs[0]);

            if (info->evalEnables & (1 << 3)) {
                glColor3fv(pGuardSetCurve(info, ppGuardSetV1[onRow], 3)->coeffs[0]);
            } else {
                glColor3f(0, 1, 0);
            }
            glVertex3f(2, .5, 0); glVertex3fv(pGuardSetCurve(info, ppGuardSetV1[onRow], 0)->coeffs[0]);

            if (quadInfo->needVInner) {
                if (info->evalEnables & (1 << 3)) {
                    glColor3fv(pGuardSetCurve(info, ppGuardSetVInner[onRow], 3)->coeffs[0]);
                } else {
                    glColor3f(1, 0, 1);
                }
                glVertex3f(.5, .5, 0); glVertex3fv(pGuardSetCurve(info, ppGuardSetVInner[onRow], 0)->coeffs[0]);
            }
        }
        glEnd();
        if (!(info->evalEnables & (1 << 3))) {
            glColor3f(.5, .5, .5);
        }
        if (info->evalEnables & (1 << NV_PATCH_ATTRIB_TEXCOORD0)) {
            glEnable(GL_TEXTURE_2D);
        }
    }
#endif

    /*****************************************************************************/
    // work on regular grid

    // calc swaths WITHOUT including ending stitch
    index = 1 + doFrac; // index is a temp // adjust to not draw FRAC RIGHT and TOP stitch
    nSwatchU = (quadInfo->uMaxSegs - index - quadInfo->stitchUEnd) / info->maxSwatch + 1;
    nSwatchV = (quadInfo->vMaxSegs - index - quadInfo->stitchVEnd) / info->maxSwatch + 1;
    if (!doFrac) {
        // note: for FRAC cases, the quadInfo->uMaxSegs used at this point can be
        // quite different than the full MAX calculated in evalCommonSetup()
        // Therefore, the following is only generally true for INT cases.
        nvAssert((stitch_nSwatchU - nSwatchU) <= 1);
    }
    nvAssert((*ppGuardSetU0[0])[0]->order == (*ppGuardSetU1[0])[0]->order);
    nvAssert((*ppGuardSetV0[0])[0]->order == (*ppGuardSetV1[0])[0]->order);
    // note: ppStitchUInner and ppStitchVInner may not be formed yet

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
    for (onCol = 0;  onCol < nSwatchU;  onCol++) {          // ********** for each col in the patch
        int rem, rightSV1;

        CopyMatrixSet(info, info->evalEnables, pBB1, pBB0);     // horz step BB0 to BB1
        OffsetFDMatrixSet(info, info->evalEnables, pBB1, 1, 0); // ...

        // cvt BB0 to BS0 (in place)
        pBS0 = pBB0;
        RetessMatrixSetU(info, 0, info->evalEnables, pBS0);

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
            rightSV1 = 0;
            ExtractVCurveSet(info, info->evalEnables, &BV1, pBB1);  // extract BV1 from BB1
        } else {
            rightSV1 = 1;
        }

        if (info->evalEnables & EV_GUARD_BIT_VERTEX) {
            pUL[ATTR_V] = (*pBS0)[ATTR_V]->data[0][0];
        }
        if (info->evalEnables & EV_GUARD_BIT_NORMAL) {
            pUL[ATTR_N] = (*pBS0)[ATTR_N]->data[0][0];
        }

        // tag:roww
        for (onRow = 0;  onRow < nSwatchV;  onRow++) {      // ********** for each row in the col
            //ffint needUL = 0;
            int needLL = 0;

            info->swatchFlags = NV_PATCH_SWATCH;
            info->setGridCorner = 0;
            enablesSU0 = enablesSU1 = enablesSV0 = enablesSV1 = info->evalEnables;

// ... col
            if (!onCol) {
                info->swatchFlags |= NV_PATCH_SWATCH_FIRST_COL;
                quadInfo->stitchUBegin = quadInfoOrig->stitchUBegin;
                if (!onRow) {
                    info->setGridCorner |= NV_PATCH_SET_GRID_CORNER00;
                }
                pSV0 = ppGuardSetVBegin[onRow];
                pSV0Next = ppGuardSetVBegin[onRow+1];

            } else {
                quadInfo->stitchUBegin = 0;
                pSV0 = &SV0;
                pSV0Next = NULL;
            }

            if (onCol == (nSwatchU-1)) {
                info->swatchFlags |= NV_PATCH_SWATCH_LAST_COL;
                quadInfo->stitchUEnd = quadInfoOrig->stitchUEnd;
                if (!onRow) {
                    info->setGridCorner |= NV_PATCH_SET_GRID_CORNER01;
                }
                pSV1 = ppGuardSetVEnd[onRow];
                pSV1Next = ppGuardSetVEnd[onRow+1];

            } else {
                quadInfo->stitchUEnd = 0;
                pSV1 = &SV1;
                pSV1Next = NULL;
            }

            for (index = 0; index < info->maxAttr; index++) {
                if (!(info->evalEnables & (1 << index))) {
                    continue;
                }
                //ffif (!EV_GUARD_ATTR(index)  &&  !doFrac) {
                    //ffcontinue;
                //ff}
                //ff(*quadInfo->pSwatchVEnd)[index] = (*pSV1)[index];
            }
// ... row
            if (!onRow) {
                info->swatchFlags |= NV_PATCH_SWATCH_FIRST_ROW;
                quadInfo->stitchVBegin = quadInfoOrig->stitchVBegin;
                pSU0 = ppGuardSetUBegin[onCol];

            } else {
                quadInfo->stitchVBegin = 0;
                pSU0 = NULL;
            }

            if (onRow == (nSwatchV-1)) {
                info->swatchFlags |= NV_PATCH_SWATCH_LAST_ROW;
                quadInfo->stitchVEnd = quadInfoOrig->stitchVEnd;
                quadInfo->pSwatchUEnd = pSU1 = ppGuardSetUEnd[onCol];
                pSU1Next = ppGuardSetUEnd[onCol+1];

                if (!onCol) {
                    info->setGridCorner |= NV_PATCH_SET_GRID_CORNER10;
                }
                if (onCol==(nSwatchU-1)) {
                    info->setGridCorner |= NV_PATCH_SET_GRID_CORNER11;
                }
            } else {
                quadInfo->stitchVEnd = 0;
                //ffquadInfo->pSwatchUEnd = &lguardSetUEnd;
                pSU1 = NULL;
                pSU1Next = NULL;
            }
// ... end row
            quadInfo->pSwatchUBegin = pSU0;
            quadInfo->pSwatchUEnd = pSU1;
            quadInfo->pSwatchVBegin = pSV0;
            quadInfo->pSwatchVEnd = pSV1;

            // ***** compute segs for this swatch, including any ending stitches
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

            // ***** prepare matrices and curves for this swatch *****
            CopyMatrixSet(info, info->evalEnables, &SS0, pBS0);
            RetessMatrixSetV(info, 0, info->evalEnables, &SS0);
            //ffEV_DRAW( evalDrawMatrix(info, SS0[0], 0, 0) );

            BEGIN_USING_TEMP_MATRIX(info->tempMatrix);
            if (rightSV1 && !onRow /*&& quadInfo->stitchUEnd*/) {
                // prepare first BV1
                for (index = 0; index < info->maxAttr; index++) {
                    if (info->evalEnables & (1 << index)) {
                        CopyMatrix(info, &info->tempMatrix, (*pBS0)[index]);
                        OffsetFDMatrix(info, &info->tempMatrix, quadInfo->uMaxSegs, 0);
                        ExtractUCurve(info, BV1[index], &info->tempMatrix);
                    }
                }
            }
            END_USING_TEMP_MATRIX(info->tempMatrix);
            OffsetFDMatrixSet(info, info->evalEnables, pBS0, 0, 1); // up to next row

            if (!rightSV1) {
                RetessCurveSetInOut(info, 0, info->evalEnables, &BV1, pSV1);
                OffsetCurveSet(info, info->evalEnables & EV_GUARD_BITS, &BV1, 1);
            }

            // ***** prepare guard information
            // prepare U0 info
            BEGIN_USING_TEMP_MATRIX(info->tempMatrix);
            for (index = 0; index < info->maxAttr; index++) {
                if (!(info->evalEnables & (1 << index))) {
                    continue;
                }
                if (!onRow && quadInfo->stitchVBegin) {
                    CopyMatrix(info, &info->tempMatrix, SS0[index]);
                    OffsetFDMatrix(info, &info->tempMatrix, 0, 1);
                    ExtractUCurve(info, (*pSU0)[index], &info->tempMatrix);
                }
                if ((info->swatchFlags & NV_PATCH_SWATCH_LAST_ROW) && quadInfo->stitchVEnd) {
                    //ffCopyMatrix(info, &info->tempMatrix, SS0[index]);
                    //ffOffsetFDMatrix(info, &info->tempMatrix, 0, quadInfo->vMaxSegs - 1);
                    //ffExtractUCurve(info, (*pSU1)[index], &info->tempMatrix);
                }
                if (!onCol && quadInfo->stitchUBegin) {
                    CopyMatrix(info, &info->tempMatrix, SS0[index]);
                    OffsetFDMatrix(info, &info->tempMatrix, 1, 0);
                    ExtractVCurve(info, (*pSV0)[index], &info->tempMatrix);
                }
                if ((info->swatchFlags & NV_PATCH_SWATCH_LAST_COL) && !onRow) {
                    CopyMatrix(info, &info->tempMatrix, SS0[index]);
                    OffsetFDMatrix(info, &info->tempMatrix, quadInfo->uMaxSegs - quadInfo->stitchUEnd, 0);
                    ExtractVCurve(info, (*pSV1)[index], &info->tempMatrix);
                }
            }
            END_USING_TEMP_MATRIX(info->tempMatrix);

            // prepare V0 info
            if (needBV0) {
                int eset = info->evalEnables;// & ~EV_GUARD_BITS;

                // BV0-->SV0 will be saved away for VInner transition curve set
                RetessCurveSetInOut(info, 0, eset, &BV0, pSV0);
                OffsetCurveSet(info, eset, &BV0, 1);        // to next swatch row

            } else if (onCol) {
                // other columns extract SV0 from SS0
                ExtractVCurveSet(info, info->evalEnables & EV_GUARD_BITS, pSV0, &SS0);
            }

            // tag:ccorn prepare corner points
            for (index = 0; index < info->maxAttr; index++) {
                if (!(info->evalEnables & (1 << index))) {
                    continue;
                }

                if (quadInfo->stitchUBegin) {
                    nvAssert(needBV0);
                    if (info->swatchFlags & NV_PATCH_SWATCH_LAST_ROW) {
                        // on the last row, always get from UEnd curve
                        AddPoint3(UL[index], (*pSU1)[index]->coeffs[0], (*pSU1)[index]->coeffs[1]);
                        pUL[index] = UL[index];
                    } else {
                        pUL[index] = (BV0)[index]->coeffs[0];
                    }
                    if (!(info->swatchFlags & NV_PATCH_SWATCH_FIRST_ROW)) {
                        pLL[index] = (*pSV0)[index]->coeffs[0];
                    } else {
                        // bot row
                        AddPoint3(LL[index], (*pSU0)[index]->coeffs[0], (*pSU0)[index]->coeffs[1]);
                        pLL[index] = LL[index];
                    }
                } else {
                    if (quadInfo->stitchVBegin) {
                        AddPoint3(LL[index], (*pSV0)[index]->coeffs[0], (*pSV0)[index]->coeffs[1]);
                        pLL[index] = LL[index];
                    } else {
                        pLL[index] = (*pSV0)[index]->coeffs[0];
                    }
                    if (info->swatchFlags & NV_PATCH_SWATCH_FIRST_COL) {
                        if (info->swatchFlags & NV_PATCH_SWATCH_LAST_ROW) {
                            pUL[index] = (*pSU1)[index]->coeffs[0];
                        } else {
                            pUL[index] = (*pSV0Next)[index]->coeffs[0];
                        }
                    } else {
                        if (info->swatchFlags & NV_PATCH_SWATCH_LAST_ROW) {
                            pUL[index] = (*pSU1)[index]->coeffs[0];
                        } else {
                            pUL[index] = (*pBS0)[index]->data[0][0];
                        }
                    }
                }

                if (quadInfo->stitchVBegin) {
                    AddPoint3(LR[index], (*pSV1)[index]->coeffs[0], (*pSV1)[index]->coeffs[1]);
                    pLR[index] = LR[index];
                } else {
                    pLR[index] = (*pSV1)[index]->coeffs[0];
                }

                if (info->swatchFlags & NV_PATCH_SWATCH_LAST_ROW) {
                    if (info->swatchFlags & NV_PATCH_SWATCH_LAST_COL) {
                        // step cur U1 to get there
                        FillIntCornerUR(info, quadInfo, quadInfoOrig, index, UR[index]);
                        pUR[index] = UR[index];
                        //ffCopyCurve(info, &info->tempCurve, (*pSU1)[index]);
                        //ffOffsetCurve(info, &info->tempCurve, quadInfo->uMaxSegs - quadInfo->stitchVEnd);
                        //ffCopyPoint(UR, info->tempCurve.coeffs[0]);
                    } else {
                        pUR[index] = (*pSU1Next)[index]->coeffs[0];
                    }
                } else {
                    if (info->swatchFlags & NV_PATCH_SWATCH_LAST_COL) {
                        pUR[index] = (*pSV1Next)[index]->coeffs[0];
                    } else {
                        pUR[index] = (BV1)[index]->coeffs[0];
                    }
                }
                //ffCopyCurve(info, &info->tempCurve, (*pSV1)[index]);
                //ffOffsetCurve(info, &info->tempCurve, quadInfo->vMaxSegs - quadInfo->stitchVEnd);
                //ffCopyPoint(UR, info->tempCurve.coeffs[0]);
                //ffpUR[index] = UR;
                if (quadInfo->stitchVEnd) {
                } else {
                }

                info->pSwatchCorner[index][0][0] = pLL[index];
                info->pSwatchCorner[index][0][1] = pLR[index];
                info->pSwatchCorner[index][1][0] = pUL[index];
                info->pSwatchCorner[index][1][1] = pUR[index];

                if (info->setGridCorner & NV_PATCH_SET_GRID_CORNER00) {
                    CopyPoint(info->gridCorner[index][1-quadInfo->vMaxDir][ 1-quadInfo->uMaxDir], pLL[index]);
                }
                if (info->setGridCorner & NV_PATCH_SET_GRID_CORNER01) {
                    CopyPoint(info->gridCorner[index][1-quadInfo->vMaxDir][   quadInfo->uMaxDir], pLR[index]);
                }

                if (info->setGridCorner & NV_PATCH_SET_GRID_CORNER10) {
                    CopyPoint(info->gridCorner[index][  quadInfo->vMaxDir][ 1-quadInfo->uMaxDir], pUL[index]);
                }
                if (info->setGridCorner & NV_PATCH_SET_GRID_CORNER11) {
                    CopyPoint(info->gridCorner[index][  quadInfo->vMaxDir][   quadInfo->uMaxDir], pUR[index]);
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
                if (info->evalEnables & (1 << ATTR_N)) {
                    info->swatchFlags |= NV_PATCH_HAS_NORMAL;
                };

                if (usingHW) {
                    if ((info->swatchFlags & NV_PATCH_SWATCH_FIRST_ROW) && (info->swatchFlags & NV_PATCH_SWATCH_FIRST_COL)) {
                        ComputeKelvinHOSControlWords(info, quadInfoOrig, info->swatchFlags);
                    }
                }

                retVal |= DrawIntPatchGrid(info, quadInfo, &SS0[0]);
            }
        }

        if (needBV0) {
            // complete generation of any inner curves
            for (onRow = nSwatchV;  onRow < stitch_nSwatchV;  onRow++) {
                RetessCurveSetInOut(info, 0, info->evalEnables, &BV0, &SV0);
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
            if (!usingHW) {
                lEnables = info->evalEnables;
            } else {
                lEnables = info->evalEnables & ~EV_GUARD_BITS;
            }
        }
        for (onCol = nSwatchU;  onCol < stitch_nSwatchU;  onCol++) {
            //ffCopyMatrixSet(info, lEnables, pBB1, pBB0);     // horz step BB0 to BB1
            //ffOffsetFDMatrixSet(info, lEnables, pBB1, 1, 0); // ...

            // cvt BB0 to BS0 (in place)
            pBS0 = pBB0;
            RetessMatrixSetU(info, 0, lEnables, pBS0);

            // prepare matrices for swatch row 0
            CopyMatrixSet(info, lEnables, &SS0, pBS0);
            RetessMatrixSetV(info, 0, lEnables, &SS0);

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

    nvAssert((*ppGuardSetU0[0])[0]->order == (*ppGuardSetU1[0])[0]->order);
    nvAssert((*ppGuardSetV0[0])[0]->order == (*ppGuardSetV1[0])[0]->order);
    if (ppStitchUInner) {
        nvAssert((*ppGuardSetU0[0])[0]->order == (*ppStitchUInner[0])[0]->order);
    }
    if (ppStitchVInner) {
        nvAssert((*ppGuardSetV0[0])[0]->order == (*ppStitchVInner[0])[0]->order);
    }
    DrawAllPatchTransitions(info, quadInfo,
                                    ppGuardSetU0, ppGuardSetU1, ppStitchUInner, ppStitchUInnerFrac,
                                    ppGuardSetV0, ppGuardSetV1, ppStitchVInner, ppStitchVInnerFrac);

    if (usingHW && (info->swatchFlags & NV_PATCH_SWATCH_LAST_ROW) && (info->swatchFlags & NV_PATCH_SWATCH_LAST_COL)) {
        HOS_CREATE_AND_ASSIGN_PUSHBUF_LOCALS();

        HOS_NOTE("End Patch");
        HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_END_PATCH)));
        HOS_PUSH(1, 0x0);
        HOS_PUSH_ADJUST(2);
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
    int usingHW = info->usingHW;

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

    NV_PATCH_EVAL_OUTPUT *pOut1, *pOut2;
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

    if (!usingHW) {
        // Reverse the front face direction
        info->backend->setFrontFace(info, 1 ^ info->reverse);

    } else {
        if (!doFrac) {
            info->reverse = info->reverse ^ (quadInfo->uMaxDir ^ quadInfo->vMaxDir);
        }
        info->backend->setFrontFace(info, 1 ^ info->reverse);
    }

    NV_TRACE_CODE(nUniqueVertices = 0);
    nBytes = 0;
    if (!usingHW) {
        nBytes += 2 * sizeof(NV_PATCH_EVAL_OUTPUT);
        nBytes += info->nAttr * 2*bytesGuardCurve;
    }
    if (nBytes) {
        EV_ALLOC_INIT_WITH_CACHE(info->context, nBytes, drawIntPatchEnd, info->pCache[EV_CACHE_DRAW_SW]);
    }

    if (!usingHW) {
        // current output points. pOut1 for outer, pOut2 for inner
        EV_ALLOC(pOut1, info->context, sizeof(NV_PATCH_EVAL_OUTPUT));
        EV_ALLOC(pOut2, info->context, sizeof(NV_PATCH_EVAL_OUTPUT));
    }

    for (index = 0; index < info->maxAttr; index++) {
        if (!(info->evalEnables & (1<<index))) {
            continue;
        }

        if (!usingHW) {
            EV_ALLOC(curves1[index], info->context, bytesGuardCurve);
            EV_ALLOC(curves2[index], info->context, bytesGuardCurve);
            curves1[index]->order = info->maps[index].uorder;
            curves2[index]->order = info->maps[index].uorder;
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
    quadInfo->pOut1 = pOut1;
    quadInfo->pOut2 = pOut2;

    info->setGridCorner = NV_PATCH_SET_GRID_CORNER_ALL;

    if (info->swatchFlags & NV_PATCH_SWATCH) {
        retVal |= DrawIntPatchSwaths(info, quadInfo, matrices);

    } else {
        // set up ptrs for writing U curves from matrix
        quadInfo->pSwatchUBegin = ppGuardSetUBegin[0];
        quadInfo->pSwatchUEnd = ppGuardSetUEnd[0];
        quadInfo->pSwatchVBegin = ppGuardSetVBegin[0];
        quadInfo->pSwatchVEnd = ppGuardSetVEnd[0];

        evalPrepareInnerCurves(info, 0, matrices);      // prepare the UInner and VInner curves

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

        if (info->evalEnables & (1 << ATTR_N)) {
            info->swatchFlags |= NV_PATCH_HAS_NORMAL;
        };

        if (usingHW) {
            //calculate and send the Patch control words
            ComputeKelvinHOSControlWords(info, quadInfo, info->swatchFlags);
        }

        quadInfo->uMaxSegs -= doFrac;
        quadInfo->vMaxSegs -= doFrac;
        retVal |= DrawIntPatchGrid(info, quadInfo, matrices);
        quadInfo->uMaxSegs += doFrac;
        quadInfo->vMaxSegs += doFrac;

        DrawAllPatchTransitions(info, quadInfo,
                            ppGuardSetU0, ppGuardSetU1, ppStitchUInner, info->ppGuardSetUInnerFrac,
                            ppGuardSetV0, ppGuardSetV1, ppStitchVInner, info->ppGuardSetVInnerFrac);

        if (usingHW) {
            //end patch
            HOS_NOTE("End Patch");
            HOS_PUSH(0, ((0x1 << 18) | (HOS_KELVIN_CHANNEL << 13) | (NV097_SET_END_PATCH)));
            HOS_PUSH(1, 0x0);
            HOS_PUSH_ADJUST(2);
        }
    }

#if defined(ENABLE_DEBUG_SWATH)
    if (!usingHW && nvDebugLevel >= 3 && (evalTrace & 2)) {
        // when all done with regular grid, draw green X through regular grid corners
        float *pGridLL = info->gridCorner[ATTR_V][1-quadInfo->vMaxDir][ 1-quadInfo->uMaxDir];
        float *pGridLR = info->gridCorner[ATTR_V][1-quadInfo->vMaxDir][   quadInfo->uMaxDir];
        float *pGridUL = info->gridCorner[ATTR_V][  quadInfo->vMaxDir][ 1-quadInfo->uMaxDir];
        float *pGridUR = info->gridCorner[ATTR_V][  quadInfo->vMaxDir][   quadInfo->uMaxDir];

        glDisable(GL_TEXTURE_2D);
        glPushMatrix();

        // and green X through gridCorner points
        glTranslatef(0, 0, .2);
        glColor3f(0, 1, 0);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0xFF00);
        glBegin(GL_LINE_STRIP);
            glVertex3fv(pGridLL); glVertex3fv(pGridLR);
            glVertex3fv(pGridUL); glVertex3fv(pGridUR);
            glVertex3fv(pGridLL);
        glEnd();
        glDisable(GL_LINE_STIPPLE);

        glPopMatrix();
        glColor3f(1, 1, 0);
        if (info->evalEnables & (1 << NV_PATCH_ATTRIB_TEXCOORD0)) {
            glEnable(GL_TEXTURE_2D);
        }
    }
#endif


drawIntPatchEnd:
    if (!usingHW) {
        EV_FREE(pOut1, info->context);
        EV_FREE(pOut2, info->context);
    }

    for (index = 0; index < info->maxAttr; index++) {
        if (info->evalEnables & (1 << index)) {
            if (!usingHW) {
                EV_FREE(curves1[index], info->context);
                EV_FREE(curves2[index], info->context);
            }
        }
    }
    NV_TRACE_CODE( if (enableUniqueCheck && !usingHW  &&  (nvDebugLevel >= 2)) {
        int A = quadInfo->nu0;
        int B = quadInfo->nv0;
        int C = quadInfo->nu1;
        int D = quadInfo->nv1;
        int maxac = MAX(A,C);
        int maxbd = MAX(B,D);
        int minac = MIN(A,C);
        int minbd = MIN(B,D);
        int expect = MAX(A,C) * MAX(B,D) + MIN(A,C) + MIN(B,D) + 1;
        int expectedTris = (maxac-1)*(maxbd-1)*2+minbd+maxbd-1+minac+maxac-1;

        if (doFrac) {
            expect = quadInfo->uMaxSegs * quadInfo->vMaxSegs + quadInfo->uMinSegs + quadInfo->vMinSegs + 1;
            expectedTris = (quadInfo->uMaxSegs-1)*(quadInfo->vMaxSegs-1)
                                +quadInfo->uMinSegs+quadInfo->uMinSegs-1
                                +quadInfo->vMinSegs+quadInfo->vMinSegs-1;
        }
        nvAssert((nvDebugLevel & 1) || expect == nUniqueVertices);
        TPRINTF(("nUniqueVertices=%d    expectV=%d expectTri=%d\n",
                 nUniqueVertices, expect, expectedTris));
        if (expect != nUniqueVertices) {
            drawUniqueVertices();
        }
    } );
    NV_TRACE_COND(TR_EVAL, 20,
        TPRINTF(("nu0=%d nu1=%d nv0=%d nv1=%d maxU=%d maxV=%d minU=%d minV=%d\n",
                quadInfo->nu0, quadInfo->nu1, quadInfo->nv0, quadInfo->nv1,
                quadInfo->uMaxSegs, quadInfo->vMaxSegs, quadInfo->uMinSegs, quadInfo->vMinSegs));
        TPRINTF(("dir BTLR=[%d %d %d %d]  uMaxDir=%d vMaxDir=%d\n",
                quadInfo->u0Dir, quadInfo->u1Dir, quadInfo->v0Dir, quadInfo->v1Dir, quadInfo->uMaxDir, quadInfo->vMaxDir));
        TPRINTF(("  stitch BTLR=[%d %d %d %d]  stitch UBE=[%d %d] VBE=[%d %d]\n",
                quadInfo->stitchBottom, quadInfo->stitchTop, quadInfo->stitchLeft, quadInfo->stitchRight,
                quadInfo->stitchUBegin, quadInfo->stitchUEnd, quadInfo->stitchVBegin, quadInfo->stitchVEnd ));
        TPRINTF(("force rev bits=%d %d %d %d  info->flags=x%X\n",
                (nvDebugOptions & TRO_TEMP1) != 0,
                (nvDebugOptions & TRO_TEMP2) != 0,
                (nvDebugOptions & TRO_TEMP3) != 0,
                (nvDebugOptions & TRO_TEMP4) != 0,
                info->flags));
        if (info->flipT && info->flipU && info->flipV) {
            TPRINTF(("\n\n***** transpose/flip alert %d <--> %d\n\n", quadInfo->nu1, quadInfo->nv1));
            TPRINTF(("***** flipU=%d flipV=%d\n", info->flipU, info->flipV));
        }
    );
    return retVal;
}

// Determine if a patch is rational (i.e. w != 1)
static int IsPatchRational(NV_PATCH_INFO *info, int attrib)
{
    NV_PATCH_MAP_INFO *map = &info->maps[attrib];
    unsigned int i, j;

/*    switch (type) {
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

static float BinomialCoefficient(int n, int r)
{
    float x;
    if (n<=10 && r <= 10) {
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

static float TrinomialCoefficient(int n, int r, float s)
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

static void ComputeNormalPatch(NV_PATCH_INFO *info, float *normalPatch,
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

const static struct {
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

    for (attr = 0; attr < info->maxAttr; attr++) {
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
//int bFirstTimeInit = 0x1;              //global value to indicate if we've built dynamic inner loops yet or not.
float fracFudge = .01f;
unsigned int nvEvalPatchSetup(NV_PATCH_INFO *info)
{
    int ii;
    unsigned int retVal = NV_PATCH_EVAL_OK;
    float *normalPatch = NULL;
    float *UVPatch = NULL;
    float segmin = 1.f;
    EV_ALLOC_DECL();
    static int onceOnlyDone = 0;

    EV_DRAW(onColor = 0);
    if (g_FDGlobalData.bFirstTimeInit) {
        nvPatchInitFD(info);
        g_FDGlobalData.bFirstTimeInit = 0;
    }    

    info->reverse = 0;
    info->usingHW = !g_FDGlobalData.bForceSoftware & (info->backendType == NV_PATCH_BACKEND_KELVIN ? 1 : 0); // identifies places that probably need code for HW version

    NV_TRACE_CODE(
        if (!info->usingHW) {
            nvControlOptions |= NCO_DID_SW;
        }
    );

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
    NV_TRACE_CODE(nUniqueVertices = 0);

    // note: must use same value for HW & SW to use the same big<-->small matrices
    info->maxSwatch = NV_PATCH_KELVIN_SWATCH_SIZE;

#if defined(IS_OPENGL)
 #if defined(FREDS_DEBUG_CODE)
    NV_TRACE_CODE(if (nvDebugLevel & 1))
    {
        info->maxSwatch = 4;
    }
    //ffnvDebugMask |= TR_PUSHBUFFER;
    //ffnvDebugOptions &= ~(TRO_TEMP1 | TRO_TEMP2 | TRO_TEMP3 | TRO_TEMP4);
 #endif

    if (info->usingHW) {
        glBegin(GL_POINTS);     // perform validation for HW
        glEnd();
        info->backendType = NV_PATCH_BACKEND_KELVIN;
    }
#endif
    if (onceOnlyDone != info->maxSwatch) {
        evalFillRetessMatrices(info);
        onceOnlyDone = info->maxSwatch;     // note: setting after fill makes it thread safe
    }

//ff#ifdef DCR_SEMAPHORE
    if (info->usingHW) {info->backend->beginPatch(info);}
//ff#endif

    if (info->flags & NV_PATCH_FLAG_AUTO_NORMAL) {
        NV_PATCH_MAP_INFO *map;
        int uorder, vorder;
        int normalUOrder, normalVOrder;

        // init for ComputeNormalPatch
        map = &info->maps[info->srcNormal];
        uorder = map->uorder;
        vorder = map->vorder;
        map->ufStride = map->stride/sizeof(float);
        map->vfStride = map->ufStride * map->pitch;

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
        info->maps[info->dstNormal].stride = 4*sizeof(float);
        info->maps[info->dstNormal].pitch  = normalUOrder;
    }

    if (info->flags & NV_PATCH_FLAG_AUTO_UV) {
        int order = 2;
        int bytes = 0;
        for (ii=0; ii < 8; ii++) {
            if (info->dstUV[ii] != 0xFFFFffff) {
                if (info->maps[info->dstUV[ii]].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
                    if (bytes == 0) {
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
                info->maps[info->dstUV[ii]].stride = 4*sizeof(float);
                info->maps[info->dstUV[ii]].pitch = order;
            }
        }
    }

    // ensure some flags are initialized
    info->flags &= ~(NV_PATCH_FLAG_SOME_DIRTY_TESS | NV_PATCH_FLAG_TRANSPOSE
                        | NV_PATCH_FLAG_FLIPPED_U | NV_PATCH_FLAG_FLIPPED_V);
    for (ii = 0;  ii < NV_PATCH_NUMBER_OF_ATTRIBS;  ii++) {
        if (info->evalEnables & (1 << ii)) {
            NV_PATCH_MAP_INFO *map = &info->maps[ii];

            if (!(map->dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS)) {
                if (map->dirtyFlags & NV_PATCH_DIRTY_TESSELLATION) {
                    info->flags |= NV_PATCH_FLAG_SOME_DIRTY_TESS;
                }
            }

            map->ufStride = map->stride/sizeof(float);
            map->vfStride = map->ufStride * map->pitch;

            if (map->uorder == 1) {
                map->uorder = 2;
                map->ufStride = 0;
            }
            if (map->vorder == 1) {
                map->vorder = 2;
                map->vfStride = 0;
            }

            EV_DRAW( if (0  &&  !ii  &&  (nvDebugLevel >= 4)) {
                unsigned int row;
                unsigned int col;

                /* draw lines through all the control points */
                glColor3f(1, 0, 1);
                for (row = 0; row < map->vorder;  row++) {
                    glBegin(GL_LINE_STRIP);
                    for (col = 0;  col < map->uorder;  col++) {
                        glVertex3fv(pRawData(info, map, map->rawData, row, col));
                    }
                    glEnd();
                }
            } );

            // calc MAX values
            info->nAttr++;
            if (info->firstAttr == -1) {
                info->firstAttr = ii;
            }
            if (ii > info->maxAttr) {
                info->maxAttr = ii;
            }
            if (map->uorder > info->maxOrder) {
                info->maxOrder = map->uorder;
            }
            if (map->vorder > info->maxOrder) {
                info->maxOrder = map->vorder;
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
        __NV_FREE(info->context, info->normalPatch);
        info->normalPatch = NULL;
    }
    if (info->UVPatch && !info->pCache[EV_CACHE_UV_SETUP]) {
        __NV_FREE(info->context, info->UVPatch);
        info->UVPatch = NULL;
    }

    // indicate that all cache's are not in use
    for (ii = 0;  ii < MAX_EV_CACHE;  ii++) {
        if (info->pCache[ii]) {
            info->pCache[ii]->inUse = 0;
        }
    }

//ff#ifdef DCR_SEMAPHORE    
    if (info->usingHW) info->backend->endPatch(info);
//ff#endif

    info->backend->setFrontFace(info, 0);
#if defined(COMPILE_TRACE_LIBRARY) && defined(FREDS_DEBUG_CODE)
    nvDebugMask &= ~TR_PUSHBUFFER;
#endif


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
        NV_TRACE_COND(TR_EVAL, 20,
            TPRINTF(("\n\n***** transpose alert %g <--> %g\n\n", fnuMax, fnvMax));
        );
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

    if (!info->usingHW) {
        if (info->flipT) {
            info->reverse ^= 1;
        }

    } else {
        info->reverse ^= info->flipTUV;
    }

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
                assert(info->maps[index].uorder == info->maps[index].vorder);
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

                assert(info->maps[index].uorder == info->maps[index].vorder);
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

    if (!info->usingHW) {
        if (info->flipT) {
            info->reverse ^= 1;
        }

    } else {
        info->reverse ^= info->flipTUV;
    }

    quadInfo->uMaxSegs = MAX(quadInfo->nu0, quadInfo->nu1);
    quadInfo->vMaxSegs = MAX(quadInfo->nv0, quadInfo->nv1);
    quadInfo->uMinSegs = MIN(quadInfo->nu0, quadInfo->nu1);
    quadInfo->vMinSegs = MIN(quadInfo->nv0, quadInfo->nv1);

    return 1;
}

/*****************************************************************************/
// common setup code for patches
static void evalCommonSetup(NV_PATCH_INFO *info, float uMaxSegs, float  vMaxSegs)
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

    if (info->backendType == NV_PATCH_BACKEND_CELSIUS) {
        if (info->backend->InitBackend) info->backend->InitBackend(info);
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
            if (MAX(u0Segs,u1Segs) > info->maxSwatch || MAX(v0Segs,v1Segs) > info->maxSwatch) {
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
                        ComputeFDCurveFlip(info, uorder, du0, &quadInfo->guardQF[i]->guardU00,
                            pRawData(info, map, rawData, 0, 0), info->maps[i].ufStride * sizeof(float), 1, info->flipU);
                        ComputeFDCurveFlip(info, uorder, du0, &quadInfo->guardQF[i]->guardU01,
                            pRawData(info, map, rawData, 0, 0), info->maps[i].ufStride * sizeof(float), 0, info->flipU);

                        // u10, u11
                        quadInfo->guardQF[i]->guardU10.order = uorder;
                        quadInfo->guardQF[i]->guardU11.order = uorder;
                        ComputeFDCurveFlip(info, uorder, du1, &quadInfo->guardQF[i]->guardU10,
                            pRawData(info, map, rawData, vorder-1, 0), info->maps[i].ufStride * sizeof(float), 1, info->flipU);
                        ComputeFDCurveFlip(info, uorder, du1, &quadInfo->guardQF[i]->guardU11,
                            pRawData(info, map, rawData, vorder-1, 0), info->maps[i].ufStride * sizeof(float), 0, info->flipU);

                        // v00, v01
                        quadInfo->guardQF[i]->guardV00.order = vorder;
                        quadInfo->guardQF[i]->guardV01.order = vorder;
                        ComputeFDCurveFlip(info, vorder, dv0, &quadInfo->guardQF[i]->guardV00,
                            pRawData(info, map, rawData, 0, 0), info->maps[i].vfStride * sizeof(float), 1, info->flipV);
                        ComputeFDCurveFlip(info, vorder, dv0, &quadInfo->guardQF[i]->guardV01,
                            pRawData(info, map, rawData, 0, 0), info->maps[i].vfStride * sizeof(float), 0, info->flipV);

                        // v10, v11
                        quadInfo->guardQF[i]->guardV10.order = vorder;
                        quadInfo->guardQF[i]->guardV11.order = vorder;
                        ComputeFDCurveFlip(info, vorder, dv1, &quadInfo->guardQF[i]->guardV10,
                            pRawData(info, map, rawData, 0, uorder-1), info->maps[i].vfStride * sizeof(float), 1, info->flipV);
                        ComputeFDCurveFlip(info, vorder, dv1, &quadInfo->guardQF[i]->guardV11,
                            pRawData(info, map, rawData, 0, uorder-1), info->maps[i].vfStride * sizeof(float), 0, info->flipV);

                        // uCenter0, uCenter1
                        quadInfo->guardQF[i]->guardUCenter0.order = uorder;
                        quadInfo->guardQF[i]->guardUCenter1.order = uorder;
                        for (j = 0; j < uorder; j++) {
                            ComputeCurvePoint(vorder, &curvePoints[j][0], pRawData(info, map, rawData, 0, j), vfStride * sizeof(float), .5);
                        }
                        ComputeFDCurveFlip(info, uorder, duMid, &quadInfo->guardQF[i]->guardUCenter0,
                            &curvePoints[0][0], 4 * sizeof(float), 1, 0);
                        ComputeFDCurveFlip(info, uorder, duMid, &quadInfo->guardQF[i]->guardUCenter1,
                            &curvePoints[0][0], 4 * sizeof(float), 0, 0);

                        // vCenter0, vCenter1
                        quadInfo->guardQF[i]->guardVCenter0.order = vorder;
                        quadInfo->guardQF[i]->guardVCenter1.order = vorder;
                        for (j = 0; j < vorder; j++) {
                            ComputeCurvePoint(uorder, &curvePoints[j][0], pRawData(info, map, rawData, j, 0),
                                                ufStride*sizeof(float),.5);
                        }
                        ComputeFDCurveFlip(info, vorder, dvMid, &quadInfo->guardQF[i]->guardVCenter0,
                            &curvePoints[0][0], 4 * sizeof(float), 1, 0);
                        ComputeFDCurveFlip(info, vorder, dvMid, &quadInfo->guardQF[i]->guardVCenter1,
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

                        RetessellateFDCurve(info, du0Old, du0, &quadInfo->guardQF[i]->guardU00, 0);
                        RetessellateFDCurve(info, du0Old, du0, &quadInfo->guardQF[i]->guardU01, 0);
                        RetessellateFDCurve(info, du1Old, du1, &quadInfo->guardQF[i]->guardU10, 0);
                        RetessellateFDCurve(info, du1Old, du1, &quadInfo->guardQF[i]->guardU11, 0);
                        RetessellateFDCurve(info, dv0Old, dv0, &quadInfo->guardQF[i]->guardV00, 0);
                        RetessellateFDCurve(info, dv0Old, dv0, &quadInfo->guardQF[i]->guardV01, 0);
                        RetessellateFDCurve(info, dv1Old, dv1, &quadInfo->guardQF[i]->guardV10, 0);
                        RetessellateFDCurve(info, dv1Old, dv1, &quadInfo->guardQF[i]->guardV11, 0);

                        RetessellateFDCurve(info, duMidOld, duMid, &quadInfo->guardQF[i]->guardUCenter0, 0);
                        RetessellateFDCurve(info, duMidOld, duMid, &quadInfo->guardQF[i]->guardUCenter1, 0);
                        RetessellateFDCurve(info, dvMidOld, dvMid, &quadInfo->guardQF[i]->guardVCenter0, 0);
                        RetessellateFDCurve(info, dvMidOld, dvMid, &quadInfo->guardQF[i]->guardVCenter1, 0);

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
            NV_TRACE_CODE(
                if (skipQuads) {
                    goto nvEvalPatchEnd;
                }
                //ffevalTrace = 0;
            );

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
                ComputeEdgeDirections(info, 0, &info->maps[ATTR_V], &quadInfo->u0Dir, &quadInfo->u1Dir, &quadInfo->v0Dir, &quadInfo->v1Dir);
            }
            else if (info->maps[ATTR_V].dirtyFlags & NV_PATCH_DIRTY_TESSELLATION) {
                ComputeEdgeDirectionsFromCorners(info, 0, &quadInfo->u0Dir, &quadInfo->u1Dir, &quadInfo->v0Dir, &quadInfo->v1Dir);
            }

            if (quadInfo->uMaxSegs == 1) {
                // then reverse transitions in U do not matter
                quadInfo->u0Dir = quadInfo->u1Dir = 1;
            }
            if (quadInfo->vMaxSegs == 1) {
                // then reverse transitions in V do not matter
                quadInfo->v0Dir = quadInfo->v1Dir = 1;
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

            if (info->maps[ATTR_V].dirtyFlags & NV_PATCH_DIRTY_CONTROL_POINTS) {
                quadInfo->olduMaxDir = quadInfo->uMaxDir;
                quadInfo->oldvMaxDir = quadInfo->vMaxDir;
                quadInfo->oldu0Dir   = quadInfo->u0Dir;
                quadInfo->oldu1Dir   = quadInfo->u1Dir;
                quadInfo->oldv0Dir   = quadInfo->v0Dir;
                quadInfo->oldv1Dir   = quadInfo->v1Dir;
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
            if (nuMaxOld > info->maxSwatch || nvMaxOld > info->maxSwatch) {
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

            NV_TRACE_COND(TR_EVAL, 25,
                TPRINTF(("dir BTLR=[%d %d %d %d]  uMaxDir=%d vMaxDir=%d\n",
                        quadInfo->u0Dir, quadInfo->u1Dir, quadInfo->v0Dir, quadInfo->v1Dir, uMaxDir, vMaxDir));
                TPRINTF(("  stitch BTLR=[%d %d %d %d]  stitch UBE=[%d %d] VBE=[%d %d]\n",
                        stitchBottom, stitchTop, stitchLeft, stitchRight,
                        stitchUBegin, stitchUEnd, stitchVBegin, stitchVEnd ));
                TPRINTF(("force rev bits=%d %d %d %d\n",
                        (nvDebugOptions & TRO_TEMP1) != 0,
                        (nvDebugOptions & TRO_TEMP2) != 0,
                        (nvDebugOptions & TRO_TEMP3) != 0,
                        (nvDebugOptions & TRO_TEMP4) != 0));
            );

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
                        ComputeFDCurveFlip(info, uorder, du0, quadInfo->pU0[i],
                            pRawData(info, map, rawData, 0, 0),
                            info->maps[i].ufStride * sizeof(float), quadInfo->u0Dir, info->flipU);
                        ComputeFDCurveFlip(info, uorder, du1, quadInfo->pU1[i],
                            pRawData(info, map, rawData, vorder-1, 0),
                            info->maps[i].ufStride * sizeof(float), quadInfo->u1Dir, info->flipU);

                        // Compute V boundary curves
                        quadInfo->pV0[i]->order = vorder;
                        quadInfo->pV1[i]->order = vorder;
                        ComputeFDCurveFlip(info, vorder, dv0, quadInfo->pV0[i],
                            pRawData(info, map, rawData, 0, 0),
                            info->maps[i].vfStride * sizeof(float), quadInfo->v0Dir, info->flipV);
                        ComputeFDCurveFlip(info, vorder, dv1, quadInfo->pV1[i],
                            pRawData(info, map, rawData, 0, uorder-1),
                            info->maps[i].vfStride * sizeof(float), quadInfo->v1Dir, info->flipV);

                        info->maps[i].dirtyFlags = 0;

                    } else if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_TESSELLATION) {
                        // Recompute U boundary curves
                        if (quadInfo->oldu0Dir == quadInfo->u0Dir) RetessellateFDCurve(info, du0Old, du0, quadInfo->pU0[i], 0);
                        else                                       RetessellateFDCurve(info, du0Old, du0, quadInfo->pU0[i], 1);                        
                        if (quadInfo->oldu1Dir == quadInfo->u1Dir) RetessellateFDCurve(info, du1Old, du1, quadInfo->pU1[i], 0);
                        else                                       RetessellateFDCurve(info, du1Old, du1, quadInfo->pU1[i], 1);

                        // Recompute V boundary curves
                        if (quadInfo->oldv0Dir == quadInfo->v0Dir) RetessellateFDCurve(info, dv0Old, dv0, quadInfo->pV0[i], 0);
                        else                                       RetessellateFDCurve(info, dv0Old, dv0, quadInfo->pV0[i], 1);
                        if (quadInfo->oldv1Dir == quadInfo->v1Dir) RetessellateFDCurve(info, dv1Old, dv1, quadInfo->pV1[i], 0);
                        else                                       RetessellateFDCurve(info, dv1Old, dv1, quadInfo->pV1[i], 1);

                        info->maps[i].dirtyFlags &= ~NV_PATCH_DIRTY_TESSELLATION;
                    }
                }
            }
            
            //save off current diretions so we can assess if the traversal direction
            //changed on a subsequent render
            quadInfo->olduMaxDir = quadInfo->uMaxDir;
            quadInfo->oldvMaxDir = quadInfo->vMaxDir;               
            quadInfo->oldu0Dir   = quadInfo->u0Dir;
            quadInfo->oldu1Dir   = quadInfo->u1Dir;
            quadInfo->oldv0Dir   = quadInfo->v0Dir;
            quadInfo->oldv1Dir   = quadInfo->v1Dir;
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
                                ComputeFDCurveFlip(info, order, duvMid, &quadInfo->guardTF[i]->guardCenter[j],
                                    &m[j][i]->data[order-1][0][0],
                                    4 * sizeof(float), 1, 0);
                            } else {
                                ComputeFDCurveFlip(info, order, duvMid, &quadInfo->guardTF[i]->guardCenter[j],
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

                        ComputeFDCurveFlip(info, order, duv[0], &quadInfo->guardTF[i]->guard[0][0],
                            pRawData(info, map, rawData, 0, 0),
                            ufStride * sizeof(float), 1, info->flipU);
                        ComputeFDCurveFlip(info, order, duv[0], &quadInfo->guardTF[i]->guard[0][1],
                            pRawData(info, map, rawData, 0, 0),
                            ufStride * sizeof(float), 0, info->flipU);

                        // 10, 11
                        quadInfo->guardTF[i]->guard[1][0].order = order;
                        quadInfo->guardTF[i]->guard[1][1].order = order;
                        ComputeFDCurveFlip(info, order, duv[1], &quadInfo->guardTF[i]->guard[1][0],
                            pRawData(info, map, rawData, 0, order-1),
                            (vfStride-4) * sizeof(float), 1, info->flipU);
                        ComputeFDCurveFlip(info, order, duv[1], &quadInfo->guardTF[i]->guard[1][1],
                            pRawData(info, map, rawData, 0, order-1),
                            (vfStride-4) * sizeof(float), 0, info->flipU);

                        // 20, 21
                        quadInfo->guardTF[i]->guard[2][0].order = order;
                        quadInfo->guardTF[i]->guard[2][1].order = order;
                        ComputeFDCurveFlip(info, order, duv[2], &quadInfo->guardTF[i]->guard[2][0],
                            pRawData(info, map, rawData, 0, 0),
                            vfStride * sizeof(float), 0, info->flipU);
                        ComputeFDCurveFlip(info, order, duv[2], &quadInfo->guardTF[i]->guard[2][1],
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
                            //BUG BUG WE NEED TO ASSES IF CURVES CHANGED DIRECTION
                            RetessellateFDCurve(info, duvOld[j], duv[j], &quadInfo->guardTF[i]->guard[j][0], 0);
                            RetessellateFDCurve(info, duvOld[j], duv[j], &quadInfo->guardTF[i]->guard[j][1], 0);

                            RetessellateFDCurve(info, duvMidOld, duvMid, &quadInfo->guardTF[i]->guardCenter[j], 0);
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
                NV_TRACE_CODE(
                    if (skipQuads) {
                        break;
                    }
                    //ffevalTrace = 0;
                );
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
            }
            /*if (info->maps[ATTR_V].dirtyFlags & NV_PATCH_DIRTY_TESSELLATION) {
                ComputeEdgeDirectionsFromCorners(info, 1, &quadInfo->u0Dir, &quadInfo->u1Dir, &quadInfo->v0Dir, &quadInfo->v1Dir);
            }*/

            if (quadInfo->uMaxSegs == 1) {
                // then reverse transitions in U do not matter
                quadInfo->u0Dir = quadInfo->u1Dir = 1;
            }
            if (quadInfo->vMaxSegs == 1) {
                // then reverse transitions in V do not matter
                quadInfo->v0Dir = quadInfo->v1Dir = 1;
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
            if (nuMaxOld > info->maxSwatch || nvMaxOld > info->maxSwatch) {      // various common init for patches
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
                        ComputeFDCurveFlip(info, uorder, du0, quadInfo->pU0[i],
                            pRawData(info, map, rawData, 0, 0),
                            info->maps[i].ufStride * sizeof(float),quadInfo->u0Dir, info->flipU);
                        ComputeFDCurveFlip(info, uorder, du1, quadInfo->pU1[i],
                            pRawData(info, map, rawData, vorder-1, 0),
                            0 /* stride at top */, quadInfo->u1Dir, info->flipU);

                        // Compute V boundary curves
                        quadInfo->pV0[i]->order = vorder;
                        quadInfo->pV1[i]->order = vorder;
                        ComputeFDCurveFlip(info, vorder, dv0, quadInfo->pV0[i],
                            pRawData(info, map, rawData, 0, 0),
                            info->maps[i].vfStride * sizeof(float), quadInfo->v0Dir, info->flipV);
                        ComputeFDCurveFlip(info, vorder, dv1, quadInfo->pV1[i],
                            pRawData(info, map, rawData, 0, uorder-1),
                            (info->maps[i].vfStride-4) * sizeof(float), quadInfo->v1Dir, info->flipV);

                        info->maps[i].dirtyFlags = 0;

                    } else if (info->maps[i].dirtyFlags & NV_PATCH_DIRTY_TESSELLATION) {
                        //BUG BUG WE NEED TO ASSES IF CURVES CHANGED DIRECTION
                        // Compute U boundary curves
                        RetessellateFDCurve(info, du0Old, du0, quadInfo->pU0[i], 0);
                        RetessellateFDCurve(info, du1Old, du1, quadInfo->pU1[i], 0);
                        // Compute V boundary curves
                        RetessellateFDCurve(info, dv0Old, dv0, quadInfo->pV0[i], 0);
                        RetessellateFDCurve(info, dv1Old, dv1, quadInfo->pV1[i], 0);

                        info->maps[i].dirtyFlags &= ~NV_PATCH_DIRTY_TESSELLATION;
                    }
                }
            }

            retVal |= DrawIntPatch(info, quadInfo, quadInfo->m00);
        }
    }

NV_TRACE_CODE(
  nvEvalPatchEnd:
);

    if (info->backendType == NV_PATCH_BACKEND_CELSIUS) {
        if (info->backend->DestroyBackend) info->backend->DestroyBackend(info);
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
#if defined(IS_OPENGL)
            nvAssert(pCache->contextCache == context);
#endif
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
        __NV_FREE(pevMemory->context, pCache->pBaseCache);
    }

    // must do new alloc
    pevMemory->context = context;
    pevMemory->pBase = __NV_MALLOC(context, tBytes);
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

/*****************************************************************************/
#if !defined(IS_OPENGL)
#undef nvAssert
#endif

#if 1 && defined(IS_OPENGL)
const int attrHwToSw[NV_PATCH_NUMBER_OF_ATTRIBS] = {
        NV_PATCH_ATTRIB_POSITION,
        NV_PATCH_ATTRIB_VERTEX_WEIGHT,
        NV_PATCH_ATTRIB_NORMAL,
        NV_PATCH_ATTRIB_COLOR,
        NV_PATCH_ATTRIB_SECONDARY_COLOR,
        NV_PATCH_ATTRIB_FOG_COORD,
        NV_PATCH_ATTRIB_UNUSED0,
        NV_PATCH_ATTRIB_UNUSED1,
        NV_PATCH_ATTRIB_TEXCOORD7,
        NV_PATCH_ATTRIB_TEXCOORD0,
        NV_PATCH_ATTRIB_TEXCOORD1,
        NV_PATCH_ATTRIB_TEXCOORD2,
        NV_PATCH_ATTRIB_TEXCOORD3,
        NV_PATCH_ATTRIB_TEXCOORD4,
        NV_PATCH_ATTRIB_TEXCOORD5,
        NV_PATCH_ATTRIB_TEXCOORD6,
    };

const int attrSwToHw[NV_PATCH_NUMBER_OF_ATTRIBS] = {
        NV_PATCH_ATTRIB_POSITION,
        NV_PATCH_ATTRIB_VERTEX_WEIGHT,
        NV_PATCH_ATTRIB_NORMAL,
        NV_PATCH_ATTRIB_COLOR,
        NV_PATCH_ATTRIB_SECONDARY_COLOR,
        NV_PATCH_ATTRIB_FOG_COORD,
        NV_PATCH_ATTRIB_UNUSED0,
        NV_PATCH_ATTRIB_UNUSED1,
        NV_PATCH_ATTRIB_TEXCOORD1,
        NV_PATCH_ATTRIB_TEXCOORD2,
        NV_PATCH_ATTRIB_TEXCOORD3,
        NV_PATCH_ATTRIB_TEXCOORD4,
        NV_PATCH_ATTRIB_TEXCOORD5,
        NV_PATCH_ATTRIB_TEXCOORD6,
        NV_PATCH_ATTRIB_TEXCOORD7,
        NV_PATCH_ATTRIB_TEXCOORD0,
    };

#else
const int attrHwToSw[NV_PATCH_NUMBER_OF_ATTRIBS] = {
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15,
    };

const int attrSwToHw[NV_PATCH_NUMBER_OF_ATTRIBS] = {
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15,
    };
#endif

