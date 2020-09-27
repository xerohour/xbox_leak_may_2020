// FILE:      library\hardware\pcibridge\specific\i20\i20.h
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1998 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   17.03.98
//
// PURPOSE:   
//
// HISTORY:

#ifndef ZIVAPC_H
#define ZIVAPC_H

#include "library\hardware\mpeg2dec\specific\ccubdvd1.h"
#include "library\lowlevel\intrctrl.h"
#include "library\lowlevel\dmachanl.h"
#include "library\lowlevel\hwbusio.h"
//#include "library\lowlevel\irqdebug.h"
#include "library\hardware\videodma\generic\colctrlr.h"

#include "zivapcrg.h"

#define	GNR_ILLEGAL_DMA_BUFFER_SIZE	MKERR(ERROR, DMA, PARAMS, 0x010)
	// The size of the dma buffer was not in the supported range

// Move this definition to somewhere more common
enum PowerState {VD_POWER_D0, VD_POWER_D1, VD_POWER_D2, VD_POWER_D3, VD_POWER_D4, VD_POWER_D5};

#define SCATTER_GATHER_DMA 1

class ZiVAPCDVDDecoder;
class ZiVADMAChannel;

class ZiVAPC : public ColorController {
	friend class ZiVAPCIntServer;
	friend class ZiVAPCDVDDecoder;
	private:
		ColorControlValues clrCtrl;
		volatile BOOL	doVSYNCJob;

	protected:           
		volatile DWORD				intStatus, intShadow;	// IRQ status from bridge chip IRQ flags
		volatile DWORD				decoderIntStatus;			// IRQ status of decoder part
		volatile	BOOL				decoderIntEnabled;
		volatile	DWORD				decoderIrqMask;
		volatile DWORD				shadowDecoderIntStatus;
		int							chipRevision;

		ZiVAPCDVDDecoder		*	mpegDecoder;

		void UpdateColorControlValues();
	public:
		//
		// Config space port
		// 
		//
		IndexedInOutPort		*	configPort;
		
		
		//
		// Host I/O Memory area
		//
		IndexedInOutPort		*	hostPort;
		
		//
		// I2C bus supported by the ZiVA-PC
		//
		ByteInOutBus			*	i2c;
		
		//
		// General I/O pins
		//
		BitIndexedInOutPort	*	genPPort;
	
   	//
   	// DVD decoder host I/O port
   	//
		ByteIndexedInOutPort	*	decoderPort;

		//
		// Interrupts
		//		
		InterruptServer		*	decoderIRQ;


		//
		// DMA channels
		//
		ZiVADMAChannel			*	muxChannel;	// channel for muxed data or video
		ZiVADMAChannel			*	extChannel;	// channel for audio

		ZiVAPC(IndexedInOutPort * hostPort, IndexedInOutPort * configPort);
		~ZiVAPC(void);

		Error ReInitialize(void);

		void SetMPEGDecoder(ZiVAPCDVDDecoder * decoder);
		
#if 0	// WDM_VERSION
		DWORD	GetDecoderIntStatus() {return decoderIntStatus;}
		// The function above was simplified from:
		//{decoderIntStatus |= shadowDecoderIntStatus; shadowDecoderIntStatus = 0; return decoderIntStatus;}
		// to avoid a late reset of the shadowDecoderIntStatus
#else
		DWORD	GetDecoderIntStatus() {decoderIntStatus |= shadowDecoderIntStatus; shadowDecoderIntStatus = 0; return decoderIntStatus;}
#endif
		void	ClearDecoderIntStatus() {decoderIntStatus = 0;}

		Error SetInterruptMask(DWORD irqMask);
		Error EnableDecoderInts();
		Error DisableDecoderInts();

		//
		// Initialize the ZiVA-PC
		//		
		virtual Error BuildPorts(void);		

		//
		// Configure code dma
		//
//		virtual Error ConfigureCodeDMA(int no, int reg, int trshld);
		
		//
		// To be called in the isr
		//
		virtual void Interrupt(void);
		InterruptServeType CheckInterrupt(void);
		
		// Function which is executed in the VSYNC IRQ of the DVD decoder part
		virtual void ExecuteVSYNCJob();

		//
		// Power management functions
		//
		Error SetPowerState(PowerState state);

		// Color Controller functions
		WORD  GetColorControl();
		Error SetColorControlValues(ColorControlValues __far & clrCtrl);

		Error	InitializeColorControl();
	};
      

class ZiVADMAChannel : public QueuedDMAChannel
	{
	friend class ZiVAPCDVDDecoder;
	friend class ZiVAPC;
	private:
		IndexedInOutPort * hostPort;

		int				channel;

		volatile DWORD	stateFlags;
		
		volatile DWORD	dmaBufferBase;
		volatile DWORD	dmaBufferSize;
		volatile DWORD dmaBufferStep;
		volatile DWORD dmaBufferOffset;
		volatile int	dmaChannel;

		ColorControlValues clrCtrl;
		volatile BOOL	doVSYNCJob;

		volatile	int	buffCount;

		Error InitTransfer(DMABuffer * buffer, DWORD start, DWORD size);
#if !SCATTER_GATHER_DMA
		Error InitCircularTransfer(DMABuffer * buffer, DWORD start, DWORD reportStep);
#endif

		Error StartTransfer(void);
		Error StopTransfer(void);
		Error EndTransfer(void);   
		
#if !SCATTER_GATHER_DMA
		Error StartNextDMAPacket(void);
#endif
		Error DMAPacketFinished(BOOL queueEmpty);
		
		long GetTransferLocation(void);
		void SetTransferLocation(long pos);
#if SCATTER_GATHER_DMA
		BOOL AcceptsData();
#endif

	public:
		ZiVADMAChannel(IndexedInOutPort * hostPort, int channel);
	};


class ZiVAPCDVDDecoder : public CCubeDVD1Decoder
	{
	friend class ZiVAPC;
	private:
		ZiVAPC * ziva;
		DWORD		irqMask;
	protected :
		// Coming from CCubeDVD1Decoder class
		DWORD	GetIRQStatus();
		void	ClearIRQStatus();
		Error	SetInterruptMask(DWORD mask) {GNRAISE(ziva->SetInterruptMask(mask));}

		void	PackSyncClear();

		void	ExecuteVSYNCJob();

	public :
		ZiVAPCDVDDecoder(Profile							*	profile,
							  ZiVAPC								*	ziva,
							  DualASyncOutStream				*	strm,
							  ProgrammableClockGenerator	*	audioClock,
							  VirtualAudioDAC					*	dac,
							  VirtualUnit						*	videoBus,  
							  VirtualUnit						*	audioBus,
							  VideoStandard						videoStandard,
							  BOOL                           lpcmSpdifOut,
							  BOOL									isZiVAPC) : CCubeDVD1Decoder(ziva->decoderPort, ziva->decoderIRQ, strm, audioClock,
							  																				  dac, videoBus, audioBus, NULL,
							  																				  NULL, NULL, videoStandard,
							  																				  lpcmSpdifOut)
			{
			this->ziva = ziva;
			irqMask = 0;
			SetProfile(profile);
			SetIsZiVAPC(isZiVAPC);
			}
	};

inline DWORD ZiVAPCDVDDecoder::GetIRQStatus() 
	{
	return ziva->GetDecoderIntStatus();
	}

inline void ZiVAPCDVDDecoder::ClearIRQStatus()
	{
	ziva->ClearDecoderIntStatus();
	}

	//
	// Interrupt server for Ziva interrupt
	//	
class ZiVAPCIntServer : public InterruptServer {
	private:
		int		intCtrlBit;	// bit in the interrupt control registers
		ZiVAPC	*	ziva;
	protected:
		Error EnableInt(void);
		Error DisableInt(void);		
	public:
		ZiVAPCIntServer(ZiVAPC * ziva, int intCtrlBit) : InterruptServer()
			{this->ziva = ziva; this->intCtrlBit = intCtrlBit;}
	};                                                                                          

#endif 
      