///////////////////////////////////////////////////////////////////////////////
// File: VBlob.cpp
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#include "precomp.h"
#include "xbs_math.h"
#include "xbs_app.h"
#include "qrand.h"
#include "VBlob.h"
#include "tex_gen.h"
///////////////////////////////////////////////////////////////////////////////
const float MIN_SPAWN_MAGN =		(0.5f);
#define BLOBLET_DIM			(8)
#define BLOB_DIM			(32)
///////////////////////////////////////////////////////////////////////////////
QRand VBlob::m_QRand;
VBlob* gpVBlob;
///////////////////////////////////////////////////////////////////////////////
void VBlob::Init()
{
    int i;

    for (i = 0; i < MAX_BLOBBUMPS; i++)
    {
        m_BlobBumps[i].Init();
    }

    for (i = 0; i < MAX_BLOBLETS; i++)
    {
        m_Bloblets[i].Init();
    }

    m_QRand.Init();

	m_pBlobletVB = m_pBlobVBConst = m_pBlobVBChangingR = m_pBlobVBChangingU = NULL;
	m_pBlobletIB = m_pBlobIB = NULL;
	m_pUnitSphereNormals = NULL;
    m_dwNumBlobletVertices = m_dwNumBlobletIndices = m_NumVertsPerFace = m_dwNumVertices = m_dwNumIndices = 0;
    m_dwVShaderBlob = m_dwVShaderBloblet = 0;
    m_dwPShaderBlob = m_dwPShaderBloblet = 0;
	m_NumBlobBumps = m_NumBloblets = 0;
	Set(&m_BlobColor, 0.25f, 1.0f, 0.15f, 1.0f);
	Set(&m_Pos, 0.0f, 0.0f, 0.0f);
	Set(&m_Scale, 1.0f, 1.0f, 1.0f);
	m_fRadius = 2.3f;
}
///////////////////////////////////////////////////////////////////////////////
void VBlob::create()
{
	gpVBlob = this;

	gpd3dDev->CreateVertexBuffer(4 * sizeof(float)*5, 0, 0, 0, &pHaloQuadVB);


//MTS	generateUnitSphere(2, &m_pBlobletVB, &m_pBlobletIB, NULL, &m_dwNumBlobletVertices, &m_dwNumBlobletIndices);
	generateUnitSphere(BLOBLET_DIM, &m_pBlobletVB, &m_pBlobletIB, NULL, &m_dwNumBlobletVertices, &m_dwNumBlobletIndices);

	generateUnitSphere(BLOB_DIM, &m_pBlobVBConst, &m_pBlobIB, &m_pUnitSphereNormals, &m_dwNumVertices, &m_dwNumIndices);
	gpd3dDev->CreateVertexBuffer(m_dwNumVertices * sizeof(VBlobChangingVertex), 0, 0, 0, &m_pBlobVBChangingR);
	gpd3dDev->CreateVertexBuffer(m_dwNumVertices * sizeof(VBlobChangingVertex), 0, 0, 0, &m_pBlobVBChangingU);
	m_NumVertsPerFace = m_dwNumVertices / 6;

	restart();

	// Initialize the pixel shaders.
    if( m_dwPShaderBlob )
    {
        gpd3dDev->DeletePixelShader( m_dwPShaderBlob );
        m_dwPShaderBlob = 0;
    }
#ifndef BINARY_RESOURCE
	m_dwPShaderBlob = gApp.loadPixelShader("D:\\Shaders\\VBlob.xpu");
#else // BINARY_RESOURCE
	m_dwPShaderBlob = gApp.loadPixelShader(g_vblob_xpu);
#endif // BINARY_RESOURCE

    if( m_dwPShaderBloblet )
    {
        gpd3dDev->DeletePixelShader( m_dwPShaderBloblet );
        m_dwPShaderBloblet = 0;
    }
#ifndef BINARY_RESOURCE
	m_dwPShaderBloblet = gApp.loadPixelShader("D:\\Shaders\\VBloblet.xpu");
#else // BINARY_RESOURCE
	m_dwPShaderBloblet = gApp.loadPixelShader(g_vbloblet_xpu);
#endif // BINARY_RESOURCE

	// Initialize the vertex shaders.
    DWORD dwBlobShaderVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),    // unit sphere normal
        D3DVSD_STREAM( 1 ),
        D3DVSD_REG( 1, D3DVSDT_FLOAT4 ),    // vertex normal, w is displacement
        D3DVSD_END()
    };
	if (m_dwVShaderBlob)
	{
		gpd3dDev->DeleteVertexShader( m_dwVShaderBlob );
		m_dwVShaderBlob = 0;
	}
#ifndef BINARY_RESOURCE
	m_dwVShaderBlob    = gApp.loadVertexShader("D:\\Shaders\\VBlob.xvu",    dwBlobShaderVertexDecl);
#else // BINARY_RESOURCE
	m_dwVShaderBlob    = gApp.loadVertexShader(g_vblob_xvu,    dwBlobShaderVertexDecl);
#endif // BINARY_RESOURCE

    DWORD dwBlobletShaderVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),    // unit sphere normal
        D3DVSD_END()
    };
	if (m_dwVShaderBloblet)
	{
		gpd3dDev->DeleteVertexShader( m_dwVShaderBloblet );
		m_dwVShaderBloblet = 0;
	}
#ifndef BINARY_RESOURCE
	m_dwVShaderBloblet = gApp.loadVertexShader("D:\\Shaders\\VBloblet.xvu", dwBlobletShaderVertexDecl);
#else // BINARY_RESOURCE
	m_dwVShaderBloblet = gApp.loadVertexShader(g_vbloblet_xvu, dwBlobletShaderVertexDecl);
#endif // BINARY_RESOURCE
}
///////////////////////////////////////////////////////////////////////////////
void VBlob::destroy()
{
	m_pBlobletVB->Release();
	m_pBlobletIB->Release();
	m_pBlobVBConst->Release();
	m_pBlobVBChangingR->Release();
	m_pBlobVBChangingU->Release();
	m_pBlobIB->Release();
	MemFree(m_pUnitSphereNormals);

	pHaloQuadVB->Release();

    if (m_dwPShaderBlob) gpd3dDev->DeletePixelShader(m_dwPShaderBlob);
    if (m_dwPShaderBloblet) gpd3dDev->DeletePixelShader(m_dwPShaderBloblet);
	if (m_dwVShaderBlob) gpd3dDev->DeleteVertexShader(m_dwVShaderBlob);
	if (m_dwVShaderBloblet)	gpd3dDev->DeleteVertexShader(m_dwVShaderBloblet);

	m_pBlobletVB = NULL;
	m_pBlobletIB = NULL;
	m_pBlobVBConst = NULL;
	m_pBlobVBChangingU = m_pBlobVBChangingR = NULL;
	m_pBlobIB = NULL;
	m_pUnitSphereNormals = NULL;
}
///////////////////////////////////////////////////////////////////////////////
void VBlob::render()
{
    // Set default states
    gpd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );
    gpd3dDev->SetRenderState( D3DRS_ZENABLE,  TRUE );

    gpd3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);

    gpd3dDev->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	gpd3dDev->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	gpd3dDev->SetTextureStageState( 1, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);

    gpd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    gpd3dDev->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    gpd3dDev->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );

		// Render halo after blob, but before bloblets.
	gpd3dDev->SetVertexShader(D3DFVF_XYZ|D3DFVF_TEX1);
	gpd3dDev->SetPixelShader(NULL);

	D3DMATRIX id_mat;
	SetIdentity(&id_mat);
	gpd3dDev->SetTransform(D3DTS_WORLD,&id_mat);
	gpd3dDev->SetTransform(D3DTS_VIEW,&gApp.theCamera.matWTC);
	gpd3dDev->SetTransform(D3DTS_PROJECTION,&gApp.theCamera.matProj);

	gpd3dDev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	gpd3dDev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	gpd3dDev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
	gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	gpd3dDev->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
	gpd3dDev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);

	float cur_rad = m_fRadius * (1.0f + 1.3f * fast_sqrt(gApp.getPulseIntensity()));

	DWORD dw_alpha;
	float f_alpha = min(gApp.getBlobIntensity(),1.f) * 255.f;
	
	__asm
    {
        cvttss2si eax, f_alpha
        shl eax,24
		mov dw_alpha,eax
    };

	DWORD dw_tf = 0x00a0ff40 | dw_alpha;
	
	gpd3dDev->SetRenderState( D3DRS_TEXTUREFACTOR,dw_tf);
	gpd3dDev->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    gpd3dDev->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_ONE );
	gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	gpd3dDev->SetTexture(0,gApp.pGlowMap);
	
	struct _decalvert
	{
		D3DVECTOR p;
		float u,v;
	};

	_decalvert * phalo_verts = NULL;
	pHaloQuadVB->Lock(0,sizeof(_decalvert) * 4, (BYTE **)&phalo_verts,NULL);

	float f_rad = cur_rad * 5.2f;

	D3DMATRIX & ctw = gApp.theCamera.matCTW;
	Sub(*(D3DVECTOR *)&ctw._21,*(D3DVECTOR *)&ctw._11,&phalo_verts[0].p);
	Scale(&phalo_verts[0].p,f_rad);
	phalo_verts[0].u = 0.f;
	phalo_verts[0].v = 1.f;

	Add(*(D3DVECTOR *)&ctw._21,*(D3DVECTOR *)&ctw._11,&phalo_verts[1].p);
	Scale(&phalo_verts[1].p,f_rad);
	phalo_verts[1].u = 1.f;
	phalo_verts[1].v = 1.f;

	Sub(*(D3DVECTOR *)&ctw._11,*(D3DVECTOR *)&ctw._21,&phalo_verts[2].p);
	Scale(&phalo_verts[2].p,f_rad);
	phalo_verts[2].u = 1.f;
	phalo_verts[2].v = 0.f;

	Set(&phalo_verts[3].p,-ctw._11-ctw._21,-ctw._12-ctw._22,-ctw._13-ctw._23);
	Scale(&phalo_verts[3].p,f_rad);
	phalo_verts[3].u = 0.f;
	phalo_verts[3].v = 0.f;

	pHaloQuadVB->Unlock();
	
	gpd3dDev->SetStreamSource( 0, pHaloQuadVB, sizeof(_decalvert));
	gpd3dDev->DrawPrimitive(D3DPT_TRIANGLEFAN,0,2);

	// Restore state
	gpd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

//MTS    gpd3dDev->SetRenderState( D3DRS_FILLMODE,         D3DFILL_WIREFRAME );

    gpd3dDev->SetTexture( 0, gApp.pNormalCubeMapLoRes );
    gpd3dDev->SetTexture( 1, gApp.pNormalCubeMapLoRes );


    gpd3dDev->SetVertexShader( m_dwVShaderBlob );
    gpd3dDev->SetPixelShader(  m_dwPShaderBlob );



    // Note: when passing matrices to a vertex shader, we transpose them, since
    // matrix multiplies are done with dot product operations on the matrix rows.
	D3DMATRIX matFinal,matTmp;
	MulMats(gApp.theCamera.matWTC,gApp.theCamera.matProj,&matTmp);
	SetTranspose(matTmp,&matFinal);
	gpd3dDev->SetVertexShaderConstant(4,(CONST void *)&matFinal,4);




	//; Expected vertex shaders constants
	//;    c0-c3    = Transpose of world matrix
	//;    c4-c7    = Transpose of view*projection matrix
	//;    c8       = some constants, x=0, y=1, z=2, w=0.5
	//;    c9       = eye location
	//;    c10      = blob scaling
	//;    c11      = 1/ blob scaling
	//;    c12      = blob center
	D3DVECTOR4 val[5];


	// Constants
	Set(&val[0], 0.0f, 1.0f, 2.0f, 0.5f);

	// Eye position
	gApp.theCamera.getCameraPos(&val[1]);

	// Scaling
	Set(&val[2], cur_rad*m_Scale.x, cur_rad*m_Scale.y, cur_rad*m_Scale.z, 1.0f);


	// 1 / blob scaling
	val[3].x = 1.0f / val[2].x;
	val[3].y = 1.0f / val[2].y;
	val[3].z = 1.0f / val[2].z;
	
	// Blob center.
//MTS	float f_jitter_mag = (gApp.getElapsedTime() - BLOB_JITTER_START) * OO_BLOB_JITTER_DELTA;
//MTS	f_jitter_mag *= f_jitter_mag;
//MTS	float cOffsets[3] = { 0.2f, 0.7f, 4.2f };
//MTS	float cScales[3]  = { 20.0f, 28.0f, 44.0f };
	D3DVECTOR4 cur_pos;
//MTS	cur_pos.x = m_Pos.x + f_jitter_mag * sinf((gApp.getElapsedTime() - cOffsets[0])*cScales[0]);
//MTS	cur_pos.y = m_Pos.y + f_jitter_mag * sinf((gApp.getElapsedTime() - cOffsets[1])*cScales[1]);
//MTS	cur_pos.z = m_Pos.z + f_jitter_mag * sinf((gApp.getElapsedTime() - cOffsets[2])*cScales[2]);
	cur_pos.x = m_Pos.x;
	cur_pos.y = m_Pos.y;
	cur_pos.z = m_Pos.z;
	cur_pos.w = 0.0f;
	Set(&val[4], cur_pos.x, cur_pos.y, cur_pos.z, 0.0f);


	gpd3dDev->SetVertexShaderConstant( 8, &val, 5 );


	// Make the object to World transform. Transpose it.
    D3DMATRIX matT, mat;

	SetIdentity( &mat );
	mat.m[0][0] = m_Scale.x * cur_rad;
	mat.m[1][1] = m_Scale.y * cur_rad;
	mat.m[2][2] = m_Scale.z * cur_rad;
	mat.m[3][0] = cur_pos.x;
	mat.m[3][1] = cur_pos.y;
	mat.m[3][2] = cur_pos.z;

	SetTranspose( mat, &matT );
    gpd3dDev->SetVertexShaderConstant( 0, &matT, 4 );


	// Expected pixel shader constants
	//; c0          = base blob color
	//; c1          = ambient color

	// Blob color
	val[0] = m_BlobColor;
	float f_color_intensity = BLOB_BASE_INTENSITY + 4.0f * (1.2f * gApp.getBaseBlobIntensity() + 0.8f * gApp.getPulseIntensity() );
//MTS	f_color_intensity *= min(1.0f, gApp.getElapsedTime() * OO_BLOB_STATIC_END_TIME);
	f_color_intensity *= min(1.0f, gApp.getElapsedTime() * 4.0f);
	Scale(&val[0], f_color_intensity);

	// Ambient light
	val[1] = m_BlobColor;
	Scale(&val[1], 0.0f);


	gpd3dDev->SetPixelShaderConstant( 0, &val[0], 2 );



	gpd3dDev->SetIndices( m_pBlobIB, 0 );
    gpd3dDev->SetStreamSource( 0, m_pBlobVBConst,    sizeof(VBlobConstantVertex) );

	// Get the Update vertex buffer and use it to render.
	swapChangingVertices();
    gpd3dDev->SetStreamSource( 1, m_pBlobVBChangingR, sizeof(VBlobChangingVertex) );

    gpd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, m_dwNumVertices,  0, m_dwNumIndices-2 );




	// Render the bloblets.

    gpd3dDev->SetVertexShader( m_dwVShaderBloblet );
    gpd3dDev->SetPixelShader(  m_dwPShaderBloblet );

	gpd3dDev->SetVertexShaderConstant(4,(CONST void *)&matFinal,4);

	// Constants
	Set(&val[0], 0.0f, 1.0f, 2.0f, 0.5f);

	// Eye position
	gApp.theCamera.getCameraPos(&val[1]);

	gpd3dDev->SetVertexShaderConstant( 8, &val, 2 );


	// Bloblet color
	val[0] = m_BlobColor;
	Scale(&val[0], 0.3f * gApp.getBlobIntensity());

	// Ambient light
	val[1] = m_BlobColor;
	Scale(&val[1], 0.2f);


	Set(&val[2], 2.0f, 2.0f, 2.0f, 2.0f);
	// 1/val is the level of brightness that is full alpha

	gpd3dDev->SetPixelShaderConstant( 0, &val, 3 );



	gpd3dDev->SetIndices( m_pBlobletIB, 0 );
    gpd3dDev->SetStreamSource( 0, m_pBlobletVB,    sizeof(VBlobConstantVertex) );


	for (int i=0; i<m_NumBloblets; i++)
	{
		//;    c9       = eye location
		//;    c10      = blob center
		//;    c11      = direction of scaling
		//;    c12      = scaling perpendicular to direction
		//;    c13      = parallel minus perpendicular scaling multiplied by scaling direction

		VBloblet* p_bloblet = &m_Bloblets[i];

		// Bloblet center.
		Set(&val[0], p_bloblet->vPosition.x, p_bloblet->vPosition.y, p_bloblet->vPosition.z, 0.0f);

		// Direction of scaling.
		Set(&val[1], p_bloblet->vDirection.x, p_bloblet->vDirection.y, p_bloblet->vDirection.z, 1.0f);

		// Scaling perpendicular to direction
		float fval = p_bloblet->fRadius / fast_sqrt(p_bloblet->fWobble);
		Set(&val[2], fval, fval, fval, 1.0f);

		// Parallel minus perpendicular scaling multiplied by scaling direction
		fval = p_bloblet->fRadius * p_bloblet->fWobble - fval;
		Set(&val[3], fval*p_bloblet->vDirection.x, fval*p_bloblet->vDirection.y, fval*p_bloblet->vDirection.z, 1.0f);

		
		gpd3dDev->SetVertexShaderConstant( 10, &val, 4 );


		gpd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, m_dwNumBlobletVertices,  0, m_dwNumBlobletIndices-2 );
	}

    // Restore the state
    gpd3dDev->SetPixelShader(  NULL );
    gpd3dDev->SetVertexShader( NULL );

	gpd3dDev->SetTexture(0, NULL);
	gpd3dDev->SetTexture(1, NULL);

	gpd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}
///////////////////////////////////////////////////////////////////////////////
void VBlob::zeroChangingVertices()
{
	VBlobChangingVertex* p_verts;
	const D3DVECTOR* p_us_normals = m_pUnitSphereNormals;
	m_pBlobVBChangingU->Lock(0, 0, (BYTE**)&p_verts, 0);	// D3DLOCK_DISCARD not on XBox!

	for (int face=0; face<6; face++)
	{
		for (int i=0; i<m_NumVertsPerFace; i++, p_verts++, p_us_normals++)
		{
			p_verts->normal.x = p_us_normals->x;
			p_verts->normal.y = p_us_normals->y;
			p_verts->normal.z = p_us_normals->z;
			p_verts->normal.w = 1.0f;
		}
	}

	m_pBlobVBChangingU->Unlock();
}
///////////////////////////////////////////////////////////////////////////////
void VBlob::prepareChangingVertices()
{
	VBlobChangingVertex* p_verts;
	const D3DVECTOR* p_us_normals = m_pUnitSphereNormals;
	m_pBlobVBChangingU->Lock(0, 0, (BYTE**)&p_verts, 0);	// D3DLOCK_DISCARD not on XBox!


/*
	// For spiky blobs mark 2.
	const float cA = 1.0f;
	const float cB = 0.1f;
	const float cD = 1.0f / ( (1.0f/cB) - (1.0f / (cA+cB)) );
	const float cE = cA / cD;
	const float cF = cB / cD;
	const float cG = - cD / (cA+cB);

	float sd_frac = max(1.0f, gApp.getElapsedTime() / 6.0f);
	float bd_frac = 1.0f - sd_frac;
*/


	for (int face=0; face<6; face++)
	{
		const VBlobBump* vp_blobs_of_interest[MAX_BLOBBUMPS];
		int i;
		int num_boi = 0;
		for (i=0; i<m_NumBlobBumps; i++)
		{
			if (m_BlobBumps[i].facesOfInterest & (1<<face))
			{
				vp_blobs_of_interest[num_boi++] = &m_BlobBumps[i];
			}
		}

		for (int i=0; i<m_NumVertsPerFace; i++, p_verts++)
		{
			const D3DVECTOR& us_normal = *(p_us_normals++);
			D3DVECTOR4& accum_normal = p_verts->normal;
//MTS			D3DVECTOR4 accum_normal;
			Set(&accum_normal, us_normal.x, us_normal.y, us_normal.z, 0.0f);
			for (int j=num_boi-1; j>=0; j--)
			{
				const VBlobBump* p_blob = vp_blobs_of_interest[j];
				D3DVECTOR delta;
				Sub(us_normal, p_blob->vPosition, &delta);
//MTS				float dist2 = Length2(delta);
				float dist2 = delta.x*delta.x + delta.y*delta.y + delta.z*delta.z;
//MTS				float dist2 = Distance2(us_normal, p_blob->vPosition);
				if (dist2 < p_blob->fRadius2)
				{
//MTS					if ((p_blob->facesOfInterest & (1<<face)) == 0)
//MTS					{
//MTS						int a = 0;
//MTS					}

					float dist2_mo = dist2 * p_blob->fOORadius2 - 1.0f;


					float displacement = m_fRadius*p_blob->fMagnitude * dist2_mo*dist2_mo;
/*
					// Spiky blob bumps mark 2.
					float bumpy_displacement = m_fRadius*p_blob->fMagnitude * dist2_mo*dist2_mo;
					float spiky_displacement = m_fRadius * p_blob->fMagnitude * ( 1.0f / (cE*dist2*p_blob->fOORadius2 + cF) + cG );
					spiky_displacement *= 1.3f;

					float displacement = bd_frac*bumpy_displacement + sd_frac*spiky_displacement;
*/

					float perturb_amount = -4.0f * p_blob->fMagnitude * p_blob->fOORadius2 * dist2_mo;


					// lnorm = us_normal + perturb_amount*(us_normal - p_blob->vPosition)
					// This should make it faster, but VTune suggests it doesn't. Maybe in Release...
//MTS					D3DVECTOR lnorm;
//MTS					float oppa = 1.0f + perturb_amount;
//MTS					lnorm.x = us_normal.x * oppa - perturb_amount * p_blob->vPosition.x;
//MTS					lnorm.y = us_normal.y * oppa - perturb_amount * p_blob->vPosition.y;
//MTS					lnorm.z = us_normal.z * oppa - perturb_amount * p_blob->vPosition.z;

					D3DVECTOR lnorm = us_normal;
//MTS					Set(&lnorm, 0.0f, 0.0f, 0.0f);

//MTS					D3DVECTOR delta;
//MTS					Sub(us_normal, p_blob->vPosition, &delta);
					AddScaled(&lnorm, delta, perturb_amount);
					QuickNormalize(&lnorm);

					accum_normal.x += lnorm.x;
					accum_normal.y += lnorm.y;
					accum_normal.z += lnorm.z;
					accum_normal.w += displacement;
				}
			}
			p_verts->normal = accum_normal;
		}
	}

	m_pBlobVBChangingU->Unlock();
}
///////////////////////////////////////////////////////////////////////////////
void VBlob::advanceTime(float fElapsedTime, float fDt)
{
	if (fElapsedTime < BLOB_STATIC_END_TIME)
	{
		if (fElapsedTime - fDt > BLOB_STATIC_END_TIME)
		{
			zeroChangingVertices();
			swapChangingVertices();
			zeroChangingVertices();
		}
		return;
	}

	for (int i=0; i<m_NumBloblets; i++)
	{
		if (!m_Bloblets[i].update(fElapsedTime, fDt))
		{
//MTS			m_Bloblets[i] = m_Bloblets[--m_NumBloblets];
//MTS			i--;
			// better notify the bump that owned it...
		}
	}

	for (int i=0; i<m_NumBlobBumps; i++)
	{
		if (m_BlobBumps[i].update(
				fElapsedTime,
				fDt,
				(m_NumBloblets<MAX_BLOBLETS) ? &m_Bloblets[m_NumBloblets] : NULL)
				)
		{
			m_NumBloblets++;
		}
	}

//MTS	m_pBlobVBChangingU->BlockUntilNotBusy();	// this causes it to block here, rather than in prepareChangingVertices
	prepareChangingVertices();
}
///////////////////////////////////////////////////////////////////////////////
void VBlob::restart()
{
	m_NumBloblets = 0;
	m_NumBlobBumps = 0;

	while (m_NumBlobBumps<MAX_BLOBBUMPS)
	{
		if (m_BlobBumps[m_NumBlobBumps++].create(
			-0.3f,
			(m_NumBloblets < MAX_BLOBLETS) ? &m_Bloblets[m_NumBloblets] : NULL
			))	// pass negative time so the blobs get a head start
		{
			m_NumBloblets++;
		}
	}

	zeroChangingVertices();
	swapChangingVertices();
	zeroChangingVertices();
}
///////////////////////////////////////////////////////////////////////////////
void VBlob::getLightForPosition(D3DVECTOR* p_light_pos, float* p_intensity, D3DVECTOR position)
{
	float total_weights = 0.0f;
	D3DVECTOR av_pos;
	float av_intensity = 0.0f;
	Set(&av_pos, 0.0f, 0.0f, 0.0f);

	float light_intensity = gApp.getBlobIntensity();

	{
		float dist2 = Distance2(position, m_Pos);
		float dist4 = dist2 * dist2;
		float weight = 1.0f / dist2;

		av_intensity += 4.0f * light_intensity * weight;
		AddScaled(&av_pos, m_Pos, weight);
		total_weights += weight;
	}

	for (int i=0; i<m_NumBloblets; i++)
	{
		float dist2 = Distance2(position, m_Bloblets[i].vPosition);
		float dist4 = dist2 * dist2;
		float weight = 1.0f / dist2;

		av_intensity += light_intensity * weight;
		AddScaled(&av_pos, m_Bloblets[i].vPosition, weight);
		total_weights += weight;
	}

	float oo_total_weights = 1.0f / total_weights;
	Set(p_light_pos, oo_total_weights * av_pos.x, oo_total_weights * av_pos.y, oo_total_weights * av_pos.z);
	*p_intensity = oo_total_weights * av_intensity;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool VBlob::generateUnitSphere(	int resolution,
								LPDIRECT3DVERTEXBUFFER8* pp_vb,
								LPDIRECT3DINDEXBUFFER8* pp_ib,
								D3DVECTOR** pp_us,
								DWORD* num_verts, DWORD* num_indices)
{
	int subdiv = max(1, resolution / 2);
	float f_division_step = 2.0f / ((float)subdiv);

	*num_verts = 6*(subdiv+1)*(subdiv+1);
	*num_indices = 6*GetNumberOfIndicesForTristripMesh(subdiv,subdiv) + 2*5;	// 5 double-taps


	gpd3dDev->CreateVertexBuffer( *num_verts * sizeof(VBlobConstantVertex), 0, 0, 0, pp_vb);
	gpd3dDev->CreateIndexBuffer(  *num_indices * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, pp_ib);
	if (pp_us) *pp_us = (D3DVECTOR*)MemAlloc(sizeof(D3DVECTOR)*(*num_verts));


	VBlobConstantVertex* p_verts;
	WORD* p_indices;

	(*pp_vb)->Lock(0, 0, (BYTE**)&p_verts, 0);
	(*pp_ib)->Lock(0, 0, (BYTE**)&p_indices, 0);
	D3DVECTOR* p_us = ((pp_us) ? *pp_us : NULL);



	// Create vertices.
	VBlobConstantVertex* p_vert = &p_verts[0];
	for (int k=0; k<6; k++)
	{
		for (int j=0; j<=subdiv; j++)
		{
			for (int i=0; i<=subdiv; i++)
			{
				D3DVECTOR& pos = p_vert->unit_sphere_normal;

				float fu = ((i==subdiv) ? +1.0f : (-1.0f + f_division_step * ((float)i)));
				float fv = ((j==subdiv) ? +1.0f : (-1.0f + f_division_step * ((float)j)));
				switch(k)
				{
					case 0: Set(&pos, -1.0f, -fu, +fv); break;
					case 1: Set(&pos, +fv, -1.0f, -fu); break;
					case 2: Set(&pos, -fu, +fv, -1.0f); break;
					case 3: Set(&pos, +1.0f, +fu, +fv); break;
					case 4: Set(&pos, +fv, +1.0f, +fu); break;
					case 5: Set(&pos, +fu, +fv, +1.0f); break;
				}

				Normalize(&pos);
				if (p_us)
				{
					*p_us = pos;
					p_us++;
				}
				p_vert++;
			}
		}
	}


	// Create indices.
	int j = 0;
	for (int i=0; i<6; i++)
	{
		j += CreateTristripForMesh(&p_indices[j], subdiv, subdiv, (i>0), (i<5), i*(subdiv+1)*(subdiv+1));
	}

	(*pp_ib)->Unlock();
	(*pp_vb)->Unlock();

	return true;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void VBlobBump::Init()
{
	pMyBloblet = NULL;
}
///////////////////////////////////////////////////////////////////////////////
void VBlobBump::UnInit()
{
}
///////////////////////////////////////////////////////////////////////////////
// randomly creates a blob bump
bool VBlobBump::create(float cur_time, VBloblet* p_bloblet)
{
	if (cur_time < 0.0f) pMyBloblet = NULL;

	vDirection.x = VBlob::fRand11();
	vDirection.y = VBlob::fRand11();
	vDirection.z = VBlob::fRand11();


	if (Length2(vDirection) < 0.001f)
	{
		vDirection.x = VBlob::fRand11();
		vDirection.y = VBlob::fRand11();
		vDirection.z = 1.0f;
	}

	Set(&vPosition, 0.0f, 0.0f, 0.0f);

	QuickNormalize(&vDirection);

	float time_prog = max(0.0f, (cur_time-BLOB_STATIC_END_TIME) * OO_MAX_INTENSITY_DELTA);
	float rad_mag_rand = VBlob::fRand01();

	fRadius = rad_mag_rand * 0.4f + 0.4f;
	fRadius2 = fRadius*fRadius;
	fOORadius2 = 1.0f / fRadius2;
	fMagnitude = 0.0f;

	recalculateFacesOfInterest();


	fStartTime = cur_time + 0.4f * VBlob::fRand01();

	fMaxMagnitude = (1.0f - rad_mag_rand) * 0.5f + 0.2f;
	fMaxMagnitude *= 0.5f + 0.5f * time_prog;

	if (!pMyBloblet) pMyBloblet = p_bloblet;
	if (pMyBloblet)
	{
		// Paired with a bloblet.
		float f_main_rad = gpVBlob->getRadius();
		pMyBloblet->fRadius = (VBlob::fRand01()+1.0f) * 0.25f * f_main_rad * fRadius;
		pMyBloblet->vDirection = vDirection;

		pMyBloblet->fMaxDist = f_main_rad * (5.0f + VBlob::fRand11() * 2.0f);
		pMyBloblet->fMaxDist *= 0.6f;

		pMyBloblet->fStartTime = (cur_time<-1.0f) ? -VBlob::fRand01() * 0.3f : cur_time;

		float period = 0.8f + 0.3f * VBlob::fRand01();
		period *= 1.0f / 0.6f;
		pMyBloblet->fTimeMultiple = 2.0f * Pi / period;

		pMyBloblet->fWobble = 1.2f;
		pMyBloblet->fWobbleDirection = 0.0f;

		bStillAttachedToBloblet = (cur_time-fStartTime < 0.4f*period);


		// Set my properties from the bloblet.
		pMyBloblet->update(cur_time, 0.0f);
		update(cur_time, 0.0f, NULL);
	}
	else
	{
		// Not paired with a bloblet.
		float sequence_len = fMaxMagnitude * 0.3f + VBlob::fRand01() * 0.3f;
		fTimeMul = Pi / sequence_len;
		fTimeMul *= time_prog*0.2f + 0.8f;

		if (cur_time < -1.0f) fStartTime = -VBlob::fRand01() * Pi / fTimeMul;
	}

	return (pMyBloblet!=NULL);
}
///////////////////////////////////////////////////////////////////////////////
// returns true if it initialized a new Bloblet
bool VBlobBump::update(float elapsed_time, float dt, VBloblet* p_bloblet)
{
	if (pMyBloblet)
	{
		float f_b_mag = (fast_fabs(pMyBloblet->fCurDist) + pMyBloblet->fRadius) / gpVBlob->getRadius();
		fMagnitude = min( 2.0f, max(0.0f, f_b_mag - 1.0f) );
		if (bStillAttachedToBloblet)
		{
			if (fMagnitude > 0.8f)
			{
				// Lost attachment
				bStillAttachedToBloblet = false;
//MTS				fMaxMagnitude = max(0.0f, fMagnitude - 2.0f * pMyBloblet->fRadius / gpVBlob->getRadius());
				fMaxMagnitude = fMagnitude;
				float sequence_len = 0.3f * fMagnitude;
				fTimeMul = 2.0f * Pi / sequence_len;
				fStartTime = elapsed_time - 0.25f * sequence_len;
				pMyBloblet->fWobble = max(0.6f, min(0.8f, fMagnitude-0.5f));	// more wobble when there is less energy
				pMyBloblet->fWobbleDirection = 0.0f;
			}
			else
			{
				// Still attached.
				if ( (Dot(vDirection, pMyBloblet->vDirection) < 0.0f) != pMyBloblet->bFarSide)
				{
					// Must reverse direction.
					Scale(&vDirection, -1.0f);
					vPosition = vDirection;
					recalculateFacesOfInterest();
				}
				return false;
			}
		}

		// May have just lost attachment.
		if (!bStillAttachedToBloblet)
		{
			if (f_b_mag < 0.9f)
			{
				// Blob is inside sphere, reattach.
				bStillAttachedToBloblet = true;
			}
		}
	}

	float t = (elapsed_time - fStartTime) * fTimeMul;
	if (t>Pi)
	{
		if (pMyBloblet==NULL)
		{
			return create(elapsed_time, p_bloblet);
		}
		fMagnitude = 0.0f;
		return false;
	}
	if (t<0.0f) return false;

	float sin_val = fast_sin(t);
	fMagnitude = fMaxMagnitude * sin_val;
//MTS	vPosition.x = vDirection.x * sin_val;
//MTS	vPosition.y = vDirection.y * sin_val;
//MTS	vPosition.z = vDirection.z * sin_val;
	vPosition = vDirection;
	recalculateFacesOfInterest();

//MTS	if ((bNotYetSpawned) && (t > 0.5f * Pi) && (p_bloblet))
//MTS	{
//MTS		bNotYetSpawned = false;
//MTS
//MTS		float f_main_rad = gpVBlob->getRadius();
//MTS		p_bloblet->fRadius = 0.7f * f_main_rad * fRadius;
//MTS
//MTS		float displ = f_main_rad * (1.0f + fMagnitude) - p_bloblet->fRadius;
//MTS		p_bloblet->vPosition = gpVBlob->getCenter();
//MTS		p_bloblet->vPosition.x += vDirection.x * displ;
//MTS		p_bloblet->vPosition.y += vDirection.y * displ;
//MTS		p_bloblet->vPosition.z += vDirection.z * displ;
//MTS
//MTS
//MTS
//MTS
//MTS
//MTS		float vel = f_main_rad * (fMagnitude-MIN_SPAWN_MAGN) * 100.0f + 20.0f;
//MTS		p_bloblet->vVelocity.x = vDirection.x * vel;
//MTS		p_bloblet->vVelocity.y = vDirection.y * vel;
//MTS		p_bloblet->vVelocity.z = vDirection.z * vel;
//MTS
//MTS		p_bloblet->vAttachedTo = gpVBlob->getCenter();
//MTS
//MTS
//MTS		// Set up a spring as per 2001.05.01 of my notes.
//MTS
//MTS		// Use a spring model for now. Period is 2*Pi/sqrt(k/m), so k/m = (2*Pi/period)^2
//MTS		float sequence_time_mul = 5.0f;
//MTS		p_bloblet->fRestoreForceConstant = 2.0f * Pi / (sequence_time_mul*(elapsed_time - fStartTime));
//MTS		p_bloblet->fRestoreForceConstant *= p_bloblet->fRestoreForceConstant;
//MTS
//MTS		p_bloblet->fEndDist2 = f_main_rad - p_bloblet->fRadius;
//MTS		p_bloblet->fEndDist2 *= p_bloblet->fEndDist2;
//MTS
//MTS		return true;
//MTS	}

	return false;
}
///////////////////////////////////////////////////////////////////////////////
void VBlobBump::recalculateFacesOfInterest()
{
	// Bitfield indicating the faces (0,1,2,4,5) <-> (-x,-y,-z,+x,+y,+z) the blob points most be checked with.
	facesOfInterest =	((vDirection.x - fRadius < -0.57735f) ? 0x0001 : 0) +
						((vDirection.y - fRadius < -0.57735f) ? 0x0002 : 0) +
						((vDirection.z - fRadius < -0.57735f) ? 0x0004 : 0) +
						((vDirection.x + fRadius > +0.57735f) ? 0x0008 : 0) +
						((vDirection.y + fRadius > +0.57735f) ? 0x0010 : 0) +
						((vDirection.z + fRadius > +0.57735f) ? 0x0020 : 0) ;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const float WOBBLE_ACCEL =		(1000.0f);
// returns false if it should be deleted
bool VBloblet::update(float elapsed_time, float dt)
{
	fWobble = min(2.0f, max(0.5f, fWobble + fWobbleDirection * dt));
	if (fWobbleDirection > 0.0f)
	{
		if ((fWobble < 0.95f) || (fWobble > 1.0f))
		{
			fWobbleDirection -= (fWobble-1.0f) * dt * WOBBLE_ACCEL;
		}
	}
	else
	{
		if ((fWobble < 1.0f) || (fWobble > 1.05f))
		{
			fWobbleDirection -= (fWobble-1.0f) * dt * WOBBLE_ACCEL;
		}
	}



	float time_prog = max(0.0f, (elapsed_time-BLOB_STATIC_END_TIME) * OO_MAX_INTENSITY_DELTA);

	float t = fTimeMultiple * (elapsed_time - fStartTime);
	t *= 1.4f * (1.0f + elapsed_time / 10.0f);	// speed up over time

	float s = fast_sin(t);

	float sm = fast_fabs(s);
	sm = 1.0f - (1.0f-sm)*fast_sqrt(1.0f-sm);
	s = (s>0.0f) ? sm : -sm;

	fCurDist = fMaxDist * s * time_prog;
	bFarSide = (fCurDist<0.0f);

	vPosition = gpVBlob->getCenter();
	AddScaled(&vPosition, vDirection, fCurDist);

	return (fast_fabs(fCurDist) + fRadius < gpVBlob->getRadius() * 0.5f);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
