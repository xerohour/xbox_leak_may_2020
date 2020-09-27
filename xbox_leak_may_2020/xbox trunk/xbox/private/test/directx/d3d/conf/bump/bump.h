// FILE:        bump.h
// DESC:        bump class header for
//                  D3DTOP_BUMPENVMAP
//                  D3DTOP_BUMPENVMAPLUMINANCE
//              conformance tests
// AUTHOR:      Todd M. Frost
// COMMENTS:    du:dv:lu usage:
//                  v += du*M[0][0] + dv*M[0][1]
//                  u += du*M[1][0] + dv*M[1][1]
//                  l = lu*scale + offset
//                  lu defaults to 1 for Du8:Dv8 textures

#include "flags.h"

//USETESTFRAME

const UINT MAXBUFFER = 128;

const DWORD NOFORMAT = 0xFFFFFFFF; // no format index

const DWORD INDEPENDENT = 0x00000001; // independent uv texture address mode
const DWORD VALIDATE    = 0x00000002; // validate src/ref devices
const DWORD MODULATE    = 0x00000010; // blend:  modulate texture/current
const DWORD MODULATE2X  = 0x00000020; // blend:  modulate texture/current 2x
const DWORD MODULATE4X  = 0x00000040; // blend:  modulate texture/current 4x
const DWORD BLEND       = 0x00000070; // blend mask

const UINT NBLENDS = 3; // number of blend modes (mod, mod2x, mod4x)

const UINT MATRIX_SCALE_DU   = 0;
const UINT MATRIX_SCALE_DV   = 1;
const UINT MATRIX_SCALE_DUDV = 2;
const UINT MATRIX_SKEW_DU    = 3;
const UINT MATRIX_SKEW_DV    = 4;
const UINT MATRIX_SKEW_DUDV  = 5;
const UINT NMATRICES         = 6;

const UINT MODE_BORDER = 0;
const UINT MODE_CLAMP  = 1;
const UINT MODE_MIRROR = 2;
const UINT MODE_WRAP   = 3;
const UINT NMODES      = 4;

const D3DTEXTUREADDRESS DEFAULT_MODE = D3DTADDRESS_WRAP;

const FLOAT DEFAULT_MAG = 0.04f; // default bumpenvmap matrix magnitude

const FLOAT DEFAULT_OFFSET = 0.0f; // default bumpenvmap luminance offset

const FLOAT DEFAULT_SCALE = 0.7f; // default bumpenvmap luminance scale

typedef union _FLOATUINT32 {
    FLOAT f;
    DWORD dw;
    UINT32 u;
} FLOATUINT32;

typedef struct _FlexVert {
    D3DVALUE fPx;
    D3DVALUE fPy;
    D3DVALUE fPz;
    D3DVALUE fNx;
    D3DVALUE fNy;
    D3DVALUE fNz; 
    D3DVALUE fSu;
    D3DVALUE fSv;   
    D3DVALUE fTu;
    D3DVALUE fTv;
} FLEXVERT, *PFLEXVERT;

class CBump: public CD3DTest
{
    public:
        CHAR m_szBuffer[MAXBUFFER];        // text buffer
        D3DTEXTUREADDRESS m_dwModes[2];    // uv texture address modes
        DWORD m_dwDestBlendCaps;           // dest blend caps
        DWORD m_dwFormat;                  // current bump texture format index
        DWORD m_dwFVF;                     // flexible vertex format
        DWORD m_dwInclusion;               // inclusion flags
        DWORD m_dwMatrixFormat;            // bump map matrix format index
        DWORD m_dwSrcBlendCaps;            // src blend caps
        DWORD m_dwTexAddressCaps;          // texture address caps
        DWORD m_dwTexOpCaps;               // texture op caps
        DWORD m_dwModeCapsVals[NMODES][2]; // texture address mode caps/vals
        FLOATUINT32 m_fOffset;             // bump map luminance offset
        FLOATUINT32 m_fScale;              // bump map luminance scale
        FLOATUINT32 m_fMatrix[2][2];       // bump map transformation matrix
        PFLEXVERT m_pVertices;             // non strided vertex data
        UINT m_uVariations;                // number of variations
        CFlags m_Flags;                    // flags class
//        CImageData *m_pBaseImage;          // image data class (base)
//        CImageData *m_pBumpImage;          // image data class (bump)
        CTexture8 *m_pBaseTexture;          // texture class (base)
        CTexture8 *m_pBumpTexture;          // texture class (bump)
        CHANNELDATA m_Data;
//        CMaterial *m_pMaterial;            // material class
        CShapesFVF m_Shape;                // shapes FVF class

        BOOL m_bExit;
        
    public:
        CBump();
        ~CBump();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        virtual bool ProcessFrame(VOID);
        virtual bool TestTerminate(VOID);
        bool SetDefaultLightStates(VOID);
        bool SetDefaultLights(VOID);
        bool SetDefaultMaterials(VOID);
        bool SetDefaultMatrices(VOID);
        bool bSetBump(DWORD dwFormat, DWORD dwWidth, DWORD dwHeight);
        VOID vSetMatrix(DWORD dwType, FLOAT fDu, FLOAT fDv);
        DWORD dwGetCountFromMask(DWORD dwMask);
        FLOAT fNormalize(DWORD dwCurrent, DWORD dwMin, DWORD dwMax);
        VOID vProcessArgs(VOID);

        virtual BOOL    AbortedExit(void);

    protected:

        virtual void    ProcessInput();
        virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                            WPARAM wParam, LPARAM lParam);
};

class CBumpAddress : public CBump
{
    public:
        CBumpAddress();
        ~CBumpAddress();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CBumpBlend : public CBump
{
    public:
        CBumpBlend();
        ~CBumpBlend();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CBumpMatrix : public CBump
{
    public:
        CBumpMatrix();
        ~CBumpMatrix();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CBumpOffset : public CBump
{
    public:
        CBumpOffset();
        ~CBumpOffset();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CBumpScale : public CBump
{
    public:
        CBumpScale();
        ~CBumpScale();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};
