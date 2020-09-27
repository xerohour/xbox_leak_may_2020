
;option expr32
;option casemap:none

; Begin of file nv_ref.h
;  @(#) $Header: /resman.040/win96/vdd/nv_ref.inc 1     5/13/97 10:43a Jeffwe $
;  NVidia Corporation 
;  basis: nv1 manuals 10.1
;  95/06/27 NVidia Corporation 
;     1 : 
;     2 : /* @(#) $Header: /resman.040/win96/vdd/nv_ref.inc 1     5/13/97 10:43a Jeffwe $
;     3 :  NVidia Corporation */
;     4 : /* basis: nv1 manuals 10.1
;     5 :  95/06/27 NVidia Corporation */
;     6 : 
;     7 : #ifndef _NV_REF_H_
;     8 : #define _NV_REF_H_
_NV_REF_H_              TEXTEQU         <>
;  dev_bus.ref 
;  RW--D 
;     9 : 
;    10 : /* dev_bus.ref */
;    11 : #define NV_SPACE                              0x01FFFFFF:0x00000000 /* RW--D */
NV_SPACE                TEXTEQU         <0x01FFFFFF:0x00000000>
;  dev_bus.ref 
;  RW--D 
;    12 : /* dev_bus.ref */
;    13 : #define NV_CONFIG                             0x000000FF:0x00000000 /* RW--D */
NV_CONFIG               TEXTEQU         <0x000000FF:0x00000000>
;  R--4R 
;    14 : #define NV_CONFIG_PCI_NV_0                               0x00000000 /* R--4R */
NV_CONFIG_PCI_NV_0              EQU             000000000h
;        
;    15 : #define NV_CONFIG_PCI_NV_0__ALIAS_1                NV_PBUS_PCI_NV_0 /*       */
NV_CONFIG_PCI_NV_0__ALIAS_1             TEXTEQU         <NV_PBUS_PCI_NV_0>
;  C--UF 
;    16 : #define NV_CONFIG_PCI_NV_0_VENDOR_ID                           15:0 /* C--UF */
NV_CONFIG_PCI_NV_0_VENDOR_ID            TEXTEQU         <15:0>
;  C---V 
;    17 : #define NV_CONFIG_PCI_NV_0_VENDOR_ID_NVIDIA_SGS          0x000012D2 /* C---V */
NV_CONFIG_PCI_NV_0_VENDOR_ID_NVIDIA_SGS         EQU             0000012d2h
;  C--UF 
;    18 : #define NV_CONFIG_PCI_NV_0_DEVICE_ID_FUNC                     18:16 /* C--UF */
NV_CONFIG_PCI_NV_0_DEVICE_ID_FUNC               TEXTEQU         <18:16>
;  C---V 
;    19 : #define NV_CONFIG_PCI_NV_0_DEVICE_ID_FUNC_VGA            0x00000000 /* C---V */
NV_CONFIG_PCI_NV_0_DEVICE_ID_FUNC_VGA           EQU             000000000h
;  C--UF 
;    20 : #define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP                     31:19 /* C--UF */
NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP               TEXTEQU         <31:19>
;  ----V 
;    21 : #define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV0            0x00000000 /* ----V */
NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV0           EQU             000000000h
;  ----V 
;    22 : #define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV1            0x00000001 /* ----V */
NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV1           EQU             000000001h
;  ----V 
;    23 : #define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV2            0x00000002 /* ----V */
NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV2           EQU             000000002h
;  C---V 
;    24 : #define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV3            0x00000003 /* C---V */
NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV3           EQU             000000003h
;  RW-4R 
;    25 : #define NV_CONFIG_PCI_NV_1                               0x00000004 /* RW-4R */
NV_CONFIG_PCI_NV_1              EQU             000000004h
;        
;    26 : #define NV_CONFIG_PCI_NV_1__ALIAS_1                NV_PBUS_PCI_NV_1 /*       */
NV_CONFIG_PCI_NV_1__ALIAS_1             TEXTEQU         <NV_PBUS_PCI_NV_1>
;  RWIVF 
;    27 : #define NV_CONFIG_PCI_NV_1_IO_SPACE                             0:0 /* RWIVF */
NV_CONFIG_PCI_NV_1_IO_SPACE             TEXTEQU         <0:0>
;  RWI-V 
;    28 : #define NV_CONFIG_PCI_NV_1_IO_SPACE_DISABLED             0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_1_IO_SPACE_DISABLED            EQU             000000000h
;  RW--V 
;    29 : #define NV_CONFIG_PCI_NV_1_IO_SPACE_ENABLED              0x00000001 /* RW--V */
NV_CONFIG_PCI_NV_1_IO_SPACE_ENABLED             EQU             000000001h
;  RWIVF 
;    30 : #define NV_CONFIG_PCI_NV_1_MEMORY_SPACE                         1:1 /* RWIVF */
NV_CONFIG_PCI_NV_1_MEMORY_SPACE         TEXTEQU         <1:1>
;  RWI-V 
;    31 : #define NV_CONFIG_PCI_NV_1_MEMORY_SPACE_DISABLED         0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_1_MEMORY_SPACE_DISABLED                EQU             000000000h
;  RW--V 
;    32 : #define NV_CONFIG_PCI_NV_1_MEMORY_SPACE_ENABLED          0x00000001 /* RW--V */
NV_CONFIG_PCI_NV_1_MEMORY_SPACE_ENABLED         EQU             000000001h
;  RWIVF 
;    33 : #define NV_CONFIG_PCI_NV_1_BUS_MASTER                           2:2 /* RWIVF */
NV_CONFIG_PCI_NV_1_BUS_MASTER           TEXTEQU         <2:2>
;  RWI-V 
;    34 : #define NV_CONFIG_PCI_NV_1_BUS_MASTER_DISABLED           0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_1_BUS_MASTER_DISABLED          EQU             000000000h
;  RW--V 
;    35 : #define NV_CONFIG_PCI_NV_1_BUS_MASTER_ENABLED            0x00000001 /* RW--V */
NV_CONFIG_PCI_NV_1_BUS_MASTER_ENABLED           EQU             000000001h
;  C--VF 
;    36 : #define NV_CONFIG_PCI_NV_1_WRITE_AND_INVAL                      4:4 /* C--VF */
NV_CONFIG_PCI_NV_1_WRITE_AND_INVAL              TEXTEQU         <4:4>
;  C---V 
;    37 : #define NV_CONFIG_PCI_NV_1_WRITE_AND_INVAL_DISABLED      0x00000000 /* C---V */
NV_CONFIG_PCI_NV_1_WRITE_AND_INVAL_DISABLED             EQU             000000000h
;  ----V 
;    38 : #define NV_CONFIG_PCI_NV_1_WRITE_AND_INVAL_ENABLED       0x00000001 /* ----V */
NV_CONFIG_PCI_NV_1_WRITE_AND_INVAL_ENABLED              EQU             000000001h
;  RWIVF 
;    39 : #define NV_CONFIG_PCI_NV_1_PALETTE_SNOOP                        5:5 /* RWIVF */
NV_CONFIG_PCI_NV_1_PALETTE_SNOOP                TEXTEQU         <5:5>
;  RWI-V 
;    40 : #define NV_CONFIG_PCI_NV_1_PALETTE_SNOOP_DISABLED        0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_1_PALETTE_SNOOP_DISABLED               EQU             000000000h
;  RW--V 
;    41 : #define NV_CONFIG_PCI_NV_1_PALETTE_SNOOP_ENABLED         0x00000001 /* RW--V */
NV_CONFIG_PCI_NV_1_PALETTE_SNOOP_ENABLED                EQU             000000001h
;  R--VF 
;    42 : #define NV_CONFIG_PCI_NV_1_CAPLIST                            20:20 /* R--VF */
NV_CONFIG_PCI_NV_1_CAPLIST              TEXTEQU         <20:20>
;  ----V 
;    43 : #define NV_CONFIG_PCI_NV_1_CAPLIST_NOT_PRESENT           0x00000000 /* ----V */
NV_CONFIG_PCI_NV_1_CAPLIST_NOT_PRESENT          EQU             000000000h
;  ----V 
;    44 : #define NV_CONFIG_PCI_NV_1_CAPLIST_PRESENT               0x00000001 /* ----V */
NV_CONFIG_PCI_NV_1_CAPLIST_PRESENT              EQU             000000001h
;  C--VF 
;    45 : #define NV_CONFIG_PCI_NV_1_66MHZ                              21:21 /* C--VF */
NV_CONFIG_PCI_NV_1_66MHZ                TEXTEQU         <21:21>
;  ----V 
;    46 : #define NV_CONFIG_PCI_NV_1_66MHZ_INCAPABLE               0x00000000 /* ----V */
NV_CONFIG_PCI_NV_1_66MHZ_INCAPABLE              EQU             000000000h
;  ----V 
;    47 : #define NV_CONFIG_PCI_NV_1_66MHZ_CAPABLE                 0x00000001 /* ----V */
NV_CONFIG_PCI_NV_1_66MHZ_CAPABLE                EQU             000000001h
;  C--VF 
;    48 : #define NV_CONFIG_PCI_NV_1_FAST_BACK2BACK                     23:23 /* C--VF */
NV_CONFIG_PCI_NV_1_FAST_BACK2BACK               TEXTEQU         <23:23>
;  ----V 
;    49 : #define NV_CONFIG_PCI_NV_1_FAST_BACK2BACK_INCAPABLE      0x00000000 /* ----V */
NV_CONFIG_PCI_NV_1_FAST_BACK2BACK_INCAPABLE             EQU             000000000h
;  C---V 
;    50 : #define NV_CONFIG_PCI_NV_1_FAST_BACK2BACK_CAPABLE        0x00000001 /* C---V */
NV_CONFIG_PCI_NV_1_FAST_BACK2BACK_CAPABLE               EQU             000000001h
;  C--VF 
;    51 : #define NV_CONFIG_PCI_NV_1_DEVSEL_TIMING                      26:25 /* C--VF */
NV_CONFIG_PCI_NV_1_DEVSEL_TIMING                TEXTEQU         <26:25>
;  ----V 
;    52 : #define NV_CONFIG_PCI_NV_1_DEVSEL_TIMING_FAST            0x00000000 /* ----V */
NV_CONFIG_PCI_NV_1_DEVSEL_TIMING_FAST           EQU             000000000h
;  C---V 
;    53 : #define NV_CONFIG_PCI_NV_1_DEVSEL_TIMING_MEDIUM          0x00000001 /* C---V */
NV_CONFIG_PCI_NV_1_DEVSEL_TIMING_MEDIUM         EQU             000000001h
;  ----V 
;    54 : #define NV_CONFIG_PCI_NV_1_DEVSEL_TIMING_SLOW            0x00000002 /* ----V */
NV_CONFIG_PCI_NV_1_DEVSEL_TIMING_SLOW           EQU             000000002h
;  RWIVF 
;    55 : #define NV_CONFIG_PCI_NV_1_SIGNALED_TARGET                    27:27 /* RWIVF */
NV_CONFIG_PCI_NV_1_SIGNALED_TARGET              TEXTEQU         <27:27>
;  R-I-V 
;    56 : #define NV_CONFIG_PCI_NV_1_SIGNALED_TARGET_NO_ABORT      0x00000000 /* R-I-V */
NV_CONFIG_PCI_NV_1_SIGNALED_TARGET_NO_ABORT             EQU             000000000h
;  R---V 
;    57 : #define NV_CONFIG_PCI_NV_1_SIGNALED_TARGET_ABORT         0x00000001 /* R---V */
NV_CONFIG_PCI_NV_1_SIGNALED_TARGET_ABORT                EQU             000000001h
;  -W--V 
;    58 : #define NV_CONFIG_PCI_NV_1_SIGNALED_TARGET_CLEAR         0x00000001 /* -W--V */
NV_CONFIG_PCI_NV_1_SIGNALED_TARGET_CLEAR                EQU             000000001h
;  RWIVF 
;    59 : #define NV_CONFIG_PCI_NV_1_RECEIVED_TARGET                    28:28 /* RWIVF */
NV_CONFIG_PCI_NV_1_RECEIVED_TARGET              TEXTEQU         <28:28>
;  R-I-V 
;    60 : #define NV_CONFIG_PCI_NV_1_RECEIVED_TARGET_NO_ABORT      0x00000000 /* R-I-V */
NV_CONFIG_PCI_NV_1_RECEIVED_TARGET_NO_ABORT             EQU             000000000h
;  R---V 
;    61 : #define NV_CONFIG_PCI_NV_1_RECEIVED_TARGET_ABORT         0x00000001 /* R---V */
NV_CONFIG_PCI_NV_1_RECEIVED_TARGET_ABORT                EQU             000000001h
;  -W--V 
;    62 : #define NV_CONFIG_PCI_NV_1_RECEIVED_TARGET_CLEAR         0x00000001 /* -W--V */
NV_CONFIG_PCI_NV_1_RECEIVED_TARGET_CLEAR                EQU             000000001h
;  RWIVF 
;    63 : #define NV_CONFIG_PCI_NV_1_RECEIVED_MASTER                    29:29 /* RWIVF */
NV_CONFIG_PCI_NV_1_RECEIVED_MASTER              TEXTEQU         <29:29>
;  R-I-V 
;    64 : #define NV_CONFIG_PCI_NV_1_RECEIVED_MASTER_NO_ABORT      0x00000000 /* R-I-V */
NV_CONFIG_PCI_NV_1_RECEIVED_MASTER_NO_ABORT             EQU             000000000h
;  R---V 
;    65 : #define NV_CONFIG_PCI_NV_1_RECEIVED_MASTER_ABORT         0x00000001 /* R---V */
NV_CONFIG_PCI_NV_1_RECEIVED_MASTER_ABORT                EQU             000000001h
;  -W--V 
;    66 : #define NV_CONFIG_PCI_NV_1_RECEIVED_MASTER_CLEAR         0x00000001 /* -W--V */
NV_CONFIG_PCI_NV_1_RECEIVED_MASTER_CLEAR                EQU             000000001h
;  R--4R 
;    67 : #define NV_CONFIG_PCI_NV_2                               0x00000008 /* R--4R */
NV_CONFIG_PCI_NV_2              EQU             000000008h
;        
;    68 : #define NV_CONFIG_PCI_NV_2__ALIAS_1                NV_PBUS_PCI_NV_2 /*       */
NV_CONFIG_PCI_NV_2__ALIAS_1             TEXTEQU         <NV_PBUS_PCI_NV_2>
;  C--UF 
;    69 : #define NV_CONFIG_PCI_NV_2_REVISION_ID                          7:0 /* C--UF */
NV_CONFIG_PCI_NV_2_REVISION_ID          TEXTEQU         <7:0>
;  C---V 
;    70 : #define NV_CONFIG_PCI_NV_2_REVISION_ID_A                 0x00000000 /* C---V */
NV_CONFIG_PCI_NV_2_REVISION_ID_A                EQU             000000000h
;  ----V 
;    71 : #define NV_CONFIG_PCI_NV_2_REVISION_ID_B                 0x00000001 /* ----V */
NV_CONFIG_PCI_NV_2_REVISION_ID_B                EQU             000000001h
;  C--VF 
;    72 : #define NV_CONFIG_PCI_NV_2_CLASS_CODE                          31:8 /* C--VF */
NV_CONFIG_PCI_NV_2_CLASS_CODE           TEXTEQU         <31:8>
;  C---V 
;    73 : #define NV_CONFIG_PCI_NV_2_CLASS_CODE_VGA                0x00030000 /* C---V */
NV_CONFIG_PCI_NV_2_CLASS_CODE_VGA               EQU             000030000h
;  ----V 
;    74 : #define NV_CONFIG_PCI_NV_2_CLASS_CODE_MULTIMEDIA         0x00048000 /* ----V */
NV_CONFIG_PCI_NV_2_CLASS_CODE_MULTIMEDIA                EQU             000048000h
;  RW-4R 
;    75 : #define NV_CONFIG_PCI_NV_3                               0x0000000C /* RW-4R */
NV_CONFIG_PCI_NV_3              EQU             00000000ch
;        
;    76 : #define NV_CONFIG_PCI_NV_3__ALIAS_1                NV_PBUS_PCI_NV_3 /*       */
NV_CONFIG_PCI_NV_3__ALIAS_1             TEXTEQU         <NV_PBUS_PCI_NV_3>
;  RWIUF 
;    77 : #define NV_CONFIG_PCI_NV_3_LATENCY_TIMER                      15:11 /* RWIUF */
NV_CONFIG_PCI_NV_3_LATENCY_TIMER                TEXTEQU         <15:11>
;  RWI-V 
;    78 : #define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_0_CLOCKS        0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_3_LATENCY_TIMER_0_CLOCKS               EQU             000000000h
;  RW--V 
;    79 : #define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_8_CLOCKS        0x00000001 /* RW--V */
NV_CONFIG_PCI_NV_3_LATENCY_TIMER_8_CLOCKS               EQU             000000001h
;  RW--V 
;    80 : #define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_240_CLOCKS      0x0000001E /* RW--V */
NV_CONFIG_PCI_NV_3_LATENCY_TIMER_240_CLOCKS             EQU             00000001eh
;  RW--V 
;    81 : #define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_248_CLOCKS      0x0000001F /* RW--V */
NV_CONFIG_PCI_NV_3_LATENCY_TIMER_248_CLOCKS             EQU             00000001fh
;  C--VF 
;    82 : #define NV_CONFIG_PCI_NV_3_HEADER_TYPE                        23:16 /* C--VF */
NV_CONFIG_PCI_NV_3_HEADER_TYPE          TEXTEQU         <23:16>
;  C---V 
;    83 : #define NV_CONFIG_PCI_NV_3_HEADER_TYPE_SINGLEFUNC        0x00000000 /* C---V */
NV_CONFIG_PCI_NV_3_HEADER_TYPE_SINGLEFUNC               EQU             000000000h
;  ----V 
;    84 : #define NV_CONFIG_PCI_NV_3_HEADER_TYPE_MULTIFUNC         0x00000080 /* ----V */
NV_CONFIG_PCI_NV_3_HEADER_TYPE_MULTIFUNC                EQU             000000080h
;  RW-4R 
;    85 : #define NV_CONFIG_PCI_NV_4                               0x00000010 /* RW-4R */
NV_CONFIG_PCI_NV_4              EQU             000000010h
;        
;    86 : #define NV_CONFIG_PCI_NV_4__ALIAS_1                NV_PBUS_PCI_NV_4 /*       */
NV_CONFIG_PCI_NV_4__ALIAS_1             TEXTEQU         <NV_PBUS_PCI_NV_4>
;  C--VF 
;    87 : #define NV_CONFIG_PCI_NV_4_SPACE_TYPE                           0:0 /* C--VF */
NV_CONFIG_PCI_NV_4_SPACE_TYPE           TEXTEQU         <0:0>
;  C---V 
;    88 : #define NV_CONFIG_PCI_NV_4_SPACE_TYPE_MEMORY             0x00000000 /* C---V */
NV_CONFIG_PCI_NV_4_SPACE_TYPE_MEMORY            EQU             000000000h
;  ----V 
;    89 : #define NV_CONFIG_PCI_NV_4_SPACE_TYPE_IO                 0x00000001 /* ----V */
NV_CONFIG_PCI_NV_4_SPACE_TYPE_IO                EQU             000000001h
;  C--VF 
;    90 : #define NV_CONFIG_PCI_NV_4_ADDRESS_TYPE                         2:1 /* C--VF */
NV_CONFIG_PCI_NV_4_ADDRESS_TYPE         TEXTEQU         <2:1>
;  C---V 
;    91 : #define NV_CONFIG_PCI_NV_4_ADDRESS_TYPE_32_BIT           0x00000000 /* C---V */
NV_CONFIG_PCI_NV_4_ADDRESS_TYPE_32_BIT          EQU             000000000h
;  ----V 
;    92 : #define NV_CONFIG_PCI_NV_4_ADDRESS_TYPE_20_BIT           0x00000001 /* ----V */
NV_CONFIG_PCI_NV_4_ADDRESS_TYPE_20_BIT          EQU             000000001h
;  ----V 
;    93 : #define NV_CONFIG_PCI_NV_4_ADDRESS_TYPE_64_BIT           0x00000002 /* ----V */
NV_CONFIG_PCI_NV_4_ADDRESS_TYPE_64_BIT          EQU             000000002h
;  C--VF 
;    94 : #define NV_CONFIG_PCI_NV_4_PREFETCHABLE                         3:3 /* C--VF */
NV_CONFIG_PCI_NV_4_PREFETCHABLE         TEXTEQU         <3:3>
;  C---V 
;    95 : #define NV_CONFIG_PCI_NV_4_PREFETCHABLE_NOT              0x00000000 /* C---V */
NV_CONFIG_PCI_NV_4_PREFETCHABLE_NOT             EQU             000000000h
;  ----V 
;    96 : #define NV_CONFIG_PCI_NV_4_PREFETCHABLE_MERGABLE         0x00000001 /* ----V */
NV_CONFIG_PCI_NV_4_PREFETCHABLE_MERGABLE                EQU             000000001h
;  RWXUF 
;    97 : #define NV_CONFIG_PCI_NV_4_BASE_ADDRESS                       31:24 /* RWXUF */
NV_CONFIG_PCI_NV_4_BASE_ADDRESS         TEXTEQU         <31:24>
;  RW-4R 
;    98 : #define NV_CONFIG_PCI_NV_5                               0x00000014 /* RW-4R */
NV_CONFIG_PCI_NV_5              EQU             000000014h
;        
;    99 : #define NV_CONFIG_PCI_NV_5__ALIAS_1                NV_PBUS_PCI_NV_5 /*       */
NV_CONFIG_PCI_NV_5__ALIAS_1             TEXTEQU         <NV_PBUS_PCI_NV_5>
;  C--VF 
;   100 : #define NV_CONFIG_PCI_NV_5_SPACE_TYPE                           0:0 /* C--VF */
NV_CONFIG_PCI_NV_5_SPACE_TYPE           TEXTEQU         <0:0>
;  C---V 
;   101 : #define NV_CONFIG_PCI_NV_5_SPACE_TYPE_MEMORY             0x00000000 /* C---V */
NV_CONFIG_PCI_NV_5_SPACE_TYPE_MEMORY            EQU             000000000h
;  ----V 
;   102 : #define NV_CONFIG_PCI_NV_5_SPACE_TYPE_IO                 0x00000001 /* ----V */
NV_CONFIG_PCI_NV_5_SPACE_TYPE_IO                EQU             000000001h
;  C--VF 
;   103 : #define NV_CONFIG_PCI_NV_5_ADDRESS_TYPE                         2:1 /* C--VF */
NV_CONFIG_PCI_NV_5_ADDRESS_TYPE         TEXTEQU         <2:1>
;  C---V 
;   104 : #define NV_CONFIG_PCI_NV_5_ADDRESS_TYPE_32_BIT           0x00000000 /* C---V */
NV_CONFIG_PCI_NV_5_ADDRESS_TYPE_32_BIT          EQU             000000000h
;  ----V 
;   105 : #define NV_CONFIG_PCI_NV_5_ADDRESS_TYPE_20_BIT           0x00000001 /* ----V */
NV_CONFIG_PCI_NV_5_ADDRESS_TYPE_20_BIT          EQU             000000001h
;  ----V 
;   106 : #define NV_CONFIG_PCI_NV_5_ADDRESS_TYPE_64_BIT           0x00000002 /* ----V */
NV_CONFIG_PCI_NV_5_ADDRESS_TYPE_64_BIT          EQU             000000002h
;  C--VF 
;   107 : #define NV_CONFIG_PCI_NV_5_PREFETCHABLE                         3:3 /* C--VF */
NV_CONFIG_PCI_NV_5_PREFETCHABLE         TEXTEQU         <3:3>
;  ----V 
;   108 : #define NV_CONFIG_PCI_NV_5_PREFETCHABLE_NOT              0x00000000 /* ----V */
NV_CONFIG_PCI_NV_5_PREFETCHABLE_NOT             EQU             000000000h
;  C---V 
;   109 : #define NV_CONFIG_PCI_NV_5_PREFETCHABLE_MERGABLE         0x00000001 /* C---V */
NV_CONFIG_PCI_NV_5_PREFETCHABLE_MERGABLE                EQU             000000001h
;  RWXUF 
;   110 : #define NV_CONFIG_PCI_NV_5_BASE_ADDRESS                       31:24 /* RWXUF */
NV_CONFIG_PCI_NV_5_BASE_ADDRESS         TEXTEQU         <31:24>
;  RW-4R 
;   111 : #define NV_CONFIG_PCI_NV_6                               0x00000018 /* RW-4R */
NV_CONFIG_PCI_NV_6              EQU             000000018h
;        
;   112 : #define NV_CONFIG_PCI_NV_6__ALIAS_1                NV_PBUS_PCI_NV_6 /*       */
NV_CONFIG_PCI_NV_6__ALIAS_1             TEXTEQU         <NV_PBUS_PCI_NV_6>
;  C--VF 
;   113 : #define NV_CONFIG_PCI_NV_6_SPACE_TYPE                           0:0 /* C--VF */
NV_CONFIG_PCI_NV_6_SPACE_TYPE           TEXTEQU         <0:0>
;  ----V 
;   114 : #define NV_CONFIG_PCI_NV_6_SPACE_TYPE_MEMORY             0x00000000 /* ----V */
NV_CONFIG_PCI_NV_6_SPACE_TYPE_MEMORY            EQU             000000000h
;  C---V 
;   115 : #define NV_CONFIG_PCI_NV_6_SPACE_TYPE_IO                 0x00000001 /* C---V */
NV_CONFIG_PCI_NV_6_SPACE_TYPE_IO                EQU             000000001h
;  RWXUF 
;   116 : #define NV_CONFIG_PCI_NV_6_BASE_ADDRESS                        31:8 /* RWXUF */
NV_CONFIG_PCI_NV_6_BASE_ADDRESS         TEXTEQU         <31:8>
;  R--4A 
;        
;   117 : #define NV_CONFIG_PCI_NV_7(i)                    (0x0000001C+(i)*4) /* R--4A */
;   118 : #define NV_CONFIG_PCI_NV_7__SIZE_1                                4 /*       */
NV_CONFIG_PCI_NV_7__SIZE_1              EQU             4t
;        
;   119 : #define NV_CONFIG_PCI_NV_7__ALIAS_1                NV_PBUS_PCI_NV_7 /*       */
NV_CONFIG_PCI_NV_7__ALIAS_1             TEXTEQU         <NV_PBUS_PCI_NV_7>
;  C--VF 
;   120 : #define NV_CONFIG_PCI_NV_7_RESERVED                            31:0 /* C--VF */
NV_CONFIG_PCI_NV_7_RESERVED             TEXTEQU         <31:0>
;  C---V 
;   121 : #define NV_CONFIG_PCI_NV_7_RESERVED_0                    0x00000000 /* C---V */
NV_CONFIG_PCI_NV_7_RESERVED_0           EQU             000000000h
;  R--4R 
;   122 : #define NV_CONFIG_PCI_NV_11                              0x0000002C /* R--4R */
NV_CONFIG_PCI_NV_11             EQU             00000002ch
;        
;   123 : #define NV_CONFIG_PCI_NV_11__ALIAS_1              NV_PBUS_PCI_NV_11 /*       */
NV_CONFIG_PCI_NV_11__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_11>
;  R--UF 
;   124 : #define NV_CONFIG_PCI_NV_11_SUBSYSTEM_VENDOR_ID               31:16 /* R--UF */
NV_CONFIG_PCI_NV_11_SUBSYSTEM_VENDOR_ID         TEXTEQU         <31:16>
;  R---V 
;   125 : #define NV_CONFIG_PCI_NV_11_SUBSYSTEM_VENDOR_ID_NONE     0x00000000 /* R---V */
NV_CONFIG_PCI_NV_11_SUBSYSTEM_VENDOR_ID_NONE            EQU             000000000h
;  R--UF 
;   126 : #define NV_CONFIG_PCI_NV_11_SUBSYSTEM_ID                       15:0 /* R--UF */
NV_CONFIG_PCI_NV_11_SUBSYSTEM_ID                TEXTEQU         <15:0>
;  R---V 
;   127 : #define NV_CONFIG_PCI_NV_11_SUBSYSTEM_ID_NONE            0x00000000 /* R---V */
NV_CONFIG_PCI_NV_11_SUBSYSTEM_ID_NONE           EQU             000000000h
;  RW-4R 
;   128 : #define NV_CONFIG_PCI_NV_12                              0x00000030 /* RW-4R */
NV_CONFIG_PCI_NV_12             EQU             000000030h
;        
;   129 : #define NV_CONFIG_PCI_NV_12__ALIAS_1              NV_PBUS_PCI_NV_12 /*       */
NV_CONFIG_PCI_NV_12__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_12>
;  RWIVF 
;   130 : #define NV_CONFIG_PCI_NV_12_ROM_DECODE                          0:0 /* RWIVF */
NV_CONFIG_PCI_NV_12_ROM_DECODE          TEXTEQU         <0:0>
;  RWI-V 
;   131 : #define NV_CONFIG_PCI_NV_12_ROM_DECODE_DISABLED          0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_12_ROM_DECODE_DISABLED         EQU             000000000h
;  RW--V 
;   132 : #define NV_CONFIG_PCI_NV_12_ROM_DECODE_ENABLED           0x00000001 /* RW--V */
NV_CONFIG_PCI_NV_12_ROM_DECODE_ENABLED          EQU             000000001h
;  RWXUF 
;   133 : #define NV_CONFIG_PCI_NV_12_ROM_BASE                          31:22 /* RWXUF */
NV_CONFIG_PCI_NV_12_ROM_BASE            TEXTEQU         <31:22>
;  RW-4R 
;   134 : #define NV_CONFIG_PCI_NV_13                              0x00000034 /* RW-4R */
NV_CONFIG_PCI_NV_13             EQU             000000034h
;        
;   135 : #define NV_CONFIG_PCI_NV_13__ALIAS_1              NV_PBUS_PCI_NV_13 /*       */
NV_CONFIG_PCI_NV_13__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_13>
;  C--VF 
;   136 : #define NV_CONFIG_PCI_NV_13_CAP_PTR                             7:0 /* C--VF */
NV_CONFIG_PCI_NV_13_CAP_PTR             TEXTEQU         <7:0>
;  C---V 
;   137 : #define NV_CONFIG_PCI_NV_13_CAP_PTR_AGP                  0x00000044 /* C---V */
NV_CONFIG_PCI_NV_13_CAP_PTR_AGP         EQU             000000044h
;  R--4A 
;   138 : #define NV_CONFIG_PCI_NV_14                              0x00000038 /* R--4A */
NV_CONFIG_PCI_NV_14             EQU             000000038h
;        
;   139 : #define NV_CONFIG_PCI_NV_14__ALIAS_1              NV_PBUS_PCI_NV_14 /*       */
NV_CONFIG_PCI_NV_14__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_14>
;  C--VF 
;   140 : #define NV_CONFIG_PCI_NV_14_RESERVED                           31:0 /* C--VF */
NV_CONFIG_PCI_NV_14_RESERVED            TEXTEQU         <31:0>
;  C---V 
;   141 : #define NV_CONFIG_PCI_NV_14_RESERVED_0                   0x00000000 /* C---V */
NV_CONFIG_PCI_NV_14_RESERVED_0          EQU             000000000h
;  RW-4R 
;   142 : #define NV_CONFIG_PCI_NV_15                              0x0000003C /* RW-4R */
NV_CONFIG_PCI_NV_15             EQU             00000003ch
;        
;   143 : #define NV_CONFIG_PCI_NV_15__ALIAS_1              NV_PBUS_PCI_NV_15 /*       */
NV_CONFIG_PCI_NV_15__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_15>
;  RWIVF 
;   144 : #define NV_CONFIG_PCI_NV_15_INTR_LINE                           7:0 /* RWIVF */
NV_CONFIG_PCI_NV_15_INTR_LINE           TEXTEQU         <7:0>
;  RWI-V 
;   145 : #define NV_CONFIG_PCI_NV_15_INTR_LINE_IRQ0               0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_15_INTR_LINE_IRQ0              EQU             000000000h
;  RW--V 
;   146 : #define NV_CONFIG_PCI_NV_15_INTR_LINE_IRQ1               0x00000001 /* RW--V */
NV_CONFIG_PCI_NV_15_INTR_LINE_IRQ1              EQU             000000001h
;  RW--V 
;   147 : #define NV_CONFIG_PCI_NV_15_INTR_LINE_IRQ15              0x0000000F /* RW--V */
NV_CONFIG_PCI_NV_15_INTR_LINE_IRQ15             EQU             00000000fh
;  RW--V 
;   148 : #define NV_CONFIG_PCI_NV_15_INTR_LINE_UNKNOWN            0x000000FF /* RW--V */
NV_CONFIG_PCI_NV_15_INTR_LINE_UNKNOWN           EQU             0000000ffh
;  C--VF 
;   149 : #define NV_CONFIG_PCI_NV_15_INTR_PIN                           15:8 /* C--VF */
NV_CONFIG_PCI_NV_15_INTR_PIN            TEXTEQU         <15:8>
;  C---V 
;   150 : #define NV_CONFIG_PCI_NV_15_INTR_PIN_INTA                0x00000001 /* C---V */
NV_CONFIG_PCI_NV_15_INTR_PIN_INTA               EQU             000000001h
;  C--VF 
;   151 : #define NV_CONFIG_PCI_NV_15_MIN_GNT                           23:16 /* C--VF */
NV_CONFIG_PCI_NV_15_MIN_GNT             TEXTEQU         <23:16>
;  ----V 
;   152 : #define NV_CONFIG_PCI_NV_15_MIN_GNT_NO_REQUIREMENTS      0x00000000 /* ----V */
NV_CONFIG_PCI_NV_15_MIN_GNT_NO_REQUIREMENTS             EQU             000000000h
;  C---V 
;   153 : #define NV_CONFIG_PCI_NV_15_MIN_GNT_750NS                0x00000003 /* C---V */
NV_CONFIG_PCI_NV_15_MIN_GNT_750NS               EQU             000000003h
;  C--VF 
;   154 : #define NV_CONFIG_PCI_NV_15_MAX_LAT                           31:24 /* C--VF */
NV_CONFIG_PCI_NV_15_MAX_LAT             TEXTEQU         <31:24>
;  ----V 
;   155 : #define NV_CONFIG_PCI_NV_15_MAX_LAT_NO_REQUIREMENTS      0x00000000 /* ----V */
NV_CONFIG_PCI_NV_15_MAX_LAT_NO_REQUIREMENTS             EQU             000000000h
;  C---V 
;   156 : #define NV_CONFIG_PCI_NV_15_MAX_LAT_250NS                0x00000001 /* C---V */
NV_CONFIG_PCI_NV_15_MAX_LAT_250NS               EQU             000000001h
;  RW-4R 
;   157 : #define NV_CONFIG_PCI_NV_16                              0x00000040 /* RW-4R */
NV_CONFIG_PCI_NV_16             EQU             000000040h
;        
;   158 : #define NV_CONFIG_PCI_NV_16__ALIAS_1              NV_PBUS_PCI_NV_16 /*       */
NV_CONFIG_PCI_NV_16__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_16>
;  RW-VF 
;   159 : #define NV_CONFIG_PCI_NV_16_SUBSYSTEM_VENDOR_ID               31:16 /* RW-VF */
NV_CONFIG_PCI_NV_16_SUBSYSTEM_VENDOR_ID         TEXTEQU         <31:16>
;  R---V 
;   160 : #define NV_CONFIG_PCI_NV_16_SUBSYSTEM_VENDOR_ID_NONE     0x00000000 /* R---V */
NV_CONFIG_PCI_NV_16_SUBSYSTEM_VENDOR_ID_NONE            EQU             000000000h
;  RW-VF 
;   161 : #define NV_CONFIG_PCI_NV_16_SUBSYSTEM_ID                       15:0 /* RW-VF */
NV_CONFIG_PCI_NV_16_SUBSYSTEM_ID                TEXTEQU         <15:0>
;  R---V 
;   162 : #define NV_CONFIG_PCI_NV_16_SUBSYSTEM_ID_NONE            0x00000000 /* R---V */
NV_CONFIG_PCI_NV_16_SUBSYSTEM_ID_NONE           EQU             000000000h
;  RW-4R 
;   163 : #define NV_CONFIG_PCI_NV_17                              0x00000044 /* RW-4R */
NV_CONFIG_PCI_NV_17             EQU             000000044h
;        
;   164 : #define NV_CONFIG_PCI_NV_17__ALIAS_1              NV_PBUS_PCI_NV_17 /*       */
NV_CONFIG_PCI_NV_17__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_17>
;  C--VF 
;   165 : #define NV_CONFIG_PCI_NV_17_AGP_REV_MAJOR                     23:20 /* C--VF */
NV_CONFIG_PCI_NV_17_AGP_REV_MAJOR               TEXTEQU         <23:20>
;  C---V 
;   166 : #define NV_CONFIG_PCI_NV_17_AGP_REV_MAJOR_1              0x00000001 /* C---V */
NV_CONFIG_PCI_NV_17_AGP_REV_MAJOR_1             EQU             000000001h
;  C--VF 
;   167 : #define NV_CONFIG_PCI_NV_17_AGP_REV_MINOR                     19:16 /* C--VF */
NV_CONFIG_PCI_NV_17_AGP_REV_MINOR               TEXTEQU         <19:16>
;  C---V 
;   168 : #define NV_CONFIG_PCI_NV_17_AGP_REV_MINOR_0              0x00000000 /* C---V */
NV_CONFIG_PCI_NV_17_AGP_REV_MINOR_0             EQU             000000000h
;  C--VF 
;   169 : #define NV_CONFIG_PCI_NV_17_NEXT_PTR                           15:8 /* C--VF */
NV_CONFIG_PCI_NV_17_NEXT_PTR            TEXTEQU         <15:8>
;  C---V 
;   170 : #define NV_CONFIG_PCI_NV_17_NEXT_PTR_NULL                0x00000000 /* C---V */
NV_CONFIG_PCI_NV_17_NEXT_PTR_NULL               EQU             000000000h
;  C--VF 
;   171 : #define NV_CONFIG_PCI_NV_17_CAP_ID                              7:0 /* C--VF */
NV_CONFIG_PCI_NV_17_CAP_ID              TEXTEQU         <7:0>
;  C---V 
;   172 : #define NV_CONFIG_PCI_NV_17_CAP_ID_AGP                   0x00000002 /* C---V */
NV_CONFIG_PCI_NV_17_CAP_ID_AGP          EQU             000000002h
;  RW-4R 
;   173 : #define NV_CONFIG_PCI_NV_18                              0x00000048 /* RW-4R */
NV_CONFIG_PCI_NV_18             EQU             000000048h
;        
;   174 : #define NV_CONFIG_PCI_NV_18__ALIAS_1              NV_PBUS_PCI_NV_18 /*       */
NV_CONFIG_PCI_NV_18__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_18>
;  C--VF 
;   175 : #define NV_CONFIG_PCI_NV_18_AGP_STATUS_RQ                     31:24 /* C--VF */
NV_CONFIG_PCI_NV_18_AGP_STATUS_RQ               TEXTEQU         <31:24>
;  C---V 
;   176 : #define NV_CONFIG_PCI_NV_18_AGP_STATUS_RQ_4              0x00000004 /* C---V */
NV_CONFIG_PCI_NV_18_AGP_STATUS_RQ_4             EQU             000000004h
;  C--VF 
;   177 : #define NV_CONFIG_PCI_NV_18_AGP_STATUS_SBA                      9:9 /* C--VF */
NV_CONFIG_PCI_NV_18_AGP_STATUS_SBA              TEXTEQU         <9:9>
;  C---V 
;   178 : #define NV_CONFIG_PCI_NV_18_AGP_STATUS_SBA_NONE          0x00000000 /* C---V */
NV_CONFIG_PCI_NV_18_AGP_STATUS_SBA_NONE         EQU             000000000h
;  C--VF 
;   179 : #define NV_CONFIG_PCI_NV_18_AGP_STATUS_RATE                     1:0 /* C--VF */
NV_CONFIG_PCI_NV_18_AGP_STATUS_RATE             TEXTEQU         <1:0>
;  C---V 
;   180 : #define NV_CONFIG_PCI_NV_18_AGP_STATUS_RATE_1X           0x00000001 /* C---V */
NV_CONFIG_PCI_NV_18_AGP_STATUS_RATE_1X          EQU             000000001h
;  RW-4R 
;   181 : #define NV_CONFIG_PCI_NV_19                              0x0000004C /* RW-4R */
NV_CONFIG_PCI_NV_19             EQU             00000004ch
;        
;   182 : #define NV_CONFIG_PCI_NV_19__ALIAS_1              NV_PBUS_PCI_NV_19 /*       */
NV_CONFIG_PCI_NV_19__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_19>
;  RWIVF 
;   183 : #define NV_CONFIG_PCI_NV_19_AGP_COMMAND_RQ_DEPTH              31:24 /* RWIVF */
NV_CONFIG_PCI_NV_19_AGP_COMMAND_RQ_DEPTH                TEXTEQU         <31:24>
;  RWI-V 
;   184 : #define NV_CONFIG_PCI_NV_19_AGP_COMMAND_RQ_DEPTH_0       0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_19_AGP_COMMAND_RQ_DEPTH_0              EQU             000000000h
;  RWIVF 
;   185 : #define NV_CONFIG_PCI_NV_19_AGP_COMMAND_AGP_ENABLE              8:8 /* RWIVF */
NV_CONFIG_PCI_NV_19_AGP_COMMAND_AGP_ENABLE              TEXTEQU         <8:8>
;  RWI-V 
;   186 : #define NV_CONFIG_PCI_NV_19_AGP_COMMAND_AGP_ENABLE_OFF   0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_19_AGP_COMMAND_AGP_ENABLE_OFF          EQU             000000000h
;  RWIVF 
;   187 : #define NV_CONFIG_PCI_NV_19_AGP_COMMAND_DATA_RATE               1:0 /* RWIVF */
NV_CONFIG_PCI_NV_19_AGP_COMMAND_DATA_RATE               TEXTEQU         <1:0>
;  RWI-V 
;   188 : #define NV_CONFIG_PCI_NV_19_AGP_COMMAND_DATA_RATE_OFF    0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_19_AGP_COMMAND_DATA_RATE_OFF           EQU             000000000h
;  RW-4R 
;   189 : #define NV_CONFIG_PCI_NV_20                              0x00000050 /* RW-4R */
NV_CONFIG_PCI_NV_20             EQU             000000050h
;        
;   190 : #define NV_CONFIG_PCI_NV_20__ALIAS_1              NV_PBUS_PCI_NV_20 /*       */
NV_CONFIG_PCI_NV_20__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_20>
;  RWIVF 
;   191 : #define NV_CONFIG_PCI_NV_20_SOUND_BLASTER                       0:0 /* RWIVF */
NV_CONFIG_PCI_NV_20_SOUND_BLASTER               TEXTEQU         <0:0>
;  RW--V 
;   192 : #define NV_CONFIG_PCI_NV_20_SOUND_BLASTER_DISABLED       0x00000000 /* RW--V */
NV_CONFIG_PCI_NV_20_SOUND_BLASTER_DISABLED              EQU             000000000h
;  RWI-V 
;   193 : #define NV_CONFIG_PCI_NV_20_SOUND_BLASTER_ENABLED        0x00000001 /* RWI-V */
NV_CONFIG_PCI_NV_20_SOUND_BLASTER_ENABLED               EQU             000000001h
;  RWIVF 
;   194 : #define NV_CONFIG_PCI_NV_20_FM_SYNTHESIS                        1:1 /* RWIVF */
NV_CONFIG_PCI_NV_20_FM_SYNTHESIS                TEXTEQU         <1:1>
;  RW--V 
;   195 : #define NV_CONFIG_PCI_NV_20_FM_SYNTHESIS_DISABLED        0x00000000 /* RW--V */
NV_CONFIG_PCI_NV_20_FM_SYNTHESIS_DISABLED               EQU             000000000h
;  RWI-V 
;   196 : #define NV_CONFIG_PCI_NV_20_FM_SYNTHESIS_ENABLED         0x00000001 /* RWI-V */
NV_CONFIG_PCI_NV_20_FM_SYNTHESIS_ENABLED                EQU             000000001h
;  RWIVF 
;   197 : #define NV_CONFIG_PCI_NV_20_GAME_PORT                           2:2 /* RWIVF */
NV_CONFIG_PCI_NV_20_GAME_PORT           TEXTEQU         <2:2>
;  RW--V 
;   198 : #define NV_CONFIG_PCI_NV_20_GAME_PORT_DISABLED           0x00000000 /* RW--V */
NV_CONFIG_PCI_NV_20_GAME_PORT_DISABLED          EQU             000000000h
;  RWI-V 
;   199 : #define NV_CONFIG_PCI_NV_20_GAME_PORT_ENABLED            0x00000001 /* RWI-V */
NV_CONFIG_PCI_NV_20_GAME_PORT_ENABLED           EQU             000000001h
;  RWIVF 
;   200 : #define NV_CONFIG_PCI_NV_20_MPU401_IO                           3:3 /* RWIVF */
NV_CONFIG_PCI_NV_20_MPU401_IO           TEXTEQU         <3:3>
;  RW--V 
;   201 : #define NV_CONFIG_PCI_NV_20_MPU401_IO_DISABLED           0x00000000 /* RW--V */
NV_CONFIG_PCI_NV_20_MPU401_IO_DISABLED          EQU             000000000h
;  RWI-V 
;   202 : #define NV_CONFIG_PCI_NV_20_MPU401_IO_ENABLED            0x00000001 /* RWI-V */
NV_CONFIG_PCI_NV_20_MPU401_IO_ENABLED           EQU             000000001h
;  RWIVF 
;   203 : #define NV_CONFIG_PCI_NV_20_MPU401_IRQ                          4:4 /* RWIVF */
NV_CONFIG_PCI_NV_20_MPU401_IRQ          TEXTEQU         <4:4>
;  RW--V 
;   204 : #define NV_CONFIG_PCI_NV_20_MPU401_IRQ_DISABLED          0x00000000 /* RW--V */
NV_CONFIG_PCI_NV_20_MPU401_IRQ_DISABLED         EQU             000000000h
;  RWI-V 
;   205 : #define NV_CONFIG_PCI_NV_20_MPU401_IRQ_ENABLED           0x00000001 /* RWI-V */
NV_CONFIG_PCI_NV_20_MPU401_IRQ_ENABLED          EQU             000000001h
;  RWIVF 
;   206 : #define NV_CONFIG_PCI_NV_20_IO_ALIASING                         5:5 /* RWIVF */
NV_CONFIG_PCI_NV_20_IO_ALIASING         TEXTEQU         <5:5>
;  RW--V 
;   207 : #define NV_CONFIG_PCI_NV_20_IO_ALIASING_DISABLED         0x00000000 /* RW--V */
NV_CONFIG_PCI_NV_20_IO_ALIASING_DISABLED                EQU             000000000h
;  RWI-V 
;   208 : #define NV_CONFIG_PCI_NV_20_IO_ALIASING_ENABLED          0x00000001 /* RWI-V */
NV_CONFIG_PCI_NV_20_IO_ALIASING_ENABLED         EQU             000000001h
;  RWIVF 
;   209 : #define NV_CONFIG_PCI_NV_20_LEGACY_AUDIO                        7:7 /* RWIVF */
NV_CONFIG_PCI_NV_20_LEGACY_AUDIO                TEXTEQU         <7:7>
;  RW--V 
;   210 : #define NV_CONFIG_PCI_NV_20_LEGACY_AUDIO_ENABLED         0x00000000 /* RW--V */
NV_CONFIG_PCI_NV_20_LEGACY_AUDIO_ENABLED                EQU             000000000h
;  RWI-V 
;   211 : #define NV_CONFIG_PCI_NV_20_LEGACY_AUDIO_DISABLED        0x00000001 /* RWI-V */
NV_CONFIG_PCI_NV_20_LEGACY_AUDIO_DISABLED               EQU             000000001h
;  RWIVF 
;   212 : #define NV_CONFIG_PCI_NV_20_SB_BASE                             9:8 /* RWIVF */
NV_CONFIG_PCI_NV_20_SB_BASE             TEXTEQU         <9:8>
;  RWI-V 
;   213 : #define NV_CONFIG_PCI_NV_20_SB_BASE_220H                 0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_20_SB_BASE_220H                EQU             000000000h
;  RW--V 
;   214 : #define NV_CONFIG_PCI_NV_20_SB_BASE_240H                 0x00000001 /* RW--V */
NV_CONFIG_PCI_NV_20_SB_BASE_240H                EQU             000000001h
;  RW--V 
;   215 : #define NV_CONFIG_PCI_NV_20_SB_BASE_260H                 0x00000002 /* RW--V */
NV_CONFIG_PCI_NV_20_SB_BASE_260H                EQU             000000002h
;  RWIVF 
;   216 : #define NV_CONFIG_PCI_NV_20_MPU_BASE                          13:12 /* RWIVF */
NV_CONFIG_PCI_NV_20_MPU_BASE            TEXTEQU         <13:12>
;  RWI-V 
;   217 : #define NV_CONFIG_PCI_NV_20_MPU_BASE_330H                0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_20_MPU_BASE_330H               EQU             000000000h
;  RW--V 
;   218 : #define NV_CONFIG_PCI_NV_20_MPU_BASE_230H                0x00000001 /* RW--V */
NV_CONFIG_PCI_NV_20_MPU_BASE_230H               EQU             000000001h
;  RW--V 
;   219 : #define NV_CONFIG_PCI_NV_20_MPU_BASE_300H                0x00000002 /* RW--V */
NV_CONFIG_PCI_NV_20_MPU_BASE_300H               EQU             000000002h
;  RWIVF 
;   220 : #define NV_CONFIG_PCI_NV_20_DMA_SNOOP                         16:16 /* RWIVF */
NV_CONFIG_PCI_NV_20_DMA_SNOOP           TEXTEQU         <16:16>
;  RWI-V 
;   221 : #define NV_CONFIG_PCI_NV_20_DMA_SNOOP_DISABLED           0x00000000 /* RWI-V */
NV_CONFIG_PCI_NV_20_DMA_SNOOP_DISABLED          EQU             000000000h
;  RW--V 
;   222 : #define NV_CONFIG_PCI_NV_20_DMA_SNOOP_ENABLED            0x00000001 /* RW--V */
NV_CONFIG_PCI_NV_20_DMA_SNOOP_ENABLED           EQU             000000001h
;  RW-4R 
;   223 : #define NV_CONFIG_PCI_NV_21                              0x00000054 /* RW-4R */
NV_CONFIG_PCI_NV_21             EQU             000000054h
;        
;   224 : #define NV_CONFIG_PCI_NV_21__ALIAS_1              NV_PBUS_PCI_NV_21 /*       */
NV_CONFIG_PCI_NV_21__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_21>
;  RWIVF 
;   225 : #define NV_CONFIG_PCI_NV_21_VGA                                 0:0 /* RWIVF */
NV_CONFIG_PCI_NV_21_VGA         TEXTEQU         <0:0>
;  RW--V 
;   226 : #define NV_CONFIG_PCI_NV_21_VGA_DISABLED                 0x00000000 /* RW--V */
NV_CONFIG_PCI_NV_21_VGA_DISABLED                EQU             000000000h
;  RWI-V 
;   227 : #define NV_CONFIG_PCI_NV_21_VGA_ENABLED                  0x00000001 /* RWI-V */
NV_CONFIG_PCI_NV_21_VGA_ENABLED         EQU             000000001h
;  RW-4R 
;   228 : #define NV_CONFIG_PCI_NV_22                              0x00000058 /* RW-4R */
NV_CONFIG_PCI_NV_22             EQU             000000058h
;        
;   229 : #define NV_CONFIG_PCI_NV_22__ALIAS_1              NV_PBUS_PCI_NV_22 /*       */
NV_CONFIG_PCI_NV_22__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_22>
;  RWIVF 
;   230 : #define NV_CONFIG_PCI_NV_22_SCRATCH                            23:0 /* RWIVF */
NV_CONFIG_PCI_NV_22_SCRATCH             TEXTEQU         <23:0>
;  RWI-V 
;   231 : #define NV_CONFIG_PCI_NV_22_SCRATCH_DEFAULT              0x0023D6CE /* RWI-V */
NV_CONFIG_PCI_NV_22_SCRATCH_DEFAULT             EQU             00023d6ceh
;  R--4A 
;        
;   232 : #define NV_CONFIG_PCI_NV_23(i)                   (0x0000005C+(i)*4) /* R--4A */
;   233 : #define NV_CONFIG_PCI_NV_23__SIZE_1                              41 /*       */
NV_CONFIG_PCI_NV_23__SIZE_1             EQU             41t
;        
;   234 : #define NV_CONFIG_PCI_NV_23__ALIAS_1              NV_PBUS_PCI_NV_23 /*       */
NV_CONFIG_PCI_NV_23__ALIAS_1            TEXTEQU         <NV_PBUS_PCI_NV_23>
;  C--VF 
;   235 : #define NV_CONFIG_PCI_NV_23_RESERVED                           31:0 /* C--VF */
NV_CONFIG_PCI_NV_23_RESERVED            TEXTEQU         <31:0>
;  C---V 
;   236 : #define NV_CONFIG_PCI_NV_23_RESERVED_0                   0x00000000 /* C---V */
NV_CONFIG_PCI_NV_23_RESERVED_0          EQU             000000000h
;  dev_bus.ref 
;  R--4R 
;   237 : /* dev_bus.ref */
;   238 : #define NV_RMA_ID                                        0x00000000 /* R--4R */
NV_RMA_ID               EQU             000000000h
;        
;   239 : #define NV_RMA_ID__ALIAS_1                          NV_PRMIO_RMA_ID /*       */
NV_RMA_ID__ALIAS_1              TEXTEQU         <NV_PRMIO_RMA_ID>
;  C--UF 
;   240 : #define NV_RMA_ID_CODE                                         31:0 /* C--UF */
NV_RMA_ID_CODE          TEXTEQU         <31:0>
;  C---V 
;   241 : #define NV_RMA_ID_CODE_VALID                             0x2B16D065 /* C---V */
NV_RMA_ID_CODE_VALID            EQU             02b16d065h
;  RW-4R 
;   242 : #define NV_RMA_PTR                                       0x00000004 /* RW-4R */
NV_RMA_PTR              EQU             000000004h
;        
;   243 : #define NV_RMA_PTR__ALIAS_1                        NV_PRMIO_RMA_PTR /*       */
NV_RMA_PTR__ALIAS_1             TEXTEQU         <NV_PRMIO_RMA_PTR>
;  RWXUF 
;   244 : #define NV_RMA_PTR_ADDRESS                                     24:2 /* RWXUF */
NV_RMA_PTR_ADDRESS              TEXTEQU         <24:2>
;  RW-4R 
;   245 : #define NV_RMA_DATA                                      0x00000008 /* RW-4R */
NV_RMA_DATA             EQU             000000008h
;        
;   246 : #define NV_RMA_DATA__ALIAS_1                      NV_PRMIO_RMA_DATA /*       */
NV_RMA_DATA__ALIAS_1            TEXTEQU         <NV_PRMIO_RMA_DATA>
;  RWXUF 
;   247 : #define NV_RMA_DATA_PORT                                       31:0 /* RWXUF */
NV_RMA_DATA_PORT                TEXTEQU         <31:0>
;  RW-2R 
;   248 : #define NV_RMA_DATA32                                    0x0000000C /* RW-2R */
NV_RMA_DATA32           EQU             00000000ch
;        
;   249 : #define NV_RMA_DATA32__ALIAS_1                  NV_PRMIO_RMA_DATA32 /*       */
NV_RMA_DATA32__ALIAS_1          TEXTEQU         <NV_PRMIO_RMA_DATA32>
;  -WXUF 
;   250 : #define NV_RMA_DATA32_MSW                                     31:16 /* -WXUF */
NV_RMA_DATA32_MSW               TEXTEQU         <31:16>
;  RWXUF 
;   251 : #define NV_RMA_DATA32_LSW                                      15:0 /* RWXUF */
NV_RMA_DATA32_LSW               TEXTEQU         <15:0>
;  RW-4R 
;   252 : #define NV_RMA_INCDATA                                   0x00000010 /* RW-4R */
NV_RMA_INCDATA          EQU             000000010h
;        
;   253 : #define NV_RMA_INCDATA__ALIAS_1                NV_PRMIO_RMA_INCDATA /*       */
NV_RMA_INCDATA__ALIAS_1         TEXTEQU         <NV_PRMIO_RMA_INCDATA>
;  RWXUF 
;   254 : #define NV_RMA_INCDATA_PORT                                    31:0 /* RWXUF */
NV_RMA_INCDATA_PORT             TEXTEQU         <31:0>
;  RW-4R 
;   255 : #define NV_RMA_INCDATA32                                 0x00000014 /* RW-4R */
NV_RMA_INCDATA32                EQU             000000014h
;        
;   256 : #define NV_RMA_INCDATA32__ALIAS_1            NV_PRMIO_RMA_INCDATA32 /*       */
NV_RMA_INCDATA32__ALIAS_1               TEXTEQU         <NV_PRMIO_RMA_INCDATA32>
;  RWXUF 
;   257 : #define NV_RMA_INCDATA32_PORT                                  31:0 /* RWXUF */
NV_RMA_INCDATA32_PORT           TEXTEQU         <31:0>
;  dev_bus.ref 
;  RW--D 
;   258 : /* dev_bus.ref */
;   259 : #define NV_RMAIO                              0x000000FF:0x00000000 /* RW--D */
NV_RMAIO                TEXTEQU         <0x000000FF:0x00000000>
;  dev_bus.ref 
;  R--4R 
;   260 : /* dev_bus.ref */
;   261 : #define NV_PRMIO_RMA_ID                                  0x00007100 /* R--4R */
NV_PRMIO_RMA_ID         EQU             000007100h
;        
;   262 : #define NV_PRMIO_RMA_ID__ALIAS_1                          NV_RMA_ID /*       */
NV_PRMIO_RMA_ID__ALIAS_1                EQU             000000000h
;  RW-4R 
;   263 : #define NV_PRMIO_RMA_PTR                                 0x00007104 /* RW-4R */
NV_PRMIO_RMA_PTR                EQU             000007104h
;        
;   264 : #define NV_PRMIO_RMA_PTR__ALIAS_1                        NV_RMA_PTR /*       */
NV_PRMIO_RMA_PTR__ALIAS_1               EQU             000000004h
;  RW-4R 
;   265 : #define NV_PRMIO_RMA_DATA                                0x00007108 /* RW-4R */
NV_PRMIO_RMA_DATA               EQU             000007108h
;        
;   266 : #define NV_PRMIO_RMA_DATA__ALIAS_1                      NV_RMA_DATA /*       */
NV_PRMIO_RMA_DATA__ALIAS_1              EQU             000000008h
;  RW-2R 
;   267 : #define NV_PRMIO_RMA_DATA32                              0x0000710C /* RW-2R */
NV_PRMIO_RMA_DATA32             EQU             00000710ch
;        
;   268 : #define NV_PRMIO_RMA_DATA32__ALIAS_1                  NV_RMA_DATA32 /*       */
NV_PRMIO_RMA_DATA32__ALIAS_1            EQU             00000000ch
;  RW-4R 
;   269 : #define NV_PRMIO_RMA_INCDATA                             0x00007110 /* RW-4R */
NV_PRMIO_RMA_INCDATA            EQU             000007110h
;        
;   270 : #define NV_PRMIO_RMA_INCDATA__ALIAS_1                NV_RMA_INCDATA /*       */
NV_PRMIO_RMA_INCDATA__ALIAS_1           EQU             000000010h
;  RW-4R 
;   271 : #define NV_PRMIO_RMA_INCDATA32                           0x00007114 /* RW-4R */
NV_PRMIO_RMA_INCDATA32          EQU             000007114h
;        
;   272 : #define NV_PRMIO_RMA_INCDATA32__ALIAS_1            NV_RMA_INCDATA32 /*       */
NV_PRMIO_RMA_INCDATA32__ALIAS_1         EQU             000000014h
;  dev_bus.ref 
;  RW--D 
;   273 : /* dev_bus.ref */
;   274 : #define NV_MEMORY                             0xFFFFFFFF:0x00000000 /* RW--D */
NV_MEMORY               TEXTEQU         <0xFFFFFFFF:0x00000000>
;  dev_realmode.ref 
;  dev_bus.ref 
;  RW--D 
;   275 : /* dev_realmode.ref */
;   276 : /* dev_bus.ref */
;   277 : #define NV_IO                                 0xFFFFFFFF:0x00000000 /* RW--D */
NV_IO           TEXTEQU         <0xFFFFFFFF:0x00000000>
;  -W-1R 
;   278 : #define NV_IO_DMA_0000                                   0x00000000 /* -W-1R */
NV_IO_DMA_0000          EQU             000000000h
;  -W-UF 
;   279 : #define NV_IO_DMA_0000_ADDRESS                                  7:0 /* -W-UF */
NV_IO_DMA_0000_ADDRESS          TEXTEQU         <7:0>
;  -W-1R 
;   280 : #define NV_IO_DMA_0001                                   0x00000001 /* -W-1R */
NV_IO_DMA_0001          EQU             000000001h
;  -W-UF 
;   281 : #define NV_IO_DMA_0001_COUNT                                    7:0 /* -W-UF */
NV_IO_DMA_0001_COUNT            TEXTEQU         <7:0>
;  -W-1R 
;   282 : #define NV_IO_DMA_0002                                   0x00000002 /* -W-1R */
NV_IO_DMA_0002          EQU             000000002h
;  -W-UF 
;   283 : #define NV_IO_DMA_0002_ADDRESS                                  7:0 /* -W-UF */
NV_IO_DMA_0002_ADDRESS          TEXTEQU         <7:0>
;  -W-1R 
;   284 : #define NV_IO_DMA_0003                                   0x00000003 /* -W-1R */
NV_IO_DMA_0003          EQU             000000003h
;  -W-UF 
;   285 : #define NV_IO_DMA_0003_COUNT                                    7:0 /* -W-UF */
NV_IO_DMA_0003_COUNT            TEXTEQU         <7:0>
;  -W-1R 
;   286 : #define NV_IO_DMA_0004                                   0x00000004 /* -W-1R */
NV_IO_DMA_0004          EQU             000000004h
;  -W-UF 
;   287 : #define NV_IO_DMA_0004_ADDRESS                                  7:0 /* -W-UF */
NV_IO_DMA_0004_ADDRESS          TEXTEQU         <7:0>
;  -W-1R 
;   288 : #define NV_IO_DMA_0005                                   0x00000005 /* -W-1R */
NV_IO_DMA_0005          EQU             000000005h
;  -W-UF 
;   289 : #define NV_IO_DMA_0005_COUNT                                    7:0 /* -W-UF */
NV_IO_DMA_0005_COUNT            TEXTEQU         <7:0>
;  -W-1R 
;   290 : #define NV_IO_DMA_0006                                   0x00000006 /* -W-1R */
NV_IO_DMA_0006          EQU             000000006h
;  -W-UF 
;   291 : #define NV_IO_DMA_0006_ADDRESS                                  7:0 /* -W-UF */
NV_IO_DMA_0006_ADDRESS          TEXTEQU         <7:0>
;  -W-1R 
;   292 : #define NV_IO_DMA_0007                                   0x00000007 /* -W-1R */
NV_IO_DMA_0007          EQU             000000007h
;  -W-UF 
;   293 : #define NV_IO_DMA_0007_COUNT                                    7:0 /* -W-UF */
NV_IO_DMA_0007_COUNT            TEXTEQU         <7:0>
;  -W-1R 
;   294 : #define NV_IO_DMA_0008                                   0x00000008 /* -W-1R */
NV_IO_DMA_0008          EQU             000000008h
;  -W-VF 
;   295 : #define NV_IO_DMA_0008_CONTROLLER                               2:2 /* -W-VF */
NV_IO_DMA_0008_CONTROLLER               TEXTEQU         <2:2>
;  -W--V 
;   296 : #define NV_IO_DMA_0008_CONTROLLER_DISABLED               0x00000000 /* -W--V */
NV_IO_DMA_0008_CONTROLLER_DISABLED              EQU             000000000h
;  -W--V 
;   297 : #define NV_IO_DMA_0008_CONTROLLER_ENABLED                0x00000001 /* -W--V */
NV_IO_DMA_0008_CONTROLLER_ENABLED               EQU             000000001h
;  -W-1R 
;   298 : #define NV_IO_DMA_000A                                   0x0000000A /* -W-1R */
NV_IO_DMA_000A          EQU             00000000ah
;  -W-UF 
;   299 : #define NV_IO_DMA_000A_CHANNEL                                  1:0 /* -W-UF */
NV_IO_DMA_000A_CHANNEL          TEXTEQU         <1:0>
;  -W-VF 
;   300 : #define NV_IO_DMA_000A_MASK_BIT                                 2:2 /* -W-VF */
NV_IO_DMA_000A_MASK_BIT         TEXTEQU         <2:2>
;  -W--V 
;   301 : #define NV_IO_DMA_000A_MASK_BIT_CLEAR                    0x00000000 /* -W--V */
NV_IO_DMA_000A_MASK_BIT_CLEAR           EQU             000000000h
;  -W--V 
;   302 : #define NV_IO_DMA_000A_MASK_BIT_SET                      0x00000001 /* -W--V */
NV_IO_DMA_000A_MASK_BIT_SET             EQU             000000001h
;  -W-1R 
;   303 : #define NV_IO_DMA_000B                                   0x0000000B /* -W-1R */
NV_IO_DMA_000B          EQU             00000000bh
;  -W-UF 
;   304 : #define NV_IO_DMA_000B_CHANNEL                                  1:0 /* -W-UF */
NV_IO_DMA_000B_CHANNEL          TEXTEQU         <1:0>
;  -W-VF 
;   305 : #define NV_IO_DMA_000B_AUTOINITIALIZE                           4:4 /* -W-VF */
NV_IO_DMA_000B_AUTOINITIALIZE           TEXTEQU         <4:4>
;  -W--V 
;   306 : #define NV_IO_DMA_000B_AUTOINITIALIZE_DISABLED           0x00000000 /* -W--V */
NV_IO_DMA_000B_AUTOINITIALIZE_DISABLED          EQU             000000000h
;  -W--V 
;   307 : #define NV_IO_DMA_000B_AUTOINITIALIZE_ENABLED            0x00000001 /* -W--V */
NV_IO_DMA_000B_AUTOINITIALIZE_ENABLED           EQU             000000001h
;  -W-VF 
;   308 : #define NV_IO_DMA_000B_DIRECTION                                5:5 /* -W-VF */
NV_IO_DMA_000B_DIRECTION                TEXTEQU         <5:5>
;  -W--V 
;   309 : #define NV_IO_DMA_000B_DIRECTION_INCREMENT               0x00000000 /* -W--V */
NV_IO_DMA_000B_DIRECTION_INCREMENT              EQU             000000000h
;  -W--V 
;   310 : #define NV_IO_DMA_000B_DIRECTION_DECREMENT               0x00000001 /* -W--V */
NV_IO_DMA_000B_DIRECTION_DECREMENT              EQU             000000001h
;  -W-1R 
;   311 : #define NV_IO_DMA_000C                                   0x0000000C /* -W-1R */
NV_IO_DMA_000C          EQU             00000000ch
;  -W-VF 
;   312 : #define NV_IO_DMA_000C_CLEAR_FLIPFLOP                           7:0 /* -W-VF */
NV_IO_DMA_000C_CLEAR_FLIPFLOP           TEXTEQU         <7:0>
;  -W-1R 
;   313 : #define NV_IO_DMA_0081                                   0x00000081 /* -W-1R */
NV_IO_DMA_0081          EQU             000000081h
;  -W-UF 
;   314 : #define NV_IO_DMA_0081_ADDRESS                                  3:0 /* -W-UF */
NV_IO_DMA_0081_ADDRESS          TEXTEQU         <3:0>
;  -W-1R 
;   315 : #define NV_IO_DMA_0082                                   0x00000082 /* -W-1R */
NV_IO_DMA_0082          EQU             000000082h
;  -W-UF 
;   316 : #define NV_IO_DMA_0082_ADDRESS                                  3:0 /* -W-UF */
NV_IO_DMA_0082_ADDRESS          TEXTEQU         <3:0>
;  -W-1R 
;   317 : #define NV_IO_DMA_0083                                   0x00000083 /* -W-1R */
NV_IO_DMA_0083          EQU             000000083h
;  -W-UF 
;   318 : #define NV_IO_DMA_0083_ADDRESS                                  3:0 /* -W-UF */
NV_IO_DMA_0083_ADDRESS          TEXTEQU         <3:0>
;  -W-1R 
;   319 : #define NV_IO_DMA_0087                                   0x00000087 /* -W-1R */
NV_IO_DMA_0087          EQU             000000087h
;  -W-UF 
;   320 : #define NV_IO_DMA_0087_ADDRESS                                  3:0 /* -W-UF */
NV_IO_DMA_0087_ADDRESS          TEXTEQU         <3:0>
;  dev_dac.ref 
;  RW--D 
;   321 : /* dev_dac.ref */
;   322 : #define NV_PRAMDAC                            0x00680FFF:0x00680300 /* RW--D */
NV_PRAMDAC              TEXTEQU         <0x00680FFF:0x00680300>
;  RW-4R 
;   323 : #define NV_PRAMDAC_CU_START_POS                          0x00680300 /* RW-4R */
NV_PRAMDAC_CU_START_POS         EQU             000680300h
;  RWXSF 
;   324 : #define NV_PRAMDAC_CU_START_POS_X                              11:0 /* RWXSF */
NV_PRAMDAC_CU_START_POS_X               TEXTEQU         <11:0>
;  RWXSF 
;   325 : #define NV_PRAMDAC_CU_START_POS_Y                             27:16 /* RWXSF */
NV_PRAMDAC_CU_START_POS_Y               TEXTEQU         <27:16>
;  RW-4R 
;   326 : #define NV_PRAMDAC_APLL_COEFF                            0x00680500 /* RW-4R */
NV_PRAMDAC_APLL_COEFF           EQU             000680500h
;  RWIUF 
;   327 : #define NV_PRAMDAC_APLL_COEFF_MDIV                              7:0 /* RWIUF */
NV_PRAMDAC_APLL_COEFF_MDIV              TEXTEQU         <7:0>
;  RWIUF 
;   328 : #define NV_PRAMDAC_APLL_COEFF_NDIV                             15:8 /* RWIUF */
NV_PRAMDAC_APLL_COEFF_NDIV              TEXTEQU         <15:8>
;  RWIVF 
;   329 : #define NV_PRAMDAC_APLL_COEFF_PDIV                            18:16 /* RWIVF */
NV_PRAMDAC_APLL_COEFF_PDIV              TEXTEQU         <18:16>
;  RW-4R 
;   330 : #define NV_PRAMDAC_MPLL_COEFF                            0x00680504 /* RW-4R */
NV_PRAMDAC_MPLL_COEFF           EQU             000680504h
;  RWIUF 
;   331 : #define NV_PRAMDAC_MPLL_COEFF_MDIV                              7:0 /* RWIUF */
NV_PRAMDAC_MPLL_COEFF_MDIV              TEXTEQU         <7:0>
;  RWIUF 
;   332 : #define NV_PRAMDAC_MPLL_COEFF_NDIV                             15:8 /* RWIUF */
NV_PRAMDAC_MPLL_COEFF_NDIV              TEXTEQU         <15:8>
;  RWIVF 
;   333 : #define NV_PRAMDAC_MPLL_COEFF_PDIV                            18:16 /* RWIVF */
NV_PRAMDAC_MPLL_COEFF_PDIV              TEXTEQU         <18:16>
;  RW-4R 
;   334 : #define NV_PRAMDAC_VPLL_COEFF                            0x00680508 /* RW-4R */
NV_PRAMDAC_VPLL_COEFF           EQU             000680508h
NV_PRAMDAC_VPLL2_COEFF          EQU             000680520h
;  RWIUF 
;   335 : #define NV_PRAMDAC_VPLL_COEFF_MDIV                              7:0 /* RWIUF */
NV_PRAMDAC_VPLL_COEFF_MDIV              TEXTEQU         <7:0>
;  RWIUF 
;   336 : #define NV_PRAMDAC_VPLL_COEFF_NDIV                             15:8 /* RWIUF */
NV_PRAMDAC_VPLL_COEFF_NDIV              TEXTEQU         <15:8>
;  RWIVF 
;   337 : #define NV_PRAMDAC_VPLL_COEFF_PDIV                            18:16 /* RWIVF */
NV_PRAMDAC_VPLL_COEFF_PDIV              TEXTEQU         <18:16>
;  RW-4R 
;   338 : #define NV_PRAMDAC_PLL_COEFF_SELECT                      0x0068050C /* RW-4R */
NV_PRAMDAC_PLL_COEFF_SELECT             EQU             00068050ch
;  RWIVF 
;   339 : #define NV_PRAMDAC_PLL_COEFF_SELECT_APLL_SOURCE                 0:0 /* RWIVF */
NV_PRAMDAC_PLL_COEFF_SELECT_APLL_SOURCE         TEXTEQU         <0:0>
;  RWI-V 
;   340 : #define NV_PRAMDAC_PLL_COEFF_SELECT_APLL_SOURCE_DEFAULT  0x00000000 /* RWI-V */
NV_PRAMDAC_PLL_COEFF_SELECT_APLL_SOURCE_DEFAULT         EQU             000000000h
;  RW--V 
;   341 : #define NV_PRAMDAC_PLL_COEFF_SELECT_APLL_SOURCE_PROG     0x00000001 /* RW--V */
NV_PRAMDAC_PLL_COEFF_SELECT_APLL_SOURCE_PROG            EQU             000000001h
;  RWIVF 
;   342 : #define NV_PRAMDAC_PLL_COEFF_SELECT_APLL_BYPASS                 4:4 /* RWIVF */
NV_PRAMDAC_PLL_COEFF_SELECT_APLL_BYPASS         TEXTEQU         <4:4>
;  RWI-V 
;   343 : #define NV_PRAMDAC_PLL_COEFF_SELECT_APLL_BYPASS_FALSE    0x00000000 /* RWI-V */
NV_PRAMDAC_PLL_COEFF_SELECT_APLL_BYPASS_FALSE           EQU             000000000h
;  RW--V 
;   344 : #define NV_PRAMDAC_PLL_COEFF_SELECT_APLL_BYPASS_TRUE     0x00000001 /* RW--V */
NV_PRAMDAC_PLL_COEFF_SELECT_APLL_BYPASS_TRUE            EQU             000000001h
;  RWIVF 
;   345 : #define NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_SOURCE                 8:8 /* RWIVF */
NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_SOURCE         TEXTEQU         <8:8>
;  RWI-V 
;   346 : #define NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_SOURCE_DEFAULT  0x00000000 /* RWI-V */
NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_SOURCE_DEFAULT         EQU             000000000h
;  RW--V 
;   347 : #define NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_SOURCE_PROG     0x00000001 /* RW--V */
NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_SOURCE_PROG            EQU             000000001h
;  RWIVF 
;   348 : #define NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_BYPASS               12:12 /* RWIVF */
NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_BYPASS         TEXTEQU         <12:12>
;  RWI-V 
;   349 : #define NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_BYPASS_FALSE    0x00000000 /* RWI-V */
NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_BYPASS_FALSE           EQU             000000000h
;  RW--V 
;   350 : #define NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_BYPASS_TRUE     0x00000001 /* RW--V */
NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_BYPASS_TRUE            EQU             000000001h
;  RWIVF 
;   351 : #define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE               16:16 /* RWIVF */
NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE         TEXTEQU         <16:16>
;  RWI-V 
;   352 : #define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE_DEFAULT  0x00000000 /* RWI-V */
NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE_DEFAULT         EQU             000000000h
;  RW--V 
;   353 : #define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE_PROG     0x00000001 /* RW--V */
NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE_PROG            EQU             000000001h
;  RWIVF 
;   354 : #define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_BYPASS               20:20 /* RWIVF */
NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_BYPASS         TEXTEQU         <20:20>
;  RWI-V 
;   355 : #define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_BYPASS_FALSE    0x00000000 /* RWI-V */
NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_BYPASS_FALSE           EQU             000000000h
;  RW--V 
;   356 : #define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_BYPASS_TRUE     0x00000001 /* RW--V */
NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_BYPASS_TRUE            EQU             000000001h
;  RWIVF 
;   357 : #define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO                28:28 /* RWIVF */
NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO          TEXTEQU         <28:28>
;  RWI-V 
;   358 : #define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO_DB1       0x00000000 /* RWI-V */
NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO_DB1              EQU             000000000h
;  RW--V 
;   359 : #define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO_DB2       0x00000001 /* RW--V */
NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO_DB2              EQU             000000001h
;  RW-4R 
;   360 : #define NV_PRAMDAC_PLL_SETUP_CONTROL                     0x00680510 /* RW-4R */
NV_PRAMDAC_PLL_SETUP_CONTROL            EQU             000680510h
;  RWIVF 
;   361 : #define NV_PRAMDAC_PLL_SETUP_CONTROL_VALUE                     10:0 /* RWIVF */
NV_PRAMDAC_PLL_SETUP_CONTROL_VALUE              TEXTEQU         <10:0>
;  RWI-V 
;   362 : #define NV_PRAMDAC_PLL_SETUP_CONTROL_VAL                 0x0000044E /* RWI-V */
NV_PRAMDAC_PLL_SETUP_CONTROL_VAL                EQU             00000044eh
;  RWIVF 
;   363 : #define NV_PRAMDAC_PLL_SETUP_CONTROL_PWRDWN                   12:12 /* RWIVF */
NV_PRAMDAC_PLL_SETUP_CONTROL_PWRDWN             TEXTEQU         <12:12>
;  RWI-V 
;   364 : #define NV_PRAMDAC_PLL_SETUP_CONTROL_PWRDWN_ON           0x00000000 /* RWI-V */
NV_PRAMDAC_PLL_SETUP_CONTROL_PWRDWN_ON          EQU             000000000h
;  RW--V 
;   365 : #define NV_PRAMDAC_PLL_SETUP_CONTROL_PWRDWN_OFF          0x00000001 /* RW--V */
NV_PRAMDAC_PLL_SETUP_CONTROL_PWRDWN_OFF         EQU             000000001h
;  RW-4R 
;   366 : #define NV_PRAMDAC_PLL_TEST_COUNTER                      0x00680514 /* RW-4R */
NV_PRAMDAC_PLL_TEST_COUNTER             EQU             000680514h
;  -WIVF 
;   367 : #define NV_PRAMDAC_PLL_TEST_COUNTER_NOOFIPCLKS                  9:0 /* -WIVF */
NV_PRAMDAC_PLL_TEST_COUNTER_NOOFIPCLKS          TEXTEQU         <9:0>
;  R--VF 
;   368 : #define NV_PRAMDAC_PLL_TEST_COUNTER_VALUE                      15:0 /* R--VF */
NV_PRAMDAC_PLL_TEST_COUNTER_VALUE               TEXTEQU         <15:0>
;  RWIVF 
;   369 : #define NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE                    16:16 /* RWIVF */
NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE              TEXTEQU         <16:16>
;  RWI-V 
;   370 : #define NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE_DEASSERTED    0x00000000 /* RWI-V */
NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE_DEASSERTED           EQU             000000000h
;  RW--V 
;   371 : #define NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE_ASSERTED      0x00000001 /* RW--V */
NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE_ASSERTED             EQU             000000001h
;  RWIVF 
;   372 : #define NV_PRAMDAC_PLL_TEST_COUNTER_RESET                     20:20 /* RWIVF */
NV_PRAMDAC_PLL_TEST_COUNTER_RESET               TEXTEQU         <20:20>
;  RWI-V 
;   373 : #define NV_PRAMDAC_PLL_TEST_COUNTER_RESET_DEASSERTED     0x00000000 /* RWI-V */
NV_PRAMDAC_PLL_TEST_COUNTER_RESET_DEASSERTED            EQU             000000000h
;  RW--V 
;   374 : #define NV_PRAMDAC_PLL_TEST_COUNTER_RESET_ASSERTED       0x00000001 /* RW--V */
NV_PRAMDAC_PLL_TEST_COUNTER_RESET_ASSERTED              EQU             000000001h
;  RWIVF 
;   375 : #define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE                    25:24 /* RWIVF */
NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE              TEXTEQU         <25:24>
;  RWI-V 
;   376 : #define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_MCLK          0x00000000 /* RWI-V */
NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_MCLK         EQU             000000000h
;  RW--V 
;   377 : #define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_VCLK          0x00000001 /* RW--V */
NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_VCLK         EQU             000000001h
;  RW--V 
;   378 : #define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_ACLK          0x00000010 /* RW--V */
NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_ACLK         EQU             000000010h
;  RWIVF 
;   379 : #define NV_PRAMDAC_PLL_TEST_COUNTER_PDIV_RST                  28:28 /* RWIVF */
NV_PRAMDAC_PLL_TEST_COUNTER_PDIV_RST            TEXTEQU         <28:28>
;  RWI-V 
;   380 : #define NV_PRAMDAC_PLL_TEST_COUNTER_PDIVRST_DEASSERTED   0x00000000 /* RWI-V */
NV_PRAMDAC_PLL_TEST_COUNTER_PDIVRST_DEASSERTED          EQU             000000000h
;  RW--V 
;   381 : #define NV_PRAMDAC_PLL_TEST_COUNTER_PDIVRST_ASSERTED     0x00000001 /* RW--V */
NV_PRAMDAC_PLL_TEST_COUNTER_PDIVRST_ASSERTED            EQU             000000001h
;  R--VF 
;   382 : #define NV_PRAMDAC_PLL_TEST_COUNTER_APLL_LOCK                 29:29 /* R--VF */
NV_PRAMDAC_PLL_TEST_COUNTER_APLL_LOCK           TEXTEQU         <29:29>
;  R---V 
;   383 : #define NV_PRAMDAC_PLL_TEST_COUNTER_APLL_NOTLOCKED       0x00000000 /* R---V */
NV_PRAMDAC_PLL_TEST_COUNTER_APLL_NOTLOCKED              EQU             000000000h
;  R---V 
;   384 : #define NV_PRAMDAC_PLL_TEST_COUNTER_APLL_LOCKED          0x00000001 /* R---V */
NV_PRAMDAC_PLL_TEST_COUNTER_APLL_LOCKED         EQU             000000001h
;  R--VF 
;   385 : #define NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCK                 30:30 /* R--VF */
NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCK           TEXTEQU         <30:30>
;  R---V 
;   386 : #define NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_NOTLOCKED       0x00000000 /* R---V */
NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_NOTLOCKED              EQU             000000000h
;  R---V 
;   387 : #define NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCKED          0x00000001 /* R---V */
NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCKED         EQU             000000001h
;  R--VF 
;   388 : #define NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCK                 31:31 /* R--VF */
NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCK           TEXTEQU         <31:31>
;  R---V 
;   389 : #define NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_NOTLOCKED       0x00000000 /* R---V */
NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_NOTLOCKED              EQU             000000000h
;  R---V 
;   390 : #define NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCKED          0x00000001 /* R---V */
NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCKED         EQU             000000001h
;  RW-4R 
;   391 : #define NV_PRAMDAC_PALETTE_TEST                          0x00680518 /* RW-4R */
NV_PRAMDAC_PALETTE_TEST         EQU             000680518h
;  R--VF 
;   392 : #define NV_PRAMDAC_PALETTE_TEST_BLUE_DATA                       7:0 /* R--VF */
NV_PRAMDAC_PALETTE_TEST_BLUE_DATA               TEXTEQU         <7:0>
;  R--VF 
;   393 : #define NV_PRAMDAC_PALETTE_TEST_GREEN_DATA                     15:8 /* R--VF */
NV_PRAMDAC_PALETTE_TEST_GREEN_DATA              TEXTEQU         <15:8>
;  R--VF 
;   394 : #define NV_PRAMDAC_PALETTE_TEST_RED_DATA                      23:16 /* R--VF */
NV_PRAMDAC_PALETTE_TEST_RED_DATA                TEXTEQU         <23:16>
;  RWIVF 
;   395 : #define NV_PRAMDAC_PALETTE_TEST_MODE                          24:24 /* RWIVF */
NV_PRAMDAC_PALETTE_TEST_MODE            TEXTEQU         <24:24>
;  RWI-V 
;   396 : #define NV_PRAMDAC_PALETTE_TEST_MODE_8BIT                0x00000000 /* RWI-V */
NV_PRAMDAC_PALETTE_TEST_MODE_8BIT               EQU             000000000h
;  RW--V 
;   397 : #define NV_PRAMDAC_PALETTE_TEST_MODE_24BIT               0x00000001 /* RW--V */
NV_PRAMDAC_PALETTE_TEST_MODE_24BIT              EQU             000000001h
;  RWIVF 
;   398 : #define NV_PRAMDAC_PALETTE_TEST_ADDRINC                       28:28 /* RWIVF */
NV_PRAMDAC_PALETTE_TEST_ADDRINC         TEXTEQU         <28:28>
;  RWI-V 
;   399 : #define NV_PRAMDAC_PALETTE_TEST_ADDRINC_READWRITE        0x00000000 /* RWI-V */
NV_PRAMDAC_PALETTE_TEST_ADDRINC_READWRITE               EQU             000000000h
;  RW--V 
;   400 : #define NV_PRAMDAC_PALETTE_TEST_ADDRINC_WRITEONLY        0x00000001 /* RW--V */
NV_PRAMDAC_PALETTE_TEST_ADDRINC_WRITEONLY               EQU             000000001h
;  RW-4R 
;   401 : #define NV_PRAMDAC_GENERAL_CONTROL                       0x00680600 /* RW-4R */
NV_PRAMDAC_GENERAL_CONTROL              EQU             000680600h
;  RWIVF 
;   402 : #define NV_PRAMDAC_GENERAL_CONTROL_FF_COEFF                     1:0 /* RWIVF */
NV_PRAMDAC_GENERAL_CONTROL_FF_COEFF             TEXTEQU         <1:0>
;  RWI-V 
;   403 : #define NV_PRAMDAC_GENERAL_CONTROL_FF_COEFF_DEF          0x00000000 /* RWI-V */
NV_PRAMDAC_GENERAL_CONTROL_FF_COEFF_DEF         EQU             000000000h
;  RWIVF 
;   404 : #define NV_PRAMDAC_GENERAL_CONTROL_IDC_MODE                     4:4 /* RWIVF */
NV_PRAMDAC_GENERAL_CONTROL_IDC_MODE             TEXTEQU         <4:4>
;  RWI-V 
;   405 : #define NV_PRAMDAC_GENERAL_CONTROL_IDC_MODE_GAMMA        0x00000000 /* RWI-V */
NV_PRAMDAC_GENERAL_CONTROL_IDC_MODE_GAMMA               EQU             000000000h
;  RW--V 
;   406 : #define NV_PRAMDAC_GENERAL_CONTROL_IDC_MODE_INDEX        0x00000001 /* RW--V */
NV_PRAMDAC_GENERAL_CONTROL_IDC_MODE_INDEX               EQU             000000001h
;  RWIVF 
;   407 : #define NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE                    8:8 /* RWIVF */
NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE            TEXTEQU         <8:8>
;  RWI-V 
;   408 : #define NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE_NOTSE       0x00000000 /* RWI-V */
NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE_NOTSE              EQU             000000000h
;  RW--V 
;   409 : #define NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE_SEL         0x00000001 /* RW--V */
NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE_SEL                EQU             000000001h
;  RWIVF 
;   410 : #define NV_PRAMDAC_GENERAL_CONTROL_565_MODE                   12:12 /* RWIVF */
NV_PRAMDAC_GENERAL_CONTROL_565_MODE             TEXTEQU         <12:12>
;  RWI-V 
;   411 : #define NV_PRAMDAC_GENERAL_CONTROL_565_MODE_NOTSEL       0x00000000 /* RWI-V */
NV_PRAMDAC_GENERAL_CONTROL_565_MODE_NOTSEL              EQU             000000000h
;  RW--V 
;   412 : #define NV_PRAMDAC_GENERAL_CONTROL_565_MODE_SEL          0x00000001 /* RW--V */
NV_PRAMDAC_GENERAL_CONTROL_565_MODE_SEL         EQU             000000001h
;  RWIVF 
;   413 : #define NV_PRAMDAC_GENERAL_CONTROL_BLK_PEDSTL                 16:16 /* RWIVF */
NV_PRAMDAC_GENERAL_CONTROL_BLK_PEDSTL           TEXTEQU         <16:16>
;  RWI-V 
;   414 : #define NV_PRAMDAC_GENERAL_CONTROL_BLK_PEDSTL_OFF        0x00000000 /* RWI-V */
NV_PRAMDAC_GENERAL_CONTROL_BLK_PEDSTL_OFF               EQU             000000000h
;  RW--V 
;   415 : #define NV_PRAMDAC_GENERAL_CONTROL_BLK_PEDSTL_ON         0x00000001 /* RW--V */
NV_PRAMDAC_GENERAL_CONTROL_BLK_PEDSTL_ON                EQU             000000001h
;  RWIVF 
;   416 : #define NV_PRAMDAC_GENERAL_CONTROL_TERMINATION                17:17 /* RWIVF */
NV_PRAMDAC_GENERAL_CONTROL_TERMINATION          TEXTEQU         <17:17>
;  RWI-V 
;   417 : #define NV_PRAMDAC_GENERAL_CONTROL_TERMINATION_37OHM     0x00000000 /* RWI-V */
NV_PRAMDAC_GENERAL_CONTROL_TERMINATION_37OHM            EQU             000000000h
;  RW--V 
;   418 : #define NV_PRAMDAC_GENERAL_CONTROL_TERMINATION_75OHM     0x00000001 /* RW--V */
NV_PRAMDAC_GENERAL_CONTROL_TERMINATION_75OHM            EQU             000000001h
;  RWIVF 
;   419 : #define NV_PRAMDAC_GENERAL_CONTROL_BPC                        20:20 /* RWIVF */
NV_PRAMDAC_GENERAL_CONTROL_BPC          TEXTEQU         <20:20>
;  RWI-V 
;   420 : #define NV_PRAMDAC_GENERAL_CONTROL_BPC_6BITS             0x00000000 /* RWI-V */
NV_PRAMDAC_GENERAL_CONTROL_BPC_6BITS            EQU             000000000h
;  RW--V 
;   421 : #define NV_PRAMDAC_GENERAL_CONTROL_BPC_8BITS             0x00000001 /* RW--V */
NV_PRAMDAC_GENERAL_CONTROL_BPC_8BITS            EQU             000000001h
;  RWIVF 
;   422 : #define NV_PRAMDAC_GENERAL_CONTROL_DAC_SLEEP                  24:24 /* RWIVF */
NV_PRAMDAC_GENERAL_CONTROL_DAC_SLEEP            TEXTEQU         <24:24>
;  RWI-V 
;   423 : #define NV_PRAMDAC_GENERAL_CONTROL_DAC_SLEEP_DIS         0x00000000 /* RWI-V */
NV_PRAMDAC_GENERAL_CONTROL_DAC_SLEEP_DIS                EQU             000000000h
;  RW--V 
;   424 : #define NV_PRAMDAC_GENERAL_CONTROL_DAC_SLEEP_EN          0x00000001 /* RW--V */
NV_PRAMDAC_GENERAL_CONTROL_DAC_SLEEP_EN         EQU             000000001h
;  RWIVF 
;   425 : #define NV_PRAMDAC_GENERAL_CONTROL_PALETTE_CLK                28:28 /* RWIVF */
NV_PRAMDAC_GENERAL_CONTROL_PALETTE_CLK          TEXTEQU         <28:28>
;  RWI-V 
;   426 : #define NV_PRAMDAC_GENERAL_CONTROL_PALETTE_CLK_EN        0x00000000 /* RWI-V */
NV_PRAMDAC_GENERAL_CONTROL_PALETTE_CLK_EN               EQU             000000000h
;  RW--V 
;   427 : #define NV_PRAMDAC_GENERAL_CONTROL_PALETTE_CLK_DIS       0x00000001 /* RW--V */
NV_PRAMDAC_GENERAL_CONTROL_PALETTE_CLK_DIS              EQU             000000001h
;  R--4R 
;   428 : #define NV_PRAMDAC_PALETTE_RECOVERY                      0x00680604 /* R--4R */
NV_PRAMDAC_PALETTE_RECOVERY             EQU             000680604h
;  R--UF 
;   429 : #define NV_PRAMDAC_PALETTE_RECOVERY_ACTIVE_ADDRESS              7:0 /* R--UF */
NV_PRAMDAC_PALETTE_RECOVERY_ACTIVE_ADDRESS              TEXTEQU         <7:0>
;  R--VF 
;   430 : #define NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER                10:8 /* R--VF */
NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER         TEXTEQU         <10:8>
;  R---V 
;   431 : #define NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER_RED      0x00000001 /* R---V */
NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER_RED             EQU             000000001h
;  R---V 
;   432 : #define NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER_GREEN    0x00000010 /* R---V */
NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER_GREEN           EQU             000000010h
;  R---V 
;   433 : #define NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER_BLUE     0x00000100 /* R---V */
NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER_BLUE            EQU             000000100h
;  R--VF 
;   434 : #define NV_PRAMDAC_PALETTE_RECOVERY_DAC_STATE                 13:12 /* R--VF */
NV_PRAMDAC_PALETTE_RECOVERY_DAC_STATE           TEXTEQU         <13:12>
;  R---V 
;   435 : #define NV_PRAMDAC_PALETTE_RECOVERY_DAC_STATE_WRITE      0x00000000 /* R---V */
NV_PRAMDAC_PALETTE_RECOVERY_DAC_STATE_WRITE             EQU             000000000h
;  R---V 
;   436 : #define NV_PRAMDAC_PALETTE_RECOVERY_DAC_STATE_READ       0x00000011 /* R---V */
NV_PRAMDAC_PALETTE_RECOVERY_DAC_STATE_READ              EQU             000000011h
;  R--VF 
;   437 : #define NV_PRAMDAC_PALETTE_RECOVERY_RED_DATA                  23:16 /* R--VF */
NV_PRAMDAC_PALETTE_RECOVERY_RED_DATA            TEXTEQU         <23:16>
;  R--VF 
;   438 : #define NV_PRAMDAC_PALETTE_RECOVERY_GREEN_DATA                31:24 /* R--VF */
NV_PRAMDAC_PALETTE_RECOVERY_GREEN_DATA          TEXTEQU         <31:24>
;  RW-4R 
;   439 : #define NV_PRAMDAC_TEST_CONTROL                          0x00680608 /* RW-4R */
NV_PRAMDAC_TEST_CONTROL         EQU             000680608h
;  RWIVF 
;   440 : #define NV_PRAMDAC_TEST_CONTROL_CRC_RESET                       0:0 /* RWIVF */
NV_PRAMDAC_TEST_CONTROL_CRC_RESET               TEXTEQU         <0:0>
;  RWI-V 
;   441 : #define NV_PRAMDAC_TEST_CONTROL_CRC_RESET_DEASSERTED     0x00000000 /* RWI-V */
NV_PRAMDAC_TEST_CONTROL_CRC_RESET_DEASSERTED            EQU             000000000h
;  RW--V 
;   442 : #define NV_PRAMDAC_TEST_CONTROL_CRC_RESET_ASSERTED       0x00000001 /* RW--V */
NV_PRAMDAC_TEST_CONTROL_CRC_RESET_ASSERTED              EQU             000000001h
;  RWIVF 
;   443 : #define NV_PRAMDAC_TEST_CONTROL_CRC_ENABLE                      4:4 /* RWIVF */
NV_PRAMDAC_TEST_CONTROL_CRC_ENABLE              TEXTEQU         <4:4>
;  RWI-V 
;   444 : #define NV_PRAMDAC_TEST_CONTROL_CRC_ENABLE_DEASSERTED    0x00000000 /* RWI-V */
NV_PRAMDAC_TEST_CONTROL_CRC_ENABLE_DEASSERTED           EQU             000000000h
;  RW--V 
;   445 : #define NV_PRAMDAC_TEST_CONTROL_CRC_ENABLE_ASSERTED      0x00000001 /* RW--V */
NV_PRAMDAC_TEST_CONTROL_CRC_ENABLE_ASSERTED             EQU             000000001h
;  RWIVF 
;   446 : #define NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL                     9:8 /* RWIVF */
NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL             TEXTEQU         <9:8>
;  RWI-V 
;   447 : #define NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL_BLUE         0x00000000 /* RWI-V */
NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL_BLUE                EQU             000000000h
;  RW--V 
;   448 : #define NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL_GREEN        0x00000001 /* RW--V */
NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL_GREEN               EQU             000000001h
;  RW--V 
;   449 : #define NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL_RED          0x00000010 /* RW--V */
NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL_RED         EQU             000000010h
;  RWIVF 
;   450 : #define NV_PRAMDAC_TEST_CONTROL_TP_INS_EN                     12:12 /* RWIVF */
NV_PRAMDAC_TEST_CONTROL_TP_INS_EN               TEXTEQU         <12:12>
;  RWI-V 
;   451 : #define NV_PRAMDAC_TEST_CONTROL_TP_INS_EN_DEASSERTED     0x00000000 /* RWI-V */
NV_PRAMDAC_TEST_CONTROL_TP_INS_EN_DEASSERTED            EQU             000000000h
;  RW--V 
;   452 : #define NV_PRAMDAC_TEST_CONTROL_TP_INS_EN_ASSERTED       0x00000001 /* RW--V */
NV_PRAMDAC_TEST_CONTROL_TP_INS_EN_ASSERTED              EQU             000000001h
;  RWIVF 
;   453 : #define NV_PRAMDAC_TEST_CONTROL_PWRDWN_DAC                    16:16 /* RWIVF */
NV_PRAMDAC_TEST_CONTROL_PWRDWN_DAC              TEXTEQU         <16:16>
;  RWI-V 
;   454 : #define NV_PRAMDAC_TEST_CONTROL_PWRDWN_DAC_ON            0x00000000 /* RWI-V */
NV_PRAMDAC_TEST_CONTROL_PWRDWN_DAC_ON           EQU             000000000h
;  RW--V 
;   455 : #define NV_PRAMDAC_TEST_CONTROL_PWRDWN_DAC_OFF           0x00000001 /* RW--V */
NV_PRAMDAC_TEST_CONTROL_PWRDWN_DAC_OFF          EQU             000000001h
;  RWIVF 
;   456 : #define NV_PRAMDAC_TEST_CONTROL_DACTM                         20:20 /* RWIVF */
NV_PRAMDAC_TEST_CONTROL_DACTM           TEXTEQU         <20:20>
;  RWI-V 
;   457 : #define NV_PRAMDAC_TEST_CONTROL_DACTM_NORMAL             0x00000000 /* RWI-V */
NV_PRAMDAC_TEST_CONTROL_DACTM_NORMAL            EQU             000000000h
;  RW--V 
;   458 : #define NV_PRAMDAC_TEST_CONTROL_DACTM_TEST               0x00000001 /* RW--V */
NV_PRAMDAC_TEST_CONTROL_DACTM_TEST              EQU             000000001h
;  RWIVF 
;   459 : #define NV_PRAMDAC_TEST_CONTROL_TPATH1                        24:24 /* RWIVF */
NV_PRAMDAC_TEST_CONTROL_TPATH1          TEXTEQU         <24:24>
;  RWI-V 
;   460 : #define NV_PRAMDAC_TEST_CONTROL_TPATH1_CLEAR             0x00000000 /* RWI-V */
NV_PRAMDAC_TEST_CONTROL_TPATH1_CLEAR            EQU             000000000h
;  RW--V 
;   461 : #define NV_PRAMDAC_TEST_CONTROL_TPATH1_SET               0x00000001 /* RW--V */
NV_PRAMDAC_TEST_CONTROL_TPATH1_SET              EQU             000000001h
;  RWIVF 
;   462 : #define NV_PRAMDAC_TEST_CONTROL_TPATH31                       25:25 /* RWIVF */
NV_PRAMDAC_TEST_CONTROL_TPATH31         TEXTEQU         <25:25>
;  RWI-V 
;   463 : #define NV_PRAMDAC_TEST_CONTROL_TPATH31_CLEAR            0x00000000 /* RWI-V */
NV_PRAMDAC_TEST_CONTROL_TPATH31_CLEAR           EQU             000000000h
;  RW--V 
;   464 : #define NV_PRAMDAC_TEST_CONTROL_TPATH31_SET              0x00000001 /* RW--V */
NV_PRAMDAC_TEST_CONTROL_TPATH31_SET             EQU             000000001h
;  R--VF 
;   465 : #define NV_PRAMDAC_TEST_CONTROL_SENSEB                        28:28 /* R--VF */
NV_PRAMDAC_TEST_CONTROL_SENSEB          TEXTEQU         <28:28>
;  R---V 
;   466 : #define NV_PRAMDAC_TEST_CONTROL_SENSEB_SOMELO            0x00000000 /* R---V */
NV_PRAMDAC_TEST_CONTROL_SENSEB_SOMELO           EQU             000000000h
;  R---V 
;   467 : #define NV_PRAMDAC_TEST_CONTROL_SENSEB_ALLHI             0x00000001 /* R---V */
NV_PRAMDAC_TEST_CONTROL_SENSEB_ALLHI            EQU             000000001h
;  R--4R 
;   468 : #define NV_PRAMDAC_CHECKSUM                              0x0068060C /* R--4R */
NV_PRAMDAC_CHECKSUM             EQU             00068060ch
;  R--VF 
;   469 : #define NV_PRAMDAC_CHECKSUM_VALUE                              23:0 /* R--VF */
NV_PRAMDAC_CHECKSUM_VALUE               TEXTEQU         <23:0>
;  -W-4R 
;   470 : #define NV_PRAMDAC_TESTPOINT_DATA                        0x00680610 /* -W-4R */
NV_PRAMDAC_TESTPOINT_DATA               EQU             000680610h
;  -W-VF 
;   471 : #define NV_PRAMDAC_TESTPOINT_DATA_RED                           9:0 /* -W-VF */
NV_PRAMDAC_TESTPOINT_DATA_RED           TEXTEQU         <9:0>
;  -W-VF 
;   472 : #define NV_PRAMDAC_TESTPOINT_DATA_GREEN                       19:10 /* -W-VF */
NV_PRAMDAC_TESTPOINT_DATA_GREEN         TEXTEQU         <19:10>
;  -W-VF 
;   473 : #define NV_PRAMDAC_TESTPOINT_DATA_BLUE                        29:20 /* -W-VF */
NV_PRAMDAC_TESTPOINT_DATA_BLUE          TEXTEQU         <29:20>
;  -W-VF 
;   474 : #define NV_PRAMDAC_TESTPOINT_DATA_BLACK                       30:30 /* -W-VF */
NV_PRAMDAC_TESTPOINT_DATA_BLACK         TEXTEQU         <30:30>
;  -W-VF 
;   475 : #define NV_PRAMDAC_TESTPOINT_DATA_NOTBLANK                    31:31 /* -W-VF */
NV_PRAMDAC_TESTPOINT_DATA_NOTBLANK              TEXTEQU         <31:31>
;  RW-4R 
;   476 : #define NV_PRAMDAC_VSERR_WIDTH                           0x00680700 /* RW-4R */
NV_PRAMDAC_VSERR_WIDTH          EQU             000680700h
;  RWIVF 
;   477 : #define NV_PRAMDAC_VSERR_WIDTH_VAL                             10:0 /* RWIVF */
NV_PRAMDAC_VSERR_WIDTH_VAL              TEXTEQU         <10:0>
;  RW-4R 
;   478 : #define NV_PRAMDAC_VEQU_END                              0x00680704 /* RW-4R */
NV_PRAMDAC_VEQU_END             EQU             000680704h
;  RWIVF 
;   479 : #define NV_PRAMDAC_VEQU_END_VAL                                10:0 /* RWIVF */
NV_PRAMDAC_VEQU_END_VAL         TEXTEQU         <10:0>
;  RW-4R 
;   480 : #define NV_PRAMDAC_VBBLANK_END                           0x00680708 /* RW-4R */
NV_PRAMDAC_VBBLANK_END          EQU             000680708h
;  RWIVF 
;   481 : #define NV_PRAMDAC_VBBLANK_END_VAL                             10:0 /* RWIVF */
NV_PRAMDAC_VBBLANK_END_VAL              TEXTEQU         <10:0>
;  RW-4R 
;   482 : #define NV_PRAMDAC_VBLANK_END                            0x0068070C /* RW-4R */
NV_PRAMDAC_VBLANK_END           EQU             00068070ch
;  RWIVF 
;   483 : #define NV_PRAMDAC_VBLANK_END_VAL                              10:0 /* RWIVF */
NV_PRAMDAC_VBLANK_END_VAL               TEXTEQU         <10:0>
;  RW-4R 
;   484 : #define NV_PRAMDAC_VBLANK_START                          0x00680710 /* RW-4R */
NV_PRAMDAC_VBLANK_START         EQU             000680710h
;  RWIVF 
;   485 : #define NV_PRAMDAC_VBLANK_START_VAL                            10:0 /* RWIVF */
NV_PRAMDAC_VBLANK_START_VAL             TEXTEQU         <10:0>
;  RW-4R 
;   486 : #define NV_PRAMDAC_VBBLANK_START                         0x00680714 /* RW-4R */
NV_PRAMDAC_VBBLANK_START                EQU             000680714h
;  RWIVF 
;   487 : #define NV_PRAMDAC_VBBLANK_START_VAL                           10:0 /* RWIVF */
NV_PRAMDAC_VBBLANK_START_VAL            TEXTEQU         <10:0>
;  RW-4R 
;   488 : #define NV_PRAMDAC_VEQU_START                            0x00680718 /* RW-4R */
NV_PRAMDAC_VEQU_START           EQU             000680718h
;  RWIVF 
;   489 : #define NV_PRAMDAC_VEQU_START_VAL                              10:0 /* RWIVF */
NV_PRAMDAC_VEQU_START_VAL               TEXTEQU         <10:0>
;  RW-4R 
;   490 : #define NV_PRAMDAC_VTOTAL                                0x0068071C /* RW-4R */
NV_PRAMDAC_VTOTAL               EQU             00068071ch
;  RWIVF 
;   491 : #define NV_PRAMDAC_VTOTAL_VAL                                  10:0 /* RWIVF */
NV_PRAMDAC_VTOTAL_VAL           TEXTEQU         <10:0>
;  RW-4R 
;   492 : #define NV_PRAMDAC_HSYNC_WIDTH                           0x00680720 /* RW-4R */
NV_PRAMDAC_HSYNC_WIDTH          EQU             000680720h
;  RWIVF 
;   493 : #define NV_PRAMDAC_HSYNC_WIDTH_VAL                             10:0 /* RWIVF */
NV_PRAMDAC_HSYNC_WIDTH_VAL              TEXTEQU         <10:0>
;  RW-4R 
;   494 : #define NV_PRAMDAC_HBURST_START                          0x00680724 /* RW-4R */
NV_PRAMDAC_HBURST_START         EQU             000680724h
;  RWIVF 
;   495 : #define NV_PRAMDAC_HBURST_START_VAL                            10:0 /* RWIVF */
NV_PRAMDAC_HBURST_START_VAL             TEXTEQU         <10:0>
;  RW-4R 
;   496 : #define NV_PRAMDAC_HBURST_END                            0x00680728 /* RW-4R */
NV_PRAMDAC_HBURST_END           EQU             000680728h
;  RWIVF 
;   497 : #define NV_PRAMDAC_HBURST_END_VAL                              10:0 /* RWIVF */
NV_PRAMDAC_HBURST_END_VAL               TEXTEQU         <10:0>
;  RW-4R 
;   498 : #define NV_PRAMDAC_HBLANK_START                          0x0068072C /* RW-4R */
NV_PRAMDAC_HBLANK_START         EQU             00068072ch
;  RWIVF 
;   499 : #define NV_PRAMDAC_HBLANK_START_VAL                            10:0 /* RWIVF */
NV_PRAMDAC_HBLANK_START_VAL             TEXTEQU         <10:0>
;  RW-4R 
;   500 : #define NV_PRAMDAC_HBLANK_END                            0x00680730 /* RW-4R */
NV_PRAMDAC_HBLANK_END           EQU             000680730h
;  RWIVF 
;   501 : #define NV_PRAMDAC_HBLANK_END_VAL                              10:0 /* RWIVF */
NV_PRAMDAC_HBLANK_END_VAL               TEXTEQU         <10:0>
;  RW-4R 
;   502 : #define NV_PRAMDAC_HTOTAL                                0x00680734 /* RW-4R */
NV_PRAMDAC_HTOTAL               EQU             000680734h
;  RWIVF 
;   503 : #define NV_PRAMDAC_HTOTAL_VAL                                  10:0 /* RWIVF */
NV_PRAMDAC_HTOTAL_VAL           TEXTEQU         <10:0>
;  RW-4R 
;   504 : #define NV_PRAMDAC_HEQU_WIDTH                            0x00680738 /* RW-4R */
NV_PRAMDAC_HEQU_WIDTH           EQU             000680738h
;  RWIVF 
;   505 : #define NV_PRAMDAC_HEQU_WIDTH_VAL                              10:0 /* RWIVF */
NV_PRAMDAC_HEQU_WIDTH_VAL               TEXTEQU         <10:0>
;  RW-4R 
;   506 : #define NV_PRAMDAC_HSERR_WIDTH                           0x0068073C /* RW-4R */
NV_PRAMDAC_HSERR_WIDTH          EQU             00068073ch
;  RWIVF 
;   507 : #define NV_PRAMDAC_HSERR_WIDTH_VAL                             10:0 /* RWIVF */
NV_PRAMDAC_HSERR_WIDTH_VAL              TEXTEQU         <10:0>

NV_PRAMDAC_FP_DEBUG_0           EQU             000680880H

;  dev_dac.ref 
;  RW--D 
;   508 : /* dev_dac.ref */
;   509 : #define NV_USER_DAC                           0x00681FFF:0x00681200 /* RW--D */
NV_USER_DAC             TEXTEQU         <0x00681FFF:0x00681200>
;  RWI1R 
;   510 : #define NV_USER_DAC_PIXEL_MASK                           0x006813C6 /* RWI1R */
NV_USER_DAC_PIXEL_MASK          EQU             0006813c6h
;  RWIVF 
;   511 : #define NV_USER_DAC_PIXEL_MASK_VALUE                            7:0 /* RWIVF */
NV_USER_DAC_PIXEL_MASK_VALUE            TEXTEQU         <7:0>
;  RWI-V 
;   512 : #define NV_USER_DAC_PIXEL_MASK_MASK                      0x000000FF /* RWI-V */
NV_USER_DAC_PIXEL_MASK_MASK             EQU             0000000ffh
;  RW-1R 
;   513 : #define NV_USER_DAC_READ_MODE_ADDRESS                    0x006813C7 /* RW-1R */
NV_USER_DAC_READ_MODE_ADDRESS           EQU             0006813c7h
;  RW-VF 
;   514 : #define NV_USER_DAC_READ_MODE_ADDRESS_VALUE                     7:0 /* RW-VF */
NV_USER_DAC_READ_MODE_ADDRESS_VALUE             TEXTEQU         <7:0>
;  -W-VF 
;   515 : #define NV_USER_DAC_READ_MODE_ADDRESS_WO_VALUE                  7:0 /* -W-VF */
NV_USER_DAC_READ_MODE_ADDRESS_WO_VALUE          TEXTEQU         <7:0>
;  R--VF 
;   516 : #define NV_USER_DAC_READ_MODE_ADDRESS_RW_STATE                  1:0 /* R--VF */
NV_USER_DAC_READ_MODE_ADDRESS_RW_STATE          TEXTEQU         <1:0>
;  R---V 
;   517 : #define NV_USER_DAC_READ_MODE_ADDRESS_RW_STATE_WRITE     0x00000000 /* R---V */
NV_USER_DAC_READ_MODE_ADDRESS_RW_STATE_WRITE            EQU             000000000h
;  R---V 
;   518 : #define NV_USER_DAC_READ_MODE_ADDRESS_RW_STATE_READ      0x00000011 /* R---V */
NV_USER_DAC_READ_MODE_ADDRESS_RW_STATE_READ             EQU             000000011h
;  RW-1R 
;   519 : #define NV_USER_DAC_WRITE_MODE_ADDRESS                   0x006813C8 /* RW-1R */
NV_USER_DAC_WRITE_MODE_ADDRESS          EQU             0006813c8h
;  RW-VF 
;   520 : #define NV_USER_DAC_WRITE_MODE_ADDRESS_VALUE                    7:0 /* RW-VF */
NV_USER_DAC_WRITE_MODE_ADDRESS_VALUE            TEXTEQU         <7:0>
;  RW-1R 
;   521 : #define NV_USER_DAC_PALETTE_DATA                         0x006813C9 /* RW-1R */
NV_USER_DAC_PALETTE_DATA                EQU             0006813c9h
;  RW-VF 
;   522 : #define NV_USER_DAC_PALETTE_DATA_VALUE                          7:0 /* RW-VF */
NV_USER_DAC_PALETTE_DATA_VALUE          TEXTEQU         <7:0>
;  dev_realmode.ref 
;  R--1R 
;   523 : /* dev_realmode.ref */
;   524 : #define NV_IO_MPU_401_DATA                               0x00000330 /* R--1R */
NV_IO_MPU_401_DATA              EQU             000000330h
;  R--1R 
;   525 : #define NV_IO_MPU_401_DATA__ALIAS_1                      0x00000300 /* R--1R */
NV_IO_MPU_401_DATA__ALIAS_1             EQU             000000300h
;  R--1R 
;   526 : #define NV_IO_MPU_401_DATA__ALIAS_2                      0x00000230 /* R--1R */
NV_IO_MPU_401_DATA__ALIAS_2             EQU             000000230h
;  R--VF 
;   527 : #define NV_IO_MPU_401_DATA_VALUE                                7:0 /* R--VF */
NV_IO_MPU_401_DATA_VALUE                TEXTEQU         <7:0>
;  R---V 
;   528 : #define NV_IO_MPU_401_DATA_ACK                           0x000000FE /* R---V */
NV_IO_MPU_401_DATA_ACK          EQU             0000000feh
;  R--1R 
;   529 : #define NV_IO_MPU_401_STATUS                             0x00000331 /* R--1R */
NV_IO_MPU_401_STATUS            EQU             000000331h
;  R--1R 
;   530 : #define NV_IO_MPU_401_STATUS__ALIAS_1                    0x00000301 /* R--1R */
NV_IO_MPU_401_STATUS__ALIAS_1           EQU             000000301h
;  R--1R 
;   531 : #define NV_IO_MPU_401_STATUS__ALIAS_2                    0x00000231 /* R--1R */
NV_IO_MPU_401_STATUS__ALIAS_2           EQU             000000231h
;  R--VF 
;   532 : #define NV_IO_MPU_401_STATUS_DATA                               5:0 /* R--VF */
NV_IO_MPU_401_STATUS_DATA               TEXTEQU         <5:0>
;  R--VF 
;   533 : #define NV_IO_MPU_401_STATUS_WRITE                              6:6 /* R--VF */
NV_IO_MPU_401_STATUS_WRITE              TEXTEQU         <6:6>
;  R---V 
;   534 : #define NV_IO_MPU_401_STATUS_WRITE_EMPTY                 0x00000000 /* R---V */
NV_IO_MPU_401_STATUS_WRITE_EMPTY                EQU             000000000h
;  R---V 
;   535 : #define NV_IO_MPU_401_STATUS_WRITE_FULL                  0x00000001 /* R---V */
NV_IO_MPU_401_STATUS_WRITE_FULL         EQU             000000001h
;  R--VF 
;   536 : #define NV_IO_MPU_401_STATUS_READ                               7:7 /* R--VF */
NV_IO_MPU_401_STATUS_READ               TEXTEQU         <7:7>
;  R---V 
;   537 : #define NV_IO_MPU_401_STATUS_READ_FULL                   0x00000000 /* R---V */
NV_IO_MPU_401_STATUS_READ_FULL          EQU             000000000h
;  R---V 
;   538 : #define NV_IO_MPU_401_STATUS_READ_EMPTY                  0x00000001 /* R---V */
NV_IO_MPU_401_STATUS_READ_EMPTY         EQU             000000001h
;  -W-1R 
;   539 : #define NV_IO_MPU_401_COM                                0x00000331 /* -W-1R */
NV_IO_MPU_401_COM               EQU             000000331h
;  -W-1R 
;   540 : #define NV_IO_MPU_401_COM__ALIAS_1                       0x00000301 /* -W-1R */
NV_IO_MPU_401_COM__ALIAS_1              EQU             000000301h
;  -W-1R 
;   541 : #define NV_IO_MPU_401_COM__ALIAS_2                       0x00000231 /* -W-1R */
NV_IO_MPU_401_COM__ALIAS_2              EQU             000000231h
;  -WIVF 
;   542 : #define NV_IO_MPU_401_COM_UART_MODE                             7:0 /* -WIVF */
NV_IO_MPU_401_COM_UART_MODE             TEXTEQU         <7:0>
;  -WI-V 
;   543 : #define NV_IO_MPU_401_COM_UART_MODE_COMPLEX              0x000000ff /* -WI-V */
NV_IO_MPU_401_COM_UART_MODE_COMPLEX             EQU             0000000ffh
;  -W--V 
;   544 : #define NV_IO_MPU_401_COM_UART_MODE_SIMPLE               0x0000003f /* -W--V */
NV_IO_MPU_401_COM_UART_MODE_SIMPLE              EQU             00000003fh
;  dev_master.ref 
;  RW--D 
;   545 : /* dev_master.ref */
;   546 : #define NV_PMC                                0x00000FFF:0x00000000 /* RW--D */
NV_PMC          TEXTEQU         <0x00000FFF:0x00000000>
;  R--4R 
;   547 : #define NV_PMC_BOOT_0                                    0x00000000 /* R--4R */
NV_PMC_BOOT_0           EQU             000000000h
;  C--VF 
;   548 : #define NV_PMC_BOOT_0_FIB_REVISION                              3:0 /* C--VF */
NV_PMC_BOOT_0_FIB_REVISION              TEXTEQU         <3:0>
;  C---V 
;   549 : #define NV_PMC_BOOT_0_FIB_REVISION_0                     0x00000000 /* C---V */
NV_PMC_BOOT_0_FIB_REVISION_0            EQU             000000000h
;  C--VF 
;   550 : #define NV_PMC_BOOT_0_MASK_REVISION                             7:4 /* C--VF */
NV_PMC_BOOT_0_MASK_REVISION             TEXTEQU         <7:4>
;  C---V 
;   551 : #define NV_PMC_BOOT_0_MASK_REVISION_A                    0x00000000 /* C---V */
NV_PMC_BOOT_0_MASK_REVISION_A           EQU             000000000h
;  R--VF 
;   552 : #define NV_PMC_BOOT_0_IMPLEMENTATION                           15:8 /* R--VF */
NV_PMC_BOOT_0_IMPLEMENTATION            TEXTEQU         <15:8>
;  ----V 
;   553 : #define NV_PMC_BOOT_0_IMPLEMENTATION_NV0                 0x00000000 /* ----V */
NV_PMC_BOOT_0_IMPLEMENTATION_NV0                EQU             000000000h
;  ----V 
;   554 : #define NV_PMC_BOOT_0_IMPLEMENTATION_NV1V32              0x00000001 /* ----V */
NV_PMC_BOOT_0_IMPLEMENTATION_NV1V32             EQU             000000001h
;  ----V 
;   555 : #define NV_PMC_BOOT_0_IMPLEMENTATION_NV1D32              0x00000002 /* ----V */
NV_PMC_BOOT_0_IMPLEMENTATION_NV1D32             EQU             000000002h
;  ----V 
;   556 : #define NV_PMC_BOOT_0_IMPLEMENTATION_PICASSO             0x00000003 /* ----V */
NV_PMC_BOOT_0_IMPLEMENTATION_PICASSO            EQU             000000003h
;  ----V 
;   557 : #define NV_PMC_BOOT_0_IMPLEMENTATION_NV2MUTARA           0x00000004 /* ----V */
NV_PMC_BOOT_0_IMPLEMENTATION_NV2MUTARA          EQU             000000004h
;  R---V 
;   558 : #define NV_PMC_BOOT_0_IMPLEMENTATION_NV3                 0x00000005 /* R---V */
NV_PMC_BOOT_0_IMPLEMENTATION_NV3                EQU             000000005h
;  C--VF 
;   559 : #define NV_PMC_BOOT_0_ARCHITECTURE                            23:16 /* C--VF */
NV_PMC_BOOT_0_ARCHITECTURE              TEXTEQU         <23:16>
;  ----V 
;   560 : #define NV_PMC_BOOT_0_ARCHITECTURE_NV0                   0x00000000 /* ----V */
NV_PMC_BOOT_0_ARCHITECTURE_NV0          EQU             000000000h
;  ----V 
;   561 : #define NV_PMC_BOOT_0_ARCHITECTURE_NV1                   0x00000001 /* ----V */
NV_PMC_BOOT_0_ARCHITECTURE_NV1          EQU             000000001h
;  ----V 
;   562 : #define NV_PMC_BOOT_0_ARCHITECTURE_NV2                   0x00000002 /* ----V */
NV_PMC_BOOT_0_ARCHITECTURE_NV2          EQU             000000002h
;  C---V 
;   563 : #define NV_PMC_BOOT_0_ARCHITECTURE_NV3                   0x00000003 /* C---V */
NV_PMC_BOOT_0_ARCHITECTURE_NV3          EQU             000000003h
;  C--UF 
;   564 : #define NV_PMC_BOOT_0_MANUFACTURER                            27:24 /* C--UF */
NV_PMC_BOOT_0_MANUFACTURER              TEXTEQU         <27:24>
;  C---V 
;   565 : #define NV_PMC_BOOT_0_MANUFACTURER_NVIDIA                0x00000000 /* C---V */
NV_PMC_BOOT_0_MANUFACTURER_NVIDIA               EQU             000000000h
;  C--VF 
;   566 : #define NV_PMC_BOOT_0_FOUNDRY                                 31:28 /* C--VF */
NV_PMC_BOOT_0_FOUNDRY           TEXTEQU         <31:28>
;  C---V 
;   567 : #define NV_PMC_BOOT_0_FOUNDRY_SGS                        0x00000000 /* C---V */
NV_PMC_BOOT_0_FOUNDRY_SGS               EQU             000000000h
;  ----V 
;   568 : #define NV_PMC_BOOT_0_FOUNDRY_HELIOS                     0x00000001 /* ----V */
NV_PMC_BOOT_0_FOUNDRY_HELIOS            EQU             000000001h
;  RW-4R 
;   569 : #define NV_PMC_INTR_0                                    0x00000100 /* RW-4R */
NV_PMC_INTR_0           EQU             000000100h
;  R--VF 
;   570 : #define NV_PMC_INTR_0_PAUDIO                                    0:0 /* R--VF */
NV_PMC_INTR_0_PAUDIO            TEXTEQU         <0:0>
;  R---V 
;   571 : #define NV_PMC_INTR_0_PAUDIO_NOT_PENDING                 0x00000000 /* R---V */
NV_PMC_INTR_0_PAUDIO_NOT_PENDING                EQU             000000000h
;  R---V 
;   572 : #define NV_PMC_INTR_0_PAUDIO_PENDING                     0x00000001 /* R---V */
NV_PMC_INTR_0_PAUDIO_PENDING            EQU             000000001h
;  R--VF 
;   573 : #define NV_PMC_INTR_0_PMEDIA                                    4:4 /* R--VF */
NV_PMC_INTR_0_PMEDIA            TEXTEQU         <4:4>
;  R---V 
;   574 : #define NV_PMC_INTR_0_PMEDIA_NOT_PENDING                 0x00000000 /* R---V */
NV_PMC_INTR_0_PMEDIA_NOT_PENDING                EQU             000000000h
;  R---V 
;   575 : #define NV_PMC_INTR_0_PMEDIA_PENDING                     0x00000001 /* R---V */
NV_PMC_INTR_0_PMEDIA_PENDING            EQU             000000001h
;  R--VF 
;   576 : #define NV_PMC_INTR_0_PFIFO                                     8:8 /* R--VF */
NV_PMC_INTR_0_PFIFO             TEXTEQU         <8:8>
;  R---V 
;   577 : #define NV_PMC_INTR_0_PFIFO_NOT_PENDING                  0x00000000 /* R---V */
NV_PMC_INTR_0_PFIFO_NOT_PENDING         EQU             000000000h
;  R---V 
;   578 : #define NV_PMC_INTR_0_PFIFO_PENDING                      0x00000001 /* R---V */
NV_PMC_INTR_0_PFIFO_PENDING             EQU             000000001h
;  R--VF 
;   579 : #define NV_PMC_INTR_0_PGRAPH0                                 12:12 /* R--VF */
NV_PMC_INTR_0_PGRAPH0           TEXTEQU         <12:12>
;  R---V 
;   580 : #define NV_PMC_INTR_0_PGRAPH0_NOT_PENDING                0x00000000 /* R---V */
NV_PMC_INTR_0_PGRAPH0_NOT_PENDING               EQU             000000000h
;  R---V 
;   581 : #define NV_PMC_INTR_0_PGRAPH0_PENDING                    0x00000001 /* R---V */
NV_PMC_INTR_0_PGRAPH0_PENDING           EQU             000000001h
;  R--VF 
;   582 : #define NV_PMC_INTR_0_PGRAPH1                                 13:13 /* R--VF */
NV_PMC_INTR_0_PGRAPH1           TEXTEQU         <13:13>
;  R---V 
;   583 : #define NV_PMC_INTR_0_PGRAPH1_NOT_PENDING                0x00000000 /* R---V */
NV_PMC_INTR_0_PGRAPH1_NOT_PENDING               EQU             000000000h
;  R---V 
;   584 : #define NV_PMC_INTR_0_PGRAPH1_PENDING                    0x00000001 /* R---V */
NV_PMC_INTR_0_PGRAPH1_PENDING           EQU             000000001h
;  R--VF 
;   585 : #define NV_PMC_INTR_0_PVIDEO                                  16:16 /* R--VF */
NV_PMC_INTR_0_PVIDEO            TEXTEQU         <16:16>
;  R---V 
;   586 : #define NV_PMC_INTR_0_PVIDEO_NOT_PENDING                 0x00000000 /* R---V */
NV_PMC_INTR_0_PVIDEO_NOT_PENDING                EQU             000000000h
;  R---V 
;   587 : #define NV_PMC_INTR_0_PVIDEO_PENDING                     0x00000001 /* R---V */
NV_PMC_INTR_0_PVIDEO_PENDING            EQU             000000001h
;  R--VF 
;   588 : #define NV_PMC_INTR_0_PTIMER                                  20:20 /* R--VF */
NV_PMC_INTR_0_PTIMER            TEXTEQU         <20:20>
;  R---V 
;   589 : #define NV_PMC_INTR_0_PTIMER_NOT_PENDING                 0x00000000 /* R---V */
NV_PMC_INTR_0_PTIMER_NOT_PENDING                EQU             000000000h
;  R---V 
;   590 : #define NV_PMC_INTR_0_PTIMER_PENDING                     0x00000001 /* R---V */
NV_PMC_INTR_0_PTIMER_PENDING            EQU             000000001h
;  R--VF 
;   591 : #define NV_PMC_INTR_0_PFB                                     24:24 /* R--VF */
NV_PMC_INTR_0_PFB               TEXTEQU         <24:24>
;  R---V 
;   592 : #define NV_PMC_INTR_0_PFB_NOT_PENDING                    0x00000000 /* R---V */
NV_PMC_INTR_0_PFB_NOT_PENDING           EQU             000000000h
;  R---V 
;   593 : #define NV_PMC_INTR_0_PFB_PENDING                        0x00000001 /* R---V */
NV_PMC_INTR_0_PFB_PENDING               EQU             000000001h
;  R--VF 
;   594 : #define NV_PMC_INTR_0_PBUS                                    28:28 /* R--VF */
NV_PMC_INTR_0_PBUS              TEXTEQU         <28:28>
;  R---V 
;   595 : #define NV_PMC_INTR_0_PBUS_NOT_PENDING                   0x00000000 /* R---V */
NV_PMC_INTR_0_PBUS_NOT_PENDING          EQU             000000000h
;  R---V 
;   596 : #define NV_PMC_INTR_0_PBUS_PENDING                       0x00000001 /* R---V */
NV_PMC_INTR_0_PBUS_PENDING              EQU             000000001h
;  RWIVF 
;   597 : #define NV_PMC_INTR_0_SOFTWARE                                31:31 /* RWIVF */
NV_PMC_INTR_0_SOFTWARE          TEXTEQU         <31:31>
;  RWI-V 
;   598 : #define NV_PMC_INTR_0_SOFTWARE_NOT_PENDING               0x00000000 /* RWI-V */
NV_PMC_INTR_0_SOFTWARE_NOT_PENDING              EQU             000000000h
;  RW--V 
;   599 : #define NV_PMC_INTR_0_SOFTWARE_PENDING                   0x00000001 /* RW--V */
NV_PMC_INTR_0_SOFTWARE_PENDING          EQU             000000001h
;  RW-4R 
;   600 : #define NV_PMC_INTR_EN_0                                 0x00000140 /* RW-4R */
NV_PMC_INTR_EN_0                EQU             000000140h
;  RWIVF 
;   601 : #define NV_PMC_INTR_EN_0_INTA                                   1:0 /* RWIVF */
NV_PMC_INTR_EN_0_INTA           TEXTEQU         <1:0>
;  RWI-V 
;   602 : #define NV_PMC_INTR_EN_0_INTA_DISABLED                   0x00000000 /* RWI-V */
NV_PMC_INTR_EN_0_INTA_DISABLED          EQU             000000000h
;  RW--V 
;   603 : #define NV_PMC_INTR_EN_0_INTA_HARDWARE                   0x00000001 /* RW--V */
NV_PMC_INTR_EN_0_INTA_HARDWARE          EQU             000000001h
;  RW--V 
;   604 : #define NV_PMC_INTR_EN_0_INTA_SOFTWARE                   0x00000002 /* RW--V */
NV_PMC_INTR_EN_0_INTA_SOFTWARE          EQU             000000002h
;  R--4R 
;   605 : #define NV_PMC_INTR_READ_0                               0x00000160 /* R--4R */
NV_PMC_INTR_READ_0              EQU             000000160h
;  R--VF 
;   606 : #define NV_PMC_INTR_READ_0_INTA                                 0:0 /* R--VF */
NV_PMC_INTR_READ_0_INTA         TEXTEQU         <0:0>
;  R---V 
;   607 : #define NV_PMC_INTR_READ_0_INTA_LOW                      0x00000000 /* R---V */
NV_PMC_INTR_READ_0_INTA_LOW             EQU             000000000h
;  R---V 
;   608 : #define NV_PMC_INTR_READ_0_INTA_HIGH                     0x00000001 /* R---V */
NV_PMC_INTR_READ_0_INTA_HIGH            EQU             000000001h
;  RW-4R 
;   609 : #define NV_PMC_ENABLE                                    0x00000200 /* RW-4R */
NV_PMC_ENABLE           EQU             000000200h
;  RWIVF 
;   610 : #define NV_PMC_ENABLE_PAUDIO                                    0:0 /* RWIVF */
NV_PMC_ENABLE_PAUDIO            TEXTEQU         <0:0>
;  RWI-V 
;   611 : #define NV_PMC_ENABLE_PAUDIO_DISABLED                    0x00000000 /* RWI-V */
NV_PMC_ENABLE_PAUDIO_DISABLED           EQU             000000000h
;  RW--V 
;   612 : #define NV_PMC_ENABLE_PAUDIO_ENABLED                     0x00000001 /* RW--V */
NV_PMC_ENABLE_PAUDIO_ENABLED            EQU             000000001h
;  RWIVF 
;   613 : #define NV_PMC_ENABLE_PMEDIA                                    4:4 /* RWIVF */
NV_PMC_ENABLE_PMEDIA            TEXTEQU         <4:4>
;  RWI-V 
;   614 : #define NV_PMC_ENABLE_PMEDIA_DISABLED                    0x00000000 /* RWI-V */
NV_PMC_ENABLE_PMEDIA_DISABLED           EQU             000000000h
;  RW--V 
;   615 : #define NV_PMC_ENABLE_PMEDIA_ENABLED                     0x00000001 /* RW--V */
NV_PMC_ENABLE_PMEDIA_ENABLED            EQU             000000001h
;  RWIVF 
;   616 : #define NV_PMC_ENABLE_PFIFO                                     8:8 /* RWIVF */
NV_PMC_ENABLE_PFIFO             TEXTEQU         <8:8>
;  RWI-V 
;   617 : #define NV_PMC_ENABLE_PFIFO_DISABLED                     0x00000000 /* RWI-V */
NV_PMC_ENABLE_PFIFO_DISABLED            EQU             000000000h
;  RW--V 
;   618 : #define NV_PMC_ENABLE_PFIFO_ENABLED                      0x00000001 /* RW--V */
NV_PMC_ENABLE_PFIFO_ENABLED             EQU             000000001h
;  RWIVF 
;   619 : #define NV_PMC_ENABLE_PGRAPH                                  12:12 /* RWIVF */
NV_PMC_ENABLE_PGRAPH            TEXTEQU         <12:12>
;  RWI-V 
;   620 : #define NV_PMC_ENABLE_PGRAPH_DISABLED                    0x00000000 /* RWI-V */
NV_PMC_ENABLE_PGRAPH_DISABLED           EQU             000000000h
;  RW--V 
;   621 : #define NV_PMC_ENABLE_PGRAPH_ENABLED                     0x00000001 /* RW--V */
NV_PMC_ENABLE_PGRAPH_ENABLED            EQU             000000001h
;  RWIVF 
;   622 : #define NV_PMC_ENABLE_PPMI                                    16:16 /* RWIVF */
NV_PMC_ENABLE_PPMI              TEXTEQU         <16:16>
;  RWI-V 
;   623 : #define NV_PMC_ENABLE_PPMI_DISABLED                      0x00000000 /* RWI-V */
NV_PMC_ENABLE_PPMI_DISABLED             EQU             000000000h
;  RW--V 
;   624 : #define NV_PMC_ENABLE_PPMI_ENABLED                       0x00000001 /* RW--V */
NV_PMC_ENABLE_PPMI_ENABLED              EQU             000000001h
;  RWIVF 
;   625 : #define NV_PMC_ENABLE_PFB                                     20:20 /* RWIVF */
NV_PMC_ENABLE_PFB               TEXTEQU         <20:20>
;  RW--V 
;   626 : #define NV_PMC_ENABLE_PFB_DISABLED                       0x00000000 /* RW--V */
NV_PMC_ENABLE_PFB_DISABLED              EQU             000000000h
;  RWI-V 
;   627 : #define NV_PMC_ENABLE_PFB_ENABLED                        0x00000001 /* RWI-V */
NV_PMC_ENABLE_PFB_ENABLED               EQU             000000001h
;  RWIVF 
;   628 : #define NV_PMC_ENABLE_PCRTC                                   24:24 /* RWIVF */
NV_PMC_ENABLE_PCRTC             TEXTEQU         <24:24>
;  RWI-V 
;   629 : #define NV_PMC_ENABLE_PCRTC_DISABLED                     0x00000000 /* RWI-V */
NV_PMC_ENABLE_PCRTC_DISABLED            EQU             000000000h
;  RW--V 
;   630 : #define NV_PMC_ENABLE_PCRTC_ENABLED                      0x00000001 /* RW--V */
NV_PMC_ENABLE_PCRTC_ENABLED             EQU             000000001h
;  RWIVF 
;   631 : #define NV_PMC_ENABLE_PVIDEO                                  28:28 /* RWIVF */
NV_PMC_ENABLE_PVIDEO            TEXTEQU         <28:28>
;  RWI-V 
;   632 : #define NV_PMC_ENABLE_PVIDEO_DISABLED                    0x00000000 /* RWI-V */
NV_PMC_ENABLE_PVIDEO_DISABLED           EQU             000000000h
;  RW--V 
;   633 : #define NV_PMC_ENABLE_PVIDEO_ENABLED                     0x00000001 /* RW--V */
NV_PMC_ENABLE_PVIDEO_ENABLED            EQU             000000001h
;  dev_bus.ref 
;  RW--D 
;   634 : /* dev_bus.ref */
;   635 : #define NV_PBUS                               0x00001FFF:0x00001000 /* RW--D */
NV_PBUS         TEXTEQU         <0x00001FFF:0x00001000>
;  RW-4R 
;   636 : #define NV_PBUS_DEBUG_0                                  0x00001080 /* RW-4R */
NV_PBUS_DEBUG_0         EQU             000001080h
;  RWIVF 
;   637 : #define NV_PBUS_DEBUG_0_MODE                                    0:0 /* RWIVF */
NV_PBUS_DEBUG_0_MODE            TEXTEQU         <0:0>
;  RWI-V 
;   638 : #define NV_PBUS_DEBUG_0_MODE_DISABLED                    0x00000000 /* RWI-V */
NV_PBUS_DEBUG_0_MODE_DISABLED           EQU             000000000h
;  RW--V 
;   639 : #define NV_PBUS_DEBUG_0_MODE_ENABLED                     0x00000001 /* RW--V */
NV_PBUS_DEBUG_0_MODE_ENABLED            EQU             000000001h
;  RWIVF 
;   640 : #define NV_PBUS_DEBUG_0_DESKEWER                                4:4 /* RWIVF */
NV_PBUS_DEBUG_0_DESKEWER                TEXTEQU         <4:4>
;  RWI-V 
;   641 : #define NV_PBUS_DEBUG_0_DESKEWER_ENABLED                 0x00000000 /* RWI-V */
NV_PBUS_DEBUG_0_DESKEWER_ENABLED                EQU             000000000h
;  RW--V 
;   642 : #define NV_PBUS_DEBUG_0_DESKEWER_BYPASS                  0x00000001 /* RW--V */
NV_PBUS_DEBUG_0_DESKEWER_BYPASS         EQU             000000001h
;  RWIVF 
;   643 : #define NV_PBUS_DEBUG_0_FBIO_SCLK_DELAY                        11:8 /* RWIVF */
NV_PBUS_DEBUG_0_FBIO_SCLK_DELAY         TEXTEQU         <11:8>
;  RWI-V 
;   644 : #define NV_PBUS_DEBUG_0_FBIO_SCLK_DELAY_8                0x00000008 /* RWI-V */
NV_PBUS_DEBUG_0_FBIO_SCLK_DELAY_8               EQU             000000008h
;  RW-4R 
;   645 : #define NV_PBUS_DEBUG_SEL_0                              0x00001090 /* RW-4R */
NV_PBUS_DEBUG_SEL_0             EQU             000001090h
;  RWXUF 
;   646 : #define NV_PBUS_DEBUG_SEL_0_X                                   2:0 /* RWXUF */
NV_PBUS_DEBUG_SEL_0_X           TEXTEQU         <2:0>
;  RW-4R 
;   647 : #define NV_PBUS_DEBUG_SEL_1                              0x00001094 /* RW-4R */
NV_PBUS_DEBUG_SEL_1             EQU             000001094h
;  RWXUF 
;   648 : #define NV_PBUS_DEBUG_SEL_1_X                                   2:0 /* RWXUF */
NV_PBUS_DEBUG_SEL_1_X           TEXTEQU         <2:0>
;  RW-4R 
;   649 : #define NV_PBUS_DEBUG_SEL_2                              0x00001098 /* RW-4R */
NV_PBUS_DEBUG_SEL_2             EQU             000001098h
;  RWXUF 
;   650 : #define NV_PBUS_DEBUG_SEL_2_X                                   2:0 /* RWXUF */
NV_PBUS_DEBUG_SEL_2_X           TEXTEQU         <2:0>
;  RW-4R 
;   651 : #define NV_PBUS_DEBUG_SEL_3                              0x0000109C /* RW-4R */
NV_PBUS_DEBUG_SEL_3             EQU             00000109ch
;  RWXUF 
;   652 : #define NV_PBUS_DEBUG_SEL_3_X                                   2:0 /* RWXUF */
NV_PBUS_DEBUG_SEL_3_X           TEXTEQU         <2:0>
;  RW-4R 
;   653 : #define NV_PBUS_DEBUG_HOST                               0x000010A0 /* RW-4R */
NV_PBUS_DEBUG_HOST              EQU             0000010a0h
;  RWXUF 
;   654 : #define NV_PBUS_DEBUG_HOST_SEL                                  2:0 /* RWXUF */
NV_PBUS_DEBUG_HOST_SEL          TEXTEQU         <2:0>
;  RW-4R 
;   655 : #define NV_PBUS_DEBUG_1                                  0x00001084 /* RW-4R */
NV_PBUS_DEBUG_1         EQU             000001084h
;  RWIVF 
;   656 : #define NV_PBUS_DEBUG_1_PCIM_THROTTLE                           0:0 /* RWIVF */
NV_PBUS_DEBUG_1_PCIM_THROTTLE           TEXTEQU         <0:0>
;  RW--V 
;   657 : #define NV_PBUS_DEBUG_1_PCIM_THROTTLE_DISABLED           0x00000000 /* RW--V */
NV_PBUS_DEBUG_1_PCIM_THROTTLE_DISABLED          EQU             000000000h
;  RWI-V 
;   658 : #define NV_PBUS_DEBUG_1_PCIM_THROTTLE_ENABLED            0x00000001 /* RWI-V */
NV_PBUS_DEBUG_1_PCIM_THROTTLE_ENABLED           EQU             000000001h
;  RWIVF 
;   659 : #define NV_PBUS_DEBUG_1_PCIM_CMD                                1:1 /* RWIVF */
NV_PBUS_DEBUG_1_PCIM_CMD                TEXTEQU         <1:1>
;  RWI-V 
;   660 : #define NV_PBUS_DEBUG_1_PCIM_CMD_SIZE_BASED              0x00000000 /* RWI-V */
NV_PBUS_DEBUG_1_PCIM_CMD_SIZE_BASED             EQU             000000000h
;  RW--V 
;   661 : #define NV_PBUS_DEBUG_1_PCIM_CMD_MRL_ONLY                0x00000001 /* RW--V */
NV_PBUS_DEBUG_1_PCIM_CMD_MRL_ONLY               EQU             000000001h
;  RWIVF 
;   662 : #define NV_PBUS_DEBUG_1_PCIM_AGP                                2:2 /* RWIVF */
NV_PBUS_DEBUG_1_PCIM_AGP                TEXTEQU         <2:2>
;  RWI-V 
;   663 : #define NV_PBUS_DEBUG_1_PCIM_AGP_IS_AGP                  0x00000000 /* RWI-V */
NV_PBUS_DEBUG_1_PCIM_AGP_IS_AGP         EQU             000000000h
;  RW--V 
;   664 : #define NV_PBUS_DEBUG_1_PCIM_AGP_IS_PCI                  0x00000001 /* RW--V */
NV_PBUS_DEBUG_1_PCIM_AGP_IS_PCI         EQU             000000001h
;  RWIVF 
;   665 : #define NV_PBUS_DEBUG_1_AGPM_CMD                                4:3 /* RWIVF */
NV_PBUS_DEBUG_1_AGPM_CMD                TEXTEQU         <4:3>
;  RW--V 
;   666 : #define NV_PBUS_DEBUG_1_AGPM_CMD_HP_ON_1ST               0x00000000 /* RW--V */
NV_PBUS_DEBUG_1_AGPM_CMD_HP_ON_1ST              EQU             000000000h
;  RWI-V 
;   667 : #define NV_PBUS_DEBUG_1_AGPM_CMD_LP_ONLY                 0x00000001 /* RWI-V */
NV_PBUS_DEBUG_1_AGPM_CMD_LP_ONLY                EQU             000000001h
;  RW--V 
;   668 : #define NV_PBUS_DEBUG_1_AGPM_CMD_HP_ONLY                 0x00000002 /* RW--V */
NV_PBUS_DEBUG_1_AGPM_CMD_HP_ONLY                EQU             000000002h
;  RWIVF 
;   669 : #define NV_PBUS_DEBUG_1_PCIS_WRITE                              5:5 /* RWIVF */
NV_PBUS_DEBUG_1_PCIS_WRITE              TEXTEQU         <5:5>
;  RW--V 
;   670 : #define NV_PBUS_DEBUG_1_PCIS_WRITE_0_CYCLE               0x00000000 /* RW--V */
NV_PBUS_DEBUG_1_PCIS_WRITE_0_CYCLE              EQU             000000000h
;  RWI-V 
;   671 : #define NV_PBUS_DEBUG_1_PCIS_WRITE_1_CYCLE               0x00000001 /* RWI-V */
NV_PBUS_DEBUG_1_PCIS_WRITE_1_CYCLE              EQU             000000001h
;  RWIVF 
;   672 : #define NV_PBUS_DEBUG_1_PCIS_2_1                                6:6 /* RWIVF */
NV_PBUS_DEBUG_1_PCIS_2_1                TEXTEQU         <6:6>
;  RW--V 
;   673 : #define NV_PBUS_DEBUG_1_PCIS_2_1_DISABLED                0x00000000 /* RW--V */
NV_PBUS_DEBUG_1_PCIS_2_1_DISABLED               EQU             000000000h
;  RWI-V 
;   674 : #define NV_PBUS_DEBUG_1_PCIS_2_1_ENABLED                 0x00000001 /* RWI-V */
NV_PBUS_DEBUG_1_PCIS_2_1_ENABLED                EQU             000000001h
;  RWIVF 
;   675 : #define NV_PBUS_DEBUG_1_PCIS_RETRY                              7:7 /* RWIVF */
NV_PBUS_DEBUG_1_PCIS_RETRY              TEXTEQU         <7:7>
;  RW--V 
;   676 : #define NV_PBUS_DEBUG_1_PCIS_RETRY_DISABLED              0x00000000 /* RW--V */
NV_PBUS_DEBUG_1_PCIS_RETRY_DISABLED             EQU             000000000h
;  RWI-V 
;   677 : #define NV_PBUS_DEBUG_1_PCIS_RETRY_ENABLED               0x00000001 /* RWI-V */
NV_PBUS_DEBUG_1_PCIS_RETRY_ENABLED              EQU             000000001h
;  RWIVF 
;   678 : #define NV_PBUS_DEBUG_1_PCIS_RD_BURST                           8:8 /* RWIVF */
NV_PBUS_DEBUG_1_PCIS_RD_BURST           TEXTEQU         <8:8>
;  RWI-V 
;   679 : #define NV_PBUS_DEBUG_1_PCIS_RD_BURST_DISABLED           0x00000000 /* RWI-V */
NV_PBUS_DEBUG_1_PCIS_RD_BURST_DISABLED          EQU             000000000h
;  RW--V 
;   680 : #define NV_PBUS_DEBUG_1_PCIS_RD_BURST_ENABLED            0x00000001 /* RW--V */
NV_PBUS_DEBUG_1_PCIS_RD_BURST_ENABLED           EQU             000000001h
;  RWIVF 
;   681 : #define NV_PBUS_DEBUG_1_PCIS_WR_BURST                           9:9 /* RWIVF */
NV_PBUS_DEBUG_1_PCIS_WR_BURST           TEXTEQU         <9:9>
;  RWI-V 
;   682 : #define NV_PBUS_DEBUG_1_PCIS_WR_BURST_DISABLED           0x00000000 /* RWI-V */
NV_PBUS_DEBUG_1_PCIS_WR_BURST_DISABLED          EQU             000000000h
;  RW--V 
;   683 : #define NV_PBUS_DEBUG_1_PCIS_WR_BURST_ENABLED            0x00000001 /* RW--V */
NV_PBUS_DEBUG_1_PCIS_WR_BURST_ENABLED           EQU             000000001h
;  RWIVF 
;   684 : #define NV_PBUS_DEBUG_1_PCIS_EARLY_RTY                        10:10 /* RWIVF */
NV_PBUS_DEBUG_1_PCIS_EARLY_RTY          TEXTEQU         <10:10>
;  RW--V 
;   685 : #define NV_PBUS_DEBUG_1_PCIS_EARLY_RTY_DISABLED          0x00000000 /* RW--V */
NV_PBUS_DEBUG_1_PCIS_EARLY_RTY_DISABLED         EQU             000000000h
;  RWI-V 
;   686 : #define NV_PBUS_DEBUG_1_PCIS_EARLY_RTY_ENABLED           0x00000001 /* RWI-V */
NV_PBUS_DEBUG_1_PCIS_EARLY_RTY_ENABLED          EQU             000000001h
;  RWIVF 
;   687 : #define NV_PBUS_DEBUG_1_PCIS_RMAIO                            11:11 /* RWIVF */
NV_PBUS_DEBUG_1_PCIS_RMAIO              TEXTEQU         <11:11>
;  RW--V 
;   688 : #define NV_PBUS_DEBUG_1_PCIS_RMAIO_DISABLED              0x00000000 /* RW--V */
NV_PBUS_DEBUG_1_PCIS_RMAIO_DISABLED             EQU             000000000h
;  RWI-V 
;   689 : #define NV_PBUS_DEBUG_1_PCIS_RMAIO_ENABLED               0x00000001 /* RWI-V */
NV_PBUS_DEBUG_1_PCIS_RMAIO_ENABLED              EQU             000000001h
;  RWIVF 
;   690 : #define NV_PBUS_DEBUG_1_PCIS_CPUQ                             12:12 /* RWIVF */
NV_PBUS_DEBUG_1_PCIS_CPUQ               TEXTEQU         <12:12>
;  RW--V 
;   691 : #define NV_PBUS_DEBUG_1_PCIS_CPUQ_DISABLED               0x00000000 /* RW--V */
NV_PBUS_DEBUG_1_PCIS_CPUQ_DISABLED              EQU             000000000h
;  RWI-V 
;   692 : #define NV_PBUS_DEBUG_1_PCIS_CPUQ_ENABLED                0x00000001 /* RWI-V */
NV_PBUS_DEBUG_1_PCIS_CPUQ_ENABLED               EQU             000000001h
;  RWIVF 
;   693 : #define NV_PBUS_DEBUG_1_SPARE1                                13:13 /* RWIVF */
NV_PBUS_DEBUG_1_SPARE1          TEXTEQU         <13:13>
;  RWI-V 
;   694 : #define NV_PBUS_DEBUG_1_SPARE1_ZERO                      0x00000000 /* RWI-V */
NV_PBUS_DEBUG_1_SPARE1_ZERO             EQU             000000000h
;  RW--V 
;   695 : #define NV_PBUS_DEBUG_1_SPARE1_ONE                       0x00000001 /* RW--V */
NV_PBUS_DEBUG_1_SPARE1_ONE              EQU             000000001h
;  RWIVF 
;   696 : #define NV_PBUS_DEBUG_1_SPARE2                                14:14 /* RWIVF */
NV_PBUS_DEBUG_1_SPARE2          TEXTEQU         <14:14>
;  RWI-V 
;   697 : #define NV_PBUS_DEBUG_1_SPARE2_ZERO                      0x00000000 /* RWI-V */
NV_PBUS_DEBUG_1_SPARE2_ZERO             EQU             000000000h
;  RW--V 
;   698 : #define NV_PBUS_DEBUG_1_SPARE2_ONE                       0x00000001 /* RW--V */
NV_PBUS_DEBUG_1_SPARE2_ONE              EQU             000000001h
;  RW-4R 
;   699 : #define NV_PBUS_INTR_0                                   0x00001100 /* RW-4R */
NV_PBUS_INTR_0          EQU             000001100h
;  RWXVF 
;   700 : #define NV_PBUS_INTR_0_PCI_BUS_ERROR                            0:0 /* RWXVF */
NV_PBUS_INTR_0_PCI_BUS_ERROR            TEXTEQU         <0:0>
;  R---V 
;   701 : #define NV_PBUS_INTR_0_PCI_BUS_ERROR_NOT_PENDING         0x00000000 /* R---V */
NV_PBUS_INTR_0_PCI_BUS_ERROR_NOT_PENDING                EQU             000000000h
;  R---V 
;   702 : #define NV_PBUS_INTR_0_PCI_BUS_ERROR_PENDING             0x00000001 /* R---V */
NV_PBUS_INTR_0_PCI_BUS_ERROR_PENDING            EQU             000000001h
;  -W--V 
;   703 : #define NV_PBUS_INTR_0_PCI_BUS_ERROR_RESET               0x00000001 /* -W--V */
NV_PBUS_INTR_0_PCI_BUS_ERROR_RESET              EQU             000000001h
;  RWI4R 
;   704 : #define NV_PBUS_INTR_EN_0                                0x00001140 /* RWI4R */
NV_PBUS_INTR_EN_0               EQU             000001140h
;  RWIVF 
;   705 : #define NV_PBUS_INTR_EN_0_PCI_BUS_ERROR                         0:0 /* RWIVF */
NV_PBUS_INTR_EN_0_PCI_BUS_ERROR         TEXTEQU         <0:0>
;  RWI-V 
;   706 : #define NV_PBUS_INTR_EN_0_PCI_BUS_ERROR_DISABLED         0x00000000 /* RWI-V */
NV_PBUS_INTR_EN_0_PCI_BUS_ERROR_DISABLED                EQU             000000000h
;  RW--V 
;   707 : #define NV_PBUS_INTR_EN_0_PCI_BUS_ERROR_ENABLED          0x00000001 /* RW--V */
NV_PBUS_INTR_EN_0_PCI_BUS_ERROR_ENABLED         EQU             000000001h
;  RW-4R 
;   708 : #define NV_PBUS_RMC_DMA_0                                0x00001E80 /* RW-4R */
NV_PBUS_RMC_DMA_0               EQU             000001e80h
;  RWXUF 
;   709 : #define NV_PBUS_RMC_DMA_0_ADDRESS_BYTE_0                        7:0 /* RWXUF */
NV_PBUS_RMC_DMA_0_ADDRESS_BYTE_0                TEXTEQU         <7:0>
;  RWXUF 
;   710 : #define NV_PBUS_RMC_DMA_0_ADDRESS_BYTE_1                       15:8 /* RWXUF */
NV_PBUS_RMC_DMA_0_ADDRESS_BYTE_1                TEXTEQU         <15:8>
;  RWXUF 
;   711 : #define NV_PBUS_RMC_DMA_0_ADDRESS_BYTE_2                      23:16 /* RWXUF */
NV_PBUS_RMC_DMA_0_ADDRESS_BYTE_2                TEXTEQU         <23:16>
;  RWXVF 
;   712 : #define NV_PBUS_RMC_DMA_0_MASK_BIT                            24:24 /* RWXVF */
NV_PBUS_RMC_DMA_0_MASK_BIT              TEXTEQU         <24:24>
;  RW--V 
;   713 : #define NV_PBUS_RMC_DMA_0_MASK_BIT_CLEAR                 0x00000000 /* RW--V */
NV_PBUS_RMC_DMA_0_MASK_BIT_CLEAR                EQU             000000000h
;  RW--V 
;   714 : #define NV_PBUS_RMC_DMA_0_MASK_BIT_SET                   0x00000001 /* RW--V */
NV_PBUS_RMC_DMA_0_MASK_BIT_SET          EQU             000000001h
;  RWXVF 
;   715 : #define NV_PBUS_RMC_DMA_0_AUTOINITIALIZE                      25:25 /* RWXVF */
NV_PBUS_RMC_DMA_0_AUTOINITIALIZE                TEXTEQU         <25:25>
;  RW--V 
;   716 : #define NV_PBUS_RMC_DMA_0_AUTOINITIALIZE_DISABLED        0x00000000 /* RW--V */
NV_PBUS_RMC_DMA_0_AUTOINITIALIZE_DISABLED               EQU             000000000h
;  RW--V 
;   717 : #define NV_PBUS_RMC_DMA_0_AUTOINITIALIZE_ENABLED         0x00000001 /* RW--V */
NV_PBUS_RMC_DMA_0_AUTOINITIALIZE_ENABLED                EQU             000000001h
;  RWXVF 
;   718 : #define NV_PBUS_RMC_DMA_0_DIRECTION                           26:26 /* RWXVF */
NV_PBUS_RMC_DMA_0_DIRECTION             TEXTEQU         <26:26>
;  RW--V 
;   719 : #define NV_PBUS_RMC_DMA_0_DIRECTION_INCREMENT            0x00000000 /* RW--V */
NV_PBUS_RMC_DMA_0_DIRECTION_INCREMENT           EQU             000000000h
;  RW--V 
;   720 : #define NV_PBUS_RMC_DMA_0_DIRECTION_DECREMENT            0x00000001 /* RW--V */
NV_PBUS_RMC_DMA_0_DIRECTION_DECREMENT           EQU             000000001h
;  RWXUF 
;   721 : #define NV_PBUS_RMC_DMA_0_CHANNEL                             29:28 /* RWXUF */
NV_PBUS_RMC_DMA_0_CHANNEL               TEXTEQU         <29:28>
;  RWXVF 
;   722 : #define NV_PBUS_RMC_DMA_0_CONTROLLER                          30:30 /* RWXVF */
NV_PBUS_RMC_DMA_0_CONTROLLER            TEXTEQU         <30:30>
;  RW--V 
;   723 : #define NV_PBUS_RMC_DMA_0_CONTROLLER_DISABLED            0x00000000 /* RW--V */
NV_PBUS_RMC_DMA_0_CONTROLLER_DISABLED           EQU             000000000h
;  RW--V 
;   724 : #define NV_PBUS_RMC_DMA_0_CONTROLLER_ENABLED             0x00000001 /* RW--V */
NV_PBUS_RMC_DMA_0_CONTROLLER_ENABLED            EQU             000000001h
;  RWIVF 
;   725 : #define NV_PBUS_RMC_DMA_0_FLIPFLOP                            31:31 /* RWIVF */
NV_PBUS_RMC_DMA_0_FLIPFLOP              TEXTEQU         <31:31>
;  RWI-V 
;   726 : #define NV_PBUS_RMC_DMA_0_FLIPFLOP_BYTE_0                0x00000000 /* RWI-V */
NV_PBUS_RMC_DMA_0_FLIPFLOP_BYTE_0               EQU             000000000h
;  RW--V 
;   727 : #define NV_PBUS_RMC_DMA_0_FLIPFLOP_BYTE_1                0x00000001 /* RW--V */
NV_PBUS_RMC_DMA_0_FLIPFLOP_BYTE_1               EQU             000000001h
;  RW-4R 
;   728 : #define NV_PBUS_RMC_DMA_1                                0x00001E84 /* RW-4R */
NV_PBUS_RMC_DMA_1               EQU             000001e84h
;  RWXUF 
;   729 : #define NV_PBUS_RMC_DMA_1_COUNT_BYTE_0                          7:0 /* RWXUF */
NV_PBUS_RMC_DMA_1_COUNT_BYTE_0          TEXTEQU         <7:0>
;  RWXUF 
;   730 : #define NV_PBUS_RMC_DMA_1_COUNT_BYTE_1                         15:8 /* RWXUF */
NV_PBUS_RMC_DMA_1_COUNT_BYTE_1          TEXTEQU         <15:8>
;  RWXVF 
;   731 : #define NV_PBUS_RMC_DMA_1_STATE                               28:28 /* RWXVF */
NV_PBUS_RMC_DMA_1_STATE         TEXTEQU         <28:28>
;  RW--V 
;   732 : #define NV_PBUS_RMC_DMA_1_STATE_CLEAN                    0x00000000 /* RW--V */
NV_PBUS_RMC_DMA_1_STATE_CLEAN           EQU             000000000h
;  RW--V 
;   733 : #define NV_PBUS_RMC_DMA_1_STATE_DIRTY                    0x00000001 /* RW--V */
NV_PBUS_RMC_DMA_1_STATE_DIRTY           EQU             000000001h
;  R--4R 
;   734 : #define NV_PBUS_PCI_NV_0                                 0x00001800 /* R--4R */
NV_PBUS_PCI_NV_0                EQU             000001800h
;        
;   735 : #define NV_PBUS_PCI_NV_0__ALIAS_1                NV_CONFIG_PCI_NV_0 /*       */
NV_PBUS_PCI_NV_0__ALIAS_1               EQU             000000000h
;  RW-4R 
;   736 : #define NV_PBUS_PCI_NV_1                                 0x00001804 /* RW-4R */
NV_PBUS_PCI_NV_1                EQU             000001804h
;        
;   737 : #define NV_PBUS_PCI_NV_1__ALIAS_1                NV_CONFIG_PCI_NV_1 /*       */
NV_PBUS_PCI_NV_1__ALIAS_1               EQU             000000004h
;  R--4R 
;   738 : #define NV_PBUS_PCI_NV_2                                 0x00001808 /* R--4R */
NV_PBUS_PCI_NV_2                EQU             000001808h
;        
;   739 : #define NV_PBUS_PCI_NV_2__ALIAS_1                NV_CONFIG_PCI_NV_2 /*       */
NV_PBUS_PCI_NV_2__ALIAS_1               EQU             000000008h
;  RW-4R 
;   740 : #define NV_PBUS_PCI_NV_3                                 0x0000180C /* RW-4R */
NV_PBUS_PCI_NV_3                EQU             00000180ch
;        
;   741 : #define NV_PBUS_PCI_NV_3__ALIAS_1                NV_CONFIG_PCI_NV_3 /*       */
NV_PBUS_PCI_NV_3__ALIAS_1               EQU             00000000ch
;  RW-4R 
;   742 : #define NV_PBUS_PCI_NV_4                                 0x00001810 /* RW-4R */
NV_PBUS_PCI_NV_4                EQU             000001810h
;        
;   743 : #define NV_PBUS_PCI_NV_4__ALIAS_1                NV_CONFIG_PCI_NV_4 /*       */
NV_PBUS_PCI_NV_4__ALIAS_1               EQU             000000010h
;  RW-4R 
;   744 : #define NV_PBUS_PCI_NV_5                                 0x00001814 /* RW-4R */
NV_PBUS_PCI_NV_5                EQU             000001814h
;        
;   745 : #define NV_PBUS_PCI_NV_5__ALIAS_1                NV_CONFIG_PCI_NV_5 /*       */
NV_PBUS_PCI_NV_5__ALIAS_1               EQU             000000014h
;  RW-4R 
;   746 : #define NV_PBUS_PCI_NV_6                                 0x00001818 /* RW-4R */
NV_PBUS_PCI_NV_6                EQU             000001818h
;        
;   747 : #define NV_PBUS_PCI_NV_6__ALIAS_1                NV_CONFIG_PCI_NV_6 /*       */
NV_PBUS_PCI_NV_6__ALIAS_1               EQU             000000018h
;  R--4A 
;        
;   748 : #define NV_PBUS_PCI_NV_7(i)                      (0x0000181C+(i)*4) /* R--4A */
;   749 : #define NV_PBUS_PCI_NV_7__SIZE_1                                  4 /*       */
NV_PBUS_PCI_NV_7__SIZE_1                EQU             4t
;        
;   750 : #define NV_PBUS_PCI_NV_7__ALIAS_1                NV_CONFIG_PCI_NV_7 /*       */
NV_PBUS_PCI_NV_7__ALIAS_1               TEXTEQU         <NV_CONFIG_PCI_NV_7>
;  R--4R 
;   751 : #define NV_PBUS_PCI_NV_11                                0x0000182C /* R--4R */
NV_PBUS_PCI_NV_11               EQU             00000182ch
;        
;   752 : #define NV_PBUS_PCI_NV_11__ALIAS_1              NV_CONFIG_PCI_NV_11 /*       */
NV_PBUS_PCI_NV_11__ALIAS_1              EQU             00000002ch
;  RW-4R 
;   753 : #define NV_PBUS_PCI_NV_12                                0x00001830 /* RW-4R */
NV_PBUS_PCI_NV_12               EQU             000001830h
;        
;   754 : #define NV_PBUS_PCI_NV_12__ALIAS_1              NV_CONFIG_PCI_NV_12 /*       */
NV_PBUS_PCI_NV_12__ALIAS_1              EQU             000000030h
;  RW-4R 
;   755 : #define NV_PBUS_PCI_NV_13                                0x00001834 /* RW-4R */
NV_PBUS_PCI_NV_13               EQU             000001834h
;        
;   756 : #define NV_PBUS_PCI_NV_13__ALIAS_1              NV_CONFIG_PCI_NV_13 /*       */
NV_PBUS_PCI_NV_13__ALIAS_1              EQU             000000034h
;  R--4A 
;   757 : #define NV_PBUS_PCI_NV_14                                0x00001838 /* R--4A */
NV_PBUS_PCI_NV_14               EQU             000001838h
;        
;   758 : #define NV_PBUS_PCI_NV_14__ALIAS_1              NV_CONFIG_PCI_NV_14 /*       */
NV_PBUS_PCI_NV_14__ALIAS_1              EQU             000000038h
;  RW-4R 
;   759 : #define NV_PBUS_PCI_NV_15                                0x0000183C /* RW-4R */
NV_PBUS_PCI_NV_15               EQU             00000183ch
;        
;   760 : #define NV_PBUS_PCI_NV_15__ALIAS_1              NV_CONFIG_PCI_NV_15 /*       */
NV_PBUS_PCI_NV_15__ALIAS_1              EQU             00000003ch
;  RW-4R 
;   761 : #define NV_PBUS_PCI_NV_16                                0x00001840 /* RW-4R */
NV_PBUS_PCI_NV_16               EQU             000001840h
;        
;   762 : #define NV_PBUS_PCI_NV_16__ALIAS_1              NV_CONFIG_PCI_NV_16 /*       */
NV_PBUS_PCI_NV_16__ALIAS_1              EQU             000000040h
;  RW-4R 
;   763 : #define NV_PBUS_PCI_NV_17                                0x00001844 /* RW-4R */
NV_PBUS_PCI_NV_17               EQU             000001844h
;        
;   764 : #define NV_PBUS_PCI_NV_17__ALIAS_1              NV_CONFIG_PCI_NV_17 /*       */
NV_PBUS_PCI_NV_17__ALIAS_1              EQU             000000044h
;  RW-4R 
;   765 : #define NV_PBUS_PCI_NV_18                                0x00001848 /* RW-4R */
NV_PBUS_PCI_NV_18               EQU             000001848h
;        
;   766 : #define NV_PBUS_PCI_NV_18__ALIAS_1              NV_CONFIG_PCI_NV_18 /*       */
NV_PBUS_PCI_NV_18__ALIAS_1              EQU             000000048h
;  RW-4R 
;   767 : #define NV_PBUS_PCI_NV_19                                0x0000184C /* RW-4R */
NV_PBUS_PCI_NV_19               EQU             00000184ch
;        
;   768 : #define NV_PBUS_PCI_NV_19__ALIAS_1              NV_CONFIG_PCI_NV_19 /*       */
NV_PBUS_PCI_NV_19__ALIAS_1              EQU             00000004ch
;  RW-4R 
;   769 : #define NV_PBUS_PCI_NV_20                                0x00001850 /* RW-4R */
NV_PBUS_PCI_NV_20               EQU             000001850h
;        
;   770 : #define NV_PBUS_PCI_NV_20__ALIAS_1              NV_CONFIG_PCI_NV_20 /*       */
NV_PBUS_PCI_NV_20__ALIAS_1              EQU             000000050h
;  RW-4R 
;   771 : #define NV_PBUS_PCI_NV_21                                0x00001854 /* RW-4R */
NV_PBUS_PCI_NV_21               EQU             000001854h
;        
;   772 : #define NV_PBUS_PCI_NV_21__ALIAS_1              NV_CONFIG_PCI_NV_21 /*       */
NV_PBUS_PCI_NV_21__ALIAS_1              EQU             000000054h
;  RW-4R 
;   773 : #define NV_PBUS_PCI_NV_22                                0x00001858 /* RW-4R */
NV_PBUS_PCI_NV_22               EQU             000001858h
;        
;   774 : #define NV_PBUS_PCI_NV_22__ALIAS_1              NV_CONFIG_PCI_NV_22 /*       */
NV_PBUS_PCI_NV_22__ALIAS_1              EQU             000000058h
;  R--4A 
;        
;   775 : #define NV_PBUS_PCI_NV_23(i)                     (0x0000185C+(i)*4) /* R--4A */
;   776 : #define NV_PBUS_PCI_NV_23__SIZE_1                                41 /*       */
NV_PBUS_PCI_NV_23__SIZE_1               EQU             41t
;        
;   777 : #define NV_PBUS_PCI_NV_23__ALIAS_1              NV_CONFIG_PCI_NV_23 /*       */
NV_PBUS_PCI_NV_23__ALIAS_1              TEXTEQU         <NV_CONFIG_PCI_NV_23>
;  dev_fifo.ref 
;  RW--D 
;   778 : /* dev_fifo.ref */
;   779 : #define NV_PFIFO                              0x00003FFF:0x00002000 /* RW--D */
NV_PFIFO                TEXTEQU         <0x00003FFF:0x00002000>
;  RW-4R 
;   780 : #define NV_PFIFO_DELAY_0                                 0x00002040 /* RW-4R */
NV_PFIFO_DELAY_0                EQU             000002040h
;  RWIUF 
;   781 : #define NV_PFIFO_DELAY_0_WAIT_RETRY                             7:0 /* RWIUF */
NV_PFIFO_DELAY_0_WAIT_RETRY             TEXTEQU         <7:0>
;  RWI-V 
;   782 : #define NV_PFIFO_DELAY_0_WAIT_RETRY_0                    0x00000000 /* RWI-V */
NV_PFIFO_DELAY_0_WAIT_RETRY_0           EQU             000000000h
;  R--4R 
;   783 : #define NV_PFIFO_DEBUG_0                                 0x00002080 /* R--4R */
NV_PFIFO_DEBUG_0                EQU             000002080h
;  R-XVF 
;   784 : #define NV_PFIFO_DEBUG_0_CACHE_ERROR0                           0:0 /* R-XVF */
NV_PFIFO_DEBUG_0_CACHE_ERROR0           TEXTEQU         <0:0>
;  R---V 
;   785 : #define NV_PFIFO_DEBUG_0_CACHE_ERROR0_NOT_PENDING        0x00000000 /* R---V */
NV_PFIFO_DEBUG_0_CACHE_ERROR0_NOT_PENDING               EQU             000000000h
;  R---V 
;   786 : #define NV_PFIFO_DEBUG_0_CACHE_ERROR0_PENDING            0x00000001 /* R---V */
NV_PFIFO_DEBUG_0_CACHE_ERROR0_PENDING           EQU             000000001h
;  R-XVF 
;   787 : #define NV_PFIFO_DEBUG_0_CACHE_ERROR1                           4:4 /* R-XVF */
NV_PFIFO_DEBUG_0_CACHE_ERROR1           TEXTEQU         <4:4>
;  R---V 
;   788 : #define NV_PFIFO_DEBUG_0_CACHE_ERROR1_NOT_PENDING        0x00000000 /* R---V */
NV_PFIFO_DEBUG_0_CACHE_ERROR1_NOT_PENDING               EQU             000000000h
;  R---V 
;   789 : #define NV_PFIFO_DEBUG_0_CACHE_ERROR1_PENDING            0x00000001 /* R---V */
NV_PFIFO_DEBUG_0_CACHE_ERROR1_PENDING           EQU             000000001h
;  RW-4R 
;   790 : #define NV_PFIFO_INTR_0                                  0x00002100 /* RW-4R */
NV_PFIFO_INTR_0         EQU             000002100h
;  RWXVF 
;   791 : #define NV_PFIFO_INTR_0_CACHE_ERROR                             0:0 /* RWXVF */
NV_PFIFO_INTR_0_CACHE_ERROR             TEXTEQU         <0:0>
;  R---V 
;   792 : #define NV_PFIFO_INTR_0_CACHE_ERROR_NOT_PENDING          0x00000000 /* R---V */
NV_PFIFO_INTR_0_CACHE_ERROR_NOT_PENDING         EQU             000000000h
;  R---V 
;   793 : #define NV_PFIFO_INTR_0_CACHE_ERROR_PENDING              0x00000001 /* R---V */
NV_PFIFO_INTR_0_CACHE_ERROR_PENDING             EQU             000000001h
;  -W--V 
;   794 : #define NV_PFIFO_INTR_0_CACHE_ERROR_RESET                0x00000001 /* -W--V */
NV_PFIFO_INTR_0_CACHE_ERROR_RESET               EQU             000000001h
;  RWXVF 
;   795 : #define NV_PFIFO_INTR_0_RUNOUT                                  4:4 /* RWXVF */
NV_PFIFO_INTR_0_RUNOUT          TEXTEQU         <4:4>
;  R---V 
;   796 : #define NV_PFIFO_INTR_0_RUNOUT_NOT_PENDING               0x00000000 /* R---V */
NV_PFIFO_INTR_0_RUNOUT_NOT_PENDING              EQU             000000000h
;  R---V 
;   797 : #define NV_PFIFO_INTR_0_RUNOUT_PENDING                   0x00000001 /* R---V */
NV_PFIFO_INTR_0_RUNOUT_PENDING          EQU             000000001h
;  -W--V 
;   798 : #define NV_PFIFO_INTR_0_RUNOUT_RESET                     0x00000001 /* -W--V */
NV_PFIFO_INTR_0_RUNOUT_RESET            EQU             000000001h
;  RWXVF 
;   799 : #define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW                         8:8 /* RWXVF */
NV_PFIFO_INTR_0_RUNOUT_OVERFLOW         TEXTEQU         <8:8>
;  R---V 
;   800 : #define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW_NOT_PENDING      0x00000000 /* R---V */
NV_PFIFO_INTR_0_RUNOUT_OVERFLOW_NOT_PENDING             EQU             000000000h
;  R---V 
;   801 : #define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW_PENDING          0x00000001 /* R---V */
NV_PFIFO_INTR_0_RUNOUT_OVERFLOW_PENDING         EQU             000000001h
;  -W--V 
;   802 : #define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW_RESET            0x00000001 /* -W--V */
NV_PFIFO_INTR_0_RUNOUT_OVERFLOW_RESET           EQU             000000001h
;  RWXVF 
;   803 : #define NV_PFIFO_INTR_0_DMA_PUSHER                            12:12 /* RWXVF */
NV_PFIFO_INTR_0_DMA_PUSHER              TEXTEQU         <12:12>
;  R---V 
;   804 : #define NV_PFIFO_INTR_0_DMA_PUSHER_NOT_PENDING           0x00000000 /* R---V */
NV_PFIFO_INTR_0_DMA_PUSHER_NOT_PENDING          EQU             000000000h
;  R---V 
;   805 : #define NV_PFIFO_INTR_0_DMA_PUSHER_PENDING               0x00000001 /* R---V */
NV_PFIFO_INTR_0_DMA_PUSHER_PENDING              EQU             000000001h
;  -W--V 
;   806 : #define NV_PFIFO_INTR_0_DMA_PUSHER_RESET                 0x00000001 /* -W--V */
NV_PFIFO_INTR_0_DMA_PUSHER_RESET                EQU             000000001h
;  RWXVF 
;   807 : #define NV_PFIFO_INTR_0_DMA_PTE                               16:16 /* RWXVF */
NV_PFIFO_INTR_0_DMA_PTE         TEXTEQU         <16:16>
;  R---V 
;   808 : #define NV_PFIFO_INTR_0_DMA_PTE_NOT_PENDING              0x00000000 /* R---V */
NV_PFIFO_INTR_0_DMA_PTE_NOT_PENDING             EQU             000000000h
;  R---V 
;   809 : #define NV_PFIFO_INTR_0_DMA_PTE_PENDING                  0x00000001 /* R---V */
NV_PFIFO_INTR_0_DMA_PTE_PENDING         EQU             000000001h
;  -W--V 
;   810 : #define NV_PFIFO_INTR_0_DMA_PTE_RESET                    0x00000001 /* -W--V */
NV_PFIFO_INTR_0_DMA_PTE_RESET           EQU             000000001h
;  RW-4R 
;   811 : #define NV_PFIFO_INTR_EN_0                               0x00002140 /* RW-4R */
NV_PFIFO_INTR_EN_0              EQU             000002140h
;  RWIVF 
;   812 : #define NV_PFIFO_INTR_EN_0_CACHE_ERROR                          0:0 /* RWIVF */
NV_PFIFO_INTR_EN_0_CACHE_ERROR          TEXTEQU         <0:0>
;  RWI-V 
;   813 : #define NV_PFIFO_INTR_EN_0_CACHE_ERROR_DISABLED          0x00000000 /* RWI-V */
NV_PFIFO_INTR_EN_0_CACHE_ERROR_DISABLED         EQU             000000000h
;  RW--V 
;   814 : #define NV_PFIFO_INTR_EN_0_CACHE_ERROR_ENABLED           0x00000001 /* RW--V */
NV_PFIFO_INTR_EN_0_CACHE_ERROR_ENABLED          EQU             000000001h
;  RWIVF 
;   815 : #define NV_PFIFO_INTR_EN_0_RUNOUT                               4:4 /* RWIVF */
NV_PFIFO_INTR_EN_0_RUNOUT               TEXTEQU         <4:4>
;  RWI-V 
;   816 : #define NV_PFIFO_INTR_EN_0_RUNOUT_DISABLED               0x00000000 /* RWI-V */
NV_PFIFO_INTR_EN_0_RUNOUT_DISABLED              EQU             000000000h
;  RW--V 
;   817 : #define NV_PFIFO_INTR_EN_0_RUNOUT_ENABLED                0x00000001 /* RW--V */
NV_PFIFO_INTR_EN_0_RUNOUT_ENABLED               EQU             000000001h
;  RWIVF 
;   818 : #define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW                      8:8 /* RWIVF */
NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW              TEXTEQU         <8:8>
;  RWI-V 
;   819 : #define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW_DISABLED      0x00000000 /* RWI-V */
NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW_DISABLED             EQU             000000000h
;  RW--V 
;   820 : #define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW_ENABLED       0x00000001 /* RW--V */
NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW_ENABLED              EQU             000000001h
;  RWIVF 
;   821 : #define NV_PFIFO_INTR_EN_0_DMA_PUSHER                         12:12 /* RWIVF */
NV_PFIFO_INTR_EN_0_DMA_PUSHER           TEXTEQU         <12:12>
;  RWI-V 
;   822 : #define NV_PFIFO_INTR_EN_0_DMA_PUSHER_DISABLED           0x00000000 /* RWI-V */
NV_PFIFO_INTR_EN_0_DMA_PUSHER_DISABLED          EQU             000000000h
;  RW--V 
;   823 : #define NV_PFIFO_INTR_EN_0_DMA_PUSHER_ENABLED            0x00000001 /* RW--V */
NV_PFIFO_INTR_EN_0_DMA_PUSHER_ENABLED           EQU             000000001h
;  RWIVF 
;   824 : #define NV_PFIFO_INTR_EN_0_DMA_PTE                            16:16 /* RWIVF */
NV_PFIFO_INTR_EN_0_DMA_PTE              TEXTEQU         <16:16>
;  RWI-V 
;   825 : #define NV_PFIFO_INTR_EN_0_DMA_PTE_DISABLED              0x00000000 /* RWI-V */
NV_PFIFO_INTR_EN_0_DMA_PTE_DISABLED             EQU             000000000h
;  RW--V 
;   826 : #define NV_PFIFO_INTR_EN_0_DMA_PTE_ENABLED               0x00000001 /* RW--V */
NV_PFIFO_INTR_EN_0_DMA_PTE_ENABLED              EQU             000000001h
;  RW-4R 
;   827 : #define NV_PFIFO_CONFIG_0                                0x00002200 /* RW-4R */
NV_PFIFO_CONFIG_0               EQU             000002200h
;  RWXVF 
;   828 : #define NV_PFIFO_CONFIG_0_DMA_FETCH                            10:8 /* RWXVF */
NV_PFIFO_CONFIG_0_DMA_FETCH             TEXTEQU         <10:8>
;  RW--V 
;   829 : #define NV_PFIFO_CONFIG_0_DMA_FETCH_32_BYTES             0x00000000 /* RW--V */
NV_PFIFO_CONFIG_0_DMA_FETCH_32_BYTES            EQU             000000000h
;  RW--V 
;   830 : #define NV_PFIFO_CONFIG_0_DMA_FETCH_64_BYTES             0x00000001 /* RW--V */
NV_PFIFO_CONFIG_0_DMA_FETCH_64_BYTES            EQU             000000001h
;  RW--V 
;   831 : #define NV_PFIFO_CONFIG_0_DMA_FETCH_96_BYTES             0x00000002 /* RW--V */
NV_PFIFO_CONFIG_0_DMA_FETCH_96_BYTES            EQU             000000002h
;  RW--V 
;   832 : #define NV_PFIFO_CONFIG_0_DMA_FETCH_128_BYTES            0x00000003 /* RW--V */
NV_PFIFO_CONFIG_0_DMA_FETCH_128_BYTES           EQU             000000003h
;  RW--V 
;   833 : #define NV_PFIFO_CONFIG_0_DMA_FETCH_160_BYTES            0x00000004 /* RW--V */
NV_PFIFO_CONFIG_0_DMA_FETCH_160_BYTES           EQU             000000004h
;  RW--V 
;   834 : #define NV_PFIFO_CONFIG_0_DMA_FETCH_192_BYTES            0x00000005 /* RW--V */
NV_PFIFO_CONFIG_0_DMA_FETCH_192_BYTES           EQU             000000005h
;  RW--V 
;   835 : #define NV_PFIFO_CONFIG_0_DMA_FETCH_224_BYTES            0x00000006 /* RW--V */
NV_PFIFO_CONFIG_0_DMA_FETCH_224_BYTES           EQU             000000006h
;  RW--V 
;   836 : #define NV_PFIFO_CONFIG_0_DMA_FETCH_256_BYTES            0x00000007 /* RW--V */
NV_PFIFO_CONFIG_0_DMA_FETCH_256_BYTES           EQU             000000007h
;  RWXVF 
;   837 : #define NV_PFIFO_CONFIG_0_DMA_WATERMARK                       22:18 /* RWXVF */
NV_PFIFO_CONFIG_0_DMA_WATERMARK         TEXTEQU         <22:18>
;  RW-4R 
;   838 : #define NV_PFIFO_RAMHT                                   0x00002210 /* RW-4R */
NV_PFIFO_RAMHT          EQU             000002210h
;  RWXVF 
;   839 : #define NV_PFIFO_RAMHT_BASE_ADDRESS                           15:12 /* RWXVF */
NV_PFIFO_RAMHT_BASE_ADDRESS             TEXTEQU         <15:12>
;  RWI-V 
;   840 : #define NV_PFIFO_RAMHT_BASE_ADDRESS_0                    0x00000000 /* RWI-V */
NV_PFIFO_RAMHT_BASE_ADDRESS_0           EQU             000000000h
;  RWXVF 
;   841 : #define NV_PFIFO_RAMHT_SIZE                                   17:16 /* RWXVF */
NV_PFIFO_RAMHT_SIZE             TEXTEQU         <17:16>
;  RWI-V 
;   842 : #define NV_PFIFO_RAMHT_SIZE_4K                           0x00000000 /* RWI-V */
NV_PFIFO_RAMHT_SIZE_4K          EQU             000000000h
;  RW--V 
;   843 : #define NV_PFIFO_RAMHT_SIZE_8K                           0x00000001 /* RW--V */
NV_PFIFO_RAMHT_SIZE_8K          EQU             000000001h
;  RW--V 
;   844 : #define NV_PFIFO_RAMHT_SIZE_16K                          0x00000002 /* RW--V */
NV_PFIFO_RAMHT_SIZE_16K         EQU             000000002h
;  RW--V 
;   845 : #define NV_PFIFO_RAMHT_SIZE_32K                          0x00000003 /* RW--V */
NV_PFIFO_RAMHT_SIZE_32K         EQU             000000003h
;  RW-4R 
;   846 : #define NV_PFIFO_RAMFC                                   0x00002214 /* RW-4R */
NV_PFIFO_RAMFC          EQU             000002214h
;  RWXVF 
;   847 : #define NV_PFIFO_RAMFC_BASE_ADDRESS                            15:9 /* RWXVF */
NV_PFIFO_RAMFC_BASE_ADDRESS             TEXTEQU         <15:9>
;  RWI-V 
;   848 : #define NV_PFIFO_RAMFC_BASE_ADDRESS_1C00                 0x00001C00 /* RWI-V */
NV_PFIFO_RAMFC_BASE_ADDRESS_1C00                EQU             000001c00h
;  RW-4R 
;   849 : #define NV_PFIFO_RAMRO                                   0x00002218 /* RW-4R */
NV_PFIFO_RAMRO          EQU             000002218h
;  RWXVF 
;   850 : #define NV_PFIFO_RAMRO_BASE_ADDRESS                            15:9 /* RWXVF */
NV_PFIFO_RAMRO_BASE_ADDRESS             TEXTEQU         <15:9>
;  RWI-V 
;   851 : #define NV_PFIFO_RAMRO_BASE_ADDRESS_1E00                 0x00001E00 /* RWI-V */
NV_PFIFO_RAMRO_BASE_ADDRESS_1E00                EQU             000001e00h
;  RWXVF 
;   852 : #define NV_PFIFO_RAMRO_SIZE                                   16:16 /* RWXVF */
NV_PFIFO_RAMRO_SIZE             TEXTEQU         <16:16>
;  RWI-V 
;   853 : #define NV_PFIFO_RAMRO_SIZE_512                          0x00000000 /* RWI-V */
NV_PFIFO_RAMRO_SIZE_512         EQU             000000000h
;  RW--V 
;   854 : #define NV_PFIFO_RAMRO_SIZE_8K                           0x00000001 /* RW--V */
NV_PFIFO_RAMRO_SIZE_8K          EQU             000000001h
;  RW-4R 
;   855 : #define NV_PFIFO_CACHES                                  0x00002500 /* RW-4R */
NV_PFIFO_CACHES         EQU             000002500h
;  RWIVF 
;   856 : #define NV_PFIFO_CACHES_REASSIGN                                0:0 /* RWIVF */
NV_PFIFO_CACHES_REASSIGN                TEXTEQU         <0:0>
;  RWI-V 
;   857 : #define NV_PFIFO_CACHES_REASSIGN_DISABLED                0x00000000 /* RWI-V */
NV_PFIFO_CACHES_REASSIGN_DISABLED               EQU             000000000h
;  RW--V 
;   858 : #define NV_PFIFO_CACHES_REASSIGN_ENABLED                 0x00000001 /* RW--V */
NV_PFIFO_CACHES_REASSIGN_ENABLED                EQU             000000001h
;  RW-4R 
;   859 : #define NV_PFIFO_CACHE0_PUSH0                            0x00003000 /* RW-4R */
NV_PFIFO_CACHE0_PUSH0           EQU             000003000h
;  RWIVF 
;   860 : #define NV_PFIFO_CACHE0_PUSH0_ACCESS                            0:0 /* RWIVF */
NV_PFIFO_CACHE0_PUSH0_ACCESS            TEXTEQU         <0:0>
;  RWI-V 
;   861 : #define NV_PFIFO_CACHE0_PUSH0_ACCESS_DISABLED            0x00000000 /* RWI-V */
NV_PFIFO_CACHE0_PUSH0_ACCESS_DISABLED           EQU             000000000h
;  RW--V 
;   862 : #define NV_PFIFO_CACHE0_PUSH0_ACCESS_ENABLED             0x00000001 /* RW--V */
NV_PFIFO_CACHE0_PUSH0_ACCESS_ENABLED            EQU             000000001h
;  RW-4R 
;   863 : #define NV_PFIFO_CACHE1_PUSH0                            0x00003200 /* RW-4R */
NV_PFIFO_CACHE1_PUSH0           EQU             000003200h
;  RWIVF 
;   864 : #define NV_PFIFO_CACHE1_PUSH0_ACCESS                            0:0 /* RWIVF */
NV_PFIFO_CACHE1_PUSH0_ACCESS            TEXTEQU         <0:0>
;  RWI-V 
;   865 : #define NV_PFIFO_CACHE1_PUSH0_ACCESS_DISABLED            0x00000000 /* RWI-V */
NV_PFIFO_CACHE1_PUSH0_ACCESS_DISABLED           EQU             000000000h
;  RW--V 
;   866 : #define NV_PFIFO_CACHE1_PUSH0_ACCESS_ENABLED             0x00000001 /* RW--V */
NV_PFIFO_CACHE1_PUSH0_ACCESS_ENABLED            EQU             000000001h
;  RW-4R 
;   867 : #define NV_PFIFO_CACHE0_PUSH1                            0x00003004 /* RW-4R */
NV_PFIFO_CACHE0_PUSH1           EQU             000003004h
;  RWXUF 
;   868 : #define NV_PFIFO_CACHE0_PUSH1_CHID                              6:0 /* RWXUF */
NV_PFIFO_CACHE0_PUSH1_CHID              TEXTEQU         <6:0>
;  RW-4R 
;   869 : #define NV_PFIFO_CACHE1_PUSH1                            0x00003204 /* RW-4R */
NV_PFIFO_CACHE1_PUSH1           EQU             000003204h
;  RWXUF 
;   870 : #define NV_PFIFO_CACHE1_PUSH1_CHID                              6:0 /* RWXUF */
NV_PFIFO_CACHE1_PUSH1_CHID              TEXTEQU         <6:0>
;  RW-4R 
;   871 : #define NV_PFIFO_CACHE1_DMA0                             0x00003220 /* RW-4R */
NV_PFIFO_CACHE1_DMA0            EQU             000003220h
;  RWIVF 
;   872 : #define NV_PFIFO_CACHE1_DMA0_ACCESS                             0:0 /* RWIVF */
NV_PFIFO_CACHE1_DMA0_ACCESS             TEXTEQU         <0:0>
;  RWI-V 
;   873 : #define NV_PFIFO_CACHE1_DMA0_ACCESS_DISABLED             0x00000000 /* RWI-V */
NV_PFIFO_CACHE1_DMA0_ACCESS_DISABLED            EQU             000000000h
;  RW--V 
;   874 : #define NV_PFIFO_CACHE1_DMA0_ACCESS_ENABLED              0x00000001 /* RW--V */
NV_PFIFO_CACHE1_DMA0_ACCESS_ENABLED             EQU             000000001h
;  R-IVF 
;   875 : #define NV_PFIFO_CACHE1_DMA0_STATE                              4:4 /* R-IVF */
NV_PFIFO_CACHE1_DMA0_STATE              TEXTEQU         <4:4>
;  R-I-V 
;   876 : #define NV_PFIFO_CACHE1_DMA0_STATE_IDLE                  0x00000000 /* R-I-V */
NV_PFIFO_CACHE1_DMA0_STATE_IDLE         EQU             000000000h
;  R---V 
;   877 : #define NV_PFIFO_CACHE1_DMA0_STATE_BUSY                  0x00000001 /* R---V */
NV_PFIFO_CACHE1_DMA0_STATE_BUSY         EQU             000000001h
;  RW-4R 
;   878 : #define NV_PFIFO_CACHE1_DMA1                             0x00003224 /* RW-4R */
NV_PFIFO_CACHE1_DMA1            EQU             000003224h
;  RWXUF 
;   879 : #define NV_PFIFO_CACHE1_DMA1_LENGTH                            23:2 /* RWXUF */
NV_PFIFO_CACHE1_DMA1_LENGTH             TEXTEQU         <23:2>
;  RW-4R 
;   880 : #define NV_PFIFO_CACHE1_DMA2                             0x00003228 /* RW-4R */
NV_PFIFO_CACHE1_DMA2            EQU             000003228h
;  RWXUF 
;   881 : #define NV_PFIFO_CACHE1_DMA2_ADDRESS                           23:2 /* RWXUF */
NV_PFIFO_CACHE1_DMA2_ADDRESS            TEXTEQU         <23:2>
;  RW-4R 
;   882 : #define NV_PFIFO_CACHE1_DMA3                             0x0000322C /* RW-4R */
NV_PFIFO_CACHE1_DMA3            EQU             00000322ch
;  RWXUF 
;   883 : #define NV_PFIFO_CACHE1_DMA3_TARGET_NODE                        1:0 /* RWXUF */
NV_PFIFO_CACHE1_DMA3_TARGET_NODE                TEXTEQU         <1:0>
;  RW--V 
;   884 : #define NV_PFIFO_CACHE1_DMA3_TARGET_NODE_PCI             0x00000002 /* RW--V */
NV_PFIFO_CACHE1_DMA3_TARGET_NODE_PCI            EQU             000000002h
;  RW--V 
;   885 : #define NV_PFIFO_CACHE1_DMA3_TARGET_NODE_AGP             0x00000003 /* RW--V */
NV_PFIFO_CACHE1_DMA3_TARGET_NODE_AGP            EQU             000000003h
;  RW-4R 
;   886 : #define NV_PFIFO_CACHE1_DMA_STATUS                       0x00003218 /* RW-4R */
NV_PFIFO_CACHE1_DMA_STATUS              EQU             000003218h
;  RWXUF 
;   887 : #define NV_PFIFO_CACHE1_DMA_STATUS_METHOD                      12:2 /* RWXUF */
NV_PFIFO_CACHE1_DMA_STATUS_METHOD               TEXTEQU         <12:2>
;  RWXUF 
;   888 : #define NV_PFIFO_CACHE1_DMA_STATUS_SUBCHANNEL                 15:13 /* RWXUF */
NV_PFIFO_CACHE1_DMA_STATUS_SUBCHANNEL           TEXTEQU         <15:13>
;  RWXUF 
;   889 : #define NV_PFIFO_CACHE1_DMA_STATUS_METHOD_COUNT               28:18 /* RWXUF */
NV_PFIFO_CACHE1_DMA_STATUS_METHOD_COUNT         TEXTEQU         <28:18>
;  RWXUF 
;   890 : #define NV_PFIFO_CACHE1_DMA_STATUS_REASON                     30:30 /* RWXUF */
NV_PFIFO_CACHE1_DMA_STATUS_REASON               TEXTEQU         <30:30>
;  RW--V 
;   891 : #define NV_PFIFO_CACHE1_DMA_STATUS_REASON_DONE           0x00000000 /* RW--V */
NV_PFIFO_CACHE1_DMA_STATUS_REASON_DONE          EQU             000000000h
;  RW--V 
;   892 : #define NV_PFIFO_CACHE1_DMA_STATUS_REASON_NON_CACHE      0x00000001 /* RW--V */
NV_PFIFO_CACHE1_DMA_STATUS_REASON_NON_CACHE             EQU             000000001h
;  RW-4R 
;   893 : #define NV_PFIFO_CACHE1_DMA_TLB_TAG                      0x00003230 /* RW-4R */
NV_PFIFO_CACHE1_DMA_TLB_TAG             EQU             000003230h
;  RWXUF 
;   894 : #define NV_PFIFO_CACHE1_DMA_TLB_TAG_ADDRESS                   23:12 /* RWXUF */
NV_PFIFO_CACHE1_DMA_TLB_TAG_ADDRESS             TEXTEQU         <23:12>
;  RW-4R 
;   895 : #define NV_PFIFO_CACHE1_DMA_TLB_PTE                      0x00003234 /* RW-4R */
NV_PFIFO_CACHE1_DMA_TLB_PTE             EQU             000003234h
;  RWXVF 
;   896 : #define NV_PFIFO_CACHE1_DMA_TLB_PTE_PAGE                        0:0 /* RWXVF */
NV_PFIFO_CACHE1_DMA_TLB_PTE_PAGE                TEXTEQU         <0:0>
;  RW--V 
;   897 : #define NV_PFIFO_CACHE1_DMA_TLB_PTE_PAGE_NOT_PRESENT     0x00000000 /* RW--V */
NV_PFIFO_CACHE1_DMA_TLB_PTE_PAGE_NOT_PRESENT            EQU             000000000h
;  RW--V 
;   898 : #define NV_PFIFO_CACHE1_DMA_TLB_PTE_PAGE_PRESENT         0x00000001 /* RW--V */
NV_PFIFO_CACHE1_DMA_TLB_PTE_PAGE_PRESENT                EQU             000000001h
;  RWXUF 
;   899 : #define NV_PFIFO_CACHE1_DMA_TLB_PTE_FRAME_ADDRESS             31:12 /* RWXUF */
NV_PFIFO_CACHE1_DMA_TLB_PTE_FRAME_ADDRESS               TEXTEQU         <31:12>
;  RW-4R 
;   900 : #define NV_PFIFO_CACHE1_DMA_TLB_PT_BASE                  0x00003238 /* RW-4R */
NV_PFIFO_CACHE1_DMA_TLB_PT_BASE         EQU             000003238h
;  RWXUF 
;   901 : #define NV_PFIFO_CACHE1_DMA_TLB_PT_BASE_ADDRESS                19:2 /* RWXUF */
NV_PFIFO_CACHE1_DMA_TLB_PT_BASE_ADDRESS         TEXTEQU         <19:2>
;  RW-4R 
;   902 : #define NV_PFIFO_CACHE0_PULL0                            0x00003040 /* RW-4R */
NV_PFIFO_CACHE0_PULL0           EQU             000003040h
;  RWIVF 
;   903 : #define NV_PFIFO_CACHE0_PULL0_ACCESS                            0:0 /* RWIVF */
NV_PFIFO_CACHE0_PULL0_ACCESS            TEXTEQU         <0:0>
;  RWI-V 
;   904 : #define NV_PFIFO_CACHE0_PULL0_ACCESS_DISABLED            0x00000000 /* RWI-V */
NV_PFIFO_CACHE0_PULL0_ACCESS_DISABLED           EQU             000000000h
;  RW--V 
;   905 : #define NV_PFIFO_CACHE0_PULL0_ACCESS_ENABLED             0x00000001 /* RW--V */
NV_PFIFO_CACHE0_PULL0_ACCESS_ENABLED            EQU             000000001h
;  R-XVF 
;   906 : #define NV_PFIFO_CACHE0_PULL0_HASH                              4:4 /* R-XVF */
NV_PFIFO_CACHE0_PULL0_HASH              TEXTEQU         <4:4>
;  R---V 
;   907 : #define NV_PFIFO_CACHE0_PULL0_HASH_SUCCEEDED             0x00000000 /* R---V */
NV_PFIFO_CACHE0_PULL0_HASH_SUCCEEDED            EQU             000000000h
;  R---V 
;   908 : #define NV_PFIFO_CACHE0_PULL0_HASH_FAILED                0x00000001 /* R---V */
NV_PFIFO_CACHE0_PULL0_HASH_FAILED               EQU             000000001h
;  R-XVF 
;   909 : #define NV_PFIFO_CACHE0_PULL0_DEVICE                            8:8 /* R-XVF */
NV_PFIFO_CACHE0_PULL0_DEVICE            TEXTEQU         <8:8>
;  R---V 
;   910 : #define NV_PFIFO_CACHE0_PULL0_DEVICE_HARDWARE            0x00000000 /* R---V */
NV_PFIFO_CACHE0_PULL0_DEVICE_HARDWARE           EQU             000000000h
;  R---V 
;   911 : #define NV_PFIFO_CACHE0_PULL0_DEVICE_SOFTWARE            0x00000001 /* R---V */
NV_PFIFO_CACHE0_PULL0_DEVICE_SOFTWARE           EQU             000000001h
;  RW-4R 
;   912 : #define NV_PFIFO_CACHE1_PULL0                            0x00003240 /* RW-4R */
NV_PFIFO_CACHE1_PULL0           EQU             000003240h
;  RWIVF 
;   913 : #define NV_PFIFO_CACHE1_PULL0_ACCESS                            0:0 /* RWIVF */
NV_PFIFO_CACHE1_PULL0_ACCESS            TEXTEQU         <0:0>
;  RWI-V 
;   914 : #define NV_PFIFO_CACHE1_PULL0_ACCESS_DISABLED            0x00000000 /* RWI-V */
NV_PFIFO_CACHE1_PULL0_ACCESS_DISABLED           EQU             000000000h
;  RW--V 
;   915 : #define NV_PFIFO_CACHE1_PULL0_ACCESS_ENABLED             0x00000001 /* RW--V */
NV_PFIFO_CACHE1_PULL0_ACCESS_ENABLED            EQU             000000001h
;  R-XVF 
;   916 : #define NV_PFIFO_CACHE1_PULL0_HASH                              4:4 /* R-XVF */
NV_PFIFO_CACHE1_PULL0_HASH              TEXTEQU         <4:4>
;  R---V 
;   917 : #define NV_PFIFO_CACHE1_PULL0_HASH_SUCCEEDED             0x00000000 /* R---V */
NV_PFIFO_CACHE1_PULL0_HASH_SUCCEEDED            EQU             000000000h
;  R---V 
;   918 : #define NV_PFIFO_CACHE1_PULL0_HASH_FAILED                0x00000001 /* R---V */
NV_PFIFO_CACHE1_PULL0_HASH_FAILED               EQU             000000001h
;  R-XVF 
;   919 : #define NV_PFIFO_CACHE1_PULL0_DEVICE                            8:8 /* R-XVF */
NV_PFIFO_CACHE1_PULL0_DEVICE            TEXTEQU         <8:8>
;  R---V 
;   920 : #define NV_PFIFO_CACHE1_PULL0_DEVICE_HARDWARE            0x00000000 /* R---V */
NV_PFIFO_CACHE1_PULL0_DEVICE_HARDWARE           EQU             000000000h
;  R---V 
;   921 : #define NV_PFIFO_CACHE1_PULL0_DEVICE_SOFTWARE            0x00000001 /* R---V */
NV_PFIFO_CACHE1_PULL0_DEVICE_SOFTWARE           EQU             000000001h
;  RW-4R 
;   922 : #define NV_PFIFO_CACHE1_PULL1                            0x00003250 /* RW-4R */
NV_PFIFO_CACHE1_PULL1           EQU             000003250h
;  RWXVF 
;   923 : #define NV_PFIFO_CACHE1_PULL1_CTX                               4:4 /* RWXVF */
NV_PFIFO_CACHE1_PULL1_CTX               TEXTEQU         <4:4>
;  RW--V 
;   924 : #define NV_PFIFO_CACHE1_PULL1_CTX_CLEAN                  0x00000000 /* RW--V */
NV_PFIFO_CACHE1_PULL1_CTX_CLEAN         EQU             000000000h
;  RW--V 
;   925 : #define NV_PFIFO_CACHE1_PULL1_CTX_DIRTY                  0x00000001 /* RW--V */
NV_PFIFO_CACHE1_PULL1_CTX_DIRTY         EQU             000000001h
;  R--4R 
;   926 : #define NV_PFIFO_CACHE0_STATUS                           0x00003014 /* R--4R */
NV_PFIFO_CACHE0_STATUS          EQU             000003014h
;  R--VF 
;   927 : #define NV_PFIFO_CACHE0_STATUS_LOW_MARK                         4:4 /* R--VF */
NV_PFIFO_CACHE0_STATUS_LOW_MARK         TEXTEQU         <4:4>
;  R---V 
;   928 : #define NV_PFIFO_CACHE0_STATUS_LOW_MARK_NOT_EMPTY        0x00000000 /* R---V */
NV_PFIFO_CACHE0_STATUS_LOW_MARK_NOT_EMPTY               EQU             000000000h
;  R---V 
;   929 : #define NV_PFIFO_CACHE0_STATUS_LOW_MARK_EMPTY            0x00000001 /* R---V */
NV_PFIFO_CACHE0_STATUS_LOW_MARK_EMPTY           EQU             000000001h
;  R--VF 
;   930 : #define NV_PFIFO_CACHE0_STATUS_HIGH_MARK                        8:8 /* R--VF */
NV_PFIFO_CACHE0_STATUS_HIGH_MARK                TEXTEQU         <8:8>
;  R---V 
;   931 : #define NV_PFIFO_CACHE0_STATUS_HIGH_MARK_NOT_FULL        0x00000000 /* R---V */
NV_PFIFO_CACHE0_STATUS_HIGH_MARK_NOT_FULL               EQU             000000000h
;  R---V 
;   932 : #define NV_PFIFO_CACHE0_STATUS_HIGH_MARK_FULL            0x00000001 /* R---V */
NV_PFIFO_CACHE0_STATUS_HIGH_MARK_FULL           EQU             000000001h
;  R--4R 
;   933 : #define NV_PFIFO_CACHE1_STATUS                           0x00003214 /* R--4R */
NV_PFIFO_CACHE1_STATUS          EQU             000003214h
;  R-XVF 
;   934 : #define NV_PFIFO_CACHE1_STATUS_RANOUT                           0:0 /* R-XVF */
NV_PFIFO_CACHE1_STATUS_RANOUT           TEXTEQU         <0:0>
;  R---V 
;   935 : #define NV_PFIFO_CACHE1_STATUS_RANOUT_FALSE              0x00000000 /* R---V */
NV_PFIFO_CACHE1_STATUS_RANOUT_FALSE             EQU             000000000h
;  R---V 
;   936 : #define NV_PFIFO_CACHE1_STATUS_RANOUT_TRUE               0x00000001 /* R---V */
NV_PFIFO_CACHE1_STATUS_RANOUT_TRUE              EQU             000000001h
;  R--VF 
;   937 : #define NV_PFIFO_CACHE1_STATUS_LOW_MARK                         4:4 /* R--VF */
NV_PFIFO_CACHE1_STATUS_LOW_MARK         TEXTEQU         <4:4>
;  R---V 
;   938 : #define NV_PFIFO_CACHE1_STATUS_LOW_MARK_NOT_EMPTY        0x00000000 /* R---V */
NV_PFIFO_CACHE1_STATUS_LOW_MARK_NOT_EMPTY               EQU             000000000h
;  R---V 
;   939 : #define NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY            0x00000001 /* R---V */
NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY           EQU             000000001h
;  R--VF 
;   940 : #define NV_PFIFO_CACHE1_STATUS_HIGH_MARK                        8:8 /* R--VF */
NV_PFIFO_CACHE1_STATUS_HIGH_MARK                TEXTEQU         <8:8>
;  R---V 
;   941 : #define NV_PFIFO_CACHE1_STATUS_HIGH_MARK_NOT_FULL        0x00000000 /* R---V */
NV_PFIFO_CACHE1_STATUS_HIGH_MARK_NOT_FULL               EQU             000000000h
;  R---V 
;   942 : #define NV_PFIFO_CACHE1_STATUS_HIGH_MARK_FULL            0x00000001 /* R---V */
NV_PFIFO_CACHE1_STATUS_HIGH_MARK_FULL           EQU             000000001h
;  RW-4R 
;   943 : #define NV_PFIFO_CACHE0_PUT                              0x00003010 /* RW-4R */
NV_PFIFO_CACHE0_PUT             EQU             000003010h
;  RWXUF 
;   944 : #define NV_PFIFO_CACHE0_PUT_ADDRESS                             2:2 /* RWXUF */
NV_PFIFO_CACHE0_PUT_ADDRESS             TEXTEQU         <2:2>
;  RW-4R 
;   945 : #define NV_PFIFO_CACHE1_PUT                              0x00003210 /* RW-4R */
NV_PFIFO_CACHE1_PUT             EQU             000003210h
;  RWXUF 
;   946 : #define NV_PFIFO_CACHE1_PUT_ADDRESS                             6:2 /* RWXUF */
NV_PFIFO_CACHE1_PUT_ADDRESS             TEXTEQU         <6:2>
;  RW-4R 
;   947 : #define NV_PFIFO_CACHE0_GET                              0x00003070 /* RW-4R */
NV_PFIFO_CACHE0_GET             EQU             000003070h
;  RWXUF 
;   948 : #define NV_PFIFO_CACHE0_GET_ADDRESS                             2:2 /* RWXUF */
NV_PFIFO_CACHE0_GET_ADDRESS             TEXTEQU         <2:2>
;  RW-4R 
;   949 : #define NV_PFIFO_CACHE1_GET                              0x00003270 /* RW-4R */
NV_PFIFO_CACHE1_GET             EQU             000003270h
;  RWXUF 
;   950 : #define NV_PFIFO_CACHE1_GET_ADDRESS                             6:2 /* RWXUF */
NV_PFIFO_CACHE1_GET_ADDRESS             TEXTEQU         <6:2>
;  RW-4A 
;        
;   951 : #define NV_PFIFO_CACHE0_CTX(i)                  (0x00003080+(i)*16) /* RW-4A */
;   952 : #define NV_PFIFO_CACHE0_CTX__SIZE_1                               1 /*       */
NV_PFIFO_CACHE0_CTX__SIZE_1             EQU             1t
;  RWXUF 
;   953 : #define NV_PFIFO_CACHE0_CTX_INSTANCE                           15:0 /* RWXUF */
NV_PFIFO_CACHE0_CTX_INSTANCE            TEXTEQU         <15:0>
;  RWXUF 
;   954 : #define NV_PFIFO_CACHE0_CTX_DEVICE                            22:16 /* RWXUF */
NV_PFIFO_CACHE0_CTX_DEVICE              TEXTEQU         <22:16>
;  RWXVF 
;   955 : #define NV_PFIFO_CACHE0_CTX_ENGINE                            23:23 /* RWXVF */
NV_PFIFO_CACHE0_CTX_ENGINE              TEXTEQU         <23:23>
;  RW--V 
;   956 : #define NV_PFIFO_CACHE0_CTX_ENGINE_SW                    0x00000000 /* RW--V */
NV_PFIFO_CACHE0_CTX_ENGINE_SW           EQU             000000000h
;  RW--V 
;   957 : #define NV_PFIFO_CACHE0_CTX_ENGINE_GRAPHICS              0x00000001 /* RW--V */
NV_PFIFO_CACHE0_CTX_ENGINE_GRAPHICS             EQU             000000001h
;  RW-4A 
;        
;   958 : #define NV_PFIFO_CACHE1_CTX(i)                  (0x00003280+(i)*16) /* RW-4A */
;   959 : #define NV_PFIFO_CACHE1_CTX__SIZE_1                               8 /*       */
NV_PFIFO_CACHE1_CTX__SIZE_1             EQU             8t
;  RWXUF 
;   960 : #define NV_PFIFO_CACHE1_CTX_INSTANCE                           15:0 /* RWXUF */
NV_PFIFO_CACHE1_CTX_INSTANCE            TEXTEQU         <15:0>
;  RWXUF 
;   961 : #define NV_PFIFO_CACHE1_CTX_DEVICE                            22:16 /* RWXUF */
NV_PFIFO_CACHE1_CTX_DEVICE              TEXTEQU         <22:16>
;  RWXVF 
;   962 : #define NV_PFIFO_CACHE1_CTX_ENGINE                            23:23 /* RWXVF */
NV_PFIFO_CACHE1_CTX_ENGINE              TEXTEQU         <23:23>
;  RW--V 
;   963 : #define NV_PFIFO_CACHE1_CTX_ENGINE_SW                    0x00000000 /* RW--V */
NV_PFIFO_CACHE1_CTX_ENGINE_SW           EQU             000000000h
;  RW--V 
;   964 : #define NV_PFIFO_CACHE1_CTX_ENGINE_GRAPHICS              0x00000001 /* RW--V */
NV_PFIFO_CACHE1_CTX_ENGINE_GRAPHICS             EQU             000000001h
;  RW-4A 
;        
;   965 : #define NV_PFIFO_CACHE0_METHOD(i)                (0x00003100+(i)*8) /* RW-4A */
;   966 : #define NV_PFIFO_CACHE0_METHOD__SIZE_1                            1 /*       */
NV_PFIFO_CACHE0_METHOD__SIZE_1          EQU             1t
;  RWXUF 
;   967 : #define NV_PFIFO_CACHE0_METHOD_ADDRESS                         12:2 /* RWXUF */
NV_PFIFO_CACHE0_METHOD_ADDRESS          TEXTEQU         <12:2>
;  RWXUF 
;   968 : #define NV_PFIFO_CACHE0_METHOD_SUBCHANNEL                     15:13 /* RWXUF */
NV_PFIFO_CACHE0_METHOD_SUBCHANNEL               TEXTEQU         <15:13>
;  RW-4A 
;        
;   969 : #define NV_PFIFO_CACHE1_METHOD(i)                (0x00003300+(i)*8) /* RW-4A */
;   970 : #define NV_PFIFO_CACHE1_METHOD__SIZE_1                           32 /*       */
NV_PFIFO_CACHE1_METHOD__SIZE_1          EQU             32t
;  RWXUF 
;   971 : #define NV_PFIFO_CACHE1_METHOD_ADDRESS                         12:2 /* RWXUF */
NV_PFIFO_CACHE1_METHOD_ADDRESS          TEXTEQU         <12:2>
;  RWXUF 
;   972 : #define NV_PFIFO_CACHE1_METHOD_SUBCHANNEL                     15:13 /* RWXUF */
NV_PFIFO_CACHE1_METHOD_SUBCHANNEL               TEXTEQU         <15:13>
;  RW-4A 
;        
;   973 : #define NV_PFIFO_CACHE0_DATA(i)                  (0x00003104+(i)*8) /* RW-4A */
;   974 : #define NV_PFIFO_CACHE0_DATA__SIZE_1                              1 /*       */
NV_PFIFO_CACHE0_DATA__SIZE_1            EQU             1t
;  RWXVF 
;   975 : #define NV_PFIFO_CACHE0_DATA_VALUE                             31:0 /* RWXVF */
NV_PFIFO_CACHE0_DATA_VALUE              TEXTEQU         <31:0>
;  RW-4A 
;        
;   976 : #define NV_PFIFO_CACHE1_DATA(i)                  (0x00003304+(i)*8) /* RW-4A */
;   977 : #define NV_PFIFO_CACHE1_DATA__SIZE_1                             32 /*       */
NV_PFIFO_CACHE1_DATA__SIZE_1            EQU             32t
;  RWXVF 
;   978 : #define NV_PFIFO_CACHE1_DATA_VALUE                             31:0 /* RWXVF */
NV_PFIFO_CACHE1_DATA_VALUE              TEXTEQU         <31:0>
;  R--4A 
;        
;   979 : #define NV_PFIFO_DEVICE(i)                       (0x00002800+(i)*4) /* R--4A */
;   980 : #define NV_PFIFO_DEVICE__SIZE_1                                 128 /*       */
NV_PFIFO_DEVICE__SIZE_1         EQU             128t
;  R--UF 
;   981 : #define NV_PFIFO_DEVICE_CHID                                    6:0 /* R--UF */
NV_PFIFO_DEVICE_CHID            TEXTEQU         <6:0>
;  R--VF 
;   982 : #define NV_PFIFO_DEVICE_SWITCH                                24:24 /* R--VF */
NV_PFIFO_DEVICE_SWITCH          TEXTEQU         <24:24>
;  R---V 
;   983 : #define NV_PFIFO_DEVICE_SWITCH_UNAVAILABLE               0x00000000 /* R---V */
NV_PFIFO_DEVICE_SWITCH_UNAVAILABLE              EQU             000000000h
;  R---V 
;   984 : #define NV_PFIFO_DEVICE_SWITCH_AVAILABLE                 0x00000001 /* R---V */
NV_PFIFO_DEVICE_SWITCH_AVAILABLE                EQU             000000001h
;  R--4R 
;   985 : #define NV_PFIFO_RUNOUT_STATUS                           0x00002400 /* R--4R */
NV_PFIFO_RUNOUT_STATUS          EQU             000002400h
;  R--VF 
;   986 : #define NV_PFIFO_RUNOUT_STATUS_RANOUT                           0:0 /* R--VF */
NV_PFIFO_RUNOUT_STATUS_RANOUT           TEXTEQU         <0:0>
;  R---V 
;   987 : #define NV_PFIFO_RUNOUT_STATUS_RANOUT_FALSE              0x00000000 /* R---V */
NV_PFIFO_RUNOUT_STATUS_RANOUT_FALSE             EQU             000000000h
;  R---V 
;   988 : #define NV_PFIFO_RUNOUT_STATUS_RANOUT_TRUE               0x00000001 /* R---V */
NV_PFIFO_RUNOUT_STATUS_RANOUT_TRUE              EQU             000000001h
;  R--VF 
;   989 : #define NV_PFIFO_RUNOUT_STATUS_LOW_MARK                         4:4 /* R--VF */
NV_PFIFO_RUNOUT_STATUS_LOW_MARK         TEXTEQU         <4:4>
;  R---V 
;   990 : #define NV_PFIFO_RUNOUT_STATUS_LOW_MARK_NOT_EMPTY        0x00000000 /* R---V */
NV_PFIFO_RUNOUT_STATUS_LOW_MARK_NOT_EMPTY               EQU             000000000h
;  R---V 
;   991 : #define NV_PFIFO_RUNOUT_STATUS_LOW_MARK_EMPTY            0x00000001 /* R---V */
NV_PFIFO_RUNOUT_STATUS_LOW_MARK_EMPTY           EQU             000000001h
;  R--VF 
;   992 : #define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK                        8:8 /* R--VF */
NV_PFIFO_RUNOUT_STATUS_HIGH_MARK                TEXTEQU         <8:8>
;  R---V 
;   993 : #define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK_NOT_FULL        0x00000000 /* R---V */
NV_PFIFO_RUNOUT_STATUS_HIGH_MARK_NOT_FULL               EQU             000000000h
;  R---V 
;   994 : #define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK_FULL            0x00000001 /* R---V */
NV_PFIFO_RUNOUT_STATUS_HIGH_MARK_FULL           EQU             000000001h
;  RW-4R 
;   995 : #define NV_PFIFO_RUNOUT_PUT                              0x00002410 /* RW-4R */
NV_PFIFO_RUNOUT_PUT             EQU             000002410h
;  RWXUF 
;   996 : #define NV_PFIFO_RUNOUT_PUT_ADDRESS                            12:3 /* RWXUF */
NV_PFIFO_RUNOUT_PUT_ADDRESS             TEXTEQU         <12:3>
;        
;   997 : #define NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_0                     8:3 /*       */
NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_0             TEXTEQU         <8:3>
;        
;   998 : #define NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_1                    12:3 /*       */
NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_1             TEXTEQU         <12:3>
;  RW-4R 
;   999 : #define NV_PFIFO_RUNOUT_GET                              0x00002420 /* RW-4R */
NV_PFIFO_RUNOUT_GET             EQU             000002420h
;  RWXUF 
;  1000 : #define NV_PFIFO_RUNOUT_GET_ADDRESS                            13:3 /* RWXUF */
NV_PFIFO_RUNOUT_GET_ADDRESS             TEXTEQU         <13:3>
;  dev_audio.ref 
;  RW--D 
;  1001 : /* dev_audio.ref */
;  1002 : #define NV_PAUDIO                             0x00300FFF:0x00300000 /* RW--D */
NV_PAUDIO               TEXTEQU         <0x00300FFF:0x00300000>
;  RW-4R 
;  1003 : #define NV_PAUDIO_PINS                                   0x00300000 /* RW-4R */
NV_PAUDIO_PINS          EQU             000300000h
;  RWIVF 
;  1004 : #define NV_PAUDIO_PINS_SI_DELAY                                 0:0 /* RWIVF */
NV_PAUDIO_PINS_SI_DELAY         TEXTEQU         <0:0>
;  RWI-V 
;  1005 : #define NV_PAUDIO_PINS_SI_DELAY_NORMAL                   0x00000000 /* RWI-V */
NV_PAUDIO_PINS_SI_DELAY_NORMAL          EQU             000000000h
;  RW--V 
;  1006 : #define NV_PAUDIO_PINS_SI_DELAY_I2S                      0x00000001 /* RW--V */
NV_PAUDIO_PINS_SI_DELAY_I2S             EQU             000000001h
;  RWIVF 
;  1007 : #define NV_PAUDIO_PINS_SI_EDGE                                  1:1 /* RWIVF */
NV_PAUDIO_PINS_SI_EDGE          TEXTEQU         <1:1>
;  RWI-V 
;  1008 : #define NV_PAUDIO_PINS_SI_EDGE_NEGATIVE                  0x00000000 /* RWI-V */
NV_PAUDIO_PINS_SI_EDGE_NEGATIVE         EQU             000000000h
;  RW--V 
;  1009 : #define NV_PAUDIO_PINS_SI_EDGE_POSITIVE                  0x00000001 /* RW--V */
NV_PAUDIO_PINS_SI_EDGE_POSITIVE         EQU             000000001h
;  RWIVF 
;  1010 : #define NV_PAUDIO_PINS_SO_DELAY                                 4:4 /* RWIVF */
NV_PAUDIO_PINS_SO_DELAY         TEXTEQU         <4:4>
;  RWI-V 
;  1011 : #define NV_PAUDIO_PINS_SO_DELAY_NORMAL                   0x00000000 /* RWI-V */
NV_PAUDIO_PINS_SO_DELAY_NORMAL          EQU             000000000h
;  RW--V 
;  1012 : #define NV_PAUDIO_PINS_SO_DELAY_I2S                      0x00000001 /* RW--V */
NV_PAUDIO_PINS_SO_DELAY_I2S             EQU             000000001h
;  RWIVF 
;  1013 : #define NV_PAUDIO_PINS_SO_EDGE                                  5:5 /* RWIVF */
NV_PAUDIO_PINS_SO_EDGE          TEXTEQU         <5:5>
;  RWI-V 
;  1014 : #define NV_PAUDIO_PINS_SO_EDGE_NEGATIVE                  0x00000000 /* RWI-V */
NV_PAUDIO_PINS_SO_EDGE_NEGATIVE         EQU             000000000h
;  RW--V 
;  1015 : #define NV_PAUDIO_PINS_SO_EDGE_POSITIVE                  0x00000001 /* RW--V */
NV_PAUDIO_PINS_SO_EDGE_POSITIVE         EQU             000000001h
;  RWIVF 
;  1016 : #define NV_PAUDIO_PINS_FMT_AC97                                 8:8 /* RWIVF */
NV_PAUDIO_PINS_FMT_AC97         TEXTEQU         <8:8>
;  RWIVF 
;  1017 : #define NV_PAUDIO_PINS_SERIAL                                   9:9 /* RWIVF */
NV_PAUDIO_PINS_SERIAL           TEXTEQU         <9:9>
;  RWI-V 
;  1018 : #define NV_PAUDIO_PINS_SERIAL_DISABLED                   0x00000000 /* RWI-V */
NV_PAUDIO_PINS_SERIAL_DISABLED          EQU             000000000h
;  RW--V 
;  1019 : #define NV_PAUDIO_PINS_SERIAL_ENABLED                    0x00000001 /* RW--V */
NV_PAUDIO_PINS_SERIAL_ENABLED           EQU             000000001h
;  RWIVF 
;  1020 : #define NV_PAUDIO_PINS_LEFT_JUST                              10:10 /* RWIVF */
NV_PAUDIO_PINS_LEFT_JUST                TEXTEQU         <10:10>
;  RWIVF 
;  1021 : #define NV_PAUDIO_PINS_LEFT_SENSE                             11:11 /* RWIVF */
NV_PAUDIO_PINS_LEFT_SENSE               TEXTEQU         <11:11>
;  RWI-V 
;  1022 : #define NV_PAUDIO_PINS_LEFT_LOW                          0x00000000 /* RWI-V */
NV_PAUDIO_PINS_LEFT_LOW         EQU             000000000h
;  RW--V 
;  1023 : #define NV_PAUDIO_PINS_LEFT_HIGH                         0x00000001 /* RW--V */
NV_PAUDIO_PINS_LEFT_HIGH                EQU             000000001h
;  RWIVF 
;  1024 : #define NV_PAUDIO_PINS_CONTROL                                14:12 /* RWIVF */
NV_PAUDIO_PINS_CONTROL          TEXTEQU         <14:12>
;  RWI-V 
;  1025 : #define NV_PAUDIO_PINS_CONTROL_0                         0x00000000 /* RWI-V */
NV_PAUDIO_PINS_CONTROL_0                EQU             000000000h
;  RW--V 
;  1026 : #define NV_PAUDIO_PINS_CONTROL_1                         0x00000001 /* RW--V */
NV_PAUDIO_PINS_CONTROL_1                EQU             000000001h
;  RW--V 
;  1027 : #define NV_PAUDIO_PINS_CONTROL_2                         0x00000002 /* RW--V */
NV_PAUDIO_PINS_CONTROL_2                EQU             000000002h
;  RW--V 
;  1028 : #define NV_PAUDIO_PINS_CONTROL_3                         0x00000003 /* RW--V */
NV_PAUDIO_PINS_CONTROL_3                EQU             000000003h
;  RW--V 
;  1029 : #define NV_PAUDIO_PINS_CONTROL_4                         0x00000004 /* RW--V */
NV_PAUDIO_PINS_CONTROL_4                EQU             000000004h
;  RW--V 
;  1030 : #define NV_PAUDIO_PINS_CONTROL_5                         0x00000005 /* RW--V */
NV_PAUDIO_PINS_CONTROL_5                EQU             000000005h
;  RW--V 
;  1031 : #define NV_PAUDIO_PINS_CONTROL_6                         0x00000006 /* RW--V */
NV_PAUDIO_PINS_CONTROL_6                EQU             000000006h
;  RW--V 
;  1032 : #define NV_PAUDIO_PINS_CONTROL_7                         0x00000007 /* RW--V */
NV_PAUDIO_PINS_CONTROL_7                EQU             000000007h
;  RW-4R 
;  1033 : #define NV_PAUDIO_GREEN_0                                0x003000C0 /* RW-4R */
NV_PAUDIO_GREEN_0               EQU             0003000c0h
;  RWIVF 
;  1034 : #define NV_PAUDIO_GREEN_0_CODEC                                 0:0 /* RWIVF */
NV_PAUDIO_GREEN_0_CODEC         TEXTEQU         <0:0>
;  RWI-V 
;  1035 : #define NV_PAUDIO_GREEN_0_CODEC_DISABLED                 0x00000000 /* RWI-V */
NV_PAUDIO_GREEN_0_CODEC_DISABLED                EQU             000000000h
;  RW--V 
;  1036 : #define NV_PAUDIO_GREEN_0_CODEC_ENABLED                  0x00000001 /* RW--V */
NV_PAUDIO_GREEN_0_CODEC_ENABLED         EQU             000000001h
;  RWIVF 
;  1037 : #define NV_PAUDIO_GREEN_0_AUDIO                                 4:4 /* RWIVF */
NV_PAUDIO_GREEN_0_AUDIO         TEXTEQU         <4:4>
;  RWI-V 
;  1038 : #define NV_PAUDIO_GREEN_0_AUDIO_DISABLED                 0x00000000 /* RWI-V */
NV_PAUDIO_GREEN_0_AUDIO_DISABLED                EQU             000000000h
;  RW--V 
;  1039 : #define NV_PAUDIO_GREEN_0_AUDIO_ENABLED                  0x00000001 /* RW--V */
NV_PAUDIO_GREEN_0_AUDIO_ENABLED         EQU             000000001h
;  RWIVF 
;  1040 : #define NV_PAUDIO_GREEN_0_APUMP                                 8:8 /* RWIVF */
NV_PAUDIO_GREEN_0_APUMP         TEXTEQU         <8:8>
;  RWI-V 
;  1041 : #define NV_PAUDIO_GREEN_0_APUMP_DISABLED                 0x00000000 /* RWI-V */
NV_PAUDIO_GREEN_0_APUMP_DISABLED                EQU             000000000h
;  RW--V 
;  1042 : #define NV_PAUDIO_GREEN_0_APUMP_ENABLED                  0x00000001 /* RW--V */
NV_PAUDIO_GREEN_0_APUMP_ENABLED         EQU             000000001h
;  RWIVF 
;  1043 : #define NV_PAUDIO_GREEN_0_CODEC_WARM                            8:8 /* RWIVF */
NV_PAUDIO_GREEN_0_CODEC_WARM            TEXTEQU         <8:8>
;  RWI-V 
;  1044 : #define NV_PAUDIO_GREEN_0_CODEC_WARM_END                 0x00000000 /* RWI-V */
NV_PAUDIO_GREEN_0_CODEC_WARM_END                EQU             000000000h
;  RW--V 
;  1045 : #define NV_PAUDIO_GREEN_0_APUMP_WARM_UP                  0x00000001 /* RW--V */
NV_PAUDIO_GREEN_0_APUMP_WARM_UP         EQU             000000001h
;  RW-4R 
;  1046 : #define NV_PAUDIO_INTR_0                                 0x00300100 /* RW-4R */
NV_PAUDIO_INTR_0                EQU             000300100h
;  RWIVF 
;  1047 : #define NV_PAUDIO_INTR_0_NOTIFY                                 0:0 /* RWIVF */
NV_PAUDIO_INTR_0_NOTIFY         TEXTEQU         <0:0>
;  R-I-V 
;  1048 : #define NV_PAUDIO_INTR_0_NOTIFY_NOT_PENDING              0x00000000 /* R-I-V */
NV_PAUDIO_INTR_0_NOTIFY_NOT_PENDING             EQU             000000000h
;  R---V 
;  1049 : #define NV_PAUDIO_INTR_0_NOTIFY_PENDING                  0x00000001 /* R---V */
NV_PAUDIO_INTR_0_NOTIFY_PENDING         EQU             000000001h
;  -W--V 
;  1050 : #define NV_PAUDIO_INTR_0_NOTIFY_RESET                    0x00000001 /* -W--V */
NV_PAUDIO_INTR_0_NOTIFY_RESET           EQU             000000001h
;  RWIVF 
;  1051 : #define NV_PAUDIO_INTR_0_OVERFLOW                               4:4 /* RWIVF */
NV_PAUDIO_INTR_0_OVERFLOW               TEXTEQU         <4:4>
;  R-I-V 
;  1052 : #define NV_PAUDIO_INTR_0_OVERFLOW_NOT_PENDING            0x00000000 /* R-I-V */
NV_PAUDIO_INTR_0_OVERFLOW_NOT_PENDING           EQU             000000000h
;  R---V 
;  1053 : #define NV_PAUDIO_INTR_0_OVERFLOW_PENDING                0x00000001 /* R---V */
NV_PAUDIO_INTR_0_OVERFLOW_PENDING               EQU             000000001h
;  -W--V 
;  1054 : #define NV_PAUDIO_INTR_0_OVERFLOW_RESET                  0x00000001 /* -W--V */
NV_PAUDIO_INTR_0_OVERFLOW_RESET         EQU             000000001h
;  RWIVF 
;  1055 : #define NV_PAUDIO_INTR_0_AN_OVFL                                8:8 /* RWIVF */
NV_PAUDIO_INTR_0_AN_OVFL                TEXTEQU         <8:8>
;  R-I-V 
;  1056 : #define NV_PAUDIO_INTR_0_AN_OVFL_NOT_PENDING             0x00000000 /* R-I-V */
NV_PAUDIO_INTR_0_AN_OVFL_NOT_PENDING            EQU             000000000h
;  R---V 
;  1057 : #define NV_PAUDIO_INTR_0_AN_OVFL_PENDING                 0x00000001 /* R---V */
NV_PAUDIO_INTR_0_AN_OVFL_PENDING                EQU             000000001h
;  -W--V 
;  1058 : #define NV_PAUDIO_INTR_0_AN_OVFL_RESET                   0x00000001 /* -W--V */
NV_PAUDIO_INTR_0_AN_OVFL_RESET          EQU             000000001h
;  RWIVF 
;  1059 : #define NV_PAUDIO_INTR_0_AN_UNFL                              12:12 /* RWIVF */
NV_PAUDIO_INTR_0_AN_UNFL                TEXTEQU         <12:12>
;  R-I-V 
;  1060 : #define NV_PAUDIO_INTR_0_AN_UNFL_NOT_PENDING             0x00000000 /* R-I-V */
NV_PAUDIO_INTR_0_AN_UNFL_NOT_PENDING            EQU             000000000h
;  R---V 
;  1061 : #define NV_PAUDIO_INTR_0_AN_UNFL_PENDING                 0x00000001 /* R---V */
NV_PAUDIO_INTR_0_AN_UNFL_PENDING                EQU             000000001h
;  -W--V 
;  1062 : #define NV_PAUDIO_INTR_0_AN_UNFL_RESET                   0x00000001 /* -W--V */
NV_PAUDIO_INTR_0_AN_UNFL_RESET          EQU             000000001h
;  RW-4R 
;  1063 : #define NV_PAUDIO_INTR_1                                 0x00300104 /* RW-4R */
NV_PAUDIO_INTR_1                EQU             000300104h
;  RWIVF 
;  1064 : #define NV_PAUDIO_INTR_1_SYNC                                   0:0 /* RWIVF */
NV_PAUDIO_INTR_1_SYNC           TEXTEQU         <0:0>
;  R-I-V 
;  1065 : #define NV_PAUDIO_INTR_1_SYNC_NOT_PENDING                0x00000000 /* R-I-V */
NV_PAUDIO_INTR_1_SYNC_NOT_PENDING               EQU             000000000h
;  R---V 
;  1066 : #define NV_PAUDIO_INTR_1_SYNC_PENDING                    0x00000001 /* R---V */
NV_PAUDIO_INTR_1_SYNC_PENDING           EQU             000000001h
;  -W--V 
;  1067 : #define NV_PAUDIO_INTR_1_SYNC_RESET                      0x00000001 /* -W--V */
NV_PAUDIO_INTR_1_SYNC_RESET             EQU             000000001h
;  RWIVF 
;  1068 : #define NV_PAUDIO_INTR_1_IDLE                                   4:4 /* RWIVF */
NV_PAUDIO_INTR_1_IDLE           TEXTEQU         <4:4>
;  R-I-V 
;  1069 : #define NV_PAUDIO_INTR_1_IDLE_NOT_PENDING                0x00000000 /* R-I-V */
NV_PAUDIO_INTR_1_IDLE_NOT_PENDING               EQU             000000000h
;  R---V 
;  1070 : #define NV_PAUDIO_INTR_1_IDLE_PENDING                    0x00000001 /* R---V */
NV_PAUDIO_INTR_1_IDLE_PENDING           EQU             000000001h
;  -W--V 
;  1071 : #define NV_PAUDIO_INTR_1_IDLE_RESET                      0x00000001 /* -W--V */
NV_PAUDIO_INTR_1_IDLE_RESET             EQU             000000001h
;  RWIVF 
;  1072 : #define NV_PAUDIO_INTR_1_NEAR_BUSY                              8:8 /* RWIVF */
NV_PAUDIO_INTR_1_NEAR_BUSY              TEXTEQU         <8:8>
;  R-I-V 
;  1073 : #define NV_PAUDIO_INTR_1_NEAR_BUSY_NOT_PENDING           0x00000000 /* R-I-V */
NV_PAUDIO_INTR_1_NEAR_BUSY_NOT_PENDING          EQU             000000000h
;  R---V 
;  1074 : #define NV_PAUDIO_INTR_1_NEAR_BUSY_PENDING               0x00000001 /* R---V */
NV_PAUDIO_INTR_1_NEAR_BUSY_PENDING              EQU             000000001h
;  -W--V 
;  1075 : #define NV_PAUDIO_INTR_1_NEAR_BUSY_RESET                 0x00000001 /* -W--V */
NV_PAUDIO_INTR_1_NEAR_BUSY_RESET                EQU             000000001h
;  RWIVF 
;  1076 : #define NV_PAUDIO_INTR_1_TOO_BUSY                             12:12 /* RWIVF */
NV_PAUDIO_INTR_1_TOO_BUSY               TEXTEQU         <12:12>
;  R-I-V 
;  1077 : #define NV_PAUDIO_INTR_1_TOO_BUSY_NOT_PENDING            0x00000000 /* R-I-V */
NV_PAUDIO_INTR_1_TOO_BUSY_NOT_PENDING           EQU             000000000h
;  R---V 
;  1078 : #define NV_PAUDIO_INTR_1_TOO_BUSY_PENDING                0x00000001 /* R---V */
NV_PAUDIO_INTR_1_TOO_BUSY_PENDING               EQU             000000001h
;  -W--V 
;  1079 : #define NV_PAUDIO_INTR_1_TOO_BUSY_RESET                  0x00000001 /* -W--V */
NV_PAUDIO_INTR_1_TOO_BUSY_RESET         EQU             000000001h
;  RW-4R 
;  1080 : #define NV_PAUDIO_INTR_2                                 0x00300108 /* RW-4R */
NV_PAUDIO_INTR_2                EQU             000300108h
;  RWIVF 
;  1081 : #define NV_PAUDIO_INTR_2_DMA_INSTANCE                           0:0 /* RWIVF */
NV_PAUDIO_INTR_2_DMA_INSTANCE           TEXTEQU         <0:0>
;  R-I-V 
;  1082 : #define NV_PAUDIO_INTR_2_DMA_INSTANCE_NOT_PENDING        0x00000000 /* R-I-V */
NV_PAUDIO_INTR_2_DMA_INSTANCE_NOT_PENDING               EQU             000000000h
;  R---V 
;  1083 : #define NV_PAUDIO_INTR_2_DMA_INSTANCE_PENDING            0x00000001 /* R---V */
NV_PAUDIO_INTR_2_DMA_INSTANCE_PENDING           EQU             000000001h
;  -W--V 
;  1084 : #define NV_PAUDIO_INTR_2_DMA_INSTANCE_RESET              0x00000001 /* -W--V */
NV_PAUDIO_INTR_2_DMA_INSTANCE_RESET             EQU             000000001h
;  RWIVF 
;  1085 : #define NV_PAUDIO_INTR_2_DMA_PRESENT                            4:4 /* RWIVF */
NV_PAUDIO_INTR_2_DMA_PRESENT            TEXTEQU         <4:4>
;  R-I-V 
;  1086 : #define NV_PAUDIO_INTR_2_DMA_PRESENT_NOT_PENDING         0x00000000 /* R-I-V */
NV_PAUDIO_INTR_2_DMA_PRESENT_NOT_PENDING                EQU             000000000h
;  R---V 
;  1087 : #define NV_PAUDIO_INTR_2_DMA_PRESENT_PENDING             0x00000001 /* R---V */
NV_PAUDIO_INTR_2_DMA_PRESENT_PENDING            EQU             000000001h
;  -W--V 
;  1088 : #define NV_PAUDIO_INTR_2_DMA_PRESENT_RESET               0x00000001 /* -W--V */
NV_PAUDIO_INTR_2_DMA_PRESENT_RESET              EQU             000000001h
;  RWIVF 
;  1089 : #define NV_PAUDIO_INTR_2_DMA_PROTECT                            8:8 /* RWIVF */
NV_PAUDIO_INTR_2_DMA_PROTECT            TEXTEQU         <8:8>
;  R-I-V 
;  1090 : #define NV_PAUDIO_INTR_2_DMA_PROTECT_NOT_PENDING         0x00000000 /* R-I-V */
NV_PAUDIO_INTR_2_DMA_PROTECT_NOT_PENDING                EQU             000000000h
;  R---V 
;  1091 : #define NV_PAUDIO_INTR_2_DMA_PROTECT_PENDING             0x00000001 /* R---V */
NV_PAUDIO_INTR_2_DMA_PROTECT_PENDING            EQU             000000001h
;  -W--V 
;  1092 : #define NV_PAUDIO_INTR_2_DMA_PROTECT_RESET               0x00000001 /* -W--V */
NV_PAUDIO_INTR_2_DMA_PROTECT_RESET              EQU             000000001h
;  RWIVF 
;  1093 : #define NV_PAUDIO_INTR_2_DMA_BUS_ERROR                        12:12 /* RWIVF */
NV_PAUDIO_INTR_2_DMA_BUS_ERROR          TEXTEQU         <12:12>
;  R-I-V 
;  1094 : #define NV_PAUDIO_INTR_2_DMA_BUS_ERROR_NOT_PENDING       0x00000000 /* R-I-V */
NV_PAUDIO_INTR_2_DMA_BUS_ERROR_NOT_PENDING              EQU             000000000h
;  R---V 
;  1095 : #define NV_PAUDIO_INTR_2_DMA_BUS_ERROR_PENDING           0x00000001 /* R---V */
NV_PAUDIO_INTR_2_DMA_BUS_ERROR_PENDING          EQU             000000001h
;  -W--V 
;  1096 : #define NV_PAUDIO_INTR_2_DMA_BUS_ERROR_RESET             0x00000001 /* -W--V */
NV_PAUDIO_INTR_2_DMA_BUS_ERROR_RESET            EQU             000000001h
;  RW-4R 
;  1097 : #define NV_PAUDIO_INTR_3                                 0x0030010C /* RW-4R */
NV_PAUDIO_INTR_3                EQU             00030010ch
;  RWIVF 
;  1098 : #define NV_PAUDIO_INTR_3_PINS_0                                 0:0 /* RWIVF */
NV_PAUDIO_INTR_3_PINS_0         TEXTEQU         <0:0>
;  R-I-V 
;  1099 : #define NV_PAUDIO_INTR_3_PINS_0_NOT_PENDING              0x00000000 /* R-I-V */
NV_PAUDIO_INTR_3_PINS_0_NOT_PENDING             EQU             000000000h
;  R---V 
;  1100 : #define NV_PAUDIO_INTR_3_PINS_0_PENDING                  0x00000001 /* R---V */
NV_PAUDIO_INTR_3_PINS_0_PENDING         EQU             000000001h
;  -W--V 
;  1101 : #define NV_PAUDIO_INTR_3_PINS_0_RESET                    0x00000001 /* -W--V */
NV_PAUDIO_INTR_3_PINS_0_RESET           EQU             000000001h
;  RWIVF 
;  1102 : #define NV_PAUDIO_INTR_3_PINS_1                                 4:4 /* RWIVF */
NV_PAUDIO_INTR_3_PINS_1         TEXTEQU         <4:4>
;  R-I-V 
;  1103 : #define NV_PAUDIO_INTR_3_PINS_1_NOT_PENDING              0x00000000 /* R-I-V */
NV_PAUDIO_INTR_3_PINS_1_NOT_PENDING             EQU             000000000h
;  R---V 
;  1104 : #define NV_PAUDIO_INTR_3_PINS_1_PENDING                  0x00000001 /* R---V */
NV_PAUDIO_INTR_3_PINS_1_PENDING         EQU             000000001h
;  -W--V 
;  1105 : #define NV_PAUDIO_INTR_3_PINS_1_RESET                    0x00000001 /* -W--V */
NV_PAUDIO_INTR_3_PINS_1_RESET           EQU             000000001h
;  RWIVF 
;  1106 : #define NV_PAUDIO_INTR_3_PINS_2                                 8:8 /* RWIVF */
NV_PAUDIO_INTR_3_PINS_2         TEXTEQU         <8:8>
;  R-I-V 
;  1107 : #define NV_PAUDIO_INTR_3_PINS_2_NOT_PENDING              0x00000000 /* R-I-V */
NV_PAUDIO_INTR_3_PINS_2_NOT_PENDING             EQU             000000000h
;  R---V 
;  1108 : #define NV_PAUDIO_INTR_3_PINS_2_PENDING                  0x00000001 /* R---V */
NV_PAUDIO_INTR_3_PINS_2_PENDING         EQU             000000001h
;  -W--V 
;  1109 : #define NV_PAUDIO_INTR_3_PINS_2_RESET                    0x00000001 /* -W--V */
NV_PAUDIO_INTR_3_PINS_2_RESET           EQU             000000001h
;  RWIVF 
;  1110 : #define NV_PAUDIO_INTR_3_PINS_3                               12:12 /* RWIVF */
NV_PAUDIO_INTR_3_PINS_3         TEXTEQU         <12:12>
;  R-I-V 
;  1111 : #define NV_PAUDIO_INTR_3_PINS_3_NOT_PENDING              0x00000000 /* R-I-V */
NV_PAUDIO_INTR_3_PINS_3_NOT_PENDING             EQU             000000000h
;  R---V 
;  1112 : #define NV_PAUDIO_INTR_3_PINS_3_PENDING                  0x00000001 /* R---V */
NV_PAUDIO_INTR_3_PINS_3_PENDING         EQU             000000001h
;  -W--V 
;  1113 : #define NV_PAUDIO_INTR_3_PINS_3_RESET                    0x00000001 /* -W--V */
NV_PAUDIO_INTR_3_PINS_3_RESET           EQU             000000001h
;  RW-4R 
;  1114 : #define NV_PAUDIO_INTR_EN_0                              0x00300140 /* RW-4R */
NV_PAUDIO_INTR_EN_0             EQU             000300140h
;  RWIVF 
;  1115 : #define NV_PAUDIO_INTR_EN_0_NOTIFY                              0:0 /* RWIVF */
NV_PAUDIO_INTR_EN_0_NOTIFY              TEXTEQU         <0:0>
;  RWI-V 
;  1116 : #define NV_PAUDIO_INTR_EN_0_NOTIFY_DISABLED              0x00000000 /* RWI-V */
NV_PAUDIO_INTR_EN_0_NOTIFY_DISABLED             EQU             000000000h
;  RW--V 
;  1117 : #define NV_PAUDIO_INTR_EN_0_NOTIFY_ENABLED               0x00000001 /* RW--V */
NV_PAUDIO_INTR_EN_0_NOTIFY_ENABLED              EQU             000000001h
;  RWIVF 
;  1118 : #define NV_PAUDIO_INTR_EN_0_OVERFLOW                            4:4 /* RWIVF */
NV_PAUDIO_INTR_EN_0_OVERFLOW            TEXTEQU         <4:4>
;  RWI-V 
;  1119 : #define NV_PAUDIO_INTR_EN_0_OVERFLOW_DISABLED            0x00000000 /* RWI-V */
NV_PAUDIO_INTR_EN_0_OVERFLOW_DISABLED           EQU             000000000h
;  RW--V 
;  1120 : #define NV_PAUDIO_INTR_EN_0_OVERFLOW_ENABLED             0x00000001 /* RW--V */
NV_PAUDIO_INTR_EN_0_OVERFLOW_ENABLED            EQU             000000001h
;  RWIVF 
;  1121 : #define NV_PAUDIO_INTR_EN_0_AN_OVFL                             8:8 /* RWIVF */
NV_PAUDIO_INTR_EN_0_AN_OVFL             TEXTEQU         <8:8>
;  RWI-V 
;  1122 : #define NV_PAUDIO_INTR_EN_0_AN_OVFL_DISABLED             0x00000000 /* RWI-V */
NV_PAUDIO_INTR_EN_0_AN_OVFL_DISABLED            EQU             000000000h
;  RW--V 
;  1123 : #define NV_PAUDIO_INTR_EN_0_AN_OVFL_ENABLED              0x00000001 /* RW--V */
NV_PAUDIO_INTR_EN_0_AN_OVFL_ENABLED             EQU             000000001h
;  RWIVF 
;  1124 : #define NV_PAUDIO_INTR_EN_0_AN_UNFL                           12:12 /* RWIVF */
NV_PAUDIO_INTR_EN_0_AN_UNFL             TEXTEQU         <12:12>
;  RWI-V 
;  1125 : #define NV_PAUDIO_INTR_EN_0_AN_UNFL_DISABLED             0x00000000 /* RWI-V */
NV_PAUDIO_INTR_EN_0_AN_UNFL_DISABLED            EQU             000000000h
;  RW--V 
;  1126 : #define NV_PAUDIO_INTR_EN_0_AN_UNFL_ENABLED              0x00000001 /* RW--V */
NV_PAUDIO_INTR_EN_0_AN_UNFL_ENABLED             EQU             000000001h
;  RW-4R 
;  1127 : #define NV_PAUDIO_INTR_EN_1                              0x00300144 /* RW-4R */
NV_PAUDIO_INTR_EN_1             EQU             000300144h
;  RWIVF 
;  1128 : #define NV_PAUDIO_INTR_EN_1_SYNC                                0:0 /* RWIVF */
NV_PAUDIO_INTR_EN_1_SYNC                TEXTEQU         <0:0>
;  RWI-V 
;  1129 : #define NV_PAUDIO_INTR_EN_1_SYNC_DISABLED                0x00000000 /* RWI-V */
NV_PAUDIO_INTR_EN_1_SYNC_DISABLED               EQU             000000000h
;  RW--V 
;  1130 : #define NV_PAUDIO_INTR_EN_1_SYNC_ENABLED                 0x00000001 /* RW--V */
NV_PAUDIO_INTR_EN_1_SYNC_ENABLED                EQU             000000001h
;  RWIVF 
;  1131 : #define NV_PAUDIO_INTR_EN_1_IDLE                                4:4 /* RWIVF */
NV_PAUDIO_INTR_EN_1_IDLE                TEXTEQU         <4:4>
;  RWI-V 
;  1132 : #define NV_PAUDIO_INTR_EN_1_IDLE_DISABLED                0x00000000 /* RWI-V */
NV_PAUDIO_INTR_EN_1_IDLE_DISABLED               EQU             000000000h
;  RW--V 
;  1133 : #define NV_PAUDIO_INTR_EN_1_IDLE_ENABLED                 0x00000001 /* RW--V */
NV_PAUDIO_INTR_EN_1_IDLE_ENABLED                EQU             000000001h
;  RWIVF 
;  1134 : #define NV_PAUDIO_INTR_EN_1_NEAR_BUSY                           8:8 /* RWIVF */
NV_PAUDIO_INTR_EN_1_NEAR_BUSY           TEXTEQU         <8:8>
;  RWI-V 
;  1135 : #define NV_PAUDIO_INTR_EN_1_NEAR_BUSY_DISABLED           0x00000000 /* RWI-V */
NV_PAUDIO_INTR_EN_1_NEAR_BUSY_DISABLED          EQU             000000000h
;  RW--V 
;  1136 : #define NV_PAUDIO_INTR_EN_1_NEAR_BUSY_ENABLED            0x00000001 /* RW--V */
NV_PAUDIO_INTR_EN_1_NEAR_BUSY_ENABLED           EQU             000000001h
;  RWIVF 
;  1137 : #define NV_PAUDIO_INTR_EN_1_TOO_BUSY                          12:12 /* RWIVF */
NV_PAUDIO_INTR_EN_1_TOO_BUSY            TEXTEQU         <12:12>
;  RWI-V 
;  1138 : #define NV_PAUDIO_INTR_EN_1_TOO_BUSY_DISABLED            0x00000000 /* RWI-V */
NV_PAUDIO_INTR_EN_1_TOO_BUSY_DISABLED           EQU             000000000h
;  RW--V 
;  1139 : #define NV_PAUDIO_INTR_EN_1_TOO_BUSY_ENABLED             0x00000001 /* RW--V */
NV_PAUDIO_INTR_EN_1_TOO_BUSY_ENABLED            EQU             000000001h
;  RW-4R 
;  1140 : #define NV_PAUDIO_INTR_EN_2                              0x00300148 /* RW-4R */
NV_PAUDIO_INTR_EN_2             EQU             000300148h
;  RWIVF 
;  1141 : #define NV_PAUDIO_INTR_EN_2_DMA_INSTANCE                        0:0 /* RWIVF */
NV_PAUDIO_INTR_EN_2_DMA_INSTANCE                TEXTEQU         <0:0>
;  R-I-V 
;  1142 : #define NV_PAUDIO_INTR_EN_2_DMA_INSTANCE_NOT_PENDING     0x00000000 /* R-I-V */
NV_PAUDIO_INTR_EN_2_DMA_INSTANCE_NOT_PENDING            EQU             000000000h
;  R---V 
;  1143 : #define NV_PAUDIO_INTR_EN_2_DMA_INSTANCE_PENDING         0x00000001 /* R---V */
NV_PAUDIO_INTR_EN_2_DMA_INSTANCE_PENDING                EQU             000000001h
;  -W--V 
;  1144 : #define NV_PAUDIO_INTR_EN_2_DMA_INSTANCE_RESET           0x00000001 /* -W--V */
NV_PAUDIO_INTR_EN_2_DMA_INSTANCE_RESET          EQU             000000001h
;  RWIVF 
;  1145 : #define NV_PAUDIO_INTR_EN_2_DMA_PRESENT                         4:4 /* RWIVF */
NV_PAUDIO_INTR_EN_2_DMA_PRESENT         TEXTEQU         <4:4>
;  R-I-V 
;  1146 : #define NV_PAUDIO_INTR_EN_2_DMA_PRESENT_NOT_PENDING      0x00000000 /* R-I-V */
NV_PAUDIO_INTR_EN_2_DMA_PRESENT_NOT_PENDING             EQU             000000000h
;  R---V 
;  1147 : #define NV_PAUDIO_INTR_EN_2_DMA_PRESENT_PENDING          0x00000001 /* R---V */
NV_PAUDIO_INTR_EN_2_DMA_PRESENT_PENDING         EQU             000000001h
;  -W--V 
;  1148 : #define NV_PAUDIO_INTR_EN_2_DMA_PRESENT_RESET            0x00000001 /* -W--V */
NV_PAUDIO_INTR_EN_2_DMA_PRESENT_RESET           EQU             000000001h
;  RWIVF 
;  1149 : #define NV_PAUDIO_INTR_EN_2_DMA_PROTECT                         8:8 /* RWIVF */
NV_PAUDIO_INTR_EN_2_DMA_PROTECT         TEXTEQU         <8:8>
;  R-I-V 
;  1150 : #define NV_PAUDIO_INTR_EN_2_DMA_PROTECT_NOT_PENDING      0x00000000 /* R-I-V */
NV_PAUDIO_INTR_EN_2_DMA_PROTECT_NOT_PENDING             EQU             000000000h
;  R---V 
;  1151 : #define NV_PAUDIO_INTR_EN_2_DMA_PROTECT_PENDING          0x00000001 /* R---V */
NV_PAUDIO_INTR_EN_2_DMA_PROTECT_PENDING         EQU             000000001h
;  -W--V 
;  1152 : #define NV_PAUDIO_INTR_EN_2_DMA_PROTECT_RESET            0x00000001 /* -W--V */
NV_PAUDIO_INTR_EN_2_DMA_PROTECT_RESET           EQU             000000001h
;  RWIVF 
;  1153 : #define NV_PAUDIO_INTR_EN_2_DMA_BUS_ERROR                     12:12 /* RWIVF */
NV_PAUDIO_INTR_EN_2_DMA_BUS_ERROR               TEXTEQU         <12:12>
;  R-I-V 
;  1154 : #define NV_PAUDIO_INTR_EN_2_DMA_BUS_ERROR_NOT_PENDING    0x00000000 /* R-I-V */
NV_PAUDIO_INTR_EN_2_DMA_BUS_ERROR_NOT_PENDING           EQU             000000000h
;  R---V 
;  1155 : #define NV_PAUDIO_INTR_EN_2_DMA_BUS_ERROR_PENDING        0x00000001 /* R---V */
NV_PAUDIO_INTR_EN_2_DMA_BUS_ERROR_PENDING               EQU             000000001h
;  -W--V 
;  1156 : #define NV_PAUDIO_INTR_EN_2_DMA_BUS_ERROR_RESET          0x00000001 /* -W--V */
NV_PAUDIO_INTR_EN_2_DMA_BUS_ERROR_RESET         EQU             000000001h
;  RW-4R 
;  1157 : #define NV_PAUDIO_INTR_EN_3                              0x0030014C /* RW-4R */
NV_PAUDIO_INTR_EN_3             EQU             00030014ch
;  RWIVF 
;  1158 : #define NV_PAUDIO_INTR_EN_3_PINS_0                              0:0 /* RWIVF */
NV_PAUDIO_INTR_EN_3_PINS_0              TEXTEQU         <0:0>
;  R-I-V 
;  1159 : #define NV_PAUDIO_INTR_EN_3_PINS_0_NOT_PENDING           0x00000000 /* R-I-V */
NV_PAUDIO_INTR_EN_3_PINS_0_NOT_PENDING          EQU             000000000h
;  R---V 
;  1160 : #define NV_PAUDIO_INTR_EN_3_PINS_0_PENDING               0x00000001 /* R---V */
NV_PAUDIO_INTR_EN_3_PINS_0_PENDING              EQU             000000001h
;  -W--V 
;  1161 : #define NV_PAUDIO_INTR_EN_3_PINS_0_RESET                 0x00000001 /* -W--V */
NV_PAUDIO_INTR_EN_3_PINS_0_RESET                EQU             000000001h
;  RWIVF 
;  1162 : #define NV_PAUDIO_INTR_EN_3_PINS_1                              4:4 /* RWIVF */
NV_PAUDIO_INTR_EN_3_PINS_1              TEXTEQU         <4:4>
;  R-I-V 
;  1163 : #define NV_PAUDIO_INTR_EN_3_PINS_1_NOT_PENDING           0x00000000 /* R-I-V */
NV_PAUDIO_INTR_EN_3_PINS_1_NOT_PENDING          EQU             000000000h
;  R---V 
;  1164 : #define NV_PAUDIO_INTR_EN_3_PINS_1_PENDING               0x00000001 /* R---V */
NV_PAUDIO_INTR_EN_3_PINS_1_PENDING              EQU             000000001h
;  -W--V 
;  1165 : #define NV_PAUDIO_INTR_EN_3_PINS_1_RESET                 0x00000001 /* -W--V */
NV_PAUDIO_INTR_EN_3_PINS_1_RESET                EQU             000000001h
;  RWIVF 
;  1166 : #define NV_PAUDIO_INTR_EN_3_PINS_2                              8:8 /* RWIVF */
NV_PAUDIO_INTR_EN_3_PINS_2              TEXTEQU         <8:8>
;  R-I-V 
;  1167 : #define NV_PAUDIO_INTR_EN_3_PINS_2_NOT_PENDING           0x00000000 /* R-I-V */
NV_PAUDIO_INTR_EN_3_PINS_2_NOT_PENDING          EQU             000000000h
;  R---V 
;  1168 : #define NV_PAUDIO_INTR_EN_3_PINS_2_PENDING               0x00000001 /* R---V */
NV_PAUDIO_INTR_EN_3_PINS_2_PENDING              EQU             000000001h
;  -W--V 
;  1169 : #define NV_PAUDIO_INTR_EN_3_PINS_2_RESET                 0x00000001 /* -W--V */
NV_PAUDIO_INTR_EN_3_PINS_2_RESET                EQU             000000001h
;  RWIVF 
;  1170 : #define NV_PAUDIO_INTR_EN_3_PINS_3                            12:12 /* RWIVF */
NV_PAUDIO_INTR_EN_3_PINS_3              TEXTEQU         <12:12>
;  R-I-V 
;  1171 : #define NV_PAUDIO_INTR_EN_3_PINS_3_NOT_PENDING           0x00000000 /* R-I-V */
NV_PAUDIO_INTR_EN_3_PINS_3_NOT_PENDING          EQU             000000000h
;  R---V 
;  1172 : #define NV_PAUDIO_INTR_EN_3_PINS_3_PENDING               0x00000001 /* R---V */
NV_PAUDIO_INTR_EN_3_PINS_3_PENDING              EQU             000000001h
;  -W--V 
;  1173 : #define NV_PAUDIO_INTR_EN_3_PINS_3_RESET                 0x00000001 /* -W--V */
NV_PAUDIO_INTR_EN_3_PINS_3_RESET                EQU             000000001h
;  RW-4R 
;  1174 : #define NV_PAUDIO_BLOCK_NEW                              0x00300200 /* RW-4R */
NV_PAUDIO_BLOCK_NEW             EQU             000300200h
;  RWXUF 
;  1175 : #define NV_PAUDIO_BLOCK_NEW_LENGTH                              7:0 /* RWXUF */
NV_PAUDIO_BLOCK_NEW_LENGTH              TEXTEQU         <7:0>
;  RWXUF 
;  1176 : #define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_LG2                     14:12 /* RWXUF */
NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_LG2               TEXTEQU         <14:12>
;  RW--V 
;  1177 : #define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_1                  0x00000000 /* RW--V */
NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_1         EQU             000000000h
;  RW--V 
;  1178 : #define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_2                  0x00000001 /* RW--V */
NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_2         EQU             000000001h
;  RW--V 
;  1179 : #define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_4                  0x00000002 /* RW--V */
NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_4         EQU             000000002h
;  RW--V 
;  1180 : #define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_8                  0x00000003 /* RW--V */
NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_8         EQU             000000003h
;  RW--V 
;  1181 : #define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_16                 0x00000004 /* RW--V */
NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_16                EQU             000000004h
;  RW--V 
;  1182 : #define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_32                 0x00000005 /* RW--V */
NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_32                EQU             000000005h
;  RW--V 
;  1183 : #define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_64                 0x00000006 /* RW--V */
NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_64                EQU             000000006h
;  RW--V 
;  1184 : #define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_128                0x00000007 /* RW--V */
NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_128               EQU             000000007h
;  RW-4R 
;  1185 : #define NV_PAUDIO_BLOCK_ENGINE                           0x00300204 /* RW-4R */
NV_PAUDIO_BLOCK_ENGINE          EQU             000300204h
;  RWXUF 
;  1186 : #define NV_PAUDIO_BLOCK_ENGINE_LENGTH                           7:0 /* RWXUF */
NV_PAUDIO_BLOCK_ENGINE_LENGTH           TEXTEQU         <7:0>
;  R-XVF 
;  1187 : #define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_LG2                  14:12 /* R-XVF */
NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_LG2            TEXTEQU         <14:12>
;  R---V 
;  1188 : #define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_1               0x00000000 /* R---V */
NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_1              EQU             000000000h
;  R---V 
;  1189 : #define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_2               0x00000001 /* R---V */
NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_2              EQU             000000001h
;  R---V 
;  1190 : #define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_4               0x00000002 /* R---V */
NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_4              EQU             000000002h
;  R---V 
;  1191 : #define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_8               0x00000003 /* R---V */
NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_8              EQU             000000003h
;  R---V 
;  1192 : #define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_16              0x00000004 /* R---V */
NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_16             EQU             000000004h
;  R---V 
;  1193 : #define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_32              0x00000005 /* R---V */
NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_32             EQU             000000005h
;  R---V 
;  1194 : #define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_64              0x00000006 /* R---V */
NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_64             EQU             000000006h
;  R---V 
;  1195 : #define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_128             0x00000007 /* R---V */
NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_128            EQU             000000007h
;  RW-4R 
;  1196 : #define NV_PAUDIO_RAMAU                                  0x00300210 /* RW-4R */
NV_PAUDIO_RAMAU         EQU             000300210h
;  RWXVF 
;  1197 : #define NV_PAUDIO_RAMAU_BASE_ADDRESS                          15:12 /* RWXVF */
NV_PAUDIO_RAMAU_BASE_ADDRESS            TEXTEQU         <15:12>
;  RWI-V 
;  1198 : #define NV_PAUDIO_RAMAU_BASE_ADDRESS_1000                0x00001000 /* RWI-V */
NV_PAUDIO_RAMAU_BASE_ADDRESS_1000               EQU             000001000h
;  RW-4R 
;  1199 : #define NV_PAUDIO_ISA_SEL                                0x00300280 /* RW-4R */
NV_PAUDIO_ISA_SEL               EQU             000300280h
;  RWXVF 
;  1200 : #define NV_PAUDIO_ISA_SEL_IRQ                                   2:0 /* RWXVF */
NV_PAUDIO_ISA_SEL_IRQ           TEXTEQU         <2:0>
;  RWI-V 
;  1201 : #define NV_PAUDIO_ISA_SEL_IRQ_0                          0x00000001 /* RWI-V */
NV_PAUDIO_ISA_SEL_IRQ_0         EQU             000000001h
;  RWI-V 
;  1202 : #define NV_PAUDIO_ISA_SEL_IRQ_1                          0x00000002 /* RWI-V */
NV_PAUDIO_ISA_SEL_IRQ_1         EQU             000000002h
;  RWI-V 
;  1203 : #define NV_PAUDIO_ISA_SEL_IRQ_2                          0x00000004 /* RWI-V */
NV_PAUDIO_ISA_SEL_IRQ_2         EQU             000000004h
;  RWXVF 
;  1204 : #define NV_PAUDIO_ISA_SEL_DRQ                                   9:8 /* RWXVF */
NV_PAUDIO_ISA_SEL_DRQ           TEXTEQU         <9:8>
;  RWI-V 
;  1205 : #define NV_PAUDIO_ISA_SEL_DRQ_0                          0x00000001 /* RWI-V */
NV_PAUDIO_ISA_SEL_DRQ_0         EQU             000000001h
;  RWI-V 
;  1206 : #define NV_PAUDIO_ISA_SEL_DRQ_1                          0x00000002 /* RWI-V */
NV_PAUDIO_ISA_SEL_DRQ_1         EQU             000000002h
;  R-X4R 
;  1207 : #define NV_PAUDIO_TLB_PTE                                0x00300310 /* R-X4R */
NV_PAUDIO_TLB_PTE               EQU             000300310h
;  R-XVF 
;  1208 : #define NV_PAUDIO_TLB_PTE_PAGE                                31:10 /* R-XVF */
NV_PAUDIO_TLB_PTE_PAGE          TEXTEQU         <31:10>
;  R-XVF 
;  1209 : #define NV_PAUDIO_TLB_PTE_READ_ONLY                             1:1 /* R-XVF */
NV_PAUDIO_TLB_PTE_READ_ONLY             TEXTEQU         <1:1>
;  R-XVF 
;  1210 : #define NV_PAUDIO_TLB_PTE_PRESENT                               0:0 /* R-XVF */
NV_PAUDIO_TLB_PTE_PRESENT               TEXTEQU         <0:0>
;  R-X4R 
;  1211 : #define NV_PAUDIO_TLB_TAG                                0x00300320 /* R-X4R */
NV_PAUDIO_TLB_TAG               EQU             000300320h
;  R-XVF 
;  1212 : #define NV_PAUDIO_DMA_TLB_TAG                                 31:12 /* R-XVF */
NV_PAUDIO_DMA_TLB_TAG           TEXTEQU         <31:12>
;  R-X4R 
;  1213 : #define NV_PAUDIO_DMAIA                                  0x00300330 /* R-X4R */
NV_PAUDIO_DMAIA         EQU             000300330h
;  R-XVF 
;  1214 : #define NV_PAUDIO_DMA_INSTANCE                                 31:2 /* R-XVF */
NV_PAUDIO_DMA_INSTANCE          TEXTEQU         <31:2>
;  R-XVF 
;  1215 : #define NV_PAUDIO_DMA_INSTANCE_TARGET                           1:0 /* R-XVF */
NV_PAUDIO_DMA_INSTANCE_TARGET           TEXTEQU         <1:0>
;  RW-4R 
;  1216 : #define NV_PAUDIO_NEAR_MARK                              0x00300400 /* RW-4R */
NV_PAUDIO_NEAR_MARK             EQU             000300400h
;  RWXUF 
;  1217 : #define NV_PAUDIO_NEAR_MARK_COUNT                               7:0 /* RWXUF */
NV_PAUDIO_NEAR_MARK_COUNT               TEXTEQU         <7:0>
;  R--4R 
;  1218 : #define NV_PAUDIO_SAMPLE_COUNT                           0x00300410 /* R--4R */
NV_PAUDIO_SAMPLE_COUNT          EQU             000300410h
;  R-XUF 
;  1219 : #define NV_PAUDIO_SAMPLE_COUNT_VALUE                            7:0 /* R-XUF */
NV_PAUDIO_SAMPLE_COUNT_VALUE            TEXTEQU         <7:0>
;  R-XUF 
;  1220 : #define NV_PAUDIO_SAMPLE_COUNT_BUFFER                           8:8 /* R-XUF */
NV_PAUDIO_SAMPLE_COUNT_BUFFER           TEXTEQU         <8:8>
;  R--4R 
;  1221 : #define NV_PAUDIO_PROG_COUNTER                           0x00300420 /* R--4R */
NV_PAUDIO_PROG_COUNTER          EQU             000300420h
;  R-XUF 
;  1222 : #define NV_PAUDIO_PROG_COUNTER_VALUE                            7:0 /* R-XUF */
NV_PAUDIO_PROG_COUNTER_VALUE            TEXTEQU         <7:0>
;  RWI4R 
;  1223 : #define NV_PAUDIO_AN_VOLUME                              0x00300430 /* RWI4R */
NV_PAUDIO_AN_VOLUME             EQU             000300430h
;  RWI-V 
;  1224 : #define NV_PAUDIO_AN_VOLUME_INITIAL                               7 /* RWI-V */
NV_PAUDIO_AN_VOLUME_INITIAL             EQU             7t
;  RWXUF 
;  1225 : #define NV_PAUDIO_AN_VOLUME_VALUE                               2:0 /* RWXUF */
NV_PAUDIO_AN_VOLUME_VALUE               TEXTEQU         <2:0>
;  RW-4R 
;  1226 : #define NV_PAUDIO_TERM_USAGE                             0x00300C0C /* RW-4R */
NV_PAUDIO_TERM_USAGE            EQU             000300c0ch
;  RWXUF 
;  1227 : #define NV_PAUDIO_TERMINATION_LEVEL                           31:16 /* RWXUF */
NV_PAUDIO_TERMINATION_LEVEL             TEXTEQU         <31:16>
;  RW--V 
;  1228 : #define NV_PAUDIO_TERMINATION_LEVEL_DISABLED             0x00000000 /* RW--V */
NV_PAUDIO_TERMINATION_LEVEL_DISABLED            EQU             000000000h
;  RWXUF 
;  1229 : #define NV_PAUDIO_USAGE_LEVEL                                  15:0 /* RWXUF */
NV_PAUDIO_USAGE_LEVEL           TEXTEQU         <15:0>
;  RW-4R 
;  1230 : #define NV_PAUDIO_CONTEXT                                0x00300E10 /* RW-4R */
NV_PAUDIO_CONTEXT               EQU             000300e10h
;  RWXUF 
;  1231 : #define NV_PAUDIO_CONTEXT_INSTANCE                             31:0 /* RWXUF */
NV_PAUDIO_CONTEXT_INSTANCE              TEXTEQU         <31:0>
;  RW-1A 
;        
;  1232 : #define NV_PAUDIO_BLASTER(i)                       (0x00301000+(i)) /* RW-1A */
;  1233 : #define NV_PAUDIO_BLASTER__SIZE_1                                 1 /*       */
NV_PAUDIO_BLASTER__SIZE_1               EQU             1t
;  RW-VF 
;  1234 : #define NV_PAUDIO_BLASTER_DATA                                  7:0 /* RW-VF */
NV_PAUDIO_BLASTER_DATA          TEXTEQU         <7:0>
;  RW-4A 
;        
;  1235 : #define NV_PAUDIO_CODEC(i)                       (0x00302800+(i)*4) /* RW-4A */
;  1236 : #define NV_PAUDIO_CODEC__SIZE_1                                 256 /*       */
NV_PAUDIO_CODEC__SIZE_1         EQU             256t
;  RW-VF 
;  1237 : #define NV_PAUDIO_CODEC_DATA                                   19:0 /* RW-VF */
NV_PAUDIO_CODEC_DATA            TEXTEQU         <19:0>
;  RW--V 
;  1238 : #define NV_PAUDIO_INST_TARGET_NVM                        0x00000000 /* RW--V */
NV_PAUDIO_INST_TARGET_NVM               EQU             000000000h
;  RW--V 
;  1239 : #define NV_PAUDIO_INST_TARGET_CART                       0x00000001 /* RW--V */
NV_PAUDIO_INST_TARGET_CART              EQU             000000001h
;  RW--V 
;  1240 : #define NV_PAUDIO_INST_TARGET_PCI                        0x00000002 /* RW--V */
NV_PAUDIO_INST_TARGET_PCI               EQU             000000002h
;  RW-4R 
;  1241 : #define NV_PAUDIO_ROOT_INPUT                             0x00300C00 /* RW-4R */
NV_PAUDIO_ROOT_INPUT            EQU             000300c00h
;  RWXUF 
;  1242 : #define NV_PAUDIO_ROOT_INPUT_TARGET                             1:0 /* RWXUF */
NV_PAUDIO_ROOT_INPUT_TARGET             TEXTEQU         <1:0>
;  RWXUF 
;  1243 : #define NV_PAUDIO_ROOT_INPUT_INSTANCE                          31:2 /* RWXUF */
NV_PAUDIO_ROOT_INPUT_INSTANCE           TEXTEQU         <31:2>
;  RW-4R 
;  1244 : #define NV_PAUDIO_ROOT_OUTPUT                            0x00300C04 /* RW-4R */
NV_PAUDIO_ROOT_OUTPUT           EQU             000300c04h
;  RWXUF 
;  1245 : #define NV_PAUDIO_ROOT_OUTPUT_TARGET                            1:0 /* RWXUF */
NV_PAUDIO_ROOT_OUTPUT_TARGET            TEXTEQU         <1:0>
;  RWXUF 
;  1246 : #define NV_PAUDIO_ROOT_OUTPUT_INSTANCE                         31:2 /* RWXUF */
NV_PAUDIO_ROOT_OUTPUT_INSTANCE          TEXTEQU         <31:2>
;  RW-4R 
;  1247 : #define NV_PAUDIO_ROOT_NOTE                              0x00300C08 /* RW-4R */
NV_PAUDIO_ROOT_NOTE             EQU             000300c08h
;  RWXUF 
;  1248 : #define NV_PAUDIO_ROOT_NOTE_TARGET                              1:0 /* RWXUF */
NV_PAUDIO_ROOT_NOTE_TARGET              TEXTEQU         <1:0>
;  RWXUF 
;  1249 : #define NV_PAUDIO_ROOT_NOTE_INSTANCE                           31:2 /* RWXUF */
NV_PAUDIO_ROOT_NOTE_INSTANCE            TEXTEQU         <31:2>
;  RW-4A 
;        
;  1250 : #define NV_PAUDIO_DIAG(i)                        (0x00300C10+(i)*4) /* RW-4A */
;  1251 : #define NV_PAUDIO_DIAG__SIZE_1                                  252 /*       */
NV_PAUDIO_DIAG__SIZE_1          EQU             252t
;  RW-VF 
;  1252 : #define NV_PAUDIO_DIAG_DATA                                    31:0 /* RW-VF */
NV_PAUDIO_DIAG_DATA             TEXTEQU         <31:0>
;  dev_graphics.ref 
;  RW--D 
;  1253 : /* dev_graphics.ref */
;  1254 : #define NV_PGRAPH                             0x00401FFF:0x00400000 /* RW--D */
NV_PGRAPH               TEXTEQU         <0x00401FFF:0x00400000>
;  RW-4R 
;  1255 : #define NV_PGRAPH_DEBUG_0                                0x00400080 /* RW-4R */
NV_PGRAPH_DEBUG_0               EQU             000400080h
;  CW-VF 
;  1256 : #define NV_PGRAPH_DEBUG_0_STATE                                 0:0 /* CW-VF */
NV_PGRAPH_DEBUG_0_STATE         TEXTEQU         <0:0>
;  CW--V 
;  1257 : #define NV_PGRAPH_DEBUG_0_STATE_NORMAL                   0x00000000 /* CW--V */
NV_PGRAPH_DEBUG_0_STATE_NORMAL          EQU             000000000h
;  -W--V 
;  1258 : #define NV_PGRAPH_DEBUG_0_STATE_RESET                    0x00000001 /* -W--V */
NV_PGRAPH_DEBUG_0_STATE_RESET           EQU             000000001h
;  CW-VF 
;  1259 : #define NV_PGRAPH_DEBUG_0_AP_PIPE_STATE                         1:1 /* CW-VF */
NV_PGRAPH_DEBUG_0_AP_PIPE_STATE         TEXTEQU         <1:1>
;  CW--V 
;  1260 : #define NV_PGRAPH_DEBUG_0_AP_PIPE_STATE_NORMAL           0x00000000 /* CW--V */
NV_PGRAPH_DEBUG_0_AP_PIPE_STATE_NORMAL          EQU             000000000h
;  -W--V 
;  1261 : #define NV_PGRAPH_DEBUG_0_AP_PIPE_STATE_RESET            0x00000001 /* -W--V */
NV_PGRAPH_DEBUG_0_AP_PIPE_STATE_RESET           EQU             000000001h
;  CW-VF 
;  1262 : #define NV_PGRAPH_DEBUG_0_3D_PIPE_STATE                         2:2 /* CW-VF */
NV_PGRAPH_DEBUG_0_3D_PIPE_STATE         TEXTEQU         <2:2>
;  CW--V 
;  1263 : #define NV_PGRAPH_DEBUG_0_3D_PIPE_STATE_NORMAL           0x00000000 /* CW--V */
NV_PGRAPH_DEBUG_0_3D_PIPE_STATE_NORMAL          EQU             000000000h
;  -W--V 
;  1264 : #define NV_PGRAPH_DEBUG_0_3D_PIPE_STATE_RESET            0x00000001 /* -W--V */
NV_PGRAPH_DEBUG_0_3D_PIPE_STATE_RESET           EQU             000000001h
;  CW-VF 
;  1265 : #define NV_PGRAPH_DEBUG_0_CACHE_STATE                           3:3 /* CW-VF */
NV_PGRAPH_DEBUG_0_CACHE_STATE           TEXTEQU         <3:3>
;  CW--V 
;  1266 : #define NV_PGRAPH_DEBUG_0_CACHE_STATE_NORMAL             0x00000000 /* CW--V */
NV_PGRAPH_DEBUG_0_CACHE_STATE_NORMAL            EQU             000000000h
;  -W--V 
;  1267 : #define NV_PGRAPH_DEBUG_0_CACHE_STATE_RESET              0x00000001 /* -W--V */
NV_PGRAPH_DEBUG_0_CACHE_STATE_RESET             EQU             000000001h
;  RWIVF 
;  1268 : #define NV_PGRAPH_DEBUG_0_BULK_READS                            4:4 /* RWIVF */
NV_PGRAPH_DEBUG_0_BULK_READS            TEXTEQU         <4:4>
;  RWI-V 
;  1269 : #define NV_PGRAPH_DEBUG_0_BULK_READS_DISABLED            0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_0_BULK_READS_DISABLED           EQU             000000000h
;  RW--V 
;  1270 : #define NV_PGRAPH_DEBUG_0_BULK_READS_ENABLED             0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_0_BULK_READS_ENABLED            EQU             000000001h
;  RWIVF 
;  1271 : #define NV_PGRAPH_DEBUG_0_SPARE1                                8:8 /* RWIVF */
NV_PGRAPH_DEBUG_0_SPARE1                TEXTEQU         <8:8>
;  RWI-V 
;  1272 : #define NV_PGRAPH_DEBUG_0_SPARE1_DISABLED                0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_0_SPARE1_DISABLED               EQU             000000000h
;  RW--V 
;  1273 : #define NV_PGRAPH_DEBUG_0_SPARE1_ENABLED                 0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_0_SPARE1_ENABLED                EQU             000000001h
;  RWIVF 
;  1274 : #define NV_PGRAPH_DEBUG_0_SPARE2                              12:12 /* RWIVF */
NV_PGRAPH_DEBUG_0_SPARE2                TEXTEQU         <12:12>
;  RWI-V 
;  1275 : #define NV_PGRAPH_DEBUG_0_SPARE2_DISABLED                0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_0_SPARE2_DISABLED               EQU             000000000h
;  RW--V 
;  1276 : #define NV_PGRAPH_DEBUG_0_SPARE2_ENABLED                 0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_0_SPARE2_ENABLED                EQU             000000001h
;  RWIVF 
;  1277 : #define NV_PGRAPH_DEBUG_0_SPARE3                              16:16 /* RWIVF */
NV_PGRAPH_DEBUG_0_SPARE3                TEXTEQU         <16:16>
;  RWI-V 
;  1278 : #define NV_PGRAPH_DEBUG_0_SPARE3_DISABLED                0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_0_SPARE3_DISABLED               EQU             000000000h
;  RW--V 
;  1279 : #define NV_PGRAPH_DEBUG_0_SPARE3_ENABLED                 0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_0_SPARE3_ENABLED                EQU             000000001h
;  RWIVF 
;  1280 : #define NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS                     20:20 /* RWIVF */
NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS               TEXTEQU         <20:20>
;  RWI-V 
;  1281 : #define NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS_DISABLED       0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS_DISABLED              EQU             000000000h
;  RW--V 
;  1282 : #define NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS_ENABLED        0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS_ENABLED               EQU             000000001h
;  RWIVF 
;  1283 : #define NV_PGRAPH_DEBUG_0_DRAWDIR_AUTO                        24:24 /* RWIVF */
NV_PGRAPH_DEBUG_0_DRAWDIR_AUTO          TEXTEQU         <24:24>
;  RWI-V 
;  1284 : #define NV_PGRAPH_DEBUG_0_DRAWDIR_AUTO_DISABLED          0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_0_DRAWDIR_AUTO_DISABLED         EQU             000000000h
;  RW--V 
;  1285 : #define NV_PGRAPH_DEBUG_0_DRAWDIR_AUTO_ENABLED           0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_0_DRAWDIR_AUTO_ENABLED          EQU             000000001h
;  RWIVF 
;  1286 : #define NV_PGRAPH_DEBUG_0_DRAWDIR_Y                           25:25 /* RWIVF */
NV_PGRAPH_DEBUG_0_DRAWDIR_Y             TEXTEQU         <25:25>
;  RWI-V 
;  1287 : #define NV_PGRAPH_DEBUG_0_DRAWDIR_Y_DECR                 0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_0_DRAWDIR_Y_DECR                EQU             000000000h
;  RW--V 
;  1288 : #define NV_PGRAPH_DEBUG_0_DRAWDIR_Y_INCR                 0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_0_DRAWDIR_Y_INCR                EQU             000000001h
;  RWIVF 
;  1289 : #define NV_PGRAPH_DEBUG_0_ALPHA_ABORT                         28:28 /* RWIVF */
NV_PGRAPH_DEBUG_0_ALPHA_ABORT           TEXTEQU         <28:28>
;  RWI-V 
;  1290 : #define NV_PGRAPH_DEBUG_0_ALPHA_ABORT_DISABLED           0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_0_ALPHA_ABORT_DISABLED          EQU             000000000h
;  RW--V 
;  1291 : #define NV_PGRAPH_DEBUG_0_ALPHA_ABORT_ENABLED            0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_0_ALPHA_ABORT_ENABLED           EQU             000000001h
;  RW-4R 
;  1292 : #define NV_PGRAPH_DEBUG_1                                0x00400084 /* RW-4R */
NV_PGRAPH_DEBUG_1               EQU             000400084h
;  RWIVF 
;  1293 : #define NV_PGRAPH_DEBUG_1_VOLATILE_RESET                        0:0 /* RWIVF */
NV_PGRAPH_DEBUG_1_VOLATILE_RESET                TEXTEQU         <0:0>
;  RWI-V 
;  1294 : #define NV_PGRAPH_DEBUG_1_VOLATILE_RESET_NOT_LAST        0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_1_VOLATILE_RESET_NOT_LAST               EQU             000000000h
;  RW--V 
;  1295 : #define NV_PGRAPH_DEBUG_1_VOLATILE_RESET_LAST            0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_1_VOLATILE_RESET_LAST           EQU             000000001h
;  CW-VF 
;  1296 : #define NV_PGRAPH_DEBUG_1_DMA_ACTIVITY                          4:4 /* CW-VF */
NV_PGRAPH_DEBUG_1_DMA_ACTIVITY          TEXTEQU         <4:4>
;  CW--V 
;  1297 : #define NV_PGRAPH_DEBUG_1_DMA_ACTIVITY_IGNORE            0x00000000 /* CW--V */
NV_PGRAPH_DEBUG_1_DMA_ACTIVITY_IGNORE           EQU             000000000h
;  -W--V 
;  1298 : #define NV_PGRAPH_DEBUG_1_DMA_ACTIVITY_CANCEL            0x00000001 /* -W--V */
NV_PGRAPH_DEBUG_1_DMA_ACTIVITY_CANCEL           EQU             000000001h
;  RWIVF 
;  1299 : #define NV_PGRAPH_DEBUG_1_TURBO3D_2X                            8:8 /* RWIVF */
NV_PGRAPH_DEBUG_1_TURBO3D_2X            TEXTEQU         <8:8>
;  RWI-V 
;  1300 : #define NV_PGRAPH_DEBUG_1_TURBO3D_2X__DISABLED           0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_1_TURBO3D_2X__DISABLED          EQU             000000000h
;  RW--V 
;  1301 : #define NV_PGRAPH_DEBUG_1_TURBO3D_2X_ENABLED             0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_1_TURBO3D_2X_ENABLED            EQU             000000001h
;  RWIVF 
;  1302 : #define NV_PGRAPH_DEBUG_1_TURBO3D_4X                            9:9 /* RWIVF */
NV_PGRAPH_DEBUG_1_TURBO3D_4X            TEXTEQU         <9:9>
;  RWI-V 
;  1303 : #define NV_PGRAPH_DEBUG_1_TURBO3D_4X__DISABLED           0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_1_TURBO3D_4X__DISABLED          EQU             000000000h
;  RW--V 
;  1304 : #define NV_PGRAPH_DEBUG_1_TURBO3D_4X_ENABLED             0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_1_TURBO3D_4X_ENABLED            EQU             000000001h
;  RWIVF 
;  1305 : #define NV_PGRAPH_DEBUG_1_TRI_OPTS                            12:12 /* RWIVF */
NV_PGRAPH_DEBUG_1_TRI_OPTS              TEXTEQU         <12:12>
;  RWI-V 
;  1306 : #define NV_PGRAPH_DEBUG_1_TRI_OPTS_DISABLED              0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_1_TRI_OPTS_DISABLED             EQU             000000000h
;  RW--V 
;  1307 : #define NV_PGRAPH_DEBUG_1_TRI_OPTS_ENABLED               0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_1_TRI_OPTS_ENABLED              EQU             000000001h
;  RWIVF 
;  1308 : #define NV_PGRAPH_DEBUG_1_INSTANCE                            16:16 /* RWIVF */
NV_PGRAPH_DEBUG_1_INSTANCE              TEXTEQU         <16:16>
;  RWI-V 
;  1309 : #define NV_PGRAPH_DEBUG_1_INSTANCE_DISABLED              0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_1_INSTANCE_DISABLED             EQU             000000000h
;  RW--V 
;  1310 : #define NV_PGRAPH_DEBUG_1_INSTANCE_ENABLED               0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_1_INSTANCE_ENABLED              EQU             000000001h
;  RWIVF 
;  1311 : #define NV_PGRAPH_DEBUG_1_CTX                                 20:20 /* RWIVF */
NV_PGRAPH_DEBUG_1_CTX           TEXTEQU         <20:20>
;  RWI-V 
;  1312 : #define NV_PGRAPH_DEBUG_1_CTX_DISABLED                   0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_1_CTX_DISABLED          EQU             000000000h
;  RW--V 
;  1313 : #define NV_PGRAPH_DEBUG_1_CTX_ENABLED                    0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_1_CTX_ENABLED           EQU             000000001h
;  CW-VF 
;  1314 : #define NV_PGRAPH_DEBUG_1_CACHE                               24:24 /* CW-VF */
NV_PGRAPH_DEBUG_1_CACHE         TEXTEQU         <24:24>
;  CW--V 
;  1315 : #define NV_PGRAPH_DEBUG_1_CACHE_IGNORE                   0x00000000 /* CW--V */
NV_PGRAPH_DEBUG_1_CACHE_IGNORE          EQU             000000000h
;  -W--V 
;  1316 : #define NV_PGRAPH_DEBUG_1_CACHE_FLUSH                    0x00000001 /* -W--V */
NV_PGRAPH_DEBUG_1_CACHE_FLUSH           EQU             000000001h
;  RWIVF 
;  1317 : #define NV_PGRAPH_DEBUG_1_SPARE1                              28:28 /* RWIVF */
NV_PGRAPH_DEBUG_1_SPARE1                TEXTEQU         <28:28>
;  RWI-V 
;  1318 : #define NV_PGRAPH_DEBUG_1_SPARE1_DISABLED                0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_1_SPARE1_DISABLED               EQU             000000000h
;  RW--V 
;  1319 : #define NV_PGRAPH_DEBUG_1_SPARE1_ENABLED                 0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_1_SPARE1_ENABLED                EQU             000000001h
;  RW-4R 
;  1320 : #define NV_PGRAPH_DEBUG_2                                0x00400088 /* RW-4R */
NV_PGRAPH_DEBUG_2               EQU             000400088h
;  RWIVF 
;  1321 : #define NV_PGRAPH_DEBUG_2_AVOID_RMW_BLEND                       0:0 /* RWIVF */
NV_PGRAPH_DEBUG_2_AVOID_RMW_BLEND               TEXTEQU         <0:0>
;  RWI-V 
;  1322 : #define NV_PGRAPH_DEBUG_2_AVOID_RMW_BLEND_DISABLED       0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_2_AVOID_RMW_BLEND_DISABLED              EQU             000000000h
;  RW--V 
;  1323 : #define NV_PGRAPH_DEBUG_2_AVOID_RMW_BLEND_ENABLED        0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_2_AVOID_RMW_BLEND_ENABLED               EQU             000000001h
;  RWIVF 
;  1324 : #define NV_PGRAPH_DEBUG_2_ALPHA_ABORT                           4:4 /* RWIVF */
NV_PGRAPH_DEBUG_2_ALPHA_ABORT           TEXTEQU         <4:4>
;  RWI-V 
;  1325 : #define NV_PGRAPH_DEBUG_2_ALPHA_ABORT_DISABLED           0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_2_ALPHA_ABORT_DISABLED          EQU             000000000h
;  RW--V 
;  1326 : #define NV_PGRAPH_DEBUG_2_ALPHA_ABORT_ENABLED            0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_2_ALPHA_ABORT_ENABLED           EQU             000000001h
;  RWIVF 
;  1327 : #define NV_PGRAPH_DEBUG_2_DPWR_FIFO                             8:8 /* RWIVF */
NV_PGRAPH_DEBUG_2_DPWR_FIFO             TEXTEQU         <8:8>
;  RWI-V 
;  1328 : #define NV_PGRAPH_DEBUG_2_DPWR_FIFO_DISABLED             0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_2_DPWR_FIFO_DISABLED            EQU             000000000h
;  RW--V 
;  1329 : #define NV_PGRAPH_DEBUG_2_DPWR_FIFO_ENABLED              0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_2_DPWR_FIFO_ENABLED             EQU             000000001h
;  RWIVF 
;  1330 : #define NV_PGRAPH_DEBUG_2_BILINEAR_3D                         12:12 /* RWIVF */
NV_PGRAPH_DEBUG_2_BILINEAR_3D           TEXTEQU         <12:12>
;  RWI-V 
;  1331 : #define NV_PGRAPH_DEBUG_2_BILINEAR_3D_DISABLED           0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_2_BILINEAR_3D_DISABLED          EQU             000000000h
;  RW--V 
;  1332 : #define NV_PGRAPH_DEBUG_2_BILINEAR_3D_ENABLED            0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_2_BILINEAR_3D_ENABLED           EQU             000000001h
;  RWIVF 
;  1333 : #define NV_PGRAPH_DEBUG_2_ANISOTROPIC_3D                      13:13 /* RWIVF */
NV_PGRAPH_DEBUG_2_ANISOTROPIC_3D                TEXTEQU         <13:13>
;  RWI-V 
;  1334 : #define NV_PGRAPH_DEBUG_2_ANISOTROPIC_3D_DISABLED        0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_2_ANISOTROPIC_3D_DISABLED               EQU             000000000h
;  RW--V 
;  1335 : #define NV_PGRAPH_DEBUG_2_ANISOTROPIC_3D_ENABLED         0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_2_ANISOTROPIC_3D_ENABLED                EQU             000000001h
;  RWIVF 
;  1336 : #define NV_PGRAPH_DEBUG_2_FOG_3D                              14:14 /* RWIVF */
NV_PGRAPH_DEBUG_2_FOG_3D                TEXTEQU         <14:14>
;  RWI-V 
;  1337 : #define NV_PGRAPH_DEBUG_2_FOG_3D_DISABLED                0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_2_FOG_3D_DISABLED               EQU             000000000h
;  RW--V 
;  1338 : #define NV_PGRAPH_DEBUG_2_FOG_3D_ENABLED                 0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_2_FOG_3D_ENABLED                EQU             000000001h
;  RWIVF 
;  1339 : #define NV_PGRAPH_DEBUG_2_LIGHTING_3D                         15:15 /* RWIVF */
NV_PGRAPH_DEBUG_2_LIGHTING_3D           TEXTEQU         <15:15>
;  RWI-V 
;  1340 : #define NV_PGRAPH_DEBUG_2_LIGHTING_3D_DISABLED           0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_2_LIGHTING_3D_DISABLED          EQU             000000000h
;  RW--V 
;  1341 : #define NV_PGRAPH_DEBUG_2_LIGHTING_3D_ENABLED            0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_2_LIGHTING_3D_ENABLED           EQU             000000001h
;  RWIVF 
;  1342 : #define NV_PGRAPH_DEBUG_2_BILINEAR_2D                         16:16 /* RWIVF */
NV_PGRAPH_DEBUG_2_BILINEAR_2D           TEXTEQU         <16:16>
;  RWI-V 
;  1343 : #define NV_PGRAPH_DEBUG_2_BILINEAR_2D_DISABLED           0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_2_BILINEAR_2D_DISABLED          EQU             000000000h
;  RW--V 
;  1344 : #define NV_PGRAPH_DEBUG_2_BILINEAR_2D_ENABLED            0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_2_BILINEAR_2D_ENABLED           EQU             000000001h
;  RWIVF 
;  1345 : #define NV_PGRAPH_DEBUG_2_ANISOTROPIC_2D                      17:17 /* RWIVF */
NV_PGRAPH_DEBUG_2_ANISOTROPIC_2D                TEXTEQU         <17:17>
;  RWI-V 
;  1346 : #define NV_PGRAPH_DEBUG_2_ANISOTROPIC_2D_DISABLED        0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_2_ANISOTROPIC_2D_DISABLED               EQU             000000000h
;  RW--V 
;  1347 : #define NV_PGRAPH_DEBUG_2_ANISOTROPIC_2D_ENABLED         0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_2_ANISOTROPIC_2D_ENABLED                EQU             000000001h
;  RWIVF 
;  1348 : #define NV_PGRAPH_DEBUG_2_COELESCE                            20:20 /* RWIVF */
NV_PGRAPH_DEBUG_2_COELESCE              TEXTEQU         <20:20>
;  RWI-V 
;  1349 : #define NV_PGRAPH_DEBUG_2_COELESCE_DISABLED              0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_2_COELESCE_DISABLED             EQU             000000000h
;  RW--V 
;  1350 : #define NV_PGRAPH_DEBUG_2_COELESCE_ENABLED               0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_2_COELESCE_ENABLED              EQU             000000001h
;  RWIVF 
;  1351 : #define NV_PGRAPH_DEBUG_2_PREFETCH                            24:24 /* RWIVF */
NV_PGRAPH_DEBUG_2_PREFETCH              TEXTEQU         <24:24>
;  RWI-V 
;  1352 : #define NV_PGRAPH_DEBUG_2_PREFETCH_DISABLED              0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_2_PREFETCH_DISABLED             EQU             000000000h
;  RW--V 
;  1353 : #define NV_PGRAPH_DEBUG_2_PREFETCH_ENABLED               0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_2_PREFETCH_ENABLED              EQU             000000001h
;  RWIVF 
;  1354 : #define NV_PGRAPH_DEBUG_2_VOLATILE_RESET                      28:28 /* RWIVF */
NV_PGRAPH_DEBUG_2_VOLATILE_RESET                TEXTEQU         <28:28>
;  RWI-V 
;  1355 : #define NV_PGRAPH_DEBUG_2_VOLATILE_RESET_DISABLED        0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_2_VOLATILE_RESET_DISABLED               EQU             000000000h
;  RW--V 
;  1356 : #define NV_PGRAPH_DEBUG_2_VOLATILE_RESET_ENABLED         0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_2_VOLATILE_RESET_ENABLED                EQU             000000001h
;  RW-4R 
;  1357 : #define NV_PGRAPH_DEBUG_3                                0x0040008C /* RW-4R */
NV_PGRAPH_DEBUG_3               EQU             00040008ch
;  RWIVF 
;  1358 : #define NV_PGRAPH_DEBUG_3_CULLING                               0:0 /* RWIVF */
NV_PGRAPH_DEBUG_3_CULLING               TEXTEQU         <0:0>
;  RWI-V 
;  1359 : #define NV_PGRAPH_DEBUG_3_CULLING_DISABLED               0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_3_CULLING_DISABLED              EQU             000000000h
;  RW--V 
;  1360 : #define NV_PGRAPH_DEBUG_3_CULLING_ENABLED                0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_3_CULLING_ENABLED               EQU             000000001h
;  RWIVF 
;  1361 : #define NV_PGRAPH_DEBUG_3_FAST_DATA_STRTCH                      4:4 /* RWIVF */
NV_PGRAPH_DEBUG_3_FAST_DATA_STRTCH              TEXTEQU         <4:4>
;  RWI-V 
;  1362 : #define NV_PGRAPH_DEBUG_3_FAST_DATA_STRTCH_DISABLED      0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_3_FAST_DATA_STRTCH_DISABLED             EQU             000000000h
;  RW--V 
;  1363 : #define NV_PGRAPH_DEBUG_3_FAST_DATA_STRTCH_ENABLED       0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_3_FAST_DATA_STRTCH_ENABLED              EQU             000000001h
;  RWIVF 
;  1364 : #define NV_PGRAPH_DEBUG_3_FAST_DATA_D3D                         5:5 /* RWIVF */
NV_PGRAPH_DEBUG_3_FAST_DATA_D3D         TEXTEQU         <5:5>
;  RWI-V 
;  1365 : #define NV_PGRAPH_DEBUG_3_FAST_DATA_D3D_DISABLED         0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_3_FAST_DATA_D3D_DISABLED                EQU             000000000h
;  RW--V 
;  1366 : #define NV_PGRAPH_DEBUG_3_FAST_DATA_D3D_ENABLED          0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_3_FAST_DATA_D3D_ENABLED         EQU             000000001h
;  RWIVF 
;  1367 : #define NV_PGRAPH_DEBUG_3_AUTOZFLUSH                            8:8 /* RWIVF */
NV_PGRAPH_DEBUG_3_AUTOZFLUSH            TEXTEQU         <8:8>
;  RWI-V 
;  1368 : #define NV_PGRAPH_DEBUG_3_AUTOZFLUSH_DISABLED            0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_3_AUTOZFLUSH_DISABLED           EQU             000000000h
;  RW--V 
;  1369 : #define NV_PGRAPH_DEBUG_3_AUTOZFLUSH_ENABLED             0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_3_AUTOZFLUSH_ENABLED            EQU             000000001h
;  CW-VF 
;  1370 : #define NV_PGRAPH_DEBUG_3_ZFLUSH                                9:9 /* CW-VF */
NV_PGRAPH_DEBUG_3_ZFLUSH                TEXTEQU         <9:9>
;  CW--V 
;  1371 : #define NV_PGRAPH_DEBUG_3_ZFLUSH_IGNORE                  0x00000000 /* CW--V */
NV_PGRAPH_DEBUG_3_ZFLUSH_IGNORE         EQU             000000000h
;  -W--V 
;  1372 : #define NV_PGRAPH_DEBUG_3_ZFLUSH_ACTIVATE                0x00000001 /* -W--V */
NV_PGRAPH_DEBUG_3_ZFLUSH_ACTIVATE               EQU             000000001h
;  RWIVF 
;  1373 : #define NV_PGRAPH_DEBUG_3_EARLYZ_ABORT                        12:12 /* RWIVF */
NV_PGRAPH_DEBUG_3_EARLYZ_ABORT          TEXTEQU         <12:12>
;  RWI-V 
;  1374 : #define NV_PGRAPH_DEBUG_3_EARLYZ_ABORT_DISABLED          0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_3_EARLYZ_ABORT_DISABLED         EQU             000000000h
;  RW--V 
;  1375 : #define NV_PGRAPH_DEBUG_3_EARLYZ_ABORT_ENABLED           0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_3_EARLYZ_ABORT_ENABLED          EQU             000000001h
;  RWIVF 
;  1376 : #define NV_PGRAPH_DEBUG_3_LINEARZ                             16:16 /* RWIVF */
NV_PGRAPH_DEBUG_3_LINEARZ               TEXTEQU         <16:16>
;  RWI-V 
;  1377 : #define NV_PGRAPH_DEBUG_3_LINEARZ_DISABLED               0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_3_LINEARZ_DISABLED              EQU             000000000h
;  RW--V 
;  1378 : #define NV_PGRAPH_DEBUG_3_LINEARZ_ENABLED                0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_3_LINEARZ_ENABLED               EQU             000000001h
;  RWIVF 
;  1379 : #define NV_PGRAPH_DEBUG_3_DATA_CHECK                          20:20 /* RWIVF */
NV_PGRAPH_DEBUG_3_DATA_CHECK            TEXTEQU         <20:20>
;  RWI-V 
;  1380 : #define NV_PGRAPH_DEBUG_3_DATA_CHECK_DISABLED            0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_3_DATA_CHECK_DISABLED           EQU             000000000h
;  RW--V 
;  1381 : #define NV_PGRAPH_DEBUG_3_DATA_CHECK_ENABLED             0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_3_DATA_CHECK_ENABLED            EQU             000000001h
;  RWIVF 
;  1382 : #define NV_PGRAPH_DEBUG_3_DATA_CHECK_FAIL                     21:21 /* RWIVF */
NV_PGRAPH_DEBUG_3_DATA_CHECK_FAIL               TEXTEQU         <21:21>
;  RWI-V 
;  1383 : #define NV_PGRAPH_DEBUG_3_DATA_CHECK_FAIL_DISABLED       0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_3_DATA_CHECK_FAIL_DISABLED              EQU             000000000h
;  RW--V 
;  1384 : #define NV_PGRAPH_DEBUG_3_DATA_CHECK_FAIL_ENABLED        0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_3_DATA_CHECK_FAIL_ENABLED               EQU             000000001h
;  RWIVF 
;  1385 : #define NV_PGRAPH_DEBUG_3_SPARE1                              24:24 /* RWIVF */
NV_PGRAPH_DEBUG_3_SPARE1                TEXTEQU         <24:24>
;  RWI-V 
;  1386 : #define NV_PGRAPH_DEBUG_3_SPARE1_DISABLED                0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_3_SPARE1_DISABLED               EQU             000000000h
;  RW--V 
;  1387 : #define NV_PGRAPH_DEBUG_3_SPARE1_ENABLED                 0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_3_SPARE1_ENABLED                EQU             000000001h
;  RWIVF 
;  1388 : #define NV_PGRAPH_DEBUG_3_SPARE2                              28:28 /* RWIVF */
NV_PGRAPH_DEBUG_3_SPARE2                TEXTEQU         <28:28>
;  RWI-V 
;  1389 : #define NV_PGRAPH_DEBUG_3_SPARE2_DISABLED                0x00000000 /* RWI-V */
NV_PGRAPH_DEBUG_3_SPARE2_DISABLED               EQU             000000000h
;  RW--V 
;  1390 : #define NV_PGRAPH_DEBUG_3_SPARE2_ENABLED                 0x00000001 /* RW--V */
NV_PGRAPH_DEBUG_3_SPARE2_ENABLED                EQU             000000001h
;  RW-4R 
;  1391 : #define NV_PGRAPH_INTR_0                                 0x00400100 /* RW-4R */
NV_PGRAPH_INTR_0                EQU             000400100h
;  RW-VF 
;  1392 : #define NV_PGRAPH_INTR_0_RESERVED                               0:0 /* RW-VF */
NV_PGRAPH_INTR_0_RESERVED               TEXTEQU         <0:0>
;  R---V 
;  1393 : #define NV_PGRAPH_INTR_0_RESERVED_NOT_PENDING            0x00000000 /* R---V */
NV_PGRAPH_INTR_0_RESERVED_NOT_PENDING           EQU             000000000h
;  R---V 
;  1394 : #define NV_PGRAPH_INTR_0_RESERVED_PENDING                0x00000001 /* R---V */
NV_PGRAPH_INTR_0_RESERVED_PENDING               EQU             000000001h
;  -W--V 
;  1395 : #define NV_PGRAPH_INTR_0_RESERVED_RESET                  0x00000001 /* -W--V */
NV_PGRAPH_INTR_0_RESERVED_RESET         EQU             000000001h
;  RWIVF 
;  1396 : #define NV_PGRAPH_INTR_0_CONTEXT_SWITCH                         4:4 /* RWIVF */
NV_PGRAPH_INTR_0_CONTEXT_SWITCH         TEXTEQU         <4:4>
;  R-I-V 
;  1397 : #define NV_PGRAPH_INTR_0_CONTEXT_SWITCH_NOT_PENDING      0x00000000 /* R-I-V */
NV_PGRAPH_INTR_0_CONTEXT_SWITCH_NOT_PENDING             EQU             000000000h
;  R---V 
;  1398 : #define NV_PGRAPH_INTR_0_CONTEXT_SWITCH_PENDING          0x00000001 /* R---V */
NV_PGRAPH_INTR_0_CONTEXT_SWITCH_PENDING         EQU             000000001h
;  -W--V 
;  1399 : #define NV_PGRAPH_INTR_0_CONTEXT_SWITCH_RESET            0x00000001 /* -W--V */
NV_PGRAPH_INTR_0_CONTEXT_SWITCH_RESET           EQU             000000001h
;  RWIVF 
;  1400 : #define NV_PGRAPH_INTR_0_VBLANK                                 8:8 /* RWIVF */
NV_PGRAPH_INTR_0_VBLANK         TEXTEQU         <8:8>
;  R-I-V 
;  1401 : #define NV_PGRAPH_INTR_0_VBLANK_NOT_PENDING              0x00000000 /* R-I-V */
NV_PGRAPH_INTR_0_VBLANK_NOT_PENDING             EQU             000000000h
;  R---V 
;  1402 : #define NV_PGRAPH_INTR_0_VBLANK_PENDING                  0x00000001 /* R---V */
NV_PGRAPH_INTR_0_VBLANK_PENDING         EQU             000000001h
;  -W--V 
;  1403 : #define NV_PGRAPH_INTR_0_VBLANK_RESET                    0x00000001 /* -W--V */
NV_PGRAPH_INTR_0_VBLANK_RESET           EQU             000000001h
;  RWIVF 
;  1404 : #define NV_PGRAPH_INTR_0_RANGE                                12:12 /* RWIVF */
NV_PGRAPH_INTR_0_RANGE          TEXTEQU         <12:12>
;  R-I-V 
;  1405 : #define NV_PGRAPH_INTR_0_RANGE_NOT_PENDING               0x00000000 /* R-I-V */
NV_PGRAPH_INTR_0_RANGE_NOT_PENDING              EQU             000000000h
;  R---V 
;  1406 : #define NV_PGRAPH_INTR_0_RANGE_PENDING                   0x00000001 /* R---V */
NV_PGRAPH_INTR_0_RANGE_PENDING          EQU             000000001h
;  -W--V 
;  1407 : #define NV_PGRAPH_INTR_0_RANGE_RESET                     0x00000001 /* -W--V */
NV_PGRAPH_INTR_0_RANGE_RESET            EQU             000000001h
;  RWIVF 
;  1408 : #define NV_PGRAPH_INTR_0_METHOD_COUNT                         16:16 /* RWIVF */
NV_PGRAPH_INTR_0_METHOD_COUNT           TEXTEQU         <16:16>
;  R-I-V 
;  1409 : #define NV_PGRAPH_INTR_0_METHOD_COUNT_NOT_PENDING        0x00000000 /* R-I-V */
NV_PGRAPH_INTR_0_METHOD_COUNT_NOT_PENDING               EQU             000000000h
;  R---V 
;  1410 : #define NV_PGRAPH_INTR_0_METHOD_COUNT_PENDING            0x00000001 /* R---V */
NV_PGRAPH_INTR_0_METHOD_COUNT_PENDING           EQU             000000001h
;  -W--V 
;  1411 : #define NV_PGRAPH_INTR_0_METHOD_COUNT_RESET              0x00000001 /* -W--V */
NV_PGRAPH_INTR_0_METHOD_COUNT_RESET             EQU             000000001h
;  RWIVF 
;  1412 : #define NV_PGRAPH_INTR_0_FORMAT_                              20:20 /* RWIVF */
NV_PGRAPH_INTR_0_FORMAT_                TEXTEQU         <20:20>
;  R-I-V 
;  1413 : #define NV_PGRAPH_INTR_0_FORMAT_NOT_PENDING              0x00000000 /* R-I-V */
NV_PGRAPH_INTR_0_FORMAT_NOT_PENDING             EQU             000000000h
;  R---V 
;  1414 : #define NV_PGRAPH_INTR_0_FORMAT_PENDING                  0x00000001 /* R---V */
NV_PGRAPH_INTR_0_FORMAT_PENDING         EQU             000000001h
;  -W--V 
;  1415 : #define NV_PGRAPH_INTR_0_FORMAT_RESET                    0x00000001 /* -W--V */
NV_PGRAPH_INTR_0_FORMAT_RESET           EQU             000000001h
;  RWIVF 
;  1416 : #define NV_PGRAPH_INTR_0_COMPLEX_CLIP                         24:24 /* RWIVF */
NV_PGRAPH_INTR_0_COMPLEX_CLIP           TEXTEQU         <24:24>
;  R-I-V 
;  1417 : #define NV_PGRAPH_INTR_0_COMPLEX_CLIP_NOT_PENDING        0x00000000 /* R-I-V */
NV_PGRAPH_INTR_0_COMPLEX_CLIP_NOT_PENDING               EQU             000000000h
;  R---V 
;  1418 : #define NV_PGRAPH_INTR_0_COMPLEX_CLIP_PENDING            0x00000001 /* R---V */
NV_PGRAPH_INTR_0_COMPLEX_CLIP_PENDING           EQU             000000001h
;  -W--V 
;  1419 : #define NV_PGRAPH_INTR_0_COMPLEX_CLIP_RESET              0x00000001 /* -W--V */
NV_PGRAPH_INTR_0_COMPLEX_CLIP_RESET             EQU             000000001h
;  RWIVF 
;  1420 : #define NV_PGRAPH_INTR_0_NOTIFY                               28:28 /* RWIVF */
NV_PGRAPH_INTR_0_NOTIFY         TEXTEQU         <28:28>
;  R-I-V 
;  1421 : #define NV_PGRAPH_INTR_0_NOTIFY_NOT_PENDING              0x00000000 /* R-I-V */
NV_PGRAPH_INTR_0_NOTIFY_NOT_PENDING             EQU             000000000h
;  R---V 
;  1422 : #define NV_PGRAPH_INTR_0_NOTIFY_PENDING                  0x00000001 /* R---V */
NV_PGRAPH_INTR_0_NOTIFY_PENDING         EQU             000000001h
;  -W--V 
;  1423 : #define NV_PGRAPH_INTR_0_NOTIFY_RESET                    0x00000001 /* -W--V */
NV_PGRAPH_INTR_0_NOTIFY_RESET           EQU             000000001h
;  RW-4R 
;  1424 : #define NV_PGRAPH_INTR_1                                 0x00400104 /* RW-4R */
NV_PGRAPH_INTR_1                EQU             000400104h
;  RWIVF 
;  1425 : #define NV_PGRAPH_INTR_1_METHOD                                 0:0 /* RWIVF */
NV_PGRAPH_INTR_1_METHOD         TEXTEQU         <0:0>
;  R-I-V 
;  1426 : #define NV_PGRAPH_INTR_1_METHOD_NOT_PENDING              0x00000000 /* R-I-V */
NV_PGRAPH_INTR_1_METHOD_NOT_PENDING             EQU             000000000h
;  R---V 
;  1427 : #define NV_PGRAPH_INTR_1_METHOD_PENDING                  0x00000001 /* R---V */
NV_PGRAPH_INTR_1_METHOD_PENDING         EQU             000000001h
;  -W--V 
;  1428 : #define NV_PGRAPH_INTR_1_METHOD_RESET                    0x00000001 /* -W--V */
NV_PGRAPH_INTR_1_METHOD_RESET           EQU             000000001h
;  RWIVF 
;  1429 : #define NV_PGRAPH_INTR_1_DATA                                   4:4 /* RWIVF */
NV_PGRAPH_INTR_1_DATA           TEXTEQU         <4:4>
;  R-I-V 
;  1430 : #define NV_PGRAPH_INTR_1_DATA_NOT_PENDING                0x00000000 /* R-I-V */
NV_PGRAPH_INTR_1_DATA_NOT_PENDING               EQU             000000000h
;  R---V 
;  1431 : #define NV_PGRAPH_INTR_1_DATA_PENDING                    0x00000001 /* R---V */
NV_PGRAPH_INTR_1_DATA_PENDING           EQU             000000001h
;  -W--V 
;  1432 : #define NV_PGRAPH_INTR_1_DATA_RESET                      0x00000001 /* -W--V */
NV_PGRAPH_INTR_1_DATA_RESET             EQU             000000001h
;  RWIVF 
;  1433 : #define NV_PGRAPH_INTR_1_DOUBLE_NOTIFY                        12:12 /* RWIVF */
NV_PGRAPH_INTR_1_DOUBLE_NOTIFY          TEXTEQU         <12:12>
;  R-I-V 
;  1434 : #define NV_PGRAPH_INTR_1_DOUBLE_NOTIFY_NOT_PENDING       0x00000000 /* R-I-V */
NV_PGRAPH_INTR_1_DOUBLE_NOTIFY_NOT_PENDING              EQU             000000000h
;  R---V 
;  1435 : #define NV_PGRAPH_INTR_1_DOUBLE_NOTIFY_PENDING           0x00000001 /* R---V */
NV_PGRAPH_INTR_1_DOUBLE_NOTIFY_PENDING          EQU             000000001h
;  -W--V 
;  1436 : #define NV_PGRAPH_INTR_1_DOUBLE_NOTIFY_RESET             0x00000001 /* -W--V */
NV_PGRAPH_INTR_1_DOUBLE_NOTIFY_RESET            EQU             000000001h
;  RWIVF 
;  1437 : #define NV_PGRAPH_INTR_1_CTXSW_NOTIFY                         16:16 /* RWIVF */
NV_PGRAPH_INTR_1_CTXSW_NOTIFY           TEXTEQU         <16:16>
;  R-I-V 
;  1438 : #define NV_PGRAPH_INTR_1_CTXSW_NOTIFY_NOT_PENDING        0x00000000 /* R-I-V */
NV_PGRAPH_INTR_1_CTXSW_NOTIFY_NOT_PENDING               EQU             000000000h
;  R---V 
;  1439 : #define NV_PGRAPH_INTR_1_CTXSW_NOTIFY_PENDING            0x00000001 /* R---V */
NV_PGRAPH_INTR_1_CTXSW_NOTIFY_PENDING           EQU             000000001h
;  -W--V 
;  1440 : #define NV_PGRAPH_INTR_1_CTXSW_NOTIFY_RESET              0x00000001 /* -W--V */
NV_PGRAPH_INTR_1_CTXSW_NOTIFY_RESET             EQU             000000001h
;  RW-4R 
;  1441 : #define NV_PGRAPH_INTR_EN_0                              0x00400140 /* RW-4R */
NV_PGRAPH_INTR_EN_0             EQU             000400140h
;  RWIVF 
;  1442 : #define NV_PGRAPH_INTR_EN_0_RESERVED                            0:0 /* RWIVF */
NV_PGRAPH_INTR_EN_0_RESERVED            TEXTEQU         <0:0>
;  RWI-V 
;  1443 : #define NV_PGRAPH_INTR_EN_0_RESERVED_DISABLED            0x00000000 /* RWI-V */
NV_PGRAPH_INTR_EN_0_RESERVED_DISABLED           EQU             000000000h
;  RW--V 
;  1444 : #define NV_PGRAPH_INTR_EN_0_RESERVED_ENABLED             0x00000001 /* RW--V */
NV_PGRAPH_INTR_EN_0_RESERVED_ENABLED            EQU             000000001h
;  RWIVF 
;  1445 : #define NV_PGRAPH_INTR_EN_0_CONTEXT_SWITCH                      4:4 /* RWIVF */
NV_PGRAPH_INTR_EN_0_CONTEXT_SWITCH              TEXTEQU         <4:4>
;  RWI-V 
;  1446 : #define NV_PGRAPH_INTR_EN_0_CONTEXT_SWITCH_DISABLED      0x00000000 /* RWI-V */
NV_PGRAPH_INTR_EN_0_CONTEXT_SWITCH_DISABLED             EQU             000000000h
;  RW--V 
;  1447 : #define NV_PGRAPH_INTR_EN_0_CONTEXT_SWITCH_ENABLED       0x00000001 /* RW--V */
NV_PGRAPH_INTR_EN_0_CONTEXT_SWITCH_ENABLED              EQU             000000001h
;  RWIVF 
;  1448 : #define NV_PGRAPH_INTR_EN_0_VBLANK                              8:8 /* RWIVF */
NV_PGRAPH_INTR_EN_0_VBLANK              TEXTEQU         <8:8>
;  RWI-V 
;  1449 : #define NV_PGRAPH_INTR_EN_0_VBLANK_DISABLED              0x00000000 /* RWI-V */
NV_PGRAPH_INTR_EN_0_VBLANK_DISABLED             EQU             000000000h
;  RW--V 
;  1450 : #define NV_PGRAPH_INTR_EN_0_VBLANK_ENABLED               0x00000001 /* RW--V */
NV_PGRAPH_INTR_EN_0_VBLANK_ENABLED              EQU             000000001h
;  RWIVF 
;  1451 : #define NV_PGRAPH_INTR_EN_0_RANGE                             12:12 /* RWIVF */
NV_PGRAPH_INTR_EN_0_RANGE               TEXTEQU         <12:12>
;  RWI-V 
;  1452 : #define NV_PGRAPH_INTR_EN_0_RANGE_DISABLED               0x00000000 /* RWI-V */
NV_PGRAPH_INTR_EN_0_RANGE_DISABLED              EQU             000000000h
;  RW--V 
;  1453 : #define NV_PGRAPH_INTR_EN_0_RANGE_ENABLED                0x00000001 /* RW--V */
NV_PGRAPH_INTR_EN_0_RANGE_ENABLED               EQU             000000001h
;  RWIVF 
;  1454 : #define NV_PGRAPH_INTR_EN_0_METHOD_COUNT                      16:16 /* RWIVF */
NV_PGRAPH_INTR_EN_0_METHOD_COUNT                TEXTEQU         <16:16>
;  RWI-V 
;  1455 : #define NV_PGRAPH_INTR_EN_0_METHOD_COUNT_DISABLED        0x00000000 /* RWI-V */
NV_PGRAPH_INTR_EN_0_METHOD_COUNT_DISABLED               EQU             000000000h
;  RW--V 
;  1456 : #define NV_PGRAPH_INTR_EN_0_METHOD_COUNT_ENABLED         0x00000001 /* RW--V */
NV_PGRAPH_INTR_EN_0_METHOD_COUNT_ENABLED                EQU             000000001h
;  RWIVF 
;  1457 : #define NV_PGRAPH_INTR_EN_0_FORMAT                            20:20 /* RWIVF */
NV_PGRAPH_INTR_EN_0_FORMAT              TEXTEQU         <20:20>
;  RWI-V 
;  1458 : #define NV_PGRAPH_INTR_EN_0_FORMAT_DISABLED              0x00000000 /* RWI-V */
NV_PGRAPH_INTR_EN_0_FORMAT_DISABLED             EQU             000000000h
;  RW--V 
;  1459 : #define NV_PGRAPH_INTR_EN_0_FORMAT_ENABLED               0x00000001 /* RW--V */
NV_PGRAPH_INTR_EN_0_FORMAT_ENABLED              EQU             000000001h
;  RWIVF 
;  1460 : #define NV_PGRAPH_INTR_EN_0_COMPLEX_CLIP                      24:24 /* RWIVF */
NV_PGRAPH_INTR_EN_0_COMPLEX_CLIP                TEXTEQU         <24:24>
;  RWI-V 
;  1461 : #define NV_PGRAPH_INTR_EN_0_COMPLEX_CLIP_DISABLED        0x00000000 /* RWI-V */
NV_PGRAPH_INTR_EN_0_COMPLEX_CLIP_DISABLED               EQU             000000000h
;  RW--V 
;  1462 : #define NV_PGRAPH_INTR_EN_0_COMPLEX_CLIP_ENABLED         0x00000001 /* RW--V */
NV_PGRAPH_INTR_EN_0_COMPLEX_CLIP_ENABLED                EQU             000000001h
;  RWIVF 
;  1463 : #define NV_PGRAPH_INTR_EN_0_NOTIFY                            28:28 /* RWIVF */
NV_PGRAPH_INTR_EN_0_NOTIFY              TEXTEQU         <28:28>
;  RWI-V 
;  1464 : #define NV_PGRAPH_INTR_EN_0_NOTIFY_DISABLED              0x00000000 /* RWI-V */
NV_PGRAPH_INTR_EN_0_NOTIFY_DISABLED             EQU             000000000h
;  RW--V 
;  1465 : #define NV_PGRAPH_INTR_EN_0_NOTIFY_ENABLED               0x00000001 /* RW--V */
NV_PGRAPH_INTR_EN_0_NOTIFY_ENABLED              EQU             000000001h
;  RW-4R 
;  1466 : #define NV_PGRAPH_INTR_EN_1                              0x00400144 /* RW-4R */
NV_PGRAPH_INTR_EN_1             EQU             000400144h
;  RWIVF 
;  1467 : #define NV_PGRAPH_INTR_EN_1_METHOD                              0:0 /* RWIVF */
NV_PGRAPH_INTR_EN_1_METHOD              TEXTEQU         <0:0>
;  RWI-V 
;  1468 : #define NV_PGRAPH_INTR_EN_1_METHOD_DISABLED              0x00000000 /* RWI-V */
NV_PGRAPH_INTR_EN_1_METHOD_DISABLED             EQU             000000000h
;  RW--V 
;  1469 : #define NV_PGRAPH_INTR_EN_1_METHOD_ENABLED               0x00000001 /* RW--V */
NV_PGRAPH_INTR_EN_1_METHOD_ENABLED              EQU             000000001h
;  RWIVF 
;  1470 : #define NV_PGRAPH_INTR_EN_1_DATA                                4:4 /* RWIVF */
NV_PGRAPH_INTR_EN_1_DATA                TEXTEQU         <4:4>
;  RWI-V 
;  1471 : #define NV_PGRAPH_INTR_EN_1_DATA_DISABLED                0x00000000 /* RWI-V */
NV_PGRAPH_INTR_EN_1_DATA_DISABLED               EQU             000000000h
;  RW--V 
;  1472 : #define NV_PGRAPH_INTR_EN_1_DATA_ENABLED                 0x00000001 /* RW--V */
NV_PGRAPH_INTR_EN_1_DATA_ENABLED                EQU             000000001h
;  RWIVF 
;  1473 : #define NV_PGRAPH_INTR_EN_1_DOUBLE_NOTIFY                     12:12 /* RWIVF */
NV_PGRAPH_INTR_EN_1_DOUBLE_NOTIFY               TEXTEQU         <12:12>
;  RWI-V 
;  1474 : #define NV_PGRAPH_INTR_EN_1_DOUBLE_NOTIFY_DISABLED       0x00000000 /* RWI-V */
NV_PGRAPH_INTR_EN_1_DOUBLE_NOTIFY_DISABLED              EQU             000000000h
;  RW--V 
;  1475 : #define NV_PGRAPH_INTR_EN_1_DOUBLE_NOTIFY_ENABLED        0x00000001 /* RW--V */
NV_PGRAPH_INTR_EN_1_DOUBLE_NOTIFY_ENABLED               EQU             000000001h
;  RWIVF 
;  1476 : #define NV_PGRAPH_INTR_EN_1_CTXSW_NOTIFY                      16:16 /* RWIVF */
NV_PGRAPH_INTR_EN_1_CTXSW_NOTIFY                TEXTEQU         <16:16>
;  RWI-V 
;  1477 : #define NV_PGRAPH_INTR_EN_1_CTXSW_NOTIFY_DISABLED        0x00000000 /* RWI-V */
NV_PGRAPH_INTR_EN_1_CTXSW_NOTIFY_DISABLED               EQU             000000000h
;  RW--V 
;  1478 : #define NV_PGRAPH_INTR_EN_1_CTXSW_NOTIFY_ENABLED         0x00000001 /* RW--V */
NV_PGRAPH_INTR_EN_1_CTXSW_NOTIFY_ENABLED                EQU             000000001h
;  RW-4R 
;  1479 : #define NV_PGRAPH_CTX_SWITCH                             0x00400180 /* RW-4R */
NV_PGRAPH_CTX_SWITCH            EQU             000400180h
;  RWIVF 
;  1480 : #define NV_PGRAPH_CTX_SWITCH_COLOR                              2:0 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_COLOR              TEXTEQU         <2:0>
;  RWI-V 
;  1481 : #define NV_PGRAPH_CTX_SWITCH_COLOR_R5G5B5                0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_COLOR_R5G5B5               EQU             000000000h
;  RW--V 
;  1482 : #define NV_PGRAPH_CTX_SWITCH_COLOR_R8G8B8                0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_COLOR_R8G8B8               EQU             000000001h
;  RW--V 
;  1483 : #define NV_PGRAPH_CTX_SWITCH_COLOR_R10G10B10             0x00000002 /* RW--V */
NV_PGRAPH_CTX_SWITCH_COLOR_R10G10B10            EQU             000000002h
;  RW--V 
;  1484 : #define NV_PGRAPH_CTX_SWITCH_COLOR_Y8                    0x00000003 /* RW--V */
NV_PGRAPH_CTX_SWITCH_COLOR_Y8           EQU             000000003h
;  RW--V 
;  1485 : #define NV_PGRAPH_CTX_SWITCH_COLOR_Y16                   0x00000004 /* RW--V */
NV_PGRAPH_CTX_SWITCH_COLOR_Y16          EQU             000000004h
;  RW--V 
;  1486 : #define NV_PGRAPH_CTX_SWITCH_COLOR_V8Y18U8Y08            0x00000005 /* RW--V */
NV_PGRAPH_CTX_SWITCH_COLOR_V8Y18U8Y08           EQU             000000005h
;  RW--V 
;  1487 : #define NV_PGRAPH_CTX_SWITCH_COLOR_Y18V8Y08U8            0x00000006 /* RW--V */
NV_PGRAPH_CTX_SWITCH_COLOR_Y18V8Y08U8           EQU             000000006h
;  RW--V 
;  1488 : #define NV_PGRAPH_CTX_SWITCH_COLOR_Y420                  0x00000007 /* RW--V */
NV_PGRAPH_CTX_SWITCH_COLOR_Y420         EQU             000000007h
;  RWIVF 
;  1489 : #define NV_PGRAPH_CTX_SWITCH_ALPHA                              3:3 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_ALPHA              TEXTEQU         <3:3>
;  RWI-V 
;  1490 : #define NV_PGRAPH_CTX_SWITCH_ALPHA_DISABLED              0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_ALPHA_DISABLED             EQU             000000000h
;  RW--V 
;  1491 : #define NV_PGRAPH_CTX_SWITCH_ALPHA_ENABLED               0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_ALPHA_ENABLED              EQU             000000001h
;  RWIVF 
;  1492 : #define NV_PGRAPH_CTX_SWITCH_SPARE1                             4:4 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_SPARE1             TEXTEQU         <4:4>
;  RWI-V 
;  1493 : #define NV_PGRAPH_CTX_SWITCH_SPARE1_DISABLED             0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_SPARE1_DISABLED            EQU             000000000h
;  RW--V 
;  1494 : #define NV_PGRAPH_CTX_SWITCH_SPARE1_ENABLED              0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_SPARE1_ENABLED             EQU             000000001h
;  RWIVF 
;  1495 : #define NV_PGRAPH_CTX_SWITCH_MONO_FORMAT                        8:8 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_MONO_FORMAT                TEXTEQU         <8:8>
;  RWI-V 
;  1496 : #define NV_PGRAPH_CTX_SWITCH_MONO_FORMAT_DISABLED        0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_MONO_FORMAT_DISABLED               EQU             000000000h
;  RW--V 
;  1497 : #define NV_PGRAPH_CTX_SWITCH_MONO_FORMAT_ENABLED         0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_MONO_FORMAT_ENABLED                EQU             000000001h
;  RWXVF 
;  1498 : #define NV_PGRAPH_CTX_SWITCH_DAC_BYPASS                         9:9 /* RWXVF */
NV_PGRAPH_CTX_SWITCH_DAC_BYPASS         TEXTEQU         <9:9>
;  RWI-V 
;  1499 : #define NV_PGRAPH_CTX_SWITCH_DAC_BYPASS_DISABLED         0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_DAC_BYPASS_DISABLED                EQU             000000000h
;  RW--V 
;  1500 : #define NV_PGRAPH_CTX_SWITCH_DAC_BYPASS_ENABLED          0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_DAC_BYPASS_ENABLED         EQU             000000001h
;  RWIVF 
;  1501 : #define NV_PGRAPH_CTX_SWITCH_SPARE2                           10:10 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_SPARE2             TEXTEQU         <10:10>
;  RWI-V 
;  1502 : #define NV_PGRAPH_CTX_SWITCH_SPARE2_DISABLED             0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_SPARE2_DISABLED            EQU             000000000h
;  RW--V 
;  1503 : #define NV_PGRAPH_CTX_SWITCH_SPARE2_ENABLED              0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_SPARE2_ENABLED             EQU             000000001h
;  RWIVF 
;  1504 : #define NV_PGRAPH_CTX_SWITCH_ZWRITE                           12:12 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_ZWRITE             TEXTEQU         <12:12>
;  RWI-V 
;  1505 : #define NV_PGRAPH_CTX_SWITCH_ZWRITE_DISABLED             0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_ZWRITE_DISABLED            EQU             000000000h
;  RW--V 
;  1506 : #define NV_PGRAPH_CTX_SWITCH_ZWRITE_ENABLED              0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_ZWRITE_ENABLED             EQU             000000001h
;  RWIVF 
;  1507 : #define NV_PGRAPH_CTX_SWITCH_CHROMA_KEY                       13:13 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_CHROMA_KEY         TEXTEQU         <13:13>
;  RWI-V 
;  1508 : #define NV_PGRAPH_CTX_SWITCH_CHROMA_KEY_DISABLED         0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_CHROMA_KEY_DISABLED                EQU             000000000h
;  RW--V 
;  1509 : #define NV_PGRAPH_CTX_SWITCH_CHROMA_KEY_ENABLED          0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_CHROMA_KEY_ENABLED         EQU             000000001h
;  RWIVF 
;  1510 : #define NV_PGRAPH_CTX_SWITCH_PLANE_MASK                       14:14 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_PLANE_MASK         TEXTEQU         <14:14>
;  RWI-V 
;  1511 : #define NV_PGRAPH_CTX_SWITCH_PLANE_MASK_DISABLED         0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_PLANE_MASK_DISABLED                EQU             000000000h
;  RW--V 
;  1512 : #define NV_PGRAPH_CTX_SWITCH_PLANE_MASK_ENABLED          0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PLANE_MASK_ENABLED         EQU             000000001h
;  RWIVF 
;  1513 : #define NV_PGRAPH_CTX_SWITCH_USER_CLIP                        15:15 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_USER_CLIP          TEXTEQU         <15:15>
;  RWI-V 
;  1514 : #define NV_PGRAPH_CTX_SWITCH_USER_CLIP_DISABLED          0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_USER_CLIP_DISABLED         EQU             000000000h
;  RW--V 
;  1515 : #define NV_PGRAPH_CTX_SWITCH_USER_CLIP_ENABLED           0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_USER_CLIP_ENABLED          EQU             000000001h
;  RWIVF 
;  1516 : #define NV_PGRAPH_CTX_SWITCH_SRC_BUFFER                       17:16 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_SRC_BUFFER         TEXTEQU         <17:16>
;  RWI-V 
;  1517 : #define NV_PGRAPH_CTX_SWITCH_SRC_BUFFER_0                0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_SRC_BUFFER_0               EQU             000000000h
;  RW--V 
;  1518 : #define NV_PGRAPH_CTX_SWITCH_SRC_BUFFER_1                0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_SRC_BUFFER_1               EQU             000000001h
;  RW--V 
;  1519 : #define NV_PGRAPH_CTX_SWITCH_SRC_BUFFER_2                0x00000002 /* RW--V */
NV_PGRAPH_CTX_SWITCH_SRC_BUFFER_2               EQU             000000002h
;  RW--V 
;  1520 : #define NV_PGRAPH_CTX_SWITCH_SRC_BUFFER_3                0x00000003 /* RW--V */
NV_PGRAPH_CTX_SWITCH_SRC_BUFFER_3               EQU             000000003h
;  RWIVF 
;  1521 : #define NV_PGRAPH_CTX_SWITCH_DST_BUFFER0                      20:20 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_DST_BUFFER0                TEXTEQU         <20:20>
;  RWI-V 
;  1522 : #define NV_PGRAPH_CTX_SWITCH_DST_BUFFER0_DISABLED        0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_DST_BUFFER0_DISABLED               EQU             000000000h
;  RW--V 
;  1523 : #define NV_PGRAPH_CTX_SWITCH_DST_BUFFER0_ENABLED         0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_DST_BUFFER0_ENABLED                EQU             000000001h
;  RWIVF 
;  1524 : #define NV_PGRAPH_CTX_SWITCH_DST_BUFFER1                      21:21 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_DST_BUFFER1                TEXTEQU         <21:21>
;  RWI-V 
;  1525 : #define NV_PGRAPH_CTX_SWITCH_DST_BUFFER1_DISABLED        0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_DST_BUFFER1_DISABLED               EQU             000000000h
;  RW--V 
;  1526 : #define NV_PGRAPH_CTX_SWITCH_DST_BUFFER1_ENABLED         0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_DST_BUFFER1_ENABLED                EQU             000000001h
;  RWIVF 
;  1527 : #define NV_PGRAPH_CTX_SWITCH_DST_BUFFER2                      22:22 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_DST_BUFFER2                TEXTEQU         <22:22>
;  RWI-V 
;  1528 : #define NV_PGRAPH_CTX_SWITCH_DST_BUFFER2_DISABLED        0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_DST_BUFFER2_DISABLED               EQU             000000000h
;  RW--V 
;  1529 : #define NV_PGRAPH_CTX_SWITCH_DST_BUFFER2_ENABLED         0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_DST_BUFFER2_ENABLED                EQU             000000001h
;  RWIVF 
;  1530 : #define NV_PGRAPH_CTX_SWITCH_DST_BUFFER3                      23:23 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_DST_BUFFER3                TEXTEQU         <23:23>
;  RWI-V 
;  1531 : #define NV_PGRAPH_CTX_SWITCH_DST_BUFFER3_DISABLED        0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_DST_BUFFER3_DISABLED               EQU             000000000h
;  RW--V 
;  1532 : #define NV_PGRAPH_CTX_SWITCH_DST_BUFFER3_ENABLED         0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_DST_BUFFER3_ENABLED                EQU             000000001h
;  RWIVF 
;  1533 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG                     28:24 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG               TEXTEQU         <28:24>
;  RWI-V 
;  1534 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_RSVD0          0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_RSVD0         EQU             000000000h
;  RW--V 
;  1535 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_DST_SRC    0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_DST_SRC           EQU             000000001h
;  RW--V 
;  1536 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_SRC_DST    0x00000002 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_SRC_DST           EQU             000000002h
;  RW--V 
;  1537 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_SRC_SRC    0x00000003 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_SRC_SRC           EQU             000000003h
;  RW--V 
;  1538 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_DST_DST    0x00000004 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_DST_DST           EQU             000000004h
;  RW--V 
;  1539 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_DST_SRC    0x00000005 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_DST_SRC           EQU             000000005h
;  RW--V 
;  1540 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_SRC_DST    0x00000006 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_SRC_DST           EQU             000000006h
;  RW--V 
;  1541 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_SRC_SRC0   0x00000007 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_SRC_SRC0          EQU             000000007h
;  RW--V 
;  1542 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_SRC_SRC1   0x00000008 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_SRC_SRC1          EQU             000000008h
;  RW--V 
;  1543 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_SRC_PAT    0x00000009 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_SRC_PAT           EQU             000000009h
;  RW--V 
;  1544 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_PAT_SRC    0x0000000a /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_PAT_SRC           EQU             00000000ah
;  RW--V 
;  1545 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_PAT_PAT    0x0000000b /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_PAT_PAT           EQU             00000000bh
;  RW--V 
;  1546 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_SRC_SRC    0x0000000c /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_SRC_SRC           EQU             00000000ch
;  RW--V 
;  1547 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_SRC_PAT    0x0000000d /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_SRC_PAT           EQU             00000000dh
;  RW--V 
;  1548 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_PAT_SRC    0x0000000e /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_PAT_SRC           EQU             00000000eh
;  RW--V 
;  1549 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_RSVD1          0x0000000f /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_RSVD1         EQU             00000000fh
;  RW--V 
;  1550 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_SRC_DST    0x00000010 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_SRC_DST           EQU             000000010h
;  RW--V 
;  1551 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_DST_SRC    0x00000011 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_DST_SRC           EQU             000000011h
;  RW--V 
;  1552 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_PAT_DST    0x00000012 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_PAT_DST           EQU             000000012h
;  RW--V 
;  1553 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_DST_PAT    0x00000013 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_DST_PAT           EQU             000000013h
;  RW--V 
;  1554 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_PAT_SRC    0x00000014 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_PAT_SRC           EQU             000000014h
;  RW--V 
;  1555 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_SRC_PAT    0x00000015 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_SRC_PAT           EQU             000000015h
;  RW--V 
;  1556 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_RSVD2          0x00000016 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_RSVD2         EQU             000000016h
;  RW--V 
;  1557 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_BYPASS     0x00000017 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_BYPASS            EQU             000000017h
;  RW--V 
;  1558 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD0    0x00000018 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD0           EQU             000000018h
;  RW--V 
;  1559 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_SRC_DST  0x00000019 /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_SRC_DST         EQU             000000019h
;  RW--V 
;  1560 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_DST_SRC  0x0000001a /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_DST_SRC         EQU             00000001ah
;  RW--V 
;  1561 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD1    0x0000001b /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD1           EQU             00000001bh
;  RW--V 
;  1562 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD2    0x0000001c /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD2           EQU             00000001ch
;  RW--V 
;  1563 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_SRC      0x0000001d /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_SRC             EQU             00000001dh
;  RW--V 
;  1564 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD3    0x0000001e /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD3           EQU             00000001eh
;  RW--V 
;  1565 : #define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD4    0x0000001f /* RW--V */
NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD4           EQU             00000001fh
;  RWIVF 
;  1566 : #define NV_PGRAPH_CTX_SWITCH_SPARE3                           29:29 /* RWIVF */
NV_PGRAPH_CTX_SWITCH_SPARE3             TEXTEQU         <29:29>
;  RWI-V 
;  1567 : #define NV_PGRAPH_CTX_SWITCH_SPARE3_DISABLED             0x00000000 /* RWI-V */
NV_PGRAPH_CTX_SWITCH_SPARE3_DISABLED            EQU             000000000h
;  RW--V 
;  1568 : #define NV_PGRAPH_CTX_SWITCH_SPARE3_ENABLED              0x00000001 /* RW--V */
NV_PGRAPH_CTX_SWITCH_SPARE3_ENABLED             EQU             000000001h
;  CWIVF 
;  1569 : #define NV_PGRAPH_CTX_SWITCH_VOLATILE                         31:31 /* CWIVF */
NV_PGRAPH_CTX_SWITCH_VOLATILE           TEXTEQU         <31:31>
;  CWI-V 
;  1570 : #define NV_PGRAPH_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* CWI-V */
NV_PGRAPH_CTX_SWITCH_VOLATILE_IGNORE            EQU             000000000h
;  -W--V 
;  1571 : #define NV_PGRAPH_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
NV_PGRAPH_CTX_SWITCH_VOLATILE_RESET             EQU             000000001h
;  RW-4A 
;        
;  1572 : #define NV_PGRAPH_CTX_CACHE(i)                   (0x004001a0+(i)*4) /* RW-4A */
;  1573 : #define NV_PGRAPH_CTX_CACHE__SIZE_1                               8 /*       */
NV_PGRAPH_CTX_CACHE__SIZE_1             EQU             8t
;  RWIVF 
;  1574 : #define NV_PGRAPH_CTX_CACHE_COLOR                              2:0  /* RWIVF */
NV_PGRAPH_CTX_CACHE_COLOR               TEXTEQU         <2:0>
;  RWIVF 
;  1575 : #define NV_PGRAPH_CTX_CACHE_ALPHA                              3:3  /* RWIVF */
NV_PGRAPH_CTX_CACHE_ALPHA               TEXTEQU         <3:3>
;  RWIVF 
;  1576 : #define NV_PGRAPH_CTX_CACHE_SPARE1                             4:4  /* RWIVF */
NV_PGRAPH_CTX_CACHE_SPARE1              TEXTEQU         <4:4>
;  RWIVF 
;  1577 : #define NV_PGRAPH_CTX_CACHE_MONO_FORMAT                        8:8  /* RWIVF */
NV_PGRAPH_CTX_CACHE_MONO_FORMAT         TEXTEQU         <8:8>
;  RWXVF 
;  1578 : #define NV_PGRAPH_CTX_CACHE_DAC_BYPASS                         9:9  /* RWXVF */
NV_PGRAPH_CTX_CACHE_DAC_BYPASS          TEXTEQU         <9:9>
;  RWXVF 
;  1579 : #define NV_PGRAPH_CTX_CACHE_SPARE2                            10:10 /* RWXVF */
NV_PGRAPH_CTX_CACHE_SPARE2              TEXTEQU         <10:10>
;  RWIVF 
;  1580 : #define NV_PGRAPH_CTX_CACHE_ZWRITE                            12:12 /* RWIVF */
NV_PGRAPH_CTX_CACHE_ZWRITE              TEXTEQU         <12:12>
;  RWIVF 
;  1581 : #define NV_PGRAPH_CTX_CACHE_CHROMA_KEY                        13:13 /* RWIVF */
NV_PGRAPH_CTX_CACHE_CHROMA_KEY          TEXTEQU         <13:13>
;  RWIVF 
;  1582 : #define NV_PGRAPH_CTX_CACHE_PLANE_MASK                        14:14 /* RWIVF */
NV_PGRAPH_CTX_CACHE_PLANE_MASK          TEXTEQU         <14:14>
;  RWIVF 
;  1583 : #define NV_PGRAPH_CTX_CACHE_USER_CLIP                         15:15 /* RWIVF */
NV_PGRAPH_CTX_CACHE_USER_CLIP           TEXTEQU         <15:15>
;  RWIVF 
;  1584 : #define NV_PGRAPH_CTX_CACHE_SRC_BUFFER                        17:16 /* RWIVF */
NV_PGRAPH_CTX_CACHE_SRC_BUFFER          TEXTEQU         <17:16>
;  RWIVF 
;  1585 : #define NV_PGRAPH_CTX_CACHE_DST_BUFFER0                       20:20 /* RWIVF */
NV_PGRAPH_CTX_CACHE_DST_BUFFER0         TEXTEQU         <20:20>
;  RWIVF 
;  1586 : #define NV_PGRAPH_CTX_CACHE_DST_BUFFER1                       21:21 /* RWIVF */
NV_PGRAPH_CTX_CACHE_DST_BUFFER1         TEXTEQU         <21:21>
;  RWIVF 
;  1587 : #define NV_PGRAPH_CTX_CACHE_DST_BUFFER2                       22:22 /* RWIVF */
NV_PGRAPH_CTX_CACHE_DST_BUFFER2         TEXTEQU         <22:22>
;  RWIVF 
;  1588 : #define NV_PGRAPH_CTX_CACHE_DST_BUFFER3                       23:23 /* RWIVF */
NV_PGRAPH_CTX_CACHE_DST_BUFFER3         TEXTEQU         <23:23>
;  RWIVF 
;  1589 : #define NV_PGRAPH_CTX_CACHE_PATCH_CONFIG                      28:24 /* RWIVF */
NV_PGRAPH_CTX_CACHE_PATCH_CONFIG                TEXTEQU         <28:24>
;  RWXVF 
;  1590 : #define NV_PGRAPH_CTX_CACHE_SPARE3                            29:29 /* RWXVF */
NV_PGRAPH_CTX_CACHE_SPARE3              TEXTEQU         <29:29>
;  RW-4R 
;  1591 : #define NV_PGRAPH_CTX_CONTROL                            0x00400190 /* RW-4R */
NV_PGRAPH_CTX_CONTROL           EQU             000400190h
;  RWIVF 
;  1592 : #define NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME                      1:0 /* RWIVF */
NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME              TEXTEQU         <1:0>
;  RWI-V 
;  1593 : #define NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME_33US          0x00000000 /* RWI-V */
NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME_33US         EQU             000000000h
;  RW--V 
;  1594 : #define NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME_262US         0x00000001 /* RW--V */
NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME_262US                EQU             000000001h
;  RW--V 
;  1595 : #define NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME_2MS           0x00000002 /* RW--V */
NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME_2MS          EQU             000000002h
;  RW--V 
;  1596 : #define NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME_17MS          0x00000003 /* RW--V */
NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME_17MS         EQU             000000003h
;  RWIVF 
;  1597 : #define NV_PGRAPH_CTX_CONTROL_TIME                              8:8 /* RWIVF */
NV_PGRAPH_CTX_CONTROL_TIME              TEXTEQU         <8:8>
;  RWI-V 
;  1598 : #define NV_PGRAPH_CTX_CONTROL_TIME_EXPIRED               0x00000000 /* RWI-V */
NV_PGRAPH_CTX_CONTROL_TIME_EXPIRED              EQU             000000000h
;  RW--V 
;  1599 : #define NV_PGRAPH_CTX_CONTROL_TIME_NOT_EXPIRED           0x00000001 /* RW--V */
NV_PGRAPH_CTX_CONTROL_TIME_NOT_EXPIRED          EQU             000000001h
;  RWIVF 
;  1600 : #define NV_PGRAPH_CTX_CONTROL_CHID                            16:16 /* RWIVF */
NV_PGRAPH_CTX_CONTROL_CHID              TEXTEQU         <16:16>
;  RWI-V 
;  1601 : #define NV_PGRAPH_CTX_CONTROL_CHID_INVALID               0x00000000 /* RWI-V */
NV_PGRAPH_CTX_CONTROL_CHID_INVALID              EQU             000000000h
;  RW--V 
;  1602 : #define NV_PGRAPH_CTX_CONTROL_CHID_VALID                 0x00000001 /* RW--V */
NV_PGRAPH_CTX_CONTROL_CHID_VALID                EQU             000000001h
;  R--VF 
;  1603 : #define NV_PGRAPH_CTX_CONTROL_SWITCH                          20:20 /* R--VF */
NV_PGRAPH_CTX_CONTROL_SWITCH            TEXTEQU         <20:20>
;  R---V 
;  1604 : #define NV_PGRAPH_CTX_CONTROL_SWITCH_UNAVAILABLE         0x00000000 /* R---V */
NV_PGRAPH_CTX_CONTROL_SWITCH_UNAVAILABLE                EQU             000000000h
;  R---V 
;  1605 : #define NV_PGRAPH_CTX_CONTROL_SWITCH_AVAILABLE           0x00000001 /* R---V */
NV_PGRAPH_CTX_CONTROL_SWITCH_AVAILABLE          EQU             000000001h
;  RWIVF 
;  1606 : #define NV_PGRAPH_CTX_CONTROL_SWITCHING                       24:24 /* RWIVF */
NV_PGRAPH_CTX_CONTROL_SWITCHING         TEXTEQU         <24:24>
;  RWI-V 
;  1607 : #define NV_PGRAPH_CTX_CONTROL_SWITCHING_IDLE             0x00000000 /* RWI-V */
NV_PGRAPH_CTX_CONTROL_SWITCHING_IDLE            EQU             000000000h
;  RW--V 
;  1608 : #define NV_PGRAPH_CTX_CONTROL_SWITCHING_BUSY             0x00000001 /* RW--V */
NV_PGRAPH_CTX_CONTROL_SWITCHING_BUSY            EQU             000000001h
;  RWIVF 
;  1609 : #define NV_PGRAPH_CTX_CONTROL_DEVICE                          28:28 /* RWIVF */
NV_PGRAPH_CTX_CONTROL_DEVICE            TEXTEQU         <28:28>
;  RWI-V 
;  1610 : #define NV_PGRAPH_CTX_CONTROL_DEVICE_DISABLED            0x00000000 /* RWI-V */
NV_PGRAPH_CTX_CONTROL_DEVICE_DISABLED           EQU             000000000h
;  RW--V 
;  1611 : #define NV_PGRAPH_CTX_CONTROL_DEVICE_ENABLED             0x00000001 /* RW--V */
NV_PGRAPH_CTX_CONTROL_DEVICE_ENABLED            EQU             000000001h
;  RW-4R 
;  1612 : #define NV_PGRAPH_CTX_USER                               0x00400194 /* RW-4R */
NV_PGRAPH_CTX_USER              EQU             000400194h
;  RWXVF 
;  1613 : #define NV_PGRAPH_CTX_USER_SUBCH                              15:13 /* RWXVF */
NV_PGRAPH_CTX_USER_SUBCH                TEXTEQU         <15:13>
;  RWXVF 
;  1614 : #define NV_PGRAPH_CTX_USER_CLASS                              20:16 /* RWXVF */
NV_PGRAPH_CTX_USER_CLASS                TEXTEQU         <20:16>
;  RWXVF 
;  1615 : #define NV_PGRAPH_CTX_USER_CHID                               30:24 /* RWXVF */
NV_PGRAPH_CTX_USER_CHID         TEXTEQU         <30:24>
;  RW-4R 
;  1616 : #define NV_PGRAPH_FIFO                                   0x004006A4 /* RW-4R */
NV_PGRAPH_FIFO          EQU             0004006a4h
;  RWIVF 
;  1617 : #define NV_PGRAPH_FIFO_ACCESS                                   0:0 /* RWIVF */
NV_PGRAPH_FIFO_ACCESS           TEXTEQU         <0:0>
;  RW--V 
;  1618 : #define NV_PGRAPH_FIFO_ACCESS_DISABLED                   0x00000000 /* RW--V */
NV_PGRAPH_FIFO_ACCESS_DISABLED          EQU             000000000h
;  RWI-V 
;  1619 : #define NV_PGRAPH_FIFO_ACCESS_ENABLED                    0x00000001 /* RWI-V */
NV_PGRAPH_FIFO_ACCESS_ENABLED           EQU             000000001h
;  R--4R 
;  1620 : #define NV_PGRAPH_STATUS                                 0x004006B0 /* R--4R */
NV_PGRAPH_STATUS                EQU             0004006b0h
;  R-IVF 
;  1621 : #define NV_PGRAPH_STATUS_STATE                                  0:0 /* R-IVF */
NV_PGRAPH_STATUS_STATE          TEXTEQU         <0:0>
;  R-I-V 
;  1622 : #define NV_PGRAPH_STATUS_STATE_IDLE                      0x00000000 /* R-I-V */
NV_PGRAPH_STATUS_STATE_IDLE             EQU             000000000h
;  R---V 
;  1623 : #define NV_PGRAPH_STATUS_STATE_BUSY                      0x00000001 /* R---V */
NV_PGRAPH_STATUS_STATE_BUSY             EQU             000000001h
;  R-IVF 
;  1624 : #define NV_PGRAPH_STATUS_XY_LOGIC                               4:4 /* R-IVF */
NV_PGRAPH_STATUS_XY_LOGIC               TEXTEQU         <4:4>
;  R-I-V 
;  1625 : #define NV_PGRAPH_STATUS_XY_LOGIC_IDLE                   0x00000000 /* R-I-V */
NV_PGRAPH_STATUS_XY_LOGIC_IDLE          EQU             000000000h
;  R---V 
;  1626 : #define NV_PGRAPH_STATUS_XY_LOGIC_BUSY                   0x00000001 /* R---V */
NV_PGRAPH_STATUS_XY_LOGIC_BUSY          EQU             000000001h
;  R-IVF 
;  1627 : #define NV_PGRAPH_STATUS_PORT_NOTIFY                            8:8 /* R-IVF */
NV_PGRAPH_STATUS_PORT_NOTIFY            TEXTEQU         <8:8>
;  R-I-V 
;  1628 : #define NV_PGRAPH_STATUS_PORT_NOTIFY_IDLE                0x00000000 /* R-I-V */
NV_PGRAPH_STATUS_PORT_NOTIFY_IDLE               EQU             000000000h
;  R---V 
;  1629 : #define NV_PGRAPH_STATUS_PORT_NOTIFY_BUSY                0x00000001 /* R---V */
NV_PGRAPH_STATUS_PORT_NOTIFY_BUSY               EQU             000000001h
;  R-IVF 
;  1630 : #define NV_PGRAPH_STATUS_PORT_REGISTER                        12:12 /* R-IVF */
NV_PGRAPH_STATUS_PORT_REGISTER          TEXTEQU         <12:12>
;  R-I-V 
;  1631 : #define NV_PGRAPH_STATUS_PORT_REGISTER_IDLE              0x00000000 /* R-I-V */
NV_PGRAPH_STATUS_PORT_REGISTER_IDLE             EQU             000000000h
;  R---V 
;  1632 : #define NV_PGRAPH_STATUS_PORT_REGISTER_BUSY              0x00000001 /* R---V */
NV_PGRAPH_STATUS_PORT_REGISTER_BUSY             EQU             000000001h
;  R-IVF 
;  1633 : #define NV_PGRAPH_STATUS_PORT_DMA                             16:16 /* R-IVF */
NV_PGRAPH_STATUS_PORT_DMA               TEXTEQU         <16:16>
;  R-I-V 
;  1634 : #define NV_PGRAPH_STATUS_PORT_DMA_IDLE                   0x00000000 /* R-I-V */
NV_PGRAPH_STATUS_PORT_DMA_IDLE          EQU             000000000h
;  R---V 
;  1635 : #define NV_PGRAPH_STATUS_PORT_DMA_BUSY                   0x00000001 /* R---V */
NV_PGRAPH_STATUS_PORT_DMA_BUSY          EQU             000000001h
;  R-IVF 
;  1636 : #define NV_PGRAPH_STATUS_DMA_ENGINE                           17:17 /* R-IVF */
NV_PGRAPH_STATUS_DMA_ENGINE             TEXTEQU         <17:17>
;  R-I-V 
;  1637 : #define NV_PGRAPH_STATUS_DMA_ENGINE_IDLE                 0x00000000 /* R-I-V */
NV_PGRAPH_STATUS_DMA_ENGINE_IDLE                EQU             000000000h
;  R---V 
;  1638 : #define NV_PGRAPH_STATUS_DMA_ENGINE_BUSY                 0x00000001 /* R---V */
NV_PGRAPH_STATUS_DMA_ENGINE_BUSY                EQU             000000001h
;  R-IVF 
;  1639 : #define NV_PGRAPH_STATUS_DMA_NOTIFY                           20:20 /* R-IVF */
NV_PGRAPH_STATUS_DMA_NOTIFY             TEXTEQU         <20:20>
;  R-I-V 
;  1640 : #define NV_PGRAPH_STATUS_DMA_NOTIFY_IDLE                 0x00000000 /* R-I-V */
NV_PGRAPH_STATUS_DMA_NOTIFY_IDLE                EQU             000000000h
;  R---V 
;  1641 : #define NV_PGRAPH_STATUS_DMA_NOTIFY_BUSY                 0x00000001 /* R---V */
NV_PGRAPH_STATUS_DMA_NOTIFY_BUSY                EQU             000000001h
;  R-IVF 
;  1642 : #define NV_PGRAPH_STATUS_3D                                   24:24 /* R-IVF */
NV_PGRAPH_STATUS_3D             TEXTEQU         <24:24>
;  R-I-V 
;  1643 : #define NV_PGRAPH_STATUS_3D_IDLE                         0x00000000 /* R-I-V */
NV_PGRAPH_STATUS_3D_IDLE                EQU             000000000h
;  R---V 
;  1644 : #define NV_PGRAPH_STATUS_3D_BUSY                         0x00000001 /* R---V */
NV_PGRAPH_STATUS_3D_BUSY                EQU             000000001h
;  R-IVF 
;  1645 : #define NV_PGRAPH_STATUS_CACHE                                25:25 /* R-IVF */
NV_PGRAPH_STATUS_CACHE          TEXTEQU         <25:25>
;  R-I-V 
;  1646 : #define NV_PGRAPH_STATUS_CACHE_IDLE                      0x00000000 /* R-I-V */
NV_PGRAPH_STATUS_CACHE_IDLE             EQU             000000000h
;  R---V 
;  1647 : #define NV_PGRAPH_STATUS_CACHE_BUSY                      0x00000001 /* R---V */
NV_PGRAPH_STATUS_CACHE_BUSY             EQU             000000001h
;  R-IVF 
;  1648 : #define NV_PGRAPH_STATUS_LIGHTING                             26:26 /* R-IVF */
NV_PGRAPH_STATUS_LIGHTING               TEXTEQU         <26:26>
;  R-I-V 
;  1649 : #define NV_PGRAPH_STATUS_LIGHTING_IDLE                   0x00000000 /* R-I-V */
NV_PGRAPH_STATUS_LIGHTING_IDLE          EQU             000000000h
;  R---V 
;  1650 : #define NV_PGRAPH_STATUS_LIGHTING_BUSY                   0x00000001 /* R---V */
NV_PGRAPH_STATUS_LIGHTING_BUSY          EQU             000000001h
;  R-IVF 
;  1651 : #define NV_PGRAPH_STATUS_ZFIFO                                27:27 /* R-IVF */
NV_PGRAPH_STATUS_ZFIFO          TEXTEQU         <27:27>
;  R-I-V 
;  1652 : #define NV_PGRAPH_STATUS_ZFIFO_IDLE                      0x00000000 /* R-I-V */
NV_PGRAPH_STATUS_ZFIFO_IDLE             EQU             000000000h
;  R---V 
;  1653 : #define NV_PGRAPH_STATUS_ZFIFO_BUSY                      0x00000001 /* R---V */
NV_PGRAPH_STATUS_ZFIFO_BUSY             EQU             000000001h
;  R--4R 
;  1654 : #define NV_PGRAPH_TRAPPED_ADDR                           0x004006B4 /* R--4R */
NV_PGRAPH_TRAPPED_ADDR          EQU             0004006b4h
;  R-XUF 
;  1655 : #define NV_PGRAPH_TRAPPED_ADDR_MTHD                            12:2 /* R-XUF */
NV_PGRAPH_TRAPPED_ADDR_MTHD             TEXTEQU         <12:2>
;  R-XUF 
;  1656 : #define NV_PGRAPH_TRAPPED_ADDR_SUBCH                          15:13 /* R-XUF */
NV_PGRAPH_TRAPPED_ADDR_SUBCH            TEXTEQU         <15:13>
;  R-XUF 
;  1657 : #define NV_PGRAPH_TRAPPED_ADDR_CLASS                          20:16 /* R-XUF */
NV_PGRAPH_TRAPPED_ADDR_CLASS            TEXTEQU         <20:16>
;  R-XUF 
;  1658 : #define NV_PGRAPH_TRAPPED_ADDR_CHID                           30:24 /* R-XUF */
NV_PGRAPH_TRAPPED_ADDR_CHID             TEXTEQU         <30:24>
;  R--4R 
;  1659 : #define NV_PGRAPH_TRAPPED_DATA                           0x004006B8 /* R--4R */
NV_PGRAPH_TRAPPED_DATA          EQU             0004006b8h
;  R-XVF 
;  1660 : #define NV_PGRAPH_TRAPPED_DATA_VALUE                           31:0 /* R-XVF */
NV_PGRAPH_TRAPPED_DATA_VALUE            TEXTEQU         <31:0>
;  R--4R 
;  1661 : #define NV_PGRAPH_TRAPPED_INST                           0x004006BC /* R--4R */
NV_PGRAPH_TRAPPED_INST          EQU             0004006bch
;  R-XVF 
;  1662 : #define NV_PGRAPH_TRAPPED_INST_VALUE                           15:0 /* R-XVF */
NV_PGRAPH_TRAPPED_INST_VALUE            TEXTEQU         <15:0>
;  RW-4R 
;  1663 : #define NV_PGRAPH_CLIP_MISC                              0x004006A0 /* RW-4R */
NV_PGRAPH_CLIP_MISC             EQU             0004006a0h
;  RWIUF 
;  1664 : #define NV_PGRAPH_CLIP_MISC_REGIONS                             1:0 /* RWIUF */
NV_PGRAPH_CLIP_MISC_REGIONS             TEXTEQU         <1:0>
;  RWI-V 
;  1665 : #define NV_PGRAPH_CLIP_MISC_REGIONS_DISABLED             0x00000000 /* RWI-V */
NV_PGRAPH_CLIP_MISC_REGIONS_DISABLED            EQU             000000000h
;  RW--V 
;  1666 : #define NV_PGRAPH_CLIP_MISC_REGIONS_1                    0x00000001 /* RW--V */
NV_PGRAPH_CLIP_MISC_REGIONS_1           EQU             000000001h
;  RW--V 
;  1667 : #define NV_PGRAPH_CLIP_MISC_REGIONS_2                    0x00000002 /* RW--V */
NV_PGRAPH_CLIP_MISC_REGIONS_2           EQU             000000002h
;  RWIVF 
;  1668 : #define NV_PGRAPH_CLIP_MISC_RENDER                              4:4 /* RWIVF */
NV_PGRAPH_CLIP_MISC_RENDER              TEXTEQU         <4:4>
;  RWI-V 
;  1669 : #define NV_PGRAPH_CLIP_MISC_RENDER_INCLUDED              0x00000000 /* RWI-V */
NV_PGRAPH_CLIP_MISC_RENDER_INCLUDED             EQU             000000000h
;  RW--V 
;  1670 : #define NV_PGRAPH_CLIP_MISC_RENDER_OCCLUDED              0x00000001 /* RW--V */
NV_PGRAPH_CLIP_MISC_RENDER_OCCLUDED             EQU             000000001h
;  RWIVF 
;  1671 : #define NV_PGRAPH_CLIP_MISC_COMPLEX                             8:8 /* RWIVF */
NV_PGRAPH_CLIP_MISC_COMPLEX             TEXTEQU         <8:8>
;  RWI-V 
;  1672 : #define NV_PGRAPH_CLIP_MISC_COMPLEX_DISABLED             0x00000000 /* RWI-V */
NV_PGRAPH_CLIP_MISC_COMPLEX_DISABLED            EQU             000000000h
;  RW--V 
;  1673 : #define NV_PGRAPH_CLIP_MISC_COMPLEX_ENABLED              0x00000001 /* RW--V */
NV_PGRAPH_CLIP_MISC_COMPLEX_ENABLED             EQU             000000001h
;  RW-4R 
;  1674 : #define NV_PGRAPH_SRC_CANVAS_MIN                         0x00400550 /* RW-4R */
NV_PGRAPH_SRC_CANVAS_MIN                EQU             000400550h
;  RWXUF 
;  1675 : #define NV_PGRAPH_SRC_CANVAS_MIN_X                             10:0 /* RWXUF */
NV_PGRAPH_SRC_CANVAS_MIN_X              TEXTEQU         <10:0>
;  RWXUF 
;  1676 : #define NV_PGRAPH_SRC_CANVAS_MIN_Y                            29:16 /* RWXUF */
NV_PGRAPH_SRC_CANVAS_MIN_Y              TEXTEQU         <29:16>
;  RW-4R 
;  1677 : #define NV_PGRAPH_DST_CANVAS_MIN                         0x00400558 /* RW-4R */
NV_PGRAPH_DST_CANVAS_MIN                EQU             000400558h
;  RWXUF 
;  1678 : #define NV_PGRAPH_DST_CANVAS_MIN_X                             10:0 /* RWXUF */
NV_PGRAPH_DST_CANVAS_MIN_X              TEXTEQU         <10:0>
;  RWXUF 
;  1679 : #define NV_PGRAPH_DST_CANVAS_MIN_Y                            29:16 /* RWXUF */
NV_PGRAPH_DST_CANVAS_MIN_Y              TEXTEQU         <29:16>
;  RW-4R 
;  1680 : #define NV_PGRAPH_SRC_CANVAS_MAX                         0x00400554 /* RW-4R */
NV_PGRAPH_SRC_CANVAS_MAX                EQU             000400554h
;  RWXUF 
;  1681 : #define NV_PGRAPH_SRC_CANVAS_MAX_X                             10:0 /* RWXUF */
NV_PGRAPH_SRC_CANVAS_MAX_X              TEXTEQU         <10:0>
;  RWXUF 
;  1682 : #define NV_PGRAPH_SRC_CANVAS_MAX_Y                            29:16 /* RWXUF */
NV_PGRAPH_SRC_CANVAS_MAX_Y              TEXTEQU         <29:16>
;  RW-4R 
;  1683 : #define NV_PGRAPH_DST_CANVAS_MAX                         0x0040055C /* RW-4R */
NV_PGRAPH_DST_CANVAS_MAX                EQU             00040055ch
;  RWXUF 
;  1684 : #define NV_PGRAPH_DST_CANVAS_MAX_X                             10:0 /* RWXUF */
NV_PGRAPH_DST_CANVAS_MAX_X              TEXTEQU         <10:0>
;  RWXUF 
;  1685 : #define NV_PGRAPH_DST_CANVAS_MAX_Y                            29:16 /* RWXUF */
NV_PGRAPH_DST_CANVAS_MAX_Y              TEXTEQU         <29:16>
;  RW-4R 
;  1686 : #define NV_PGRAPH_CLIP0_MIN                              0x00400690 /* RW-4R */
NV_PGRAPH_CLIP0_MIN             EQU             000400690h
;  RWXSF 
;  1687 : #define NV_PGRAPH_CLIP0_MIN_X                                  10:0 /* RWXSF */
NV_PGRAPH_CLIP0_MIN_X           TEXTEQU         <10:0>
;  RWXSF 
;  1688 : #define NV_PGRAPH_CLIP0_MIN_Y                                 27:16 /* RWXSF */
NV_PGRAPH_CLIP0_MIN_Y           TEXTEQU         <27:16>
;  RW-4R 
;  1689 : #define NV_PGRAPH_CLIP1_MIN                              0x00400698 /* RW-4R */
NV_PGRAPH_CLIP1_MIN             EQU             000400698h
;  RWXSF 
;  1690 : #define NV_PGRAPH_CLIP1_MIN_X                                  10:0 /* RWXSF */
NV_PGRAPH_CLIP1_MIN_X           TEXTEQU         <10:0>
;  RWXSF 
;  1691 : #define NV_PGRAPH_CLIP1_MIN_Y                                 27:16 /* RWXSF */
NV_PGRAPH_CLIP1_MIN_Y           TEXTEQU         <27:16>
;  RW-4R 
;  1692 : #define NV_PGRAPH_CLIP0_MAX                              0x00400694 /* RW-4R */
NV_PGRAPH_CLIP0_MAX             EQU             000400694h
;  RWXSF 
;  1693 : #define NV_PGRAPH_CLIP0_MAX_X                                  10:0 /* RWXSF */
NV_PGRAPH_CLIP0_MAX_X           TEXTEQU         <10:0>
;  RWXSF 
;  1694 : #define NV_PGRAPH_CLIP0_MAX_Y                                 27:16 /* RWXSF */
NV_PGRAPH_CLIP0_MAX_Y           TEXTEQU         <27:16>
;  RW-4R 
;  1695 : #define NV_PGRAPH_CLIP1_MAX                              0x0040069C /* RW-4R */
NV_PGRAPH_CLIP1_MAX             EQU             00040069ch
;  RWXSF 
;  1696 : #define NV_PGRAPH_CLIP1_MAX_X                                  10:0 /* RWXSF */
NV_PGRAPH_CLIP1_MAX_X           TEXTEQU         <10:0>
;  RWXSF 
;  1697 : #define NV_PGRAPH_CLIP1_MAX_Y                                 27:16 /* RWXSF */
NV_PGRAPH_CLIP1_MAX_Y           TEXTEQU         <27:16>
;  RW-4R 
;  1698 : #define NV_PGRAPH_DMA                                    0x00400680 /* RW-4R */
NV_PGRAPH_DMA           EQU             000400680h
;  RWXUF 
;  1699 : #define NV_PGRAPH_DMA_INSTANCE                                 15:0 /* RWXUF */
NV_PGRAPH_DMA_INSTANCE          TEXTEQU         <15:0>
;  RW-4R 
;  1700 : #define NV_PGRAPH_NOTIFY                                 0x00400684 /* RW-4R */
NV_PGRAPH_NOTIFY                EQU             000400684h
;  RWXUF 
;  1701 : #define NV_PGRAPH_NOTIFY_INSTANCE                              15:0 /* RWXUF */
NV_PGRAPH_NOTIFY_INSTANCE               TEXTEQU         <15:0>
;  RWIVF 
;  1702 : #define NV_PGRAPH_NOTIFY_REQ                                  16:16 /* RWIVF */
NV_PGRAPH_NOTIFY_REQ            TEXTEQU         <16:16>
;  RWI-V 
;  1703 : #define NV_PGRAPH_NOTIFY_REQ_NOT_PENDING                 0x00000000 /* RWI-V */
NV_PGRAPH_NOTIFY_REQ_NOT_PENDING                EQU             000000000h
;  RW--V 
;  1704 : #define NV_PGRAPH_NOTIFY_REQ_PENDING                     0x00000001 /* RW--V */
NV_PGRAPH_NOTIFY_REQ_PENDING            EQU             000000001h
;  RWIVF 
;  1705 : #define NV_PGRAPH_NOTIFY_TYPE                                 23:20 /* RWIVF */
NV_PGRAPH_NOTIFY_TYPE           TEXTEQU         <23:20>
;  RWI-V 
;  1706 : #define NV_PGRAPH_NOTIFY_TYPE_HW                         0x00000000 /* RWI-V */
NV_PGRAPH_NOTIFY_TYPE_HW                EQU             000000000h
;  RW--V 
;  1707 : #define NV_PGRAPH_NOTIFY_TYPE_SW_1                       0x00000001 /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_1              EQU             000000001h
;  RW--V 
;  1708 : #define NV_PGRAPH_NOTIFY_TYPE_SW_2                       0x00000002 /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_2              EQU             000000002h
;  RW--V 
;  1709 : #define NV_PGRAPH_NOTIFY_TYPE_SW_3                       0x00000003 /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_3              EQU             000000003h
;  RW--V 
;  1710 : #define NV_PGRAPH_NOTIFY_TYPE_SW_4                       0x00000004 /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_4              EQU             000000004h
;  RW--V 
;  1711 : #define NV_PGRAPH_NOTIFY_TYPE_SW_5                       0x00000005 /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_5              EQU             000000005h
;  RW--V 
;  1712 : #define NV_PGRAPH_NOTIFY_TYPE_SW_6                       0x00000006 /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_6              EQU             000000006h
;  RW--V 
;  1713 : #define NV_PGRAPH_NOTIFY_TYPE_SW_7                       0x00000007 /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_7              EQU             000000007h
;  RW--V 
;  1714 : #define NV_PGRAPH_NOTIFY_TYPE_SW_8                       0x00000008 /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_8              EQU             000000008h
;  RW--V 
;  1715 : #define NV_PGRAPH_NOTIFY_TYPE_SW_9                       0x00000009 /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_9              EQU             000000009h
;  RW--V 
;  1716 : #define NV_PGRAPH_NOTIFY_TYPE_SW_10                      0x0000000A /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_10             EQU             00000000ah
;  RW--V 
;  1717 : #define NV_PGRAPH_NOTIFY_TYPE_SW_11                      0x0000000B /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_11             EQU             00000000bh
;  RW--V 
;  1718 : #define NV_PGRAPH_NOTIFY_TYPE_SW_12                      0x0000000C /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_12             EQU             00000000ch
;  RW--V 
;  1719 : #define NV_PGRAPH_NOTIFY_TYPE_SW_13                      0x0000000D /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_13             EQU             00000000dh
;  RW--V 
;  1720 : #define NV_PGRAPH_NOTIFY_TYPE_SW_14                      0x0000000E /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_14             EQU             00000000eh
;  RW--V 
;  1721 : #define NV_PGRAPH_NOTIFY_TYPE_SW_15                      0x0000000F /* RW--V */
NV_PGRAPH_NOTIFY_TYPE_SW_15             EQU             00000000fh
;  RW-4R 
;  1722 : #define NV_PGRAPH_INSTANCE                               0x00400688 /* RW-4R */
NV_PGRAPH_INSTANCE              EQU             000400688h
;  RWXUF 
;  1723 : #define NV_PGRAPH_INSTANCE_TAG                                 15:0 /* RWXUF */
NV_PGRAPH_INSTANCE_TAG          TEXTEQU         <15:0>
;  RWI-V 
;  1724 : #define NV_PGRAPH_INSTANCE_TAG_INVALID                   0x00000000 /* RWI-V */
NV_PGRAPH_INSTANCE_TAG_INVALID          EQU             000000000h
;  RW-4R 
;  1725 : #define NV_PGRAPH_MEMFMT                                 0x0040068C /* RW-4R */
NV_PGRAPH_MEMFMT                EQU             00040068ch
;  RWXUF 
;  1726 : #define NV_PGRAPH_MEMFMT_INSTANCE                              15:0 /* RWXUF */
NV_PGRAPH_MEMFMT_INSTANCE               TEXTEQU         <15:0>
;  RWIVF 
;  1727 : #define NV_PGRAPH_MEMFMT_LINEAR                               16:16 /* RWIVF */
NV_PGRAPH_MEMFMT_LINEAR         TEXTEQU         <16:16>
;  RW--V 
;  1728 : #define NV_PGRAPH_MEMFMT_LINEAR_OUT                      0x00000000 /* RW--V */
NV_PGRAPH_MEMFMT_LINEAR_OUT             EQU             000000000h
;  RW--V 
;  1729 : #define NV_PGRAPH_MEMFMT_LINEAR_IN                       0x00000001 /* RW--V */
NV_PGRAPH_MEMFMT_LINEAR_IN              EQU             000000001h
;  RW-4R 
;  1730 : #define NV_PGRAPH_BOFFSET0                               0x00400630 /* RW-4R */
NV_PGRAPH_BOFFSET0              EQU             000400630h
;  RWIUF 
;  1731 : #define NV_PGRAPH_BOFFSET0_LINADRS                             22:0 /* RWIUF */
NV_PGRAPH_BOFFSET0_LINADRS              TEXTEQU         <22:0>
;  RWI-V 
;  1732 : #define NV_PGRAPH_BOFFSET0_LINADRS_0                     0x00000000 /* RWI-V */
NV_PGRAPH_BOFFSET0_LINADRS_0            EQU             000000000h
;  RW-4R 
;  1733 : #define NV_PGRAPH_BOFFSET1                               0x00400634 /* RW-4R */
NV_PGRAPH_BOFFSET1              EQU             000400634h
;  RWIUF 
;  1734 : #define NV_PGRAPH_BOFFSET1_LINADRS                             22:0 /* RWIUF */
NV_PGRAPH_BOFFSET1_LINADRS              TEXTEQU         <22:0>
;  RWI-V 
;  1735 : #define NV_PGRAPH_BOFFSET1_LINADRS_0                     0x00000000 /* RWI-V */
NV_PGRAPH_BOFFSET1_LINADRS_0            EQU             000000000h
;  RW-4R 
;  1736 : #define NV_PGRAPH_BOFFSET2                               0x00400638 /* RW-4R */
NV_PGRAPH_BOFFSET2              EQU             000400638h
;  RWIUF 
;  1737 : #define NV_PGRAPH_BOFFSET2_LINADRS                             22:0 /* RWIUF */
NV_PGRAPH_BOFFSET2_LINADRS              TEXTEQU         <22:0>
;  RWI-V 
;  1738 : #define NV_PGRAPH_BOFFSET2_LINADRS_0                     0x00000000 /* RWI-V */
NV_PGRAPH_BOFFSET2_LINADRS_0            EQU             000000000h
;  RW-4R 
;  1739 : #define NV_PGRAPH_BOFFSET3                               0x0040063C /* RW-4R */
NV_PGRAPH_BOFFSET3              EQU             00040063ch
;  RWIUF 
;  1740 : #define NV_PGRAPH_BOFFSET3_LINADRS                             22:0 /* RWIUF */
NV_PGRAPH_BOFFSET3_LINADRS              TEXTEQU         <22:0>
;  RWI-V 
;  1741 : #define NV_PGRAPH_BOFFSET3_LINADRS_0                     0x00000000 /* RWI-V */
NV_PGRAPH_BOFFSET3_LINADRS_0            EQU             000000000h
;  RW-4R 
;  1742 : #define NV_PGRAPH_BPITCH0                                0x00400650 /* RW-4R */
NV_PGRAPH_BPITCH0               EQU             000400650h
;  RWXUF 
;  1743 : #define NV_PGRAPH_BPITCH0_VALUE                                15:0 /* RWXUF */
NV_PGRAPH_BPITCH0_VALUE         TEXTEQU         <15:0>
;  RW-4R 
;  1744 : #define NV_PGRAPH_BPITCH1                                0x00400654 /* RW-4R */
NV_PGRAPH_BPITCH1               EQU             000400654h
;  RWXUF 
;  1745 : #define NV_PGRAPH_BPITCH1_VALUE                                15:0 /* RWXUF */
NV_PGRAPH_BPITCH1_VALUE         TEXTEQU         <15:0>
;  RW-4R 
;  1746 : #define NV_PGRAPH_BPITCH2                                0x00400658 /* RW-4R */
NV_PGRAPH_BPITCH2               EQU             000400658h
;  RWXUF 
;  1747 : #define NV_PGRAPH_BPITCH2_VALUE                                15:0 /* RWXUF */
NV_PGRAPH_BPITCH2_VALUE         TEXTEQU         <15:0>
;  RW-4R 
;  1748 : #define NV_PGRAPH_BPITCH3                                0x0040065C /* RW-4R */
NV_PGRAPH_BPITCH3               EQU             00040065ch
;  RWXUF 
;  1749 : #define NV_PGRAPH_BPITCH3_VALUE                                15:0 /* RWXUF */
NV_PGRAPH_BPITCH3_VALUE         TEXTEQU         <15:0>
;  RW-4R 
;  1750 : #define NV_PGRAPH_BPIXEL                                 0x004006a8 /* RW-4R */
NV_PGRAPH_BPIXEL                EQU             0004006a8h
;  RWXVF 
;  1751 : #define NV_PGRAPH_BPIXEL_DEPTH0_FMT                             1:0 /* RWXVF */
NV_PGRAPH_BPIXEL_DEPTH0_FMT             TEXTEQU         <1:0>
;  RW--V 
;  1752 : #define NV_PGRAPH_BPIXEL_DEPTH0_FMT_Y16_BITS             0x00000000 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH0_FMT_Y16_BITS            EQU             000000000h
;  RW--V 
;  1753 : #define NV_PGRAPH_BPIXEL_DEPTH0_FMT_8_BITS               0x00000001 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH0_FMT_8_BITS              EQU             000000001h
;  RW--V 
;  1754 : #define NV_PGRAPH_BPIXEL_DEPTH0_FMT_16_BITS              0x00000002 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH0_FMT_16_BITS             EQU             000000002h
;  RW--V 
;  1755 : #define NV_PGRAPH_BPIXEL_DEPTH0_FMT_32_BITS              0x00000003 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH0_FMT_32_BITS             EQU             000000003h
;  RWXVF 
;  1756 : #define NV_PGRAPH_BPIXEL_DEPTH0                                 2:2 /* RWXVF */
NV_PGRAPH_BPIXEL_DEPTH0         TEXTEQU         <2:2>
;  RW--V 
;  1757 : #define NV_PGRAPH_BPIXEL_DEPTH0_NOT_VALID                0x00000000 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH0_NOT_VALID               EQU             000000000h
;  RW--V 
;  1758 : #define NV_PGRAPH_BPIXEL_DEPTH0_VALID                    0x00000001 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH0_VALID           EQU             000000001h
;  RWXVF 
;  1759 : #define NV_PGRAPH_BPIXEL_DEPTH1_FMT                             5:4 /* RWXVF */
NV_PGRAPH_BPIXEL_DEPTH1_FMT             TEXTEQU         <5:4>
;  RW--V 
;  1760 : #define NV_PGRAPH_BPIXEL_DEPTH1_FMT_Y16_BITS             0x00000000 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH1_FMT_Y16_BITS            EQU             000000000h
;  RW--V 
;  1761 : #define NV_PGRAPH_BPIXEL_DEPTH1_FMT_8_BITS               0x00000001 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH1_FMT_8_BITS              EQU             000000001h
;  RW--V 
;  1762 : #define NV_PGRAPH_BPIXEL_DEPTH1_FMT_16_BITS              0x00000002 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH1_FMT_16_BITS             EQU             000000002h
;  RW--V 
;  1763 : #define NV_PGRAPH_BPIXEL_DEPTH1_FMT_32_BITS              0x00000003 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH1_FMT_32_BITS             EQU             000000003h
;  RWXVF 
;  1764 : #define NV_PGRAPH_BPIXEL_DEPTH1                                 6:6 /* RWXVF */
NV_PGRAPH_BPIXEL_DEPTH1         TEXTEQU         <6:6>
;  RW--V 
;  1765 : #define NV_PGRAPH_BPIXEL_DEPTH1_NOT_VALID                0x00000000 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH1_NOT_VALID               EQU             000000000h
;  RW--V 
;  1766 : #define NV_PGRAPH_BPIXEL_DEPTH1_VALID                    0x00000001 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH1_VALID           EQU             000000001h
;  RWXVF 
;  1767 : #define NV_PGRAPH_BPIXEL_DEPTH2_FMT                             9:8 /* RWXVF */
NV_PGRAPH_BPIXEL_DEPTH2_FMT             TEXTEQU         <9:8>
;  RW--V 
;  1768 : #define NV_PGRAPH_BPIXEL_DEPTH2_FMT_Y16_BITS             0x00000000 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH2_FMT_Y16_BITS            EQU             000000000h
;  RW--V 
;  1769 : #define NV_PGRAPH_BPIXEL_DEPTH2_FMT_8_BITS               0x00000001 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH2_FMT_8_BITS              EQU             000000001h
;  RW--V 
;  1770 : #define NV_PGRAPH_BPIXEL_DEPTH2_FMT_16_BITS              0x00000002 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH2_FMT_16_BITS             EQU             000000002h
;  RW--V 
;  1771 : #define NV_PGRAPH_BPIXEL_DEPTH2_FMT_32_BITS              0x00000003 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH2_FMT_32_BITS             EQU             000000003h
;  RWXVF 
;  1772 : #define NV_PGRAPH_BPIXEL_DEPTH2                               10:10 /* RWXVF */
NV_PGRAPH_BPIXEL_DEPTH2         TEXTEQU         <10:10>
;  RW--V 
;  1773 : #define NV_PGRAPH_BPIXEL_DEPTH2_NOT_VALID                0x00000000 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH2_NOT_VALID               EQU             000000000h
;  RW--V 
;  1774 : #define NV_PGRAPH_BPIXEL_DEPTH2_VALID                    0x00000001 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH2_VALID           EQU             000000001h
;  RWXVF 
;  1775 : #define NV_PGRAPH_BPIXEL_DEPTH3_FMT                           13:12 /* RWXVF */
NV_PGRAPH_BPIXEL_DEPTH3_FMT             TEXTEQU         <13:12>
;  RW--V 
;  1776 : #define NV_PGRAPH_BPIXEL_DEPTH3_FMT_Y16_BITS             0x00000000 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH3_FMT_Y16_BITS            EQU             000000000h
;  RW--V 
;  1777 : #define NV_PGRAPH_BPIXEL_DEPTH3_FMT_8_BITS               0x00000001 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH3_FMT_8_BITS              EQU             000000001h
;  RW--V 
;  1778 : #define NV_PGRAPH_BPIXEL_DEPTH3_FMT_16_BITS              0x00000002 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH3_FMT_16_BITS             EQU             000000002h
;  RW--V 
;  1779 : #define NV_PGRAPH_BPIXEL_DEPTH3_FMT_32_BITS              0x00000003 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH3_FMT_32_BITS             EQU             000000003h
;  RWXVF 
;  1780 : #define NV_PGRAPH_BPIXEL_DEPTH3                               14:14 /* RWXVF */
NV_PGRAPH_BPIXEL_DEPTH3         TEXTEQU         <14:14>
;  RW--V 
;  1781 : #define NV_PGRAPH_BPIXEL_DEPTH3_NOT_VALID                0x00000000 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH3_NOT_VALID               EQU             000000000h
;  RW--V 
;  1782 : #define NV_PGRAPH_BPIXEL_DEPTH3_VALID                    0x00000001 /* RW--V */
NV_PGRAPH_BPIXEL_DEPTH3_VALID           EQU             000000001h
;  RW-4R 
;  1783 : #define NV_PGRAPH_PATT_COLOR0_0                          0x00400600 /* RW-4R */
NV_PGRAPH_PATT_COLOR0_0         EQU             000400600h
;  RWXUF 
;  1784 : #define NV_PGRAPH_PATT_COLOR0_0_BLUE                            9:0 /* RWXUF */
NV_PGRAPH_PATT_COLOR0_0_BLUE            TEXTEQU         <9:0>
;  RWXUF 
;  1785 : #define NV_PGRAPH_PATT_COLOR0_0_GREEN                         19:10 /* RWXUF */
NV_PGRAPH_PATT_COLOR0_0_GREEN           TEXTEQU         <19:10>
;  RWXUF 
;  1786 : #define NV_PGRAPH_PATT_COLOR0_0_RED                           29:20 /* RWXUF */
NV_PGRAPH_PATT_COLOR0_0_RED             TEXTEQU         <29:20>
;  RW-4R 
;  1787 : #define NV_PGRAPH_PATT_COLOR0_1                          0x00400604 /* RW-4R */
NV_PGRAPH_PATT_COLOR0_1         EQU             000400604h
;  RWXUF 
;  1788 : #define NV_PGRAPH_PATT_COLOR0_1_ALPHA                           7:0 /* RWXUF */
NV_PGRAPH_PATT_COLOR0_1_ALPHA           TEXTEQU         <7:0>
;  RW-4R 
;  1789 : #define NV_PGRAPH_PATT_COLOR1_0                          0x00400608 /* RW-4R */
NV_PGRAPH_PATT_COLOR1_0         EQU             000400608h
;  RWXUF 
;  1790 : #define NV_PGRAPH_PATT_COLOR1_0_BLUE                            9:0 /* RWXUF */
NV_PGRAPH_PATT_COLOR1_0_BLUE            TEXTEQU         <9:0>
;  RWXUF 
;  1791 : #define NV_PGRAPH_PATT_COLOR1_0_GREEN                         19:10 /* RWXUF */
NV_PGRAPH_PATT_COLOR1_0_GREEN           TEXTEQU         <19:10>
;  RWXUF 
;  1792 : #define NV_PGRAPH_PATT_COLOR1_0_RED                           29:20 /* RWXUF */
NV_PGRAPH_PATT_COLOR1_0_RED             TEXTEQU         <29:20>
;  RW-4R 
;  1793 : #define NV_PGRAPH_PATT_COLOR1_1                          0x0040060C /* RW-4R */
NV_PGRAPH_PATT_COLOR1_1         EQU             00040060ch
;  RWXUF 
;  1794 : #define NV_PGRAPH_PATT_COLOR1_1_ALPHA                           7:0 /* RWXUF */
NV_PGRAPH_PATT_COLOR1_1_ALPHA           TEXTEQU         <7:0>
;  RW-4A 
;        
;  1795 : #define NV_PGRAPH_PATTERN(i)                     (0x00400610+(i)*4) /* RW-4A */
;  1796 : #define NV_PGRAPH_PATTERN__SIZE_1                                 2 /*       */
NV_PGRAPH_PATTERN__SIZE_1               EQU             2t
;  RWXVF 
;  1797 : #define NV_PGRAPH_PATTERN_BITMAP                               31:0 /* RWXVF */
NV_PGRAPH_PATTERN_BITMAP                TEXTEQU         <31:0>
;  RW-4R 
;  1798 : #define NV_PGRAPH_PATTERN_SHAPE                          0x00400618 /* RW-4R */
NV_PGRAPH_PATTERN_SHAPE         EQU             000400618h
;  RWXVF 
;  1799 : #define NV_PGRAPH_PATTERN_SHAPE_VALUE                           1:0 /* RWXVF */
NV_PGRAPH_PATTERN_SHAPE_VALUE           TEXTEQU         <1:0>
;  RW--V 
;  1800 : #define NV_PGRAPH_PATTERN_SHAPE_VALUE_8X8                0x00000000 /* RW--V */
NV_PGRAPH_PATTERN_SHAPE_VALUE_8X8               EQU             000000000h
;  RW--V 
;  1801 : #define NV_PGRAPH_PATTERN_SHAPE_VALUE_64X1               0x00000001 /* RW--V */
NV_PGRAPH_PATTERN_SHAPE_VALUE_64X1              EQU             000000001h
;  RW--V 
;  1802 : #define NV_PGRAPH_PATTERN_SHAPE_VALUE_1X64               0x00000002 /* RW--V */
NV_PGRAPH_PATTERN_SHAPE_VALUE_1X64              EQU             000000002h
;  RW-4R 
;  1803 : #define NV_PGRAPH_MONO_COLOR0                            0x0040061C /* RW-4R */
NV_PGRAPH_MONO_COLOR0           EQU             00040061ch
;  RWXUF 
;  1804 : #define NV_PGRAPH_MONO_COLOR0_BLUE                              9:0 /* RWXUF */
NV_PGRAPH_MONO_COLOR0_BLUE              TEXTEQU         <9:0>
;  RWXUF 
;  1805 : #define NV_PGRAPH_MONO_COLOR0_GREEN                           19:10 /* RWXUF */
NV_PGRAPH_MONO_COLOR0_GREEN             TEXTEQU         <19:10>
;  RWXUF 
;  1806 : #define NV_PGRAPH_MONO_COLOR0_RED                             29:20 /* RWXUF */
NV_PGRAPH_MONO_COLOR0_RED               TEXTEQU         <29:20>
;  RWXUF 
;  1807 : #define NV_PGRAPH_MONO_COLOR0_ALPHA                           30:30 /* RWXUF */
NV_PGRAPH_MONO_COLOR0_ALPHA             TEXTEQU         <30:30>
;  RW-4R 
;  1808 : #define NV_PGRAPH_ROP3                                   0x00400624 /* RW-4R */
NV_PGRAPH_ROP3          EQU             000400624h
;  RWXVF 
;  1809 : #define NV_PGRAPH_ROP3_VALUE                                    7:0 /* RWXVF */
NV_PGRAPH_ROP3_VALUE            TEXTEQU         <7:0>
;  RW-4R 
;  1810 : #define NV_PGRAPH_PLANE_MASK                             0x00400628 /* RW-4R */
NV_PGRAPH_PLANE_MASK            EQU             000400628h
;  RWXUF 
;  1811 : #define NV_PGRAPH_PLANE_MASK_BLUE                               9:0 /* RWXUF */
NV_PGRAPH_PLANE_MASK_BLUE               TEXTEQU         <9:0>
;  RWXUF 
;  1812 : #define NV_PGRAPH_PLANE_MASK_GREEN                            19:10 /* RWXUF */
NV_PGRAPH_PLANE_MASK_GREEN              TEXTEQU         <19:10>
;  RWXUF 
;  1813 : #define NV_PGRAPH_PLANE_MASK_RED                              29:20 /* RWXUF */
NV_PGRAPH_PLANE_MASK_RED                TEXTEQU         <29:20>
;  RWXUF 
;  1814 : #define NV_PGRAPH_PLANE_MASK_ALPHA                            30:30 /* RWXUF */
NV_PGRAPH_PLANE_MASK_ALPHA              TEXTEQU         <30:30>
;  RW-4R 
;  1815 : #define NV_PGRAPH_CHROMA                                 0x0040062C /* RW-4R */
NV_PGRAPH_CHROMA                EQU             00040062ch
;  RWXUF 
;  1816 : #define NV_PGRAPH_CHROMA_BLUE                                   9:0 /* RWXUF */
NV_PGRAPH_CHROMA_BLUE           TEXTEQU         <9:0>
;  RWXUF 
;  1817 : #define NV_PGRAPH_CHROMA_GREEN                                19:10 /* RWXUF */
NV_PGRAPH_CHROMA_GREEN          TEXTEQU         <19:10>
;  RWXUF 
;  1818 : #define NV_PGRAPH_CHROMA_RED                                  29:20 /* RWXUF */
NV_PGRAPH_CHROMA_RED            TEXTEQU         <29:20>
;  RWXUF 
;  1819 : #define NV_PGRAPH_CHROMA_ALPHA                                30:30 /* RWXUF */
NV_PGRAPH_CHROMA_ALPHA          TEXTEQU         <30:30>
;  RW-4R 
;  1820 : #define NV_PGRAPH_BETA                                   0x00400640 /* RW-4R */
NV_PGRAPH_BETA          EQU             000400640h
;  RWXUF 
;  1821 : #define NV_PGRAPH_BETA_VALUE_FRACTION                         30:23 /* RWXUF */
NV_PGRAPH_BETA_VALUE_FRACTION           TEXTEQU         <30:23>
;  RW-4R 
;  1822 : #define NV_PGRAPH_CONTROL_OUT                            0x00400644 /* RW-4R */
NV_PGRAPH_CONTROL_OUT           EQU             000400644h
;  -W-UF 
;  1823 : #define NV_PGRAPH_CONTROL_OUT_INTERPOLATOR                      1:0 /* -W-UF */
NV_PGRAPH_CONTROL_OUT_INTERPOLATOR              TEXTEQU         <1:0>
;  -W-UV 
;  1824 : #define NV_PGRAPH_CONTROL_OUT_INTERPOLATOR_ZOH           0x00000000 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_INTERPOLATOR_ZOH          EQU             000000000h
;  -W-UV 
;  1825 : #define NV_PGRAPH_CONTROL_OUT_INTERPOLATOR_ZOH_MS        0x00000001 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_INTERPOLATOR_ZOH_MS               EQU             000000001h
;  -W-UV 
;  1826 : #define NV_PGRAPH_CONTROL_OUT_INTERPOLATOR_FOH           0x00000002 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_INTERPOLATOR_FOH          EQU             000000002h
;  -W-UF 
;  1827 : #define NV_PGRAPH_CONTROL_OUT_WRAP_U                            5:4 /* -W-UF */
NV_PGRAPH_CONTROL_OUT_WRAP_U            TEXTEQU         <5:4>
;  -W-UV 
;  1828 : #define NV_PGRAPH_CONTROL_OUT_WRAP_U_CYLINDRICAL         0x00000000 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_WRAP_U_CYLINDRICAL                EQU             000000000h
;  -W-UV 
;  1829 : #define NV_PGRAPH_CONTROL_OUT_WRAP_U_WRAP                0x00000001 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_WRAP_U_WRAP               EQU             000000001h
;  -W-UV 
;  1830 : #define NV_PGRAPH_CONTROL_OUT_WRAP_U_MIRROR              0x00000002 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_WRAP_U_MIRROR             EQU             000000002h
;  -W-UV 
;  1831 : #define NV_PGRAPH_CONTROL_OUT_WRAP_U_CLAMP               0x00000003 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_WRAP_U_CLAMP              EQU             000000003h
;  -W-UF 
;  1832 : #define NV_PGRAPH_CONTROL_OUT_WRAP_V                            7:6 /* -W-UF */
NV_PGRAPH_CONTROL_OUT_WRAP_V            TEXTEQU         <7:6>
;  -W-UV 
;  1833 : #define NV_PGRAPH_CONTROL_OUT_WRAP_V_CYLINDRICAL         0x00000000 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_WRAP_V_CYLINDRICAL                EQU             000000000h
;  -W-UV 
;  1834 : #define NV_PGRAPH_CONTROL_OUT_WRAP_V_WRAP                0x00000001 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_WRAP_V_WRAP               EQU             000000001h
;  -W-UV 
;  1835 : #define NV_PGRAPH_CONTROL_OUT_WRAP_V_MIRROR              0x00000002 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_WRAP_V_MIRROR             EQU             000000002h
;  -W-UV 
;  1836 : #define NV_PGRAPH_CONTROL_OUT_WRAP_V_CLAMP               0x00000003 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_WRAP_V_CLAMP              EQU             000000003h
;  -W-UF 
;  1837 : #define NV_PGRAPH_CONTROL_OUT_COLOR_FORMAT                      8:8 /* -W-UF */
NV_PGRAPH_CONTROL_OUT_COLOR_FORMAT              TEXTEQU         <8:8>
;  -W-UV 
;  1838 : #define NV_PGRAPH_CONTROL_OUT_COLOR_FORMAT_LE_X8R8G8B8   0x00000000 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_COLOR_FORMAT_LE_X8R8G8B8          EQU             000000000h
;  -W-UV 
;  1839 : #define NV_PGRAPH_CONTROL_OUT_COLOR_FORMAT_LE_A8R8G8B8   0x00000001 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_COLOR_FORMAT_LE_A8R8G8B8          EQU             000000001h
;  -W-UF 
;  1840 : #define NV_PGRAPH_CONTROL_OUT_CULLING                         13:12 /* -W-UF */
NV_PGRAPH_CONTROL_OUT_CULLING           TEXTEQU         <13:12>
;  -W-UV 
;  1841 : #define NV_PGRAPH_CONTROL_OUT_CULLING_NONE               0x00000001 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_CULLING_NONE              EQU             000000001h
;  -W-UV 
;  1842 : #define NV_PGRAPH_CONTROL_OUT_CULLING_CLOCKWISE          0x00000002 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_CULLING_CLOCKWISE         EQU             000000002h
;  -W-UV 
;  1843 : #define NV_PGRAPH_CONTROL_OUT_CULLING_COUNTERCLOCKWISE   0x00000003 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_CULLING_COUNTERCLOCKWISE          EQU             000000003h
;  -W-UF 
;  1844 : #define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE                    19:16 /* -W-UF */
NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE              TEXTEQU         <19:16>
;  -W-UV 
;  1845 : #define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_FALSE         0x00000001 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_FALSE                EQU             000000001h
;  -W-UV 
;  1846 : #define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_LT            0x00000002 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_LT           EQU             000000002h
;  -W-UV 
;  1847 : #define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_EQ            0x00000003 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_EQ           EQU             000000003h
;  -W-UV 
;  1848 : #define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_LE            0x00000004 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_LE           EQU             000000004h
;  -W-UV 
;  1849 : #define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_GT            0x00000005 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_GT           EQU             000000005h
;  -W-UV 
;  1850 : #define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_NE            0x00000006 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_NE           EQU             000000006h
;  -W-UV 
;  1851 : #define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_GE            0x00000007 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_GE           EQU             000000007h
;  -W-UV 
;  1852 : #define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_TRUE          0x00000008 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_TRUE         EQU             000000008h
;  -W-UF 
;  1853 : #define NV_PGRAPH_CONTROL_OUT_ZETA_WRITE                      22:20 /* -W-UF */
NV_PGRAPH_CONTROL_OUT_ZETA_WRITE                TEXTEQU         <22:20>
;  -W-UV 
;  1854 : #define NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_NEVER           0x00000000 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_NEVER          EQU             000000000h
;  -W-UV 
;  1855 : #define NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_ALPHA           0x00000001 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_ALPHA          EQU             000000001h
;  -W-UV 
;  1856 : #define NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_ALPHA_ZETA      0x00000002 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_ALPHA_ZETA             EQU             000000002h
;  -W-UV 
;  1857 : #define NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_ZETA            0x00000003 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_ZETA           EQU             000000003h
;  -W-UV 
;  1858 : #define NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_ALWAYS          0x00000004 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_ALWAYS         EQU             000000004h
;  -W-UF 
;  1859 : #define NV_PGRAPH_CONTROL_OUT_COLOR_WRITE                     25:24 /* -W-UF */
NV_PGRAPH_CONTROL_OUT_COLOR_WRITE               TEXTEQU         <25:24>
;  -W-UV 
;  1860 : #define NV_PGRAPH_CONTROL_OUT_COLOR_WRITE_NEVER          0x00000000 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_COLOR_WRITE_NEVER         EQU             000000000h
;  -W-UV 
;  1861 : #define NV_PGRAPH_CONTROL_OUT_COLOR_WRITE_ALPHA          0x00000001 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_COLOR_WRITE_ALPHA         EQU             000000001h
;  -W-UV 
;  1862 : #define NV_PGRAPH_CONTROL_OUT_COLOR_WRITE_ALPHA_ZETA     0x00000002 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_COLOR_WRITE_ALPHA_ZETA            EQU             000000002h
;  -W-UF 
;  1863 : #define NV_PGRAPH_CONTROL_OUT_ROP                             28:28 /* -W-UF */
NV_PGRAPH_CONTROL_OUT_ROP               TEXTEQU         <28:28>
;  -W-UV 
;  1864 : #define NV_PGRAPH_CONTROL_OUT_ROP_BLEND_AND              0x00000000 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ROP_BLEND_AND             EQU             000000000h
;  -W-UV 
;  1865 : #define NV_PGRAPH_CONTROL_OUT_ROP_ADD_WITH_SATURATION    0x00000001 /* -W-UV */
NV_PGRAPH_CONTROL_OUT_ROP_ADD_WITH_SATURATION           EQU             000000001h
;  RW-4R 
;  1866 : #define NV_PGRAPH_DPRAM_INDEX                            0x00400648 /* RW-4R */
NV_PGRAPH_DPRAM_INDEX           EQU             000400648h
;  RWIVF 
;  1867 : #define NV_PGRAPH_DPRAM_INDEX_ADRS                              5:0 /* RWIVF */
NV_PGRAPH_DPRAM_INDEX_ADRS              TEXTEQU         <5:0>
;  RWI-V 
;  1868 : #define NV_PGRAPH_DPRAM_INDEX_ADRS_0                     0x00000000 /* RWI-V */
NV_PGRAPH_DPRAM_INDEX_ADRS_0            EQU             000000000h
;  RWIVF 
;  1869 : #define NV_PGRAPH_DPRAM_INDEX_SELECT                           10:8 /* RWIVF */
NV_PGRAPH_DPRAM_INDEX_SELECT            TEXTEQU         <10:8>
;  RWI-V 
;  1870 : #define NV_PGRAPH_DPRAM_INDEX_SELECT_ADRS_0              0x00000000 /* RWI-V */
NV_PGRAPH_DPRAM_INDEX_SELECT_ADRS_0             EQU             000000000h
;  RW--V 
;  1871 : #define NV_PGRAPH_DPRAM_INDEX_SELECT_ADRS_1              0x00000001 /* RW--V */
NV_PGRAPH_DPRAM_INDEX_SELECT_ADRS_1             EQU             000000001h
;  RW--V 
;  1872 : #define NV_PGRAPH_DPRAM_INDEX_SELECT_DATA_0              0x00000002 /* RW--V */
NV_PGRAPH_DPRAM_INDEX_SELECT_DATA_0             EQU             000000002h
;  RW--V 
;  1873 : #define NV_PGRAPH_DPRAM_INDEX_SELECT_DATA_1              0x00000003 /* RW--V */
NV_PGRAPH_DPRAM_INDEX_SELECT_DATA_1             EQU             000000003h
;  RW--V 
;  1874 : #define NV_PGRAPH_DPRAM_INDEX_SELECT_WE                  0x00000004 /* RW--V */
NV_PGRAPH_DPRAM_INDEX_SELECT_WE         EQU             000000004h
;  RW--V 
;  1875 : #define NV_PGRAPH_DPRAM_INDEX_SELECT_ALPHA               0x00000005 /* RW--V */
NV_PGRAPH_DPRAM_INDEX_SELECT_ALPHA              EQU             000000005h
;  RW-4R 
;  1876 : #define NV_PGRAPH_DPRAM_DATA                             0x0040064c /* RW-4R */
NV_PGRAPH_DPRAM_DATA            EQU             00040064ch
;  RWXVF 
;  1877 : #define NV_PGRAPH_DPRAM_DATA_VALUE                             31:0 /* RWXVF */
NV_PGRAPH_DPRAM_DATA_VALUE              TEXTEQU         <31:0>
;  RW-4R 
;  1878 : #define NV_PGRAPH_DPRAM_ADRS_0                           0x0040064c /* RW-4R */
NV_PGRAPH_DPRAM_ADRS_0          EQU             00040064ch
;        
;  1879 : #define NV_PGRAPH_DPRAM_ADRS_0__ALIAS_1        NV_PGRAPH_DPRAM_DATA /*       */
NV_PGRAPH_DPRAM_ADRS_0__ALIAS_1         EQU             00040064ch
;  RWXVF 
;  1880 : #define NV_PGRAPH_DPRAM_ADRS_0_VALUE                           19:0 /* RWXVF */
NV_PGRAPH_DPRAM_ADRS_0_VALUE            TEXTEQU         <19:0>
;  RW-4R 
;  1881 : #define NV_PGRAPH_DPRAM_ADRS_1                           0x0040064c /* RW-4R */
NV_PGRAPH_DPRAM_ADRS_1          EQU             00040064ch
;        
;  1882 : #define NV_PGRAPH_DPRAM_ADRS_1__ALIAS_1        NV_PGRAPH_DPRAM_DATA /*       */
NV_PGRAPH_DPRAM_ADRS_1__ALIAS_1         EQU             00040064ch
;  RWXVF 
;  1883 : #define NV_PGRAPH_DPRAM_ADRS_1_VALUE                           19:0 /* RWXVF */
NV_PGRAPH_DPRAM_ADRS_1_VALUE            TEXTEQU         <19:0>
;  RW-4R 
;  1884 : #define NV_PGRAPH_DPRAM_DATA_0                           0x0040064c /* RW-4R */
NV_PGRAPH_DPRAM_DATA_0          EQU             00040064ch
;        
;  1885 : #define NV_PGRAPH_DPRAM_DATA_0__ALIAS_1        NV_PGRAPH_DPRAM_DATA /*       */
NV_PGRAPH_DPRAM_DATA_0__ALIAS_1         EQU             00040064ch
;  RWXVF 
;  1886 : #define NV_PGRAPH_DPRAM_DATA_0_VALUE                           31:0 /* RWXVF */
NV_PGRAPH_DPRAM_DATA_0_VALUE            TEXTEQU         <31:0>
;  RW-4R 
;  1887 : #define NV_PGRAPH_DPRAM_DATA_1                           0x0040064c /* RW-4R */
NV_PGRAPH_DPRAM_DATA_1          EQU             00040064ch
;        
;  1888 : #define NV_PGRAPH_DPRAM_DATA_1__ALIAS_1        NV_PGRAPH_DPRAM_DATA /*       */
NV_PGRAPH_DPRAM_DATA_1__ALIAS_1         EQU             00040064ch
;  RWXVF 
;  1889 : #define NV_PGRAPH_DPRAM_DATA_1_VALUE                           31:0 /* RWXVF */
NV_PGRAPH_DPRAM_DATA_1_VALUE            TEXTEQU         <31:0>
;  RW-4R 
;  1890 : #define NV_PGRAPH_DPRAM_WE                               0x0040064c /* RW-4R */
NV_PGRAPH_DPRAM_WE              EQU             00040064ch
;        
;  1891 : #define NV_PGRAPH_DPRAM_WE__ALIAS_1            NV_PGRAPH_DPRAM_DATA /*       */
NV_PGRAPH_DPRAM_WE__ALIAS_1             EQU             00040064ch
;  RWXVF 
;  1892 : #define NV_PGRAPH_DPRAM_WE_VALUE                               31:0 /* RWXVF */
NV_PGRAPH_DPRAM_WE_VALUE                TEXTEQU         <31:0>
;  RW-4R 
;  1893 : #define NV_PGRAPH_DPRAM_ALPHA                            0x0040064c /* RW-4R */
NV_PGRAPH_DPRAM_ALPHA           EQU             00040064ch
;        
;  1894 : #define NV_PGRAPH_DPRAM_ALPHA__ALIAS_1         NV_PGRAPH_DPRAM_DATA /*       */
NV_PGRAPH_DPRAM_ALPHA__ALIAS_1          EQU             00040064ch
;  RWXVF 
;  1895 : #define NV_PGRAPH_DPRAM_ALPHA_VALUE                            31:0 /* RWXVF */
NV_PGRAPH_DPRAM_ALPHA_VALUE             TEXTEQU         <31:0>
;  RW-4A 
;        
;  1896 : #define NV_PGRAPH_ABS_X_RAM(i)                   (0x00400400+(i)*4) /* RW-4A */
;  1897 : #define NV_PGRAPH_ABS_X_RAM__SIZE_1                              32 /*       */
NV_PGRAPH_ABS_X_RAM__SIZE_1             EQU             32t
;  RWXUF 
;  1898 : #define NV_PGRAPH_ABS_X_RAM_VALUE                              31:0 /* RWXUF */
NV_PGRAPH_ABS_X_RAM_VALUE               TEXTEQU         <31:0>
;  R--4A 
;        
;  1899 : #define NV_PGRAPH_X_RAM_BPORT(i)                 (0x00400c00+(i)*4) /* R--4A */
;  1900 : #define NV_PGRAPH_X_RAM_BPORT__SIZE_1                            32 /*       */
NV_PGRAPH_X_RAM_BPORT__SIZE_1           EQU             32t
;  R--UF 
;  1901 : #define NV_PGRAPH_X_RAM_BPORT_VALUE                            31:0 /* R--UF */
NV_PGRAPH_X_RAM_BPORT_VALUE             TEXTEQU         <31:0>
;  RW-4A 
;        
;  1902 : #define NV_PGRAPH_ABS_Y_RAM(i)                   (0x00400480+(i)*4) /* RW-4A */
;  1903 : #define NV_PGRAPH_ABS_Y_RAM__SIZE_1                              32 /*       */
NV_PGRAPH_ABS_Y_RAM__SIZE_1             EQU             32t
;  RWXUF 
;  1904 : #define NV_PGRAPH_ABS_Y_RAM_VALUE                              31:0 /* RWXUF */
NV_PGRAPH_ABS_Y_RAM_VALUE               TEXTEQU         <31:0>
;  R--4A 
;        
;  1905 : #define NV_PGRAPH_Y_RAM_BPORT(i)                 (0x00400c80+(i)*4) /* R--4A */
;  1906 : #define NV_PGRAPH_Y_RAM_BPORT__SIZE_1                            32 /*       */
NV_PGRAPH_Y_RAM_BPORT__SIZE_1           EQU             32t
;  R--UF 
;  1907 : #define NV_PGRAPH_Y_RAM_BPORT_VALUE                            31:0 /* R--UF */
NV_PGRAPH_Y_RAM_BPORT_VALUE             TEXTEQU         <31:0>
;  RW-4R 
;  1908 : #define NV_PGRAPH_XY_LOGIC_MISC0                         0x00400514 /* RW-4R */
NV_PGRAPH_XY_LOGIC_MISC0                EQU             000400514h
;  RWIUF 
;  1909 : #define NV_PGRAPH_XY_LOGIC_MISC0_COUNTER                       17:0 /* RWIUF */
NV_PGRAPH_XY_LOGIC_MISC0_COUNTER                TEXTEQU         <17:0>
;  RWI-V 
;  1910 : #define NV_PGRAPH_XY_LOGIC_MISC0_COUNTER_0               0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC0_COUNTER_0              EQU             000000000h
;  RWXVF 
;  1911 : #define NV_PGRAPH_XY_LOGIC_MISC0_DIMENSION                    20:20 /* RWXVF */
NV_PGRAPH_XY_LOGIC_MISC0_DIMENSION              TEXTEQU         <20:20>
;  RW--V 
;  1912 : #define NV_PGRAPH_XY_LOGIC_MISC0_DIMENSION_NONZERO       0x00000000 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC0_DIMENSION_NONZERO              EQU             000000000h
;  RW--V 
;  1913 : #define NV_PGRAPH_XY_LOGIC_MISC0_DIMENSION_ZERO          0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC0_DIMENSION_ZERO         EQU             000000001h
;  RWIUF 
;  1914 : #define NV_PGRAPH_XY_LOGIC_MISC0_INDEX                        31:28 /* RWIUF */
NV_PGRAPH_XY_LOGIC_MISC0_INDEX          TEXTEQU         <31:28>
;  RWI-V 
;  1915 : #define NV_PGRAPH_XY_LOGIC_MISC0_INDEX_0                 0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC0_INDEX_0                EQU             000000000h
;  RW-4R 
;  1916 : #define NV_PGRAPH_XY_LOGIC_MISC1                         0x00400518 /* RW-4R */
NV_PGRAPH_XY_LOGIC_MISC1                EQU             000400518h
;  RWIVF 
;  1917 : #define NV_PGRAPH_XY_LOGIC_MISC1_INITIAL                        0:0 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_INITIAL                TEXTEQU         <0:0>
;  RWI-V 
;  1918 : #define NV_PGRAPH_XY_LOGIC_MISC1_INITIAL_NEEDED          0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_INITIAL_NEEDED         EQU             000000000h
;  RW--V 
;  1919 : #define NV_PGRAPH_XY_LOGIC_MISC1_INITIAL_DONE            0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC1_INITIAL_DONE           EQU             000000001h
;  RWIVF 
;  1920 : #define NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPX                      4:4 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPX              TEXTEQU         <4:4>
;  RWI-V 
;  1921 : #define NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPX_NOTNULL       0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPX_NOTNULL              EQU             000000000h
;  RW--V 
;  1922 : #define NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPX_NULL          0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPX_NULL         EQU             000000001h
;  RWIVF 
;  1923 : #define NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPY                      5:5 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPY              TEXTEQU         <5:5>
;  RWI-V 
;  1924 : #define NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPY_NOTNULL       0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPY_NOTNULL              EQU             000000000h
;  RW--V 
;  1925 : #define NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPY_NULL          0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPY_NULL         EQU             000000001h
;  RWIVF 
;  1926 : #define NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPX                      8:8 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPX              TEXTEQU         <8:8>
;  RWI-V 
;  1927 : #define NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPX_NOTNULL       0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPX_NOTNULL              EQU             000000000h
;  RW--V 
;  1928 : #define NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPX_NULL          0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPX_NULL         EQU             000000001h
;  RWIVF 
;  1929 : #define NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPY                      9:9 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPY              TEXTEQU         <9:9>
;  RWI-V 
;  1930 : #define NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPY_NOTNULL       0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPY_NOTNULL              EQU             000000000h
;  RW--V 
;  1931 : #define NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPY_NULL          0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPY_NULL         EQU             000000001h
;  RWIVF 
;  1932 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMIN                    12:12 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMIN              TEXTEQU         <12:12>
;  RWI-V 
;  1933 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMIN_CANVASMIN     0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMIN_CANVASMIN            EQU             000000000h
;  RW--V 
;  1934 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMIN_USERMIN       0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMIN_USERMIN              EQU             000000001h
;  RWIVF 
;  1935 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMAX                    13:13 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMAX              TEXTEQU         <13:13>
;  RWI-V 
;  1936 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMAX_CANVASMAX     0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMAX_CANVASMAX            EQU             000000000h
;  RW--V 
;  1937 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMAX_USERMAX       0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMAX_USERMAX              EQU             000000001h
;  RWIVF 
;  1938 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XIMAX                    14:14 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_XIMAX              TEXTEQU         <14:14>
;  RWI-V 
;  1939 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XIMAX_UCMAX         0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_XIMAX_UCMAX                EQU             000000000h
;  RW--V 
;  1940 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XIMAX_IMAGEMAX      0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_XIMAX_IMAGEMAX             EQU             000000001h
;  RWIVF 
;  1941 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMIN                    16:16 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMIN              TEXTEQU         <16:16>
;  RWI-V 
;  1942 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMIN_CANVASMIN     0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMIN_CANVASMIN            EQU             000000000h
;  RW--V 
;  1943 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMIN_USERMIN       0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMIN_USERMIN              EQU             000000001h
;  RWIVF 
;  1944 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMAX                    17:17 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMAX              TEXTEQU         <17:17>
;  RWI-V 
;  1945 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMAX_CANVASMAX     0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMAX_CANVASMAX            EQU             000000000h
;  RW--V 
;  1946 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMAX_USERMAX       0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMAX_USERMAX              EQU             000000001h
;  RWIVF 
;  1947 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YIMAX                    18:18 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_YIMAX              TEXTEQU         <18:18>
;  RWI-V 
;  1948 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YIMAX_UCMAX         0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_YIMAX_UCMAX                EQU             000000000h
;  RW--V 
;  1949 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YIMAX_IMAGEMAX      0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_YIMAX_IMAGEMAX             EQU             000000001h
;  RWIVF 
;  1950 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XXTRA                    20:20 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_XXTRA              TEXTEQU         <20:20>
;  RWI-V 
;  1951 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XXTRA_CLIPMAX       0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_XXTRA_CLIPMAX              EQU             000000000h
;  RW--V 
;  1952 : #define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XXTRA_IMAGEMAX      0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC1_SEL_XXTRA_IMAGEMAX             EQU             000000001h
;  RWIVF 
;  1953 : #define NV_PGRAPH_XY_LOGIC_MISC1_DUDX                         27:24 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_DUDX           TEXTEQU         <27:24>
;  RWI-V 
;  1954 : #define NV_PGRAPH_XY_LOGIC_MISC1_DUDX_VALUE              0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_DUDX_VALUE             EQU             000000000h
;  RW-4R 
;  1955 : #define NV_PGRAPH_XY_LOGIC_MISC2                         0x0040051C /* RW-4R */
NV_PGRAPH_XY_LOGIC_MISC2                EQU             00040051ch
;  RWIVF 
;  1956 : #define NV_PGRAPH_XY_LOGIC_MISC2_HANDOFF                        0:0 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC2_HANDOFF                TEXTEQU         <0:0>
;  RWI-V 
;  1957 : #define NV_PGRAPH_XY_LOGIC_MISC2_HANDOFF_DISABLE         0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC2_HANDOFF_DISABLE                EQU             000000000h
;  RW--V 
;  1958 : #define NV_PGRAPH_XY_LOGIC_MISC2_HANDOFF_ENABLE          0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC2_HANDOFF_ENABLE         EQU             000000001h
;  RWIVF 
;  1959 : #define NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPX                      4:4 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPX              TEXTEQU         <4:4>
;  RWI-V 
;  1960 : #define NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPX_NOTNULL       0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPX_NOTNULL              EQU             000000000h
;  RW--V 
;  1961 : #define NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPX_NULL          0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPX_NULL         EQU             000000001h
;  RWIVF 
;  1962 : #define NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPY                      5:5 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPY              TEXTEQU         <5:5>
;  RWI-V 
;  1963 : #define NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPY_NOTNULL       0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPY_NOTNULL              EQU             000000000h
;  RW--V 
;  1964 : #define NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPY_NULL          0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPY_NULL         EQU             000000001h
;  RWIVF 
;  1965 : #define NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPX                      8:8 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPX              TEXTEQU         <8:8>
;  RWI-V 
;  1966 : #define NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPX_NOTNULL       0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPX_NOTNULL              EQU             000000000h
;  RW--V 
;  1967 : #define NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPX_NULL          0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPX_NULL         EQU             000000001h
;  RWIVF 
;  1968 : #define NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPY                      9:9 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPY              TEXTEQU         <9:9>
;  RWI-V 
;  1969 : #define NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPY_NOTNULL       0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPY_NOTNULL              EQU             000000000h
;  RW--V 
;  1970 : #define NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPY_NULL          0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPY_NULL         EQU             000000001h
;  RWIVF 
;  1971 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMIN                    12:12 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMIN              TEXTEQU         <12:12>
;  RWI-V 
;  1972 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMIN_CANVASMIN     0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMIN_CANVASMIN            EQU             000000000h
;  RW--V 
;  1973 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMIN_USERMIN       0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMIN_USERMIN              EQU             000000001h
;  RWIVF 
;  1974 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMAX                    13:13 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMAX              TEXTEQU         <13:13>
;  RWI-V 
;  1975 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMAX_CANVASMAX     0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMAX_CANVASMAX            EQU             000000000h
;  RW--V 
;  1976 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMAX_USERMAX       0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMAX_USERMAX              EQU             000000001h
;  RWIVF 
;  1977 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XIMAX                    14:14 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_XIMAX              TEXTEQU         <14:14>
;  RWI-V 
;  1978 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XIMAX_UCMAX         0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_XIMAX_UCMAX                EQU             000000000h
;  RW--V 
;  1979 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XIMAX_IMAGEMAX      0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_XIMAX_IMAGEMAX             EQU             000000001h
;  RWIVF 
;  1980 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMIN                    16:16 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMIN              TEXTEQU         <16:16>
;  RWI-V 
;  1981 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMIN_CANVASMIN     0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMIN_CANVASMIN            EQU             000000000h
;  RW--V 
;  1982 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMIN_USERMIN       0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMIN_USERMIN              EQU             000000001h
;  RWIVF 
;  1983 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMAX                    17:17 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMAX              TEXTEQU         <17:17>
;  RWI-V 
;  1984 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMAX_CANVASMAX     0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMAX_CANVASMAX            EQU             000000000h
;  RW--V 
;  1985 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMAX_USERMAX       0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMAX_USERMAX              EQU             000000001h
;  RWIVF 
;  1986 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YIMAX                    18:18 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_YIMAX              TEXTEQU         <18:18>
;  RWI-V 
;  1987 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YIMAX_UCMAX         0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_YIMAX_UCMAX                EQU             000000000h
;  RW--V 
;  1988 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YIMAX_IMAGEMAX      0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_YIMAX_IMAGEMAX             EQU             000000001h
;  RWIVF 
;  1989 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XXTRA                    20:20 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_XXTRA              TEXTEQU         <20:20>
;  RWI-V 
;  1990 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XXTRA_CLIPMAX       0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_XXTRA_CLIPMAX              EQU             000000000h
;  RW--V 
;  1991 : #define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XXTRA_IMAGEMAX      0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC2_SEL_XXTRA_IMAGEMAX             EQU             000000001h
;  RWIVF 
;  1992 : #define NV_PGRAPH_XY_LOGIC_MISC1_DVDY                         27:24 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC1_DVDY           TEXTEQU         <27:24>
;  RWI-V 
;  1993 : #define NV_PGRAPH_XY_LOGIC_MISC1_DVDY_VALUE              0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC1_DVDY_VALUE             EQU             000000000h
;  RW-4R 
;  1994 : #define NV_PGRAPH_XY_LOGIC_MISC3                         0x00400520 /* RW-4R */
NV_PGRAPH_XY_LOGIC_MISC3                EQU             000400520h
;  RWXVF 
;  1995 : #define NV_PGRAPH_XY_LOGIC_MISC3_WDIMY_EQ_0                     0:0 /* RWXVF */
NV_PGRAPH_XY_LOGIC_MISC3_WDIMY_EQ_0             TEXTEQU         <0:0>
;  RW--V 
;  1996 : #define NV_PGRAPH_XY_LOGIC_MISC3_WDIMY_EQ_0_NULL         0x00000000 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC3_WDIMY_EQ_0_NULL                EQU             000000000h
;  RW--V 
;  1997 : #define NV_PGRAPH_XY_LOGIC_MISC3_WDIMY_EQ_0_TRUE         0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC3_WDIMY_EQ_0_TRUE                EQU             000000001h
;  RWXVF 
;  1998 : #define NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WDIMY                   4:4 /* RWXVF */
NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WDIMY           TEXTEQU         <4:4>
;  RW--V 
;  1999 : #define NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WDIMY_NULL       0x00000000 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WDIMY_NULL              EQU             000000000h
;  RW--V 
;  2000 : #define NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WDIMY_TRUE       0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WDIMY_TRUE              EQU             000000001h
;  RWIVF 
;  2001 : #define NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WX                      8:8 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WX              TEXTEQU         <8:8>
;  RWI-V 
;  2002 : #define NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WX_NULL          0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WX_NULL         EQU             000000000h
;  RW--V 
;  2003 : #define NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WX_TRUE          0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WX_TRUE         EQU             000000001h
;  RWIVF 
;  2004 : #define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_ALG                     12:12 /* RWIVF */
NV_PGRAPH_XY_LOGIC_MISC3_TEXT_ALG               TEXTEQU         <12:12>
;  RWI-V 
;  2005 : #define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_ALG_NULL           0x00000000 /* RWI-V */
NV_PGRAPH_XY_LOGIC_MISC3_TEXT_ALG_NULL          EQU             000000000h
;  RW--V 
;  2006 : #define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_ALG_TRUE           0x00000001 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC3_TEXT_ALG_TRUE          EQU             000000001h
;  RWXUF 
;  2007 : #define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_DIMX                    22:16 /* RWXUF */
NV_PGRAPH_XY_LOGIC_MISC3_TEXT_DIMX              TEXTEQU         <22:16>
;  RW--V 
;  2008 : #define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_DIMX_0             0x00000000 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC3_TEXT_DIMX_0            EQU             000000000h
;  RWXUF 
;  2009 : #define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_WDIMX                   30:24 /* RWXUF */
NV_PGRAPH_XY_LOGIC_MISC3_TEXT_WDIMX             TEXTEQU         <30:24>
;  RW--V 
;  2010 : #define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_WDIMX_0            0x00000000 /* RW--V */
NV_PGRAPH_XY_LOGIC_MISC3_TEXT_WDIMX_0           EQU             000000000h
;  RW-4R 
;  2011 : #define NV_PGRAPH_X_MISC                                 0x00400500 /* RW-4R */
NV_PGRAPH_X_MISC                EQU             000400500h
;  RWIVF 
;  2012 : #define NV_PGRAPH_X_MISC_BIT33_0                                0:0 /* RWIVF */
NV_PGRAPH_X_MISC_BIT33_0                TEXTEQU         <0:0>
;  RWI-V 
;  2013 : #define NV_PGRAPH_X_MISC_BIT33_0_0                       0x00000000 /* RWI-V */
NV_PGRAPH_X_MISC_BIT33_0_0              EQU             000000000h
;  RWIVF 
;  2014 : #define NV_PGRAPH_X_MISC_BIT33_1                                1:1 /* RWIVF */
NV_PGRAPH_X_MISC_BIT33_1                TEXTEQU         <1:1>
;  RWI-V 
;  2015 : #define NV_PGRAPH_X_MISC_BIT33_1_0                       0x00000000 /* RWI-V */
NV_PGRAPH_X_MISC_BIT33_1_0              EQU             000000000h
;  RWIVF 
;  2016 : #define NV_PGRAPH_X_MISC_BIT33_2                                2:2 /* RWIVF */
NV_PGRAPH_X_MISC_BIT33_2                TEXTEQU         <2:2>
;  RWI-V 
;  2017 : #define NV_PGRAPH_X_MISC_BIT33_2_0                       0x00000000 /* RWI-V */
NV_PGRAPH_X_MISC_BIT33_2_0              EQU             000000000h
;  RWIVF 
;  2018 : #define NV_PGRAPH_X_MISC_BIT33_3                                3:3 /* RWIVF */
NV_PGRAPH_X_MISC_BIT33_3                TEXTEQU         <3:3>
;  RWI-V 
;  2019 : #define NV_PGRAPH_X_MISC_BIT33_3_0                       0x00000000 /* RWI-V */
NV_PGRAPH_X_MISC_BIT33_3_0              EQU             000000000h
;  RWIVF 
;  2020 : #define NV_PGRAPH_X_MISC_RANGE_0                                4:4 /* RWIVF */
NV_PGRAPH_X_MISC_RANGE_0                TEXTEQU         <4:4>
;  RWI-V 
;  2021 : #define NV_PGRAPH_X_MISC_RANGE_0_0                       0x00000000 /* RWI-V */
NV_PGRAPH_X_MISC_RANGE_0_0              EQU             000000000h
;  RWIVF 
;  2022 : #define NV_PGRAPH_X_MISC_RANGE_1                                5:5 /* RWIVF */
NV_PGRAPH_X_MISC_RANGE_1                TEXTEQU         <5:5>
;  RWI-V 
;  2023 : #define NV_PGRAPH_X_MISC_RANGE_1_0                       0x00000000 /* RWI-V */
NV_PGRAPH_X_MISC_RANGE_1_0              EQU             000000000h
;  RWIVF 
;  2024 : #define NV_PGRAPH_X_MISC_RANGE_2                                6:6 /* RWIVF */
NV_PGRAPH_X_MISC_RANGE_2                TEXTEQU         <6:6>
;  RWI-V 
;  2025 : #define NV_PGRAPH_X_MISC_RANGE_2_0                       0x00000000 /* RWI-V */
NV_PGRAPH_X_MISC_RANGE_2_0              EQU             000000000h
;  RWIVF 
;  2026 : #define NV_PGRAPH_X_MISC_RANGE_3                                7:7 /* RWIVF */
NV_PGRAPH_X_MISC_RANGE_3                TEXTEQU         <7:7>
;  RWI-V 
;  2027 : #define NV_PGRAPH_X_MISC_RANGE_3_0                       0x00000000 /* RWI-V */
NV_PGRAPH_X_MISC_RANGE_3_0              EQU             000000000h
;  RWXVF 
;  2028 : #define NV_PGRAPH_X_MISC_ADDER_OUTPUT                         29:28 /* RWXVF */
NV_PGRAPH_X_MISC_ADDER_OUTPUT           TEXTEQU         <29:28>
;  RW--V 
;  2029 : #define NV_PGRAPH_X_MISC_ADDER_OUTPUT_EQ_0               0x00000000 /* RW--V */
NV_PGRAPH_X_MISC_ADDER_OUTPUT_EQ_0              EQU             000000000h
;  RW--V 
;  2030 : #define NV_PGRAPH_X_MISC_ADDER_OUTPUT_LT_0               0x00000001 /* RW--V */
NV_PGRAPH_X_MISC_ADDER_OUTPUT_LT_0              EQU             000000001h
;  RW--V 
;  2031 : #define NV_PGRAPH_X_MISC_ADDER_OUTPUT_GT_0               0x00000002 /* RW--V */
NV_PGRAPH_X_MISC_ADDER_OUTPUT_GT_0              EQU             000000002h
;  RW-4R 
;  2032 : #define NV_PGRAPH_Y_MISC                                 0x00400504 /* RW-4R */
NV_PGRAPH_Y_MISC                EQU             000400504h
;  RWIVF 
;  2033 : #define NV_PGRAPH_Y_MISC_BIT33_0                                0:0 /* RWIVF */
NV_PGRAPH_Y_MISC_BIT33_0                TEXTEQU         <0:0>
;  RWI-V 
;  2034 : #define NV_PGRAPH_Y_MISC_BIT33_0_0                       0x00000000 /* RWI-V */
NV_PGRAPH_Y_MISC_BIT33_0_0              EQU             000000000h
;  RWIVF 
;  2035 : #define NV_PGRAPH_Y_MISC_BIT33_1                                1:1 /* RWIVF */
NV_PGRAPH_Y_MISC_BIT33_1                TEXTEQU         <1:1>
;  RWI-V 
;  2036 : #define NV_PGRAPH_Y_MISC_BIT33_1_0                       0x00000000 /* RWI-V */
NV_PGRAPH_Y_MISC_BIT33_1_0              EQU             000000000h
;  RWIVF 
;  2037 : #define NV_PGRAPH_Y_MISC_BIT33_2                                2:2 /* RWIVF */
NV_PGRAPH_Y_MISC_BIT33_2                TEXTEQU         <2:2>
;  RWI-V 
;  2038 : #define NV_PGRAPH_Y_MISC_BIT33_2_0                       0x00000000 /* RWI-V */
NV_PGRAPH_Y_MISC_BIT33_2_0              EQU             000000000h
;  RWIVF 
;  2039 : #define NV_PGRAPH_Y_MISC_BIT33_3                                3:3 /* RWIVF */
NV_PGRAPH_Y_MISC_BIT33_3                TEXTEQU         <3:3>
;  RWI-V 
;  2040 : #define NV_PGRAPH_Y_MISC_BIT33_3_0                       0x00000000 /* RWI-V */
NV_PGRAPH_Y_MISC_BIT33_3_0              EQU             000000000h
;  RWIVF 
;  2041 : #define NV_PGRAPH_Y_MISC_RANGE_0                                4:4 /* RWIVF */
NV_PGRAPH_Y_MISC_RANGE_0                TEXTEQU         <4:4>
;  RWI-V 
;  2042 : #define NV_PGRAPH_Y_MISC_RANGE_0_0                       0x00000000 /* RWI-V */
NV_PGRAPH_Y_MISC_RANGE_0_0              EQU             000000000h
;  RWIVF 
;  2043 : #define NV_PGRAPH_Y_MISC_RANGE_1                                5:5 /* RWIVF */
NV_PGRAPH_Y_MISC_RANGE_1                TEXTEQU         <5:5>
;  RWI-V 
;  2044 : #define NV_PGRAPH_Y_MISC_RANGE_1_0                       0x00000000 /* RWI-V */
NV_PGRAPH_Y_MISC_RANGE_1_0              EQU             000000000h
;  RWIVF 
;  2045 : #define NV_PGRAPH_Y_MISC_RANGE_2                                6:6 /* RWIVF */
NV_PGRAPH_Y_MISC_RANGE_2                TEXTEQU         <6:6>
;  RWI-V 
;  2046 : #define NV_PGRAPH_Y_MISC_RANGE_2_0                       0x00000000 /* RWI-V */
NV_PGRAPH_Y_MISC_RANGE_2_0              EQU             000000000h
;  RWIVF 
;  2047 : #define NV_PGRAPH_Y_MISC_RANGE_3                                7:7 /* RWIVF */
NV_PGRAPH_Y_MISC_RANGE_3                TEXTEQU         <7:7>
;  RWI-V 
;  2048 : #define NV_PGRAPH_Y_MISC_RANGE_3_0                       0x00000000 /* RWI-V */
NV_PGRAPH_Y_MISC_RANGE_3_0              EQU             000000000h
;  RWXVF 
;  2049 : #define NV_PGRAPH_Y_MISC_ADDER_OUTPUT                         29:28 /* RWXVF */
NV_PGRAPH_Y_MISC_ADDER_OUTPUT           TEXTEQU         <29:28>
;  RW--V 
;  2050 : #define NV_PGRAPH_Y_MISC_ADDER_OUTPUT_EQ_0               0x00000000 /* RW--V */
NV_PGRAPH_Y_MISC_ADDER_OUTPUT_EQ_0              EQU             000000000h
;  RW--V 
;  2051 : #define NV_PGRAPH_Y_MISC_ADDER_OUTPUT_LT_0               0x00000001 /* RW--V */
NV_PGRAPH_Y_MISC_ADDER_OUTPUT_LT_0              EQU             000000001h
;  RW--V 
;  2052 : #define NV_PGRAPH_Y_MISC_ADDER_OUTPUT_GT_0               0x00000002 /* RW--V */
NV_PGRAPH_Y_MISC_ADDER_OUTPUT_GT_0              EQU             000000002h
;  RW-4R 
;  2053 : #define NV_PGRAPH_ABS_UCLIP_XMIN                         0x0040053C /* RW-4R */
NV_PGRAPH_ABS_UCLIP_XMIN                EQU             00040053ch
;  RWXSF 
;  2054 : #define NV_PGRAPH_ABS_UCLIP_XMIN_VALUE                         17:0 /* RWXSF */
NV_PGRAPH_ABS_UCLIP_XMIN_VALUE          TEXTEQU         <17:0>
;  RW-4R 
;  2055 : #define NV_PGRAPH_ABS_UCLIP_XMAX                         0x00400544 /* RW-4R */
NV_PGRAPH_ABS_UCLIP_XMAX                EQU             000400544h
;  RWXSF 
;  2056 : #define NV_PGRAPH_ABS_UCLIP_XMAX_VALUE                         17:0 /* RWXSF */
NV_PGRAPH_ABS_UCLIP_XMAX_VALUE          TEXTEQU         <17:0>
;  RW-4R 
;  2057 : #define NV_PGRAPH_ABS_UCLIP_YMIN                         0x00400540 /* RW-4R */
NV_PGRAPH_ABS_UCLIP_YMIN                EQU             000400540h
;  RWXSF 
;  2058 : #define NV_PGRAPH_ABS_UCLIP_YMIN_VALUE                         17:0 /* RWXSF */
NV_PGRAPH_ABS_UCLIP_YMIN_VALUE          TEXTEQU         <17:0>
;  RW-4R 
;  2059 : #define NV_PGRAPH_ABS_UCLIP_YMAX                         0x00400548 /* RW-4R */
NV_PGRAPH_ABS_UCLIP_YMAX                EQU             000400548h
;  RWXSF 
;  2060 : #define NV_PGRAPH_ABS_UCLIP_YMAX_VALUE                         17:0 /* RWXSF */
NV_PGRAPH_ABS_UCLIP_YMAX_VALUE          TEXTEQU         <17:0>
;  RW-4R 
;  2061 : #define NV_PGRAPH_ABS_UCLIPA_XMIN                        0x00400560 /* RW-4R */
NV_PGRAPH_ABS_UCLIPA_XMIN               EQU             000400560h
;  RWXSF 
;  2062 : #define NV_PGRAPH_ABS_UCLIPA_XMIN_VALUE                        17:0 /* RWXSF */
NV_PGRAPH_ABS_UCLIPA_XMIN_VALUE         TEXTEQU         <17:0>
;  RW-4R 
;  2063 : #define NV_PGRAPH_ABS_UCLIPA_XMAX                        0x00400568 /* RW-4R */
NV_PGRAPH_ABS_UCLIPA_XMAX               EQU             000400568h
;  RWXSF 
;  2064 : #define NV_PGRAPH_ABS_UCLIPA_XMAX_VALUE                        17:0 /* RWXSF */
NV_PGRAPH_ABS_UCLIPA_XMAX_VALUE         TEXTEQU         <17:0>
;  RW-4R 
;  2065 : #define NV_PGRAPH_ABS_UCLIPA_YMIN                        0x00400564 /* RW-4R */
NV_PGRAPH_ABS_UCLIPA_YMIN               EQU             000400564h
;  RWXSF 
;  2066 : #define NV_PGRAPH_ABS_UCLIPA_YMIN_VALUE                        17:0 /* RWXSF */
NV_PGRAPH_ABS_UCLIPA_YMIN_VALUE         TEXTEQU         <17:0>
;  RW-4R 
;  2067 : #define NV_PGRAPH_ABS_UCLIPA_YMAX                        0x0040056C /* RW-4R */
NV_PGRAPH_ABS_UCLIPA_YMAX               EQU             00040056ch
;  RWXSF 
;  2068 : #define NV_PGRAPH_ABS_UCLIPA_YMAX_VALUE                        17:0 /* RWXSF */
NV_PGRAPH_ABS_UCLIPA_YMAX_VALUE         TEXTEQU         <17:0>
;  RW-4R 
;  2069 : #define NV_PGRAPH_SOURCE_COLOR                           0x0040050C /* RW-4R */
NV_PGRAPH_SOURCE_COLOR          EQU             00040050ch
;  RWXVF 
;  2070 : #define NV_PGRAPH_SOURCE_COLOR_VALUE                           31:0 /* RWXVF */
NV_PGRAPH_SOURCE_COLOR_VALUE            TEXTEQU         <31:0>
;  RW-4R 
;  2071 : #define NV_PGRAPH_EXCEPTIONS                             0x00400508 /* RW-4R */
NV_PGRAPH_EXCEPTIONS            EQU             000400508h
;  RWIVF 
;  2072 : #define NV_PGRAPH_EXCEPTIONS_VALID                             27:0 /* RWIVF */
NV_PGRAPH_EXCEPTIONS_VALID              TEXTEQU         <27:0>
;  RWI-V 
;  2073 : #define NV_PGRAPH_EXCEPTIONS_VALID_0                     0x00000000 /* RWI-V */
NV_PGRAPH_EXCEPTIONS_VALID_0            EQU             000000000h
;  RWIVF 
;  2074 : #define NV_PGRAPH_EXCEPTIONS_CLIP_MIN                         28:28 /* RWIVF */
NV_PGRAPH_EXCEPTIONS_CLIP_MIN           TEXTEQU         <28:28>
;  RWI-V 
;  2075 : #define NV_PGRAPH_EXCEPTIONS_CLIP_MIN_NO_ERROR           0x00000000 /* RWI-V */
NV_PGRAPH_EXCEPTIONS_CLIP_MIN_NO_ERROR          EQU             000000000h
;  RW--V 
;  2076 : #define NV_PGRAPH_EXCEPTIONS_CLIP_MIN_ONLY               0x00000001 /* RW--V */
NV_PGRAPH_EXCEPTIONS_CLIP_MIN_ONLY              EQU             000000001h
;  RWIVF 
;  2077 : #define NV_PGRAPH_EXCEPTIONS_CLIPA_MIN                        29:29 /* RWIVF */
NV_PGRAPH_EXCEPTIONS_CLIPA_MIN          TEXTEQU         <29:29>
;  RWI-V 
;  2078 : #define NV_PGRAPH_EXCEPTIONS_CLIPA_MIN_NO_ERROR          0x00000000 /* RWI-V */
NV_PGRAPH_EXCEPTIONS_CLIPA_MIN_NO_ERROR         EQU             000000000h
;  RW--V 
;  2079 : #define NV_PGRAPH_EXCEPTIONS_CLIPA_MIN_ONLY              0x00000001 /* RW--V */
NV_PGRAPH_EXCEPTIONS_CLIPA_MIN_ONLY             EQU             000000001h
;  RWIVF 
;  2080 : #define NV_PGRAPH_EXCEPTIONS_CLIP_MAX                         30:30 /* RWIVF */
NV_PGRAPH_EXCEPTIONS_CLIP_MAX           TEXTEQU         <30:30>
;  RWI-V 
;  2081 : #define NV_PGRAPH_EXCEPTIONS_CLIP_MAX_NO_ERROR           0x00000000 /* RWI-V */
NV_PGRAPH_EXCEPTIONS_CLIP_MAX_NO_ERROR          EQU             000000000h
;  RW--V 
;  2082 : #define NV_PGRAPH_EXCEPTIONS_CLIP_MAX_ONLY               0x00000001 /* RW--V */
NV_PGRAPH_EXCEPTIONS_CLIP_MAX_ONLY              EQU             000000001h
;  RWIVF 
;  2083 : #define NV_PGRAPH_EXCEPTIONS_CLIPA_MAX                        31:31 /* RWIVF */
NV_PGRAPH_EXCEPTIONS_CLIPA_MAX          TEXTEQU         <31:31>
;  RWI-V 
;  2084 : #define NV_PGRAPH_EXCEPTIONS_CLIPA_MAX_NO_ERROR          0x00000000 /* RWI-V */
NV_PGRAPH_EXCEPTIONS_CLIPA_MAX_NO_ERROR         EQU             000000000h
;  RW--V 
;  2085 : #define NV_PGRAPH_EXCEPTIONS_CLIPA_MAX_ONLY              0x00000001 /* RW--V */
NV_PGRAPH_EXCEPTIONS_CLIPA_MAX_ONLY             EQU             000000001h
;  RW-4R 
;  2086 : #define NV_PGRAPH_ABS_ICLIP_XMAX                         0x00400534 /* RW-4R */
NV_PGRAPH_ABS_ICLIP_XMAX                EQU             000400534h
;  RWXSF 
;  2087 : #define NV_PGRAPH_ABS_ICLIP_XMAX_VALUE                         17:0 /* RWXSF */
NV_PGRAPH_ABS_ICLIP_XMAX_VALUE          TEXTEQU         <17:0>
;  RW-4R 
;  2088 : #define NV_PGRAPH_ABS_ICLIP_YMAX                         0x00400538 /* RW-4R */
NV_PGRAPH_ABS_ICLIP_YMAX                EQU             000400538h
;  RWXSF 
;  2089 : #define NV_PGRAPH_ABS_ICLIP_YMAX_VALUE                         17:0 /* RWXSF */
NV_PGRAPH_ABS_ICLIP_YMAX_VALUE          TEXTEQU         <17:0>
;  RW-4R 
;  2090 : #define NV_PGRAPH_CLIPX_0                                0x00400524 /* RW-4R */
NV_PGRAPH_CLIPX_0               EQU             000400524h
;  RWIVF 
;  2091 : #define NV_PGRAPH_CLIPX_0_CLIP0_MIN                             1:0 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP0_MIN             TEXTEQU         <1:0>
;  RW--V 
;  2092 : #define NV_PGRAPH_CLIPX_0_CLIP0_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP0_MIN_GT          EQU             000000000h
;  RWI-V 
;  2093 : #define NV_PGRAPH_CLIPX_0_CLIP0_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP0_MIN_LT          EQU             000000001h
;  RW--V 
;  2094 : #define NV_PGRAPH_CLIPX_0_CLIP0_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP0_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2095 : #define NV_PGRAPH_CLIPX_0_CLIP0_MAX                             3:2 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP0_MAX             TEXTEQU         <3:2>
;  RW--V 
;  2096 : #define NV_PGRAPH_CLIPX_0_CLIP0_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP0_MAX_LT          EQU             000000000h
;  RWI-V 
;  2097 : #define NV_PGRAPH_CLIPX_0_CLIP0_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP0_MAX_GT          EQU             000000001h
;  RW--V 
;  2098 : #define NV_PGRAPH_CLIPX_0_CLIP0_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP0_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2099 : #define NV_PGRAPH_CLIPX_0_CLIP1_MIN                             5:4 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP1_MIN             TEXTEQU         <5:4>
;  RW--V 
;  2100 : #define NV_PGRAPH_CLIPX_0_CLIP1_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP1_MIN_GT          EQU             000000000h
;  RWI-V 
;  2101 : #define NV_PGRAPH_CLIPX_0_CLIP1_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP1_MIN_LT          EQU             000000001h
;  RW--V 
;  2102 : #define NV_PGRAPH_CLIPX_0_CLIP1_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP1_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2103 : #define NV_PGRAPH_CLIPX_0_CLIP1_MAX                             7:6 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP1_MAX             TEXTEQU         <7:6>
;  RW--V 
;  2104 : #define NV_PGRAPH_CLIPX_0_CLIP1_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP1_MAX_LT          EQU             000000000h
;  RWI-V 
;  2105 : #define NV_PGRAPH_CLIPX_0_CLIP1_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP1_MAX_GT          EQU             000000001h
;  RW--V 
;  2106 : #define NV_PGRAPH_CLIPX_0_CLIP1_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP1_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2107 : #define NV_PGRAPH_CLIPX_0_CLIP2_MIN                             9:8 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP2_MIN             TEXTEQU         <9:8>
;  RW--V 
;  2108 : #define NV_PGRAPH_CLIPX_0_CLIP2_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP2_MIN_GT          EQU             000000000h
;  RWI-V 
;  2109 : #define NV_PGRAPH_CLIPX_0_CLIP2_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP2_MIN_LT          EQU             000000001h
;  RW--V 
;  2110 : #define NV_PGRAPH_CLIPX_0_CLIP2_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP2_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2111 : #define NV_PGRAPH_CLIPX_0_CLIP2_MAX                           11:10 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP2_MAX             TEXTEQU         <11:10>
;  RW--V 
;  2112 : #define NV_PGRAPH_CLIPX_0_CLIP2_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP2_MAX_LT          EQU             000000000h
;  RWI-V 
;  2113 : #define NV_PGRAPH_CLIPX_0_CLIP2_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP2_MAX_GT          EQU             000000001h
;  RW--V 
;  2114 : #define NV_PGRAPH_CLIPX_0_CLIP2_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP2_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2115 : #define NV_PGRAPH_CLIPX_0_CLIP3_MIN                           13:12 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP3_MIN             TEXTEQU         <13:12>
;  RW--V 
;  2116 : #define NV_PGRAPH_CLIPX_0_CLIP3_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP3_MIN_GT          EQU             000000000h
;  RWI-V 
;  2117 : #define NV_PGRAPH_CLIPX_0_CLIP3_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP3_MIN_LT          EQU             000000001h
;  RW--V 
;  2118 : #define NV_PGRAPH_CLIPX_0_CLIP3_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP3_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2119 : #define NV_PGRAPH_CLIPX_0_CLIP3_MAX                           15:14 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP3_MAX             TEXTEQU         <15:14>
;  RW--V 
;  2120 : #define NV_PGRAPH_CLIPX_0_CLIP3_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP3_MAX_LT          EQU             000000000h
;  RWI-V 
;  2121 : #define NV_PGRAPH_CLIPX_0_CLIP3_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP3_MAX_GT          EQU             000000001h
;  RW--V 
;  2122 : #define NV_PGRAPH_CLIPX_0_CLIP3_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP3_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2123 : #define NV_PGRAPH_CLIPX_0_CLIP4_MIN                           17:16 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP4_MIN             TEXTEQU         <17:16>
;  RW--V 
;  2124 : #define NV_PGRAPH_CLIPX_0_CLIP4_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP4_MIN_GT          EQU             000000000h
;  RWI-V 
;  2125 : #define NV_PGRAPH_CLIPX_0_CLIP4_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP4_MIN_LT          EQU             000000001h
;  RW--V 
;  2126 : #define NV_PGRAPH_CLIPX_0_CLIP4_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP4_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2127 : #define NV_PGRAPH_CLIPX_0_CLIP4_MAX                           19:18 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP4_MAX             TEXTEQU         <19:18>
;  RW--V 
;  2128 : #define NV_PGRAPH_CLIPX_0_CLIP4_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP4_MAX_LT          EQU             000000000h
;  RWI-V 
;  2129 : #define NV_PGRAPH_CLIPX_0_CLIP4_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP4_MAX_GT          EQU             000000001h
;  RW--V 
;  2130 : #define NV_PGRAPH_CLIPX_0_CLIP4_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP4_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2131 : #define NV_PGRAPH_CLIPX_0_CLIP5_MIN                           21:20 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP5_MIN             TEXTEQU         <21:20>
;  RW--V 
;  2132 : #define NV_PGRAPH_CLIPX_0_CLIP5_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP5_MIN_GT          EQU             000000000h
;  RWI-V 
;  2133 : #define NV_PGRAPH_CLIPX_0_CLIP5_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP5_MIN_LT          EQU             000000001h
;  RW--V 
;  2134 : #define NV_PGRAPH_CLIPX_0_CLIP5_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP5_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2135 : #define NV_PGRAPH_CLIPX_0_CLIP5_MAX                           23:22 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP5_MAX             TEXTEQU         <23:22>
;  RW--V 
;  2136 : #define NV_PGRAPH_CLIPX_0_CLIP5_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP5_MAX_LT          EQU             000000000h
;  RWI-V 
;  2137 : #define NV_PGRAPH_CLIPX_0_CLIP5_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP5_MAX_GT          EQU             000000001h
;  RW--V 
;  2138 : #define NV_PGRAPH_CLIPX_0_CLIP5_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP5_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2139 : #define NV_PGRAPH_CLIPX_0_CLIP6_MIN                           25:24 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP6_MIN             TEXTEQU         <25:24>
;  RW--V 
;  2140 : #define NV_PGRAPH_CLIPX_0_CLIP6_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP6_MIN_GT          EQU             000000000h
;  RWI-V 
;  2141 : #define NV_PGRAPH_CLIPX_0_CLIP6_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP6_MIN_LT          EQU             000000001h
;  RW--V 
;  2142 : #define NV_PGRAPH_CLIPX_0_CLIP6_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP6_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2143 : #define NV_PGRAPH_CLIPX_0_CLIP6_MAX                           27:26 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP6_MAX             TEXTEQU         <27:26>
;  RW--V 
;  2144 : #define NV_PGRAPH_CLIPX_0_CLIP6_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP6_MAX_LT          EQU             000000000h
;  RWI-V 
;  2145 : #define NV_PGRAPH_CLIPX_0_CLIP6_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP6_MAX_GT          EQU             000000001h
;  RW--V 
;  2146 : #define NV_PGRAPH_CLIPX_0_CLIP6_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP6_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2147 : #define NV_PGRAPH_CLIPX_0_CLIP7_MIN                           29:28 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP7_MIN             TEXTEQU         <29:28>
;  RW--V 
;  2148 : #define NV_PGRAPH_CLIPX_0_CLIP7_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP7_MIN_GT          EQU             000000000h
;  RWI-V 
;  2149 : #define NV_PGRAPH_CLIPX_0_CLIP7_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP7_MIN_LT          EQU             000000001h
;  RW--V 
;  2150 : #define NV_PGRAPH_CLIPX_0_CLIP7_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP7_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2151 : #define NV_PGRAPH_CLIPX_0_CLIP7_MAX                           31:30 /* RWIVF */
NV_PGRAPH_CLIPX_0_CLIP7_MAX             TEXTEQU         <31:30>
;  RW--V 
;  2152 : #define NV_PGRAPH_CLIPX_0_CLIP7_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP7_MAX_LT          EQU             000000000h
;  RWI-V 
;  2153 : #define NV_PGRAPH_CLIPX_0_CLIP7_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_0_CLIP7_MAX_GT          EQU             000000001h
;  RW--V 
;  2154 : #define NV_PGRAPH_CLIPX_0_CLIP7_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_0_CLIP7_MAX_EQ          EQU             000000002h
;  RW-4R 
;  2155 : #define NV_PGRAPH_CLIPX_1                                0x00400528 /* RW-4R */
NV_PGRAPH_CLIPX_1               EQU             000400528h
;  RWIVF 
;  2156 : #define NV_PGRAPH_CLIPX_1_CLIP8_MIN                             1:0 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP8_MIN             TEXTEQU         <1:0>
;  RW--V 
;  2157 : #define NV_PGRAPH_CLIPX_1_CLIP8_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP8_MIN_GT          EQU             000000000h
;  RWI-V 
;  2158 : #define NV_PGRAPH_CLIPX_1_CLIP8_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP8_MIN_LT          EQU             000000001h
;  RW--V 
;  2159 : #define NV_PGRAPH_CLIPX_1_CLIP8_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP8_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2160 : #define NV_PGRAPH_CLIPX_1_CLIP8_MAX                             3:2 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP8_MAX             TEXTEQU         <3:2>
;  RW--V 
;  2161 : #define NV_PGRAPH_CLIPX_1_CLIP8_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP8_MAX_LT          EQU             000000000h
;  RWI-V 
;  2162 : #define NV_PGRAPH_CLIPX_1_CLIP8_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP8_MAX_GT          EQU             000000001h
;  RW--V 
;  2163 : #define NV_PGRAPH_CLIPX_1_CLIP8_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP8_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2164 : #define NV_PGRAPH_CLIPX_1_CLIP9_MIN                             5:4 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP9_MIN             TEXTEQU         <5:4>
;  RW--V 
;  2165 : #define NV_PGRAPH_CLIPX_1_CLIP9_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP9_MIN_GT          EQU             000000000h
;  RWI-V 
;  2166 : #define NV_PGRAPH_CLIPX_1_CLIP9_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP9_MIN_LT          EQU             000000001h
;  RW--V 
;  2167 : #define NV_PGRAPH_CLIPX_1_CLIP9_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP9_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2168 : #define NV_PGRAPH_CLIPX_1_CLIP9_MAX                             7:6 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP9_MAX             TEXTEQU         <7:6>
;  RW--V 
;  2169 : #define NV_PGRAPH_CLIPX_1_CLIP9_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP9_MAX_LT          EQU             000000000h
;  RWI-V 
;  2170 : #define NV_PGRAPH_CLIPX_1_CLIP9_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP9_MAX_GT          EQU             000000001h
;  RW--V 
;  2171 : #define NV_PGRAPH_CLIPX_1_CLIP9_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP9_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2172 : #define NV_PGRAPH_CLIPX_1_CLIP10_MIN                            9:8 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP10_MIN            TEXTEQU         <9:8>
;  RW--V 
;  2173 : #define NV_PGRAPH_CLIPX_1_CLIP10_MIN_GT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP10_MIN_GT         EQU             000000000h
;  RWI-V 
;  2174 : #define NV_PGRAPH_CLIPX_1_CLIP10_MIN_LT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP10_MIN_LT         EQU             000000001h
;  RW--V 
;  2175 : #define NV_PGRAPH_CLIPX_1_CLIP10_MIN_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP10_MIN_EQ         EQU             000000002h
;  RWIVF 
;  2176 : #define NV_PGRAPH_CLIPX_1_CLIP10_MAX                          11:10 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP10_MAX            TEXTEQU         <11:10>
;  RW--V 
;  2177 : #define NV_PGRAPH_CLIPX_1_CLIP10_MAX_LT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP10_MAX_LT         EQU             000000000h
;  RWI-V 
;  2178 : #define NV_PGRAPH_CLIPX_1_CLIP10_MAX_GT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP10_MAX_GT         EQU             000000001h
;  RW--V 
;  2179 : #define NV_PGRAPH_CLIPX_1_CLIP10_MAX_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP10_MAX_EQ         EQU             000000002h
;  RWIVF 
;  2180 : #define NV_PGRAPH_CLIPX_1_CLIP11_MIN                          13:12 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP11_MIN            TEXTEQU         <13:12>
;  RW--V 
;  2181 : #define NV_PGRAPH_CLIPX_1_CLIP11_MIN_GT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP11_MIN_GT         EQU             000000000h
;  RWI-V 
;  2182 : #define NV_PGRAPH_CLIPX_1_CLIP11_MIN_LT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP11_MIN_LT         EQU             000000001h
;  RW--V 
;  2183 : #define NV_PGRAPH_CLIPX_1_CLIP11MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP11MIN_EQ          EQU             000000002h
;  RWIVF 
;  2184 : #define NV_PGRAPH_CLIPX_1_CLIP11_MAX                          15:14 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP11_MAX            TEXTEQU         <15:14>
;  RW--V 
;  2185 : #define NV_PGRAPH_CLIPX_1_CLIP11_MAX_LT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP11_MAX_LT         EQU             000000000h
;  RWI-V 
;  2186 : #define NV_PGRAPH_CLIPX_1_CLIP11_MAX_GT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP11_MAX_GT         EQU             000000001h
;  RW--V 
;  2187 : #define NV_PGRAPH_CLIPX_1_CLIP11_MAX_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP11_MAX_EQ         EQU             000000002h
;  RWIVF 
;  2188 : #define NV_PGRAPH_CLIPX_1_CLIP12_MIN                          17:16 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP12_MIN            TEXTEQU         <17:16>
;  RW--V 
;  2189 : #define NV_PGRAPH_CLIPX_1_CLIP12_MIN_GT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP12_MIN_GT         EQU             000000000h
;  RWI-V 
;  2190 : #define NV_PGRAPH_CLIPX_1_CLIP12_MIN_LT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP12_MIN_LT         EQU             000000001h
;  RW--V 
;  2191 : #define NV_PGRAPH_CLIPX_1_CLIP12_MIN_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP12_MIN_EQ         EQU             000000002h
;  RWIVF 
;  2192 : #define NV_PGRAPH_CLIPX_1_CLIP12_MAX                          19:18 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP12_MAX            TEXTEQU         <19:18>
;  RW--V 
;  2193 : #define NV_PGRAPH_CLIPX_1_CLIP12_MAX_LT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP12_MAX_LT         EQU             000000000h
;  RWI-V 
;  2194 : #define NV_PGRAPH_CLIPX_1_CLIP12_MAX_GT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP12_MAX_GT         EQU             000000001h
;  RW--V 
;  2195 : #define NV_PGRAPH_CLIPX_1_CLIP12_MAX_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP12_MAX_EQ         EQU             000000002h
;  RWIVF 
;  2196 : #define NV_PGRAPH_CLIPX_1_CLIP13_MIN                          21:20 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP13_MIN            TEXTEQU         <21:20>
;  RW--V 
;  2197 : #define NV_PGRAPH_CLIPX_1_CLIP13_MIN_GT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP13_MIN_GT         EQU             000000000h
;  RWI-V 
;  2198 : #define NV_PGRAPH_CLIPX_1_CLIP13_MIN_LT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP13_MIN_LT         EQU             000000001h
;  RW--V 
;  2199 : #define NV_PGRAPH_CLIPX_1_CLIP13_MIN_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP13_MIN_EQ         EQU             000000002h
;  RWIVF 
;  2200 : #define NV_PGRAPH_CLIPX_1_CLIP13_MAX                          23:22 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP13_MAX            TEXTEQU         <23:22>
;  RW--V 
;  2201 : #define NV_PGRAPH_CLIPX_1_CLIP13_MAX_LT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP13_MAX_LT         EQU             000000000h
;  RWI-V 
;  2202 : #define NV_PGRAPH_CLIPX_1_CLIP13_MAX_GT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP13_MAX_GT         EQU             000000001h
;  RW--V 
;  2203 : #define NV_PGRAPH_CLIPX_1_CLIP13_MAX_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP13_MAX_EQ         EQU             000000002h
;  RWIVF 
;  2204 : #define NV_PGRAPH_CLIPX_1_CLIP14_MIN                          25:24 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP14_MIN            TEXTEQU         <25:24>
;  RW--V 
;  2205 : #define NV_PGRAPH_CLIPX_1_CLIP14_MIN_GT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP14_MIN_GT         EQU             000000000h
;  RWI-V 
;  2206 : #define NV_PGRAPH_CLIPX_1_CLIP14_MIN_LT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP14_MIN_LT         EQU             000000001h
;  RW--V 
;  2207 : #define NV_PGRAPH_CLIPX_1_CLIP14_MIN_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP14_MIN_EQ         EQU             000000002h
;  RWIVF 
;  2208 : #define NV_PGRAPH_CLIPX_1_CLIP14_MAX                          27:26 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP14_MAX            TEXTEQU         <27:26>
;  RW--V 
;  2209 : #define NV_PGRAPH_CLIPX_1_CLIP14_MAX_LT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP14_MAX_LT         EQU             000000000h
;  RWI-V 
;  2210 : #define NV_PGRAPH_CLIPX_1_CLIP14_MAX_GT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP14_MAX_GT         EQU             000000001h
;  RW--V 
;  2211 : #define NV_PGRAPH_CLIPX_1_CLIP14_MAX_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP14_MAX_EQ         EQU             000000002h
;  RWIVF 
;  2212 : #define NV_PGRAPH_CLIPX_1_CLIP15_MIN                          29:28 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP15_MIN            TEXTEQU         <29:28>
;  RW--V 
;  2213 : #define NV_PGRAPH_CLIPX_1_CLIP15_MIN_GT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP15_MIN_GT         EQU             000000000h
;  RWI-V 
;  2214 : #define NV_PGRAPH_CLIPX_1_CLIP15_MIN_LT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP15_MIN_LT         EQU             000000001h
;  RW--V 
;  2215 : #define NV_PGRAPH_CLIPX_1_CLIP15_MIN_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP15_MIN_EQ         EQU             000000002h
;  RWIVF 
;  2216 : #define NV_PGRAPH_CLIPX_1_CLIP15_MAX                          31:30 /* RWIVF */
NV_PGRAPH_CLIPX_1_CLIP15_MAX            TEXTEQU         <31:30>
;  RW--V 
;  2217 : #define NV_PGRAPH_CLIPX_1_CLIP15_MAX_LT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP15_MAX_LT         EQU             000000000h
;  RWI-V 
;  2218 : #define NV_PGRAPH_CLIPX_1_CLIP15_MAX_GT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPX_1_CLIP15_MAX_GT         EQU             000000001h
;  RW--V 
;  2219 : #define NV_PGRAPH_CLIPX_1_CLIP15_MAX_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPX_1_CLIP15_MAX_EQ         EQU             000000002h
;  RW-4R 
;  2220 : #define NV_PGRAPH_CLIPY_0                                0x0040052c /* RW-4R */
NV_PGRAPH_CLIPY_0               EQU             00040052ch
;  RWIVF 
;  2221 : #define NV_PGRAPH_CLIPY_0_CLIP0_MIN                             1:0 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP0_MIN             TEXTEQU         <1:0>
;  RW--V 
;  2222 : #define NV_PGRAPH_CLIPY_0_CLIP0_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP0_MIN_GT          EQU             000000000h
;  RWI-V 
;  2223 : #define NV_PGRAPH_CLIPY_0_CLIP0_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP0_MIN_LT          EQU             000000001h
;  RW--V 
;  2224 : #define NV_PGRAPH_CLIPY_0_CLIP0_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP0_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2225 : #define NV_PGRAPH_CLIPY_0_CLIP0_MAX                             3:2 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP0_MAX             TEXTEQU         <3:2>
;  RW--V 
;  2226 : #define NV_PGRAPH_CLIPY_0_CLIP0_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP0_MAX_LT          EQU             000000000h
;  RWI-V 
;  2227 : #define NV_PGRAPH_CLIPY_0_CLIP0_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP0_MAX_GT          EQU             000000001h
;  RW--V 
;  2228 : #define NV_PGRAPH_CLIPY_0_CLIP0_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP0_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2229 : #define NV_PGRAPH_CLIPY_0_CLIP1_MIN                             5:4 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP1_MIN             TEXTEQU         <5:4>
;  RW--V 
;  2230 : #define NV_PGRAPH_CLIPY_0_CLIP1_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP1_MIN_GT          EQU             000000000h
;  RWI-V 
;  2231 : #define NV_PGRAPH_CLIPY_0_CLIP1_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP1_MIN_LT          EQU             000000001h
;  RW--V 
;  2232 : #define NV_PGRAPH_CLIPY_0_CLIP1_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP1_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2233 : #define NV_PGRAPH_CLIPY_0_CLIP1_MAX                             7:6 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP1_MAX             TEXTEQU         <7:6>
;  RW--V 
;  2234 : #define NV_PGRAPH_CLIPY_0_CLIP1_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP1_MAX_LT          EQU             000000000h
;  RWI-V 
;  2235 : #define NV_PGRAPH_CLIPY_0_CLIP1_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP1_MAX_GT          EQU             000000001h
;  RW--V 
;  2236 : #define NV_PGRAPH_CLIPY_0_CLIP1_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP1_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2237 : #define NV_PGRAPH_CLIPY_0_CLIP2_MIN                             9:8 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP2_MIN             TEXTEQU         <9:8>
;  RW--V 
;  2238 : #define NV_PGRAPH_CLIPY_0_CLIP2_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP2_MIN_GT          EQU             000000000h
;  RWI-V 
;  2239 : #define NV_PGRAPH_CLIPY_0_CLIP2_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP2_MIN_LT          EQU             000000001h
;  RW--V 
;  2240 : #define NV_PGRAPH_CLIPY_0_CLIP2_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP2_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2241 : #define NV_PGRAPH_CLIPY_0_CLIP2_MAX                           11:10 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP2_MAX             TEXTEQU         <11:10>
;  RW--V 
;  2242 : #define NV_PGRAPH_CLIPY_0_CLIP2_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP2_MAX_LT          EQU             000000000h
;  RWI-V 
;  2243 : #define NV_PGRAPH_CLIPY_0_CLIP2_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP2_MAX_GT          EQU             000000001h
;  RW--V 
;  2244 : #define NV_PGRAPH_CLIPY_0_CLIP2_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP2_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2245 : #define NV_PGRAPH_CLIPY_0_CLIP3_MIN                           13:12 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP3_MIN             TEXTEQU         <13:12>
;  RW--V 
;  2246 : #define NV_PGRAPH_CLIPY_0_CLIP3_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP3_MIN_GT          EQU             000000000h
;  RWI-V 
;  2247 : #define NV_PGRAPH_CLIPY_0_CLIP3_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP3_MIN_LT          EQU             000000001h
;  RW--V 
;  2248 : #define NV_PGRAPH_CLIPY_0_CLIP3_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP3_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2249 : #define NV_PGRAPH_CLIPY_0_CLIP3_MAX                           15:14 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP3_MAX             TEXTEQU         <15:14>
;  RW--V 
;  2250 : #define NV_PGRAPH_CLIPY_0_CLIP3_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP3_MAX_LT          EQU             000000000h
;  RWI-V 
;  2251 : #define NV_PGRAPH_CLIPY_0_CLIP3_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP3_MAX_GT          EQU             000000001h
;  RW--V 
;  2252 : #define NV_PGRAPH_CLIPY_0_CLIP3_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP3_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2253 : #define NV_PGRAPH_CLIPY_0_CLIP4_MIN                           17:16 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP4_MIN             TEXTEQU         <17:16>
;  RW--V 
;  2254 : #define NV_PGRAPH_CLIPY_0_CLIP4_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP4_MIN_GT          EQU             000000000h
;  RWI-V 
;  2255 : #define NV_PGRAPH_CLIPY_0_CLIP4_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP4_MIN_LT          EQU             000000001h
;  RW--V 
;  2256 : #define NV_PGRAPH_CLIPY_0_CLIP4_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP4_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2257 : #define NV_PGRAPH_CLIPY_0_CLIP4_MAX                           19:18 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP4_MAX             TEXTEQU         <19:18>
;  RW--V 
;  2258 : #define NV_PGRAPH_CLIPY_0_CLIP4_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP4_MAX_LT          EQU             000000000h
;  RWI-V 
;  2259 : #define NV_PGRAPH_CLIPY_0_CLIP4_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP4_MAX_GT          EQU             000000001h
;  RW--V 
;  2260 : #define NV_PGRAPH_CLIPY_0_CLIP4_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP4_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2261 : #define NV_PGRAPH_CLIPY_0_CLIP5_MIN                           21:20 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP5_MIN             TEXTEQU         <21:20>
;  RW--V 
;  2262 : #define NV_PGRAPH_CLIPY_0_CLIP5_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP5_MIN_GT          EQU             000000000h
;  RWI-V 
;  2263 : #define NV_PGRAPH_CLIPY_0_CLIP5_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP5_MIN_LT          EQU             000000001h
;  RW--V 
;  2264 : #define NV_PGRAPH_CLIPY_0_CLIP5_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP5_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2265 : #define NV_PGRAPH_CLIPY_0_CLIP5_MAX                           23:22 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP5_MAX             TEXTEQU         <23:22>
;  RW--V 
;  2266 : #define NV_PGRAPH_CLIPY_0_CLIP5_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP5_MAX_LT          EQU             000000000h
;  RWI-V 
;  2267 : #define NV_PGRAPH_CLIPY_0_CLIP5_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP5_MAX_GT          EQU             000000001h
;  RW--V 
;  2268 : #define NV_PGRAPH_CLIPY_0_CLIP5_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP5_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2269 : #define NV_PGRAPH_CLIPY_0_CLIP6_MIN                           25:24 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP6_MIN             TEXTEQU         <25:24>
;  RW--V 
;  2270 : #define NV_PGRAPH_CLIPY_0_CLIP6_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP6_MIN_GT          EQU             000000000h
;  RWI-V 
;  2271 : #define NV_PGRAPH_CLIPY_0_CLIP6_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP6_MIN_LT          EQU             000000001h
;  RW--V 
;  2272 : #define NV_PGRAPH_CLIPY_0_CLIP6_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP6_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2273 : #define NV_PGRAPH_CLIPY_0_CLIP6_MAX                           27:26 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP6_MAX             TEXTEQU         <27:26>
;  RW--V 
;  2274 : #define NV_PGRAPH_CLIPY_0_CLIP6_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP6_MAX_LT          EQU             000000000h
;  RWI-V 
;  2275 : #define NV_PGRAPH_CLIPY_0_CLIP6_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP6_MAX_GT          EQU             000000001h
;  RW--V 
;  2276 : #define NV_PGRAPH_CLIPY_0_CLIP6_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP6_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2277 : #define NV_PGRAPH_CLIPY_0_CLIP7_MIN                           29:28 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP7_MIN             TEXTEQU         <29:28>
;  RW--V 
;  2278 : #define NV_PGRAPH_CLIPY_0_CLIP7_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP7_MIN_GT          EQU             000000000h
;  RWI-V 
;  2279 : #define NV_PGRAPH_CLIPY_0_CLIP7_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP7_MIN_LT          EQU             000000001h
;  RW--V 
;  2280 : #define NV_PGRAPH_CLIPY_0_CLIP7_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP7_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2281 : #define NV_PGRAPH_CLIPY_0_CLIP7_MAX                           31:30 /* RWIVF */
NV_PGRAPH_CLIPY_0_CLIP7_MAX             TEXTEQU         <31:30>
;  RW--V 
;  2282 : #define NV_PGRAPH_CLIPY_0_CLIP7_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP7_MAX_LT          EQU             000000000h
;  RWI-V 
;  2283 : #define NV_PGRAPH_CLIPY_0_CLIP7_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_0_CLIP7_MAX_GT          EQU             000000001h
;  RW--V 
;  2284 : #define NV_PGRAPH_CLIPY_0_CLIP7_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_0_CLIP7_MAX_EQ          EQU             000000002h
;  RW-4R 
;  2285 : #define NV_PGRAPH_CLIPY_1                                0x00400530 /* RW-4R */
NV_PGRAPH_CLIPY_1               EQU             000400530h
;  RWIVF 
;  2286 : #define NV_PGRAPH_CLIPY_1_CLIP8_MIN                             1:0 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP8_MIN             TEXTEQU         <1:0>
;  RW--V 
;  2287 : #define NV_PGRAPH_CLIPY_1_CLIP8_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP8_MIN_GT          EQU             000000000h
;  RWI-V 
;  2288 : #define NV_PGRAPH_CLIPY_1_CLIP8_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP8_MIN_LT          EQU             000000001h
;  RW--V 
;  2289 : #define NV_PGRAPH_CLIPY_1_CLIP8_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP8_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2290 : #define NV_PGRAPH_CLIPY_1_CLIP8_MAX                             3:2 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP8_MAX             TEXTEQU         <3:2>
;  RW--V 
;  2291 : #define NV_PGRAPH_CLIPY_1_CLIP8_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP8_MAX_LT          EQU             000000000h
;  RWI-V 
;  2292 : #define NV_PGRAPH_CLIPY_1_CLIP8_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP8_MAX_GT          EQU             000000001h
;  RW--V 
;  2293 : #define NV_PGRAPH_CLIPY_1_CLIP8_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP8_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2294 : #define NV_PGRAPH_CLIPY_1_CLIP9_MIN                             5:4 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP9_MIN             TEXTEQU         <5:4>
;  RW--V 
;  2295 : #define NV_PGRAPH_CLIPY_1_CLIP9_MIN_GT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP9_MIN_GT          EQU             000000000h
;  RWI-V 
;  2296 : #define NV_PGRAPH_CLIPY_1_CLIP9_MIN_LT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP9_MIN_LT          EQU             000000001h
;  RW--V 
;  2297 : #define NV_PGRAPH_CLIPY_1_CLIP9_MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP9_MIN_EQ          EQU             000000002h
;  RWIVF 
;  2298 : #define NV_PGRAPH_CLIPY_1_CLIP9_MAX                             7:6 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP9_MAX             TEXTEQU         <7:6>
;  RW--V 
;  2299 : #define NV_PGRAPH_CLIPY_1_CLIP9_MAX_LT                   0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP9_MAX_LT          EQU             000000000h
;  RWI-V 
;  2300 : #define NV_PGRAPH_CLIPY_1_CLIP9_MAX_GT                   0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP9_MAX_GT          EQU             000000001h
;  RW--V 
;  2301 : #define NV_PGRAPH_CLIPY_1_CLIP9_MAX_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP9_MAX_EQ          EQU             000000002h
;  RWIVF 
;  2302 : #define NV_PGRAPH_CLIPY_1_CLIP10_MIN                            9:8 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP10_MIN            TEXTEQU         <9:8>
;  RW--V 
;  2303 : #define NV_PGRAPH_CLIPY_1_CLIP10_MIN_GT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP10_MIN_GT         EQU             000000000h
;  RWI-V 
;  2304 : #define NV_PGRAPH_CLIPY_1_CLIP10_MIN_LT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP10_MIN_LT         EQU             000000001h
;  RW--V 
;  2305 : #define NV_PGRAPH_CLIPY_1_CLIP10_MIN_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP10_MIN_EQ         EQU             000000002h
;  RWIVF 
;  2306 : #define NV_PGRAPH_CLIPY_1_CLIP10_MAX                          11:10 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP10_MAX            TEXTEQU         <11:10>
;  RW--V 
;  2307 : #define NV_PGRAPH_CLIPY_1_CLIP10_MAX_LT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP10_MAX_LT         EQU             000000000h
;  RWI-V 
;  2308 : #define NV_PGRAPH_CLIPY_1_CLIP10_MAX_GT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP10_MAX_GT         EQU             000000001h
;  RW--V 
;  2309 : #define NV_PGRAPH_CLIPY_1_CLIP10_MAX_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP10_MAX_EQ         EQU             000000002h
;  RWIVF 
;  2310 : #define NV_PGRAPH_CLIPY_1_CLIP11_MIN                          13:12 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP11_MIN            TEXTEQU         <13:12>
;  RW--V 
;  2311 : #define NV_PGRAPH_CLIPY_1_CLIP11_MIN_GT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP11_MIN_GT         EQU             000000000h
;  RWI-V 
;  2312 : #define NV_PGRAPH_CLIPY_1_CLIP11_MIN_LT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP11_MIN_LT         EQU             000000001h
;  RW--V 
;  2313 : #define NV_PGRAPH_CLIPY_1_CLIP11MIN_EQ                   0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP11MIN_EQ          EQU             000000002h
;  RWIVF 
;  2314 : #define NV_PGRAPH_CLIPY_1_CLIP11_MAX                          15:14 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP11_MAX            TEXTEQU         <15:14>
;  RW--V 
;  2315 : #define NV_PGRAPH_CLIPY_1_CLIP11_MAX_LT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP11_MAX_LT         EQU             000000000h
;  RWI-V 
;  2316 : #define NV_PGRAPH_CLIPY_1_CLIP11_MAX_GT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP11_MAX_GT         EQU             000000001h
;  RW--V 
;  2317 : #define NV_PGRAPH_CLIPY_1_CLIP11_MAX_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP11_MAX_EQ         EQU             000000002h
;  RWIVF 
;  2318 : #define NV_PGRAPH_CLIPY_1_CLIP12_MIN                          17:16 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP12_MIN            TEXTEQU         <17:16>
;  RW--V 
;  2319 : #define NV_PGRAPH_CLIPY_1_CLIP12_MIN_GT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP12_MIN_GT         EQU             000000000h
;  RWI-V 
;  2320 : #define NV_PGRAPH_CLIPY_1_CLIP12_MIN_LT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP12_MIN_LT         EQU             000000001h
;  RW--V 
;  2321 : #define NV_PGRAPH_CLIPY_1_CLIP12_MIN_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP12_MIN_EQ         EQU             000000002h
;  RWIVF 
;  2322 : #define NV_PGRAPH_CLIPY_1_CLIP12_MAX                          19:18 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP12_MAX            TEXTEQU         <19:18>
;  RW--V 
;  2323 : #define NV_PGRAPH_CLIPY_1_CLIP12_MAX_LT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP12_MAX_LT         EQU             000000000h
;  RWI-V 
;  2324 : #define NV_PGRAPH_CLIPY_1_CLIP12_MAX_GT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP12_MAX_GT         EQU             000000001h
;  RW--V 
;  2325 : #define NV_PGRAPH_CLIPY_1_CLIP12_MAX_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP12_MAX_EQ         EQU             000000002h
;  RWIVF 
;  2326 : #define NV_PGRAPH_CLIPY_1_CLIP13_MIN                          21:20 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP13_MIN            TEXTEQU         <21:20>
;  RW--V 
;  2327 : #define NV_PGRAPH_CLIPY_1_CLIP13_MIN_GT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP13_MIN_GT         EQU             000000000h
;  RWI-V 
;  2328 : #define NV_PGRAPH_CLIPY_1_CLIP13_MIN_LT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP13_MIN_LT         EQU             000000001h
;  RW--V 
;  2329 : #define NV_PGRAPH_CLIPY_1_CLIP13_MIN_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP13_MIN_EQ         EQU             000000002h
;  RWIVF 
;  2330 : #define NV_PGRAPH_CLIPY_1_CLIP13_MAX                          23:22 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP13_MAX            TEXTEQU         <23:22>
;  RW--V 
;  2331 : #define NV_PGRAPH_CLIPY_1_CLIP13_MAX_LT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP13_MAX_LT         EQU             000000000h
;  RWI-V 
;  2332 : #define NV_PGRAPH_CLIPY_1_CLIP13_MAX_GT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP13_MAX_GT         EQU             000000001h
;  RW--V 
;  2333 : #define NV_PGRAPH_CLIPY_1_CLIP13_MAX_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP13_MAX_EQ         EQU             000000002h
;  RWIVF 
;  2334 : #define NV_PGRAPH_CLIPY_1_CLIP14_MIN                          25:24 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP14_MIN            TEXTEQU         <25:24>
;  RW--V 
;  2335 : #define NV_PGRAPH_CLIPY_1_CLIP14_MIN_GT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP14_MIN_GT         EQU             000000000h
;  RWI-V 
;  2336 : #define NV_PGRAPH_CLIPY_1_CLIP14_MIN_LT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP14_MIN_LT         EQU             000000001h
;  RW--V 
;  2337 : #define NV_PGRAPH_CLIPY_1_CLIP14_MIN_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP14_MIN_EQ         EQU             000000002h
;  RWIVF 
;  2338 : #define NV_PGRAPH_CLIPY_1_CLIP14_MAX                          27:26 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP14_MAX            TEXTEQU         <27:26>
;  RW--V 
;  2339 : #define NV_PGRAPH_CLIPY_1_CLIP14_MAX_LT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP14_MAX_LT         EQU             000000000h
;  RWI-V 
;  2340 : #define NV_PGRAPH_CLIPY_1_CLIP14_MAX_GT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP14_MAX_GT         EQU             000000001h
;  RW--V 
;  2341 : #define NV_PGRAPH_CLIPY_1_CLIP14_MAX_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP14_MAX_EQ         EQU             000000002h
;  RWIVF 
;  2342 : #define NV_PGRAPH_CLIPY_1_CLIP15_MIN                          29:28 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP15_MIN            TEXTEQU         <29:28>
;  RW--V 
;  2343 : #define NV_PGRAPH_CLIPY_1_CLIP15_MIN_GT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP15_MIN_GT         EQU             000000000h
;  RWI-V 
;  2344 : #define NV_PGRAPH_CLIPY_1_CLIP15_MIN_LT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP15_MIN_LT         EQU             000000001h
;  RW--V 
;  2345 : #define NV_PGRAPH_CLIPY_1_CLIP15_MIN_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP15_MIN_EQ         EQU             000000002h
;  RWIVF 
;  2346 : #define NV_PGRAPH_CLIPY_1_CLIP15_MAX                          31:30 /* RWIVF */
NV_PGRAPH_CLIPY_1_CLIP15_MAX            TEXTEQU         <31:30>
;  RW--V 
;  2347 : #define NV_PGRAPH_CLIPY_1_CLIP15_MAX_LT                  0x00000000 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP15_MAX_LT         EQU             000000000h
;  RWI-V 
;  2348 : #define NV_PGRAPH_CLIPY_1_CLIP15_MAX_GT                  0x00000001 /* RWI-V */
NV_PGRAPH_CLIPY_1_CLIP15_MAX_GT         EQU             000000001h
;  RW--V 
;  2349 : #define NV_PGRAPH_CLIPY_1_CLIP15_MAX_EQ                  0x00000002 /* RW--V */
NV_PGRAPH_CLIPY_1_CLIP15_MAX_EQ         EQU             000000002h
;  RW-4R 
;  2350 : #define NV_PGRAPH_MISC24_0                               0x00400510 /* RW-4R */
NV_PGRAPH_MISC24_0              EQU             000400510h
;  RWXSF 
;  2351 : #define NV_PGRAPH_MISC24_0_VALUE                               23:0 /* RWXSF */
NV_PGRAPH_MISC24_0_VALUE                TEXTEQU         <23:0>
;  RW-4R 
;  2352 : #define NV_PGRAPH_MISC24_1                               0x00400570 /* RW-4R */
NV_PGRAPH_MISC24_1              EQU             000400570h
;  RWXSF 
;  2353 : #define NV_PGRAPH_MISC24_1_VALUE                               23:0 /* RWXSF */
NV_PGRAPH_MISC24_1_VALUE                TEXTEQU         <23:0>
;  RW-4R 
;  2354 : #define NV_PGRAPH_PASSTHRU                               0x0040054c /* RW-4R */
NV_PGRAPH_PASSTHRU              EQU             00040054ch
;  RWXSF 
;  2355 : #define NV_PGRAPH_PASSTHRU_VALUE                               23:0 /* RWXSF */
NV_PGRAPH_PASSTHRU_VALUE                TEXTEQU         <23:0>
;  RW-4A 
;        
;  2356 : #define NV_PGRAPH_ZFOG_RAM(i)                    (0x00400580+(i)*4) /* RW-4A */
;  2357 : #define NV_PGRAPH_ZFOG_RAM__SIZE_1                               16 /*       */
NV_PGRAPH_ZFOG_RAM__SIZE_1              EQU             16t
;  RWXUF 
;  2358 : #define NV_PGRAPH_ZFOG_RAM_ZETA                                15:0 /* RWXUF */
NV_PGRAPH_ZFOG_RAM_ZETA         TEXTEQU         <15:0>
;  RWXUF 
;  2359 : #define NV_PGRAPH_ZFOG_RAM_FOG                                23:16 /* RWXUF */
NV_PGRAPH_ZFOG_RAM_FOG          TEXTEQU         <23:16>
;  RW-4R 
;  2360 : #define NV_PGRAPH_D3D_XY                                 0x004005c0 /* RW-4R */
NV_PGRAPH_D3D_XY                EQU             0004005c0h
;  RWXSF 
;  2361 : #define NV_PGRAPH_D3D_XY_X_VALUE                               15:0 /* RWXSF */
NV_PGRAPH_D3D_XY_X_VALUE                TEXTEQU         <15:0>
;  RWXSF 
;  2362 : #define NV_PGRAPH_D3D_XY_Y_VALUE                              31:16 /* RWXSF */
NV_PGRAPH_D3D_XY_Y_VALUE                TEXTEQU         <31:16>
;  RW-4R 
;  2363 : #define NV_PGRAPH_D3D_UV                                 0x004005c4 /* RW-4R */
NV_PGRAPH_D3D_UV                EQU             0004005c4h
;  RWXSF 
;  2364 : #define NV_PGRAPH_D3D_UV_U_VALUE                               15:0 /* RWXSF */
NV_PGRAPH_D3D_UV_U_VALUE                TEXTEQU         <15:0>
;  RWXSF 
;  2365 : #define NV_PGRAPH_D3D_UV_V_VALUE                              31:16 /* RWXSF */
NV_PGRAPH_D3D_UV_V_VALUE                TEXTEQU         <31:16>
;  RW-4R 
;  2366 : #define NV_PGRAPH_D3D_ZETA                               0x004005c8 /* RW-4R */
NV_PGRAPH_D3D_ZETA              EQU             0004005c8h
;  RWXSF 
;  2367 : #define NV_PGRAPH_D3D_ZETA_VALUE                               15:0 /* RWXSF */
NV_PGRAPH_D3D_ZETA_VALUE                TEXTEQU         <15:0>
;  RW-4R 
;  2368 : #define NV_PGRAPH_D3D_RGB                                0x004005cc /* RW-4R */
NV_PGRAPH_D3D_RGB               EQU             0004005cch
;  RWXSF 
;  2369 : #define NV_PGRAPH_D3D_RGB_VALUE                                15:0 /* RWXSF */
NV_PGRAPH_D3D_RGB_VALUE         TEXTEQU         <15:0>
;  RW-4R 
;  2370 : #define NV_PGRAPH_D3D_FOG                                0x004005d0 /* RW-4R */
NV_PGRAPH_D3D_FOG               EQU             0004005d0h
;  RWXSF 
;  2371 : #define NV_PGRAPH_D3D_FOG_I0                                    3:0 /* RWXSF */
NV_PGRAPH_D3D_FOG_I0            TEXTEQU         <3:0>
;  RWXSF 
;  2372 : #define NV_PGRAPH_D3D_FOG_I1                                    7:4 /* RWXSF */
NV_PGRAPH_D3D_FOG_I1            TEXTEQU         <7:4>
;  RWXSF 
;  2373 : #define NV_PGRAPH_D3D_FOG_I2                                   11:8 /* RWXSF */
NV_PGRAPH_D3D_FOG_I2            TEXTEQU         <11:8>
;  RWXSF 
;  2374 : #define NV_PGRAPH_D3D_FOG_I3                                  15:12 /* RWXSF */
NV_PGRAPH_D3D_FOG_I3            TEXTEQU         <15:12>
;  RWXSF 
;  2375 : #define NV_PGRAPH_D3D_FOG_I4                                  19:16 /* RWXSF */
NV_PGRAPH_D3D_FOG_I4            TEXTEQU         <19:16>
;  RWXSF 
;  2376 : #define NV_PGRAPH_D3D_FOG_I5                                  23:20 /* RWXSF */
NV_PGRAPH_D3D_FOG_I5            TEXTEQU         <23:20>
;  RWXSF 
;  2377 : #define NV_PGRAPH_D3D_FOG_FOG_VALUE                           31:24 /* RWXSF */
NV_PGRAPH_D3D_FOG_FOG_VALUE             TEXTEQU         <31:24>
;  RW-4R 
;  2378 : #define NV_PGRAPH_D3D_M                                  0x004005d4 /* RW-4R */
NV_PGRAPH_D3D_M         EQU             0004005d4h
;  RWXSF 
;  2379 : #define NV_PGRAPH_D3D_M_VALUE                                  31:0 /* RWXSF */
NV_PGRAPH_D3D_M_VALUE           TEXTEQU         <31:0>
;  RW-4R 
;  2380 : #define NV_PGRAPH_DMA_INTR_0                             0x00401100 /* RW-4R */
NV_PGRAPH_DMA_INTR_0            EQU             000401100h
;  RWXVF 
;  2381 : #define NV_PGRAPH_DMA_INTR_0_INSTANCE                           0:0 /* RWXVF */
NV_PGRAPH_DMA_INTR_0_INSTANCE           TEXTEQU         <0:0>
;  R---V 
;  2382 : #define NV_PGRAPH_DMA_INTR_0_INSTANCE_NOT_PENDING        0x00000000 /* R---V */
NV_PGRAPH_DMA_INTR_0_INSTANCE_NOT_PENDING               EQU             000000000h
;  R---V 
;  2383 : #define NV_PGRAPH_DMA_INTR_0_INSTANCE_PENDING            0x00000001 /* R---V */
NV_PGRAPH_DMA_INTR_0_INSTANCE_PENDING           EQU             000000001h
;  -W--V 
;  2384 : #define NV_PGRAPH_DMA_INTR_0_INSTANCE_RESET              0x00000001 /* -W--V */
NV_PGRAPH_DMA_INTR_0_INSTANCE_RESET             EQU             000000001h
;  RWXVF 
;  2385 : #define NV_PGRAPH_DMA_INTR_0_PRESENT                            4:4 /* RWXVF */
NV_PGRAPH_DMA_INTR_0_PRESENT            TEXTEQU         <4:4>
;  R---V 
;  2386 : #define NV_PGRAPH_DMA_INTR_0_PRESENT_NOT_PENDING         0x00000000 /* R---V */
NV_PGRAPH_DMA_INTR_0_PRESENT_NOT_PENDING                EQU             000000000h
;  R---V 
;  2387 : #define NV_PGRAPH_DMA_INTR_0_PRESENT_PENDING             0x00000001 /* R---V */
NV_PGRAPH_DMA_INTR_0_PRESENT_PENDING            EQU             000000001h
;  -W--V 
;  2388 : #define NV_PGRAPH_DMA_INTR_0_PRESENT_RESET               0x00000001 /* -W--V */
NV_PGRAPH_DMA_INTR_0_PRESENT_RESET              EQU             000000001h
;  RWXVF 
;  2389 : #define NV_PGRAPH_DMA_INTR_0_PROTECTION                         8:8 /* RWXVF */
NV_PGRAPH_DMA_INTR_0_PROTECTION         TEXTEQU         <8:8>
;  R---V 
;  2390 : #define NV_PGRAPH_DMA_INTR_0_PROTECTION_NOT_PENDING      0x00000000 /* R---V */
NV_PGRAPH_DMA_INTR_0_PROTECTION_NOT_PENDING             EQU             000000000h
;  R---V 
;  2391 : #define NV_PGRAPH_DMA_INTR_0_PROTECTION_PENDING          0x00000001 /* R---V */
NV_PGRAPH_DMA_INTR_0_PROTECTION_PENDING         EQU             000000001h
;  -W--V 
;  2392 : #define NV_PGRAPH_DMA_INTR_0_PROTECTION_RESET            0x00000001 /* -W--V */
NV_PGRAPH_DMA_INTR_0_PROTECTION_RESET           EQU             000000001h
;  RWXVF 
;  2393 : #define NV_PGRAPH_DMA_INTR_0_LINEAR                           12:12 /* RWXVF */
NV_PGRAPH_DMA_INTR_0_LINEAR             TEXTEQU         <12:12>
;  R---V 
;  2394 : #define NV_PGRAPH_DMA_INTR_0_LINEAR_NOT_PENDING          0x00000000 /* R---V */
NV_PGRAPH_DMA_INTR_0_LINEAR_NOT_PENDING         EQU             000000000h
;  R---V 
;  2395 : #define NV_PGRAPH_DMA_INTR_0_LINEAR_PENDING              0x00000001 /* R---V */
NV_PGRAPH_DMA_INTR_0_LINEAR_PENDING             EQU             000000001h
;  -W--V 
;  2396 : #define NV_PGRAPH_DMA_INTR_0_LINEAR_RESET                0x00000001 /* -W--V */
NV_PGRAPH_DMA_INTR_0_LINEAR_RESET               EQU             000000001h
;  RWXVF 
;  2397 : #define NV_PGRAPH_DMA_INTR_0_NOTIFY                           16:16 /* RWXVF */
NV_PGRAPH_DMA_INTR_0_NOTIFY             TEXTEQU         <16:16>
;  R---V 
;  2398 : #define NV_PGRAPH_DMA_INTR_0_NOTIFY_NOT_PENDING          0x00000000 /* R---V */
NV_PGRAPH_DMA_INTR_0_NOTIFY_NOT_PENDING         EQU             000000000h
;  R---V 
;  2399 : #define NV_PGRAPH_DMA_INTR_0_NOTIFY_PENDING              0x00000001 /* R---V */
NV_PGRAPH_DMA_INTR_0_NOTIFY_PENDING             EQU             000000001h
;  -W--V 
;  2400 : #define NV_PGRAPH_DMA_INTR_0_NOTIFY_RESET                0x00000001 /* -W--V */
NV_PGRAPH_DMA_INTR_0_NOTIFY_RESET               EQU             000000001h
;  RW-4R 
;  2401 : #define NV_PGRAPH_DMA_INTR_EN_0                          0x00401140 /* RW-4R */
NV_PGRAPH_DMA_INTR_EN_0         EQU             000401140h
;  RWIVF 
;  2402 : #define NV_PGRAPH_DMA_INTR_EN_0_INSTANCE                        0:0 /* RWIVF */
NV_PGRAPH_DMA_INTR_EN_0_INSTANCE                TEXTEQU         <0:0>
;  RWI-V 
;  2403 : #define NV_PGRAPH_DMA_INTR_EN_0_INSTANCE_DISABLED        0x00000000 /* RWI-V */
NV_PGRAPH_DMA_INTR_EN_0_INSTANCE_DISABLED               EQU             000000000h
;  RW--V 
;  2404 : #define NV_PGRAPH_DMA_INTR_EN_0_INSTANCE_ENABLED         0x00000001 /* RW--V */
NV_PGRAPH_DMA_INTR_EN_0_INSTANCE_ENABLED                EQU             000000001h
;  RWIVF 
;  2405 : #define NV_PGRAPH_DMA_INTR_EN_0_PRESENT                         4:4 /* RWIVF */
NV_PGRAPH_DMA_INTR_EN_0_PRESENT         TEXTEQU         <4:4>
;  RWI-V 
;  2406 : #define NV_PGRAPH_DMA_INTR_EN_0_PRESENT_DISABLED         0x00000000 /* RWI-V */
NV_PGRAPH_DMA_INTR_EN_0_PRESENT_DISABLED                EQU             000000000h
;  RW--V 
;  2407 : #define NV_PGRAPH_DMA_INTR_EN_0_PRESENT_ENABLED          0x00000001 /* RW--V */
NV_PGRAPH_DMA_INTR_EN_0_PRESENT_ENABLED         EQU             000000001h
;  RWIVF 
;  2408 : #define NV_PGRAPH_DMA_INTR_EN_0_PROTECTION                      8:8 /* RWIVF */
NV_PGRAPH_DMA_INTR_EN_0_PROTECTION              TEXTEQU         <8:8>
;  RWI-V 
;  2409 : #define NV_PGRAPH_DMA_INTR_EN_0_PROTECTION_DISABLED      0x00000000 /* RWI-V */
NV_PGRAPH_DMA_INTR_EN_0_PROTECTION_DISABLED             EQU             000000000h
;  RW--V 
;  2410 : #define NV_PGRAPH_DMA_INTR_EN_0_PROTECTION_ENABLED       0x00000001 /* RW--V */
NV_PGRAPH_DMA_INTR_EN_0_PROTECTION_ENABLED              EQU             000000001h
;  RWIVF 
;  2411 : #define NV_PGRAPH_DMA_INTR_EN_0_LINEAR                        12:12 /* RWIVF */
NV_PGRAPH_DMA_INTR_EN_0_LINEAR          TEXTEQU         <12:12>
;  RWI-V 
;  2412 : #define NV_PGRAPH_DMA_INTR_EN_0_LINEAR_DISABLED          0x00000000 /* RWI-V */
NV_PGRAPH_DMA_INTR_EN_0_LINEAR_DISABLED         EQU             000000000h
;  RW--V 
;  2413 : #define NV_PGRAPH_DMA_INTR_EN_0_LINEAR_ENABLED           0x00000001 /* RW--V */
NV_PGRAPH_DMA_INTR_EN_0_LINEAR_ENABLED          EQU             000000001h
;  RWIVF 
;  2414 : #define NV_PGRAPH_DMA_INTR_EN_0_NOTIFY                        16:16 /* RWIVF */
NV_PGRAPH_DMA_INTR_EN_0_NOTIFY          TEXTEQU         <16:16>
;  RWI-V 
;  2415 : #define NV_PGRAPH_DMA_INTR_EN_0_NOTIFY_DISABLED          0x00000000 /* RWI-V */
NV_PGRAPH_DMA_INTR_EN_0_NOTIFY_DISABLED         EQU             000000000h
;  RW--V 
;  2416 : #define NV_PGRAPH_DMA_INTR_EN_0_NOTIFY_ENABLED           0x00000001 /* RW--V */
NV_PGRAPH_DMA_INTR_EN_0_NOTIFY_ENABLED          EQU             000000001h
;  -W-4R 
;  2417 : #define NV_PGRAPH_DMA_ACCESS                             0x00401200 /* -W-4R */
NV_PGRAPH_DMA_ACCESS            EQU             000401200h
;  -W-VF 
;  2418 : #define NV_PGRAPH_DMA_PTE_ACCESS_PTE                            0:0 /* -W-VF */
NV_PGRAPH_DMA_PTE_ACCESS_PTE            TEXTEQU         <0:0>
;  -W--V 
;  2419 : #define NV_PGRAPH_DMA_PTE_ACCESS_PTE_ENABLE              0x00000001 /* -W--V */
NV_PGRAPH_DMA_PTE_ACCESS_PTE_ENABLE             EQU             000000001h
;  RW-4R 
;  2420 : #define NV_PGRAPH_DMA_CONTROL                            0x00401210 /* RW-4R */
NV_PGRAPH_DMA_CONTROL           EQU             000401210h
;  RWXUF 
;  2421 : #define NV_PGRAPH_DMA_CONTROL_ADJUST                           11:0 /* RWXUF */
NV_PGRAPH_DMA_CONTROL_ADJUST            TEXTEQU         <11:0>
;  RWXVF 
;  2422 : #define NV_PGRAPH_DMA_CONTROL_PAGE_TABLE                      16:16 /* RWXVF */
NV_PGRAPH_DMA_CONTROL_PAGE_TABLE                TEXTEQU         <16:16>
;  RW--V 
;  2423 : #define NV_PGRAPH_DMA_CONTROL_PAGE_TABLE_NOT_PRESENT     0x00000000 /* RW--V */
NV_PGRAPH_DMA_CONTROL_PAGE_TABLE_NOT_PRESENT            EQU             000000000h
;  RW--V 
;  2424 : #define NV_PGRAPH_DMA_CONTROL_PAGE_TABLE_PRESENT         0x00000001 /* RW--V */
NV_PGRAPH_DMA_CONTROL_PAGE_TABLE_PRESENT                EQU             000000001h
;  RWXUF 
;  2425 : #define NV_PGRAPH_DMA_CONTROL_TARGET_NODE                     25:24 /* RWXUF */
NV_PGRAPH_DMA_CONTROL_TARGET_NODE               TEXTEQU         <25:24>
;  RW--V 
;  2426 : #define NV_PGRAPH_DMA_CONTROL_TARGET_NODE_NVM            0x00000000 /* RW--V */
NV_PGRAPH_DMA_CONTROL_TARGET_NODE_NVM           EQU             000000000h
;  RW--V 
;  2427 : #define NV_PGRAPH_DMA_CONTROL_TARGET_NODE_CART           0x00000001 /* RW--V */
NV_PGRAPH_DMA_CONTROL_TARGET_NODE_CART          EQU             000000001h
;  RW--V 
;  2428 : #define NV_PGRAPH_DMA_CONTROL_TARGET_NODE_PCI            0x00000002 /* RW--V */
NV_PGRAPH_DMA_CONTROL_TARGET_NODE_PCI           EQU             000000002h
;  RW--V 
;  2429 : #define NV_PGRAPH_DMA_CONTROL_TARGET_NODE_AGP            0x00000003 /* RW--V */
NV_PGRAPH_DMA_CONTROL_TARGET_NODE_AGP           EQU             000000003h
;  RW-4R 
;  2430 : #define NV_PGRAPH_DMA_LIMIT                              0x00401220 /* RW-4R */
NV_PGRAPH_DMA_LIMIT             EQU             000401220h
;  RWXUF 
;  2431 : #define NV_PGRAPH_DMA_LIMIT_OFFSET                             31:0 /* RWXUF */
NV_PGRAPH_DMA_LIMIT_OFFSET              TEXTEQU         <31:0>
;  RW-4R 
;  2432 : #define NV_PGRAPH_DMA_TLB_PTE                            0x00401230 /* RW-4R */
NV_PGRAPH_DMA_TLB_PTE           EQU             000401230h
;  RWXVF 
;  2433 : #define NV_PGRAPH_DMA_TLB_PTE_PAGE                              0:0 /* RWXVF */
NV_PGRAPH_DMA_TLB_PTE_PAGE              TEXTEQU         <0:0>
;  RW--V 
;  2434 : #define NV_PGRAPH_DMA_TLB_PTE_PAGE_NOT_PRESENT           0x00000000 /* RW--V */
NV_PGRAPH_DMA_TLB_PTE_PAGE_NOT_PRESENT          EQU             000000000h
;  RW--V 
;  2435 : #define NV_PGRAPH_DMA_TLB_PTE_PAGE_PRESENT               0x00000001 /* RW--V */
NV_PGRAPH_DMA_TLB_PTE_PAGE_PRESENT              EQU             000000001h
;  RWXVF 
;  2436 : #define NV_PGRAPH_DMA_TLB_PTE_ACCESS                            1:1 /* RWXVF */
NV_PGRAPH_DMA_TLB_PTE_ACCESS            TEXTEQU         <1:1>
;  RW--V 
;  2437 : #define NV_PGRAPH_DMA_TLB_PTE_ACCESS_READ_ONLY           0x00000000 /* RW--V */
NV_PGRAPH_DMA_TLB_PTE_ACCESS_READ_ONLY          EQU             000000000h
;  RW--V 
;  2438 : #define NV_PGRAPH_DMA_TLB_PTE_ACCESS_READ_WRITE          0x00000001 /* RW--V */
NV_PGRAPH_DMA_TLB_PTE_ACCESS_READ_WRITE         EQU             000000001h
;  RWXUF 
;  2439 : #define NV_PGRAPH_DMA_TLB_PTE_FRAME_ADDRESS                   31:12 /* RWXUF */
NV_PGRAPH_DMA_TLB_PTE_FRAME_ADDRESS             TEXTEQU         <31:12>
;  RW-4R 
;  2440 : #define NV_PGRAPH_DMA_TLB_TAG                            0x00401240 /* RW-4R */
NV_PGRAPH_DMA_TLB_TAG           EQU             000401240h
;  RWXUF 
;  2441 : #define NV_PGRAPH_DMA_TLB_TAG_ADDRESS                         31:12 /* RWXUF */
NV_PGRAPH_DMA_TLB_TAG_ADDRESS           TEXTEQU         <31:12>
;  RW-4R 
;  2442 : #define NV_PGRAPH_DMA_ADJ_OFFSET                         0x00401250 /* RW-4R */
NV_PGRAPH_DMA_ADJ_OFFSET                EQU             000401250h
;  RWXUF 
;  2443 : #define NV_PGRAPH_DMA_ADJ_OFFSET_VALUE                         31:0 /* RWXUF */
NV_PGRAPH_DMA_ADJ_OFFSET_VALUE          TEXTEQU         <31:0>
;  RW-4R 
;  2444 : #define NV_PGRAPH_DMA_OFFSET                             0x00401260 /* RW-4R */
NV_PGRAPH_DMA_OFFSET            EQU             000401260h
;  RW-UF 
;  2445 : #define NV_PGRAPH_DMA_OFFSET_VALUE                             31:0 /* RW-UF */
NV_PGRAPH_DMA_OFFSET_VALUE              TEXTEQU         <31:0>
;  RW-4R 
;  2446 : #define NV_PGRAPH_DMA_SIZE                               0x00401270 /* RW-4R */
NV_PGRAPH_DMA_SIZE              EQU             000401270h
;  RW-UF 
;  2447 : #define NV_PGRAPH_DMA_SIZE_VALUE                               18:0 /* RW-UF */
NV_PGRAPH_DMA_SIZE_VALUE                TEXTEQU         <18:0>
;  RW-4R 
;  2448 : #define NV_PGRAPH_DMA_XLATE_INST                         0x00401280 /* RW-4R */
NV_PGRAPH_DMA_XLATE_INST                EQU             000401280h
;  RWXUF 
;  2449 : #define NV_PGRAPH_DMA_XLATE_INST_VALUE                         15:0 /* RWXUF */
NV_PGRAPH_DMA_XLATE_INST_VALUE          TEXTEQU         <15:0>
;  RW-4R 
;  2450 : #define NV_PGRAPH_DMA_Y_SIZE                             0x00401290 /* RW-4R */
NV_PGRAPH_DMA_Y_SIZE            EQU             000401290h
;  RW-UF 
;  2451 : #define NV_PGRAPH_DMA_Y_SIZE_VALUE                             18:0 /* RW-UF */
NV_PGRAPH_DMA_Y_SIZE_VALUE              TEXTEQU         <18:0>
;  RW-4R 
;  2452 : #define NV_PGRAPH_DMA_LINEAR_LIMIT                       0x00401400 /* RW-4R */
NV_PGRAPH_DMA_LINEAR_LIMIT              EQU             000401400h
;  RWXUF 
;  2453 : #define NV_PGRAPH_DMA_LINEAR_LIMIT_VALUE                       21:0 /* RWXUF */
NV_PGRAPH_DMA_LINEAR_LIMIT_VALUE                TEXTEQU         <21:0>
;  RW-4A 
;        
;  2454 : #define NV_PGRAPH_DMA_START(i)                  (0x00401800+(i)*16) /* RW-4A */
;  2455 : #define NV_PGRAPH_DMA_START__SIZE_1                               3 /*       */
NV_PGRAPH_DMA_START__SIZE_1             EQU             3t
;  RWXUF 
;  2456 : #define NV_PGRAPH_DMA_START_VALUE                              31:0 /* RWXUF */
NV_PGRAPH_DMA_START_VALUE               TEXTEQU         <31:0>
;  RW-4R 
;  2457 : #define NV_PGRAPH_DMA_PITCH                              0x00401830 /* RW-4R */
NV_PGRAPH_DMA_PITCH             EQU             000401830h
;  RWXS4 
;  2458 : #define NV_PGRAPH_DMA_PITCH_0                                  15:0 /* RWXS4 */
NV_PGRAPH_DMA_PITCH_0           TEXTEQU         <15:0>
;  RWXS4 
;  2459 : #define NV_PGRAPH_DMA_PITCH_1                                 31:16 /* RWXS4 */
NV_PGRAPH_DMA_PITCH_1           TEXTEQU         <31:16>
;  RW-4R 
;  2460 : #define NV_PGRAPH_DMA_FORMAT                             0x00401840 /* RW-4R */
NV_PGRAPH_DMA_FORMAT            EQU             000401840h
;  RWXS4 
;  2461 : #define NV_PGRAPH_DMA_FORMAT_SRC                                2:0 /* RWXS4 */
NV_PGRAPH_DMA_FORMAT_SRC                TEXTEQU         <2:0>
;  RW--V 
;  2462 : #define NV_PGRAPH_DMA_FORMAT_SRC_INC_1                   0x00000001 /* RW--V */
NV_PGRAPH_DMA_FORMAT_SRC_INC_1          EQU             000000001h
;  RW--V 
;  2463 : #define NV_PGRAPH_DMA_FORMAT_SRC_INC_2                   0x00000002 /* RW--V */
NV_PGRAPH_DMA_FORMAT_SRC_INC_2          EQU             000000002h
;  RW--V 
;  2464 : #define NV_PGRAPH_DMA_FORMAT_SRC_INC_4                   0x00000004 /* RW--V */
NV_PGRAPH_DMA_FORMAT_SRC_INC_4          EQU             000000004h
;  RWXS4 
;  2465 : #define NV_PGRAPH_DMA_FORMAT_DST                               10:8 /* RWXS4 */
NV_PGRAPH_DMA_FORMAT_DST                TEXTEQU         <10:8>
;  RW--V 
;  2466 : #define NV_PGRAPH_DMA_FORMAT_DST_INC_1                   0x00000001 /* RW--V */
NV_PGRAPH_DMA_FORMAT_DST_INC_1          EQU             000000001h
;  RW--V 
;  2467 : #define NV_PGRAPH_DMA_FORMAT_DST_INC_2                   0x00000002 /* RW--V */
NV_PGRAPH_DMA_FORMAT_DST_INC_2          EQU             000000002h
;  RW--V 
;  2468 : #define NV_PGRAPH_DMA_FORMAT_DST_INC_4                   0x00000004 /* RW--V */
NV_PGRAPH_DMA_FORMAT_DST_INC_4          EQU             000000004h
;  RW-4R 
;  2469 : #define NV_PGRAPH_DMA_MTMF_NOTIFY                        0x00401820 /* RW-4R */
NV_PGRAPH_DMA_MTMF_NOTIFY               EQU             000401820h
;  RWIVF 
;  2470 : #define NV_PGRAPH_MTMF_NOTIFY_TYPE                             31:0 /* RWIVF */
NV_PGRAPH_MTMF_NOTIFY_TYPE              TEXTEQU         <31:0>
;  RWI-V 
;  2471 : #define NV_PGRAPH_MTMF_NOTIFY_TYPE_HW                    0x00000000 /* RWI-V */
NV_PGRAPH_MTMF_NOTIFY_TYPE_HW           EQU             000000000h
;  dev_video.ref 
;  RW--D 
;  2472 : /* dev_video.ref */
;  2473 : #define NV_PVIDEO                             0x006802FF:0x00680000 /* RW--D */
NV_PVIDEO               TEXTEQU         <0x006802FF:0x00680000>
;  RWI4R 
;  2474 : #define NV_PVIDEO_INTR_0                                 0x00680100 /* RWI4R */
NV_PVIDEO_INTR_0                EQU             000680100h
;  RWIVF 
;  2475 : #define NV_PVIDEO_INTR_0_NOTIFY                                 0:0 /* RWIVF */
NV_PVIDEO_INTR_0_NOTIFY         TEXTEQU         <0:0>
;  R-I-V 
;  2476 : #define NV_PVIDEO_INTR_0_NOTIFY_NOT_PENDING              0x00000000 /* R-I-V */
NV_PVIDEO_INTR_0_NOTIFY_NOT_PENDING             EQU             000000000h
;  R---V 
;  2477 : #define NV_PVIDEO_INTR_0_NOTIFY_PENDING                  0x00000001 /* R---V */
NV_PVIDEO_INTR_0_NOTIFY_PENDING         EQU             000000001h
;  -W--V 
;  2478 : #define NV_PVIDEO_INTR_0_NOTIFY_RESET                    0x00000001 /* -W--V */
NV_PVIDEO_INTR_0_NOTIFY_RESET           EQU             000000001h
;  RWI4R 
;  2479 : #define NV_PVIDEO_INTR_EN_0                              0x00680140 /* RWI4R */
NV_PVIDEO_INTR_EN_0             EQU             000680140h
;  RWIVF 
;  2480 : #define NV_PVIDEO_INTR_EN_0_NOTIFY                              0:0 /* RWIVF */
NV_PVIDEO_INTR_EN_0_NOTIFY              TEXTEQU         <0:0>
;  RWI-V 
;  2481 : #define NV_PVIDEO_INTR_EN_0_NOTIFY_DISABLED              0x00000000 /* RWI-V */
NV_PVIDEO_INTR_EN_0_NOTIFY_DISABLED             EQU             000000000h
;  RW--V 
;  2482 : #define NV_PVIDEO_INTR_EN_0_NOTIFY_ENABLED               0x00000001 /* RW--V */
NV_PVIDEO_INTR_EN_0_NOTIFY_ENABLED              EQU             000000001h
;  RW-4R 
;  2483 : #define NV_PVIDEO_STEP_SIZE                              0x00680200 /* RW-4R */
NV_PVIDEO_STEP_SIZE             EQU             000680200h
;  RWXVF 
;  2484 : #define NV_PVIDEO_STEP_SIZE_X                                  11:0 /* RWXVF */
NV_PVIDEO_STEP_SIZE_X           TEXTEQU         <11:0>
;  RWXVF 
;  2485 : #define NV_PVIDEO_STEP_SIZE_Y                                 27:16 /* RWXVF */
NV_PVIDEO_STEP_SIZE_Y           TEXTEQU         <27:16>
;  RW-4R 
;  2486 : #define NV_PVIDEO_CONTROL_Y                              0x00680204 /* RW-4R */
NV_PVIDEO_CONTROL_Y             EQU             000680204h
;  RW--F 
;  2487 : #define NV_PVIDEO_CONTROL_Y_BLUR                                0:0 /* RW--F */
NV_PVIDEO_CONTROL_Y_BLUR                TEXTEQU         <0:0>
;  RW--V 
;  2488 : #define NV_PVIDEO_CONTROL_Y_BLUR_OFF                     0x00000000 /* RW--V */
NV_PVIDEO_CONTROL_Y_BLUR_OFF            EQU             000000000h
;  RW--V 
;  2489 : #define NV_PVIDEO_CONTROL_Y_BLUR_ON                      0x00000001 /* RW--V */
NV_PVIDEO_CONTROL_Y_BLUR_ON             EQU             000000001h
;  RW--F 
;  2490 : #define NV_PVIDEO_CONTROL_Y_LINE                                4:4 /* RW--F */
NV_PVIDEO_CONTROL_Y_LINE                TEXTEQU         <4:4>
;  RW--V 
;  2491 : #define NV_PVIDEO_CONTROL_Y_LINE_HALF                    0x00000000 /* RW--V */
NV_PVIDEO_CONTROL_Y_LINE_HALF           EQU             000000000h
;  RW--V 
;  2492 : #define NV_PVIDEO_CONTROL_Y_LINE_FULL                    0x00000001 /* RW--V */
NV_PVIDEO_CONTROL_Y_LINE_FULL           EQU             000000001h
;  RW-4R 
;  2493 : #define NV_PVIDEO_CONTROL_X                              0x00680208 /* RW-4R */
NV_PVIDEO_CONTROL_X             EQU             000680208h
;  RW--F 
;  2494 : #define NV_PVIDEO_CONTROL_X_WEIGHT                              0:0 /* RW--F */
NV_PVIDEO_CONTROL_X_WEIGHT              TEXTEQU         <0:0>
;  RW--V 
;  2495 : #define NV_PVIDEO_CONTROL_X_WEIGHT_LIGHT                 0x00000000 /* RW--V */
NV_PVIDEO_CONTROL_X_WEIGHT_LIGHT                EQU             000000000h
;  RW--V 
;  2496 : #define NV_PVIDEO_CONTROL_X_WEIGHT_HEAVY                 0x00000001 /* RW--V */
NV_PVIDEO_CONTROL_X_WEIGHT_HEAVY                EQU             000000001h
;  RW--F 
;  2497 : #define NV_PVIDEO_CONTROL_X_SHARPENING                          4:4 /* RW--F */
NV_PVIDEO_CONTROL_X_SHARPENING          TEXTEQU         <4:4>
;  RW--V 
;  2498 : #define NV_PVIDEO_CONTROL_X_SHARPENING_OFF               0x00000000 /* RW--V */
NV_PVIDEO_CONTROL_X_SHARPENING_OFF              EQU             000000000h
;  RW--V 
;  2499 : #define NV_PVIDEO_CONTROL_X_SHARPENING_ON                0x00000001 /* RW--V */
NV_PVIDEO_CONTROL_X_SHARPENING_ON               EQU             000000001h
;  RW--F 
;  2500 : #define NV_PVIDEO_CONTROL_X_SMOOTHING                           8:8 /* RW--F */
NV_PVIDEO_CONTROL_X_SMOOTHING           TEXTEQU         <8:8>
;  RW--V 
;  2501 : #define NV_PVIDEO_CONTROL_X_SMOOTHING_OFF                0x00000000 /* RW--V */
NV_PVIDEO_CONTROL_X_SMOOTHING_OFF               EQU             000000000h
;  RW--V 
;  2502 : #define NV_PVIDEO_CONTROL_X_SMOOTHING_ON                 0x00000001 /* RW--V */
NV_PVIDEO_CONTROL_X_SMOOTHING_ON                EQU             000000001h
;  RW-4R 
;  2503 : #define NV_PVIDEO_BUFF0_START                            0x0068020c /* RW-4R */
NV_PVIDEO_BUFF0_START           EQU             00068020ch
;  RW-UF 
;  2504 : #define NV_PVIDEO_BUFF0_START_ADDRESS                          22:2 /* RW-UF */
NV_PVIDEO_BUFF0_START_ADDRESS           TEXTEQU         <22:2>
;  RW-4R 
;  2505 : #define NV_PVIDEO_BUFF1_START                            0x00680210 /* RW-4R */
NV_PVIDEO_BUFF1_START           EQU             000680210h
;  RW-UF 
;  2506 : #define NV_PVIDEO_BUFF1_START_ADDRESS                          22:2 /* RW-UF */
NV_PVIDEO_BUFF1_START_ADDRESS           TEXTEQU         <22:2>
;  RW-4R 
;  2507 : #define NV_PVIDEO_BUFF0_PITCH                            0x00680214 /* RW-4R */
NV_PVIDEO_BUFF0_PITCH           EQU             000680214h
;  RW-UF 
;  2508 : #define NV_PVIDEO_BUFF0_PITCH_LENGTH                           11:2 /* RW-UF */
NV_PVIDEO_BUFF0_PITCH_LENGTH            TEXTEQU         <11:2>
;  RW-4R 
;  2509 : #define NV_PVIDEO_BUFF1_PITCH                            0x00680218 /* RW-4R */
NV_PVIDEO_BUFF1_PITCH           EQU             000680218h
;  RW-UF 
;  2510 : #define NV_PVIDEO_BUFF1_PITCH_LENGTH                           11:2 /* RW-UF */
NV_PVIDEO_BUFF1_PITCH_LENGTH            TEXTEQU         <11:2>
;  RW-4R 
;  2511 : #define NV_PVIDEO_BUFF0_OFFSET                           0x0068021c /* RW-4R */
NV_PVIDEO_BUFF0_OFFSET          EQU             00068021ch
;  RW--F 
;  2512 : #define NV_PVIDEO_BUFF0_OFFSET_X                                0:0 /* RW--F */
NV_PVIDEO_BUFF0_OFFSET_X                TEXTEQU         <0:0>
;  RW--V 
;  2513 : #define NV_PVIDEO_BUFF0_OFFSET_X_OFF                     0x00000000 /* RW--V */
NV_PVIDEO_BUFF0_OFFSET_X_OFF            EQU             000000000h
;  RW--V 
;  2514 : #define NV_PVIDEO_BUFF0_OFFSET_X_ON                      0x00000001 /* RW--V */
NV_PVIDEO_BUFF0_OFFSET_X_ON             EQU             000000001h
;  RW--F 
;  2515 : #define NV_PVIDEO_BUFF0_OFFSET_Y                                5:4 /* RW--F */
NV_PVIDEO_BUFF0_OFFSET_Y                TEXTEQU         <5:4>
;  RW--V 
;  2516 : #define NV_PVIDEO_BUFF0_OFFSET_Y_OFF                     0x00000000 /* RW--V */
NV_PVIDEO_BUFF0_OFFSET_Y_OFF            EQU             000000000h
;  RW--V 
;  2517 : #define NV_PVIDEO_BUFF0_OFFSET_Y_QUARTER                 0x00000001 /* RW--V */
NV_PVIDEO_BUFF0_OFFSET_Y_QUARTER                EQU             000000001h
;  RW--V 
;  2518 : #define NV_PVIDEO_BUFF0_OFFSET_Y_HALF                    0x00000002 /* RW--V */
NV_PVIDEO_BUFF0_OFFSET_Y_HALF           EQU             000000002h
;  RW-4R 
;  2519 : #define NV_PVIDEO_BUFF1_OFFSET                           0x00680220 /* RW-4R */
NV_PVIDEO_BUFF1_OFFSET          EQU             000680220h
;  RW--F 
;  2520 : #define NV_PVIDEO_BUFF1_OFFSET_X                                0:0 /* RW--F */
NV_PVIDEO_BUFF1_OFFSET_X                TEXTEQU         <0:0>
;  RW--V 
;  2521 : #define NV_PVIDEO_BUFF1_OFFSET_X_OFF                     0x00000000 /* RW--V */
NV_PVIDEO_BUFF1_OFFSET_X_OFF            EQU             000000000h
;  RW--V 
;  2522 : #define NV_PVIDEO_BUFF1_OFFSET_X_ON                      0x00000001 /* RW--V */
NV_PVIDEO_BUFF1_OFFSET_X_ON             EQU             000000001h
;  RW--F 
;  2523 : #define NV_PVIDEO_BUFF1_OFFSET_Y                                5:4 /* RW--F */
NV_PVIDEO_BUFF1_OFFSET_Y                TEXTEQU         <5:4>
;  RW--V 
;  2524 : #define NV_PVIDEO_BUFF1_OFFSET_Y_OFF                     0x00000000 /* RW--V */
NV_PVIDEO_BUFF1_OFFSET_Y_OFF            EQU             000000000h
;  RW--V 
;  2525 : #define NV_PVIDEO_BUFF1_OFFSET_Y_QUARTER                 0x00000001 /* RW--V */
NV_PVIDEO_BUFF1_OFFSET_Y_QUARTER                EQU             000000001h
;  RW--V 
;  2526 : #define NV_PVIDEO_BUFF1_OFFSET_Y_HALF                    0x00000002 /* RW--V */
NV_PVIDEO_BUFF1_OFFSET_Y_HALF           EQU             000000002h
;  RW-4R 
;  2527 : #define NV_PVIDEO_OE_STATE                               0x00680224 /* RW-4R */
NV_PVIDEO_OE_STATE              EQU             000680224h
;  RWXVF 
;  2528 : #define NV_PVIDEO_OE_STATE_BUFF0_INTR_NOTIFY                    0:0 /* RWXVF */
NV_PVIDEO_OE_STATE_BUFF0_INTR_NOTIFY            TEXTEQU         <0:0>
;  RWXVF 
;  2529 : #define NV_PVIDEO_OE_STATE_BUFF1_INTR_NOTIFY                    4:4 /* RWXVF */
NV_PVIDEO_OE_STATE_BUFF1_INTR_NOTIFY            TEXTEQU         <4:4>
;  RWXVF 
;  2530 : #define NV_PVIDEO_OE_STATE_BUFF0_ERROR                          8:8 /* RWXVF */
NV_PVIDEO_OE_STATE_BUFF0_ERROR          TEXTEQU         <8:8>
;  RWXVF 
;  2531 : #define NV_PVIDEO_OE_STATE_BUFF1_ERROR                        12:12 /* RWXVF */
NV_PVIDEO_OE_STATE_BUFF1_ERROR          TEXTEQU         <12:12>
;  RWXVF 
;  2532 : #define NV_PVIDEO_OE_STATE_BUFF0_IN_USE                       16:16 /* RWXVF */
NV_PVIDEO_OE_STATE_BUFF0_IN_USE         TEXTEQU         <16:16>
;  RWXVF 
;  2533 : #define NV_PVIDEO_OE_STATE_BUFF1_IN_USE                       20:20 /* RWXVF */
NV_PVIDEO_OE_STATE_BUFF1_IN_USE         TEXTEQU         <20:20>
;  RWXVF 
;  2534 : #define NV_PVIDEO_OE_STATE_CURRENT_BUFFER                     24:24 /* RWXVF */
NV_PVIDEO_OE_STATE_CURRENT_BUFFER               TEXTEQU         <24:24>
;  RW--V 
;  2535 : #define NV_PVIDEO_OE_STATE_CURRENT_BUFFER_0              0x00000000 /* RW--V */
NV_PVIDEO_OE_STATE_CURRENT_BUFFER_0             EQU             000000000h
;  RW--V 
;  2536 : #define NV_PVIDEO_OE_STATE_CURRENT_BUFFER_1              0x00000001 /* RW--V */
NV_PVIDEO_OE_STATE_CURRENT_BUFFER_1             EQU             000000001h
;  RW-4R 
;  2537 : #define NV_PVIDEO_SU_STATE                               0x00680228 /* RW-4R */
NV_PVIDEO_SU_STATE              EQU             000680228h
;  RWXVF 
;  2538 : #define NV_PVIDEO_SU_STATE_BUFF0_IN_USE                       16:16 /* RWXVF */
NV_PVIDEO_SU_STATE_BUFF0_IN_USE         TEXTEQU         <16:16>
;  RWXVF 
;  2539 : #define NV_PVIDEO_SU_STATE_BUFF1_IN_USE                       20:20 /* RWXVF */
NV_PVIDEO_SU_STATE_BUFF1_IN_USE         TEXTEQU         <20:20>
;  RW-4R 
;  2540 : #define NV_PVIDEO_RM_STATE                               0x0068022c /* RW-4R */
NV_PVIDEO_RM_STATE              EQU             00068022ch
;  RWXVF 
;  2541 : #define NV_PVIDEO_RM_STATE_BUFF0_INTR_NOTIFY                    0:0 /* RWXVF */
NV_PVIDEO_RM_STATE_BUFF0_INTR_NOTIFY            TEXTEQU         <0:0>
;  RWXVF 
;  2542 : #define NV_PVIDEO_RM_STATE_BUFF1_INTR_NOTIFY                    4:4 /* RWXVF */
NV_PVIDEO_RM_STATE_BUFF1_INTR_NOTIFY            TEXTEQU         <4:4>
;  RW-4R 
;  2543 : #define NV_PVIDEO_WINDOW_START                           0x00680230 /* RW-4R */
NV_PVIDEO_WINDOW_START          EQU             000680230h
;  RWXUF 
;  2544 : #define NV_PVIDEO_WINDOW_START_X                               10:0 /* RWXUF */
NV_PVIDEO_WINDOW_START_X                TEXTEQU         <10:0>
;  RWXUF 
;  2545 : #define NV_PVIDEO_WINDOW_START_Y                              26:16 /* RWXUF */
NV_PVIDEO_WINDOW_START_Y                TEXTEQU         <26:16>
;  RW-4R 
;  2546 : #define NV_PVIDEO_WINDOW_SIZE                            0x00680234 /* RW-4R */
NV_PVIDEO_WINDOW_SIZE           EQU             000680234h
;  RWXUF 
;  2547 : #define NV_PVIDEO_WINDOW_SIZE_X                                10:0 /* RWXUF */
NV_PVIDEO_WINDOW_SIZE_X         TEXTEQU         <10:0>
;  RWXUF 
;  2548 : #define NV_PVIDEO_WINDOW_SIZE_Y                               26:16 /* RWXUF */
NV_PVIDEO_WINDOW_SIZE_Y         TEXTEQU         <26:16>
;  RW-4R 
;  2549 : #define NV_PVIDEO_FIFO_THRES                             0x00680238 /* RW-4R */
NV_PVIDEO_FIFO_THRES            EQU             000680238h
;  RW--F 
;  2550 : #define NV_PVIDEO_FIFO_THRES_SIZE                               7:3 /* RW--F */
NV_PVIDEO_FIFO_THRES_SIZE               TEXTEQU         <7:3>
;  RW-4R 
;  2551 : #define NV_PVIDEO_FIFO_BURST                             0x0068023c /* RW-4R */
NV_PVIDEO_FIFO_BURST            EQU             00068023ch
;  RW--F 
;  2552 : #define NV_PVIDEO_FIFO_BURST_LENGTH                             1:0 /* RW--F */
NV_PVIDEO_FIFO_BURST_LENGTH             TEXTEQU         <1:0>
;  RW--V 
;  2553 : #define NV_PVIDEO_FIFO_BURST_LENGTH_32                   0x00000001 /* RW--V */
NV_PVIDEO_FIFO_BURST_LENGTH_32          EQU             000000001h
;  RW--V 
;  2554 : #define NV_PVIDEO_FIFO_BURST_LENGTH_64                   0x00000002 /* RW--V */
NV_PVIDEO_FIFO_BURST_LENGTH_64          EQU             000000002h
;  RW--V 
;  2555 : #define NV_PVIDEO_FIFO_BURST_LENGTH_128                  0x00000003 /* RW--V */
NV_PVIDEO_FIFO_BURST_LENGTH_128         EQU             000000003h
;  RW-4R 
;  2556 : #define NV_PVIDEO_KEY                                    0x00680240 /* RW-4R */
NV_PVIDEO_KEY           EQU             000680240h
;  RW-VF 
;  2557 : #define NV_PVIDEO_KEY_INDEX                                     7:0 /* RW-VF */
NV_PVIDEO_KEY_INDEX             TEXTEQU         <7:0>
;  RW-VF 
;  2558 : #define NV_PVIDEO_KEY_565                                      15:0 /* RW-VF */
NV_PVIDEO_KEY_565               TEXTEQU         <15:0>
;  RW-VF 
;  2559 : #define NV_PVIDEO_KEY_555                                      14:0 /* RW-VF */
NV_PVIDEO_KEY_555               TEXTEQU         <14:0>
;  RW-VF 
;  2560 : #define NV_PVIDEO_KEY_888                                      23:0 /* RW-VF */
NV_PVIDEO_KEY_888               TEXTEQU         <23:0>
;  RW-VF 
;  2561 : #define NV_PVIDEO_KEY_PACK                                    31:24 /* RW-VF */
NV_PVIDEO_KEY_PACK              TEXTEQU         <31:24>
;  RWI4R 
;  2562 : #define NV_PVIDEO_OVERLAY                                0x00680244 /* RWI4R */
NV_PVIDEO_OVERLAY               EQU             000680244h
;  RWI-F 
;  2563 : #define NV_PVIDEO_OVERLAY_VIDEO                                 0:0 /* RWI-F */
NV_PVIDEO_OVERLAY_VIDEO         TEXTEQU         <0:0>
;  RWI-V 
;  2564 : #define NV_PVIDEO_OVERLAY_VIDEO_OFF                      0x00000000 /* RWI-V */
NV_PVIDEO_OVERLAY_VIDEO_OFF             EQU             000000000h
;  RW--V 
;  2565 : #define NV_PVIDEO_OVERLAY_VIDEO_ON                       0x00000001 /* RW--V */
NV_PVIDEO_OVERLAY_VIDEO_ON              EQU             000000001h
;  RW--F 
;  2566 : #define NV_PVIDEO_OVERLAY_KEY                                   4:4 /* RW--F */
NV_PVIDEO_OVERLAY_KEY           TEXTEQU         <4:4>
;  RW--V 
;  2567 : #define NV_PVIDEO_OVERLAY_KEY_OFF                        0x00000000 /* RW--V */
NV_PVIDEO_OVERLAY_KEY_OFF               EQU             000000000h
;  RW--V 
;  2568 : #define NV_PVIDEO_OVERLAY_KEY_ON                         0x00000001 /* RW--V */
NV_PVIDEO_OVERLAY_KEY_ON                EQU             000000001h
;  RW--F 
;  2569 : #define NV_PVIDEO_OVERLAY_FORMAT                                8:8 /* RW--F */
NV_PVIDEO_OVERLAY_FORMAT                TEXTEQU         <8:8>
;  RW--V 
;  2570 : #define NV_PVIDEO_OVERLAY_FORMAT_CCIR                    0x00000000 /* RW--V */
NV_PVIDEO_OVERLAY_FORMAT_CCIR           EQU             000000000h
;  RW--V 
;  2571 : #define NV_PVIDEO_OVERLAY_FORMAT_YUY2                    0x00000001 /* RW--V */
NV_PVIDEO_OVERLAY_FORMAT_YUY2           EQU             000000001h
;  RW-4R 
;  2572 : #define NV_PVIDEO_RED_CSC                                0x00680280 /* RW-4R */
NV_PVIDEO_RED_CSC               EQU             000680280h
;  RWX-F 
;  2573 : #define NV_PVIDEO_RED_CSC_OFFSET                                7:0 /* RWX-F */
NV_PVIDEO_RED_CSC_OFFSET                TEXTEQU         <7:0>
;  RW-4R 
;  2574 : #define NV_PVIDEO_GREEN_CSC                              0x00680284 /* RW-4R */
NV_PVIDEO_GREEN_CSC             EQU             000680284h
;  RWX-F 
;  2575 : #define NV_PVIDEO_GREEN_CSC_OFFSET                              7:0 /* RWX-F */
NV_PVIDEO_GREEN_CSC_OFFSET              TEXTEQU         <7:0>
;  RW-4R 
;  2576 : #define NV_PVIDEO_BLUE_CSC                               0x00680288 /* RW-4R */
NV_PVIDEO_BLUE_CSC              EQU             000680288h
;  RWX-F 
;  2577 : #define NV_PVIDEO_BLUE_CSC_OFFSET                               7:0 /* RWX-F */
NV_PVIDEO_BLUE_CSC_OFFSET               TEXTEQU         <7:0>
;  RW-4R 
;  2578 : #define NV_PVIDEO_CSC_ADJUST                             0x0068028c /* RW-4R */
NV_PVIDEO_CSC_ADJUST            EQU             00068028ch
;  RW--F 
;  2579 : #define NV_PVIDEO_CSC_ADJUST_B_FLAG                             0:0 /* RW--F */
NV_PVIDEO_CSC_ADJUST_B_FLAG             TEXTEQU         <0:0>
;  RW--V 
;  2580 : #define NV_PVIDEO_CSC_ADJUST_B_FLAG_OFF                  0x00000000 /* RW--V */
NV_PVIDEO_CSC_ADJUST_B_FLAG_OFF         EQU             000000000h
;  RW--V 
;  2581 : #define NV_PVIDEO_CSC_ADJUST_B_FLAG_ON                   0x00000001 /* RW--V */
NV_PVIDEO_CSC_ADJUST_B_FLAG_ON          EQU             000000001h
;  RW--F 
;  2582 : #define NV_PVIDEO_CSC_ADJUST_G_FLAG                             4:4 /* RW--F */
NV_PVIDEO_CSC_ADJUST_G_FLAG             TEXTEQU         <4:4>
;  RW--V 
;  2583 : #define NV_PVIDEO_CSC_ADJUST_G_FLAG_OFF                  0x00000000 /* RW--V */
NV_PVIDEO_CSC_ADJUST_G_FLAG_OFF         EQU             000000000h
;  RW--V 
;  2584 : #define NV_PVIDEO_CSC_ADJUST_G_FLAG_ON                   0x00000001 /* RW--V */
NV_PVIDEO_CSC_ADJUST_G_FLAG_ON          EQU             000000001h
;  RW--F 
;  2585 : #define NV_PVIDEO_CSC_ADJUST_R_FLAG                             8:8 /* RW--F */
NV_PVIDEO_CSC_ADJUST_R_FLAG             TEXTEQU         <8:8>
;  RW--V 
;  2586 : #define NV_PVIDEO_CSC_ADJUST_R_FLAG_OFF                  0x00000000 /* RW--V */
NV_PVIDEO_CSC_ADJUST_R_FLAG_OFF         EQU             000000000h
;  RW--V 
;  2587 : #define NV_PVIDEO_CSC_ADJUST_R_FLAG_ON                   0x00000001 /* RW--V */
NV_PVIDEO_CSC_ADJUST_R_FLAG_ON          EQU             000000001h
;  RW--F 
;  2588 : #define NV_PVIDEO_CSC_ADJUST_L_FLAG                           12:12 /* RW--F */
NV_PVIDEO_CSC_ADJUST_L_FLAG             TEXTEQU         <12:12>
;  RW--V 
;  2589 : #define NV_PVIDEO_CSC_ADJUST_L_FLAG_OFF                  0x00000000 /* RW--V */
NV_PVIDEO_CSC_ADJUST_L_FLAG_OFF         EQU             000000000h
;  RW--V 
;  2590 : #define NV_PVIDEO_CSC_ADJUST_L_FLAG_ON                   0x00000001 /* RW--V */
NV_PVIDEO_CSC_ADJUST_L_FLAG_ON          EQU             000000001h
;  RW--F 
;  2591 : #define NV_PVIDEO_CSC_ADJUST_CHROMA                           16:16 /* RW--F */
NV_PVIDEO_CSC_ADJUST_CHROMA             TEXTEQU         <16:16>
;  RW--V 
;  2592 : #define NV_PVIDEO_CSC_ADJUST_CHROMA_OFF                  0x00000000 /* RW--V */
NV_PVIDEO_CSC_ADJUST_CHROMA_OFF         EQU             000000000h
;  RW--V 
;  2593 : #define NV_PVIDEO_CSC_ADJUST_CHROMA_ON                   0x00000001 /* RW--V */
NV_PVIDEO_CSC_ADJUST_CHROMA_ON          EQU             000000001h
;  vga.ref 
;  RW--D 
;  2594 : /* vga.ref */
;  2595 : #define NV_PRMCIO                             0x00601FFF:0x00601000 /* RW--D */
NV_PRMCIO               TEXTEQU         <0x00601FFF:0x00601000>
;  R--1R 
;  2596 : #define NV_PRMCIO_INP0                                   0x006013c2 /* R--1R */
NV_PRMCIO_INP0          EQU             0006013c2h
;  R--1R 
;  2597 : #define NV_PRMCIO_INP0__MONO                             0x006013ba /* R--1R */
NV_PRMCIO_INP0__MONO            EQU             0006013bah
;  R--1R 
;  2598 : #define NV_PRMCIO_INP0__COLOR                            0x006013da /* R--1R */
NV_PRMCIO_INP0__COLOR           EQU             0006013dah
;  R--1R 
;  2599 : #define NV_PRMCIO_INP0__READ_MONO                        0x006013ca /* R--1R */
NV_PRMCIO_INP0__READ_MONO               EQU             0006013cah
;  -W-1R 
;  2600 : #define NV_PRMCIO_INP0__WRITE_MONO                       0x006013ba /* -W-1R */
NV_PRMCIO_INP0__WRITE_MONO              EQU             0006013bah
;  -W-1R 
;  2601 : #define NV_PRMCIO_INP0__WRITE_COLOR                      0x006013da /* -W-1R */
NV_PRMCIO_INP0__WRITE_COLOR             EQU             0006013dah
;  RW-1R 
;  2602 : #define NV_PRMCIO_ARX                                    0x006013c0 /* RW-1R */
NV_PRMCIO_ARX           EQU             0006013c0h
;  -W-1R 
;  2603 : #define NV_PRMCIO_AR_PALETTE__WRITE                      0x006013c0 /* -W-1R */
NV_PRMCIO_AR_PALETTE__WRITE             EQU             0006013c0h
;  R--1R 
;  2604 : #define NV_PRMCIO_AR_PALETTE__READ                       0x006013c1 /* R--1R */
NV_PRMCIO_AR_PALETTE__READ              EQU             0006013c1h
;  -W-1R 
;  2605 : #define NV_PRMCIO_AR_MODE__WRITE                         0x006013c0 /* -W-1R */
NV_PRMCIO_AR_MODE__WRITE                EQU             0006013c0h
;  R--1R 
;  2606 : #define NV_PRMCIO_AR_MODE__READ                          0x006013c1 /* R--1R */
NV_PRMCIO_AR_MODE__READ         EQU             0006013c1h
;        
;  2607 : #define NV_PRMCIO_AR_MODE_INDEX                          0x00000010 /*       */
NV_PRMCIO_AR_MODE_INDEX         EQU             000000010h
;  -W-1R 
;  2608 : #define NV_PRMCIO_AR_OSCAN__WRITE                        0x006013c0 /* -W-1R */
NV_PRMCIO_AR_OSCAN__WRITE               EQU             0006013c0h
;  R--1R 
;  2609 : #define NV_PRMCIO_AR_OSCAN__READ                         0x006013c1 /* R--1R */
NV_PRMCIO_AR_OSCAN__READ                EQU             0006013c1h
;        
;  2610 : #define NV_PRMCIO_AR_OSCAN_INDEX                         0x00000011 /*       */
NV_PRMCIO_AR_OSCAN_INDEX                EQU             000000011h
;  -W-1R 
;  2611 : #define NV_PRMCIO_AR_PLANE__WRITE                        0x006013c0 /* -W-1R */
NV_PRMCIO_AR_PLANE__WRITE               EQU             0006013c0h
;  R--1R 
;  2612 : #define NV_PRMCIO_AR_PLANE__READ                         0x006013c1 /* R--1R */
NV_PRMCIO_AR_PLANE__READ                EQU             0006013c1h
;        
;  2613 : #define NV_PRMCIO_AR_PLANE_INDEX                         0x00000012 /*       */
NV_PRMCIO_AR_PLANE_INDEX                EQU             000000012h
;  -W-1R 
;  2614 : #define NV_PRMCIO_AR_HPP__WRITE                          0x006013c0 /* -W-1R */
NV_PRMCIO_AR_HPP__WRITE         EQU             0006013c0h
;  R--1R 
;  2615 : #define NV_PRMCIO_AR_HPP__READ                           0x006013c1 /* R--1R */
NV_PRMCIO_AR_HPP__READ          EQU             0006013c1h
;        
;  2616 : #define NV_PRMCIO_AR_HPP_INDEX                           0x00000013 /*       */
NV_PRMCIO_AR_HPP_INDEX          EQU             000000013h
;  -W-1R 
;  2617 : #define NV_PRMCIO_AR_CSEL__WRITE                         0x006013c0 /* -W-1R */
NV_PRMCIO_AR_CSEL__WRITE                EQU             0006013c0h
;  R--1R 
;  2618 : #define NV_PRMCIO_AR_CSEL__READ                          0x006013c1 /* R--1R */
NV_PRMCIO_AR_CSEL__READ         EQU             0006013c1h
;        
;  2619 : #define NV_PRMCIO_AR_CSEL_INDEX                          0x00000014 /*       */
NV_PRMCIO_AR_CSEL_INDEX         EQU             000000014h
;  RW-1R 
;  2620 : #define NV_PRMCIO_CRX__MONO                              0x006013b4 /* RW-1R */
NV_PRMCIO_CRX__MONO             EQU             0006013b4h
;  RW-1R 
;  2621 : #define NV_PRMCIO_CRX__COLOR                             0x006013d4 /* RW-1R */
NV_PRMCIO_CRX__COLOR            EQU             0006013d4h
;  RW-1R 
;  2622 : #define NV_PRMCIO_CR__MONO                               0x006013b5 /* RW-1R */
NV_PRMCIO_CR__MONO              EQU             0006013b5h
;  RW-1R 
;  2623 : #define NV_PRMCIO_CR__COLOR                              0x006013d5 /* RW-1R */
NV_PRMCIO_CR__COLOR             EQU             0006013d5h
;  RW-1R 
;  2624 : #define NV_PRMCIO_CRE__MONO                              0x006013b5 /* RW-1R */
NV_PRMCIO_CRE__MONO             EQU             0006013b5h
;  RW-1R 
;  2625 : #define NV_PRMCIO_CRE__COLOR                             0x006013d5 /* RW-1R */
NV_PRMCIO_CRE__COLOR            EQU             0006013d5h
;  vga.ref 
;  ----- 
;  2626 : /* vga.ref */
;  2627 : #define NV_CIO                                          0x3DF:0x3B0 /* ----- */
NV_CIO          TEXTEQU         <0x3DF:0x3B0>
;  R--1R 
;  2628 : #define NV_CIO_INP0                                      0x000003c2 /* R--1R */
NV_CIO_INP0             EQU             0000003c2h
;  R--1R 
;  2629 : #define NV_CIO_INP0__MONO                                0x000003ba /* R--1R */
NV_CIO_INP0__MONO               EQU             0000003bah
;  R--1R 
;  2630 : #define NV_CIO_INP0__COLOR                               0x000003da /* R--1R */
NV_CIO_INP0__COLOR              EQU             0000003dah
;  R--1R 
;  2631 : #define NV_CIO_INP0__READ_MONO                           0x000003ca /* R--1R */
NV_CIO_INP0__READ_MONO          EQU             0000003cah
;  -W-1R 
;  2632 : #define NV_CIO_INP0__WRITE_MONO                          0x000003ba /* -W-1R */
NV_CIO_INP0__WRITE_MONO         EQU             0000003bah
;  -W-1R 
;  2633 : #define NV_CIO_INP0__WRITE_COLOR                         0x000003da /* -W-1R */
NV_CIO_INP0__WRITE_COLOR                EQU             0000003dah
;  RW-1R 
;  2634 : #define NV_CIO_ARX                                       0x000003c0 /* RW-1R */
NV_CIO_ARX              EQU             0000003c0h
;  -W-1R 
;  2635 : #define NV_CIO_AR_PALETTE__WRITE                         0x000003c0 /* -W-1R */
NV_CIO_AR_PALETTE__WRITE                EQU             0000003c0h
;  R--1R 
;  2636 : #define NV_CIO_AR_PALETTE__READ                          0x000003c1 /* R--1R */
NV_CIO_AR_PALETTE__READ         EQU             0000003c1h
;  -W-1R 
;  2637 : #define NV_CIO_AR_MODE__WRITE                            0x000003c0 /* -W-1R */
NV_CIO_AR_MODE__WRITE           EQU             0000003c0h
;  R--1R 
;  2638 : #define NV_CIO_AR_MODE__READ                             0x000003c1 /* R--1R */
NV_CIO_AR_MODE__READ            EQU             0000003c1h
;        
;  2639 : #define NV_CIO_AR_MODE_INDEX                             0x00000010 /*       */
NV_CIO_AR_MODE_INDEX            EQU             000000010h
;  -W-1R 
;  2640 : #define NV_CIO_AR_OSCAN__WRITE                           0x000003c0 /* -W-1R */
NV_CIO_AR_OSCAN__WRITE          EQU             0000003c0h
;  R--1R 
;  2641 : #define NV_CIO_AR_OSCAN__READ                            0x000003c1 /* R--1R */
NV_CIO_AR_OSCAN__READ           EQU             0000003c1h
;        
;  2642 : #define NV_CIO_AR_OSCAN_INDEX                            0x00000011 /*       */
NV_CIO_AR_OSCAN_INDEX           EQU             000000011h
;  -W-1R 
;  2643 : #define NV_CIO_AR_PLANE__WRITE                           0x000003c0 /* -W-1R */
NV_CIO_AR_PLANE__WRITE          EQU             0000003c0h
;  R--1R 
;  2644 : #define NV_CIO_AR_PLANE__READ                            0x000003c1 /* R--1R */
NV_CIO_AR_PLANE__READ           EQU             0000003c1h
;        
;  2645 : #define NV_CIO_AR_PLANE_INDEX                            0x00000012 /*       */
NV_CIO_AR_PLANE_INDEX           EQU             000000012h
;  -W-1R 
;  2646 : #define NV_CIO_AR_HPP__WRITE                             0x000003c0 /* -W-1R */
NV_CIO_AR_HPP__WRITE            EQU             0000003c0h
;  R--1R 
;  2647 : #define NV_CIO_AR_HPP__READ                              0x000003c1 /* R--1R */
NV_CIO_AR_HPP__READ             EQU             0000003c1h
;        
;  2648 : #define NV_CIO_AR_HPP_INDEX                              0x00000013 /*       */
NV_CIO_AR_HPP_INDEX             EQU             000000013h
;  -W-1R 
;  2649 : #define NV_CIO_AR_CSEL__WRITE                            0x000003c0 /* -W-1R */
NV_CIO_AR_CSEL__WRITE           EQU             0000003c0h
;  R--1R 
;  2650 : #define NV_CIO_AR_CSEL__READ                             0x000003c1 /* R--1R */
NV_CIO_AR_CSEL__READ            EQU             0000003c1h
;        
;  2651 : #define NV_CIO_AR_CSEL_INDEX                             0x00000014 /*       */
NV_CIO_AR_CSEL_INDEX            EQU             000000014h
;  RW-1R 
;  2652 : #define NV_CIO_CRX__MONO                                 0x000003b4 /* RW-1R */
NV_CIO_CRX__MONO                EQU             0000003b4h
;  RW-1R 
;  2653 : #define NV_CIO_CRX__COLOR                                0x000003d4 /* RW-1R */
NV_CIO_CRX__COLOR               EQU             0000003d4h
;  RW-1R 
;  2654 : #define NV_CIO_CR__MONO                                  0x000003b5 /* RW-1R */
NV_CIO_CR__MONO         EQU             0000003b5h
;  RW-1R 
;  2655 : #define NV_CIO_CR__COLOR                                 0x000003d5 /* RW-1R */
NV_CIO_CR__COLOR                EQU             0000003d5h
;        
;  2656 : #define NV_CIO_CR_HDT_INDEX                              0x00000000 /*       */
NV_CIO_CR_HDT_INDEX             EQU             000000000h
;        
;  2657 : #define NV_CIO_CR_HDE_INDEX                              0x00000001 /*       */
NV_CIO_CR_HDE_INDEX             EQU             000000001h
;        
;  2658 : #define NV_CIO_CR_HBS_INDEX                              0x00000002 /*       */
NV_CIO_CR_HBS_INDEX             EQU             000000002h
;        
;  2659 : #define NV_CIO_CR_HBE_INDEX                              0x00000003 /*       */
NV_CIO_CR_HBE_INDEX             EQU             000000003h
;  RW--F 
;  2660 : #define NV_CIO_CR_HBE_4_0                                       4:0 /* RW--F */
NV_CIO_CR_HBE_4_0               TEXTEQU         <4:0>
;        
;  2661 : #define NV_CIO_CR_HRS_INDEX                              0x00000004 /*       */
NV_CIO_CR_HRS_INDEX             EQU             000000004h
;        
;  2662 : #define NV_CIO_CR_HRE_INDEX                              0x00000005 /*       */
NV_CIO_CR_HRE_INDEX             EQU             000000005h
;  RW--F 
;  2663 : #define NV_CIO_CR_HRE_HBE_5                                     7:7 /* RW--F */
NV_CIO_CR_HRE_HBE_5             TEXTEQU         <7:7>
;  RW--F 
;  2664 : #define NV_CIO_CR_HRE_4_0                                       4:0 /* RW--F */
NV_CIO_CR_HRE_4_0               TEXTEQU         <4:0>
;        
;  2665 : #define NV_CIO_CR_VDT_INDEX                              0x00000006 /*       */
NV_CIO_CR_VDT_INDEX             EQU             000000006h
;        
;  2666 : #define NV_CIO_CR_OVL_INDEX                              0x00000007 /*       */
NV_CIO_CR_OVL_INDEX             EQU             000000007h
;  RW--F 
;  2667 : #define NV_CIO_CR_OVL_VDE_8                                     1:1 /* RW--F */
NV_CIO_CR_OVL_VDE_8             TEXTEQU         <1:1>
;  RW--F 
;  2668 : #define NV_CIO_CR_OVL_VDE_9                                     6:6 /* RW--F */
NV_CIO_CR_OVL_VDE_9             TEXTEQU         <6:6>
;  RW--F 
;  2669 : #define NV_CIO_CR_OVL_VDT_8                                     0:0 /* RW--F */
NV_CIO_CR_OVL_VDT_8             TEXTEQU         <0:0>
;  RW--F 
;  2670 : #define NV_CIO_CR_OVL_VDT_9                                     5:5 /* RW--F */
NV_CIO_CR_OVL_VDT_9             TEXTEQU         <5:5>
;  RW--F 
;  2671 : #define NV_CIO_CR_OVL_VBS_8                                     3:3 /* RW--F */
NV_CIO_CR_OVL_VBS_8             TEXTEQU         <3:3>
;  RW--F 
;  2672 : #define NV_CIO_CR_OVL_VRS_8                                     2:2 /* RW--F */
NV_CIO_CR_OVL_VRS_8             TEXTEQU         <2:2>
;  RW--F 
;  2673 : #define NV_CIO_CR_OVL_VRS_9                                     7:7 /* RW--F */
NV_CIO_CR_OVL_VRS_9             TEXTEQU         <7:7>
;        
;  2674 : #define NV_CIO_CR_RSAL_INDEX                             0x00000008 /*       */
NV_CIO_CR_RSAL_INDEX            EQU             000000008h
;  RW--F 
;  2675 : #define NV_CIO_CR_RSAL_PANNING                                  6:5 /* RW--F */
NV_CIO_CR_RSAL_PANNING          TEXTEQU         <6:5>
;        
;  2676 : #define NV_CIO_CR_CELL_HT_INDEX                          0x00000009 /*       */
NV_CIO_CR_CELL_HT_INDEX         EQU             000000009h
;  RW--F 
;  2677 : #define NV_CIO_CR_CELL_HT_SCANDBL                               7:7 /* RW--F */
NV_CIO_CR_CELL_HT_SCANDBL               TEXTEQU         <7:7>
;  RW--F 
;  2678 : #define NV_CIO_CR_CELL_HT_VBS_9                                 5:5 /* RW--F */
NV_CIO_CR_CELL_HT_VBS_9         TEXTEQU         <5:5>
;        
;  2679 : #define NV_CIO_CR_CURS_ST_INDEX                          0x0000000A /*       */
NV_CIO_CR_CURS_ST_INDEX         EQU             00000000ah
;        
;  2680 : #define NV_CIO_CR_CURS_END_INDEX                         0x0000000B /*       */
NV_CIO_CR_CURS_END_INDEX                EQU             00000000bh
;        
;  2681 : #define NV_CIO_CR_SA_HI_INDEX                            0x0000000C /*       */
NV_CIO_CR_SA_HI_INDEX           EQU             00000000ch
;        
;  2682 : #define NV_CIO_CR_SA_LO_INDEX                            0x0000000D /*       */
NV_CIO_CR_SA_LO_INDEX           EQU             00000000dh
;        
;  2683 : #define NV_CIO_CR_TCOFF_HI_INDEX                         0x0000000E /*       */
NV_CIO_CR_TCOFF_HI_INDEX                EQU             00000000eh
;        
;  2684 : #define NV_CIO_CR_TCOFF_LO_INDEX                         0x0000000F /*       */
NV_CIO_CR_TCOFF_LO_INDEX                EQU             00000000fh
;        
;  2685 : #define NV_CIO_CR_VRS_INDEX                              0x00000010 /*       */
NV_CIO_CR_VRS_INDEX             EQU             000000010h
;        
;  2686 : #define NV_CIO_CR_VRE_INDEX                              0x00000011 /*       */
NV_CIO_CR_VRE_INDEX             EQU             000000011h
;  RW--F 
;  2687 : #define NV_CIO_CR_VRE_3_0                                       3:0 /* RW--F */
NV_CIO_CR_VRE_3_0               TEXTEQU         <3:0>
;        
;  2688 : #define NV_CIO_CR_VDE_INDEX                              0x00000012 /*       */
NV_CIO_CR_VDE_INDEX             EQU             000000012h
;        
;  2689 : #define NV_CIO_CR_OFFSET_INDEX                           0x00000013 /*       */
NV_CIO_CR_OFFSET_INDEX          EQU             000000013h
;        
;  2690 : #define NV_CIO_CR_ULINE_INDEX                            0x00000014 /*       */
NV_CIO_CR_ULINE_INDEX           EQU             000000014h
;        
;  2691 : #define NV_CIO_CR_VBS_INDEX                              0x00000015 /*       */
NV_CIO_CR_VBS_INDEX             EQU             000000015h
;        
;  2692 : #define NV_CIO_CR_VBE_INDEX                              0x00000016 /*       */
NV_CIO_CR_VBE_INDEX             EQU             000000016h
;        
;  2693 : #define NV_CIO_CR_MODE_INDEX                             0x00000017 /*       */
NV_CIO_CR_MODE_INDEX            EQU             000000017h
;        
;  2694 : #define NV_CIO_CR_LCOMP_INDEX                            0x00000018 /*       */
NV_CIO_CR_LCOMP_INDEX           EQU             000000018h
;        
;  2695 : #define NV_CIO_CR_GDATA_INDEX                            0x00000022 /*       */
NV_CIO_CR_GDATA_INDEX           EQU             000000022h
;        
;  2696 : #define NV_CIO_CR_ARFF_INDEX                             0x00000024 /*       */
NV_CIO_CR_ARFF_INDEX            EQU             000000024h
;        
;  2697 : #define NV_CIO_CR_ARX_INDEX                              0x00000026 /*       */
NV_CIO_CR_ARX_INDEX             EQU             000000026h
;  RW-1R 
;  2698 : #define NV_CIO_CRE__MONO                                 0x000003b5 /* RW-1R */
NV_CIO_CRE__MONO                EQU             0000003b5h
;  RW-1R 
;  2699 : #define NV_CIO_CRE__COLOR                                0x000003d5 /* RW-1R */
NV_CIO_CRE__COLOR               EQU             0000003d5h
;        
;  2700 : #define NV_CIO_CRE_RPC0_INDEX                            0x00000019 /*       */
NV_CIO_CRE_RPC0_INDEX           EQU             000000019h
;  RW--F 
;  2701 : #define NV_CIO_CRE_RPC0_START                                   4:0 /* RW--F */
NV_CIO_CRE_RPC0_START           TEXTEQU         <4:0>
;  RW--F 
;  2702 : #define NV_CIO_CRE_RPC0_OFFSET_10_8                             7:5 /* RW--F */
NV_CIO_CRE_RPC0_OFFSET_10_8             TEXTEQU         <7:5>
;        
;  2703 : #define NV_CIO_CRE_RPC1_INDEX                            0x0000001A /*       */
NV_CIO_CRE_RPC1_INDEX           EQU             00000001ah
;        
;  2704 : #define NV_CIO_CRE_FF_INDEX                              0x0000001B /*       */
NV_CIO_CRE_FF_INDEX             EQU             00000001bh
;  RW--F 
;  2705 : #define NV_CIO_CRE_FF_BURST                                     2:0 /* RW--F */
NV_CIO_CRE_FF_BURST             TEXTEQU         <2:0>
;  RW--V 
;  2706 : #define NV_CIO_CRE_FF_BURST_8                            0x00000000 /* RW--V */
NV_CIO_CRE_FF_BURST_8           EQU             000000000h
;  RW--V 
;  2707 : #define NV_CIO_CRE_FF_BURST_32                           0x00000001 /* RW--V */
NV_CIO_CRE_FF_BURST_32          EQU             000000001h
;  RW--V 
;  2708 : #define NV_CIO_CRE_FF_BURST_64                           0x00000002 /* RW--V */
NV_CIO_CRE_FF_BURST_64          EQU             000000002h
;  RW--V 
;  2709 : #define NV_CIO_CRE_FF_BURST_128                          0x00000003 /* RW--V */
NV_CIO_CRE_FF_BURST_128         EQU             000000003h
;  RW--V 
;  2710 : #define NV_CIO_CRE_FF_BURST_256                          0x00000004 /* RW--V */
NV_CIO_CRE_FF_BURST_256         EQU             000000004h
;        
;  2711 : #define NV_CIO_CRE_ENH_INDEX                             0x0000001C /*       */
NV_CIO_CRE_ENH_INDEX            EQU             00000001ch
;        
;  2712 : #define NV_CIO_CRE_PAGE0_INDEX                           0x0000001D /*       */
NV_CIO_CRE_PAGE0_INDEX          EQU             00000001dh
;        
;  2713 : #define NV_CIO_CRE_PAGE1_INDEX                           0x0000001E /*       */
NV_CIO_CRE_PAGE1_INDEX          EQU             00000001eh
;        
;  2714 : #define NV_CIO_CRE_FFLWM__INDEX                          0x00000020 /*       */
NV_CIO_CRE_FFLWM__INDEX         EQU             000000020h
;  RW--F 
;  2715 : #define NV_CIO_CRE_FFLWM_LWM                                    5:0 /* RW--F */
NV_CIO_CRE_FFLWM_LWM            TEXTEQU         <5:0>
;        
;  2716 : #define NV_CIO_CRE_LSR_INDEX                             0x00000025 /*       */
NV_CIO_CRE_LSR_INDEX            EQU             000000025h
;  RW--F 
;  2717 : #define NV_CIO_CRE_LSR_FORMAT                                   7:6 /* RW--F */
NV_CIO_CRE_LSR_FORMAT           TEXTEQU         <7:6>
;  RW--V 
;  2718 : #define NV_CIO_CRE_LSR_FORMAT_8BIT                       0x00000001 /* RW--V */
NV_CIO_CRE_LSR_FORMAT_8BIT              EQU             000000001h
;  RW--V 
;  2719 : #define NV_CIO_CRE_LSR_FORMAT_555                        0x00000002 /* RW--V */
NV_CIO_CRE_LSR_FORMAT_555               EQU             000000002h
;  RW--V 
;  2720 : #define NV_CIO_CRE_LSR_FORMAT_565                        0x00000003 /* RW--V */
NV_CIO_CRE_LSR_FORMAT_565               EQU             000000003h
;  RW--F 
;  2721 : #define NV_CIO_CRE_LSR_VDE_10                                   1:1 /* RW--F */
NV_CIO_CRE_LSR_VDE_10           TEXTEQU         <1:1>
;  RW--F 
;  2722 : #define NV_CIO_CRE_LSR_VDT_10                                   0:0 /* RW--F */
NV_CIO_CRE_LSR_VDT_10           TEXTEQU         <0:0>
;  RW--F 
;  2723 : #define NV_CIO_CRE_LSR_HBE_6                                    4:4 /* RW--F */
NV_CIO_CRE_LSR_HBE_6            TEXTEQU         <4:4>
;  RW--F 
;  2724 : #define NV_CIO_CRE_LSR_VBS_10                                   3:3 /* RW--F */
NV_CIO_CRE_LSR_VBS_10           TEXTEQU         <3:3>
;  RW--F 
;  2725 : #define NV_CIO_CRE_LSR_VRS_10                                   2:2 /* RW--F */
NV_CIO_CRE_LSR_VRS_10           TEXTEQU         <2:2>
;        
;  2726 : #define NV_CIO_CRE_PIXEL_INDEX                           0x00000028 /*       */
NV_CIO_CRE_PIXEL_INDEX          EQU             000000028h
;  RW--F 
;  2727 : #define NV_CIO_CRE_PIXEL_FORMAT                                 1:0 /* RW--F */
NV_CIO_CRE_PIXEL_FORMAT         TEXTEQU         <1:0>
;  RW--V 
;  2728 : #define NV_CIO_CRE_PIXEL_FORMAT_VGA                      0x00000000 /* RW--V */
NV_CIO_CRE_PIXEL_FORMAT_VGA             EQU             000000000h
;  RW--V 
;  2729 : #define NV_CIO_CRE_PIXEL_FORMAT_8BPP                     0x00000001 /* RW--V */
NV_CIO_CRE_PIXEL_FORMAT_8BPP            EQU             000000001h
;  RW--V 
;  2730 : #define NV_CIO_CRE_PIXEL_FORMAT_16BPP                    0x00000002 /* RW--V */
NV_CIO_CRE_PIXEL_FORMAT_16BPP           EQU             000000002h
;  RW--V 
;  2731 : #define NV_CIO_CRE_PIXEL_FORMAT_32BPP                    0x00000003 /* RW--V */
NV_CIO_CRE_PIXEL_FORMAT_32BPP           EQU             000000003h
;  RW--F 
;  2732 : #define NV_CIO_CRE_PIXEL_TILING                                 2:2 /* RW--F */
NV_CIO_CRE_PIXEL_TILING         TEXTEQU         <2:2>
;        
;  2733 : #define NV_CIO_CRE_DEC__INDEX                            0x00000029 /*       */
NV_CIO_CRE_DEC__INDEX           EQU             000000029h
;        
;  2734 : #define NV_CIO_CRE_OSCOL__INDEX                          0x0000002A /*       */
NV_CIO_CRE_OSCOL__INDEX         EQU             00000002ah
;        
;  2735 : #define NV_CIO_CRE_SCRATCH0__INDEX                       0x0000002B /*       */
NV_CIO_CRE_SCRATCH0__INDEX              EQU             00000002bh
;        
;  2736 : #define NV_CIO_CRE_SCRATCH1__INDEX                       0x0000002C /*       */
NV_CIO_CRE_SCRATCH1__INDEX              EQU             00000002ch
;        
;  2737 : #define NV_CIO_CRE_HEB__INDEX                            0x0000002D /*       */
NV_CIO_CRE_HEB__INDEX           EQU             00000002dh
;  RW--F 
;  2738 : #define NV_CIO_CRE_HEB_ILC_8                                    4:4 /* RW--F */
NV_CIO_CRE_HEB_ILC_8            TEXTEQU         <4:4>
;  RW--F 
;  2739 : #define NV_CIO_CRE_HEB_HRS_8                                    3:3 /* RW--F */
NV_CIO_CRE_HEB_HRS_8            TEXTEQU         <3:3>
;  RW--F 
;  2740 : #define NV_CIO_CRE_HEB_HBS_8                                    2:2 /* RW--F */
NV_CIO_CRE_HEB_HBS_8            TEXTEQU         <2:2>
;  RW--F 
;  2741 : #define NV_CIO_CRE_HEB_HDE_8                                    1:1 /* RW--F */
NV_CIO_CRE_HEB_HDE_8            TEXTEQU         <1:1>
;  RW--F 
;  2742 : #define NV_CIO_CRE_HEB_HDT_8                                    0:0 /* RW--F */
NV_CIO_CRE_HEB_HDT_8            TEXTEQU         <0:0>
;        
;  2743 : #define NV_CIO_CRE_HCUR_ADDR0_INDEX                      0x00000030 /*       */
NV_CIO_CRE_HCUR_ADDR0_INDEX             EQU             000000030h
;  RW--F 
;  2744 : #define NV_CIO_CRE_HCUR_ADDR0_ADR                               6:0 /* RW--F */
NV_CIO_CRE_HCUR_ADDR0_ADR               TEXTEQU         <6:0>
;        
;  2745 : #define NV_CIO_CRE_HCUR_ADDR1_INDEX                      0x00000031 /*       */
NV_CIO_CRE_HCUR_ADDR1_INDEX             EQU             000000031h
;  RW--F 
;  2746 : #define NV_CIO_CRE_HCUR_ADDR1_ADR                               7:3 /* RW--F */
NV_CIO_CRE_HCUR_ADDR1_ADR               TEXTEQU         <7:3>
;  RW--F 
;  2747 : #define NV_CIO_CRE_HCUR_ADDR1_CUR_DBL                           1:1 /* RW--F */
NV_CIO_CRE_HCUR_ADDR1_CUR_DBL           TEXTEQU         <1:1>
;  RW--F 
;  2748 : #define NV_CIO_CRE_HCUR_ADDR1_ENABLE                            0:0 /* RW--F */
NV_CIO_CRE_HCUR_ADDR1_ENABLE            TEXTEQU         <0:0>
;        
;  2749 : #define NV_CIO_CRE_VID_END0__INDEX                       0x00000032 /*       */
NV_CIO_CRE_VID_END0__INDEX              EQU             000000032h
;        
;  2750 : #define NV_CIO_CRE_VID_END1__INDEX                       0x00000033 /*       */
NV_CIO_CRE_VID_END1__INDEX              EQU             000000033h
;        
;  2751 : #define NV_CIO_CRE_RL0__INDEX                            0x00000034 /*       */
NV_CIO_CRE_RL0__INDEX           EQU             000000034h
;        
;  2752 : #define NV_CIO_CRE_RL1__INDEX                            0x00000035 /*       */
NV_CIO_CRE_RL1__INDEX           EQU             000000035h
;        
;  2753 : #define NV_CIO_CRE_RMA__INDEX                            0x00000038 /*       */
NV_CIO_CRE_RMA__INDEX           EQU             000000038h
;        
;  2754 : #define NV_CIO_CRE_ILACE__INDEX                          0x00000039 /*       */
NV_CIO_CRE_ILACE__INDEX         EQU             000000039h
;        
;  2755 : #define NV_CIO_CRE_TREG__INDEX                           0x0000003D /*       */
NV_CIO_CRE_TREG__INDEX          EQU             00000003dh
;        
;  2756 : #define NV_CIO_CRE_DDC_STATUS__INDEX                     0x0000003E /*       */
NV_CIO_CRE_DDC_STATUS__INDEX            EQU             00000003eh
;        
;  2757 : #define NV_CIO_CRE_DDC_WR__INDEX                         0x0000003F /*       */
NV_CIO_CRE_DDC_WR__INDEX                EQU             00000003fh
;  vga.ref 
;  RW--D 
;  2758 : /* vga.ref */
;  2759 : #define NV_PRMVIO                             0x000C7FFF:0x000C0000 /* RW--D */
NV_PRMVIO               TEXTEQU         <0x000C7FFF:0x000C0000>
;  RW-1R 
;  2760 : #define NV_PRMVIO_MBEN                                   0x000C0094 /* RW-1R */
NV_PRMVIO_MBEN          EQU             0000c0094h
;  RW-1R 
;  2761 : #define NV_PRMVIO_ADDEN                                  0x000C46e8 /* RW-1R */
NV_PRMVIO_ADDEN         EQU             0000c46e8h
;  RW-1R 
;  2762 : #define NV_PRMVIO_VSE1                                   0x000C0102 /* RW-1R */
NV_PRMVIO_VSE1          EQU             0000c0102h
;  RW-1R 
;  2763 : #define NV_PRMVIO_VSE2                                   0x000C03c3 /* RW-1R */
NV_PRMVIO_VSE2          EQU             0000c03c3h
;  R--1R 
;  2764 : #define NV_PRMVIO_MISC__READ                             0x000C03cc /* R--1R */
NV_PRMVIO_MISC__READ            EQU             0000c03cch
;  -W-1R 
;  2765 : #define NV_PRMVIO_MISC__WRITE                            0x000C03c2 /* -W-1R */
NV_PRMVIO_MISC__WRITE           EQU             0000c03c2h
;  RW-1R 
;  2766 : #define NV_PRMVIO_SRX                                    0x000C03c4 /* RW-1R */
NV_PRMVIO_SRX           EQU             0000c03c4h
;  RW-1R 
;  2767 : #define NV_PRMVIO_SR_RESET                               0x000C03c5 /* RW-1R */
NV_PRMVIO_SR_RESET              EQU             0000c03c5h
;        
;  2768 : #define NV_PRMVIO_SR_RESET_INDEX                         0x00000000 /*       */
NV_PRMVIO_SR_RESET_INDEX                EQU             000000000h
;  RW-1R 
;  2769 : #define NV_PRMVIO_SR_CLOCK                               0x000C03c5 /* RW-1R */
NV_PRMVIO_SR_CLOCK              EQU             0000c03c5h
;        
;  2770 : #define NV_PRMVIO_SR_CLOCK_INDEX                         0x00000001 /*       */
NV_PRMVIO_SR_CLOCK_INDEX                EQU             000000001h
;  RW-1R 
;  2771 : #define NV_PRMVIO_SR_PLANE_MASK                          0x000C03c5 /* RW-1R */
NV_PRMVIO_SR_PLANE_MASK         EQU             0000c03c5h
;        
;  2772 : #define NV_PRMVIO_SR_PLANE_MASK_INDEX                    0x00000002 /*       */
NV_PRMVIO_SR_PLANE_MASK_INDEX           EQU             000000002h
;  RW-1R 
;  2773 : #define NV_PRMVIO_SR_CHAR_MAP                            0x000C03c5 /* RW-1R */
NV_PRMVIO_SR_CHAR_MAP           EQU             0000c03c5h
;        
;  2774 : #define NV_PRMVIO_SR_CHAR_MAP_INDEX                      0x00000003 /*       */
NV_PRMVIO_SR_CHAR_MAP_INDEX             EQU             000000003h
;  RW-1R 
;  2775 : #define NV_PRMVIO_SR_MEM_MODE                            0x000C03c5 /* RW-1R */
NV_PRMVIO_SR_MEM_MODE           EQU             0000c03c5h
;        
;  2776 : #define NV_PRMVIO_SR_MEM_MODE_INDEX                      0x00000004 /*       */
NV_PRMVIO_SR_MEM_MODE_INDEX             EQU             000000004h
;  RW-1R 
;  2777 : #define NV_PRMVIO_SR_LOCK                                0x000C03c5 /* RW-1R */
NV_PRMVIO_SR_LOCK               EQU             0000c03c5h
;        
;  2778 : #define NV_PRMVIO_SR_LOCK_INDEX                          0x00000005 /*       */
NV_PRMVIO_SR_LOCK_INDEX         EQU             000000005h
;  RW-1R 
;  2779 : #define NV_PRMVIO_GRX                                    0x000C03ce /* RW-1R */
NV_PRMVIO_GRX           EQU             0000c03ceh
;  RW-1R 
;  2780 : #define NV_PRMVIO_GX_SR                                  0x000C03cf /* RW-1R */
NV_PRMVIO_GX_SR         EQU             0000c03cfh
;        
;  2781 : #define NV_PRMVIO_GX_SR_INDEX                            0x00000000 /*       */
NV_PRMVIO_GX_SR_INDEX           EQU             000000000h
;  RW-1R 
;  2782 : #define NV_PRMVIO_GX_SREN                                0x000C03cf /* RW-1R */
NV_PRMVIO_GX_SREN               EQU             0000c03cfh
;        
;  2783 : #define NV_PRMVIO_GX_SREN_INDEX                          0x00000001 /*       */
NV_PRMVIO_GX_SREN_INDEX         EQU             000000001h
;  RW-1R 
;  2784 : #define NV_PRMVIO_GX_CCOMP                               0x000C03cf /* RW-1R */
NV_PRMVIO_GX_CCOMP              EQU             0000c03cfh
;        
;  2785 : #define NV_PRMVIO_GX_CCOMP_INDEX                         0x00000002 /*       */
NV_PRMVIO_GX_CCOMP_INDEX                EQU             000000002h
;  RW-1R 
;  2786 : #define NV_PRMVIO_GX_ROP                                 0x000C03cf /* RW-1R */
NV_PRMVIO_GX_ROP                EQU             0000c03cfh
;        
;  2787 : #define NV_PRMVIO_GX_ROP_INDEX                           0x00000003 /*       */
NV_PRMVIO_GX_ROP_INDEX          EQU             000000003h
;  RW-1R 
;  2788 : #define NV_PRMVIO_GX_READ_MAP                            0x000C03cf /* RW-1R */
NV_PRMVIO_GX_READ_MAP           EQU             0000c03cfh
;        
;  2789 : #define NV_PRMVIO_GX_READ_MAP_INDEX                      0x00000004 /*       */
NV_PRMVIO_GX_READ_MAP_INDEX             EQU             000000004h
;  RW-1R 
;  2790 : #define NV_PRMVIO_GX_MODE                                0x000C03cf /* RW-1R */
NV_PRMVIO_GX_MODE               EQU             0000c03cfh
;        
;  2791 : #define NV_PRMVIO_GX_MODE_INDEX                          0x00000005 /*       */
NV_PRMVIO_GX_MODE_INDEX         EQU             000000005h
;  RW-1R 
;  2792 : #define NV_PRMVIO_GX_MISC                                0x000C03cf /* RW-1R */
NV_PRMVIO_GX_MISC               EQU             0000c03cfh
;        
;  2793 : #define NV_PRMVIO_GX_MISC_INDEX                          0x00000006 /*       */
NV_PRMVIO_GX_MISC_INDEX         EQU             000000006h
;  RW-1R 
;  2794 : #define NV_PRMVIO_GX_DONT_CARE                           0x000C03cf /* RW-1R */
NV_PRMVIO_GX_DONT_CARE          EQU             0000c03cfh
;        
;  2795 : #define NV_PRMVIO_GX_DONT_CARE_INDEX                     0x00000007 /*       */
NV_PRMVIO_GX_DONT_CARE_INDEX            EQU             000000007h
;  RW-1R 
;  2796 : #define NV_PRMVIO_GX_BIT_MASK                            0x000C03cf /* RW-1R */
NV_PRMVIO_GX_BIT_MASK           EQU             0000c03cfh
;        
;  2797 : #define NV_PRMVIO_GX_BIT_MASK_INDEX                      0x00000008 /*       */
NV_PRMVIO_GX_BIT_MASK_INDEX             EQU             000000008h
;  vga.ref 
;  RW--D 
;  2798 : /* vga.ref */
;  2799 : #define NV_PRMVGA                             0x000BFFFF:0x000A0000 /* RW--D */
NV_PRMVGA               TEXTEQU         <0x000BFFFF:0x000A0000>
;  dev_media.ref 
;  RW--D 
;  2800 : /* dev_media.ref */
;  2801 : #define NV_PME                                0x00200FFF:0x00200000 /* RW--D */
NV_PME          TEXTEQU         <0x00200FFF:0x00200000>
;  RWI4R 
;  2802 : #define NV_PME_INTR_0                                    0x00200100 /* RWI4R */
NV_PME_INTR_0           EQU             000200100h
;  RWIVF 
;  2803 : #define NV_PME_INTR_0_NOTIFY                                    0:0 /* RWIVF */
NV_PME_INTR_0_NOTIFY            TEXTEQU         <0:0>
;  R-I-V 
;  2804 : #define NV_PME_INTR_0_NOTIFY_NOT_PENDING                 0x00000000 /* R-I-V */
NV_PME_INTR_0_NOTIFY_NOT_PENDING                EQU             000000000h
;  R---V 
;  2805 : #define NV_PME_INTR_0_NOTIFY_PENDING                     0x00000001 /* R---V */
NV_PME_INTR_0_NOTIFY_PENDING            EQU             000000001h
;  -W--V 
;  2806 : #define NV_PME_INTR_0_NOTIFY_RESET                       0x00000001 /* -W--V */
NV_PME_INTR_0_NOTIFY_RESET              EQU             000000001h
;  RWIVF 
;  2807 : #define NV_PME_INTR_0_VMI                                       4:4 /* RWIVF */
NV_PME_INTR_0_VMI               TEXTEQU         <4:4>
;  R-I-V 
;  2808 : #define NV_PME_INTR_0_VMI_NOT_PENDING                    0x00000000 /* R-I-V */
NV_PME_INTR_0_VMI_NOT_PENDING           EQU             000000000h
;  R---V 
;  2809 : #define NV_PME_INTR_0_VMI_PENDING                        0x00000001 /* R---V */
NV_PME_INTR_0_VMI_PENDING               EQU             000000001h
;  -W--V 
;  2810 : #define NV_PME_INTR_0_VMI_RESET                          0x00000001 /* -W--V */
NV_PME_INTR_0_VMI_RESET         EQU             000000001h
;  RWI4R 
;  2811 : #define NV_PME_INTR_EN_0                                 0x00200140 /* RWI4R */
NV_PME_INTR_EN_0                EQU             000200140h
;  RWIVF 
;  2812 : #define NV_PME_INTR_EN_0_NOTIFY                                 0:0 /* RWIVF */
NV_PME_INTR_EN_0_NOTIFY         TEXTEQU         <0:0>
;  RWI-V 
;  2813 : #define NV_PME_INTR_EN_0_NOTIFY_DISABLED                 0x00000000 /* RWI-V */
NV_PME_INTR_EN_0_NOTIFY_DISABLED                EQU             000000000h
;  RW--V 
;  2814 : #define NV_PME_INTR_EN_0_NOTIFY_ENABLED                  0x00000001 /* RW--V */
NV_PME_INTR_EN_0_NOTIFY_ENABLED         EQU             000000001h
;  RWIVF 
;  2815 : #define NV_PME_INTR_EN_0_VMI                                    4:4 /* RWIVF */
NV_PME_INTR_EN_0_VMI            TEXTEQU         <4:4>
;  RWI-V 
;  2816 : #define NV_PME_INTR_EN_0_VMI_DISABLED                    0x00000000 /* RWI-V */
NV_PME_INTR_EN_0_VMI_DISABLED           EQU             000000000h
;  RW--V 
;  2817 : #define NV_PME_INTR_EN_0_VMI_ENABLED                     0x00000001 /* RW--V */
NV_PME_INTR_EN_0_VMI_ENABLED            EQU             000000001h
;  RWI4R 
;  2818 : #define NV_PME_CONFIG_0                                  0x00200200 /* RWI4R */
NV_PME_CONFIG_0         EQU             000200200h
;  RWIVF 
;  2819 : #define NV_PME_CONFIG_0_CCIR656                                 0:0 /* RWIVF */
NV_PME_CONFIG_0_CCIR656         TEXTEQU         <0:0>
;  RWI-V 
;  2820 : #define NV_PME_CONFIG_0_CCIR656_DISABLED                 0x00000000 /* RWI-V */
NV_PME_CONFIG_0_CCIR656_DISABLED                EQU             000000000h
;  RW--V 
;  2821 : #define NV_PME_CONFIG_0_CCIR656_ENABLED                  0x00000001 /* RW--V */
NV_PME_CONFIG_0_CCIR656_ENABLED         EQU             000000001h
;  RWIVF 
;  2822 : #define NV_PME_CONFIG_0_VMI                                     4:4 /* RWIVF */
NV_PME_CONFIG_0_VMI             TEXTEQU         <4:4>
;  RWI-V 
;  2823 : #define NV_PME_CONFIG_0_VMI_DISABLED                     0x00000000 /* RWI-V */
NV_PME_CONFIG_0_VMI_DISABLED            EQU             000000000h
;  RW--V 
;  2824 : #define NV_PME_CONFIG_0_VMI_ENABLED                      0x00000001 /* RW--V */
NV_PME_CONFIG_0_VMI_ENABLED             EQU             000000001h
;  RWIVF 
;  2825 : #define NV_PME_CONFIG_0_VBI_MODE                                9:8 /* RWIVF */
NV_PME_CONFIG_0_VBI_MODE                TEXTEQU         <9:8>
;  RWI-V 
;  2826 : #define NV_PME_CONFIG_0_VBI_MODE_DISABLED                0x00000000 /* RWI-V */
NV_PME_CONFIG_0_VBI_MODE_DISABLED               EQU             000000000h
;  RW--V 
;  2827 : #define NV_PME_CONFIG_0_VBI_MODE_1                       0x00000001 /* RW--V */
NV_PME_CONFIG_0_VBI_MODE_1              EQU             000000001h
;  RW--V 
;  2828 : #define NV_PME_CONFIG_0_VBI_MODE_2                       0x00000002 /* RW--V */
NV_PME_CONFIG_0_VBI_MODE_2              EQU             000000002h
;  RWIVF 
;  2829 : #define NV_PME_CONFIG_0_VID_CD                                12:12 /* RWIVF */
NV_PME_CONFIG_0_VID_CD          TEXTEQU         <12:12>
;  RWI-V 
;  2830 : #define NV_PME_CONFIG_0_VID_CD_DISABLED                  0x00000000 /* RWI-V */
NV_PME_CONFIG_0_VID_CD_DISABLED         EQU             000000000h
;  RW--V 
;  2831 : #define NV_PME_CONFIG_0_VID_CD_ENABLED                   0x00000001 /* RW--V */
NV_PME_CONFIG_0_VID_CD_ENABLED          EQU             000000001h
;  RWIVF 
;  2832 : #define NV_PME_CONFIG_0_AUD_CD                                16:16 /* RWIVF */
NV_PME_CONFIG_0_AUD_CD          TEXTEQU         <16:16>
;  RWI-V 
;  2833 : #define NV_PME_CONFIG_0_AUD_CD_DISABLED                  0x00000000 /* RWI-V */
NV_PME_CONFIG_0_AUD_CD_DISABLED         EQU             000000000h
;  RW--V 
;  2834 : #define NV_PME_CONFIG_0_AUD_CD_ENABLED                   0x00000001 /* RW--V */
NV_PME_CONFIG_0_AUD_CD_ENABLED          EQU             000000001h
;  RWI4R 
;  2835 : #define NV_PME_CONFIG_1                                  0x00200204 /* RWI4R */
NV_PME_CONFIG_1         EQU             000200204h
;  RWIVF 
;  2836 : #define NV_PME_CONFIG_1_BUFFS                                   0:0 /* RWIVF */
NV_PME_CONFIG_1_BUFFS           TEXTEQU         <0:0>
;  RWI-V 
;  2837 : #define NV_PME_CONFIG_1_BUFFS_PNVM                       0x00000000 /* RWI-V */
NV_PME_CONFIG_1_BUFFS_PNVM              EQU             000000000h
;  RW--V 
;  2838 : #define NV_PME_CONFIG_1_BUFFS_SYS                        0x00000001 /* RW--V */
NV_PME_CONFIG_1_BUFFS_SYS               EQU             000000001h
;  RWIVF 
;  2839 : #define NV_PME_CONFIG_1_HOST                                    4:4 /* RWIVF */
NV_PME_CONFIG_1_HOST            TEXTEQU         <4:4>
;  RWI-V 
;  2840 : #define NV_PME_CONFIG_1_HOST_PCI                         0x00000000 /* RWI-V */
NV_PME_CONFIG_1_HOST_PCI                EQU             000000000h
;  RW--V 
;  2841 : #define NV_PME_CONFIG_1_HOST_AGP                         0x00000001 /* RW--V */
NV_PME_CONFIG_1_HOST_AGP                EQU             000000001h
;  RWI4R 
;  2842 : #define NV_PME_VID_BUFF0_START_SYS                       0x00200300 /* RWI4R */ 
NV_PME_VID_BUFF0_START_SYS              EQU             000200300h
;  RWXUF 
;  2843 : #define NV_PME_VID_BUFF0_START_SYS_ADDRESS                     31:4 /* RWXUF */
NV_PME_VID_BUFF0_START_SYS_ADDRESS              TEXTEQU         <31:4>
;  RWI4R 
;  2844 : #define NV_PME_VID_BUFF1_START_SYS                       0x00200304 /* RWI4R */ 
NV_PME_VID_BUFF1_START_SYS              EQU             000200304h
;  RWXUF 
;  2845 : #define NV_PME_VID_BUFF1_START_SYS_ADDRESS                     31:4 /* RWXUF */
NV_PME_VID_BUFF1_START_SYS_ADDRESS              TEXTEQU         <31:4>
;  RWI4R 
;  2846 : #define NV_PME_VID_BUFF0_START_PNVM                      0x00200308 /* RWI4R */ 
NV_PME_VID_BUFF0_START_PNVM             EQU             000200308h
;  RWXUF 
;  2847 : #define NV_PME_VID_BUFF0_START_PNVM_ADDRESS                    22:4 /* RWXUF */
NV_PME_VID_BUFF0_START_PNVM_ADDRESS             TEXTEQU         <22:4>
;  RWI4R 
;  2848 : #define NV_PME_VID_BUFF1_START_PNVM                      0x0020030c /* RWI4R */ 
NV_PME_VID_BUFF1_START_PNVM             EQU             00020030ch
;  RWXUF 
;  2849 : #define NV_PME_VID_BUFF1_START_PNVM_ADDRESS                    22:4 /* RWXUF */
NV_PME_VID_BUFF1_START_PNVM_ADDRESS             TEXTEQU         <22:4>
;  RWI4R 
;  2850 : #define NV_PME_VID_BUFF0_LENGTH                          0x00200310 /* RWI4R */ 
NV_PME_VID_BUFF0_LENGTH         EQU             000200310h
;  RWXUF 
;  2851 : #define NV_PME_VID_BUFF0_LENGTH_BITS                          15:12 /* RWXUF */
NV_PME_VID_BUFF0_LENGTH_BITS            TEXTEQU         <15:12>
;  RWI4R 
;  2852 : #define NV_PME_VID_BUFF1_LENGTH                          0x00200314 /* RWI4R */ 
NV_PME_VID_BUFF1_LENGTH         EQU             000200314h
;  RWXUF 
;  2853 : #define NV_PME_VID_BUFF1_LENGTH_BITS                          15:12 /* RWXUF */
NV_PME_VID_BUFF1_LENGTH_BITS            TEXTEQU         <15:12>
;  RW-4R 
;  2854 : #define NV_PME_VID_ME_STATE                              0x00200318 /* RW-4R */
NV_PME_VID_ME_STATE             EQU             000200318h
;  RWIVF 
;  2855 : #define NV_PME_VID_ME_STATE_BUFF0_INTR_NOTIFY                   0:0 /* RWIVF */
NV_PME_VID_ME_STATE_BUFF0_INTR_NOTIFY           TEXTEQU         <0:0>
;  RWXVF 
;  2856 : #define NV_PME_VID_ME_STATE_BUFF1_INTR_NOTIFY                   4:4 /* RWXVF */
NV_PME_VID_ME_STATE_BUFF1_INTR_NOTIFY           TEXTEQU         <4:4>
;  RWXVF 
;  2857 : #define NV_PME_VID_ME_STATE_BUFF0_INTR_CHAINGAP                 8:8 /* RWXVF */
NV_PME_VID_ME_STATE_BUFF0_INTR_CHAINGAP         TEXTEQU         <8:8>
;  RWXVF 
;  2858 : #define NV_PME_VID_ME_STATE_BUFF1_INTR_CHAINGAP               12:12 /* RWXVF */
NV_PME_VID_ME_STATE_BUFF1_INTR_CHAINGAP         TEXTEQU         <12:12>
;  RWXVF 
;  2859 : #define NV_PME_VID_ME_STATE_BUFF0_IN_USE                      16:16 /* RWXVF */
NV_PME_VID_ME_STATE_BUFF0_IN_USE                TEXTEQU         <16:16>
;  RWXVF 
;  2860 : #define NV_PME_VID_ME_STATE_BUFF1_IN_USE                      20:20 /* RWXVF */
NV_PME_VID_ME_STATE_BUFF1_IN_USE                TEXTEQU         <20:20>
;  RWXVF 
;  2861 : #define NV_PME_VID_ME_STATE_CURRENT_BUFFER                    24:24 /* RWXVF */
NV_PME_VID_ME_STATE_CURRENT_BUFFER              TEXTEQU         <24:24>
;  RW--V 
;  2862 : #define NV_PME_VID_ME_STATE_CURRENT_BUFFER_0             0x00000000 /* RW--V */
NV_PME_VID_ME_STATE_CURRENT_BUFFER_0            EQU             000000000h
;  RW--V 
;  2863 : #define NV_PME_VID_ME_STATE_CURRENT_BUFFER_1             0x00000001 /* RW--V */
NV_PME_VID_ME_STATE_CURRENT_BUFFER_1            EQU             000000001h
;  RW-4R 
;  2864 : #define NV_PME_VID_SU_STATE                              0x0020031c /* RW-4R */
NV_PME_VID_SU_STATE             EQU             00020031ch
;  RWXVF 
;  2865 : #define NV_PME_VID_SU_STATE_BUFF0_IN_USE                      16:16 /* RWXVF */
NV_PME_VID_SU_STATE_BUFF0_IN_USE                TEXTEQU         <16:16>
;  RWXVF 
;  2866 : #define NV_PME_VID_SU_STATE_BUFF1_IN_USE                      20:20 /* RWXVF */
NV_PME_VID_SU_STATE_BUFF1_IN_USE                TEXTEQU         <20:20>
;  RW-4R 
;  2867 : #define NV_PME_VID_RM_STATE                              0x00200320 /* RW-4R */
NV_PME_VID_RM_STATE             EQU             000200320h
;  RWXVF 
;  2868 : #define NV_PME_VID_RM_STATE_BUFF0_INTR_NOTIFY                   0:0 /* RWXVF */
NV_PME_VID_RM_STATE_BUFF0_INTR_NOTIFY           TEXTEQU         <0:0>
;  RWXVF 
;  2869 : #define NV_PME_VID_RM_STATE_BUFF1_INTR_NOTIFY                   4:4 /* RWXVF */
NV_PME_VID_RM_STATE_BUFF1_INTR_NOTIFY           TEXTEQU         <4:4>
;  RWXVF 
;  2870 : #define NV_PME_VID_RM_STATE_BUFF0_INTR_CHAINGAP                 8:8 /* RWXVF */
NV_PME_VID_RM_STATE_BUFF0_INTR_CHAINGAP         TEXTEQU         <8:8>
;  RWXVF 
;  2871 : #define NV_PME_VID_RM_STATE_BUFF1_INTR_CHAINGAP               12:12 /* RWXVF */
NV_PME_VID_RM_STATE_BUFF1_INTR_CHAINGAP         TEXTEQU         <12:12>
;  RWI4R 
;  2872 : #define NV_PME_VID_CURRENT                               0x00200324 /* RWI4R */ 
NV_PME_VID_CURRENT              EQU             000200324h
;  RWXUF 
;  2873 : #define NV_PME_VID_CURRENT_POS                                 15:2 /* RWXUF */
NV_PME_VID_CURRENT_POS          TEXTEQU         <15:2>
;  RWI4R 
;  2874 : #define NV_PME_AUD_BUFF0_START_SYS                       0x00200340 /* RWI4R */ 
NV_PME_AUD_BUFF0_START_SYS              EQU             000200340h
;  RWXUF 
;  2875 : #define NV_PME_AUD_BUFF0_START_SYS_ADDRESS                     31:4 /* RWXUF */
NV_PME_AUD_BUFF0_START_SYS_ADDRESS              TEXTEQU         <31:4>
;  RWI4R 
;  2876 : #define NV_PME_AUD_BUFF1_START_SYS                       0x00200344 /* RWI4R */ 
NV_PME_AUD_BUFF1_START_SYS              EQU             000200344h
;  RWXUF 
;  2877 : #define NV_PME_AUD_BUFF1_START_SYS_ADDRESS                     31:4 /* RWXUF */
NV_PME_AUD_BUFF1_START_SYS_ADDRESS              TEXTEQU         <31:4>
;  RWI4R 
;  2878 : #define NV_PME_AUD_BUFF0_START_PNVM                      0x00200348 /* RWI4R */ 
NV_PME_AUD_BUFF0_START_PNVM             EQU             000200348h
;  RWXUF 
;  2879 : #define NV_PME_AUD_BUFF0_START_PNVM_ADDRESS                    22:4 /* RWXUF */
NV_PME_AUD_BUFF0_START_PNVM_ADDRESS             TEXTEQU         <22:4>
;  RWI4R 
;  2880 : #define NV_PME_AUD_BUFF1_START_PNVM                      0x0020034c /* RWI4R */ 
NV_PME_AUD_BUFF1_START_PNVM             EQU             00020034ch
;  RWXUF 
;  2881 : #define NV_PME_AUD_BUFF1_START_PNVM_ADDRESS                    22:4 /* RWXUF */
NV_PME_AUD_BUFF1_START_PNVM_ADDRESS             TEXTEQU         <22:4>
;  RWI4R 
;  2882 : #define NV_PME_AUD_BUFF0_LENGTH                          0x00200350 /* RWI4R */ 
NV_PME_AUD_BUFF0_LENGTH         EQU             000200350h
;  RWXUF 
;  2883 : #define NV_PME_AUD_BUFF0_LENGTH_BITS                          12:10 /* RWXUF */
NV_PME_AUD_BUFF0_LENGTH_BITS            TEXTEQU         <12:10>
;  RWI4R 
;  2884 : #define NV_PME_AUD_BUFF1_LENGTH                          0x00200354 /* RWI4R */ 
NV_PME_AUD_BUFF1_LENGTH         EQU             000200354h
;  RWXUF 
;  2885 : #define NV_PME_AUD_BUFF1_LENGTH_BITS                          12:10 /* RWXUF */
NV_PME_AUD_BUFF1_LENGTH_BITS            TEXTEQU         <12:10>
;  RW-4R 
;  2886 : #define NV_PME_AUD_ME_STATE                              0x00200358 /* RW-4R */
NV_PME_AUD_ME_STATE             EQU             000200358h
;  RWIVF 
;  2887 : #define NV_PME_AUD_ME_STATE_BUFF0_INTR_NOTIFY                   0:0 /* RWIVF */
NV_PME_AUD_ME_STATE_BUFF0_INTR_NOTIFY           TEXTEQU         <0:0>
;  RWXVF 
;  2888 : #define NV_PME_AUD_ME_STATE_BUFF1_INTR_NOTIFY                   4:4 /* RWXVF */
NV_PME_AUD_ME_STATE_BUFF1_INTR_NOTIFY           TEXTEQU         <4:4>
;  RWXVF 
;  2889 : #define NV_PME_AUD_ME_STATE_BUFF0_INTR_CHAINGAP                 8:8 /* RWXVF */
NV_PME_AUD_ME_STATE_BUFF0_INTR_CHAINGAP         TEXTEQU         <8:8>
;  RWXVF 
;  2890 : #define NV_PME_AUD_ME_STATE_BUFF1_INTR_CHAINGAP               12:12 /* RWXVF */
NV_PME_AUD_ME_STATE_BUFF1_INTR_CHAINGAP         TEXTEQU         <12:12>
;  RWXVF 
;  2891 : #define NV_PME_AUD_ME_STATE_BUFF0_IN_USE                      16:16 /* RWXVF */
NV_PME_AUD_ME_STATE_BUFF0_IN_USE                TEXTEQU         <16:16>
;  RWXVF 
;  2892 : #define NV_PME_AUD_ME_STATE_BUFF1_IN_USE                      20:20 /* RWXVF */
NV_PME_AUD_ME_STATE_BUFF1_IN_USE                TEXTEQU         <20:20>
;  RWXVF 
;  2893 : #define NV_PME_AUD_ME_STATE_CURRENT_BUFFER                    24:24 /* RWXVF */
NV_PME_AUD_ME_STATE_CURRENT_BUFFER              TEXTEQU         <24:24>
;  RW--V 
;  2894 : #define NV_PME_AUD_ME_STATE_CURRENT_BUFFER_0             0x00000000 /* RW--V */
NV_PME_AUD_ME_STATE_CURRENT_BUFFER_0            EQU             000000000h
;  RW--V 
;  2895 : #define NV_PME_AUD_ME_STATE_CURRENT_BUFFER_1             0x00000001 /* RW--V */
NV_PME_AUD_ME_STATE_CURRENT_BUFFER_1            EQU             000000001h
;  RW-4R 
;  2896 : #define NV_PME_AUD_SU_STATE                              0x0020035c /* RW-4R */
NV_PME_AUD_SU_STATE             EQU             00020035ch
;  RWXVF 
;  2897 : #define NV_PME_AUD_SU_STATE_BUFF0_IN_USE                      16:16 /* RWXVF */
NV_PME_AUD_SU_STATE_BUFF0_IN_USE                TEXTEQU         <16:16>
;  RWXVF 
;  2898 : #define NV_PME_AUD_SU_STATE_BUFF1_IN_USE                      20:20 /* RWXVF */
NV_PME_AUD_SU_STATE_BUFF1_IN_USE                TEXTEQU         <20:20>
;  RW-4R 
;  2899 : #define NV_PME_AUD_RM_STATE                              0x00200360 /* RW-4R */
NV_PME_AUD_RM_STATE             EQU             000200360h
;  RWXVF 
;  2900 : #define NV_PME_AUD_RM_STATE_BUFF0_INTR_NOTIFY                   0:0 /* RWXVF */
NV_PME_AUD_RM_STATE_BUFF0_INTR_NOTIFY           TEXTEQU         <0:0>
;  RWXVF 
;  2901 : #define NV_PME_AUD_RM_STATE_BUFF1_INTR_NOTIFY                   4:4 /* RWXVF */
NV_PME_AUD_RM_STATE_BUFF1_INTR_NOTIFY           TEXTEQU         <4:4>
;  RWXVF 
;  2902 : #define NV_PME_AUD_RM_STATE_BUFF0_INTR_CHAINGAP                 8:8 /* RWXVF */
NV_PME_AUD_RM_STATE_BUFF0_INTR_CHAINGAP         TEXTEQU         <8:8>
;  RWXVF 
;  2903 : #define NV_PME_AUD_RM_STATE_BUFF1_INTR_CHAINGAP               12:12 /* RWXVF */
NV_PME_AUD_RM_STATE_BUFF1_INTR_CHAINGAP         TEXTEQU         <12:12>
;  RWI4R 
;  2904 : #define NV_PME_AUD_CURRENT                               0x00200364 /* RWI4R */ 
NV_PME_AUD_CURRENT              EQU             000200364h
;  RWXUF 
;  2905 : #define NV_PME_AUD_CURRENT_POS                                 12:2 /* RWXUF */
NV_PME_AUD_CURRENT_POS          TEXTEQU         <12:2>
;  RWI4R 
;  2906 : #define NV_PME_VBI_BUFF0_START                           0x00200380 /* RWI4R */ 
NV_PME_VBI_BUFF0_START          EQU             000200380h
;  RWXUF 
;  2907 : #define NV_PME_VBI_BUFF0_START_ADDRESS                         22:4 /* RWXUF */
NV_PME_VBI_BUFF0_START_ADDRESS          TEXTEQU         <22:4>
;  RWI4R 
;  2908 : #define NV_PME_VBI_BUFF1_START                           0x00200384 /* RWI4R */ 
NV_PME_VBI_BUFF1_START          EQU             000200384h
;  RWXUF 
;  2909 : #define NV_PME_VBI_BUFF1_START_ADDRESS                         22:4 /* RWXUF */
NV_PME_VBI_BUFF1_START_ADDRESS          TEXTEQU         <22:4>
;  RWI4R 
;  2910 : #define NV_PME_VBI_BUFF0_PITCH                           0x00200388 /* RWI4R */ 
NV_PME_VBI_BUFF0_PITCH          EQU             000200388h
;  RWXUF 
;  2911 : #define NV_PME_VBI_BUFF0_PITCH_VALUE                           13:4 /* RWXUF */
NV_PME_VBI_BUFF0_PITCH_VALUE            TEXTEQU         <13:4>
;  RWI4R 
;  2912 : #define NV_PME_VBI_BUFF1_PITCH                           0x0020038c /* RWI4R */ 
NV_PME_VBI_BUFF1_PITCH          EQU             00020038ch
;  RWXUF 
;  2913 : #define NV_PME_VBI_BUFF1_PITCH_VALUE                           13:4 /* RWXUF */
NV_PME_VBI_BUFF1_PITCH_VALUE            TEXTEQU         <13:4>
;  RWI4R 
;  2914 : #define NV_PME_VBI_BUFF0_LENGTH                          0x00200390 /* RWI4R */ 
NV_PME_VBI_BUFF0_LENGTH         EQU             000200390h
;  RWXUF 
;  2915 : #define NV_PME_VBI_BUFF0_LENGTH_BITS                           19:4 /* RWXUF */
NV_PME_VBI_BUFF0_LENGTH_BITS            TEXTEQU         <19:4>
;  RWI4R 
;  2916 : #define NV_PME_VBI_BUFF1_LENGTH                          0x00200394 /* RWI4R */ 
NV_PME_VBI_BUFF1_LENGTH         EQU             000200394h
;  RWXUF 
;  2917 : #define NV_PME_VBI_BUFF1_LENGTH_BITS                           19:4 /* RWXUF */
NV_PME_VBI_BUFF1_LENGTH_BITS            TEXTEQU         <19:4>
;  RW-4R 
;  2918 : #define NV_PME_VBI_ME_STATE                              0x00200398 /* RW-4R */
NV_PME_VBI_ME_STATE             EQU             000200398h
;  RWXVF 
;  2919 : #define NV_PME_VBI_ME_STATE_BUFF0_INTR_NOTIFY                   0:0 /* RWXVF */
NV_PME_VBI_ME_STATE_BUFF0_INTR_NOTIFY           TEXTEQU         <0:0>
;  RWXVF 
;  2920 : #define NV_PME_VBI_ME_STATE_BUFF1_INTR_NOTIFY                   4:4 /* RWXVF */
NV_PME_VBI_ME_STATE_BUFF1_INTR_NOTIFY           TEXTEQU         <4:4>
;  RWXVF 
;  2921 : #define NV_PME_VBI_ME_STATE_BUFF0_ERROR_CODE                   10:8 /* RWXVF */
NV_PME_VBI_ME_STATE_BUFF0_ERROR_CODE            TEXTEQU         <10:8>
;  RWXVF 
;  2922 : #define NV_PME_VBI_ME_STATE_BUFF1_ERROR_CODE                  14:12 /* RWXVF */
NV_PME_VBI_ME_STATE_BUFF1_ERROR_CODE            TEXTEQU         <14:12>
;  RWXVF 
;  2923 : #define NV_PME_VBI_ME_STATE_BUFF0_IN_USE                      16:16 /* RWXVF */
NV_PME_VBI_ME_STATE_BUFF0_IN_USE                TEXTEQU         <16:16>
;  RWXVF 
;  2924 : #define NV_PME_VBI_ME_STATE_BUFF1_IN_USE                      20:20 /* RWXVF */
NV_PME_VBI_ME_STATE_BUFF1_IN_USE                TEXTEQU         <20:20>
;  RWXVF 
;  2925 : #define NV_PME_VBI_ME_STATE_CURRENT_BUFFER                    24:24 /* RWXVF */
NV_PME_VBI_ME_STATE_CURRENT_BUFFER              TEXTEQU         <24:24>
;  RW--V 
;  2926 : #define NV_PME_VBI_ME_STATE_CURRENT_BUFFER_0             0x00000000 /* RW--V */
NV_PME_VBI_ME_STATE_CURRENT_BUFFER_0            EQU             000000000h
;  RW--V 
;  2927 : #define NV_PME_VBI_ME_STATE_CURRENT_BUFFER_1             0x00000001 /* RW--V */
NV_PME_VBI_ME_STATE_CURRENT_BUFFER_1            EQU             000000001h
;  RW-4R 
;  2928 : #define NV_PME_VBI_SU_STATE                              0x0020039c /* RW-4R */
NV_PME_VBI_SU_STATE             EQU             00020039ch
;  RWXVF 
;  2929 : #define NV_PME_VBI_SU_STATE_BUFF0_FIELD                         8:8 /* RWXVF */
NV_PME_VBI_SU_STATE_BUFF0_FIELD         TEXTEQU         <8:8>
;  RWXVF 
;  2930 : #define NV_PME_VBI_SU_STATE_BUFF1_FIELD                       12:12 /* RWXVF */
NV_PME_VBI_SU_STATE_BUFF1_FIELD         TEXTEQU         <12:12>
;  RWXVF 
;  2931 : #define NV_PME_VBI_SU_STATE_BUFF0_IN_USE                      16:16 /* RWXVF */
NV_PME_VBI_SU_STATE_BUFF0_IN_USE                TEXTEQU         <16:16>
;  RWXVF 
;  2932 : #define NV_PME_VBI_SU_STATE_BUFF1_IN_USE                      20:20 /* RWXVF */
NV_PME_VBI_SU_STATE_BUFF1_IN_USE                TEXTEQU         <20:20>
;  RW-4R 
;  2933 : #define NV_PME_VBI_RM_STATE                              0x002003a0 /* RW-4R */
NV_PME_VBI_RM_STATE             EQU             0002003a0h
;  RWXVF 
;  2934 : #define NV_PME_VBI_RM_STATE_BUFF0_INTR_NOTIFY                   0:0 /* RWXVF */
NV_PME_VBI_RM_STATE_BUFF0_INTR_NOTIFY           TEXTEQU         <0:0>
;  RWXVF 
;  2935 : #define NV_PME_VBI_RM_STATE_BUFF1_INTR_NOTIFY                   4:4 /* RWXVF */
NV_PME_VBI_RM_STATE_BUFF1_INTR_NOTIFY           TEXTEQU         <4:4>
;  RWI4R 
;  2936 : #define NV_PME_VBI                                       0x002003a4 /* RWI4R */
NV_PME_VBI              EQU             0002003a4h
;  RWX-F 
;  2937 : #define NV_PME_VBI_START_LINE                                   4:0 /* RWX-F */
NV_PME_VBI_START_LINE           TEXTEQU         <4:0>
;  RWX-F 
;  2938 : #define NV_PME_VBI_NUM_LINES                                  20:16 /* RWX-F */
NV_PME_VBI_NUM_LINES            TEXTEQU         <20:16>
;  RWI4R 
;  2939 : #define NV_PME_IMAGE_BUFF0_START                         0x00200400 /* RWI4R */ 
NV_PME_IMAGE_BUFF0_START                EQU             000200400h
;  RWXUF 
;  2940 : #define NV_PME_IMAGE_BUFF0_START_ADDRESS                       22:4 /* RWXUF */
NV_PME_IMAGE_BUFF0_START_ADDRESS                TEXTEQU         <22:4>
;  RWI4R 
;  2941 : #define NV_PME_IMAGE_BUFF1_START                         0x00200404 /* RWI4R */ 
NV_PME_IMAGE_BUFF1_START                EQU             000200404h
;  RWXUF 
;  2942 : #define NV_PME_IMAGE_BUFF1_START_ADDRESS                       22:4 /* RWXUF */
NV_PME_IMAGE_BUFF1_START_ADDRESS                TEXTEQU         <22:4>
;  RWI4R 
;  2943 : #define NV_PME_IMAGE_BUFF0_PITCH                         0x00200408 /* RWI4R */ 
NV_PME_IMAGE_BUFF0_PITCH                EQU             000200408h
;  RWXUF 
;  2944 : #define NV_PME_IMAGE_BUFF0_PITCH_VALUE                         13:4 /* RWXUF */
NV_PME_IMAGE_BUFF0_PITCH_VALUE          TEXTEQU         <13:4>
;  RWI4R 
;  2945 : #define NV_PME_IMAGE_BUFF1_PITCH                         0x0020040c /* RWI4R */ 
NV_PME_IMAGE_BUFF1_PITCH                EQU             00020040ch
;  RWXUF 
;  2946 : #define NV_PME_IMAGE_BUFF1_PITCH_VALUE                         13:4 /* RWXUF */
NV_PME_IMAGE_BUFF1_PITCH_VALUE          TEXTEQU         <13:4>
;  RWI4R 
;  2947 : #define NV_PME_IMAGE_BUFF0_LENGTH                        0x00200410 /* RWI4R */ 
NV_PME_IMAGE_BUFF0_LENGTH               EQU             000200410h
;  RWXUF 
;  2948 : #define NV_PME_IMAGE_BUFF0_LENGTH_BITS                         19:4 /* RWXUF */
NV_PME_IMAGE_BUFF0_LENGTH_BITS          TEXTEQU         <19:4>
;  RWI4R 
;  2949 : #define NV_PME_IMAGE_BUFF1_LENGTH                        0x00200414 /* RWI4R */ 
NV_PME_IMAGE_BUFF1_LENGTH               EQU             000200414h
;  RWXUF 
;  2950 : #define NV_PME_IMAGE_BUFF1_LENGTH_BITS                         19:4 /* RWXUF */
NV_PME_IMAGE_BUFF1_LENGTH_BITS          TEXTEQU         <19:4>
;  RW-4R 
;  2951 : #define NV_PME_IMAGE_ME_STATE                            0x00200418 /* RW-4R */
NV_PME_IMAGE_ME_STATE           EQU             000200418h
;  RWXVF 
;  2952 : #define NV_PME_IMAGE_ME_STATE_BUFF0_INTR_NOTIFY                 0:0 /* RWXVF */
NV_PME_IMAGE_ME_STATE_BUFF0_INTR_NOTIFY         TEXTEQU         <0:0>
;  RWXVF 
;  2953 : #define NV_PME_IMAGE_ME_STATE_BUFF1_INTR_NOTIFY                 4:4 /* RWXVF */
NV_PME_IMAGE_ME_STATE_BUFF1_INTR_NOTIFY         TEXTEQU         <4:4>
;  RWXVF 
;  2954 : #define NV_PME_IMAGE_ME_STATE_BUFF0_ERROR_CODE                 10:8 /* RWXVF */
NV_PME_IMAGE_ME_STATE_BUFF0_ERROR_CODE          TEXTEQU         <10:8>
;  RWXVF 
;  2955 : #define NV_PME_IMAGE_ME_STATE_BUFF1_ERROR_CODE                14:12 /* RWXVF */
NV_PME_IMAGE_ME_STATE_BUFF1_ERROR_CODE          TEXTEQU         <14:12>
;  RWXVF 
;  2956 : #define NV_PME_IMAGE_ME_STATE_BUFF0_IN_USE                    16:16 /* RWXVF */
NV_PME_IMAGE_ME_STATE_BUFF0_IN_USE              TEXTEQU         <16:16>
;  RWXVF 
;  2957 : #define NV_PME_IMAGE_ME_STATE_BUFF1_IN_USE                    20:20 /* RWXVF */
NV_PME_IMAGE_ME_STATE_BUFF1_IN_USE              TEXTEQU         <20:20>
;  RWXVF 
;  2958 : #define NV_PME_IMAGE_ME_STATE_CURRENT_BUFFER                  24:24 /* RWXVF */
NV_PME_IMAGE_ME_STATE_CURRENT_BUFFER            TEXTEQU         <24:24>
;  RW--V 
;  2959 : #define NV_PME_IMAGE_ME_STATE_CURRENT_BUFFER_0           0x00000000 /* RW--V */
NV_PME_IMAGE_ME_STATE_CURRENT_BUFFER_0          EQU             000000000h
;  RW--V 
;  2960 : #define NV_PME_IMAGE_ME_STATE_CURRENT_BUFFER_1           0x00000001 /* RW--V */
NV_PME_IMAGE_ME_STATE_CURRENT_BUFFER_1          EQU             000000001h
;  RW-4R 
;  2961 : #define NV_PME_IMAGE_SU_STATE                            0x0020041c /* RW-4R */
NV_PME_IMAGE_SU_STATE           EQU             00020041ch
;  RWXVF 
;  2962 : #define NV_PME_IMAGE_SU_STATE_BUFF0_FIELD                       8:8 /* RWXVF */
NV_PME_IMAGE_SU_STATE_BUFF0_FIELD               TEXTEQU         <8:8>
;  RWXVF 
;  2963 : #define NV_PME_IMAGE_SU_STATE_BUFF1_FIELD                     12:12 /* RWXVF */
NV_PME_IMAGE_SU_STATE_BUFF1_FIELD               TEXTEQU         <12:12>
;  RWXVF 
;  2964 : #define NV_PME_IMAGE_SU_STATE_BUFF0_IN_USE                    16:16 /* RWXVF */
NV_PME_IMAGE_SU_STATE_BUFF0_IN_USE              TEXTEQU         <16:16>
;  RWXVF 
;  2965 : #define NV_PME_IMAGE_SU_STATE_BUFF1_IN_USE                    20:20 /* RWXVF */
NV_PME_IMAGE_SU_STATE_BUFF1_IN_USE              TEXTEQU         <20:20>
;  RW-4R 
;  2966 : #define NV_PME_IMAGE_RM_STATE                            0x00200420 /* RW-4R */
NV_PME_IMAGE_RM_STATE           EQU             000200420h
;  RWXVF 
;  2967 : #define NV_PME_IMAGE_RM_STATE_BUFF0_INTR_NOTIFY                 0:0 /* RWXVF */
NV_PME_IMAGE_RM_STATE_BUFF0_INTR_NOTIFY         TEXTEQU         <0:0>
;  RWXVF 
;  2968 : #define NV_PME_IMAGE_RM_STATE_BUFF1_INTR_NOTIFY                 4:4 /* RWXVF */
NV_PME_IMAGE_RM_STATE_BUFF1_INTR_NOTIFY         TEXTEQU         <4:4>
;  RW-4R 
;  2969 : #define NV_PME_IMAGE_BUFF0_SCALE_INCR                    0x00200424 /* RW-4R */
NV_PME_IMAGE_BUFF0_SCALE_INCR           EQU             000200424h
;  RWXVF 
;  2970 : #define NV_PME_IMAGE_BUFF0_SCALE_INCR_Y                       26:16 /* RWXVF */
NV_PME_IMAGE_BUFF0_SCALE_INCR_Y         TEXTEQU         <26:16>
;  RWXVF 
;  2971 : #define NV_PME_IMAGE_BUFF0_SCALE_INCR_X                        10:0 /* RWXVF */
NV_PME_IMAGE_BUFF0_SCALE_INCR_X         TEXTEQU         <10:0>
;  RW-4R 
;  2972 : #define NV_PME_IMAGE_BUFF1_SCALE_INCR                    0x00200428 /* RW-4R */
NV_PME_IMAGE_BUFF1_SCALE_INCR           EQU             000200428h
;  RWXVF 
;  2973 : #define NV_PME_IMAGE_BUFF1_SCALE_INCR_Y                       26:16 /* RWXVF */
NV_PME_IMAGE_BUFF1_SCALE_INCR_Y         TEXTEQU         <26:16>
;  RWXVF 
;  2974 : #define NV_PME_IMAGE_BUFF1_SCALE_INCR_X                        10:0 /* RWXVF */
NV_PME_IMAGE_BUFF1_SCALE_INCR_X         TEXTEQU         <10:0>
;  R--4R 
;  2975 : #define NV_PME_FIFO_LINE_START                           0x00200480 /* R--4R */
NV_PME_FIFO_LINE_START          EQU             000200480h
;  R-XVF 
;  2976 : #define NV_PME_FIFO_LINE_START_ADDRESS                         20:4 /* R-XVF */
NV_PME_FIFO_LINE_START_ADDRESS          TEXTEQU         <20:4>
;  RWI4R 
;  2977 : #define NV_PME_FIFO_CURRENT                              0x00200484 /* RWI4R */
NV_PME_FIFO_CURRENT             EQU             000200484h
;  RWXVF 
;  2978 : #define NV_PME_FIFO_CURRENT_ADDRESS                            20:2 /* RWXVF */
NV_PME_FIFO_CURRENT_ADDRESS             TEXTEQU         <20:2>
;  R--4R 
;  2979 : #define NV_PME_VMI_POLL                                  0x00200488 /* R--4R */
NV_PME_VMI_POLL         EQU             000200488h
;  R-IVF 
;  2980 : #define NV_PME_VMI_POLL_UNCD                                    0:0 /* R-IVF */
NV_PME_VMI_POLL_UNCD            TEXTEQU         <0:0>
;  R-IVF 
;  2981 : #define NV_PME_VMI_POLL_UNCD_NOT_PENDING                 0x00000000 /* R-IVF */
NV_PME_VMI_POLL_UNCD_NOT_PENDING                EQU             000000000h
;  R--VF 
;  2982 : #define NV_PME_VMI_POLL_UNCD_PENDING                     0x00000001 /* R--VF */
NV_PME_VMI_POLL_UNCD_PENDING            EQU             000000001h
;  R-IVF 
;  2983 : #define NV_PME_VMI_POLL_VIDCD                                   1:1 /* R-IVF */
NV_PME_VMI_POLL_VIDCD           TEXTEQU         <1:1>
;  R-IVF 
;  2984 : #define NV_PME_VMI_POLL_VIDCD_NOT_PENDING                0x00000000 /* R-IVF */
NV_PME_VMI_POLL_VIDCD_NOT_PENDING               EQU             000000000h
;  R--VF 
;  2985 : #define NV_PME_VMI_POLL_VIDCD_PENDING                    0x00000001 /* R--VF */
NV_PME_VMI_POLL_VIDCD_PENDING           EQU             000000001h
;  R-IVF 
;  2986 : #define NV_PME_VMI_POLL_AUDCD                                   2:2 /* R-IVF */
NV_PME_VMI_POLL_AUDCD           TEXTEQU         <2:2>
;  R-IVF 
;  2987 : #define NV_PME_VMI_POLL_AUDCD_NOT_PENDING                0x00000000 /* R-IVF */
NV_PME_VMI_POLL_AUDCD_NOT_PENDING               EQU             000000000h
;  R--VF 
;  2988 : #define NV_PME_VMI_POLL_AUDCD_PENDING                    0x00000001 /* R--VF */
NV_PME_VMI_POLL_AUDCD_PENDING           EQU             000000001h
;  R-IVF 
;  2989 : #define NV_PME_VMI_POLL_INT                                     3:3 /* R-IVF */
NV_PME_VMI_POLL_INT             TEXTEQU         <3:3>
;  R-IVF 
;  2990 : #define NV_PME_VMI_POLL_INT_NOT_PENDING                  0x00000000 /* R-IVF */
NV_PME_VMI_POLL_INT_NOT_PENDING         EQU             000000000h
;  R--VF 
;  2991 : #define NV_PME_VMI_POLL_INT_PENDING                      0x00000001 /* R--VF */
NV_PME_VMI_POLL_INT_PENDING             EQU             000000001h
;  R-IVF 
;  2992 : #define NV_PME_VMI_POLL_CPURDREC                                4:4 /* R-IVF */
NV_PME_VMI_POLL_CPURDREC                TEXTEQU         <4:4>
;  R-IVF 
;  2993 : #define NV_PME_VMI_POLL_CPURDREC_NOT_PENDING             0x00000000 /* R-IVF */
NV_PME_VMI_POLL_CPURDREC_NOT_PENDING            EQU             000000000h
;  R--VF 
;  2994 : #define NV_PME_VMI_POLL_CPURDREC_PENDING                 0x00000001 /* R--VF */
NV_PME_VMI_POLL_CPURDREC_PENDING                EQU             000000001h
;  RWI4R 
;  2995 : #define NV_PME_DMA_STATUS_0                              0x00200500 /* RWI4R */
NV_PME_DMA_STATUS_0             EQU             000200500h
;  R-I-F 
;  2996 : #define NV_PME_DMA_STATUS_0_STATE                               0:0 /* R-I-F */
NV_PME_DMA_STATUS_0_STATE               TEXTEQU         <0:0>
;  R-I-V 
;  2997 : #define NV_PME_DMA_STATUS_0_STATE_IDLE                   0x00000000 /* R-I-V */
NV_PME_DMA_STATUS_0_STATE_IDLE          EQU             000000000h
;  R-I-V 
;  2998 : #define NV_PME_DMA_STATUS_0_STATE_BUSY                   0x00000001 /* R-I-V */                  
NV_PME_DMA_STATUS_0_STATE_BUSY          EQU             000000001h
;  R-I-F 
;  2999 : #define NV_PME_DMA_STATUS_0_DRAIN                               8:8 /* R-I-F */
NV_PME_DMA_STATUS_0_DRAIN               TEXTEQU         <8:8>
;  R-I-V 
;  3000 : #define NV_PME_DMA_STATUS_0_DRAIN_IDLE                   0x00000000 /* R-I-V */
NV_PME_DMA_STATUS_0_DRAIN_IDLE          EQU             000000000h
;  R-I-V 
;  3001 : #define NV_PME_DMA_STATUS_0_DRAIN_BUSY                   0x00000001 /* R-I-V */                  
NV_PME_DMA_STATUS_0_DRAIN_BUSY          EQU             000000001h
;  R-I-F 
;  3002 : #define NV_PME_DMA_STATUS_0_FLUSH                             12:12 /* R-I-F */
NV_PME_DMA_STATUS_0_FLUSH               TEXTEQU         <12:12>
;  R-I-V 
;  3003 : #define NV_PME_DMA_STATUS_0_FLUSH_IDLE                   0x00000000 /* R-I-V */
NV_PME_DMA_STATUS_0_FLUSH_IDLE          EQU             000000000h
;  R-I-V 
;  3004 : #define NV_PME_DMA_STATUS_0_FLUSH_BUSY                   0x00000001 /* R-I-V */                  
NV_PME_DMA_STATUS_0_FLUSH_BUSY          EQU             000000001h
;  R-I-F 
;  3005 : #define NV_PME_DMA_STATUS_0_READ                              16:16 /* R-I-F */
NV_PME_DMA_STATUS_0_READ                TEXTEQU         <16:16>
;  R-I-V 
;  3006 : #define NV_PME_DMA_STATUS_0_READ_IDLE                    0x00000000 /* R-I-V */
NV_PME_DMA_STATUS_0_READ_IDLE           EQU             000000000h
;  R-I-V 
;  3007 : #define NV_PME_DMA_STATUS_0_READ_BUSY                    0x00000001 /* R-I-V */                  
NV_PME_DMA_STATUS_0_READ_BUSY           EQU             000000001h
;  RW-4A 
;        
;  3008 : #define NV_PME_EXTERNAL(i)                       (0x00200600+(i)*4) /* RW-4A */
;  3009 : #define NV_PME_EXTERNAL_SIZE_1                                  256 /*       */  
NV_PME_EXTERNAL_SIZE_1          EQU             256t
;  RWXVF 
;  3010 : #define NV_PME_EXTERNAL_DATA                                    7:0 /* RWXVF */
NV_PME_EXTERNAL_DATA            TEXTEQU         <7:0>
;  usr_beta_solid.ref 
;  -W--D 
;  3011 : /* usr_beta_solid.ref */
;  3012 : #define NV_UBETA                              0x00411FFF:0x00410000 /* -W--D */
NV_UBETA                TEXTEQU         <0x00411FFF:0x00410000>
;  -W-4R 
;  3013 : #define NV_UBETA_CTX_SWITCH                              0x00410000 /* -W-4R */
NV_UBETA_CTX_SWITCH             EQU             000410000h
;  -W-UF 
;  3014 : #define NV_UBETA_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
NV_UBETA_CTX_SWITCH_INSTANCE            TEXTEQU         <15:0>
;  -W-UF 
;  3015 : #define NV_UBETA_CTX_SWITCH_CHID                              22:16 /* -W-UF */
NV_UBETA_CTX_SWITCH_CHID                TEXTEQU         <22:16>
;  -W-VF 
;  3016 : #define NV_UBETA_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
NV_UBETA_CTX_SWITCH_VOLATILE            TEXTEQU         <31:31>
;  -W--V 
;  3017 : #define NV_UBETA_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
NV_UBETA_CTX_SWITCH_VOLATILE_IGNORE             EQU             000000000h
;  -W--V 
;  3018 : #define NV_UBETA_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
NV_UBETA_CTX_SWITCH_VOLATILE_RESET              EQU             000000001h
;  -W-4R 
;  3019 : #define NV_UBETA_SET_NOTIFY                              0x00410104 /* -W-4R */
NV_UBETA_SET_NOTIFY             EQU             000410104h
;  -W-VF 
;  3020 : #define NV_UBETA_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
NV_UBETA_SET_NOTIFY_PARAMETER           TEXTEQU         <31:0>
;  -W--V 
;  3021 : #define NV_UBETA_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
NV_UBETA_SET_NOTIFY_PARAMETER_WRITE             EQU             000000000h
;  -W-4R 
;  3022 : #define NV_UBETA_SET_BETA1D31                            0x00410300 /* -W-4R */
NV_UBETA_SET_BETA1D31           EQU             000410300h
;  -W-UF 
;  3023 : #define NV_UBETA_SET_BETA1D31_VALUE_FRACTION                  30:21 /* -W-UF */
NV_UBETA_SET_BETA1D31_VALUE_FRACTION            TEXTEQU         <30:21>
;  -W-SF 
;  3024 : #define NV_UBETA_SET_BETA1D31_VALUE                           31:31 /* -W-SF */
NV_UBETA_SET_BETA1D31_VALUE             TEXTEQU         <31:31>
;  usr_rop_solid.ref 
;  -W--D 
;  3025 : /* usr_rop_solid.ref */
;  3026 : #define NV_UROP                               0x00421FFF:0x00420000 /* -W--D */
NV_UROP         TEXTEQU         <0x00421FFF:0x00420000>
;  -W-4R 
;  3027 : #define NV_UROP_CTX_SWITCH                               0x00420000 /* -W-4R */
NV_UROP_CTX_SWITCH              EQU             000420000h
;  -W-UF 
;  3028 : #define NV_UROP_CTX_SWITCH_INSTANCE                            15:0 /* -W-UF */
NV_UROP_CTX_SWITCH_INSTANCE             TEXTEQU         <15:0>
;  -W-UF 
;  3029 : #define NV_UROP_CTX_SWITCH_CHID                               22:16 /* -W-UF */
NV_UROP_CTX_SWITCH_CHID         TEXTEQU         <22:16>
;  -W-VF 
;  3030 : #define NV_UROP_CTX_SWITCH_VOLATILE                           31:31 /* -W-VF */
NV_UROP_CTX_SWITCH_VOLATILE             TEXTEQU         <31:31>
;  -W--V 
;  3031 : #define NV_UROP_CTX_SWITCH_VOLATILE_IGNORE               0x00000000 /* -W--V */
NV_UROP_CTX_SWITCH_VOLATILE_IGNORE              EQU             000000000h
;  -W--V 
;  3032 : #define NV_UROP_CTX_SWITCH_VOLATILE_RESET                0x00000001 /* -W--V */
NV_UROP_CTX_SWITCH_VOLATILE_RESET               EQU             000000001h
;  -W-4R 
;  3033 : #define NV_UROP_SET_NOTIFY                               0x00420104 /* -W-4R */
NV_UROP_SET_NOTIFY              EQU             000420104h
;  -W-VF 
;  3034 : #define NV_UROP_SET_NOTIFY_PARAMETER                           31:0 /* -W-VF */
NV_UROP_SET_NOTIFY_PARAMETER            TEXTEQU         <31:0>
;  -W--V 
;  3035 : #define NV_UROP_SET_NOTIFY_PARAMETER_WRITE               0x00000000 /* -W--V */
NV_UROP_SET_NOTIFY_PARAMETER_WRITE              EQU             000000000h
;  -W-4R 
;  3036 : #define NV_UROP_SET_ROP                                  0x00420300 /* -W-4R */
NV_UROP_SET_ROP         EQU             000420300h
;  -W-VF 
;  3037 : #define NV_UROP_SET_ROP_VALUE                                   7:0 /* -W-VF */
NV_UROP_SET_ROP_VALUE           TEXTEQU         <7:0>
;  usr_color_key.ref 
;  -W--D 
;  3038 : /* usr_color_key.ref */
;  3039 : #define NV_UCHROMA                            0x00431FFF:0x00430000 /* -W--D */
NV_UCHROMA              TEXTEQU         <0x00431FFF:0x00430000>
;  -W-4R 
;  3040 : #define NV_UCHROMA_CTX_SWITCH                            0x00430000 /* -W-4R */
NV_UCHROMA_CTX_SWITCH           EQU             000430000h
;  -W-UF 
;  3041 : #define NV_UCHROMA_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
NV_UCHROMA_CTX_SWITCH_INSTANCE          TEXTEQU         <15:0>
;  -W-UF 
;  3042 : #define NV_UCHROMA_CTX_SWITCH_CHID                            22:16 /* -W-UF */
NV_UCHROMA_CTX_SWITCH_CHID              TEXTEQU         <22:16>
;  -W-VF 
;  3043 : #define NV_UCHROMA_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
NV_UCHROMA_CTX_SWITCH_VOLATILE          TEXTEQU         <31:31>
;  -W--V 
;  3044 : #define NV_UCHROMA_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
NV_UCHROMA_CTX_SWITCH_VOLATILE_IGNORE           EQU             000000000h
;  -W--V 
;  3045 : #define NV_UCHROMA_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
NV_UCHROMA_CTX_SWITCH_VOLATILE_RESET            EQU             000000001h
;  -W-4R 
;  3046 : #define NV_UCHROMA_SET_NOTIFY                            0x00430104 /* -W-4R */
NV_UCHROMA_SET_NOTIFY           EQU             000430104h
;  -W-VF 
;  3047 : #define NV_UCHROMA_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
NV_UCHROMA_SET_NOTIFY_PARAMETER         TEXTEQU         <31:0>
;  -W--V 
;  3048 : #define NV_UCHROMA_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
NV_UCHROMA_SET_NOTIFY_PARAMETER_WRITE           EQU             000000000h
;  -W-4R 
;  3049 : #define NV_UCHROMA_SET_COLOR                             0x00430304 /* -W-4R */
NV_UCHROMA_SET_COLOR            EQU             000430304h
;  -W-VF 
;  3050 : #define NV_UCHROMA_SET_COLOR_VALUE                             31:0 /* -W-VF */
NV_UCHROMA_SET_COLOR_VALUE              TEXTEQU         <31:0>
;  usr_plane_switch.ref 
;  -W--D 
;  3051 : /* usr_plane_switch.ref */
;  3052 : #define NV_UPLANE                             0x00441FFF:0x00440000 /* -W--D */
NV_UPLANE               TEXTEQU         <0x00441FFF:0x00440000>
;  -W-4R 
;  3053 : #define NV_UPLANE_CTX_SWITCH                             0x00440000 /* -W-4R */
NV_UPLANE_CTX_SWITCH            EQU             000440000h
;  -W-UF 
;  3054 : #define NV_UPLANE_CTX_SWITCH_INSTANCE                          15:0 /* -W-UF */
NV_UPLANE_CTX_SWITCH_INSTANCE           TEXTEQU         <15:0>
;  -W-UF 
;  3055 : #define NV_UPLANE_CTX_SWITCH_CHID                             22:16 /* -W-UF */
NV_UPLANE_CTX_SWITCH_CHID               TEXTEQU         <22:16>
;  -W-VF 
;  3056 : #define NV_UPLANE_CTX_SWITCH_VOLATILE                         31:31 /* -W-VF */
NV_UPLANE_CTX_SWITCH_VOLATILE           TEXTEQU         <31:31>
;  -W--V 
;  3057 : #define NV_UPLANE_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* -W--V */
NV_UPLANE_CTX_SWITCH_VOLATILE_IGNORE            EQU             000000000h
;  -W--V 
;  3058 : #define NV_UPLANE_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
NV_UPLANE_CTX_SWITCH_VOLATILE_RESET             EQU             000000001h
;  -W-4R 
;  3059 : #define NV_UPLANE_SET_NOTIFY                             0x00440104 /* -W-4R */
NV_UPLANE_SET_NOTIFY            EQU             000440104h
;  -W-VF 
;  3060 : #define NV_UPLANE_SET_NOTIFY_PARAMETER                         31:0 /* -W-VF */
NV_UPLANE_SET_NOTIFY_PARAMETER          TEXTEQU         <31:0>
;  -W--V 
;  3061 : #define NV_UPLANE_SET_NOTIFY_PARAMETER_WRITE             0x00000000 /* -W--V */
NV_UPLANE_SET_NOTIFY_PARAMETER_WRITE            EQU             000000000h
;  -W-4R 
;  3062 : #define NV_UPLANE_SET_COLOR                              0x00440304 /* -W-4R */
NV_UPLANE_SET_COLOR             EQU             000440304h
;  -W-VF 
;  3063 : #define NV_UPLANE_SET_COLOR_VALUE                              31:0 /* -W-VF */
NV_UPLANE_SET_COLOR_VALUE               TEXTEQU         <31:0>
;  usr_clipping.ref 
;  -W--D 
;  3064 : /* usr_clipping.ref */
;  3065 : #define NV_UCLIP                              0x00451FFF:0x00450000 /* -W--D */
NV_UCLIP                TEXTEQU         <0x00451FFF:0x00450000>
;  -W-4R 
;  3066 : #define NV_UCLIP_CTX_SWITCH                              0x00450000 /* -W-4R */
NV_UCLIP_CTX_SWITCH             EQU             000450000h
;  -W-UF 
;  3067 : #define NV_UCLIP_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
NV_UCLIP_CTX_SWITCH_INSTANCE            TEXTEQU         <15:0>
;  -W-UF 
;  3068 : #define NV_UCLIP_CTX_SWITCH_CHID                              22:16 /* -W-UF */
NV_UCLIP_CTX_SWITCH_CHID                TEXTEQU         <22:16>
;  -W-VF 
;  3069 : #define NV_UCLIP_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
NV_UCLIP_CTX_SWITCH_VOLATILE            TEXTEQU         <31:31>
;  -W--V 
;  3070 : #define NV_UCLIP_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
NV_UCLIP_CTX_SWITCH_VOLATILE_IGNORE             EQU             000000000h
;  -W--V 
;  3071 : #define NV_UCLIP_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
NV_UCLIP_CTX_SWITCH_VOLATILE_RESET              EQU             000000001h
;  -W-4R 
;  3072 : #define NV_UCLIP_SET_NOTIFY                              0x00450104 /* -W-4R */
NV_UCLIP_SET_NOTIFY             EQU             000450104h
;  -W-VF 
;  3073 : #define NV_UCLIP_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
NV_UCLIP_SET_NOTIFY_PARAMETER           TEXTEQU         <31:0>
;  -W--V 
;  3074 : #define NV_UCLIP_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
NV_UCLIP_SET_NOTIFY_PARAMETER_WRITE             EQU             000000000h
;  -W-4R 
;  3075 : #define NV_UCLIP_SET_RECT_0                              0x00450300 /* -W-4R */
NV_UCLIP_SET_RECT_0             EQU             000450300h
;  -W-SF 
;  3076 : #define NV_UCLIP_SET_RECT_0_X                                  15:0 /* -W-SF */
NV_UCLIP_SET_RECT_0_X           TEXTEQU         <15:0>
;  -W-SF 
;  3077 : #define NV_UCLIP_SET_RECT_0_Y                                 31:16 /* -W-SF */
NV_UCLIP_SET_RECT_0_Y           TEXTEQU         <31:16>
;  -W-4R 
;  3078 : #define NV_UCLIP_SET_RECT_1                              0x00450304 /* -W-4R */
NV_UCLIP_SET_RECT_1             EQU             000450304h
;  -W-UF 
;  3079 : #define NV_UCLIP_SET_RECT_1_WIDTH                              15:0 /* -W-UF */
NV_UCLIP_SET_RECT_1_WIDTH               TEXTEQU         <15:0>
;  -W-UF 
;  3080 : #define NV_UCLIP_SET_RECT_1_HEIGHT                            31:16 /* -W-UF */
NV_UCLIP_SET_RECT_1_HEIGHT              TEXTEQU         <31:16>
;  usr_d3d0_triangle_zeta.ref 
;  -W--D 
;  3081 : /* usr_d3d0_triangle_zeta.ref */
;  3082 : #define NV_UD3D0Z                             0x00571FFF:0x00570000 /* -W--D */
NV_UD3D0Z               TEXTEQU         <0x00571FFF:0x00570000>
;  -W-4R 
;  3083 : #define NV_UD3D0Z_CTX_SWITCH                             0x00570000 /* -W-4R */
NV_UD3D0Z_CTX_SWITCH            EQU             000570000h
;  -W-UF 
;  3084 : #define NV_UD3D0Z_CTX_SWITCH_INSTANCE                          15:0 /* -W-UF */
NV_UD3D0Z_CTX_SWITCH_INSTANCE           TEXTEQU         <15:0>
;  -W-UF 
;  3085 : #define NV_UD3D0Z_CTX_SWITCH_CHID                             22:16 /* -W-UF */
NV_UD3D0Z_CTX_SWITCH_CHID               TEXTEQU         <22:16>
;  -W-VF 
;  3086 : #define NV_UD3D0Z_CTX_SWITCH_VOLATILE                         31:31 /* -W-VF */
NV_UD3D0Z_CTX_SWITCH_VOLATILE           TEXTEQU         <31:31>
;  -W--V 
;  3087 : #define NV_UD3D0Z_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* -W--V */
NV_UD3D0Z_CTX_SWITCH_VOLATILE_IGNORE            EQU             000000000h
;  -W--V 
;  3088 : #define NV_UD3D0Z_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
NV_UD3D0Z_CTX_SWITCH_VOLATILE_RESET             EQU             000000001h
;  -W-4R 
;  3089 : #define NV_UD3D0Z_SET_NOTIFY                             0x00570104 /* -W-4R */
NV_UD3D0Z_SET_NOTIFY            EQU             000570104h
;  -W-VF 
;  3090 : #define NV_UD3D0Z_SET_NOTIFY_PARAMETER                         31:0 /* -W-VF */
NV_UD3D0Z_SET_NOTIFY_PARAMETER          TEXTEQU         <31:0>
;  -W--V 
;  3091 : #define NV_UD3D0Z_SET_NOTIFY_PARAMETER_WRITE             0x00000000 /* -W--V */
NV_UD3D0Z_SET_NOTIFY_PARAMETER_WRITE            EQU             000000000h
;  -W-4R 
;  3092 : #define NV_UD3D0Z_TEXTURE_OFFSET                         0x00570304 /* -W-4R */
NV_UD3D0Z_TEXTURE_OFFSET                EQU             000570304h
;  -W-UF 
;  3093 : #define NV_UD3D0Z_TEXTURE_OFFSET_VALUE                         31:0 /* -W-UF */
NV_UD3D0Z_TEXTURE_OFFSET_VALUE          TEXTEQU         <31:0>
;  -W-4R 
;  3094 : #define NV_UD3D0Z_TEXTURE_FORMAT                         0x00570308 /* -W-4R */
NV_UD3D0Z_TEXTURE_FORMAT                EQU             000570308h
;  -W-UF 
;  3095 : #define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_KEY_COLOR_MASK          15:0 /* -W-UF */
NV_UD3D0Z_TEXTURE_FORMAT_COLOR_KEY_COLOR_MASK           TEXTEQU         <15:0>
;  -W-UF 
;  3096 : #define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_KEY                    16:16 /* -W-UF */
NV_UD3D0Z_TEXTURE_FORMAT_COLOR_KEY              TEXTEQU         <16:16>
;  -W-UV 
;  3097 : #define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_KEY_DISABLED      0x00000000 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_COLOR_KEY_DISABLED             EQU             000000000h
;  -W-UV 
;  3098 : #define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_KEY_ENABLED       0x00000001 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_COLOR_KEY_ENABLED              EQU             000000001h
;  -W-UF 
;  3099 : #define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT                 21:20 /* -W-UF */
NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT           TEXTEQU         <21:20>
;  -W-UV 
;  3100 : #define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT_A1R5G5B5   0x00000000 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT_A1R5G5B5          EQU             000000000h
;  -W-UV 
;  3101 : #define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT_X1R5G5B5   0x00000001 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT_X1R5G5B5          EQU             000000001h
;  -W-UV 
;  3102 : #define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT_A4R4G4G4   0x00000002 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT_A4R4G4G4          EQU             000000002h
;  -W-UV 
;  3103 : #define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT_R5G6B5     0x00000003 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT_R5G6B5            EQU             000000003h
;  -W-UF 
;  3104 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN                     27:24 /* -W-UF */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN               TEXTEQU         <27:24>
;  -W-UV 
;  3105 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_1X1            0x00000000 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_1X1           EQU             000000000h
;  -W-UV 
;  3106 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_2X2            0x00000001 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_2X2           EQU             000000001h
;  -W-UV 
;  3107 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_4X4            0x00000002 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_4X4           EQU             000000002h
;  -W-UV 
;  3108 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_8X8            0x00000003 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_8X8           EQU             000000003h
;  -W-UV 
;  3109 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_16X16          0x00000004 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_16X16         EQU             000000004h
;  -W-UV 
;  3110 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_32X32          0x00000005 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_32X32         EQU             000000005h
;  -W-UV 
;  3111 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_64X64          0x00000006 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_64X64         EQU             000000006h
;  -W-UV 
;  3112 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_128X128        0x00000007 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_128X128               EQU             000000007h
;  -W-UV 
;  3113 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_256X256        0x00000008 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_256X256               EQU             000000008h
;  -W-UV 
;  3114 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_512X512        0x00000009 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_512X512               EQU             000000009h
;  -W-UV 
;  3115 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_1024X1024      0x0000000a /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_1024X1024             EQU             00000000ah
;  -W-UV 
;  3116 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_2048X2048      0x0000000b /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_2048X2048             EQU             00000000bh
;  -W-UF 
;  3117 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX                     31:28 /* -W-UF */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX               TEXTEQU         <31:28>
;  -W-UV 
;  3118 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_1X1            0x00000000 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_1X1           EQU             000000000h
;  -W-UV 
;  3119 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_2X2            0x00000001 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_2X2           EQU             000000001h
;  -W-UV 
;  3120 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_4X4            0x00000002 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_4X4           EQU             000000002h
;  -W-UV 
;  3121 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_8X8            0x00000003 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_8X8           EQU             000000003h
;  -W-UV 
;  3122 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_16X16          0x00000004 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_16X16         EQU             000000004h
;  -W-UV 
;  3123 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_32X32          0x00000005 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_32X32         EQU             000000005h
;  -W-UV 
;  3124 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_64X64          0x00000006 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_64X64         EQU             000000006h
;  -W-UV 
;  3125 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_128X128        0x00000007 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_128X128               EQU             000000007h
;  -W-UV 
;  3126 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_256X256        0x00000008 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_256X256               EQU             000000008h
;  -W-UV 
;  3127 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_512X512        0x00000009 /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_512X512               EQU             000000009h
;  -W-UV 
;  3128 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_1024X1024      0x0000000a /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_1024X1024             EQU             00000000ah
;  -W-UV 
;  3129 : #define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_2048X2048      0x0000000b /* -W-UV */
NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_2048X2048             EQU             00000000bh
;  -W-4R 
;  3130 : #define NV_UD3D0Z_FILTER                                 0x0057030C /* -W-4R */
NV_UD3D0Z_FILTER                EQU             00057030ch
;  -W-UF 
;  3131 : #define NV_UD3D0Z_FILTER_SPREADX                                7:0 /* -W-UF */
NV_UD3D0Z_FILTER_SPREADX                TEXTEQU         <7:0>
;  -W-UF 
;  3132 : #define NV_UD3D0Z_FILTER_SPREADY                               15:8 /* -W-UF */
NV_UD3D0Z_FILTER_SPREADY                TEXTEQU         <15:8>
;  -W-SF 
;  3133 : #define NV_UD3D0Z_FILTER_MIPMAP                               23:16 /* -W-SF */
NV_UD3D0Z_FILTER_MIPMAP         TEXTEQU         <23:16>
;  -W-SF 
;  3134 : #define NV_UD3D0Z_FILTER_TURBO                                31:24 /* -W-SF */
NV_UD3D0Z_FILTER_TURBO          TEXTEQU         <31:24>
;  -W-4R 
;  3135 : #define NV_UD3D0Z_FOG_COLOR                              0x00570310 /* -W-4R */
NV_UD3D0Z_FOG_COLOR             EQU             000570310h
;  -W-UF 
;  3136 : #define NV_UD3D0Z_FOG_COLOR_BLU                                 7:0 /* -W-UF */
NV_UD3D0Z_FOG_COLOR_BLU         TEXTEQU         <7:0>
;  -W-UF 
;  3137 : #define NV_UD3D0Z_FOG_COLOR_GRN                                15:8 /* -W-UF */
NV_UD3D0Z_FOG_COLOR_GRN         TEXTEQU         <15:8>
;  -W-UF 
;  3138 : #define NV_UD3D0Z_FOG_COLOR_RED                               23:16 /* -W-UF */
NV_UD3D0Z_FOG_COLOR_RED         TEXTEQU         <23:16>
;  -W-UF 
;  3139 : #define NV_UD3D0Z_FOG_COLOR_VALUE                              31:0 /* -W-UF */
NV_UD3D0Z_FOG_COLOR_VALUE               TEXTEQU         <31:0>
;  -W-4R 
;  3140 : #define NV_UD3D0Z_CONTROL_OUT                            0x00570314 /* -W-4R */
NV_UD3D0Z_CONTROL_OUT           EQU             000570314h
;  -W-UF 
;  3141 : #define NV_UD3D0Z_CONTROL_OUT_INTERPOLATOR                      1:0 /* -W-UF */
NV_UD3D0Z_CONTROL_OUT_INTERPOLATOR              TEXTEQU         <1:0>
;  -W-UV 
;  3142 : #define NV_UD3D0Z_CONTROL_OUT_INTERPOLATOR_ZOH           0x00000000 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_INTERPOLATOR_ZOH          EQU             000000000h
;  -W-UV 
;  3143 : #define NV_UD3D0Z_CONTROL_OUT_INTERPOLATOR_ZOH_MS        0x00000001 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_INTERPOLATOR_ZOH_MS               EQU             000000001h
;  -W-UV 
;  3144 : #define NV_UD3D0Z_CONTROL_OUT_INTERPOLATOR_FOH           0x00000002 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_INTERPOLATOR_FOH          EQU             000000002h
;  -W-UF 
;  3145 : #define NV_UD3D0Z_CONTROL_OUT_WRAP_U                            5:4 /* -W-UF */
NV_UD3D0Z_CONTROL_OUT_WRAP_U            TEXTEQU         <5:4>
;  -W-UV 
;  3146 : #define NV_UD3D0Z_CONTROL_OUT_WRAP_U_CYLINDRICAL         0x00000000 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_WRAP_U_CYLINDRICAL                EQU             000000000h
;  -W-UV 
;  3147 : #define NV_UD3D0Z_CONTROL_OUT_WRAP_U_WRAP                0x00000001 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_WRAP_U_WRAP               EQU             000000001h
;  -W-UV 
;  3148 : #define NV_UD3D0Z_CONTROL_OUT_WRAP_U_MIRROR              0x00000002 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_WRAP_U_MIRROR             EQU             000000002h
;  -W-UV 
;  3149 : #define NV_UD3D0Z_CONTROL_OUT_WRAP_U_CLAMP               0x00000003 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_WRAP_U_CLAMP              EQU             000000003h
;  -W-UF 
;  3150 : #define NV_UD3D0Z_CONTROL_OUT_WRAP_V                            7:6 /* -W-UF */
NV_UD3D0Z_CONTROL_OUT_WRAP_V            TEXTEQU         <7:6>
;  -W-UV 
;  3151 : #define NV_UD3D0Z_CONTROL_OUT_WRAP_V_CYLINDRICAL         0x00000000 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_WRAP_V_CYLINDRICAL                EQU             000000000h
;  -W-UV 
;  3152 : #define NV_UD3D0Z_CONTROL_OUT_WRAP_V_WRAP                0x00000001 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_WRAP_V_WRAP               EQU             000000001h
;  -W-UV 
;  3153 : #define NV_UD3D0Z_CONTROL_OUT_WRAP_V_MIRROR              0x00000002 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_WRAP_V_MIRROR             EQU             000000002h
;  -W-UV 
;  3154 : #define NV_UD3D0Z_CONTROL_OUT_WRAP_V_CLAMP               0x00000003 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_WRAP_V_CLAMP              EQU             000000003h
;  -W-UF 
;  3155 : #define NV_UD3D0Z_CONTROL_OUT_COLOR_FORMAT                      8:8 /* -W-UF */
NV_UD3D0Z_CONTROL_OUT_COLOR_FORMAT              TEXTEQU         <8:8>
;  -W-UV 
;  3156 : #define NV_UD3D0Z_CONTROL_OUT_COLOR_FORMAT_LE_X8R8G8B8   0x00000000 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_COLOR_FORMAT_LE_X8R8G8B8          EQU             000000000h
;  -W-UV 
;  3157 : #define NV_UD3D0Z_CONTROL_OUT_COLOR_FORMAT_LE_A8R8G8B8   0x00000001 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_COLOR_FORMAT_LE_A8R8G8B8          EQU             000000001h
;  -W-UF 
;  3158 : #define NV_UD3D0Z_CONTROL_OUT_CULLING                         13:12 /* -W-UF */
NV_UD3D0Z_CONTROL_OUT_CULLING           TEXTEQU         <13:12>
;  -W-UV 
;  3159 : #define NV_UD3D0Z_CONTROL_OUT_CULLING_NONE               0x00000001 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_CULLING_NONE              EQU             000000001h
;  -W-UV 
;  3160 : #define NV_UD3D0Z_CONTROL_OUT_CULLING_CLOCKWISE          0x00000002 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_CULLING_CLOCKWISE         EQU             000000002h
;  -W-UV 
;  3161 : #define NV_UD3D0Z_CONTROL_OUT_CULLING_COUNTERCLOCKWISE   0x00000003 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_CULLING_COUNTERCLOCKWISE          EQU             000000003h
;  -W-UF 
;  3162 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE                    19:16 /* -W-UF */
NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE              TEXTEQU         <19:16>
;  -W-UV 
;  3163 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_FALSE         0x00000001 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_FALSE                EQU             000000001h
;  -W-UV 
;  3164 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_LT            0x00000002 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_LT           EQU             000000002h
;  -W-UV 
;  3165 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_EQ            0x00000003 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_EQ           EQU             000000003h
;  -W-UV 
;  3166 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_LE            0x00000004 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_LE           EQU             000000004h
;  -W-UV 
;  3167 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_GT            0x00000005 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_GT           EQU             000000005h
;  -W-UV 
;  3168 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_NE            0x00000006 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_NE           EQU             000000006h
;  -W-UV 
;  3169 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_GE            0x00000007 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_GE           EQU             000000007h
;  -W-UV 
;  3170 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_TRUE          0x00000008 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_TRUE         EQU             000000008h
;  -W-UF 
;  3171 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE                      22:20 /* -W-UF */
NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE                TEXTEQU         <22:20>
;  -W-UV 
;  3172 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_NEVER           0x00000000 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_NEVER          EQU             000000000h
;  -W-UV 
;  3173 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_ALPHA           0x00000001 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_ALPHA          EQU             000000001h
;  -W-UV 
;  3174 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_ALPHA_ZETA      0x00000002 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_ALPHA_ZETA             EQU             000000002h
;  -W-UV 
;  3175 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_ZETA            0x00000003 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_ZETA           EQU             000000003h
;  -W-UV 
;  3176 : #define NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_ALWAYS          0x00000004 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_ALWAYS         EQU             000000004h
;  -W-UF 
;  3177 : #define NV_UD3D0Z_CONTROL_OUT_COLOR_WRITE                     25:24 /* -W-UF */
NV_UD3D0Z_CONTROL_OUT_COLOR_WRITE               TEXTEQU         <25:24>
;  -W-UV 
;  3178 : #define NV_UD3D0Z_CONTROL_OUT_COLOR_WRITE_NEVER          0x00000000 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_COLOR_WRITE_NEVER         EQU             000000000h
;  -W-UV 
;  3179 : #define NV_UD3D0Z_CONTROL_OUT_COLOR_WRITE_ALPHA          0x00000001 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_COLOR_WRITE_ALPHA         EQU             000000001h
;  -W-UV 
;  3180 : #define NV_UD3D0Z_CONTROL_OUT_COLOR_WRITE_ALPHA_ZETA     0x00000002 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_COLOR_WRITE_ALPHA_ZETA            EQU             000000002h
;  -W-UF 
;  3181 : #define NV_UD3D0Z_CONTROL_OUT_ROP                             28:28 /* -W-UF */
NV_UD3D0Z_CONTROL_OUT_ROP               TEXTEQU         <28:28>
;  -W-UV 
;  3182 : #define NV_UD3D0Z_CONTROL_OUT_ROP_BLEND_AND              0x00000000 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ROP_BLEND_AND             EQU             000000000h
;  -W-UV 
;  3183 : #define NV_UD3D0Z_CONTROL_OUT_ROP_ADD_WITH_SATURATION    0x00000001 /* -W-UV */
NV_UD3D0Z_CONTROL_OUT_ROP_ADD_WITH_SATURATION           EQU             000000001h
;  -W-4A 
;        
;  3184 : #define NV_UD3D0Z_SPECULAR(i)                   (0x00571000+(i)*32) /* -W-4A */
;  3185 : #define NV_UD3D0Z_SPECULAR__SIZE_1                              128 /*       */
NV_UD3D0Z_SPECULAR__SIZE_1              EQU             128t
;  -W-UF 
;  3186 : #define NV_UD3D0Z_SPECULAR_I0                                   3:0 /* -W-UF */
NV_UD3D0Z_SPECULAR_I0           TEXTEQU         <3:0>
;  -W-UF 
;  3187 : #define NV_UD3D0Z_SPECULAR_I1                                   7:4 /* -W-UF */
NV_UD3D0Z_SPECULAR_I1           TEXTEQU         <7:4>
;  -W-UF 
;  3188 : #define NV_UD3D0Z_SPECULAR_I2                                  11:8 /* -W-UF */
NV_UD3D0Z_SPECULAR_I2           TEXTEQU         <11:8>
;  -W-UF 
;  3189 : #define NV_UD3D0Z_SPECULAR_I3                                 15:12 /* -W-UF */
NV_UD3D0Z_SPECULAR_I3           TEXTEQU         <15:12>
;  -W-UF 
;  3190 : #define NV_UD3D0Z_SPECULAR_I4                                 19:16 /* -W-UF */
NV_UD3D0Z_SPECULAR_I4           TEXTEQU         <19:16>
;  -W-UF 
;  3191 : #define NV_UD3D0Z_SPECULAR_I5                                 23:20 /* -W-UF */
NV_UD3D0Z_SPECULAR_I5           TEXTEQU         <23:20>
;  -W-UF 
;  3192 : #define NV_UD3D0Z_SPECULAR_FOG                                31:24 /* -W-UF */
NV_UD3D0Z_SPECULAR_FOG          TEXTEQU         <31:24>
;  -W-4A 
;        
;  3193 : #define NV_UD3D0Z_COLOR(i)                      (0x00571004+(i)*32) /* -W-4A */
;  3194 : #define NV_UD3D0Z_COLOR__SIZE_1                                 128 /*       */
NV_UD3D0Z_COLOR__SIZE_1         EQU             128t
;  -W-UF 
;  3195 : #define NV_UD3D0Z_COLOR_B8                                      7:0 /* -W-UF */
NV_UD3D0Z_COLOR_B8              TEXTEQU         <7:0>
;  -W-UF 
;  3196 : #define NV_UD3D0Z_COLOR_G8                                     16:8 /* -W-UF */
NV_UD3D0Z_COLOR_G8              TEXTEQU         <16:8>
;  -W-UF 
;  3197 : #define NV_UD3D0Z_COLOR_R8                                    23:16 /* -W-UF */
NV_UD3D0Z_COLOR_R8              TEXTEQU         <23:16>
;  -W-UF 
;  3198 : #define NV_UD3D0Z_COLOR_A8                                    32:24 /* -W-UF */
NV_UD3D0Z_COLOR_A8              TEXTEQU         <32:24>
;  -W-4A 
;        
;  3199 : #define NV_UD3D0Z_X(i)                          (0x00571008+(i)*32) /* -W-4A */
;  3200 : #define NV_UD3D0Z_X__SIZE_1                                     128 /*       */
NV_UD3D0Z_X__SIZE_1             EQU             128t
;  -W-FF 
;  3201 : #define NV_UD3D0Z_X_VALUE                                      31:0 /* -W-FF */
NV_UD3D0Z_X_VALUE               TEXTEQU         <31:0>
;  -W-4A 
;        
;  3202 : #define NV_UD3D0Z_Y(i)                          (0x0057100C+(i)*32) /* -W-4A */
;  3203 : #define NV_UD3D0Z_Y__SIZE_1                                     128 /*       */
NV_UD3D0Z_Y__SIZE_1             EQU             128t
;  -W-FF 
;  3204 : #define NV_UD3D0Z_Y_VALUE                                      31:0 /* -W-FF */
NV_UD3D0Z_Y_VALUE               TEXTEQU         <31:0>
;  -W-4A 
;        
;  3205 : #define NV_UD3D0Z_Z(i)                          (0x00571010+(i)*32) /* -W-4A */
;  3206 : #define NV_UD3D0Z_Z__SIZE_1                                     128 /*       */
NV_UD3D0Z_Z__SIZE_1             EQU             128t
;  -W-FF 
;  3207 : #define NV_UD3D0Z_Z_VALUE                                      31:0 /* -W-FF */
NV_UD3D0Z_Z_VALUE               TEXTEQU         <31:0>
;  -W-4A 
;        
;  3208 : #define NV_UD3D0Z_M(i)                          (0x00571014+(i)*32) /* -W-4A */
;  3209 : #define NV_UD3D0Z_M__SIZE_1                                     128 /*       */
NV_UD3D0Z_M__SIZE_1             EQU             128t
;  -W-FF 
;  3210 : #define NV_UD3D0Z_M_VALUE                                      31:0 /* -W-FF */
NV_UD3D0Z_M_VALUE               TEXTEQU         <31:0>
;  -W-4A 
;        
;  3211 : #define NV_UD3D0Z_U(i)                          (0x00571018+(i)*32) /* -W-4A */
;  3212 : #define NV_UD3D0Z_U__SIZE_1                                     128 /*       */
NV_UD3D0Z_U__SIZE_1             EQU             128t
;  -W-FF 
;  3213 : #define NV_UD3D0Z_U_VALUE                                      31:0 /* -W-FF */
NV_UD3D0Z_U_VALUE               TEXTEQU         <31:0>
;  -W-4A 
;        
;  3214 : #define NV_UD3D0Z_V(i)                          (0x0057101c+(i)*32) /* -W-4A */
;  3215 : #define NV_UD3D0Z_V__SIZE_1                                     128 /*       */
NV_UD3D0Z_V__SIZE_1             EQU             128t
;  -W-FF 
;  3216 : #define NV_UD3D0Z_V_VALUE                                      31:0 /* -W-FF */
NV_UD3D0Z_V_VALUE               TEXTEQU         <31:0>
;  usr_d3d0_triangle.ref 
;  -W--D 
;  3217 : /* usr_d3d0_triangle.ref */
;  3218 : #define NV_UD3D0                              0x00561FFF:0x00560000 /* -W--D */
NV_UD3D0                TEXTEQU         <0x00561FFF:0x00560000>
;  -W-4R 
;  3219 : #define NV_UD3D0_CTX_SWITCH                              0x00560000 /* -W-4R */
NV_UD3D0_CTX_SWITCH             EQU             000560000h
;  -W-UF 
;  3220 : #define NV_UD3D0_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
NV_UD3D0_CTX_SWITCH_INSTANCE            TEXTEQU         <15:0>
;  -W-UF 
;  3221 : #define NV_UD3D0_CTX_SWITCH_CHID                              22:16 /* -W-UF */
NV_UD3D0_CTX_SWITCH_CHID                TEXTEQU         <22:16>
;  -W-VF 
;  3222 : #define NV_UD3D0_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
NV_UD3D0_CTX_SWITCH_VOLATILE            TEXTEQU         <31:31>
;  -W--V 
;  3223 : #define NV_UD3D0_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
NV_UD3D0_CTX_SWITCH_VOLATILE_IGNORE             EQU             000000000h
;  -W--V 
;  3224 : #define NV_UD3D0_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
NV_UD3D0_CTX_SWITCH_VOLATILE_RESET              EQU             000000001h
;  -W-4R 
;  3225 : #define NV_UD3D0_SET_NOTIFY                              0x00560104 /* -W-4R */
NV_UD3D0_SET_NOTIFY             EQU             000560104h
;  -W-VF 
;  3226 : #define NV_UD3D0_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
NV_UD3D0_SET_NOTIFY_PARAMETER           TEXTEQU         <31:0>
;  -W--V 
;  3227 : #define NV_UD3D0_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
NV_UD3D0_SET_NOTIFY_PARAMETER_WRITE             EQU             000000000h
;  -W-4R 
;  3228 : #define NV_UD3D0_TEXTURE_OFFSET                          0x00560304 /* -W-4R */
NV_UD3D0_TEXTURE_OFFSET         EQU             000560304h
;  -W-UF 
;  3229 : #define NV_UD3D0_TEXTURE_OFFSET_VALUE                          31:0 /* -W-UF */
NV_UD3D0_TEXTURE_OFFSET_VALUE           TEXTEQU         <31:0>
;  -W-4R 
;  3230 : #define NV_UD3D0_TEXTURE_FORMAT                          0x00560308 /* -W-4R */
NV_UD3D0_TEXTURE_FORMAT         EQU             000560308h
;  -W-UF 
;  3231 : #define NV_UD3D0_TEXTURE_FORMAT_COLOR_KEY_COLOR_MASK           15:0 /* -W-UF */
NV_UD3D0_TEXTURE_FORMAT_COLOR_KEY_COLOR_MASK            TEXTEQU         <15:0>
;  -W-UF 
;  3232 : #define NV_UD3D0_TEXTURE_FORMAT_COLOR_KEY                     16:16 /* -W-UF */
NV_UD3D0_TEXTURE_FORMAT_COLOR_KEY               TEXTEQU         <16:16>
;  -W-UV 
;  3233 : #define NV_UD3D0_TEXTURE_FORMAT_COLOR_KEY_DISABLED       0x00000000 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_COLOR_KEY_DISABLED              EQU             000000000h
;  -W-UV 
;  3234 : #define NV_UD3D0_TEXTURE_FORMAT_COLOR_KEY_ENABLED        0x00000001 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_COLOR_KEY_ENABLED               EQU             000000001h
;  -W-UF 
;  3235 : #define NV_UD3D0_TEXTURE_FORMAT_COLOR_FORMAT                  21:20 /* -W-UF */
NV_UD3D0_TEXTURE_FORMAT_COLOR_FORMAT            TEXTEQU         <21:20>
;  -W-UV 
;  3236 : #define NV_UD3D0_TEXTURE_FORMAT_COLOR_FORMAT_A1R5G5B5    0x00000000 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_COLOR_FORMAT_A1R5G5B5           EQU             000000000h
;  -W-UV 
;  3237 : #define NV_UD3D0_TEXTURE_FORMAT_COLOR_FORMAT_X1R5G5B5    0x00000001 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_COLOR_FORMAT_X1R5G5B5           EQU             000000001h
;  -W-UV 
;  3238 : #define NV_UD3D0_TEXTURE_FORMAT_COLOR_FORMAT_A4R4G4G4    0x00000002 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_COLOR_FORMAT_A4R4G4G4           EQU             000000002h
;  -W-UV 
;  3239 : #define NV_UD3D0_TEXTURE_FORMAT_COLOR_FORMAT_R5G6B5      0x00000003 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_COLOR_FORMAT_R5G6B5             EQU             000000003h
;  -W-UF 
;  3240 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN                      27:24 /* -W-UF */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN                TEXTEQU         <27:24>
;  -W-UV 
;  3241 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_1X1             0x00000000 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_1X1            EQU             000000000h
;  -W-UV 
;  3242 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_2X2             0x00000001 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_2X2            EQU             000000001h
;  -W-UV 
;  3243 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_4X4             0x00000002 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_4X4            EQU             000000002h
;  -W-UV 
;  3244 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_8X8             0x00000003 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_8X8            EQU             000000003h
;  -W-UV 
;  3245 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_16X16           0x00000004 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_16X16          EQU             000000004h
;  -W-UV 
;  3246 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_32X32           0x00000005 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_32X32          EQU             000000005h
;  -W-UV 
;  3247 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_64X64           0x00000006 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_64X64          EQU             000000006h
;  -W-UV 
;  3248 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_128X128         0x00000007 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_128X128                EQU             000000007h
;  -W-UV 
;  3249 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_256X256         0x00000008 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_256X256                EQU             000000008h
;  -W-UV 
;  3250 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_512X512         0x00000009 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_512X512                EQU             000000009h
;  -W-UV 
;  3251 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_1024X1024       0x0000000a /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_1024X1024              EQU             00000000ah
;  -W-UV 
;  3252 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_2048X2048       0x0000000b /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MIN_2048X2048              EQU             00000000bh
;  -W-UF 
;  3253 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX                      31:28 /* -W-UF */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX                TEXTEQU         <31:28>
;  -W-UV 
;  3254 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_1X1             0x00000000 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_1X1            EQU             000000000h
;  -W-UV 
;  3255 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_2X2             0x00000001 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_2X2            EQU             000000001h
;  -W-UV 
;  3256 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_4X4             0x00000002 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_4X4            EQU             000000002h
;  -W-UV 
;  3257 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_8X8             0x00000003 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_8X8            EQU             000000003h
;  -W-UV 
;  3258 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_16X16           0x00000004 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_16X16          EQU             000000004h
;  -W-UV 
;  3259 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_32X32           0x00000005 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_32X32          EQU             000000005h
;  -W-UV 
;  3260 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_64X64           0x00000006 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_64X64          EQU             000000006h
;  -W-UV 
;  3261 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_128X128         0x00000007 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_128X128                EQU             000000007h
;  -W-UV 
;  3262 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_256X256         0x00000008 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_256X256                EQU             000000008h
;  -W-UV 
;  3263 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_512X512         0x00000009 /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_512X512                EQU             000000009h
;  -W-UV 
;  3264 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_1024X1024       0x0000000a /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_1024X1024              EQU             00000000ah
;  -W-UV 
;  3265 : #define NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_2048X2048       0x0000000b /* -W-UV */
NV_UD3D0_TEXTURE_FORMAT_SIZE_MAX_2048X2048              EQU             00000000bh
;  -W-4R 
;  3266 : #define NV_UD3D0_FILTER                                  0x0056030C /* -W-4R */
NV_UD3D0_FILTER         EQU             00056030ch
;  -W-UF 
;  3267 : #define NV_UD3D0_FILTER_SPREADX                                 7:0 /* -W-UF */
NV_UD3D0_FILTER_SPREADX         TEXTEQU         <7:0>
;  -W-UF 
;  3268 : #define NV_UD3D0_FILTER_SPREADY                                15:8 /* -W-UF */
NV_UD3D0_FILTER_SPREADY         TEXTEQU         <15:8>
;  -W-SF 
;  3269 : #define NV_UD3D0_FILTER_MIPMAP                                23:16 /* -W-SF */
NV_UD3D0_FILTER_MIPMAP          TEXTEQU         <23:16>
;  -W-SF 
;  3270 : #define NV_UD3D0_FILTER_TURBO                                 31:24 /* -W-SF */
NV_UD3D0_FILTER_TURBO           TEXTEQU         <31:24>
;  -W-4R 
;  3271 : #define NV_UD3D0_FOG_COLOR                               0x00560310 /* -W-4R */
NV_UD3D0_FOG_COLOR              EQU             000560310h
;  -W-UF 
;  3272 : #define NV_UD3D0_FOG_COLOR_VALUE                               31:0 /* -W-UF */
NV_UD3D0_FOG_COLOR_VALUE                TEXTEQU         <31:0>
;  -W-4R 
;  3273 : #define NV_UD3D0_CONTROL_OUT                             0x00560314 /* -W-4R */
NV_UD3D0_CONTROL_OUT            EQU             000560314h
;  -W-UF 
;  3274 : #define NV_UD3D0_CONTROL_OUT_INTERPOLATOR                       1:0 /* -W-UF */
NV_UD3D0_CONTROL_OUT_INTERPOLATOR               TEXTEQU         <1:0>
;  -W-UV 
;  3275 : #define NV_UD3D0_CONTROL_OUT_INTERPOLATOR_ZOH            0x00000000 /* -W-UV */
NV_UD3D0_CONTROL_OUT_INTERPOLATOR_ZOH           EQU             000000000h
;  -W-UV 
;  3276 : #define NV_UD3D0_CONTROL_OUT_INTERPOLATOR_ZOH_MS         0x00000001 /* -W-UV */
NV_UD3D0_CONTROL_OUT_INTERPOLATOR_ZOH_MS                EQU             000000001h
;  -W-UV 
;  3277 : #define NV_UD3D0_CONTROL_OUT_INTERPOLATOR_FOH            0x00000002 /* -W-UV */
NV_UD3D0_CONTROL_OUT_INTERPOLATOR_FOH           EQU             000000002h
;  -W-UF 
;  3278 : #define NV_UD3D0_CONTROL_OUT_WRAP_U                             5:4 /* -W-UF */
NV_UD3D0_CONTROL_OUT_WRAP_U             TEXTEQU         <5:4>
;  -W-UV 
;  3279 : #define NV_UD3D0_CONTROL_OUT_WRAP_U_CYLINDRICAL          0x00000000 /* -W-UV */
NV_UD3D0_CONTROL_OUT_WRAP_U_CYLINDRICAL         EQU             000000000h
;  -W-UV 
;  3280 : #define NV_UD3D0_CONTROL_OUT_WRAP_U_WRAP                 0x00000001 /* -W-UV */
NV_UD3D0_CONTROL_OUT_WRAP_U_WRAP                EQU             000000001h
;  -W-UV 
;  3281 : #define NV_UD3D0_CONTROL_OUT_WRAP_U_MIRROR               0x00000002 /* -W-UV */
NV_UD3D0_CONTROL_OUT_WRAP_U_MIRROR              EQU             000000002h
;  -W-UV 
;  3282 : #define NV_UD3D0_CONTROL_OUT_WRAP_U_CLAMP                0x00000003 /* -W-UV */
NV_UD3D0_CONTROL_OUT_WRAP_U_CLAMP               EQU             000000003h
;  -W-UF 
;  3283 : #define NV_UD3D0_CONTROL_OUT_WRAP_V                             7:6 /* -W-UF */
NV_UD3D0_CONTROL_OUT_WRAP_V             TEXTEQU         <7:6>
;  -W-UV 
;  3284 : #define NV_UD3D0_CONTROL_OUT_WRAP_V_CYLINDRICAL          0x00000000 /* -W-UV */
NV_UD3D0_CONTROL_OUT_WRAP_V_CYLINDRICAL         EQU             000000000h
;  -W-UV 
;  3285 : #define NV_UD3D0_CONTROL_OUT_WRAP_V_WRAP                 0x00000001 /* -W-UV */
NV_UD3D0_CONTROL_OUT_WRAP_V_WRAP                EQU             000000001h
;  -W-UV 
;  3286 : #define NV_UD3D0_CONTROL_OUT_WRAP_V_MIRROR               0x00000002 /* -W-UV */
NV_UD3D0_CONTROL_OUT_WRAP_V_MIRROR              EQU             000000002h
;  -W-UV 
;  3287 : #define NV_UD3D0_CONTROL_OUT_WRAP_V_CLAMP                0x00000003 /* -W-UV */
NV_UD3D0_CONTROL_OUT_WRAP_V_CLAMP               EQU             000000003h
;  -W-UF 
;  3288 : #define NV_UD3D0_CONTROL_OUT_COLOR_FORMAT                       8:8 /* -W-UF */
NV_UD3D0_CONTROL_OUT_COLOR_FORMAT               TEXTEQU         <8:8>
;  -W-UV 
;  3289 : #define NV_UD3D0_CONTROL_OUT_COLOR_FORMAT_LE_X8R8G8B8    0x00000000 /* -W-UV */
NV_UD3D0_CONTROL_OUT_COLOR_FORMAT_LE_X8R8G8B8           EQU             000000000h
;  -W-UV 
;  3290 : #define NV_UD3D0_CONTROL_OUT_COLOR_FORMAT_LE_A8R8G8B8    0x00000001 /* -W-UV */
NV_UD3D0_CONTROL_OUT_COLOR_FORMAT_LE_A8R8G8B8           EQU             000000001h
;  -W-UF 
;  3291 : #define NV_UD3D0_CONTROL_OUT_CULLING                          13:12 /* -W-UF */
NV_UD3D0_CONTROL_OUT_CULLING            TEXTEQU         <13:12>
;  -W-UV 
;  3292 : #define NV_UD3D0_CONTROL_OUT_CULLING_NONE                0x00000001 /* -W-UV */
NV_UD3D0_CONTROL_OUT_CULLING_NONE               EQU             000000001h
;  -W-UV 
;  3293 : #define NV_UD3D0_CONTROL_OUT_CULLING_CLOCKWISE           0x00000002 /* -W-UV */
NV_UD3D0_CONTROL_OUT_CULLING_CLOCKWISE          EQU             000000002h
;  -W-UV 
;  3294 : #define NV_UD3D0_CONTROL_OUT_CULLING_COUNTERCLOCKWISE    0x00000003 /* -W-UV */
NV_UD3D0_CONTROL_OUT_CULLING_COUNTERCLOCKWISE           EQU             000000003h
;  -W-UF 
;  3295 : #define NV_UD3D0_CONTROL_OUT_ZETA_COMPARE                     19:16 /* -W-UF */
NV_UD3D0_CONTROL_OUT_ZETA_COMPARE               TEXTEQU         <19:16>
;  -W-UV 
;  3296 : #define NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_FALSE          0x00000001 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_FALSE         EQU             000000001h
;  -W-UV 
;  3297 : #define NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_LT             0x00000002 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_LT            EQU             000000002h
;  -W-UV 
;  3298 : #define NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_EQ             0x00000003 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_EQ            EQU             000000003h
;  -W-UV 
;  3299 : #define NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_LE             0x00000004 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_LE            EQU             000000004h
;  -W-UV 
;  3300 : #define NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_GT             0x00000005 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_GT            EQU             000000005h
;  -W-UV 
;  3301 : #define NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_NE             0x00000006 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_NE            EQU             000000006h
;  -W-UV 
;  3302 : #define NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_GE             0x00000007 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_GE            EQU             000000007h
;  -W-UV 
;  3303 : #define NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_TRUE           0x00000008 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_COMPARE_TRUE          EQU             000000008h
;  -W-UF 
;  3304 : #define NV_UD3D0_CONTROL_OUT_ZETA_WRITE                       22:20 /* -W-UF */
NV_UD3D0_CONTROL_OUT_ZETA_WRITE         TEXTEQU         <22:20>
;  -W-UV 
;  3305 : #define NV_UD3D0_CONTROL_OUT_ZETA_WRITE_NEVER            0x00000000 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_WRITE_NEVER           EQU             000000000h
;  -W-UV 
;  3306 : #define NV_UD3D0_CONTROL_OUT_ZETA_WRITE_ALPHA            0x00000001 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_WRITE_ALPHA           EQU             000000001h
;  -W-UV 
;  3307 : #define NV_UD3D0_CONTROL_OUT_ZETA_WRITE_ALPHA_ZETA       0x00000002 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_WRITE_ALPHA_ZETA              EQU             000000002h
;  -W-UV 
;  3308 : #define NV_UD3D0_CONTROL_OUT_ZETA_WRITE_ZETA             0x00000003 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_WRITE_ZETA            EQU             000000003h
;  -W-UV 
;  3309 : #define NV_UD3D0_CONTROL_OUT_ZETA_WRITE_ALWAYS           0x00000004 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ZETA_WRITE_ALWAYS          EQU             000000004h
;  -W-UF 
;  3310 : #define NV_UD3D0_CONTROL_OUT_COLOR_WRITE                      25:24 /* -W-UF */
NV_UD3D0_CONTROL_OUT_COLOR_WRITE                TEXTEQU         <25:24>
;  -W-UV 
;  3311 : #define NV_UD3D0_CONTROL_OUT_COLOR_WRITE_NEVER           0x00000000 /* -W-UV */
NV_UD3D0_CONTROL_OUT_COLOR_WRITE_NEVER          EQU             000000000h
;  -W-UV 
;  3312 : #define NV_UD3D0_CONTROL_OUT_COLOR_WRITE_ALPHA           0x00000001 /* -W-UV */
NV_UD3D0_CONTROL_OUT_COLOR_WRITE_ALPHA          EQU             000000001h
;  -W-UV 
;  3313 : #define NV_UD3D0_CONTROL_OUT_COLOR_WRITE_ALPHA_ZETA      0x00000002 /* -W-UV */
NV_UD3D0_CONTROL_OUT_COLOR_WRITE_ALPHA_ZETA             EQU             000000002h
;  -W-UF 
;  3314 : #define NV_UD3D0_CONTROL_OUT_ROP                              28:28 /* -W-UF */
NV_UD3D0_CONTROL_OUT_ROP                TEXTEQU         <28:28>
;  -W-UV 
;  3315 : #define NV_UD3D0_CONTROL_OUT_ROP_BLEND_AND               0x00000000 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ROP_BLEND_AND              EQU             000000000h
;  -W-UV 
;  3316 : #define NV_UD3D0_CONTROL_OUT_ROP_ADD_WITH_SATURATION     0x00000001 /* -W-UV */
NV_UD3D0_CONTROL_OUT_ROP_ADD_WITH_SATURATION            EQU             000000001h
;  -W-4A 
;        
;  3317 : #define NV_UD3D0_SPECULAR(i)                    (0x00561000+(i)*32) /* -W-4A */
;  3318 : #define NV_UD3D0_SPECULAR__SIZE_1                               128 /*       */
NV_UD3D0_SPECULAR__SIZE_1               EQU             128t
;  -W-UF 
;  3319 : #define NV_UD3D0_SPECULAR_I0                                    3:0 /* -W-UF */
NV_UD3D0_SPECULAR_I0            TEXTEQU         <3:0>
;  -W-UF 
;  3320 : #define NV_UD3D0_SPECULAR_I1                                    7:4 /* -W-UF */
NV_UD3D0_SPECULAR_I1            TEXTEQU         <7:4>
;  -W-UF 
;  3321 : #define NV_UD3D0_SPECULAR_I2                                   11:8 /* -W-UF */
NV_UD3D0_SPECULAR_I2            TEXTEQU         <11:8>
;  -W-UF 
;  3322 : #define NV_UD3D0_SPECULAR_I3                                  15:12 /* -W-UF */
NV_UD3D0_SPECULAR_I3            TEXTEQU         <15:12>
;  -W-UF 
;  3323 : #define NV_UD3D0_SPECULAR_I4                                  19:16 /* -W-UF */
NV_UD3D0_SPECULAR_I4            TEXTEQU         <19:16>
;  -W-UF 
;  3324 : #define NV_UD3D0_SPECULAR_I5                                  23:20 /* -W-UF */
NV_UD3D0_SPECULAR_I5            TEXTEQU         <23:20>
;  -W-UF 
;  3325 : #define NV_UD3D0_SPECULAR_FOG                                 31:24 /* -W-UF */
NV_UD3D0_SPECULAR_FOG           TEXTEQU         <31:24>
;  -W-4A 
;        
;  3326 : #define NV_UD3D0_COLOR(i)                       (0x00561004+(i)*32) /* -W-4A */
;  3327 : #define NV_UD3D0_COLOR__SIZE_1                                  128 /*       */
NV_UD3D0_COLOR__SIZE_1          EQU             128t
;  -W-UF 
;  3328 : #define NV_UD3D0_COLOR_B8                                       7:0 /* -W-UF */
NV_UD3D0_COLOR_B8               TEXTEQU         <7:0>
;  -W-UF 
;  3329 : #define NV_UD3D0_COLOR_G8                                      16:8 /* -W-UF */
NV_UD3D0_COLOR_G8               TEXTEQU         <16:8>
;  -W-UF 
;  3330 : #define NV_UD3D0_COLOR_R8                                     23:16 /* -W-UF */
NV_UD3D0_COLOR_R8               TEXTEQU         <23:16>
;  -W-UF 
;  3331 : #define NV_UD3D0_COLOR_A8                                     32:24 /* -W-UF */
NV_UD3D0_COLOR_A8               TEXTEQU         <32:24>
;  -W-4A 
;        
;  3332 : #define NV_UD3D0_X(i)                           (0x00561008+(i)*32) /* -W-4A */
;  3333 : #define NV_UD3D0_X__SIZE_1                                      128 /*       */
NV_UD3D0_X__SIZE_1              EQU             128t
;  -W-FF 
;  3334 : #define NV_UD3D0_X_VALUE                                       31:0 /* -W-FF */
NV_UD3D0_X_VALUE                TEXTEQU         <31:0>
;  -W-4A 
;        
;  3335 : #define NV_UD3D0_Y(i)                           (0x0056100C+(i)*32) /* -W-4A */
;  3336 : #define NV_UD3D0_Y__SIZE_1                                      128 /*       */
NV_UD3D0_Y__SIZE_1              EQU             128t
;  -W-FF 
;  3337 : #define NV_UD3D0_Y_VALUE                                       31:0 /* -W-FF */
NV_UD3D0_Y_VALUE                TEXTEQU         <31:0>
;  -W-4A 
;        
;  3338 : #define NV_UD3D0_Z(i)                           (0x00561010+(i)*32) /* -W-4A */
;  3339 : #define NV_UD3D0_Z__SIZE_1                                      128 /*       */
NV_UD3D0_Z__SIZE_1              EQU             128t
;  -W-FF 
;  3340 : #define NV_UD3D0_Z_VALUE                                       31:0 /* -W-FF */
NV_UD3D0_Z_VALUE                TEXTEQU         <31:0>
;  -W-4A 
;        
;  3341 : #define NV_UD3D0_M(i)                           (0x00561014+(i)*32) /* -W-4A */
;  3342 : #define NV_UD3D0_M__SIZE_1                                      128 /*       */
NV_UD3D0_M__SIZE_1              EQU             128t
;  -W-FF 
;  3343 : #define NV_UD3D0_M_VALUE                                       31:0 /* -W-FF */
NV_UD3D0_M_VALUE                TEXTEQU         <31:0>
;  -W-4A 
;        
;  3344 : #define NV_UD3D0_U(i)                           (0x00561018+(i)*32) /* -W-4A */
;  3345 : #define NV_UD3D0_U__SIZE_1                                      128 /*       */
NV_UD3D0_U__SIZE_1              EQU             128t
;  -W-FF 
;  3346 : #define NV_UD3D0_U_VALUE                                       31:0 /* -W-FF */
NV_UD3D0_U_VALUE                TEXTEQU         <31:0>
;  -W-4A 
;        
;  3347 : #define NV_UD3D0_V(i)                           (0x0056101c+(i)*32) /* -W-4A */
;  3348 : #define NV_UD3D0_V__SIZE_1                                      128 /*       */
NV_UD3D0_V__SIZE_1              EQU             128t
;  -W-FF 
;  3349 : #define NV_UD3D0_V_VALUE                                       31:0 /* -W-FF */
NV_UD3D0_V_VALUE                TEXTEQU         <31:0>
;  usr_pattern.ref 
;  -W--D 
;  3350 : /* usr_pattern.ref */
;  3351 : #define NV_UPATT                              0x00461FFF:0x00460000 /* -W--D */
NV_UPATT                TEXTEQU         <0x00461FFF:0x00460000>
;  -W-4R 
;  3352 : #define NV_UPATT_CTX_SWITCH                              0x00460000 /* -W-4R */
NV_UPATT_CTX_SWITCH             EQU             000460000h
;  -W-UF 
;  3353 : #define NV_UPATT_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
NV_UPATT_CTX_SWITCH_INSTANCE            TEXTEQU         <15:0>
;  -W-UF 
;  3354 : #define NV_UPATT_CTX_SWITCH_CHID                              22:16 /* -W-UF */
NV_UPATT_CTX_SWITCH_CHID                TEXTEQU         <22:16>
;  -W-VF 
;  3355 : #define NV_UPATT_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
NV_UPATT_CTX_SWITCH_VOLATILE            TEXTEQU         <31:31>
;  -W--V 
;  3356 : #define NV_UPATT_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
NV_UPATT_CTX_SWITCH_VOLATILE_IGNORE             EQU             000000000h
;  -W--V 
;  3357 : #define NV_UPATT_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
NV_UPATT_CTX_SWITCH_VOLATILE_RESET              EQU             000000001h
;  -W-4R 
;  3358 : #define NV_UPATT_SET_NOTIFY                              0x00460104 /* -W-4R */
NV_UPATT_SET_NOTIFY             EQU             000460104h
;  -W-VF 
;  3359 : #define NV_UPATT_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
NV_UPATT_SET_NOTIFY_PARAMETER           TEXTEQU         <31:0>
;  -W--V 
;  3360 : #define NV_UPATT_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
NV_UPATT_SET_NOTIFY_PARAMETER_WRITE             EQU             000000000h
;  -W-4R 
;  3361 : #define NV_UPATT_SET_SHAPE                               0x00460308 /* -W-4R */
NV_UPATT_SET_SHAPE              EQU             000460308h
;  -W-VF 
;  3362 : #define NV_UPATT_SET_SHAPE_VALUE                                1:0 /* -W-VF */
NV_UPATT_SET_SHAPE_VALUE                TEXTEQU         <1:0>
;  -W--V 
;  3363 : #define NV_UPATT_SET_SHAPE_VALUE_8X8                     0x00000000 /* -W--V */
NV_UPATT_SET_SHAPE_VALUE_8X8            EQU             000000000h
;  -W--V 
;  3364 : #define NV_UPATT_SET_SHAPE_VALUE_64X1                    0x00000001 /* -W--V */
NV_UPATT_SET_SHAPE_VALUE_64X1           EQU             000000001h
;  -W--V 
;  3365 : #define NV_UPATT_SET_SHAPE_VALUE_1X64                    0x00000002 /* -W--V */
NV_UPATT_SET_SHAPE_VALUE_1X64           EQU             000000002h
;  -W-4R 
;  3366 : #define NV_UPATT_SET_COLOR0                              0x00460310 /* -W-4R */
NV_UPATT_SET_COLOR0             EQU             000460310h
;  -W-VF 
;  3367 : #define NV_UPATT_SET_COLOR0_VALUE                              31:0 /* -W-VF */
NV_UPATT_SET_COLOR0_VALUE               TEXTEQU         <31:0>
;  -W-4R 
;  3368 : #define NV_UPATT_SET_COLOR1                              0x00460314 /* -W-4R */
NV_UPATT_SET_COLOR1             EQU             000460314h
;  -W-VF 
;  3369 : #define NV_UPATT_SET_COLOR1_VALUE                              31:0 /* -W-VF */
NV_UPATT_SET_COLOR1_VALUE               TEXTEQU         <31:0>
;  -W-4A 
;        
;  3370 : #define NV_UPATT_SET_PATTERN(i)                  (0x00460318+(i)*4) /* -W-4A */
;  3371 : #define NV_UPATT_SET_PATTERN__SIZE_1                              2 /*       */
NV_UPATT_SET_PATTERN__SIZE_1            EQU             2t
;  -W-VF 
;  3372 : #define NV_UPATT_SET_PATTERN_BITMAP                            31:0 /* -W-VF */
NV_UPATT_SET_PATTERN_BITMAP             TEXTEQU         <31:0>
;  usr_point.ref 
;  -W--D 
;  3373 : /* usr_point.ref */
;  3374 : #define NV_UPOINT                             0x00481FFF:0x00480000 /* -W--D */
NV_UPOINT               TEXTEQU         <0x00481FFF:0x00480000>
;  -W-4R 
;  3375 : #define NV_UPOINT_CTX_SWITCH                             0x00480000 /* -W-4R */
NV_UPOINT_CTX_SWITCH            EQU             000480000h
;  -W-UF 
;  3376 : #define NV_UPOINT_CTX_SWITCH_INSTANCE                          15:0 /* -W-UF */
NV_UPOINT_CTX_SWITCH_INSTANCE           TEXTEQU         <15:0>
;  -W-UF 
;  3377 : #define NV_UPOINT_CTX_SWITCH_CHID                             22:16 /* -W-UF */
NV_UPOINT_CTX_SWITCH_CHID               TEXTEQU         <22:16>
;  -W-VF 
;  3378 : #define NV_UPOINT_CTX_SWITCH_VOLATILE                         31:31 /* -W-VF */
NV_UPOINT_CTX_SWITCH_VOLATILE           TEXTEQU         <31:31>
;  -W--V 
;  3379 : #define NV_UPOINT_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* -W--V */
NV_UPOINT_CTX_SWITCH_VOLATILE_IGNORE            EQU             000000000h
;  -W--V 
;  3380 : #define NV_UPOINT_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
NV_UPOINT_CTX_SWITCH_VOLATILE_RESET             EQU             000000001h
;  -W-4R 
;  3381 : #define NV_UPOINT_SET_NOTIFY                             0x00480104 /* -W-4R */
NV_UPOINT_SET_NOTIFY            EQU             000480104h
;  -W-VF 
;  3382 : #define NV_UPOINT_SET_NOTIFY_PARAMETER                         31:0 /* -W-VF */
NV_UPOINT_SET_NOTIFY_PARAMETER          TEXTEQU         <31:0>
;  -W--V 
;  3383 : #define NV_UPOINT_SET_NOTIFY_PARAMETER_WRITE             0x00000000 /* -W--V */
NV_UPOINT_SET_NOTIFY_PARAMETER_WRITE            EQU             000000000h
;  -W-4R 
;  3384 : #define NV_UPOINT_COLOR                                  0x00480304 /* -W-4R */
NV_UPOINT_COLOR         EQU             000480304h
;  -W-VF 
;  3385 : #define NV_UPOINT_COLOR_VALUE                                  31:0 /* -W-VF */
NV_UPOINT_COLOR_VALUE           TEXTEQU         <31:0>
;  -W-4A 
;        
;  3386 : #define NV_UPOINT_POINT(i)                       (0x00480400+(i)*4) /* -W-4A */
;  3387 : #define NV_UPOINT_POINT__SIZE_1                                  32 /*       */
NV_UPOINT_POINT__SIZE_1         EQU             32t
;  -W-SF 
;  3388 : #define NV_UPOINT_POINT_X                                      15:0 /* -W-SF */
NV_UPOINT_POINT_X               TEXTEQU         <15:0>
;  -W-SF 
;  3389 : #define NV_UPOINT_POINT_Y                                     31:16 /* -W-SF */
NV_UPOINT_POINT_Y               TEXTEQU         <31:16>
;  -W-4A 
;        
;  3390 : #define NV_UPOINT_POINT32_0(i)                   (0x00480480+(i)*8) /* -W-4A */
;  3391 : #define NV_UPOINT_POINT32_0__SIZE_1                              16 /*       */
NV_UPOINT_POINT32_0__SIZE_1             EQU             16t
;  -W-SF 
;  3392 : #define NV_UPOINT_POINT32_0_X                                  31:0 /* -W-SF */
NV_UPOINT_POINT32_0_X           TEXTEQU         <31:0>
;  -W-4A 
;        
;  3393 : #define NV_UPOINT_POINT32_1(i)                   (0x00480484+(i)*8) /* -W-4A */
;  3394 : #define NV_UPOINT_POINT32_1__SIZE_1                              16 /*       */
NV_UPOINT_POINT32_1__SIZE_1             EQU             16t
;  -W-SF 
;  3395 : #define NV_UPOINT_POINT32_1_Y                                  31:0 /* -W-SF */
NV_UPOINT_POINT32_1_Y           TEXTEQU         <31:0>
;  -W-4A 
;        
;  3396 : #define NV_UPOINT_CPOINT_0(i)                    (0x00480500+(i)*8) /* -W-4A */
;  3397 : #define NV_UPOINT_CPOINT_0__SIZE_1                               16 /*       */
NV_UPOINT_CPOINT_0__SIZE_1              EQU             16t
;  -W-VF 
;  3398 : #define NV_UPOINT_CPOINT_0_COLOR                               31:0 /* -W-VF */
NV_UPOINT_CPOINT_0_COLOR                TEXTEQU         <31:0>
;  -W-4A 
;        
;  3399 : #define NV_UPOINT_CPOINT_1(i)                    (0x00480504+(i)*8) /* -W-4A */
;  3400 : #define NV_UPOINT_CPOINT_1__SIZE_1                               16 /*       */
NV_UPOINT_CPOINT_1__SIZE_1              EQU             16t
;  -W-SF 
;  3401 : #define NV_UPOINT_CPOINT_1_X                                   15:0 /* -W-SF */
NV_UPOINT_CPOINT_1_X            TEXTEQU         <15:0>
;  -W-SF 
;  3402 : #define NV_UPOINT_CPOINT_1_Y                                  31:16 /* -W-SF */
NV_UPOINT_CPOINT_1_Y            TEXTEQU         <31:16>
;  usr_pointz.ref 
;  -W--D 
;  3403 : /* usr_pointz.ref */
;  3404 : #define NV_UPOINTZ                            0x00581FFF:0x00580000 /* -W--D */
NV_UPOINTZ              TEXTEQU         <0x00581FFF:0x00580000>
;  -W-4R 
;  3405 : #define NV_UPOINTZ_CTX_SWITCH                            0x00580000 /* -W-4R */
NV_UPOINTZ_CTX_SWITCH           EQU             000580000h
;  -W-UF 
;  3406 : #define NV_UPOINTZ_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
NV_UPOINTZ_CTX_SWITCH_INSTANCE          TEXTEQU         <15:0>
;  -W-UF 
;  3407 : #define NV_UPOINTZ_CTX_SWITCH_CHID                            22:16 /* -W-UF */
NV_UPOINTZ_CTX_SWITCH_CHID              TEXTEQU         <22:16>
;  -W-VF 
;  3408 : #define NV_UPOINTZ_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
NV_UPOINTZ_CTX_SWITCH_VOLATILE          TEXTEQU         <31:31>
;  -W--V 
;  3409 : #define NV_UPOINTZ_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
NV_UPOINTZ_CTX_SWITCH_VOLATILE_IGNORE           EQU             000000000h
;  -W--V 
;  3410 : #define NV_UPOINTZ_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
NV_UPOINTZ_CTX_SWITCH_VOLATILE_RESET            EQU             000000001h
;  -W-4R 
;  3411 : #define NV_UPOINTZ_SET_NOTIFY                            0x00580104 /* -W-4R */
NV_UPOINTZ_SET_NOTIFY           EQU             000580104h
;  -W-VF 
;  3412 : #define NV_UPOINTZ_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
NV_UPOINTZ_SET_NOTIFY_PARAMETER         TEXTEQU         <31:0>
;  -W--V 
;  3413 : #define NV_UPOINTZ_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
NV_UPOINTZ_SET_NOTIFY_PARAMETER_WRITE           EQU             000000000h
;  -W-4R 
;  3414 : #define NV_UPOINTZ_CONTROL_OUT                           0x00580304 /* -W-4R */
NV_UPOINTZ_CONTROL_OUT          EQU             000580304h
;  -W-UF 
;  3415 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE                   19:16 /* -W-UF */
NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE             TEXTEQU         <19:16>
;  -W-UV 
;  3416 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_FALSE        0x00000001 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_FALSE               EQU             000000001h
;  -W-UV 
;  3417 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_LT           0x00000002 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_LT          EQU             000000002h
;  -W-UV 
;  3418 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_EQ           0x00000003 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_EQ          EQU             000000003h
;  -W-UV 
;  3419 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_LE           0x00000004 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_LE          EQU             000000004h
;  -W-UV 
;  3420 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_GT           0x00000005 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_GT          EQU             000000005h
;  -W-UV 
;  3421 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_NE           0x00000006 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_NE          EQU             000000006h
;  -W-UV 
;  3422 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_GE           0x00000007 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_GE          EQU             000000007h
;  -W-UV 
;  3423 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_TRUE         0x00000008 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_TRUE                EQU             000000008h
;  -W-UF 
;  3424 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE                     22:20 /* -W-UF */
NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE               TEXTEQU         <22:20>
;  -W-UV 
;  3425 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_NEVER          0x00000000 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_NEVER         EQU             000000000h
;  -W-UV 
;  3426 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_ALPHA          0x00000001 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_ALPHA         EQU             000000001h
;  -W-UV 
;  3427 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_ALPHA_ZETA     0x00000002 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_ALPHA_ZETA            EQU             000000002h
;  -W-UV 
;  3428 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_ZETA           0x00000003 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_ZETA          EQU             000000003h
;  -W-UV 
;  3429 : #define NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_ALWAYS         0x00000004 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_ALWAYS                EQU             000000004h
;  -W-UF 
;  3430 : #define NV_UPOINTZ_CONTROL_OUT_COLOR_WRITE                    25:24 /* -W-UF */
NV_UPOINTZ_CONTROL_OUT_COLOR_WRITE              TEXTEQU         <25:24>
;  -W-UV 
;  3431 : #define NV_UPOINTZ_CONTROL_OUT_COLOR_WRITE_NEVER         0x00000000 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_COLOR_WRITE_NEVER                EQU             000000000h
;  -W-UV 
;  3432 : #define NV_UPOINTZ_CONTROL_OUT_COLOR_WRITE_ALPHA         0x00000001 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_COLOR_WRITE_ALPHA                EQU             000000001h
;  -W-UV 
;  3433 : #define NV_UPOINTZ_CONTROL_OUT_COLOR_WRITE_ALPHA_ZETA    0x00000002 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_COLOR_WRITE_ALPHA_ZETA           EQU             000000002h
;  -W-UF 
;  3434 : #define NV_UPOINTZ_CONTROL_OUT_ROP                            28:28 /* -W-UF */
NV_UPOINTZ_CONTROL_OUT_ROP              TEXTEQU         <28:28>
;  -W-UV 
;  3435 : #define NV_UPOINTZ_CONTROL_OUT_ROP_BLEND_AND             0x00000000 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ROP_BLEND_AND            EQU             000000000h
;  -W-UV 
;  3436 : #define NV_UPOINTZ_CONTROL_OUT_ROP_ADD_WITH_SATURATION   0x00000001 /* -W-UV */
NV_UPOINTZ_CONTROL_OUT_ROP_ADD_WITH_SATURATION          EQU             000000001h
;  -W-4R 
;  3437 : #define NV_UPOINTZ_POINT                                 0x005807FC /* -W-4R */
NV_UPOINTZ_POINT                EQU             0005807fch
;  -W-SF 
;  3438 : #define NV_UPOINTZ_POINT_X                                     15:0 /* -W-SF */
NV_UPOINTZ_POINT_X              TEXTEQU         <15:0>
;  -W-SF 
;  3439 : #define NV_UPOINTZ_POINT_Y                                    31:16 /* -W-SF */
NV_UPOINTZ_POINT_Y              TEXTEQU         <31:16>
;  -W-4A 
;        
;  3440 : #define NV_UPOINTZ_COLOR(i)                      (0x00580800+(i)*8) /* -W-4A */
;  3441 : #define NV_UPOINTZ_COLOR__SIZE_1                                256 /*       */
NV_UPOINTZ_COLOR__SIZE_1                EQU             256t
;  -W-VF 
;  3442 : #define NV_UPOINTZ_COLOR_VALUE                                 31:0 /* -W-VF */
NV_UPOINTZ_COLOR_VALUE          TEXTEQU         <31:0>
;  -W-4A 
;        
;  3443 : #define NV_UPOINTZ_ZETA(i)                       (0x00580804+(i)*8) /* -W-4A */
;  3444 : #define NV_UPOINTZ_ZETA__SIZE_1                                 256 /*       */
NV_UPOINTZ_ZETA__SIZE_1         EQU             256t
;  -W-VF 
;  3445 : #define NV_UPOINTZ_ZETA_VALUE                                  31:0 /* -W-VF */
NV_UPOINTZ_ZETA_VALUE           TEXTEQU         <31:0>
;  usr_line.ref 
;  -W--D 
;  3446 : /* usr_line.ref */
;  3447 : #define NV_ULINE                              0x00491FFF:0x00490000 /* -W--D */
NV_ULINE                TEXTEQU         <0x00491FFF:0x00490000>
;  -W-4R 
;  3448 : #define NV_ULINE_CTX_SWITCH                              0x00490000 /* -W-4R */
NV_ULINE_CTX_SWITCH             EQU             000490000h
;  -W-UF 
;  3449 : #define NV_ULINE_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
NV_ULINE_CTX_SWITCH_INSTANCE            TEXTEQU         <15:0>
;  -W-UF 
;  3450 : #define NV_ULINE_CTX_SWITCH_CHID                              22:16 /* -W-UF */
NV_ULINE_CTX_SWITCH_CHID                TEXTEQU         <22:16>
;  -W-VF 
;  3451 : #define NV_ULINE_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
NV_ULINE_CTX_SWITCH_VOLATILE            TEXTEQU         <31:31>
;  -W--V 
;  3452 : #define NV_ULINE_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
NV_ULINE_CTX_SWITCH_VOLATILE_IGNORE             EQU             000000000h
;  -W--V 
;  3453 : #define NV_ULINE_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
NV_ULINE_CTX_SWITCH_VOLATILE_RESET              EQU             000000001h
;  -W-4R 
;  3454 : #define NV_ULINE_SET_NOTIFY                              0x00490104 /* -W-4R */
NV_ULINE_SET_NOTIFY             EQU             000490104h
;  -W-VF 
;  3455 : #define NV_ULINE_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
NV_ULINE_SET_NOTIFY_PARAMETER           TEXTEQU         <31:0>
;  -W--V 
;  3456 : #define NV_ULINE_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
NV_ULINE_SET_NOTIFY_PARAMETER_WRITE             EQU             000000000h
;  -W-4R 
;  3457 : #define NV_ULINE_COLOR                                   0x00490304 /* -W-4R */
NV_ULINE_COLOR          EQU             000490304h
;  -W-VF 
;  3458 : #define NV_ULINE_COLOR_VALUE                                   31:0 /* -W-VF */
NV_ULINE_COLOR_VALUE            TEXTEQU         <31:0>
;  -W-4A 
;        
;  3459 : #define NV_ULINE_LINE_0(i)                       (0x00490400+(i)*8) /* -W-4A */
;  3460 : #define NV_ULINE_LINE_0__SIZE_1                                  16 /*       */
NV_ULINE_LINE_0__SIZE_1         EQU             16t
;  -W-SF 
;  3461 : #define NV_ULINE_LINE_0_X                                      15:0 /* -W-SF */
NV_ULINE_LINE_0_X               TEXTEQU         <15:0>
;  -W-SF 
;  3462 : #define NV_ULINE_LINE_0_Y                                     31:16 /* -W-SF */
NV_ULINE_LINE_0_Y               TEXTEQU         <31:16>
;  -W-4A 
;        
;  3463 : #define NV_ULINE_LINE_1(i)                       (0x00490404+(i)*8) /* -W-4A */
;  3464 : #define NV_ULINE_LINE_1__SIZE_1                                  16 /*       */
NV_ULINE_LINE_1__SIZE_1         EQU             16t
;  -W-SF 
;  3465 : #define NV_ULINE_LINE_1_X                                      15:0 /* -W-SF */
NV_ULINE_LINE_1_X               TEXTEQU         <15:0>
;  -W-SF 
;  3466 : #define NV_ULINE_LINE_1_Y                                     31:16 /* -W-SF */
NV_ULINE_LINE_1_Y               TEXTEQU         <31:16>
;  -W-4A 
;        
;  3467 : #define NV_ULINE_LINE32_0(i)                    (0x00490480+(i)*16) /* -W-4A */
;  3468 : #define NV_ULINE_LINE32_0__SIZE_1                                 8 /*       */
NV_ULINE_LINE32_0__SIZE_1               EQU             8t
;  -W-SF 
;  3469 : #define NV_ULINE_LINE32_0_X                                    31:0 /* -W-SF */
NV_ULINE_LINE32_0_X             TEXTEQU         <31:0>
;  -W-4A 
;        
;  3470 : #define NV_ULINE_LINE32_1(i)                    (0x00490484+(i)*16) /* -W-4A */
;  3471 : #define NV_ULINE_LINE32_1__SIZE_1                                 8 /*       */
NV_ULINE_LINE32_1__SIZE_1               EQU             8t
;  -W-SF 
;  3472 : #define NV_ULINE_LINE32_1_Y                                    31:0 /* -W-SF */
NV_ULINE_LINE32_1_Y             TEXTEQU         <31:0>
;  -W-4A 
;        
;  3473 : #define NV_ULINE_LINE32_2(i)                    (0x00490488+(i)*16) /* -W-4A */
;  3474 : #define NV_ULINE_LINE32_2__SIZE_1                                 8 /*       */
NV_ULINE_LINE32_2__SIZE_1               EQU             8t
;  -W-SF 
;  3475 : #define NV_ULINE_LINE32_2_X                                    31:0 /* -W-SF */
NV_ULINE_LINE32_2_X             TEXTEQU         <31:0>
;  -W-4A 
;        
;  3476 : #define NV_ULINE_LINE32_3(i)                    (0x0049048C+(i)*16) /* -W-4A */
;  3477 : #define NV_ULINE_LINE32_3__SIZE_1                                 8 /*       */
NV_ULINE_LINE32_3__SIZE_1               EQU             8t
;  -W-SF 
;  3478 : #define NV_ULINE_LINE32_3_Y                                    31:0 /* -W-SF */
NV_ULINE_LINE32_3_Y             TEXTEQU         <31:0>
;  -W-4A 
;        
;  3479 : #define NV_ULINE_POLYLINE(i)                     (0x00490500+(i)*4) /* -W-4A */
;  3480 : #define NV_ULINE_POLYLINE__SIZE_1                                32 /*       */
NV_ULINE_POLYLINE__SIZE_1               EQU             32t
;  -W-SF 
;  3481 : #define NV_ULINE_POLYLINE_X                                    15:0 /* -W-SF */
NV_ULINE_POLYLINE_X             TEXTEQU         <15:0>
;  -W-SF 
;  3482 : #define NV_ULINE_POLYLINE_Y                                   31:16 /* -W-SF */
NV_ULINE_POLYLINE_Y             TEXTEQU         <31:16>
;  -W-4A 
;        
;  3483 : #define NV_ULINE_POLYLINE32_0(i)                 (0x00490580+(i)*8) /* -W-4A */
;  3484 : #define NV_ULINE_POLYLINE32_0__SIZE_1                            16 /*       */
NV_ULINE_POLYLINE32_0__SIZE_1           EQU             16t
;  -W-SF 
;  3485 : #define NV_ULINE_POLYLINE32_0_X                                31:0 /* -W-SF */
NV_ULINE_POLYLINE32_0_X         TEXTEQU         <31:0>
;  -W-4A 
;        
;  3486 : #define NV_ULINE_POLYLINE32_1(i)                 (0x00490584+(i)*8) /* -W-4A */
;  3487 : #define NV_ULINE_POLYLINE32_1__SIZE_1                            16 /*       */
NV_ULINE_POLYLINE32_1__SIZE_1           EQU             16t
;  -W-SF 
;  3488 : #define NV_ULINE_POLYLINE32_1_Y                                31:0 /* -W-SF */
NV_ULINE_POLYLINE32_1_Y         TEXTEQU         <31:0>
;  -W-4A 
;        
;  3489 : #define NV_ULINE_CPOLYLINE_0(i)                  (0x00490600+(i)*8) /* -W-4A */
;  3490 : #define NV_ULINE_CPOLYLINE_0__SIZE_1                             16 /*       */
NV_ULINE_CPOLYLINE_0__SIZE_1            EQU             16t
;  -W-VF 
;  3491 : #define NV_ULINE_CPOLYLINE_0_COLOR                             31:0 /* -W-VF */
NV_ULINE_CPOLYLINE_0_COLOR              TEXTEQU         <31:0>
;  -W-4A 
;        
;  3492 : #define NV_ULINE_CPOLYLINE_1(i)                  (0x00490604+(i)*8) /* -W-4A */
;  3493 : #define NV_ULINE_CPOLYLINE_1__SIZE_1                             16 /*       */
NV_ULINE_CPOLYLINE_1__SIZE_1            EQU             16t
;  -W-SF 
;  3494 : #define NV_ULINE_CPOLYLINE_1_X                                 15:0 /* -W-SF */
NV_ULINE_CPOLYLINE_1_X          TEXTEQU         <15:0>
;  -W-SF 
;  3495 : #define NV_ULINE_CPOLYLINE_1_Y                                31:16 /* -W-SF */
NV_ULINE_CPOLYLINE_1_Y          TEXTEQU         <31:16>
;  usr_lin.ref 
;  -W--D 
;  3496 : /* usr_lin.ref */
;  3497 : #define NV_ULIN                               0x004A1FFF:0x004A0000 /* -W--D */
NV_ULIN         TEXTEQU         <0x004A1FFF:0x004A0000>
;  -W-4R 
;  3498 : #define NV_ULIN_CTX_SWITCH                               0x004A0000 /* -W-4R */
NV_ULIN_CTX_SWITCH              EQU             0004a0000h
;  -W-UF 
;  3499 : #define NV_ULIN_CTX_SWITCH_INSTANCE                            15:0 /* -W-UF */
NV_ULIN_CTX_SWITCH_INSTANCE             TEXTEQU         <15:0>
;  -W-UF 
;  3500 : #define NV_ULIN_CTX_SWITCH_CHID                               22:16 /* -W-UF */
NV_ULIN_CTX_SWITCH_CHID         TEXTEQU         <22:16>
;  -W-VF 
;  3501 : #define NV_ULIN_CTX_SWITCH_VOLATILE                           31:31 /* -W-VF */
NV_ULIN_CTX_SWITCH_VOLATILE             TEXTEQU         <31:31>
;  -W--V 
;  3502 : #define NV_ULIN_CTX_SWITCH_VOLATILE_IGNORE               0x00000000 /* -W--V */
NV_ULIN_CTX_SWITCH_VOLATILE_IGNORE              EQU             000000000h
;  -W--V 
;  3503 : #define NV_ULIN_CTX_SWITCH_VOLATILE_RESET                0x00000001 /* -W--V */
NV_ULIN_CTX_SWITCH_VOLATILE_RESET               EQU             000000001h
;  -W-4R 
;  3504 : #define NV_ULIN_SET_NOTIFY                               0x004A0104 /* -W-4R */
NV_ULIN_SET_NOTIFY              EQU             0004a0104h
;  -W-VF 
;  3505 : #define NV_ULIN_SET_NOTIFY_PARAMETER                           31:0 /* -W-VF */
NV_ULIN_SET_NOTIFY_PARAMETER            TEXTEQU         <31:0>
;  -W--V 
;  3506 : #define NV_ULIN_SET_NOTIFY_PARAMETER_WRITE               0x00000000 /* -W--V */
NV_ULIN_SET_NOTIFY_PARAMETER_WRITE              EQU             000000000h
;  -W-4R 
;  3507 : #define NV_ULIN_COLOR                                    0x004A0304 /* -W-4R */
NV_ULIN_COLOR           EQU             0004a0304h
;  -W-VF 
;  3508 : #define NV_ULIN_COLOR_VALUE                                    31:0 /* -W-VF */
NV_ULIN_COLOR_VALUE             TEXTEQU         <31:0>
;  -W-4A 
;        
;  3509 : #define NV_ULIN_LIN_0(i)                         (0x004A0400+(i)*8) /* -W-4A */
;  3510 : #define NV_ULIN_LIN_0__SIZE_1                                    16 /*       */
NV_ULIN_LIN_0__SIZE_1           EQU             16t
;  -W-SF 
;  3511 : #define NV_ULIN_LIN_0_X                                        15:0 /* -W-SF */
NV_ULIN_LIN_0_X         TEXTEQU         <15:0>
;  -W-SF 
;  3512 : #define NV_ULIN_LIN_0_Y                                       31:16 /* -W-SF */
NV_ULIN_LIN_0_Y         TEXTEQU         <31:16>
;  -W-4A 
;        
;  3513 : #define NV_ULIN_LIN_1(i)                         (0x004A0404+(i)*8) /* -W-4A */
;  3514 : #define NV_ULIN_LIN_1__SIZE_1                                    16 /*       */
NV_ULIN_LIN_1__SIZE_1           EQU             16t
;  -W-SF 
;  3515 : #define NV_ULIN_LIN_1_X                                        15:0 /* -W-SF */
NV_ULIN_LIN_1_X         TEXTEQU         <15:0>
;  -W-SF 
;  3516 : #define NV_ULIN_LIN_1_Y                                       31:16 /* -W-SF */
NV_ULIN_LIN_1_Y         TEXTEQU         <31:16>
;  -W-4A 
;        
;  3517 : #define NV_ULIN_LIN32_0(i)                      (0x004A0480+(i)*16) /* -W-4A */
;  3518 : #define NV_ULIN_LIN32_0__SIZE_1                                   8 /*       */
NV_ULIN_LIN32_0__SIZE_1         EQU             8t
;  -W-SF 
;  3519 : #define NV_ULIN_LIN32_0_X                                      31:0 /* -W-SF */
NV_ULIN_LIN32_0_X               TEXTEQU         <31:0>
;  -W-4A 
;        
;  3520 : #define NV_ULIN_LIN32_1(i)                      (0x004A0484+(i)*16) /* -W-4A */
;  3521 : #define NV_ULIN_LIN32_1__SIZE_1                                   8 /*       */
NV_ULIN_LIN32_1__SIZE_1         EQU             8t
;  -W-SF 
;  3522 : #define NV_ULIN_LIN32_1_Y                                      31:0 /* -W-SF */
NV_ULIN_LIN32_1_Y               TEXTEQU         <31:0>
;  -W-4A 
;        
;  3523 : #define NV_ULIN_LIN32_2(i)                      (0x004A0488+(i)*16) /* -W-4A */
;  3524 : #define NV_ULIN_LIN32_2__SIZE_1                                   8 /*       */
NV_ULIN_LIN32_2__SIZE_1         EQU             8t
;  -W-SF 
;  3525 : #define NV_ULIN_LIN32_2_X                                      31:0 /* -W-SF */
NV_ULIN_LIN32_2_X               TEXTEQU         <31:0>
;  -W-4A 
;        
;  3526 : #define NV_ULIN_LIN32_3(i)                      (0x004A048C+(i)*16) /* -W-4A */
;  3527 : #define NV_ULIN_LIN32_3__SIZE_1                                   8 /*       */
NV_ULIN_LIN32_3__SIZE_1         EQU             8t
;  -W-SF 
;  3528 : #define NV_ULIN_LIN32_3_Y                                      31:0 /* -W-SF */
NV_ULIN_LIN32_3_Y               TEXTEQU         <31:0>
;  -W-4A 
;        
;  3529 : #define NV_ULIN_POLYLIN(i)                       (0x004A0500+(i)*4) /* -W-4A */
;  3530 : #define NV_ULIN_POLYLIN__SIZE_1                                  32 /*       */
NV_ULIN_POLYLIN__SIZE_1         EQU             32t
;  -W-SF 
;  3531 : #define NV_ULIN_POLYLIN_X                                      15:0 /* -W-SF */
NV_ULIN_POLYLIN_X               TEXTEQU         <15:0>
;  -W-SF 
;  3532 : #define NV_ULIN_POLYLIN_Y                                     31:16 /* -W-SF */
NV_ULIN_POLYLIN_Y               TEXTEQU         <31:16>
;  -W-4A 
;        
;  3533 : #define NV_ULIN_POLYLIN32_0(i)                   (0x004A0580+(i)*8) /* -W-4A */
;  3534 : #define NV_ULIN_POLYLIN32_0__SIZE_1                              16 /*       */
NV_ULIN_POLYLIN32_0__SIZE_1             EQU             16t
;  -W-SF 
;  3535 : #define NV_ULIN_POLYLIN32_0_X                                  31:0 /* -W-SF */
NV_ULIN_POLYLIN32_0_X           TEXTEQU         <31:0>
;  -W-4A 
;        
;  3536 : #define NV_ULIN_POLYLIN32_1(i)                   (0x004A0584+(i)*8) /* -W-4A */
;  3537 : #define NV_ULIN_POLYLIN32_1__SIZE_1                              16 /*       */
NV_ULIN_POLYLIN32_1__SIZE_1             EQU             16t
;  -W-SF 
;  3538 : #define NV_ULIN_POLYLIN32_1_Y                                  31:0 /* -W-SF */
NV_ULIN_POLYLIN32_1_Y           TEXTEQU         <31:0>
;  -W-4A 
;        
;  3539 : #define NV_ULIN_CPOLYLIN_0(i)                    (0x004A0600+(i)*8) /* -W-4A */
;  3540 : #define NV_ULIN_CPOLYLIN_0__SIZE_1                               16 /*       */
NV_ULIN_CPOLYLIN_0__SIZE_1              EQU             16t
;  -W-VF 
;  3541 : #define NV_ULIN_CPOLYLIN_0_COLOR                               31:0 /* -W-VF */
NV_ULIN_CPOLYLIN_0_COLOR                TEXTEQU         <31:0>
;  -W-4A 
;        
;  3542 : #define NV_ULIN_CPOLYLIN_1(i)                    (0x004A0604+(i)*8) /* -W-4A */
;  3543 : #define NV_ULIN_CPOLYLIN_1__SIZE_1                               16 /*       */
NV_ULIN_CPOLYLIN_1__SIZE_1              EQU             16t
;  -W-SF 
;  3544 : #define NV_ULIN_CPOLYLIN_1_X                                   15:0 /* -W-SF */
NV_ULIN_CPOLYLIN_1_X            TEXTEQU         <15:0>
;  -W-SF 
;  3545 : #define NV_ULIN_CPOLYLIN_1_Y                                  31:16 /* -W-SF */
NV_ULIN_CPOLYLIN_1_Y            TEXTEQU         <31:16>
;  usr_mem_to_mem.ref 
;  -W--D 
;  3546 : /* usr_mem_to_mem.ref */
;  3547 : #define NV_UMEMFMT                            0x004D1FFF:0x004D0000 /* -W--D */
NV_UMEMFMT              TEXTEQU         <0x004D1FFF:0x004D0000>
;  -W-4R 
;  3548 : #define NV_UMEMFMT_CTX_SWITCH                            0x004D0000 /* -W-4R */
NV_UMEMFMT_CTX_SWITCH           EQU             0004d0000h
;  -W-UF 
;  3549 : #define NV_UMEMFMT_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
NV_UMEMFMT_CTX_SWITCH_INSTANCE          TEXTEQU         <15:0>
;  -W-UF 
;  3550 : #define NV_UMEMFMT_CTX_SWITCH_CHID                            22:16 /* -W-UF */
NV_UMEMFMT_CTX_SWITCH_CHID              TEXTEQU         <22:16>
;  -W-VF 
;  3551 : #define NV_UMEMFMT_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
NV_UMEMFMT_CTX_SWITCH_VOLATILE          TEXTEQU         <31:31>
;  -W--V 
;  3552 : #define NV_UMEMFMT_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
NV_UMEMFMT_CTX_SWITCH_VOLATILE_IGNORE           EQU             000000000h
;  -W--V 
;  3553 : #define NV_UMEMFMT_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
NV_UMEMFMT_CTX_SWITCH_VOLATILE_RESET            EQU             000000001h
;  -W-4R 
;  3554 : #define NV_UMEMFMT_SET_NOTIFY                            0x004D0104 /* -W-4R */
NV_UMEMFMT_SET_NOTIFY           EQU             0004d0104h
;  -W-VF 
;  3555 : #define NV_UMEMFMT_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
NV_UMEMFMT_SET_NOTIFY_PARAMETER         TEXTEQU         <31:0>
;  -W--V 
;  3556 : #define NV_UMEMFMT_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
NV_UMEMFMT_SET_NOTIFY_PARAMETER_WRITE           EQU             000000000h
;  -W-4R 
;  3557 : #define NV_UMEMFMT_OFFSET_IN                             0x004D030C /* -W-4R */
NV_UMEMFMT_OFFSET_IN            EQU             0004d030ch
;  -W-UF 
;  3558 : #define NV_UMEMFMT_OFFSET_IN_VALUE                             31:0 /* -W-UF */
NV_UMEMFMT_OFFSET_IN_VALUE              TEXTEQU         <31:0>
;  -W-4R 
;  3559 : #define NV_UMEMFMT_OFFSET_OUT                            0x004D0310 /* -W-4R */
NV_UMEMFMT_OFFSET_OUT           EQU             0004d0310h
;  -W-UF 
;  3560 : #define NV_UMEMFMT_OFFSET_OUT_VALUE                            31:0 /* -W-UF */
NV_UMEMFMT_OFFSET_OUT_VALUE             TEXTEQU         <31:0>
;  -W-4R 
;  3561 : #define NV_UMEMFMT_PITCH_IN                              0x004D0314 /* -W-4R */
NV_UMEMFMT_PITCH_IN             EQU             0004d0314h
;  -W-SF 
;  3562 : #define NV_UMEMFMT_PITCH_IN_VALUE                              31:0 /* -W-SF */
NV_UMEMFMT_PITCH_IN_VALUE               TEXTEQU         <31:0>
;  -W-4R 
;  3563 : #define NV_UMEMFMT_PITCH_OUT                             0x004D0318 /* -W-4R */
NV_UMEMFMT_PITCH_OUT            EQU             0004d0318h
;  -W-SF 
;  3564 : #define NV_UMEMFMT_PITCH_OUT_VALUE                             31:0 /* -W-SF */
NV_UMEMFMT_PITCH_OUT_VALUE              TEXTEQU         <31:0>
;  -W-4R 
;  3565 : #define NV_UMEMFMT_LINE_LENGTH_IN                        0x004D031C /* -W-4R */
NV_UMEMFMT_LINE_LENGTH_IN               EQU             0004d031ch
;  -W-UF 
;  3566 : #define NV_UMEMFMT_LINE_LENGTH_IN_VALUE                        31:0 /* -W-UF */
NV_UMEMFMT_LINE_LENGTH_IN_VALUE         TEXTEQU         <31:0>
;  -W-4R 
;  3567 : #define NV_UMEMFMT_LINE_COUNT                            0x004D0320 /* -W-4R */
NV_UMEMFMT_LINE_COUNT           EQU             0004d0320h
;  -W-UF 
;  3568 : #define NV_UMEMFMT_LINE_COUNT_VALUE                            31:0 /* -W-UF */
NV_UMEMFMT_LINE_COUNT_VALUE             TEXTEQU         <31:0>
;  -W-4R 
;  3569 : #define NV_UMEMFMT_FORMAT                                0x004D0324 /* -W-4R */
NV_UMEMFMT_FORMAT               EQU             0004d0324h
;  -W-UF 
;  3570 : #define NV_UMEMFMT_FORMAT_INPUT_INC                             2:0 /* -W-UF */
NV_UMEMFMT_FORMAT_INPUT_INC             TEXTEQU         <2:0>
;  -WIUV 
;  3571 : #define NV_UMEMFMT_FORMAT_INPUT_INC_1                    0x00000001 /* -WIUV */
NV_UMEMFMT_FORMAT_INPUT_INC_1           EQU             000000001h
;  -W-UV 
;  3572 : #define NV_UMEMFMT_FORMAT_INPUT_INC_2                    0x00000002 /* -W-UV */
NV_UMEMFMT_FORMAT_INPUT_INC_2           EQU             000000002h
;  -W-UV 
;  3573 : #define NV_UMEMFMT_FORMAT_INPUT_INC_4                    0x00000004 /* -W-UV */
NV_UMEMFMT_FORMAT_INPUT_INC_4           EQU             000000004h
;  -W-UF 
;  3574 : #define NV_UMEMFMT_FORMAT_OUTPUT_INC                           10:8 /* -W-UF */
NV_UMEMFMT_FORMAT_OUTPUT_INC            TEXTEQU         <10:8>
;  -WIUV 
;  3575 : #define NV_UMEMFMT_FORMAT_OUTPUT_INC_1                   0x00000001 /* -WIUV */
NV_UMEMFMT_FORMAT_OUTPUT_INC_1          EQU             000000001h
;  -W-UV 
;  3576 : #define NV_UMEMFMT_FORMAT_OUTPUT_INC_2                   0x00000002 /* -W-UV */
NV_UMEMFMT_FORMAT_OUTPUT_INC_2          EQU             000000002h
;  -W-UV 
;  3577 : #define NV_UMEMFMT_FORMAT_OUTPUT_INC_4                   0x00000004 /* -W-UV */
NV_UMEMFMT_FORMAT_OUTPUT_INC_4          EQU             000000004h
;  -W-4R 
;  3578 : #define NV_UMEMFMT_BUF_NOTIFY                            0x004D0328 /* -W-4R */
NV_UMEMFMT_BUF_NOTIFY           EQU             0004d0328h
;  -W-UF 
;  3579 : #define NV_UMEMFMT_BUF_NOTIFY_VALUE                            31:0 /* -W-UF */
NV_UMEMFMT_BUF_NOTIFY_VALUE             TEXTEQU         <31:0>
;  usr_triangle.ref 
;  -W--D 
;  3580 : /* usr_triangle.ref */
;  3581 : #define NV_UTRI                               0x004B1FFF:0x004B0000 /* -W--D */
NV_UTRI         TEXTEQU         <0x004B1FFF:0x004B0000>
;  -W-4R 
;  3582 : #define NV_UTRI_CTX_SWITCH                               0x004B0000 /* -W-4R */
NV_UTRI_CTX_SWITCH              EQU             0004b0000h
;  -W-UF 
;  3583 : #define NV_UTRI_CTX_SWITCH_INSTANCE                            15:0 /* -W-UF */
NV_UTRI_CTX_SWITCH_INSTANCE             TEXTEQU         <15:0>
;  -W-UF 
;  3584 : #define NV_UTRI_CTX_SWITCH_CHID                               22:16 /* -W-UF */
NV_UTRI_CTX_SWITCH_CHID         TEXTEQU         <22:16>
;  -W-VF 
;  3585 : #define NV_UTRI_CTX_SWITCH_VOLATILE                           31:31 /* -W-VF */
NV_UTRI_CTX_SWITCH_VOLATILE             TEXTEQU         <31:31>
;  -W--V 
;  3586 : #define NV_UTRI_CTX_SWITCH_VOLATILE_IGNORE               0x00000000 /* -W--V */
NV_UTRI_CTX_SWITCH_VOLATILE_IGNORE              EQU             000000000h
;  -W--V 
;  3587 : #define NV_UTRI_CTX_SWITCH_VOLATILE_RESET                0x00000001 /* -W--V */
NV_UTRI_CTX_SWITCH_VOLATILE_RESET               EQU             000000001h
;  -W-4R 
;  3588 : #define NV_UTRI_SET_NOTIFY                               0x004B0104 /* -W-4R */
NV_UTRI_SET_NOTIFY              EQU             0004b0104h
;  -W-VF 
;  3589 : #define NV_UTRI_SET_NOTIFY_PARAMETER                           31:0 /* -W-VF */
NV_UTRI_SET_NOTIFY_PARAMETER            TEXTEQU         <31:0>
;  -W--V 
;  3590 : #define NV_UTRI_SET_NOTIFY_PARAMETER_WRITE               0x00000000 /* -W--V */
NV_UTRI_SET_NOTIFY_PARAMETER_WRITE              EQU             000000000h
;  -W-4R 
;  3591 : #define NV_UTRI_COLOR                                    0x004B0304 /* -W-4R */
NV_UTRI_COLOR           EQU             0004b0304h
;  -W-VF 
;  3592 : #define NV_UTRI_COLOR_VALUE                                    31:0 /* -W-VF */
NV_UTRI_COLOR_VALUE             TEXTEQU         <31:0>
;  -W-4R 
;  3593 : #define NV_UTRI_TRIANGLE_0                               0x004B0310 /* -W-4R */
NV_UTRI_TRIANGLE_0              EQU             0004b0310h
;  -W-SF 
;  3594 : #define NV_UTRI_TRIANGLE_0_X                                   15:0 /* -W-SF */
NV_UTRI_TRIANGLE_0_X            TEXTEQU         <15:0>
;  -W-SF 
;  3595 : #define NV_UTRI_TRIANGLE_0_Y                                  31:16 /* -W-SF */
NV_UTRI_TRIANGLE_0_Y            TEXTEQU         <31:16>
;  -W-4R 
;  3596 : #define NV_UTRI_TRIANGLE_1                               0x004B0314 /* -W-4R */
NV_UTRI_TRIANGLE_1              EQU             0004b0314h
;  -W-SF 
;  3597 : #define NV_UTRI_TRIANGLE_1_X                                   15:0 /* -W-SF */
NV_UTRI_TRIANGLE_1_X            TEXTEQU         <15:0>
;  -W-SF 
;  3598 : #define NV_UTRI_TRIANGLE_1_Y                                  31:16 /* -W-SF */
NV_UTRI_TRIANGLE_1_Y            TEXTEQU         <31:16>
;  -W-4R 
;  3599 : #define NV_UTRI_TRIANGLE_2                               0x004B0318 /* -W-4R */
NV_UTRI_TRIANGLE_2              EQU             0004b0318h
;  -W-SF 
;  3600 : #define NV_UTRI_TRIANGLE_2_X                                   15:0 /* -W-SF */
NV_UTRI_TRIANGLE_2_X            TEXTEQU         <15:0>
;  -W-SF 
;  3601 : #define NV_UTRI_TRIANGLE_2_Y                                  31:16 /* -W-SF */
NV_UTRI_TRIANGLE_2_Y            TEXTEQU         <31:16>
;  -W-4R 
;  3602 : #define NV_UTRI_TRIANGLE32_0                             0x004B0320 /* -W-4R */
NV_UTRI_TRIANGLE32_0            EQU             0004b0320h
;  -W-SF 
;  3603 : #define NV_UTRI_TRIANGLE32_0_X                                 31:0 /* -W-SF */
NV_UTRI_TRIANGLE32_0_X          TEXTEQU         <31:0>
;  -W-4R 
;  3604 : #define NV_UTRI_TRIANGLE32_1                             0x004B0324 /* -W-4R */
NV_UTRI_TRIANGLE32_1            EQU             0004b0324h
;  -W-SF 
;  3605 : #define NV_UTRI_TRIANGLE32_1_Y                                 31:0 /* -W-SF */
NV_UTRI_TRIANGLE32_1_Y          TEXTEQU         <31:0>
;  -W-4R 
;  3606 : #define NV_UTRI_TRIANGLE32_2                             0x004B0328 /* -W-4R */
NV_UTRI_TRIANGLE32_2            EQU             0004b0328h
;  -W-SF 
;  3607 : #define NV_UTRI_TRIANGLE32_2_X                                 31:0 /* -W-SF */
NV_UTRI_TRIANGLE32_2_X          TEXTEQU         <31:0>
;  -W-4R 
;  3608 : #define NV_UTRI_TRIANGLE32_3                             0x004B032C /* -W-4R */
NV_UTRI_TRIANGLE32_3            EQU             0004b032ch
;  -W-SF 
;  3609 : #define NV_UTRI_TRIANGLE32_3_Y                                 31:0 /* -W-SF */
NV_UTRI_TRIANGLE32_3_Y          TEXTEQU         <31:0>
;  -W-4R 
;  3610 : #define NV_UTRI_TRIANGLE32_4                             0x004B0330 /* -W-4R */
NV_UTRI_TRIANGLE32_4            EQU             0004b0330h
;  -W-SF 
;  3611 : #define NV_UTRI_TRIANGLE32_4_X                                 31:0 /* -W-SF */
NV_UTRI_TRIANGLE32_4_X          TEXTEQU         <31:0>
;  -W-4R 
;  3612 : #define NV_UTRI_TRIANGLE32_5                             0x004B0334 /* -W-4R */
NV_UTRI_TRIANGLE32_5            EQU             0004b0334h
;  -W-SF 
;  3613 : #define NV_UTRI_TRIANGLE32_5_Y                                 31:0 /* -W-SF */
NV_UTRI_TRIANGLE32_5_Y          TEXTEQU         <31:0>
;  -W-4A 
;        
;  3614 : #define NV_UTRI_TRIMESH(i)                       (0x004B0400+(i)*4) /* -W-4A */
;  3615 : #define NV_UTRI_TRIMESH__SIZE_1                                  32 /*       */
NV_UTRI_TRIMESH__SIZE_1         EQU             32t
;  -W-SF 
;  3616 : #define NV_UTRI_TRIMESH_X                                      15:0 /* -W-SF */
NV_UTRI_TRIMESH_X               TEXTEQU         <15:0>
;  -W-SF 
;  3617 : #define NV_UTRI_TRIMESH_Y                                     31:16 /* -W-SF */
NV_UTRI_TRIMESH_Y               TEXTEQU         <31:16>
;  -W-4A 
;        
;  3618 : #define NV_UTRI_TRIMESH32_0(i)                   (0x004B0480+(i)*8) /* -W-4A */
;  3619 : #define NV_UTRI_TRIMESH32_0__SIZE_1                              16 /*       */
NV_UTRI_TRIMESH32_0__SIZE_1             EQU             16t
;  -W-SF 
;  3620 : #define NV_UTRI_TRIMESH32_0_X                                  31:0 /* -W-SF */
NV_UTRI_TRIMESH32_0_X           TEXTEQU         <31:0>
;  -W-4A 
;        
;  3621 : #define NV_UTRI_TRIMESH32_1(i)                   (0x004B0484+(i)*8) /* -W-4A */
;  3622 : #define NV_UTRI_TRIMESH32_1__SIZE_1                              16 /*       */
NV_UTRI_TRIMESH32_1__SIZE_1             EQU             16t
;  -W-SF 
;  3623 : #define NV_UTRI_TRIMESH32_1_Y                                  31:0 /* -W-SF */
NV_UTRI_TRIMESH32_1_Y           TEXTEQU         <31:0>
;  -W-4A 
;        
;  3624 : #define NV_UTRI_CTRIANGLE_0(i)                  (0x004B0500+(i)*16) /* -W-4A */
;  3625 : #define NV_UTRI_CTRIANGLE_0__SIZE_1                               8 /*       */
NV_UTRI_CTRIANGLE_0__SIZE_1             EQU             8t
;  -W-VF 
;  3626 : #define NV_UTRI_CTRIANGLE_0_COLOR                              31:0 /* -W-VF */
NV_UTRI_CTRIANGLE_0_COLOR               TEXTEQU         <31:0>
;  -W-4A 
;        
;  3627 : #define NV_UTRI_CTRIANGLE_1(i)                  (0x004B0504+(i)*16) /* -W-4A */
;  3628 : #define NV_UTRI_CTRIANGLE_1__SIZE_1                               8 /*       */
NV_UTRI_CTRIANGLE_1__SIZE_1             EQU             8t
;  -W-SF 
;  3629 : #define NV_UTRI_CTRIANGLE_1_X                                  15:0 /* -W-SF */
NV_UTRI_CTRIANGLE_1_X           TEXTEQU         <15:0>
;  -W-SF 
;  3630 : #define NV_UTRI_CTRIANGLE_1_Y                                 31:16 /* -W-SF */
NV_UTRI_CTRIANGLE_1_Y           TEXTEQU         <31:16>
;  -W-4A 
;        
;  3631 : #define NV_UTRI_CTRIANGLE_2(i)                  (0x004B0508+(i)*16) /* -W-4A */
;  3632 : #define NV_UTRI_CTRIANGLE_2__SIZE_1                               8 /*       */
NV_UTRI_CTRIANGLE_2__SIZE_1             EQU             8t
;  -W-SF 
;  3633 : #define NV_UTRI_CTRIANGLE_2_X                                  15:0 /* -W-SF */
NV_UTRI_CTRIANGLE_2_X           TEXTEQU         <15:0>
;  -W-SF 
;  3634 : #define NV_UTRI_CTRIANGLE_2_Y                                 31:16 /* -W-SF */
NV_UTRI_CTRIANGLE_2_Y           TEXTEQU         <31:16>
;  -W-4A 
;        
;  3635 : #define NV_UTRI_CTRIANGLE_3(i)                  (0x004B050C+(i)*16) /* -W-4A */
;  3636 : #define NV_UTRI_CTRIANGLE_3__SIZE_1                               8 /*       */
NV_UTRI_CTRIANGLE_3__SIZE_1             EQU             8t
;  -W-SF 
;  3637 : #define NV_UTRI_CTRIANGLE_3_X                                  15:0 /* -W-SF */
NV_UTRI_CTRIANGLE_3_X           TEXTEQU         <15:0>
;  -W-SF 
;  3638 : #define NV_UTRI_CTRIANGLE_3_Y                                 31:16 /* -W-SF */
NV_UTRI_CTRIANGLE_3_Y           TEXTEQU         <31:16>
;  -W-4A 
;        
;  3639 : #define NV_UTRI_CTRIMESH_0(i)                    (0x004B0580+(i)*8) /* -W-4A */
;  3640 : #define NV_UTRI_CTRIMESH_0__SIZE_1                               16 /*       */
NV_UTRI_CTRIMESH_0__SIZE_1              EQU             16t
;  -W-VF 
;  3641 : #define NV_UTRI_CTRIMESH_0_COLOR                               31:0 /* -W-VF */
NV_UTRI_CTRIMESH_0_COLOR                TEXTEQU         <31:0>
;  -W-4A 
;        
;  3642 : #define NV_UTRI_CTRIMESH_1(i)                    (0x004B0584+(i)*8) /* -W-4A */
;  3643 : #define NV_UTRI_CTRIMESH_1__SIZE_1                               16 /*       */
NV_UTRI_CTRIMESH_1__SIZE_1              EQU             16t
;  -W-SF 
;  3644 : #define NV_UTRI_CTRIMESH_1_X                                   15:0 /* -W-SF */
NV_UTRI_CTRIMESH_1_X            TEXTEQU         <15:0>
;  -W-SF 
;  3645 : #define NV_UTRI_CTRIMESH_1_Y                                  31:16 /* -W-SF */
NV_UTRI_CTRIMESH_1_Y            TEXTEQU         <31:16>
;  usr_rectangle.ref 
;  -W--D 
;  3646 : /* usr_rectangle.ref */
;  3647 : #define NV_URECT                              0x00471FFF:0x00470000 /* -W--D */
NV_URECT                TEXTEQU         <0x00471FFF:0x00470000>
;  -W-4R 
;  3648 : #define NV_URECT_CTX_SWITCH                              0x00470000 /* -W-4R */
NV_URECT_CTX_SWITCH             EQU             000470000h
;  -W-UF 
;  3649 : #define NV_URECT_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
NV_URECT_CTX_SWITCH_INSTANCE            TEXTEQU         <15:0>
;  -W-UF 
;  3650 : #define NV_URECT_CTX_SWITCH_CHID                              22:16 /* -W-UF */
NV_URECT_CTX_SWITCH_CHID                TEXTEQU         <22:16>
;  -W-VF 
;  3651 : #define NV_URECT_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
NV_URECT_CTX_SWITCH_VOLATILE            TEXTEQU         <31:31>
;  -W--V 
;  3652 : #define NV_URECT_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
NV_URECT_CTX_SWITCH_VOLATILE_IGNORE             EQU             000000000h
;  -W--V 
;  3653 : #define NV_URECT_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
NV_URECT_CTX_SWITCH_VOLATILE_RESET              EQU             000000001h
;  -W-4R 
;  3654 : #define NV_URECT_SET_NOTIFY                              0x00470104 /* -W-4R */
NV_URECT_SET_NOTIFY             EQU             000470104h
;  -W-VF 
;  3655 : #define NV_URECT_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
NV_URECT_SET_NOTIFY_PARAMETER           TEXTEQU         <31:0>
;  -W--V 
;  3656 : #define NV_URECT_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
NV_URECT_SET_NOTIFY_PARAMETER_WRITE             EQU             000000000h
;  -W-4R 
;  3657 : #define NV_URECT_COLOR                                   0x00470304 /* -W-4R */
NV_URECT_COLOR          EQU             000470304h
;  -W-VF 
;  3658 : #define NV_URECT_COLOR_VALUE                                   31:0 /* -W-VF */
NV_URECT_COLOR_VALUE            TEXTEQU         <31:0>
;  -W-4A 
;        
;  3659 : #define NV_URECT_RECTANGLE_0(i)                  (0x00470400+(i)*8) /* -W-4A */
;  3660 : #define NV_URECT_RECTANGLE_0__SIZE_1                             16 /*       */
NV_URECT_RECTANGLE_0__SIZE_1            EQU             16t
;  -W-SF 
;  3661 : #define NV_URECT_RECTANGLE_0_X                                 15:0 /* -W-SF */
NV_URECT_RECTANGLE_0_X          TEXTEQU         <15:0>
;  -W-SF 
;  3662 : #define NV_URECT_RECTANGLE_0_Y                                31:16 /* -W-SF */
NV_URECT_RECTANGLE_0_Y          TEXTEQU         <31:16>
;  -W-4A 
;        
;  3663 : #define NV_URECT_RECTANGLE_1(i)                  (0x00470404+(i)*8) /* -W-4A */
;  3664 : #define NV_URECT_RECTANGLE_1__SIZE_1                             16 /*       */
NV_URECT_RECTANGLE_1__SIZE_1            EQU             16t
;  -W-UF 
;  3665 : #define NV_URECT_RECTANGLE_1_WIDTH                             15:0 /* -W-UF */
NV_URECT_RECTANGLE_1_WIDTH              TEXTEQU         <15:0>
;  -W-UF 
;  3666 : #define NV_URECT_RECTANGLE_1_HEIGHT                           31:16 /* -W-UF */
NV_URECT_RECTANGLE_1_HEIGHT             TEXTEQU         <31:16>
;  usr_image_blit.ref 
;  -W--D 
;  3667 : /* usr_image_blit.ref */
;  3668 : #define NV_UBLIT                              0x00501FFF:0x00500000 /* -W--D */
NV_UBLIT                TEXTEQU         <0x00501FFF:0x00500000>
;  -W-4R 
;  3669 : #define NV_UBLIT_CTX_SWITCH                              0x00500000 /* -W-4R */
NV_UBLIT_CTX_SWITCH             EQU             000500000h
;  -W-UF 
;  3670 : #define NV_UBLIT_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
NV_UBLIT_CTX_SWITCH_INSTANCE            TEXTEQU         <15:0>
;  -W-UF 
;  3671 : #define NV_UBLIT_CTX_SWITCH_CHID                              22:16 /* -W-UF */
NV_UBLIT_CTX_SWITCH_CHID                TEXTEQU         <22:16>
;  -W-VF 
;  3672 : #define NV_UBLIT_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
NV_UBLIT_CTX_SWITCH_VOLATILE            TEXTEQU         <31:31>
;  -W--V 
;  3673 : #define NV_UBLIT_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
NV_UBLIT_CTX_SWITCH_VOLATILE_IGNORE             EQU             000000000h
;  -W--V 
;  3674 : #define NV_UBLIT_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
NV_UBLIT_CTX_SWITCH_VOLATILE_RESET              EQU             000000001h
;  -W-4R 
;  3675 : #define NV_UBLIT_SET_NOTIFY                              0x00500104 /* -W-4R */
NV_UBLIT_SET_NOTIFY             EQU             000500104h
;  -W-VF 
;  3676 : #define NV_UBLIT_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
NV_UBLIT_SET_NOTIFY_PARAMETER           TEXTEQU         <31:0>
;  -W--V 
;  3677 : #define NV_UBLIT_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
NV_UBLIT_SET_NOTIFY_PARAMETER_WRITE             EQU             000000000h
;  -W-4R 
;  3678 : #define NV_UBLIT_POINT_IN                                0x00500300 /* -W-4R */
NV_UBLIT_POINT_IN               EQU             000500300h
;  -W-SF 
;  3679 : #define NV_UBLIT_POINT_IN_X                                    15:0 /* -W-SF */
NV_UBLIT_POINT_IN_X             TEXTEQU         <15:0>
;  -W-SF 
;  3680 : #define NV_UBLIT_POINT_IN_Y                                   31:16 /* -W-SF */
NV_UBLIT_POINT_IN_Y             TEXTEQU         <31:16>
;  -W-4R 
;  3681 : #define NV_UBLIT_POINT_OUT                               0x00500304 /* -W-4R */
NV_UBLIT_POINT_OUT              EQU             000500304h
;  -W-SF 
;  3682 : #define NV_UBLIT_POINT_OUT_X                                   15:0 /* -W-SF */
NV_UBLIT_POINT_OUT_X            TEXTEQU         <15:0>
;  -W-SF 
;  3683 : #define NV_UBLIT_POINT_OUT_Y                                  31:16 /* -W-SF */
NV_UBLIT_POINT_OUT_Y            TEXTEQU         <31:16>
;  -W-4R 
;  3684 : #define NV_UBLIT_SIZE                                    0x00500308 /* -W-4R */
NV_UBLIT_SIZE           EQU             000500308h
;  -W-UF 
;  3685 : #define NV_UBLIT_SIZE_WIDTH                                    15:0 /* -W-UF */
NV_UBLIT_SIZE_WIDTH             TEXTEQU         <15:0>
;  -W-UF 
;  3686 : #define NV_UBLIT_SIZE_HEIGHT                                  31:16 /* -W-UF */
NV_UBLIT_SIZE_HEIGHT            TEXTEQU         <31:16>
;  usr_image_from_cpu.ref 
;  -W--D 
;  3687 : /* usr_image_from_cpu.ref */
;  3688 : #define NV_UIMAGE                             0x00511FFF:0x00510000 /* -W--D */
NV_UIMAGE               TEXTEQU         <0x00511FFF:0x00510000>
;  -W-4R 
;  3689 : #define NV_UIMAGE_CTX_SWITCH                             0x00510000 /* -W-4R */
NV_UIMAGE_CTX_SWITCH            EQU             000510000h
;  -W-UF 
;  3690 : #define NV_UIMAGE_CTX_SWITCH_INSTANCE                          15:0 /* -W-UF */
NV_UIMAGE_CTX_SWITCH_INSTANCE           TEXTEQU         <15:0>
;  -W-UF 
;  3691 : #define NV_UIMAGE_CTX_SWITCH_CHID                             22:16 /* -W-UF */
NV_UIMAGE_CTX_SWITCH_CHID               TEXTEQU         <22:16>
;  -W-VF 
;  3692 : #define NV_UIMAGE_CTX_SWITCH_VOLATILE                         31:31 /* -W-VF */
NV_UIMAGE_CTX_SWITCH_VOLATILE           TEXTEQU         <31:31>
;  -W--V 
;  3693 : #define NV_UIMAGE_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* -W--V */
NV_UIMAGE_CTX_SWITCH_VOLATILE_IGNORE            EQU             000000000h
;  -W--V 
;  3694 : #define NV_UIMAGE_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
NV_UIMAGE_CTX_SWITCH_VOLATILE_RESET             EQU             000000001h
;  -W-4R 
;  3695 : #define NV_UIMAGE_SET_NOTIFY                             0x00510104 /* -W-4R */
NV_UIMAGE_SET_NOTIFY            EQU             000510104h
;  -W-VF 
;  3696 : #define NV_UIMAGE_SET_NOTIFY_PARAMETER                         31:0 /* -W-VF */
NV_UIMAGE_SET_NOTIFY_PARAMETER          TEXTEQU         <31:0>
;  -W--V 
;  3697 : #define NV_UIMAGE_SET_NOTIFY_PARAMETER_WRITE             0x00000000 /* -W--V */
NV_UIMAGE_SET_NOTIFY_PARAMETER_WRITE            EQU             000000000h
;  -W-4R 
;  3698 : #define NV_UIMAGE_POINT                                  0x00510304 /* -W-4R */
NV_UIMAGE_POINT         EQU             000510304h
;  -W-SF 
;  3699 : #define NV_UIMAGE_POINT_X                                      15:0 /* -W-SF */
NV_UIMAGE_POINT_X               TEXTEQU         <15:0>
;  -W-SF 
;  3700 : #define NV_UIMAGE_POINT_Y                                     31:16 /* -W-SF */
NV_UIMAGE_POINT_Y               TEXTEQU         <31:16>
;  -W-4R 
;  3701 : #define NV_UIMAGE_SIZE                                   0x00510308 /* -W-4R */
NV_UIMAGE_SIZE          EQU             000510308h
;  -W-UF 
;  3702 : #define NV_UIMAGE_SIZE_WIDTH                                   15:0 /* -W-UF */
NV_UIMAGE_SIZE_WIDTH            TEXTEQU         <15:0>
;  -W-UF 
;  3703 : #define NV_UIMAGE_SIZE_HEIGHT                                 31:16 /* -W-UF */
NV_UIMAGE_SIZE_HEIGHT           TEXTEQU         <31:16>
;  -W-4R 
;  3704 : #define NV_UIMAGE_SIZE_IN                                0x0051030C /* -W-4R */
NV_UIMAGE_SIZE_IN               EQU             00051030ch
;  -W-UF 
;  3705 : #define NV_UIMAGE_SIZE_IN_WIDTH                                15:0 /* -W-UF */
NV_UIMAGE_SIZE_IN_WIDTH         TEXTEQU         <15:0>
;  -W-UF 
;  3706 : #define NV_UIMAGE_SIZE_IN_HEIGHT                              31:16 /* -W-UF */
NV_UIMAGE_SIZE_IN_HEIGHT                TEXTEQU         <31:16>
;  -W-4A 
;        
;  3707 : #define NV_UIMAGE_COLOR(i)                       (0x00510400+(i)*4) /* -W-4A */
;  3708 : #define NV_UIMAGE_COLOR__SIZE_1                                  32 /*       */
NV_UIMAGE_COLOR__SIZE_1         EQU             32t
;  -W-VF 
;  3709 : #define NV_UIMAGE_COLOR_VALUE                                  31:0 /* -W-VF */
NV_UIMAGE_COLOR_VALUE           TEXTEQU         <31:0>
;  usr_bitmap_from_cpu.ref 
;  -W--D 
;  3710 : /* usr_bitmap_from_cpu.ref */
;  3711 : #define NV_UBITMAP                            0x00521FFF:0x00520000 /* -W--D */
NV_UBITMAP              TEXTEQU         <0x00521FFF:0x00520000>
;  -W-4R 
;  3712 : #define NV_UBITMAP_CTX_SWITCH                            0x00520000 /* -W-4R */
NV_UBITMAP_CTX_SWITCH           EQU             000520000h
;  -W-UF 
;  3713 : #define NV_UBITMAP_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
NV_UBITMAP_CTX_SWITCH_INSTANCE          TEXTEQU         <15:0>
;  -W-UF 
;  3714 : #define NV_UBITMAP_CTX_SWITCH_CHID                            22:16 /* -W-UF */
NV_UBITMAP_CTX_SWITCH_CHID              TEXTEQU         <22:16>
;  -W-VF 
;  3715 : #define NV_UBITMAP_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
NV_UBITMAP_CTX_SWITCH_VOLATILE          TEXTEQU         <31:31>
;  -W--V 
;  3716 : #define NV_UBITMAP_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
NV_UBITMAP_CTX_SWITCH_VOLATILE_IGNORE           EQU             000000000h
;  -W--V 
;  3717 : #define NV_UBITMAP_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
NV_UBITMAP_CTX_SWITCH_VOLATILE_RESET            EQU             000000001h
;  -W-4R 
;  3718 : #define NV_UBITMAP_SET_NOTIFY                            0x00520104 /* -W-4R */
NV_UBITMAP_SET_NOTIFY           EQU             000520104h
;  -W-VF 
;  3719 : #define NV_UBITMAP_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
NV_UBITMAP_SET_NOTIFY_PARAMETER         TEXTEQU         <31:0>
;  -W--V 
;  3720 : #define NV_UBITMAP_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
NV_UBITMAP_SET_NOTIFY_PARAMETER_WRITE           EQU             000000000h
;  -W-4R 
;  3721 : #define NV_UBITMAP_COLOR0                                0x00520308 /* -W-4R */
NV_UBITMAP_COLOR0               EQU             000520308h
;  -W-VF 
;  3722 : #define NV_UBITMAP_COLOR0_VALUE                                31:0 /* -W-VF */
NV_UBITMAP_COLOR0_VALUE         TEXTEQU         <31:0>
;  -W-4R 
;  3723 : #define NV_UBITMAP_COLOR1                                0x0052030C /* -W-4R */
NV_UBITMAP_COLOR1               EQU             00052030ch
;  -W-VF 
;  3724 : #define NV_UBITMAP_COLOR1_VALUE                                31:0 /* -W-VF */
NV_UBITMAP_COLOR1_VALUE         TEXTEQU         <31:0>
;  -W-4R 
;  3725 : #define NV_UBITMAP_POINT                                 0x00520310 /* -W-4R */
NV_UBITMAP_POINT                EQU             000520310h
;  -W-SF 
;  3726 : #define NV_UBITMAP_POINT_X                                     15:0 /* -W-SF */
NV_UBITMAP_POINT_X              TEXTEQU         <15:0>
;  -W-SF 
;  3727 : #define NV_UBITMAP_POINT_Y                                    31:16 /* -W-SF */
NV_UBITMAP_POINT_Y              TEXTEQU         <31:16>
;  -W-4R 
;  3728 : #define NV_UBITMAP_SIZE                                  0x00520314 /* -W-4R */
NV_UBITMAP_SIZE         EQU             000520314h
;  -W-UF 
;  3729 : #define NV_UBITMAP_SIZE_WIDTH                                  15:0 /* -W-UF */
NV_UBITMAP_SIZE_WIDTH           TEXTEQU         <15:0>
;  -W-UF 
;  3730 : #define NV_UBITMAP_SIZE_HEIGHT                                31:16 /* -W-UF */
NV_UBITMAP_SIZE_HEIGHT          TEXTEQU         <31:16>
;  -W-4R 
;  3731 : #define NV_UBITMAP_SIZE_IN                               0x00520318 /* -W-4R */
NV_UBITMAP_SIZE_IN              EQU             000520318h
;  -W-UF 
;  3732 : #define NV_UBITMAP_SIZE_IN_WIDTH                               15:0 /* -W-UF */
NV_UBITMAP_SIZE_IN_WIDTH                TEXTEQU         <15:0>
;  -W-UF 
;  3733 : #define NV_UBITMAP_SIZE_IN_HEIGHT                             31:16 /* -W-UF */
NV_UBITMAP_SIZE_IN_HEIGHT               TEXTEQU         <31:16>
;  -W-4A 
;        
;  3734 : #define NV_UBITMAP_MONOCHROME(i)                 (0x00520400+(i)*4) /* -W-4A */
;  3735 : #define NV_UBITMAP_MONOCHROME__SIZE_1                            32 /*       */
NV_UBITMAP_MONOCHROME__SIZE_1           EQU             32t
;  -W-VF 
;  3736 : #define NV_UBITMAP_MONOCHROME_BITMAP                           31:0 /* -W-VF */
NV_UBITMAP_MONOCHROME_BITMAP            TEXTEQU         <31:0>
;  usr_image_from_mem.ref 
;  -W--D 
;  3737 : /* usr_image_from_mem.ref */
;  3738 : #define NV_UFROMEM                            0x00531FFF:0x00530000 /* -W--D */
NV_UFROMEM              TEXTEQU         <0x00531FFF:0x00530000>
;  -W-4R 
;  3739 : #define NV_UFROMEM_CTX_SWITCH                            0x00530000 /* -W-4R */
NV_UFROMEM_CTX_SWITCH           EQU             000530000h
;  -W-UF 
;  3740 : #define NV_UFROMEM_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
NV_UFROMEM_CTX_SWITCH_INSTANCE          TEXTEQU         <15:0>
;  -W-UF 
;  3741 : #define NV_UFROMEM_CTX_SWITCH_CHID                            22:16 /* -W-UF */
NV_UFROMEM_CTX_SWITCH_CHID              TEXTEQU         <22:16>
;  -W-VF 
;  3742 : #define NV_UFROMEM_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
NV_UFROMEM_CTX_SWITCH_VOLATILE          TEXTEQU         <31:31>
;  -W--V 
;  3743 : #define NV_UFROMEM_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
NV_UFROMEM_CTX_SWITCH_VOLATILE_IGNORE           EQU             000000000h
;  -W--V 
;  3744 : #define NV_UFROMEM_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
NV_UFROMEM_CTX_SWITCH_VOLATILE_RESET            EQU             000000001h
;  -W-4A 
;        
;  3745 : #define NV_UFROMEM_DMA_REPLY(i)                  (0x00530040+(i)*4) /* -W-4A */
;  3746 : #define NV_UFROMEM_DMA_REPLY__SIZE_1                             16 /*       */
NV_UFROMEM_DMA_REPLY__SIZE_1            EQU             16t
;  -W-VF 
;  3747 : #define NV_UFROMEM_DMA_REPLY_DATA                              31:0 /* -W-VF */
NV_UFROMEM_DMA_REPLY_DATA               TEXTEQU         <31:0>
;  -W-4R 
;  3748 : #define NV_UFROMEM_SET_NOTIFY                            0x00530104 /* -W-4R */
NV_UFROMEM_SET_NOTIFY           EQU             000530104h
;  -W-VF 
;  3749 : #define NV_UFROMEM_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
NV_UFROMEM_SET_NOTIFY_PARAMETER         TEXTEQU         <31:0>
;  -W--V 
;  3750 : #define NV_UFROMEM_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
NV_UFROMEM_SET_NOTIFY_PARAMETER_WRITE           EQU             000000000h
;  -W-4R 
;  3751 : #define NV_UFROMEM_POINT                                 0x00530308 /* -W-4R */
NV_UFROMEM_POINT                EQU             000530308h
;  -W-SF 
;  3752 : #define NV_UFROMEM_POINT_X                                     15:0 /* -W-SF */
NV_UFROMEM_POINT_X              TEXTEQU         <15:0>
;  -W-SF 
;  3753 : #define NV_UFROMEM_POINT_Y                                    31:16 /* -W-SF */
NV_UFROMEM_POINT_Y              TEXTEQU         <31:16>
;  -W-4R 
;  3754 : #define NV_UFROMEM_SIZE                                  0x0053030C /* -W-4R */
NV_UFROMEM_SIZE         EQU             00053030ch
;  -W-UF 
;  3755 : #define NV_UFROMEM_SIZE_WIDTH                                  15:0 /* -W-UF */
NV_UFROMEM_SIZE_WIDTH           TEXTEQU         <15:0>
;  -W-UF 
;  3756 : #define NV_UFROMEM_SIZE_HEIGHT                                31:16 /* -W-UF */
NV_UFROMEM_SIZE_HEIGHT          TEXTEQU         <31:16>
;  -W-4R 
;  3757 : #define NV_UFROMEM_PITCH                                 0x00530310 /* -W-4R */
NV_UFROMEM_PITCH                EQU             000530310h
;  -W-SF 
;  3758 : #define NV_UFROMEM_PITCH_VALUE                                 31:0 /* -W-SF */
NV_UFROMEM_PITCH_VALUE          TEXTEQU         <31:0>
;  -W-4R 
;  3759 : #define NV_UFROMEM_IMAGE_START                           0x00530314 /* -W-4R */
NV_UFROMEM_IMAGE_START          EQU             000530314h
;  -W-UF 
;  3760 : #define NV_UFROMEM_PITCH_START_OFFSET                          31:0 /* -W-UF */
NV_UFROMEM_PITCH_START_OFFSET           TEXTEQU         <31:0>
;  usr_image_to_mem.ref 
;  -W--D 
;  3761 : /* usr_image_to_mem.ref */
;  3762 : #define NV_UTOMEM                             0x00541FFF:0x00540000 /* -W--D */
NV_UTOMEM               TEXTEQU         <0x00541FFF:0x00540000>
;  -W-4R 
;  3763 : #define NV_UTOMEM_CTX_SWITCH                             0x00540000 /* -W-4R */
NV_UTOMEM_CTX_SWITCH            EQU             000540000h
;  -W-UF 
;  3764 : #define NV_UTOMEM_CTX_SWITCH_INSTANCE                          15:0 /* -W-UF */
NV_UTOMEM_CTX_SWITCH_INSTANCE           TEXTEQU         <15:0>
;  -W-UF 
;  3765 : #define NV_UTOMEM_CTX_SWITCH_CHID                             22:16 /* -W-UF */
NV_UTOMEM_CTX_SWITCH_CHID               TEXTEQU         <22:16>
;  -W-VF 
;  3766 : #define NV_UTOMEM_CTX_SWITCH_VOLATILE                         31:31 /* -W-VF */
NV_UTOMEM_CTX_SWITCH_VOLATILE           TEXTEQU         <31:31>
;  -W--V 
;  3767 : #define NV_UTOMEM_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* -W--V */
NV_UTOMEM_CTX_SWITCH_VOLATILE_IGNORE            EQU             000000000h
;  -W--V 
;  3768 : #define NV_UTOMEM_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
NV_UTOMEM_CTX_SWITCH_VOLATILE_RESET             EQU             000000001h
;  -W-4R 
;  3769 : #define NV_UTOMEM_SET_NOTIFY                             0x00540104 /* -W-4R */
NV_UTOMEM_SET_NOTIFY            EQU             000540104h
;  -W-VF 
;  3770 : #define NV_UTOMEM_SET_NOTIFY_PARAMETER                         31:0 /* -W-VF */
NV_UTOMEM_SET_NOTIFY_PARAMETER          TEXTEQU         <31:0>
;  -W--V 
;  3771 : #define NV_UTOMEM_SET_NOTIFY_PARAMETER_WRITE             0x00000000 /* -W--V */
NV_UTOMEM_SET_NOTIFY_PARAMETER_WRITE            EQU             000000000h
;  -W-4R 
;  3772 : #define NV_UTOMEM_POINT                                  0x00540308 /* -W-4R */
NV_UTOMEM_POINT         EQU             000540308h
;  -W-SF 
;  3773 : #define NV_UTOMEM_POINT_X                                      15:0 /* -W-SF */
NV_UTOMEM_POINT_X               TEXTEQU         <15:0>
;  -W-SF 
;  3774 : #define NV_UTOMEM_POINT_Y                                     31:16 /* -W-SF */
NV_UTOMEM_POINT_Y               TEXTEQU         <31:16>
;  -W-4R 
;  3775 : #define NV_UTOMEM_SIZE                                   0x0054030C /* -W-4R */
NV_UTOMEM_SIZE          EQU             00054030ch
;  -W-UF 
;  3776 : #define NV_UTOMEM_SIZE_WIDTH                                   15:0 /* -W-UF */
NV_UTOMEM_SIZE_WIDTH            TEXTEQU         <15:0>
;  -W-UF 
;  3777 : #define NV_UTOMEM_SIZE_HEIGHT                                 31:16 /* -W-UF */
NV_UTOMEM_SIZE_HEIGHT           TEXTEQU         <31:16>
;  -W-4R 
;  3778 : #define NV_UTOMEM_IMAGE_PITCH                            0x00540310 /* -W-4R */
NV_UTOMEM_IMAGE_PITCH           EQU             000540310h
;  -W-SF 
;  3779 : #define NV_UTOMEM_IMAGE_PITCH_VALUE                            31:0 /* -W-SF */
NV_UTOMEM_IMAGE_PITCH_VALUE             TEXTEQU         <31:0>
;  -W-4R 
;  3780 : #define NV_UTOMEM_IMAGE_START                            0x00540314 /* -W-4R */
NV_UTOMEM_IMAGE_START           EQU             000540314h
;  -W-UF 
;  3781 : #define NV_UTOMEM_IMAGE_START_OFFSET                           31:0 /* -W-UF */
NV_UTOMEM_IMAGE_START_OFFSET            TEXTEQU         <31:0>
;  usr_scaled_image_from_mem.ref 
;  -W--D 
;  3782 : /* usr_scaled_image_from_mem.ref */
;  3783 : #define NV_USCALED                            0x004E1FFF:0x004E0000 /* -W--D */
NV_USCALED              TEXTEQU         <0x004E1FFF:0x004E0000>
;  -W-4R 
;  3784 : #define NV_USCALED_CTX_SWITCH                            0x004E0000 /* -W-4R */
NV_USCALED_CTX_SWITCH           EQU             0004e0000h
;  -W-UF 
;  3785 : #define NV_USCALED_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
NV_USCALED_CTX_SWITCH_INSTANCE          TEXTEQU         <15:0>
;  -W-UF 
;  3786 : #define NV_USCALED_CTX_SWITCH_CHID                            22:16 /* -W-UF */
NV_USCALED_CTX_SWITCH_CHID              TEXTEQU         <22:16>
;  -W-VF 
;  3787 : #define NV_USCALED_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
NV_USCALED_CTX_SWITCH_VOLATILE          TEXTEQU         <31:31>
;  -W--V 
;  3788 : #define NV_USCALED_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
NV_USCALED_CTX_SWITCH_VOLATILE_IGNORE           EQU             000000000h
;  -W--V 
;  3789 : #define NV_USCALED_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
NV_USCALED_CTX_SWITCH_VOLATILE_RESET            EQU             000000001h
;  -W-4R 
;  3790 : #define NV_USCALED_SET_NOTIFY                            0x004E0104 /* -W-4R */
NV_USCALED_SET_NOTIFY           EQU             0004e0104h
;  -W-VF 
;  3791 : #define NV_USCALED_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
NV_USCALED_SET_NOTIFY_PARAMETER         TEXTEQU         <31:0>
;  -W--V 
;  3792 : #define NV_USCALED_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
NV_USCALED_SET_NOTIFY_PARAMETER_WRITE           EQU             000000000h
;  -W-4R 
;  3793 : #define NV_USCALED_CLIP_0                                0x004E0308 /* -W-4R */
NV_USCALED_CLIP_0               EQU             0004e0308h
;  -W-SF 
;  3794 : #define NV_USCALED_CLIP_0_X                                    15:0 /* -W-SF */
NV_USCALED_CLIP_0_X             TEXTEQU         <15:0>
;  -W-SF 
;  3795 : #define NV_USCALED_CLIP_0_Y                                   31:16 /* -W-SF */
NV_USCALED_CLIP_0_Y             TEXTEQU         <31:16>
;  -W-4R 
;  3796 : #define NV_USCALED_CLIP_1                                0x004E030C /* -W-4R */
NV_USCALED_CLIP_1               EQU             0004e030ch
;  -W-UF 
;  3797 : #define NV_USCALED_CLIP_1_WIDTH                                15:0 /* -W-UF */
NV_USCALED_CLIP_1_WIDTH         TEXTEQU         <15:0>
;  -W-UF 
;  3798 : #define NV_USCALED_CLIP_1_HEIGHT                              31:16 /* -W-UF */
NV_USCALED_CLIP_1_HEIGHT                TEXTEQU         <31:16>
;  -W-4R 
;  3799 : #define NV_USCALED_RECTANGLE_OUT_0                       0x004E0310 /* -W-4R */
NV_USCALED_RECTANGLE_OUT_0              EQU             0004e0310h
;  -W-SF 
;  3800 : #define NV_USCALED_RECTANGLE_OUT_0_X                           15:0 /* -W-SF */
NV_USCALED_RECTANGLE_OUT_0_X            TEXTEQU         <15:0>
;  -W-SF 
;  3801 : #define NV_USCALED_RECTANGLE_OUT_0_Y                          31:16 /* -W-SF */
NV_USCALED_RECTANGLE_OUT_0_Y            TEXTEQU         <31:16>
;  -W-4R 
;  3802 : #define NV_USCALED_RECTANGLE_OUT_1                       0x004E0314 /* -W-4R */
NV_USCALED_RECTANGLE_OUT_1              EQU             0004e0314h
;  -W-UF 
;  3803 : #define NV_USCALED_RECTANGLE_OUT_1_WIDTH                       15:0 /* -W-UF */
NV_USCALED_RECTANGLE_OUT_1_WIDTH                TEXTEQU         <15:0>
;  -W-UF 
;  3804 : #define NV_USCALED_RECTANGLE_OUT_1_HEIGHT                     31:16 /* -W-UF */
NV_USCALED_RECTANGLE_OUT_1_HEIGHT               TEXTEQU         <31:16>
;  -W-4R 
;  3805 : #define NV_USCALED_DELTA_DU_DX                           0x004E0318 /* -W-4R */
NV_USCALED_DELTA_DU_DX          EQU             0004e0318h
;  -W-SF 
;  3806 : #define NV_USCALED_DELTA_DU_DX_R_FRACTION                      19:0 /* -W-SF */
NV_USCALED_DELTA_DU_DX_R_FRACTION               TEXTEQU         <19:0>
;  -W-UF 
;  3807 : #define NV_USCALED_DELTA_DU_DX_R_INT                          31:20 /* -W-UF */
NV_USCALED_DELTA_DU_DX_R_INT            TEXTEQU         <31:20>
;  -W-UF 
;  3808 : #define NV_USCALED_DELTA_DU_DX_R                               31:0 /* -W-UF */
NV_USCALED_DELTA_DU_DX_R                TEXTEQU         <31:0>
;  -W-4R 
;  3809 : #define NV_USCALED_DELTA_DV_DY                           0x004E031C /* -W-4R */
NV_USCALED_DELTA_DV_DY          EQU             0004e031ch
;  -W-SF 
;  3810 : #define NV_USCALED_DELTA_DV_DY_R_FRACTION                      19:0 /* -W-SF */
NV_USCALED_DELTA_DV_DY_R_FRACTION               TEXTEQU         <19:0>
;  -W-UF 
;  3811 : #define NV_USCALED_DELTA_DV_DY_R_INT                          31:20 /* -W-UF */
NV_USCALED_DELTA_DV_DY_R_INT            TEXTEQU         <31:20>
;  -W-UF 
;  3812 : #define NV_USCALED_DELTA_DV_DY_R                               31:0 /* -W-UF */
NV_USCALED_DELTA_DV_DY_R                TEXTEQU         <31:0>
;  -W-4R 
;  3813 : #define NV_USCALED_SIZE                                  0x004E0400 /* -W-4R */
NV_USCALED_SIZE         EQU             0004e0400h
;  -W-UF 
;  3814 : #define NV_USCALED_SIZE_WIDTH                                  15:0 /* -W-UF */
NV_USCALED_SIZE_WIDTH           TEXTEQU         <15:0>
;  -W-UF 
;  3815 : #define NV_USCALED_SIZE_HEIGHT                                31:16 /* -W-UF */
NV_USCALED_SIZE_HEIGHT          TEXTEQU         <31:16>
;  -W-4R 
;  3816 : #define NV_USCALED_PITCH                                 0x004E0404 /* -W-4R */
NV_USCALED_PITCH                EQU             0004e0404h
;  -W-SF 
;  3817 : #define NV_USCALED_PITCH_VALUE                                 31:0 /* -W-SF */
NV_USCALED_PITCH_VALUE          TEXTEQU         <31:0>
;  -W-4R 
;  3818 : #define NV_USCALED_OFFSET                                0x004E0408 /* -W-4R */
NV_USCALED_OFFSET               EQU             0004e0408h
;  -W-UF 
;  3819 : #define NV_USCALED_OFFSET_VALUE                               31:0  /* -W-UF */
NV_USCALED_OFFSET_VALUE         TEXTEQU         <31:0>
;  -W-4R 
;  3820 : #define NV_USCALED_POINT                                 0x004E040C /* -W-4R */
NV_USCALED_POINT                EQU             0004e040ch
;  -W-UF 
;  3821 : #define NV_USCALED_POINT_V_FRACTION                            11:0 /* -W-UF */
NV_USCALED_POINT_V_FRACTION             TEXTEQU         <11:0>
;  -W-UF 
;  3822 : #define NV_USCALED_POINT_V_INT                                15:12 /* -W-UF */
NV_USCALED_POINT_V_INT          TEXTEQU         <15:12>
;  -W-UF 
;  3823 : #define NV_USCALED_POINT_V_VALUE                               15:0 /* -W-UF */
NV_USCALED_POINT_V_VALUE                TEXTEQU         <15:0>
;  -W-UF 
;  3824 : #define NV_USCALED_POINT_U_FRACTION                           19:16 /* -W-UF */
NV_USCALED_POINT_U_FRACTION             TEXTEQU         <19:16>
;  -W-UF 
;  3825 : #define NV_USCALED_POINT_U_INT                                31:20 /* -W-UF */
NV_USCALED_POINT_U_INT          TEXTEQU         <31:20>
;  -W-UF 
;  3826 : #define NV_USCALED_POINT_U_VALUE                               31:0 /* -W-UF */
NV_USCALED_POINT_U_VALUE                TEXTEQU         <31:0>
;  -W-4R 
;  3827 : #define NV_USCALED_SIZE_Y                                0x004E0500 /* -W-4R */
NV_USCALED_SIZE_Y               EQU             0004e0500h
;  -W-UF 
;  3828 : #define NV_USCALED_SIZE_Y_WIDTH                                15:0 /* -W-UF */
NV_USCALED_SIZE_Y_WIDTH         TEXTEQU         <15:0>
;  -W-UF 
;  3829 : #define NV_USCALED_SIZE_Y_HEIGHT                              31:16 /* -W-UF */
NV_USCALED_SIZE_Y_HEIGHT                TEXTEQU         <31:16>
;  -W-4R 
;  3830 : #define NV_USCALED_PITCH_420                             0x004E0504 /* -W-4R */
NV_USCALED_PITCH_420            EQU             0004e0504h
;  -W-UF 
;  3831 : #define NV_USCALED_PITCH_420_Y                                 15:0 /* -W-UF */
NV_USCALED_PITCH_420_Y          TEXTEQU         <15:0>
;  -W-UF 
;  3832 : #define NV_USCALED_PITCH_420_UV                               31:16 /* -W-UF */
NV_USCALED_PITCH_420_UV         TEXTEQU         <31:16>
;  -W-4R 
;  3833 : #define NV_USCALED_OFFSET_Y                              0x004E0508 /* -W-4R */
NV_USCALED_OFFSET_Y             EQU             0004e0508h
;  -W-UF 
;  3834 : #define NV_USCALED_OFFSET_Y_VALUE                             31:0  /* -W-UF */
NV_USCALED_OFFSET_Y_VALUE               TEXTEQU         <31:0>
;  -W-4R 
;  3835 : #define NV_USCALED_OFFSET_U                              0x004E050C /* -W-4R */
NV_USCALED_OFFSET_U             EQU             0004e050ch
;  -W-UF 
;  3836 : #define NV_USCALED_OFFSET_U_VALUE                             31:0  /* -W-UF */
NV_USCALED_OFFSET_U_VALUE               TEXTEQU         <31:0>
;  -W-4R 
;  3837 : #define NV_USCALED_OFFSET_V                              0x004E0510 /* -W-4R */
NV_USCALED_OFFSET_V             EQU             0004e0510h
;  -W-UF 
;  3838 : #define NV_USCALED_OFFSET_V_VALUE                             31:0  /* -W-UF */
NV_USCALED_OFFSET_V_VALUE               TEXTEQU         <31:0>
;  -W-4R 
;  3839 : #define NV_USCALED_POINT_Y                               0x004E0514 /* -W-4R */
NV_USCALED_POINT_Y              EQU             0004e0514h
;  -W-UF 
;  3840 : #define NV_USCALED_POINT_Y_V_FRACTION                          11:0 /* -W-UF */
NV_USCALED_POINT_Y_V_FRACTION           TEXTEQU         <11:0>
;  -W-UF 
;  3841 : #define NV_USCALED_POINT_Y_V_INT                              15:12 /* -W-UF */
NV_USCALED_POINT_Y_V_INT                TEXTEQU         <15:12>
;  -W-UF 
;  3842 : #define NV_USCALED_POINT_Y_V_VALUE                             15:0 /* -W-UF */
NV_USCALED_POINT_Y_V_VALUE              TEXTEQU         <15:0>
;  -W-UF 
;  3843 : #define NV_USCALED_POINT_Y_U_FRACTION                         19:16 /* -W-UF */
NV_USCALED_POINT_Y_U_FRACTION           TEXTEQU         <19:16>
;  -W-UF 
;  3844 : #define NV_USCALED_POINT_Y_U_INT                              31:20 /* -W-UF */
NV_USCALED_POINT_Y_U_INT                TEXTEQU         <31:20>
;  -W-UF 
;  3845 : #define NV_USCALED_POINT_Y_U_VALUE                            31:16 /* -W-UF */
NV_USCALED_POINT_Y_U_VALUE              TEXTEQU         <31:16>
;  usr_stretch_from_cpu.ref 
;  -W--D 
;  3846 : /* usr_stretch_from_cpu.ref */
;  3847 : #define NV_USTRTCH                            0x00551FFF:0x00550000 /* -W--D */
NV_USTRTCH              TEXTEQU         <0x00551FFF:0x00550000>
;  -W-4R 
;  3848 : #define NV_USTRTCH_CTX_SWITCH                            0x00550000 /* -W-4R */
NV_USTRTCH_CTX_SWITCH           EQU             000550000h
;  -W-UF 
;  3849 : #define NV_USTRTCH_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
NV_USTRTCH_CTX_SWITCH_INSTANCE          TEXTEQU         <15:0>
;  -W-UF 
;  3850 : #define NV_USTRTCH_CTX_SWITCH_CHID                            22:16 /* -W-UF */
NV_USTRTCH_CTX_SWITCH_CHID              TEXTEQU         <22:16>
;  -W-VF 
;  3851 : #define NV_USTRTCH_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
NV_USTRTCH_CTX_SWITCH_VOLATILE          TEXTEQU         <31:31>
;  -W--V 
;  3852 : #define NV_USTRTCH_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
NV_USTRTCH_CTX_SWITCH_VOLATILE_IGNORE           EQU             000000000h
;  -W--V 
;  3853 : #define NV_USTRTCH_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
NV_USTRTCH_CTX_SWITCH_VOLATILE_RESET            EQU             000000001h
;  -W-4R 
;  3854 : #define NV_USTRTCH_SET_NOTIFY                            0x00550104 /* -W-4R */
NV_USTRTCH_SET_NOTIFY           EQU             000550104h
;  -W-VF 
;  3855 : #define NV_USTRTCH_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
NV_USTRTCH_SET_NOTIFY_PARAMETER         TEXTEQU         <31:0>
;  -W--V 
;  3856 : #define NV_USTRTCH_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
NV_USTRTCH_SET_NOTIFY_PARAMETER_WRITE           EQU             000000000h
;  -W-4R 
;  3857 : #define NV_USTRTCH_SIZE_IN                               0x00550304 /* -W-4R */
NV_USTRTCH_SIZE_IN              EQU             000550304h
;  -W-UF 
;  3858 : #define NV_USTRTCH_SIZE_IN_WIDTH                               15:0 /* -W-UF */
NV_USTRTCH_SIZE_IN_WIDTH                TEXTEQU         <15:0>
;  -W-UF 
;  3859 : #define NV_USTRTCH_SIZE_IN_HEIGHT                             31:16 /* -W-UF */
NV_USTRTCH_SIZE_IN_HEIGHT               TEXTEQU         <31:16>
;  -W-4R 
;  3860 : #define NV_USTRTCH_DELTA_DX_DU                           0x00550308 /* -W-4R */
NV_USTRTCH_DELTA_DX_DU          EQU             000550308h
;  -W-UF 
;  3861 : #define NV_USTRTCH_DELTA_DX_DU_R_FRACTION                      19:0 /* -W-UF */
NV_USTRTCH_DELTA_DX_DU_R_FRACTION               TEXTEQU         <19:0>
;  -W-UF 
;  3862 : #define NV_USTRTCH_DELTA_DX_DU_R_INT                          31:20 /* -W-UF */
NV_USTRTCH_DELTA_DX_DU_R_INT            TEXTEQU         <31:20>
;  -W-UF 
;  3863 : #define NV_USTRTCH_DELTA_DX_DU_R                               31:0 /* -W-UF */
NV_USTRTCH_DELTA_DX_DU_R                TEXTEQU         <31:0>
;  -W-4R 
;  3864 : #define NV_USTRTCH_DELTA_DY_DV                           0x0055030C /* -W-4R */
NV_USTRTCH_DELTA_DY_DV          EQU             00055030ch
;  -W-UF 
;  3865 : #define NV_USTRTCH_DELTA_DY_DV_R_FRACTION                      19:0 /* -W-UF */
NV_USTRTCH_DELTA_DY_DV_R_FRACTION               TEXTEQU         <19:0>
;  -W-UF 
;  3866 : #define NV_USTRTCH_DELTA_DY_DV_R_INT                          31:20 /* -W-UF */
NV_USTRTCH_DELTA_DY_DV_R_INT            TEXTEQU         <31:20>
;  -W-UF 
;  3867 : #define NV_USTRTCH_DELTA_DY_DV_R                               31:0 /* -W-UF */
NV_USTRTCH_DELTA_DY_DV_R                TEXTEQU         <31:0>
;  -W-4R 
;  3868 : #define NV_USTRTCH_CLIP_0                                0x00550310 /* -W-4R */
NV_USTRTCH_CLIP_0               EQU             000550310h
;  -W-SF 
;  3869 : #define NV_USTRTCH_CLIP_0_X                                    15:0 /* -W-SF */
NV_USTRTCH_CLIP_0_X             TEXTEQU         <15:0>
;  -W-SF 
;  3870 : #define NV_USTRTCH_CLIP_0_Y                                   31:16 /* -W-SF */
NV_USTRTCH_CLIP_0_Y             TEXTEQU         <31:16>
;  -W-4R 
;  3871 : #define NV_USTRTCH_CLIP_1                                0x00550314 /* -W-4R */
NV_USTRTCH_CLIP_1               EQU             000550314h
;  -W-UF 
;  3872 : #define NV_USTRTCH_CLIP_1_WIDTH                                15:0 /* -W-UF */
NV_USTRTCH_CLIP_1_WIDTH         TEXTEQU         <15:0>
;  -W-UF 
;  3873 : #define NV_USTRTCH_CLIP_1_HEIGHT                              31:16 /* -W-UF */
NV_USTRTCH_CLIP_1_HEIGHT                TEXTEQU         <31:16>
;  -W-4R 
;  3874 : #define NV_USTRTCH_POINT12D4                             0x00550318 /* -W-4R */
NV_USTRTCH_POINT12D4            EQU             000550318h
;  -W-SF 
;  3875 : #define NV_USTRTCH_POINT12D4_X_FRACTION                         3:0 /* -W-SF */
NV_USTRTCH_POINT12D4_X_FRACTION         TEXTEQU         <3:0>
;  -W-SF 
;  3876 : #define NV_USTRTCH_POINT12D4_X_INT                             15:4 /* -W-SF */
NV_USTRTCH_POINT12D4_X_INT              TEXTEQU         <15:4>
;  -W-SF 
;  3877 : #define NV_USTRTCH_POINT12D4_X                                 15:0 /* -W-SF */
NV_USTRTCH_POINT12D4_X          TEXTEQU         <15:0>
;  -W-SF 
;  3878 : #define NV_USTRTCH_POINT12D4_Y_FRACTION                       19:16 /* -W-SF */
NV_USTRTCH_POINT12D4_Y_FRACTION         TEXTEQU         <19:16>
;  -W-SF 
;  3879 : #define NV_USTRTCH_POINT12D4_Y_INT                            31:20 /* -W-SF */
NV_USTRTCH_POINT12D4_Y_INT              TEXTEQU         <31:20>
;  -W-SF 
;  3880 : #define NV_USTRTCH_POINT12D4_Y                                31:16 /* -W-SF */
NV_USTRTCH_POINT12D4_Y          TEXTEQU         <31:16>
;  -W-4A 
;        
;  3881 : #define NV_USTRTCH_COLOR(i)                      (0x00550400+(i)*4) /* -W-4A */
;  3882 : #define NV_USTRTCH_COLOR__SIZE_1                               1792 /*       */
NV_USTRTCH_COLOR__SIZE_1                EQU             1792t
;  -W-VF 
;  3883 : #define NV_USTRTCH_COLOR_VALUE                                 31:0 /* -W-VF */
NV_USTRTCH_COLOR_VALUE          TEXTEQU         <31:0>
;  usr_win95_text.ref 
;  -W--D 
;  3884 : /* usr_win95_text.ref */
;  3885 : #define NV_UW95TXT                            0x004C1FFF:0x004C0000 /* -W--D */
NV_UW95TXT              TEXTEQU         <0x004C1FFF:0x004C0000>
;  -W-4R 
;  3886 : #define NV_UW95TXT_CTX_SWITCH                            0x004C0000 /* -W-4R */
NV_UW95TXT_CTX_SWITCH           EQU             0004c0000h
;  -W-UF 
;  3887 : #define NV_UW95TXT_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
NV_UW95TXT_CTX_SWITCH_INSTANCE          TEXTEQU         <15:0>
;  -W-UF 
;  3888 : #define NV_UW95TXT_CTX_SWITCH_CHID                            22:16 /* -W-UF */
NV_UW95TXT_CTX_SWITCH_CHID              TEXTEQU         <22:16>
;  -W-VF 
;  3889 : #define NV_UW95TXT_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
NV_UW95TXT_CTX_SWITCH_VOLATILE          TEXTEQU         <31:31>
;  -W--V 
;  3890 : #define NV_UW95TXT_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
NV_UW95TXT_CTX_SWITCH_VOLATILE_IGNORE           EQU             000000000h
;  -W--V 
;  3891 : #define NV_UW95TXT_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
NV_UW95TXT_CTX_SWITCH_VOLATILE_RESET            EQU             000000001h
;  -W-4R 
;  3892 : #define NV_UW95TXT_SET_NOTIFY                            0x004C0104 /* -W-4R */
NV_UW95TXT_SET_NOTIFY           EQU             0004c0104h
;  -W-VF 
;  3893 : #define NV_UW95TXT_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
NV_UW95TXT_SET_NOTIFY_PARAMETER         TEXTEQU         <31:0>
;  -W--V 
;  3894 : #define NV_UW95TXT_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
NV_UW95TXT_SET_NOTIFY_PARAMETER_WRITE           EQU             000000000h
;  -W-4R 
;  3895 : #define NV_UW95TXT_COLOR_A                               0x004C03FC /* -W-4R */
NV_UW95TXT_COLOR_A              EQU             0004c03fch
;  -W-VF 
;  3896 : #define NV_UW95TXT_COLOR_A_VALUE                               31:0 /* -W-VF */
NV_UW95TXT_COLOR_A_VALUE                TEXTEQU         <31:0>
;  -W-4A 
;        
;  3897 : #define NV_UW95TXT_RECT_NCLIP_0(i)               (0x004C0400+(i)*8) /* -W-4A */
;  3898 : #define NV_UW95TXT_RECT_NCLIP_0__SIZE_1                          64 /*       */
NV_UW95TXT_RECT_NCLIP_0__SIZE_1         EQU             64t
;  -W-SF 
;  3899 : #define NV_UW95TXT_RECT_NCLIP_0_Y                              15:0 /* -W-SF */
NV_UW95TXT_RECT_NCLIP_0_Y               TEXTEQU         <15:0>
;  -W-SF 
;  3900 : #define NV_UW95TXT_RECT_NCLIP_0_X                             31:16 /* -W-SF */
NV_UW95TXT_RECT_NCLIP_0_X               TEXTEQU         <31:16>
;  -W-4A 
;        
;  3901 : #define NV_UW95TXT_RECT_NCLIP_1(i)               (0x004C0404+(i)*8) /* -W-4A */
;  3902 : #define NV_UW95TXT_RECT_NCLIP_1__SIZE_1                          64 /*       */
NV_UW95TXT_RECT_NCLIP_1__SIZE_1         EQU             64t
;  -W-UF 
;  3903 : #define NV_UW95TXT_RECT_NCLIP_1_HEIGHT                         15:0 /* -W-UF */
NV_UW95TXT_RECT_NCLIP_1_HEIGHT          TEXTEQU         <15:0>
;  -W-UF 
;  3904 : #define NV_UW95TXT_RECT_NCLIP_1_WIDTH                         31:16 /* -W-UF */
NV_UW95TXT_RECT_NCLIP_1_WIDTH           TEXTEQU         <31:16>
;  -W-4R 
;  3905 : #define NV_UW95TXT_CLIP_B_0                              0x004C07F4 /* -W-4R */
NV_UW95TXT_CLIP_B_0             EQU             0004c07f4h
;  -W-SF 
;  3906 : #define NV_UW95TXT_CLIP_B_0_LEFT                               15:0 /* -W-SF */
NV_UW95TXT_CLIP_B_0_LEFT                TEXTEQU         <15:0>
;  -W-SF 
;  3907 : #define NV_UW95TXT_CLIP_B_0_TOP                               31:16 /* -W-SF */
NV_UW95TXT_CLIP_B_0_TOP         TEXTEQU         <31:16>
;  -W-4R 
;  3908 : #define NV_UW95TXT_CLIP_B_1                              0x004C07F8 /* -W-4R */
NV_UW95TXT_CLIP_B_1             EQU             0004c07f8h
;  -W-SF 
;  3909 : #define NV_UW95TXT_CLIP_B_1_RIGHT                              15:0 /* -W-SF */
NV_UW95TXT_CLIP_B_1_RIGHT               TEXTEQU         <15:0>
;  -W-SF 
;  3910 : #define NV_UW95TXT_CLIP_B_1_BOTTOM                            31:16 /* -W-SF */
NV_UW95TXT_CLIP_B_1_BOTTOM              TEXTEQU         <31:16>
;  -W-4R 
;  3911 : #define NV_UW95TXT_COLOR_B                               0x004C07FC /* -W-4R */
NV_UW95TXT_COLOR_B              EQU             0004c07fch
;  -W-VF 
;  3912 : #define NV_UW95TXT_COLOR_B_VALUE                               31:0 /* -W-VF */
NV_UW95TXT_COLOR_B_VALUE                TEXTEQU         <31:0>
;  -W-4A 
;        
;  3913 : #define NV_UW95TXT_RECT_CLIP_0(i)                (0x004C0800+(i)*8) /* -W-4A */
;  3914 : #define NV_UW95TXT_RECT_CLIP_0__SIZE_1                           64 /*       */
NV_UW95TXT_RECT_CLIP_0__SIZE_1          EQU             64t
;  -W-SF 
;  3915 : #define NV_UW95TXT_RECT_CLIP_0_LEFT                            15:0 /* -W-SF */
NV_UW95TXT_RECT_CLIP_0_LEFT             TEXTEQU         <15:0>
;  -W-SF 
;  3916 : #define NV_UW95TXT_RECT_CLIP_0_TOP                            31:16 /* -W-SF */
NV_UW95TXT_RECT_CLIP_0_TOP              TEXTEQU         <31:16>
;  -W-4A 
;        
;  3917 : #define NV_UW95TXT_RECT_CLIP_1(i)                (0x004C0804+(i)*8) /* -W-4A */
;  3918 : #define NV_UW95TXT_RECT_CLIP_1__SIZE_1                           64 /*       */
NV_UW95TXT_RECT_CLIP_1__SIZE_1          EQU             64t
;  -W-SF 
;  3919 : #define NV_UW95TXT_RECT_CLIP_1_RIGHT                           15:0 /* -W-SF */
NV_UW95TXT_RECT_CLIP_1_RIGHT            TEXTEQU         <15:0>
;  -W-SF 
;  3920 : #define NV_UW95TXT_RECT_CLIP_1_BOTTOM                         31:16 /* -W-SF */
NV_UW95TXT_RECT_CLIP_1_BOTTOM           TEXTEQU         <31:16>
;  -W-4R 
;  3921 : #define NV_UW95TXT_CLIP_C_0                              0x004C0BEC /* -W-4R */
NV_UW95TXT_CLIP_C_0             EQU             0004c0bech
;  -W-SF 
;  3922 : #define NV_UW95TXT_CLIP_C_0_LEFT                               15:0 /* -W-SF */
NV_UW95TXT_CLIP_C_0_LEFT                TEXTEQU         <15:0>
;  -W-SF 
;  3923 : #define NV_UW95TXT_CLIP_C_0_TOP                               31:16 /* -W-SF */
NV_UW95TXT_CLIP_C_0_TOP         TEXTEQU         <31:16>
;  -W-4R 
;  3924 : #define NV_UW95TXT_CLIP_C_1                              0x004C0BF0 /* -W-4R */
NV_UW95TXT_CLIP_C_1             EQU             0004c0bf0h
;  -W-SF 
;  3925 : #define NV_UW95TXT_CLIP_C_1_RIGHT                              15:0 /* -W-SF */
NV_UW95TXT_CLIP_C_1_RIGHT               TEXTEQU         <15:0>
;  -W-SF 
;  3926 : #define NV_UW95TXT_CLIP_C_1_BOTTOM                            31:16 /* -W-SF */
NV_UW95TXT_CLIP_C_1_BOTTOM              TEXTEQU         <31:16>
;  -W-4R 
;  3927 : #define NV_UW95TXT_COLOR1_C                              0x004C0BF4 /* -W-4R */
NV_UW95TXT_COLOR1_C             EQU             0004c0bf4h
;  -W-VF 
;  3928 : #define NV_UW95TXT_COLOR1_C_VALUE                              31:0 /* -W-VF */
NV_UW95TXT_COLOR1_C_VALUE               TEXTEQU         <31:0>
;  -W-4R 
;  3929 : #define NV_UW95TXT_SIZE_C                                0x004C0BF8 /* -W-4R */
NV_UW95TXT_SIZE_C               EQU             0004c0bf8h
;  -W-UF 
;  3930 : #define NV_UW95TXT_SIZE_C_WIDTH                                15:0 /* -W-UF */
NV_UW95TXT_SIZE_C_WIDTH         TEXTEQU         <15:0>
;  -W-UF 
;  3931 : #define NV_UW95TXT_SIZE_C_HEIGHT                              31:16 /* -W-UF */
NV_UW95TXT_SIZE_C_HEIGHT                TEXTEQU         <31:16>
;  -W-4R 
;  3932 : #define NV_UW95TXT_POINT_C                               0x004C0BFC /* -W-4R */
NV_UW95TXT_POINT_C              EQU             0004c0bfch
;  -W-SF 
;  3933 : #define NV_UW95TXT_POINT_C_X                                   15:0 /* -W-SF */
NV_UW95TXT_POINT_C_X            TEXTEQU         <15:0>
;  -W-SF 
;  3934 : #define NV_UW95TXT_POINT_C_Y                                  31:16 /* -W-SF */
NV_UW95TXT_POINT_C_Y            TEXTEQU         <31:16>
;  -W-4A 
;        
;  3935 : #define NV_UW95TXT_MONO_COLOR1_C(i)              (0x004C0C00+(i)*4) /* -W-4A */
;  3936 : #define NV_UW95TXT_MONO_COLOR1_C__SIZE_1                        128 /*       */
NV_UW95TXT_MONO_COLOR1_C__SIZE_1                EQU             128t
;  -W-VF 
;  3937 : #define NV_UW95TXT_MONO_COLOR1_C_BITMAP                        31:0 /* -W-VF */
NV_UW95TXT_MONO_COLOR1_C_BITMAP         TEXTEQU         <31:0>
;  -W-4R 
;  3938 : #define NV_UW95TXT_CLIP_D_0                              0x004C0FE8 /* -W-4R */
NV_UW95TXT_CLIP_D_0             EQU             0004c0fe8h
;  -W-SF 
;  3939 : #define NV_UW95TXT_CLIP_D_0_LEFT                               15:0 /* -W-SF */
NV_UW95TXT_CLIP_D_0_LEFT                TEXTEQU         <15:0>
;  -W-SF 
;  3940 : #define NV_UW95TXT_CLIP_D_0_TOP                               31:16 /* -W-SF */
NV_UW95TXT_CLIP_D_0_TOP         TEXTEQU         <31:16>
;  -W-4R 
;  3941 : #define NV_UW95TXT_CLIP_D_1                              0x004C0FEC /* -W-4R */
NV_UW95TXT_CLIP_D_1             EQU             0004c0fech
;  -W-SF 
;  3942 : #define NV_UW95TXT_CLIP_D_1_RIGHT                              15:0 /* -W-SF */
NV_UW95TXT_CLIP_D_1_RIGHT               TEXTEQU         <15:0>
;  -W-SF 
;  3943 : #define NV_UW95TXT_CLIP_D_1_BOTTOM                            31:16 /* -W-SF */
NV_UW95TXT_CLIP_D_1_BOTTOM              TEXTEQU         <31:16>
;  -W-4R 
;  3944 : #define NV_UW95TXT_COLOR1_D                              0x004C0FF0 /* -W-4R */
NV_UW95TXT_COLOR1_D             EQU             0004c0ff0h
;  -W-VF 
;  3945 : #define NV_UW95TXT_COLOR1_D_VALUE                              31:0 /* -W-VF */
NV_UW95TXT_COLOR1_D_VALUE               TEXTEQU         <31:0>
;  -W-4R 
;  3946 : #define NV_UW95TXT_SIZE_IN_D                             0x004C0FF4 /* -W-4R */
NV_UW95TXT_SIZE_IN_D            EQU             0004c0ff4h
;  -W-UF 
;  3947 : #define NV_UW95TXT_SIZE_IN_D_WIDTH                             15:0 /* -W-UF */
NV_UW95TXT_SIZE_IN_D_WIDTH              TEXTEQU         <15:0>
;  -W-UF 
;  3948 : #define NV_UW95TXT_SIZE_IN_D_HEIGHT                           31:16 /* -W-UF */
NV_UW95TXT_SIZE_IN_D_HEIGHT             TEXTEQU         <31:16>
;  -W-4R 
;  3949 : #define NV_UW95TXT_SIZE_OUT_D                            0x004C0FF8 /* -W-4R */
NV_UW95TXT_SIZE_OUT_D           EQU             0004c0ff8h
;  -W-UF 
;  3950 : #define NV_UW95TXT_SIZE_OUT_D_WIDTH                            15:0 /* -W-UF */
NV_UW95TXT_SIZE_OUT_D_WIDTH             TEXTEQU         <15:0>
;  -W-UF 
;  3951 : #define NV_UW95TXT_SIZE_OUT_D_HEIGHT                          31:16 /* -W-UF */
NV_UW95TXT_SIZE_OUT_D_HEIGHT            TEXTEQU         <31:16>
;  -W-4R 
;  3952 : #define NV_UW95TXT_POINT_D                               0x004C0FFC /* -W-4R */
NV_UW95TXT_POINT_D              EQU             0004c0ffch
;  -W-SF 
;  3953 : #define NV_UW95TXT_POINT_D_X                                   15:0 /* -W-SF */
NV_UW95TXT_POINT_D_X            TEXTEQU         <15:0>
;  -W-SF 
;  3954 : #define NV_UW95TXT_POINT_D_Y                                  31:16 /* -W-SF */
NV_UW95TXT_POINT_D_Y            TEXTEQU         <31:16>
;  -W-4A 
;        
;  3955 : #define NV_UW95TXT_MONO_COLOR1_D(i)              (0x004C1000+(i)*4) /* -W-4A */
;  3956 : #define NV_UW95TXT_MONO_COLOR1_D__SIZE_1                        128 /*       */
NV_UW95TXT_MONO_COLOR1_D__SIZE_1                EQU             128t
;  -W-VF 
;  3957 : #define NV_UW95TXT_MONO_COLOR1_D_BITMAP                        31:0 /* -W-VF */
NV_UW95TXT_MONO_COLOR1_D_BITMAP         TEXTEQU         <31:0>
;  -W-4R 
;  3958 : #define NV_UW95TXT_CLIP_E_0                              0x004C13E4 /* -W-4R */
NV_UW95TXT_CLIP_E_0             EQU             0004c13e4h
;  -W-SF 
;  3959 : #define NV_UW95TXT_CLIP_E_0_LEFT                               15:0 /* -W-SF */
NV_UW95TXT_CLIP_E_0_LEFT                TEXTEQU         <15:0>
;  -W-SF 
;  3960 : #define NV_UW95TXT_CLIP_E_0_TOP                               31:16 /* -W-SF */
NV_UW95TXT_CLIP_E_0_TOP         TEXTEQU         <31:16>
;  -W-4R 
;  3961 : #define NV_UW95TXT_CLIP_E_1                              0x004C13E8 /* -W-4R */
NV_UW95TXT_CLIP_E_1             EQU             0004c13e8h
;  -W-SF 
;  3962 : #define NV_UW95TXT_CLIP_E_1_RIGHT                              15:0 /* -W-SF */
NV_UW95TXT_CLIP_E_1_RIGHT               TEXTEQU         <15:0>
;  -W-SF 
;  3963 : #define NV_UW95TXT_CLIP_E_1_BOTTOM                            31:16 /* -W-SF */
NV_UW95TXT_CLIP_E_1_BOTTOM              TEXTEQU         <31:16>
;  -W-4R 
;  3964 : #define NV_UW95TXT_COLOR0_E                              0x004C13EC /* -W-4R */
NV_UW95TXT_COLOR0_E             EQU             0004c13ech
;  -W-VF 
;  3965 : #define NV_UW95TXT_COLOR0_E_VALUE                              31:0 /* -W-VF */
NV_UW95TXT_COLOR0_E_VALUE               TEXTEQU         <31:0>
;  -W-4R 
;  3966 : #define NV_UW95TXT_COLOR1_E                              0x004C13F0 /* -W-4R */
NV_UW95TXT_COLOR1_E             EQU             0004c13f0h
;  -W-VF 
;  3967 : #define NV_UW95TXT_COLOR1_E_VALUE                              31:0 /* -W-VF */
NV_UW95TXT_COLOR1_E_VALUE               TEXTEQU         <31:0>
;  -W-4R 
;  3968 : #define NV_UW95TXT_SIZE_IN_E                             0x004C13F4 /* -W-4R */
NV_UW95TXT_SIZE_IN_E            EQU             0004c13f4h
;  -W-UF 
;  3969 : #define NV_UW95TXT_SIZE_IN_E_WIDTH                             15:0 /* -W-UF */
NV_UW95TXT_SIZE_IN_E_WIDTH              TEXTEQU         <15:0>
;  -W-UF 
;  3970 : #define NV_UW95TXT_SIZE_IN_E_HEIGHT                           31:16 /* -W-UF */
NV_UW95TXT_SIZE_IN_E_HEIGHT             TEXTEQU         <31:16>
;  -W-4R 
;  3971 : #define NV_UW95TXT_SIZE_OUT_E                            0x004C13F8 /* -W-4R */
NV_UW95TXT_SIZE_OUT_E           EQU             0004c13f8h
;  -W-UF 
;  3972 : #define NV_UW95TXT_SIZE_OUT_E_WIDTH                            15:0 /* -W-UF */
NV_UW95TXT_SIZE_OUT_E_WIDTH             TEXTEQU         <15:0>
;  -W-UF 
;  3973 : #define NV_UW95TXT_SIZE_OUT_E_HEIGHT                          31:16 /* -W-UF */
NV_UW95TXT_SIZE_OUT_E_HEIGHT            TEXTEQU         <31:16>
;  -W-4R 
;  3974 : #define NV_UW95TXT_POINT_E                               0x004C13FC /* -W-4R */
NV_UW95TXT_POINT_E              EQU             0004c13fch
;  -W-SF 
;  3975 : #define NV_UW95TXT_POINT_E_X                                   15:0 /* -W-SF */
NV_UW95TXT_POINT_E_X            TEXTEQU         <15:0>
;  -W-SF 
;  3976 : #define NV_UW95TXT_POINT_E_Y                                  31:16 /* -W-SF */
NV_UW95TXT_POINT_E_Y            TEXTEQU         <31:16>
;  -W-4A 
;        
;  3977 : #define NV_UW95TXT_MONO_COLOR01_E(i)             (0x004C1400+(i)*4) /* -W-4A */
;  3978 : #define NV_UW95TXT_MONO_COLOR01_E__SIZE_1                       128 /*       */
NV_UW95TXT_MONO_COLOR01_E__SIZE_1               EQU             128t
;  -W-VF 
;  3979 : #define NV_UW95TXT_MONO_COLOR01_E_BITMAP                       31:0 /* -W-VF */
NV_UW95TXT_MONO_COLOR01_E_BITMAP                TEXTEQU         <31:0>
;  usr_image_in_mem.ref 
;  -W--D 
;  3980 : /* usr_image_in_mem.ref */
;  3981 : #define NV_UINMEM                             0x005C1FFF:0x005C0000 /* -W--D */
NV_UINMEM               TEXTEQU         <0x005C1FFF:0x005C0000>
;  -W-4R 
;  3982 : #define NV_UINMEM_CTX_SWITCH                             0x005C0000 /* -W-4R */
NV_UINMEM_CTX_SWITCH            EQU             0005c0000h
;  -W-UF 
;  3983 : #define NV_UINMEM_CTX_SWITCH_INSTANCE                          15:0 /* -W-UF */
NV_UINMEM_CTX_SWITCH_INSTANCE           TEXTEQU         <15:0>
;  -W-UF 
;  3984 : #define NV_UINMEM_CTX_SWITCH_CHID                             22:16 /* -W-UF */
NV_UINMEM_CTX_SWITCH_CHID               TEXTEQU         <22:16>
;  -W-VF 
;  3985 : #define NV_UINMEM_CTX_SWITCH_VOLATILE                         31:31 /* -W-VF */
NV_UINMEM_CTX_SWITCH_VOLATILE           TEXTEQU         <31:31>
;  -W--V 
;  3986 : #define NV_UINMEM_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* -W--V */
NV_UINMEM_CTX_SWITCH_VOLATILE_IGNORE            EQU             000000000h
;  -W--V 
;  3987 : #define NV_UINMEM_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
NV_UINMEM_CTX_SWITCH_VOLATILE_RESET             EQU             000000001h
;  -W-4R 
;  3988 : #define NV_UINMEM_SET_NOTIFY                             0x005C0104 /* -W-4R */
NV_UINMEM_SET_NOTIFY            EQU             0005c0104h
;  -W-VF 
;  3989 : #define NV_UINMEM_SET_NOTIFY_PARAMETER                         31:0 /* -W-VF */
NV_UINMEM_SET_NOTIFY_PARAMETER          TEXTEQU         <31:0>
;  -W--V 
;  3990 : #define NV_UINMEM_SET_NOTIFY_PARAMETER_WRITE             0x00000000 /* -W--V */
NV_UINMEM_SET_NOTIFY_PARAMETER_WRITE            EQU             000000000h
;  -W-4R 
;  3991 : #define NV_UINMEM_PITCH                                  0x005C0308 /* -W-4R */
NV_UINMEM_PITCH         EQU             0005c0308h
;  -WXUF 
;  3992 : #define NV_UINMEM_PITCH_VALUE                                  15:0 /* -WXUF */
NV_UINMEM_PITCH_VALUE           TEXTEQU         <15:0>
;  -W-4R 
;  3993 : #define NV_UINMEM_OFFSET                                 0x005C030C /* -W-4R */
NV_UINMEM_OFFSET                EQU             0005c030ch
;  -WIUF 
;  3994 : #define NV_UINMEM_OFFSET_LINADRS                               22:4 /* -WIUF */
NV_UINMEM_OFFSET_LINADRS                TEXTEQU         <22:4>
;  -WI-V 
;  3995 : #define NV_UINMEM_OFFSET_LINADRS_0                       0x00000000 /* -WI-V */
NV_UINMEM_OFFSET_LINADRS_0              EQU             000000000h
;  -W-4R 
;  3996 : #define NV_UINMEM_FORMAT                                 0x005C0300 /* -W-4R */
NV_UINMEM_FORMAT                EQU             0005c0300h
;  -WXUF 
;  3997 : #define NV_UINMEM_FORMAT_VALUE                                 31:0 /* -WXUF */
NV_UINMEM_FORMAT_VALUE          TEXTEQU         <31:0>
;  -W--V 
;  3998 : #define NV_UINMEM_FORMAT_VALUE_LE_Y8_P4                  0x01010000 /* -W--V */
NV_UINMEM_FORMAT_VALUE_LE_Y8_P4         EQU             001010000h
;  -W--V 
;  3999 : #define NV_UINMEM_FORMAT_VALUE_LE_Y16_P2                 0x01010001 /* -W--V */
NV_UINMEM_FORMAT_VALUE_LE_Y16_P2                EQU             001010001h
;  -W--V 
;  4000 : #define NV_UINMEM_FORMAT_VALUE_LE_X1R5G5B5_P2            0x01000000 /* -W--V */
NV_UINMEM_FORMAT_VALUE_LE_X1R5G5B5_P2           EQU             001000000h
;  -W--V 
;  4001 : #define NV_UINMEM_FORMAT_VALUE_LE_X8R8G8B8               0x00000001 /* -W--V */
NV_UINMEM_FORMAT_VALUE_LE_X8R8G8B8              EQU             000000001h
;  dev_framebuffer.ref 
;  RW--D 
;  4002 : /* dev_framebuffer.ref */
;  4003 : #define NV_PFB                                0x00100FFF:0x00100000 /* RW--D */
NV_PFB          TEXTEQU         <0x00100FFF:0x00100000>
;  RW-4R 
;  4004 : #define NV_PFB_BOOT_0                                    0x00100000 /* RW-4R */
NV_PFB_BOOT_0           EQU             000100000h
;  RWIVF 
;  4005 : #define NV_PFB_BOOT_0_RAM_AMOUNT                                1:0 /* RWIVF */
NV_PFB_BOOT_0_RAM_AMOUNT                TEXTEQU         <1:0>
;  RW--V 
;  4006 : #define NV_PFB_BOOT_0_RAM_AMOUNT_1MB                     0x00000000 /* RW--V */
NV_PFB_BOOT_0_RAM_AMOUNT_1MB            EQU             000000000h
;  RW--V 
;  4007 : #define NV_PFB_BOOT_0_RAM_AMOUNT_2MB                     0x00000001 /* RW--V */
NV_PFB_BOOT_0_RAM_AMOUNT_2MB            EQU             000000001h
;  RW--V 
;  4008 : #define NV_PFB_BOOT_0_RAM_AMOUNT_4MB                     0x00000002 /* RW--V */
NV_PFB_BOOT_0_RAM_AMOUNT_4MB            EQU             000000002h
;  RWI-V 
;  4009 : #define NV_PFB_BOOT_0_RAM_AMOUNT_DEFAULT                 0x00000002 /* RWI-V */
NV_PFB_BOOT_0_RAM_AMOUNT_DEFAULT                EQU             000000002h
;  RW-VF 
;  4010 : #define NV_PFB_BOOT_0_RAM_WIDTH_128                             2:2 /* RW-VF */
NV_PFB_BOOT_0_RAM_WIDTH_128             TEXTEQU         <2:2>
;  RW--V 
;  4011 : #define NV_PFB_BOOT_0_RAM_WIDTH_128_OFF                  0x00000000 /* RW--V */
NV_PFB_BOOT_0_RAM_WIDTH_128_OFF         EQU             000000000h
;  RW--V 
;  4012 : #define NV_PFB_BOOT_0_RAM_WIDTH_128_ON                   0x00000001 /* RW--V */
NV_PFB_BOOT_0_RAM_WIDTH_128_ON          EQU             000000001h
;  RW-4R 
;  4013 : #define NV_PFB_DELAY_1                                   0x00100044 /* RW-4R */
NV_PFB_DELAY_1          EQU             000100044h
;  RWIUF 
;  4014 : #define NV_PFB_DELAY_1_WRITE_ENABLE_RISE                        1:0 /* RWIUF */
NV_PFB_DELAY_1_WRITE_ENABLE_RISE                TEXTEQU         <1:0>
;  RWI-V 
;  4015 : #define NV_PFB_DELAY_1_WRITE_ENABLE_RISE_0               0x00000000 /* RWI-V */
NV_PFB_DELAY_1_WRITE_ENABLE_RISE_0              EQU             000000000h
;  RWIUF 
;  4016 : #define NV_PFB_DELAY_1_WRITE_ENABLE_FALL                        5:4 /* RWIUF */
NV_PFB_DELAY_1_WRITE_ENABLE_FALL                TEXTEQU         <5:4>
;  RWI-V 
;  4017 : #define NV_PFB_DELAY_1_WRITE_ENABLE_FALL_0               0x00000000 /* RWI-V */
NV_PFB_DELAY_1_WRITE_ENABLE_FALL_0              EQU             000000000h
;  RWIUF 
;  4018 : #define NV_PFB_DELAY_1_CAS_ENABLE_RISE                          9:8 /* RWIUF */
NV_PFB_DELAY_1_CAS_ENABLE_RISE          TEXTEQU         <9:8>
;  RWI-V 
;  4019 : #define NV_PFB_DELAY_1_CAS_ENABLE_RISE_0                 0x00000000 /* RWI-V */
NV_PFB_DELAY_1_CAS_ENABLE_RISE_0                EQU             000000000h
;  RWIUF 
;  4020 : #define NV_PFB_DELAY_1_CAS_ENABLE_FALL                        13:12 /* RWIUF */
NV_PFB_DELAY_1_CAS_ENABLE_FALL          TEXTEQU         <13:12>
;  RWI-V 
;  4021 : #define NV_PFB_DELAY_1_CAS_ENABLE_FALL_0                 0x00000000 /* RWI-V */
NV_PFB_DELAY_1_CAS_ENABLE_FALL_0                EQU             000000000h
;  RWIUF 
;  4022 : #define NV_PFB_DELAY_1_OUTPUT_DATA                            17:16 /* RWIUF */
NV_PFB_DELAY_1_OUTPUT_DATA              TEXTEQU         <17:16>
;  RWI-V 
;  4023 : #define NV_PFB_DELAY_1_OUTPUT_DATA_0                     0x00000000 /* RWI-V */
NV_PFB_DELAY_1_OUTPUT_DATA_0            EQU             000000000h
;  RWIUF 
;  4024 : #define NV_PFB_DELAY_1_RAS_ENABLE                             21:20 /* RWIUF */
NV_PFB_DELAY_1_RAS_ENABLE               TEXTEQU         <21:20>
;  RWI-V 
;  4025 : #define NV_PFB_DELAY_1_RAS_ENABLE_0                      0x00000000 /* RWI-V */
NV_PFB_DELAY_1_RAS_ENABLE_0             EQU             000000000h
;  RW-4R 
;  4026 : #define NV_PFB_DEBUG_0                                   0x00100080 /* RW-4R */
NV_PFB_DEBUG_0          EQU             000100080h
;  RWIVF 
;  4027 : #define NV_PFB_DEBUG_0_PAGE_MODE                                0:0 /* RWIVF */
NV_PFB_DEBUG_0_PAGE_MODE                TEXTEQU         <0:0>
;  RWI-V 
;  4028 : #define NV_PFB_DEBUG_0_PAGE_MODE_ENABLED                 0x00000000 /* RWI-V */
NV_PFB_DEBUG_0_PAGE_MODE_ENABLED                EQU             000000000h
;  RW--V 
;  4029 : #define NV_PFB_DEBUG_0_PAGE_MODE_DISABLED                0x00000001 /* RW--V */
NV_PFB_DEBUG_0_PAGE_MODE_DISABLED               EQU             000000001h
;  RWIVF 
;  4030 : #define NV_PFB_DEBUG_0_REFRESH                                  4:4 /* RWIVF */
NV_PFB_DEBUG_0_REFRESH          TEXTEQU         <4:4>
;  RWI-V 
;  4031 : #define NV_PFB_DEBUG_0_REFRESH_ENABLED                   0x00000000 /* RWI-V */
NV_PFB_DEBUG_0_REFRESH_ENABLED          EQU             000000000h
;  RW--V 
;  4032 : #define NV_PFB_DEBUG_0_REFRESH_DISABLED                  0x00000001 /* RW--V */
NV_PFB_DEBUG_0_REFRESH_DISABLED         EQU             000000001h
;  RWIVF 
;  4033 : #define NV_PFB_DEBUG_0_REFRESH_COUNTX64                        12:8 /* RWIVF */
NV_PFB_DEBUG_0_REFRESH_COUNTX64         TEXTEQU         <12:8>
;  RWI-V 
;  4034 : #define NV_PFB_DEBUG_0_REFRESH_COUNTX64_DEFAULT          0x00000010 /* RWI-V */
NV_PFB_DEBUG_0_REFRESH_COUNTX64_DEFAULT         EQU             000000010h
;  RWIVF 
;  4035 : #define NV_PFB_DEBUG_0_CASOE                                  20:20 /* RWIVF */
NV_PFB_DEBUG_0_CASOE            TEXTEQU         <20:20>
;  RWI-V 
;  4036 : #define NV_PFB_DEBUG_0_CASOE_ENABLED                     0x00000000 /* RWI-V */
NV_PFB_DEBUG_0_CASOE_ENABLED            EQU             000000000h
;  RW--V 
;  4037 : #define NV_PFB_DEBUG_0_CASOE_DISABLED                    0x00000001 /* RW--V */
NV_PFB_DEBUG_0_CASOE_DISABLED           EQU             000000001h
;  RWIVF 
;  4038 : #define NV_PFB_DEBUG_0_CKE_INVERT                             28:28 /* RWIVF */
NV_PFB_DEBUG_0_CKE_INVERT               TEXTEQU         <28:28>
;  RWIVF 
;  4039 : #define NV_PFB_DEBUG_0_CKE_INVERT_OFF                    0x00000000 /* RWIVF */
NV_PFB_DEBUG_0_CKE_INVERT_OFF           EQU             000000000h
;  RW-VF 
;  4040 : #define NV_PFB_DEBUG_0_CKE_INVERT_ON                     0x00000001 /* RW-VF */
NV_PFB_DEBUG_0_CKE_INVERT_ON            EQU             000000001h
;  RWIVF 
;  4041 : #define NV_PFB_DEBUG_0_CKE_ALWAYSON                           29:29 /* RWIVF */
NV_PFB_DEBUG_0_CKE_ALWAYSON             TEXTEQU         <29:29>
;  RW-VF 
;  4042 : #define NV_PFB_DEBUG_0_CKE_ALWAYSON_OFF                  0x00000000 /* RW-VF */
NV_PFB_DEBUG_0_CKE_ALWAYSON_OFF         EQU             000000000h
;  RWIVF 
;  4043 : #define NV_PFB_DEBUG_0_CKE_ALWAYSON_ON                   0x00000001 /* RWIVF */
NV_PFB_DEBUG_0_CKE_ALWAYSON_ON          EQU             000000001h
;  RWIVF 
;  4044 : #define NV_PFB_DEBUG_0_SAVE_POWER                             30:30 /* RWIVF */
NV_PFB_DEBUG_0_SAVE_POWER               TEXTEQU         <30:30>
;  RWIVF 
;  4045 : #define NV_PFB_DEBUG_0_SAVE_POWER_ON                     0x00000000 /* RWIVF */
NV_PFB_DEBUG_0_SAVE_POWER_ON            EQU             000000000h
;  RW-VF 
;  4046 : #define NV_PFB_DEBUG_0_SAVE_POWER_OFF                    0x00000001 /* RW-VF */
NV_PFB_DEBUG_0_SAVE_POWER_OFF           EQU             000000001h
;  RW-4R 
;  4047 : #define NV_PFB_GREEN_0                                   0x001000C0 /* RW-4R */
NV_PFB_GREEN_0          EQU             0001000c0h
;  RWIVF 
;  4048 : #define NV_PFB_GREEN_0_LEVEL                                    1:0 /* RWIVF */
NV_PFB_GREEN_0_LEVEL            TEXTEQU         <1:0>
;  RW--V 
;  4049 : #define NV_PFB_GREEN_0_LEVEL_VIDEO_ENABLED               0x00000000 /* RW--V */
NV_PFB_GREEN_0_LEVEL_VIDEO_ENABLED              EQU             000000000h
;  RW--V 
;  4050 : #define NV_PFB_GREEN_0_LEVEL_VIDEO_DISABLED              0x00000001 /* RW--V */
NV_PFB_GREEN_0_LEVEL_VIDEO_DISABLED             EQU             000000001h
;  RW--V 
;  4051 : #define NV_PFB_GREEN_0_LEVEL_TIMING_DISABLED             0x00000002 /* RW--V */
NV_PFB_GREEN_0_LEVEL_TIMING_DISABLED            EQU             000000002h
;  RWI-V 
;  4052 : #define NV_PFB_GREEN_0_LEVEL_MEMORY_DISABLED             0x00000003 /* RWI-V */
NV_PFB_GREEN_0_LEVEL_MEMORY_DISABLED            EQU             000000003h
;  RW-4R 
;  4053 : #define NV_PFB_CONFIG_0                                  0x00100200 /* RW-4R */
NV_PFB_CONFIG_0         EQU             000100200h
;  RWIVF 
;  4054 : #define NV_PFB_CONFIG_0_RESOLUTION                              5:0 /* RWIVF */
NV_PFB_CONFIG_0_RESOLUTION              TEXTEQU         <5:0>
;  RW--V 
;  4055 : #define NV_PFB_CONFIG_0_RESOLUTION_320_PIXELS            0x0000000a /* RW--V */
NV_PFB_CONFIG_0_RESOLUTION_320_PIXELS           EQU             00000000ah
;  RW--V 
;  4056 : #define NV_PFB_CONFIG_0_RESOLUTION_400_PIXELS            0x0000000d /* RW--V */
NV_PFB_CONFIG_0_RESOLUTION_400_PIXELS           EQU             00000000dh
;  RW--V 
;  4057 : #define NV_PFB_CONFIG_0_RESOLUTION_480_PIXELS            0x0000000f /* RW--V */
NV_PFB_CONFIG_0_RESOLUTION_480_PIXELS           EQU             00000000fh
;  RW--V 
;  4058 : #define NV_PFB_CONFIG_0_RESOLUTION_512_PIXELS            0x00000010 /* RW--V */
NV_PFB_CONFIG_0_RESOLUTION_512_PIXELS           EQU             000000010h
;  RW--V 
;  4059 : #define NV_PFB_CONFIG_0_RESOLUTION_640_PIXELS            0x00000014 /* RW--V */
NV_PFB_CONFIG_0_RESOLUTION_640_PIXELS           EQU             000000014h
;  RW--V 
;  4060 : #define NV_PFB_CONFIG_0_RESOLUTION_800_PIXELS            0x00000019 /* RW--V */
NV_PFB_CONFIG_0_RESOLUTION_800_PIXELS           EQU             000000019h
;  RW--V 
;  4061 : #define NV_PFB_CONFIG_0_RESOLUTION_960_PIXELS            0x0000001e /* RW--V */
NV_PFB_CONFIG_0_RESOLUTION_960_PIXELS           EQU             00000001eh
;  RW--V 
;  4062 : #define NV_PFB_CONFIG_0_RESOLUTION_1024_PIXELS           0x00000020 /* RW--V */
NV_PFB_CONFIG_0_RESOLUTION_1024_PIXELS          EQU             000000020h
;  RW--V 
;  4063 : #define NV_PFB_CONFIG_0_RESOLUTION_1152_PIXELS           0x00000024 /* RW--V */
NV_PFB_CONFIG_0_RESOLUTION_1152_PIXELS          EQU             000000024h
;  RW--V 
;  4064 : #define NV_PFB_CONFIG_0_RESOLUTION_1280_PIXELS           0x00000028 /* RW--V */
NV_PFB_CONFIG_0_RESOLUTION_1280_PIXELS          EQU             000000028h
;  RW--V 
;  4065 : #define NV_PFB_CONFIG_0_RESOLUTION_1600_PIXELS           0x00000032 /* RW--V */
NV_PFB_CONFIG_0_RESOLUTION_1600_PIXELS          EQU             000000032h
;  RWI-V 
;  4066 : #define NV_PFB_CONFIG_0_RESOLUTION_DEFAULT               0x00000014 /* RWI-V */
NV_PFB_CONFIG_0_RESOLUTION_DEFAULT              EQU             000000014h
;  RWIVF 
;  4067 : #define NV_PFB_CONFIG_0_PIXEL_DEPTH                             9:8 /* RWIVF */
NV_PFB_CONFIG_0_PIXEL_DEPTH             TEXTEQU         <9:8>
;  RW--V 
;  4068 : #define NV_PFB_CONFIG_0_PIXEL_DEPTH_8_BITS               0x00000001 /* RW--V */
NV_PFB_CONFIG_0_PIXEL_DEPTH_8_BITS              EQU             000000001h
;  RW--V 
;  4069 : #define NV_PFB_CONFIG_0_PIXEL_DEPTH_16_BITS              0x00000002 /* RW--V */
NV_PFB_CONFIG_0_PIXEL_DEPTH_16_BITS             EQU             000000002h
;  RW--V 
;  4070 : #define NV_PFB_CONFIG_0_PIXEL_DEPTH_32_BITS              0x00000003 /* RW--V */
NV_PFB_CONFIG_0_PIXEL_DEPTH_32_BITS             EQU             000000003h
;  RWI-V 
;  4071 : #define NV_PFB_CONFIG_0_PIXEL_DEPTH_DEFAULT              0x00000001 /* RWI-V */
NV_PFB_CONFIG_0_PIXEL_DEPTH_DEFAULT             EQU             000000001h
;  RWIVF 
;  4072 : #define NV_PFB_CONFIG_0_TILING                                12:12 /* RWIVF */
NV_PFB_CONFIG_0_TILING          TEXTEQU         <12:12>
;  RW--V 
;  4073 : #define NV_PFB_CONFIG_0_TILING_ENABLED                   0x00000000 /* RW--V */
NV_PFB_CONFIG_0_TILING_ENABLED          EQU             000000000h
;  RWI-V 
;  4074 : #define NV_PFB_CONFIG_0_TILING_DISABLED                  0x00000001 /* RWI-V */
NV_PFB_CONFIG_0_TILING_DISABLED         EQU             000000001h
;  RWIVF 
;  4075 : #define NV_PFB_CONFIG_0_TILING_DEBUG                          23:13 /* RWIVF */
NV_PFB_CONFIG_0_TILING_DEBUG            TEXTEQU         <23:13>
;  RW--V 
;  4076 : #define NV_PFB_CONFIG_0_TILING_DEBUG_DISABLED            0x00000000 /* RW--V */
NV_PFB_CONFIG_0_TILING_DEBUG_DISABLED           EQU             000000000h
;  RW-4R 
;  4077 : #define NV_PFB_CONFIG_1                                  0x00100204 /* RW-4R */
NV_PFB_CONFIG_1         EQU             000100204h
;  RWIVF 
;  4078 : #define NV_PFB_CONFIG_1_CAS_LATENCY                             2:0 /* RWIVF */
NV_PFB_CONFIG_1_CAS_LATENCY             TEXTEQU         <2:0>
;  RWI-V 
;  4079 : #define NV_PFB_CONFIG_1_CAS_LATENCY_3                    0x00000003 /* RWI-V */
NV_PFB_CONFIG_1_CAS_LATENCY_3           EQU             000000003h
;  RWIVF 
;  4080 : #define NV_PFB_CONFIG_1_SGRAM100                                3:3 /* RWIVF */
NV_PFB_CONFIG_1_SGRAM100                TEXTEQU         <3:3>
;  RWI-V 
;  4081 : #define NV_PFB_CONFIG_1_SGRAM100_ENABLED                 0x00000000 /* RWI-V */
NV_PFB_CONFIG_1_SGRAM100_ENABLED                EQU             000000000h
;  RW--V 
;  4082 : #define NV_PFB_CONFIG_1_SGRAM100_DISABLED                0x00000001 /* RW--V */
NV_PFB_CONFIG_1_SGRAM100_DISABLED               EQU             000000001h
;  RWIUF 
;  4083 : #define NV_PFB_CONFIG_1_RAS_RAS                                 7:4 /* RWIUF */
NV_PFB_CONFIG_1_RAS_RAS         TEXTEQU         <7:4>
;  RWI-V 
;  4084 : #define NV_PFB_CONFIG_1_RAS_RAS_DEFAULT                  0x00000009 /* RWI-V */
NV_PFB_CONFIG_1_RAS_RAS_DEFAULT         EQU             000000009h
;  RW--V 
;  4085 : #define NV_PFB_CONFIG_1_RAS_RAS_SG100                    0x00000004 /* RW--V */
NV_PFB_CONFIG_1_RAS_RAS_SG100           EQU             000000004h
;  RWIUF 
;  4086 : #define NV_PFB_CONFIG_1_RAS_PCHG                               10:8 /* RWIUF */
NV_PFB_CONFIG_1_RAS_PCHG                TEXTEQU         <10:8>
;  RWI-V 
;  4087 : #define NV_PFB_CONFIG_1_RAS_PCHG_DEFAULT                 0x00000002 /* RWI-V */
NV_PFB_CONFIG_1_RAS_PCHG_DEFAULT                EQU             000000002h
;  RW--V 
;  4088 : #define NV_PFB_CONFIG_1_RAS_PCHG_SG100                   0x00000001 /* RW--V */
NV_PFB_CONFIG_1_RAS_PCHG_SG100          EQU             000000001h
;  RWIUF 
;  4089 : #define NV_PFB_CONFIG_1_RAS_LOW                               14:12 /* RWIUF */
NV_PFB_CONFIG_1_RAS_LOW         TEXTEQU         <14:12>
;  RWI-V 
;  4090 : #define NV_PFB_CONFIG_1_RAS_LOW_DEFAULT                  0x00000006 /* RWI-V */
NV_PFB_CONFIG_1_RAS_LOW_DEFAULT         EQU             000000006h
;  RW--V 
;  4091 : #define NV_PFB_CONFIG_1_RAS_LOW_SG100                    0x00000003 /* RW--V */
NV_PFB_CONFIG_1_RAS_LOW_SG100           EQU             000000003h
;  RWIUF 
;  4092 : #define NV_PFB_CONFIG_1_MRS_TO_RAS                            18:16 /* RWIUF */
NV_PFB_CONFIG_1_MRS_TO_RAS              TEXTEQU         <18:16>
;  RWI-V 
;  4093 : #define NV_PFB_CONFIG_1_MRS_TO_RAS_DEFAULT               0x00000001 /* RWI-V */
NV_PFB_CONFIG_1_MRS_TO_RAS_DEFAULT              EQU             000000001h
;  RW--V 
;  4094 : #define NV_PFB_CONFIG_1_MRS_TO_RAS_SG100                 0x00000000 /* RW--V */
NV_PFB_CONFIG_1_MRS_TO_RAS_SG100                EQU             000000000h
;  RWIUF 
;  4095 : #define NV_PFB_CONFIG_1_WRITE_TO_READ                         22:20 /* RWIUF */
NV_PFB_CONFIG_1_WRITE_TO_READ           TEXTEQU         <22:20>
;  RWI-V 
;  4096 : #define NV_PFB_CONFIG_1_WRITE_TO_READ_DEFAULT            0x00000000 /* RWI-V */
NV_PFB_CONFIG_1_WRITE_TO_READ_DEFAULT           EQU             000000000h
;  RW--V 
;  4097 : #define NV_PFB_CONFIG_1_WRITE_TO_READ_SG100              0x00000000 /* RW--V */
NV_PFB_CONFIG_1_WRITE_TO_READ_SG100             EQU             000000000h
;  RWIUF 
;  4098 : #define NV_PFB_CONFIG_1_RAS_TO_CAS_M1                         26:24 /* RWIUF */
NV_PFB_CONFIG_1_RAS_TO_CAS_M1           TEXTEQU         <26:24>
;  RWI-V 
;  4099 : #define NV_PFB_CONFIG_1_RAS_TO_CAS_M1_DEFAULT            0x00000001 /* RWI-V */
NV_PFB_CONFIG_1_RAS_TO_CAS_M1_DEFAULT           EQU             000000001h
;  RWI-V 
;  4100 : #define NV_PFB_CONFIG_1_RAS_TO_CAS_M1_SG100              0x00000000 /* RWI-V */
NV_PFB_CONFIG_1_RAS_TO_CAS_M1_SG100             EQU             000000000h
;  RWIUF 
;  4101 : #define NV_PFB_CONFIG_1_READ_TO_WRITE                         30:28 /* RWIUF */
NV_PFB_CONFIG_1_READ_TO_WRITE           TEXTEQU         <30:28>
;  RWI-V 
;  4102 : #define NV_PFB_CONFIG_1_READ_TO_WRITE_DEFAULT            0x00000004 /* RWI-V */ 
NV_PFB_CONFIG_1_READ_TO_WRITE_DEFAULT           EQU             000000004h
;  RWI-V 
;  4103 : #define NV_PFB_CONFIG_1_READ_TO_WRITE_SG100              0x00000002 /* RWI-V */ 
NV_PFB_CONFIG_1_READ_TO_WRITE_SG100             EQU             000000002h
;  RWIUF 
;  4104 : #define NV_PFB_CONFIG_1_ED_RAC1                                 0:0 /* RWIUF */
NV_PFB_CONFIG_1_ED_RAC1         TEXTEQU         <0:0>
;  RWI-V 
;  4105 : #define NV_PFB_CONFIG_1_ED_RAC1_DEFAULT                  0x00000000 /* RWI-V */
NV_PFB_CONFIG_1_ED_RAC1_DEFAULT         EQU             000000000h
;  RWIUF 
;  4106 : #define NV_PFB_CONFIG_1_ED_RWT1                                 1:1 /* RWIUF */
NV_PFB_CONFIG_1_ED_RWT1         TEXTEQU         <1:1>
;  RWI-V 
;  4107 : #define NV_PFB_CONFIG_1_ED_RWT1_DEFAULT                  0x00000001 /* RWI-V */
NV_PFB_CONFIG_1_ED_RWT1_DEFAULT         EQU             000000001h
;  RWIUF 
;  4108 : #define NV_PFB_CONFIG_1_ED_RPC1                                 2:2 /* RWIUF */
NV_PFB_CONFIG_1_ED_RPC1         TEXTEQU         <2:2>
;  RWI-V 
;  4109 : #define NV_PFB_CONFIG_1_ED_RPC1_DEFAULT                  0x00000001 /* RWI-V */
NV_PFB_CONFIG_1_ED_RPC1_DEFAULT         EQU             000000001h
;  RWIUF 
;  4110 : #define NV_PFB_CONFIG_1_ED_PMAW                                 4:4 /* RWIUF */
NV_PFB_CONFIG_1_ED_PMAW         TEXTEQU         <4:4>
;  RWI-V 
;  4111 : #define NV_PFB_CONFIG_1_ED_PMAW_DEFAULT                  0x00000000 /* RWI-V */
NV_PFB_CONFIG_1_ED_PMAW_DEFAULT         EQU             000000000h
;  RWIUF 
;  4112 : #define NV_PFB_CONFIG_1_ED_PAGE                                 5:5 /* RWIUF */
NV_PFB_CONFIG_1_ED_PAGE         TEXTEQU         <5:5>
;  RWI-V 
;  4113 : #define NV_PFB_CONFIG_1_ED_PAGE_DEFAULT                  0x00000000 /* RWI-V */
NV_PFB_CONFIG_1_ED_PAGE_DEFAULT         EQU             000000000h
;  RWIUF 
;  4114 : #define NV_PFB_CONFIG_1_ED_RAC0_0                               8:8 /* RWIUF */
NV_PFB_CONFIG_1_ED_RAC0_0               TEXTEQU         <8:8>
;  RWI-V 
;  4115 : #define NV_PFB_CONFIG_1_ED_RAC0_0_DEFAULT                0x00000000 /* RWI-V */
NV_PFB_CONFIG_1_ED_RAC0_0_DEFAULT               EQU             000000000h
;  RWIUF 
;  4116 : #define NV_PFB_CONFIG_1_ED_RAC0_1                               9:9 /* RWIUF */
NV_PFB_CONFIG_1_ED_RAC0_1               TEXTEQU         <9:9>
;  RWI-V 
;  4117 : #define NV_PFB_CONFIG_1_ED_RAC0_1_DEFAULT                0x00000000 /* RWI-V */
NV_PFB_CONFIG_1_ED_RAC0_1_DEFAULT               EQU             000000000h
;  RWIUF 
;  4118 : #define NV_PFB_CONFIG_1_ED_RAC1_0                             10:10 /* RWIUF */
NV_PFB_CONFIG_1_ED_RAC1_0               TEXTEQU         <10:10>
;  RWI-V 
;  4119 : #define NV_PFB_CONFIG_1_ED_RAC1_0_DEFAULT                0x00000001 /* RWI-V */
NV_PFB_CONFIG_1_ED_RAC1_0_DEFAULT               EQU             000000001h
;  RWIUF 
;  4120 : #define NV_PFB_CONFIG_1_ED_RAC1_1                             12:12 /* RWIUF */
NV_PFB_CONFIG_1_ED_RAC1_1               TEXTEQU         <12:12>
;  RWI-V 
;  4121 : #define NV_PFB_CONFIG_1_ED_RAC1_1_DEFAULT                0x00000000 /* RWI-V */
NV_PFB_CONFIG_1_ED_RAC1_1_DEFAULT               EQU             000000000h
;  RWIUF 
;  4122 : #define NV_PFB_CONFIG_1_ED_RAD0_0                             13:13 /* RWIUF */
NV_PFB_CONFIG_1_ED_RAD0_0               TEXTEQU         <13:13>
;  RWI-V 
;  4123 : #define NV_PFB_CONFIG_1_ED_RAD0_0_DEFAULT                0x00000001 /* RWI-V */
NV_PFB_CONFIG_1_ED_RAD0_0_DEFAULT               EQU             000000001h
;  RWIUF 
;  4124 : #define NV_PFB_CONFIG_1_ED_RAD0_1                             14:14 /* RWIUF */
NV_PFB_CONFIG_1_ED_RAD0_1               TEXTEQU         <14:14>
;  RWI-V 
;  4125 : #define NV_PFB_CONFIG_1_ED_RAD0_1_DEFAULT                0x00000001 /* RWI-V */
NV_PFB_CONFIG_1_ED_RAD0_1_DEFAULT               EQU             000000001h
;  RWIUF 
;  4126 : #define NV_PFB_CONFIG_1_ED_RPC0_0                             16:16 /* RWIUF */
NV_PFB_CONFIG_1_ED_RPC0_0               TEXTEQU         <16:16>
;  RWI-V 
;  4127 : #define NV_PFB_CONFIG_1_ED_RPC0_0_DEFAULT                0x00000000 /* RWI-V */
NV_PFB_CONFIG_1_ED_RPC0_0_DEFAULT               EQU             000000000h
;  RWIUF 
;  4128 : #define NV_PFB_CONFIG_1_ED_RPC0_1                             17:17 /* RWIUF */
NV_PFB_CONFIG_1_ED_RPC0_1               TEXTEQU         <17:17>
;  RWI-V 
;  4129 : #define NV_PFB_CONFIG_1_ED_RPC0_1_DEFAULT                0x00000000 /* RWI-V */
NV_PFB_CONFIG_1_ED_RPC0_1_DEFAULT               EQU             000000000h
;  RWIUF 
;  4130 : #define NV_PFB_CONFIG_1_ED_RPC1_0                             18:18 /* RWIUF */
NV_PFB_CONFIG_1_ED_RPC1_0               TEXTEQU         <18:18>
;  RWI-V 
;  4131 : #define NV_PFB_CONFIG_1_ED_RPC1_0_DEFAULT                0x00000001 /* RWI-V */
NV_PFB_CONFIG_1_ED_RPC1_0_DEFAULT               EQU             000000001h
;  RWIUF 
;  4132 : #define NV_PFB_CONFIG_1_ED_RPC1_1                             20:20 /* RWIUF */
NV_PFB_CONFIG_1_ED_RPC1_1               TEXTEQU         <20:20>
;  RWI-V 
;  4133 : #define NV_PFB_CONFIG_1_ED_RPC1_1_DEFAULT                0x00000001 /* RWI-V */
NV_PFB_CONFIG_1_ED_RPC1_1_DEFAULT               EQU             000000001h
;  RW-4R 
;  4134 : #define NV_PFB_RTL                                       0x00100300 /* RW-4R */
NV_PFB_RTL              EQU             000100300h
;  RWIUF 
;  4135 : #define NV_PFB_RTL_S                                            1:0 /* RWIUF */
NV_PFB_RTL_S            TEXTEQU         <1:0>
;  RWI-V 
;  4136 : #define NV_PFB_RTL_S_DEFAULT                             0x00000002 /* RWI-V */
NV_PFB_RTL_S_DEFAULT            EQU             000000002h
;  RWIUF 
;  4137 : #define NV_PFB_RTL_V                                            5:4 /* RWIUF */
NV_PFB_RTL_V            TEXTEQU         <5:4>
;  RWI-V 
;  4138 : #define NV_PFB_RTL_V_DEFAULT                             0x00000002 /* RWI-V */
NV_PFB_RTL_V_DEFAULT            EQU             000000002h
;  RWIUF 
;  4139 : #define NV_PFB_RTL_M                                            9:8 /* RWIUF */
NV_PFB_RTL_M            TEXTEQU         <9:8>
;  RWI-V 
;  4140 : #define NV_PFB_RTL_M_DEFAULT                             0x00000002 /* RWI-V */
NV_PFB_RTL_M_DEFAULT            EQU             000000002h
;  RWIUF 
;  4141 : #define NV_PFB_RTL_H                                          13:12 /* RWIUF */
NV_PFB_RTL_H            TEXTEQU         <13:12>
;  RWI-V 
;  4142 : #define NV_PFB_RTL_H_DEFAULT                             0x00000001 /* RWI-V */
NV_PFB_RTL_H_DEFAULT            EQU             000000001h
;  RWIUF 
;  4143 : #define NV_PFB_RTL_A                                          17:16 /* RWIUF */
NV_PFB_RTL_A            TEXTEQU         <17:16>
;  RWI-V 
;  4144 : #define NV_PFB_RTL_A_DEFAULT                             0x00000001 /* RWI-V */
NV_PFB_RTL_A_DEFAULT            EQU             000000001h
;  RWIUF 
;  4145 : #define NV_PFB_RTL_G                                          21:20 /* RWIUF */
NV_PFB_RTL_G            TEXTEQU         <21:20>
;  RWI-V 
;  4146 : #define NV_PFB_RTL_G_DEFAULT                             0x00000001 /* RWI-V */
NV_PFB_RTL_G_DEFAULT            EQU             000000001h
;  RWIUF 
;  4147 : #define NV_PFB_RTL_ARB_GR_HI_PRIOR                            24:24 /* RWIUF */
NV_PFB_RTL_ARB_GR_HI_PRIOR              TEXTEQU         <24:24>
;  RWI-V 
;  4148 : #define NV_PFB_RTL_ARB_GR_HI_PRIOR_DEFAULT               0x00000000 /* RWI-V */
NV_PFB_RTL_ARB_GR_HI_PRIOR_DEFAULT              EQU             000000000h
;  RWIUF 
;  4149 : #define NV_PFB_RTL_ARB_MEDIA_HI_PRIOR                         28:28 /* RWIUF */
NV_PFB_RTL_ARB_MEDIA_HI_PRIOR           TEXTEQU         <28:28>
;  RWI-V 
;  4150 : #define NV_PFB_RTL_ARB_MEDIA_HI_PRIOR_DEFAULT            0x00000000 /* RWI-V */
NV_PFB_RTL_ARB_MEDIA_HI_PRIOR_DEFAULT           EQU             000000000h
;  dev_framebuffer.ref 
;  RW--M 
;  4151 : /* dev_framebuffer.ref */
;  4152 : #define NV_PNVM                               0x01BFFFFF:0x01800000 /* RW--M */
NV_PNVM         TEXTEQU         <0x01BFFFFF:0x01800000>
;  RW-4A 
;        
;  4153 : #define NV_PNVM_DATA032(i)                       (0x01800000+(i)*4) /* RW-4A */
;  4154 : #define NV_PNVM_DATA032__SIZE_1                              524288 /*       */
NV_PNVM_DATA032__SIZE_1         EQU             524288t
;  RWXVF 
;  4155 : #define NV_PNVM_DATA032_VALUE                                  31:0 /* RWXVF */
NV_PNVM_DATA032_VALUE           TEXTEQU         <31:0>
;  RW-3A 
;        
;  4156 : #define NV_PNVM_DATA024(i)           (0x01800000+((i)/2)*4+((i)%2)) /* RW-3A */
;  4157 : #define NV_PNVM_DATA024__SIZE_1                             1048576 /*       */
NV_PNVM_DATA024__SIZE_1         EQU             1048576t
;  RWXVF 
;  4158 : #define NV_PNVM_DATA024_VALUE                                  23:0 /* RWXVF */
NV_PNVM_DATA024_VALUE           TEXTEQU         <23:0>
;  RW-2A 
;        
;  4159 : #define NV_PNVM_DATA016(i)           (0x01800000+((i)/3)*4+((i)%3)) /* RW-2A */
;  4160 : #define NV_PNVM_DATA016__SIZE_1                             1572864 /*       */
NV_PNVM_DATA016__SIZE_1         EQU             1572864t
;  RWXVF 
;  4161 : #define NV_PNVM_DATA016_VALUE                                  15:0 /* RWXVF */
NV_PNVM_DATA016_VALUE           TEXTEQU         <15:0>
;  RW-1A 
;        
;  4162 : #define NV_PNVM_DATA008(i)                         (0x01800000+(i)) /* RW-1A */
;  4163 : #define NV_PNVM_DATA008__SIZE_1                             2097152 /*       */
NV_PNVM_DATA008__SIZE_1         EQU             2097152t
;  RWXVF 
;  4164 : #define NV_PNVM_DATA008_VALUE                                   7:0 /* RWXVF */
NV_PNVM_DATA008_VALUE           TEXTEQU         <7:0>
;  dev_framebuffer.ref 
;  RW--M 
;  4165 : /* dev_framebuffer.ref */
;  4166 : #define NV_PRAMIN                             0x01FFFFFF:0x01C00000 /* RW--M */
NV_PRAMIN               TEXTEQU         <0x01FFFFFF:0x01C00000>
;  RW-4A 
;        
;  4167 : #define NV_PRAMIN_DATA032(i)                       (0x01C00000+(i)*4) /* RW-4A */
;  4168 : #define NV_PRAMIN_DATA032__SIZE_1                              524288 /*       */
NV_PRAMIN_DATA032__SIZE_1               EQU             524288t
;  RWXVF 
;  4169 : #define NV_PRAMIN_DATA032_VALUE                                  31:0 /* RWXVF */
NV_PRAMIN_DATA032_VALUE         TEXTEQU         <31:0>
;  RW-3A 
;        
;  4170 : #define NV_PRAMIN_DATA024(i)           (0x01C00000+((i)/2)*4+((i)%2)) /* RW-3A */
;  4171 : #define NV_PRAMIN_DATA024__SIZE_1                             1048576 /*       */
NV_PRAMIN_DATA024__SIZE_1               EQU             1048576t
;  RWXVF 
;  4172 : #define NV_PRAMIN_DATA024_VALUE                                  23:0 /* RWXVF */
NV_PRAMIN_DATA024_VALUE         TEXTEQU         <23:0>
;  RW-2A 
;        
;  4173 : #define NV_PRAMIN_DATA016(i)           (0x01C00000+((i)/3)*4+((i)%3)) /* RW-2A */
;  4174 : #define NV_PRAMIN_DATA016__SIZE_1                             1572864 /*       */
NV_PRAMIN_DATA016__SIZE_1               EQU             1572864t
;  RWXVF 
;  4175 : #define NV_PRAMIN_DATA016_VALUE                                  15:0 /* RWXVF */
NV_PRAMIN_DATA016_VALUE         TEXTEQU         <15:0>
;  RW-1A 
;        
;  4176 : #define NV_PRAMIN_DATA008(i)                         (0x01C00000+(i)) /* RW-1A */
;  4177 : #define NV_PRAMIN_DATA008__SIZE_1                             2097152 /*       */
NV_PRAMIN_DATA008__SIZE_1               EQU             2097152t
;  RWXVF 
;  4178 : #define NV_PRAMIN_DATA008_VALUE                                   7:0 /* RWXVF */
NV_PRAMIN_DATA008_VALUE         TEXTEQU         <7:0>
;  dev_ram.ref 
;  RW--M 
;        
;  4179 : /* dev_ram.ref */
;  4180 : #define NV_PRAMIN                             0x01FFFFFF:0x01C00000 /* RW--M */
;  4181 : #define NV_PRAMIN_CONTEXT_0                   ( 0*32+31):( 0*32+ 0) /*       */
NV_PRAMIN_CONTEXT_0             TEXTEQU         <( 0*32+31):( 0*32+ 0)>
;        
;  4182 : #define NV_PRAMIN_CONTEXT_1                   ( 1*32+31):( 1*32+ 0) /*       */
NV_PRAMIN_CONTEXT_1             TEXTEQU         <( 1*32+31):( 1*32+ 0)>
;        
;  4183 : #define NV_PRAMIN_CONTEXT_2                   ( 2*32+31):( 2*32+ 0) /*       */
NV_PRAMIN_CONTEXT_2             TEXTEQU         <( 2*32+31):( 2*32+ 0)>
;        
;  4184 : #define NV_PRAMIN_CONTEXT_3                   ( 3*32+31):( 3*32+ 0) /*       */
NV_PRAMIN_CONTEXT_3             TEXTEQU         <( 3*32+31):( 3*32+ 0)>
;  RW--M 
;  4185 : #define NV_PRAMIN_RAMHT_0                     0x01400FFF:0x01400000 /* RW--M */
NV_PRAMIN_RAMHT_0               TEXTEQU         <0x01400FFF:0x01400000>
;  RW--M 
;     9 : #define NV_PRAMIN_RAMAU_0                     0x01401BFF:0x01401000 /* RW--M */
NV_PRAMIN_RAMAU_0               TEXTEQU         <0x01401BFF:0x01401000>
;  RW--M 
;    10 : #define NV_PRAMIN_RAMFC_0                     0x01401DFF:0x01401C00 /* RW--M */
NV_PRAMIN_RAMFC_0               TEXTEQU         <0x01401DFF:0x01401C00>
;  RW--M 
;    11 : #define NV_PRAMIN_RAMRO_0                     0x01401FFF:0x01401E00 /* RW--M */
NV_PRAMIN_RAMRO_0               TEXTEQU         <0x01401FFF:0x01401E00>
;  RW--M 
;    12 : #define NV_PRAMIN_RAMRM_0                     0x01402FFF:0x01402000 /* RW--M */
NV_PRAMIN_RAMRM_0               TEXTEQU         <0x01402FFF:0x01402000>
;  dev_framebuffer.ref 
;  RW--D 
;    13 : /* dev_framebuffer.ref */
;    14 : #define NV_PDFB                               0x017FFFFF:0x01000000 /* RW--D */
NV_PDFB         TEXTEQU         <0x017FFFFF:0x01000000>
;  RW-4A 
;        
;    15 : #define NV_PDFB_DATA032(i)                       (0x01000000+(i)*4) /* RW-4A */
;    16 : #define NV_PDFB_DATA032__SIZE_1                              524288 /*       */
NV_PDFB_DATA032__SIZE_1         EQU             524288t
;  RWXVF 
;    17 : #define NV_PDFB_DATA032_VALUE                                  31:0 /* RWXVF */
NV_PDFB_DATA032_VALUE           TEXTEQU         <31:0>
;  RW-3A 
;        
;    18 : #define NV_PDFB_DATA024(i)           (0x01000000+((i)/2)*4+((i)%2)) /* RW-3A */
;    19 : #define NV_PDFB_DATA024__SIZE_1                             1048576 /*       */
NV_PDFB_DATA024__SIZE_1         EQU             1048576t
;  RWXVF 
;    20 : #define NV_PDFB_DATA024_VALUE                                  23:0 /* RWXVF */
NV_PDFB_DATA024_VALUE           TEXTEQU         <23:0>
;  RW-2A 
;        
;    21 : #define NV_PDFB_DATA016(i)           (0x01000000+((i)/3)*4+((i)%3)) /* RW-2A */
;    22 : #define NV_PDFB_DATA016__SIZE_1                             1572864 /*       */
NV_PDFB_DATA016__SIZE_1         EQU             1572864t
;  RWXVF 
;    23 : #define NV_PDFB_DATA016_VALUE                                  15:0 /* RWXVF */
NV_PDFB_DATA016_VALUE           TEXTEQU         <15:0>
;  RW-1A 
;        
;    24 : #define NV_PDFB_DATA008(i)                         (0x01000000+(i)) /* RW-1A */
;    25 : #define NV_PDFB_DATA008__SIZE_1                             2097152 /*       */
NV_PDFB_DATA008__SIZE_1         EQU             2097152t
;  RWXVF 
;    26 : #define NV_PDFB_DATA008_VALUE                                   7:0 /* RWXVF */
NV_PDFB_DATA008_VALUE           TEXTEQU         <7:0>
;  dev_ram.ref 
;  RW--D 
;    27 : /* dev_ram.ref */
;    28 : #define NV_PRAM                               0x00006FFF:0x00006000 /* RW--D */
NV_PRAM         TEXTEQU         <0x00006FFF:0x00006000>
;  dev_ext_devices.ref 
;  RW--D 
;    29 : /* dev_ext_devices.ref */
;    30 : #define NV_PEXTDEV                            0x00101FFF:0x00101000 /* RW--D */
NV_PEXTDEV              TEXTEQU         <0x00101FFF:0x00101000>
;  R--4R 
;    31 : #define NV_PEXTDEV_BOOT_0                                0x00101000 /* R--4R */
NV_PEXTDEV_BOOT_0               EQU             000101000h
;  R-XVF 
;    32 : #define NV_PEXTDEV_BOOT_0_STRAP_BUS_SPEED                       0:0 /* R-XVF */
NV_PEXTDEV_BOOT_0_STRAP_BUS_SPEED               TEXTEQU         <0:0>
;  R---V 
;    33 : #define NV_PEXTDEV_BOOT_0_STRAP_BUS_SPEED_33MHZ          0x00000000 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_BUS_SPEED_33MHZ         EQU             000000000h
;  R---V 
;    34 : #define NV_PEXTDEV_BOOT_0_STRAP_BUS_SPEED_66MHZ          0x00000001 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_BUS_SPEED_66MHZ         EQU             000000001h
;  R-XVF 
;    35 : #define NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR                      1:1 /* R-XVF */
NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR              TEXTEQU         <1:1>
;  R---V 
;    36 : #define NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR_NO_BIOS       0x00000000 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR_NO_BIOS              EQU             000000000h
;  R---V 
;    37 : #define NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR_BIOS          0x00000001 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR_BIOS         EQU             000000001h
;  R-XVF 
;    38 : #define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE                        3:2 /* R-XVF */
NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE                TEXTEQU         <3:2>
;  R---V 
;    39 : #define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_EDO             0x00000000 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_EDO            EQU             000000000h
;  R---V 
;    40 : #define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_SGRAM_256K      0x00000001 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_SGRAM_256K             EQU             000000001h
;  R---V 
;    41 : #define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_RESERVED        0x00000002 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_RESERVED               EQU             000000002h
;  R---V 
;    42 : #define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_SGRAM_512K      0x00000003 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_SGRAM_512K             EQU             000000003h
;  R-XVF 
;    43 : #define NV_PEXTDEV_BOOT_0_STRAP_RAM_WIDTH                       4:4 /* R-XVF */
NV_PEXTDEV_BOOT_0_STRAP_RAM_WIDTH               TEXTEQU         <4:4>
;  R---V 
;    44 : #define NV_PEXTDEV_BOOT_0_STRAP_RAM_WIDTH_64             0x00000000 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_RAM_WIDTH_64            EQU             000000000h
;  R---V 
;    45 : #define NV_PEXTDEV_BOOT_0_STRAP_RAM_WIDTH_128            0x00000001 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_RAM_WIDTH_128           EQU             000000001h
;  R-XVF 
;    46 : #define NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE                        5:5 /* R-XVF */
NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE                TEXTEQU         <5:5>
;  R---V 
;    47 : #define NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE_PCI             0x00000000 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE_PCI            EQU             000000000h
;  R---V 
;    48 : #define NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE_AGP             0x00000001 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE_AGP            EQU             000000001h
;  R-XVF 
;    49 : #define NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL                         6:6 /* R-XVF */
NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL         TEXTEQU         <6:6>
;  R---V 
;    50 : #define NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_13500K           0x00000000 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_13500K          EQU             000000000h
;  R---V 
;    51 : #define NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_14318180         0x00000001 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_14318180                EQU             000000001h
;  R-XVF 
;    52 : #define NV_PEXTDEV_BOOT_0_STRAP_TVMODE                          8:7 /* R-XVF */
NV_PEXTDEV_BOOT_0_STRAP_TVMODE          TEXTEQU         <8:7>
;  R---V 
;    53 : #define NV_PEXTDEV_BOOT_0_STRAP_TVMODE_SECAM             0x00000000 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_TVMODE_SECAM            EQU             000000000h
;  R---V 
;    54 : #define NV_PEXTDEV_BOOT_0_STRAP_TVMODE_NTSC              0x00000001 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_TVMODE_NTSC             EQU             000000001h
;  R---V 
;    55 : #define NV_PEXTDEV_BOOT_0_STRAP_TVMODE_PAL               0x00000002 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_TVMODE_PAL              EQU             000000002h
;  R---V 
;    56 : #define NV_PEXTDEV_BOOT_0_STRAP_TVMODE_DISABLED          0x00000003 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_TVMODE_DISABLED         EQU             000000003h
;  R-XVF 
;    57 : #define NV_PEXTDEV_BOOT_0_STRAP_PCI_MODE                        9:9 /* R-XVF */
NV_PEXTDEV_BOOT_0_STRAP_PCI_MODE                TEXTEQU         <9:9>
;  R---V 
;    58 : #define NV_PEXTDEV_BOOT_0_STRAP_PCI_MODE_20              0x00000000 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_PCI_MODE_20             EQU             000000000h
;  R---V 
;    59 : #define NV_PEXTDEV_BOOT_0_STRAP_PCI_MODE_21              0x00000001 /* R---V */
NV_PEXTDEV_BOOT_0_STRAP_PCI_MODE_21             EQU             000000001h
;  dev_ext_devices.ref 
;  RW--D 
;    60 : /* dev_ext_devices.ref */
;    61 : #define NV_PDAC                               0x00680FFF:0x00680000 /* RW--D */
NV_PDAC         TEXTEQU         <0x00680FFF:0x00680000>
;  RW-4A 
;        
;    62 : #define NV_PDAC_DATA(i)                          (0x00500000+(i)*4) /* RW-4A */
;    63 : #define NV_PDAC_DATA__SIZE_1                                     16 /*       */
NV_PDAC_DATA__SIZE_1            EQU             16t
;  RW-VF 
;    64 : #define NV_PDAC_DATA_VALUE                                      7:0 /* RW-VF */
NV_PDAC_DATA_VALUE              TEXTEQU         <7:0>
;  dev_ext_devices.ref 
;  dev_ext_devices.ref 
;  RW--D 
;    65 : /* dev_ext_devices.ref */
;    66 : /* dev_ext_devices.ref */
;    67 : #define NV_PROM                               0x0011FFFF:0x00110000 /* RW--D */
NV_PROM         TEXTEQU         <0x0011FFFF:0x00110000>
;  RW-1A 
;        
;    68 : #define NV_PROM_DATA(i)                            (0x00110000+(i)) /* RW-1A */
;    69 : #define NV_PROM_DATA__SIZE_1                                  65536 /*       */
NV_PROM_DATA__SIZE_1            EQU             65536t
;  RW-VF 
;    70 : #define NV_PROM_DATA_VALUE                                      7:0 /* RW-VF */
NV_PROM_DATA_VALUE              TEXTEQU         <7:0>
;  dev_ext_devices.ref 
;  RW--D 
;    71 : /* dev_ext_devices.ref */
;    72 : #define NV_PALT                               0x0012FFFF:0x00120000 /* RW--D */
NV_PALT         TEXTEQU         <0x0012FFFF:0x00120000>
;  RW-1A 
;        
;    73 : #define NV_PALT_DATA(i)                            (0x00120000+(i)) /* RW-1A */
;    74 : #define NV_PALT_DATA__SIZE_1                                  65536 /*       */
NV_PALT_DATA__SIZE_1            EQU             65536t
;  RW-VF 
;    75 : #define NV_PALT_DATA_VALUE                                      7:0 /* RW-VF */
NV_PALT_DATA_VALUE              TEXTEQU         <7:0>
;  dev_realmode.ref 
;  RW--D 
;    76 : /* dev_realmode.ref */
;    77 : #define NV_PRM                                0x00005FFF:0x00004000 /* RW--D */
NV_PRM          TEXTEQU         <0x00005FFF:0x00004000>
;  RW-4R 
;    78 : #define NV_PRM_INTR_0                                    0x00004100 /* RW-4R */
NV_PRM_INTR_0           EQU             000004100h
;  RWXVF 
;    79 : #define NV_PRM_INTR_0_TRACE_MPU401                              0:0 /* RWXVF */
NV_PRM_INTR_0_TRACE_MPU401              TEXTEQU         <0:0>
;  R---V 
;    80 : #define NV_PRM_INTR_0_TRACE_MPU401_NOT_PENDING           0x00000000 /* R---V */
NV_PRM_INTR_0_TRACE_MPU401_NOT_PENDING          EQU             000000000h
;  R---V 
;    81 : #define NV_PRM_INTR_0_TRACE_MPU401_PENDING               0x00000001 /* R---V */
NV_PRM_INTR_0_TRACE_MPU401_PENDING              EQU             000000001h
;  -W--V 
;    82 : #define NV_PRM_INTR_0_TRACE_MPU401_RESET                 0x00000001 /* -W--V */
NV_PRM_INTR_0_TRACE_MPU401_RESET                EQU             000000001h
;  RWXVF 
;    83 : #define NV_PRM_INTR_0_TRACE_FM                                  4:4 /* RWXVF */
NV_PRM_INTR_0_TRACE_FM          TEXTEQU         <4:4>
;  R---V 
;    84 : #define NV_PRM_INTR_0_TRACE_FM_NOT_PENDING               0x00000000 /* R---V */
NV_PRM_INTR_0_TRACE_FM_NOT_PENDING              EQU             000000000h
;  R---V 
;    85 : #define NV_PRM_INTR_0_TRACE_FM_PENDING                   0x00000001 /* R---V */
NV_PRM_INTR_0_TRACE_FM_PENDING          EQU             000000001h
;  -W--V 
;    86 : #define NV_PRM_INTR_0_TRACE_FM_RESET                     0x00000001 /* -W--V */
NV_PRM_INTR_0_TRACE_FM_RESET            EQU             000000001h
;  RWXVF 
;    87 : #define NV_PRM_INTR_0_TRACE_SB_DIGITAL                          8:8 /* RWXVF */
NV_PRM_INTR_0_TRACE_SB_DIGITAL          TEXTEQU         <8:8>
;  R---V 
;    88 : #define NV_PRM_INTR_0_TRACE_SB_DIGITAL_NOT_PENDING       0x00000000 /* R---V */
NV_PRM_INTR_0_TRACE_SB_DIGITAL_NOT_PENDING              EQU             000000000h
;  R---V 
;    89 : #define NV_PRM_INTR_0_TRACE_SB_DIGITAL_PENDING           0x00000001 /* R---V */
NV_PRM_INTR_0_TRACE_SB_DIGITAL_PENDING          EQU             000000001h
;  -W--V 
;    90 : #define NV_PRM_INTR_0_TRACE_SB_DIGITAL_RESET             0x00000001 /* -W--V */
NV_PRM_INTR_0_TRACE_SB_DIGITAL_RESET            EQU             000000001h
;  RWXVF 
;    91 : #define NV_PRM_INTR_0_TRACE_SB_MIXER                          12:12 /* RWXVF */
NV_PRM_INTR_0_TRACE_SB_MIXER            TEXTEQU         <12:12>
;  R---V 
;    92 : #define NV_PRM_INTR_0_TRACE_SB_MIXER_NOT_PENDING         0x00000000 /* R---V */
NV_PRM_INTR_0_TRACE_SB_MIXER_NOT_PENDING                EQU             000000000h
;  R---V 
;    93 : #define NV_PRM_INTR_0_TRACE_SB_MIXER_PENDING             0x00000001 /* R---V */
NV_PRM_INTR_0_TRACE_SB_MIXER_PENDING            EQU             000000001h
;  -W--V 
;    94 : #define NV_PRM_INTR_0_TRACE_SB_MIXER_RESET               0x00000001 /* -W--V */
NV_PRM_INTR_0_TRACE_SB_MIXER_RESET              EQU             000000001h
;  RWXVF 
;    95 : #define NV_PRM_INTR_0_TRACE_OVERFLOW                          16:16 /* RWXVF */
NV_PRM_INTR_0_TRACE_OVERFLOW            TEXTEQU         <16:16>
;  R---V 
;    96 : #define NV_PRM_INTR_0_TRACE_OVERFLOW_NOT_PENDING         0x00000000 /* R---V */
NV_PRM_INTR_0_TRACE_OVERFLOW_NOT_PENDING                EQU             000000000h
;  R---V 
;    97 : #define NV_PRM_INTR_0_TRACE_OVERFLOW_PENDING             0x00000001 /* R---V */
NV_PRM_INTR_0_TRACE_OVERFLOW_PENDING            EQU             000000001h
;  -W--V 
;    98 : #define NV_PRM_INTR_0_TRACE_OVERFLOW_RESET               0x00000001 /* -W--V */
NV_PRM_INTR_0_TRACE_OVERFLOW_RESET              EQU             000000001h
;  RW-4R 
;    99 : #define NV_PRM_INTR_EN_0                                 0x00004140 /* RW-4R */
NV_PRM_INTR_EN_0                EQU             000004140h
;  RWIVF 
;   100 : #define NV_PRM_INTR_EN_0_TRACE_MPU401                           0:0 /* RWIVF */
NV_PRM_INTR_EN_0_TRACE_MPU401           TEXTEQU         <0:0>
;  RWI-V 
;   101 : #define NV_PRM_INTR_EN_0_TRACE_MPU401_DISABLED           0x00000000 /* RWI-V */
NV_PRM_INTR_EN_0_TRACE_MPU401_DISABLED          EQU             000000000h
;  RW--V 
;   102 : #define NV_PRM_INTR_EN_0_TRACE_MPU401_ENABLED            0x00000001 /* RW--V */
NV_PRM_INTR_EN_0_TRACE_MPU401_ENABLED           EQU             000000001h
;  RWIVF 
;   103 : #define NV_PRM_INTR_EN_0_TRACE_FM                               4:4 /* RWIVF */
NV_PRM_INTR_EN_0_TRACE_FM               TEXTEQU         <4:4>
;  RWI-V 
;   104 : #define NV_PRM_INTR_EN_0_TRACE_FM_DISABLED               0x00000000 /* RWI-V */
NV_PRM_INTR_EN_0_TRACE_FM_DISABLED              EQU             000000000h
;  RW--V 
;   105 : #define NV_PRM_INTR_EN_0_TRACE_FM_ENABLED                0x00000001 /* RW--V */
NV_PRM_INTR_EN_0_TRACE_FM_ENABLED               EQU             000000001h
;  RWIVF 
;   106 : #define NV_PRM_INTR_EN_0_TRACE_SB_DIGITAL                       8:8 /* RWIVF */
NV_PRM_INTR_EN_0_TRACE_SB_DIGITAL               TEXTEQU         <8:8>
;  RWI-V 
;   107 : #define NV_PRM_INTR_EN_0_TRACE_SB_DIGITAL_DISABLED       0x00000000 /* RWI-V */
NV_PRM_INTR_EN_0_TRACE_SB_DIGITAL_DISABLED              EQU             000000000h
;  RW--V 
;   108 : #define NV_PRM_INTR_EN_0_TRACE_SB_DIGITAL_ENABLED        0x00000001 /* RW--V */
NV_PRM_INTR_EN_0_TRACE_SB_DIGITAL_ENABLED               EQU             000000001h
;  RWIVF 
;   109 : #define NV_PRM_INTR_EN_0_TRACE_SB_MIXER                       12:12 /* RWIVF */
NV_PRM_INTR_EN_0_TRACE_SB_MIXER         TEXTEQU         <12:12>
;  RWI-V 
;   110 : #define NV_PRM_INTR_EN_0_TRACE_SB_MIXER_DISABLED         0x00000000 /* RWI-V */
NV_PRM_INTR_EN_0_TRACE_SB_MIXER_DISABLED                EQU             000000000h
;  RW--V 
;   111 : #define NV_PRM_INTR_EN_0_TRACE_SB_MIXER_ENABLED          0x00000001 /* RW--V */
NV_PRM_INTR_EN_0_TRACE_SB_MIXER_ENABLED         EQU             000000001h
;  RWIVF 
;   112 : #define NV_PRM_INTR_EN_0_TRACE_OVERFLOW                       16:16 /* RWIVF */
NV_PRM_INTR_EN_0_TRACE_OVERFLOW         TEXTEQU         <16:16>
;  RWI-V 
;   113 : #define NV_PRM_INTR_EN_0_TRACE_OVERFLOW_DISABLED         0x00000000 /* RWI-V */
NV_PRM_INTR_EN_0_TRACE_OVERFLOW_DISABLED                EQU             000000000h
;  RW--V 
;   114 : #define NV_PRM_INTR_EN_0_TRACE_OVERFLOW_ENABLED          0x00000001 /* RW--V */
NV_PRM_INTR_EN_0_TRACE_OVERFLOW_ENABLED         EQU             000000001h
;  RW-4R 
;   115 : #define NV_PRM_RAMRM                                     0x00004200 /* RW-4R */
NV_PRM_RAMRM            EQU             000004200h
;  RWXVF 
;   116 : #define NV_PRM_RAMRM_BASE_ADDRESS                             15:12 /* RWXVF */
NV_PRM_RAMRM_BASE_ADDRESS               TEXTEQU         <15:12>
;  RWI-V 
;   117 : #define NV_PRM_RAMRM_BASE_ADDRESS_2000                   0x00002000 /* RWI-V */
NV_PRM_RAMRM_BASE_ADDRESS_2000          EQU             000002000h
;  RW-4R 
;   118 : #define NV_PRM_TRACE                                     0x00004300 /* RW-4R */
NV_PRM_TRACE            EQU             000004300h
;  RWIVF 
;   119 : #define NV_PRM_TRACE_IO_CAPTURE                                 1:0 /* RWIVF */
NV_PRM_TRACE_IO_CAPTURE         TEXTEQU         <1:0>
;  RWI-V 
;   120 : #define NV_PRM_TRACE_IO_CAPTURE_DISABLED                 0x00000000 /* RWI-V */
NV_PRM_TRACE_IO_CAPTURE_DISABLED                EQU             000000000h
;  RW--V 
;   121 : #define NV_PRM_TRACE_IO_CAPTURE_WRITES                   0x00000001 /* RW--V */
NV_PRM_TRACE_IO_CAPTURE_WRITES          EQU             000000001h
;  RW--V 
;   122 : #define NV_PRM_TRACE_IO_CAPTURE_READS                    0x00000002 /* RW--V */
NV_PRM_TRACE_IO_CAPTURE_READS           EQU             000000002h
;  RW--V 
;   123 : #define NV_PRM_TRACE_IO_CAPTURE_READS_WRITES             0x00000003 /* RW--V */
NV_PRM_TRACE_IO_CAPTURE_READS_WRITES            EQU             000000003h
;  RWXVF 
;   124 : #define NV_PRM_TRACE_IO_WRITE                                   4:4 /* RWXVF */
NV_PRM_TRACE_IO_WRITE           TEXTEQU         <4:4>
;  R---V 
;   125 : #define NV_PRM_TRACE_IO_WRITE_NONE                       0x00000000 /* R---V */
NV_PRM_TRACE_IO_WRITE_NONE              EQU             000000000h
;  R---V 
;   126 : #define NV_PRM_TRACE_IO_WRITE_OCCURED                    0x00000001 /* R---V */
NV_PRM_TRACE_IO_WRITE_OCCURED           EQU             000000001h
;  -W--V 
;   127 : #define NV_PRM_TRACE_IO_WRITE_RESET                      0x00000001 /* -W--V */
NV_PRM_TRACE_IO_WRITE_RESET             EQU             000000001h
;  RWXVF 
;   128 : #define NV_PRM_TRACE_IO_READ                                    5:5 /* RWXVF */
NV_PRM_TRACE_IO_READ            TEXTEQU         <5:5>
;  R---V 
;   129 : #define NV_PRM_TRACE_IO_READ_NONE                        0x00000000 /* R---V */
NV_PRM_TRACE_IO_READ_NONE               EQU             000000000h
;  R---V 
;   130 : #define NV_PRM_TRACE_IO_READ_OCCURED                     0x00000001 /* R---V */
NV_PRM_TRACE_IO_READ_OCCURED            EQU             000000001h
;  -W--V 
;   131 : #define NV_PRM_TRACE_IO_READ_RESET                       0x00000001 /* -W--V */
NV_PRM_TRACE_IO_READ_RESET              EQU             000000001h
;  RW-4R 
;   132 : #define NV_PRM_TRACE_INDEX                               0x00004310 /* RW-4R */
NV_PRM_TRACE_INDEX              EQU             000004310h
;  RWXUF 
;   133 : #define NV_PRM_TRACE_INDEX_ADDRESS                              9:0 /* RWXUF */
NV_PRM_TRACE_INDEX_ADDRESS              TEXTEQU         <9:0>
;  RW--V 
;   134 : #define NV_PRM_TRACE_INDEX_ADDRESS_0                     0x00000000 /* RW--V */
NV_PRM_TRACE_INDEX_ADDRESS_0            EQU             000000000h
;  RW-4R 
;   135 : #define NV_PRM_IGNORE_0                                  0x00004320 /* RW-4R */
NV_PRM_IGNORE_0         EQU             000004320h
;  RWXVF 
;   136 : #define NV_PRM_IGNORE_0_MPU401                                  1:0 /* RWXVF */
NV_PRM_IGNORE_0_MPU401          TEXTEQU         <1:0>
;  RW--V 
;   137 : #define NV_PRM_IGNORE_0_MPU401_DISABLED                  0x00000000 /* RW--V */
NV_PRM_IGNORE_0_MPU401_DISABLED         EQU             000000000h
;  RW--V 
;   138 : #define NV_PRM_IGNORE_0_MPU401_WRITES                    0x00000001 /* RW--V */
NV_PRM_IGNORE_0_MPU401_WRITES           EQU             000000001h
;  RW--V 
;   139 : #define NV_PRM_IGNORE_0_MPU401_READS                     0x00000002 /* RW--V */
NV_PRM_IGNORE_0_MPU401_READS            EQU             000000002h
;  RW--V 
;   140 : #define NV_PRM_IGNORE_0_MPU401_READS_WRITES              0x00000003 /* RW--V */
NV_PRM_IGNORE_0_MPU401_READS_WRITES             EQU             000000003h
;  RWXVF 
;   141 : #define NV_PRM_IGNORE_0_FM                                      5:4 /* RWXVF */
NV_PRM_IGNORE_0_FM              TEXTEQU         <5:4>
;  RW--V 
;   142 : #define NV_PRM_IGNORE_0_FM_DISABLED                      0x00000000 /* RW--V */
NV_PRM_IGNORE_0_FM_DISABLED             EQU             000000000h
;  RW--V 
;   143 : #define NV_PRM_IGNORE_0_FM_WRITES                        0x00000001 /* RW--V */
NV_PRM_IGNORE_0_FM_WRITES               EQU             000000001h
;  RW--V 
;   144 : #define NV_PRM_IGNORE_0_FM_READS                         0x00000002 /* RW--V */
NV_PRM_IGNORE_0_FM_READS                EQU             000000002h
;  RW--V 
;   145 : #define NV_PRM_IGNORE_0_FM_READS_WRITES                  0x00000003 /* RW--V */
NV_PRM_IGNORE_0_FM_READS_WRITES         EQU             000000003h
;  RWXVF 
;   146 : #define NV_PRM_IGNORE_0_SB_DIGITAL                              9:8 /* RWXVF */
NV_PRM_IGNORE_0_SB_DIGITAL              TEXTEQU         <9:8>
;  RW--V 
;   147 : #define NV_PRM_IGNORE_0_SB_DIGITAL_DISABLED              0x00000000 /* RW--V */
NV_PRM_IGNORE_0_SB_DIGITAL_DISABLED             EQU             000000000h
;  RW--V 
;   148 : #define NV_PRM_IGNORE_0_SB_DIGITAL_WRITES                0x00000001 /* RW--V */
NV_PRM_IGNORE_0_SB_DIGITAL_WRITES               EQU             000000001h
;  RW--V 
;   149 : #define NV_PRM_IGNORE_0_SB_DIGITAL_READS                 0x00000002 /* RW--V */
NV_PRM_IGNORE_0_SB_DIGITAL_READS                EQU             000000002h
;  RW--V 
;   150 : #define NV_PRM_IGNORE_0_SB_DIGITAL_READS_WRITES          0x00000003 /* RW--V */
NV_PRM_IGNORE_0_SB_DIGITAL_READS_WRITES         EQU             000000003h
;  RWXVF 
;   151 : #define NV_PRM_IGNORE_0_SB_MIXER                              13:12 /* RWXVF */
NV_PRM_IGNORE_0_SB_MIXER                TEXTEQU         <13:12>
;  RW--V 
;   152 : #define NV_PRM_IGNORE_0_SB_MIXER_DISABLED                0x00000000 /* RW--V */
NV_PRM_IGNORE_0_SB_MIXER_DISABLED               EQU             000000000h
;  RW--V 
;   153 : #define NV_PRM_IGNORE_0_SB_MIXER_WRITES                  0x00000001 /* RW--V */
NV_PRM_IGNORE_0_SB_MIXER_WRITES         EQU             000000001h
;  RW--V 
;   154 : #define NV_PRM_IGNORE_0_SB_MIXER_READS                   0x00000002 /* RW--V */
NV_PRM_IGNORE_0_SB_MIXER_READS          EQU             000000002h
;  RW--V 
;   155 : #define NV_PRM_IGNORE_0_SB_MIXER_READS_WRITES            0x00000003 /* RW--V */
NV_PRM_IGNORE_0_SB_MIXER_READS_WRITES           EQU             000000003h
;  dev_realmode.ref 
;  RW--D 
;   156 : /* dev_realmode.ref */
;   157 : #define NV_PRMIO                              0x00007FFF:0x00007000 /* RW--D */
NV_PRMIO                TEXTEQU         <0x00007FFF:0x00007000>
;  dev_realmode.ref 
;  dev_fifo.ref 
;  RW--D 
;   158 : /* dev_realmode.ref */
;   159 : /* dev_fifo.ref */
;   160 : #define NV_USER                               0x00FFFFFF:0x00800000 /* RW--D */
NV_USER         TEXTEQU         <0x00FFFFFF:0x00800000>
;  -W-4A 
;        
;   161 : #define NV_USER_OBJECT(i,j)     (0x00800000+(i)*0x10000+(j)*0x2000) /* -W-4A */
;   162 : #define NV_USER_OBJECT__SIZE_1                                  128 /*       */
NV_USER_OBJECT__SIZE_1          EQU             128t
;        
;   163 : #define NV_USER_OBJECT__SIZE_2                                    8 /*       */
NV_USER_OBJECT__SIZE_2          EQU             8t
;  -W-VF 
;   164 : #define NV_USER_OBJECT_HANDLE                                  31:0 /* -W-VF */
NV_USER_OBJECT_HANDLE           TEXTEQU         <31:0>
;  R--2A 
;        
;   165 : #define NV_USER_FREE016(i,j)        (0x00800010+(i)*65536+(j)*8192) /* R--2A */
;   166 : #define NV_USER_FREE016__SIZE_1                                 128 /*       */
NV_USER_FREE016__SIZE_1         EQU             128t
;        
;   167 : #define NV_USER_FREE016__SIZE_2                                   8 /*       */
NV_USER_FREE016__SIZE_2         EQU             8t
;  C--UF 
;   168 : #define NV_USER_FREE016_COUNT_LO                                1:0 /* C--UF */
NV_USER_FREE016_COUNT_LO                TEXTEQU         <1:0>
;  C---V 
;   169 : #define NV_USER_FREE016_COUNT_LO_0                       0x00000000 /* C---V */
NV_USER_FREE016_COUNT_LO_0              EQU             000000000h
;  R--UF 
;   170 : #define NV_USER_FREE016_COUNT                                   9:2 /* R--UF */
NV_USER_FREE016_COUNT           TEXTEQU         <9:2>
;  C--UF 
;   171 : #define NV_USER_FREE016_COUNT_HI                              15:10 /* C--UF */
NV_USER_FREE016_COUNT_HI                TEXTEQU         <15:10>
;  C---V 
;   172 : #define NV_USER_FREE016_COUNT_HI_0                       0x00000000 /* C---V */
NV_USER_FREE016_COUNT_HI_0              EQU             000000000h
;  R--4A 
;        
;   173 : #define NV_USER_FREE032(i,j)        (0x00800010+(i)*65536+(j)*8192) /* R--4A */
;   174 : #define NV_USER_FREE032__SIZE_1                                 128 /*       */
NV_USER_FREE032__SIZE_1         EQU             128t
;        
;   175 : #define NV_USER_FREE032__SIZE_2                                   8 /*       */
NV_USER_FREE032__SIZE_2         EQU             8t
;  C--UF 
;   176 : #define NV_USER_FREE032_COUNT_LO                                1:0 /* C--UF */
NV_USER_FREE032_COUNT_LO                TEXTEQU         <1:0>
;  C---V 
;   177 : #define NV_USER_FREE032_COUNT_LO_0                       0x00000000 /* C---V */
NV_USER_FREE032_COUNT_LO_0              EQU             000000000h
;  R--UF 
;   178 : #define NV_USER_FREE032_COUNT                                   9:2 /* R--UF */
NV_USER_FREE032_COUNT           TEXTEQU         <9:2>
;  C--UF 
;   179 : #define NV_USER_FREE032_COUNT_HI                              31:10 /* C--UF */
NV_USER_FREE032_COUNT_HI                TEXTEQU         <31:10>
;  C---V 
;   180 : #define NV_USER_FREE032_COUNT_HI_0                       0x00000000 /* C---V */
NV_USER_FREE032_COUNT_HI_0              EQU             000000000h
;  R--2A 
;        
;   181 : #define NV_USER_ZERO016(i,j,k) (0x0800012+(i)*65536+(j)*8192+(k)*2) /* R--2A */
;   182 : #define NV_USER_ZERO016__SIZE_1                                 128 /*       */
NV_USER_ZERO016__SIZE_1         EQU             128t
;        
;   183 : #define NV_USER_ZERO016__SIZE_2                                   8 /*       */
NV_USER_ZERO016__SIZE_2         EQU             8t
;        
;   184 : #define NV_USER_ZERO016__SIZE_3                                   7 /*       */
NV_USER_ZERO016__SIZE_3         EQU             7t
;  C--UF 
;   185 : #define NV_USER_ZERO016_COUNT                                  15:0 /* C--UF */
NV_USER_ZERO016_COUNT           TEXTEQU         <15:0>
;  C---V 
;   186 : #define NV_USER_ZERO016_COUNT_0                          0x00000000 /* C---V */
NV_USER_ZERO016_COUNT_0         EQU             000000000h
;  R--4A 
;        
;   187 : #define NV_USER_ZERO032(i,j,k) (0x0800014+(i)*65536+(j)*8192+(k)*4) /* R--4A */
;   188 : #define NV_USER_ZERO032__SIZE_1                                 128 /*       */
NV_USER_ZERO032__SIZE_1         EQU             128t
;        
;   189 : #define NV_USER_ZERO032__SIZE_2                                   8 /*       */
NV_USER_ZERO032__SIZE_2         EQU             8t
;        
;   190 : #define NV_USER_ZERO032__SIZE_3                                   3 /*       */
NV_USER_ZERO032__SIZE_3         EQU             3t
;  C--UF 
;   191 : #define NV_USER_ZERO032_COUNT                                  31:0 /* C--UF */
NV_USER_ZERO032_COUNT           TEXTEQU         <31:0>
;  C---V 
;   192 : #define NV_USER_ZERO032_COUNT_0                          0x00000000 /* C---V */
NV_USER_ZERO032_COUNT_0         EQU             000000000h
;  dev_misc.ref 
;        
;   193 : /* dev_misc.ref */
;   194 : #define NV_USER_ADR_CHID                                      22:16 /*       */
NV_USER_ADR_CHID                TEXTEQU         <22:16>
;        
;   195 : #define NV_USER_ADR_SUBCHID                                   15:13 /*       */
NV_USER_ADR_SUBCHID             TEXTEQU         <15:13>
;        
;   196 : #define NV_USER_ADR_METHOD                                     12:0 /*       */
NV_USER_ADR_METHOD              TEXTEQU         <12:0>
;        
;   197 : #define NV_USER_DEVICE                                        22:16 /*       */
NV_USER_DEVICE          TEXTEQU         <22:16>
;  dev_timer.ref 
;  RW--D 
;   198 : /* dev_timer.ref */
;   199 : #define NV_PTIMER                             0x00009FFF:0x00009000 /* RW--D */
NV_PTIMER               TEXTEQU         <0x00009FFF:0x00009000>
;  RW-4R 
;   200 : #define NV_PTIMER_INTR_0                                 0x00009100 /* RW-4R */
NV_PTIMER_INTR_0                EQU             000009100h
;  RWXVF 
;   201 : #define NV_PTIMER_INTR_0_ALARM                                  0:0 /* RWXVF */
NV_PTIMER_INTR_0_ALARM          TEXTEQU         <0:0>
;  R---V 
;   202 : #define NV_PTIMER_INTR_0_ALARM_NOT_PENDING               0x00000000 /* R---V */
NV_PTIMER_INTR_0_ALARM_NOT_PENDING              EQU             000000000h
;  R---V 
;   203 : #define NV_PTIMER_INTR_0_ALARM_PENDING                   0x00000001 /* R---V */
NV_PTIMER_INTR_0_ALARM_PENDING          EQU             000000001h
;  -W--V 
;   204 : #define NV_PTIMER_INTR_0_ALARM_RESET                     0x00000001 /* -W--V */
NV_PTIMER_INTR_0_ALARM_RESET            EQU             000000001h
;  RW-4R 
;   205 : #define NV_PTIMER_INTR_EN_0                              0x00009140 /* RW-4R */
NV_PTIMER_INTR_EN_0             EQU             000009140h
;  RWIVF 
;   206 : #define NV_PTIMER_INTR_EN_0_ALARM                               0:0 /* RWIVF */
NV_PTIMER_INTR_EN_0_ALARM               TEXTEQU         <0:0>
;  RWI-V 
;   207 : #define NV_PTIMER_INTR_EN_0_ALARM_DISABLED               0x00000000 /* RWI-V */
NV_PTIMER_INTR_EN_0_ALARM_DISABLED              EQU             000000000h
;  RW--V 
;   208 : #define NV_PTIMER_INTR_EN_0_ALARM_ENABLED                0x00000001 /* RW--V */
NV_PTIMER_INTR_EN_0_ALARM_ENABLED               EQU             000000001h
;  RW-4R 
;   209 : #define NV_PTIMER_NUMERATOR                              0x00009200 /* RW-4R */
NV_PTIMER_NUMERATOR             EQU             000009200h
;  RWIUF 
;   210 : #define NV_PTIMER_NUMERATOR_VALUE                              15:0 /* RWIUF */
NV_PTIMER_NUMERATOR_VALUE               TEXTEQU         <15:0>
;  RWI-V 
;   211 : #define NV_PTIMER_NUMERATOR_VALUE_0                      0x00000000 /* RWI-V */
NV_PTIMER_NUMERATOR_VALUE_0             EQU             000000000h
;  RW-4R 
;   212 : #define NV_PTIMER_DENOMINATOR                            0x00009210 /* RW-4R */
NV_PTIMER_DENOMINATOR           EQU             000009210h
;  RWIUF 
;   213 : #define NV_PTIMER_DENOMINATOR_VALUE                            15:0 /* RWIUF */
NV_PTIMER_DENOMINATOR_VALUE             TEXTEQU         <15:0>
;  RWI-V 
;   214 : #define NV_PTIMER_DENOMINATOR_VALUE_0                    0x00000000 /* RWI-V */
NV_PTIMER_DENOMINATOR_VALUE_0           EQU             000000000h
;  RW-4R 
;   215 : #define NV_PTIMER_TIME_0                                 0x00009400 /* RW-4R */
NV_PTIMER_TIME_0                EQU             000009400h
;  RWXUF 
;   216 : #define NV_PTIMER_TIME_0_NSEC                                  31:5 /* RWXUF */
NV_PTIMER_TIME_0_NSEC           TEXTEQU         <31:5>
;  RW-4R 
;   217 : #define NV_PTIMER_TIME_1                                 0x00009410 /* RW-4R */
NV_PTIMER_TIME_1                EQU             000009410h
;  RWXUF 
;   218 : #define NV_PTIMER_TIME_1_NSEC                                  28:0 /* RWXUF */
NV_PTIMER_TIME_1_NSEC           TEXTEQU         <28:0>
;  RW-4R 
;   219 : #define NV_PTIMER_ALARM_0                                0x00009420 /* RW-4R */
NV_PTIMER_ALARM_0               EQU             000009420h
;  RWXUF 
;   220 : #define NV_PTIMER_ALARM_0_NSEC                                 31:5 /* RWXUF */
NV_PTIMER_ALARM_0_NSEC          TEXTEQU         <31:5>
;  dev_realmode.ref 
;  RW--M 
;   221 : /* dev_realmode.ref */
;   222 : #define NV_TRACE                              0x0000FFFF:0x00000000 /* RW--M */
NV_TRACE                TEXTEQU         <0x0000FFFF:0x00000000>
;  RWXVF 
;   223 : #define NV_TRACE_DATA                         ( 0*32+ 7):( 0*32+ 0) /* RWXVF */
NV_TRACE_DATA           TEXTEQU         <( 0*32+ 7):( 0*32+ 0)>
;  RWXVF 
;   224 : #define NV_TRACE_ACCESS                       ( 0*32+14):( 0*32+14) /* RWXVF */
NV_TRACE_ACCESS         TEXTEQU         <( 0*32+14):( 0*32+14)>
;  RW--V 
;   225 : #define NV_TRACE_ACCESS_WRITE                            0x00000000 /* RW--V */
NV_TRACE_ACCESS_WRITE           EQU             000000000h
;  RW--V 
;   226 : #define NV_TRACE_ACCESS_READ                             0x00000001 /* RW--V */
NV_TRACE_ACCESS_READ            EQU             000000001h
;  RWXVF 
;   227 : #define NV_TRACE_TYPE                         ( 0*32+15):( 0*32+15) /* RWXVF */
NV_TRACE_TYPE           TEXTEQU         <( 0*32+15):( 0*32+15)>
;  RW--V 
;   228 : #define NV_TRACE_TYPE_IO                                 0x00000000 /* RW--V */
NV_TRACE_TYPE_IO                EQU             000000000h
;  RW--V 
;   229 : #define NV_TRACE_TYPE_MEMORY                             0x00000001 /* RW--V */
NV_TRACE_TYPE_MEMORY            EQU             000000001h
;  RWXUF 
;   230 : #define NV_TRACE_ADDRESS                      ( 0*32+31):( 0*32+16) /* RWXUF */
NV_TRACE_ADDRESS                TEXTEQU         <( 0*32+31):( 0*32+16)>
;  dev_ram.ref 
;  RW--M 
;   231 : /* dev_ram.ref */
;   232 : #define NV_RAMHT__SIZE_0                      0x00000FFF:0x00000000 /* RW--M */
NV_RAMHT__SIZE_0                TEXTEQU         <0x00000FFF:0x00000000>
;  RW--M 
;   233 : #define NV_RAMHT__SIZE_1                      0x00001FFF:0x00000000 /* RW--M */
NV_RAMHT__SIZE_1                TEXTEQU         <0x00001FFF:0x00000000>
;  RW--M 
;   234 : #define NV_RAMHT__SIZE_2                      0x00003FFF:0x00000000 /* RW--M */
NV_RAMHT__SIZE_2                TEXTEQU         <0x00003FFF:0x00000000>
;  RW--M 
;   235 : #define NV_RAMHT__SIZE_3                      0x00007FFF:0x00000000 /* RW--M */
NV_RAMHT__SIZE_3                TEXTEQU         <0x00007FFF:0x00000000>
;  RWXVF 
;   236 : #define NV_RAMHT_HANDLE                       ( 0*32+31):( 0*32+ 0) /* RWXVF */
NV_RAMHT_HANDLE         TEXTEQU         <( 0*32+31):( 0*32+ 0)>
;  RWXUF 
;   237 : #define NV_RAMHT_INSTANCE                     ( 1*32+15):( 1*32+ 0) /* RWXUF */
NV_RAMHT_INSTANCE               TEXTEQU         <( 1*32+15):( 1*32+ 0)>
;  RWXUF 
;   238 : #define NV_RAMHT_DEVICE                       ( 1*32+22):( 1*32+16) /* RWXUF */
NV_RAMHT_DEVICE         TEXTEQU         <( 1*32+22):( 1*32+16)>
;  RWXVF 
;   239 : #define NV_RAMHT_ENGINE                       ( 1*32+23):( 1*32+23) /* RWXVF */
NV_RAMHT_ENGINE         TEXTEQU         <( 1*32+23):( 1*32+23)>
;  RW--V 
;   240 : #define NV_RAMHT_ENGINE_SW                               0x00000000 /* RW--V */
NV_RAMHT_ENGINE_SW              EQU             000000000h
;  RW--V 
;   241 : #define NV_RAMHT_ENGINE_GRAPHICS                         0x00000001 /* RW--V */
NV_RAMHT_ENGINE_GRAPHICS                EQU             000000001h
;  RWXUF 
;   242 : #define NV_RAMHT_CHID                         ( 1*32+30):( 1*32+24) /* RWXUF */
NV_RAMHT_CHID           TEXTEQU         <( 1*32+30):( 1*32+24)>
;  dev_ram.ref 
;  RW--M 
;   243 : /* dev_ram.ref */
;   244 : #define NV_RAMRO__SIZE_0                      0x000001FF:0x00000000 /* RW--M */
NV_RAMRO__SIZE_0                TEXTEQU         <0x000001FF:0x00000000>
;  RW--M 
;   245 : #define NV_RAMRO__SIZE_1                      0x00001FFF:0x00000000 /* RW--M */
NV_RAMRO__SIZE_1                TEXTEQU         <0x00001FFF:0x00000000>
;  RWXUF 
;   246 : #define NV_RAMRO_METHOD                       ( 0*32+15):( 0*32+ 0) /* RWXUF */
NV_RAMRO_METHOD         TEXTEQU         <( 0*32+15):( 0*32+ 0)>
;  RWXUF 
;   247 : #define NV_RAMRO_CHID                         ( 0*32+22):( 0*32+16) /* RWXUF */
NV_RAMRO_CHID           TEXTEQU         <( 0*32+22):( 0*32+16)>
;  RWXVF 
;   248 : #define NV_RAMRO_TYPE                         ( 0*32+23):( 0*32+23) /* RWXVF */
NV_RAMRO_TYPE           TEXTEQU         <( 0*32+23):( 0*32+23)>
;  RW--V 
;   249 : #define NV_RAMRO_TYPE_WRITE                              0x00000000 /* RW--V */
NV_RAMRO_TYPE_WRITE             EQU             000000000h
;  RW--V 
;   250 : #define NV_RAMRO_TYPE_READ                               0x00000001 /* RW--V */
NV_RAMRO_TYPE_READ              EQU             000000001h
;  RWXVF 
;   251 : #define NV_RAMRO_BYTE_ENABLES                 ( 0*32+27):( 0*32+24) /* RWXVF */
NV_RAMRO_BYTE_ENABLES           TEXTEQU         <( 0*32+27):( 0*32+24)>
;  RWXVF 
;   252 : #define NV_RAMRO_REASON                       ( 0*32+31):( 0*32+28) /* RWXVF */
NV_RAMRO_REASON         TEXTEQU         <( 0*32+31):( 0*32+28)>
;  RW--V 
;   253 : #define NV_RAMRO_REASON_ILLEGAL_ACCESS                   0x00000000 /* RW--V */
NV_RAMRO_REASON_ILLEGAL_ACCESS          EQU             000000000h
;  RW--V 
;   254 : #define NV_RAMRO_REASON_NO_CACHE_AVAILABLE               0x00000001 /* RW--V */
NV_RAMRO_REASON_NO_CACHE_AVAILABLE              EQU             000000001h
;  RW--V 
;   255 : #define NV_RAMRO_REASON_CACHE_RAN_OUT                    0x00000002 /* RW--V */
NV_RAMRO_REASON_CACHE_RAN_OUT           EQU             000000002h
;  RW--V 
;   256 : #define NV_RAMRO_REASON_FREE_COUNT_OVERRUN               0x00000003 /* RW--V */
NV_RAMRO_REASON_FREE_COUNT_OVERRUN              EQU             000000003h
;  RW--V 
;   257 : #define NV_RAMRO_REASON_CAUGHT_LYING                     0x00000004 /* RW--V */
NV_RAMRO_REASON_CAUGHT_LYING            EQU             000000004h
;  RW--V 
;   258 : #define NV_RAMRO_REASON_RESERVED_ACCESS                  0x00000005 /* RW--V */
NV_RAMRO_REASON_RESERVED_ACCESS         EQU             000000005h
;  RWXVF 
;   259 : #define NV_RAMRO_DATA                         ( 1*32+31):( 1*32+ 0) /* RWXVF */
NV_RAMRO_DATA           TEXTEQU         <( 1*32+31):( 1*32+ 0)>
;  dev_ram.ref 
;  RW--M 
;   260 : /* dev_ram.ref */
;   261 : #define NV_RAMAU                              0x00000BFF:0x00000000 /* RW--M */
NV_RAMAU                TEXTEQU         <0x00000BFF:0x00000000>
;  RWXVF 
;   262 : #define NV_RAMAU_DATA                                          31:0 /* RWXVF */
NV_RAMAU_DATA           TEXTEQU         <31:0>
;  dev_ram.ref 
;  RW--M 
;   263 : /* dev_ram.ref */
;   264 : #define NV_RAMFC__SIZE_0                      0x000001FF:0x00000000 /* RW--M */
NV_RAMFC__SIZE_0                TEXTEQU         <0x000001FF:0x00000000>
;  RW--M 
;   265 : #define NV_RAMFC__SIZE_1                      0x00000FFF:0x00000000 /* RW--M */
NV_RAMFC__SIZE_1                TEXTEQU         <0x00000FFF:0x00000000>
;  RWXUF 
;   266 : #define NV_RAMFC_INSTANCE_0                   ( 0*32+15):( 0*32+ 0) /* RWXUF */
NV_RAMFC_INSTANCE_0             TEXTEQU         <( 0*32+15):( 0*32+ 0)>
;  RWXUF 
;   267 : #define NV_RAMFC_DEVICE_0                     ( 0*32+22):( 0*32+16) /* RWXUF */
NV_RAMFC_DEVICE_0               TEXTEQU         <( 0*32+22):( 0*32+16)>
;  RW--V 
;   268 : #define NV_RAMFC_DEVICE_0_NOT_FOUND                      0x00000000 /* RW--V */
NV_RAMFC_DEVICE_0_NOT_FOUND             EQU             000000000h
;  RWXVF 
;   269 : #define NV_RAMFC_ENGINE_0                     ( 0*32+23):( 0*32+23) /* RWXVF */
NV_RAMFC_ENGINE_0               TEXTEQU         <( 0*32+23):( 0*32+23)>
;  RW--V 
;   270 : #define NV_RAMFC_ENGINE_0_SW                             0x00000000 /* RW--V */
NV_RAMFC_ENGINE_0_SW            EQU             000000000h
;  RW--V 
;   271 : #define NV_RAMFC_ENGINE_0_GRAPHICS                       0x00000001 /* RW--V */
NV_RAMFC_ENGINE_0_GRAPHICS              EQU             000000001h
;  RWXUF 
;   272 : #define NV_RAMFC_INSTANCE_1                   ( 1*32+15):( 1*32+ 0) /* RWXUF */
NV_RAMFC_INSTANCE_1             TEXTEQU         <( 1*32+15):( 1*32+ 0)>
;  RWXUF 
;   273 : #define NV_RAMFC_DEVICE_1                     ( 1*32+22):( 1*32+16) /* RWXUF */
NV_RAMFC_DEVICE_1               TEXTEQU         <( 1*32+22):( 1*32+16)>
;  RW--V 
;   274 : #define NV_RAMFC_DEVICE_1_NOT_FOUND                      0x00000000 /* RW--V */
NV_RAMFC_DEVICE_1_NOT_FOUND             EQU             000000000h
;  RWXVF 
;   275 : #define NV_RAMFC_ENGINE_1                     ( 1*32+23):( 1*32+23) /* RWXVF */
NV_RAMFC_ENGINE_1               TEXTEQU         <( 1*32+23):( 1*32+23)>
;  RW--V 
;   276 : #define NV_RAMFC_ENGINE_1_SW                             0x00000000 /* RW--V */
NV_RAMFC_ENGINE_1_SW            EQU             000000000h
;  RW--V 
;   277 : #define NV_RAMFC_ENGINE_1_GRAPHICS                       0x00000001 /* RW--V */
NV_RAMFC_ENGINE_1_GRAPHICS              EQU             000000001h
;  RWXUF 
;   278 : #define NV_RAMFC_INSTANCE_7                   ( 7*32+15):( 7*32+ 0) /* RWXUF */
NV_RAMFC_INSTANCE_7             TEXTEQU         <( 7*32+15):( 7*32+ 0)>
;  RWXUF 
;   279 : #define NV_RAMFC_DEVICE_7                     ( 7*32+22):( 7*32+16) /* RWXUF */
NV_RAMFC_DEVICE_7               TEXTEQU         <( 7*32+22):( 7*32+16)>
;  RW--V 
;   280 : #define NV_RAMFC_DEVICE_7_NOT_FOUND                      0x00000000 /* RW--V */
NV_RAMFC_DEVICE_7_NOT_FOUND             EQU             000000000h
;  RWXVF 
;   281 : #define NV_RAMFC_ENGINE_7                     ( 7*32+23):( 7*32+23) /* RWXVF */
NV_RAMFC_ENGINE_7               TEXTEQU         <( 7*32+23):( 7*32+23)>
;  RW--V 
;   282 : #define NV_RAMFC_ENGINE_7_SW                             0x00000000 /* RW--V */
NV_RAMFC_ENGINE_7_SW            EQU             000000000h
;  RW--V 
;   283 : #define NV_RAMFC_ENGINE_7_GRAPHICS                       0x00000001 /* RW--V */
NV_RAMFC_ENGINE_7_GRAPHICS              EQU             000000001h
;  dev_ram.ref 
;  dev_ram.ref 
;  dev_ram.ref 
;  RWXUF 
;   284 : /* dev_ram.ref */
;   285 : /* dev_ram.ref */
;   286 : /* dev_ram.ref */
;   287 : #define NV_DMA_ADJUST                         ( 0*32+11):( 0*32+ 0) /* RWXUF */
NV_DMA_ADJUST           TEXTEQU         <( 0*32+11):( 0*32+ 0)>
;  RWXVF 
;   288 : #define NV_DMA_PAGE_TABLE                     ( 0*32+16):( 0*32+16) /* RWXVF */
NV_DMA_PAGE_TABLE               TEXTEQU         <( 0*32+16):( 0*32+16)>
;  RW--V 
;   289 : #define NV_DMA_PAGE_TABLE_NOT_PRESENT                    0x00000000 /* RW--V */
NV_DMA_PAGE_TABLE_NOT_PRESENT           EQU             000000000h
;  RW--V 
;   290 : #define NV_DMA_PAGE_TABLE_PRESENT                        0x00000001 /* RW--V */
NV_DMA_PAGE_TABLE_PRESENT               EQU             000000001h
;  RWXVF 
;   291 : #define NV_DMA_TARGET_NODE                    ( 0*32+25):( 0*32+24) /* RWXVF */
NV_DMA_TARGET_NODE              TEXTEQU         <( 0*32+25):( 0*32+24)>
;  RW--V 
;   292 : #define NV_DMA_TARGET_NODE_NVM                           0x00000000 /* RW--V */
NV_DMA_TARGET_NODE_NVM          EQU             000000000h
;  RW--V 
;   293 : #define NV_DMA_TARGET_NODE_PCI                           0x00000002 /* RW--V */
NV_DMA_TARGET_NODE_PCI          EQU             000000002h
;  RW--V 
;   294 : #define NV_DMA_TARGET_NODE_AGP                           0x00000003 /* RW--V */
NV_DMA_TARGET_NODE_AGP          EQU             000000003h
;  RWXUF 
;   295 : #define NV_DMA_LIMIT                          ( 1*32+31):( 1*32+ 0) /* RWXUF */
NV_DMA_LIMIT            TEXTEQU         <( 1*32+31):( 1*32+ 0)>
;  RWXVF 
;   296 : #define NV_DMA_PAGE                           ( 2*32+ 0):( 2*32+ 0) /* RWXVF */
NV_DMA_PAGE             TEXTEQU         <( 2*32+ 0):( 2*32+ 0)>
;  RW--V 
;   297 : #define NV_DMA_PAGE_NOT_PRESENT                          0x00000000 /* RW--V */
NV_DMA_PAGE_NOT_PRESENT         EQU             000000000h
;  RW--V 
;   298 : #define NV_DMA_PAGE_PRESENT                              0x00000001 /* RW--V */
NV_DMA_PAGE_PRESENT             EQU             000000001h
;  RWXVF 
;   299 : #define NV_DMA_ACCESS                         ( 2*32+ 1):( 2*32+ 1) /* RWXVF */
NV_DMA_ACCESS           TEXTEQU         <( 2*32+ 1):( 2*32+ 1)>
;  RW--V 
;   300 : #define NV_DMA_ACCESS_READ_ONLY                          0x00000000 /* RW--V */
NV_DMA_ACCESS_READ_ONLY         EQU             000000000h
;  RW--V 
;   301 : #define NV_DMA_ACCESS_READ_AND_WRITE                     0x00000001 /* RW--V */
NV_DMA_ACCESS_READ_AND_WRITE            EQU             000000001h
;  RWXUF 
;   302 : #define NV_DMA_FRAME_ADDRESS                  ( 2*32+31):( 2*32+12) /* RWXUF */
NV_DMA_FRAME_ADDRESS            TEXTEQU         <( 2*32+31):( 2*32+12)>
;  dev_ram.ref 
;  RWXUF 
;   303 : /* dev_ram.ref */
;   304 : #define NV_SUBCHAN_CTX_SWITCH                 ( 0*32+31):( 0*32+ 0) /* RWXUF */
NV_SUBCHAN_CTX_SWITCH           TEXTEQU         <( 0*32+31):( 0*32+ 0)>
;  RWXUF 
;   305 : #define NV_SUBCHAN_DMA_INSTANCE               ( 1*32+15):( 1*32+ 0) /* RWXUF */
NV_SUBCHAN_DMA_INSTANCE         TEXTEQU         <( 1*32+15):( 1*32+ 0)>
;  RWXUF 
;   306 : #define NV_SUBCHAN_NOTIFY_INSTANCE            ( 1*32+31):( 1*32+16) /* RWXUF */
NV_SUBCHAN_NOTIFY_INSTANCE              TEXTEQU         <( 1*32+31):( 1*32+16)>
;  RWXUF 
;   307 : #define NV_SUBCHAN_MEMFMT_INSTANCE            ( 2*32+15):( 2*32+ 0) /* RWXUF */
NV_SUBCHAN_MEMFMT_INSTANCE              TEXTEQU         <( 2*32+15):( 2*32+ 0)>
;  RWXUF 
;   308 : #define NV_SUBCHAN_MEMFMT_LINEAR              ( 2*32+16):( 2*32+16) /* RWXUF */
NV_SUBCHAN_MEMFMT_LINEAR                TEXTEQU         <( 2*32+16):( 2*32+16)>
;  RW--V 
;   309 : #define NV_SUBCHAN_MEMFMT_LINEAR_OUT                     0x00000000 /* RW--V */
NV_SUBCHAN_MEMFMT_LINEAR_OUT            EQU             000000000h
;  RW--V 
;   310 : #define NV_SUBCHAN_MEMFMT_LINEAR_IN                      0x00000001 /* RW--V */
NV_SUBCHAN_MEMFMT_LINEAR_IN             EQU             000000001h
;  dev_ram.ref 
;  RW--M 
;   311 : /* dev_ram.ref */
;   312 : #define NV_AUDIN                              0x00000033:0x00000000 /* RW--M */
NV_AUDIN                TEXTEQU         <0x00000033:0x00000000>
;  RWWVF 
;   313 : #define NV_AUDIN_AE_STATE                       (0*32+15):(0*32+ 0) /* RWWVF */
NV_AUDIN_AE_STATE               TEXTEQU         <(0*32+15):(0*32+ 0)>
;  RWWUF 
;   314 : #define NV_AUDIN_AE_SKIP_COUNT                  (1*32+31):(1*32+ 0) /* RWWUF */
NV_AUDIN_AE_SKIP_COUNT          TEXTEQU         <(1*32+31):(1*32+ 0)>
;  RWWUF 
;   315 : #define NV_AUDIN_AE_POSITION                    (2*32+31):(2*32+ 0) /* RWWUF */
NV_AUDIN_AE_POSITION            TEXTEQU         <(2*32+31):(2*32+ 0)>
;  RWWUF 
;   316 : #define NV_AUDIN_RM_NEXT_INSTANCE               (3*32+31):(3*32+ 0) /* RWWUF */
NV_AUDIN_RM_NEXT_INSTANCE               TEXTEQU         <(3*32+31):(3*32+ 0)>
;  RWWVF 
;   317 : #define NV_AUDIN_RM_STATE                       (4*32+15):(4*32+ 0) /* RWWVF */
NV_AUDIN_RM_STATE               TEXTEQU         <(4*32+15):(4*32+ 0)>
;  RWWVF 
;   318 : #define NV_AUDIN_SU_STATE                       (5*32+15):(5*32+ 0) /* RWWVF */
NV_AUDIN_SU_STATE               TEXTEQU         <(5*32+15):(5*32+ 0)>
;  RWWUF 
;   319 : #define NV_AUDIN_SU_START_TIME_LOW              (5*32+31):(5*32+16) /* RWWUF */
NV_AUDIN_SU_START_TIME_LOW              TEXTEQU         <(5*32+31):(5*32+16)>
;  RWWUF 
;   320 : #define NV_AUDIN_SU_START_TIME_HIGH             (6*32+31):(6*32+ 0) /* RWWUF */
NV_AUDIN_SU_START_TIME_HIGH             TEXTEQU         <(6*32+31):(6*32+ 0)>
;  RWWUF 
;   321 : #define NV_AUDIN_SU_SKIP_INC                    (7*32+31):(7*32+ 0) /* RWWUF */
NV_AUDIN_SU_SKIP_INC            TEXTEQU         <(7*32+31):(7*32+ 0)>
;  RWWUF 
;   322 : #define NV_AUDIN_SU_BUFF0_DMA_INSTANCE          (8*32+15):(8*32+ 0) /* RWWUF */
NV_AUDIN_SU_BUFF0_DMA_INSTANCE          TEXTEQU         <(8*32+15):(8*32+ 0)>
;  RWWUF 
;   323 : #define NV_AUDIN_SU_BUFF1_DMA_INSTANCE          (8*32+31):(8*32+16) /* RWWUF */
NV_AUDIN_SU_BUFF1_DMA_INSTANCE          TEXTEQU         <(8*32+31):(8*32+16)>
;  RWWUF 
;   324 : #define NV_AUDIN_SU_BUFF0_START_POSITION        (9*32+31):(9*32+ 0) /* RWWUF */
NV_AUDIN_SU_BUFF0_START_POSITION                TEXTEQU         <(9*32+31):(9*32+ 0)>
;  RWWUF 
;   325 : #define NV_AUDIN_SU_BUFF0_LAST_PLUS1          (10*32+31):(10*32+ 0) /* RWWUF */
NV_AUDIN_SU_BUFF0_LAST_PLUS1            TEXTEQU         <(10*32+31):(10*32+ 0)>
;  RWWUF 
;   326 : #define NV_AUDIN_SU_BUFF1_START_POSITION      (11*32+31):(11*32+ 0) /* RWWUF */
NV_AUDIN_SU_BUFF1_START_POSITION                TEXTEQU         <(11*32+31):(11*32+ 0)>
;  RWWUF 
;   327 : #define NV_AUDIN_SU_BUFF1_LAST_PLUS1          (12*32+31):(12*32+ 0) /* RWWUF */
NV_AUDIN_SU_BUFF1_LAST_PLUS1            TEXTEQU         <(12*32+31):(12*32+ 0)>
;  RWXVF 
;   328 : #define NV_AUDIN_AE_EVENT_START               ( 0*32+ 3):( 0*32+ 3) /* RWXVF */
NV_AUDIN_AE_EVENT_START         TEXTEQU         <( 0*32+ 3):( 0*32+ 3)>
;  RWXVF 
;   329 : #define NV_AUDIN_AE_BUFF0_INTR_NOTIFY         ( 0*32+ 4):( 0*32+ 4) /* RWXVF */
NV_AUDIN_AE_BUFF0_INTR_NOTIFY           TEXTEQU         <( 0*32+ 4):( 0*32+ 4)>
;  RWXVF 
;   330 : #define NV_AUDIN_AE_BUFF1_INTR_NOTIFY         ( 0*32+ 5):( 0*32+ 5) /* RWXVF */
NV_AUDIN_AE_BUFF1_INTR_NOTIFY           TEXTEQU         <( 0*32+ 5):( 0*32+ 5)>
;  RWXVF 
;   331 : #define NV_AUDIN_AE_BUFF0_INTR_CHAIN_GAP      ( 0*32+ 6):( 0*32+ 6) /* RWXVF */
NV_AUDIN_AE_BUFF0_INTR_CHAIN_GAP                TEXTEQU         <( 0*32+ 6):( 0*32+ 6)>
;  RWXVF 
;   332 : #define NV_AUDIN_AE_BUFF1_INTR_CHAIN_GAP      ( 0*32+ 7):( 0*32+ 7) /* RWXVF */
NV_AUDIN_AE_BUFF1_INTR_CHAIN_GAP                TEXTEQU         <( 0*32+ 7):( 0*32+ 7)>
;  RWXVF 
;   333 : #define NV_AUDIN_AE_BUFF0_IN_USE              ( 0*32+ 8):( 0*32+ 8) /* RWXVF */
NV_AUDIN_AE_BUFF0_IN_USE                TEXTEQU         <( 0*32+ 8):( 0*32+ 8)>
;  RWXVF 
;   334 : #define NV_AUDIN_AE_BUFF1_IN_USE              ( 0*32+ 9):( 0*32+ 9) /* RWXVF */
NV_AUDIN_AE_BUFF1_IN_USE                TEXTEQU         <( 0*32+ 9):( 0*32+ 9)>
;  RWXVF 
;   335 : #define NV_AUDIN_AE_CURRENT_BUFFER            ( 0*32+12):( 0*32+12) /* RWXVF */
NV_AUDIN_AE_CURRENT_BUFFER              TEXTEQU         <( 0*32+12):( 0*32+12)>
;  RW--V 
;   336 : #define NV_AUDIN_AE_CURRENT_BUFFER_0                     0x00000000 /* RW--V */
NV_AUDIN_AE_CURRENT_BUFFER_0            EQU             000000000h
;  RW--V 
;   337 : #define NV_AUDIN_AE_CURRENT_BUFFER_1                     0x00000001 /* RW--V */
NV_AUDIN_AE_CURRENT_BUFFER_1            EQU             000000001h
;  RWXVF 
;   338 : #define NV_AUDIN_AE_INTR_DMA                  ( 0*32+11):( 0*32+11) /* RWXVF */
NV_AUDIN_AE_INTR_DMA            TEXTEQU         <( 0*32+11):( 0*32+11)>
;  RWXVF 
;   339 : #define NV_AUDIN_RM_BUFF0_INTR_NOTIFY         ( 4*32+ 4):( 4*32+ 4) /* RWXVF */
NV_AUDIN_RM_BUFF0_INTR_NOTIFY           TEXTEQU         <( 4*32+ 4):( 4*32+ 4)>
;  RWXVF 
;   340 : #define NV_AUDIN_RM_BUFF1_INTR_NOTIFY         ( 4*32+ 5):( 4*32+ 5) /* RWXVF */
NV_AUDIN_RM_BUFF1_INTR_NOTIFY           TEXTEQU         <( 4*32+ 5):( 4*32+ 5)>
;  RWXVF 
;   341 : #define NV_AUDIN_RM_BUFF0_INTR_CHAIN_GAP      ( 4*32+ 6):( 4*32+ 6) /* RWXVF */
NV_AUDIN_RM_BUFF0_INTR_CHAIN_GAP                TEXTEQU         <( 4*32+ 6):( 4*32+ 6)>
;  RWXVF 
;   342 : #define NV_AUDIN_RM_BUFF1_INTR_CHAIN_GAP      ( 4*32+ 7):( 4*32+ 7) /* RWXVF */
NV_AUDIN_RM_BUFF1_INTR_CHAIN_GAP                TEXTEQU         <( 4*32+ 7):( 4*32+ 7)>
;  RWXVF 
;   343 : #define NV_AUDIN_SU_BUFF0_OFFSET              ( 5*32+ 1):( 5*32+ 0) /* RWXVF */
NV_AUDIN_SU_BUFF0_OFFSET                TEXTEQU         <( 5*32+ 1):( 5*32+ 0)>
;  RWXVF 
;   344 : #define NV_AUDIN_SU_BUFF1_OFFSET              ( 5*32+ 3):( 5*32+ 2) /* RWXVF */
NV_AUDIN_SU_BUFF1_OFFSET                TEXTEQU         <( 5*32+ 3):( 5*32+ 2)>
;  RWXVF 
;   345 : #define NV_AUDIN_SU_BYPASS_PTE                ( 5*32+ 4):( 5*32+ 4) /* RWXVF */
NV_AUDIN_SU_BYPASS_PTE          TEXTEQU         <( 5*32+ 4):( 5*32+ 4)>
;  RW--V 
;   346 : #define NV_AUDIN_SU_BYPASS_PTE_DISABLED                  0x00000000 /* RW--V */
NV_AUDIN_SU_BYPASS_PTE_DISABLED         EQU             000000000h
;  RW--V 
;   347 : #define NV_AUDIN_SU_BYPASS_PTE_ENABLED                   0x00000001 /* RW--V */
NV_AUDIN_SU_BYPASS_PTE_ENABLED          EQU             000000001h
;  RWXVF 
;   348 : #define NV_AUDIN_SU_BYPASS_TARGET             ( 5*32+ 6):( 5*32+ 5) /* RWXVF */
NV_AUDIN_SU_BYPASS_TARGET               TEXTEQU         <( 5*32+ 6):( 5*32+ 5)>
;  RW--V 
;   349 : #define NV_AUDIN_SU_BYPASS_TARGET_NVM                    0x00000000 /* RW--V */
NV_AUDIN_SU_BYPASS_TARGET_NVM           EQU             000000000h
;  RW--V 
;   350 : #define NV_AUDIN_SU_BYPASS_TARGET_PCI                    0x00000002 /* RW--V */
NV_AUDIN_SU_BYPASS_TARGET_PCI           EQU             000000002h
;  RW--V 
;   351 : #define NV_AUDIN_SU_BYPASS_TARGET_AGP                    0x00000003 /* RW--V */
NV_AUDIN_SU_BYPASS_TARGET_AGP           EQU             000000003h
;  RWXVF 
;   352 : #define NV_AUDIN_SU_BUFF0_IN_USE              ( 5*32+ 8):( 5*32+ 8) /* RWXVF */
NV_AUDIN_SU_BUFF0_IN_USE                TEXTEQU         <( 5*32+ 8):( 5*32+ 8)>
;  RWXVF 
;   353 : #define NV_AUDIN_SU_BUFF1_IN_USE              ( 5*32+ 9):( 5*32+ 9) /* RWXVF */
NV_AUDIN_SU_BUFF1_IN_USE                TEXTEQU         <( 5*32+ 9):( 5*32+ 9)>
;  RWXVF 
;   354 : #define NV_AUDIN_SU_BUFF0_NOTIFY              (16*32+10):(16*32+10) /* RWXVF */
NV_AUDIN_SU_BUFF0_NOTIFY                TEXTEQU         <(16*32+10):(16*32+10)>
;  RW--V 
;   355 : #define NV_AUDIN_SU_BUFF0_NOTIFY_DMA_WRITE               0x00000000 /* RW--V */
NV_AUDIN_SU_BUFF0_NOTIFY_DMA_WRITE              EQU             000000000h
;  RW--V 
;   356 : #define NV_AUDIN_SU_BUFF0_NOTIFY_INTERRUPT               0x00000001 /* RW--V */
NV_AUDIN_SU_BUFF0_NOTIFY_INTERRUPT              EQU             000000001h
;  RWXVF 
;   357 : #define NV_AUDIN_SU_BUFF1_NOTIFY              (16*32+11):(16*32+11) /* RWXVF */
NV_AUDIN_SU_BUFF1_NOTIFY                TEXTEQU         <(16*32+11):(16*32+11)>
;  RW--V 
;   358 : #define NV_AUDIN_SU_BUFF1_NOTIFY_DMA_WRITE               0x00000000 /* RW--V */
NV_AUDIN_SU_BUFF1_NOTIFY_DMA_WRITE              EQU             000000000h
;  RW--V 
;   359 : #define NV_AUDIN_SU_BUFF1_NOTIFY_INTERRUPT               0x00000001 /* RW--V */
NV_AUDIN_SU_BUFF1_NOTIFY_INTERRUPT              EQU             000000001h
;  RWXVF 
;   360 : #define NV_AUDIN_SU_CHANNEL                   ( 5*32+12):( 5*32+12) /* RWXVF */
NV_AUDIN_SU_CHANNEL             TEXTEQU         <( 5*32+12):( 5*32+12)>
;  RW--V 
;   361 : #define NV_AUDIN_SU_CHANNEL_MONO                         0x00000000 /* RW--V */
NV_AUDIN_SU_CHANNEL_MONO                EQU             000000000h
;  RW--V 
;   362 : #define NV_AUDIN_SU_CHANNEL_STEREO                       0x00000001 /* RW--V */
NV_AUDIN_SU_CHANNEL_STEREO              EQU             000000001h
;  RWXVF 
;   363 : #define NV_AUDIN_SU_FORMAT                    ( 5*32+15):( 5*32+13) /* RWXVF */
NV_AUDIN_SU_FORMAT              TEXTEQU         <( 5*32+15):( 5*32+13)>
;  RW--V 
;   364 : #define NV_AUDIN_SU_FORMAT_LINEAR                        0x00000000 /* RW--V */
NV_AUDIN_SU_FORMAT_LINEAR               EQU             000000000h
;  RW--V 
;   365 : #define NV_AUDIN_SU_FORMAT_ULAW                          0x00000001 /* RW--V */
NV_AUDIN_SU_FORMAT_ULAW         EQU             000000001h
;  RW--V 
;   366 : #define NV_AUDIN_SU_FORMAT_ALAW                          0x00000002 /* RW--V */
NV_AUDIN_SU_FORMAT_ALAW         EQU             000000002h
;  RW--V 
;   367 : #define NV_AUDIN_SU_FORMAT_OFFSET8                       0x00000003 /* RW--V */
NV_AUDIN_SU_FORMAT_OFFSET8              EQU             000000003h
;  dev_ram.ref 
;  RW--M 
;   368 : /* dev_ram.ref */
;   369 : #define NV_AUDOUT                             0x0000003b:0x00000000 /* RW--M */
NV_AUDOUT               TEXTEQU         <0x0000003b:0x00000000>
;  RWWVF 
;   370 : #define NV_AUDOUT_AE_STATE                      (0*32+15):(0*32+ 0) /* RWWVF */
NV_AUDOUT_AE_STATE              TEXTEQU         <(0*32+15):(0*32+ 0)>
;  RWWUF 
;   371 : #define NV_AUDOUT_AE_POSITION_LOW               (0*32+31):(0*32+16) /* RWWUF */
NV_AUDOUT_AE_POSITION_LOW               TEXTEQU         <(0*32+31):(0*32+16)>
;  RWWUF 
;   372 : #define NV_AUDOUT_AE_POSITION                   (1*32+31):(1*32+ 0) /* RWWUF */
NV_AUDOUT_AE_POSITION           TEXTEQU         <(1*32+31):(1*32+ 0)>
;  RWWUF 
;   373 : #define NV_AUDOUT_AE_VOLUME_1                   (2*32+15):(2*32+ 0) /* RWWUF */
NV_AUDOUT_AE_VOLUME_1           TEXTEQU         <(2*32+15):(2*32+ 0)>
;  RWWUF 
;   374 : #define NV_AUDOUT_AE_VOLUME_0                   (2*32+31):(2*32+16) /* RWWUF */
NV_AUDOUT_AE_VOLUME_0           TEXTEQU         <(2*32+31):(2*32+16)>
;  RWWUF 
;   375 : #define NV_AUDOUT_RM_VOLUME_0                   (3*32+15):(3*32+ 0) /* RWWUF */
NV_AUDOUT_RM_VOLUME_0           TEXTEQU         <(3*32+15):(3*32+ 0)>
;  RWWUF 
;   376 : #define NV_AUDOUT_SU_VOLUME_0                   (3*32+31):(3*32+16) /* RWWUF */
NV_AUDOUT_SU_VOLUME_0           TEXTEQU         <(3*32+31):(3*32+16)>
;  RWWUF 
;   377 : #define NV_AUDOUT_RM_VOLUME_1                   (4*32+15):(4*32+ 0) /* RWWUF */
NV_AUDOUT_RM_VOLUME_1           TEXTEQU         <(4*32+15):(4*32+ 0)>
;  RWWUF 
;   378 : #define NV_AUDOUT_SU_VOLUME_1                   (4*32+31):(4*32+16) /* RWWUF */
NV_AUDOUT_SU_VOLUME_1           TEXTEQU         <(4*32+31):(4*32+16)>
;  RWWUF 
;   379 : #define NV_AUDOUT_AE_POSITION_INC               (5*32+31):(5*32+ 0) /* RWWUF */
NV_AUDOUT_AE_POSITION_INC               TEXTEQU         <(5*32+31):(5*32+ 0)>
;  RWWUF 
;   380 : #define NV_AUDOUT_RM_NEXT_INSTANCE              (6*32+31):(6*32+ 0) /* RWWUF */
NV_AUDOUT_RM_NEXT_INSTANCE              TEXTEQU         <(6*32+31):(6*32+ 0)>
;  RWWVF 
;   381 : #define NV_AUDOUT_RM_STATE                      (7*32+15):(7*32+ 0) /* RWWVF */
NV_AUDOUT_RM_STATE              TEXTEQU         <(7*32+15):(7*32+ 0)>
;  RWWVF 
;   382 : #define NV_AUDOUT_SU_STATE                      (8*32+15):(8*32+ 0) /* RWWVF */
NV_AUDOUT_SU_STATE              TEXTEQU         <(8*32+15):(8*32+ 0)>
;  RWWUF 
;   383 : #define NV_AUDOUT_SU_START_TIME_LOW             (8*32+31):(8*32+16) /* RWWUF */
NV_AUDOUT_SU_START_TIME_LOW             TEXTEQU         <(8*32+31):(8*32+16)>
;  RWWUF 
;   384 : #define NV_AUDOUT_SU_START_TIME_HIGH            (9*32+31):(9*32+ 0) /* RWWUF */
NV_AUDOUT_SU_START_TIME_HIGH            TEXTEQU         <(9*32+31):(9*32+ 0)>
;  RWWUF 
;   385 : #define NV_AUDOUT_SU_BUFF0_DMA_INSTANCE       (10*32+15):(10*32+ 0) /* RWWUF */
NV_AUDOUT_SU_BUFF0_DMA_INSTANCE         TEXTEQU         <(10*32+15):(10*32+ 0)>
;  RWWUF 
;   386 : #define NV_AUDOUT_SU_BUFF1_DMA_INSTANCE       (10*32+31):(10*32+16) /* RWWUF */
NV_AUDOUT_SU_BUFF1_DMA_INSTANCE         TEXTEQU         <(10*32+31):(10*32+16)>
;  RWWUF 
;   387 : #define NV_AUDOUT_SU_BUFF0_START_POSITION     (11*32+31):(11*32+ 0) /* RWWUF */
NV_AUDOUT_SU_BUFF0_START_POSITION               TEXTEQU         <(11*32+31):(11*32+ 0)>
;  RWWUF 
;   388 : #define NV_AUDOUT_SU_BUFF0_LAST_PLUS1         (12*32+31):(12*32+ 0) /* RWWUF */
NV_AUDOUT_SU_BUFF0_LAST_PLUS1           TEXTEQU         <(12*32+31):(12*32+ 0)>
;  RWWUF 
;   389 : #define NV_AUDOUT_SU_BUFF1_START_POSITION     (13*32+31):(13*32+ 0) /* RWWUF */
NV_AUDOUT_SU_BUFF1_START_POSITION               TEXTEQU         <(13*32+31):(13*32+ 0)>
;  RWWUF 
;   390 : #define NV_AUDOUT_SU_BUFF1_LAST_PLUS1         (14*32+31):(14*32+ 0) /* RWWUF */
NV_AUDOUT_SU_BUFF1_LAST_PLUS1           TEXTEQU         <(14*32+31):(14*32+ 0)>
;  RWXVF 
;   391 : #define NV_AUDOUT_AE_INTR_MIXING              ( 0*32+ 0):( 0*32+ 0) /* RWXVF */
NV_AUDOUT_AE_INTR_MIXING                TEXTEQU         <( 0*32+ 0):( 0*32+ 0)>
;  RWXVF 
;   392 : #define NV_AUDOUT_AE_INTR_VOLUME              ( 0*32+ 1):( 0*32+ 1) /* RWXVF */
NV_AUDOUT_AE_INTR_VOLUME                TEXTEQU         <( 0*32+ 1):( 0*32+ 1)>
;  RWXVF 
;   393 : #define NV_AUDOUT_AE_EVENT_START              ( 0*32+ 3):( 0*32+ 3) /* RWXVF */
NV_AUDOUT_AE_EVENT_START                TEXTEQU         <( 0*32+ 3):( 0*32+ 3)>
;  RWXVF 
;   394 : #define NV_AUDOUT_AE_BUFF0_INTR_NOTIFY        ( 0*32+ 4):( 0*32+ 4) /* RWXVF */
NV_AUDOUT_AE_BUFF0_INTR_NOTIFY          TEXTEQU         <( 0*32+ 4):( 0*32+ 4)>
;  RWXVF 
;   395 : #define NV_AUDOUT_AE_BUFF1_INTR_NOTIFY        ( 0*32+ 5):( 0*32+ 5) /* RWXVF */
NV_AUDOUT_AE_BUFF1_INTR_NOTIFY          TEXTEQU         <( 0*32+ 5):( 0*32+ 5)>
;  RWXVF 
;   396 : #define NV_AUDOUT_AE_BUFF0_INTR_CHAIN_GAP     ( 0*32+ 6):( 0*32+ 6) /* RWXVF */
NV_AUDOUT_AE_BUFF0_INTR_CHAIN_GAP               TEXTEQU         <( 0*32+ 6):( 0*32+ 6)>
;  RWXVF 
;   397 : #define NV_AUDOUT_AE_BUFF1_INTR_CHAIN_GAP     ( 0*32+ 7):( 0*32+ 7) /* RWXVF */
NV_AUDOUT_AE_BUFF1_INTR_CHAIN_GAP               TEXTEQU         <( 0*32+ 7):( 0*32+ 7)>
;  RWXVF 
;   398 : #define NV_AUDOUT_AE_BUFF0_IN_USE             ( 0*32+ 8):( 0*32+ 8) /* RWXVF */
NV_AUDOUT_AE_BUFF0_IN_USE               TEXTEQU         <( 0*32+ 8):( 0*32+ 8)>
;  RWXVF 
;   399 : #define NV_AUDOUT_AE_BUFF1_IN_USE             ( 0*32+ 9):( 0*32+ 9) /* RWXVF */
NV_AUDOUT_AE_BUFF1_IN_USE               TEXTEQU         <( 0*32+ 9):( 0*32+ 9)>
;  RWXVF 
;   400 : #define NV_AUDOUT_AE_GAP_DETECT               ( 0*32+10):( 0*32+10) /* RWXVF */
NV_AUDOUT_AE_GAP_DETECT         TEXTEQU         <( 0*32+10):( 0*32+10)>
;  RWXVF 
;   401 : #define NV_AUDOUT_AE_INTR_DMA                 ( 0*32+11):( 0*32+11) /* RWXVF */
NV_AUDOUT_AE_INTR_DMA           TEXTEQU         <( 0*32+11):( 0*32+11)>
;  RWXVF 
;   402 : #define NV_AUDOUT_AE_CURRENT_BUFFER           ( 0*32+12):( 0*32+12) /* RWXVF */
NV_AUDOUT_AE_CURRENT_BUFFER             TEXTEQU         <( 0*32+12):( 0*32+12)>
;  RW--V 
;   403 : #define NV_AUDOUT_AE_CURRENT_BUFFER_0                    0x00000000 /* RW--V */
NV_AUDOUT_AE_CURRENT_BUFFER_0           EQU             000000000h
;  RW--V 
;   404 : #define NV_AUDOUT_AE_CURRENT_BUFFER_1                    0x00000001 /* RW--V */
NV_AUDOUT_AE_CURRENT_BUFFER_1           EQU             000000001h
;  RWXVF 
;   405 : #define NV_AUDOUT_RM_INTR_MIXING              (15*32+ 0):(15*32+ 0) /* RWXVF */
NV_AUDOUT_RM_INTR_MIXING                TEXTEQU         <(15*32+ 0):(15*32+ 0)>
;  RWXVF 
;   406 : #define NV_AUDOUT_RM_INTR_VOLUME              (15*32+ 1):(15*32+ 1) /* RWXVF */
NV_AUDOUT_RM_INTR_VOLUME                TEXTEQU         <(15*32+ 1):(15*32+ 1)>
;  RWXVF 
;   407 : #define NV_AUDOUT_RM_BUFF0_INTR_NOTIFY        (15*32+ 4):(15*32+ 4) /* RWXVF */
NV_AUDOUT_RM_BUFF0_INTR_NOTIFY          TEXTEQU         <(15*32+ 4):(15*32+ 4)>
;  RWXVF 
;   408 : #define NV_AUDOUT_RM_BUFF1_INTR_NOTIFY        (15*32+ 5):(15*32+ 5) /* RWXVF */
NV_AUDOUT_RM_BUFF1_INTR_NOTIFY          TEXTEQU         <(15*32+ 5):(15*32+ 5)>
;  RWXVF 
;   409 : #define NV_AUDOUT_RM_BUFF0_INTR_CHAIN_GAP     (15*32+ 6):(15*32+ 6) /* RWXVF */
NV_AUDOUT_RM_BUFF0_INTR_CHAIN_GAP               TEXTEQU         <(15*32+ 6):(15*32+ 6)>
;  RWXVF 
;   410 : #define NV_AUDOUT_RM_BUFF1_INTR_CHAIN_GAP     (15*32+ 7):(15*32+ 7) /* RWXVF */
NV_AUDOUT_RM_BUFF1_INTR_CHAIN_GAP               TEXTEQU         <(15*32+ 7):(15*32+ 7)>
;  RWXVF 
;   411 : #define NV_AUDOUT_SU_BUFF0_OFFSET             (16*32+ 1):(16*32+ 0) /* RWXVF */
NV_AUDOUT_SU_BUFF0_OFFSET               TEXTEQU         <(16*32+ 1):(16*32+ 0)>
;  RWXVF 
;   412 : #define NV_AUDOUT_SU_BUFF1_OFFSET             (16*32+ 3):(16*32+ 2) /* RWXVF */
NV_AUDOUT_SU_BUFF1_OFFSET               TEXTEQU         <(16*32+ 3):(16*32+ 2)>
;  RWXVF 
;   413 : #define NV_AUDOUT_SU_BYPASS_PTE               (16*32+ 4):(16*32+ 4) /* RWXVF */
NV_AUDOUT_SU_BYPASS_PTE         TEXTEQU         <(16*32+ 4):(16*32+ 4)>
;  RW--V 
;   414 : #define NV_AUDOUT_SU_BYPASS_PTE_DISABLED                 0x00000000 /* RW--V */
NV_AUDOUT_SU_BYPASS_PTE_DISABLED                EQU             000000000h
;  RW--V 
;   415 : #define NV_AUDOUT_SU_BYPASS_PTE_ENABLED                  0x00000001 /* RW--V */
NV_AUDOUT_SU_BYPASS_PTE_ENABLED         EQU             000000001h
;  RWXVF 
;   416 : #define NV_AUDOUT_SU_BYPASS_TARGET            (16*32+ 6):(16*32+ 5) /* RWXVF */
NV_AUDOUT_SU_BYPASS_TARGET              TEXTEQU         <(16*32+ 6):(16*32+ 5)>
;  RW--V 
;   417 : #define NV_AUDOUT_SU_BYPASS_TARGET_NVM                   0x00000000 /* RW--V */
NV_AUDOUT_SU_BYPASS_TARGET_NVM          EQU             000000000h
;  RW--V 
;   418 : #define NV_AUDOUT_SU_BYPASS_TARGET_PCI                   0x00000002 /* RW--V */
NV_AUDOUT_SU_BYPASS_TARGET_PCI          EQU             000000002h
;  RW--V 
;   419 : #define NV_AUDOUT_SU_BYPASS_TARGET_AGP                   0x00000003 /* RW--V */
NV_AUDOUT_SU_BYPASS_TARGET_AGP          EQU             000000003h
;  RWXVF 
;   420 : #define NV_AUDOUT_SU_BUFF0_IN_USE             (16*32+ 8):(16*32+ 8) /* RWXVF */
NV_AUDOUT_SU_BUFF0_IN_USE               TEXTEQU         <(16*32+ 8):(16*32+ 8)>
;  RWXVF 
;   421 : #define NV_AUDOUT_SU_BUFF1_IN_USE             (16*32+ 9):(16*32+ 9) /* RWXVF */
NV_AUDOUT_SU_BUFF1_IN_USE               TEXTEQU         <(16*32+ 9):(16*32+ 9)>
;  RWXVF 
;   422 : #define NV_AUDOUT_SU_BUFF0_NOTIFY             (16*32+10):(16*32+10) /* RWXVF */
NV_AUDOUT_SU_BUFF0_NOTIFY               TEXTEQU         <(16*32+10):(16*32+10)>
;  RW--V 
;   423 : #define NV_AUDOUT_SU_BUFF0_NOTIFY_DMA_WRITE              0x00000000 /* RW--V */
NV_AUDOUT_SU_BUFF0_NOTIFY_DMA_WRITE             EQU             000000000h
;  RW--V 
;   424 : #define NV_AUDOUT_SU_BUFF0_NOTIFY_INTERRUPT              0x00000001 /* RW--V */
NV_AUDOUT_SU_BUFF0_NOTIFY_INTERRUPT             EQU             000000001h
;  RWXVF 
;   425 : #define NV_AUDOUT_SU_BUFF1_NOTIFY             (16*32+11):(16*32+11) /* RWXVF */
NV_AUDOUT_SU_BUFF1_NOTIFY               TEXTEQU         <(16*32+11):(16*32+11)>
;  RW--V 
;   426 : #define NV_AUDOUT_SU_BUFF1_NOTIFY_DMA_WRITE              0x00000000 /* RW--V */
NV_AUDOUT_SU_BUFF1_NOTIFY_DMA_WRITE             EQU             000000000h
;  RW--V 
;   427 : #define NV_AUDOUT_SU_BUFF1_NOTIFY_INTERRUPT              0x00000001 /* RW--V */
NV_AUDOUT_SU_BUFF1_NOTIFY_INTERRUPT             EQU             000000001h
;  RWXVF 
;   428 : #define NV_AUDOUT_SU_CHANNEL                  (16*32+12):(16*32+12) /* RWXVF */
NV_AUDOUT_SU_CHANNEL            TEXTEQU         <(16*32+12):(16*32+12)>
;  RW--V 
;   429 : #define NV_AUDOUT_SU_CHANNEL_MONO                        0x00000000 /* RW--V */
NV_AUDOUT_SU_CHANNEL_MONO               EQU             000000000h
;  RW--V 
;   430 : #define NV_AUDOUT_SU_CHANNEL_STEREO                      0x00000001 /* RW--V */
NV_AUDOUT_SU_CHANNEL_STEREO             EQU             000000001h
;  RWXVF 
;   431 : #define NV_AUDOUT_SU_FORMAT                   (16*32+15):(16*32+13) /* RWXVF */
NV_AUDOUT_SU_FORMAT             TEXTEQU         <(16*32+15):(16*32+13)>
;  RW--V 
;   432 : #define NV_AUDOUT_SU_FORMAT_LINEAR                       0x00000000 /* RW--V */
NV_AUDOUT_SU_FORMAT_LINEAR              EQU             000000000h
;  RW--V 
;   433 : #define NV_AUDOUT_SU_FORMAT_ULAW                         0x00000001 /* RW--V */
NV_AUDOUT_SU_FORMAT_ULAW                EQU             000000001h
;  RW--V 
;   434 : #define NV_AUDOUT_SU_FORMAT_ALAW                         0x00000002 /* RW--V */
NV_AUDOUT_SU_FORMAT_ALAW                EQU             000000002h
;  RW--V 
;   435 : #define NV_AUDOUT_SU_FORMAT_OFFSET8                      0x00000003 /* RW--V */
NV_AUDOUT_SU_FORMAT_OFFSET8             EQU             000000003h
;  RWXVF 
;  dev_ram.ref 
;  RW--M 
;   436 : #define NV_AUDOUT_SU_BUFF0_OFFSET             (16*32+ 1):(16*32+ 0) /* RWXVF */
;   437 : /* dev_ram.ref */
;   438 : #define NV_AUDNOTE                            0x00000083:0x00000000 /* RW--M */
NV_AUDNOTE              TEXTEQU         <0x00000083:0x00000000>
;  RWWVF 
;   439 : #define NV_AUDNOTE_AE_STATE                     (0*32+15):(0*32+ 0) /* RWWVF */
NV_AUDNOTE_AE_STATE             TEXTEQU         <(0*32+15):(0*32+ 0)>
;  RWWUF 
;   440 : #define NV_AUDNOTE_AE_POSITION_LOW              (0*32+31):(0*32+16) /* RWWUF */
NV_AUDNOTE_AE_POSITION_LOW              TEXTEQU         <(0*32+31):(0*32+16)>
;  RWWUF 
;   441 : #define NV_AUDNOTE_AE_POSITION                  (1*32+31):(1*32+ 0) /* RWWUF */
NV_AUDNOTE_AE_POSITION          TEXTEQU         <(1*32+31):(1*32+ 0)>
;  RWWUF 
;   442 : #define NV_AUDNOTE_AE_POSITION_INC              (2*32+31):(2*32+ 0) /* RWWUF */
NV_AUDNOTE_AE_POSITION_INC              TEXTEQU         <(2*32+31):(2*32+ 0)>
;  RWWUF 
;   443 : #define NV_AUDNOTE_AE_EG_ADSR_LEVEL             (3*32+31):(3*32+ 0) /* RWWUF */
NV_AUDNOTE_AE_EG_ADSR_LEVEL             TEXTEQU         <(3*32+31):(3*32+ 0)>
;  RWWUF 
;   444 : #define NV_AUDNOTE_AE_PE_ADSR_LEVEL             (4*32+31):(4*32+ 0) /* RWWUF */
NV_AUDNOTE_AE_PE_ADSR_LEVEL             TEXTEQU         <(4*32+31):(4*32+ 0)>
;  RWWUF 
;   445 : #define NV_AUDNOTE_AE_ADSR_VOLUME               (5*32+15):(5*32+ 0) /* RWWUF */
NV_AUDNOTE_AE_ADSR_VOLUME               TEXTEQU         <(5*32+15):(5*32+ 0)>
;  RWWUF 
;   446 : #define NV_AUDNOTE_AE_VIBRATO_TREMOLO_LEVEL     (5*32+31):(5*32+16) /* RWWUF */
NV_AUDNOTE_AE_VIBRATO_TREMOLO_LEVEL             TEXTEQU         <(5*32+31):(5*32+16)>
;  RWWUF 
;   447 : #define NV_AUDNOTE_AE_VIBRATO_POSITION          (6*32+31):(6*32+ 0) /* RWWUF */
NV_AUDNOTE_AE_VIBRATO_POSITION          TEXTEQU         <(6*32+31):(6*32+ 0)>
;  RWWUF 
;   448 : #define NV_AUDNOTE_AE_TREMOLO_POSITION          (7*32+31):(7*32+ 0) /* RWWUF */
NV_AUDNOTE_AE_TREMOLO_POSITION          TEXTEQU         <(7*32+31):(7*32+ 0)>
;  RWWUF 
;   449 : #define NV_AUDNOTE_AE_VOLUME_1                  (8*32+15):(8*32+ 0) /* RWWUF */
NV_AUDNOTE_AE_VOLUME_1          TEXTEQU         <(8*32+15):(8*32+ 0)>
;  RWWUF 
;   450 : #define NV_AUDNOTE_AE_VOLUME_0                  (8*32+31):(8*32+16) /* RWWUF */
NV_AUDNOTE_AE_VOLUME_0          TEXTEQU         <(8*32+31):(8*32+16)>
;  RWWUF 
;   451 : #define NV_AUDNOTE_RM_NEXT_INSTANCE             (9*32+31):(9*32+ 0) /* RWWUF */
NV_AUDNOTE_RM_NEXT_INSTANCE             TEXTEQU         <(9*32+31):(9*32+ 0)>
;  RWWUF 
;   452 : #define NV_AUDNOTE_RM_VOLUME_0                (10*32+15):(10*32+ 0) /* RWWUF */
NV_AUDNOTE_RM_VOLUME_0          TEXTEQU         <(10*32+15):(10*32+ 0)>
;  RWWUF 
;   453 : #define NV_AUDNOTE_SU_VOLUME_0                (10*32+31):(10*32+16) /* RWWUF */
NV_AUDNOTE_SU_VOLUME_0          TEXTEQU         <(10*32+31):(10*32+16)>
;  RWWUF 
;   454 : #define NV_AUDNOTE_RM_VOLUME_1                (11*32+15):(11*32+ 0) /* RWWUF */
NV_AUDNOTE_RM_VOLUME_1          TEXTEQU         <(11*32+15):(11*32+ 0)>
;  RWWUF 
;   455 : #define NV_AUDNOTE_SU_VOLUME_1                (11*32+31):(11*32+16) /* RWWUF */
NV_AUDNOTE_SU_VOLUME_1          TEXTEQU         <(11*32+31):(11*32+16)>
;  RWWVF 
;   456 : #define NV_AUDNOTE_RM_STATE                   (12*32+15):(12*32+ 0) /* RWWVF */
NV_AUDNOTE_RM_STATE             TEXTEQU         <(12*32+15):(12*32+ 0)>
;  RWWUF 
;   457 : #define NV_AUDNOTE_SU_STOP_TIME_LOW           (12*32+31):(12*32+16) /* RWWUF */
NV_AUDNOTE_SU_STOP_TIME_LOW             TEXTEQU         <(12*32+31):(12*32+16)>
;  RWWVF 
;   458 : #define NV_AUDNOTE_SU_STATE                   (13*32+15):(13*32+ 0) /* RWWVF */
NV_AUDNOTE_SU_STATE             TEXTEQU         <(13*32+15):(13*32+ 0)>
;  RWWUF 
;   459 : #define NV_AUDNOTE_SU_START_TIME_LOW          (13*32+31):(13*32+16) /* RWWUF */
NV_AUDNOTE_SU_START_TIME_LOW            TEXTEQU         <(13*32+31):(13*32+16)>
;  RWWUF 
;   460 : #define NV_AUDNOTE_SU_START_TIME_HIGH         (14*32+31):(14*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_START_TIME_HIGH           TEXTEQU         <(14*32+31):(14*32+ 0)>
;  RWWUF 
;   461 : #define NV_AUDNOTE_SU_STOP_TIME_HIGH          (15*32+31):(15*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_STOP_TIME_HIGH            TEXTEQU         <(15*32+31):(15*32+ 0)>
;  RWWVF 
;   462 : #define NV_AUDNOTE_SU_FM_STATE                (16*32+15):(16*32+ 0) /* RWWVF */
NV_AUDNOTE_SU_FM_STATE          TEXTEQU         <(16*32+15):(16*32+ 0)>
;  RWWUF 
;   463 : #define NV_AUDNOTE_SU_RELEASE_TIME_LOW        (16*32+31):(16*32+16) /* RWWUF */
NV_AUDNOTE_SU_RELEASE_TIME_LOW          TEXTEQU         <(16*32+31):(16*32+16)>
;  RWWUF 
;   464 : #define NV_AUDNOTE_SU_DMA_INSTANCE            (17*32+15):(17*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_DMA_INSTANCE              TEXTEQU         <(17*32+15):(17*32+ 0)>
;  RWWUF 
;   465 : #define NV_AUDNOTE_SU_VIBRATO_TREMOLO_TIME_LO (17*32+31):(17*32+16) /* RWWUF */
NV_AUDNOTE_SU_VIBRATO_TREMOLO_TIME_LO           TEXTEQU         <(17*32+31):(17*32+16)>
;  RWWUF 
;   466 : #define NV_AUDNOTE_SU_VIBRATO_TREMOLO_TIME_HI (18*32+31):(18*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_VIBRATO_TREMOLO_TIME_HI           TEXTEQU         <(18*32+31):(18*32+ 0)>
;  RWWUF 
;   467 : #define NV_AUDNOTE_SU_RELEASE_TIME_HIGH       (19*32+31):(19*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_RELEASE_TIME_HIGH         TEXTEQU         <(19*32+31):(19*32+ 0)>
;  RWWUF 
;   468 : #define NV_AUDNOTE_SU_POSITION_INC            (20*32+31):(20*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_POSITION_INC              TEXTEQU         <(20*32+31):(20*32+ 0)>
;  RWWUF 
;   469 : #define NV_AUDNOTE_SU_LOOP_START              (21*32+31):(21*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_LOOP_START                TEXTEQU         <(21*32+31):(21*32+ 0)>
;  RWWUF 
;   470 : #define NV_AUDNOTE_SU_LOOP_END_PLUS1          (22*32+31):(22*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_LOOP_END_PLUS1            TEXTEQU         <(22*32+31):(22*32+ 0)>
;  RWWUF 
;   471 : #define NV_AUDNOTE_SU_LAST_PLUS1              (23*32+31):(23*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_LAST_PLUS1                TEXTEQU         <(23*32+31):(23*32+ 0)>
;  RWWUF 
;   472 : #define NV_AUDNOTE_SU_EG_ATTACK_RATE          (24*32+15):(24*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_EG_ATTACK_RATE            TEXTEQU         <(24*32+15):(24*32+ 0)>
;  RWWUF 
;   473 : #define NV_AUDNOTE_SU_EG_DECAY_RATE           (24*32+31):(24*32+16) /* RWWUF */
NV_AUDNOTE_SU_EG_DECAY_RATE             TEXTEQU         <(24*32+31):(24*32+16)>
;  RWWUF 
;   474 : #define NV_AUDNOTE_SU_EG_SUSTAIN_RATE         (25*32+15):(25*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_EG_SUSTAIN_RATE           TEXTEQU         <(25*32+15):(25*32+ 0)>
;  RWWUF 
;   475 : #define NV_AUDNOTE_SU_EG_RELEASE_RATE         (25*32+31):(25*32+16) /* RWWUF */
NV_AUDNOTE_SU_EG_RELEASE_RATE           TEXTEQU         <(25*32+31):(25*32+16)>
;  RWWUF 
;   476 : #define NV_AUDNOTE_SU_PE_ATTACK_RATE          (26*32+15):(26*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_PE_ATTACK_RATE            TEXTEQU         <(26*32+15):(26*32+ 0)>
;  RWWUF 
;   477 : #define NV_AUDNOTE_SU_PE_DECAY_RATE           (26*32+31):(26*32+16) /* RWWUF */
NV_AUDNOTE_SU_PE_DECAY_RATE             TEXTEQU         <(26*32+31):(26*32+16)>
;  RWWUF 
;   478 : #define NV_AUDNOTE_SU_PE_SUSTAIN_RATE         (27*32+15):(27*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_PE_SUSTAIN_RATE           TEXTEQU         <(27*32+15):(27*32+ 0)>
;  RWWUF 
;   479 : #define NV_AUDNOTE_SU_PE_RELEASE_RATE         (27*32+31):(27*32+16) /* RWWUF */
NV_AUDNOTE_SU_PE_RELEASE_RATE           TEXTEQU         <(27*32+31):(27*32+16)>
;  RWWUF 
;   480 : #define NV_AUDNOTE_SU_VIBRATO_TREMOLO_ATTACK  (28*32+15):(28*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_VIBRATO_TREMOLO_ATTACK            TEXTEQU         <(28*32+15):(28*32+ 0)>
;  RWWUF 
;   481 : #define NV_AUDNOTE_SU_VIBRATO_DEPTH           (29*32+15):(29*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_VIBRATO_DEPTH             TEXTEQU         <(29*32+15):(29*32+ 0)>
;  RWWUF 
;   482 : #define NV_AUDNOTE_SU_EG_SUSTAIN_LEVEL        (29*32+31):(29*32+16) /* RWWUF */
NV_AUDNOTE_SU_EG_SUSTAIN_LEVEL          TEXTEQU         <(29*32+31):(29*32+16)>
;  RWWUF 
;   483 : #define NV_AUDNOTE_SU_PE_SCALE                (30*32+15):(30*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_PE_SCALE          TEXTEQU         <(30*32+15):(30*32+ 0)>
;  RWWUF 
;   484 : #define NV_AUDNOTE_SU_PE_SUSTAIN_LEVEL        (30*32+31):(30*32+16) /* RWWUF */
NV_AUDNOTE_SU_PE_SUSTAIN_LEVEL          TEXTEQU         <(30*32+31):(30*32+16)>
;  RWWUF 
;   485 : #define NV_AUDNOTE_SU_TREMOLO_POSITION_INC    (31*32+15):(31*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_TREMOLO_POSITION_INC              TEXTEQU         <(31*32+15):(31*32+ 0)>
;  RWWUF 
;   486 : #define NV_AUDNOTE_SU_VIBRATO_POSITION_INC    (31*32+31):(31*32+16) /* RWWUF */
NV_AUDNOTE_SU_VIBRATO_POSITION_INC              TEXTEQU         <(31*32+31):(31*32+16)>
;  RWWUF 
;   487 : #define NV_AUDNOTE_SU_TREMOLO_DEPTH           (32*32+15):(32*32+ 0) /* RWWUF */
NV_AUDNOTE_SU_TREMOLO_DEPTH             TEXTEQU         <(32*32+15):(32*32+ 0)>
;  RWWUF 
;   488 : #define NV_AUDNOTE_SU_TREMOLO_OFFSET          (32*32+31):(32*32+16) /* RWWUF */
NV_AUDNOTE_SU_TREMOLO_OFFSET            TEXTEQU         <(32*32+31):(32*32+16)>
;  RWXVF 
;   489 : #define NV_AUDNOTE_AE_INTR_MIXING             ( 0*32+ 0):( 0*32+ 0) /* RWXVF */
NV_AUDNOTE_AE_INTR_MIXING               TEXTEQU         <( 0*32+ 0):( 0*32+ 0)>
;  RWXVF 
;   490 : #define NV_AUDNOTE_AE_INTR_VOLUME             ( 0*32+ 1):( 0*32+ 1) /* RWXVF */
NV_AUDNOTE_AE_INTR_VOLUME               TEXTEQU         <( 0*32+ 1):( 0*32+ 1)>
;  RWXVF 
;   491 : #define NV_AUDNOTE_AE_EVENT_STOP              ( 0*32+ 2):( 0*32+ 2) /* RWXVF */
NV_AUDNOTE_AE_EVENT_STOP                TEXTEQU         <( 0*32+ 2):( 0*32+ 2)>
;  RWXVF 
;   492 : #define NV_AUDNOTE_AE_EVENT_START             ( 0*32+ 3):( 0*32+ 3) /* RWXVF */
NV_AUDNOTE_AE_EVENT_START               TEXTEQU         <( 0*32+ 3):( 0*32+ 3)>
;  RWXVF 
;   493 : #define NV_AUDNOTE_AE_INTR_NOTIFY             ( 0*32+ 4):( 0*32+ 4) /* RWXVF */
NV_AUDNOTE_AE_INTR_NOTIFY               TEXTEQU         <( 0*32+ 4):( 0*32+ 4)>
;  RWXVF 
;   494 : #define NV_AUDNOTE_AE_PE_STATE                ( 0*32+ 7):( 0*32+ 6) /* RWXVF */
NV_AUDNOTE_AE_PE_STATE          TEXTEQU         <( 0*32+ 7):( 0*32+ 6)>
;  RW--V 
;   495 : #define NV_AUDNOTE_AE_PE_ATTACK                          0x00000000 /* RW--V */
NV_AUDNOTE_AE_PE_ATTACK         EQU             000000000h
;  RW--V 
;   496 : #define NV_AUDNOTE_AE_PE_DECAY                           0x00000001 /* RW--V */
NV_AUDNOTE_AE_PE_DECAY          EQU             000000001h
;  RW--V 
;   497 : #define NV_AUDNOTE_AE_PE_SUSTAIN                         0x00000002 /* RW--V */
NV_AUDNOTE_AE_PE_SUSTAIN                EQU             000000002h
;  RW--V 
;   498 : #define NV_AUDNOTE_AE_PE_RELEASE                         0x00000003 /* RW--V */
NV_AUDNOTE_AE_PE_RELEASE                EQU             000000003h
;  RWXVF 
;   499 : #define NV_AUDNOTE_AE_IN_USE                  ( 0*32+ 8):( 0*32+ 8) /* RWXVF */
NV_AUDNOTE_AE_IN_USE            TEXTEQU         <( 0*32+ 8):( 0*32+ 8)>
;  RWXVF 
;   500 : #define NV_AUDNOTE_AE_GAP_DETECT              ( 0*32+10):( 0*32+10) /* RWXVF */
NV_AUDNOTE_AE_GAP_DETECT                TEXTEQU         <( 0*32+10):( 0*32+10)>
;  RWXVF 
;   501 : #define NV_AUDNOTE_AE_INTR_DMA                ( 0*32+11):( 0*32+11) /* RWXVF */
NV_AUDNOTE_AE_INTR_DMA          TEXTEQU         <( 0*32+11):( 0*32+11)>
;  RWXVF 
;   502 : #define NV_AUDNOTE_AE_EG_STATE                ( 0*32+14):( 0*32+13) /* RWXVF */
NV_AUDNOTE_AE_EG_STATE          TEXTEQU         <( 0*32+14):( 0*32+13)>
;  RW--V 
;   503 : #define NV_AUDNOTE_AE_EG_ATTACK                          0x00000000 /* RW--V */
NV_AUDNOTE_AE_EG_ATTACK         EQU             000000000h
;  RW--V 
;   504 : #define NV_AUDNOTE_AE_EG_DECAY                           0x00000001 /* RW--V */
NV_AUDNOTE_AE_EG_DECAY          EQU             000000001h
;  RW--V 
;   505 : #define NV_AUDNOTE_AE_EG_SUSTAIN                         0x00000002 /* RW--V */
NV_AUDNOTE_AE_EG_SUSTAIN                EQU             000000002h
;  RW--V 
;   506 : #define NV_AUDNOTE_AE_EG_RELEASE                         0x00000003 /* RW--V */
NV_AUDNOTE_AE_EG_RELEASE                EQU             000000003h
;  RWXVF 
;   507 : #define NV_AUDNOTE_AE_POS_VALID               ( 0*32+15):( 0*32+15) /* RWXVF */
NV_AUDNOTE_AE_POS_VALID         TEXTEQU         <( 0*32+15):( 0*32+15)>
;  RWXVF 
;   508 : #define NV_AUDNOTE_RM_INTR_MIXING             (14*32+ 0):(14*32+ 0) /* RWXVF */
NV_AUDNOTE_RM_INTR_MIXING               TEXTEQU         <(14*32+ 0):(14*32+ 0)>
;  RWXVF 
;   509 : #define NV_AUDNOTE_RM_INTR_VOLUME             (14*32+ 1):(14*32+ 1) /* RWXVF */
NV_AUDNOTE_RM_INTR_VOLUME               TEXTEQU         <(14*32+ 1):(14*32+ 1)>
;  RWXVF 
;   510 : #define NV_AUDNOTE_RM_INTR_NOTIFY             (14*32+ 4):(14*32+ 4) /* RWXVF */
NV_AUDNOTE_RM_INTR_NOTIFY               TEXTEQU         <(14*32+ 4):(14*32+ 4)>
;  RWXVF 
;   511 : #define NV_AUDNOTE_SU_BYPASS_PTE              (12*32+ 4):(12*32+ 4) /* RWXVF */
NV_AUDNOTE_SU_BYPASS_PTE                TEXTEQU         <(12*32+ 4):(12*32+ 4)>
;  RW--V 
;   512 : #define NV_AUDNOTE_SU_BYPASS_PTE_DISABLED                0x00000000 /* RW--V */
NV_AUDNOTE_SU_BYPASS_PTE_DISABLED               EQU             000000000h
;  RW--V 
;   513 : #define NV_AUDNOTE_SU_BYPASS_PTE_ENABLED                 0x00000001 /* RW--V */
NV_AUDNOTE_SU_BYPASS_PTE_ENABLED                EQU             000000001h
;  RWXVF 
;   514 : #define NV_AUDNOTE_SU_BYPASS_TARGET           (12*32+ 6):(12*32+ 5) /* RWXVF */
NV_AUDNOTE_SU_BYPASS_TARGET             TEXTEQU         <(12*32+ 6):(12*32+ 5)>
;  RW--V 
;   515 : #define NV_AUDNOTE_SU_BYPASS_TARGET_NVM                  0x00000000 /* RW--V */
NV_AUDNOTE_SU_BYPASS_TARGET_NVM         EQU             000000000h
;  RW--V 
;   516 : #define NV_AUDNOTE_SU_BYPASS_TARGET_PCI                  0x00000002 /* RW--V */
NV_AUDNOTE_SU_BYPASS_TARGET_PCI         EQU             000000002h
;  RW--V 
;   517 : #define NV_AUDNOTE_SU_BYPASS_TARGET_AGP                  0x00000003 /* RW--V */
NV_AUDNOTE_SU_BYPASS_TARGET_AGP         EQU             000000003h
;  RWXVF 
;   518 : #define NV_AUDNOTE_SU_EG_LINEAR               (12*32+ 7):(12*32+ 7) /* RWXVF */
NV_AUDNOTE_SU_EG_LINEAR         TEXTEQU         <(12*32+ 7):(12*32+ 7)>
;  RW--V 
;   519 : #define NV_AUDNOTE_SU_EG_ATCK_NV1                        0x00000000 /* RW--V */
NV_AUDNOTE_SU_EG_ATCK_NV1               EQU             000000000h
;  RW--V 
;   520 : #define NV_AUDNOTE_SU_EG_ATCK_LINEAR                     0x00000001 /* RW--V */
NV_AUDNOTE_SU_EG_ATCK_LINEAR            EQU             000000001h
;  RWXVF 
;   521 : #define NV_AUDNOTE_SU_IN_USE                  (12*32+ 8):(12*32+ 8) /* RWXVF */
NV_AUDNOTE_SU_IN_USE            TEXTEQU         <(12*32+ 8):(12*32+ 8)>
;  RWXVF 
;   522 : #define NV_AUDNOTE_SU_BUFF_NOTIFY             (16*32+10):(16*32+10) /* RWXVF */
NV_AUDNOTE_SU_BUFF_NOTIFY               TEXTEQU         <(16*32+10):(16*32+10)>
;  RW--V 
;   523 : #define NV_AUDNOTE_SU_BUFF_NOTIFY_DMA_WRITE              0x00000000 /* RW--V */
NV_AUDNOTE_SU_BUFF_NOTIFY_DMA_WRITE             EQU             000000000h
;  RW--V 
;   524 : #define NV_AUDNOTE_SU_BUFF_NOTIFY_INTERRUPT              0x00000001 /* RW--V */
NV_AUDNOTE_SU_BUFF_NOTIFY_INTERRUPT             EQU             000000001h
;  RWXVF 
;   525 : #define NV_AUDNOTE_SU_CHANNEL                 (12*32+12):(12*32+12) /* RWXVF */
NV_AUDNOTE_SU_CHANNEL           TEXTEQU         <(12*32+12):(12*32+12)>
;  RW--V 
;   526 : #define NV_AUDNOTE_SU_CHANNEL_MONO                       0x00000000 /* RW--V */
NV_AUDNOTE_SU_CHANNEL_MONO              EQU             000000000h
;  RW--V 
;   527 : #define NV_AUDNOTE_SU_CHANNEL_STEREO                     0x00000001 /* RW--V */
NV_AUDNOTE_SU_CHANNEL_STEREO            EQU             000000001h
;  RWXVF 
;   528 : #define NV_AUDNOTE_SU_FORMAT                  (12*32+15):(12*32+13) /* RWXVF */
NV_AUDNOTE_SU_FORMAT            TEXTEQU         <(12*32+15):(12*32+13)>
;  RW--V 
;   529 : #define NV_AUDNOTE_SU_FORMAT_LINEAR                      0x00000000 /* RW--V */
NV_AUDNOTE_SU_FORMAT_LINEAR             EQU             000000000h
;  RW--V 
;   530 : #define NV_AUDNOTE_SU_FORMAT_ULAW                        0x00000001 /* RW--V */
NV_AUDNOTE_SU_FORMAT_ULAW               EQU             000000001h
;  RW--V 
;   531 : #define NV_AUDNOTE_SU_FORMAT_ALAW                        0x00000002 /* RW--V */
NV_AUDNOTE_SU_FORMAT_ALAW               EQU             000000002h
;  RW--V 
;   532 : #define NV_AUDNOTE_SU_FORMAT_OFFSET8                     0x00000003 /* RW--V */
NV_AUDNOTE_SU_FORMAT_OFFSET8            EQU             000000003h
;  RWXVF 
;   533 : #define NV_AUDNOTE_FM_FEEDBACK                (15*32+ 6):(15*32+ 4) /* RWXVF */
NV_AUDNOTE_FM_FEEDBACK          TEXTEQU         <(15*32+ 6):(15*32+ 4)>
;  RW--V 
;   534 : #define NV_AUDNOTE_FM_FB_0                               0x00000000 /* RW--V */
NV_AUDNOTE_FM_FB_0              EQU             000000000h
;  RW--V 
;   535 : #define NV_AUDNOTE_FM_FB_PI_16                           0x00000001 /* RW--V */
NV_AUDNOTE_FM_FB_PI_16          EQU             000000001h
;  RW--V 
;   536 : #define NV_AUDNOTE_FM_FB_PI_8                            0x00000002 /* RW--V */
NV_AUDNOTE_FM_FB_PI_8           EQU             000000002h
;  RW--V 
;   537 : #define NV_AUDNOTE_FM_FB_PI_4                            0x00000003 /* RW--V */
NV_AUDNOTE_FM_FB_PI_4           EQU             000000003h
;  RW--V 
;   538 : #define NV_AUDNOTE_FM_FB_PI_2                            0x00000004 /* RW--V */
NV_AUDNOTE_FM_FB_PI_2           EQU             000000004h
;  RW--V 
;   539 : #define NV_AUDNOTE_FM_FB_1_PI                            0x00000005 /* RW--V */
NV_AUDNOTE_FM_FB_1_PI           EQU             000000005h
;  RW--V 
;   540 : #define NV_AUDNOTE_FM_FB_2_PI                            0x00000006 /* RW--V */
NV_AUDNOTE_FM_FB_2_PI           EQU             000000006h
;  RW--V 
;   541 : #define NV_AUDNOTE_FM_FB_4_PI                            0x00000007 /* RW--V */
NV_AUDNOTE_FM_FB_4_PI           EQU             000000007h
;  RWXVF 
;   542 : #define NV_AUDNOTE_FM_WAVE_SELECT             (15*32+ 2):(15*32+ 0) /* RWXVF */
NV_AUDNOTE_FM_WAVE_SELECT               TEXTEQU         <(15*32+ 2):(15*32+ 0)>
;  RW--V 
;   543 : #define NV_AUDNOTE_FM_WS_SINE                            0x00000000 /* RW--V */
NV_AUDNOTE_FM_WS_SINE           EQU             000000000h
;  RW--V 
;   544 : #define NV_AUDNOTE_FM_WS_HALF_SINE                       0x00000001 /* RW--V */
NV_AUDNOTE_FM_WS_HALF_SINE              EQU             000000001h
;  RW--V 
;   545 : #define NV_AUDNOTE_FM_WS_ABS_SINE                        0x00000002 /* RW--V */
NV_AUDNOTE_FM_WS_ABS_SINE               EQU             000000002h
;  RW--V 
;   546 : #define NV_AUDNOTE_FM_WS_SHARK_SINE                      0x00000003 /* RW--V */
NV_AUDNOTE_FM_WS_SHARK_SINE             EQU             000000003h
;  RW--V 
;   547 : #define NV_AUDNOTE_FM_WS_QUICK_SINE                      0x00000004 /* RW--V */
NV_AUDNOTE_FM_WS_QUICK_SINE             EQU             000000004h
;  RW--V 
;   548 : #define NV_AUDNOTE_FM_WS_ABS_QUICK                       0x00000005 /* RW--V */
NV_AUDNOTE_FM_WS_ABS_QUICK              EQU             000000005h
;  RW--V 
;   549 : #define NV_AUDNOTE_FM_WS_SQUARE                          0x00000006 /* RW--V */
NV_AUDNOTE_FM_WS_SQUARE         EQU             000000006h
;  RW--V 
;   550 : #define NV_AUDNOTE_FM_WS_INV_SINE                        0x00000007 /* RW--V */
NV_AUDNOTE_FM_WS_INV_SINE               EQU             000000007h
;  RWXVF 
;   551 : #define NV_AUDNOTE_FM_PHASE_OUT               (15*32+ 8):(15*32+ 8) /* RWXVF */
NV_AUDNOTE_FM_PHASE_OUT         TEXTEQU         <(15*32+ 8):(15*32+ 8)>
;  RWXVF 
;   552 : #define NV_AUDNOTE_FM_SYNTHESIS               (15*32+12):(15*32+12) /* RWXVF */
NV_AUDNOTE_FM_SYNTHESIS         TEXTEQU         <(15*32+12):(15*32+12)>
;  _NV_REF_H_ 
;   553 : #endif /* _NV_REF_H_ */
;   554 : 
; End of file nv_ref.h
