//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "Address.h"

//************************************************************************
// Wrap Test functions

CWrapTest::CWrapTest()
{
	m_szTestName = TEXT("Texture Address Wrap");
	m_szCommandKey = TEXT("Wrap");

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_WRAP;
	dwAddressCapV = D3DPTADDRESSCAPS_WRAP;
}

CWrapTest::~CWrapTest()
{
}

bool CWrapTest::SetDefaultRenderStates(void)
{
	// Turn on Wrap addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, (DWORD)D3DTADDRESS_WRAP);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}
