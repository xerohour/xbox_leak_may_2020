
// FILE:      library\hardware\mpegdec\specific\zr361x0.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   10.04.95
//
// PURPOSE:   Combined parent class for ZR36100 (I11) and ZR36110 (I16)
//
// HISTORY:

#ifndef ZR361X0_H
#define ZR361X0_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "..\generic\mpegdec.h"        
#include "library\lowlevel\hardwrio.h"
#include "library\lowlevel\intrctrl.h"
#include "library\general\asncstrm.h"
#include "library\common\krnlstr.h"

#define	GNR_ZR361X0_TIMEOUT			MKERR(ERROR, MPEG, TIMEOUT, 0x01)
	// Timeout during parameter transfer

#define	GNR_ZR361X0_INITIAL_IDLE	MKERR(ERROR, MPEG, BUSY, 0x02)
	// Chip not correctly initialized

class ZR361X0VBlankIntHandler;
class ZR361X0IdleIntHandler;

	//
	// Polled IO data transfer class.
	//
class ZR361X0PortOutStream : public ASyncOutStream {
	protected:
		ByteIndexedInOutPort	*	bio;					// data ports
		WordIndexedInOutPort	*	wio;                    
		WORD				   		transferSize;		// size of one transfer burst
		BOOL							transferRunning;	// transfer is running
		//
		// Circular buffer handling:
		//            _____________________AvailData();
		//           /                    /
		//   .......######################............
		//          |                    |
		//           \startValid          \endValid
		//
		// or
		//
		//   #########......................##########
		//           |                      |
		//            \endValid              \startValid
		//   
		volatile DWORD				startValid;			// start of circular buffer
		DWORD							endValid;       	// end of circular buffer
		DWORD							buffSize;   		// size of the circular buffer
		DWORD							buffStart;			// start of buffer in stream
		
		HBPTR							buffer;				// pointer to buffer

		virtual DWORD TransferBlock(FPTR buff, DWORD num) = 0;
		DWORD TransferHBlock(HBPTR buff, DWORD num);
	public:
		ZR361X0PortOutStream(ByteIndexedInOutPort * bio,
		                     WordIndexedInOutPort * wio,
		                     WORD                   transferSize,
		                     DWORD						  buffSize);
		
		//
		// Transfer data in interrupt
		//
		void IntTransfer(DWORD maxTransfer);

		//
		// Implementation of ASyncOutStream
		//				
		Error WriteData(HPTR data, DWORD size, DWORD __far &done);
		DWORD AvailSpace(void);
		DWORD AvailData(void);
		DWORD GetTransferLocation(void);
		void SetTransferLocation(DWORD pos);
		Error RecoverData(HPTR buffer, DWORD size, DWORD __far &done);
				
		Error StartTransfer(void);
		Error StopTransfer(void);       
		Error SyncTransfer(void);
		Error CompleteTransfer(void);
		Error FlushBuffer(void);		
	};

	//
	// Base definition of a MPEG audio decoder
	//	
class MPEGAudioDecoder {
	public:
		virtual Error MuteAudio(BOOL mute) = 0;
		virtual Error SetLeftVolume(WORD volume) = 0; // 0..10000
		virtual Error SetRightVolume(WORD volume) = 0; // 0..10000
	};

	//
	// Base class for ZR36100 and ZR36110 MPEG decoders
	//	
class ZR361X0 : public MPEGDecoder, protected ASyncRefillRequest {
	friend class VirtualZR361X0;
	friend class ZR361X0VBlankIntHandler;
	friend class ZR361X0IdleIntHandler;
	protected:                       
		BitOutputPort				*	resetPin;
		BitOutputPort				*	mb4;
		ByteIndexedInOutPort    *	bio;
		WordIndexedInOutPort		*	wio;
		ASyncOutStream				*	strm;                          
		MPEGAudioDecoder			*	audio;
		ZR361X0PortOutStream		*	pstrm;   //	Port dma out stream
						
		ZR361X0IdleIntHandler 	*	idleIntHandler;	// handler for IDLE interrupt
		ZR361X0VBlankIntHandler *	vblankIntHandler;	// handler for VBLANK interrupt	

		DWORD								intCount;			// interrupt counter
		DWORD								lastInnerInt;		// last interrupt during transfer
		
      InterruptServer		  	*	vblankIntServer;  // VBLANK interrupt server
		
		struct FullCommand {						// Command queue
			MPEGCommand	com;
			DWORD			data;
			} comBuffer[16];
		volatile DWORD			lastTag;			// last inserted command
		volatile DWORD			nextTag;			// first inserted command
		volatile DWORD			doneTag;			// last executed command
				
		volatile enum ExtStates {xreset,				// 0
		                         xinitial1,       // 1
		                         xinitial2,       // 2
		                         xinitial3,       // 3
					                xfrozen,         // 4
					                xplaying,        // 5
					                xsuspended,      // 6
					                xstepping,       // 7
					                xseekpending,    // 8
					                xseeking,        // 9
					                xresyncing,      // 10
					                xresyncing1,     // 11
					                xresyncing2,     // 12
					                xresyncing3,     // 13
					                xstopfreeze,     // 14
					                xstopped,        // 15
					                xrestarting,     // 16
					                xrestarting1,    // 17
					                xleaping,        // 18
					                xslowleaping,    // 19
					                xseekfreeze,		// 20
					                xresetfreeze,		// 21
					                xcued,           // 22
					                xresetpending,	// 23
					                xfastleaping,		// 24
					                xfastfreezing,	// 25
					                xkeystepping,		// 26
					                xillegal} extState; // 27

		WORD						stateDelay;			// delay counter for state transition
		DWORD						stateRepeat;		// number of repeate for current state
		WORD						lastPlayCommand; 	// last play command, for speed changes
		DWORD						statePosition;		// file position at last state

		DWORD						maxTransfer;		// maximum number of bytes to transfer per interrupt
		
		KernelString			mCodePath;			// path to the microcode directories
		
		BOOL						wordHost;         // flag whether this is a word host
		BOOL						mixedIO;				// mixedIO or DMA only
		WORD						transferSize;		// number of bytes per port stroke
		MPEGStreamType			streamType;			// type of current MPEG stream
		BOOL						pulldown;			// 3/2 pulldown (speed bug fix)
		
		DWORD						signalPosition;	// position of next signal

		DWORD						watchPosition;		
		DWORD						watchCount;
		
		//
		// Tests whether commands are pending
		//
		BOOL CommandsPending(void) {return nextTag != lastTag;}
		//
		// Get the next command from the queue
		//
		BOOL NextCommand(MPEGCommand __far &com, DWORD __far &param);
		//
		// Complete command execution
		//
		void DoneCommand(void);
		//
		// Check the next command
		//
		MPEGCommand PeekCommand(void);

		//
		// Wait for ready bit set
		//
		virtual Error WaitForReady(void) = 0;
		//
		// Send data to the chip
		//
		Error SendChipData(FPTR data, WORD num);
		//
		// Send a command to the chip
		//
		virtual Error SendChipCommand(WORD command) = 0;
		
		//
		// Send a microcode
		//		
		Error SendMCode(KernelString name, BYTE reg, WORD len);
		//
		// Initialize the chip, microcodes/parameters
		//
		virtual Error InitChip(MPEGParams __far &params) = 0;
		//
		// Put the chip into resetstate
		//
		Error ResetChip(void);
		
		//
		// VBlank ISR
		//
		virtual void VBlankInterrupt(void) = 0;
		//
		// Idle ISR
		//
		void IdleInterrupt(void);                        
		
		BOOL			forwardRequest;

		//
		// Forward the refill request
		//
		void RefillRequest(DWORD free);
		//
		// Send data to the buffer
		//
		DWORD SendData(HPTR data, DWORD size);
		//
		// Recover data from the buffer
		//
		DWORD RecoverData(HPTR data, DWORD size); 
		//
		// Send a fake restart header to the buffer
		//
		virtual DWORD SendFakeHeader(MPEGParams __far &params) = 0;
      
      //
      // Set audio parameters
      //
		Error SetLeftVolume(WORD volume);
		Error SetRightVolume(WORD volume);
		Error MuteAudio(BOOL mute);       
		
		Error SetVideoStreamID(BYTE id);
		Error SetAudioStreamID(BYTE id);

		DWORD prevPos;
		DWORD	signalPos;
		
   	//
   	// Set a signal
   	//
   	Error	SetSignal(DWORD position);   	

		VirtualUnit	*	videoBus;
		VirtualUnit	*	audioBus;	 
		
		Error Lock(VirtualUnit * unit);
		Error Unlock(VirtualUnit * unit);
	public:
		ZR361X0(ByteIndexedInOutPort * bio, 
		        WordIndexedInOutPort * wio,
		        BitOutputPort		  * resetPin,
		        BitOutputPort		  * mb4,
		        MPEGAudioDecoder	  * audio,
		        BOOL						 mixedIO, 
		        WORD						 transferSize, 
		        KernelString				 mCodePath,
		        VirtualUnit			  * videoBus,
		        VirtualUnit          * audioBus,
		        InterruptServer		  * vblankIntServer);
		~ZR361X0(void);
      
		Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag); 
		Error CompleteCommand(DWORD tag);
		
//		DWORD SendData(APTR data, DWORD size);
		void CompleteData(void);
		DWORD GetData(APTR data, DWORD size);
		
		MPEGState CurrentState(void);    
		DWORD CurrentLocation(void);
	};



class VirtualZR361X0 : public VirtualMPEGDecoder {
	protected:
		ZR361X0	*	decoder;             
		
		BOOL			initialCall;
		DWORD			illegalFakeOffset;

		virtual Error SetOnlineParams(void);
		
		Error Preempt(VirtualUnit * unit);
	public:
		VirtualZR361X0(ZR361X0	*	unit);
		
		void IllegalSeek(void);
		
		void IllegalResync(DWORD pos);
	};

#endif
