/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       patch.cpp
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

#pragma warning(disable:4244)

float g_Scratch[16][36][4];
NV_PATCH_ALLOC_CACHE MyCache[MAX_EV_CACHE];

int SlotToAttribute(DWORD Slot)
{
    switch (Slot)
    {
    case D3DVSDE_POSITION:
    case D3DVSDE_BLENDWEIGHT:
    case D3DVSDE_NORMAL:
    case D3DVSDE_DIFFUSE:
    case D3DVSDE_SPECULAR:
    case D3DVSDE_TEXCOORD0:
    case D3DVSDE_TEXCOORD1:
    case D3DVSDE_TEXCOORD2:
    case D3DVSDE_TEXCOORD3:
        return Slot;
    }
    return -1;
}

DWORD g_PatchArraySize = 0;
CPatch** g_PatchArray;

//------------------------------------------------------------------------------
// FindPatch

inline CPatch* FindPatch(DWORD dwHandle)
{
    if (dwHandle < g_PatchArraySize)
    {
        return g_PatchArray[dwHandle];
    }
    else
    {
        return NULL;
    }
}

//------------------------------------------------------------------------------
// RemovePatch

inline void RemovePatch(DWORD dwHandle)
{
    g_PatchArray[dwHandle] = NULL;
}

//------------------------------------------------------------------------------
// AddPatch

BOOL AddPatch(CPatch *p, DWORD dwHandle)
{
    CPatch** NewPatchArray;
    DWORD count = dwHandle + 1;

    if (count > g_PatchArraySize)
    {
        DWORD newCount = (count + 16) & ~0xf;

        NewPatchArray = (CPatch**)MemAlloc(sizeof(PVOID) * newCount);
        if (NewPatchArray == NULL)
        {
            return FALSE;
        }

        if (g_PatchArray != NULL)
        {
            memcpy(NewPatchArray, g_PatchArray, count * sizeof(PVOID));
            MemFree(g_PatchArray);
        }

        g_PatchArray = NewPatchArray;
        g_PatchArraySize = newCount;
    }

    g_PatchArray[dwHandle] = p;

    return TRUE;
}

//------------------------------------------------------------------------------
// D3DDevice_DrawRectPatch

extern "C"
HRESULT WINAPI D3DDevice_DrawRectPatch(
    UINT dwHandle,
    CONST float* pfSegs,
    CONST D3DRECTPATCH_INFO* pRSInfo) 
{ 
    COUNT_API(API_D3DDEVICE_DRAWRECTPATCH);

    // PERF: Because the driver needs to generate the forward differencing
    //       values from the stream data using the CPU, it would be best if 
    //       we used a cached view.  But since we have no cheap means of 
    //       flushing the cache, we can't mix cacheable and uncacheable
    //       views.  We might need a new sys-mem VB construct for patches?
    //
    // NOTE: I think the API starts reading at offset 0 of the VB?  Shouldn't
    //       there be a means of specifying a different offset?  (Otherwise
    //       there has to be a separate VB per patch?)


    float Segs[4];
    CPatch *pPatch = NULL;
    DWORD  num_subpatches = 0;
    DWORD dwDirty = 0;
    DWORD retVal = D3D_OK;
    int i;
    CDevice *pDevice = g_pDevice;

    pDevice->StartBracket();

    //if no pfSegs info get it from the renderstate
    if (pfSegs == NULL)
    {
        Segs[0] = Segs[1] = Segs[2] = Segs[3] = *(float*)(&(D3D__RenderState[D3DRS_PATCHSEGMENTS]));
    }
    else
    {
        if (D3D__RenderState[D3DRS_PATCHEDGESTYLE] == 0x1)
        {
            for (i = 0; i < 4; i++)
            {
                Segs[i] = pfSegs[i];
            }
        }
        else
        {
            for (i = 0; i < 4; i++)
            {
                Segs[i] = int(pfSegs[i]);
            }
        }
    }

    //check to see if we've cached this patch.
    //Note a handle of 0 implies NOT to cache the patch.
    if (dwHandle != 0)
    {
        pPatch = FindPatch(dwHandle);
    }
        
    if (pPatch != NULL)
    {
        if (Segs[0] != pPatch->getSeg(0) || Segs[1] != pPatch->getSeg(1) ||
                Segs[2] != pPatch->getSeg(2) || Segs[3] != pPatch->getSeg(3))
        {
            //tessellation has changed recalc parameterization
            pPatch->setCalc(NV_PATCH_DIRTY_TESSELLATION);
            pPatch->setTessellation(Segs, NV_PATCH_FLAG_TYPE_PATCH);
            dwDirty = NV_PATCH_DIRTY_TESSELLATION;
        }
        else
        {
            //used cached FD params directly
            pPatch->setCalc(NV_PATCH_DIRTY_NONE);
        }
    }

    //patch hasn't been cached -- build a new one
    if (pPatch == NULL || pRSInfo != NULL)
    {
        if (pPatch != NULL)
        {
            pPatch->UnInit();
            MemFree(pPatch);
        }

        pPatch = (CPatch*)MemAlloc(sizeof(CPatch));
        if (!pPatch->Init(dwHandle))
        {
            return E_OUTOFMEMORY;
        }

        pPatch->setTessellation(Segs, NV_PATCH_FLAG_TYPE_PATCH);
        pPatch->setOriginalTessellation(Segs, NV_PATCH_FLAG_TYPE_PATCH);
    }

    if (pRSInfo != NULL)
    {
        //given this is a new cached patch  assume there is a valid pRSInfo.
        //initialize the patch structure
        pPatch->setDimension(pRSInfo->Width, pRSInfo->Height);
        pPatch->setBasis((NV_PATCH_BASIS_TYPE)pRSInfo->Basis);    //int cast to deal with enum casts.
        pPatch->setOrder(pRSInfo->Order);
        pPatch->setStride(pRSInfo->Stride);                       //IS THIS RIGHT??

        //alwasy set as fractional tessellation until they get their renderstate defined.
        if (D3D__RenderState[D3DRS_PATCHEDGESTYLE] == 0x1)
        {
            pPatch->setTessMode(NV_PATCH_FLAG_TESS_FRACTIONAL);
        }
        else
        {
            pPatch->setTessMode(NV_PATCH_FLAG_TESS_INTEGER);
        }

        //do this AFTER the normal/uv/stream setup cause they use these variable to figure out what to allocate
        pPatch->setCalc(NV_PATCH_DIRTY_TESSELLATION | NV_PATCH_DIRTY_CONTROL_POINTS);
        dwDirty = NV_PATCH_DIRTY_TESSELLATION | NV_PATCH_DIRTY_CONTROL_POINTS;

        //defined in # of vertices in a logical 2 dimensional Vertex buffer
        pPatch->setVertexOffset(pRSInfo->StartVertexOffsetHeight * pRSInfo->Stride + pRSInfo->StartVertexOffsetWidth);

        //copy data to scratch.
        pPatch->setupStreams();
    }

    switch (pPatch->getBasis())
    {
    case NV_PATCH_BASIS_BEZIER:

        //if bezier don't cycle through multiple patches in the width/height fashion as bsplines
        //if we dirtied the control points -- free them up and reallocate some cached space for them.
        if (dwDirty & NV_PATCH_DIRTY_CONTROL_POINTS)
        {
            pPatch->copyDataToScratch(0,0);
            pPatch->freeCached();
            if(!pPatch->allocCached(1))
            {
                retVal = D3DERR_DRIVERINTERNALERROR;
                goto nvPatch_Mem_Fault;
            }

            pPatch->getCachedData(0);
        }

        pPatch->setTextureOffsets(0.0,1.0,0.0,1.0);
        pPatch->setupStreams();

        retVal = nvEvalPatchSetup(pPatch->getInfo());    
        nvAllocInfoScratch(pPatch->getInfo());
        if (dwDirty & NV_PATCH_DIRTY_CONTROL_POINTS)
        {
            nvAllocFDMatricesEtc(pPatch->getInfo());
            retVal |= nvEvalPatch(pPatch->getInfo());
            pPatch->saveCachedData(0);
        }
        else
        {
            retVal |= nvEvalPatch(pPatch->getInfo());
        }
        break;

    case NV_PATCH_BASIS_BSPLINE:
    case NV_PATCH_BASIS_CATMULL_ROM:
        DWORD upatches = pPatch->getWidth()  - pPatch->getOrder();
        DWORD vpatches = pPatch->getHeight() - pPatch->getOrder();
        float ustep,vstep;

        //allocate some data for
        if (dwDirty & NV_PATCH_DIRTY_CONTROL_POINTS)
        {
            pPatch->freeCached();     //free up old cached data and reinit for a new patch to be rebuilt.
            if (!pPatch->allocCached(upatches * vpatches))
            {
                retVal = D3DERR_DRIVERINTERNALERROR;
                goto nvPatch_Mem_Fault;
            }

        }

        //reset the tessellation value here to account for subpatches.  The tessellation is meant to describe
        //the ENTIRE patch, not each subpatch.  Divide the tessellation by the # of subpatches in each direction.
        if (dwDirty)
        {
            if (D3D__RenderState[D3DRS_PATCHEDGESTYLE] == 0x1)
            {
                //fractional
                Segs[0] = max(1.0,Segs[0]/upatches+0.5); Segs[1] = max(1.0,Segs[1]/vpatches);
                Segs[2] = max(1.0,Segs[2]/upatches+0.5); Segs[3] = max(1.0,Segs[3]/vpatches);
            }
            else
            {
                //integer
                Segs[0] = max(1.0,int(Segs[0]/upatches+0.5)); Segs[1] = max(1.0,int(Segs[1]/vpatches+0.5));
                Segs[2] = max(1.0,int(Segs[2]/upatches+0.5)); Segs[3] = max(1.0,int(Segs[3]/vpatches+0.5));
            }

            switch (dwDirty)
            {
            case NV_PATCH_DIRTY_CONTROL_POINTS | NV_PATCH_DIRTY_TESSELLATION:
            case NV_PATCH_DIRTY_CONTROL_POINTS:
                pPatch->setOriginalTessellation(Segs, NV_PATCH_FLAG_TYPE_PATCH);
            case NV_PATCH_DIRTY_TESSELLATION:
                pPatch->setTessellation(Segs, NV_PATCH_FLAG_TYPE_PATCH);
            }
        }

        //bpsline -- observe that width/height in PRSInfo implies multiple subpatches...
        //render and cache each one seperately.
        //pPatch->setupStreams();

        ustep = 1.0/upatches;
        vstep = 1.0/vpatches;
        unsigned int i,j;
        for (i = 0; i < (upatches); i++)
        {
            for (j = 0; j < (vpatches); j++)
            {
                pPatch->setCalc(dwDirty);     //reset these flags everytime through loop
                                                //because they'll be cleared internally for each map
                                                //though technically I'm rebuilding all here
                pPatch->getCachedData(upatches*i+j);
                pPatch->setTextureOffsets(i*ustep, (i+1)*ustep, j*vstep, (j+1)*vstep);
                pPatch->copyDataToScratch(i, j);
                pPatch->setupStreams();

                //if we dirtied the control points -- free them up and reallocate some cached space for them.
                nvEvalPatchSetup(pPatch->getInfo());
                if (dwDirty & NV_PATCH_DIRTY_CONTROL_POINTS)
                {
                    nvAllocFDMatricesEtc(pPatch->getInfo());                
                    nvAllocInfoScratch(pPatch->getInfo());
                    retVal = nvEvalPatch(pPatch->getInfo());
                    pPatch->saveCachedData(upatches*i+j);
                }
                else
                {
                    retVal = nvEvalPatch(pPatch->getInfo());
                }
                nvEvalPatchCleanup(pPatch->getInfo());
            }
        }
    }

    nvEvalPatchCleanup(pPatch->getInfo());

    retVal = D3D_OK;

    //if handle is 0 we don't want this cached and should release the pNvObj, patch, and cached data
    if (dwHandle == 0)
    {
        pPatch->UnInit();
        MemFree(pPatch);
    }

nvPatch_Mem_Fault:

    // Restore the actual front face mode, since the front face register is whacked
    // directly during patch rendering.

    if (D3D__RenderState[D3DRS_CULLMODE] != D3DCULL_NONE)
    {
        D3DDevice_SetRenderState_FrontFace(D3D__RenderState[D3DRS_FRONTFACE]);
    }

    pDevice->EndBracket();

    return retVal;
}

//------------------------------------------------------------------------------
// D3DDevice_DrawTriPatch

extern "C"
HRESULT WINAPI D3DDevice_DrawTriPatch(
    UINT dwHandle,
    CONST float* pfSegs,
    CONST D3DTRIPATCH_INFO* pTSInfo) 
{ 
    COUNT_API(API_D3DDEVICE_DRAWTRIPATCH);

    float Segs[4];
    CPatch *pPatch = NULL;
    DWORD dwDirty = 0;
    DWORD retVal = D3D_OK;
    int i;
    CDevice *pDevice = g_pDevice;

    pDevice->StartBracket();

    //if no pfSegs info get it from the renderstate
    if (pfSegs == NULL)
    {
        Segs[0] = Segs[1] = Segs[2] = Segs[3] = *(float*)(&(D3D__RenderState[D3DRS_PATCHSEGMENTS]));
    }
    else
    {
        if (D3D__RenderState[D3DRS_PATCHEDGESTYLE] == 0x1)
        {
            for (i = 0; i < 4; i++)
            {
                Segs[i] = pfSegs[i];
            }
        }
        else
        {
            for (i = 0; i < 4; i++)
            {
                Segs[i] = int(pfSegs[i]);
            }
        }
    }
    Segs[3] = 0;    

    //check to see if we've cached this patch.
    //Note a handle of 0 implies NOT to cache the patch.
    if (dwHandle != 0)
    {
        pPatch = FindPatch(dwHandle);
    }

    if (pPatch != NULL)
    {
        //we have a pObj then we have a cached patch.
        if(Segs[0] != pPatch->getSeg(0) || Segs[1] != pPatch->getSeg(1) ||
           Segs[2] != pPatch->getSeg(2))
        {
            //tessellation has changed recalc parameterization
            pPatch->setCalc(NV_PATCH_DIRTY_TESSELLATION);
            pPatch->setTessellation(Segs, NV_PATCH_FLAG_TYPE_TRIANGULAR);
        }
        else
            //used cached FD params directly
            pPatch->setCalc(NV_PATCH_DIRTY_NONE);
    }

    //patch hasn't been cached -- build a new one
    if (pPatch == NULL || pTSInfo != NULL)
    {
        if (pPatch != NULL)
        {
            pPatch->UnInit();
            MemFree(pPatch);
        }

        pPatch = (CPatch*)MemAlloc(sizeof(CPatch));
        if (!pPatch->Init(dwHandle))
        {
            return E_OUTOFMEMORY;
        }

        pPatch->setTessellation(Segs, NV_PATCH_FLAG_TYPE_TRIANGULAR);
        pPatch->setOriginalTessellation(Segs, NV_PATCH_FLAG_TYPE_TRIANGULAR);
    }

    if (pTSInfo)
    {   //new RenderSurface info -- reinitialize the patch data

        //given this is a new cached patch  assume there is a valid pRSInfo.
        //initialize the patch structure
        pPatch->setDimension(1, 1);
        pPatch->setBasis((NV_PATCH_BASIS_TYPE)pTSInfo->Basis);    //int cast to deal with enum casts.
        pPatch->setOrder(pTSInfo->Order);
        pPatch->setStride(pTSInfo->Order);                       //IS THIS RIGHT FOR TRI PATCHES????
        pPatch->setPrimMode(NV_PATCH_FLAG_TYPE_TRIANGULAR);

        //alwasy set as fractional tessellation until they get their renderstate defined.
        if(D3D__RenderState[D3DRS_PATCHEDGESTYLE] == 0x1)
            pPatch->setTessMode(NV_PATCH_FLAG_TESS_FRACTIONAL);
        else pPatch->setTessMode(NV_PATCH_FLAG_TESS_INTEGER);

        //do this AFTER the normal/uv/stream setup cause they use these variable to figure out what to allocate
        pPatch->setCalc(NV_PATCH_DIRTY_TESSELLATION | NV_PATCH_DIRTY_CONTROL_POINTS);
        dwDirty = NV_PATCH_DIRTY_TESSELLATION | NV_PATCH_DIRTY_CONTROL_POINTS;

        //defined in # of vertices in a logical 2 dimensional Vertex buffer
        pPatch->setVertexOffset(pTSInfo->StartVertexOffset);

        //copy data to scratch.
        pPatch->setupStreams();
    }

    switch(pPatch->getBasis()){
    case NV_PATCH_BASIS_BEZIER:

        //if bezier don't cycle through multiple patches in the width/height fashion as bsplines
        if(dwDirty & NV_PATCH_DIRTY_CONTROL_POINTS){
            pPatch->copyDataToScratchTri(0,0);
            pPatch->freeCached();     //free up old cached data and reinit for a new patch to be built.
            if(!pPatch->allocCached(1)){
                retVal = D3DERR_DRIVERINTERNALERROR;
                goto nvPatch_Mem_Fault;
            }
            pPatch->getCachedData(0);
        }

        //the app? will make a call for each seperately.
        //if we dirtied the control points -- free them up and reallocate some cached space for them.
        pPatch->setTextureOffsets(0.0,1.0,0.0,1.0);
        pPatch->setupStreams();

        retVal = nvEvalPatchSetup(pPatch->getInfo());    
        nvAllocInfoScratch(pPatch->getInfo());
        if (dwDirty & NV_PATCH_DIRTY_CONTROL_POINTS)
        {
            nvAllocFDMatricesEtc(pPatch->getInfo());   
            retVal |= nvEvalPatch(pPatch->getInfo());
            pPatch->saveCachedData(0);
        }
        else
        {
            retVal |= nvEvalPatch(pPatch->getInfo());
        }
        break;
    case NV_PATCH_BASIS_BSPLINE:
    case NV_PATCH_BASIS_CATMULL_ROM:
        nvAssert(0);  //THIS SHOULD NEVER HAPPEN FOR TRI PATCHES!!!
    }

    nvEvalPatchCleanup(pPatch->getInfo());

    retVal = D3D_OK;

    //if handle is 0 we don't want this cached and should release the pNvObj, patch, and cached data
    if (dwHandle == 0)
    {
        pPatch->UnInit();
        MemFree(pPatch);
    }

nvPatch_Mem_Fault:

    // Restore the actual front face mode, since the front face register is whacked
    // directly during patch rendering.

    if (D3D__RenderState[D3DRS_CULLMODE] != D3DCULL_NONE)
    {
        D3DDevice_SetRenderState_FrontFace(D3D__RenderState[D3DRS_FRONTFACE]);
    }

    pDevice->EndBracket();

    return retVal;
}

//------------------------------------------------------------------------------
// D3DDevice_DeletePatch

extern "C"
void WINAPI D3DDevice_DeletePatch(
    UINT Handle) 
{ 
    COUNT_API(API_D3DDEVICE_DELETEPATCH);
    CPatch *pPatch = FindPatch(Handle);

    if (pPatch != NULL)
    {
        pPatch->UnInit();
        MemFree(pPatch);
    }
}

BOOL CPatch::Init(DWORD dwHandle)
{
    int i;

    if (!AddPatch(this, dwHandle))
    {
        return FALSE;
    }

    m_dwHandle = dwHandle;

    m_PatchData.maxSwatch = NV_PATCH_KELVIN_SWATCH_SIZE;

    m_subpatchcount = 0;
    m_dwStride = m_dwWidth = m_dwHeight = 0;
    m_dwOrder = 1;
    m_PatchData.srcNormal = -1;
    m_PatchData.dstNormal = -1;
    m_PatchData.maxSwatch = NV_PATCH_KELVIN_SWATCH_SIZE;
    m_PatchData.nAttr = NV_PATCH_NUMBER_OF_ATTRIBS;
    m_PatchData.maxAttr = NV_PATCH_NUMBER_OF_ATTRIBS;
    m_PatchData.maxOrder = NV_PATCH_MAX_ORDER;
//    m_PatchData.evAlloc9FDMatrices.pBase = NULL;
    m_PatchData.quadInfo = NULL;

    for (i = 0; i < 8; i++)
    {
        m_PatchData.srcUV[i] = -1;
        m_PatchData.dstUV[i] = -1;
    }

    m_PatchData.tess.tensor.nu0 = 0;
    m_PatchData.tess.tensor.nu1 = 0;
    m_PatchData.tess.tensor.nv0 = 0;
    m_PatchData.tess.tensor.nv1 = 0;

    m00 = m10 = m01 = m11 = NULL;
    guardQF = NULL;
    guardTF = NULL;

    cachedQuadInfo = NULL;

    for(i=0; i < MAX_EV_CACHE; i++){
        m_PatchData.pCache[i] = &MyCache[i];       
    }

    //setup default render flags
    m_PatchData.flags = 0x0;    //TESS=FLOAT, PRIM=QUAD, NO BUFFER ALLOCATED

    return TRUE;
}

void CPatch::UnInit(void)
{
    RemovePatch(m_dwHandle);

    freeCached();
}

void CPatch::setStream(UINT streamID, void *memptr, DWORD stride, DWORD pitch, NV_PATCH_VERTEX_FORMAT_TYPE mt, UINT order)
{
    m_PatchData.maps[streamID].maptype       = NV_PATCH_VERTEX_FORMAT_FLOAT_4;//shared code deals in 4 vectors!!!                                                                             
    //copy to scratch routine will expand.
    m_PatchData.maps[streamID].Originaltype  = mt;
    m_PatchData.maps[streamID].rawData       = (float*)memptr;
    m_PatchData.maps[streamID].stride        = stride;
    m_PatchData.maps[streamID].pitch         = pitch;                           //in # of vertices to next row.
    m_PatchData.maps[streamID].ufStride      = 4;                               //# of floats to next vertex in u.
    m_PatchData.maps[streamID].vfStride      = pitch*4;                         //# of floats to next vertex in v.
    m_PatchData.maps[streamID].uorder        = order;
    m_PatchData.maps[streamID].vorder        = order;
    if(order == 0) m_PatchData.evalEnables &= ~(0x1 << streamID);
    else m_PatchData.evalEnables |= (0x1 << streamID);
}

void CPatch::getCachedData(DWORD i){
    m_PatchData.quadInfo = cachedQuadInfo[i];
}

void CPatch::saveCachedData(DWORD i){
    //this assumes that the EVALLOC routines in the shared code allocate Quadinfo first yes?
    cachedQuadInfo[i] = (NV_PATCH_QUAD_INFO*)(m_PatchData.pCache[EV_CACHE_QUADINFO_ETC]->pBaseCache);    
    m_PatchData.pCache[EV_CACHE_QUADINFO_ETC]->pBaseCache = NULL;    
    m_PatchData.pCache[EV_CACHE_QUADINFO_ETC]->bytesAllocCache = 0;        
}

BOOL CPatch::allocCached(DWORD dwNumSubPatches)
{
    DWORD evalEnables = m_PatchData.evalEnables;
    DWORD i;

    //new char[NV_PATCH_NUMBER_OF_ATTRIBS*dwNumSubPatches*sizeof(FDMatrix*)]
    if(m_PatchData.flags & NV_PATCH_FLAG_AUTO_UV){
        for(i=0; i < 8; i++){   if(m_PatchData.dstUV[i] != 0xFFFFFFFF) evalEnables |= (1<< m_PatchData.dstUV[i]); }
    }
    if(m_PatchData.flags & NV_PATCH_FLAG_AUTO_NORMAL){ evalEnables |= (1<< m_PatchData.dstNormal); }
   
    //allocate set of ptrs to our cached data structures
    m_subpatchcount = dwNumSubPatches;
    bool failed = FALSE;

    //allocate space for quadInfo structs
    cachedQuadInfo = (NV_PATCH_QUAD_INFO**)MemAlloc(sizeof(NV_PATCH_QUAD_INFO*)*dwNumSubPatches);
    if(cachedQuadInfo == NULL){ return FALSE; }

    for(i=0; i < dwNumSubPatches; i++){ cachedQuadInfo[i] = NULL; }
    
    return TRUE;
}

void CPatch::freeCached()
{
    DWORD i;

    for(i=0; i < this->m_subpatchcount; i++){       
        if(cachedQuadInfo[i]) MemFree(cachedQuadInfo[i]);
        cachedQuadInfo[i] = NULL;
    }
    if(cachedQuadInfo){ MemFree(cachedQuadInfo); cachedQuadInfo = NULL; }    
    m_PatchData.quadInfo = NULL;
}

inline NV_PATCH_VERTEX_FORMAT_TYPE SizeAndTypeToType(DWORD SizeAndType)
{
    switch (SizeAndType)
    {
    case ST(1, F): return NV_PATCH_VERTEX_FORMAT_FLOAT_1;
    case ST(2, F): return NV_PATCH_VERTEX_FORMAT_FLOAT_2;
    case ST(3, F): return NV_PATCH_VERTEX_FORMAT_FLOAT_3;
    case ST(4, F): return NV_PATCH_VERTEX_FORMAT_FLOAT_4;
    default: return NV_PATCH_VERTEX_FORMAT_FLOAT_1;
    }
}

DWORD GetAutoNormalStream(VertexShader *pVShader)
{
    int i, v;

    for (v = 0; v < VERTEXSHADER_SLOTS; v++)
    {
        i = SlotToAttribute(v);
        if (i == -1)
        {
            continue;
        }

        if (pVShader->Slot[v].Flags == AUTONORMAL)
        {
            return i;
        }
    }
    return STREAM_NONE;
}

DWORD GetAutoTextureStream(DWORD dwCount)
{
    CDevice *pDevice = g_pDevice;
    VertexShader *pVShader = pDevice->m_pVertexShader;
    int i, v;

    for (v = 0; v < VERTEXSHADER_SLOTS; v++) {

        i = SlotToAttribute(v);
        if (i == -1)
        {
            continue;
        }

        if (pVShader->Slot[v].Flags == AUTOTEXCOORD) {
            if (dwCount == 0)
                return i;
            else
                dwCount--;
        }
    }
    return STREAM_NONE;
}

void CPatch::setupStreams()
{
    DWORD i, v, dwOffset, dwVBStride, dwOrder, dwStream;
    BYTE *dwVBBase;
    NV_PATCH_VERTEX_FORMAT_TYPE dwType;
    CDevice *pDevice = g_pDevice;
    VertexShader *pVShader;

    pVShader = pDevice->m_pVertexShader;

    m_PatchData.nAttr = 0x0;

    for (v = 0; v < VERTEXSHADER_SLOTS; v++)
    {
        i = SlotToAttribute(v);
        if (i == -1)
        {
            continue;
        }

        dwStream = pVShader->Slot[v].StreamIndex;

        if (pVShader->Slot[v].SizeAndType != SIZEANDTYPE_DISABLED &&
                pVShader->Slot[v].Flags == AUTONONE &&
                g_Stream[dwStream].pVertexBuffer != NULL)
        {
            dwType = SizeAndTypeToType(pVShader->Slot[v].SizeAndType);
            dwVBStride = 4*sizeof(float);   //stride size for shared code -- i.e. vector.
            dwVBBase = (BYTE*)&g_Scratch[i][0][0];
            dwOffset = 0;
            dwOrder = getOrder() + 1;
            m_PatchData.nAttr++;
        }
        else
        {
            dwVBBase = NULL;
            dwOffset = dwVBStride = dwOrder = 0;
            dwType = NV_PATCH_VERTEX_FORMAT_FLOAT_1;
        }
        setStream(i, dwVBBase + dwOffset, dwVBStride, dwOrder, dwType, dwOrder);
    }

    //get auto generated normals
    dwStream = GetAutoNormalStream(pVShader);
    if (dwStream != STREAM_NONE)
    {
        //destination should be the normal stream itself
        setAutoNormal(pVShader->Slot[dwStream].Source, dwStream);
    }

    //get auto generated texture coordinates
    for (i=0; i < 8; i++)
    {
        dwStream = GetAutoTextureStream(i);
        if (dwStream != STREAM_NONE)
        {
            //destination should be texture stream itself
            setAutoUV(pVShader->Slot[dwStream].Source, dwStream, i);
        }
    }

    pDevice->SetStateUP();
    setVertexStride(pDevice->m_InlineVertexDwords);
}

void CPatch::copyDataToScratch(DWORD offsetu, DWORD offsetv)
{
    DWORD v,i,j,k, pitch, vertexcount;
    float *data;
    DWORD evalEnables, dwOffset, dwStream, dwStride;
    DWORD order;
    CDevice *pDevice = g_pDevice;
    VertexShader *pVShader = pDevice->m_pVertexShader;

    order = m_dwOrder + 1;
    evalEnables = m_PatchData.evalEnables;

    for (v = 0; v < VERTEXSHADER_SLOTS; v++)
    {
        i = SlotToAttribute(v);
        if (i == -1)
        {
            continue;
        }

        if ((evalEnables & (1 << i)) &&
                pVShader->Slot[v].SizeAndType != SIZEANDTYPE_DISABLED)
        {
            dwStream = pVShader->Slot[v].StreamIndex;

            if (g_Stream[dwStream].pVertexBuffer != NULL) {

                data = (float*)GetDataFromResource(g_Stream[dwStream].pVertexBuffer);

                dwOffset = (offsetv * getWidth() + offsetu) + m_dwVertexOffset;
                dwStride =  g_Stream[dwStream].Stride >> 2; //convert to float
                data += dwOffset * dwStride;

                //BUG BUG: THIS IS WEIRD TO ADD AN OFFSET TO OFFSETS?? NEED TO LOOK INTO THIS
                data += (pVShader->Slot[v].Offset >> 0x2);  //vertex offset converted to offset in float
                pitch = getStride() * dwStride;   //getStride returns a stride value in vertices for mosaiced patches
                vertexcount = 0;

                switch (m_PatchData.maps[i].Originaltype)
                {
                case NV_PATCH_VERTEX_FORMAT_FLOAT_1:
                    for (j = 0; j < order; j++)
                    {
                        for(k = 0; k < order*dwStride; k+=dwStride)
                        {
                            g_Scratch[i][vertexcount][0] = data[k+0];
                            g_Scratch[i][vertexcount][1] = 0.0;
                            g_Scratch[i][vertexcount][2] = 0.0;
                            g_Scratch[i][vertexcount][3] = 1.0;
                            vertexcount++;
                        }
                        data += pitch;
                    }
                    break;
                case NV_PATCH_VERTEX_FORMAT_FLOAT_2:
                    for (j = 0; j < order; j++)
                    {
                        for (k = 0; k < order*dwStride; k+=dwStride)
                        {
                            g_Scratch[i][vertexcount][0] = data[k+0];
                            g_Scratch[i][vertexcount][1] = data[k+1];
                            g_Scratch[i][vertexcount][2] = 0.0;         
                            g_Scratch[i][vertexcount][3] = 1.0; 
                            vertexcount++;
                        }
                        data += pitch;
                    }
                    break;
                case NV_PATCH_VERTEX_FORMAT_FLOAT_3:
                    for (j = 0; j < order; j++)
                    {
                        for(k=0; k < order*dwStride; k+=dwStride)
                        {
                            g_Scratch[i][vertexcount][0] = data[k+0]; 
                            g_Scratch[i][vertexcount][1] = data[k+1];
                            g_Scratch[i][vertexcount][2] = data[k+2]; 
                            g_Scratch[i][vertexcount][3] = 1.0; 
                            vertexcount++;
                        }
                        data+=pitch;
                    }
                    break;
                case NV_PATCH_VERTEX_FORMAT_FLOAT_4:
                    for (j = 0; j < order; j++)
                    {
                        for (k = 0; k < order*dwStride; k+=dwStride)
                        {
                            g_Scratch[i][vertexcount][0] = data[k+0]; 
                            g_Scratch[i][vertexcount][1] = data[k+1];
                            g_Scratch[i][vertexcount][2] = data[k+2]; 
                            g_Scratch[i][vertexcount][3] = data[k+3]; 
                            vertexcount++;
                        }
                        data+=pitch;
                    }
                    break;
                }
            }
        }
    }

    //conversions not necessary for linear maps.
    if (m_dwOrder > 1)
    {
        switch(m_PatchData.basis){
        case NV_PATCH_BASIS_BEZIER:
            //convert data inline from BSpline to Scratch.
            break;
        case NV_PATCH_BASIS_BSPLINE:
            //convert basis inline from BSpline to Scratch.
            convertBsplineToBezier();
            break;
        case NV_PATCH_BASIS_CATMULL_ROM:
            ASSERT(m_dwOrder == 3);   //we only ever expect a cubic catmull rom here.
            //convert basis inline from Catmul Rom to Scratch.
            convertCatmullRomToBezier();
            break;
        }
    }
}

const int trilookup_1[] = {2, 1, 0};
const int trilookup_3[] = {12,9,8,6,5,4,3,2,1,0};
const int trilookup_5[] = {30,25,24,20,19,18,15,14,13,12,10,9,8,7,6,5,4,3,2,1,0};

void CPatch::copyDataToScratchTri(DWORD offsetu, DWORD offsetv)
{
    DWORD v,i,j,pitch;
    float *data;
    DWORD evalEnables, dwOffset, dwStream, dwStride, vertex;
    DWORD order;
    order = m_dwOrder+1;
    int *lookup;
    DWORD count = 0;
    evalEnables = m_PatchData.evalEnables;
    CDevice *pDevice = g_pDevice;
    VertexShader *pVShader = pDevice->m_pVertexShader;

    for (v = 0; v < VERTEXSHADER_SLOTS; v++){

        i = SlotToAttribute(v);
        if (i == -1)
        {
            continue;
        }

        if((evalEnables & (1<<i)) && pVShader->Slot[v].SizeAndType != SIZEANDTYPE_DISABLED){
            dwStream = pVShader->Slot[v].StreamIndex;

            if (g_Stream[dwStream].pVertexBuffer != NULL) {
                data = (float*)GetDataFromResource(g_Stream[dwStream].pVertexBuffer);
                dwOffset = (offsetv*getWidth() + offsetu) + m_dwVertexOffset;
                dwStride = g_Stream[dwStream].Stride >> 2;
                data += dwOffset * dwStride;
                data += (pVShader->Slot[v].Offset >> 0x2);
                pitch = getStride()*dwStride; //in vertices
                switch(order){
                    case 2: lookup = (int*)trilookup_1; count = 3;  break;    //linear
                    case 4: lookup = (int*)trilookup_3; count = 10; break;    //cubic
                    case 6: lookup = (int*)trilookup_5; count = 21; break;    //quintic
                    default: nvAssert(0);                   //shouldn't ever be here.
                }
                switch(m_PatchData.maps[i].Originaltype) {
                case NV_PATCH_VERTEX_FORMAT_FLOAT_1:
                    for(j=0; j < count; j++){
                        vertex = lookup[j];
                        g_Scratch[i][vertex][0] = data[0];   g_Scratch[i][vertex][1] = 0.0;
                        g_Scratch[i][vertex][2] = 0.0;       g_Scratch[i][vertex][3] = 1.0;
                        data+=dwStride;
                    }
                    break;
                case NV_PATCH_VERTEX_FORMAT_FLOAT_2:
                    for(j=0; j < count; j++){
                        vertex = lookup[j];
                        g_Scratch[i][vertex][0] = data[0];   g_Scratch[i][vertex][1] = data[1];
                        g_Scratch[i][vertex][2] = 0.0;       g_Scratch[i][vertex][3] = 1.0;
                        data+=dwStride;
                    }
                    break;
                case NV_PATCH_VERTEX_FORMAT_FLOAT_3:
                    for(j=0; j < count; j++){
                        vertex = lookup[j];
                        g_Scratch[i][vertex][0] = data[0];   g_Scratch[i][vertex][1] = data[1];
                        g_Scratch[i][vertex][2] = data[2];   g_Scratch[i][vertex][3] = 1.0;
                        data+=dwStride;
                    }
                    break;
                case NV_PATCH_VERTEX_FORMAT_FLOAT_4:
                    for(j=0; j < count; j++){
                        vertex = lookup[j];
                        g_Scratch[i][vertex][0] = data[0];   g_Scratch[i][vertex][1] = data[1];
                        g_Scratch[i][vertex][2] = data[2];   g_Scratch[i][vertex][3] = data[3];
                        data+=dwStride;
                    }
                    break;
                case NV_PATCH_VERTEX_FORMAT_UBYTE:
                case NV_PATCH_VERTEX_FORMAT_D3DCOLOR:
                    for(j=0; j < count; j++){
                        vertex = lookup[j];
                        g_Scratch[i][vertex][0] = ((VIEW_AS_DWORD(data[0])&0x00ff0000)>>16)/255.0;
                        g_Scratch[i][vertex][1] = ((VIEW_AS_DWORD(data[0])&0x0000ff00)>>8 )/255.0;
                        g_Scratch[i][vertex][2] = ((VIEW_AS_DWORD(data[0])&0x000000ff)>>0 )/255.0;
                        g_Scratch[i][vertex][3] = ((VIEW_AS_DWORD(data[0])&0xff000000)>>24)/255.0;                                             
                        data+=dwStride;
                    }
                    break;
                default:
                    nvAssert(0);
                }
            }
        }
    }
}

__inline void catrom2bez3(float *in, int stride) {

    float in1_1x,in1_1y,in1_1z,in1_1w;
    float in0_1x,in0_1y,in0_1z,in0_1w;
    float in2_1x,in2_1y,in2_1z,in2_1w;

    in1_1x = (1.0/6.0)*in[1*stride+0];
    in1_1y = (1.0/6.0)*in[1*stride+1];
    in1_1z = (1.0/6.0)*in[1*stride+2];
    in1_1w = (1.0/6.0)*in[1*stride+3];
    in0_1x = (1.0/6.0)*in[0*stride+0];
    in0_1y = (1.0/6.0)*in[0*stride+1];
    in0_1z = (1.0/6.0)*in[0*stride+2];
    in0_1w = (1.0/6.0)*in[0*stride+3];
    in2_1x = in[2*stride+0];
    in2_1y = in[2*stride+1];
    in2_1z = in[2*stride+2];
    in2_1w = in[2*stride+3];

    in[0*stride+0] = in[1*stride+0];
    in[0*stride+1] = in[1*stride+1];
    in[0*stride+2] = in[1*stride+2];
    in[0*stride+3] = in[1*stride+3];
    in[1*stride+0] = -in0_1x + in[1*stride+0] +  (1.0/6.0)*in[2*stride+0];
    in[1*stride+1] = -in0_1y + in[1*stride+1] +  (1.0/6.0)*in[2*stride+1];
    in[1*stride+2] = -in0_1z + in[1*stride+2] +  (1.0/6.0)*in[2*stride+2];
    in[1*stride+3] = -in0_1w + in[1*stride+3] +  (1.0/6.0)*in[2*stride+3];
    in[2*stride+0] =  in1_1x + in[2*stride+0] + -(1.0/6.0)*in[3*stride+0];
    in[2*stride+1] =  in1_1y + in[2*stride+1] + -(1.0/6.0)*in[3*stride+1];
    in[2*stride+2] =  in1_1z + in[2*stride+2] + -(1.0/6.0)*in[3*stride+2];
    in[2*stride+3] =  in1_1w + in[2*stride+3] + -(1.0/6.0)*in[3*stride+3];
    in[3*stride+0] = in2_1x;
    in[3*stride+1] = in2_1y;
    in[3*stride+2] = in2_1z;
    in[3*stride+3] = in2_1w;

}

__inline void bsp2bez5(float *in, int stride) {
    float in2_66x,in2_66y,in2_66z,in2_66w;
    float in3_66x,in3_66y,in3_66z,in3_66w;
    float in1_1x,in1_1y,in1_1z,in1_1w;
    float in2_48x,in2_48y,in2_48z,in2_48w;
    float in2_36x,in2_36y,in2_36z,in2_36w;

    in2_66x = (66.0f/120.0f)*in[2*stride+0]; in2_66y = (66.0f/120.0f)*in[2*stride+1];
    in2_66z = (66.0f/120.0f)*in[2*stride+2]; in2_66w = (66.0f/120.0f)*in[2*stride+3];
    in3_66x = (66.0f/120.0f)*in[3*stride+0]; in3_66y = (66.0f/120.0f)*in[3*stride+1];
    in3_66z = (66.0f/120.0f)*in[3*stride+2]; in3_66w = (66.0f/120.0f)*in[3*stride+3];
    in1_1x =  (1.0f/120.0f)*in[1*stride+0];  in1_1y  = (1.0f/120.0f)*in[1*stride+1];
    in1_1z =  (1.0f/120.0f)*in[1*stride+2];  in1_1w  = (1.0f/120.0f)*in[1*stride+3];
    in2_48x = (48.0f/120.0f)*in[2*stride+0]; in2_48y =  (48.0f/120.0f)*in[2*stride+1];
    in2_48z = (48.0f/120.0f)*in[2*stride+2]; in2_48w = (48.0f/120.0f)*in[2*stride+3];
    in2_36x = (36.0f/120.0f)*in[2*stride+0]; in2_36y = (36.0f/120.0f)*in[2*stride+1];
    in2_36z = (36.0f/120.0f)*in[2*stride+2]; in2_36w = (36.0f/120.0f)*in[2*stride+3];

    //in[0] = (1.0f/120.0f)*in[0] + (26.0f/120.0f)*in[1]          + in2_66  + (26.0f/120.0f)*in[3]         + ( 1.0f/120.0f)*in[4];
    in[0] = (1.0f/120.0f)*in[0]   + (26.0f/120.0f)*in[1*stride+0] + in2_66x + (26.0f/120.0f)*in[3*stride+0] + ( 1.0f/120.0f)*in[4*stride+0];
    in[1] = (1.0f/120.0f)*in[1]   + (26.0f/120.0f)*in[1*stride+1] + in2_66y + (26.0f/120.0f)*in[3*stride+1] + ( 1.0f/120.0f)*in[4*stride+1];
    in[2] = (1.0f/120.0f)*in[2]   + (26.0f/120.0f)*in[1*stride+2] + in2_66z + (26.0f/120.0f)*in[3*stride+2] + ( 1.0f/120.0f)*in[4*stride+2];
    in[3] = (1.0f/120.0f)*in[3]   + (26.0f/120.0f)*in[1*stride+3] + in2_66w + (26.0f/120.0f)*in[3*stride+3] + ( 1.0f/120.0f)*in[4*stride+3];

    //move this up here to avoid conflicts on in place operations -- this ONLY works because pt5 is only used in the last pt calc and no where else
    //in[5] =         ( 1.0f/120.0f)*in[1]          + (26.0f/120.0f)*in[2]          + in3_66  + (26.0f/120.0f)*in[4]          + (1.0f/120.0f)*in[5];
    in[5*stride+0] =  in1_1x + (26.0f/120.0f)*in[2*stride+0] + in3_66x + (26.0f/120.0f)*in[4*stride+0] + (1.0f/120.0f)*in[5*stride+0];
    in[5*stride+1] =  in1_1y + (26.0f/120.0f)*in[2*stride+1] + in3_66y + (26.0f/120.0f)*in[4*stride+1] + (1.0f/120.0f)*in[5*stride+1];
    in[5*stride+2] =  in1_1z + (26.0f/120.0f)*in[2*stride+2] + in3_66z + (26.0f/120.0f)*in[4*stride+2] + (1.0f/120.0f)*in[5*stride+2];
    in[5*stride+3] =  in1_1w + (26.0f/120.0f)*in[2*stride+3] + in3_66w + (26.0f/120.0f)*in[4*stride+3] + (1.0f/120.0f)*in[5*stride+3];

    //in[1] =         (16.0f/120.0f)*in[1]          + in2_66  + (36.0f/120.0f)*in[3]          + ( 2.0f/120.0f)*in[4];
    in[1*stride+0] =  (16.0f/120.0f)*in[1*stride+0] + in2_66x + (36.0f/120.0f)*in[3*stride+0] + ( 2.0f/120.0f)*in[4*stride+0];
    in[1*stride+1] =  (16.0f/120.0f)*in[1*stride+1] + in2_66y + (36.0f/120.0f)*in[3*stride+1] + ( 2.0f/120.0f)*in[4*stride+1];
    in[1*stride+2] =  (16.0f/120.0f)*in[1*stride+2] + in2_66z + (36.0f/120.0f)*in[3*stride+2] + ( 2.0f/120.0f)*in[4*stride+2];
    in[1*stride+3] =  (16.0f/120.0f)*in[1*stride+3] + in2_66w + (36.0f/120.0f)*in[3*stride+3] + ( 2.0f/120.0f)*in[4*stride+3];

    //in[2] =         ( 8.0f/120.0f)*in[1]          + (60.0f/120.0f)*in[2]          + (48.0f/120.0f)*in[3]          + ( 4.0f/120.0f)*in[4];
    in[2*stride+0] =  in1_1x*8 + (60.0f/120.0f)*in[2*stride+0] + (48.0f/120.0f)*in[3*stride+0] + ( 4.0f/120.0f)*in[4*stride+0];
    in[2*stride+1] =  in1_1y*8 + (60.0f/120.0f)*in[2*stride+1] + (48.0f/120.0f)*in[3*stride+1] + ( 4.0f/120.0f)*in[4*stride+1];
    in[2*stride+2] =  in1_1z*8 + (60.0f/120.0f)*in[2*stride+2] + (48.0f/120.0f)*in[3*stride+2] + ( 4.0f/120.0f)*in[4*stride+2];
    in[2*stride+3] =  in1_1w*8 + (60.0f/120.0f)*in[2*stride+3] + (48.0f/120.0f)*in[3*stride+3] + ( 4.0f/120.0f)*in[4*stride+3];

    //in[3] =         ( 4.0f/120.0f)*in[1]          + (48.0f/120.0f)*in[2]          + (60.0f/120.0f)*in[3]          + ( 8.0f/120.0f)*in[4];
    in[3*stride+0] =  in1_1x*4 + in2_48x + (60.0f/120.0f)*in[3*stride+0] + ( 8.0f/120.0f)*in[4*stride+0];
    in[3*stride+1] =  in1_1y*4 + in2_48y + (60.0f/120.0f)*in[3*stride+1] + ( 8.0f/120.0f)*in[4*stride+1];
    in[3*stride+2] =  in1_1z*4 + in2_48z + (60.0f/120.0f)*in[3*stride+2] + ( 8.0f/120.0f)*in[4*stride+2];
    in[3*stride+3] =  in1_1w*4 + in2_48w + (60.0f/120.0f)*in[3*stride+3] + ( 8.0f/120.0f)*in[4*stride+3];

    //in[4] =         ( 2.0f/120.0f)*in[1]          + (36.0f/120.0f)*in[2]          + in3_66  + (16.0f/120.0f)*in[4];
    in[4*stride+0] =  in1_1x*2 + in2_36x + in3_66x + (16.0f/120.0f)*in[4*stride+0];
    in[4*stride+1] =  in1_1y*2 + in2_36y + in3_66y + (16.0f/120.0f)*in[4*stride+1];
    in[4*stride+2] =  in1_1z*2 + in2_36z + in3_66z + (16.0f/120.0f)*in[4*stride+2];
    in[4*stride+3] =  in1_1w*2 + in2_36w + in3_66w + (16.0f/120.0f)*in[4*stride+3];
}

//stride = 4 or 16;
// 4 ---> traverses across a row
//16 ---> traverses down a column
__inline void bsp2bez3(float *in, int stride) {
    float in1_4x,in1_4y,in1_4z,in1_4w;
    float in2_4x,in2_4y,in2_4z,in2_4w;
    float in1_2x,in1_2y,in1_2z,in1_2w;
    float in1_1x,in1_1y,in1_1z,in1_1w;

    in1_4x = (4.0f/6.0f) * in[stride+0];
    in1_4y = (4.0f/6.0f) * in[stride+1];
    in1_4z = (4.0f/6.0f) * in[stride+2];
    in1_4w = (4.0f/6.0f) * in[stride+3];

    in2_4x = (4.0f/6.0f) * in[2*stride+0];
    in2_4y = (4.0f/6.0f) * in[2*stride+1];
    in2_4z = (4.0f/6.0f) * in[2*stride+2];
    in2_4w = (4.0f/6.0f) * in[2*stride+3];

    in1_2x = (2.0f/6.0f) * in[stride+0];
    in1_2y = (2.0f/6.0f) * in[stride+1];
    in1_2z = (2.0f/6.0f) * in[stride+2];
    in1_2w = (2.0f/6.0f) * in[stride+3];

    in1_1x = (1.0f/6.0f) * in[stride+0];
    in1_1y = (1.0f/6.0f) * in[stride+1];
    in1_1z = (1.0f/6.0f) * in[stride+2];
    in1_1w = (1.0f/6.0f) * in[stride+3];

    in[0] = (1.0f/6.0f)*in[0] + in1_4x + (1.0f/6.0f) * in[2*stride+0];           //x
    in[1] = (1.0f/6.0f)*in[1] + in1_4y + (1.0f/6.0f) * in[2*stride+1];           //y
    in[2] = (1.0f/6.0f)*in[2] + in1_4z + (1.0f/6.0f) * in[2*stride+2];           //z
    in[3] = (1.0f/6.0f)*in[3] + in1_4w + (1.0f/6.0f) * in[2*stride+3];           //w

    in[stride+0] =              in1_4x + (2.0f/6.0f) * in[2*stride+0];       //x
    in[stride+1] =              in1_4y + (2.0f/6.0f) * in[2*stride+1];       //y
    in[stride+2] =              in1_4z + (2.0f/6.0f) * in[2*stride+2];       //z
    in[stride+3] =              in1_4w + (2.0f/6.0f) * in[2*stride+3];       //w

    in[2*stride+0] =            in1_2x + in2_4x;                         //x
    in[2*stride+1] =            in1_2y + in2_4y;                         //y
    in[2*stride+2] =            in1_2z + in2_4z;                         //z
    in[2*stride+3] =            in1_2w + in2_4w;                         //w

    in[3*stride+0] =            in1_1x + in2_4x + (1.0f/6.0f)*in[3*stride+0];    //x
    in[3*stride+1] =            in1_1y + in2_4y + (1.0f/6.0f)*in[3*stride+1];    //y
    in[3*stride+2] =            in1_1z + in2_4z + (1.0f/6.0f)*in[3*stride+2];    //z
    in[3*stride+3] =            in1_1w + in2_4w + (1.0f/6.0f)*in[3*stride+3];    //w
}

void CPatch::convertCatmullRomToBezier(){
    int i;
    for(i=0; i < NV_PATCH_NUMBER_OF_ATTRIBS; i++){
        if(m_PatchData.evalEnables & (1<<i)){
            switch(m_dwOrder){
            case 1:
            case 2:
            case 4:
            case 5:
                nvAssert(0);    //we shouldn't ever get these.
                break;
            case 3:
                //convert cubic bezier patch.
                catrom2bez3(&g_Scratch[i][0][0],  4);  //row1
                catrom2bez3(&g_Scratch[i][4][0],  4);  //row2
                catrom2bez3(&g_Scratch[i][8][0],  4);  //row3
                catrom2bez3(&g_Scratch[i][12][0], 4);  //row4
                catrom2bez3(&g_Scratch[i][0][0], 16);  //column 1
                catrom2bez3(&g_Scratch[i][1][0], 16);  //column 2
                catrom2bez3(&g_Scratch[i][2][0], 16);  //column 3
                catrom2bez3(&g_Scratch[i][3][0], 16);  //column 4
                break;
            }
        }
    }
}

void CPatch::convertBsplineToBezier(){

    int i;
    for(i=0; i < NV_PATCH_NUMBER_OF_ATTRIBS; i++){
        if(m_PatchData.evalEnables & (1<<i)){
            switch(m_dwOrder){
            case 3:
                //convert cubic bezier patch.
                bsp2bez3(&g_Scratch[i][0][0],  4);  //row1
                bsp2bez3(&g_Scratch[i][4][0],  4);  //row2
                bsp2bez3(&g_Scratch[i][8][0],  4);  //row3
                bsp2bez3(&g_Scratch[i][12][0], 4);  //row4
                bsp2bez3(&g_Scratch[i][0][0], 16);  //column 1
                bsp2bez3(&g_Scratch[i][1][0], 16);  //column 2
                bsp2bez3(&g_Scratch[i][2][0], 16);  //column 3
                bsp2bez3(&g_Scratch[i][3][0], 16);  //column 4
                break;
            case 5:
                //convert quintic bezier patch.
                bsp2bez5(&g_Scratch[i][0][0],  4);  //row1
                bsp2bez5(&g_Scratch[i][6][0],  4);  //row2
                bsp2bez5(&g_Scratch[i][12][0], 4);  //row3
                bsp2bez5(&g_Scratch[i][18][0], 4);  //row4
                bsp2bez5(&g_Scratch[i][24][0], 4);  //row1
                bsp2bez5(&g_Scratch[i][30][0], 4);  //row2
                bsp2bez5(&g_Scratch[i][0][0],  24);  //column1
                bsp2bez5(&g_Scratch[i][1][0],  24);  //column2
                bsp2bez5(&g_Scratch[i][2][0],  24);  //column3
                bsp2bez5(&g_Scratch[i][3][0],  24);  //column4
                bsp2bez5(&g_Scratch[i][4][0],  24);  //column5
                bsp2bez5(&g_Scratch[i][5][0],  24);  //column6
                break;
            default:
                nvAssert(0);
                break;
            }
        }
    }
}

} // end of namespace
