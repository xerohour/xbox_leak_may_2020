#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "NavInfo.h"
#include "Viewpoint.h"
#include "Lerper.h"

extern D3DXMATRIX g_matView;
extern D3DXMATRIX g_matPosition;
extern D3DXMATRIX g_matProjection;
extern D3DXMATRIX g_matIdentity;

class CLayer : public CGroup
{
	DECLARE_NODE(CLayer, CGroup)
public:
	CLayer();
	~CLayer();

	void Render();
	void Advance(float nSeconds);
	bool OnSetProperty(const PRD* pprd, const void* pvValue);

	CNode* m_viewpoint;
	CNode* m_navigationInfo;

	float m_fade;
	float m_transparency;

protected:
	LPDIRECT3DVERTEXBUFFER8 m_pVB;
	float m_alpha;

	DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("Layer", CLayer, CGroup)

START_NODE_PROPS(CLayer, CGroup)
	NODE_PROP(pt_node, CLayer, viewpoint)
	NODE_PROP(pt_node, CLayer, navigationInfo)
	NODE_PROP(pt_number, CLayer, fade)
	NODE_PROP(pt_number, CLayer, transparency)
END_NODE_PROPS()


CLayer::CLayer() :
	m_viewpoint(NULL),
	m_navigationInfo(NULL),
	m_fade(0.0f),
	m_transparency(0.0f)
{
	m_pVB = NULL;
}

CLayer::~CLayer()
{
	if (m_viewpoint != NULL)
		m_viewpoint->Release();

	if (m_navigationInfo != NULL)
		m_navigationInfo->Release();

	if (m_pVB != NULL)
		m_pVB->Release();
}

struct COLORVERTEX
{
	float dvX, dvY, dvZ;
	DWORD color;
};

bool CLayer::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_transparency))
	{
		CLerper::RemoveObject(this);
		if (m_fade > 0.0f)
			new CLerper(this, &m_alpha, 1.0f - *(float*)pvValue, m_fade);
		else
			m_alpha = 1.0f - *(float*)pvValue;
	}

	return CGroup::OnSetProperty(pprd, pvValue);
}

void CLayer::Render()
{
	D3DXMATRIX matProjection, matProjectionSave, matWorldSave, matViewSave;

	if (m_viewpoint == NULL)
	{
		TRACE(_T("\001Layer is missing a viewpoint!\n"));
		return;
	}

	if (m_alpha == 0.0f)
		return;

	{
		if (m_pVB == NULL)
		{
			VERIFYHR(XAppGetD3DDev()->CreateVertexBuffer(4 * sizeof(COLORVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &m_pVB));

			COLORVERTEX* verts;
			VERIFYHR(m_pVB->Lock(0, 4 * sizeof (COLORVERTEX), (BYTE**)&verts, 0));

			verts[0].dvX = (float)theApp.m_nViewWidth / 2.0f;
			verts[0].dvY = -(float)theApp.m_nViewHeight / 2.0f;
			verts[0].dvZ = 0.0f;
			verts[0].color = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);

			verts[1].dvX = -(float)theApp.m_nViewWidth / 2.0f;
			verts[1].dvY = -(float)theApp.m_nViewHeight / 2.0f;
			verts[1].dvZ = 0.0f;
			verts[1].color = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);

			verts[2].dvX = (float)theApp.m_nViewWidth / 2.0f;
			verts[2].dvY = (float)theApp.m_nViewHeight / 2.0f;
			verts[2].dvZ = 0.0f;
			verts[2].color = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);

			verts[3].dvX = -(float)theApp.m_nViewWidth / 2.0f;
			verts[3].dvY = (float)theApp.m_nViewHeight / 2.0f;
			verts[3].dvZ = 0.0f;
			verts[3].color = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);

			VERIFYHR(m_pVB->Unlock());
		}

		D3DXMATRIX matProjection, matProjectionSave, matWorldSave, matViewSave;

		XAppGetTransform(D3DTS_PROJECTION, &matProjectionSave);
		XAppGetTransform(D3DTS_WORLD, &matWorldSave);
		XAppGetTransform(D3DTS_VIEW, &matViewSave);

		//D3DXMatrixOrthoLH(&matProjection, 720.0f, 480.0f, -10000.0f, 10000.0f);
		D3DXMatrixOrthoLH(&matProjection, theApp.m_nViewWidth, theApp.m_nViewHeight, -10000.0f, 10000.0f);
		XAppSetTransform(D3DTS_PROJECTION, &matProjection);
		XAppSetTransform(D3DTS_WORLD, &g_matIdentity);
		XAppSetTransform(D3DTS_VIEW, &g_matIdentity);

        XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
        XAppSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
        XAppSetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, (BYTE)(m_alpha * 255.0f)));
        XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
        XAppSetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
        XAppSetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
        XAppSetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        XAppSetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		XAppSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		XAppSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

        VERIFYHR(XAppGetD3DDev()->SetStreamSource(0, m_pVB, sizeof (COLORVERTEX)));
        VERIFYHR(XAppGetD3DDev()->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE));
        VERIFYHR(XAppGetD3DDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));

		XAppSetTransform(D3DTS_PROJECTION, &matProjectionSave);
		XAppSetTransform(D3DTS_WORLD, &matWorldSave);
		XAppSetTransform(D3DTS_VIEW, &matViewSave);
	}

	CViewpoint* pViewpoint = (CViewpoint*)m_viewpoint;
	CNavigationInfo* pNavigationInfo = (CNavigationInfo*)m_navigationInfo;

	if (pNavigationInfo != NULL && !pNavigationInfo->IsKindOf(NODE_CLASS(CNavigationInfo)))
		TRACE(_T("\002Layer with bogus NavigationInfo!\n"));

	XAppGetTransform(D3DTS_PROJECTION, &matProjectionSave);
	XAppGetTransform(D3DTS_WORLD, &matWorldSave);
	XAppGetTransform(D3DTS_VIEW, &matViewSave);

	// BLOCK: Calculate projection matrix
	{
		float nNear = 0.1f;
		float nFar = 1000.0f;
		float fieldOfView = D3DX_PI / 2.0f;

		if (pNavigationInfo != NULL)
		{
			nNear = pNavigationInfo->m_avatarSize.x / 2.0f;
			if (pNavigationInfo->m_visibilityLimit != 0.0f)
				nFar = pNavigationInfo->m_visibilityLimit;
		}

		fieldOfView = pViewpoint->m_fieldOfView;

		float aspect = 720.0f/480.0f;

		if (theApp.m_bStretchWidescreen)
		{
			// This adjusts for 4x3 style resolutions being displayed
			// widescreen on a 16x9 monitor...
			aspect *= 1.25f;
		}

		D3DXMatrixPerspectiveFovLH(&g_matProjection, fieldOfView, aspect, nNear, nFar);
		XAppSetTransform(D3DTS_PROJECTION, &g_matProjection);
	}

	// BLOCK: Calculate view matrix
	if (pViewpoint != NULL)
	{
		D3DXVECTOR3 position = pViewpoint->m_position;
		position.z = -position.z;

		D3DXQUATERNION orientation;
		D3DXQuaternionRotationAxis(&orientation, (D3DXVECTOR3*)&pViewpoint->m_orientation, -pViewpoint->m_orientation.w);

		D3DXMatrixAffineTransformation(&g_matView, 1.0f, NULL, &orientation, &position);
		D3DXMatrixInverse(&g_matView, NULL, &g_matView);

		g_matView._31 = -g_matView._31;
		g_matView._32 = -g_matView._32;
		g_matView._33 = -g_matView._33;
		g_matView._34 = -g_matView._34;
		XAppSetTransform(D3DTS_VIEW, &g_matView);
	}

	// BLOCK: Render children
	{
		XAppPushWorld();
		XAppIdentityWorld();
		XAppUpdateWorld();
		CGroup::Render();
		XAppPopWorld();
	}

	// BLOCK: Restore everything...
	{
		g_matProjection = matProjectionSave;
		g_matView = matViewSave;

		XAppSetTransform(D3DTS_PROJECTION, &matProjectionSave);
		XAppSetTransform(D3DTS_WORLD, &matWorldSave);
		XAppSetTransform(D3DTS_VIEW, &matViewSave);
	}
}

void CLayer::Advance(float nSeconds)
{
	CGroup::Advance(nSeconds);

	if (m_navigationInfo != NULL)
		m_navigationInfo->Advance(nSeconds);

	if (m_viewpoint != NULL)
		m_viewpoint->Advance(nSeconds);
}
