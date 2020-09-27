        TITLE   "SMBus Routines"
;++
;
;  Copyright (c) Microsoft Corporation
;
;  Module Name:
;
;     smbus.asm
;
;  Abstract:
;
;     This module implements the routines to send and receive data over SMBus.
;
;
;--

        .686p
        .xlist
INCLUDE ks386.inc
INCLUDE callconv.inc
        .list

;
; Define the SMBus registers starting at XPCICFG_SMBUS_IO_REGISTER_BASE_1.
;

MCPX_SMBUS_HOST_STATUS_REGISTER         EQU     00h
MCPX_SMBUS_HOST_CONTROL_REGISTER        EQU     02h
MCPX_SMBUS_HOST_ADDRESS_REGISTER        EQU     04h
MCPX_SMBUS_HOST_DATA_REGISTER           EQU     06h
MCPX_SMBUS_HOST_COMMAND_REGISTER        EQU     08h
MCPX_SMBUS_HOST_BLOCK_DATA_REGISTER     EQU     09h

;
; Define the status bits for MCPX_SMBUS_HOST_STATUS_REGISTER.
;

MCPX_STATUS_HOST_ABORT                  EQU     0001h
MCPX_STATUS_COLLISION                   EQU     0002h
MCPX_STATUS_PROTOCOL_ERROR              EQU     0004h
MCPX_STATUS_HOST_BUSY                   EQU     0008h
MCPX_STATUS_HOST_CYCLE_COMPLETE         EQU     0010h
MCPX_STATUS_TIMEOUT_ERROR               EQU     0020h

;
; Define the control bits for MCPX_SMBUS_HOST_CONTROL_REGISTER.
;

MCPX_CONTROL_CYCLE_READ_WRITE_BYTE      EQU     0002h
MCPX_CONTROL_CYCLE_READ_WRITE_WORD      EQU     0003h
MCPX_CONTROL_HOST_START                 EQU     0008h

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
;
; NTSTATUS
; BldrReadSMBusValue(
;     IN UCHAR SlaveAddress,
;     IN UCHAR CommandCode,
;     IN BOOLEAN ReadWordValue,
;     OUT UCHAR *DataValue
;     )
;
; Routine Description:
;
;     This routine submits a "read byte" or "read word" transaction over SMBus.
;
; Arguments:
;
;     SlaveAddress - Specifies the slave address of the SMBus device to access.
;
;     CommandCode - Specifies the command field to pass during as part of the
;         host cycle.
;
;     ReadWordValue - Specifies TRUE if 16-bits should be read, else FALSE if
;         8-bits should be read.
;
;     DataValue - Specifies the location of the buffer to receive the data read
;         from the SMBus.
;
; Return Value:
;
;     Status of operation.
;
;--
cPublicProc _BldrReadSMBusValue, 4

RsmbSlaveAddress    EQU     4
RsmbCommandCode     EQU     8
RsmbReadWordValue   EQU     12
RsmbDataValue       EQU     16


;
; Set the host address register.
;

rsmb10: xor     eax, eax                        ; eliminate partial stall
        mov     al, BYTE PTR [esp+RsmbSlaveAddress]
        mov     edx, XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_ADDRESS_REGISTER
        or      al, 1                           ; force a read cycle
        out     dx, al

;
; Set the host command register.
;

        mov     al, BYTE PTR [esp+RsmbCommandCode]
        add     edx, MCPX_SMBUS_HOST_COMMAND_REGISTER - MCPX_SMBUS_HOST_ADDRESS_REGISTER
        out     dx, al

;
; Write the current contents of the status register out to itself in order to
; clear the register.
;

        sub     edx, MCPX_SMBUS_HOST_COMMAND_REGISTER - MCPX_SMBUS_HOST_STATUS_REGISTER
        in      ax, dx
        out     dx, ax

;
; Initiate the read transaction.
;

        add     edx, MCPX_SMBUS_HOST_CONTROL_REGISTER - MCPX_SMBUS_HOST_STATUS_REGISTER
        mov     al, MCPX_CONTROL_HOST_START + MCPX_CONTROL_CYCLE_READ_WRITE_BYTE
        cmp     BYTE PTR [esp+RsmbReadWordValue], 0
        je      rsmb15
        inc     al                              ; select the read word cycle
rsmb15: out     dx, al

;
; Spin until the host controller indicates that its not busy.
;

        sub     edx, MCPX_SMBUS_HOST_CONTROL_REGISTER - MCPX_SMBUS_HOST_STATUS_REGISTER
rsmb20: in      al, dx
        test    al, MCPX_STATUS_HOST_BUSY
        jnz     rsmb20

;
; Check if there was a collision on the SMBus.  If so, retry the operation.
;

        test    al, MCPX_STATUS_COLLISION
        jnz     rsmb10

;
; Check if the transaction timed out.
;

        mov     edx, STATUS_IO_TIMEOUT
        test    al, MCPX_STATUS_TIMEOUT_ERROR
        jnz     rsmb30

;
; Check if the transaction caused a protocol error.
;

        mov     edx, STATUS_DEVICE_PROTOCOL_ERROR
        test    al, MCPX_STATUS_PROTOCOL_ERROR
        jnz     rsmb30

;
; Check if the transaction completed successfully.
;

        mov     edx, STATUS_IO_DEVICE_ERROR
        test    al, MCPX_STATUS_HOST_CYCLE_COMPLETE
        jz      rsmb30

;
; Read the host data register and write the contents back to the user's buffer
; and return STATUS_SUCCESS;
;

        xor     eax, eax
        mov     edx, XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_DATA_REGISTER
        in      ax, dx
        cmp     BYTE PTR [esp+RsmbReadWordValue], 0
        jne     rsmb25
        xor     ah, ah                          ; only return eight bits of data
rsmb25: mov     edx, DWORD PTR [esp+RsmbDataValue]
        mov     DWORD PTR [edx], eax
        xor     edx, edx                        ; edx = STATUS_SUCCESS

rsmb30:
        mov     eax, edx
        
        stdRET  _BldrReadSMBusValue

stdENDP _BldrReadSMBusValue

;++
;
; NTSTATUS
; BldrWriteSMBusByte(
;     IN UCHAR SlaveAddress,
;     IN UCHAR CommandCode,
;     IN BOOLEAN WriteWordValue,
;     IN ULONG DataValue
;     )
;
; Routine Description:
;
;     This routine submits a "write byte" or "write word" transaction over
;     SMBus.
;
; Arguments:
;
;     SlaveAddress - Specifies the slave address of the SMBus device to access.
;
;     CommandCode - Specifies the command field to pass during as part of the
;         host cycle.
;
;     WriteWordValue - Specifies TRUE if 16-bits should be written, else FALSE
;         if 8-bits should be written.
;
;     DataValue - Specifies the data value to transmit over SMBus.
;
; Return Value:
;
;     Status of operation.
;
;--
cPublicProc _BldrWriteSMBusValue, 4

WsmbSlaveAddress    EQU     4
WsmbCommandCode     EQU     8
WsmbWriteWordValue  EQU     12
WsmbDataValue       EQU     16


;
; Set the host address register.
;

wsmb10: xor     eax, eax                        ; eliminate partial stall
        mov     al, BYTE PTR [esp+WsmbSlaveAddress]
        mov     edx, XPCICFG_SMBUS_IO_REGISTER_BASE_1 + MCPX_SMBUS_HOST_ADDRESS_REGISTER
        and     al, NOT 1                       ; force a write cycle
        out     dx, al

;
; Set the host command register.
;

        mov     al, BYTE PTR [esp+WsmbCommandCode]
        add     edx, MCPX_SMBUS_HOST_COMMAND_REGISTER - MCPX_SMBUS_HOST_ADDRESS_REGISTER
        out     dx, al

;
; Set the host data register.
;

        mov     eax, DWORD PTR [esp+WsmbDataValue]
        sub     edx, MCPX_SMBUS_HOST_COMMAND_REGISTER - MCPX_SMBUS_HOST_DATA_REGISTER
        out     dx, ax

;
; Write the current contents of the status register out to itself in order to
; clear the register.
;

        sub     edx, MCPX_SMBUS_HOST_DATA_REGISTER - MCPX_SMBUS_HOST_STATUS_REGISTER
        in      ax, dx
        out     dx, ax

;
; Initiate the write transaction.
;

        add     edx, MCPX_SMBUS_HOST_CONTROL_REGISTER - MCPX_SMBUS_HOST_STATUS_REGISTER
        mov     al, MCPX_CONTROL_HOST_START + MCPX_CONTROL_CYCLE_READ_WRITE_BYTE
        cmp     BYTE PTR [esp+WsmbWriteWordValue], 0
        je      wsmb15
        inc     al                              ; select the read word cycle
wsmb15: out     dx, al

;
; Spin until the host controller indicates that its not busy.
;

        sub     edx, MCPX_SMBUS_HOST_CONTROL_REGISTER - MCPX_SMBUS_HOST_STATUS_REGISTER
wsmb20: in      al, dx
        test    al, MCPX_STATUS_HOST_BUSY
        jnz     wsmb20

;
; Check if there was a collision on the SMBus.  If so, retry the operation.
;

        test    al, MCPX_STATUS_COLLISION
        jnz     wsmb10

;
; Check if the transaction timed out.
;

        mov     edx, STATUS_IO_TIMEOUT
        test    al, MCPX_STATUS_TIMEOUT_ERROR
        jnz     wsmb30

;
; Check if the transaction caused a protocol error.
;

        mov     edx, STATUS_DEVICE_PROTOCOL_ERROR
        test    al, MCPX_STATUS_PROTOCOL_ERROR
        jnz     wsmb30

;
; Check if the transaction completed successfully.
;

        test    al, MCPX_STATUS_HOST_CYCLE_COMPLETE
        mov     edx, STATUS_IO_DEVICE_ERROR
        jz      wsmb30

;
; The write completed successfully, so return STATUS_SUCCESS.
;

        xor     edx, edx                        ; edx = STATUS_SUCCESS

wsmb30:
        mov     eax, edx
        
        stdRET  _BldrWriteSMBusValue

stdENDP _BldrWriteSMBusValue

_TEXT   ends
        end

