//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "Zero.h"

//************************************************************************
// Zero/Zero Test functions

CZeroZeroTest::CZeroZeroTest()
{
	m_szTestName = TEXT("SrcBlend: Zero DestBlend: Zero");
	m_szCommandKey = TEXT("ZeroZero");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "Zero";
}

CZeroZeroTest::~CZeroZeroTest()
{
}

bool CZeroZeroTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:Zero and Dest:Zero
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	return true;
}

//************************************************************************
// Zero/One Test functions

CZeroOneTest::CZeroOneTest()
{
	m_szTestName = TEXT("SrcBlend: Zero DestBlend: One");
	m_szCommandKey = TEXT("ZeroOne");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "One";
}

CZeroOneTest::~CZeroOneTest()
{
}

bool CZeroOneTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:Zero and Dest:One
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ONE);

	return true;
}

//************************************************************************
// Zero/SrcColor Test functions

CZeroSrcColorTest::CZeroSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: Zero DestBlend: SrcColor");
	m_szCommandKey = TEXT("ZeroSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "SrcColor";
}

CZeroSrcColorTest::~CZeroSrcColorTest()
{
}

bool CZeroSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:Zero and Dest:SrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCCOLOR);

	return true;
}

//************************************************************************
// Zero/InvSrcColor Test functions

CZeroInvSrcColorTest::CZeroInvSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: Zero DestBlend: InvSrcColor");
	m_szCommandKey = TEXT("ZeroInvSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "InvSrcColor";
}

CZeroInvSrcColorTest::~CZeroInvSrcColorTest()
{
}

bool CZeroInvSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:Zero and Dest:InvSrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);

	return true;
}

//************************************************************************
// Zero/SrcAlpha Test functions

CZeroSrcAlphaTest::CZeroSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: Zero DestBlend: SrcAlpha");
	m_szCommandKey = TEXT("ZeroSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "SrcAlpha";
}

CZeroSrcAlphaTest::~CZeroSrcAlphaTest()
{
}

bool CZeroSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:Zero and Dest:SrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHA);

	return true;
}

//************************************************************************
// Zero/InvSrcAlpha Test functions

CZeroInvSrcAlphaTest::CZeroInvSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: Zero DestBlend: InvSrcAlpha");
	m_szCommandKey = TEXT("ZeroInvSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "InvSrcAlpha";
}

CZeroInvSrcAlphaTest::~CZeroInvSrcAlphaTest()
{
}

bool CZeroInvSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:Zero and Dest:InvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	return true;
}

//************************************************************************
// Zero/DestAlpha Test functions

CZeroDestAlphaTest::CZeroDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: Zero DestBlend: DestAlpha");
	m_szCommandKey = TEXT("ZeroDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "DestAlpha";
}

CZeroDestAlphaTest::~CZeroDestAlphaTest()
{
}

bool CZeroDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:Zero and Dest:DestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTALPHA);

	return true;
}

//************************************************************************
// Zero/InvDestAlpha Test functions

CZeroInvDestAlphaTest::CZeroInvDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: Zero DestBlend: InvDestAlpha");
	m_szCommandKey = TEXT("ZeroInvDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "InvDestAlpha";
}

CZeroInvDestAlphaTest::~CZeroInvDestAlphaTest()
{
}

bool CZeroInvDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:Zero and Dest:InvDestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTALPHA);

	return true;
}

//************************************************************************
// Zero/DestColor Test functions

CZeroDestColorTest::CZeroDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: Zero DestBlend: DestColor");
	m_szCommandKey = TEXT("ZeroDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "DestColor";
}

CZeroDestColorTest::~CZeroDestColorTest()
{
}

bool CZeroDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:Zero and Dest:DestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTCOLOR);

	return true;
}

//************************************************************************
// Zero/InvDestColor Test functions

CZeroInvDestColorTest::CZeroInvDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: Zero DestBlend: InvDestColor");
	m_szCommandKey = TEXT("ZeroInvDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "InvDestColor";
}

CZeroInvDestColorTest::~CZeroInvDestColorTest()
{
}

bool CZeroInvDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:Zero and Dest:InvDestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);

	return true;
}

//************************************************************************
// Zero/SrcAlphaSat Test functions

CZeroSrcAlphaSatTest::CZeroSrcAlphaSatTest()
{
	m_szTestName = TEXT("SrcBlend: Zero DestBlend: SrcAlphaSat");
	m_szCommandKey = TEXT("ZeroSrcAlphaSat");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_ZERO;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "Zero";
	szDestName = "SrcAlphaSat";
}

CZeroSrcAlphaSatTest::~CZeroSrcAlphaSatTest()
{
}

bool CZeroSrcAlphaSatTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:Zero and Dest:SrcAlphaSat
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHASAT);

	return true;
}

