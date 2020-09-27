//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaCmp.h"

//************************************************************************
// Greater Test functions

CAlphaGreaterTest::CAlphaGreaterTest()
{
	m_szTestName = TEXT("Alpha Compare Greater");
	m_szCommandKey = TEXT("Greater");

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_GREATER;
}

CAlphaGreaterTest::~CAlphaGreaterTest()
{
}

bool CAlphaGreaterTest::SetDefaultRenderStates(void)
{
	// Setup the common default render states
	CAlphaCmpTest::SetDefaultRenderStates();

	// Set the Alpha compare func to Greater
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_GREATER);
	return true;
}

//************************************************************************
// GreaterEqual Test functions

CAlphaGreaterEqualTest::CAlphaGreaterEqualTest()
{
	m_szTestName = TEXT("Alpha Compare GreaterEqual");
	m_szCommandKey = TEXT("GreaterEqual");

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_GREATEREQUAL;
}

CAlphaGreaterEqualTest::~CAlphaGreaterEqualTest()
{
}

bool CAlphaGreaterEqualTest::SetDefaultRenderStates(void)
{
	// Setup the common default render states
	CAlphaCmpTest::SetDefaultRenderStates();

	// Set the Alpha compare func to GreaterEqual
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_GREATEREQUAL);
	return true;
}
