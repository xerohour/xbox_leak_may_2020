;/*++
;
;Copyright (c) 2001  Microsoft Corporation
;
;Module Name:
;
;    mcpxsmb.inc
;
;Abstract:
;
;    This module defines the MCPX SMBus logic block registers and fields.
;
;--
if 0        ; Begin C only code         */

//
// Define the SMBus registers starting at XPCICFG_SMBUS_IO_REGISTER_BASE_1.
//

#define MCPX_SMBUS_HOST_STATUS_REGISTER         0x00
#define MCPX_SMBUS_HOST_CONTROL_REGISTER        0x02
#define MCPX_SMBUS_HOST_ADDRESS_REGISTER        0x04
#define MCPX_SMBUS_HOST_DATA_REGISTER           0x06
#define MCPX_SMBUS_HOST_COMMAND_REGISTER        0x08
#define MCPX_SMBUS_HOST_BLOCK_DATA_REGISTER     0x09

//
// Define the status bits for MCPX_SMBUS_HOST_STATUS_REGISTER.
//

#define MCPX_STATUS_HOST_ABORT                  0x0001
#define MCPX_STATUS_COLLISION                   0x0002
#define MCPX_STATUS_PROTOCOL_ERROR              0x0004
#define MCPX_STATUS_HOST_BUSY                   0x0008
#define MCPX_STATUS_HOST_CYCLE_COMPLETE         0x0010
#define MCPX_STATUS_TIMEOUT_ERROR               0x0020

//
// Define the control bits for MCPX_SMBUS_HOST_CONTROL_REGISTER.
//

#define MCPX_CONTROL_CYCLE_READ_WRITE_BYTE      0x0002
#define MCPX_CONTROL_CYCLE_READ_WRITE_WORD      0x0003
#define MCPX_CONTROL_HOST_START                 0x0008
#define MCPX_CONTROL_INTERRUPT_ENABLE           0x0010

//
// Define the address bits for MCPX_SMBUS_HOST_ADDRESS_REGISTER.
//

#define MCPX_ADDRESS_READ_CYCLE                 0x0001

/*
endif

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
MCPX_CONTROL_INTERRUPT_ENABLE           EQU     0010h

;
; Define the address bits for MCPX_SMBUS_HOST_ADDRESS_REGISTER.
;

MCPX_ADDRESS_READ_CYCLE                 EQU     0001h

;*/
