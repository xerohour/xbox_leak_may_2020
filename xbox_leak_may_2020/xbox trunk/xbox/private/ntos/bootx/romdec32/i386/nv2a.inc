;
; NV2A ROM table ptr (NV20 Register Base/Limit (BAR0)
;
NV2A_XBOX_ROM_TABLE_PTR          EQU 0FF000008h

;
; Datatbl starts at this offset
;
; IMPORTANT: This must be adjusted if the size of the init table
; code has changed
;
IFDEF SYS_RETAIL
DATATBL_BASE_OFFSET              EQU 0C90h
ELSE
DATATBL_BASE_OFFSET              EQU 0BF0h
ENDIF

;
; Integrated NV20 Register Base (BAR0)
;
NV20_REG_BASE                    EQU 00F000000h    ; only during the init tbl
NV20_REG_BASE_KERNEL             EQU 0FD000000h    ; reloaded to agree with kernel code
NV20_FB_BASE                     EQU 0F0000000h
APERTURE_BASE                    EQU 040000000h

SMB_CONTROL_BASE                 EQU 00000C000h
SMB_DDC_SPIO_BASE                EQU 00000C200h

;
; NV2A config equates
;
CR_CPU_MEMTOP                    EQU 080000084h
CR_CPU_MEMTOP_LIMIT_64MB         EQU 003FFFFFFh
CR_CPU_MEMTOP_LIMIT_128MB        EQU 007FFFFFFh

CR_CPU_CST_0                     EQU 080000340h
CR_CPU_CST_1                     EQU 080000344h
CR_CPU_CST_7                     EQU 08000035Ch

CR_CPU_MPLL_COEFF                EQU 08000036Ch

CR_APC_P2P_1                     EQU 08000F004h
CR_APC_P2P_1_BUS_MSTR_MEM_IO     EQU 000000007h    ; should be RMW, still ok?

CR_APC_P2P_6                     EQU 08000F018h
CR_APC_P2P_6_SUB_SEC_PRI_BUS     EQU 000010100h    ; change CR_NV20, if bus changes

CR_APC_P2P_8                     EQU 08000F020h
CR_APC_P2P_8_MLIMIT_MBASE_INIT   EQU (NV20_REG_BASE + 0F00000h) OR (NV20_REG_BASE SHR 16)
CR_APC_P2P_8_MLIMIT_MBASE_KERNEL EQU (NV20_REG_BASE_KERNEL + 0F00000h) OR (NV20_REG_BASE_KERNEL SHR 16)
CR_APC_P2P_9                     EQU 08000F024h
CR_APC_P2P_9_PREF_MLIMIT_MBASE   EQU (NV20_FB_BASE +  7F00000h) OR (NV20_FB_BASE SHR 16)

CR_APC_P2P_19                    EQU 08000F04Ch
CR_APC_P2P_19_NV20_ENABLE        EQU 000000001h    ; enables internal graphics

;
; MCP config equates
;
MCP_LEG_CFG_1                    EQU 080000804h    ; enable IO space
MCP_LEG_CFG_2                    EQU 080000808h    ; holds MCP revision ID

IFDEF MCP_REV_C03
MCP_LEG_CFG_4                    EQU 080000884h    ; IO BAR for revs >= C03
ELSE
MCP_LEG_CFG_4                    EQU 080000810h    ; IO BAR for revs < C03
ENDIF

MCP_LEG_CFG_19                   EQU 08000084Ch
MCP_LEG_CFG_24                   EQU 080000860h
MCP_LEG_CFG_39                   EQU 08000089Ch
MCP_LEG_CFG_45                   EQU 0800008B4h

MCP_SMB_CFG_1                    EQU 080000904h
MCP_SMB_CFG_1_IO_SPACE           EQU 000000001h    ; should be RMW, still ok?

MCP_SMB_CFG_5                    EQU 080000914h
MCP_SMB_CFG_5_IO_BASE            EQU SMB_CONTROL_BASE + 1

MCP_SMB_CFG_6                    EQU 080000918h
MCP_SMB_CFG_6_IO_BASE            EQU SMB_DDC_SPIO_BASE + 1

MCP_USBA_CFG_15                  EQU 08000103Ch    ; scratch pad for meminit
MCP_USBB_CFG_15                  EQU 08000183Ch    ; scratch pad for meminit

;
; MCP registers used as scratchpad for memory test
;
MEMTEST_TYPE                     EQU 08000103Ch    ; MCP_USBA_CFG_15 
MEMTEST_RESULT                   EQU 08000183Ch    ; MCP_USBB_CFG_15 

;
; Memory test patterns
;
MEMTEST_PATTERN1                 EQU 0AAAAAAAAh
MEMTEST_PATTERN2                 EQU 05A5A5A5Ah
MEMTEST_PATTERN3                 EQU 055555555h
MEMTEST_PATTERN4                 EQU 0CCCCCCCCh


; Internal NV20 config equates (assumes bus 1)
;
CR_NV20_PCI_1                    EQU 080010004h
CR_NV20_PCI_1_BUS_MSTR_MEM_IO    EQU 000000007h    ; should be RMW, still ok?

CR_NV20_PCI_4                    EQU 080010010h
CR_NV20_PCI_5                    EQU 080010014h

;
; NV20 register offsets and init values (from nv2a\manuals\rom_table.txt)
;
NV_PBUS_DEBUG_CTRIM_0            EQU 0000010B0h
NV_PBUS_DEBUG_CTRIM_1            EQU 0000010B4h
NV_PBUS_DEBUG_CTRIM_2            EQU 0000010B8h
NV_PBUS_DEBUG_CTRIM_3            EQU 0000010BCh
NV_PBUS_DEBUG_CTRIM_4            EQU 0000010C4h
NV_PBUS_DEBUG_CTRIM_5            EQU 0000010C8h
NV_PBUS_DEBUG_CTRIM_6            EQU 0000010CCh
NV_PBUS_DEBUG_CTRIM_7            EQU 0000010D4h
NV_PBUS_DEBUG_CTRIM_8            EQU 0000010D8h
NV_PBUS_DEBUG_CTRIM_9            EQU 0000010DCh
NV_PBUS_DEBUG_CTRIM_10           EQU 0000010E8h

NV_PBUS_FBIO_CFG                 EQU 000001210h

NV_PBUS_FBIO_CALEN               EQU 000001220h
NV_PBUS_FBIO_CALEN_OFF           EQU 000000000h

NV_PBUS_FBIO_CALSEL              EQU 000001228h
NV_PBUS_FBIO_CALSEL_VALUE        EQU 000000000h

NV_PBUS_FBIO_DLY                 EQU 000001214h

NV_PBUS_FBIO_ADRDRV              EQU 00000122Ch
NV_PBUS_FBIO_CLKDRV              EQU 000001230h
NV_PBUS_FBIO_DATDRV              EQU 000001234h
NV_PBUS_FBIO_DATDRV_INIT         EQU 0AAAAAAAAh
NV_PBUS_FBIO_DQSDRV              EQU 000001238h
NV_PBUS_FBIO_DQSDRV_INIT         EQU 0AAAAAAAAh
NV_PBUS_FBIO_ADRSLW              EQU 00000123Ch
NV_PBUS_FBIO_ADRSLW_INIT         EQU 08B8B8B8Bh
NV_PBUS_FBIO_CLKSLW              EQU 000001240h
NV_PBUS_FBIO_DATSLW              EQU 000001244h
NV_PBUS_FBIO_DATSLW_INIT         EQU 08B8B8B8Bh
NV_PBUS_FBIO_DQSSLW              EQU 000001248h
NV_PBUS_FBIO_DQSSLW_INIT         EQU 08B8B8B8Bh
NV_PBUS_DISPIO_PADCTL            EQU 00000124Ch
NV_PBUS_DISPIO_PADCTL_INIT       EQU 0AA8BAA8Bh
NV_PBUS_TVDIO_PADCTL             EQU 000001250h
NV_PBUS_TVDIO_PADCTL_INIT        EQU 00000AA8Bh

NV_PBUS_TVDIO_CALEN              EQU 000001264h
NV_PBUS_TVDIO_CALEN_OFF          EQU 000000000h

NV_PEXTDEV_BOOT_0                EQU 000101000h

NV_PFB_REF                       EQU 0001002D0h
NV_PFB_PRE                       EQU 0001002D4h
NV_PFB_PRE_CMD_PRECHARGE_1       EQU 000000001h
NV_PFB_EMRS                      EQU 0001002C4h
NV_PFB_EMRS_EXT                  EQU 0001002CCh
NV_PFB_MRS                       EQU 0001002C0h
NV_PFB_MRS_DLL_RESET             EQU 000000132h
NV_PFB_MRS_EXT                   EQU 0001002C8h
NV_PFB_MRS_EXT_DLL_RESET         EQU 000000132h
NV_PFB_REFCTRL                   EQU 000100210h
NV_PFB_REFCTRL_VALID_1           EQU 080000000h
NV_PFB_WBC                       EQU 000100410h
NV_PFB_CPU_RRQ                   EQU 000100420h
NV_PFB_CPU_RRQ_FWP_LAT_ENABLE    EQU 000000401h
NV_PFB_BYPASS                    EQU 000100424h
NV_PFB_BYPASS_VALUE              EQU 0F1780031h

NV_PMC_BOOT_0                    EQU 000000000h

NV_PRAMDAC_PLL_COEFF_SELECT      EQU 00068050Ch
NV_PFB_CFG0                      EQU 000100200h
NV_PFB_CFG1                      EQU 000100204h

NV_PFB_TIMING2                   EQU 000100228h

NV_PFB_ARB_XFER_SZ               EQU 000100330h
NV_PFB_ARB_TIMEOUT               EQU 00010032Ch
NV_PFB_ARB_PREDIVIDER            EQU 000100328h
NV_PFB_ARB_DIFF_BANK             EQU 000100338h

NV_PBUS_FBIO_RAM                 EQU 000001218h

NV_PRAMDAC_NVPLL_COEFF           EQU 000680500h

NV_PGRAPH_DEBUG_2                EQU 000400880h
NV_PGRAPH_DEBUG_2_VALUE          EQU 0002EC3FFh
NV_PGRAPH_DEBUG_10               EQU 000400b88h
NV_PGRAPH_DEBUG_10_ROP_BLEND     EQU 000000003h

;
; Equates used for the memory detect/test
;
MEM_PART_0                       EQU 000000000h
MEM_PART_1                       EQU 000000010h
MEM_PART_2                       EQU 000000020h
MEM_PART_3                       EQU 000000030h
MEM_PART_4                       EQU 004000000h
MEM_PART_5                       EQU 004000010h
MEM_PART_6                       EQU 004000020h
MEM_PART_7                       EQU 004000030h


;
; First byte of the desired SMC revision
;
DESIRED_SMC_VER                  EQU 050h




