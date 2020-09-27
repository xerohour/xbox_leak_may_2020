
// FILE:      panadvd.h
// AUTHOR:    H.Horak
// COPYRIGHT: (c) 1998 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   11.09.98
//
// PURPOSE:   SW decoder for Panasonic DVD IR remote control.
//
// HISTORY:

#ifndef REMPANAS_H
#define REMPANAS_H

#include "library\common\gnerrors.h"
#include "library\common\winports.h"
#include "library\lowlevel\timer.h"
#include "library\hardware\remote\generic\remotedefs.h"

class PanasonicRemote : public GenericRemote
	{
	private:
		clock_t lastTime, errorTime;
		DWORD bitCount;
		DWORD group;
		WORD code;

		enum {
			IDLE,
			HIGH_HEADER,
			LOW_HEADER,
			CLOCK,
			DECODE,
			ERROR
			} state;

	public:
		PanasonicRemote (WinPort *winPort);
		virtual ~PanasonicRemote (void);

		virtual Error Initialize (void);

		virtual void Interrupt (void);
	};

#endif
