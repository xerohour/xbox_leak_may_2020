/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    usbxtransfer.h

Abstract:

    a shared header for usbxcom and xboxserver, including networking protocols

Author:

    Jason Gould (a-jasgou) June 2000
	Dennis Krueger <a-denkru> November 2001

--*/
#ifndef __usbxtransfer_h_
#define __usbxtransfer_h_

#define __TRANSFER_BASE struct{unsigned long size; BYTE type;} 

typedef __TRANSFER_BASE TRANSFER_BASE;					//base structure for all sends

#pragma warning(push)
#pragma warning(disable:4200)	//since we're using 0-length arrays... kill the warning

typedef struct 
{
	BYTE bPort;
	BYTE bSlot;
	BYTE bInterfaceNumber;
} Topology;

typedef struct : TRANSFER_BASE{	//box->2k IN data
	BYTE ret;
	char data[0];	
} TRANSFER_IN;					

typedef struct : TRANSFER_BASE{ //2k->box OUT data
	BYTE Port;
	BYTE Slot;
	BYTE interfacenumber;
	BYTE endpoint;
	BYTE dataPid;
	char data[0];
} TRANSFER_OUT;

typedef struct : TRANSFER_BASE{	//2k->box SETUP data
	BYTE Port;
	BYTE Slot;
	BYTE interfacenumber;
	BYTE endpoint;
	char data[0];
} TRANSFER_SETUP;

typedef struct : TRANSFER_BASE{	//2k->box DataToggle messages
	BYTE endpoint;
} TRANSFER_DATA_TOGGLE;

typedef struct : TRANSFER_BASE{	//2k->box DataToggle messages
	ULONG ulTimeoutValue;
} TRANSFER_USB_TIMEOUT;

typedef struct : TRANSFER_BASE{
	BYTE bDeviceCount;
	Topology TopArray[0];
} TRANSFER_DEVICE_STATE;

typedef struct : TRANSFER_BASE{	//box->2k result of OUT or SETUP
	BYTE ret;
} TRANSFER_RETURN;

typedef struct : TRANSFER_BASE{	//2k->box request for IN
	BYTE Port;
	BYTE Slot;
	BYTE interfacenumber;
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
} TRANSFER_REFRESH_CONFIG;

typedef struct : TRANSFER_BASE{
	BYTE Port;
	BYTE Slot;
	BYTE interfacenumber;
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
	TRANSFER_USB_TIMEOUT UsbTimeout;
	TRANSFER_DEVICE_STATE DeviceState;
	char all[1032];
} TRANSFERDATA, *pTRANSFERDATA;

#define RECV_TIMEOUT				10000

#define PID_OUT						0x01
#define PID_IN						0x69
#define PID_SETUP					0x2D
#define PID_DATA0					0xC3
#define PID_DATA1					0x4B

//these aren't PID codes, but they are being used in the same way, in my code, 
//as the 5 codes above. They are just arbitrary numbers...
#define PID_GET_DEVICE_STATE		0xD8
#define PID_SET_USB_TIMEOUT			0xD9
#define PID_RET						0xDA	
#define PID_INIT_IN					0xDB 
#define PID_RESET					0xDC
#define PID_BEGIN_QUEUE				0xDD
#define PID_INIT_END_QUEUE			0xDE
#define PID_RETURN_END_QUEUE		0xDF

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
	_sockdata.size = sizeof(TRANSFER_INIT_END_QUEUE);		\
	_sockdata.type = PID_INIT_END_QUEUE;				

#define CREATE_ININIT(_sockdata, _Port,_Slot, _interfacenumber, _endpoint, _maxsize)	\
	_sockdata.size = sizeof(TRANSFER_INIT_IN);				\
	_sockdata.type = PID_INIT_IN;							\
	_sockdata.initin.Port = _Port;							\
	_sockdata.initin.Slot = _Slot;							\
	_sockdata.initin.interfacenumber = _interfacenumber;	\
	_sockdata.initin.max = _maxsize;						\
	_sockdata.initin.endpoint = _endpoint;

//Macro to fill in a TRANSFER_OUT packet to send from Slix2k to Slixbox
#define CREATE_OUT(_sockdata, _Port,_Slot, _interfacenumber, _endpoint, _pid, _data, _size)	\
	_sockdata.size = sizeof(TRANSFER_OUT) + _size;						\
	_sockdata.type = PID_OUT;											\
	_sockdata.out.Port = _Port;											\
	_sockdata.out.Slot = _Slot;											\
	_sockdata.out.interfacenumber = _interfacenumber;					\
	_sockdata.out.endpoint = _endpoint;									\
	_sockdata.out.dataPid = _pid;										\
	{int _arf; for(_arf = 0; _arf < _size; _arf++) sockdata.out.data[_arf] = _data[_arf];}	

//Macro to fill in a TRANSFER_SETUP packet to send from Slix2k to Slixbox
#define CREATE_SETUP(_sockdata, _Port,_Slot, _interfacenumber, _endpoint, _data, _size)		\
	_sockdata.size = sizeof(TRANSFER_SETUP) + _size;					\
	_sockdata.type = PID_SETUP;											\
	_sockdata.setup.Port = _Port;										\
	_sockdata.setup.Slot = _Slot;										\
	_sockdata.setup.interfacenumber = _interfacenumber;					\
	_sockdata.setup.endpoint = _endpoint;								\
	{int _arf; for(_arf = 0; _arf < _size; _arf++) sockdata.setup.data[_arf] = _data[_arf]; }

//#include "\MyAssert.h"	//definition of MyAssert, & its support features
#define PID_OUT                    0x01
#define PID_IN                     0x69
#define PID_SETUP                  0x2D
#define PID_DATA0			       0xC3
#define PID_DATA1			       0x4B

const BYTE STATUS_Success            = 0x00;
const BYTE STATUS_Ack                = 0x02;
const BYTE STATUS_Nak                = 0x0A;
const BYTE STATUS_Stall              = 0x0E;
const BYTE STATUS_Ignore             = 0x80;
const BYTE STATUS_DataCRCError       = 0x81;
const BYTE STATUS_DataToggleError    = 0x82;
const BYTE STATUS_SyncError          = 0x83;
const BYTE STATUS_BabbleError        = 0x84;
const BYTE STATUS_PIDError           = 0x85;
const BYTE STATUS_ShortPacketError   = 0x86;
const BYTE STATUS_ConfigurationError = 0x87;
const BYTE STATUS_TransmissionError	 = 0x89;

#define MAX_PORTS 8					    //max num of ports on the xbox (currently 5)
#define MAX_SLOTS 4						//max num of slots on a duke, plus 1 (currently 3)
#define MAX_INTERFACES 4				// max num of interfaces (currently 2 on Hawk)
#define XPortNumber		53331
#define XBoxEventsPortNumber 53335


class CXBoxTopology
{
public:
	BYTE m_bPort;
	BYTE m_bSlot;
	BYTE m_bInterfaceNumber;
};

// added support for event handling separate from main packet processing
class CEventPacket
{
public:
	int		m_iEventCode;  
	CXBoxTopology m_Topology;
	ULONG	m_ulErrorCode;
};

typedef enum {
	eInsertionEvent = 3,
	eRemovalEvent
} EVENTTYPES;



#endif