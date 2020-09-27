//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "clear_test.h"

/******* Clear Texture as render target. *******/

CTextureClearTest::CTextureClearTest()
{
	m_szTestName = TEXT("Clear_Test Texture");
	m_szCommandKey = TEXT("Texture");
}

CTextureClearTest::~CTextureClearTest()
{
}

/******* Clear Offscreen as render target. *******/

COffscreenClearTest::COffscreenClearTest()
{
	m_szTestName = TEXT("Clear_Test Offscreen");
	m_szCommandKey = TEXT("Offscreen");

    m_bOffscreen = true;
}

COffscreenClearTest::~COffscreenClearTest()
{
}
