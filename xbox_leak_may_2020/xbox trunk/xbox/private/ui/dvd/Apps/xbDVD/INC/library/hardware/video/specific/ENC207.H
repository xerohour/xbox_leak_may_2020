
// FILE:      library\hardware\video\specific\enc207.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   03.04.95
//
// PURPOSE: The class for the MD 0207 video encoder.
//
// HISTORY:

#ifndef ENC207_H
#define ENC207_H

#include "..\generic\videnc.h"
#include "..\..\..\lowlevel\hardwrio.h"



class VirtualMD207;   // forward declaration



class MD207 : public VideoEncoder
	{
	friend class VirtualMD207;

	protected:
		ByteIndexedInOutPort *port;
		
		VideoStandard	standard;
		VideoMode		mode;
		
		BOOL				changed;
		
		Error SetVideoStandard(VideoStandard std);
		Error SetMode(VideoMode mode);
		Error SetExtSync(BOOL extsync);

		Error ProcessChanges(void);

	public:
		MD207 (ByteIndexedInOutPort *port)
			: VideoEncoder() {this->port=port;}
	};



class VirtualMD207 : public VirtualVideoEncoder
	{
	friend class MD207;
   private:
   	MD207		*	md207;
	public:
		VirtualMD207 (MD207 *physical) : VirtualVideoEncoder(physical) {md207 = physical;}

		Error Configure (TAG *tags);

	protected:
		Error Preempt (VirtualUnit *previous);
	};



#endif
