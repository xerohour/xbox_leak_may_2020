#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Shape.h"
#include "Runner.h"
#include "Texture.h"
#include "Level.h"
#include "Mesh.h"

extern class CMeshNode* g_pRenderMeshNode;

////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NODE("Shape", CShape, CNode)

START_NODE_PROPS(CShape, CNode)
	NODE_PROP(pt_node, CShape, appearance)
	NODE_PROP(pt_node, CShape, geometry)
END_NODE_PROPS()

CShape::CShape()
{
	m_appearance = NULL;
	m_geometry = NULL;
}

CShape::~CShape()
{
	if (m_appearance != NULL)
		m_appearance->Release();

	if (m_geometry != NULL)
		m_geometry->Release();
}

void CShape::Render()
{
	if (m_geometry == NULL)
		return;

	CMeshNode* pMeshNode = NULL;
	if (m_geometry->IsKindOf(NODE_CLASS(CMeshNode)))
	{
		pMeshNode = (CMeshNode*)m_geometry;
		
		if (pMeshNode->m_bDirty && !pMeshNode->Initialize())
			return;

		DWORD fvf = pMeshNode->GetFVF();
		if (fvf == 0)
			return;

		g_pRenderMeshNode = pMeshNode;

		XAppSetVertexShader(GetFixedFunctionShader(fvf));
	}

	if (m_appearance != NULL)
		m_appearance->Render();

	m_geometry->Render();

	g_pRenderMeshNode = NULL;

	// BLOCK: Cache this objects position in world space...
	{
		D3DXMATRIX mat;
		XAppGetTransform(D3DTS_WORLD, &mat);

		m_position.x = mat.m[3][0];
		m_position.y = mat.m[3][1];
		m_position.z = mat.m[3][2];
	}
}

void CShape::GetBBox(BBox* pBBox)
{
	if (m_geometry != NULL)
		m_geometry->GetBBox(pBBox);
	else
		CNode::GetBBox(pBBox);
}

float CShape::GetRadius()
{
	if (m_geometry == NULL)
		return 0.0f;

	return m_geometry->GetRadius();
}

void CShape::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);

	if (m_geometry != NULL)
		m_geometry->Advance(nSeconds);

	if (m_appearance != NULL)
		m_appearance->Advance(nSeconds);
}

////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NODE("Appearance", CAppearance, CNode)

START_NODE_PROPS(CAppearance, CNode)
	NODE_PROP(pt_node, CAppearance, material)
	NODE_PROP(pt_node, CAppearance, texture)
END_NODE_PROPS()

CAppearance::CAppearance()
{
	m_material = NULL;
	m_texture = NULL;
}

CAppearance::~CAppearance()
{
	if (m_material != NULL)
		m_material->Release();

	if (m_texture != NULL)
		m_texture->Release();
}

void CAppearance::Render()
{
	XAppSetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	XAppSetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	XAppSetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	XAppSetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	XAppSetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	XAppSetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	XAppSetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	XAppSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	XAppSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	XAppSetTexture(0, NULL);
	XAppSetTexture(1, NULL);

#ifdef _DEBUG
	XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(255, 0, 0, 255));
#endif

	if (m_texture != NULL)
	{
		m_texture->Render();

		LPDIRECT3DTEXTURE8 pSurface = m_texture->GetTextureSurface();
		if (pSurface != NULL)
		{
			XAppSetTexture(0, pSurface);

			XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		}
		else
		{
			XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
			XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
			XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 0.0f));
		}
	}

	if (m_material != NULL)
		m_material->Render();
}

void CAppearance::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);

	if (m_material != NULL)
		m_material->Advance(nSeconds);

	if (m_texture != NULL)
		m_texture->Advance(nSeconds);
}

////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NODE("Material", CMaterial, CNode)

START_NODE_PROPS(CMaterial, CNode)
	NODE_PROP(pt_number, CMaterial, ambientIntensity)
	NODE_PROP(pt_color, CMaterial, diffuseColor)
	NODE_PROP(pt_color, CMaterial, emissiveColor)
	NODE_PROP(pt_number, CMaterial, shininess)
	NODE_PROP(pt_color, CMaterial, specularColor)
	NODE_PROP(pt_number, CMaterial, transparency)
END_NODE_PROPS()

CMaterial::CMaterial() :
	m_ambientIntensity(0.2f),
	m_diffuseColor(0.8f, 0.8f, 0.8f),
	m_emissiveColor(0.0f, 0.0f, 0.0f),
	m_shininess(0.2f),
	m_specularColor(0.0f, 0.0f, 0.0f),
	m_transparency(0.0f)
{
}

void CMaterial::Render()
{
	m_material.Diffuse.r = m_diffuseColor.x;
	m_material.Diffuse.g = m_diffuseColor.y;
	m_material.Diffuse.b = m_diffuseColor.z;
	m_material.Diffuse.a = 1.0f - m_transparency;

	m_material.Ambient.r = m_ambientIntensity;
	m_material.Ambient.g = m_ambientIntensity;
	m_material.Ambient.b = m_ambientIntensity;
	m_material.Ambient.a = 1.0f - m_transparency;

	m_material.Specular.r = m_specularColor.x;
	m_material.Specular.g = m_specularColor.y;
	m_material.Specular.b = m_specularColor.z;
	m_material.Specular.a = 1.0f - m_transparency;

	m_material.Emissive.r = m_emissiveColor.x;
	m_material.Emissive.g = m_emissiveColor.y;
	m_material.Emissive.b = m_emissiveColor.z;
	m_material.Emissive.a = 1.0f - m_transparency;

	m_material.Power = m_shininess;

	XAppSetMaterial(&m_material);
}

////////////////////////////////////////////////////////////////////////////

void CreateCube(D3DVERTEX* pVertices, WORD* pIndices, D3DXVECTOR3 size, bool bInside = false)
{
    // Define the normals for the cube
    D3DXVECTOR3 n0( 0.0f, 0.0f,-1.0f ); // Front face
    D3DXVECTOR3 n1( 0.0f, 0.0f, 1.0f ); // Back face
    D3DXVECTOR3 n2( 0.0f, 1.0f, 0.0f ); // Top face
    D3DXVECTOR3 n3( 0.0f,-1.0f, 0.0f ); // Bottom face
    D3DXVECTOR3 n4( 1.0f, 0.0f, 0.0f ); // Right face
    D3DXVECTOR3 n5(-1.0f, 0.0f, 0.0f ); // Left face

    // Set up the vertices for the cube. Note: to prevent tiling problems,
    // the u/v coords are knocked slightly inwards.

	if (bInside)
	{
		// Front face
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x, 0.5f * size.y,-0.5f * size.z), n0, 0.01f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x, 0.5f * size.y,-0.5f * size.z), n0, 0.99f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x,-0.5f * size.y,-0.5f * size.z), n0, 0.99f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x,-0.5f * size.y,-0.5f * size.z), n0, 0.01f, 0.01f);

		// Back face
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x, 0.5f * size.y, 0.5f * size.z), n1, 0.99f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x,-0.5f * size.y, 0.5f * size.z), n1, 0.99f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x,-0.5f * size.y, 0.5f * size.z), n1, 0.01f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x, 0.5f * size.y, 0.5f * size.z), n1, 0.01f, 0.99f);

		// Top face
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x, 0.5f * size.y, 0.5f * size.z), n2, 0.01f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x, 0.5f * size.y, 0.5f * size.z), n2, 0.99f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x, 0.5f * size.y,-0.5f * size.z), n2, 0.99f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x, 0.5f * size.y,-0.5f * size.z), n2, 0.01f, 0.01f);

		// Bottom face
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x,-0.5f * size.y, 0.5f * size.z), n3, 0.01f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x,-0.5f * size.y,-0.5f * size.z), n3, 0.01f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x,-0.5f * size.y,-0.5f * size.z), n3, 0.99f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x,-0.5f * size.y, 0.5f * size.z), n3, 0.99f, 0.99f);

		// Right face
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x, 0.5f * size.y,-0.5f * size.z), n4, 0.01f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x, 0.5f * size.y, 0.5f * size.z), n4, 0.99f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x,-0.5f * size.y, 0.5f * size.z), n4, 0.99f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x,-0.5f * size.y,-0.5f * size.z), n4, 0.01f, 0.01f);

		// Left face
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x, 0.5f * size.y,-0.5f * size.z), n5, 0.99f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x,-0.5f * size.y,-0.5f * size.z), n5, 0.99f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x,-0.5f * size.y, 0.5f * size.z), n5, 0.01f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x, 0.5f * size.y, 0.5f * size.z), n5, 0.01f, 0.99f);
	}
	else
	{
		// Front face
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x,-0.5f * size.y,-0.5f * size.z), n0, 0.01f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x,-0.5f * size.y,-0.5f * size.z), n0, 0.99f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x, 0.5f * size.y,-0.5f * size.z), n0, 0.99f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x, 0.5f * size.y,-0.5f * size.z), n0, 0.01f, 0.99f);

		// Back face
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x, 0.5f * size.y, 0.5f * size.z), n1, 0.01f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x,-0.5f * size.y, 0.5f * size.z), n1, 0.01f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x,-0.5f * size.y, 0.5f * size.z), n1, 0.99f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x, 0.5f * size.y, 0.5f * size.z), n1, 0.99f, 0.99f);

		// Top face
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x, 0.5f * size.y,-0.5f * size.z), n2, 0.01f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x, 0.5f * size.y,-0.5f * size.z), n2, 0.99f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x, 0.5f * size.y, 0.5f * size.z), n2, 0.99f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x, 0.5f * size.y, 0.5f * size.z), n2, 0.01f, 0.99f);

		// Bottom face
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x,-0.5f * size.y, 0.5f * size.z), n3, 0.99f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x,-0.5f * size.y,-0.5f * size.z), n3, 0.99f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x,-0.5f * size.y,-0.5f * size.z), n3, 0.01f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x,-0.5f * size.y, 0.5f * size.z), n3, 0.01f, 0.99f);

		// Right face
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x,-0.5f * size.y,-0.5f * size.z), n4, 0.01f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x,-0.5f * size.y, 0.5f * size.z), n4, 0.99f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x, 0.5f * size.y, 0.5f * size.z), n4, 0.99f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3( 0.5f * size.x, 0.5f * size.y,-0.5f * size.z), n4, 0.01f, 0.99f);

		// Left face
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x, 0.5f * size.y, 0.5f * size.z), n5, 0.01f, 0.99f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x,-0.5f * size.y, 0.5f * size.z), n5, 0.01f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x,-0.5f * size.y,-0.5f * size.z), n5, 0.99f, 0.01f);
		*pVertices++ = D3DVERTEX(D3DXVECTOR3(-0.5f * size.x, 0.5f * size.y,-0.5f * size.z), n5, 0.99f, 0.99f);
	}

    // Set up the indices for the cube
    *pIndices++ =  0+0;   *pIndices++ =  0+1;   *pIndices++ =  0+2;
    *pIndices++ =  0+2;   *pIndices++ =  0+3;   *pIndices++ =  0+0;
    *pIndices++ =  4+0;   *pIndices++ =  4+1;   *pIndices++ =  4+2;
    *pIndices++ =  4+2;   *pIndices++ =  4+3;   *pIndices++ =  4+0;
    *pIndices++ =  8+0;   *pIndices++ =  8+1;   *pIndices++ =  8+2;
    *pIndices++ =  8+2;   *pIndices++ =  8+3;   *pIndices++ =  8+0;
    *pIndices++ = 12+0;   *pIndices++ = 12+1;   *pIndices++ = 12+2;
    *pIndices++ = 12+2;   *pIndices++ = 12+3;   *pIndices++ = 12+0;
    *pIndices++ = 16+0;   *pIndices++ = 16+1;   *pIndices++ = 16+2;
    *pIndices++ = 16+2;   *pIndices++ = 16+3;   *pIndices++ = 16+0;
    *pIndices++ = 20+0;   *pIndices++ = 20+1;   *pIndices++ = 20+2;
    *pIndices++ = 20+2;   *pIndices++ = 20+3;   *pIndices++ = 20+0;
}

class CBox : public CNode
{
	DECLARE_NODE(CBox, CNode)
public:
	CBox();
	~CBox();

	void Render();
	void GetBBox(BBox* pBBox);
	float GetRadius();


	D3DXVECTOR3 m_size;

	IDirect3DVertexBuffer8* m_pVB;
	IDirect3DIndexBuffer8* m_pIB;

#define NUM_CUBE_VERTICES (4*6)
#define NUM_CUBE_INDICES  (6*6)

	D3DVERTEX m_pCubeVertices [NUM_CUBE_VERTICES];
	WORD m_pCubeIndices [NUM_CUBE_INDICES];

	bool m_bDirty;

	DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("Box", CBox, CNode)

START_NODE_PROPS(CBox, CNode)
	NODE_PROP(pt_vec3, CBox, size)
END_NODE_PROPS()

CBox::CBox() :
	m_size(1.0f, 1.0f, 1.0f)
{
	m_bDirty = true;

	m_pVB = NULL;
	m_pIB = NULL;
}

CBox::~CBox()
{
	if (m_pVB != NULL)
		m_pVB->Release();

	if (m_pIB != NULL)
		m_pIB->Release();
}

void CBox::Render()
{
	if (m_bDirty)
	{
		TRACE(_T("Creating a cube...\n"));
		CreateCube(m_pCubeVertices, m_pCubeIndices, m_size);
		m_bDirty = false;
	}

	if (m_pVB == NULL)
	{
		void* pVerts;
		XAppCreateVertexBuffer(NUM_CUBE_VERTICES * sizeof (D3DVERTEX), D3DUSAGE_DYNAMIC, D3DFVF_VERTEX, D3DPOOL_MANAGED, &m_pVB);

		VERIFYHR(m_pVB->Lock(0, NUM_CUBE_VERTICES * sizeof (D3DVERTEX), (BYTE**)&pVerts, 0));
		CopyMemory(pVerts, m_pCubeVertices, NUM_CUBE_VERTICES * sizeof (D3DVERTEX));
		VERIFYHR(m_pVB->Unlock());
	}

	if (m_pIB == NULL)
	{
		void* pIndices;
		XAppCreateIndexBuffer(NUM_CUBE_INDICES * sizeof (WORD), D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB);
		VERIFYHR(m_pIB->Lock(0, NUM_CUBE_INDICES * sizeof (WORD), (BYTE**)&pIndices, 0));
		CopyMemory(pIndices, m_pCubeIndices, NUM_CUBE_INDICES * sizeof (WORD));
		VERIFYHR(m_pIB->Unlock());
	}

	if (m_pVB != NULL && m_pIB != NULL)
	{
		XAppSetStreamSource(0, m_pVB, sizeof (D3DVERTEX));
		XAppSetIndices(m_pIB, 0);
		XAppSetVertexShader(D3DFVF_VERTEX);
	    XAppDrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 0, 12);

//		g_bResetStream = true;
	}
}

void CBox::GetBBox(BBox* pBBox)
{
	pBBox->center.x = 0.0f;
	pBBox->center.y = 0.0f;
	pBBox->center.z = 0.0f;
	pBBox->size = m_size;
}

float CBox::GetRadius()
{
	return D3DXVec3Length(&m_size) / 2.0f;
}

////////////////////////////////////////////////////////////////////////////

extern class CMesh* MakeSphere(float nRadius, int nSlices, int nStacks);

class CSphere : public CMeshNode
{
	DECLARE_NODE(CSphere, CMeshNode)
public:
	CSphere();
	~CSphere();

	float m_radius;
	int m_slices;
	int m_stacks;

	void Init();

	DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("Sphere", CSphere, CMeshNode)

START_NODE_PROPS(CSphere, CMeshNode)
	NODE_PROP(pt_number, CSphere, radius)
	NODE_PROP(pt_integer, CSphere, slices)
	NODE_PROP(pt_integer, CSphere, stacks)
END_NODE_PROPS()

CSphere::CSphere() :
	m_radius(1.0f),
	m_slices(32),
	m_stacks(32)
{
	m_bDirty = true;
}

CSphere::~CSphere()
{
}

void CSphere::Init()
{
	m_pMesh = MakeSphere(m_radius, m_slices, m_stacks);

    if (m_pMesh)
    {
        m_bDirty = false;
    }
}
