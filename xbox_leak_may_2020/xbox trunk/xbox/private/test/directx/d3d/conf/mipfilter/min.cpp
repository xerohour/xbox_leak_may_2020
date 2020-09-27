//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "MipFilter.h"

//************************************************************************
// Point_MinPoint Test functions

CPoint_MinPointTest::CPoint_MinPointTest()
{
	m_szTestName = TEXT("MipFilter Point_MinPoint");
	m_szCommandKey = TEXT("Point_MinPoint");
}

CPoint_MinPointTest::~CPoint_MinPointTest()
{
}

//************************************************************************
// Point_MinLinear Test functions

CPoint_MinLinearTest::CPoint_MinLinearTest()
{
	m_szTestName = TEXT("MipFilter Point_MinLinear");
	m_szCommandKey = TEXT("Point_MinLinear");

	// Inidicate that we are a MinLinear test
	bMinLinear = true;
}

CPoint_MinLinearTest::~CPoint_MinLinearTest()
{
}

//************************************************************************
// Linear_MinPoint Test functions

CLinear_MinPointTest::CLinear_MinPointTest()
{
	m_szTestName = TEXT("MipFilter Linear_MinPoint");
	m_szCommandKey = TEXT("Linear_MinPoint");

	// Inidicate that we are a MipLinear test
	bMipLinear = true;
}

CLinear_MinPointTest::~CLinear_MinPointTest()
{
}

//************************************************************************
// Linear_MinLinear Test functions

CLinear_MinLinearTest::CLinear_MinLinearTest()
{
	m_szTestName = TEXT("MipFilter Linear_MinLinear");
	m_szCommandKey = TEXT("Linear_MinLinear");

	// Inidicate that we are a MipLinear & MinLinear test
	bMipLinear = true;
	bMinLinear = true;
}

CLinear_MinLinearTest::~CLinear_MinLinearTest()
{
}
