//
// FILE:			library\hardware\mpeg2enc\specific\altrreg.h
// AUTHOR:		Martin Stephan
// COPYRIGHT:	(c) 1999 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.12.1999
//
// PURPOSE:		--- Altera chip specific register definitions for CineMaster II 2000 Encoder --- 
//
// HISTORY:		The definitions are taken from VisionTech's driver.
//					
//

#ifndef ALTRREG_H
#define ALTRREG_H

// All data is in DWORDs
// Kfir registers 
#define ALT_ADR_KFIR_REV_AB					0x000


#define ALT_V11_OLD_REV_AB						0x000


///////////////////////////////////////////////////////////////////////////////////////
//
//		BOARD REVISION 5E
//
///////////////////////////////////////////////////////////////////////////////////////

// offset for Kfir registers 
#define ALT_ADR_KFIR_REV_5E					0x2000

// 1 - Source of interrupt on Read
// 1 - Clear Interrupt when Write
#define ALTREG_INT_REG_CLR_REV_5E			0x3000
#define ALTVAL_INT_CLR_KFIR_INT					0x0001
#define ALTVAL_INT_CLR_ADSP_INT					0x0002
#define ALTVAL_INT_CLEAR_ALL                 0xFF

// 1 - Mask this interupt when Write
#define ALTREG_INT_MASK_REV_5E				0x3004

// 1 - Reset the device when Write 0, except for Kfir PLL - 1
#define ALTREG_RESET_REV_5E					0x3008
#define ALTIDX_RESET_KFIR_REV_5E					0			// 0x0001 - bit 0
#define ALTIDX_RESET_PLL_REV_5E					1			// 0x0002 - bit 1




// 1 - Source of interrupt on Read
// 1 - Clear Interrupt when Write
#define ALT_INT_REG_CLR_REV_AB				0x400
#define ALT_D_INT_REG_CLR_KFIR_INT				0x0001	// bit 0
#define ALT_D_INT_REG_CLR_ADSP_INT           0x0002	// bit 1
#define ALT_D_INT_REG_CLR_LB_TIMEOUT_REV_AB  0x0010	// bit 4
#define ALT_D_INT_CLEAR_ALL                  0xFF		// 

// 1 - Mask this interupt when Write
#define ALT_INT_MASK_REV_AB					0x404
// see ALT_ADR_INT_REG_CLR


// 1 - Reset the device when Write 0;  valid for board revision A and B
#define ALTREG_RESET_REV_AB					0x408			//#define ALT_RESET_REG_REV_AB					0x408
#define ALTIDX_D_RESET_KFIR_REV_ABD				0			//#define ALT_D_RESET_REG_KFIR_REV_ABD			0x0001	// bit 0 
#define ALTIDX_D_RESET_ADSP_REV_AB				1			//#define ALT_D_RESET_REG_ADSP					0x0002	// bit 1
#define ALTIDX_D_RESET_ATOD_REV_AB				2			//#define ALT_D_RESET_REG_ATOD_REV_ABD			0x0004	// bit 2

// 1 - Reset the device when Write 0, except for Kfir PLL - 1; valid for board revision C and D
#define ALTREG_RESET_REV_CD					0x00100008
#define ALTIDX_D_RESET_ATOD_REV_C				0			//0x0001	- bit 0 
#define ALTIDX_D_RESET_ADSP_REV_C				1			//0x0002	- bit 1
#define ALTIDX_D_RESET_KFIR_REV_C				2			//0x0004	- bit 2
#define ALTIDX_D_RESET_GENNUM_REV_C				3			//0x0008	- bit 3
#define ALTIDX_D_RESET_KFIR_PLL_REV_C			4			//0x0010	- bit 4




// ADSP address when write - IAL
#define ALT_ADSP_ADDR_REV_AB					0x40C


// ADSP data when read/write - IDMA
//#define ALT_ADR_ADSP_DATA					0x410
#define ALT_ADSP_DATA_REV_AB					0x450


// Set ADSP IRQs (write 0, then 1) - GPPORT
#define ALT_ADSP_IRQ_REV_AB					0x418
#define ALT_D_ADSP_IRQ_E							0x0001	// bit 0
#define ALT_D_ADSP_IRQ_2							0x0002	// bit 1
#define ALT_D_ADSP_IRQ_FL0_REV_AB				0x0004	// bit 2
#define ALT_D_ADSP_ALL_IRQ							0xFF

// Set ADSP frequency
#define ALT_ADSP_FREQ_REV_AB					0x41C
#define ALT_ADR_ADSP_FREQ_MASK					0x0003	// bit 0 and bit 1
//        00 - 32 KHz,
//        01 - 44.1 KHz,
//        10 - 48 KHz
#define ALT_D_ADSP_SYNC								0x000C	// bit 2 and bit 3

#define ALTREG_KFIR_BASE_ADDRESS				0x420			//#define ALT_KFIR_BASE_ADDRESS_REV_AB		0x420
#define ALTIDX_KFIR_BASE_ADDRESS_BIT0			0
#define ALTIDX_KFIR_BASE_ADDRESS_BIT1			1
#define ALTIDX_KFIR_BASE_ADDRESS_BIT2			2
//#define ALT_KFIR_BASE_ADDRESS_MASK				0x0007	// bit 0 and bit 1 and bit 2
//    000 - Base Address 0,
//    ...
//    111 - Base Address 7
#define ALTIDX_D_KFIR_OUTPUT_FIFO_PRESENT		3			//#define ALT_D_KFIR_OUTPUT_FIFO_PRESENT			0x0008	// bit 3
#define ALTIDX_D_KFIR_I960_MODE					4			//#define ALT_D_KFIR_I960_MODE						0x0010	// bit 4


// I2C interface.
#define ALT_I2C_REV_AB							0x42C //0x414     //0x42C
#define ALT_I2C_DATA_MASK							0x000F	// bit 3,2,1,0
#define ALT_I2C_SUBADDRESS_MASK					0x00F0	// bit 7,6,5,4
#define ALT_I2C_SUBADDRESS_SHIFT					8
#define ALT_I2C_SLAVE_MASK							0x0F00	// bit 11,10,9,8
#define ALT_I2C_SLAVE_SHIFT						16
#define ALT_D_I2C_READY								0x1000	// bit 12
#define ALT_D_I2C_ACCESS							0x2000	// bit 13


// Kfir Audio Base.
#define ALT_KFIR_AUDIO_BASE_REV_AB			0x430
#define ALT_D_KFIR_AUDIO_BASE_MASK				0xFFF		// 
#define ALT_D_KFIR_AUDIO_BASE_SHIFT				20
#define ALT_D_AUDIO_2_ALTERA_2_KFIR				(1<<12)

// 
#define ALT_ADSP_DMA_REV_AB					0x600


// Encoded data when read.
#define ALT_KFIR_OUTPUT_FIFO_DATA_REV_AB  0x10000


#endif // ALTRREG_H
