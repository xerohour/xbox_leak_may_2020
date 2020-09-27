
// FILE:      library\hardware\mpegdec\specific\zr36110.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   10.04.95
//
// PURPOSE:   
//
// HISTORY:

#ifndef ZR36110_H
#define ZR36110_H

#include "zr361x0.h"

class VirtualZR36110;
	
class ZR36110 : public ZR361X0 {
	friend class VirtualZR36110;
	private:                       
		BOOL	audioIDPending;
		BYTE	newAudioID;
		
		Error WaitForReady(void);
		Error SendChipCommand(WORD command);
		
		Error SendRegisterSet(MPEGParams __far &params);
		Error InitChip(MPEGParams __far &params);

		DWORD SendFakeHeader(MPEGParams __far &params);

		Error SetBrightness(WORD brightness);
		Error SetContrast(WORD contrast);

		Error SetVideoStreamID(BYTE id);
		Error SetAudioStreamID(BYTE id);
		
		void VBlankInterrupt(void);
	public:
		ZR36110(ASyncOutStream       * strm, 
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
		~ZR36110(void);

		VirtualUnit * CreateVirtual(void);
	};



class VirtualZR36110 : public VirtualZR361X0 {
	protected:
		WORD brightness;
		WORD contrast;
				
		Error GetDisplaySize(WORD __far &width, WORD __far &height);
		Error Configure (TAG __far *tags);    
		
		Error SetOnlineParams(void);
	public:
		VirtualZR36110(ZR361X0	*	unit);
	};

#endif

