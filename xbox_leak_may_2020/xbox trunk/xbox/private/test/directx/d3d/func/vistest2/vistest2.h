/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vistest2.h

Author:

    Robert Heitkamp

Description:

    Visibilty Test functions

*******************************************************************************/

#ifndef __VISTEST2_H__
#define __VISTEST2_H__

//------------------------------------------------------------------------------
//	Vertex structure
//------------------------------------------------------------------------------
struct VISTEST_VERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
	DWORD color;        // The vertex color
};
#define FVF_VISTEST_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

//------------------------------------------------------------------------------
//	Constantes
//------------------------------------------------------------------------------
static const int testMaxIterations		= 4;
static const int obscureMaxIterations	= 10;

//------------------------------------------------------------------------------
//	CVisibilityTest
//------------------------------------------------------------------------------
class CVisibilityTest : public CScene {
public:

                                    CVisibilityTest();
                                    ~CVisibilityTest();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Setup();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);

private:

	BOOL UpdateIndices(void);
	UINT Max(UINT a, UINT b);
	UINT Min(UINT a, UINT b);
    void RenderStage1(void);
    void RenderStage2(void);
    void RenderStage3(void);
    void RenderStage4(void);
    void RenderStage5(void);
	HRESULT GetVisibilityTestResult(DWORD id, UINT& uResult);

private:

	VISTEST_VERTEX			m_testTris[6];
	VISTEST_VERTEX			m_obscureTris[6];
	float					m_testY[testMaxIterations];
	float					m_testX[testMaxIterations];
	float					m_obscureY[obscureMaxIterations];
	float					m_obscureX[obscureMaxIterations];
	int						m_indices[8];
	DWORD					m_index;
	UINT					m_uVisible;
	UINT					m_height;
	UINT					m_width;
	int						m_stage;
	int						m_nextStage;
	DWORD					m_testId;
	D3DPRESENT_PARAMETERS	m_savedD3dpp;


};

#endif //__VISTEST_H__
