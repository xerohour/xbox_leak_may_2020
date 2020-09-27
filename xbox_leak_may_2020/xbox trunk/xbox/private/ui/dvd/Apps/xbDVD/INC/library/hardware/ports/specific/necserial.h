// FILE:			library\hardware\port\specific\necserial.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1999 VIONA Development GmbH & Co. KG.  All Rights Reserved.
// CREATED:		05.08.1999
//
// PURPOSE:		Serial Bidirectional byte port for NEC devices
//
// HISTORY:

#ifndef NECSERIAL_H
#define NECSERIAL_H

#include "library\lowlevel\hardwrio.h"
#include "library\common\krnlsync.h"

class NECSerialBidirectionalPort : public ByteIndexedInOutPort, public VDMutex
	{
	private:
		BitOutputPort	* strobe;
		BitOutputPort	* clk;
		BitInOutPort	* dio;

		Error ShiftByteOut(BYTE b);
		Error ShiftByteIn(BYTE __far & b);
	public:
		NECSerialBidirectionalPort(BitOutputPort * strobe, BitOutputPort * clk, BitInOutPort * dio);

		Error OutByte(BYTE data);		
		Error OutByte(int at, BYTE data); 
		Error OutBytes(int at, int inc, BYTE __far * data, int num);		
		Error OutByteSeqA(int at, int inc, int __far* data);

		Error InByte(BYTE __far &data);
		Error InByte(int at, BYTE __far &data);
		Error InBytes(BYTE __far * data, int num);
		Error InBytes(int at, int inc, BYTE __far * data, int num);		

	};


#endif


