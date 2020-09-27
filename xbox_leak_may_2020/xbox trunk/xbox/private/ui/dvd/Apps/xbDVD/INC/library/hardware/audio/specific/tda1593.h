// FILE:			library\hardware\audio\specific\tda1593.h
// AUTHOR:		Stefan Herr
// COPYRIGHT:	(c) 1996 Viona Development.  All Rights Reserved.
// CREATED:		23.08.95
//
// PURPOSE:		Class for Philips TDA1593 FM stereo decoders
//
// HISTORY:

#ifndef TDA1593_H
#define TDA1593_H

#include "library\lowlevel\hardwrio.h"
#include "library\hardware\audio\generic\stereodc.h"

class TDA1593 : public StereoDecoder	{
	friend class VirtualTDA1593;
	protected:
		BitInputPort 	*	stereoDetectPort;
		BitOutputPort	*	forceMonoPort;

		BOOL	forceMono;
		
		BOOL	changed;
	
		Error GetSignalMode(SignalMode __far & mode);
		Error SetSignalMode(SignalMode mode);

		Error ProcessChanges();
	public:
		TDA1593(BitInputPort * stereoDetectPort, BitOutputPort * forceMonoPort);

		VirtualUnit * CreateVirtual(void); 
	};
	
class VirtualTDA1593 : public VirtualStereoDecoder {
	private:
		TDA1593	*	decoder;
	public:
		VirtualTDA1593(TDA1593 * decoder);
	};

#endif
