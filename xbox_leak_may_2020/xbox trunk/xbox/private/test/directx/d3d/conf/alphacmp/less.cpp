//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaCmp.h"

//************************************************************************
// Less Test functions

CAlphaLessTest::CAlphaLessTest()
{
	m_szTestName = TEXT("Alpha Compare Less");
	m_szCommandKey = TEXT("Less");

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_LESS;
}

CAlphaLessTest::~CAlphaLessTest()
{
}

bool CAlphaLessTest::SetDefaultRenderStates(void)
{
	// Setup the common default render states
	CAlphaCmpTest::SetDefaultRenderStates();

	// Set the Alpha compare func to Less
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_LESS);
	return true;
}

//************************************************************************
// LessEqual Test functions

CAlphaLessEqualTest::CAlphaLessEqualTest()
{
	m_szTestName = TEXT("Alpha Compare LessEqual");
	m_szCommandKey = TEXT("LessEqual");

	// Tell parent class what cap to look for
	dwAlphaCmpCap = D3DPCMPCAPS_LESSEQUAL;
}

CAlphaLessEqualTest::~CAlphaLessEqualTest()
{
}

bool CAlphaLessEqualTest::SetDefaultRenderStates(void)
{
	// Setup the common default render states
	CAlphaCmpTest::SetDefaultRenderStates();

	// Set the Alpha compare func to LessEqual
	SetRenderState(D3DRENDERSTATE_ALPHAFUNC, (DWORD)D3DCMP_LESSEQUAL);
	return true;
}

