// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: nvPatch.cpp
//
// **************************************************************************
//
//  History:
//      Daniel Rohrer?             Jun00             DX8 development
//
// **************************************************************************

#include "nvprecomp.h"
#define PB_DUMP_SIZE 256
NV_PATCH_DRIVER_CALLBACKS CPatch::m_callbacks = {CPatch::alloc, CPatch::free, CPatch::memcpy, NULL};
#define DCR_CACHEDPB_OPT

//scratch space
static float scratch[16][36][4];
NV_PATCH_ALLOC_CACHE MyCache[MAX_EV_CACHE];

DWORD nvDrawTriSurface (PNVD3DCONTEXT pContext, DWORD dwHandle,
                        float *pfSegs, D3DTRIPATCH_INFO *pTSInfo)
{
    float Segs[4];
    CNvObject *pObj = NULL;
    CPatch *pNvPatch = NULL;
    DWORD dwDirty = 0;
    DWORD retVal = D3D_OK;
    int i;   

    if(!pContext->pCurrentVShader) return D3DERR_DRIVERINTERNALERROR;  //must have a vertex shader bound    

    //if no pfSegs info get it from the renderstate
    if (!pfSegs) {
        Segs[0] = Segs[1] = Segs[2] = *(float*)(&(pContext->dwRenderState[D3DRS_PATCHSEGMENTS]));
    }
    else {
        if (pContext->dwRenderState[D3DRS_PATCHEDGESTYLE] == 0x1)
            for (i=0; i < 4; i++) Segs[i] = pfSegs[i];
        else
            for (i=0; i < 4; i++) Segs[i] = int(pfSegs[i]);
    }
    Segs[3] = 0;    

    // check to see if we've cached this patch.
    // Note a handle of 0 implies NOT to cache the patch.
    if (dwHandle) pObj = nvGetObjectFromHandle(pContext->dwDDLclID, dwHandle, &global.pNvPatchLists);
    if (pObj) {
        //we have a pObj then we have a cached patch.
        pNvPatch = pObj->getPatch();
        assert(pNvPatch);
        if(Segs[0] != pNvPatch->getSeg(0) || Segs[1] != pNvPatch->getSeg(1) ||
           Segs[2] != pNvPatch->getSeg(2))
        {
            //tessellation has changed recalc parameterization
            pNvPatch->setCalc(NV_PATCH_DIRTY_TESSELLATION);
            pNvPatch->setTessellation(Segs, NV_PATCH_FLAG_TYPE_TRIANGULAR);
        }
        else
            //used cached FD params directly
            pNvPatch->setCalc(NV_PATCH_DIRTY_NONE);
    }

    //patch hasn't been cached -- build a new one
    if(pObj == NULL || pTSInfo){
        if(pObj){
            //pObj was previously cached and pRSInfo is new so we need to rebuild anyway
            //remove the cached one and rebuild from scratch
            PNV_OBJECT_LIST pNvPL = nvFindObjectList(pContext->dwDDLclID, &global.pNvPatchLists);
            nvClearObjectListEntry(pObj, &pNvPL);
            pObj->release();
        }
        pObj  =  new CNvObject(0);
        pNvPatch = new CPatch(pObj, dwHandle);      //cache the handle, and original segs.
        pNvPatch->setContext(pContext);
        DPF_LEVEL(NVDBG_LEVEL_HOS_INFO,"Creating TRI Patch #%d",dwHandle);

        nvAssert(pObj);
        nvAssert(pNvPatch);
        if(!pObj || !pNvPatch){ retVal = D3DERR_DRIVERINTERNALERROR; goto nvPatch_Mem_Fault; }
        pObj->setObject(CNvObject::NVOBJ_CACHEDPATCH, pNvPatch);    //bind wrapper
        if(dwHandle){
            //cache the patch if the handle isn't 0.
            PNV_OBJECT_LIST pNvPL = nvFindObjectList(pContext->dwDDLclID, &global.pNvPatchLists);
            nvAddObjectToList(pNvPL ,pObj, dwHandle);
        }
        pNvPatch->setTessellation(Segs, NV_PATCH_FLAG_TYPE_TRIANGULAR);
        pNvPatch->setOriginalTessellation(Segs, NV_PATCH_FLAG_TYPE_TRIANGULAR);
    }

    if(pTSInfo){    //new RenderSurface info -- reinitialize the patch data
        //select backend stream type
        if(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0097_KELVIN) pNvPatch->setOutputMode(NV_PATCH_BACKEND_KELVIN);
        else pNvPatch->setOutputMode(NV_PATCH_BACKEND_CELSIUS);

        //given this is a new cached patch  assume there is a valid pRSInfo.
        //initialize the patch structure
        pNvPatch->setDimension(1, 1);
        pNvPatch->setBasis((NV_PATCH_BASIS_TYPE)pTSInfo->Basis);    //int cast to deal with enum casts.
        pNvPatch->setOrder(pTSInfo->Order);
        pNvPatch->setStride(pTSInfo->Order);                       //IS THIS RIGHT FOR TRI PATCHES????
        pNvPatch->setPrimMode(NV_PATCH_FLAG_TYPE_TRIANGULAR);

        //alwasy set as fractional tessellation until they get their renderstate defined.
        if(pContext->dwRenderState[D3DRS_PATCHEDGESTYLE] == 0x1)
            pNvPatch->setTessMode(NV_PATCH_FLAG_TESS_FRACTIONAL);
        else pNvPatch->setTessMode(NV_PATCH_FLAG_TESS_INTEGER);

        //do this AFTER the normal/uv/stream setup cause they use these variable to figure out what to allocate
        pNvPatch->setCalc(NV_PATCH_DIRTY_TESSELLATION | NV_PATCH_DIRTY_CONTROL_POINTS);
        dwDirty = NV_PATCH_DIRTY_TESSELLATION | NV_PATCH_DIRTY_CONTROL_POINTS;

        //defined in # of vertices in a logical 2 dimensional Vertex buffer
        pNvPatch->setVertexOffset(pTSInfo->StartVertexOffset);

        //copy data to scratch.
        pNvPatch->setupStreams(pContext);
    }

    //setup celsius strides for inlining vertices
#ifdef HOSURF_ENABLE
    //set ptr to cache parameters
    nvHWLockTextures (pContext);    

    switch(pNvPatch->getBasis()){
    case NV_PATCH_BASIS_BEZIER:

        //if bezier don't cycle through multiple patches in the width/height fashion as bsplines
        if(dwDirty & NV_PATCH_DIRTY_CONTROL_POINTS){
            pNvPatch->copyDataToScratchTri(pContext,0,0);
            pNvPatch->freeCached();     //free up old cached data and reinit for a new patch to be built.
            if(!pNvPatch->allocCached(1)){
                retVal = D3DERR_DRIVERINTERNALERROR;
                goto nvPatch_Mem_Fault;
            }
            pNvPatch->getCachedData(0);
        }
        pNvPatch->setTextureOffsets(0.0,1.0,0.0,1.0); 
        pNvPatch->setupStreams(pContext);
       
    
#ifdef DCR_CACHEDPB_OPT
        if(dwDirty == NV_PATCH_DIRTY_NONE){
            if(pNvPatch->getCachedPBSize() == 0){
                DWORD dwWC = pDriverData->nvPusher.getWrapCount();                
                DWORD dwCPut = pDriverData->nvPusher.getPut();
                retVal = nvEvalPatchSetup(pNvPatch->getInfo());    
                nvAllocInfoScratch(pNvPatch->getInfo());
                retVal = nvEvalPatch(pNvPatch->getInfo());
                if(dwWC == pDriverData->nvPusher.getWrapCount()){
                    //we didn't actually wrap so copy off the pb to be used later.
                    pNvPatch->allocCachedPB((pDriverData->nvPusher.getPut() - dwCPut));
                    nvMemCopy((unsigned char *)pNvPatch->getCachedPB(),(unsigned char *)dwCPut, pNvPatch->getCachedPBSize());
                }
            }
            else{
                DWORD dwSize = pNvPatch->getCachedPBSize();
                DWORD *dwPB = (DWORD *)(&(pNvPatch->getCachedPB()[0]));
                pDriverData->nvPusher.makeSpace(dwSize);
                nvMemCopy(pDriverData->nvPusher.getPut(), (DWORD)dwPB, dwSize);               
                pDriverData->nvPusher.inc(dwSize>>2);    //push the put forward                    

                /* DWORD dwPut;
                DWORD dwSize = (pNvPatch->getCachedPBSize() % (PB_DUMP_SIZE*sizeof(DWORD)));
                DWORD dwCount = (pNvPatch->getCachedPBSize() - dwSize)/(PB_DUMP_SIZE*sizeof(DWORD));
                DWORD *dwPB;    
                DWORD i;

                dwPB  = (DWORD *)(&(pNvPatch->getCachedPB()[0]));
                for(i=0; i < dwCount; i++){
                    pDriverData->nvPusher.makeSpace(PB_DUMP_SIZE);
                    dwPut = pDriverData->nvPusher.getPut();             //don't calc cause the PB could wrap.
                    nvMemCopy(dwPut, (DWORD)dwPB, PB_DUMP_SIZE*sizeof(DWORD));
                    pDriverData->nvPusher.inc(PB_DUMP_SIZE);            //push the put forward                    
                    dwPB+=PB_DUMP_SIZE;
                }
                pDriverData->nvPusher.makeSpace(dwSize);
                nvMemCopy(pDriverData->nvPusher.getPut(), (DWORD)dwPB, dwSize);               
                pDriverData->nvPusher.inc(dwSize>>2);    //push the put forward */
            }
            pDriverData->nvPusher.start(TRUE);
        }
        else {
            pNvPatch->freeCachedPB();
#endif
        retVal = nvEvalPatchSetup(pNvPatch->getInfo());    
        nvAllocInfoScratch(pNvPatch->getInfo());
        if(dwDirty & NV_PATCH_DIRTY_CONTROL_POINTS){
            nvAllocFDMatricesEtc(pNvPatch->getInfo());   
            retVal |= nvEvalPatch(pNvPatch->getInfo());
            pNvPatch->saveCachedData(0);
        }
        else{
            retVal |= nvEvalPatch(pNvPatch->getInfo());
        }
        pDriverData->nvPusher.start(FALSE);
#ifdef DCR_CACHEDPB_OPT
        }
#endif
        break;
    case NV_PATCH_BASIS_BSPLINE:
    case NV_PATCH_BASIS_CATMULL_ROM:
        nvAssert(0);  //THIS SHOULD NEVER HAPPEN FOR TRI PATCHES!!!
    }

    nvEvalPatchCleanup(pNvPatch->getInfo());
    nvHWUnlockTextures (pContext);
#else
    retVal = D3D_OK;
#endif

    //if handle is 0 we don't want this cached and should release the pNvObj, patch, and cached data
    if(dwHandle == 0){
        pObj->release();
    }

nvPatch_Mem_Fault:
    return retVal;
}

static DWORD SizeCounter = 0;
static DWORD PatchCounter = 0;
DWORD nvDrawRectSurface (PNVD3DCONTEXT pContext, DWORD dwHandle,
                         float *pfSegs, D3DRECTPATCH_INFO *pRSInfo)
{
    float      Segs[4];
    CNvObject *pObj = NULL;
    CPatch    *pNvPatch = NULL;
    DWORD      num_subpatches = 0;
    DWORD      dwDirty = 0;
    DWORD      retVal = D3D_OK;
    int i;

    if(!pContext->pCurrentVShader) return D3DERR_DRIVERINTERNALERROR;  //must have a vertex shader bound

    //if no pfSegs info get it from the renderstate
    if (!pfSegs) {
        Segs[0] = Segs[1] = Segs[2] = Segs[3] = *(float*)(&(pContext->dwRenderState[D3DRS_PATCHSEGMENTS]));
    }
    else {
        if(pContext->dwRenderState[D3DRS_PATCHEDGESTYLE] == 0x1)
            for(i=0; i < 4; i++) Segs[i] = pfSegs[i];
        else
            for(i=0; i < 4; i++) Segs[i] = int(pfSegs[i]);
    }

    //check to see if we've cached this patch.
    //Note a handle of 0 implies NOT to cache the patch.
    if (dwHandle) pObj = nvGetObjectFromHandle(pContext->dwDDLclID, dwHandle, &global.pNvPatchLists);
    if(pObj){
        //we have a pObj then we have a cached patch.
        pNvPatch = pObj->getPatch();
        assert(pNvPatch);
        if(Segs[0] != pNvPatch->getSeg(0) || Segs[1] != pNvPatch->getSeg(1) ||
           Segs[2] != pNvPatch->getSeg(2) || Segs[3] != pNvPatch->getSeg(3))
        {
            //tessellation has changed recalc parameterization
            pNvPatch->setCalc(NV_PATCH_DIRTY_TESSELLATION);
            pNvPatch->setTessellation(Segs, NV_PATCH_FLAG_TYPE_PATCH);
            dwDirty = NV_PATCH_DIRTY_TESSELLATION;
        }
        else{
            //used cached FD params directly
            pNvPatch->setCalc(NV_PATCH_DIRTY_NONE);
        }
    }

    //patch hasn't been cached -- build a new one
    if(pObj == NULL || pRSInfo){
        if(pObj){
            //pObj was previously cached and pRSInfo is new so we need to rebuild anyway
            //remove the cached one and rebuild   from scratch
            PNV_OBJECT_LIST pNvPL = nvFindObjectList(pContext->dwDDLclID, &global.pNvPatchLists);
            nvClearObjectListEntry(pObj, &pNvPL);
            pObj->release();
        }
        pObj  =  new CNvObject(0);
        pNvPatch = new CPatch(pObj, dwHandle);      //cache the handle, and original segs.
        pNvPatch->setContext(pContext);
        DPF_LEVEL(NVDBG_LEVEL_HOS_INFO,"Creating QUAD Patch #%d PObj = %08x pNvPatch = %08x DDLcl = %08x",dwHandle,pObj,pNvPatch,pContext->dwDDLclID);

        nvAssert(pObj);
        nvAssert(pNvPatch);
        if(!pObj || !pNvPatch){ retVal = D3DERR_DRIVERINTERNALERROR; goto nvPatch_Mem_Fault; }
        pObj->setObject(CNvObject::NVOBJ_CACHEDPATCH, pNvPatch);    //bind wrapper

        if(dwHandle){
            //cache the patch if the handle isn't 0.
            PNV_OBJECT_LIST pNvPL = nvFindObjectList(pContext->dwDDLclID, &global.pNvPatchLists);
            nvAddObjectToList(pNvPL ,pObj, dwHandle);
        }
        pNvPatch->setTessellation(Segs, NV_PATCH_FLAG_TYPE_PATCH);
        pNvPatch->setOriginalTessellation(Segs, NV_PATCH_FLAG_TYPE_PATCH);        
    }
    
    if(pRSInfo){    //new RenderSurface info -- reinitialize the patch data
        //select backend stream type
        if(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0097_KELVIN) pNvPatch->setOutputMode(NV_PATCH_BACKEND_KELVIN);
        else pNvPatch->setOutputMode(NV_PATCH_BACKEND_CELSIUS);

        //given this is a new cached patch  assume there is a valid pRSInfo.
        //initialize the patch structure
        pNvPatch->setDimension(pRSInfo->Width, pRSInfo->Height);
        pNvPatch->setBasis((NV_PATCH_BASIS_TYPE)pRSInfo->Basis);    //int cast to deal with enum casts.
        pNvPatch->setOrder(pRSInfo->Order);
        pNvPatch->setStride(pRSInfo->Stride);                       //IS THIS RIGHT??

        //alwasy set as fractional tessellation until they get their renderstate defined.
        if(pContext->dwRenderState[D3DRS_PATCHEDGESTYLE] == 0x1)
            pNvPatch->setTessMode(NV_PATCH_FLAG_TESS_FRACTIONAL);
        else pNvPatch->setTessMode(NV_PATCH_FLAG_TESS_INTEGER);
        
        //do this AFTER the normal/uv/stream setup cause they use these variable to figure out what to allocate
        pNvPatch->setCalc(NV_PATCH_DIRTY_TESSELLATION | NV_PATCH_DIRTY_CONTROL_POINTS);
        dwDirty = NV_PATCH_DIRTY_TESSELLATION | NV_PATCH_DIRTY_CONTROL_POINTS;

        //defined in # of vertices in a logical 2 dimensional Vertex buffer
        pNvPatch->setVertexOffset(pRSInfo->StartVertexOffsetHeight * pRSInfo->Stride + pRSInfo->StartVertexOffsetWidth);

        //copy data to scratch.
        pNvPatch->setupStreams(pContext);
    }

    //setup celsius strides for inlining vertices
#ifdef HOSURF_ENABLE
    //set ptr to cache parameters
    nvHWLockTextures (pContext);

    switch(pNvPatch->getBasis()){
    case NV_PATCH_BASIS_BEZIER:

        //if bezier don't cycle through multiple patches in the width/height fashion as bsplines
        //if we dirtied the control points -- free them up and reallocate some cached space for them.
        if(dwDirty & NV_PATCH_DIRTY_CONTROL_POINTS){
            pNvPatch->copyDataToScratch(pContext,0,0);
            pNvPatch->freeCached();     //free up old cached data and reinit for a new patch to be built.
            if(!pNvPatch->allocCached(1)){
                retVal = D3DERR_DRIVERINTERNALERROR;
                goto nvPatch_Mem_Fault;
            }
            pNvPatch->getCachedData(0);
        }

        pNvPatch->setTextureOffsets(0.0,1.0,0.0,1.0);
        pNvPatch->setupStreams(pContext);
    
#ifdef DCR_CACHEDPB_OPT
        if(dwDirty == NV_PATCH_DIRTY_NONE){
            if(pNvPatch->getCachedPBSize() == 0){
                DWORD dwWC = pDriverData->nvPusher.getWrapCount();
                DWORD dwCPut = pDriverData->nvPusher.getPut();
                retVal = nvEvalPatchSetup(pNvPatch->getInfo());    
                nvAllocInfoScratch(pNvPatch->getInfo());
                retVal = nvEvalPatch(pNvPatch->getInfo());
                if(dwWC == pDriverData->nvPusher.getWrapCount()){
                    //we didn't actually wrap so copy off the pb to be used later.
                    pNvPatch->allocCachedPB((pDriverData->nvPusher.getPut() - dwCPut));
                    nvMemCopy((unsigned char *)pNvPatch->getCachedPB(),(unsigned char *)dwCPut, pNvPatch->getCachedPBSize());
                }
            }
            else{
                DWORD dwSize = pNvPatch->getCachedPBSize();
                DWORD *dwPB = (DWORD *)(&(pNvPatch->getCachedPB()[0]));
                pDriverData->nvPusher.makeSpace(dwSize);
                nvMemCopy(pDriverData->nvPusher.getPut(), (DWORD)dwPB, dwSize);               
                pDriverData->nvPusher.inc(dwSize>>2);    //push the put forward                    

                /* DWORD dwPut;
                DWORD dwSize = (pNvPatch->getCachedPBSize() % (PB_DUMP_SIZE*sizeof(DWORD)));
                DWORD dwCount = (pNvPatch->getCachedPBSize() - dwSize)/(PB_DUMP_SIZE*sizeof(DWORD));
                DWORD *dwPB;    
                DWORD i;

                dwPB  = (DWORD *)(&(pNvPatch->getCachedPB()[0]));
                for(i=0; i < dwCount; i++){
                    pDriverData->nvPusher.makeSpace(PB_DUMP_SIZE);
                    dwPut = pDriverData->nvPusher.getPut();             //don't calc cause the PB could wrap.
                    nvMemCopy(dwPut, (DWORD)dwPB, PB_DUMP_SIZE*sizeof(DWORD));
                    pDriverData->nvPusher.inc(PB_DUMP_SIZE);            //push the put forward                    
                    dwPB+=PB_DUMP_SIZE;
                }
                pDriverData->nvPusher.makeSpace(dwSize);
                nvMemCopy(pDriverData->nvPusher.getPut(), (DWORD)dwPB, dwSize);               
                pDriverData->nvPusher.inc(dwSize>>2);    //push the put forward */
            }
            pDriverData->nvPusher.start(TRUE);
        }
        else {
            pNvPatch->freeCachedPB();
#endif
            retVal = nvEvalPatchSetup(pNvPatch->getInfo());    
            nvAllocInfoScratch(pNvPatch->getInfo());
            if(dwDirty & NV_PATCH_DIRTY_CONTROL_POINTS){
                nvAllocFDMatricesEtc(pNvPatch->getInfo());
                retVal |= nvEvalPatch(pNvPatch->getInfo());
                pNvPatch->saveCachedData(0);
            }
            else{
                retVal |= nvEvalPatch(pNvPatch->getInfo());
            }
            pDriverData->nvPusher.start(FALSE);
#ifdef DCR_CACHEDPB_OPT
        }
#endif
        break;
    case NV_PATCH_BASIS_BSPLINE:
    case NV_PATCH_BASIS_CATMULL_ROM:
        DWORD upatches = pNvPatch->getWidth()  - pNvPatch->getOrder();
        DWORD vpatches = pNvPatch->getHeight() - pNvPatch->getOrder();
        float ustep,vstep;

        //allocate some data for
        if(dwDirty & NV_PATCH_DIRTY_CONTROL_POINTS){
            pNvPatch->freeCached();     //free up old cached data and reinit for a new patch to be rebuilt.
            if(!pNvPatch->allocCached(upatches*vpatches)){
                retVal = D3DERR_DRIVERINTERNALERROR;
                goto nvPatch_Mem_Fault;
            }

        }

        //reset the tessellation value here to account for subpatches.  The tessellation is meant to describe
        //the ENTIRE patch, not each subpatch.  Divide the tessellation by the # of subpatches in each direction.
        if(dwDirty){
            if(pContext->dwRenderState[D3DRS_PATCHEDGESTYLE] == 0x1){
                //fractional
                Segs[0] = max(2.0,Segs[0]/upatches); Segs[1] = max(2.0,Segs[1]/vpatches);
                Segs[2] = max(2.0,Segs[2]/upatches); Segs[3] = max(2.0,Segs[3]/vpatches);
            }
            else{
                //integer
                Segs[0] = max(1.0,int(Segs[0]/upatches+0.5)); Segs[1] = max(1.0,int(Segs[1]/vpatches+0.5));
                Segs[2] = max(1.0,int(Segs[2]/upatches+0.5)); Segs[3] = max(1.0,int(Segs[3]/vpatches+0.5));
            }

            switch(dwDirty){ //deliverately falls through.
            case NV_PATCH_DIRTY_CONTROL_POINTS | NV_PATCH_DIRTY_TESSELLATION:
            case NV_PATCH_DIRTY_CONTROL_POINTS:
                pNvPatch->setOriginalTessellation(Segs, NV_PATCH_FLAG_TYPE_PATCH);
            case NV_PATCH_DIRTY_TESSELLATION:
                pNvPatch->setTessellation(Segs, NV_PATCH_FLAG_TYPE_PATCH);
            }
        }

        //bpsline -- observe that width/height in PRSInfo implies multiple subpatches...
        //render and cache each one seperately.
        //pNvPatch->setupStreams(pContext);

        ustep = 1.0/upatches;
        vstep = 1.0/vpatches;
        unsigned int i,j;
        for(i=0; i < (upatches); i++){
            for(j=0; j < (vpatches); j++){
                pNvPatch->setCalc(dwDirty);     //reset these flags everytime through loop
                                                //because they'll be cleared internally for each map
                                                //though technically I'm rebuilding all here
                pNvPatch->getCachedData(upatches*i+j);
                pNvPatch->setTextureOffsets(i*ustep, (i+1)*ustep, j*vstep, (j+1)*vstep);
                pNvPatch->copyDataToScratch(pContext, i, j);
                pNvPatch->setupStreams(pContext);

                //if we dirtied the control points -- free them up and reallocate some cached space for them.
                nvEvalPatchSetup(pNvPatch->getInfo());
                if(dwDirty & NV_PATCH_DIRTY_CONTROL_POINTS){
                    nvAllocFDMatricesEtc(pNvPatch->getInfo());                
                    nvAllocInfoScratch(pNvPatch->getInfo());
                    retVal = nvEvalPatch(pNvPatch->getInfo());
                    pNvPatch->saveCachedData(upatches*i+j);
                } else {
                    retVal = nvEvalPatch(pNvPatch->getInfo());
                }
                nvEvalPatchCleanup(pNvPatch->getInfo());
            }
        }
    }
    
    nvEvalPatchCleanup(pNvPatch->getInfo());
    nvHWUnlockTextures (pContext);
#else
    retVal = D3D_OK;
#endif

    //if handle is 0 we don't want this cached and should release the pNvObj, patch, and cached data
    if(dwHandle == 0){
        pObj->release();
    }

nvPatch_Mem_Fault:
    return retVal;

}

CPatch::CPatch(CNvObject *pObj, DWORD dwHandle)
{
    int i;
    m_dwHandle = dwHandle;
    m_pWrapperObject = pObj;

    m_PatchData.backendType = NV_PATCH_BACKEND_CELSIUS; //default
    m_subpatchcount = 0;
    m_dwStride = m_dwWidth = m_dwHeight = 0;
    m_dwOrder = 1;
    m_PatchData.srcNormal = -1;
    m_PatchData.dstNormal = -1;
    m_PatchData.maxSwatch = NV_PATCH_KELVIN_SWATCH_SIZE;
    m_PatchData.nAttr = NV_PATCH_NUMBER_OF_ATTRIBS;
    m_PatchData.maxAttr = NV_PATCH_NUMBER_OF_ATTRIBS;
    m_PatchData.maxOrder = NV_PATCH_MAX_ORDER;    
    m_PatchData.evalEnables = 0;
    //ffm_PatchData.evAlloc9FDMatrices.pBase = NULL;
    m_PatchData.quadInfo = NULL;
    m_PatchData.cachedPB = NULL;
    m_PatchData.cachedPBSize = 0;
    m_PatchData.cachedPBCounter = 0;
    m_PatchData.normalPatch = NULL;
    m_PatchData.UVPatch = NULL;
    for(i=0; i < 8; i++){
        m_PatchData.srcUV[i] = -1;
        m_PatchData.dstUV[i] = -1; }
    m_PatchData.tess.tensor.nu0 = m_PatchData.tess.tensor.nu1 = m_PatchData.tess.tensor.nv0 = m_PatchData.tess.tensor.nv1 = 0;
       //init ptrs to null
    m00 = m10 = m01 = m11 = NULL;
    guardQF = NULL;
    guardTF = NULL;
    cachedQuadInfo = NULL;

    //output buffer pts for a DP2 stream
    m_PatchData.buffer = 0;
    m_PatchData.bufferLength = NULL;

    //
    for(i=0; i < MAX_EV_CACHE; i++){
        m_PatchData.pCache[i] = &MyCache[i];       
    }

    //flag for assembly loop optimizations
    m_PatchData.cpuType = pDriverData->nvD3DPerfData.dwCPUFeatureSet;

    //setup default render flags
    m_PatchData.flags = 0x0;    //TESS=FLOAT, PRIM=QUAD, NO BUFFER ALLOCATED

    //setup allocation callbacks.
    m_PatchData.callbacks = &m_callbacks;

    return;
}

CPatch::~CPatch(){
            
    freeCachedPB();
    freeCached();   //free any cached data we might have hanging around.

    return;
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

void CPatch::setOutputMode(NV_PATCH_BACKEND_TYPE rf)
{
    NV_PATCH_BACKEND_TYPE old_rf;
    old_rf = m_PatchData.backendType;

    //if stream type has changed, unhook old buffer
    switch(rf){
        case NV_PATCH_BACKEND_CELSIUS:
            m_PatchData.backend = &nvCelsiusImm_Backend;
            m_PatchData.maxSwatch = 0x40;
            break;
        case NV_PATCH_BACKEND_KELVIN:
            m_PatchData.backend = &nvKelvinImm_Backend;
            m_PatchData.maxSwatch = NV_PATCH_KELVIN_SWATCH_SIZE;
            break;
        default:
            nvAssert(0);
    }
    m_PatchData.backendType = rf;
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


BOOL CPatch::allocCached(DWORD dwNumSubPatches){
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
    cachedQuadInfo = new NV_PATCH_QUAD_INFO*[dwNumSubPatches];
    for(i=0; i < dwNumSubPatches; i++){ cachedQuadInfo[i] = NULL; }
    if(cachedQuadInfo == NULL){ failed = TRUE; }    
    
/*  
    for(subpatch = 0; subpatch < dwNumSubPatches; subpatch++){
        if(!(cachedQuadInfo[subpatch] = new NV_PATCH_QUAD_INFO)){ failed = TRUE;}
    }
*/

    return TRUE;
}

void CPatch::freeCached(){       
    DWORD i;

    for(i=0; i < this->m_subpatchcount; i++){       
        if(cachedQuadInfo[i]) delete cachedQuadInfo[i];
        cachedQuadInfo[i] = NULL;
    }
    if(cachedQuadInfo){ delete []cachedQuadInfo; cachedQuadInfo = NULL; }    
    m_PatchData.quadInfo = NULL;

    return;
}

void *CPatch::alloc(void *context, size_t bytes){
    BYTE *ptr = new BYTE[bytes];
    return (void*)ptr;
}

void CPatch::free(void *context, void *ptr){
    delete [](BYTE*)ptr;
    return;
}

void  CPatch::memcpy(void *dst, const void *src, size_t size){
    nvMemCopy(dst, const_cast <void *>(src), (DWORD)size);
    return;
}

void CPatch::setupStreams(PNVD3DCONTEXT pContext){
    DWORD i, dwOffset, dwVBStride, dwOrder, dwStream;
    BYTE *dwVBBase;
    NV_PATCH_VERTEX_FORMAT_TYPE dwType;
    

    CVertexShader *lpCurrent = pContext->pCurrentVShader;
    //16 is the # of attributes handled by Kelvin -- should probably be changed to an RM
    //configurable constant based on chip architecture.
    m_PatchData.nAttr = 0x0;

    for (i=0; i < NV_PATCH_NUMBER_OF_ATTRIBS; i++) {
        dwStream = lpCurrent->getVAStream(i);
        if ( lpCurrent                        &&
            (dwStream != 0xFFFFFFFF)          &&
            (lpCurrent->getVAFlag(i)==CVertexShader::AUTONONE) &&
            pContext->ppDX8Streams[dwStream])
        {
            dwType = (NV_PATCH_VERTEX_FORMAT_TYPE)lpCurrent->getVAType(i);
            //type of incoming data -- copy to scratch will expand to a 4-vector for the shared code
            //CelsiusBackend will convert it back.
            dwVBStride = 4*sizeof(float);   //stride size for shared code -- i.e. vector.
            dwVBBase = (BYTE*)&scratch[i][0][0];
            dwOffset = 0;
            dwOrder    = (DWORD)getOrder()+1;
        }

        else {
            //vsize+=0;
            dwVBBase = NULL;
            dwOffset = dwVBStride = dwOrder = 0;
            dwType = (NV_PATCH_VERTEX_FORMAT_TYPE)0;
        }
        setStream (i, dwVBBase + dwOffset, dwVBStride, dwOrder, dwType, dwOrder);
    }

    //get auto generated normals
    dwStream = pContext->pCurrentVShader->getAutoNormalStream();
    if (dwStream != -1) {        
        //destination should be the normal stream itself
        setAutoNormal (pContext->pCurrentVShader->getVASrc(dwStream), dwStream);
    }
 
    //get auto generated texture coordinates
    for (i=0; i < 8; i++){
        //shouldn't need to swap here.
        dwStream = pContext->pCurrentVShader->getAutoTextureStream(i);
        if (dwStream != -1) {            
            //destination should be texture stream itself
            setAutoUV (pContext->pCurrentVShader->getVASrc(dwStream), dwStream, i);
        }
    }

    //if stream 2 is enabled it is assumed there is a guard curved normal active... 
    dwStream = pContext->pCurrentVShader->getVAStream(2);
    if(dwStream != 0xFFFFFFFF){ m_PatchData.dstNormal = 0x2; }    //BUG BUG HACK!!!!

    setVertexStride (pContext->hwState.dwInlineVertexStride >> 2);

    return;
}

#define VIEW_AS_DWORD(f) (*(DWORD *)(&(f)))
void CPatch::copyDataToScratch(PNVD3DCONTEXT pContext, DWORD offsetu, DWORD offsetv){

    DWORD i,j,k, pitch, vertexcount;
    float *data;
    DWORD evalEnables, dwOffset, dwStream, dwStride;
    DWORD order;
    order = m_dwOrder+1;

    evalEnables = m_PatchData.evalEnables;

    for (i=0; i < NV_PATCH_NUMBER_OF_ATTRIBS; i++) {
        if (evalEnables & (1<<i)) {
            if(pContext->pCurrentVShader && pContext->ppDX8Streams[pContext->pCurrentVShader->getVAStream(i)]){
                dwStream   = pContext->pCurrentVShader->getVAStream(i);
                data = (float*) pContext->ppDX8Streams[dwStream]->getAddress();
                dwOffset = (offsetv*getWidth() + offsetu) + m_dwVertexOffset;
                dwStride =  pContext->ppDX8Streams[dwStream]->getVertexStride() >> 2; //convert to float
                data += dwOffset * dwStride;
                //BUG BUG: THIS IS WEIRD TO ADD AN OFFSET TO OFFSETS?? NEED TO LOOK INTO THIS
                data += (pContext->pCurrentVShader->getVAOffset(i)>>0x2);  //vertex offset converted to offset in float
                pitch = getStride()*dwStride;   //getStride returns a stride value in vertices for mosaiced patches
                vertexcount = 0;

                switch(m_PatchData.maps[i].Originaltype){
                    case NV_PATCH_VERTEX_FORMAT_FLOAT_1:
                        for(j=0; j < order; j++){
                            for(k=0; k < order*dwStride; k+=dwStride){
                                scratch[i][vertexcount][0] = data[k+0]; scratch[i][vertexcount][1] = 0.0;
                                scratch[i][vertexcount][2] = 0.0;       scratch[i][vertexcount][3] = 1.0; vertexcount++;
                            }
                            data+=pitch;
                        }
                        break;
                    case NV_PATCH_VERTEX_FORMAT_FLOAT_2:
                        for(j=0; j < order; j++){
                            for(k=0; k < order*dwStride; k+=dwStride){
                                scratch[i][vertexcount][0] = data[k+0]; scratch[i][vertexcount][1] = data[k+1];
                                scratch[i][vertexcount][2] = 0.0;       scratch[i][vertexcount][3] = 1.0; vertexcount++;
                            }
                            data+=pitch;
                        }
                        break;
                    case NV_PATCH_VERTEX_FORMAT_FLOAT_3:
                        for(j=0; j < order; j++){
                            for(k=0; k < order*dwStride; k+=dwStride){
                                scratch[i][vertexcount][0] = data[k+0]; scratch[i][vertexcount][1] = data[k+1];
                                scratch[i][vertexcount][2] = data[k+2]; scratch[i][vertexcount][3] = 1.0; vertexcount++;
                            }
                            data+=pitch;
                        }
                        break;
                    case NV_PATCH_VERTEX_FORMAT_FLOAT_4:
                        for(j=0; j < order; j++){
                            for(k=0; k < order*dwStride; k+=dwStride){
                                scratch[i][vertexcount][0] = data[k+0]; scratch[i][vertexcount][1] = data[k+1];
                                scratch[i][vertexcount][2] = data[k+2]; scratch[i][vertexcount][3] = data[k+3]; vertexcount++;
                            }
                            data+=pitch;
                        }
                        break;
                    case NV_PATCH_VERTEX_FORMAT_D3DCOLOR:
                    case NV_PATCH_VERTEX_FORMAT_UBYTE:
                        for(j=0; j < order; j++){
                            for(k=0; k < order*dwStride; k+=dwStride){
                                scratch[i][vertexcount][0] = ((VIEW_AS_DWORD(data[k])&0x00ff0000)>>16)/255.0;
                                scratch[i][vertexcount][1] = ((VIEW_AS_DWORD(data[k])&0x0000ff00)>>8 )/255.0;
                                scratch[i][vertexcount][2] = ((VIEW_AS_DWORD(data[k])&0x000000ff)>>0 )/255.0;
                                scratch[i][vertexcount][3] = ((VIEW_AS_DWORD(data[k])&0xff000000)>>24)/255.0;                                             
                                vertexcount++;
                            }
                            data+=pitch;
                        }
                        break;
                    default:
                        nvAssert(0);    //short cases -- what do we do here?
                } // switch
            }
        }
    }

    if(m_dwOrder > 1){  //conversions not necessary for linear maps.
        switch(m_PatchData.basis){
        case NV_PATCH_BASIS_BEZIER:
            //convert data inline from BSpline to Scratch.
            break;
        case NV_PATCH_BASIS_BSPLINE:
            //convert basis inline from BSpline to Scratch.
            convertBsplineToBezier();
            break;
        case NV_PATCH_BASIS_CATMULL_ROM:
            nvAssert(m_dwOrder == 3);   //we only ever expect a cubic catmull rom here.
            //convert basis inline from Catmul Rom to Scratch.
            convertCatmullRomToBezier();
            break;
        }
    }


    return;
}

static int trilookup_1[] = {2, 1, 0};
static int trilookup_3[] = {12,9,8,6,5,4,3,2,1,0};
static int trilookup_5[] = {30,25,24,20,19,18,15,14,13,12,10,9,8,7,6,5,4,3,2,1,0};
void CPatch::copyDataToScratchTri(PNVD3DCONTEXT pContext, DWORD offsetu, DWORD offsetv){

    DWORD i,j,pitch;
    float *data;
    DWORD evalEnables, dwOffset, dwStream, dwStride, vertex;
    DWORD order;
    order = m_dwOrder+1;
    int *lookup;
    DWORD count = 0;
    evalEnables = m_PatchData.evalEnables;
    for(i=0; i < NV_PATCH_NUMBER_OF_ATTRIBS; i++){
        if(evalEnables & (1<<i)){
            //BUG BUG: THIS IS WEIRD TO ADD AN OFFSET TO OFFSETS?? NEED TO LOOK INTO THIS
            if(pContext->pCurrentVShader && pContext->ppDX8Streams[pContext->pCurrentVShader->getVAStream(i)]){
                dwStream   = pContext->pCurrentVShader->getVAStream(i);
                data = (float*) pContext->ppDX8Streams[dwStream]->getAddress();
                dwOffset = (offsetv*getWidth() + offsetu) + m_dwVertexOffset;
                dwStride = pContext->ppDX8Streams[dwStream]->getVertexStride() >> 2; //convert to float
                data += dwOffset * dwStride;
                data += (pContext->pCurrentVShader->getVAOffset(i)>>2);  //vertex offset converted to offset in bytes
                pitch = getStride()*dwStride; //in vertices
                switch(order){
                    case 2: lookup = trilookup_1; count = 3;  break;    //linear
                    case 4: lookup = trilookup_3; count = 10; break;    //cubic
                    case 6: lookup = trilookup_5; count = 21; break;    //quintic
                    default: nvAssert(0);                   //shouldn't ever be here.
                }
                switch(m_PatchData.maps[i].Originaltype) {
                case NV_PATCH_VERTEX_FORMAT_FLOAT_1:
                    for(j=0; j < count; j++){
                        vertex = lookup[j];
                        scratch[i][vertex][0] = data[0];   scratch[i][vertex][1] = 0.0;
                        scratch[i][vertex][2] = 0.0;       scratch[i][vertex][3] = 1.0;
                        data+=dwStride;
                    }
                    break;
                case NV_PATCH_VERTEX_FORMAT_FLOAT_2:
                    for(j=0; j < count; j++){
                        vertex = lookup[j];
                        scratch[i][vertex][0] = data[0];   scratch[i][vertex][1] = data[1];
                        scratch[i][vertex][2] = 0.0;       scratch[i][vertex][3] = 1.0;
                        data+=dwStride;
                    }
                    break;
                case NV_PATCH_VERTEX_FORMAT_FLOAT_3:
                    for(j=0; j < count; j++){
                        vertex = lookup[j];
                        scratch[i][vertex][0] = data[0];   scratch[i][vertex][1] = data[1];
                        scratch[i][vertex][2] = data[2];   scratch[i][vertex][3] = 1.0;
                        data+=dwStride;
                    }
                    break;
                case NV_PATCH_VERTEX_FORMAT_FLOAT_4:
                    for(j=0; j < count; j++){
                        vertex = lookup[j];
                        scratch[i][vertex][0] = data[0];   scratch[i][vertex][1] = data[1];
                        scratch[i][vertex][2] = data[2];   scratch[i][vertex][3] = data[3];
                        data+=dwStride;
                    }
                    break;
                case NV_PATCH_VERTEX_FORMAT_UBYTE:
                case NV_PATCH_VERTEX_FORMAT_D3DCOLOR:
                    for(j=0; j < count; j++){
                        vertex = lookup[j];
                        scratch[i][vertex][0] = ((VIEW_AS_DWORD(data[0])&0x00ff0000)>>16)/255.0;
                        scratch[i][vertex][1] = ((VIEW_AS_DWORD(data[0])&0x0000ff00)>>8 )/255.0;
                        scratch[i][vertex][2] = ((VIEW_AS_DWORD(data[0])&0x000000ff)>>0 )/255.0;
                        scratch[i][vertex][3] = ((VIEW_AS_DWORD(data[0])&0xff000000)>>24)/255.0;                                             
                        data+=dwStride;
                    }
                    break;
                default:
                    nvAssert(0);
                }
            }
        }
    }
    return;
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
    float in1_6x,in1_6y,in1_6z,in1_6w;

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

    in1_6x = in[stride+0];
    in1_6y = in[stride+1];
    in1_6z = in[stride+2];
    in1_6w = in[stride+3];

    // Note about precision:
    // We want to ensure that the same formulas give the same result, 
    // regardless of the order of input data.  Unfortunately, floating
    // point arithmetic is not associative, and hence we need to force
    // a consistent order of operations.  This is not possible in the 
    // general case without actually sorting your data, but here we make 
    // the observation that in any one formula, there are never more than
    // two components which could potentially be swapped (i.e., they
    // have the same scaling value).  We thus group components in this way.

    in[0] =          (1.0f/6.0f)*(in[0] + in[2*stride+0]) + in1_4x;  //x
    in[1] =          (1.0f/6.0f)*(in[1] + in[2*stride+1]) + in1_4y;  //y
    in[2] =          (1.0f/6.0f)*(in[2] + in[2*stride+2]) + in1_4z;  //z
    in[3] =          (1.0f/6.0f)*(in[3] + in[2*stride+3]) + in1_4w;  //w

    in[stride+0] =   in1_4x + (2.0f/6.0f) * in[2*stride+0];          //x
    in[stride+1] =   in1_4y + (2.0f/6.0f) * in[2*stride+1];          //y
    in[stride+2] =   in1_4z + (2.0f/6.0f) * in[2*stride+2];          //z
    in[stride+3] =   in1_4w + (2.0f/6.0f) * in[2*stride+3];          //w

    in[2*stride+0] = in1_2x + in2_4x;                                //x
    in[2*stride+1] = in1_2y + in2_4y;                                //y
    in[2*stride+2] = in1_2z + in2_4z;                                //z
    in[2*stride+3] = in1_2w + in2_4w;                                //w

    in[3*stride+0] = (1.0f/6.0f)*(in1_6x + in[3*stride+0]) + in2_4x; //x
    in[3*stride+1] = (1.0f/6.0f)*(in1_6y + in[3*stride+1]) + in2_4y; //y
    in[3*stride+2] = (1.0f/6.0f)*(in1_6z + in[3*stride+2]) + in2_4z; //z
    in[3*stride+3] = (1.0f/6.0f)*(in1_6w + in[3*stride+3]) + in2_4w; //w
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
                catrom2bez3(&scratch[i][0][0],  4);  //row1
                catrom2bez3(&scratch[i][4][0],  4);  //row2
                catrom2bez3(&scratch[i][8][0],  4);  //row3
                catrom2bez3(&scratch[i][12][0], 4);  //row4
                catrom2bez3(&scratch[i][0][0], 16);  //column 1
                catrom2bez3(&scratch[i][1][0], 16);  //column 2
                catrom2bez3(&scratch[i][2][0], 16);  //column 3
                catrom2bez3(&scratch[i][3][0], 16);  //column 4
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
                bsp2bez3(&scratch[i][0][0],  4);  //row1
                bsp2bez3(&scratch[i][4][0],  4);  //row2
                bsp2bez3(&scratch[i][8][0],  4);  //row3
                bsp2bez3(&scratch[i][12][0], 4);  //row4
                bsp2bez3(&scratch[i][0][0], 16);  //column 1
                bsp2bez3(&scratch[i][1][0], 16);  //column 2
                bsp2bez3(&scratch[i][2][0], 16);  //column 3
                bsp2bez3(&scratch[i][3][0], 16);  //column 4
                break;
            case 5:
                //convert quintic bezier patch.
                bsp2bez5(&scratch[i][0][0],  4);  //row1
                bsp2bez5(&scratch[i][6][0],  4);  //row2
                bsp2bez5(&scratch[i][12][0], 4);  //row3
                bsp2bez5(&scratch[i][18][0], 4);  //row4
                bsp2bez5(&scratch[i][24][0], 4);  //row1
                bsp2bez5(&scratch[i][30][0], 4);  //row2
                bsp2bez5(&scratch[i][0][0],  24);  //column1
                bsp2bez5(&scratch[i][1][0],  24);  //column2
                bsp2bez5(&scratch[i][2][0],  24);  //column3
                bsp2bez5(&scratch[i][3][0],  24);  //column4
                bsp2bez5(&scratch[i][4][0],  24);  //column5
                bsp2bez5(&scratch[i][5][0],  24);  //column6
                break;
            default:
                nvAssert(0);
                break;
            }
        }
    }
}
