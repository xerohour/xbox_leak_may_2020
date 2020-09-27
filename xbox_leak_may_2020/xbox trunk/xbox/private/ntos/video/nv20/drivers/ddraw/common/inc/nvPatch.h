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
//  Module: nvPatch.h
//      description here
//
// **************************************************************************
//
//  History:
//      Daniel ROhrer?          Jul00           DX8 development
//
// **************************************************************************

#ifndef _nvPatch_h
#define _nvPatch_h

#ifdef WINNT
#include "driver.h"
#endif
#include "nvPatchInterface.h"

#define CPATCH_DP2_BUFFER_SIZE 8000
extern NV_PATCH_BACKEND nvCelsiusImm_Backend;
extern NV_PATCH_BACKEND nvKelvinImm_Backend;

//max D3DOrder is 5 so 6x6=36 ctrl pts per patch max here.
// scratch[attribute][u][v][xyzw]

class CPatch {
    private:
        DWORD               m_dwFlags;          //
        DWORD               m_dwHandle;         //cache patch handle
        CNvObject          *m_pWrapperObject;   //

        //copy of pRSInfo data
        DWORD               m_subpatchcount;
        DWORD               m_dwHeight, m_dwWidth, m_dwOrder, m_dwStride;
        DWORD               m_dwVertexOffset;
        NV_PATCH_VERTEX_FORMAT_TYPE m_dwTypes[NV_PATCH_NUMBER_OF_ATTRIBS];

        //cached data ptrs
        FDMatrix            **m00, **m10, **m01, **m11;
        NV_PATCH_FRAC_QUAD_GUARD_INFO **guardQF;
        NV_PATCH_QUAD_INFO       **cachedQuadInfo;
        NV_PATCH_FRAC_TRI_GUARD_INFO  **guardTF;
        //temporary data ptrs -- we need to allocate a patches worth of data then free it at the end.

        NV_PATCH_INFO       m_PatchData;        //patch struct to hand to shared code

        //callback functions
        static NV_PATCH_DRIVER_CALLBACKS m_callbacks;
        static void *alloc         (void *context, size_t bytes);
        static void free           (void *context, void *ptr   );
        static void memcpy         (void *dst, const void *src, size_t size);

        void setStream      (UINT streamID, void *memptr, DWORD stride, DWORD pitch,
                             NV_PATCH_VERTEX_FORMAT_TYPE mt, UINT order);
    public:

        CPatch(CNvObject *pObj,DWORD dwHandle);
        ~CPatch(void);

        void getCachedData  (DWORD subpatch);       //copy the 'nth' subpatch FD ptrs over to m_PatchData for rendering
        void saveCachedData (DWORD subpatch);       //save the 'nth' subpatch FD ptrs from m_PatchData (i.e. cache new data)
        void freeCached();                          //free any cached data members
        BOOL allocCached(DWORD);                    //alloc space for n 'subpatches' worth of FD coeffs/guards.

        void setupStreams   (PNVD3DCONTEXT pContext);

        void copyDataToScratch(PNVD3DCONTEXT,DWORD,DWORD);
        void copyDataToScratchTri(PNVD3DCONTEXT,DWORD,DWORD);
        void convertBsplineToBezier();              //helper routines -- converts patch data inplace
        void convertCatmullRomToBezier();           //helper routines -- converts patch data inplace
        void setBasis       (NV_PATCH_BASIS_TYPE bt)    {m_PatchData.basis = bt; }
        UINT getBasis()                                 {return m_PatchData.basis; }
        NV_PATCH_INFO*      getInfo()                   { return &m_PatchData; }

        void setTessMode    (UINT rf)                   {(rf == NV_PATCH_FLAG_TESS_FRACTIONAL ? m_PatchData.flags |= NV_PATCH_FLAG_TESS_FRACTIONAL : 
                                                                                                m_PatchData.flags &= ~NV_PATCH_FLAG_TESS_MASK);}        
        void setPrimMode    (UINT rf)                   {(rf == NV_PATCH_FLAG_TYPE_TRIANGULAR ? m_PatchData.flags |= NV_PATCH_FLAG_TYPE_TRIANGULAR : 
                                                                                                m_PatchData.flags &= ~NV_PATCH_FLAG_TYPE_MASK);}        
        void setTessellation(float *segs, int flag)     { if((flag & NV_PATCH_FLAG_TYPE_MASK) == NV_PATCH_FLAG_TYPE_TRIANGULAR)
                                                          {  m_PatchData.tess.tri.n1 = segs[1]; 
                                                             m_PatchData.tess.tri.n2 = segs[2]; 
                                                             m_PatchData.tess.tri.n3 = segs[0]; }
                                                          else
                                                          {  m_PatchData.tess.tensor.nu0 = segs[0]; m_PatchData.tess.tensor.nv0 = segs[3]; 
                                                             m_PatchData.tess.tensor.nu1 = segs[2]; m_PatchData.tess.tensor.nv1 = segs[1]; }
                                                        }        
        void setOriginalTessellation(float *segs, int flag){ if((flag & NV_PATCH_FLAG_TYPE_MASK) == NV_PATCH_FLAG_TYPE_TRIANGULAR)
                                                          {  m_PatchData.originaltess.tri.n1 = segs[1]; 
                                                             m_PatchData.originaltess.tri.n2 = segs[2]; 
                                                             m_PatchData.originaltess.tri.n3 = segs[0]; }
                                                          else
                                                          {  m_PatchData.originaltess.tensor.nu0 = segs[0]; m_PatchData.originaltess.tensor.nv0 = segs[3]; 
                                                             m_PatchData.originaltess.tensor.nu1 = segs[2]; m_PatchData.originaltess.tensor.nv1 = segs[1]; }
                                                        }        
       
        //THERE ARE SIDE AFFECTS TO THIS CALL IN REGARDS TO MEMORY ALLOCATION
        void setOutputMode  (NV_PATCH_BACKEND_TYPE rf);
        void prepareBuffer  (long);                     //stream type should be set BEFORE calling this

        void setVertexOffset(DWORD dwOffset)            {m_dwVertexOffset = dwOffset;}
        void setAutoNormal  (UINT dwSrc, UINT dwDst)    {m_PatchData.srcNormal = dwSrc; m_PatchData.dstNormal = dwDst;
                                                         m_PatchData.flags |= NV_PATCH_FLAG_AUTO_NORMAL; }
        void setAutoUV      (UINT dwSrc, UINT dwDst, int i){m_PatchData.srcUV[i] = dwSrc; m_PatchData.dstUV[i] = dwDst;
                                                         m_PatchData.flags |= NV_PATCH_FLAG_AUTO_UV;}
        void  setDimension  (UINT width, UINT height)   {m_dwWidth = width; m_dwHeight = height;}
        UINT  getWidth      ()                          {return m_dwWidth;}
        UINT  getHeight     ()                          {return m_dwHeight;}
        void  setOrder      (DWORD order)               {m_dwOrder = order;}
        DWORD getOrder      ()                          {return m_dwOrder;}
        void  setStride     (DWORD stride)              {m_dwStride = stride;}
        DWORD getStride     ()                          {return m_dwStride;}
        void  setTextureOffsets(float startu, float endu,
                               float startv, float endv){ m_PatchData.startu = startu; m_PatchData.startv = startv;
                                                          m_PatchData.endu   = endu;   m_PatchData.endv   = endv; }

        float getSeg        (UINT i)                    {switch(i){
                                                            case 0: return m_PatchData.tess.tensor.nu0;
                                                            case 1: return m_PatchData.tess.tensor.nv1;
                                                            case 2: return m_PatchData.tess.tensor.nu1;
                                                            case 3: return m_PatchData.tess.tensor.nv0;
                                                            default: return 0;
                                                        }}
        void setCalc        (UINT df)                   {for(int i=0; i < NV_PATCH_NUMBER_OF_ATTRIBS; i++) m_PatchData.maps[i].dirtyFlags = df;}
        UINT getCalc        ()                          {return m_PatchData.maps[0].dirtyFlags;}
        void setContext     (PNVD3DCONTEXT pContext)    {m_PatchData.context = (void*)pContext;}
        void setVertexStride(int size)                  {m_PatchData.vertexSize = size;}
        unsigned char* getCachedPB()                    {return m_PatchData.cachedPB;}
        unsigned long  getCachedPBSize()                {return m_PatchData.cachedPBSize;}
        void           allocCachedPB(unsigned long dwSize){m_PatchData.cachedPB = new unsigned char[dwSize]; 
                                                         m_PatchData.cachedPBSize = dwSize;
                                                         m_PatchData.cachedPBCounter = 0;
                                                        }
        void           freeCachedPB()                   {if(m_PatchData.cachedPB) delete []m_PatchData.cachedPB; 
                                                                                  m_PatchData.cachedPB = NULL;
                                                                                  m_PatchData.cachedPBSize = 0;}        
};

#endif

