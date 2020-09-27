//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaCmp.h"

//************************************************************************
// Equal Test functions

CAlphaEqualTest::CAlphaEqualTest()
{
	m_szTestName = TEXT("Alpha Compare Equal");
	m_szCommandKey = TEXT("Equal");

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_EQUAL;
}

CAlphaEqualTest::~CAlphaEqualTest()
{
}

bool CAlphaEqualTest::SetDefaultRenderStates(void)
{
	// Setup the common default render states
	CAlphaCmpTest::SetDefaultRenderStates();

	// Set the Alpha compare func to Equal
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_EQUAL);
	return true;
}

//************************************************************************
// NotEqual Test functions

CAlphaNotEqualTest::CAlphaNotEqualTest()
{
	m_szTestName = TEXT("Alpha Compare NotEqual");
	m_szCommandKey = TEXT("NotEqual");

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_NOTEQUAL;
}

CAlphaNotEqualTest::~CAlphaNotEqualTest()
{
}

bool CAlphaNotEqualTest::SetDefaultRenderStates(void)
{
	// Setup the common default render states
	CAlphaCmpTest::SetDefaultRenderStates();

	// Set the Alpha compare func to NotEqual
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_NOTEQUAL);
	return true;
}
