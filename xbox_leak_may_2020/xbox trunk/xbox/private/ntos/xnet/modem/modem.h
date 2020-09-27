#ifndef __MODEM_H__
#define __MODEM_H__

// defines
#define CONFIG_DESC_SIZE 80

#define MODEM_ENTER_CRIT() \
    gIrql = KeRaiseIrqlToDpcLevel();

#define MODEM_LEAVE_CRIT() \
    KeLowerIrql(gIrql);
    
// data classes
typedef struct _COMM_DEVICE_INFO {

    IUsbDevice *pDevice;

    UCHAR bCommInterface;

    UCHAR bBulkInEndpointAddress;
    USHORT wBulkInMaxPacket;
    PVOID pBulkInEndpointHandle;

    UCHAR bBulkOutEndpointAddress;
    USHORT wBulkOutMaxPacket;
    PVOID pBulkOutEndpointHandle;

    UCHAR bIntInEndpointAddress;
    USHORT wIntInMaxPacket;
    PVOID pIntInEndpointHandle;

    USHORT wDTR_RTS;

} COMM_DEVICE_INFO;

typedef union _URB_MEMORY {

    UCHAR ConfigDesc[CONFIG_DESC_SIZE];
    USB_DEVICE_DESCRIPTOR DevDesc;
    PUSB_INTERFACE_DESCRIPTOR InterDesc;
    USB_COMM_LINE_CODING LineCoding;

} URB_MEMORY;

typedef struct _READ_BUFFER {

    UCHAR buffer[MODEM_BUFFER_SIZE];
    ULONG iCurrent;
    ULONG cb;
    PKEVENT pevent;
    ULONG fReadPending : 1;

} READ_BUFFER;

// globals
extern USB_PNP_CLASS_DRIVER_DESCRIPTION gClassDescription;
extern USB_PNP_DEVICE_TYPE_DESCRIPTION  gDeviceDescription;

extern PMDL gpMdl;
extern URB gUrb;
extern BOOL gfUrbInUse;
extern BOOL gfRemoved;
extern KIRQL gIrql;

extern PMDL gpMdlRead;
extern URB gUrbRead;

extern URB_MEMORY gMem;

extern COMM_DEVICE_INFO gDevice;
extern READ_BUFFER gRead;

// function declarations
BOOL Modem_IsConnected(VOID);
BOOL Modem_Create(VOID);
BOOL Modem_Close(VOID);

VOID Wait(PKEVENT pevent);

VOID CloseEndpoints(VOID);
BOOL OpenEndpoints(VOID);
VOID ConfigureDevice(IUsbDevice *pDevice);

VOID ReadWorker(VOID);
BOOL ReadData(PUCHAR buffer, ULONG *pcb);
BOOL WriteData(PUCHAR buffer, ULONG *pcb);

BOOL GetLineCoding(PULONG pBaudRate, PUCHAR pStopBits, PUCHAR pParity,
        PUCHAR pDataBits);
BOOL SetLineCoding(ULONG BaudRate, UCHAR StopBits, UCHAR Parity,
        UCHAR DataBits);
BOOL GetDtrRts(USHORT *pwState);
BOOL SetDtrRts(USHORT wState);

#endif // __MODEM_H__

