/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/*;***************************************************************************
;
; VCOMM.H
;
; Version:	1.00
;
;============================================================================*/

/* H2INCSWITCHES -d */

#ifndef _VCOMM_H
#define _VCOMM_H

/*XLATOFF*/
#pragma	pack(1)
/*XLATON*/

#ifndef Not_VxD

/*XLATOFF*/
#define	VCOMM_Service	Declare_Service
/*XLATON*/

/*MACROS*/
Begin_Service_Table(VCOMM, VxD)

VCOMM_Service	(VCOMM_Get_Version, LOCAL)
VCOMM_Service	(_VCOMM_Register_Port_Driver,LOCAL)
VCOMM_Service	(_VCOMM_Acquire_Port,LOCAL)
VCOMM_Service	(_VCOMM_Release_Port,LOCAL)
VCOMM_Service	(_VCOMM_OpenComm, VxD_CODE)
VCOMM_Service	(_VCOMM_SetCommState, VxD_CODE)
VCOMM_Service	(_VCOMM_GetCommState, VxD_CODE)
VCOMM_Service	(_VCOMM_SetupComm, VxD_CODE)
VCOMM_Service	(_VCOMM_TransmitCommChar, VxD_CODE)
VCOMM_Service	(_VCOMM_CloseComm, VxD_CODE)
VCOMM_Service	(_VCOMM_GetCommQueueStatus, VxD_CODE)
VCOMM_Service	(_VCOMM_ClearCommError, VxD_CODE)
VCOMM_Service	(_VCOMM_GetModemStatus, VxD_CODE)
VCOMM_Service	(_VCOMM_GetCommProperties, VxD_CODE)
VCOMM_Service	(_VCOMM_EscapeCommFunction, VxD_CODE)
VCOMM_Service	(_VCOMM_PurgeComm, VxD_CODE)
VCOMM_Service	(_VCOMM_SetCommEventMask, VxD_CODE)
VCOMM_Service	(_VCOMM_GetCommEventMask, VxD_CODE)
VCOMM_Service	(_VCOMM_WriteComm, VxD_CODE)
VCOMM_Service	(_VCOMM_ReadComm, VxD_CODE)
VCOMM_Service	(_VCOMM_EnableCommNotification, VxD_CODE)
VCOMM_Service	(_VCOMM_GetLastError, VxD_CODE)

VCOMM_Service	(_VCOMM_Steal_Port,LOCAL)

VCOMM_Service	(_VCOMM_SetReadCallBack, VxD_CODE)
VCOMM_Service	(_VCOMM_SetWriteCallBack, VxD_CODE)

VCOMM_Service	(_VCOMM_Add_Port, LOCAL)

/*ENDMACROS*/
#ifndef		WIN31COMPAT
/*MACROS*/

VCOMM_Service	(_VCOMM_GetSetCommTimeouts, VxD_CODE)
VCOMM_Service	(_VCOMM_SetWriteRequest, VxD_CODE)
VCOMM_Service	(_VCOMM_SetReadRequest, VxD_CODE)
VCOMM_Service	(_VCOMM_Dequeue_Request, VxD_CODE)

VCOMM_Service	(_VCOMM_Enumerate_DevNodes, VxD_CODE)

VCOMM_Service	(VCOMM_Map_Win32DCB_To_Ring0, VxD_CODE)
VCOMM_Service	(VCOMM_Map_Ring0DCB_To_Win32, VxD_CODE)

VCOMM_Service	(_VCOMM_Get_Contention_Handler, VxD_CODE)
VCOMM_Service	(_VCOMM_Map_Name_To_Resource, VxD_CODE)
/*ENDMACROS*/
#endif
/*MACROS*/

End_Service_Table(VCOMM, VxD)
/*ENDMACROS*/

#endif

typedef struct _PortFunctions {
	BOOL (*pPortSetCommState)();	/* ptr to PortSetState */
	BOOL (*pPortGetCommState)();	/* ptr to PortGetState */
	BOOL (*pPortSetup)();		/* ptr to PortSetup */
	BOOL (*pPortTransmitChar)();	/* ptr to PortTransmitChar */
	BOOL (*pPortClose)();		/* ptr to PortClose */
	BOOL (*pPortGetQueueStatus)();	/* ptr to PortGetQueueStatus */
	BOOL (*pPortClearError)();	/* ptr to PortClearError */
	BOOL (*pPortSetModemStatusShadow)(); /* ptr to
					        PortSetModemStatusShadow */
	BOOL (*pPortGetProperties)();	/* ptr to PortGetProperties */
	BOOL (*pPortEscapeFunction)();	/* ptr to PortEscapeFunction */
	BOOL (*pPortPurge)();		/* ptr to PortPurge */
	BOOL (*pPortSetEventMask)();	/* ptr to PortSetEventMask */
	BOOL (*pPortGetEventMask)();	/* ptr to PortGetEventMask */
	BOOL (*pPortWrite)();		/* ptr to PortWrite */
	BOOL (*pPortRead)();		/* ptr to PortRead */
	BOOL (*pPortEnableNotification)(); /* ptr to PortEnableNotification */
	BOOL (*pPortSetReadCallBack)(); /* ptr to PortSetReadCallBack */
	BOOL (*pPortSetWriteCallBack)(); /* ptr to PortSetWriteCallBack */
	BOOL (*pPortGetModemStatus)();	/* ptr to PortGetModemStatus */

#ifndef	WIN31COMPAT
	
	BOOL (*pPortGetCommConfig)();	/* ptr to PortGetCommConfig */
	BOOL (*pPortSetCommConfig)();	/* ptr to PortSetCommConfig */
	BOOL (*pPortGetError)();	/* ptr to PortGetError (win32 style)*/
	BOOL (*pPortDeviceIOCtl)();	/* ptr to PortDeviceIOCtl */
		
#endif
	
} PortFunctions;

typedef struct COMMTIMEOUTS {
	DWORD ReadIntervalTimeout;
	DWORD ReadTotalTimeoutMultiplier;
	DWORD ReadTotalTimeoutConstant;
	DWORD WriteTotalTimeoutMultiplier;
	DWORD WriteTotalTimeoutConstant;
} COMMTIMEOUTS, *LPCOMMTIMEOUTS;

typedef struct _PortData {
	WORD PDLength;			/* sizeof (PortData) */
        WORD PDVersion;			/* version of struct */
        PortFunctions *PDfunctions;	/* Points to a list of functions
					   supported by	the port driver */
        DWORD PDNumFunctions;		/* Highest ordinal of supported
					   function */
        DWORD dwLastError;		/* what was the error
					   for the last operation */
        DWORD dwClientEventMask;	/* event mask set by client */
        DWORD lpClientEventNotify;	/* address set by client for
					   event notification */
	DWORD lpClientReadNotify;	/* adress set by client for
					   read threshold notification */
	DWORD lpClientWriteNotify;	/* address set by client for
					   write threshold notification */
	DWORD dwClientRefData;		/* Client's reference data */
	DWORD dwWin31Req;		/* Used for WIN3.1 specific reasons */
	DWORD dwClientEvent;		/* Event to send to client */
	DWORD dwCallerVMId;		/* Used for supporting all VMs */

	DWORD dwDetectedEvents;		/* mask of detected and
					   enabled events */
        DWORD dwCommError;		/* non-zero if I/O error. */
	BYTE bMSRShadow;		/* the shadow of Modem Status
					   Register */
	WORD wFlags;			/* flags for the port */
	BYTE LossByte;			/* For COMM ports 1-4 VCD flags this
					   byte telling port driver that it
					   has lost the port */

	DWORD QInAddr;			/* Address of the queue */
	DWORD QInSize;			/* Length of queue in bytes */
	DWORD QOutAddr;			/* Address of the queue */
	DWORD QOutSize;			/* Length of queue in bytes */
	DWORD QInCount;			/* # of bytes currently in queue */
	DWORD QInGet;			/* Offset into q to get bytes from */
	DWORD QInPut;			/* Offset into q to put bytes in */
	DWORD QOutCount;		/* Number of bytes currently in q */
	DWORD QOutGet;			/* Offset into q to get bytes from */
	DWORD QOutPut;			/* Offset into q to put bytes in */

	DWORD ValidPortData;		/* For checking validity etc. */

	DWORD lpLoadHandle;		/* load handle of the owner VxD */
	COMMTIMEOUTS cmto;		/* Commtimeouts struct */

	DWORD lpReadRequestQueue;	/* Pointer to pending Read requests */
	DWORD lpWriteRequestQueue;	/* Pointer to pending Write requests */

	DWORD dwLastReceiveTime;	/* Time of last reception of data */
		
	DWORD dwReserved1;		/* Reserved */
	DWORD dwReserved2;		/* Reserved */
} PortData;

#ifndef	Not_VxD

/* Standard error codes set in dwLastError. */

#define	IE_BADID	-1	 /* invalid or unsupported device */
#define	IE_OPEN		-2	 /* Device already open */
#define	IE_NOPEN	-3	 /* Device not open */
#define	IE_MEMORY	-4	 /* unable to allocate queues */
#define	IE_DEFAULT	-5	 /* error in default params */
#define	IE_INVALIDSERVICE -6	 /* port driver doesn't support this service*/
#define	IE_HARDWARE	-10	 /* hardware not present */
#define	IE_BYTESIZE	-11	 /* illegal byte size */
#define	IE_BAUDRATE	-12	 /* unsupported baud rate */
#define	IE_EXTINVALID	-20	 /* unsupported extended function */
#define	IE_INVALIDPARAM	-21	 /* Parameters are wrong */
#define	IE_TRANSMITCHARFAILED 0x4000 /* TransmitChar failed */

/* Events that can be set in dwClientEventMask */

#define	EV_RXCHAR	0x00000001	 /* Any Character received */
#define	EV_RXFLAG	0x00000002	 /* Received certain character */
#define	EV_TXEMPTY	0x00000004	 /* Transmitt Queue Empty */
#define	EV_CTS		0x00000008	 /* CTS changed state */
#define	EV_DSR		0x00000010	 /* DSR changed state */
#define	EV_RLSD		0x00000020	 /* RLSD changed state */
#define	EV_BREAK	0x00000040	 /* BREAK received */
#define	EV_ERR		0x00000080	 /* Line status error occurred */
#define	EV_RING		0x00000100	 /* Ring signal detected */
#define	EV_PERR		0x00000200	 /* Printer error occured */
#define	EV_CTSS		0x00000400	 /* CTS state */
#define	EV_DSRS		0x00000800	 /* DSR state */
#define	EV_RLSDS	0x00001000	 /* RLSD state */
#define	EV_RingTe	0x00002000	 /* Ring Trailing Edge Indicator */
#define	EV_TXCHAR	0x00004000	 /* Any character transmitted */
#define	EV_DRIVER	0x00008000	 /* Driver specific event */
#define	EV_UNAVAIL	0x00010000	 /* Acquired port has been stolen */
#define	EV_AVAIL	0x00020000	 /* stolen port has been released */


/* Error Flags for dwCommError */

#define	CE_RXOVER	0x00000001		/* Receive Queue overflow */
#define	CE_OVERRUN	0x00000002		/* Receive Overrun Error */
#define	CE_RXPARITY	0x00000004		/* Receive Parity Error */
#define	CE_FRAME	0x00000008		/* Receive Framing error */
#define	CE_BREAK	0x00000010		/* Break Detected */
#define	CE_CTSTO	0x00000020		/* CTS Timeout */
#define	CE_DSRTO	0x00000040		/* DSR Timeout */
#define	CE_RLSDTO	0x00000080		/* RLSD Timeout */
#define	CE_TXFULL	0x00000100		/* TX Queue is full */
#define	CE_PTO		0x00000200		/* LPTx Timeout */
#define	CE_IOE		0x00000400		/* LPTx I/O Error */
#define	CE_DNS		0x00000800		/* LPTx Device not selected */
#define	CE_OOP		0x00001000		/* LPTx Out-Of-Paper */
#define	CE_Unused1	0x00002000		/* unused */
#define	CE_Unused2	0x00004000		/* unused */
#define	CE_MODE		0x00008000		/* Requested mode unsupported*/

#endif

//
// Modem status flags
//

#define	MS_CTS_ON	0x0010
#define	MS_DSR_ON	0x0020
#define	MS_RING_ON	0x0040
#define	MS_RLSD_ON	0x0080
#define	MS_Modem_Status	0x00F0


/* flags for wFlags of PortData */

#define	Event_Sent	0x0001	/* has an event been set for COMM.DRV ? */
#define	Event_Sent_Bit	0x0000	/* bit in the flag word */
#define	TimeOut_Error	0x0002	/* Operation was abandoned due to timeout */
#define	TimeOut_Error_Bit 0x0001 /* its bit field */
#define	Flush_In_Progress 0x0004 /* FlushFileBuffers is in progress */
#define	Flush_In_Progress_Bit 0x0002 /* its bit */
#define	TxQueuesSet	0x0008	/* Non-zero xmit queue exists */
#define	TxQueuesSetBit	0x0003	/* its bit */
#define	CloseComm_In_Progress	0x0010	/* CloseComm is in progress */
#define	CloseComm_In_Progress_Bit 0x0004 /* its bit */
#define Spec_Timeouts_Set	0x0020	/* Special timeouts have been set */
#define	Spec_Timeouts_Set_Bit	0x0005	/* its bit */

typedef struct _DCB {
	DWORD DCBLength;		/* sizeof (DCB) */
	DWORD BaudRate ;		/* Baudrate at which running */
        DWORD BitMask;			/* flag DWORD */
        DWORD XonLim;			/* Transmit X-ON threshold */
        DWORD XoffLim;			/* Transmit X-OFF threshold */
        WORD wReserved;			/* reserved */
        BYTE ByteSize;			/* Number of bits/byte, 4-8 */
	BYTE Parity;			/* 0-4=None,Odd,Even,Mark,Space */
	BYTE StopBits;			/* 0,1,2 = 1, 1.5, 2 */
	char XonChar;			/* Tx and Rx X-ON character */
	char XoffChar;			/* Tx and Rx X-OFF character */
	char ErrorChar;			/* Parity error replacement char */
	char EofChar;			/* End of Input character */
	char EvtChar1;			/* special event character */
	char EvtChar2;			/* Another special event character */
	BYTE bReserved;			/* reserved */
	DWORD RlsTimeout;		/* Timeout for RLSD to be set */
	DWORD CtsTimeout;		/* Timeout for CTS to be set */
	DWORD DsrTimeout;		/* Timeout for DSR to be set */
	DWORD TxDelay;			/* Amount of time between chars */
} _DCB;

#ifndef	Not_VxD

/*  Comm Baud Rate indices : allowable values for BaudRate */

#define	CBR_110		0x0000FF10
#define	CBR_300		0x0000FF11
#define	CBR_600		0x0000FF12
#define	CBR_1200	0x0000FF13
#define	CBR_2400	0x0000FF14
#define	CBR_4800	0x0000FF15
#define	CBR_9600	0x0000FF16
#define	CBR_14400	0x0000FF17
#define	CBR_19200	0x0000FF18

/* 0x0000FF19,0x00000FF1A are reserved */

#define	CBR_38400	0x0000FF1B

/* 0x0000FF1C, 0x0000FF1D, 0x0000FF1E are reserved */

#define	CBR_56000	0x0000FF1F

/* 0x0000FF20, 0x0000FF21, 0x0000FF22 are reserved */

#define	CBR_128000	0x0000FF23

/* 0x0000FF24, 0x0000FF25, 0x0000FF26 are reserved */

#define	CBR_256000	0x0000FF27

#endif

/* Flags for Bitmask */

#define	fBinary		0x00000001	/* Binary mode */
#define	fRTSDisable	0x00000002	/* Disable RTS */
#define fParity		0x00000004	/* Perform parity checking */
#define	fOutXCTSFlow	0x00000008	/* Output handshaking using CTS */
#define	fOutXDSRFlow	0x00000010	/* Output handshaking using DSR */
#define	fEnqAck		0x00000020	/* ENQ/ACK software handshaking */
#define	fEtxAck		0x00000040	/* ETX/ACK software handshaking */
#define	fDTRDisable	0x00000080	/* Disable DTR */

#define	fOutX		0x00000100	/* Output X-ON/X-OFF */
#define	fInX		0x00000200	/* Input X-ON/X-OFF */
#define	fPErrChar	0x00000400	/* Parity error replacement active */
#define	fNullStrip	0x00000800	/* Null stripping */
#define	fCharEvent	0x00001000	/* Character event */
#define	fDTRFlow	0x00002000	/* Input handshaking using DTR */
#define	fRTSFlow	0x00004000	/* Output handshaking using RTS */
#define	fWin30Compat	0x00008000	/* Maintain Compatiblity */

#define	fDsrSensitivity	0x00010000	/* DSR sensitivity */
#define	fTxContinueOnXoff 0x00020000	/* Continue Tx when Xoff sent */
#define	fDtrEnable	0x00040000	/* Enable DTR on device open */
#define	fAbortOnError	0x00080000	/*abort all reads and writes on error*/
#define	fRTSEnable	0x00100000	/* enable RTS on device open */
#define	fRTSToggle	0x00200000	/* iff bytes in Q, set RTS high */

/* Allowable parity values */

#define	NOPARITY	0
#define	ODDPARITY	1
#define	EVENPARITY	2
#define	MARKPARITY	3
#define	SPACEPARITY	4

/* Allowable stopbits */

#define	ONESTOPBIT	0
#define	ONE5STOPBITS	1
#define	TWOSTOPBITS	2

typedef	unsigned short WCHAR;

typedef struct _COMMPROP {
	WORD   wPacketLength;		/* length of property structure
					   in bytes */
	WORD   wPacketVersion;		/* version of this structure */
	DWORD  dwServiceMask;		/* Bitmask indicating services
					   provided */
	DWORD  dwReserved1;		/* reserved */
	DWORD  dwMaxTxQueue;		/* Max transmit queue size.
					   0 => not used */
	DWORD  dwMaxRxQueue;		/* Max receive queue size.
					   0 => not used */
	DWORD  dwMaxBaud;		/* maximum baud supported */
	DWORD  dwProvSubType;		/* specific COMM provider type */
	DWORD  dwProvCapabilities;	/* flow control capabilities */
	DWORD  dwSettableParams;	/* Bitmask indicating params
					   that can be set. */
	DWORD  dwSettableBaud;		/* Bitmask indicating baud rates
					   that can be set*/
	WORD   wSettableData;		/* bitmask indicating # of data bits
					   that can be set*/
	WORD   wSettableStopParity;	/* bitmask indicating allowed
					   stopbits and parity checking */
	DWORD  dwCurrentTxQueue;	/* Current size of transmit queue
					   0 => unavailable */
	DWORD  dwCurrentRxQueue;	/* Current size of receive queue
					   0 => unavailable */
	DWORD  dwProvSpec1;		/* Used iff clients have
					   intimate knowledge of format */
	DWORD  dwProvSpec2;		/* Used iff clients have intimate
					   knowledge of format */
	WCHAR  wcProvChar[1];		/* Used iff clients have intimate \						   knowledge of format */
	WORD	filler;			/* To make it multiple of 4 */
	
} _COMMPROP;

//
// Serial provider type.
//

#define SP_SERIALCOMM    ((DWORD)0x00000001)

//
// Provider SubTypes
//

#define PST_UNSPECIFIED      ((DWORD)0x00000000)
#define PST_RS232            ((DWORD)0x00000001)
#define PST_PARALLELPORT     ((DWORD)0x00000002)
#define PST_RS422            ((DWORD)0x00000003)
#define PST_RS423            ((DWORD)0x00000004)
#define PST_RS449            ((DWORD)0x00000005)
#define PST_MODEM            ((DWORD)0x00000006)
#define PST_FAX              ((DWORD)0x00000021)
#define PST_SCANNER          ((DWORD)0x00000022)
#define PST_NETWORK_BRIDGE   ((DWORD)0x00000100)
#define PST_LAT              ((DWORD)0x00000101)
#define PST_TCPIP_TELNET     ((DWORD)0x00000102)
#define PST_X25              ((DWORD)0x00000103)

//
// Provider capabilities flags.
//

#define PCF_DTRDSR        ((DWORD)0x0001)
#define PCF_RTSCTS        ((DWORD)0x0002)
#define PCF_RLSD          ((DWORD)0x0004)
#define PCF_PARITY_CHECK  ((DWORD)0x0008)
#define PCF_XONXOFF       ((DWORD)0x0010)
#define PCF_SETXCHAR      ((DWORD)0x0020)
#define PCF_TOTALTIMEOUTS ((DWORD)0x0040)
#define PCF_INTTIMEOUTS   ((DWORD)0x0080)
#define PCF_SPECIALCHARS  ((DWORD)0x0100)
#define PCF_16BITMODE     ((DWORD)0x0200)

//
// Comm provider settable parameters.
//

#define SP_PARITY         ((DWORD)0x0001)
#define SP_BAUD           ((DWORD)0x0002)
#define SP_DATABITS       ((DWORD)0x0004)
#define SP_STOPBITS       ((DWORD)0x0008)
#define SP_HANDSHAKING    ((DWORD)0x0010)
#define SP_PARITY_CHECK   ((DWORD)0x0020)
#define SP_RLSD           ((DWORD)0x0040)

//
// Settable baud rates in the provider.
//

#define BAUD_075          ((DWORD)0x00000001)
#define BAUD_110          ((DWORD)0x00000002)
#define BAUD_134_5        ((DWORD)0x00000004)
#define BAUD_150          ((DWORD)0x00000008)
#define BAUD_300          ((DWORD)0x00000010)
#define BAUD_600          ((DWORD)0x00000020)
#define BAUD_1200         ((DWORD)0x00000040)
#define BAUD_1800         ((DWORD)0x00000080)
#define BAUD_2400         ((DWORD)0x00000100)
#define BAUD_4800         ((DWORD)0x00000200)
#define BAUD_7200         ((DWORD)0x00000400)
#define BAUD_9600         ((DWORD)0x00000800)
#define BAUD_14400        ((DWORD)0x00001000)
#define BAUD_19200        ((DWORD)0x00002000)
#define BAUD_38400        ((DWORD)0x00004000)
#define BAUD_56K          ((DWORD)0x00008000)
#define BAUD_128K         ((DWORD)0x00010000)
#define BAUD_USER         ((DWORD)0x10000000)

//
// Settable Data Bits
//

#define DATABITS_5        ((WORD)0x0001)
#define DATABITS_6        ((WORD)0x0002)
#define DATABITS_7        ((WORD)0x0004)
#define DATABITS_8        ((WORD)0x0008)
#define DATABITS_16       ((WORD)0x0010)
#define DATABITS_16X      ((WORD)0x0020)

//
// Settable Stop and Parity bits.
//

#define STOPBITS_10       ((WORD)0x0001)
#define STOPBITS_15       ((WORD)0x0002)
#define STOPBITS_20       ((WORD)0x0004)
#define PARITY_NONE       ((WORD)0x0100)
#define PARITY_ODD        ((WORD)0x0200)
#define PARITY_EVEN       ((WORD)0x0400)
#define PARITY_MARK       ((WORD)0x0800)
#define PARITY_SPACE      ((WORD)0x1000)

typedef struct  _COMSTAT {
	DWORD BitMask;		/* flags DWORD */
	DWORD cbInque;		/* Count of characters in receive queue */
	DWORD cbOutque;		/* Count of characters in transmit queue */
} _COMSTAT;

/* Flags of BitMask */

#define	fCtsHold	0x00000001	/* Transmit is on CTS hold */
#define	fDsrHold	0x00000002	/* Transmit is on DSR hold */
#define	fRlsdHold	0x00000004	/* Transmit is on RLSD hold */
#define	fXoffHold	0x00000008	/* Received handshake */
#define	fXoffSent	0x00000010	/* Issued handshake */
#define	fEof		0x00000020	/* EOF character found */
#define	fTximmed	0x00000040	/* Character being transmitted */

typedef struct _QSB {
	DWORD ReceiveQueueGetPtr; /* offset into queue to get bytes from */
	DWORD ReceiveQueueCount;  /* Number of bytes in receive queue */
} _QSB;


/* Following constants are used by COMM.DRV to access the PM apis supported
   by VCOMM.
*/

#define	VCOMM_PM_API_OpenCom	0
#define	VCOMM_PM_API_SetCom	1
#define	VCOMM_PM_API_Setup	2
#define	VCOMM_PM_API_ctx	3
#define	VCOMM_PM_API_TrmCom	4
#define	VCOMM_PM_API_StaCom	5
#define	VCOMM_PM_API_cextnfcn	6
#define	VCOMM_PM_API_cFlush	7
#define	VCOMM_PM_API_cevt	8
#define	VCOMM_PM_API_cevtget	9

#define	VCOMM_PM_API_SetMSRShadow	10
#define	VCOMM_PM_API_WriteString	11
#define	VCOMM_PM_API_ReadString		12
#define	VCOMM_PM_API_EnableNotify	13

#ifndef	Not_VxD

/* Escape Functions (extended functions). */

#define	Dummy	0		  /* Dummy */
#define	SETXOFF	1		  /* Simulate XOFF received */
#define	SETXON  2		  /* Simulate XON received */
#define	SETRTS	3		  /* Set RTS high */
#define	CLRRTS	4		  /* Set RTS low */
#define	SETDTR	5		  /* Set DTR high */
#define	CLRDTR	6		  /* Set DTR low */
#define	RESETDEV 7		  /* Reset device if possible */

#endif

#ifdef	Not_VxD

/* These numbers are reserved for compatibility reasons */

#define	GETLPTMAX 8		 /* Get maximum LPT supported */
#define	GETCOMMAX 9		 /* Get maximum COM supported */

#endif

#define	GETCOMBASEIRQ	10	 /* Get COM base and IRQ */
#define	GETCOMBASEIRQ1	11	 /* FOR COMPATIBILITY REASONS */

#define	SETBREAK	12	 /* Set break condition */
#define	CLEARBREAK	13	 /* Clear break condition */

#ifdef	Not_VxD

/* These too are not available to client VxDs and not implemented by
   port drivers. */

#define	GETPORTHANDLE	14	 /* Get handle for Port */
#define	GETEXTENDEDFNADDR 15	 /* Get the address of extended functions */

#endif

#define	CLRTIMERLOGIC	16	/* Clear the timer logic of the port driver */

#define GETDEVICEID	17	/* Get the device ID of the device */
#define SETECPADDRESS	18	/* Set ECP channel address */

#define SETUPDATETIMEADDR	19	/* Set address of update field for
					   last time a char was received */

#define IGNOREERRORONREADS	20	/* Ignore pending IO errors on reads*/

#define ENABLETIMERLOGIC	21	/* Re-enable timer logic code */
#define	IGNORESELECT		22	/* Ignore select bit */

#define STARTNONSTDESCAPES	200	/* non standard escapes */

#define	PEEKCHAR		200	/* peek the Rx Q */

/* END OF ESCAPES for ESCAPECOMMFUNCTION */

#ifndef	Not_VxD
/* notifications passed in Event of Notification function */

#define	CN_RECEIVE	1	 /* bytes are available in the input queue */
#define	CN_TRANSMIT	2	 /* fewer than wOutTrigger bytes still */
				 /* remain in the output queue waiting */
				 /* to be transmitted. */
#define	CN_EVENT	4	 /* an enabled event has occurred */

#endif
				
/* Other constants */

#ifndef	Not_VxD
				
#define	IGNORE		0		/* Ignore signal */

#endif
				
#define	INFINITE32	-1		/* Infinite timeout */

/* Defined ways of supporting a port (return value of PortInquire fn) */

#define	PD_UNSUPPORTED	0		/* Port is not supported */
#define	PD_DEFAULT	1		/* Port is supported by default */
#define	PD_BEST		-1		/* Best possible support */

/* Masks for relevant fields of DCB */
#define	fBaudRate	0x00000001
#define	fBitMask	0x00000002
#define	fXonLim		0x00000004
#define	fXoffLim	0x00000008
#define	fByteSize	0x00000010
#define	fbParity	0x00000020
#define	fStopBits	0x00000040
#define	fXonChar	0x00000080
#define	fXoffChar	0x00000100
#define	fErrorChar	0x00000200
#define	fEofChar	0x00000400
#define	fEvtChar1	0x00000800
#define	fEvtChar2	0x00001000
#define	fRlsTimeout	0x00002000
#define	fCtsTimeout	0x00004000
#define	fDsrTimeout	0x00008000
#define	fTxDelay	0x00010000
#define	fTimeout	(fRlsTimeout | fDsrTimeout | fCtsTimeout)
#define	fLCR		0x00000070

/*

 IORequest

 VCOMM allows a client to impose a "block" structure on a port
 driver in the following way. It can call VCOMM_SetWriteRequest
 with the buffer to write. When all the earlier write requests are satisfied
 by the port driver, VCOMM will call the port driver to satisfy this
 request. A client can also call VCOMM_SetReadRequest with the buffer
 to read data into. VCOMM will queue up this request and when all previous
 read requests are satisfied, call the port driver to satisfy this request.
 So now, clients can get a notification per read/write request. The buffers
 passed by client must be in locked memory during the duration of the whole
 operation and should not be touched by the client till it receives the
 notification.

 A client should not use a combination of VCOMM_WriteComm and
 VCOMM_SetWriteRequest. This will lead to strange results. VCOMM_WriteComm
 goes directly to the port driver without any processing by VCOMM. There
 is considerable work done by VCOMM for VCOMM_SetWriteRequest. Similarly,
 VCOMM_SetReadRequest and VCOMM_ReadComm should not be used together.

 When a client attempts to get the total queued count of characters to write,
 VCOMM calls the port driver to find the size of its xmit queue and then
 adds up all the pending write operations to return the count.

 VCOMM_PurgeComm leads to VCOMM calling each aborted IORequest if TXAbort
 parameter is set. For TxClear, it does not do so, it just calls down into
 the port driver to clear its transmit queue.
 
*/

typedef struct IORequest {

	DWORD IOBuffer;			//  address of IO buffer
	DWORD IOCount;			// the size of IO buffer
	DWORD IOCallCompletionRoutine;	// whom to call back when done
	DWORD NumBytes;			// How many bytes were written/read
	DWORD dwReserved[8];		// eight reserved words used by VCOMM
	DWORD dwSig;			// signature dword
	
} IORequest;

#define	WRITE_REQUEST	1
#define	READ_REQUEST	0


/* For supporting Plug and Play, a port driver must be dynamically
   loadable. It must exist in the system directory (where DOS386.exe
   is executed from).
   It will be loaded by VCOMM which will be its devloader. During
   Sys_Dynamic_Device_Init, it should register with VCOMM via
   _VCOMM_Register_Port_Driver. While registering, it should pass
   the address of its initialization function. VCOMM will call this
   function to initialize a port supported by the port driver. The
   port driver should check if such a port exists and call VCOMM to
   add this port to its list via _VCOMM_Add_Port. It MUST add the
   port in its initialization function before returning to VCOMM.
   When all the ports supported by the port driver are closed, VCOMM
   will unload the driver. It will call the initialization function of
   the driver once per open of each port. So the port driver can allocate
   the required data for the port at this time and deallocate it at
   the time of closing of the port.
   
   The procedure must be of the form:
   
   void InitFunction(DWORD hDevNode, DWORD AllocPortBase, DWORD AllocPortIrq,
		      char *PortName);

   where:
   hDevNode = devnode for the port
   AllocPortBase = allocated base port address of the port being initialized.
   AllocPortIrq = allocated base Irq of the port being initialized.
   PortName -> name of the port. The port driver, if it supports the port
	   should add the port by using a copy of this name.
	   It will be later be called to open the port with this name.
   
   */

/*

The DriverControl function of the port drivers is called by VCOMM to
perform various actions such as initialize all or one port etc.
Following function codes specify various actions.

*/

#define	DC_Initialize	0x00000000		/* initialize port. */

/* ASM

IFDEF	WIN31COMPAT

VxD_My_Pageable_Code_Seg MACRO
VxD_Locked_Code_Seg
ENDM

VxD_My_Pageable_Code_Ends MACRO
VxD_Locked_Code_Ends
ENDM

VxD_My_Pageable_Data_Seg MACRO
VxD_Locked_Data_Seg
ENDM

VxD_My_Pageable_Data_Ends MACRO
VxD_Locked_Data_Ends
ENDM


ELSE

VxD_My_Pageable_Code_Seg MACRO
VxD_Pageable_Code_Seg
ENDM

VxD_My_Pageable_Code_Ends MACRO
VxD_Pageable_Code_Ends
ENDM

VxD_My_Pageable_Data_Seg MACRO
VxD_Pageable_Data_Seg
ENDM

VxD_My_Pageable_Data_Ends MACRO
VxD_Pageable_Data_Ends
ENDM

ENDIF

*/

#define	ERROR_NOT_READY	21L
#define	ERROR_OUT_OF_PAPER	28L
#define	ERROR_COUNTER_TIMEOUT	1121L

/*XLATOFF*/
#pragma	pack()
/*XLATON*/

#endif                  // _VCOMM_H

typedef	struct IOCtlBlock {
	DWORD IO_lpOverlapped;
	DWORD IO_lpcbBytesReturned;
	DWORD IO_Event;
	DWORD IO_ThreadHandle;
	DWORD IO_NumBytesReturned;
} IOCtlBlock;

/*
 * Contention related equates
 */

#define	MAP_DEVICE_TO_RESOURCE	0
#define	ACQUIRE_RESOURCE	1
#define	STEAL_RESOURCE		2
#define	RELEASE_RESOURCE	3
#define	ADD_RESOURCE		4
#define	REMOVE_RESOURCE		5

#define	MAX_CONTEND_FUNCTIONS	5

