// FILE:        lightmap.h
// DESC:        lightmap class header for D3DTOP_DOTPRODUCT3 conformance test
// AUTHOR:      Todd M. Frost

//USETESTFRAME

const DWORD NOFORMAT = 0xFFFFFFFF; // no format index

const UINT MAXBUFFER = 128;

const UINT NSTAGES = 2; // number of stages

class CLightmap: public CD3DTest
{
    private:
        CHAR m_szBuffer[MAXBUFFER];   // text buffer
        DOUBLE m_fFactor;             // lemniscate factor (r = sin(factor*theta))
        DWORD m_dwFormat;             // current RGB texture format index
        DWORD m_dwIntervals;          // intervals for lemniscate traversal 
        DWORD m_dwStages;             // max texture stages (clamped to 2)
        CFlags m_Flags;               // flags class
//        CImageData *m_pNorthernImage; // image data class (northern hemisphere/field)
//        CImageData *m_pSouthernImage; // image data class (southern hemisphere/field)
        CTexture8 *m_pNorthernTexture; // texture class
        CTexture8 *m_pSouthernTexture; // texture class
        
        CHANNELDATA Data;

    public:
        CLightmap();
        ~CLightmap();
        virtual void CommandLineHelp(void);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        virtual bool ProcessFrame(VOID);
        virtual bool TestTerminate(VOID);
        VOID vProcessArgs(VOID);
};
