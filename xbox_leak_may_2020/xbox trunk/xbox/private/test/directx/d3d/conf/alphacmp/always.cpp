//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaCmp.h"

//************************************************************************
// Never Test functions

CAlphaNeverTest::CAlphaNeverTest()
{
	m_szTestName = TEXT("Alpha Compare Never");
	m_szCommandKey = TEXT("Never");

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_NEVER;
}

CAlphaNeverTest::~CAlphaNeverTest()
{
}

bool CAlphaNeverTest::SetDefaultRenderStates(void)
{
	// Setup the common default render states
	CAlphaCmpTest::SetDefaultRenderStates();

	// Set the Alpha compare func to Never
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_NEVER);
	return true;
}

//************************************************************************
// Always Test functions

CAlphaAlwaysTest::CAlphaAlwaysTest()
{
	m_szTestName = TEXT("Alpha Compare Always");
	m_szCommandKey = TEXT("Always");

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_ALWAYS;
}

CAlphaAlwaysTest::~CAlphaAlwaysTest()
{
}

bool CAlphaAlwaysTest::SetDefaultRenderStates(void)
{
	// Setup the common default render states
	CAlphaCmpTest::SetDefaultRenderStates();

	// Set the Alpha compare func to Always
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_ALWAYS);
	return true;
}
