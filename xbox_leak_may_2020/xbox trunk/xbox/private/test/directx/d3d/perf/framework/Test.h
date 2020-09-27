/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Test.h

Abstract:

	Abstract class to inherit from for individual tests

Author:

	Jeff Sullivan (jeffsul) 14-Jun-2001

Revision History:

	14-Jun-2001 jeffsul
		Initial Version

--*/

#ifndef __TEST_H__
#define __TEST_H__

#include <xtl.h>

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

class CTest
{
public:

	CTest();												//	constructor: for now only sets m_bQuit to FALSE
	virtual ~CTest();										//	virtually destroy class (and inherited class)
	HRESULT Run();											//	the test's main loop

protected:

	//------------------------------------------------------------------------------
	//	Overridable functions to do the test's initialization/cleanup (before and
	//	after main loop)
	//------------------------------------------------------------------------------

	virtual HRESULT InitGraphics() { return S_OK; }			//	init d3d here (i.e., create a device, etc.)
	virtual HRESULT DestroyGraphics() { return S_OK; }		//	cleanup anything created in InitGraphics() here
	virtual HRESULT InitInput() { return S_OK; }			//	init input devices (i.e., call XInputOpen(), not XInitDevices())
	virtual HRESULT DestroyInput() { return S_OK; }			//	cleanup anything created in InitInput() here
	virtual HRESULT InitAudio() { return S_OK; }			//	init audio devices here
	virtual HRESULT DestroyAudio() { return S_OK; }			//	cleanup anything created in InitAudio() here
	virtual HRESULT InitResources() { return S_OK; }		//	init resources (models, textures, sound files, etc.) here
	virtual HRESULT DestroyResources() { return S_OK; }		//	cleanup anything created in InitResources() here


	//------------------------------------------------------------------------------
	//	Overidable functions to do the test's real work (inside the main loop)
	//------------------------------------------------------------------------------

	virtual HRESULT ProcessInput() { return S_OK; }			//	should be used to get any input from devices and process it
	virtual HRESULT Update() { return S_OK; }				//	any updates that need to be performed each frame are done here
	virtual HRESULT Render() { return S_OK; }				//	draw everything to the screen here


	//------------------------------------------------------------------------------
	//	Protected data members that are useful to the inherited classes
	//------------------------------------------------------------------------------

	BOOL				m_bQuit;							//	loop continues while FALSE
	DWORD				m_dwStartTime;						//	start time of test
	DWORD				m_dwCurrentTime;					//	current time of test
	DWORD				m_dwDeltaTime;						//	time between current frame and previous

private:

	//------------------------------------------------------------------------------
	//	Private data member that should not be visible to inherited classes
	//------------------------------------------------------------------------------

	HRESULT Init();											//	calls overridable functions to do all initialization
	HRESULT Destroy();										//	calls overridable functions to do all cleanup

	static BOOL			m_bInitDevicesCalled;				//	keeps track of whether XInitDevices() has been called
};
	
#endif // #ifndef __TEST_H__
