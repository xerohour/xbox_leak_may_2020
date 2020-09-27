// FILE:      library\hardware\mpeg2enc\specific\iVac.h
// AUTHOR:    Martin Stephan
// COPYRIGHT: (c) 1999 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   
//
// PURPOSE:   iCompression iVac Encoder Chip class  --- header file 
//
// HISTORY:

#ifndef IVAC_H
#define IVAC_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\common\vddebug.h"
#include "library\lowlevel\intrctrl.h"
#include "library\hardware\mpeg2enc\generic\mpeg2enc.h"

class DataBuffer
	{
	public:
		Error Init(HPTR data, DWORD size);

	private:
		HPTR  data;			// pointer to address that should be filled with incoming data 
		DWORD size;			// size of data area 
	};

// the data buffer queue class manages data buffers that are sent from the application (or higher-level drivers)
//	to our driver. A data buffer itself (see the declaration above) exists of a pointer to the data and the size
//	of the buffer. 
class DataBufferQueue
	{
	public:
		DataBufferQueue(WORD size);
		~DataBufferQueue(void);

		Error Reset(void);
		Error InsertDataBuffer(HPTR data, DWORD size);
		Error GetDataBuffer(DataBuffer &buffer);
		WORD DataBufferAvailable() {return (first <= last) ? (last - first) : (sizeOfQueue + last - first);}

	private:
		DataBuffer * dataBuffer;
		int sizeOfQueue;		// size of data buffer queue
		int num;					// current number of data buffers in queue. Only valid when initialised with 1 buffer!!!
		int first, last;		// first and last data buffer
	};

//
// iVac encoder chip class
// 
// inherits from MPEG2AVEncoder, InterruptHandler, and VDSpinLock
// To synchronize the access to the device's registers you can use the VDSpinLock class. 
// This is mostly the case when the Interrupt Service Routine is doing a lot of access to the 
// device's registers. 
//	For example if you want to read or write to a register just call EnterLock() before and 
// LeaveLock() after the access:
// Function OutDWord()
//		{
//		EnterLock()
//		...
//		register access 
//		...
//		LeaveLock()
//		}
//
class IVACEncoder	: public MPEG2AVEncoder, protected InterruptHandler, protected VDSpinLock
	{
	friend class VirtualIVACEncoder;
	friend class IVACPCI;

	public:
		//IVACEncoder(DWordIndexedInOutPort * port);
		//IVACEncoder(MemoryMappedIO * mem, MemoryMappedIO * reg);
		IVACEncoder(InterruptServer * irqServer);
		~IVACEncoder();

		Error Initialize(void);
		void Interrupt(void);
		MPEGEncoderState CurrentState(void);

	protected:
		VirtualMPEG2AVEncoder * CreateVirtualMPEGEncoder(void);		

		DataBufferQueue				dataBuffers;
		DWordIndexedInOutPort	*	port;

		Error OutDWord(int idx, DWORD val);                             
		Error InDWord(int idx, DWORD __far&val);

		Error StartCapture(void);
		Error StopCapture(void);
		virtual Error PutBuffer(HPTR data, DWORD size);
		Error	AudioInterrupt(void);
		Error VideoInterrupt(void);
		Error SetOutputType(MPEGEncoderOutputType outputType);

	private:
		InterruptServer			*	irqServer;
		MPEGEncoderState			currentState;

		Error DownloadFirmware(void);

   };

//
// virtual iVac encoder chip class
//
class VirtualIVACEncoder : public VirtualMPEG2AVEncoder
	{
	public:
		VirtualIVACEncoder(IVACEncoder * encoder);

		Error Configure(TAG __far * tags);

	protected:

	private:
		IVACEncoder * encoder;

	};


#endif // IVAC_H
