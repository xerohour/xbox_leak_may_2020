/*++

Copyright (c) Microsoft Corporation

Module Name:

    debug.h

Abstract:
    
    Debug Facilities.

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    03-27-2001 : created

--*/


//
// Which stuff do you want to turn on for debug builds
//
#ifdef DBG
//#define SPEW_ENTER_STUFF   //Define to spew on entry to functions
#define OBJECT_TRACKING      //Define to track object (for finding leaks)
#endif

//----------------------------------------------------------------------
//  Debug Spew
//----------------------------------------------------------------------
#ifdef SPEW_ENTER_STUFF
#define RETURN(_RetValue_)\
    ATLTRACE("%s returning 0x%0.8x\n", __FUNCTION__, _RetValue_);\
    return _RetValue_;

#define ENTER_SPEW ATLTRACE("Enter %s\n", __FUNCTION__);
#else
#define RETURN(_RetValue_) return _RetValue_;
#define ENTER_SPEW
#endif

//----------------------------------------------------------------------
//  Object Tracking
//----------------------------------------------------------------------
#ifdef OBJECT_TRACKING

struct CTrackableObject
{
 CTrackableObject();
 ~CTrackableObject();
 CTrackableObject *m_pNext;
 CTrackableObject *m_pPrevious;
 LPCSTR m_pszClassName;
};

struct CObjectTracker
{
  CObjectTracker() : m_pHead(NULL), m_pTail(NULL), m_iObjectCount(0){InitializeCriticalSection(&m_CriticalSection);}
  ~CObjectTracker();
  void DumpObjects();
  CTrackableObject *m_pHead;
  CTrackableObject *m_pTail;
  int m_iObjectCount;
  CRITICAL_SECTION m_CriticalSection;
};

extern CObjectTracker g_ObjectTracker;


// Macros for declarations
#define CTRACKABLEOBJECT ,public CTrackableObject
#define INIT_TRACKABLEOBJECT_NAME(_ClassName_) m_pszClassName=#_ClassName_;
#define DUMP_TRACKED_OBJECTS() g_ObjectTracker.DumpObjects()

#else //OBJECT_TRACKING not defined

#define CTRACKABLEOBJECT
#define INIT_TRACKABLEOBJECT_NAME(_ClassName_)
#define DUMP_TRACKED_OBJECTS()

#endif
