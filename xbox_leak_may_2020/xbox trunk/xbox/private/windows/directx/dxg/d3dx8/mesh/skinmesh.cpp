/*//////////////////////////////////////////////////////////////////////////////
//
// File: skinmesh.cpp
//
// Copyright (C) 2000 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@- 03/17/99 (anujg)     - created 
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchmesh.h"

#include "skinmesh.h"
#include "cd3dxbitfield.h"

#define D3DX_MAX_BONES 256

const DWORD CD3DXBitField256::nibbleCount[] = { 0, // 0000
                                                1, // 0001
                                                1, // 0010
                                                2, // 0011
                                                1, // 0100
                                                2, // 0101
                                                2, // 0110
                                                3, // 0111
                                                1, // 1000
                                                2, // 1001
                                                2, // 1010
                                                3, // 1011
                                                2, // 1100
                                                3, // 1101
                                                3, // 1110
                                                4};// 1111

CBone::CBone()
{
	m_numWeights = 0;
	m_pVertIndices = NULL;
	m_pWeights = NULL;
}

CBone::~CBone()
{
	delete [] m_pVertIndices;
    m_pVertIndices = NULL;
	delete [] m_pWeights;
    m_pWeights = NULL;
}

CD3DXSkinMesh::CD3DXSkinMesh()
{
	m_pBones = NULL;
	m_pMesh = NULL;
    // used by GenerateSkinnedMesh
    m_fMinWeight        = 0.0f;
    m_bChangedWeights   = true;
    m_rgfCodes          = NULL;
    m_vertInfl = NULL;
    m_vertMatId = NULL;
    m_vertMatWeight = NULL;
    m_DataValid = 0;
}

CD3DXSkinMesh::~CD3DXSkinMesh()
{
	delete [] m_pBones;
    m_pBones = NULL;
    delete [] m_vertInfl;
    m_vertInfl = NULL;
    delete [] m_vertMatId;
    m_vertMatId = NULL;
    delete [] m_vertMatWeight;
    m_vertMatWeight = NULL;
	if (m_pMesh)
    {
		m_pMesh->Release();
        m_pMesh = NULL;
    }

    if (m_rgfCodes)
        delete[] m_rgfCodes;
}

HRESULT CD3DXSkinMesh::QueryInterface(REFIID riid, LPVOID *ppv)
{
    *ppv=NULL;
    if (riid == IID_IUnknown)
        *ppv=(IUnknown*)this;
    else if (riid == IID_ID3DXSkinMesh)
        *ppv=(ID3DXSkinMesh*)this;
    else
        return E_NOINTERFACE;
    ((LPUNKNOWN)*ppv)->AddRef();

    return S_OK;
}

DWORD CD3DXSkinMesh::AddRef()
{
	return ++m_refCnt;
}

DWORD CD3DXSkinMesh::Release()
{
	if (--m_refCnt != 0)
		return m_refCnt;
	delete this;
	return 0;
}

// Common initialization
HRESULT CD3DXSkinMesh::Init(DWORD numBones)
{
    m_refCnt = 1;
    if(numBones && (m_pBones == NULL))
    {
	    m_pBones = new CBone[numBones];
	    if (m_pBones == NULL)
		    return E_OUTOFMEMORY;
    }
	m_numBones = numBones;
    
    m_vertInfl = new DWORD[m_pMesh->m_cVertices];
	if (m_vertInfl == NULL)
		return E_OUTOFMEMORY;
    memset(m_vertInfl, 0, sizeof(DWORD) * m_pMesh->m_cVertices);
    m_faceClamp = 4; // D3D limit of per face influences in non-indexed skinning in SW
    m_bMixedDevice = FALSE;
    D3DCAPS8 caps;
    HRESULT hr = m_pMesh->m_pD3DDevice->GetDeviceCaps(&caps);
    if (hr != S_OK)
        return hr;

#if 0
    D3DDEVICE_CREATION_PARAMETERS cparams;
    hr = m_pMesh->m_pD3DDevice->GetCreationParameters(&cparams);
    if (hr != S_OK)
        return hr;
    if (cparams.BehaviorFlags & (D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING))
#endif
        m_faceClamp = caps.MaxVertexBlendMatrices;

#if 0
    if (cparams.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
    {
        m_faceClamp = caps.MaxVertexBlendMatrices;
        m_bMixedDevice = TRUE;
    }
    if (m_faceClamp < 2) // minimum reqd for skinning
    {
        DPF(0, "This device does not support skinning");
        DPF(0, "Try using mixed or software vertex processing");
        return D3DXERR_SKINNINGNOTSUPPORTED;
    }
#endif 0
    return S_OK;
}

HRESULT CD3DXSkinMesh::Init(DWORD numFaces, DWORD numVertices, DWORD numBones, DWORD options, DWORD fvf, LPDIRECT3DDEVICE8 pD3DDevice)
{
    HRESULT hr;

    //TODO: remove this limitation
    if ((numFaces > UNUSED16) || (numVertices > UNUSED16) || (options & D3DXMESH_32BIT))
    {
        DPF(0, "ID3DXSkinMesh object currently does not support 32-bit indices");
        return E_NOTIMPL; 
    }
    m_pMesh = new GXTri3Mesh<tp16BitIndex>(pD3DDevice, fvf, options);
    if (m_pMesh == NULL)
        return E_OUTOFMEMORY;

    if ((hr  = m_pMesh->Resize(numFaces, numVertices)) != S_OK)
        return hr;
    return Init(numBones);
}

HRESULT CD3DXSkinMesh::Init(LPD3DXMESH pMesh, DWORD numBones)
{
    DWORD reqdOptions = D3DXMESH_SYSTEMMEM;
    if ((pMesh->GetOptions() & reqdOptions) != reqdOptions)
    {
        DPF(0, "The mesh must be created with D3DXMESH_SYSTEMMEM");
        return D3DERR_INVALIDCALL;
    }
    if (pMesh->GetOptions() & D3DXMESH_32BIT)
    {
        DPF(0, "ID3DXSkinMesh object currently does not support 32-bit indices");
        return E_NOTIMPL; 
    }

    // TODO: This is ugly. This may not work since it might not be safe to assume this is a 16-bit mesh
    m_pMesh = static_cast<GXTri3Mesh<tp16BitIndex> *>(pMesh);
    m_pMesh->AddRef();
    return Init(numBones);
}

HRESULT CD3DXSkinMesh::Init(LPD3DXMESH pMesh, LPBONE pBones, DWORD numBones)
{
    m_pBones = pBones;
    return Init(pMesh, numBones);
}

HRESULT WINAPI
D3DXCreateSkinMesh(DWORD numFaces, DWORD numVertices, DWORD numBones, DWORD options, CONST DWORD* pDeclaration, 
                   LPDIRECT3DDEVICE8 pD3D, LPD3DXSKINMESH* ppSkinMesh)
{
    DWORD fvf;
    HRESULT hr = D3DXFVFFromDeclarator(pDeclaration, &fvf);
    if (FAILED(hr))
    {
        DPF(0, "Cannot handle this declaration. Only a subset of legal declarations are allowed for this call");
        return D3DERR_INVALIDCALL;
    }
    return D3DXCreateSkinMeshFVF(numFaces, numVertices, numBones, options, fvf, pD3D, ppSkinMesh);
}

HRESULT WINAPI
D3DXCreateSkinMeshFVF(DWORD numFaces, DWORD numVertices, DWORD numBones, DWORD options, DWORD fvf, 
                      LPDIRECT3DDEVICE8 pD3D, LPD3DXSKINMESH* ppSkinMesh)
{
#if DBG
    if (numBones > D3DX_MAX_BONES)
    {
        DPF(0, "Too many bones. Cannot handle more than %d bones", D3DX_MAX_BONES);
        return D3DERR_INVALIDCALL;
    }
#endif
	CD3DXSkinMesh* pSkinMesh = new CD3DXSkinMesh;
	if (pSkinMesh == NULL)
		return E_OUTOFMEMORY;

    HRESULT hr = pSkinMesh->Init(numFaces, numVertices, numBones, options, fvf, pD3D);
	if (FAILED(hr))
	{
		DPF(0, "Could not initialize skin mesh");
		goto e_Exit;
	}

	*ppSkinMesh = pSkinMesh;
	return S_OK;

e_Exit:
	delete pSkinMesh;
	return hr;
}

//TODO: Should this copy the mesh instead of referring to it ?
HRESULT WINAPI
D3DXCreateSkinMeshFromMesh(LPD3DXMESH pMesh, DWORD numBones, LPD3DXSKINMESH* ppMesh)
{
#if DBG
    if (numBones > D3DX_MAX_BONES)
    {
        DPF(0, "Too many bones. Cannot handle more than %d bones", D3DX_MAX_BONES);
        return D3DERR_INVALIDCALL;
    }
#endif
	CD3DXSkinMesh* pSkinMesh = new CD3DXSkinMesh;
	if (pSkinMesh == NULL)
		return E_OUTOFMEMORY;

    HRESULT hr = pSkinMesh->Init(pMesh, numBones);
	if (FAILED(hr))
	{
		DPF(0, "Could not initialize skin mesh");
		goto e_Exit;
	}

	*ppMesh = pSkinMesh;
	return S_OK;

e_Exit:
	delete pSkinMesh;
	return hr;
}

HRESULT WINAPI
D3DXCreateSkinMeshFromMeshAndBones(LPD3DXMESH pMesh, DWORD numBones, LPBONE pBones, LPD3DXSKINMESH* ppMesh)
{
#if DBG
    if (numBones > D3DX_MAX_BONES)
    {
        DPF(0, "Too many bones. Cannot handle more than %d bones", D3DX_MAX_BONES);
        return D3DERR_INVALIDCALL;
    }
#endif
	CD3DXSkinMesh* pSkinMesh = new CD3DXSkinMesh;
	if (pSkinMesh == NULL)
		return E_OUTOFMEMORY;

    HRESULT hr = pSkinMesh->Init(pMesh, pBones, numBones);
	if (FAILED(hr))
	{
		DPF(0, "Could not initialize skin mesh");
		goto e_Exit;
	}

	*ppMesh = pSkinMesh;
	return S_OK;

e_Exit:
	delete pSkinMesh;
	return hr;
}

HRESULT CD3DXSkinMesh::SetBoneInfluence(DWORD boneNum, DWORD numInfluences, CONST DWORD* vertices, CONST float* weights)
{
	CBone& bone = m_pBones[boneNum];
	if (bone.m_numWeights)
	{
		D3DXASSERT(bone.m_pVertIndices);
		delete [] bone.m_pVertIndices;
		D3DXASSERT(bone.m_pWeights);
		delete [] bone.m_pWeights;
		bone.m_numWeights = 0;
	}
	bone.m_pVertIndices = new DWORD[numInfluences];
	if (bone.m_pVertIndices == NULL)
		return E_OUTOFMEMORY;
	bone.m_pWeights = new float[numInfluences];
	if (bone.m_pWeights == NULL)
	{
		delete [] bone.m_pVertIndices;
		return E_OUTOFMEMORY;
	}
	bone.m_numWeights = numInfluences;
	memcpy(bone.m_pVertIndices, vertices, sizeof(DWORD) * numInfluences);
	memcpy(bone.m_pWeights, weights, sizeof(float) * numInfluences);
    m_bChangedWeights   = true;
    m_DataValid &= ~(D3DXSM_VERTINFL | D3DXSM_FACEINFL | D3DXSM_VERTDATA);
    delete [] m_vertMatId;
    m_vertMatId = NULL;
    delete [] m_vertMatWeight;
    m_vertMatWeight = NULL;
	return S_OK;
}

HRESULT CD3DXSkinMesh::GetVertexBuffer(LPDIRECT3DVERTEXBUFFER8* ppVB)
{
	return m_pMesh->GetVertexBuffer(ppVB);
}

HRESULT CD3DXSkinMesh::GetIndexBuffer(LPDIRECT3DINDEXBUFFER8* ppIB)
{
	return m_pMesh->GetIndexBuffer(ppIB);
}

HRESULT CD3DXSkinMesh::LockVertexBuffer(DWORD flags, BYTE** ppData)
{
	return m_pMesh->LockVertexBuffer(flags, ppData);
}

HRESULT CD3DXSkinMesh::UnlockVertexBuffer()
{
	return m_pMesh->UnlockVertexBuffer();
}

HRESULT CD3DXSkinMesh::LockIndexBuffer(DWORD flags, BYTE** ppData)
{
    if (!(flags & D3DLOCK_READONLY))
        m_DataValid &= ~D3DXSM_FACEINFL;
	return m_pMesh->LockIndexBuffer(flags, ppData);
}

HRESULT CD3DXSkinMesh::UnlockIndexBuffer()
{
	return m_pMesh->UnlockIndexBuffer();
}

HRESULT CD3DXSkinMesh::LockAttributeBuffer(DWORD flags, DWORD** ppData)
{
	return m_pMesh->LockAttributeBuffer(flags, ppData);
}

HRESULT CD3DXSkinMesh::UnlockAttributeBuffer()
{
	return m_pMesh->UnlockAttributeBuffer();
}

HRESULT CD3DXSkinMesh::GetDevice(LPDIRECT3DDEVICE8 *ppDevice)
{
	return m_pMesh->GetDevice(ppDevice);
}

DWORD CD3DXSkinMesh::GetNumBones()
{
    return m_numBones;
}
HRESULT CD3DXSkinMesh::GetOriginalMesh(LPD3DXMESH* ppMesh)
{
    m_pMesh->AddRef();
    *ppMesh = m_pMesh;
    return S_OK;
}

DWORD CD3DXSkinMesh::GetNumFaces()
{
    return m_pMesh->GetNumFaces();
}

DWORD CD3DXSkinMesh::GetNumVertices()
{
    return m_pMesh->GetNumVertices();
}

DWORD CD3DXSkinMesh::GetFVF()
{
    return m_pMesh->GetFVF();
}

HRESULT CD3DXSkinMesh::GetDeclaration(DWORD Declaration[MAX_FVF_DECL_SIZE])
{
    return m_pMesh->GetDeclaration(Declaration);
}

DWORD CD3DXSkinMesh::GetOptions()
{
    return m_pMesh->GetOptions();
}

HRESULT CD3DXSkinMesh::CalcVertexSkinData()
{
    DWORD i, j;

    if (m_DataValid & D3DXSM_VERTDATA)
    {
        return S_OK;
    }
    HRESULT hr = GetMaxVertexInfluences(&m_maxVertInfl);

    if (FAILED(hr))
        return hr;

    if (m_vertMatId != NULL)
    {
        delete[] m_vertMatId;
        m_vertMatId = NULL;
    }

    if (m_vertMatWeight != NULL)
    {
        delete[] m_vertMatWeight;
        m_vertMatWeight = NULL;
    }

    m_vertMatId = new DWORD[m_pMesh->m_cVertices * m_maxVertInfl]; // per vertex list of matrix IDs
    m_vertMatWeight = new float[m_pMesh->m_cVertices * m_maxVertInfl]; // per vertex list matrix weights
    if (m_vertMatId == NULL || 
        m_vertMatWeight == NULL)
    {
        DPF(0, "Out of Memory");
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // reuse vertInf[i] as the current num of influences per vertex
    memset(m_vertInfl, 0, sizeof(DWORD) * m_pMesh->m_cVertices);
    // Compute per vertex skin data
	for (i = 0; i < m_numBones; ++i)
    {
		CBone* pBone = &m_pBones[i];
        for (j = 0; j < pBone->m_numWeights; ++j)
        {
            DWORD v = pBone->m_pVertIndices[j];
            GXASSERT(v < m_pMesh->m_cVertices);
            m_vertMatId[v * m_maxVertInfl + m_vertInfl[v]] = i;
            m_vertMatWeight[v * m_maxVertInfl + m_vertInfl[v]] = pBone->m_pWeights[j];
            GXASSERT(m_vertInfl[v] <= m_maxVertInfl);
            m_vertInfl[v]++;
        }
    }
    // Initialize remaining m_vertMatId values
    for (i = 0; i < m_pMesh->m_cVertices; ++i)
    {
        for (j = m_vertInfl[i]; j < m_maxVertInfl; ++j)
        {
            m_vertMatId[i * m_maxVertInfl + j] = 0;
        }
    }

    m_DataValid |= D3DXSM_VERTDATA;
    return S_OK;

e_Exit:
    delete [] m_vertMatId;
    m_vertMatId = NULL;
    delete [] m_vertMatWeight;
    m_vertMatWeight = NULL;
    return hr;
}

// This function sorts vertex weight by importance only if the num infl for the vertex
// is greated than truncVertInfl. It does not guarantee that all vertex weights are sorted
HRESULT CD3DXSkinMesh::TruncVertexSkinData(DWORD truncVertInfl, LPDWORD pAdjacencyIn)
{
    HRESULT hr;
    DWORD i, j, k;

    if (!(m_DataValid & D3DXSM_VERTDATA))
    {
        hr = CalcVertexSkinData();
        if (hr != S_OK)
            return hr;
    }

    DWORD bPRep = FALSE;
    DWORD* pointRep = NULL;
#if DBG
    float maxTrunc = 0.0f;
#endif
    for (i = 0; i < m_pMesh->m_cVertices; ++i)
    {
        if (m_vertInfl[i] > truncVertInfl)
        {
            if ((!bPRep) && pAdjacencyIn)
            {
                pointRep = new DWORD[m_pMesh->m_cVertices];
                if (!pointRep)
                {
                    DPF(0, "Out of Memory");
                    return E_OUTOFMEMORY;
                }
                // Get Point reps for the original mesh
                hr = m_pMesh->ConvertAdjacencyToPointReps(pAdjacencyIn, pointRep);
                if (hr != S_OK)
                {
                    return hr;
                }
                bPRep = TRUE;
            }
            DWORD v = bPRep ? pointRep[i] : i;
            float* vertMatWeight = m_vertMatWeight + (v * m_maxVertInfl);
            DWORD* vertMatId = m_vertMatId + (v * m_maxVertInfl);
            float sum = 0.0f;
            // Sort vertex weights by importance
            for (j = 0; j < truncVertInfl; ++j)
            {
                DWORD max = 0;
                float maxWeight = 0.0f;
                for (k = j; k < m_vertInfl[v]; ++k)
                {
                    if (vertMatWeight[k] > maxWeight)
                    {
                        maxWeight = vertMatWeight[k];
                        max = k;
                    }
                }
                // Move jth largest element to its correct position in sorted array
                DWORD t1 = vertMatId[j];
                vertMatId[j] = vertMatId[max];
                vertMatId[max] = t1;
                float t2 = vertMatWeight[j];
                vertMatWeight[j] = vertMatWeight[max];
                sum += vertMatWeight[max];
                vertMatWeight[max] = t2;
            }
            double scale = 1.0 / sum;
            // Renormalize vertex weights
            for (j = 0; j < truncVertInfl; ++j)
            {
                vertMatWeight[j] = (float)(vertMatWeight[j] * scale);
            }
            for (; j < m_vertInfl[v]; ++j)
            {
#if DBG
                maxTrunc = max(maxTrunc, vertMatWeight[j]);
#endif
                vertMatWeight[j] = 0.0f;
            }
        }
    }

    if (bPRep)
    {
        DPF(1, "Mesh has upto %d infl per vertex, truncating to %d", m_maxVertInfl, m_faceClamp);
        DPF(1, "Max truncated vertex weight = %f", maxTrunc);
        // propogate changes to point reps to all replicated points
        for (i = 0; i < m_pMesh->m_cVertices; ++i)
        {
            if (pointRep[i] != i)
            {
                for (j = 0; j < m_vertInfl[i]; ++j)
                {
                    m_vertMatId[i * m_maxVertInfl + j] = m_vertMatId[pointRep[i] * m_maxVertInfl + j];
                    m_vertMatWeight[i * m_maxVertInfl + j] = m_vertMatWeight[pointRep[i] * m_maxVertInfl + j];
                }
            }
        }
    }
    return S_OK;
}





struct SBoneCombinationNode
{
    DWORD                   iBoneCombination;

    CD3DXBitField256        bitBones;
    DWORD                   iAttrib;

    SBoneCombinationNode*   pPrev;
    SBoneCombinationNode*   pNext;
}; 



HRESULT GreedyTSP
(
    SBoneCombinationNode*   pCandidatePool, 
    SBoneCombinationNode**  ppElected, 
    DWORD*                  pcCost,                 // ASSUMPTION:  *pcCost is initialized to starting cost
    DWORD*                  pcSavings               // ASSUMPTION:  *pcSavings is initialized to starting savings
)
{
    SBoneCombinationNode*   pElected;
    SBoneCombinationNode*   pCandidate;
    SBoneCombinationNode*   pBestCandidate;

    DWORD                   cMaxVote;
    DWORD                   cBestVote;
    DWORD                   cVote;


    HRESULT hr  = S_OK;

    const   DWORD           nAttributeWeighting = 1;


    // initialize elected

    *ppElected  = NULL;


    if (pCandidatePool == NULL || pCandidatePool->pNext == NULL)
    {
        *ppElected  = pCandidatePool;

        goto e_Exit;
    }

    // elect first candidate in line

    pElected        = pCandidatePool;

    pCandidatePool  = pElected->pNext;

    pElected->pNext         = NULL;

    pCandidatePool->pPrev   = NULL;


    // elect subsequent candidates greedily

    while (pCandidatePool)      // candidate pool is not empty
    {

        // calculate maximum possible vote (i.e. savings)

        cMaxVote    = pElected->bitBones.Count() + nAttributeWeighting;

    
        // initialize candidate & best vote

        pBestCandidate  = NULL;

        cBestVote       = -1;



        // search through candidates for best candidate

        for (pCandidate = pCandidatePool;   pCandidate != NULL;   pCandidate = pCandidate->pNext)
        {
            CD3DXBitField256    bitSharedBones;

            // calculate candidate's vote (as the number of shared bones & attributes with the elected bone combination)
            
            bitSharedBones.And(pElected->bitBones, pCandidate->bitBones);

            cVote = bitSharedBones.Count()
                  + nAttributeWeighting * (pElected->iAttrib == pCandidate->iAttrib);


            if (cVote + 1 > cBestVote + 1)      // this is the best vote yet
            {
                // update best candidate & vote

                pBestCandidate  = pCandidate;

                cBestVote       = cVote;


                if (cBestVote == cMaxVote)          // this is definitely the best possible vote
                {
                    break;
                }
            }
        }


        // update cost & savings

        *pcCost      += cMaxVote;

        *pcSavings   += cBestVote;


        // remove the best candidate from candidate pool

        if (pBestCandidate->pPrev)                              
        {
            pBestCandidate->pPrev->pNext    = pBestCandidate->pNext;    // INVARIANT:  pBestCandidate != NULL
        }

        if (pBestCandidate->pNext)                              
        {
            pBestCandidate->pNext->pPrev    = pBestCandidate->pPrev;    // INVARIANT:  pBestCandidate != NULL
        }

        if (pCandidatePool == pBestCandidate)
        {
            pCandidatePool  = pCandidatePool->pNext;
        }



        // elect the best candidate

        pBestCandidate->pPrev   = NULL;

        pBestCandidate->pNext   = pElected;

        pElected->pPrev = pBestCandidate;                               // INVARIANT: pElected != NULL

        pElected        = pBestCandidate;
    }


    // update cost

    *pcCost  += pElected->bitBones.Count() + nAttributeWeighting;
    

    // update elected

    *ppElected  = pElected;

e_Exit:

    return hr;
}

void CD3DXSkinMesh::CalcNumAttributes(LPDWORD pAttrMask, LPDWORD pAttrBits)
{
    DWORD attrMask = 0;
    DWORD attribBits = 0;

    m_pMesh->LockIB((PBYTE*)&m_pMesh->m_pFaces);
	// Count the number of attributes in the mesh
	m_pMesh->GetAttributeTable(NULL, &m_numAttrib);
	if (!m_numAttrib)
	{
		LPDWORD pAttrib;
		m_pMesh->CollectAttributeIds(&pAttrib, &m_numAttrib);
		delete [] pAttrib;
	}
    m_pMesh->UnlockIB();
    m_pMesh->m_pFaces = NULL;

    // Compute attribBits = log2(m_numAttrib)
    while ((m_numAttrib & attrMask) != m_numAttrib)
    {
        attribBits++;
        attrMask = (attrMask << 1) | 1;
    };
    *pAttrMask = attrMask;
    *pAttrBits = attribBits;
}

HRESULT CD3DXSkinMesh::ConvertToBlendedMesh(DWORD options, CONST LPDWORD pAdjacencyIn, LPDWORD pAdjacencyOut,
									DWORD* pNumBoneCombinations, LPD3DXBUFFER* ppBoneCombinationTable, LPD3DXMESH* ppMesh)
{
    DWORD stride;
    DWORD* faceMatId = NULL;
    // TODO: this needs to be extended to 32-bit
    WORD (*pFaces)[3] = NULL;
    DWORD* origAttribIds = NULL;
    DWORD fvf;
    DWORD* vertRemap1 = NULL;
    DWORD* vertRemap2 = NULL;
    LPD3DXATTRIBUTERANGE attrTable = NULL;
    LPD3DXBONECOMBINATION pBoneCombination = NULL;
    DWORD matidMask;
    BOOL bTruncWeights;
    BOOL bMixedDevice;
    float* faceWeights = NULL;
    DWORD faceClamp, faceClampHW;
    DWORD attribBits;
#if DBG
    float maxTrunc = 0.0f;
#endif
    DWORD numAttrib = 0;
    DWORD attrMask;
    BOOL bPRep = FALSE;
    GXTri3Mesh<tp16BitIndex>* pNewMesh4 = NULL;
    DWORD   cMaxFaceInfl;

#if DBG
    DWORD* pAdjTest = NULL;
#endif

    HRESULT hr = S_OK;

    bMixedDevice    = FALSE;
    faceClamp       = m_faceClamp;
    faceClampHW     = m_faceClamp;

    // TODO: remove this limitation
    if (options & D3DXMESH_32BIT)
    {
        DPF(0, "32-bit meshes are not yet supported");
        return E_NOTIMPL;
    }

    // Is there any skinning info ?
    if (m_numBones == 0)
    {
        hr = m_pMesh->CloneMeshFVF(options, m_pMesh->GetFVF(), m_pMesh->m_pD3DDevice, ppMesh);
        return hr;
    }

    if (m_bMixedDevice)
    {
        if (!(options & D3DXMESH_USEHWONLY))
        {
            bMixedDevice = TRUE;
            DPF(1, "Detected mixed device: faceClampHW = %d", faceClampHW);
            faceClamp = 4;
        }
    }
    DWORD numVerts = m_pMesh->GetNumVertices();
    DWORD numFaces = m_pMesh->GetNumFaces();
    LPDWORD faceInfl = new DWORD[numFaces];
    DWORD* faceAttr = new DWORD[numFaces]; // per face attribute
    DWORD* faceRemap = new DWORD[numFaces];
    DWORD* pointRep = new DWORD[numVerts];
    DWORD* pAdjacencyTemp = new DWORD[numFaces * 3];
    if (faceInfl == NULL ||
        faceAttr == NULL ||
        faceRemap == NULL ||
        pAdjacencyTemp == NULL ||
        pointRep == NULL)
    {
        DPF(0, "Out of Memory");
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(faceInfl, 0, sizeof(DWORD) * numFaces);
    memset(faceAttr, 0, sizeof(DWORD) * numFaces);

    DWORD i,j;
//    DWORD cpattrOld;
    //IGXAttributeBundleDX7 **rgpattrOld = NULL;

    CalcNumAttributes(&attrMask, &attribBits);
    
    hr = CalcVertexSkinData();
    if (hr != S_OK)
    {
        goto e_Exit;
    }

    // Allocate per face strorage for skin data
    stride = m_maxVertInfl * 3;
    faceMatId = new DWORD[numFaces * stride]; // per face list of matrix IDs
    faceWeights = new float[stride]; // cumulative weights for a face for sorting
    if (faceMatId == NULL || 
        faceWeights == NULL)
    {
        DPF(0, "Out of Memory");
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

#if DBG
    // Verify weights add up to 1
    for (i = 0; i < numVerts; ++i)
    {
        float sum = 0.f;
        for (j = 0; j < m_vertInfl[i]; ++j)
            sum += m_vertMatWeight[i * m_maxVertInfl + j];
        GXASSERT(fabs(sum - 1.f) < 0.001);
    }
#endif

    // Get Faces
    m_pMesh->LockIB((PBYTE*)&m_pMesh->m_pFaces);
    pFaces = reinterpret_cast<WORD(*)[3]>(m_pMesh->m_pFaces);

    // Compute influences per face
    memset(faceMatId, 0xff, sizeof(DWORD) * numFaces * stride);
    cMaxFaceInfl = 0;
    for (i = 0; i < numFaces; ++i)
    {
        faceInfl[i] = 0;
        for (j = 0; j < 3; ++j)
        {
            DWORD v;
            if (bPRep)
                v = pointRep[pFaces[i][j]];
            else
                v = pFaces[i][j];
            for (DWORD k = 0; k < m_vertInfl[v]; ++k)
            {
                if (m_vertMatWeight[v * m_maxVertInfl + k] == 0.0f)
                    continue;
                for (DWORD l = 0; l < faceInfl[i]; ++l)
                {
                    if (faceMatId[i * stride + l] == m_vertMatId[v * m_maxVertInfl + k])
                    {
                        faceWeights[l] = max(faceWeights[l], m_vertMatWeight[v * m_maxVertInfl + k]);
                        break;
                    }
                }
                if (l == faceInfl[i])
                {
                    faceMatId[i * stride + l] = m_vertMatId[v * m_maxVertInfl + k];
                    faceWeights[l] = m_vertMatWeight[v * m_maxVertInfl + k];
                    faceInfl[i]++;
                }
            }
        }
        if (faceInfl[i] > cMaxFaceInfl)
        {
            cMaxFaceInfl = faceInfl[i];
        }
        if (faceInfl[i] > faceClamp) 
        {
            if (!bPRep)
            {
                // Get Point reps for the original mesh
                hr = m_pMesh->ConvertAdjacencyToPointReps(pAdjacencyIn, pointRep);
                if (hr != S_OK)
                {
                    goto e_Exit;
                }
                bPRep = TRUE;
            }
            if (faceClamp > 2)
            {
                // Sort face weights by importance
                for (j = 0; j < faceClamp; ++j)
                {
                    DWORD max = 0;
                    float maxWeight = 0.0f;
                    for (DWORD k = j; k < faceInfl[i]; ++k)
                    {
                        if (faceWeights[k] > maxWeight)
                        {
                            maxWeight = faceWeights[k];
                            max = k;
                        }
                    }
                    // Move jth largest element to its correct position in sorted array
                    DWORD t1 = faceMatId[i * stride + j];
                    faceMatId[i * stride +j] = faceMatId[i * stride + max];
                    faceMatId[i * stride + max] = t1;
                    float t2 = faceWeights[j];
                    faceWeights[j] = faceWeights[max];
                    faceWeights[max] = t2;
                }
                faceInfl[i] = faceClamp;
                bTruncWeights = TRUE;

                // Fix vertex weights too
                for (j = 0; j < 3; ++j)
                {
                    DWORD v = pointRep[pFaces[i][j]];
                    float trunc = 0.0f;
                    for (DWORD k = 0; k < m_vertInfl[v]; ++k)
                    {
                        for (DWORD l = 0; l < faceInfl[i]; ++l)
                        {
                            if (faceMatId[i * stride + l] == m_vertMatId[v * m_maxVertInfl + k])
                                break;
                        }
                        if (l == faceInfl[i]) // Not found, we must have trucated it
                        {
                            // Mark the corresponding vertex weight *of the representative vertex* to 0
                            GXASSERT(fabs(m_vertMatWeight[v * m_maxVertInfl + k] - 1.0f) > 0.001);
                            trunc += m_vertMatWeight[v * m_maxVertInfl + k];
#if DBG
                            maxTrunc = max(maxTrunc, m_vertMatWeight[v * m_maxVertInfl + k]);
#endif
                            m_vertMatWeight[v * m_maxVertInfl + k] = 0.0f;
                        }
                    }
                    if (trunc != 0.0f)
                    {
                        // Re-normalize the vertex weights
                        double scale = 1 / (1.0 - trunc);
                        for (DWORD k = 0; k < m_vertInfl[v]; ++k)
                        {
                            if (m_vertMatWeight[v * m_maxVertInfl + k] != 0.0f)
                                m_vertMatWeight[v * m_maxVertInfl + k] = (float)(m_vertMatWeight[v * m_maxVertInfl + k] * scale);
                        }
                    }
                }
            }
        }
    }

    delete [] faceWeights;
    faceWeights = NULL;

    if (cMaxFaceInfl > faceClamp)
    {
        if (faceClamp < 3)
        {
            DPF(0, "ConvertToBlendedMesh: The HW only supports 2 infl per tri.");
            DPF(0, "ConvertToBlendedMesh: This mesh has %d infl per tri and cannot be truncated", cMaxFaceInfl);
            DPF(0, "ConvertToBlendedMesh: Try using a mixed mode device to render this mesh");
            hr = D3DXERR_TOOMANYINFLUENCES;
            goto e_Exit;
        }
        DPF(1, "ConvertToBlendedMesh: Mesh has upto %d infl per tri, truncating to %d", cMaxFaceInfl, faceClamp);
        DPF(1, "ConvertToBlendedMesh: Max truncated vertex weight = %f", maxTrunc);
        cMaxFaceInfl = faceClamp;

        // Recompute face influences with new trucated vertex influences to avoid cracks
        memset(faceMatId, 0xff, sizeof(DWORD) * numFaces * stride);
        for (i = 0; i < numFaces; ++i)
        {
            faceInfl[i] = 0;
            for (j = 0; j < 3; ++j)
            {
                DWORD v = pointRep[pFaces[i][j]];
                for (DWORD k = 0; k < m_vertInfl[v]; ++k)
                {
                    if (m_vertMatWeight[v * m_maxVertInfl + k] == 0.0f)
                        continue;
                    for (DWORD l = 0; l < faceInfl[i]; ++l)
                    {
                        if (faceMatId[i * stride + l] == m_vertMatId[v * m_maxVertInfl + k])
                        {
                            break;
                        }
                    }
                    if (l == faceInfl[i])
                    {
                        faceMatId[i * stride + l] = m_vertMatId[v * m_maxVertInfl + k];
                        faceInfl[i]++;
                    }
                }
            }
            GXASSERT(faceInfl[i] <= faceClamp);
        }
    }

    // Done with original mesh's faces
    m_pMesh->UnlockIB();
    m_pMesh->m_pFaces = NULL;
    pFaces = NULL;

    GXASSERT(cMaxFaceInfl <= faceClamp); // DX7 API restriction

    // Allocate bits in DWORD for bone ID & AttribId
    m_matidShift = (32 - attribBits) / cMaxFaceInfl;
    matidMask = (1 << m_matidShift) - 1;
    GXASSERT((1i64 << m_matidShift) - 1 > m_numBones + 1);

    // Generate Unique attribute ids
    // An attribute ID uniquely identifies a combination of matrix IDs per face
    for (i = 0; i < numFaces; ++i)
    {
        // Generate unique face attribute
        for (j = 0; j < faceInfl[i]; ++j)
        {
            faceAttr[i] |= (faceMatId[i * stride + j] + 1) << (m_matidShift * j);
        }
        faceAttr[i] |= m_pMesh->m_rgiAttributeIds[i] << (m_matidShift * cMaxFaceInfl);
    }
    attrMask = attrMask << (m_matidShift * cMaxFaceInfl);

    // Evil Hack: Temporarily substitute the new attributes to optimize
    origAttribIds = m_pMesh->m_rgiAttributeIds;
    m_pMesh->m_rgiAttributeIds = faceAttr;

    // Optimize using new attribute IDs to new mesh
    GXTri3Mesh<tp16BitIndex>* pNewMesh1;
    LPD3DXBUFFER pVertRemapBuf1;
    // TODO: Is it worthwhile to reduce the fvf of the temporary mesh to have minimum vertex data.
    // We would then copy all vertex data later during the computation of per vertex weights ?
    hr = m_pMesh->Optimize2(D3DXMESH_SYSTEMMEM, D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT,
                            pAdjacencyIn, pAdjacencyTemp, faceRemap, &pVertRemapBuf1, m_pMesh->GetFVF(), &pNewMesh1);
    if (hr != S_OK)
    {
        goto e_Exit;
    }

    // Undo evil hack: Restore the original attribute Ids
    m_pMesh->m_rgiAttributeIds = origAttribIds;
    delete [] faceAttr;
    faceAttr = NULL;

    vertRemap1 = reinterpret_cast<DWORD*>(pVertRemapBuf1->GetBufferPointer());

#if 0//DBG // This no longer works since pFacesNew is not yet initialized
    // Verify the remap tables
    m_pMesh->LockIB((PBYTE*)&pFaces);
    for (i = 0; i < numFaces; ++i)
    {
        DWORD fold = faceRemap[i];
        for (j = 0; j < 3; ++j)
        {
            DWORD vnew = pFacesNew[i][j];
            if (vertRemap1[vnew] != pFaces[fold][j])
            {
                DPF(1, "Remap inconsistency:");
                DPF(1, "    NewFace[%d][%d] = %d", i, j, vnew);
                DPF(1, "    vertRemap1[%d] = %d, faceRemap[%d] = %d", vnew,
                    vertRemap1[vnew], i, fold);
                DPF(1, "    OldFace[%d][%d] = %d (!= %d)", fold, j, pFaces[fold][j], vertRemap1[vnew]);
            }
        }
    }
    // Done with original mesh's faces
    m_pMesh->UnlockIB();
    pFaces = NULL;
#endif

    // Get the new mesh's attribute table
    numAttrib = pNewMesh1->m_caeAttributeTable;
    //DPF(1, "Num Attributes before coalescing: %d", numAttrib);
    attrTable = new D3DXATTRIBUTERANGE[numAttrib];
    pNewMesh1->GetAttributeTable(attrTable, NULL);

    { // Silly indent to define pFaceInflBits after a goto and keep the compiler happy
        // TODO: Currently we merge combinations after an initial attribute sort to generate a table
        // of diffrent combinations. Is it ever better to merge combinations before attribute sort on
        // the faces ? Since this algorithm is n^2, this is a win if num combinations is smaller than
        // numFaces. This is should be mostly the case but when the difference is not large, the higher
        // constant overhead of an optimize call might make the current approach inefficient

        // Generate Bitfields for each bone combination
        CD3DXBitField256* pFaceInflBits = new CD3DXBitField256[numAttrib];
        DWORD* faceInflCount = new DWORD[numAttrib];
        if (pFaceInflBits == NULL || faceInflCount == NULL)
        {
            DPF(0, "Out of Memory");
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        memset(faceInflCount, 0, sizeof(DWORD) * numAttrib);
        for (i = 0; i < numAttrib; ++i)
        {
		    for (j = 0; j < cMaxFaceInfl; ++j)
		    {
                DWORD mat = (attrTable[i].AttribId >> (j * m_matidShift)) & matidMask;
			    if (mat)
                {
                    pFaceInflBits[i].Set(mat - 1);
                    faceInflCount[i]++;
                }
            }
        }

        // Merge subset influences
        for (i = 0; i < numAttrib; ++i)
        {
            for (j = 0; j < numAttrib; ++j)
            {
                if (pFaceInflBits[i].IsSubset(pFaceInflBits[j]))
                {
                    if ((faceInflCount[j] > faceClampHW) || (faceInflCount[i] <= faceClampHW))
                        pFaceInflBits[j] = pFaceInflBits[i];
                }
            }
        }

        // Rewrite update attributes into attribute table
        DWORD* pAttrib;
        pNewMesh1->LockAttributeBuffer(NULL, &pAttrib);
        for (i = 0; i < numAttrib; ++i)
        {
            // Convert bitfield back to attribIds
            // TODO: Can this be improved ?
            DWORD attribId = 0;
            DWORD k = 0;
            for (j = 0; j < m_numBones; ++j)
            {
                if (pFaceInflBits[i].IsSet(j))
                {
                    attribId |= (j + 1) << (m_matidShift * k);
                    ++k;
                }
            }
            // Copy the original attribute back into the new attribId
            attribId |= attrTable[i].AttribId & attrMask;
            // Fill per face attribute Ids
            for (j = 0; j < attrTable[i].FaceCount; ++j)
            {
                pAttrib[j + attrTable[i].FaceStart] = attribId;

            }
        }
        pNewMesh1->UnlockAttributeBuffer();
        pAttrib = NULL;
        delete [] pFaceInflBits;
        pFaceInflBits = NULL;
        delete [] attrTable;
        attrTable = NULL;
        delete [] faceInflCount;
        faceInflCount = NULL;
    }
    // Calculate fvf for new mesh
    fvf = D3DFVF_XYZ;
    if (cMaxFaceInfl - 1)
        fvf = D3DFVF_XYZRHW + (cMaxFaceInfl - 1) * 2; // hack to compute FVF
    fvf = (m_pMesh->GetFVF() & (~D3DFVF_POSITION_MASK)) | fvf; // new FVF

    GXTri3Mesh<tp16BitIndex>* pNewMesh2;
    LPD3DXBUFFER pVertRemapBuf2;
    
    hr = pNewMesh1->Optimize2(D3DXMESH_SYSTEMMEM, D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT,
                            pAdjacencyTemp, pAdjacencyOut, faceRemap, &pVertRemapBuf2, fvf, &pNewMesh2);

    pNewMesh1->Release();
    pNewMesh1 = NULL;
    // Get access to the new mesh's vertex data
    LPBYTE pVertOut;
    hr = pNewMesh2->LockVertexBuffer(0, &pVertOut);
    if (hr != S_OK)
    {
        goto e_Exit;
    }
    WORD (*pFacesNew)[3];
    hr = pNewMesh2->LockIndexBuffer(0, (LPBYTE*)&pFacesNew);
    if (hr != S_OK)
    {
        goto e_Exit;
    }
    vertRemap2 = reinterpret_cast<DWORD*>(pVertRemapBuf2->GetBufferPointer());

    // Get the new mesh's attribute table
    attrTable = pNewMesh2->m_rgaeAttributeTable;
    *pNumBoneCombinations = pNewMesh2->m_caeAttributeTable;
    //DPF(1, "Num Attributes after coalescing: %d", *pNumBoneCombinations);

    { // indent to define cfvfout after a goto and keep the compiler happy
        DXCrackFVF  cfvfout(fvf);
        // Generate vertex weights for the new mesh
        for (j = 0; j < *pNumBoneCombinations; ++j)
        {
            for (i = 0; i < attrTable[j].VertexCount; ++i) // For each vertex of a particular bone combination
            {
                DWORD v;
                // Original *representative* vertex corresponding to the new vertex
                if (bPRep)
                    v = pointRep[vertRemap1[vertRemap2[i + attrTable[j].VertexStart]]];
                else
                    v = vertRemap1[vertRemap2[i + attrTable[j].VertexStart]];
                LPBYTE pOut = (LPBYTE)cfvfout.GetArrayElem(pVertOut, i + attrTable[j].VertexStart);
                float sum = 0.0f;
                for (DWORD k = 0; k < cMaxFaceInfl - 1; ++k)
                { // For each face matrix, see if this vertex is affected by the matrix
                    ((float*)(pOut + sizeof(D3DVECTOR)))[k] = 0.0f;
                    DWORD mat = (attrTable[j].AttribId >> (k * m_matidShift)) & matidMask;
			        if (mat)
                    {
                        for (DWORD l = 0; l < m_vertInfl[v]; ++ l)
                        {
                            if (m_vertMatId[v * m_maxVertInfl + l] == mat - 1)
                            {
                                ((float*)(pOut + sizeof(D3DVECTOR)))[k] = m_vertMatWeight[v * m_maxVertInfl + l];
                                sum += m_vertMatWeight[v * m_maxVertInfl + l];
                                break;
                            }
                        }
                    }
                }
#if DBG
                // Verify new vertex weights
                float lastweight = 0.0f;
                DWORD mat = (attrTable[j].AttribId >> (k * m_matidShift)) & matidMask;
			    if (mat)
                {
                    for (DWORD l = 0; l < m_vertInfl[v]; ++ l)
                    {
                        if (m_vertMatId[v * m_maxVertInfl + l] == mat - 1)
                        {
                            lastweight = m_vertMatWeight[v * m_maxVertInfl + l];
                            sum += lastweight;
                            break;
                        }
                    }
                }
                // All weights should add up to 1
                if (fabs(sum - 1.f) > 0.001)
                {
                    GXASSERT(fabs(sum - 1.f) < 0.001);
					DPF(1, "Weights of vertex %d (orig. %d) do not add up (sum is %f)", i + attrTable[j].VertexStart, v, sum);
#if 0 // This no longer works since we have 2 face remap tables
                    // Find the faces referring to the offending vertex
                    for (DWORD k = 0; k < attrTable[j].FaceCount; ++k)
                    {
                        for (DWORD l = 0; l < 3; ++l)
                        {
                            if (pFacesNew[k + attrTable[j].FaceStart][l] == i + attrTable[j].VertexStart)
                                DPF(1, "Face %d (orig. %d) uses vertex %d", k + attrTable[j].FaceStart, 
                                    faceRemap[k + attrTable[j].FaceStart], i + attrTable[j].VertexStart);
                        }
                    }
#endif
                }
#if 0 // This verification is dangerous. It reads back from the dest VB which could be in vid mem
                for (DWORD l = 0; l < m_vertInfl[v]; ++l)
                {
                    for (k = 0; k < cMaxFaceInfl; ++k)
                    {
                        DWORD mat = (attrTable[j].AttribId >> (k * m_matidShift)) & matidMask;
			            if (mat)
                        {
                            if (m_vertMatId[v * m_maxVertInfl + l] == mat - 1)
                            {
                                if (k == cMaxFaceInfl - 1)
                                {
                                    GXASSERT(m_vertMatWeight[v * m_maxVertInfl + l] == lastweight);
                                }
                                else
                                {
                                    GXASSERT(m_vertMatWeight[v * m_maxVertInfl + l] == ((float*)(pOut + sizeof(D3DVECTOR)))[k]);
                                }
                            }
                        }
                    }
                }
#endif // 0
#endif // DBG
            }
        }

        // Free stuff not needed any more
        delete [] faceInfl;
        faceInfl = NULL;
        delete [] faceMatId;
        faceMatId = NULL;
        delete [] pointRep;
        pointRep = NULL;

        // Generate the bone combination table
	    hr = D3DXCreateBuffer((sizeof(D3DXBONECOMBINATION) + 4 * sizeof(DWORD))* *pNumBoneCombinations, ppBoneCombinationTable);
        if (hr != S_OK)
        {
            goto e_Exit;
        }
	    pBoneCombination = static_cast<LPD3DXBONECOMBINATION>((*ppBoneCombinationTable)->GetBufferPointer());

        // set up BoneId pointers inside the buffer
        DWORD (*pBoneIds)[4] = reinterpret_cast<DWORD(*)[4]>(pBoneCombination + *pNumBoneCombinations);
        for (i = 0; i < *pNumBoneCombinations; ++i)
        {
            pBoneCombination[i].BoneId = pBoneIds[i];
        }
        // Hack: Lie about read only even though we modify the attribute data since the vertices need not be reordered
        // by this change. attribute table has already been updated.
        DWORD* pAttrib;
        pNewMesh2->LockAttributeBuffer(D3DLOCK_READONLY, &pAttrib);

        for (i = 0; i < *pNumBoneCombinations; ++i)
        {
            for (j = 0; j < attrTable[i].FaceCount; ++j)
            {
                pAttrib[attrTable[i].FaceStart + j] = i;
            }
		    for (j = 0; j < cMaxFaceInfl; ++j)
		    {
			    pBoneCombination[i].BoneId[j] = (attrTable[i].AttribId >> (j * m_matidShift)) & matidMask;
                if (pBoneCombination[i].BoneId[j])
                {
                    pBoneCombination[i].BoneId[j]--;
                }
                else
                {
                    pBoneCombination[i].BoneId[j] = UINT_MAX;
                }
		    }
            for (; j < 4; ++j)
            {
                pBoneCombination[i].BoneId[j] = UINT_MAX;
            }
		    pBoneCombination[i].AttribId = attrTable[i].AttribId >> (cMaxFaceInfl * m_matidShift);
		    pBoneCombination[i].FaceCount = attrTable[i].FaceCount;
		    pBoneCombination[i].FaceStart = attrTable[i].FaceStart;
		    pBoneCombination[i].VertexCount = attrTable[i].VertexCount;
		    pBoneCombination[i].VertexStart = attrTable[i].VertexStart;
            attrTable[i].AttribId = i;
        }

        pNewMesh2->UnlockAttributeBuffer();
        pAttrib = NULL;




#if 1
        // MINIMIZE BONE ID & ATTRIBUTE CHANGES




        DWORD iInfl, iAttr, iPoint, i;

        pAttrib         = NULL;


        GXTri3Mesh<tp16BitIndex>* pNewMesh3     = NULL;

        LPDIRECT3DDEVICE8       pDevice         = NULL;

        LPD3DXBUFFER            pVertRemapBuf3  = NULL;
        LPD3DXBUFFER            pVertRemapBuf4  = NULL;

        PBYTE                   pbPoints        = NULL;

        DWORD*                  rgiOrder        = NULL;

        D3DXBONECOMBINATION*    rgbcOptimized   = NULL;


        SBoneCombinationNode*   rgNodes         = NULL;

        SBoneCombinationNode*   pNode;
        DWORD                   iNode;


        D3DCAPS8                caps;


        SBoneCombinationNode*   pCandidatePoolHW;
        SBoneCombinationNode*   pElectedHW;

        SBoneCombinationNode*   pCandidatePoolSW;
        SBoneCombinationNode*   pElectedSW;


        DWORD                   cCost;
        DWORD                   cSavings;


        DXCrackFVF              cfvf(pNewMesh2->GetFVF());      // since eventually, pNewMesh3->GetFVF() == pNewMesh2->GetFVF()

        DWORD                   cbPointSize;


        

 
        // don't optimize if there are less than 2 bone combinations

        if (*pNumBoneCombinations <= 1)     // there are less than two bone combinations
        {
            goto e_ExitTSP;
        }





        // allocate lump of bone combination nodes

        rgNodes = new SBoneCombinationNode[*pNumBoneCombinations];

        if (rgNodes == NULL)
        {
            hr  = E_OUTOFMEMORY;

            DPF(1, "ConvertToBlendedMesh:  not enough memory; aborting optimization");

            goto e_ExitTSP;
        }

    
        // allocate reordered bone combination table

        rgbcOptimized   = new D3DXBONECOMBINATION[*pNumBoneCombinations];

        if (rgbcOptimized == NULL)
        {
            hr  = E_OUTOFMEMORY;

            DPF(1, "ConvertToBlendedMesh:  not enough memory; aborting optimization");

            goto e_ExitTSP;
        }


        // allocate reordering array

        rgiOrder    = new DWORD[*pNumBoneCombinations];

        if (rgiOrder == NULL)
        {
            hr  = E_OUTOFMEMORY;

            DPF(1, "ConvertToBlendedMesh:  not enough memory; aborting optimization");

            goto e_ExitTSP;
        }

  
       

        // initialize bone combination node data

        for (iNode = 0; iNode < *pNumBoneCombinations; iNode++)
        {
            for (DWORD iBone = 0; iBone < 4; iBone++)
            {
                if (pBoneCombination[iNode].BoneId[iBone] != UINT_MAX)
                {
                    rgNodes[iNode].bitBones.Set(pBoneCombination[iNode].BoneId[iBone]);
                }
            }
            
            rgNodes[iNode].iBoneCombination = iNode;

            rgNodes[iNode].iAttrib          = pBoneCombination[iNode].AttribId;
        }




        // generate candidate pools

        pCandidatePoolHW    = NULL;

        pCandidatePoolSW    = NULL;


        for (iNode = 0; iNode < *pNumBoneCombinations; iNode++)
        {
            rgNodes[iNode].pPrev    = NULL;


            if (bMixedDevice && rgNodes[iNode].bitBones.Count() > faceClampHW)
            {
                // add node to software candidate pool

                rgNodes[iNode].pNext    = pCandidatePoolSW;

                pCandidatePoolSW        = rgNodes + iNode;

                if (pCandidatePoolSW->pNext)
                {
                    pCandidatePoolSW->pNext->pPrev  = pCandidatePoolSW;
                }
            }
            else
            {
                // add node to hardware candidate pool

                rgNodes[iNode].pNext    = pCandidatePoolHW;

                pCandidatePoolHW        = rgNodes + iNode;

                if (pCandidatePoolHW->pNext)
                {
                    pCandidatePoolHW->pNext->pPrev  = pCandidatePoolHW;
                }
            }
        }


        
        // initialize cost & savings

        cCost       = 0;

        cSavings    = 0;
    

        // generate election lists

        hr = GreedyTSP(pCandidatePoolHW, &pElectedHW, &cCost, &cSavings);

        if (FAILED(hr))
            goto e_ExitTSP;

        
        hr = GreedyTSP(pCandidatePoolSW, &pElectedSW, &cCost, &cSavings);

        if (FAILED(hr))
            goto e_ExitTSP;


 
        // calculate reordering array and reordered bone combination table

        iNode   = 0;


        for (pNode = pElectedHW; pNode != NULL; pNode = pNode->pNext, iNode++)
        {
            rgbcOptimized[iNode] = pBoneCombination[pNode->iBoneCombination];

            rgiOrder[pNode->iBoneCombination]    = iNode;
        }


        for (pNode = pElectedSW; pNode != NULL; pNode = pNode->pNext, iNode++)
        {
            rgbcOptimized[iNode] = pBoneCombination[pNode->iBoneCombination];

            rgiOrder[pNode->iBoneCombination]    = iNode;
        }




        

        // load the reordered bone combination table

        memcpy(pBoneCombination, rgbcOptimized, *pNumBoneCombinations * sizeof(D3DXBONECOMBINATION));


        // fill in reordered attributes

        pNewMesh2->LockAttributeBuffer(0, &pAttrib);

        for (iAttr = 0; iAttr < pNewMesh2->GetNumFaces(); iAttr++)
        {
            pAttrib[iAttr]  = rgiOrder[pAttrib[iAttr]];
        }


        // copy adjacency info into temporary adjacency array

        if (pAdjacencyOut != NULL)  // adjacency info exists
        {
            memcpy(pAdjacencyTemp, pAdjacencyOut, numFaces * 3 * sizeof(DWORD));
        }


        // sort attributes

        hr  = pNewMesh2->Optimize2
                         (
                              D3DXMESH_SYSTEMMEM,
                              D3DXMESHOPT_ATTRSORT,
                              pAdjacencyTemp, 
                              pAdjacencyOut, 
                              faceRemap, 
                              &pVertRemapBuf3, 
                              pNewMesh2->GetFVF(), 
                              &pNewMesh3
                         );                         // INVARIANT:  AttrTable count is same before and after Optimize
                                                    // INVARIANT:  pNewMesh3->GetFVF() == pNewMesh2->GetFVF()

        if (FAILED(hr))
            goto e_ExitTSP;

        
        // update bone combination face & vertex ranges

        attrTable   = pNewMesh3->m_rgaeAttributeTable;        // TODO:  use GetAttribTable instead
        
        for (iNode = 0; iNode < *pNumBoneCombinations; iNode++)
        {
            pBoneCombination[iNode].FaceStart   = attrTable[iNode].FaceStart;

            pBoneCombination[iNode].FaceCount   = attrTable[iNode].FaceCount;

            pBoneCombination[iNode].VertexStart = attrTable[iNode].VertexStart;

            pBoneCombination[iNode].VertexCount = attrTable[iNode].VertexCount;
        }


        // get the byte size of each point (i.e. vertex) in the mesh

        cbPointSize     = cfvf.m_cBytesPerVertex;



        pNewMesh3->LockVertexBuffer(0, &pbPoints);

        DWORD rgiBones[4];
        DWORD rgiWeightOrder[4];


        iNode = 0;

        for (iInfl = 0; iInfl < 4; iInfl++)
        {
            rgiBones[iInfl] = pBoneCombination[iNode].BoneId[iInfl];
        }


        for (iNode = 1; iNode < *pNumBoneCombinations; iNode++)
        {
            for (DWORD iWeight = 0; iWeight < 4; iWeight++)
            {
                rgiWeightOrder[iWeight] = iWeight;
            }

            for (iWeight = 0; iWeight < 4; iWeight++)
            {
                DWORD iBone = rgiBones[iWeight];

                for (DWORD i = 0; i < 4; i++)
                {
                    if (iBone != UINT_MAX && iBone == pBoneCombination[iNode].BoneId[rgiWeightOrder[i]])
                    {
                        DWORD   temp = rgiWeightOrder[i];
                        rgiWeightOrder[i] = rgiWeightOrder[iWeight];
                        rgiWeightOrder[iWeight] = temp;
                    }
                }
            }


            for (iInfl = 0; iInfl < 4; iInfl++)
            {
                rgiBones[iInfl] = pBoneCombination[iNode].BoneId[rgiWeightOrder[iInfl]];
            }


            PBYTE   pbCurrPoint = (PBYTE)cfvf.GetArrayElem(pbPoints, pBoneCombination[iNode].VertexStart);

//          DPF(1, "%3d %3d %3d %3d|%3d", rgiBones[0], rgiBones[1], rgiBones[2], rgiBones[3], pBoneCombination[iNode].AttribId);

            for (i = 0; i < 4; i++)
            {
                pBoneCombination[iNode].BoneId[i] = rgiBones[i];
            }


            for (iPoint = 0; iPoint < pBoneCombination[iNode].VertexCount; iPoint++)
            {
                float* rgfWeights = cfvf.PfGetWeights(pbCurrPoint);

                float rgfWeights1[4];
                
                rgfWeights1[3]  = 1.0f;

                for (DWORD i = 0; i < 3; i++)
                {
                    rgfWeights1[i] = rgfWeights[i];
                    rgfWeights1[3] -= rgfWeights[i];
                }

                for (i = 0; i < 3; i++)
                {
                    cfvf.SetWeight(pbCurrPoint, i, rgfWeights1[rgiWeightOrder[i]]);
                }


                pbCurrPoint += cbPointSize;
            }
        }
       

       

        // clone mesh with the correct options

        // TODO:  try to use clone instead of Optimize2 & then manually copy the attribute table

        hr  = pNewMesh3->Optimize2
                 (
                      options,
                      D3DXMESHOPT_ATTRSORT,
                      pAdjacencyOut, 
                      NULL, 
                      faceRemap, 
                      &pVertRemapBuf4, 
                      pNewMesh3->GetFVF(), 
                      &pNewMesh4
                 );

        if (FAILED(hr))
            goto e_ExitTSP;



//      DPF(1, "ConvertToBlendedMesh:  %d original cost - %d savings = %d new cost (%.1f%% savings)", cCost, cSavings, cCost - cSavings, 100.0 * (float)cSavings / (float)cCost);


e_ExitTSP:      // TSP stands for "Traveling Salesman Problem", which is essentially what this optimization tries to solve

        delete[] rgNodes;
        delete[] rgbcOptimized;
        delete[] rgiOrder;
        delete[] pAdjacencyTemp;

        if (pbPoints != NULL)
        {
            pNewMesh3->UnlockVertexBuffer();

            pbPoints = NULL;
        }

        if (pAttrib != NULL)
        {
            pNewMesh2->UnlockAttributeBuffer();

            pAttrib = NULL;
        }

        GXRELEASE(pVertRemapBuf3);

        GXRELEASE(pVertRemapBuf4);

        GXRELEASE(pDevice);

        GXRELEASE(pNewMesh3);

#endif






#if 0 // DBG this check no longer works
    
        // Verify replicated vertices have the same matrix & weights so that there are no cracks during skinning
        // This is a concern when truncating weights
        for (j = 0; j < *pNumBoneCombinations; ++j) // Cycle tru vertices via bone combination table
            for (i = 0; i < pBoneCombination[j].VertexCount; ++i) // For each vertex of a particular bone combination
            {
                DWORD v = i + pBoneCombination[j].VertexStart;
                if (pointRep[v] != v)
                {
                    // Find the table entry k of the representative vertex
                    for (DWORD k = 0; k < *pNumBoneCombinations; ++k)
                    {
                        if (pointRep[v] >= pBoneCombination[k].VertexStart && 
                            pointRep[v] < pBoneCombination[k].VertexStart + pBoneCombination[k].VertexCount)
                            break;
                    }
                    GXASSERT(k < *pNumBoneCombinations); // Must find k

                    LPBYTE pVert1 = (LPBYTE)cfvfout.GetArrayElem(pVertOut, v);
                    LPBYTE pVert2 = (LPBYTE)cfvfout.GetArrayElem(pVertOut, pointRep[v]);
                    for (DWORD l = 0; l < cMaxFaceInfl; ++l) // For each non-zero weight
                    {
                        float w1, w2;
                        if (l == cMaxFaceInfl - 1)
                        {
                            w1 = 1.0f;
                            for (DWORD m = 0; m < l; ++m)
                                w1 -= ((float*)(pVert1 + sizeof(D3DVECTOR)))[m];
                        }
                        else
                            w1 = ((float*)(pVert1 + sizeof(D3DVECTOR)))[l];
                        if (w1 > 0.0001f)
                        {
                            for (DWORD m = 0; m < cMaxFaceInfl; ++m) // Find a match in the representative vertex
                            {
                                if (pBoneCombination[j].BoneId[l] == pBoneCombination[k].BoneId[m])
                                {
                                    if (m == cMaxFaceInfl - 1)
                                    {
                                        w2 = 1.0f;
                                        for (DWORD n = 0; n < m; ++n)
                                            w2 -= ((float*)(pVert2 + sizeof(D3DVECTOR)))[n];
                                    }
                                    else
                                        w2 = ((float*)(pVert2 + sizeof(D3DVECTOR)))[m];
                                    if (fabs(w1 - w2) > 0.1)
                                        DPF(1, "Vert %d & Vert[Prep[%d]=%d] differ", v, v, pointRep[v]);
                                    break;
                                }
                            }
                            GXASSERT(m < cMaxFaceInfl); // Must find m that matches
                        }
                    }
                }
            }
        delete [] pointRep;
        pointRep = NULL;
#endif

    }
    // Free remap data
    delete [] faceRemap;
    faceRemap = NULL;
    vertRemap1 = NULL;
    vertRemap2 = NULL;
    pVertRemapBuf1->Release();
    pVertRemapBuf2->Release();

	pNewMesh2->UnlockVertexBuffer();
    pVertOut = NULL;
	pNewMesh2->UnlockIndexBuffer();
    pFacesNew = NULL;


    if (pNewMesh4)
    {
        pNewMesh2->Release();

        *ppMesh = pNewMesh4;
    }
    else
        *ppMesh = pNewMesh2;


    return hr;

e_Exit:
    // Error cleanup
    delete [] faceWeights;
    delete [] faceAttr;
    delete [] faceInfl;
    delete [] faceMatId;
    delete [] pointRep;
    delete [] faceRemap;
    delete [] pAdjacencyTemp;
    return hr;
}

DWORD CD3DXSkinMesh::GetNumBoneInfluences(DWORD bone)
{
    if (bone < m_numBones)
    {
        return m_pBones[bone].m_numWeights;
    }
    else
    {
        DPF(0, "Invalid bone nummber");
        return 0;
    }
}
	
HRESULT CD3DXSkinMesh::GetBoneInfluence(DWORD bone, 
							 DWORD* vertices,
							 float* weights)
{
    if (bone < m_numBones)
    {
        memcpy(vertices, m_pBones[bone].m_pVertIndices, m_pBones[bone].m_numWeights * sizeof(DWORD));
        memcpy(weights, m_pBones[bone].m_pWeights, m_pBones[bone].m_numWeights * sizeof(float));
        return S_OK;
    }
    else
    {
        DPF(0, "Invalid bone number");
        return D3DERR_INVALIDCALL;
    }
}
	
HRESULT CD3DXSkinMesh::GetMaxVertexInfluences(DWORD* maxVertexInfluences)
{
    *maxVertexInfluences = 0;

    if (!(m_DataValid & D3DXSM_VERTINFL))
    {
        // Compute max influences per vertex
        m_maxVertInfl = 0;


        memset(m_vertInfl, 0, m_pMesh->m_cVertices * sizeof(DWORD));

	    for (DWORD i = 0; i < m_numBones; ++i)
        {
		    CBone* pBone = &m_pBones[i];
            for (DWORD j = 0; j < pBone->m_numWeights; ++j)
            {
                m_vertInfl[pBone->m_pVertIndices[j]]++;
                if (m_vertInfl[pBone->m_pVertIndices[j]] > m_maxVertInfl)
                    m_maxVertInfl = m_vertInfl[pBone->m_pVertIndices[j]];
            }
        }
        m_DataValid |= D3DXSM_VERTINFL;
    }
    *maxVertexInfluences = m_maxVertInfl;
    return S_OK;
}
	
HRESULT CD3DXSkinMesh::GetMaxFaceInfluences(DWORD* maxFaceInfluences)
{
    if (!(m_DataValid & D3DXSM_FACEINFL))
    {
        WORD (*pFaces)[3] = NULL;
        HRESULT hr = CalcVertexSkinData();
        if (hr != S_OK)
        {
            return hr;
        }

        DWORD* faceMatId = new DWORD[m_maxVertInfl * 3]; // upper limit for maxFaceInfl
        if (faceMatId == NULL)
        {
            DPF(0, "Out of Memory");
            return E_OUTOFMEMORY;
        }

        m_maxFaceInfl = 0;
        m_pMesh->LockIB((PBYTE*)&pFaces);
        for (DWORD i = 0; i < m_pMesh->m_cFaces; ++i)
        {
            DWORD faceInfl = 0;
            memset(faceMatId, 0xff, sizeof(DWORD) * m_maxVertInfl * 3);

            for (DWORD j = 0; j < 3; ++j)
            {
                DWORD v = pFaces[i][j];
                for (DWORD k = 0; k < m_vertInfl[v]; ++k)
                {
                    for (DWORD l = 0; l < faceInfl; ++l)
                    {
                        if (faceMatId[l] == m_vertMatId[v * m_maxVertInfl + k])
                        {
                            break;
                        }
                    }
                    if (l == faceInfl)
                    {
                        faceMatId[l] = m_vertMatId[v * m_maxVertInfl + k];
                        faceInfl++;
                    }
                }
            }
            if (faceInfl > m_maxFaceInfl)
            {
                m_maxFaceInfl = faceInfl;
            }
        }
        m_pMesh->UnlockIB();
        pFaces = NULL;
        m_DataValid |= D3DXSM_FACEINFL;
    }
    *maxFaceInfluences = m_maxFaceInfl;
    return S_OK;
}


HRESULT CD3DXSkinMesh::GenerateCodes()
{
    HRESULT hr  = S_OK;


    // get the number of points (i.e. vertices) in the source mesh

    DWORD   cPoints = m_pMesh->GetNumVertices();

    
    DWORD   cMaxInfluencesPerVertex;


    // get the maximum number of influences per vertex

    hr = GetMaxVertexInfluences(&cMaxInfluencesPerVertex);

    if (FAILED(hr))
        return hr;


    float*	rgfCodeTable	= NULL;
    DWORD*  rgcInfluences   = NULL;


    // allocate memory for a temporary code table

    rgfCodeTable = new float[cPoints * cMaxInfluencesPerVertex];

	if (!rgfCodeTable)
    {
        hr  = E_OUTOFMEMORY;

        goto e_Exit1;
    }


    // initialize the code table to zeroes

	memset(rgfCodeTable, 0, cPoints * cMaxInfluencesPerVertex * sizeof(float));   // initialize weights to 0


    // allocate memory for a temporary per vertex influence count array

	rgcInfluences	= new DWORD[cPoints];

	if (!rgcInfluences)
    {
        hr  = E_OUTOFMEMORY;

        goto e_Exit1;
    }


    // initialize the influence count array to zero

	memset(rgcInfluences, 0, cPoints * sizeof(DWORD));    // initialize influence counts to 0


    
    CBone*  pBone;

    DWORD   cCodes;

    DWORD iWeight, iPoint, iBone;

    // initialize the bone pointer to point to the first bone

    pBone   = m_pBones;




    // FILL IN THE CODE TABLE



	for (iBone = 0; iBone < m_numBones; ++iBone)
	{
		float   f2xBone     = (float)(iBone << 1);


        for (iWeight = 0; iWeight < pBone->m_numWeights; ++iWeight)
		{
            // get the affected vertex

			DWORD   iVertex	= pBone->m_pVertIndices[iWeight];


            // code = boneID * 2 + weight

			float   fCode	= f2xBone + pBone->m_pWeights[iWeight];
				
			rgfCodeTable[iVertex * cMaxInfluencesPerVertex + rgcInfluences[iVertex]]	= fCode;


            // update the influence count array

			rgcInfluences[iVertex]++;
		}



        // increment the bone pointer 

        ++pBone;
	}





    // CLAMP THE WEIGHTS



    float*  rgfCodes;
    DWORD*  pcInfluences;


    // initialize total code count to zero

    cCodes  = 0;

    
    // intialize vertex code array pointer

    rgfCodes        = rgfCodeTable;


    // initialize influence count pointer

    pcInfluences    = rgcInfluences;


    // run through the vertices, clamping the weights

    for (iPoint = 0; iPoint < cPoints; ++iPoint)
    {
        // initialize vertex influence count

        DWORD   cInfluences = 0;


        // initialize vertex weight sum

        float   fWeightSum  = 0.0f;


        for (iWeight = 0; iWeight < *pcInfluences; iWeight++)
        {
            // decode weight

            float   fCode   = rgfCodes[iWeight];

            DWORD   iBone   = ((DWORD)fCode) >> 1;

            float   f2xBone = (float)(iBone << 1);

            float   fWeight = fCode - f2xBone;



            if (fWeight >= m_fMinWeight)     // if the weight is at least minimum allowed
            {
                // save the code

                rgfCodes[cInfluences]   = fCode;


                // update vertex weight sum

                fWeightSum  += fWeight;


                // update vertex influence count

                cInfluences++;
            }
        }


        if (cInfluences)        // there's at least one influence after clamping
        {
            // update influence count array

            *pcInfluences   = cInfluences;


            for (iWeight = 0; iWeight < cInfluences; iWeight++)
            {
                // decode weight

                float   fCode   = rgfCodes[iWeight];

                DWORD   iBone   = ((DWORD)fCode) >> 1;

                float   f2xBone = (float)(iBone << 1);

                float   fWeight = fCode - f2xBone;
    

                // save encoded normalized weight
            
                rgfCodes[iWeight]   = f2xBone + (fWeight / fWeightSum);
            }
        }


        // update the total code count

		cCodes  += *pcInfluences;

        
        // imcrement vertex code array pointer

        rgfCodes   += cMaxInfluencesPerVertex;


        // increment influence count pointer

        pcInfluences++;


    }





    // SAVE THE CODE TABLE INTO THE CODE ARRAY


    // reallocate memory for the code array

    if (m_rgfCodes)
        delete[] m_rgfCodes;

	m_rgfCodes	= new float[cCodes + cPoints];

	if (!m_rgfCodes)
    {
        hr  = E_OUTOFMEMORY;

        goto e_Exit1;
    }



    float*  pfCodeSrce;
    float*  pfCodeDest;


    // initialize influence count pointer

    pcInfluences    = rgcInfluences;


    // initialize source code pointer

    pfCodeSrce      = rgfCodeTable;


    // initialize destination code pointer

    pfCodeDest      = m_rgfCodes;


    // run through the vertices, filling in the codes from the code table

	for (iPoint = 0; iPoint < cPoints; ++iPoint)
	{
        if (*pcInfluences > 0)  // there is at least one influence
        {
            // copy codes from code table to code array

    		memcpy(pfCodeDest, pfCodeSrce, *pcInfluences * sizeof(float));


            // update destination code pointer

            pfCodeDest += *pcInfluences;
        }


        // set code to negative to indicate end of influences for this vertex

        *pfCodeDest = -1.0f;


        // increment influence count pointer

        pcInfluences++;


        // increment source code pointer

        pfCodeSrce  += cMaxInfluencesPerVertex;


        // increment destination code pointer

        pfCodeDest++;
    }

e_Exit1:

    // free memory from code table

    if (rgfCodeTable)
        delete[] rgfCodeTable;


    // free memory from influence count array

    if (rgcInfluences)
        delete[] rgcInfluences;


    // return if there were errors

    if (FAILED(hr))
        return hr;


    // indicate that the code array is up to date

    m_bChangedWeights	= false;


    return hr;
}


HRESULT CD3DXSkinMesh::GenerateSkinnedMesh(DWORD options, FLOAT fMinWeight, CONST LPDWORD rgiAdjacencyIn, LPDWORD rgiAdjacencyOut, LPD3DXMESH* ppMesh)
{
	HRESULT	hr	= S_OK;

    // get the number of points (i.e. vertices) in the source mesh

    DWORD   cPoints = m_pMesh->GetNumVertices();


    // TODO: remove this limitation
    if (options & D3DXMESH_32BIT)
    {
        DPF(0, "32-bit meshes are not yet supported");
        return E_NOTIMPL;
    }

    LPDIRECT3DDEVICE8   pDevice = NULL;
    
    DWORD*  rgiVertexRemap;

    GXTri3Mesh<tp16BitIndex>* pOptMesh    = NULL;

    DWORD*  rgcPointReps    = NULL;
    DWORD*  rgiPointReps    = NULL;
    DWORD** rgpPointReps    = NULL;

    float*  rgfWeights  = NULL;
    DWORD*  rgiIndices  = NULL;

    DWORD   cPoints_;
    DWORD   cWeights;
    DWORD   iWeight, iPoint, iBone, iPoint_, iWeight_;
    float   fWeight;


    DWORD*          rgiFaceRemap    = NULL;

    LPD3DXBUFFER    pVertexRemap    = NULL;

    // allocate face remap array

    rgiFaceRemap    = new DWORD[m_pMesh->GetNumFaces()];

    if (rgiFaceRemap == NULL)
    {
        hr  = E_OUTOFMEMORY;

        goto e_Exit0;
    }


    // attribute sort the original mesh
    
    hr = m_pMesh->Optimize2
                  (
                      m_pMesh->GetOptions(),
                      D3DXMESHOPT_ATTRSORT,
                      rgiAdjacencyIn,
                      rgiAdjacencyOut,
                      rgiFaceRemap,
                      &pVertexRemap,
                      m_pMesh->GetFVF(),
                      &pOptMesh
                  );

    if (FAILED(hr))
        goto e_Exit0;


    // get vertex remap array

    rgiVertexRemap  = reinterpret_cast<DWORD*>(pVertexRemap->GetBufferPointer()); 


    // save old point (i.e. vertex) count

    cPoints_    = cPoints;


    // load new point (i.e. vertex) count

    cPoints     = pOptMesh->GetNumVertices();



    // allocate rep count array

    rgcPointReps = new DWORD[cPoints_];

    if (rgcPointReps == NULL)
    {
        hr  = E_OUTOFMEMORY;

        goto e_Exit0;
    }


    // initialize rep count array to zeros

    memset(rgcPointReps, 0, cPoints_ * sizeof(DWORD));

    
    // allocate rep index array

    rgiPointReps = new DWORD[cPoints_];

    if (rgiPointReps == NULL)
    {
        hr  = E_OUTOFMEMORY;

        goto e_Exit0;
    }


    // initialize rep index array to zeros

    memset(rgiPointReps, 0, cPoints_ * sizeof(DWORD));

    
    // allocate rep pointer array

    rgpPointReps = new DWORD*[cPoints_];

    if (rgpPointReps == NULL)
    {
        hr  = E_OUTOFMEMORY;

        goto e_Exit0;
    }


    // initalize rep pointer array to NULL

    memset(rgpPointReps, 0, cPoints_ * sizeof(DWORD*));


    // calculate rep count array

    for (iPoint = 0; iPoint < cPoints; iPoint++)
    {
        rgcPointReps[rgiVertexRemap[iPoint]]++; 
    }


    // allocate rep arrays

    for (iPoint_ = 0; iPoint_ < cPoints_; iPoint_++)
    {
        rgpPointReps[iPoint_]   = new DWORD[rgcPointReps[iPoint_]]; 

        if (rgpPointReps[iPoint_] == NULL)
        {
            hr  = E_OUTOFMEMORY;

            goto e_Exit0;
        }
    }


    // fill in rep arrays

    for (iPoint = 0; iPoint < cPoints; iPoint++)
    {
        iPoint_ = rgiVertexRemap[iPoint];

        rgpPointReps[iPoint_][rgiPointReps[iPoint_]]    = iPoint;

        rgiPointReps[iPoint_]++;
    }


    // run through the bones

    for (iBone = 0; iBone < m_numBones; iBone++)
    {

        // initialize influence count to zero

        cWeights    = 0;

        for (iWeight_ = 0; iWeight_ < m_pBones[iBone].m_numWeights; iWeight_++)
        {
            cWeights    += rgcPointReps[m_pBones[iBone].m_pVertIndices[iWeight_]];
        }


        // allocate weight array

        rgfWeights  = new float[cWeights];  

        if (rgfWeights == NULL)
        {
            hr  = E_OUTOFMEMORY;

            goto e_Exit0;
        }


        // allocate vertex indices array

        rgiIndices  = new DWORD[cWeights];  

        if (rgiIndices == NULL)
        {
            hr  = E_OUTOFMEMORY;

            goto e_Exit0;
        }


        // fill in vertex indices and weight arrays

        iWeight = 0;

        for (iWeight_ = 0; iWeight_ < m_pBones[iBone].m_numWeights; iWeight_++)
        {
            fWeight = m_pBones[iBone].m_pWeights[iWeight_];

            for (DWORD iRep = 0; iRep < rgcPointReps[m_pBones[iBone].m_pVertIndices[iWeight_]]; iRep++)
            {
                rgfWeights[iWeight] = fWeight;
                rgiIndices[iWeight] = rgpPointReps[m_pBones[iBone].m_pVertIndices[iWeight_]][iRep];

                iWeight++;
            }
        }


        // free the old weight & vertex indices arrays

        delete[] m_pBones[iBone].m_pWeights;

        delete[] m_pBones[iBone].m_pVertIndices;


        // update the weight & vertex indices array pointers

        m_pBones[iBone].m_pWeights      = rgfWeights;

        m_pBones[iBone].m_pVertIndices  = rgiIndices;


        // update the weight count

        m_pBones[iBone].m_numWeights    = cWeights;


        // set the old pointers to NULL

        rgfWeights  = NULL;

        rgiIndices  = NULL;
    }


    // release the original mesh

    GXRELEASE(m_pMesh);


    // set the original mesh pointer to the optimized mesh

    m_pMesh = pOptMesh;


    // get the mesh's device

    hr  = m_pMesh->GetDevice(&pDevice);

    if (FAILED(hr))
        goto e_Exit0;


    // clone the mesh

    hr  = m_pMesh->CloneMeshFVF
                   (
                       options | D3DXMESH_VB_DYNAMIC,
                       m_pMesh->GetFVF(),
                       pDevice,
                       ppMesh
                   );

    if (FAILED(hr))
        goto e_Exit0;


    // reallocate m_vertInfl since number of vertices may have changed

    delete[] m_vertInfl;

    m_vertInfl  = new DWORD[m_pMesh->m_cVertices];

    if (m_vertInfl == NULL)
        goto e_Exit0;

    memset(m_vertInfl, 0, m_pMesh->m_cVertices * sizeof(DWORD));


    // INVARIANT:  m_maxVertInfl remains same

    // INVARIANT:  m_maxFaceInfl remains same

    // INVARIANT:  m_numAttrib remains same (TODO:  make sure of this)


    // free matrix id & matrix weight arrays

    if (m_vertMatId != NULL)
    {
        delete[] m_vertMatId;

        m_vertMatId     = NULL;
    }

    if (m_vertMatWeight != NULL)
    {
        delete[] m_vertMatWeight;

        m_vertMatWeight = NULL;
    }


    // recalculate matrix id & matrix weight arrays

    hr  = CalcVertexSkinData();

    if (FAILED(hr))
        goto e_Exit0;


    // set the min weight clamp

    m_fMinWeight    = fMinWeight;

    
    // recalculate codes

    hr  = GenerateCodes();

    if (FAILED(hr))
        goto e_Exit0;


    // indicate that codes have been calculated

    m_bChangedWeights   = false;


e_Exit0:

   
    // indicate that data needs to be recalculated

    m_DataValid     = 0;


    // cleanup

    if (rgpPointReps)
    {
        for (DWORD iPoint_ = 0; iPoint_ < cPoints_; iPoint_++)
        {
            delete[] rgpPointReps[iPoint_];
        }
    }

    delete[] rgpPointReps;

    delete[] rgcPointReps;

    delete[] rgiPointReps;

    
    delete[] rgiFaceRemap;


    delete[] rgfWeights;

    delete[] rgiIndices;


    rgiVertexRemap = NULL;

    GXRELEASE(pVertexRemap);


    GXRELEASE(pDevice);


    return hr;
}

HRESULT CD3DXSkinMesh::UpdateSkinnedMesh(CONST D3DXMATRIX* pBoneTransforms, LPD3DXMESH pMesh)
{
	HRESULT	hr	= S_OK;


    // recalulate codes if necessary

	if (m_bChangedWeights)	    // influence weights have changed
	{
        // reallocate & recalculate code array

        hr  = GenerateCodes();

        if (FAILED(hr))
            return hr;


        // indicate that the code array is up to date

        m_bChangedWeights	= false;
	}


    



    // SKIN THE MESH USING THE CODE ARRAY




    // get the number of points (i.e. vertices) in the source mesh

    DWORD   cPoints = m_pMesh->GetNumVertices();

    
    // create classes to read/write the source & destination mesh's vertex buffers

    DXCrackFVF  cfvfSrce(m_pMesh->GetFVF()),
                cfvfDest(pMesh->GetFVF());






    // calculate source and destination pointer increments

    DWORD   cbPointSizeSrce = cfvfSrce.m_cBytesPerVertex;
    DWORD   cbPointSizeDest = cfvfDest.m_cBytesPerVertex;




#if DBG     // check whether source & destination meshes are compatible


    // check whether source and destination meshes have same number of vertices

    if (pMesh->GetNumVertices() != cPoints)
    {
        DPF(0, "UpdateSkinnedMesh:  Vertex count mismatch (%d input vertices, %d output vertices)", cPoints, pMesh->GetNumVertices());

        return E_FAIL;
    }

    if (m_pMesh->GetFVF() != pMesh->GetFVF())
    {
        DPF(0, "UpdateSkinnedMesh:  FVF mismatch between source & destination meshes.");

        return E_FAIL;
    }


#endif







    PBYTE   pbData  = NULL;

    PBYTE   pbSrce  = NULL;
    PBYTE   pbDest  = NULL;
 
	PBYTE   pbCurrSrce;
	PBYTE   pbCurrDest;


    // lock source mesh vertex buffer for "read-only"

    hr  = m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, &pbSrce);

    if (FAILED(hr))
        goto e_Exit;


    // lock destination mesh vertex buffer to "write"

    hr  = pMesh->LockVertexBuffer(0, &pbDest);

    if (FAILED(hr))
        goto e_Exit;


    // initialize source & destination mesh's current pointers

    pbCurrSrce  = pbSrce;
    pbCurrDest  = pbDest;


    // allocate temporary point data storage

    pbData  = new BYTE[cbPointSizeSrce];

    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;

        goto e_Exit;
    }


    if (cfvfSrce.BNormal() && cfvfDest.BNormal())     // source mesh has normals
    {
        
        D3DXVECTOR3	    vVertexSrce;
        D3DXVECTOR3		vVertexDest;

        D3DXVECTOR3	    vNormalSrce;
        D3DXVECTOR3		vNormalDest;


        // initialize code pointer

        float*  pfCode  = m_rgfCodes;


        // run through each vertex, calculating the skinned position & normal

	    for (DWORD iPoint = 0; iPoint < cPoints; ++iPoint)
	    {
            // load point data

            memcpy(pbData, pbCurrSrce, cbPointSizeSrce * sizeof(BYTE));


            // read source position & normal

            vVertexSrce  = *cfvfSrce.PvGetPosition(pbData);
		    vNormalSrce  = *cfvfSrce.PvGetNormal(pbData);


            // initialize blend tranform to zero

            D3DXMATRIX mBoneTransform
                       (
                           0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f
                       );


            // calculate blend transform

		    while (*pfCode >= 0.0f) 
		    {
                // decode bone and weight

                DWORD   iBone   = ((DWORD)(*pfCode)) >> 1;

                float   fWeight	= (*pfCode) - (float)(iBone << 1);


                // add weighted influence

                mBoneTransform  += pBoneTransforms[iBone] * fWeight;


                // increment code pointer

                ++pfCode;
		    }


            // calculate blended vertex

            D3DXVec3TransformCoord(&vVertexDest, &vVertexSrce, &mBoneTransform);


            // calculate blended normal

            D3DXVec3TransformNormal(&vNormalDest, &vNormalSrce, &mBoneTransform);


            // write output position & normal

		    cfvfDest.SetPosition(pbData, &vVertexDest);
		    cfvfDest.SetNormal(pbData, &vNormalDest);


            // save point data

            memcpy(pbCurrDest, pbData, cbPointSizeSrce * sizeof(BYTE));     // INVARIANT:  cbPointSizeSrce == cbPointSizeDest
            

            // increment source & destination pointers

            pbCurrSrce += cbPointSizeSrce;
            pbCurrDest += cbPointSizeDest; 


            // increment code pointer

            ++pfCode;
        }
	}
    else        // source mesh does not have normals
    {
        D3DXVECTOR3	    vVertexSrce;
        D3DXVECTOR3		vVertexDest;


        // initialize code pointer

        float*  pfCode  = m_rgfCodes;


        // run through each vertex, calculating the skinned position & normal

	    for (DWORD iPoint = 0; iPoint < cPoints; ++iPoint)
	    {
            // load point data

            memcpy(pbData, pbCurrSrce, cbPointSizeSrce * sizeof(BYTE));

            
            // read source position

            vVertexSrce  = *cfvfSrce.PvGetPosition(pbData);


            // initialize blend tranform to zero

            D3DXMATRIX mBoneTransform
                       (
                           0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f
                       );


            // calculate blend transform

		    while (*pfCode >= 0.0f) 
		    {
                // decode bone and weight

                DWORD   iBone   = ((DWORD)(*pfCode)) >> 1;

                float   fWeight	= (*pfCode) - (float)(iBone << 1);


                // add weighted influence

                mBoneTransform  += pBoneTransforms[iBone] * fWeight;


                // increment code pointer

                ++pfCode;
		    }


            // calculate blended vertex

            D3DXVec3TransformCoord(&vVertexDest, &vVertexSrce, &mBoneTransform);


            // write output position

		    cfvfDest.SetPosition(pbData, &vVertexDest);


            // save point data

            memcpy(pbCurrDest, pbData, cbPointSizeSrce * sizeof(BYTE));     // INVARIANT:  cbPointSizeSrce == cbPointSizeDest

            
            // increment source & destination pointers

            pbCurrSrce += cbPointSizeSrce;
            pbCurrDest += cbPointSizeDest; 


            // increment code pointer

            pfCode++;
        }
    }


e_Exit:

    // unlock source mesh's vertex buffer

    if (pbSrce)
        m_pMesh->UnlockVertexBuffer();


    // unlock destination mesh's vertex buffer

    if (pbDest)
        pMesh->UnlockVertexBuffer();


    // release temporary point data storage

    if (pbData)
        delete[] pbData;



    return hr;
}

HRESULT CD3DXSkinMesh::ConvertToIndexedBlendedMesh(DWORD options,
						 CONST LPDWORD pAdjacencyIn,
                         DWORD paletteSize, 
						 LPDWORD pAdjacencyOut,
 					     DWORD* pNumBoneCombinations, 
                         LPD3DXBUFFER* ppBoneCombinationTable, 
						 LPD3DXMESH* ppMesh)
{
    GXTri3Mesh<tp16BitIndex>* pMeshOut = NULL;
    LPBYTE pVertOut = NULL;
    LPD3DXATTRIBUTERANGE attrTable = NULL;
    LPD3DXBONECOMBINATION pBoneCombination = NULL;
    DWORD* pBoneIds = NULL;
    DWORD i, j, k;
    DWORD numFaces = m_pMesh->m_cFaces;
    DWORD fvf = D3DFVF_XYZ;
    DWORD* origAttribIds = NULL;
    DWORD numMat = 0;
    DWORD attribBits;
    DWORD attrMask;
    HRESULT hr;

    // TODO: remove this limitation
    if (options & D3DXMESH_32BIT)
    {
        DPF(0, "32-bit meshes are not yet supported");
        return E_NOTIMPL;
    }

    // Is there any skinning info ?
    if (m_numBones == 0)
    {
        hr = m_pMesh->CloneMeshFVF(options, m_pMesh->GetFVF(), m_pMesh->m_pD3DDevice, ppMesh);
        return hr;
    }

    *ppBoneCombinationTable = NULL;
    *ppMesh = NULL;
    *pNumBoneCombinations = 0;

    CalcNumAttributes(&attrMask, &attribBits);

    hr = TruncVertexSkinData(m_faceClamp, pAdjacencyIn);
    if (hr != S_OK)
    {
        m_DataValid = 0;
        return hr;
    }
    DWORD maxVertInfl = min(m_maxVertInfl, m_faceClamp);

    if (paletteSize < maxVertInfl * 3)
    {
        DPF(0, "ConvertToIndexedBlendedMesh: Palette size %d too small. Need atleast %d", paletteSize, maxVertInfl * 3);
        m_DataValid = 0;
        return E_NOTIMPL;
    }

    DWORD* faceRemap = new DWORD[numFaces];
    if (faceRemap == NULL)
    {
        DPF(0, "Out of Memory");
        m_DataValid = 0;
        return E_OUTOFMEMORY;
    }

    DWORD numComb = 0;
    DWORD numUnassigned = numFaces;
    DWORD numCombAlloc = (DWORD)(m_numBones * 1.2 + paletteSize - 1) / paletteSize;
    WORD (*pFaces)[3] = NULL;
    WORD (*pFacesOut)[3] = NULL;
    LPD3DXBUFFER pVertRemapBuf = NULL;
    DWORD* matIdRemap;

    CD3DXBitField256* combBits = new CD3DXBitField256[numCombAlloc];
    CD3DXBitField256* faceInflBits = new CD3DXBitField256[numFaces];
    DWORD* faceAttr = new DWORD[numFaces];
    if (combBits == NULL ||
        faceInflBits == NULL ||
        faceAttr == NULL)
    {
        DPF(0, "Out of Memory");
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // Initialize to "unassigned"
    memset(faceAttr, 0xff, sizeof(DWORD) * numFaces);

    // Generate per face bit fields
    m_pMesh->LockIndexBuffer(0, (PBYTE*)&pFaces);
    for (i = 0; i < numFaces; ++i)
    {
        for (j = 0; j < 3; ++j)
        {
            DWORD v = pFaces[i][j];
            DWORD vertInfl = min(m_vertInfl[v], maxVertInfl);
            for (k = 0; k < vertInfl; ++k)
            {
                faceInflBits[i].Set(m_vertMatId[v * m_maxVertInfl + k]);
            }
        }
        GXASSERT(faceInflBits[i].Count() <= paletteSize);
    }
    m_pMesh->UnlockIndexBuffer();
    pFaces = NULL;

    // Accumalate faces in to palettes 
    while (numUnassigned)
    {
        DWORD bMat = FALSE;
        DWORD combAttrib;
        if (numComb >= numCombAlloc)
        {
            numCombAlloc = (DWORD)(ceil(numCombAlloc * 1.2));
            CD3DXBitField256* newCombBits = new CD3DXBitField256[numCombAlloc];
            if (newCombBits == NULL)
            {
                DPF(0, "Out of Memory");
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            for (i = 0; i < numComb; ++i)
            {
                newCombBits[i] = combBits[i];
            }
            delete [] combBits;
            combBits = newCombBits;
        }
        // Pack as many faces as possible in this palette
        // as long they have the same attibute Ids
        for (i = 0; i < numFaces; ++i)
        {
            if (faceAttr[i] != UINT_MAX)
                continue;
            CD3DXBitField256 t;
            t.Or(combBits[numComb], faceInflBits[i]);
            if (t.Count() <= paletteSize)
            {
                if (!bMat) // If this is the first face, record attrib
                {
                    combAttrib = m_pMesh->m_rgiAttributeIds[i];
                    bMat = TRUE;
                }
                if (m_pMesh->m_rgiAttributeIds[i] == combAttrib) // make sure attrib matches
                {
                    faceAttr[i] = numComb;
                    --numUnassigned;
                    combBits[numComb] = t;
                }
            }
        }
        ++numComb;
    }

    DPF(1, "ConvertToIndexedBlendedMesh: Number of combinations = %d", numComb);
    // Evil Hack: Temporarily substitute the new attributes to optimize
    origAttribIds = m_pMesh->m_rgiAttributeIds;
    m_pMesh->m_rgiAttributeIds = faceAttr;

    // Calculate fvf for new mesh
    if (maxVertInfl - 1)
    {
        fvf = D3DFVF_XYZRHW + maxVertInfl * 2; // compute FVF
    }
    fvf = (m_pMesh->GetFVF() & (~D3DFVF_POSITION_MASK)) | fvf; // new FVF

    // Optimize using new attribute IDs to new mesh
    DWORD* vertRemap;
    
    hr = m_pMesh->Optimize2(options, D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT,
                            pAdjacencyIn, pAdjacencyOut, faceRemap, &pVertRemapBuf, fvf, &pMeshOut);
    if (hr != S_OK)
    {
        goto e_Exit;
    }

    // Undo evil hack: Restore the original attribute Ids
    m_pMesh->m_rgiAttributeIds = origAttribIds;

    // Get the new mesh's attribute table
    attrTable = pMeshOut->m_rgaeAttributeTable;
    *pNumBoneCombinations = pMeshOut->m_caeAttributeTable;
    GXASSERT(*pNumBoneCombinations == numComb);
    // Generate the bone combination table
	hr = D3DXCreateBuffer((sizeof(D3DXBONECOMBINATION) + paletteSize * sizeof(DWORD))* *pNumBoneCombinations, ppBoneCombinationTable);
    if (hr != S_OK)
    {
        goto e_Exit;
    }
	pBoneCombination = static_cast<LPD3DXBONECOMBINATION>((*ppBoneCombinationTable)->GetBufferPointer());

    // set up BoneId pointers inside the buffer
    pBoneIds = reinterpret_cast<DWORD*>(pBoneCombination + *pNumBoneCombinations);

    matIdRemap = new DWORD[m_numBones * numComb];
    if (matIdRemap == NULL)
    {
        DPF(0, "Out of Memory");
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

#if DBG
    DWORD* pAttrib;
    pMeshOut->LockAttributeBuffer(D3DLOCK_READONLY, &pAttrib);
#endif
    for (i = 0; i < *pNumBoneCombinations; ++i)
    {
#if DBG
        for (j = 0; j < attrTable[i].FaceCount; ++j)
        {
            GXASSERT(pAttrib[attrTable[i].FaceStart + j] == i);
        }
#endif
		pBoneCombination[i].AttribId = m_pMesh->m_rgiAttributeIds[faceRemap[attrTable[i].FaceStart]];
		pBoneCombination[i].FaceCount = attrTable[i].FaceCount;
		pBoneCombination[i].FaceStart = attrTable[i].FaceStart;
		pBoneCombination[i].VertexCount = attrTable[i].VertexCount;
		pBoneCombination[i].VertexStart = attrTable[i].VertexStart;
        pBoneCombination[i].BoneId = pBoneIds + i * paletteSize;
        k = 0;
        for (j = 0; j < m_numBones; ++j)
        {
            if (combBits[i].IsSet(j))
            {
                pBoneCombination[i].BoneId[k] = j;
                matIdRemap[i * m_numBones + j] = k;
                ++k;
            }
        }
        for (; k < paletteSize; ++k)
        {
            pBoneCombination[i].BoneId[k] = UINT_MAX;
        }
        GXASSERT(attrTable[i].AttribId == i);
    }

    vertRemap = reinterpret_cast<DWORD*>(pVertRemapBuf->GetBufferPointer());

    if (maxVertInfl - 1)
    {
        DWORD comb = 0;
        DWORD count = 0;
        // Fill in weights and indices
        pMeshOut->LockVertexBuffer(0, &pVertOut);
#if DBG
        pMeshOut->LockIndexBuffer(0, (PBYTE*)&pFacesOut);
#endif
        DXCrackFVF  cfvfout(fvf);
        for (i = 0; i < pMeshOut->m_cVertices; ++i)
        {
#if DBG
            for (j = 0; j < numFaces; ++j)
            {
                for (k = 0; k < 3; ++k)
                {
                    if (pFacesOut[j][k] == i)
                    {
                        GXASSERT(j >= pBoneCombination[comb].FaceStart);
                        GXASSERT(j < pBoneCombination[comb].FaceStart + pBoneCombination[comb].FaceCount);
                        DWORD attr = faceAttr[faceRemap[j]];
                        GXASSERT(attr == comb);
                        GXASSERT(combBits[comb].IsSubset(faceInflBits[faceRemap[j]]));
                        DWORD v = vertRemap[i];
                        DWORD vertInfl = min(m_vertInfl[v], maxVertInfl);
                        for (DWORD l = 0; l < vertInfl; ++l)
                        {
                            DWORD matId = m_vertMatId[v * m_maxVertInfl + l];
                            GXASSERT(faceInflBits[faceRemap[j]].IsSet(matId));
                        }
                    }
                }
            }
#endif
            float* pOut = cfvfout.PfGetWeights(cfvfout.GetArrayElem(pVertOut, i));
            LPBYTE matIndices = (LPBYTE)(pOut + maxVertInfl - 1);
            DWORD v = vertRemap[i];
#if DBG
            float sum = 0.0f;
#endif
            for (j = 0; j < maxVertInfl - 1; ++j)
            {
                pOut[j] = m_vertMatWeight[v * m_maxVertInfl + j];
#if DBG
                sum += m_vertMatWeight[v * m_maxVertInfl + j];
#endif
            }
#if DBG
            sum += m_vertMatWeight[v * m_maxVertInfl + j];
//            GXASSERT(fabs(sum - 1.f) < 0.001);
#endif
            DWORD vertInfl = min(m_vertInfl[v], maxVertInfl);
            for (j = 0; j < vertInfl; ++j)
            {
                DWORD matId = m_vertMatId[v * m_maxVertInfl + j];
                GXASSERT(matId < m_numBones);
                matIndices[j] = (BYTE)matIdRemap[comb * m_numBones + matId];
                GXASSERT(pBoneCombination[comb].BoneId[matIdRemap[comb * m_numBones + matId]] == matId);
            }
            ++count;
            // Are we done with all vertices of this particular palette ?
            if (pBoneCombination[comb].VertexCount <= count)
            {
                ++comb; // Move to the next palette
                count = 0;
            }
        }
        pMeshOut->UnlockVertexBuffer();
#if DBG
        pMeshOut->UnlockIndexBuffer();
#endif
    }

    delete [] faceInflBits;
    faceInflBits = NULL;
    delete [] combBits;
    combBits = NULL;
    delete [] faceAttr;
    faceAttr = NULL;
    delete [] faceRemap;
    faceRemap = NULL;
    delete [] matIdRemap;
    matIdRemap = NULL;

    *ppMesh = pMeshOut;
    m_DataValid = 0;
    return S_OK;
e_Exit:
    delete [] combBits;
    delete [] faceAttr;
    delete [] faceInflBits;
    delete [] faceRemap;
    delete [] matIdRemap;
    GXRELEASE(pMeshOut);
    m_DataValid = 0;
    return hr;
}

