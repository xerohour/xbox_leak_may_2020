;       (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
;
;       NVIDIA.INC - Compile options for NVidia MiniVDD
;
;       Written by:     Mark Stephen Krueger
;       Date:           10/10/96
;
;       $Revision: 2 $
;       $Date: 10/15/97 1:27p $
;       $Logfile: /resman.040/win96/vdd/nvdef.inc $
;       $Log: /resman.040/win96/vdd/nvdef.inc $
; 
; 2     10/15/97 1:27p Jhinman
; Don't define DOS_REFRESH!
; 
; 1     5/13/97 10:43a Jeffwe
; 
; 1     5/12/97 6:49p Jeffwe
; 
; 10    4/01/97 8:43p Jhinman
; Turn off debug msgs.
; 
; 9     1/31/97 6:00p Markk
; Added DETECT_LATE define.
; 
; 8     1/23/97 11:02a Markk
; Removed tabs.
; Fixed copyright.
; Put refresh support back in.
; Added GET_RM_MAP define to get linear addresses from resource manager.
; 
; 7     1/22/97 12:10p Markk
; Don't do refresh rate right away, test on IKOS without it first.
; 
; 6     1/14/97 3:41p Markk
; Setup defines for NV3.
; Added OLD_VESA30_REFRESH define to ifdef out the old frequency list
; scan.
; 
; 5     11/19/96 2:04p Markk
; Added DOS refresh rates.
; Removed NVDEB stuff.
; 
; 4     11/15/96 6:07p Markk
; Added NVIDIA_COMMON, NVIDIA_SIMULATE for simulating.
; Added REFRESH_INCLUDE to include refresh rate control.
; 
; 3     11/13/96 12:29p Markk
; Added TEST_PCI_NV1 define for testing the PCI code on NV1.
; 
; 2     11/01/96 3:58p Markk
; Added DEBUG_LOG define.
; 
; 1     10/16/96 9:16a Markk
; Contains compile time option settings for the VDD.
;

;DEBUG_LOG                       equ             1       ; Output debug info to debug device
NVIDIA_SPECIFIC                 equ             1       ; Assemble items specific to NVidia
;NVIDIA_SIMULATE                 equ             1       ; Simulate real mode access registers
NVIDIA_COMMON                   equ             1       ; Define if NVIDIA_SPECIFIC or NVIDIA_SIMULATE are defined!!!
;WEITEK_PROTO                   equ             1       ; Assemble items specific to Weitek
ALLOW_32K_VIRTUALIZATION        equ             1       ; Allow returning of a 32KB block for VGA 4 plane virtualization
;FAST_HIRES_SAVE_RESTORE                equ             1       ; Allow linear frame buffer hires save and restore
VESA30                          equ             1       ; Include VESA VBE 3.0
VESA30_REFRESH                  equ             1       ; Include VESA VBE 3.0 refresh rate definitions
;TEST_PCI_NV1                    equ             1       ; Test PCI code by searching for NV1
INCLUDE_REFRESH                 equ             1       ; Include refresh rate control
;DOS_REFRESH                     equ             1       ; Include refresh rate control for DirectX and DOS
;OLD_VESA30_REFRESH              equ             1       ; Include support for old VBE 3.0 dot clock method
GET_RM_MAP                      equ             1       ; Get linear addresses from resource manager
DETECT_LATE                     equ             1       ; Detect the chip later on
