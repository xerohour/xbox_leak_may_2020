///////////////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 1998.
//
// reftnl.h
//
// Direct3D Reference Transform and Lighting - Main Header File
//
///////////////////////////////////////////////////////////////////////////////

#ifndef  _REFTNL_H
#define  _REFTNL_H

#ifdef __cplusplus
extern "C" {
#endif

// Default color values that should be used when ther is no lighting and
// color in vertices provided
#define D3D_DEFAULT_DIFFUSE  0xFFFFFFFF;
#define D3D_DEFAULT_SPECULAR 0;

struct _nvLight;

//---------------------------------------------------------------------
// NVLIGHTING
// All the lighting related state clubbed together
//---------------------------------------------------------------------

typedef struct {D3DVALUE r,g,b;} RGBCOLOR;

typedef struct _NVLIGHTING
{
    // Active Light list
    struct _nvLight *pActiveLights;

    // Ambient color set by D3DRENDERSTATE_AMBIENT
    // components are all scaled to 0 - 1
    D3DCOLORVALUE dcvAmbient;

} NVLIGHTING;

//-----------------------------------------------------------------------------
//
// nvLight - The light object used by the Reference T&L implementation
// An array of these are instanced in the ReferenceRasterizer object.
//
//-----------------------------------------------------------------------------

// nvLight flags
#define NVLIGHT_DEFINED             0x00000001  // has this light been defined?
#define NVLIGHT_ENABLED             0x00000002  // Is the light active

typedef struct _expParams
{
    D3DVALUE L;
    D3DVALUE M;
    D3DVALUE N;
} expParams, *pexpParms;

typedef struct _nvLight
{
    DWORD            dwFlags;           // see above
    struct _nvLight *pNext;             // next active light
    D3DLIGHT7        Light7;            // Light data set by the runtime (unmodified!!)
    // derived data
    D3DVECTOR        direction;         // negated a la OGL and normalized
    expParams        falloffParams;     // spotlight falloff params
    D3DVALUE         dvScale;           // spotlight scale factor
    D3DVALUE         dvW;               // spotlight direction W
} nvLight, *pnvLight;

#ifdef __cplusplus
}
#endif

#endif // _REFTNL_H
