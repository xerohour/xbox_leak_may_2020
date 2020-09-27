// FILE:			library\hardware\audio\generic\bufstrms.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1997 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		10.03.97
//
// PURPOSE:		Buffer based streams classes
//
// HISTORY:

#ifndef BUFSTRMS_H
#define BUFSTRMS_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#define NUM_STREAM_BUFFERS		32


///////////////////////////////////////////////////////////////////////////////
// StreamFIFO Base Class and related classes
///////////////////////////////////////////////////////////////////////////////


//
// Event Class called from StreamFIFO when a certain FIFO output position is reached
//
class StreamFIFOEvent
	{
	public:
		virtual void FIFOEventReached(DWORD position) = 0;
	};

//
// Request Class called from StreamFIFO when refilling/emptying of data necessary
//
class StreamFIFORequest
	{
	public:
		virtual void FIFORequest(void) = 0;
	};


//
// Base Class for FIFOs to accomplish a continuous stream of data
//
class StreamFIFO
	{ 
	protected:
		StreamFIFORequest			*	req;
		StreamFIFOEvent			*	event;
		
	public:
		virtual Error Flush(void) = 0;
		
		virtual Error BeginTransfer(StreamFIFORequest * req,
											 BOOL  encode,
											 DWORD bytesPerSecond,
											 BYTE  dummyFill,
											 DWORD format,
											 DWORD mode,
											 DWORD initialSkip) = 0;
		virtual Error CompleteTransfer(void) = 0;

		virtual Error StartTransfer(void) = 0;
		virtual Error StopTransfer(void) = 0;
		virtual Error EndTransfer(void) = 0;

		virtual Error WriteBytes(HBPTR buff, DWORD size, DWORD __far & done) = 0;
		virtual Error ReadBytes(HBPTR buff, DWORD size, DWORD __far & done) = 0;
		
		virtual Error ReadInputCounter(DWORD __far & done) = 0;
		virtual Error ReadOutputCounter(DWORD __far & done) = 0;

		virtual DWORD BytesFull(void) = 0;
		virtual DWORD BytesFree(void) = 0;
		
		virtual Error ScheduleEvent(StreamFIFOEvent * event, DWORD position) = 0;		
		
		virtual Error SuspendCallbacks(void) = 0;
		virtual Error ResumeCallbacks(void) = 0;
		
		virtual Error GetDataPeak(DWORD __far & info, BOOL encode) {info = 0L; GNRAISE_OK;}
	};



///////////////////////////////////////////////////////////////////////////////
// BufferStream Base Class and related classes
///////////////////////////////////////////////////////////////////////////////


//
// Request called from a BufferStream when processing of a Buffer is completed
//
class BufferStreamCompletionRequest
	{
	public:
		virtual void BufferCompletionRequest(HBPTR buff, DWORD size, DWORD bufferID, DWORD done) = 0;
	};


//
// Realizes FIFO based streams whose data is delivered/retrieved in chunks (buffers)
//
class BufferStream : protected StreamFIFOEvent, protected StreamFIFORequest
	{
   protected:
		StreamFIFO	*	fifo;

   	BufferStreamCompletionRequest	*	req;

		struct
			{
			HBPTR		data;
			DWORD		size;
			DWORD		bufferID;
			} buffers[NUM_STREAM_BUFFERS];

		WORD	firstBuffer, lastBuffer;

		DWORD	firstPosition;
		DWORD	basePosition;

		DWORD	currentDone;
		
		DWORD startThreshold;
		
		virtual void FIFOEventReached(DWORD position) = 0;
		virtual void FIFORequest(void) = 0;
		
	public:
   	BufferStream(StreamFIFO * fifo);

		Error SetCompletionRequest(BufferStreamCompletionRequest * req) {this->req = req; GNRAISE_OK;}		

		virtual Error BeginTransfer(DWORD bytesPerSecond) = 0;
		virtual Error StartTransfer(void) = 0;
		virtual Error CompleteTransfer(void) = 0;
		virtual Error StopTransfer(void) = 0;
		virtual Error EndTransfer(void) = 0;

		virtual Error GetPosition(DWORD __far * position) = 0;
		virtual Error SetPosition(DWORD position) = 0;

		virtual Error AddBuffer(HBPTR data, DWORD size, DWORD bufferID) = 0;
		virtual Error FlushBuffers(void) = 0;
		
		virtual Error GetDataPeak(DWORD __far & info) = 0;
	};



///////////////////////////////////////////////////////////////////////////////
// BufferOutputStream (for Playback)
///////////////////////////////////////////////////////////////////////////////

class BufferOutputStream : public BufferStream
	{
	private:
		DWORD state;
		int	suspendCount;
 		WORD	currentBuffer;
 	protected:
	   
		void FIFOEventReached(DWORD position);
		void FIFORequest(void);
	public:
		BufferOutputStream(StreamFIFO * fifo);

		virtual Error BeginTransfer(DWORD bytesPerSecond, BYTE dummyFill, DWORD format, DWORD mode);
		Error BeginTransfer(DWORD bytesPerSecond) {GNRAISE(BeginTransfer(bytesPerSecond, 0, 0L, 0L));}
		Error CompleteTransfer(void);
		Error StartTransfer(void);
		Error StopTransfer(void);
		Error EndTransfer(void);

		Error GetPosition(DWORD __far * position);
		Error SetPosition(DWORD position);

		Error AddBuffer(HBPTR data, DWORD size, DWORD bufferID);
		Error FlushBuffers(void);

		Error GetDataPeak(DWORD __far & info) {GNRAISE(fifo->GetDataPeak(info, FALSE));}
	};


///////////////////////////////////////////////////////////////////////////////
// BufferInputStream (for Capture)
///////////////////////////////////////////////////////////////////////////////

class BufferInputStream : public BufferStream
	{
	private:
		DWORD state;
		int	suspendCount;

		DWORD	initialSkip;	// Number of bytes that are skipped in the beginning
	protected:
		void FIFOEventReached(DWORD position);
		void FIFORequest(void);
	public:
		BufferInputStream(StreamFIFO * fifo);

		Error BeginTransfer(DWORD bytesPerSecond)
			{GNRAISE(BeginTransfer(bytesPerSecond, 0L, 0L, 0L));}

		virtual Error BeginTransfer(DWORD bytesPerSecond, DWORD format, DWORD mode, DWORD initialSkip = 0L);

		Error CompleteTransfer(void);
		Error StartTransfer(void);
		Error StopTransfer(void);
		Error EndTransfer(void);

		Error GetPosition(DWORD __far * position);
		Error SetPosition(DWORD position);

		Error AddBuffer(HBPTR data, DWORD size, DWORD bufferID);
		Error FlushBuffers(void);
	
		Error GetDataPeak(DWORD __far & info) {GNRAISE(fifo->GetDataPeak(info, TRUE));}
	};


#endif

#endif

