//
//	qrand.cpp
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
#include "precomp.h"
#include "qrand.h"

int	QRand::Rand(int scale)
{
	_asm
	{
		mov		edi,this
		mov		eax,[edi]
		mov		ebx,eax
		ror		eax,13
		sub		ebx,11
		sub		eax,ebx
		mov		[edi],eax
		mul		scale
		mov		eax,edx
	}
}

int	QRand::Rand()
{

	_asm
	{
		mov		edi,this
		mov		eax,[edi]
		mov		ebx,eax
		ror		eax,13
		sub		ebx,11
		sub		eax,ebx
		mov		[edi],eax
	}
}
