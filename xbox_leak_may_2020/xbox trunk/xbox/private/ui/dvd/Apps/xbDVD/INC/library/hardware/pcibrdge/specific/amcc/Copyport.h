
#ifndef COPYPORT_H
#define COPYPORT_H

#include "library\common\prelude.h"


extern "C"
	{
#if NT_KERNEL
	void __stdcall CopyFromInPort(WORD pollport, WORD dataport, FPTR p, WORD dsize);
	void __stdcall CopyToOutPort(WORD pollport, WORD dataport, FPTR p, WORD dsize, WORD rest);

	void __stdcall CopyBytesFromInPort(WORD dataport, FPTR p, DWORD bytesize);
	void __stdcall CopyBytesToOutPort(WORD dataport, FPTR p, DWORD bytesize);
#else
	void FAR PASCAL CopyFromInPort (WORD pollport, WORD dataport, FPTR p, WORD dsize);
	void FAR PASCAL CopyToOutPort (WORD pollport, WORD dataport, FPTR p, WORD dsize, WORD rest);

	void FAR PASCAL CopyBytesFromInPort (WORD dataport, FPTR p, DWORD bytesize);
	void FAR PASCAL CopyBytesToOutPort  (WORD dataport, FPTR p, DWORD bytesize);
#endif
	};

#endif
