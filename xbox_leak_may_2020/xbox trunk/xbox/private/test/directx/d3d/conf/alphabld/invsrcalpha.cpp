//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "InvSrcAlpha.h"

//************************************************************************
// InvSrcAlpha/Zero Test functions

CInvSrcAlphaZeroTest::CInvSrcAlphaZeroTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcAlpha DestBlend: Zero");
	m_szCommandKey = TEXT("InvSrcAlphaZero");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "Zero";
}

CInvSrcAlphaZeroTest::~CInvSrcAlphaZeroTest()
{
}

bool CInvSrcAlphaZeroTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcAlpha and Dest:Zero
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	return true;
}

//************************************************************************
// InvSrcAlpha/One Test functions

CInvSrcAlphaOneTest::CInvSrcAlphaOneTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcAlpha DestBlend: One");
	m_szCommandKey = TEXT("InvSrcAlphaOne");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "One";
}

CInvSrcAlphaOneTest::~CInvSrcAlphaOneTest()
{
}

bool CInvSrcAlphaOneTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcAlpha and Dest:One
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ONE);

	return true;
}

//************************************************************************
// InvSrcAlpha/SrcColor Test functions

CInvSrcAlphaSrcColorTest::CInvSrcAlphaSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcAlpha DestBlend: SrcColor");
	m_szCommandKey = TEXT("InvSrcAlphaSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "SrcColor";
}

CInvSrcAlphaSrcColorTest::~CInvSrcAlphaSrcColorTest()
{
}

bool CInvSrcAlphaSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcAlpha and Dest:SrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCCOLOR);

	return true;
}

//************************************************************************
// InvSrcAlpha/InvSrcColor Test functions

CInvSrcAlphaInvSrcColorTest::CInvSrcAlphaInvSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcAlpha DestBlend: InvSrcColor");
	m_szCommandKey = TEXT("InvSrcAlphaInvSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "InvSrcColor";
}

CInvSrcAlphaInvSrcColorTest::~CInvSrcAlphaInvSrcColorTest()
{
}

bool CInvSrcAlphaInvSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcAlpha and Dest:InvSrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);

	return true;
}

//************************************************************************
// InvSrcAlpha/SrcAlpha Test functions

CInvSrcAlphaSrcAlphaTest::CInvSrcAlphaSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcAlpha DestBlend: SrcAlpha");
	m_szCommandKey = TEXT("InvSrcAlphaSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "SrcAlpha";
}

CInvSrcAlphaSrcAlphaTest::~CInvSrcAlphaSrcAlphaTest()
{
}

bool CInvSrcAlphaSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcAlpha and Dest:SrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHA);

	return true;
}

//************************************************************************
// InvSrcAlpha/InvSrcAlpha Test functions

CInvSrcAlphaInvSrcAlphaTest::CInvSrcAlphaInvSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcAlpha DestBlend: InvSrcAlpha");
	m_szCommandKey = TEXT("InvSrcAlphaInvSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "InvSrcAlpha";
}

CInvSrcAlphaInvSrcAlphaTest::~CInvSrcAlphaInvSrcAlphaTest()
{
}

bool CInvSrcAlphaInvSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcAlpha and Dest:InvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	return true;
}

//************************************************************************
// InvSrcAlpha/DestAlpha Test functions

CInvSrcAlphaDestAlphaTest::CInvSrcAlphaDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcAlpha DestBlend: DestAlpha");
	m_szCommandKey = TEXT("InvSrcAlphaDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "DestAlpha";
}

CInvSrcAlphaDestAlphaTest::~CInvSrcAlphaDestAlphaTest()
{
}

bool CInvSrcAlphaDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcAlpha and Dest:DestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTALPHA);

	return true;
}

//************************************************************************
// InvSrcAlpha/InvDestAlpha Test functions

CInvSrcAlphaInvDestAlphaTest::CInvSrcAlphaInvDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcAlpha DestBlend: InvDestAlpha");
	m_szCommandKey = TEXT("InvSrcAlphaInvDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "InvDestAlpha";
}

CInvSrcAlphaInvDestAlphaTest::~CInvSrcAlphaInvDestAlphaTest()
{
}

bool CInvSrcAlphaInvDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcAlpha and Dest:InvDestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTALPHA);

	return true;
}

//************************************************************************
// InvSrcAlpha/DestColor Test functions

CInvSrcAlphaDestColorTest::CInvSrcAlphaDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcAlpha DestBlend: DestColor");
	m_szCommandKey = TEXT("InvSrcAlphaDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "DestColor";
}

CInvSrcAlphaDestColorTest::~CInvSrcAlphaDestColorTest()
{
}

bool CInvSrcAlphaDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcAlpha and Dest:DestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTCOLOR);

	return true;
}

//************************************************************************
// InvSrcAlpha/InvDestColor Test functions

CInvSrcAlphaInvDestColorTest::CInvSrcAlphaInvDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcAlpha DestBlend: InvDestColor");
	m_szCommandKey = TEXT("InvSrcAlphaInvDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "InvDestColor";
}

CInvSrcAlphaInvDestColorTest::~CInvSrcAlphaInvDestColorTest()
{
}

bool CInvSrcAlphaInvDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcAlpha and Dest:InvDestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);

	return true;
}

//************************************************************************
// InvSrcAlpha/SrcAlphaSat Test functions

CInvSrcAlphaSrcAlphaSatTest::CInvSrcAlphaSrcAlphaSatTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcAlpha DestBlend: SrcAlphaSat");
	m_szCommandKey = TEXT("InvSrcAlphaSrcAlphaSat");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcAlpha";
	szDestName = "SrcAlphaSat";
}

CInvSrcAlphaSrcAlphaSatTest::~CInvSrcAlphaSrcAlphaSatTest()
{
}

bool CInvSrcAlphaSrcAlphaSatTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcAlpha and Dest:SrcAlphaSat
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHASAT);

	return true;
}

