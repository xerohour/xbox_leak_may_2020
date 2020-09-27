//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "SrcColor.h"

//************************************************************************
// SrcColor/Zero Test functions

CSrcColorZeroTest::CSrcColorZeroTest()
{
	m_szTestName = TEXT("SrcBlend: SrcColor DestBlend: Zero");
	m_szCommandKey = TEXT("SrcColorZero");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "Zero";
}

CSrcColorZeroTest::~CSrcColorZeroTest()
{
}

bool CSrcColorZeroTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcColor and Dest:Zero
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	return true;
}

//************************************************************************
// SrcColor/One Test functions

CSrcColorOneTest::CSrcColorOneTest()
{
	m_szTestName = TEXT("SrcBlend: SrcColor DestBlend: One");
	m_szCommandKey = TEXT("SrcColorOne");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "One";
}

CSrcColorOneTest::~CSrcColorOneTest()
{
}

bool CSrcColorOneTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcColor and Dest:One
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ONE);

	return true;
}

//************************************************************************
// SrcColor/SrcColor Test functions

CSrcColorSrcColorTest::CSrcColorSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: SrcColor DestBlend: SrcColor");
	m_szCommandKey = TEXT("SrcColorSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "SrcColor";
}

CSrcColorSrcColorTest::~CSrcColorSrcColorTest()
{
}

bool CSrcColorSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcColor and Dest:SrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCCOLOR);

	return true;
}

//************************************************************************
// SrcColor/InvSrcColor Test functions

CSrcColorInvSrcColorTest::CSrcColorInvSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: SrcColor DestBlend: InvSrcColor");
	m_szCommandKey = TEXT("SrcColorInvSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "InvSrcColor";
}

CSrcColorInvSrcColorTest::~CSrcColorInvSrcColorTest()
{
}

bool CSrcColorInvSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcColor and Dest:InvSrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);

	return true;
}

//************************************************************************
// SrcColor/SrcAlpha Test functions

CSrcColorSrcAlphaTest::CSrcColorSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: SrcColor DestBlend: SrcAlpha");
	m_szCommandKey = TEXT("SrcColorSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "SrcAlpha";
}

CSrcColorSrcAlphaTest::~CSrcColorSrcAlphaTest()
{
}

bool CSrcColorSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcColor and Dest:SrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHA);

	return true;
}

//************************************************************************
// SrcColor/InvSrcAlpha Test functions

CSrcColorInvSrcAlphaTest::CSrcColorInvSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: SrcColor DestBlend: InvSrcAlpha");
	m_szCommandKey = TEXT("SrcColorInvSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "InvSrcAlpha";
}

CSrcColorInvSrcAlphaTest::~CSrcColorInvSrcAlphaTest()
{
}

bool CSrcColorInvSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcColor and Dest:InvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	return true;
}

//************************************************************************
// SrcColor/DestAlpha Test functions

CSrcColorDestAlphaTest::CSrcColorDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: SrcColor DestBlend: DestAlpha");
	m_szCommandKey = TEXT("SrcColorDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "DestAlpha";
}

CSrcColorDestAlphaTest::~CSrcColorDestAlphaTest()
{
}

bool CSrcColorDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcColor and Dest:DestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTALPHA);

	return true;
}

//************************************************************************
// SrcColor/InvDestAlpha Test functions

CSrcColorInvDestAlphaTest::CSrcColorInvDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: SrcColor DestBlend: InvDestAlpha");
	m_szCommandKey = TEXT("SrcColorInvDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "InvDestAlpha";
}

CSrcColorInvDestAlphaTest::~CSrcColorInvDestAlphaTest()
{
}

bool CSrcColorInvDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcColor and Dest:InvDestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTALPHA);

	return true;
}

//************************************************************************
// SrcColor/DestColor Test functions

CSrcColorDestColorTest::CSrcColorDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: SrcColor DestBlend: DestColor");
	m_szCommandKey = TEXT("SrcColorDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "DestColor";
}

CSrcColorDestColorTest::~CSrcColorDestColorTest()
{
}

bool CSrcColorDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcColor and Dest:DestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTCOLOR);

	return true;
}

//************************************************************************
// SrcColor/InvDestColor Test functions

CSrcColorInvDestColorTest::CSrcColorInvDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: SrcColor DestBlend: InvDestColor");
	m_szCommandKey = TEXT("SrcColorInvDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "InvDestColor";
}

CSrcColorInvDestColorTest::~CSrcColorInvDestColorTest()
{
}

bool CSrcColorInvDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcColor and Dest:InvDestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);

	return true;
}

//************************************************************************
// SrcColor/SrcAlphaSat Test functions

CSrcColorSrcAlphaSatTest::CSrcColorSrcAlphaSatTest()
{
	m_szTestName = TEXT("SrcBlend: SrcColor DestBlend: SrcAlphaSat");
	m_szCommandKey = TEXT("SrcColorSrcAlphaSat");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCCOLOR;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "SrcColor";
	szDestName = "SrcAlphaSat";
}

CSrcColorSrcAlphaSatTest::~CSrcColorSrcAlphaSatTest()
{
}

bool CSrcColorSrcAlphaSatTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcColor and Dest:SrcAlphaSat
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCCOLOR);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHASAT);

	return true;
}

