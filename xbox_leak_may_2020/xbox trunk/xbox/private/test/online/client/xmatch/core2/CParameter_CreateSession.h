#ifndef __CPARAMETER_CREATESESSION_H__
#define __CPARAMETER_CREATESESSION_H__

#include "matchcoretest.h"
#include "CTestCase_CreateSession.h"

//#include <xonlinep.h>
namespace MatchCoreTestNamespace {

class CParameter_CreateSession
{
	friend class CTestCase_CreateSession;
	
    DWORD m_dwPublicCurrent;
    DWORD m_dwPublicAvailable;
    DWORD m_dwPrivateCurrent;
    DWORD m_dwPrivateAvailable;
    DWORD m_dwNumAttributes;
    PXONLINE_ATTRIBUTE m_pAttributes;
    BOOL  m_bAttrAllocated;
    HANDLE m_hWorkEvent;
    BOOL	m_bEventCreated;
    XONLINETASK_HANDLE* m_phTask;
//    BOOL  m_bHTaskAllocated;
    XONLINETASK_HANDLE m_hTask;

public:
	CParameter_CreateSession();
	~CParameter_CreateSession();
};
}
#endif

