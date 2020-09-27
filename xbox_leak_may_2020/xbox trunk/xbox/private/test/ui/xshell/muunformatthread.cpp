/*****************************************************
*** muformatthread.cpp
***
*** CPP file for our MU Format Thread class.
*** This class will format an MU in a background
*** thread, and report the result to the caller
***
*** by James N. Helm
*** February 9th, 2001
***
*****************************************************/
#include "stdafx.h"
#include "muunformatthread.h"

// Constructor
CMUUnFormatThread::CMUUnFormatThread() :
m_bUnFormatComplete( TRUE ),
m_bUnFormatSuccessful( FALSE ),
m_pMemoryUnit( NULL )
{
}

// Destructor
CMUUnFormatThread::~CMUUnFormatThread()
{
}

/*****************************************************************************

Routine Description:

    ThreadFunct

    This function will format an MU and store the result of the format

Arguments:

    none

Return Value:


Notes:

    See CWorkerThread.h & CWorkerThread.cpp for more information on this
    member function.

*****************************************************************************/
DWORD CMUUnFormatThread::ThreadFunct(void)
{
    // Format the MU

    BOOL bFormatSuccessful;
    if( FAILED( m_pMemoryUnit->Unformat() ) )
    {
        bFormatSuccessful = FALSE;
    }
    else
    {
        bFormatSuccessful = TRUE;
    }
    
    Lock();

    m_bUnFormatSuccessful = bFormatSuccessful;
    m_bUnFormatComplete = TRUE;
    
    Unlock();

    XDBGTRC( APP_TITLE_NAME_A, "CMUUnFormatThread::ThreadFunct():Exiting..." );
    
    return 0;
}


// Informs the caller if the format in progress is complete
BOOL CMUUnFormatThread::IsUnFormatComplete()
{
    BOOL bReturn;

    Lock(); // Protect our memory
    
    bReturn = m_bUnFormatComplete;

    Unlock();

    return bReturn;
}


// Informs the caller if the format was successful or not
BOOL CMUUnFormatThread::IsUnFormatSuccessful()
{
    BOOL bReturn;

    Lock(); // Protect our memory
    
    bReturn = m_bUnFormatSuccessful;

    Unlock();

    return bReturn;
}


// Set the Port and the Slot you wish to format, and begin the format
void CMUUnFormatThread::BeginUnFormat( CMemoryUnit* pMemoryUnit )
{
    // Verify we have a valid pointer
    if( !pMemoryUnit )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CMUUnFormatThread::BeginFormat():Invalid pointer passed in!!" );

        return;
    }

    Lock(); // Protect our memory

    // Make sure we don't start the thread twice
    if( TRUE == m_bUnFormatComplete )
    {
        m_pMemoryUnit = pMemoryUnit;

        m_bUnFormatSuccessful = FALSE;
        m_bUnFormatComplete = FALSE;

        // Begin the Format
        Run();
    }

    Unlock();
}
