
// FILE:			library\hardware\mpeg3dec\specific\i5505r.h
// AUTHOR:		D.Heidrich
// COPYRIGHT:	(c) 1998 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		18.06.98
//
// PURPOSE:		Register definition for the STi5505 DVD backend decoder.
//
// HISTORY:

#ifndef I5505R_H
#define I5505R_H

#include "library\common\prelude.h"



//************************************************************************
// Video decoder registers.

#define I5505REG_CFG_MCF		0x00
#define I5505REG_CFG_CCF		0x01


#define I5505REG_VID_CTL		0x02

#define I5505IDX_VID_CTL_ERU		7
#define I5505IDX_VID_CTL_ERS		6
#define I5505IDX_VID_CTL_SPR		5
#define I5505IDX_VID_CTL_ERP		3
#define I5505IDX_VID_CTL_PRS		2
#define I5505IDX_VID_CTL_SRS		1
#define I5505IDX_VID_CTL_EDC		0


#define I5505REG_VID_TIS		0x03

#define I5505TIS_EXE			0
#define I5505TIS_RPT			1
#define I5505TIS_FIS			2
#define I5505TIS_OVW			3
#define I5505TIS_SKP			4, 2
#define I5505TIS_MP2			6			// in VID_PPR2 for chip Cut B


#define I5505REG_VID_PFH		0x04
#define I5505REG_VID_PFV		0x05
#define I5505REG_VID_PPR1		0x06
#define I5505REG_VID_PPR2		0x07


// Display pointers luma and chroma.
#define I5505REG_VID_DFP8		0x0C
#define I5505REG_VID_DFP0		0x0D
#define I5505REG_VID_DFC8		0x58
#define I5505REG_VID_DFC0		0x59

// Reconstructed pointers luma and chroma.
#define I5505REG_VID_RFP8		0x0E
#define I5505REG_VID_RFP0		0x0F
#define I5505REG_VID_RFC8		0x5A
#define I5505REG_VID_RFC0		0x5B

// Forward pointers luma and chroma.
#define I5505REG_VID_FFP8		0x10
#define I5505REG_VID_FFP0		0x11
#define I5505REG_VID_FFC8		0x5C
#define I5505REG_VID_FFC0		0x5D

// Backward pointers luma and chroma.
#define I5505REG_VID_BFP8		0x12
#define I5505REG_VID_BFP0		0x13
#define I5505REG_VID_BFC8		0x5E
#define I5505REG_VID_BFC0		0x5F

// Video bit buffer.
#define I5505REG_VID_VBG8		0x14
#define I5505REG_VID_VBG0		0x15
#define I5505REG_VID_VBL8		0x16
#define I5505REG_VID_VBL0		0x17
#define I5505REG_VID_VBS8		0x18
#define I5505REG_VID_VBS0		0x19
#define I5505REG_VID_VBT8		0x1A
#define I5505REG_VID_VBT0		0x1B

// Audio bit buffer.
#define I5505REG_VID_ABG8		0x1C
#define I5505REG_VID_ABG0		0x1D
#define I5505REG_VID_ABL8		0x1E
#define I5505REG_VID_ABL0		0x1F
#define I5505REG_VID_ABS8		0x20
#define I5505REG_VID_ABS0		0x21
#define I5505REG_VID_ABT8		0x22
#define I5505REG_VID_ABT0		0x23


#define I5505REG_VID_DFS		0x24
#define I5505REG_VID_DFW		0x25

#define I5505REG_VID_XFW		0x28

#define I5505REG_VID_SCN		0x29

#define I5505REG_VID_OTP		0x2A
#define I5505REG_VID_OBP		0x2B

#define I5505REG_VID_PAN8		0x2C
#define I5505REG_VID_PAN0		0x2D

#define I5505REG_CKG_PLL		0x30
#define I5505REG_CKG_CFG		0x31
#define I5505REG_CKG_SMC		0x32
#define I5505REG_CKG_LNK		0x33
#define I5505REG_CKG_AUD		0x35
#define I5505REG_CKG_MCK		0x36
#define I5505REG_CKG_AUX		0x37

#define I5505REG_CFG_DRC		0x38

#define I5505REG_CFG_GCF		0x3A

#define I5505REG_PES_CF1		0x40
#define I5505REG_PES_CF2		0x41


// Bit definitions in status and interrupt registers.
#define I5505STA_SCH			MKFLAG(0)	// start code hit
#define I5505STA_BFF			MKFLAG(1)	// video bit FIFO full
#define I5505STA_HFE			MKFLAG(2)	// header FIFO empty
#define I5505STA_BBF			MKFLAG(3)	// bit buffer full
#define I5505STA_BBE			MKFLAG(4)	// bit buffer empty
#define I5505STA_VSB			MKFLAG(5)	// VSYNC bottom
#define I5505STA_VST			MKFLAG(6)	// VSYNC top
#define I5505STA_PSD			MKFLAG(7)	// pipeline starting to decode
#define I5505STA_PID			MKFLAG(9)	// pipeline idle
#define I5505STA_ERC			MKFLAG(10)	// error concealment
#define I5505STA_PNC			MKFLAG(11)	// panic mode
#define I5505STA_HFF			MKFLAG(12)	// header FIFO full
#define I5505STA_BMI			MKFLAG(13)	// block move idle
#define I5505STA_SER			MKFLAG(14)	// severe error or overflow error
#define I5505STA_PDE			MKFLAG(15)	// picture decoding error or underflow error
#define I5505STA_ABE			MKFLAG(16)	// audio bit buffer empty
#define I5505STA_AFF			MKFLAG(17)	// audio FIFO full
#define I5505STA_SFF			MKFLAG(18)	// sub picture FIFO full
#define I5505STA_ABF			MKFLAG(19)	// audio bit buffer full
#define I5505STA_ERR			MKFLAG(22)	// inconsistency error in PES parser
#define I5505STA_NDP			MKFLAG(23)	// new discarded packet


#define I5505REG_CFG_CDR		0x44

#define I5505REG_VID_FRZ		0x45

#define I5505REG_VID_MLU		0x54   // cut 2.0
#define I5505REG_VID_MCH		0x55   // cut 2.0
#define I5505REG_VID_VFL		0x54   // other cuts
#define I5505REG_VID_VFC		0x55   // other cuts


#define I5505REG_VID_ITM8		0x60   // interrupt mask
#define I5505REG_VID_ITM0		0x61
#define I5505REG_VID_ITM16		0x3C

#define I5505REG_VID_ITS8		0x62   // interrupt status
#define I5505REG_VID_ITS0		0x63
#define I5505REG_VID_ITS16		0x3D

#define I5505REG_VID_STA8		0x64   // status
#define I5505REG_VID_STA0		0x65
#define I5505REG_VID_STA16		0x3B


#define I5505REG_VID_HDF		0x66


#define I5505REG_VID_CDCNT		0x67
#define I5505REG_VID_SCDCNT	0x68

#define I5505REG_VID_HDS		0x69


#define I5505REG_VID_LSO		0x6A
#define I5505REG_VID_LSR0		0x6B
#define I5505REG_VID_CSO		0x6C
#define I5505REG_VID_LSR8		0x6D

#define I5505REG_VID_YDO		0x6E
#define I5505REG_VID_YDS		0x6F
#define I5505REG_VID_XDO8		0x70
#define I5505REG_VID_XDO0		0x71
#define I5505REG_VID_XDS8		0x72
#define I5505REG_VID_XDS0		0x73


#define I5505REG_VID_DCF8		0x74
#define I5505REG_VID_DCF0		0x75

#define I5505IDX_VID_DCF_SPP	14
#define I5505IDX_VID_DCF_BLL	13
#define I5505IDX_VID_DCF_BFL	12
#define I5505IDX_VID_DCF_FNF	11
#define I5505IDX_VID_DCF_FLY	10
#define I5505IDX_VID_DCF_ORF	9
#define I5505IDX_VID_DCF_PXD	6
#define I5505IDX_VID_DCF_EVD	5
#define I5505IDX_VID_DCF_EOS	4
#define I5505IDX_VID_DCF_DSR	3
#define I5505REG_VID_DCF_VFC	0, 3


#define I5505REG_VID_QMW		0x76

// Must be in test mode to access register 0x79
#define I5505REG_VID_TEST		0x77 
#define I5505REG_VID_SET_TEST_MODE		1 // value for above register
#define I5505REG_VID_SET_FUNCTIONAL_MODE	0 // value for above register

#define I5505REG_VID_REV		0x78

// fallback mode enable (set up digital video output)
#define I5505REG_VID_DIGOUT			0x79
#define I5505REG_VID_DIGOUT_ENABLE_CUT2		0x40 // value for above register
#define I5505REG_VID_DIGOUT_ENABLE_CUT3AND4	0xC0 // value for above register





//************************************************************************
// Audio decoder registers.

#define I5505REG_VERSION		0x00
#define I5505REG_IDENT			0x01

/*#define I5505REG_FBADDRL		0x02	// ???
#define I5505REG_FBADDRH		0x03	// ???
#define I5505REG_FBDATA			0x04	// ???
*/
#define I5505REG_SFREQ			0x05
#define I5505REG_EMPH			0x06


#define I5505REG_INTEL			0x07
#define I5505REG_INTEH			0x08
#define I5505REG_INTL			0x09
#define I5505REG_INTH			0x0a
//#define I5505REG_SETINT			0x0b	// ???

#define I5505INTIDX_SYNC		0
#define I5505INTIDX_HEADER		1
#define I5505INTIDX_ERROR		2
#define I5505INTIDX_SFREQ		3
#define I5505INTIDX_DEEMPH		4
#define I5505INTIDX_BOF			5
#define I5505INTIDX_PTS			6
#define I5505INTIDX_ANC			7
#define I5505INTIDX_PCM			8
#define I5505INTIDX_FBFULL		9
#define I5505INTIDX_FBEMPTY	10
#define I5505INTIDX_FIFO		11
//#define I5505INTIDX_BREAK		15	// ???


#define I5505REG_SINSETUP		0x0c

#define I5505IDX_INPUTSETUP	0, 2
#define I5505IDX_REQPOL			2

#define I5505INSET_PARALLEL	0
#define I5505INSET_SERIAL		1
#define I5505INSET_ANALOG		3


#define I5505REG_CANSETUP		0x0d

#define I5505CANSET_PADDING	0
#define I5505CANSET_LEFT1		1
#define I5505CANSET_FALLEDGE	2
#define I5505CANSET_SLOTCNT32	3


#define I5505REG_DATAIN			0x0e
#define I5505REG_ERROR			0x0f
#define I5505REG_RESET			0x10


/*#define I5505REG_PLLSYS			0x11

#define I5505IDXPLL_BYPASS		6
#define I5505IDXPLL_DISABLED	5
#define I5505IDXPLL_DIV			0, 4
*/


#define I5505REG_PLLPCM			0x12	// has additional bit 2
#define I5505IDX_256				2
#define I5505IDX_RP				1
#define I5505IDX_DP				0

#define I5505REG_PLAY			0x13
#define I5505REG_MUTE			0x14
//#define I5505REG_REQ				0x15	// ???
//#define I5505REG_ACK				0x16	// ???


#define I5505REG_PLLMASK		0x18


#define I5505REG_SYNCSTATUS	0x40

#define I5505IDX_FRAMESTATUS	0, 2

#define I5505FRMSTAT_SEARCH	0
#define I5505FRMSTAT_WAIT		1
#define I5505FRMSTAT_SYNC		2

#define I5505IDX_PACKETSTATUS	2, 2

#define I5505PCKSTAT_SEARCH	0
#define I5505PCKSTAT_WAIT		1
#define I5505PCKSTAT_SYNC		2


#define I5505REG_ANCCOUNT		0x41
#define I5505REG_HEAD24			0x42
#define I5505REG_HEAD16			0x43
#define I5505REG_HEAD8			0x44
#define I5505REG_HEAD0			0x45
#define I5505REG_PTS33			0x46
#define I5505REG_PTS24			0x47
#define I5505REG_PTS16			0x48
#define I5505REG_PTS8			0x49
#define I5505REG_PTS0			0x4a
//#define I5505REG_USER1			0x4b	// ???


#define I5505REG_STREAMSEL		0x4c

#define I5505STRSEL_PES			0
#define I5505STRSEL_PESDVD		1
#define I5505STRSEL_PACKMPEG1	2
#define I5505STRSEL_ELEMNTRY	3
#define I5505STRSEL_RES3520A	4
#define I5505STRSEL_SPDIF_IN	5


#define I5505REG_DECODESEL		0x4d

#define I5505DECSEL_AC3			0
#define I5505DECSEL_MPEG		1
#define I5505DECSEL_MPEGEXT	2                                                                
#define I5505DECSEL_LPCM		3
#define I5505DECSEL_PINKNOISE	4
#define I5505DECSEL_CD_DA		5
#define I5505DECSEL_DTS			6

#define I5505REG_BAL_LR			0x4e   // only until cut 2.1
#define I5505REG_VOLUME0		0x4e   // cut 3.1 and higher


#define I5505REG_PACKETLOCK	0x4f
#define I5505REG_AUDIOIDEN		0x50
#define I5505REG_AUDIOID		0x51
#define I5505REG_AUDIOIDEXT	0x52
#define I5505REG_SYNCLOCK		0x53


#define I5505REG_PCMDIVIDER	0x54
#define I5505REG_PCMCONFIG		0x55

#define I5505IDX_PCMORDER		6
#define I5505IDX_PCMDIFF		5
#define I5505IDX_INVLRCLK		4
#define I5505IDX_FORMAT			3
#define I5505IDX_INVSCLK		2
#define I5505IDX_PCMPREC		0, 2

#define I5505PCMPREC_16			0
#define I5505PCMPREC_18			1
#define I5505PCMPREC_20			2
#define I5505PCMPREC_24			3


#define I5505REG_PCMCROSS		0x56


// These registers are not documented for the 5505, but they must be written
// in order to have the audio decoder working.
#define I5505REG_LDLY			0x57
#define I5505REG_RDLY			0x58
#define I5505REG_CDLY			0x59
#define I5505REG_SUBDLY			0x5a
#define I5505REG_LSDLY			0x5b
#define I5505REG_RSDLY			0x5c
#define I5505REG_DLYUPDATE		0x5d


#define I5505REG_IEC958CMD		0x5e
#define I5505IECMD_OFF			0
#define I5505IECMD_MUTED		1
#define I5505IECMD_PCM			2
#define I5505IECMD_ENCODED		3

#define I5505REG_IEC958CAT		0x5f

#define I5505REG_IEC958CONF	0x60
#define I5505IDX_IECDIV			0, 5
#define I5505IDX_IECIDLESTATE	5
#define I5505IDX_IECSLR			5		// Swap left/right on SPDIF. New for Chipcut > 3.1
#define I5505IDX_IECSYNCMT		6
#define I5505IDX_IECAUTOLATOFF	7		// Auto latency off (1)/on (0)

#define I5505REG_IEC958STATUS	0x61
#define I5505IDX_IEC44KHZ		3, 4
#define I5505IDX_IECPREEMPH	2
#define I5505IDX_IECCOPY		1
#define I5505IDX_IECCOMPRESS	0


#define I5505REG_PDEC			0x62

#define I5505IDX_SRSTSENA		6
#define I5505IDX_DEM				5
#define I5505IDX_DCF				4
#define I5505IDX_MDRC			1
#define I5505IDX_PL				0


#define I5505REG_BAL_SUR		0x63   // only until cut 2.1
#define I5505REG_VOLUME1		0x63   // cut 3.1 and higher


#define I5505REG_PLAB			0x64

#define I5505IDX_SRSTSCFG		2, 5


#define I5505REG_PLDWNX			0x65

#define I5505PLDWNX_DISABLED	0
#define I5505PLDWNX_3_0			3
#define I5505PLDWNX_2_1			4
#define I5505PLDWNX_3_1			5
#define I5505PLDWNX_2_2			6
#define I5505PLDWNX_3_2			7


#define I5505REG_OCFG			0x66

#define I5505IDX_OCFG			0, 3
#define I5505IDX_LFE				7


#define I5505REG_PCMSCALE		0x67   // only until cut 2.1
#define I5505REG_CHAN_IDX		0x67   // cut 3.1 and higher


#define I5505REG_LFE				0x68	


// Meaning of register depends on AC3 or MPEG2 audio.
#define I5505REG_COMPMOD		0x69
#define I5505REG_PROGNUMBER	0x69

#define I5505CMPMOD_CUSTOMA	0
#define I5505CMPMOD_CUSTOMD	1
#define I5505CMPMOD_LINEOUT	2
#define I5505CMPMOD_RFMODE		3


#define I5505REG_HDR				0x6a
// meaning of this register in MPEG mode
#define I5505REG_DRC				0x6a

#define I5505REG_LDR				0x6b
#define I5505REG_RPC				0x6c


#define I5505REG_KARAMODE		0x6d

#define I5505KARAMD_AWARE		0
#define I5505KARAMD_MLTCHANL	3
#define I5505KARAMD_DOWNMIX	7
#define I5505KARAMD_REPRDC_1	5
#define I5505KARAMD_REPRDC_2	6
#define I5505KARAMD_REPRDC_12	4

// Definition of this register in MPEG mode:
#define I5505REG_MCOFF			0x6d
#define I5505IDX_MC				0
#define I5505IDX_DEN				4

#define I5505REG_DUALMODE		0x6e

#define I5505DLMD_STEREO		0
#define I5505DLMD_CHANNEL1		1
#define I5505DLMD_CHANNEL2		2
#define I5505DLMD_MIX			3


#define I5505REG_DOWNMIX		0x6f

#define I5505DWNMX_2_0_COMP	0
#define I5505DWNMX_1_0			1
#define I5505DWNMX_2_0			2
#define I5505DWNMX_3_0			3
#define I5505DWNMX_2_1			4
#define I5505DWNMX_3_1			5
#define I5505DWNMX_2_2			6
#define I5505DWNMX_3_2			7

// Definition for MPEG and MPEG Karaoke

#define I5505DMPG_1_0			0
#define I5505DMPG_2_0			1
#define I5505DMPG_3_0			2
#define I5505DMPG_2_1			3
#define I5505DMPG_3_1			4
#define I5505DMPG_2_2			5
#define I5505DMPG_3_2			6
#define I5505DMPG_2_0_DS		9
#define I5505DMPG_KARA_2_0		10
#define I5505DMPG_KARA_A1		11
#define I5505DMPG_KARA_A2		12
#define I5505DMPG_KARA_NONE	13

// Definition for DOWNMIX bits in Pink Noise mode

#define I5505DMPN_L				0
#define I5505DMPN_R				1
#define I5505DMPN_C				2
#define I5505DMPN_LFE			3
#define I5505DMPN_LS				4
#define I5505DMPN_RS				5


// Downsampling mode

#define I5505REG_DWSMODE		0x70

#define I5505DWSM_AUTO			0
#define I5505DWSM_FORCE			1
#define I5505DWSM_SUPPRESS		2


#define I5505REG_SOFTVER		0x71
#define I5505REG_RUN				0x72


#define I5505REG_SKIP_MUTE_CMD	0x73	// ???

#define I5505IDX_SMUT			0
#define I5505IDX_SKP				1
#define I5505IDX_BLK				2
#define I5505IDX_PAU				3
#define I5505IDX_REB				4
#define I5505IDX_MU				5


#define I5505REG_SKIP_MUTE_VALUE	0x74	// ???


#define I5505REG_IEC958REPTIME	0x75


#define I5505REG_AC3STAT0		0x76

#define I5505IDX_FSCOD			5, 2
#define I5505IDX_FRMSZCOD		0, 5


#define I5505REG_AC3STAT1		0x77

#define I5505IDX_LFEPRESENT	3
#define I5505IDX_ACMOD			0, 3


#define I5505REG_AC3STAT2		0x78

#define I5505IDX_BSMOD			5, 3
#define I5505IDX_BSID			0, 5


#define I5505REG_AC3STAT3		0x79	// ???

#define I5505IDX_CMIXLEVEL		2, 2
#define I5505IDX_SURMIXLEVEL	0, 2


#define I5505REG_AC3STAT4		0x7a

#define I5505IDX_DSURMOD		3, 2
#define I5505IDX_COPYRIGHT		2
#define I5505IDX_ORIGBS			1
#define I5505IDX_LANGCODE		0


#define I5505REG_LANGCODE		0x7b
#define I5505REG_DIALNORM		0x7c


#define I5505REG_AC3STAT7		0x7d

#define I5505IDX_ROOMTYPE		6, 2
#define I5505IDX_MIXLEVEL		1, 5
#define I5505IDX_AUDPRODIE		0


#define I5505REG_IEC958LATENCY		0x7e


//#define I5505REG_PCMFCROSS		0x7f
#define I5505REG_IEC958DTDI	0x7f

#define I5505IDX_PFC				7
#define I5505IDX_DTD				6
#define I5505IDX_INF				0, 5


//************************************************************************
// Subpicture decoder registers.

#define I5505REG_SPD_CTL1		0x00

#define I5505IDX_SPD_CTL1_S	0
#define I5505IDX_SPD_CTL1_D	1
#define I5505IDX_SPD_CTL1_V	2
#define I5505IDX_SPD_CTL1_H	3
#define I5505IDX_SPD_CTL1_B	4
#define I5505IDX_SPD_CTL1_SPP	5
#define I5505IDX_SPD_CTL1_TOP	6
#define I5505IDX_SPD_CTL1_BOT	7


#define I5505REG_SPD_RST		0x01

#define I5505REG_SPD_CTL2		0x02

#define I5505REG_SPD_LUT		0x03

#define I5505REG_SPD_XDO8		0x04
#define I5505REG_SPD_XDO0		0x05

#define I5505REG_SPD_YDO8		0x06
#define I5505REG_SPD_YDO0		0x07

#define I5505REG_SPD_XD18		0x08
#define I5505REG_SPD_XD10		0x09

#define I5505REG_SPD_YD18		0x0a
#define I5505REG_SPD_YD10		0x0b

#define I5505REG_SPD_HLSX8		0x0c
#define I5505REG_SPD_HLSX0		0x0d

#define I5505REG_SPD_HLSY8		0x0e
#define I5505REG_SPD_HLSY0		0x0f

#define I5505REG_SPD_HLEX8		0x10
#define I5505REG_SPD_HLEX0		0x11

#define I5505REG_SPD_HLEY8		0x12
#define I5505REG_SPD_HLEY0		0x13

#define I5505REG_SPD_HLRCO8	0x14
#define I5505REG_SPD_HLRCO0	0x15

#define I5505REG_SPD_HLRC8		0x16
#define I5505REG_SPD_HLRC0		0x17

#define I5505REG_SPD_SXDO8		0x24
#define I5505REG_SPD_SXDO0		0x25

#define I5505REG_SPD_SYDO8		0x26
#define I5505REG_SPD_SYDO0		0x27

#define I5505REG_SPD_SXD18		0x28
#define I5505REG_SPD_SXD10		0x29

#define I5505REG_SPD_SYD18		0x2a
#define I5505REG_SPD_SYD10		0x2b

#define I5505REG_SPD_SPB8		0x50
#define I5505REG_SPD_SPB0		0x51

#define I5505REG_SPD_SPE8		0x52
#define I5505REG_SPD_SPE0		0x53

#define I5505REG_SPD_SPREAD	0x4e
#define I5505REG_SPD_SPWRITE	0x4f










//************************************************************************

#define I3520AREG_CFG_MCF		0x00
#define I3520AREG_CFG_CCF		0x01
#define I3520AREG_VID_CTL		0x02

#define I3520AIDX_VID_CTL_ERU		7
#define I3520AIDX_VID_CTL_ERS		6
#define I3520AIDX_VID_CTL_CFB		5
#define I3520AIDX_VID_CTL_DEC		4
#define I3520AIDX_VID_CTL_ERP		3
#define I3520AIDX_VID_CTL_PRS		2
#define I3520AIDX_VID_CTL_SRS		1
#define I3520AIDX_VID_CTL_EDC		0


#define I3520AREG_VID_TIS		0x03  

#define I3520ATIS_EXE			0
#define I3520ATIS_RPT			1
#define I3520ATIS_FIS			2
#define I3520ATIS_OVW			3
#define I3520ATIS_SKP			4, 2
#define I3520ATIS_MP2			6

#define I3520AREG_VID_PFH		0x04
#define I3520AREG_VID_PFV		0x05
#define I3520AREG_VID_PPR1		0x06
#define I3520AREG_VID_PPR2		0x07

#define I3520AREG_CFG_MRF		0x08
#define I3520AREG_CFG_MWF		0x08
#define I3520AREG_CFG_BMS		0x09
#define I3520AREG_CFG_MRP		0x0A
#define I3520AREG_CFG_MWP		0x0B
#define I3520AREG_VID_DFP8		0x0C
#define I3520AREG_VID_DFP0		0x0D
#define I3520AREG_VID_RFP8		0x0E
#define I3520AREG_VID_RFP0		0x0F

#define I3520AREG_VID_FFP8		0x10
#define I3520AREG_VID_FFP0		0x11
#define I3520AREG_VID_BFP8		0x12
#define I3520AREG_VID_BFP0		0x13
#define I3520AREG_VID_VBG8		0x14
#define I3520AREG_VID_VBG0		0x15
#define I3520AREG_VID_VBL8		0x16
#define I3520AREG_VID_VBL0		0x17

#define I3520AREG_VID_VBS8		0x18
#define I3520AREG_VID_VBS0		0x19
#define I3520AREG_VID_VBT8		0x1A
#define I3520AREG_VID_VBT0		0x1B
#define I3520AREG_AUD_ABG8		0x1C
#define I3520AREG_AUD_ABG0		0x1D
#define I3520AREG_AUD_ABL8		0x1E
#define I3520AREG_AUD_ABL0		0x1F

#define I3520AREG_AUD_ABS8		0x20
#define I3520AREG_AUD_ABS0		0x21
#define I3520AREG_AUD_ABT8		0x22
#define I3520AREG_AUD_ABT0		0x23
#define I3520AREG_VID_DFS		0x24
#define I3520AREG_VID_DFW		0x25
#define I3520AREG_VID_DFA		0x26
#define I3520AREG_VID_XFS		0x27

#define I3520AREG_VID_XFW		0x28
#define I3520AREG_VID_XFA		0x29
#define I3520AREG_VID_OTP		0x2A
#define I3520AREG_VID_OBP		0x2B
#define I3520AREG_VID_PAN8		0x2C
#define I3520AREG_VID_PAN0		0x2D
#define I3520AREG_VID_SCN8		0x2E
#define I3520AREG_VID_SCN0		0x2F

#define I3520AREG_CKG_PLL		0x30
#define I3520AREG_CKG_CFG		0x31
#define I3520AREG_CKG_AUD		0x32
#define I3520AREG_CKG_VID		0x33
#define I3520AREG_CKG_PIX		0x34
#define I3520AREG_CKG_PCM		0x35
#define I3520AREG_CKG_MCK		0x36
#define I3520AREG_CKG_AUX		0x37

#define I3520AREG_CFG_DRC		0x38
#define I3520AREG_CFG_BFS		0x39
#define I3520AREG_CFG_GCF		0x3A
#define I3520AREG_VID_STA16	0x3B
#define I3520AREG_VID_ITM16	0x3C
#define I3520AREG_VID_ITS16	0x3D

#define I3520AREG_PES_CF1		0x40
#define I3520AREG_PES_CF2		0x41
#define I3520AREG_PES_SPF		0x42
#define I3520AREG_PES_STA		0x43
#define I3520AREG_PES_SC1		0x44
#define I3520AREG_PES_SC2		0x45
#define I3520AREG_PES_SC3		0x46
#define I3520AREG_PES_SC4		0x47

#define I3520AREG_PES_SC5		0x48
#define I3520AREG_PES_TS1		0x49
#define I3520AREG_PES_TS2		0x4A
#define I3520AREG_PES_TS3		0x4B
#define I3520AREG_PES_TS4		0x4C
#define I3520AREG_PES_TS5		0x4D

#define I3520ASTA_SCH			MKFLAG(0)	//	StartCodeHit
#define I3520ASTA_BFF			MKFLAG(1)	// 
#define I3520ASTA_HFE			MKFLAG(2)	// HeaderFifoEmpty
#define I3520ASTA_BBF			MKFLAG(3)	// BitBufferFull
#define I3520ASTA_BBE			MKFLAG(4)	// BitBufferEmpty
#define I3520ASTA_VSB			MKFLAG(5)	// VSYNCBottom
#define I3520ASTA_VST			MKFLAG(6)	// VSYNCTop
#define I3520ASTA_PSD			MKFLAG(7)	// PipelineStartingToDecode
#define I3520ASTA_PER			MKFLAG(8)	// PipelineError
#define I3520ASTA_PID			MKFLAG(9)	// PipelineIdle
#define I3520ASTA_WFE			MKFLAG(10)	// WriteFIFOEmpty
#define I3520ASTA_RFF			MKFLAG(11)	// ReadFIFOFull
#define I3520ASTA_HFF			MKFLAG(12)	// HeaderFIFOFull
#define I3520ASTA_BMI			MKFLAG(13)	// BlockMoveIdle
#define I3520ASTA_SER			MKFLAG(14)	// SevererErrorOrOverflowError
#define I3520ASTA_PDE			MKFLAG(15)	// PictureDecodingErrorOrUnderflowError
#define I3520ASTA_ABE			MKFLAG(16)	// AudioBitBufferEmpty
#define I3520ASTA_WFN			MKFLAG(17)	// WriteFIFONotFull
#define I3520ASTA_RFN			MKFLAG(18)	// ReadFIFONotEmpty
#define I3520ASTA_ABF			MKFLAG(19)	// AudioBitBufferFull
#define I3520ASTA_HAF			MKFLAG(20)	// HistoryFIFOAlmostFull
#define I3520ASTA_SCR			MKFLAG(21)	// NewSCRLatched
#define I3520ASTA_ERR			MKFLAG(22)	// InconsistencyErrorInPESParser
#define I3520ASTA_NDP			MKFLAG(23)	// NewDiscardedPacket

#define I3520AREG_VID_ITM8		0x60
#define I3520AREG_VID_ITM0		0x61
#define I3520AREG_VID_ITS8		0x62
#define I3520AREG_VID_ITS0		0x63
#define I3520AREG_VID_STA8		0x64
#define I3520AREG_VID_STA0		0x65
#define I3520AREG_VID_HDF		0x66
#define I3520AREG_VID_CDCNT	0x67
#define I3520AREG_VID_SCDCNT	0x68 
#define I3520AREG_VID_HDS		0x69
#define I3520AREG_VID_LSO		0x6A
#define I3520AREG_VID_LSR0		0x6B
#define I3520AREG_VID_CSO		0x6C
#define I3520AREG_VID_LSR8		0x6D
#define I3520AREG_VID_YDO		0x6E
#define I3520AREG_VID_YDS		0x6F
#define I3520AREG_VID_XDO8		0x70
#define I3520AREG_VID_XDO0		0x71
#define I3520AREG_VID_XDS8		0x72
#define I3520AREG_VID_XDS0		0x73
#define I3520AREG_VID_DCF8		0x74
#define I3520AREG_VID_DCF0		0x75    

#define I3520AREG_VID_DCF_OAD	14, 2
#define I3520AREG_VID_DCF_OAM	13
#define I3520AREG_VID_DCF_FRZ	12
#define I3520AREG_VID_DCF_DAM	9, 3
#define I3520AREG_VID_DCF_FLD	8
#define I3520AREG_VID_DCF_USR	7
#define I3520AREG_VID_DCF_PXD	6
#define I3520AREG_VID_DCF_EVD	5
#define I3520AREG_VID_DCF_EOS	4
#define I3520AREG_VID_DCF_DSE	3
#define I3520AREG_VID_DCF_VCF	0, 3

#define I3520AREG_VID_QMW		0x76

#define I3520AREG_AUD_ANC0		0x86
#define I3520AREG_AUD_ANC8		0x87

#define I3520AREG_AUD_ANC16	0x88
#define I3520AREG_AUD_ANC24	0x89
#define I3520AREG_AUD_ESC0		0x8A
#define I3520AREG_AUD_ESC8		0x8B
#define I3520AREG_AUD_ESC16	0x8C
#define I3520AREG_AUD_ESC24	0x8D
#define I3520AREG_AUD_ESC32	0x8E
#define I3520AREG_AUD_ESCX0	0x8F

#define I3520AREG_STC_INC		0x90
#define I3520AREG_AUD_LRP		0x91
#define I3520AREG_AUD_FFL0		0x94
#define I3520AREG_AUD_FFL8		0x95
#define I3520AREG_AUD_P18		0x96

#define I3520AREG_AUD_CDI0		0x98
#define I3520AREG_AUD_FOR		0x99
#define I3520AREG_AUD_ITR0		0x9A
#define I3520AREG_AUD_ITR8		0x9B
#define I3520AREG_AUD_ITM0		0x9C
#define I3520AREG_AUD_ITM8		0x9D
#define I3520AREG_AUD_LCA		0x9E
#define I3520AREG_AUD_EXT		0x9F

#define I3520AREG_AUD_RCA		0xA0
#define I3520AREG_STC_CTL		0xA1
#define I3520AREG_AUD_SID		0xA2
#define I3520AREG_AUD_SYN		0xA3
#define I3520AREG_AUD_IDE		0xA4
#define I3520AREG_AUD_SCM		0xA5
#define I3520AREG_AUD_SYS		0xA6
#define I3520AREG_AUD_SYE		0xA7

#define I3520AREG_AUD_LCK		0xA8
#define I3520AREG_AUD_CRC		0xAA
#define I3520AREG_AUD_SEM		0xAC
#define I3520AREG_AUD_PLY		0xAE

#define I3520AREG_AUD_MUT		0xB0
#define I3520AREG_AUD_SKP		0xB2
#define I3520AREG_AUD_ISS		0xB6

#define I3520AREG_AUD_ORD		0xB8
#define I3520AREG_AUD_LAT		0xBC

#define I3520AREG_AUD_RES		0xC0
#define I3520AREG_AUD_RST		0xC2
#define I3520AREG_AUD_SFR		0xC4
#define I3520AREG_AUD_DEM		0xC6

#define I3520AREG_STC_DIV0		0xC8
#define I3520AREG_STC_DIV8		0xC9
#define I3520AREG_STC0			0xCA
#define I3520AREG_STC8			0xCB
#define I3520AREG_STC16			0xCC
#define I3520AREG_STC24			0xCD
#define I3520AREG_STC32			0xCE

#define I3520AREG_AUD_IFT		0xD2
#define I3520AREG_AUD_SCP		0xD3

#define I3520AREG_AUD_ITS		0xDB
#define I3520AREG_AUD_IMS		0xDC
#define I3520AREG_AUD_HDR0		0xDE
#define I3520AREG_AUD_HDR8		0xDF

#define I3520AREG_AUD_HDR16	0xE0
#define I3520AREG_AUD_HDR24	0xE1
#define I3520AREG_AUD_PTS0		0xE2
#define I3520AREG_AUD_PTS8		0xE3
#define I3520AREG_AUD_PTS16	0xE4
#define I3520AREG_AUD_PTS24	0xE5
#define I3520AREG_AUD_PTS32	0xE6

#define I3520AREG_BAL_LIMx		0xE9
#define I3520AREG_BAL_LIMy		0xEB
#define I3520AREG_AUD_ADA		0xEC
#define I3520AREG_AUD_REV		0xED
#define I3520AREG_AUD_DIV		0xEE
#define I3520AREG_AUD_DIF		0xEF

#define I3520AREG_AUD_BBE		0xF0

#define I3520AREG_VID_REV		0xF8

#endif
