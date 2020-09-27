/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WMVTest.h

Abstract:

	Created to test WMV on the xbox

Author:

	Jeff Sullivan (jeffsul) 18-Jul-2001

Revision History:

	18-Jul-2001 jeffsul
		Initial Version

--*/

#ifndef __WMVTEST_H__
#define __WMVTEST_H__

#include "Test.h"

#define WMVTEST_APP_NAME	"WMVTEST"
#define	WMVTEST_INI_PATH	"D:\\WMVTest.ini"

class CWMVTest : public CTest
{
public:
	CWMVTest();
	~CWMVTest();

protected:
	HRESULT ReadIni();
	HRESULT InitGraphics();
	HRESULT DestroyGraphics();

	HRESULT Update();
	HRESULT Render();

private:
	LPDIRECT3DDEVICE8		m_pDevice;
	CHAR					m_szPath[MAX_PATH];
	UINT					m_nNumFilesPlayed;
	HANDLE					m_hFiles;
	WIN32_FIND_DATA			m_FindData;
};
	
#endif // #ifndef __WMVTEST_H__
