
// FILE:      library\hardware\uproc\specific\pd4995\pd4995r.h
// AUTHOR:
// COPYRIGHT: (c) 1999 Ravisent Technologies. All Rights Reserved.
// CREATED:   21.09.99
//
// PURPOSE:   Definitions for Pioneer PD4995 controller for front end, panel etc.
//
// HISTORY:

#ifndef PD4995R_H
#define PD4995R_H



#define PD4995REG_FP_BASE				0x60000100
#define PD4995REG_FP_SIZEOF			0xab



#define PD4995REG_MODE_SET			0x00	// Mode Setting Register

#define PD4995IDX_CDDVD				0		// CD/DVD		0 : DVD  1 : CD
#define PD4995IDX_SINGLE_WR		1		// Single WR/RD	0 : read 1 : write
#define PD4995IDX_VDI				2		// Video DMA interface
													// 0 : asynchronous bus mode
													// 1 : synchronous pulse mode
#define PD4995IDX_VDT				3 		// Video DMA terminal
													// 0 : output terminals are high impedance
													// 1 : terminal outputs are valid
#define PD4995IDX_RCS				4, 2	// Refresh Cycle Selection
#define PD4995IDX_S_RW_BREAK		6		// Single R/W Break En
#define PD4995IDX_SPINDLE_RS		7		// Spindle Reg. Sel
													// 0: sub CPU 1: main CPU writes to the spindle servo control reg


#define PD4995REG_INIT				0x01	// Initializing Register

#define PD4995IDX_C_DMA_RESET		2		// CPU_DMA Reset
#define PD4995IDX_A_DMA_RESET		3		// Audio_DMA Reset
#define PD4995IDX_V_DMA_RESET		4		// Video_DMA Reset
#define PD4995IDX_C1_RESET			5		// C1 Reset
#define PD4995IDX_C2_RESET			6		// C2 Reset
#define PD4995IDX_MASTER_RESET	7		// Master Reset


#define PD4995REG_IRM_A						0x02	// Interrupt Mask Register A

#define PD4995IDX_ID							0		// reading ID    (for DVD)
#define PD4995IDX_SUB_Q						0		// reading Subcode Q data (for CD)
#define PD4995IDX_FIRST_LINE_END			1		// First Line End
#define PD4995IDX_BLOCK_END				2		// Block End
#define PD4995IDX_C_DMA_END				3		// CPU_DMA End
#define PD4995IDX_V_DMA_PTR_READY		4		// Video_DMA Pointer Ready
#define PD4995IDX_A_DMA_PTR_READY		5		// Audio_DMA Pointer Ready
#define PD4995IDX_RECEIVE_BUF_FULL		6		// Receive Buffer Full
#define PD4995IDX_TRANSFER_BUF_EMPTY	7		// Transfer Buffer Empty


#define PD4995REG_IRM_B				0x03	// Interrupt Mask Register B

#define PD4995IDX_FG_INT			0		// spindle FG Interrupt
#define PD4995IDX_A_FRAME			1		// Audio Block Interrupt
#define PD4995IDX_C1_BLOCK_END	2		// DVD: C1 block read operation completed -> C2 correction start
													// CD: sector read operation completed
#define PD4995IDX_V_DMA_END		4		// Video DMA competed
#define PD4995IDX_A_DMA_FIFO		5		// Audio DMA completed, FIFO empty
#define PD4995IDX_END_DETECT		6		// MPEG sequence end code detected
#define PD4995IDX_END_SST			7		// End of Synchronous Serial Transfer


#define PD4995REG_IRF_A				0x04	// Interrupt Factor Register A
// bits are same as PD4995REG_IRM_A

#define PD4995REG_IRF_B				0x05	// Interrupt Factor Register B
// bits are same as PD4995REG_IRM_B

#define PD4995REG_ITS_A				0x06	// Interrupt Terminal Selection Register A/Version Register
// bits are same as PD4995REG_IRM_A

#define PD4995REG_ITS_B				0x07	// Interrupt Terminal Selection Register B
// bits are same as PD4995REG_IRM_B


// Mapping for DVD mode.

#define PD4995REG_ID_0				0x08	// ID Register First ID Byte
#define PD4995REG_ID_8				0x09	// ID Register Second ID Byte/PLL synchronous clear (WR)
#define PD4995REG_ID_16				0x0a	// ID Register Third ID Byte
#define PD4995REG_ID_24				0x0b	// ID Register Fourth ID Byte

#define PD4995REG_ID_EDC			0x0c	// EDC Result of ID read (0x08-0x0b)

#define PD4995IDX_C1_CRCTN_INH	0		// C1 Correction Inhibited
#define PD4995IDX_C2_CRCTN_INH	1		// C2 Correction Inhibited
#define PD4995IDX_DSCRMBL_INH		2		// Descramble Inhibited
#define PD4995IDX_CORRECTED		6		// in case of error: correction successful
#define PD4995IDX_ID_EDC_OK		7		// an EDC of an ID detected no error
#define PD4995IDX_EDC_STATUS		6, 2	// EDC Status of ID


// Mapping for CD mode.

#define PD4995REG_SUBCODE_CONT	0x08	// Sub Code Control Register
#define PD4995IDX_SUB_Q_CRC		0		// Subcode Q data CRC check OK
#define PD4995IDX_SUBCODE_DMA		1		// Subcode R-W DMA enabled
#define PD4995IDX_EDC_MODE_SEL	2		// EDC Mode Selection
#define PD4995IDX_EDC_RESULT		4		// EDC Result
#define PD4995IDX_CDROM_RESYNC	5, 2	// CDROM Resync Mode

#define PD4995REG_SUBCODE_R-W		0x09
#define PD4995IDX_SUBCODE_W		0
#define PD4995IDX_SUBCODE_V		1
#define PD4995IDX_SUBCODE_U		2
#define PD4995IDX_SUBCODE_T		3
#define PD4995IDX_SUBCODE_S		4
#define PD4995IDX_SUBCODE_R		5

#define PD4995REG_SUBCODE_S0		0x0a	// writing triggers a Reed-Solomon syndrom calculation
#define PD4995REG_SUBCODE_S1		0x0b	// writing initializes this

#define PD4995REG_SUBCODE_Q		0x0c	// Subcode Q data Read Register


#define PD4995REG_SUB_CPU			0x0d	// Sub CPU Data Transfer/Reception Register (check sum)

#define PD4995REG_SCPU_STAT		0x0e	// Sub CPU Status Register
#define PD4995IDX_ATN				0		// Attention: communication request of the sub CPU / error in multibyte transfer
#define PD4995IDX_BUSY				1		// sub CPU is processing an operation
#define PD4995IDX_READY				2		// player ready to play the disk
#define PD4995IDX_COMM_BUSY		3
#define PD4995IDX_PLAYER_TYPE		5, 2	// Type of player

#define PD4995REG_MCPU_STAT		0x0f	// Main CPU Status Register
// bits 0 and 1 are analog to PD4995REG_SCPU_ST

#define PD4995REG_BYTE_RW			0x10	// CPU Single Byte Read/Write Register

#define PD4995REG_BYTE_RW_BS		0x11	// CPU Single Byte Read/Write BLOCK Setting
#define PD4995IDX_B_ADDRESS		0, 7	// Block Address
#define PD4995IDX_RAMRQ0			7		// RAMRQ0

#define PD4995REG_BYTE_RW_V		0x12	// CPU Single Byte Read/Write Address Setting (V Address)
#define PD4995REG_BYTE_RW_H		0x13	// CPU Single Byte Read/Write Address Setting (H Address)

#define PD4995REG_PD_TC						0x14	// Playback Data Transfer Control Register
#define PD4995IDX_DISK_RD_ENABLE			0		// DISK_RD Enable
#define PD4995IDX_POINTER_READY			1		// Pointer Ready
#define PD4995IDX_DISK_RD_BUSY			2		// DISK_RD Busy
#define PD4995IDX_NEXT_BLOCK_ENABLE		3		// Next Block Enable
#define PD4995IDX_SECTOR_NO				4, 4	// Sector No.

#define PD4995REG_PD_TBS			0x15	// Playback Data Transfer Block Specification/Playback End Block
#define PD4995IDX_BLOCK_NUMBER	0, 7	// Block Number


#define PD4995REG_PD_EDC_8			0x16	// Playback Data EDC Register
#define PD4995IDX_SECTOR_8			0		// EDC Result of Sector 8
#define PD4995IDX_SECTOR_9			1		// EDC Result of Sector 9
#define PD4995IDX_SECTOR_10		2		// EDC Result of Sector 10
#define PD4995IDX_SECTOR_11		3		// EDC Result of Sector 11
#define PD4995IDX_SECTOR_12		4		// EDC Result of Sector 12
#define PD4995IDX_SECTOR_13		5		// EDC Result of Sector 13
#define PD4995IDX_SECTOR_14		6		// EDC Result of Sector 14
#define PD4995IDX_SECTOR_15		7		// EDC Result of Sector 15

#define PD4995REG_PD_EDC_0			0x17	// Playback Data EDC Register
#define PD4995IDX_SECTOR_0			0		// EDC Result of Sector 0
#define PD4995IDX_SECTOR_1			1		// EDC Result of Sector 1
#define PD4995IDX_SECTOR_2			2		// EDC Result of Sector 2
#define PD4995IDX_SECTOR_3			3		// EDC Result of Sector 3
#define PD4995IDX_SECTOR_4			4		// EDC Result of Sector 4
#define PD4995IDX_SECTOR_5			5		// EDC Result of Sector 5
#define PD4995IDX_SECTOR_6			6		// EDC Result of Sector 6
#define PD4995IDX_SECTOR_7			7		// EDC Result of Sector 7

// CD Mode for the above two registers should be added later

// register definition for 0x18 - 0x4f will be added here later


#define PD4995REG_V_DMA_CONTROL				0x20

#define PD4995IDX_DMA_ENABLE					0
#define PD4995IDX_V_POINTER_READY			1
#define PD4995IDX_MAX_TRANSFER_RATE			5
#define PD4995IDX_XWR_TERMINAL_DUTY_RATE	6


#define PD4995REG_V_DMA_BLOCK				0x21

#define PD4995REG_V_DMA_ADDRESS_V		0x22
#define PD4995REG_V_DMA_ADDRESS_H		0x23

#define PD4995REG_V_DMA_BYTECOUNT8		0x24
#define PD4995REG_V_DMA_BYTECOUNT0		0x25


#define PD4995REG_CD_SHOCKMODE			0x46

#define PD4995IDX_RESET_SHOCK_MODE		0


// 0x50 - 0x5e not specified


#define PD4995REG_SETUP_MEDIA				0x5f

#define PD4995IDX_ASDATA_EN				2		// validate terminal outputs
#define PD4995IDX_CDDA_SEL					7		// select CD/DA


#define PD4995REG_COMMAND			0x60
#define PD4995REG_PARAMETER_0		0x61
#define PD4995REG_PARAMETER_1		0x62
#define PD4995REG_PARAMETER_2		0x63
#define PD4995REG_PARAMETER_3		0x64
#define PD4995REG_PARAMETER_4		0x65
#define PD4995REG_PARAMETER_5		0x66
#define PD4995REG_PARAMETER_6		0x67


#define PD4995REG_SSP_CPS			 	0x68	// Synchronous Serial Port Clock and Prescalar Setting Register
#define PD4995IDX_DIVIDING_VALUE 	0, 6	// Dividing Value

#define PD4995REG_SSP_IC			 	0x69	// Synchronous Serial Port Interrupt Control Register
#define PD4995IDX_SCI_CH0_EOT	 		0		// SCI CH0 End of Transfer
#define PD4995IDX_SCI_CH1_EOT	 		1		// SCI CH1 End of Transfer
#define PD4995IDX_SCI_CH2_EOT	 		2		// SCI CH2 End of Transfer
#define PD4995IDX_SCI_CH0_INT_MASK	4		// SCI CH0 INT Mask
#define PD4995IDX_SCI_CH1_INT_MASK	5		// SCI CH1 INT Mask
#define PD4995IDX_SCI_CH2_INT_MASK	6		// SCI CH2 INT Mask


// Register definitions for 0x6a - 0x6f will be added here later

#define PD4995REG_PFS					0x70	// Pin Function Setup Register
#define PD4995IDX_PAPORT				0		// PA Port Main/Sub
#define PD4995IDX_PBPORT				1		// PB Port Main/Sub
#define PD4995IDX_PCPORT				2		// PC Port Main/Sub
#define PD4995IDX_PBSELECT				3		// PB Select Ned/Port
#define PD4995IDX_PCSELECT				4		// PC Select Stat/Port

#define PD4995REG_PA_PORT_DD			0x71	// PA Port Data Direction Register
#define PD4995REG_PB_PORT_DD			0x72	// PB Port Data Direction Register
#define PD4995REG_PC_PORT_DD			0x73	// PC Port Data Direction Register
#define PD4995IDX_DIR0					0		// Direction for port bit 0
#define PD4995IDX_DIR1					1		// Direction for port bit 1
#define PD4995IDX_DIR2					2		// Direction for port bit 2
#define PD4995IDX_DIR3					3		// Direction for port bit 3
#define PD4995IDX_DIR4					4		// Direction for port bit 4
#define PD4995IDX_DIR5					5		// Direction for port bit 5
#define PD4995IDX_DIR6					6		// Direction for port bit 6
#define PD4995IDX_DIR7					7		// Direction for port bit 7

#define PD4995REG_PA_PORT_DATA		0x74	// PA Port Data
#define PD4995REG_PB_PORT_DATA		0x75	// PB Port Data
#define PD4995REG_PC_PORT_DATA		0x76	// PC Port Data

// 0x77 not specified
// Register definitions for 0x78 - 0x7f will be added here later


/*** Register definitions for Front Panel ***/

// For front panel registers, you must add PD4995REG_FP_BASEADDRESS, which is now 0x100
// this will be changed to 0x00 with the next hardware revision

#define PD4995REG_DATA_0				0x80
#define PD4995REG_DATA_1				0x81
#define PD4995REG_DATA_2				0x82
#define PD4995REG_DATA_3				0x83
#define PD4995REG_DATA_4				0x84
#define PD4995REG_DATA_5				0x85
#define PD4995REG_DATA_6				0x86
#define PD4995REG_DATA_7				0x87
#define PD4995REG_DATA_8				0x88
#define PD4995REG_DATA_9				0x89
#define PD4995REG_DATA_A				0x8a
#define PD4995REG_DATA_B				0x8b
#define PD4995REG_DATA_C				0x8c
#define PD4995REG_ECHO					0x8d
#define PD4995REG_DATA_E				0x8e
#define PD4995REG_CHECKSUM				0x8f


#define PD4995REG_TX_MODE_SETUP		0xa0	// Transmission Mode Setup
#define PD4995REG_TX_CONTROL			0xa1	// Transmission Control

#define PD4995REG_TX_LENGTH			0xa3	// Transmission Length
#define PD4995REG_TX_CLOCK				0xa4	// Transmission Clock
#define PD4995REG_TX_PORT_CONTROL	0xa8	// Transmission Port Control
#define PD4995REG_TX_IRQ_EOT			0xa9	// IRQ for End of Transmission

#define PD4995REG_PA_PORT_CONT		0xaa	// Control of PA Port
#define PD4995IDX_PA_LSB				0		// LSB first
#define PD4995IDX_PA6_ALTERNATE		1		// Alternate function (use serial data)
#define PD4995IDX_PA7_ALTERNATE		2		// Alternate function (use serial clock)
#define PD4995IDX_PA_CLOCK				3, 2	// Set up of the clock
#define PD4995IDX_PA_RESET				7		// Port reset

#endif
