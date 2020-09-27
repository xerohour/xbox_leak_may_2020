/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       patch.hpp
 *  Content:    High order surface implementation.
 *
 ****************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#ifndef __PATCH_HPP__
#define __PATCH_HPP__

enum {
    AUTONONE      = 0,
    AUTONORMAL    = 1,
    AUTOTEXCOORD  = 2,
};

enum {
    VERTEX_ARRAY  = D3DVSDE_POSITION,
    WEIGHT_ARRAY  = D3DVSDE_BLENDWEIGHT,
    NORMAL_ARRAY  = D3DVSDE_NORMAL,
    DIFFUSE_ARRAY = D3DVSDE_DIFFUSE,
    SPEC_ARRAY    = D3DVSDE_SPECULAR,
    TEX0_ARRAY    = D3DVSDE_TEXCOORD0,
    TEX1_ARRAY    = D3DVSDE_TEXCOORD1,
    TEX2_ARRAY    = D3DVSDE_TEXCOORD2,
    TEX3_ARRAY    = D3DVSDE_TEXCOORD3,
};

enum {
    STREAM_NONE = 0xFFFFFFFF,
};

class CPatch
{
private:

    DWORD m_dwFlags;
    DWORD m_dwHandle;

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

    void setStream      (UINT streamID, void *memptr, DWORD stride, DWORD pitch,
                         NV_PATCH_VERTEX_FORMAT_TYPE mt, UINT order);
public:

    BOOL Init(DWORD dwHandle);
    void UnInit(void);

    void getCachedData  (DWORD subpatch);       //copy the 'nth' subpatch FD ptrs over to m_PatchData for rendering
    void saveCachedData (DWORD subpatch);       //save the 'nth' subpatch FD ptrs from m_PatchData (i.e. cache new data)
    void freeCached();                          //free any cached data members
    BOOL allocCached(DWORD);                    //alloc space for n 'subpatches' worth of FD coeffs/guards.

    void setupStreams   ();

    void copyDataToScratch(DWORD,DWORD);
    void copyDataToScratchTri(DWORD,DWORD);
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
                                                      {  m_PatchData.tess.tri.n1 = segs[1]; m_PatchData.tess.tri.n2 = segs[0]; 
                                                      {  m_PatchData.tess.tri.n1 = segs[1]; 
                                                         m_PatchData.tess.tri.n3 = segs[2]; }
                                                         m_PatchData.tess.tri.n2 = segs[2]; 
                                                         m_PatchData.tess.tri.n3 = segs[0]; }
                                                      else
                                                      {  m_PatchData.tess.tensor.nu0 = segs[0]; m_PatchData.tess.tensor.nv0 = segs[3]; 
                                                         m_PatchData.tess.tensor.nu1 = segs[2]; m_PatchData.tess.tensor.nv1 = segs[1]; }
                                                    }        
    void setOriginalTessellation(float *segs, int flag){ if((flag & NV_PATCH_FLAG_TYPE_MASK) == NV_PATCH_FLAG_TYPE_TRIANGULAR)
                                                      {  m_PatchData.originaltess.tri.n1 = segs[1]; m_PatchData.originaltess.tri.n2 = segs[0]; 
                                                      {  m_PatchData.originaltess.tri.n1 = segs[1]; 
                                                         m_PatchData.originaltess.tri.n3 = segs[2]; }
                                                         m_PatchData.originaltess.tri.n2 = segs[2]; 
                                                         m_PatchData.originaltess.tri.n3 = segs[0]; }
                                                      else
                                                      {  m_PatchData.originaltess.tensor.nu0 = segs[0]; m_PatchData.originaltess.tensor.nv0 = segs[3]; 
                                                         m_PatchData.originaltess.tensor.nu1 = segs[2]; m_PatchData.originaltess.tensor.nv1 = segs[1]; }
                                                    }        
   
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
    void setVertexStride(int size)                  {m_PatchData.vertexSize = size;}
};

#endif // __PATCH_HPP__

} // end namespace
