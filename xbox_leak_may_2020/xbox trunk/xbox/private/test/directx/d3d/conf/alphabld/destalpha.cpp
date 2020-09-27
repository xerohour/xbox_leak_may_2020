//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "DestAlpha.h"

//************************************************************************
// DestAlpha/Zero Test functions

CDestAlphaZeroTest::CDestAlphaZeroTest()
{
	m_szTestName = TEXT("SrcBlend: DestAlpha DestBlend: Zero");
	m_szCommandKey = TEXT("DestAlphaZero");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "Zero";
}

CDestAlphaZeroTest::~CDestAlphaZeroTest()
{
}

bool CDestAlphaZeroTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestAlpha and Dest:Zero
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	return true;
}

//************************************************************************
// DestAlpha/One Test functions

CDestAlphaOneTest::CDestAlphaOneTest()
{
	m_szTestName = TEXT("SrcBlend: DestAlpha DestBlend: One");
	m_szCommandKey = TEXT("DestAlphaOne");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "One";
}

CDestAlphaOneTest::~CDestAlphaOneTest()
{
}

bool CDestAlphaOneTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestAlpha and Dest:One
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ONE);

	return true;
}

//************************************************************************
// DestAlpha/SrcColor Test functions

CDestAlphaSrcColorTest::CDestAlphaSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: DestAlpha DestBlend: SrcColor");
	m_szCommandKey = TEXT("DestAlphaSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "SrcColor";
}

CDestAlphaSrcColorTest::~CDestAlphaSrcColorTest()
{
}

bool CDestAlphaSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestAlpha and Dest:SrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCCOLOR);

	return true;
}

//************************************************************************
// DestAlpha/InvSrcColor Test functions

CDestAlphaInvSrcColorTest::CDestAlphaInvSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: DestAlpha DestBlend: InvSrcColor");
	m_szCommandKey = TEXT("DestAlphaInvSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "InvSrcColor";
}

CDestAlphaInvSrcColorTest::~CDestAlphaInvSrcColorTest()
{
}

bool CDestAlphaInvSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestAlpha and Dest:InvSrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);

	return true;
}

//************************************************************************
// DestAlpha/SrcAlpha Test functions

CDestAlphaSrcAlphaTest::CDestAlphaSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: DestAlpha DestBlend: SrcAlpha");
	m_szCommandKey = TEXT("DestAlphaSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "SrcAlpha";
}

CDestAlphaSrcAlphaTest::~CDestAlphaSrcAlphaTest()
{
}

bool CDestAlphaSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestAlpha and Dest:SrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHA);

	return true;
}

//************************************************************************
// DestAlpha/InvSrcAlpha Test functions

CDestAlphaInvSrcAlphaTest::CDestAlphaInvSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: DestAlpha DestBlend: InvSrcAlpha");
	m_szCommandKey = TEXT("DestAlphaInvSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "InvSrcAlpha";
}

CDestAlphaInvSrcAlphaTest::~CDestAlphaInvSrcAlphaTest()
{
}

bool CDestAlphaInvSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestAlpha and Dest:InvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	return true;
}

//************************************************************************
// DestAlpha/DestAlpha Test functions

CDestAlphaDestAlphaTest::CDestAlphaDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: DestAlpha DestBlend: DestAlpha");
	m_szCommandKey = TEXT("DestAlphaDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "DestAlpha";
}

CDestAlphaDestAlphaTest::~CDestAlphaDestAlphaTest()
{
}

bool CDestAlphaDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestAlpha and Dest:DestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTALPHA);

	return true;
}

//************************************************************************
// DestAlpha/InvDestAlpha Test functions

CDestAlphaInvDestAlphaTest::CDestAlphaInvDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: DestAlpha DestBlend: InvDestAlpha");
	m_szCommandKey = TEXT("DestAlphaInvDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "InvDestAlpha";
}

CDestAlphaInvDestAlphaTest::~CDestAlphaInvDestAlphaTest()
{
}

bool CDestAlphaInvDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestAlpha and Dest:InvDestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTALPHA);

	return true;
}

//************************************************************************
// DestAlpha/DestColor Test functions

CDestAlphaDestColorTest::CDestAlphaDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: DestAlpha DestBlend: DestColor");
	m_szCommandKey = TEXT("DestAlphaDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "DestColor";
}

CDestAlphaDestColorTest::~CDestAlphaDestColorTest()
{
}

bool CDestAlphaDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestAlpha and Dest:DestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTCOLOR);

	return true;
}

//************************************************************************
// DestAlpha/InvDestColor Test functions

CDestAlphaInvDestColorTest::CDestAlphaInvDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: DestAlpha DestBlend: InvDestColor");
	m_szCommandKey = TEXT("DestAlphaInvDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "InvDestColor";
}

CDestAlphaInvDestColorTest::~CDestAlphaInvDestColorTest()
{
}

bool CDestAlphaInvDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestAlpha and Dest:InvDestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);

	return true;
}

//************************************************************************
// DestAlpha/SrcAlphaSat Test functions

CDestAlphaSrcAlphaSatTest::CDestAlphaSrcAlphaSatTest()
{
	m_szTestName = TEXT("SrcBlend: DestAlpha DestBlend: SrcAlphaSat");
	m_szCommandKey = TEXT("DestAlphaSrcAlphaSat");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "SrcAlphaSat";
}

CDestAlphaSrcAlphaSatTest::~CDestAlphaSrcAlphaSatTest()
{
}

bool CDestAlphaSrcAlphaSatTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestAlpha and Dest:SrcAlphaSat
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHASAT);

	return true;
}

