
// FILE:      vesteldvd.h
// AUTHOR:    H.Horak
// COPYRIGHT: (c) 1998 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   11.09.98
//
// PURPOSE:   SW decoder for Vestel DVD IR remote control.
//
// HISTORY:

#ifndef VESTELDVD_H
#define VESTELDVD_H

#include "library\common\gnerrors.h"
#include "library\common\winports.h"
#include "library\lowlevel\timer.h"
#include "library\hardware\remote\generic\remotedefs.h"

class VestelRemote : public GenericRemote
	{
	private:
		clock_t lastTime, errorTime;
		int bitCount;
		BOOL bit, decode, check;
		DWORD data, group, code;

		// States of decoding state machine.
		enum RemoteState
			{
			ERROR,
			IDLE,
			HIGH_HEADER,
			LOW_HEADER,
			STARTUP,
			DECODE,
			RESYNC
			} state;

	public:
		VestelRemote (WinPort *winPort);
		virtual ~VestelRemote (void);

		virtual Error Initialize (void);

		virtual void Interrupt (void);
	};

#endif
