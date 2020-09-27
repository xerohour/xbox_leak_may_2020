// FILE:			library\hardware\audio\specific\q22fifo.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1997 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		18.03.97
//
// PURPOSE:		Class for Q22 audio FIFO
//
// HISTORY:

#ifndef Q22FIFO_H
#define Q22FIFO_H

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\hardwrio.h"
#include "library\lowlevel\intrctrl.h"
#include "library\common\krnlsync.h"
#include "library\common\profiles.h"
#include	"library\hardware\audio\generic\audtypes.h"

#include "..\generic\bufstrms.h"

class Q22AudioFIFO : public StreamFIFO, protected VDCriticalSection
	{ 
	friend class Q22FIFOIntHandler;
	private:
		ByteIndexedInOutPort		*	fifoPort;
      InterruptServer 			*	fifoIntServer;
      InterruptHandler			*	fifoIntHandler;
      
		BitIndexedByteSplitOutputPort	*	controlPort;
		
		long bufferSize;
		
		long	threshold;
		long	minIRQDistance;
		
		long	inCount;
		long	dummyCount;
		long irqCount;
		long finalCount;

		long	eventPosition;
		
		DWORD	state;
		
		BOOL	encode;
		
		Profile	* profile;
		
		DWORD		baseMaxIRQLatency;
		DWORD		baseMinIRQDistance;
		
		BOOL		dmaEnable;
		BOOL		inIRQ;
		BOOL		bufferNotEmpty;

		BYTE		dummyData[32];
		
		BYTE		peakLeft, peakRight;
		WORD		oldPeakLeft, oldPeakRight;
		DWORD		offsetLeft, offsetRight;
		DWORD		realCount;
		
		AudioDataFormat	format;
		AudioMode			mode;
		
		BOOL		peakMeasure;
		
		DWORD		initialSkip;
		DWORD		skipCount;
	protected:
		Error	CheckThreshold(void);

		Error FillDummyBytes(void);
		Error ReadDummyBytes(void);
		
		Error ReadInternalInputCounter(long __far & done);
		Error ReadInternalOutputCounter(long __far & done);
		
		void CriticalSection(void);

		Error CalculatePeaks(HBPTR buff, DWORD curCount, DWORD done);
	public:
		Q22AudioFIFO(ByteIndexedInOutPort				* fifoPort,
						 BitIndexedByteSplitOutputPort	* controlPort,
						 InterruptServer						* fifoIntServer,
						 DWORD bufferSize,
						 Profile 								* profile);
						 
		~Q22AudioFIFO();
	
		Error Flush(void);
		
		Error BeginTransfer(StreamFIFORequest * req,
							 	  BOOL  encode,
							 	  DWORD bytesPerSecond,
							 	  BYTE  dummyFill,
							 	  DWORD format,
							 	  DWORD mode,
							 	  DWORD initialSkip);
		Error CompleteTransfer(void);
		Error StartTransfer(void);
		Error StopTransfer(void);
		Error	EndTransfer(void);

		Error WriteBytes(HBPTR buff, DWORD size, DWORD __far & done);		
		Error ReadBytes(HBPTR buff, DWORD size, DWORD __far & done);

		Error ReadInputCounter(DWORD __far & done);
		Error ReadOutputCounter(DWORD __far & done);

		DWORD BytesFull(void);
		DWORD BytesFree(void);
		
		Error ScheduleEvent(StreamFIFOEvent * event, DWORD position);
	
		Error SuspendCallbacks(void);
		Error ResumeCallbacks(void);

		Error GetDataPeak(DWORD __far & info, BOOL encode);
	};

#endif // of ONLY_EXTERNAL_VISIBLE

#endif
