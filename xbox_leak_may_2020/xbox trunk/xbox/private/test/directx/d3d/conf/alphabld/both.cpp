//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "Both.h"

#ifndef UNDER_XBOX

//************************************************************************
// BothSrcAlpha Test functions

CBothSrcAlphaTest::CBothSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: BothSrcAlpha");
	m_szCommandKey = TEXT("BothSrcAlpha");

	// Tell parent class what cap to look for
	dwSrcCap = D3DPBLENDCAPS_BOTHSRCALPHA;
	dwDestCap = 0xffffffff;

	// Give parent a printable name for the caps
	szSrcName = "BothSrcAlpha";
}

CBothSrcAlphaTest::~CBothSrcAlphaTest()
{
}

bool CBothSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:BothSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_BOTHSRCALPHA);

	return true;
}

//************************************************************************
// BothInvSrcAlpha Test functions

CBothInvSrcAlphaTest::CBothInvSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: BothInvSrcAlpha");
	m_szCommandKey = TEXT("BothSrcAlpha");

	// Tell parent class what cap to look for
	dwSrcCap = D3DPBLENDCAPS_BOTHINVSRCALPHA;
	dwDestCap = 0xffffffff;

	// Give parent a printable name for the caps
	szSrcName = "BothInvSrcAlpha";
}

CBothInvSrcAlphaTest::~CBothInvSrcAlphaTest()
{
}

bool CBothInvSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:BothInvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_BOTHINVSRCALPHA);

	return true;
}

#endif // !UNDER_XBOX
