
// FILE:      library\hardware\mpegdec\specific\cs4920.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   10.04.95
//
// PURPOSE:   Implementation of ZR36100 abstract audio decoder class.
//
// HISTORY:

#ifndef CS4920_H
#define CS4920_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\common\krnlstr.h"
#include "library\lowlevel\hardwrio.h"
#include "zr36100.h"

#define	GNR_NO_CRYSTAL_MCODE				MKERR(ERROR, MPEG, FILEIO, 0x10)
	// The microcode for the audio DSP could not be found

#define	GNR_CRYSTAL_DOWNLOAD_FAILED   MKERR(ERROR, MPEG, NOACK,  0x11)
	// The microcode download failed.
	
#define I2C_CS4920	0xCC
	// I2C address for the currently used microcode.

class CS4920 : public MPEGAudioDecoder {
	private:
		ByteInOutPort		*	i2c;
		BitOutputPort		*	reset;
		BitOutputPort		*	boot; 
		BitInputPort		*	request;
		
		WORD						leftVolume;
		WORD						rightVolume;
		
	public:
		CS4920(ByteInOutPort * i2c,			// I2C bus for comunication
		       BitOutputPort * reset,			// __HIGH__ active reset line
		       BitOutputPort * boot,			// __HIGH__ active boot line
		       BitInputPort	* request);		// __HIGH__ active request line

			//
			// Check, if a microcode is already present
			//
		BOOL CheckMicrocode(void);		       		
		Error SendCommand(BYTE command, WORD level);		
		
			//
			// Load a new microcode into the chip
			//
		Error LoadMicrocode(KernelString name);
	
			//
			// Mute/Unmute the audio outputs
			//		
      Error MuteAudio(BOOL mute);      
      
      	//
      	// Set the channel volumes [0..10000]
      	//
      Error SetLeftVolume(WORD volume);
      Error SetRightVolume(WORD volume);		
	};

#endif
