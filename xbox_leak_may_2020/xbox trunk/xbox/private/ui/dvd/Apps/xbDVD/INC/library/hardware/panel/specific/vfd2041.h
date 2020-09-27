// FILE:      vfd2041.h
// AUTHOR:    H.Horak
// COPYRIGHT: (c) 1998 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   29.09.98
//
// PURPOSE:   
//
// HISTORY:

#ifndef VFD2041_H
#define VFD2041_H

#include "library\lowlevel\hwbusio.h"
#include "library\common\gnerrors.h"
#include "library\common\krnlsync.h"



#define VFD2041_I2C_ADDRESS  0x5c



class VFD2041
	{
	private:
		AsyncByteInOutBus *port;
		VDMutex monitorMutex;
		BOOL	enablePanel;
		
	public:
		VFD2041 (AsyncByteInOutBus *port);
		virtual ~VFD2041 (void);

		Error Initialize (void);

		Error PanelWrite (char * str, int row, int column);

		Error PanelButton (char * str);

		Error PanelPlaybackSpeed (int speed);

		Error PanelBitRate (int kiloBits);

		Error PanelChapter(int chapter);

		Error PanelAudioBits(int bitsPerSample);

		Error PanelAudioSamples(int samplesPerSecond);

		Error PanelAudioChannels(int channels);

		Error PanelTime (int hours, int minutes, int seconds);

		Error VFD2041::PanelClr (void);

	};
#endif
