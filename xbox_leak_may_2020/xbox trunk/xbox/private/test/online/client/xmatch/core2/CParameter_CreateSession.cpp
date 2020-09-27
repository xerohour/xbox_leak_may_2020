#include "CParameter_CreateSession.h"

using namespace MatchCoreTestNamespace;

namespace MatchCoreTestNamespace {

// CParameter_CreateSession member functions
CParameter_CreateSession::CParameter_CreateSession()
	: m_bAttrAllocated(FALSE), m_bEventCreated(FALSE)
{
}

CParameter_CreateSession::~CParameter_CreateSession()
{
	if(m_bEventCreated)
		CloseHandle(m_hWorkEvent);
	if(m_bAttrAllocated)
		delete[] m_pAttributes;
}


}



