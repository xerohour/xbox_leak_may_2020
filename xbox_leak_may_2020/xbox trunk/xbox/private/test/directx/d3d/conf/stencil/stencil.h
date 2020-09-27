// FILE:        stencil.h
// DESC:        stencil class header for stencil conformance tests
// AUTHOR:      Todd M. Frost

//#include "CD3DTest.h"
//#include "flags.h"

//USETESTFRAME

const UINT MAXBUFFER = 128;

const UINT NMASKS = 2;

const DWORD VERBOSE = 0x00000001;
const DWORD SUBRECT = 0x00000002;
const DWORD HIMASK  = 0x00000004;
const DWORD LOMASK  = 0x00000008;

const FLOAT HALF_SUBRECT_WIDTH = 16.0f;
const FLOAT HALF_SUBRECT_HEIGHT = 16.0f;

typedef struct _Args
{
    D3DCOLOR dwClearColor; // clear color
    DWORD dwStencil;       // stencil clear
    DWORD dwReference[2];  // reference masks
} ARGS, *PARGS;

typedef struct _Buffer
{
    DWORD dwStencilBitMask; // stencil bit mask
    DWORD dwZBitMask;       // z bit mask
    struct _Buffer *pNext;  // address of BUFFER data
} BUFFER, *PBUFFER;

class CStencil: public CD3DTest
{
    public:
        CHAR m_szBuffer[MAXBUFFER];        // text buffer
        D3DCOLOR m_dwFailColor;            // fail color
        D3DCOLOR m_dwPassColor;            // pass color
        DWORD m_dwRedMask;                 // red mask for source render target
        DWORD m_dwRequestedDepth;          // requested stencil bit depth
        DWORD m_dwStencilBitDepth;         // stencil bit depth
        DWORD m_dwStencilBitDepthReported; // stencil bit depth reported
        DWORD m_dwStencilBitMask;          // stencil bit mask
        DWORD m_dwStencilCaps;             // stencil caps
        DWORD m_dwZCmpCaps;                // z compare caps
        UINT m_uRange;                     // test range
        CFlags m_Flags;                    // flags class

        BOOL            m_bExit;

	public:
    	CStencil();
    	~CStencil();
        virtual void CommandLineHelp(void);
//        virtual bool AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize);
    	virtual UINT EmptyModeList(VOID);
//    	virtual bool PreModeSetup(void);
        virtual bool SetDefaultRenderStates(VOID);
    	virtual bool ClearFrame(VOID);
        virtual bool BeginScene(D3DVALUE dvZ, DWORD dwStencil, DWORD dwColor, DWORD dwFlags);
        virtual bool SetTestRange(UINT uStart, UINT uEnd);
    	virtual UINT TestInitialize(VOID);
    	virtual bool ExecuteTest(UINT uTest);
//        virtual bool CompareImages(CDirectDrawSurface *pSrc, CDirectDrawSurface *pRef, CDirectDrawSurface *pResult);
        virtual bool ProcessFrame(VOID);
        bool bCompare(VOID);
        bool bSupportedOperation(DWORD dwOp);
    	bool bUnique(PBUFFER *pBuffers);
        VOID vGetRange(INT nMin[2], INT nMax[2]);
        VOID vFlood(D3DVALUE dvDepth, D3DCOLOR dwColor);
        VOID vStipple(bool bEnable);
        VOID vProcessArgs(VOID);

        virtual BOOL    AbortedExit(void);

    protected:

        virtual void    ProcessInput();
        virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                            WPARAM wParam, LPARAM lParam);
};

class CStencilCap : public CStencil
{
    public:
        CStencilCap();
        ~CStencilCap();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CStencilClear : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilClear();
        ~CStencilClear();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID vTest(bool bStipple, DWORD dwClear, DWORD dwMask);
};

class CStencilFunc : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilFunc();
        ~CStencilFunc();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID vTest(PARGS pArgs);
        LPSTR lpszGetFunc(DWORD dwFunc);
};

class CStencilIgnore : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilIgnore();
        ~CStencilIgnore();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID vTest(bool bStipple, DWORD dwClear, DWORD dwMask);
};

class CStencilOp : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilOp();
        ~CStencilOp();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID vTest(DWORD dwState, DWORD dwOp);
        LPSTR lpszGetOp(DWORD dwOp);
        LPSTR lpszGetState(DWORD dwState);
};

class CStencilSCorrupt : public CStencil 
{
    public:
        UINT m_nStipple;

    public:
        CStencilSCorrupt();
        ~CStencilSCorrupt();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CStencilZCorrupt : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilZCorrupt();
        ~CStencilZCorrupt();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};
