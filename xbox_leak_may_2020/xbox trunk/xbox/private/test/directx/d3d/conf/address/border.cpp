//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "Address.h"

//************************************************************************
// Border Test functions

CBorderTest::CBorderTest()
{
	m_szTestName = TEXT("Texture Address Border");
	m_szCommandKey = TEXT("Border");

	// Tell parent to use border colors
	bBorder = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_BORDER;
	dwAddressCapV = D3DPTADDRESSCAPS_BORDER;
}

CBorderTest::~CBorderTest()
{
}

bool CBorderTest::SetDefaultRenderStates(void)
{
	// Turn on Border addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, (DWORD)D3DTADDRESS_BORDER);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

