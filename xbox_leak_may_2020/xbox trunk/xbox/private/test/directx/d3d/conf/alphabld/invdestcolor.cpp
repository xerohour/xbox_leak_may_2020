//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "InvDestColor.h"

//************************************************************************
// InvDestColor/Zero Test functions

CInvDestColorZeroTest::CInvDestColorZeroTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestColor DestBlend: Zero");
	m_szCommandKey = TEXT("InvDestColorZero");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "Zero";
}

CInvDestColorZeroTest::~CInvDestColorZeroTest()
{
}

bool CInvDestColorZeroTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestColor and Dest:Zero
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	return true;
}

//************************************************************************
// InvDestColor/One Test functions

CInvDestColorOneTest::CInvDestColorOneTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestColor DestBlend: One");
	m_szCommandKey = TEXT("InvDestColorOne");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "One";
}

CInvDestColorOneTest::~CInvDestColorOneTest()
{
}

bool CInvDestColorOneTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestColor and Dest:One
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ONE);

	return true;
}

//************************************************************************
// InvDestColor/SrcColor Test functions

CInvDestColorSrcColorTest::CInvDestColorSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestColor DestBlend: SrcColor");
	m_szCommandKey = TEXT("InvDestColorSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "SrcColor";
}

CInvDestColorSrcColorTest::~CInvDestColorSrcColorTest()
{
}

bool CInvDestColorSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestColor and Dest:SrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCCOLOR);

	return true;
}

//************************************************************************
// InvDestColor/InvSrcColor Test functions

CInvDestColorInvSrcColorTest::CInvDestColorInvSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestColor DestBlend: InvSrcColor");
	m_szCommandKey = TEXT("InvDestColorInvSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "InvSrcColor";
}

CInvDestColorInvSrcColorTest::~CInvDestColorInvSrcColorTest()
{
}

bool CInvDestColorInvSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestColor and Dest:InvSrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);

	return true;
}

//************************************************************************
// InvDestColor/SrcAlpha Test functions

CInvDestColorSrcAlphaTest::CInvDestColorSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestColor DestBlend: SrcAlpha");
	m_szCommandKey = TEXT("InvDestColorSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "SrcAlpha";
}

CInvDestColorSrcAlphaTest::~CInvDestColorSrcAlphaTest()
{
}

bool CInvDestColorSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestColor and Dest:SrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHA);

	return true;
}

//************************************************************************
// InvDestColor/InvSrcAlpha Test functions

CInvDestColorInvSrcAlphaTest::CInvDestColorInvSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestColor DestBlend: InvSrcAlpha");
	m_szCommandKey = TEXT("InvDestColorInvSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "InvSrcAlpha";
}

CInvDestColorInvSrcAlphaTest::~CInvDestColorInvSrcAlphaTest()
{
}

bool CInvDestColorInvSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestColor and Dest:InvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	return true;
}

//************************************************************************
// InvDestColor/DestAlpha Test functions

CInvDestColorDestAlphaTest::CInvDestColorDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestColor DestBlend: DestAlpha");
	m_szCommandKey = TEXT("InvDestColorDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "DestAlpha";
}

CInvDestColorDestAlphaTest::~CInvDestColorDestAlphaTest()
{
}

bool CInvDestColorDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestColor and Dest:DestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTALPHA);

	return true;
}

//************************************************************************
// InvDestColor/InvDestAlpha Test functions

CInvDestColorInvDestAlphaTest::CInvDestColorInvDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestColor DestBlend: InvDestAlpha");
	m_szCommandKey = TEXT("InvDestColorInvDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "InvDestAlpha";
}

CInvDestColorInvDestAlphaTest::~CInvDestColorInvDestAlphaTest()
{
}

bool CInvDestColorInvDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestColor and Dest:InvDestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTALPHA);

	return true;
}

//************************************************************************
// InvDestColor/DestColor Test functions

CInvDestColorDestColorTest::CInvDestColorDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestColor DestBlend: DestColor");
	m_szCommandKey = TEXT("InvDestColorDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "DestColor";
}

CInvDestColorDestColorTest::~CInvDestColorDestColorTest()
{
}

bool CInvDestColorDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestColor and Dest:DestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTCOLOR);

	return true;
}

//************************************************************************
// InvDestColor/InvDestColor Test functions

CInvDestColorInvDestColorTest::CInvDestColorInvDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestColor DestBlend: InvDestColor");
	m_szCommandKey = TEXT("InvDestColorInvDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "InvDestColor";
}

CInvDestColorInvDestColorTest::~CInvDestColorInvDestColorTest()
{
}

bool CInvDestColorInvDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestColor and Dest:InvDestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);

	return true;
}

//************************************************************************
// InvDestColor/SrcAlphaSat Test functions

CInvDestColorSrcAlphaSatTest::CInvDestColorSrcAlphaSatTest()
{
	m_szTestName = TEXT("SrcBlend: InvDestColor DestBlend: SrcAlphaSat");
	m_szCommandKey = TEXT("InvDestColorSrcAlphaSat");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVDESTCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "InvDestColor";
	szDestName = "SrcAlphaSat";
}

CInvDestColorSrcAlphaSatTest::~CInvDestColorSrcAlphaSatTest()
{
}

bool CInvDestColorSrcAlphaSatTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvDestColor and Dest:SrcAlphaSat
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHASAT);

	return true;
}

