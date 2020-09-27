#include "pchshape.h"


struct VERTEX
{
    D3DXVECTOR3 pos;
    D3DXVECTOR3 norm;
};

#define VERTEX_FVF (D3DFVF_XYZ | D3DFVF_NORMAL)



//
// sincosf - Compute the sin and cos of an angle at the same time
//

static inline void
sincosf(float angle, float *psin, float *pcos)
{
#ifdef _X86_
#define fsincos __asm _emit 0xd9 __asm _emit 0xfb
    __asm {
        mov eax, psin
        mov edx, pcos
        fld angle
        fsincos
        fstp DWORD ptr [edx]
        fstp DWORD ptr [eax]
    }
#undef fsincos
#else //!_X86_
    *psin = sinf(angle);
    *pcos = cosf(angle);
#endif //!_X86_
}



//----------------------------------------------------------------------------
// D3DXCreatePolygon
//----------------------------------------------------------------------------
static void
    MakePolygon(
        VERTEX*             pVertices, 
        WORD*               pwIndices, 
        float               fLength, 
        UINT                uSides)
{
    // Calculate the radius
    float radius = fLength * 0.5f / sinf( D3DX_PI / (float) uSides );
    float angle  = (float) (2.0f * D3DX_PI / (float) uSides);

    // Fill in vertices
    VERTEX *pVertex = pVertices;

    pVertex->pos  = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    pVertex->norm = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    pVertex++;

    for(UINT j=0; j < uSides; j++)
    {
        float s, c;
        sincosf(angle * j, &s, &c);

        pVertex->pos  = D3DXVECTOR3( c * radius, s * radius, 0.0f );
        pVertex->norm = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
        pVertex++;
    }

    // Fill in indices
    WORD *pwFace = pwIndices;

    for(UINT iFace = 0; iFace < uSides - 1; iFace++)
    {
        pwFace[0] = 0;
        pwFace[1] = iFace + 1;
        pwFace[2] = iFace + 2;

        pwFace += 3;
    }

    // handle the wrapping of the last case
    pwFace[0] = 0;
    pwFace[1] = iFace + 1;
    pwFace[2] = 1;
}


HRESULT WINAPI
    D3DXCreatePolygon( 
        LPDIRECT3DDEVICE8   pDevice, 
        float               fLength, 
        UINT                uSides, 
        LPD3DXMESH*         ppMesh,
        LPD3DXBUFFER*       ppAdjacency)
{
    HRESULT hr;

    LPD3DXMESH pMesh  = NULL;
    WORD *pwIndices   = NULL;
    VERTEX *pVertices = NULL;


    // Set up the defaults
    if(D3DX_DEFAULT == uSides)
        uSides = 3;
    if(D3DX_DEFAULT_FLOAT == fLength)
        fLength = 1.0f;


    // Validate parameters
    if(!pDevice) 
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!ppMesh)
    {
        DPF(0, "ppMesh pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(fLength < 0.0f)
    {
        DPF(0, "fLength must be >= 0.0f");
        return D3DERR_INVALIDCALL;
    }
    if(uSides < 3)
    {
        DPF(0, "uSides must be >= 3");
        return D3DERR_INVALIDCALL;
    }


    // Create the mesh
    UINT  cFaces    = uSides;
    UINT  cVertices = uSides + 1;

    if(FAILED(hr = D3DXCreateMeshFVF(cFaces, cVertices, D3DXMESH_MANAGED, VERTEX_FVF, pDevice, &pMesh)))
        return hr;

    pMesh->LockVertexBuffer(0, (LPBYTE *) &pVertices);
    pMesh->LockIndexBuffer (0, (LPBYTE *) &pwIndices);

    MakePolygon(pVertices, pwIndices, fLength, uSides);

    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();   
    
    if (ppAdjacency != NULL)
    {
        hr = D3DXCreateBuffer(cFaces * 3 * sizeof(DWORD), ppAdjacency);
        if (FAILED(hr))
        {
            RELEASE(pMesh);
            return hr;
        }

        hr = pMesh->ConvertPointRepsToAdjacency(NULL, (DWORD*)(*ppAdjacency)->GetBufferPointer());
        if (FAILED(hr))
        {
            RELEASE(pMesh);
            return hr;
        }
    }

    *ppMesh = pMesh;
    return S_OK;
}



//----------------------------------------------------------------------------
// D3DXCreateBox
//----------------------------------------------------------------------------
static float cubeN[6][3] = 
{
    {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
    {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}
};

static WORD cubeF[6][4] = 
{
    { 0, 1, 5, 4 }, { 4, 5, 6, 7 }, { 7, 6, 2, 3 },
    { 1, 0, 3, 2 }, { 1, 2, 6, 5 }, { 0, 4, 7, 3 }
};

static float cubeV[8][3] = 
{
    // Lower tier (lower in y)
    {-.5f, -.5f, -.5f}, 
    {-.5f, -.5f,  .5f}, 
    { .5f, -.5f,  .5f}, 
    { .5f, -.5f, -.5f}, 
    
    // Upper tier
    {-.5f, .5f, -.5f}, 
    {-.5f, .5f,  .5f}, 
    { .5f, .5f,  .5f}, 
    { .5f, .5f, -.5f}, 
};

static float cubeT[4][2] = 
{
    // Lower tier (lower in y)
    {0.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    {1.0f, 0.0f}
};

static WORD cubeFT[6][4] = 
{
    { 3, 0, 1, 2 }, { 0, 1, 2, 3 }, { 1, 2, 3, 0 },
    { 0, 1, 2, 3 }, { 3, 0, 1, 2 }, { 0, 1, 2, 3 }
};


static void
    MakeBox(
        VERTEX*             pVertices, 
        DWORD*              pPointRep,
        WORD*               pwIndices, 
        float               fWidth, 
        float               fHeight, 
        float               fDepth)
{
    // Fill in the data
    VERTEX *pVertex = pVertices;
    WORD   *pwFace  = pwIndices;
    UINT    iVertex = 0;

    // i iterates over the faces, 2 triangles per face
    for (int i=0; i<6; i++)
    {
        for (int j=0; j<4; j++)
        {
            pVertex->pos.x = cubeV[cubeF[i][j]][0] * fWidth;
            pVertex->pos.y = cubeV[cubeF[i][j]][1] * fHeight;
            pVertex->pos.z = cubeV[cubeF[i][j]][2] * fDepth;

            pVertex->norm.x = cubeN[i][0];
            pVertex->norm.y = cubeN[i][1];
            pVertex->norm.z = cubeN[i][2];

            if (pPointRep != NULL)
            {
                *pPointRep = cubeF[i][j];
                pPointRep++;
            }

            pVertex++;
        }

        pwFace[0] = (WORD) (iVertex);
        pwFace[1] = (WORD) (iVertex + 1);
        pwFace[2] = (WORD) (iVertex + 2);
        pwFace += 3;

        pwFace[0] = (WORD) (iVertex + 2);
        pwFace[1] = (WORD) (iVertex + 3);
        pwFace[2] = (WORD) (iVertex);
        pwFace += 3;

        iVertex += 4;
    }
}


HRESULT WINAPI
    D3DXCreateBox(
        LPDIRECT3DDEVICE8   pDevice, 
        float               fWidth, 
        float               fHeight, 
        float               fDepth, 
        LPD3DXMESH*         ppMesh,
        LPD3DXBUFFER*       ppAdjacency)
{
    HRESULT hr = S_OK;

    LPD3DXMESH pMesh  = NULL;
    WORD *pwIndices   = NULL;
    VERTEX *pVertices = NULL;
    DWORD *pPointReps = NULL;

    // Set up the defaults
    if(D3DX_DEFAULT_FLOAT == fWidth)
        fWidth = 1.0f;
    if(D3DX_DEFAULT_FLOAT == fHeight)
        fHeight = 1.0f;
    if(D3DX_DEFAULT_FLOAT == fDepth)
        fDepth = 1.0f;


    // Validate parameters
    if(!pDevice) 
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!ppMesh)
    {
        DPF(0, "ppMesh pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(fWidth < 0.0f)
    {
        DPF(0, "fWidth must be >= 0.0f");
        return D3DERR_INVALIDCALL;
    }
    if(fHeight < 0.0f)
    {
        DPF(0, "fHeight must be >= 0.0f");
        return D3DERR_INVALIDCALL;
    }
    if(fDepth < 0.0f)
    {
        DPF(0, "fDepth must be >= 0.0f");
        return D3DERR_INVALIDCALL;
    }


    // Create the mesh
    UINT  cFaces    = 12;
    UINT  cVertices = 24;

    if(FAILED(hr = D3DXCreateMeshFVF(cFaces, cVertices, D3DXMESH_MANAGED, VERTEX_FVF, pDevice, &pMesh)))
        goto e_Exit;

    pMesh->LockVertexBuffer(0, (LPBYTE *) &pVertices);
    pMesh->LockIndexBuffer (0, (LPBYTE *) &pwIndices);

    if (ppAdjacency != NULL)
    {
        pPointReps = new DWORD[cVertices];
        if (pPointReps == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
    }

    MakeBox(pVertices, pPointReps, pwIndices, fWidth, fHeight, fDepth);

    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();

    if (ppAdjacency != NULL)
    {
        hr = D3DXCreateBuffer(cFaces * 3 * sizeof(DWORD), ppAdjacency);
        if (FAILED(hr))
            goto e_Exit;

        hr = pMesh->ConvertPointRepsToAdjacency(pPointReps, (DWORD*)(*ppAdjacency)->GetBufferPointer());
        if (FAILED(hr))
            goto e_Exit;
    }

    *ppMesh = pMesh;
    pMesh = NULL;

e_Exit:
    delete []pPointReps;
    RELEASE(pMesh);
    return hr;
}


//----------------------------------------------------------------------------
// D3DXCreateCylinder
//----------------------------------------------------------------------------
#define CACHE_SIZE    240

static void
    MakeCylinder(
        VERTEX*             pVertices, 
        DWORD*              pPointReps,
        WORD*               pwIndices, 
        float               fRadius1, 
        float               fRadius2, 
        float               fLength, 
        UINT                uSlices, 
        UINT                uStacks)
{
    UINT i, j;

    // Sin/Cos caches
    float sinI[CACHE_SIZE], cosI[CACHE_SIZE];

    for(i = 0; i < uSlices; i++) 
        sincosf(2.0f * D3DX_PI * i / uSlices, sinI + i, cosI + i);


    // Compute side normal angle
    float fDeltaRadius = fRadius2 - fRadius1;
    float fSideLength = sqrtf(fDeltaRadius * fDeltaRadius + fLength * fLength);

    float fNormalXY = (fSideLength > 0.00001f) ? (fLength / fSideLength)       : 1.0f;
    float fNormalZ  = (fSideLength > 0.00001f) ? (-fDeltaRadius / fSideLength) : 0.0f;



    // Generate vertices
    VERTEX *pVertex = pVertices;
    float fZ, fRadius;
    DWORD iVertex;

    // Base cap (uSlices + 1)
    fZ = fLength * -0.5f;
    fRadius = fRadius1;
    iVertex = 0;

    pVertex->pos  = D3DXVECTOR3(0.0f, 0.0f, fZ);
    pVertex->norm = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
    pVertex++;
    if (pPointReps != NULL)
        pPointReps[iVertex] = iVertex;
    iVertex++;

    for(i = 0; i < uSlices; i++)
    {
        pVertex->pos  = D3DXVECTOR3(fRadius * sinI[i], fRadius * cosI[i], fZ);
        pVertex->norm = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
        pVertex++;
        
        // link into stack vertices, which follow
        if (pPointReps != NULL)
            pPointReps[iVertex] = iVertex + uSlices;
        iVertex++;
    }

    // Stacks ((uStacks + 1)*uSlices)
    for(j = 0; j <= uStacks; j++)
    {
        float f = (float) j / (float) uStacks;

        fZ = fLength * (f - 0.5f);
        fRadius = fRadius1 + f * fDeltaRadius;

        for(i = 0; i < uSlices; i++)
        {
            pVertex->pos  = D3DXVECTOR3(fRadius * sinI[i], fRadius * cosI[i], fZ);
            pVertex->norm = D3DXVECTOR3(fNormalXY * sinI[i], fNormalXY * cosI[i], fNormalZ);
            pVertex++;
            if (pPointReps != NULL)
                pPointReps[iVertex] = iVertex;
            iVertex++;
        }
    }

    // Top cap (uSlices + 1)
    fZ = fLength * 0.5f;
    fRadius = fRadius2;

    for(i = 0; i < uSlices; i++)
    {
        pVertex->pos  = D3DXVECTOR3(fRadius * sinI[i], fRadius * cosI[i], fZ);
        pVertex->norm = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
        pVertex++;

        // link into stack vertices, which precede
        if (pPointReps != NULL)
            pPointReps[iVertex] = iVertex - uSlices;
        iVertex++;
    }

    pVertex->pos  = D3DXVECTOR3(0.0f, 0.0f, fZ);
    pVertex->norm = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    pVertex++;
    if (pPointReps != NULL)
        pPointReps[iVertex] = iVertex;
    iVertex++;



    // Generate indices
    WORD *pwFace = pwIndices;
    UINT uRowA, uRowB;

    // Z+ pole (uSlices)
    uRowA = 0;
    uRowB = 1;

    for(i = 0; i < uSlices - 1; i++)
    {
        pwFace[0] = (WORD) (uRowA);
        pwFace[1] = (WORD) (uRowB + i);
        pwFace[2] = (WORD) (uRowB + i + 1);
        pwFace += 3;
    }

    pwFace[0] = (WORD) (uRowA);
    pwFace[1] = (WORD) (uRowB + i);
    pwFace[2] = (WORD) (uRowB);
    pwFace += 3;

    // Interior stacks (uStacks * uSlices * 2)
    for(j = 0; j < uStacks; j++)
    {
        uRowA = 1 + (j + 1) * uSlices;
        uRowB = uRowA + uSlices;

        for(i = 0; i < uSlices - 1; i++)
        {
            pwFace[0] = (WORD) (uRowA + i);
            pwFace[1] = (WORD) (uRowB + i);
            pwFace[2] = (WORD) (uRowA + i + 1);
            pwFace += 3;

            pwFace[0] = (WORD) (uRowA + i + 1);
            pwFace[1] = (WORD) (uRowB + i);
            pwFace[2] = (WORD) (uRowB + i + 1);
            pwFace += 3;
        }

        pwFace[0] = (WORD) (uRowA + i);
        pwFace[1] = (WORD) (uRowB + i);
        pwFace[2] = (WORD) (uRowA);
        pwFace += 3;

        pwFace[0] = (WORD) (uRowA);
        pwFace[1] = (WORD) (uRowB + i);
        pwFace[2] = (WORD) (uRowB);
        pwFace += 3;
    }

    // Z- pole (uSlices)
    uRowA = 1 + (uStacks + 2) * uSlices;
    uRowB = uRowA + uSlices;

    for(i = 0; i < uSlices - 1; i++)
    {
        pwFace[0] = (WORD) (uRowA + i);
        pwFace[1] = (WORD) (uRowB);
        pwFace[2] = (WORD) (uRowA + i + 1);
        pwFace += 3;
    }

    pwFace[0] = (WORD) (uRowA + i);
    pwFace[1] = (WORD) (uRowB);
    pwFace[2] = (WORD) (uRowA);
    pwFace += 3;
}
        

HRESULT WINAPI
    D3DXCreateCylinder(
        LPDIRECT3DDEVICE8   pDevice, 
        float               fRadius1, 
        float               fRadius2, 
        float               fLength, 
        UINT                uSlices, 
        UINT                uStacks,   
        LPD3DXMESH*         ppMesh,
        LPD3DXBUFFER*       ppAdjacency)
{
    HRESULT hr;

    LPD3DXMESH pMesh  = NULL;
    WORD *pwIndices   = NULL;
    VERTEX *pVertices = NULL;
    DWORD *pPointReps = NULL;

    // Set up the defaults
    if(D3DX_DEFAULT_FLOAT == fRadius1)
        fRadius1 = 1.0f;
    if(D3DX_DEFAULT_FLOAT == fRadius2)
        fRadius2 = 1.0f;
    if(D3DX_DEFAULT_FLOAT == fLength)
        fLength = 1.0f;
    if(D3DX_DEFAULT == uSlices)
        uSlices = 8;
    if(D3DX_DEFAULT == uStacks)
        uStacks = 8;


    // Validate parameters
    if(!pDevice) 
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!ppMesh)
    {
        DPF(0, "ppMesh pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(fRadius1 < 0.0f)
    {
        DPF(0, "fRadius1 must be >= 0.0f");
        return D3DERR_INVALIDCALL;
    }
    if(fRadius2 < 0.0f)
    {
        DPF(0, "fRadius2 must be >= 0.0f");
        return D3DERR_INVALIDCALL;
    }
    if(fLength < 0.0f)
    {
        DPF(0, "fLength must be >= 0.0f");
        return D3DERR_INVALIDCALL;
    }
    if(uSlices < 2)
    {
        DPF(0, "uSlices must be >= 2");
        return D3DERR_INVALIDCALL;
    }
    if(uStacks < 1)
    {
        DPF(0, "uStacks must be >= 1");
        return D3DERR_INVALIDCALL;
    }
    if(uSlices >= CACHE_SIZE) 
        uSlices = CACHE_SIZE-1;



    // Create the mesh
    UINT  cFaces    = (uStacks + 1) * uSlices * 2;
    UINT  cVertices = 2 + (uStacks + 3) * uSlices;

    if(FAILED(hr = D3DXCreateMeshFVF(cFaces, cVertices, D3DXMESH_MANAGED, VERTEX_FVF, pDevice, &pMesh)))
        return hr;

    pMesh->LockVertexBuffer(0, (LPBYTE *) &pVertices);
    pMesh->LockIndexBuffer (0, (LPBYTE *) &pwIndices);

    if (ppAdjacency != NULL)
    {
        pPointReps = new DWORD[cVertices];
        if (pPointReps == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
    }

    MakeCylinder(pVertices, pPointReps, pwIndices, fRadius1, fRadius2,
        fLength, uSlices, uStacks);

    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();

    if (ppAdjacency != NULL)
    {
        hr = D3DXCreateBuffer(cFaces * 3 * sizeof(DWORD), ppAdjacency);
        if (FAILED(hr))
            goto e_Exit;

        hr = pMesh->ConvertPointRepsToAdjacency(pPointReps, (DWORD*)(*ppAdjacency)->GetBufferPointer());
        if (FAILED(hr))
            goto e_Exit;
    }

    *ppMesh = pMesh;
    pMesh = NULL;

e_Exit:
    RELEASE(pMesh);
    delete []pPointReps;
    return hr;
}



//---------------------------------------------------------------------
// D3DXCreateSphere
//---------------------------------------------------------------------
static void
    MakeSphere( 
        VERTEX*             pVertices, 
        WORD*               pwIndices, 
        float               fRadius, 
        UINT                uSlices,
        UINT                uStacks)
{
    UINT i,j;


    // Sin/Cos caches
    float sinI[CACHE_SIZE], cosI[CACHE_SIZE];
    float sinJ[CACHE_SIZE], cosJ[CACHE_SIZE];

    for(i = 0; i < uSlices; i++) 
        sincosf(2.0f * D3DX_PI * i / uSlices, sinI + i, cosI + i);

    for(j = 0; j < uStacks; j++) 
        sincosf(D3DX_PI * j / uStacks, sinJ + j, cosJ + j);



    // Generate vertices
    VERTEX *pVertex = pVertices;

    // +Z pole
    pVertex->pos  = D3DXVECTOR3(0.0f, 0.0f, fRadius);
    pVertex->norm = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    pVertex++;

    // Stacks
    for(j = 1; j < uStacks; j++) 
    {
        for(i = 0; i < uSlices; i++)
        {
            D3DXVECTOR3 norm(sinI[i] * sinJ[j], cosI[i] * sinJ[j], cosJ[j]);

            pVertex->pos  = norm * fRadius;
            pVertex->norm = norm;

            pVertex++;
        }
    }

    // Z- pole
    pVertex->pos  = D3DXVECTOR3(0.0f, 0.0f, -fRadius);
    pVertex->norm = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
    pVertex++;



    // Generate indices
    WORD *pwFace = pwIndices;
    UINT uRowA, uRowB;

    // Z+ pole
    uRowA = 0;
    uRowB = 1;

    for(i = 0; i < uSlices - 1; i++)
    {
        pwFace[0] = (WORD) (uRowA);
        pwFace[1] = (WORD) (uRowB + i + 1);
        pwFace[2] = (WORD) (uRowB + i);
        pwFace += 3;
    }

    pwFace[0] = (WORD) (uRowA);
    pwFace[1] = (WORD) (uRowB);
    pwFace[2] = (WORD) (uRowB + i);
    pwFace += 3;

    // Interior stacks
    for(j = 1; j < uStacks - 1; j++)
    {
        uRowA = 1 + (j - 1) * uSlices;
        uRowB = uRowA + uSlices;

        for(i = 0; i < uSlices - 1; i++)
        {
            pwFace[0] = (WORD) (uRowA + i);
            pwFace[1] = (WORD) (uRowA + i + 1);
            pwFace[2] = (WORD) (uRowB + i);
            pwFace += 3;

            pwFace[0] = (WORD) (uRowA + i + 1);
            pwFace[1] = (WORD) (uRowB + i + 1);
            pwFace[2] = (WORD) (uRowB + i);
            pwFace += 3;
        }

        pwFace[0] = (WORD) (uRowA + i);
        pwFace[1] = (WORD) (uRowA);
        pwFace[2] = (WORD) (uRowB + i);
        pwFace += 3;

        pwFace[0] = (WORD) (uRowA);
        pwFace[1] = (WORD) (uRowB);
        pwFace[2] = (WORD) (uRowB + i);
        pwFace += 3;
    }

    // Z- pole
    uRowA = 1 + (uStacks - 2) * uSlices;
    uRowB = uRowA + uSlices;

    for(i = 0; i < uSlices - 1; i++)
    {
        pwFace[0] = (WORD) (uRowA + i);
        pwFace[1] = (WORD) (uRowA + i + 1);
        pwFace[2] = (WORD) (uRowB);
        pwFace += 3;
    }

    pwFace[0] = (WORD) (uRowA + i);
    pwFace[1] = (WORD) (uRowA);
    pwFace[2] = (WORD) (uRowB);
    pwFace += 3;
}


HRESULT WINAPI
    D3DXCreateSphere(
        LPDIRECT3DDEVICE8   pDevice, 
        float               fRadius, 
        UINT                uSlices, 
        UINT                uStacks,
        LPD3DXMESH*         ppMesh,
        LPD3DXBUFFER*       ppAdjacency)
{
    HRESULT hr;

    LPD3DXMESH pMesh  = NULL;
    WORD *pwIndices   = NULL;
    VERTEX *pVertices = NULL;

    // Set up the defaults
    if(D3DX_DEFAULT_FLOAT == fRadius)
        fRadius = 1.0f;
    if(D3DX_DEFAULT == uSlices)
        uSlices = 8;
    if(D3DX_DEFAULT == uStacks)
        uStacks = 8;

    // Validate parameters
    if(!pDevice) 
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!ppMesh)
    {
        DPF(0, "ppMesh pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(fRadius < 0.0f)
    {
        DPF(0, "fRadius must be >= 0.0f");
        return D3DERR_INVALIDCALL;
    }
    if(uSlices < 2)
    {
        DPF(0, "uSlices must be >= 2");
        return D3DERR_INVALIDCALL;
    }
    if(uStacks < 2)
    {
        DPF(0, "uStacks must be >= 2");
        return D3DERR_INVALIDCALL;
    }

    if(uSlices > CACHE_SIZE)
        uSlices = CACHE_SIZE;
    if(uStacks > CACHE_SIZE)
        uStacks = CACHE_SIZE;


    // Create the mesh
    UINT  cFaces    = 2 * (uStacks - 1) * uSlices;
    UINT  cVertices = (uStacks - 1) * uSlices + 2;

    if(FAILED(hr = D3DXCreateMeshFVF(cFaces, cVertices, D3DXMESH_MANAGED, VERTEX_FVF, pDevice, &pMesh)))
        return hr;

    pMesh->LockVertexBuffer(0, (LPBYTE *) &pVertices);
    pMesh->LockIndexBuffer (0, (LPBYTE *) &pwIndices);

    MakeSphere(pVertices, pwIndices, fRadius, uSlices, uStacks);

    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();

    if (ppAdjacency != NULL)
    {
        hr = D3DXCreateBuffer(cFaces * 3 * sizeof(DWORD), ppAdjacency);
        if (FAILED(hr))
        {
            RELEASE(pMesh);
            return hr;
        }

        hr = pMesh->ConvertPointRepsToAdjacency(NULL, (DWORD*)(*ppAdjacency)->GetBufferPointer());
        if (FAILED(hr))
        {
            RELEASE(pMesh);
            return hr;
        }
    }

    *ppMesh = pMesh;
    return S_OK;
}



//---------------------------------------------------------------------
// D3DXCreateTorus
//---------------------------------------------------------------------
static void
    MakeTorus( 
        VERTEX*             pVertices, 
        WORD*               pwIndices, 
        float               fInnerRadius, 
        float               fOuterRadius, 
        UINT                uSides, 
        UINT                uRings)
{
    UINT i, j;

    //
    // Compute the vertices
    //

    VERTEX *pVertex = pVertices;

    for (i = 0; i < uRings; i++) 
    {
        float theta = (float) i * 2.0f * D3DX_PI / (float)uRings;
        float st, ct;

        sincosf(theta, &st, &ct);

        for (j = 0; j < uSides; j++) 
        {
            float phi = (float) j * 2.0f * D3DX_PI / uSides;
            float sp, cp;

            sincosf(phi, &sp, &cp);

            pVertex->pos.x  =  ct * (fOuterRadius + fInnerRadius * cp);
            pVertex->pos.y  = -st * (fOuterRadius + fInnerRadius * cp);
            pVertex->pos.z  =  sp * fInnerRadius;

            pVertex->norm.x =  ct * cp;
            pVertex->norm.y = -st * cp;
            pVertex->norm.z =  sp;

            pVertex++;
        }
    }

    //
    // Compute the indices: 
    // There are uRings * uSides faces
    // Each face has 2 triangles (6 indices)
    //

    // Tube i has indices:  
    //        Left Edge: i*(uSides+1) -- i*(uSides+1)+uSides
    //        Right Edge: (i+1)*(uSides+1) -- (i+1)*(uSides+1)+uSides
    //
    // Face j on tube i has the 4 indices:
    //        Left Edge: i*(uSides+1)+j -- i*(uSides+1)+j+1
    //        Right Edge: (i+1)*(uSides+1)+j -- (i+1)*(uSides+1)+j+1
    //
    WORD *pwFace = pwIndices;

    for (i = 0; i < uRings - 1; i++) 
    {
        for (j = 0; j < uSides - 1; j++) 
        {                                

            // Tri 1 (Top-Left tri, CCW)
            pwFace[0] = (WORD) (i * uSides + j);
            pwFace[1] = (WORD) (i * uSides + j + 1);
            pwFace[2] = (WORD) ((i + 1) * uSides + j);
            pwFace += 3;

            // Tri 2 (Bottom-Right tri, CCW)
            pwFace[0] = (WORD) ((i + 1) * uSides + j);
            pwFace[1] = (WORD) (i * uSides + j + 1);
            pwFace[2] = (WORD) ((i + 1) * uSides + j + 1);
            pwFace += 3;
        }

        // Tri 1 (Top-Left tri, CCW)
        pwFace[0] = (WORD) (i * uSides + j);
        pwFace[1] = (WORD) (i * uSides);
        pwFace[2] = (WORD) ((i + 1) * uSides + j);
        pwFace += 3;

        // Tri 2 (Bottom-Right tri, CCW)
        pwFace[0] = (WORD) ((i + 1) * uSides + j);
        pwFace[1] = (WORD) (i * uSides + 0);
        pwFace[2] = (WORD) ((i + 1) * uSides + 0);
        pwFace += 3;
    }


    // join the two ends of the tube
    for (j = 0; j < uSides - 1; j++) 
    {
        // Tri 1 (Top-Left tri, CCW)
        pwFace[0] = (WORD) (i * uSides + j);
        pwFace[1] = (WORD) (i * uSides + j + 1);
        pwFace[2] = (WORD) (j);
        pwFace += 3;

        // Tri 2 (Bottom-Right tri, CCW)
        pwFace[0] = (WORD) (j);
        pwFace[1] = (WORD) (i * uSides + j + 1);
        pwFace[2] = (WORD) (j + 1);
        pwFace += 3;
    }

    // Tri 1 (Top-Left tri, CCW)
    pwFace[0] = (WORD) (i * uSides + j);
    pwFace[1] = (WORD) (i * uSides);
    pwFace[2] = (WORD) (j);
    pwFace += 3;

    // Tri 2 (Bottom-Right tri, CCW)
    pwFace[0] = (WORD) (j);
    pwFace[1] = (WORD) (i * uSides);
    pwFace[2] = (WORD) (0);
    pwFace += 3;
}


HRESULT WINAPI
    D3DXCreateTorus(
        LPDIRECT3DDEVICE8   pDevice,
        float               fInnerRadius, 
        float               fOuterRadius, 
        UINT                uSides, 
        UINT                uRings, 
        LPD3DXMESH*         ppMesh,
        LPD3DXBUFFER*       ppAdjacency)
{
    HRESULT hr;

    LPD3DXMESH pMesh  = NULL;
    WORD *pwIndices   = NULL;
    VERTEX *pVertices = NULL;


    // Set up the defaults
    if(D3DX_DEFAULT_FLOAT == fInnerRadius)
        fInnerRadius = 1.0f;
    if(D3DX_DEFAULT_FLOAT == fOuterRadius)
        fOuterRadius = 2.0f;
    if(D3DX_DEFAULT == uSides)
        uSides = 8;
    if(D3DX_DEFAULT == uRings)
        uRings = 15;

    // Validate parameters
    if(!pDevice) 
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!ppMesh)
    {
        DPF(0, "ppMesh pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(fInnerRadius < 0.0f)
    {
        DPF(0, "fInnerRadius must be >= 0.0f");
        return D3DERR_INVALIDCALL;
    }
    if(fOuterRadius < 0.0f)
    {
        DPF(0, "fOuterRadius must be >= 0.0f");
        return D3DERR_INVALIDCALL;
    }
    if(uSides < 3)
    {
        DPF(0, "uSides must be >= 3");
        return D3DERR_INVALIDCALL;
    }
    if(uRings < 3)
    {
        DPF(0, "uRings must be >= 3");
        return D3DERR_INVALIDCALL;
    }


    // Create the mesh
    UINT  cFaces    = 2 * uSides * uRings;
    UINT  cVertices = uRings * uSides;

    if(FAILED(hr = D3DXCreateMeshFVF(cFaces, cVertices, D3DXMESH_MANAGED, VERTEX_FVF, pDevice, &pMesh)))
        return hr;

    pMesh->LockVertexBuffer(0, (LPBYTE *) &pVertices);
    pMesh->LockIndexBuffer (0, (LPBYTE *) &pwIndices);

    MakeTorus(pVertices, pwIndices, fInnerRadius, fOuterRadius,
        uSides, uRings);

    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();

    if (ppAdjacency != NULL)
    {
        hr = D3DXCreateBuffer(cFaces * 3 * sizeof(DWORD), ppAdjacency);
        if (FAILED(hr))
        {
            RELEASE(pMesh);
            return hr;
        }

        hr = pMesh->ConvertPointRepsToAdjacency(NULL, (DWORD*)(*ppAdjacency)->GetBufferPointer());
        if (FAILED(hr))
        {
            RELEASE(pMesh);
            return hr;
        }
    }

    *ppMesh = pMesh;
    return S_OK;
}



//----------------------------------------------------------------------------
// D3DXCreateTeapot
//----------------------------------------------------------------------------
#include "teapot.cpp"


static void
    MakeTeapot(
        VERTEX*             pVertices,
        WORD*               pwIndices)
{
    DWORD iVertex; 

    // Copy vertices
    for (iVertex = 0; iVertex < NUMTEAPOTVERTICES; iVertex++)
    {
        pVertices[iVertex].pos = teapotPositions[iVertex];
        pVertices[iVertex].norm = teapotNormals[iVertex];
    }

    // Copy face indices
    WORD *pwFace = pwIndices;
    WORD *pwFaceLim = pwFace + NUMTEAPOTINDICES;
    WORD *pwTeapotFace = teapotIndices;

    while(pwFace < pwFaceLim)
    {
        pwFace[0] = pwTeapotFace[0];
        pwFace[1] = pwTeapotFace[1];
        pwFace[2] = pwTeapotFace[2];

        pwFace += 3;
        pwTeapotFace += 3;
    }
}


HRESULT WINAPI
    D3DXCreateTeapot(
        LPDIRECT3DDEVICE8   pDevice,
        LPD3DXMESH*         ppMesh,
        LPD3DXBUFFER*       ppAdjacency)
{
    HRESULT hr;

    LPD3DXMESH pMesh  = NULL;
    WORD *pwIndices   = NULL;
    VERTEX *pVertices = NULL;


    // Validate parameters
    if(!pDevice) 
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!ppMesh)
    {
        DPF(0, "ppMesh pointer is invalid");
        return D3DERR_INVALIDCALL;
    }


    // Create the mesh
    UINT  cFaces    = NUMTEAPOTINDICES / 3;
    UINT  cVertices = NUMTEAPOTVERTICES;

    if(FAILED(hr = D3DXCreateMeshFVF(cFaces, cVertices, D3DXMESH_MANAGED, VERTEX_FVF, pDevice, &pMesh)))
        return hr;

    pMesh->LockVertexBuffer(0, (LPBYTE *) &pVertices);
    pMesh->LockIndexBuffer (0, (LPBYTE *) &pwIndices);

    MakeTeapot(pVertices, pwIndices);

    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();

    if (ppAdjacency != NULL)
    {
        hr = D3DXCreateBuffer(cFaces * 3 * sizeof(DWORD), ppAdjacency);
        if (FAILED(hr))
        {
            RELEASE(pMesh);
            return hr;
        }

        hr = pMesh->ConvertPointRepsToAdjacency(NULL, (DWORD*)(*ppAdjacency)->GetBufferPointer());
        if (FAILED(hr))
        {
            RELEASE(pMesh);
            return hr;
        }
    }

    *ppMesh = pMesh;
    return S_OK;
}
