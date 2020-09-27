/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WmvMemTest.h

Abstract:

	General MEM tests including parameter tests.

Author:

	Jeff Sullivan (jeffsul) 13-Sep-2001

Revision History:

	13-Sep-2001 jeffsul
		Initial Version

--*/

#ifndef __WMVMEMTEST_H__
#define __WMVMEMTEST_H__

#include "..\WmvCommon\Test.h"

#include <wmvxmo.h>

//#include "..\WmvCommon\FrameQueue.h"
//#include "..\WmvCommon\bitfont.h"

#define WMV_MEM_TEST_NUM_FRAMES		16
#define WMV_MEM_TEST_NUM_PACKETS	48
#define WMV_MEM_TEST_PACKET_SIZE	4096
#define WMV_MEM_TEST_NUM_TESTS		1000
#define WMV_MEM_TEST_LARGE_MEMORY_SIZE	(1024*1024*120)

class CWmvMemTest : public CTest
{
public:
	CWmvMemTest();
	~CWmvMemTest();

protected:
	HRESULT ReadIni();

private:

	VOID ExecuteMemoryChecks();
	VOID 
	CheckProcessMultiple( 
		LPWMVDECODER	pWmvDecoder,
		LPCXMEDIAPACKET pVideoOutputPacket,
		LPCXMEDIAPACKET pAudioOutputPacket,
		UINT			nNumRuns, 
		BOOL			bDecodeEntire 
	);
};	
#endif // #ifndef __WMVMEMTEST_H__ 
