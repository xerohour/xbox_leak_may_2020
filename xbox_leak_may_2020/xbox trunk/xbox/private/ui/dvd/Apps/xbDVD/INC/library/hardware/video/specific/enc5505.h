
// FILE:      library\hardware\video\specific\enc5505.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1998 Viona Development.  All Rights Reserved.
// CREATED:   02.06.98
//
// PURPOSE: The class for the video encoder part in the STi5505.
//
// HISTORY:

#ifndef ENC5505_H
#define ENC5505_H

#include "library\common\virtunit.h"
#include "library\hardware\video\generic\ccapvenc.h"
#include "library\lowlevel\memmpdio.h"

#ifdef WEBDVD
class EncCH7004;
#endif

class VirtualEnc5505;


class Enc5505 : public CloseCaptionVideoEncoder
	{
	friend class VirtualEnc5505;
#ifdef WEBDVD
	friend class EncCH7004;
#endif

	protected:
		MemoryMappedIO	*	mem;
		BitOutputPort	*	videoActivePort;
		BitOutputPort	*	svideoActivePort;

		VideoStandard		standard;		// current video standard
		PALVideoSubStandard	palSubStandard;
		NTSCVideoSubStandard	ntscSubStandard;
		int					idleDisplay;
		VideoMode			mode;				// current video mode
		BOOL					extSync;			// current state of extSync
		int					copyProtection;
		DWORD					chromaFilterFrequency;

		int					vsyncCopyProtection;

		BOOL					ntscSetup;		// NTSC 7.5 IRE setup
		BOOL					videoInverted;
		BOOL					hSyncPol;
		BOOL					vSyncPol;
		int					syncMode;
		BOOL					fixedExtSync;
		int					component;
		BOOL					enableMacrovision;

		BYTE					chipSyncMode;

		DWORD					changed;			// state changed ??

		BYTE					ctrlState;

		// Change parameters of physical unit.
		virtual Error SetVideoStandard (VideoStandard std);
		virtual Error SetMode (VideoMode mode);
		virtual Error SetExtSync (BOOL extsync);
		virtual Error SetPALVideoSubStandard (PALVideoSubStandard palSubStandard);
		virtual Error SetNTSCVideoSubStandard (NTSCVideoSubStandard ntscSubStandard);
		virtual Error SetSampleMode (VideoSampleMode mode);
		virtual Error SetHOffset (int offset) {GNRAISE_OK;}
		virtual Error SetVOffset (int offset) {GNRAISE_OK;}
		virtual Error SetIdleScreen (int idleScreen);

      // Perform the changes
		Error ProcessChanges (void);

		virtual Error SetIdleScreen (void);
		virtual Error SetCopyProtection (int protection);
		virtual Error SetChromaFilter (DWORD frequency);
		virtual Error SetSVideoActive (BOOL active);

		//
		// Program specific encoder mode
		//
		Error ProgramCopyProtection (int protection);

		Error SetInit (void);
		Error SetNTSC (void);
		Error SetPAL (void);
		Error SetNTSCPattern (void);
		Error SetPALPattern (void);
		Error SetNTSCPlayback (void);
		Error SetPALPlayback (void);

		Error DisableEncoder (void);
		Error EnableEncoder (void);

		Error Configure (TAG __far *tags);

		virtual void Interrupt (void);

		Error GetCCStatus(BOOL __far & first, BOOL __far & second);
		Error PutCCData(BOOL field, BYTE d1, BYTE d2);
		Error EnableCCTransfer(void);
		Error DisableCCTransfer(void);		
	public:
		Enc5505 (MemoryMappedIO *mem, GenericProfile *profile = NULL,
		         BitOutputPort *videoActivePort = NULL, BitOutputPort *svideoActivePort = NULL);
		~Enc5505 (void);

		VirtualUnit * CreateVirtual (void);

		void DeactivateOutputs (void);
		void ReactivateOutputs (void);
	};



class VirtualEnc5505 : public VirtualCloseCaptionVideoEncoder
	{
	friend class Enc5505;

	private:
		Enc5505	*enc5505;

	public:
		VirtualEnc5505 (Enc5505 *physical)
			: VirtualCloseCaptionVideoEncoder (physical)
			{enc5505 = physical;}

		Error Configure (TAG __far *tags);

	protected:
		Error Preempt (VirtualUnit *previous);
	};



#endif
