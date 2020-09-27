/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    revolve.h

Description:

    Vertex generation routines for objects symmetrical in one dimension.

*******************************************************************************/

#ifndef __REVOLVE_H__
#define __REVOLVE_H__

//******************************************************************************
// Data types
//******************************************************************************

typedef float (* RADPROC)(float y, float fLength, LPVOID pvContext);

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
// Generated vertices defining a revolved mesh
typedef struct _REVOLUTION {
    PVERTEX         prCenter;
    PVERTEX         prTop;
    PVERTEX         prBottom;
    LPWORD          pwCenter;
    UINT            uCenterVertices;
    UINT            uCenterIndices;
    UINT            uTopVertices;
    UINT            uBottomVertices;
} REVOLUTION, *PREVOLUTION;

//******************************************************************************
// Function prototypes
//******************************************************************************

BOOL CreateRevolution(PREVOLUTION prv, float fLength, UINT uSteps, 
                        UINT uRevolutions, RADPROC pfnRadius, 
                        LPVOID pvContext = NULL, float fThreshold = 0.001f, 
                        BOOL bConvex = TRUE, 
                        float fScaleU = 1.0f, float fScaleV = 1.0f);

void ReleaseRevolution(PREVOLUTION prv);

#endif // __REVOLVE_H__
