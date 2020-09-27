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
#include "muformatthread.h"

// Constructor
CMUFormatThread::CMUFormatThread() :
m_bFormatComplete( TRUE ),
m_bFormatSuccessful( FALSE ),
m_pMemoryUnit( NULL )
{
}

// Destructor
CMUFormatThread::~CMUFormatThread()
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
DWORD CMUFormatThread::ThreadFunct(void)
{
    // Format the MU

    BOOL bFormatSuccessful;
    if( FAILED( m_pMemoryUnit->Format( TRUE ) ) )
    {
        bFormatSuccessful = FALSE;
    }
    else
    {
        bFormatSuccessful = TRUE;
    }
    
    Lock();

    m_bFormatSuccessful = bFormatSuccessful;
    m_bFormatComplete = TRUE;
    
    Unlock();

    XDBGTRC( APP_TITLE_NAME_A, "CMUFormatThread::ThreadFunct():Exiting..." );
    
    return 0;
}


// Informs the caller if the format in progress is complete
BOOL CMUFormatThread::IsFormatComplete()
{
    BOOL bReturn;

    Lock(); // Protect our memory
    
    bReturn = m_bFormatComplete;

    Unlock();

    return bReturn;
}


// Informs the caller if the format was successful or not
BOOL CMUFormatThread::IsFormatSuccessful()
{
    BOOL bReturn;

    Lock(); // Protect our memory
    
    bReturn = m_bFormatSuccessful;

    Unlock();

    return bReturn;
}


// Set the Port and the Slot you wish to format, and begin the format
void CMUFormatThread::BeginFormat( CMemoryUnit* pMemoryUnit )
{
    // Verify we have a valid pointer
    if( !pMemoryUnit )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CMUFormatThread::BeginFormat():Invalid pointer passed in!!" );

        return;
    }

    Lock(); // Protect our memory

    // Make sure we don't start the thread twice
    if( TRUE == m_bFormatComplete )
    {
        m_pMemoryUnit = pMemoryUnit;

        m_bFormatSuccessful = FALSE;
        m_bFormatComplete = FALSE;

        // Begin the Format
        Run();
    }

    Unlock();
}
