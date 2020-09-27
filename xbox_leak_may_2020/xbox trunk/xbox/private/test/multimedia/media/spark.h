/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Spark.cpp

Abstract:

	sparks

Author:

	Jason Gould (jgould) 19-May-2001

Revision History:

	19-May-2001	jgould
		Initial Version

--*/

#ifndef __SPARK_H__
#define __SPARK_H__

//------------------------------------------------------------------------------
//	Includes
//------------------------------------------------------------------------------
#include "xtl.h"
#include "xgmath.h"

namespace Media {

class CSpark
{
public:
	struct Vertex {
		XGVECTOR3 location;
		float age;
	};


private:
	Vertex* m_pVertex;
	XGVECTOR3 *m_pDirection;
	float *m_pSpeed;

	static D3DVertexBuffer* m_pVB;
	UINT m_Next;
	UINT m_First;
	void RemoveSpark();
    D3DLIGHT8               m_light; //use GetLight to set this up
	UINT m_LastLight;

public:
	CSpark() {}
	~CSpark() { Release(); }

	HRESULT Create (IN D3DDevice* pDevice);
	void Release();
	void AddSpark(XGVECTOR3& loc);
	void Update(IN float amount);
	void Render(D3DDevice* pDevice, XGMATRIX* pmViewProj);
	int GetNumSparks();
    D3DLIGHT8* GetLight(int iSpark);
};
}


#endif //__SPARK_H__
