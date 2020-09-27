// FILE:        pointsprite.h
// DESC:        point sprite class header for point sprite conformance test
// AUTHOR:      Todd M. Frost

//#include "TestFrameBase.h"
//#include "CD3DTest.h"
#include "flags.h"

//USETESTFRAME

#define MYD3DFVF_LVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#ifndef UNDER_XBOX
#define MYD3DFVF_LSVERTEX (D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#else
#define MYD3DFVF_LSVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#endif
#define MYD3DFVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#ifndef UNDER_XBOX
#define MYD3DFVF_TLSVERTEX (D3DFVF_XYZRHW | D3DFVF_PSIZE | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#else
#define MYD3DFVF_TLSVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#endif
#define MYD3DFVF_TLVERTEX_NOTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR)

#define MAXBUFFER   128 // max buffer length for strings

#define TEXTURE_WIDTH   256 // texture width
#define TEXTURE_HEIGHT  256 // texture height

#define NVDEF 8   // number of variations (default)
#define NVMIN 2   // number of variations (min)
#define NVMAX 256 // number of variations (max)

#define NOFORMAT -1 // no format flag

#define	NCLIPTESTS 24 // number of clip tests (translates to (x,y) positions)
#define NUMPOINTSPRITES_ONSCREEN 6 // This is the number of pointsprites render to the target.

#define NCSETS 4 // number of coefficient sets (constant, linear, quadratic, full)

#define EPSILON ((float)(1.0e-3)) // clip plane epsilon (moves primitives just inside clip volume)

#define ZERO 1.0e-8 // floating point zero

#define VALIDATE        0x00000001
#define VERTEXPOINTSIZE 0x00000002
#define INVALID         0x00000004
#define SYMMETRIC       0x00000008
#define TEXTUREON		0x00000010
#define FLATSHADE		0x00000020

typedef union _FLOATUINT32 {
    FLOAT f;
    DWORD dw;
    UINT32 u;
} FLOATUINT32;

typedef struct _COEFFDATA {
    FLOATUINT32 Ka;
    FLOATUINT32 Kb;
    FLOATUINT32 Kc;
} COEFFDATA, *PCOEFFDATA;

typedef struct _SPRITEDATA {
    FLOATUINT32 Ka;
    FLOATUINT32 Kb;
    FLOATUINT32 Kc;
    FLOATUINT32 PointSize;
    FLOATUINT32 PointSizeMax;
    FLOATUINT32 PointSizeMin;
} SPRITEDATA, *PSPRITEDATA;

typedef struct _MYD3DLVERTEX {
    D3DVALUE dvX;
    D3DVALUE dvY;
    D3DVALUE dvZ;
    D3DCOLOR dcColor;
    D3DCOLOR dcSpecular;
    D3DVALUE dvTU;
    D3DVALUE dvTV;
} MYD3DLVERTEX, *PMYD3DLVERTEX;

typedef struct _MYD3DFVFLS {
    D3DVALUE dvX;
    D3DVALUE dvY;
    D3DVALUE dvZ;
#ifndef UNDER_XBOX
    D3DVALUE dvS;
#endif
    D3DCOLOR dcColor;
    D3DCOLOR dcSpecular;
    D3DVALUE dvTU;
    D3DVALUE dvTV;
} MYD3DLSVERTEX, *PMYD3DLSVERTEX;

typedef struct _MYD3DFVFTL {
    D3DVALUE dvSX;
    D3DVALUE dvSY;
    D3DVALUE dvSZ;
    D3DVALUE dvRHW;
    D3DCOLOR dcColor;
    D3DCOLOR dcSpecular;
    D3DVALUE dvTU;
    D3DVALUE dvTV;
} MYD3DTLVERTEX, *PMYD3DTLVERTEX;

typedef struct _MYD3DFVFTLNOTEX {
    D3DVALUE dvSX;
    D3DVALUE dvSY;
    D3DVALUE dvSZ;
    D3DVALUE dvRHW;
    D3DCOLOR dcColor;
    D3DCOLOR dcSpecular;
} MYD3DTLVERTEXNOTEX, *PMYD3DTLVERTEXNOTEX;

typedef struct _MYD3DFVFTLS {
    D3DVALUE dvSX;
    D3DVALUE dvSY;
    D3DVALUE dvSZ;
    D3DVALUE dvRHW;
#ifndef UNDER_XBOX
    D3DVALUE dvS;
#endif
    D3DCOLOR dcColor;
    D3DCOLOR dcSpecular;
    D3DVALUE dvTU;
    D3DVALUE dvTV;
} MYD3DTLSVERTEX, *PMYD3DTLSVERTEX;

class CSprite: public TESTFRAME(CD3DTest)
{
    private:
        INT m_iFormat;               // current texture format index
        INT *m_pFormats;             // texture format indices       
//        CImageData *m_pImage;        // image data class
        CTexture8 *m_pTexture;        // texture class

    protected:
        CHAR m_szBuffer[MAXBUFFER];  // text buffer
        D3DCOLOR m_dcBackground;     // background color
        D3DVALUE m_dvAngle;          // view angle (fov)
        D3DVALUE m_dvFar;            // far clipping plane
        D3DVALUE m_dvNear;           // near clipping plane
        D3DVALUE m_dvPointSizeMax;   // point sprite size max wrt viewport
        D3DVECTOR m_At;              // at point
        D3DVECTOR m_From;            // from point
        D3DVECTOR m_Up;              // up vector
        D3DVECTOR m_ViewportCenter;  // viewport center (screen space)
        INT m_nFormats;              // number of texture format indices
        UINT m_nVariationsPerFormat; // variations per texture format
        SPRITEDATA m_Sprite;         // sprite data

        CHANNELDATA Data;
        BOOL            m_bExit;

    public:
        CFlags m_Flags;              // flags class
        CSprite();
        ~CSprite();
		virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        virtual bool ProcessFrame(VOID);
        virtual bool TestTerminate(VOID);
        virtual bool ClearFrame(VOID);
        bool SetDefaultMatrices(VOID);
        bool SetDefaultRenderStates(VOID);
        VOID ProcessArgs(VOID);
 		VOID SetPosition(UINT uTest, float *pfX, float *pfY);

        virtual BOOL    AbortedExit(void);

    protected:

        virtual void    ProcessInput();
        virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                            WPARAM wParam, LPARAM lParam);
};

class CSpriteAttenuateFVFL : public CSprite
{
    private:
        D3DVECTOR m_LineOfSight;            // line of sight (normalized)
        COEFFDATA m_Coefficients[NCSETS];   // attenuation coefficient data

    public:
        CSpriteAttenuateFVFL();
        ~CSpriteAttenuateFVFL();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CSpriteAttenuateFVFLS : public CSprite
{
    private:
        D3DVECTOR m_LineOfSight;            // line of sight (normalized)
        COEFFDATA m_Coefficients[NCSETS];   // attenuation coefficient data

    public:
        CSpriteAttenuateFVFLS();
        ~CSpriteAttenuateFVFLS();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CSpriteScaleFVFL : public CSprite
{
    public:
        CSpriteScaleFVFL();
        ~CSpriteScaleFVFL();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CSpriteScaleFVFLS : public CSprite
{
    public:
        CSpriteScaleFVFLS();
        ~CSpriteScaleFVFLS();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CSpriteScaleFVFTL : public CSprite
{
    public:
        CSpriteScaleFVFTL();
        ~CSpriteScaleFVFTL();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CSpriteScaleFVFTLS : public CSprite
{
    public:
        CSpriteScaleFVFTLS();
        ~CSpriteScaleFVFTLS();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);		
};

class CSpriteBatch : public CSprite
{
    public:
        CSpriteBatch();
        ~CSpriteBatch();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
		bool DrawPointSprites(LPVOID Vertex, DWORD dwFormat, UINT StartPosNum, UINT NumToDraw);
		bool ProcessFrame(void);
	private:
		DWORD m_dwPointSpriteEnable;
		bool m_fChangeMode;
};