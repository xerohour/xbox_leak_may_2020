//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Mapping.h"

//************************************************************************
// Bi-Linear Test functions

CMapLinearTest::CMapLinearTest()
{
	m_szTestName = TEXT("Texture Mapping Linear");
	m_szCommandKey = TEXT("Linear");

	// Tell the parent which caps to look for
	dwMinFilterCap = D3DPTFILTERCAPS_MINFLINEAR;//D3DPTFILTERCAPS_LINEAR;
	dwMagFilterCap = D3DPTFILTERCAPS_MAGFLINEAR;//D3DPTFILTERCAPS_LINEAR;
}

CMapLinearTest::~CMapLinearTest()
{
}
 
bool CMapLinearTest::SetDefaultRenderStates(void)
{
	// Make sure that perspection is off and use Bi-Linear filtering
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)false);
//	SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (DWORD)D3DFILTER_LINEAR);
    SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
//	SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (DWORD)D3DFILTER_LINEAR);
    SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);

	return true;
}
