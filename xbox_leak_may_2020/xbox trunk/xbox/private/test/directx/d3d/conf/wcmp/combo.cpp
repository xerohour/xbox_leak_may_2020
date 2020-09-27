//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "WCmp.h"

//************************************************************************
// Internal API:    ComboTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************
void CWCmpTest::ComboTests(float Zval)
{
	// Red Triangle
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f,(float)m_vpTest.Y +  10.0f,0.0f),  0.015f, RGBA_MAKE(255,0,0,255),0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 210.0f,(float)m_vpTest.Y + 210.0f,0.0f),  1.0f,   RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f,(float)m_vpTest.Y + 210.0f,0.0f),  1.0f,   RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
	// Green Triangle
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f,(float)m_vpTest.Y +  10.0f,Zval), (1.0f - Zval), RGBA_MAKE(0,255,0,255),0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 210.0f,(float)m_vpTest.Y + 210.0f,Zval), (1.0f - Zval), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f,(float)m_vpTest.Y + 210.0f,Zval), (1.0f - Zval), RGBA_MAKE(0,255,0,255),0, 0.0f,1.0f);
}
