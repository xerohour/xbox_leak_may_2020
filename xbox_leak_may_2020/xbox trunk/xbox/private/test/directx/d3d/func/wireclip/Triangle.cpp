#include "d3dlocus.h"
#include "cd3dtest.h"
#include "dxlegacy.h"
#include "3dmath.h"
#include "Clipping.h"



D3DVERTEX	WTriangle::m_pTriangleVertices[24] = {
	D3DVERTEX(cD3DVECTOR( 0.00f, 900.f, 0.f), Normalize(cD3DVECTOR( 0.f, 1.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR(-0.25f,-0.25f, 0.f), Normalize(cD3DVECTOR( 1.f, 0.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR( 0.25f,-0.25f, 0.f), Normalize(cD3DVECTOR( 1.f, 0.f, 1.f)), 0.f, 0.f),

	D3DVERTEX(cD3DVECTOR( 0.25f,-0.25f, 0.f), Normalize(cD3DVECTOR( 0.f, 1.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR(-0.25f,-0.25f, 0.f), Normalize(cD3DVECTOR( 1.f, 0.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR( 0.65f,-0.65f, 0.f), Normalize(cD3DVECTOR(-1.f, 0.f, 1.f)), 0.f, 0.f),

	D3DVERTEX(cD3DVECTOR(-0.25f,-0.25f, 0.f), Normalize(cD3DVECTOR( 0.f, 1.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR(-0.65f,-0.65f, 0.f), Normalize(cD3DVECTOR(-1.f, 0.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR( 0.25f,-0.25f, 0.f), Normalize(cD3DVECTOR( 1.f, 0.f, 1.f)), 0.f, 0.f),

	D3DVERTEX(cD3DVECTOR( 0.00f, 0.30f, 0.f), Normalize(cD3DVECTOR( 0.f, 1.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR(-0.65f,-0.65f, 0.f), Normalize(cD3DVECTOR(-1.f, 0.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR( 0.65f,-0.65f, 0.f), Normalize(cD3DVECTOR( 1.f, 0.f, 1.f)), 0.f, 0.f),

	D3DVERTEX(cD3DVECTOR( 0.00f, 0.95f, 0.f), Normalize(cD3DVECTOR( 0.f, 1.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR(-0.25f,-0.25f, 0.f), Normalize(cD3DVECTOR( 1.f, 0.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR( 0.25f,-0.25f, 0.f), Normalize(cD3DVECTOR( 1.f, 0.f, 1.f)), 0.f, 0.f),

	D3DVERTEX(cD3DVECTOR( 0.00f, 0.95f, 0.f), Normalize(cD3DVECTOR( 0.f, 1.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR( 0.00f, 0.30f, 0.f), Normalize(cD3DVECTOR( 1.f, 0.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR( 0.65f,-0.65f, 0.f), Normalize(cD3DVECTOR(-1.f, 0.f, 1.f)), 0.f, 0.f),

	D3DVERTEX(cD3DVECTOR( 0.00f, 0.95f, 0.f), Normalize(cD3DVECTOR( 0.f, 1.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR(-0.65f,-0.65f, 0.f), Normalize(cD3DVECTOR(-1.f, 0.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR( 0.00f, 0.30f, 0.f), Normalize(cD3DVECTOR( 1.f, 0.f, 1.f)), 0.f, 0.f),

	D3DVERTEX(cD3DVECTOR( 0.00f, 0.95f, 0.f), Normalize(cD3DVECTOR( 0.f, 1.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR(-0.65f,-0.65f, 0.f), Normalize(cD3DVECTOR(-1.f, 0.f, 1.f)), 0.f, 0.f),
	D3DVERTEX(cD3DVECTOR( 0.65f,-0.65f, 0.f), Normalize(cD3DVECTOR( 1.f, 0.f, 1.f)), 0.f, 0.f),
};


WTriangle::WTriangle()
{
    m_szTestName = TEXT("Triangle clipping");
    m_szCommandKey = TEXT("triangle");
    
#ifdef TESTTYPE_WIREFRAME
    m_dwVertices = 24;
#else
    m_dwVertices = 3;
#endif
    
    m_pRenderVertices = NULL;
}

WTriangle::~WTriangle()
{
    if (m_pRenderVertices)
        delete [] m_pRenderVertices;
}

UINT WTriangle::TestInitialize()
{
    m_pRenderVertices = new BYTE [(3 + 3 + 8*2)*sizeof(float)*m_dwVertices];
    if (m_pRenderVertices == 0)
    {
        WriteToLog("%s: Can't allocate memory for vertices\n", m_szTestName);
        return D3DTESTINIT_ABORT;
    }

    // Calculate total tests
    if (0x0600 == m_dwVersion)
    {
        // Total tests:     
        // -triangle:       8
        // -API:            m_dwRPFullDX6APIListSize (14) * frequency (2)
        // -FVF:            2
        // -properties:     7
        // -primitive type: 3
        m_dwTotalAPIs = m_dwRPFullDX6APIListSize * 2;
#ifdef TESTTYPE_WIREFRAME
        m_dwTotalTests = m_dwTotalAPIs * 2 * 7 * 3; // (9408)
#else
        m_dwTotalTests = 8 * m_dwTotalAPIs * 2 * 7 * 3; // (9408)
#endif

        SetAPI(m_pRPFullDX6APIList, m_dwRPFullDX6APIListSize, 2*8);
    }
    else if (0x0700 == m_dwVersion)
    {
        // Total tests:     
        // -triangle:       8
        // -API:            m_dwRPFullDX7APIListSize (14) * frequency (2) 
        // -FVF:            2
        // -properties:     7
        // -primitive type: 3
        m_dwTotalAPIs = m_dwRPFullDX7APIListSize * 2;
#ifdef TESTTYPE_WIREFRAME
        m_dwTotalTests = m_dwTotalAPIs * 2 * 7 * 3; // (9408)
#else
        m_dwTotalTests = 8 * m_dwTotalAPIs * 2 * 7 * 3; // (9408)
#endif

        SetAPI(m_pRPFullDX7APIList, m_dwRPFullDX7APIListSize, 2*8);
    }
    else if (0x0800 == m_dwVersion)
    {
        // Total tests:     
        // -triangle:       8
        // -API:            m_dwRPFullDX7APIListSize (?) * frequency (2) 
        // -FVF:            2
        // -properties:     7
        // -primitive type: 3
        m_dwTotalAPIs = m_dwRPFullDX8APIListSize * 2;
#ifdef TESTTYPE_WIREFRAME
        m_dwTotalTests = m_dwTotalAPIs * 2 * 7 * 3; // (?)
#else
        m_dwTotalTests = 8 * m_dwTotalAPIs * 2 * 7 * 3; // (?)
#endif
        SetAPI(m_pRPFullDX8APIList, m_dwRPFullDX8APIListSize, 2*8);
    }
    else
    {
        WriteToLog("%s: Incorrect DirectX version\n", m_szTestName);
    }

    return WireClip::TestInitialize();
}

bool WTriangle::TestTerminate(void)
{
    if (m_pRenderVertices)
    {
        delete [] m_pRenderVertices;
        m_pRenderVertices = NULL;
    }
    return WireClip::TestTerminate();
}


bool WTriangle::ResolveTestNumber()
{
    DWORD   dwCurrentPrimitiveType;
    DWORD   num = m_dwCurrentTestNumber - 1;

#ifndef TESTTYPE_WIREFRAME
    m_dwCurrentTriangle = num % 8;          num /= 8;
#endif
    m_dwCurrentAPI = num % m_dwTotalAPIs;   num /= m_dwTotalAPIs;
    m_dwCurrentFVF = num % 2;               num /= 2;
    m_dwCurrentProperties = num % 7;        num /= 7;
#ifdef TESTTYPE_WIREFRAME
    dwCurrentPrimitiveType = 0;
#else
    dwCurrentPrimitiveType = num % 3;       num /= 3;
#endif

    // Resolve current triangle
#ifdef TESTTYPE_WIREFRAME
    m_pInputVertices = m_pTriangleVertices;
#else
    m_pInputVertices = &m_pTriangleVertices[3 * m_dwCurrentTriangle];
#endif
    
    // Resolve current FVF
    if (m_dwCurrentFVF == 0)        m_dwCurrentFVF = D3DFVF_XYZ | D3DFVF_NORMAL;
    else                            m_dwCurrentFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
    
    // Add specular to the FVF
    if ((m_dwCurrentFVF & D3DFVF_DIFFUSE) && (m_dwCurrentProperties == CP_SPECULAR || m_dwCurrentProperties == CP_FOG))
        m_dwCurrentFVF |= D3DFVF_SPECULAR;

    // Add texture to the FVF
    if (m_dwCurrentProperties == CP_TEXTURE)    m_dwCurrentFVF |= 1 << D3DFVF_TEXCOUNT_SHIFT;
    if (m_dwCurrentProperties == CP_MULTITEX)   m_dwCurrentFVF |= (m_dwMaxTextures) << D3DFVF_TEXCOUNT_SHIFT;

    // Resolve current primitive type
    if  (dwCurrentPrimitiveType == 0)         m_dwCurrentPrimitiveType = D3DPT_TRIANGLELIST;
    else if  (dwCurrentPrimitiveType == 1)    m_dwCurrentPrimitiveType = D3DPT_TRIANGLESTRIP;
    else                                      m_dwCurrentPrimitiveType = D3DPT_TRIANGLEFAN;

    return WireClip::ResolveTestNumber();
}

