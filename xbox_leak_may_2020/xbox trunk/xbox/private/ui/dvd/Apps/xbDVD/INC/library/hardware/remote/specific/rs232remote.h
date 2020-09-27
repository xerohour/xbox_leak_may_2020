
// FILE:      rs232remote.h
// AUTHOR:    B. Pruden
// COPYRIGHT: (c) 1999 Ravisent Technologies.  All Rights Reserved.
// CREATED:   10.28.99
//
// PURPOSE:   Receive remote control commands via RS232 serial port.
//
// HISTORY:

#ifndef RS232REMOTE_H
#define RS232REMOTE_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\common\winports.h"

#include "driver\dvdpldrv\win32\dvdpld32.h"

class Rs232Remote
	{
	private:
		WinPort *winPort;
 		UnitSet commUnit;		// handle to the virtual cinemaster units
		task_t* commRemoteThread;
		DWORD code;

  public:
		Rs232Remote (WinPort *winPort, UnitSet commUnit);
		~Rs232Remote (void);

		Error Initialize (void);

		void ThreadEntry (void);
	};

#endif
