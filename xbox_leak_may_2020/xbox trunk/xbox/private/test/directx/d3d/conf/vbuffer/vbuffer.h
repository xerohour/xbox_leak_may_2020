/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	VBuffer.h
 *
 ***************************************************************************/

#ifndef __VBUFFER_H__
#define __VBUFFER_H__

//#include "TestFrameBase.h"
//#include "CD3DTest.h"

//USETESTFRAME

const int TL_NUMWIDTH  = 6;
const int TL_DESCWIDTH = 80;
const int TL_VALWIDTH  = 10;
const int TL_TOTALWIDTH = TL_NUMWIDTH + 3 + TL_DESCWIDTH + TL_VALWIDTH;

// constants
const int   NUMOBJECTS = 6;
const int   FRAMESPEROBJ = 16;
const int   NUMTESTS = (NUMOBJECTS * FRAMESPEROBJ);
const float ROTFACTOR = (2.0f * pi / FRAMESPEROBJ);

struct OBJINFO
{
	D3DPRIMITIVETYPE PrimType;   // primitive type
	UINT  nVertices;  // number of vertices in vertex buffer
	UINT  nStart;     // start of vertices in vertex buffer
	UINT  nIndices;   // number of indices in list
	WORD  *pIndices;  // pointer to indices
	TCHAR tcsName[17];
    CIndexBuffer8* pIBSrc;
};

// Class definition
class CVBufferTest: public TESTFRAME(CD3DTest)
{
private:
	OBJINFO   Objects[NUMOBJECTS];
	OBJINFO   Arrow;

	CVertexBuffer8 *pVBSrc;
	CVertexBuffer8 *pVBRef;

//    CLight        *pLight;
//	CMaterial     *pMaterial;

	UINT          nCurObj;
	UINT          nCurObjFrame;
	TCHAR         tcsTestDesc[TL_DESCWIDTH + 1];

public:
	CVBufferTest();
	~CVBufferTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ClearFrame(void);
	bool ExecuteTest(UINT);
	void SceneRefresh(void);
	bool ProcessFrame(void);
	bool TestTerminate(void);
	bool SetDefaultMatrices(void);
	bool SetDefaultMaterials(void);
	bool SetDefaultLights(void);
};

#endif


