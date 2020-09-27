
/* @(#) $Header: /drivers/display/WINNT40/miniport/NV_MINI/NV3A_REF.H 2     1/10/97 7:10p Vidall $
 NVidia Corporation */
/* basis: nv1 manuals 10.1
 95/06/27 NVidia Corporation */

#ifndef _NV_REF_H_
#define _NV_REF_H_

/* dev_bus.ref */
#define NV_SPACE                              0x01FFFFFF:0x00000000 /* RW--D */
/* dev_bus.ref */
#define NV_CONFIG                             0x000000FF:0x00000000 /* RW--D */
#define NV_CONFIG_PCI_NV_0                               0x00000000 /* R--4R */
#define NV_CONFIG_PCI_NV_0__ALIAS_1                NV_PBUS_PCI_NV_0 /*       */
#define NV_CONFIG_PCI_NV_0_VENDOR_ID                           15:0 /* C--UF */
#define NV_CONFIG_PCI_NV_0_VENDOR_ID_NVIDIA_SGS          0x000012D2 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_FUNC                     18:16 /* C--UF */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_FUNC_VGA            0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP                     31:19 /* C--UF */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV0            0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV1            0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV2            0x00000002 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV3            0x00000003 /* C---V */
#define NV_CONFIG_PCI_NV_1                               0x00000004 /* RW-4R */
#define NV_CONFIG_PCI_NV_1__ALIAS_1                NV_PBUS_PCI_NV_1 /*       */
#define NV_CONFIG_PCI_NV_1_IO_SPACE                             0:0 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_IO_SPACE_DISABLED             0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_1_IO_SPACE_ENABLED              0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_1_MEMORY_SPACE                         1:1 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_MEMORY_SPACE_DISABLED         0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_1_MEMORY_SPACE_ENABLED          0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_1_BUS_MASTER                           2:2 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_BUS_MASTER_DISABLED           0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_1_BUS_MASTER_ENABLED            0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_1_WRITE_AND_INVAL                      4:4 /* C--VF */
#define NV_CONFIG_PCI_NV_1_WRITE_AND_INVAL_DISABLED      0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_1_WRITE_AND_INVAL_ENABLED       0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_1_PALETTE_SNOOP                        5:5 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_PALETTE_SNOOP_DISABLED        0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_1_PALETTE_SNOOP_ENABLED         0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_1_CAPLIST                            20:20 /* R--VF */
#define NV_CONFIG_PCI_NV_1_CAPLIST_NOT_PRESENT           0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_1_CAPLIST_PRESENT               0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_1_66MHZ                              21:21 /* C--VF */
#define NV_CONFIG_PCI_NV_1_66MHZ_INCAPABLE               0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_1_66MHZ_CAPABLE                 0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_1_FAST_BACK2BACK                     23:23 /* C--VF */
#define NV_CONFIG_PCI_NV_1_FAST_BACK2BACK_INCAPABLE      0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_1_FAST_BACK2BACK_CAPABLE        0x00000001 /* C---V */
#define NV_CONFIG_PCI_NV_1_DEVSEL_TIMING                      26:25 /* C--VF */
#define NV_CONFIG_PCI_NV_1_DEVSEL_TIMING_FAST            0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_1_DEVSEL_TIMING_MEDIUM          0x00000001 /* C---V */
#define NV_CONFIG_PCI_NV_1_DEVSEL_TIMING_SLOW            0x00000002 /* ----V */
#define NV_CONFIG_PCI_NV_1_SIGNALED_TARGET                    27:27 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_SIGNALED_TARGET_NO_ABORT      0x00000000 /* R-I-V */
#define NV_CONFIG_PCI_NV_1_SIGNALED_TARGET_ABORT         0x00000001 /* R---V */
#define NV_CONFIG_PCI_NV_1_SIGNALED_TARGET_CLEAR         0x00000001 /* -W--V */
#define NV_CONFIG_PCI_NV_1_RECEIVED_TARGET                    28:28 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_RECEIVED_TARGET_NO_ABORT      0x00000000 /* R-I-V */
#define NV_CONFIG_PCI_NV_1_RECEIVED_TARGET_ABORT         0x00000001 /* R---V */
#define NV_CONFIG_PCI_NV_1_RECEIVED_TARGET_CLEAR         0x00000001 /* -W--V */
#define NV_CONFIG_PCI_NV_1_RECEIVED_MASTER                    29:29 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_RECEIVED_MASTER_NO_ABORT      0x00000000 /* R-I-V */
#define NV_CONFIG_PCI_NV_1_RECEIVED_MASTER_ABORT         0x00000001 /* R---V */
#define NV_CONFIG_PCI_NV_1_RECEIVED_MASTER_CLEAR         0x00000001 /* -W--V */
#define NV_CONFIG_PCI_NV_2                               0x00000008 /* R--4R */
#define NV_CONFIG_PCI_NV_2__ALIAS_1                NV_PBUS_PCI_NV_2 /*       */
#define NV_CONFIG_PCI_NV_2_REVISION_ID                          7:0 /* C--UF */
#define NV_CONFIG_PCI_NV_2_REVISION_ID_A                 0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_2_REVISION_ID_B                 0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_2_CLASS_CODE                          31:8 /* C--VF */
#define NV_CONFIG_PCI_NV_2_CLASS_CODE_VGA                0x00030000 /* C---V */
#define NV_CONFIG_PCI_NV_2_CLASS_CODE_MULTIMEDIA         0x00048000 /* ----V */
#define NV_CONFIG_PCI_NV_3                               0x0000000C /* RW-4R */
#define NV_CONFIG_PCI_NV_3__ALIAS_1                NV_PBUS_PCI_NV_3 /*       */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER                      15:11 /* RWIUF */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_0_CLOCKS        0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_8_CLOCKS        0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_240_CLOCKS      0x0000001E /* RW--V */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_248_CLOCKS      0x0000001F /* RW--V */
#define NV_CONFIG_PCI_NV_3_HEADER_TYPE                        23:16 /* C--VF */
#define NV_CONFIG_PCI_NV_3_HEADER_TYPE_SINGLEFUNC        0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_3_HEADER_TYPE_MULTIFUNC         0x00000080 /* ----V */
#define NV_CONFIG_PCI_NV_4                               0x00000010 /* RW-4R */
#define NV_CONFIG_PCI_NV_4__ALIAS_1                NV_PBUS_PCI_NV_4 /*       */
#define NV_CONFIG_PCI_NV_4_SPACE_TYPE                           0:0 /* C--VF */
#define NV_CONFIG_PCI_NV_4_SPACE_TYPE_MEMORY             0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_4_SPACE_TYPE_IO                 0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_4_ADDRESS_TYPE                         2:1 /* C--VF */
#define NV_CONFIG_PCI_NV_4_ADDRESS_TYPE_32_BIT           0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_4_ADDRESS_TYPE_20_BIT           0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_4_ADDRESS_TYPE_64_BIT           0x00000002 /* ----V */
#define NV_CONFIG_PCI_NV_4_PREFETCHABLE                         3:3 /* C--VF */
#define NV_CONFIG_PCI_NV_4_PREFETCHABLE_NOT              0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_4_PREFETCHABLE_MERGABLE         0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_4_BASE_ADDRESS                       31:24 /* RWXUF */
#define NV_CONFIG_PCI_NV_5                               0x00000014 /* RW-4R */
#define NV_CONFIG_PCI_NV_5__ALIAS_1                NV_PBUS_PCI_NV_5 /*       */
#define NV_CONFIG_PCI_NV_5_SPACE_TYPE                           0:0 /* C--VF */
#define NV_CONFIG_PCI_NV_5_SPACE_TYPE_MEMORY             0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_5_SPACE_TYPE_IO                 0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_5_ADDRESS_TYPE                         2:1 /* C--VF */
#define NV_CONFIG_PCI_NV_5_ADDRESS_TYPE_32_BIT           0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_5_ADDRESS_TYPE_20_BIT           0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_5_ADDRESS_TYPE_64_BIT           0x00000002 /* ----V */
#define NV_CONFIG_PCI_NV_5_PREFETCHABLE                         3:3 /* C--VF */
#define NV_CONFIG_PCI_NV_5_PREFETCHABLE_NOT              0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_5_PREFETCHABLE_MERGABLE         0x00000001 /* C---V */
#define NV_CONFIG_PCI_NV_5_BASE_ADDRESS                       31:24 /* RWXUF */
#define NV_CONFIG_PCI_NV_6                               0x00000018 /* RW-4R */
#define NV_CONFIG_PCI_NV_6__ALIAS_1                NV_PBUS_PCI_NV_6 /*       */
#define NV_CONFIG_PCI_NV_6_SPACE_TYPE                           0:0 /* C--VF */
#define NV_CONFIG_PCI_NV_6_SPACE_TYPE_MEMORY             0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_6_SPACE_TYPE_IO                 0x00000001 /* C---V */
#define NV_CONFIG_PCI_NV_6_BASE_ADDRESS                        31:8 /* RWXUF */
#define NV_CONFIG_PCI_NV_7(i)                    (0x0000001C+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_NV_7__SIZE_1                                4 /*       */
#define NV_CONFIG_PCI_NV_7__ALIAS_1                NV_PBUS_PCI_NV_7 /*       */
#define NV_CONFIG_PCI_NV_7_RESERVED                            31:0 /* C--VF */
#define NV_CONFIG_PCI_NV_7_RESERVED_0                    0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_11                              0x0000002C /* R--4R */
#define NV_CONFIG_PCI_NV_11__ALIAS_1              NV_PBUS_PCI_NV_11 /*       */
#define NV_CONFIG_PCI_NV_11_SUBSYSTEM_VENDOR_ID               31:16 /* R--UF */
#define NV_CONFIG_PCI_NV_11_SUBSYSTEM_VENDOR_ID_NONE     0x00000000 /* R---V */
#define NV_CONFIG_PCI_NV_11_SUBSYSTEM_ID                       15:0 /* R--UF */
#define NV_CONFIG_PCI_NV_11_SUBSYSTEM_ID_NONE            0x00000000 /* R---V */
#define NV_CONFIG_PCI_NV_12                              0x00000030 /* RW-4R */
#define NV_CONFIG_PCI_NV_12__ALIAS_1              NV_PBUS_PCI_NV_12 /*       */
#define NV_CONFIG_PCI_NV_12_ROM_DECODE                          0:0 /* RWIVF */
#define NV_CONFIG_PCI_NV_12_ROM_DECODE_DISABLED          0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_12_ROM_DECODE_ENABLED           0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_12_ROM_BASE                          31:22 /* RWXUF */
#define NV_CONFIG_PCI_NV_13                              0x00000034 /* RW-4R */
#define NV_CONFIG_PCI_NV_13__ALIAS_1              NV_PBUS_PCI_NV_13 /*       */
#define NV_CONFIG_PCI_NV_13_CAP_PTR                             7:0 /* C--VF */
#define NV_CONFIG_PCI_NV_13_CAP_PTR_AGP                  0x00000044 /* C---V */
#define NV_CONFIG_PCI_NV_14                              0x00000038 /* R--4A */
#define NV_CONFIG_PCI_NV_14__ALIAS_1              NV_PBUS_PCI_NV_14 /*       */
#define NV_CONFIG_PCI_NV_14_RESERVED                           31:0 /* C--VF */
#define NV_CONFIG_PCI_NV_14_RESERVED_0                   0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_15                              0x0000003C /* RW-4R */
#define NV_CONFIG_PCI_NV_15__ALIAS_1              NV_PBUS_PCI_NV_15 /*       */
#define NV_CONFIG_PCI_NV_15_INTR_LINE                           7:0 /* RWIVF */
#define NV_CONFIG_PCI_NV_15_INTR_LINE_IRQ0               0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_15_INTR_LINE_IRQ1               0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_15_INTR_LINE_IRQ15              0x0000000F /* RW--V */
#define NV_CONFIG_PCI_NV_15_INTR_LINE_UNKNOWN            0x000000FF /* RW--V */
#define NV_CONFIG_PCI_NV_15_INTR_PIN                           15:8 /* C--VF */
#define NV_CONFIG_PCI_NV_15_INTR_PIN_INTA                0x00000001 /* C---V */
#define NV_CONFIG_PCI_NV_15_MIN_GNT                           23:16 /* C--VF */
#define NV_CONFIG_PCI_NV_15_MIN_GNT_NO_REQUIREMENTS      0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_15_MIN_GNT_750NS                0x00000003 /* C---V */
#define NV_CONFIG_PCI_NV_15_MAX_LAT                           31:24 /* C--VF */
#define NV_CONFIG_PCI_NV_15_MAX_LAT_NO_REQUIREMENTS      0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_15_MAX_LAT_250NS                0x00000001 /* C---V */
#define NV_CONFIG_PCI_NV_16                              0x00000040 /* RW-4R */
#define NV_CONFIG_PCI_NV_16__ALIAS_1              NV_PBUS_PCI_NV_16 /*       */
#define NV_CONFIG_PCI_NV_16_SUBSYSTEM_VENDOR_ID               31:16 /* RW-VF */
#define NV_CONFIG_PCI_NV_16_SUBSYSTEM_VENDOR_ID_NONE     0x00000000 /* R---V */
#define NV_CONFIG_PCI_NV_16_SUBSYSTEM_ID                       15:0 /* RW-VF */
#define NV_CONFIG_PCI_NV_16_SUBSYSTEM_ID_NONE            0x00000000 /* R---V */
#define NV_CONFIG_PCI_NV_17                              0x00000044 /* RW-4R */
#define NV_CONFIG_PCI_NV_17__ALIAS_1              NV_PBUS_PCI_NV_17 /*       */
#define NV_CONFIG_PCI_NV_17_AGP_REV_MAJOR                     23:20 /* C--VF */
#define NV_CONFIG_PCI_NV_17_AGP_REV_MAJOR_1              0x00000001 /* C---V */
#define NV_CONFIG_PCI_NV_17_AGP_REV_MINOR                     19:16 /* C--VF */
#define NV_CONFIG_PCI_NV_17_AGP_REV_MINOR_0              0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_17_NEXT_PTR                           15:8 /* C--VF */
#define NV_CONFIG_PCI_NV_17_NEXT_PTR_NULL                0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_17_CAP_ID                              7:0 /* C--VF */
#define NV_CONFIG_PCI_NV_17_CAP_ID_AGP                   0x00000002 /* C---V */
#define NV_CONFIG_PCI_NV_18                              0x00000048 /* RW-4R */
#define NV_CONFIG_PCI_NV_18__ALIAS_1              NV_PBUS_PCI_NV_18 /*       */
#define NV_CONFIG_PCI_NV_18_AGP_STATUS_RQ                     31:24 /* C--VF */
#define NV_CONFIG_PCI_NV_18_AGP_STATUS_RQ_4              0x00000004 /* C---V */
#define NV_CONFIG_PCI_NV_18_AGP_STATUS_SBA                      9:9 /* C--VF */
#define NV_CONFIG_PCI_NV_18_AGP_STATUS_SBA_NONE          0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_18_AGP_STATUS_RATE                     1:0 /* C--VF */
#define NV_CONFIG_PCI_NV_18_AGP_STATUS_RATE_1X           0x00000001 /* C---V */
#define NV_CONFIG_PCI_NV_19                              0x0000004C /* RW-4R */
#define NV_CONFIG_PCI_NV_19__ALIAS_1              NV_PBUS_PCI_NV_19 /*       */
#define NV_CONFIG_PCI_NV_19_AGP_COMMAND_RQ_DEPTH              31:24 /* RWIVF */
#define NV_CONFIG_PCI_NV_19_AGP_COMMAND_RQ_DEPTH_0       0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_19_AGP_COMMAND_AGP_ENABLE              8:8 /* RWIVF */
#define NV_CONFIG_PCI_NV_19_AGP_COMMAND_AGP_ENABLE_OFF   0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_19_AGP_COMMAND_DATA_RATE               1:0 /* RWIVF */
#define NV_CONFIG_PCI_NV_19_AGP_COMMAND_DATA_RATE_OFF    0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_20                              0x00000050 /* RW-4R */
#define NV_CONFIG_PCI_NV_20__ALIAS_1              NV_PBUS_PCI_NV_20 /*       */
#define NV_CONFIG_PCI_NV_20_SOUND_BLASTER                       0:0 /* RWIVF */
#define NV_CONFIG_PCI_NV_20_SOUND_BLASTER_DISABLED       0x00000000 /* RW--V */
#define NV_CONFIG_PCI_NV_20_SOUND_BLASTER_ENABLED        0x00000001 /* RWI-V */
#define NV_CONFIG_PCI_NV_20_FM_SYNTHESIS                        1:1 /* RWIVF */
#define NV_CONFIG_PCI_NV_20_FM_SYNTHESIS_DISABLED        0x00000000 /* RW--V */
#define NV_CONFIG_PCI_NV_20_FM_SYNTHESIS_ENABLED         0x00000001 /* RWI-V */
#define NV_CONFIG_PCI_NV_20_GAME_PORT                           2:2 /* RWIVF */
#define NV_CONFIG_PCI_NV_20_GAME_PORT_DISABLED           0x00000000 /* RW--V */
#define NV_CONFIG_PCI_NV_20_GAME_PORT_ENABLED            0x00000001 /* RWI-V */
#define NV_CONFIG_PCI_NV_20_MPU401_IO                           3:3 /* RWIVF */
#define NV_CONFIG_PCI_NV_20_MPU401_IO_DISABLED           0x00000000 /* RW--V */
#define NV_CONFIG_PCI_NV_20_MPU401_IO_ENABLED            0x00000001 /* RWI-V */
#define NV_CONFIG_PCI_NV_20_MPU401_IRQ                          4:4 /* RWIVF */
#define NV_CONFIG_PCI_NV_20_MPU401_IRQ_DISABLED          0x00000000 /* RW--V */
#define NV_CONFIG_PCI_NV_20_MPU401_IRQ_ENABLED           0x00000001 /* RWI-V */
#define NV_CONFIG_PCI_NV_20_IO_ALIASING                         5:5 /* RWIVF */
#define NV_CONFIG_PCI_NV_20_IO_ALIASING_DISABLED         0x00000000 /* RW--V */
#define NV_CONFIG_PCI_NV_20_IO_ALIASING_ENABLED          0x00000001 /* RWI-V */
#define NV_CONFIG_PCI_NV_20_LEGACY_AUDIO                        7:7 /* RWIVF */
#define NV_CONFIG_PCI_NV_20_LEGACY_AUDIO_ENABLED         0x00000000 /* RW--V */
#define NV_CONFIG_PCI_NV_20_LEGACY_AUDIO_DISABLED        0x00000001 /* RWI-V */
#define NV_CONFIG_PCI_NV_20_SB_BASE                             9:8 /* RWIVF */
#define NV_CONFIG_PCI_NV_20_SB_BASE_220H                 0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_20_SB_BASE_240H                 0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_20_SB_BASE_260H                 0x00000002 /* RW--V */
#define NV_CONFIG_PCI_NV_20_MPU_BASE                          13:12 /* RWIVF */
#define NV_CONFIG_PCI_NV_20_MPU_BASE_330H                0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_20_MPU_BASE_230H                0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_20_MPU_BASE_300H                0x00000002 /* RW--V */
#define NV_CONFIG_PCI_NV_20_DMA_SNOOP                         16:16 /* RWIVF */
#define NV_CONFIG_PCI_NV_20_DMA_SNOOP_DISABLED           0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_20_DMA_SNOOP_ENABLED            0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_21                              0x00000054 /* RW-4R */
#define NV_CONFIG_PCI_NV_21__ALIAS_1              NV_PBUS_PCI_NV_21 /*       */
#define NV_CONFIG_PCI_NV_21_VGA                                 0:0 /* RWIVF */
#define NV_CONFIG_PCI_NV_21_VGA_DISABLED                 0x00000000 /* RW--V */
#define NV_CONFIG_PCI_NV_21_VGA_ENABLED                  0x00000001 /* RWI-V */
#define NV_CONFIG_PCI_NV_22                              0x00000058 /* RW-4R */
#define NV_CONFIG_PCI_NV_22__ALIAS_1              NV_PBUS_PCI_NV_22 /*       */
#define NV_CONFIG_PCI_NV_22_SCRATCH                            23:0 /* RWIVF */
#define NV_CONFIG_PCI_NV_22_SCRATCH_DEFAULT              0x0023D6CE /* RWI-V */
#define NV_CONFIG_PCI_NV_23(i)                   (0x0000005C+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_NV_23__SIZE_1                              41 /*       */
#define NV_CONFIG_PCI_NV_23__ALIAS_1              NV_PBUS_PCI_NV_23 /*       */
#define NV_CONFIG_PCI_NV_23_RESERVED                           31:0 /* C--VF */
#define NV_CONFIG_PCI_NV_23_RESERVED_0                   0x00000000 /* C---V */
/* dev_bus.ref */
#define NV_RMA_ID                                        0x00000000 /* R--4R */
#define NV_RMA_ID__ALIAS_1                          NV_PRMIO_RMA_ID /*       */
#define NV_RMA_ID_CODE                                         31:0 /* C--UF */
#define NV_RMA_ID_CODE_VALID                             0x2B16D065 /* C---V */
#define NV_RMA_PTR                                       0x00000004 /* RW-4R */
#define NV_RMA_PTR__ALIAS_1                        NV_PRMIO_RMA_PTR /*       */
#define NV_RMA_PTR_ADDRESS                                     24:2 /* RWXUF */
#define NV_RMA_DATA                                      0x00000008 /* RW-4R */
#define NV_RMA_DATA__ALIAS_1                      NV_PRMIO_RMA_DATA /*       */
#define NV_RMA_DATA_PORT                                       31:0 /* RWXUF */
#define NV_RMA_DATA32                                    0x0000000C /* RW-2R */
#define NV_RMA_DATA32__ALIAS_1                  NV_PRMIO_RMA_DATA32 /*       */
#define NV_RMA_DATA32_MSW                                     31:16 /* -WXUF */
#define NV_RMA_DATA32_LSW                                      15:0 /* RWXUF */
#define NV_RMA_INCDATA                                   0x00000010 /* RW-4R */
#define NV_RMA_INCDATA__ALIAS_1                NV_PRMIO_RMA_INCDATA /*       */
#define NV_RMA_INCDATA_PORT                                    31:0 /* RWXUF */
#define NV_RMA_INCDATA32                                 0x00000014 /* RW-4R */
#define NV_RMA_INCDATA32__ALIAS_1            NV_PRMIO_RMA_INCDATA32 /*       */
#define NV_RMA_INCDATA32_PORT                                  31:0 /* RWXUF */
/* dev_bus.ref */
#define NV_RMAIO                              0x000000FF:0x00000000 /* RW--D */
/* dev_bus.ref */
#define NV_PRMIO_RMA_ID                                  0x00007100 /* R--4R */
#define NV_PRMIO_RMA_ID__ALIAS_1                          NV_RMA_ID /*       */
#define NV_PRMIO_RMA_PTR                                 0x00007104 /* RW-4R */
#define NV_PRMIO_RMA_PTR__ALIAS_1                        NV_RMA_PTR /*       */
#define NV_PRMIO_RMA_DATA                                0x00007108 /* RW-4R */
#define NV_PRMIO_RMA_DATA__ALIAS_1                      NV_RMA_DATA /*       */
#define NV_PRMIO_RMA_DATA32                              0x0000710C /* RW-2R */
#define NV_PRMIO_RMA_DATA32__ALIAS_1                  NV_RMA_DATA32 /*       */
#define NV_PRMIO_RMA_INCDATA                             0x00007110 /* RW-4R */
#define NV_PRMIO_RMA_INCDATA__ALIAS_1                NV_RMA_INCDATA /*       */
#define NV_PRMIO_RMA_INCDATA32                           0x00007114 /* RW-4R */
#define NV_PRMIO_RMA_INCDATA32__ALIAS_1            NV_RMA_INCDATA32 /*       */
/* dev_bus.ref */
#define NV_MEMORY                             0xFFFFFFFF:0x00000000 /* RW--D */
/* dev_realmode.ref */
/* dev_bus.ref */
#define NV_IO                                 0xFFFFFFFF:0x00000000 /* RW--D */
#define NV_IO_DMA_0000                                   0x00000000 /* -W-1R */
#define NV_IO_DMA_0000_ADDRESS                                  7:0 /* -W-UF */
#define NV_IO_DMA_0001                                   0x00000001 /* -W-1R */
#define NV_IO_DMA_0001_COUNT                                    7:0 /* -W-UF */
#define NV_IO_DMA_0002                                   0x00000002 /* -W-1R */
#define NV_IO_DMA_0002_ADDRESS                                  7:0 /* -W-UF */
#define NV_IO_DMA_0003                                   0x00000003 /* -W-1R */
#define NV_IO_DMA_0003_COUNT                                    7:0 /* -W-UF */
#define NV_IO_DMA_0004                                   0x00000004 /* -W-1R */
#define NV_IO_DMA_0004_ADDRESS                                  7:0 /* -W-UF */
#define NV_IO_DMA_0005                                   0x00000005 /* -W-1R */
#define NV_IO_DMA_0005_COUNT                                    7:0 /* -W-UF */
#define NV_IO_DMA_0006                                   0x00000006 /* -W-1R */
#define NV_IO_DMA_0006_ADDRESS                                  7:0 /* -W-UF */
#define NV_IO_DMA_0007                                   0x00000007 /* -W-1R */
#define NV_IO_DMA_0007_COUNT                                    7:0 /* -W-UF */
#define NV_IO_DMA_0008                                   0x00000008 /* -W-1R */
#define NV_IO_DMA_0008_CONTROLLER                               2:2 /* -W-VF */
#define NV_IO_DMA_0008_CONTROLLER_DISABLED               0x00000000 /* -W--V */
#define NV_IO_DMA_0008_CONTROLLER_ENABLED                0x00000001 /* -W--V */
#define NV_IO_DMA_000A                                   0x0000000A /* -W-1R */
#define NV_IO_DMA_000A_CHANNEL                                  1:0 /* -W-UF */
#define NV_IO_DMA_000A_MASK_BIT                                 2:2 /* -W-VF */
#define NV_IO_DMA_000A_MASK_BIT_CLEAR                    0x00000000 /* -W--V */
#define NV_IO_DMA_000A_MASK_BIT_SET                      0x00000001 /* -W--V */
#define NV_IO_DMA_000B                                   0x0000000B /* -W-1R */
#define NV_IO_DMA_000B_CHANNEL                                  1:0 /* -W-UF */
#define NV_IO_DMA_000B_AUTOINITIALIZE                           4:4 /* -W-VF */
#define NV_IO_DMA_000B_AUTOINITIALIZE_DISABLED           0x00000000 /* -W--V */
#define NV_IO_DMA_000B_AUTOINITIALIZE_ENABLED            0x00000001 /* -W--V */
#define NV_IO_DMA_000B_DIRECTION                                5:5 /* -W-VF */
#define NV_IO_DMA_000B_DIRECTION_INCREMENT               0x00000000 /* -W--V */
#define NV_IO_DMA_000B_DIRECTION_DECREMENT               0x00000001 /* -W--V */
#define NV_IO_DMA_000C                                   0x0000000C /* -W-1R */
#define NV_IO_DMA_000C_CLEAR_FLIPFLOP                           7:0 /* -W-VF */
#define NV_IO_DMA_0081                                   0x00000081 /* -W-1R */
#define NV_IO_DMA_0081_ADDRESS                                  3:0 /* -W-UF */
#define NV_IO_DMA_0082                                   0x00000082 /* -W-1R */
#define NV_IO_DMA_0082_ADDRESS                                  3:0 /* -W-UF */
#define NV_IO_DMA_0083                                   0x00000083 /* -W-1R */
#define NV_IO_DMA_0083_ADDRESS                                  3:0 /* -W-UF */
#define NV_IO_DMA_0087                                   0x00000087 /* -W-1R */
#define NV_IO_DMA_0087_ADDRESS                                  3:0 /* -W-UF */
/* dev_dac.ref */
#define NV_PRAMDAC                            0x00680FFF:0x00680300 /* RW--D */
#define NV_PRAMDAC_CU_START_POS                          0x00680300 /* RW-4R */
#define NV_PRAMDAC_CU_START_POS_X                              11:0 /* RWXSF */
#define NV_PRAMDAC_CU_START_POS_Y                             27:16 /* RWXSF */
#define NV_PRAMDAC_APLL_COEFF                            0x00680500 /* RW-4R */
#define NV_PRAMDAC_APLL_COEFF_MDIV                              7:0 /* RWIUF */
#define NV_PRAMDAC_APLL_COEFF_NDIV                             15:8 /* RWIUF */
#define NV_PRAMDAC_APLL_COEFF_PDIV                            18:16 /* RWIVF */
#define NV_PRAMDAC_MPLL_COEFF                            0x00680504 /* RW-4R */
#define NV_PRAMDAC_MPLL_COEFF_MDIV                              7:0 /* RWIUF */
#define NV_PRAMDAC_MPLL_COEFF_NDIV                             15:8 /* RWIUF */
#define NV_PRAMDAC_MPLL_COEFF_PDIV                            18:16 /* RWIVF */
#define NV_PRAMDAC_VPLL_COEFF                            0x00680508 /* RW-4R */
#define NV_PRAMDAC_VPLL_COEFF_MDIV                              7:0 /* RWIUF */
#define NV_PRAMDAC_VPLL_COEFF_NDIV                             15:8 /* RWIUF */
#define NV_PRAMDAC_VPLL_COEFF_PDIV                            18:16 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT                      0x0068050C /* RW-4R */
#define NV_PRAMDAC_PLL_COEFF_SELECT_DLL_BYPASS                  4:4 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_DLL_BYPASS_FALSE     0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_DLL_BYPASS_TRUE      0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_SOURCE                 8:8 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_SOURCE_DEFAULT  0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_SOURCE_PROG     0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_BYPASS               12:12 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_BYPASS_FALSE    0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_MPLL_BYPASS_TRUE     0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE               16:16 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE_DEFAULT  0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE_PROG     0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_BYPASS               20:20 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_BYPASS_FALSE    0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_BYPASS_TRUE     0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_PCLK_SOURCE               25:24 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_PCLK_SOURCE_VPLL     0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_PCLK_SOURCE_VIP      0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_PCLK_SOURCE_XTALOSC  0x00000002 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO                28:28 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO_DB1       0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO_DB2       0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_SETUP_CONTROL                     0x00680510 /* RW-4R */
#define NV_PRAMDAC_PLL_SETUP_CONTROL_VALUE                     10:0 /* RWIVF */
#define NV_PRAMDAC_PLL_SETUP_CONTROL_VAL                 0x0000044E /* RWI-V */
#define NV_PRAMDAC_PLL_SETUP_CONTROL_PWRDWN                   12:12 /* RWIVF */
#define NV_PRAMDAC_PLL_SETUP_CONTROL_PWRDWN_ON           0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_SETUP_CONTROL_PWRDWN_OFF          0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_TEST_COUNTER                      0x00680514 /* RW-4R */
#define NV_PRAMDAC_PLL_TEST_COUNTER_NOOFIPCLKS                  9:0 /* -WIVF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_VALUE                      15:0 /* R--VF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE                    16:16 /* RWIVF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE_DEASSERTED    0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE_ASSERTED      0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_RESET                     20:20 /* RWIVF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_RESET_DEASSERTED     0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_RESET_ASSERTED       0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE                    25:24 /* RWIVF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_MCLK          0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_VCLK          0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_ACLK          0x00000010 /* RW--V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_PDIV_RST                  28:28 /* RWIVF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_PDIVRST_DEASSERTED   0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_PDIVRST_ASSERTED     0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_APLL_LOCK                 29:29 /* R--VF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_APLL_NOTLOCKED       0x00000000 /* R---V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_APLL_LOCKED          0x00000001 /* R---V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCK                 30:30 /* R--VF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_NOTLOCKED       0x00000000 /* R---V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCKED          0x00000001 /* R---V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCK                 31:31 /* R--VF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_NOTLOCKED       0x00000000 /* R---V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCKED          0x00000001 /* R---V */
#define NV_PRAMDAC_PALETTE_TEST                          0x00680518 /* RW-4R */
#define NV_PRAMDAC_PALETTE_TEST_BLUE_DATA                       7:0 /* R--VF */
#define NV_PRAMDAC_PALETTE_TEST_GREEN_DATA                     15:8 /* R--VF */
#define NV_PRAMDAC_PALETTE_TEST_RED_DATA                      23:16 /* R--VF */
#define NV_PRAMDAC_PALETTE_TEST_MODE                          24:24 /* RWIVF */
#define NV_PRAMDAC_PALETTE_TEST_MODE_8BIT                0x00000000 /* RWI-V */
#define NV_PRAMDAC_PALETTE_TEST_MODE_24BIT               0x00000001 /* RW--V */
#define NV_PRAMDAC_PALETTE_TEST_ADDRINC                       28:28 /* RWIVF */
#define NV_PRAMDAC_PALETTE_TEST_ADDRINC_READWRITE        0x00000000 /* RWI-V */
#define NV_PRAMDAC_PALETTE_TEST_ADDRINC_WRITEONLY        0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL                       0x00680600 /* RW-4R */
#define NV_PRAMDAC_GENERAL_CONTROL_FF_COEFF                     1:0 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_FF_COEFF_DEF          0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_IDC_MODE                     4:4 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_IDC_MODE_GAMMA        0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_IDC_MODE_INDEX        0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE                    8:8 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE_NOTSE       0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE_SEL         0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_565_MODE                   12:12 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_565_MODE_NOTSEL       0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_565_MODE_SEL          0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_BLK_PEDSTL                 16:16 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_BLK_PEDSTL_OFF        0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_BLK_PEDSTL_ON         0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_TERMINATION                17:17 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_TERMINATION_37OHM     0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_TERMINATION_75OHM     0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_BPC                        20:20 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_BPC_6BITS             0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_BPC_8BITS             0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_DAC_SLEEP                  24:24 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_DAC_SLEEP_DIS         0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_DAC_SLEEP_EN          0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_PALETTE_CLK                28:28 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_PALETTE_CLK_EN        0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_PALETTE_CLK_DIS       0x00000001 /* RW--V */
#define NV_PRAMDAC_PALETTE_RECOVERY                      0x00680604 /* R--4R */
#define NV_PRAMDAC_PALETTE_RECOVERY_ACTIVE_ADDRESS              7:0 /* R--UF */
#define NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER                10:8 /* R--VF */
#define NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER_RED      0x00000001 /* R---V */
#define NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER_GREEN    0x00000010 /* R---V */
#define NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER_BLUE     0x00000100 /* R---V */
#define NV_PRAMDAC_PALETTE_RECOVERY_DAC_STATE                 13:12 /* R--VF */
#define NV_PRAMDAC_PALETTE_RECOVERY_DAC_STATE_WRITE      0x00000000 /* R---V */
#define NV_PRAMDAC_PALETTE_RECOVERY_DAC_STATE_READ       0x00000011 /* R---V */
#define NV_PRAMDAC_PALETTE_RECOVERY_RED_DATA                  23:16 /* R--VF */
#define NV_PRAMDAC_PALETTE_RECOVERY_GREEN_DATA                31:24 /* R--VF */
#define NV_PRAMDAC_TEST_CONTROL                          0x00680608 /* RW-4R */
#define NV_PRAMDAC_TEST_CONTROL_CRC_RESET                       0:0 /* RWIVF */
#define NV_PRAMDAC_TEST_CONTROL_CRC_RESET_DEASSERTED     0x00000000 /* RWI-V */
#define NV_PRAMDAC_TEST_CONTROL_CRC_RESET_ASSERTED       0x00000001 /* RW--V */
#define NV_PRAMDAC_TEST_CONTROL_CRC_ENABLE                      4:4 /* RWIVF */
#define NV_PRAMDAC_TEST_CONTROL_CRC_ENABLE_DEASSERTED    0x00000000 /* RWI-V */
#define NV_PRAMDAC_TEST_CONTROL_CRC_ENABLE_ASSERTED      0x00000001 /* RW--V */
#define NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL                     9:8 /* RWIVF */
#define NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL_BLUE         0x00000000 /* RWI-V */
#define NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL_GREEN        0x00000001 /* RW--V */
#define NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL_RED          0x00000010 /* RW--V */
#define NV_PRAMDAC_TEST_CONTROL_TP_INS_EN                     12:12 /* RWIVF */
#define NV_PRAMDAC_TEST_CONTROL_TP_INS_EN_DEASSERTED     0x00000000 /* RWI-V */
#define NV_PRAMDAC_TEST_CONTROL_TP_INS_EN_ASSERTED       0x00000001 /* RW--V */
#define NV_PRAMDAC_TEST_CONTROL_PWRDWN_DAC                    16:16 /* RWIVF */
#define NV_PRAMDAC_TEST_CONTROL_PWRDWN_DAC_ON            0x00000000 /* RWI-V */
#define NV_PRAMDAC_TEST_CONTROL_PWRDWN_DAC_OFF           0x00000001 /* RW--V */
#define NV_PRAMDAC_TEST_CONTROL_DACTM                         20:20 /* RWIVF */
#define NV_PRAMDAC_TEST_CONTROL_DACTM_NORMAL             0x00000000 /* RWI-V */
#define NV_PRAMDAC_TEST_CONTROL_DACTM_TEST               0x00000001 /* RW--V */
#define NV_PRAMDAC_TEST_CONTROL_TPATH1                        24:24 /* RWIVF */
#define NV_PRAMDAC_TEST_CONTROL_TPATH1_CLEAR             0x00000000 /* RWI-V */
#define NV_PRAMDAC_TEST_CONTROL_TPATH1_SET               0x00000001 /* RW--V */
#define NV_PRAMDAC_TEST_CONTROL_TPATH31                       25:25 /* RWIVF */
#define NV_PRAMDAC_TEST_CONTROL_TPATH31_CLEAR            0x00000000 /* RWI-V */
#define NV_PRAMDAC_TEST_CONTROL_TPATH31_SET              0x00000001 /* RW--V */
#define NV_PRAMDAC_TEST_CONTROL_SENSEB                        28:28 /* R--VF */
#define NV_PRAMDAC_TEST_CONTROL_SENSEB_SOMELO            0x00000000 /* R---V */
#define NV_PRAMDAC_TEST_CONTROL_SENSEB_ALLHI             0x00000001 /* R---V */
#define NV_PRAMDAC_CHECKSUM                              0x0068060C /* R--4R */
#define NV_PRAMDAC_CHECKSUM_VALUE                              23:0 /* R--VF */
#define NV_PRAMDAC_TESTPOINT_DATA                        0x00680610 /* -W-4R */
#define NV_PRAMDAC_TESTPOINT_DATA_RED                           9:0 /* -W-VF */
#define NV_PRAMDAC_TESTPOINT_DATA_GREEN                       19:10 /* -W-VF */
#define NV_PRAMDAC_TESTPOINT_DATA_BLUE                        29:20 /* -W-VF */
#define NV_PRAMDAC_TESTPOINT_DATA_BLACK                       30:30 /* -W-VF */
#define NV_PRAMDAC_TESTPOINT_DATA_NOTBLANK                    31:31 /* -W-VF */
#define NV_PRAMDAC_VSERR_WIDTH                           0x00680700 /* RW-4R */
#define NV_PRAMDAC_VSERR_WIDTH_VAL                             10:0 /* RWIVF */
#define NV_PRAMDAC_VEQU_END                              0x00680704 /* RW-4R */
#define NV_PRAMDAC_VEQU_END_VAL                                10:0 /* RWIVF */
#define NV_PRAMDAC_VBBLANK_END                           0x00680708 /* RW-4R */
#define NV_PRAMDAC_VBBLANK_END_VAL                             10:0 /* RWIVF */
#define NV_PRAMDAC_VBLANK_END                            0x0068070C /* RW-4R */
#define NV_PRAMDAC_VBLANK_END_VAL                              10:0 /* RWIVF */
#define NV_PRAMDAC_VBLANK_START                          0x00680710 /* RW-4R */
#define NV_PRAMDAC_VBLANK_START_VAL                            10:0 /* RWIVF */
#define NV_PRAMDAC_VBBLANK_START                         0x00680714 /* RW-4R */
#define NV_PRAMDAC_VBBLANK_START_VAL                           10:0 /* RWIVF */
#define NV_PRAMDAC_VEQU_START                            0x00680718 /* RW-4R */
#define NV_PRAMDAC_VEQU_START_VAL                              10:0 /* RWIVF */
#define NV_PRAMDAC_VTOTAL                                0x0068071C /* RW-4R */
#define NV_PRAMDAC_VTOTAL_VAL                                  10:0 /* RWIVF */
#define NV_PRAMDAC_HSYNC_WIDTH                           0x00680720 /* RW-4R */
#define NV_PRAMDAC_HSYNC_WIDTH_VAL                             10:0 /* RWIVF */
#define NV_PRAMDAC_HBURST_START                          0x00680724 /* RW-4R */
#define NV_PRAMDAC_HBURST_START_VAL                            10:0 /* RWIVF */
#define NV_PRAMDAC_HBURST_END                            0x00680728 /* RW-4R */
#define NV_PRAMDAC_HBURST_END_VAL                              10:0 /* RWIVF */
#define NV_PRAMDAC_HBLANK_START                          0x0068072C /* RW-4R */
#define NV_PRAMDAC_HBLANK_START_VAL                            10:0 /* RWIVF */
#define NV_PRAMDAC_HBLANK_END                            0x00680730 /* RW-4R */
#define NV_PRAMDAC_HBLANK_END_VAL                              10:0 /* RWIVF */
#define NV_PRAMDAC_HTOTAL                                0x00680734 /* RW-4R */
#define NV_PRAMDAC_HTOTAL_VAL                                  10:0 /* RWIVF */
#define NV_PRAMDAC_HEQU_WIDTH                            0x00680738 /* RW-4R */
#define NV_PRAMDAC_HEQU_WIDTH_VAL                              10:0 /* RWIVF */
#define NV_PRAMDAC_HSERR_WIDTH                           0x0068073C /* RW-4R */
#define NV_PRAMDAC_HSERR_WIDTH_VAL                             10:0 /* RWIVF */
/* dev_dac.ref */
#define NV_USER_DAC                           0x00681FFF:0x00681200 /* RW--D */
#define NV_USER_DAC_PIXEL_MASK                           0x006813C6 /* RWI1R */
#define NV_USER_DAC_PIXEL_MASK_VALUE                            7:0 /* RWIVF */
#define NV_USER_DAC_PIXEL_MASK_MASK                      0x000000FF /* RWI-V */
#define NV_USER_DAC_READ_MODE_ADDRESS                    0x006813C7 /* RW-1R */
#define NV_USER_DAC_READ_MODE_ADDRESS_VALUE                     7:0 /* RW-VF */
#define NV_USER_DAC_READ_MODE_ADDRESS_WO_VALUE                  7:0 /* -W-VF */
#define NV_USER_DAC_READ_MODE_ADDRESS_RW_STATE                  1:0 /* R--VF */
#define NV_USER_DAC_READ_MODE_ADDRESS_RW_STATE_WRITE     0x00000000 /* R---V */
#define NV_USER_DAC_READ_MODE_ADDRESS_RW_STATE_READ      0x00000011 /* R---V */
#define NV_USER_DAC_WRITE_MODE_ADDRESS                   0x006813C8 /* RW-1R */
#define NV_USER_DAC_WRITE_MODE_ADDRESS_VALUE                    7:0 /* RW-VF */
#define NV_USER_DAC_PALETTE_DATA                         0x006813C9 /* RW-1R */
#define NV_USER_DAC_PALETTE_DATA_VALUE                          7:0 /* RW-VF */
/* dev_realmode.ref */
#define NV_IO_MPU_401_DATA                               0x00000330 /* R--1R */
#define NV_IO_MPU_401_DATA__ALIAS_1                      0x00000300 /* R--1R */
#define NV_IO_MPU_401_DATA__ALIAS_2                      0x00000230 /* R--1R */
#define NV_IO_MPU_401_DATA_VALUE                                7:0 /* R--VF */
#define NV_IO_MPU_401_DATA_ACK                           0x000000FE /* R---V */
#define NV_IO_MPU_401_STATUS                             0x00000331 /* R--1R */
#define NV_IO_MPU_401_STATUS__ALIAS_1                    0x00000301 /* R--1R */
#define NV_IO_MPU_401_STATUS__ALIAS_2                    0x00000231 /* R--1R */
#define NV_IO_MPU_401_STATUS_DATA                               5:0 /* R--VF */
#define NV_IO_MPU_401_STATUS_WRITE                              6:6 /* R--VF */
#define NV_IO_MPU_401_STATUS_WRITE_EMPTY                 0x00000000 /* R---V */
#define NV_IO_MPU_401_STATUS_WRITE_FULL                  0x00000001 /* R---V */
#define NV_IO_MPU_401_STATUS_READ                               7:7 /* R--VF */
#define NV_IO_MPU_401_STATUS_READ_FULL                   0x00000000 /* R---V */
#define NV_IO_MPU_401_STATUS_READ_EMPTY                  0x00000001 /* R---V */
#define NV_IO_MPU_401_COM                                0x00000331 /* -W-1R */
#define NV_IO_MPU_401_COM__ALIAS_1                       0x00000301 /* -W-1R */
#define NV_IO_MPU_401_COM__ALIAS_2                       0x00000231 /* -W-1R */
#define NV_IO_MPU_401_COM_UART_MODE                             7:0 /* -WIVF */
#define NV_IO_MPU_401_COM_UART_MODE_COMPLEX              0x000000ff /* -WI-V */
#define NV_IO_MPU_401_COM_UART_MODE_SIMPLE               0x0000003f /* -W--V */
/* dev_master.ref */
#define NV_PMC                                0x00000FFF:0x00000000 /* RW--D */
#define NV_PMC_BOOT_0                                    0x00000000 /* R--4R */
#define NV_PMC_BOOT_0_FIB_REVISION                              3:0 /* C--VF */
#define NV_PMC_BOOT_0_FIB_REVISION_0                     0x00000000 /* ----V */
#define NV_PMC_BOOT_0_FIB_REVISION_1                     0x00000001 /* ----V */
#define NV_PMC_BOOT_0_FIB_REVISION_2                     0x00000002 /* C---V */
#define NV_PMC_BOOT_0_MASK_REVISION                             7:4 /* C--VF */
#define NV_PMC_BOOT_0_MASK_REVISION_A                    0x00000000 /* ----V */
#define NV_PMC_BOOT_0_MASK_REVISION_B                    0x00000001 /* ----V */
#define NV_PMC_BOOT_0_MASK_REVISION_C                    0x00000002 /* C---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION                           15:8 /* R--VF */
#define NV_PMC_BOOT_0_IMPLEMENTATION_NV0                 0x00000000 /* ----V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_NV1V32              0x00000001 /* ----V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_NV1D32              0x00000002 /* ----V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_PICASSO             0x00000003 /* ----V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_NV2MUTARA           0x00000004 /* ----V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_NV3                 0x00000005 /* R---V */
#define NV_PMC_BOOT_0_ARCHITECTURE                            23:16 /* C--VF */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV0                   0x00000000 /* ----V */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV1                   0x00000001 /* ----V */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV2                   0x00000002 /* ----V */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV3                   0x00000003 /* C---V */
#define NV_PMC_BOOT_0_MANUFACTURER                            27:24 /* C--UF */
#define NV_PMC_BOOT_0_MANUFACTURER_NVIDIA                0x00000000 /* C---V */
#define NV_PMC_BOOT_0_FOUNDRY                                 31:28 /* C--VF */
#define NV_PMC_BOOT_0_FOUNDRY_SGS                        0x00000000 /* C---V */
#define NV_PMC_BOOT_0_FOUNDRY_HELIOS                     0x00000001 /* ----V */
#define NV_PMC_INTR_0                                    0x00000100 /* RW-4R */
#define NV_PMC_INTR_0_PAUDIO                                    0:0 /* R--VF */
#define NV_PMC_INTR_0_PAUDIO_NOT_PENDING                 0x00000000 /* R---V */
#define NV_PMC_INTR_0_PAUDIO_PENDING                     0x00000001 /* R---V */
#define NV_PMC_INTR_0_PMEDIA                                    4:4 /* R--VF */
#define NV_PMC_INTR_0_PMEDIA_NOT_PENDING                 0x00000000 /* R---V */
#define NV_PMC_INTR_0_PMEDIA_PENDING                     0x00000001 /* R---V */
#define NV_PMC_INTR_0_PFIFO                                     8:8 /* R--VF */
#define NV_PMC_INTR_0_PFIFO_NOT_PENDING                  0x00000000 /* R---V */
#define NV_PMC_INTR_0_PFIFO_PENDING                      0x00000001 /* R---V */
#define NV_PMC_INTR_0_PGRAPH0                                 12:12 /* R--VF */
#define NV_PMC_INTR_0_PGRAPH0_NOT_PENDING                0x00000000 /* R---V */
#define NV_PMC_INTR_0_PGRAPH0_PENDING                    0x00000001 /* R---V */
#define NV_PMC_INTR_0_PGRAPH1                                 13:13 /* R--VF */
#define NV_PMC_INTR_0_PGRAPH1_NOT_PENDING                0x00000000 /* R---V */
#define NV_PMC_INTR_0_PGRAPH1_PENDING                    0x00000001 /* R---V */
#define NV_PMC_INTR_0_PVIDEO                                  16:16 /* R--VF */
#define NV_PMC_INTR_0_PVIDEO_NOT_PENDING                 0x00000000 /* R---V */
#define NV_PMC_INTR_0_PVIDEO_PENDING                     0x00000001 /* R---V */
#define NV_PMC_INTR_0_PTIMER                                  20:20 /* R--VF */
#define NV_PMC_INTR_0_PTIMER_NOT_PENDING                 0x00000000 /* R---V */
#define NV_PMC_INTR_0_PTIMER_PENDING                     0x00000001 /* R---V */
#define NV_PMC_INTR_0_PFB                                     24:24 /* R--VF */
#define NV_PMC_INTR_0_PFB_NOT_PENDING                    0x00000000 /* R---V */
#define NV_PMC_INTR_0_PFB_PENDING                        0x00000001 /* R---V */
#define NV_PMC_INTR_0_PBUS                                    28:28 /* R--VF */
#define NV_PMC_INTR_0_PBUS_NOT_PENDING                   0x00000000 /* R---V */
#define NV_PMC_INTR_0_PBUS_PENDING                       0x00000001 /* R---V */
#define NV_PMC_INTR_0_SOFTWARE                                31:31 /* RWIVF */
#define NV_PMC_INTR_0_SOFTWARE_NOT_PENDING               0x00000000 /* RWI-V */
#define NV_PMC_INTR_0_SOFTWARE_PENDING                   0x00000001 /* RW--V */
#define NV_PMC_INTR_EN_0                                 0x00000140 /* RW-4R */
#define NV_PMC_INTR_EN_0_INTA                                   1:0 /* RWIVF */
#define NV_PMC_INTR_EN_0_INTA_DISABLED                   0x00000000 /* RWI-V */
#define NV_PMC_INTR_EN_0_INTA_HARDWARE                   0x00000001 /* RW--V */
#define NV_PMC_INTR_EN_0_INTA_SOFTWARE                   0x00000002 /* RW--V */
#define NV_PMC_INTR_READ_0                               0x00000160 /* R--4R */
#define NV_PMC_INTR_READ_0_INTA                                 0:0 /* R--VF */
#define NV_PMC_INTR_READ_0_INTA_LOW                      0x00000000 /* R---V */
#define NV_PMC_INTR_READ_0_INTA_HIGH                     0x00000001 /* R---V */
#define NV_PMC_ENABLE                                    0x00000200 /* RW-4R */
#define NV_PMC_ENABLE_PAUDIO                                    0:0 /* RWIVF */
#define NV_PMC_ENABLE_PAUDIO_DISABLED                    0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PAUDIO_ENABLED                     0x00000001 /* RW--V */
#define NV_PMC_ENABLE_PMEDIA                                    4:4 /* RWIVF */
#define NV_PMC_ENABLE_PMEDIA_DISABLED                    0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PMEDIA_ENABLED                     0x00000001 /* RW--V */
#define NV_PMC_ENABLE_PFIFO                                     8:8 /* RWIVF */
#define NV_PMC_ENABLE_PFIFO_DISABLED                     0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PFIFO_ENABLED                      0x00000001 /* RW--V */
#define NV_PMC_ENABLE_PGRAPH                                  12:12 /* RWIVF */
#define NV_PMC_ENABLE_PGRAPH_DISABLED                    0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PGRAPH_ENABLED                     0x00000001 /* RW--V */
#define NV_PMC_ENABLE_PPMI                                    16:16 /* RWIVF */
#define NV_PMC_ENABLE_PPMI_DISABLED                      0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PPMI_ENABLED                       0x00000001 /* RW--V */
#define NV_PMC_ENABLE_PFB                                     20:20 /* RWIVF */
#define NV_PMC_ENABLE_PFB_DISABLED                       0x00000000 /* RW--V */
#define NV_PMC_ENABLE_PFB_ENABLED                        0x00000001 /* RWI-V */
#define NV_PMC_ENABLE_PCRTC                                   24:24 /* RWIVF */
#define NV_PMC_ENABLE_PCRTC_DISABLED                     0x00000000 /* RW--V */
#define NV_PMC_ENABLE_PCRTC_ENABLED                      0x00000001 /* RWI-V */
#define NV_PMC_ENABLE_PVIDEO                                  28:28 /* RWIVF */
#define NV_PMC_ENABLE_PVIDEO_DISABLED                    0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PVIDEO_ENABLED                     0x00000001 /* RW--V */
/* dev_bus.ref */
#define NV_PBUS                               0x00001FFF:0x00001000 /* RW--D */
#define NV_PBUS_DEBUG_0                                  0x00001080 /* RW-4R */
#define NV_PBUS_DEBUG_0_MODE                                    0:0 /* RWIVF */
#define NV_PBUS_DEBUG_0_MODE_DISABLED                    0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_0_MODE_ENABLED                     0x00000001 /* RW--V */
#define NV_PBUS_DEBUG_0_DESKEWER                                4:4 /* RWIVF */
#define NV_PBUS_DEBUG_0_DESKEWER_ENABLED                 0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_0_DESKEWER_BYPASS                  0x00000001 /* RW--V */
#define NV_PBUS_DEBUG_0_FBIO_SCLK_DELAY                        11:8 /* RWIVF */
#define NV_PBUS_DEBUG_0_FBIO_SCLK_DELAY_8                0x00000008 /* RWI-V */
#define NV_PBUS_DEBUG_SEL_0                              0x00001090 /* RW-4R */
#define NV_PBUS_DEBUG_SEL_0_X                                   2:0 /* RWXUF */
#define NV_PBUS_DEBUG_SEL_1                              0x00001094 /* RW-4R */
#define NV_PBUS_DEBUG_SEL_1_X                                   2:0 /* RWXUF */
#define NV_PBUS_DEBUG_SEL_2                              0x00001098 /* RW-4R */
#define NV_PBUS_DEBUG_SEL_2_X                                   2:0 /* RWXUF */
#define NV_PBUS_DEBUG_SEL_3                              0x0000109C /* RW-4R */
#define NV_PBUS_DEBUG_SEL_3_X                                   2:0 /* RWXUF */
#define NV_PBUS_DEBUG_HOST                               0x000010A0 /* RW-4R */
#define NV_PBUS_DEBUG_HOST_SEL                                  2:0 /* RWXUF */
#define NV_PBUS_DEBUG_1                                  0x00001084 /* RW-4R */
#define NV_PBUS_DEBUG_1_PCIM_THROTTLE                           0:0 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIM_THROTTLE_DISABLED           0x00000000 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIM_THROTTLE_ENABLED            0x00000001 /* RWI-V */
#define NV_PBUS_DEBUG_1_PCIM_CMD                                1:1 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIM_CMD_SIZE_BASED              0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_1_PCIM_CMD_MRL_ONLY                0x00000001 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIM_AGP                                2:2 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIM_AGP_IS_AGP                  0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_1_PCIM_AGP_IS_PCI                  0x00000001 /* RW--V */
#define NV_PBUS_DEBUG_1_AGPM_CMD                                4:3 /* RWIVF */
#define NV_PBUS_DEBUG_1_AGPM_CMD_HP_ON_1ST               0x00000000 /* RW--V */
#define NV_PBUS_DEBUG_1_AGPM_CMD_LP_ONLY                 0x00000001 /* RWI-V */
#define NV_PBUS_DEBUG_1_AGPM_CMD_HP_ONLY                 0x00000002 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIS_WRITE                              5:5 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIS_WRITE_0_CYCLE               0x00000000 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIS_WRITE_1_CYCLE               0x00000001 /* RWI-V */
#define NV_PBUS_DEBUG_1_PCIS_2_1                                6:6 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIS_2_1_DISABLED                0x00000000 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIS_2_1_ENABLED                 0x00000001 /* RWI-V */
#define NV_PBUS_DEBUG_1_PCIS_RETRY                              7:7 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIS_RETRY_DISABLED              0x00000000 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIS_RETRY_ENABLED               0x00000001 /* RWI-V */
#define NV_PBUS_DEBUG_1_PCIS_RD_BURST                           8:8 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIS_RD_BURST_DISABLED           0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_1_PCIS_RD_BURST_ENABLED            0x00000001 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIS_WR_BURST                           9:9 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIS_WR_BURST_DISABLED           0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_1_PCIS_WR_BURST_ENABLED            0x00000001 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIS_EARLY_RTY                        10:10 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIS_EARLY_RTY_DISABLED          0x00000000 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIS_EARLY_RTY_ENABLED           0x00000001 /* RWI-V */
#define NV_PBUS_DEBUG_1_PCIS_RMAIO                            11:11 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIS_RMAIO_DISABLED              0x00000000 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIS_RMAIO_ENABLED               0x00000001 /* RWI-V */
#define NV_PBUS_DEBUG_1_PCIS_CPUQ                             12:12 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIS_CPUQ_DISABLED               0x00000000 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIS_CPUQ_ENABLED                0x00000001 /* RWI-V */
#define NV_PBUS_DEBUG_1_SPARE1                                13:13 /* RWIVF */
#define NV_PBUS_DEBUG_1_SPARE1_ZERO                      0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_1_SPARE1_ONE                       0x00000001 /* RW--V */
#define NV_PBUS_DEBUG_1_SPARE2                                14:14 /* RWIVF */
#define NV_PBUS_DEBUG_1_SPARE2_ZERO                      0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_1_SPARE2_ONE                       0x00000001 /* RW--V */
#define NV_PBUS_INTR_0                                   0x00001100 /* RW-4R */
#define NV_PBUS_INTR_0_PCI_BUS_ERROR                            0:0 /* RWXVF */
#define NV_PBUS_INTR_0_PCI_BUS_ERROR_NOT_PENDING         0x00000000 /* R---V */
#define NV_PBUS_INTR_0_PCI_BUS_ERROR_PENDING             0x00000001 /* R---V */
#define NV_PBUS_INTR_0_PCI_BUS_ERROR_RESET               0x00000001 /* -W--V */
#define NV_PBUS_INTR_EN_0                                0x00001140 /* RWI4R */
#define NV_PBUS_INTR_EN_0_PCI_BUS_ERROR                         0:0 /* RWIVF */
#define NV_PBUS_INTR_EN_0_PCI_BUS_ERROR_DISABLED         0x00000000 /* RWI-V */
#define NV_PBUS_INTR_EN_0_PCI_BUS_ERROR_ENABLED          0x00000001 /* RW--V */
#define NV_PBUS_RMC_DMA_0                                0x00001E80 /* RW-4R */
#define NV_PBUS_RMC_DMA_0_ADDRESS_BYTE_0                        7:0 /* RWXUF */
#define NV_PBUS_RMC_DMA_0_ADDRESS_BYTE_1                       15:8 /* RWXUF */
#define NV_PBUS_RMC_DMA_0_ADDRESS_BYTE_2                      23:16 /* RWXUF */
#define NV_PBUS_RMC_DMA_0_MASK_BIT                            24:24 /* RWXVF */
#define NV_PBUS_RMC_DMA_0_MASK_BIT_CLEAR                 0x00000000 /* RW--V */
#define NV_PBUS_RMC_DMA_0_MASK_BIT_SET                   0x00000001 /* RW--V */
#define NV_PBUS_RMC_DMA_0_AUTOINITIALIZE                      25:25 /* RWXVF */
#define NV_PBUS_RMC_DMA_0_AUTOINITIALIZE_DISABLED        0x00000000 /* RW--V */
#define NV_PBUS_RMC_DMA_0_AUTOINITIALIZE_ENABLED         0x00000001 /* RW--V */
#define NV_PBUS_RMC_DMA_0_DIRECTION                           26:26 /* RWXVF */
#define NV_PBUS_RMC_DMA_0_DIRECTION_INCREMENT            0x00000000 /* RW--V */
#define NV_PBUS_RMC_DMA_0_DIRECTION_DECREMENT            0x00000001 /* RW--V */
#define NV_PBUS_RMC_DMA_0_CHANNEL                             29:28 /* RWXUF */
#define NV_PBUS_RMC_DMA_0_CONTROLLER                          30:30 /* RWXVF */
#define NV_PBUS_RMC_DMA_0_CONTROLLER_DISABLED            0x00000000 /* RW--V */
#define NV_PBUS_RMC_DMA_0_CONTROLLER_ENABLED             0x00000001 /* RW--V */
#define NV_PBUS_RMC_DMA_0_FLIPFLOP                            31:31 /* RWIVF */
#define NV_PBUS_RMC_DMA_0_FLIPFLOP_BYTE_0                0x00000000 /* RWI-V */
#define NV_PBUS_RMC_DMA_0_FLIPFLOP_BYTE_1                0x00000001 /* RW--V */
#define NV_PBUS_RMC_DMA_1                                0x00001E84 /* RW-4R */
#define NV_PBUS_RMC_DMA_1_COUNT_BYTE_0                          7:0 /* RWXUF */
#define NV_PBUS_RMC_DMA_1_COUNT_BYTE_1                         15:8 /* RWXUF */
#define NV_PBUS_RMC_DMA_1_STATE                               28:28 /* RWXVF */
#define NV_PBUS_RMC_DMA_1_STATE_CLEAN                    0x00000000 /* RW--V */
#define NV_PBUS_RMC_DMA_1_STATE_DIRTY                    0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_0                                 0x00001800 /* R--4R */
#define NV_PBUS_PCI_NV_0__ALIAS_1                NV_CONFIG_PCI_NV_0 /*       */
#define NV_PBUS_PCI_NV_1                                 0x00001804 /* RW-4R */
#define NV_PBUS_PCI_NV_1__ALIAS_1                NV_CONFIG_PCI_NV_1 /*       */
#define NV_PBUS_PCI_NV_2                                 0x00001808 /* R--4R */
#define NV_PBUS_PCI_NV_2__ALIAS_1                NV_CONFIG_PCI_NV_2 /*       */
#define NV_PBUS_PCI_NV_3                                 0x0000180C /* RW-4R */
#define NV_PBUS_PCI_NV_3__ALIAS_1                NV_CONFIG_PCI_NV_3 /*       */
#define NV_PBUS_PCI_NV_4                                 0x00001810 /* RW-4R */
#define NV_PBUS_PCI_NV_4__ALIAS_1                NV_CONFIG_PCI_NV_4 /*       */
#define NV_PBUS_PCI_NV_5                                 0x00001814 /* RW-4R */
#define NV_PBUS_PCI_NV_5__ALIAS_1                NV_CONFIG_PCI_NV_5 /*       */
#define NV_PBUS_PCI_NV_6                                 0x00001818 /* RW-4R */
#define NV_PBUS_PCI_NV_6__ALIAS_1                NV_CONFIG_PCI_NV_6 /*       */
#define NV_PBUS_PCI_NV_7(i)                      (0x0000181C+(i)*4) /* R--4A */
#define NV_PBUS_PCI_NV_7__SIZE_1                                  4 /*       */
#define NV_PBUS_PCI_NV_7__ALIAS_1                NV_CONFIG_PCI_NV_7 /*       */
#define NV_PBUS_PCI_NV_11                                0x0000182C /* R--4R */
#define NV_PBUS_PCI_NV_11__ALIAS_1              NV_CONFIG_PCI_NV_11 /*       */
#define NV_PBUS_PCI_NV_12                                0x00001830 /* RW-4R */
#define NV_PBUS_PCI_NV_12__ALIAS_1              NV_CONFIG_PCI_NV_12 /*       */
#define NV_PBUS_PCI_NV_13                                0x00001834 /* RW-4R */
#define NV_PBUS_PCI_NV_13__ALIAS_1              NV_CONFIG_PCI_NV_13 /*       */
#define NV_PBUS_PCI_NV_14                                0x00001838 /* R--4A */
#define NV_PBUS_PCI_NV_14__ALIAS_1              NV_CONFIG_PCI_NV_14 /*       */
#define NV_PBUS_PCI_NV_15                                0x0000183C /* RW-4R */
#define NV_PBUS_PCI_NV_15__ALIAS_1              NV_CONFIG_PCI_NV_15 /*       */
#define NV_PBUS_PCI_NV_16                                0x00001840 /* RW-4R */
#define NV_PBUS_PCI_NV_16__ALIAS_1              NV_CONFIG_PCI_NV_16 /*       */
#define NV_PBUS_PCI_NV_17                                0x00001844 /* RW-4R */
#define NV_PBUS_PCI_NV_17__ALIAS_1              NV_CONFIG_PCI_NV_17 /*       */
#define NV_PBUS_PCI_NV_18                                0x00001848 /* RW-4R */
#define NV_PBUS_PCI_NV_18__ALIAS_1              NV_CONFIG_PCI_NV_18 /*       */
#define NV_PBUS_PCI_NV_19                                0x0000184C /* RW-4R */
#define NV_PBUS_PCI_NV_19__ALIAS_1              NV_CONFIG_PCI_NV_19 /*       */
#define NV_PBUS_PCI_NV_20                                0x00001850 /* RW-4R */
#define NV_PBUS_PCI_NV_20__ALIAS_1              NV_CONFIG_PCI_NV_20 /*       */
#define NV_PBUS_PCI_NV_21                                0x00001854 /* RW-4R */
#define NV_PBUS_PCI_NV_21__ALIAS_1              NV_CONFIG_PCI_NV_21 /*       */
#define NV_PBUS_PCI_NV_22                                0x00001858 /* RW-4R */
#define NV_PBUS_PCI_NV_22__ALIAS_1              NV_CONFIG_PCI_NV_22 /*       */
#define NV_PBUS_PCI_NV_23(i)                     (0x0000185C+(i)*4) /* R--4A */
#define NV_PBUS_PCI_NV_23__SIZE_1                                41 /*       */
#define NV_PBUS_PCI_NV_23__ALIAS_1              NV_CONFIG_PCI_NV_23 /*       */
/* dev_fifo.ref */
#define NV_PFIFO                              0x00003FFF:0x00002000 /* RW--D */
#define NV_PFIFO_DELAY_0                                 0x00002040 /* RW-4R */
#define NV_PFIFO_DELAY_0_WAIT_RETRY                             7:0 /* RWIUF */
#define NV_PFIFO_DELAY_0_WAIT_RETRY_0                    0x00000000 /* RWI-V */
#define NV_PFIFO_DEBUG_0                                 0x00002080 /* R--4R */
#define NV_PFIFO_DEBUG_0_CACHE_ERROR0                           0:0 /* R-XVF */
#define NV_PFIFO_DEBUG_0_CACHE_ERROR0_NOT_PENDING        0x00000000 /* R---V */
#define NV_PFIFO_DEBUG_0_CACHE_ERROR0_PENDING            0x00000001 /* R---V */
#define NV_PFIFO_DEBUG_0_CACHE_ERROR1                           4:4 /* R-XVF */
#define NV_PFIFO_DEBUG_0_CACHE_ERROR1_NOT_PENDING        0x00000000 /* R---V */
#define NV_PFIFO_DEBUG_0_CACHE_ERROR1_PENDING            0x00000001 /* R---V */
#define NV_PFIFO_INTR_0                                  0x00002100 /* RW-4R */
#define NV_PFIFO_INTR_0_CACHE_ERROR                             0:0 /* RWXVF */
#define NV_PFIFO_INTR_0_CACHE_ERROR_NOT_PENDING          0x00000000 /* R---V */
#define NV_PFIFO_INTR_0_CACHE_ERROR_PENDING              0x00000001 /* R---V */
#define NV_PFIFO_INTR_0_CACHE_ERROR_RESET                0x00000001 /* -W--V */
#define NV_PFIFO_INTR_0_RUNOUT                                  4:4 /* RWXVF */
#define NV_PFIFO_INTR_0_RUNOUT_NOT_PENDING               0x00000000 /* R---V */
#define NV_PFIFO_INTR_0_RUNOUT_PENDING                   0x00000001 /* R---V */
#define NV_PFIFO_INTR_0_RUNOUT_RESET                     0x00000001 /* -W--V */
#define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW                         8:8 /* RWXVF */
#define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW_NOT_PENDING      0x00000000 /* R---V */
#define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW_PENDING          0x00000001 /* R---V */
#define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW_RESET            0x00000001 /* -W--V */
#define NV_PFIFO_INTR_0_DMA_PUSHER                            12:12 /* RWXVF */
#define NV_PFIFO_INTR_0_DMA_PUSHER_NOT_PENDING           0x00000000 /* R---V */
#define NV_PFIFO_INTR_0_DMA_PUSHER_PENDING               0x00000001 /* R---V */
#define NV_PFIFO_INTR_0_DMA_PUSHER_RESET                 0x00000001 /* -W--V */
#define NV_PFIFO_INTR_0_DMA_PTE                               16:16 /* RWXVF */
#define NV_PFIFO_INTR_0_DMA_PTE_NOT_PENDING              0x00000000 /* R---V */
#define NV_PFIFO_INTR_0_DMA_PTE_PENDING                  0x00000001 /* R---V */
#define NV_PFIFO_INTR_0_DMA_PTE_RESET                    0x00000001 /* -W--V */
#define NV_PFIFO_INTR_EN_0                               0x00002140 /* RW-4R */
#define NV_PFIFO_INTR_EN_0_CACHE_ERROR                          0:0 /* RWIVF */
#define NV_PFIFO_INTR_EN_0_CACHE_ERROR_DISABLED          0x00000000 /* RWI-V */
#define NV_PFIFO_INTR_EN_0_CACHE_ERROR_ENABLED           0x00000001 /* RW--V */
#define NV_PFIFO_INTR_EN_0_RUNOUT                               4:4 /* RWIVF */
#define NV_PFIFO_INTR_EN_0_RUNOUT_DISABLED               0x00000000 /* RWI-V */
#define NV_PFIFO_INTR_EN_0_RUNOUT_ENABLED                0x00000001 /* RW--V */
#define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW                      8:8 /* RWIVF */
#define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW_DISABLED      0x00000000 /* RWI-V */
#define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW_ENABLED       0x00000001 /* RW--V */
#define NV_PFIFO_INTR_EN_0_DMA_PUSHER                         12:12 /* RWIVF */
#define NV_PFIFO_INTR_EN_0_DMA_PUSHER_DISABLED           0x00000000 /* RWI-V */
#define NV_PFIFO_INTR_EN_0_DMA_PUSHER_ENABLED            0x00000001 /* RW--V */
#define NV_PFIFO_INTR_EN_0_DMA_PTE                            16:16 /* RWIVF */
#define NV_PFIFO_INTR_EN_0_DMA_PTE_DISABLED              0x00000000 /* RWI-V */
#define NV_PFIFO_INTR_EN_0_DMA_PTE_ENABLED               0x00000001 /* RW--V */
#define NV_PFIFO_CONFIG_0                                0x00002200 /* RW-4R */
#define NV_PFIFO_CONFIG_0_DMA_FETCH                            10:8 /* RWXVF */
#define NV_PFIFO_CONFIG_0_DMA_FETCH_32_BYTES             0x00000000 /* RW--V */
#define NV_PFIFO_CONFIG_0_DMA_FETCH_64_BYTES             0x00000001 /* RW--V */
#define NV_PFIFO_CONFIG_0_DMA_FETCH_96_BYTES             0x00000002 /* RW--V */
#define NV_PFIFO_CONFIG_0_DMA_FETCH_128_BYTES            0x00000003 /* RW--V */
#define NV_PFIFO_CONFIG_0_DMA_FETCH_160_BYTES            0x00000004 /* RW--V */
#define NV_PFIFO_CONFIG_0_DMA_FETCH_192_BYTES            0x00000005 /* RW--V */
#define NV_PFIFO_CONFIG_0_DMA_FETCH_224_BYTES            0x00000006 /* RW--V */
#define NV_PFIFO_CONFIG_0_DMA_FETCH_256_BYTES            0x00000007 /* RW--V */
#define NV_PFIFO_CONFIG_0_DMA_WATERMARK                       22:18 /* RWXVF */
#define NV_PFIFO_RAMHT                                   0x00002210 /* RW-4R */
#define NV_PFIFO_RAMHT_BASE_ADDRESS                           15:12 /* RWXVF */
#define NV_PFIFO_RAMHT_BASE_ADDRESS_0                    0x00000000 /* RWI-V */
#define NV_PFIFO_RAMHT_SIZE                                   17:16 /* RWXVF */
#define NV_PFIFO_RAMHT_SIZE_4K                           0x00000000 /* RWI-V */
#define NV_PFIFO_RAMHT_SIZE_8K                           0x00000001 /* RW--V */
#define NV_PFIFO_RAMHT_SIZE_16K                          0x00000002 /* RW--V */
#define NV_PFIFO_RAMHT_SIZE_32K                          0x00000003 /* RW--V */
#define NV_PFIFO_RAMFC                                   0x00002214 /* RW-4R */
#define NV_PFIFO_RAMFC_BASE_ADDRESS                            15:9 /* RWXVF */
#define NV_PFIFO_RAMFC_BASE_ADDRESS_1C00                 0x00001C00 /* RWI-V */
#define NV_PFIFO_RAMRO                                   0x00002218 /* RW-4R */
#define NV_PFIFO_RAMRO_BASE_ADDRESS                            15:9 /* RWXVF */
#define NV_PFIFO_RAMRO_BASE_ADDRESS_1E00                 0x00001E00 /* RWI-V */
#define NV_PFIFO_RAMRO_SIZE                                   16:16 /* RWXVF */
#define NV_PFIFO_RAMRO_SIZE_512                          0x00000000 /* RWI-V */
#define NV_PFIFO_RAMRO_SIZE_8K                           0x00000001 /* RW--V */
#define NV_PFIFO_CACHES                                  0x00002500 /* RW-4R */
#define NV_PFIFO_CACHES_REASSIGN                                0:0 /* RWIVF */
#define NV_PFIFO_CACHES_REASSIGN_DISABLED                0x00000000 /* RWI-V */
#define NV_PFIFO_CACHES_REASSIGN_ENABLED                 0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_PUSH0                            0x00003000 /* RW-4R */
#define NV_PFIFO_CACHE0_PUSH0_ACCESS                            0:0 /* RWIVF */
#define NV_PFIFO_CACHE0_PUSH0_ACCESS_DISABLED            0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE0_PUSH0_ACCESS_ENABLED             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_PUSH0                            0x00003200 /* RW-4R */
#define NV_PFIFO_CACHE1_PUSH0_ACCESS                            0:0 /* RWIVF */
#define NV_PFIFO_CACHE1_PUSH0_ACCESS_DISABLED            0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE1_PUSH0_ACCESS_ENABLED             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_PUSH1                            0x00003004 /* RW-4R */
#define NV_PFIFO_CACHE0_PUSH1_CHID                              6:0 /* RWXUF */
#define NV_PFIFO_CACHE1_PUSH1                            0x00003204 /* RW-4R */
#define NV_PFIFO_CACHE1_PUSH1_CHID                              6:0 /* RWXUF */
#define NV_PFIFO_CACHE1_DMA0                             0x00003220 /* RW-4R */
#define NV_PFIFO_CACHE1_DMA0_ACCESS                             0:0 /* RWIVF */
#define NV_PFIFO_CACHE1_DMA0_ACCESS_DISABLED             0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE1_DMA0_ACCESS_ENABLED              0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_DMA0_STATE                              4:4 /* R-IVF */
#define NV_PFIFO_CACHE1_DMA0_STATE_IDLE                  0x00000000 /* R-I-V */
#define NV_PFIFO_CACHE1_DMA0_STATE_BUSY                  0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_DMA1                             0x00003224 /* RW-4R */
#define NV_PFIFO_CACHE1_DMA1_LENGTH                            23:2 /* RWXUF */
#define NV_PFIFO_CACHE1_DMA2                             0x00003228 /* RW-4R */
#define NV_PFIFO_CACHE1_DMA2_ADDRESS                           23:2 /* RWXUF */
#define NV_PFIFO_CACHE1_DMA3                             0x0000322C /* RW-4R */
#define NV_PFIFO_CACHE1_DMA3_TARGET_NODE                        1:0 /* RWXUF */
#define NV_PFIFO_CACHE1_DMA3_TARGET_NODE_PCI             0x00000002 /* RW--V */
#define NV_PFIFO_CACHE1_DMA3_TARGET_NODE_AGP             0x00000003 /* RW--V */
#define NV_PFIFO_CACHE1_DMA_STATUS                       0x00003218 /* RW-4R */
#define NV_PFIFO_CACHE1_DMA_STATUS_METHOD                      12:2 /* RWXUF */
#define NV_PFIFO_CACHE1_DMA_STATUS_SUBCHANNEL                 15:13 /* RWXUF */
#define NV_PFIFO_CACHE1_DMA_STATUS_METHOD_COUNT               28:18 /* RWXUF */
#define NV_PFIFO_CACHE1_DMA_STATUS_REASON                     30:30 /* RWXUF */
#define NV_PFIFO_CACHE1_DMA_STATUS_REASON_DONE           0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_DMA_STATUS_REASON_NON_CACHE      0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_DMA_TLB_TAG                      0x00003230 /* RW-4R */
#define NV_PFIFO_CACHE1_DMA_TLB_TAG_ADDRESS                   23:12 /* RWXUF */
#define NV_PFIFO_CACHE1_DMA_TLB_PTE                      0x00003234 /* RW-4R */
#define NV_PFIFO_CACHE1_DMA_TLB_PTE_PAGE                        0:0 /* RWXVF */
#define NV_PFIFO_CACHE1_DMA_TLB_PTE_PAGE_NOT_PRESENT     0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_DMA_TLB_PTE_PAGE_PRESENT         0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_DMA_TLB_PTE_FRAME_ADDRESS             31:12 /* RWXUF */
#define NV_PFIFO_CACHE1_DMA_TLB_PT_BASE                  0x00003238 /* RW-4R */
#define NV_PFIFO_CACHE1_DMA_TLB_PT_BASE_ADDRESS                19:2 /* RWXUF */
#define NV_PFIFO_CACHE0_PULL0                            0x00003040 /* RW-4R */
#define NV_PFIFO_CACHE0_PULL0_ACCESS                            0:0 /* RWIVF */
#define NV_PFIFO_CACHE0_PULL0_ACCESS_DISABLED            0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE0_PULL0_ACCESS_ENABLED             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_PULL0_HASH                              4:4 /* R-XVF */
#define NV_PFIFO_CACHE0_PULL0_HASH_SUCCEEDED             0x00000000 /* R---V */
#define NV_PFIFO_CACHE0_PULL0_HASH_FAILED                0x00000001 /* R---V */
#define NV_PFIFO_CACHE0_PULL0_DEVICE                            8:8 /* R-XVF */
#define NV_PFIFO_CACHE0_PULL0_DEVICE_HARDWARE            0x00000000 /* R---V */
#define NV_PFIFO_CACHE0_PULL0_DEVICE_SOFTWARE            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_PULL0                            0x00003240 /* RW-4R */
#define NV_PFIFO_CACHE1_PULL0_ACCESS                            0:0 /* RWIVF */
#define NV_PFIFO_CACHE1_PULL0_ACCESS_DISABLED            0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE1_PULL0_ACCESS_ENABLED             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_PULL0_HASH                              4:4 /* R-XVF */
#define NV_PFIFO_CACHE1_PULL0_HASH_SUCCEEDED             0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_HASH_FAILED                0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_DEVICE                            8:8 /* R-XVF */
#define NV_PFIFO_CACHE1_PULL0_DEVICE_HARDWARE            0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_DEVICE_SOFTWARE            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_PULL1                            0x00003250 /* RW-4R */
#define NV_PFIFO_CACHE1_PULL1_CTX                               4:4 /* RWXVF */
#define NV_PFIFO_CACHE1_PULL1_CTX_CLEAN                  0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1_CTX_DIRTY                  0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_STATUS                           0x00003014 /* R--4R */
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK                         4:4 /* R--VF */
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK_NOT_EMPTY        0x00000000 /* R---V */
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK_EMPTY            0x00000001 /* R---V */
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK                        8:8 /* R--VF */
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK_NOT_FULL        0x00000000 /* R---V */
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK_FULL            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_STATUS                           0x00003214 /* R--4R */
#define NV_PFIFO_CACHE1_STATUS_RANOUT                           0:0 /* R-XVF */
#define NV_PFIFO_CACHE1_STATUS_RANOUT_FALSE              0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_RANOUT_TRUE               0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK                         4:4 /* R--VF */
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK_NOT_EMPTY        0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK                        8:8 /* R--VF */
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK_NOT_FULL        0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK_FULL            0x00000001 /* R---V */
#define NV_PFIFO_CACHE0_PUT                              0x00003010 /* RW-4R */
#define NV_PFIFO_CACHE0_PUT_ADDRESS                             2:2 /* RWXUF */
#define NV_PFIFO_CACHE1_PUT                              0x00003210 /* RW-4R */
#define NV_PFIFO_CACHE1_PUT_ADDRESS                             6:2 /* RWXUF */
#define NV_PFIFO_CACHE0_GET                              0x00003070 /* RW-4R */
#define NV_PFIFO_CACHE0_GET_ADDRESS                             2:2 /* RWXUF */
#define NV_PFIFO_CACHE1_GET                              0x00003270 /* RW-4R */
#define NV_PFIFO_CACHE1_GET_ADDRESS                             6:2 /* RWXUF */
#define NV_PFIFO_CACHE0_CTX(i)                  (0x00003080+(i)*16) /* RW-4A */
#define NV_PFIFO_CACHE0_CTX__SIZE_1                               1 /*       */
#define NV_PFIFO_CACHE0_CTX_INSTANCE                           15:0 /* RWXUF */
#define NV_PFIFO_CACHE0_CTX_DEVICE                            22:16 /* RWXUF */
#define NV_PFIFO_CACHE0_CTX_ENGINE                            23:23 /* RWXVF */
#define NV_PFIFO_CACHE0_CTX_ENGINE_SW                    0x00000000 /* RW--V */
#define NV_PFIFO_CACHE0_CTX_ENGINE_GRAPHICS              0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_CTX(i)                  (0x00003280+(i)*16) /* RW-4A */
#define NV_PFIFO_CACHE1_CTX__SIZE_1                               8 /*       */
#define NV_PFIFO_CACHE1_CTX_INSTANCE                           15:0 /* RWXUF */
#define NV_PFIFO_CACHE1_CTX_DEVICE                            22:16 /* RWXUF */
#define NV_PFIFO_CACHE1_CTX_ENGINE                            23:23 /* RWXVF */
#define NV_PFIFO_CACHE1_CTX_ENGINE_SW                    0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_CTX_ENGINE_GRAPHICS              0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_METHOD(i)                (0x00003100+(i)*8) /* RW-4A */
#define NV_PFIFO_CACHE0_METHOD__SIZE_1                            1 /*       */
#define NV_PFIFO_CACHE0_METHOD_ADDRESS                         12:2 /* RWXUF */
#define NV_PFIFO_CACHE0_METHOD_SUBCHANNEL                     15:13 /* RWXUF */
#define NV_PFIFO_CACHE1_METHOD(i)                (0x00003300+(i)*8) /* RW-4A */
#define NV_PFIFO_CACHE1_METHOD__SIZE_1                           32 /*       */
#define NV_PFIFO_CACHE1_METHOD_ADDRESS                         12:2 /* RWXUF */
#define NV_PFIFO_CACHE1_METHOD_SUBCHANNEL                     15:13 /* RWXUF */
#define NV_PFIFO_CACHE0_DATA(i)                  (0x00003104+(i)*8) /* RW-4A */
#define NV_PFIFO_CACHE0_DATA__SIZE_1                              1 /*       */
#define NV_PFIFO_CACHE0_DATA_VALUE                             31:0 /* RWXVF */
#define NV_PFIFO_CACHE1_DATA(i)                  (0x00003304+(i)*8) /* RW-4A */
#define NV_PFIFO_CACHE1_DATA__SIZE_1                             32 /*       */
#define NV_PFIFO_CACHE1_DATA_VALUE                             31:0 /* RWXVF */
#define NV_PFIFO_DEVICE(i)                       (0x00002800+(i)*4) /* R--4A */
#define NV_PFIFO_DEVICE__SIZE_1                                 128 /*       */
#define NV_PFIFO_DEVICE_CHID                                    6:0 /* R--UF */
#define NV_PFIFO_DEVICE_SWITCH                                24:24 /* R--VF */
#define NV_PFIFO_DEVICE_SWITCH_UNAVAILABLE               0x00000000 /* R---V */
#define NV_PFIFO_DEVICE_SWITCH_AVAILABLE                 0x00000001 /* R---V */
#define NV_PFIFO_RUNOUT_STATUS                           0x00002400 /* R--4R */
#define NV_PFIFO_RUNOUT_STATUS_RANOUT                           0:0 /* R--VF */
#define NV_PFIFO_RUNOUT_STATUS_RANOUT_FALSE              0x00000000 /* R---V */
#define NV_PFIFO_RUNOUT_STATUS_RANOUT_TRUE               0x00000001 /* R---V */
#define NV_PFIFO_RUNOUT_STATUS_LOW_MARK                         4:4 /* R--VF */
#define NV_PFIFO_RUNOUT_STATUS_LOW_MARK_NOT_EMPTY        0x00000000 /* R---V */
#define NV_PFIFO_RUNOUT_STATUS_LOW_MARK_EMPTY            0x00000001 /* R---V */
#define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK                        8:8 /* R--VF */
#define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK_NOT_FULL        0x00000000 /* R---V */
#define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK_FULL            0x00000001 /* R---V */
#define NV_PFIFO_RUNOUT_PUT                              0x00002410 /* RW-4R */
#define NV_PFIFO_RUNOUT_PUT_ADDRESS                            12:3 /* RWXUF */
#define NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_0                     8:3 /*       */
#define NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_1                    12:3 /*       */
#define NV_PFIFO_RUNOUT_GET                              0x00002420 /* RW-4R */
#define NV_PFIFO_RUNOUT_GET_ADDRESS                            13:3 /* RWXUF */
/* dev_audio.ref */
#define NV_PAUDIO                             0x00300FFF:0x00300000 /* RW--D */
#define NV_PAUDIO_PINS                                   0x00300000 /* RW-4R */
#define NV_PAUDIO_PINS_SI_DELAY                                 0:0 /* RWIVF */
#define NV_PAUDIO_PINS_SI_DELAY_NORMAL                   0x00000000 /* RWI-V */
#define NV_PAUDIO_PINS_SI_DELAY_I2S                      0x00000001 /* RW--V */
#define NV_PAUDIO_PINS_SI_EDGE                                  1:1 /* RWIVF */
#define NV_PAUDIO_PINS_SI_EDGE_NEGATIVE                  0x00000000 /* RWI-V */
#define NV_PAUDIO_PINS_SI_EDGE_POSITIVE                  0x00000001 /* RW--V */
#define NV_PAUDIO_PINS_SO_DELAY                                 4:4 /* RWIVF */
#define NV_PAUDIO_PINS_SO_DELAY_NORMAL                   0x00000000 /* RWI-V */
#define NV_PAUDIO_PINS_SO_DELAY_I2S                      0x00000001 /* RW--V */
#define NV_PAUDIO_PINS_SO_EDGE                                  5:5 /* RWIVF */
#define NV_PAUDIO_PINS_SO_EDGE_NEGATIVE                  0x00000000 /* RWI-V */
#define NV_PAUDIO_PINS_SO_EDGE_POSITIVE                  0x00000001 /* RW--V */
#define NV_PAUDIO_PINS_FMT_AC97                                 8:8 /* RWIVF */
#define NV_PAUDIO_PINS_SERIAL                                   9:9 /* RWIVF */
#define NV_PAUDIO_PINS_SERIAL_DISABLED                   0x00000000 /* RWI-V */
#define NV_PAUDIO_PINS_SERIAL_ENABLED                    0x00000001 /* RW--V */
#define NV_PAUDIO_PINS_LEFT_JUST                              10:10 /* RWIVF */
#define NV_PAUDIO_PINS_LEFT_SENSE                             11:11 /* RWIVF */
#define NV_PAUDIO_PINS_LEFT_LOW                          0x00000000 /* RWI-V */
#define NV_PAUDIO_PINS_LEFT_HIGH                         0x00000001 /* RW--V */
#define NV_PAUDIO_PINS_CONTROL                                14:12 /* RWIVF */
#define NV_PAUDIO_PINS_CONTROL_0                         0x00000000 /* RWI-V */
#define NV_PAUDIO_PINS_CONTROL_1                         0x00000001 /* RW--V */
#define NV_PAUDIO_PINS_CONTROL_2                         0x00000002 /* RW--V */
#define NV_PAUDIO_PINS_CONTROL_3                         0x00000003 /* RW--V */
#define NV_PAUDIO_PINS_CONTROL_4                         0x00000004 /* RW--V */
#define NV_PAUDIO_PINS_CONTROL_5                         0x00000005 /* RW--V */
#define NV_PAUDIO_PINS_CONTROL_6                         0x00000006 /* RW--V */
#define NV_PAUDIO_PINS_CONTROL_7                         0x00000007 /* RW--V */
#define NV_PAUDIO_GREEN_0                                0x003000C0 /* RW-4R */
#define NV_PAUDIO_GREEN_0_CODEC                                 0:0 /* RWIVF */
#define NV_PAUDIO_GREEN_0_CODEC_DISABLED                 0x00000000 /* RWI-V */
#define NV_PAUDIO_GREEN_0_CODEC_ENABLED                  0x00000001 /* RW--V */
#define NV_PAUDIO_GREEN_0_AUDIO                                 4:4 /* RWIVF */
#define NV_PAUDIO_GREEN_0_AUDIO_DISABLED                 0x00000000 /* RWI-V */
#define NV_PAUDIO_GREEN_0_AUDIO_ENABLED                  0x00000001 /* RW--V */
#define NV_PAUDIO_GREEN_0_APUMP                                 8:8 /* RWIVF */
#define NV_PAUDIO_GREEN_0_APUMP_DISABLED                 0x00000000 /* RWI-V */
#define NV_PAUDIO_GREEN_0_APUMP_ENABLED                  0x00000001 /* RW--V */
#define NV_PAUDIO_GREEN_0_CODEC_WARM                            8:8 /* RWIVF */
#define NV_PAUDIO_GREEN_0_CODEC_WARM_END                 0x00000000 /* RWI-V */
#define NV_PAUDIO_GREEN_0_APUMP_WARM_UP                  0x00000001 /* RW--V */
#define NV_PAUDIO_INTR_0                                 0x00300100 /* RW-4R */
#define NV_PAUDIO_INTR_0_NOTIFY                                 0:0 /* RWIVF */
#define NV_PAUDIO_INTR_0_NOTIFY_NOT_PENDING              0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_0_NOTIFY_PENDING                  0x00000001 /* R---V */
#define NV_PAUDIO_INTR_0_NOTIFY_RESET                    0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_0_OVERFLOW                               4:4 /* RWIVF */
#define NV_PAUDIO_INTR_0_OVERFLOW_NOT_PENDING            0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_0_OVERFLOW_PENDING                0x00000001 /* R---V */
#define NV_PAUDIO_INTR_0_OVERFLOW_RESET                  0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_0_AN_OVFL                                8:8 /* RWIVF */
#define NV_PAUDIO_INTR_0_AN_OVFL_NOT_PENDING             0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_0_AN_OVFL_PENDING                 0x00000001 /* R---V */
#define NV_PAUDIO_INTR_0_AN_OVFL_RESET                   0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_0_AN_UNFL                              12:12 /* RWIVF */
#define NV_PAUDIO_INTR_0_AN_UNFL_NOT_PENDING             0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_0_AN_UNFL_PENDING                 0x00000001 /* R---V */
#define NV_PAUDIO_INTR_0_AN_UNFL_RESET                   0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_1                                 0x00300104 /* RW-4R */
#define NV_PAUDIO_INTR_1_SYNC                                   0:0 /* RWIVF */
#define NV_PAUDIO_INTR_1_SYNC_NOT_PENDING                0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_1_SYNC_PENDING                    0x00000001 /* R---V */
#define NV_PAUDIO_INTR_1_SYNC_RESET                      0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_1_IDLE                                   4:4 /* RWIVF */
#define NV_PAUDIO_INTR_1_IDLE_NOT_PENDING                0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_1_IDLE_PENDING                    0x00000001 /* R---V */
#define NV_PAUDIO_INTR_1_IDLE_RESET                      0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_1_NEAR_BUSY                              8:8 /* RWIVF */
#define NV_PAUDIO_INTR_1_NEAR_BUSY_NOT_PENDING           0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_1_NEAR_BUSY_PENDING               0x00000001 /* R---V */
#define NV_PAUDIO_INTR_1_NEAR_BUSY_RESET                 0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_1_TOO_BUSY                             12:12 /* RWIVF */
#define NV_PAUDIO_INTR_1_TOO_BUSY_NOT_PENDING            0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_1_TOO_BUSY_PENDING                0x00000001 /* R---V */
#define NV_PAUDIO_INTR_1_TOO_BUSY_RESET                  0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_2                                 0x00300108 /* RW-4R */
#define NV_PAUDIO_INTR_2_DMA_INSTANCE                           0:0 /* RWIVF */
#define NV_PAUDIO_INTR_2_DMA_INSTANCE_NOT_PENDING        0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_2_DMA_INSTANCE_PENDING            0x00000001 /* R---V */
#define NV_PAUDIO_INTR_2_DMA_INSTANCE_RESET              0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_2_DMA_PRESENT                            4:4 /* RWIVF */
#define NV_PAUDIO_INTR_2_DMA_PRESENT_NOT_PENDING         0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_2_DMA_PRESENT_PENDING             0x00000001 /* R---V */
#define NV_PAUDIO_INTR_2_DMA_PRESENT_RESET               0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_2_DMA_PROTECT                            8:8 /* RWIVF */
#define NV_PAUDIO_INTR_2_DMA_PROTECT_NOT_PENDING         0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_2_DMA_PROTECT_PENDING             0x00000001 /* R---V */
#define NV_PAUDIO_INTR_2_DMA_PROTECT_RESET               0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_2_DMA_BUS_ERROR                        12:12 /* RWIVF */
#define NV_PAUDIO_INTR_2_DMA_BUS_ERROR_NOT_PENDING       0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_2_DMA_BUS_ERROR_PENDING           0x00000001 /* R---V */
#define NV_PAUDIO_INTR_2_DMA_BUS_ERROR_RESET             0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_3                                 0x0030010C /* RW-4R */
#define NV_PAUDIO_INTR_3_PINS_0                                 0:0 /* RWIVF */
#define NV_PAUDIO_INTR_3_PINS_0_NOT_PENDING              0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_3_PINS_0_PENDING                  0x00000001 /* R---V */
#define NV_PAUDIO_INTR_3_PINS_0_RESET                    0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_3_PINS_1                                 4:4 /* RWIVF */
#define NV_PAUDIO_INTR_3_PINS_1_NOT_PENDING              0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_3_PINS_1_PENDING                  0x00000001 /* R---V */
#define NV_PAUDIO_INTR_3_PINS_1_RESET                    0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_3_PINS_2                                 8:8 /* RWIVF */
#define NV_PAUDIO_INTR_3_PINS_2_NOT_PENDING              0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_3_PINS_2_PENDING                  0x00000001 /* R---V */
#define NV_PAUDIO_INTR_3_PINS_2_RESET                    0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_3_PINS_3                               12:12 /* RWIVF */
#define NV_PAUDIO_INTR_3_PINS_3_NOT_PENDING              0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_3_PINS_3_PENDING                  0x00000001 /* R---V */
#define NV_PAUDIO_INTR_3_PINS_3_RESET                    0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_EN_0                              0x00300140 /* RW-4R */
#define NV_PAUDIO_INTR_EN_0_NOTIFY                              0:0 /* RWIVF */
#define NV_PAUDIO_INTR_EN_0_NOTIFY_DISABLED              0x00000000 /* RWI-V */
#define NV_PAUDIO_INTR_EN_0_NOTIFY_ENABLED               0x00000001 /* RW--V */
#define NV_PAUDIO_INTR_EN_0_OVERFLOW                            4:4 /* RWIVF */
#define NV_PAUDIO_INTR_EN_0_OVERFLOW_DISABLED            0x00000000 /* RWI-V */
#define NV_PAUDIO_INTR_EN_0_OVERFLOW_ENABLED             0x00000001 /* RW--V */
#define NV_PAUDIO_INTR_EN_0_AN_OVFL                             8:8 /* RWIVF */
#define NV_PAUDIO_INTR_EN_0_AN_OVFL_DISABLED             0x00000000 /* RWI-V */
#define NV_PAUDIO_INTR_EN_0_AN_OVFL_ENABLED              0x00000001 /* RW--V */
#define NV_PAUDIO_INTR_EN_0_AN_UNFL                           12:12 /* RWIVF */
#define NV_PAUDIO_INTR_EN_0_AN_UNFL_DISABLED             0x00000000 /* RWI-V */
#define NV_PAUDIO_INTR_EN_0_AN_UNFL_ENABLED              0x00000001 /* RW--V */
#define NV_PAUDIO_INTR_EN_1                              0x00300144 /* RW-4R */
#define NV_PAUDIO_INTR_EN_1_SYNC                                0:0 /* RWIVF */
#define NV_PAUDIO_INTR_EN_1_SYNC_DISABLED                0x00000000 /* RWI-V */
#define NV_PAUDIO_INTR_EN_1_SYNC_ENABLED                 0x00000001 /* RW--V */
#define NV_PAUDIO_INTR_EN_1_IDLE                                4:4 /* RWIVF */
#define NV_PAUDIO_INTR_EN_1_IDLE_DISABLED                0x00000000 /* RWI-V */
#define NV_PAUDIO_INTR_EN_1_IDLE_ENABLED                 0x00000001 /* RW--V */
#define NV_PAUDIO_INTR_EN_1_NEAR_BUSY                           8:8 /* RWIVF */
#define NV_PAUDIO_INTR_EN_1_NEAR_BUSY_DISABLED           0x00000000 /* RWI-V */
#define NV_PAUDIO_INTR_EN_1_NEAR_BUSY_ENABLED            0x00000001 /* RW--V */
#define NV_PAUDIO_INTR_EN_1_TOO_BUSY                          12:12 /* RWIVF */
#define NV_PAUDIO_INTR_EN_1_TOO_BUSY_DISABLED            0x00000000 /* RWI-V */
#define NV_PAUDIO_INTR_EN_1_TOO_BUSY_ENABLED             0x00000001 /* RW--V */
#define NV_PAUDIO_INTR_EN_2                              0x00300148 /* RW-4R */
#define NV_PAUDIO_INTR_EN_2_DMA_INSTANCE                        0:0 /* RWIVF */
#define NV_PAUDIO_INTR_EN_2_DMA_INSTANCE_NOT_PENDING     0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_EN_2_DMA_INSTANCE_PENDING         0x00000001 /* R---V */
#define NV_PAUDIO_INTR_EN_2_DMA_INSTANCE_RESET           0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_EN_2_DMA_PRESENT                         4:4 /* RWIVF */
#define NV_PAUDIO_INTR_EN_2_DMA_PRESENT_NOT_PENDING      0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_EN_2_DMA_PRESENT_PENDING          0x00000001 /* R---V */
#define NV_PAUDIO_INTR_EN_2_DMA_PRESENT_RESET            0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_EN_2_DMA_PROTECT                         8:8 /* RWIVF */
#define NV_PAUDIO_INTR_EN_2_DMA_PROTECT_NOT_PENDING      0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_EN_2_DMA_PROTECT_PENDING          0x00000001 /* R---V */
#define NV_PAUDIO_INTR_EN_2_DMA_PROTECT_RESET            0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_EN_2_DMA_BUS_ERROR                     12:12 /* RWIVF */
#define NV_PAUDIO_INTR_EN_2_DMA_BUS_ERROR_NOT_PENDING    0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_EN_2_DMA_BUS_ERROR_PENDING        0x00000001 /* R---V */
#define NV_PAUDIO_INTR_EN_2_DMA_BUS_ERROR_RESET          0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_EN_3                              0x0030014C /* RW-4R */
#define NV_PAUDIO_INTR_EN_3_PINS_0                              0:0 /* RWIVF */
#define NV_PAUDIO_INTR_EN_3_PINS_0_NOT_PENDING           0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_EN_3_PINS_0_PENDING               0x00000001 /* R---V */
#define NV_PAUDIO_INTR_EN_3_PINS_0_RESET                 0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_EN_3_PINS_1                              4:4 /* RWIVF */
#define NV_PAUDIO_INTR_EN_3_PINS_1_NOT_PENDING           0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_EN_3_PINS_1_PENDING               0x00000001 /* R---V */
#define NV_PAUDIO_INTR_EN_3_PINS_1_RESET                 0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_EN_3_PINS_2                              8:8 /* RWIVF */
#define NV_PAUDIO_INTR_EN_3_PINS_2_NOT_PENDING           0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_EN_3_PINS_2_PENDING               0x00000001 /* R---V */
#define NV_PAUDIO_INTR_EN_3_PINS_2_RESET                 0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_EN_3_PINS_3                            12:12 /* RWIVF */
#define NV_PAUDIO_INTR_EN_3_PINS_3_NOT_PENDING           0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_EN_3_PINS_3_PENDING               0x00000001 /* R---V */
#define NV_PAUDIO_INTR_EN_3_PINS_3_RESET                 0x00000001 /* -W--V */
#define NV_PAUDIO_BLOCK_NEW                              0x00300200 /* RW-4R */
#define NV_PAUDIO_BLOCK_NEW_LENGTH                              7:0 /* RWXUF */
#define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_LG2                     14:12 /* RWXUF */
#define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_1                  0x00000000 /* RW--V */
#define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_2                  0x00000001 /* RW--V */
#define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_4                  0x00000002 /* RW--V */
#define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_8                  0x00000003 /* RW--V */
#define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_16                 0x00000004 /* RW--V */
#define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_32                 0x00000005 /* RW--V */
#define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_64                 0x00000006 /* RW--V */
#define NV_PAUDIO_BLOCK_NEW_SUB_BLOCK_128                0x00000007 /* RW--V */
#define NV_PAUDIO_BLOCK_ENGINE                           0x00300204 /* RW-4R */
#define NV_PAUDIO_BLOCK_ENGINE_LENGTH                           7:0 /* RWXUF */
#define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_LG2                  14:12 /* R-XVF */
#define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_1               0x00000000 /* R---V */
#define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_2               0x00000001 /* R---V */
#define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_4               0x00000002 /* R---V */
#define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_8               0x00000003 /* R---V */
#define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_16              0x00000004 /* R---V */
#define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_32              0x00000005 /* R---V */
#define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_64              0x00000006 /* R---V */
#define NV_PAUDIO_BLOCK_ENGINE_SUB_BLOCK_128             0x00000007 /* R---V */
#define NV_PAUDIO_RAMAU                                  0x00300210 /* RW-4R */
#define NV_PAUDIO_RAMAU_BASE_ADDRESS                          15:12 /* RWXVF */
#define NV_PAUDIO_RAMAU_BASE_ADDRESS_1000                0x00001000 /* RWI-V */
#define NV_PAUDIO_ISA_SEL                                0x00300280 /* RW-4R */
#define NV_PAUDIO_ISA_SEL_IRQ                                   2:0 /* RWXVF */
#define NV_PAUDIO_ISA_SEL_IRQ_0                          0x00000001 /* RWI-V */
#define NV_PAUDIO_ISA_SEL_IRQ_1                          0x00000002 /* RWI-V */
#define NV_PAUDIO_ISA_SEL_IRQ_2                          0x00000004 /* RWI-V */
#define NV_PAUDIO_ISA_SEL_DRQ                                   9:8 /* RWXVF */
#define NV_PAUDIO_ISA_SEL_DRQ_0                          0x00000001 /* RWI-V */
#define NV_PAUDIO_ISA_SEL_DRQ_1                          0x00000002 /* RWI-V */
#define NV_PAUDIO_TLB_PTE                                0x00300310 /* R-X4R */
#define NV_PAUDIO_TLB_PTE_PAGE                                31:10 /* R-XVF */
#define NV_PAUDIO_TLB_PTE_READ_ONLY                             1:1 /* R-XVF */
#define NV_PAUDIO_TLB_PTE_PRESENT                               0:0 /* R-XVF */
#define NV_PAUDIO_TLB_TAG                                0x00300320 /* R-X4R */
#define NV_PAUDIO_DMA_TLB_TAG                                 31:12 /* R-XVF */
#define NV_PAUDIO_DMAIA                                  0x00300330 /* R-X4R */
#define NV_PAUDIO_DMA_INSTANCE                                 31:2 /* R-XVF */
#define NV_PAUDIO_DMA_INSTANCE_TARGET                           1:0 /* R-XVF */
#define NV_PAUDIO_NEAR_MARK                              0x00300400 /* RW-4R */
#define NV_PAUDIO_NEAR_MARK_COUNT                               7:0 /* RWXUF */
#define NV_PAUDIO_SAMPLE_COUNT                           0x00300410 /* R--4R */
#define NV_PAUDIO_SAMPLE_COUNT_VALUE                            7:0 /* R-XUF */
#define NV_PAUDIO_SAMPLE_COUNT_BUFFER                           8:8 /* R-XUF */
#define NV_PAUDIO_PROG_COUNTER                           0x00300420 /* R--4R */
#define NV_PAUDIO_PROG_COUNTER_VALUE                            7:0 /* R-XUF */
#define NV_PAUDIO_AN_VOLUME                              0x00300430 /* RWI4R */
#define NV_PAUDIO_AN_VOLUME_INITIAL                               7 /* RWI-V */
#define NV_PAUDIO_AN_VOLUME_VALUE                               2:0 /* RWXUF */
#define NV_PAUDIO_TERM_USAGE                             0x00300C0C /* RW-4R */
#define NV_PAUDIO_TERMINATION_LEVEL                           31:16 /* RWXUF */
#define NV_PAUDIO_TERMINATION_LEVEL_DISABLED             0x00000000 /* RW--V */
#define NV_PAUDIO_USAGE_LEVEL                                  15:0 /* RWXUF */
#define NV_PAUDIO_CONTEXT                                0x00300E10 /* RW-4R */
#define NV_PAUDIO_CONTEXT_INSTANCE                             31:0 /* RWXUF */
#define NV_PAUDIO_BLASTER(i)                       (0x00301000+(i)) /* RW-1A */
#define NV_PAUDIO_BLASTER__SIZE_1                                 1 /*       */
#define NV_PAUDIO_BLASTER_DATA                                  7:0 /* RW-VF */
#define NV_PAUDIO_CODEC(i)                       (0x00302800+(i)*4) /* RW-4A */
#define NV_PAUDIO_CODEC__SIZE_1                                 256 /*       */
#define NV_PAUDIO_CODEC_DATA                                   19:0 /* RW-VF */
#define NV_PAUDIO_INST_TARGET_NVM                        0x00000000 /* RW--V */
#define NV_PAUDIO_INST_TARGET_CART                       0x00000001 /* RW--V */
#define NV_PAUDIO_INST_TARGET_PCI                        0x00000002 /* RW--V */
#define NV_PAUDIO_ROOT_INPUT                             0x00300C00 /* RW-4R */
#define NV_PAUDIO_ROOT_INPUT_TARGET                             1:0 /* RWXUF */
#define NV_PAUDIO_ROOT_INPUT_INSTANCE                          31:2 /* RWXUF */
#define NV_PAUDIO_ROOT_OUTPUT                            0x00300C04 /* RW-4R */
#define NV_PAUDIO_ROOT_OUTPUT_TARGET                            1:0 /* RWXUF */
#define NV_PAUDIO_ROOT_OUTPUT_INSTANCE                         31:2 /* RWXUF */
#define NV_PAUDIO_ROOT_NOTE                              0x00300C08 /* RW-4R */
#define NV_PAUDIO_ROOT_NOTE_TARGET                              1:0 /* RWXUF */
#define NV_PAUDIO_ROOT_NOTE_INSTANCE                           31:2 /* RWXUF */
#define NV_PAUDIO_DIAG(i)                        (0x00300C10+(i)*4) /* RW-4A */
#define NV_PAUDIO_DIAG__SIZE_1                                  252 /*       */
#define NV_PAUDIO_DIAG_DATA                                    31:0 /* RW-VF */
/* dev_graphics.ref */
#define NV_PGRAPH                             0x00401FFF:0x00400000 /* RW--D */
#define NV_PGRAPH_DEBUG_0                                0x00400080 /* RW-4R */
#define NV_PGRAPH_DEBUG_0_STATE                                 0:0 /* CW-VF */
#define NV_PGRAPH_DEBUG_0_STATE_NORMAL                   0x00000000 /* CW--V */
#define NV_PGRAPH_DEBUG_0_STATE_RESET                    0x00000001 /* -W--V */
#define NV_PGRAPH_DEBUG_0_AP_PIPE_STATE                         1:1 /* CW-VF */
#define NV_PGRAPH_DEBUG_0_AP_PIPE_STATE_NORMAL           0x00000000 /* CW--V */
#define NV_PGRAPH_DEBUG_0_AP_PIPE_STATE_RESET            0x00000001 /* -W--V */
#define NV_PGRAPH_DEBUG_0_CACHE_STATE                           2:2 /* CW-VF */
#define NV_PGRAPH_DEBUG_0_CACHE_STATE_NORMAL             0x00000000 /* CW--V */
#define NV_PGRAPH_DEBUG_0_CACHE_STATE_RESET              0x00000001 /* -W--V */
#define NV_PGRAPH_DEBUG_0_3D_PIPE_STATE                         3:3 /* CW-VF */
#define NV_PGRAPH_DEBUG_0_3D_PIPE_STATE_NORMAL           0x00000000 /* CW--V */
#define NV_PGRAPH_DEBUG_0_3D_PIPE_STATE_RESET            0x00000001 /* -W--V */
#define NV_PGRAPH_DEBUG_0_BULK_READS                            4:4 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_BULK_READS_DISABLED            0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_BULK_READS_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_SPARE1                                8:8 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_SPARE1_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_SPARE1_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_SPARE2                              12:12 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_SPARE2_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_SPARE2_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_TILING                              16:16 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_TILING_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_TILING_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS_2D                  20:20 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS_2D_DISABLED    0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS_2D_ENABLED     0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS_3D                  21:21 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS_3D_DISABLED    0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS_3D_ENABLED     0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_DRAWDIR_AUTO                        24:24 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_DRAWDIR_AUTO_DISABLED          0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_DRAWDIR_AUTO_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_DRAWDIR_Y                           25:25 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_DRAWDIR_Y_DECR                 0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_DRAWDIR_Y_INCR                 0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_ALPHA_ABORT                         28:28 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_ALPHA_ABORT_DISABLED           0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_ALPHA_ABORT_ENABLED            0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1                                0x00400084 /* RW-4R */
#define NV_PGRAPH_DEBUG_1_VOLATILE_RESET                        0:0 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_VOLATILE_RESET_NOT_LAST        0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_VOLATILE_RESET_LAST            0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_DMA_ACTIVITY                          4:4 /* CW-VF */
#define NV_PGRAPH_DEBUG_1_DMA_ACTIVITY_IGNORE            0x00000000 /* CW--V */
#define NV_PGRAPH_DEBUG_1_DMA_ACTIVITY_CANCEL            0x00000001 /* -W--V */
#define NV_PGRAPH_DEBUG_1_TURBO3D_2X                            8:8 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_TURBO3D_2X__DISABLED           0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_TURBO3D_2X_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_TURBO3D_4X                            9:9 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_TURBO3D_4X__DISABLED           0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_TURBO3D_4X_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_TRI_OPTS                            12:12 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_TRI_OPTS_DISABLED              0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_TRI_OPTS_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_TRICLIP_OPTS                        13:13 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_TRICLIP_OPTS_DISABLED          0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_TRICLIP_OPTS_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_INSTANCE                            16:16 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_INSTANCE_DISABLED              0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_INSTANCE_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_CTX                                 20:20 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_CTX_DISABLED                   0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_CTX_ENABLED                    0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_CACHE                               24:24 /* CW-VF */
#define NV_PGRAPH_DEBUG_1_CACHE_IGNORE                   0x00000000 /* CW--V */
#define NV_PGRAPH_DEBUG_1_CACHE_FLUSH                    0x00000001 /* -W--V */
#define NV_PGRAPH_DEBUG_1_SPARE1                              28:28 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_SPARE1_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_SPARE1_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2                                0x00400088 /* RW-4R */
#define NV_PGRAPH_DEBUG_2_AVOID_RMW_BLEND                       0:0 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_AVOID_RMW_BLEND_DISABLED       0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_AVOID_RMW_BLEND_ENABLED        0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_SPARE1                                4:4 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_SPARE1_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_SPARE1_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_DPWR_FIFO                             8:8 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_DPWR_FIFO_DISABLED             0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_DPWR_FIFO_ENABLED              0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_BILINEAR_3D                         12:12 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_BILINEAR_3D_DISABLED           0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_BILINEAR_3D_ENABLED            0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_ANISOTROPIC_3D                      13:13 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_ANISOTROPIC_3D_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_ANISOTROPIC_3D_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_FOG_3D                              14:14 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_FOG_3D_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_FOG_3D_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_LIGHTING_3D                         15:15 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_LIGHTING_3D_DISABLED           0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_LIGHTING_3D_ENABLED            0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_BILINEAR_2D                         16:16 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_BILINEAR_2D_DISABLED           0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_BILINEAR_2D_ENABLED            0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_ANISOTROPIC_2D                      17:17 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_ANISOTROPIC_2D_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_ANISOTROPIC_2D_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_COELESCE_D3D                        20:20 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_COELESCE_D3D_DISABLED          0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_COELESCE_D3D_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_COELESCE_PTZ                        21:21 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_COELESCE_PTZ_DISABLED          0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_COELESCE_PTZ_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_PREFETCH                            24:24 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_PREFETCH_DISABLED              0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_PREFETCH_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_VOLATILE_RESET                      28:28 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_VOLATILE_RESET_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_VOLATILE_RESET_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3                                0x0040008C /* RW-4R */
#define NV_PGRAPH_DEBUG_3_CULLING                               0:0 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_CULLING_DISABLED               0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_CULLING_ENABLED                0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_FAST_DATA_STRTCH                      4:4 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_FAST_DATA_STRTCH_DISABLED      0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_FAST_DATA_STRTCH_ENABLED       0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_FAST_DATA_D3D                         5:5 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_FAST_DATA_D3D_DISABLED         0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_FAST_DATA_D3D_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_ZFLUSH                                7:7 /* CW-VF */
#define NV_PGRAPH_DEBUG_3_ZFLUSH_IGNORE                  0x00000000 /* CW--V */
#define NV_PGRAPH_DEBUG_3_ZFLUSH_ACTIVATE                0x00000001 /* -W--V */
#define NV_PGRAPH_DEBUG_3_AUTOZFLUSH_PTZ                        8:8 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_AUTOZFLUSH_PTZ_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_AUTOZFLUSH_PTZ_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_AUTOZFLUSH_D3D                        9:9 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_AUTOZFLUSH_D3D_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_AUTOZFLUSH_D3D_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_SLOT_CONFLICT_PTZ                   10:10 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_SLOT_CONFLICT_PTZ_DISABLED     0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_SLOT_CONFLICT_PTZ_ENABLED      0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_SLOT_CONFLICT_D3D                   11:11 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_SLOT_CONFLICT_D3D_DISABLED     0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_SLOT_CONFLICT_D3D_ENABLED      0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_EARLYZ_ABORT                        12:12 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_EARLYZ_ABORT_DISABLED          0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_EARLYZ_ABORT_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_TRIEND_FLUSH                        13:13 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_TRIEND_FLUSH_DISABLED          0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_TRIEND_FLUSH_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_ZFIFO_NOP_OPT                       14:14 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_ZFIFO_NOP_OPT_DISABLED         0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_ZFIFO_NOP_OPT_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_DITHER_3D                           15:15 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_DITHER_3D_DISABLED             0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_DITHER_3D_ENABLED              0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_FORCE_CREAD                         16:16 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_FORCE_CREAD_DISABLED           0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_FORCE_CREAD_ENABLED            0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_FORCE_ZREAD                         17:17 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_FORCE_ZREAD_DISABLED           0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_FORCE_ZREAD_ENABLED            0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_DATA_CHECK                          20:20 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_DATA_CHECK_DISABLED            0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_DATA_CHECK_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_DATA_CHECK_FAIL                     21:21 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_DATA_CHECK_FAIL_DISABLED       0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_DATA_CHECK_FAIL_ENABLED        0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_FORMAT_CHECK                        22:22 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_FORMAT_CHECK_DISABLED          0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_FORMAT_CHECK_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_HONOR_ALPHA                         24:24 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_HONOR_ALPHA_DISABLED           0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_HONOR_ALPHA_ENABLED            0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_SPARE1                              28:28 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_SPARE1_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_SPARE1_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_0                                 0x00400100 /* RW-4R */
#define NV_PGRAPH_INTR_0_RESERVED                               0:0 /* RW-VF */
#define NV_PGRAPH_INTR_0_RESERVED_NOT_PENDING            0x00000000 /* R---V */
#define NV_PGRAPH_INTR_0_RESERVED_PENDING                0x00000001 /* R---V */
#define NV_PGRAPH_INTR_0_RESERVED_RESET                  0x00000001 /* -W--V */
#define NV_PGRAPH_INTR_0_CONTEXT_SWITCH                         4:4 /* RWIVF */
#define NV_PGRAPH_INTR_0_CONTEXT_SWITCH_NOT_PENDING      0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_0_CONTEXT_SWITCH_PENDING          0x00000001 /* R---V */
#define NV_PGRAPH_INTR_0_CONTEXT_SWITCH_RESET            0x00000001 /* -W--V */
#define NV_PGRAPH_INTR_0_VBLANK                                 8:8 /* RWIVF */
#define NV_PGRAPH_INTR_0_VBLANK_NOT_PENDING              0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_0_VBLANK_PENDING                  0x00000001 /* R---V */
#define NV_PGRAPH_INTR_0_VBLANK_RESET                    0x00000001 /* -W--V */
#define NV_PGRAPH_INTR_0_RANGE                                12:12 /* RWIVF */
#define NV_PGRAPH_INTR_0_RANGE_NOT_PENDING               0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_0_RANGE_PENDING                   0x00000001 /* R---V */
#define NV_PGRAPH_INTR_0_RANGE_RESET                     0x00000001 /* -W--V */
#define NV_PGRAPH_INTR_0_METHOD_COUNT                         16:16 /* RWIVF */
#define NV_PGRAPH_INTR_0_METHOD_COUNT_NOT_PENDING        0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_0_METHOD_COUNT_PENDING            0x00000001 /* R---V */
#define NV_PGRAPH_INTR_0_METHOD_COUNT_RESET              0x00000001 /* -W--V */
#define NV_PGRAPH_INTR_0_FORMAT                               20:20 /* RWIVF */
#define NV_PGRAPH_INTR_0_FORMAT_NOT_PENDING              0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_0_FORMAT_PENDING                  0x00000001 /* R---V */
#define NV_PGRAPH_INTR_0_FORMAT_RESET                    0x00000001 /* -W--V */
#define NV_PGRAPH_INTR_0_COMPLEX_CLIP                         24:24 /* RWIVF */
#define NV_PGRAPH_INTR_0_COMPLEX_CLIP_NOT_PENDING        0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_0_COMPLEX_CLIP_PENDING            0x00000001 /* R---V */
#define NV_PGRAPH_INTR_0_COMPLEX_CLIP_RESET              0x00000001 /* -W--V */
#define NV_PGRAPH_INTR_0_NOTIFY                               28:28 /* RWIVF */
#define NV_PGRAPH_INTR_0_NOTIFY_NOT_PENDING              0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_0_NOTIFY_PENDING                  0x00000001 /* R---V */
#define NV_PGRAPH_INTR_0_NOTIFY_RESET                    0x00000001 /* -W--V */
#define NV_PGRAPH_INTR_1                                 0x00400104 /* RW-4R */
#define NV_PGRAPH_INTR_1_METHOD                                 0:0 /* RWIVF */
#define NV_PGRAPH_INTR_1_METHOD_NOT_PENDING              0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_1_METHOD_PENDING                  0x00000001 /* R---V */
#define NV_PGRAPH_INTR_1_METHOD_RESET                    0x00000001 /* -W--V */
#define NV_PGRAPH_INTR_1_DATA                                   4:4 /* RWIVF */
#define NV_PGRAPH_INTR_1_DATA_NOT_PENDING                0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_1_DATA_PENDING                    0x00000001 /* R---V */
#define NV_PGRAPH_INTR_1_DATA_RESET                      0x00000001 /* -W--V */
#define NV_PGRAPH_INTR_1_DOUBLE_NOTIFY                        12:12 /* RWIVF */
#define NV_PGRAPH_INTR_1_DOUBLE_NOTIFY_NOT_PENDING       0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_1_DOUBLE_NOTIFY_PENDING           0x00000001 /* R---V */
#define NV_PGRAPH_INTR_1_DOUBLE_NOTIFY_RESET             0x00000001 /* -W--V */
#define NV_PGRAPH_INTR_1_CTXSW_NOTIFY                         16:16 /* RWIVF */
#define NV_PGRAPH_INTR_1_CTXSW_NOTIFY_NOT_PENDING        0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_1_CTXSW_NOTIFY_PENDING            0x00000001 /* R---V */
#define NV_PGRAPH_INTR_1_CTXSW_NOTIFY_RESET              0x00000001 /* -W--V */
#define NV_PGRAPH_INTR_EN_0                              0x00400140 /* RW-4R */
#define NV_PGRAPH_INTR_EN_0_RESERVED                            0:0 /* RWIVF */
#define NV_PGRAPH_INTR_EN_0_RESERVED_DISABLED            0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_0_RESERVED_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_0_CONTEXT_SWITCH                      4:4 /* RWIVF */
#define NV_PGRAPH_INTR_EN_0_CONTEXT_SWITCH_DISABLED      0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_0_CONTEXT_SWITCH_ENABLED       0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_0_VBLANK                              8:8 /* RWIVF */
#define NV_PGRAPH_INTR_EN_0_VBLANK_DISABLED              0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_0_VBLANK_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_0_RANGE                             12:12 /* RWIVF */
#define NV_PGRAPH_INTR_EN_0_RANGE_DISABLED               0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_0_RANGE_ENABLED                0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_0_METHOD_COUNT                      16:16 /* RWIVF */
#define NV_PGRAPH_INTR_EN_0_METHOD_COUNT_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_0_METHOD_COUNT_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_0_FORMAT                            20:20 /* RWIVF */
#define NV_PGRAPH_INTR_EN_0_FORMAT_DISABLED              0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_0_FORMAT_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_0_COMPLEX_CLIP                      24:24 /* RWIVF */
#define NV_PGRAPH_INTR_EN_0_COMPLEX_CLIP_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_0_COMPLEX_CLIP_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_0_NOTIFY                            28:28 /* RWIVF */
#define NV_PGRAPH_INTR_EN_0_NOTIFY_DISABLED              0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_0_NOTIFY_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_1                              0x00400144 /* RW-4R */
#define NV_PGRAPH_INTR_EN_1_METHOD                              0:0 /* RWIVF */
#define NV_PGRAPH_INTR_EN_1_METHOD_DISABLED              0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_1_METHOD_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_1_DATA                                4:4 /* RWIVF */
#define NV_PGRAPH_INTR_EN_1_DATA_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_1_DATA_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_1_DOUBLE_NOTIFY                     12:12 /* RWIVF */
#define NV_PGRAPH_INTR_EN_1_DOUBLE_NOTIFY_DISABLED       0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_1_DOUBLE_NOTIFY_ENABLED        0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_1_CTXSW_NOTIFY                      16:16 /* RWIVF */
#define NV_PGRAPH_INTR_EN_1_CTXSW_NOTIFY_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_1_CTXSW_NOTIFY_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH                             0x00400180 /* RW-4R */
#define NV_PGRAPH_CTX_SWITCH_COLOR                              2:0 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_COLOR_R5G5B5                0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_COLOR_R8G8B8                0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_COLOR_R10G10B10             0x00000002 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_COLOR_Y8                    0x00000003 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_COLOR_Y16                   0x00000004 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_COLOR_V8Y18U8Y08            0x00000005 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_COLOR_Y18V8Y08U8            0x00000006 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_COLOR_Y420                  0x00000007 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_ALPHA                              3:3 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_ALPHA_DISABLED              0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_ALPHA_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_SPARE1                             4:4 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_SPARE1_DISABLED             0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_SPARE1_ENABLED              0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_MONO_FORMAT                        8:8 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_MONO_FORMAT_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_MONO_FORMAT_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_DAC_BYPASS                         9:9 /* RWXVF */
#define NV_PGRAPH_CTX_SWITCH_DAC_BYPASS_DISABLED         0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_DAC_BYPASS_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_SPARE2                           10:10 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_SPARE2_DISABLED             0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_SPARE2_ENABLED              0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_ZWRITE                           12:12 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_ZWRITE_DISABLED             0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_ZWRITE_ENABLED              0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_CHROMA_KEY                       13:13 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_CHROMA_KEY_DISABLED         0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_CHROMA_KEY_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PLANE_MASK                       14:14 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_PLANE_MASK_DISABLED         0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_PLANE_MASK_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_USER_CLIP                        15:15 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_USER_CLIP_DISABLED          0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_USER_CLIP_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_SRC_BUFFER                       17:16 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_SRC_BUFFER_0                0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_SRC_BUFFER_1                0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_SRC_BUFFER_2                0x00000002 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_SRC_BUFFER_3                0x00000003 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_DST_BUFFER0                      20:20 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_DST_BUFFER0_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_DST_BUFFER0_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_DST_BUFFER1                      21:21 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_DST_BUFFER1_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_DST_BUFFER1_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_DST_BUFFER2                      22:22 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_DST_BUFFER2_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_DST_BUFFER2_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_DST_BUFFER3                      23:23 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_DST_BUFFER3_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_DST_BUFFER3_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG                     28:24 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_RSVD0          0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_DST_SRC    0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_SRC_DST    0x00000002 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_SRC_SRC    0x00000003 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_DST_DST    0x00000004 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_DST_SRC    0x00000005 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_SRC_DST    0x00000006 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_SRC_SRC0   0x00000007 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_SRC_SRC1   0x00000008 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_SRC_PAT    0x00000009 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_PAT_SRC    0x0000000a /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_PAT_PAT    0x0000000b /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_SRC_SRC    0x0000000c /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_SRC_PAT    0x0000000d /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_PAT_SRC    0x0000000e /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_RSVD1          0x0000000f /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_SRC_DST    0x00000010 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_PAT_DST_SRC    0x00000011 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_PAT_DST    0x00000012 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_DST_PAT    0x00000013 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_PAT_SRC    0x00000014 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_DST_SRC_PAT    0x00000015 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_RSVD2          0x00000016 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_SRC_BYPASS     0x00000017 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD0    0x00000018 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_SRC_DST  0x00000019 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_DST_SRC  0x0000001a /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD1    0x0000001b /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD2    0x0000001c /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_SRC      0x0000001d /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD3    0x0000001e /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG_BLEND_RSVD4    0x0000001f /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_SPARE3                           29:29 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_SPARE3_DISABLED             0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_SPARE3_ENABLED              0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_VOLATILE                         31:31 /* CWIVF */
#define NV_PGRAPH_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* CWI-V */
#define NV_PGRAPH_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
#define NV_PGRAPH_CTX_CACHE(i)                   (0x004001a0+(i)*4) /* RW-4A */
#define NV_PGRAPH_CTX_CACHE__SIZE_1                               8 /*       */
#define NV_PGRAPH_CTX_CACHE_COLOR                              2:0  /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_ALPHA                              3:3  /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_SPARE1                             4:4  /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_MONO_FORMAT                        8:8  /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_DAC_BYPASS                         9:9  /* RWXVF */
#define NV_PGRAPH_CTX_CACHE_SPARE2                            10:10 /* RWXVF */
#define NV_PGRAPH_CTX_CACHE_ZWRITE                            12:12 /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_CHROMA_KEY                        13:13 /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_PLANE_MASK                        14:14 /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_USER_CLIP                         15:15 /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_SRC_BUFFER                        17:16 /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_DST_BUFFER0                       20:20 /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_DST_BUFFER1                       21:21 /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_DST_BUFFER2                       22:22 /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_DST_BUFFER3                       23:23 /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_PATCH_CONFIG                      28:24 /* RWIVF */
#define NV_PGRAPH_CTX_CACHE_SPARE3                            29:29 /* RWXVF */
#define NV_PGRAPH_CTX_CONTROL                            0x00400190 /* RW-4R */
#define NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME                      1:0 /* RWIVF */
#define NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME_33US          0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME_262US         0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME_2MS           0x00000002 /* RW--V */
#define NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME_17MS          0x00000003 /* RW--V */
#define NV_PGRAPH_CTX_CONTROL_TIME                              8:8 /* RWIVF */
#define NV_PGRAPH_CTX_CONTROL_TIME_EXPIRED               0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_CONTROL_TIME_NOT_EXPIRED           0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_CONTROL_CHID                            16:16 /* RWIVF */
#define NV_PGRAPH_CTX_CONTROL_CHID_INVALID               0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_CONTROL_CHID_VALID                 0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_CONTROL_SWITCH                          20:20 /* R--VF */
#define NV_PGRAPH_CTX_CONTROL_SWITCH_UNAVAILABLE         0x00000000 /* R---V */
#define NV_PGRAPH_CTX_CONTROL_SWITCH_AVAILABLE           0x00000001 /* R---V */
#define NV_PGRAPH_CTX_CONTROL_SWITCHING                       24:24 /* RWIVF */
#define NV_PGRAPH_CTX_CONTROL_SWITCHING_IDLE             0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_CONTROL_SWITCHING_BUSY             0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_CONTROL_DEVICE                          28:28 /* RWIVF */
#define NV_PGRAPH_CTX_CONTROL_DEVICE_DISABLED            0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_CONTROL_DEVICE_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_USER                               0x00400194 /* RW-4R */
#define NV_PGRAPH_CTX_USER_SUBCH                              15:13 /* RWXVF */
#define NV_PGRAPH_CTX_USER_CLASS                              20:16 /* RWXVF */
#define NV_PGRAPH_CTX_USER_CHID                               30:24 /* RWXVF */
#define NV_PGRAPH_FIFO                                   0x004006A4 /* RW-4R */
#define NV_PGRAPH_FIFO_ACCESS                                   0:0 /* RWIVF */
#define NV_PGRAPH_FIFO_ACCESS_DISABLED                   0x00000000 /* RW--V */
#define NV_PGRAPH_FIFO_ACCESS_ENABLED                    0x00000001 /* RWI-V */
#define NV_PGRAPH_STATUS                                 0x004006B0 /* R--4R */
#define NV_PGRAPH_STATUS_STATE                                  0:0 /* R-IVF */
#define NV_PGRAPH_STATUS_STATE_IDLE                      0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_STATE_BUSY                      0x00000001 /* R---V */
#define NV_PGRAPH_STATUS_XY_LOGIC                               4:4 /* R-IVF */
#define NV_PGRAPH_STATUS_XY_LOGIC_IDLE                   0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_XY_LOGIC_BUSY                   0x00000001 /* R---V */
#define NV_PGRAPH_STATUS_PORT_NOTIFY                            8:8 /* R-IVF */
#define NV_PGRAPH_STATUS_PORT_NOTIFY_IDLE                0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_PORT_NOTIFY_BUSY                0x00000001 /* R---V */
#define NV_PGRAPH_STATUS_PORT_REGISTER                        12:12 /* R-IVF */
#define NV_PGRAPH_STATUS_PORT_REGISTER_IDLE              0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_PORT_REGISTER_BUSY              0x00000001 /* R---V */
#define NV_PGRAPH_STATUS_PORT_DMA                             16:16 /* R-IVF */
#define NV_PGRAPH_STATUS_PORT_DMA_IDLE                   0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_PORT_DMA_BUSY                   0x00000001 /* R---V */
#define NV_PGRAPH_STATUS_DMA_ENGINE                           17:17 /* R-IVF */
#define NV_PGRAPH_STATUS_DMA_ENGINE_IDLE                 0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_DMA_ENGINE_BUSY                 0x00000001 /* R---V */
#define NV_PGRAPH_STATUS_DMA_NOTIFY                           20:20 /* R-IVF */
#define NV_PGRAPH_STATUS_DMA_NOTIFY_IDLE                 0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_DMA_NOTIFY_BUSY                 0x00000001 /* R---V */
#define NV_PGRAPH_STATUS_3D                                   24:24 /* R-IVF */
#define NV_PGRAPH_STATUS_3D_IDLE                         0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_3D_BUSY                         0x00000001 /* R---V */
#define NV_PGRAPH_STATUS_CACHE                                25:25 /* R-IVF */
#define NV_PGRAPH_STATUS_CACHE_IDLE                      0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_CACHE_BUSY                      0x00000001 /* R---V */
#define NV_PGRAPH_STATUS_LIGHTING                             26:26 /* R-IVF */
#define NV_PGRAPH_STATUS_LIGHTING_IDLE                   0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_LIGHTING_BUSY                   0x00000001 /* R---V */
#define NV_PGRAPH_STATUS_ZFIFO                                27:27 /* R-IVF */
#define NV_PGRAPH_STATUS_ZFIFO_IDLE                      0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_ZFIFO_BUSY                      0x00000001 /* R---V */
#define NV_PGRAPH_STATUS_PORT_USER                            28:28 /* R-IVF */
#define NV_PGRAPH_STATUS_PORT_USER_IDLE                  0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_PORT_USER_BUSY                  0x00000001 /* R---V */
#define NV_PGRAPH_TRAPPED_ADDR                           0x004006B4 /* R--4R */
#define NV_PGRAPH_TRAPPED_ADDR_MTHD                            12:2 /* R-XUF */
#define NV_PGRAPH_TRAPPED_ADDR_SUBCH                          15:13 /* R-XUF */
#define NV_PGRAPH_TRAPPED_ADDR_CLASS                          20:16 /* R-XUF */
#define NV_PGRAPH_TRAPPED_ADDR_CHID                           30:24 /* R-XUF */
#define NV_PGRAPH_TRAPPED_DATA                           0x004006B8 /* R--4R */
#define NV_PGRAPH_TRAPPED_DATA_VALUE                           31:0 /* R-XVF */
#define NV_PGRAPH_TRAPPED_INST                           0x004006BC /* R--4R */
#define NV_PGRAPH_TRAPPED_INST_VALUE                           15:0 /* R-XVF */
#define NV_PGRAPH_CLIP_MISC                              0x004006A0 /* RW-4R */
#define NV_PGRAPH_CLIP_MISC_REGIONS                             1:0 /* RWIUF */
#define NV_PGRAPH_CLIP_MISC_REGIONS_DISABLED             0x00000000 /* RWI-V */
#define NV_PGRAPH_CLIP_MISC_REGIONS_1                    0x00000001 /* RW--V */
#define NV_PGRAPH_CLIP_MISC_REGIONS_2                    0x00000002 /* RW--V */
#define NV_PGRAPH_CLIP_MISC_RENDER                              4:4 /* RWIVF */
#define NV_PGRAPH_CLIP_MISC_RENDER_INCLUDED              0x00000000 /* RWI-V */
#define NV_PGRAPH_CLIP_MISC_RENDER_OCCLUDED              0x00000001 /* RW--V */
#define NV_PGRAPH_CLIP_MISC_COMPLEX                             8:8 /* RWIVF */
#define NV_PGRAPH_CLIP_MISC_COMPLEX_DISABLED             0x00000000 /* RWI-V */
#define NV_PGRAPH_CLIP_MISC_COMPLEX_ENABLED              0x00000001 /* RW--V */
#define NV_PGRAPH_SRC_CANVAS_MIN                         0x00400550 /* RW-4R */
#define NV_PGRAPH_SRC_CANVAS_MIN_X                             10:0 /* RWXUF */
#define NV_PGRAPH_SRC_CANVAS_MIN_Y                            29:16 /* RWXUF */
#define NV_PGRAPH_DST_CANVAS_MIN                         0x00400558 /* RW-4R */
#define NV_PGRAPH_DST_CANVAS_MIN_X                             10:0 /* RWXUF */
#define NV_PGRAPH_DST_CANVAS_MIN_Y                            29:16 /* RWXUF */
#define NV_PGRAPH_SRC_CANVAS_MAX                         0x00400554 /* RW-4R */
#define NV_PGRAPH_SRC_CANVAS_MAX_X                             10:0 /* RWXUF */
#define NV_PGRAPH_SRC_CANVAS_MAX_Y                            29:16 /* RWXUF */
#define NV_PGRAPH_DST_CANVAS_MAX                         0x0040055C /* RW-4R */
#define NV_PGRAPH_DST_CANVAS_MAX_X                             10:0 /* RWXUF */
#define NV_PGRAPH_DST_CANVAS_MAX_Y                            29:16 /* RWXUF */
#define NV_PGRAPH_CLIP0_MIN                              0x00400690 /* RW-4R */
#define NV_PGRAPH_CLIP0_MIN_X                                  10:0 /* RWXSF */
#define NV_PGRAPH_CLIP0_MIN_Y                                 29:16 /* RWXSF */
#define NV_PGRAPH_CLIP1_MIN                              0x00400698 /* RW-4R */
#define NV_PGRAPH_CLIP1_MIN_X                                  10:0 /* RWXSF */
#define NV_PGRAPH_CLIP1_MIN_Y                                 29:16 /* RWXSF */
#define NV_PGRAPH_CLIP0_MAX                              0x00400694 /* RW-4R */
#define NV_PGRAPH_CLIP0_MAX_X                                  10:0 /* RWXSF */
#define NV_PGRAPH_CLIP0_MAX_Y                                 29:16 /* RWXSF */
#define NV_PGRAPH_CLIP1_MAX                              0x0040069C /* RW-4R */
#define NV_PGRAPH_CLIP1_MAX_X                                  10:0 /* RWXSF */
#define NV_PGRAPH_CLIP1_MAX_Y                                 29:16 /* RWXSF */
#define NV_PGRAPH_DMA                                    0x00400680 /* RW-4R */
#define NV_PGRAPH_DMA_INSTANCE                                 15:0 /* RWXUF */
#define NV_PGRAPH_NOTIFY                                 0x00400684 /* RW-4R */
#define NV_PGRAPH_NOTIFY_INSTANCE                              15:0 /* RWXUF */
#define NV_PGRAPH_NOTIFY_REQ                                  16:16 /* RWIVF */
#define NV_PGRAPH_NOTIFY_REQ_NOT_PENDING                 0x00000000 /* RWI-V */
#define NV_PGRAPH_NOTIFY_REQ_PENDING                     0x00000001 /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE                                 23:20 /* RWIVF */
#define NV_PGRAPH_NOTIFY_TYPE_HW                         0x00000000 /* RWI-V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_1                       0x00000001 /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_2                       0x00000002 /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_3                       0x00000003 /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_4                       0x00000004 /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_5                       0x00000005 /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_6                       0x00000006 /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_7                       0x00000007 /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_8                       0x00000008 /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_9                       0x00000009 /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_10                      0x0000000A /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_11                      0x0000000B /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_12                      0x0000000C /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_13                      0x0000000D /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_14                      0x0000000E /* RW--V */
#define NV_PGRAPH_NOTIFY_TYPE_SW_15                      0x0000000F /* RW--V */
#define NV_PGRAPH_INSTANCE                               0x00400688 /* RW-4R */
#define NV_PGRAPH_INSTANCE_TAG                                 15:0 /* RWXUF */
#define NV_PGRAPH_INSTANCE_TAG_INVALID                   0x00000000 /* RWI-V */
#define NV_PGRAPH_MEMFMT                                 0x0040068C /* RW-4R */
#define NV_PGRAPH_MEMFMT_INSTANCE                              15:0 /* RWXUF */
#define NV_PGRAPH_MEMFMT_LINEAR                               16:16 /* RWIVF */
#define NV_PGRAPH_MEMFMT_LINEAR_OUT                      0x00000000 /* RW--V */
#define NV_PGRAPH_MEMFMT_LINEAR_IN                       0x00000001 /* RW--V */
#define NV_PGRAPH_BOFFSET0                               0x00400630 /* RW-4R */
#define NV_PGRAPH_BOFFSET0_LINADRS                             21:0 /* RWIUF */
#define NV_PGRAPH_BOFFSET0_LINADRS_0                     0x00000000 /* RWI-V */
#define NV_PGRAPH_BOFFSET1                               0x00400634 /* RW-4R */
#define NV_PGRAPH_BOFFSET1_LINADRS                             21:0 /* RWIUF */
#define NV_PGRAPH_BOFFSET1_LINADRS_0                     0x00000000 /* RWI-V */
#define NV_PGRAPH_BOFFSET2                               0x00400638 /* RW-4R */
#define NV_PGRAPH_BOFFSET2_LINADRS                             21:0 /* RWIUF */
#define NV_PGRAPH_BOFFSET2_LINADRS_0                     0x00000000 /* RWI-V */
#define NV_PGRAPH_BOFFSET3                               0x0040063C /* RW-4R */
#define NV_PGRAPH_BOFFSET3_LINADRS                             21:0 /* RWIUF */
#define NV_PGRAPH_BOFFSET3_LINADRS_0                     0x00000000 /* RWI-V */
#define NV_PGRAPH_BPITCH0                                0x00400650 /* RW-4R */
#define NV_PGRAPH_BPITCH0_VALUE                                12:0 /* RWXUF */
#define NV_PGRAPH_BPITCH1                                0x00400654 /* RW-4R */
#define NV_PGRAPH_BPITCH1_VALUE                                12:0 /* RWXUF */
#define NV_PGRAPH_BPITCH2                                0x00400658 /* RW-4R */
#define NV_PGRAPH_BPITCH2_VALUE                                12:0 /* RWXUF */
#define NV_PGRAPH_BPITCH3                                0x0040065C /* RW-4R */
#define NV_PGRAPH_BPITCH3_VALUE                                12:0 /* RWXUF */
#define NV_PGRAPH_BPIXEL                                 0x004006a8 /* RW-4R */
#define NV_PGRAPH_BPIXEL_DEPTH0_FMT                             1:0 /* RWXVF */
#define NV_PGRAPH_BPIXEL_DEPTH0_FMT_Y16_BITS             0x00000000 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH0_FMT_8_BITS               0x00000001 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH0_FMT_16_BITS              0x00000002 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH0_FMT_32_BITS              0x00000003 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH0                                 2:2 /* RWXVF */
#define NV_PGRAPH_BPIXEL_DEPTH0_NOT_VALID                0x00000000 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH0_VALID                    0x00000001 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH1_FMT                             5:4 /* RWXVF */
#define NV_PGRAPH_BPIXEL_DEPTH1_FMT_Y16_BITS             0x00000000 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH1_FMT_8_BITS               0x00000001 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH1_FMT_16_BITS              0x00000002 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH1_FMT_32_BITS              0x00000003 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH1                                 6:6 /* RWXVF */
#define NV_PGRAPH_BPIXEL_DEPTH1_NOT_VALID                0x00000000 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH1_VALID                    0x00000001 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH2_FMT                             9:8 /* RWXVF */
#define NV_PGRAPH_BPIXEL_DEPTH2_FMT_Y16_BITS             0x00000000 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH2_FMT_8_BITS               0x00000001 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH2_FMT_16_BITS              0x00000002 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH2_FMT_32_BITS              0x00000003 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH2                               10:10 /* RWXVF */
#define NV_PGRAPH_BPIXEL_DEPTH2_NOT_VALID                0x00000000 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH2_VALID                    0x00000001 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH3_FMT                           13:12 /* RWXVF */
#define NV_PGRAPH_BPIXEL_DEPTH3_FMT_Y16_BITS             0x00000000 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH3_FMT_8_BITS               0x00000001 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH3_FMT_16_BITS              0x00000002 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH3_FMT_32_BITS              0x00000003 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH3                               14:14 /* RWXVF */
#define NV_PGRAPH_BPIXEL_DEPTH3_NOT_VALID                0x00000000 /* RW--V */
#define NV_PGRAPH_BPIXEL_DEPTH3_VALID                    0x00000001 /* RW--V */
#define NV_PGRAPH_CACHE_INDEX                            0x004006c0 /* RW-4R */
#define NV_PGRAPH_CACHE_INDEX_BANK                              2:2 /* RWXVF */
#define NV_PGRAPH_CACHE_INDEX_BANK_10                    0x00000000 /* RW--V */
#define NV_PGRAPH_CACHE_INDEX_BANK_32                    0x00000001 /* RW--V */
#define NV_PGRAPH_CACHE_INDEX_ADRS                             12:3 /* RWXVF */
#define NV_PGRAPH_CACHE_INDEX_ADRS_0                     0x00000000 /* RW--V */
#define NV_PGRAPH_CACHE_INDEX_ADRS_1024                  0x00000400 /* RW--V */
#define NV_PGRAPH_CACHE_INDEX_OP                              14:13 /* RWXVF */
#define NV_PGRAPH_CACHE_INDEX_OP_WR_CACHE                0x00000000 /* RW--V */
#define NV_PGRAPH_CACHE_INDEX_OP_RD_CACHE                0x00000001 /* RW--V */
#define NV_PGRAPH_CACHE_INDEX_OP_RD_INDEX                0x00000002 /* RW--V */
#define NV_PGRAPH_CACHE_RAM                              0x004006c4 /* RW-4R */
#define NV_PGRAPH_CACHE_RAM_VALUE                              31:0 /* RWXVF */
#define NV_PGRAPH_PATT_COLOR0_0                          0x00400600 /* RW-4R */
#define NV_PGRAPH_PATT_COLOR0_0_BLUE                            9:0 /* RWXUF */
#define NV_PGRAPH_PATT_COLOR0_0_GREEN                         19:10 /* RWXUF */
#define NV_PGRAPH_PATT_COLOR0_0_RED                           29:20 /* RWXUF */
#define NV_PGRAPH_PATT_COLOR0_1                          0x00400604 /* RW-4R */
#define NV_PGRAPH_PATT_COLOR0_1_ALPHA                           7:0 /* RWXUF */
#define NV_PGRAPH_PATT_COLOR1_0                          0x00400608 /* RW-4R */
#define NV_PGRAPH_PATT_COLOR1_0_BLUE                            9:0 /* RWXUF */
#define NV_PGRAPH_PATT_COLOR1_0_GREEN                         19:10 /* RWXUF */
#define NV_PGRAPH_PATT_COLOR1_0_RED                           29:20 /* RWXUF */
#define NV_PGRAPH_PATT_COLOR1_1                          0x0040060C /* RW-4R */
#define NV_PGRAPH_PATT_COLOR1_1_ALPHA                           7:0 /* RWXUF */
#define NV_PGRAPH_PATTERN(i)                     (0x00400610+(i)*4) /* RW-4A */
#define NV_PGRAPH_PATTERN__SIZE_1                                 2 /*       */
#define NV_PGRAPH_PATTERN_BITMAP                               31:0 /* RWXVF */
#define NV_PGRAPH_PATTERN_SHAPE                          0x00400618 /* RW-4R */
#define NV_PGRAPH_PATTERN_SHAPE_VALUE                           1:0 /* RWXVF */
#define NV_PGRAPH_PATTERN_SHAPE_VALUE_8X8                0x00000000 /* RW--V */
#define NV_PGRAPH_PATTERN_SHAPE_VALUE_64X1               0x00000001 /* RW--V */
#define NV_PGRAPH_PATTERN_SHAPE_VALUE_1X64               0x00000002 /* RW--V */
#define NV_PGRAPH_MONO_COLOR0                            0x0040061C /* RW-4R */
#define NV_PGRAPH_MONO_COLOR0_BLUE                              9:0 /* RWXUF */
#define NV_PGRAPH_MONO_COLOR0_GREEN                           19:10 /* RWXUF */
#define NV_PGRAPH_MONO_COLOR0_RED                             29:20 /* RWXUF */
#define NV_PGRAPH_MONO_COLOR0_ALPHA                           30:30 /* RWXUF */
#define NV_PGRAPH_ROP3                                   0x00400624 /* RW-4R */
#define NV_PGRAPH_ROP3_VALUE                                    7:0 /* RWXVF */
#define NV_PGRAPH_PLANE_MASK                             0x00400628 /* RW-4R */
#define NV_PGRAPH_PLANE_MASK_BLUE                               9:0 /* RWXUF */
#define NV_PGRAPH_PLANE_MASK_GREEN                            19:10 /* RWXUF */
#define NV_PGRAPH_PLANE_MASK_RED                              29:20 /* RWXUF */
#define NV_PGRAPH_PLANE_MASK_ALPHA                            30:30 /* RWXUF */
#define NV_PGRAPH_CHROMA                                 0x0040062C /* RW-4R */
#define NV_PGRAPH_CHROMA_BLUE                                   9:0 /* RWXUF */
#define NV_PGRAPH_CHROMA_GREEN                                19:10 /* RWXUF */
#define NV_PGRAPH_CHROMA_RED                                  29:20 /* RWXUF */
#define NV_PGRAPH_CHROMA_ALPHA                                30:30 /* RWXUF */
#define NV_PGRAPH_BETA                                   0x00400640 /* RW-4R */
#define NV_PGRAPH_BETA_VALUE_FRACTION                         30:23 /* RWXUF */
#define NV_PGRAPH_CONTROL_OUT                            0x00400644 /* RW-4R */
#define NV_PGRAPH_CONTROL_OUT_INTERPOLATOR                      1:0 /* -WIUF */
#define NV_PGRAPH_CONTROL_OUT_INTERPOLATOR_ZOH           0x00000000 /* -WIUV */
#define NV_PGRAPH_CONTROL_OUT_INTERPOLATOR_ZOH_MS        0x00000001 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_INTERPOLATOR_FOH           0x00000002 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_WRAP_U                            5:4 /* -WIUF */
#define NV_PGRAPH_CONTROL_OUT_WRAP_U_CYLINDRICAL         0x00000000 /* -WIUV */
#define NV_PGRAPH_CONTROL_OUT_WRAP_U_WRAP                0x00000001 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_WRAP_U_MIRROR              0x00000002 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_WRAP_U_CLAMP               0x00000003 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_WRAP_V                            7:6 /* -WIUF */
#define NV_PGRAPH_CONTROL_OUT_WRAP_V_CYLINDRICAL         0x00000000 /* -WIUV */
#define NV_PGRAPH_CONTROL_OUT_WRAP_V_WRAP                0x00000001 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_WRAP_V_MIRROR              0x00000002 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_WRAP_V_CLAMP               0x00000003 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_COLOR_FORMAT                      8:8 /* -WIUF */
#define NV_PGRAPH_CONTROL_OUT_COLOR_FORMAT_LE_X8R8G8B8   0x00000000 /* -WIUV */
#define NV_PGRAPH_CONTROL_OUT_COLOR_FORMAT_LE_A8R8G8B8   0x00000001 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_CULLING                         13:12 /* -WIUF */
#define NV_PGRAPH_CONTROL_OUT_CULLING_ILLEGAL            0x00000000 /* -WIUV */
#define NV_PGRAPH_CONTROL_OUT_CULLING_NONE               0x00000001 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_CULLING_CLOCKWISE          0x00000002 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_CULLING_COUNTERCLOCKWISE   0x00000003 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZBUFFER                         15:15 /* -WIUF */
#define NV_PGRAPH_CONTROL_OUT_ZBUFFER_SCREEN             0x00000000 /* -WIUV */
#define NV_PGRAPH_CONTROL_OUT_ZBUFFER_LINEAR             0x00000001 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE                    19:16 /* -WIUF */
#define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_ILLEGAL       0x00000000 /* -WIUV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_FALSE         0x00000001 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_LT            0x00000002 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_EQ            0x00000003 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_LE            0x00000004 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_GT            0x00000005 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_NE            0x00000006 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_GE            0x00000007 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_COMPARE_TRUE          0x00000008 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_WRITE                      22:20 /* -WIUF */
#define NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_NEVER           0x00000000 /* -WIUV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_ALPHA           0x00000001 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_ALPHA_ZETA      0x00000002 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_ZETA            0x00000003 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ZETA_WRITE_ALWAYS          0x00000004 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_COLOR_WRITE                     25:24 /* -WIUF */
#define NV_PGRAPH_CONTROL_OUT_COLOR_WRITE_NEVER          0x00000000 /* -WIUV */
#define NV_PGRAPH_CONTROL_OUT_COLOR_WRITE_ALPHA          0x00000001 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_COLOR_WRITE_ALPHA_ZETA     0x00000002 /* -W-UV */
#define NV_PGRAPH_CONTROL_OUT_ROP                             28:28 /* -WIUF */
#define NV_PGRAPH_CONTROL_OUT_ROP_BLEND_AND              0x00000000 /* -WIUV */
#define NV_PGRAPH_CONTROL_OUT_ROP_ADD_WITH_SATURATION    0x00000001 /* -W-UV */
#define NV_PGRAPH_DPRAM_INDEX                            0x00400648 /* RW-4R */
#define NV_PGRAPH_DPRAM_INDEX_ADRS                              5:0 /* RWIVF */
#define NV_PGRAPH_DPRAM_INDEX_ADRS_0                     0x00000000 /* RWI-V */
#define NV_PGRAPH_DPRAM_INDEX_SELECT                           10:8 /* RWIVF */
#define NV_PGRAPH_DPRAM_INDEX_SELECT_ADRS_0              0x00000000 /* RWI-V */
#define NV_PGRAPH_DPRAM_INDEX_SELECT_ADRS_1              0x00000001 /* RW--V */
#define NV_PGRAPH_DPRAM_INDEX_SELECT_DATA_0              0x00000002 /* RW--V */
#define NV_PGRAPH_DPRAM_INDEX_SELECT_DATA_1              0x00000003 /* RW--V */
#define NV_PGRAPH_DPRAM_INDEX_SELECT_WE                  0x00000004 /* RW--V */
#define NV_PGRAPH_DPRAM_INDEX_SELECT_ALPHA               0x00000005 /* RW--V */
#define NV_PGRAPH_DPRAM_DATA                             0x0040064c /* RW-4R */
#define NV_PGRAPH_DPRAM_DATA_VALUE                             31:0 /* RWXVF */
#define NV_PGRAPH_DPRAM_ADRS_0                           0x0040064c /* RW-4R */
#define NV_PGRAPH_DPRAM_ADRS_0__ALIAS_1        NV_PGRAPH_DPRAM_DATA /*       */
#define NV_PGRAPH_DPRAM_ADRS_0_VALUE                           19:0 /* RWXVF */
#define NV_PGRAPH_DPRAM_ADRS_1                           0x0040064c /* RW-4R */
#define NV_PGRAPH_DPRAM_ADRS_1__ALIAS_1        NV_PGRAPH_DPRAM_DATA /*       */
#define NV_PGRAPH_DPRAM_ADRS_1_VALUE                           19:0 /* RWXVF */
#define NV_PGRAPH_DPRAM_DATA_0                           0x0040064c /* RW-4R */
#define NV_PGRAPH_DPRAM_DATA_0__ALIAS_1        NV_PGRAPH_DPRAM_DATA /*       */
#define NV_PGRAPH_DPRAM_DATA_0_VALUE                           31:0 /* RWXVF */
#define NV_PGRAPH_DPRAM_DATA_1                           0x0040064c /* RW-4R */
#define NV_PGRAPH_DPRAM_DATA_1__ALIAS_1        NV_PGRAPH_DPRAM_DATA /*       */
#define NV_PGRAPH_DPRAM_DATA_1_VALUE                           31:0 /* RWXVF */
#define NV_PGRAPH_DPRAM_WE                               0x0040064c /* RW-4R */
#define NV_PGRAPH_DPRAM_WE__ALIAS_1            NV_PGRAPH_DPRAM_DATA /*       */
#define NV_PGRAPH_DPRAM_WE_VALUE                               31:0 /* RWXVF */
#define NV_PGRAPH_DPRAM_ALPHA                            0x0040064c /* RW-4R */
#define NV_PGRAPH_DPRAM_ALPHA__ALIAS_1         NV_PGRAPH_DPRAM_DATA /*       */
#define NV_PGRAPH_DPRAM_ALPHA_VALUE                            31:0 /* RWXVF */
#define NV_PGRAPH_ABS_X_RAM(i)                   (0x00400400+(i)*4) /* RW-4A */
#define NV_PGRAPH_ABS_X_RAM__SIZE_1                              32 /*       */
#define NV_PGRAPH_ABS_X_RAM_VALUE                              31:0 /* RWXUF */
#define NV_PGRAPH_X_RAM_BPORT(i)                 (0x00400c00+(i)*4) /* R--4A */
#define NV_PGRAPH_X_RAM_BPORT__SIZE_1                            32 /*       */
#define NV_PGRAPH_X_RAM_BPORT_VALUE                            31:0 /* R--UF */
#define NV_PGRAPH_ABS_Y_RAM(i)                   (0x00400480+(i)*4) /* RW-4A */
#define NV_PGRAPH_ABS_Y_RAM__SIZE_1                              32 /*       */
#define NV_PGRAPH_ABS_Y_RAM_VALUE                              31:0 /* RWXUF */
#define NV_PGRAPH_Y_RAM_BPORT(i)                 (0x00400c80+(i)*4) /* R--4A */
#define NV_PGRAPH_Y_RAM_BPORT__SIZE_1                            32 /*       */
#define NV_PGRAPH_Y_RAM_BPORT_VALUE                            31:0 /* R--UF */
#define NV_PGRAPH_XY_LOGIC_MISC0                         0x00400514 /* RW-4R */
#define NV_PGRAPH_XY_LOGIC_MISC0_COUNTER                       17:0 /* RWIUF */
#define NV_PGRAPH_XY_LOGIC_MISC0_COUNTER_0               0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC0_DIMENSION                    20:20 /* RWXVF */
#define NV_PGRAPH_XY_LOGIC_MISC0_DIMENSION_NONZERO       0x00000000 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC0_DIMENSION_ZERO          0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC0_INDEX                        31:28 /* RWIUF */
#define NV_PGRAPH_XY_LOGIC_MISC0_INDEX_0                 0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1                         0x00400518 /* RW-4R */
#define NV_PGRAPH_XY_LOGIC_MISC1_INITIAL                        0:0 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_INITIAL_NEEDED          0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_INITIAL_DONE            0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPX                      4:4 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPX_NOTNULL       0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPX_NULL          0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPY                      5:5 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPY_NOTNULL       0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_XTRACLIPY_NULL          0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPX                      8:8 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPX_NOTNULL       0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPX_NULL          0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPY                      9:9 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPY_NOTNULL       0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_USERCLIPY_NULL          0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMIN                    12:12 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMIN_CANVASMIN     0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMIN_USERMIN       0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMAX                    13:13 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMAX_CANVASMAX     0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XCMAX_USERMAX       0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XIMAX                    14:14 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XIMAX_UCMAX         0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XIMAX_IMAGEMAX      0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMIN                    16:16 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMIN_CANVASMIN     0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMIN_USERMIN       0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMAX                    17:17 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMAX_CANVASMAX     0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YCMAX_USERMAX       0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YIMAX                    18:18 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YIMAX_UCMAX         0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_YIMAX_IMAGEMAX      0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XXTRA                    20:20 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XXTRA_CLIPMAX       0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XXTRA_IMAGEMAX      0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_DUDX                         27:24 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_DUDX_VALUE              0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2                         0x0040051C /* RW-4R */
#define NV_PGRAPH_XY_LOGIC_MISC2_HANDOFF                        0:0 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC2_HANDOFF_DISABLE         0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2_HANDOFF_ENABLE          0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPX                      4:4 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPX_NOTNULL       0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPX_NULL          0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPY                      5:5 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPY_NOTNULL       0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2_XTRACLIPY_NULL          0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPX                      8:8 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPX_NOTNULL       0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPX_NULL          0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPY                      9:9 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPY_NOTNULL       0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2_USERCLIPY_NULL          0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMIN                    12:12 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMIN_CANVASMIN     0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMIN_USERMIN       0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMAX                    13:13 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMAX_CANVASMAX     0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XCMAX_USERMAX       0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XIMAX                    14:14 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XIMAX_UCMAX         0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XIMAX_IMAGEMAX      0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMIN                    16:16 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMIN_CANVASMIN     0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMIN_USERMIN       0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMAX                    17:17 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMAX_CANVASMAX     0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YCMAX_USERMAX       0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YIMAX                    18:18 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YIMAX_UCMAX         0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_YIMAX_IMAGEMAX      0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XXTRA                    20:20 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XXTRA_CLIPMAX       0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC2_SEL_XXTRA_IMAGEMAX      0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_DVDY                         27:24 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_DVDY_VALUE              0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC3                         0x00400520 /* RW-4R */
#define NV_PGRAPH_XY_LOGIC_MISC3_WDIMY_EQ_0                     0:0 /* RWXVF */
#define NV_PGRAPH_XY_LOGIC_MISC3_WDIMY_EQ_0_NULL         0x00000000 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC3_WDIMY_EQ_0_TRUE         0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WDIMY                   4:4 /* RWXVF */
#define NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WDIMY_NULL       0x00000000 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WDIMY_TRUE       0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WX                      8:8 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WX_NULL          0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC3_RELOAD_WX_TRUE          0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_ALG                     12:12 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_ALG_NULL           0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_ALG_TRUE           0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_DIMX                    22:16 /* RWXUF */
#define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_DIMX_0             0x00000000 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_WDIMX                   30:24 /* RWXUF */
#define NV_PGRAPH_XY_LOGIC_MISC3_TEXT_WDIMX_0            0x00000000 /* RW--V */
#define NV_PGRAPH_X_MISC                                 0x00400500 /* RW-4R */
#define NV_PGRAPH_X_MISC_BIT33_0                                0:0 /* RWIVF */
#define NV_PGRAPH_X_MISC_BIT33_0_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_X_MISC_BIT33_1                                1:1 /* RWIVF */
#define NV_PGRAPH_X_MISC_BIT33_1_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_X_MISC_BIT33_2                                2:2 /* RWIVF */
#define NV_PGRAPH_X_MISC_BIT33_2_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_X_MISC_BIT33_3                                3:3 /* RWIVF */
#define NV_PGRAPH_X_MISC_BIT33_3_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_X_MISC_RANGE_0                                4:4 /* RWIVF */
#define NV_PGRAPH_X_MISC_RANGE_0_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_X_MISC_RANGE_1                                5:5 /* RWIVF */
#define NV_PGRAPH_X_MISC_RANGE_1_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_X_MISC_RANGE_2                                6:6 /* RWIVF */
#define NV_PGRAPH_X_MISC_RANGE_2_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_X_MISC_RANGE_3                                7:7 /* RWIVF */
#define NV_PGRAPH_X_MISC_RANGE_3_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_X_MISC_ADDER_OUTPUT                         29:28 /* RWXVF */
#define NV_PGRAPH_X_MISC_ADDER_OUTPUT_EQ_0               0x00000000 /* RW--V */
#define NV_PGRAPH_X_MISC_ADDER_OUTPUT_LT_0               0x00000001 /* RW--V */
#define NV_PGRAPH_X_MISC_ADDER_OUTPUT_GT_0               0x00000002 /* RW--V */
#define NV_PGRAPH_Y_MISC                                 0x00400504 /* RW-4R */
#define NV_PGRAPH_Y_MISC_BIT33_0                                0:0 /* RWIVF */
#define NV_PGRAPH_Y_MISC_BIT33_0_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_Y_MISC_BIT33_1                                1:1 /* RWIVF */
#define NV_PGRAPH_Y_MISC_BIT33_1_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_Y_MISC_BIT33_2                                2:2 /* RWIVF */
#define NV_PGRAPH_Y_MISC_BIT33_2_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_Y_MISC_BIT33_3                                3:3 /* RWIVF */
#define NV_PGRAPH_Y_MISC_BIT33_3_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_Y_MISC_RANGE_0                                4:4 /* RWIVF */
#define NV_PGRAPH_Y_MISC_RANGE_0_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_Y_MISC_RANGE_1                                5:5 /* RWIVF */
#define NV_PGRAPH_Y_MISC_RANGE_1_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_Y_MISC_RANGE_2                                6:6 /* RWIVF */
#define NV_PGRAPH_Y_MISC_RANGE_2_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_Y_MISC_RANGE_3                                7:7 /* RWIVF */
#define NV_PGRAPH_Y_MISC_RANGE_3_0                       0x00000000 /* RWI-V */
#define NV_PGRAPH_Y_MISC_ADDER_OUTPUT                         29:28 /* RWXVF */
#define NV_PGRAPH_Y_MISC_ADDER_OUTPUT_EQ_0               0x00000000 /* RW--V */
#define NV_PGRAPH_Y_MISC_ADDER_OUTPUT_LT_0               0x00000001 /* RW--V */
#define NV_PGRAPH_Y_MISC_ADDER_OUTPUT_GT_0               0x00000002 /* RW--V */
#define NV_PGRAPH_ABS_UCLIP_XMIN                         0x0040053C /* RW-4R */
#define NV_PGRAPH_ABS_UCLIP_XMIN_VALUE                         17:0 /* RWXSF */
#define NV_PGRAPH_ABS_UCLIP_XMAX                         0x00400544 /* RW-4R */
#define NV_PGRAPH_ABS_UCLIP_XMAX_VALUE                         17:0 /* RWXSF */
#define NV_PGRAPH_ABS_UCLIP_YMIN                         0x00400540 /* RW-4R */
#define NV_PGRAPH_ABS_UCLIP_YMIN_VALUE                         17:0 /* RWXSF */
#define NV_PGRAPH_ABS_UCLIP_YMAX                         0x00400548 /* RW-4R */
#define NV_PGRAPH_ABS_UCLIP_YMAX_VALUE                         17:0 /* RWXSF */
#define NV_PGRAPH_ABS_UCLIPA_XMIN                        0x00400560 /* RW-4R */
#define NV_PGRAPH_ABS_UCLIPA_XMIN_VALUE                        17:0 /* RWXSF */
#define NV_PGRAPH_ABS_UCLIPA_XMAX                        0x00400568 /* RW-4R */
#define NV_PGRAPH_ABS_UCLIPA_XMAX_VALUE                        17:0 /* RWXSF */
#define NV_PGRAPH_ABS_UCLIPA_YMIN                        0x00400564 /* RW-4R */
#define NV_PGRAPH_ABS_UCLIPA_YMIN_VALUE                        17:0 /* RWXSF */
#define NV_PGRAPH_ABS_UCLIPA_YMAX                        0x0040056C /* RW-4R */
#define NV_PGRAPH_ABS_UCLIPA_YMAX_VALUE                        17:0 /* RWXSF */
#define NV_PGRAPH_SOURCE_COLOR                           0x0040050C /* RW-4R */
#define NV_PGRAPH_SOURCE_COLOR_VALUE                           31:0 /* RWXVF */
#define NV_PGRAPH_EXCEPTIONS                             0x00400508 /* RW-4R */
#define NV_PGRAPH_EXCEPTIONS_VALID                             27:0 /* RWIVF */
#define NV_PGRAPH_EXCEPTIONS_VALID_0                     0x00000000 /* RWI-V */
#define NV_PGRAPH_EXCEPTIONS_CLIP_MIN                         28:28 /* RWIVF */
#define NV_PGRAPH_EXCEPTIONS_CLIP_MIN_NO_ERROR           0x00000000 /* RWI-V */
#define NV_PGRAPH_EXCEPTIONS_CLIP_MIN_ONLY               0x00000001 /* RW--V */
#define NV_PGRAPH_EXCEPTIONS_CLIPA_MIN                        29:29 /* RWIVF */
#define NV_PGRAPH_EXCEPTIONS_CLIPA_MIN_NO_ERROR          0x00000000 /* RWI-V */
#define NV_PGRAPH_EXCEPTIONS_CLIPA_MIN_ONLY              0x00000001 /* RW--V */
#define NV_PGRAPH_EXCEPTIONS_CLIP_MAX                         30:30 /* RWIVF */
#define NV_PGRAPH_EXCEPTIONS_CLIP_MAX_NO_ERROR           0x00000000 /* RWI-V */
#define NV_PGRAPH_EXCEPTIONS_CLIP_MAX_ONLY               0x00000001 /* RW--V */
#define NV_PGRAPH_EXCEPTIONS_CLIPA_MAX                        31:31 /* RWIVF */
#define NV_PGRAPH_EXCEPTIONS_CLIPA_MAX_NO_ERROR          0x00000000 /* RWI-V */
#define NV_PGRAPH_EXCEPTIONS_CLIPA_MAX_ONLY              0x00000001 /* RW--V */
#define NV_PGRAPH_ABS_ICLIP_XMAX                         0x00400534 /* RW-4R */
#define NV_PGRAPH_ABS_ICLIP_XMAX_VALUE                         17:0 /* RWXSF */
#define NV_PGRAPH_ABS_ICLIP_YMAX                         0x00400538 /* RW-4R */
#define NV_PGRAPH_ABS_ICLIP_YMAX_VALUE                         17:0 /* RWXSF */
#define NV_PGRAPH_CLIPX_0                                0x00400524 /* RW-4R */
#define NV_PGRAPH_CLIPX_0_CLIP0_MIN                             1:0 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP0_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP0_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP0_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP0_MAX                             3:2 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP0_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP0_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP0_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP1_MIN                             5:4 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP1_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP1_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP1_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP1_MAX                             7:6 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP1_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP1_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP1_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP2_MIN                             9:8 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP2_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP2_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP2_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP2_MAX                           11:10 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP2_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP2_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP2_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP3_MIN                           13:12 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP3_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP3_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP3_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP3_MAX                           15:14 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP3_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP3_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP3_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP4_MIN                           17:16 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP4_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP4_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP4_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP4_MAX                           19:18 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP4_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP4_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP4_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP5_MIN                           21:20 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP5_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP5_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP5_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP5_MAX                           23:22 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP5_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP5_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP5_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP6_MIN                           25:24 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP6_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP6_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP6_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP6_MAX                           27:26 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP6_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP6_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP6_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP7_MIN                           29:28 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP7_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP7_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP7_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP7_MAX                           31:30 /* RWIVF */
#define NV_PGRAPH_CLIPX_0_CLIP7_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_0_CLIP7_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_0_CLIP7_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1                                0x00400528 /* RW-4R */
#define NV_PGRAPH_CLIPX_1_CLIP8_MIN                             1:0 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP8_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP8_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP8_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP8_MAX                             3:2 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP8_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP8_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP8_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP9_MIN                             5:4 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP9_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP9_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP9_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP9_MAX                             7:6 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP9_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP9_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP9_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP10_MIN                            9:8 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP10_MIN_GT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP10_MIN_LT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP10_MIN_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP10_MAX                          11:10 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP10_MAX_LT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP10_MAX_GT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP10_MAX_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP11_MIN                          13:12 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP11_MIN_GT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP11_MIN_LT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP11MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP11_MAX                          15:14 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP11_MAX_LT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP11_MAX_GT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP11_MAX_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP12_MIN                          17:16 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP12_MIN_GT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP12_MIN_LT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP12_MIN_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP12_MAX                          19:18 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP12_MAX_LT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP12_MAX_GT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP12_MAX_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP13_MIN                          21:20 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP13_MIN_GT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP13_MIN_LT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP13_MIN_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP13_MAX                          23:22 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP13_MAX_LT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP13_MAX_GT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP13_MAX_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP14_MIN                          25:24 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP14_MIN_GT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP14_MIN_LT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP14_MIN_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP14_MAX                          27:26 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP14_MAX_LT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP14_MAX_GT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP14_MAX_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP15_MIN                          29:28 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP15_MIN_GT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP15_MIN_LT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP15_MIN_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP15_MAX                          31:30 /* RWIVF */
#define NV_PGRAPH_CLIPX_1_CLIP15_MAX_LT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPX_1_CLIP15_MAX_GT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPX_1_CLIP15_MAX_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0                                0x0040052c /* RW-4R */
#define NV_PGRAPH_CLIPY_0_CLIP0_MIN                             1:0 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP0_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP0_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP0_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP0_MAX                             3:2 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP0_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP0_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP0_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP1_MIN                             5:4 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP1_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP1_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP1_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP1_MAX                             7:6 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP1_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP1_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP1_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP2_MIN                             9:8 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP2_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP2_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP2_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP2_MAX                           11:10 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP2_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP2_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP2_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP3_MIN                           13:12 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP3_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP3_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP3_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP3_MAX                           15:14 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP3_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP3_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP3_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP4_MIN                           17:16 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP4_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP4_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP4_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP4_MAX                           19:18 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP4_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP4_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP4_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP5_MIN                           21:20 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP5_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP5_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP5_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP5_MAX                           23:22 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP5_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP5_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP5_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP6_MIN                           25:24 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP6_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP6_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP6_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP6_MAX                           27:26 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP6_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP6_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP6_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP7_MIN                           29:28 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP7_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP7_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP7_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP7_MAX                           31:30 /* RWIVF */
#define NV_PGRAPH_CLIPY_0_CLIP7_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_0_CLIP7_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_0_CLIP7_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1                                0x00400530 /* RW-4R */
#define NV_PGRAPH_CLIPY_1_CLIP8_MIN                             1:0 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP8_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP8_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP8_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP8_MAX                             3:2 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP8_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP8_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP8_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP9_MIN                             5:4 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP9_MIN_GT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP9_MIN_LT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP9_MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP9_MAX                             7:6 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP9_MAX_LT                   0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP9_MAX_GT                   0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP9_MAX_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP10_MIN                            9:8 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP10_MIN_GT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP10_MIN_LT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP10_MIN_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP10_MAX                          11:10 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP10_MAX_LT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP10_MAX_GT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP10_MAX_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP11_MIN                          13:12 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP11_MIN_GT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP11_MIN_LT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP11MIN_EQ                   0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP11_MAX                          15:14 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP11_MAX_LT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP11_MAX_GT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP11_MAX_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP12_MIN                          17:16 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP12_MIN_GT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP12_MIN_LT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP12_MIN_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP12_MAX                          19:18 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP12_MAX_LT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP12_MAX_GT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP12_MAX_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP13_MIN                          21:20 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP13_MIN_GT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP13_MIN_LT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP13_MIN_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP13_MAX                          23:22 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP13_MAX_LT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP13_MAX_GT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP13_MAX_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP14_MIN                          25:24 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP14_MIN_GT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP14_MIN_LT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP14_MIN_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP14_MAX                          27:26 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP14_MAX_LT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP14_MAX_GT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP14_MAX_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP15_MIN                          29:28 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP15_MIN_GT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP15_MIN_LT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP15_MIN_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP15_MAX                          31:30 /* RWIVF */
#define NV_PGRAPH_CLIPY_1_CLIP15_MAX_LT                  0x00000000 /* RW--V */
#define NV_PGRAPH_CLIPY_1_CLIP15_MAX_GT                  0x00000001 /* RWI-V */
#define NV_PGRAPH_CLIPY_1_CLIP15_MAX_EQ                  0x00000002 /* RW--V */
#define NV_PGRAPH_MISC24_0                               0x00400510 /* RW-4R */
#define NV_PGRAPH_MISC24_0_VALUE                               23:0 /* RWXSF */
#define NV_PGRAPH_MISC24_1                               0x00400570 /* RW-4R */
#define NV_PGRAPH_MISC24_1_VALUE                               23:0 /* RWXSF */
#define NV_PGRAPH_PASSTHRU                               0x0040054c /* RW-4R */
#define NV_PGRAPH_PASSTHRU_VALUE                               23:0 /* RWXSF */
#define NV_PGRAPH_ZFOG_RAM(i)                    (0x00400580+(i)*4) /* RW-4A */
#define NV_PGRAPH_ZFOG_RAM__SIZE_1                               16 /*       */
#define NV_PGRAPH_ZFOG_RAM_ZETA                                15:0 /* RWXUF */
#define NV_PGRAPH_ZFOG_RAM_FOG                                23:16 /* RWXUF */
#define NV_PGRAPH_D3D_XY                                 0x004005c0 /* RW-4R */
#define NV_PGRAPH_D3D_XY_X_VALUE                               15:0 /* RWXSF */
#define NV_PGRAPH_D3D_XY_Y_VALUE                              31:16 /* RWXSF */
#define NV_PGRAPH_D3D_UV                                 0x004005c4 /* RW-4R */
#define NV_PGRAPH_D3D_UV_U_VALUE                               15:0 /* RWXSF */
#define NV_PGRAPH_D3D_UV_V_VALUE                              31:16 /* RWXSF */
#define NV_PGRAPH_D3D_ZETA                               0x004005c8 /* RW-4R */
#define NV_PGRAPH_D3D_ZETA_VALUE                               15:0 /* RWXSF */
#define NV_PGRAPH_D3D_RGB                                0x004005cc /* RW-4R */
#define NV_PGRAPH_D3D_RGB_VALUE                                15:0 /* RWXSF */
#define NV_PGRAPH_D3D_FOG                                0x004005d0 /* RW-4R */
#define NV_PGRAPH_D3D_FOG_I0                                    3:0 /* RWXSF */
#define NV_PGRAPH_D3D_FOG_I1                                    7:4 /* RWXSF */
#define NV_PGRAPH_D3D_FOG_I2                                   11:8 /* RWXSF */
#define NV_PGRAPH_D3D_FOG_I3                                  15:12 /* RWXSF */
#define NV_PGRAPH_D3D_FOG_I4                                  19:16 /* RWXSF */
#define NV_PGRAPH_D3D_FOG_I5                                  23:20 /* RWXSF */
#define NV_PGRAPH_D3D_FOG_FOG_VALUE                           31:24 /* RWXSF */
#define NV_PGRAPH_D3D_M                                  0x004005d4 /* RW-4R */
#define NV_PGRAPH_D3D_M_VALUE                                  31:0 /* RWXSF */
#define NV_PGRAPH_DMA_INTR_0                             0x00401100 /* RW-4R */
#define NV_PGRAPH_DMA_INTR_0_INSTANCE                           0:0 /* RWXVF */
#define NV_PGRAPH_DMA_INTR_0_INSTANCE_NOT_PENDING        0x00000000 /* R---V */
#define NV_PGRAPH_DMA_INTR_0_INSTANCE_PENDING            0x00000001 /* R---V */
#define NV_PGRAPH_DMA_INTR_0_INSTANCE_RESET              0x00000001 /* -W--V */
#define NV_PGRAPH_DMA_INTR_0_PRESENT                            4:4 /* RWXVF */
#define NV_PGRAPH_DMA_INTR_0_PRESENT_NOT_PENDING         0x00000000 /* R---V */
#define NV_PGRAPH_DMA_INTR_0_PRESENT_PENDING             0x00000001 /* R---V */
#define NV_PGRAPH_DMA_INTR_0_PRESENT_RESET               0x00000001 /* -W--V */
#define NV_PGRAPH_DMA_INTR_0_PROTECTION                         8:8 /* RWXVF */
#define NV_PGRAPH_DMA_INTR_0_PROTECTION_NOT_PENDING      0x00000000 /* R---V */
#define NV_PGRAPH_DMA_INTR_0_PROTECTION_PENDING          0x00000001 /* R---V */
#define NV_PGRAPH_DMA_INTR_0_PROTECTION_RESET            0x00000001 /* -W--V */
#define NV_PGRAPH_DMA_INTR_0_LINEAR                           12:12 /* RWXVF */
#define NV_PGRAPH_DMA_INTR_0_LINEAR_NOT_PENDING          0x00000000 /* R---V */
#define NV_PGRAPH_DMA_INTR_0_LINEAR_PENDING              0x00000001 /* R---V */
#define NV_PGRAPH_DMA_INTR_0_LINEAR_RESET                0x00000001 /* -W--V */
#define NV_PGRAPH_DMA_INTR_0_NOTIFY                           16:16 /* RWXVF */
#define NV_PGRAPH_DMA_INTR_0_NOTIFY_NOT_PENDING          0x00000000 /* R---V */
#define NV_PGRAPH_DMA_INTR_0_NOTIFY_PENDING              0x00000001 /* R---V */
#define NV_PGRAPH_DMA_INTR_0_NOTIFY_RESET                0x00000001 /* -W--V */
#define NV_PGRAPH_DMA_INTR_EN_0                          0x00401140 /* RW-4R */
#define NV_PGRAPH_DMA_INTR_EN_0_INSTANCE                        0:0 /* RWIVF */
#define NV_PGRAPH_DMA_INTR_EN_0_INSTANCE_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_DMA_INTR_EN_0_INSTANCE_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_DMA_INTR_EN_0_PRESENT                         4:4 /* RWIVF */
#define NV_PGRAPH_DMA_INTR_EN_0_PRESENT_DISABLED         0x00000000 /* RWI-V */
#define NV_PGRAPH_DMA_INTR_EN_0_PRESENT_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_DMA_INTR_EN_0_PROTECTION                      8:8 /* RWIVF */
#define NV_PGRAPH_DMA_INTR_EN_0_PROTECTION_DISABLED      0x00000000 /* RWI-V */
#define NV_PGRAPH_DMA_INTR_EN_0_PROTECTION_ENABLED       0x00000001 /* RW--V */
#define NV_PGRAPH_DMA_INTR_EN_0_LINEAR                        12:12 /* RWIVF */
#define NV_PGRAPH_DMA_INTR_EN_0_LINEAR_DISABLED          0x00000000 /* RWI-V */
#define NV_PGRAPH_DMA_INTR_EN_0_LINEAR_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_DMA_INTR_EN_0_NOTIFY                        16:16 /* RWIVF */
#define NV_PGRAPH_DMA_INTR_EN_0_NOTIFY_DISABLED          0x00000000 /* RWI-V */
#define NV_PGRAPH_DMA_INTR_EN_0_NOTIFY_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_DMA_ACCESS                             0x00401200 /* -W-4R */
#define NV_PGRAPH_DMA_PTE_ACCESS_PTE                            0:0 /* -W-VF */
#define NV_PGRAPH_DMA_PTE_ACCESS_PTE_ENABLE              0x00000001 /* -W--V */
#define NV_PGRAPH_DMA_CONTROL                            0x00401210 /* RW-4R */
#define NV_PGRAPH_DMA_CONTROL_ADJUST                           11:0 /* RWXUF */
#define NV_PGRAPH_DMA_CONTROL_PAGE_TABLE                      16:16 /* RWXVF */
#define NV_PGRAPH_DMA_CONTROL_PAGE_TABLE_NOT_PRESENT     0x00000000 /* RW--V */
#define NV_PGRAPH_DMA_CONTROL_PAGE_TABLE_PRESENT         0x00000001 /* RW--V */
#define NV_PGRAPH_DMA_CONTROL_TARGET_NODE                     25:24 /* RWXUF */
#define NV_PGRAPH_DMA_CONTROL_TARGET_NODE_NVM            0x00000000 /* RW--V */
#define NV_PGRAPH_DMA_CONTROL_TARGET_NODE_CART           0x00000001 /* RW--V */
#define NV_PGRAPH_DMA_CONTROL_TARGET_NODE_PCI            0x00000002 /* RW--V */
#define NV_PGRAPH_DMA_CONTROL_TARGET_NODE_AGP            0x00000003 /* RW--V */
#define NV_PGRAPH_DMA_LIMIT                              0x00401220 /* RW-4R */
#define NV_PGRAPH_DMA_LIMIT_OFFSET                             31:0 /* RWXUF */
#define NV_PGRAPH_DMA_TLB_PTE                            0x00401230 /* RW-4R */
#define NV_PGRAPH_DMA_TLB_PTE_PAGE                              0:0 /* RWXVF */
#define NV_PGRAPH_DMA_TLB_PTE_PAGE_NOT_PRESENT           0x00000000 /* RW--V */
#define NV_PGRAPH_DMA_TLB_PTE_PAGE_PRESENT               0x00000001 /* RW--V */
#define NV_PGRAPH_DMA_TLB_PTE_ACCESS                            1:1 /* RWXVF */
#define NV_PGRAPH_DMA_TLB_PTE_ACCESS_READ_ONLY           0x00000000 /* RW--V */
#define NV_PGRAPH_DMA_TLB_PTE_ACCESS_READ_WRITE          0x00000001 /* RW--V */
#define NV_PGRAPH_DMA_TLB_PTE_FRAME_ADDRESS                   31:12 /* RWXUF */
#define NV_PGRAPH_DMA_TLB_TAG                            0x00401240 /* RW-4R */
#define NV_PGRAPH_DMA_TLB_TAG_ADDRESS                         31:12 /* RWXUF */
#define NV_PGRAPH_DMA_ADJ_OFFSET                         0x00401250 /* RW-4R */
#define NV_PGRAPH_DMA_ADJ_OFFSET_VALUE                         31:0 /* RWXUF */
#define NV_PGRAPH_DMA_OFFSET                             0x00401260 /* RW-4R */
#define NV_PGRAPH_DMA_OFFSET_VALUE                             31:0 /* RW-UF */
#define NV_PGRAPH_DMA_SIZE                               0x00401270 /* RW-4R */
#define NV_PGRAPH_DMA_SIZE_VALUE                               21:0 /* RW-UF */
#define NV_PGRAPH_DMA_XLATE_INST                         0x00401280 /* RW-4R */
#define NV_PGRAPH_DMA_XLATE_INST_VALUE                         15:0 /* RWXUF */
#define NV_PGRAPH_DMA_Y_SIZE                             0x00401290 /* RW-4R */
#define NV_PGRAPH_DMA_Y_SIZE_VALUE                             10:0 /* RW-UF */
#define NV_PGRAPH_DMA_LINEAR_LIMIT                       0x00401400 /* RW-4R */
#define NV_PGRAPH_DMA_LINEAR_LIMIT_VALUE                       21:0 /* RWXUF */
#define NV_PGRAPH_DMA_START(i)                  (0x00401800+(i)*16) /* RW-4A */
#define NV_PGRAPH_DMA_START__SIZE_1                               3 /*       */
#define NV_PGRAPH_DMA_START_VALUE                              31:0 /* RWXUF */
#define NV_PGRAPH_DMA_PITCH                              0x00401830 /* RW-4R */
#define NV_PGRAPH_DMA_PITCH_0                                  15:0 /* RWXS4 */
#define NV_PGRAPH_DMA_PITCH_1                                 31:16 /* RWXS4 */
#define NV_PGRAPH_DMA_FORMAT                             0x00401840 /* RW-4R */
#define NV_PGRAPH_DMA_FORMAT_SRC                                2:0 /* RWXS4 */
#define NV_PGRAPH_DMA_FORMAT_SRC_INC_1                   0x00000001 /* RW--V */
#define NV_PGRAPH_DMA_FORMAT_SRC_INC_2                   0x00000002 /* RW--V */
#define NV_PGRAPH_DMA_FORMAT_SRC_INC_4                   0x00000004 /* RW--V */
#define NV_PGRAPH_DMA_FORMAT_DST                               10:8 /* RWXS4 */
#define NV_PGRAPH_DMA_FORMAT_DST_INC_1                   0x00000001 /* RW--V */
#define NV_PGRAPH_DMA_FORMAT_DST_INC_2                   0x00000002 /* RW--V */
#define NV_PGRAPH_DMA_FORMAT_DST_INC_4                   0x00000004 /* RW--V */
#define NV_PGRAPH_DMA_MTMF_NOTIFY                        0x00401820 /* RW-4R */
#define NV_PGRAPH_DMA_MTMF_NOTIFY_TYPE                         31:0 /* RWIVF */
#define NV_PGRAPH_DMA_MTMF_NOTIFY_TYPE_HW                0x00000000 /* RWI-V */
/* dev_video.ref */
#define NV_PVIDEO                             0x006802FF:0x00680000 /* RW--D */
#define NV_PVIDEO_INTR_0                                 0x00680100 /* RWI4R */
#define NV_PVIDEO_INTR_0_NOTIFY                                 0:0 /* RWIVF */
#define NV_PVIDEO_INTR_0_NOTIFY_NOT_PENDING              0x00000000 /* R-I-V */
#define NV_PVIDEO_INTR_0_NOTIFY_PENDING                  0x00000001 /* R---V */
#define NV_PVIDEO_INTR_0_NOTIFY_RESET                    0x00000001 /* -W--V */
#define NV_PVIDEO_INTR_EN_0                              0x00680140 /* RWI4R */
#define NV_PVIDEO_INTR_EN_0_NOTIFY                              0:0 /* RWIVF */
#define NV_PVIDEO_INTR_EN_0_NOTIFY_DISABLED              0x00000000 /* RWI-V */
#define NV_PVIDEO_INTR_EN_0_NOTIFY_ENABLED               0x00000001 /* RW--V */
#define NV_PVIDEO_STEP_SIZE                              0x00680200 /* RW-4R */
#define NV_PVIDEO_STEP_SIZE_X                                  11:0 /* RWXVF */
#define NV_PVIDEO_STEP_SIZE_Y                                 27:16 /* RWXVF */
#define NV_PVIDEO_CONTROL_Y                              0x00680204 /* RW-4R */
#define NV_PVIDEO_CONTROL_Y_BLUR                                0:0 /* RW--F */
#define NV_PVIDEO_CONTROL_Y_BLUR_OFF                     0x00000000 /* RW--V */
#define NV_PVIDEO_CONTROL_Y_BLUR_ON                      0x00000001 /* RW--V */
#define NV_PVIDEO_CONTROL_Y_LINE                                4:4 /* RW--F */
#define NV_PVIDEO_CONTROL_Y_LINE_HALF                    0x00000000 /* RW--V */
#define NV_PVIDEO_CONTROL_Y_LINE_FULL                    0x00000001 /* RW--V */
#define NV_PVIDEO_CONTROL_X                              0x00680208 /* RW-4R */
#define NV_PVIDEO_CONTROL_X_WEIGHT                              0:0 /* RW--F */
#define NV_PVIDEO_CONTROL_X_WEIGHT_LIGHT                 0x00000000 /* RW--V */
#define NV_PVIDEO_CONTROL_X_WEIGHT_HEAVY                 0x00000001 /* RW--V */
#define NV_PVIDEO_CONTROL_X_SHARPENING                          4:4 /* RW--F */
#define NV_PVIDEO_CONTROL_X_SHARPENING_OFF               0x00000000 /* RW--V */
#define NV_PVIDEO_CONTROL_X_SHARPENING_ON                0x00000001 /* RW--V */
#define NV_PVIDEO_CONTROL_X_SMOOTHING                           8:8 /* RW--F */
#define NV_PVIDEO_CONTROL_X_SMOOTHING_OFF                0x00000000 /* RW--V */
#define NV_PVIDEO_CONTROL_X_SMOOTHING_ON                 0x00000001 /* RW--V */
#define NV_PVIDEO_BUFF0_START                            0x0068020c /* RW-4R */
#define NV_PVIDEO_BUFF0_START_ADDRESS                          22:2 /* RW-UF */
#define NV_PVIDEO_BUFF1_START                            0x00680210 /* RW-4R */
#define NV_PVIDEO_BUFF1_START_ADDRESS                          22:2 /* RW-UF */
#define NV_PVIDEO_BUFF0_PITCH                            0x00680214 /* RW-4R */
#define NV_PVIDEO_BUFF0_PITCH_LENGTH                           13:4 /* RW-UF */
#define NV_PVIDEO_BUFF1_PITCH                            0x00680218 /* RW-4R */
#define NV_PVIDEO_BUFF1_PITCH_LENGTH                           13:4 /* RW-UF */
#define NV_PVIDEO_BUFF0_OFFSET                           0x0068021c /* RW-4R */
#define NV_PVIDEO_BUFF0_OFFSET_X                                0:0 /* RW--F */
#define NV_PVIDEO_BUFF0_OFFSET_X_OFF                     0x00000000 /* RW--V */
#define NV_PVIDEO_BUFF0_OFFSET_X_ON                      0x00000001 /* RW--V */
#define NV_PVIDEO_BUFF0_OFFSET_Y                                5:4 /* RW--F */
#define NV_PVIDEO_BUFF0_OFFSET_Y_OFF                     0x00000000 /* RW--V */
#define NV_PVIDEO_BUFF0_OFFSET_Y_QUARTER                 0x00000001 /* RW--V */
#define NV_PVIDEO_BUFF0_OFFSET_Y_HALF                    0x00000002 /* RW--V */
#define NV_PVIDEO_BUFF1_OFFSET                           0x00680220 /* RW-4R */
#define NV_PVIDEO_BUFF1_OFFSET_X                                0:0 /* RW--F */
#define NV_PVIDEO_BUFF1_OFFSET_X_OFF                     0x00000000 /* RW--V */
#define NV_PVIDEO_BUFF1_OFFSET_X_ON                      0x00000001 /* RW--V */
#define NV_PVIDEO_BUFF1_OFFSET_Y                                5:4 /* RW--F */
#define NV_PVIDEO_BUFF1_OFFSET_Y_OFF                     0x00000000 /* RW--V */
#define NV_PVIDEO_BUFF1_OFFSET_Y_QUARTER                 0x00000001 /* RW--V */
#define NV_PVIDEO_BUFF1_OFFSET_Y_HALF                    0x00000002 /* RW--V */
#define NV_PVIDEO_OE_STATE                               0x00680224 /* RW-4R */
#define NV_PVIDEO_OE_STATE_BUFF0_INTR_NOTIFY                    0:0 /* RWXVF */
#define NV_PVIDEO_OE_STATE_BUFF1_INTR_NOTIFY                    4:4 /* RWXVF */
#define NV_PVIDEO_OE_STATE_BUFF0_ERROR                          8:8 /* RWXVF */
#define NV_PVIDEO_OE_STATE_BUFF1_ERROR                        12:12 /* RWXVF */
#define NV_PVIDEO_OE_STATE_BUFF0_IN_USE                       16:16 /* RWXVF */
#define NV_PVIDEO_OE_STATE_BUFF1_IN_USE                       20:20 /* RWXVF */
#define NV_PVIDEO_OE_STATE_CURRENT_BUFFER                     24:24 /* RWXVF */
#define NV_PVIDEO_OE_STATE_CURRENT_BUFFER_0              0x00000000 /* RW--V */
#define NV_PVIDEO_OE_STATE_CURRENT_BUFFER_1              0x00000001 /* RW--V */
#define NV_PVIDEO_SU_STATE                               0x00680228 /* RW-4R */
#define NV_PVIDEO_SU_STATE_BUFF0_IN_USE                       16:16 /* RWXVF */
#define NV_PVIDEO_SU_STATE_BUFF1_IN_USE                       20:20 /* RWXVF */
#define NV_PVIDEO_RM_STATE                               0x0068022c /* RW-4R */
#define NV_PVIDEO_RM_STATE_BUFF0_INTR_NOTIFY                    0:0 /* RWXVF */
#define NV_PVIDEO_RM_STATE_BUFF1_INTR_NOTIFY                    4:4 /* RWXVF */
#define NV_PVIDEO_WINDOW_START                           0x00680230 /* RW-4R */
#define NV_PVIDEO_WINDOW_START_X                               10:0 /* RWXUF */
#define NV_PVIDEO_WINDOW_START_Y                              26:16 /* RWXUF */
#define NV_PVIDEO_WINDOW_SIZE                            0x00680234 /* RW-4R */
#define NV_PVIDEO_WINDOW_SIZE_X                                10:0 /* RWXUF */
#define NV_PVIDEO_WINDOW_SIZE_Y                               26:16 /* RWXUF */
#define NV_PVIDEO_FIFO_THRES                             0x00680238 /* RW-4R */
#define NV_PVIDEO_FIFO_THRES_SIZE                               6:3 /* RW--F */
#define NV_PVIDEO_FIFO_BURST                             0x0068023c /* RW-4R */
#define NV_PVIDEO_FIFO_BURST_LENGTH                             1:0 /* RW--F */
#define NV_PVIDEO_FIFO_BURST_LENGTH_32                   0x00000001 /* RW--V */
#define NV_PVIDEO_FIFO_BURST_LENGTH_64                   0x00000002 /* RW--V */
#define NV_PVIDEO_FIFO_BURST_LENGTH_128                  0x00000003 /* RW--V */
#define NV_PVIDEO_KEY                                    0x00680240 /* RW-4R */
#define NV_PVIDEO_KEY_INDEX                                     7:0 /* RW-VF */
#define NV_PVIDEO_KEY_565                                      15:0 /* RW-VF */
#define NV_PVIDEO_KEY_555                                      14:0 /* RW-VF */
#define NV_PVIDEO_KEY_888                                      23:0 /* RW-VF */
#define NV_PVIDEO_KEY_PACK                                    31:24 /* RW-VF */
#define NV_PVIDEO_OVERLAY                                0x00680244 /* RWI4R */
#define NV_PVIDEO_OVERLAY_VIDEO                                 0:0 /* RWI-F */
#define NV_PVIDEO_OVERLAY_VIDEO_OFF                      0x00000000 /* RWI-V */
#define NV_PVIDEO_OVERLAY_VIDEO_ON                       0x00000001 /* RW--V */
#define NV_PVIDEO_OVERLAY_KEY                                   4:4 /* RW--F */
#define NV_PVIDEO_OVERLAY_KEY_OFF                        0x00000000 /* RW--V */
#define NV_PVIDEO_OVERLAY_KEY_ON                         0x00000001 /* RW--V */
#define NV_PVIDEO_OVERLAY_FORMAT                                8:8 /* RW--F */
#define NV_PVIDEO_OVERLAY_FORMAT_CCIR                    0x00000000 /* RW--V */
#define NV_PVIDEO_OVERLAY_FORMAT_YUY2                    0x00000001 /* RW--V */
#define NV_PVIDEO_RED_CSC                                0x00680280 /* RW-4R */
#define NV_PVIDEO_RED_CSC_OFFSET                                7:0 /* RWX-F */
#define NV_PVIDEO_GREEN_CSC                              0x00680284 /* RW-4R */
#define NV_PVIDEO_GREEN_CSC_OFFSET                              7:0 /* RWX-F */
#define NV_PVIDEO_BLUE_CSC                               0x00680288 /* RW-4R */
#define NV_PVIDEO_BLUE_CSC_OFFSET                               7:0 /* RWX-F */
#define NV_PVIDEO_CSC_ADJUST                             0x0068028c /* RW-4R */
#define NV_PVIDEO_CSC_ADJUST_B_FLAG                             0:0 /* RW--F */
#define NV_PVIDEO_CSC_ADJUST_B_FLAG_OFF                  0x00000000 /* RW--V */
#define NV_PVIDEO_CSC_ADJUST_B_FLAG_ON                   0x00000001 /* RW--V */
#define NV_PVIDEO_CSC_ADJUST_G_FLAG                             4:4 /* RW--F */
#define NV_PVIDEO_CSC_ADJUST_G_FLAG_OFF                  0x00000000 /* RW--V */
#define NV_PVIDEO_CSC_ADJUST_G_FLAG_ON                   0x00000001 /* RW--V */
#define NV_PVIDEO_CSC_ADJUST_R_FLAG                             8:8 /* RW--F */
#define NV_PVIDEO_CSC_ADJUST_R_FLAG_OFF                  0x00000000 /* RW--V */
#define NV_PVIDEO_CSC_ADJUST_R_FLAG_ON                   0x00000001 /* RW--V */
#define NV_PVIDEO_CSC_ADJUST_L_FLAG                           12:12 /* RW--F */
#define NV_PVIDEO_CSC_ADJUST_L_FLAG_OFF                  0x00000000 /* RW--V */
#define NV_PVIDEO_CSC_ADJUST_L_FLAG_ON                   0x00000001 /* RW--V */
#define NV_PVIDEO_CSC_ADJUST_CHROMA                           16:16 /* RW--F */
#define NV_PVIDEO_CSC_ADJUST_CHROMA_OFF                  0x00000000 /* RW--V */
#define NV_PVIDEO_CSC_ADJUST_CHROMA_ON                   0x00000001 /* RW--V */
/* vga.ref */
#define NV_PRMCIO                             0x00601FFF:0x00601000 /* RW--D */
#define NV_PRMCIO_INP0                                   0x006013c2 /* R--1R */
#define NV_PRMCIO_INP0__MONO                             0x006013ba /* R--1R */
#define NV_PRMCIO_INP0__COLOR                            0x006013da /* R--1R */
#define NV_PRMCIO_INP0__READ_MONO                        0x006013ca /* R--1R */
#define NV_PRMCIO_INP0__WRITE_MONO                       0x006013ba /* -W-1R */
#define NV_PRMCIO_INP0__WRITE_COLOR                      0x006013da /* -W-1R */
#define NV_PRMCIO_ARX                                    0x006013c0 /* RW-1R */
#define NV_PRMCIO_AR_PALETTE__WRITE                      0x006013c0 /* -W-1R */
#define NV_PRMCIO_AR_PALETTE__READ                       0x006013c1 /* R--1R */
#define NV_PRMCIO_AR_MODE__WRITE                         0x006013c0 /* -W-1R */
#define NV_PRMCIO_AR_MODE__READ                          0x006013c1 /* R--1R */
#define NV_PRMCIO_AR_MODE_INDEX                          0x00000010 /*       */
#define NV_PRMCIO_AR_OSCAN__WRITE                        0x006013c0 /* -W-1R */
#define NV_PRMCIO_AR_OSCAN__READ                         0x006013c1 /* R--1R */
#define NV_PRMCIO_AR_OSCAN_INDEX                         0x00000011 /*       */
#define NV_PRMCIO_AR_PLANE__WRITE                        0x006013c0 /* -W-1R */
#define NV_PRMCIO_AR_PLANE__READ                         0x006013c1 /* R--1R */
#define NV_PRMCIO_AR_PLANE_INDEX                         0x00000012 /*       */
#define NV_PRMCIO_AR_HPP__WRITE                          0x006013c0 /* -W-1R */
#define NV_PRMCIO_AR_HPP__READ                           0x006013c1 /* R--1R */
#define NV_PRMCIO_AR_HPP_INDEX                           0x00000013 /*       */
#define NV_PRMCIO_AR_CSEL__WRITE                         0x006013c0 /* -W-1R */
#define NV_PRMCIO_AR_CSEL__READ                          0x006013c1 /* R--1R */
#define NV_PRMCIO_AR_CSEL_INDEX                          0x00000014 /*       */
#define NV_PRMCIO_CRX__MONO                              0x006013b4 /* RW-1R */
#define NV_PRMCIO_CRX__COLOR                             0x006013d4 /* RW-1R */
#define NV_PRMCIO_CR__MONO                               0x006013b5 /* RW-1R */
#define NV_PRMCIO_CR__COLOR                              0x006013d5 /* RW-1R */
#define NV_PRMCIO_CRE__MONO                              0x006013b5 /* RW-1R */
#define NV_PRMCIO_CRE__COLOR                             0x006013d5 /* RW-1R */

// actually an NV4 register, but nv3ddraw uses this file for both NV3 and NV4
#define NV_PCRTC_RASTER                                  0x00600808 /* R-I4R */
#define NV_PCRTC_RASTER_POSITION                               10:0 /* R---F */
#define NV_PCRTC_RASTER_SA_LOAD                               13:12 /* R---F */
#define NV_PCRTC_RASTER_SA_LOAD_DISPLAY                  0x00000000 /* R---V */
#define NV_PCRTC_RASTER_SA_LOAD_BEFORE                   0x00000001 /* R---V */
#define NV_PCRTC_RASTER_SA_LOAD_AFTER                    0x00000002 /* R---V */
#define NV_PCRTC_RASTER_VERT_BLANK                            16:16 /* R---F */
#define NV_PCRTC_RASTER_VERT_BLANK_ACTIVE                0x00000001 /* R---V */
#define NV_PCRTC_RASTER_VERT_BLANK_INACTIVE              0x00000000 /* R---V */

/* vga.ref */
#define NV_CIO                                          0x3DF:0x3B0 /* ----- */
#define NV_CIO_INP0                                      0x000003c2 /* R--1R */
#define NV_CIO_INP0__MONO                                0x000003ba /* R--1R */
#define NV_CIO_INP0__COLOR                               0x000003da /* R--1R */
#define NV_CIO_INP0__READ_MONO                           0x000003ca /* R--1R */
#define NV_CIO_INP0__WRITE_MONO                          0x000003ba /* -W-1R */
#define NV_CIO_INP0__WRITE_COLOR                         0x000003da /* -W-1R */
#define NV_CIO_ARX                                       0x000003c0 /* RW-1R */
#define NV_CIO_AR_PALETTE__WRITE                         0x000003c0 /* -W-1R */
#define NV_CIO_AR_PALETTE__READ                          0x000003c1 /* R--1R */
#define NV_CIO_AR_MODE__WRITE                            0x000003c0 /* -W-1R */
#define NV_CIO_AR_MODE__READ                             0x000003c1 /* R--1R */
#define NV_CIO_AR_MODE_INDEX                             0x00000010 /*       */
#define NV_CIO_AR_OSCAN__WRITE                           0x000003c0 /* -W-1R */
#define NV_CIO_AR_OSCAN__READ                            0x000003c1 /* R--1R */
#define NV_CIO_AR_OSCAN_INDEX                            0x00000011 /*       */
#define NV_CIO_AR_PLANE__WRITE                           0x000003c0 /* -W-1R */
#define NV_CIO_AR_PLANE__READ                            0x000003c1 /* R--1R */
#define NV_CIO_AR_PLANE_INDEX                            0x00000012 /*       */
#define NV_CIO_AR_HPP__WRITE                             0x000003c0 /* -W-1R */
#define NV_CIO_AR_HPP__READ                              0x000003c1 /* R--1R */
#define NV_CIO_AR_HPP_INDEX                              0x00000013 /*       */
#define NV_CIO_AR_CSEL__WRITE                            0x000003c0 /* -W-1R */
#define NV_CIO_AR_CSEL__READ                             0x000003c1 /* R--1R */
#define NV_CIO_AR_CSEL_INDEX                             0x00000014 /*       */
#define NV_CIO_CRX__MONO                                 0x000003b4 /* RW-1R */
#define NV_CIO_CRX__COLOR                                0x000003d4 /* RW-1R */
#define NV_CIO_CR__MONO                                  0x000003b5 /* RW-1R */
#define NV_CIO_CR__COLOR                                 0x000003d5 /* RW-1R */
#define NV_CIO_CR_HDT_INDEX                              0x00000000 /*       */
#define NV_CIO_CR_HDE_INDEX                              0x00000001 /*       */
#define NV_CIO_CR_HBS_INDEX                              0x00000002 /*       */
#define NV_CIO_CR_HBE_INDEX                              0x00000003 /*       */
#define NV_CIO_CR_HBE_4_0                                       4:0 /* RW--F */
#define NV_CIO_CR_HRS_INDEX                              0x00000004 /*       */
#define NV_CIO_CR_HRE_INDEX                              0x00000005 /*       */
#define NV_CIO_CR_HRE_HBE_5                                     7:7 /* RW--F */
#define NV_CIO_CR_HRE_4_0                                       4:0 /* RW--F */
#define NV_CIO_CR_VDT_INDEX                              0x00000006 /*       */
#define NV_CIO_CR_OVL_INDEX                              0x00000007 /*       */
#define NV_CIO_CR_OVL_VDE_8                                     1:1 /* RW--F */
#define NV_CIO_CR_OVL_VDE_9                                     6:6 /* RW--F */
#define NV_CIO_CR_OVL_VDT_8                                     0:0 /* RW--F */
#define NV_CIO_CR_OVL_VDT_9                                     5:5 /* RW--F */
#define NV_CIO_CR_OVL_VBS_8                                     3:3 /* RW--F */
#define NV_CIO_CR_OVL_VRS_8                                     2:2 /* RW--F */
#define NV_CIO_CR_OVL_VRS_9                                     7:7 /* RW--F */
#define NV_CIO_CR_RSAL_INDEX                             0x00000008 /*       */
#define NV_CIO_CR_RSAL_PANNING                                  6:5 /* RW--F */
#define NV_CIO_CR_CELL_HT_INDEX                          0x00000009 /*       */
#define NV_CIO_CR_CELL_HT_SCANDBL                               7:7 /* RW--F */
#define NV_CIO_CR_CELL_HT_VBS_9                                 5:5 /* RW--F */
#define NV_CIO_CR_CURS_ST_INDEX                          0x0000000A /*       */
#define NV_CIO_CR_CURS_END_INDEX                         0x0000000B /*       */
#define NV_CIO_CR_SA_HI_INDEX                            0x0000000C /*       */
#define NV_CIO_CR_SA_LO_INDEX                            0x0000000D /*       */
#define NV_CIO_CR_TCOFF_HI_INDEX                         0x0000000E /*       */
#define NV_CIO_CR_TCOFF_LO_INDEX                         0x0000000F /*       */
#define NV_CIO_CR_VRS_INDEX                              0x00000010 /*       */
#define NV_CIO_CR_VRE_INDEX                              0x00000011 /*       */
#define NV_CIO_CR_VRE_3_0                                       3:0 /* RW--F */
#define NV_CIO_CR_VDE_INDEX                              0x00000012 /*       */
#define NV_CIO_CR_OFFSET_INDEX                           0x00000013 /*       */
#define NV_CIO_CR_ULINE_INDEX                            0x00000014 /*       */
#define NV_CIO_CR_VBS_INDEX                              0x00000015 /*       */
#define NV_CIO_CR_VBE_INDEX                              0x00000016 /*       */
#define NV_CIO_CR_MODE_INDEX                             0x00000017 /*       */
#define NV_CIO_CR_LCOMP_INDEX                            0x00000018 /*       */
#define NV_CIO_CR_GDATA_INDEX                            0x00000022 /*       */
#define NV_CIO_CR_ARFF_INDEX                             0x00000024 /*       */
#define NV_CIO_CR_ARX_INDEX                              0x00000026 /*       */
#define NV_CIO_CRE__MONO                                 0x000003b5 /* RW-1R */
#define NV_CIO_CRE__COLOR                                0x000003d5 /* RW-1R */
#define NV_CIO_CRE_RPC0_INDEX                           0x00000019 /*       */
#define NV_CIO_CRE_RPC0_START                                   4:0 /* RW--F */
#define NV_CIO_CRE_RPC0_OFFSET_10_8                             7:5 /* RW--F */
#define NV_CIO_CRE_RPC1_INDEX                            0x0000001A /*       */
#define NV_CIO_CRE_RPC1_LARGE                                  2:2 /* RW--F */
#define NV_CIO_CRE_FF_INDEX                              0x0000001B /*       */
#define NV_CIO_CRE_FF_BURST                                     2:0 /* RW--F */
#define NV_CIO_CRE_FF_BURST_8                            0x00000000 /* RW--V */
#define NV_CIO_CRE_FF_BURST_32                           0x00000001 /* RW--V */
#define NV_CIO_CRE_FF_BURST_64                           0x00000002 /* RW--V */
#define NV_CIO_CRE_FF_BURST_128                          0x00000003 /* RW--V */
#define NV_CIO_CRE_FF_BURST_256                          0x00000004 /* RW--V */
#define NV_CIO_CRE_ENH_INDEX                             0x0000001C /*       */
#define NV_CIO_CRE_PAGE0_INDEX                           0x0000001D /*       */
#define NV_CIO_CRE_PAGE1_INDEX                           0x0000001E /*       */
#define NV_CIO_CRE_FFLWM__INDEX                          0x00000020 /*       */
#define NV_CIO_CRE_FFLWM_LWM                                    5:0 /* RW--F */
#define NV_CIO_CRE_LSR_INDEX                             0x00000025 /*       */
#define NV_CIO_CRE_LSR_FORMAT                                   7:6 /* RW--F */
#define NV_CIO_CRE_LSR_FORMAT_8BIT                       0x00000001 /* RW--V */
#define NV_CIO_CRE_LSR_FORMAT_555                        0x00000002 /* RW--V */
#define NV_CIO_CRE_LSR_FORMAT_565                        0x00000003 /* RW--V */
#define NV_CIO_CRE_LSR_VDE_10                                   1:1 /* RW--F */
#define NV_CIO_CRE_LSR_VDT_10                                   0:0 /* RW--F */
#define NV_CIO_CRE_LSR_HBE_6                                    4:4 /* RW--F */
#define NV_CIO_CRE_LSR_VBS_10                                   3:3 /* RW--F */
#define NV_CIO_CRE_LSR_VRS_10                                   2:2 /* RW--F */
#define NV_CIO_CRE_PIXEL_INDEX                           0x00000028 /*       */
#define NV_CIO_CRE_PIXEL_FORMAT                                 1:0 /* RW--F */
#define NV_CIO_CRE_PIXEL_FORMAT_VGA                      0x00000000 /* RW--V */
#define NV_CIO_CRE_PIXEL_FORMAT_8BPP                     0x00000001 /* RW--V */
#define NV_CIO_CRE_PIXEL_FORMAT_16BPP                    0x00000002 /* RW--V */
#define NV_CIO_CRE_PIXEL_FORMAT_32BPP                    0x00000003 /* RW--V */
#define NV_CIO_CRE_PIXEL_TILING                                 2:2 /* RW--F */
#define NV_CIO_CRE_DEC__INDEX                            0x00000029 /*       */
#define NV_CIO_CRE_OSCOL__INDEX                          0x0000002A /*       */
#define NV_CIO_CRE_SCRATCH0__INDEX                       0x0000002B /*       */
#define NV_CIO_CRE_SCRATCH1__INDEX                       0x0000002C /*       */
#define NV_CIO_CRE_HEB__INDEX                            0x0000002D /*       */
#define NV_CIO_CRE_HEB_ILC_8                                    4:4 /* RW--F */
#define NV_CIO_CRE_HEB_HRS_8                                    3:3 /* RW--F */
#define NV_CIO_CRE_HEB_HBS_8                                    2:2 /* RW--F */
#define NV_CIO_CRE_HEB_HDE_8                                    1:1 /* RW--F */
#define NV_CIO_CRE_HEB_HDT_8                                    0:0 /* RW--F */
#define NV_CIO_CRE_HCUR_ADDR0_INDEX                      0x00000030 /*       */
#define NV_CIO_CRE_HCUR_ADDR0_ADR                               6:0 /* RW--F */
#define NV_CIO_CRE_HCUR_ADDR1_INDEX                      0x00000031 /*       */
#define NV_CIO_CRE_HCUR_ADDR1_ADR                               7:3 /* RW--F */
#define NV_CIO_CRE_HCUR_ADDR1_CUR_DBL                           1:1 /* RW--F */
#define NV_CIO_CRE_HCUR_ADDR1_ENABLE                            0:0 /* RW--F */
#define NV_CIO_CRE_VID_END0__INDEX                       0x00000032 /*       */
#define NV_CIO_CRE_VID_END_7_0                                  7:0 /* RW--F */
#define NV_CIO_CRE_VID_END1__INDEX                       0x00000033 /*       */
#define NV_CIO_CRE_VID_END_ENABLE                               4:4 /* RW--F */
#define NV_CIO_CRE_VID_END_10_8                                 2:0 /* RW--F */
#define NV_CIO_CRE_RL0__INDEX                            0x00000034 /*       */
#define NV_CIO_CRE_RL1__INDEX                            0x00000035 /*       */
#define NV_CIO_CRE_RMA__INDEX                            0x00000038 /*       */
#define NV_CIO_CRE_ILACE__INDEX                          0x00000039 /*       */
#define NV_CIO_CRE_TREG__INDEX                           0x0000003D /*       */
#define NV_CIO_CRE_TREG_HCNT                                    6:6 /* RW--F */
#define NV_CIO_CRE_TREG_VCNT                                    4:4 /* RW--F */
#define NV_CIO_CRE_TREG_HCNT_INDEX                       0x00000000 /*       */
#define NV_CIO_CRE_TREG_VCNTA_INDEX                      0x00000006 /*       */
#define NV_CIO_CRE_TREG_VCNTB_INDEX                      0x00000007 /*       */
#define NV_CIO_CRE_DDC_STATUS__INDEX                     0x0000003E /*       */
#define NV_CIO_CRE_DDC_WR__INDEX                         0x0000003F /*       */
/* vga.ref */
#define NV_PRMVIO                             0x000C7FFF:0x000C0000 /* RW--D */
#define NV_PRMVIO_MBEN                                   0x000C0094 /* RW-1R */
#define NV_PRMVIO_ADDEN                                  0x000C46e8 /* RW-1R */
#define NV_PRMVIO_VSE1                                   0x000C0102 /* RW-1R */
#define NV_PRMVIO_VSE2                                   0x000C03c3 /* RW-1R */
#define NV_PRMVIO_MISC__READ                             0x000C03cc /* R--1R */
#define NV_PRMVIO_MISC__WRITE                            0x000C03c2 /* -W-1R */
#define NV_PRMVIO_SRX                                    0x000C03c4 /* RW-1R */
#define NV_PRMVIO_SR_RESET                               0x000C03c5 /* RW-1R */
#define NV_PRMVIO_SR_RESET_INDEX                         0x00000000 /*       */
#define NV_PRMVIO_SR_CLOCK                               0x000C03c5 /* RW-1R */
#define NV_PRMVIO_SR_CLOCK_INDEX                         0x00000001 /*       */
#define NV_PRMVIO_SR_PLANE_MASK                          0x000C03c5 /* RW-1R */
#define NV_PRMVIO_SR_PLANE_MASK_INDEX                    0x00000002 /*       */
#define NV_PRMVIO_SR_CHAR_MAP                            0x000C03c5 /* RW-1R */
#define NV_PRMVIO_SR_CHAR_MAP_INDEX                      0x00000003 /*       */
#define NV_PRMVIO_SR_MEM_MODE                            0x000C03c5 /* RW-1R */
#define NV_PRMVIO_SR_MEM_MODE_INDEX                      0x00000004 /*       */
#define NV_PRMVIO_SR_LOCK                                0x000C03c5 /* RW-1R */
#define NV_PRMVIO_SR_LOCK_INDEX                          0x00000006 /*       */
#define NV_PRMVIO_GRX                                    0x000C03ce /* RW-1R */
#define NV_PRMVIO_GX_SR                                  0x000C03cf /* RW-1R */
#define NV_PRMVIO_GX_SR_INDEX                            0x00000000 /*       */
#define NV_PRMVIO_GX_SREN                                0x000C03cf /* RW-1R */
#define NV_PRMVIO_GX_SREN_INDEX                          0x00000001 /*       */
#define NV_PRMVIO_GX_CCOMP                               0x000C03cf /* RW-1R */
#define NV_PRMVIO_GX_CCOMP_INDEX                         0x00000002 /*       */
#define NV_PRMVIO_GX_ROP                                 0x000C03cf /* RW-1R */
#define NV_PRMVIO_GX_ROP_INDEX                           0x00000003 /*       */
#define NV_PRMVIO_GX_READ_MAP                            0x000C03cf /* RW-1R */
#define NV_PRMVIO_GX_READ_MAP_INDEX                      0x00000004 /*       */
#define NV_PRMVIO_GX_MODE                                0x000C03cf /* RW-1R */
#define NV_PRMVIO_GX_MODE_INDEX                          0x00000005 /*       */
#define NV_PRMVIO_GX_MISC                                0x000C03cf /* RW-1R */
#define NV_PRMVIO_GX_MISC_INDEX                          0x00000006 /*       */
#define NV_PRMVIO_GX_DONT_CARE                           0x000C03cf /* RW-1R */
#define NV_PRMVIO_GX_DONT_CARE_INDEX                     0x00000007 /*       */
#define NV_PRMVIO_GX_BIT_MASK                            0x000C03cf /* RW-1R */
#define NV_PRMVIO_GX_BIT_MASK_INDEX                      0x00000008 /*       */
/* vga.ref */
#define NV_PRMVGA                             0x000BFFFF:0x000A0000 /* RW--D */
/* dev_media.ref */
#define NV_PME                                0x00200FFF:0x00200000 /* RW--D */
#define NV_PME_DEBUG_0                                   0x00200080 /* RWI4R */
#define NV_PME_DEBUG_0_DET_FIELD_SWITCH                         0:0 /* RWI-F */
#define NV_PME_DEBUG_0_DET_FIELD_SWITCH_DISABLED         0x00000000 /* RWI-V */
#define NV_PME_DEBUG_0_DET_FIELD_SWITCH_ENABLED          0x00000001 /* RW--V */
#define NV_PME_DEBUG_1                                   0x00200084 /* RWI4R */
#define NV_PME_DEBUG_1_SEL                                      1:0 /* RWI-F */
#define NV_PME_DEBUG_1_SEL_VIPCLK                        0x00000000 /* RWI-V */
#define NV_PME_DEBUG_1_SEL_MCLK                          0x00000001 /* RW--V */
#define NV_PME_DEBUG_1_SEL_GLOB                          0x00000002 /* RW--V */
#define NV_PME_DEBUG_1_VIPCLK_SEL                               6:4 /* RWI-F */
#define NV_PME_DEBUG_1_VIPCLK_SEL_DEFAULT                0x00000000 /* RWI-V */
#define NV_PME_DEBUG_1_MCLK_SEL                                 9:8 /* RWI-F */
#define NV_PME_DEBUG_1_MCLK_SEL_DEFAULT                  0x00000000 /* RWI-V */
#define NV_PME_INTR_0                                    0x00200100 /* RWI4R */
#define NV_PME_INTR_0_NOTIFY                                    0:0 /* RWIVF */
#define NV_PME_INTR_0_NOTIFY_NOT_PENDING                 0x00000000 /* R-I-V */
#define NV_PME_INTR_0_NOTIFY_PENDING                     0x00000001 /* R---V */
#define NV_PME_INTR_0_NOTIFY_RESET                       0x00000001 /* -W--V */
#define NV_PME_INTR_0_VMI                                       4:4 /* RWIVF */
#define NV_PME_INTR_0_VMI_NOT_PENDING                    0x00000000 /* R-I-V */
#define NV_PME_INTR_0_VMI_PENDING                        0x00000001 /* R---V */
#define NV_PME_INTR_0_VMI_RESET                          0x00000001 /* -W--V */
#define NV_PME_INTR_EN_0                                 0x00200140 /* RWI4R */
#define NV_PME_INTR_EN_0_NOTIFY                                 0:0 /* RWIVF */
#define NV_PME_INTR_EN_0_NOTIFY_DISABLED                 0x00000000 /* RWI-V */
#define NV_PME_INTR_EN_0_NOTIFY_ENABLED                  0x00000001 /* RW--V */
#define NV_PME_INTR_EN_0_VMI                                    4:4 /* RWIVF */
#define NV_PME_INTR_EN_0_VMI_DISABLED                    0x00000000 /* RWI-V */
#define NV_PME_INTR_EN_0_VMI_ENABLED                     0x00000001 /* RW--V */
#define NV_PME_CONFIG_0                                  0x00200200 /* RWI4R */
#define NV_PME_CONFIG_0_CCIR656                                 0:0 /* RWIVF */
#define NV_PME_CONFIG_0_CCIR656_DISABLED                 0x00000000 /* RWI-V */
#define NV_PME_CONFIG_0_CCIR656_ENABLED                  0x00000001 /* RW--V */
#define NV_PME_CONFIG_0_VMI                                     4:4 /* RWIVF */
#define NV_PME_CONFIG_0_VMI_DISABLED                     0x00000000 /* RWI-V */
#define NV_PME_CONFIG_0_VMI_ENABLED                      0x00000001 /* RW--V */
#define NV_PME_CONFIG_0_VBI_MODE                                9:8 /* RWIVF */
#define NV_PME_CONFIG_0_VBI_MODE_DISABLED                0x00000000 /* RWI-V */
#define NV_PME_CONFIG_0_VBI_MODE_1                       0x00000001 /* RW--V */
#define NV_PME_CONFIG_0_VBI_MODE_2                       0x00000002 /* RW--V */
#define NV_PME_CONFIG_0_VID_CD                                12:12 /* RWIVF */
#define NV_PME_CONFIG_0_VID_CD_DISABLED                  0x00000000 /* RWI-V */
#define NV_PME_CONFIG_0_VID_CD_ENABLED                   0x00000001 /* RW--V */
#define NV_PME_CONFIG_0_AUD_CD                                16:16 /* RWIVF */
#define NV_PME_CONFIG_0_AUD_CD_DISABLED                  0x00000000 /* RWI-V */
#define NV_PME_CONFIG_0_AUD_CD_ENABLED                   0x00000001 /* RW--V */
#define NV_PME_CONFIG_1                                  0x00200204 /* RWI4R */
#define NV_PME_CONFIG_1_BUFFS                                   0:0 /* RWIVF */
#define NV_PME_CONFIG_1_BUFFS_PNVM                       0x00000000 /* RWI-V */
#define NV_PME_CONFIG_1_BUFFS_SYS                        0x00000001 /* RW--V */
#define NV_PME_CONFIG_1_HOST                                    4:4 /* RWIVF */
#define NV_PME_CONFIG_1_HOST_PCI                         0x00000000 /* RWI-V */
#define NV_PME_CONFIG_1_HOST_AGP                         0x00000001 /* RW--V */
#define NV_PME_VID_BUFF0_START_SYS                       0x00200300 /* RWI4R */
#define NV_PME_VID_BUFF0_START_SYS_ADDRESS                     31:4 /* RWXUF */
#define NV_PME_VID_BUFF1_START_SYS                       0x00200304 /* RWI4R */
#define NV_PME_VID_BUFF1_START_SYS_ADDRESS                     31:4 /* RWXUF */
#define NV_PME_VID_BUFF0_START_PNVM                      0x00200308 /* RWI4R */
#define NV_PME_VID_BUFF0_START_PNVM_ADDRESS                    22:4 /* RWXUF */
#define NV_PME_VID_BUFF1_START_PNVM                      0x0020030c /* RWI4R */
#define NV_PME_VID_BUFF1_START_PNVM_ADDRESS                    22:4 /* RWXUF */
#define NV_PME_VID_BUFF0_LENGTH                          0x00200310 /* RWI4R */
#define NV_PME_VID_BUFF0_LENGTH_BITS                          15:12 /* RWXUF */
#define NV_PME_VID_BUFF1_LENGTH                          0x00200314 /* RWI4R */
#define NV_PME_VID_BUFF1_LENGTH_BITS                          15:12 /* RWXUF */
#define NV_PME_VID_ME_STATE                              0x00200318 /* RW-4R */
#define NV_PME_VID_ME_STATE_BUFF0_INTR_NOTIFY                   0:0 /* RWIVF */
#define NV_PME_VID_ME_STATE_BUFF1_INTR_NOTIFY                   4:4 /* RWXVF */
#define NV_PME_VID_ME_STATE_BUFF0_INTR_CHAINGAP                 8:8 /* RWXVF */
#define NV_PME_VID_ME_STATE_BUFF1_INTR_CHAINGAP               12:12 /* RWXVF */
#define NV_PME_VID_ME_STATE_BUFF0_IN_USE                      16:16 /* RWXVF */
#define NV_PME_VID_ME_STATE_BUFF1_IN_USE                      20:20 /* RWXVF */
#define NV_PME_VID_ME_STATE_CURRENT_BUFFER                    24:24 /* RWXVF */
#define NV_PME_VID_ME_STATE_CURRENT_BUFFER_0             0x00000000 /* RW--V */
#define NV_PME_VID_ME_STATE_CURRENT_BUFFER_1             0x00000001 /* RW--V */
#define NV_PME_VID_SU_STATE                              0x0020031c /* RW-4R */
#define NV_PME_VID_SU_STATE_BUFF0_IN_USE                      16:16 /* RWXVF */
#define NV_PME_VID_SU_STATE_BUFF1_IN_USE                      20:20 /* RWXVF */
#define NV_PME_VID_RM_STATE                              0x00200320 /* RW-4R */
#define NV_PME_VID_RM_STATE_BUFF0_INTR_NOTIFY                   0:0 /* RWXVF */
#define NV_PME_VID_RM_STATE_BUFF1_INTR_NOTIFY                   4:4 /* RWXVF */
#define NV_PME_VID_RM_STATE_BUFF0_INTR_CHAINGAP                 8:8 /* RWXVF */
#define NV_PME_VID_RM_STATE_BUFF1_INTR_CHAINGAP               12:12 /* RWXVF */
#define NV_PME_VID_CURRENT                               0x00200324 /* RWI4R */
#define NV_PME_VID_CURRENT_POS                                 15:2 /* RWXUF */
#define NV_PME_AUD_BUFF0_START_SYS                       0x00200340 /* RWI4R */
#define NV_PME_AUD_BUFF0_START_SYS_ADDRESS                     31:4 /* RWXUF */
#define NV_PME_AUD_BUFF1_START_SYS                       0x00200344 /* RWI4R */
#define NV_PME_AUD_BUFF1_START_SYS_ADDRESS                     31:4 /* RWXUF */
#define NV_PME_AUD_BUFF0_START_PNVM                      0x00200348 /* RWI4R */
#define NV_PME_AUD_BUFF0_START_PNVM_ADDRESS                    22:4 /* RWXUF */
#define NV_PME_AUD_BUFF1_START_PNVM                      0x0020034c /* RWI4R */
#define NV_PME_AUD_BUFF1_START_PNVM_ADDRESS                    22:4 /* RWXUF */
#define NV_PME_AUD_BUFF0_LENGTH                          0x00200350 /* RWI4R */
#define NV_PME_AUD_BUFF0_LENGTH_BITS                          12:10 /* RWXUF */
#define NV_PME_AUD_BUFF1_LENGTH                          0x00200354 /* RWI4R */
#define NV_PME_AUD_BUFF1_LENGTH_BITS                          12:10 /* RWXUF */
#define NV_PME_AUD_ME_STATE                              0x00200358 /* RW-4R */
#define NV_PME_AUD_ME_STATE_BUFF0_INTR_NOTIFY                   0:0 /* RWIVF */
#define NV_PME_AUD_ME_STATE_BUFF1_INTR_NOTIFY                   4:4 /* RWXVF */
#define NV_PME_AUD_ME_STATE_BUFF0_INTR_CHAINGAP                 8:8 /* RWXVF */
#define NV_PME_AUD_ME_STATE_BUFF1_INTR_CHAINGAP               12:12 /* RWXVF */
#define NV_PME_AUD_ME_STATE_BUFF0_IN_USE                      16:16 /* RWXVF */
#define NV_PME_AUD_ME_STATE_BUFF1_IN_USE                      20:20 /* RWXVF */
#define NV_PME_AUD_ME_STATE_CURRENT_BUFFER                    24:24 /* RWXVF */
#define NV_PME_AUD_ME_STATE_CURRENT_BUFFER_0             0x00000000 /* RW--V */
#define NV_PME_AUD_ME_STATE_CURRENT_BUFFER_1             0x00000001 /* RW--V */
#define NV_PME_AUD_SU_STATE                              0x0020035c /* RW-4R */
#define NV_PME_AUD_SU_STATE_BUFF0_IN_USE                      16:16 /* RWXVF */
#define NV_PME_AUD_SU_STATE_BUFF1_IN_USE                      20:20 /* RWXVF */
#define NV_PME_AUD_RM_STATE                              0x00200360 /* RW-4R */
#define NV_PME_AUD_RM_STATE_BUFF0_INTR_NOTIFY                   0:0 /* RWXVF */
#define NV_PME_AUD_RM_STATE_BUFF1_INTR_NOTIFY                   4:4 /* RWXVF */
#define NV_PME_AUD_RM_STATE_BUFF0_INTR_CHAINGAP                 8:8 /* RWXVF */
#define NV_PME_AUD_RM_STATE_BUFF1_INTR_CHAINGAP               12:12 /* RWXVF */
#define NV_PME_AUD_CURRENT                               0x00200364 /* RWI4R */
#define NV_PME_AUD_CURRENT_POS                                 12:2 /* RWXUF */
#define NV_PME_VBI_BUFF0_START                           0x00200380 /* RWI4R */
#define NV_PME_VBI_BUFF0_START_ADDRESS                         22:4 /* RWXUF */
#define NV_PME_VBI_BUFF1_START                           0x00200384 /* RWI4R */
#define NV_PME_VBI_BUFF1_START_ADDRESS                         22:4 /* RWXUF */
#define NV_PME_VBI_BUFF0_PITCH                           0x00200388 /* RWI4R */
#define NV_PME_VBI_BUFF0_PITCH_VALUE                           13:4 /* RWXUF */
#define NV_PME_VBI_BUFF1_PITCH                           0x0020038c /* RWI4R */
#define NV_PME_VBI_BUFF1_PITCH_VALUE                           13:4 /* RWXUF */
#define NV_PME_VBI_BUFF0_LENGTH                          0x00200390 /* RWI4R */
#define NV_PME_VBI_BUFF0_LENGTH_BITS                           19:4 /* RWXUF */
#define NV_PME_VBI_BUFF1_LENGTH                          0x00200394 /* RWI4R */
#define NV_PME_VBI_BUFF1_LENGTH_BITS                           19:4 /* RWXUF */
#define NV_PME_VBI_ME_STATE                              0x00200398 /* RW-4R */
#define NV_PME_VBI_ME_STATE_BUFF0_INTR_NOTIFY                   0:0 /* RWXVF */
#define NV_PME_VBI_ME_STATE_BUFF1_INTR_NOTIFY                   4:4 /* RWXVF */
#define NV_PME_VBI_ME_STATE_BUFF0_ERROR_CODE                   10:8 /* RWXVF */
#define NV_PME_VBI_ME_STATE_BUFF1_ERROR_CODE                  14:12 /* RWXVF */
#define NV_PME_VBI_ME_STATE_BUFF0_IN_USE                      16:16 /* RWXVF */
#define NV_PME_VBI_ME_STATE_BUFF1_IN_USE                      20:20 /* RWXVF */
#define NV_PME_VBI_ME_STATE_CURRENT_BUFFER                    24:24 /* RWXVF */
#define NV_PME_VBI_ME_STATE_CURRENT_BUFFER_0             0x00000000 /* RW--V */
#define NV_PME_VBI_ME_STATE_CURRENT_BUFFER_1             0x00000001 /* RW--V */
#define NV_PME_VBI_SU_STATE                              0x0020039c /* RW-4R */
#define NV_PME_VBI_SU_STATE_BUFF0_FIELD                         8:8 /* RWXVF */
#define NV_PME_VBI_SU_STATE_BUFF1_FIELD                       12:12 /* RWXVF */
#define NV_PME_VBI_SU_STATE_BUFF0_IN_USE                      16:16 /* RWXVF */
#define NV_PME_VBI_SU_STATE_BUFF1_IN_USE                      20:20 /* RWXVF */
#define NV_PME_VBI_RM_STATE                              0x002003a0 /* RW-4R */
#define NV_PME_VBI_RM_STATE_BUFF0_INTR_NOTIFY                   0:0 /* RWXVF */
#define NV_PME_VBI_RM_STATE_BUFF1_INTR_NOTIFY                   4:4 /* RWXVF */
#define NV_PME_VBI                                       0x002003a4 /* RWI4R */
#define NV_PME_VBI_START_LINE                                   4:0 /* RWX-F */
#define NV_PME_VBI_NUM_LINES                                  20:16 /* RWX-F */
#define NV_PME_IMAGE_BUFF0_START                         0x00200400 /* RWI4R */
#define NV_PME_IMAGE_BUFF0_START_ADDRESS                       22:4 /* RWXUF */
#define NV_PME_IMAGE_BUFF1_START                         0x00200404 /* RWI4R */
#define NV_PME_IMAGE_BUFF1_START_ADDRESS                       22:4 /* RWXUF */
#define NV_PME_IMAGE_BUFF0_PITCH                         0x00200408 /* RWI4R */
#define NV_PME_IMAGE_BUFF0_PITCH_VALUE                         13:4 /* RWXUF */
#define NV_PME_IMAGE_BUFF1_PITCH                         0x0020040c /* RWI4R */
#define NV_PME_IMAGE_BUFF1_PITCH_VALUE                         13:4 /* RWXUF */
#define NV_PME_IMAGE_BUFF0_LENGTH                        0x00200410 /* RWI4R */
#define NV_PME_IMAGE_BUFF0_LENGTH_BITS                         19:4 /* RWXUF */
#define NV_PME_IMAGE_BUFF1_LENGTH                        0x00200414 /* RWI4R */
#define NV_PME_IMAGE_BUFF1_LENGTH_BITS                         19:4 /* RWXUF */
#define NV_PME_IMAGE_ME_STATE                            0x00200418 /* RW-4R */
#define NV_PME_IMAGE_ME_STATE_BUFF0_INTR_NOTIFY                 0:0 /* RWXVF */
#define NV_PME_IMAGE_ME_STATE_BUFF1_INTR_NOTIFY                 4:4 /* RWXVF */
#define NV_PME_IMAGE_ME_STATE_BUFF0_ERROR_CODE                 10:8 /* RWXVF */
#define NV_PME_IMAGE_ME_STATE_BUFF1_ERROR_CODE                14:12 /* RWXVF */
#define NV_PME_IMAGE_ME_STATE_BUFF0_IN_USE                    16:16 /* RWXVF */
#define NV_PME_IMAGE_ME_STATE_BUFF1_IN_USE                    20:20 /* RWXVF */
#define NV_PME_IMAGE_ME_STATE_CURRENT_BUFFER                  24:24 /* RWXVF */
#define NV_PME_IMAGE_ME_STATE_CURRENT_BUFFER_0           0x00000000 /* RW--V */
#define NV_PME_IMAGE_ME_STATE_CURRENT_BUFFER_1           0x00000001 /* RW--V */
#define NV_PME_IMAGE_SU_STATE                            0x0020041c /* RW-4R */
#define NV_PME_IMAGE_SU_STATE_BUFF0_FIELD                       8:8 /* RWXVF */
#define NV_PME_IMAGE_SU_STATE_BUFF1_FIELD                     12:12 /* RWXVF */
#define NV_PME_IMAGE_SU_STATE_BUFF0_IN_USE                    16:16 /* RWXVF */
#define NV_PME_IMAGE_SU_STATE_BUFF1_IN_USE                    20:20 /* RWXVF */
#define NV_PME_IMAGE_RM_STATE                            0x00200420 /* RW-4R */
#define NV_PME_IMAGE_RM_STATE_BUFF0_INTR_NOTIFY                 0:0 /* RWXVF */
#define NV_PME_IMAGE_RM_STATE_BUFF1_INTR_NOTIFY                 4:4 /* RWXVF */
#define NV_PME_IMAGE_BUFF0_SCALE_INCR                    0x00200424 /* RW-4R */
#define NV_PME_IMAGE_BUFF0_SCALE_INCR_Y                       26:16 /* RWXVF */
#define NV_PME_IMAGE_BUFF0_SCALE_INCR_X                        10:0 /* RWXVF */
#define NV_PME_IMAGE_BUFF1_SCALE_INCR                    0x00200428 /* RW-4R */
#define NV_PME_IMAGE_BUFF1_SCALE_INCR_Y                       26:16 /* RWXVF */
#define NV_PME_IMAGE_BUFF1_SCALE_INCR_X                        10:0 /* RWXVF */
#define NV_PME_IMAGE_Y_CROP                              0x0020042c /* RW-4R */
#define NV_PME_IMAGE_Y_CROP_STARTLINE                           8:0 /* RWXVF */
#define NV_PME_FIFO_LINE_START                           0x00200480 /* R--4R */
#define NV_PME_FIFO_LINE_START_ADDRESS                         20:4 /* R-XVF */
#define NV_PME_FIFO_CURRENT                              0x00200484 /* RWI4R */
#define NV_PME_FIFO_CURRENT_ADDRESS                            20:2 /* RWXVF */
#define NV_PME_VMI_POLL                                  0x00200488 /* R--4R */
#define NV_PME_VMI_POLL_UNCD                                    0:0 /* R-IVF */
#define NV_PME_VMI_POLL_UNCD_NOT_PENDING                 0x00000000 /* R-IVF */
#define NV_PME_VMI_POLL_UNCD_PENDING                     0x00000001 /* R--VF */
#define NV_PME_VMI_POLL_VIDCD                                   1:1 /* R-IVF */
#define NV_PME_VMI_POLL_VIDCD_NOT_PENDING                0x00000000 /* R-IVF */
#define NV_PME_VMI_POLL_VIDCD_PENDING                    0x00000001 /* R--VF */
#define NV_PME_VMI_POLL_AUDCD                                   2:2 /* R-IVF */
#define NV_PME_VMI_POLL_AUDCD_NOT_PENDING                0x00000000 /* R-IVF */
#define NV_PME_VMI_POLL_AUDCD_PENDING                    0x00000001 /* R--VF */
#define NV_PME_VMI_POLL_INT                                     3:3 /* R-IVF */
#define NV_PME_VMI_POLL_INT_NOT_PENDING                  0x00000000 /* R-IVF */
#define NV_PME_VMI_POLL_INT_PENDING                      0x00000001 /* R--VF */
#define NV_PME_VMI_POLL_CPURDREC                                4:4 /* R-IVF */
#define NV_PME_VMI_POLL_CPURDREC_NOT_PENDING             0x00000000 /* R-IVF */
#define NV_PME_VMI_POLL_CPURDREC_PENDING                 0x00000001 /* R--VF */
#define NV_PME_EXTERNAL(i)                       (0x00200600+(i)*4) /* RW-4A */
#define NV_PME_EXTERNAL_SIZE_1                                  256 /*       */
#define NV_PME_EXTERNAL_DATA                                    7:0 /* RWXVF */
/* usr_beta_solid.ref */
#define NV_UBETA                              0x00411FFF:0x00410000 /* -W--D */
#define NV_UBETA_CTX_SWITCH                              0x00410000 /* -W-4R */
#define NV_UBETA_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
#define NV_UBETA_CTX_SWITCH_CHID                              22:16 /* -W-UF */
#define NV_UBETA_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
#define NV_UBETA_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
#define NV_UBETA_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
#define NV_UBETA_SET_NOTIFY                              0x00410104 /* -W-4R */
#define NV_UBETA_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
#define NV_UBETA_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
#define NV_UBETA_SET_BETA1D31                            0x00410300 /* -W-4R */
#define NV_UBETA_SET_BETA1D31_VALUE_FRACTION                  30:21 /* -W-UF */
#define NV_UBETA_SET_BETA1D31_VALUE                           31:31 /* -W-SF */
/* usr_rop_solid.ref */
#define NV_UROP                               0x00421FFF:0x00420000 /* -W--D */
#define NV_UROP_CTX_SWITCH                               0x00420000 /* -W-4R */
#define NV_UROP_CTX_SWITCH_INSTANCE                            15:0 /* -W-UF */
#define NV_UROP_CTX_SWITCH_CHID                               22:16 /* -W-UF */
#define NV_UROP_CTX_SWITCH_VOLATILE                           31:31 /* -W-VF */
#define NV_UROP_CTX_SWITCH_VOLATILE_IGNORE               0x00000000 /* -W--V */
#define NV_UROP_CTX_SWITCH_VOLATILE_RESET                0x00000001 /* -W--V */
#define NV_UROP_SET_NOTIFY                               0x00420104 /* -W-4R */
#define NV_UROP_SET_NOTIFY_PARAMETER                           31:0 /* -W-VF */
#define NV_UROP_SET_NOTIFY_PARAMETER_WRITE               0x00000000 /* -W--V */
#define NV_UROP_SET_ROP                                  0x00420300 /* -W-4R */
#define NV_UROP_SET_ROP_VALUE                                   7:0 /* -W-VF */
/* usr_color_key.ref */
#define NV_UCHROMA                            0x00431FFF:0x00430000 /* -W--D */
#define NV_UCHROMA_CTX_SWITCH                            0x00430000 /* -W-4R */
#define NV_UCHROMA_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
#define NV_UCHROMA_CTX_SWITCH_CHID                            22:16 /* -W-UF */
#define NV_UCHROMA_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
#define NV_UCHROMA_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
#define NV_UCHROMA_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
#define NV_UCHROMA_SET_NOTIFY                            0x00430104 /* -W-4R */
#define NV_UCHROMA_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
#define NV_UCHROMA_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
#define NV_UCHROMA_SET_COLOR                             0x00430304 /* -W-4R */
#define NV_UCHROMA_SET_COLOR_VALUE                             31:0 /* -W-VF */
/* usr_plane_switch.ref */
#define NV_UPLANE                             0x00441FFF:0x00440000 /* -W--D */
#define NV_UPLANE_CTX_SWITCH                             0x00440000 /* -W-4R */
#define NV_UPLANE_CTX_SWITCH_INSTANCE                          15:0 /* -W-UF */
#define NV_UPLANE_CTX_SWITCH_CHID                             22:16 /* -W-UF */
#define NV_UPLANE_CTX_SWITCH_VOLATILE                         31:31 /* -W-VF */
#define NV_UPLANE_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* -W--V */
#define NV_UPLANE_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
#define NV_UPLANE_SET_NOTIFY                             0x00440104 /* -W-4R */
#define NV_UPLANE_SET_NOTIFY_PARAMETER                         31:0 /* -W-VF */
#define NV_UPLANE_SET_NOTIFY_PARAMETER_WRITE             0x00000000 /* -W--V */
#define NV_UPLANE_SET_COLOR                              0x00440304 /* -W-4R */
#define NV_UPLANE_SET_COLOR_VALUE                              31:0 /* -W-VF */
/* usr_clipping.ref */
#define NV_UCLIP                              0x00451FFF:0x00450000 /* -W--D */
#define NV_UCLIP_CTX_SWITCH                              0x00450000 /* -W-4R */
#define NV_UCLIP_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
#define NV_UCLIP_CTX_SWITCH_CHID                              22:16 /* -W-UF */
#define NV_UCLIP_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
#define NV_UCLIP_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
#define NV_UCLIP_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
#define NV_UCLIP_SET_NOTIFY                              0x00450104 /* -W-4R */
#define NV_UCLIP_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
#define NV_UCLIP_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
#define NV_UCLIP_SET_RECT_0                              0x00450300 /* -W-4R */
#define NV_UCLIP_SET_RECT_0_X                                  15:0 /* -W-SF */
#define NV_UCLIP_SET_RECT_0_Y                                 31:16 /* -W-SF */
#define NV_UCLIP_SET_RECT_1                              0x00450304 /* -W-4R */
#define NV_UCLIP_SET_RECT_1_WIDTH                              15:0 /* -W-UF */
#define NV_UCLIP_SET_RECT_1_HEIGHT                            31:16 /* -W-UF */
/* usr_d3d0_triangle_zeta.ref */
#define NV_UD3D0Z                             0x00571FFF:0x00570000 /* -W--D */
#define NV_UD3D0Z_CTX_SWITCH                             0x00570000 /* -W-4R */
#define NV_UD3D0Z_CTX_SWITCH_INSTANCE                          15:0 /* -W-UF */
#define NV_UD3D0Z_CTX_SWITCH_CHID                             22:16 /* -W-UF */
#define NV_UD3D0Z_CTX_SWITCH_VOLATILE                         31:31 /* -W-VF */
#define NV_UD3D0Z_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* -W--V */
#define NV_UD3D0Z_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
#define NV_UD3D0Z_SET_NOTIFY                             0x00570104 /* -W-4R */
#define NV_UD3D0Z_SET_NOTIFY_PARAMETER                         31:0 /* -W-VF */
#define NV_UD3D0Z_SET_NOTIFY_PARAMETER_WRITE             0x00000000 /* -W--V */
#define NV_UD3D0Z_TEXTURE_OFFSET                         0x00570304 /* -W-4R */
#define NV_UD3D0Z_TEXTURE_OFFSET_VALUE                         31:0 /* -W-UF */
#define NV_UD3D0Z_TEXTURE_FORMAT                         0x00570308 /* -W-4R */
#define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_KEY_COLOR_MASK          15:0 /* -W-UF */
#define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_KEY                    16:16 /* -W-UF */
#define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_KEY_DISABLED      0x00000000 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_KEY_ENABLED       0x00000001 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT                 21:20 /* -W-UF */
#define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT_A1R5G5B5   0x00000000 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT_X1R5G5B5   0x00000001 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT_A4R4G4B4   0x00000002 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_COLOR_FORMAT_R5G6B5     0x00000003 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN                     27:24 /* -W-UF */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_1X1            0x00000000 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_2X2            0x00000001 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_4X4            0x00000002 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_8X8            0x00000003 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_16X16          0x00000004 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_32X32          0x00000005 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_64X64          0x00000006 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_128X128        0x00000007 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_256X256        0x00000008 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_512X512        0x00000009 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_1024X1024      0x0000000a /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MIN_2048X2048      0x0000000b /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX                     31:28 /* -W-UF */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_1X1            0x00000000 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_2X2            0x00000001 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_4X4            0x00000002 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_8X8            0x00000003 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_16X16          0x00000004 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_32X32          0x00000005 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_64X64          0x00000006 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_128X128        0x00000007 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_256X256        0x00000008 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_512X512        0x00000009 /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_1024X1024      0x0000000a /* -W-UV */
#define NV_UD3D0Z_TEXTURE_FORMAT_SIZE_MAX_2048X2048      0x0000000b /* -W-UV */
#define NV_UD3D0Z_FILTER                                 0x0057030C /* -W-4R */
#define NV_UD3D0Z_FILTER_SPREADX                                7:0 /* -W-UF */
#define NV_UD3D0Z_FILTER_SPREADY                               15:8 /* -W-UF */
#define NV_UD3D0Z_FILTER_MIPMAP                               23:16 /* -W-SF */
#define NV_UD3D0Z_FILTER_TURBO                                31:24 /* -W-SF */
#define NV_UD3D0Z_FOG_COLOR                              0x00570310 /* -W-4R */
#define NV_UD3D0Z_FOG_COLOR_BLU                                 7:0 /* -W-UF */
#define NV_UD3D0Z_FOG_COLOR_GRN                                15:8 /* -W-UF */
#define NV_UD3D0Z_FOG_COLOR_RED                               23:16 /* -W-UF */
#define NV_UD3D0Z_FOG_COLOR_VALUE                              31:0 /* -W-UF */
#define NV_UD3D0Z_CONTROL_OUT                            0x00570314 /* -W-4R */
#define NV_UD3D0Z_CONTROL_OUT_INTERPOLATOR                      1:0 /* -W-UF */
#define NV_UD3D0Z_CONTROL_OUT_INTERPOLATOR_ZOH           0x00000000 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_INTERPOLATOR_ZOH_MS        0x00000001 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_INTERPOLATOR_FOH           0x00000002 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_WRAP_U                            5:4 /* -W-UF */
#define NV_UD3D0Z_CONTROL_OUT_WRAP_U_CYLINDRICAL         0x00000000 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_WRAP_U_WRAP                0x00000001 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_WRAP_U_MIRROR              0x00000002 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_WRAP_U_CLAMP               0x00000003 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_WRAP_V                            7:6 /* -W-UF */
#define NV_UD3D0Z_CONTROL_OUT_WRAP_V_CYLINDRICAL         0x00000000 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_WRAP_V_WRAP                0x00000001 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_WRAP_V_MIRROR              0x00000002 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_WRAP_V_CLAMP               0x00000003 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_COLOR_FORMAT                      8:8 /* -W-UF */
#define NV_UD3D0Z_CONTROL_OUT_COLOR_FORMAT_LE_X8R8G8B8   0x00000000 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_COLOR_FORMAT_LE_A8R8G8B8   0x00000001 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_CULLING                         13:12 /* -W-UF */
#define NV_UD3D0Z_CONTROL_OUT_CULLING_NONE               0x00000001 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_CULLING_CLOCKWISE          0x00000002 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_CULLING_COUNTERCLOCKWISE   0x00000003 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZBUFFER                         15:15 /* -W-UF */
#define NV_UD3D0Z_CONTROL_OUT_ZBUFFER_SCREEN             0x00000000 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZBUFFER_LINEAR             0x00000001 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE                    19:16 /* -W-UF */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_FALSE         0x00000001 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_LT            0x00000002 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_EQ            0x00000003 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_LE            0x00000004 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_GT            0x00000005 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_NE            0x00000006 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_GE            0x00000007 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_COMPARE_TRUE          0x00000008 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE                      22:20 /* -W-UF */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_NEVER           0x00000000 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_ALPHA           0x00000001 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_ALPHA_ZETA      0x00000002 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_ZETA            0x00000003 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ZETA_WRITE_ALWAYS          0x00000004 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_COLOR_WRITE                     25:24 /* -W-UF */
#define NV_UD3D0Z_CONTROL_OUT_COLOR_WRITE_NEVER          0x00000000 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_COLOR_WRITE_ALPHA          0x00000001 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_COLOR_WRITE_ALPHA_ZETA     0x00000002 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ROP                             28:28 /* -W-UF */
#define NV_UD3D0Z_CONTROL_OUT_ROP_BLEND_AND              0x00000000 /* -W-UV */
#define NV_UD3D0Z_CONTROL_OUT_ROP_ADD_WITH_SATURATION    0x00000001 /* -W-UV */
#define NV_UD3D0Z_SPECULAR(i)                   (0x00571000+(i)*32) /* -W-4A */
#define NV_UD3D0Z_SPECULAR__SIZE_1                              128 /*       */
#define NV_UD3D0Z_SPECULAR_I0                                   3:0 /* -W-UF */
#define NV_UD3D0Z_SPECULAR_I1                                   7:4 /* -W-UF */
#define NV_UD3D0Z_SPECULAR_I2                                  11:8 /* -W-UF */
#define NV_UD3D0Z_SPECULAR_I3                                 15:12 /* -W-UF */
#define NV_UD3D0Z_SPECULAR_I4                                 19:16 /* -W-UF */
#define NV_UD3D0Z_SPECULAR_I5                                 23:20 /* -W-UF */
#define NV_UD3D0Z_SPECULAR_FOG                                31:24 /* -W-UF */
#define NV_UD3D0Z_COLOR(i)                      (0x00571004+(i)*32) /* -W-4A */
#define NV_UD3D0Z_COLOR__SIZE_1                                 128 /*       */
#define NV_UD3D0Z_COLOR_B8                                      7:0 /* -W-UF */
#define NV_UD3D0Z_COLOR_G8                                     16:8 /* -W-UF */
#define NV_UD3D0Z_COLOR_R8                                    23:16 /* -W-UF */
#define NV_UD3D0Z_COLOR_A8                                    32:24 /* -W-UF */
#define NV_UD3D0Z_X(i)                          (0x00571008+(i)*32) /* -W-4A */
#define NV_UD3D0Z_X__SIZE_1                                     128 /*       */
#define NV_UD3D0Z_X_VALUE                                      31:0 /* -W-FF */
#define NV_UD3D0Z_Y(i)                          (0x0057100C+(i)*32) /* -W-4A */
#define NV_UD3D0Z_Y__SIZE_1                                     128 /*       */
#define NV_UD3D0Z_Y_VALUE                                      31:0 /* -W-FF */
#define NV_UD3D0Z_Z(i)                          (0x00571010+(i)*32) /* -W-4A */
#define NV_UD3D0Z_Z__SIZE_1                                     128 /*       */
#define NV_UD3D0Z_Z_VALUE                                      31:0 /* -W-FF */
#define NV_UD3D0Z_M(i)                          (0x00571014+(i)*32) /* -W-4A */
#define NV_UD3D0Z_M__SIZE_1                                     128 /*       */
#define NV_UD3D0Z_M_VALUE                                      31:0 /* -W-FF */
#define NV_UD3D0Z_U(i)                          (0x00571018+(i)*32) /* -W-4A */
#define NV_UD3D0Z_U__SIZE_1                                     128 /*       */
#define NV_UD3D0Z_U_VALUE                                      31:0 /* -W-FF */
#define NV_UD3D0Z_V(i)                          (0x0057101c+(i)*32) /* -W-4A */
#define NV_UD3D0Z_V__SIZE_1                                     128 /*       */
#define NV_UD3D0Z_V_VALUE                                      31:0 /* -W-FF */
/* usr_pattern.ref */
#define NV_UPATT                              0x00461FFF:0x00460000 /* -W--D */
#define NV_UPATT_CTX_SWITCH                              0x00460000 /* -W-4R */
#define NV_UPATT_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
#define NV_UPATT_CTX_SWITCH_CHID                              22:16 /* -W-UF */
#define NV_UPATT_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
#define NV_UPATT_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
#define NV_UPATT_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
#define NV_UPATT_SET_NOTIFY                              0x00460104 /* -W-4R */
#define NV_UPATT_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
#define NV_UPATT_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
#define NV_UPATT_SET_SHAPE                               0x00460308 /* -W-4R */
#define NV_UPATT_SET_SHAPE_VALUE                                1:0 /* -W-VF */
#define NV_UPATT_SET_SHAPE_VALUE_8X8                     0x00000000 /* -W--V */
#define NV_UPATT_SET_SHAPE_VALUE_64X1                    0x00000001 /* -W--V */
#define NV_UPATT_SET_SHAPE_VALUE_1X64                    0x00000002 /* -W--V */
#define NV_UPATT_SET_COLOR0                              0x00460310 /* -W-4R */
#define NV_UPATT_SET_COLOR0_VALUE                              31:0 /* -W-VF */
#define NV_UPATT_SET_COLOR1                              0x00460314 /* -W-4R */
#define NV_UPATT_SET_COLOR1_VALUE                              31:0 /* -W-VF */
#define NV_UPATT_SET_PATTERN(i)                  (0x00460318+(i)*4) /* -W-4A */
#define NV_UPATT_SET_PATTERN__SIZE_1                              2 /*       */
#define NV_UPATT_SET_PATTERN_BITMAP                            31:0 /* -W-VF */
/* usr_point.ref */
#define NV_UPOINT                             0x00481FFF:0x00480000 /* -W--D */
#define NV_UPOINT_CTX_SWITCH                             0x00480000 /* -W-4R */
#define NV_UPOINT_CTX_SWITCH_INSTANCE                          15:0 /* -W-UF */
#define NV_UPOINT_CTX_SWITCH_CHID                             22:16 /* -W-UF */
#define NV_UPOINT_CTX_SWITCH_VOLATILE                         31:31 /* -W-VF */
#define NV_UPOINT_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* -W--V */
#define NV_UPOINT_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
#define NV_UPOINT_SET_NOTIFY                             0x00480104 /* -W-4R */
#define NV_UPOINT_SET_NOTIFY_PARAMETER                         31:0 /* -W-VF */
#define NV_UPOINT_SET_NOTIFY_PARAMETER_WRITE             0x00000000 /* -W--V */
#define NV_UPOINT_COLOR                                  0x00480304 /* -W-4R */
#define NV_UPOINT_COLOR_VALUE                                  31:0 /* -W-VF */
#define NV_UPOINT_POINT(i)                       (0x00480400+(i)*4) /* -W-4A */
#define NV_UPOINT_POINT__SIZE_1                                  32 /*       */
#define NV_UPOINT_POINT_X                                      15:0 /* -W-SF */
#define NV_UPOINT_POINT_Y                                     31:16 /* -W-SF */
#define NV_UPOINT_POINT32_0(i)                   (0x00480480+(i)*8) /* -W-4A */
#define NV_UPOINT_POINT32_0__SIZE_1                              16 /*       */
#define NV_UPOINT_POINT32_0_X                                  31:0 /* -W-SF */
#define NV_UPOINT_POINT32_1(i)                   (0x00480484+(i)*8) /* -W-4A */
#define NV_UPOINT_POINT32_1__SIZE_1                              16 /*       */
#define NV_UPOINT_POINT32_1_Y                                  31:0 /* -W-SF */
#define NV_UPOINT_CPOINT_0(i)                    (0x00480500+(i)*8) /* -W-4A */
#define NV_UPOINT_CPOINT_0__SIZE_1                               16 /*       */
#define NV_UPOINT_CPOINT_0_COLOR                               31:0 /* -W-VF */
#define NV_UPOINT_CPOINT_1(i)                    (0x00480504+(i)*8) /* -W-4A */
#define NV_UPOINT_CPOINT_1__SIZE_1                               16 /*       */
#define NV_UPOINT_CPOINT_1_X                                   15:0 /* -W-SF */
#define NV_UPOINT_CPOINT_1_Y                                  31:16 /* -W-SF */
/* usr_pointz.ref */
#define NV_UPOINTZ                            0x00581FFF:0x00580000 /* -W--D */
#define NV_UPOINTZ_CTX_SWITCH                            0x00580000 /* -W-4R */
#define NV_UPOINTZ_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
#define NV_UPOINTZ_CTX_SWITCH_CHID                            22:16 /* -W-UF */
#define NV_UPOINTZ_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
#define NV_UPOINTZ_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
#define NV_UPOINTZ_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
#define NV_UPOINTZ_SET_NOTIFY                            0x00580104 /* -W-4R */
#define NV_UPOINTZ_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
#define NV_UPOINTZ_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
#define NV_UPOINTZ_CONTROL_OUT                           0x00580304 /* -W-4R */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE                   19:16 /* -W-UF */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_FALSE        0x00000001 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_LT           0x00000002 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_EQ           0x00000003 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_LE           0x00000004 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_GT           0x00000005 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_NE           0x00000006 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_GE           0x00000007 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_COMPARE_TRUE         0x00000008 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE                     22:20 /* -W-UF */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_NEVER          0x00000000 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_ALPHA          0x00000001 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_ALPHA_ZETA     0x00000002 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_ZETA           0x00000003 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ZETA_WRITE_ALWAYS         0x00000004 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_COLOR_WRITE                    25:24 /* -W-UF */
#define NV_UPOINTZ_CONTROL_OUT_COLOR_WRITE_NEVER         0x00000000 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_COLOR_WRITE_ALPHA         0x00000001 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_COLOR_WRITE_ALPHA_ZETA    0x00000002 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ROP                            28:28 /* -W-UF */
#define NV_UPOINTZ_CONTROL_OUT_ROP_BLEND_AND             0x00000000 /* -W-UV */
#define NV_UPOINTZ_CONTROL_OUT_ROP_ADD_WITH_SATURATION   0x00000001 /* -W-UV */
#define NV_UPOINTZ_POINT                                 0x005807FC /* -W-4R */
#define NV_UPOINTZ_POINT_X                                     15:0 /* -W-SF */
#define NV_UPOINTZ_POINT_Y                                    31:16 /* -W-SF */
#define NV_UPOINTZ_COLOR(i)                      (0x00580800+(i)*8) /* -W-4A */
#define NV_UPOINTZ_COLOR__SIZE_1                                256 /*       */
#define NV_UPOINTZ_COLOR_VALUE                                 31:0 /* -W-VF */
#define NV_UPOINTZ_ZETA(i)                       (0x00580804+(i)*8) /* -W-4A */
#define NV_UPOINTZ_ZETA__SIZE_1                                 256 /*       */
#define NV_UPOINTZ_ZETA_VALUE                                  31:0 /* -W-VF */
/* usr_line.ref */
#define NV_ULINE                              0x00491FFF:0x00490000 /* -W--D */
#define NV_ULINE_CTX_SWITCH                              0x00490000 /* -W-4R */
#define NV_ULINE_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
#define NV_ULINE_CTX_SWITCH_CHID                              22:16 /* -W-UF */
#define NV_ULINE_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
#define NV_ULINE_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
#define NV_ULINE_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
#define NV_ULINE_SET_NOTIFY                              0x00490104 /* -W-4R */
#define NV_ULINE_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
#define NV_ULINE_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
#define NV_ULINE_COLOR                                   0x00490304 /* -W-4R */
#define NV_ULINE_COLOR_VALUE                                   31:0 /* -W-VF */
#define NV_ULINE_LINE_0(i)                       (0x00490400+(i)*8) /* -W-4A */
#define NV_ULINE_LINE_0__SIZE_1                                  16 /*       */
#define NV_ULINE_LINE_0_X                                      15:0 /* -W-SF */
#define NV_ULINE_LINE_0_Y                                     31:16 /* -W-SF */
#define NV_ULINE_LINE_1(i)                       (0x00490404+(i)*8) /* -W-4A */
#define NV_ULINE_LINE_1__SIZE_1                                  16 /*       */
#define NV_ULINE_LINE_1_X                                      15:0 /* -W-SF */
#define NV_ULINE_LINE_1_Y                                     31:16 /* -W-SF */
#define NV_ULINE_LINE32_0(i)                    (0x00490480+(i)*16) /* -W-4A */
#define NV_ULINE_LINE32_0__SIZE_1                                 8 /*       */
#define NV_ULINE_LINE32_0_X                                    31:0 /* -W-SF */
#define NV_ULINE_LINE32_1(i)                    (0x00490484+(i)*16) /* -W-4A */
#define NV_ULINE_LINE32_1__SIZE_1                                 8 /*       */
#define NV_ULINE_LINE32_1_Y                                    31:0 /* -W-SF */
#define NV_ULINE_LINE32_2(i)                    (0x00490488+(i)*16) /* -W-4A */
#define NV_ULINE_LINE32_2__SIZE_1                                 8 /*       */
#define NV_ULINE_LINE32_2_X                                    31:0 /* -W-SF */
#define NV_ULINE_LINE32_3(i)                    (0x0049048C+(i)*16) /* -W-4A */
#define NV_ULINE_LINE32_3__SIZE_1                                 8 /*       */
#define NV_ULINE_LINE32_3_Y                                    31:0 /* -W-SF */
#define NV_ULINE_POLYLINE(i)                     (0x00490500+(i)*4) /* -W-4A */
#define NV_ULINE_POLYLINE__SIZE_1                                32 /*       */
#define NV_ULINE_POLYLINE_X                                    15:0 /* -W-SF */
#define NV_ULINE_POLYLINE_Y                                   31:16 /* -W-SF */
#define NV_ULINE_POLYLINE32_0(i)                 (0x00490580+(i)*8) /* -W-4A */
#define NV_ULINE_POLYLINE32_0__SIZE_1                            16 /*       */
#define NV_ULINE_POLYLINE32_0_X                                31:0 /* -W-SF */
#define NV_ULINE_POLYLINE32_1(i)                 (0x00490584+(i)*8) /* -W-4A */
#define NV_ULINE_POLYLINE32_1__SIZE_1                            16 /*       */
#define NV_ULINE_POLYLINE32_1_Y                                31:0 /* -W-SF */
#define NV_ULINE_CPOLYLINE_0(i)                  (0x00490600+(i)*8) /* -W-4A */
#define NV_ULINE_CPOLYLINE_0__SIZE_1                             16 /*       */
#define NV_ULINE_CPOLYLINE_0_COLOR                             31:0 /* -W-VF */
#define NV_ULINE_CPOLYLINE_1(i)                  (0x00490604+(i)*8) /* -W-4A */
#define NV_ULINE_CPOLYLINE_1__SIZE_1                             16 /*       */
#define NV_ULINE_CPOLYLINE_1_X                                 15:0 /* -W-SF */
#define NV_ULINE_CPOLYLINE_1_Y                                31:16 /* -W-SF */
/* usr_lin.ref */
#define NV_ULIN                               0x004A1FFF:0x004A0000 /* -W--D */
#define NV_ULIN_CTX_SWITCH                               0x004A0000 /* -W-4R */
#define NV_ULIN_CTX_SWITCH_INSTANCE                            15:0 /* -W-UF */
#define NV_ULIN_CTX_SWITCH_CHID                               22:16 /* -W-UF */
#define NV_ULIN_CTX_SWITCH_VOLATILE                           31:31 /* -W-VF */
#define NV_ULIN_CTX_SWITCH_VOLATILE_IGNORE               0x00000000 /* -W--V */
#define NV_ULIN_CTX_SWITCH_VOLATILE_RESET                0x00000001 /* -W--V */
#define NV_ULIN_SET_NOTIFY                               0x004A0104 /* -W-4R */
#define NV_ULIN_SET_NOTIFY_PARAMETER                           31:0 /* -W-VF */
#define NV_ULIN_SET_NOTIFY_PARAMETER_WRITE               0x00000000 /* -W--V */
#define NV_ULIN_COLOR                                    0x004A0304 /* -W-4R */
#define NV_ULIN_COLOR_VALUE                                    31:0 /* -W-VF */
#define NV_ULIN_LIN_0(i)                         (0x004A0400+(i)*8) /* -W-4A */
#define NV_ULIN_LIN_0__SIZE_1                                    16 /*       */
#define NV_ULIN_LIN_0_X                                        15:0 /* -W-SF */
#define NV_ULIN_LIN_0_Y                                       31:16 /* -W-SF */
#define NV_ULIN_LIN_1(i)                         (0x004A0404+(i)*8) /* -W-4A */
#define NV_ULIN_LIN_1__SIZE_1                                    16 /*       */
#define NV_ULIN_LIN_1_X                                        15:0 /* -W-SF */
#define NV_ULIN_LIN_1_Y                                       31:16 /* -W-SF */
#define NV_ULIN_LIN32_0(i)                      (0x004A0480+(i)*16) /* -W-4A */
#define NV_ULIN_LIN32_0__SIZE_1                                   8 /*       */
#define NV_ULIN_LIN32_0_X                                      31:0 /* -W-SF */
#define NV_ULIN_LIN32_1(i)                      (0x004A0484+(i)*16) /* -W-4A */
#define NV_ULIN_LIN32_1__SIZE_1                                   8 /*       */
#define NV_ULIN_LIN32_1_Y                                      31:0 /* -W-SF */
#define NV_ULIN_LIN32_2(i)                      (0x004A0488+(i)*16) /* -W-4A */
#define NV_ULIN_LIN32_2__SIZE_1                                   8 /*       */
#define NV_ULIN_LIN32_2_X                                      31:0 /* -W-SF */
#define NV_ULIN_LIN32_3(i)                      (0x004A048C+(i)*16) /* -W-4A */
#define NV_ULIN_LIN32_3__SIZE_1                                   8 /*       */
#define NV_ULIN_LIN32_3_Y                                      31:0 /* -W-SF */
#define NV_ULIN_POLYLIN(i)                       (0x004A0500+(i)*4) /* -W-4A */
#define NV_ULIN_POLYLIN__SIZE_1                                  32 /*       */
#define NV_ULIN_POLYLIN_X                                      15:0 /* -W-SF */
#define NV_ULIN_POLYLIN_Y                                     31:16 /* -W-SF */
#define NV_ULIN_POLYLIN32_0(i)                   (0x004A0580+(i)*8) /* -W-4A */
#define NV_ULIN_POLYLIN32_0__SIZE_1                              16 /*       */
#define NV_ULIN_POLYLIN32_0_X                                  31:0 /* -W-SF */
#define NV_ULIN_POLYLIN32_1(i)                   (0x004A0584+(i)*8) /* -W-4A */
#define NV_ULIN_POLYLIN32_1__SIZE_1                              16 /*       */
#define NV_ULIN_POLYLIN32_1_Y                                  31:0 /* -W-SF */
#define NV_ULIN_CPOLYLIN_0(i)                    (0x004A0600+(i)*8) /* -W-4A */
#define NV_ULIN_CPOLYLIN_0__SIZE_1                               16 /*       */
#define NV_ULIN_CPOLYLIN_0_COLOR                               31:0 /* -W-VF */
#define NV_ULIN_CPOLYLIN_1(i)                    (0x004A0604+(i)*8) /* -W-4A */
#define NV_ULIN_CPOLYLIN_1__SIZE_1                               16 /*       */
#define NV_ULIN_CPOLYLIN_1_X                                   15:0 /* -W-SF */
#define NV_ULIN_CPOLYLIN_1_Y                                  31:16 /* -W-SF */
/* usr_mem_to_mem.ref */
#define NV_UMEMFMT                            0x004D1FFF:0x004D0000 /* -W--D */
#define NV_UMEMFMT_CTX_SWITCH                            0x004D0000 /* -W-4R */
#define NV_UMEMFMT_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
#define NV_UMEMFMT_CTX_SWITCH_CHID                            22:16 /* -W-UF */
#define NV_UMEMFMT_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
#define NV_UMEMFMT_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
#define NV_UMEMFMT_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
#define NV_UMEMFMT_SET_NOTIFY                            0x004D0104 /* -W-4R */
#define NV_UMEMFMT_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
#define NV_UMEMFMT_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
#define NV_UMEMFMT_OFFSET_IN                             0x004D030C /* -W-4R */
#define NV_UMEMFMT_OFFSET_IN_VALUE                             31:0 /* -W-UF */
#define NV_UMEMFMT_OFFSET_OUT                            0x004D0310 /* -W-4R */
#define NV_UMEMFMT_OFFSET_OUT_VALUE                            31:0 /* -W-UF */
#define NV_UMEMFMT_PITCH_IN                              0x004D0314 /* -W-4R */
#define NV_UMEMFMT_PITCH_IN_VALUE                              31:0 /* -W-SF */
#define NV_UMEMFMT_PITCH_OUT                             0x004D0318 /* -W-4R */
#define NV_UMEMFMT_PITCH_OUT_VALUE                             31:0 /* -W-SF */
#define NV_UMEMFMT_LINE_LENGTH_IN                        0x004D031C /* -W-4R */
#define NV_UMEMFMT_LINE_LENGTH_IN_VALUE                        31:0 /* -W-UF */
#define NV_UMEMFMT_LINE_COUNT                            0x004D0320 /* -W-4R */
#define NV_UMEMFMT_LINE_COUNT_VALUE                            31:0 /* -W-UF */
#define NV_UMEMFMT_FORMAT                                0x004D0324 /* -W-4R */
#define NV_UMEMFMT_FORMAT_INPUT_INC                             2:0 /* -W-UF */
#define NV_UMEMFMT_FORMAT_INPUT_INC_1                    0x00000001 /* -WIUV */
#define NV_UMEMFMT_FORMAT_INPUT_INC_2                    0x00000002 /* -W-UV */
#define NV_UMEMFMT_FORMAT_INPUT_INC_4                    0x00000004 /* -W-UV */
#define NV_UMEMFMT_FORMAT_OUTPUT_INC                           10:8 /* -W-UF */
#define NV_UMEMFMT_FORMAT_OUTPUT_INC_1                   0x00000001 /* -WIUV */
#define NV_UMEMFMT_FORMAT_OUTPUT_INC_2                   0x00000002 /* -W-UV */
#define NV_UMEMFMT_FORMAT_OUTPUT_INC_4                   0x00000004 /* -W-UV */
#define NV_UMEMFMT_BUF_NOTIFY                            0x004D0328 /* -W-4R */
#define NV_UMEMFMT_BUF_NOTIFY_VALUE                            31:0 /* -W-UF */
/* usr_triangle.ref */
#define NV_UTRI                               0x004B1FFF:0x004B0000 /* -W--D */
#define NV_UTRI_CTX_SWITCH                               0x004B0000 /* -W-4R */
#define NV_UTRI_CTX_SWITCH_INSTANCE                            15:0 /* -W-UF */
#define NV_UTRI_CTX_SWITCH_CHID                               22:16 /* -W-UF */
#define NV_UTRI_CTX_SWITCH_VOLATILE                           31:31 /* -W-VF */
#define NV_UTRI_CTX_SWITCH_VOLATILE_IGNORE               0x00000000 /* -W--V */
#define NV_UTRI_CTX_SWITCH_VOLATILE_RESET                0x00000001 /* -W--V */
#define NV_UTRI_SET_NOTIFY                               0x004B0104 /* -W-4R */
#define NV_UTRI_SET_NOTIFY_PARAMETER                           31:0 /* -W-VF */
#define NV_UTRI_SET_NOTIFY_PARAMETER_WRITE               0x00000000 /* -W--V */
#define NV_UTRI_COLOR                                    0x004B0304 /* -W-4R */
#define NV_UTRI_COLOR_VALUE                                    31:0 /* -W-VF */
#define NV_UTRI_TRIANGLE_0                               0x004B0310 /* -W-4R */
#define NV_UTRI_TRIANGLE_0_X                                   15:0 /* -W-SF */
#define NV_UTRI_TRIANGLE_0_Y                                  31:16 /* -W-SF */
#define NV_UTRI_TRIANGLE_1                               0x004B0314 /* -W-4R */
#define NV_UTRI_TRIANGLE_1_X                                   15:0 /* -W-SF */
#define NV_UTRI_TRIANGLE_1_Y                                  31:16 /* -W-SF */
#define NV_UTRI_TRIANGLE_2                               0x004B0318 /* -W-4R */
#define NV_UTRI_TRIANGLE_2_X                                   15:0 /* -W-SF */
#define NV_UTRI_TRIANGLE_2_Y                                  31:16 /* -W-SF */
#define NV_UTRI_TRIANGLE32_0                             0x004B0320 /* -W-4R */
#define NV_UTRI_TRIANGLE32_0_X                                 31:0 /* -W-SF */
#define NV_UTRI_TRIANGLE32_1                             0x004B0324 /* -W-4R */
#define NV_UTRI_TRIANGLE32_1_Y                                 31:0 /* -W-SF */
#define NV_UTRI_TRIANGLE32_2                             0x004B0328 /* -W-4R */
#define NV_UTRI_TRIANGLE32_2_X                                 31:0 /* -W-SF */
#define NV_UTRI_TRIANGLE32_3                             0x004B032C /* -W-4R */
#define NV_UTRI_TRIANGLE32_3_Y                                 31:0 /* -W-SF */
#define NV_UTRI_TRIANGLE32_4                             0x004B0330 /* -W-4R */
#define NV_UTRI_TRIANGLE32_4_X                                 31:0 /* -W-SF */
#define NV_UTRI_TRIANGLE32_5                             0x004B0334 /* -W-4R */
#define NV_UTRI_TRIANGLE32_5_Y                                 31:0 /* -W-SF */
#define NV_UTRI_TRIMESH(i)                       (0x004B0400+(i)*4) /* -W-4A */
#define NV_UTRI_TRIMESH__SIZE_1                                  32 /*       */
#define NV_UTRI_TRIMESH_X                                      15:0 /* -W-SF */
#define NV_UTRI_TRIMESH_Y                                     31:16 /* -W-SF */
#define NV_UTRI_TRIMESH32_0(i)                   (0x004B0480+(i)*8) /* -W-4A */
#define NV_UTRI_TRIMESH32_0__SIZE_1                              16 /*       */
#define NV_UTRI_TRIMESH32_0_X                                  31:0 /* -W-SF */
#define NV_UTRI_TRIMESH32_1(i)                   (0x004B0484+(i)*8) /* -W-4A */
#define NV_UTRI_TRIMESH32_1__SIZE_1                              16 /*       */
#define NV_UTRI_TRIMESH32_1_Y                                  31:0 /* -W-SF */
#define NV_UTRI_CTRIANGLE_0(i)                  (0x004B0500+(i)*16) /* -W-4A */
#define NV_UTRI_CTRIANGLE_0__SIZE_1                               8 /*       */
#define NV_UTRI_CTRIANGLE_0_COLOR                              31:0 /* -W-VF */
#define NV_UTRI_CTRIANGLE_1(i)                  (0x004B0504+(i)*16) /* -W-4A */
#define NV_UTRI_CTRIANGLE_1__SIZE_1                               8 /*       */
#define NV_UTRI_CTRIANGLE_1_X                                  15:0 /* -W-SF */
#define NV_UTRI_CTRIANGLE_1_Y                                 31:16 /* -W-SF */
#define NV_UTRI_CTRIANGLE_2(i)                  (0x004B0508+(i)*16) /* -W-4A */
#define NV_UTRI_CTRIANGLE_2__SIZE_1                               8 /*       */
#define NV_UTRI_CTRIANGLE_2_X                                  15:0 /* -W-SF */
#define NV_UTRI_CTRIANGLE_2_Y                                 31:16 /* -W-SF */
#define NV_UTRI_CTRIANGLE_3(i)                  (0x004B050C+(i)*16) /* -W-4A */
#define NV_UTRI_CTRIANGLE_3__SIZE_1                               8 /*       */
#define NV_UTRI_CTRIANGLE_3_X                                  15:0 /* -W-SF */
#define NV_UTRI_CTRIANGLE_3_Y                                 31:16 /* -W-SF */
#define NV_UTRI_CTRIMESH_0(i)                    (0x004B0580+(i)*8) /* -W-4A */
#define NV_UTRI_CTRIMESH_0__SIZE_1                               16 /*       */
#define NV_UTRI_CTRIMESH_0_COLOR                               31:0 /* -W-VF */
#define NV_UTRI_CTRIMESH_1(i)                    (0x004B0584+(i)*8) /* -W-4A */
#define NV_UTRI_CTRIMESH_1__SIZE_1                               16 /*       */
#define NV_UTRI_CTRIMESH_1_X                                   15:0 /* -W-SF */
#define NV_UTRI_CTRIMESH_1_Y                                  31:16 /* -W-SF */
/* usr_rectangle.ref */
#define NV_URECT                              0x00471FFF:0x00470000 /* -W--D */
#define NV_URECT_CTX_SWITCH                              0x00470000 /* -W-4R */
#define NV_URECT_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
#define NV_URECT_CTX_SWITCH_CHID                              22:16 /* -W-UF */
#define NV_URECT_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
#define NV_URECT_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
#define NV_URECT_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
#define NV_URECT_SET_NOTIFY                              0x00470104 /* -W-4R */
#define NV_URECT_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
#define NV_URECT_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
#define NV_URECT_COLOR                                   0x00470304 /* -W-4R */
#define NV_URECT_COLOR_VALUE                                   31:0 /* -W-VF */
#define NV_URECT_RECTANGLE_0(i)                  (0x00470400+(i)*8) /* -W-4A */
#define NV_URECT_RECTANGLE_0__SIZE_1                             16 /*       */
#define NV_URECT_RECTANGLE_0_X                                 15:0 /* -W-SF */
#define NV_URECT_RECTANGLE_0_Y                                31:16 /* -W-SF */
#define NV_URECT_RECTANGLE_1(i)                  (0x00470404+(i)*8) /* -W-4A */
#define NV_URECT_RECTANGLE_1__SIZE_1                             16 /*       */
#define NV_URECT_RECTANGLE_1_WIDTH                             15:0 /* -W-UF */
#define NV_URECT_RECTANGLE_1_HEIGHT                           31:16 /* -W-UF */
/* usr_image_blit.ref */
#define NV_UBLIT                              0x00501FFF:0x00500000 /* -W--D */
#define NV_UBLIT_CTX_SWITCH                              0x00500000 /* -W-4R */
#define NV_UBLIT_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
#define NV_UBLIT_CTX_SWITCH_CHID                              22:16 /* -W-UF */
#define NV_UBLIT_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
#define NV_UBLIT_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
#define NV_UBLIT_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
#define NV_UBLIT_SET_NOTIFY                              0x00500104 /* -W-4R */
#define NV_UBLIT_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
#define NV_UBLIT_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
#define NV_UBLIT_POINT_IN                                0x00500300 /* -W-4R */
#define NV_UBLIT_POINT_IN_X                                    15:0 /* -W-SF */
#define NV_UBLIT_POINT_IN_Y                                   31:16 /* -W-SF */
#define NV_UBLIT_POINT_OUT                               0x00500304 /* -W-4R */
#define NV_UBLIT_POINT_OUT_X                                   15:0 /* -W-SF */
#define NV_UBLIT_POINT_OUT_Y                                  31:16 /* -W-SF */
#define NV_UBLIT_SIZE                                    0x00500308 /* -W-4R */
#define NV_UBLIT_SIZE_WIDTH                                    15:0 /* -W-UF */
#define NV_UBLIT_SIZE_HEIGHT                                  31:16 /* -W-UF */
/* usr_image_from_cpu.ref */
#define NV_UIMAGE                             0x00511FFF:0x00510000 /* -W--D */
#define NV_UIMAGE_CTX_SWITCH                             0x00510000 /* -W-4R */
#define NV_UIMAGE_CTX_SWITCH_INSTANCE                          15:0 /* -W-UF */
#define NV_UIMAGE_CTX_SWITCH_CHID                             22:16 /* -W-UF */
#define NV_UIMAGE_CTX_SWITCH_VOLATILE                         31:31 /* -W-VF */
#define NV_UIMAGE_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* -W--V */
#define NV_UIMAGE_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
#define NV_UIMAGE_SET_NOTIFY                             0x00510104 /* -W-4R */
#define NV_UIMAGE_SET_NOTIFY_PARAMETER                         31:0 /* -W-VF */
#define NV_UIMAGE_SET_NOTIFY_PARAMETER_WRITE             0x00000000 /* -W--V */
#define NV_UIMAGE_POINT                                  0x00510304 /* -W-4R */
#define NV_UIMAGE_POINT_X                                      15:0 /* -W-SF */
#define NV_UIMAGE_POINT_Y                                     31:16 /* -W-SF */
#define NV_UIMAGE_SIZE                                   0x00510308 /* -W-4R */
#define NV_UIMAGE_SIZE_WIDTH                                   15:0 /* -W-UF */
#define NV_UIMAGE_SIZE_HEIGHT                                 31:16 /* -W-UF */
#define NV_UIMAGE_SIZE_IN                                0x0051030C /* -W-4R */
#define NV_UIMAGE_SIZE_IN_WIDTH                                15:0 /* -W-UF */
#define NV_UIMAGE_SIZE_IN_HEIGHT                              31:16 /* -W-UF */
#define NV_UIMAGE_COLOR(i)                       (0x00510400+(i)*4) /* -W-4A */
#define NV_UIMAGE_COLOR__SIZE_1                                  32 /*       */
#define NV_UIMAGE_COLOR_VALUE                                  31:0 /* -W-VF */
/* usr_bitmap_from_cpu.ref */
#define NV_UBITMAP                            0x00521FFF:0x00520000 /* -W--D */
#define NV_UBITMAP_CTX_SWITCH                            0x00520000 /* -W-4R */
#define NV_UBITMAP_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
#define NV_UBITMAP_CTX_SWITCH_CHID                            22:16 /* -W-UF */
#define NV_UBITMAP_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
#define NV_UBITMAP_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
#define NV_UBITMAP_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
#define NV_UBITMAP_SET_NOTIFY                            0x00520104 /* -W-4R */
#define NV_UBITMAP_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
#define NV_UBITMAP_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
#define NV_UBITMAP_COLOR0                                0x00520308 /* -W-4R */
#define NV_UBITMAP_COLOR0_VALUE                                31:0 /* -W-VF */
#define NV_UBITMAP_COLOR1                                0x0052030C /* -W-4R */
#define NV_UBITMAP_COLOR1_VALUE                                31:0 /* -W-VF */
#define NV_UBITMAP_POINT                                 0x00520310 /* -W-4R */
#define NV_UBITMAP_POINT_X                                     15:0 /* -W-SF */
#define NV_UBITMAP_POINT_Y                                    31:16 /* -W-SF */
#define NV_UBITMAP_SIZE                                  0x00520314 /* -W-4R */
#define NV_UBITMAP_SIZE_WIDTH                                  15:0 /* -W-UF */
#define NV_UBITMAP_SIZE_HEIGHT                                31:16 /* -W-UF */
#define NV_UBITMAP_SIZE_IN                               0x00520318 /* -W-4R */
#define NV_UBITMAP_SIZE_IN_WIDTH                               15:0 /* -W-UF */
#define NV_UBITMAP_SIZE_IN_HEIGHT                             31:16 /* -W-UF */
#define NV_UBITMAP_MONOCHROME(i)                 (0x00520400+(i)*4) /* -W-4A */
#define NV_UBITMAP_MONOCHROME__SIZE_1                            32 /*       */
#define NV_UBITMAP_MONOCHROME_BITMAP                           31:0 /* -W-VF */
/* usr_image_to_mem.ref */
#define NV_UTOMEM                             0x00541FFF:0x00540000 /* -W--D */
#define NV_UTOMEM_CTX_SWITCH                             0x00540000 /* -W-4R */
#define NV_UTOMEM_CTX_SWITCH_INSTANCE                          15:0 /* -W-UF */
#define NV_UTOMEM_CTX_SWITCH_CHID                             22:16 /* -W-UF */
#define NV_UTOMEM_CTX_SWITCH_VOLATILE                         31:31 /* -W-VF */
#define NV_UTOMEM_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* -W--V */
#define NV_UTOMEM_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
#define NV_UTOMEM_SET_NOTIFY                             0x00540104 /* -W-4R */
#define NV_UTOMEM_SET_NOTIFY_PARAMETER                         31:0 /* -W-VF */
#define NV_UTOMEM_SET_NOTIFY_PARAMETER_WRITE             0x00000000 /* -W--V */
#define NV_UTOMEM_POINT                                  0x00540308 /* -W-4R */
#define NV_UTOMEM_POINT_X                                      15:0 /* -W-SF */
#define NV_UTOMEM_POINT_Y                                     31:16 /* -W-SF */
#define NV_UTOMEM_SIZE                                   0x0054030C /* -W-4R */
#define NV_UTOMEM_SIZE_WIDTH                                   15:0 /* -W-UF */
#define NV_UTOMEM_SIZE_HEIGHT                                 31:16 /* -W-UF */
#define NV_UTOMEM_IMAGE_PITCH                            0x00540310 /* -W-4R */
#define NV_UTOMEM_IMAGE_PITCH_VALUE                            31:0 /* -W-SF */
#define NV_UTOMEM_IMAGE_START                            0x00540314 /* -W-4R */
#define NV_UTOMEM_IMAGE_START_OFFSET                           31:0 /* -W-UF */
/* usr_scaled_image_from_mem.ref */
#define NV_USCALED                            0x004E1FFF:0x004E0000 /* -W--D */
#define NV_USCALED_CTX_SWITCH                            0x004E0000 /* -W-4R */
#define NV_USCALED_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
#define NV_USCALED_CTX_SWITCH_CHID                            22:16 /* -W-UF */
#define NV_USCALED_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
#define NV_USCALED_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
#define NV_USCALED_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
#define NV_USCALED_SET_NOTIFY                            0x004E0104 /* -W-4R */
#define NV_USCALED_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
#define NV_USCALED_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
#define NV_USCALED_CLIP_0                                0x004E0308 /* -W-4R */
#define NV_USCALED_CLIP_0_X                                    15:0 /* -W-SF */
#define NV_USCALED_CLIP_0_Y                                   31:16 /* -W-SF */
#define NV_USCALED_CLIP_1                                0x004E030C /* -W-4R */
#define NV_USCALED_CLIP_1_WIDTH                                15:0 /* -W-UF */
#define NV_USCALED_CLIP_1_HEIGHT                              31:16 /* -W-UF */
#define NV_USCALED_RECTANGLE_OUT_0                       0x004E0310 /* -W-4R */
#define NV_USCALED_RECTANGLE_OUT_0_X                           15:0 /* -W-SF */
#define NV_USCALED_RECTANGLE_OUT_0_Y                          31:16 /* -W-SF */
#define NV_USCALED_RECTANGLE_OUT_1                       0x004E0314 /* -W-4R */
#define NV_USCALED_RECTANGLE_OUT_1_WIDTH                       15:0 /* -W-UF */
#define NV_USCALED_RECTANGLE_OUT_1_HEIGHT                     31:16 /* -W-UF */
#define NV_USCALED_DELTA_DU_DX                           0x004E0318 /* -W-4R */
#define NV_USCALED_DELTA_DU_DX_R_FRACTION                      19:0 /* -W-SF */
#define NV_USCALED_DELTA_DU_DX_R_INT                          31:20 /* -W-UF */
#define NV_USCALED_DELTA_DU_DX_R                               31:0 /* -W-UF */
#define NV_USCALED_DELTA_DV_DY                           0x004E031C /* -W-4R */
#define NV_USCALED_DELTA_DV_DY_R_FRACTION                      19:0 /* -W-SF */
#define NV_USCALED_DELTA_DV_DY_R_INT                          31:20 /* -W-UF */
#define NV_USCALED_DELTA_DV_DY_R                               31:0 /* -W-UF */
#define NV_USCALED_SIZE                                  0x004E0400 /* -W-4R */
#define NV_USCALED_SIZE_WIDTH                                  15:0 /* -W-UF */
#define NV_USCALED_SIZE_HEIGHT                                31:16 /* -W-UF */
#define NV_USCALED_PITCH                                 0x004E0404 /* -W-4R */
#define NV_USCALED_PITCH_VALUE                                 31:0 /* -W-SF */
#define NV_USCALED_OFFSET                                0x004E0408 /* -W-4R */
#define NV_USCALED_OFFSET_VALUE                               31:0  /* -W-UF */
#define NV_USCALED_POINT                                 0x004E040C /* -W-4R */
#define NV_USCALED_POINT_V_FRACTION                            11:0 /* -W-UF */
#define NV_USCALED_POINT_V_INT                                15:12 /* -W-UF */
#define NV_USCALED_POINT_V_VALUE                               15:0 /* -W-UF */
#define NV_USCALED_POINT_U_FRACTION                           19:16 /* -W-UF */
#define NV_USCALED_POINT_U_INT                                31:20 /* -W-UF */
#define NV_USCALED_POINT_U_VALUE                               31:0 /* -W-UF */
#define NV_USCALED_SIZE_Y                                0x004E0400 /* -W-4R */
#define NV_USCALED_SIZE_Y_WIDTH                                15:0 /* -W-UF */
#define NV_USCALED_SIZE_Y_HEIGHT                              31:16 /* -W-UF */
#define NV_USCALED_PITCH_420                             0x004E0404 /* -W-4R */
#define NV_USCALED_PITCH_420_Y                                 15:0 /* -W-UF */
#define NV_USCALED_PITCH_420_UV                               31:16 /* -W-UF */
#define NV_USCALED_OFFSET_Y                              0x004E0408 /* -W-4R */
#define NV_USCALED_OFFSET_Y_VALUE                             31:0  /* -W-UF */
#define NV_USCALED_OFFSET_U                              0x004E040C /* -W-4R */
#define NV_USCALED_OFFSET_U_VALUE                             31:0  /* -W-UF */
#define NV_USCALED_OFFSET_V                              0x004E0410 /* -W-4R */
#define NV_USCALED_OFFSET_V_VALUE                             31:0  /* -W-UF */
#define NV_USCALED_POINT_Y                               0x004E0414 /* -W-4R */
#define NV_USCALED_POINT_Y_V_FRACTION                          11:0 /* -W-UF */
#define NV_USCALED_POINT_Y_V_INT                              15:12 /* -W-UF */
#define NV_USCALED_POINT_Y_V_VALUE                             15:0 /* -W-UF */
#define NV_USCALED_POINT_Y_U_FRACTION                         19:16 /* -W-UF */
#define NV_USCALED_POINT_Y_U_INT                              31:20 /* -W-UF */
#define NV_USCALED_POINT_Y_U_VALUE                            31:16 /* -W-UF */
/* usr_stretch_from_cpu.ref */
#define NV_USTRTCH                            0x00551FFF:0x00550000 /* -W--D */
#define NV_USTRTCH_CTX_SWITCH                            0x00550000 /* -W-4R */
#define NV_USTRTCH_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
#define NV_USTRTCH_CTX_SWITCH_CHID                            22:16 /* -W-UF */
#define NV_USTRTCH_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
#define NV_USTRTCH_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
#define NV_USTRTCH_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
#define NV_USTRTCH_SET_NOTIFY                            0x00550104 /* -W-4R */
#define NV_USTRTCH_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
#define NV_USTRTCH_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
#define NV_USTRTCH_SIZE_IN                               0x00550304 /* -W-4R */
#define NV_USTRTCH_SIZE_IN_WIDTH                               15:0 /* -W-UF */
#define NV_USTRTCH_SIZE_IN_HEIGHT                             31:16 /* -W-UF */
#define NV_USTRTCH_DELTA_DX_DU                           0x00550308 /* -W-4R */
#define NV_USTRTCH_DELTA_DX_DU_R_FRACTION                      19:0 /* -W-UF */
#define NV_USTRTCH_DELTA_DX_DU_R_INT                          31:20 /* -W-UF */
#define NV_USTRTCH_DELTA_DX_DU_R                               31:0 /* -W-UF */
#define NV_USTRTCH_DELTA_DY_DV                           0x0055030C /* -W-4R */
#define NV_USTRTCH_DELTA_DY_DV_R_FRACTION                      19:0 /* -W-UF */
#define NV_USTRTCH_DELTA_DY_DV_R_INT                          31:20 /* -W-UF */
#define NV_USTRTCH_DELTA_DY_DV_R                               31:0 /* -W-UF */
#define NV_USTRTCH_CLIP_0                                0x00550310 /* -W-4R */
#define NV_USTRTCH_CLIP_0_X                                    15:0 /* -W-SF */
#define NV_USTRTCH_CLIP_0_Y                                   31:16 /* -W-SF */
#define NV_USTRTCH_CLIP_1                                0x00550314 /* -W-4R */
#define NV_USTRTCH_CLIP_1_WIDTH                                15:0 /* -W-UF */
#define NV_USTRTCH_CLIP_1_HEIGHT                              31:16 /* -W-UF */
#define NV_USTRTCH_POINT12D4                             0x00550318 /* -W-4R */
#define NV_USTRTCH_POINT12D4_X_FRACTION                         3:0 /* -W-SF */
#define NV_USTRTCH_POINT12D4_X_INT                             15:4 /* -W-SF */
#define NV_USTRTCH_POINT12D4_X                                 15:0 /* -W-SF */
#define NV_USTRTCH_POINT12D4_Y_FRACTION                       19:16 /* -W-SF */
#define NV_USTRTCH_POINT12D4_Y_INT                            31:20 /* -W-SF */
#define NV_USTRTCH_POINT12D4_Y                                31:16 /* -W-SF */
#define NV_USTRTCH_COLOR(i)                      (0x00550400+(i)*4) /* -W-4A */
#define NV_USTRTCH_COLOR__SIZE_1                               1792 /*       */
#define NV_USTRTCH_COLOR_VALUE                                 31:0 /* -W-VF */
/* usr_win95_text.ref */
#define NV_UW95TXT                            0x004C1FFF:0x004C0000 /* -W--D */
#define NV_UW95TXT_CTX_SWITCH                            0x004C0000 /* -W-4R */
#define NV_UW95TXT_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
#define NV_UW95TXT_CTX_SWITCH_CHID                            22:16 /* -W-UF */
#define NV_UW95TXT_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
#define NV_UW95TXT_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
#define NV_UW95TXT_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
#define NV_UW95TXT_SET_NOTIFY                            0x004C0104 /* -W-4R */
#define NV_UW95TXT_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
#define NV_UW95TXT_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
#define NV_UW95TXT_COLOR_A                               0x004C03FC /* -W-4R */
#define NV_UW95TXT_COLOR_A_VALUE                               31:0 /* -W-VF */
#define NV_UW95TXT_RECT_NCLIP_0(i)               (0x004C0400+(i)*8) /* -W-4A */
#define NV_UW95TXT_RECT_NCLIP_0__SIZE_1                          64 /*       */
#define NV_UW95TXT_RECT_NCLIP_0_Y                              15:0 /* -W-SF */
#define NV_UW95TXT_RECT_NCLIP_0_X                             31:16 /* -W-SF */
#define NV_UW95TXT_RECT_NCLIP_1(i)               (0x004C0404+(i)*8) /* -W-4A */
#define NV_UW95TXT_RECT_NCLIP_1__SIZE_1                          64 /*       */
#define NV_UW95TXT_RECT_NCLIP_1_HEIGHT                         15:0 /* -W-UF */
#define NV_UW95TXT_RECT_NCLIP_1_WIDTH                         31:16 /* -W-UF */
#define NV_UW95TXT_CLIP_B_0                              0x004C07F4 /* -W-4R */
#define NV_UW95TXT_CLIP_B_0_LEFT                               15:0 /* -W-SF */
#define NV_UW95TXT_CLIP_B_0_TOP                               31:16 /* -W-SF */
#define NV_UW95TXT_CLIP_B_1                              0x004C07F8 /* -W-4R */
#define NV_UW95TXT_CLIP_B_1_RIGHT                              15:0 /* -W-SF */
#define NV_UW95TXT_CLIP_B_1_BOTTOM                            31:16 /* -W-SF */
#define NV_UW95TXT_COLOR_B                               0x004C07FC /* -W-4R */
#define NV_UW95TXT_COLOR_B_VALUE                               31:0 /* -W-VF */
#define NV_UW95TXT_RECT_CLIP_0(i)                (0x004C0800+(i)*8) /* -W-4A */
#define NV_UW95TXT_RECT_CLIP_0__SIZE_1                           64 /*       */
#define NV_UW95TXT_RECT_CLIP_0_LEFT                            15:0 /* -W-SF */
#define NV_UW95TXT_RECT_CLIP_0_TOP                            31:16 /* -W-SF */
#define NV_UW95TXT_RECT_CLIP_1(i)                (0x004C0804+(i)*8) /* -W-4A */
#define NV_UW95TXT_RECT_CLIP_1__SIZE_1                           64 /*       */
#define NV_UW95TXT_RECT_CLIP_1_RIGHT                           15:0 /* -W-SF */
#define NV_UW95TXT_RECT_CLIP_1_BOTTOM                         31:16 /* -W-SF */
#define NV_UW95TXT_CLIP_C_0                              0x004C0BEC /* -W-4R */
#define NV_UW95TXT_CLIP_C_0_LEFT                               15:0 /* -W-SF */
#define NV_UW95TXT_CLIP_C_0_TOP                               31:16 /* -W-SF */
#define NV_UW95TXT_CLIP_C_1                              0x004C0BF0 /* -W-4R */
#define NV_UW95TXT_CLIP_C_1_RIGHT                              15:0 /* -W-SF */
#define NV_UW95TXT_CLIP_C_1_BOTTOM                            31:16 /* -W-SF */
#define NV_UW95TXT_COLOR1_C                              0x004C0BF4 /* -W-4R */
#define NV_UW95TXT_COLOR1_C_VALUE                              31:0 /* -W-VF */
#define NV_UW95TXT_SIZE_C                                0x004C0BF8 /* -W-4R */
#define NV_UW95TXT_SIZE_C_WIDTH                                15:0 /* -W-UF */
#define NV_UW95TXT_SIZE_C_HEIGHT                              31:16 /* -W-UF */
#define NV_UW95TXT_POINT_C                               0x004C0BFC /* -W-4R */
#define NV_UW95TXT_POINT_C_X                                   15:0 /* -W-SF */
#define NV_UW95TXT_POINT_C_Y                                  31:16 /* -W-SF */
#define NV_UW95TXT_MONO_COLOR1_C(i)              (0x004C0C00+(i)*4) /* -W-4A */
#define NV_UW95TXT_MONO_COLOR1_C__SIZE_1                        128 /*       */
#define NV_UW95TXT_MONO_COLOR1_C_BITMAP                        31:0 /* -W-VF */
#define NV_UW95TXT_CLIP_D_0                              0x004C0FE8 /* -W-4R */
#define NV_UW95TXT_CLIP_D_0_LEFT                               15:0 /* -W-SF */
#define NV_UW95TXT_CLIP_D_0_TOP                               31:16 /* -W-SF */
#define NV_UW95TXT_CLIP_D_1                              0x004C0FEC /* -W-4R */
#define NV_UW95TXT_CLIP_D_1_RIGHT                              15:0 /* -W-SF */
#define NV_UW95TXT_CLIP_D_1_BOTTOM                            31:16 /* -W-SF */
#define NV_UW95TXT_COLOR1_D                              0x004C0FF0 /* -W-4R */
#define NV_UW95TXT_COLOR1_D_VALUE                              31:0 /* -W-VF */
#define NV_UW95TXT_SIZE_IN_D                             0x004C0FF4 /* -W-4R */
#define NV_UW95TXT_SIZE_IN_D_WIDTH                             15:0 /* -W-UF */
#define NV_UW95TXT_SIZE_IN_D_HEIGHT                           31:16 /* -W-UF */
#define NV_UW95TXT_SIZE_OUT_D                            0x004C0FF8 /* -W-4R */
#define NV_UW95TXT_SIZE_OUT_D_WIDTH                            15:0 /* -W-UF */
#define NV_UW95TXT_SIZE_OUT_D_HEIGHT                          31:16 /* -W-UF */
#define NV_UW95TXT_POINT_D                               0x004C0FFC /* -W-4R */
#define NV_UW95TXT_POINT_D_X                                   15:0 /* -W-SF */
#define NV_UW95TXT_POINT_D_Y                                  31:16 /* -W-SF */
#define NV_UW95TXT_MONO_COLOR1_D(i)              (0x004C1000+(i)*4) /* -W-4A */
#define NV_UW95TXT_MONO_COLOR1_D__SIZE_1                        128 /*       */
#define NV_UW95TXT_MONO_COLOR1_D_BITMAP                        31:0 /* -W-VF */
#define NV_UW95TXT_CLIP_E_0                              0x004C13E4 /* -W-4R */
#define NV_UW95TXT_CLIP_E_0_LEFT                               15:0 /* -W-SF */
#define NV_UW95TXT_CLIP_E_0_TOP                               31:16 /* -W-SF */
#define NV_UW95TXT_CLIP_E_1                              0x004C13E8 /* -W-4R */
#define NV_UW95TXT_CLIP_E_1_RIGHT                              15:0 /* -W-SF */
#define NV_UW95TXT_CLIP_E_1_BOTTOM                            31:16 /* -W-SF */
#define NV_UW95TXT_COLOR0_E                              0x004C13EC /* -W-4R */
#define NV_UW95TXT_COLOR0_E_VALUE                              31:0 /* -W-VF */
#define NV_UW95TXT_COLOR1_E                              0x004C13F0 /* -W-4R */
#define NV_UW95TXT_COLOR1_E_VALUE                              31:0 /* -W-VF */
#define NV_UW95TXT_SIZE_IN_E                             0x004C13F4 /* -W-4R */
#define NV_UW95TXT_SIZE_IN_E_WIDTH                             15:0 /* -W-UF */
#define NV_UW95TXT_SIZE_IN_E_HEIGHT                           31:16 /* -W-UF */
#define NV_UW95TXT_SIZE_OUT_E                            0x004C13F8 /* -W-4R */
#define NV_UW95TXT_SIZE_OUT_E_WIDTH                            15:0 /* -W-UF */
#define NV_UW95TXT_SIZE_OUT_E_HEIGHT                          31:16 /* -W-UF */
#define NV_UW95TXT_POINT_E                               0x004C13FC /* -W-4R */
#define NV_UW95TXT_POINT_E_X                                   15:0 /* -W-SF */
#define NV_UW95TXT_POINT_E_Y                                  31:16 /* -W-SF */
#define NV_UW95TXT_MONO_COLOR01_E(i)             (0x004C1400+(i)*4) /* -W-4A */
#define NV_UW95TXT_MONO_COLOR01_E__SIZE_1                       128 /*       */
#define NV_UW95TXT_MONO_COLOR01_E_BITMAP                       31:0 /* -W-VF */
/* usr_image_in_mem.ref */
#define NV_UINMEM                             0x005C1FFF:0x005C0000 /* -W--D */
#define NV_UINMEM_CTX_SWITCH                             0x005C0000 /* -W-4R */
#define NV_UINMEM_CTX_SWITCH_INSTANCE                          15:0 /* -W-UF */
#define NV_UINMEM_CTX_SWITCH_CHID                             22:16 /* -W-UF */
#define NV_UINMEM_CTX_SWITCH_VOLATILE                         31:31 /* -W-VF */
#define NV_UINMEM_CTX_SWITCH_VOLATILE_IGNORE             0x00000000 /* -W--V */
#define NV_UINMEM_CTX_SWITCH_VOLATILE_RESET              0x00000001 /* -W--V */
#define NV_UINMEM_SET_NOTIFY                             0x005C0104 /* -W-4R */
#define NV_UINMEM_SET_NOTIFY_PARAMETER                         31:0 /* -W-VF */
#define NV_UINMEM_SET_NOTIFY_PARAMETER_WRITE             0x00000000 /* -W--V */
#define NV_UINMEM_PITCH                                  0x005C0308 /* -W-4R */
#define NV_UINMEM_PITCH_VALUE                                  15:0 /* -WXUF */
#define NV_UINMEM_OFFSET                                 0x005C030C /* -W-4R */
#define NV_UINMEM_OFFSET_LINADRS                               22:0 /* -WIUF */
#define NV_UINMEM_OFFSET_LINADRS_0                       0x00000000 /* -WI-V */
#define NV_UINMEM_FORMAT                                 0x005C0300 /* -W-4R */
#define NV_UINMEM_FORMAT_VALUE                                 31:0 /* -WXUF */
#define NV_UINMEM_FORMAT_VALUE_LE_Y8_P4                  0x01010000 /* -W--V */
#define NV_UINMEM_FORMAT_VALUE_LE_Y16_P2                 0x01010001 /* -W--V */
#define NV_UINMEM_FORMAT_VALUE_LE_X1R5G5B5_P2            0x01000000 /* -W--V */
#define NV_UINMEM_FORMAT_VALUE_LE_X8R8G8B8               0x00000001 /* -W--V */
/* dev_framebuffer.ref */
#define NV_PFB                                0x00100FFF:0x00100000 /* RW--D */
#define NV_PFB_BOOT_0                                    0x00100000 /* RW-4R */
#define NV_PFB_BOOT_0_RAM_AMOUNT                                1:0 /* RWIVF */
#define NV_PFB_BOOT_0_RAM_AMOUNT_1MB                     0x00000000 /* RW--V */
#define NV_PFB_BOOT_0_RAM_AMOUNT_2MB                     0x00000001 /* RW--V */
#define NV_PFB_BOOT_0_RAM_AMOUNT_4MB                     0x00000002 /* RW--V */
#define NV_PFB_BOOT_0_RAM_AMOUNT_DEFAULT                 0x00000002 /* RWI-V */
#define NV_PFB_BOOT_0_RAM_WIDTH_128                             2:2 /* RW-VF */
#define NV_PFB_BOOT_0_RAM_WIDTH_128_OFF                  0x00000000 /* RW--V */
#define NV_PFB_BOOT_0_RAM_WIDTH_128_ON                   0x00000001 /* RW--V */
// need to consolidate nv3.ref files!
#define NV_PFB_BOOT_0_RAM_AMOUNT_8MB                     0x00000000 /* RW--V */
#define NV_PFB_BOOT_0_RAM_AMOUNT_UNDEFINED               0x00000003 /* RW--V */
#define NV_PFB_BOOT_0_RAM_BANKS                                 3:3 /* RWIVF */
#define NV_PFB_BOOT_0_RAM_BANKS_2BANK                    0x00000000 /* RWI-V */
#define NV_PFB_BOOT_0_RAM_BANKS_4BANK                    0x00000001 /* RW--V */
#define NV_PFB_BOOT_0_RAMDATA_TWIDDLE                           4:4 /* RWIVF */
#define NV_PFB_BOOT_0_RAMDATA_TWIDDLE_OFF                0x00000000 /* RWI-V */
#define NV_PFB_BOOT_0_RAMDATA_TWIDDLE_ON                 0x00000001 /* RW--V */
#define NV_PFB_BOOT_0_RAM_AMOUNT_EXTENSION                      5:5 /* RWIVF */
#define NV_PFB_BOOT_0_RAM_AMOUNT_EXTENSION_OFF           0x00000000 /* RWI-V */
#define NV_PFB_BOOT_0_RAM_AMOUNT_EXTENSION_8MB           0x00000001 /* RW--V */

#define NV_PFB_DELAY_1                                   0x00100044 /* RW-4R */
#define NV_PFB_DELAY_1_WRITE_ENABLE_RISE                        1:0 /* RWIUF */
#define NV_PFB_DELAY_1_WRITE_ENABLE_RISE_0               0x00000000 /* RWI-V */
#define NV_PFB_DELAY_1_WRITE_ENABLE_FALL                        5:4 /* RWIUF */
#define NV_PFB_DELAY_1_WRITE_ENABLE_FALL_0               0x00000000 /* RWI-V */
#define NV_PFB_DELAY_1_CAS_ENABLE_RISE                          9:8 /* RWIUF */
#define NV_PFB_DELAY_1_CAS_ENABLE_RISE_0                 0x00000000 /* RWI-V */
#define NV_PFB_DELAY_1_CAS_ENABLE_FALL                        13:12 /* RWIUF */
#define NV_PFB_DELAY_1_CAS_ENABLE_FALL_0                 0x00000000 /* RWI-V */
#define NV_PFB_DELAY_1_OUTPUT_DATA                            17:16 /* RWIUF */
#define NV_PFB_DELAY_1_OUTPUT_DATA_0                     0x00000000 /* RWI-V */
#define NV_PFB_DELAY_1_RAS_ENABLE                             21:20 /* RWIUF */
#define NV_PFB_DELAY_1_RAS_ENABLE_0                      0x00000000 /* RWI-V */
#define NV_PFB_DEBUG_0                                   0x00100080 /* RW-4R */
#define NV_PFB_DEBUG_0_PAGE_MODE                                0:0 /* RWIVF */
#define NV_PFB_DEBUG_0_PAGE_MODE_ENABLED                 0x00000000 /* RWI-V */
#define NV_PFB_DEBUG_0_PAGE_MODE_DISABLED                0x00000001 /* RW--V */
#define NV_PFB_DEBUG_0_REFRESH                                  4:4 /* RWIVF */
#define NV_PFB_DEBUG_0_REFRESH_ENABLED                   0x00000000 /* RWI-V */
#define NV_PFB_DEBUG_0_REFRESH_DISABLED                  0x00000001 /* RW--V */
#define NV_PFB_DEBUG_0_REFRESH_COUNTX64                        12:8 /* RWIVF */
#define NV_PFB_DEBUG_0_REFRESH_COUNTX64_DEFAULT          0x00000010 /* RWI-V */
#define NV_PFB_DEBUG_0_CASOE                                  20:20 /* RWIVF */
#define NV_PFB_DEBUG_0_CASOE_ENABLED                     0x00000000 /* RWI-V */
#define NV_PFB_DEBUG_0_CASOE_DISABLED                    0x00000001 /* RW--V */
#define NV_PFB_DEBUG_0_CKE_INVERT                             28:28 /* RWIVF */
#define NV_PFB_DEBUG_0_CKE_INVERT_OFF                    0x00000000 /* RWIVF */
#define NV_PFB_DEBUG_0_CKE_INVERT_ON                     0x00000001 /* RW-VF */
#define NV_PFB_DEBUG_0_CKE_ALWAYSON                           29:29 /* RWIVF */
#define NV_PFB_DEBUG_0_CKE_ALWAYSON_OFF                  0x00000000 /* RW-VF */
#define NV_PFB_DEBUG_0_CKE_ALWAYSON_ON                   0x00000001 /* RWIVF */
#define NV_PFB_DEBUG_0_SAVE_POWER                             30:30 /* RWIVF */
#define NV_PFB_DEBUG_0_SAVE_POWER_ON                     0x00000000 /* RWIVF */
#define NV_PFB_DEBUG_0_SAVE_POWER_OFF                    0x00000001 /* RW-VF */
#define NV_PFB_GREEN_0                                   0x001000C0 /* RW-4R */
#define NV_PFB_GREEN_0_LEVEL                                    1:0 /* RWIVF */
#define NV_PFB_GREEN_0_LEVEL_VIDEO_ENABLED               0x00000000 /* RW--V */
#define NV_PFB_GREEN_0_LEVEL_VIDEO_DISABLED              0x00000001 /* RW--V */
#define NV_PFB_GREEN_0_LEVEL_TIMING_DISABLED             0x00000002 /* RW--V */
#define NV_PFB_GREEN_0_LEVEL_MEMORY_DISABLED             0x00000003 /* RWI-V */
#define NV_PFB_CONFIG_0                                  0x00100200 /* RW-4R */
#define NV_PFB_CONFIG_0_RESOLUTION                              5:0 /* RWIVF */
#define NV_PFB_CONFIG_0_RESOLUTION_320_PIXELS            0x0000000a /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_400_PIXELS            0x0000000d /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_480_PIXELS            0x0000000f /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_512_PIXELS            0x00000010 /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_640_PIXELS            0x00000014 /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_800_PIXELS            0x00000019 /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_960_PIXELS            0x0000001e /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_1024_PIXELS           0x00000020 /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_1152_PIXELS           0x00000024 /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_1280_PIXELS           0x00000028 /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_1600_PIXELS           0x00000032 /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_DEFAULT               0x00000014 /* RWI-V */
#define NV_PFB_CONFIG_0_PIXEL_DEPTH                             9:8 /* RWIVF */
#define NV_PFB_CONFIG_0_PIXEL_DEPTH_8_BITS               0x00000001 /* RW--V */
#define NV_PFB_CONFIG_0_PIXEL_DEPTH_16_BITS              0x00000002 /* RW--V */
#define NV_PFB_CONFIG_0_PIXEL_DEPTH_32_BITS              0x00000003 /* RW--V */
#define NV_PFB_CONFIG_0_PIXEL_DEPTH_DEFAULT              0x00000001 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING                                12:12 /* RWIVF */
#define NV_PFB_CONFIG_0_TILING_ENABLED                   0x00000000 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DISABLED                  0x00000001 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG                          23:13 /* RWIVF */
#define NV_PFB_CONFIG_0_TILING_DEBUG_DISABLED            0x00000000 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_ON                       13:13 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_ON_ENABLED          0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_ON_DISABLED         0x00000001 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TILESIZE                 14:14 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TILESIZE_FIXED      0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TILESIZE_VARIABLE   0x00000001 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_MODE              17:15 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_MODE_PASS    0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_MODE_1       0x00000001 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_MODE_2       0x00000002 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_MODE_3       0x00000003 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_MODE_4       0x00000004 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_MODE_5       0x00000005 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_MODE_6       0x00000006 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_MODE_7       0x00000007 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_SHIFT             19:18 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_SHIFT_0      0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_SHIFT_1      0x00000001 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_TETRIS_SHIFT_2      0x00000002 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_BANK_SWAP                20:20 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_BANK_SWAP_OFF       0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_BANK_SWAP_ON        0x00000001 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_BANK_SWAP_MSB            22:21 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_BANK_SWAP_MSB_1M    0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_BANK_SWAP_MSB_2M    0x00000002 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_BANK_SWAP_MSB_4M    0x00000003 /* RW--V */
#define NV_PFB_CONFIG_0_TILING_DEBUG_UNUSED                   23:23 /* RW--V */
#define NV_PFB_CONFIG_1                                  0x00100204 /* RW-4R */
#define NV_PFB_CONFIG_1_CAS_LATENCY                             2:0 /* RWIVF */
#define NV_PFB_CONFIG_1_CAS_LATENCY_3                    0x00000003 /* RWI-V */
#define NV_PFB_CONFIG_1_SGRAM100                                3:3 /* RWIVF */
#define NV_PFB_CONFIG_1_SGRAM100_ENABLED                 0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_1_SGRAM100_DISABLED                0x00000001 /* RW--V */
#define NV_PFB_CONFIG_1_RAS_RAS                                 7:4 /* RWIUF */
#define NV_PFB_CONFIG_1_RAS_RAS_DEFAULT                  0x00000009 /* RWI-V */
#define NV_PFB_CONFIG_1_RAS_RAS_9CYCLES                  0x00000008 /* RW--V */
#define NV_PFB_CONFIG_1_RAS_RAS_SG100                    0x00000004 /* RW--V */
#define NV_PFB_CONFIG_1_RAS_PCHG                               10:8 /* RWIUF */
#define NV_PFB_CONFIG_1_RAS_PCHG_DEFAULT                 0x00000002 /* RWI-V */
#define NV_PFB_CONFIG_1_RAS_PCHG_SG100                   0x00000001 /* RW--V */
#define NV_PFB_CONFIG_1_RAS_LOW                               14:12 /* RWIUF */
#define NV_PFB_CONFIG_1_RAS_LOW_DEFAULT                  0x00000006 /* RWI-V */
#define NV_PFB_CONFIG_1_RAS_LOW_SG100                    0x00000003 /* RW--V */
#define NV_PFB_CONFIG_1_MRS_TO_RAS                            18:16 /* RWIUF */
#define NV_PFB_CONFIG_1_MRS_TO_RAS_DEFAULT               0x00000001 /* RWI-V */
#define NV_PFB_CONFIG_1_MRS_TO_RAS_SG100                 0x00000000 /* RW--V */
#define NV_PFB_CONFIG_1_WRITE_TO_READ                         22:20 /* RWIUF */
#define NV_PFB_CONFIG_1_WRITE_TO_READ_DEFAULT            0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_1_WRITE_TO_READ_SG100              0x00000000 /* RW--V */
#define NV_PFB_CONFIG_1_RAS_TO_CAS_M1                         26:24 /* RWIUF */
#define NV_PFB_CONFIG_1_RAS_TO_CAS_M1_DEFAULT            0x00000001 /* RWI-V */
#define NV_PFB_CONFIG_1_RAS_TO_CAS_M1_SG100              0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_1_READ_TO_WRITE                         30:28 /* RWIUF */
#define NV_PFB_CONFIG_1_READ_TO_WRITE_DEFAULT            0x00000004 /* RWI-V */
#define NV_PFB_CONFIG_1_READ_TO_WRITE_SG100              0x00000002 /* RWI-V */
#define NV_PFB_CONFIG_1_READ_TO_PCHG                          31:31 /* RWIUF */
#define NV_PFB_CONFIG_1_READ_TO_PCHG_ON                  0x00000001 /* RWI-V */
#define NV_PFB_CONFIG_1_READ_TO_PCHG_OFF                 0x00000000 /* RW--V */
#define NV_PFB_CONFIG_1_ED_RAC1                                 0:0 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RAC1_DEFAULT                  0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_RWT1                                 1:1 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RWT1_DEFAULT                  0x00000001 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_RPC1                                 2:2 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RPC1_DEFAULT                  0x00000001 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_PMAW                                 4:4 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_PMAW_DEFAULT                  0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_PAGE                                 5:5 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_PAGE_DEFAULT                  0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_RAC0_0                               8:8 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RAC0_0_DEFAULT                0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_RAC0_1                               9:9 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RAC0_1_DEFAULT                0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_RAC1_0                             10:10 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RAC1_0_DEFAULT                0x00000001 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_RAC1_1                             12:12 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RAC1_1_DEFAULT                0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_RAD0_0                             13:13 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RAD0_0_DEFAULT                0x00000001 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_RAD0_1                             14:14 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RAD0_1_DEFAULT                0x00000001 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_RPC0_0                             16:16 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RPC0_0_DEFAULT                0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_RPC0_1                             17:17 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RPC0_1_DEFAULT                0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_RPC1_0                             18:18 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RPC1_0_DEFAULT                0x00000001 /* RWI-V */
#define NV_PFB_CONFIG_1_ED_RPC1_1                             20:20 /* RWIUF */
#define NV_PFB_CONFIG_1_ED_RPC1_1_DEFAULT                0x00000001 /* RWI-V */
#define NV_PFB_RTL                                       0x00100300 /* RW-4R */
#define NV_PFB_RTL_S                                            1:0 /* RWIUF */
#define NV_PFB_RTL_S_DEFAULT                             0x00000002 /* RWI-V */
#define NV_PFB_RTL_V                                            5:4 /* RWIUF */
#define NV_PFB_RTL_V_DEFAULT                             0x00000002 /* RWI-V */
#define NV_PFB_RTL_M                                            9:8 /* RWIUF */
#define NV_PFB_RTL_M_DEFAULT                             0x00000002 /* RWI-V */
#define NV_PFB_RTL_H                                          13:12 /* RWIUF */
#define NV_PFB_RTL_H_DEFAULT                             0x00000001 /* RWI-V */
#define NV_PFB_RTL_A                                          17:16 /* RWIUF */
#define NV_PFB_RTL_A_DEFAULT                             0x00000001 /* RWI-V */
#define NV_PFB_RTL_G                                          21:20 /* RWIUF */
#define NV_PFB_RTL_G_DEFAULT                             0x00000001 /* RWI-V */
#define NV_PFB_RTL_ARB_GR_HI_PRIOR                            24:24 /* RWIUF */
#define NV_PFB_RTL_ARB_GR_HI_PRIOR_DEFAULT               0x00000000 /* RWI-V */
#define NV_PFB_RTL_ARB_MEDIA_HI_PRIOR                         28:28 /* RWIUF */
#define NV_PFB_RTL_ARB_MEDIA_HI_PRIOR_DEFAULT            0x00000000 /* RWI-V */
/* dev_framebuffer.ref */
#define NV_PNVM                               0x01BFFFFF:0x01800000 /* RW--M */
#define NV_PNVM_DATA032(i)                       (0x01800000+(i)*4) /* RW-4A */
#define NV_PNVM_DATA032__SIZE_1                              524288 /*       */
#define NV_PNVM_DATA032_VALUE                                  31:0 /* RWXVF */
#define NV_PNVM_DATA024(i)           (0x01800000+((i)/2)*4+((i)%2)) /* RW-3A */
#define NV_PNVM_DATA024__SIZE_1                             1048576 /*       */
#define NV_PNVM_DATA024_VALUE                                  23:0 /* RWXVF */
#define NV_PNVM_DATA016(i)           (0x01800000+((i)/3)*4+((i)%3)) /* RW-2A */
#define NV_PNVM_DATA016__SIZE_1                             1572864 /*       */
#define NV_PNVM_DATA016_VALUE                                  15:0 /* RWXVF */
#define NV_PNVM_DATA008(i)                         (0x01800000+(i)) /* RW-1A */
#define NV_PNVM_DATA008__SIZE_1                             2097152 /*       */
#define NV_PNVM_DATA008_VALUE                                   7:0 /* RWXVF */
/* dev_framebuffer.ref */
#define NV_PRAMIN                             0x01FFFFFF:0x01C00000 /* RW--M */
#define NV_PRAMIN_DATA032(i)                       (0x01C00000+(i)*4) /* RW-4A */
#define NV_PRAMIN_DATA032__SIZE_1                              524288 /*       */
#define NV_PRAMIN_DATA032_VALUE                                  31:0 /* RWXVF */
#define NV_PRAMIN_DATA024(i)           (0x01C00000+((i)/2)*4+((i)%2)) /* RW-3A */
#define NV_PRAMIN_DATA024__SIZE_1                             1048576 /*       */
#define NV_PRAMIN_DATA024_VALUE                                  23:0 /* RWXVF */
#define NV_PRAMIN_DATA016(i)           (0x01C00000+((i)/3)*4+((i)%3)) /* RW-2A */
#define NV_PRAMIN_DATA016__SIZE_1                             1572864 /*       */
#define NV_PRAMIN_DATA016_VALUE                                  15:0 /* RWXVF */
#define NV_PRAMIN_DATA008(i)                         (0x01C00000+(i)) /* RW-1A */
#define NV_PRAMIN_DATA008__SIZE_1                             2097152 /*       */
#define NV_PRAMIN_DATA008_VALUE                                   7:0 /* RWXVF */
/* dev_ram.ref */
#define NV_PRAMIN                             0x01FFFFFF:0x01C00000 /* RW--M */
#define NV_PRAMIN_CONTEXT_0                   ( 0*32+31):( 0*32+ 0) /*       */
#define NV_PRAMIN_CONTEXT_1                   ( 1*32+31):( 1*32+ 0) /*       */
#define NV_PRAMIN_CONTEXT_2                   ( 2*32+31):( 2*32+ 0) /*       */
#define NV_PRAMIN_CONTEXT_3                   ( 3*32+31):( 3*32+ 0) /*       */
#define NV_PRAMIN_RAMHT_0                     0x01400FFF:0x01400000 /* RW--M */
#define NV_PRAMIN_RAMAU_0                     0x01401BFF:0x01401000 /* RW--M */
#define NV_PRAMIN_RAMFC_0                     0x01401DFF:0x01401C00 /* RW--M */
#define NV_PRAMIN_RAMRO_0                     0x01401FFF:0x01401E00 /* RW--M */
#define NV_PRAMIN_RAMRM_0                     0x01402FFF:0x01402000 /* RW--M */
/* dev_framebuffer.ref */
#define NV_PDFB                               0x017FFFFF:0x01000000 /* RW--D */
#define NV_PDFB_DATA032(i)                       (0x01000000+(i)*4) /* RW-4A */
#define NV_PDFB_DATA032__SIZE_1                              524288 /*       */
#define NV_PDFB_DATA032_VALUE                                  31:0 /* RWXVF */
#define NV_PDFB_DATA024(i)           (0x01000000+((i)/2)*4+((i)%2)) /* RW-3A */
#define NV_PDFB_DATA024__SIZE_1                             1048576 /*       */
#define NV_PDFB_DATA024_VALUE                                  23:0 /* RWXVF */
#define NV_PDFB_DATA016(i)           (0x01000000+((i)/3)*4+((i)%3)) /* RW-2A */
#define NV_PDFB_DATA016__SIZE_1                             1572864 /*       */
#define NV_PDFB_DATA016_VALUE                                  15:0 /* RWXVF */
#define NV_PDFB_DATA008(i)                         (0x01000000+(i)) /* RW-1A */
#define NV_PDFB_DATA008__SIZE_1                             2097152 /*       */
#define NV_PDFB_DATA008_VALUE                                   7:0 /* RWXVF */
/* dev_ram.ref */
#define NV_PRAM                               0x00006FFF:0x00006000 /* RW--D */
/* dev_ext_devices.ref */
#define NV_PEXTDEV                            0x00101FFF:0x00101000 /* RW--D */
#define NV_PEXTDEV_BOOT_0                                0x00101000 /* R--4R */
#define NV_PEXTDEV_BOOT_0_STRAP_BUS_SPEED                       0:0 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_BUS_SPEED_33MHZ          0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BUS_SPEED_66MHZ          0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR                      1:1 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR_NO_BIOS       0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR_BIOS          0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE                        3:2 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_EDO             0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_SGRAM_256K      0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_RESERVED        0x00000002 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_SGRAM_512K      0x00000003 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_RAM_WIDTH                       4:4 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_RAM_WIDTH_64             0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_RAM_WIDTH_128            0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE                        5:5 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE_PCI             0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE_AGP             0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL                         6:6 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_13500K           0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_14318180         0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_TVMODE                          8:7 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_TVMODE_SECAM             0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_TVMODE_NTSC              0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_TVMODE_PAL               0x00000002 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_TVMODE_DISABLED          0x00000003 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_PCI_MODE                        9:9 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_PCI_MODE_20              0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_PCI_MODE_21              0x00000001 /* R---V */
/* dev_ext_devices.ref */
#define NV_PDAC                               0x00680FFF:0x00680000 /* RW--D */
#define NV_PDAC_DATA(i)                          (0x00500000+(i)*4) /* RW-4A */
#define NV_PDAC_DATA__SIZE_1                                     16 /*       */
#define NV_PDAC_DATA_VALUE                                      7:0 /* RW-VF */
/* dev_ext_devices.ref */
/* dev_ext_devices.ref */
#define NV_PROM                               0x0011FFFF:0x00110000 /* RW--D */
#define NV_PROM_DATA(i)                            (0x00110000+(i)) /* RW-1A */
#define NV_PROM_DATA__SIZE_1                                  65536 /*       */
#define NV_PROM_DATA_VALUE                                      7:0 /* RW-VF */
/* dev_ext_devices.ref */
#define NV_PALT                               0x0012FFFF:0x00120000 /* RW--D */
#define NV_PALT_DATA(i)                            (0x00120000+(i)) /* RW-1A */
#define NV_PALT_DATA__SIZE_1                                  65536 /*       */
#define NV_PALT_DATA_VALUE                                      7:0 /* RW-VF */
/* dev_realmode.ref */
#define NV_PRM                                0x00005FFF:0x00004000 /* RW--D */
#define NV_PRM_INTR_0                                    0x00004100 /* RW-4R */
#define NV_PRM_INTR_0_TRACE_MPU401                              0:0 /* RWXVF */
#define NV_PRM_INTR_0_TRACE_MPU401_NOT_PENDING           0x00000000 /* R---V */
#define NV_PRM_INTR_0_TRACE_MPU401_PENDING               0x00000001 /* R---V */
#define NV_PRM_INTR_0_TRACE_MPU401_RESET                 0x00000001 /* -W--V */
#define NV_PRM_INTR_0_TRACE_FM                                  4:4 /* RWXVF */
#define NV_PRM_INTR_0_TRACE_FM_NOT_PENDING               0x00000000 /* R---V */
#define NV_PRM_INTR_0_TRACE_FM_PENDING                   0x00000001 /* R---V */
#define NV_PRM_INTR_0_TRACE_FM_RESET                     0x00000001 /* -W--V */
#define NV_PRM_INTR_0_TRACE_SB_DIGITAL                          8:8 /* RWXVF */
#define NV_PRM_INTR_0_TRACE_SB_DIGITAL_NOT_PENDING       0x00000000 /* R---V */
#define NV_PRM_INTR_0_TRACE_SB_DIGITAL_PENDING           0x00000001 /* R---V */
#define NV_PRM_INTR_0_TRACE_SB_DIGITAL_RESET             0x00000001 /* -W--V */
#define NV_PRM_INTR_0_TRACE_SB_MIXER                          12:12 /* RWXVF */
#define NV_PRM_INTR_0_TRACE_SB_MIXER_NOT_PENDING         0x00000000 /* R---V */
#define NV_PRM_INTR_0_TRACE_SB_MIXER_PENDING             0x00000001 /* R---V */
#define NV_PRM_INTR_0_TRACE_SB_MIXER_RESET               0x00000001 /* -W--V */
#define NV_PRM_INTR_0_TRACE_OVERFLOW                          16:16 /* RWXVF */
#define NV_PRM_INTR_0_TRACE_OVERFLOW_NOT_PENDING         0x00000000 /* R---V */
#define NV_PRM_INTR_0_TRACE_OVERFLOW_PENDING             0x00000001 /* R---V */
#define NV_PRM_INTR_0_TRACE_OVERFLOW_RESET               0x00000001 /* -W--V */
#define NV_PRM_INTR_EN_0                                 0x00004140 /* RW-4R */
#define NV_PRM_INTR_EN_0_TRACE_MPU401                           0:0 /* RWIVF */
#define NV_PRM_INTR_EN_0_TRACE_MPU401_DISABLED           0x00000000 /* RWI-V */
#define NV_PRM_INTR_EN_0_TRACE_MPU401_ENABLED            0x00000001 /* RW--V */
#define NV_PRM_INTR_EN_0_TRACE_FM                               4:4 /* RWIVF */
#define NV_PRM_INTR_EN_0_TRACE_FM_DISABLED               0x00000000 /* RWI-V */
#define NV_PRM_INTR_EN_0_TRACE_FM_ENABLED                0x00000001 /* RW--V */
#define NV_PRM_INTR_EN_0_TRACE_SB_DIGITAL                       8:8 /* RWIVF */
#define NV_PRM_INTR_EN_0_TRACE_SB_DIGITAL_DISABLED       0x00000000 /* RWI-V */
#define NV_PRM_INTR_EN_0_TRACE_SB_DIGITAL_ENABLED        0x00000001 /* RW--V */
#define NV_PRM_INTR_EN_0_TRACE_SB_MIXER                       12:12 /* RWIVF */
#define NV_PRM_INTR_EN_0_TRACE_SB_MIXER_DISABLED         0x00000000 /* RWI-V */
#define NV_PRM_INTR_EN_0_TRACE_SB_MIXER_ENABLED          0x00000001 /* RW--V */
#define NV_PRM_INTR_EN_0_TRACE_OVERFLOW                       16:16 /* RWIVF */
#define NV_PRM_INTR_EN_0_TRACE_OVERFLOW_DISABLED         0x00000000 /* RWI-V */
#define NV_PRM_INTR_EN_0_TRACE_OVERFLOW_ENABLED          0x00000001 /* RW--V */
#define NV_PRM_RAMRM                                     0x00004200 /* RW-4R */
#define NV_PRM_RAMRM_BASE_ADDRESS                             15:12 /* RWXVF */
#define NV_PRM_RAMRM_BASE_ADDRESS_2000                   0x00002000 /* RWI-V */
#define NV_PRM_TRACE                                     0x00004300 /* RW-4R */
#define NV_PRM_TRACE_IO_CAPTURE                                 1:0 /* RWIVF */
#define NV_PRM_TRACE_IO_CAPTURE_DISABLED                 0x00000000 /* RWI-V */
#define NV_PRM_TRACE_IO_CAPTURE_WRITES                   0x00000001 /* RW--V */
#define NV_PRM_TRACE_IO_CAPTURE_READS                    0x00000002 /* RW--V */
#define NV_PRM_TRACE_IO_CAPTURE_READS_WRITES             0x00000003 /* RW--V */
#define NV_PRM_TRACE_IO_WRITE                                   4:4 /* RWXVF */
#define NV_PRM_TRACE_IO_WRITE_NONE                       0x00000000 /* R---V */
#define NV_PRM_TRACE_IO_WRITE_OCCURED                    0x00000001 /* R---V */
#define NV_PRM_TRACE_IO_WRITE_RESET                      0x00000001 /* -W--V */
#define NV_PRM_TRACE_IO_READ                                    5:5 /* RWXVF */
#define NV_PRM_TRACE_IO_READ_NONE                        0x00000000 /* R---V */
#define NV_PRM_TRACE_IO_READ_OCCURED                     0x00000001 /* R---V */
#define NV_PRM_TRACE_IO_READ_RESET                       0x00000001 /* -W--V */
#define NV_PRM_TRACE_INDEX                               0x00004310 /* RW-4R */
#define NV_PRM_TRACE_INDEX_ADDRESS                              9:0 /* RWXUF */
#define NV_PRM_TRACE_INDEX_ADDRESS_0                     0x00000000 /* RW--V */
#define NV_PRM_IGNORE_0                                  0x00004320 /* RW-4R */
#define NV_PRM_IGNORE_0_MPU401                                  1:0 /* RWXVF */
#define NV_PRM_IGNORE_0_MPU401_DISABLED                  0x00000000 /* RW--V */
#define NV_PRM_IGNORE_0_MPU401_WRITES                    0x00000001 /* RW--V */
#define NV_PRM_IGNORE_0_MPU401_READS                     0x00000002 /* RW--V */
#define NV_PRM_IGNORE_0_MPU401_READS_WRITES              0x00000003 /* RW--V */
#define NV_PRM_IGNORE_0_FM                                      5:4 /* RWXVF */
#define NV_PRM_IGNORE_0_FM_DISABLED                      0x00000000 /* RW--V */
#define NV_PRM_IGNORE_0_FM_WRITES                        0x00000001 /* RW--V */
#define NV_PRM_IGNORE_0_FM_READS                         0x00000002 /* RW--V */
#define NV_PRM_IGNORE_0_FM_READS_WRITES                  0x00000003 /* RW--V */
#define NV_PRM_IGNORE_0_SB_DIGITAL                              9:8 /* RWXVF */
#define NV_PRM_IGNORE_0_SB_DIGITAL_DISABLED              0x00000000 /* RW--V */
#define NV_PRM_IGNORE_0_SB_DIGITAL_WRITES                0x00000001 /* RW--V */
#define NV_PRM_IGNORE_0_SB_DIGITAL_READS                 0x00000002 /* RW--V */
#define NV_PRM_IGNORE_0_SB_DIGITAL_READS_WRITES          0x00000003 /* RW--V */
#define NV_PRM_IGNORE_0_SB_MIXER                              13:12 /* RWXVF */
#define NV_PRM_IGNORE_0_SB_MIXER_DISABLED                0x00000000 /* RW--V */
#define NV_PRM_IGNORE_0_SB_MIXER_WRITES                  0x00000001 /* RW--V */
#define NV_PRM_IGNORE_0_SB_MIXER_READS                   0x00000002 /* RW--V */
#define NV_PRM_IGNORE_0_SB_MIXER_READS_WRITES            0x00000003 /* RW--V */
/* dev_realmode.ref */
#define NV_PRMIO                              0x00007FFF:0x00007000 /* RW--D */
/* dev_realmode.ref */
/* dev_fifo.ref */
#define NV_USER                               0x00FFFFFF:0x00800000 /* RW--D */
#define NV_USER_OBJECT(i,j)     (0x00800000+(i)*0x10000+(j)*0x2000) /* -W-4A */
#define NV_USER_OBJECT__SIZE_1                                  128 /*       */
#define NV_USER_OBJECT__SIZE_2                                    8 /*       */
#define NV_USER_OBJECT_HANDLE                                  31:0 /* -W-VF */
#define NV_USER_FREE016(i,j)        (0x00800010+(i)*65536+(j)*8192) /* R--2A */
#define NV_USER_FREE016__SIZE_1                                 128 /*       */
#define NV_USER_FREE016__SIZE_2                                   8 /*       */
#define NV_USER_FREE016_COUNT_LO                                1:0 /* C--UF */
#define NV_USER_FREE016_COUNT_LO_0                       0x00000000 /* C---V */
#define NV_USER_FREE016_COUNT                                   9:2 /* R--UF */
#define NV_USER_FREE016_COUNT_HI                              15:10 /* C--UF */
#define NV_USER_FREE016_COUNT_HI_0                       0x00000000 /* C---V */
#define NV_USER_FREE032(i,j)        (0x00800010+(i)*65536+(j)*8192) /* R--4A */
#define NV_USER_FREE032__SIZE_1                                 128 /*       */
#define NV_USER_FREE032__SIZE_2                                   8 /*       */
#define NV_USER_FREE032_COUNT_LO                                1:0 /* C--UF */
#define NV_USER_FREE032_COUNT_LO_0                       0x00000000 /* C---V */
#define NV_USER_FREE032_COUNT                                   9:2 /* R--UF */
#define NV_USER_FREE032_COUNT_HI                              31:10 /* C--UF */
#define NV_USER_FREE032_COUNT_HI_0                       0x00000000 /* C---V */
#define NV_USER_ZERO016(i,j,k) (0x0800012+(i)*65536+(j)*8192+(k)*2) /* R--2A */
#define NV_USER_ZERO016__SIZE_1                                 128 /*       */
#define NV_USER_ZERO016__SIZE_2                                   8 /*       */
#define NV_USER_ZERO016__SIZE_3                                   7 /*       */
#define NV_USER_ZERO016_COUNT                                  15:0 /* C--UF */
#define NV_USER_ZERO016_COUNT_0                          0x00000000 /* C---V */
#define NV_USER_ZERO032(i,j,k) (0x0800014+(i)*65536+(j)*8192+(k)*4) /* R--4A */
#define NV_USER_ZERO032__SIZE_1                                 128 /*       */
#define NV_USER_ZERO032__SIZE_2                                   8 /*       */
#define NV_USER_ZERO032__SIZE_3                                   3 /*       */
#define NV_USER_ZERO032_COUNT                                  31:0 /* C--UF */
#define NV_USER_ZERO032_COUNT_0                          0x00000000 /* C---V */
/* dev_misc.ref */
#define NV_USER_ADR_CHID                                      22:16 /*       */
#define NV_USER_ADR_SUBCHID                                   15:13 /*       */
#define NV_USER_ADR_METHOD                                     12:0 /*       */
#define NV_USER_DEVICE                                        22:16 /*       */
/* dev_timer.ref */
#define NV_PTIMER                             0x00009FFF:0x00009000 /* RW--D */
#define NV_PTIMER_INTR_0                                 0x00009100 /* RW-4R */
#define NV_PTIMER_INTR_0_ALARM                                  0:0 /* RWXVF */
#define NV_PTIMER_INTR_0_ALARM_NOT_PENDING               0x00000000 /* R---V */
#define NV_PTIMER_INTR_0_ALARM_PENDING                   0x00000001 /* R---V */
#define NV_PTIMER_INTR_0_ALARM_RESET                     0x00000001 /* -W--V */
#define NV_PTIMER_INTR_EN_0                              0x00009140 /* RW-4R */
#define NV_PTIMER_INTR_EN_0_ALARM                               0:0 /* RWIVF */
#define NV_PTIMER_INTR_EN_0_ALARM_DISABLED               0x00000000 /* RWI-V */
#define NV_PTIMER_INTR_EN_0_ALARM_ENABLED                0x00000001 /* RW--V */
#define NV_PTIMER_NUMERATOR                              0x00009200 /* RW-4R */
#define NV_PTIMER_NUMERATOR_VALUE                              15:0 /* RWIUF */
#define NV_PTIMER_NUMERATOR_VALUE_0                      0x00000000 /* RWI-V */
#define NV_PTIMER_DENOMINATOR                            0x00009210 /* RW-4R */
#define NV_PTIMER_DENOMINATOR_VALUE                            15:0 /* RWIUF */
#define NV_PTIMER_DENOMINATOR_VALUE_0                    0x00000000 /* RWI-V */
#define NV_PTIMER_TIME_0                                 0x00009400 /* RW-4R */
#define NV_PTIMER_TIME_0_NSEC                                  31:5 /* RWXUF */
#define NV_PTIMER_TIME_1                                 0x00009410 /* RW-4R */
#define NV_PTIMER_TIME_1_NSEC                                  28:0 /* RWXUF */
#define NV_PTIMER_ALARM_0                                0x00009420 /* RW-4R */
#define NV_PTIMER_ALARM_0_NSEC                                 31:5 /* RWXUF */
/* dev_realmode.ref */
#define NV_TRACE                              0x0000FFFF:0x00000000 /* RW--M */
#define NV_TRACE_DATA                         ( 0*32+ 7):( 0*32+ 0) /* RWXVF */
#define NV_TRACE_ACCESS                       ( 0*32+14):( 0*32+14) /* RWXVF */
#define NV_TRACE_ACCESS_WRITE                            0x00000000 /* RW--V */
#define NV_TRACE_ACCESS_READ                             0x00000001 /* RW--V */
#define NV_TRACE_TYPE                         ( 0*32+15):( 0*32+15) /* RWXVF */
#define NV_TRACE_TYPE_IO                                 0x00000000 /* RW--V */
#define NV_TRACE_TYPE_MEMORY                             0x00000001 /* RW--V */
#define NV_TRACE_ADDRESS                      ( 0*32+31):( 0*32+16) /* RWXUF */
/* dev_ram.ref */
#define NV_RAMHT__SIZE_0                      0x00000FFF:0x00000000 /* RW--M */
#define NV_RAMHT__SIZE_1                      0x00001FFF:0x00000000 /* RW--M */
#define NV_RAMHT__SIZE_2                      0x00003FFF:0x00000000 /* RW--M */
#define NV_RAMHT__SIZE_3                      0x00007FFF:0x00000000 /* RW--M */
#define NV_RAMHT_HANDLE                       ( 0*32+31):( 0*32+ 0) /* RWXVF */
#define NV_RAMHT_INSTANCE                     ( 1*32+15):( 1*32+ 0) /* RWXUF */
#define NV_RAMHT_DEVICE                       ( 1*32+22):( 1*32+16) /* RWXUF */
#define NV_RAMHT_ENGINE                       ( 1*32+23):( 1*32+23) /* RWXVF */
#define NV_RAMHT_ENGINE_SW                               0x00000000 /* RW--V */
#define NV_RAMHT_ENGINE_GRAPHICS                         0x00000001 /* RW--V */
#define NV_RAMHT_CHID                         ( 1*32+30):( 1*32+24) /* RWXUF */
/* dev_ram.ref */
#define NV_RAMRO__SIZE_0                      0x000001FF:0x00000000 /* RW--M */
#define NV_RAMRO__SIZE_1                      0x00001FFF:0x00000000 /* RW--M */
#define NV_RAMRO_METHOD                       ( 0*32+15):( 0*32+ 0) /* RWXUF */
#define NV_RAMRO_CHID                         ( 0*32+22):( 0*32+16) /* RWXUF */
#define NV_RAMRO_TYPE                         ( 0*32+23):( 0*32+23) /* RWXVF */
#define NV_RAMRO_TYPE_WRITE                              0x00000000 /* RW--V */
#define NV_RAMRO_TYPE_READ                               0x00000001 /* RW--V */
#define NV_RAMRO_BYTE_ENABLES                 ( 0*32+27):( 0*32+24) /* RWXVF */
#define NV_RAMRO_REASON                       ( 0*32+31):( 0*32+28) /* RWXVF */
#define NV_RAMRO_REASON_ILLEGAL_ACCESS                   0x00000000 /* RW--V */
#define NV_RAMRO_REASON_NO_CACHE_AVAILABLE               0x00000001 /* RW--V */
#define NV_RAMRO_REASON_CACHE_RAN_OUT                    0x00000002 /* RW--V */
#define NV_RAMRO_REASON_FREE_COUNT_OVERRUN               0x00000003 /* RW--V */
#define NV_RAMRO_REASON_CAUGHT_LYING                     0x00000004 /* RW--V */
#define NV_RAMRO_REASON_RESERVED_ACCESS                  0x00000005 /* RW--V */
#define NV_RAMRO_DATA                         ( 1*32+31):( 1*32+ 0) /* RWXVF */
/* dev_ram.ref */
#define NV_RAMAU                              0x00000BFF:0x00000000 /* RW--M */
#define NV_RAMAU_DATA                                          31:0 /* RWXVF */
/* dev_ram.ref */
#define NV_RAMFC__SIZE_0                      0x000001FF:0x00000000 /* RW--M */
#define NV_RAMFC__SIZE_1                      0x00000FFF:0x00000000 /* RW--M */
#define NV_RAMFC_INSTANCE_0                   ( 0*32+15):( 0*32+ 0) /* RWXUF */
#define NV_RAMFC_DEVICE_0                     ( 0*32+22):( 0*32+16) /* RWXUF */
#define NV_RAMFC_DEVICE_0_NOT_FOUND                      0x00000000 /* RW--V */
#define NV_RAMFC_ENGINE_0                     ( 0*32+23):( 0*32+23) /* RWXVF */
#define NV_RAMFC_ENGINE_0_SW                             0x00000000 /* RW--V */
#define NV_RAMFC_ENGINE_0_GRAPHICS                       0x00000001 /* RW--V */
#define NV_RAMFC_INSTANCE_1                   ( 1*32+15):( 1*32+ 0) /* RWXUF */
#define NV_RAMFC_DEVICE_1                     ( 1*32+22):( 1*32+16) /* RWXUF */
#define NV_RAMFC_DEVICE_1_NOT_FOUND                      0x00000000 /* RW--V */
#define NV_RAMFC_ENGINE_1                     ( 1*32+23):( 1*32+23) /* RWXVF */
#define NV_RAMFC_ENGINE_1_SW                             0x00000000 /* RW--V */
#define NV_RAMFC_ENGINE_1_GRAPHICS                       0x00000001 /* RW--V */
#define NV_RAMFC_INSTANCE_7                   ( 7*32+15):( 7*32+ 0) /* RWXUF */
#define NV_RAMFC_DEVICE_7                     ( 7*32+22):( 7*32+16) /* RWXUF */
#define NV_RAMFC_DEVICE_7_NOT_FOUND                      0x00000000 /* RW--V */
#define NV_RAMFC_ENGINE_7                     ( 7*32+23):( 7*32+23) /* RWXVF */
#define NV_RAMFC_ENGINE_7_SW                             0x00000000 /* RW--V */
#define NV_RAMFC_ENGINE_7_GRAPHICS                       0x00000001 /* RW--V */
/* dev_ram.ref */
/* dev_ram.ref */
/* dev_ram.ref */
#define NV_DMA_ADJUST                         ( 0*32+11):( 0*32+ 0) /* RWXUF */
#define NV_DMA_PAGE_TABLE                     ( 0*32+16):( 0*32+16) /* RWXVF */
#define NV_DMA_PAGE_TABLE_NOT_PRESENT                    0x00000000 /* RW--V */
#define NV_DMA_PAGE_TABLE_PRESENT                        0x00000001 /* RW--V */
#define NV_DMA_TARGET_NODE                    ( 0*32+25):( 0*32+24) /* RWXVF */
#define NV_DMA_TARGET_NODE_NVM                           0x00000000 /* RW--V */
#define NV_DMA_TARGET_NODE_PCI                           0x00000002 /* RW--V */
#define NV_DMA_TARGET_NODE_AGP                           0x00000003 /* RW--V */
#define NV_DMA_LIMIT                          ( 1*32+31):( 1*32+ 0) /* RWXUF */
#define NV_DMA_PAGE                           ( 2*32+ 0):( 2*32+ 0) /* RWXVF */
#define NV_DMA_PAGE_NOT_PRESENT                          0x00000000 /* RW--V */
#define NV_DMA_PAGE_PRESENT                              0x00000001 /* RW--V */
#define NV_DMA_ACCESS                         ( 2*32+ 1):( 2*32+ 1) /* RWXVF */
#define NV_DMA_ACCESS_READ_ONLY                          0x00000000 /* RW--V */
#define NV_DMA_ACCESS_READ_AND_WRITE                     0x00000001 /* RW--V */
#define NV_DMA_FRAME_ADDRESS                  ( 2*32+31):( 2*32+12) /* RWXUF */
/* dev_ram.ref */
#define NV_SUBCHAN_CTX_SWITCH                 ( 0*32+31):( 0*32+ 0) /* RWXUF */
#define NV_SUBCHAN_DMA_INSTANCE               ( 1*32+15):( 1*32+ 0) /* RWXUF */
#define NV_SUBCHAN_NOTIFY_INSTANCE            ( 1*32+31):( 1*32+16) /* RWXUF */
#define NV_SUBCHAN_MEMFMT_INSTANCE            ( 2*32+15):( 2*32+ 0) /* RWXUF */
#define NV_SUBCHAN_MEMFMT_LINEAR              ( 2*32+16):( 2*32+16) /* RWXUF */
#define NV_SUBCHAN_MEMFMT_LINEAR_OUT                     0x00000000 /* RW--V */
#define NV_SUBCHAN_MEMFMT_LINEAR_IN                      0x00000001 /* RW--V */
/* dev_ram.ref */
#define NV_AUDIN                              0x00000033:0x00000000 /* RW--M */
#define NV_AUDIN_AE_STATE                       (0*32+15):(0*32+ 0) /* RWWVF */
#define NV_AUDIN_AE_SKIP_COUNT                  (1*32+31):(1*32+ 0) /* RWWUF */
#define NV_AUDIN_AE_POSITION                    (2*32+31):(2*32+ 0) /* RWWUF */
#define NV_AUDIN_RM_NEXT_INSTANCE               (3*32+31):(3*32+ 0) /* RWWUF */
#define NV_AUDIN_RM_STATE                       (4*32+15):(4*32+ 0) /* RWWVF */
#define NV_AUDIN_SU_STATE                       (5*32+15):(5*32+ 0) /* RWWVF */
#define NV_AUDIN_SU_START_TIME_LOW              (5*32+31):(5*32+16) /* RWWUF */
#define NV_AUDIN_SU_START_TIME_HIGH             (6*32+31):(6*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_SKIP_INC                    (7*32+31):(7*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_BUFF0_DMA_INSTANCE          (8*32+15):(8*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_BUFF1_DMA_INSTANCE          (8*32+31):(8*32+16) /* RWWUF */
#define NV_AUDIN_SU_BUFF0_START_POSITION        (9*32+31):(9*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_BUFF0_LAST_PLUS1          (10*32+31):(10*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_BUFF1_START_POSITION      (11*32+31):(11*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_BUFF1_LAST_PLUS1          (12*32+31):(12*32+ 0) /* RWWUF */
#define NV_AUDIN_AE_EVENT_START               ( 0*32+ 3):( 0*32+ 3) /* RWXVF */
#define NV_AUDIN_AE_BUFF0_INTR_NOTIFY         ( 0*32+ 4):( 0*32+ 4) /* RWXVF */
#define NV_AUDIN_AE_BUFF1_INTR_NOTIFY         ( 0*32+ 5):( 0*32+ 5) /* RWXVF */
#define NV_AUDIN_AE_BUFF0_INTR_CHAIN_GAP      ( 0*32+ 6):( 0*32+ 6) /* RWXVF */
#define NV_AUDIN_AE_BUFF1_INTR_CHAIN_GAP      ( 0*32+ 7):( 0*32+ 7) /* RWXVF */
#define NV_AUDIN_AE_BUFF0_IN_USE              ( 0*32+ 8):( 0*32+ 8) /* RWXVF */
#define NV_AUDIN_AE_BUFF1_IN_USE              ( 0*32+ 9):( 0*32+ 9) /* RWXVF */
#define NV_AUDIN_AE_CURRENT_BUFFER            ( 0*32+12):( 0*32+12) /* RWXVF */
#define NV_AUDIN_AE_CURRENT_BUFFER_0                     0x00000000 /* RW--V */
#define NV_AUDIN_AE_CURRENT_BUFFER_1                     0x00000001 /* RW--V */
#define NV_AUDIN_AE_INTR_DMA                  ( 0*32+11):( 0*32+11) /* RWXVF */
#define NV_AUDIN_RM_BUFF0_INTR_NOTIFY         ( 4*32+ 4):( 4*32+ 4) /* RWXVF */
#define NV_AUDIN_RM_BUFF1_INTR_NOTIFY         ( 4*32+ 5):( 4*32+ 5) /* RWXVF */
#define NV_AUDIN_RM_BUFF0_INTR_CHAIN_GAP      ( 4*32+ 6):( 4*32+ 6) /* RWXVF */
#define NV_AUDIN_RM_BUFF1_INTR_CHAIN_GAP      ( 4*32+ 7):( 4*32+ 7) /* RWXVF */
#define NV_AUDIN_SU_BUFF0_OFFSET              ( 5*32+ 1):( 5*32+ 0) /* RWXVF */
#define NV_AUDIN_SU_BUFF1_OFFSET              ( 5*32+ 3):( 5*32+ 2) /* RWXVF */
#define NV_AUDIN_SU_BYPASS_PTE                ( 5*32+ 4):( 5*32+ 4) /* RWXVF */
#define NV_AUDIN_SU_BYPASS_PTE_DISABLED                  0x00000000 /* RW--V */
#define NV_AUDIN_SU_BYPASS_PTE_ENABLED                   0x00000001 /* RW--V */
#define NV_AUDIN_SU_BYPASS_TARGET             ( 5*32+ 6):( 5*32+ 5) /* RWXVF */
#define NV_AUDIN_SU_BYPASS_TARGET_NVM                    0x00000000 /* RW--V */
#define NV_AUDIN_SU_BYPASS_TARGET_PCI                    0x00000002 /* RW--V */
#define NV_AUDIN_SU_BYPASS_TARGET_AGP                    0x00000003 /* RW--V */
#define NV_AUDIN_SU_BUFF0_IN_USE              ( 5*32+ 8):( 5*32+ 8) /* RWXVF */
#define NV_AUDIN_SU_BUFF1_IN_USE              ( 5*32+ 9):( 5*32+ 9) /* RWXVF */
#define NV_AUDIN_SU_BUFF0_NOTIFY              (16*32+10):(16*32+10) /* RWXVF */
#define NV_AUDIN_SU_BUFF0_NOTIFY_DMA_WRITE               0x00000000 /* RW--V */
#define NV_AUDIN_SU_BUFF0_NOTIFY_INTERRUPT               0x00000001 /* RW--V */
#define NV_AUDIN_SU_BUFF1_NOTIFY              (16*32+11):(16*32+11) /* RWXVF */
#define NV_AUDIN_SU_BUFF1_NOTIFY_DMA_WRITE               0x00000000 /* RW--V */
#define NV_AUDIN_SU_BUFF1_NOTIFY_INTERRUPT               0x00000001 /* RW--V */
#define NV_AUDIN_SU_CHANNEL                   ( 5*32+12):( 5*32+12) /* RWXVF */
#define NV_AUDIN_SU_CHANNEL_MONO                         0x00000000 /* RW--V */
#define NV_AUDIN_SU_CHANNEL_STEREO                       0x00000001 /* RW--V */
#define NV_AUDIN_SU_FORMAT                    ( 5*32+15):( 5*32+13) /* RWXVF */
#define NV_AUDIN_SU_FORMAT_LINEAR                        0x00000000 /* RW--V */
#define NV_AUDIN_SU_FORMAT_ULAW                          0x00000001 /* RW--V */
#define NV_AUDIN_SU_FORMAT_ALAW                          0x00000002 /* RW--V */
#define NV_AUDIN_SU_FORMAT_OFFSET8                       0x00000003 /* RW--V */
/* dev_ram.ref */
#define NV_AUDOUT                             0x0000003b:0x00000000 /* RW--M */
#define NV_AUDOUT_AE_STATE                      (0*32+15):(0*32+ 0) /* RWWVF */
#define NV_AUDOUT_AE_POSITION_LOW               (0*32+31):(0*32+16) /* RWWUF */
#define NV_AUDOUT_AE_POSITION                   (1*32+31):(1*32+ 0) /* RWWUF */
#define NV_AUDOUT_AE_VOLUME_1                   (2*32+15):(2*32+ 0) /* RWWUF */
#define NV_AUDOUT_AE_VOLUME_0                   (2*32+31):(2*32+16) /* RWWUF */
#define NV_AUDOUT_RM_VOLUME_0                   (3*32+15):(3*32+ 0) /* RWWUF */
#define NV_AUDOUT_SU_VOLUME_0                   (3*32+31):(3*32+16) /* RWWUF */
#define NV_AUDOUT_RM_VOLUME_1                   (4*32+15):(4*32+ 0) /* RWWUF */
#define NV_AUDOUT_SU_VOLUME_1                   (4*32+31):(4*32+16) /* RWWUF */
#define NV_AUDOUT_AE_POSITION_INC               (5*32+31):(5*32+ 0) /* RWWUF */
#define NV_AUDOUT_RM_NEXT_INSTANCE              (6*32+31):(6*32+ 0) /* RWWUF */
#define NV_AUDOUT_RM_STATE                      (7*32+15):(7*32+ 0) /* RWWVF */
#define NV_AUDOUT_SU_STATE                      (8*32+15):(8*32+ 0) /* RWWVF */
#define NV_AUDOUT_SU_START_TIME_LOW             (8*32+31):(8*32+16) /* RWWUF */
#define NV_AUDOUT_SU_START_TIME_HIGH            (9*32+31):(9*32+ 0) /* RWWUF */
#define NV_AUDOUT_SU_BUFF0_DMA_INSTANCE       (10*32+15):(10*32+ 0) /* RWWUF */
#define NV_AUDOUT_SU_BUFF1_DMA_INSTANCE       (10*32+31):(10*32+16) /* RWWUF */
#define NV_AUDOUT_SU_BUFF0_START_POSITION     (11*32+31):(11*32+ 0) /* RWWUF */
#define NV_AUDOUT_SU_BUFF0_LAST_PLUS1         (12*32+31):(12*32+ 0) /* RWWUF */
#define NV_AUDOUT_SU_BUFF1_START_POSITION     (13*32+31):(13*32+ 0) /* RWWUF */
#define NV_AUDOUT_SU_BUFF1_LAST_PLUS1         (14*32+31):(14*32+ 0) /* RWWUF */
#define NV_AUDOUT_AE_INTR_MIXING              ( 0*32+ 0):( 0*32+ 0) /* RWXVF */
#define NV_AUDOUT_AE_INTR_VOLUME              ( 0*32+ 1):( 0*32+ 1) /* RWXVF */
#define NV_AUDOUT_AE_EVENT_START              ( 0*32+ 3):( 0*32+ 3) /* RWXVF */
#define NV_AUDOUT_AE_BUFF0_INTR_NOTIFY        ( 0*32+ 4):( 0*32+ 4) /* RWXVF */
#define NV_AUDOUT_AE_BUFF1_INTR_NOTIFY        ( 0*32+ 5):( 0*32+ 5) /* RWXVF */
#define NV_AUDOUT_AE_BUFF0_INTR_CHAIN_GAP     ( 0*32+ 6):( 0*32+ 6) /* RWXVF */
#define NV_AUDOUT_AE_BUFF1_INTR_CHAIN_GAP     ( 0*32+ 7):( 0*32+ 7) /* RWXVF */
#define NV_AUDOUT_AE_BUFF0_IN_USE             ( 0*32+ 8):( 0*32+ 8) /* RWXVF */
#define NV_AUDOUT_AE_BUFF1_IN_USE             ( 0*32+ 9):( 0*32+ 9) /* RWXVF */
#define NV_AUDOUT_AE_GAP_DETECT               ( 0*32+10):( 0*32+10) /* RWXVF */
#define NV_AUDOUT_AE_INTR_DMA                 ( 0*32+11):( 0*32+11) /* RWXVF */
#define NV_AUDOUT_AE_CURRENT_BUFFER           ( 0*32+12):( 0*32+12) /* RWXVF */
#define NV_AUDOUT_AE_CURRENT_BUFFER_0                    0x00000000 /* RW--V */
#define NV_AUDOUT_AE_CURRENT_BUFFER_1                    0x00000001 /* RW--V */
#define NV_AUDOUT_RM_INTR_MIXING              (15*32+ 0):(15*32+ 0) /* RWXVF */
#define NV_AUDOUT_RM_INTR_VOLUME              (15*32+ 1):(15*32+ 1) /* RWXVF */
#define NV_AUDOUT_RM_BUFF0_INTR_NOTIFY        (15*32+ 4):(15*32+ 4) /* RWXVF */
#define NV_AUDOUT_RM_BUFF1_INTR_NOTIFY        (15*32+ 5):(15*32+ 5) /* RWXVF */
#define NV_AUDOUT_RM_BUFF0_INTR_CHAIN_GAP     (15*32+ 6):(15*32+ 6) /* RWXVF */
#define NV_AUDOUT_RM_BUFF1_INTR_CHAIN_GAP     (15*32+ 7):(15*32+ 7) /* RWXVF */
#define NV_AUDOUT_SU_BUFF0_OFFSET             (16*32+ 1):(16*32+ 0) /* RWXVF */
#define NV_AUDOUT_SU_BUFF1_OFFSET             (16*32+ 3):(16*32+ 2) /* RWXVF */
#define NV_AUDOUT_SU_BYPASS_PTE               (16*32+ 4):(16*32+ 4) /* RWXVF */
#define NV_AUDOUT_SU_BYPASS_PTE_DISABLED                 0x00000000 /* RW--V */
#define NV_AUDOUT_SU_BYPASS_PTE_ENABLED                  0x00000001 /* RW--V */
#define NV_AUDOUT_SU_BYPASS_TARGET            (16*32+ 6):(16*32+ 5) /* RWXVF */
#define NV_AUDOUT_SU_BYPASS_TARGET_NVM                   0x00000000 /* RW--V */
#define NV_AUDOUT_SU_BYPASS_TARGET_PCI                   0x00000002 /* RW--V */
#define NV_AUDOUT_SU_BYPASS_TARGET_AGP                   0x00000003 /* RW--V */
#define NV_AUDOUT_SU_BUFF0_IN_USE             (16*32+ 8):(16*32+ 8) /* RWXVF */
#define NV_AUDOUT_SU_BUFF1_IN_USE             (16*32+ 9):(16*32+ 9) /* RWXVF */
#define NV_AUDOUT_SU_BUFF0_NOTIFY             (16*32+10):(16*32+10) /* RWXVF */
#define NV_AUDOUT_SU_BUFF0_NOTIFY_DMA_WRITE              0x00000000 /* RW--V */
#define NV_AUDOUT_SU_BUFF0_NOTIFY_INTERRUPT              0x00000001 /* RW--V */
#define NV_AUDOUT_SU_BUFF1_NOTIFY             (16*32+11):(16*32+11) /* RWXVF */
#define NV_AUDOUT_SU_BUFF1_NOTIFY_DMA_WRITE              0x00000000 /* RW--V */
#define NV_AUDOUT_SU_BUFF1_NOTIFY_INTERRUPT              0x00000001 /* RW--V */
#define NV_AUDOUT_SU_CHANNEL                  (16*32+12):(16*32+12) /* RWXVF */
#define NV_AUDOUT_SU_CHANNEL_MONO                        0x00000000 /* RW--V */
#define NV_AUDOUT_SU_CHANNEL_STEREO                      0x00000001 /* RW--V */
#define NV_AUDOUT_SU_FORMAT                   (16*32+15):(16*32+13) /* RWXVF */
#define NV_AUDOUT_SU_FORMAT_LINEAR                       0x00000000 /* RW--V */
#define NV_AUDOUT_SU_FORMAT_ULAW                         0x00000001 /* RW--V */
#define NV_AUDOUT_SU_FORMAT_ALAW                         0x00000002 /* RW--V */
#define NV_AUDOUT_SU_FORMAT_OFFSET8                      0x00000003 /* RW--V */
#define NV_AUDOUT_SU_BUFF0_OFFSET             (16*32+ 1):(16*32+ 0) /* RWXVF */
/* dev_ram.ref */
#define NV_AUDNOTE                            0x00000083:0x00000000 /* RW--M */
#define NV_AUDNOTE_AE_STATE                     (0*32+15):(0*32+ 0) /* RWWVF */
#define NV_AUDNOTE_AE_POSITION_LOW              (0*32+31):(0*32+16) /* RWWUF */
#define NV_AUDNOTE_AE_POSITION                  (1*32+31):(1*32+ 0) /* RWWUF */
#define NV_AUDNOTE_AE_POSITION_INC              (2*32+31):(2*32+ 0) /* RWWUF */
#define NV_AUDNOTE_AE_EG_ADSR_LEVEL             (3*32+31):(3*32+ 0) /* RWWUF */
#define NV_AUDNOTE_AE_PE_ADSR_LEVEL             (4*32+31):(4*32+ 0) /* RWWUF */
#define NV_AUDNOTE_AE_ADSR_VOLUME               (5*32+15):(5*32+ 0) /* RWWUF */
#define NV_AUDNOTE_AE_VIBRATO_TREMOLO_LEVEL     (5*32+31):(5*32+16) /* RWWUF */
#define NV_AUDNOTE_AE_VIBRATO_POSITION          (6*32+31):(6*32+ 0) /* RWWUF */
#define NV_AUDNOTE_AE_TREMOLO_POSITION          (7*32+31):(7*32+ 0) /* RWWUF */
#define NV_AUDNOTE_AE_VOLUME_1                  (8*32+15):(8*32+ 0) /* RWWUF */
#define NV_AUDNOTE_AE_VOLUME_0                  (8*32+31):(8*32+16) /* RWWUF */
#define NV_AUDNOTE_RM_NEXT_INSTANCE             (9*32+31):(9*32+ 0) /* RWWUF */
#define NV_AUDNOTE_RM_VOLUME_0                (10*32+15):(10*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_VOLUME_0                (10*32+31):(10*32+16) /* RWWUF */
#define NV_AUDNOTE_RM_VOLUME_1                (11*32+15):(11*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_VOLUME_1                (11*32+31):(11*32+16) /* RWWUF */
#define NV_AUDNOTE_RM_STATE                   (12*32+15):(12*32+ 0) /* RWWVF */
#define NV_AUDNOTE_SU_STOP_TIME_LOW           (12*32+31):(12*32+16) /* RWWUF */
#define NV_AUDNOTE_SU_STATE                   (13*32+15):(13*32+ 0) /* RWWVF */
#define NV_AUDNOTE_SU_START_TIME_LOW          (13*32+31):(13*32+16) /* RWWUF */
#define NV_AUDNOTE_SU_START_TIME_HIGH         (14*32+31):(14*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_STOP_TIME_HIGH          (15*32+31):(15*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_FM_STATE                (16*32+15):(16*32+ 0) /* RWWVF */
#define NV_AUDNOTE_SU_RELEASE_TIME_LOW        (16*32+31):(16*32+16) /* RWWUF */
#define NV_AUDNOTE_SU_DMA_INSTANCE            (17*32+15):(17*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_VIBRATO_TREMOLO_TIME_LO (17*32+31):(17*32+16) /* RWWUF */
#define NV_AUDNOTE_SU_VIBRATO_TREMOLO_TIME_HI (18*32+31):(18*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_RELEASE_TIME_HIGH       (19*32+31):(19*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_POSITION_INC            (20*32+31):(20*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_LOOP_START              (21*32+31):(21*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_LOOP_END_PLUS1          (22*32+31):(22*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_LAST_PLUS1              (23*32+31):(23*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_EG_ATTACK_RATE          (24*32+15):(24*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_EG_DECAY_RATE           (24*32+31):(24*32+16) /* RWWUF */
#define NV_AUDNOTE_SU_EG_SUSTAIN_RATE         (25*32+15):(25*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_EG_RELEASE_RATE         (25*32+31):(25*32+16) /* RWWUF */
#define NV_AUDNOTE_SU_PE_ATTACK_RATE          (26*32+15):(26*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_PE_DECAY_RATE           (26*32+31):(26*32+16) /* RWWUF */
#define NV_AUDNOTE_SU_PE_SUSTAIN_RATE         (27*32+15):(27*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_PE_RELEASE_RATE         (27*32+31):(27*32+16) /* RWWUF */
#define NV_AUDNOTE_SU_VIBRATO_TREMOLO_ATTACK  (28*32+15):(28*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_VIBRATO_DEPTH           (29*32+15):(29*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_EG_SUSTAIN_LEVEL        (29*32+31):(29*32+16) /* RWWUF */
#define NV_AUDNOTE_SU_PE_SCALE                (30*32+15):(30*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_PE_SUSTAIN_LEVEL        (30*32+31):(30*32+16) /* RWWUF */
#define NV_AUDNOTE_SU_TREMOLO_POSITION_INC    (31*32+15):(31*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_VIBRATO_POSITION_INC    (31*32+31):(31*32+16) /* RWWUF */
#define NV_AUDNOTE_SU_TREMOLO_DEPTH           (32*32+15):(32*32+ 0) /* RWWUF */
#define NV_AUDNOTE_SU_TREMOLO_OFFSET          (32*32+31):(32*32+16) /* RWWUF */
#define NV_AUDNOTE_AE_INTR_MIXING             ( 0*32+ 0):( 0*32+ 0) /* RWXVF */
#define NV_AUDNOTE_AE_INTR_VOLUME             ( 0*32+ 1):( 0*32+ 1) /* RWXVF */
#define NV_AUDNOTE_AE_EVENT_STOP              ( 0*32+ 2):( 0*32+ 2) /* RWXVF */
#define NV_AUDNOTE_AE_EVENT_START             ( 0*32+ 3):( 0*32+ 3) /* RWXVF */
#define NV_AUDNOTE_AE_INTR_NOTIFY             ( 0*32+ 4):( 0*32+ 4) /* RWXVF */
#define NV_AUDNOTE_AE_PE_STATE                ( 0*32+ 7):( 0*32+ 6) /* RWXVF */
#define NV_AUDNOTE_AE_PE_ATTACK                          0x00000000 /* RW--V */
#define NV_AUDNOTE_AE_PE_DECAY                           0x00000001 /* RW--V */
#define NV_AUDNOTE_AE_PE_SUSTAIN                         0x00000002 /* RW--V */
#define NV_AUDNOTE_AE_PE_RELEASE                         0x00000003 /* RW--V */
#define NV_AUDNOTE_AE_IN_USE                  ( 0*32+ 8):( 0*32+ 8) /* RWXVF */
#define NV_AUDNOTE_AE_GAP_DETECT              ( 0*32+10):( 0*32+10) /* RWXVF */
#define NV_AUDNOTE_AE_INTR_DMA                ( 0*32+11):( 0*32+11) /* RWXVF */
#define NV_AUDNOTE_AE_EG_STATE                ( 0*32+14):( 0*32+13) /* RWXVF */
#define NV_AUDNOTE_AE_EG_ATTACK                          0x00000000 /* RW--V */
#define NV_AUDNOTE_AE_EG_DECAY                           0x00000001 /* RW--V */
#define NV_AUDNOTE_AE_EG_SUSTAIN                         0x00000002 /* RW--V */
#define NV_AUDNOTE_AE_EG_RELEASE                         0x00000003 /* RW--V */
#define NV_AUDNOTE_AE_POS_VALID               ( 0*32+15):( 0*32+15) /* RWXVF */
#define NV_AUDNOTE_RM_INTR_MIXING             (14*32+ 0):(14*32+ 0) /* RWXVF */
#define NV_AUDNOTE_RM_INTR_VOLUME             (14*32+ 1):(14*32+ 1) /* RWXVF */
#define NV_AUDNOTE_RM_INTR_NOTIFY             (14*32+ 4):(14*32+ 4) /* RWXVF */
#define NV_AUDNOTE_SU_BYPASS_PTE              (12*32+ 4):(12*32+ 4) /* RWXVF */
#define NV_AUDNOTE_SU_BYPASS_PTE_DISABLED                0x00000000 /* RW--V */
#define NV_AUDNOTE_SU_BYPASS_PTE_ENABLED                 0x00000001 /* RW--V */
#define NV_AUDNOTE_SU_BYPASS_TARGET           (12*32+ 6):(12*32+ 5) /* RWXVF */
#define NV_AUDNOTE_SU_BYPASS_TARGET_NVM                  0x00000000 /* RW--V */
#define NV_AUDNOTE_SU_BYPASS_TARGET_PCI                  0x00000002 /* RW--V */
#define NV_AUDNOTE_SU_BYPASS_TARGET_AGP                  0x00000003 /* RW--V */
#define NV_AUDNOTE_SU_EG_LINEAR               (12*32+ 7):(12*32+ 7) /* RWXVF */
#define NV_AUDNOTE_SU_EG_ATCK_NV1                        0x00000000 /* RW--V */
#define NV_AUDNOTE_SU_EG_ATCK_LINEAR                     0x00000001 /* RW--V */
#define NV_AUDNOTE_SU_IN_USE                  (12*32+ 8):(12*32+ 8) /* RWXVF */
#define NV_AUDNOTE_SU_BUFF_NOTIFY             (16*32+10):(16*32+10) /* RWXVF */
#define NV_AUDNOTE_SU_BUFF_NOTIFY_DMA_WRITE              0x00000000 /* RW--V */
#define NV_AUDNOTE_SU_BUFF_NOTIFY_INTERRUPT              0x00000001 /* RW--V */
#define NV_AUDNOTE_SU_CHANNEL                 (12*32+12):(12*32+12) /* RWXVF */
#define NV_AUDNOTE_SU_CHANNEL_MONO                       0x00000000 /* RW--V */
#define NV_AUDNOTE_SU_CHANNEL_STEREO                     0x00000001 /* RW--V */
#define NV_AUDNOTE_SU_FORMAT                  (12*32+15):(12*32+13) /* RWXVF */
#define NV_AUDNOTE_SU_FORMAT_LINEAR                      0x00000000 /* RW--V */
#define NV_AUDNOTE_SU_FORMAT_ULAW                        0x00000001 /* RW--V */
#define NV_AUDNOTE_SU_FORMAT_ALAW                        0x00000002 /* RW--V */
#define NV_AUDNOTE_SU_FORMAT_OFFSET8                     0x00000003 /* RW--V */
#define NV_AUDNOTE_FM_FEEDBACK                (15*32+ 6):(15*32+ 4) /* RWXVF */
#define NV_AUDNOTE_FM_FB_0                               0x00000000 /* RW--V */
#define NV_AUDNOTE_FM_FB_PI_16                           0x00000001 /* RW--V */
#define NV_AUDNOTE_FM_FB_PI_8                            0x00000002 /* RW--V */
#define NV_AUDNOTE_FM_FB_PI_4                            0x00000003 /* RW--V */
#define NV_AUDNOTE_FM_FB_PI_2                            0x00000004 /* RW--V */
#define NV_AUDNOTE_FM_FB_1_PI                            0x00000005 /* RW--V */
#define NV_AUDNOTE_FM_FB_2_PI                            0x00000006 /* RW--V */
#define NV_AUDNOTE_FM_FB_4_PI                            0x00000007 /* RW--V */
#define NV_AUDNOTE_FM_WAVE_SELECT             (15*32+ 2):(15*32+ 0) /* RWXVF */
#define NV_AUDNOTE_FM_WS_SINE                            0x00000000 /* RW--V */
#define NV_AUDNOTE_FM_WS_HALF_SINE                       0x00000001 /* RW--V */
#define NV_AUDNOTE_FM_WS_ABS_SINE                        0x00000002 /* RW--V */
#define NV_AUDNOTE_FM_WS_SHARK_SINE                      0x00000003 /* RW--V */
#define NV_AUDNOTE_FM_WS_QUICK_SINE                      0x00000004 /* RW--V */
#define NV_AUDNOTE_FM_WS_ABS_QUICK                       0x00000005 /* RW--V */
#define NV_AUDNOTE_FM_WS_SQUARE                          0x00000006 /* RW--V */
#define NV_AUDNOTE_FM_WS_INV_SINE                        0x00000007 /* RW--V */
#define NV_AUDNOTE_FM_PHASE_OUT               (15*32+ 8):(15*32+ 8) /* RWXVF */
#define NV_AUDNOTE_FM_SYNTHESIS               (15*32+12):(15*32+12) /* RWXVF */
#endif /* _NV_REF_H_ */

