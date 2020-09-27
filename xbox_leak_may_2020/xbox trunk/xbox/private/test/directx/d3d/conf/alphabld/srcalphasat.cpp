//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "SrcAlphaSat.h"

//************************************************************************
// SrcAlphaSat/Zero Test functions

CSrcAlphaSatZeroTest::CSrcAlphaSatZeroTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlphaSat DestBlend: Zero");
	m_szCommandKey = TEXT("SrcAlphaSatZero");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_ZERO;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "Zero";
}

CSrcAlphaSatZeroTest::~CSrcAlphaSatZeroTest()
{
}

bool CSrcAlphaSatZeroTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlphaSat and Dest:Zero
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHASAT);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	return true;
}

//************************************************************************
// SrcAlphaSat/One Test functions

CSrcAlphaSatOneTest::CSrcAlphaSatOneTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlphaSat DestBlend: One");
	m_szCommandKey = TEXT("SrcAlphaSatOne");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_ONE;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "One";
}

CSrcAlphaSatOneTest::~CSrcAlphaSatOneTest()
{
}

bool CSrcAlphaSatOneTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlphaSat and Dest:One
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHASAT);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ONE);

	return true;
}

//************************************************************************
// SrcAlphaSat/SrcColor Test functions

CSrcAlphaSatSrcColorTest::CSrcAlphaSatSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlphaSat DestBlend: SrcColor");
	m_szCommandKey = TEXT("SrcAlphaSatSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_SRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "SrcColor";
}

CSrcAlphaSatSrcColorTest::~CSrcAlphaSatSrcColorTest()
{
}

bool CSrcAlphaSatSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlphaSat and Dest:SrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHASAT);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCCOLOR);

	return true;
}

//************************************************************************
// SrcAlphaSat/InvSrcColor Test functions

CSrcAlphaSatInvSrcColorTest::CSrcAlphaSatInvSrcColorTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlphaSat DestBlend: InvSrcColor");
	m_szCommandKey = TEXT("SrcAlphaSatInvSrcColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_INVSRCCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "InvSrcColor";
}

CSrcAlphaSatInvSrcColorTest::~CSrcAlphaSatInvSrcColorTest()
{
}

bool CSrcAlphaSatInvSrcColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlphaSat and Dest:InvSrcColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHASAT);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCCOLOR);

	return true;
}

//************************************************************************
// SrcAlphaSat/SrcAlpha Test functions

CSrcAlphaSatSrcAlphaTest::CSrcAlphaSatSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlphaSat DestBlend: SrcAlpha");
	m_szCommandKey = TEXT("SrcAlphaSatSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_SRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "SrcAlpha";
}

CSrcAlphaSatSrcAlphaTest::~CSrcAlphaSatSrcAlphaTest()
{
}

bool CSrcAlphaSatSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlphaSat and Dest:SrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHASAT);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHA);

	return true;
}

//************************************************************************
// SrcAlphaSat/InvSrcAlpha Test functions

CSrcAlphaSatInvSrcAlphaTest::CSrcAlphaSatInvSrcAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlphaSat DestBlend: InvSrcAlpha");
	m_szCommandKey = TEXT("SrcAlphaSatInvSrcAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_INVSRCALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "InvSrcAlpha";
}

CSrcAlphaSatInvSrcAlphaTest::~CSrcAlphaSatInvSrcAlphaTest()
{
}

bool CSrcAlphaSatInvSrcAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlphaSat and Dest:InvSrcAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHASAT);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

	return true;
}

//************************************************************************
// SrcAlphaSat/DestAlpha Test functions

CSrcAlphaSatDestAlphaTest::CSrcAlphaSatDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlphaSat DestBlend: DestAlpha");
	m_szCommandKey = TEXT("SrcAlphaSatDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_DESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "DestAlpha";
}

CSrcAlphaSatDestAlphaTest::~CSrcAlphaSatDestAlphaTest()
{
}

bool CSrcAlphaSatDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlphaSat and Dest:DestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHASAT);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTALPHA);

	return true;
}

//************************************************************************
// SrcAlphaSat/InvDestAlpha Test functions

CSrcAlphaSatInvDestAlphaTest::CSrcAlphaSatInvDestAlphaTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlphaSat DestBlend: InvDestAlpha");
	m_szCommandKey = TEXT("SrcAlphaSatInvDestAlpha");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "InvDestAlpha";
}

CSrcAlphaSatInvDestAlphaTest::~CSrcAlphaSatInvDestAlphaTest()
{
}

bool CSrcAlphaSatInvDestAlphaTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlphaSat and Dest:InvDestAlpha
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHASAT);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTALPHA);

	return true;
}

//************************************************************************
// SrcAlphaSat/DestColor Test functions

CSrcAlphaSatDestColorTest::CSrcAlphaSatDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlphaSat DestBlend: DestColor");
	m_szCommandKey = TEXT("SrcAlphaSatDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_DESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "DestColor";
}

CSrcAlphaSatDestColorTest::~CSrcAlphaSatDestColorTest()
{
}

bool CSrcAlphaSatDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlphaSat and Dest:DestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHASAT);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_DESTCOLOR);

	return true;
}

//************************************************************************
// SrcAlphaSat/InvDestColor Test functions

CSrcAlphaSatInvDestColorTest::CSrcAlphaSatInvDestColorTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlphaSat DestBlend: InvDestColor");
	m_szCommandKey = TEXT("SrcAlphaSatInvDestColor");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_INVDESTCOLOR;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "InvDestColor";
}

CSrcAlphaSatInvDestColorTest::~CSrcAlphaSatInvDestColorTest()
{
}

bool CSrcAlphaSatInvDestColorTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlphaSat and Dest:InvDestColor
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHASAT);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVDESTCOLOR);

	return true;
}

//************************************************************************
// SrcAlphaSat/SrcAlphaSat Test functions

CSrcAlphaSatSrcAlphaSatTest::CSrcAlphaSatSrcAlphaSatTest()
{
	m_szTestName = TEXT("SrcBlend: SrcAlphaSat DestBlend: SrcAlphaSat");
	m_szCommandKey = TEXT("SrcAlphaSatSrcAlphaSat");

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_SRCALPHASAT;
	dwDestCap = D3DPBLENDCAPS_SRCALPHASAT;

	// Give parent a printable name for the caps
	szSrcName = "SrcAlphaSat";
	szDestName = "SrcAlphaSat";
}

CSrcAlphaSatSrcAlphaSatTest::~CSrcAlphaSatSrcAlphaSatTest()
{
}

bool CSrcAlphaSatSrcAlphaSatTest::SetDefaultRenderStates(void)
{
	// Set blend modes to Src:SrcAlphaSat and Dest:SrcAlphaSat
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHASAT);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_SRCALPHASAT);

	return true;
}

