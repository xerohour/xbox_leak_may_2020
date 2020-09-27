/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    cl.h

Abstract:

    function declarations and type definitions for cll.cpp

Author:

    Jason Gould (a-jasgou)   May 2000

Revision History:

--*/
#ifndef __cl_h_
#define __cl_h_

//#include "cpx.h"

typedef VOID (*ControlFunc)(DWORD context, BOOL repeat, BYTE slot);

typedef union {
	struct{
		DWORD Db   : 16;
		DWORD Ab0  : 1;				//16
		DWORD Ab1  : 1;				//17
		DWORD Ab2  : 1;				//18
		DWORD Ab3  : 1;				//19
		DWORD Ab4  : 1;				//20
		DWORD Ab5  : 1;				//21
		DWORD Ab6  : 1;				//22
		DWORD Ab7  : 1;				//23
		DWORD LtUp : 1;				//24
		DWORD LtDn : 1;				//25
		DWORD LtLt : 1;				//26
		DWORD LtRt : 1;				//27
		DWORD RtUp : 1;				//28
		DWORD RtDn : 1;				//29
		DWORD RtLt : 1;				//30
		DWORD RtRt : 1;				//31
		DWORD DpUp : 1;				//32
		DWORD DpDn : 1;				//33
		DWORD DpLt : 1;				//34
		DWORD DpRt : 1;				//35
		DWORD R    : 28;			//36-64
	};
	UINT64 all;
} BUTTONS;

//this big mess here should map exactly to the structure above, and can be used in the elmement "all".
#define bDB0_PRESSED				0x00000001
#define bDB1_PRESSED				0x00000002
#define bDB2_PRESSED				0x00000004
#define bDB3_PRESSED				0x00000008
#define bDB4_PRESSED				0x00000010
#define bDB5_PRESSED				0x00000020
#define bDB6_PRESSED				0x00000040
#define bDB7_PRESSED				0x00000080
#define bDB8_PRESSED				0x00000100
#define bDB9_PRESSED				0x00000200
#define bDB10_PRESSED				0x00000400
#define bDB11_PRESSED				0x00000800
#define bDB12_PRESSED				0x00001000
#define bDB13_PRESSED				0x00002000
#define bDB14_PRESSED				0x00004000
//#define bDB15_PRESSED				0x00008000
#define bAB0_PRESSED				0x00010000
#define bAB1_PRESSED				0x00020000
#define bAB2_PRESSED				0x00040000
#define bAB3_PRESSED				0x00080000
#define bAB4_PRESSED				0x00100000
#define bAB5_PRESSED				0x00200000
#define bAB6_PRESSED				0x00400000
#define bAB7_PRESSED				0x00800000
#define bLTS_UP_PRESSED				0x01000000
#define bLTS_DN_PRESSED				0x02000000
#define bLTS_LT_PRESSED				0x04000000
#define bLTS_RT_PRESSED				0x08000000
#define bRTS_UP_PRESSED				0x10000000
#define bRTS_DN_PRESSED				0x20000000
#define bRTS_LT_PRESSED				0x40000000
#define bRTS_RT_PRESSED				0x80000000

#define bANY						0x00008000
#define bALL						0x00000000

#define bANYBUTTON					0x00ffffff
#define bANYTHING					0xffffffff

#define bDPD_UP_PRESSED				bDB0_PRESSED
#define bDPD_DN_PRESSED				bDB1_PRESSED
#define bDPD_LT_PRESSED				bDB2_PRESSED
#define bDPD_RT_PRESSED				bDB3_PRESSED
#define bSTART_PRESSED				bDB4_PRESSED
#define bBACK_PRESSED				bDB5_PRESSED
#define bLT_PRESSED					bDB6_PRESSED
#define bRT_PRESSED					bDB7_PRESSED

//#define bSELECT_PRESSED bBACK_PRESSED //hackhack

typedef struct _CF{
	BUTTONS but;
	ControlFunc func;
	struct _CF * next;
	DWORD context;
	BOOL repeat;
}CF, *pCF;

class CL{
private:
	CF head;
	int num;
public:
	CL();
	~CL();
	DWORD RegisterControl(BUTTONS but, ControlFunc func, DWORD context);
	DWORD CheckControls();
};

extern BYTE gDontRead;				//the index of a controller to not read commands from

BUTTONS GetButtons(DWORD slot);

BOOL cclCheckButtons(int slot, UINT64 b);	//use slot==-1 to check all.


#endif