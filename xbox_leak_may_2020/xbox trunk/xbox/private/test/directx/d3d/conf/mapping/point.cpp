//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Mapping.h"
#include "Mapping.h"

//************************************************************************
// Point Sample Test functions

CMapPointTest::CMapPointTest()
{
	m_szTestName = TEXT("Texture Mapping Nearest");
	m_szCommandKey = TEXT("Nearest");

	// Tell the parent which caps to look for
	dwMinFilterCap = D3DPTFILTERCAPS_MINFPOINT;//D3DPTFILTERCAPS_NEAREST;
	dwMagFilterCap = D3DPTFILTERCAPS_MAGFPOINT;//D3DPTFILTERCAPS_NEAREST;
}  

CMapPointTest::~CMapPointTest()
{
}

bool CMapPointTest::SetDefaultRenderStates(void)
{
	// Make sure that perspection is off and use Point Sample filtering
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)false);
//	SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (DWORD)D3DFILTER_NEAREST);
    SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
//	SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (DWORD)D3DFILTER_NEAREST);
    SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);

	return true;
}
