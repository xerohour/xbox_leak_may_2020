//-----------------------------------------------------------------------------
// File: XbOnlineTask.cpp
//
// Desc: Wraps online task HANDLE object
//
// Hist: 09.04.01 - New for Nov XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "XbOnlineTask.h"
#include <cassert>




//-----------------------------------------------------------------------------
// Name: CXBOnlineTask()
// Desc: Associate existing online task handle with object. Object will close
//       the handle when it is destroyed.
//-----------------------------------------------------------------------------
CXBOnlineTask::CXBOnlineTask( XONLINETASK_HANDLE hOnlineTask )
:
    m_hOnlineTask( hOnlineTask )
{
}




//-----------------------------------------------------------------------------
// Name: ~CXBOnlineTask()
// Desc: Close and release online task handle
//-----------------------------------------------------------------------------
CXBOnlineTask::~CXBOnlineTask()
{
    Close();
}




//-----------------------------------------------------------------------------
// Name: XONLINETASK_HANDLE()
// Desc: Direct access
//-----------------------------------------------------------------------------
CXBOnlineTask::operator XONLINETASK_HANDLE() const
{
    return m_hOnlineTask;
}




//-----------------------------------------------------------------------------
// Name: op&()
// Desc: Direct access to task handle
//-----------------------------------------------------------------------------
XONLINETASK_HANDLE* CXBOnlineTask::operator&()
{
    // If we're passing the task handle to a function that's changing it, the
    // handle should be closed (NULL). If this assertion fails, it's because
    // Close() -- i.e. XOnlineTaskCloseHandle -- wasn't called.
    assert( m_hOnlineTask == NULL );
    return &m_hOnlineTask;
}




//-----------------------------------------------------------------------------
// Name: IsOpen()
// Desc: TRUE if online task is open (being used)
//-----------------------------------------------------------------------------
BOOL CXBOnlineTask::IsOpen() const
{
    return( m_hOnlineTask != NULL );
}




//-----------------------------------------------------------------------------
// Name: IsClosed()
// Desc: TRUE if online task is closed (not in use)
//-----------------------------------------------------------------------------
BOOL CXBOnlineTask::IsClosed() const
{
    return( m_hOnlineTask == NULL );
}




//-----------------------------------------------------------------------------
// Name: Close()
// Desc: Close online task
//-----------------------------------------------------------------------------
void CXBOnlineTask::Close()
{
    if( m_hOnlineTask != NULL )
    {
        XOnlineTaskClose( m_hOnlineTask );
        m_hOnlineTask = NULL;
    }
}




//-----------------------------------------------------------------------------
// Name: Continue()
// Desc: Pump the task 
//-----------------------------------------------------------------------------
HRESULT CXBOnlineTask::Continue()
{
   return XOnlineTaskContinue( m_hOnlineTask );
}






