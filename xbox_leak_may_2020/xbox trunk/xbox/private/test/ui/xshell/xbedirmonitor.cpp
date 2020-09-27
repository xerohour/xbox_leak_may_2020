/*****************************************************
*** xbedirmonitor.cpp
***
*** CPP file for our Directory Monitor class.
*** This class will monitor an XBE directory for
*** changes so that our XBE list can be kept up to
*** date
***
*** by James N. Helm
*** December 13th, 2000
***
*****************************************************/

#include "stdafx.h"
#include "xbedirmonitor.h"

// Constructor
CXBEDirMonitor::CXBEDirMonitor()
: m_bList1InUse( FALSE ),
m_pXBEList( NULL ),
m_LastTrayStateChangeCount( 0 )
{
}

// Destructor
CXBEDirMonitor::~CXBEDirMonitor()
{
}

/*****************************************************************************

Routine Description:

    ThreadFunct

    This function will monitor the XBE directory for changes, and re-populate
    itself at each interval

Arguments:

    none

Return Value:


Notes:

    See CWorkerThread.h & CWorkerThread.cpp for more information on this
    member function.

*****************************************************************************/
DWORD CXBEDirMonitor::ThreadFunct(void)
{
    while( !GetExitFlag() )
    {
        // Re-poll our directory if needed
        Sleep( EXECUTABLE_DIRECTORY_POLL_INTERVAL );
        m_pXBEList->Init( FILE_EXECUTABLE_DIRECTORY_A, FILE_EXECUTABLE_EXENSION_A );

        Lock();

        if( m_bList1InUse )
        {
            m_bList1InUse = FALSE;
            m_pXBEList = &m_XBEList2;
        }
        else
        {
            m_bList1InUse = TRUE;
            m_pXBEList = &m_XBEList1;
        }

        Unlock();

        PollTrayState();
    }

    return 0;
}


// This function will initialize the object to a ready state, and should
// be called before calling "Run()" on the thread
void CXBEDirMonitor::Init()
{
    m_bList1InUse = TRUE;               // Default to using list #1
    m_pXBEList    = &m_XBEList1;        // XBE List currently in use by this class (List 1)

    // Initialize our XBE Lists
    m_XBEList1.Init( FILE_EXECUTABLE_DIRECTORY_A, FILE_EXECUTABLE_EXENSION_A );
    m_XBEList2.Init( FILE_EXECUTABLE_DIRECTORY_A, FILE_EXECUTABLE_EXENSION_A );
}


// This function will return a pointer to the most up to date XBE List
CXBEList* CXBEDirMonitor::GetXBEListPtr()
{
    CXBEList* pXBEReturn = NULL;

    Lock(); // Protect our memory

    if( m_bList1InUse )
    {
        pXBEReturn = &m_XBEList2;
    }
    else
    {
        pXBEReturn = &m_XBEList1;
    }

    Unlock();

    return pXBEReturn;
}

void CXBEDirMonitor::PollTrayState(void)
{
    ULONG TrayState;
    ULONG TrayStateChangeCount;
    OBJECT_STRING DeviceName;

    // Read the SMC tray state
    if (NT_SUCCESS(HalReadSMCTrayState(&TrayState, &TrayStateChangeCount))) {

        // If something has changed, then force a dismount of the DVD
        if (TrayStateChangeCount != m_LastTrayStateChangeCount) {
            RtlInitObjectString(&DeviceName, "\\??\\CdRom0:");
            IoDismountVolumeByName(&DeviceName);
            m_LastTrayStateChangeCount = TrayStateChangeCount;
        }
    }
}


BOOL CXBEDirMonitor::SoftBreak(DWORD dwTimeOut)
{
    Lock();
    m_XBEList1.SetStopXBEEnumeration( TRUE );
    m_XBEList2.SetStopXBEEnumeration( TRUE );
    Unlock();

    return CWorkerThread::SoftBreak( dwTimeOut );
}
