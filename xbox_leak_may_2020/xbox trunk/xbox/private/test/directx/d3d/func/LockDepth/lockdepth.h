//
//  LockDepth.h
//

#ifndef __LockDepth_H__
#define __LockDepth_H__


#define TESTDXVER 0x0800

//USETESTFRAME

//
// handy stuff
//

#define COUNTOF(a)  (sizeof(a)/sizeof((a)[0]))
//#define DecodeHResult(hr) (GetHResultString(hr).c_str())



//extern CD3DWindowFramework *g_pD3DFramework;



//
//  The CDevice8Test specification
//


class LockDepth : public CD3DTest
{
public:
//	LPDIRECT3DDEVICE8 m_pDevice;
	CDirect3D8*       m_pD3D;
	WORD			  m_wTolerance;

public:
    
	LockDepth();

    bool ExecuteTest(UINT nTest);
    UINT TestInitialize();
	bool TestTerminate();


private:
	bool InitializeQuickTest(UINT nTest, D3DLOCKED_RECT* plrcDepth, D3DSURFACE_DESC* DescDepth );
	bool ExecuteQuickTest(UINT nTest);
	bool ValidateQuickTest(UINT nTest, D3DLOCKED_RECT* plrcColor, D3DSURFACE_DESC* DescColor, D3DLOCKED_RECT* plrcDepth, D3DSURFACE_DESC* DescDepth, LPTSTR szLogBuffer );
	HRESULT Render( UINT nTest );
	HRESULT OneTimeInit( VOID );
	WORD GetZDepthWord( UINT nTest, FLOAT x, FLOAT y);
	FLOAT GetZDepthFloat( UINT nTest, FLOAT x, FLOAT y);
	FLOAT GetZImageDepthFloat(UINT nTest, FLOAT x, FLOAT y);
	WORD GetZImageDepthWord(UINT nTest, FLOAT x, FLOAT y);
	CVertexBuffer8* m_pSrcVB;
};

#endif

