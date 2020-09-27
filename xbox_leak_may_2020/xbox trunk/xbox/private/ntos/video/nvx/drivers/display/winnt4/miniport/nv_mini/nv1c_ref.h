
/* %W% %E% NVidia Corporation */

#ifndef _NV_REF_H_
#define _NV_REF_H_

/* dev_bus.ref */
#define NV_SPACE                              0x01FFFFFF:0x00000000 /* RW--D */
/* dev_bus.ref */
#define NV_CONFIG                             0x000001FF:0x00000000 /* RW--D */
#define NV_CONFIG_PCI_VGA_0                              0x00000000 /* R--4R */
#define NV_CONFIG_PCI_VGA_0__ALIAS_1              NV_PBUS_PCI_VGA_0 /*       */
#define NV_CONFIG_PCI_VGA_0_VENDOR_ID                          15:0 /* R--VF */
#define NV_CONFIG_PCI_VGA_0_VENDOR_ID_NVIDIA             0x000010DE /* R---V */
#define NV_CONFIG_PCI_VGA_0_VENDOR_ID_SGS                0x0000104A /* R---V */
#define NV_CONFIG_PCI_VGA_0_DEVICE_ID_FUNC                    18:16 /* C--UF */
#define NV_CONFIG_PCI_VGA_0_DEVICE_ID_FUNC_VGA           0x00000000 /* C---V */
#define NV_CONFIG_PCI_VGA_0_DEVICE_ID_FUNC_NV            0x00000001 /* ----V */
#define NV_CONFIG_PCI_VGA_0_DEVICE_ID_CHIP                    31:19 /* C--UF */
#define NV_CONFIG_PCI_VGA_0_DEVICE_ID_CHIP_NV0           0x00000000 /* ----V */
#define NV_CONFIG_PCI_VGA_0_DEVICE_ID_CHIP_NV1           0x00000001 /* C---V */
#define NV_CONFIG_PCI_VGA_0_DEVICE_ID_CHIP_NV2           0x00000002 /* ----V */
#define NV_CONFIG_PCI_NV_0                               0x00000100 /* R--4R */
#define NV_CONFIG_PCI_NV_0__ALIAS_1                NV_PBUS_PCI_NV_0 /*       */
#define NV_CONFIG_PCI_NV_0_VENDOR_ID                           15:0 /* R--VF */
#define NV_CONFIG_PCI_NV_0_VENDOR_ID_NVIDIA              0x000010DE /* R---V */
#define NV_CONFIG_PCI_NV_0_VENDOR_ID_SGS                 0x0000104A /* R---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_FUNC                     18:16 /* C--UF */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_FUNC_VGA            0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_FUNC_NV             0x00000001 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP                     31:19 /* C--UF */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV0            0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV1            0x00000001 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_CHIP_NV2            0x00000002 /* ----V */
#define NV_CONFIG_PCI_VGA_1                              0x00000004 /* RW-4R */
#define NV_CONFIG_PCI_VGA_1__ALIAS_1              NV_PBUS_PCI_VGA_1 /*       */
#define NV_CONFIG_PCI_VGA_1_IO_SPACE                            0:0 /* RWIVF */
#define NV_CONFIG_PCI_VGA_1_IO_SPACE_DISABLED            0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_VGA_1_IO_SPACE_ENABLED             0x00000001 /* RW--V */
#define NV_CONFIG_PCI_VGA_1_MEMORY_SPACE                        1:1 /* RWIVF */
#define NV_CONFIG_PCI_VGA_1_MEMORY_SPACE_DISABLED        0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_VGA_1_MEMORY_SPACE_ENABLED         0x00000001 /* RW--V */
#define NV_CONFIG_PCI_VGA_1_PALETTE_SNOOP                       5:5 /* RWIVF */
#define NV_CONFIG_PCI_VGA_1_PALETTE_SNOOP_DISABLED       0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_VGA_1_PALETTE_SNOOP_ENABLED        0x00000001 /* RW--V */
#define NV_CONFIG_PCI_VGA_1_FAST_BACK2BACK                    23:23 /* C--VF */
#define NV_CONFIG_PCI_VGA_1_FAST_BACK2BACK_INCAPABLE     0x00000000 /* ----V */
#define NV_CONFIG_PCI_VGA_1_FAST_BACK2BACK_CAPABLE       0x00000001 /* C---V */
#define NV_CONFIG_PCI_VGA_1_DEVSEL_TIMING                     26:25 /* C--VF */
#define NV_CONFIG_PCI_VGA_1_DEVSEL_TIMING_FAST           0x00000000 /* ----V */
#define NV_CONFIG_PCI_VGA_1_DEVSEL_TIMING_MEDIUM         0x00000001 /* C---V */
#define NV_CONFIG_PCI_VGA_1_DEVSEL_TIMING_SLOW           0x00000002 /* ----V */
#define NV_CONFIG_PCI_VGA_1_SIGNALED_TARGET                   27:27 /* RWIVF */
#define NV_CONFIG_PCI_VGA_1_SIGNALED_TARGET_NO_ABORT     0x00000000 /* R-I-V */
#define NV_CONFIG_PCI_VGA_1_SIGNALED_TARGET_ABORT        0x00000001 /* R---V */
#define NV_CONFIG_PCI_VGA_1_SIGNALED_TARGET_CLEAR        0x00000001 /* -W--V */
#define NV_CONFIG_PCI_NV_1                               0x00000104 /* RW-4R */
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
#define NV_CONFIG_PCI_NV_1_WRITE_AND_INVAL                      4:4 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_WRITE_AND_INVAL_DISABLED      0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_1_WRITE_AND_INVAL_ENABLED       0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_1_SERR                                 8:8 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_SERR_DISABLED                 0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_1_SERR_ENABLED                  0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_1_FAST_BACK2BACK                     23:23 /* C--VF */
#define NV_CONFIG_PCI_NV_1_FAST_BACK2BACK_INCAPABLE      0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_1_FAST_BACK2BACK_CAPABLE        0x00000001 /* C---V */
#define NV_CONFIG_PCI_NV_1_DEVSEL_TIMING                      26:25 /* C--VF */
#define NV_CONFIG_PCI_NV_1_DEVSEL_TIMING_FAST            0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_1_DEVSEL_TIMING_MEDIUM          0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_1_DEVSEL_TIMING_SLOW            0x00000002 /* ----V */
#define NV_CONFIG_PCI_NV_1_RECEIVED_TARGET                    28:28 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_RECEIVED_TARGET_NO_ABORT      0x00000000 /* R-I-V */
#define NV_CONFIG_PCI_NV_1_RECEIVED_TARGET_ABORT         0x00000001 /* R---V */
#define NV_CONFIG_PCI_NV_1_RECEIVED_TARGET_CLEAR         0x00000001 /* -W--V */
#define NV_CONFIG_PCI_NV_1_RECEIVED_MASTER                    29:29 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_RECEIVED_MASTER_NO_ABORT      0x00000000 /* R-I-V */
#define NV_CONFIG_PCI_NV_1_RECEIVED_MASTER_ABORT         0x00000001 /* R---V */
#define NV_CONFIG_PCI_NV_1_RECEIVED_MASTER_CLEAR         0x00000001 /* -W--V */
#define NV_CONFIG_PCI_NV_1_SYSTEM_ERROR                       30:30 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_SYSTEM_ERROR_NOT_SIGNALED     0x00000000 /* R-I-V */
#define NV_CONFIG_PCI_NV_1_SYSTEM_ERROR_SIGNALED         0x00000001 /* R---V */
#define NV_CONFIG_PCI_NV_1_SYSTEM_ERROR_CLEAR            0x00000001 /* -W--V */
#define NV_CONFIG_PCI_VGA_2                              0x00000008 /* R--4R */
#define NV_CONFIG_PCI_VGA_2__ALIAS_1              NV_PBUS_PCI_VGA_2 /*       */
#define NV_CONFIG_PCI_VGA_2_REVISION_ID                         7:0 /* C--UF */
#define NV_CONFIG_PCI_VGA_2_REVISION_ID_A                0x00000000 /* ----V */
#define NV_CONFIG_PCI_VGA_2_REVISION_ID_B                0x00000001 /* ----V */
#define NV_CONFIG_PCI_VGA_2_REVISION_ID_B02              0x00000002 /* ----V */
#define NV_CONFIG_PCI_VGA_2_REVISION_ID_B03              0x00000003 /* C---V */
#define NV_CONFIG_PCI_VGA_2_REVISION_ID_C01              0x00000004 /* ----V */
#define NV_CONFIG_PCI_VGA_2_CLASS_CODE                         31:8 /* C--VF */
#define NV_CONFIG_PCI_VGA_2_CLASS_CODE_VGA               0x00030000 /* C---V */
#define NV_CONFIG_PCI_VGA_2_CLASS_CODE_MULTIMEDIA        0x00048000 /* ----V */
#define NV_CONFIG_PCI_NV_2                               0x00000108 /* R--4R */
#define NV_CONFIG_PCI_NV_2__ALIAS_1                NV_PBUS_PCI_NV_2 /*       */
#define NV_CONFIG_PCI_NV_2_REVISION_ID                          7:0 /* C--UF */
#define NV_CONFIG_PCI_NV_2_REVISION_ID_A                 0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_2_REVISION_ID_B                 0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_2_REVISION_ID_B02               0x00000002 /* ----V */
#define NV_CONFIG_PCI_NV_2_REVISION_ID_B03               0x00000003 /* C---V */
#define NV_CONFIG_PCI_NV_2_REVISION_ID_C01               0x00000004 /* ----V */
#define NV_CONFIG_PCI_NV_2_CLASS_CODE                          31:8 /* C--VF */
#define NV_CONFIG_PCI_NV_2_CLASS_CODE_VGA                0x00030000 /* ----V */
#define NV_CONFIG_PCI_NV_2_CLASS_CODE_MULTIMEDIA         0x00048000 /* C---V */
#define NV_CONFIG_PCI_VGA_3                              0x0000000C /* RW-4R */
#define NV_CONFIG_PCI_VGA_3__ALIAS_1              NV_PBUS_PCI_VGA_3 /*       */
#define NV_CONFIG_PCI_VGA_3_HEADER_TYPE                       23:16 /* C--VF */
#define NV_CONFIG_PCI_VGA_3_HEADER_TYPE_MULTIFUNC        0x00000080 /* C---V */
#define NV_CONFIG_PCI_NV_3                               0x0000010C /* RW-4R */
#define NV_CONFIG_PCI_NV_3__ALIAS_1                NV_PBUS_PCI_NV_3 /*       */
#define NV_CONFIG_PCI_NV_3_CACHE_LINE_SIZE                      7:0 /* RWIUF */
#define NV_CONFIG_PCI_NV_3_CACHE_LINE_SIZE_0_DWORDS      0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER                      15:11 /* RWIUF */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_0_CLOCKS        0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_8_CLOCKS        0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_240_CLOCKS      0x0000001E /* RW--V */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_248_CLOCKS      0x0000001F /* RW--V */
#define NV_CONFIG_PCI_NV_3_HEADER_TYPE                        23:16 /* C--VF */
#define NV_CONFIG_PCI_NV_3_HEADER_TYPE_MULTIFUNC         0x00000080 /* C---V */
#define NV_CONFIG_PCI_NV_4                               0x00000110 /* RW-4R */
#define NV_CONFIG_PCI_NV_4__ALIAS_1                NV_PBUS_PCI_NV_4 /*       */
#define NV_CONFIG_PCI_NV_4_SPACE_TYPE                           0:0 /* C--VF */
#define NV_CONFIG_PCI_NV_4_SPACE_TYPE_MEMORY             0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_4_SPACE_TYPE_IO                 0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_4_ADDRESS_TYPE                         2:1 /* C--VF */
#define NV_CONFIG_PCI_NV_4_ADDRESS_TYPE_32_BIT           0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_4_ADDRESS_TYPE_20_BIT           0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_4_ADDRESS_TYPE_64_BIT           0x00000002 /* ----V */
#define NV_CONFIG_PCI_NV_4_PREFETCHABLE                         3:3 /* C--VF */
#define NV_CONFIG_PCI_NV_4_PREFETCHABLE_NOT              0x00000000 /* ----V */
#define NV_CONFIG_PCI_NV_4_PREFETCHABLE_MERGABLE         0x00000001 /* C---V */
#define NV_CONFIG_PCI_NV_4_BASE_ADDRESS                       31:25 /* RWIUF */
#define NV_CONFIG_PCI_NV_4_BASE_ADDRESS_0                0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_VGA_4(i)                   (0x00000010+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_VGA_4__SIZE_1                               6 /*       */
#define NV_CONFIG_PCI_VGA_4__ALIAS_1              NV_PBUS_PCI_VGA_4 /*       */
#define NV_CONFIG_PCI_VGA_4_RESERVED                           31:0 /* C--VF */
#define NV_CONFIG_PCI_VGA_4_RESERVED_0                   0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_5(i)                    (0x00000114+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_NV_5__SIZE_1                                5 /*       */
#define NV_CONFIG_PCI_NV_5__ALIAS_1                NV_PBUS_PCI_NV_5 /*       */
#define NV_CONFIG_PCI_NV_5_RESERVED                            31:0 /* C--VF */
#define NV_CONFIG_PCI_NV_5_RESERVED_0                    0x00000000 /* C---V */
#define NV_CONFIG_PCI_VGA_10(i)                  (0x00000028+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_VGA_10__SIZE_1                              2 /*       */
#define NV_CONFIG_PCI_VGA_10__ALIAS_1            NV_PBUS_PCI_VGA_10 /*       */
#define NV_CONFIG_PCI_VGA_10_RESERVED                          31:0 /* C--VF */
#define NV_CONFIG_PCI_VGA_10_RESERVED_0                  0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_10(i)                   (0x00000128+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_NV_10__SIZE_1                               2 /*       */
#define NV_CONFIG_PCI_NV_10__ALIAS_1              NV_PBUS_PCI_NV_10 /*       */
#define NV_CONFIG_PCI_NV_10_RESERVED                           31:0 /* C--VF */
#define NV_CONFIG_PCI_NV_10_RESERVED_0                   0x00000000 /* C---V */
#define NV_CONFIG_PCI_VGA_12                             0x00000030 /* RW-4R */
#define NV_CONFIG_PCI_VGA_12__ALIAS_1            NV_PBUS_PCI_VGA_12 /*       */
#define NV_CONFIG_PCI_VGA_12_ROM_DECODE                         0:0 /* RWIVF */
#define NV_CONFIG_PCI_VGA_12_ROM_DECODE_DISABLED         0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_VGA_12_ROM_DECODE_ENABLED          0x00000001 /* RW--V */
#define NV_CONFIG_PCI_VGA_12_ROM_BASE                         31:22 /* RWXUF */
#define NV_CONFIG_PCI_NV_12                              0x00000130 /* RW-4R */
#define NV_CONFIG_PCI_NV_12__ALIAS_1              NV_PBUS_PCI_NV_12 /*       */
#define NV_CONFIG_PCI_NV_12_ROM_DECODE                          0:0 /* RWIVF */
#define NV_CONFIG_PCI_NV_12_ROM_DECODE_DISABLED          0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_12_ROM_DECODE_ENABLED           0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_12_ROM_BASE                          31:22 /* RWXUF */
#define NV_CONFIG_PCI_VGA_13(i)                  (0x00000034+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_VGA_13__SIZE_1                              2 /*       */
#define NV_CONFIG_PCI_VGA_13__ALIAS_1            NV_PBUS_PCI_VGA_13 /*       */
#define NV_CONFIG_PCI_VGA_13_RESERVED                          31:0 /* C--VF */
#define NV_CONFIG_PCI_VGA_13_RESERVED_0                  0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_13(i)                   (0x00000134+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_NV_13__SIZE_1                               2 /*       */
#define NV_CONFIG_PCI_NV_13__ALIAS_1              NV_PBUS_PCI_NV_13 /*       */
#define NV_CONFIG_PCI_NV_13_RESERVED                           31:0 /* C--VF */
#define NV_CONFIG_PCI_NV_13_RESERVED_0                   0x00000000 /* C---V */
#define NV_CONFIG_PCI_VGA_15                             0x0000003C /* RW-4R */
#define NV_CONFIG_PCI_VGA_15__ALIAS_1            NV_PBUS_PCI_VGA_15 /*       */
#define NV_CONFIG_PCI_VGA_15_INTR_LINE                          7:0 /* RWIVF */
#define NV_CONFIG_PCI_VGA_15_INTR_LINE_IRQ0              0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_VGA_15_INTR_LINE_IRQ1              0x00000001 /* RW--V */
#define NV_CONFIG_PCI_VGA_15_INTR_LINE_IRQ15             0x0000000F /* RW--V */
#define NV_CONFIG_PCI_VGA_15_INTR_LINE_UNKNOWN           0x000000FF /* RW--V */
#define NV_CONFIG_PCI_VGA_15_INTR_PIN                          15:8 /* C--VF */
#define NV_CONFIG_PCI_VGA_15_INTR_PIN_INTA               0x00000001 /* C---V */
#define NV_CONFIG_PCI_VGA_15_MIN_GNT                          23:16 /* C--VF */
#define NV_CONFIG_PCI_VGA_15_MIN_GNT_NO_REQUIREMENTS     0x00000000 /* C---V */
#define NV_CONFIG_PCI_VGA_15_MIN_GNT_750NS               0x00000003 /* ----V */
#define NV_CONFIG_PCI_VGA_15_MAX_LAT                          31:24 /* C--VF */
#define NV_CONFIG_PCI_VGA_15_MAX_LAT_NO_REQUIREMENTS     0x00000000 /* C---V */
#define NV_CONFIG_PCI_VGA_15_MAX_LAT_250NS               0x00000001 /* ----V */
#define NV_CONFIG_PCI_NV_15                              0x0000013C /* RW-4R */
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
#define NV_CONFIG_PCI_VGA_16(i)                  (0x00000040+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_VGA_16__SIZE_1                             48 /*       */
#define NV_CONFIG_PCI_VGA_16__ALIAS_1            NV_PBUS_PCI_VGA_16 /*       */
#define NV_CONFIG_PCI_VGA_16_RESERVED                          31:0 /* C--VF */
#define NV_CONFIG_PCI_VGA_16_RESERVED_0                  0x00000000 /* C---V */
#define NV_CONFIG_PCI_NV_16(i)                   (0x00000140+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_NV_16__SIZE_1                              48 /*       */
#define NV_CONFIG_PCI_NV_16__ALIAS_1              NV_PBUS_PCI_NV_16 /*       */
#define NV_CONFIG_PCI_NV_16_RESERVED                           31:0 /* C--VF */
#define NV_CONFIG_PCI_NV_16_RESERVED_0                   0x00000000 /* C---V */
/* dev_bus.ref */
#define NV_MEMORY                             0xFFFFFFFF:0x00000000 /* RW--D */
#define NV_MEMORY_RMC_SVGA                               0x000B1E10 /* RW-4R */
#define NV_MEMORY_RMC_SVGA__ALIAS_1                NV_PBUS_RMC_SVGA /*       */
#define NV_MEMORY_RMC_ACCESS(i)                  (0x000B1E00+(i)*4) /* -W-4A */
#define NV_MEMORY_RMC_ACCESS__SIZE_1                              4 /*       */
#define NV_MEMORY_RMC_ACCESS_SECURITY                          31:0 /* -W-VF */
#define NV_MEMORY_RMC_ACCESS_SECURITY_DISABLE            0x564E6F4E /* -W--V */
#define NV_MEMORY_RMC_ACCESS_SECURITY_ENABLE             0x564E6F47 /* -W--V */
#define NV_MEMORY_RMC_WINDOW(i)                 (0x000B1E40+(i)*16) /* RW-4A */
#define NV_MEMORY_RMC_WINDOW__SIZE_1                              3 /*       */
#define NV_MEMORY_RMC_WINDOW__ALIAS_1            NV_PBUS_RMC_WINDOW /*       */
#define NV_MEMORY_BIOS_ROM008(i)                   (0x000C0000+(i)) /* RW-1A */
#define NV_MEMORY_BIOS_ROM008__SIZE_1                         32768 /*       */
#define NV_MEMORY_BIOS_ROM008_VALUE                             7:0 /* RW-VF */
#define NV_MEMORY_BIOS_ROM016(i)                 (0x000C0000+(i)*2) /* RW-2A */
#define NV_MEMORY_BIOS_ROM016__SIZE_1                         16384 /*       */
#define NV_MEMORY_BIOS_ROM016_VALUE                            15:0 /* RW-VF */
#define NV_MEMORY_BIOS_ROM032(i)                 (0x000C0000+(i)*4) /* RW-4A */
#define NV_MEMORY_BIOS_ROM032__SIZE_1                          8192 /*       */
#define NV_MEMORY_BIOS_ROM032_VALUE                            31:0 /* RW-VF */
#define NV_MEMORY_BIOS_RAM008(i)                   (0x000B1000+(i)) /* RW-1A */
#define NV_MEMORY_BIOS_RAM008__SIZE_1                          3584 /*       */
#define NV_MEMORY_BIOS_RAM008_VALUE                             7:0 /* RW-VF */
#define NV_MEMORY_BIOS_RAM016(i)                 (0x000B1000+(i)*2) /* RW-2A */
#define NV_MEMORY_BIOS_RAM016__SIZE_1                          1792 /*       */
#define NV_MEMORY_BIOS_RAM016_VALUE                            15:0 /* RW-VF */
#define NV_MEMORY_BIOS_RAM032(i)                 (0x000B1000+(i)*4) /* RW-4A */
#define NV_MEMORY_BIOS_RAM032__SIZE_1                           896 /*       */
#define NV_MEMORY_BIOS_RAM032_VALUE                            31:0 /* RW-VF */
#define NV_MEMORY_WINDOW008(i,j)          (0x000B2000+(i)*8192+(j)) /* RW-1A */
#define NV_MEMORY_WINDOW008__SIZE_1                               3 /*       */
#define NV_MEMORY_WINDOW008__SIZE_2                            8192 /*       */
#define NV_MEMORY_WINDOW008_VALUE                               7:0 /* RW-VF */
#define NV_MEMORY_WINDOW016(i,j)        (0x000B2000+(i)*8192+(j)*2) /* RW-2A */
#define NV_MEMORY_WINDOW016__SIZE_1                               3 /*       */
#define NV_MEMORY_WINDOW016__SIZE_2                            4096 /*       */
#define NV_MEMORY_WINDOW016_VALUE                              15:0 /* RW-VF */
#define NV_MEMORY_WINDOW032(i,j)        (0x000B2000+(i)*8192+(j)*4) /* RW-4A */
#define NV_MEMORY_WINDOW032__SIZE_1                               3 /*       */
#define NV_MEMORY_WINDOW032__SIZE_2                            2048 /*       */
#define NV_MEMORY_WINDOW032_VALUE                              31:0 /* RW-VF */
#define NV_MEMORY_RMC_VL                                 0x000B1E90 /* RW-4R */
#define NV_MEMORY_RMC_VL__ALIAS_1                    NV_PBUS_RMC_VL /*       */
#define NV_MEMORY_RMC_DMA_0                              0x000B1E80 /* RW-4R */
#define NV_MEMORY_RMC_DMA_0__ALIAS_1              NV_PBUS_RMC_DMA_0 /*       */
#define NV_MEMORY_RMC_DMA_1                              0x000B1E84 /* RW-4R */
#define NV_MEMORY_RMC_DMA_1__ALIAS_1              NV_PBUS_RMC_DMA_1 /*       */
/* dev_realmode.ref */
#define NV_MEMORY_TRACE                                  0x000B1F00 /* RW-4R */
#define NV_MEMORY_TRACE__ALIAS_1                       NV_PRM_TRACE /*       */
#define NV_MEMORY_TRACE_INDEX                            0x000B1F10 /* RW-4R */
#define NV_MEMORY_TRACE_INDEX__ALIAS_1           NV_PRM_TRACE_INDEX /*       */
#define NV_MEMORY_IGNORE_0                               0x000B1F20 /* RW-4R */
#define NV_MEMORY_IGNORE_0__ALIAS_1                    NV_PRM_TRACE /*       */
#define NV_MEMORY_IGNORE_1                               0x000B1F24 /* RW-4R */
#define NV_MEMORY_IGNORE_1__ALIAS_1                    NV_PRM_TRACE /*       */
/* dev_bus.ref */
#define NV_IO                                 0xFFFFFFFF:0x00000000 /* RW--D */
#define NV_IO_VSE_03C3                                   0x000003C3 /* RW-1R */
#define NV_IO_VSE_03C3_ENABLE                                   0:0 /* RW-VF */
#define NV_IO_VSE_03C3_ENABLE_DISABLED                   0x00000000 /* RW--V */
#define NV_IO_VSE_03C3_ENABLE_ENABLED                    0x00000001 /* RW--V */
#define NV_IO_VSE_46E8                                   0x000046E8 /* RW-1R */
#define NV_IO_VSE_46E8_ENABLE                                   3:3 /* RW-VF */
#define NV_IO_VSE_46E8_ENABLE_DISABLED                   0x00000000 /* RW--V */
#define NV_IO_VSE_46E8_ENABLE_ENABLED                    0x00000001 /* RW--V */
#define NV_IO_VSE_46E8_SETUP                                    4:4 /* RW-VF */
#define NV_IO_VSE_46E8_SETUP_DISABLED                    0x00000000 /* RW--V */
#define NV_IO_VSE_46E8_SETUP_ENABLED                     0x00000001 /* RW--V */
#define NV_IO_POS_0102                                   0x00000102 /* RW-1R */
#define NV_IO_POS_0102_ENABLE                                   0:0 /* RW-VF */
#define NV_IO_POS_0102_ENABLE_DISABLED                   0x00000000 /* RW--V */
#define NV_IO_POS_0102_ENABLE_ENABLED                    0x00000001 /* RW--V */
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
/* dev_realmode.ref */
#define NV_IO_GAME_PORT                                  0x00000201 /* RW-1R */
#define NV_IO_GAME_PORT__ALIAS_1                 NV_PRMIO_GAME_PORT /*       */
#define NV_IO_GAME_PORT_DATA                                    7:0 /* RW-VF */
#define NV_IO_FM_ADDRESS__SOUND_BLASTER                  0x00000228 /* -W-1R */
#define NV_IO_FM_ADDRESS__ADLIB                          0x00000388 /* -W-1R */
#define NV_IO_RM_ADDRESS_INDEX                                  7:0 /* -WIUF */
#define NV_IO_RM_ADDRESS_INDEX_0                         0x00000000 /* -WI-V */
#define NV_IO_SB_WRITE_STATUS                            0x0000022C /* R--1R */
#define NV_IO_SB_WRITE_STATUS__ALIAS_1     NV_PRMIO_SB_WRITE_STATUS /*       */
#define NV_IO_SB_WRITE_STATUS_STATE                             7:7 /* R-IVF */
#define NV_IO_SB_WRITE_STATUS_STATE_EMPTY                0x00000000 /* R-I-V */
#define NV_IO_SB_WRITE_STATUS_STATE_FULL                 0x00000001 /* R---V */
#define NV_IO_SB_READ_STATUS                             0x0000022E /* RW-1R */
#define NV_IO_SB_READ_STATUS__ALIAS_1       NV_PRMIO_SB_READ_STATUS /*       */
#define NV_IO_SB_READ_STATUS_STATE                              7:7 /* RWIVF */
#define NV_IO_SB_READ_STATUS_STATE_EMPTY                 0x00000000 /* RWI-V */
#define NV_IO_SB_READ_STATUS_STATE_FULL                  0x00000001 /* RW--V */
#define NV_IO_MPU_401_DATA                               0x00000330 /* R--1R */
#define NV_IO_MPU_401_DATA__ALIAS_1           NV_PRMIO_MPU_401_DATA /*       */
#define NV_IO_MPU_401_DATA_VALUE                                7:0 /* R--VF */
#define NV_IO_MPU_401_DATA_ACK                           0x000000FE /* R---V */
#define NV_IO_MPU_401_STATUS                             0x00000331 /* R--1R */
#define NV_IO_MPU_401_STATUS__ALIAS_1       NV_PRMIO_MPU_401_STATUS /*       */
#define NV_IO_MPU_401_STATUS_DATA                               5:0 /* R--VF */
#define NV_IO_MPU_401_STATUS_WRITE                              6:6 /* R--VF */
#define NV_IO_MPU_401_STATUS_WRITE_EMPTY                 0x00000000 /* R---V */
#define NV_IO_MPU_401_STATUS_WRITE_FULL                  0x00000001 /* R---V */
#define NV_IO_MPU_401_STATUS_READ                               7:7 /* R--VF */
#define NV_IO_MPU_401_STATUS_READ_FULL                   0x00000000 /* R---V */
#define NV_IO_MPU_401_STATUS_READ_EMPTY                  0x00000001 /* R---V */
#define NV_IO_MISC_OUT__WRITE                            0x000003C2 /* -W-1R */
#define NV_IO_MISC_OUT__READ                             0x000003CC /* R--1R */
#define NV_IO_EX_MISC_OUT_IO_ADDRESS                            0:0 /* RWIVF */
#define NV_IO_EX_MISC_OUT_IO_ADDRESS_MONO                0x00000000 /* RWI-V */
#define NV_IO_EX_MISC_OUT_IO_ADDRESS_COLOR               0x00000001 /* RW--V */
#define NV_IO_EX_MISC_OUT_RAM_ACCESS                            1:1 /* RWIVF */
#define NV_IO_EX_MISC_OUT_RAM_ACCESS_DISABLED            0x00000000 /* RWI-V */
#define NV_IO_EX_MISC_OUT_RAM_ACCESS_ENABLED             0x00000001 /* RW--V */
#define NV_IO_EX_MISC_OUT_CLOCK_SELECT                          3:2 /* RWIVF */
#define NV_IO_EX_MISC_OUT_CLOCK_SELECT_25MHZ             0x00000000 /* RWI-V */
#define NV_IO_EX_MISC_OUT_CLOCK_SELECT_28MHZ             0x00000001 /* RW--V */
#define NV_IO_EX_MISC_OUT_OE_PAGE                               5:5 /* RWIVF */
#define NV_IO_EX_MISC_OUT_OE_PAGE_LOW_64K                0x00000000 /* RWI-V */
#define NV_IO_EX_MISC_OUT_OE_PAGE_HIGH_64K               0x00000001 /* RW--V */
#define NV_IO_EX_MISC_OUT_HSYNC_POLARITY                        6:6 /* RWIVF */
#define NV_IO_EX_MISC_OUT_HSYNC_POLARITY_POSITIVE        0x00000000 /* RWI-V */
#define NV_IO_EX_MISC_OUT_HSYNC_POLARITY_NEGATIVE        0x00000001 /* RW--V */
#define NV_IO_EX_MISC_OUT_VSYNC_POLARITY                        7:7 /* RWIVF */
#define NV_IO_EX_MISC_OUT_VSYNC_POLARITY_POSITIVE        0x00000000 /* RWI-V */
#define NV_IO_EX_MISC_OUT_VSYNC_POLARITY_NEGATIVE        0x00000001 /* RW--V */
#define NV_IO_EX_FEATURE__WRITE_MONO                     0x000003BA /* -W-1R */
#define NV_IO_EX_FEATURE__WRITE_COLOR                    0x000003DA /* -W-1R */
#define NV_IO_EX_FEATURE__READ                           0x000003CA /* R--1R */
#define NV_IO_EX_FEATURE_CONTROL_0                              0:0 /* C--VF */
#define NV_IO_EX_FEATURE_CONTROL_0_0                     0x00000000 /* C---V */
#define NV_IO_EX_FEATURE_CONTROL_1                              1:1 /* C--VF */
#define NV_IO_EX_FEATURE_CONTROL_1_0                     0x00000000 /* C---V */
#define NV_IO_EX_FEATURE_VSYNC_SELECT                           7:7 /* RWIVF */
#define NV_IO_EX_FEATURE_VSYNC_SELECT_ONLY               0x00000000 /* RWI-V */
#define NV_IO_EX_FEATURE_VSYNC_SELECT_OR_DISPLAY         0x00000001 /* RW--V */
#define NV_IO_EX_STATUS_0__READ                          0x000003C2 /* R--1R */
#define NV_IO_EX_STATUS_0_SENSE_CODE                            4:4 /* RWIVF */
#define NV_IO_EX_STATUS_0_SENSE_CODE_OFF                 0x00000000 /* RW--V */
#define NV_IO_EX_STATUS_0_SENSE_CODE_ON                  0x00000001 /* RWI-V */
#define NV_IO_EX_STATUS_0_FEATURE_0                             5:5 /* C--VF */
#define NV_IO_EX_STATUS_0_FEATURE_0_0                    0x00000000 /* C---V */
#define NV_IO_EX_STATUS_0_FEATURE_1                             6:6 /* C--VF */
#define NV_IO_EX_STATUS_0_FEATURE_1_0                    0x00000000 /* C---V */
#define NV_IO_EX_STATUS_0_VTRACE_INTR                           7:7 /* R-IVF */
#define NV_IO_EX_STATUS_0_VTRACE_INTR_VBLANK             0x00000000 /* R-I-V */
#define NV_IO_EX_STATUS_0_VTRACE_INTR_DISPLAY            0x00000001 /* R---V */
#define NV_IO_EX_STATUS_1__READ_MONO                     0x000003BA /* R--1R */
#define NV_IO_EX_STATUS_1__READ_COLOR                    0x000003DA /* R--1R */
#define NV_IO_EX_STATUS_1_BLANK                                 0:0 /* R-XVF */
#define NV_IO_EX_STATUS_1_BLANK_DISPLAY                  0x00000000 /* R---V */
#define NV_IO_EX_STATUS_1_BLANK_HORZ_VERT                0x00000001 /* R---V */
#define NV_IO_EX_STATUS_1_LPEN_STROBE                           1:1 /* C--VF */
#define NV_IO_EX_STATUS_1_LPEN_STROBE_0                  0x00000000 /* C---V */
#define NV_IO_EX_STATUS_1_LPEN_SWITCH                           2:2 /* C--VF */
#define NV_IO_EX_STATUS_1_LPEN_SWITCH_0                  0x00000001 /* C---V */
#define NV_IO_EX_STATUS_1_VSYNC                                 3:3 /* R-IVF */
#define NV_IO_EX_STATUS_1_VSYNC_OUTSIDE                  0x00000000 /* R-I-V */
#define NV_IO_EX_STATUS_1_VSYNC_INSIDE                   0x00000001 /* R---V */
#define NV_IO_EX_STATUS_1_DIAGNOSTICS                           5:4 /* C--VF */
#define NV_IO_EX_STATUS_1_DIAGNOSTICS_0                  0x00000000 /* C---V */
#define NV_IO_SQ_ADDRESS                                 0x000003C4 /* RW-1R */
#define NV_IO_SQ_ADDRESS__ALIAS_1               NV_PRMIO_SQ_ADDRESS /*       */
#define NV_IO_SQ_ADDRESS_INDEX                                  2:0 /* RWIUF */
#define NV_IO_SQ_ADDRESS_INDEX_0                         0x00000000 /* RWI-V */
#define NV_IO_SQ_MAP_MASK                                0x000003C5 /* RW-1R */
#define NV_IO_SQ_MAP_MASK__ALIAS_1             NV_PRMIO_SQ_MAP_MASK /*       */
#define NV_IO_SQ_MAP_MASK__INDEX                         0x00000002 /*       */
#define NV_IO_SQ_MAP_MASK_PLANE_MASK_0                          0:0 /* RWIVF */
#define NV_IO_SQ_MAP_MASK_PLANE_MASK_0_DONT_WRITE        0x00000000 /* RWI-V */
#define NV_IO_SQ_MAP_MASK_PLANE_MASK_0_WRITE             0x00000001 /* RW--V */
#define NV_IO_SQ_MAP_MASK_PLANE_MASK_1                          1:1 /* RWIVF */
#define NV_IO_SQ_MAP_MASK_PLANE_MASK_1_DONT_WRITE        0x00000000 /* RWI-V */
#define NV_IO_SQ_MAP_MASK_PLANE_MASK_1_WRITE             0x00000001 /* RW--V */
#define NV_IO_SQ_MAP_MASK_PLANE_MASK_2                          2:2 /* RWIVF */
#define NV_IO_SQ_MAP_MASK_PLANE_MASK_2_DONT_WRITE        0x00000000 /* RWI-V */
#define NV_IO_SQ_MAP_MASK_PLANE_MASK_2_WRITE             0x00000001 /* RW--V */
#define NV_IO_SQ_MAP_MASK_PLANE_MASK_3                          3:3 /* RWIVF */
#define NV_IO_SQ_MAP_MASK_PLANE_MASK_3_DONT_WRITE        0x00000000 /* RWI-V */
#define NV_IO_SQ_MAP_MASK_PLANE_MASK_3_WRITE             0x00000001 /* RW--V */
#define NV_IO_SQ_CHAR_MAP                                0x000003C5 /* RW-1R */
#define NV_IO_SQ_CHAR_MAP__ALIAS_1             NV_PRMIO_SQ_CHAR_MAP /*       */
#define NV_IO_SQ_CHAR_MAP__INDEX                         0x00000003 /*       */
#define NV_IO_SQ_CHAR_MAP_FONT_0_LO                             1:0 /* RWXVF */
#define NV_IO_SQ_CHAR_MAP_FONT_0_LO_0K                   0x00000000 /* RW--V */
#define NV_IO_SQ_CHAR_MAP_FONT_0_LO_16K                  0x00000001 /* RW--V */
#define NV_IO_SQ_CHAR_MAP_FONT_0_LO_32K                  0x00000002 /* RW--V */
#define NV_IO_SQ_CHAR_MAP_FONT_0_LO_48K                  0x00000003 /* RW--V */
#define NV_IO_SQ_CHAR_MAP_FONT_1_LO                             3:2 /* RWXVF */
#define NV_IO_SQ_CHAR_MAP_FONT_1_LO_0K                   0x00000000 /* RW--V */
#define NV_IO_SQ_CHAR_MAP_FONT_1_LO_16K                  0x00000001 /* RW--V */
#define NV_IO_SQ_CHAR_MAP_FONT_1_LO_32K                  0x00000002 /* RW--V */
#define NV_IO_SQ_CHAR_MAP_FONT_1_LO_48K                  0x00000003 /* RW--V */
#define NV_IO_SQ_CHAR_MAP_FONT_0_HI                             4:4 /* RWXVF */
#define NV_IO_SQ_CHAR_MAP_FONT_0_HI_PLUS_0K              0x00000000 /* RW--V */
#define NV_IO_SQ_CHAR_MAP_FONT_0_HI_PLUS_16K             0x00000001 /* RW--V */
#define NV_IO_SQ_CHAR_MAP_FONT_1_HI                             5:5 /* RWXVF */
#define NV_IO_SQ_CHAR_MAP_FONT_1_HI_PLUS_0K              0x00000000 /* RW--V */
#define NV_IO_SQ_CHAR_MAP_FONT_1_HI_PLUS_16K             0x00000001 /* RW--V */
#define NV_IO_SQ_MEM_MAP                                 0x000003C5 /* RW-1R */
#define NV_IO_SQ_MEM_MAP__ALIAS_1               NV_PRMIO_SQ_MEM_MAP /*       */
#define NV_IO_SQ_MEM_MAP__INDEX                          0x00000004 /*       */
#define NV_IO_SQ_MEM_MAP_ALPHA_GRAPH                            0:0 /* C--VF */
#define NV_IO_SQ_MEM_MAP_ALPHA_GRAPH_0                   0x00000000 /* C---V */
#define NV_IO_SQ_MEM_MAP_EXT_MEMORY                             1:1 /* C--VF */
#define NV_IO_SQ_MEM_MAP_EXT_MEMORY_64K                  0x00000001 /* C---V */
#define NV_IO_SQ_MEM_MAP_ODD_EVEN                               2:2 /* RWIVF */
#define NV_IO_SQ_MEM_MAP_ODD_EVEN_ODD_EVEN               0x00000000 /* RWI-V */
#define NV_IO_SQ_MEM_MAP_ODD_EVEN_SEQUENTIAL             0x00000001 /* RW--V */
#define NV_IO_SQ_MEM_MAP_CHAIN_FOUR                             3:3 /* RWIVF */
#define NV_IO_SQ_MEM_MAP_CHAIN_FOUR_PLANAR               0x00000000 /* RWI-V */
#define NV_IO_SQ_MEM_MAP_CHAIN_FOUR_PIXEL                0x00000001 /* RW--V */
#define NV_IO_CC_ADDRESS__MONO                           0x000003B4 /* RW-1R */
#define NV_IO_CC_ADDRESS__COLOR                          0x000003D4 /* RW-1R */
#define NV_IO_CC_ADDRESS_INDEX                                  5:0 /* RWIUF */
#define NV_IO_CC_ADDRESS_INDEX_0                         0x00000000 /* RWI-V */
#define NV_IO_CC_MAX_SCAN__MONO                          0x000003B5 /* RW-1R */
#define NV_IO_CC_MAX_SCAN__COLOR                         0x000003D5 /* RW-1R */
#define NV_IO_CC_MAX_SCAN__INDEX                         0x00000009 /*       */
#define NV_IO_CC_MAX_SCAN_CHAR_HEIGHT                           4:0 /* RWXUF */
#define NV_IO_CC_MAX_SCAN_2T4                                   5:5 /* RWXVF */
#define NV_IO_CC_MAX_SCAN_LINE_CMP9                             6:6 /* RWXVF */
#define NV_IO_CC_MAX_SCAN_VBS9                                  7:7 /* RWXVF */
#define NV_IO_CC_CURSOR_START__MONO                      0x000003B5 /* RW-1R */
#define NV_IO_CC_CURSOR_START__COLOR                     0x000003D5 /* RW-1R */
#define NV_IO_CC_CURSOR_START__INDEX                     0x0000000A /*       */
#define NV_IO_CC_CURSOR_START_START                             4:0 /* RWXUF */
#define NV_IO_CC_CURSOR_START_DISABLE                           5:5 /* RWXVF */
#define NV_IO_CC_CURSOR_START_DISABLE_CURSOR_ON          0x00000000 /* RW--V */
#define NV_IO_CC_CURSOR_START_DISABLE_CURSOR_OFF         0x00000001 /* RW--V */
#define NV_IO_CC_START_HI__MONO                          0x000003B5 /* RW-1R */
#define NV_IO_CC_START_HI__COLOR                         0x000003D5 /* RW-1R */
#define NV_IO_CC_START_HI__INDEX                         0x0000000C /*       */
#define NV_IO_CC_START_HI_ADDRESS                               7:0 /* RWXUF */
#define NV_IO_CC_START_LO__MONO                          0x000003B5 /* RW-1R */
#define NV_IO_CC_START_LO__COLOR                         0x000003D5 /* RW-1R */
#define NV_IO_CC_START_LO__INDEX                         0x0000000D /*       */
#define NV_IO_CC_START_LO_ADDRESS                               7:0 /* RWXUF */
#define NV_IO_CC_CURSOR_HI__MONO                         0x000003B5 /* RW-1R */
#define NV_IO_CC_CURSOR_HI__COLOR                        0x000003D5 /* RW-1R */
#define NV_IO_CC_CURSOR_HI__INDEX                        0x0000000E /*       */
#define NV_IO_CC_CURSOR_HI_LOCATION                             7:0 /* RWXUF */
#define NV_IO_CC_CURSOR_LO__MONO                         0x000003B5 /* RW-1R */
#define NV_IO_CC_CURSOR_LO__COLOR                        0x000003D5 /* RW-1R */
#define NV_IO_CC_CURSOR_LO__INDEX                        0x0000000F /*       */
#define NV_IO_CC_CURSOR_LO_LOCATION                             7:0 /* RWXUF */
#define NV_IO_CC_OFFSET__MONO                            0x000003B5 /* RW-1R */
#define NV_IO_CC_OFFSET__COLOR                           0x000003D5 /* RW-1R */
#define NV_IO_CC_OFFSET__INDEX                           0x00000013 /*       */
#define NV_IO_CC_OFFSET_PITCH                                   7:0 /* RWXUF */
#define NV_IO_GC_ADDRESS                                 0x000003CE /* RW-1R */
#define NV_IO_GC_ADDRESS__ALIAS_1               NV_PRMIO_GC_ADDRESS /*       */
#define NV_IO_GC_ADDRESS_INDEX                                  3:0 /* RWIUF */
#define NV_IO_GC_ADDRESS_INDEX_0                         0x00000000 /* RWI-V */
#define NV_IO_GC_SR                                      0x000003CF /* RW-1R */
#define NV_IO_GC_SR__ALIAS_1                         NV_PRMIO_GC_SR /*       */
#define NV_IO_GC_SR__INDEX                               0x00000000 /*       */
#define NV_IO_GC_SR_VALUE_0                                     0:0 /* RWIVF */
#define NV_IO_GC_SR_VALUE_0_RESET_PLANE                  0x00000000 /* RWI-V */
#define NV_IO_GC_SR_VALUE_0_SET_PLANE                    0x00000001 /* RW--V */
#define NV_IO_GC_SR_VALUE_1                                     1:1 /* RWIVF */
#define NV_IO_GC_SR_VALUE_1_RESET_PLANE                  0x00000000 /* RWI-V */
#define NV_IO_GC_SR_VALUE_1_SET_PLANE                    0x00000001 /* RW--V */
#define NV_IO_GC_SR_VALUE_2                                     2:2 /* RWIVF */
#define NV_IO_GC_SR_VALUE_2_RESET_PLANE                  0x00000000 /* RWI-V */
#define NV_IO_GC_SR_VALUE_2_SET_PLANE                    0x00000001 /* RW--V */
#define NV_IO_GC_SR_VALUE_3                                     3:3 /* RWIVF */
#define NV_IO_GC_SR_VALUE_3_RESET_PLANE                  0x00000000 /* RWI-V */
#define NV_IO_GC_SR_VALUE_3_SET_PLANE                    0x00000001 /* RW--V */
#define NV_IO_GC_ENABLE_SR                               0x000003CF /* RW-1R */
#define NV_IO_GC_ENABLE_SR__ALIAS_1           NV_PRMIO_GC_ENABLE_SR /*       */
#define NV_IO_GC_ENABLE_SR__INDEX                        0x00000001 /*       */
#define NV_IO_GC_ENABLE_SR_ENABLE_0                             0:0 /* RWIVF */
#define NV_IO_GC_ENABLE_SR_ENABLE_0_CPU_DATA             0x00000000 /* RWI-V */
#define NV_IO_GC_ENABLE_SR_ENABLE_0_GC_SR_VALUE          0x00000001 /* RW--V */
#define NV_IO_GC_ENABLE_SR_ENABLE_1                             1:1 /* RWIVF */
#define NV_IO_GC_ENABLE_SR_ENABLE_1_CPU_DATA             0x00000000 /* RWI-V */
#define NV_IO_GC_ENABLE_SR_ENABLE_1_GC_SR_VALUE          0x00000001 /* RW--V */
#define NV_IO_GC_ENABLE_SR_ENABLE_2                             2:2 /* RWIVF */
#define NV_IO_GC_ENABLE_SR_ENABLE_2_CPU_DATA             0x00000000 /* RWI-V */
#define NV_IO_GC_ENABLE_SR_ENABLE_2_GC_SR_VALUE          0x00000001 /* RW--V */
#define NV_IO_GC_ENABLE_SR_ENABLE_3                             3:3 /* RWIVF */
#define NV_IO_GC_ENABLE_SR_ENABLE_3_CPU_DATA             0x00000000 /* RWI-V */
#define NV_IO_GC_ENABLE_SR_ENABLE_3_GC_SR_VALUE          0x00000001 /* RW--V */
#define NV_IO_GC_COLOR_COMP                              0x000003CF /* RW-1R */
#define NV_IO_GC_COLOR_COMP__ALIAS_1         NV_PRMIO_GC_COLOR_COMP /*       */
#define NV_IO_GC_COLOR_COMP__INDEX                       0x00000002 /*       */
#define NV_IO_GC_COLOR_COMP_COLOR_0                             0:0 /* RWIVF */
#define NV_IO_GC_COLOR_COMP_COLOR_0_0                    0x00000000 /* RWI-V */
#define NV_IO_GC_COLOR_COMP_COLOR_1                             1:1 /* RWIVF */
#define NV_IO_GC_COLOR_COMP_COLOR_1_0                    0x00000000 /* RWI-V */
#define NV_IO_GC_COLOR_COMP_COLOR_2                             2:2 /* RWIVF */
#define NV_IO_GC_COLOR_COMP_COLOR_2_0                    0x00000000 /* RWI-V */
#define NV_IO_GC_COLOR_COMP_COLOR_3                             3:3 /* RWIVF */
#define NV_IO_GC_COLOR_COMP_COLOR_3_0                    0x00000000 /* RWI-V */
#define NV_IO_GC_ROTATE                                  0x000003CF /* RW-1R */
#define NV_IO_GC_ROTATE__ALIAS_1                 NV_PRMIO_GC_ROTATE /*       */
#define NV_IO_GC_ROTATE__INDEX                           0x00000003 /*       */
#define NV_IO_GC_ROTATE_RIGHT                                   2:0 /* RWIVF */
#define NV_IO_GC_ROTATE_RIGHT_0                          0x00000000 /* RWI-V */
#define NV_IO_GC_ROTATE_FUNCTION                                4:3 /* RWIVF */
#define NV_IO_GC_ROTATE_FUNCTION_NOP                     0x00000000 /* RWI-V */
#define NV_IO_GC_ROTATE_FUNCTION_AND                     0x00000001 /* RW--V */
#define NV_IO_GC_ROTATE_FUNCTION_OR                      0x00000002 /* RW--V */
#define NV_IO_GC_ROTATE_FUNCTION_XOR                     0x00000003 /* RW--V */
#define NV_IO_GC_READ_MAP                                0x000003CF /* RW-1R */
#define NV_IO_GC_READ_MAP__ALIAS_1             NV_PRMIO_GC_READ_MAP /*       */
#define NV_IO_GC_READ_MAP__INDEX                         0x00000004 /*       */
#define NV_IO_GC_READ_MAP_PLANE                                 1:0 /* RWIUF */
#define NV_IO_GC_READ_MAP_PLANE_0                        0x00000000 /* RWI-V */
#define NV_IO_GC_MODE                                    0x000003CF /* RW-1R */
#define NV_IO_GC_MODE__ALIAS_1                     NV_PRMIO_GC_MODE /*       */
#define NV_IO_GC_MODE__INDEX                             0x00000005 /*       */
#define NV_IO_GC_MODE_WRITE_MODE                                1:0 /* RWIVF */
#define NV_IO_GC_MODE_WRITE_MODE_0                       0x00000000 /* RWI-V */
#define NV_IO_GC_MODE_TEST                                      2:2 /* C--VF */
#define NV_IO_GC_MODE_TEST_0                             0x00000000 /* C---V */
#define NV_IO_GC_MODE_READ_MODE                                 3:3 /* RWIVF */
#define NV_IO_GC_MODE_READ_MODE_LATCH                    0x00000000 /* RWI-V */
#define NV_IO_GC_MODE_READ_MODE_COMPARISON               0x00000001 /* RW--V */
#define NV_IO_GC_MODE_ODD_EVEN                                  4:4 /* RWIVF */
#define NV_IO_GC_MODE_ODD_EVEN_SEQUENTIAL                0x00000000 /* RWI-V */
#define NV_IO_GC_MODE_ODD_EVEN_ODD_EVEN                  0x00000001 /* RW--V */
#define NV_IO_GC_MODE_SHIFT                                     6:5 /* RWIVF */
#define NV_IO_GC_MODE_SHIFT_PLANAR                       0x00000000 /* RWI-V */
#define NV_IO_GC_MODE_SHIFT_CGA                          0x00000001 /* RW--V */
#define NV_IO_GC_MODE_SHIFT_PIXEL                        0x00000002 /* RW--V */
#define NV_IO_GC_MISC                                    0x000003CF /* RW-1R */
#define NV_IO_GC_MISC__ALIAS_1                     NV_PRMIO_GC_MISC /*       */
#define NV_IO_GC_MISC__INDEX                             0x00000006 /*       */
#define NV_IO_GC_MISC_GRAPH_ALPHA                               0:0 /* RWIVF */
#define NV_IO_GC_MISC_GRAPH_ALPHA_ALPHANUMERICS          0x00000000 /* RWI-V */
#define NV_IO_GC_MISC_GRAPH_ALPHA_GRAPHICS               0x00000001 /* RW--V */
#define NV_IO_GC_MISC_CHAIN_OE                                  1:1 /* RWIVF */
#define NV_IO_GC_MISC_CHAIN_OE_EGA_VGA                   0x00000000 /* RWI-V */
#define NV_IO_GC_MISC_CHAIN_OE_MDA                       0x00000001 /* RW--V */
#define NV_IO_GC_MISC_MEMORY_MAP                                3:2 /* RWIVF */
#define NV_IO_GC_MISC_MEMORY_MAP_ALL                     0x00000000 /* RWI-V */
#define NV_IO_GC_MISC_MEMORY_MAP_EGA                     0x00000001 /* RW--V */
#define NV_IO_GC_MISC_MEMORY_MAP_MDA                     0x00000002 /* RW--V */
#define NV_IO_GC_MISC_MEMORY_MAP_CGA                     0x00000003 /* RW--V */
#define NV_IO_GC_DONT_CARE                               0x000003CF /* RW-1R */
#define NV_IO_GC_DONT_CARE__ALIAS_1           NV_PRMIO_GC_DONT_CARE /*       */
#define NV_IO_GC_DONT_CARE__INDEX                        0x00000007 /*       */
#define NV_IO_GC_DONT_CARE_COLOR_0                              0:0 /* RWIVF */
#define NV_IO_GC_DONT_CARE_COLOR_0_DONT_COMPARE          0x00000000 /* RWI-V */
#define NV_IO_GC_DONT_CARE_COLOR_0_COMPARE               0x00000001 /* RW--V */
#define NV_IO_GC_DONT_CARE_COLOR_1                              1:1 /* RWIVF */
#define NV_IO_GC_DONT_CARE_COLOR_1_DONT_COMPARE          0x00000000 /* RWI-V */
#define NV_IO_GC_DONT_CARE_COLOR_1_COMPARE               0x00000001 /* RW--V */
#define NV_IO_GC_DONT_CARE_COLOR_2                              2:2 /* RWIVF */
#define NV_IO_GC_DONT_CARE_COLOR_2_DONT_COMPARE          0x00000000 /* RWI-V */
#define NV_IO_GC_DONT_CARE_COLOR_2_COMPARE               0x00000001 /* RW--V */
#define NV_IO_GC_DONT_CARE_COLOR_3                              3:3 /* RWIVF */
#define NV_IO_GC_DONT_CARE_COLOR_3_DONT_COMPARE          0x00000000 /* RWI-V */
#define NV_IO_GC_DONT_CARE_COLOR_3_COMPARE               0x00000001 /* RW--V */
#define NV_IO_GC_BIT_MASK                                0x000003CF /* RW-1R */
#define NV_IO_GC_BIT_MASK__ALIAS_1             NV_PRMIO_GC_BIT_MASK /*       */
#define NV_IO_GC_BIT_MASK__INDEX                         0x00000008 /*       */
#define NV_IO_GC_BIT_MASK_VALUE_0                               0:0 /* RWIVF */
#define NV_IO_GC_BIT_MASK_VALUE_0_DONT_WRITE             0x00000000 /* RWI-V */
#define NV_IO_GC_BIT_MASK_VALUE_0_WRITE                  0x00000001 /* RW--V */
#define NV_IO_GC_BIT_MASK_VALUE_1                               1:1 /* RWIVF */
#define NV_IO_GC_BIT_MASK_VALUE_1_DONT_WRITE             0x00000000 /* RWI-V */
#define NV_IO_GC_BIT_MASK_VALUE_1_WRITE                  0x00000001 /* RW--V */
#define NV_IO_GC_BIT_MASK_VALUE_2                               2:2 /* RWIVF */
#define NV_IO_GC_BIT_MASK_VALUE_2_DONT_WRITE             0x00000000 /* RWI-V */
#define NV_IO_GC_BIT_MASK_VALUE_2_WRITE                  0x00000001 /* RW--V */
#define NV_IO_GC_BIT_MASK_VALUE_3                               3:3 /* RWIVF */
#define NV_IO_GC_BIT_MASK_VALUE_3_DONT_WRITE             0x00000000 /* RWI-V */
#define NV_IO_GC_BIT_MASK_VALUE_3_WRITE                  0x00000001 /* RW--V */
#define NV_IO_GC_BIT_MASK_VALUE_4                               4:4 /* RWIVF */
#define NV_IO_GC_BIT_MASK_VALUE_4_DONT_WRITE             0x00000000 /* RWI-V */
#define NV_IO_GC_BIT_MASK_VALUE_4_WRITE                  0x00000001 /* RW--V */
#define NV_IO_GC_BIT_MASK_VALUE_5                               5:5 /* RWIVF */
#define NV_IO_GC_BIT_MASK_VALUE_5_DONT_WRITE             0x00000000 /* RWI-V */
#define NV_IO_GC_BIT_MASK_VALUE_5_WRITE                  0x00000001 /* RW--V */
#define NV_IO_GC_BIT_MASK_VALUE_6                               6:6 /* RWIVF */
#define NV_IO_GC_BIT_MASK_VALUE_6_DONT_WRITE             0x00000000 /* RWI-V */
#define NV_IO_GC_BIT_MASK_VALUE_6_WRITE                  0x00000001 /* RW--V */
#define NV_IO_GC_BIT_MASK_VALUE_7                               7:7 /* RWIVF */
#define NV_IO_GC_BIT_MASK_VALUE_7_DONT_WRITE             0x00000000 /* RWI-V */
#define NV_IO_GC_BIT_MASK_VALUE_7_WRITE                  0x00000001 /* RW--V */
#define NV_IO_AT_ADDRESS__WRITE_FLIPFLOP0                0x000003C0 /* -W-1R */
#define NV_IO_AT_ADDRESS__READ                           0x000003C0 /* R--1R */
#define NV_IO_AT_ADDRESS_INDEX                                  4:0 /* RWIUF */
#define NV_IO_AT_ADDRESS_INDEX_0                         0x00000000 /* RWI-V */
#define NV_IO_AT_ADDRESS_PALETTE_ACCESS                         5:5 /* RWIVF */
#define NV_IO_AT_ADDRESS_PALETTE_ACCESS_CPU              0x00000000 /* RWI-V */
#define NV_IO_AT_ADDRESS_PALETTE_ACCESS_DISPLAY          0x00000001 /* RW--V */
#define NV_IO_AT_MODE__WRITE_FLIPFLOP1                   0x000003C0 /* -W-1R */
#define NV_IO_AT_MODE__READ                              0x000003C1 /* R--1R */
#define NV_IO_AT_MODE__INDEX                             0x00000010 /*       */
#define NV_IO_AT_MODE_G_OR_A                                    0:0 /* RWXVF */
#define NV_IO_AT_MODE_G_OR_A_ALPHA                       0x00000000 /* RW--V */
#define NV_IO_AT_MODE_G_OR_A_GRAPHICS                    0x00000001 /* RW--V */
#define NV_IO_AT_MODE_DISPLAY                                   1:1 /* RWXVF */
#define NV_IO_AT_MODE_DISPLAY_COLOR                      0x00000000 /* RW--V */
#define NV_IO_AT_MODE_DISPLAY_MDA                        0x00000001 /* RW--V */
#define NV_IO_AT_MODE_EXTEND_9                                  2:2 /* RWXVF */
#define NV_IO_AT_MODE_EXTEND_9_BACKGROUND                0x00000000 /* RW--V */
#define NV_IO_AT_MODE_EXTEND_9_BIT_8                     0x00000001 /* RW--V */
#define NV_IO_AT_MODE_BLINK_ENABLE                              3:3 /* RWXVF */
#define NV_IO_AT_MODE_BLINK_ENABLE_INTENSITY             0x00000000 /* RW--V */
#define NV_IO_AT_MODE_BLINK_ENABLE_BLINKING_ENABLED      0x00000001 /* RW--V */
#define NV_IO_AT_MODE_PIXEL_PANNING_COMP                        5:5 /* RWXVF */
#define NV_IO_AT_MODE_PIXEL_CLOCK_SELECT                        6:6 /* RWXVF */
#define NV_IO_AT_MODE_PALETTE_SIZE                              7:7 /* RWXVF */
#define NV_IO_DAC_DATA                                   0x000003C9 /* RW-1R */
#define NV_IO_DAC_DATA__ALIAS_1                   NV_PRMIO_DAC_DATA /*       */
#define NV_IO_DAC_DATA_VALUE                                    7:0 /* RW-VF */
/* dev_master.ref */
#define NV_PMC                                0x00000FFF:0x00000000 /* RW--D */
#define NV_PMC_BOOT_0                                    0x00000000 /* R--4R */
#define NV_PMC_BOOT_0_REVISION                                  7:0 /* C--VF */
#define NV_PMC_BOOT_0_REVISION_A                         0x00000000 /* ----V */
#define NV_PMC_BOOT_0_REVISION_B                         0x00000001 /* ----V */
#define NV_PMC_BOOT_0_REVISION_B02                       0x00000002 /* ----V */
#define NV_PMC_BOOT_0_REVISION_B03                       0x00000003 /* C---V */
#define NV_PMC_BOOT_0_REVISION_C01                       0x00000004 /* ----V */
#define NV_PMC_BOOT_0_IMPLEMENTATION                           15:8 /* R--VF */
#define NV_PMC_BOOT_0_IMPLEMENTATION_NV0                 0x00000000 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_NV1V32              0x00000001 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_NV1D32              0x00000002 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_PICASSO             0x00000003 /* R---V */
#define NV_PMC_BOOT_0_ARCHITECTURE                            23:16 /* C--VF */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV0                   0x00000000 /* ----V */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV1                   0x00000001 /* C---V */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV2                   0x00000002 /* ----V */
#define NV_PMC_BOOT_0_MANUFACTURER                            31:24 /* R--VF */
#define NV_PMC_BOOT_0_MANUFACTURER_NVIDIA                0x00000000 /* R---V */
#define NV_PMC_BOOT_0_MANUFACTURER_SGS                   0x00000001 /* R---V */
#define NV_PMC_DEBUG_0                                   0x00000080 /* R--4R */
#define NV_PMC_DEBUG_0_NVBUS_PAUDIO                             0:0 /* RWIVF */
#define NV_PMC_DEBUG_0_NVBUS_PAUDIO_DISABLED             0x00000000 /* RWI-V */
#define NV_PMC_DEBUG_0_NVBUS_PAUDIO_ENABLED              0x00000001 /* RW--V */
#define NV_PMC_DEBUG_0_NVBUS_PGRAPH                           12:12 /* RWIVF */
#define NV_PMC_DEBUG_0_NVBUS_PGRAPH_DISABLED             0x00000000 /* RWI-V */
#define NV_PMC_DEBUG_0_NVBUS_PGRAPH_ENABLED              0x00000001 /* RW--V */
#define NV_PMC_INTR_0                                    0x00000100 /* RW-4R */
#define NV_PMC_INTR_0_PAUDIO                                    0:0 /* R--VF */
#define NV_PMC_INTR_0_PAUDIO_NOT_PENDING                 0x00000000 /* R---V */
#define NV_PMC_INTR_0_PAUDIO_PENDING                     0x00000001 /* R---V */
#define NV_PMC_INTR_0_PDMA                                      4:4 /* R--VF */
#define NV_PMC_INTR_0_PDMA_NOT_PENDING                   0x00000000 /* R---V */
#define NV_PMC_INTR_0_PDMA_PENDING                       0x00000001 /* R---V */
#define NV_PMC_INTR_0_PFIFO                                     8:8 /* R--VF */
#define NV_PMC_INTR_0_PFIFO_NOT_PENDING                  0x00000000 /* R---V */
#define NV_PMC_INTR_0_PFIFO_PENDING                      0x00000001 /* R---V */
#define NV_PMC_INTR_0_PGRAPH                                  12:12 /* R--VF */
#define NV_PMC_INTR_0_PGRAPH_NOT_PENDING                 0x00000000 /* R---V */
#define NV_PMC_INTR_0_PGRAPH_PENDING                     0x00000001 /* R---V */
#define NV_PMC_INTR_0_PRM                                     16:16 /* R--VF */
#define NV_PMC_INTR_0_PRM_NOT_PENDING                    0x00000000 /* R---V */
#define NV_PMC_INTR_0_PRM_PENDING                        0x00000001 /* R---V */
#define NV_PMC_INTR_0_PTIMER                                  20:20 /* R--VF */
#define NV_PMC_INTR_0_PTIMER_NOT_PENDING                 0x00000000 /* R---V */
#define NV_PMC_INTR_0_PTIMER_PENDING                     0x00000001 /* R---V */
#define NV_PMC_INTR_0_PFB                                     24:24 /* R--VF */
#define NV_PMC_INTR_0_PFB_NOT_PENDING                    0x00000000 /* R---V */
#define NV_PMC_INTR_0_PFB_PENDING                        0x00000001 /* R---V */
#define NV_PMC_INTR_0_SOFTWARE                                28:28 /* RWIVF */
#define NV_PMC_INTR_0_SOFTWARE_NOT_PENDING               0x00000000 /* RWI-V */
#define NV_PMC_INTR_0_SOFTWARE_PENDING                   0x00000001 /* RW--V */
#define NV_PMC_INTR_EN_0                                 0x00000140 /* RW-4R */
#define NV_PMC_INTR_EN_0_INTA                                   1:0 /* RWIVF */
#define NV_PMC_INTR_EN_0_INTA_DISABLED                   0x00000000 /* RWI-V */
#define NV_PMC_INTR_EN_0_INTA_HARDWARE                   0x00000001 /* RW--V */
#define NV_PMC_INTR_EN_0_INTA_SOFTWARE                   0x00000002 /* RW--V */
#define NV_PMC_INTR_EN_0_INTB                                   5:4 /* RWIVF */
#define NV_PMC_INTR_EN_0_INTB_DISABLED                   0x00000000 /* RWI-V */
#define NV_PMC_INTR_EN_0_INTB_HARDWARE                   0x00000001 /* RW--V */
#define NV_PMC_INTR_EN_0_INTB_SOFTWARE                   0x00000002 /* RW--V */
#define NV_PMC_INTR_EN_0_INTC                                   9:8 /* RWIVF */
#define NV_PMC_INTR_EN_0_INTC_DISABLED                   0x00000000 /* RWI-V */
#define NV_PMC_INTR_EN_0_INTC_HARDWARE                   0x00000001 /* RW--V */
#define NV_PMC_INTR_EN_0_INTC_SOFTWARE                   0x00000002 /* RW--V */
#define NV_PMC_INTR_EN_0_INTD                                 13:12 /* RWIVF */
#define NV_PMC_INTR_EN_0_INTD_DISABLED                   0x00000000 /* RWI-V */
#define NV_PMC_INTR_EN_0_INTD_HARDWARE                   0x00000001 /* RW--V */
#define NV_PMC_INTR_EN_0_INTD_SOFTWARE                   0x00000002 /* RW--V */
#define NV_PMC_INTR_READ_0                               0x00000160 /* R--4R */
#define NV_PMC_INTR_READ_0_INTA                                 0:0 /* R--VF */
#define NV_PMC_INTR_READ_0_INTA_LOW                      0x00000000 /* R---V */
#define NV_PMC_INTR_READ_0_INTA_HIGH                     0x00000001 /* R---V */
#define NV_PMC_INTR_READ_0_INTB                                 4:4 /* R--VF */
#define NV_PMC_INTR_READ_0_INTB_LOW                      0x00000000 /* R---V */
#define NV_PMC_INTR_READ_0_INTB_HIGH                     0x00000001 /* R---V */
#define NV_PMC_INTR_READ_0_INTC                                 8:8 /* R--VF */
#define NV_PMC_INTR_READ_0_INTC_LOW                      0x00000000 /* R---V */
#define NV_PMC_INTR_READ_0_INTC_HIGH                     0x00000001 /* R---V */
#define NV_PMC_INTR_READ_0_INTD                               12:12 /* R--VF */
#define NV_PMC_INTR_READ_0_INTD_LOW                      0x00000000 /* R---V */
#define NV_PMC_INTR_READ_0_INTD_HIGH                     0x00000001 /* R---V */
#define NV_PMC_INTR_READ_0_SERR                               16:16 /* R--VF */
#define NV_PMC_INTR_READ_0_SERR_LOW                      0x00000000 /* R---V */
#define NV_PMC_INTR_READ_0_SERR_HIGH                     0x00000001 /* R---V */
#define NV_PMC_ENABLE                                    0x00000200 /* RW-4R */
#define NV_PMC_ENABLE_PAUDIO                                    0:0 /* RWIVF */
#define NV_PMC_ENABLE_PAUDIO_DISABLED                    0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PAUDIO_ENABLED                     0x00000001 /* RW--V */
#define NV_PMC_ENABLE_PDMA                                      4:4 /* RWIVF */
#define NV_PMC_ENABLE_PDMA_DISABLED                      0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PDMA_ENABLED                       0x00000001 /* RW--V */
#define NV_PMC_ENABLE_PFIFO                                     8:8 /* RWIVF */
#define NV_PMC_ENABLE_PFIFO_DISABLED                     0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PFIFO_ENABLED                      0x00000001 /* RW--V */
#define NV_PMC_ENABLE_PGRAPH                                  12:12 /* RWIVF */
#define NV_PMC_ENABLE_PGRAPH_DISABLED                    0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PGRAPH_ENABLED                     0x00000001 /* RW--V */
#define NV_PMC_ENABLE_PRM                                     16:16 /* RWIVF */
#define NV_PMC_ENABLE_PRM_DISABLED                       0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PRM_ENABLED                        0x00000001 /* RW--V */
#define NV_PMC_ENABLE_PFB                                     24:24 /* RWIVF */
#define NV_PMC_ENABLE_PFB_DISABLED                       0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PFB_ENABLED                        0x00000001 /* RW--V */
#define NV_PMC_WATCHDOG                                  0x00000400 /* RW-4R */
#define NV_PMC_WATCHDOG_TIMEOUT                                 3:0 /* RWIVF */
#define NV_PMC_WATCHDOG_TIMEOUT_1_US                     0x00000000 /* RWI-V */
#define NV_PMC_WATCHDOG_TIMEOUT_2_US                     0x00000001 /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_4_US                     0x00000002 /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_8_US                     0x00000003 /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_16_US                    0x00000004 /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_32_US                    0x00000005 /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_64_US                    0x00000006 /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_128_US                   0x00000007 /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_256_US                   0x00000008 /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_512_US                   0x00000009 /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_1_MS                     0x0000000A /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_2_MS                     0x0000000B /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_4_MS                     0x0000000C /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_8_MS                     0x0000000D /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_16_MS                    0x0000000E /* RW--V */
#define NV_PMC_WATCHDOG_TIMEOUT_32_MS                    0x0000000F /* RW--V */
#define NV_PMC_WATCHDOG_STATE                                  10:8 /* RWIVF */
#define NV_PMC_WATCHDOG_STATE_SLEEPING                   0x00000000 /* RWI-V */
#define NV_PMC_WATCHDOG_STATE_SNIFFING                   0x00000001 /* RW--V */
#define NV_PMC_WATCHDOG_STATE_GROWLING                   0x00000002 /* RW--V */
#define NV_PMC_WATCHDOG_STATE_BARKING                    0x00000003 /* RW--V */
#define NV_PMC_WATCHDOG_STATE_BITING                     0x00000004 /* RW--V */
/* dev_bus.ref */
#define NV_PBUS                               0x00001FFF:0x00001000 /* RW--D */
#define NV_PBUS_DEBUG_0                                  0x00001080 /* RW-4R */
#define NV_PBUS_DEBUG_0_MCLK_DIVIDE                             0:0 /* RWIVF */
#define NV_PBUS_DEBUG_0_MCLK_DIVIDE_BY_TWO               0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_0_MCLK_DIVIDE_BY_ONE               0x00000001 /* RW--V */
#define NV_PBUS_DEBUG_0_MCLK_LCLK                               4:4 /* RWIVF */
#define NV_PBUS_DEBUG_0_MCLK_LCLK_ASYNCHRONOUS           0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_0_MCLK_LCLK_SYNCHRONOUS            0x00000001 /* RW--V */
#define NV_PBUS_DEBUG_0_MEMORY_READ                             8:8 /* RWIVF */
#define NV_PBUS_DEBUG_0_MEMORY_READ_DWORD                0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_0_MEMORY_READ_LINE                 0x00000001 /* RW--V */
#define NV_PBUS_ACCESS                                   0x00001200 /* RW-4R */
#define NV_PBUS_ACCESS_GAME_PORT                                0:0 /* RWIVF */
#define NV_PBUS_ACCESS_GAME_PORT_DISABLED                0x00000000 /* RWI-V */
#define NV_PBUS_ACCESS_GAME_PORT_ENABLED                 0x00000001 /* RW--V */
#define NV_PBUS_ACCESS_MDA                                      4:4 /* RWIVF */
#define NV_PBUS_ACCESS_MDA_DISABLED                      0x00000000 /* RWI-V */
#define NV_PBUS_ACCESS_MDA_ENABLED                       0x00000001 /* RW--V */
#define NV_PBUS_ACCESS_SOUND_BOARD                              8:8 /* RWIVF */
#define NV_PBUS_ACCESS_SOUND_BOARD_DISABLED              0x00000000 /* RWI-V */
#define NV_PBUS_ACCESS_SOUND_BOARD_ENABLED               0x00000001 /* RW--V */
#define NV_PBUS_ACCESS_RM_CONTROL                             12:12 /* RWIVF */
#define NV_PBUS_ACCESS_RM_CONTROL_DISABLED               0x00000000 /* RWI-V */
#define NV_PBUS_ACCESS_RM_CONTROL_ENABLED                0x00000001 /* RW--V */
#define NV_PBUS_ACCESS_MPU_401                                17:16 /* RWIVF */
#define NV_PBUS_ACCESS_MPU_401_DISABLED                  0x00000000 /* RWI-V */
#define NV_PBUS_ACCESS_MPU_401_0330H                     0x00000001 /* RW--V */
#define NV_PBUS_ACCESS_MPU_401_0300H                     0x00000002 /* RW--V */
#define NV_PBUS_ACCESS_MPU_401_0230H                     0x00000003 /* RW--V */
#define NV_PBUS_ACCESS_DMA                                    20:20 /* RWIVF */
#define NV_PBUS_ACCESS_DMA_DISABLED                      0x00000000 /* RWI-V */
#define NV_PBUS_ACCESS_DMA_ENABLED                       0x00000001 /* RW--V */
#define NV_PBUS_ACCESS_FM_SYNTHESIS                           24:24 /* RWIVF */
#define NV_PBUS_ACCESS_FM_SYNTHESIS_DISABLED             0x00000000 /* RWI-V */
#define NV_PBUS_ACCESS_FM_SYNTHESIS_ENABLED              0x00000001 /* RW--V */
#define NV_PBUS_RMC_SVGA                                 0x00001410 /* RW-4R */
#define NV_PBUS_RMC_SVGA__ALIAS_1                NV_MEMORY_RMC_SVGA /*       */
#define NV_PBUS_RMC_SVGA_SEGMENT                                3:0 /* RWXUF */
#define NV_PBUS_RMC_SVGA_SEGMENT_0K                      0x00000000 /* RW--V */
#define NV_PBUS_RMC_SVGA_SEGMENT_64K                     0x00000001 /* RW--V */
#define NV_PBUS_RMC_SVGA_SEGMENT_896K                    0x0000000E /* RW--V */
#define NV_PBUS_RMC_SVGA_SEGMENT_960K                    0x0000000F /* RW--V */
#define NV_PBUS_RMC_SVGA_TYPE                                 12:12 /* RWXVF */
#define NV_PBUS_RMC_SVGA_TYPE_LINEAR                     0x00000000 /* RW--V */
#define NV_PBUS_RMC_SVGA_TYPE_VGA                        0x00000001 /* RW--V */
#define NV_PBUS_RMC_WINDOW(i)                   (0x00001440+(i)*16) /* RW-4A */
#define NV_PBUS_RMC_WINDOW__SIZE_1                                3 /*       */
#define NV_PBUS_RMC_WINDOW__ALIAS_1            NV_MEMORY_RMC_WINDOW /*       */
#define NV_PBUS_RMC_WINDOW_SEGMENT                            24:13 /* RWXUF */
#define NV_PBUS_RMC_WINDOW_SEGMENT_0K                    0x00000000 /* RW--V */
#define NV_PBUS_RMC_WINDOW_SEGMENT_8K                    0x00000001 /* RW--V */
#define NV_PBUS_RMC_WINDOW_SEGMENT_32752K                0x00000FFE /* RW--V */
#define NV_PBUS_RMC_WINDOW_SEGMENT_32760K                0x00000FFF /* RW--V */
#define NV_PBUS_RMC_VL                                   0x00001490 /* RW-4R */
#define NV_PBUS_RMC_VL__ALIAS_1                    NV_MEMORY_RMC_VL /*       */
#define NV_PBUS_RMC_VL_VSE_46E8_ENABLE                          3:3 /* RWIVF */
#define NV_PBUS_RMC_VL_VSE_46E8_ENABLE_DISABLED          0x00000000 /* RWI-V */
#define NV_PBUS_RMC_VL_VSE_46E8_ENABLE_ENABLED           0x00000001 /* RW--V */
#define NV_PBUS_RMC_VL_VSE_46E8_SETUP                           4:4 /* RWIVF */
#define NV_PBUS_RMC_VL_VSE_46E8_SETUP_DISABLED           0x00000000 /* RWI-V */
#define NV_PBUS_RMC_VL_VSE_46E8_SETUP_ENABLED            0x00000001 /* RW--V */
#define NV_PBUS_RMC_VL_POS_0102_ENABLE                        16:16 /* RWIVF */
#define NV_PBUS_RMC_VL_POS_0102_ENABLE_DISABLED          0x00000000 /* RWI-V */
#define NV_PBUS_RMC_VL_POS_0102_ENABLE_ENABLED           0x00000001 /* RW--V */
#define NV_PBUS_RMC_VL_VSE_03C3_ENABLE                        24:24 /* RWIVF */
#define NV_PBUS_RMC_VL_VSE_03C3_ENABLE_DISABLED          0x00000000 /* RWI-V */
#define NV_PBUS_RMC_VL_VSE_03C3_ENABLE_ENABLED           0x00000001 /* RW--V */
#define NV_PBUS_RMC_DMA_0                                0x00001480 /* RW-4R */
#define NV_PBUS_RMC_DMA_0__ALIAS_1              NV_MEMORY_RMC_DMA_0 /*       */
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
#define NV_PBUS_RMC_DMA_1                                0x00001484 /* RW-4R */
#define NV_PBUS_RMC_DMA_1__ALIAS_1              NV_MEMORY_RMC_DMA_1 /*       */
#define NV_PBUS_RMC_DMA_1_COUNT_BYTE_0                          7:0 /* RWXUF */
#define NV_PBUS_RMC_DMA_1_COUNT_BYTE_1                         15:8 /* RWXUF */
#define NV_PBUS_RMC_DMA_1_STATE                               28:28 /* RWXVF */
#define NV_PBUS_RMC_DMA_1_STATE_CLEAN                    0x00000000 /* RW--V */
#define NV_PBUS_RMC_DMA_1_STATE_DIRTY                    0x00000001 /* RW--V */
#define NV_PBUS_PCI_VGA_0                                0x00001800 /* R--4R */
#define NV_PBUS_PCI_VGA_0__ALIAS_1              NV_CONFIG_PCI_VGA_0 /*       */
#define NV_PBUS_PCI_NV_0                                 0x00001900 /* R--4R */
#define NV_PBUS_PCI_NV_0__ALIAS_1                NV_CONFIG_PCI_NV_0 /*       */
#define NV_PBUS_PCI_VGA_1                                0x00001804 /* RW-4R */
#define NV_PBUS_PCI_VGA_1__ALIAS_1              NV_CONFIG_PCI_VGA_1 /*       */
#define NV_PBUS_PCI_NV_1                                 0x00001904 /* RW-4R */
#define NV_PBUS_PCI_NV_1__ALIAS_1                NV_CONFIG_PCI_NV_1 /*       */
#define NV_PBUS_PCI_VGA_2                                0x00001808 /* R--4R */
#define NV_PBUS_PCI_VGA_2__ALIAS_1               NV_CONFIG_PCI_NV_2 /*       */
#define NV_PBUS_PCI_NV_2                                 0x00001908 /* R--4R */
#define NV_PBUS_PCI_NV_2__ALIAS_1                NV_CONFIG_PCI_NV_2 /*       */
#define NV_PBUS_PCI_VGA_3                                0x0000180C /* RW-4R */
#define NV_PBUS_PCI_VGA_3__ALIAS_1              NV_CONFIG_PCI_VGA_3 /*       */
#define NV_PBUS_PCI_NV_3                                 0x0000190C /* RW-4R */
#define NV_PBUS_PCI_NV_3__ALIAS_1                NV_CONFIG_PCI_NV_3 /*       */
#define NV_PBUS_PCI_NV_4                                 0x00001910 /* RW-4R */
#define NV_PBUS_PCI_NV_4__ALIAS_1                NV_CONFIG_PCI_NV_4 /*       */
#define NV_PBUS_PCI_VGA_4(i)                     (0x00001810+(i)*4) /* R--4A */
#define NV_PBUS_PCI_VGA_4__SIZE_1                                 6 /*       */
#define NV_PBUS_PCI_VGA_4__ALIAS_1              NV_CONFIG_PCI_VGA_4 /*       */
#define NV_PBUS_PCI_NV_5(i)                      (0x00001914+(i)*4) /* R--4A */
#define NV_PBUS_PCI_NV_5__SIZE_1                                  5 /*       */
#define NV_PBUS_PCI_NV_5__ALIAS_1                NV_CONFIG_PCI_NV_5 /*       */
#define NV_PBUS_PCI_VGA_10(i)                    (0x00001828+(i)*4) /* R--4A */
#define NV_PBUS_PCI_VGA_10__SIZE_1                                2 /*       */
#define NV_PBUS_PCI_VGA_10__ALIAS_1            NV_CONFIG_PCI_VGA_10 /*       */
#define NV_PBUS_PCI_NV_10(i)                     (0x00001928+(i)*4) /* R--4A */
#define NV_PBUS_PCI_NV_10__SIZE_1                                 2 /*       */
#define NV_PBUS_PCI_NV_10__ALIAS_1              NV_CONFIG_PCI_NV_10 /*       */
#define NV_PBUS_PCI_VGA_12                               0x00001830 /* RW-4R */
#define NV_PBUS_PCI_VGA_12__ALIAS_1            NV_CONFIG_PCI_VGA_12 /*       */
#define NV_PBUS_PCI_NV_12                                0x00001930 /* RW-4R */
#define NV_PBUS_PCI_NV_12__ALIAS_1              NV_CONFIG_PCI_NV_12 /*       */
#define NV_PBUS_PCI_VGA_13(i)                    (0x00001834+(i)*4) /* R--4A */
#define NV_PBUS_PCI_VGA_13__SIZE_1                                2 /*       */
#define NV_PBUS_PCI_VGA_13__ALIAS_1            NV_CONFIG_PCI_VGA_13 /*       */
#define NV_PBUS_PCI_NV_13(i)                     (0x00001934+(i)*4) /* R--4A */
#define NV_PBUS_PCI_NV_13__SIZE_1                                 2 /*       */
#define NV_PBUS_PCI_NV_13__ALIAS_1              NV_CONFIG_PCI_NV_13 /*       */
#define NV_PBUS_PCI_VGA_15                               0x0000183C /* RW-4R */
#define NV_PBUS_PCI_VGA_15__ALIAS_1            NV_CONFIG_PCI_VGA_15 /*       */
#define NV_PBUS_PCI_NV_15                                0x0000193C /* RW-4R */
#define NV_PBUS_PCI_NV_15__ALIAS_1              NV_CONFIG_PCI_NV_15 /*       */
#define NV_PBUS_PCI_VGA_16(i)                    (0x00001840+(i)*4) /* R--4A */
#define NV_PBUS_PCI_VGA_16__SIZE_1                               48 /*       */
#define NV_PBUS_PCI_VGA_16__ALIAS_1            NV_CONFIG_PCI_VGA_16 /*       */
#define NV_PBUS_PCI_NV_16(i)                     (0x00001940+(i)*4) /* R--4A */
#define NV_PBUS_PCI_NV_16__SIZE_1                                48 /*       */
#define NV_PBUS_PCI_NV_16__ALIAS_1              NV_CONFIG_PCI_NV_16 /*       */
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
#define NV_PFIFO_CONFIG_0                                0x00002200 /* RW-4R */
#define NV_PFIFO_CONFIG_0_FREE_LIE                              1:0 /* RWXVF */
#define NV_PFIFO_CONFIG_0_FREE_LIE_DISABLED              0x00000000 /* RW--V */
#define NV_PFIFO_CONFIG_0_FREE_LIE_252_BYTES             0x00000001 /* RW--V */
#define NV_PFIFO_CONFIG_0_FREE_LIE_508_BYTES             0x00000002 /* RW--V */
#define NV_PFIFO_CONFIG_0_FREE_LIE_1020_BYTES            0x00000003 /* RW--V */
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
#define NV_PFIFO_CACHE0_PUSH1                            0x00003010 /* RW-4R */
#define NV_PFIFO_CACHE0_PUSH1_CHID                              6:0 /* RWXUF */

//	Fix double-definition warning

#ifdef	NV_PFIFO_CACHE1_PUSH1
#undef	NV_PFIFO_CACHE1_PUSH1
#endif
#define NV_PFIFO_CACHE1_PUSH1                            0x00003210 /* RW-4R */
#define NV_PFIFO_CACHE1_PUSH1_CHID                              6:0 /* RWXUF */
#define NV_PFIFO_CACHE0_PULL0                            0x00003040 /* RW-4R */
#define NV_PFIFO_CACHE0_PULL0_ACCESS                            0:0 /* RWIVF */
#define NV_PFIFO_CACHE0_PULL0_ACCESS_DISABLED            0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE0_PULL0_ACCESS_ENABLED             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_PULL0                            0x00003240 /* R--4R */
#define NV_PFIFO_CACHE1_PULL0_ACCESS                            0:0 /* RWIVF */
#define NV_PFIFO_CACHE1_PULL0_ACCESS_DISABLED            0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE1_PULL0_ACCESS_ENABLED             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_PULL0_HASH                              4:4 /* R-XVF */
#define NV_PFIFO_CACHE1_PULL0_HASH_SUCCEEDED             0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_HASH_FAILED                0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_DEVICE                            8:8 /* R-XVF */
#define NV_PFIFO_CACHE1_PULL0_DEVICE_HARDWARE            0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_DEVICE_SOFTWARE            0x00000001 /* R---V */
#define NV_PFIFO_CACHE0_PULL1                            0x00003050 /* RW-4R */
#define NV_PFIFO_CACHE0_PULL1_OBJECT                            8:8 /* RWXVF */
#define NV_PFIFO_CACHE0_PULL1_OBJECT_UNCHANGED           0x00000000 /* RW--V */
#define NV_PFIFO_CACHE0_PULL1_OBJECT_CHANGED             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1                            0x00003250 /* RW-4R */
#define NV_PFIFO_CACHE1_PULL1_SUBCHANNEL                        2:0 /* RWXUF */
#define NV_PFIFO_CACHE1_PULL1_CTX                               4:4 /* RWXVF */
#define NV_PFIFO_CACHE1_PULL1_CTX_CLEAN                  0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1_CTX_DIRTY                  0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1_OBJECT                            8:8 /* RWXVF */
#define NV_PFIFO_CACHE1_PULL1_OBJECT_UNCHANGED           0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1_OBJECT_CHANGED             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_STATUS                           0x00003020 /* R--4R */
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK                         4:4 /* R--VF */
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK_NOT_EMPTY        0x00000000 /* R---V */
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK_EMPTY            0x00000001 /* R---V */
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK                        8:8 /* R--VF */
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK_NOT_FULL        0x00000000 /* R---V */
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK_FULL            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_STATUS                           0x00003220 /* R--4R */
#define NV_PFIFO_CACHE1_STATUS_RANOUT                           0:0 /* R-XVF */
#define NV_PFIFO_CACHE1_STATUS_RANOUT_FALSE              0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_RANOUT_TRUE               0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK                         4:4 /* R--VF */
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK_NOT_EMPTY        0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK                        8:8 /* R--VF */
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK_NOT_FULL        0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK_FULL            0x00000001 /* R---V */
#define NV_PFIFO_CACHE0_PUT                              0x00003030 /* RW-4R */
#define NV_PFIFO_CACHE0_PUT_ADDRESS                             2:2 /* RWXUF */
#define NV_PFIFO_CACHE1_PUT                              0x00003230 /* RW-4R */
#define NV_PFIFO_CACHE1_PUT_ADDRESS                             6:2 /* RWXUF */
#define NV_PFIFO_CACHE0_GET                              0x00003070 /* RW-4R */
#define NV_PFIFO_CACHE0_GET_ADDRESS                             2:2 /* RWXUF */
#define NV_PFIFO_CACHE1_GET                              0x00003270 /* RW-4R */
#define NV_PFIFO_CACHE1_GET_ADDRESS                             6:2 /* RWXUF */
#define NV_PFIFO_CACHE0_CTX(i)                  (0x00003080+(i)*16) /* RW-4A */
#define NV_PFIFO_CACHE0_CTX__SIZE_1                               1 /*       */
#define NV_PFIFO_CACHE0_CTX_INSTANCE                           15:0 /* RWXUF */
#define NV_PFIFO_CACHE0_CTX_DEVICE                            22:16 /* RWXUF */
#define NV_PFIFO_CACHE1_CTX(i)                  (0x00003280+(i)*16) /* RW-4A */
#define NV_PFIFO_CACHE1_CTX__SIZE_1                               8 /*       */
#define NV_PFIFO_CACHE1_CTX_INSTANCE                           15:0 /* RWXUF */
#define NV_PFIFO_CACHE1_CTX_DEVICE                            22:16 /* RWXUF */
#define NV_PFIFO_CACHE1_CTX_LIE                               24:24 /* RWXVF */
#define NV_PFIFO_CACHE1_CTX_LIE_DISABLED                 0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_CTX_LIE_ENABLED                  0x00000001 /* RW--V */
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
#define NV_PFIFO_DEVICE_ACCESS                                28:28 /* R--VF */
#define NV_PFIFO_DEVICE_ACCESS_DISABLED                  0x00000000 /* R---V */
#define NV_PFIFO_DEVICE_ACCESS_ENABLED                   0x00000001 /* R---V */
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
#define NV_PFIFO_RUNOUT_PUT_ADDRESS                            13:3 /* RWXUF */
#define NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_0                    10:3 /*       */
#define NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_1                    11:3 /*       */
#define NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_2                    12:3 /*       */
#define NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_3                    13:3 /*       */
#define NV_PFIFO_RUNOUT_GET                              0x00002420 /* RW-4R */
#define NV_PFIFO_RUNOUT_GET_ADDRESS                            13:3 /* RWXUF */
/* dev_dma.ref */
#define NV_PDMA                               0x00100FFF:0x00100000 /* RW--D */
#define NV_PDMA_RM_INTR_0                                0x00100100 /* RW-4R */
#define NV_PDMA_RM_INTR_0_BUS_ERROR                           12:12 /* RWXVF */
#define NV_PDMA_RM_INTR_0_BUS_ERROR_NOT_PENDING          0x00000000 /* R---V */
#define NV_PDMA_RM_INTR_0_BUS_ERROR_PENDING              0x00000001 /* R---V */
#define NV_PDMA_RM_INTR_0_BUS_ERROR_RESET                0x00000001 /* -W--V */
#define NV_PDMA_AU_INTR_0                                0x00100108 /* RW-4R */
#define NV_PDMA_AU_INTR_0_INSTANCE                              0:0 /* RWXVF */
#define NV_PDMA_AU_INTR_0_INSTANCE_NOT_PENDING           0x00000000 /* R---V */
#define NV_PDMA_AU_INTR_0_INSTANCE_PENDING               0x00000001 /* R---V */
#define NV_PDMA_AU_INTR_0_INSTANCE_RESET                 0x00000001 /* -W--V */
#define NV_PDMA_AU_INTR_0_PRESENT                               4:4 /* RWXVF */
#define NV_PDMA_AU_INTR_0_PRESENT_NOT_PENDING            0x00000000 /* R---V */
#define NV_PDMA_AU_INTR_0_PRESENT_PENDING                0x00000001 /* R---V */
#define NV_PDMA_AU_INTR_0_PRESENT_RESET                  0x00000001 /* -W--V */
#define NV_PDMA_AU_INTR_0_PROTECTION                            8:8 /* RWXVF */
#define NV_PDMA_AU_INTR_0_PROTECTION_NOT_PENDING         0x00000000 /* R---V */
#define NV_PDMA_AU_INTR_0_PROTECTION_PENDING             0x00000001 /* R---V */
#define NV_PDMA_AU_INTR_0_PROTECTION_RESET               0x00000001 /* -W--V */
#define NV_PDMA_AU_INTR_0_BUS_ERROR                           12:12 /* RWXVF */
#define NV_PDMA_AU_INTR_0_BUS_ERROR_NOT_PENDING          0x00000000 /* R---V */
#define NV_PDMA_AU_INTR_0_BUS_ERROR_PENDING              0x00000001 /* R---V */
#define NV_PDMA_AU_INTR_0_BUS_ERROR_RESET                0x00000001 /* -W--V */
#define NV_PDMA_GR_INTR_0                                0x00100110 /* RW-4R */
#define NV_PDMA_GR_INTR_0_INSTANCE                              0:0 /* RWXVF */
#define NV_PDMA_GR_INTR_0_INSTANCE_NOT_PENDING           0x00000000 /* R---V */
#define NV_PDMA_GR_INTR_0_INSTANCE_PENDING               0x00000001 /* R---V */
#define NV_PDMA_GR_INTR_0_INSTANCE_RESET                 0x00000001 /* -W--V */
#define NV_PDMA_GR_INTR_0_PRESENT                               4:4 /* RWXVF */
#define NV_PDMA_GR_INTR_0_PRESENT_NOT_PENDING            0x00000000 /* R---V */
#define NV_PDMA_GR_INTR_0_PRESENT_PENDING                0x00000001 /* R---V */
#define NV_PDMA_GR_INTR_0_PRESENT_RESET                  0x00000001 /* -W--V */
#define NV_PDMA_GR_INTR_0_PROTECTION                            8:8 /* RWXVF */
#define NV_PDMA_GR_INTR_0_PROTECTION_NOT_PENDING         0x00000000 /* R---V */
#define NV_PDMA_GR_INTR_0_PROTECTION_PENDING             0x00000001 /* R---V */
#define NV_PDMA_GR_INTR_0_PROTECTION_RESET               0x00000001 /* -W--V */
#define NV_PDMA_GR_INTR_0_BUS_ERROR                           12:12 /* RWXVF */
#define NV_PDMA_GR_INTR_0_BUS_ERROR_NOT_PENDING          0x00000000 /* R---V */
#define NV_PDMA_GR_INTR_0_BUS_ERROR_PENDING              0x00000001 /* R---V */
#define NV_PDMA_GR_INTR_0_BUS_ERROR_RESET                0x00000001 /* -W--V */
#define NV_PDMA_RM_INTR_EN_0                             0x00100140 /* RW-4R */
#define NV_PDMA_RM_INTR_EN_0_BUS_ERROR                        12:12 /* RWIVF */
#define NV_PDMA_RM_INTR_EN_0_BUS_ERROR_DISABLED          0x00000000 /* RWI-V */
#define NV_PDMA_RM_INTR_EN_0_BUS_ERROR_ENABLED           0x00000001 /* RW--V */
#define NV_PDMA_AU_INTR_EN_0                             0x00100148 /* RW-4R */
#define NV_PDMA_AU_INTR_EN_0_INSTANCE                           0:0 /* RWIVF */
#define NV_PDMA_AU_INTR_EN_0_INSTANCE_DISABLED           0x00000000 /* RWI-V */
#define NV_PDMA_AU_INTR_EN_0_INSTANCE_ENABLED            0x00000001 /* RW--V */
#define NV_PDMA_AU_INTR_EN_0_PRESENT                            4:4 /* RWIVF */
#define NV_PDMA_AU_INTR_EN_0_PRESENT_DISABLED            0x00000000 /* RWI-V */
#define NV_PDMA_AU_INTR_EN_0_PRESENT_ENABLED             0x00000001 /* RW--V */
#define NV_PDMA_AU_INTR_EN_0_PROTECTION                         8:8 /* RWIVF */
#define NV_PDMA_AU_INTR_EN_0_PROTECTION_DISABLED         0x00000000 /* RWI-V */
#define NV_PDMA_AU_INTR_EN_0_PROTECTION_ENABLED          0x00000001 /* RW--V */
#define NV_PDMA_AU_INTR_EN_0_BUS_ERROR                        12:12 /* RWIVF */
#define NV_PDMA_AU_INTR_EN_0_BUS_ERROR_DISABLED          0x00000000 /* RWI-V */
#define NV_PDMA_AU_INTR_EN_0_BUS_ERROR_ENABLED           0x00000001 /* RW--V */
#define NV_PDMA_GR_INTR_EN_0                             0x00100150 /* RW-4R */
#define NV_PDMA_GR_INTR_EN_0_INSTANCE                           0:0 /* RWIVF */
#define NV_PDMA_GR_INTR_EN_0_INSTANCE_DISABLED           0x00000000 /* RWI-V */
#define NV_PDMA_GR_INTR_EN_0_INSTANCE_ENABLED            0x00000001 /* RW--V */
#define NV_PDMA_GR_INTR_EN_0_PRESENT                            4:4 /* RWIVF */
#define NV_PDMA_GR_INTR_EN_0_PRESENT_DISABLED            0x00000000 /* RWI-V */
#define NV_PDMA_GR_INTR_EN_0_PRESENT_ENABLED             0x00000001 /* RW--V */
#define NV_PDMA_GR_INTR_EN_0_PROTECTION                         8:8 /* RWIVF */
#define NV_PDMA_GR_INTR_EN_0_PROTECTION_DISABLED         0x00000000 /* RWI-V */
#define NV_PDMA_GR_INTR_EN_0_PROTECTION_ENABLED          0x00000001 /* RW--V */
#define NV_PDMA_GR_INTR_EN_0_BUS_ERROR                        12:12 /* RWIVF */
#define NV_PDMA_GR_INTR_EN_0_BUS_ERROR_DISABLED          0x00000000 /* RWI-V */
#define NV_PDMA_GR_INTR_EN_0_BUS_ERROR_ENABLED           0x00000001 /* RW--V */
#define NV_PDMA_AU_CONTROL                               0x00100600 /* RW-4R */
#define NV_PDMA_AU_CONTROL_ADJUST                              11:0 /* RWXUF */
#define NV_PDMA_AU_CONTROL_PAGE_TABLE                         16:16 /* RWIVF */
#define NV_PDMA_AU_CONTROL_PAGE_TABLE_NOT_PRESENT        0x00000000 /* RWI-V */
#define NV_PDMA_AU_CONTROL_PAGE_TABLE_PRESENT            0x00000001 /* RW--V */
#define NV_PDMA_AU_CONTROL_LIMIT32                            20:20 /* RWXUF */
#define NV_PDMA_GR_CONTROL                               0x00100800 /* RW-4R */
#define NV_PDMA_GR_CONTROL_ADJUST                              11:0 /* RWXUF */
#define NV_PDMA_GR_CONTROL_PAGE_TABLE                         16:16 /* RWIVF */
#define NV_PDMA_GR_CONTROL_PAGE_TABLE_NOT_PRESENT        0x00000000 /* RWI-V */
#define NV_PDMA_GR_CONTROL_PAGE_TABLE_PRESENT            0x00000001 /* RW--V */
#define NV_PDMA_GR_CONTROL_LIMIT32                            20:20 /* RWXUF */
#define NV_PDMA_AU_LIMIT                                 0x00100604 /* RW-4R */
#define NV_PDMA_AU_LIMIT_OFFSET                                31:0 /* RWXUF */
#define NV_PDMA_GR_LIMIT                                 0x00100804 /* RW-4R */
#define NV_PDMA_GR_LIMIT_OFFSET                                31:0 /* RWXUF */
#define NV_PDMA_AU_TLB_PTE                               0x00100608 /* RW-4R */
#define NV_PDMA_AU_TLB_PTE_PAGE                                 0:0 /* RWXVF */
#define NV_PDMA_AU_TLB_PTE_PAGE_NOT_PRESENT              0x00000000 /* RW--V */
#define NV_PDMA_AU_TLB_PTE_PAGE_PRESENT                  0x00000001 /* RW--V */
#define NV_PDMA_AU_TLB_PTE_ACCESS                               1:1 /* RWXVF */
#define NV_PDMA_AU_TLB_PTE_ACCESS_READ_ONLY              0x00000000 /* RW--V */
#define NV_PDMA_AU_TLB_PTE_ACCESS_READ_WRITE             0x00000001 /* RW--V */
#define NV_PDMA_AU_TLB_PTE_FRAME_ADDRESS                      31:12 /* RWXUF */
#define NV_PDMA_GR_TLB_PTE                               0x00100808 /* RW-4R */
#define NV_PDMA_GR_TLB_PTE_PAGE                                 0:0 /* RWXVF */
#define NV_PDMA_GR_TLB_PTE_PAGE_NOT_PRESENT              0x00000000 /* RW--V */
#define NV_PDMA_GR_TLB_PTE_PAGE_PRESENT                  0x00000001 /* RW--V */
#define NV_PDMA_GR_TLB_PTE_ACCESS                               1:1 /* RWXVF */
#define NV_PDMA_GR_TLB_PTE_ACCESS_READ_ONLY              0x00000000 /* RW--V */
#define NV_PDMA_GR_TLB_PTE_ACCESS_READ_WRITE             0x00000001 /* RW--V */
#define NV_PDMA_GR_TLB_PTE_FRAME_ADDRESS                      31:12 /* RWXUF */
#define NV_PDMA_AU_CHANNEL                               0x00100610 /* RW-4R */
#define NV_PDMA_AU_CHANNEL_ACCESS                               0:0 /* RWIVF */
#define NV_PDMA_AU_CHANNEL_ACCESS_DISABLED               0x00000000 /* RWI-V */
#define NV_PDMA_AU_CHANNEL_ACCESS_ENABLED                0x00000001 /* RW--V */
#define NV_PDMA_GR_CHANNEL                               0x00100810 /* RW-4R */
#define NV_PDMA_GR_CHANNEL_ACCESS                               0:0 /* RWIVF */
#define NV_PDMA_GR_CHANNEL_ACCESS_DISABLED               0x00000000 /* RWI-V */
#define NV_PDMA_GR_CHANNEL_ACCESS_ENABLED                0x00000001 /* RW--V */
#define NV_PDMA_RM_STATUS_0                              0x00100414 /* R--4R */
#define NV_PDMA_RM_STATUS_0_STATE                               0:0 /* R-XVF */
#define NV_PDMA_RM_STATUS_0_STATE_IDLE                   0x00000000 /* R---V */
#define NV_PDMA_RM_STATUS_0_STATE_BUSY                   0x00000001 /* R---V */
#define NV_PDMA_AU_STATUS_0                              0x00100614 /* R--4R */
#define NV_PDMA_AU_STATUS_0_STATE                               0:0 /* R-XVF */
#define NV_PDMA_AU_STATUS_0_STATE_IDLE                   0x00000000 /* R---V */
#define NV_PDMA_AU_STATUS_0_STATE_BUSY                   0x00000001 /* R---V */
#define NV_PDMA_AU_STATUS_0_FETCH_STATE                         4:4 /* R-IVF */
#define NV_PDMA_AU_STATUS_0_FETCH_STATE_IDLE             0x00000000 /* R-I-V */
#define NV_PDMA_AU_STATUS_0_FETCH_STATE_BUSY             0x00000001 /* R---V */
#define NV_PDMA_AU_STATUS_0_DRAIN_STATE                         8:8 /* R-IVF */
#define NV_PDMA_AU_STATUS_0_DRAIN_STATE_IDLE             0x00000000 /* R-I-V */
#define NV_PDMA_AU_STATUS_0_DRAIN_STATE_BUSY             0x00000001 /* R---V */
#define NV_PDMA_AU_STATUS_0_FLUSH_STATE                       12:12 /* R-IVF */
#define NV_PDMA_AU_STATUS_0_FLUSH_STATE_IDLE             0x00000000 /* R-I-V */
#define NV_PDMA_AU_STATUS_0_FLUSH_STATE_BUSY             0x00000001 /* R---V */
#define NV_PDMA_AU_STATUS_0_READ_STATE                        16:16 /* R-IVF */
#define NV_PDMA_AU_STATUS_0_READ_STATE_IDLE              0x00000000 /* R-I-V */
#define NV_PDMA_AU_STATUS_0_READ_STATE_BUSY              0x00000001 /* R---V */
#define NV_PDMA_AU_STATUS_0_NOTIFY_STATE                      20:20 /* R-IVF */
#define NV_PDMA_AU_STATUS_0_NOTIFY_STATE_IDLE            0x00000000 /* R-I-V */
#define NV_PDMA_AU_STATUS_0_NOTIFY_STATE_BUSY            0x00000001 /* R---V */
#define NV_PDMA_GR_STATUS_0                              0x00100814 /* R--4R */
#define NV_PDMA_GR_STATUS_0_STATE                               0:0 /* R-XVF */
#define NV_PDMA_GR_STATUS_0_STATE_IDLE                   0x00000000 /* R---V */
#define NV_PDMA_GR_STATUS_0_STATE_BUSY                   0x00000001 /* R---V */
#define NV_PDMA_GR_STATUS_0_FETCH_STATE                         4:4 /* R-IVF */
#define NV_PDMA_GR_STATUS_0_FETCH_STATE_IDLE             0x00000000 /* R-I-V */
#define NV_PDMA_GR_STATUS_0_FETCH_STATE_BUSY             0x00000001 /* R---V */
#define NV_PDMA_GR_STATUS_0_DRAIN_STATE                         8:8 /* R-IVF */
#define NV_PDMA_GR_STATUS_0_DRAIN_STATE_IDLE             0x00000000 /* R-I-V */
#define NV_PDMA_GR_STATUS_0_DRAIN_STATE_BUSY             0x00000001 /* R---V */
#define NV_PDMA_GR_STATUS_0_FLUSH_STATE                       12:12 /* R-IVF */
#define NV_PDMA_GR_STATUS_0_FLUSH_STATE_IDLE             0x00000000 /* R-I-V */
#define NV_PDMA_GR_STATUS_0_FLUSH_STATE_BUSY             0x00000001 /* R---V */
#define NV_PDMA_GR_STATUS_0_READ_STATE                        16:16 /* R-IVF */
#define NV_PDMA_GR_STATUS_0_READ_STATE_IDLE              0x00000000 /* R-I-V */
#define NV_PDMA_GR_STATUS_0_READ_STATE_BUSY              0x00000001 /* R---V */
#define NV_PDMA_GR_STATUS_0_NOTIFY_STATE                      20:20 /* R-IVF */
#define NV_PDMA_GR_STATUS_0_NOTIFY_STATE_IDLE            0x00000000 /* R-I-V */
#define NV_PDMA_GR_STATUS_0_NOTIFY_STATE_BUSY            0x00000001 /* R---V */
#define NV_PDMA_AU_STATUS_1                              0x00100618 /* RW-4R */
#define NV_PDMA_AU_STATUS_1_CONTROL                             0:0 /* RWIVF */
#define NV_PDMA_AU_STATUS_1_CONTROL_INVALID              0x00000000 /* RW--V */
#define NV_PDMA_AU_STATUS_1_CONTROL_VALID                0x00000001 /* RWI-V */
#define NV_PDMA_AU_STATUS_1_OFFSET                              4:4 /* RWIVF */
#define NV_PDMA_AU_STATUS_1_OFFSET_INVALID               0x00000000 /* RWI-V */
#define NV_PDMA_AU_STATUS_1_OFFSET_VALID                 0x00000001 /* RW--V */
#define NV_PDMA_AU_STATUS_1_TLB_PTE                             8:8 /* RWIVF */
#define NV_PDMA_AU_STATUS_1_TLB_PTE_INVALID              0x00000000 /* RW--V */
#define NV_PDMA_AU_STATUS_1_TLB_PTE_VALID                0x00000001 /* RWI-V */
#define NV_PDMA_AU_STATUS_1_ADJ_OFFSET32                      12:12 /* RWXUF */
#define NV_PDMA_GR_STATUS_1                              0x00100818 /* RW-4R */
#define NV_PDMA_GR_STATUS_1_CONTROL                             0:0 /* RWIVF */
#define NV_PDMA_GR_STATUS_1_CONTROL_INVALID              0x00000000 /* RW--V */
#define NV_PDMA_GR_STATUS_1_CONTROL_VALID                0x00000001 /* RWI-V */
#define NV_PDMA_GR_STATUS_1_OFFSET                              4:4 /* RWIVF */
#define NV_PDMA_GR_STATUS_1_OFFSET_INVALID               0x00000000 /* RWI-V */
#define NV_PDMA_GR_STATUS_1_OFFSET_VALID                 0x00000001 /* RW--V */
#define NV_PDMA_GR_STATUS_1_TLB_PTE                             8:8 /* RWIVF */
#define NV_PDMA_GR_STATUS_1_TLB_PTE_INVALID              0x00000000 /* RW--V */
#define NV_PDMA_GR_STATUS_1_TLB_PTE_VALID                0x00000001 /* RWI-V */
#define NV_PDMA_GR_STATUS_1_ADJ_OFFSET32                      12:12 /* RWXUF */
#define NV_PDMA_AU_TLB_TAG                               0x00100620 /* RW-4R */
#define NV_PDMA_AU_TLB_TAG_ADDRESS                            31:12 /* RWXUF */
#define NV_PDMA_GR_TLB_TAG                               0x00100820 /* RW-4R */
#define NV_PDMA_GR_TLB_TAG_ADDRESS                            31:12 /* RWXUF */
#define NV_PDMA_AU_ADJ_OFFSET                            0x00100630 /* RW-4R */
#define NV_PDMA_AU_ADJ_OFFSET_VALUE                            31:0 /* RWXUF */
#define NV_PDMA_GR_ADJ_OFFSET                            0x00100830 /* RW-4R */
#define NV_PDMA_GR_ADJ_OFFSET_VALUE                            31:0 /* RWXUF */
#define NV_PDMA_RM_PHY_START                             0x00100440 /* RW-4R */
#define NV_PDMA_RM_PHY_START_ADDRESS                           31:0 /* RWXUF */
#define NV_PDMA_AU_PHY_START                             0x00100640 /* RW-4R */
#define NV_PDMA_AU_PHY_START_ADDRESS                           31:0 /* RWXUF */
#define NV_PDMA_GR_PHY_START                             0x00100840 /* RW-4R */
#define NV_PDMA_GR_PHY_START_ADDRESS                           31:0 /* RWXUF */
#define NV_PDMA_RM_BUFF_OUT(i)                   (0x00100500+(i)*4) /* RW-4A */
#define NV_PDMA_RM_BUFF_OUT__SIZE_1                               1 /*       */
#define NV_PDMA_RM_BUFF_OUT_DATA                               31:0 /* RWXVF */
#define NV_PDMA_AU_BUFF_OUT(i)                   (0x00100700+(i)*4) /* RW-4A */
#define NV_PDMA_AU_BUFF_OUT__SIZE_1                               4 /*       */
#define NV_PDMA_AU_BUFF_OUT_DATA                               31:0 /* RWXVF */
#define NV_PDMA_GR_BUFF_OUT(i)                   (0x00100900+(i)*4) /* RW-4A */
#define NV_PDMA_GR_BUFF_OUT__SIZE_1                               8 /*       */
#define NV_PDMA_GR_BUFF_OUT_DATA                               31:0 /* RWXVF */
#define NV_PDMA_BUFF_IN(i)                       (0x00100580+(i)*4) /* RW-4A */
#define NV_PDMA_BUFF_IN__SIZE_1                                   8 /*       */
#define NV_PDMA_BUFF_IN_DATA                                   31:0 /* RWXVF */
#define NV_PDMA_AU_INSTANCE                              0x00100680 /* RW-4R */
#define NV_PDMA_AU_INSTANCE_ID                                 15:0 /* RWIUF */
#define NV_PDMA_AU_INSTANCE_ID_0                         0x00000000 /* RWI-V */
#define NV_PDMA_GR_INSTANCE                              0x00100880 /* RW-4R */
#define NV_PDMA_GR_INSTANCE_ID                                 15:0 /* RWIUF */
#define NV_PDMA_GR_INSTANCE_ID_0                         0x00000000 /* RWI-V */
#define NV_PDMA_AU_OFFSET                                0x00100690 /* -W-4R */
#define NV_PDMA_AU_OFFSET_VALUE                                31:0 /* -W-UF */
#define NV_PDMA_GR_OFFSET                                0x00100890 /* -W-4R */
#define NV_PDMA_GR_OFFSET_VALUE                                31:0 /* -W-UF */
#define NV_PDMA_AU_OUT8                                  0x001006A0 /* -W-1R */
#define NV_PDMA_AU_OUT8_DATA                                    7:0 /* -W-VF */
#define NV_PDMA_AU_OUT16                                 0x001006A0 /* -W-2R */
#define NV_PDMA_AU_OUT16_DATA                                  15:0 /* -W-VF */
#define NV_PDMA_AU_OUT32                                 0x001006A0 /* -W-4R */
#define NV_PDMA_AU_OUT32_DATA                                  31:0 /* -W-VF */
#define NV_PDMA_GR_OUT8                                  0x001008A0 /* -W-1R */
#define NV_PDMA_GR_OUT8_DATA                                    7:0 /* -W-VF */
#define NV_PDMA_GR_OUT16                                 0x001008A0 /* -W-2R */
#define NV_PDMA_GR_OUT16_DATA                                  15:0 /* -W-VF */
#define NV_PDMA_GR_OUT32                                 0x001008A0 /* -W-4R */
#define NV_PDMA_GR_OUT32_DATA                                  31:0 /* -W-VF */
#define NV_PDMA_RM_FLUSH8                                0x001004B0 /* -W-1R */
#define NV_PDMA_RM_FLUSH8_DATA                                  7:0 /* -W-VF */
#define NV_PDMA_RM_FLUSH16                               0x001004B0 /* -W-2R */
#define NV_PDMA_RM_FLUSH16_DATA                                15:0 /* -W-VF */
#define NV_PDMA_RM_FLUSH32                               0x001004B0 /* -W-4R */
#define NV_PDMA_RM_FLUSH32_DATA                                31:0 /* -W-VF */
#define NV_PDMA_AU_FLUSH8                                0x001006B0 /* -W-1R */
#define NV_PDMA_AU_FLUSH8_DATA                                  7:0 /* -W-VF */
#define NV_PDMA_AU_FLUSH16                               0x001006B0 /* -W-2R */
#define NV_PDMA_AU_FLUSH16_DATA                                15:0 /* -W-VF */
#define NV_PDMA_AU_FLUSH32                               0x001006B0 /* -W-4R */
#define NV_PDMA_AU_FLUSH32_DATA                                31:0 /* -W-VF */
#define NV_PDMA_GR_FLUSH8                                0x001008B0 /* -W-1R */
#define NV_PDMA_GR_FLUSH8_DATA                                  7:0 /* -W-VF */
#define NV_PDMA_GR_FLUSH16                               0x001008B0 /* -W-2R */
#define NV_PDMA_GR_FLUSH16_DATA                                15:0 /* -W-VF */
#define NV_PDMA_GR_FLUSH32                               0x001008B0 /* -W-4R */
#define NV_PDMA_GR_FLUSH32_DATA                                31:0 /* -W-VF */
#define NV_PDMA_AU_FLUSH_BUFF                            0x001006C0 /* -W-4R */
#define NV_PDMA_AU_FLUSH_BUFF_NOW                              31:0 /* -W-VF */
#define NV_PDMA_GR_FLUSH_BUFF                            0x001008C0 /* -W-4R */
#define NV_PDMA_GR_FLUSH_BUFF_NOW                              31:0 /* -W-VF */
#define NV_PDMA_RM_IN                                    0x001004D0 /* -W-4R */
#define NV_PDMA_RM_IN_REPLY_ADDRESS                            23:0 /* -W-VF */
#define NV_PDMA_AU_IN                                    0x001006D0 /* -W-4R */
#define NV_PDMA_AU_IN_REPLY_ADDRESS                            23:0 /* -W-VF */
#define NV_PDMA_GR_IN                                    0x001008D0 /* -W-4R */
#define NV_PDMA_GR_IN_REPLY_ADDRESS                            23:0 /* -W-VF */
#define NV_PDMA_AU_NOTIFY                                0x001006E0 /* -W-4R */
#define NV_PDMA_AU_NOTIFY_INSTANCE                             15:0 /* -W-UF */
#define NV_PDMA_GR_NOTIFY                                0x001008E0 /* -W-4R */
#define NV_PDMA_GR_NOTIFY_INSTANCE                             15:0 /* -W-UF */
/* dev_timer.ref */
#define NV_PTIMER                             0x00101FFF:0x00101000 /* RW--D */
#define NV_PTIMER_INTR_0                                 0x00101100 /* RW-4R */
#define NV_PTIMER_INTR_0_ALARM                                  0:0 /* RWXVF */
#define NV_PTIMER_INTR_0_ALARM_NOT_PENDING               0x00000000 /* R---V */
#define NV_PTIMER_INTR_0_ALARM_PENDING                   0x00000001 /* R---V */
#define NV_PTIMER_INTR_0_ALARM_RESET                     0x00000001 /* -W--V */
#define NV_PTIMER_INTR_EN_0                              0x00101140 /* RW-4R */
#define NV_PTIMER_INTR_EN_0_ALARM                               0:0 /* RWIVF */
#define NV_PTIMER_INTR_EN_0_ALARM_DISABLED               0x00000000 /* RWI-V */
#define NV_PTIMER_INTR_EN_0_ALARM_ENABLED                0x00000001 /* RW--V */
#define NV_PTIMER_NUMERATOR                              0x00101200 /* RW-4R */
#define NV_PTIMER_NUMERATOR_VALUE                              15:0 /* RWIUF */
#define NV_PTIMER_NUMERATOR_VALUE_0                      0x00000000 /* RWI-V */
#define NV_PTIMER_DENOMINATOR                            0x00101210 /* RW-4R */
#define NV_PTIMER_DENOMINATOR_VALUE                            15:0 /* RWIUF */
#define NV_PTIMER_DENOMINATOR_VALUE_0                    0x00000000 /* RWI-V */
#define NV_PTIMER_TIME_0                                 0x00101400 /* RW-4R */
#define NV_PTIMER_TIME_0_NSEC                                  31:5 /* RWXUF */
#define NV_PTIMER_TIME_1                                 0x00101404 /* RW-4R */
#define NV_PTIMER_TIME_1_NSEC                                  28:0 /* RWXUF */
#define NV_PTIMER_ALARM_0                                0x00101410 /* RW-4R */
#define NV_PTIMER_ALARM_0_NSEC                                 31:5 /* RWXUF */
/* dev_audio.ref */
#define NV_PAUDIO                             0x00300FFF:0x00300000 /* RW--D */
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
#define NV_PAUDIO_INTR_0                                 0x00300100 /* RW-4R */
#define NV_PAUDIO_INTR_0_NOTIFY                                 0:0 /* RWIVF */
#define NV_PAUDIO_INTR_0_NOTIFY_NOT_PENDING              0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_0_NOTIFY_PENDING                  0x00000001 /* R---V */
#define NV_PAUDIO_INTR_0_NOTIFY_RESET                    0x00000001 /* -W--V */
#define NV_PAUDIO_INTR_0_OVERFLOW                               4:4 /* RWIVF */
#define NV_PAUDIO_INTR_0_OVERFLOW_NOT_PENDING            0x00000000 /* R-I-V */
#define NV_PAUDIO_INTR_0_OVERFLOW_PENDING                0x00000001 /* R---V */
#define NV_PAUDIO_INTR_0_OVERFLOW_RESET                  0x00000001 /* -W--V */
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
#define NV_PAUDIO_INTR_EN_0                              0x00300140 /* RW-4R */
#define NV_PAUDIO_INTR_EN_0_NOTIFY                              0:0 /* RWIVF */
#define NV_PAUDIO_INTR_EN_0_NOTIFY_DISABLED              0x00000000 /* RWI-V */
#define NV_PAUDIO_INTR_EN_0_NOTIFY_ENABLED               0x00000001 /* RW--V */
#define NV_PAUDIO_INTR_EN_0_OVERFLOW                            4:4 /* RWIVF */
#define NV_PAUDIO_INTR_EN_0_OVERFLOW_DISABLED            0x00000000 /* RWI-V */
#define NV_PAUDIO_INTR_EN_0_OVERFLOW_ENABLED             0x00000001 /* RW--V */
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
#define NV_PAUDIO_CONTEXT                                0x00300A08 /* RW-4R */
#define NV_PAUDIO_CONTEXT_INSTANCE                             15:0 /* RWXUF */
#define NV_PAUDIO_BLOCK_NEW                              0x00300200 /* RW-4R */
#define NV_PAUDIO_BLOCK_NEW_LENGTH                              7:0 /* RWXUF */
#define NV_PAUDIO_BLOCK_NEW_RATIO                             13:12 /* RWXVF */
#define NV_PAUDIO_BLOCK_NEW_RATIO_1X                     0x00000000 /* RW--V */
#define NV_PAUDIO_BLOCK_NEW_RATIO_2X                     0x00000001 /* RW--V */
#define NV_PAUDIO_BLOCK_NEW_RATIO_4X                     0x00000002 /* RW--V */
#define NV_PAUDIO_BLOCK_NEW_RATIO_8X                     0x00000003 /* RW--V */
#define NV_PAUDIO_BLOCK_ENGINE                           0x00300204 /* RW-4R */
#define NV_PAUDIO_BLOCK_ENGINE_LENGTH                           7:0 /* RWXUF */
#define NV_PAUDIO_BLOCK_ENGINE_RATIO                          13:12 /* RWXVF */
#define NV_PAUDIO_BLOCK_ENGINE_RATIO_1X                  0x00000000 /* RW--V */
#define NV_PAUDIO_BLOCK_ENGINE_RATIO_2X                  0x00000001 /* RW--V */
#define NV_PAUDIO_BLOCK_ENGINE_RATIO_4X                  0x00000002 /* RW--V */
#define NV_PAUDIO_BLOCK_ENGINE_RATIO_8X                  0x00000003 /* RW--V */
#define NV_PAUDIO_BLOCK_PUMP                             0x00300208 /* RW-4R */
#define NV_PAUDIO_BLOCK_PUMP_LENGTH                             7:0 /* R-XUF */
#define NV_PAUDIO_BLOCK_PUMP_RATIO                            13:12 /* RWXVF */
#define NV_PAUDIO_BLOCK_PUMP_RATIO_1X                    0x00000000 /* RW--V */
#define NV_PAUDIO_BLOCK_PUMP_RATIO_2X                    0x00000001 /* RW--V */
#define NV_PAUDIO_BLOCK_PUMP_RATIO_4X                    0x00000002 /* RW--V */
#define NV_PAUDIO_BLOCK_PUMP_RATIO_8X                    0x00000003 /* RW--V */
#define NV_PAUDIO_NEAR_MARK                              0x00300400 /* RW-4R */
#define NV_PAUDIO_NEAR_MARK_COUNT                               7:0 /* RWXUF */
#define NV_PAUDIO_SAMPLE_COUNT                           0x00300410 /* R--4R */
#define NV_PAUDIO_SAMPLE_COUNT_VALUE                            7:0 /* R-XUF */
#define NV_PAUDIO_SAMPLE_COUNT_BUFFER                           8:8 /* R-XUF */
#define NV_PAUDIO_TERMINATION                            0x00300980 /* RW-4R */
#define NV_PAUDIO_TERMINATION_LEVEL                            15:0 /* RWXUF */
#define NV_PAUDIO_TERMINATION_LEVEL_DISABLED             0x00000000 /* RW--V */
#define NV_PAUDIO_USAGE                                  0x0030080C /* RW-4R */
#define NV_PAUDIO_USAGE_LEVEL                                  15:0 /* RWXUF */
#define NV_PAUDIO_CODEC(i)                      (0x00300500+(i)*16) /* RW-4A */
#define NV_PAUDIO_CODEC__SIZE_1                                   4 /*       */
#define NV_PAUDIO_CODEC_DATA                                    7:0 /* RW-VF */
#define NV_PAUDIO_CACHE_ANALOG                           0x00300600 /* -W-4R */
#define NV_PAUDIO_CACHE_ANALOG_CHANNEL_0                       15:0 /* -W-VF */
#define NV_PAUDIO_CACHE_ANALOG_CHANNEL_1                      31:16 /* -W-VF */
#define NV_PAUDIO_CACHE_INPUT                            0x00300610 /* -W-4R */
#define NV_PAUDIO_CACHE_INPUT_CHANNEL_0                        15:0 /* -W-VF */
#define NV_PAUDIO_CACHE_INPUT_CHANNEL_1                       31:16 /* -W-VF */
#define NV_PAUDIO_CACHE_OUTPUT                           0x00300620 /* -W-4R */
#define NV_PAUDIO_CACHE_OUTPUT_CHANNEL_0                       15:0 /* -W-VF */
#define NV_PAUDIO_CACHE_OUTPUT_CHANNEL_1                      31:16 /* -W-VF */
#define NV_PAUDIO_FETCH(i)                       (0x00300640+(i)*4) /* -W-4A */
#define NV_PAUDIO_FETCH__SIZE_1                                   4 /*       */
#define NV_PAUDIO_FETCH_DATA                                   31:0 /* -W-VF */
#define NV_PAUDIO_TIME_RETURN(i)                 (0x00300660+(i)*4) /* -W-4A */
#define NV_PAUDIO_TIME_RETURN__SIZE_1                             2 /*       */
#define NV_PAUDIO_TIME_RETURN_VALUE                            31:0 /* -W-VF */
#define NV_PAUDIO_HEADER(i)                      (0x00300680+(i)*4) /* -W-4A */
#define NV_PAUDIO_HEADER__SIZE_1                                 32 /*       */
#define NV_PAUDIO_HEADER_DATA                                  31:0 /* -W-VF */
#define NV_PAUDIO_ROOT_INPUT                             0x00300800 /* RW-4R */
#define NV_PAUDIO_ROOT_INPUT_INSTANCE                          15:0 /* RWXUF */
#define NV_PAUDIO_ROOT_OUTPUT                            0x00300804 /* RW-4R */
#define NV_PAUDIO_ROOT_OUTPUT_INSTANCE                         15:0 /* RWXUF */
#define NV_PAUDIO_ROOT_NOTE                              0x00300808 /* RW-4R */
#define NV_PAUDIO_ROOT_NOTE_INSTANCE                           15:0 /* RWXUF */
#define NV_PAUDIO_DIAG(i)                        (0x0030080C+(i)*4) /* RW-4A */
#define NV_PAUDIO_DIAG__SIZE_1                                  253 /*       */
#define NV_PAUDIO_DIAG_DATA                                    31:0 /* RW-VF */
/* dev_graphics.ref */
#define NV_PGRAPH                             0x00400FFF:0x00400000 /* RW--D */
#define NV_PGRAPH_DEBUG_0                                0x00400080 /* RW-4R */
#define NV_PGRAPH_DEBUG_0_STATE                                 0:0 /* CW-VF */
#define NV_PGRAPH_DEBUG_0_STATE_NORMAL                   0x00000000 /* CW--V */
#define NV_PGRAPH_DEBUG_0_STATE_RESET                    0x00000001 /* -W--V */
#define NV_PGRAPH_DEBUG_0_BULK_READS                            4:4 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_BULK_READS_DISABLED            0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_BULK_READS_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_BLOCK                                 8:8 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_BLOCK_DISABLED                 0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_BLOCK_ENABLED                  0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_BLOCK_BROAD                         12:12 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_BLOCK_BROAD_DISABLED           0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_BLOCK_BROAD_ENABLED            0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_NONBLOCK_BROAD                      16:16 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_NONBLOCK_BROAD_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_NONBLOCK_BROAD_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS                     20:20 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS_DISABLED       0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_WRITE_ONLY_ROPS_ENABLED        0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_0_EDGE_FILLING                        24:24 /* RWIVF */
#define NV_PGRAPH_DEBUG_0_EDGE_FILLING_DISABLED          0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_0_EDGE_FILLING_ENABLED           0x00000001 /* RW--V */
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
#define NV_PGRAPH_DEBUG_1_BI_RECTS                              8:8 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_BI_RECTS_DISABLED              0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_BI_RECTS_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_TRI_OPTS                            12:12 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_TRI_OPTS_DISABLED              0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_TRI_OPTS_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_PATT_BLOCK                          16:16 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_PATT_BLOCK_DISABLED            0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_PATT_BLOCK_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_FAST_RMW_BLITS                      20:20 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_FAST_RMW_BLITS_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_FAST_RMW_BLITS_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_TM_QUAD_HANDOFF                     24:24 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_TM_QUAD_HANDOFF_DISABLED       0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_TM_QUAD_HANDOFF_ENABLED        0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_FAST_BUS                            28:28 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_FAST_BUS_DISABLED              0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_FAST_BUS_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_1_HIRES_TM                            29:29 /* RWIVF */
#define NV_PGRAPH_DEBUG_1_HIRES_TM_DISABLED              0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_1_HIRES_TM_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2                                0x00400088 /* RW-4R */
#define NV_PGRAPH_DEBUG_2_AVOID_RMW_BLEND                       0:0 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_AVOID_RMW_BLEND_DISABLED       0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_AVOID_RMW_BLEND_ENABLED        0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_ALPHA_ABORT                           4:4 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_ALPHA_ABORT_DISABLED           0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_ALPHA_ABORT_ENABLED            0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_BETA_ABORT                            8:8 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_BETA_ABORT_DISABLED            0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_BETA_ABORT_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_MONO_ABORT                          12:12 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_MONO_ABORT_DISABLED            0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_MONO_ABORT_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_TRAPEZOID_TEXEL                     16:16 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_TRAPEZOID_TEXEL_DISABLED       0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_TRAPEZOID_TEXEL_ENABLED        0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_BUSY_PATIENCE                       20:20 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_BUSY_PATIENCE_DISABLED         0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_BUSY_PATIENCE_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_TM_FASTINPUT                        24:24 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_TM_FASTINPUT_DISABLED          0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_TM_FASTINPUT_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_2_VOLATILE_RESET                      28:28 /* RWIVF */
#define NV_PGRAPH_DEBUG_2_VOLATILE_RESET_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_2_VOLATILE_RESET_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3                                0x0040008c /* RW-4R */
#define NV_PGRAPH_DEBUG_3_TM_RANGE_INTERRUPT                    0:0 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_TM_RANGE_INTERRUPT_DISABLED    0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_TM_RANGE_INTERRUPT_ENABLED     0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_MONO_BLOCK                            4:4 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_MONO_BLOCK_DISABLED            0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_MONO_BLOCK_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_SPARE1                                8:8 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_SPARE1_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_SPARE1_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_SPARE2                              12:12 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_SPARE2_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_SPARE2_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_SPARE3                              16:16 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_SPARE3_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_SPARE3_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_SPARE4                              20:20 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_SPARE4_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_SPARE4_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_SPARE5                              24:24 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_SPARE5_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_SPARE5_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_DEBUG_3_SPARE6                              28:28 /* RWIVF */
#define NV_PGRAPH_DEBUG_3_SPARE6_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_DEBUG_3_SPARE6_ENABLED                 0x00000001 /* RW--V */
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
#define NV_PGRAPH_INTR_0_SOFTWARE                             20:20 /* RWIVF */
#define NV_PGRAPH_INTR_0_SOFTWARE_NOT_PENDING            0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_0_SOFTWARE_PENDING                0x00000001 /* R---V */
#define NV_PGRAPH_INTR_0_SOFTWARE_RESET                  0x00000001 /* -W--V */
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
#define NV_PGRAPH_INTR_1_NOTIFY_INST                            8:8 /* RWIVF */
#define NV_PGRAPH_INTR_1_NOTIFY_INST_NOT_PENDING         0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_1_NOTIFY_INST_PENDING             0x00000001 /* R---V */
#define NV_PGRAPH_INTR_1_NOTIFY_INST_RESET               0x00000001 /* -W--V */
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
#define NV_PGRAPH_INTR_EN_0_SOFTWARE                          20:20 /* RWIVF */
#define NV_PGRAPH_INTR_EN_0_SOFTWARE_DISABLED            0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_0_SOFTWARE_ENABLED             0x00000001 /* RW--V */
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
#define NV_PGRAPH_INTR_EN_1_NOTIFY_INST                         8:8 /* RWIVF */
#define NV_PGRAPH_INTR_EN_1_NOTIFY_INST_DISABLED         0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_1_NOTIFY_INST_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_1_DOUBLE_NOTIFY                     12:12 /* RWIVF */
#define NV_PGRAPH_INTR_EN_1_DOUBLE_NOTIFY_DISABLED       0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_1_DOUBLE_NOTIFY_ENABLED        0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_1_CTXSW_NOTIFY                      16:16 /* RWIVF */
#define NV_PGRAPH_INTR_EN_1_CTXSW_NOTIFY_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_1_CTXSW_NOTIFY_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH                             0x00400180 /* RW-4R */
#define NV_PGRAPH_CTX_SWITCH_INSTANCE                          15:0 /* RW-VF */
#define NV_PGRAPH_CTX_SWITCH_PATCH_CONFIG                       4:0 /* RWXVF */
#define NV_PGRAPH_CTX_SWITCH_CHROMA_KEY                         5:5 /* RWXVF */
#define NV_PGRAPH_CTX_SWITCH_CHROMA_KEY_DISABLED         0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_CHROMA_KEY_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_BETA_MAXIMUM                       5:5 /* RWXVF */
#define NV_PGRAPH_CTX_SWITCH_BETA_MAXIMUM_DISABLED       0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_BETA_MAXIMUM_ENABLED        0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PLANE_MASK                         6:6 /* RWXVF */
#define NV_PGRAPH_CTX_SWITCH_PLANE_MASK_DISABLED         0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PLANE_MASK_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_USER_CLIP                          7:7 /* RWXVF */
#define NV_PGRAPH_CTX_SWITCH_USER_CLIP_DISABLED          0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_USER_CLIP_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_NOTIFY                             8:8 /* RWXVF */
#define NV_PGRAPH_CTX_SWITCH_NOTIFY_INVALID              0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_NOTIFY_VALID                0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_COLOR_BUFFER                      12:9 /* RWXVF */
#define NV_PGRAPH_CTX_SWITCH_ALPHA                            13:13 /* RWXVF */
#define NV_PGRAPH_CTX_SWITCH_ALPHA_DISABLED              0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_ALPHA_ENABLED               0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_SOURCE_BUFFER                    13:13 /* RW-VF */
#define NV_PGRAPH_CTX_SWITCH_MONO_FORMAT                      14:14 /* RWXVF */
#define NV_PGRAPH_CTX_SWITCH_MONO_FORMAT_LE              0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_MONO_FORMAT_CGA6            0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH_PATCH_ID                         15:15 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_PATCH_ID_0                  0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_CHID                             22:16 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_CHID_0                      0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_OBJECT                           31:31 /* RWIVF */
#define NV_PGRAPH_CTX_SWITCH_OBJECT_UNCHANGED            0x00000000 /* RWI-V */
#define NV_PGRAPH_CTX_SWITCH_OBJECT_CHANGED              0x00000001 /* RW--V */
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
#define NV_PGRAPH_MISC                                   0x004006A4 /* RW-4R */
#define NV_PGRAPH_MISC_FIFO                                     0:0 /* RWIVF */
#define NV_PGRAPH_MISC_FIFO_DISABLED                     0x00000000 /* RW--V */
#define NV_PGRAPH_MISC_FIFO_ENABLED                      0x00000001 /* RWI-V */
#define NV_PGRAPH_MISC_DMA                                      4:4 /* RWIVF */
#define NV_PGRAPH_MISC_DMA_DISABLED                      0x00000000 /* RW--V */
#define NV_PGRAPH_MISC_DMA_ENABLED                       0x00000001 /* RWI-V */
#define NV_PGRAPH_MISC_FLOWTHRU                                 8:8 /* RWIVF */
#define NV_PGRAPH_MISC_FLOWTHRU_DISABLED                 0x00000000 /* RW--V */
#define NV_PGRAPH_MISC_FLOWTHRU_ENABLED                  0x00000001 /* RWI-V */
#define NV_PGRAPH_MISC_CLASS                                  16:12 /* RWXVF */
#define NV_PGRAPH_MISC_FIFO_WRITE                             24:24 /* CW-VF */
#define NV_PGRAPH_MISC_FIFO_WRITE_IGNORED                0x00000000 /* -W--V */
#define NV_PGRAPH_MISC_FIFO_WRITE_ENABLED                0x00000001 /* CW--V */
#define NV_PGRAPH_MISC_DMA_WRITE                              25:25 /* CW-VF */
#define NV_PGRAPH_MISC_DMA_WRITE_IGNORED                 0x00000000 /* -W--V */
#define NV_PGRAPH_MISC_DMA_WRITE_ENABLED                 0x00000001 /* CW--V */
#define NV_PGRAPH_MISC_FLOWTHRU_WRITE                         26:26 /* CW-VF */
#define NV_PGRAPH_MISC_FLOWTHRU_WRITE_IGNORED            0x00000000 /* -W--V */
#define NV_PGRAPH_MISC_FLOWTHRU_WRITE_ENABLED            0x00000001 /* CW--V */
#define NV_PGRAPH_MISC_CLASS_WRITE                            27:27 /* CW-VF */
#define NV_PGRAPH_MISC_CLASS_WRITE_IGNORED               0x00000000 /* -W--V */
#define NV_PGRAPH_MISC_CLASS_WRITE_ENABLED               0x00000001 /* CW--V */
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
#define NV_PGRAPH_STATUS_DMA_NOTIFY                           20:20 /* R-IVF */
#define NV_PGRAPH_STATUS_DMA_NOTIFY_IDLE                 0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_DMA_NOTIFY_BUSY                 0x00000001 /* R---V */
#define NV_PGRAPH_STATUS_PORT_FIFO                            24:24 /* R-IVF */
#define NV_PGRAPH_STATUS_PORT_FIFO_IDLE                  0x00000000 /* R-I-V */
#define NV_PGRAPH_STATUS_PORT_FIFO_BUSY                  0x00000001 /* R---V */
#define NV_PGRAPH_TRAPPED_ADDR                           0x004006A8 /* R--4R */
#define NV_PGRAPH_TRAPPED_ADDR_VALUE                           20:2 /* R-XUF */
#define NV_PGRAPH_TRAPPED_DATA                           0x004006AC /* R--4R */
#define NV_PGRAPH_TRAPPED_DATA_VALUE                           31:0 /* R-XVF */
#define NV_PGRAPH_CANVAS_MISC                            0x00400634 /* RW-4R */
#define NV_PGRAPH_CANVAS_MISC_DAC_BYPASS                        0:0 /* RWXVF */
#define NV_PGRAPH_CANVAS_MISC_DAC_BYPASS_DISABLED        0x00000000 /* RW--V */
#define NV_PGRAPH_CANVAS_MISC_DAC_BYPASS_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_CANVAS_MISC_RETAINED                          4:4 /* RWXVF */
#define NV_PGRAPH_CANVAS_MISC_RETAINED_DISABLED          0x00000000 /* RW--V */
#define NV_PGRAPH_CANVAS_MISC_RETAINED_ENABLED           0x00000001 /* RW--V */
#define NV_PGRAPH_CANVAS_MISC_DAC_DECODE                      12:12 /* RWXVF */
#define NV_PGRAPH_CANVAS_MISC_DAC_DECODE_SINGLE          0x00000000 /* RW--V */
#define NV_PGRAPH_CANVAS_MISC_DAC_DECODE_TRIPLE          0x00000001 /* RW--V */
#define NV_PGRAPH_CANVAS_MISC_DITHER                          16:16 /* RWXVF */
#define NV_PGRAPH_CANVAS_MISC_DITHER_DISABLED            0x00000000 /* RW--V */
#define NV_PGRAPH_CANVAS_MISC_DITHER_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_CANVAS_MISC_REPLICATE                       20:20 /* RWXVF */
#define NV_PGRAPH_CANVAS_MISC_REPLICATE_DISABLED         0x00000000 /* RW--V */
#define NV_PGRAPH_CANVAS_MISC_REPLICATE_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_CANVAS_MISC_SOFTWARE                        24:24 /* RWXVF */
#define NV_PGRAPH_CANVAS_MISC_SOFTWARE_DISABLED          0x00000000 /* RW--V */
#define NV_PGRAPH_CANVAS_MISC_SOFTWARE_ENABLED           0x00000001 /* RW--V */
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
#define NV_PGRAPH_CANVAS_MIN                             0x00400688 /* RW-4R */
#define NV_PGRAPH_CANVAS_MIN_X                                 15:0 /* RWXSF */
#define NV_PGRAPH_CANVAS_MIN_Y                                31:16 /* RWXSF */
#define NV_PGRAPH_CANVAS_MAX                             0x0040068C /* RW-4R */
#define NV_PGRAPH_CANVAS_MAX_X                                 11:0 /* RWXUF */
#define NV_PGRAPH_CANVAS_MAX_Y                                27:16 /* RWXUF */
#define NV_PGRAPH_CLIP0_MIN                              0x00400690 /* RW-4R */
#define NV_PGRAPH_CLIP0_MIN_X                                  11:0 /* RWXSF */
#define NV_PGRAPH_CLIP0_MIN_Y                                 27:16 /* RWXSF */
#define NV_PGRAPH_CLIP1_MIN                              0x00400698 /* RW-4R */
#define NV_PGRAPH_CLIP1_MIN_X                                  11:0 /* RWXSF */
#define NV_PGRAPH_CLIP1_MIN_Y                                 27:16 /* RWXSF */
#define NV_PGRAPH_CLIP0_MAX                              0x00400694 /* RW-4R */
#define NV_PGRAPH_CLIP0_MAX_X                                  11:0 /* RWXSF */
#define NV_PGRAPH_CLIP0_MAX_Y                                 27:16 /* RWXSF */
#define NV_PGRAPH_CLIP1_MAX                              0x0040069C /* RW-4R */
#define NV_PGRAPH_CLIP1_MAX_X                                  11:0 /* RWXSF */
#define NV_PGRAPH_CLIP1_MAX_Y                                 27:16 /* RWXSF */
#define NV_PGRAPH_DMA                                    0x00400680 /* RW-4R */
#define NV_PGRAPH_DMA_INSTANCE                                 15:0 /* RWXUF */
#define NV_PGRAPH_NOTIFY                                 0x00400684 /* RW-4R */
#define NV_PGRAPH_NOTIFY_INSTANCE                              15:0 /* RWXUF */
#define NV_PGRAPH_NOTIFY_WRITE                                16:16 /* RWIVF */
#define NV_PGRAPH_NOTIFY_WRITE_NOT_PENDING               0x00000000 /* RWI-V */
#define NV_PGRAPH_NOTIFY_WRITE_PENDING                   0x00000001 /* RW--V */
#define NV_PGRAPH_NOTIFY_INTERRUPT                            20:20 /* RWIVF */
#define NV_PGRAPH_NOTIFY_INTERRUPT_NOT_PENDING           0x00000000 /* RWI-V */
#define NV_PGRAPH_NOTIFY_INTERRUPT_PENDING               0x00000001 /* RW--V */
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
#define NV_PGRAPH_MONO_COLOR1                            0x00400620 /* RW-4R */
#define NV_PGRAPH_MONO_COLOR1_BLUE                              9:0 /* RWXUF */
#define NV_PGRAPH_MONO_COLOR1_GREEN                           19:10 /* RWXUF */
#define NV_PGRAPH_MONO_COLOR1_RED                             29:20 /* RWXUF */
#define NV_PGRAPH_MONO_COLOR1_ALPHA                           30:30 /* RWXUF */
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
#define NV_PGRAPH_BETA                                   0x00400630 /* RW-4R */
#define NV_PGRAPH_BETA_VALUE_FRACTION                         30:23 /* RWXUF */
#define NV_PGRAPH_ABS_X_RAM(i)                   (0x00400400+(i)*4) /* RW-4A */
#define NV_PGRAPH_ABS_X_RAM__SIZE_1                              18 /*       */
#define NV_PGRAPH_ABS_X_RAM_VALUE                              31:0 /* RWXUF */
#define NV_PGRAPH_REL_X_RAM(i)                   (0x00400500+(i)*4) /* RW-4A */
#define NV_PGRAPH_REL_X_RAM__SIZE_1                              18 /*       */
#define NV_PGRAPH_REL_X_RAM_VALUE                              31:0 /* -W-UF */
#define NV_PGRAPH_X_RAM_BPORT(i)                 (0x00400c00+(i)*4) /* R--4A */
#define NV_PGRAPH_X_RAM_BPORT__SIZE_1                            18 /*       */
#define NV_PGRAPH_X_RAM_BPORT_VALUE                            31:0 /* R--UF */
#define NV_PGRAPH_ABS_Y_RAM(i)                   (0x00400480+(i)*4) /* RW-4A */
#define NV_PGRAPH_ABS_Y_RAM__SIZE_1                              18 /*       */
#define NV_PGRAPH_ABS_Y_RAM_VALUE                              31:0 /* RWXUF */
#define NV_PGRAPH_REL_Y_RAM(i)                   (0x00400580+(i)*4) /* RW-4A */
#define NV_PGRAPH_REL_Y_RAM__SIZE_1                              18 /*       */
#define NV_PGRAPH_REL_Y_RAM_VALUE                              31:0 /* -W-UF */
#define NV_PGRAPH_Y_RAM_BPORT(i)                 (0x00400c80+(i)*4) /* R--4A */
#define NV_PGRAPH_Y_RAM_BPORT__SIZE_1                            18 /*       */
#define NV_PGRAPH_Y_RAM_BPORT_VALUE                            31:0 /* R--UF */
#define NV_PGRAPH_XY_LOGIC_MISC0                         0x00400640 /* RW-4R */
#define NV_PGRAPH_XY_LOGIC_MISC0_MINOR                          8:0 /* RWIUF */
#define NV_PGRAPH_XY_LOGIC_MISC0_MINOR_0                 0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC0_DIMENSION                    12:12 /* RWXVF */
#define NV_PGRAPH_XY_LOGIC_MISC0_DIMENSION_NONZERO       0x00000000 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC0_DIMENSION_ZERO          0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC0_MAJOR                        24:16 /* RWIUF */
#define NV_PGRAPH_XY_LOGIC_MISC0_MAJOR_0                 0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC0_INDEX                        31:28 /* RWIUF */
#define NV_PGRAPH_XY_LOGIC_MISC0_INDEX_0                 0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1                         0x00400644 /* RW-4R */
#define NV_PGRAPH_XY_LOGIC_MISC1_INITIALIZE                     0:0 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_INITIALIZE_NEEDED       0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_INITIALIZE_DONE         0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_IMAGECLIPX                     4:4 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_IMAGECLIPX_NOTNULL      0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_IMAGECLIPX_NULL         0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_IMAGECLIPY                     5:5 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_IMAGECLIPY_NOTNULL      0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_IMAGECLIPY_NULL         0x00000001 /* RW--V */
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
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XIMAGE                   20:20 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XIMAGE_CLIPMAX      0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_SEL_XIMAGE_IMAGEMAX     0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_TM_COORD_FLAG                24:24 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_TM_COORD_FLAG_SET       0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_TM_COORD_FLAG_CLR       0x00000001 /* RW--V */
#define NV_PGRAPH_XY_LOGIC_MISC1_FRACT_FMT                    25:25 /* RWIVF */
#define NV_PGRAPH_XY_LOGIC_MISC1_FRACT_FMT_DISABLED      0x00000000 /* RWI-V */
#define NV_PGRAPH_XY_LOGIC_MISC1_FRACT_FMT_ENABLED       0x00000001 /* RW--V */
#define NV_PGRAPH_X_MISC                                 0x00400648 /* RW-4R */
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
#define NV_PGRAPH_X_MISC_CLIP0_MIN                              9:8 /* RWIVF */
#define NV_PGRAPH_X_MISC_CLIP0_MIN_GT                    0x00000000 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP0_MIN_LT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_X_MISC_CLIP0_MIN_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP0_MAX                            11:10 /* RWIVF */
#define NV_PGRAPH_X_MISC_CLIP0_MAX_LT                    0x00000000 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP0_MAX_GT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_X_MISC_CLIP0_MAX_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP1_MIN                            13:12 /* RWIVF */
#define NV_PGRAPH_X_MISC_CLIP1_MIN_GT                    0x00000000 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP1_MIN_LT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_X_MISC_CLIP1_MIN_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP1_MAX                            15:14 /* RWIVF */
#define NV_PGRAPH_X_MISC_CLIP1_MAX_LT                    0x00000000 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP1_MAX_GT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_X_MISC_CLIP1_MAX_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP2_MIN                            17:16 /* RWIVF */
#define NV_PGRAPH_X_MISC_CLIP2_MIN_GT                    0x00000000 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP2_MIN_LT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_X_MISC_CLIP2_MIN_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP2_MAX                            19:18 /* RWIVF */
#define NV_PGRAPH_X_MISC_CLIP2_MAX_LT                    0x00000000 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP2_MAX_GT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_X_MISC_CLIP2_MAX_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP3_MIN                            21:20 /* RWIVF */
#define NV_PGRAPH_X_MISC_CLIP3_MIN_GT                    0x00000000 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP3_MIN_LT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_X_MISC_CLIP3_MIN_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP3_MAX                            23:22 /* RWIVF */
#define NV_PGRAPH_X_MISC_CLIP3_MAX_LT                    0x00000000 /* RW--V */
#define NV_PGRAPH_X_MISC_CLIP3_MAX_GT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_X_MISC_CLIP3_MAX_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_X_MISC_ADDER_OUTPUT                         29:28 /* RWXVF */
#define NV_PGRAPH_X_MISC_ADDER_OUTPUT_EQ_0               0x00000000 /* RW--V */
#define NV_PGRAPH_X_MISC_ADDER_OUTPUT_LT_0               0x00000001 /* RW--V */
#define NV_PGRAPH_X_MISC_ADDER_OUTPUT_GT_0               0x00000002 /* RW--V */
#define NV_PGRAPH_Y_MISC                                 0x0040064c /* RW-4R */
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
#define NV_PGRAPH_Y_MISC_CLIP0_MIN                              9:8 /* RWIVF */
#define NV_PGRAPH_Y_MISC_CLIP0_MIN_GT                    0x00000000 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP0_MIN_LT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_Y_MISC_CLIP0_MIN_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP0_MAX                            11:10 /* RWIVF */
#define NV_PGRAPH_Y_MISC_CLIP0_MAX_LT                    0x00000000 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP0_MAX_GT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_Y_MISC_CLIP0_MAX_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP1_MIN                            13:12 /* RWIVF */
#define NV_PGRAPH_Y_MISC_CLIP1_MIN_GT                    0x00000000 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP1_MIN_LT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_Y_MISC_CLIP1_MIN_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP1_MAX                            15:14 /* RWIVF */
#define NV_PGRAPH_Y_MISC_CLIP1_MAX_LT                    0x00000000 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP1_MAX_GT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_Y_MISC_CLIP1_MAX_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP2_MIN                            17:16 /* RWIVF */
#define NV_PGRAPH_Y_MISC_CLIP2_MIN_GT                    0x00000000 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP2_MIN_LT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_Y_MISC_CLIP2_MIN_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP2_MAX                            19:18 /* RWIVF */
#define NV_PGRAPH_Y_MISC_CLIP2_MAX_LT                    0x00000000 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP2_MAX_GT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_Y_MISC_CLIP2_MAX_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP3_MIN                            21:20 /* RWIVF */
#define NV_PGRAPH_Y_MISC_CLIP3_MIN_GT                    0x00000000 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP3_MIN_LT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_Y_MISC_CLIP3_MIN_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP3_MAX                            23:22 /* RWIVF */
#define NV_PGRAPH_Y_MISC_CLIP3_MAX_LT                    0x00000000 /* RW--V */
#define NV_PGRAPH_Y_MISC_CLIP3_MAX_GT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_Y_MISC_CLIP3_MAX_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_Y_MISC_ADDER_OUTPUT                         29:28 /* RWXVF */
#define NV_PGRAPH_Y_MISC_ADDER_OUTPUT_EQ_0               0x00000000 /* RW--V */
#define NV_PGRAPH_Y_MISC_ADDER_OUTPUT_LT_0               0x00000001 /* RW--V */
#define NV_PGRAPH_Y_MISC_ADDER_OUTPUT_GT_0               0x00000002 /* RW--V */
#define NV_PGRAPH_ABS_UCLIP_XMIN                         0x00400460 /* RW-4R */
#define NV_PGRAPH_ABS_UCLIP_XMIN_VALUE                         17:0 /* RWXSF */
#define NV_PGRAPH_REL_UCLIP_XMIN                         0x00400560 /* -W-4R */
#define NV_PGRAPH_REL_UCLIP_XMIN_VALUE                         17:0 /* -W-SF */
#define NV_PGRAPH_ABS_UCLIP_XMAX                         0x00400464 /* RW-4R */
#define NV_PGRAPH_ABS_UCLIP_XMAX_VALUE                         17:0 /* RWXSF */
#define NV_PGRAPH_REL_UCLIP_XMAX                         0x00400564 /* -W-4R */
#define NV_PGRAPH_REL_UCLIP_XMAX_VALUE                         17:0 /* -W-SF */
#define NV_PGRAPH_ABS_UCLIP_YMIN                         0x00400468 /* RW-4R */
#define NV_PGRAPH_ABS_UCLIP_YMIN_VALUE                         17:0 /* RWXSF */
#define NV_PGRAPH_REL_UCLIP_YMIN                         0x00400568 /* -W-4R */
#define NV_PGRAPH_REL_UCLIP_YMIN_VALUE                         17:0 /* -W-SF */
#define NV_PGRAPH_ABS_UCLIP_YMAX                         0x0040046C /* RW-4R */
#define NV_PGRAPH_ABS_UCLIP_YMAX_VALUE                         17:0 /* RWXSF */
#define NV_PGRAPH_REL_UCLIP_YMAX                         0x0040056C /* -W-4R */
#define NV_PGRAPH_REL_UCLIP_YMAX_VALUE                         17:0 /* -W-SF */
#define NV_PGRAPH_SOURCE_COLOR                           0x00400654 /* RW-4R */
#define NV_PGRAPH_SOURCE_COLOR_VALUE                           31:0 /* RWXVF */
#define NV_PGRAPH_SUBDIVIDE                              0x00400658 /* RW-4R */
#define NV_PGRAPH_SUBDIVIDE_BTM_MAJOR_0123                      3:0 /* RWXVF */
#define NV_PGRAPH_SUBDIVIDE_QTM_MAJOR_0268                      3:0 /* RW-VF */
#define NV_PGRAPH_SUBDIVIDE_BTM_MINOR_0213                      7:4 /* RWXVF */
#define NV_PGRAPH_SUBDIVIDE_QTM_MINOR_0628                      7:4 /* RW-VF */
#define NV_PGRAPH_SUBDIVIDE_BTM_EDGE_01                       19:16 /* RWXVF */
#define NV_PGRAPH_SUBDIVIDE_QTM_EDGE_02                       19:16 /* RW-VF */
#define NV_PGRAPH_SUBDIVIDE_BTM_EDGE_23                       23:20 /* RWXVF */
#define NV_PGRAPH_SUBDIVIDE_QTM_EDGE_68                       23:20 /* RW-VF */
#define NV_PGRAPH_SUBDIVIDE_BTM_EDGE_02                       27:24 /* RWXVF */
#define NV_PGRAPH_SUBDIVIDE_QTM_EDGE_06                       27:24 /* RW-VF */
#define NV_PGRAPH_SUBDIVIDE_BTM_EDGE_13                       31:28 /* RWXVF */
#define NV_PGRAPH_SUBDIVIDE_QTM_EDGE_28                       31:28 /* RW-VF */
#define NV_PGRAPH_SUBDIVIDE_BY_1                         0x00000000 /* RW--V */
#define NV_PGRAPH_SUBDIVIDE_BY_2                         0x00000001 /* RW--V */
#define NV_PGRAPH_SUBDIVIDE_BY_4                         0x00000002 /* RW--V */
#define NV_PGRAPH_SUBDIVIDE_BY_8                         0x00000003 /* RW--V */
#define NV_PGRAPH_SUBDIVIDE_BY_16                        0x00000004 /* RW--V */
#define NV_PGRAPH_SUBDIVIDE_BY_32                        0x00000005 /* RW--V */
#define NV_PGRAPH_SUBDIVIDE_BY_64                        0x00000006 /* RW--V */
#define NV_PGRAPH_SUBDIVIDE_BY_128                       0x00000007 /* RW--V */
#define NV_PGRAPH_SUBDIVIDE_BY_256                       0x00000008 /* RW--V */
#define NV_PGRAPH_EXCEPTIONS                             0x00400650 /* RW-4R */
#define NV_PGRAPH_EXCEPTIONS_X_VALID                            8:0 /* RWIVF */
#define NV_PGRAPH_EXCEPTIONS_X_VALID_0                   0x00000000 /* RWI-V */
#define NV_PGRAPH_EXCEPTIONS_Y_VALID                          20:12 /* RWIVF */
#define NV_PGRAPH_EXCEPTIONS_Y_VALID_0                   0x00000000 /* RWI-V */
#define NV_PGRAPH_EXCEPTIONS_CLIP_XY                          24:24 /* RWIVF */
#define NV_PGRAPH_EXCEPTIONS_CLIP_XY_NO_ERROR            0x00000000 /* RWI-V */
#define NV_PGRAPH_EXCEPTIONS_CLIP_XY_ONLY                0x00000001 /* RW--V */
#define NV_PGRAPH_EXCEPTIONS_CLIP_WH                          28:28 /* RWIVF */
#define NV_PGRAPH_EXCEPTIONS_CLIP_WH_NO_ERROR            0x00000000 /* RWI-V */
#define NV_PGRAPH_EXCEPTIONS_CLIP_WH_ONLY                0x00000001 /* RW--V */
#define NV_PGRAPH_EDGEFILL                               0x0040065c /* RW-4R */
#define NV_PGRAPH_EDGEFILL_STAGE                                1:0 /* RWXVF */
#define NV_PGRAPH_EDGEFILL_STAGE_FIRST_STRIP             0x00000000 /* RW--V */
#define NV_PGRAPH_EDGEFILL_STAGE_MIDDLE_STRIPS           0x00000001 /* RW--V */
#define NV_PGRAPH_EDGEFILL_STAGE_LAST_STRIP              0x00000002 /* RW--V */
#define NV_PGRAPH_EDGEFILL_FIRST_MAJMIN                         4:4 /* RWXVF */
#define NV_PGRAPH_EDGEFILL_FIRST_MINMAJ                         8:8 /* RWXVF */
#define NV_PGRAPH_EDGEFILL_X16_MIN                            17:16 /* RWIVF */
#define NV_PGRAPH_EDGEFILL_X16_MIN_GT                    0x00000000 /* RW--V */
#define NV_PGRAPH_EDGEFILL_X16_MIN_LT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_EDGEFILL_X16_MIN_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_EDGEFILL_X16_MAX                            19:18 /* RWIVF */
#define NV_PGRAPH_EDGEFILL_X16_MAX_LT                    0x00000000 /* RW--V */
#define NV_PGRAPH_EDGEFILL_X16_MAX_GT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_EDGEFILL_X16_MAX_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_EDGEFILL_Y16_MIN                            21:20 /* RWIVF */
#define NV_PGRAPH_EDGEFILL_Y16_MIN_GT                    0x00000000 /* RW--V */
#define NV_PGRAPH_EDGEFILL_Y16_MIN_LT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_EDGEFILL_Y16_MIN_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_EDGEFILL_Y16_MAX                            23:22 /* RWIVF */
#define NV_PGRAPH_EDGEFILL_Y16_MAX_LT                    0x00000000 /* RW--V */
#define NV_PGRAPH_EDGEFILL_Y16_MAX_GT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_EDGEFILL_Y16_MAX_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_EDGEFILL_X17_MIN                            25:24 /* RWIVF */
#define NV_PGRAPH_EDGEFILL_X17_MIN_GT                    0x00000000 /* RW--V */
#define NV_PGRAPH_EDGEFILL_X17_MIN_LT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_EDGEFILL_X17_MIN_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_EDGEFILL_X17_MAX                            27:26 /* RWIVF */
#define NV_PGRAPH_EDGEFILL_X17_MAX_LT                    0x00000000 /* RW--V */
#define NV_PGRAPH_EDGEFILL_X17_MAX_GT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_EDGEFILL_X17_MAX_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_EDGEFILL_Y17_MIN                            29:28 /* RWIVF */
#define NV_PGRAPH_EDGEFILL_Y17_MIN_GT                    0x00000000 /* RW--V */
#define NV_PGRAPH_EDGEFILL_Y17_MIN_LT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_EDGEFILL_Y17_MIN_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_EDGEFILL_Y17_MAX                            31:30 /* RWIVF */
#define NV_PGRAPH_EDGEFILL_Y17_MAX_LT                    0x00000000 /* RW--V */
#define NV_PGRAPH_EDGEFILL_Y17_MAX_GT                    0x00000001 /* RWI-V */
#define NV_PGRAPH_EDGEFILL_Y17_MAX_EQ                    0x00000002 /* RW--V */
#define NV_PGRAPH_BETA_RAM(i)                    (0x00400700+(i)*4) /* RW-4A */
#define NV_PGRAPH_BETA_RAM__SIZE_1                               14 /*       */
#define NV_PGRAPH_BETA_RAM_VALUE                               24:0 /* RWXUF */
#define NV_PGRAPH_BETA_RAM_BPORT(i)              (0x00400d00+(i)*4) /* R--4A */
#define NV_PGRAPH_BETA_RAM_BPORT__SIZE_1                         14 /*       */
#define NV_PGRAPH_BETA_RAM_BPORT_VALUE                         24:0 /* R--UF */
#define NV_PGRAPH_ABS_ICLIP_XMAX                         0x00400450 /* RW-4R */
#define NV_PGRAPH_ABS_ICLIP_XMAX_VALUE                         17:0 /* RWXSF */
#define NV_PGRAPH_REL_ICLIP_XMAX                         0x00400550 /* -W-4R */
#define NV_PGRAPH_REL_ICLIP_XMAX_VALUE                         17:0 /* -W-SF */
#define NV_PGRAPH_ABS_ICLIP_YMAX                         0x00400454 /* RW-4R */
#define NV_PGRAPH_ABS_ICLIP_YMAX_VALUE                         17:0 /* RWXSF */
#define NV_PGRAPH_REL_ICLIP_YMAX                         0x00400554 /* -W-4R */
#define NV_PGRAPH_REL_ICLIP_YMAX_VALUE                         17:0 /* -W-SF */
#define NV_PGRAPH_BIT33                                  0x00400660 /* RW-4R */
#define NV_PGRAPH_BIT33_X_REG0                                  0:0 /* RWXVF */
#define NV_PGRAPH_BIT33_X_REG3                                  1:1 /* RWXVF */
#define NV_PGRAPH_BIT33_X_REG6                                  2:2 /* RWXVF */
#define NV_PGRAPH_BIT33_X_REG9                                  3:3 /* RWXVF */
#define NV_PGRAPH_BIT33_X_REG12                                 4:4 /* RWXVF */
#define NV_PGRAPH_BIT33_X_REG15                                 5:5 /* RWXVF */
#define NV_PGRAPH_BIT33_X_REG16                                 6:6 /* RWXVF */
#define NV_PGRAPH_BIT33_X_REG17                                 7:7 /* RWXVF */
#define NV_PGRAPH_BIT33_Y_REG0                                16:16 /* RWXVF */
#define NV_PGRAPH_BIT33_Y_REG3                                17:17 /* RWXVF */
#define NV_PGRAPH_BIT33_Y_REG6                                18:18 /* RWXVF */
#define NV_PGRAPH_BIT33_Y_REG9                                19:19 /* RWXVF */
#define NV_PGRAPH_BIT33_Y_REG12                               20:20 /* RWXVF */
#define NV_PGRAPH_BIT33_Y_REG15                               21:21 /* RWXVF */
#define NV_PGRAPH_BIT33_Y_REG16                               22:22 /* RWXVF */
#define NV_PGRAPH_BIT33_Y_REG17                               23:23 /* RWXVF */
#define NV_PGRAPH_DEBUG_0                                0x00400080 /* RW-4R */
#define NV_PGRAPH_DEBUG_1                                0x00400084 /* RW-4R */
#define NV_PGRAPH_DEBUG_2                                0x00400088 /* RW-4R */
#define NV_PGRAPH_DEBUG_3                                0x0040008c /* RW-4R */
#define NV_PGRAPH_INTR_0                                 0x00400100 /* RW-4R */
#define NV_PGRAPH_INTR_1                                 0x00400104 /* RW-4R */
#define NV_PGRAPH_INTR_EN_0                              0x00400140 /* RW-4R */
#define NV_PGRAPH_INTR_EN_1                              0x00400144 /* RW-4R */
#define NV_PGRAPH_CTX_SWITCH                             0x00400180 /* RW-4R */
#define NV_PGRAPH_CTX_CONTROL                            0x00400190 /* RW-4R */
#define NV_PGRAPH_MISC                                   0x004006A4 /* RW-4R */
#define NV_PGRAPH_STATUS                                 0x004006B0 /* R--4R */
#define NV_PGRAPH_TRAPPED_ADDR                           0x004006A8 /* R--4R */
#define NV_PGRAPH_TRAPPED_DATA                           0x004006AC /* R--4R */
#define NV_PGRAPH_CANVAS_MISC                            0x00400634 /* RW-4R */
#define NV_PGRAPH_CLIP_MISC                              0x004006A0 /* RW-4R */
#define NV_PGRAPH_CANVAS_MIN                             0x00400688 /* RW-4R */
#define NV_PGRAPH_CANVAS_MAX                             0x0040068C /* RW-4R */
#define NV_PGRAPH_CLIP0_MIN                              0x00400690 /* RW-4R */
#define NV_PGRAPH_CLIP1_MIN                              0x00400698 /* RW-4R */
#define NV_PGRAPH_CLIP0_MAX                              0x00400694 /* RW-4R */
#define NV_PGRAPH_CLIP1_MAX                              0x0040069C /* RW-4R */
#define NV_PGRAPH_DMA                                    0x00400680 /* RW-4R */
#define NV_PGRAPH_NOTIFY                                 0x00400684 /* RW-4R */
#define NV_PGRAPH_PATT_COLOR0_0                          0x00400600 /* RW-4R */
#define NV_PGRAPH_PATT_COLOR0_1                          0x00400604 /* RW-4R */
#define NV_PGRAPH_PATT_COLOR1_0                          0x00400608 /* RW-4R */
#define NV_PGRAPH_PATT_COLOR1_1                          0x0040060C /* RW-4R */
#define NV_PGRAPH_PATTERN_SHAPE                          0x00400618 /* RW-4R */
#define NV_PGRAPH_MONO_COLOR0                            0x0040061C /* RW-4R */
#define NV_PGRAPH_MONO_COLOR1                            0x00400620 /* RW-4R */
#define NV_PGRAPH_ROP3                                   0x00400624 /* RW-4R */
#define NV_PGRAPH_PLANE_MASK                             0x00400628 /* RW-4R */
#define NV_PGRAPH_CHROMA                                 0x0040062C /* RW-4R */
#define NV_PGRAPH_BETA                                   0x00400630 /* RW-4R */
#define NV_PGRAPH_XY_LOGIC_MISC0                         0x00400640 /* RW-4R */
#define NV_PGRAPH_XY_LOGIC_MISC1                         0x00400644 /* RW-4R */
#define NV_PGRAPH_X_MISC                                 0x00400648 /* RW-4R */
#define NV_PGRAPH_Y_MISC                                 0x0040064c /* RW-4R */
#define NV_PGRAPH_ABS_UCLIP_XMIN                         0x00400460 /* RW-4R */
#define NV_PGRAPH_REL_UCLIP_XMIN                         0x00400560 /* -W-4R */
#define NV_PGRAPH_ABS_UCLIP_XMAX                         0x00400464 /* RW-4R */
#define NV_PGRAPH_REL_UCLIP_XMAX                         0x00400564 /* -W-4R */
#define NV_PGRAPH_ABS_UCLIP_YMIN                         0x00400468 /* RW-4R */
#define NV_PGRAPH_REL_UCLIP_YMIN                         0x00400568 /* -W-4R */
#define NV_PGRAPH_ABS_UCLIP_YMAX                         0x0040046C /* RW-4R */
#define NV_PGRAPH_REL_UCLIP_YMAX                         0x0040056C /* -W-4R */
#define NV_PGRAPH_SOURCE_COLOR                           0x00400654 /* RW-4R */
#define NV_PGRAPH_SUBDIVIDE                              0x00400658 /* RW-4R */
#define NV_PGRAPH_EXCEPTIONS                             0x00400650 /* RW-4R */
#define NV_PGRAPH_EDGEFILL                               0x0040065c /* RW-4R */
#define NV_PGRAPH_ABS_ICLIP_XMAX                         0x00400450 /* RW-4R */
#define NV_PGRAPH_REL_ICLIP_XMAX                         0x00400550 /* -W-4R */
#define NV_PGRAPH_ABS_ICLIP_YMAX                         0x00400454 /* RW-4R */
#define NV_PGRAPH_REL_ICLIP_YMAX                         0x00400554 /* -W-4R */
#define NV_PGRAPH_BIT33                                  0x00400660 /* RW-4R */
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
#define NV_URECT                              0x004C1FFF:0x004C0000 /* -W--D */
#define NV_URECT_CTX_SWITCH                              0x004C0000 /* -W-4R */
#define NV_URECT_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
#define NV_URECT_CTX_SWITCH_CHID                              22:16 /* -W-UF */
#define NV_URECT_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
#define NV_URECT_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
#define NV_URECT_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
#define NV_URECT_SET_NOTIFY                              0x004C0104 /* -W-4R */
#define NV_URECT_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
#define NV_URECT_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
#define NV_URECT_COLOR                                   0x004C0304 /* -W-4R */
#define NV_URECT_COLOR_VALUE                                   31:0 /* -W-VF */
#define NV_URECT_RECTANGLE_0(i)                  (0x004C0400+(i)*8) /* -W-4A */
#define NV_URECT_RECTANGLE_0__SIZE_1                             16 /*       */
#define NV_URECT_RECTANGLE_0_X                                 15:0 /* -W-SF */
#define NV_URECT_RECTANGLE_0_Y                                31:16 /* -W-SF */
#define NV_URECT_RECTANGLE_1(i)                  (0x004C0404+(i)*8) /* -W-4A */
#define NV_URECT_RECTANGLE_1__SIZE_1                             16 /*       */
#define NV_URECT_RECTANGLE_1_WIDTH                             15:0 /* -W-UF */
#define NV_URECT_RECTANGLE_1_HEIGHT                           31:16 /* -W-UF */
/* usr_bilinear.ref */
#define NV_UBTM                               0x004D1FFF:0x004D0000 /* -W--D */
#define NV_UBTM_CTX_SWITCH                               0x004D0000 /* -W-4R */
#define NV_UBTM_CTX_SWITCH_INSTANCE                            15:0 /* -W-UF */
#define NV_UBTM_CTX_SWITCH_CHID                               22:16 /* -W-UF */
#define NV_UBTM_CTX_SWITCH_VOLATILE                           31:31 /* -W-VF */
#define NV_UBTM_CTX_SWITCH_VOLATILE_IGNORE               0x00000000 /* -W--V */
#define NV_UBTM_CTX_SWITCH_VOLATILE_RESET                0x00000001 /* -W--V */
#define NV_UBTM_SET_NOTIFY                               0x004D0104 /* -W-4R */
#define NV_UBTM_SET_NOTIFY_PARAMETER                           31:0 /* -W-VF */
#define NV_UBTM_SET_NOTIFY_PARAMETER_WRITE               0x00000000 /* -W--V */
#define NV_UBTM_SUBDIVIDE_IN                             0x004D0304 /* -W-4R */
#define NV_UBTM_SUBDIVIDE_IN_MAJOR_0123                         3:0 /* -W-VF */
#define NV_UBTM_SUBDIVIDE_IN_MINOR_0213                         7:4 /* -W-VF */
#define NV_UBTM_SUBDIVIDE_IN_EDGE_01                          19:16 /* -W-VF */
#define NV_UBTM_SUBDIVIDE_IN_EDGE_23                          23:20 /* -W-VF */
#define NV_UBTM_SUBDIVIDE_IN_EDGE_02                          27:24 /* -W-VF */
#define NV_UBTM_SUBDIVIDE_IN_EDGE_13                          31:28 /* -W-VF */
#define NV_UBTM_SUBDIVIDE_IN_BY_1                        0x00000000 /* -W--V */
#define NV_UBTM_SUBDIVIDE_IN_BY_2                        0x00000001 /* -W--V */
#define NV_UBTM_SUBDIVIDE_IN_BY_4                        0x00000002 /* -W--V */
#define NV_UBTM_SUBDIVIDE_IN_BY_8                        0x00000003 /* -W--V */
#define NV_UBTM_SUBDIVIDE_IN_BY_16                       0x00000004 /* -W--V */
#define NV_UBTM_SUBDIVIDE_IN_BY_32                       0x00000005 /* -W--V */
#define NV_UBTM_SUBDIVIDE_IN_BY_64                       0x00000006 /* -W--V */
#define NV_UBTM_SUBDIVIDE_IN_BY_128                      0x00000007 /* -W--V */
#define NV_UBTM_SUBDIVIDE_IN_BY_256                      0x00000008 /* -W--V */
#define NV_UBTM_POINT_OUT(i)                     (0x004D0310+(i)*4) /* -W-4A */
#define NV_UBTM_POINT_OUT__SIZE_1                                 4 /*       */
#define NV_UBTM_POINT_OUT_X                                    15:0 /* -W-SF */
#define NV_UBTM_POINT_OUT_Y                                   31:16 /* -W-SF */
#define NV_UBTM_POINT_OUT12D4(i)                 (0x004D0350+(i)*4) /* -W-4A */
#define NV_UBTM_POINT_OUT12D4__SIZE_1                             4 /*       */
#define NV_UBTM_POINT_OUT12D4_X_FRACTION                        3:0 /* -W-UF */
#define NV_UBTM_POINT_OUT12D4_X                                15:4 /* -W-SF */
#define NV_UBTM_POINT_OUT12D4_Y_FRACTION                      19:16 /* -W-UF */
#define NV_UBTM_POINT_OUT12D4_Y                               31:20 /* -W-SF */
#define NV_UBTM_COLOR(i)                         (0x004D0400+(i)*4) /* -W-4A */
#define NV_UBTM_COLOR__SIZE_1                                    32 /*       */
#define NV_UBTM_COLOR_VALUE                                    31:0 /* -W-VF */
/* usr_quadratic.ref */
#define NV_UQTM                               0x004E1FFF:0x004E0000 /* -W--D */
#define NV_UQTM_CTX_SWITCH                               0x004E0000 /* -W-4R */
#define NV_UQTM_CTX_SWITCH_INSTANCE                            15:0 /* -W-UF */
#define NV_UQTM_CTX_SWITCH_CHID                               22:16 /* -W-UF */
#define NV_UQTM_CTX_SWITCH_VOLATILE                           31:31 /* -W-VF */
#define NV_UQTM_CTX_SWITCH_VOLATILE_IGNORE               0x00000000 /* -W--V */
#define NV_UQTM_CTX_SWITCH_VOLATILE_RESET                0x00000001 /* -W--V */
#define NV_UQTM_SET_NOTIFY                               0x004E0104 /* -W-4R */
#define NV_UQTM_SET_NOTIFY_PARAMETER                           31:0 /* -W-VF */
#define NV_UQTM_SET_NOTIFY_PARAMETER_WRITE               0x00000000 /* -W--V */
#define NV_UQTM_SUBDIVIDE_IN                             0x004E0304 /* -W-4R */
#define NV_UQTM_SUBDIVIDE_IN_MAJOR_0268                         3:0 /* -W-VF */
#define NV_UQTM_SUBDIVIDE_IN_MINOR_0628                         7:4 /* -W-VF */
#define NV_UQTM_SUBDIVIDE_IN_EDGE_02                          19:16 /* -W-VF */
#define NV_UQTM_SUBDIVIDE_IN_EDGE_68                          23:20 /* -W-VF */
#define NV_UQTM_SUBDIVIDE_IN_EDGE_06                          27:24 /* -W-VF */
#define NV_UQTM_SUBDIVIDE_IN_EDGE_28                          31:28 /* -W-VF */
#define NV_UQTM_SUBDIVIDE_IN_BY_1                        0x00000000 /* -W--V */
#define NV_UQTM_SUBDIVIDE_IN_BY_2                        0x00000001 /* -W--V */
#define NV_UQTM_SUBDIVIDE_IN_BY_4                        0x00000002 /* -W--V */
#define NV_UQTM_SUBDIVIDE_IN_BY_8                        0x00000003 /* -W--V */
#define NV_UQTM_SUBDIVIDE_IN_BY_16                       0x00000004 /* -W--V */
#define NV_UQTM_SUBDIVIDE_IN_BY_32                       0x00000005 /* -W--V */
#define NV_UQTM_SUBDIVIDE_IN_BY_64                       0x00000006 /* -W--V */
#define NV_UQTM_SUBDIVIDE_IN_BY_128                      0x00000007 /* -W--V */
#define NV_UQTM_SUBDIVIDE_IN_BY_256                      0x00000008 /* -W--V */
#define NV_UQTM_POINT_OUT(i)                     (0x004E0310+(i)*4) /* -W-4A */
#define NV_UQTM_POINT_OUT__SIZE_1                                 9 /*       */
#define NV_UQTM_POINT_OUT_X                                    15:0 /* -W-SF */
#define NV_UQTM_POINT_OUT_Y                                   31:16 /* -W-SF */
#define NV_UQTM_POINT_OUT12D4(i)                 (0x004E0350+(i)*4) /* -W-4A */
#define NV_UQTM_POINT_OUT12D4__SIZE_1                             9 /*       */
#define NV_UQTM_POINT_OUT12D4_X_FRACTION                        3:0 /* -W-UF */
#define NV_UQTM_POINT_OUT12D4_X                                15:4 /* -W-SF */
#define NV_UQTM_POINT_OUT12D4_Y_FRACTION                      19:16 /* -W-UF */
#define NV_UQTM_POINT_OUT12D4_Y                               31:20 /* -W-SF */
#define NV_UQTM_COLOR(i)                         (0x004E0400+(i)*4) /* -W-4A */
#define NV_UQTM_COLOR__SIZE_1                                    32 /*       */
#define NV_UQTM_COLOR_VALUE                                    31:0 /* -W-VF */
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
/* usr_image_from_mem.ref */
#define NV_UFROMEM                            0x00531FFF:0x00530000 /* -W--D */
#define NV_UFROMEM_CTX_SWITCH                            0x00530000 /* -W-4R */
#define NV_UFROMEM_CTX_SWITCH_INSTANCE                         15:0 /* -W-UF */
#define NV_UFROMEM_CTX_SWITCH_CHID                            22:16 /* -W-UF */
#define NV_UFROMEM_CTX_SWITCH_VOLATILE                        31:31 /* -W-VF */
#define NV_UFROMEM_CTX_SWITCH_VOLATILE_IGNORE            0x00000000 /* -W--V */
#define NV_UFROMEM_CTX_SWITCH_VOLATILE_RESET             0x00000001 /* -W--V */
#define NV_UFROMEM_DMA_REPLY(i)                  (0x00530040+(i)*4) /* -W-4A */
#define NV_UFROMEM_DMA_REPLY__SIZE_1                             16 /*       */
#define NV_UFROMEM_DMA_REPLY_DATA                              31:0 /* -W-VF */
#define NV_UFROMEM_SET_NOTIFY                            0x00530104 /* -W-4R */
#define NV_UFROMEM_SET_NOTIFY_PARAMETER                        31:0 /* -W-VF */
#define NV_UFROMEM_SET_NOTIFY_PARAMETER_WRITE            0x00000000 /* -W--V */
#define NV_UFROMEM_POINT                                 0x00530308 /* -W-4R */
#define NV_UFROMEM_POINT_X                                     15:0 /* -W-SF */
#define NV_UFROMEM_POINT_Y                                    31:16 /* -W-SF */
#define NV_UFROMEM_SIZE                                  0x0053030C /* -W-4R */
#define NV_UFROMEM_SIZE_WIDTH                                  15:0 /* -W-UF */
#define NV_UFROMEM_SIZE_HEIGHT                                31:16 /* -W-UF */
#define NV_UFROMEM_PITCH                                 0x00530310 /* -W-4R */
#define NV_UFROMEM_PITCH_VALUE                                 31:0 /* -W-SF */
#define NV_UFROMEM_IMAGE_START                           0x00530314 /* -W-4R */
#define NV_UFROMEM_PITCH_START_OFFSET                          31:0 /* -W-UF */
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
/* usr_bilinear_beta.ref */
#define NV_UBTMB                              0x005D1FFF:0x005D0000 /* -W--D */
#define NV_UBTMB_CTX_SWITCH                              0x005D0000 /* -W-4R */
#define NV_UBTMB_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
#define NV_UBTMB_CTX_SWITCH_CHID                              22:16 /* -W-UF */
#define NV_UBTMB_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
#define NV_UBTMB_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
#define NV_UBTMB_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
#define NV_UBTMB_SET_NOTIFY                              0x005D0104 /* -W-4R */
#define NV_UBTMB_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
#define NV_UBTMB_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
#define NV_UBTMB_SUBDIVIDE_IN                            0x005D0304 /* -W-4R */
#define NV_UBTMB_SUBDIVIDE_IN_MAJOR_0123                        3:0 /* -W-VF */
#define NV_UBTMB_SUBDIVIDE_IN_MINOR_0213                        7:4 /* -W-VF */
#define NV_UBTMB_SUBDIVIDE_IN_EDGE_01                         19:16 /* -W-VF */
#define NV_UBTMB_SUBDIVIDE_IN_EDGE_23                         23:20 /* -W-VF */
#define NV_UBTMB_SUBDIVIDE_IN_EDGE_02                         27:24 /* -W-VF */
#define NV_UBTMB_SUBDIVIDE_IN_EDGE_13                         31:28 /* -W-VF */
#define NV_UBTMB_SUBDIVIDE_IN_BY_1                       0x00000000 /* -W--V */
#define NV_UBTMB_SUBDIVIDE_IN_BY_2                       0x00000001 /* -W--V */
#define NV_UBTMB_SUBDIVIDE_IN_BY_4                       0x00000002 /* -W--V */
#define NV_UBTMB_SUBDIVIDE_IN_BY_8                       0x00000003 /* -W--V */
#define NV_UBTMB_SUBDIVIDE_IN_BY_16                      0x00000004 /* -W--V */
#define NV_UBTMB_SUBDIVIDE_IN_BY_32                      0x00000005 /* -W--V */
#define NV_UBTMB_SUBDIVIDE_IN_BY_64                      0x00000006 /* -W--V */
#define NV_UBTMB_SUBDIVIDE_IN_BY_128                     0x00000007 /* -W--V */
#define NV_UBTMB_SUBDIVIDE_IN_BY_256                     0x00000008 /* -W--V */
#define NV_UBTMB_POINT_OUT(i)                    (0x005D0310+(i)*4) /* -W-4A */
#define NV_UBTMB_POINT_OUT__SIZE_1                                4 /*       */
#define NV_UBTMB_POINT_OUT_X                                   15:0 /* -W-SF */
#define NV_UBTMB_POINT_OUT_Y                                  31:16 /* -W-SF */
#define NV_UBTMB_POINT_OUT12D4(i)                (0x005D0350+(i)*4) /* -W-4A */
#define NV_UBTMB_POINT_OUT12D4__SIZE_1                            4 /*       */
#define NV_UBTMB_POINT_OUT12D4_X_FRACTION                       3:0 /* -W-UF */
#define NV_UBTMB_POINT_OUT12D4_X                               15:4 /* -W-SF */
#define NV_UBTMB_POINT_OUT12D4_Y_FRACTION                     19:16 /* -W-UF */
#define NV_UBTMB_POINT_OUT12D4_Y                              31:20 /* -W-SF */
#define NV_UBTMB_BETA(i)                         (0x005D0380+(i)*4) /* -W-4A */
#define NV_UBTMB_BETA__SIZE_1                                     2 /*       */
#define NV_UBTMB_BETA_VALUE_0_FRACTION                         14:0 /* -W-UF */
#define NV_UBTMB_BETA_VALUE_0                                 15:15 /* -W-SF */
#define NV_UBTMB_BETA_VALUE_1_FRACTION                        30:16 /* -W-UF */
#define NV_UBTMB_BETA_VALUE_1                                 31:31 /* -W-SF */
#define NV_UBTMB_COLOR(i)                        (0x005D0400+(i)*4) /* -W-4A */
#define NV_UBTMB_COLOR__SIZE_1                                   32 /*       */
#define NV_UBTMB_COLOR_VALUE                                   31:0 /* -W-VF */
/* usr_quadratic_beta.ref */
#define NV_UQTMB                              0x005E1FFF:0x005E0000 /* -W--D */
#define NV_UQTMB_CTX_SWITCH                              0x005E0000 /* -W-4R */
#define NV_UQTMB_CTX_SWITCH_INSTANCE                           15:0 /* -W-UF */
#define NV_UQTMB_CTX_SWITCH_CHID                              22:16 /* -W-UF */
#define NV_UQTMB_CTX_SWITCH_VOLATILE                          31:31 /* -W-VF */
#define NV_UQTMB_CTX_SWITCH_VOLATILE_IGNORE              0x00000000 /* -W--V */
#define NV_UQTMB_CTX_SWITCH_VOLATILE_RESET               0x00000001 /* -W--V */
#define NV_UQTMB_SET_NOTIFY                              0x005E0104 /* -W-4R */
#define NV_UQTMB_SET_NOTIFY_PARAMETER                          31:0 /* -W-VF */
#define NV_UQTMB_SET_NOTIFY_PARAMETER_WRITE              0x00000000 /* -W--V */
#define NV_UQTMB_SUBDIVIDE_IN                            0x005E0304 /* -W-4R */
#define NV_UQTMB_SUBDIVIDE_IN_MAJOR_0268                        3:0 /* -W-VF */
#define NV_UQTMB_SUBDIVIDE_IN_MINOR_0628                        7:4 /* -W-VF */
#define NV_UQTMB_SUBDIVIDE_IN_EDGE_02                         19:16 /* -W-VF */
#define NV_UQTMB_SUBDIVIDE_IN_EDGE_68                         23:20 /* -W-VF */
#define NV_UQTMB_SUBDIVIDE_IN_EDGE_06                         27:24 /* -W-VF */
#define NV_UQTMB_SUBDIVIDE_IN_EDGE_28                         31:28 /* -W-VF */
#define NV_UQTMB_SUBDIVIDE_IN_BY_1                       0x00000000 /* -W--V */
#define NV_UQTMB_SUBDIVIDE_IN_BY_2                       0x00000001 /* -W--V */
#define NV_UQTMB_SUBDIVIDE_IN_BY_4                       0x00000002 /* -W--V */
#define NV_UQTMB_SUBDIVIDE_IN_BY_8                       0x00000003 /* -W--V */
#define NV_UQTMB_SUBDIVIDE_IN_BY_16                      0x00000004 /* -W--V */
#define NV_UQTMB_SUBDIVIDE_IN_BY_32                      0x00000005 /* -W--V */
#define NV_UQTMB_SUBDIVIDE_IN_BY_64                      0x00000006 /* -W--V */
#define NV_UQTMB_SUBDIVIDE_IN_BY_128                     0x00000007 /* -W--V */
#define NV_UQTMB_SUBDIVIDE_IN_BY_256                     0x00000008 /* -W--V */
#define NV_UQTMB_POINT_OUT(i)                    (0x005E0310+(i)*4) /* -W-4A */
#define NV_UQTMB_POINT_OUT__SIZE_1                                9 /*       */
#define NV_UQTMB_POINT_OUT_X                                   15:0 /* -W-SF */
#define NV_UQTMB_POINT_OUT_Y                                  31:16 /* -W-SF */
#define NV_UQTMB_POINT_OUT12D4(i)                (0x005E0350+(i)*4) /* -W-4A */
#define NV_UQTMB_POINT_OUT12D4__SIZE_1                            9 /*       */
#define NV_UQTMB_POINT_OUT12D4_X_FRACTION                       3:0 /* -W-UF */
#define NV_UQTMB_POINT_OUT12D4_X                               15:4 /* -W-SF */
#define NV_UQTMB_POINT_OUT12D4_Y_FRACTION                     19:16 /* -W-UF */
#define NV_UQTMB_POINT_OUT12D4_Y                              31:20 /* -W-SF */
#define NV_UQTMB_BETA(i)                         (0x005E0380+(i)*4) /* -W-4A */
#define NV_UQTMB_BETA__SIZE_1                                     5 /*       */
#define NV_UQTMB_BETA_VALUE_0_FRACTION                         14:0 /* -W-UF */
#define NV_UQTMB_BETA_VALUE_0                                 15:15 /* -W-SF */
#define NV_UQTMB_BETA_VALUE_1_FRACTION                        30:16 /* -W-UF */
#define NV_UQTMB_BETA_VALUE_1                                 31:31 /* -W-SF */
#define NV_UQTMB_COLOR(i)                        (0x005E0400+(i)*4) /* -W-4A */
#define NV_UQTMB_COLOR__SIZE_1                                   32 /*       */
#define NV_UQTMB_COLOR_VALUE                                   31:0 /* -W-VF */
/* dev_framebuffer.ref */
#define NV_PFB                                0x00600FFF:0x00600000 /* RW--D */
#define NV_PFB_BOOT_0                                    0x00600000 /* RW-4R */
#define NV_PFB_BOOT_0_RAM_AMOUNT                                1:0 /* RWIVF */
#define NV_PFB_BOOT_0_RAM_AMOUNT_1MB                     0x00000000 /* RW--V */
#define NV_PFB_BOOT_0_RAM_AMOUNT_2MB                     0x00000001 /* RW--V */
#define NV_PFB_BOOT_0_RAM_AMOUNT_4MB                     0x00000002 /* RWI-V */
#define NV_PFB_BOOT_0_DAC_WIDTH                                 9:8 /* RWXVF */
#define NV_PFB_BOOT_0_DAC_WIDTH_16_BIT                   0x00000000 /* RW--V */
#define NV_PFB_BOOT_0_DAC_WIDTH_32_BIT                   0x00000001 /* RW--V */
#define NV_PFB_BOOT_0_DAC_WIDTH_64_BIT                   0x00000002 /* RW--V */
#define NV_PFB_BOOT_0_RAS_PRECHARGE                           12:12 /* RWIVF */
#define NV_PFB_BOOT_0_RAS_PRECHARGE_3_CYCLES             0x00000000 /* RWI-V */
#define NV_PFB_BOOT_0_RAS_PRECHARGE_2_CYCLES             0x00000001 /* RW--V */
#define NV_PFB_BOOT_0_RAM_BLOCK_MODE                          17:16 /* RWIVF */
#define NV_PFB_BOOT_0_RAM_BLOCK_MODE_DISABLED            0x00000000 /* RWI-V */
#define NV_PFB_BOOT_0_RAM_BLOCK_MODE_BY_8                0x00000001 /* RW--V */
#define NV_PFB_BOOT_0_RAM_BLOCK_MODE_BY_4                0x00000003 /* RW--V */
#define NV_PFB_BOOT_0_RAM_WRITE_PER_BIT                       20:20 /* RWIVF */
#define NV_PFB_BOOT_0_RAM_WRITE_PER_BIT_DISABLED         0x00000000 /* RWI-V */
#define NV_PFB_BOOT_0_RAM_WRITE_PER_BIT_ENABLED          0x00000001 /* RW--V */
#define NV_PFB_BOOT_0_VRAM_SAM                                24:24 /* RWIVF */
#define NV_PFB_BOOT_0_VRAM_SAM_8192_BIT                  0x00000000 /* RWI-V */
#define NV_PFB_BOOT_0_VRAM_SAM_4096_BIT                  0x00000001 /* RW--V */
#define NV_PFB_BOOT_0_RAM_HYPER_PAGE                          28:28 /* RWIVF */
#define NV_PFB_BOOT_0_RAM_HYPER_PAGE_DISABLED            0x00000000 /* RWI-V */
#define NV_PFB_BOOT_0_RAM_HYPER_PAGE_ENABLED             0x00000001 /* RW--V */
#define NV_PFB_DELAY_0                                   0x00600040 /* RW-4R */
#define NV_PFB_DELAY_0_SYNC                                     4:0 /* RWIUF */
#define NV_PFB_DELAY_0_SYNC_0                            0x00000000 /* RWI-V */
#define NV_PFB_DELAY_0_DSF                                      9:8 /* RWIUF */
#define NV_PFB_DELAY_0_DSF_0                             0x00000000 /* RWI-V */
#define NV_PFB_DELAY_0_SOE_RISE                               13:12 /* RWIUF */
#define NV_PFB_DELAY_0_SOE_RISE_0                        0x00000000 /* RWI-V */
#define NV_PFB_DELAY_0_SOE_FALL                               17:16 /* RWIUF */
#define NV_PFB_DELAY_0_SOE_FALL_0                        0x00000000 /* RWI-V */
#define NV_PFB_DELAY_0_LOAD_CLK                               25:20 /* RWIUF */
#define NV_PFB_DELAY_0_LOAD_CLK_0                        0x00000000 /* RWI-V */
#define NV_PFB_DELAY_1                                   0x00600044 /* RW-4R */
#define NV_PFB_DELAY_1_WRITE_ENABLE_RISE                        1:0 /* RWIUF */
#define NV_PFB_DELAY_1_WRITE_ENABLE_RISE_0               0x00000000 /* RWI-V */
#define NV_PFB_DELAY_1_WRITE_ENABLE_FALL                        5:4 /* RWIUF */
#define NV_PFB_DELAY_1_WRITE_ENABLE_FALL_0               0x00000000 /* RWI-V */
#define NV_PFB_DELAY_1_OUTPUT_ENABLE                            9:8 /* RWIUF */
#define NV_PFB_DELAY_1_OUTPUT_ENABLE_0                   0x00000000 /* RWI-V */
#define NV_PFB_DELAY_1_OUTPUT_DATA                            17:16 /* RWIUF */
#define NV_PFB_DELAY_1_OUTPUT_DATA_0                     0x00000000 /* RWI-V */
#define NV_PFB_DEBUG_0                                   0x00600080 /* RW-4R */
#define NV_PFB_DEBUG_0_PAGE_MODE                                0:0 /* RWIVF */
#define NV_PFB_DEBUG_0_PAGE_MODE_ENABLED                 0x00000000 /* RWI-V */
#define NV_PFB_DEBUG_0_PAGE_MODE_DISABLED                0x00000001 /* RW--V */
#define NV_PFB_DEBUG_0_REFRESH                                  4:4 /* RWIVF */
#define NV_PFB_DEBUG_0_REFRESH_ENABLED                   0x00000000 /* RWI-V */
#define NV_PFB_DEBUG_0_REFRESH_DISABLED                  0x00000001 /* RW--V */
#define NV_PFB_DEBUG_0_SELFTIME                                 8:8 /* RWIVF */
#define NV_PFB_DEBUG_0_SELFTIME_ENABLED                  0x00000000 /* RWI-V */
#define NV_PFB_DEBUG_0_SELFTIME_DISABLED                 0x00000001 /* RW--V */
#define NV_PFB_DEBUG_0_TRANSFER                               12:12 /* RWIVF */
#define NV_PFB_DEBUG_0_TRANSFER_ENABLED                  0x00000000 /* RWI-V */
#define NV_PFB_DEBUG_0_TRANSFER_DISABLED                 0x00000001 /* RW--V */
#define NV_PFB_DEBUG_0_LOAD_CLK_FIX_DELAY                     16:16 /* RWIVF */
#define NV_PFB_DEBUG_0_LOAD_CLK_FIX_DELAY_DISABLED       0x00000000 /* RWI-V */
#define NV_PFB_DEBUG_0_LOAD_CLK_FIX_DELAY_ENABLED        0x00000001 /* RW--V */
#define NV_PFB_DEBUG_0_CASOE                                  20:20 /* RWIVF */
#define NV_PFB_DEBUG_0_CASOE_ENABLED                     0x00000000 /* RWI-V */
#define NV_PFB_DEBUG_0_CASOE_DISABLED                    0x00000001 /* RW--V */
#define NV_PFB_DEBUG_0_FIFO_DRAIN_HINT                        31:24 /* RWIUF */
#define NV_PFB_DEBUG_0_FIFO_DRAIN_HINT_0                 0x00000000 /* RWI-V */
#define NV_PFB_GREEN_0                                   0x006000C0 /* RW-4R */
#define NV_PFB_GREEN_0_LEVEL                                    1:0 /* RWIVF */
#define NV_PFB_GREEN_0_LEVEL_VIDEO_ENABLED               0x00000000 /* RW--V */
#define NV_PFB_GREEN_0_LEVEL_VIDEO_DISABLED              0x00000001 /* RW--V */
#define NV_PFB_GREEN_0_LEVEL_TIMING_DISABLED             0x00000002 /* RW--V */
#define NV_PFB_GREEN_0_LEVEL_MEMORY_DISABLED             0x00000003 /* RWI-V */
#define NV_PFB_GREEN_0_POLAR_HSYNC                            17:16 /* RWIVF */
#define NV_PFB_GREEN_0_POLAR_HSYNC_HIGH                  0x00000000 /* RWI-V */
#define NV_PFB_GREEN_0_POLAR_HSYNC_LOW                   0x00000001 /* RW--V */
#define NV_PFB_GREEN_0_POLAR_HSYNC_POSITIVE              0x00000002 /* RW--V */
#define NV_PFB_GREEN_0_POLAR_HSYNC_NEGATIVE              0x00000003 /* RW--V */
#define NV_PFB_GREEN_0_POLAR_VSYNC                            21:20 /* RWIVF */
#define NV_PFB_GREEN_0_POLAR_VSYNC_LOW                   0x00000000 /* RWI-V */
#define NV_PFB_GREEN_0_POLAR_VSYNC_HIGH                  0x00000001 /* RW--V */
#define NV_PFB_GREEN_0_POLAR_VSYNC_POSITIVE              0x00000002 /* RW--V */
#define NV_PFB_GREEN_0_POLAR_VSYNC_NEGATIVE              0x00000003 /* RW--V */
#define NV_PFB_GREEN_0_CSYNC                                  24:24 /* RWIVF */
#define NV_PFB_GREEN_0_CSYNC_DISABLED                    0x00000000 /* RWI-V */
#define NV_PFB_GREEN_0_CSYNC_ENABLED                     0x00000001 /* RW--V */
#define NV_PFB_CONFIG_0                                  0x00600200 /* RW-4R */
#define NV_PFB_CONFIG_0_VERTICAL                                0:0 /* R-XVF */
#define NV_PFB_CONFIG_0_VERTICAL_DISPLAY                 0x00000000 /* R---V */
#define NV_PFB_CONFIG_0_VERTICAL_BLANK                   0x00000001 /* R---V */
#define NV_PFB_CONFIG_0_RESOLUTION                              6:4 /* RWIVF */
#define NV_PFB_CONFIG_0_RESOLUTION_576_PIXELS            0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_RESOLUTION_640_PIXELS            0x00000001 /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_800_PIXELS            0x00000002 /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_1024_PIXELS           0x00000003 /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_1152_PIXELS           0x00000004 /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_1280_PIXELS           0x00000005 /* RW--V */
#define NV_PFB_CONFIG_0_RESOLUTION_1600_PIXELS           0x00000006 /* RW--V */
#define NV_PFB_CONFIG_0_PIXEL_DEPTH                             9:8 /* RWIVF */
#define NV_PFB_CONFIG_0_PIXEL_DEPTH_4_BITS               0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_PIXEL_DEPTH_8_BITS               0x00000001 /* RW--V */
#define NV_PFB_CONFIG_0_PIXEL_DEPTH_16_BITS              0x00000002 /* RW--V */
#define NV_PFB_CONFIG_0_PIXEL_DEPTH_32_BITS              0x00000003 /* RW--V */
#define NV_PFB_CONFIG_0_SECOND_BUFFER                         12:12 /* RWIVF */
#define NV_PFB_CONFIG_0_SECOND_BUFFER_DISABLED           0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_SECOND_BUFFER_ENABLED            0x00000001 /* RW--V */
#define NV_PFB_CONFIG_0_PAGE_HEIGHT                           18:16 /* RWIVF */
#define NV_PFB_CONFIG_0_PAGE_HEIGHT_DISABLED             0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_PAGE_HEIGHT_4_LINES              0x00000001 /* RW--V */
#define NV_PFB_CONFIG_0_PAGE_HEIGHT_8_LINES              0x00000002 /* RW--V */
#define NV_PFB_CONFIG_0_PAGE_HEIGHT_16_LINES             0x00000003 /* RW--V */
#define NV_PFB_CONFIG_0_PAGE_HEIGHT_32_LINES             0x00000004 /* RW--V */
#define NV_PFB_CONFIG_0_SCANLINE                              20:20 /* RWIVF */
#define NV_PFB_CONFIG_0_SCANLINE_NO_DUPLICATE            0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_SCANLINE_DUPLICATE               0x00000001 /* RW--V */
#define NV_PFB_CONFIG_0_PCLK_VCLK_RATIO                       26:24 /* RWIVF */
#define NV_PFB_CONFIG_0_PCLK_VCLK_RATIO_1_TO_1           0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_PCLK_VCLK_RATIO_2_TO_1           0x00000001 /* RW--V */
#define NV_PFB_CONFIG_0_PCLK_VCLK_RATIO_4_TO_1           0x00000002 /* RW--V */
#define NV_PFB_CONFIG_0_PCLK_VCLK_RATIO_8_TO_1           0x00000003 /* RW--V */
#define NV_PFB_CONFIG_0_PCLK_VCLK_RATIO_16_TO_1          0x00000004 /* RW--V */
#define NV_PFB_CONFIG_0_STEREOSCOPIC                          28:28 /* RWIVF */
#define NV_PFB_CONFIG_0_STEREOSCOPIC_DISABLED            0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_0_STEREOSCOPIC_ENABLED             0x00000001 /* RW--V */
#define NV_PFB_CONFIG_1                                  0x00600204 /* RW-4R */
#define NV_PFB_CONFIG_1_FIFO_LOW_MARK                           0:0 /* RWIVF */
#define NV_PFB_CONFIG_1_FIFO_LOW_MARK_512_BITS           0x00000000 /* RWI-V */
#define NV_PFB_CONFIG_1_FIFO_LOW_MARK_2048_BITS          0x00000001 /* RW--V */
#define NV_PFB_START                                     0x00600400 /* RW-4R */
#define NV_PFB_START_ADDRESS                                   21:1 /* RWIUF */
#define NV_PFB_START_ADDRESS_0                           0x00000000 /* RWI-V */
#define NV_PFB_HOR_FRNT_PORCH                            0x00600500 /* RW-4R */
#define NV_PFB_HOR_FRNT_PORCH_PIXELS                           10:0 /* RWIUF */
#define NV_PFB_HOR_FRNT_PORCH_PIXELS_1                   0x00000001 /* RWI-V */
#define NV_PFB_HOR_SYNC_WIDTH                            0x00600510 /* RW-4R */
#define NV_PFB_HOR_SYNC_WIDTH_PIXELS                           10:0 /* RWIUF */
#define NV_PFB_HOR_SYNC_WIDTH_PIXELS_1                   0x00000001 /* RWI-V */
#define NV_PFB_HOR_BACK_PORCH                            0x00600520 /* RW-4R */
#define NV_PFB_HOR_BACK_PORCH_PIXELS                           10:0 /* RWIUF */
#define NV_PFB_HOR_BACK_PORCH_PIXELS_1                   0x00000001 /* RWI-V */
#define NV_PFB_HOR_DISP_WIDTH                            0x00600530 /* RW-4R */
#define NV_PFB_HOR_DISP_WIDTH_PIXELS                           10:0 /* RWIUF */
#define NV_PFB_HOR_DISP_WIDTH_PIXELS_1                   0x00000001 /* RWI-V */
#define NV_PFB_VER_FRNT_PORCH                            0x00600540 /* RW-4R */
#define NV_PFB_VER_FRNT_PORCH_LINES                            10:0 /* RWIUF */
#define NV_PFB_VER_FRNT_PORCH_LINES_1                    0x00000001 /* RWI-V */
#define NV_PFB_VER_SYNC_WIDTH                            0x00600550 /* RW-4R */
#define NV_PFB_VER_SYNC_WIDTH_LINES                            10:0 /* RWIUF */
#define NV_PFB_VER_SYNC_WIDTH_LINES_1                    0x00000001 /* RWI-V */
#define NV_PFB_VER_BACK_PORCH                            0x00600560 /* RW-4R */
#define NV_PFB_VER_BACK_PORCH_LINES                            10:0 /* RWIUF */
#define NV_PFB_VER_BACK_PORCH_LINES_1                    0x00000001 /* RWI-V */
#define NV_PFB_VER_DISP_WIDTH                            0x00600570 /* RW-4R */
#define NV_PFB_VER_DISP_WIDTH_LINES                            10:0 /* RWIUF */
#define NV_PFB_VER_DISP_WIDTH_LINES_1                    0x00000001 /* RWI-V */
/* dev_ram.ref */
#define NV_PRAM                               0x00602FFF:0x00602000 /* RW--D */
#define NV_PRAM_CONFIG_0                                 0x00602200 /* RW-4R */
#define NV_PRAM_CONFIG_0_SIZE                                   1:0 /* RWIVF */
#define NV_PRAM_CONFIG_0_SIZE_12KB                       0x00000000 /* RWI-V */
#define NV_PRAM_CONFIG_0_SIZE_20KB                       0x00000001 /* RW--V */
#define NV_PRAM_CONFIG_0_SIZE_36KB                       0x00000002 /* RW--V */
#define NV_PRAM_CONFIG_0_SIZE_68KB                       0x00000003 /* RW--V */
#define NV_PRAM_HASH_VIRTUAL(i)                  (0x00602400+(i)*4) /* -W-4A */
#define NV_PRAM_HASH_VIRTUAL__SIZE_1                            128 /*       */
#define NV_PRAM_HASH_VIRTUAL_HANDLE                            31:0 /* -W-VF */
#define NV_PRAM_HASH_PHYSICAL                            0x00602600 /* R--4R */
#define NV_PRAM_HASH_PHYSICAL_INSTANCE                         15:0 /* R-IUF */
#define NV_PRAM_HASH_PHYSICAL_INSTANCE_0                 0x00000000 /* R-I-V */
#define NV_PRAM_HASH_PHYSICAL_DEVICE                          22:16 /* R-IUF */
#define NV_PRAM_HASH_PHYSICAL_DEVICE_NOT_FOUND           0x00000000 /* R-I-V */
#define NV_PRAM_HASH_PHYSICAL_FREE_LIE                        24:24 /* R-IVF */
#define NV_PRAM_HASH_PHYSICAL_FREE_LIE_DISABLED          0x00000000 /* R-I-V */
#define NV_PRAM_HASH_PHYSICAL_FREE_LIE_ENABLED           0x00000001 /* R---V */
/* dev_ram.ref */
#define NV_PRAMFC                             0x0064BFFF:0x00648000 /* RW--D */
/* dev_ram.ref */
#define NV_PRAMAU                             0x00604FFF:0x00604000 /* RW--D */
/* dev_spooks.ref */
#define NV_PAUTH                              0x00605FFF:0x00605000 /* RW--D */
#define NV_PAUTH_DEBUG_0                                 0x00605080 /* R--4R */
#define NV_PAUTH_DEBUG_0_BREACH                                 0:0 /* R-IVF */
#define NV_PAUTH_DEBUG_0_BREACH_NOT_DETECTED             0x00000000 /* R-I-V */
#define NV_PAUTH_DEBUG_0_BREACH_DETECTED                 0x00000001 /* R---V */
#define NV_PAUTH_DEBUG_0_EEPROM                                 4:4 /* R-IVF */
#define NV_PAUTH_DEBUG_0_EEPROM_VALID                    0x00000000 /* R-I-V */
#define NV_PAUTH_DEBUG_0_EEPROM_INVALID                  0x00000001 /* R---V */
#define NV_PAUTH_CHIP_TOKEN_0                            0x00605400 /* R--4R */
#define NV_PAUTH_CHIP_TOKEN_0_ENCRYPT_01_08                     7:0 /* R--VF */
#define NV_PAUTH_CHIP_TOKEN_0_ENCRYPT_09_16                    15:8 /* R--VF */
#define NV_PAUTH_CHIP_TOKEN_0_ENCRYPT_17_24                   23:16 /* R--VF */
#define NV_PAUTH_CHIP_TOKEN_0_ENCRYPT_25_32                   31:24 /* R--VF */
#define NV_PAUTH_CHIP_TOKEN_1                            0x00605404 /* R--4R */
#define NV_PAUTH_CHIP_TOKEN_1_ENCRYPT_33_40                     7:0 /* R--VF */
#define NV_PAUTH_CHIP_TOKEN_1_ENCRYPT_41_48                    15:8 /* R--VF */
#define NV_PAUTH_CHIP_TOKEN_1_ENCRYPT_49_56                   23:16 /* R--VF */
#define NV_PAUTH_CHIP_TOKEN_1_ENCRYPT_57_64                   31:24 /* R--VF */
#define NV_PAUTH_PASSWD_0(i)                  (0x00605800+(i)*0x10) /* RW-4A */
#define NV_PAUTH_PASSWD_0__SIZE_1                               128 /*       */
#define NV_PAUTH_PASSWD_0_ENCRYPT_01_08                         7:0 /* CW-VF */
#define NV_PAUTH_PASSWD_0_ENCRYPT_01_08_0                0x00000000 /* CW--V */
#define NV_PAUTH_PASSWD_0_ENCRYPT_09_16                        15:8 /* CW-VF */
#define NV_PAUTH_PASSWD_0_ENCRYPT_09_16_0                0x00000000 /* CW--V */
#define NV_PAUTH_PASSWD_0_ENCRYPT_17_24                       23:16 /* CW-VF */
#define NV_PAUTH_PASSWD_0_ENCRYPT_17_24_0                0x00000000 /* CW--V */
#define NV_PAUTH_PASSWD_0_ENCRYPT_25_32                       31:24 /* CW-VF */
#define NV_PAUTH_PASSWD_0_ENCRYPT_25_32_0                0x00000000 /* CW--V */
#define NV_PAUTH_PASSWD_1(i)                  (0x00605804+(i)*0x10) /* RW-4A */
#define NV_PAUTH_PASSWD_1__SIZE_1                               128 /*       */
#define NV_PAUTH_PASSWD_1_ENCRYPT_33_40                         7:0 /* CW-VF */
#define NV_PAUTH_PASSWD_1_ENCRYPT_33_40_0                0x00000000 /* CW--V */
#define NV_PAUTH_PASSWD_1_ENCRYPT_41_48                        15:8 /* CW-VF */
#define NV_PAUTH_PASSWD_1_ENCRYPT_41_48_0                0x00000000 /* CW--V */
#define NV_PAUTH_PASSWD_1_ENCRYPT_49_56                       23:16 /* CW-VF */
#define NV_PAUTH_PASSWD_1_ENCRYPT_49_56_0                0x00000000 /* CW--V */
#define NV_PAUTH_PASSWD_1_ENCRYPT_57_64                       31:24 /* CW-VF */
#define NV_PAUTH_PASSWD_1_ENCRYPT_57_64_0                0x00000000 /* CW--V */
#define NV_PAUTH_PASSWD_2(i)                  (0x00605808+(i)*0x10) /* RW-4A */
#define NV_PAUTH_PASSWD_2__SIZE_1                               128 /*       */
#define NV_PAUTH_PASSWD_2_RESERVED                             31:0 /* CW-VF */
#define NV_PAUTH_PASSWD_2_RESERVED_0                     0x00000000 /* CW--V */
#define NV_PAUTH_PASSWD_3(i)                  (0x0060580C+(i)*0x10) /* RW-4A */
#define NV_PAUTH_PASSWD_3__SIZE_1                               128 /*       */
#define NV_PAUTH_PASSWD_3_RESERVED                             15:0 /* CW-VF */
#define NV_PAUTH_PASSWD_3_RESERVED_0                     0x00000000 /* CW--V */
#define NV_PAUTH_PASSWD_3_APP_SN_0                            23:16 /* CW-VF */
#define NV_PAUTH_PASSWD_3_APP_SN_0_0                     0x00000000 /* CW--V */
#define NV_PAUTH_PASSWD_3_APP_SN_1                            31:24 /* CW-VF */
#define NV_PAUTH_PASSWD_3_APP_SN_1_0                     0x00000000 /* CW--V */
#define NV_PAUTH_UNUSED_0(i)                     (0x00605000+(i)*4) /* R--4A */
#define NV_PAUTH_UNUSED_0__SIZE_1                                32 /*       */
#define NV_PAUTH_UNUSED_0_VALUE                                31:0 /* C--VF */
#define NV_PAUTH_UNUSED_0_VALUE_0                        0x00000000 /* C---V */
#define NV_PAUTH_UNUSED_1(i)                     (0x00605084+(i)*4) /* R--4A */
#define NV_PAUTH_UNUSED_1__SIZE_1                               223 /*       */
#define NV_PAUTH_UNUSED_1_VALUE                                31:0 /* C--VF */
#define NV_PAUTH_UNUSED_1_VALUE_0                        0x00000000 /* C---V */
#define NV_PAUTH_UNUSED_2(i)                     (0x00605408+(i)*4) /* R--4A */
#define NV_PAUTH_UNUSED_2__SIZE_1                               254 /*       */
#define NV_PAUTH_UNUSED_2_VALUE                                31:0 /* C--VF */
#define NV_PAUTH_UNUSED_2_VALUE_0                        0x00000000 /* C---V */
/* dev_ram.ref */
#define NV_PRAMPW                             0x00606FFF:0x00606000 /* RW--D */
/* dev_ext_devices.ref */
#define NV_PEXTDEV                            0x00608FFF:0x00608000 /* RW--D */
#define NV_PEXTDEV_BOOT_0                                0x00608000 /* R--4R */
#define NV_PEXTDEV_BOOT_0_STRAP_VENDOR                          0:0 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_VENDOR_NVIDIA            0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_VENDOR_SGS               0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE                        1:1 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_VRAM            0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_DRAM            0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BOARD                           3:2 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_BOARD_MOTHER             0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BOARD_ADAPTER_1          0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BOARD_ADAPTER_2          0x00000002 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BOARD_ADAPTER_3          0x00000003 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE                        4:4 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE_PCI             0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE_VL              0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_RESERVED                        7:5 /* R-XVF */
/* dev_ext_devices.ref */
#define NV_PDAC                               0x00609FFF:0x00609000 /* RW--D */
#define NV_PDAC_DATA(i)                          (0x00609000+(i)*4) /* RW-4A */
#define NV_PDAC_DATA__SIZE_1                                     16 /*       */
#define NV_PDAC_DATA_VALUE                                      7:0 /* RW-VF */
/* dev_ext_devices.ref */
#define NV_PEEPROM                            0x0060AFFF:0x0060A000 /* RW--D */
#define NV_PEEPROM_CONTROL                               0x0060A400 /* RW-4R */
#define NV_PEEPROM_CONTROL_DATA                                 7:0 /* RWXVF */
#define NV_PEEPROM_CONTROL_ADDRESS                             14:8 /* RWXUF */
#define NV_PEEPROM_CONTROL_COMMAND                            25:24 /* RWIVF */
#define NV_PEEPROM_CONTROL_COMMAND_NOP_0                 0x00000000 /* RWI-V */
#define NV_PEEPROM_CONTROL_COMMAND_WRITE                 0x00000001 /* RW--V */
#define NV_PEEPROM_CONTROL_COMMAND_READ                  0x00000002 /* RW--V */
#define NV_PEEPROM_CONTROL_COMMAND_NOP_3                 0x00000003 /* RW--V */
#define NV_PEEPROM_CONTROL_STATUS                             28:28 /* R-IVF */
#define NV_PEEPROM_CONTROL_STATUS_IDLE                   0x00000000 /* R---V */
#define NV_PEEPROM_CONTROL_STATUS_BUSY                   0x00000001 /* R-I-V */
/* dev_ext_devices.ref */
#define NV_PROM                               0x00617FFF:0x00610000 /* RW--D */
#define NV_PROM_DATA(i)                            (0x00610000+(i)) /* RW-1A */
#define NV_PROM_DATA__SIZE_1                                  32768 /*       */
#define NV_PROM_DATA_VALUE                                      7:0 /* RW-VF */
/* dev_ext_devices.ref */
#define NV_PALT                               0x0061FFFF:0x00618000 /* RW--D */
#define NV_PALT_DATA(i)                            (0x00618000+(i)) /* RW-1A */
#define NV_PALT_DATA__SIZE_1                                  32768 /*       */
#define NV_PALT_DATA_VALUE                                      7:0 /* RW-VF */
/* dev_ram.ref */
#define NV_PRAMHT                             0x00647FFF:0x00640000 /* RW--D */
/* dev_ram.ref */
#define NV_PRAMRO                             0x00653FFF:0x00650000 /* RW--D */
/* dev_realmode.ref */
#define NV_PRM                                0x006C7FFF:0x006C0000 /* RW--D */
#define NV_PRM_DEBUG_0                                   0x006C0080 /* RW-4R */
#define NV_PRM_DEBUG_0_FLIPFLOP                                 0:0 /* RWIVF */
#define NV_PRM_DEBUG_0_FLIPFLOP_ADDRESS                  0x00000000 /* RWI-V */
#define NV_PRM_DEBUG_0_FLIPFLOP_DATA                     0x00000001 /* RW--V */
#define NV_PRM_DEBUG_0_TEXT_WRITE                               4:4 /* RWIVF */
#define NV_PRM_DEBUG_0_TEXT_WRITE_NOT_PENDING            0x00000000 /* RWI-V */
#define NV_PRM_DEBUG_0_TEXT_WRITE_PENDING                0x00000001 /* RW--V */
#define NV_PRM_DEBUG_0_TEXT_WRITE_SYNC                          8:8 /* R-IVF */
#define NV_PRM_DEBUG_0_TEXT_WRITE_SYNC_NOT_PENDING       0x00000000 /* R-I-V */
#define NV_PRM_DEBUG_0_TEXT_WRITE_SYNC_PENDING           0x00000001 /* R---V */
#define NV_PRM_DEBUG_0_TEXT_CHARACTER                         12:12 /* RWIVF */
#define NV_PRM_DEBUG_0_TEXT_CHARACTER_FOREGROUND         0x00000000 /* RWI-V */
#define NV_PRM_DEBUG_0_TEXT_CHARACTER_BACKGROUND         0x00000001 /* RW--V */
#define NV_PRM_DEBUG_0_TEXT_CURSOR                            16:16 /* RWIVF */
#define NV_PRM_DEBUG_0_TEXT_CURSOR_FOREGROUND            0x00000000 /* RWI-V */
#define NV_PRM_DEBUG_0_TEXT_CURSOR_OFF                   0x00000001 /* RW--V */
#define NV_PRM_DEBUG_0_TEXT                                   20:20 /* RWIVF */
#define NV_PRM_DEBUG_0_TEXT_DISABLED                     0x00000000 /* RWI-V */
#define NV_PRM_DEBUG_0_TEXT_ENABLED                      0x00000001 /* RW--V */
#define NV_PRM_DEBUG_0_TEXT_STATE                             24:24 /* R-IVF */
#define NV_PRM_DEBUG_0_TEXT_STATE_IDLE                   0x00000000 /* R-I-V */
#define NV_PRM_DEBUG_0_TEXT_STATE_RENDERING              0x00000001 /* R---V */
#define NV_PRM_DEBUG_0_TRACE_FLIPFLOP                         28:28 /* RWIVF */
#define NV_PRM_DEBUG_0_TRACE_FLIPFLOP_DISABLED           0x00000000 /* RWI-V */
#define NV_PRM_DEBUG_0_TRACE_FLIPFLOP_ENABLED            0x00000001 /* RW--V */
#define NV_PRM_INTR_0                                    0x006C0100 /* RW-4R */
#define NV_PRM_INTR_0_TRACE_INDEX                             12:12 /* RWIVF */
#define NV_PRM_INTR_0_TRACE_INDEX_NOT_PENDING            0x00000000 /* R-I-V */
#define NV_PRM_INTR_0_TRACE_INDEX_PENDING                0x00000001 /* R---V */
#define NV_PRM_INTR_0_TRACE_INDEX_RESET                  0x00000001 /* -W--V */
#define NV_PRM_INTR_0_TRACE_OVERFLOW                          16:16 /* RWIVF */
#define NV_PRM_INTR_0_TRACE_OVERFLOW_NOT_PENDING         0x00000000 /* R-I-V */
#define NV_PRM_INTR_0_TRACE_OVERFLOW_PENDING             0x00000001 /* R---V */
#define NV_PRM_INTR_0_TRACE_OVERFLOW_RESET               0x00000001 /* -W--V */
#define NV_PRM_INTR_0_VBLANK                                  20:20 /* RWIVF */
#define NV_PRM_INTR_0_VBLANK_NOT_PENDING                 0x00000000 /* R-I-V */
#define NV_PRM_INTR_0_VBLANK_PENDING                     0x00000001 /* R---V */
#define NV_PRM_INTR_0_VBLANK_RESET                       0x00000001 /* -W--V */
#define NV_PRM_INTR_EN_0                                 0x006C0140 /* RW-4R */
#define NV_PRM_INTR_EN_0_TRACE_INDEX                          12:12 /* RWIVF */
#define NV_PRM_INTR_EN_0_TRACE_INDEX_DISABLED            0x00000000 /* RWI-V */
#define NV_PRM_INTR_EN_0_TRACE_INDEX_ENABLED             0x00000001 /* RW--V */
#define NV_PRM_INTR_EN_0_TRACE_OVERFLOW                       16:16 /* RWIVF */
#define NV_PRM_INTR_EN_0_TRACE_OVERFLOW_DISABLED         0x00000000 /* RWI-V */
#define NV_PRM_INTR_EN_0_TRACE_OVERFLOW_ENABLED          0x00000001 /* RW--V */
#define NV_PRM_INTR_EN_0_VBLANK                               20:20 /* RWIVF */
#define NV_PRM_INTR_EN_0_VBLANK_DISABLED                 0x00000000 /* RWI-V */
#define NV_PRM_INTR_EN_0_VBLANK_ENABLED                  0x00000001 /* RW--V */
#define NV_PRM_CONFIG_0                                  0x006C0200 /* RW-4R */
#define NV_PRM_CONFIG_0_TEXT                                    0:0 /* RWIVF */
#define NV_PRM_CONFIG_0_TEXT_DISABLED                    0x00000000 /* RWI-V */
#define NV_PRM_CONFIG_0_TEXT_ENABLED                     0x00000001 /* RW--V */
#define NV_PRM_CONFIG_0_DAC_WIDTH                               4:4 /* RWIVF */
#define NV_PRM_CONFIG_0_DAC_WIDTH_6_BITS                 0x00000000 /* RWI-V */
#define NV_PRM_CONFIG_0_DAC_WIDTH_8_BITS                 0x00000001 /* RW--V */
#define NV_PRM_TRACE                                     0x006C1F00 /* RW-4R */
#define NV_PRM_TRACE__ALIAS_1                       NV_MEMORY_TRACE /*       */
#define NV_PRM_TRACE_IO_CAPTURE                                 1:0 /* RWIVF */
#define NV_PRM_TRACE_IO_CAPTURE_DISABLED                 0x00000000 /* RWI-V */
#define NV_PRM_TRACE_IO_CAPTURE_WRITES                   0x00000001 /* RW--V */
#define NV_PRM_TRACE_IO_CAPTURE_READS                    0x00000002 /* RW--V */
#define NV_PRM_TRACE_IO_CAPTURE_READS_WRITES             0x00000003 /* RW--V */
#define NV_PRM_TRACE_IO_WRITE                                   4:4 /* RWIVF */
#define NV_PRM_TRACE_IO_WRITE_NONE                       0x00000000 /* R-I-V */
#define NV_PRM_TRACE_IO_WRITE_OCCURED                    0x00000001 /* R---V */
#define NV_PRM_TRACE_IO_WRITE_RESET                      0x00000001 /* -W--V */
#define NV_PRM_TRACE_IO_READ                                    5:5 /* RWIVF */
#define NV_PRM_TRACE_IO_READ_NONE                        0x00000000 /* R-I-V */
#define NV_PRM_TRACE_IO_READ_OCCURED                     0x00000001 /* R---V */
#define NV_PRM_TRACE_IO_READ_RESET                       0x00000001 /* -W--V */
#define NV_PRM_TRACE_VGA_CAPTURE                                9:8 /* RWIVF */
#define NV_PRM_TRACE_VGA_CAPTURE_DISABLED                0x00000000 /* RWI-V */
#define NV_PRM_TRACE_VGA_CAPTURE_WRITES                  0x00000001 /* RW--V */
#define NV_PRM_TRACE_VGA_CAPTURE_READS                   0x00000002 /* RW--V */
#define NV_PRM_TRACE_VGA_CAPTURE_READS_WRITES            0x00000003 /* RW--V */
#define NV_PRM_TRACE_VGA_WRITE                                12:12 /* RWIVF */
#define NV_PRM_TRACE_VGA_WRITE_NONE                      0x00000000 /* R-I-V */
#define NV_PRM_TRACE_VGA_WRITE_OCCURED                   0x00000001 /* R---V */
#define NV_PRM_TRACE_VGA_WRITE_RESET                     0x00000001 /* -W--V */
#define NV_PRM_TRACE_VGA_READ                                 13:13 /* RWIVF */
#define NV_PRM_TRACE_VGA_READ_NONE                       0x00000000 /* R-I-V */
#define NV_PRM_TRACE_VGA_READ_OCCURED                    0x00000001 /* R---V */
#define NV_PRM_TRACE_VGA_READ_RESET                      0x00000001 /* -W--V */
#define NV_PRM_TRACE_INDEX                               0x006C1F10 /* RW-4R */
#define NV_PRM_TRACE_INDEX__ALIAS_1           NV_MEMORY_TRACE_INDEX /*       */
#define NV_PRM_TRACE_INDEX_ADDRESS                             13:0 /* RWIUF */
#define NV_PRM_TRACE_INDEX_ADDRESS_0                     0x00000000 /* RWI-V */
#define NV_PRM_IGNORE_0                                  0x006C1F20 /* RW-4R */
#define NV_PRM_IGNORE_0__ALIAS_1                    NV_MEMORY_TRACE /*       */
#define NV_PRM_IGNORE_0_DAC                                     1:0 /* RWIVF */
#define NV_PRM_IGNORE_0_DAC_DISABLED                     0x00000000 /* RWI-V */
#define NV_PRM_IGNORE_0_DAC_WRITES                       0x00000001 /* RW--V */
#define NV_PRM_IGNORE_0_DAC_READS                        0x00000002 /* RW--V */
#define NV_PRM_IGNORE_0_DAC_READS_WRITES                 0x00000003 /* RW--V */
#define NV_PRM_IGNORE_0_GAMEPORT                                5:4 /* RWIVF */
#define NV_PRM_IGNORE_0_GAMEPORT_DISABLED                0x00000000 /* RWI-V */
#define NV_PRM_IGNORE_0_GAMEPORT_WRITES                  0x00000001 /* RW--V */
#define NV_PRM_IGNORE_0_GAMEPORT_READS                   0x00000002 /* RW--V */
#define NV_PRM_IGNORE_0_GAMEPORT_READS_WRITES            0x00000003 /* RW--V */
#define NV_PRM_IGNORE_0_MPU401                                  9:8 /* RWIVF */
#define NV_PRM_IGNORE_0_MPU401_DISABLED                  0x00000000 /* RWI-V */
#define NV_PRM_IGNORE_0_MPU401_WRITES                    0x00000001 /* RW--V */
#define NV_PRM_IGNORE_0_MPU401_READS                     0x00000002 /* RW--V */
#define NV_PRM_IGNORE_0_MPU401_READS_WRITES              0x00000003 /* RW--V */
#define NV_PRM_IGNORE_1                                  0x006C1F24 /* RW-4R */
#define NV_PRM_IGNORE_1__ALIAS_1                    NV_MEMORY_TRACE /*       */
#define NV_PRM_IGNORE_1_FM_INDEX                                1:0 /* RWIVF */
#define NV_PRM_IGNORE_1_FM_INDEX_DISABLED                0x00000000 /* RWI-V */
#define NV_PRM_IGNORE_1_FM_INDEX_WRITES                  0x00000001 /* RW--V */
#define NV_PRM_IGNORE_1_FM_INDEX_READS                   0x00000002 /* RW--V */
#define NV_PRM_IGNORE_1_FM_INDEX_READS_WRITES            0x00000003 /* RW--V */
#define NV_PRM_IGNORE_1_FM_DATA                                 5:4 /* RWIVF */
#define NV_PRM_IGNORE_1_FM_DATA_DISABLED                 0x00000000 /* RWI-V */
#define NV_PRM_IGNORE_1_FM_DATA_WRITES                   0x00000001 /* RW--V */
#define NV_PRM_IGNORE_1_FM_DATA_READS                    0x00000002 /* RW--V */
#define NV_PRM_IGNORE_1_FM_DATA_READS_WRITES             0x00000003 /* RW--V */
#define NV_PRM_IGNORE_1_SB_DIGITAL                              9:8 /* RWIVF */
#define NV_PRM_IGNORE_1_SB_DIGITAL_DISABLED              0x00000000 /* RWI-V */
#define NV_PRM_IGNORE_1_SB_DIGITAL_WRITES                0x00000001 /* RW--V */
#define NV_PRM_IGNORE_1_SB_DIGITAL_READS                 0x00000002 /* RW--V */
#define NV_PRM_IGNORE_1_SB_DIGITAL_READS_WRITES          0x00000003 /* RW--V */
#define NV_PRM_IGNORE_1_VGA_HW                                17:16 /* RWIVF */
#define NV_PRM_IGNORE_1_VGA_HW_DISABLED                  0x00000000 /* RWI-V */
#define NV_PRM_IGNORE_1_VGA_HW_WRITES                    0x00000001 /* RW--V */
#define NV_PRM_IGNORE_1_VGA_HW_READS                     0x00000002 /* RW--V */
#define NV_PRM_IGNORE_1_VGA_HW_READS_WRITES              0x00000003 /* RW--V */
#define NV_PRM_IGNORE_1_VGA_SW                                21:20 /* RWIVF */
#define NV_PRM_IGNORE_1_VGA_SW_DISABLED                  0x00000000 /* RWI-V */
#define NV_PRM_IGNORE_1_VGA_SW_WRITES                    0x00000001 /* RW--V */
#define NV_PRM_IGNORE_1_VGA_SW_READS                     0x00000002 /* RW--V */
#define NV_PRM_IGNORE_1_VGA_SW_READS_WRITES              0x00000003 /* RW--V */
#define NV_PRM_IGNORE_1_VGA_MISC                              25:24 /* RWIVF */
#define NV_PRM_IGNORE_1_VGA_MISC_DISABLED                0x00000000 /* RWI-V */
#define NV_PRM_IGNORE_1_VGA_MISC_WRITES                  0x00000001 /* RW--V */
#define NV_PRM_IGNORE_1_VGA_MISC_READS                   0x00000002 /* RW--V */
#define NV_PRM_IGNORE_1_VGA_MISC_READS_WRITES            0x00000003 /* RW--V */
#define NV_PRM_MPU_401                                   0x006C0400 /* RW-4R */
#define NV_PRM_MPU_401_DATA                                     7:0 /* RWXVF */
#define NV_PRM_MPU_401_DATA_ACK                          0x000000FE /* RW--V */
#define NV_PRM_MPU_401_WRITE_STATUS                             8:8 /* RWIVF */
#define NV_PRM_MPU_401_WRITE_STATUS_EMPTY                0x00000000 /* RWI-V */
#define NV_PRM_MPU_401_WRITE_STATUS_FULL                 0x00000001 /* RW--V */
#define NV_PRM_MPU_401_WRITE_FORCE                            12:12 /* RWIVF */
#define NV_PRM_MPU_401_WRITE_FORCE_EMPTY                 0x00000000 /* RWI-V */
#define NV_PRM_MPU_401_WRITE_FORCE_DISABLED              0x00000001 /* RW--V */
#define NV_PRM_MPU_401_READ_STATUS                            16:16 /* RWIVF */
#define NV_PRM_MPU_401_READ_STATUS_FULL                  0x00000000 /* RWI-V */
#define NV_PRM_MPU_401_READ_STATUS_EMPTY                 0x00000001 /* RW--V */
#define NV_PRM_MPU_401_READ_FORCE                             20:20 /* RWIVF */
#define NV_PRM_MPU_401_READ_FORCE_FULL                   0x00000000 /* RWI-V */
#define NV_PRM_MPU_401_READ_FORCE_DISABLED               0x00000001 /* RW--V */
#define NV_PRM_MPU_401_UART_MODE                              24:24 /* RWIVF */
#define NV_PRM_MPU_401_UART_MODE_COMPLEX                 0x00000000 /* RWI-V */
#define NV_PRM_MPU_401_UART_MODE_SIMPLE                  0x00000001 /* RW--V */
/* dev_realmode.ref */
#define NV_PRMIO                              0x006DFFFF:0x006D0000 /* RW--D */
#define NV_PRMIO_GAME_PORT                               0x006D0201 /* RW-1R */
#define NV_PRMIO_GAME_PORT__ALIAS_1                 NV_IO_GAME_PORT /*       */
#define NV_PRMIO_FM_ADDRESS__SOUND_BLASTER               0x006D0228 /* -W-1R */
#define NV_PRMIO_FM_ADDRESS__ADLIB                       0x006D0388 /* -W-1R */
#define NV_PRMIO_SB_WRITE_STATUS                         0x006D022C /* R--1R */
#define NV_PRMIO_SB_WRITE_STATUS__ALIAS_1     NV_IO_SB_WRITE_STATUS /*       */
#define NV_PRMIO_SB_READ_STATUS                          0x006D022E /* RW-1R */
#define NV_PRMIO_SB_READ_STATUS__ALIAS_1       NV_IO_SB_READ_STATUS /*       */
#define NV_PRMIO_MPU_401_DATA                            0x006D0330 /* R--1R */
#define NV_PRMIO_MPU_401_DATA__ALIAS_1           NV_IO_MPU_401_DATA /*       */
#define NV_PRMIO_MPU_401_STATUS                          0x006D0331 /* R--1R */
#define NV_PRMIO_MPU_401_STATUS__ALIAS_1       NV_IO_MPU_401_STATUS /*       */
#define NV_PRMIO_EX_MISC_OUT__WRITE                      0x006D03C2 /* -W-1R */
#define NV_PRMIO_EX_MISC_OUT__READ                       0x006D03CC /* R--1R */
#define NV_PRMIO_EX_FEATURE__WRITE_MONO                  0x006D03BA /* -W-1R */
#define NV_PRMIO_EX_FEATURE__WRITE_COLOR                 0x006D03DA /* -W-1R */
#define NV_PRMIO_EX_FEATURE__READ                        0x006D03CA /* R--1R */
#define NV_PRMIO_EX_STATUS_0__READ                       0x006D03c2 /* R--1R */
#define NV_PRMIO_EX_STATUS_1__READ_MONO                  0x006D03BA /* R--1R */
#define NV_PRMIO_EX_STATUS_1__READ_COLOR                 0x006D03DA /* R--1R */
#define NV_PRMIO_SQ_ADDRESS                              0x006D03C4 /* RW-1R */
#define NV_PRMIO_SQ_ADDRESS__ALIAS_1               NV_IO_SQ_ADDRESS /*       */
#define NV_PRMIO_SQ_MAP_MASK                             0x006D03C5 /* RW-1R */
#define NV_PRMIO_SQ_MAP_MASK__ALIAS_1             NV_IO_SQ_MAP_MASK /*       */
#define NV_PRMIO_SQ_MAP_MASK__INDEX                      0x00000002 /*       */
#define NV_PRMIO_SQ_CHAR_MAP                             0x006D03C5 /* RW-1R */
#define NV_PRMIO_SQ_CHAR_MAP__ALIAS_1             NV_IO_SQ_CHAR_MAP /*       */
#define NV_PRMIO_SQ_CHAR_MAP__INDEX                      0x00000003 /*       */
#define NV_PRMIO_SQ_MEM_MAP                              0x006D03C5 /* RW-1R */
#define NV_PRMIO_SQ_MEM_MAP__ALIAS_1               NV_IO_SQ_MEM_MAP /*       */
#define NV_PRMIO_SQ_MEM_MAP__INDEX                       0x00000004 /*       */
#define NV_PRMIO_CC_ADDRESS__MONO                        0x006D03B4 /* RW-1R */
#define NV_PRMIO_CC_ADDRESS__COLOR                       0x006D03D4 /* RW-1R */
#define NV_PRMIO_CC_MAX_SCAN__MONO                       0x006D03B5 /* RW-1R */
#define NV_PRMIO_CC_MAX_SCAN__COLOR                      0x006D03D5 /* RW-1R */
#define NV_PRMIO_CC_MAX_SCAN__INDEX                      0x00000009 /*       */
#define NV_PRMIO_CC_CURSOR_START__MONO                   0x006D03B5 /* RW-1R */
#define NV_PRMIO_CC_CURSOR_START__COLOR                  0x006D03D5 /* RW-1R */
#define NV_PRMIO_CC_CURSOR_START__INDEX                  0x0000000A /*       */
#define NV_PRMIO_CC_START_HI__MONO                       0x006D03B5 /* RW-1R */
#define NV_PRMIO_CC_START_HI__COLOR                      0x006D03D5 /* RW-1R */
#define NV_PRMIO_CC_START_HI__INDEX                      0x0000000C /*       */
#define NV_PRMIO_CC_START_LO__MONO                       0x006D03B5 /* RW-1R */
#define NV_PRMIO_CC_START_LO__COLOR                      0x006D03D5 /* RW-1R */
#define NV_PRMIO_CC_START_LO__INDEX                      0x0000000D /*       */
#define NV_PRMIO_CC_CURSOR_HI__MONO                      0x006D03B5 /* RW-1R */
#define NV_PRMIO_CC_CURSOR_HI__COLOR                     0x006D03D5 /* RW-1R */
#define NV_PRMIO_CC_CURSOR_HI__INDEX                     0x0000000E /*       */
#define NV_PRMIO_CC_CURSOR_LO__MONO                      0x006D03B5 /* RW-1R */
#define NV_PRMIO_CC_CURSOR_LO__COLOR                     0x006D03D5 /* RW-1R */
#define NV_PRMIO_CC_CURSOR_LO__INDEX                     0x0000000F /*       */
#define NV_PRMIO_CC_OFFSET__MONO                         0x006D03B5 /* RW-1R */
#define NV_PRMIO_CC_OFFSET__COLOR                        0x006D03D5 /* RW-1R */
#define NV_PRMIO_CC_OFFSET__INDEX                        0x00000013 /*       */
#define NV_PRMIO_GC_ADDRESS                              0x006D03CE /* RW-1R */
#define NV_PRMIO_GC_ADDRESS__ALIAS_1               NV_IO_GC_ADDRESS /*       */
#define NV_PRMIO_GC_SR                                   0x006D03CF /* RW-1R */
#define NV_PRMIO_GC_SR__ALIAS_1                         NV_IO_GC_SR /*       */
#define NV_PRMIO_GC_SR__INDEX                            0x00000000 /*       */
#define NV_PRMIO_GC_ENABLE_SR                            0x006D03CF /* RW-1R */
#define NV_PRMIO_GC_ENABLE_SR__ALIAS_1           NV_IO_GC_ENABLE_SR /*       */
#define NV_PRMIO_GC_ENABLE_SR__INDEX                     0x00000001 /*       */
#define NV_PRMIO_GC_COLOR_COMP                           0x006D03CF /* RW-1R */
#define NV_PRMIO_GC_COLOR_COMP__ALIAS_1         NV_IO_GC_COLOR_COMP /*       */
#define NV_PRMIO_GC_COLOR_COMP__INDEX                    0x00000002 /*       */
#define NV_PRMIO_GC_ROTATE                               0x006D03CF /* RW-1R */
#define NV_PRMIO_GC_ROTATE__ALIAS_1                 NV_IO_GC_ROTATE /*       */
#define NV_PRMIO_GC_ROTATE__INDEX                        0x00000003 /*       */
#define NV_PRMIO_GC_READ_MAP                             0x006D03CF /* RW-1R */
#define NV_PRMIO_GC_READ_MAP__ALIAS_1             NV_IO_GC_READ_MAP /*       */
#define NV_PRMIO_GC_READ_MAP__INDEX                      0x00000004 /*       */
#define NV_PRMIO_GC_MODE                                 0x006D03CF /* RW-1R */
#define NV_PRMIO_GC_MODE__ALIAS_1                     NV_IO_GC_MODE /*       */
#define NV_PRMIO_GC_MODE__INDEX                          0x00000005 /*       */
#define NV_PRMIO_GC_MISC                                 0x006D03CF /* RW-1R */
#define NV_PRMIO_GC_MISC__ALIAS_1                     NV_IO_GC_MISC /*       */
#define NV_PRMIO_GC_MISC__INDEX                          0x00000006 /*       */
#define NV_PRMIO_GC_DONT_CARE                            0x006D03CF /* RW-1R */
#define NV_PRMIO_GC_DONT_CARE__ALIAS_1           NV_IO_GC_DONT_CARE /*       */
#define NV_PRMIO_GC_DONT_CARE__INDEX                     0x00000007 /*       */
#define NV_PRMIO_GC_BIT_MASK                             0x006D03CF /* RW-1R */
#define NV_PRMIO_GC_BIT_MASK__ALIAS_1             NV_IO_GC_BIT_MASK /*       */
#define NV_PRMIO_GC_BIT_MASK__INDEX                      0x00000008 /*       */
#define NV_PRMIO_AT_ADDRESS__WRITE_FLIPFLOP0             0x006D03C0 /* -W-1R */
#define NV_PRMIO_AT_ADDRESS__READ                        0x006D03C0 /* R--1R */
#define NV_PRMIO_AT_MODE__WRITE_FLIPFLOP1                0x006D03C0 /* -W-1R */
#define NV_PRMIO_AT_MODE__READ                           0x006D03C1 /* R--1R */
#define NV_PRMIO_DAC_DATA                                0x006D03C9 /* RW-1R */
#define NV_PRMIO_DAC_DATA__ALIAS_1                   NV_IO_DAC_DATA /*       */
/* dev_realmode.ref */
#define NV_PRMVGA                             0x006EFFFF:0x006E0000 /* RW--D */
/* dev_ram.ref */
#define NV_PRAMIN                             0x007FFFFF:0x00700000 /* RW--D */
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
#define NV_USER_PW(i,j,k)   (0x800020+(i)*0x10000+(j)*0x2000+(k)*4) /* -W-4A */
#define NV_USER_PW__SIZE_1                                      128 /*       */
#define NV_USER_PW__SIZE_2                                        8 /*       */
#define NV_USER_PW__SIZE_3                                        4 /*       */
#define NV_USER_PW_PASSWORD                                    31:0 /* -W-VF */
/* dev_framebuffer.ref */
#define NV_PDFB                               0x01FFFFFF:0x01000000 /* RW--D */
#define NV_PDFB_DATA032(i)                       (0x01000000+(i)*4) /* RW-4A */
#define NV_PDFB_DATA032__SIZE_1                             4194304 /*       */
#define NV_PDFB_DATA032_VALUE                                  31:0 /* RWXVF */
#define NV_PDFB_DATA024(i)           (0x01000000+((i)/2)*4+((i)%2)) /* RW-3A */
#define NV_PDFB_DATA024__SIZE_1                             8388608 /*       */
#define NV_PDFB_DATA024_VALUE                                  23:0 /* RWXVF */
#define NV_PDFB_DATA016(i)           (0x01000000+((i)/3)*4+((i)%3)) /* RW-2A */
#define NV_PDFB_DATA016__SIZE_1                            12582912 /*       */
#define NV_PDFB_DATA016_VALUE                                  15:0 /* RWXVF */
#define NV_PDFB_DATA008(i)                         (0x01000000+(i)) /* RW-1A */
#define NV_PDFB_DATA008__SIZE_1                            16777216 /*       */
#define NV_PDFB_DATA008_VALUE                                   7:0 /* RWXVF */
/* dev_dma.ref */
#define NV_NOTIFY                             0x0000000F:0x00000000 /* RW--M */
#define NV_NOTIFY_TIME_0                      ( 0*32+31):( 0*32+ 5) /* RWXUF */
#define NV_NOTIFY_TIME_1                      ( 1*32+28):( 1*32+ 0) /* RWXUF */
#define NV_NOTIFY_RETURN_VALUE                ( 2*32+31):( 2*32+ 0) /* RWXVF */
#define NV_NOTIFY_ERROR_CODE                  ( 3*32+15):( 3*32+ 0) /* RWXUF */
#define NV_NOTIFY_STATUS                      ( 3*32+31):( 3*32+24) /* RWXUF */
#define NV_NOTIFY_STATUS_COMPLETED                       0x00000000 /* RW--V */
#define NV_NOTIFY_STATUS_IN_PROCESS                      0x00000001 /* RW--V */
/* dev_realmode.ref */
#define NV_TRACE                              0x0000FFFF:0x00000000 /* RW--M */
#define NV_TRACE_DATA                         ( 0*32+ 7):( 0*32+ 0) /* RWXVF */
#define NV_TRACE_FLIPFLOP                     ( 0*32+13):( 0*32+13) /* RWXVF */
#define NV_TRACE_FLIPFLOP_ADDRESS                        0x00000000 /* RW--V */
#define NV_TRACE_FLIPFLOP_DATA                           0x00000001 /* RW--V */
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
#define NV_RAMHT_CHID                         ( 1*32+30):( 1*32+24) /* RWXUF */
#define NV_RAMHT_FREE_LIE                     ( 1*32+31):( 1*32+31) /* RWXVF */
#define NV_RAMHT_FREE_LIE_DISABLED                       0x00000000 /* RW--V */
#define NV_RAMHT_FREE_LIE_ENABLED                        0x00000001 /* RW--V */
/* dev_ram.ref */
#define NV_RAMRO__SIZE_0                      0x000007FF:0x00000000 /* RW--M */
#define NV_RAMRO__SIZE_1                      0x00000FFF:0x00000000 /* RW--M */
#define NV_RAMRO__SIZE_2                      0x00001FFF:0x00000000 /* RW--M */
#define NV_RAMRO__SIZE_3                      0x00003FFF:0x00000000 /* RW--M */
#define NV_RAMRO_METHOD                       ( 0*32+15):( 0*32+ 0) /* RWXUF */
#define NV_RAMRO_CHID                         ( 0*32+22):( 0*32+16) /* RWXUF */
#define NV_RAMRO_SIZE                         ( 0*32+26):( 0*32+26) /* RWXVF */
#define NV_RAMRO_SIZE_ILLEGAL                            0x00000000 /* RW--V */
#define NV_RAMRO_SIZE_16_BIT                             0x00000001 /* RW--V */
#define NV_RAMRO_TYPE                         ( 0*32+27):( 0*32+27) /* RWXVF */
#define NV_RAMRO_TYPE_WRITE                              0x00000000 /* RW--V */
#define NV_RAMRO_TYPE_READ                               0x00000001 /* RW--V */
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
#define NV_RAMFC__SIZE_0                      0x000007FF:0x00000000 /* RW--M */
#define NV_RAMFC__SIZE_1                      0x00000FFF:0x00000000 /* RW--M */
#define NV_RAMFC__SIZE_2                      0x00001FFF:0x00000000 /* RW--M */
#define NV_RAMFC__SIZE_3                      0x00003FFF:0x00000000 /* RW--M */
#define NV_RAMFC_INSTANCE_0                   ( 0*32+15):( 0*32+ 0) /* RWXUF */
#define NV_RAMFC_DEVICE_0                     ( 0*32+22):( 0*32+16) /* RWXUF */
#define NV_RAMFC_DEVICE_0_NOT_FOUND                      0x00000000 /* RW--V */
#define NV_RAMFC_LIE_0                        ( 0*32+24):( 0*32+24) /* RWXVF */
#define NV_RAMFC_LIE_0_DISABLED                          0x00000000 /* RW--V */
#define NV_RAMFC_LIE_0_ENABLED                           0x00000001 /* RW--V */
#define NV_RAMFC_SUBCHANNEL                   ( 0*32+30):( 0*32+28) /* RWXUF */
#define NV_RAMFC_OBJECT                       ( 0*32+31):( 0*32+31) /* RWXVF */
#define NV_RAMFC_OBJECT_UNCHANGED                        0x00000000 /* RW--V */
#define NV_RAMFC_OBJECT_CHANGED                          0x00000001 /* RW--V */
#define NV_RAMFC_INSTANCE_1                   ( 1*32+15):( 1*32+ 0) /* RWXUF */
#define NV_RAMFC_DEVICE_1                     ( 1*32+22):( 1*32+16) /* RWXUF */
#define NV_RAMFC_DEVICE_1_NOT_FOUND                      0x00000000 /* RW--V */
#define NV_RAMFC_LIE_1                        ( 1*32+24):( 1*32+24) /* RWXVF */
#define NV_RAMFC_LIE_1_DISABLED                          0x00000000 /* RW--V */
#define NV_RAMFC_LIE_1_ENABLED                           0x00000001 /* RW--V */
#define NV_RAMFC_INSTANCE_7                   ( 7*32+15):( 7*32+ 0) /* RWXUF */
#define NV_RAMFC_DEVICE_7                     ( 7*32+22):( 7*32+16) /* RWXUF */
#define NV_RAMFC_DEVICE_7_NOT_FOUND                      0x00000000 /* RW--V */
#define NV_RAMFC_LIE_7                        ( 7*32+24):( 7*32+24) /* RWXVF */
#define NV_RAMFC_LIE_7_DISABLED                          0x00000000 /* RW--V */
#define NV_RAMFC_LIE_7_ENABLED                           0x00000001 /* RW--V */
/* dev_ram.ref */
#define NV_RAMPW                              0x000003FF:0x00000000 /* RW--M */
#define NV_RAMPW_PASSWORD_0                   ( 0*32+31):( 0*32+ 0) /* RWXVF */
#define NV_RAMPW_PASSWORD_1                   ( 1*32+31):( 1*32+ 0) /* RWXVF */
/* dev_ram.ref */
#define NV_RAMIN                              0x000FFFFF:0x00000000 /* RW--M */
#define NV_RAMIN_CONTEXT_0                    ( 0*32+31):( 0*32+ 0) /*       */
#define NV_RAMIN_CONTEXT_1                    ( 1*32+31):( 1*32+ 0) /*       */
#define NV_RAMIN_CONTEXT_2                    ( 2*32+31):( 2*32+ 0) /*       */
#define NV_RAMIN_CONTEXT_3                    ( 3*32+31):( 3*32+ 0) /*       */
/* dev_ram.ref */
#define NV_DMA_ADJUST                         ( 0*32+11):( 0*32+ 0) /* RWXUF */
#define NV_DMA_PAGE_TABLE                     ( 0*32+16):( 0*32+16) /* RWXVF */
#define NV_DMA_PAGE_TABLE_NOT_PRESENT                    0x00000000 /* RW--V */
#define NV_DMA_PAGE_TABLE_PRESENT                        0x00000001 /* RW--V */
#define NV_DMA_LIMIT                          ( 1*32+31):( 1*32+ 0) /* RWXUF */
#define NV_DMA_PAGE                           ( 2*32+ 0):( 2*32+ 0) /* RWXVF */
#define NV_DMA_PAGE_NOT_PRESENT                          0x00000000 /* RW--V */
#define NV_DMA_PAGE_PRESENT                              0x00000001 /* RW--V */
#define NV_DMA_ACCESS                         ( 2*32+ 1):( 2*32+ 1) /* RWXVF */
#define NV_DMA_ACCESS_READ_ONLY                          0x00000000 /* RW--V */
#define NV_DMA_ACCESS_READ_AND_WRITE                     0x00000001 /* RW--V */
#define NV_DMA_FRAME_ADDRESS                  ( 2*32+31):( 2*32+12) /* RWXUF */
/* dev_ram.ref */
#define NV_AUDIN                              0x00000033:0x00000000 /* RW--M */
#define NV_AUDIN_AE_STATE                     ( 0*32+31):( 0*32+ 0) /* RWWVF */
#define NV_AUDIN_AE_SKIP_COUNT                ( 1*32+31):( 1*32+ 0) /* RWWUF */
#define NV_AUDIN_AE_POSITION                  ( 2*32+31):( 2*32+ 0) /* RWWUF */
#define NV_AUDIN_RM_NEXT_INSTANCE             ( 3*32+15):( 3*32+ 0) /* RWWUF */
#define NV_AUDIN_RM_STATE                     ( 3*32+31):( 3*32+16) /* RWWVF */
#define NV_AUDIN_SU_START_TIME_LOW            ( 4*32+15):( 4*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_STATE                     ( 4*32+31):( 4*32+16) /* RWWVF */
#define NV_AUDIN_SU_START_TIME_HIGH           ( 5*32+31):( 5*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_SKIP_INC                  ( 6*32+31):( 6*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_BUFF0_DMA_INSTANCE        ( 7*32+15):( 7*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_BUFF0_NOTIFY_INSTANCE     ( 7*32+31):( 7*32+16) /* RWWUF */
#define NV_AUDIN_SU_BUFF0_START_POSITION      ( 8*32+31):( 8*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_BUFF0_LAST_SAMPLE_PLUS1   ( 9*32+31):( 9*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_BUFF1_DMA_INSTANCE        (10*32+15):(10*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_BUFF1_NOTIFY_INSTANCE     (10*32+31):(10*32+16) /* RWWUF */
#define NV_AUDIN_SU_BUFF1_START_POSITION      (11*32+31):(11*32+ 0) /* RWWUF */
#define NV_AUDIN_SU_BUFF1_LAST_SAMPLE_PLUS1   (12*32+31):(12*32+ 0) /* RWWUF */
#define NV_AUDIN_AE_BUFF0_INTR_NOTIFY         ( 0*32+20):( 0*32+20) /* RWXVF */
#define NV_AUDIN_AE_BUFF1_INTR_NOTIFY         ( 0*32+21):( 0*32+21) /* RWXVF */
#define NV_AUDIN_AE_BUFF0_INTR_CHAIN_GAP      ( 0*32+22):( 0*32+22) /* RWXVF */
#define NV_AUDIN_AE_BUFF1_INTR_CHAIN_GAP      ( 0*32+23):( 0*32+23) /* RWXVF */
#define NV_AUDIN_AE_BUFF0_IN_USE              ( 0*32+24):( 0*32+24) /* RWXVF */
#define NV_AUDIN_AE_BUFF1_IN_USE              ( 0*32+25):( 0*32+25) /* RWXVF */
#define NV_AUDIN_AE_CURRENT_BUFFER            ( 0*32+28):( 0*32+28) /* RWXVF */
#define NV_AUDIN_AE_CURRENT_BUFFER_0                     0x00000000 /* RW--V */
#define NV_AUDIN_AE_CURRENT_BUFFER_1                     0x00000001 /* RW--V */
#define NV_AUDIN_RM_BUFF0_INTR_NOTIFY         ( 3*32+20):( 3*32+20) /* RWXVF */
#define NV_AUDIN_RM_BUFF1_INTR_NOTIFY         ( 3*32+21):( 3*32+21) /* RWXVF */
#define NV_AUDIN_RM_BUFF0_INTR_CHAIN_GAP      ( 3*32+22):( 3*32+22) /* RWXVF */
#define NV_AUDIN_RM_BUFF1_INTR_CHAIN_GAP      ( 3*32+23):( 3*32+23) /* RWXVF */
#define NV_AUDIN_SU_BUFF0_OFFSET              ( 4*32+17):( 4*32+16) /* RWXVF */
#define NV_AUDIN_SU_BUFF1_OFFSET              ( 4*32+21):( 4*32+20) /* RWXVF */
#define NV_AUDIN_SU_BUFF0_IN_USE              ( 4*32+24):( 4*32+24) /* RWXVF */
#define NV_AUDIN_SU_BUFF1_IN_USE              ( 4*32+25):( 4*32+25) /* RWXVF */
#define NV_AUDIN_SU_BUFF0_NOTIFY              ( 4*32+26):( 4*32+26) /* RWXVF */
#define NV_AUDIN_SU_BUFF0_NOTIFY_DMA_WRITE               0x00000000 /* RW--V */
#define NV_AUDIN_SU_BUFF0_NOTIFY_INTERRUPT               0x00000001 /* RW--V */
#define NV_AUDIN_SU_BUFF1_NOTIFY              ( 4*32+27):( 4*32+27) /* RWXVF */
#define NV_AUDIN_SU_BUFF1_NOTIFY_DMA_WRITE               0x00000000 /* RW--V */
#define NV_AUDIN_SU_BUFF1_NOTIFY_INTERRUPT               0x00000001 /* RW--V */
#define NV_AUDIN_SU_FORMAT                    ( 4*32+29):( 4*32+28) /* RWXVF */
#define NV_AUDIN_SU_FORMAT_LINEAR                        0x00000000 /* RW--V */
#define NV_AUDIN_SU_FORMAT_ULAW                          0x00000001 /* RW--V */
#define NV_AUDIN_SU_FORMAT_ALAW                          0x00000002 /* RW--V */
#define NV_AUDIN_SU_FORMAT_OFFSET8                       0x00000003 /* RW--V */
#define NV_AUDIN_SU_CHANNEL                   ( 4*32+30):( 4*32+30) /* RWXVF */
#define NV_AUDIN_SU_CHANNEL_MONO                         0x00000000 /* RW--V */
#define NV_AUDIN_SU_CHANNEL_STEREO                       0x00000001 /* RW--V */
#define NV_AUDIN_SU_PDFB                      ( 4*32+31):( 4*32+31) /* RWXVF */
/* dev_ram.ref */
#define NV_AUDOUT                             0x0000003B:0x00000000 /* RW--M */
#define NV_AUDOUT_AE_POSITION_LOW             ( 0*32+15):( 0*32+ 0) /* RWXUF */
#define NV_AUDOUT_AE_STATE                    ( 0*32+31):( 0*32+16) /* RWXVF */
#define NV_AUDOUT_AE_POSITION                 ( 1*32+31):( 1*32+ 0) /* RWXUF */
#define NV_AUDOUT_AE_VOLUME_CHANNEL_1         ( 2*32+15):( 2*32+ 0) /* RWXUF */
#define NV_AUDOUT_AE_VOLUME_CHANNEL_0         ( 2*32+31):( 2*32+16) /* RWXUF */
#define NV_AUDOUT_RM_NEXT_INSTANCE            ( 3*32+15):( 3*32+ 0) /* RWXUF */
#define NV_AUDOUT_RM_STATE                    ( 3*32+31):( 3*32+16) /* RWXVF */
#define NV_AUDOUT_RM_VOLUME_CHANNEL_1         ( 4*32+15):( 4*32+ 0) /* RWXUF */
#define NV_AUDOUT_RM_VOLUME_CHANNEL_0         ( 4*32+31):( 4*32+16) /* RWXUF */
#define NV_AUDOUT_SU_POSITION_INC             ( 5*32+31):( 5*32+ 0) /* RWXUF */
#define NV_AUDOUT_SU_VOLUME_CHANNEL_1         ( 6*32+15):( 6*32+ 0) /* RWXUF */
#define NV_AUDOUT_SU_VOLUME_CHANNEL_0         ( 6*32+31):( 6*32+16) /* RWXUF */
#define NV_AUDOUT_SU_START_TIME_LOW           ( 7*32+15):( 7*32+ 0) /* RWXUF */
#define NV_AUDOUT_SU_STATE                    ( 7*32+31):( 7*32+16) /* RWXUF */
#define NV_AUDOUT_SU_START_TIME_HIGH          ( 8*32+31):( 8*32+ 0) /* RWXUF */
#define NV_AUDOUT_SU_BUFF0_DMA_INSTANCE       ( 9*32+15):( 9*32+ 0) /* RWXUF */
#define NV_AUDOUT_SU_BUFF0_NOTIFY_INSTANCE    ( 9*32+31):( 9*32+16) /* RWXUF */
#define NV_AUDOUT_SU_BUFF0_START_POSITION     (10*32+31):(10*32+ 0) /* RWXUF */
#define NV_AUDOUT_SU_BUFF0_LAST_SAMPLE_PLUS1  (11*32+31):(11*32+ 0) /* RWXUF */
#define NV_AUDOUT_SU_BUFF1_DMA_INSTANCE       (12*32+15):(12*32+ 0) /* RWXUF */
#define NV_AUDOUT_SU_BUFF1_NOTIFY_INSTANCE    (12*32+31):(12*32+16) /* RWXUF */
#define NV_AUDOUT_SU_BUFF1_START_POSITION     (13*32+31):(13*32+ 0) /* RWXUF */
#define NV_AUDOUT_SU_BUFF1_LAST_SAMPLE_PLUS1  (14*32+31):(14*32+ 0) /* RWXUF */
#define NV_AUDOUT_AE_INTR_MIXING              ( 0*32+16):( 0*32+16) /* RWXVF */
#define NV_AUDOUT_AE_INTR_VOLUME              ( 0*32+17):( 0*32+17) /* RWXVF */
#define NV_AUDOUT_AE_BUFF0_INTR_NOTIFY        ( 0*32+20):( 0*32+20) /* RWXVF */
#define NV_AUDOUT_AE_BUFF1_INTR_NOTIFY        ( 0*32+21):( 0*32+21) /* RWXVF */
#define NV_AUDOUT_AE_BUFF0_INTR_CHAIN_GAP     ( 0*32+22):( 0*32+22) /* RWXVF */
#define NV_AUDOUT_AE_BUFF1_INTR_CHAIN_GAP     ( 0*32+23):( 0*32+23) /* RWXVF */
#define NV_AUDOUT_AE_BUFF0_IN_USE             ( 0*32+24):( 0*32+24) /* RWXVF */
#define NV_AUDOUT_AE_BUFF1_IN_USE             ( 0*32+25):( 0*32+25) /* RWXVF */
#define NV_AUDOUT_AE_CURRENT_BUFFER           ( 0*32+28):( 0*32+28) /* RWXVF */
#define NV_AUDOUT_AE_CURRENT_BUFFER_0                    0x00000000 /* RW--V */
#define NV_AUDOUT_AE_CURRENT_BUFFER_1                    0x00000001 /* RW--V */
#define NV_AUDOUT_RM_INTR_MIXING              ( 3*32+16):( 3*32+16) /* RWXVF */
#define NV_AUDOUT_RM_INTR_VOLUME              ( 3*32+17):( 3*32+17) /* RWXVF */
#define NV_AUDOUT_RM_BUFF0_INTR_NOTIFY        ( 3*32+20):( 3*32+20) /* RWXVF */
#define NV_AUDOUT_RM_BUFF1_INTR_NOTIFY        ( 3*32+21):( 3*32+21) /* RWXVF */
#define NV_AUDOUT_RM_BUFF0_INTR_CHAIN_GAP     ( 3*32+22):( 3*32+22) /* RWXVF */
#define NV_AUDOUT_RM_BUFF1_INTR_CHAIN_GAP     ( 3*32+23):( 3*32+23) /* RWXVF */
#define NV_AUDOUT_SU_BUFF0_BYTE_OFFSET        ( 7*32+17):( 7*32+16) /* RWXVF */
#define NV_AUDOUT_SU_BUFF1_BYTE_OFFSET        ( 7*32+21):( 7*32+20) /* RWXVF */
#define NV_AUDOUT_SU_BUFF0_IN_USE             ( 7*32+24):( 7*32+24) /* RWXVF */
#define NV_AUDOUT_SU_BUFF1_IN_USE             ( 7*32+25):( 7*32+25) /* RWXVF */
#define NV_AUDOUT_SU_BUFF0_NOTIFY             ( 7*32+26):( 7*32+26) /* RWXVF */
#define NV_AUDOUT_SU_BUFF0_NOTIFY_DMA_WRITE              0x00000000 /* RW--V */
#define NV_AUDOUT_SU_BUFF0_NOTIFY_INTERRUPT              0x00000001 /* RW--V */
#define NV_AUDOUT_SU_BUFF1_NOTIFY             ( 7*32+27):( 7*32+27) /* RWXVF */
#define NV_AUDOUT_SU_BUFF1_NOTIFY_DMA_WRITE              0x00000000 /* RW--V */
#define NV_AUDOUT_SU_BUFF1_NOTIFY_INTERRUPT              0x00000001 /* RW--V */
#define NV_AUDOUT_SU_FORMAT                   ( 7*32+29):( 7*32+28) /* RWXVF */
#define NV_AUDOUT_SU_FORMAT_LINEAR                       0x00000000 /* RW--V */
#define NV_AUDOUT_SU_FORMAT_ULAW                         0x00000001 /* RW--V */
#define NV_AUDOUT_SU_FORMAT_ALAW                         0x00000002 /* RW--V */
#define NV_AUDOUT_SU_FORMAT_OFFSET8                      0x00000003 /* RW--V */
#define NV_AUDOUT_SU_CHANNEL                  ( 7*32+30):( 7*32+30) /* RWXVF */
#define NV_AUDOUT_SU_CHANNEL_MONO                        0x00000000 /* RW--V */
#define NV_AUDOUT_SU_CHANNEL_STEREO                      0x00000001 /* RW--V */
#define NV_AUDOUT_SU_PDFB                     ( 7*32+31):( 7*32+31) /* RWXVF */
/* dev_ram.ref */
#define NV_AUDNOTE                            0x0000007B:0x00000000 /* RW--M */
#define NV_AUDNOTE_AE_VIBRATO_TREMOLO_LEVEL   ( 0*32+15):( 0*32+ 0) /* RWXUF */
#define NV_AUDNOTE_AE_STATE                   ( 0*32+31):( 0*32+16) /* RWXVF */
#define NV_AUDNOTE_AE_ADSR_LEVEL              ( 1*32+31):( 1*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_POSITION_LOW            ( 2*32+15):( 2*32+ 0) /* RWXUF */
#define NV_AUDNOTE_AE_ADSR_VOLUME             ( 2*32+31):( 2*32+16) /* RWXUF */
#define NV_AUDNOTE_AE_POSITION_INC            ( 3*32+31):( 3*32+ 0) /* RWXUF */
#define NV_AUDNOTE_AE_VIBRATO_POSITION        ( 4*32+31):( 4*32+ 0) /* RWXUF */
#define NV_AUDNOTE_AE_VIBRATO_VALUE           ( 5*32+31):( 5*32+ 0) /* RWXUF */
#define NV_AUDNOTE_AE_TREMOLO_POSITION        ( 6*32+31):( 6*32+ 0) /* RWXUF */
#define NV_AUDNOTE_AE_VOLUME_CHANNEL_1        ( 7*32+15):( 7*32+ 0) /* RWXUF */
#define NV_AUDNOTE_AE_VOLUME_CHANNEL_0        ( 7*32+31):( 7*32+16) /* RWXUF */
#define NV_AUDNOTE_RM_NEXT_INSTANCE           ( 8*32+15):( 8*32+ 0) /* RWXUF */
#define NV_AUDNOTE_RM_STATE                   ( 8*32+31):( 8*32+16) /* RWXVF */
#define NV_AUDNOTE_RM_VOLUME_CHANNEL_1        ( 9*32+15):( 9*32+ 0) /* RWXUF */
#define NV_AUDNOTE_RM_VOLUME_CHANNEL_0        ( 9*32+31):( 9*32+16) /* RWXUF */
#define NV_AUDNOTE_SU_STATE                   (10*32+31):(10*32+16) /* RWXVF */
#define NV_AUDNOTE_SU_DMA_INSTANCE            (11*32+15):(11*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_NOTIFY_INSTANCE         (11*32+31):(11*32+16) /* RWXUF */
#define NV_AUDNOTE_SU_START_TIME_LOW          (12*32+15):(12*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_STOP_TIME_LOW           (12*32+31):(12*32+16) /* RWXUF */
#define NV_AUDNOTE_SU_START_TIME_HIGH         (13*32+31):(13*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_STOP_TIME_HIGH          (14*32+31):(14*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_VIBRATO_TREMOLO_TIME_LO (15*32+15):(15*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_RELEASE_TIME_LOW        (15*32+31):(15*32+16) /* RWXUF */
#define NV_AUDNOTE_SU_VIBRATO_TREMOLO_TIME_HI (16*32+31):(16*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_RELEASE_TIME_HIGH       (17*32+31):(17*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_VOLUME_CHANNEL_1        (18*32+15):(18*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_VOLUME_CHANNEL_0        (18*32+31):(18*32+16) /* RWXUF */
#define NV_AUDNOTE_SU_POSITION                (19*32+31):(19*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_POSITION_INC            (20*32+31):(20*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_LOOP_START_POSITION     (21*32+31):(21*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_LOOP_LAST_SAMPLE_PLUS1  (22*32+31):(22*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_LAST_SAMPLE_PLUS1       (23*32+31):(23*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_ATTACK_RATE             (24*32+15):(24*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_DECAY_RATE              (24*32+31):(24*32+16) /* RWXUF */
#define NV_AUDNOTE_SU_RELEASE_RATE            (25*32+15):(25*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_SUSTAIN_RATE            (25*32+31):(25*32+16) /* RWXUF */
#define NV_AUDNOTE_SU_SUSTAIN_LEVEL           (26*32+15):(26*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_PORTAMENTO_RATE         (26*32+31):(26*32+16) /* RWXUF */
#define NV_AUDNOTE_SU_VIBRATO_DEPTH           (27*32+15):(27*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_VIBRATO_TREMOLO_ATTACK  (27*32+31):(27*32+16) /* RWXUF */
#define NV_AUDNOTE_SU_VIBRATO_POSITION_INC    (28*32+15):(28*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_TREMOLO_POSITION_INC    (28*32+31):(28*32+16) /* RWXUF */
#define NV_AUDNOTE_SU_TREMOLO_DEPTH           (29*32+15):(29*32+ 0) /* RWXUF */
#define NV_AUDNOTE_SU_TREMOLO_OFFSET          (29*32+31):(29*32+16) /* RWXUF */
#define NV_AUDNOTE_AE_INTR_MIXING             ( 0*32+16):( 0*32+16) /* RWXVF */
#define NV_AUDNOTE_AE_INTR_VOLUME             ( 0*32+17):( 0*32+17) /* RWXVF */
#define NV_AUDNOTE_AE_INTR_NOTIFY             ( 0*32+20):( 0*32+20) /* RWXVF */
#define NV_AUDNOTE_AE_IN_USE                  ( 0*32+24):( 0*32+24) /* RWXVF */
#define NV_AUDNOTE_RM_INTR_MIXING             ( 8*32+16):( 8*32+16) /* RWXVF */
#define NV_AUDNOTE_RM_INTR_VOLUME             ( 8*32+17):( 8*32+17) /* RWXVF */
#define NV_AUDNOTE_RM_INTR_NOTIFY             ( 8*32+20):( 8*32+20) /* RWXVF */
#define NV_AUDNOTE_SU_OFFSET                  (10*32+17):(10*32+16) /* RWXVF */
#define NV_AUDNOTE_SU_IN_USE                  (10*32+24):(10*32+24) /* RWXVF */
#define NV_AUDNOTE_SU_NOTIFY                  (10*32+26):(10*32+26) /* RWXVF */
#define NV_AUDNOTE_SU_NOTIFY_DMA_WRITE                   0x00000000 /* RW--V */
#define NV_AUDNOTE_SU_NOTIFY_INTERRUPT                   0x00000001 /* RW--V */
#define NV_AUDNOTE_SU_FORMAT                  (10*32+29):(10*32+28) /* RWXVF */
#define NV_AUDNOTE_SU_FORMAT_LINEAR                      0x00000000 /* RW--V */
#define NV_AUDNOTE_SU_FORMAT_ULAW                        0x00000001 /* RW--V */
#define NV_AUDNOTE_SU_FORMAT_ALAW                        0x00000002 /* RW--V */
#define NV_AUDNOTE_SU_FORMAT_OFFSET8                     0x00000003 /* RW--V */
#define NV_AUDNOTE_SU_CHANNEL                 (10*32+30):(10*32+30) /* RWXVF */
#define NV_AUDNOTE_SU_CHANNEL_MONO                       0x00000000 /* RW--V */
#define NV_AUDNOTE_SU_CHANNEL_STEREO                     0x00000001 /* RW--V */
#define NV_AUDNOTE_SU_PDFB                    (10*32+31):(10*32+31) /* RWXVF */
#endif /* _NV_REF_H_ */

