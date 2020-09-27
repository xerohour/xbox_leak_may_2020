//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "srt.h"

//************************************************************************
// Texture Test functions

CSRTTexture::CSRTTexture()
{
	m_szTestName = TEXT("SRT Texture");
	m_szCommandKey = TEXT("Texture");
}

CSRTTexture::~CSRTTexture()
{
}

//************************************************************************
// No ZBuffer Texture Test functions

CSRTNoZTexture::CSRTNoZTexture()
{
	m_szTestName = TEXT("SRT No ZBuffer Texture");
	m_szCommandKey = TEXT("NoZTexture");

    // Disable ZBuffers
    m_ModeOptions.fZBuffer = false;
}

CSRTNoZTexture::~CSRTNoZTexture()
{
}
