/*****************************************************
*** muformatthread.h
***
*** Header file for our MU Format Thread class.
*** This class will format an MU in a background
*** thread, and report the result to the caller
***
*** by James N. Helm
*** February 9th, 2001
***
*****************************************************/

#ifndef _MUFORMATTHREAD_H_
#define _MUFORMATTHREAD_H_

#include "cworkerthread.h"
#include "memoryunit.h"

class CMUFormatThread : public CWorkerThread
{
public:
    // Constructors and Destructor
    CMUFormatThread();
    ~CMUFormatThread();

    // Properties

    // Methods
    BOOL IsFormatComplete();                                                                    // Informs the caller if the format in progress is complete
    BOOL IsFormatSuccessful();                                                                  // Informs the caller if the format was successful or not
    void BeginFormat( CMemoryUnit* pMemoryUnit );                                              // Set the Port and the Slot you wish to format

private:

    // Properties
    BOOL m_bFormatComplete;             // TRUE if there is no format in progress, otherwise FALSE
    BOOL m_bFormatSuccessful;           // TRUE if the format was successful, otherwise FALSE
    CMemoryUnit* m_pMemoryUnit;         // Active Memory Unit

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
};

#endif // _MUFORMATTHREAD_H_