/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WmvApiTest.h

Abstract:

	General API tests including parameter tests.

Author:

	Jeff Sullivan (jeffsul) 13-Sep-2001

Revision History:

	13-Sep-2001 jeffsul
		Initial Version

--*/

#ifndef __WMVAPITEST_H__
#define __WMVAPITEST_H__

#include "..\WmvCommon\Test.h"

#include <wmvxmo.h>

//#include "..\WmvCommon\FrameQueue.h"
//#include "..\WmvCommon\bitfont.h"

#define WMV_API_TEST_NUM_FRAMES		16
#define WMV_API_TEST_NUM_PACKETS	48
#define WMV_API_TEST_PACKET_SIZE	4096

// Vertex definition for the screen-space quad
#define FVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1)
typedef struct {
    D3DXVECTOR4 p;
    D3DXVECTOR2 t;
} CUSTOMVERTEX;

typedef struct _TVERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
	FLOAT u, v;         // Texture coordinates
} TVERTEX;

class CWmvApiTest : public CTest
{
public:
	CWmvApiTest();
	~CWmvApiTest();

protected:
	HRESULT ReadIni();

private:
	VOID ExecuteParameterChecks();
};	
#endif // #ifndef __WMVAPITEST_H__ 
