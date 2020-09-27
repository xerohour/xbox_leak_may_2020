// FILE:        TLVertexClip.h
// DESC:        TL vertex clip class header for TL vertex clip conformance tests

#ifndef __TLVERTEXCLIP_H__
#define __TLVERTEXCLIP_H__

//#include "TestFrameBase.h"
//#include "CD3DTest.h"

const UINT MAXBUFFER = 1024;
const UINT NVERTICES = 10;
const UINT EDGES = 4;
const UINT POINTS_PER_SIDE = 5;
const UINT TESTS_PER_EDGE = 3;
const UINT EDGE_TESTS = (EDGES*TESTS_PER_EDGE);
const UINT QUAD_TESTS = ((4*POINTS_PER_SIDE)*(4*POINTS_PER_SIDE));

const long PORT_X = 20;
const long PORT_Y = 20;
const long PORT_WIDTH = 280;
const long PORT_HEIGHT = 240;

const long CLIP_X = 0;
const long CLIP_Y = 0;
const long CLIP_WIDTH = 320;
const long CLIP_HEIGHT = 280;
 
//USETESTFRAME

typedef struct
{
    BYTE    Red;
    BYTE    Green;
    BYTE    Blue;
} RGBSIZE, *PRGBSIZE;

struct CPort
{
	long Top;
    long Left;
    long Bottom;
    long Right;

	CPort(long x, long y, long Width, long Height)
    {
        Top = y;
        Left = x;
        Bottom = y + Height;
        Right = x + Width;
    }
	bool Inside(long x, long y)
    {
        if ((y >= Top) && (y < Bottom) && (x >= Left) && (x < Right))
            return true;
        else
            return false;
    }
    long Width(void)
    {
        return (Right - Left);
    }
    long Height(void)
    {
        return (Bottom - Top);
    }
};

class CTLVertexClip: public TESTFRAME(CD3DTest)
{
	public:
    	bool m_bShow;
        D3DTLVERTEX m_Vertices[NVERTICES];
    	D3DPRIMITIVETYPE m_Type;
    	DWORD m_dwPitch;
    	DWORD m_dwLineColor;
    	DWORD m_dwPortColor;
    	char  m_szBuffer[MAXBUFFER];
    	UINT m_nTests;
	    CPort m_Port;

        ARGBPIXELDESC pixd;
        BOOL            m_bExit;

	public:
       	CTLVertexClip();
       	~CTLVertexClip();
	    virtual void CommandLineHelp(void);
       	virtual bool SetupViewport();
       	virtual bool ClearFrame(void);
        virtual UINT TestInitialize(void);
       	virtual bool ExecuteTest(UINT);
       	virtual bool TestTerminate(void)
        {
            return true;
        }
//        virtual bool CompareImages(CDirectDrawSurface *pSrc, CDirectDrawSurface *pRef, CDirectDrawSurface *pResult)
//        {
//            return true;
//        }
       	virtual bool ProcessFrame(void);
       	virtual void SceneRefresh(void);
        bool AnyExtraPixels(void);
        int PixelCheckAtDepth(PWORD pBuffer, long *pdwDx, long *pdwDy);
        int PixelCheckAtDepth(PDWORD pBuffer, long *pdwDx, long *pdwDy);
        int PixelCheckAtDepth(PRGBSIZE pBuffer, long *pdwDx, long *pdwDy);
        float CTLVertexClip::PxFromTest(UINT nPoint);
        float CTLVertexClip::PyFromTest(UINT nPoint);
    	virtual void SetEndPoints(D3DVECTOR *A, D3DVECTOR *B, UINT uTest) {}

        virtual BOOL    AbortedExit(void);

    protected:

        virtual void    ProcessInput();
        virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                            WPARAM wParam, LPARAM lParam);
};

class CTLVertexClipQuad : public CTLVertexClip
{
    public:
        CTLVertexClipQuad();
        ~CTLVertexClipQuad();
        virtual void SetEndPoints(D3DVECTOR *A, D3DVECTOR *B, UINT uTest);
};

class CTLVertexClipEdge : public CTLVertexClip
{
    public:
        CTLVertexClipEdge();
        ~CTLVertexClipEdge();
        virtual void SetEndPoints(D3DVECTOR *A, D3DVECTOR *B, UINT uTest);
};

#endif
