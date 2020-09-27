// ======================================================================================
// 
//  Groups.cpp -- group definitions for vblend.exe
//
// ======================================================================================

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "CShapes.h"
#include "VBlend.h"
#include "Groups.h"

// --------------------------------------------------------------------------------------

BlendPos::BlendPos()
{
	m_wFlags = BLEND_POS;
	m_szTestName = _T("Blend Positions");
	m_szCommandKey = _T("Pos");
}

BlendPos::~BlendPos()
{
}

bool BlendPos::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),

		D3DVSD_STREAM(1),
		D3DVSD_REG(1, D3DVSDT_FLOAT3),

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                                        \n"
		"mul r0, v0, c4.x      ; Pos1 * B                           \n"
		"mad r0, v1, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B            \n"
		"; transform                                                \n"
		"m4x4 r0, r0, c0                                            \n"
		"mov oPos, r0          ; write results to position register \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendNml::BlendNml()
{
	m_wFlags = BLEND_NML;
	m_szTestName = _T("Blend Normals");
	m_szCommandKey = _T("Nml");
}

BlendNml::~BlendNml()
{
}

bool BlendNml::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_SKIP(3),                  // skip pos info
		D3DVSD_REG(0, D3DVSDT_FLOAT3),

		D3DVSD_STREAM(1),
		D3DVSD_SKIP(3),                  // skip pos info
		D3DVSD_REG(1, D3DVSDT_FLOAT3),

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                               \n"
		"mul r0, v0, c4.x      ; Pos1 * B                  \n"
		"mad r0, v1, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"; transform                                       \n"
		"m4x4 r0, r0, c0                                   \n"
		"mov oPos, r0                                      \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendClr::BlendClr()
{
	m_wFlags = BLEND_CLR;
	m_szTestName = _T("Blend Colors");
	m_szCommandKey = _T("Clr");
}

BlendClr::~BlendClr()
{
}

bool BlendClr::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_SKIP(6),                  // skip pos, normal info
		D3DVSD_REG(0, D3DVSDT_PACKEDBYTE),

		D3DVSD_STREAM(1),
		D3DVSD_SKIP(6),                  // skip pos, normal info
		D3DVSD_REG(1, D3DVSDT_PACKEDBYTE),

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                                     \n"
		"mul r0, v0, c4.x      ; Pos1 * B                        \n"
		"mad r0, v1, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B         \n"
		"; transform                                             \n"
		"m4x4 r0, r0, c0                                         \n"
		"mov oD0, r0           ; write results to color register \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendTxc::BlendTxc()
{
	m_wFlags = BLEND_TXC;
	m_szTestName = _T("Blend Texture Coords");
	m_szCommandKey = _T("Txc");
}

BlendTxc::~BlendTxc()
{
}

bool BlendTxc::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_SKIP(7),                  // skip pos, normal, color info
		D3DVSD_REG(0, D3DVSDT_FLOAT2),

		D3DVSD_STREAM(1),
		D3DVSD_SKIP(7),                  // skip pos, normal, color info
		D3DVSD_REG(1, D3DVSDT_FLOAT2),

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                               \n"
		"mul r0, v0, c4.x      ; Pos1 * B                  \n"
		"mad r0, v1, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"; transform                                       \n"
		"m4x4 r0, r0, c0                                   \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendPosNml::BlendPosNml()
{
	m_wFlags = BLEND_POS | BLEND_NML;
	m_szTestName = _T("Blend Positions / Normals");
	m_szCommandKey = _T("PosNml");
}

BlendPosNml::~BlendPosNml()
{
}

bool BlendPosNml::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),  // pos
		D3DVSD_REG(1, D3DVSDT_FLOAT3),  // normal

		D3DVSD_STREAM(1),
		D3DVSD_REG(2, D3DVSDT_FLOAT3),  // pos
		D3DVSD_REG(3, D3DVSDT_FLOAT3),  // normal

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                               \n"
		"mul r0, v0, c4.x      ; Pos1 * B                  \n"
		"mad r0, v2, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"mul r1, v1, c4.x      ; Pos1 * B                  \n"
		"mad r1, v3, c4.y, r1  ; Pos2 * (1-B) + Pos1 * B   \n"
		"; transform                                       \n"
		"m4x4 r0, r0, c0                                   \n";
		"m4x4 r1, r1, c0                                   \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendPosClr::BlendPosClr()
{
	m_wFlags = BLEND_POS | BLEND_CLR;
	m_szTestName = _T("Blend Positions / Colors");
	m_szCommandKey = _T("PosClr");
}

BlendPosClr::~BlendPosClr()
{
}

bool BlendPosClr::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),  // pos
		D3DVSD_SKIP(3),                 // skip normal
		D3DVSD_REG(1, D3DVSDT_PACKEDBYTE),   // color

		D3DVSD_STREAM(1),
		D3DVSD_REG(2, D3DVSDT_FLOAT3),  // pos
		D3DVSD_SKIP(3),                 // skip normal
		D3DVSD_REG(3, D3DVSDT_PACKEDBYTE),   // color

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                               \n"
		"mul r0, v0, c4.x      ; Pos1 * B                  \n"
		"mad r0, v2, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"mul r0, v1, c4.x      ; Pos1 * B                  \n"
		"mad r0, v3, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"; transform                                       \n"
		"m4x4 r0, r0, c0                                   \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendPosTxc::BlendPosTxc()
{
	m_wFlags = BLEND_POS | BLEND_TXC;
	m_szTestName = _T("Blend Positions / Texture Coords");
	m_szCommandKey = _T("PosTxc");
}

BlendPosTxc::~BlendPosTxc()
{
}

bool BlendPosTxc::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),  // pos
		D3DVSD_SKIP(4),                 // skip normal, color
		D3DVSD_REG(1, D3DVSDT_FLOAT2),  // texture coords

		D3DVSD_STREAM(1),
		D3DVSD_REG(2, D3DVSDT_FLOAT3),  // pos
		D3DVSD_SKIP(4),                 // skip normal, color
		D3DVSD_REG(3, D3DVSDT_FLOAT2),  // texture coords

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                               \n"
		"mul r0, v0, c4.x      ; Pos1 * B                  \n"
		"mad r0, v2, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"mul r0, v1, c4.x      ; Pos1 * B                  \n"
		"mad r0, v3, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"; transform                                       \n"
		"m4x4 r0, r0, c0                                   \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendPosNmlClr::BlendPosNmlClr()
{
	m_wFlags = BLEND_POS | BLEND_NML | BLEND_CLR;
	m_szTestName = _T("Blend Positions / Normals / Colors");
	m_szCommandKey = _T("PosNmlClr");
}

BlendPosNmlClr::~BlendPosNmlClr()
{
}

bool BlendPosNmlClr::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),  // pos
		D3DVSD_REG(1, D3DVSDT_FLOAT3),  // normal
		D3DVSD_REG(2, D3DVSDT_PACKEDBYTE),   // color

		D3DVSD_STREAM(1),
		D3DVSD_REG(3, D3DVSDT_FLOAT3),  // pos
		D3DVSD_REG(4, D3DVSDT_FLOAT3),  // normal
		D3DVSD_REG(5, D3DVSDT_PACKEDBYTE),   // color

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                               \n"
		"mul r0, v0, c4.x      ; Pos1 * B                  \n"
		"mad r0, v3, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"mul r0, v1, c4.x      ; Pos1 * B                  \n"
		"mad r0, v4, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"mul r0, v2, c4.x      ; Pos1 * B                  \n"
		"mad r0, v5, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"; transform                                       \n"
		"m4x4 r0, r0, c0                                   \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendPosNmlTxc::BlendPosNmlTxc()
{
	m_wFlags = BLEND_POS | BLEND_NML | BLEND_TXC;
	m_szTestName = _T("Blend Positions / Normals / Texture Coords");
	m_szCommandKey = _T("PosNmlTxc");
}

BlendPosNmlTxc::~BlendPosNmlTxc()
{
}

bool BlendPosNmlTxc::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),  // pos
		D3DVSD_REG(1, D3DVSDT_FLOAT3),  // normal
		D3DVSD_SKIP(1),                 // skip color
		D3DVSD_REG(2, D3DVSDT_FLOAT2),  // texture coords

		D3DVSD_STREAM(1),
		D3DVSD_REG(3, D3DVSDT_FLOAT3),  // pos
		D3DVSD_REG(4, D3DVSDT_FLOAT3),  // normal
		D3DVSD_SKIP(1),                 // skip color
		D3DVSD_REG(5, D3DVSDT_FLOAT2),  // texture coords

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                               \n"
		"mul r0, v0, c4.x      ; Pos1 * B                  \n"
		"mad r0, v3, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"mul r0, v1, c4.x      ; Pos1 * B                  \n"
		"mad r0, v4, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"mul r0, v2, c4.x      ; Pos1 * B                  \n"
		"mad r0, v5, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"; transform                                       \n"
		"m4x4 r0, r0, c0                                   \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendPosNmlClrTxc::BlendPosNmlClrTxc()
{
	m_wFlags = BLEND_POS | BLEND_NML | BLEND_CLR | BLEND_TXC;
	m_szTestName = _T("Blend Positions / Normals / Colors / Texture Coords");
	m_szCommandKey = _T("PosNmlClrTxc");
}

BlendPosNmlClrTxc::~BlendPosNmlClrTxc()
{
}

bool BlendPosNmlClrTxc::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),  // pos
		D3DVSD_REG(1, D3DVSDT_FLOAT3),  // normal
		D3DVSD_REG(2, D3DVSDT_PACKEDBYTE),   // color
		D3DVSD_REG(3, D3DVSDT_FLOAT2),  // texture coords

		D3DVSD_STREAM(1),
		D3DVSD_REG(4, D3DVSDT_FLOAT3),  // pos
		D3DVSD_REG(5, D3DVSDT_FLOAT3),  // normal
		D3DVSD_REG(6, D3DVSDT_PACKEDBYTE),   // color
		D3DVSD_REG(7, D3DVSDT_FLOAT2),  // texture coords

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                               \n"
		"mul r0, v0, c4.x      ; Pos1 * B                  \n"
		"mad r0, v1, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"; transform                                       \n"
		"m4x4 r0, r0, c0                                   \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendNmlClr::BlendNmlClr()
{
	m_wFlags = BLEND_NML | BLEND_CLR;
	m_szTestName = _T("Blend Normals / Colors");
	m_szCommandKey = _T("NmlClr");
}

BlendNmlClr::~BlendNmlClr()
{
}

bool BlendNmlClr::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_SKIP(3),                 // skip pos
		D3DVSD_REG(0, D3DVSDT_FLOAT3),  // normal
		D3DVSD_REG(1, D3DVSDT_PACKEDBYTE),   // color

		D3DVSD_STREAM(1),
		D3DVSD_SKIP(3),                 // skip pos
		D3DVSD_REG(0, D3DVSDT_FLOAT3),  // normal
		D3DVSD_REG(1, D3DVSDT_PACKEDBYTE),   // color

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                               \n"
		"mul r0, v0, c4.x      ; Pos1 * B                  \n"
		"mad r0, v1, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"; transform                                       \n"
		"m4x4 r0, r0, c0                                   \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendNmlTxc::BlendNmlTxc()
{
	m_wFlags = BLEND_NML | BLEND_TXC;
	m_szTestName = _T("Blend Normals / Texture Coords");
	m_szCommandKey = _T("NmlTxc");
}

BlendNmlTxc::~BlendNmlTxc()
{
}

bool BlendNmlTxc::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_SKIP(3),                 // skip pos
		D3DVSD_REG(0, D3DVSDT_FLOAT3),  // normal
		D3DVSD_SKIP(1),                 // skip color
		D3DVSD_REG(1, D3DVSDT_FLOAT2),  // texture coords

		D3DVSD_STREAM(1),
		D3DVSD_SKIP(3),                 // skip pos
		D3DVSD_REG(0, D3DVSDT_FLOAT3),  // normal
		D3DVSD_SKIP(1),                 // skip color
		D3DVSD_REG(1, D3DVSDT_FLOAT2),  // texture coords

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                               \n"
		"mul r0, v0, c4.x      ; Pos1 * B                  \n"
		"mad r0, v1, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"; transform                                       \n"
		"m4x4 r0, r0, c0                                   \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendNmlClrTxc::BlendNmlClrTxc()
{
	m_wFlags = BLEND_NML | BLEND_CLR | BLEND_TXC;
	m_szTestName = _T("Blend Normals / Colors / Texture Coords");
	m_szCommandKey = _T("NmlClrTxc");
}

BlendNmlClrTxc::~BlendNmlClrTxc()
{
}

bool BlendNmlClrTxc::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_SKIP(3),                 // skip pos
		D3DVSD_REG(0, D3DVSDT_FLOAT3),  // normal
		D3DVSD_REG(1, D3DVSDT_PACKEDBYTE),   // color
		D3DVSD_REG(2, D3DVSDT_FLOAT2),  // texture coords

		D3DVSD_STREAM(1),
		D3DVSD_SKIP(3),                 // skip pos
		D3DVSD_REG(0, D3DVSDT_FLOAT3),  // normal
		D3DVSD_REG(1, D3DVSDT_PACKEDBYTE),   // color
		D3DVSD_REG(2, D3DVSDT_FLOAT2),  // texture coords

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                               \n"
		"mul r0, v0, c4.x      ; Pos1 * B                  \n"
		"mad r0, v1, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"; transform                                       \n"
		"m4x4 r0, r0, c0                                   \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

BlendClrTxc::BlendClrTxc()
{
	m_wFlags = BLEND_CLR | BLEND_TXC;
	m_szTestName = _T("Blend Colors / Texture Coords");
	m_szCommandKey = _T("ClrTxc");
}

BlendClrTxc::~BlendClrTxc()
{
}

bool BlendClrTxc::CreateShader(void)
{
	DWORD decl[] = {
		D3DVSD_STREAM(0),
		D3DVSD_SKIP(6),                 // skip pos, normal
		D3DVSD_REG(0, D3DVSDT_PACKEDBYTE),   // color
		D3DVSD_REG(1, D3DVSDT_FLOAT2),  // texture coords

		D3DVSD_STREAM(1),
		D3DVSD_SKIP(6),                 // skip pos, normal
		D3DVSD_REG(0, D3DVSDT_PACKEDBYTE),   // color
		D3DVSD_REG(1, D3DVSDT_FLOAT2),  // texture coords

		D3DVSD_END()
	};

	char pShader[] =
		"; do blending first                               \n"
		"mul r0, v0, c4.x      ; Pos1 * B                  \n"
		"mad r0, v1, c4.y, r0  ; Pos2 * (1-B) + Pos1 * B   \n"
		"; transform                                       \n"
		"m4x4 r0, r0, c0                                   \n";

	return SetupShader(decl, pShader);
}

// --------------------------------------------------------------------------------------

