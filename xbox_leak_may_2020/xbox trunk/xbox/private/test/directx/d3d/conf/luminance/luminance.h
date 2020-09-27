// FILE:        luminance.h
// DESC:        luminance class header for AL8LU8 and LU8 texture formats
// AUTHOR:      Todd M. Frost

//USETESTFRAME

const DWORD NOFORMAT = 0xFFFFFFFF; // no format index

const DWORD INDEPENDENT = 0x00000001; // independent uv address modes
const DWORD VALIDATE    = 0x00000002; // validate src/ref devices

const UINT DEFAULT_BUMPS = 8;  // default number of bumps
const UINT MIN_BUMPS     = 1;  // min number of bumps
const UINT MAX_BUMPS     = 64; // max number of bumps

const UINT MAXBUFFER = 128;

const UINT NMODES = 4; // number of address modes

const UINT NSTAGES = 3; // number of stages

const D3DTEXTUREADDRESS DEFAULT_MODE = D3DTADDRESS_WRAP; // default address mode

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

class CLuminance: public CD3DTest
{
    public:
        CHAR m_szBuffer[MAXBUFFER];        // text buffer
        D3DTEXTUREADDRESS m_dwModes[2];    // uv texture address modes
        DWORD m_dwFormat;                  // current luminance texture format index
        DWORD m_dwFVF;                     // flexible vertex format
        DWORD m_dwStages;                  // texture stages (used)
        DWORD m_dwStagesMax;               // texture stages (max)
        DWORD m_dwTexAddressCaps;          // texture address caps
        DWORD m_dwTexOpCaps;               // texture op caps
        DWORD m_dwModeCapsVals[NMODES][2]; // texture address mode caps/vals
        PFLEXVERT m_pVertices;             // non strided vertex data
        CFlags m_Flags;                    // flags class
//        CImageData *m_pBaseImage;          // image data class (base)
//        CImageData *m_pLuminanceImage;     // image data class (luminance)
        CTexture8 *m_pBaseTexture;          // texture class (base)
        CTexture8 *m_pLuminanceTexture;     // texture class (luminance)
//        CMaterial *m_pMaterial;            // material class
        CShapesFVF m_Shape;                // shapes FVF class

        CHANNELDATA Data;
        int nTextureFormat;
        
    public:
        CLuminance();
        ~CLuminance();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        virtual bool ProcessFrame(VOID);
        virtual bool TestTerminate(VOID);
        bool SetDefaultLightStates(VOID);
        bool SetDefaultLights(VOID);
        bool SetDefaultMaterials(VOID);
        bool SetDefaultMatrices(VOID);
//        bool bMatch(DWORD dwFormat, DDPIXELFORMAT ddpfPixelFormat);
        bool bSetLuminance(DWORD dwFormat, DWORD dwWidth, DWORD dwHeight);
        DWORD dwGetCountFromMask(DWORD dwMask);
        VOID vProcessArgs(VOID);
};

class CLuminanceAddress : public CLuminance
{
    public:
        CLuminanceAddress();
        ~CLuminanceAddress();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};
