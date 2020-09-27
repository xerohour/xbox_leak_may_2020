/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    tunnel.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "tunnel.h"

#define D3DVAL(x)   ((float)(x))
/*
 * These defines describe the section of the tube in the execute buffer at
 * one time. (Note, tube and tunnel are used interchangeably).
 */
#define SEGMENTS 20   /* Number of segments in memory at one time.  Each
                       * segment is made up oftriangles spanning between
                       * two rings.
                       */ 
#define SIDES 32//8       /* Number of sides on each ring. */
#define TEX_RINGS 5   /* Number of rings to stretch the texture over. */
#define NUM_V (SIDES*(SEGMENTS+1)) // Number of vertices in memory at once
#define NUM_TRI (SIDES*SEGMENTS*2) // Number of triangles in memory
#define TUBE_R 1.0f      /* Radius of the tube. */
#define SPLINE_POINTS 50 /* Number of spline points to initially
                          * calculate.  The section in memory represents
                          * only a fraction of this.
                          */
/*
 * Movement and track scalars given in terms of position along the spline
 * curve.
 */
#define SEGMENT_LENGTH 0.05 /* Length of each segment along curve. */
#define SPEED 0.02          /* Amount to increment camera position along
                             * curve for each frame.
                             */
#define DEPTH 0.8           /* How close the camera can get to the end of
                             * track before new segments are added.
                             */
#define PATH_LENGTH (SPLINE_POINTS - 1) /*Total length of the tunnel.*/

//******************************************************************************
// Structures
//******************************************************************************

typedef D3DMATRIX* LPD3DMATRIX;
typedef D3DVECTOR* LPD3DVECTOR;

/*
 * Triangle list primitive structure
 */
typedef struct _D3DTRIANGLE {
    union {
    WORD    v1;            /* Vertex indices */
    WORD    wV1;
    };
    union {
    WORD    v2;
    WORD    wV2;
    };
    union {
    WORD    v3;
    WORD    wV3;
    };
    WORD        wFlags;       /* Edge (and other) flags */
} D3DTRIANGLE, *LPD3DTRIANGLE;

/*
 * A global structure holding the tube data.
 */
static struct _tube {
    PVERTEX lpV;            /* Points to the vertices. */
    LPWORD pwIndexList;
    LPD3DTRIANGLE lpTri;    /* Points to the triangles which make up the
                             * segments.
                             */
    int TriOffset;          /* Offset into the execute buffer were the
                             * triangle list is found.
                             */
    LPD3DVECTOR lpPoints;   /* Points to the points defining the spline
                             * curve.
                             */
    D3DLIGHT8 light;             /* Structure defining the light. */
    D3DVECTOR cameraP, cameraD, cameraN; /* Vectors defining the camera 
                                          * position, direction and up.
                                          */
    float cameraPos;                     /* Camera position along the 
                                          * spline curve.
                                          */
    D3DVECTOR endP, endD, endN; /* Vectors defining the position, 
                                 * direction and up at the foremost end of
                                 * the section in memory.
                                 */
    float endPos; /* Position along the spline curve of the end. */
    int currentRing, currentSegment; /* Numbers of the ring and tube at 
                                      * the back end of the section.
                                      */
} tube;

//******************************************************************************
// Helper functions
//******************************************************************************

/*
 * -1 d = a
 */
LPD3DMATRIX 
D3DMATRIXInvert(LPD3DMATRIX d, LPD3DMATRIX a)
{
    d->_11 = a->_11;
    d->_12 = a->_21;
    d->_13 = a->_31;
    d->_14 = a->_14;

    d->_21 = a->_12;
    d->_22 = a->_22;
    d->_23 = a->_32;
    d->_24 = a->_24;

    d->_31 = a->_13;
    d->_32 = a->_23;
    d->_33 = a->_33;
    d->_34 = a->_34;

    d->_41 = a->_14;
    d->_42 = a->_24;
    d->_43 = a->_34;
    d->_44 = a->_44;

    return d;
}


/*
 * Set the rotation part of a matrix such that the vector lpD is the new
 * z-axis and lpU is the new y-axis.
 */
LPD3DMATRIX 
D3DMATRIXSetRotation(LPD3DMATRIX lpM, LPD3DVECTOR lpD, LPD3DVECTOR lpU)
{
    float t;
    D3DXVECTOR3 d, u, r;

    /*
     * Normalise the direction vector.
     */
    d.x = lpD->x;
    d.y = lpD->y;
    d.z = lpD->z;
    D3DXVec3Normalize(&d, &d);

    u.x = lpU->x;
    u.y = lpU->y;
    u.z = lpU->z;
    /*
     * Project u into the plane defined by d and normalise.
     */
    t = u.x * d.x + u.y * d.y + u.z * d.z;
    u.x -= d.x * t;
    u.y -= d.y * t;
    u.z -= d.z * t;
    D3DXVec3Normalize(&u, &u);

    /*
     * Calculate the vector pointing along the matrix x axis (in a right
     * handed coordinate system) using cross product.
     */
    D3DXVec3Cross(&r, &u, &d);

    lpM->_11 = r.x;
    lpM->_12 = r.y, lpM->_13 = r.z;
    lpM->_21 = u.x;
    lpM->_22 = u.y, lpM->_23 = u.z;
    lpM->_31 = d.x;
    lpM->_32 = d.y;
    lpM->_33 = d.z;

    return lpM;
}

/*
 * Calculates a point along a B-Spline curve defined by four points. p
 * n output, contain the point. t                                Position
 * along the curve between p2 and p3.  This position is a float between 0
 * and 1. p1, p2, p3, p4    Points defining spline curve. p, at parameter
 * t along the spline curve
 */
D3DVECTOR 
spline(double t, const LPD3DVECTOR p[4])
{
    double t2 = t * t;
    double t3 = t2 * t;
    D3DXVECTOR3 ret(0.0f, 0.0f, 0.0f);
    float m[4];

    m[0] = (float) (0.5f*((-1.0 * t3) + (2.0 * t2) + (-1.0 * t)));
    m[1] = (float) (0.5f*((3.0 * t3) + (-5.0 * t2) + (0.0 * t) + 2.0));
    m[2] = (float) (0.5f*((-3.0 * t3) + (4.0 * t2) + (1.0 * t)));
    m[3] = (float) (0.5f*((1.0 * t3) + (-1.0 * t2) + (0.0 * t)));

    for (int i = 0; i < 4; i += 1) {
        ret += *(D3DXVECTOR3*)(p[i])*m[i];
    }
    return (D3DVECTOR)ret;
}

/*
 * Creates a matrix which is equivalent to having the camera at a
 * specified position. This matrix can be used to convert vertices to
 * camera coordinates. lpP    Position of the camera. lpD    Direction of
 * view. lpN    Up vector. lpM    Matrix to update.
 */
void 
PositionCamera(LPD3DVECTOR lpP, LPD3DVECTOR lpD, LPD3DVECTOR lpN, 
               LPD3DMATRIX lpM)
{
    D3DMATRIX tmp;

    /*
     * Set the rotation part of the matrix and invert it. Vertices must be
     * inverse rotated to achieve the same result of a corresponding 
     * camera rotation.
     */
    tmp._14 = tmp._24 = tmp._34 = tmp._41 = tmp._42 = tmp._43 = (float)0.0;
    tmp._44 = (float)1.0;
    D3DMATRIXSetRotation(&tmp, lpD, lpN);
    D3DMATRIXInvert(lpM, &tmp);
    /*
     * Multiply the rotation matrix by a translation transform.  The
     * translation matrix must be applied first (left of rotation).
     */
    lpM->_41=-(lpM->_11 * lpP->x + lpM->_21 * lpP->y + lpM->_31 * lpP->z);
    lpM->_42=-(lpM->_12 * lpP->x + lpM->_22 * lpP->y + lpM->_32 * lpP->z);
    lpM->_43=-(lpM->_13 * lpP->x + lpM->_23 * lpP->y + lpM->_33 * lpP->z);
}

/*
 * Updates the given position, direction and normal vectors to a given
 * position on the spline curve.  The given up vector is used to determine
 * the new up vector.
 */
void 
MoveToPosition(float position, LPD3DVECTOR lpP, LPD3DVECTOR lpD, 
               LPD3DVECTOR lpN)
{
    LPD3DVECTOR lpSplinePoint[4];
    D3DXVECTOR3 pp, x;
    int i, j;
    float t;

    /*
     * Find the four points along the curve which are around the position.
     */
    i = 0;
    t = position;
    while (t > 1.0) {
        i++;
        if (i == SPLINE_POINTS)
            i = 0;
        t -= 1.0f;
    }
    for (j = 0; j < 4; j++) {
        lpSplinePoint[j] = &tube.lpPoints[i];
        i++;
        if (i == SPLINE_POINTS)
            i = 0;
    }
    /*
     * Get the point at the given position and one just before it.
     */
    *lpP = spline(t, lpSplinePoint);
    pp = spline(t - (float)0.01, lpSplinePoint);
    /*
     * Calculate the direction.
     */
    D3DXVec3Normalize((D3DXVECTOR3*)lpD, &(*(D3DXVECTOR3*)lpP - pp));
    /*
     * Find the new normal.  This method will work provided the change in
     * the normal is not very large.
     */
    D3DXVec3Normalize((D3DXVECTOR3*)lpN, (D3DXVECTOR3*)lpN);
    D3DXVec3Cross(&x, (D3DXVECTOR3*)lpN, (D3DXVECTOR3*)lpD);
    D3DXVec3Normalize((D3DXVECTOR3*)lpN, D3DXVec3Cross(&pp, (D3DXVECTOR3*)lpD, &x));
}


/*
 * Generates a ring of vertices in a plane defined by n and the cross
 * product of n and p.  On exit, joint contains the vertices.  Join must
 * be pre-allocated. Normals are generated pointing in.  Texture
 * coordinates are generated along tu axis and are given along tv.
 */
static void 
MakeRing(const D3DVECTOR& p, 
         const D3DVECTOR& d, 
         const D3DVECTOR& n, float tv,
         PVERTEX joint)
{
    D3DXVECTOR3 nxd;
    
    D3DXVec3Cross(&nxd, (D3DXVECTOR3*)&n, (D3DXVECTOR3*)&d);

    for (int spoke = 0; spoke < SIDES; spoke++) {
        float theta = (float)(2.0 * M_PI) * spoke / SIDES;
        /*
         * v, u defines a unit vector in the plane
         * defined by vectors nxd and n.
         */
        float v = (float)sin(theta);
        float u = (float)cos(theta);
        /*
         * x, y, z define a unit vector in standard coordiante space
         */
        D3DXVECTOR3 pt = u*nxd + v*((D3DXVECTOR3)n);
        /*
         * Position, normals and texture coordiantes.
         */
        joint[spoke] = VERTEX((D3DXVECTOR3)p + pt * TUBE_R, -pt,
                                 1.0f - theta / (float) (2.0f * M_PI), tv);

    }
}


/*
 * Defines the triangles which form a segment between ring1 and ring2 and
 * stores them at lpTri.  lpTri must be pre-allocated.
 */
void 
MakeSegment(int ring1, int ring2, LPWORD pwTri)
{
    int side, triangle = 0;

    for (side = 0; side < SIDES; side++) {
        /*
         * Each side consists of two triangles.
         */
        pwTri[triangle * 3] = ring1 * SIDES + side;
        pwTri[triangle * 3 + 1] = ring2 * SIDES + side;
        pwTri[triangle * 3 + 2] = ring2 * SIDES + ((side + 1) % SIDES);
        
        triangle++;
        pwTri[triangle * 3 + 1] = ring2 * SIDES + ((side + 1) % SIDES);
        pwTri[triangle * 3 + 2] = ring1 * SIDES + ((side + 1) % SIDES);
        pwTri[triangle * 3] = ring1 * SIDES + side;
        
        triangle++;
    }
}


/*
 * Creates a new segment of the tunnel at the current end position.
 * Creates a new ring and segment.
 */
void 
UpdateTubeInMemory(void)
{
    static int texRing = 0; /* Static counter defining the position of
                             * this ring on the texture.
                             */
    int endRing; /* The ring at the end of the tube in memory. */
    int RingOffset, SegmentOffset; /* Offsets into the vertex and triangle 
                                    * lists for the new data.
                                    */
    /*
     * Replace the back ring with a new ring at the front of the tube
     * in memory.
     */
    memmove(&tube.lpV[SIDES], &tube.lpV[0], sizeof(tube.lpV[0]) * (NUM_V - SIDES));
    MakeRing(tube.endP, tube.endD, tube.endN, texRing/(float)TEX_RINGS,
             &tube.lpV[0]);
    /*
     * Replace the back segment with a new segment at the front of the
     * tube in memory. Update the current end position of the tube in
     * memory.
     */
    endRing = (tube.currentRing + SEGMENTS) % (SEGMENTS + 1);
    MoveToPosition(tube.endPos, &tube.endP, &tube.endD, &tube.endN);
    /*
     * Update the execute buffer with the new vertices and triangles.
     */
    RingOffset = sizeof(VERTEX) * tube.currentRing * SIDES;
    SegmentOffset = sizeof(D3DTRIANGLE) * tube.currentSegment * SIDES * 2;
    /*
     * Update the position of the back of the tube in memory and texture
     * counter.
     */
    tube.currentRing = (tube.currentRing + 1) % (SEGMENTS + 1);
    tube.currentSegment = (tube.currentSegment + 1) % SEGMENTS;
    texRing = (texRing + 1) % TEX_RINGS;
}

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

    CTunnel* pTunnel;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTunnel = new CTunnel();
    if (!pTunnel) {
        return FALSE;
    }

    // Initialize the scene
    if (!pTunnel->Create(pDisplay)) {
        pTunnel->Release();
        return FALSE;
    }

    bRet = pTunnel->Exhibit(pnExitCode);

    // Clean up the scene
    pTunnel->Release();

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
// CTunnel
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CTunnel
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
CTunnel::CTunnel() {

    memset(&tube, 0, sizeof(tube));
#ifdef CRASH
    m_uMaxFrames = 200 + (rand() % 100);
#endif
    m_bFade = TRUE;
}

//******************************************************************************
//
// Method:
//
//     ~CTunnel
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
CTunnel::~CTunnel() {

    if (tube.lpPoints)
        free(tube.lpPoints);
//    if (tube.lpTri)
//        free(tube.lpTri);
    if (tube.lpV)
        MemFree32(tube.lpV);
}

//******************************************************************************
//
// Method:
//
//     Create
//
// Description:
//
//     Prepare the test for rendering.
//
// Arguments:
//
//     CDisplay* pDisplay               - Pointer to a Display object.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CTunnel::Create(CDisplay* pDisplay) {

    return CScene::Create(pDisplay);
}

//******************************************************************************
//
// Method:
//
//     Exhibit
//
// Description:
//
//     Execute the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     The exit value contained in the wParam parameter of the WM_QUIT message.
//
//******************************************************************************
int CTunnel::Exhibit(int *pnExitCode) {

    return CScene::Exhibit(pnExitCode);
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
BOOL CTunnel::Prepare() {

    float position;               /* Curve position counter. */
    int   i;                      /* counter */

    /*
     * Reserved memory for vertices, triangles and spline points.
     */
    tube.lpV = (VERTEX*) MemAlloc32(sizeof(VERTEX) * NUM_V);
    if (!tube.lpV) {
        return FALSE;
    }
//    tube.lpTri = (LPD3DTRIANGLE) malloc(sizeof(D3DTRIANGLE) * NUM_TRI);
    tube.lpPoints = (LPD3DVECTOR) malloc(sizeof(D3DVECTOR)*SPLINE_POINTS);
    if (!tube.lpPoints) {
        return FALSE;
    }
    tube.pwIndexList = (LPWORD) malloc(sizeof(WORD) * NUM_TRI * 3);
    if (!tube.pwIndexList) {
        return FALSE;
    }
    /*
     * Generate spline points
     */
    for (i = 0; i < SPLINE_POINTS; i++) {
        tube.lpPoints[i].x = (float)(cos(i * 4.0) * 20.0);
        tube.lpPoints[i].y = (float)(sin(i * 4.0) * 20.0);
        tube.lpPoints[i].z = i * 20.0f;
    }
    /*
     * Create the initial tube section in memory.
     */
    tube.endN.x = (float)0.0f;
    tube.endN.y = (float)1.0f;
    tube.endN.z = (float)0.0f;
    position = (float)0.0f;
    for (i = 0; i < SEGMENTS + 1; i++) {
        MoveToPosition(position, &tube.endP, &tube.endD, &tube.endN);
        position += (float)SEGMENT_LENGTH;
        MakeRing(tube.endP, tube.endD, tube.endN, 
                 (float)(i % TEX_RINGS) / TEX_RINGS,
                 &tube.lpV[(SEGMENTS - i) * SIDES]);
    }
    for (i = 0; i < SEGMENTS; i++)
        MakeSegment(i + 1, i, &tube.pwIndexList[i * SIDES * 6]);
//        MakeSegment(i + 1, i, &tube.lpTri[i * SIDES * 2]);
    /*
     * Move the camera to the begining and set some globals
     */
    tube.cameraN.x = (float)0.0;
    tube.cameraN.y = (float)1.0;
    tube.cameraN.z = (float)0.0;
    MoveToPosition((float)0.0, &tube.cameraP, &tube.cameraD, &tube.cameraN);
    tube.currentRing = 0;
    tube.currentSegment = 0;
    tube.cameraPos = (float)0.0;
    tube.endPos = position;

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     Setup
//
// Description:
//
//     Obtain the device interface pointer from the display, save the current
//     state of the device, and initialize the background vertices to the
//     dimensions of the render target.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the scene was successfully prepared for initialization, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CTunnel::Setup() {

    return CScene::Setup();
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
BOOL CTunnel::Initialize() {

    D3DXMATRIX mView, mProj;

    /*
     * Set the view, projection and world matrices
     */
    InitMatrix(&mView,
        D3DVAL( 1.0), D3DVAL( 0.0), D3DVAL( 0.0), D3DVAL( 0.0),
        D3DVAL( 0.0), D3DVAL( 1.0), D3DVAL( 0.0), D3DVAL( 0.0),
        D3DVAL( 0.0), D3DVAL( 0.0), D3DVAL( 1.0), D3DVAL( 0.0),
        D3DVAL( 0.0), D3DVAL( 0.0), D3DVAL(10.0), D3DVAL( 1.0)
    );
    m_pDevice->SetTransform(D3DTS_VIEW, &mView);

    InitMatrix(&mProj,
        D3DVAL( 2.0), D3DVAL( 0.0), D3DVAL( 0.0), D3DVAL( 0.0),
        D3DVAL( 0.0), D3DVAL( 2.0), D3DVAL( 0.0), D3DVAL( 0.0),
        D3DVAL( 0.0), D3DVAL( 0.0), D3DVAL( 1.0), D3DVAL( 1.0),
        D3DVAL( 0.0), D3DVAL( 0.0), D3DVAL(-1.0), D3DVAL( 0.0)
    );
    m_pDevice->SetTransform(D3DTS_PROJECTION, &mProj);

    /*
     * Setup materials and lights
     */

    if (!SetMaterial(m_pDevice, RGB_MAKE(255, 255, 255), RGB_MAKE(255, 255, 255), RGB_MAKE(255, 255, 255), 0, 20.0f)) {
        return FALSE;
    }

    if (!SetPointLight(m_pDevice, 0, tube.cameraP, RGB_MAKE(230, 230, 230))) { //, 0.0f, 0.0f, 0.05f);
        return FALSE;
    }

    SetPointLight(&tube.light, tube.cameraP, RGB_MAKE(230, 230, 230)); //, 0.0f, 0.0f, 0.05f);

    // Create the texture
    m_pd3dt = (CTexture8*)CreateTexture(m_pDevice, TEXT("checker.bmp"), D3DFMT_A1R5G5B5);
    if (!m_pd3dt) {
        return FALSE;
    }

#ifdef CRASH

    m_pd3dtErrorBox = (CTexture8*)CreateTexture(m_pDevice, TEXT("errorbox.bmp"), D3DFMT_A8R8G8B8);
    if (!m_pd3dtErrorBox) {
        return FALSE;
    }

    // Create the error dialog
    m_ptlrErrorBox[0] = TLVERTEX(D3DXVECTOR3(106.667f, 325.333f, 0.00002f), 90000.0f, RGBA_MAKE(255,255,255,255), 0, 0.0f, 1.0f);
    m_ptlrErrorBox[1] = TLVERTEX(D3DXVECTOR3(106.667f, 154.667f, 0.00002f), 90000.0f, RGBA_MAKE(255,255,255,255), 0, 0.0f, 0.0f);
    m_ptlrErrorBox[2] = TLVERTEX(D3DXVECTOR3(533.333f, 154.667f, 0.00002f), 90000.0f, RGBA_MAKE(255,255,255,255), 0, 1.0f, 0.0f);
    m_ptlrErrorBox[3] = TLVERTEX(D3DXVECTOR3(533.333f, 325.333f, 0.00002f), 90000.0f, RGBA_MAKE(255,255,255,255), 0, 1.0f, 1.0f);

#endif // CRASH

    m_pDevice->SetRenderState(D3DRS_WRAP0, (DWORD)(D3DWRAP_U | D3DWRAP_V));

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
void CTunnel::Efface() {

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
void CTunnel::Update() {

#ifdef CRASH
    if (m_fFrame < (float)m_uMaxFrames) {
#endif // CRASH

    /*
     * Move the camera through the tunnel.  Create new segments of the tunnel
     * when the camera gets close to the end of the section in memory.
     */

    /*
     * Update the position on curve and camera vectors.
     */
    tube.cameraPos += (float)SPEED;
    if (tube.cameraPos > PATH_LENGTH)
        tube.cameraPos -= PATH_LENGTH;
    MoveToPosition(tube.cameraPos, &tube.cameraP, &tube.cameraD,
                   &tube.cameraN);
    /*
     * If the camera is close to the end, add a new segment.
     */
    if (tube.endPos - tube.cameraPos < DEPTH) {
        tube.endPos = tube.endPos + (float)SEGMENT_LENGTH;
        if (tube.endPos > PATH_LENGTH)
            tube.endPos -= PATH_LENGTH;
        UpdateTubeInMemory();
    }

#ifdef CRASH
    }

    if ((UINT)m_fFrame == m_uMaxFrames) {
        OutputDebugString(TEXT("0x81fca97c: Exception 060 Thread=81fca97c Proc=21ed809e 'tunnel.exe'\n")
                     TEXT("0x81fca97c: AKY=00000101 PC=01f8d78c RA=01f88e7a TEA=352c6d90\n")
                     TEXT("First chance exception c0000005 (Access Violation) occurred\n")
                     TEXT("Thread stopped.\n")
                     TEXT("Module Load: C:\\WINCE\\RELEASE\\DDHAL.pdb  (symbols loaded)"));
    }
#endif // CRASH
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
BOOL CTunnel::Render() {

    D3DXMATRIX  mView;
    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    HRESULT     hr;

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

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VERTEX);

    /*
     * Move the camera by updating the view matrix and move the light.
     */
    PositionCamera(&tube.cameraP, &tube.cameraD, &tube.cameraN, &mView);
    m_pDevice->SetTransform(D3DTS_VIEW, &mView);

    tube.light.Position = tube.cameraP;
    m_pDevice->SetLight(0, &tube.light);

    m_pDevice->SetTexture(0, m_pd3dt);

    // Draw the tunnel
    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, NUM_V, NUM_TRI, 
                                            tube.pwIndexList, D3DFMT_INDEX16, 
                                            tube.lpV, sizeof(VERTEX));

#ifdef CRASH
    if (m_fFrame >= (float)m_uMaxFrames) {

        m_pDevice->SetRenderState(D3DRS_WRAP0, 0);

        m_pDevice->SetTexture(0, m_pd3dtErrorBox);

        m_pDevice->SetVertexShader(FVF_TLVERTEX);

        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2,
                m_ptlrErrorBox, sizeof(TLVERTEX));

        m_pDevice->SetRenderState(D3DRS_WRAP0, (DWORD)(D3DWRAP_U | D3DWRAP_V));
    }
#endif // CRASH

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

    return TRUE;
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
void CTunnel::ProcessInput() {

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
BOOL CTunnel::InitView() {

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
BOOL CTunnel::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
