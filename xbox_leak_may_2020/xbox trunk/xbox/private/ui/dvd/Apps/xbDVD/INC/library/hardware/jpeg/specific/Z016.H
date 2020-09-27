
// FILE:      library\hardware\jpeg\generic\z016.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   05.04.95
//
// PURPOSE: The class for the Zoran ZR36016 Motion JPEG controller.
//
// HISTORY:

#ifndef Z016_H
#define Z016_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\lowlevel\hardwrio.h"
#include "library\hardware\video\generic\vidtypes.h"
#include "library\support\gfxdebug\gfxdebug.h"



// When you use the Z016 configuration, note that it has a default setting
// defined by the constructor. Change only those settings that you need.

class Z016_CONFIG
	{
	public:
		Z016_CONFIG (void);

		BOOL compress;             // compression or expansion ?
		BOOL stillMode;            // still or motion mode ?
		BOOL thumbnail;
		BOOL colFiltersOn;
		BOOL horFilterOn;
		BOOL hDecimation;          // horizontal decimation ?
		BOOL vDecimation;          // vertical decimation ?
		BOOL doubleClock;
		BOOL ccirLevels;
		BIT syen;
		VideoFormat vbusFormat;    // on video bus side
		VideoFormat coderFormat;   // on 050 side
		VideoFormat pxoutFormat;   // 016 driving video bus in expansion
		int top, left;
		int width, height;
	};

typedef Z016_CONFIG __far *LP_Z016_CONFIG;



class Z016Class
	{
	protected:
		ByteIndexedInOutPort *port;

	public:
		Z016Class (ByteIndexedInOutPort *port) {this->port = port;}

		BOOL Configure (LP_Z016_CONFIG cfg);
		void Start (void) {port->OutByte(0, 0x01);}
		void Stop  (void) {port->OutByte(0, 0x00);}
	};



#endif
