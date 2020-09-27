/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Test.cpp

Abstract:

	Abstract class implementation to inherit from for individual tests

Author:

	Jeff Sullivan (jeffsul) 14-Jun-2001

Revision History:

	14-Jun-2001 jeffsul
		Initial Version

--*/

#include "Test.h"

#include <xdbg.h>
#include <xlog.h>
#include <macros.h>

#include <stdio.h>


//------------------------------------------------------------------------------
//	Inititialize static variable to FALSE so that XInitDevices() is called
//------------------------------------------------------------------------------

BOOL CTest::m_bInitDevicesCalled = FALSE;


/*++

	CTest constructor

Routine Description:

	constructor

Arguments:

	None

Return Value:

	None

--*/

CTest::CTest()
{
	m_bQuit = FALSE;
}


/*++

	CTest destructor

Routine Description:

	destructor

Arguments:

	None

Return Value:

	None

--*/

CTest::~CTest()
{
}


/*++

	Run

Routine Description:

	The test's main loop: initialize, enter loop, destroy

Arguments:

	None

Return Value:

	None

--*/

HRESULT CTest::Run()
{
	HRESULT hr = S_OK;

	// initialize all resources/devices
	EXECUTE( Init() );
	if ( FAILED( hr ) )
	{
		Destroy();
		return hr;
	}

	// loop for as long as test needs to run
	while ( !m_bQuit )
	{
		// calculate time change since last frame and update time variables
		DWORD dwTempTime = timeGetTime();
		m_dwDeltaTime = dwTempTime - m_dwCurrentTime;
		m_dwCurrentTime = dwTempTime;

		// do the real work of the test inside these functions
		EXECUTE( ProcessInput() )
		EXECUTE( Update() )
		EXECUTE( Render() )
	}

	// clean up 
	EXECUTE( Destroy() )

	return hr;
}


/*++

	Init

Routine Description:

	Common initialization function for all tests, calls inheritable functions to do
	initialization and starts timers

Arguments:

	None

Return Value:

	None

--*/

HRESULT CTest::Init()
{
	HRESULT hr = S_OK;

	EXECUTE( InitGraphics() )
	if ( FAILED ( hr ) )
	{
		return hr;
	}

	EXECUTE( InitAudio() )
	if ( FAILED ( hr ) )
	{
		DestroyGraphics();
		return hr;
	}

	if ( !m_bInitDevicesCalled )
	{
		// we only need to call this once for all tests, hence static declaration of m_bInitDevicesCalled
		XInitDevices( 0, NULL );
		m_bInitDevicesCalled = TRUE;
	}

	EXECUTE( InitInput() )
	if ( FAILED ( hr ) )
	{
		DestroyGraphics();
		DestroyAudio();
		return hr;
	}

	EXECUTE( InitResources() )
	if ( FAILED ( hr ) )
	{
		DestroyGraphics();
		DestroyAudio();
		DestroyInput();
		return hr;
	}

	m_dwStartTime = m_dwCurrentTime = timeGetTime(); 

	return hr;
}


/*++

	Destroy

Routine Description:

	Common de-allocation function for all tests, calls inheritable functions to do
	un-initialization/deletion

Arguments:

	None

Return Value:

	None

--*/

HRESULT CTest::Destroy()
{
	HRESULT hr = S_OK;

	EXECUTE( DestroyGraphics() )
	EXECUTE( DestroyAudio() )
	EXECUTE( DestroyInput() )
	EXECUTE( DestroyResources() )

	return hr;
}
