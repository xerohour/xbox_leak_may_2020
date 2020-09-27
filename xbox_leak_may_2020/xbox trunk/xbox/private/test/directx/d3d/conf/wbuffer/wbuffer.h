/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	WBuffer.h
 *
 ***************************************************************************/

#ifndef __WBUFFER_H__
#define __WBUFFER_H__

//include "TestFrameBase.h"
//#include "CD3DTest.h"

//USETESTFRAME

// Defines
#define MAX_TESTS	100

typedef struct _Buffer
{
    DWORD dwStencilBitMask; // stencil bit mask
    DWORD dwZBitMask;       // z bit mask
    struct _Buffer *pNext;  // address of BUFFER data
} BUFFER, *PBUFFER;

// Base Class definitions
class CWBufferTest: public CD3DTest
{
    struct LVERTEX_ {
        D3DVALUE     x;             /* Homogeneous coordinates */
        D3DVALUE     y;
        D3DVALUE     z;
        D3DCOLOR     color;         /* Vertex color */
        D3DCOLOR     specular;      /* Specular component of vertex */
        D3DVALUE     tu;            /* Texture coordinates */
        D3DVALUE     tv;

        LVERTEX_() { }
        LVERTEX_(const D3DVECTOR& v,
                    D3DCOLOR _color, D3DCOLOR _specular,
                    float _tu, float _tv)
            { x = v.x; y = v.y; z = v.z; 
              color = _color; specular = _specular;
              tu = _tu; tv = _tv;
            }
    };

    // Data
	LVERTEX_	WaffleList[2][42];
	LVERTEX_	PlaneList[4];
	char		msgString[80];

    CSurface8*  m_pd3dsOriginalZ;

    UINT        m_uZBufferFormats;
    D3DFORMAT   m_fmtd[4];

	public:
	CWBufferTest();
	~CWBufferTest();

	// Framework functions
//	virtual bool AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize);
	virtual bool SetDefaultRenderStates(void);
	virtual bool SetDefaultMatrices(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT uTestNum);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
};

#endif
