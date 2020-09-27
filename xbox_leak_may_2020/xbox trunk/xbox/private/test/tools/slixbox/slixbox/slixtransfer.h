/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    slixtransfer.h

Abstract:

    a shared header for slixbox and slix2k, including networking protocols

Author:

    Jason Gould (a-jasgou) June 2000

--*/
#ifndef __slixtransfer_h_
#define __slixtransfer_h_

#define __TRANSFER_BASE struct{unsigned long size; BYTE type;} 

typedef __TRANSFER_BASE TRANSFER_BASE;					//base structure for all sends

#pragma warning(push)
#pragma warning(disable:4200)	//since we're using 0-length arrays... kill the warning

typedef struct : TRANSFER_BASE{	//box->2k IN data
	BYTE ret;
	char data[0];	
} TRANSFER_IN;					

typedef struct : TRANSFER_BASE{ //2k->box OUT data
	BYTE address;
	BYTE endpoint;
	BYTE dataPid;
	char data[0];
} TRANSFER_OUT;

typedef struct : TRANSFER_BASE{	//2k->box SETUP data
	BYTE address;
	BYTE endpoint;
	char data[0];
} TRANSFER_SETUP;

typedef struct : TRANSFER_BASE{	//2k->box DataToggle messages
	BYTE endpoint;
} TRANSFER_DATA_TOGGLE;

typedef struct : TRANSFER_BASE{	//box->2k result of OUT or SETUP
	BYTE ret;
} TRANSFER_RETURN;

typedef struct : TRANSFER_BASE{	//2k->box request for IN
	BYTE address;
	BYTE endpoint;
	DWORD max;
} TRANSFER_INIT_IN;

typedef struct : TRANSFER_RETURN{
	DWORD linefailed;
} TRANSFER_RETURN_END_QUEUE;

typedef struct : TRANSFER_BASE{
	BYTE successonnak;
} TRANSFER_BEGIN_QUEUE;

typedef struct : TRANSFER_BASE{
} TRANSFER_INIT_END_QUEUE;

typedef struct : TRANSFER_BASE{
} TRANSFER_RESET;

#pragma warning(pop)

typedef union ussTransferData{	//all possible data & transfer types
	__TRANSFER_BASE;
	TRANSFER_INIT_IN initin;
	TRANSFER_IN in;
	TRANSFER_OUT out;
	TRANSFER_SETUP setup;
	TRANSFER_DATA_TOGGLE toggle;
	TRANSFER_RETURN ret;
	TRANSFER_RESET reset;
	TRANSFER_RETURN_END_QUEUE retendqueue;
	TRANSFER_INIT_END_QUEUE initendqueue;
	TRANSFER_BEGIN_QUEUE beginqueue;
	char all[1032];
} TRANSFERDATA, *pTRANSFERDATA;

#define RECV_TIMEOUT				10000

#define PID_OUT                    0x01
#define PID_IN                     0x69
#define PID_SETUP                  0x2D
#define PID_DATA0			       0xC3
#define PID_DATA1			       0x4B

//these aren't PID codes, but they are being used in the same way, in my code, 
//as the 5 codes above. They are just arbitrary numbers...
#define PID_RET					   0xDA	
#define PID_INIT_IN				   0xDB 
#define PID_RESET				   0xDC
#define PID_BEGIN_QUEUE			   0xDD
#define PID_INIT_END_QUEUE		   0xDE
#define PID_RETURN_END_QUEUE	   0xDF

const BYTE XSTATUS_Success            = 0x00;
const BYTE XSTATUS_Ack                = 0x02;
const BYTE XSTATUS_Nak                = 0x0A;
const BYTE XSTATUS_Stall              = 0x0E;
const BYTE XSTATUS_Ignore             = 0x80;
const BYTE XSTATUS_DataCRCError       = 0x81;
const BYTE XSTATUS_DataToggleError    = 0x82;
const BYTE XSTATUS_SyncError          = 0x83;
const BYTE XSTATUS_BabbleError        = 0x84;
const BYTE XSTATUS_PIDError           = 0x85;
const BYTE XSTATUS_ShortPacketError   = 0x86;
const BYTE XSTATUS_ConfigurationError = 0x87;
const BYTE XSTATUS_TransmissionError  = 0x92;	//this is new, as of 6/21/2000
const BYTE XSTATUS_UserError          = 0x95;	//new, as of 8-28-2000


//Macro to fill in a TRANSFER_IN_INIT packet to send from Slix2k to Slixbox
#define CREATE_BEGIN_QUEUE(_sockdata, _SuccessOnNak)					\
	_sockdata.size = sizeof(TRANSFER_BEGIN_QUEUE);		\
	_sockdata.type = PID_BEGIN_QUEUE;					\
	_sockdata.beginqueue.successonnak = (BYTE)_SuccessOnNak;

#define CREATE_INIT_END_QUEUE(_sockdata)					\
	_sockdata.size = sizeof(TRANSFER_INIT_END_QUEUE);	\
	_sockdata.type = PID_INIT_END_QUEUE;				

#define CREATE_ININIT(_sockdata, _address, _endpoint, _maxsize)	\
	_sockdata.size = sizeof(TRANSFER_INIT_IN);			\
	_sockdata.type = PID_INIT_IN;						\
	_sockdata.initin.address = _address;				\
	_sockdata.initin.max = _maxsize;					\
	_sockdata.initin.endpoint = _endpoint;

//Macro to fill in a TRANSFER_OUT packet to send from Slix2k to Slixbox
#define CREATE_OUT(_sockdata, _address, _endpoint, _pid, _data, _size)	\
	_sockdata.size = sizeof(TRANSFER_OUT) + _size;						\
	_sockdata.type = PID_OUT;											\
	_sockdata.out.address = _address;									\
	_sockdata.out.endpoint = _endpoint;									\
	_sockdata.out.dataPid = _pid;										\
	{int _arf; for(_arf = 0; _arf < _size; _arf++) sockdata.out.data[_arf] = _data[_arf];}	

//Macro to fill in a TRANSFER_SETUP packet to send from Slix2k to Slixbox
#define CREATE_SETUP(_sockdata, _address, _endpoint, _data, _size)		\
	_sockdata.size = sizeof(TRANSFER_SETUP) + _size;					\
	_sockdata.type = PID_SETUP;											\
	_sockdata.setup.address = _address;									\
	_sockdata.setup.endpoint = _endpoint;								\
	{int _arf; for(_arf = 0; _arf < _size; _arf++) sockdata.setup.data[_arf] = _data[_arf]; }

//#include "\MyAssert.h"	//definition of MyAssert, & its support features

#endif