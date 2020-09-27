#include "d3dlocus.h"
#include "cd3dtest.h"
#include "dxlegacy.h"
#include "3dmath.h"
#include "Clipping.h"


RENDERPRIMITIVEAPI WPoint::m_pDX8APIList[2] =
{
    RP_DP,                      // DrawPrimitive
    RP_DPS,                     // DrawPrimitiveStrided
};

DWORD WPoint::m_dwDX8APIListSize = 2;

WPoint::WPoint()
{
    m_pInputVertices = 0;
    m_pRenderVertices = 0;

    m_szTestName = TEXT("point clipping");
    m_szCommandKey = TEXT("point");
    
    // Total tests:     ?
    //
    // Total rotations: 14
    // -rotation:       7
    // -ZClip:          2
    //
    // Total settings:  140
    // -API:            10
    // -FVF:            2
    // -properties:     7

    m_dwTotalTests = 140;
    m_dwRotations = 7;
}

WPoint::~WPoint()
{
    if (m_pInputVertices)
        delete [] m_pInputVertices;
    if (m_pRenderVertices)
        delete [] m_pRenderVertices;
}

UINT WPoint::TestInitialize()
{
    if (!GeneratePointList())
    {
        WriteToLog("%s: Can't allocate memory for vertices\n", m_szTestName);
        return D3DTESTINIT_ABORT;
    }

    // Calculate total tests
    // Total tests:     
    // -API:            m_pDX7APIListSize (?) * frequency (2) 
    // -FVF:            2
    // -properties:     7

    m_dwTotalAPIs = m_dwDX8APIListSize * 2;
    m_dwTotalTests = m_dwTotalAPIs * 2 * 7; // (?)

    SetAPI(m_pDX8APIList, m_dwDX8APIListSize, 2);

    return WireClip::TestInitialize();
}

bool WPoint::TestTerminate(void)
{
    if (m_pInputVertices)
    {
        delete [] m_pInputVertices;
        m_pInputVertices = NULL;
    }
    if (m_pRenderVertices)
    {
        delete [] m_pRenderVertices;
        m_pRenderVertices = NULL;
    }
    return WireClip::TestTerminate();
}


bool WPoint::GeneratePointList(void) {
	int		minX = -40;
	int		maxX =  40;
	int		minY = -40;
	int		maxY =  40;
	int		step =  1;
	DWORD	dwCurrentVertex = 0;


	m_dwVertices = (DWORD)(((maxX-minX)/step + 1) * ((maxY-minY)/step + 1));
	
	m_pInputVertices = new D3DVERTEX [ m_dwVertices ];
	if (m_pInputVertices == NULL)
		return false;
	for (int curX = minX; curX <= maxX; curX += step)
		for (int curY = minY; curY <= maxY; curY += step)
			m_pInputVertices[dwCurrentVertex++] = D3DVERTEX( 
				cD3DVECTOR(float(curX)/50.f, float(curY)/50.f, 0.f), cD3DVECTOR(0.f, 0.f, 1.f), 0.f, 0.f );

    m_pRenderVertices = new BYTE [ (3 + 3 + 8*2)*sizeof(float)*m_dwVertices ];
    if (m_pRenderVertices == NULL)
        return false;

	return true;
} 




bool WPoint::ResolveTestNumber()
{
    DWORD   num = m_dwCurrentTestNumber - 1;
    m_dwCurrentAPI = num % m_dwTotalAPIs;   num /= m_dwTotalAPIs;
    m_dwCurrentFVF = num % 2;               num /= 2;
    m_dwCurrentProperties = num % 7;        num /= 7;

    // Resolve current FVF
    if (m_dwCurrentFVF == 0)        m_dwCurrentFVF = D3DFVF_XYZ | D3DFVF_NORMAL;
    else                            m_dwCurrentFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
    
    // Add specular to the FVF
    if ((m_dwCurrentFVF & D3DFVF_DIFFUSE) && (m_dwCurrentProperties == CP_SPECULAR || m_dwCurrentProperties == CP_FOG))
        m_dwCurrentFVF |= D3DFVF_SPECULAR;

    // Add texture to the FVF
    if (m_dwCurrentProperties == CP_TEXTURE)    m_dwCurrentFVF |= 1 << D3DFVF_TEXCOUNT_SHIFT;
    if (m_dwCurrentProperties == CP_MULTITEX)   m_dwCurrentFVF |= (m_dwMaxTextures) << D3DFVF_TEXCOUNT_SHIFT;

    // Primitive type
    m_dwCurrentPrimitiveType = D3DPT_POINTLIST;

    return WireClip::ResolveTestNumber();
}


