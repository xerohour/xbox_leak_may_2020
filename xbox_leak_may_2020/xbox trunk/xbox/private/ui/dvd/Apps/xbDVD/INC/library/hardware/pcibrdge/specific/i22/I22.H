
// FILE:      library\hardware\pcibridge\specific\i22\i22.h
// AUTHOR:    Ulrich Sigmund, Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   15.05.95
//
// PURPOSE: The I22 chip class.
//
// HISTORY:

#ifndef I22_H
#define I22_H

#include "..\..\..\..\common\prelude.h"
#include "..\..\..\..\common\gnerrors.h"
#include "..\..\..\..\lowlevel\memmpdio.h"
#include "..\..\..\..\lowlevel\i2c.h"
#include "..\i20\i20.h"

#include "i22regs.h"



class I22 : public I20
	{
	public:
		I22 (MemoryMappedIO *mem);
		virtual ~I22 (void);

		InterruptServer *jpegCodeIRQ;

		virtual Error BuildPorts (void);
		virtual Error ConfigureGuest (int no, int dur, int rec);

		virtual void Interrupt (void);
	};



#endif
