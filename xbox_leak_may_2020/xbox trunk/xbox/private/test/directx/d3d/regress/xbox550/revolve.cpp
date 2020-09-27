/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    revolve.cpp

Description:

    Direct3D light routines.

*******************************************************************************/

#define D3D_OVERLOADS

#include <windows.h>
#include "..\..\inc\d3dlocus.h"
#include "revolve.h"

//******************************************************************************
// Local function prototypes
//******************************************************************************

static void         GetSteps(float fLength, UINT* puSteps, float* pfSteps,
                             RADPROC pfnRadius, LPVOID pvContext, 
                             float fThreshold);
static D3DVECTOR    SubtractVertexVector(VERTEX r1, VERTEX r2);
static void         NormalizeVertexNormal(PVERTEX pr);

//******************************************************************************
// Revolution functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     CreateRevolution
//
// Description:
//
//     Generate a set of vertices defining a three dimensional object 
//     according to a given radius function "revolved" around the y axis
//     (to determine the discrete points of the vertices).
//
// Arguments:
//
//     PREVOLUTION                  - Pointer to a structure that will
//                                    contain the generated vertices
//
//     float fLength                - Length of the object along the y axis
//
//     UINT uSteps                  - Maximum number of steps along the y axis 
//                                    at which the radius function will
//                                    be evaluated to generate vertices.  The
//                                    actual number of steps used may be 
//                                    less than uSteps depending on the value
//                                    of fThreshold.
//
//     UINT uRevolutions            - Number of rotational steps taken about
//                                    the y axis for each step (uSteps) taken
//                                    along the length of the object
//
//     RADPROC pfnRadius            - Radius function used to shape the object
//
//     LPVOID pvContext             - User variable passed into the radius function
//
//     float fThreshold             - Threshold value used to cull steps along
//              (Optional)            the y axis.  If the rate of change from
//                                    one step to the next is not significant
//                                    enough according to the threshold, the
//                                    vertices at that step will not be included
//                                    in the final mesh.
//
//     BOOL bConvex (Optional)      - TRUE to generate a convex object, FALSE
//                                    to generate a concave object
//
//     float fScaleU (Optional)     - Horizontal texture scale value
//
//     float fScaleV (Optional)     - Vertical texture scale value
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CreateRevolution(PREVOLUTION prv, float fLength, UINT uSteps, 
                        UINT uRevolutions, RADPROC pfnRadius, LPVOID pvContext,
                        float fThreshold, BOOL bConvex, 
                        float fScaleU, float fScaleV)
{
    UINT                    uNumVertices;
    PVERTEX                 pdvVertices;
    VERTEX                  rTemp;
    D3DXVECTOR3             v1, v2, v3, v4, n1, n2, n3, n4;
    UINT                    i, j, k;
    float                   a, da = M_2PI / (float)(uRevolutions);
    float                   y;
    float                   fRadius;
    BOOL                    bPointedTop, bPointedBottom;
    float*                  pfSteps;

    // Parameter validation
    if (!prv || !pfnRadius) {
        return FALSE;
    }

    // Initialize the revolution structure
    prv->prTop = NULL;
    prv->uTopVertices = 0;
    prv->prBottom = NULL;
    prv->uBottomVertices = 0;

    // Use the threshold to cull the steps
    pfSteps = new float[uSteps + 1];
    if (!pfSteps) {
        return FALSE;
    }
    GetSteps(fLength, &uSteps, pfSteps, pfnRadius, pvContext, fThreshold);
    pfSteps[uSteps] = 0.0f;

    uNumVertices = (uSteps + 1) * uRevolutions;

    // Create the vertices
    pdvVertices = new VERTEX[uNumVertices];
    if (!pdvVertices) {
        delete [] pfSteps;
        return FALSE;
    }
    memset(pdvVertices, 0, uNumVertices*sizeof(VERTEX));

    for (i = 0; i <= uSteps; i++) {
        a = (float)(0.0);
        y = pfSteps[i];
        fRadius = pfnRadius(y, fLength, pvContext);
        for (j = 0; j < uRevolutions; j++) {
            (pdvVertices+(i*uRevolutions)+j)->vPosition.x = (fRadius*(float)(cos((double)a)));
            (pdvVertices+(i*uRevolutions)+j)->vPosition.z = (fRadius*(float)(sin((double)a)));
            (pdvVertices+(i*uRevolutions)+j)->vPosition.y = y;
            (pdvVertices+(i*uRevolutions)+j)->u0 = (bConvex?a / M_2PI:(M_2PI - a) / M_2PI) * fScaleU;
            (pdvVertices+(i*uRevolutions)+j)->v0 = ((fLength - y) / fLength) * fScaleV;
            if ((pdvVertices+(i*uRevolutions)+j)->vPosition.x < (float)(0.00001) 
                            && (pdvVertices+(i*uRevolutions)+j)->vPosition.x > (float)(-0.00001)) {
                (pdvVertices+(i*uRevolutions)+j)->vPosition.x = (float)(0.0);
            }
            if ((pdvVertices+(i*uRevolutions)+j)->vPosition.y < (float)(0.00001) 
                            && (pdvVertices+(i*uRevolutions)+j)->vPosition.y > (float)(-0.00001)) {
                (pdvVertices+(i*uRevolutions)+j)->vPosition.y = (float)(0.0);
            }
            if ((pdvVertices+(i*uRevolutions)+j)->vPosition.z < (float)(0.00001) 
                            && (pdvVertices+(i*uRevolutions)+j)->vPosition.z > (float)(-0.00001)) {
                (pdvVertices+(i*uRevolutions)+j)->vPosition.z = (float)(0.0);
            }
            if ((pdvVertices+(i*uRevolutions)+j)->u0 < (float)(0.00001) 
                            && (pdvVertices+(i*uRevolutions)+j)->u0 > (float)(-0.00001)) {
                (pdvVertices+(i*uRevolutions)+j)->u0 = (float)(0.0);
            }
            if ((pdvVertices+(i*uRevolutions)+j)->v0 < (float)(0.00001) 
                            && (pdvVertices+(i*uRevolutions)+j)->v0 > (float)(-0.00001)) {
                (pdvVertices+(i*uRevolutions)+j)->v0 = (float)(0.0);
            }
            a += da;
        }
    }

    delete [] pfSteps;

    bPointedTop = (pdvVertices[0].vPosition.x == pdvVertices[1].vPosition.x && 
                    pdvVertices[0].vPosition.y == pdvVertices[1].vPosition.y &&
                    pdvVertices[0].vPosition.z == pdvVertices[1].vPosition.z);
    bPointedBottom = ((pdvVertices + uNumVertices - uRevolutions)->vPosition.x 
                        == (pdvVertices + uNumVertices - uRevolutions + 1)->vPosition.x &&
                       (pdvVertices + uNumVertices - uRevolutions)->vPosition.y 
                        == (pdvVertices + uNumVertices - uRevolutions + 1)->vPosition.y &&
                       (pdvVertices + uNumVertices - uRevolutions)->vPosition.z 
                        == (pdvVertices + uNumVertices - uRevolutions + 1)->vPosition.z);

    // Generate the normals
    for (i = 0; i < uRevolutions; i++) {
        if (bPointedTop && uSteps > 1) {
            pdvVertices[i].vNormal.x = (float)(0.0);
            pdvVertices[i].vNormal.y = (float)(1.0);
            pdvVertices[i].vNormal.z = (float)(0.0);
        }
        else {
            if (bPointedTop) {
                v1 = SubtractVertexVector(pdvVertices[!(i%uRevolutions)?i+uRevolutions-1+uRevolutions:i-1+uRevolutions], pdvVertices[i]);
                v3 = SubtractVertexVector(pdvVertices[!((i+1)%uRevolutions)?i+1:i+1+uRevolutions], pdvVertices[i]);
            }
            else {
                v1 = SubtractVertexVector(pdvVertices[!(i%uRevolutions)?i+uRevolutions-1:i-1], pdvVertices[i]);
                v3 = SubtractVertexVector(pdvVertices[!((i+1)%uRevolutions)?i-uRevolutions+1:i+1], pdvVertices[i]);
            }
            v4 = SubtractVertexVector(pdvVertices[i+uRevolutions], pdvVertices[i]);
            if (bConvex) {
                D3DXVec3Cross(&n3, &v3, &v4);
                D3DXVec3Cross(&n4, &v4, &v1);
            }
            else {
                D3DXVec3Cross(&n3, &v4, &v3);
                D3DXVec3Cross(&n4, &v1, &v4);
            }
            pdvVertices[i].vNormal.x = (n3.x + n4.x) / (float)(2.0);
            pdvVertices[i].vNormal.y = (n3.y + n4.y) / (float)(2.0);
            pdvVertices[i].vNormal.z = (n3.z + n4.z) / (float)(2.0);
            NormalizeVertexNormal(&pdvVertices[i]);

            if (pdvVertices[i].vNormal.x < (float)(0.00001) && pdvVertices[i].vNormal.x > (float)(-0.00001)) {
                pdvVertices[i].vNormal.x = (float)(0.0);
            }
            if (pdvVertices[i].vNormal.y < (float)(0.00001) && pdvVertices[i].vNormal.y > (float)(-0.00001)) {
                pdvVertices[i].vNormal.y = (float)(0.0);
            }
            if (pdvVertices[i].vNormal.z < (float)(0.00001) && pdvVertices[i].vNormal.z > (float)(-0.00001)) {
                pdvVertices[i].vNormal.z = (float)(0.0);
            }
        }
    }

    for (i = uRevolutions; i < uNumVertices - uRevolutions; i++) {
        v1 = SubtractVertexVector(pdvVertices[!(i%uRevolutions)?i+uRevolutions-1:i-1], pdvVertices[i]);
        v2 = SubtractVertexVector(pdvVertices[i-uRevolutions], pdvVertices[i]);
        v3 = SubtractVertexVector(pdvVertices[!((i+1)%uRevolutions)?i-uRevolutions+1:i+1], pdvVertices[i]);
        v4 = SubtractVertexVector(pdvVertices[i+uRevolutions], pdvVertices[i]);
        if (bConvex) {
            D3DXVec3Cross(&n1, &v1, &v2);
            D3DXVec3Cross(&n2, &v2, &v3);
            D3DXVec3Cross(&n3, &v3, &v4);
            D3DXVec3Cross(&n4, &v4, &v1);
        }
        else {
            D3DXVec3Cross(&n1, &v2, &v1);
            D3DXVec3Cross(&n2, &v3, &v2);
            D3DXVec3Cross(&n3, &v4, &v3);
            D3DXVec3Cross(&n4, &v1, &v4);
        }
        pdvVertices[i].vNormal.x = (n1.x + n2.x + n3.x + n4.x) / (float)(4.0);
        pdvVertices[i].vNormal.y = (n1.y + n2.y + n3.y + n4.y) / (float)(4.0);
        pdvVertices[i].vNormal.z = (n1.z + n2.z + n3.z + n4.z) / (float)(4.0);
        NormalizeVertexNormal(&pdvVertices[i]);
        if (pdvVertices[i].vNormal.x < (float)(0.00001) && pdvVertices[i].vNormal.x > (float)(-0.00001)) {
            pdvVertices[i].vNormal.x = (float)(0.0);
        }
        if (pdvVertices[i].vNormal.y < (float)(0.00001) && pdvVertices[i].vNormal.y > (float)(-0.00001)) {
            pdvVertices[i].vNormal.y = (float)(0.0);
        }
        if (pdvVertices[i].vNormal.z < (float)(0.00001) && pdvVertices[i].vNormal.z > (float)(-0.00001)) {
            pdvVertices[i].vNormal.z = (float)(0.0);
        }
    }

    for (i = uNumVertices - uRevolutions; i < uNumVertices; i++) {
        if (bPointedBottom && uSteps > 1) {
            pdvVertices[i].vNormal.x = (float)(0.0);
            pdvVertices[i].vNormal.y = (float)(-1.0);
            pdvVertices[i].vNormal.z = (float)(0.0);
        }
        else {
            if (bPointedBottom) {
                v1 = SubtractVertexVector(pdvVertices[!(i%uRevolutions)?i-1:i-1-uRevolutions], pdvVertices[i]);
                v3 = SubtractVertexVector(pdvVertices[!((i+1)%uRevolutions)?i-uRevolutions+1-uRevolutions:i+1-uRevolutions], pdvVertices[i]);
            }
            else {
                v1 = SubtractVertexVector(pdvVertices[!(i%uRevolutions)?i+uRevolutions-1:i-1], pdvVertices[i]);
                v3 = SubtractVertexVector(pdvVertices[!((i+1)%uRevolutions)?i-uRevolutions+1:i+1], pdvVertices[i]);
            }
            v2 = SubtractVertexVector(pdvVertices[i-uRevolutions], pdvVertices[i]);
            if (bConvex) {
                D3DXVec3Cross(&n1, &v1, &v2);
                D3DXVec3Cross(&n2, &v2, &v3);
            }
            else {
                D3DXVec3Cross(&n1, &v2, &v1);
                D3DXVec3Cross(&n2, &v3, &v2);
            }
            pdvVertices[i].vNormal.x = (n1.x + n2.x) / (float)(2.0);
            pdvVertices[i].vNormal.y = (n1.y + n2.y) / (float)(2.0);
            pdvVertices[i].vNormal.z = (n1.z + n2.z) / (float)(2.0);
            NormalizeVertexNormal(&pdvVertices[i]);
            if (pdvVertices[i].vNormal.x < (float)(0.00001) && pdvVertices[i].vNormal.x > (float)(-0.00001)) {
                pdvVertices[i].vNormal.x = (float)(0.0);
            }
            if (pdvVertices[i].vNormal.y < (float)(0.00001) && pdvVertices[i].vNormal.y > (float)(-0.00001)) {
                pdvVertices[i].vNormal.y = (float)(0.0);
            }
            if (pdvVertices[i].vNormal.z < (float)(0.00001) && pdvVertices[i].vNormal.z > (float)(-0.00001)) {
                pdvVertices[i].vNormal.z = (float)(0.0);
            }
        }
    }

    // Allocate the center vertices
    prv->uCenterVertices = (uSteps + 1) * (uRevolutions + 1);
    prv->prCenter = (PVERTEX)MemAlloc32(prv->uCenterVertices * sizeof(VERTEX));
    if (!prv->prCenter) {
        delete [] pdvVertices;
        return FALSE;
    }

    for (i = 0; i <= uSteps; i++) {
        memcpy(&prv->prCenter[i*(uRevolutions+1)], &pdvVertices[i*uRevolutions], uRevolutions * sizeof(VERTEX));
        prv->prCenter[(i+1)*(uRevolutions+1)-1] = pdvVertices[i*uRevolutions];
        prv->prCenter[(i+1)*(uRevolutions+1)-1].u0 = bConvex?fScaleU:(float)(0.0);
    }

    // Add the list indices
    prv->uCenterIndices = uRevolutions * uSteps * 6;
    prv->pwCenter = new WORD[prv->uCenterIndices];
    if (!prv->pwCenter) {
        delete [] prv->prCenter;
        delete [] pdvVertices;
        return FALSE;
    }

    k = 0;
    for (i = 1; i <= uSteps; i++) {
        for (j = 0; j < uRevolutions; j++) {
            prv->pwCenter[k++] = i*(uRevolutions+1)+j;
            prv->pwCenter[k++] = (i-1)*(uRevolutions+1)+j;
            prv->pwCenter[k++] = i*(uRevolutions+1)+j+1;
            prv->pwCenter[k++] = (i-1)*(uRevolutions+1)+j;
            prv->pwCenter[k++] = (i-1)*(uRevolutions+1)+j+1;
            prv->pwCenter[k++] = i*(uRevolutions+1)+j+1;
        }
    }

    // Close off the ends
    if (bConvex) {
        for (i = 0; i < uRevolutions / 2; i++) {
            rTemp = pdvVertices[i];
            pdvVertices[i] = pdvVertices[uRevolutions - i - 1];
            pdvVertices[uRevolutions - i - 1] = rTemp;
        }
    }
    else {
        for (i = 0; i < uRevolutions / 2; i++) {
            rTemp = pdvVertices[uNumVertices - uRevolutions + i];
            pdvVertices[uNumVertices - uRevolutions + i] = pdvVertices[uNumVertices - i - 1];
            pdvVertices[uNumVertices - i - 1] = rTemp;
        }
    }
    v1 = SubtractVertexVector(pdvVertices[1], pdvVertices[0]);
    v2 = SubtractVertexVector(pdvVertices[2], pdvVertices[0]);
    D3DXVec3Cross(&v1, &v1, &v2);
    D3DXVec3Normalize(&v1, &v1);
    if (v1.x < (float)(0.00001) && v1.x > (float)(-0.00001)) {
        v1.x = (float)(0.0);
    }
    if (v1.y < (float)(0.00001) && v1.y > (float)(-0.00001)) {
        v1.y = (float)(0.0);
    }
    if (v1.z < (float)(0.00001) && v1.z > (float)(-0.00001)) {
        v1.z = (float)(0.0);
    }
    for (i = 0; i < uRevolutions; i++) {
        pdvVertices[i].vNormal.x = v1.x;
        pdvVertices[i].vNormal.y = v1.y;
        pdvVertices[i].vNormal.z = v1.z;
    }
    v1 = SubtractVertexVector(pdvVertices[uNumVertices - uRevolutions + 1], pdvVertices[uNumVertices - uRevolutions]);
    v2 = SubtractVertexVector(pdvVertices[uNumVertices - uRevolutions + 2], pdvVertices[uNumVertices - uRevolutions]);
    D3DXVec3Cross(&v1, &v1, &v2);
    D3DXVec3Normalize(&v1, &v1);
    if (v1.x < (float)(0.00001) && v1.x > (float)(-0.00001)) {
        v1.x = (float)(0.0);
    }
    if (v1.y < (float)(0.00001) && v1.y > (float)(-0.00001)) {
        v1.y = (float)(0.0);
    }
    if (v1.z < (float)(0.00001) && v1.z > (float)(-0.00001)) {
        v1.z = (float)(0.0);
    }
    for (i = uNumVertices - uRevolutions; i < uNumVertices; i++) {
        pdvVertices[i].vNormal.x = v1.x;
        pdvVertices[i].vNormal.y = v1.y;
        pdvVertices[i].vNormal.z = v1.z;
    }
    if (pdvVertices[0].vPosition.x != pdvVertices[1].vPosition.x 
                                || pdvVertices[0].vPosition.y != pdvVertices[1].vPosition.y
                                || pdvVertices[0].vPosition.z != pdvVertices[1].vPosition.z) {
        prv->prTop = (PVERTEX)MemAlloc32(uRevolutions * sizeof(VERTEX));
        if (prv->prTop) {
            memcpy(prv->prTop, pdvVertices, uRevolutions * sizeof(VERTEX));
            prv->uTopVertices = uRevolutions;
        }
    }
    if ((pdvVertices + uNumVertices - uRevolutions)->vPosition.x != (pdvVertices + uNumVertices - uRevolutions + 1)->vPosition.x
        || (pdvVertices + uNumVertices - uRevolutions)->vPosition.y != (pdvVertices + uNumVertices - uRevolutions + 1)->vPosition.y
        || (pdvVertices + uNumVertices - uRevolutions)->vPosition.z != (pdvVertices + uNumVertices - uRevolutions + 1)->vPosition.z) {
        prv->prBottom = (PVERTEX)MemAlloc32(uRevolutions * sizeof(VERTEX));
        if (prv->prBottom) {
            memcpy(prv->prBottom, pdvVertices + uNumVertices - uRevolutions, uRevolutions * sizeof(VERTEX));
            prv->uBottomVertices = uRevolutions;
        }
    }
        
    delete [] pdvVertices;

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     ReleaseRevolution
//
// Description:
//
//     Release the memory allocated in the creation of the revolved object.
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
void ReleaseRevolution(PREVOLUTION prv) {

    if (prv->prCenter) {
        MemFree32(prv->prCenter);
        prv->prCenter = NULL;
    }
    if (prv->pwCenter) {
        delete [] prv->pwCenter;
        prv->pwCenter = NULL;
    }
    if (prv->prTop) {
        MemFree32(prv->prTop);
        prv->prTop = NULL;
    }
    if (prv->prBottom) {
        MemFree32(prv->prBottom);
        prv->prBottom = NULL;
    }
}

//******************************************************************************
//
// Function:
//
//     GetSteps
//
// Description:
//
//     Generate a set of vertices defining a three dimensional object 
//     according to a given radius function "revolved" around the y axis
//     (to determine the discrete points of the vertices).
//
// Arguments:
//
//     float fLength             - Length of the object along the y axis
//
//     UINT* puSteps (In)           - Pointer to the maximum number of steps 
//                                    along the y axis at which the radius 
//                                    function will be evaluated to generate
//                                    vertices.
//
//     UINT* puSteps (Out)          - Actual number of culled steps to use
//
//     float* pfSteps           - Array to be filled with the step values
//                                    along the y axis at which the radius
//                                    function will be evaluated
//
//     RADPROC pfnRadius            - Radius function used to shape the object
//
//     LPVOID pvContext             - User variable passed into the radius function
//
//     float fThreshold          - Threshold value used to cull steps along
//              (Optional)            the y axis.  If the rate of change from
//                                    one step to the next is not significant
//                                    enough according to the threshold, the
//                                    vertices at that step will not be included
//                                    in the final mesh.
//
// Return Value:
//
//     None.
//
//******************************************************************************
static void GetSteps(float fLength, UINT* puSteps, float* pfSteps,
                     RADPROC pfnRadius, LPVOID pvContext, float fThreshold) 
{
    float fStep = fLength / ((float)(*puSteps));
    float fChange;
    float fRadius1, fRadius2;
    float fRadiusDelta1, fRadiusDelta2;
    float f;

    pfSteps[0] = fLength;
    *puSteps = 1;

    fRadius1 = pfnRadius(fLength - fStep, fLength, pvContext);
    fRadiusDelta1 = pfnRadius(fLength, fLength, pvContext) - fRadius1;

    for (f = fLength - fStep; f > 0.00001f; f -= fStep) {

        fRadius2 = pfnRadius(f - fStep, fLength, pvContext);
        fRadiusDelta2 = fRadius1 - fRadius2;
        fChange = (float)(fabs(fRadiusDelta1 - fRadiusDelta2));
        fRadius1 = fRadius2;
        fRadiusDelta1 = fRadiusDelta2;
        if (fChange > fThreshold) {
            // The rate of change of the function at this step is significant
            // enough to include the vertices in the revolution
            pfSteps[(*puSteps)++] = f;
        }
    }
}

//******************************************************************************
// Helper functions
//******************************************************************************

//******************************************************************************
static D3DVECTOR SubtractVertexVector(VERTEX r1, VERTEX r2) {

    D3DXVECTOR3 v;

    v.x = r1.vPosition.x - r2.vPosition.x;
    v.y = r1.vPosition.y - r2.vPosition.y;
    v.z = r1.vPosition.z - r2.vPosition.z;

    return v;
}

//******************************************************************************
static void NormalizeVertexNormal(PVERTEX pr) {

/*
    D3DXVECTOR3 v;
    v.x = pr->vNormal.x;
    v.y = pr->vNormal.y;
    v.z = pr->vNormal.z;
    D3DXVec3Normalize(&v, &v);
    pr->vNormal.x = v.x;
    pr->vNormal.y = v.y;
    pr->vNormal.z = v.z;
*/
    D3DXVec3Normalize(&pr->vNormal, &pr->vNormal);
}
