//-----------------------------------------------------------------------------
// File: ScriptProxy.cpp
//
// Desc: Methods to proxy a DirectMusic script onto the Xbox
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBUtil.h>
#include <XBFont.h>
#include <XBHelp.h>
#include "dmusicip.h"
#include "DebugCmd.h"
#include "DMConsole.h"
#include <xbdm.h>
//#include <objbase.h>
//#include "..\..\..\dmusic\dmscript\dmscript.h"

extern CRITICAL_SECTION g_cs;
extern CXBoxSample g_xbApp;
extern void SendNotification( char *strName, DWORD dwData1, DWORD dwData2 );

CScriptItem *GetScriptFromID( DWORD dwScriptID )
{
	// Ensure this script ID doesn't exist
	CScriptItem *pScriptItem = (CScriptItem *)g_xbApp.m_lstScripts.GetHead();
	while( pScriptItem )
	{
		if( dwScriptID == pScriptItem->m_dwScriptID )
		{
			return pScriptItem;
		}
		pScriptItem = pScriptItem->GetNext();
	}

	return NULL;
}


//-----------------------------------------------------------------------------
// Name: RCmdScriptInit
// Desc: initializes a script
//-----------------------------------------------------------------------------

void RCmdScriptInit(int argc, char *argv[])
{
	HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 3 )
    {
        DCCMDPrintf("Need to specify the ID and filename\n");
        hr = E_INVALIDARG;
    }

    DWORD dwScriptID = 0xFFFFFFFF;
	
	if( SUCCEEDED(hr) )
	{
		dwScriptID = atoi(argv[1]);

		// Ensure this script ID doesn't exist
		if( GetScriptFromID( dwScriptID ) )
		{
			OutputDebugString( "RCmdScriptInit: Script ID already in use\n" );
			hr = E_INVALIDARG;
		}
	}

    // Load script
    IDirectMusicScript8 *pIDirectMusicScript8 = NULL;
	if( SUCCEEDED(hr) )
	{
		hr = g_pLoader->LoadObjectFromFile( CLSID_DirectMusicScript, IID_IDirectMusicScript8, 
											argv[2], (VOID**)&pIDirectMusicScript8 );
	}
 
	DMUS_SCRIPT_ERRORINFO dmScriptErrorInfo;
	ZeroMemory( &dmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO) );
	dmScriptErrorInfo.dwSize = sizeof(DMUS_SCRIPT_ERRORINFO);
	if( SUCCEEDED( hr ) )
	{
	    // Initialize
        hr = pIDirectMusicScript8->Init( g_pPerformance, &dmScriptErrorInfo );
    }

	if( SUCCEEDED( hr ) )
	{
		// Add script to list
        CScriptItem *pScriptItem = new CScriptItem;
        pScriptItem->m_dwScriptID = dwScriptID;
        pScriptItem->m_pScript8 = pIDirectMusicScript8;
		pScriptItem->m_pScript8->AddRef();
        g_xbApp.m_lstScripts.AddHead( pScriptItem );
	}

	// Release script
	if( pIDirectMusicScript8 )
	{
		pIDirectMusicScript8->Release();
	}

    // Pass the result back to the Windows application
	SendNotification( "!Script.InitResult ", dwScriptID, hr );
}


//-----------------------------------------------------------------------------
// Name: RCmdScriptRelease
// Desc: Releases a script
//-----------------------------------------------------------------------------

void RCmdScriptRelease(int argc, char *argv[])
{
	HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 2 )
    {
        DCCMDPrintf("Need to specify the ID\n");
        hr = E_INVALIDARG;
    }

    DWORD dwScriptID = 0xFFFFFFFF;
	CScriptItem *pScriptItem = NULL;
	if( SUCCEEDED(hr) )
	{
		dwScriptID = atoi(argv[1]);

		// Ensure this script ID exists
		pScriptItem = GetScriptFromID( dwScriptID );
	}

	if( NULL == pScriptItem )
	{
        OutputDebugString( "RCmdScriptRelease: Script ID not found\n" );
		hr = E_INVALIDARG;
	}

    // Release script
	if( SUCCEEDED(hr) )
	{
		pScriptItem->m_pScript8->Release();
		pScriptItem->m_pScript8 = NULL;

		// Remove the item from the list
		g_xbApp.m_lstScripts.Remove( pScriptItem );

		// Delete the item
		delete pScriptItem;
	}
 
    // Pass the result back to the Windows application
	SendNotification( "!Script.ReleaseResult ", dwScriptID, hr );
}


//-----------------------------------------------------------------------------
// Name: RCmdScriptEnumVariable
// Desc: Enumerates a variable
//-----------------------------------------------------------------------------

void RCmdScriptEnumVariable(int argc, char *argv[])
{
	HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 3 )
    {
        DCCMDPrintf("Need to specify the ID and variable index\n");
		hr = E_INVALIDARG;
    }

    DWORD dwScriptID = 0xFFFFFFFF;
	CScriptItem *pScriptItem = NULL;
	if( SUCCEEDED(hr) )
	{
		dwScriptID = atoi(argv[1]);

		// Ensure this script ID exists
		pScriptItem = GetScriptFromID( dwScriptID );
	}

	if( NULL == pScriptItem )
	{
        OutputDebugString( "RCmdScriptEnumVariable: Script ID not found\n" );
		hr = E_INVALIDARG;
	}

	// Query the script
	if( SUCCEEDED(hr) )
	{
		// TODO:
		//DWORD dwVariableID = atoi(argv[2]);
		//WCHAR wcstrName[MAX_PATH];
		//hr = pScriptItem->m_pScript8->EnumVariable( dwVariableID, wcstrName );
	}

	// TODO: Pass the value back

    // Pass the result back to the Windows application
	SendNotification( "!Script.EnumVariableResult ", dwScriptID, hr );
}


//-----------------------------------------------------------------------------
// Name: RCmdScriptGetVariableVariant
// Desc: Gets a variable
//-----------------------------------------------------------------------------

void RCmdScriptGetVariableVariant(int argc, char *argv[])
{
	HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 3 )
    {
        DCCMDPrintf("Need to specify the ID and variable index\n");
		hr = E_INVALIDARG;
    }

    DWORD dwScriptID = 0xFFFFFFFF;
	CScriptItem *pScriptItem = NULL;
	if( SUCCEEDED(hr) )
	{
		dwScriptID = atoi(argv[1]);

		// Ensure this script ID exists
		pScriptItem = GetScriptFromID( dwScriptID );
	}

	if( NULL == pScriptItem )
	{
        OutputDebugString( "RCmdScriptGetVariableVariant: Script ID not found\n" );
		hr = E_INVALIDARG;
	}

	// Query the script
	if( SUCCEEDED(hr) )
	{
		// TODO:
		//VARIANT varValue;
		//DMUS_SCRIPT_ERRORINFO dmScriptErrorInfo;
		//ZeroMemory( &dmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO) );
		//dmScriptErrorInfo.dwSize = sizeof(DMUS_SCRIPT_ERRORINFO);
		//WCHAR wcstrVariableName[MAX_PATH];
		//wsprintfW( wcstrVariableName, L"%S", argv[2] );
		//hr = pScriptItem->m_pScript8->GetVariableVariant( wcstrVariableName, &varValue, &dmScriptErrorInfo );
	}

	// TODO: Pass the value back

    // Pass the result back to the Windows application
	SendNotification( "!Script.GetVariableVariantResult ", dwScriptID, hr );
}


//-----------------------------------------------------------------------------
// Name: RCmdScriptSetVariableVariant
// Desc: Sets a variable
//-----------------------------------------------------------------------------

void RCmdScriptSetVariableVariant(int argc, char *argv[])
{
	// TODO:
}


//-----------------------------------------------------------------------------
// Name: RCmdScriptEnumRoutine
// Desc: Enumerates a routine
//-----------------------------------------------------------------------------

void RCmdScriptEnumRoutine(int argc, char *argv[])
{
	HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 3 )
    {
        DCCMDPrintf("Need to specify the ID and routine index\n");
		hr = E_INVALIDARG;
    }

    DWORD dwScriptID = 0xFFFFFFFF;
	CScriptItem *pScriptItem = NULL;
	if( SUCCEEDED(hr) )
	{
		dwScriptID = atoi(argv[1]);

		// Ensure this script ID exists
		pScriptItem = GetScriptFromID( dwScriptID );
	}

	if( NULL == pScriptItem )
	{
        OutputDebugString( "RCmdScriptEnumRoutine: Script ID not found\n" );
		hr = E_INVALIDARG;
	}

	// Query the script
	if( SUCCEEDED(hr) )
	{
		// TODO:
		//DWORD dwRoutineID = atoi(argv[2]);
		//WCHAR wcstrName[MAX_PATH];
		//hr = pScriptItem->m_pScript8->EnumRoutine( dwRoutineID, wcstrName );
	}

	// TODO: Pass the value back

    // Pass the result back to the Windows application
	SendNotification( "!Script.EnumRoutineResult ", dwScriptID, hr );
}


//-----------------------------------------------------------------------------
// Name: RCmdScriptCallRoutine
// Desc: Calls a routine
//-----------------------------------------------------------------------------

void RCmdScriptCallRoutine(int argc, char *argv[])
{
	HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 3 )
    {
        DCCMDPrintf("Need to specify the ID and routine name\n");
		hr = E_INVALIDARG;
    }

    DWORD dwScriptID = 0xFFFFFFFF;
	CScriptItem *pScriptItem = NULL;
	if( SUCCEEDED(hr) )
	{
		dwScriptID = atoi(argv[1]);

		// Ensure this script ID exists
		pScriptItem = GetScriptFromID( dwScriptID );
	}

	if( NULL == pScriptItem )
	{
        OutputDebugString( "RCmdScriptCallRoutine: Script ID not found\n" );
		hr = E_INVALIDARG;
	}

	// Query the script
	if( SUCCEEDED(hr) )
	{
		// TODO:
		DMUS_SCRIPT_ERRORINFO dmScriptErrorInfo;
		ZeroMemory( &dmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO) );
		dmScriptErrorInfo.dwSize = sizeof(DMUS_SCRIPT_ERRORINFO);
		hr = pScriptItem->m_pScript8->CallRoutine( argv[2], &dmScriptErrorInfo );
	}

	// TODO: Pass the error info back

    // Pass the result back to the Windows application
	SendNotification( "!Script.CallRoutineResult ", dwScriptID, hr );
}
