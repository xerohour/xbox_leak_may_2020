//-----------------------------------------------------------------------------
// File: XbOnlineTask.h
//
// Desc: Wraps online task HANDLE object
//
// Hist: 09.04.01 - New for Nov XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBONLINE_TASK_H
#define XBONLINE_TASK_H

#include <xtl.h>
#include <xonline.h>




//-----------------------------------------------------------------------------
// Name: class OnlineTask
// Desc: Xbox online task object
//-----------------------------------------------------------------------------
class CXBOnlineTask
{
    XONLINETASK_HANDLE m_hOnlineTask;

public:

    explicit CXBOnlineTask( XONLINETASK_HANDLE = NULL );
    ~CXBOnlineTask();

    operator XONLINETASK_HANDLE() const;
    XONLINETASK_HANDLE* operator&();

    BOOL    IsOpen() const;
    BOOL    IsClosed() const;
    void    Close();
    HRESULT Continue();

private:

    CXBOnlineTask( const CXBOnlineTask& );
    CXBOnlineTask& operator=( const CXBOnlineTask& );

};

#endif // XBONLINE_TASK_H
