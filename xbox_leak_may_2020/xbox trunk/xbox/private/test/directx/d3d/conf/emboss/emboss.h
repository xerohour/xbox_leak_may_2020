// FILE:        emboss.h
// DESC:        emboss class header for emboss conformance tests
// AUTHOR:      Todd M. Frost

//USETESTFRAME

const DWORD NOFORMAT = 0xFFFFFFFF; // no format index

const UINT MAXBUFFER = 128;

const DWORD VALIDATE = 0x00000001; // validate src/ref devices

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

class CEmboss: public CD3DTest
{
    public:
        double m_fFactor;           // lemniscate factor (r = sin(factor*theta))
        double m_fOffset[2];        // texture offset
        DWORD m_dwFormat;           // current ARGB texture format index
        DWORD m_dwFVF;              // flexible vertex format
        PFLEXVERT m_pVertices;      // non strided vertex data
        UINT m_uIntervals;          // number of intervals
        CFlags m_Flags;             // flags class
//        CImageData *m_pBaseImage;   // image data class (base)
//        CImageData *m_pHeightImage; // image data class (height)
        CTexture8 *m_pBaseTexture;   // texture class (base)
        CTexture8 *m_pHeightTexture; // texture class (height)
//        CMaterial *m_pMaterial;     // material class
        CShapesFVF m_Shape;         // shapes FVF class

        CHANNELDATA m_DataBase;
        CHANNELDATA m_DataHeight;
        
    public:
        CEmboss();
        ~CEmboss();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        virtual bool ProcessFrame(VOID);
        virtual bool TestTerminate(VOID);
        bool SetDefaultLightStates(VOID);
        bool SetDefaultLights(VOID);
        bool SetDefaultMaterials(VOID);
        bool SetDefaultMatrices(VOID);
        bool bEmboss(UINT uTest);
        VOID vProcessArgs(VOID);
};
