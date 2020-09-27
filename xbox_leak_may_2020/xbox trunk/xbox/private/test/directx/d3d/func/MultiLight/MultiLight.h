#ifndef __MultiLight_h__
#define __MultiLight_h__

class MultiLight : public CD3DTest
{
public:
	MultiLight(DWORD dwScenario);
	~MultiLight();

	virtual UINT	TestInitialize(void);
	virtual bool	ExecuteTest(UINT);
	virtual void	SceneRefresh(void);
	virtual bool	ProcessFrame(void);
	virtual void	TestCompleted(void);
	virtual bool	TestTerminate(void);

	virtual bool	SetDefaultMatrices(void);
	virtual bool	SetDefaultMaterials(void);
	virtual bool	SetDefaultLights(void);
	virtual bool	SetDefaultLightStates(void);
	virtual bool	SetDefaultRenderStates(void);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);

private:
//	CMaterial*			m_pMaterial;
    D3DMATERIAL8        m_Material;
	CShapes*			m_pSphere;
	CTexture8*			m_pTexture;
	D3DTLVERTEX*		m_pStars;
	DWORD				m_dwLightsNumber;

	DWORD				m_dwScenario;
	DWORD				m_dwMaxLights;
	DWORD				m_dwStarsNumber;
    float               m_fScale;

	DWORD				m_dwCurrentTestNumber;
	DWORD				m_dwNumFailed;

    BOOL                m_bExit;
};


/*
class MultiLight1 : public MultiLight
{
public:
	MultiLight1() : MultiLight(0)
	{
		m_szTestName = "MultiLight1";
		m_szCommandKey = "MultiLight1";
	}
};

class MultiLight2 : public MultiLight
{
public:
	MultiLight2() : MultiLight(1)
	{
		m_szTestName = "MultiLight2";
		m_szCommandKey = "MultiLight2";
	}
};

class MultiLight3 : public MultiLight
{
public:
	MultiLight3() : MultiLight(2)
	{
		m_szTestName = "MultiLight3";
		m_szCommandKey = "MultiLight3";
	}
};

class MultiLight4 : public MultiLight
{
public:
	MultiLight4() : MultiLight(3)
	{
		m_szTestName = "MultiLight4";
		m_szCommandKey = "MultiLight4";
	}
};

class MultiLight5 : public MultiLight
{
public:
	MultiLight5() : MultiLight(4)
	{
		m_szTestName = "MultiLight5";
		m_szCommandKey = "MultiLight5";
	}
};
*/

// Values for m_dwScenario class variable:
// 1 - Disable lights without initialization
// 2 - 0 to MAX lights initialization
// 3 - Max to 0 lights initialization
// 4 - Random light initialization
// 5 - Enable some lights with default parameters (does call SetLight())

#endif