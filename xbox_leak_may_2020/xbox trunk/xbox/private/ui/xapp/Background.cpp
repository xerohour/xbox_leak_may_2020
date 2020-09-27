#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Camera.h"
#include "Background.h"

extern D3DXMATRIX g_matIdentity;
extern D3DXMATRIX g_matPosition;
extern D3DXMATRIX g_matView;


IMPLEMENT_NODE("Background", CBackground, CNode)

START_NODE_PROPS(CBackground, CNode)
	NODE_PROP(pt_vec3, CBackground, skyColor)
	NODE_PROP(pt_node, CBackground, backdrop)
	NODE_PROP(pt_boolean, CBackground, isBound)
END_NODE_PROPS()


CBackground::CBackground() :
	m_skyColor(0.0f, 0.0f, 0.0f),
	m_backdrop(NULL),
	m_isBound(false)
{
	m_pVB = NULL;

	if (theApp.m_pBackground == NULL)
		Bind();
}

CBackground::~CBackground()
{
	if (m_pVB != NULL)
		m_pVB->Release();

	if (m_backdrop != NULL)
		m_backdrop->Release();

	if (theApp.m_pBackground == this)
		theApp.m_pBackground = NULL;
}

void CBackground::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);

	if (m_backdrop != NULL)
		m_backdrop->Advance(nSeconds);

	if (m_pVB == NULL)
	{
		VERIFYHR(XAppGetD3DDev()->CreateVertexBuffer(4 * sizeof(D3DVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_VERTEX, D3DPOOL_MANAGED, &m_pVB));

		D3DVERTEX* verts;
		VERIFYHR(m_pVB->Lock(0, 4 * sizeof (D3DVERTEX), (BYTE**)&verts, 0));

		verts[0].dvX = (float)theApp.m_nViewWidth / 2.0f;
		verts[0].dvY = -(float)theApp.m_nViewHeight / 2.0f;
		verts[0].dvZ = -10000.0f;
		verts[0].dvNX = (float)theApp.m_nViewWidth / 2.0f;
		verts[0].dvNY = -(float)theApp.m_nViewHeight / 2.0f;
		verts[0].dvNZ = 1.0f;
		verts[0].dvTU = 1.0f;
		verts[0].dvTV = 1.0f;

		verts[1].dvX = -(float)theApp.m_nViewWidth / 2.0f;
		verts[1].dvY = -(float)theApp.m_nViewHeight / 2.0f;
		verts[1].dvZ = -10000.0f;
		verts[1].dvNX = (float)theApp.m_nViewWidth / 2.0f;
		verts[1].dvNY = (float)theApp.m_nViewHeight / 2.0f;
		verts[1].dvNZ = 1.0f;
		verts[1].dvTU = 0.0f;
		verts[1].dvTV = 1.0f;

		verts[2].dvX = (float)theApp.m_nViewWidth / 2.0f;
		verts[2].dvY = (float)theApp.m_nViewHeight / 2.0f;
		verts[2].dvZ = -10000.0f;
		verts[2].dvNX = (float)theApp.m_nViewWidth / 2.0f;
		verts[2].dvNY = (float)theApp.m_nViewHeight / 2.0f;
		verts[2].dvNZ = 1.0f;
		verts[2].dvTU = 1.0f;
		verts[2].dvTV = 0.0f;

		verts[3].dvX = -(float)theApp.m_nViewWidth / 2.0f;
		verts[3].dvY = (float)theApp.m_nViewHeight / 2.0f;
		verts[3].dvZ = -10000.0f;
		verts[3].dvNX = -(float)theApp.m_nViewWidth / 2.0f;
		verts[3].dvNY = (float)theApp.m_nViewHeight / 2.0f;
		verts[3].dvNZ = 1.0f;
		verts[3].dvTU = 0.0f;
		verts[3].dvTV = 0.0f;

		VERIFYHR(m_pVB->Unlock());
	}
}

/*
void CBackground::Render()
{
	if (m_skyColor.x == 0 && m_skyColor.y == 0 && m_skyColor.z == 0)
		return;

	return;

	if (m_rgvert == NULL)
	{
		static float angles [] = { 0.1f, 0.3f, 0.6f, 1.0f , D3DX_PI / 2.0f};
		static D3DCOLOR colors [] = 
		{
			D3DRGB(0, 0, 0),
			D3DRGB(0.3f, 0.1f, 0),
			D3DRGB(0.4f, 0.15f, 0),
			D3DRGB(0.6f, 0.25f, 0),
			D3DRGB(1.0f, 0.5f, 0),
			D3DRGB(1.0f, 0, 0)
		};

		CreateBackgroundSphere(true, 5, angles, 6, colors);
	}

	XAppPushWorld();

	D3DXVECTOR3 pos = theCamera.m_position;

	XAppIdentityWorld();
	XAppMultWorld(&g_matView);
	XAppUpdateWorld();

	XAppSetTexture(0, NULL);

	DWORD dwOldLighting;
	XAppGetRenderState(D3DRS_LIGHTING, &dwOldLighting);
	XAppSetRenderState(D3DRS_LIGHTING, false);

	XAppSetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	XAppGetD3DDev()->SetVertexShader(D3DFVF_VERTEX);
	HRESULT hr = XAppGetD3DDev()->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_nIndices, m_nIndices * 3, m_rgindices, D3DFMT_INDEX16, m_rgvert, sizeof (m_rgvert[0]));
	XAppSetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
	XAppSetRenderState(D3DRS_LIGHTING, dwOldLighting);

	XAppPopWorld();
}
*/

/*
void CBackground::CreateBackgroundSphere(bool bSky, int n2, const float* angles, int nColors, D3DCOLOR* colors)
{
	int i, j;

	float r = 20.0f;
	int n1 = 12;

	if (n1 <= 0)
		n1 = 12;

	if (n2 <= 0)
		n2 = n1 / 2;

	n1 += 1;
	n2 += 1;

	// Create vertices...

	m_nVerts = n1 * n2;
	m_rgvert = new D3DLVERTEX [m_nVerts];

	float du = (2.0f * D3DX_PI) / (float) (n1 - 1);
	float dv = D3DX_PI / (float) (n2 - 1);

	float dpu = 1.0f / (float) (n1 - 1);
	float dpv = 1.0f / (float) (n2 - 1);

	float c0 = -D3DX_PI * 0.5;

	for (i = 0; i < n1; i++)
	{
		int offset = i * n2;

		float u = c0 +  du * i;
		float cosu = cosf(u);
		float sinu = sinf(u);

		float pu = 1.0f - ((float) (i) * dpu);

		for (j = 0; j < n2; j++)
		{
			float v;

			if (j == 0)
				v = 0; 
			else
				v = angles[j - 1];

			if (!bSky)
				v = D3DX_PI - v;

			float cosv = cosf(v);
			float sinv = sinf(v);

			m_rgvert[offset + j].dvX = r * cosu * sinv;
			m_rgvert[offset + j].dvY = r * sinu * sinv;
			m_rgvert[offset + j].dvZ = r * cosv;

			m_rgvert[offset + j].dcColor = colors[min(nColors - 1, j)];
		}
	}


	// Create faces...

	m_nIndices = (n1 - 1) * (n2 - 1) * 3 * 2;
	m_rgindices = new WORD [m_nIndices];
	WORD* pw = m_rgindices;

	for (i = 0; i < (n1 - 1); i++)
	{
		int voffset = i * n2;

		for (j = 0; j < (n2 - 1); j++)
		{
			if (bSky)
			{
				*pw++ = voffset + (j + 1);
				*pw++ = voffset + n2 + (j + 1);
				*pw++ = voffset + j + n2;

				*pw++ = voffset + (j + 1);
				*pw++ = voffset + j + n2;
				*pw++ = voffset + j;
			}
			else
			{
				*pw++ = voffset + (j + 1);
				*pw++ = voffset + j;
				*pw++ = voffset + j + n2;

				*pw++ = voffset + (j + 1);
				*pw++ = voffset + j + n2;
				*pw++ = voffset + n2 + (j + 1);
			}
		}
	}
}
*/

void CBackground::RenderBackdrop()
{
	ASSERT(m_backdrop != NULL);

	LPDIRECT3DTEXTURE8 pTexture = m_backdrop->GetTextureSurface();
	if (pTexture == NULL)
		return;

	XAppSetTexture(0, pTexture);
	XAppSetTexture(1, NULL);

	D3DXMATRIX matProjection, matProjectionSave, matWorldSave, matViewSave;

	XAppGetTransform(D3DTS_PROJECTION, &matProjectionSave);
	XAppGetTransform(D3DTS_WORLD, &matWorldSave);
	XAppGetTransform(D3DTS_VIEW, &matViewSave);

	D3DXMatrixOrthoLH(&matProjection, theApp.m_nViewWidth, theApp.m_nViewHeight, -10000.0f, 10000.0f);
	XAppSetTransform(D3DTS_PROJECTION, &matProjection);
	XAppSetTransform(D3DTS_WORLD, &g_matIdentity);
	XAppSetTransform(D3DTS_VIEW, &g_matIdentity);

	DWORD dwLighting, dwAlphaBlendEnable, dwCullMode, dwZWriteEnable;
	XAppGetRenderState(D3DRS_LIGHTING, &dwLighting);
	XAppGetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable);
	XAppGetRenderState(D3DRS_CULLMODE, &dwCullMode);
	XAppGetRenderState(D3DRS_ZWRITEENABLE, &dwZWriteEnable);

	XAppSetRenderState(D3DRS_LIGHTING, FALSE);
	XAppSetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	XAppSetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	XAppSetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	XAppSetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	XAppSetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

		VERIFYHR(XAppGetD3DDev()->SetStreamSource(0, m_pVB, sizeof (D3DVERTEX)));
		VERIFYHR(XAppGetD3DDev()->SetVertexShader(D3DFVF_VERTEX));
		VERIFYHR(XAppGetD3DDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));

	XAppSetRenderState(D3DRS_LIGHTING, dwLighting);
	XAppSetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable);
	XAppSetRenderState(D3DRS_CULLMODE, dwCullMode);
	XAppSetRenderState(D3DRS_ZWRITEENABLE, dwZWriteEnable);

	XAppSetTransform(D3DTS_PROJECTION, &matProjectionSave);
	XAppSetTransform(D3DTS_WORLD, &matWorldSave);
	XAppSetTransform(D3DTS_VIEW, &matViewSave);
}

void CBackground::Bind()
{
	if (theApp.m_pBackground != NULL)
		theApp.m_pBackground->m_isBound = false;

	theApp.m_pBackground = this;
	m_isBound = true;
}

bool CBackground::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_isBound))
	{
		if (*(bool*)pvValue)
			Bind();
	}

	return true;
}




/*
D3DXQUATERNION backgroundRotation(0, 0, 1.0f, 0);

void CBackground::UpdateBackgroundCamera(Vector axis,float angle)
{
	// realign current transformation of background node with new camera 
	if (backgroundRotation != NULL)
	{
		if (thirdPersonView)
		{
			// need physical camera rotation
			ConvertCameraToOrientation(physicalCamera->position, physicalCamera->target, physicalCamera->up,axis, angle);
		}

		backgroundRotation = D3DXQUATERNION(axis.x, axis.y, axis.z, -angle);

		GCamera *theCamera = &m_camera;
		GCamera *c = GetPhysicalCamera();

		// take view angle constant 
		float field = 2.0;
		float targetDistance = field / (2.0f * tanf(c->GetFovY() * 0.5f));
		D3DXVECTOR3 dir(0.0f, 0.0f, -1.0f);

		theCamera->position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		theCamera->height = field;
		theCamera->width = field;
		theCamera->target = theCamera->position + targetDistance * dir;
		theCamera->SetAspect(c->aspect);
		theCamera->SetFieldPixel(renderState.renderSizex, renderState.renderSizey);
	}
}
*/
