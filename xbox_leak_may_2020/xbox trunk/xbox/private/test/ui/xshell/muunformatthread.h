/*****************************************************
*** muunformatthread.h
***
*** Header file for our MU Format Thread class.
*** This class will format an MU in a background
*** thread, and report the result to the caller
***
*** by James N. Helm
*** February 9th, 2001
***
*****************************************************/

#ifndef _MUUNFORMATTHREAD_H_
#define _MUUNFORMATTHREAD_H_

#include "cworkerthread.h"
#include "memoryunit.h"

class CMUUnFormatThread : public CWorkerThread
{
public:
    // Constructors and Destructor
    CMUUnFormatThread();
    ~CMUUnFormatThread();

    // Properties

    // Methods
    BOOL IsUnFormatComplete();                                                                    // Informs the caller if the format in progress is complete
    BOOL IsUnFormatSuccessful();                                                                  // Informs the caller if the format was successful or not
    void BeginUnFormat( CMemoryUnit* pMemoryUnit );                                              // Set the Port and the Slot you wish to format

private:

    // Properties
    BOOL m_bUnFormatComplete;             // TRUE if there is no format in progress, otherwise FALSE
    BOOL m_bUnFormatSuccessful;           // TRUE if the format was successful, otherwise FALSE
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

#endif // _MUUNFORMATTHREAD_H_