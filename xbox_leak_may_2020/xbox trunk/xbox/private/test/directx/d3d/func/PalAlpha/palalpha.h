#ifndef __PalAlpha_h__
#define __PalAlpha_h__

//USETESTFRAME

typedef enum TEXTURE_TYPE {
	UNDEFINED_TEXTURE,
	PAL8ALPHA,
	PAL8,
	PAL4ALPHA,
	PAL4
};

class CPalTest: public CD3DTest
{
public:
    CPalTest();
	~CPalTest();

    virtual UINT		TestInitialize(void);
    virtual bool		ExecuteTest(UINT);
    virtual bool        ClearFrame(void);
    virtual void		SceneRefresh(void);
    virtual bool		ProcessFrame(void);
    virtual void		TestCompleted(void);
    virtual bool        TestTerminate(void);

	virtual bool		SetDefaultMatrices(void);
	virtual bool		SetDefaultMaterials(void);
	virtual bool		SetDefaultLightStates(void);
	virtual bool		SetDefaultRenderStates(void);
	virtual bool		SetDefaultLights(void);

private:
	CTexture8*				m_pTexture;
    CTexture8*				m_pTextureSource;
//	CMaterial*				m_pMaterial;
//	CLight*					m_pLight;
	
	LPD3DTLVERTEX			pVertices;
//	LPDIRECTDRAWPALETTE		pPalette;
    DWORD					dwVertexCount;
	LPWORD					pIndices;
	DWORD					dwIndexCount;
	DWORD					dwNumFail;
	bool					bFrame;

	// Palette type
	bool					bTest4BitPalette;
	bool					bTest8BitPalette;
	bool					bTestNonAlphaPalette;
	bool					bTestAlphaPalette;
	DWORD					dwTextureTypesNumber;
	LPTSTR					lpszTextureType;
	DWORD					dwCurrentTextureType;
	bool					bCreateNewTexture;

	// Test type
	bool					bTestTextureCreate;
	bool					bTestTextureLoad;
	bool					bTestPaletteCreate;
	bool					bTestSetEntries;
	bool					bTestWrapMode;
	bool					bTestMirrorMode;
	bool					bTestClampMode;
	bool					bTestBorderMode;
	bool					bTestColorKey;
	bool					bTestColorKeyBlend;
	DWORD					dwTestTypesNumber;
	LPTSTR					lpszTestType;
	DWORD					dwCurrentTestType;

	// Test number
	DWORD					dwSameFramesNumber;
	DWORD					dwTotalTestsNumber;

	// Current test information
	DWORD					dwCurrentTestNumber;
	unsigned char			dwPaletteType;
	bool					bAlphaPalette;
	bool					bCreateTexture;
	bool					bLoadTexture;
	bool					bCreatePalette;
	bool					bSetPaletteEntries;
	DWORD					dwAddressMode;
	bool					bUseColorKey;
	bool					bUseColorKeyBlend;
	DWORD					dwFrameParam;
	bool					bLastTestFailed;

	static D3DTLVERTEX		vertices1[4];
	static D3DTLVERTEX		vertices2[4];
	static WORD				indices[6];

	void					ResolveTestNumber(int iTestNum);
	bool					ValidateTest(int iTestNum);

//    CTexture*	            CreateTexture(DWORD dwWidth, DWORD dwHeight, DWORD dwPaletteType, bool bFillTexture, bool bColorKeyFill);
//	bool					CreatePalettedTexture(CDirectDrawSurface*, DWORD dwWidth, DWORD dwHeight, DWORD dwFlags, DWORD dwPaletteType, bool bFillTexture, bool bColorKeyFill);
//	bool					CreateTexturePalette(CDirectDrawSurface*, DWORD dwPaletteType, bool bAlphaPalette, DWORD dwParam, bool bColorKeyFill);
//	bool					ChangeTexturePalette(CDirectDrawSurface*, DWORD dwPaletteType, bool bAlphaPalette, DWORD dwParam, bool bColorKeyFill);
	void					FillPaletteEntries(PALETTEENTRY *pal, DWORD dwPaletteType, bool bAlphaPalette, DWORD dwParam, bool bColorKeyFill);
//	bool					ColorSurface(LPDIRECTDRAWSURFACE4);

    CTexture8*              CreateTexture8(DWORD dwWidth, DWORD dwHeight);
    bool                    CreateTexturePalette8(DWORD dwPalette, bool bAlphaPalette, DWORD dwParam);
    bool                    SetCurrentTexturePalette8(DWORD dwPalette);
};

#endif // __PalAlpha_h__
