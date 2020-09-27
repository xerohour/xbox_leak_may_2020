//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Overdraw.h"

//************************************************************************
// Alpha_Overdraw Test functions

CAlpha_OverdrawTest::CAlpha_OverdrawTest()
{
	m_szTestName = TEXT("Overdraw/Underdraw");
	m_szCommandKey = TEXT("Alpha");

	// Inidicate that we are the alpha test
	bAlpha = true;
}

CAlpha_OverdrawTest::~CAlpha_OverdrawTest()
{
}


