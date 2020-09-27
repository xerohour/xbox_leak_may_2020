//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "Address.h"

//************************************************************************
// Clamp Test functions

CClampTest::CClampTest()
{
	m_szTestName = TEXT("Texture Address Clamp");
	m_szCommandKey = TEXT("Clamp");

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_CLAMP;
	dwAddressCapV = D3DPTADDRESSCAPS_CLAMP;
}

CClampTest::~CClampTest()
{
}

bool CClampTest::SetDefaultRenderStates(void)
{
	// Turn on Clamp addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, (DWORD)D3DTADDRESS_CLAMP);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

