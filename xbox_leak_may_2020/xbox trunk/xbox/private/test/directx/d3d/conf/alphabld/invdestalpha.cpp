//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "InvDestAlpha.h"

//************************************************************************
// InvDestAlpha/Zero Test functions

CInvDestAlphaZeroTest::CInvDestAlphaZeroTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestAlpha DestBlend: Zero");
	m_szCommandKey = TEXT("InvDestAlphaZero");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "Zero";
}

CInvDestAlphaZeroTest::~CInvDestAlphaZeroTest()
{
}

bool CInvDestAlphaZeroTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestAlpha and Dest:Zero
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	return true;
}

//************************************************************************
// InvDestAlpha/One Test functions

CInvDestAlphaOneTest::CInvDestAlphaOneTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestAlpha DestBlend: One");
	m_szCommandKey = TEXT("InvDestAlphaOne");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "One";
}

CInvDestAlphaOneTest::~CInvDestAlphaOneTest()
{
}

bool CInvDestAlphaOneTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestAlpha and Dest:One
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ONE);

	return true;
}

//************************************************************************
// InvDestAlpha/SrcColor Test functions

CInvDestAlphaSrcColorTest::CInvDestAlphaSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestAlpha DestBlend: SrcColor");
	m_szCommandKey = TEXT("InvDestAlphaSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "SrcColor";
}

CInvDestAlphaSrcColorTest::~CInvDestAlphaSrcColorTest()
{
}

bool CInvDestAlphaSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestAlpha and Dest:SrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCCOLOR);

	return true;
}

//************************************************************************
// InvDestAlpha/InvSrcColor Test functions

CInvDestAlphaInvSrcColorTest::CInvDestAlphaInvSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestAlpha DestBlend: InvSrcColor");
	m_szCommandKey = TEXT("InvDestAlphaInvSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "InvSrcColor";
}

CInvDestAlphaInvSrcColorTest::~CInvDestAlphaInvSrcColorTest()
{
}

bool CInvDestAlphaInvSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestAlpha and Dest:InvSrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);

	return true;
}

//************************************************************************
// InvDestAlpha/SrcAlpha Test functions

CInvDestAlphaSrcAlphaTest::CInvDestAlphaSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestAlpha DestBlend: SrcAlpha");
	m_szCommandKey = TEXT("InvDestAlphaSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "SrcAlpha";
}

CInvDestAlphaSrcAlphaTest::~CInvDestAlphaSrcAlphaTest()
{
}

bool CInvDestAlphaSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestAlpha and Dest:SrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHA);

	return true;
}

//************************************************************************
// InvDestAlpha/InvSrcAlpha Test functions

CInvDestAlphaInvSrcAlphaTest::CInvDestAlphaInvSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestAlpha DestBlend: InvSrcAlpha");
	m_szCommandKey = TEXT("InvDestAlphaInvSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "InvSrcAlpha";
}

CInvDestAlphaInvSrcAlphaTest::~CInvDestAlphaInvSrcAlphaTest()
{
}

bool CInvDestAlphaInvSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestAlpha and Dest:InvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	return true;
}

//************************************************************************
// InvDestAlpha/DestAlpha Test functions

CInvDestAlphaDestAlphaTest::CInvDestAlphaDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestAlpha DestBlend: DestAlpha");
	m_szCommandKey = TEXT("InvDestAlphaDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "DestAlpha";
}

CInvDestAlphaDestAlphaTest::~CInvDestAlphaDestAlphaTest()
{
}

bool CInvDestAlphaDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestAlpha and Dest:DestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTALPHA);

	return true;
}

//************************************************************************
// InvDestAlpha/InvDestAlpha Test functions

CInvDestAlphaInvDestAlphaTest::CInvDestAlphaInvDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestAlpha DestBlend: InvDestAlpha");
	m_szCommandKey = TEXT("InvDestAlphaInvDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "InvDestAlpha";
}

CInvDestAlphaInvDestAlphaTest::~CInvDestAlphaInvDestAlphaTest()
{
}

bool CInvDestAlphaInvDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestAlpha and Dest:InvDestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTALPHA);

	return true;
}

//************************************************************************
// InvDestAlpha/DestColor Test functions

CInvDestAlphaDestColorTest::CInvDestAlphaDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestAlpha DestBlend: DestColor");
	m_szCommandKey = TEXT("InvDestAlphaDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "DestColor";
}

CInvDestAlphaDestColorTest::~CInvDestAlphaDestColorTest()
{
}

bool CInvDestAlphaDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestAlpha and Dest:DestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTCOLOR);

	return true;
}

//************************************************************************
// InvDestAlpha/InvDestColor Test functions

CInvDestAlphaInvDestColorTest::CInvDestAlphaInvDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestAlpha DestBlend: InvDestColor");
	m_szCommandKey = TEXT("InvDestAlphaInvDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "InvDestColor";
}

CInvDestAlphaInvDestColorTest::~CInvDestAlphaInvDestColorTest()
{
}

bool CInvDestAlphaInvDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestAlpha and Dest:InvDestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);

	return true;
}

//************************************************************************
// InvDestAlpha/SrcAlphaSat Test functions

CInvDestAlphaSrcAlphaSatTest::CInvDestAlphaSrcAlphaSatTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestAlpha DestBlend: SrcAlphaSat");
	m_szCommandKey = TEXT("InvDestAlphaSrcAlphaSat");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "InvDestAlpha";
	szDestName = "SrcAlphaSat";
}

CInvDestAlphaSrcAlphaSatTest::~CInvDestAlphaSrcAlphaSatTest()
{
}

bool CInvDestAlphaSrcAlphaSatTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestAlpha and Dest:SrcAlphaSat
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHASAT);

	return true;
}

