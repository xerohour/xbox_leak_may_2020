
// FILE:      library\hardware\jpeg\generic\jcodrate.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   12.01.96
//
// PURPOSE: JPEG coder with bit rate control class.
//
// HISTORY:

#ifndef JCODRATE_H
#define JCODRATE_H

#include "jcoder.h"



#ifndef ONLY_EXTERNAL_VISIBLE



class JCoderRated : public JCoder
	{
	protected:
		virtual WORD GetLastScaleFactor (void) = 0;
	};



class VirtualJCoderRated : public VirtualJCoder
	{
	public:
		VirtualJCoderRated (JCoderRated *physical);

		virtual Error Configure (TAG __far *tags);

		virtual WORD GetLastScaleFactor (void) = 0;

	protected:
		DWORD totalCodeVolume;   // in bytes
		WORD  maxBlockCodeVolume;
		WORD  scaleFactor;
		BOOL  enableDRI;
		WORD  dri;
	};



#endif // ONLY_EXTERNAL_VISIBILE



#endif
