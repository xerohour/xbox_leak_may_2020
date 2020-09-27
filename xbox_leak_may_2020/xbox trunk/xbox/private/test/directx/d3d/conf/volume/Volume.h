/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Volume.h
 *
 ***************************************************************************/

#ifndef __VOLUMETEST_H__
#define __VOLUMETEST_H__

//#include "TestFrameBase.h"
//#include "CD3DTest.h"

//USETESTFRAME

const int MAX_TESTS = 50;
const int MAX_MIP_TESTS = 100;

struct TVertex {									// Cubemap vertex with 3D texture coords
	D3DVECTOR v;
	D3DVECTOR n;
	D3DVECTOR t;
};

struct TGVertex {									// Cubemap vertex without 3D texture coords
	D3DVECTOR v;
	D3DVECTOR n;
};

// Class definition
class CVolumeTest: public TESTFRAME(CD3DTest)
{
    // Data
	public:

//    struct LVERTEX {
//        D3DVALUE     x;             /* Homogeneous coordinates */
//        D3DVALUE     y;
//        D3DVALUE     z;
//        D3DCOLOR     color;         /* Vertex color */
//        D3DCOLOR     specular;      /* Specular component of vertex */
//        D3DVALUE     tu;            /* Texture coordinates */
//        D3DVALUE     tv;
//        D3DVALUE     tw;
//    };

//	LVERTEX         VertexList[3];
//	CMaterial *     pMaterial;
	TVertex *       pTVertex;							// Sphere vertices with texture vectors
	TGVertex *      pTGVertex;							// Sphere vertices without texture vectors
	int             nVertexMax;
    DWORD           dwVertexCount;
	CVolumeTexture8 *pVolTexture;
	bool			bMipMap;
    bool            bTexGen;
    float           fAngle;
    char 	        msgString[80];
	char 			szStatus[80];

    int             nVolumeFormat;
	D3DCOLOR		Stripes[8];
	D3DCOLOR		AlphaStripes[8];
	D3DCOLOR		RedStripes[8];
	D3DCOLOR		GreenStripes[8];
	D3DCOLOR		BlueStripes[8];
	D3DCOLOR		YellowStripes[8];
	D3DCOLOR		MagentaStripes[8];
	DWORD           dwColors[4][4];

    BOOL            m_bExit;

	public:
	CVolumeTest();
	~CVolumeTest();

	// Framework functions
    virtual bool SetDefaultMaterials(void);
    virtual bool SetDefaultRenderStates(void);
    virtual bool SetDefaultMatrices(void);
//	virtual bool ClearFrame(void);
//	virtual bool ExecuteTest(UINT);
    virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
    virtual bool TestTerminate(void);

    // Helper functions
    bool LoadSphere(void);
    void UpdateStatus(bool);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

// Texture Class definitions
class CVolTexTest: public CVolumeTest
{
    protected:
//	CVolumeImage VolImage;

    public:
	CVolTexTest();
	~CVolTexTest();

	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
};

// MipMap Class definitions
class CMipVolTest: public CVolumeTest
{
    protected:
//	CVolumeImage MipVolImage[3];

	public:
	CMipVolTest();
	~CMipVolTest();

	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
};

// TexGen Class definitions
class CVolTexGen: public CVolumeTest
{
    protected:
//	CVolumeImage VolImage;

    public:
	CVolTexGen();
	~CVolTexGen();

	virtual bool SetDefaultMatrices(void);
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
};

// Filter Class definitions
class CVolFilter: public CVolumeTest
{
    protected:
//	CVolumeImage MinVolImage;
//	CVolumeImage MagVolImage;
//	CVolumeImage MipVolImage[3];
	char szStr[100];

    public:
	CVolFilter();
	~CVolFilter();

	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);

    bool SetupFilterMode(int);
};

class CVolBlend: public CVolumeTest
{
    protected:
//	CVolumeImage VolImage;
//	CVolumeImage AlphaVolImage;
	char szStr[100];

    public:
	CVolBlend();
	~CVolBlend();

    virtual bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);

    bool ApplyMaterial(int, bool);
    bool SetupBlendMode(int);
};

#endif
