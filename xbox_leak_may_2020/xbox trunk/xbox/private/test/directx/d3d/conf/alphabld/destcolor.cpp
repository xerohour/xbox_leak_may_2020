//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "DestColor.h"

//************************************************************************
// DestColor/Zero Test functions

CDestColorZeroTest::CDestColorZeroTest()
{
	m_szTestName = TEXT("SrcBlend: DestColor DestBlend: Zero");
	m_szCommandKey = TEXT("DestColorZero");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "Zero";
}

CDestColorZeroTest::~CDestColorZeroTest()
{
}

bool CDestColorZeroTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestColor and Dest:Zero
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	return true;
}

//************************************************************************
// DestColor/One Test functions

CDestColorOneTest::CDestColorOneTest()
{
	m_szTestName = TEXT("SrcBlend: DestColor DestBlend: One");
	m_szCommandKey = TEXT("DestColorOne");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "One";
}

CDestColorOneTest::~CDestColorOneTest()
{
}

bool CDestColorOneTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestColor and Dest:One
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ONE);

	return true;
}

//************************************************************************
// DestColor/SrcColor Test functions

CDestColorSrcColorTest::CDestColorSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: DestColor DestBlend: SrcColor");
	m_szCommandKey = TEXT("DestColorSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "SrcColor";
}

CDestColorSrcColorTest::~CDestColorSrcColorTest()
{
}

bool CDestColorSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestColor and Dest:SrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCCOLOR);

	return true;
}

//************************************************************************
// DestColor/InvSrcColor Test functions

CDestColorInvSrcColorTest::CDestColorInvSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: DestColor DestBlend: InvSrcColor");
	m_szCommandKey = TEXT("DestColorInvSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "InvSrcColor";
}

CDestColorInvSrcColorTest::~CDestColorInvSrcColorTest()
{
}

bool CDestColorInvSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestColor and Dest:InvSrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);

	return true;
}

//************************************************************************
// DestColor/SrcAlpha Test functions

CDestColorSrcAlphaTest::CDestColorSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: DestColor DestBlend: SrcAlpha");
	m_szCommandKey = TEXT("DestColorSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "SrcAlpha";
}

CDestColorSrcAlphaTest::~CDestColorSrcAlphaTest()
{
}

bool CDestColorSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestColor and Dest:SrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHA);

	return true;
}

//************************************************************************
// DestColor/InvSrcAlpha Test functions

CDestColorInvSrcAlphaTest::CDestColorInvSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: DestColor DestBlend: InvSrcAlpha");
	m_szCommandKey = TEXT("DestColorInvSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "InvSrcAlpha";
}

CDestColorInvSrcAlphaTest::~CDestColorInvSrcAlphaTest()
{
}

bool CDestColorInvSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestColor and Dest:InvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	return true;
}

//************************************************************************
// DestColor/DestAlpha Test functions

CDestColorDestAlphaTest::CDestColorDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: DestColor DestBlend: DestAlpha");
	m_szCommandKey = TEXT("DestColorDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "DestAlpha";
}

CDestColorDestAlphaTest::~CDestColorDestAlphaTest()
{
}

bool CDestColorDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestColor and Dest:DestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTALPHA);

	return true;
}

//************************************************************************
// DestColor/InvDestAlpha Test functions

CDestColorInvDestAlphaTest::CDestColorInvDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: DestColor DestBlend: InvDestAlpha");
	m_szCommandKey = TEXT("DestColorInvDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "InvDestAlpha";
}

CDestColorInvDestAlphaTest::~CDestColorInvDestAlphaTest()
{
}

bool CDestColorInvDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestColor and Dest:InvDestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTALPHA);

	return true;
}

//************************************************************************
// DestColor/DestColor Test functions

CDestColorDestColorTest::CDestColorDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: DestColor DestBlend: DestColor");
	m_szCommandKey = TEXT("DestColorDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "DestColor";
}

CDestColorDestColorTest::~CDestColorDestColorTest()
{
}

bool CDestColorDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestColor and Dest:DestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTCOLOR);

	return true;
}

//************************************************************************
// DestColor/InvDestColor Test functions

CDestColorInvDestColorTest::CDestColorInvDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: DestColor DestBlend: InvDestColor");
	m_szCommandKey = TEXT("DestColorInvDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "InvDestColor";
}

CDestColorInvDestColorTest::~CDestColorInvDestColorTest()
{
}

bool CDestColorInvDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestColor and Dest:InvDestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);

	return true;
}

//************************************************************************
// DestColor/SrcAlphaSat Test functions

CDestColorSrcAlphaSatTest::CDestColorSrcAlphaSatTest()
{
	m_szTestName = TEXT("SrcBlend: DestColor DestBlend: SrcAlphaSat");
	m_szCommandKey = TEXT("DestColorSrcAlphaSat");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "DestColor";
	szDestName = "SrcAlphaSat";
}

CDestColorSrcAlphaSatTest::~CDestColorSrcAlphaSatTest()
{
}

bool CDestColorSrcAlphaSatTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:DestColor and Dest:SrcAlphaSat
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_DESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHASAT);

	return true;
}

