//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "Address.h"

//************************************************************************
// Mirror Test functions

CMirrorTest::CMirrorTest()
{
	m_szTestName = TEXT("Texture Address Mirror");
	m_szCommandKey = TEXT("Mirror");

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRROR;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRROR;
}

CMirrorTest::~CMirrorTest()
{
}

bool CMirrorTest::SetDefaultRenderStates(void)
{
	// Turn on Mirror addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_MIRROR);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_MIRROR);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, (DWORD)D3DTADDRESS_MIRROR);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

