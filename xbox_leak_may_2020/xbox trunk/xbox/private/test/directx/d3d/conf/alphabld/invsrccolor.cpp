//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "InvSrcColor.h"

//************************************************************************
// InvSrcColor/Zero Test functions

CInvSrcColorZeroTest::CInvSrcColorZeroTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcColor DestBlend: Zero");
	m_szCommandKey = TEXT("InvSrcColorZero");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "Zero";
}

CInvSrcColorZeroTest::~CInvSrcColorZeroTest()
{
}

bool CInvSrcColorZeroTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcColor and Dest:Zero
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	return true;
}

//************************************************************************
// InvSrcColor/One Test functions

CInvSrcColorOneTest::CInvSrcColorOneTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcColor DestBlend: One");
	m_szCommandKey = TEXT("InvSrcColorOne");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "One";
}

CInvSrcColorOneTest::~CInvSrcColorOneTest()
{
}

bool CInvSrcColorOneTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcColor and Dest:One
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ONE);

	return true;
}

//************************************************************************
// InvSrcColor/SrcColor Test functions

CInvSrcColorSrcColorTest::CInvSrcColorSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcColor DestBlend: SrcColor");
	m_szCommandKey = TEXT("InvSrcColorSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "SrcColor";
}

CInvSrcColorSrcColorTest::~CInvSrcColorSrcColorTest()
{
}

bool CInvSrcColorSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcColor and Dest:SrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCCOLOR);

	return true;
}

//************************************************************************
// InvSrcColor/InvSrcColor Test functions

CInvSrcColorInvSrcColorTest::CInvSrcColorInvSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcColor DestBlend: InvSrcColor");
	m_szCommandKey = TEXT("InvSrcColorInvSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "InvSrcColor";
}

CInvSrcColorInvSrcColorTest::~CInvSrcColorInvSrcColorTest()
{
}

bool CInvSrcColorInvSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcColor and Dest:InvSrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);

	return true;
}

//************************************************************************
// InvSrcColor/SrcAlpha Test functions

CInvSrcColorSrcAlphaTest::CInvSrcColorSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcColor DestBlend: SrcAlpha");
	m_szCommandKey = TEXT("InvSrcColorSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "SrcAlpha";
}

CInvSrcColorSrcAlphaTest::~CInvSrcColorSrcAlphaTest()
{
}

bool CInvSrcColorSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcColor and Dest:SrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHA);

	return true;
}

//************************************************************************
// InvSrcColor/InvSrcAlpha Test functions

CInvSrcColorInvSrcAlphaTest::CInvSrcColorInvSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcColor DestBlend: InvSrcAlpha");
	m_szCommandKey = TEXT("InvSrcColorInvSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "InvSrcAlpha";
}

CInvSrcColorInvSrcAlphaTest::~CInvSrcColorInvSrcAlphaTest()
{
}

bool CInvSrcColorInvSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcColor and Dest:InvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	return true;
}

//************************************************************************
// InvSrcColor/DestAlpha Test functions

CInvSrcColorDestAlphaTest::CInvSrcColorDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcColor DestBlend: DestAlpha");
	m_szCommandKey = TEXT("InvSrcColorDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "DestAlpha";
}

CInvSrcColorDestAlphaTest::~CInvSrcColorDestAlphaTest()
{
}

bool CInvSrcColorDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcColor and Dest:DestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTALPHA);

	return true;
}

//************************************************************************
// InvSrcColor/InvDestAlpha Test functions

CInvSrcColorInvDestAlphaTest::CInvSrcColorInvDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcColor DestBlend: InvDestAlpha");
	m_szCommandKey = TEXT("InvSrcColorInvDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "InvDestAlpha";
}

CInvSrcColorInvDestAlphaTest::~CInvSrcColorInvDestAlphaTest()
{
}

bool CInvSrcColorInvDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcColor and Dest:InvDestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTALPHA);

	return true;
}

//************************************************************************
// InvSrcColor/DestColor Test functions

CInvSrcColorDestColorTest::CInvSrcColorDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcColor DestBlend: DestColor");
	m_szCommandKey = TEXT("InvSrcColorDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "DestColor";
}

CInvSrcColorDestColorTest::~CInvSrcColorDestColorTest()
{
}

bool CInvSrcColorDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcColor and Dest:DestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTCOLOR);

	return true;
}

//************************************************************************
// InvSrcColor/InvDestColor Test functions

CInvSrcColorInvDestColorTest::CInvSrcColorInvDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcColor DestBlend: InvDestColor");
	m_szCommandKey = TEXT("InvSrcColorInvDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "InvDestColor";
}

CInvSrcColorInvDestColorTest::~CInvSrcColorInvDestColorTest()
{
}

bool CInvSrcColorInvDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcColor and Dest:InvDestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);

	return true;
}

//************************************************************************
// InvSrcColor/SrcAlphaSat Test functions

CInvSrcColorSrcAlphaSatTest::CInvSrcColorSrcAlphaSatTest()
{
	m_szTestName = TEXT("SrcBlend: InvSrcColor DestBlend: SrcAlphaSat");
	m_szCommandKey = TEXT("InvSrcColorSrcAlphaSat");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_INVSRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "InvSrcColor";
	szDestName = "SrcAlphaSat";
}

CInvSrcColorSrcAlphaSatTest::~CInvSrcColorSrcAlphaSatTest()
{
}

bool CInvSrcColorSrcAlphaSatTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:InvSrcColor and Dest:SrcAlphaSat
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHASAT);

	return true;
}

