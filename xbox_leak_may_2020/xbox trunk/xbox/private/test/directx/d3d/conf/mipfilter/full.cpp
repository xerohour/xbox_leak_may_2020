//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "MipFilter.h"

//************************************************************************
// Point_FullSquare Test functions

CPoint_FullSquareTest::CPoint_FullSquareTest()
{
	m_szTestName = TEXT("MipFilter Point_FullSquare");
	m_szCommandKey = TEXT("Point_FullSquare");

	// Inidicate that we are a Full test
	bFull = true;
}

CPoint_FullSquareTest::~CPoint_FullSquareTest()
{
}

//************************************************************************
// Point_FullNonSquareW Test functions

CPoint_FullNonSquareWTest::CPoint_FullNonSquareWTest()
{
	m_szTestName = TEXT("MipFilter Point_FullNonSquareWidth");
	m_szCommandKey = TEXT("Point_FullNonSquareWidth");

	// Inidicate that we are a Full & NonSquareW test
	bFull = true;
	bNonSquareW = true;
}

CPoint_FullNonSquareWTest::~CPoint_FullNonSquareWTest()
{
}
 
//************************************************************************
// Point_FullNonSquareH Test functions

CPoint_FullNonSquareHTest::CPoint_FullNonSquareHTest()
{
	m_szTestName = TEXT("MipFilter Point_FullNonSquareHeight");
	m_szCommandKey = TEXT("Point_FullNonSquareHeight");

	// Inidicate that we are a Full & NonSquareH test
	bFull = true;
	bNonSquareH = true;
}

CPoint_FullNonSquareHTest::~CPoint_FullNonSquareHTest()
{
}

//************************************************************************
// Linear_FullSquare Test functions

CLinear_FullSquareTest::CLinear_FullSquareTest()
{
	m_szTestName = TEXT("MipFilter Linear_FullSquare");
	m_szCommandKey = TEXT("Linear_FullSquare");

	// Inidicate that we are a MipLinear & Full test
	bFull = true;
	bMipLinear = true;
}

CLinear_FullSquareTest::~CLinear_FullSquareTest()
{
}

//************************************************************************
// Linear_FullNonSquareW Test functions

CLinear_FullNonSquareWTest::CLinear_FullNonSquareWTest()
{
	m_szTestName = TEXT("MipFilter Linear_FullNonSquareWidth");
	m_szCommandKey = TEXT("Linear_FullNonSquareWidth");

	// Inidicate that we are a MipLinear, Full, & NonSquareW test
	bFull = true;
	bMipLinear = true;
	bNonSquareW = true;
}

CLinear_FullNonSquareWTest::~CLinear_FullNonSquareWTest()
{
}
 
//************************************************************************
// Linear_FullNonSquareH Test functions

CLinear_FullNonSquareHTest::CLinear_FullNonSquareHTest()
{
	m_szTestName = TEXT("MipFilter Linear_FullNonSquareHeight");
	m_szCommandKey = TEXT("Linear_FullNonSquareHeight");

	// Inidicate that we are a MipLinear, Full, & NonSquareH test
	bFull = true;
	bMipLinear = true;
	bNonSquareH = true;
}

CLinear_FullNonSquareHTest::~CLinear_FullNonSquareHTest()
{
}
