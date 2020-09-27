// FILE:      library\hardware\video\specific\enc8172.h
// AUTHOR:    M. Stephan
// COPYRIGHT: (c) 1999 Viona Development.  All Rights Reserved.
// CREATED:   23.6.99
//
// PURPOSE: The class for the  HMP8172 video encoder
//
// HISTORY:

#ifndef ENC8172_H
#define ENC8172_H

#include "..\generic\videnc.h"

class VirtualHMP8172;

class HMP8172 : public VideoEncoder 
	{
	friend class VirtualHMP8172;

	protected:
		
	public:
		HMP8172(void);
		~HMP8172(void);
	};



class VirtualHMP8172 : public VirtualVideoEncoder
	{
	friend class HMP8172;

	private:
		HMP8172 *	hmp8172;

	public:
		VirtualHMP8172 (HMP8172 * physical) : VirtualVideoEncoder (physical) {hmp8172 = physical;}
	};

#endif

