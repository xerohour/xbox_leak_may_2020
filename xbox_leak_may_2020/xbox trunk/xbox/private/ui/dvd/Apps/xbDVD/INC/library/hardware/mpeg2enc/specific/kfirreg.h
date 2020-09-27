//
// FILE:      library\hardware\mpeg2enc\specific\kfirreg.h
// AUTHOR:    Martin Stephan
// COPYRIGHT: (c) 1999 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   27.12.1999
//
// PURPOSE:   --- KFIR chip specific register definitions --- 
//
// HISTORY:
//

#ifndef KFIRREG_H
#define KFIRREG_H


#define DEV_ID_GC									7

#if 0

#define KFIR_ASPECT_RATIO						0*4
#define KFIR_FRAME_RATE_CODE					1*4
#define KFIR_COLOR_PRIM							2*4
#define KFIR_TRANSFER_CHAR						3*4
#define KFIR_MATRIX_COEFF						4*4
#define KFIR_BIT_RATE_0							5*4
#define KFIR_BIT_RATE_1							6*4
#define KFIR_BIT_RATE_2							7*4
#define KFIR_VBV_BUFFER_SIZE					8*4
#define KFIR_PROGRESSIVE_SEQ					9*4
#define KFIR_PROGRESSIVE_FRAME				10*4
#define KFIR_INTRA_DC_PRECISION				11*4
#define KFIR_INTRA_VLC_FORMAT_I				12*4
#define KFIR_INTRA_VLC_FORMAT_P				13*4
#define KFIR_INTRA_VLC_FORMAT_B				14*4
#define KFIR_ALT_SCAN_I							15*4  //  R/W
#define KFIR_ALT_SCAN_P							16*4  //  R/W
#define KFIR_ALT_SCAN_B							17*4  //  R/W
#define KFIR_FRAME_PRED_FRAME_DCT_I			18*4  //  R/W
#define KFIR_FRAME_PRED_FRAME_DCT_P			19*4  //  R/W
#define KFIR_FRAME_PRED_FRAME_DCT_B			20*4  //  R/W
#define KFIR_AVG_ACT								21*4  //  R/W
#define KFIR_X_I									22*4  //  R/W
#define KFIR_X_P									23*4  //  R/W
#define KFIR_X_B									24*4  //  R/W
#define KFIR_D0_I_0								25*4  //  R/W
#define KFIR_D0_P_0								26*4  //  R/W
#define KFIR_D0_B_0								27*4  //  R/W
#define KFIR_D0_I_1								28*4  //  R/W
#define KFIR_D0_P_1								29*4  //  R/W
#define KFIR_D0_B_1								30*4  //  R/W
#define KFIR_INTRAD								31*4  //  R/W
#define KFIR_TMIN									32*4  //  R/W
#define KFIR_R2									33*4  //  R/W
#define KFIR_MB_WIDTH							34*4  //  R/W
#define KFIR_RMBW									35*4  //  R/W
#define KFIR_RMB									36*4  //  R/W
#define KFIR_GETB									37*4  //  R/W
#define KFIR_C1									38*4  //  R/W
#define KFIR_C2									39*4  //  R/W
#define KFIR_STD_PICT_DEL						40*4  //  R/W
#define KFIR_FIRST_PICT_DEL					41*4  //  R/W
#define KFIR_GOPR1_1								42*4  //  R/W
#define KFIR_GOPR2_1								43*4  //  R/W
#define KFIR_GOPR1_0								44*4  //  R/W
#define KFIR_GOPR2_0								45*4  //  R/W
#define KFIR_SLICE_CONTROL						46*4  //  R/W
#define KFIR_VIDEO_FORMAT						47*4  //  R/W
#define KFIR_INTERLACED_SOURCE				48*4  //  R/W
#define KFIR_M										49*4  //  R/W
#define KFIR_MULTIPLE							50*4  //  R/W
#define KFIR_TOTAL_MBS							51*4  //  R/W
#define KFIR_MB_LINES							52*4  //  R/W
#define KFIR_RANDOM_SCAN						53*4  //  R/W
#define KFIR_VBR_ENABLE							54*4  //  R/W
#define KFIR_TOP_FIRST							55*4  //  R/W
#define KFIR_LOW_SEARCH_RANGE					56*4  //  R/W
#define KFIR_CONST_MQUANT						57*4  //  R/W
#define KFIR_LOW_LATENCY						58*4  //  R/W
#define KFIR_AUTO_PULLDOWN						59*4  //  R/W
#define KFIR_BITSTREAM_WIDTH					60*4  //  R/W
#define KFIR_VOB_BASE_ADDR						61*4  //  R/W
#define KFIR_USE_SDRAM_FIFO					62*4  //  R/W
#define KFIR_VIB_DEC_PATTERN					63*4  //  R/W
#define KFIR_VIB_DEC_SIZE						64*4  //  R/W
#define KFIR_VIB_VIDEO_PAR						65*4  //  R/W
#define KFIR_VIB_HORI_TOP_BOT					66*4  //  R/W
#define KFIR_VIB_VERT_TOP_BOT					67*4  //  R/W
#define KFIR_VIB_FILTERC_YH_0					68*4  //  R/W
#define KFIR_VIB_FILTERC_YH_1					69*4  //  R/W
#define KFIR_VIB_FILTERC_YH_2					70*4  //  R/W
#define KFIR_VIB_FILTERC_YH_3					71*4  //  R/W
#define KFIR_VIB_FILTERC_YH_4					72*4  //  R/W
#define KFIR_VIB_FILTERC_YH_5					73*4  //  R/W
#define KFIR_VIB_FILTERC_YH_6					74*4  //  R/W
#define KFIR_VIB_FILTERC_CH_0					75*4  //  R/W
#define KFIR_VIB_FILTERC_CH_1					76*4  //  R/W
#define KFIR_VIB_FILTERC_CH_2					77*4  //  R/W
#define KFIR_VIB_FILTERC_CH_3					78*4  //  R/W
#define KFIR_VIB_FILTERC_CH_4					79*4  //  R/W
#define KFIR_VIB_FILTERC_CH_5					80*4  //  R/W
#define KFIR_VIB_FILTERC_CH_6					81*4  //  R/W
#define KFIR_VIB_FILTERC_CV0_0				82*4  //  R/W
#define KFIR_VIB_FILTERC_CV0_1				83*4  //  R/W
#define KFIR_VIB_FILTERC_CV0_2				84*4  //  R/W
#define KFIR_VIB_FILTERC_CV0_3				85*4  //  R/W
#define KFIR_VIB_FILTERC_CV0_4				86*4  //  R/W
#define KFIR_VIB_FILTERC_CV0_5				87*4  //  R/W
#define KFIR_VIB_FILTERC_CV1_0				88*4  //  R/W
#define KFIR_VIB_FILTERC_CV1_1				89*4  //  R/W
#define KFIR_VIB_FILTERC_CV1_2				90*4  //  R/W
#define KFIR_VIB_FILTERC_CV1_3				91*4  //  R/W
#define KFIR_VIB_FILTERC_CV1_4				92*4  //  R/W
#define KFIR_VIB_FILTERC_CV1_5				93*4  //  R/W
#define KFIR_TELE_THRESHOLD_0					94*4  //  R/W
#define KFIR_TELE_THRESHOLD_1					95*4  //  R/W
#define KFIR_DMA_BSM_BURST						96*4  //  R/W
#define KFIR_DMA_BSM_BURST_END				97*4  //  R/W
#define KFIR_DMA_VIB_BURST						98*4  //  R/W
#define KFIR_DMA_VIB_BURST_END				99*4  //  R/W
#define KFIR_FILE_MODE							100*4		// taken from VisionTech's driver (file KfirDef.h)
#define KFIR_MPEG_I_NEG							103*4 	// taken from VisionTech's driver (file KfirDef.h)
#define KFIR_FRAME_SKIP_RATIO					104*4		// taken from VisionTech's driver (file KfirDef.h)
#define KFIR_GOP_0								128*4  //  R                      
#define KFIR_GOP_1								129*4  //  R
#define KFIR_PICT_TYPE							132*4  //  R
#define KFIR_X										146*4  // ?/W
#define KFIR_Y										149*4  //  ?/W
// DRAM check
// 1 in at least one bit - error
// 0                     - no errors.
#define KFIR_TEST_0								171*4  
#define KFIR_TEST_1								172*4  
// end DRAM check
#define KFIR_FRAME								183*4  //  R
#define KFIR_FIFO_REG_0							184*4  //  R
#define KFIR_FIFO_REG_1							185*4  //  R
#define KFIR_SRAM_CNT_0							186*4  //  R
#define KFIR_SRAM_CNT_1							187*4  //  R
#define KFIR_SRAM_REG_0							188*4  //  R
#define KFIR_SRAM_REG_1							189*4  //  R

#define KFIR_200                          200*4   // W
#define KFIR_DEV_INST                     KFIR_200   // W/R

#define KFIR_201                          201*4   // R
#define KFIR_EXEC                         KFIR_201  // R

#define KFIR_202                          202*4
#define KFIR_VIB_FIFO_BASE_ADR            KFIR_202   // W

#define KFIR_203									203*4
#define KFIR_MUX_FIFO_BASE_ADR            KFIR_203   // W

#define KFIR_204                          204*4
#define KFIR_AUDIO_FIFO_BASE_ADR          KFIR_204   // W

#define KFIR_205									205*4
#define KFIR_DREQ_MUX_VIB						KFIR_205

// 27 MHz based counter
#define KFIR_206									206*4
#define KFIR_REF_CLK_27M						KFIR_206

// MUX Memory Size
#define KFIR_207                          207*4
#define KFIR_MUX_MEM_SIZE						KFIR_207
// 20 bits

// MUX Memory Counter
#define KFIR_208                          208*4
#define KFIR_MUX_MEM_CNT						KFIR_208
// 20 bits

// VIB, Video Input Buffer, File Mode Memory Size
#define KFIR_209                          209*4
#define KFIR_VIB_MEM_SIZE						KFIR_209
// 20 bits

// VIB, Video Input Buffer, File Mode Memory Counter
#define KFIR_210                          210*4
#define KFIR_VIB_MEM_CNT						KFIR_210
// 20 bits

// I960 Config register. Master/Slave mode configuration. 
#define KFIR_211                          211*4   // W
#define KFIR_I960_CONFIG						KFIR_211
#define KFIR_I960_CONFIG_SLAVE					0
#define KFIR_I960_CONFIG_MASTER					1


#define KFIR_INTERRUPT_CNT						221*4  //  R
#define KFIR_AUDIO_PTS_0						222*4  //  R/W
#define KFIR_AUDIO_PTS_1						223*4  //  R/W
#define KFIR_AUDIO_PTS_2						224*4  //  R/W
#define KFIR_AUDIO_INT_ACK						228*4  //  ?/W
#define KFIR_AUDIO_FRAME_SIZE					229*4   // R/W
#define KFIR_MUX_CNT_0							231*4
#define KFIR_MUX_CNT_1							232*4
#define KFIR_MUX_CNT_2							233*4
#define KFIR_INT_VIDEO							234*4  //  R/W
#define KFIR_INT_AUDIO							235*4  //  R/W
#define KFIR_INT_DMA_MUX						236*4  //  R/W
#define KFIR_INT_DMA_VIB						237*4  //  R/W
#define KFIR_TEST_MODE							252*4  //  ?/W
#define KFIRIDX_TEST_MODE							0		// bit 0 
#define KFIR_TEST_MODE_OFF							0x0
#define KFIR_TEST_MODE_ON							0x1
#define KFIR_PAUSE								253*4   // R/W
#define KFIRVAL_RESUME								0x0
#define KFIRVAL_PAUSE								0x1

#define KFIR_EREADY								254*4  //  R
#define KFIRIDX_EREADY_0							0		// bit 0
#define KFIRVAL_ENCODING							0x0
#define KFIRVAL_READY								0x1
#define KFIRVAL_TEST									0x2
#define KFIRVAL_STOP_DETECT						0x3
#define KFIRVAL_SEQ_END								0x4

#define KFIR_RECORD								255*4  //  R/W
#define KFIRIDX_RECORD								0		// bit 0
#define KFIRVAL_RECORD_STOP						0x0
#define KFIRVAL_RECORD_START						0x1

#else // board revision 5E

#define KFIR_ASPECT_RATIO						0*4+ALT_ADR_KFIR_REV_5E
#define KFIR_FRAME_RATE_CODE					1*4+ALT_ADR_KFIR_REV_5E
#define KFIR_COLOR_PRIM							2*4+ALT_ADR_KFIR_REV_5E
#define KFIR_TRANSFER_CHAR						3*4+ALT_ADR_KFIR_REV_5E
#define KFIR_MATRIX_COEFF						4*4+ALT_ADR_KFIR_REV_5E
#define KFIR_BIT_RATE_0							5*4+ALT_ADR_KFIR_REV_5E
#define KFIR_BIT_RATE_1							6*4+ALT_ADR_KFIR_REV_5E
#define KFIR_BIT_RATE_2							7*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VBV_BUFFER_SIZE					8*4+ALT_ADR_KFIR_REV_5E
#define KFIR_PROGRESSIVE_SEQ					9*4+ALT_ADR_KFIR_REV_5E
#define KFIR_PROGRESSIVE_FRAME				10*4+ALT_ADR_KFIR_REV_5E
#define KFIR_INTRA_DC_PRECISION				11*4+ALT_ADR_KFIR_REV_5E
#define KFIR_INTRA_VLC_FORMAT_I				12*4+ALT_ADR_KFIR_REV_5E
#define KFIR_INTRA_VLC_FORMAT_P				13*4+ALT_ADR_KFIR_REV_5E
#define KFIR_INTRA_VLC_FORMAT_B				14*4+ALT_ADR_KFIR_REV_5E
#define KFIR_ALT_SCAN_I							15*4+ALT_ADR_KFIR_REV_5E 
#define KFIR_ALT_SCAN_P							16*4+ALT_ADR_KFIR_REV_5E
#define KFIR_ALT_SCAN_B							17*4+ALT_ADR_KFIR_REV_5E
#define KFIR_FRAME_PRED_FRAME_DCT_I			18*4+ALT_ADR_KFIR_REV_5E
#define KFIR_FRAME_PRED_FRAME_DCT_P			19*4+ALT_ADR_KFIR_REV_5E
#define KFIR_FRAME_PRED_FRAME_DCT_B			20*4+ALT_ADR_KFIR_REV_5E
#define KFIR_AVG_ACT								21*4+ALT_ADR_KFIR_REV_5E
#define KFIR_X_I									22*4+ALT_ADR_KFIR_REV_5E
#define KFIR_X_P									23*4+ALT_ADR_KFIR_REV_5E
#define KFIR_X_B									24*4+ALT_ADR_KFIR_REV_5E
#define KFIR_D0_I_0								25*4+ALT_ADR_KFIR_REV_5E
#define KFIR_D0_P_0								26*4+ALT_ADR_KFIR_REV_5E
#define KFIR_D0_B_0								27*4+ALT_ADR_KFIR_REV_5E
#define KFIR_D0_I_1								28*4+ALT_ADR_KFIR_REV_5E
#define KFIR_D0_P_1								29*4+ALT_ADR_KFIR_REV_5E
#define KFIR_D0_B_1								30*4+ALT_ADR_KFIR_REV_5E
#define KFIR_INTRAD								31*4+ALT_ADR_KFIR_REV_5E
#define KFIR_TMIN									32*4+ALT_ADR_KFIR_REV_5E
#define KFIR_R2									33*4+ALT_ADR_KFIR_REV_5E
#define KFIR_MB_WIDTH							34*4+ALT_ADR_KFIR_REV_5E
#define KFIR_RMBW									35*4+ALT_ADR_KFIR_REV_5E
#define KFIR_RMB									36*4+ALT_ADR_KFIR_REV_5E
#define KFIR_GETB									37*4+ALT_ADR_KFIR_REV_5E
#define KFIR_C1									38*4+ALT_ADR_KFIR_REV_5E
#define KFIR_C2									39*4+ALT_ADR_KFIR_REV_5E
#define KFIR_STD_PICT_DEL						40*4+ALT_ADR_KFIR_REV_5E
#define KFIR_FIRST_PICT_DEL					41*4+ALT_ADR_KFIR_REV_5E
#define KFIR_GOPR1_1								42*4+ALT_ADR_KFIR_REV_5E
#define KFIR_GOPR2_1								43*4+ALT_ADR_KFIR_REV_5E
#define KFIR_GOPR1_0								44*4+ALT_ADR_KFIR_REV_5E
#define KFIR_GOPR2_0								45*4+ALT_ADR_KFIR_REV_5E
#define KFIR_SLICE_CONTROL						46*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIDEO_FORMAT						47*4+ALT_ADR_KFIR_REV_5E
#define KFIR_INTERLACED_SOURCE				48*4+ALT_ADR_KFIR_REV_5E
#define KFIR_M										49*4+ALT_ADR_KFIR_REV_5E
#define KFIR_MULTIPLE							50*4+ALT_ADR_KFIR_REV_5E
#define KFIR_TOTAL_MBS							51*4+ALT_ADR_KFIR_REV_5E
#define KFIR_MB_LINES							52*4+ALT_ADR_KFIR_REV_5E
#define KFIR_RANDOM_SCAN						53*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VBR_ENABLE							54*4+ALT_ADR_KFIR_REV_5E
#define KFIR_TOP_FIRST							55*4+ALT_ADR_KFIR_REV_5E
#define KFIR_LOW_SEARCH_RANGE					56*4+ALT_ADR_KFIR_REV_5E
#define KFIR_CONST_MQUANT						57*4+ALT_ADR_KFIR_REV_5E
#define KFIR_LOW_LATENCY						58*4+ALT_ADR_KFIR_REV_5E
#define KFIR_AUTO_PULLDOWN						59*4+ALT_ADR_KFIR_REV_5E
#define KFIR_BITSTREAM_WIDTH					60*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VOB_BASE_ADDR						61*4+ALT_ADR_KFIR_REV_5E
#define KFIR_USE_SDRAM_FIFO					62*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_DEC_PATTERN					63*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_DEC_SIZE						64*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_VIDEO_PAR						65*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_HORI_TOP_BOT					66*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_VERT_TOP_BOT					67*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_YH_0					68*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_YH_1					69*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_YH_2					70*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_YH_3					71*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_YH_4					72*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_YH_5					73*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_YH_6					74*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CH_0					75*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CH_1					76*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CH_2					77*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CH_3					78*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CH_4					79*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CH_5					80*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CH_6					81*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CV0_0				82*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CV0_1				83*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CV0_2				84*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CV0_3				85*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CV0_4				86*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CV0_5				87*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CV1_0				88*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CV1_1				89*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CV1_2				90*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CV1_3				91*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CV1_4				92*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FILTERC_CV1_5				93*4+ALT_ADR_KFIR_REV_5E
#define KFIR_TELE_THRESHOLD_0					94*4+ALT_ADR_KFIR_REV_5E
#define KFIR_TELE_THRESHOLD_1					95*4+ALT_ADR_KFIR_REV_5E
#define KFIR_DMA_BSM_BURST						96*4+ALT_ADR_KFIR_REV_5E
#define KFIR_DMA_BSM_BURST_END				97*4+ALT_ADR_KFIR_REV_5E
#define KFIR_DMA_VIB_BURST						98*4+ALT_ADR_KFIR_REV_5E
#define KFIR_DMA_VIB_BURST_END				99*4+ALT_ADR_KFIR_REV_5E
#define KFIR_FILE_MODE							100*4+ALT_ADR_KFIR_REV_5E	// taken from VisionTech's driver (file KfirDef.h)
#define KFIR_MPEG_I_NEG							103*4+ALT_ADR_KFIR_REV_5E 	// taken from VisionTech's driver (file KfirDef.h)
#define KFIR_FRAME_SKIP_RATIO					104*4+ALT_ADR_KFIR_REV_5E		// taken from VisionTech's driver (file KfirDef.h)
#define KFIR_GOP_0								128*4+ALT_ADR_KFIR_REV_5E         
#define KFIR_GOP_1								129*4+ALT_ADR_KFIR_REV_5E
#define KFIR_PICT_TYPE							132*4+ALT_ADR_KFIR_REV_5E
#define KFIR_X										146*4+ALT_ADR_KFIR_REV_5E
#define KFIR_Y										149*4+ALT_ADR_KFIR_REV_5E
// DRAM check
// 1 in at least one bit - error
// 0                     - no errors.
#define KFIR_TEST_0								171*4+ALT_ADR_KFIR_REV_5E
#define KFIR_TEST_1								172*4+ALT_ADR_KFIR_REV_5E
// end DRAM check
#define KFIR_FRAME								183*4+ALT_ADR_KFIR_REV_5E
#define KFIR_FIFO_REG_0							184*4+ALT_ADR_KFIR_REV_5E
#define KFIR_FIFO_REG_1							185*4+ALT_ADR_KFIR_REV_5E
#define KFIR_SRAM_CNT_0							186*4+ALT_ADR_KFIR_REV_5E
#define KFIR_SRAM_CNT_1							187*4+ALT_ADR_KFIR_REV_5E
#define KFIR_SRAM_REG_0							188*4+ALT_ADR_KFIR_REV_5E
#define KFIR_SRAM_REG_1							189*4+ALT_ADR_KFIR_REV_5E

#define KFIR_200                          200*4+ALT_ADR_KFIR_REV_5E
#define KFIR_DEV_INST                     KFIR_200   // W/R

#define KFIR_201                          201*4+ALT_ADR_KFIR_REV_5E
#define KFIR_EXEC                         KFIR_201  // R

#define KFIR_202                          202*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_FIFO_BASE_ADR            KFIR_202   // W

#define KFIR_203									203*4+ALT_ADR_KFIR_REV_5E
#define KFIR_MUX_FIFO_BASE_ADR            KFIR_203   // W

#define KFIR_204                          204*4+ALT_ADR_KFIR_REV_5E
#define KFIR_AUDIO_FIFO_BASE_ADR          KFIR_204   // W

#define KFIR_205									205*4+ALT_ADR_KFIR_REV_5E
#define KFIR_DREQ_MUX_VIB						KFIR_205

// 27 MHz based counter
#define KFIR_206									206*4+ALT_ADR_KFIR_REV_5E
#define KFIR_REF_CLK_27M						KFIR_206

// MUX Memory Size
#define KFIR_207                          207*4+ALT_ADR_KFIR_REV_5E
#define KFIR_MUX_MEM_SIZE						KFIR_207
// 20 bits

// MUX Memory Counter
#define KFIR_208                          208*4+ALT_ADR_KFIR_REV_5E
#define KFIR_MUX_MEM_CNT						KFIR_208
// 20 bits

// VIB, Video Input Buffer, File Mode Memory Size
#define KFIR_209                          209*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_MEM_SIZE						KFIR_209
// 20 bits

// VIB, Video Input Buffer, File Mode Memory Counter
#define KFIR_210                          210*4+ALT_ADR_KFIR_REV_5E
#define KFIR_VIB_MEM_CNT						KFIR_210
// 20 bits

// I960 Config register. Master/Slave mode configuration. 
#define KFIR_211                          211*4+ALT_ADR_KFIR_REV_5E
#define KFIR_I960_CONFIG						KFIR_211
#define KFIR_I960_CONFIG_SLAVE					0
#define KFIR_I960_CONFIG_MASTER					1


#define KFIR_INTERRUPT_CNT						221*4+ALT_ADR_KFIR_REV_5E
#define KFIR_AUDIO_PTS_0						222*4+ALT_ADR_KFIR_REV_5E
#define KFIR_AUDIO_PTS_1						223*4+ALT_ADR_KFIR_REV_5E
#define KFIR_AUDIO_PTS_2						224*4+ALT_ADR_KFIR_REV_5E
#define KFIR_AUDIO_INT_ACK						228*4+ALT_ADR_KFIR_REV_5E
#define KFIR_AUDIO_FRAME_SIZE					229*4+ALT_ADR_KFIR_REV_5E
#define KFIR_MUX_CNT_0							231*4+ALT_ADR_KFIR_REV_5E
#define KFIR_MUX_CNT_1							232*4+ALT_ADR_KFIR_REV_5E
#define KFIR_MUX_CNT_2							233*4+ALT_ADR_KFIR_REV_5E
#define KFIR_INT_VIDEO							234*4+ALT_ADR_KFIR_REV_5E
#define KFIR_INT_AUDIO							235*4+ALT_ADR_KFIR_REV_5E
#define KFIR_INT_DMA_MUX						236*4+ALT_ADR_KFIR_REV_5E
#define KFIR_INT_DMA_VIB						237*4+ALT_ADR_KFIR_REV_5E
#define KFIR_TEST_MODE							252*4+ALT_ADR_KFIR_REV_5E
#define KFIRIDX_TEST_MODE							0		// bit 0 
#define KFIR_TEST_MODE_OFF							0x0
#define KFIR_TEST_MODE_ON							0x1
#define KFIR_PAUSE								253*4+ALT_ADR_KFIR_REV_5E
#define KFIRVAL_RESUME								0x0
#define KFIRVAL_PAUSE								0x1

#define KFIR_EREADY								254*4+ALT_ADR_KFIR_REV_5E
#define KFIRIDX_EREADY_0							0		// bit 0
#define KFIRVAL_ENCODING							0x0
#define KFIRVAL_READY								0x1
#define KFIRVAL_TEST									0x2
#define KFIRVAL_STOP_DETECT						0x3
#define KFIRVAL_SEQ_END								0x4

#define KFIR_RECORD								255*4+ALT_ADR_KFIR_REV_5E
#define KFIRIDX_RECORD								0		// bit 0
#define KFIRVAL_RECORD_STOP						0x0
#define KFIRVAL_RECORD_START						0x1

#endif 

//
// A/V Multiplexer Parameters
//

#define KFIR_MUX_STREAM_TYPE							0				// stream type
#define KFIR_MUX_PROGRAM_NUMBER						1				// number (id) of a program within the transport stream
#define KFIR_MUX_TRANSPORT_STREAM_ID				2				// transport stream id
#define KFIR_MUX_AUDIO_STREAM_TYPE					3				// audio stream type
#define KFIR_MUX_PMT_PID								4				// program map table packet id
#define KFIR_MUX_VIDEO_PID								5				// video packet id
#define KFIR_MUX_AUDIO_PID								6				// audio packet id
#define KFIR_MUX_VIDEO_STREAM_ID						7				// video stream id within the PES packet
#define KFIR_MUX_AUDIO_STREAM_ID						8				// audio stream id within the PES packet
#define KFIR_MUX_VIDEO_BITRATE_LOW					9				// 16 least significant bits of video bit rate
#define KFIR_MUX_VIDEO_BITRATE_HIGH					10				// 16 most significant bits of video bit rate
#define KFIR_MUX_AUDIO_BITRATE_LOW					11				// 16 least significant bits of audio bit rate
#define KFIR_MUX_AUDIO_BITRATE_HIGH					12				// 16 most significant bits of audio bit rate
#define KFIR_MUX_TRANSPORT_BITRATE_LOW				13				// 16 least significant bits of transport bitrate
#define KFIR_MUX_TRANSPORT_BITRATE_HIGH			14				// 16 most significant bits of transport bitrate
#define KFIR_MUX_VPTS_OFFSET_LOW						15				// 16 least significant bits of initial video PTS/DTS offset, ms
#define KFIR_MUX_VPTS_OFFSET_HIGH					16				// 16 most significant bits of initial video PTS/DTS offset, ms
#define KFIR_MUX_APTS_OFFSET_LOW						17				// 16 least significant bits of initial audio PTS/DTS offset, ms
#define KFIR_MUX_APTS_OFFSET_HIGH					18				// 16 most significant bits of initial audio PTS/DTS offset, ms 
#define KFIR_MUX_MAX_VIDEO_PES_SIZE					19				// maximum size of a packetized video elementary stream packet, bytes
#define KFIR_MUX_MAX_AUDIO_PES_SIZE					20				// maximum size of a packetized audio elementary stream packet, bytes
#define KFIR_MUX_PAT_RATE								21				// time interval between two successive PAT/PMT table transmission, ms
#define KFIR_MUX_FRAMES_PER_100_SEC					22				// number of video frames within 100 sec
#define KFIR_MUX_PAT_TABLE_SIZE						23				// size of program association table, bytes
#define KFIR_MUX_PMT_TABLE_SIZE						24				// size of program map table, bytes
#define KFIR_MUX_PCR_BITLIMIT_LOW					25				// PCR bitlimit [15:0]
#define KFIR_MUX_PCR_BITLIMIT_MID					26				// PCR bitlimit [31:16]
#define KFIR_MUX_PCR_BITLIMIT_HIGH					27				// PCR bitlimit [47:32]
#define KFIR_MUX_PCR_STEP_LOW							28				// PCR step	[15:0]
#define KFIR_MUX_PCR_STEP_MID							29				// PCR step [31:16]
#define KFIR_MUX_PCR_STEP_HIGH						30				// PCR step [47:32]
#define KFIR_MUX_PCR_STEP_100SEC_LOW				31				// PCR step 100 sec [15:0]
#define KFIR_MUX_PCR_STEP_100SEC_HIGH				32				// PCR step 100 sec [31:16]
#define KFIR_MUX_PCR_BASE_STEP_LOW					33				// PCR base step [15:0]
#define KFIR_MUX_PCR_BASE_STEP_HIGH					34				// PCR base step [31:16]
#define KFIR_MUX_PCR_BASE_STEP_100SEC_LOW			35				// PCR base step 100 sec [15:0]
#define KFIR_MUX_PCR_BASE_STEP_100SEC_HIGH		36				// PCR base step 100 sec [31:16]
#define KFIR_MUX_PPH_C1									37				// PES packet header constant #1
#define KFIR_MUX_PPH_C2									38				// PES packet header constant #2
#define KFIR_MUX_TPH_C1									39				// transport packet header constant #1
#define KFIR_MUX_TPH_C2									40				// transport packet header constant #2
#define KFIR_MUX_ES_RATE_FLAG							41				// ES rate flag
#define KFIR_MUX_ESCR_FLAG								42				// ESCR flag
#define KFIR_MUX_ESCR_BASE_LOW						43				// ESCR base [15:0]
#define KFIR_MUX_ESCR_BASE_MID						44				// ESCR base [31:16]
#define KFIR_MUX_ESCR_BASE_HIGH						45				// ESCR base [32]
#define KFIR_MUX_ESCR_EXTENTION						46				// ESCR extension
#define KFIR_MUX_SCRAMBLING							47				// scrambling
#define KFIR_MUX_ERROR_INDICATOR						48				// error indicator
#define KFIR_MUX_PRIORITY								49				// priority
#define KFIR_MUX_DTS_PER_FRAME						50				// DTS per frame
#define KFIR_MUX_PTS_DTS_OFFSET						51				// PTS - DTS offset
#define KFIR_MUX_PCR_CORRECTION_CONST_LOW			52				// PCR correction const [10:0]
#define KFIR_MUX_PCR_CORRECTION_CONST_HIGH		53				// PCR correction const [21:11]
#define KFIR_MUX_PCR_BASE_CORRECTION_CONST_LOW	54				// PCR base correction const [10:0]
#define KFIR_MUX_PCR_BASE_CORRECTION_CONST_HIGH	55				// PCR base correction const [21:11]
#define KFIR_MUX_ES_RATE_LOW							56				// ES rate [15:0]
#define KFIR_MUX_ES_RATE_HIGH							57				// ES rate [31:16]
#define KFIR_MUX_PROGRAM_MUX_RATE_LOW				58				// program mux rate (video bit rate + audio bit rate) in BPS/400, bits [15:0]
#define KFIR_MUX_PROGRAM_MUX_RATE_HIGH				59				// program mux rate (video bit rate + audio bit rate) in BPS/400, bits [21:16]
#define KFIR_MUX_RATE_BOUND_LOW						60				// max program mux rate over the entire stream, bits [15:0]
#define KFIR_MUX_RATE_BOUND_HIGH						61				// max program mux rate over the entire stream, bits [21:16]
#define KFIR_MUX_SHF_LOW								62				// bits [15:0] of SHF
#define KFIR_MUX_SHF_HIGH								63				// bits [24:16] of SHF
#define KFIR_MUX_AUDIO_PAR_CONST_LOW				64				// audio par const [15:0]
#define KFIR_MUX_AUDIO_PAR_CONST_HIGH				65				// audio par const [23:16]
#define KFIR_MUX_VIDEO_PAR_CONST_LOW				66				// video par const [15:0]
#define KFIR_MUX_VIDEO_PAR_CONST_HIGH				67				// video par const [23:16]
#define KFIR_MUX_AUDIO_MISS_LATENCY					68				// sets the maximum period of waiting for missing audio data before sending the video frames (in video frames)
#define KFIR_MUX_PAT_TABLE_START						80				// PAT table elements, leftmost element first
#define KFIR_MUX_PMT_TABLE_START						110			// PMT table elements, leftmost element first



#endif // KFIRREG_H
