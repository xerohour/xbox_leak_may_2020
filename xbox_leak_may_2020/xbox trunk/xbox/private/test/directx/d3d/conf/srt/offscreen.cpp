//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "srt.h"

//************************************************************************
// Offscreen Test functions

CSRTOffscreen::CSRTOffscreen()
{
	m_szTestName = TEXT("SRT Offscreen");
	m_szCommandKey = TEXT("Offscreen");

    m_bOffscreen = true;
}

CSRTOffscreen::~CSRTOffscreen()
{
}

//************************************************************************
// No ZBuffer Offscreen Test functions

CSRTNoZOffscreen::CSRTNoZOffscreen()
{
	m_szTestName = TEXT("SRT No ZBuffer Offscreen");
	m_szCommandKey = TEXT("NoZOffscreen");

    m_bOffscreen = true;

    // Disable ZBuffers
    m_ModeOptions.fZBuffer = false;
}

CSRTNoZOffscreen::~CSRTNoZOffscreen()
{
}

