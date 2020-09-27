/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

	dbgosdp.h

Abstract:

	Private header file for dbgosd.h

Author:

	Matthew D Hendel

History:

	math 14-Aug-95	Created.
	
--*/


#ifndef _DBGOSDP_H_
#define _DBGOSDP_H_


// this is a private header file for the file dbgosd.cpp


class CDbgEvent
{

  public:

	CDbgEvent ()
	{
		VERIFY (m_h = CreateEvent (NULL, TRUE, FALSE, NULL));
	}
	   ~CDbgEvent ()
	{
		VERIFY (CloseHandle (m_h));
	}
	void Set ()
	{
		VERIFY (SetEvent (m_h));
	}
	void Reset ()
	{
		VERIFY (ResetEvent (m_h));
	}

	HANDLE m_h;
};


// generic macro which returns number of elements in an array
#define CELEM_ARRAY(a) (sizeof(a) / sizeof(a[0]))


// Exception Stuff: an EXSTUFF* is passed in dwParam to DoCallBack
struct EXSTUFF
{
	BOOL fFirstChance;			// This is a first-chance exception.  (On
	// platforms that don't have the concept of
	// first- and last-chance exceptions, this
	// flag will always be false.)

	BOOL fContinuing;			// OSDebug is continuing to run the debuggee
	// despite receiving the exception.  For
	// last-chance exceptions, this will never
	// be set.

	ADDR addrPC;				// The PC of the thread when the exception
	// occurred.

};


#endif // _DBGOSDP_H_
