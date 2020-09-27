
// FILE:      library\hardware\video\specific\encCH7004.h
// AUTHOR:    Sam Frantz
// COPYRIGHT: (c) 2000 Ravisent Technologies, Inc.  All Rights Reserved.
// CREATED:   10-MAR-2000
//
// PURPOSE: The class for the Chrontel 7004C video encoder
//
// HISTORY:

#ifndef ENCCH7004_H
#define ENCCH7004_H

#include "library\common\virtunit.h"
#include "library\hardware\video\generic\ccapvenc.h"
#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\i2c.h"
#include "encCH7004reg.h"
#include "library\hardware\video\specific\enc5505.h"

#define MUX_SRC_EXTERNAL		1	// for WEBDVD: PIO1 bit6 - high
#define MUX_SRC_INTERNAL		0	// for WEBDVD: PIO1 bit6 - low


class VirtualEncCH7004;

typedef struct
	{
	// reg10H: MVEN, EOFP, VBIP, CSP
	BYTE	bMVEN, bEOFP, bVBIP, bCSP;
	// reg04H&05H BPWD[8:0], reg01H&02H BPST1[9:0]
	WORD	wBPWD, wBPST1;
	// reg08H BPED1[4:0]
	BYTE	bBPED1; 
	// reg0EH&0CH BPST2[9:0], reg17H&16H BPED2[9:0]
	WORD	wBPST2, wBPED2;
	// reg2BH ADBST
	BYTE	bADBST; 
	// reg2BH&2CH Z1S[8:0], reg2BH&2DH Z2S[8:0]
	WORD	wZ1S, wZ2S;
	// reg2BH&2EH ASB[8:0]
	WORD	wASB;
	// reg11H PZ[3:1] 
	BYTE	bPZ;
	// reg12H: P1L[1:0], P2L[1:0], P3L[1:0], P4L[1:0], P5L[1:0]
	BYTE	bP1L, bP2L, bP3L, bP4L, bP5L;
	// reg13H LPCS[2:0], reg0DH CSPF[3:0], reg0FH CSSP[5:0]
	BYTE	bLPCS, bCSPF, bCSSP;
	// reg2FH FL1F[6:0]
	BYTE	bFL1F;
	// reg2FH&30H FL2F[8:0], reg19H&18H F1SP[5:0], reg19H&1AH F2SP[5:0]
	WORD 	wFL2F, wF1SP, wF2SP;
	// reg2BH&31H PSLA[8:0], reg2BH&32H PSLB[8:0]
	WORD	wPSLA, wPSLB;
	// reg2AH AGCD[6:0]
	BYTE	bAGCD;
	// reg33H PSDA[6:0], reg34H PSDB[6:0]
	BYTE	bPSDA, bPSDB;
	// reg2BH&35H PSSA[8:0], reg2BH&36H PSSB[8:0]
	WORD	wPSSA, wPSSB;
	// reg3BH PSCA[7:0], reg3CH PSCB[7:0]
	BYTE	bPSCA, bPSCB;
	// reg37H&38H PSON[14:0], reg39H&3AH PSFM[14:0]
	WORD	wPSON, wPSFM;
	// reg3DH: VAMP, HAMP, AGCY
	BYTE	bVAMP, bHAMP, bAGCY;
	} EncCH7004MacrovisionSettings;


class EncCH7004 : public CloseCaptionVideoEncoder
	{
	friend class VirtualEncCH7004;

	protected:
		Enc5505					*internalEncoder;
		VirtualEncCH7004			*defaultVEncoder;

		// physical ports
		MemoryMappedIO				*mem;
		BitOutputPort				*videoActivePort;
		BitOutputPort				*svideoActivePort;
		BitOutputPort				*muxPort;
		AsyncByteInOutBus			*i2c;

		// general state parameters
		DWORD							changed;	// state changed
		BOOL							ignoreErrors;
		
		// Chrontel 7004 register values
		BYTE					encoderVersionId;
		int					displayMode;	// Table 16 - Mode selection
		int					inputDataFormat;	// Table 21, p. 35 in CH7004C
		int					pllm, plln;	// Table 25 - PLL M and N values
		int					pllcap;		// Table 30 - PLL capacitor setting
		int					blackLevel;	// Black Level
		int					clockMode;	// master/slave+divider, p.36, table 22
		int					calculatedIncrement;	// p. 44, 1 for slave, 0 for master
		int					syncPolarity;	// p. 38, symbol SPR
		int					videoBandwidth; // p. 34, register 0x03
		int					flickerFilterMode; // p. 33, register 0x01
		int					pmScart;	// Power Management - SCART control
		int					subCarrierFrequency;
		int					startActiveVideoPixelDelay; 
		
		// These four are part of the VirtualVideoChip class
		VideoStandard			standard;		// current video standard
		PALVideoSubStandard	palSubStandard;
		NTSCVideoSubStandard	ntscSubStandard;
		VideoMode			mode;				// current video mode
		BOOL					extSync;			// current state of extSync
	
		// These four are part of the VirtualVideoEncoder class
		int					copyProtection;
		DWORD					chromaFilterFrequency;
		int					hOffset;
		int					vOffset;
		
		//what is this?		BOOL	ntscSetup;		// NTSC 7.5 IRE setup
		BOOL					videoInverted;
		BOOL					hSyncPol;
		BOOL					vSyncPol;
		int					syncMode;
		BOOL					fixedExtSync;
		int					component;
		int					vsyncCopyProtection;
		BOOL					enableMacrovision;
		
		VideoMuxSource		muxSetting;	// VIDEOMUXSRC_INTERNAL or VIDEOMUXSRC_EXTERNAL
		VideoLineMode		lineMode;	// VLM_PROGRESSIVE or VLM_INTERLACED
		VideoPixClockMode	pixclkMode;	// VIDEOPIXCLK_INTERNAL or VIDEOPIXCLK_EXTERNAL
		BOOL					embeddedSync;
		int					idleDisplay;

		// Change parameters of physical unit.
#ifdef DUMP_VIDEO_REGISTERS
		void ReadAllRegisters(void);
#endif
		virtual Error SetVideoStandard (VideoStandard std);
		virtual Error SetMode (VideoMode mode);
		virtual Error SetExtSync (BOOL extsync);
		virtual Error SetPALVideoSubStandard (PALVideoSubStandard palSubStandard);
		virtual Error SetNTSCVideoSubStandard (NTSCVideoSubStandard ntscSubStandard);
		virtual Error SetSampleMode (VideoSampleMode mode);
		virtual Error SetHOffset (int offset);
		virtual Error SetVOffset (int offset);
		virtual Error SetIdleScreen (int idleScreen);
		virtual Error SetIdleScreen (void);
		virtual Error SetCopyProtection (int protection);
		virtual Error ApplyCopyProtection(EncCH7004MacrovisionSettings *p);
		virtual Error SetChromaFilter (DWORD frequency);
		virtual Error SetSVideoActive (BOOL active);
		virtual Error SetMuxSetting(VideoMuxSource muxSetting);
		virtual Error SetLineMode(VideoLineMode lineMode);
		virtual Error SetPixclkMode(VideoPixClockMode pixclkMode);
		virtual Error SetEmbeddedSync(BOOL embeddedSync);
		virtual Error SetHSyncPol(BOOL hSyncPol);
		virtual Error SetVSyncPol(BOOL vSyncPol);
		virtual Error SetStartActiveVideoPixelDelay(int startActiveVideoPixelDelay);
		virtual Error SetVideoStandardDependentParameters();

      // Perform the changes
		Error ProcessChanges (void);

		// Program specific encoder modes
		Error ProgramCopyProtection (int protection);

		Error SetInit (void);
		Error DisableEncoder (void);
		Error EnableEncoder (void);
		Error ConfigureClockMode();
		Error ConfigureMuxMode();
		Error ConfigureSyncMode();
		Error ConfigureVideoMode();

		Error Configure (TAG __far *tags);

		virtual void Interrupt (void);

		Error GetCCStatus(BOOL __far & first, BOOL __far & second);
		Error PutCCData(BOOL field, BYTE d1, BYTE d2);
		Error EnableCCTransfer(void);
		Error DisableCCTransfer(void);		
		Error CopySettingsToInternalEncoder(void);
	public:
		EncCH7004 (MemoryMappedIO *mem, GenericProfile *profile = NULL,
			BitOutputPort *videoActivePort = NULL, 
			BitOutputPort *svideoActivePort = NULL, 
			AsyncByteInOutBus *i2c = NULL,
			BitOutputPort *muxPort = NULL,
			Enc5505 *enc5505 = NULL);
		~EncCH7004 (void);

		VirtualUnit * CreateVirtual (void);

		void DeactivateOutputs (void);
		void ReactivateOutputs (void);
	};



class VirtualEncCH7004 : public VirtualCloseCaptionVideoEncoder
	{
	friend class EncCH7004;

	private:
		EncCH7004	*ch7004;
		int		instance;
		
		// VideoStandard			standard;		// current video standard
		// PALVideoSubStandard	palSubStandard;
		// NTSCVideoSubStandard	ntscSubStandard;
		// VideoMode				mode;	// current video mode
		// BOOL						extSync;			// current state of extSync
	
		// These four are also part of the VirtualVideoEncoder class
		// int					copyProtection;
		// DWORD					chromaFilterFrequency;
		// int					hOffset;
		// int					vOffset;
	
		// These members are specific to the EncCH7004 class
		VideoMuxSource		muxSetting;	// VIDEOMUXSRC_INTERNAL or VIDEOMUXSRC_EXTERNAL
		VideoLineMode		lineMode;	// VLM_PROGRESSIVE or VLM_INTERLACED
		VideoPixClockMode	pixclkMode;	// VIDEOPIXCLK_INTERNAL or VIDEOPIXCLK_EXTERNAL
		BOOL					embeddedSync;
		BOOL					hSyncPol; 
		BOOL					vSyncPol; 
		int					startActiveVideoPixelDelay; 
		
		int					component;
		BOOL					enableMacrovision;
		int					vsyncCopyProtection;
		BOOL					svideoActive;
		int					idleDisplay;

		BOOL					ignoreErrors;
		DWORD					changed;	// state changed

	// protected:
		// Error PreemptStopPrevious(VirtualUnit * previous);
		// Error PreemptStartNew(VirtualUnit * previous);
      // Error PreemptChange(VirtualUnit * previous);
		
	public:
		VirtualEncCH7004 (EncCH7004 *physical, BOOL interlaced=TRUE, BOOL returnOK=TRUE, int instance=0);
		Error Configure (TAG __far *tags);

	protected:
		Error Preempt (VirtualUnit *previous);
	};



#endif
