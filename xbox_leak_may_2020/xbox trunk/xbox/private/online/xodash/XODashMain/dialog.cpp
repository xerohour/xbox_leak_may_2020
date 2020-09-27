/*************************************************************************************************\
CMsgDialog.cpp		: Implementation of the CMsgDialog and the CProgress Dialog
Creation Date		: 1/8/2002 11:43:08 AM
Copyright Notice	: (C) 2000 Microsoft
Author				: Heidi Gaertner
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/
#include "std.h"
#include "MsgDialog.h"
#include "XOConst.h"
#include "XBInput.h"
#include "SmartObject.h"
#include "tgl.h"
#include "SceneNavigator.h"
#include "globals.h"
#include "XODash.h"
#include "Dialog.h"

const TCHAR* FindErrorStrID(HRESULT hrError )
{
    // see if it's local, if so 
    // we could change this to be lots faster
    for ( int i = 0; i < ERROR_STR_COUNT; i++ )
    {
        if ( ErrorStringIDs[i].m_hrError == hrError )
        {
            // get the error out of the locale file
            return ErrorStringIDs[i].m_pErrorID;
        }
    }

    // for now ASSERT that we couldn't find it
    ASSERT( !"Missing error string\n" );

    // if we haven't found it go bugger a server as soon as the API's are done
    // but for now return NULL
    return NULL;
}

HRESULT RunDialog(MSGPARAMETERS* pData)
{
	HRESULT hr = CSceneNavigator::Instance()->RunDialog(pData);
	hr = g_pXBApp->Run();
	CSceneNavigator::Instance()->CleanupDialog();
	return hr;
}




HRESULT RunOKDlg( long ErrorID )
{
    return RunOKDlg( FindErrorStrID( ErrorID ), _T("OK") );
}
HRESULT RunOKDlg( const TCHAR* pMessageID, const TCHAR* pOKStrID )
{
    MSGPARAMETERS params;
    memset( &params, 0, sizeof( MSGPARAMETERS ) );
    params.m_SceneID = eMsgBoxSceneId;
    params.m_pStrMessageID = pMessageID;
    params.m_pStrCenterButtonID = pOKStrID;
    return RunDialog( &params );
}
HRESULT RunYesNoDlg( const TCHAR* pMessageID, const TCHAR* pYESStrID,
                   const TCHAR* pNoStrID, bool bUseBigButtons )
{
    MSGPARAMETERS params;
    memset( &params, 0, sizeof( MSGPARAMETERS ) );
    params.m_SceneID = eMsgBoxSceneId;
    params.m_pStrMessageID = pMessageID;
    params.m_bUseBigButtons = bUseBigButtons;
    params.m_pStrYesID = pYESStrID;
    params.m_pStrNoID = pNoStrID;
    
    return RunDialog( &params );
}
HRESULT RunProgressDlg( const TCHAR* pMessageID, ProgressFunction pFunction, const TCHAR* pCancelButtonStrID )
{
    MSGPARAMETERS params;
    memset( &params, 0, sizeof( MSGPARAMETERS ) );
    params.m_SceneID = eProgressBarSceneId;
    params.m_pStrMessageID = pMessageID;
    params.m_pFunction = pFunction;
    params.m_pStrCenterButtonID = pCancelButtonStrID;
    
    return RunDialog( &params );

}

HRESULT RunWaitCursor( const TCHAR* pMessageID, WaitFunction pFunction, const TCHAR* pCancelButtonStrID )
{
    MSGPARAMETERS params;
    memset( &params, 0, sizeof( MSGPARAMETERS ) );
    params.m_SceneID = pMessageID ? eConnectAnim : eAnimWait;
    params.m_pStrMessageID = pMessageID;
    params.m_pWaitFunction = pFunction;
    params.m_pStrCenterButtonID = pCancelButtonStrID;
    
    return RunDialog( &params );

}


//*************************************************************************************************
// end of file ( CMsgDialog.cpp )
