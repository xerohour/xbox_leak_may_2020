/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    psraster.cpp

Author:

    Matt Bronder

Description:

    Pixel shader rasterizer.

*******************************************************************************/

#include "d3dlocus.h"
#ifndef UNDER_XBOX
#include "psd.h"
#endif
#include "psgen.h"
#include "psraster.h"

#define D3DCOLOR_HIWORD(c)  ((WORD)(((DWORD)(c) >> 16) & 0x0000FFFF)) 
#define D3DCOLOR_LOWORD(c)  ((WORD)((DWORD)(c) & 0x0000FFFF))
#define D3DCOLOR_HILO(h, l) ((D3DCOLOR)((((h) & 0x0000FFFF) << 16) | ((l) & 0x0000FFFF)))

typedef D3DCOLOR (* LERPPROC)(D3DCOLOR, D3DCOLOR, float);

//******************************************************************************
D3DCOLOR MultiplyColorScalar(D3DCOLOR c, float s) {

    int r, g, b, a;

    r = (int)((float)RGBA_GETRED(c) * s + 0.5f);
    g = (int)((float)RGBA_GETGREEN(c) * s + 0.5f);
    b = (int)((float)RGBA_GETBLUE(c) * s + 0.5f);
    a = (int)((float)RGBA_GETALPHA(c) * s + 0.5f);

    if (r < 0) r = 0;
    else if (r > 255) r = 255;
    if (g < 0) g = 0;
    else if (g > 255) g = 255;
    if (b < 0) b = 0;
    else if (b > 255) b = 255;
    if (a < 0) a = 0;
    else if (a > 255) a = 255;

    return D3DCOLOR_RGBA(r, g, b, a);
}

//******************************************************************************
inline void ClampZeroToOne(D3DXVECTOR4* pvReg) {

    if (pvReg->x > 1.0f) pvReg->x = 1.0f;
    else if (pvReg->x < 0.0f) pvReg->x = 0.0f;
    if (pvReg->y > 1.0f) pvReg->y = 1.0f;
    else if (pvReg->y < 0.0f) pvReg->y = 0.0f;
    if (pvReg->z > 1.0f) pvReg->z = 1.0f;
    else if (pvReg->z < 0.0f) pvReg->z = 0.0f;
    if (pvReg->w > 1.0f) pvReg->w = 1.0f;
    else if (pvReg->w < 0.0f) pvReg->w = 0.0f;
}

//******************************************************************************
inline void ClampNegOneToOne(D3DXVECTOR4* pvReg) {

    if (pvReg->x > 1.0f) pvReg->x = 1.0f;
    else if (pvReg->x < (-128.0f / 127.0f)) pvReg->x = -128.0f / 127.0f;
    if (pvReg->y > 1.0f) pvReg->y = 1.0f;
    else if (pvReg->y < (-128.0f / 127.0f)) pvReg->y = -128.0f / 127.0f;
    if (pvReg->z > 1.0f) pvReg->z = 1.0f;
    else if (pvReg->z < (-128.0f / 127.0f)) pvReg->z = -128.0f / 127.0f;
    if (pvReg->w > 1.0f) pvReg->w = 1.0f;
    else if (pvReg->w < (-128.0f / 127.0f)) pvReg->w = -128.0f / 127.0f;
}

//******************************************************************************
inline void ClampNegOneToOneH(D3DXVECTOR4* pvReg) {

    if (pvReg->x > 1.0f) pvReg->x = 1.0f;
    else if (pvReg->x < -1.0f) pvReg->x = -1.0f;
    if (pvReg->y > 1.0f) pvReg->y = 1.0f;
    else if (pvReg->y < -1.0f) pvReg->y = -1.0f;
    if (pvReg->z > 1.0f) pvReg->z = 1.0f;
    else if (pvReg->z < -1.0f) pvReg->z = -1.0f;
    if (pvReg->w > 1.0f) pvReg->w = 1.0f;
    else if (pvReg->w < -1.0f) pvReg->w = -1.0f;
}

//******************************************************************************
inline void ColorToVector(D3DXVECTOR4* pv, D3DCOLOR c) {

    pv->x = (float)RGBA_GETRED(c) / 255.0f;
    pv->y = (float)RGBA_GETGREEN(c) / 255.0f;
    pv->z = (float)RGBA_GETBLUE(c) / 255.0f;
    pv->w = (float)RGBA_GETALPHA(c) / 255.0f;
}

//******************************************************************************
inline void ColorToVectorZeroToOne(D3DXVECTOR4* pv, D3DCOLOR c) {

    pv->x = (float)RGBA_GETRED(c) / 255.0f;
    pv->y = (float)RGBA_GETGREEN(c) / 255.0f;
    pv->z = (float)RGBA_GETBLUE(c) / 255.0f;
    pv->w = (float)RGBA_GETALPHA(c) / 255.0f;
}

//******************************************************************************
inline D3DCOLOR VectorToColorZeroToOne(D3DXVECTOR4* pv) {

    D3DXVECTOR4 v;

    v = *pv;
    ClampZeroToOne(&v);

    return D3DCOLOR_RGBA((BYTE)(v.x * 255.0f + 0.5f), (BYTE)(v.y * 255.0f + 0.5f),
                         (BYTE)(v.z * 255.0f + 0.5f), (BYTE)(v.w * 255.0f + 0.5f));
}

//******************************************************************************
inline void ColorToVectorNegOneToOneMS(D3DXVECTOR4* pv, D3DCOLOR c) {

    pv->x = ((float)RGBA_GETRED(c) - 128.0f) / 127.0f;
    pv->y = ((float)RGBA_GETGREEN(c) - 128.0f) / 127.0f;
    pv->z = ((float)RGBA_GETBLUE(c) - 128.0f) / 127.0f;
    pv->w = ((float)RGBA_GETALPHA(c) - 128.0f) / 127.0f;
//    if (pv->x < (-128.0f / 127.0f)) pv->x = -128.0f / 127.0f;
//    if (pv->y < (-128.0f / 127.0f)) pv->y = -128.0f / 127.0f;
//    if (pv->z < (-128.0f / 127.0f)) pv->z = -128.0f / 127.0f;
//    if (pv->w < (-128.0f / 127.0f)) pv->w = -128.0f / 127.0f;
}

//******************************************************************************
inline D3DCOLOR VectorToColorNegOneToOneMS(D3DXVECTOR4* pv) {

    D3DXVECTOR4 v;
//    int r, g, b, a;

    v = *pv;
    ClampNegOneToOne(&v);

    return D3DCOLOR_RGBA((BYTE)(v.x * 127.0f + 128.0f), (BYTE)(v.y * 127.0f + 128.0f),
                         (BYTE)(v.z * 127.0f + 128.0f), (BYTE)(v.w * 127.0f + 128.0f));
}

//******************************************************************************
inline void ColorToVectorNegOneToOneGL(D3DXVECTOR4* pv, D3DCOLOR c) {

    // 0x0 -> 1/128    0x7F -> 128/128    0x80 -> -128/128    0xFF -> -1/128

    pv->x = (-129.0f * (float)((RGBA_GETRED(c) & 0x80) >> 7) + (float)(RGBA_GETRED(c) & 0x7F) + 1.0f) / 128.0f;
    pv->y = (-129.0f * (float)((RGBA_GETGREEN(c) & 0x80) >> 7) + (float)(RGBA_GETGREEN(c) & 0x7F) + 1.0f) / 128.0f;
    pv->z = (-129.0f * (float)((RGBA_GETBLUE(c) & 0x80) >> 7) + (float)(RGBA_GETBLUE(c) & 0x7F) + 1.0f) / 128.0f;
    pv->w = (-129.0f * (float)((RGBA_GETALPHA(c) & 0x80) >> 7) + (float)(RGBA_GETALPHA(c) & 0x7F) + 1.0f) / 128.0f;
//    if (pv->x < -1.0f) pv->x = -1.0f;
//    if (pv->y < -1.0f) pv->y = -1.0f;
//    if (pv->z < -1.0f) pv->z = -1.0f;
//    if (pv->w < -1.0f) pv->w = -1.0f;
}

//******************************************************************************
inline D3DCOLOR VectorToColorNegOneToOneGL(D3DXVECTOR4* pv) {

    D3DXVECTOR4 v;
    BYTE r, g, b, a;

    v = *pv;
    ClampNegOneToOne(&v);

    r = (v.x < 0.0f) ? ((BYTE)(v.x * 128.0f + 128.0f) | 0x80) : (BYTE)(v.x * 128.0f - 1.0f);
    g = (v.y < 0.0f) ? ((BYTE)(v.y * 128.0f + 128.0f) | 0x80) : (BYTE)(v.y * 128.0f - 1.0f);
    b = (v.z < 0.0f) ? ((BYTE)(v.z * 128.0f + 128.0f) | 0x80) : (BYTE)(v.z * 128.0f - 1.0f);
    a = (v.w < 0.0f) ? ((BYTE)(v.w * 128.0f + 128.0f) | 0x80) : (BYTE)(v.w * 128.0f - 1.0f);

    return D3DCOLOR_RGBA(r, g, b, a);
}

//******************************************************************************
inline void ColorToVectorNegOneToOneNV(D3DXVECTOR4* pv, D3DCOLOR c) {

    // 0x0 -> 0/127    0x7F -> 127/127    0x80 -> -128/127    0xFF -> -1/127

    pv->x = (-128.0f * (float)((RGBA_GETRED(c) & 0x80) >> 7) + (float)(RGBA_GETRED(c) & 0x7F)) / 127.0f;
    pv->y = (-128.0f * (float)((RGBA_GETGREEN(c) & 0x80) >> 7) + (float)(RGBA_GETGREEN(c) & 0x7F)) / 127.0f;
    pv->z = (-128.0f * (float)((RGBA_GETBLUE(c) & 0x80) >> 7) + (float)(RGBA_GETBLUE(c) & 0x7F)) / 127.0f;
    pv->w = (-128.0f * (float)((RGBA_GETALPHA(c) & 0x80) >> 7) + (float)(RGBA_GETALPHA(c) & 0x7F)) / 127.0f;
//    if (pv->x < (-128.0f / 127.0f)) pv->x = -128.0f / 127.0f;
//    if (pv->y < (-128.0f / 127.0f)) pv->y = -128.0f / 127.0f;
//    if (pv->z < (-128.0f / 127.0f)) pv->z = -128.0f / 127.0f;
//    if (pv->w < (-128.0f / 127.0f)) pv->w = -128.0f / 127.0f;
}

//******************************************************************************
inline D3DCOLOR VectorToColorNegOneToOneNV(D3DXVECTOR4* pv) {

    D3DXVECTOR4 v;
    BYTE r, g, b, a;

    v = *pv;
    ClampNegOneToOne(&v);

    r = (v.x < 0.0f) ? ((BYTE)(v.x * 127.0f + 128.0f) | 0x80) : (BYTE)(v.x * 127.0f);
    g = (v.x < 0.0f) ? ((BYTE)(v.y * 127.0f + 128.0f) | 0x80) : (BYTE)(v.y * 127.0f);
    b = (v.x < 0.0f) ? ((BYTE)(v.z * 127.0f + 128.0f) | 0x80) : (BYTE)(v.z * 127.0f);
    a = (v.x < 0.0f) ? ((BYTE)(v.w * 127.0f + 128.0f) | 0x80) : (BYTE)(v.w * 127.0f);

    return D3DCOLOR_RGBA(r, g, b, a);
}

//******************************************************************************
inline void ColorToVectorHiloOne(D3DXVECTOR4* pv, D3DCOLOR c) {

    pv->x = (float)D3DCOLOR_HIWORD(c) / 65535.0f;
    pv->y = (float)D3DCOLOR_LOWORD(c) / 65535.0f;
    pv->z = 1.0f;
    pv->w = 1.0f;
}

//******************************************************************************
inline D3DCOLOR VectorToColorHiloOne(D3DXVECTOR4* pv) {

    D3DXVECTOR4 v;

    v = *pv;
    ClampZeroToOne(&v);

    return D3DCOLOR_HILO((WORD)(v.x * 65535.0f + 0.5f), (WORD)(v.y * 65535.0f + 0.5f));
}

//******************************************************************************
inline void ColorToVectorHiloHemisphereMS(D3DXVECTOR4* pv, D3DCOLOR c) {

    // 0x0 -> 0    0x7FFF -> 32767/32768    0x8000 -> -32768/32768    0xFFFF -> -1/32768

    DWORD h, l;
    h = D3DCOLOR_HIWORD(c);
    l = D3DCOLOR_LOWORD(c);
    pv->x = (-32768.0f * (float)((h & 0x8000) >> 7) + (float)(h & 0x7FFF)) / 32768.0f;
    pv->y = (-32768.0f * (float)((l & 0x8000) >> 7) + (float)(l & 0x7FFF)) / 32768.0f;

    pv->z = (float)sqrt(1.0f - pv->x * pv->x - pv->y * pv->y);
    pv->w = 1.0f;
}

//******************************************************************************
inline D3DCOLOR VectorToColorHiloHemisphereMS(D3DXVECTOR4* pv) {

    D3DXVECTOR4 v;
    WORD h, l;

    v = *pv;
    ClampNegOneToOneH(&v);

    h = (v.x < 0.0f) ? ((WORD)(v.x * 32768.0f + 32768.0f) | 0x8000) : (WORD)(v.x * 32768.0f);
    l = (v.x < 0.0f) ? ((WORD)(v.y * 32768.0f + 32768.0f) | 0x8000) : (WORD)(v.y * 32768.0f);

    return D3DCOLOR_HILO(h, l);
}

//******************************************************************************
inline void ColorToVectorHiloHemisphereGL(D3DXVECTOR4* pv, D3DCOLOR c) {

    // 0x0 -> 1/32768    0x7FFF -> 32768/32768    0x8000 -> -32768/32768    0xFFFF -> -1/32768

    DWORD h, l;
    h = D3DCOLOR_HIWORD(c);
    l = D3DCOLOR_LOWORD(c);
    pv->x = (-32769.0f * (float)((h & 0x8000) >> 7) + (float)(h & 0x7FFF) + 1.0f) / 32768.0f;
    pv->y = (-32769.0f * (float)((l & 0x8000) >> 7) + (float)(l & 0x7FFF) + 1.0f) / 32768.0f;

    if (pv->x < -1.0f) pv->x = -1.0f;
    if (pv->y < -1.0f) pv->y = -1.0f;

    pv->z = (float)sqrt(1.0f - pv->x * pv->x - pv->y * pv->y);
    pv->w = 1.0f;
}

//******************************************************************************
inline D3DCOLOR VectorToColorHiloHemisphereGL(D3DXVECTOR4* pv) {

    D3DXVECTOR4 v;
    WORD h, l;

    v = *pv;
    ClampNegOneToOneH(&v);

    h = (v.x < 0.0f) ? ((WORD)(v.x * 32768.0f + 32768.0f) | 0x8000) : (WORD)(v.x * 32768.0f - 1.0f);
    l = (v.x < 0.0f) ? ((WORD)(v.y * 32768.0f + 32768.0f) | 0x8000) : (WORD)(v.y * 32768.0f - 1.0f);

    return D3DCOLOR_HILO(h, l);
}

//******************************************************************************
inline void ColorToVectorHiloHemisphereNV(D3DXVECTOR4* pv, D3DCOLOR c) {

    // 0x0 -> 0    0x7FFF -> 32767/32767    0x8000 -> -32768/32767    0xFFFF -> -1/32767

    DWORD h, l;
    h = D3DCOLOR_HIWORD(c);
    l = D3DCOLOR_LOWORD(c);
    pv->x = (-32768.0f * (float)((h & 0x8000) >> 7) + (float)(h & 0x7FFF)) / 32767.0f;
    pv->y = (-32768.0f * (float)((l & 0x8000) >> 7) + (float)(l & 0x7FFF)) / 32767.0f;

    if (pv->x < -1.0f) pv->x = -1.0f;
    if (pv->y < -1.0f) pv->y = -1.0f;

    pv->z = (float)sqrt(1.0f - pv->x * pv->x - pv->y * pv->y);
    pv->w = 1.0f;
}

//******************************************************************************
inline D3DCOLOR VectorToColorHiloHemisphereNV(D3DXVECTOR4* pv) {

    D3DXVECTOR4 v;
    WORD h, l;

    v = *pv;
    ClampNegOneToOneH(&v);

    h = (v.x < 0.0f) ? ((WORD)(v.x * 32767.0f + 32768.0f) | 0x8000) : (WORD)(v.x * 32767.0f);
    l = (v.x < 0.0f) ? ((WORD)(v.y * 32767.0f + 32768.0f) | 0x8000) : (WORD)(v.y * 32767.0f);

    return D3DCOLOR_HILO(h, l);
}

//******************************************************************************
inline void DotColorToVector(D3DXVECTOR4* pv, D3DCOLOR c, DWORD dwDotMap) {

    switch (dwDotMap) {
        case PS_DOTMAPPING_ZERO_TO_ONE:
            ColorToVectorZeroToOne(pv, c);
        case PS_DOTMAPPING_MINUS1_TO_1_D3D:
            ColorToVectorNegOneToOneMS(pv, c);
        case PS_DOTMAPPING_MINUS1_TO_1_GL:
            ColorToVectorNegOneToOneGL(pv, c);
        case PS_DOTMAPPING_MINUS1_TO_1:
            ColorToVectorNegOneToOneNV(pv, c);
        case PS_DOTMAPPING_HILO_1:
            ColorToVectorHiloOne(pv, c);
        case PS_DOTMAPPING_HILO_HEMISPHERE_D3D:
            ColorToVectorHiloHemisphereMS(pv, c);
        case PS_DOTMAPPING_HILO_HEMISPHERE_GL:
            ColorToVectorHiloHemisphereGL(pv, c);
        case PS_DOTMAPPING_HILO_HEMISPHERE:
            ColorToVectorHiloHemisphereNV(pv, c);
        default:
            __asm int 3;
    }
}

//******************************************************************************
D3DCOLOR LerpColorsZeroToOne(D3DCOLOR c1, D3DCOLOR c2, float d) {

    D3DXVECTOR4 v1, v2, r;
    ColorToVectorZeroToOne(&v1, c1);
    ColorToVectorZeroToOne(&v2, c2);
    v1 = v1 * d;
    v2 = v2 * (1.0f - d);
    r = v1 + v2;
    return VectorToColorZeroToOne(&r);
}

//******************************************************************************
D3DCOLOR LerpColorsNegOneToOneMS(D3DCOLOR c1, D3DCOLOR c2, float d) {

    D3DXVECTOR4 v1, v2, r;
    ColorToVectorNegOneToOneMS(&v1, c1);
    ColorToVectorNegOneToOneMS(&v2, c2);
    v1 = v1 * d;
    v2 = v2 * (1.0f - d);
    r = v1 + v2;
    return VectorToColorNegOneToOneMS(&r);
}
//        cSample[4] = LerpColorsNegOneToOneMS(cSample[0], cSample[1], dx);

//******************************************************************************
D3DCOLOR LerpColorsNegOneToOneGL(D3DCOLOR c1, D3DCOLOR c2, float d) {

    D3DXVECTOR4 v1, v2, r;
    ColorToVectorNegOneToOneGL(&v1, c1);
    ColorToVectorNegOneToOneGL(&v2, c2);
    v1 = v1 * d;
    v2 = v2 * (1.0f - d);
    r = v1 + v2;
    return VectorToColorNegOneToOneGL(&r);
}

//******************************************************************************
D3DCOLOR LerpColorsNegOneToOneNV(D3DCOLOR c1, D3DCOLOR c2, float d) {

    D3DXVECTOR4 v1, v2, r;
    ColorToVectorNegOneToOneNV(&v1, c1);
    ColorToVectorNegOneToOneNV(&v2, c2);
    v1 = v1 * d;
    v2 = v2 * (1.0f - d);
    r = v1 + v2;
    return VectorToColorNegOneToOneNV(&r);
}

//******************************************************************************
D3DCOLOR LerpColorsHiloOne(D3DCOLOR c1, D3DCOLOR c2, float d) {

    D3DXVECTOR4 v1, v2, r;
    ColorToVectorHiloOne(&v1, c1);
    ColorToVectorHiloOne(&v2, c2);
    v1 = v1 * d;
    v2 = v2 * (1.0f - d);
    r = v1 + v2;
    return VectorToColorHiloOne(&r);
}

//******************************************************************************
D3DCOLOR LerpColorsHiloHemisphereMS(D3DCOLOR c1, D3DCOLOR c2, float d) {

    D3DXVECTOR4 v1, v2, r;
    ColorToVectorHiloHemisphereMS(&v1, c1);
    ColorToVectorHiloHemisphereMS(&v2, c2);
    v1 = v1 * d;
    v2 = v2 * (1.0f - d);
    r = v1 + v2;
    return VectorToColorHiloHemisphereMS(&r);
}

//******************************************************************************
D3DCOLOR LerpColorsHiloHemisphereGL(D3DCOLOR c1, D3DCOLOR c2, float d) {

    D3DXVECTOR4 v1, v2, r;
    ColorToVectorHiloHemisphereGL(&v1, c1);
    ColorToVectorHiloHemisphereGL(&v2, c2);
    v1 = v1 * d;
    v2 = v2 * (1.0f - d);
    r = v1 + v2;
    return VectorToColorHiloHemisphereGL(&r);
}

//******************************************************************************
D3DCOLOR LerpColorsHiloHemisphereNV(D3DCOLOR c1, D3DCOLOR c2, float d) {

    D3DXVECTOR4 v1, v2, r;
    ColorToVectorHiloHemisphereNV(&v1, c1);
    ColorToVectorHiloHemisphereNV(&v2, c2);
    v1 = v1 * d;
    v2 = v2 * (1.0f - d);
    r = v1 + v2;
    return VectorToColorHiloHemisphereNV(&r);
}

/*
//******************************************************************************
D3DCOLOR MultiplyColorScalarNegOneToOneMS(D3DCOLOR c, float s) {

    D3DXVECTOR4 v;
    ColorToVectorNegOneToOneMS(&v, c);
    v = v * s;
    return VectorToColorNegOneToOneMS(&v);
}
*/
/*
//******************************************************************************
D3DCOLOR MultiplyHiloColorScalar(D3DCOLOR c, float s) {

    int h, l;

    h = (int)((float)D3DCOLOR_HIWORD(c) * s + 0.5f);
    l = (int)((float)D3DCOLOR_LOWORD(c) * s + 0.5f);

    if (h < 0) h = 0;
    else if (h > 65535) h = 65535;
    if (l < 0) l = 0;
    else if (l > 65535) l = 65535;

    return D3DCOLOR_HILO(h, l);
}
*/
//******************************************************************************
inline D3DCOLOR MultiplyColors(D3DCOLOR c1, D3DCOLOR c2) {

    float r, g, b, a;

    r = ((float)RGBA_GETRED(c1) / 255.0f) * ((float)RGBA_GETRED(c2) / 255.0f);
    g = ((float)RGBA_GETGREEN(c1) / 255.0f) * ((float)RGBA_GETGREEN(c2) / 255.0f);
    b = ((float)RGBA_GETBLUE(c1) / 255.0f) * ((float)RGBA_GETBLUE(c2) / 255.0f);
    a = ((float)RGBA_GETALPHA(c1) / 255.0f) * ((float)RGBA_GETALPHA(c2) / 255.0f);

    return D3DCOLOR_RGBA((BYTE)(r * 255.0f), (BYTE)(g * 255.0f), (BYTE)(b * 255.0f), (BYTE)(a * 255.0f));
}

//******************************************************************************
D3DCOLOR AddColors(D3DCOLOR c1, D3DCOLOR c2) {

    int r, g, b, a;

    r = (int)RGBA_GETRED(c1) + (int)RGBA_GETRED(c2);
    g = (int)RGBA_GETGREEN(c1) + (int)RGBA_GETGREEN(c2);
    b = (int)RGBA_GETBLUE(c1) + (int)RGBA_GETBLUE(c2);
    a = (int)RGBA_GETALPHA(c1) + (int)RGBA_GETALPHA(c2);

    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    if (a > 255) a = 255;

    return D3DCOLOR_RGBA(r, g, b, a);
}

//******************************************************************************
D3DCOLOR AddHiloColors(D3DCOLOR c1, D3DCOLOR c2) {

    int h, l;

    h = (int)D3DCOLOR_HIWORD(c1) + (int)D3DCOLOR_HIWORD(c2);
    l = (int)D3DCOLOR_LOWORD(c1) + (int)D3DCOLOR_LOWORD(c2);

    if (h > 65535) h = 65535;
    if (l > 65535) l = 65535;

    return D3DCOLOR_HILO(h, l);
}

//******************************************************************************
inline D3DCOLOR SubtractColors(D3DCOLOR c1, D3DCOLOR c2) {

    int r, g, b, a;

    r = (int)RGBA_GETRED(c1) - (int)RGBA_GETRED(c2);
    g = (int)RGBA_GETGREEN(c1) - (int)RGBA_GETGREEN(c2);
    b = (int)RGBA_GETBLUE(c1) - (int)RGBA_GETBLUE(c2);
    a = (int)RGBA_GETALPHA(c1) - (int)RGBA_GETALPHA(c2);

    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    if (a < 0) a = 0;

    return D3DCOLOR_RGBA(r, g, b, a);
}

//******************************************************************************
inline void WrapVector(D3DXVECTOR4* pvDst, D3DXVECTOR4* pvSrc) {

    pvDst->x = pvSrc->x - (float)(int)pvSrc->x;
    pvDst->y = pvSrc->y - (float)(int)pvSrc->y;
    pvDst->z = pvSrc->z - (float)(int)pvSrc->z;
    pvDst->w = pvSrc->w - (float)(int)pvSrc->w;

    if (pvDst->x < 0.0f) pvDst->x += 1.0f;
    if (pvDst->y < 0.0f) pvDst->y += 1.0f;
    if (pvDst->z < 0.0f) pvDst->z += 1.0f;
    if (pvDst->w < 0.0f) pvDst->w += 1.0f;
}

/*
//******************************************************************************
inline void ClampNegOneToOne(D3DXVECTOR4* pvReg) {

    if (pvReg->x > 1.0f) pvReg->x = 1.0f;
    else if (pvReg->x < -1.0f) pvReg->x = -1.0f;
    if (pvReg->y > 1.0f) pvReg->y = 1.0f;
    else if (pvReg->y < -1.0f) pvReg->y = -1.0f;
    if (pvReg->z > 1.0f) pvReg->z = 1.0f;
    else if (pvReg->z < -1.0f) pvReg->z = -1.0f;
    if (pvReg->w > 1.0f) pvReg->w = 1.0f;
    else if (pvReg->w < -1.0f) pvReg->w = -1.0f;
}

//******************************************************************************
inline void ClampRNegOneToOne(D3DXVECTOR4* pvReg) {

    float fRegNegOne = -256.0f / 255.0f;

    if (pvReg->x > 1.0f) pvReg->x = 1.0f;
    else if (pvReg->x < fRegNegOne) pvReg->x = fRegNegOne;
    if (pvReg->y > 1.0f) pvReg->y = 1.0f;
    else if (pvReg->y < fRegNegOne) pvReg->y = fRegNegOne;
    if (pvReg->z > 1.0f) pvReg->z = 1.0f;
    else if (pvReg->z < fRegNegOne) pvReg->z = fRegNegOne;
    if (pvReg->w > 1.0f) pvReg->w = 1.0f;
    else if (pvReg->w < fRegNegOne) pvReg->w = fRegNegOne;
}
*/
//******************************************************************************
//
// Function:
//
//     ExhibitScene
//
// Description:
//
//     Create the scene, pump messages, process user input,
//     update the scene, render the scene, and release the scene when finished.
//
// Arguments:
//
//     CDisplay* pDisplay           - Pointer to the Display object
//
//     int* pnExitCode              - Optional pointer to an integer that will
//                                    be set to the exit value contained in the 
//                                    wParam parameter of the WM_QUIT message 
//                                    (if received)
//
// Return Value:
//
//     TRUE if the display remains functional on exit, FALSE otherwise.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ExhibitScene)(CDisplay* pDisplay, int* pnExitCode) {

    CPSRasterizer*  pPSRasterizer;
    BOOL            bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pPSRasterizer = new CPSRasterizer();
    if (!pPSRasterizer) {
        return FALSE;
    }

    // Initialize the scene
    if (!pPSRasterizer->Create(pDisplay)) {
        pPSRasterizer->Release();
        return FALSE;
    }

    bRet = pPSRasterizer->Exhibit(pnExitCode);

    // Clean up the scene
    pPSRasterizer->Release();

    return bRet;
}

//******************************************************************************
//
// Function:
//
//     ValidateDisplay
//
// Description:
//
//     Evaluate the given display information in order to determine whether or
//     not the display is capable of rendering the scene.  If not, the given 
//     display will not be included in the display list.
//
// Arguments:
//
//     CDirect3D8* pd3d                 - Pointer to the Direct3D object
//
//     D3DCAPS8* pd3dcaps               - Capabilities of the device
//
//     D3DDISPLAYMODE*                  - Display mode into which the device
//                                        will be placed
//
// Return Value:
//
//     TRUE if the scene can be rendered using the given display, FALSE if
//     it cannot.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ValidateDisplay)(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    return TRUE;
}

//******************************************************************************
// CPSRasterizer
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CPSRasterizer
//
// Description:
//
//     Initialize the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CPSRasterizer::CPSRasterizer() {

    m_pShaderGen = NULL;
    m_uPSGenerated = 0;
    m_uPSRasterSuccess = 0;
    m_uPSRasterFailure = 0;

    m_bGenerateShader = TRUE;
    m_szVariation[0] = TEXT('\0');
    m_fThreshold = 0.99f;
    m_fMaxMagSq = D3DXVec4LengthSq(&D3DXVECTOR4(255.0f, 255.0f, 255.0f, 255.0f));
//    m_vZero = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
//    m_vOne = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
//    m_vHalf = D3DXVECTOR4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f);

    m_prTriangle[0] = PSVERTEX(D3DXVECTOR3(0.0f, 400.0f, 0.5f), 2.0f, D3DCOLOR_RGBA(255, 0, 0, 240), D3DCOLOR_RGBA(128, 128, 0, 240), D3DXVECTOR4(-2.0f, 2.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.5f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, -4.0f, 0.0f, 1.0f));
    m_prTriangle[1] = PSVERTEX(D3DXVECTOR3(160.0f, 80.0f, 0.5f), 2.0f, D3DCOLOR_RGBA(0, 0, 255, 240), D3DCOLOR_RGBA(0, 128, 128, 240), D3DXVECTOR4(0.0f, -2.0f, 0.0f, 1.0f), D3DXVECTOR4(0.5f, 0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.25f, 0.0f, 0.0f, 1.0f), D3DXVECTOR4(2.0f, 0.0f, 0.0f, 1.0f));
    m_prTriangle[2] = PSVERTEX(D3DXVECTOR3(319.0f, 400.0f, 0.5f), 2.0f, D3DCOLOR_RGBA(0, 255, 0, 240), D3DCOLOR_RGBA(128, 0, 128, 240), D3DXVECTOR4(2.0f, 2.0f, 0.0f, 1.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR4(0.5f, 0.5f, 0.0f, 1.0f), D3DXVECTOR4(4.0f, -4.0f, 0.0f, 1.0f));
}

//******************************************************************************
//
// Method:
//
//     ~CPSRasterizer
//
// Description:
//
//     Clean up the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CPSRasterizer::~CPSRasterizer() {

    if (m_pShaderGen) {
        delete m_pShaderGen;
    }
}

//******************************************************************************
//
// Method:
//
//     Prepare
//
// Description:
//
//     Initialize all device-independent data to be used in the scene.  This
//     method is called only once at creation (as opposed to Setup and
//     Initialize, which get called each time the device is Reset).
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the scene was successfully prepared, FALSE if it was not.
//
//******************************************************************************
BOOL CPSRasterizer::Prepare() {

    // Create the generator
    m_pShaderGen = new CShaderGenerator();
    if (!m_pShaderGen) {
        return FALSE;
    }

    // Seed the generator
    m_pShaderGen->Seed(GetTickCount());

    // Set the cascade frequency
    m_pShaderGen->SetCascadeFrequency(0.65f);

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     Initialize
//
// Description:
//
//     Initialize the device and all device objects to be used in the test (or
//     at least all device resource objects to be created in the video, 
//     non-local video, or default memory pools).
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the test was successfully initialized for rendering, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CPSRasterizer::Initialize() {

    D3DPIXELSHADERDEF d3dpsd;
    UINT i;
    HRESULT hr;

    m_pDevice->GetRenderTarget(&m_pd3dsTarget);

    m_pd3dtBase = (CTexture8*)CreateTexture(m_pDevice, TEXT("zealot.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 0.5f);
    if (!m_pd3dtBase) {
        return FALSE;
    }

#ifdef UNDER_XBOX

    // Create an eight stage shader that emits white
    memset(&d3dpsd, 0, sizeof(D3DPIXELSHADERDEF));

    d3dpsd.PSTextureModes = PS_TEXTUREMODES(PS_TEXTUREMODES_NONE,
                                            PS_TEXTUREMODES_NONE,
                                            PS_TEXTUREMODES_NONE,
                                            PS_TEXTUREMODES_NONE);

    d3dpsd.PSDotMapping = PS_DOTMAPPING(0, PS_DOTMAPPING_MINUS1_TO_1_D3D,
                                            PS_DOTMAPPING_HILO_1,
                                            PS_DOTMAPPING_HILO_HEMISPHERE);

    d3dpsd.PSCompareMode = PS_COMPAREMODE(PS_COMPAREMODE_S_GE,
                                            PS_COMPAREMODE_T_GE,
                                            PS_COMPAREMODE_R_GE,
                                            PS_COMPAREMODE_Q_GE);

    d3dpsd.PSInputTexture = PS_INPUTTEXTURE(0, 0, 1, 2);

    d3dpsd.PSCombinerCount = PS_COMBINERCOUNT(8, PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);

    for (i = 0; i < 8; i++) {

        d3dpsd.PSRGBInputs[i] = PS_COMBINERINPUTS(PS_REGISTER_C0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                                PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
                                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_EXPAND_NORMAL,
                                                PS_REGISTER_C1 | PS_CHANNEL_RGB | PS_INPUTMAPPING_EXPAND_NEGATE);

        d3dpsd.PSRGBOutputs[i] = PS_COMBINEROUTPUTS(PS_REGISTER_R0, PS_REGISTER_R1, PS_REGISTER_T0,
                                                PS_COMBINEROUTPUT_SHIFTLEFT_2 | 
//                                                PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA |
//                                                PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA |
                                                PS_COMBINEROUTPUT_AB_DOT_PRODUCT |
                                                PS_COMBINEROUTPUT_CD_DOT_PRODUCT |
                                                PS_COMBINEROUTPUT_AB_CD_MUX);

        d3dpsd.PSAlphaInputs[i] = PS_COMBINERINPUTS(PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_HALFBIAS_NORMAL,
                                                PS_REGISTER_C0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_HALFBIAS_NEGATE,
                                                PS_REGISTER_C1 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_SIGNED_IDENTITY,
                                                PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_SIGNED_NEGATE);

        d3dpsd.PSAlphaOutputs[i] = PS_COMBINEROUTPUTS(PS_REGISTER_T1, PS_REGISTER_T2, PS_REGISTER_T3,
                                                PS_COMBINEROUTPUT_SHIFTRIGHT_1 | 
                                                PS_COMBINEROUTPUT_AB_CD_MUX);

        d3dpsd.PSConstant0[i] = 0;
        d3dpsd.PSConstant1[i] = 0;
    }


    d3dpsd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_EF_PROD | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
                                            PS_REGISTER_C0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_C1 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_INVERT);

    d3dpsd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
                                            PS_REGISTER_R1 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_INVERT,
                                            PS_FINALCOMBINERSETTING_CLAMP_SUM | 
                                            PS_FINALCOMBINERSETTING_COMPLEMENT_V1 |
                                            PS_FINALCOMBINERSETTING_COMPLEMENT_R0);

    d3dpsd.PSFinalCombinerConstant0 = 0;
    d3dpsd.PSFinalCombinerConstant1 = 0;

//    m_d3dpsd.(initialize)
    hr = m_pDevice->CreatePixelShader(&d3dpsd, &m_dwDiffuseShader);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
        return FALSE;
    }
#endif

    for (i = 0; i < 4; i++) {
        m_pDevice->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        m_pDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    }

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     Efface
//
// Description:
//
//     Release all device resource objects (or at least those objects created
//     in video memory, non-local video memory, or the default memory pools)
//     and restore the device to its initial state.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CPSRasterizer::Efface() {

#ifdef UNDER_XBOX
    m_pDevice->DeletePixelShader(m_dwDiffuseShader);
#endif // UNDER_XBOX
    ReleaseTexture(m_pd3dtBase);
    m_pd3dsTarget->Release();
    CScene::Efface();
}

//******************************************************************************
//
// Method:
//
//     Update
//
// Description:
//
//     Update the state of the scene to coincide with the given time.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CPSRasterizer::Update() {

    UINT i;
    for (i = 0; i < 4; i++) {
        m_pDevice->SetTexture(i, NULL);
    }

    // Generate a pixel shader
    if (m_bGenerateShader) {
        if (!m_pShaderGen->GeneratePixelShader(&m_d3dpsd, NULL, &m_dwInputRegisters)) {
            Log(LOG_ABORT, TEXT("Pixel shader generation failed for shader %d"), (UINT)m_fFrame);
            return;
        }

    m_d3dpsd.PSTextureModes = PS_TEXTUREMODES(PS_TEXTUREMODES_PROJECT2D,
                                            PS_TEXTUREMODES_PROJECT2D,
                                            PS_TEXTUREMODES_PROJECT2D,
                                            PS_TEXTUREMODES_PROJECT2D);

    m_d3dpsd.PSDotMapping = PS_DOTMAPPING(0, PS_DOTMAPPING_ZERO_TO_ONE,
                                            PS_DOTMAPPING_ZERO_TO_ONE,
                                            PS_DOTMAPPING_ZERO_TO_ONE);

    m_d3dpsd.PSCompareMode = PS_COMPAREMODE(PS_COMPAREMODE_S_LT,
                                            PS_COMPAREMODE_S_LT,
                                            PS_COMPAREMODE_S_LT,
                                            PS_COMPAREMODE_S_LT);

    m_d3dpsd.PSInputTexture = PS_INPUTTEXTURE(0, 0, 1, 1);

    }
    else {
        if (!InitPixelShader(&m_d3dpsd, &m_dwInputRegisters)) {
            Log(LOG_ABORT, TEXT("Pixel shader initialization failed for shader %d"), (UINT)m_fFrame);
            return;
        }
    }

    m_uPSGenerated++;

        LPXGBUFFER pxgb;
        m_pShaderGen->OutputPixelShaderDef(&m_d3dpsd, &pxgb);
        Log(LOG_DETAIL, TEXT("Asssembled shader definition (%d):\n\n%S"), m_uPSGenerated, pxgb->GetBufferPointer());
        pxgb->Release();
}

//******************************************************************************
//
// Method:
//
//     Render
//
// Description:
//
//     Render the test scene.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CPSRasterizer::Render() {

    DWORD    dwShader;
    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    UINT     i;
    HRESULT  hr;

#ifndef UNDER_XBOX
    hr = m_pDevice->TestCooperativeLevel();
    if (FAILED(hr)) {
        if (hr == D3DERR_DEVICELOST) {
            return TRUE;
        }
        if (hr == D3DERR_DEVICENOTRESET) {
            if (!Reset()) {
                return FALSE;
            }
        }
    }
#endif // !UNDER_XBOX

    LogBeginVariation(m_szVariation);

#ifdef UNDER_XBOX
    if (!((UINT)m_fFrame % 2)) {
        hr = m_pDevice->CreatePixelShader(&m_d3dpsd, &dwShader);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
            return FALSE;
        }
    }
#else
    dwShader;
#endif

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGBA_MAKE(113, 39, 176, 255), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_PSVERTEX);

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    for (i = 0; i < 4; i++) {
        m_pDevice->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, i);
        m_pDevice->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4 | D3DTTFF_PROJECTED);
    }

m_pDevice->SetTexture(0, m_pd3dtBase);
m_pDevice->SetTexture(1, m_pd3dtBase);
m_pDevice->SetTexture(2, m_pd3dtBase);
m_pDevice->SetTexture(3, m_pd3dtBase);

#ifdef UNDER_XBOX
    if ((UINT)m_fFrame % 2) {

        hr = m_pDevice->SetPixelShader(m_dwDiffuseShader);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPixelShader"))) {
            return FALSE;
        }

        if (!SetShaderStates(&m_d3dpsd)) {
            return FALSE;
        }
    }
    else {

        hr = m_pDevice->SetPixelShader(dwShader);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPixelShader"))) {
            m_pDevice->DeletePixelShader(dwShader);
            return FALSE;
        }
/*
        if (!SetShaderStates(&m_d3dpsdDiffuse)) {
            m_pDevice->DeletePixelShader(dwShader);
            return FALSE;
        }

        hr = m_pDevice->SetPixelShader(dwShader);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetPixelShader"))) {
            m_pDevice->DeletePixelShader(dwShader);
            return FALSE;
        }
*/
    }
#endif

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, m_prTriangle, sizeof(PSVERTEX));

#ifdef UNDER_XBOX
    if (!((UINT)m_fFrame % 2)) {
        m_pDevice->SetPixelShader(NULL);

        hr = m_pDevice->DeletePixelShader(dwShader);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::DeletePixelShader"))) {
            return FALSE;
        }
    }
#endif

    memcpy(m_prTriangleR, m_prTriangle, 3 * sizeof(PSVERTEX));
    for (i = 0; i < 3; i++) {
        m_prTriangleR[i].vPosition.x += 320.0f;
    }

    DrawTriangle(m_prTriangleR);

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // Fade out the scene on exit
    if (m_pfnFade) {
        (this->*m_pfnFade)();
    }

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

    LogEndVariation();

    return TRUE;
}

//******************************************************************************
BOOL CPSRasterizer::InitPixelShader(D3DPIXELSHADERDEF* pd3dpsd, LPDWORD pdwInputRegisters) {

D3DPIXELSHADERDEF psd;
ZeroMemory(&psd, sizeof(psd));

psd.PSCombinerCount=PS_COMBINERCOUNT(
    1,
    PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_SAME_C1);

psd.PSTextureModes=PS_TEXTUREMODES(
    PS_TEXTUREMODES_PROJECT2D,
    PS_TEXTUREMODES_PROJECT2D,
    PS_TEXTUREMODES_PROJECT2D,
    PS_TEXTUREMODES_PROJECT2D);

psd.PSInputTexture=PS_INPUTTEXTURE(0,0,1,1);

psd.PSDotMapping=PS_DOTMAPPING(
    0,
    PS_DOTMAPPING_ZERO_TO_ONE,
    PS_DOTMAPPING_ZERO_TO_ONE,
    PS_DOTMAPPING_ZERO_TO_ONE);

psd.PSCompareMode=PS_COMPAREMODE(
    PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
    PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
    PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
    PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT);

//------------- Stage 0 -------------

psd.PSRGBInputs[0]=PS_COMBINERINPUTS(
    PS_REGISTER_T0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

psd.PSRGBOutputs[0]=PS_COMBINEROUTPUTS(
    PS_REGISTER_R0,
    PS_REGISTER_DISCARD,
    PS_REGISTER_DISCARD,
    PS_COMBINEROUTPUT_AB_MULTIPLY);

//------------- FinalCombiner -------------

psd.PSFinalCombinerInputsABCD=PS_COMBINERINPUTS(
    PS_REGISTER_V1R0_SUM | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

psd.PSFinalCombinerInputsEFG=PS_COMBINERINPUTS(
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_INVERT,
    0 | 0 | 0);

psd.PSFinalCombinerConstant0 = 0x6D860C44;
psd.PSFinalCombinerConstant1 = 0x7F5A7B7B;

psd.PSC0Mapping = 0x8E45C009;

psd.PSC1Mapping = 0x95CF3F6C;

psd.PSFinalCombinerConstants = 0x000000D4;

    memcpy(pd3dpsd, &psd, sizeof(D3DPIXELSHADERDEF));

/*
    memset(pd3dpsd, 0, sizeof(D3DPIXELSHADERDEF));

    pd3dpsd->PSTextureModes = PS_TEXTUREMODES(PS_TEXTUREMODES_PROJECT2D,
                                            PS_TEXTUREMODES_NONE,
                                            PS_TEXTUREMODES_NONE,
                                            PS_TEXTUREMODES_NONE);

    pd3dpsd->PSDotMapping = PS_DOTMAPPING(0, PS_DOTMAPPING_ZERO_TO_ONE,
                                            PS_DOTMAPPING_ZERO_TO_ONE,
                                            PS_DOTMAPPING_ZERO_TO_ONE);

    pd3dpsd->PSCompareMode = PS_COMPAREMODE(PS_COMPAREMODE_S_LT,
                                            PS_COMPAREMODE_S_LT,
                                            PS_COMPAREMODE_S_LT,
                                            PS_COMPAREMODE_S_LT);

    pd3dpsd->PSInputTexture = PS_INPUTTEXTURE(0, 0, 1, 1);

    pd3dpsd->PSCombinerCount = PS_COMBINERCOUNT(1, PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);

    pd3dpsd->PSRGBInputs[0] = PS_COMBINERINPUTS(PS_REGISTER_V0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_T0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

    pd3dpsd->PSRGBOutputs[0] = PS_COMBINEROUTPUTS(PS_REGISTER_R0, PS_REGISTER_DISCARD, PS_REGISTER_DISCARD,
                                            PS_COMBINEROUTPUT_IDENTITY);

    pd3dpsd->PSAlphaInputs[0] = PS_COMBINERINPUTS(PS_REGISTER_V0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_T0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

    pd3dpsd->PSAlphaOutputs[0] = PS_COMBINEROUTPUTS(PS_REGISTER_R0, PS_REGISTER_DISCARD, PS_REGISTER_DISCARD,
                                            PS_COMBINEROUTPUT_IDENTITY);

    pd3dpsd->PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

    pd3dpsd->PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_R0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            0);

    pd3dpsd->PSFinalCombinerConstant0 = 0;
    pd3dpsd->PSFinalCombinerConstant1 = 0;

    if (pdwInputRegisters) {
        *pdwInputRegisters = PSREG_ZERO | PSREG_C0 | PSREG_C1 | PSREG_FOG | 
                             PSREG_V0 | PSREG_V1 | PSREG_T0 | PSREG_T1 | 
                             PSREG_T2 | PSREG_T3 | PSREG_R0 | PSREG_R1 | 
                             PSREG_V1R0_SUM | PSREG_EF_PROD;
    }
*/

/*
    memset(pd3dpsd, 0, sizeof(D3DPIXELSHADERDEF));

    pd3dpsd->PSTextureModes = PS_TEXTUREMODES(PS_TEXTUREMODES_PROJECT2D,
                                            PS_TEXTUREMODES_NONE,
                                            PS_TEXTUREMODES_NONE,
                                            PS_TEXTUREMODES_NONE);

    pd3dpsd->PSDotMapping = PS_DOTMAPPING(0, PS_DOTMAPPING_ZERO_TO_ONE,
                                            PS_DOTMAPPING_ZERO_TO_ONE,
                                            PS_DOTMAPPING_ZERO_TO_ONE);

    pd3dpsd->PSCompareMode = PS_COMPAREMODE(PS_COMPAREMODE_S_LT,
                                            PS_COMPAREMODE_S_LT,
                                            PS_COMPAREMODE_S_LT,
                                            PS_COMPAREMODE_S_LT);

    pd3dpsd->PSInputTexture = PS_INPUTTEXTURE(0, 0, 1, 1);

    pd3dpsd->PSCombinerCount = PS_COMBINERCOUNT(1, PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);

    pd3dpsd->PSRGBInputs[0] = PS_COMBINERINPUTS(PS_REGISTER_T0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ONE | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

    pd3dpsd->PSRGBOutputs[0] = PS_COMBINEROUTPUTS(PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, PS_REGISTER_DISCARD,
                                            PS_COMBINEROUTPUT_IDENTITY | 
                                            PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA |
                                            PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA |
                                            PS_COMBINEROUTPUT_AB_DOT_PRODUCT |
                                            PS_COMBINEROUTPUT_CD_DOT_PRODUCT |
                                            PS_COMBINEROUTPUT_AB_CD_MUX);

    pd3dpsd->PSAlphaInputs[0] = PS_COMBINERINPUTS(PS_REGISTER_T0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ONE | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

    pd3dpsd->PSAlphaOutputs[0] = PS_COMBINEROUTPUTS(PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, PS_REGISTER_DISCARD,
                                            PS_COMBINEROUTPUT_IDENTITY | 
                                            PS_COMBINEROUTPUT_AB_CD_MUX);

    pd3dpsd->PSConstant0[0] = 0;
    pd3dpsd->PSConstant1[0] = 0;

    pd3dpsd->PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_T0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

    pd3dpsd->PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_REGISTER_T0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                                            PS_FINALCOMBINERSETTING_CLAMP_SUM | 
                                            PS_FINALCOMBINERSETTING_COMPLEMENT_V1 |
                                            PS_FINALCOMBINERSETTING_COMPLEMENT_R0);

    pd3dpsd->PSFinalCombinerConstant0 = 0;
    pd3dpsd->PSFinalCombinerConstant1 = 0;

    if (pdwInputRegisters) {
        *pdwInputRegisters = PSREG_ZERO | PSREG_C0 | PSREG_C1 | PSREG_FOG | 
                             PSREG_V0 | PSREG_V1 | PSREG_T0 | PSREG_T1 | 
                             PSREG_T2 | PSREG_T3 | PSREG_R0 | PSREG_R1 | 
                             PSREG_V1R0_SUM | PSREG_EF_PROD;
    }
*/
    return TRUE;
}

/*
//******************************************************************************
void CPSRasterizer::DrawTriangle(PSVERTEX* prTriangle) {

    UINT x, y;
    float fX, fY, fDeltaX, fDeltaY, fX1, fX2;
    D3DCOLOR cDiffuse[2];
    PSVERTEX rIPixel;
    D3DLOCKED_RECT d3dlr;

    m_pd3dsTarget->LockRect(&d3dlr, NULL, 0);

    for (x = 0; x < 3; x++) {
        prTriangle[x].vPosition.x = (float)(UINT)prTriangle[x].vPosition.x;
        prTriangle[x].vPosition.y = (float)(UINT)prTriangle[x].vPosition.y;
        prTriangle[x].vPosition.z = (float)(UINT)prTriangle[x].vPosition.z;
    }

    rIPixel.vPosition.z = prTriangle[0].vPosition.z;

    fY = 0.0f;
    fDeltaY = 1.0f / (prTriangle[0].vPosition.y - prTriangle[1].vPosition.y);

    for (y = (UINT)prTriangle[1].vPosition.y; y <= (UINT)prTriangle[0].vPosition.y; y++, fY += fDeltaY) {

        fX1 = (1.0f - fY) * prTriangle[1].vPosition.x + fY * prTriangle[0].vPosition.x;
        fX2 = (1.0f - fY) * prTriangle[1].vPosition.x + fY * prTriangle[2].vPosition.x;

        cDiffuse[0] = D3DCOLOR_RGBA((BYTE)((1.0f - fY) * (float)RGBA_GETRED(prTriangle[1].cDiffuse) + fY * (float)RGBA_GETRED(prTriangle[0].cDiffuse)),
                                (BYTE)((1.0f - fY) * (float)RGBA_GETGREEN(prTriangle[1].cDiffuse) + fY * (float)RGBA_GETGREEN(prTriangle[0].cDiffuse)),
                                (BYTE)((1.0f - fY) * (float)RGBA_GETBLUE(prTriangle[1].cDiffuse) + fY * (float)RGBA_GETBLUE(prTriangle[0].cDiffuse)),
                                (BYTE)((1.0f - fY) * (float)RGBA_GETALPHA(prTriangle[1].cDiffuse) + fY * (float)RGBA_GETALPHA(prTriangle[0].cDiffuse)));

        cDiffuse[1] = D3DCOLOR_RGBA((BYTE)((1.0f - fY) * (float)RGBA_GETRED(prTriangle[1].cDiffuse) + fY * (float)RGBA_GETRED(prTriangle[2].cDiffuse)),
                                (BYTE)((1.0f - fY) * (float)RGBA_GETGREEN(prTriangle[1].cDiffuse) + fY * (float)RGBA_GETGREEN(prTriangle[2].cDiffuse)),
                                (BYTE)((1.0f - fY) * (float)RGBA_GETBLUE(prTriangle[1].cDiffuse) + fY * (float)RGBA_GETBLUE(prTriangle[2].cDiffuse)),
                                (BYTE)((1.0f - fY) * (float)RGBA_GETALPHA(prTriangle[1].cDiffuse) + fY * (float)RGBA_GETALPHA(prTriangle[2].cDiffuse)));

        fX = 0.0f;
        fDeltaX = 1.0f / (fX2 - fX1);

        rIPixel.vPosition.y = (float)y;

        for (x = (UINT)fX1; x <= (UINT)fX2; x++, fX += fDeltaX) {

            rIPixel.vPosition.x = (float)x;

            rIPixel.cDiffuse = D3DCOLOR_RGBA((BYTE)((1.0f - fX) * (float)RGBA_GETRED(cDiffuse[0]) + fX * (float)RGBA_GETRED(cDiffuse[1])),
                                         (BYTE)((1.0f - fX) * (float)RGBA_GETGREEN(cDiffuse[0]) + fX * (float)RGBA_GETGREEN(cDiffuse[1])),
                                         (BYTE)((1.0f - fX) * (float)RGBA_GETBLUE(cDiffuse[0]) + fX * (float)RGBA_GETBLUE(cDiffuse[1])),
                                         (BYTE)((1.0f - fX) * (float)RGBA_GETALPHA(cDiffuse[0]) + fX * (float)RGBA_GETALPHA(cDiffuse[1])));

            *((LPDWORD)((LPBYTE)d3dlr.pBits + y * d3dlr.Pitch) + x) = ShadePixel(&rIPixel);
        }
    }

    m_pd3dsTarget->UnlockRect();
}
*/

//******************************************************************************
void CPSRasterizer::DrawTriangle(PSVERTEX* prTriangle) {

    float x, y, fBottom, fLeft, fRight, fWidth, fDeltaX, fDeltaYL, fDeltaYR;
    int i;
    PSVERTEX prTri[3];
    PSVERTEX rIPixel;
    D3DLOCKED_RECT d3dlr;
    LPDWORD pdwPixel;
    D3DCOLOR cDiffuse[2];
    D3DCOLOR cSpecular[2];
    D3DXVECTOR4 pvTCoord[4][2];
    BOOL bMatch = TRUE;

    // Get the bump environment map transform and luminance values
    for (i = 0; i < 4; i++) {
        m_pDevice->GetTextureStageState(i, D3DTSS_BUMPENVMAT00, (LPDWORD)&m_fBumpEnvMat[i][0]);
        m_pDevice->GetTextureStageState(i, D3DTSS_BUMPENVMAT01, (LPDWORD)&m_fBumpEnvMat[i][1]);
        m_pDevice->GetTextureStageState(i, D3DTSS_BUMPENVMAT10, (LPDWORD)&m_fBumpEnvMat[i][2]);
        m_pDevice->GetTextureStageState(i, D3DTSS_BUMPENVMAT11, (LPDWORD)&m_fBumpEnvMat[i][3]);
        m_pDevice->GetTextureStageState(i, D3DTSS_BUMPENVLOFFSET, (LPDWORD)&m_fLumOffset[i]);
        m_pDevice->GetTextureStageState(i, D3DTSS_BUMPENVLSCALE, (LPDWORD)&m_fLumScale[i]);
    }

    memcpy(prTri, prTriangle, 3 * sizeof(PSVERTEX));

    // Sort the vertices in ascending y order
    for (i = 1; i >= 0; i--) {
        if (prTri[i+1].vPosition.y < prTri[i].vPosition.y || (prTri[i+1].vPosition.y == prTri[i].vPosition.y && prTri[i+1].vPosition.x < prTri[i].vPosition.x)) {
            memcpy(&rIPixel, &prTri[i], sizeof(PSVERTEX));
            memcpy(&prTri[i], &prTri[i+1], sizeof(PSVERTEX));
            memcpy(&prTri[i+1], &rIPixel, sizeof(PSVERTEX));
        }
    }

    // Sort the last two vertices in ascending x order
    if (prTri[0].vPosition.y != prTri[1].vPosition.y) {
        if (prTri[2].vPosition.x < prTri[1].vPosition.x) {
            memcpy(&rIPixel, &prTri[1], sizeof(PSVERTEX));
            memcpy(&prTri[1], &prTri[2], sizeof(PSVERTEX));
            memcpy(&prTri[2], &rIPixel, sizeof(PSVERTEX));
        }
    }

    if (prTri[1].vPosition.y > prTri[2].vPosition.y) {
        fBottom = prTri[1].vPosition.y;
    }
    else {
        fBottom = prTri[2].vPosition.y;
    }

    m_pd3dsTarget->LockRect(&d3dlr, NULL, 0);

    for (y = (float)(UINT)(prTri[0].vPosition.y + 0.5f); y < fBottom; y += 1.0f) {

        if (y < prTri[1].vPosition.y) {
            fDeltaYL = (y - prTri[0].vPosition.y) / (prTri[1].vPosition.y - prTri[0].vPosition.y);
            fLeft = (1.0f - fDeltaYL) * prTri[0].vPosition.x + fDeltaYL * prTri[1].vPosition.x;
        }
        else {
            fDeltaYL = (y - prTri[1].vPosition.y) / (prTri[2].vPosition.y - prTri[1].vPosition.y);
            fLeft = (1.0f - fDeltaYL) * prTri[1].vPosition.x + fDeltaYL * prTri[2].vPosition.x;
        }
        if (y < prTri[2].vPosition.y) {
            fDeltaYR = (y - prTri[0].vPosition.y) / (prTri[2].vPosition.y - prTri[0].vPosition.y);
            fRight = (1.0f - fDeltaYR) * prTri[0].vPosition.x + fDeltaYR * prTri[2].vPosition.x;
        }
        else {
            fDeltaYR = (y - prTri[2].vPosition.y) / (prTri[1].vPosition.y - prTri[2].vPosition.y);
            fRight = (1.0f - fDeltaYR) * prTri[2].vPosition.x + fDeltaYR * prTri[1].vPosition.x;
        }

        cDiffuse[0] = D3DCOLOR_RGBA((BYTE)((1.0f - fDeltaYL) * (float)RGBA_GETRED(prTriangle[1].cDiffuse) + fDeltaYL * (float)RGBA_GETRED(prTriangle[0].cDiffuse)),
                                    (BYTE)((1.0f - fDeltaYL) * (float)RGBA_GETGREEN(prTriangle[1].cDiffuse) + fDeltaYL * (float)RGBA_GETGREEN(prTriangle[0].cDiffuse)),
                                    (BYTE)((1.0f - fDeltaYL) * (float)RGBA_GETBLUE(prTriangle[1].cDiffuse) + fDeltaYL * (float)RGBA_GETBLUE(prTriangle[0].cDiffuse)),
                                    (BYTE)((1.0f - fDeltaYL) * (float)RGBA_GETALPHA(prTriangle[1].cDiffuse) + fDeltaYL * (float)RGBA_GETALPHA(prTriangle[0].cDiffuse)));

        cDiffuse[1] = D3DCOLOR_RGBA((BYTE)((1.0f - fDeltaYR) * (float)RGBA_GETRED(prTriangle[1].cDiffuse) + fDeltaYR * (float)RGBA_GETRED(prTriangle[2].cDiffuse)),
                                    (BYTE)((1.0f - fDeltaYR) * (float)RGBA_GETGREEN(prTriangle[1].cDiffuse) + fDeltaYR * (float)RGBA_GETGREEN(prTriangle[2].cDiffuse)),
                                    (BYTE)((1.0f - fDeltaYR) * (float)RGBA_GETBLUE(prTriangle[1].cDiffuse) + fDeltaYR * (float)RGBA_GETBLUE(prTriangle[2].cDiffuse)),
                                    (BYTE)((1.0f - fDeltaYR) * (float)RGBA_GETALPHA(prTriangle[1].cDiffuse) + fDeltaYR * (float)RGBA_GETALPHA(prTriangle[2].cDiffuse)));

        cSpecular[0] = D3DCOLOR_RGBA((BYTE)((1.0f - fDeltaYL) * (float)RGBA_GETRED(prTriangle[1].cSpecular) + fDeltaYL * (float)RGBA_GETRED(prTriangle[0].cSpecular)),
                                     (BYTE)((1.0f - fDeltaYL) * (float)RGBA_GETGREEN(prTriangle[1].cSpecular) + fDeltaYL * (float)RGBA_GETGREEN(prTriangle[0].cSpecular)),
                                     (BYTE)((1.0f - fDeltaYL) * (float)RGBA_GETBLUE(prTriangle[1].cSpecular) + fDeltaYL * (float)RGBA_GETBLUE(prTriangle[0].cSpecular)),
                                     (BYTE)((1.0f - fDeltaYL) * (float)RGBA_GETALPHA(prTriangle[1].cSpecular) + fDeltaYL * (float)RGBA_GETALPHA(prTriangle[0].cSpecular)));

        cSpecular[1] = D3DCOLOR_RGBA((BYTE)((1.0f - fDeltaYR) * (float)RGBA_GETRED(prTriangle[1].cSpecular) + fDeltaYR * (float)RGBA_GETRED(prTriangle[2].cSpecular)),
                                     (BYTE)((1.0f - fDeltaYR) * (float)RGBA_GETGREEN(prTriangle[1].cSpecular) + fDeltaYR * (float)RGBA_GETGREEN(prTriangle[2].cSpecular)),
                                     (BYTE)((1.0f - fDeltaYR) * (float)RGBA_GETBLUE(prTriangle[1].cSpecular) + fDeltaYR * (float)RGBA_GETBLUE(prTriangle[2].cSpecular)),
                                     (BYTE)((1.0f - fDeltaYR) * (float)RGBA_GETALPHA(prTriangle[1].cSpecular) + fDeltaYR * (float)RGBA_GETALPHA(prTriangle[2].cSpecular)));

        for (i = 0; i < 4; i++) {
            pvTCoord[i][0] = (1.0f - fDeltaYL) * prTriangle[1].pvTCoord[i] + fDeltaYL * prTriangle[0].pvTCoord[i];
            pvTCoord[i][1] = (1.0f - fDeltaYR) * prTriangle[1].pvTCoord[i] + fDeltaYR * prTriangle[2].pvTCoord[i];
        }

        fWidth = fRight - fLeft;

        rIPixel.vPosition.y = (float)y;

        for (x = (float)(UINT)(fLeft + 0.5f); x < fRight; x += 1.0f) {

            fDeltaX = (x - fLeft) / fWidth;

            rIPixel.vPosition.x = (float)x;

            rIPixel.cDiffuse = D3DCOLOR_RGBA((BYTE)((1.0f - fDeltaX) * (float)RGBA_GETRED(cDiffuse[0]) + fDeltaX * (float)RGBA_GETRED(cDiffuse[1])),
                                             (BYTE)((1.0f - fDeltaX) * (float)RGBA_GETGREEN(cDiffuse[0]) + fDeltaX * (float)RGBA_GETGREEN(cDiffuse[1])),
                                             (BYTE)((1.0f - fDeltaX) * (float)RGBA_GETBLUE(cDiffuse[0]) + fDeltaX * (float)RGBA_GETBLUE(cDiffuse[1])),
                                             (BYTE)((1.0f - fDeltaX) * (float)RGBA_GETALPHA(cDiffuse[0]) + fDeltaX * (float)RGBA_GETALPHA(cDiffuse[1])));

            rIPixel.cSpecular = D3DCOLOR_RGBA((BYTE)((1.0f - fDeltaX) * (float)RGBA_GETRED(cSpecular[0]) + fDeltaX * (float)RGBA_GETRED(cSpecular[1])),
                                              (BYTE)((1.0f - fDeltaX) * (float)RGBA_GETGREEN(cSpecular[0]) + fDeltaX * (float)RGBA_GETGREEN(cSpecular[1])),
                                              (BYTE)((1.0f - fDeltaX) * (float)RGBA_GETBLUE(cSpecular[0]) + fDeltaX * (float)RGBA_GETBLUE(cSpecular[1])),
                                              (BYTE)((1.0f - fDeltaX) * (float)RGBA_GETALPHA(cSpecular[0]) + fDeltaX * (float)RGBA_GETALPHA(cSpecular[1])));

            for (i = 0; i < 4; i++) {
                rIPixel.pvTCoord[i] = (1.0f - fDeltaX) * pvTCoord[i][0] + fDeltaX * pvTCoord[i][1];
            }

            pdwPixel = ((LPDWORD)((LPBYTE)d3dlr.pBits + (UINT)(y + 0.5f) * d3dlr.Pitch) + (UINT)(x + 0.5f));
            ShadePixel(pdwPixel, &rIPixel);
            if (bMatch) {
                bMatch = ComparePixels(*((LPDWORD)((LPBYTE)d3dlr.pBits + (UINT)(y + 0.5f) * d3dlr.Pitch) + (UINT)(x - 320.0f + 0.5f)), *pdwPixel);
                if (!bMatch) {
                    Log(LOG_FAIL, TEXT("Pixel %d, %d does not match the reference pixel for test %d"), (UINT)x - 320, (UINT)y, (UINT)m_fFrame);
//__asm int 3;
                }
            }
        }
    }

    m_pd3dsTarget->UnlockRect();

    if (bMatch) {
        m_uPSRasterSuccess++;
    }
    else {
        m_uPSRasterFailure++;
    }
}

//******************************************************************************
D3DCOLOR CPSRasterizer::GetBlendColor(BOOL bSource, D3DCOLOR cDstPixel, D3DCOLOR cSrcPixel) {

    D3DCOLOR  cBlend;
    D3DBLEND blend;
    BYTE     alpha;

    m_pDevice->GetRenderState(bSource ? D3DRS_SRCBLEND : D3DRS_DESTBLEND, (LPDWORD)&blend);

    switch (blend) {

        case D3DBLEND_ZERO:
            cBlend = D3DCOLOR_RGBA(0, 0, 0, 0);
            break;
        case D3DBLEND_ONE:
            cBlend = D3DCOLOR_RGBA(1, 1, 1, 1);
            break;
        case D3DBLEND_SRCCOLOR:
            cBlend = D3DCOLOR_RGBA(RGBA_GETRED(cSrcPixel), RGBA_GETGREEN(cSrcPixel), RGBA_GETBLUE(cSrcPixel), RGBA_GETALPHA(cSrcPixel));
            break;
        case D3DBLEND_INVSRCCOLOR:
            cBlend = D3DCOLOR_RGBA(255 - RGBA_GETRED(cSrcPixel), 255 - RGBA_GETGREEN(cSrcPixel), 255 - RGBA_GETBLUE(cSrcPixel), 255 - RGBA_GETALPHA(cSrcPixel));
            break;
        case D3DBLEND_SRCALPHA:
            cBlend = D3DCOLOR_RGBA(RGBA_GETALPHA(cSrcPixel), RGBA_GETALPHA(cSrcPixel), RGBA_GETALPHA(cSrcPixel), RGBA_GETALPHA(cSrcPixel));
            break;
        case D3DBLEND_INVSRCALPHA:
            cBlend = D3DCOLOR_RGBA(255 - RGBA_GETALPHA(cSrcPixel), 255 - RGBA_GETALPHA(cSrcPixel), 255 - RGBA_GETALPHA(cSrcPixel), 255 - RGBA_GETALPHA(cSrcPixel));
            break;
        case D3DBLEND_DESTALPHA:
            cBlend = D3DCOLOR_RGBA(RGBA_GETALPHA(cDstPixel), RGBA_GETALPHA(cDstPixel), RGBA_GETALPHA(cDstPixel), RGBA_GETALPHA(cDstPixel));
            break;
        case D3DBLEND_INVDESTALPHA:
            cBlend = D3DCOLOR_RGBA(255 - RGBA_GETALPHA(cDstPixel), 255 - RGBA_GETALPHA(cDstPixel), 255 - RGBA_GETALPHA(cDstPixel), 255 - RGBA_GETALPHA(cDstPixel));
            break;
        case D3DBLEND_DESTCOLOR:
            cBlend = D3DCOLOR_RGBA(RGBA_GETRED(cDstPixel), RGBA_GETGREEN(cDstPixel), RGBA_GETBLUE(cDstPixel), RGBA_GETALPHA(cDstPixel));
            break;
        case D3DBLEND_INVDESTCOLOR:
            cBlend = D3DCOLOR_RGBA(255 - RGBA_GETRED(cDstPixel), 255 - RGBA_GETGREEN(cDstPixel), 255 - RGBA_GETBLUE(cDstPixel), 255 - RGBA_GETALPHA(cDstPixel));
            break;
        case D3DBLEND_SRCALPHASAT:
            alpha = RGBA_GETALPHA(cSrcPixel) < RGBA_GETALPHA(cDstPixel) ? (BYTE)RGBA_GETALPHA(cSrcPixel) : (BYTE)RGBA_GETALPHA(cDstPixel);
            cBlend = D3DCOLOR_RGBA(alpha, alpha, alpha, 255);
            break;
//        case D3DBLEND_BOTHSRCALPHA:
//        case D3DBLEND_BOTHINVSRCALPHA:
        default:
            DebugString(TEXT("Unsupported blend mode set"));
            __asm int 3;
            cBlend = 0;
            break;
    }

    return cBlend;
}

//#define PS_TM0_MASK     0x0000000F
//#define PS_TM1_MASK     0x000003F0
//#define PS_TM2_MASK     0x00007C00
//#define PS_TM3_MASK     0x000F8000
#define PS_TEXMODE_MASK     0x0000001F
#define PS_GETDOTMAP(d, s)  (((d).PSTextureModes >> ((s-1)*4)) & 0x7)

//******************************************************************************
void CPSRasterizer::TexturePixel(PSVERTEX* prIPixel) {

    D3DXVECTOR4*    pvTemp;
    UINT            uInput;

    // Texture addressing
    switch (m_d3dpsd.PSTextureModes & PS_TEXMODE_MASK) {
        case PS_TEXTUREMODES_NONE:
            m_reg.t0 = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case PS_TEXTUREMODES_PROJECT2D:
        case PS_TEXTUREMODES_PROJECT3D:
            ColorToVector(&m_reg.t0, SampleTexture(0, &prIPixel->pvTCoord[0], FALSE));
            break;
        case PS_TEXTUREMODES_CUBEMAP:
            ColorToVector(&m_reg.t0, SampleTexture(0, &prIPixel->pvTCoord[0], TRUE));
            break;
        case PS_TEXTUREMODES_PASSTHRU:
            WrapVector(&m_reg.t0, &prIPixel->pvTCoord[0]);
            break;
//        case PS_TEXTUREMODES_CLIPPLANE:
//            break;
    }

    switch ((m_d3dpsd.PSTextureModes >> 5) & PS_TEXMODE_MASK) {
        case PS_TEXTUREMODES_NONE:
            m_reg.t1 = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case PS_TEXTUREMODES_PROJECT2D:
        case PS_TEXTUREMODES_PROJECT3D:
            ColorToVector(&m_reg.t1, SampleTexture(1, &prIPixel->pvTCoord[1], FALSE));
            break;
        case PS_TEXTUREMODES_CUBEMAP:
            ColorToVector(&m_reg.t1, SampleTexture(1, &prIPixel->pvTCoord[1], TRUE));
            break;
        case PS_TEXTUREMODES_PASSTHRU:
            WrapVector(&m_reg.t1, &prIPixel->pvTCoord[1]);
            break;
//        case PS_TEXTUREMODES_CLIPPLANE:
//            break;
        case PS_TEXTUREMODES_BUMPENVMAP:
        case PS_TEXTUREMODES_BUMPENVMAP_LUM:
            pvTemp = &m_reg.t0;
// REVIEW: What pvTemp channels should be used as the delta values?  x and y? z and y?  Also, which ends up being luminance?
            ColorToVector(&m_reg.t1, SampleTexture(1, 
                &D3DXVECTOR4(prIPixel->pvTCoord[1].x + m_fBumpEnvMat[1][0] * pvTemp->x + m_fBumpEnvMat[1][2] * pvTemp->y, 
                             prIPixel->pvTCoord[1].y + m_fBumpEnvMat[1][1] * pvTemp->x + m_fBumpEnvMat[1][3] * pvTemp->y,
                             0.0f, 1.0f), 
                FALSE));
            break;
        case PS_TEXTUREMODES_DPNDNT_AR:
        case PS_TEXTUREMODES_DPNDNT_GB:
            pvTemp = &m_reg.t0;
            if (((m_d3dpsd.PSTextureModes >> 5) & PS_TEXMODE_MASK) == PS_TEXTUREMODES_DPNDNT_AR) {
                ColorToVector(&m_reg.t1, SampleTexture(1, &D3DXVECTOR4(pvTemp->w, pvTemp->x, 0.0f, 1.0f), FALSE));
            }
            else {
                ColorToVector(&m_reg.t1, SampleTexture(1, &D3DXVECTOR4(pvTemp->y, pvTemp->z, 0.0f, 1.0f), FALSE));
            }
            break;
        case PS_TEXTUREMODES_DOTPRODUCT:
// REVIEW: Should a 3D or 4D dotproduct be performed?  What happens to alpha in this instance? (For that matter, what happens to alpha during dot mapping?)
            DotColorToVector(&m_reg.t1, SampleTexture(0, &prIPixel->pvTCoord[0], FALSE, PS_GETDOTMAP(m_d3dpsd, 1)), PS_GETDOTMAP(m_d3dpsd, 1));
            m_reg.t1.w = D3DXVec4Dot(&prIPixel->pvTCoord[1], &m_reg.t1);
            break;
    }

    switch ((m_d3dpsd.PSTextureModes >> 10) & PS_TEXMODE_MASK) {
        case PS_TEXTUREMODES_NONE:
            m_reg.t2 = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case PS_TEXTUREMODES_PROJECT2D:
        case PS_TEXTUREMODES_PROJECT3D:
            ColorToVector(&m_reg.t2, SampleTexture(2, &prIPixel->pvTCoord[2], FALSE));
            break;
        case PS_TEXTUREMODES_CUBEMAP:
            ColorToVector(&m_reg.t2, SampleTexture(2, &prIPixel->pvTCoord[2], TRUE));
            break;
        case PS_TEXTUREMODES_PASSTHRU:
            WrapVector(&m_reg.t2, &prIPixel->pvTCoord[2]);
            break;
//        case PS_TEXTUREMODES_CLIPPLANE:
//            break;
        case PS_TEXTUREMODES_BUMPENVMAP:
        case PS_TEXTUREMODES_BUMPENVMAP_LUM:
            pvTemp = &m_reg.t0 + ((m_d3dpsd.PSInputTexture >> 16) & 0x1);
// REVIEW: What pvTemp channels should be used as the delta values?  x and y? z and y?  Also, which ends up being luminance?
            ColorToVector(&m_reg.t2, SampleTexture(2, 
                &D3DXVECTOR4(prIPixel->pvTCoord[2].x + m_fBumpEnvMat[1][0] * pvTemp->x + m_fBumpEnvMat[1][2] * pvTemp->y, 
                             prIPixel->pvTCoord[2].y + m_fBumpEnvMat[1][1] * pvTemp->x + m_fBumpEnvMat[1][3] * pvTemp->y,
                             0.0f, 1.0f), 
                FALSE));
            break;
        case PS_TEXTUREMODES_DPNDNT_AR:
        case PS_TEXTUREMODES_DPNDNT_GB:
            pvTemp = &m_reg.t0 + ((m_d3dpsd.PSInputTexture >> 16) & 0x1);
            if (((m_d3dpsd.PSTextureModes >> 10) & PS_TEXMODE_MASK) == PS_TEXTUREMODES_DPNDNT_AR) {
                ColorToVector(&m_reg.t2, SampleTexture(2, &D3DXVECTOR4(pvTemp->w, pvTemp->x, 0.0f, 1.0f), FALSE));
            }
            else {
                ColorToVector(&m_reg.t2, SampleTexture(2, &D3DXVECTOR4(pvTemp->y, pvTemp->z, 0.0f, 1.0f), FALSE));
            }
            break;
        case PS_TEXTUREMODES_DOTPRODUCT:
        case PS_TEXTUREMODES_DOT_ST:
        case PS_TEXTUREMODES_DOT_ZW:
            uInput = ((m_d3dpsd.PSInputTexture >> 16) & 0x1);
            DotColorToVector(&m_reg.t2, SampleTexture(uInput, &prIPixel->pvTCoord[uInput], FALSE, PS_GETDOTMAP(m_d3dpsd, 2)), PS_GETDOTMAP(m_d3dpsd, 2));
            m_reg.t2.w = D3DXVec4Dot(&prIPixel->pvTCoord[2], &m_reg.t2);
            switch ((m_d3dpsd.PSTextureModes >> 10) & PS_TEXMODE_MASK) {
                case PS_TEXTUREMODES_DOT_ST:
                    ColorToVector(&m_reg.t2, SampleTexture(2, &D3DXVECTOR4(m_reg.t1.w, m_reg.t2.w, 0.0f, 0.0f), FALSE));
                    break;
                case PS_TEXTUREMODES_DOT_ZW:
                    
                    break;
            }
            break;
// REVIEW: Assumption: PSInputTexture for things like "Dot product ST" refers to the input sample color to dotproduct with the texture coordinates in the current stage to calculate T and NOT to the previous stage in which the S portion of the dotproduct was calculated...
    }

    switch ((m_d3dpsd.PSTextureModes >> 15) & PS_TEXMODE_MASK) {
        case PS_TEXTUREMODES_NONE:
            m_reg.t3 = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case PS_TEXTUREMODES_PROJECT2D:
        case PS_TEXTUREMODES_PROJECT3D:
            ColorToVector(&m_reg.t3, SampleTexture(3, &prIPixel->pvTCoord[3], FALSE));
            break;
        case PS_TEXTUREMODES_CUBEMAP:
            ColorToVector(&m_reg.t3, SampleTexture(3, &prIPixel->pvTCoord[3], TRUE));
            break;
        case PS_TEXTUREMODES_PASSTHRU:
            WrapVector(&m_reg.t3, &prIPixel->pvTCoord[3]);
            break;
//        case PS_TEXTUREMODES_CLIPPLANE:
//            break;
        case PS_TEXTUREMODES_BUMPENVMAP:
        case PS_TEXTUREMODES_BUMPENVMAP_LUM:
            pvTemp = &m_reg.t0 + ((m_d3dpsd.PSInputTexture >> 20) & 0x3);
// REVIEW: What pvTemp channels should be used as the delta values?  x and y? z and y?  Also, which ends up being luminance?
            ColorToVector(&m_reg.t3, SampleTexture(3, 
                &D3DXVECTOR4(prIPixel->pvTCoord[3].x + m_fBumpEnvMat[1][0] * pvTemp->x + m_fBumpEnvMat[1][2] * pvTemp->y, 
                             prIPixel->pvTCoord[3].y + m_fBumpEnvMat[1][1] * pvTemp->x + m_fBumpEnvMat[1][3] * pvTemp->y,
                             0.0f, 1.0f), 
                FALSE));
            break;
        case PS_TEXTUREMODES_DPNDNT_AR:
        case PS_TEXTUREMODES_DPNDNT_GB:
            pvTemp = &m_reg.t0 + ((m_d3dpsd.PSInputTexture >> 20) & 0x3);
            if (((m_d3dpsd.PSTextureModes >> 15) & PS_TEXMODE_MASK) == PS_TEXTUREMODES_DPNDNT_AR) {
                ColorToVector(&m_reg.t3, SampleTexture(3, &D3DXVECTOR4(pvTemp->w, pvTemp->x, 0.0f, 1.0f), FALSE));
            }
            else {
                ColorToVector(&m_reg.t3, SampleTexture(3, &D3DXVECTOR4(pvTemp->y, pvTemp->z, 0.0f, 1.0f), FALSE));
            }
            break;
        case PS_TEXTUREMODES_DOT_ST:
        case PS_TEXTUREMODES_DOT_STR_3D:
        case PS_TEXTUREMODES_DOT_STR_CUBE:
            uInput = ((m_d3dpsd.PSInputTexture >> 20) & 0x3);
            DotColorToVector(&m_reg.t3, SampleTexture(uInput, &prIPixel->pvTCoord[uInput], FALSE, PS_GETDOTMAP(m_d3dpsd, 3)), PS_GETDOTMAP(m_d3dpsd, 3));
            m_reg.t3.w = D3DXVec4Dot(&prIPixel->pvTCoord[3], &m_reg.t3);
            switch ((m_d3dpsd.PSTextureModes >> 15) & PS_TEXMODE_MASK) {
                case PS_TEXTUREMODES_DOT_ST:
                    ColorToVector(&m_reg.t3, SampleTexture(3, &D3DXVECTOR4(m_reg.t2.w, m_reg.t3.w, 0.0f, 0.0f), FALSE));
                    break;
                case PS_TEXTUREMODES_DOT_STR_3D:
                    ColorToVector(&m_reg.t3, SampleTexture(3, &D3DXVECTOR4(m_reg.t1.w, m_reg.t2.w, m_reg.t3.w, 0.0f), FALSE));
                    break;
                case PS_TEXTUREMODES_DOT_STR_CUBE:
                    ColorToVector(&m_reg.t3, SampleTexture(3, &D3DXVECTOR4(m_reg.t1.w, m_reg.t2.w, m_reg.t3.w, 0.0f), TRUE));
                    break;
            }
    }

    // Apply any luminance values (this must be done after texture addressing has been performed in all stages but
    // before the results are sent to the combiners)
    if (((m_d3dpsd.PSTextureModes >> 5) & PS_TEXMODE_MASK) == PS_TEXTUREMODES_BUMPENVMAP_LUM) {
        float fLum;
        pvTemp = &m_reg.t0;
        fLum = m_fLumOffset[1] + m_fLumScale[1] * pvTemp->z;
        if (fLum < 0.0f) fLum = 0.0f;
        else if (fLum > 1.0f) fLum = 1.0f;
        m_reg.t1 *= fLum;
    }
    if (((m_d3dpsd.PSTextureModes >> 10) & PS_TEXMODE_MASK) == PS_TEXTUREMODES_BUMPENVMAP_LUM) {
        float fLum;
        pvTemp = &m_reg.t0 + ((m_d3dpsd.PSInputTexture >> 16) & 0x1);
        fLum = m_fLumOffset[1] + m_fLumScale[1] * pvTemp->z;
        if (fLum < 0.0f) fLum = 0.0f;
        else if (fLum > 1.0f) fLum = 1.0f;
        m_reg.t2 *= fLum;
    }
    if (((m_d3dpsd.PSTextureModes >> 15) & PS_TEXMODE_MASK) == PS_TEXTUREMODES_BUMPENVMAP_LUM) {
        float fLum;
        pvTemp = &m_reg.t0 + ((m_d3dpsd.PSInputTexture >> 20) & 0x3);
        fLum = m_fLumOffset[1] + m_fLumScale[1] * pvTemp->z;
        if (fLum < 0.0f) fLum = 0.0f;
        else if (fLum > 1.0f) fLum = 1.0f;
        m_reg.t3 *= fLum;
    }

}

/*
#define PS_CC_ENABLE_MASK   0x0000000F
#define REG_INPUT_A_MASK    0x0F000000
#define REG_INPUT_B_MASK    0x000F0000
#define REG_INPUT_C_MASK    0x00000F00
#define REG_INPUT_D_MASK    0x0000000F
#define REG_INMAP_A_MASK    0xE0000000
#define REG_INMAP_B_MASK    0x00E00000
#define REG_INMAP_C_MASK    0x0000E000
#define REG_INMAP_D_MASK    0x000000E0
#define REG_INMAP_CHANNEL_MASK 0x00000001
#define REG_OUTPUT_R_MASK   0x00000F00
#define REG_OUTPUT_AB_MASK  0x000000F0
#define REG_OUTPUT_CD_MASK  0x0000000F
#define REG_OUTMAP_R_MASK   
#define REG_OUTMAP_AB_MASK  
#define REG_OUTMAP_CD_MASK  
#define REG_INPUT_A_SHIFT   24
#define REG_INPUT_B_SHIFT   16
#define REG_INPUT_C_SHIFT   8
#define REG_INPUT_D_SHIFT   0
#define REG_INMAP_A_SHIFT   28
#define REG_INMAP_B_SHIFT   20
#define REG_INMAP_C_SHIFT   12
#define REG_INMAP_D_SHIFT   4
#define REG_OUTPUT_R_SHIFT  8
#define REG_OUTPUT_AB_SHIFT 4
#define REG_OUTPUT_CD_SHIFT 0
*/

#define PS_COMBINERCOUNT_MASK   0x00000007
#define REG_INOUT_MASK          0x0000000F
#define REG_INMAP_MASK          0x000000E0
#define REG_OUTMAP_MASK         0x00000038
#define REG_INPUT_A_SHIFT       24
#define REG_INPUT_B_SHIFT       16
#define REG_INPUT_C_SHIFT       8
#define REG_INPUT_D_SHIFT       0
#define REG_OUTPUT_AB_SHIFT     4
#define REG_OUTPUT_CD_SHIFT     0
#define REG_OUTPUT_R_SHIFT      8

//#define REG_INMAP_MASK      0x0000000E
//#define REG_INCHANNEL_MASK  0x00000001
//#define REG_OUTMAP_MASK     0
//#define REG_INMAP_A_SHIFT   28
//#define REG_INMAP_B_SHIFT   20
//#define REG_INMAP_C_SHIFT   12
//#define REG_INMAP_D_SHIFT   4
//#define REG_OUTPUT_R_SHIFT  8
//#define REG_OUTPUT_AB_SHIFT 4
//#define REG_OUTPUT_CD_SHIFT 0

/*
#define REG_ZERO            0x0
#define REG_C0              0x1
#define REG_C1              0x2
#define REG_FOG             0x3
#define REG_V0              0x4
#define REG_V1              0x5
#define REG_T0              0x8
#define REG_T1              0x9
#define REG_T2              0xA
#define REG_T3              0xB
#define REG_R0              0xC
#define REG_R1              0xD
*/
/*
#define INMAP_ALPHA_CHANNEL     0x1
#define INMAP_UNSIGNED_IDENTITY 0x0
#define INMAP_UNSIGNED_INVERT   0x2
#define INMAP_EXPAND_NORMAL     0x4
#define INMAP_EXPAND_NEGATE     0x6
#define INMAP_HALFBIAS_NORMAL   0x8
#define INMAP_HALFBIAS_NEGATE   0xA
#define INMAP_SIGNED_IDENTITY   0xC
#define INMAP_SIGNED_NEGATE     0xE
*/

//******************************************************************************
D3DXVECTOR4* CPSRasterizer::GetCombinerRegister(UINT uStage, DWORD dwPSIOReg, DWORD dwShift) {

    switch ((dwPSIOReg >> dwShift) & REG_INOUT_MASK) {

        case PS_REGISTER_ZERO:
            return &m_reg.zero;
        case PS_REGISTER_C0:
            if ((m_d3dpsd.PSCombinerCount & (PS_COMBINERCOUNT_UNIQUE_C0 << 8)) || uStage == 8) {
                return m_reg.c0 + uStage;
            }
            else {
                return m_reg.c0;
            }
        case PS_REGISTER_C1:
            if ((m_d3dpsd.PSCombinerCount & (PS_COMBINERCOUNT_UNIQUE_C1 << 8)) || uStage == 8) {
                return m_reg.c1 + uStage;
            }
            else {
                return m_reg.c1;
            }
        case PS_REGISTER_FOG:
            return &m_reg.fog;
        case PS_REGISTER_V0:
            return &m_reg.v0;
        case PS_REGISTER_V1:
            return &m_reg.v1;
        case PS_REGISTER_T0:
            return &m_reg.t0;
        case PS_REGISTER_T1:
            return &m_reg.t1;
        case PS_REGISTER_T2:
            return &m_reg.t2;
        case PS_REGISTER_T3:
            return &m_reg.t3;
        case PS_REGISTER_R0:
            return &m_reg.r0;
        case PS_REGISTER_R1:
            return &m_reg.r1;
        case PS_REGISTER_V1R0_SUM:
            return &m_reg.sum;
        case PS_REGISTER_EF_PROD:
            return &m_reg.prod;
        default:
            __asm int 3;
            return &m_reg.zero;
    }
}

//******************************************************************************
void CPSRasterizer::SelectedChannelToColor(DWORD dwPSInReg, DWORD dwShift, D3DXVECTOR4* pvDst, D3DXVECTOR4* pvSrc) {

    if ((dwPSInReg >> dwShift) & PS_CHANNEL_ALPHA) {
        pvDst->x = pvSrc->w;
        pvDst->y = pvSrc->w;
        pvDst->z = pvSrc->w;
    }
    else {
        pvDst->x = pvSrc->x;
        pvDst->y = pvSrc->y;
        pvDst->z = pvSrc->z;
    }
}

//******************************************************************************
void CPSRasterizer::SelectedChannelToAlpha(DWORD dwPSInReg, DWORD dwShift, D3DXVECTOR4* pvDst, D3DXVECTOR4* pvSrc) {

    if ((dwPSInReg >> dwShift) & PS_CHANNEL_ALPHA) {
        pvDst->w = pvSrc->w;
    }
    else {
        pvDst->w = pvSrc->z;
    }
}

//******************************************************************************
void CPSRasterizer::MapInputRange(DWORD dwPSRGBInReg, DWORD dwPSAInReg, DWORD dwShift, D3DXVECTOR4* pvReg) {

    static float fHalf = 128.0f / 255.0f;
    static float fRegNegOne = -256.0f / 255.0f;

    switch ((dwPSRGBInReg >> dwShift) & REG_INMAP_MASK) {

        case PS_INPUTMAPPING_UNSIGNED_IDENTITY:
            if (pvReg->x > 1.0f) pvReg->x = 1.0f;
            else if (pvReg->x < 0.0f) pvReg->x = 0.0f;
            if (pvReg->y > 1.0f) pvReg->y = 1.0f;
            else if (pvReg->y < 0.0f) pvReg->y = 0.0f;
            if (pvReg->z > 1.0f) pvReg->z = 1.0f;
            else if (pvReg->z < 0.0f) pvReg->z = 0.0f;
            break;
        case PS_INPUTMAPPING_UNSIGNED_INVERT:
            if (pvReg->x > 1.0f) pvReg->x = 1.0f;
            else if (pvReg->x < 0.0f) pvReg->x = 0.0f;
            if (pvReg->y > 1.0f) pvReg->y = 1.0f;
            else if (pvReg->y < 0.0f) pvReg->y = 0.0f;
            if (pvReg->z > 1.0f) pvReg->z = 1.0f;
            else if (pvReg->z < 0.0f) pvReg->z = 0.0f;
            pvReg->x = 1.0f - pvReg->x;
            pvReg->y = 1.0f - pvReg->y;
            pvReg->z = 1.0f - pvReg->z;
            break;
        case PS_INPUTMAPPING_EXPAND_NORMAL:
            if (pvReg->x > 1.0f) pvReg->x = 1.0f;
            else if (pvReg->x < 0.0f) pvReg->x = 0.0f;
            if (pvReg->y > 1.0f) pvReg->y = 1.0f;
            else if (pvReg->y < 0.0f) pvReg->y = 0.0f;
            if (pvReg->z > 1.0f) pvReg->z = 1.0f;
            else if (pvReg->z < 0.0f) pvReg->z = 0.0f;
            pvReg->x = 2.0f * pvReg->x - 1.0f;
            pvReg->y = 2.0f * pvReg->y - 1.0f;
            pvReg->z = 2.0f * pvReg->z - 1.0f;
            break;
        case PS_INPUTMAPPING_EXPAND_NEGATE:
            if (pvReg->x > 1.0f) pvReg->x = 1.0f;
            else if (pvReg->x < 0.0f) pvReg->x = 0.0f;
            if (pvReg->y > 1.0f) pvReg->y = 1.0f;
            else if (pvReg->y < 0.0f) pvReg->y = 0.0f;
            if (pvReg->z > 1.0f) pvReg->z = 1.0f;
            else if (pvReg->z < 0.0f) pvReg->z = 0.0f;
            pvReg->x = 1.0f - 2.0f * pvReg->x;
            pvReg->y = 1.0f - 2.0f * pvReg->y;
            pvReg->z = 1.0f - 2.0f * pvReg->z;
            break;
        case PS_INPUTMAPPING_HALFBIAS_NORMAL:
            if (pvReg->x > 1.0f) pvReg->x = 1.0f;
            else if (pvReg->x < 0.0f) pvReg->x = 0.0f;
            if (pvReg->y > 1.0f) pvReg->y = 1.0f;
            else if (pvReg->y < 0.0f) pvReg->y = 0.0f;
            if (pvReg->z > 1.0f) pvReg->z = 1.0f;
            else if (pvReg->z < 0.0f) pvReg->z = 0.0f;
            pvReg->x -= fHalf;
            pvReg->y -= fHalf;
            pvReg->z -= fHalf;
            break;
        case PS_INPUTMAPPING_HALFBIAS_NEGATE:
            if (pvReg->x > 1.0f) pvReg->x = 1.0f;
            else if (pvReg->x < 0.0f) pvReg->x = 0.0f;
            if (pvReg->y > 1.0f) pvReg->y = 1.0f;
            else if (pvReg->y < 0.0f) pvReg->y = 0.0f;
            if (pvReg->z > 1.0f) pvReg->z = 1.0f;
            else if (pvReg->z < 0.0f) pvReg->z = 0.0f;
            pvReg->x = fHalf - pvReg->x;
            pvReg->y = fHalf - pvReg->y;
            pvReg->z = fHalf - pvReg->z;
            break;
        case PS_INPUTMAPPING_SIGNED_IDENTITY:
            if (pvReg->x > 1.0f) pvReg->x = 1.0f;
            else if (pvReg->x < fRegNegOne) pvReg->x = fRegNegOne;
            if (pvReg->y > 1.0f) pvReg->y = 1.0f;
            else if (pvReg->y < fRegNegOne) pvReg->y = fRegNegOne;
            if (pvReg->z > 1.0f) pvReg->z = 1.0f;
            else if (pvReg->z < fRegNegOne) pvReg->z = fRegNegOne;
            break;
        case PS_INPUTMAPPING_SIGNED_NEGATE:
            if (pvReg->x > 1.0f) pvReg->x = 1.0f;
            else if (pvReg->x < -1.0f) pvReg->x = -1.0f;
            if (pvReg->y > 1.0f) pvReg->y = 1.0f;
            else if (pvReg->y < -1.0f) pvReg->y = -1.0f;
            if (pvReg->z > 1.0f) pvReg->z = 1.0f;
            else if (pvReg->z < -1.0f) pvReg->z = -1.0f;
            pvReg->x = -pvReg->x;
            pvReg->y = -pvReg->y;
            pvReg->z = -pvReg->z;
            break;
    }

    switch ((dwPSAInReg >> dwShift) & REG_INMAP_MASK) {

        case PS_INPUTMAPPING_UNSIGNED_IDENTITY:
            if (pvReg->w > 1.0f) pvReg->w = 1.0f;
            else if (pvReg->w < 0.0f) pvReg->w = 0.0f;
            break;
        case PS_INPUTMAPPING_UNSIGNED_INVERT:
            if (pvReg->w > 1.0f) pvReg->w = 1.0f;
            else if (pvReg->w < 0.0f) pvReg->w = 0.0f;
            pvReg->w = 1.0f - pvReg->w;
            break;
        case PS_INPUTMAPPING_EXPAND_NORMAL:
            if (pvReg->w > 1.0f) pvReg->w = 1.0f;
            else if (pvReg->w < 0.0f) pvReg->w = 0.0f;
            pvReg->w = 2.0f * pvReg->w - 1.0f;
            break;
        case PS_INPUTMAPPING_EXPAND_NEGATE:
            if (pvReg->w > 1.0f) pvReg->w = 1.0f;
            else if (pvReg->w < 0.0f) pvReg->w = 0.0f;
            pvReg->w = 1.0f - 2.0f * pvReg->w;
            break;
        case PS_INPUTMAPPING_HALFBIAS_NORMAL:
            if (pvReg->w > 1.0f) pvReg->w = 1.0f;
            else if (pvReg->w < 0.0f) pvReg->w = 0.0f;
            pvReg->w -= fHalf;
            break;
        case PS_INPUTMAPPING_HALFBIAS_NEGATE:
            if (pvReg->w > 1.0f) pvReg->w = 1.0f;
            else if (pvReg->w < 0.0f) pvReg->w = 0.0f;
            pvReg->w = fHalf - pvReg->w;
            break;
        case PS_INPUTMAPPING_SIGNED_IDENTITY:
            if (pvReg->w > 1.0f) pvReg->w = 1.0f;
            else if (pvReg->w < fRegNegOne) pvReg->w = fRegNegOne;
            break;
        case PS_INPUTMAPPING_SIGNED_NEGATE:
            if (pvReg->w > 1.0f) pvReg->w = 1.0f;
            else if (pvReg->w < -1.0f) pvReg->w = -1.0f;
            pvReg->w = -pvReg->w;
            break;
    }
}

//******************************************************************************
void CPSRasterizer::MapOutputRange(DWORD dwPSRGBOutReg, DWORD dwPSAOutReg, D3DXVECTOR4* pvReg) {

    static float fNegHalf = -128.0f / 255.0f;
    static float fRegNegOne = -256.0f / 255.0f;

    switch ((dwPSRGBOutReg >> 12) & REG_OUTMAP_MASK) {

        case PS_COMBINEROUTPUT_BIAS:
            pvReg->x += fNegHalf;
            pvReg->y += fNegHalf;
            pvReg->z += fNegHalf;
            break;
        case PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS:
            pvReg->x += fNegHalf;
            pvReg->y += fNegHalf;
            pvReg->z += fNegHalf;
            // Fall through
        case PS_COMBINEROUTPUT_SHIFTLEFT_1:
            pvReg->x *= 2.0f;
            pvReg->y *= 2.0f;
            pvReg->z *= 2.0f;
            break;
        case PS_COMBINEROUTPUT_SHIFTLEFT_2:
            pvReg->x *= 4.0f;
            pvReg->y *= 4.0f;
            pvReg->z *= 4.0f;
            break;
        case PS_COMBINEROUTPUT_SHIFTRIGHT_1:
            pvReg->x /= 2.0f;
            pvReg->y /= 2.0f;
            pvReg->z /= 2.0f;
            break;
    }

    switch ((dwPSAOutReg >> 12) & REG_OUTMAP_MASK) {

        case PS_COMBINEROUTPUT_BIAS:
            pvReg->w += fNegHalf;
            break;
        case PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS:
            pvReg->w += fNegHalf;
            // Fall through
        case PS_COMBINEROUTPUT_SHIFTLEFT_1:
            pvReg->w *= 2.0f;
            break;
        case PS_COMBINEROUTPUT_SHIFTLEFT_2:
            pvReg->w *= 4.0f;
            break;
        case PS_COMBINEROUTPUT_SHIFTRIGHT_1:
            pvReg->w /= 2.0f;
            break;
    }

    // Clamp outputs
    if (pvReg->x > 1.0f) pvReg->x = 1.0f;
    else if (pvReg->x < fRegNegOne) pvReg->x = fRegNegOne;
    if (pvReg->y > 1.0f) pvReg->y = 1.0f;
    else if (pvReg->y < fRegNegOne) pvReg->y = fRegNegOne;
    if (pvReg->z > 1.0f) pvReg->z = 1.0f;
    else if (pvReg->z < fRegNegOne) pvReg->z = fRegNegOne;
    if (pvReg->w > 1.0f) pvReg->w = 1.0f;
    else if (pvReg->w < fRegNegOne) pvReg->w = fRegNegOne;
}

//******************************************************************************
D3DCOLOR CPSRasterizer::CombinePixel(PSVERTEX* prIPixel) {

    UINT uNumStages;
    BOOL bMuxOnMSB;
//    BOOL bPerStageC0;
//    BOOL bPerStageC1;
    D3DXVECTOR4 A, B, C, D, E, F, G, AB, CD, R;
    D3DXVECTOR4 *pA, *pB, *pC, *pD, *pAB, *pCD, *pR, *pE, *pF, *pG;
    UINT i;


    uNumStages = m_d3dpsd.PSCombinerCount & PS_COMBINERCOUNT_MASK;
    bMuxOnMSB = m_d3dpsd.PSCombinerCount & (PS_COMBINERCOUNT_MUX_MSB << 8);
//    bPerStageC0 = m_d3dpsd.PSCombinerCount & PS_CC_EACH_C0_UNIQUE;
//    bPerStageC1 = m_d3dpsd.PSCombinerCount & PS_CC_EACH_C1_UNIQUE;

    // Stage combiners
    for (i = 0; i < uNumStages; i++) {

        // Select the color input registers
        pA = GetCombinerRegister(i, m_d3dpsd.PSRGBInputs[i], REG_INPUT_A_SHIFT);
        pB = GetCombinerRegister(i, m_d3dpsd.PSRGBInputs[i], REG_INPUT_B_SHIFT);
        pC = GetCombinerRegister(i, m_d3dpsd.PSRGBInputs[i], REG_INPUT_C_SHIFT);
        pD = GetCombinerRegister(i, m_d3dpsd.PSRGBInputs[i], REG_INPUT_D_SHIFT);

        // Copy the selected channels from the color input registers to A, B, C, and D
        SelectedChannelToColor(m_d3dpsd.PSRGBInputs[i], REG_INPUT_A_SHIFT, &A, pA);
        SelectedChannelToColor(m_d3dpsd.PSRGBInputs[i], REG_INPUT_B_SHIFT, &B, pB);
        SelectedChannelToColor(m_d3dpsd.PSRGBInputs[i], REG_INPUT_C_SHIFT, &C, pC);
        SelectedChannelToColor(m_d3dpsd.PSRGBInputs[i], REG_INPUT_D_SHIFT, &D, pD);

        // Select the alpha input registers
        pA = GetCombinerRegister(i, m_d3dpsd.PSAlphaInputs[i], REG_INPUT_A_SHIFT);
        pB = GetCombinerRegister(i, m_d3dpsd.PSAlphaInputs[i], REG_INPUT_B_SHIFT);
        pC = GetCombinerRegister(i, m_d3dpsd.PSAlphaInputs[i], REG_INPUT_C_SHIFT);
        pD = GetCombinerRegister(i, m_d3dpsd.PSAlphaInputs[i], REG_INPUT_D_SHIFT);

        // Copy the selected channels from the alpha input registers to A, B, C, and D
        SelectedChannelToAlpha(m_d3dpsd.PSAlphaInputs[i], REG_INPUT_A_SHIFT, &A, pA);
        SelectedChannelToAlpha(m_d3dpsd.PSAlphaInputs[i], REG_INPUT_B_SHIFT, &B, pB);
        SelectedChannelToAlpha(m_d3dpsd.PSAlphaInputs[i], REG_INPUT_C_SHIFT, &C, pC);
        SelectedChannelToAlpha(m_d3dpsd.PSAlphaInputs[i], REG_INPUT_D_SHIFT, &D, pD);

        // Map the inputs into their selected color and alpha ranges
        MapInputRange(m_d3dpsd.PSRGBInputs[i], m_d3dpsd.PSAlphaInputs[i], REG_INPUT_A_SHIFT, &A);
        MapInputRange(m_d3dpsd.PSRGBInputs[i], m_d3dpsd.PSAlphaInputs[i], REG_INPUT_B_SHIFT, &B);
        MapInputRange(m_d3dpsd.PSRGBInputs[i], m_d3dpsd.PSAlphaInputs[i], REG_INPUT_C_SHIFT, &C);
        MapInputRange(m_d3dpsd.PSRGBInputs[i], m_d3dpsd.PSAlphaInputs[i], REG_INPUT_D_SHIFT, &D);

        // Multiply the A and B registers together
        AB.x = A.x * B.x;
        AB.y = A.y * B.y;
        AB.z = A.z * B.z;
        AB.w = A.w * B.w;

        // Multiply the C and D registers together
        CD.x = C.x * D.x;
        CD.y = C.y * D.y;
        CD.z = C.z * D.z;
        CD.w = C.w * D.w;

        // Mux or sum the AB and CD color results
        if ((m_d3dpsd.PSRGBOutputs[i] >> 12) & PS_COMBINEROUTPUT_AB_CD_MUX) {

            BOOL bUseCD;

            if (bMuxOnMSB) {
                bUseCD = m_reg.r0.w >= 0.5f;
            }
            else {
                USHORT mux;
                if (m_reg.r0.w < 0.0f) {
                    mux = (USHORT)((1.0f + m_reg.r0.w) * 255.0f + 1.0f) | 0x100;
                }
                else {
                    mux = (USHORT)(m_reg.r0.w * 255.0f);
                }
                bUseCD = mux & 0x1;
            }

            if (bUseCD) {
                R.x = CD.x;
                R.y = CD.y;
                R.z = CD.z;
            }
            else {
                R.x = AB.x;
                R.y = AB.y;
                R.z = AB.z;
            }
        }
        else {
            R.x = AB.x + CD.x;
            R.y = AB.y + CD.y;
            R.z = AB.z + CD.z;
        }

        // Mux or sum the AB and CD alpha results
        if ((m_d3dpsd.PSAlphaOutputs[i] >> 12) & PS_COMBINEROUTPUT_AB_CD_MUX) {

            BOOL bUseCD;

            if (bMuxOnMSB) {
                bUseCD = m_reg.r0.w >= 0.5f;
            }
            else {
                USHORT mux;
                if (m_reg.r0.w < 0.0f) {
                    mux = (USHORT)((1.0f + m_reg.r0.w) * 255.0f + 1.0f) | 0x100;
                }
                else {
                    mux = (USHORT)(m_reg.r0.w * 255.0f);
                }
                bUseCD = mux & 0x1;
            }

            if (bUseCD) {
                R.w = CD.w;
            }
            else {
                R.w = AB.w;
            }
        }
        else {
            R.w = AB.w + CD.w;
        }

        // Propagate AB dot product if requested
        if ((m_d3dpsd.PSRGBOutputs[i] >> 12) & PS_COMBINEROUTPUT_AB_DOT_PRODUCT) {
            AB.x = AB.y = AB.z = AB.x + AB.y + AB.z;
        }

        // Propagate CD dot product if requested
        if ((m_d3dpsd.PSRGBOutputs[i] >> 12) & PS_COMBINEROUTPUT_CD_DOT_PRODUCT) {
            CD.x = CD.y = CD.z = CD.x + CD.y + CD.z;
        }

        // Map the outputs into their selected color and alpha ranges
        MapOutputRange(m_d3dpsd.PSRGBOutputs[i], m_d3dpsd.PSAlphaOutputs[i], &AB);
        MapOutputRange(m_d3dpsd.PSRGBOutputs[i], m_d3dpsd.PSAlphaOutputs[i], &CD);
        MapOutputRange(m_d3dpsd.PSRGBOutputs[i], m_d3dpsd.PSAlphaOutputs[i], &R);

        // Select the color output registers
        pAB = GetCombinerRegister(i, m_d3dpsd.PSRGBOutputs[i], REG_OUTPUT_AB_SHIFT);
        pCD = GetCombinerRegister(i, m_d3dpsd.PSRGBOutputs[i], REG_OUTPUT_CD_SHIFT);
        pR  = GetCombinerRegister(i, m_d3dpsd.PSRGBOutputs[i], REG_OUTPUT_R_SHIFT);

        // Store the color outputs
        if (pAB != &m_reg.zero) {
            pAB->x = AB.x;
            pAB->y = AB.y;
            pAB->z = AB.z;

            // Copy the blue channel to the alpha channel of the AB register if requested
            if ((m_d3dpsd.PSRGBOutputs[i] >> 12) & PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA) {
                pAB->w = AB.z;
            }
        }
        if (pCD != &m_reg.zero) {
            pCD->x = CD.x;
            pCD->y = CD.y;
            pCD->z = CD.z;

            // Copy the blue channel to the alpha channel of the CD register if requested
            if ((m_d3dpsd.PSRGBOutputs[i] >> 12) & PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA) {
                pCD->w = CD.z;
            }
        }
        if (pR != &m_reg.zero) {
            pR->x = R.x;
            pR->y = R.y;
            pR->z = R.z;
        }

        // Select the alpha output registers
        pAB = GetCombinerRegister(i, m_d3dpsd.PSAlphaOutputs[i], REG_OUTPUT_AB_SHIFT);
        pCD = GetCombinerRegister(i, m_d3dpsd.PSAlphaOutputs[i], REG_OUTPUT_CD_SHIFT);
        pR  = GetCombinerRegister(i, m_d3dpsd.PSAlphaOutputs[i], REG_OUTPUT_R_SHIFT);

        // Store the alpha outputs
        if (pAB != &m_reg.zero) {
            pAB->w = AB.w;
        }
        if (pCD != &m_reg.zero) {
            pCD->w = CD.w;
        }
        if (pR != &m_reg.zero) {
            pR->w = R.w;
        }
    }

    // Initialize the sum register
    E = m_reg.r0;
    F = m_reg.v1;
    ClampZeroToOne(&E);
    ClampZeroToOne(&F);
    if ((m_d3dpsd.PSFinalCombinerInputsEFG & REG_INMAP_MASK) & PS_FINALCOMBINERSETTING_COMPLEMENT_R0) {
        E.x = 1.0f - E.x;
        E.y = 1.0f - E.y;
        E.z = 1.0f - E.z;
        E.w = 1.0f - E.w;
    }
    if ((m_d3dpsd.PSFinalCombinerInputsEFG & REG_INMAP_MASK) & PS_FINALCOMBINERSETTING_COMPLEMENT_V1) {
        F.x = 1.0f - F.x;
        F.y = 1.0f - F.y;
        F.z = 1.0f - F.z;
        F.w = 1.0f - F.w;
    }
    m_reg.sum = E + F;

    // Initialize the product register
    pE = GetCombinerRegister(8, m_d3dpsd.PSFinalCombinerInputsEFG, REG_INPUT_A_SHIFT);
    pF = GetCombinerRegister(8, m_d3dpsd.PSFinalCombinerInputsEFG, REG_INPUT_B_SHIFT);

    // Copy the selected channels from the color input registers to E and F
    SelectedChannelToColor(m_d3dpsd.PSFinalCombinerInputsEFG, REG_INPUT_A_SHIFT, &E, pE);
    SelectedChannelToColor(m_d3dpsd.PSFinalCombinerInputsEFG, REG_INPUT_B_SHIFT, &F, pF);

    // Map the inputs into their selected color ranges
    MapInputRange(m_d3dpsd.PSFinalCombinerInputsEFG, m_d3dpsd.PSFinalCombinerInputsEFG, REG_INPUT_A_SHIFT, &E);
    MapInputRange(m_d3dpsd.PSFinalCombinerInputsEFG, m_d3dpsd.PSFinalCombinerInputsEFG, REG_INPUT_B_SHIFT, &F);

    m_reg.prod.x = E.x * F.x;
    m_reg.prod.y = E.y * F.y;
    m_reg.prod.z = E.z * F.z;
    m_reg.prod.w = E.w * F.w;

    // Initialize the final combiner color input registers
    pA = GetCombinerRegister(8, m_d3dpsd.PSFinalCombinerInputsABCD, REG_INPUT_A_SHIFT);
    pB = GetCombinerRegister(8, m_d3dpsd.PSFinalCombinerInputsABCD, REG_INPUT_B_SHIFT);
    pC = GetCombinerRegister(8, m_d3dpsd.PSFinalCombinerInputsABCD, REG_INPUT_C_SHIFT);
    pD = GetCombinerRegister(8, m_d3dpsd.PSFinalCombinerInputsABCD, REG_INPUT_D_SHIFT);

    // Copy the selected channels from the color input registers to A, B, C, D
    SelectedChannelToColor(m_d3dpsd.PSFinalCombinerInputsABCD, REG_INPUT_A_SHIFT, &A, pA);
    SelectedChannelToColor(m_d3dpsd.PSFinalCombinerInputsABCD, REG_INPUT_B_SHIFT, &B, pB);
    SelectedChannelToColor(m_d3dpsd.PSFinalCombinerInputsABCD, REG_INPUT_C_SHIFT, &C, pC);
    SelectedChannelToColor(m_d3dpsd.PSFinalCombinerInputsABCD, REG_INPUT_D_SHIFT, &D, pD);

    // Map the inputs into their selected color ranges
    MapInputRange(m_d3dpsd.PSFinalCombinerInputsABCD, m_d3dpsd.PSFinalCombinerInputsABCD, REG_INPUT_A_SHIFT, &A);
    MapInputRange(m_d3dpsd.PSFinalCombinerInputsABCD, m_d3dpsd.PSFinalCombinerInputsABCD, REG_INPUT_B_SHIFT, &B);
    MapInputRange(m_d3dpsd.PSFinalCombinerInputsABCD, m_d3dpsd.PSFinalCombinerInputsABCD, REG_INPUT_C_SHIFT, &C);
    MapInputRange(m_d3dpsd.PSFinalCombinerInputsABCD, m_d3dpsd.PSFinalCombinerInputsABCD, REG_INPUT_D_SHIFT, &D);

    // Final color combiner
    E.x = A.x * B.x;
    E.y = A.y * B.y;
    E.z = A.z * B.z;
    E.w = A.w * B.w;

    F.x = (1.0f - A.x) * C.x;
    F.y = (1.0f - A.y) * C.y;
    F.z = (1.0f - A.z) * C.z;
    F.w = (1.0f - A.w) * C.w;

    R.x = E.x + F.x + D.x;
    R.y = E.y + F.y + D.y;
    R.z = E.z + F.z + D.z;

    // Initialize the final combiner alpha input
    pG = GetCombinerRegister(8, m_d3dpsd.PSFinalCombinerInputsEFG, REG_INPUT_C_SHIFT);

    // Copy the selected channel from the alpha input register to G
    SelectedChannelToAlpha(m_d3dpsd.PSFinalCombinerInputsEFG, REG_INPUT_C_SHIFT, &G, pG);

    // Map the input into the selected alpha range
    MapInputRange(m_d3dpsd.PSFinalCombinerInputsEFG, m_d3dpsd.PSFinalCombinerInputsEFG, REG_INPUT_C_SHIFT, &G);

    // Final alpha combiner
    R.w = G.w;

    ClampZeroToOne(&R);

    return D3DCOLOR_COLORVALUE(R.x, R.y, R.z, R.w);
}

//******************************************************************************
void CPSRasterizer::ShadePixel(D3DCOLOR* pcPixel, PSVERTEX* prIPixel) {

    D3DCOLOR        cPixel, cSrcBlend, cDstBlend;
    BOOL            bAlphaEnabled;
    D3DBLENDOP      blendop;
    UINT            i;

// QUESTION: How do constant values set by SetPixelShaderConstant get mapped
// to the constant settings in D3DPIXELSHADERDEF?
    D3DXVECTOR4 t0;
    D3DXVECTOR4 t1;
    D3DXVECTOR4 t2;
    D3DXVECTOR4 t3;
    D3DXVECTOR4 r0;
    D3DXVECTOR4 r1;
    D3DXVECTOR4 c0[8];
    D3DXVECTOR4 c1[8];
    D3DXVECTOR4 fog;
    D3DXVECTOR4 zero;

    // Clear all registers (and initialize the zero register)
    memset(&m_reg, 0, sizeof(PSREGISTERS));

    // Initialize the color registers
    ColorToVector(&m_reg.v0, prIPixel->cDiffuse);
    ColorToVector(&m_reg.v1, prIPixel->cSpecular);

    // Initialize the texture registers
    m_reg.t3 = m_reg.t2 = m_reg.t1 = m_reg.t0 = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);

    // Initialize the constant registers
    for (i = 0; i < 8; i++) {
        ColorToVector(&m_reg.c0[i], m_d3dpsd.PSConstant0[i]);
        ColorToVector(&m_reg.c1[i], m_d3dpsd.PSConstant1[i]);
    }
    ColorToVector(&m_reg.c0[8], m_d3dpsd.PSFinalCombinerConstant0);
    ColorToVector(&m_reg.c1[8], m_d3dpsd.PSFinalCombinerConstant1);

    // Texture addressing
    TexturePixel(prIPixel);

    // Copy the alpha component of t0 into r0 to be the initial conditional for the AB/CD mux
    m_reg.r0.w = m_reg.t0.w;

    // Combiners
    cPixel = CombinePixel(prIPixel);

//    cPixel = prIPixel->cDiffuse & 0x00FFFFFF;
//    cPixel = SampleTexture(0, &prIPixel->pvTCoord[0], FALSE);

    // Final blend of source and destination pixels
    m_pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, (LPDWORD)&bAlphaEnabled);

    if (bAlphaEnabled) {

        float fMagSqSrc, fMagSqDst;

        cSrcBlend = GetBlendColor(TRUE, *pcPixel, cPixel);
        cDstBlend = GetBlendColor(FALSE, *pcPixel, cPixel);

        cSrcBlend = MultiplyColors(cPixel, cSrcBlend);
        cDstBlend = MultiplyColors(*pcPixel, cDstBlend);

        m_pDevice->GetRenderState(D3DRS_BLENDOP, (LPDWORD)&blendop);

        switch (blendop) {
            case D3DBLENDOP_ADD:
                *pcPixel = AddColors(cSrcBlend, cDstBlend);
                break;
            case D3DBLENDOP_SUBTRACT:
                *pcPixel = SubtractColors(cSrcBlend, cDstBlend);
                break;
            case D3DBLENDOP_REVSUBTRACT:
                *pcPixel = SubtractColors(cDstBlend, cSrcBlend);
                break;
            case D3DBLENDOP_MIN:
                fMagSqSrc = D3DXVec4LengthSq(&D3DXVECTOR4((float)RGBA_GETRED(cSrcBlend), (float)RGBA_GETGREEN(cSrcBlend), (float)RGBA_GETBLUE(cSrcBlend), (float)RGBA_GETALPHA(cSrcBlend)));
                fMagSqDst = D3DXVec4LengthSq(&D3DXVECTOR4((float)RGBA_GETRED(cDstBlend), (float)RGBA_GETGREEN(cDstBlend), (float)RGBA_GETBLUE(cDstBlend), (float)RGBA_GETALPHA(cDstBlend)));
                if (fMagSqSrc < fMagSqDst) {
                    *pcPixel = cPixel;
                }
                break;
            case D3DBLENDOP_MAX:
                fMagSqSrc = D3DXVec4LengthSq(&D3DXVECTOR4((float)RGBA_GETRED(cSrcBlend), (float)RGBA_GETGREEN(cSrcBlend), (float)RGBA_GETBLUE(cSrcBlend), (float)RGBA_GETALPHA(cSrcBlend)));
                fMagSqDst = D3DXVec4LengthSq(&D3DXVECTOR4((float)RGBA_GETRED(cDstBlend), (float)RGBA_GETGREEN(cDstBlend), (float)RGBA_GETBLUE(cDstBlend), (float)RGBA_GETALPHA(cDstBlend)));
                if (fMagSqSrc > fMagSqDst) {
                    *pcPixel = cPixel;
                }
                break;
        }
    }

    else {
        *pcPixel = cPixel;
    }

*pcPixel &= 0x00FFFFFF;
}

/*
When Direct3D renders a primitive, it generates a color for the primitive based on the primitive's material 
(or the colors of its vertices) and lighting information. For details, see Lighting and Materials. If an 
application enables texture blending, Direct3D must then blend the texel colors of one or more textures with 
the primitive's current colors. Direct3D uses the following formula to determine the final color for each 
pixel in the primitive's image.

FinalColor = TexelColor * SourceBlendFactor + PixelColor * DestBlendFactor (the + is now D3DBLENDOP)
 
In the preceding formula, FinalColor is the pixel color that is output to the target rendering surface. 
TexelColor stands for the color of the texel that corresponds to the current pixel. For details on how 
Direct3D maps pixels to texels, see Texture Filtering. SourceBlendFactor is a calculated value that Direct3D 
uses to determine the percentage of the texel color to apply to the final color. PixelColor is the color of 
the current pixel in the primitive's image. DestBlendFactor represents the percentage of the current pixel's 
color that will be used in the final color. The values of SourceBlendFactor and DestBlendFactor range from 0.0
 or 1.0 inclusive.

As you can see from the preceding formula, a texture is not rendered as transparent at all if the SourceBlendFactor 
is 1.0 and the DestBlendFactor is 0.0. It is completely transparent if the SourceBlendFactor is 0.0 and the 
DestBlendFactor is 1.0. If an application sets these factors to any other values, the resulting texture will be 
blended with some degree of transparency.

Every texel in a texture has a red, a green, and a blue color value. By default, Direct3D uses the alpha values of 
texels as the SourceBlendFactor. Therefore, applications can control the transparency of textures by setting the 
alpha values in their textures.

*/

//******************************************************************************
BOOL CPSRasterizer::ComparePixels(D3DCOLOR cSrc, D3DCOLOR cRef) {

    float       fMagSq;
    D3DXVECTOR4 vDiff = D3DXVECTOR4((float)((int)RGBA_GETRED(cRef) - (int)RGBA_GETRED(cSrc)),
                                    (float)((int)RGBA_GETGREEN(cRef) - (int)RGBA_GETGREEN(cSrc)),
                                    (float)((int)RGBA_GETBLUE(cRef) - (int)RGBA_GETBLUE(cSrc)),
                                    (float)((int)RGBA_GETALPHA(cRef) - (int)RGBA_GETALPHA(cSrc)));

    fMagSq = D3DXVec4LengthSq(&vDiff);
    if ((1.0f - fMagSq / m_fMaxMagSq) < m_fThreshold) {
        Log(LOG_FAIL, TEXT("Pixel differs from reference value by a %3.3f variance (Src: 0x%X, Ref: 0x%X)"), fMagSq / m_fMaxMagSq, cSrc, cRef);
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
D3DCOLOR CPSRasterizer::SampleTexture(DWORD dwStage, D3DXVECTOR4* pvTCoord, BOOL bCube, DWORD dwDotMap) {

    CBaseTexture8* pd3dtb;
    D3DRESOURCETYPE d3drt;
    D3DCOLOR cTexel;
    CSurface8* pd3ds = NULL;
    LERPPROC pfnLerp;
    float u, v, s;
    UINT x, y, z;
    HRESULT hr;

    m_pDevice->GetTexture(dwStage, &pd3dtb);

    d3drt = pd3dtb->GetType();

    if (bCube && d3drt != D3DRTYPE_CUBETEXTURE) {
        __asm int 3;
    }

    switch (dwDotMap) {
        case PS_DOTMAPPING_ZERO_TO_ONE:
            pfnLerp = LerpColorsZeroToOne;
            break;
        case PS_DOTMAPPING_MINUS1_TO_1_D3D:
            pfnLerp = LerpColorsNegOneToOneMS;
            break;
        case PS_DOTMAPPING_MINUS1_TO_1_GL:
            pfnLerp = LerpColorsNegOneToOneGL;
            break;
        case PS_DOTMAPPING_MINUS1_TO_1:
            pfnLerp = LerpColorsNegOneToOneNV;
            break;
        case PS_DOTMAPPING_HILO_1:
            pfnLerp = LerpColorsHiloOne;
            break;
        case PS_DOTMAPPING_HILO_HEMISPHERE_D3D:
            pfnLerp = LerpColorsHiloHemisphereMS;
            break;
        case PS_DOTMAPPING_HILO_HEMISPHERE_GL:
            pfnLerp = LerpColorsHiloHemisphereGL;
            break;
        case PS_DOTMAPPING_HILO_HEMISPHERE:
            pfnLerp = LerpColorsHiloHemisphereNV;
            break;
        default:
            __asm int 3;
    }

    switch (d3drt) {

        case D3DRTYPE_TEXTURE:

            if (FABS(pvTCoord->w) < 0.000001f) {
                DebugString(TEXT("Attempt to do a projected divide by zero on w"));
                __asm int 3;
                return 0;
            }
            u = pvTCoord->x / pvTCoord->w;
            v = pvTCoord->y / pvTCoord->w;

            hr = ((CTexture8*)pd3dtb)->GetSurfaceLevel(0, &pd3ds);
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                pd3dtb->Release();
                return 0;
            }

            break;

        case D3DRTYPE_CUBETEXTURE: {

            D3DCUBEMAP_FACES d3dcf;
            u = FABS(pvTCoord->x);
            v = FABS(pvTCoord->y);
            s = FABS(pvTCoord->z);
            if (u > v && u > s) {
                if (pvTCoord->x < 0.0f) {
                    u = -pvTCoord->z / pvTCoord->x;
                    v = -pvTCoord->y / pvTCoord->x;
                    d3dcf = D3DCUBEMAP_FACE_NEGATIVE_X;
                }
                else {
                    u = -pvTCoord->z / pvTCoord->x;
                    v = pvTCoord->y / pvTCoord->x;
                    d3dcf = D3DCUBEMAP_FACE_POSITIVE_X;
                }
            }
            else if (v > s) {
                if (pvTCoord->y < 0.0f) {
                    u = -pvTCoord->x / pvTCoord->y;
                    v = -pvTCoord->z / pvTCoord->y;
                    d3dcf = D3DCUBEMAP_FACE_NEGATIVE_Y;
                }
                else {
                    u = pvTCoord->x / pvTCoord->y;
                    v = -pvTCoord->z / pvTCoord->y;
                    d3dcf = D3DCUBEMAP_FACE_POSITIVE_Y;
                }
            }
            else {
                if (pvTCoord->z < 0.0f) {
                    u = pvTCoord->x / pvTCoord->z;
                    v = -pvTCoord->y / pvTCoord->z;
                    d3dcf = D3DCUBEMAP_FACE_NEGATIVE_Z;
                }
                else {
                    u = pvTCoord->x / pvTCoord->z;
                    v = pvTCoord->y / pvTCoord->z;
                    d3dcf = D3DCUBEMAP_FACE_POSITIVE_Z;
                }
            }

            hr = ((CCubeTexture8*)pd3dtb)->GetCubeMapSurface(d3dcf, 0, &pd3ds);
            if (ResultFailed(hr, TEXT("IDirect3DCubeTexture8::GetCubeMapSurface"))) {
                pd3dtb->Release();
                return 0;
            }

            if (!bCube) {
                if (FABS(pvTCoord->w) < 0.000001f) {
                    DebugString(TEXT("Attempt to do a projected divide by zero on w"));
                    __asm int 3;
                    return 0;
                }
                u = pvTCoord->x / pvTCoord->w;
                v = pvTCoord->y / pvTCoord->w;
                s = pvTCoord->z / pvTCoord->w;
            }

            break;
        }

        case D3DRTYPE_VOLUMETEXTURE:

            if (FABS(pvTCoord->w) < 0.000001f) {
                DebugString(TEXT("Attempt to do a projected divide by zero on w"));
                __asm int 3;
                return 0;
            }
            u = pvTCoord->x / pvTCoord->w;
            v = pvTCoord->y / pvTCoord->w;
            s = pvTCoord->z / pvTCoord->w;
            break;

        default:

            Log(LOG_FAIL, TEXT("Incorrect resource type %d reported for base texture"), d3drt);
            pd3dtb->Release();
            return 0;
    }

//    u -= (float)(int)(u < 0.0f ? u - 1.0f : u);
//    v -= (float)(int)(v < 0.0f ? v - 1.0f : v);
//    s -= (float)(int)(s < 0.0f ? s - 1.0f : s);

    u -= (float)(int)(u);
    v -= (float)(int)(v);
    s -= (float)(int)(s);

    if (u < 0.0f) u += 1.0f;
    if (v < 0.0f) v += 1.0f;
    if (s < 0.0f) s += 1.0f;

    if (pd3ds) {

        D3DLOCKED_RECT d3dlr;
        D3DSURFACE_DESC d3dsd;
        D3DCOLOR cSample[6];
        float dx, dy;
        UINT tx[2], ty[2];

        pd3ds->GetDesc(&d3dsd);        

        if (!XGIsSwizzledFormat(d3dsd.Format)) {
            DebugString(TEXT("Linear formats unsupported"));
            return 0;
        }

        dx = u * (float)d3dsd.Width;
        dy = v * (float)d3dsd.Height;

        tx[0] = (UINT)dx;
        ty[0] = (UINT)dy;

        dx = (float)fmod(dx, 1.0);
        dy = (float)fmod(dy, 1.0);

        tx[1] = (tx[0] + d3dsd.Width + ((dx < 0.5f) ? -1 : 1)) % d3dsd.Width;
        ty[1] = (ty[0] + d3dsd.Height + ((dy < 0.5f) ? -1 : 1)) % d3dsd.Height;

        dx = 1.0f - FABS(dx - 0.5f);
        dy = 1.0f - FABS(dy - 0.5f);

        hr = pd3ds->LockRect(&d3dlr, NULL, D3DLOCK_READONLY);
        if (ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"))) {
            pd3ds->Release();
            pd3dtb->Release();
            return 0;
        }

        Swizzler swz(d3dsd.Width, d3dsd.Height, 1);

#if 0
        cSample[0] = *((LPDWORD)((LPBYTE)d3dlr.pBits + ty[0] * d3dlr.Pitch) + tx[0]);
        cSample[1] = *((LPDWORD)((LPBYTE)d3dlr.pBits + ty[0] * d3dlr.Pitch) + tx[1]);
        cSample[2] = *((LPDWORD)((LPBYTE)d3dlr.pBits + ty[1] * d3dlr.Pitch) + tx[0]);
        cSample[3] = *((LPDWORD)((LPBYTE)d3dlr.pBits + ty[1] * d3dlr.Pitch) + tx[1]);
#else
        cSample[0] = *((LPDWORD)d3dlr.pBits + (swz.SwizzleU(tx[0]) | swz.SwizzleV(ty[0])));
        cSample[1] = *((LPDWORD)d3dlr.pBits + (swz.SwizzleU(tx[1]) | swz.SwizzleV(ty[0])));
        cSample[2] = *((LPDWORD)d3dlr.pBits + (swz.SwizzleU(tx[0]) | swz.SwizzleV(ty[1])));
        cSample[3] = *((LPDWORD)d3dlr.pBits + (swz.SwizzleU(tx[1]) | swz.SwizzleV(ty[1])));
#endif
        cSample[4] = pfnLerp(cSample[0], cSample[1], dx);
//        cSample[4] = AddColors(MultiplyColorScalar(cSample[0], dx), MultiplyColorScalar(cSample[1], 1.0f - dx));
        cSample[5] = pfnLerp(cSample[2], cSample[3], dx);
//        cSample[5] = AddColors(MultiplyColorScalar(cSample[2], dx), MultiplyColorScalar(cSample[3], 1.0f - dx));

        cTexel = pfnLerp(cSample[4], cSample[5], dy);
//        cTexel = AddColors(MultiplyColorScalar(cSample[4], dy), MultiplyColorScalar(cSample[5], 1.0f - dy));

        pd3ds->UnlockRect();
        pd3ds->Release();
    }
    else {

        CVolumeTexture8* pd3dtv;
        D3DLOCKED_BOX d3dlb;
        D3DVOLUME_DESC d3dvd;
        D3DCOLOR cSample[14];
        float dx, dy, dz;
        UINT tx[2], ty[2], tz[2];

        pd3dtv = (CVolumeTexture8*)pd3dtb;

        pd3dtv->GetLevelDesc(0, &d3dvd);        

        if (!XGIsSwizzledFormat(d3dvd.Format)) {
            DebugString(TEXT("Linear formats unsupported"));
            return 0;
        }

        dx = u * (float)d3dvd.Width;
        dy = v * (float)d3dvd.Height;
        dz = s * (float)d3dvd.Depth;

        tx[0] = (UINT)dx;
        ty[0] = (UINT)dy;
        tz[0] = (UINT)dz;

        dx = (float)fmod(dx, 1.0);
        dy = (float)fmod(dy, 1.0);
        dz = (float)fmod(dz, 1.0);

        tx[1] = (tx[0] + d3dvd.Width + ((dx < 0.5f) ? -1 : 1)) % d3dvd.Width;
        ty[1] = (ty[0] + d3dvd.Height + ((dy < 0.5f) ? -1 : 1)) % d3dvd.Height;
        tz[1] = (tz[0] + d3dvd.Depth + ((dz < 0.5f) ? -1 : 1)) % d3dvd.Depth;

        dx = 1.0f - FABS(dx - 0.5f);
        dy = 1.0f - FABS(dy - 0.5f);
        dz = 1.0f - FABS(dz - 0.5f);

        hr = pd3dtv->LockBox(0, &d3dlb, NULL, D3DLOCK_READONLY);
        if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::LockBox"))) {
            pd3dtv->Release();
            return 0;
        }

        Swizzler swz(d3dvd.Width, d3dvd.Height, d3dvd.Depth);

#if 0
        cSample[0] = *((LPDWORD)((LPBYTE)d3dlb.pBits + tz[0] * d3dlb.SlicePitch + ty[0] * d3dlb.RowPitch) + tx[0]);
        cSample[1] = *((LPDWORD)((LPBYTE)d3dlb.pBits + tz[0] * d3dlb.SlicePitch + ty[0] * d3dlb.RowPitch) + tx[1]);
        cSample[2] = *((LPDWORD)((LPBYTE)d3dlb.pBits + tz[0] * d3dlb.SlicePitch + ty[1] * d3dlb.RowPitch) + tx[0]);
        cSample[3] = *((LPDWORD)((LPBYTE)d3dlb.pBits + tz[0] * d3dlb.SlicePitch + ty[1] * d3dlb.RowPitch) + tx[1]);
#else
        cSample[0] = *((LPDWORD)d3dlb.pBits + (swz.SwizzleU(tx[0]) | swz.SwizzleV(ty[0]) | swz.SwizzleW(tz[0])));
        cSample[1] = *((LPDWORD)d3dlb.pBits + (swz.SwizzleU(tx[1]) | swz.SwizzleV(ty[0]) | swz.SwizzleW(tz[0])));
        cSample[2] = *((LPDWORD)d3dlb.pBits + (swz.SwizzleU(tx[0]) | swz.SwizzleV(ty[1]) | swz.SwizzleW(tz[0])));
        cSample[3] = *((LPDWORD)d3dlb.pBits + (swz.SwizzleU(tx[1]) | swz.SwizzleV(ty[1]) | swz.SwizzleW(tz[0])));
#endif
        cSample[4] = pfnLerp(cSample[0], cSample[1], dx);
//        cSample[4] = AddColors(MultiplyColorScalar(cSample[0], dx), MultiplyColorScalar(cSample[1], 1.0f - dx));
        cSample[5] = pfnLerp(cSample[2], cSample[3], dx);
//        cSample[5] = AddColors(MultiplyColorScalar(cSample[2], dx), MultiplyColorScalar(cSample[3], 1.0f - dx));
        cSample[6] = pfnLerp(cSample[4], cSample[5], dy);
//        cSample[6] = AddColors(MultiplyColorScalar(cSample[4], dy), MultiplyColorScalar(cSample[5], 1.0f - dy));

#if 0
        cSample[7]  = *((LPDWORD)((LPBYTE)d3dlb.pBits + tz[1] * d3dlb.SlicePitch + ty[0] * d3dlb.RowPitch) + tx[0]);
        cSample[8]  = *((LPDWORD)((LPBYTE)d3dlb.pBits + tz[1] * d3dlb.SlicePitch + ty[0] * d3dlb.RowPitch) + tx[1]);
        cSample[9]  = *((LPDWORD)((LPBYTE)d3dlb.pBits + tz[1] * d3dlb.SlicePitch + ty[1] * d3dlb.RowPitch) + tx[0]);
        cSample[10] = *((LPDWORD)((LPBYTE)d3dlb.pBits + tz[1] * d3dlb.SlicePitch + ty[1] * d3dlb.RowPitch) + tx[1]);
#else
        cSample[7]  = *((LPDWORD)d3dlb.pBits + (swz.SwizzleU(tx[0]) | swz.SwizzleV(ty[0]) | swz.SwizzleW(tz[1])));
        cSample[8]  = *((LPDWORD)d3dlb.pBits + (swz.SwizzleU(tx[1]) | swz.SwizzleV(ty[0]) | swz.SwizzleW(tz[1])));
        cSample[9]  = *((LPDWORD)d3dlb.pBits + (swz.SwizzleU(tx[0]) | swz.SwizzleV(ty[1]) | swz.SwizzleW(tz[1])));
        cSample[10] = *((LPDWORD)d3dlb.pBits + (swz.SwizzleU(tx[1]) | swz.SwizzleV(ty[1]) | swz.SwizzleW(tz[1])));
#endif
        cSample[11] = pfnLerp(cSample[7], cSample[8], dx);
//        cSample[11] = AddColors(MultiplyColorScalar(cSample[7], dx), MultiplyColorScalar(cSample[8], 1.0f - dx));
        cSample[12] = pfnLerp(cSample[9], cSample[10], dx);
//        cSample[12] = AddColors(MultiplyColorScalar(cSample[9], dx), MultiplyColorScalar(cSample[10], 1.0f - dx));
        cSample[13] = pfnLerp(cSample[11], cSample[12], dy);
//        cSample[13] = AddColors(MultiplyColorScalar(cSample[11], dy), MultiplyColorScalar(cSample[12], 1.0f - dy));

        cTexel = pfnLerp(cSample[6], cSample[13], dz);
//        cTexel = AddColors(MultiplyColorScalar(cSample[6], dz), MultiplyColorScalar(cSample[13], 1.0f - dz));

        pd3dtv->UnlockBox(0);
    }

    pd3dtb->Release();

    return cTexel;
}

//******************************************************************************
BOOL CPSRasterizer::SetShaderStates(D3DPIXELSHADERDEF* pd3dpsd) {

#ifdef UNDER_XBOX

    HRESULT hr;
    BOOL bRet = TRUE;

    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAINPUTS0, pd3dpsd->PSAlphaInputs[0]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAINPUTS0, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAINPUTS1, pd3dpsd->PSAlphaInputs[1]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAINPUTS1, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAINPUTS2, pd3dpsd->PSAlphaInputs[2]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAINPUTS2, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAINPUTS3, pd3dpsd->PSAlphaInputs[3]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAINPUTS3, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAINPUTS4, pd3dpsd->PSAlphaInputs[4]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAINPUTS4, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAINPUTS5, pd3dpsd->PSAlphaInputs[5]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAINPUTS5, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAINPUTS6, pd3dpsd->PSAlphaInputs[6]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAINPUTS6, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAINPUTS7, pd3dpsd->PSAlphaInputs[7]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAINPUTS7, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSFINALCOMBINERINPUTSABCD, pd3dpsd->PSFinalCombinerInputsABCD);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSFINALCOMBINERINPUTS0, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSFINALCOMBINERINPUTSEFG, pd3dpsd->PSFinalCombinerInputsEFG);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSFINALCOMBINERINPUTS1, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT0_0, pd3dpsd->PSConstant0[0]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT0_0, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT0_1, pd3dpsd->PSConstant0[1]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT0_1, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT0_2, pd3dpsd->PSConstant0[2]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT0_2, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT0_3, pd3dpsd->PSConstant0[3]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT0_3, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT0_4, pd3dpsd->PSConstant0[4]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT0_4, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT0_5, pd3dpsd->PSConstant0[5]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT0_5, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT0_6, pd3dpsd->PSConstant0[6]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT0_6, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT0_7, pd3dpsd->PSConstant0[7]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT0_7, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT1_0, pd3dpsd->PSConstant1[0]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT1_0, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT1_1, pd3dpsd->PSConstant1[1]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT1_1, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT1_2, pd3dpsd->PSConstant1[2]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT1_2, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT1_3, pd3dpsd->PSConstant1[3]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT1_3, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT1_4, pd3dpsd->PSConstant1[4]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT1_4, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT1_5, pd3dpsd->PSConstant1[5]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT1_5, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT1_6, pd3dpsd->PSConstant1[6]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT1_6, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCONSTANT1_7, pd3dpsd->PSConstant1[7]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCONSTANT1_7, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAOUTPUTS0, pd3dpsd->PSAlphaOutputs[0]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAOUTPUTS0, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAOUTPUTS1, pd3dpsd->PSAlphaOutputs[1]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAOUTPUTS1, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAOUTPUTS2, pd3dpsd->PSAlphaOutputs[2]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAOUTPUTS2, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAOUTPUTS3, pd3dpsd->PSAlphaOutputs[3]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAOUTPUTS3, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAOUTPUTS4, pd3dpsd->PSAlphaOutputs[4]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAOUTPUTS4, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAOUTPUTS5, pd3dpsd->PSAlphaOutputs[5]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAOUTPUTS5, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAOUTPUTS6, pd3dpsd->PSAlphaOutputs[6]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAOUTPUTS6, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSALPHAOUTPUTS7, pd3dpsd->PSAlphaOutputs[7]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSALPHAOUTPUTS7, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBINPUTS0, pd3dpsd->PSRGBInputs[0]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBINPUTS0, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBINPUTS1, pd3dpsd->PSRGBInputs[1]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBINPUTS1, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBINPUTS2, pd3dpsd->PSRGBInputs[2]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBINPUTS2, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBINPUTS3, pd3dpsd->PSRGBInputs[3]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBINPUTS3, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBINPUTS4, pd3dpsd->PSRGBInputs[4]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBINPUTS4, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBINPUTS5, pd3dpsd->PSRGBInputs[5]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBINPUTS5, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBINPUTS6, pd3dpsd->PSRGBInputs[6]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBINPUTS6, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBINPUTS7, pd3dpsd->PSRGBInputs[7]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBINPUTS7, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCOMPAREMODE, pd3dpsd->PSCompareMode);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCOMPAREMODE, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSFINALCOMBINERCONSTANT0, pd3dpsd->PSFinalCombinerConstant0);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSFINALCOMBINERCONSTANT0, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSFINALCOMBINERCONSTANT1, pd3dpsd->PSFinalCombinerConstant1);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSFINALCOMBINERCONSTANT1, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBOUTPUTS0, pd3dpsd->PSRGBOutputs[0]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBOUTPUTS0, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBOUTPUTS1, pd3dpsd->PSRGBOutputs[1]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBOUTPUTS1, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBOUTPUTS2, pd3dpsd->PSRGBOutputs[2]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBOUTPUTS2, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBOUTPUTS3, pd3dpsd->PSRGBOutputs[3]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBOUTPUTS3, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBOUTPUTS4, pd3dpsd->PSRGBOutputs[4]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBOUTPUTS4, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBOUTPUTS5, pd3dpsd->PSRGBOutputs[5]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBOUTPUTS5, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBOUTPUTS6, pd3dpsd->PSRGBOutputs[6]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBOUTPUTS6, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSRGBOUTPUTS7, pd3dpsd->PSRGBOutputs[7]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSRGBOUTPUTS7, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSCOMBINERCOUNT, pd3dpsd->PSCombinerCount);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSCOMBINERCOUNT, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSTEXTUREMODES, pd3dpsd->PSTextureModes);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSTEXTUREMODES, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSDOTMAPPING, pd3dpsd->PSDotMapping);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSDOTMAPPING, ...)"))) {
        bRet = FALSE;
    }
    hr = m_pDevice->SetRenderState(D3DRS_PSINPUTTEXTURE, pd3dpsd->PSInputTexture);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_PSINPUTTEXTURE, ...)"))) {
        bRet = FALSE;
    }

    return bRet;

#else
    return FALSE;
#endif
}

//******************************************************************************
//
// Method:
//
//     ProcessInput
//
// Description:
//
//     Process user input for the scene.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CPSRasterizer::ProcessInput() {

    CScene::ProcessInput();
}

//******************************************************************************
//
// Method:
//
//     InitView
//
// Description:
//
//     Initialize the camera view in the scene.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CPSRasterizer::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -50.0f);
    m_camInitial.vInterest     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_camInitial.fRoll         = 0.0f;
    m_camInitial.fFieldOfView  = M_PI / 4.0f;
    m_camInitial.fNearPlane    = 0.1f;
    m_camInitial.fFarPlane     = 1000.0f;
    m_pDisplay->SetCamera(&m_camInitial);

    return m_pDisplay->SetView(&m_camInitial);
}

//******************************************************************************
// Scene window procedure (pseudo-subclassed off the main window procedure)
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     WndProc
//
// Description:
//
//     Scene window procedure to process messages received by the main 
//     application window.
//
// Arguments:
//
//     LRESULT* plr             - Result of the message processing
//
//     HWND hWnd                - Application window
//
//     UINT uMsg                - Message to process
//
//     WPARAM wParam            - First message parameter
//
//     LPARAM lParam            - Second message parameter
//
// Return Value:
//
//     TRUE if the message was handled, FALSE otherwise.
//
//******************************************************************************
BOOL CPSRasterizer::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;

#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_F12: // Toggle pause state
                    m_bPaused = !m_bPaused;
                    return TRUE;
            }

            break;
    }

#endif // !UNDER_XBOX

    return CScene::WndProc(plr, hWnd, uMsg, wParam, lParam);
}
