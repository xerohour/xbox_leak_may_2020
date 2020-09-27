//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "Address.h"

//************************************************************************
// WrapMirror Test functions

CWrapMirrorTest::CWrapMirrorTest()
{
	m_szTestName = TEXT("Texture Address WrapU MirrorV");
	m_szCommandKey = TEXT("WrapMirror");

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_WRAP;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRROR;

	// Give parent a printable name for the caps
	szAddrU = TEXT("WrapU");
	szAddrV = TEXT("MirrorV");
}

CWrapMirrorTest::~CWrapMirrorTest()
{
}

bool CWrapMirrorTest::SetDefaultRenderStates(void)
{
	// Turn on WrapU MirrorV addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_MIRROR);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_WRAP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_MIRROR);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

//************************************************************************
// WrapClamp Test functions

CWrapClampTest::CWrapClampTest()
{
	m_szTestName = TEXT("Texture Address WrapU ClampV");
	m_szCommandKey = TEXT("WrapClamp");

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_WRAP;
	dwAddressCapV = D3DPTADDRESSCAPS_CLAMP;

	// Give parent a printable name for the caps
	szAddrU = TEXT("WrapU");
	szAddrV = TEXT("ClampV");
}

CWrapClampTest::~CWrapClampTest()
{
}

bool CWrapClampTest::SetDefaultRenderStates(void)
{
	// Turn on WrapU ClampV addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_WRAP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

//************************************************************************
// WrapBorder Test functions

CWrapBorderTest::CWrapBorderTest()
{
	m_szTestName = TEXT("Texture Address WrapU BorderV");
	m_szCommandKey = TEXT("WrapBorder");

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_WRAP;
	dwAddressCapV = D3DPTADDRESSCAPS_BORDER;

	// Give parent a printable name for the caps
	szAddrU = TEXT("WrapU");
	szAddrV = TEXT("BorderV");
}

CWrapBorderTest::~CWrapBorderTest()
{
}

bool CWrapBorderTest::SetDefaultRenderStates(void)
{
	// Turn on WrapU BorderV addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_WRAP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_BORDER);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

//************************************************************************
// MirrorWrap Test functions

CMirrorWrapTest::CMirrorWrapTest()
{
	m_szTestName = TEXT("Texture Address MirrorU WrapV");
	m_szCommandKey = TEXT("MirrorWrap");

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRROR;
	dwAddressCapV = D3DPTADDRESSCAPS_WRAP;

	// Give parent a printable name for the caps
	szAddrU = TEXT("MirrorU");
	szAddrV = TEXT("WrapV");
}

CMirrorWrapTest::~CMirrorWrapTest()
{
}

bool CMirrorWrapTest::SetDefaultRenderStates(void)
{
	// Turn on WrapU MirrorV addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_MIRROR);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_WRAP);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

//************************************************************************
// MirrorClamp Test functions

CMirrorClampTest::CMirrorClampTest()
{
	m_szTestName = TEXT("Texture Address MirrorU ClampV");
	m_szCommandKey = TEXT("MirrorClamp");

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRROR;
	dwAddressCapV = D3DPTADDRESSCAPS_CLAMP;

	// Give parent a printable name for the caps
	szAddrU = TEXT("MirrorU");
	szAddrV = TEXT("ClampV");
}

CMirrorClampTest::~CMirrorClampTest()
{
}

bool CMirrorClampTest::SetDefaultRenderStates(void)
{
	// Turn on WrapU ClampV addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_MIRROR);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

//************************************************************************
// MirrorBorder Test functions

CMirrorBorderTest::CMirrorBorderTest()
{
	m_szTestName = TEXT("Texture Address MirrorU BorderV");
	m_szCommandKey = TEXT("MirrorBorder");

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_MIRROR;
	dwAddressCapV = D3DPTADDRESSCAPS_BORDER;

	// Give parent a printable name for the caps
	szAddrU = TEXT("MirrorU");
	szAddrV = TEXT("BorderV");
}

CMirrorBorderTest::~CMirrorBorderTest()
{
}

bool CMirrorBorderTest::SetDefaultRenderStates(void)
{
	// Turn on WrapU BorderV addressingaddressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_MIRROR);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_BORDER);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

//************************************************************************
// ClampWrap Test functions

CClampWrapTest::CClampWrapTest()
{
	m_szTestName = TEXT("Texture Address ClampU WrapV");
	m_szCommandKey = TEXT("ClampWrap");

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_CLAMP;
	dwAddressCapV = D3DPTADDRESSCAPS_WRAP;

	// Give parent a printable name for the caps
	szAddrU = TEXT("ClampU");
	szAddrV = TEXT("WrapV");
}

CClampWrapTest::~CClampWrapTest()
{
}

bool CClampWrapTest::SetDefaultRenderStates(void)
{
	// Turn on WrapU MirrorV addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_WRAP);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

//************************************************************************
// ClampMirror Test functions

CClampMirrorTest::CClampMirrorTest()
{
	m_szTestName = TEXT("Texture Address ClampU MirrorV");
	m_szCommandKey = TEXT("ClampMirror");

	// Tell parent to check indpendent address modes
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_CLAMP;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRROR;

	// Give parent a printable name for the caps
	szAddrU = TEXT("ClampU");
	szAddrV = TEXT("MirrorV");
}

CClampMirrorTest::~CClampMirrorTest()
{
}

bool CClampMirrorTest::SetDefaultRenderStates(void)
{
	// Turn on WrapU ClampV addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_MIRROR);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_MIRROR);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

//************************************************************************
// ClampBorder Test functions

CClampBorderTest::CClampBorderTest()
{
	m_szTestName = TEXT("Texture Address ClampU BorderV");
	m_szCommandKey = TEXT("ClampBorder");

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_CLAMP;
	dwAddressCapV = D3DPTADDRESSCAPS_BORDER;

	// Give parent a printable name for the caps
	szAddrU = TEXT("ClampU");
	szAddrV = TEXT("BorderV");
}

CClampBorderTest::~CClampBorderTest()
{
}

bool CClampBorderTest::SetDefaultRenderStates(void)
{
	// Turn on WrapU BorderV addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_BORDER);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

//************************************************************************
// BorderWrap Test functions

CBorderWrapTest::CBorderWrapTest()
{
	m_szTestName = TEXT("Texture Address BorderU WrapV");
	m_szCommandKey = TEXT("BorderWrap");

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_BORDER;
	dwAddressCapV = D3DPTADDRESSCAPS_WRAP;

	// Give parent a printable name for the caps
	szAddrU = TEXT("BorderU");
	szAddrV = TEXT("WrapV");
}

CBorderWrapTest::~CBorderWrapTest()
{
}

bool CBorderWrapTest::SetDefaultRenderStates(void)
{
	// Turn on WrapU MirrorV addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_BORDER);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_WRAP);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

//************************************************************************
// BorderMirror Test functions

CBorderMirrorTest::CBorderMirrorTest()
{
	m_szTestName = TEXT("Texture Address BorderU MirrorV");
	m_szCommandKey = TEXT("BorderMirror");

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_BORDER;
	dwAddressCapV = D3DPTADDRESSCAPS_MIRROR;

	// Give parent a printable name for the caps
	szAddrU = TEXT("BorderU");
	szAddrV = TEXT("MirrorV");
}

CBorderMirrorTest::~CBorderMirrorTest()
{
}

bool CBorderMirrorTest::SetDefaultRenderStates(void)
{
	// Turn on WrapU ClampV addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_MIRROR);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_BORDER);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_MIRROR);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

//************************************************************************
// BorderClamp Test functions

CBorderClampTest::CBorderClampTest()
{
	m_szTestName = TEXT("Texture Address BorderU ClampV");
	m_szCommandKey = TEXT("BorderClamp");

	// Tell parent to use border colors and check indpendent address modes
	bBorder = true;
	bIndependentUV = true;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_BORDER;
	dwAddressCapV = D3DPTADDRESSCAPS_CLAMP;

	// Give parent a printable name for the caps
	szAddrU = TEXT("BorderU");
	szAddrV = TEXT("ClampV");
}

CBorderClampTest::~CBorderClampTest()
{
}

bool CBorderClampTest::SetDefaultRenderStates(void)
{
	// Turn on WrapU BorderV addressing
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, (DWORD)D3DTADDRESS_BORDER);
//	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
    SetRenderState(D3DRS_WRAP0, 0);

	return true;
}

