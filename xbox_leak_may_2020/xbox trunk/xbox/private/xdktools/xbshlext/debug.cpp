/*++

Copyright (c) Microsoft Corporation

Module Name:

    debug.cpp

Abstract:

    Implementation of some debug facilities

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    08-09-2001 : created (mitchd)

--*/

#include <stdafx.h>

#ifdef OBJECT_TRACKING

//
//  Declare the global object tracker.
//
CObjectTracker g_ObjectTracker;

//
//  Dump Objects before the d'tor on the global is called.
//
CObjectTracker::~CObjectTracker()
{
    DUMP_TRACKED_OBJECTS();
    DeleteCriticalSection(&m_CriticalSection);
}


//
//  Trackable Object insert themselves on contruction.
//
CTrackableObject::CTrackableObject() : 
    m_pszClassName(NULL), m_pNext(NULL), m_pPrevious(NULL)
{
    EnterCriticalSection(&g_ObjectTracker.m_CriticalSection);
    if(g_ObjectTracker.m_pHead)
    {
        m_pPrevious = g_ObjectTracker.m_pTail;
        g_ObjectTracker.m_pTail->m_pNext = this;
        g_ObjectTracker.m_pTail = this;
    } else
    {
        g_ObjectTracker.m_pHead = g_ObjectTracker.m_pTail = this;
    }
    g_ObjectTracker.m_iObjectCount++;
    LeaveCriticalSection(&g_ObjectTracker.m_CriticalSection);
}

//
//  Trackable Object remove themselves on destruction.
//
CTrackableObject::~CTrackableObject()
{
    EnterCriticalSection(&g_ObjectTracker.m_CriticalSection);
    g_ObjectTracker.m_iObjectCount--;
    if(m_pPrevious)
    {
        m_pPrevious->m_pNext = m_pNext;
    } else
    {
        g_ObjectTracker.m_pHead = m_pNext;
    }
    if(m_pNext)
    {
        m_pNext->m_pPrevious = m_pPrevious;
    } else
    {
        g_ObjectTracker.m_pTail = m_pPrevious;
    }
    LeaveCriticalSection(&g_ObjectTracker.m_CriticalSection);
}

//
//  The object tracker can dump its objects
//
void CObjectTracker::DumpObjects()
{
    EnterCriticalSection(&m_CriticalSection);
    CTrackableObject *pObject = m_pHead;
    if(m_iObjectCount)
    {
        ATLTRACE("Dumping %d objects still allocated\n", m_iObjectCount);
        while(pObject)
        {
            ATLTRACE("OBJ: %s(0x%0.8x)\n", pObject->m_pszClassName, pObject);
            pObject = pObject->m_pNext;
        }
    } else
    {
        ATLTRACE("There are no objects still allocated\n");
    }
    LeaveCriticalSection(&m_CriticalSection);
}

#endif