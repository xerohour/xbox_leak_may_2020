
// FILE:      remotedefs.h
// AUTHOR:    D.Heidrich
// COPYRIGHT: (c) 1999 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   21.08.99
//
// PURPOSE:   Generic definitions for remote controls.
//
// HISTORY:

#ifndef REMDEFS_H
#define REMDEFS_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\common\winports.h"

class GenericRemote
	{
	protected:
		WinPort *winPort;

		int sendCounter, receiveCounter;

		DWORD currentMessage, previousMessage, failedMessage;
		DWORD currentButtonTime, failedTime;   // milliseconds
		DWORD releaseTime;
		DWORD releaseDelay;   // in ticks of 64us
		BOOL sendingRelease;

	public:
		GenericRemote (WinPort *winPort, DWORD releaseDelay);   // delay in microseconds
		virtual ~GenericRemote (void) {}

		virtual Error Initialize (void) = 0;

		virtual void SetWinPort (WinPort *newPort);

		virtual void Interrupt (void) = 0;

		virtual void ButtonPress (DWORD ceMsg);

		// This function must be called periodically to send the button events to the winPort.
		virtual void ProcessButtons (void);
	};

#endif
