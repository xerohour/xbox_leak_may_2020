/*****************************************************
*** xbedirmonitor.h
***
*** Header file for our Directory Monitor class.
*** This class will monitor an XBE directory for
*** changes so that our XBE list can be kept up to
*** date
***
*** by James N. Helm
*** December 13th, 2000
***
*****************************************************/

#ifndef _XBEDIRMONITOR_H_
#define _XBEDIRMONITOR_H_

#include "cworkerthread.h"
#include "xbelist.h"

class CXBEDirMonitor : public CWorkerThread
{
public:
    // Constructors and Destructor
    CXBEDirMonitor();
    ~CXBEDirMonitor();

    // Properties
    // Methods
    CXBEList* GetXBEListPtr();          // Return a pointer to the most up to date XBE List
    void Init();                        // Initialize the Object
    BOOL SoftBreak(DWORD dwTimeOut);    // Used to stop the thread early

private:
    // Properties
    CXBEList m_XBEList1;                // List of XBE's that are found in the directory
    CXBEList m_XBEList2;                // List of XBE's that are found in the directory
    BOOL m_bList1InUse;                 // Used to determine if we are currently using list one, or list two
    CXBEList* m_pXBEList;               // XBE List currently in use by this class
    ULONG m_LastTrayStateChangeCount;

    // Methods
    /*****************************************************************************
    NOTE: in the implementation of this function, the user must check
           the bExit flag and exit gracefully if set:
    while(1)
        {
        if(GetExitFlag()) break;
        }
    *****************************************************************************/
    DWORD ThreadFunct(void);
    void PollTrayState(void);
};

#endif // _XBEDIRMONITOR_H_
