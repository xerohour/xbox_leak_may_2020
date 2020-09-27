;/*++
;
;Copyright (c) 2001  Microsoft Corporation
;
;Module Name:
;
;    mcpxacpi.inc
;
;Abstract:
;
;    This module defines the MCPX ACPI logic block registers and fields.
;
;--
if 0        ; Begin C only code         */

//
// Define the ACPI registers starting at XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0.
//

#define MCPX_ACPI_PM1_STATUS_REGISTER           0x00
#define MCPX_ACPI_PM1_ENABLE_REGISTER           0x02
#define MCPX_ACPI_PM1_CONTROL_REGISTER          0x04
#define MCPX_ACPI_PM_TIMER_REGISTER             0x08
#define MCPX_ACPI_GPE0_STATUS_REGISTER          0x20
#define MCPX_ACPI_GPE0_ENABLE_REGISTER          0x22
#define MCPX_ACPI_GLOBAL_SMI_CONTROL            0x28

//
// Define the status bits for MCPX_ACPI_PM1_STATUS_REGISTER.
//

#define PM1_TIMER_STATUS                        0x0001

//
// Define the enable bits for MCPX_ACPI_PM1_ENABLE_REGISTER.
//

#define PM1_TIMER_ENABLE                        0x0001

//
// Define the control bits for MCPX_ACPI_PM1_CONTROL_REGISTER.
//

#define PM1_SCI_ENABLE                          0x0001

//
// Define the enable bits for MCPX_ACPI_GPE0_ENABLE_REGISTER.
//

#define GPE0_EXTSMI_ENABLE                      0x0002

//
// Define the control bits for MCPX_ACPI_GLOBAL_SMI_CONTROL.
//

#define GPE0_SMI_ENABLE                         0x0001

/*
endif

;
; Define the ACPI registers starting at XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0.
;

MCPX_ACPI_PM1_STATUS_REGISTER           EQU     00h
MCPX_ACPI_PM1_ENABLE_REGISTER           EQU     02h
MCPX_ACPI_PM1_CONTROL_REGISTER          EQU     04h
MCPX_ACPI_PM_TIMER_REGISTER             EQU     08h
MCPX_ACPI_GPE0_STATUS_REGISTER          EQU     20h
MCPX_ACPI_GPE0_ENABLE_REGISTER          EQU     22h
MCPX_ACPI_GLOBAL_SMI_CONTROL            EQU     28h

;
; Define the status bits for MCPX_ACPI_PM1_STATUS_REGISTER.
;

PM1_TIMER_STATUS                        EQU     0001h

;
; Define the status bits for MCPX_ACPI_GPE0_STATUS_REGISTER.
;

GPE0_EXTSMI_STATUS                      EQU     0002h

;*/
