
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "library\common\virtunit.h"
#include "library\common\tags.h"
#include "library\common\tagunits.h"
#include "library\common\krnlsync.h"

#define COPR_RS232			MKFLAG(0)
#define COPR_I2C				MKFLAG(1)

MKTAG(COMM_PROTOCOL_SUPPORTED,		COMMUNICATION_UNIT, 0x0001, DWORD)
MKTAG(COMM_PROTOCOL,						COMMUNICATION_UNIT, 0x0002, DWORD)
MKTAG(COMM_BAUD_RATE,					COMMUNICATION_UNIT, 0x0003, DWORD)
MKTAG(COMM_PORT_AFFINITY,				COMMUNICATION_UNIT, 0x0004, DWORD)
MKTAG(COMM_INPUT_BUFFER_SIZE,			COMMUNICATION_UNIT, 0x0005, DWORD)
MKTAG(COMM_OUTPUT_BUFFER_SIZE,		COMMUNICATION_UNIT, 0x0006, DWORD)

#define COUN_INFINITE		0xffffffff

class CommunicationBuffer
	{
	protected:
		BYTE				*	buffer;
		DWORD					bufferSize, bufferMask, bufferStart, bufferStop;

		DWORD					waitForDataSize, waitForSpaceSize;
		BOOL					waitForDataAbort, waitForSpaceAbort;
		VDTimedSemaphore	waitForDataEvent, waitForSpaceEvent;

	public:
		CommunicationBuffer(void);
		~CommunicationBuffer(void);

		Error AllocateBuffer(DWORD size);
		Error FlushBuffer(void);

		DWORD AvailData(void);
		DWORD AvailSpace(void);

		Error WriteByte(BYTE b);
		Error ReadByte(BYTE & b);
		Error WriteBytes(BYTE * p, DWORD num);
		Error ReadBytes(BYTE * p, DWORD num);

		Error WaitForData(DWORD num, DWORD timeout);
		Error WaitForSpace(DWORD num, DWORD timeout);

		Error AbortWaitForData(void);
		Error AbortWaitForSpace(void);
	};

class CommunicationStreamInterface
	{
	public:
		virtual DWORD AvailableInputData(void) = 0;
		virtual Error ReadData(BYTE * buffer, DWORD num, DWORD & done) = 0;
		virtual Error WaitForInputData(DWORD num, DWORD timeout) = 0;
		virtual Error AbortWaitForInputData(void) = 0;

		virtual DWORD AvailableOutputBuffer(void) = 0;
		virtual Error WriteData(BYTE * buffer, DWORD num, DWORD & done) = 0;
		virtual Error WaitForOutputBuffer(DWORD num, DWORD timeout) = 0;
		virtual Error AbortWaitForOutputBuffer(void) = 0;

		virtual Error InitializeConnection(void) = 0;
		virtual Error AbortConnection(void) = 0;

		virtual Error SetBufferSize(DWORD input, DWORD output) = 0;
	};

class CommunicationPort : public PhysicalUnit, virtual public CommunicationStreamInterface
	{
	friend class VirtualCommunicationUnit;
	public:
		CommunicationPort(void);
		~CommunicationPort(void);

		virtual Error SetProtocol(DWORD protocol) = 0;
		virtual Error SetBaudRate(DWORD rate) = 0;
	};

class BufferedCommunicationStream : virtual public CommunicationStreamInterface
	{
	protected:
		CommunicationBuffer	inputBuffer, outputBuffer;
		BOOL						abortConnection;

		virtual void NotifyOutputDataAvailable(void) = 0;
		virtual void NotifyInputSpaceAvailable(void) = 0;

	public:
		BufferedCommunicationStream(void);
		virtual ~BufferedCommunicationStream(void);

		// Functions of CommunicationStreamInterface.
		DWORD AvailableInputData(void);
		Error ReadData(BYTE * buffer, DWORD num, DWORD & done);
		Error WaitForInputData(DWORD num, DWORD timeout);
		Error AbortWaitForInputData(void);

		DWORD AvailableOutputBuffer(void);
		Error WriteData(BYTE * buffer, DWORD num, DWORD & done);
		Error WaitForOutputBuffer(DWORD num, DWORD timeout);
		Error AbortWaitForOutputBuffer(void);

		Error InitializeConnection(void);
		Error AbortConnection(void);

		Error SetBufferSize(DWORD input, DWORD output);
	};

class BufferedByteCommunicationStream : public BufferedCommunicationStream
	{
	protected:
		Error GetOutputByte(BYTE & b) {return outputBuffer.ReadByte(b);}
		BOOL IsOutputDataAvailable(void) {return outputBuffer.AvailData() >= 1;}
		Error PutInputByte(BYTE b) {return inputBuffer.WriteByte(b);}
		BOOL IsInputSpaceAvailable(void) {return inputBuffer.AvailSpace() >= 1;}
	};


class VirtualCommunicationPort : public VirtualUnit
	{
	protected:
		virtual DWORD ProtocolsSupported(void) = 0;

		CommunicationPort * physical;

		DWORD protocol;
		DWORD baudRate;
		DWORD affinity;
		DWORD inputBufferSize, outputBufferSize;

		Error PreemptStopPrevious(VirtualUnit * previous);
		Error PreemptChange(VirtualUnit * previous);
		Error PreemptStartNew(VirtualUnit * previous);

	public:
		VirtualCommunicationPort(CommunicationPort * physical);
		virtual ~VirtualCommunicationPort(void);

		Error Configure (TAG __far *tags);

		DWORD AvailableInputData(void);
		Error ReadData(BYTE * buffer, DWORD num, DWORD & done);
		Error WaitForInputData(DWORD num, DWORD timeout);
		Error AbortWaitForInputData(void);

		DWORD AvailableOutputBuffer(void);
		Error WriteData(BYTE * buffer, DWORD num, DWORD & done);
		Error WaitForOutputBuffer(DWORD num, DWORD timeout);
		Error AbortWaitForOutputBuffer(void);
	};

#endif
