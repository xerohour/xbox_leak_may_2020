//
// FILE:      library\hardware\pcibrdge\specific\plx9054\p9054reg.h
// AUTHOR:    Martin Stephan
// COPYRIGHT: (c) 1999 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   23.11.1999
//
// PURPOSE:   PLX 9054 register specific definitions --- interface
//
// HISTORY:
//

#ifndef P9054REG_H
#define P9054REG_H

////////////////////////////////////////////
// PCI CONFIGURATION REGISTERS				//
////////////////////////////////////////////
// Definitions for the PLX Configuration space registers.
// All bits numbers are starting from 0.
#define PLX9054_PCI_CONFIGURATION_ID			0x00
#define PLX9054_PCI_VENDOR_ID						0x00
#define PLX9054_PCI_DEVICE_ID						0x02
#define PLX9054_PCI_COMMAND						0x04
#define PLX9054_PCI_STATUS							0x06
#define PLX9054_PCI_REVISION_ID					0x08
#define PLX9054_PCI_CLASS_CODE					0x09
#define PLX9054_PCI_CACHE_LINE_SIZE				0x0C
#define PLX9054_PCI_BUS_LATENCY_TIMER			0x0D
#define PLX9054_PCI_HEADER_TYPE					0x0E
#define PLX9054_PCI_BUILT_IN_SELF_TEST			0x0F
#define PLX9054_PCI_BASE_ADDRESS_0				0x10
#define PLX9054_PCI_BASE_ADDRESS_1				0x14
#define PLX9054_PCI_BASE_ADDRESS_2				0x18
#define PLX9054_PCI_BASE_ADDRESS_3				0x1C
#define PLX9054_PCI_BASE_ADDRESS_4				0x20
#define PLX9054_PCI_BASE_ADDRESS_5				0x24
#define PLX9054_PCI_CRD_BUS_CIS_POINTER		0x28
#define PLX9054_PCI_SUBSYSTEM_VENDOR_ID		0x2C					// Subsystem ID
#define SUBSYSTEM_ID_SHIFT								16					// Subsystem Vendor ID
#define PLX9054_PCI_SUBSYSTEM_DEVICE_ID		0x2E
#define PLX9054_PCI_EXPANSION_ROM_BASE			0x30
#define PLX9054_PCI_NEW_CAPABILITY_POINTER	0x34
#define PLX9054_PCI_INTERRUPT_LINE				0x3C
#define PLX9054_PCI_INTERRUPT_PIN				0x3D
#define PLX9054_PCI_MIN_GRANT						0x3E
#define PLX9054_PCI_MAX_LATENCY					0x3F
#define PLX9054_PCI_POWER_MANAGEMENT_CAPABILITY_ID					0x40
#define PLX9054_PCI_POWER_MANAGEMENT_NEXT_CAPABILITY_POINTER	0x41
#define PLX9054_PCI_POWER_MANAGEMENT_CAPABILITIES					0x42
#define PLX9054_PCI_POWER_MANAGEMENT_CONTROL_STATUS				0x44
#define PLX9054_PCI_BRIDGE_SUPPORT_EXTENSIONS						0x46
#define PLX9054_PCI_POWER_MANAGEMENT_DATA								0x47
#define PLX9054_PCI_HOT_SWAP_CONTROL									0x48
#define HOT_SWAP_LED																0x00080000
#define PLX9054_PCI_HOT_SWAP_NEXT_CAPABILITY_POINTER				0x49
#define PLX9054_PCI_HOT_SWAP_CONTROL_STATUS							0x4A
#define PLX9054_IDX_LED_ON_OFF												3
#define PLX9054_PCI_VITAL_PRODUCT_DATA_CONTROL						0x4C
#define VPD_ADDRESS_SHIFT														16
#define VPD_ADDRESS																0x7FFF0000
#define VPD_FLAG																	0x80000000
#define PLX9054_PCI_VITAL_PRODUCT_DATA_NEXT_CAPABILITY_POINTER	0x4D
#define PLX9054_PCI_VITAL_PRODUCT_DATA_ADRESS						0x4E
#define PLX9054_PCI_VPD_DATA												0x50


////////////////////////////////////////////
// LOCAL CONFIGURATION REGISTERS				//
////////////////////////////////////////////
// Definitions for the PLX local internal registers.	
#define PLX9054_LAS0RR_OFFSET         0x00						// Range for PCI-to-Local Address Space 0

#define PLX9054_LB0_BASE_ADR          0x04						// Local Base Address (Remap) for PCI-to-Local Address Space 0
#define SPACE0_ENABLED                      0x00000001
#define LB_SPACE0_ADDRESS_MASK              0xFFFFFFF0

#define PLX9054_MARBR_OFFSET          0x08						// Mode/DMA Arbitration
#define DIRECT_SLAVE_LOCK_ENABLE            (1 << 22)			// 22 bit
#define PRIORITY_MASK                       0x00180000
#define PRIORITY_DMA_0                      0x00100000

// 
#define PLX9054_BIGEND_LMISC_PROTAREA 0x0C						// Serial EEPROM Write-Protected address boundary *
																				// Local miscellaneous control +
																				// Big/Little endian descriptor
#define PROT_AREA_SHIFT                     16					
#define PROT_AREA_MASK                      0x007F0000
#define BIGEND_DIRECT_MASTER                0x00000002
#define BIGEND_SPACE_1                      (1 << 5)			// 5 bit
#define BIGEND_DMA0                         (1 << 7)

#define PLX9054_LBRD0_OFFSET          0x18						// local bus region descriptors for PCI-to-Local address
#define SPACE0_BUSWIDTH8                    0x00000000
#define SPACE0_BUSWIDTH16                   0x00000001
#define SPACE0_BUSWIDTH32                   0x00000003
#define SPACE0_READY_INPUT_ENABLE           0x00000040
#define SPACE0_BTERM_INPUT_ENABLE           0x00000080
#define SPACE0_PREFETCH_DISABLE             0x00000100
#define SPACE0_BURST_ENABLE                 0x01000000
#define SPACE0_PREFETCH_COUNT_ENABLE        0x00000400
#define PCI_TARGET_RETRY_DELAY_MAX          0xF0000000      /* 15 * 4 PCI clocks */


#define EROM_BUSWIDTH8                      0x00000000
#define EROM_BUSWIDTH16                     0x00010000
#define EROM_BUSWIDTH32                     0x00030000
#define EROM_READY_INPUT_ENABLE             0x00400000
#define EROM_BTERM_INPUT_ENABLE             0x00800000
#define EROM_PREFETCH_DISABLE               0x00000200
#define EROM_BURST_ENABLE                   0x04000000
#define EROM_PREFETCH_COUNT_ENABLE          0x00000400

#define PLX9054_LB_PCI_RANGE          0x1C						// Range for PCI initiator-to-PCI
#define PLX9054_DMRR_OFFSET           PLX9054_LB_PCI_RANGE
#define LB_PCI_RANGE_MASK                   0xFFFF0000

#define PLX9054_LB_BASE_ADR           0x20						// local base address for PCI initiator-to-PCI memory
#define PLX9054_DMLBAM_OFFSET         PLX9054_LB_BASE_ADR
#define LB_BASE_ADR_MASK                    0xFFFF0000

#define PLX9054_DMLBA1_OFFSET         0x24						// local base address for PCI initiator-to-PCI I/O configuration

#define PLX9054_PCI_BASE_ADR          0x28						// PCI base address for PCI initiator-to-PCI
#define PLX9054_DMPBAM_OFFSET         PLX9054_PCI_BASE_ADR 
#define LB_PCI_MEM_ENABLE                   0x00000001
#define LB_PCI_IO_ENABLE                    0x00000002
#define PCI_PREFETCH_SIZE_INF               0x00001008
#define PCI_KEEP_BUS                        0x00000010
#define USE_PCI_BASE_ADR                    0x00002000
#define PCI_BASE_ADR_MASK                   0xFFFF0000

////////////////////////////////////////////
// RUNTIME REGISTERS								//
////////////////////////////////////////////
#define PLX9054_MBOX0							0x40
#define PLX9054_MBOX1							0x44
#define PLX9054_MBOX2							0x48
#define PLX9054_MBOX3							0x4C
#define PLX9054_MBOX4							0x50
#define PLX9054_MBOX5							0x54
#define PLX9054_MBOX6							0x58
#define PLX9054_MBOX7							0x5C
#define PLX9054_P2LDBELL						0x60
#define PLX9054_L2PDBELL						0x64
#define PLX9054_INTCSR							0x68				// Interrupt Control/Status
#define PLX9054IDX_EN_TEA_LSERR0							0
#define PLX9054IDX_EN_TEA_LSERR1							1
#define PLX9054IDX_GEN_SERR_INT							2	
#define PLX9054IDX_MAILBOX_INT_ENABLE					3
#define PLX9054IDX_POWER_MANAGEMENT_INT_ENABLE		4
#define PLX9054IDX_POWER_MANAGEMENT_INT				5
#define PLX9054IDX_DATA_CHECK_PAR_ERROR_ENABLE		6
#define PLX9054IDX_DATA_CHECK_PAR_ERROR_STATUS		7
#define PLX9054IDX_PCI_INT_ENABLE						8
#define PLX9054IDX_PCI_DOORBELL_INT_ENABLE			9
#define PLX9054IDX_PCI_ABORT_INT_ENABLE				10
#define PLX9054IDX_LOCAL_IN_INT_ENABLE					11
#define PLX9054IDX_RETRY_ABORT_ENABLE					12
#define PLX9054IDX_PCI_DOORBELL_INT_ACTIVE			13
#define PLX9054IDX_PCI_ABORT_INT_ACTIVE				14
#define PLX9054IDX_LOCAL_IN_INT_ACTIVE					15
#define PLX9054IDX_LOCAL_OUT_INT_ENABLE				16
#define PLX9054IDX_LOCAL_DOORBELL_INT_ENABLE			17
#define PLX9054IDX_LOCAL_DMA_CH0_INT_ENABLE			18
#define PLX9054IDX_LOCAL_DMA_CH1_INT_ENABLE			19
#define PLX9054IDX_LOCAL_DOORBELL_INT_ACTIVE			20
#define PLX9054IDX_DMA_CH0_INT_ACTIVE					21
#define PLX9054IDX_DMA_CH1_INT_ACTIVE					22
#define PLX9054IDX_BIST_INT_ACTIVE						23
#define PLX9054_CNTRL							0x6C						// USER I/O Control
#define PLX9054_IDX_GENERAL_PURPOSE_OUTPUT			16
#define PLX9054_IDX_GENERAL_PURPOSE_INPUT				17
#define PLX9054_IDX_SERIAL_EEPROM_CLK					24
#define PLX9054_IDX_SERIAL_EEPROM_CS					25
#define PLX9054_IDX_SERIAL_EEPROM_WRITE_BIT			26
#define PLX9054_IDX_PCI_ADAPTER_SOFT_RESET			30
#define GP_OUTPUT_ENABLE									0x00040000
#define PLX9054_PCIHIDR							0x70
#define PLX9054_PCIHREV							0x74


/////////////////////////////////////////
// DMA REGISTERS								//
/////////////////////////////////////////
// DMAMODE0 register.
#define PLX9054_DMA0_MODE             0x80						// DMA Channel 0 Mode
#define DMA0_BTERM_ENABLE                   (1 << 7)      // 7 bit
#define DMA0_LOCAL_BURST_ENABLE             (1 << 8)      // 8 bit
#define DMA0_DONE_ENABLE                    (1 << 10)     // 10 bit
#define DMA0_HOLD_LOCAL_ADRESS              (1 << 11)     // 11 bit
#define DMA0_DEMAND_MODE                    (1 << 12)     // 12 bit
#define DMA0_FAST_TERMINATION               (1 << 15)     // 15 bit
#define DMA0_INT_TO_PCI                     (1 << 17)     // 17 bit


// DMAPADR0 register
#define PLX9054_DMA0_PCI_ADDRESS      0x84						// DMA Channel 0 PCI Address


// DMALADR0 register
#define PLX9054_DMA0_LB_ADDRESS       0x88						// DMA Channel 0 Local Address


// DMASIZ0 register
#define PLX9054_DMA0_SIZE             0x8C						// DMA Channel 0 Transfer Byte Count


// DMADPR0 register
#define PLX9054_DMA0_DESCR_PTR        0x90						// DMA Channel 0 Descriptor pointer
#define DMA0_DESCRIPTOR_INT_ENABLE          (1 << 2)      // 2 bit
#define DMA0_LOCAL_TO_PCI_TRANSFER          (1 << 3)      // 3 bit


#define PLX9054_DMA1_MODE             0x94						// DMA Channel 1 Mode
#define DMA1_DONE_ENABLE                    0x00000400    // 10 bit
#define DMA1_INT_TO_PCI                     0x00020000    // 18 bit


#define PLX9054_DMA1_DESCR_PTR        0xA4						// DMA Channel 1 Descriptor pointer
#define DMA1_INT_TERM_CNT_ENABLE            0x00000004

// DMACSR0 register
#define PLX9054_DMA_CMD_STS           0xA8						// DMA Channel 0 and 1 Command/Status
#define DMA0_ENABLE                         1             // 0 bit
#define DMA0_START                          (1 << 1)      // 1 bit
#define DMA0_ABORT                          (1 << 2)      // 2 bit
#define DMA0_CLEAR_INT                      (1 << 3)      // 3 bit
#define DMA0_TRANSFER_DONE                  (1 << 4)      // 4 bit

#define DMA1_CLEAR_INT                      0x00000800
#define DMA1_TRANSFER_DONE                  0x00001000

#define PLX9054_LAS1RR_OFFSET         0xF0						// Range for PCI-to-Local Address Space 1

#define PLX9054_LB1_BASE_ADR          0xF4						// Local Base Address (Remap) for PCI-to-Local Address Space 1
#define PLX9054_LAS1BA_OFFSET         PLX9054_LB1_BASE_ADR
#define SPACE1_ENABLED                      0x00000001
#define LB_SPACE1_ADDRESS_MASK              0xFFFFFFF0

#define PLX9054_LBRD1_OFFSET          0xF8						// Local Bus Region Descriptor (Space 1) for PCI-to-Local Accesses 
#define SPACE1_BUSWIDTH8                    0x00000000
#define SPACE1_BUSWIDTH16                   0x00000001
#define SPACE1_BUSWIDTH32                   0x00000003
#define SPACE1_READY_INPUT_ENABLE           0x00000040
#define SPACE1_BTERM_INPUT_ENABLE           0x00000080
#define SPACE1_PREFETCH_DISABLE             0x00000200
#define SPACE1_BURST_ENABLE                 0x00000100
#define SPACE1_PREFETCH_COUNT_ENABLE        0x00000400


#endif // P9054REG_H
