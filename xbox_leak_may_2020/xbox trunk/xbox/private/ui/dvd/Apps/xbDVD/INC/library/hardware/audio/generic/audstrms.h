// FILE:			library\hardware\audio\generic\audstrms.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.11.95
//
// PURPOSE:		Classes for audio streams going over this FIFO
//
// HISTORY:

#ifndef AUDSTRMS_H
#define AUDSTRMS_H

#include "library\general\asncstrm.h"
#include "audfifos.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#define GNR_AUDSTRM_TRANSFER_HALTED		MKERR(ERROR, AUDIO, GENERAL, 0x0301)

/////////////////////////////////////////////////////////////////////////////////
// Asynchronous FIFO audio output stream
/////////////////////////////////////////////////////////////////////////////////

class FIFOOutStream : public ASyncOutStream, protected AudioFIFOEventRequest {
	private:
		AudioFIFO				*	fifo;
		
		DWORD position;
		
		BOOL	transferRunning;
		BOOL	transferStarted;
	public:
		FIFOOutStream(AudioFIFO * fifo); 
		~FIFOOutStream(void) {};

		Error WriteData(HPTR data, DWORD size, DWORD __far &done);

		DWORD AvailSpace(void);
		DWORD AvailData(void);

		DWORD GetTransferLocation(void);
		void  SetTransferLocation(DWORD pos);

		Error RecoverData(HPTR buffer, DWORD size, DWORD __far &done);

		Error StartTransfer();
		Error StopTransfer(void);

		Error SuspendTransfer(void);
		Error ResumeTransfer(void);

		Error SyncTransfer(void);
		Error CompleteTransfer(void);
      
      Error FlushBuffer(void);		
		
		void FIFOEvent(BOOL read, DWORD bytesAvailable);
	};

/////////////////////////////////////////////////////////////////////////////////
// Asynchronous FIFO audio input stream
/////////////////////////////////////////////////////////////////////////////////

class FIFOInStream : public ASyncInStream, protected AudioFIFOEventRequest {
	private:
		AudioFIFO	*	fifo;

		DWORD position;
		
		BOOL	transferRunning;
		BOOL	transferStarted;
	public:
		FIFOInStream(AudioFIFO * fifo);
		~FIFOInStream(void) {};

		Error ReadData(HPTR data, DWORD size, DWORD __far &done);

		DWORD AvailSpace(void);
		DWORD AvailData(void);

		DWORD GetTransferLocation(void);
		void	SetTransferLocation(DWORD pos);
		
		Error StartTransfer(void);
		Error StopTransfer(void);

		Error SuspendTransfer(void);
		Error ResumeTransfer(void);

		Error SyncTransfer(void);

		Error FlushBuffer(void);

		void FIFOEvent(BOOL read, DWORD bytesAvailable);
	};

#endif // ONLY_EXTERNAL_VISIBLE

#endif
	