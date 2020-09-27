//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "SrcAlpha.h"

//************************************************************************
// SrcAlpha/Zero Test functions

CSrcAlphaZeroTest::CSrcAlphaZeroTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlpha DestBlend: Zero");
	m_szCommandKey = TEXT("SrcAlphaZero");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "Zero";
}

CSrcAlphaZeroTest::~CSrcAlphaZeroTest()
{
}

bool CSrcAlphaZeroTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlpha and Dest:Zero
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	return true;
}

//************************************************************************
// SrcAlpha/One Test functions

CSrcAlphaOneTest::CSrcAlphaOneTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlpha DestBlend: One");
	m_szCommandKey = TEXT("SrcAlphaOne");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "One";
}

CSrcAlphaOneTest::~CSrcAlphaOneTest()
{
}

bool CSrcAlphaOneTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlpha and Dest:One
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ONE);

	return true;
}

//************************************************************************
// SrcAlpha/SrcColor Test functions

CSrcAlphaSrcColorTest::CSrcAlphaSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlpha DestBlend: SrcColor");
	m_szCommandKey = TEXT("SrcAlphaSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "SrcColor";
}

CSrcAlphaSrcColorTest::~CSrcAlphaSrcColorTest()
{
}

bool CSrcAlphaSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlpha and Dest:SrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCCOLOR);

	return true;
}

//************************************************************************
// SrcAlpha/InvSrcColor Test functions

CSrcAlphaInvSrcColorTest::CSrcAlphaInvSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlpha DestBlend: InvSrcColor");
	m_szCommandKey = TEXT("SrcAlphaInvSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "InvSrcColor";
}

CSrcAlphaInvSrcColorTest::~CSrcAlphaInvSrcColorTest()
{
}

bool CSrcAlphaInvSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlpha and Dest:InvSrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);

	return true;
}

//************************************************************************
// SrcAlpha/SrcAlpha Test functions

CSrcAlphaSrcAlphaTest::CSrcAlphaSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlpha DestBlend: SrcAlpha");
	m_szCommandKey = TEXT("SrcAlphaSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "SrcAlpha";
}

CSrcAlphaSrcAlphaTest::~CSrcAlphaSrcAlphaTest()
{
}

bool CSrcAlphaSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlpha and Dest:SrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHA);

	return true;
}

//************************************************************************
// SrcAlpha/InvSrcAlpha Test functions

CSrcAlphaInvSrcAlphaTest::CSrcAlphaInvSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlpha DestBlend: InvSrcAlpha");
	m_szCommandKey = TEXT("SrcAlphaInvSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "InvSrcAlpha";
}

CSrcAlphaInvSrcAlphaTest::~CSrcAlphaInvSrcAlphaTest()
{
}

bool CSrcAlphaInvSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlpha and Dest:InvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	return true;
}

//************************************************************************
// SrcAlpha/DestAlpha Test functions

CSrcAlphaDestAlphaTest::CSrcAlphaDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlpha DestBlend: DestAlpha");
	m_szCommandKey = TEXT("SrcAlphaDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "DestAlpha";
}

CSrcAlphaDestAlphaTest::~CSrcAlphaDestAlphaTest()
{
}

bool CSrcAlphaDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlpha and Dest:DestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTALPHA);

	return true;
}

//************************************************************************
// SrcAlpha/InvDestAlpha Test functions

CSrcAlphaInvDestAlphaTest::CSrcAlphaInvDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlpha DestBlend: InvDestAlpha");
	m_szCommandKey = TEXT("SrcAlphaInvDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "InvDestAlpha";
}

CSrcAlphaInvDestAlphaTest::~CSrcAlphaInvDestAlphaTest()
{
}

bool CSrcAlphaInvDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlpha and Dest:InvDestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTALPHA);

	return true;
}

//************************************************************************
// SrcAlpha/DestColor Test functions

CSrcAlphaDestColorTest::CSrcAlphaDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlpha DestBlend: DestColor");
	m_szCommandKey = TEXT("SrcAlphaDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "DestColor";
}

CSrcAlphaDestColorTest::~CSrcAlphaDestColorTest()
{
}

bool CSrcAlphaDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlpha and Dest:DestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTCOLOR);

	return true;
}

//************************************************************************
// SrcAlpha/InvDestColor Test functions

CSrcAlphaInvDestColorTest::CSrcAlphaInvDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlpha DestBlend: InvDestColor");
	m_szCommandKey = TEXT("SrcAlphaInvDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "InvDestColor";
}

CSrcAlphaInvDestColorTest::~CSrcAlphaInvDestColorTest()
{
}

bool CSrcAlphaInvDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlpha and Dest:InvDestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);

	return true;
}

//************************************************************************
// SrcAlpha/SrcAlphaSat Test functions

CSrcAlphaSrcAlphaSatTest::CSrcAlphaSrcAlphaSatTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlpha DestBlend: SrcAlphaSat");
	m_szCommandKey = TEXT("SrcAlphaSrcAlphaSat");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHA;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlpha";
	szDestName = "SrcAlphaSat";
}

CSrcAlphaSrcAlphaSatTest::~CSrcAlphaSrcAlphaSatTest()
{
}

bool CSrcAlphaSrcAlphaSatTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlpha and Dest:SrcAlphaSat
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHASAT);

	return true;
}

