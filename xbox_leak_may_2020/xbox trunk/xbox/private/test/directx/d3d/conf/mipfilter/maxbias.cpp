//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "MipFilter.h"

//************************************************************************
// Point_MaxMipLevel Test functions

CPoint_MaxMipLevelTest::CPoint_MaxMipLevelTest()
{
	m_szTestName = TEXT("MipFilter Point_MaxMipLevel");
	m_szCommandKey = TEXT("Point_MaxMipLevel");

	// Inidicate that we are a Max test
	bMax = true;
}

CPoint_MaxMipLevelTest::~CPoint_MaxMipLevelTest()
{
}

//************************************************************************
// Linear_MaxMipLevel Test functions

CLinear_MaxMipLevelTest::CLinear_MaxMipLevelTest()
{
	m_szTestName = TEXT("MipFilter Linear_MaxMipLevel");
	m_szCommandKey = TEXT("Linear_MaxMipLevel");

	// Inidicate that we are a MipLinear & Max test
	bMax = true;
	bMipLinear = true;
}

CLinear_MaxMipLevelTest::~CLinear_MaxMipLevelTest()
{
}

//************************************************************************
// Point_LODBias Test functions

CPoint_LODBiasTest::CPoint_LODBiasTest()
{
	m_szTestName = TEXT("MipFilter Point_LODBias");
	m_szCommandKey = TEXT("Point_LODBias");

	// Inidicate that we are a Bias test
	bBias = true;
}

CPoint_LODBiasTest::~CPoint_LODBiasTest()
{
}

//************************************************************************
// Linear_LODBias Test functions

CLinear_LODBiasTest::CLinear_LODBiasTest()
{
	m_szTestName = TEXT("MipFilter Linear_LODBias");
	m_szCommandKey = TEXT("Linear_LODBias");

	// Inidicate that we are a MipLinear & Bias test
	bBias = true;
	bMipLinear = true;
}

CLinear_LODBiasTest::~CLinear_LODBiasTest()
{
}

