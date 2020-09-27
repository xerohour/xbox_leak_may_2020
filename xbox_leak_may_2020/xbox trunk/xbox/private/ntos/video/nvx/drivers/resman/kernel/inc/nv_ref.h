//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/***************************************************************************\
*                                                                           *
*               Hardware Reference Manual extracted defines.                *
*                                                                           *
\***************************************************************************/
#ifndef _NV_REF_H_
#define _NV_REF_H_

//
// VGA hardware is supported on NV3/NV4/NV10
//
#define NV_VGA  1

#ifdef NV3_HW
//
// VGA hardware is supported on NV3
//
#include <nv3_ref.h>
#define NV_PFIFO_CACHE1_SIZE                                  32
#define BUG_BM_OFF()
#define BUG_BM_ON()

#else
//
// These registers can be accessed by chip-independent code as
// well as chip-dependent code.
//

/* NVidia Corporation */ 
/* basis: nv4 manuals 
built on Tue Nov 17 04:01:20 PST 1998*/

#ifndef _NV_REG_H_
#define _NV_REG_H_

#ifdef DEBUG
/*
 * These macros are here strictly for debugging purposes.
 * For the real definitions to be used in the code, refer to
 * the chip-specific headers.
 */
#define NV_PGRAPH_DMA_A_XLATE_INST                       0x00401040 /* RW-4R */
#define NV_PGRAPH_DMA_A_CONTROL                          0x00401044 /* RW-4R */
#define NV_PGRAPH_DMA_A_LIMIT                            0x00401048 /* RW-4R */
#define NV_PGRAPH_DMA_A_TLB_PTE                          0x0040104C /* RW-4R */
#define NV_PGRAPH_DMA_A_TLB_TAG                          0x00401050 /* RW-4R */
#define NV_PGRAPH_DMA_A_ADJ_OFFSET                       0x00401054 /* RW-4R */
#define NV_PGRAPH_DMA_A_OFFSET                           0x00401058 /* RW-4R */
#define NV_PGRAPH_DMA_A_SIZE                             0x0040105C /* RW-4R */
#define NV_PGRAPH_DMA_A_Y_SIZE                           0x00401060 /* RW-4R */

#define NV_PGRAPH_DMA_B_XLATE_INST                       0x00401080 /* RW-4R */
#define NV_PGRAPH_DMA_B_CONTROL                          0x00401084 /* RW-4R */
#define NV_PGRAPH_DMA_B_LIMIT                            0x00401088 /* RW-4R */
#define NV_PGRAPH_DMA_B_TLB_PTE                          0x0040108C /* RW-4R */
#define NV_PGRAPH_DMA_B_TLB_TAG                          0x00401090 /* RW-4R */
#define NV_PGRAPH_DMA_B_ADJ_OFFSET                       0x00401094 /* RW-4R */
#define NV_PGRAPH_DMA_B_OFFSET                           0x00401098 /* RW-4R */
#define NV_PGRAPH_DMA_B_SIZE                             0x0040109C /* RW-4R */
#define NV_PGRAPH_DMA_B_Y_SIZE                           0x004010A0 /* RW-4R */
#endif

/* dev_bus.ref */
/*
 * This macro is used in the NvDeviceBaseGet 16bit call.
 */
#define NV_SPACE                              0x01FFFFFF:0x00000000 /* RW--D */
/* dev_bus.ref */
#define NV_CONFIG                             0x000000FF:0x00000000 /* RW--D */
#define NV_CONFIG_PCI_NV_0                               0x00000000 /* R--4R */
#define NV_CONFIG_PCI_NV_0__ALIAS_1                NV_PBUS_PCI_NV_0 /*       */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID                          31:16 /* R--UF */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV01_A              0x00000009 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV01_B_B02_B03_C01  0x00000008 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV02_A01            0x00000010 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV03_NOACPI         0x00000018 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV03_ACPI           0x00000019 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV04                0x00000020 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID0         0x00000028 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID1         0x00000029 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID2         0x0000002A /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID3         0x0000002B /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV06_DEVID0         0x0000002C /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV06_DEVID1         0x0000002D /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV06_DEVID2         0x0000002E /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV06_DEVID3         0x0000002F /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID0         0x000000A0 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID1         0x000000A1 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID2         0x000000A2 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID3         0x000000A3 /* ----V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID0         0x00000100 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID1         0x00000101 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID2         0x00000102 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID3         0x00000103 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV11_DEVID0         0x00000110 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV11_DEVID1         0x00000111 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV11_DEVID2         0x00000112 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV11_DEVID3         0x00000113 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID0         0x00000150 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID1         0x00000151 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID2         0x00000152 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID3         0x00000153 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV20_DEVID0         0x00000200 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV20_DEVID1         0x00000201 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV20_DEVID2         0x00000202 /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID_NV20_DEVID3         0x00000203 /* C---V */
#define NV_CONFIG_PCI_NV_1                               0x00000004 /* RW-4R */
#define NV_CONFIG_PCI_NV_1__ALIAS_1                NV_PBUS_PCI_NV_1 /*       */
#define NV_CONFIG_PCI_NV_2                               0x00000008 /* R--4R */
#define NV_CONFIG_PCI_NV_2__ALIAS_1                NV_PBUS_PCI_NV_2 /*       */
#define NV_CONFIG_PCI_NV_3                               0x0000000C /* RW-4R */
#define NV_CONFIG_PCI_NV_3__ALIAS_1                NV_PBUS_PCI_NV_3 /*       */
#define NV_CONFIG_PCI_NV_4                               0x00000010 /* RW-4R */
#define NV_CONFIG_PCI_NV_4__ALIAS_1                NV_PBUS_PCI_NV_4 /*       */
#define NV_CONFIG_PCI_NV_5                               0x00000014 /* RW-4R */
#define NV_CONFIG_PCI_NV_5__ALIAS_1                NV_PBUS_PCI_NV_5 /*       */
#define NV_CONFIG_PCI_NV_6                               0x00000018 /* RW-4R */
#define NV_CONFIG_PCI_NV_6__ALIAS_1                NV_PBUS_PCI_NV_6 /*       */
#define NV_CONFIG_PCI_NV_7(i)                    (0x0000001C+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_NV_7__SIZE_1                                4 /*       */
#define NV_CONFIG_PCI_NV_7__ALIAS_1                NV_PBUS_PCI_NV_7 /*       */
#define NV_CONFIG_PCI_NV_11                              0x0000002C /* R--4R */
#define NV_CONFIG_PCI_NV_11__ALIAS_1              NV_PBUS_PCI_NV_11 /*       */
#define NV_CONFIG_PCI_NV_12                              0x00000030 /* RW-4R */
#define NV_CONFIG_PCI_NV_12__ALIAS_1              NV_PBUS_PCI_NV_12 /*       */
#define NV_CONFIG_PCI_NV_13                              0x00000034 /* RW-4R */
#define NV_CONFIG_PCI_NV_13__ALIAS_1              NV_PBUS_PCI_NV_13 /*       */
#define NV_CONFIG_PCI_NV_14                              0x00000038 /* R--4R */
#define NV_CONFIG_PCI_NV_14__ALIAS_1              NV_PBUS_PCI_NV_14 /*       */
#define NV_CONFIG_PCI_NV_15                              0x0000003C /* RW-4R */
#define NV_CONFIG_PCI_NV_15__ALIAS_1              NV_PBUS_PCI_NV_15 /*       */
#define NV_CONFIG_PCI_NV_16                              0x00000040 /* RW-4R */
#define NV_CONFIG_PCI_NV_16__ALIAS_1              NV_PBUS_PCI_NV_16 /*       */
#define NV_CONFIG_PCI_NV_17                              0x00000044 /* RW-4R */
#define NV_CONFIG_PCI_NV_17__ALIAS_1              NV_PBUS_PCI_NV_17 /*       */
#define NV_CONFIG_PCI_NV_18                              0x00000048 /* RW-4R */
#define NV_CONFIG_PCI_NV_18__ALIAS_1              NV_PBUS_PCI_NV_18 /*       */
#define NV_CONFIG_PCI_NV_19                              0x0000004C /* RW-4R */
#define NV_CONFIG_PCI_NV_19__ALIAS_1              NV_PBUS_PCI_NV_19 /*       */
#define NV_CONFIG_PCI_NV_20                              0x00000050 /* RW-4R */
#define NV_CONFIG_PCI_NV_20__ALIAS_1              NV_PBUS_PCI_NV_20 /*       */
#define NV_CONFIG_PCI_NV_21                              0x00000054 /* RW-4R */
#define NV_CONFIG_PCI_NV_21__ALIAS_1              NV_PBUS_PCI_NV_21 /*       */
#define NV_CONFIG_PCI_NV_22                              0x00000058 /* RW-4R */
#define NV_CONFIG_PCI_NV_22__ALIAS_1              NV_PBUS_PCI_NV_22 /*       */
#define NV_CONFIG_PCI_NV_23                              0x0000005C /* RW-4R */
#define NV_CONFIG_PCI_NV_23__ALIAS_1              NV_PBUS_PCI_NV_23 /*       */
#define NV_CONFIG_PCI_NV_24                              0x00000060 /* RW-4R */
#define NV_CONFIG_PCI_NV_24__ALIAS_1              NV_PBUS_PCI_NV_24 /*       */
#define NV_CONFIG_PCI_NV_25                              0x00000064 /* RW-4R */
#define NV_CONFIG_PCI_NV_25__ALIAS_1              NV_PBUS_PCI_NV_25 /*       */
#define NV_CONFIG_PCI_NV_26(i)                   (0x00000068+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_NV_26__SIZE_1                              38 /*       */
#define NV_CONFIG_PCI_NV_26__ALIAS_1              NV_PBUS_PCI_NV_25 /*       */
/* dev_dac.ref */
#define NV_PRAMDAC                            0x00680FFF:0x00680300 /* RW--D */
#define NV_PRAMDAC_CU_START_POS                          0x00680300 /* RW-4R */
#define NV_PRAMDAC_CU_START_POS_X                              11:0 /* RWXSF */
#define NV_PRAMDAC_CU_START_POS_Y                             27:16 /* RWXSF */
#define NV_PRAMDAC_CURSOR_CNTRL                          0x00680320 /* RWI4R */
#define NV_PRAMDAC_CURSOR_CNTRL_ADDRESS                         3:0 /* RW--F */
#define NV_PRAMDAC_CURSOR_CNTRL_RAM                             8:8 /* -W--F */
#define NV_PRAMDAC_CURSOR_CNTRL_RAM_RD                          9:9 /* R---F */
#define NV_PRAMDAC_CURSOR_CNTR_TESTMODE                       16:16 /* -W--F */
#define NV_PRAMDAC_CURSOR_CNTR_TESTMODE_RD                    17:17 /* R---F */
#define NV_PRAMDAC_CURSOR_CNTR_TESTMODE_ENABLE                    1 /* RW--V */
#define NV_PRAMDAC_CURSOR_CNTR_TESTMODE_DISABLE                   0 /* RW--V */
#define NV_PRAMDAC_CURSOR_DATA_31_0                      0x00680324 /* RWI4R */
#define NV_PRAMDAC_CURSOR_DATA_31_0_VAL                        31:0 /* RW--F */
#define NV_PRAMDAC_CURSOR_DATA_63_32                     0x00680328 /* RWI4R */
#define NV_PRAMDAC_CURSOR_DATA_63_32_VAL                       31:0 /* RW--F */
#define NV_PRAMDAC_CURSOR_DATA_95_64                     0x0068032C /* RWI4R */
#define NV_PRAMDAC_CURSOR_DATA_95_64_VAL                       31:0 /* RW--F */
#define NV_PRAMDAC_CURSOR_DATA_127_96                    0x00680330 /* RWI4R */
#define NV_PRAMDAC_CURSOR_DATA_127_96_VAL                      31:0 /* RW--F */
#define NV_PRAMDAC_NVPLL_COEFF                           0x00680500 /* RW-4R */
#define NV_PRAMDAC_NVPLL_COEFF_MDIV                             7:0 /* RWIUF */
#define NV_PRAMDAC_NVPLL_COEFF_NDIV                            15:8 /* RWIUF */
#define NV_PRAMDAC_NVPLL_COEFF_PDIV                           18:16 /* RWIVF */
#define NV_PRAMDAC_MPLL_COEFF                            0x00680504 /* RW-4R */
#define NV_PRAMDAC_MPLL_COEFF_MDIV                              7:0 /* RWIUF */
#define NV_PRAMDAC_MPLL_COEFF_NDIV                             15:8 /* RWIUF */
#define NV_PRAMDAC_MPLL_COEFF_PDIV                            18:16 /* RWIVF */
#define NV_PRAMDAC_VPLL_COEFF                            0x00680508 /* RW-4R */
#define NV_PRAMDAC_VPLL_COEFF_MDIV                              7:0 /* RWIUF */
#define NV_PRAMDAC_VPLL_COEFF_NDIV                             15:8 /* RWIUF */
#define NV_PRAMDAC_VPLL_COEFF_PDIV                            18:16 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT                      0x0068050C /* RW-4R */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE                 0:0 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE_XTAL     0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE_VIP      0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_SOURCE                     10:8 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_SOURCE_DEFAULT       0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_SOURCE_PROG_MPLL     0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_SOURCE_PROG_VPLL     0x00000002 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_SOURCE_PROG_NVPLL    0x00000004 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_SOURCE_PROG_ALL      0x00000007 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_MSOURCE                     8:8 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_MSOURCE_DEFAULT      0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_MSOURCE_PROG         0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_V1SOURCE                    9:9 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_V1SOURCE_DEFAULT     0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_V1SOURCE_PROG        0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_NVSOURCE                  10:10 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_NVSOURCE_DEFAULT     0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_NVSOURCE_PROG        0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_V2SOURCE                  11:11 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_V2SOURCE_DEFAULT     0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_V2SOURCE_PROG        0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK_TV                17:16 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK_TV_NONE      0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK_TV_VSCLK     0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK_TV_PCLK      0x00000002 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK_TV_BOTH      0x00000003 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK2_TV               19:18 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK2_TV_NONE     0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK2_TV_VSCLK    0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK2_TV_PCLK     0x00000002 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK2_TV_BOTH     0x00000003 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_TVCLK_SOURCE              20:20 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_TVCLK_SOURCE_EXT     0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_TVCLK_SOURCE_VIP     0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_TVCLK_RATIO               24:24 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_TVCLK_RATIO_DB1      0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_TVCLK_RATIO_DB2      0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO                28:28 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO_DB1       0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO_DB2       0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK2_RATIO               29:29 /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK2_RATIO_DB1      0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK2_RATIO_DB2      0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_SETUP_CONTROL                     0x00680510 /* RW-4R */
#define NV_PRAMDAC_PLL_SETUP_CONTROL_VALUE                     10:0 /* RWIVF */
#define NV_PRAMDAC_PLL_SETUP_CONTROL_VAL                 0x0000044E /* RWI-V */
/*
 * NV_PRAMDAC_PLL_SETUP_PWRDWN is chip-dependent
 */
#define NV_PRAMDAC_PLL_TEST_COUNTER                      0x00680514 /* RW-4R */
#define NV_PRAMDAC_PLL_TEST_COUNTER_NOOFIPCLKS                  9:0 /* -WIVF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_VALUE                      15:0 /* R--VF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE                    16:16 /* RWIVF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE_DEASSERTED    0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE_ASSERTED      0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_RESET                     20:20 /* RWIVF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_RESET_DEASSERTED     0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_RESET_ASSERTED       0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_VCLK2         0x00000004 /* RW--V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_MCLK          0x00000002 /* RWI-V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_VCLK          0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE_NVCLK         0x00000000 /* RW--V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_VPLL2_LOCK                27:27 /* R--VF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_VPLL2_NOTLOCKED      0x00000000 /* R---V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_VPLL2_LOCKED         0x00000001 /* R---V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_PDIV_RST                  28:28 /* RWIVF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_PDIVRST_DEASSERTED   0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_PDIVRST_ASSERTED     0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCK                29:29 /* R--VF */
#define NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_NOTLOCKED      0x00000000 /* R---V */
#define NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCKED         0x00000001 /* R---V */
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
#define NV_PRAMDAC_VPLL2_COEFF                           0x00680520 /* RW-4R */
#define NV_PRAMDAC_VPLL2_COEFF_MDIV                             7:0 /* RWIUF */
#define NV_PRAMDAC_VPLL2_COEFF_NDIV                            15:8 /* RWIUF */
#define NV_PRAMDAC_VPLL2_COEFF_PDIV                           18:16 /* RWIVF */
#define NV_PRAMDAC_SEL_CLK                               0x00680524 /* RW-4R */
#define NV_PRAMDAC_SEL_CLK_SPREAD_SPECTRUM_NVPLL                0:0 /* RWIVF */
#define NV_PRAMDAC_SEL_CLK_SPREAD_SPECTRUM_NVPLL_OFF     0x00000000 /* RWI-V */
#define NV_PRAMDAC_SEL_CLK_SPREAD_SPECTRUM_NVPLL_ON      0x00000001 /* RW--V */
#define NV_PRAMDAC_SEL_CLK_SPREAD_SPECTRUM_MPLL                 2:2 /* RWIVF */
#define NV_PRAMDAC_SEL_CLK_SPREAD_SPECTRUM_MPLL_OFF      0x00000000 /* RWI-V */
#define NV_PRAMDAC_SEL_CLK_SPREAD_SPECTRUM_MPLL_ON       0x00000001 /* RW--V */
#define NV_PRAMDAC_SEL_CLK_SPREAD_SPECTRUM_VPLL1                4:4 /* RWIVF */
#define NV_PRAMDAC_SEL_CLK_SPREAD_SPECTRUM_VPLL1_OFF     0x00000000 /* RWI-V */
#define NV_PRAMDAC_SEL_CLK_SPREAD_SPECTRUM_VPLL1_ON      0x00000001 /* RW--V */
#define NV_PRAMDAC_SEL_CLK_SPREAD_SPECTRUM_VPLL2                6:6 /* RWIVF */
#define NV_PRAMDAC_SEL_CLK_SPREAD_SPECTRUM_VPLL2_OFF     0x00000000 /* RWI-V */
#define NV_PRAMDAC_SEL_CLK_SPREAD_SPECTRUM_VPLL2_ON      0x00000001 /* RW--V */
#define NV_PRAMDAC_SEL_CLK_IFPCLK1                            16:16 /* RWIVF */
#define NV_PRAMDAC_SEL_CLK_IFPCLK1_SEL_FPCLK1            0x00000000 /* RWI-V */
#define NV_PRAMDAC_SEL_CLK_IFPCLK1_SEL_FPCLK2            0x00000001 /* RW--V */
#define NV_PRAMDAC_SEL_CLK_IFPCLK2                            18:18 /* RWIVF */
#define NV_PRAMDAC_SEL_CLK_IFPCLK2_SEL_FPCLK1            0x00000000 /* RWI-V */
#define NV_PRAMDAC_SEL_CLK_IFPCLK2_SEL_FPCLK2            0x00000001 /* RW--V */
#define NV_PRAMDAC_PLL_COMPAT                            0x00680528 /* RW-4R */
#define NV_PRAMDAC_PLL_COMPAT_NVPLL_DET_MODE                    1:0 /* RWIVF */
#define NV_PRAMDAC_PLL_COMPAT_NVPLL_DET_MODE_MULTOFF     0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COMPAT_NVPLL_DET_MODE_AUTO        0x00000002 /* RW--V */
#define NV_PRAMDAC_PLL_COMPAT_NVPLL_DET_MODE_MULTON      0x00000003 /* RW--V */
#define NV_PRAMDAC_PLL_COMPAT_NVPLL_DET_STAT                    3:3 /* R--VF */
#define NV_PRAMDAC_PLL_COMPAT_NVPLL_DET_STAT_OFF         0x00000000 /* R---V */
#define NV_PRAMDAC_PLL_COMPAT_NVPLL_DET_STAT_ON          0x00000001 /* R---V */
#define NV_PRAMDAC_PLL_COMPAT_MPLL_DET_MODE                     5:4 /* RWIVF */
#define NV_PRAMDAC_PLL_COMPAT_MPLL_DET_MODE_MULTOFF      0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COMPAT_MPLL_DET_MODE_AUTO         0x00000002 /* RW--V */
#define NV_PRAMDAC_PLL_COMPAT_MPLL_DET_MODE_MULTON       0x00000003 /* RW--V */
#define NV_PRAMDAC_PLL_COMPAT_MPLL_DET_STAT                     7:7 /* R--VF */
#define NV_PRAMDAC_PLL_COMPAT_MPLL_DET_STAT_OFF          0x00000000 /* R---V */
#define NV_PRAMDAC_PLL_COMPAT_MPLL_DET_STAT_ON           0x00000001 /* R---V */
#define NV_PRAMDAC_PLL_COMPAT_VPLL_DET_MODE                     9:8 /* RWIVF */
#define NV_PRAMDAC_PLL_COMPAT_VPLL_DET_MODE_MULTOFF      0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COMPAT_VPLL_DET_MODE_AUTO         0x00000002 /* RW--V */
#define NV_PRAMDAC_PLL_COMPAT_VPLL_DET_MODE_MULTON       0x00000003 /* RW--V */
#define NV_PRAMDAC_PLL_COMPAT_VPLL_DET_STAT                   11:11 /* R--VF */
#define NV_PRAMDAC_PLL_COMPAT_VPLL_DET_STAT_OFF          0x00000000 /* R---V */
#define NV_PRAMDAC_PLL_COMPAT_VPLL_DET_STAT_ON           0x00000001 /* R---V */
#define NV_PRAMDAC_PLL_COMPAT_VPLL2_DET_MODE                  13:12 /* RWIVF */
#define NV_PRAMDAC_PLL_COMPAT_VPLL2_DET_MODE_MULTOFF     0x00000000 /* RWI-V */
#define NV_PRAMDAC_PLL_COMPAT_VPLL2_DET_MODE_AUTO        0x00000002 /* RW--V */
#define NV_PRAMDAC_PLL_COMPAT_VPLL2_DET_MODE_MULTON      0x00000003 /* RW--V */
#define NV_PRAMDAC_PLL_COMPAT_VPLL2_DET_STAT                  15:15 /* R--VF */
#define NV_PRAMDAC_PLL_COMPAT_VPLL2_DET_STAT_OFF         0x00000000 /* R---V */
#define NV_PRAMDAC_PLL_COMPAT_VPLL2_DET_STAT_ON          0x00000001 /* R---V */
#define NV_PRAMDAC_PLL_COMPAT_MPDIV_XOR                       26:24 /* RWIVF */
#define NV_PRAMDAC_PLL_COMPAT_MPDIV_XOR_DISABLED         0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL                       0x00680600 /* RW-4R */
#define NV_PRAMDAC_GENERAL_CONTROL_PIXMIX32_BIT                 0:0 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_PIXMIX32_BIT_24       0x00000001 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_PIXMIX32_BIT_31       0x00000000 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_PIXMIX                       5:4 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_PIXMIX_OFF            0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_PIXMIX_POS            0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_PIXMIX_NEG            0x00000002 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_PIXMIX_ON             0x00000003 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE                    8:8 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE_NOTSEL      0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE_SEL         0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE                   12:12 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE_NOTSEL       0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE_SEL          0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE_15           0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE_16           0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE_24           0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE_30           0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_BLK_PEDSTL                 16:16 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_BLK_PEDSTL_OFF        0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_BLK_PEDSTL_ON         0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_TERMINATION                17:17 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_TERMINATION_37OHM     0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_TERMINATION_75OHM     0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_BPC                        20:20 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_BPC_6BITS             0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_BPC_8BITS             0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_LUT                        21:21 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_LUT_8BITS             0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_LUT_10BITS            0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_DAC_SLEEP                  24:24 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_DAC_SLEEP_DIS         0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_DAC_SLEEP_EN          0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_PALETTE_CLK                28:28 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_PALETTE_CLK_EN        0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_PALETTE_CLK_DIS       0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_PIPE                       29:29 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_PIPE_SHORT            0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_PIPE_LONG             0x00000001 /* RW--V */
#define NV_PRAMDAC_GENERAL_CONTROL_CUR_32B_ROP                30:30 /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_CUR_32B_ROP_DISABLE   0x00000000 /* RWI-V */
#define NV_PRAMDAC_GENERAL_CONTROL_CUR_32B_ROP_ENABLE    0x00000001 /* RW--V */
#define NV_PRAMDAC_PALETTE_RECOVERY                      0x00680604 /* R--4R */
#define NV_PRAMDAC_PALETTE_RECOVERY_ACTIVE_ADDRESS              7:0 /* R--UF */
#define NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER                10:8 /* R--VF */
#define NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER_RED      0x00000001 /* R---V */
#define NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER_GREEN    0x00000002 /* R---V */
#define NV_PRAMDAC_PALETTE_RECOVERY_RGB_POINTER_BLUE     0x00000004 /* R---V */
#define NV_PRAMDAC_PALETTE_RECOVERY_DAC_STATE                 13:12 /* R--VF */
#define NV_PRAMDAC_PALETTE_RECOVERY_DAC_STATE_WRITE      0x00000000 /* R---V */
#define NV_PRAMDAC_PALETTE_RECOVERY_DAC_STATE_READ       0x00000003 /* R---V */
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
#define NV_PRAMDAC_TEST_CONTROL_CRC_CHANNEL_RED          0x00000002 /* RW--V */
#define NV_PRAMDAC_TEST_CONTROL_CRC_CAPTURE                   10:10 /* RWIVF */
#define NV_PRAMDAC_TEST_CONTROL_CRC_CAPTURE_ALWAYS       0x00000000 /* RWI-V */
#define NV_PRAMDAC_TEST_CONTROL_CRC_CAPTURE_ONE          0x00000001 /* RW--V */
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
#define NV_PRAMDAC_CHECKSUM_STATUS                            24:24 /* R--VF */
#define NV_PRAMDAC_CHECKSUM_STATUS_CAPTURED              0x00000001 /* R---V */
#define NV_PRAMDAC_CHECKSUM_STATUS_WAITING               0x00000000 /* R---V */
#define NV_PRAMDAC_CHECKSUM_VALUE                              23:0 /* R--VF */
#define NV_PRAMDAC_TESTPOINT_DATA                        0x00680610 /* -W-4R */
#define NV_PRAMDAC_TESTPOINT_DATA_RED                           9:0 /* -W-VF */
#define NV_PRAMDAC_TESTPOINT_DATA_GREEN                       19:10 /* -W-VF */
#define NV_PRAMDAC_TESTPOINT_DATA_BLUE                        29:20 /* -W-VF */
#define NV_PRAMDAC_TESTPOINT_DATA_BLACK                       30:30 /* -W-VF */
#define NV_PRAMDAC_TESTPOINT_DATA_NOTBLANK                    31:31 /* -W-VF */
#define NV_PRAMDAC_PALETTE_LUT_INDEX                     0x00680620 /* RW-4R */
#define NV_PRAMDAC_PALETTE_LUT_INDEX_ADDR                       7:0 /* RW-VF */
#define NV_PRAMDAC_PALETTE_LUT_INDEX_ADDRINC                  12:12 /* RWIVF */
#define NV_PRAMDAC_PALETTE_LUT_INDEX_ADDRINC_ENABLE      0x00000000 /* RWI-V */
#define NV_PRAMDAC_PALETTE_LUT_INDEX_ADDRINC_DISABLE     0x00000001 /* RW--V */
#define NV_PRAMDAC_PALETTE_LUT_DATA                      0x00680624 /* RW-4R */
#define NV_PRAMDAC_PALETTE_LUT_DATA_BLUE                        9:0 /* RW-VF */
#define NV_PRAMDAC_PALETTE_LUT_DATA_GREEN                     19:10 /* RW-VF */
#define NV_PRAMDAC_PALETTE_LUT_DATA_RED                       29:20 /* RW-VF */
#define NV_PRAMDAC_TV_SETUP                              0x00680700 /* RW-4R */
#define NV_PRAMDAC_TV_SETUP_DEV_TYPE                            1:0 /* RWIVF */
#define NV_PRAMDAC_TV_SETUP_DEV_TYPE_SLAVE               0x00000000 /* RWI-V */
#define NV_PRAMDAC_TV_SETUP_DEV_TYPE_MASTER              0x00000001 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_DEV_TYPE_SLAVE_ALT           0x00000002 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_DEV_TYPE_MASTER_ALT          0x00000003 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_VS_PIXFMT                           6:4 /* RWIVF */
#define NV_PRAMDAC_TV_SETUP_VS_PIXFMT_555                0x00000000 /* RWI-V */
#define NV_PRAMDAC_TV_SETUP_VS_PIXFMT_565                0x00000001 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_VS_PIXFMT_888                0x00000002 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_VS_PIXFMT_101010             0x00000003 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_VS_PIXFMT_YUV                0x00000004 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_DATA_SRC                            9:8 /* RWIVF */
#define NV_PRAMDAC_TV_SETUP_DATA_SRC_COMP                0x00000000 /* RWI-V */
#define NV_PRAMDAC_TV_SETUP_DATA_SRC_SCALER              0x00000001 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_DATA_SRC_VIP                 0x00000002 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_DATA_SRC_NONE                0x00000003 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_COMP_SRC                          12:12 /* RWIVF */
#define NV_PRAMDAC_TV_SETUP_COMP_SRC_SCALER              0x00000000 /* RWI-V */
#define NV_PRAMDAC_TV_SETUP_COMP_SRC_NO_SCALER           0x00000001 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_SYNC_POL                          17:16 /* RWIVF */
#define NV_PRAMDAC_TV_SETUP_SYNC_POL_NEG_NONE            0x00000000 /* RWI-V */
#define NV_PRAMDAC_TV_SETUP_SYNC_POL_NEG_HSYNC           0x00000001 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_SYNC_POL_NEG_VSYNC           0x00000002 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_SYNC_POL_NEG_BOTH            0x00000003 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_VIP_VSYNC                         20:20 /* RWIVF */
#define NV_PRAMDAC_TV_SETUP_VIP_VSYNC_LEAD               0x00000000 /* RWI-V */
#define NV_PRAMDAC_TV_SETUP_VIP_VSYNC_TRAIL              0x00000001 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_VIP_DATAPOS                       24:24 /* RWIVF */
#define NV_PRAMDAC_TV_SETUP_VIP_DATAPOS_7_0              0x00000000 /* RWI-V */
#define NV_PRAMDAC_TV_SETUP_VIP_DATAPOS_11_4             0x00000001 /* RW--V */
#define NV_PRAMDAC_TV_SETUP_VIP_FIELD                         28:28 /* RWIVF */
#define NV_PRAMDAC_TV_SETUP_VIP_FIELD_0                  0x00000000 /* RWI-V */
#define NV_PRAMDAC_TV_SETUP_VIP_FIELD_1                  0x00000001 /* RW--V */
#define NV_PRAMDAC_TV_VBLANK_START                       0x00680704 /* RW-4R */
#define NV_PRAMDAC_TV_VBLANK_START_VAL                         10:0 /* RWIVF */
#define NV_PRAMDAC_TV_VBLANK_END                         0x00680708 /* RW-4R */
#define NV_PRAMDAC_TV_VBLANK_END_VAL                           10:0 /* RWIVF */
#define NV_PRAMDAC_TV_HBLANK_START                       0x0068070C /* RW-4R */
#define NV_PRAMDAC_TV_HBLANK_START_VAL                         10:0 /* RWIVF */
#define NV_PRAMDAC_TV_HBLANK_END                         0x00680710 /* RW-4R */
#define NV_PRAMDAC_TV_HBLANK_END_VAL                           10:0 /* RWIVF */
#define NV_PRAMDAC_BLANK_COLOR                           0x00680714 /* RW-4R */
#define NV_PRAMDAC_BLANK_COLOR_VAL                             23:0 /* RWIVF */
#define NV_PRAMDAC_TV_CHECKSUM                           0x00680718 /* RW-4R */
#define NV_PRAMDAC_TV_CHECKSUM_VAL                             23:0 /* R--VF */
#define NV_PRAMDAC_TV_CHECKSUM_STATUS                         24:24 /* R--VF */
#define NV_PRAMDAC_TV_CHECKSUM_STATUS_CAPTURED           0x00000001 /* R---V */
#define NV_PRAMDAC_TV_CHECKSUM_STATUS_WAITING            0x00000000 /* R---V */
#define NV_PRAMDAC_TV_VSYNC                                   28:28 /* R--VF */
#define NV_PRAMDAC_TV_VSYNC_ACTIVE                       0x00000000 /* R---V */
#define NV_PRAMDAC_TV_VSYNC_LOW                          0x00000000 /* R---V */
#define NV_PRAMDAC_TV_VSYNC_INACTIVE                     0x00000001 /* R---V */
#define NV_PRAMDAC_TV_VSYNC_HIGH                         0x00000001 /* R---V */
#define NV_PRAMDAC_TV_TEST_CONTROL                       0x0068071c /* RW-4R */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_RESET                    0:0 /* RWIVF */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_RESET_DEASSERTED  0x00000000 /* RWI-V */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_RESET_ASSERTED    0x00000001 /* RW--V */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_ENABLE                   4:4 /* RWIVF */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_ENABLE_DEASSERTED 0x00000000 /* RWI-V */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_ENABLE_ASSERTED   0x00000001 /* RW--V */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_CHANNEL                  9:8 /* RWIVF */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_CHANNEL_7_0       0x00000000 /* RWI-V */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_CHANNEL_15_8      0x00000001 /* RW--V */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_CHANNEL_23_16     0x00000002 /* RW--V */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_CAPTURE                10:10 /* RWIVF */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_CAPTURE_ALWAYS    0x00000000 /* RWI-V */
#define NV_PRAMDAC_TV_TEST_CONTROL_CRC_CAPTURE_ONE       0x00000001 /* RW--V */
#define NV_PRAMDAC_TV_VTOTAL                             0x00680720 /* RW-4R */
#define NV_PRAMDAC_TV_VTOTAL_VAL                               10:0 /* RWIVF */
#define NV_PRAMDAC_TV_VSYNC_START                        0x00680724 /* RW-4R */
#define NV_PRAMDAC_TV_VSYNC_START_VAL                          10:0 /* RWIVF */
#define NV_PRAMDAC_TV_VSYNC_END                          0x00680728 /* RW-4R */
#define NV_PRAMDAC_TV_VSYNC_END_VAL                            10:0 /* RWIVF */
#define NV_PRAMDAC_TV_HTOTAL                             0x0068072C /* RW-4R */
#define NV_PRAMDAC_TV_HTOTAL_VAL                               10:0 /* RWIVF */
#define NV_PRAMDAC_TV_HSYNC_START                        0x00680730 /* RW-4R */
#define NV_PRAMDAC_TV_HSYNC_START_VAL                          10:0 /* RWIVF */
#define NV_PRAMDAC_TV_HSYNC_END                          0x00680734 /* RW-4R */
#define NV_PRAMDAC_TV_HSYNC_END_VAL                            10:0 /* RWIVF */
#define NV_PRAMDAC_FP_VDISPLAY_END                       0x00680800 /* RW-4R */
#define NV_PRAMDAC_FP_VDISPLAY_END_VAL                         15:0 /* RWIVF */
#define NV_PRAMDAC_FP_VTOTAL                             0x00680804 /* RW-4R */
#define NV_PRAMDAC_FP_VTOTAL_VAL                               15:0 /* RWIVF */
#define NV_PRAMDAC_FP_VCRTC                              0x00680808 /* RW-4R */
#define NV_PRAMDAC_FP_VCRTC_VAL                                15:0 /* RWIVF */
#define NV_PRAMDAC_FP_VSYNC_START                        0x0068080c /* RW-4R */
#define NV_PRAMDAC_FP_VSYNC_START_VAL                          15:0 /* RWIVF */
#define NV_PRAMDAC_FP_VSYNC_END                          0x00680810 /* RW-4R */
#define NV_PRAMDAC_FP_VSYNC_END_VAL                            15:0 /* RWIVF */
#define NV_PRAMDAC_FP_VVALID_START                       0x00680814 /* RW-4R */
#define NV_PRAMDAC_FP_VVALID_START_VAL                         15:0 /* RWIVF */
#define NV_PRAMDAC_FP_VVALID_END                         0x00680818 /* RW-4R */
#define NV_PRAMDAC_FP_VVALID_END_VAL                           15:0 /* RWIVF */
#define NV_PRAMDAC_FP_HDISPLAY_END                       0x00680820 /* RW-4R */
#define NV_PRAMDAC_FP_HDISPLAY_END_VAL                         15:0 /* RWIVF */
#define NV_PRAMDAC_FP_HTOTAL                             0x00680824 /* RW-4R */
#define NV_PRAMDAC_FP_HTOTAL_VAL                               15:0 /* RWIVF */
#define NV_PRAMDAC_FP_HCRTC                              0x00680828 /* RW-4R */
#define NV_PRAMDAC_FP_HCRTC_VAL                                15:0 /* RWIVF */
#define NV_PRAMDAC_FP_HSYNC_START                        0x0068082c /* RW-4R */
#define NV_PRAMDAC_FP_HSYNC_START_VAL                          15:0 /* RWIVF */
#define NV_PRAMDAC_FP_HSYNC_END                          0x00680830 /* RW-4R */
#define NV_PRAMDAC_FP_HSYNC_END_VAL                            15:0 /* RWIVF */
#define NV_PRAMDAC_FP_HVALID_START                       0x00680834 /* RW-4R */
#define NV_PRAMDAC_FP_HVALID_START_VAL                         15:0 /* RWIVF */
#define NV_PRAMDAC_FP_HVALID_END                         0x00680838 /* RW-4R */
#define NV_PRAMDAC_FP_HVALID_END_VAL                           15:0 /* RWIVF */
#define NV_PRAMDAC_FP_CHECKSUM                           0x00680840 /* RW-4R */
#define NV_PRAMDAC_FP_CHECKSUM_VAL                             23:0 /* R--VF */
#define NV_PRAMDAC_FP_CHECKSUM_STATUS                         24:24 /* R--VF */
#define NV_PRAMDAC_FP_CHECKSUM_STATUS_CAPTURED           0x00000001 /* R---V */
#define NV_PRAMDAC_FP_CHECKSUM_STATUS_WAITING            0x00000000 /* R---V */
#define NV_PRAMDAC_FP_VSYNC                                   28:28 /* R--VF */
#define NV_PRAMDAC_FP_VSYNC_LOW                          0x00000000 /* R---V */
#define NV_PRAMDAC_FP_VSYNC_HIGH                         0x00000001 /* R---V */
#define NV_PRAMDAC_FP_TEST_CONTROL                       0x00680844 /* RW-4R */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_RESET                    0:0 /* RWIVF */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_RESET_DEASSERTED  0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_RESET_ASSERTED    0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_ENABLE                   4:4 /* RWIVF */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_ENABLE_DEASSERTED 0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_ENABLE_ASSERTED   0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_CHANNEL                  9:8 /* RWIVF */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_CHANNEL_7_0       0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_CHANNEL_15_8      0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_CHANNEL_23_16     0x00000002 /* RW--V */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_CAPTURE                10:10 /* RWIVF */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_CAPTURE_ALWAYS    0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_TEST_CONTROL_CRC_CAPTURE_ONE       0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_TEST_CONTROL_TMDS                       16:16 /* R---F */
#define NV_PRAMDAC_FP_TEST_CONTROL_TMDS_INTERNAL         0x00000000 /* R---V */
#define NV_PRAMDAC_FP_TEST_CONTROL_TMDS_EXTERNAL         0x00000001 /* R---V */
#define NV_PRAMDAC_FP_TG_CONTROL                         0x00680848 /* RW-4R */
#define NV_PRAMDAC_FP_TG_CONTROL_VSYNC                          1:0 /* RWIVF */
#define NV_PRAMDAC_FP_TG_CONTROL_VSYNC_NEG               0x00000000 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_VSYNC_POS               0x00000001 /* RWI-V */
#define NV_PRAMDAC_FP_TG_CONTROL_VSYNC_DISABLE           0x00000002 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_VSYNC_RSVD              0x00000003 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_HSYNC                          5:4 /* RWIVF */
#define NV_PRAMDAC_FP_TG_CONTROL_HSYNC_NEG               0x00000000 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_HSYNC_POS               0x00000001 /* RWI-V */
#define NV_PRAMDAC_FP_TG_CONTROL_HSYNC_DISABLE           0x00000002 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_HSYNC_RSVD              0x00000003 /* RWI-V */
#define NV_PRAMDAC_FP_TG_CONTROL_MODE                           9:8 /* RWIVF */
#define NV_PRAMDAC_FP_TG_CONTROL_MODE_SCALE              0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_TG_CONTROL_MODE_CENTER             0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_MODE_NATIVE             0x00000002 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_CENTER                       13:12 /* RWIVF */
#define NV_PRAMDAC_FP_TG_CONTROL_CENTER_NONE             0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_TG_CONTROL_CENTER_HORIZ            0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_CENTER_VERT             0x00000002 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_CENTER_BOTH             0x00000003 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_NATIVE                       17:16 /* RWIVF */
#define NV_PRAMDAC_FP_TG_CONTROL_NATIVE_NONE             0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_TG_CONTROL_NATIVE_HORIZ            0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_NATIVE_VERT             0x00000002 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_NATIVE_BOTH             0x00000003 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_READ                         20:20 /* RWIVF */
#define NV_PRAMDAC_FP_TG_CONTROL_READ_ACTUAL             0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_TG_CONTROL_READ_PROG               0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_WIDTH                        24:24 /* RWIVF */
#define NV_PRAMDAC_FP_TG_CONTROL_WIDTH_24                0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_TG_CONTROL_WIDTH_12                0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_DISPEN                       29:28 /* RWIVF */
#define NV_PRAMDAC_FP_TG_CONTROL_DISPEN_NEG              0x00000000 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_DISPEN_POS              0x00000001 /* RWI-V */
#define NV_PRAMDAC_FP_TG_CONTROL_DISPEN_DISABLE          0x00000002 /* RW--V */
#define NV_PRAMDAC_FP_TG_CONTROL_DISPEN_RSVD             0x00000003 /* RWI-V */
#define NV_PRAMDAC_FP_TG_CONTROL_FPCLK_RATIO                  31:31 /* RWIVF */
#define NV_PRAMDAC_FP_TG_CONTROL_FPCLK_RATIO_DB1         0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_TG_CONTROL_FPCLK_RATIO_DB2         0x00000001 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_0                            0x00680880 /* RW-4R */
#define NV_PRAMDAC_FP_DEBUG_0_XSCALE                            0:0 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_0_XSCALE_DISABLE             0x00000000 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_0_XSCALE_ENABLE              0x00000001 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_0_YSCALE                            4:4 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_0_YSCALE_DISABLE             0x00000000 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_0_YSCALE_ENABLE              0x00000001 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_0_XINTERP                           8:8 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_0_XINTERP_TRUNCATE           0x00000000 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_0_XINTERP_BILINEAR           0x00000001 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_0_YINTERP                         12:12 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_0_YINTERP_TRUNCATE           0x00000000 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_0_YINTERP_BILINEAR           0x00000001 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_0_VCNTR                           17:16 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_0_TEST_NONE                  0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_0_TEST_VCNTR                 0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_0_TEST_NEWPIX                0x00000002 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_0_TEST_BOTH                  0x00000003 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_0_XWEIGHT                         20:20 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_0_XWEIGHT_TRUNCATE           0x00000000 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_0_XWEIGHT_ROUND              0x00000001 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_0_YWEIGHT                         24:24 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_0_YWEIGHT_TRUNCATE           0x00000000 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_0_YWEIGHT_ROUND              0x00000001 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_0_PWRDOWN                         29:28 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_NONE               0x00000000 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_FPCLK              0x00000001 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_TMDS               0x00000002 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_BOTH               0x00000003 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_1                            0x00680884 /* RW-4R */
#define NV_PRAMDAC_FP_DEBUG_1_XSCALE_VALUE                     11:0 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_1_XSCALE_VALUE_ZERO          0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_1_XSCALE_TESTMODE                 12:12 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_1_XSCALE_TESTMODE_DISABLE    0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_1_XSCALE_TESTMODE_ENABLE     0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_1_YSCALE_VALUE                    27:16 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_1_YSCALE_VALUE_ZERO          0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_1_YSCALE_TESTMODE                 28:28 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_1_YSCALE_TESTMODE_DISABLE    0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_1_YSCALE_TESTMODE_ENABLE     0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_2                            0x00680888 /* RW-4R */
#define NV_PRAMDAC_FP_DEBUG_2_HTOTAL_VALUE                     11:0 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_2_HTOTAL_TESTMODE                 12:12 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_2_HTOTAL_TESTMODE_DISABLE    0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_2_HTOTAL_TESTMODE_ENABLE     0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_2_VTOTAL_VALUE                    27:16 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_2_VTOTAL_TESTMODE                 28:28 /* RWIVF */
#define NV_PRAMDAC_FP_DEBUG_2_VTOTAL_TESTMODE_DISABLE    0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_DEBUG_2_VTOTAL_TESTMODE_ENABLE     0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_DEBUG_3                            0x0068088c /* R--4R */
#define NV_PRAMDAC_FP_DEBUG_3_XSTEPSIZE                        12:0 /* R--VF */
#define NV_PRAMDAC_FP_DEBUG_3_YSTEPSIZE                       28:16 /* R--VF */
#define NV_PRAMDAC_FP_RAM_CONTROL                        0x006808A0 /* RW-4R */
#define NV_PRAMDAC_FP_RAM_CONTROL_ADDRESS                       8:0 /* RW-VF */
#define NV_PRAMDAC_FP_RAM_CONTROL_TESTMODE                    16:16 /* RWIVF */
#define NV_PRAMDAC_FP_RAM_CONTROL_TESTMODE_DISABLE       0x00000000 /* RWI-V */
#define NV_PRAMDAC_FP_RAM_CONTROL_TESTMODE_ENABLE        0x00000001 /* RW--V */
#define NV_PRAMDAC_FP_RAM_DATA_0                         0x006808A4 /* RWI4R */
#define NV_PRAMDAC_FP_RAM_DATA_0_VAL                           31:0 /* RW--F */
#define NV_PRAMDAC_FP_RAM_DATA_1                         0x006808A8 /* RWI4R */
#define NV_PRAMDAC_FP_RAM_DATA_1_VAL                           31:0 /* RW--F */
#define NV_PRAMDAC_FP_RAM_DATA_2                         0x006808AC /* RWI4R */
#define NV_PRAMDAC_FP_RAM_DATA_2_VAL                            7:0 /* RW--F */
#define NV_PRAMDAC_FP_TMDS_CONTROL                       0x006808B0 /* RW-4R */
#define NV_PRAMDAC_FP_TMDS_CONTROL_ADDRESS                      7:0 /* RW-VF */
#define NV_PRAMDAC_FP_TMDS_CONTROL_WRITE                      16:16 /* RWIVF */
#define NV_PRAMDAC_FP_TMDS_CONTROL_WRITE_DISABLE         0x00000001 /* RWI-V */
#define NV_PRAMDAC_FP_TMDS_CONTROL_WRITE_ENABLE          0x00000000 /* RW--V */
#define NV_PRAMDAC_FP_TMDS_DATA                          0x006808B4 /* RW-4R */
#define NV_PRAMDAC_FP_TMDS_DATA_DATA                            7:0 /* RW-VF */
#define NV_PRAMDAC_MCHIP_GENERAL_CONTROL                 0x00680900 /* RW-4R */
#define NV_PRAMDAC_MCHIP_GENERAL_CONTROL_DISP                   1:0 /* RW-VF */
#define NV_PRAMDAC_MCHIP_GENERAL_CONTROL_DISP_NEVER      0x00000000 /* RWI-V */
#define NV_PRAMDAC_MCHIP_GENERAL_CONTROL_DISP_ODD        0x00000001 /* RW--V */
#define NV_PRAMDAC_MCHIP_GENERAL_CONTROL_DISP_EVEN       0x00000002 /* RW--V */
#define NV_PRAMDAC_MCHIP_GENERAL_CONTROL_DISP_ALWAYS     0x00000003 /* RW--V */
#define NV_PRAMDAC_MCHIP_GENERAL_CONTROL_PROG                   4:4 /* RW-VF */
#define NV_PRAMDAC_MCHIP_GENERAL_CONTROL_PROG_DISABLE    0x00000000 /* RWI-V */
#define NV_PRAMDAC_MCHIP_GENERAL_CONTROL_PROG_ENABLE     0x00000001 /* RW--V */
#define NV_PRAMDAC_MCHIP_VDISPLAY_FIELD                  0x00680904 /* RW-4R */
#define NV_PRAMDAC_MCHIP_VDISPLAY_FIELD_START                  11:0 /* RW-VF */
#define NV_PRAMDAC_MCHIP_VDISPLAY_FIELD_END                   27:16 /* RW-VF */
#define NV_PRAMDAC_INDIR_TMDS_PLL0                             0x00 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_PLL0_SEL10UA                      0:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL0_SEL10UA_RESET                0x1 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL0_SEL50UA                      1:1 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL0_SEL50UA_RESET                0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL0_SEL100UA                     2:2 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL0_SEL100UA_RESET               0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL0_FILSEL                       5:3 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL0_FILSEL_RESET                 0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL0_CONF                         7:6 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL0_CONF_RESET                   0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL1                             0x01 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_PLL1_RSEL                         2:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL1_RSEL_RESET                   0x7 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL1_CSEL                         4:3 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL1_CSEL_RESET                   0x3 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL1_IOCTRL1                      5:5 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL1_IOCTRL1_RESET                0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL1_DIVBY10                      6:6 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL1_DIVBY10_RESET                0x1 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL1_IRSEL                        7:7 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL1_IRSEL_RESET                  0x1 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL2                             0x02 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_PLL2_DIVBY1                       0:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL2_DIVBY1_RESET                 0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL2_DIVBY7                       1:1 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL2_DIVBY7_RESET                 0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL2_ALTCLK                       2:2 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL2_ALTCLK_RESET                 0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_PLL2_AUX                          7:3 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_PLL2_AUX_RESET                    0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_IDLY                             0x03 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_IDLY_IDEL                         3:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_IDLY_IDEL_RESET                   0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_IDLY_CDEL                         7:4 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_IDLY_CDEL_RESET                   0x3 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_MODE                             0x04 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_MODE_LVDSMODE                     0:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_MODE_LVDSMODE_RESET               0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_MODE_DUALMODE                     1:1 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_MODE_DUALMODE_RESET               0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_MODE_DINSEL                       3:2 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_MODE_DINSEL_RESET                 0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_MODE_DINSEL_DINRISE               0x0 /* RW--V */
#define NV_PRAMDAC_INDIR_TMDS_MODE_DINSEL_DINFALL               0x1 /* RW--V */
#define NV_PRAMDAC_INDIR_TMDS_MODE_DINSEL_DALTRISE              0x2 /* RW--V */
#define NV_PRAMDAC_INDIR_TMDS_MODE_DINSEL_DALTFALL              0x3 /* RW--V */
#define NV_PRAMDAC_INDIR_TMDS_MODE_LINKACT                      7:7 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_MODE_LINKACT_RESET                0x1 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_LVDS                             0x05 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODESWAPCTL                  0:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODESWAPCTL_RESET            0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODEHS                       1:1 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODEHS_RESET                 0x1 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODEVS                       2:2 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODEVS_RESET                 0x1 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODEDEN                      3:3 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODEDEN_RESET                0x1 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODEBALANCED                 4:4 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODEBALANCED_RESET           0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODE24B                      5:5 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODE24B_RESET                0x1 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODEUPPER                    6:6 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_LVDS_MODEUPPER_RESET              0x1 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_TRIG0                            0x06 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_TRIG0_VAL                         7:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_TRIG0_VAL_RESET                  0x00 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_TRIG1                            0x07 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_TRIG1_VAL                         7:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_TRIG1_VAL_RESET                  0x00 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_TRIG2                            0x08 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_TRIG2_VAL                         7:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_TRIG2_VAL_RESET                  0x00 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_VCRC0                            0x09 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_VCRC0_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_VCRC1                            0x0a /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_VCRC1_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_VCRC2                            0x0b /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_VCRC2_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_VCRC3                            0x0c /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_VCRC3_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_VCRC4                            0x0d /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_VCRC4_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_VCRC5                            0x0e /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_VCRC5_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_IDATA0                           0x0f /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_IDATA0_VAL                        7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_IDATA1                           0x10 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_IDATA1_VAL                        7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_IDATA2                           0x11 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_IDATA2_VAL                        7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_IDATA3                           0x12 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_IDATA3_VAL                        7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_EDATA0                           0x13 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_EDATA0_VAL                        7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_EDATA1                           0x14 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_EDATA1_VAL                        7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_EDATA2                           0x15 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_EDATA2_VAL                        7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_EDATA3                           0x16 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_EDATA3_VAL                        7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_EDATA4                           0x17 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_EDATA4_VAL                        7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_CNTL0                            0x18 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_CNTL0_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_CNTH0                            0x19 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_CNTH0_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_CNTL1                            0x1a /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_CNTL1_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_CNTH1                            0x1b /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_CNTH1_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_CNTL2                            0x1c /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_CNTL2_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_CNTH2                            0x1d /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_CNTH2_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_DISPAR0                          0x1e /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_DISPAR0_THISPAR                   3:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_DISPAR0_RUNPAR                    7:4 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_DISPAR1                          0x1f /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_DISPAR1_THISPAR                   3:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_DISPAR1_RUNPAR                    7:4 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_DISPAR2                          0x20 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_DISPAR2_THISPAR                   3:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_DISPAR2_RUNPAR                    7:4 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_DISPAR3                          0x21 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_DISPAR3_THISPAR                   3:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_DISPAR3_RUNPAR                    7:4 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_DISPARCK                         0x22 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_DISPARCK_THISPAR                  3:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_DISPARCK_RUNPAR                   7:4 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_CCRC0                            0x23 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_CCRC0_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_CCRC1                            0x24 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_CCRC1_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_CCRC2                            0x25 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_CCRC2_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_CCRC3                            0x26 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_CCRC3_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_CCRC4                            0x27 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_CCRC4_VAL                         7:0 /* R--VF */
#define NV_PRAMDAC_INDIR_TMDS_CCRC5                            0x28 /* R--1R */
#define NV_PRAMDAC_INDIR_TMDS_CCRC5_VAL                         7:0 /* R--VF */
 // nv20-specific
#define NV_PRAMDAC_INDIR_TMDS_ROTCK                            0x29 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_ROTCK_ROTVAL                      3:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_ROTCK_RSEL_RESET                  0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL0                          0x30 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL0_ICHPMP                    3:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL0_ICHPMP_RESET              0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL0_FILSEL                    6:4 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL0_FILSEL_RESET              0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL0_ICLKSEL                   7:7 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL0_ICLKSEL_RESET             0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL1                          0x31 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL1_CSEL                      1:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL1_CSEL_RESET                0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL1_RSEL                      5:2 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL1_RSEL_RESET                0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL1_FBACK_SEL                 6:6 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL1_FBACK_SEL_RESET           0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL1_SEL1UA                    7:7 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL1_SEL1UA_RESET              0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL2                          0x32 /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL2_AUX                       7:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_FE_PLL2_AUX_RESET                 0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_CTL                        0x3a /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_CTL_TEST_DATA               0:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_CTL_TEST_DATA_DISABLE       0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_CTL_TEST_DATA_ENABLE        0x1 /* RW--V */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_CTL_SYNC_LOAD_EN            7:7 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_CTL_SYNC_LOAD_EN_OFF        0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_CTL_SYNC_LOAD_EN_ON         0x1 /* RW--V */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA0                      0x3b /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA0_VAL                   7:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA0_VAL_RESET             0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA1                      0x3c /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA1_VAL                   7:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA1_VAL_RESET             0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA2                      0x3d /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA2_VAL                   7:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA2_VAL_RESET             0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA3                      0x3e /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA3_VAL                   7:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA3_VAL_RESET             0x0 /* RWI-V */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA4                      0x3f /* RW-1R */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA4_VAL                   7:0 /* RWIVF */
#define NV_PRAMDAC_INDIR_TMDS_DEBUG_DATA4_VAL_RESET             0x0 /* RWI-V */
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
#define NV_USER_DAC_READ_MODE_ADDRESS_RW_STATE_READ      0x00000003 /* R---V */
#define NV_USER_DAC_WRITE_MODE_ADDRESS                   0x006813C8 /* RW-1R */
#define NV_USER_DAC_WRITE_MODE_ADDRESS_VALUE                    7:0 /* RW-VF */
#define NV_USER_DAC_PALETTE_DATA                         0x006813C9 /* RW-1R */
#define NV_USER_DAC_PALETTE_DATA_VALUE                          7:0 /* RW-VF */
/* dev_dac.ref */
#define NV_PDAC                               0x00680FFF:0x00680000 /* RW--D */
/* dev_dac.ref */
#define NV_PRMDIO                             0x00681FFF:0x00681000 /* RW--D */
/* dev_master.ref */
#define NV_PMC                                0x00000FFF:0x00000000 /* RW--D */
#define NV_PMC_BOOT_0                                    0x00000000 /* R--4R */
#define NV_PMC_BOOT_0_ID                                       31:0 /* R--VF */
#define NV_PMC_BOOT_0_ID_NV01_A                          0x00010100 /* ----V */
#define NV_PMC_BOOT_0_ID_NV01_B                          0x00010101 /* ----V */
#define NV_PMC_BOOT_0_ID_NV01_B02                        0x00010102 /* ----V */
#define NV_PMC_BOOT_0_ID_NV01_B03                        0x00010103 /* ----V */
#define NV_PMC_BOOT_0_ID_NV01_C01                        0x00010104 /* ----V */
#define NV_PMC_BOOT_0_ID_NV02_A01                        0x10020400 /* ----V */
#define NV_PMC_BOOT_0_ID_NV03_A01                        0x00030100 /* ----V */
#define NV_PMC_BOOT_0_ID_NV03_B01                        0x00030110 /* ----V */
#define NV_PMC_BOOT_0_ID_NV03T_A01                       0x20030120 /* ----V */
#define NV_PMC_BOOT_0_ID_NV03T_A02                       0x20030121 /* ----V */
#define NV_PMC_BOOT_0_ID_NV03T_A03_A04                   0x20030122 /* ----V */
#define NV_PMC_BOOT_0_ID_NV04_A01_A02_A03                0x20004000 /* ----V */
#define NV_PMC_BOOT_0_ID_NV04_A04                        0x20034001 /* ----V */
#define NV_PMC_BOOT_0_ID_NV04_A05                        0x20044001 /* ----V */
#define NV_PMC_BOOT_0_ID_NV05_NV06_A01                   0x20104000 /* ----V */
#define NV_PMC_BOOT_0_ID_NV05_NV06_A02                   0x20114000 /* ----V */
#define NV_PMC_BOOT_0_ID_NV05_NV06_A03                   0x20124000 /* ----V */
#define NV_PMC_BOOT_0_ID_NV05_NV06_B01                   0x20204000 /* ----V */
#define NV_PMC_BOOT_0_ID_NV05_NV06_B02                   0x20214000 /* ----V */
#define NV_PMC_BOOT_0_ID_NV05_NV06_B03                   0x20224000 /* ----V */
#define NV_PMC_BOOT_0_ID_NV0A_A01                        0x20204000 /* ----V */
#define NV_PMC_BOOT_0_ID_NV0A_A02                        0x20214000 /* ----V */
#define NV_PMC_BOOT_0_ID_NV0A_B01                        0x20224000 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_A1_DEVID0                  0x010000A1 /* C---V */
#define NV_PMC_BOOT_0_ID_NV10_A1_DEVID1                  0x010100A1 /* C---V */
#define NV_PMC_BOOT_0_ID_NV10_A1_DEVID2                  0x010200A1 /* C---V */
#define NV_PMC_BOOT_0_ID_NV10_A1_DEVID3                  0x010300A1 /* C---V */
#define NV_PMC_BOOT_0_ID_NV10_A2_DEVID0                  0x010000A2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_A2_DEVID1                  0x010100A2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_A2_DEVID2                  0x010200A2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_A2_DEVID3                  0x010300A2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_A3_DEVID0                  0x010000A3 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_A3_DEVID1                  0x010100A3 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_A3_DEVID2                  0x010200A3 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_A3_DEVID3                  0x010300A3 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_B1_DEVID0                  0x010000B1 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_B1_DEVID1                  0x010100B1 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_B1_DEVID2                  0x010200B1 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_B1_DEVID3                  0x010300B1 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_B2_DEVID0                  0x010000B2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_B2_DEVID1                  0x010100B2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_B2_DEVID2                  0x010200B2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV10_B2_DEVID3                  0x010300B2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV15_A1_DEVID0                  0x015000A1 /* C---V */
#define NV_PMC_BOOT_0_ID_NV15_A1_DEVID1                  0x015100A1 /* C---V */
#define NV_PMC_BOOT_0_ID_NV15_A1_DEVID2                  0x015200A1 /* C---V */
#define NV_PMC_BOOT_0_ID_NV15_A1_DEVID3                  0x015300A1 /* C---V */
#define NV_PMC_BOOT_0_ID_NV15_A2_DEVID0                  0x015000A2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV15_A2_DEVID1                  0x015100A2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV15_A2_DEVID2                  0x015200A2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV15_A2_DEVID3                  0x015300A2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV15_B1_DEVID0                  0x015000B1 /* ----V */
#define NV_PMC_BOOT_0_ID_NV15_B1_DEVID1                  0x015100B1 /* ----V */
#define NV_PMC_BOOT_0_ID_NV15_B1_DEVID2                  0x015200B1 /* ----V */
#define NV_PMC_BOOT_0_ID_NV15_B1_DEVID3                  0x015300B1 /* ----V */
#define NV_PMC_BOOT_0_ID_NV15_B2_DEVID0                  0x015000B2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV15_B2_DEVID1                  0x015100B2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV15_B2_DEVID2                  0x015200B2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV15_B2_DEVID3                  0x015300B2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV20_A1_DEVID0                  0x020000A1 /* C---V */
#define NV_PMC_BOOT_0_ID_NV20_A1_DEVID1                  0x020100A1 /* C---V */
#define NV_PMC_BOOT_0_ID_NV20_A1_DEVID2                  0x020200A1 /* C---V */
#define NV_PMC_BOOT_0_ID_NV20_A1_DEVID3                  0x020300A1 /* C---V */
#define NV_PMC_BOOT_0_ID_NV20_A2_DEVID0                  0x020000A2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV20_A2_DEVID1                  0x020100A2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV20_A2_DEVID2                  0x020200A2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV20_A2_DEVID3                  0x020300A2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV20_B1_DEVID0                  0x020000B1 /* ----V */
#define NV_PMC_BOOT_0_ID_NV20_B1_DEVID1                  0x020100B1 /* ----V */
#define NV_PMC_BOOT_0_ID_NV20_B1_DEVID2                  0x020200B1 /* ----V */
#define NV_PMC_BOOT_0_ID_NV20_B1_DEVID3                  0x020300B1 /* ----V */
#define NV_PMC_BOOT_0_ID_NV20_B2_DEVID0                  0x020000B2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV20_B2_DEVID1                  0x020100B2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV20_B2_DEVID2                  0x020200B2 /* ----V */
#define NV_PMC_BOOT_0_ID_NV20_B2_DEVID3                  0x020300B2 /* ----V */
#define NV_PMC_BOOT_0_MINOR_REVISION                            3:0 /* C--VF */
#define NV_PMC_BOOT_0_MINOR_REVISION_0                   0x00000000 /* C---V */
#define NV_PMC_BOOT_0_MAJOR_REVISION                            7:4 /* C--VF */
#define NV_PMC_BOOT_0_MAJOR_REVISION_A                   0x00000000 /* C---V */
#define NV_PMC_BOOT_0_MAJOR_REVISION_B                   0x00000001 /* ----V */
#define NV_PMC_BOOT_0_IMPLEMENTATION                           11:8 /* C--VF */
#define NV_PMC_BOOT_0_IMPLEMENTATION_NV4_0               0x00000000 /* C---V */
#define NV_PMC_BOOT_0_ARCHITECTURE                            15:12 /* C--VF */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV0                   0x00000000 /* ----V */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV1                   0x00000001 /* ----V */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV2                   0x00000002 /* ----V */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV3                   0x00000003 /* ----V */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV4                   0x00000004 /* ----V */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV10                  0x00000010 /* ----V */
#define NV_PMC_BOOT_0_ARCHITECTURE_NV20                  0x00000020 /* C---V */
#define NV_PMC_BOOT_0_FIB_REVISION                            19:16 /* C--VF */
#define NV_PMC_BOOT_0_FIB_REVISION_0                     0x00000000 /* C---V */
#define NV_PMC_BOOT_0_MASK_REVISION                           23:20 /* C--VF */
#define NV_PMC_BOOT_0_MASK_REVISION_A                    0x00000000 /* C---V */
#define NV_PMC_BOOT_0_MASK_REVISION_B                    0x00000001 /* ----V */
#define NV_PMC_BOOT_0_MASK_REVISION_C                    0x00000002 /* ----V */
#define NV_PMC_BOOT_0_MANUFACTURER                            27:24 /* C--UF */
#define NV_PMC_BOOT_0_MANUFACTURER_NVIDIA                0x00000000 /* C---V */
#define NV_PMC_BOOT_0_FOUNDRY                                 31:28 /* C--VF */
#define NV_PMC_BOOT_0_FOUNDRY_SGS                        0x00000000 /* ----V */
#define NV_PMC_BOOT_0_FOUNDRY_HELIOS                     0x00000001 /* ----V */
#define NV_PMC_BOOT_0_FOUNDRY_TSMC                       0x00000002 /* C---V */
#define NV_PMC_BOOT_1                                    0x00000004 /* R--4R */
#define NV_PMC_BOOT_1_ENDIAN00                                  0:0 /* R--VF */
#define NV_PMC_BOOT_1_ENDIAN00_LITTLE                    0x00000000 /* R-I-V */
#define NV_PMC_BOOT_1_ENDIAN00_BIG                       0x00000001 /* R---V */
#define NV_PMC_BOOT_1_ENDIAN24                                24:24 /* RW-VF */
#define NV_PMC_BOOT_1_ENDIAN24_LITTLE                    0x00000000 /* RWI-V */
#define NV_PMC_BOOT_1_ENDIAN24_BIG                       0x00000001 /* RW--V */
#define NV_PMC_INTR_0                                    0x00000100 /* RW-4R */
#define NV_PMC_INTR_0_MD                                        0:0 /* R--VF */
#define NV_PMC_INTR_0_MD_NOT_PENDING                     0x00000000 /* R---V */
#define NV_PMC_INTR_0_MD_PENDING                         0x00000001 /* R---V */
#define NV_PMC_INTR_0_PMEDIA                                    4:4 /* R--VF */
#define NV_PMC_INTR_0_PMEDIA_NOT_PENDING                 0x00000000 /* R---V */
#define NV_PMC_INTR_0_PMEDIA_PENDING                     0x00000001 /* R---V */
#define NV_PMC_INTR_0_PFIFO                                     8:8 /* R--VF */
#define NV_PMC_INTR_0_PFIFO_NOT_PENDING                  0x00000000 /* R---V */
#define NV_PMC_INTR_0_PFIFO_PENDING                      0x00000001 /* R---V */
#define NV_PMC_INTR_0_REMAPPER                                  9:9 /* R--VF */
#define NV_PMC_INTR_0_REMAPPER_NOT_PENDING               0x00000000 /* R---V */
#define NV_PMC_INTR_0_REMAPPER_PENDING                   0x00000001 /* R---V */
#define NV_PMC_INTR_0_PGRAPH                                  12:12 /* R--VF */
#define NV_PMC_INTR_0_PGRAPH_NOT_PENDING                 0x00000000 /* R---V */
#define NV_PMC_INTR_0_PGRAPH_PENDING                     0x00000001 /* R---V */
#define NV_PMC_INTR_0_PVIDEO                                  16:16 /* R--VF */
#define NV_PMC_INTR_0_PVIDEO_NOT_PENDING                 0x00000000 /* R---V */
#define NV_PMC_INTR_0_PVIDEO_PENDING                     0x00000001 /* R---V */
#define NV_PMC_INTR_0_PTIMER                                  20:20 /* R--VF */
#define NV_PMC_INTR_0_PTIMER_NOT_PENDING                 0x00000000 /* R---V */
#define NV_PMC_INTR_0_PTIMER_PENDING                     0x00000001 /* R---V */
#define NV_PMC_INTR_0_PCRTC                                   24:24 /* R--VF */
#define NV_PMC_INTR_0_PCRTC_NOT_PENDING                  0x00000000 /* R---V */
#define NV_PMC_INTR_0_PCRTC_PENDING                      0x00000001 /* R---V */
#define NV_PMC_INTR_0_PCRTC2                                  25:25 /* R--VF */
#define NV_PMC_INTR_0_PCRTC2_NOT_PENDING                 0x00000000 /* R---V */
#define NV_PMC_INTR_0_PCRTC2_PENDING                     0x00000001 /* R---V */
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
#define NV_PMC_ENABLE_BUF_RESET                                 0:0 /* RWIVF */
#define NV_PMC_ENABLE_BUF_RESET_DISABLE                  0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_BUF_RESET_ENABLE                   0x00000001 /* RW--V */
#define NV_PMC_ENABLE_MD_RESET                                  1:1 /* RWIVF */
#define NV_PMC_ENABLE_MD_RESET_DISABLE                   0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_MD_RESET_ENABLE                    0x00000001 /* RW--V */
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
#define NV_PMC_ENABLE_PCRTC2                                  25:25 /* RWIVF */
#define NV_PMC_ENABLE_PCRTC2_DISABLED                    0x00000000 /* RW--V */
#define NV_PMC_ENABLE_PCRTC2_ENABLED                     0x00000001 /* RWI-V */
#define NV_PMC_ENABLE_PVIDEO                                  28:28 /* RWIVF */
#define NV_PMC_ENABLE_PVIDEO_DISABLED                    0x00000000 /* RWI-V */
#define NV_PMC_ENABLE_PVIDEO_ENABLED                     0x00000001 /* RW--V */
#define NV_PMC_FRAME_PROTECT_MIN                         0x00000300 /* RW-4R */
#define NV_PMC_FRAME_PROTECT_MIN_VAL                           28:0 /* RWIVF */
#define NV_PMC_FRAME_PROTECT_MIN_VAL0                    0x00000000 /* RWI-V */
#define NV_PMC_FRAME_PROTECT_EN                               31:31 /* RWIVF */
#define NV_PMC_FRAME_PROTECT_DISABLED                    0x00000000 /* RWI-V */
#define NV_PMC_FRAME_PROTECT_ENABLED                     0x00000001 /* RWI-V */
#define NV_PMC_FRAME_PROTECT_MAX                         0x00000304 /* RW-4R */
#define NV_PMC_FRAME_PROTECT_MAX_VAL                           28:0 /* RWIVF */
#define NV_PMC_FRAME_PROTECT_MAX_VAL0                    0x00000000 /* RWI-V */
/* dev_bus.ref */
#define NV_PBUS                               0x00001FFF:0x00001000 /* RW--D */
#define NV_PBUS_DEBUG_1                                  0x00001084 /* RW-4R */
/* NV_PBUS_DEBUG_1_PCIM_THROTTLE is used in osapi.c for APM */
#define NV_PBUS_DEBUG_1_PCIM_THROTTLE                           0:0 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIM_THROTTLE_DISABLED           0x00000000 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIM_THROTTLE_ENABLED            0x00000001 /* RWI-V */
/*  NV_PBUS_DEBUG_1_PCIS_WRITE is tweaked by win9x osinit.c */
#define NV_PBUS_DEBUG_1_PCIS_WRITE                              5:5 /* RWIVF */
#define NV_PBUS_DEBUG_1_PCIS_WRITE_0_CYCLE               0x00000000 /* RW--V */
#define NV_PBUS_DEBUG_1_PCIS_WRITE_1_CYCLE               0x00000001 /* RWI-V */
/*  NV_PBUS_DEBUG_1_OPENGL is used in mcstate.c */
#define NV_PBUS_DEBUG_1_OPENGL                                16:16 /* R--VF */
#define NV_PBUS_DEBUG_1_OPENGL_OFF                       0x00000000 /* R---V */
#define NV_PBUS_DEBUG_1_OPENGL_ON                        0x00000001 /* R---V */
#define NV_PBUS_DEBUG_1_AGPFW_ADIS                            21:21 /* RWIVF */
#define NV_PBUS_DEBUG_1_AGPFW_ADIS_ENABLED               0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_1_AGPFW_ADIS_DISABLED              0x00000001 /* RW--V */
#define NV_PBUS_DEBUG_1_IDDQ                                  22:22
#define NV_PBUS_DEBUG_1_IDDQ_ZERO                        0x00000000
#define NV_PBUS_DEBUG_1_IDDQ_ONE                         0x00000001
#define NV_PBUS_DEBUG_1_AGPSTOPCLK                            24:24
#define NV_PBUS_DEBUG_1_AGPSTOPCLK_DISABLED              0x00000000
#define NV_PBUS_DEBUG_1_AGPSTOPCLK_ENABLED               0x00000001
#define NV_PBUS_DEBUG_1_PLL_STOPCLK                           27:27
#define NV_PBUS_DEBUG_1_PLL_STOPCLK_DISABLE              0x00000000
#define NV_PBUS_DEBUG_1_PLL_STOPCLK_ENABLE               0x00000001
#define NV_PBUS_DEBUG_1_DISP_MIRROR                           28:28 /* RWIVF */
#define NV_PBUS_DEBUG_1_DISP_MIRROR_DISABLE              0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_1_DISP_MIRROR_ENABLE               0x00000001 /* RW--V */
#define NV_PBUS_DEBUG_1_CORE_SLOWDWN                          30:29 /* RWIVF */
#define NV_PBUS_DEBUG_1_CORE_SLOWDWN_DISABLE             0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_1_CORE_SLOWDWN_ENABLE              0x00000001 /* RWI-V */
/*  NV_PBUS_DEBUG_2_AGP_VREF is used in nvagp.c */
#define NV_PBUS_DEBUG_2                                  0x00001088 /* RW-4R */
#define NV_PBUS_DEBUG_2_AGP_VREF                                0:0 /* RWIVF */
#define NV_PBUS_DEBUG_2_AGP_VREF_DISABLED                0x00000000 /* RWI-V */
#define NV_PBUS_DEBUG_2_AGP_VREF_ENABLED                 0x00000001 /* RW--V */
/* NV_PBUS_DEBUG_3_AGP_4X_NVCLK is used in osapi.c for APM */
#define NV_PBUS_DEBUG_3                                  0x0000108C /* RW-4R */
#define NV_PBUS_DEBUG_3_AGP_4X_NVCLK                            7:4 /* RWIVF */
#define NV_PBUS_PCI_NV_0                                 0x00001800 /* R--4R */
#define NV_PBUS_PCI_NV_0__ALIAS_1                NV_CONFIG_PCI_NV_0 /*       */
#define NV_PBUS_PCI_NV_0_VENDOR_ID                             15:0 /* C--UF */
#define NV_PBUS_PCI_NV_0_VENDOR_ID_NVIDIA_SGS            0x000012D2 /* ----V */
#define NV_PBUS_PCI_NV_0_VENDOR_ID_NVIDIA                0x000010DE /* C---V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC                       18:16 /* C--UF */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC_VGA              0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC_ALT1             0x00000001 /* C---V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC_ALT2             0x00000002 /* C---V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC_ALT3             0x00000003 /* C---V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC_LC0              0x00000004 /* C---V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC_LC1              0x00000005 /* C---V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC_LC2              0x00000006 /* C---V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_FUNC_LC3              0x00000007 /* C---V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_CHIP                       31:19 /* C--UF */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_CHIP_NV0              0x00000000 /* ----V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_CHIP_NV1              0x00000001 /* ----V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_CHIP_NV2              0x00000002 /* ----V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_CHIP_NV3              0x00000003 /* ----V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_CHIP_NV4              0x00000004 /* ----V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_CHIP_NV5              0x00000005 /* ----V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_CHIP_NV10             0x00000010 /* ----V */
#define NV_PBUS_PCI_NV_0_DEVICE_ID_CHIP_NV20             0x00000020 /* C---V */
#define NV_PBUS_PCI_NV_1                                 0x00001804 /* RW-4R */
#define NV_PBUS_PCI_NV_1__ALIAS_1                NV_CONFIG_PCI_NV_1 /*       */
#define NV_PBUS_PCI_NV_1_IO_SPACE                               0:0 /* RWIVF */
#define NV_PBUS_PCI_NV_1_IO_SPACE_DISABLED               0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_1_IO_SPACE_ENABLED                0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_1_MEMORY_SPACE                           1:1 /* RWIVF */
#define NV_PBUS_PCI_NV_1_MEMORY_SPACE_DISABLED           0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_1_MEMORY_SPACE_ENABLED            0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_1_BUS_MASTER                             2:2 /* RWIVF */
#define NV_PBUS_PCI_NV_1_BUS_MASTER_DISABLED             0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_1_BUS_MASTER_ENABLED              0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_1_WRITE_AND_INVAL                        4:4 /* C--VF */
#define NV_PBUS_PCI_NV_1_WRITE_AND_INVAL_DISABLED        0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_1_WRITE_AND_INVAL_ENABLED         0x00000001 /* ----V */
#define NV_PBUS_PCI_NV_1_PALETTE_SNOOP                          5:5 /* RWIVF */
#define NV_PBUS_PCI_NV_1_PALETTE_SNOOP_DISABLED          0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_1_PALETTE_SNOOP_ENABLED           0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_1_CAPLIST                              20:20 /* C--VF */
#define NV_PBUS_PCI_NV_1_CAPLIST_NOT_PRESENT             0x00000000 /* ----V */
#define NV_PBUS_PCI_NV_1_CAPLIST_PRESENT                 0x00000001 /* C---V */
#define NV_PBUS_PCI_NV_1_66MHZ                                21:21 /* C--VF */
#define NV_PBUS_PCI_NV_1_66MHZ_INCAPABLE                 0x00000000 /* ----V */
#define NV_PBUS_PCI_NV_1_66MHZ_CAPABLE                   0x00000001 /* C---V */
#define NV_PBUS_PCI_NV_1_FAST_BACK2BACK                       23:23 /* C--VF */
#define NV_PBUS_PCI_NV_1_FAST_BACK2BACK_INCAPABLE        0x00000000 /* ----V */
#define NV_PBUS_PCI_NV_1_FAST_BACK2BACK_CAPABLE          0x00000001 /* C---V */
#define NV_PBUS_PCI_NV_1_DEVSEL_TIMING                        26:25 /* C--VF */
#define NV_PBUS_PCI_NV_1_DEVSEL_TIMING_FAST              0x00000000 /* ----V */
#define NV_PBUS_PCI_NV_1_DEVSEL_TIMING_MEDIUM            0x00000001 /* C---V */
#define NV_PBUS_PCI_NV_1_DEVSEL_TIMING_SLOW              0x00000002 /* ----V */
#define NV_PBUS_PCI_NV_1_SIGNALED_TARGET                      27:27 /* RWIVF */
#define NV_PBUS_PCI_NV_1_SIGNALED_TARGET_NO_ABORT        0x00000000 /* R-I-V */
#define NV_PBUS_PCI_NV_1_SIGNALED_TARGET_ABORT           0x00000001 /* R---V */
#define NV_PBUS_PCI_NV_1_SIGNALED_TARGET_CLEAR           0x00000001 /* -W--V */
#define NV_PBUS_PCI_NV_1_RECEIVED_TARGET                      28:28 /* RWIVF */
#define NV_PBUS_PCI_NV_1_RECEIVED_TARGET_NO_ABORT        0x00000000 /* R-I-V */
#define NV_PBUS_PCI_NV_1_RECEIVED_TARGET_ABORT           0x00000001 /* R---V */
#define NV_PBUS_PCI_NV_1_RECEIVED_TARGET_CLEAR           0x00000001 /* -W--V */
#define NV_PBUS_PCI_NV_1_RECEIVED_MASTER                      29:29 /* RWIVF */
#define NV_PBUS_PCI_NV_1_RECEIVED_MASTER_NO_ABORT        0x00000000 /* R-I-V */
#define NV_PBUS_PCI_NV_1_RECEIVED_MASTER_ABORT           0x00000001 /* R---V */
#define NV_PBUS_PCI_NV_1_RECEIVED_MASTER_CLEAR           0x00000001 /* -W--V */
#define NV_PBUS_PCI_NV_2                                 0x00001808 /* R--4R */
#define NV_PBUS_PCI_NV_2__ALIAS_1                NV_CONFIG_PCI_NV_2 /*       */
#define NV_PBUS_PCI_NV_2_REVISION_ID                            7:0 /* C--UF */
#define NV_PBUS_PCI_NV_2_CLASS_CODE                            31:8 /* C--VF */
#define NV_PBUS_PCI_NV_2_CLASS_CODE_VGA                  0x00030000 /* C---V */
#define NV_PBUS_PCI_NV_2_CLASS_CODE_MULTIMEDIA           0x00048000 /* ----V */
#define NV_PBUS_PCI_NV_3                                 0x0000180C /* RW-4R */
#define NV_PBUS_PCI_NV_3__ALIAS_1                NV_CONFIG_PCI_NV_3 /*       */
#define NV_PBUS_PCI_NV_3_LATENCY_TIMER                        15:11 /* RWIUF */
#define NV_PBUS_PCI_NV_3_LATENCY_TIMER_0_CLOCKS          0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_3_LATENCY_TIMER_8_CLOCKS          0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_3_LATENCY_TIMER_240_CLOCKS        0x0000001E /* RW--V */
#define NV_PBUS_PCI_NV_3_LATENCY_TIMER_248_CLOCKS        0x0000001F /* RW--V */
#define NV_PBUS_PCI_NV_3_HEADER_TYPE                          23:16 /* C--VF */
#define NV_PBUS_PCI_NV_3_HEADER_TYPE_SINGLEFUNC          0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_3_HEADER_TYPE_MULTIFUNC           0x00000080 /* ----V */
#define NV_PBUS_PCI_NV_4                                 0x00001810 /* RW-4R */
#define NV_PBUS_PCI_NV_4__ALIAS_1                NV_CONFIG_PCI_NV_4 /*       */
#define NV_PBUS_PCI_NV_4_SPACE_TYPE                             0:0 /* C--VF */
#define NV_PBUS_PCI_NV_4_SPACE_TYPE_MEMORY               0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_4_SPACE_TYPE_IO                   0x00000001 /* ----V */
#define NV_PBUS_PCI_NV_4_ADDRESS_TYPE                           2:1 /* C--VF */
#define NV_PBUS_PCI_NV_4_ADDRESS_TYPE_32_BIT             0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_4_ADDRESS_TYPE_20_BIT             0x00000001 /* ----V */
#define NV_PBUS_PCI_NV_4_ADDRESS_TYPE_64_BIT             0x00000002 /* ----V */
#define NV_PBUS_PCI_NV_4_PREFETCHABLE                           3:3 /* C--VF */
#define NV_PBUS_PCI_NV_4_PREFETCHABLE_NOT                0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_4_PREFETCHABLE_MERGABLE           0x00000001 /* ----V */
#define NV_PBUS_PCI_NV_4_BASE_ADDRESS                         31:24 /* RWXUF */
#define NV_PBUS_PCI_NV_5                                 0x00001814 /* RW-4R */
#define NV_PBUS_PCI_NV_5__ALIAS_1                NV_CONFIG_PCI_NV_5 /*       */
#define NV_PBUS_PCI_NV_5_SPACE_TYPE                             0:0 /* C--VF */
#define NV_PBUS_PCI_NV_5_SPACE_TYPE_MEMORY               0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_5_SPACE_TYPE_IO                   0x00000001 /* ----V */
#define NV_PBUS_PCI_NV_5_ADDRESS_TYPE                           2:1 /* C--VF */
#define NV_PBUS_PCI_NV_5_ADDRESS_TYPE_32_BIT             0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_5_ADDRESS_TYPE_20_BIT             0x00000001 /* ----V */
#define NV_PBUS_PCI_NV_5_ADDRESS_TYPE_64_BIT             0x00000002 /* ----V */
#define NV_PBUS_PCI_NV_5_PREFETCHABLE                           3:3 /* C--VF */
#define NV_PBUS_PCI_NV_5_PREFETCHABLE_NOT                0x00000000 /* ----V */
#define NV_PBUS_PCI_NV_5_PREFETCHABLE_MERGABLE           0x00000001 /* C---V */
#define NV_PBUS_PCI_NV_6                                 0x00001818 /* RW-4R */
#define NV_PBUS_PCI_NV_6__ALIAS_1                NV_CONFIG_PCI_NV_6 /*       */
#define NV_PBUS_PCI_NV_6_SPACE_TYPE                             0:0 /* C--VF */
#define NV_PBUS_PCI_NV_6_SPACE_TYPE_MEMORY               0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_6_SPACE_TYPE_IO                   0x00000001 /* ----V */
#define NV_PBUS_PCI_NV_6_ADDRESS_TYPE                           2:1 /* C--VF */
#define NV_PBUS_PCI_NV_6_ADDRESS_TYPE_32_BIT             0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_6_ADDRESS_TYPE_20_BIT             0x00000001 /* ----V */
#define NV_PBUS_PCI_NV_6_ADDRESS_TYPE_64_BIT             0x00000002 /* ----V */
#define NV_PBUS_PCI_NV_6_PREFETCHABLE                           3:3 /* C--VF */
#define NV_PBUS_PCI_NV_6_PREFETCHABLE_NOT                0x00000000 /* ----V */
#define NV_PBUS_PCI_NV_6_PREFETCHABLE_MERGABLE           0x00000001 /* C---V */
#define NV_PBUS_PCI_NV_6_BASE_ADDRESS                         31:19 /* RWXUF */
#define NV_PBUS_PCI_NV_7(i)                      (0x0000181C+(i)*4) /* R--4A */
#define NV_PBUS_PCI_NV_7__SIZE_1                                  4 /*       */
#define NV_PBUS_PCI_NV_7__ALIAS_1                NV_CONFIG_PCI_NV_7 /*       */
#define NV_PBUS_PCI_NV_7_RESERVED                              31:0 /* C--VF */
#define NV_PBUS_PCI_NV_7_RESERVED_0                      0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_11                                0x0000182C /* R--4R */
#define NV_PBUS_PCI_NV_11__ALIAS_1              NV_CONFIG_PCI_NV_11 /*       */
#define NV_PBUS_PCI_NV_11_SUBSYSTEM_VENDOR_ID                  15:0 /* R--UF */
#define NV_PBUS_PCI_NV_11_SUBSYSTEM_VENDOR_ID_NONE       0x00000000 /* R---V */
#define NV_PBUS_PCI_NV_11_SUBSYSTEM_ID                        31:16 /* R--UF */
#define NV_PBUS_PCI_NV_11_SUBSYSTEM_ID_NONE              0x00000000 /* R---V */
#define NV_PBUS_PCI_NV_11_SUBSYSTEM_ID_TNT2PRO           0x0000001f
#define NV_PBUS_PCI_NV_12                                0x00001830 /* RW-4R */
#define NV_PBUS_PCI_NV_12__ALIAS_1              NV_CONFIG_PCI_NV_12 /*       */
#define NV_PBUS_PCI_NV_12_ROM_DECODE                            0:0 /* RWIVF */
#define NV_PBUS_PCI_NV_12_ROM_DECODE_DISABLED            0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_12_ROM_DECODE_ENABLED             0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_12_ROM_BASE                            31:16 /* RWXUF */
#define NV_PBUS_PCI_NV_13                                0x00001834 /* RW-4R */
#define NV_PBUS_PCI_NV_13__ALIAS_1              NV_CONFIG_PCI_NV_13 /*       */
#define NV_PBUS_PCI_NV_13_CAP_PTR                               7:0 /* C--VF */
#define NV_PBUS_PCI_NV_13_CAP_PTR_AGP                    0x00000044 /* ----V */
#define NV_PBUS_PCI_NV_13_CAP_PTR_POWER_MGMT             0x00000060 /* C---V */
#define NV_PBUS_PCI_NV_14                                0x00001838 /* R--4R */
#define NV_PBUS_PCI_NV_14__ALIAS_1              NV_CONFIG_PCI_NV_14 /*       */
#define NV_PBUS_PCI_NV_14_RESERVED                             31:0 /* C--VF */
#define NV_PBUS_PCI_NV_14_RESERVED_0                     0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_15                                0x0000183C /* RW-4R */
#define NV_PBUS_PCI_NV_15__ALIAS_1              NV_CONFIG_PCI_NV_15 /*       */
#define NV_PBUS_PCI_NV_15_INTR_LINE                             7:0 /* RWIVF */
#define NV_PBUS_PCI_NV_15_INTR_LINE_IRQ0                 0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_15_INTR_LINE_IRQ1                 0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_15_INTR_LINE_IRQ15                0x0000000F /* RW--V */
#define NV_PBUS_PCI_NV_15_INTR_LINE_UNKNOWN              0x000000FF /* RW--V */
#define NV_PBUS_PCI_NV_15_INTR_PIN                             15:8 /* C--VF */
#define NV_PBUS_PCI_NV_15_INTR_PIN_INTA                  0x00000001 /* C---V */
#define NV_PBUS_PCI_NV_15_MIN_GNT                             23:16 /* C--VF */
#define NV_PBUS_PCI_NV_15_MIN_GNT_NO_REQUIREMENTS        0x00000000 /* ----V */
#define NV_PBUS_PCI_NV_15_MIN_GNT_750NS                  0x00000003 /* ----V */
#define NV_PBUS_PCI_NV_15_MIN_GNT_1250NS                 0x00000005 /* C---V */
#define NV_PBUS_PCI_NV_15_MAX_LAT                             31:24 /* C--VF */
#define NV_PBUS_PCI_NV_15_MAX_LAT_NO_REQUIREMENTS        0x00000000 /* ----V */
#define NV_PBUS_PCI_NV_15_MAX_LAT_250NS                  0x00000001 /* C---V */
#define NV_PBUS_PCI_NV_16                                0x00001840 /* RW-4R */
#define NV_PBUS_PCI_NV_16__ALIAS_1              NV_CONFIG_PCI_NV_16 /*       */
#define NV_PBUS_PCI_NV_16_SUBSYSTEM_VENDOR_ID                  15:0 /* RW-VF */
#define NV_PBUS_PCI_NV_16_SUBSYSTEM_VENDOR_ID_NONE       0x00000000 /* R---V */
#define NV_PBUS_PCI_NV_16_SUBSYSTEM_ID                        31:16 /* RW-VF */
#define NV_PBUS_PCI_NV_16_SUBSYSTEM_ID_NONE              0x00000000 /* R---V */
#define NV_PBUS_PCI_NV_17                                0x00001844 /* RW-4R */
#define NV_PBUS_PCI_NV_17__ALIAS_1              NV_CONFIG_PCI_NV_17 /*       */
#define NV_PBUS_PCI_NV_17_AGP_REV_MAJOR                       23:20 /* C--VF */
#define NV_PBUS_PCI_NV_17_AGP_REV_MAJOR_2                0x00000002 /* C---V */
#define NV_PBUS_PCI_NV_17_AGP_REV_MINOR                       19:16 /* C--VF */
#define NV_PBUS_PCI_NV_17_AGP_REV_MINOR_0                0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_17_NEXT_PTR                             15:8 /* C--VF */
#define NV_PBUS_PCI_NV_17_NEXT_PTR_NULL                  0x00000000 /* C---V */
#define NV_PBUS_PCI_NV_17_CAP_ID                                7:0 /* C--VF */
#define NV_PBUS_PCI_NV_17_CAP_ID_AGP                     0x00000002 /* C---V */
#define NV_PBUS_PCI_NV_18                                0x00001848 /* RW-4R */
#define NV_PBUS_PCI_NV_18__ALIAS_1              NV_CONFIG_PCI_NV_18 /*       */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RQ                       31:24 /* C--VF */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RQ_16               0x0000000F /* C---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RQ_32               0x0000001F /* C---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_SBA                        9:9 /* R--VF */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_SBA_NONE            0x00000000 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_SBA_CAPABLE         0x00000001 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_FW                         4:4 /* R--VF */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_FW_NONE             0x00000000 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_FW_CAPABLE          0x00000001 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RATE                       2:0 /* R--VF */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RATE_1X             0x00000001 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RATE_2X             0x00000002 /* ----V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RATE_1X_AND_2X      0x00000003 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RATE_4X             0x00000004 /* R---V */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_RATE_1X_2X_4X       0x00000007 /* R---V */
#define NV_PBUS_PCI_NV_19                                0x0000184C /* RW-4R */
#define NV_PBUS_PCI_NV_19__ALIAS_1              NV_CONFIG_PCI_NV_19 /*       */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_RQ_DEPTH                28:24 /* RWIVF */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_RQ_DEPTH_0         0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_SBA_ENABLE                9:9 /* RWIVF */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_SBA_ENABLE_OFF     0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_SBA_ENABLE_ON      0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_AGP_ENABLE                8:8 /* RWIVF */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_AGP_ENABLE_OFF     0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_AGP_ENABLE_ON      0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_FW_ENABLE                 4:4 /* RWIVF */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_FW_ENABLE_OFF      0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_FW_ENABLE_ON       0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_DATA_RATE                 2:0 /* RWIVF */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_DATA_RATE_OFF      0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_DATA_RATE_1X       0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_DATA_RATE_2X       0x00000002 /* RW--V */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_DATA_RATE_4X       0x00000004 /* RW--V */
#define NV_PBUS_PCI_NV_20                                0x00001850 /* RW-4R */
#define NV_PBUS_PCI_NV_20__ALIAS_1              NV_CONFIG_PCI_NV_20 /*       */
#define NV_PBUS_PCI_NV_20_ROM_SHADOW                            0:0 /* RWIVF */
#define NV_PBUS_PCI_NV_20_ROM_SHADOW_DISABLED            0x00000000 /* RWI-V */
#define NV_PBUS_PCI_NV_20_ROM_SHADOW_ENABLED             0x00000001 /* RW--V */
#define NV_PBUS_PCI_NV_21                                0x00001854 /* RW-4R */
#define NV_PBUS_PCI_NV_21__ALIAS_1              NV_CONFIG_PCI_NV_21 /*       */
#define NV_PBUS_PCI_NV_21_VGA                                   0:0 /* RWIVF */
#define NV_PBUS_PCI_NV_21_VGA_DISABLED                   0x00000000 /* RW--V */
#define NV_PBUS_PCI_NV_21_VGA_ENABLED                    0x00000001 /* RWI-V */
#define NV_PBUS_PCI_NV_22                                0x00001858 /* RW-4R */
#define NV_PBUS_PCI_NV_22__ALIAS_1              NV_CONFIG_PCI_NV_22 /*       */
#define NV_PBUS_PCI_NV_22_SCRATCH                              23:0 /* RWIVF */
#define NV_PBUS_PCI_NV_22_SCRATCH_DEFAULT                0x0023D6CE /* RWI-V */
#define NV_PBUS_PCI_NV_23                                0x0000185C /* RW-4R */
#define NV_PBUS_PCI_NV_23__ALIAS_1              NV_CONFIG_PCI_NV_23 /*       */
#define NV_PBUS_PCI_NV_23_DT_TIMEOUT                            3:0 /* RWIVF */
#define NV_PBUS_PCI_NV_23_DT_TIMEOUT_16                  0x0000000F /* RWI-V */
#define NV_PBUS_PCI_NV_24                                0x00001860 /* RW-4R */
#define NV_PBUS_PCI_NV_24__ALIAS_1              NV_CONFIG_PCI_NV_24 /*       */
#define NV_PBUS_PCI_NV_24_PME_D3_COLD                         31:31 /* C--VF */
#define NV_PBUS_PCI_NV_24_PME_D3_COLD_SUPPORTED          0x00000001 /* ---VV */
#define NV_PBUS_PCI_NV_24_PME_D3_COLD_NOT_SUPPORTED      0x00000000 /* C--VV */
#define NV_PBUS_PCI_NV_24_PME_D3_HOT                          30:30 /* C--VF */
#define NV_PBUS_PCI_NV_24_PME_D3_HOT_SUPPORTED           0x00000001 /* ---VV */
#define NV_PBUS_PCI_NV_24_PME_D3_HOT_NOT_SUPPORTED       0x00000000 /* C--VV */
#define NV_PBUS_PCI_NV_24_PME_D2                              29:29 /* C--VF */
#define NV_PBUS_PCI_NV_24_PME_D2_SUPPORTED               0x00000001 /* ---VV */
#define NV_PBUS_PCI_NV_24_PME_D2_NOT_SUPPORTED           0x00000000 /* C--VV */
#define NV_PBUS_PCI_NV_24_PME_D1                              28:28 /* C--VF */
#define NV_PBUS_PCI_NV_24_PME_D1_SUPPORTED               0x00000001 /* ---VV */
#define NV_PBUS_PCI_NV_24_PME_D1_NOT_SUPPORTED           0x00000000 /* C--VV */
#define NV_PBUS_PCI_NV_24_PME_D0                              27:27 /* C--VF */
#define NV_PBUS_PCI_NV_24_PME_D0_SUPPORTED               0x00000001 /* ---VV */
#define NV_PBUS_PCI_NV_24_PME_D0_NOT_SUPPORTED           0x00000000 /* C--VV */
#define NV_PBUS_PCI_NV_24_D2                                  26:26 /* C--VF */
#define NV_PBUS_PCI_NV_24_D2_SUPPORTED                   0x00000001 /* ---VV */
#define NV_PBUS_PCI_NV_24_D2_NOT_SUPPORTED               0x00000000 /* C--VV */
#define NV_PBUS_PCI_NV_24_D1                                  25:25 /* C--VF */
#define NV_PBUS_PCI_NV_24_D1_SUPPORTED                   0x00000001 /* ---VV */
#define NV_PBUS_PCI_NV_24_D1_NOT_SUPPORTED               0x00000000 /* C--VV */
#define NV_PBUS_PCI_NV_24_DSI                                 21:21 /* C--VF */
#define NV_PBUS_PCI_NV_24_DSI_NOT_REQUIRED               0x00000000 /* C--VV */
#define NV_PBUS_PCI_NV_24_PME_CLOCK                           19:19 /* C--VV */
#define NV_PBUS_PCI_NV_24_PME_CLOCK_NOT_REQUIRED         0x00000000 /* C--VV */
#define NV_PBUS_PCI_NV_24_VERSION                             18:16 /* C--VV */
#define NV_PBUS_PCI_NV_24_VERSION_1                      0x00000001 /* ---VV */
#define NV_PBUS_PCI_NV_24_VERSION_2                      0x00000002 /* C--VV */
#define NV_PBUS_PCI_NV_24_NEXT_PTR                             15:8 /* R--VF */
#define NV_PBUS_PCI_NV_24_NEXT_PTR_NULL                  0x00000000 /* ----V */
#define NV_PBUS_PCI_NV_24_NEXT_PTR_AGP                   0x00000044 /* ----V */
#define NV_PBUS_PCI_NV_24_CAP_ID                                7:0 /* C--VF */
#define NV_PBUS_PCI_NV_24_CAP_ID_POWER_MGMT              0x00000001 /* C---V */
#define NV_PBUS_PCI_NV_25                                0x00001864 /* RW-4R */
#define NV_PBUS_PCI_NV_25__ALIAS_1              NV_CONFIG_PCI_NV_25 /*       */
#define NV_PBUS_PCI_NV_25_POWER_STATE                           1:0 /* RWIVF */
#define NV_PBUS_PCI_NV_25_POWER_STATE_D3_HOT             0x00000003 /* RW-VV */
#define NV_PBUS_PCI_NV_25_POWER_STATE_D2                 0x00000002 /* RW-VV */
#define NV_PBUS_PCI_NV_25_POWER_STATE_D1                 0x00000001 /* RW-VV */
#define NV_PBUS_PCI_NV_25_POWER_STATE_D0                 0x00000000 /* RWIVV */
#define NV_PBUS_PCI_NV_26(i)                     (0x00001868+(i)*4) /* R--4A */
#define NV_PBUS_PCI_NV_26__SIZE_1                                38 /*       */
#define NV_PBUS_PCI_NV_26__ALIAS_1              NV_CONFIG_PCI_NV_25 /*       */
#define NV_PBUS_PCI_NV_26_RESERVED                             31:0 /* C--VF */
#define NV_PBUS_PCI_NV_26_RESERVED_0                     0x00000000 /* C---V */
#define NV_PBUS_PWM                                      0x000010F0 /* RW-4R */
#define NV_PBUS_PWM_EN                                        31:31 /* RWIVF */
#define NV_PBUS_PWM_EN_DISABLED                          0x00000000 /* RWI-V */
#define NV_PBUS_PWM_EN_ENABLED                           0x00000001 /* RW--V */
#define NV_PBUS_PWM_HI                                        30:16 /* RWIVF */
#define NV_PBUS_PWM_HI_ZERO                              0x00000000 /* RWI-V */
#define NV_PBUS_PWM_PERIOD                                     14:0 /* RWIVF */
#define NV_PBUS_PWM_MAX_PERIOD                           0x0000EFFF /* RWI-V */
/* dev_fifo.ref */
#define NV_PFIFO                              0x00003FFF:0x00002000 /* RW--D */
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
#define NV_PFIFO_INTR_0_DMA_PT                                16:16 /* RWXVF */
#define NV_PFIFO_INTR_0_DMA_PT_NOT_PENDING               0x00000000 /* R---V */
#define NV_PFIFO_INTR_0_DMA_PT_PENDING                   0x00000001 /* R---V */
#define NV_PFIFO_INTR_0_DMA_PT_RESET                     0x00000001 /* -W--V */
#define NV_PFIFO_INTR_0_SEMAPHORE                             20:20 /* RWXVF */
#define NV_PFIFO_INTR_0_SEMAPHORE_NOT_PENDING            0x00000000 /* R---V */
#define NV_PFIFO_INTR_0_SEMAPHORE_PENDING                0x00000001 /* R---V */
#define NV_PFIFO_INTR_0_SEMAPHORE_RESET                  0x00000001 /* -W--V */
#define NV_PFIFO_INTR_0_ACQUIRE_TIMEOUT                       24:24 /* RWXVF */
#define NV_PFIFO_INTR_0_ACQUIRE_TIMEOUT_NOT_PENDING      0x00000000 /* R---V */
#define NV_PFIFO_INTR_0_ACQUIRE_TIMEOUT_PENDING          0x00000001 /* R---V */
#define NV_PFIFO_INTR_0_ACQUIRE_TIMEOUT_RESET            0x00000001 /* -W--V */
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
#define NV_PFIFO_INTR_EN_0_DMA_PT                             16:16 /* RWIVF */
#define NV_PFIFO_INTR_EN_0_DMA_PT_DISABLED               0x00000000 /* RWI-V */
#define NV_PFIFO_INTR_EN_0_DMA_PT_ENABLED                0x00000001 /* RW--V */
#define NV_PFIFO_INTR_EN_0_SEMAPHORE                          20:20 /* RWIVF */
#define NV_PFIFO_INTR_EN_0_SEMAPHORE_DISABLED            0x00000000 /* RWI-V */
#define NV_PFIFO_INTR_EN_0_SEMAPHORE_ENABLED             0x00000001 /* RW--V */
#define NV_PFIFO_INTR_EN_0_ACQUIRE_TIMEOUT                    24:24 /* RWIVF */
#define NV_PFIFO_INTR_EN_0_ACQUIRE_TIMEOUT_DISABLED      0x00000000 /* RWI-V */
#define NV_PFIFO_INTR_EN_0_ACQUIRE_TIMEOUT_ENABLED       0x00000001 /* RW--V */
#define NV_PFIFO_CACHES                                  0x00002500 /* RW-4R */
#define NV_PFIFO_CACHES_REASSIGN                                0:0 /* RWIVF */
#define NV_PFIFO_CACHES_REASSIGN_DISABLED                0x00000000 /* RWI-V */
#define NV_PFIFO_CACHES_REASSIGN_ENABLED                 0x00000001 /* RW--V */
#define NV_PFIFO_CACHES_DMA_SUSPEND                             4:4 /* R--VF */
#define NV_PFIFO_CACHES_DMA_SUSPEND_IDLE                 0x00000000 /* R---V */
#define NV_PFIFO_CACHES_DMA_SUSPEND_BUSY                 0x00000001 /* R---V */
#define NV_PFIFO_CACHE0_PUSH0                            0x00003000 /* RW-4R */
#define NV_PFIFO_CACHE0_PUSH0_ACCESS                            0:0 /* RWIVF */
#define NV_PFIFO_CACHE0_PUSH0_ACCESS_DISABLED            0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE0_PUSH0_ACCESS_ENABLED             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_PUSH0                            0x00003200 /* RW-4R */
#define NV_PFIFO_CACHE1_PUSH0_ACCESS                            0:0 /* RWIVF */
#define NV_PFIFO_CACHE1_PUSH0_ACCESS_DISABLED            0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE1_PUSH0_ACCESS_ENABLED             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_PUSH1                            0x00003004 /* RW-4R */
#define NV_PFIFO_CACHE0_PUSH1_CHID                              4:0 /* RWXUF */
#define NV_PFIFO_CACHE1_PUSH1                            0x00003204 /* RW-4R */
#define NV_PFIFO_CACHE1_PUSH1_CHID                              4:0 /* RWXUF */
#define NV_PFIFO_CACHE1_PUSH1_MODE                              8:8 /* RWIVF */
#define NV_PFIFO_CACHE1_PUSH1_MODE_PIO                   0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE1_PUSH1_MODE_DMA                   0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_DMA_PUSH                         0x00003220 /* RW-4R */
#define NV_PFIFO_CACHE1_DMA_PUSH_ACCESS                         0:0 /* RWIVF */
#define NV_PFIFO_CACHE1_DMA_PUSH_ACCESS_DISABLED         0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE1_DMA_PUSH_ACCESS_ENABLED          0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_DMA_PUSH_STATE                          4:4 /* R--VF */
#define NV_PFIFO_CACHE1_DMA_PUSH_STATE_IDLE              0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY              0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_DMA_PUSH_BUFFER                         8:8 /* R--VF */
#define NV_PFIFO_CACHE1_DMA_PUSH_BUFFER_NOT_EMPTY        0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_DMA_PUSH_BUFFER_EMPTY            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_DMA_PUSH_STATUS                       12:12 /* RWIVF */
#define NV_PFIFO_CACHE1_DMA_PUSH_STATUS_RUNNING          0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED        0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_DMA_PUSH_ACQUIRE                      16:16 /* RWIVF */
#define NV_PFIFO_CACHE1_DMA_PUSH_ACQUIRE_NOT_PENDING     0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_DMA_PUSH_ACQUIRE_PENDING         0x00000001 /* RWI-V */
#define NV_PFIFO_CACHE1_DMA_PUT                          0x00003240 /* RW-4R */
#define NV_PFIFO_CACHE1_DMA_GET                          0x00003244 /* RW-4R */
#define NV_PFIFO_CACHE1_DMA_STATE                        0x00003228 /* RW-4R */
#define NV_PFIFO_CACHE0_PULL0                            0x00003050 /* RW-4R */
#define NV_PFIFO_CACHE0_PULL0_ACCESS                            0:0 /* RWIVF */
#define NV_PFIFO_CACHE0_PULL0_ACCESS_DISABLED            0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE0_PULL0_ACCESS_ENABLED             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_PULL0_HASH                              4:4 /* R-XVF */
#define NV_PFIFO_CACHE0_PULL0_HASH_SUCCEEDED             0x00000000 /* R---V */
#define NV_PFIFO_CACHE0_PULL0_HASH_FAILED                0x00000001 /* R---V */
#define NV_PFIFO_CACHE0_PULL0_DEVICE                            8:8 /* R-XVF */
#define NV_PFIFO_CACHE0_PULL0_DEVICE_HARDWARE            0x00000000 /* R---V */
#define NV_PFIFO_CACHE0_PULL0_DEVICE_SOFTWARE            0x00000001 /* R---V */
#define NV_PFIFO_CACHE0_PULL0_HASH_STATE                      12:12 /* R-XVF */
#define NV_PFIFO_CACHE0_PULL0_HASH_STATE_IDLE            0x00000000 /* R---V */
#define NV_PFIFO_CACHE0_PULL0_HASH_STATE_BUSY            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_PULL0                            0x00003250 /* RW-4R */
#define NV_PFIFO_CACHE1_PULL0_ACCESS                            0:0 /* RWIVF */
#define NV_PFIFO_CACHE1_PULL0_ACCESS_DISABLED            0x00000000 /* RWI-V */
#define NV_PFIFO_CACHE1_PULL0_ACCESS_ENABLED             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_PULL0_HASH                              4:4 /* R-XVF */
#define NV_PFIFO_CACHE1_PULL0_HASH_SUCCEEDED             0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_HASH_FAILED                0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_DEVICE                            8:8 /* R-XVF */
#define NV_PFIFO_CACHE1_PULL0_DEVICE_HARDWARE            0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_DEVICE_SOFTWARE            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_HASH_STATE                      12:12 /* R-XVF */
#define NV_PFIFO_CACHE1_PULL0_HASH_STATE_IDLE            0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_HASH_STATE_BUSY            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_ACQUIRE_STATE                   16:16 /* R-XVF */
#define NV_PFIFO_CACHE1_PULL0_ACQUIRE_STATE_IDLE         0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_ACQUIRE_STATE_BUSY         0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_SEMAPHORE                       21:20 /* R-XVF */
#define NV_PFIFO_CACHE1_PULL0_SEMAPHORE_NO_ERROR         0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_SEMAPHORE_BAD_ARG          0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_PULL0_SEMAPHORE_INV_STATE        0x00000002 /* R---V */
#define NV_PFIFO_CACHE0_PULL1                            0x00003054 /* RW-4R */
#define NV_PFIFO_CACHE0_PULL1_ENGINE                            1:0 /* RWXUF */
#define NV_PFIFO_CACHE0_PULL1_ENGINE_SW                  0x00000000 /* RW--V */
#define NV_PFIFO_CACHE0_PULL1_ENGINE_GRAPHICS            0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_PULL1_ENGINE_DVD                 0x00000002 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1                            0x00003254 /* RW-4R */
#define NV_PFIFO_CACHE1_PULL1_ENGINE                            1:0 /* RWXUF */
#define NV_PFIFO_CACHE1_PULL1_ENGINE_SW                  0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1_ENGINE_GRAPHICS            0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1_ENGINE_DVD                 0x00000002 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1_ACQUIRE                           4:4 /* RWXVF */
#define NV_PFIFO_CACHE1_PULL1_ACQUIRE_INACTIVE           0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1_ACQUIRE_ACTIVE             0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1_SEM_TARGET_NODE                 17:16 /* RWXUF */
#define NV_PFIFO_CACHE1_PULL1_SEM_TARGET_NODE_NVM        0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1_SEM_TARGET_NODE_PCI        0x00000002 /* RW--V */
#define NV_PFIFO_CACHE1_PULL1_SEM_TARGET_NODE_AGP        0x00000003 /* RW--V */
#define NV_PFIFO_CACHE0_HASH                             0x00003058 /* RW-4R */
#define NV_PFIFO_CACHE0_HASH_INSTANCE                          15:0 /* RWXUF */
#define NV_PFIFO_CACHE0_HASH_VALID                            16:16 /* RWXVF */
#define NV_PFIFO_CACHE1_HASH                             0x00003258 /* RW-4R */
#define NV_PFIFO_CACHE1_HASH_INSTANCE                          15:0 /* RWXUF */
#define NV_PFIFO_CACHE1_HASH_VALID                            16:16 /* RWXVF */
#define NV_PFIFO_CACHE1_ACQUIRE_0                        0x00003260 /* RW-4R */
#define NV_PFIFO_CACHE1_ACQUIRE_0_TIMEOUT                      30:0 /* RWXUF */
#define NV_PFIFO_CACHE1_ACQUIRE_1                        0x00003264 /* RW-4R */
#define NV_PFIFO_CACHE1_ACQUIRE_1_TIMESTAMP                    31:0 /* RWXUF */
#define NV_PFIFO_CACHE1_ACQUIRE_2                        0x00003268 /* RW-4R */
#define NV_PFIFO_CACHE1_ACQUIRE_2_VALUE                        31:0 /* RWXUF */
#define NV_PFIFO_CACHE1_SEMAPHORE                        0x0000326C /* RW-4R */
#define NV_PFIFO_CACHE1_SEMAPHORE_CTXDMA                        0:0 /* RWXVF */
#define NV_PFIFO_CACHE1_SEMAPHORE_CTXDMA_INVALID         0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_SEMAPHORE_CTXDMA_VALID           0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_SEMAPHORE_OFFSET                       11:2 /* RWXUF */
#define NV_PFIFO_CACHE1_SEMAPHORE_PAGE_ADDRESS                31:12 /* RWXUF */
#define NV_PFIFO_CACHE0_STATUS                           0x00003014 /* R--4R */
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK                         4:4 /* R--VF */
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK_NOT_EMPTY        0x00000000 /* R---V */
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK_EMPTY            0x00000001 /* R---V */
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK                        8:8 /* R--VF */
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK_NOT_FULL        0x00000000 /* R---V */
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK_FULL            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_STATUS                           0x00003214 /* R--4R */
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK                         4:4 /* R--VF */
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK_NOT_EMPTY        0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK                        8:8 /* R--VF */
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK_NOT_FULL        0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK_FULL            0x00000001 /* R---V */
#define NV_PFIFO_CACHE1_STATUS1                          0x00003218 /* R--4R */
#define NV_PFIFO_CACHE1_STATUS1_RANOUT                          0:0 /* R-XVF */
#define NV_PFIFO_CACHE1_STATUS1_RANOUT_FALSE             0x00000000 /* R---V */
#define NV_PFIFO_CACHE1_STATUS1_RANOUT_TRUE              0x00000001 /* R---V */
#define NV_PFIFO_CACHE0_PUT                              0x00003010 /* RW-4R */
#define NV_PFIFO_CACHE0_PUT_ADDRESS                             2:2 /* RWXUF */
#define NV_PFIFO_CACHE1_PUT                              0x00003210 /* RW-4R */
#define NV_PFIFO_CACHE1_PUT_ADDRESS                             9:2 /* RWXUF */
#define NV_PFIFO_CACHE0_GET                              0x00003070 /* RW-4R */
#define NV_PFIFO_CACHE0_GET_ADDRESS                             2:2 /* RWXUF */
#define NV_PFIFO_CACHE1_GET                              0x00003270 /* RW-4R */
#define NV_PFIFO_CACHE1_GET_ADDRESS                             9:2 /* RWXUF */
#define NV_PFIFO_CACHE0_ENGINE                           0x00003080 /* RW-4R */
#define NV_PFIFO_CACHE0_ENGINE_0                                1:0 /* RWXUF */
#define NV_PFIFO_CACHE0_ENGINE_0_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_0_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_0_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_1                                5:4 /* RWXUF */
#define NV_PFIFO_CACHE0_ENGINE_1_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_1_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_1_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_2                                9:8 /* RWXUF */
#define NV_PFIFO_CACHE0_ENGINE_2_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_2_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_2_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_3                              13:12 /* RWXUF */
#define NV_PFIFO_CACHE0_ENGINE_3_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_3_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_3_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_4                              17:16 /* RWXUF */
#define NV_PFIFO_CACHE0_ENGINE_4_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_4_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_4_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_5                              21:20 /* RWXUF */
#define NV_PFIFO_CACHE0_ENGINE_5_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_5_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_5_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_6                              25:24 /* RWXUF */
#define NV_PFIFO_CACHE0_ENGINE_6_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_6_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_6_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_7                              29:28 /* RWXUF */
#define NV_PFIFO_CACHE0_ENGINE_7_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_7_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE0_ENGINE_7_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE                           0x00003280 /* RW-4R */
#define NV_PFIFO_CACHE1_ENGINE_0                                1:0 /* RWXUF */
#define NV_PFIFO_CACHE1_ENGINE_0_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_0_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_0_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_1                                5:4 /* RWXUF */
#define NV_PFIFO_CACHE1_ENGINE_1_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_1_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_1_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_2                                9:8 /* RWXUF */
#define NV_PFIFO_CACHE1_ENGINE_2_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_2_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_2_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_3                              13:12 /* RWXUF */
#define NV_PFIFO_CACHE1_ENGINE_3_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_3_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_3_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_4                              17:16 /* RWXUF */
#define NV_PFIFO_CACHE1_ENGINE_4_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_4_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_4_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_5                              21:20 /* RWXUF */
#define NV_PFIFO_CACHE1_ENGINE_5_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_5_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_5_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_6                              25:24 /* RWXUF */
#define NV_PFIFO_CACHE1_ENGINE_6_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_6_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_6_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_7                              29:28 /* RWXUF */
#define NV_PFIFO_CACHE1_ENGINE_7_SW                      0x00000000 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_7_GRAPHICS                0x00000001 /* RW--V */
#define NV_PFIFO_CACHE1_ENGINE_7_DVD                     0x00000002 /* RW--V */
#define NV_PFIFO_CACHE0_METHOD(i)                (0x00003100+(i)*8) /* RW-4A */
#define NV_PFIFO_CACHE0_METHOD__SIZE_1                            1 /*       */
#define NV_PFIFO_CACHE0_METHOD_ADDRESS                         12:2 /* RWXUF */
#define NV_PFIFO_CACHE0_METHOD_SUBCHANNEL                     15:13 /* RWXUF */
#define NV_PFIFO_CACHE1_METHOD(i)                (0x00003800+(i)*8) /* RW-4A */
#define NV_PFIFO_CACHE1_METHOD__SIZE_1                          128 /*       */
#define NV_PFIFO_CACHE1_METHOD_TYPE                             0:0 /* RWXUF */
#define NV_PFIFO_CACHE1_METHOD_ADDRESS                         12:2 /* RWXUF */
#define NV_PFIFO_CACHE1_METHOD_SUBCHANNEL                     15:13 /* RWXUF */
#define NV_PFIFO_CACHE1_METHOD_ALIAS(i)          (0x00003C00+(i)*8) /* RW-4A */
#define NV_PFIFO_CACHE1_METHOD_ALIAS__SIZE_1                    128 /*       */
#define NV_PFIFO_CACHE0_DATA(i)                  (0x00003104+(i)*8) /* RW-4A */
#define NV_PFIFO_CACHE0_DATA__SIZE_1                              1 /*       */
#define NV_PFIFO_CACHE0_DATA_VALUE                             31:0 /* RWXVF */
#define NV_PFIFO_CACHE1_DATA(i)                  (0x00003804+(i)*8) /* RW-4A */
#define NV_PFIFO_CACHE1_DATA__SIZE_1                            128 /*       */
#define NV_PFIFO_CACHE1_DATA_VALUE                             31:0 /* RWXVF */
#define NV_PFIFO_CACHE1_DATA_ALIAS(i)            (0x00003C04+(i)*8) /* RW-4A */
#define NV_PFIFO_CACHE1_DATA_ALIAS__SIZE_1                      128 /*       */
/* dev_graphics.ref */
#define NV_PGRAPH                             0x00401FFF:0x00400000 /* RW--D */
#define NV_PGRAPH_INTR                                   0x00400100 /* RW-4R */
#define NV_PGRAPH_INTR_NOTIFY                                   0:0 /* RWIVF */
#define NV_PGRAPH_INTR_NOTIFY_NOT_PENDING                0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_NOTIFY_PENDING                    0x00000001 /* R---V */
#define NV_PGRAPH_INTR_NOTIFY_RESET                      0x00000001 /* -W--C */
#define NV_PGRAPH_INTR_MISSING_HW                               4:4 /* RWIVF */
#define NV_PGRAPH_INTR_MISSING_HW_NOT_PENDING            0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_MISSING_HW_PENDING                0x00000001 /* R---V */
#define NV_PGRAPH_INTR_MISSING_HW_RESET                  0x00000001 /* -W--C */
/*  bits [7:6] are nv20 only */
#define NV_PGRAPH_INTR_TLB_PRESENT_DMA_R                        6:6 /* RWIVF */
#define NV_PGRAPH_INTR_TLB_PRESENT_DMA_R_NOT_PENDING     0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_TLB_PRESENT_DMA_R_PENDING         0x00000001 /* R---V */
#define NV_PGRAPH_INTR_TLB_PRESENT_DMA_R_RESET           0x00000001 /* -W--C */
#define NV_PGRAPH_INTR_TLB_PRESENT_DMA_W                        7:7 /* RWIVF */
#define NV_PGRAPH_INTR_TLB_PRESENT_DMA_W_NOT_PENDING     0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_TLB_PRESENT_DMA_W_PENDING         0x00000001 /* R---V */
#define NV_PGRAPH_INTR_TLB_PRESENT_DMA_W_RESET           0x00000001 /* -W--C */
/*  TEX_{A,B} are nv20 only */
#define NV_PGRAPH_INTR_TLB_PRESENT_A                            8:8 /* RWIVF */
#define NV_PGRAPH_INTR_TLB_PRESENT_A_NOT_PENDING         0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_TLB_PRESENT_A_PENDING             0x00000001 /* R---V */
#define NV_PGRAPH_INTR_TLB_PRESENT_A_RESET               0x00000001 /* -W--C */
#define NV_PGRAPH_INTR_TLB_PRESENT_TEX_A                        8:8 /* RWIVF */
#define NV_PGRAPH_INTR_TLB_PRESENT_TEX_A_NOT_PENDING     0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_TLB_PRESENT_TEX_A_PENDING         0x00000001 /* R---V */
#define NV_PGRAPH_INTR_TLB_PRESENT_TEX_A_RESET           0x00000001 /* -W--C */
#define NV_PGRAPH_INTR_TLB_PRESENT_B                            9:9 /* RWIVF */
#define NV_PGRAPH_INTR_TLB_PRESENT_B_NOT_PENDING         0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_TLB_PRESENT_B_PENDING             0x00000001 /* R---V */
#define NV_PGRAPH_INTR_TLB_PRESENT_B_RESET               0x00000001 /* -W--C */
#define NV_PGRAPH_INTR_TLB_PRESENT_TEX_B                        9:9 /* RWIVF */
#define NV_PGRAPH_INTR_TLB_PRESENT_TEX_B_NOT_PENDING     0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_TLB_PRESENT_TEX_B_PENDING         0x00000001 /* R---V */
#define NV_PGRAPH_INTR_TLB_PRESENT_TEX_B_RESET           0x00000001 /* -W--C */
#define NV_PGRAPH_INTR_TLB_PRESENT_VTX                        10:10 /* RWIVF */
#define NV_PGRAPH_INTR_TLB_PRESENT_VTX_NOT_PENDING       0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_TLB_PRESENT_VTX_PENDING           0x00000001 /* R---V */
#define NV_PGRAPH_INTR_TLB_PRESENT_VTX_RESET             0x00000001 /* -W--C */
#define NV_PGRAPH_INTR_CONTEXT_SWITCH                         12:12 /* RWIVF */
#define NV_PGRAPH_INTR_CONTEXT_SWITCH_NOT_PENDING        0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_CONTEXT_SWITCH_PENDING            0x00000001 /* R---V */
#define NV_PGRAPH_INTR_CONTEXT_SWITCH_RESET              0x00000001 /* -W--C */
#define NV_PGRAPH_INTR_STATE3D                                13:13 /* RWIVF */
#define NV_PGRAPH_INTR_STATE3D_NOT_PENDING               0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_STATE3D_PENDING                   0x00000001 /* R---V */
#define NV_PGRAPH_INTR_STATE3D_RESET                     0x00000001 /* -W--C */
#define NV_PGRAPH_INTR_BUFFER_NOTIFY                          16:16 /* RWIVF */
#define NV_PGRAPH_INTR_BUFFER_NOTIFY_NOT_PENDING         0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_BUFFER_NOTIFY_PENDING             0x00000001 /* R---V */
#define NV_PGRAPH_INTR_BUFFER_NOTIFY_RESET               0x00000001 /* -W--C */
#define NV_PGRAPH_INTR_ERROR                                  20:20 /* RWIVF */
#define NV_PGRAPH_INTR_ERROR_NOT_PENDING                 0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_ERROR_PENDING                     0x00000001 /* R---V */
#define NV_PGRAPH_INTR_ERROR_RESET                       0x00000001 /* -W--C */
#define NV_PGRAPH_INTR_SINGLE_STEP                            24:24 /* RWIVF */
#define NV_PGRAPH_INTR_SINGLE_STEP_NOT_PENDING           0x00000000 /* R-I-V */
#define NV_PGRAPH_INTR_SINGLE_STEP_PENDING               0x00000001 /* R---V */
#define NV_PGRAPH_INTR_SINGLE_STEP_RESET                 0x00000001 /* -W--C */
#define NV_PGRAPH_NSOURCE                                0x00400108 /* R--4R */
#define NV_PGRAPH_NSOURCE_NOTIFICATION                          0:0 /* R-IVF */
#define NV_PGRAPH_NSOURCE_NOTIFICATION_NOT_PENDING       0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_NOTIFICATION_PENDING           0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_DATA_ERROR                            1:1 /* R-IVF */
#define NV_PGRAPH_NSOURCE_DATA_ERROR_NOT_PENDING         0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_DATA_ERROR_PENDING             0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_PROTECTION_ERROR                      2:2 /* R-IVF */
#define NV_PGRAPH_NSOURCE_PROTECTION_ERROR_NOT_PENDING   0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_PROTECTION_ERROR_PENDING       0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_RANGE_EXCEPTION                       3:3 /* R-IVF */
#define NV_PGRAPH_NSOURCE_RANGE_EXCEPTION_NOT_PENDING    0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_RANGE_EXCEPTION_PENDING        0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_LIMIT_COLOR                           4:4 /* R-IVF */
#define NV_PGRAPH_NSOURCE_LIMIT_COLOR_NOT_PENDING        0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_LIMIT_COLOR_PENDING            0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_LIMIT_ZETA                            5:5 /* R-IVF */
#define NV_PGRAPH_NSOURCE_LIMIT_ZETA_NOT_PENDING         0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_LIMIT_ZETA_PENDING             0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_ILLEGAL_MTHD                          6:6 /* R-IVF */
#define NV_PGRAPH_NSOURCE_ILLEGAL_MTHD_NOT_PENDING       0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_ILLEGAL_MTHD_PENDING           0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_DMA_R_PROTECTION                      7:7 /* R-IVF */
#define NV_PGRAPH_NSOURCE_DMA_R_PROTECTION_NOT_PENDING   0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_DMA_R_PROTECTION_PENDING       0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_DMA_W_PROTECTION                      8:8 /* R-IVF */
#define NV_PGRAPH_NSOURCE_DMA_W_PROTECTION_NOT_PENDING   0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_DMA_W_PROTECTION_PENDING       0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_FORMAT_EXCEPTION                      9:9 /* R-IVF */
#define NV_PGRAPH_NSOURCE_FORMAT_EXCEPTION_NOT_PENDING   0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_FORMAT_EXCEPTION_PENDING       0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_PATCH_EXCEPTION                     10:10 /* R-IVF */
#define NV_PGRAPH_NSOURCE_PATCH_EXCEPTION_NOT_PENDING    0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_PATCH_EXCEPTION_PENDING        0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_STATE_INVALID                       11:11 /* R-IVF */
#define NV_PGRAPH_NSOURCE_STATE_INVALID_NOT_PENDING      0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_STATE_INVALID_PENDING          0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_DOUBLE_NOTIFY                       12:12 /* R-IVF */
#define NV_PGRAPH_NSOURCE_DOUBLE_NOTIFY_NOT_PENDING      0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_DOUBLE_NOTIFY_PENDING          0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_NOTIFY_IN_USE                       13:13 /* R-IVF */
#define NV_PGRAPH_NSOURCE_NOTIFY_IN_USE_NOT_PENDING      0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_NOTIFY_IN_USE_PENDING          0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_METHOD_CNT                          14:14 /* R-IVF */
#define NV_PGRAPH_NSOURCE_METHOD_CNT_NOT_PENDING         0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_METHOD_CNT_PENDING             0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_BFR_NOTIFICATION                    15:15 /* R-IVF */
#define NV_PGRAPH_NSOURCE_BFR_NOTIFICATION_NOT_PENDING   0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_BFR_NOTIFICATION_PENDING       0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_DMA_VTX_PROTECTION                  16:16 /* R-IVF */
#define NV_PGRAPH_NSOURCE_DMA_VTX_PROTECTION_NOT_PENDING 0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_DMA_VTX_PROTECTION_PENDING     0x00000001 /* R---V */
/*  NV_PGRAPH_NSOURCE_DMA_WIDTH_A is nv4/nv10 only */
#define NV_PGRAPH_NSOURCE_DMA_WIDTH_A                         17:17 /* R-IVF */
#define NV_PGRAPH_NSOURCE_DMA_WIDTH_A_NOT_PENDING        0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_DMA_WIDTH_A_PENDING            0x00000001 /* R---V */
/*  NV_PGRAPH_NSOURCE_IDX_INLINE_REUSE is nv20-only */
#define NV_PGRAPH_NSOURCE_IDX_INLINE_REUSE                    17:17 /* R-IVF */
#define NV_PGRAPH_NSOURCE_IDX_INLINE_REUSE_NOT_PENDING   0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_IDX_INLINE_REUSE_PENDING       0x00000001 /* R---V */
/*  NV_PGRAPH_NSOURCE_DMA_WIDTH_B is nv4/nv10 only */
#define NV_PGRAPH_NSOURCE_DMA_WIDTH_B                         18:18 /* R-IVF */
#define NV_PGRAPH_NSOURCE_DMA_WIDTH_B_NOT_PENDING        0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_DMA_WIDTH_B_PENDING            0x00000001 /* R---V */
/*  NV_PGRAPH_NSOURCE_INVALID_OPERATION is nv20-only */
#define NV_PGRAPH_NSOURCE_INVALID_OPERATION                   18:18 /* R-IVF */
#define NV_PGRAPH_NSOURCE_INVALID_OPERATION_NOT_PENDING  0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_INVALID_OPERATION_PENDING      0x00000001 /* R---V */
/*  NV_PGRAPH_NSOURCE_FD_INVALID is nv20-only */
#define NV_PGRAPH_NSOURCE_FD_INVALID_OP                       19:19 /* R-IVF */
#define NV_PGRAPH_NSOURCE_FD_INVALID_OP_NOT_PENDING      0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_FD_INVALID_OP_PENDING          0x00000001 /* R---V */
/*  NV_PGRAPH_NSOURCE_INVALID_OPERATION is nv20-only */
#define NV_PGRAPH_NSOURCE_FD_ERROR_CODE                       21:20 /* R-IVF */
#define NV_PGRAPH_NSOURCE_FD_ERROR_CODE_0                0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_FD_ERROR_CODE_1                0x00000001 /* R---V */
#define NV_PGRAPH_NSOURCE_FD_ERROR_CODE_2                0x00000002 /* R---V */
#define NV_PGRAPH_NSOURCE_FD_ERROR_CODE_3                0x00000003 /* R---V */
/*  NV_PGRAPH_NSOURCE_TEX_A_PROTECTION is nv20-only */
#define NV_PGRAPH_NSOURCE_TEX_A_PROTECTION                    22:22 /* R-IVF */
#define NV_PGRAPH_NSOURCE_TEX_A_PROTECTION_NOT_PENDING   0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_TEX_A_PROTECTION_PENDING       0x00000001 /* R---V */
/*  NV_PGRAPH_NSOURCE_TEX_B_PROTECTION is nv20-only */
#define NV_PGRAPH_NSOURCE_TEX_B_PROTECTION                    23:23 /* R-IVF */
#define NV_PGRAPH_NSOURCE_TEX_B_PROTECTION_NOT_PENDING   0x00000000 /* R-I-V */
#define NV_PGRAPH_NSOURCE_TEX_B_PROTECTION_PENDING       0x00000001 /* R---V */
#define NV_PGRAPH_INTR_EN                                0x00400140 /* RW-4R */
#define NV_PGRAPH_INTR_EN_NOTIFY                                0:0 /* RWIVF */
#define NV_PGRAPH_INTR_EN_NOTIFY_DISABLED                0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_NOTIFY_ENABLED                 0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_MISSING_HW                            4:4 /* RWIVF */
#define NV_PGRAPH_INTR_EN_MISSING_HW_DISABLED            0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_MISSING_HW_ENABLED             0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_DMA_R                     6:6 /* RWIVF */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_DMA_R_DISABLED     0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_DMA_R_ENABLED      0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_DMA_W                     7:7 /* RWIVF */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_DMA_W_DISABLED     0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_DMA_W_ENABLED      0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_A                         8:8 /* RWIVF */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_A_DISABLED         0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_A_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_TEX_A                     8:8 /* RWIVF */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_TEX_A_DISABLED     0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_TEX_A_ENABLED      0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_B                         9:9 /* RWIVF */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_B_DISABLED         0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_B_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_TEX_B                     9:9 /* RWIVF */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_TEX_B_DISABLED     0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_TEX_B_ENABLED      0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_VTX                     10:10 /* RWIVF */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_VTX_DISABLED       0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_VTX_ENABLED        0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_CONTEXT_SWITCH                      12:12 /* RWIVF */
#define NV_PGRAPH_INTR_EN_CONTEXT_SWITCH_DISABLED        0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_CONTEXT_SWITCH_ENABLED         0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_STATE3D                             13:13 /* RWIVF */
#define NV_PGRAPH_INTR_EN_STATE3D_DISABLED               0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_STATE3D_ENABLED                0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_BUFFER_NOTIFY                       16:16 /* RWIVF */
#define NV_PGRAPH_INTR_EN_BUFFER_NOTIFY_DISABLED         0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_BUFFER_NOTIFY_ENABLED          0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_ERROR                               20:20 /* RWIVF */
#define NV_PGRAPH_INTR_EN_ERROR_DISABLED                 0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_ERROR_ENABLED                  0x00000001 /* RW--V */
#define NV_PGRAPH_INTR_EN_SINGLE_STEP                         24:24 /* RWIVF */
#define NV_PGRAPH_INTR_EN_SINGLE_STEP_DISABLED           0x00000000 /* RWI-V */
#define NV_PGRAPH_INTR_EN_SINGLE_STEP_ENABLED            0x00000001 /* RW--V */
/*  NV_PGRAPH_CTX_SWITCH1 needs to be available for nv4 gr object validation */
#define NV_PGRAPH_CTX_SWITCH1_GRCLASS                           7:0 /* RWXVF */
#define NV_PGRAPH_CTX_SWITCH1_CHROMA_KEY                      12:12 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_CHROMA_KEY_DISABLE         0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CHROMA_KEY_ENABLE          0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_USER_CLIP                       13:13 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_USER_CLIP_DISABLE          0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_USER_CLIP_ENABLE           0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_SWIZZLE                         14:14 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_SWIZZLE_DISABLE            0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_SWIZZLE_ENABLE             0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG                    17:15 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_SRCCOPY_AND   0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_ROP_AND       0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_BLEND_AND     0x00000002 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_SRCCOPY       0x00000003 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_SRCCOPY_PRE   0x00000004 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_BLEND_PRE     0x00000005 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_SYNCHRONIZE                     18:18 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_SYNCHRONIZE_DISABLE        0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_SYNCHRONIZE_ENABLE         0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_ENDIAN_MODE                     19:19 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_ENDIAN_MODE_LITTLE         0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_ENDIAN_MODE_BIG            0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_DITHER_MODE                     21:20 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_DITHER_MODE_COMPATIBILITY  0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_DITHER_MODE_DITHER         0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_DITHER_MODE_TRUNCATE       0x00000002 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_DITHER_MODE_MS             0x00000003 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CLASS_TYPE                      22:22 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_CLASS_TYPE_COMPATIBILITY   0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CLASS_TYPE_PERFORMANCE     0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_SINGLE_STEP                     23:23 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_SINGLE_STEP_DISABLED       0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_SINGLE_STEP_ENABLED        0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_PATCH_STATUS                    24:24 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_PATCH_STATUS_INVALID       0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_PATCH_STATUS_VALID         0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_SURFACE                 25:25 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_SURFACE_INVALID    0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_SURFACE_VALID      0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_SURFACE0                25:25 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_SURFACE0_INVALID   0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_SURFACE0_VALID     0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_SURFACE1                26:26 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_SURFACE1_INVALID   0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_SURFACE1_VALID     0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_PATTERN                 27:27 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_PATTERN_INVALID    0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_PATTERN_VALID      0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_ROP                     28:28 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_ROP_INVALID        0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_ROP_VALID          0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_BETA1                   29:29 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_BETA1_INVALID      0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_BETA1_VALID        0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_BETA4                   30:30 /* RWXUF */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_BETA4_INVALID      0x00000000 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_CONTEXT_BETA4_VALID        0x00000001 /* RW--V */
#define NV_PGRAPH_CTX_SWITCH1_VOLATILE_RESET                  31:31 /* CWIVF */
#define NV_PGRAPH_CTX_SWITCH1_VOLATILE_RESET_IGNORE      0x00000000 /* CWI-V */
#define NV_PGRAPH_CTX_SWITCH1_VOLATILE_RESET_ENABLED     0x00000001 /* -W--T */
/*  NV_PGRAPH_STATUS needed for GR_IDLE()/GR_DONE() macros */
#define NV_PGRAPH_STATUS                                 0x00400700 /* R--4R */
/*  NV_PGRAPH_INCREMENT is needed by NV15_VIDEO_LUT_CURSOR_DAC class */
/*   this needs to be moved back to nv10_ref.h as soon as we HALify the dac */
#define NV_PGRAPH_INCREMENT                              0x0040071C /* RW-4R */
#define NV_PGRAPH_INCREMENT_READ_BLIT                           0:0 /* CWIVF */
#define NV_PGRAPH_INCREMENT_READ_BLIT_IGNORE             0x00000000 /* CWI-V */
#define NV_PGRAPH_INCREMENT_READ_BLIT_TRIGGER            0x00000001 /* -W--T */
#define NV_PGRAPH_INCREMENT_READ_3D                             1:1 /* CWIVF */
#define NV_PGRAPH_INCREMENT_READ_3D_IGNORE               0x00000000 /* CWI-V */
#define NV_PGRAPH_INCREMENT_READ_3D_TRIGGER              0x00000001 /* -W--T */
#define NV_PGRAPH_FIFO                                   0x00400720 /* RW-4R */
#define NV_PGRAPH_FIFO_ACCESS                                   0:0 /* RWIVF */
#define NV_PGRAPH_FIFO_ACCESS_DISABLED                   0x00000000 /* RW--V */
#define NV_PGRAPH_FIFO_ACCESS_ENABLED                    0x00000001 /* RWI-V */
/* dev_vga.ref */
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
/* dev_vga.ref */
#define NV_PCRTC_INTR_0                                  0x00600100 /* RWI4R */
#define NV_PCRTC_INTR_0_VBLANK                                  0:0 /* RWI-F */
#define NV_PCRTC_INTR_0_VBLANK_NOT_PENDING               0x00000000 /* R-I-V */
#define NV_PCRTC_INTR_0_VBLANK_PENDING                   0x00000001 /* R---V */
#define NV_PCRTC_INTR_0_VBLANK_RESET                     0x00000001 /* -W--V */
#define NV_PCRTC_INTR_EN_0                               0x00600140 /* RWI4R */
#define NV_PCRTC_INTR_EN_0_VBLANK                               0:0 /* RWIVF */
#define NV_PCRTC_INTR_EN_0_VBLANK_DISABLED               0x00000000 /* RWI-V */
#define NV_PCRTC_INTR_EN_0_VBLANK_ENABLED                0x00000001 /* RW--V */
#define NV_PCRTC_START                                   0x00600800 /* RWI4R */
/*  use the larger nv10 define (nv4 should just read/write 0's) */
#define NV_PCRTC_START_ADDRESS                                 26:2 /* RWI-F */
#define NV_PCRTC_CONFIG                                  0x00600804 /* RWI4R */
/*  use the larger nv10 define (nv4 should just read/write 0's) */
#define NV_PCRTC_CONFIG_START_ADDRESS                           2:0 /* RWI-F */
#define NV_PCRTC_CONFIG_START_ADDRESS_VGA                0x00000000 /* RWI-V */
#define NV_PCRTC_CONFIG_START_ADDRESS_NON_VGA            0x00000001 /* RW--V */
#define NV_PCRTC_CONFIG_START_ADDRESS_HSYNC_NV10         0x00000002 /* RW--V */
#define NV_PCRTC_CONFIG_START_ADDRESS_HSYNC_NV5          0x00000004 /* RW--V */
#define NV_PCRTC_CONFIG_ENDIAN                                31:31 /* RWI-F */
#define NV_PCRTC_CONFIG_ENDIAN_LITTLE                    0x00000000 /* RWI-V */
#define NV_PCRTC_CONFIG_ENDIAN_BIG                       0x00000001 /* RWI-V */
#define NV_PCRTC_RASTER                                  0x00600808 /* R-I4R */
#define NV_PCRTC_RASTER_POSITION                               10:0 /* R---F */
#define NV_PCRTC_RASTER_SA_LOAD                               13:12 /* R---F */
#define NV_PCRTC_RASTER_SA_LOAD_DISPLAY                  0x00000000 /* R---V */
#define NV_PCRTC_RASTER_SA_LOAD_BEFORE                   0x00000001 /* R---V */
#define NV_PCRTC_RASTER_SA_LOAD_AFTER                    0x00000002 /* R---V */
#define NV_PCRTC_RASTER_VERT_BLANK                            16:16 /* R---F */
#define NV_PCRTC_RASTER_VERT_BLANK_ACTIVE                0x00000001 /* R---V */
#define NV_PCRTC_RASTER_VERT_BLANK_INACTIVE              0x00000000 /* R---V */
#define NV_PCRTC_RASTER_FIELD                                 20:20 /* R---F */
#define NV_PCRTC_RASTER_FIELD_EVEN                       0x00000000 /* R---V */
#define NV_PCRTC_RASTER_FIELD_ODD                        0x00000001 /* R---V */
#define NV_PCRTC_RASTER_STEREO                                24:24 /* R---F */
#define NV_PCRTC_RASTER_STEREO_LEFT                      0x00000000 /* R---V */
#define NV_PCRTC_RASTER_STEREO_RIGHT                     0x00000001 /* R---V */
#define NV_PCRTC_RASTER_START                            0x00600830 /* RWI4R */
#define NV_PCRTC_RASTER_START_VAL                              15:0 /* RW--F */
#define NV_PCRTC_RASTER_START_POL                             31:31 /* RW--F */
#define NV_PCRTC_RASTER_START_POL_IN_RANGE               0x00000000 /* RWI-V */
#define NV_PCRTC_RASTER_START_POL_OUT_RANGE              0x00000001 /* RW--V */
 /* nv11-only */
#define NV_PCRTC_GPIO_EXT                                0x0060081C /* RWI4R */
#define NV_PCRTC_GPIO_EXT_2_OUTPUT                              0:0 /* RWI-F */
#define NV_PCRTC_GPIO_EXT_2_OUTPUT_0                     0x00000000 /* RWI-V */
#define NV_PCRTC_GPIO_EXT_2_ENABLE                              1:1 /* R---F */
#define NV_PCRTC_GPIO_EXT_2_ENABLE_DISABLE               0x00000001 /* RWI-V */
#define NV_PCRTC_GPIO_EXT_2_ENABLE_ENABLE                0x00000000 /* RW--V */
#define NV_PCRTC_GPIO_EXT_2_INPUT                               2:2 /* R---F */
#define NV_PCRTC_GPIO_EXT_3_OUTPUT                              4:4 /* RWI-F */
#define NV_PCRTC_GPIO_EXT_3_OUTPUT_0                     0x00000000 /* RWI-V */
#define NV_PCRTC_GPIO_EXT_3_ENABLE                              5:5 /* R---F */
#define NV_PCRTC_GPIO_EXT_3_ENABLE_DISABLE               0x00000001 /* RWI-V */
#define NV_PCRTC_GPIO_EXT_3_ENABLE_ENABLE                0x00000000 /* RW--V */
#define NV_PCRTC_GPIO_EXT_3_INPUT                               6:6 /* R---F */
#define NV_PCRTC_GPIO_EXT_4_OUTPUT                              8:8 /* RWI-F */
#define NV_PCRTC_GPIO_EXT_4_OUTPUT_0                     0x00000000 /* RWI-V */
#define NV_PCRTC_GPIO_EXT_4_ENABLE                              9:9 /* R---F */
#define NV_PCRTC_GPIO_EXT_4_ENABLE_DISABLE               0x00000001 /* RWI-V */
#define NV_PCRTC_GPIO_EXT_4_ENABLE_ENABLE                0x00000000 /* RW--V */
#define NV_PCRTC_GPIO_EXT_4_INPUT                             10:10 /* R---F */
#define NV_PCRTC_GPIO_EXT_5_OUTPUT                            12:12 /* RWI-F */
#define NV_PCRTC_GPIO_EXT_5_OUTPUT_0                     0x00000000 /* RWI-V */
#define NV_PCRTC_GPIO_EXT_5_ENABLE                            13:13 /* R---F */
#define NV_PCRTC_GPIO_EXT_5_ENABLE_DISABLE               0x00000001 /* RWI-V */
#define NV_PCRTC_GPIO_EXT_5_ENABLE_ENABLE                0x00000000 /* RW--V */
#define NV_PCRTC_GPIO_EXT_5_INPUT                             14:14 /* R---F */
 /* nv20-only */
#define NV_PCRTC_RASTER_POL                                   31:31 /* RW--F */
#define NV_PCRTC_RASTER_POL_IN_RANGE                              0 /* RWI-V */
#define NV_PCRTC_RASTER_POL_OUT_RANGE                             1 /* RW--V */
#define NV_PCRTC_RASTER_STOP                             0x00600834 /* RWI4R */
#define NV_PCRTC_RASTER_STOP_VAL                               15:0 /* RW--F */
#define NV_PCRTC_ENGINE_CTRL                             0x00600860 /* RWI4R */
#define NV_PCRTC_ENGINE_CTRL_GPIO                               0:0 /* RW--F */
#define NV_PCRTC_ENGINE_CTRL_GPIO_DEFAULT                0x00000000 /* RWI-F */
#define NV_PCRTC_ENGINE_CTRL_GPIO_ENABLE                 0x00000001 /* RW--F */
#define NV_PCRTC_ENGINE_CTRL_I2C                                4:4 /* RW--F */
#define NV_PCRTC_ENGINE_CTRL_I2C_DEFAULT                 0x00000000 /* RWI-F */
#define NV_PCRTC_ENGINE_CTRL_I2C_ENABLE                  0x00000001 /* RW--F */
#define NV_PCRTC_ENGINE_CTRL_TV                                 8:8 /* RW--F */
#define NV_PCRTC_ENGINE_CTRL_TV_DEFAULT                  0x00000000 /* RWI-F */
#define NV_PCRTC_ENGINE_CTRL_TV_ENABLE                   0x00000001 /* RW--F */
#define NV_PCRTC_ENGINE_CTRL_VS                               12:12 /* RW--F */
#define NV_PCRTC_ENGINE_CTRL_VS_DEFAULT                  0x00000000 /* RWI-F */
#define NV_PCRTC_ENGINE_CTRL_VS_ENABLE                   0x00000001 /* RW--F */
/* dev_vga.ref */
#define NV_CIO                                          0x3DF:0x3B0 /* ----D */
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
#define NV_CIO_CRE_RPC0_INDEX                            0x00000019 /*       */
#define NV_CIO_CRE_RPC0_START                                   4:0 /* RW--F */
#define NV_CIO_CRE_RPC0_OFFSET_10_8                             7:5 /* RW--F */
#define NV_CIO_CRE_RPC1_INDEX                            0x0000001A /*       */
#define NV_CIO_CRE_RPC1_LARGE                                   2:2 /* RW--F */
#define NV_CIO_CRE_FF_INDEX                              0x0000001B /*       */
#define NV_CIO_CRE_FF_BURST                                     2:0 /* RW--F */
#define NV_CIO_CRE_FF_BURST_8                            0x00000000 /* RW--V */
#define NV_CIO_CRE_FF_BURST_32                           0x00000001 /* RW--V */
#define NV_CIO_CRE_FF_BURST_64                           0x00000002 /* RW--V */
#define NV_CIO_CRE_FF_BURST_128                          0x00000003 /* RW--V */
#define NV_CIO_CRE_FF_BURST_256                          0x00000004 /* RW--V */
#define NV_CIO_CRE_FF_BURST_512                          0x00000005 /* RW--V */
#define NV_CIO_CRE_ENH_INDEX                             0x0000001C /*       */
#define NV_CIO_CRE_PAGE0_INDEX                           0x0000001D /*       */
#define NV_CIO_CRE_PAGE1_INDEX                           0x0000001E /*       */
#define NV_CIO_SR_LOCK_INDEX                             0x0000001F /*       */
#define NV_CIO_SR_UNLOCK_RW_VALUE                        0x00000057 /*       */
#define NV_CIO_SR_UNLOCK_RO_VALUE                        0x00000075 /*       */
#define NV_CIO_SR_LOCK_VALUE                             0x00000099 /*       */
#define NV_SR_UNLOCK_RW_VALUE                            0x00000057 /*       */
#define NV_SR_UNLOCK_RO_VALUE                            0x00000075 /*       */
#define NV_SR_LOCK_VALUE                                 0x00000099 /*       */
#define NV_CIO_CRE_FFLWM__INDEX                          0x00000020 /*       */
#define NV_CIO_CRE_FABID_INDEX                           0x00000025 /*       */
#define NV_CIO_CRE_LSR_INDEX                             0x00000025 /*       */
#define NV_CIO_CRE_LSR_SA_26                                    6:6 /* RW--F */
#define NV_CIO_CRE_LSR_VDE_10                                   1:1 /* RW--F */
#define NV_CIO_CRE_LSR_VDT_10                                   0:0 /* RW--F */
#define NV_CIO_CRE_LSR_HBE_6                                    4:4 /* RW--F */
#define NV_CIO_CRE_LSR_VBS_10                                   3:3 /* RW--F */
#define NV_CIO_CRE_LSR_VRS_10                                   2:2 /* RW--F */
#define NV_CIO_CRE_CHIP_ID_INDEX                         0x00000027 /*       */
#define NV_CIO_CRE_PIXEL_INDEX                           0x00000028 /*       */
#define NV_CIO_CRE_PIXEL_TV_ADJ                                 5:3 /* RW--F */
#define NV_CIO_CRE_PIXEL_FORMAT                                 1:0 /* RW--F */
#define NV_CIO_CRE_PIXEL_FORMAT_VGA                      0x00000000 /* RW--V */
#define NV_CIO_CRE_PIXEL_FORMAT_8BPP                     0x00000001 /* RW--V */
#define NV_CIO_CRE_PIXEL_FORMAT_16BPP                    0x00000002 /* RW--V */
#define NV_CIO_CRE_PIXEL_FORMAT_32BPP                    0x00000003 /* RW--V */
#define NV_CIO_CRE_PAGE_OVFL__INDEX                      0x00000029 /*       */
#define NV_CIO_CRE_OSCOL__INDEX                          0x0000002A /*       */
#define NV_CIO_CRE_SCRATCH0__INDEX                       0x0000002B /*       */
#define NV_CIO_CRE_SCRATCH1__INDEX                       0x0000002C /*       */
#define NV_CIO_CRE_HEB__INDEX                            0x0000002D /*       */
#define NV_CIO_CRE_HEB_SA_25                                    7:7 /* RW--F */
#define NV_CIO_CRE_HEB_SA_24                                    6:6 /* RW--F */
#define NV_CIO_CRE_HEB_SA_23                                    5:5 /* RW--F */
#define NV_CIO_CRE_HEB_ILC_8                                    4:4 /* RW--F */
#define NV_CIO_CRE_HEB_HRS_8                                    3:3 /* RW--F */
#define NV_CIO_CRE_HEB_HBS_8                                    2:2 /* RW--F */
#define NV_CIO_CRE_HEB_HDE_8                                    1:1 /* RW--F */
#define NV_CIO_CRE_HEB_HDT_8                                    0:0 /* RW--F */
#define NV_CIO_CRE_HCUR_ADDR2_INDEX                      0x0000002f /*       */
#define NV_CIO_CRE_HCUR_ADDR2_ADR                               2:0 /* RW--F */
#define NV_CIO_CRE_HCUR_ADDR0_INDEX                      0x00000030 /*       */
#define NV_CIO_CRE_HCUR_ASI                                     7:7 /* RW--F */
#define NV_CIO_CRE_HCUR_ASI_FRAMEBUFFER                  0x00000001
#define NV_CIO_CRE_HCUR_ASI_INSTMEM                      0x00000000
#define NV_CIO_CRE_HCUR_ADDR0_ADR                               6:0 /* RW--F */
#define NV_CIO_CRE_HCUR_ADDR1_INDEX                      0x00000031 /*       */
#define NV_CIO_CRE_HCUR_ADDR1_ADR                               7:2 /* RW--F */
#define NV_CIO_CRE_HCUR_ADDR1_CUR_DBL                           1:1 /* RW--F */
#define NV_CIO_CRE_HCUR_ADDR1_ENABLE                            0:0 /* RW--F */
#define NV_CIO_CRE_VID_END0__INDEX                       0x00000032 /*       */
#define NV_CIO_CRE_LCD__INDEX                            0x00000033 /*       */
#define NV_CIO_CRE_LCD_LCD_SELECT                               0:0 /* RW--F */
#define NV_CIO_CRE_LCD_VPLL_VCO                                 6:6 /* RW--F */
#define NV_CIO_CRE_LCD_STEREO_ENABLE                            7:7 /* RW--F */
#define NV_CIO_CRE_RMA__INDEX                            0x00000038 /*       */
#define NV_CIO_CRE_ILACE__INDEX                          0x00000039 /*       */
#define NV_CIO_CRE_SCRATCH2__INDEX                       0x0000003A /*       */
#define NV_CIO_CRE_SCRATCH3__INDEX                       0x0000003B /*       */
#define NV_CIO_CRE_SCRATCH4__INDEX                       0x0000003C /*       */
#define NV_CIO_CRE_TREG__INDEX                           0x0000003D /*       */
#define NV_CIO_CRE_TREG_HCNT                                    6:6 /* RW--F */
#define NV_CIO_CRE_TREG_VCNT                                    4:4 /* RW--F */
#define NV_CIO_CRE_TREG_SHADOW                                  0:0 /* RW--F */
#define NV_CIO_CRE_TREG_HCNT_INDEX                       0x00000000 /*       */
#define NV_CIO_CRE_TREG_VCNTA_INDEX                      0x00000006 /*       */
#define NV_CIO_CRE_TREG_VCNTB_INDEX                      0x00000007 /*       */
#define NV_CIO_CRE_DDC_STATUS__INDEX                     0x0000003E /*       */
#define NV_CIO_CRE_DDC_WR__INDEX                         0x0000003F /*       */
#define NV_CIO_CRE_PCI_TO__INDEX                         0x00000040 /*       */
#define NV_CIO_CRE_PCI_TO_DELAY                                 7:0 /* -W--F */
 /* these are nv20-only */
#define NV_CIO_CRE_EBR_INDEX                             0x00000041 /*       */
#define NV_CIO_CRE_EBR_VBS_11                                   6:6 /* RW--F */
#define NV_CIO_CRE_EBR_VRS_11                                   4:4 /* RW--F */
#define NV_CIO_CRE_EBR_VDE_11                                   2:2 /* RW--F */
#define NV_CIO_CRE_EBR_VDT_11                                   0:0 /* RW--F */
#define NV_CIO_CRE_USA_INDEX                             0x00000042 /*       */
#define NV_CIO_CRE_USA_SA__31                                   3:3 /* RW--F */
#define NV_CIO_CRE_USA_SA__30                                   2:2 /* RW--F */
#define NV_CIO_CRE_USA_SA__29                                   1:1 /* RW--F */
#define NV_CIO_CRE_USA_SA__28                                   0:0 /* RW--F */
#define NV_CIO_CRE_MBI                                   0x00000043 /*       */
#define NV_CIO_CRE_MBI_EN                                       0:0 /* RW--F */
#define NV_CIO_CRE_MBI_EN_ENABLE                         0x00000001 /* RW--V */
#define NV_CIO_CRE_MBI_EN_DISABLE                        0x00000000 /* RW--V */
#define NV_CIO_CRE_H2OWNS                                0x00000044 /*       */
#define NV_CIO_CRE_H2OWNS_VGA                                   0:0 /* RW--F */
#define NV_CIO_CRE_H2OWNS_VGA_DEFAULT                    0x00000000 /* RWI-V */
#define NV_CIO_CRE_H2OWNS_VGA_DISABLE                    0x00000000 /* RW--V */
#define NV_CIO_CRE_H2OWNS_VGA_ENABLE                     0x00000001 /* RW--V */
#define NV_CIO_CRE_H2OWNS_DAC                                   1:1 /* RW--F */
#define NV_CIO_CRE_H2OWNS_DAC_DEFAULT                    0x00000000 /* RWI-V */
#define NV_CIO_CRE_H2OWNS_DAC_DISABLE                    0x00000000 /* RW--V */
#define NV_CIO_CRE_H2OWNS_DAC_ENABLE                     0x00000001 /* RW--V */
#define NV_CIO_CRE_H2OWNS_MIRROR                                2:2 /* RW--F */
#define NV_CIO_CRE_H2OWNS_MIRROR_DEFAULT                 0x00000000 /* RWI-V */
#define NV_CIO_CRE_H2OWNS_MIRROR_DISABLE                 0x00000000 /* RW--V */
#define NV_CIO_CRE_H2OWNS_MIRROR_ENABLE                  0x00000001 /* RW--V */
#define NV_CIO_CRE_CSB                                   0x00000045 /*       */
#define NV_CIO_CRE_CSB_VAL                                      1:0 /* RW--F */
#define NV_CIO_CRE_CSB_VAL_NONE                          0x00000000 /* RW--V */
#define NV_CIO_CRE_CSB_VAL_3BY16                         0x00000001 /* RW--V */
#define NV_CIO_CRE_CSB_VAL_3BY08                         0x00000002 /* RW--V */
#define NV_CIO_CRE_CSB_VAL_3BY04                         0x00000003 /* RW--V */
#define NV_CIO_CRE_RCR                                   0x00000046 /*       */
#define NV_CIO_CRE_RCR_RNDM_REQ                                 1:0 /* RW--F */
#define NV_CIO_CRE_RCR_RNDM_REQ_NONE                     0x00000000 /* RW--V */
#define NV_CIO_CRE_RCR_RNDM_REQ_08                       0x00000001 /* RW--V */
#define NV_CIO_CRE_RCR_RNDM_REQ_16                       0x00000002 /* RW--V */
#define NV_CIO_CRE_RCR_RNDM_REQ_32                       0x00000003 /* RW--V */
#define NV_CIO_CRE_RCR_ENDIAN                                   7:7 /* RW--F */
#define NV_CIO_CRE_RCR_ENDIAN_LITTLE                     0x00000000 /* RW--V */
#define NV_CIO_CRE_RCR_ENDIAN_BIG                        0x00000001 /* RW--V */
#define NV_CIO_CRE_FFLWM_MSB_INDEX                       0x00000047 /*       */
#define NV_CIO_CRE_FFLWM_MSB_LWM                                0:0 /* RW--F */
#define NV_CIO_CRE_EXT_PIXEL_INDEX                       0x00000048 /*       */
#define NV_CIO_CRE_EXT_PIXEL_FORMAT                             0:0 /* RW--F */
#define NV_CIO_CRE_EXT_PIXEL_FORMAT_RGB                  0x00000000 /* RWI-V */
#define NV_CIO_CRE_EXT_PIXEL_FORMAT_YUV                  0x00000001 /* RW--V */
#define NV_CIO_CRE_EXT_PIXEL_YUV_FMT                            1:1 /* RW--F */
#define NV_CIO_CRE_EXT_PIXEL_YUV_FMT_YVYU                0x00000000 /* RWI-V */
#define NV_CIO_CRE_EXT_PIXEL_YUV_FMT_VYUY                0x00000001 /* RW--V */
#define NV_CIO_CRE_ENGINE_CTRL                           0x00000049 /*       */
#define NV_CIO_CRE_ENGINE_CTRL_GPIO                             0:0 /* RW--F */
#define NV_CIO_CRE_ENGINE_CTRL_GPIO_DEFAULT              0x00000000 /* RWI-V */
#define NV_CIO_CRE_ENGINE_CTRL_GPIO_ENABLE               0x00000001 /* RW--V */
#define NV_CIO_CRE_ENGINE_CTRL_I2C                              2:2 /* RW--F */
#define NV_CIO_CRE_ENGINE_CTRL_I2C_DEFAULT               0x00000000 /* RWI-V */
#define NV_CIO_CRE_ENGINE_CTRL_I2C_ENABLE                0x00000001 /* RW--V */
#define NV_CIO_CRE_ENGINE_CTRL_TV                               4:4 /* RW--F */
#define NV_CIO_CRE_ENGINE_CTRL_TV_DEFAULT                0x00000000 /* RWI-V */
#define NV_CIO_CRE_ENGINE_CTRL_TV_ENABLE                 0x00000001 /* RW--V */
#define NV_CIO_CRE_ENGINE_CTRL_VS                               6:6 /* RW--F */
#define NV_CIO_CRE_ENGINE_CTRL_VS_DEFAULT                0x00000000 /* RWI-V */
#define NV_CIO_CRE_ENGINE_CTRL_VS_ENABLE                 0x00000001 /* RW--V */
#define NV_CIO_CRE_SCRATCH5__INDEX                       0x0000004A /*       */
#define NV_CIO_CRE_SCRATCH6__INDEX                       0x0000004B /*       */
#define NV_CIO_CRE_SCRATCH7__INDEX                       0x0000004C /*       */
#define NV_CIO_GPIO23_INDEX                              0x0000004D /*       */
#define NV_CIO_GPIO45_INDEX                              0x0000004E /*       */
#define NV_CIO_CRE_DDC2_STATUS__INDEX                    0x00000050 /*       */
#define NV_CIO_CRE_DDC2_WR__INDEX                        0x00000051 /*       */
#define NV_CIO_CRE_TVOUT_LATENCY                         0x00000052 /*       */
/* dev_vga.ref */
#define NV_VIO_MBEN                                      0x00000094 /* RW-1R */
#define NV_VIO_ADDEN                                     0x000046e8 /* RW-1R */
#define NV_VIO_VSE1                                      0x00000102 /* RW-1R */
#define NV_VIO_VSE2                                      0x000003c3 /* RW-1R */
#define NV_VIO_MISC__READ                                0x000003cc /* R--1R */
#define NV_VIO_MISC__WRITE                               0x000003c2 /* -W-1R */
#define NV_VIO_SRX                                       0x000003c4 /* RW-1R */
#define NV_VIO_SR_RESET                                  0x000003c5 /* RW-1R */
#define NV_VIO_SR_RESET_INDEX                            0x00000000 /*     V */
#define NV_VIO_SR_CLOCK                                  0x000003c5 /* RW-1R */
#define NV_VIO_SR_CLOCK_INDEX                            0x00000001 /*     V */
#define NV_VIO_SR_PLANE_MASK                             0x000003c5 /* RW-1R */
#define NV_VIO_SR_PLANE_MASK_INDEX                       0x00000002 /*     V */
#define NV_VIO_SR_CHAR_MAP                               0x000003c5 /* RW-1R */
#define NV_VIO_SR_CHAR_MAP_INDEX                         0x00000003 /*       */
#define NV_VIO_SR_MEM_MODE                               0x000003c5 /* RW-1R */
#define NV_VIO_SR_MEM_MODE_INDEX                         0x00000004 /*       */
#define NV_VIO_GRX                                       0x000003ce /* RW-1R */
#define NV_VIO_GX_SR                                     0x000003cf /* RW-1R */
#define NV_VIO_GX_SR_INDEX                               0x00000000 /*       */
#define NV_VIO_GX_SREN                                   0x000003cf /* RW-1R */
#define NV_VIO_GX_SREN_INDEX                             0x00000001 /*       */
#define NV_VIO_GX_CCOMP                                  0x000003cf /* RW-1R */
#define NV_VIO_GX_CCOMP_INDEX                            0x00000002 /*       */
#define NV_VIO_GX_ROP                                    0x000003cf /* RW-1R */
#define NV_VIO_GX_ROP_INDEX                              0x00000003 /*       */
#define NV_VIO_GX_READ_MAP                               0x000003cf /* RW-1R */
#define NV_VIO_GX_READ_MAP_INDEX                         0x00000004 /*       */
#define NV_VIO_GX_MODE                                   0x000003cf /* RW-1R */
#define NV_VIO_GX_MODE_INDEX                             0x00000005 /*       */
#define NV_VIO_GX_MISC                                   0x000003cf /* RW-1R */
#define NV_VIO_GX_MISC_INDEX                             0x00000006 /*       */
#define NV_VIO_GX_DONT_CARE                              0x000003cf /* RW-1R */
#define NV_VIO_GX_DONT_CARE_INDEX                        0x00000007 /*       */
#define NV_VIO_GX_BIT_MASK                               0x000003cf /* RW-1R */
#define NV_VIO_GX_BIT_MASK_INDEX                         0x00000008 /*       */
/* dev_vga.ref */
#define NV_PRMVIO                             0x000C7FFF:0x000C0000 /* RW--D */
#define NV_PRMVIO_MBEN                                   0x000C0094 /* RW-1R */
#define NV_PRMVIO_ADDEN                                  0x000C46e8 /* RW-1R */
#define NV_PRMVIO_VSE1                                   0x000C0102 /* RW-1R */
#define NV_PRMVIO_VSE2                                   0x000C03c3 /* RW-1R */
#define NV_PRMVIO_MISC__READ                             0x000C03cc /* R--1R */
#define NV_PRMVIO_MISC__WRITE                            0x000C03c2 /* -W-1R */
#define NV_PRMVIO_SRX                                    0x000C03c4 /* RW-1R */
#define NV_PRMVIO_SR_RESET                               0x000C03c5 /* RW-1R */
#define NV_PRMVIO_SR_RESET_INDEX                         0x00000000 /*     V */
#define NV_PRMVIO_SR_CLOCK                               0x000C03c5 /* RW-1R */
#define NV_PRMVIO_SR_CLOCK_INDEX                         0x00000001 /*     V */
#define NV_PRMVIO_SR_PLANE_MASK                          0x000C03c5 /* RW-1R */
#define NV_PRMVIO_SR_PLANE_MASK_INDEX                    0x00000002 /*     V */
#define NV_PRMVIO_SR_CHAR_MAP                            0x000C03c5 /* RW-1R */
#define NV_PRMVIO_SR_CHAR_MAP_INDEX                      0x00000003 /*       */
#define NV_PRMVIO_SR_MEM_MODE                            0x000C03c5 /* RW-1R */
#define NV_PRMVIO_SR_MEM_MODE_INDEX                      0x00000004 /*       */
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
/* dev_vga.ref */
#define NV_PRMVGA                             0x000BFFFF:0x000A0000 /* RW--D */
/* dev_framebuffer.ref */
/* dev_fb.ref */
#define NV_PFB                                0x00100FFF:0x00100000 /* RW--D */
/*  needed by dma/dma.c:dmaDeallocate() */
#define NV_PRAMIN_CTX_1_NOTIFY_INSTANCE                       31:16 /* RWXUF */
#define NV_PRAMIN_CTX_2_DMA_0_INSTANCE                         15:0 /* RWXUF */
#define NV_PRAMIN_CTX_2_DMA_1_INSTANCE                        31:16 /* RWXUF */
/*  needed by osapi.c/fbstate.c/mcstate.c/dac.c */
#define NV_PRAMIN_DATA032(i)                     (0x00700000+(i)*4) /* RW-4A */
#define NV_PRAMIN_DATA016(i)         (0x00700000+((i)/3)*4+((i)%3)) /* RW-2A */
#define NV_PRAMIN_DATA008(i)                       (0x00700000+(i)) /* RW-1A */
/*  needed by gr/gr.c:grDeleteObjectInstance() */
#define NV_PRAMIN_CONTEXT_0                   ( 0*32+31):( 0*32+ 0) /*       */
#define NV_PRAMIN_CONTEXT_1                   ( 1*32+31):( 1*32+ 0) /*       */
#define NV_PRAMIN_CONTEXT_2                   ( 2*32+31):( 2*32+ 0) /*       */
/* dev_ext_devices.ref */
#define NV_PEXTDEV                            0x00101FFF:0x00101000 /* RW--D */
#define NV_PEXTDEV_BOOT_0                                0x00101000 /* R--4R */
#define NV_PEXTDEV_BOOT_0_STRAP_PCI_AD                          0:0 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_PCI_AD_REVERSED          0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_PCI_AD_NORMAL            0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR                      1:1 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR_NO_BIOS       0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_SUB_VENDOR_BIOS          0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_RAMCFG                          5:2 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL                         6:6 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_13500K           0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_14318180         0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_TVMODE                          8:7 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_TVMODE_SECAM             0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_TVMODE_NTSC              0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_TVMODE_PAL               0x00000002 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_TVMODE_DISABLED          0x00000003 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_AGP_4X                          9:9 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_AGP_4X_ENABLED           0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_AGP_4X_DISABLED          0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_AGP_SBA                       10:10 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_AGP_SBA_ENABLED          0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_AGP_SBA_DISABLED         0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_AGP_FASTWR                    11:11 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_AGP_FASTWR_ENABLED       0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_AGP_FASTWR_DISABLED      0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_PCI_DEVID                     13:12 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_PCI_DEVID_0              0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_PCI_DEVID_1              0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_PCI_DEVID_2              0x00000002 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_PCI_DEVID_3              0x00000003 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE                      14:14 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE_PCI             0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BUS_TYPE_AGP             0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_FP_IFACE                      15:15 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_FP_IFACE_24BIT           0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_FP_IFACE_12BIT           0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_FB                            17:16 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_FB_64M                   0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_FB_128M                  0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_FB_256M                  0x00000002 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_FB_512M                  0x00000003 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BR                            18:18 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_BR_DISABLED              0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BR_ENABLED               0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BR_REG_128M                   19:19 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_BR_REG_128M_ENABLED      0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BR_REG_128M_DISABLED     0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BR_AGP_DEV                    20:20 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_BR_AGP_DEV_DISABLED      0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BR_AGP_DEV_ENABLED       0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BR_IO_DEV                     21:21 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_BR_IO_DEV_DISABLED       0x00000000 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_BR_IO_DEV_ENABLED        0x00000001 /* R---V */
#define NV_PEXTDEV_BOOT_0_STRAP_USER_MOBILE                   21:16 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_USER                          30:22 /* R-XVF */
#define NV_PEXTDEV_BOOT_0_STRAP_OVERWRITE                     31:31 /* RWIVF */
#define NV_PEXTDEV_BOOT_0_STRAP_OVERWRITE_DISABLED       0x00000000 /* RWI-V */
#define NV_PEXTDEV_BOOT_0_STRAP_OVERWRITE_ENABLED        0x00000001 /* RW--V */
#define NV_PEXTDEV_NEW_BOOT_0                            0x00101000 /* RW-4R */
#define NV_PEXTDEV_NEW_BOOT_0_STRAP_VALUE                      30:0 /* RWIVF */
#define NV_PEXTDEV_NEW_BOOT_0_STRAP_OVERWRITE                 31:31 /* RWIVF */
#define NV_PEXTDEV_NEW_BOOT_0_STRAP_OVERWRITE_DISABLED   0x00000000 /* RWI-V */
#define NV_PEXTDEV_NEW_BOOT_0_STRAP_OVERWRITE_ENABLED    0x00000001 /* RW--V */
/* dev_ext_devices.ref */
/*  needed by win9x/vxd/osinit.c */
#define NV_PROM                               0x0030FFFF:0x00300000 /* RW--D */
#define NV_PROM_DATA(i)                            (0x00300000+(i)) /* RW-1A */
#define NV_PROM_DATA__SIZE_1                                  65536 /*       */
#define NV_PROM_DATA_VALUE                                      7:0 /* RW-VF */
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

#endif /* _NV_REG_H_ */

#endif /* NV3_HW */

#endif // _NV_REF_H_
