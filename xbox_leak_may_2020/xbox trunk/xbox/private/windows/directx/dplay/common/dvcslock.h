/*==========================================================================
 * Copyright (C) 1999, 2000 Microsoft Corporation.  All Rights Reserved.
 *
 * File:       dvcslock.h
 * Content:    Class to handle auto-leave of critical sections
 * History:
 *   Date		By		Reason
 *   ====		==		======
 *	07/05/00	rodtoll	Created It
 *
 ***************************************************************************/
#ifndef __DVCSLOCK_H
#define __DVCSLOCK_H

// CDVCSLock
//
// A class to provide automatic unlocking of critical sections when the object
// passes out of scope.
//
class CDVCSLock
{
	STDNEWDELETE

public:
	CDVCSLock( CRITICAL_SECTION *pcs ): m_pcs( pcs ), m_fLocked( FALSE )
	{
	};

	~CDVCSLock() 
	{ 
		if( m_fLocked ) LeaveCriticalSection( m_pcs ); 
	}

	void Lock()
	{
		EnterCriticalSection( m_pcs );
		m_fLocked = TRUE;
	}

	void Unlock()
	{
		LeaveCriticalSection( m_pcs );
		m_fLocked = FALSE;
	}

private:

	CRITICAL_SECTION *m_pcs;
	BOOL m_fLocked;
};

#endif