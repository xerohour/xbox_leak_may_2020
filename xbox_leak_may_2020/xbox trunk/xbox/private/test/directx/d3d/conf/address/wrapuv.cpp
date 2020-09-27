//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "Address.h"

//************************************************************************
// WrapUV Test functions

CWrapUVTest::CWrapUVTest()
{
	m_szTestName = TEXT("Texture Address WrapUV");
	m_szCommandKey = TEXT("WrapUV");

	// Tell parent to use wrap grid
	bWrapUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_WRAP;
	dwAddressCapV = D3DPTADDRESSCAPS_WRAP;
}

CWrapUVTest::~CWrapUVTest()
{
}

bool CWrapUVTest::SetDefaultRenderStates(void)
{
	// Turn on Wrap addressing and the WrapU & Wrap V modes
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, (DWORD)D3DTADDRESS_WRAP);
    SetRenderState(D3DRS_WRAP0, D3DWRAP_U | D3DWRAP_V);
//	SetRenderState(D3DRENDERSTATE_WRAPU, (DWORD)TRUE);
//	SetRenderState(D3DRENDERSTATE_WRAPV, (DWORD)TRUE);

	return true;
}
