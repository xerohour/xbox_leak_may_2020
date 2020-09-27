//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "One.h"

//************************************************************************
// One/Zero Test functions

COneZeroTest::COneZeroTest()
{
	m_szTestName = TEXT("SrcBlend: One DestBlend: Zero");
	m_szCommandKey = TEXT("OneZero");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "Zero";
}

COneZeroTest::~COneZeroTest()
{
}

bool COneZeroTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:One and Dest:Zero
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	return true;
}

//************************************************************************
// One/One Test functions

COneOneTest::COneOneTest()
{
	m_szTestName = TEXT("SrcBlend: One DestBlend: One");
	m_szCommandKey = TEXT("OneOne");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "One";
}

COneOneTest::~COneOneTest()
{
}

bool COneOneTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:One and Dest:One
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ONE);

	return true;
}

//************************************************************************
// One/SrcColor Test functions

COneSrcColorTest::COneSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: One DestBlend: SrcColor");
	m_szCommandKey = TEXT("OneSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "SrcColor";
}

COneSrcColorTest::~COneSrcColorTest()
{
}

bool COneSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:One and Dest:SrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCCOLOR);

	return true;
}

//************************************************************************
// One/InvSrcColor Test functions

COneInvSrcColorTest::COneInvSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: One DestBlend: InvSrcColor");
	m_szCommandKey = TEXT("OneInvSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "InvSrcColor";
}

COneInvSrcColorTest::~COneInvSrcColorTest()
{
}

bool COneInvSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:One and Dest:InvSrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);

	return true;
}

//************************************************************************
// One/SrcAlpha Test functions

COneSrcAlphaTest::COneSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: One DestBlend: SrcAlpha");
	m_szCommandKey = TEXT("OneSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "SrcAlpha";
}

COneSrcAlphaTest::~COneSrcAlphaTest()
{
}

bool COneSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:One and Dest:SrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHA);

	return true;
}

//************************************************************************
// One/InvSrcAlpha Test functions

COneInvSrcAlphaTest::COneInvSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: One DestBlend: InvSrcAlpha");
	m_szCommandKey = TEXT("OneInvSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "InvSrcAlpha";
}

COneInvSrcAlphaTest::~COneInvSrcAlphaTest()
{
}

bool COneInvSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:One and Dest:InvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	return true;
}

//************************************************************************
// One/DestAlpha Test functions

COneDestAlphaTest::COneDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: One DestBlend: DestAlpha");
	m_szCommandKey = TEXT("OneDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "DestAlpha";
}

COneDestAlphaTest::~COneDestAlphaTest()
{
}

bool COneDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:One and Dest:DestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTALPHA);

	return true;
}

//************************************************************************
// One/InvDestAlpha Test functions

COneInvDestAlphaTest::COneInvDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: One DestBlend: InvDestAlpha");
	m_szCommandKey = TEXT("OneInvDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "InvDestAlpha";
}

COneInvDestAlphaTest::~COneInvDestAlphaTest()
{
}

bool COneInvDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:One and Dest:InvDestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTALPHA);

	return true;
}

//************************************************************************
// One/DestColor Test functions

COneDestColorTest::COneDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: One DestBlend: DestColor");
	m_szCommandKey = TEXT("OneDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "DestColor";
}

COneDestColorTest::~COneDestColorTest()
{
}

bool COneDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:One and Dest:DestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTCOLOR);

	return true;
}

//************************************************************************
// One/InvDestColor Test functions

COneInvDestColorTest::COneInvDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: One DestBlend: InvDestColor");
	m_szCommandKey = TEXT("OneInvDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "InvDestColor";
}

COneInvDestColorTest::~COneInvDestColorTest()
{
}

bool COneInvDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:One and Dest:InvDestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);

	return true;
}

//************************************************************************
// One/SrcAlphaSat Test functions

COneSrcAlphaSatTest::COneSrcAlphaSatTest()
{
	m_szTestName = TEXT("SrcBlend: One DestBlend: SrcAlphaSat");
	m_szCommandKey = TEXT("OneSrcAlphaSat");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ONE;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "One";
	szDestName = "SrcAlphaSat";
}

COneSrcAlphaSatTest::~COneSrcAlphaSatTest()
{
}

bool COneSrcAlphaSatTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:One and Dest:SrcAlphaSat
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHASAT);

	return true;
}

