#ifndef _SERIAL_H_
#define _SERIAL_H_

#define DBG_COM1_PORT (0x3F8)
#define DBG_COM2_PORT (0x2F8)

#define DATA_OFFSET (0)
#define DLOW_OFFSET (0)
#define ICR_OFFSET (1)
#define DHI_OFFSET (1)
#define IIR_OFFSET (2)
#define FIFO_OFFSET (2)
#define LCR_OFFSET (3)
#define MCR_OFFSET (4)
#define LSR_OFFSET (5)
#define MRR_OFFSET (6)
#define SPARE_OFFSET (7)

#ifndef PSTR
typedef char * PSTR;
#endif

NvU16 dbgBaudRate(NvU32 uBaudRate);
VOID dbgInitSerial(PDBGPORT pDbgPort);
VOID dbgDisplaySerial(PSTR pStr);


#define DBG_FFLAGS_BINARY (0x00000001)
#define DBG_FFLAGS_PARITY (0x00000002)
#define DBG_FFLAGS_CTSFLOW (0x00000004)
#define DBG_FFLAGS_DSRFLOW (0x00000008)
#define DBG_FFLAGS_DTRCONTROL (0x00000030)
#define DBG_FFLAGS_TXONOFF (0x00000040)
#define DBG_FFLAGS_OUTX (0x00000080)
#define DBG_FFLAGS_INX (0x00000100)
#define DBG_FFLAGS_ERRORCHAR (0x00000200)
#define DBG_FFLAGS_FNULL (0x00000400)
#define DBG_FFLAGS_RTS (0x00001800)
#define DBG_FFLAGS_ABORT (0x00020000)
#define DBG_FFLAGS_DUMMY2 (0xFFFC0000)

#define FAR far

typedef struct _WIN32DCB {
    DWORD DCBlength;      /* sizeof(DCB)                     */
    DWORD BaudRate;       /* Baudrate at which running       */
    DWORD fFlags;         /* All the stuff blow is one dword */ 
#if 0
    DWORD fBinary: 1;     /* Binary Mode (skip EOF check)    */
    DWORD fParity: 1;     /* Enable parity checking          */
    DWORD fOutxCtsFlow:1; /* CTS handshaking on output       */
    DWORD fOutxDsrFlow:1; /* DSR handshaking on output       */
    DWORD fDtrControl:2;  /* DTR Flow control                */
    DWORD fDsrSensitivity:1; /* DSR Sensitivity              */
    DWORD fTXContinueOnXoff: 1; /* Continue TX when Xoff sent */
    DWORD fOutX: 1;       /* Enable output X-ON/X-OFF        */
    DWORD fInX: 1;        /* Enable input X-ON/X-OFF         */
    DWORD fErrorChar: 1;  /* Enable Err Replacement          */
    DWORD fNull: 1;       /* Enable Null stripping           */
    DWORD fRtsControl:2;  /* Rts Flow control                */
    DWORD fAbortOnError:1; /* Abort all reads and writes on Error */
    DWORD fDummy2:17;     /* Reserved                        */
#endif
    WORD wReserved;       /* Not currently used              */
    WORD XonLim;          /* Transmit X-ON threshold         */
    WORD XoffLim;         /* Transmit X-OFF threshold        */
    BYTE ByteSize;        /* Number of bits/byte, 4-8        */
    BYTE Parity;          /* 0-4=None,Odd,Even,Mark,Space    */
    BYTE StopBits;        /* 0,1,2 = 1, 1.5, 2               */
    char XonChar;         /* Tx and Rx X-ON character        */
    char XoffChar;        /* Tx and Rx X-OFF character       */
    char ErrorChar;       /* Error replacement char          */
    char EofChar;         /* End of Input character          */
    char EvtChar;         /* Recieved Event character        */
    char xx1;
    char xx2;
} WIN32DCB, * PWIN32DCB;

#endif
