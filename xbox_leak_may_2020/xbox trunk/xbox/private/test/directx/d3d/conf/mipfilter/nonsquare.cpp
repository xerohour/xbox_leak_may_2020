//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "MipFilter.h"

//************************************************************************
// Point_NonSquareW Test functions

CPoint_NonSquareWTest::CPoint_NonSquareWTest()
{
	m_szTestName = TEXT("MipFilter Point_NonSquareWidth");
	m_szCommandKey = TEXT("Point_NonSquareWidth");

	// Inidicate that we are a NonSquareW test
	bNonSquareW = true;
}

CPoint_NonSquareWTest::~CPoint_NonSquareWTest()
{
}
 
//************************************************************************
// Point_NonSquareH Test functions

CPoint_NonSquareHTest::CPoint_NonSquareHTest()
{
	m_szTestName = TEXT("MipFilter Point_NonSquareHeight");
	m_szCommandKey = TEXT("Point_NonSquareHeight");

	// Inidicate that we are a NonSquareH test
	bNonSquareH = true;
}

CPoint_NonSquareHTest::~CPoint_NonSquareHTest()
{
}
 
//************************************************************************
// Linear_NonSquareW Test functions

CLinear_NonSquareWTest::CLinear_NonSquareWTest()
{
	m_szTestName = TEXT("MipFilter Linear_NonSquareWidth");
	m_szCommandKey = TEXT("Linear_NonSquareWidth");

	// Inidicate that we are a MipLinear & NonSquareW test
	bMipLinear = true;
	bNonSquareW = true;
}

CLinear_NonSquareWTest::~CLinear_NonSquareWTest()
{
}
 
//************************************************************************
// Linear_NonSquareH Test functions

CLinear_NonSquareHTest::CLinear_NonSquareHTest()
{
	m_szTestName = TEXT("MipFilter Linear_NonSquareHeight");
	m_szCommandKey = TEXT("Linear_NonSquareHeight");

	// Inidicate that we are a MipLinear & NonSquareH test
	bMipLinear = true;
	bNonSquareH = true;
}

CLinear_NonSquareHTest::~CLinear_NonSquareHTest()
{
}
 
