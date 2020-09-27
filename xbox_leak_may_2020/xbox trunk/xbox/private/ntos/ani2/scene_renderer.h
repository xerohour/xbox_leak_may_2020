//
//	scene_renderer.h
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved

#ifndef __SCENE_RENDERER_H__
#define __SCENE_RENDERER_H__

#include "xbs_math.h"
#include "prim_types.h"

///////////////////////////////////////////////////////////////////////////////
enum ShaderTypes
{
	st_Phong		= 0,
	st_Bump			= 1,
	st_Depth		= 2,
	st_ShadowMap	= 3,
	st_NoTypes		= 4,
};
///////////////////////////////////////////////////////////////////////////////
struct BaseStream
{
	D3DVECTOR p;
};
///////////////////////////////////////////////////////////////////////////////
struct BumpStream
{
	FLOAT	  u,v;
	D3DVECTOR s;
	D3DVECTOR t;
	D3DVECTOR n;
};
///////////////////////////////////////////////////////////////////////////////
struct PhongStream
{
	D3DVECTOR s;
	D3DVECTOR t;
	D3DVECTOR n;
};
///////////////////////////////////////////////////////////////////////////////
struct PrimitiveVersionRecord
{
	DWORD		dwVertexStart;
	DWORD		dwVertexCount;
	DWORD		dwIndexStart;
	DWORD		dwPrimCount;
	DWORD		dwIndexCount;
	DWORD		dwParameter;
};
///////////////////////////////////////////////////////////////////////////////
struct PrimitiveInstanceRecord
{
	unsigned short		idxVersion;

	short				idxPosAnim;
	short				idxRotAnim;
	
	D3DMATRIX			matOffset;		
	D3DMATRIX           matInvOffset;
	
	D3DMATRIX			matScale;
	D3DVECTOR4          vObjScale;
	
	D3DMATRIX			matScaleOffset; 

	bool				bHiZ;
};
///////////////////////////////////////////////////////////////////////////////
class PrimitiveSet
{
private:
	
	LPDIRECT3DVERTEXBUFFER8    pBaseStream;
	LPDIRECT3DVERTEXBUFFER8    pExtraStream;

	LPDIRECT3DINDEXBUFFER8     pIB;

	PrimitiveVersionRecord	 * aVersRecs;
	int						   nVersions;

	PrimitiveInstanceRecord  * aInstRecs;
	int						   nInstances;

	D3DPRIMITIVETYPE           d3dType;
	

	LPDIRECT3DTEXTURE8         pNormalMap;
	LPDIRECT3DCUBETEXTURE8	   pCubeMap;

	D3DVECTOR4				   vAmbient;
	D3DVECTOR4                 vDiffuse;
	D3DVECTOR4                 vSpecular;

	DWORD					   dwPShader;
	DWORD					   dwVShader;

	ShaderTypes				   shaderType;
	PrimitiveTypes             primitiveType;
	bool					   bHiZ;


	void createSphereVersion(const SphereVers *psphere,
							 BYTE *pbase,BYTE *pextra,
							 unsigned short *pi,
							 int idx,int ndet_bias );

	void createCylinderVersion(const CylinderVers *pcyl,
							   BYTE *pbase,BYTE *pextra,
							   unsigned short *pi,
							   int idx,int ndet_bias );

	void createConeVersion(const ConeVers  * pcone,
		                   BYTE *pbase,BYTE *pextra,
						   unsigned short *pi,
						   int idx, int ndet_bias );

	void createBoxVersion(BYTE *pbase,BYTE *pextra,
		                  unsigned short *pi,
						  int idx );

	void createTorusVersion(const TorusVers  * ptorus,
		                    BYTE *pbase,BYTE *pextra,
							unsigned short *pi, 
							int idx,int ndet_bias );

	void createSurfOfRevVersion(const SurfOfRevVers *psurf,
		                        BYTE *pbase,BYTE *pextra,
								unsigned short *pi,
								int idx,int ndet_bias);
	
	void initSphereInstance( const SphereInst *psphere, int idx);
	void initCylinderInstance( const CylinderInst *pcyl, int idx);
	void initConeInstance(const ConeInst *pcone,int idx);
	void initBoxInstance(const BoxInst *pbox,int idx);
	void initTorusInstance(const TorusInst *ptorus,int idx);
	void initSurfOfRevInstance(const SurfOfRevInst *psurf,int idx);

	void initSphereVersion(const SphereVers *psphere,int idx,int ndet_bias);
	void initCylinderVersion(const CylinderVers *pcyl,int idx,int ndet_bias);
	void initConeVersion(const ConeVers *pcone,int idx,int ndet_bias);
	void initBoxVersion(int idx);
	void initTorusVersion(const TorusVers *ptorus,int idx,int ndet_bias);
	void initSurfOfRevVersion(const SurfOfRevVers *psurf,int idx,int ndet_bias);
	
public:

	void Init();
	void UnInit();

	void render(const D3DLIGHT8 &light,bool b_query_blob);
	void renderZ();
	void renderShadowMap(bool b_hi_z);

	void create( const void      *		version_data, 
				 int					num_versions, 
		         const void      *		instance_data,
				 int					num_insts,
				 int					ndet_bias,
				 ShaderTypes			shader,
				 PrimitiveTypes			primitive,
				 LPDIRECT3DTEXTURE8		pnormal_map,
				 LPDIRECT3DCUBETEXTURE8 pcubemap );
};
///////////////////////////////////////////////////////////////////////////////
class SceneRenderer 
{
protected:

	PrimitiveSet	  * pSetHiDet[pt_NoTypes];
	PrimitiveSet	  * pSetLoDet[pt_NoTypes];

	IDirect3DSurface8 * pSBufTarget;			


    IDirect3DSurface8 * pRenderTarget;
    IDirect3DSurface8 * pZBuffer;				  			

    D3DSurface			fakeTarget;

    float				fZOffset;				  
    float				fZSlopeScale;
	
	bool				bUseLoDetail;

	D3DVECTOR4 * decompressQuats(short *p_quat_data, DWORD *p_sign_data,int nquats);
	D3DVECTOR  * decompressVecs(short *p_vec_data, int nvecs);
	short      * decompressIndices(char *p_index_data,int nindices);
	void		 updateSBuffer(bool b_hi_z);

	void createSceneGeometry(PrimitiveSet **pp_sets,int ndet_bias);

public:

	D3DVECTOR4			*pQuats;
	short				*pQuatIdSeq[NUM_ROT_SEQ];

	D3DVECTOR			*pPos;
	short				*pPosIdSeq[NUM_POS_SEQ];

	IDirect3DTexture8	*pSBufDepthHi;           
	IDirect3DTexture8   *pSBufDepthLo;

	D3DMATRIX			matWTSHi;
	D3DMATRIX			matWTSLo;

	D3DMATRIX			*pRotAnims;
	int					nRotAnims;

	D3DVECTOR           *pPosAnims;
	int					nPosAnims;

	void create();
	void destroy();

	void render(bool b_with_shadows,bool b_use_blob_intensity);
	void renderZ();

	void advanceTime(float fElapsedTime, float fDt);
	void updateShadows();
};

#endif // __SCENE_RENDERER_H__