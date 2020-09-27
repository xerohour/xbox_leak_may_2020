
// FILE:      library\hardware\mpegdec\specific\zr36100.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   10.04.95
//
// PURPOSE:   
//
// HISTORY:

#ifndef ZR36100_H
#define ZR36100_H

#include "zr361x0.h"
	
class ZR36100 : public ZR361X0 {
	friend class VirtualZR36100;
	private:                       
		DWORD lastCommandInt;
		
		Error WaitForReady(void);
		Error SendChipCommand(WORD command);

		Error SendRegisterSet(MPEGParams __far &params);
		Error InitChip(MPEGParams __far &params);

		DWORD SendFakeHeader(MPEGParams __far &params);
		
		void VBlankInterrupt(void);

		BOOL			forwardRequest;

	public:
		ZR36100(ASyncOutStream       * strm, 
		        ByteIndexedInOutPort * bio, 
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
		~ZR36100(void);

		VirtualUnit * CreateVirtual(void);
	};



class VirtualZR36100 : public VirtualZR361X0 {
	protected:
		Error GetDisplaySize(WORD __far &width, WORD __far &height);
	public:
		VirtualZR36100(ZR361X0	*	unit);
	};

#endif

