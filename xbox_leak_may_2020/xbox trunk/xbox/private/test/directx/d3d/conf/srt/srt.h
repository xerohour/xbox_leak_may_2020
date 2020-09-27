//
// SRT.H - Header file for the SetRenderTarget test
//

#ifndef _SRT_H_
#define _SRT_H_

//#include "testframebase.h"
//#include "CD3DTest.h"
//#include <tchar.h>
//#include <vector>

//USETESTFRAME

#define SRT_SIZE 256 
#define SRT_FVFTLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

typedef struct _srt_tlvertex
{
    float x, y, z, rhw;
    DWORD dwDiffuse;
    float u, v;
} SRT_TLVERTEX, *PSRT_TLVERTEX;

class CSRT: public TESTFRAME(CD3DTest)
{

public:
    bool m_bInvalid;
    bool m_bOffscreen;
    DWORD * m_pStripes;
    int m_RTWidth, m_RTHeight;
//    CImageData * m_pImageData;
    CSurface8 * m_pOffSrcSurf;
    CSurface8 * m_pOffRefSurf;

    D3DFORMAT       m_fmtdOriginal;
    BOOL            m_bExit;

    CSRT();
    ~CSRT();

    virtual void CommandLineHelp(void);
    virtual bool SetDefaultRenderStates(void);
    virtual UINT TestInitialize(void);
    virtual bool ClearFrame(void);
    virtual bool ExecuteTest(UINT uiTest);
    virtual void SceneRefresh(void);
    virtual bool ProcessFrame(void);
    virtual bool TestTerminate(void);

    virtual BOOL    Render(void);
    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);

private:

    CTexture8 *  m_pTextureRT;
    CSurface8 *  m_pTextureRTS;
    CSurface8 * m_pSrcZ, * m_pRefZ;
    
    CSurface8 * m_pOriginalSrcRT, * m_pOriginalRefRT; 
    CSurface8 * m_pOriginalSrcZ, * m_pOriginalRefZ; 

    int m_nOriginalZFormat, m_nTexFormat, m_nZFormat;

    bool CopyDestBuffer(void);
    bool CreateDestBuffer(bool bCreateZ, CSurface8 *& pSrcZ, CSurface8 *& pRefZ);
    void DescribeTest(char *, int, int);
//    std::vector< std::pair<DWORD, DWORD> > m_vSupportedFormats;
};

// Texture Class definitions
class CSRTTexture: public CSRT
{
	public:
	CSRTTexture();
	~CSRTTexture();
};

// No ZBuffer Texture Class definitions
class CSRTNoZTexture: public CSRT
{
	public:
	CSRTNoZTexture();
	~CSRTNoZTexture();
};

// Offscreen Class definitions
class CSRTOffscreen: public CSRT
{
	public:
	CSRTOffscreen();
	~CSRTOffscreen();
};

// No ZBuffer Offscreen Class definitions
class CSRTNoZOffscreen: public CSRT
{
	public:
	CSRTNoZOffscreen();
	~CSRTNoZOffscreen();
};

// Invalid Class definitions
class CSRTInvalid: public CSRT
{
	public:
	CSRTInvalid();
	~CSRTInvalid();

    // Framework functions
	UINT TestInitialize(void);
};

#endif //_SRT_H_
