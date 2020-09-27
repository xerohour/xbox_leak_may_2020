//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "False.h"

//************************************************************************
// False Test functions

CFalseTest::CFalseTest()
{
	m_szTestName = TEXT("False (SrcAlpha/InvSrcAlpha)");
	m_szCommandKey = TEXT("False");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "InvSrcAlpha";

	// Overwrite Enable variable
	nAlphaEnable = FALSE;
}

CFalseTest::~CFalseTest()
{
}

bool CFalseTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlpha and Dest:InvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	return true;
}
