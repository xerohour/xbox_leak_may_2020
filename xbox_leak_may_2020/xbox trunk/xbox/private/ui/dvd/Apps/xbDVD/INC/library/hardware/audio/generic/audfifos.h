// FILE:			library\hardware\audio\generic\audfifos.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		12.12.95
//
// PURPOSE:		Classes for custom audio FIFOs
//
// HISTORY:

#ifndef AUDFIFOS_H
#define AUDFIFOS_H

#include "library\common\gnerrors.h"

//
// Full size of FIFO. Change here, if size of FIFO on board changes.
//
#define DEFAULT_FIFO_SIZE 2048L		// Minimal FIFO size as default

//
// Errors for audio FIFO
//
// Transfer is already running though it should be halted.
#define GNR_FIFO_TRANSFER_RUNNING	MKERR(ERROR, AUDIO, GENERAL, 0x0101)
// Transfer is halted though it should be running
#define GNR_FIFO_TRANSFER_HALTED		MKERR(ERROR, AUDIO, GENERAL, 0x0102)
// FIFO is already full.
#define GNR_FIFO_FULL					MKERR(ERROR, AUDIO, GENERAL, 0x0103)
// FIFO is already empty.
#define GNR_FIFO_EMPTY					MKERR(ERROR, AUDIO, GENERAL, 0x0104)
// FIFO size could not be detected. Possibly there is none...
#define GNR_FIFO_SIZE_UNDETERMINED	MKERR(ERROR, AUDIO, GENERAL, 0x0105)

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\hardwrio.h"
#include "library\lowlevel\intrctrl.h"
#include "library\common\virtunit.h"

class AudioFIFOEventRequest {
	public:
		virtual void FIFOEvent(BOOL read, DWORD bytesAvailable) = 0;
	};

class AudioFIFO {
	private:
		ByteIndexedInOutPort		*	fifoPort;
	protected:
		BOOL	read;
		
		AudioFIFOEventRequest	*	request;
	public:
		AudioFIFO(ByteIndexedInOutPort * fifoPort) {this->fifoPort = fifoPort; request = NULL;}

		// Initialize a transfer. Install request callback class. 
		virtual Error InitTransfer(BOOL read, AudioFIFOEventRequest * request);	
		virtual Error StartTransfer(void) = 0;
		virtual Error HaltTransfer(void) = 0;
		virtual Error ResumeTransfer(void) = 0;
		virtual Error EndTransfer(void) = 0;

		virtual DWORD GetSize(void) = 0;								// Get the full size of the FIFO.

		virtual DWORD AvailBytes(void) = 0;

		virtual Error Fill(FPTR data, DWORD bytes) = 0;			// Fill the FIFO with num bytes.
		virtual Error Get(FPTR data, DWORD bytes) = 0;			// Get num bytes from the FIFO
	};


enum StatusType {FIFO_STATUS_FIELD_ERROR,
					  FIFO_STATUS_RUNNING = FIFO_STATUS_FIELD_ERROR,
					  FIFO_STATUS_EMPTY,
					  FIFO_STATUS_FULL,
					  FIFO_STATUS_HALFFULL
					 };

class H22AudioFIFO : public AudioFIFO {
	private:
		ByteIndexedInOutPort		*	fifoPort;
		BitInputPort				*	statusPort;
      InterruptServer 			*	fifoIntServer;
      InterruptHandler			*	fifoIntHandler;
	      
		BOOL	transferRunning;
		DWORD	availableBytes;
		DWORD	haltCount;
		BOOL	manualStart;

		DWORD	fifoSize;
		
		BOOL	read;
	protected:
		Error	GetStatus(StatusType type, BOOL __far & status);	// Get status of a certain type.
		Error WriteBytes(DWORD numBytes);
		Error DetermineSize(DWORD __far & size);
	public:
		H22AudioFIFO(ByteIndexedInOutPort	* fifoPort,
						 BitInputPort				* statusPort,
						 InterruptServer			* fifoIntServer);
						 
		~H22AudioFIFO();

		virtual Error Reset(void);

		virtual Error SetManualStart(BOOL manualStart);

		Error InitTransfer(BOOL read, AudioFIFOEventRequest * request);
		Error StartTransfer(void);
		Error HaltTransfer(void);
		Error ResumeTransfer(void);
		Error EndTransfer(void);

		DWORD	GetSize(void) {return fifoSize;}
		DWORD	AvailBytes(void);
		
		Error Fill(FPTR data, DWORD bytes);
		Error Get(FPTR data, DWORD bytes);

		void 	Interrupt(void);									// Interrupt handling routine
	}; 

#endif	

#endif	
