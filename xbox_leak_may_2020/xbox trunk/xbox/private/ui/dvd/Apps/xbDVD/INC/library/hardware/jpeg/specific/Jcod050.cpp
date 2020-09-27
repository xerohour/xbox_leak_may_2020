
// FILE:      library\hardware\jpeg\generic\jcod050.cpp
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   12.01.96
//
// PURPOSE: Implementation of JPEG coder with bit rate control using the ZR36050.
//
// HISTORY:

#include "jcod050.h"
#include "library\lowlevel\timer.h"

#include "library\support\gfxdebug\gfxdebug.h"





#define TABLE_PRELOAD_TIMEOUT  200





// This is the bit field that represents the video formats we support.

static DWORD VideoFormatBitField = (1 << VFMT_YUV_422) | (1 << VFMT_YUV_411) | (1 << VFMT_YUV_420);





// Definitions of quantities for baseline JPEG. This section can be modified
// if another compression scheme is needed.

#define NO_OF_COMPONENTS          3   // Y, U and V
#define BASELINE_PRECISION        8
#define NO_OF_SCAN1_COMPONENTS    3   // Y, U and V



// Indexes of 050 internal registers and of functional locations in the 050's
// internal memory.

#define GO                  0x000
// NOT USED                 0x001
#define HARDWARE            0x002
#define MODE                0x003
#define OPTIONS             0x004
#define MBCV_050            0x005
#define MARKERS_EN          0x006
#define INT_REQ_0           0x007
#define INT_REQ_1           0x008
#define TCV_NET_HI          0x009
#define TCV_NET_MH          0x00a
#define TCV_NET_ML          0x00b
#define TCV_NET_LO          0x00c
#define TCV_DATA_HI         0x00d
#define TCV_DATA_MH         0x00e
#define TCV_DATA_ML         0x00f
#define TCV_DATA_LO         0x010
#define SF_HI               0x011
#define SF_LO               0x012
#define AF_HI               0x013
#define AF_M                0x014
#define AF_LO               0x015
#define ACV_HI              0x016
#define ACV_MH              0x017
#define ACV_ML              0x018
#define ACV_LO              0x019
#define ACT_HI              0x01a
#define ACT_MH              0x01b
#define ACT_ML              0x01c
#define ACT_LO              0x01d
#define ACV_TRUN_HI         0x01e
#define ACV_TRUN_MH         0x01f
#define ACV_TRUN_ML         0x020
#define ACV_TRUN_LO         0x021
#define STATUS_0            0x02e
#define STATUS_1            0x02f



// Indexes of JPEG baseline markers segment in the 050's internal memory.

#define SOF_INDX            0x040
#define SOS1_INDX           0x07a
#define SOS2_INDX           0x08a
#define SOS3_INDX           0x09a
#define SOS4_INDX           0x0aa
#define DRI_INDX            0x0c0
#define DNL_INDX            0x0c6
#define DQT_INDX            0x0cc
#define DHT_INDX            0x1d4
#define APP_INDX            0x380
#define COM_INDX            0x3c0



// Values of JPEG baseline markers.

#define SOI_MARKER          0xD8
#define EOI_MARKER          0xD9
#define SOS_MARKER          0xDA
#define DQT_MARKER          0xDB
#define DNL_MARKER          0xDC
#define DRI_MARKER          0xDD
#define SOF0_MARKER         0xC0
#define DHT_MARKER          0xC4
#define APP_MARKER          0xE0
#define COM_MARKER          0xFE



// Bit masks in the 050's HARDWARE register.

#define BSWD_MASK           0x80
#define BSWD                0x80
#define NO_BSWD             0x00

#define MSTR_MASK           0x40
#define MSTR                0x40
#define NO_MSTR             0x00

#define DMA_MASK            0x20
#define DMA                 0x20
#define NO_DMA              0x00

#define CFIS_MASK           0x1C
#define CFIS_1_CLK          0x00
#define CFIS_2_CLK          0x04
#define CFIS_3_CLK          0x08
#define CFIS_4_CLK          0x0C
#define CFIS_5_CLK          0x10
#define CFIS_6_CLK          0x14
#define CFIS_7_CLK          0x18
#define CFIS_8_CLK          0x1C

#define BELE_MASK           0x01
#define BELE                0x01
#define NO_BELE             0x00



// Bit masks in the 050's MODE register.

#define COMP_MASK           0x80
#define COMP                0x80
#define NO_COMP             0x00

#define ATP_MASK            0x40
#define ATP                 0x40
#define NO_ATP              0x00

#define PASS2_MASK          0x20
#define PASS2               0x20
#define NO_PASS2            0x00

#define TLM_MASK            0x10
#define TLM                 0x10
#define NO_TLM              0x00

#define DCONLY_MASK         0x08
#define DCONLY              0x08
#define NO_DCONLY           0x00

#define BRC_MASK            0x04
#define BRC                 0x04
#define NO_BRC              0x00



// Bit masks in the 050's OPTIONS register.

#define NSCN_MASK           0xE0
#define NSCN_1              0x00
#define NSCN_2              0x20
#define NSCN_3              0x40
#define NSCN_4              0x60
#define NSCN_5              0x80
#define NSCN_6              0xA0
#define NSCN_7              0xC0
#define NSCN_8              0xE0

#define OVF_MASK            0x10
#define OVF                 0x10
#define NO_OVF              0x00



// Bit masks in the 050's MARKERS_EN register.

#define APP_MASK            0x80
#define APP_ON              0x80
#define APP_OFF             0x00

#define COM_MASK            0x40
#define COM_ON              0x40
#define COM_OFF             0x00

#define DRI_MASK            0x20
#define DRI_ON              0x20
#define DRI_OFF             0x00

#define DQT_MASK            0x10
#define DQT_ON              0x10
#define DQT_OFF             0x00

#define DHT_MASK            0x08
#define DHT_ON              0x08
#define DHT_OFF             0x00

#define DNL_MASK            0x04
#define DNL_ON              0x04
#define DNL_OFF             0x00

#define DQTI_MASK           0x02
#define DQTI_ON             0x02
#define DQTI_OFF            0x00

#define DHTI_MASK           0x01
#define DHTI_ON             0x01
#define DHTI_OFF            0x00



// Bit masks in 050's STATUS_0 and INT_REQ_0 registers.
//
// Note: Bit mapping in this register is identical to MARKERS_EN register
//       except the last two bits. Therefore the same masks are used.

#define RST_MASK            DRI_MASK   // two functions for this bit
#define SOF_MASK            0x02
#define SOS_MASK            0x02



// Bit masks in 050's STATUS_1 and INT_REQ_1 registers.

#define DATRDY_MASK         0x80
#define MRKDET_MASK         0x40
#define RFM_MASK            0x10
#define RFD_MASK            0x08
#define END_MASK            0x04
#define TCVOVF_MASK         0x02
#define DATOVF_MASK         0x01



// Sizes of marker segments.

#define QTABLE_SIZE         0x40
#define HUFF_AC_SIZE        0xB2   // 178 !!
#define HUFF_DC_SIZE        0x1C   // 28 !!
#define APP_SIZE            0x3C   // 60 - NET length !!
#define COM_SIZE            0x3C   // 60 - NET length !!



// Pixel components indexes.

#define Y_COMPONENT         0
#define U_COMPONENT         1
#define V_COMPONENT         2





// Assignment of Q and H tables to components. Max. NO_OF_COMPONENTS is 8.

static BYTE QTid[8] = { 0, 1, 1, 0, 0, 0, 0, 0 };

static BYTE SCAN1_HDCid[8] = { 0, 1, 1, 0, 0, 0, 0, 0 };
static BYTE SCAN1_HACid[8] = { 0, 1, 1, 0, 0, 0, 0, 0 };





// Sampling ratios for different video formats.

static BYTE HSampRatio422[8] = { 2,1,1,0,0,0,0,0 };
static BYTE VSampRatio422[8] = { 1,1,1,0,0,0,0,0 };

static BYTE HSampRatio411[8] = { 4,1,1,0,0,0,0,0 };
static BYTE VSampRatio411[8] = { 1,1,1,0,0,0,0,0 };

static BYTE HSampRatio411H2V2[8] = { 2,1,1,0,0,0,0,0 };
static BYTE VSampRatio411H2V2[8] = { 2,1,1,0,0,0,0,0 };





// Default DQT tables in zigzag order.

BYTE DefaultQTables[130] = {
	0x00,                                           // Pq,Tq of first table
	0x10, 0x0b, 0x0c, 0x0e, 0x0c, 0x0a, 0x10, 0x0e,
	0x0d, 0x0e, 0x12, 0x11, 0x10, 0x13, 0x18, 0x28,
	0x1a, 0x18, 0x16, 0x16, 0x18, 0x31, 0x23, 0x25,
	0x1d, 0x28, 0x3a, 0x33, 0x3d, 0x3c, 0x39, 0x33,
	0x38, 0x37, 0x40, 0x48, 0x5c, 0x4e, 0x40, 0x44,
	0x57, 0x45, 0x37, 0x38, 0x50, 0x6d, 0x51, 0x57,
	0x5f, 0x62, 0x67, 0x68, 0x67, 0x3e, 0x4d, 0x71,
	0x79, 0x70, 0x64, 0x78, 0x5c, 0x65, 0x67, 0x63,
	0x01,                                           // Pq,Tq of second table
	0x11, 0x12, 0x12, 0x18, 0x15, 0x18, 0x2f, 0x1a,
	0x1a, 0x2f, 0x63, 0x42, 0x38, 0x42, 0x63, 0x63,
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63
	};



// Default DHT segment.

BYTE DefaultHuffTables[0x1A0] = {
	// JPEG DHT Segment for YCrCb omitted from MJPG data.
	0x00,
	// DC0 length specifications
	0x00,0x01,0x05,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	// DC0 symbol definitions
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,

	0x01,
	// DC1 length specifications
	0x00,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
	// DC1 symbol definitions
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,

	0x10,
	// AC0 length specifications
	0x00,0x02,0x01,0x03,0x03,0x02,0x04,0x03,0x05,0x05,0x04,0x04,0x00,0x00,0x01,0x7D,
	// AC0 symbol definitions
	0x01,0x02,0x03,0x00,0x04,0x11,0x05,0x12,0x21,0x31,0x41,0x06,0x13,0x51,0x61,0x07,
	0x22,0x71,0x14,0x32,0x81,0x91,0xA1,0x08,0x23,0x42,0xB1,0xC1,0x15,0x52,0xD1,0xF0,
	0x24,0x33,0x62,0x72,0x82,0x09,0x0A,0x16,0x17,0x18,0x19,0x1A,0x25,0x26,0x27,0x28,
	0x29,0x2A,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
	0x4A,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
	0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
	0x8A,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
	0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xC2,0xC3,0xC4,0xC5,
	0xC6,0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xE1,0xE2,
	0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,
	0xF9,0xFA,

	0x11,
	// AC1 length specifications
	0x00,0x02,0x01,0x02,0x04,0x04,0x03,0x04,0x07,0x05,0x04,0x04,0x00,0x01,0x02,0x77,
	// AC1 symbol definitions
	0x00,0x01,0x02,0x03,0x11,0x04,0x05,0x21,0x31,0x06,0x12,0x41,0x51,0x07,0x61,0x71,
	0x13,0x22,0x32,0x81,0x08,0x14,0x42,0x91,0xA1,0xB1,0xC1,0x09,0x23,0x33,0x52,0xF0,
	0x15,0x62,0x72,0xD1,0x0A,0x16,0x24,0x34,0xE1,0x25,0xF1,0x17,0x18,0x19,0x1A,0x26,
	0x27,0x28,0x29,0x2A,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,
	0x49,0x4A,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,
	0x69,0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x82,0x83,0x84,0x85,0x86,0x87,
	0x88,0x89,0x8A,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,
	0xA6,0xA7,0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xC2,0xC3,
	0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,
	0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,
	0xF9,0xFA
	};










//************************************************************************
// Physical unit.
//************************************************************************



VirtualUnit *JCoder050::CreateVirtual (void)
	{
	return NULL;   // NOT USED YET
	}





// Check if the 050 coder has ASSERTED the END signal.

BOOL JCoder050::Check050End (void)
	{
	BYTE status;

	port->InByte (STATUS_1, status);
	return (status & END_MASK) != 0;
	}





// Prepare the soft GO.

void JCoder050::PrepareGO (void)
	{
	BYTE tmp;

	// Preset 050 addressing so that logic holds GO register address.
	port->InByte (GO, tmp);
	}





// Create the SOF marker segment for a given compression/expansion configuration
// and return its size.

DWORD JCoder050::MakeSOFMarker (void)
	{
	WORD org = SOF_INDX;
	DWORD mlen = 8 + NO_OF_COMPONENTS * 3 + 2;
	int j;
	VirtualJCoder050 *jpeg = GetCurrent ();
	WORD height;


	port->OutByte (org, 0xff);
	port->OutByte (org+1, SOF0_MARKER);

	port->OutByte (org+2, HIBYTE(mlen-2));
	port->OutByte (org+3, LOBYTE(mlen-2));

	port->OutByte (org+4, BASELINE_PRECISION);   // P [8]

	if (jpeg->fieldOrder != VFLD_SINGLE)
		height = jpeg->height / 2;
	else
		height = jpeg->height;
	port->OutByte (org+5, HIBYTE(height));
	port->OutByte (org+6, LOBYTE(height));

	port->OutByte (org+7, HIBYTE(jpeg->width));
	port->OutByte (org+8, LOBYTE(jpeg->width));

	port->OutByte (org+9, NO_OF_COMPONENTS);   // YUV - 3 components

	for (j = 0;  j < NO_OF_COMPONENTS;  j++)
		{
		port->OutByte (org+10+j*3, j);   // C1 - id 0
		port->OutByte (org+11+j*3, HSampRatio[j] * 16 + VSampRatio[j]);   // H and V
		port->OutByte (org+12+j*3, QTid[j]);   // Qtbl
		}

	return mlen;
	}





// Create the SOS1 marker segment for a given compression/expansion configuration
// and return its size.

DWORD JCoder050::MakeSOS1Marker (void)
	{
	WORD org = SOS1_INDX;
	WORD len = 3 + NO_OF_SCAN1_COMPONENTS * 2 + 3;
	DWORD mlen = len + 2;
	int j;


	port->OutByte (org, 0xff);
	port->OutByte (org+1, SOS_MARKER);

	port->OutByte (org+2, HIBYTE(len));
	port->OutByte (org+3, LOBYTE(len));

	port->OutByte (org+4, NO_OF_SCAN1_COMPONENTS);   // YUV - 3 components

	for (j = 0;  j < NO_OF_SCAN1_COMPONENTS;  j++)
		{
		port->OutByte (org+5+j*2, j);   // C1 - id 0
		port->OutByte (org+6+j*2, SCAN1_HDCid[j] * 16 + SCAN1_HACid[j]);   // AC, DC tables
		}

	port->OutByte (org + len - 1, 0x00);
	port->OutByte (org + len    , 0x3f);
	port->OutByte (org + len + 1, 0x00);

	return mlen;
	}





// Create the DRI marker segment for a given compression/expansion configuration
// and return its size.

DWORD JCoder050::MakeDRIMarker (void)
	{
	WORD org = DRI_INDX;
	VirtualJCoder050 *jpeg = GetCurrent ();


	port->OutByte (org, 0xff);
	port->OutByte (org+1, DRI_MARKER);

	port->OutByte (org+2, 0);
	port->OutByte (org+3, 4);   // fixed length

	port->OutByte (org+4, HIBYTE(jpeg->dri));
	port->OutByte (org+5, LOBYTE(jpeg->dri));

	return 6;   // fixed len
	}





// Create the quantization tables and return their total size.
// Q tables are given as a pointer to an array of 130 chars, which contains
// two contiguous Q tables including the Pq, Tq byte before each table. The
// organization of the Q tables array is expected to be as follows:
//    [0]      - Pq,Tq       Precision and identifier of first table
//    [1:64]   - Qk          64 elements of first table
//    [65]     - Pq,Tq       Precision and identifier of second table
//    [66:129] - Qk          64 elements of second table

DWORD JCoder050::MakeQTable (void)
	{
	WORD org = DQT_INDX;
	WORD len = (QTABLE_SIZE + 1) * 2 + 2;
	DWORD mlen = len + 2;
	int j;
	VirtualJCoder050 *jpeg = GetCurrent ();
	BYTE __far *qTable = jpeg->qTable;


	if (qTable == NULL)
		qTable = DefaultQTables;

	port->OutByte (org, 0xff);
	port->OutByte (org+1, DQT_MARKER);

	port->OutByte (org+2, HIBYTE(len));
	port->OutByte (org+3, LOBYTE(len));

	for (j = 0;  j < (QTABLE_SIZE + 1) * 2;  j++)
		{
		port->OutByte (org+4+j, (BYTE)qTable[j]);
		}

	return mlen;
	};





// Create the huffman tables and return their total size.
// Huff tables are given as a pointer to an array of 420 chars, which is
// expected to contain two AC and two DC tables including the ID byte for each
// table. The organization of each table is expected to be as follows:
//    DC table:
//    [0]      - 0,Th        DC indicator (0) and identifier of
//                           the DC table
//    [1:28]   - DCk         28 elements of DC table
//    AC table:
//    [0]      - 1,Th        AC indicator (1) and identifier of
//                           the AC table
//    [1:178]  - ACk         178 elements of AC table
// The tables can come in any order, DC,DC,AC,AC or AC,DC,AC,DC or whatever.

DWORD JCoder050::MakeHuffTable (void)
	{
	WORD org = DHT_INDX;
	WORD len = (HUFF_AC_SIZE+1 + HUFF_DC_SIZE+1) * 2 + 2;
	DWORD mlen = len + 2;
	int j;
	VirtualJCoder050 *jpeg = GetCurrent ();
	BYTE __far *huffTable = jpeg->huffTable;


	if (huffTable == NULL)
		huffTable = DefaultHuffTables;

	port->OutByte (org, 0xff);
	port->OutByte (org+1, DHT_MARKER);

	port->OutByte (org+2, HIBYTE(len));
	port->OutByte (org+3, LOBYTE(len));

	for (j = 0;  j < (HUFF_AC_SIZE+1 + HUFF_DC_SIZE+1) * 2;  j++)
		{
		port->OutByte (org+4+j, (BYTE)huffTable[j]);
		}

	return mlen;
	};





// Create the APP segment and return its size. The APP string is given as a
// pointer to an array of 64 chars. Only 64 chars are supported now.

DWORD JCoder050::MakeAPPString (void)
	{
	WORD org = APP_INDX;
	int j;
	VirtualJCoder050 *jpeg = GetCurrent ();
	BYTE __far *appString = jpeg->appMarker;


	if (appString == NULL)
		return 0;

	port->OutByte (org, 0xff);
	port->OutByte (org+1, APP_MARKER);

	port->OutByte (org+2, HIBYTE(APP_SIZE+2));
	port->OutByte (org+3, LOBYTE(APP_SIZE+2));

	for (j = 0;  j < APP_SIZE;  j++)
		{
		port->OutByte (org+4+j, (BYTE)appString[j]);
		}

	return APP_SIZE + 4;
	};





// Create the COM segment and return its size. The COM string is given as a
// pointer to an array of 64 chars. Only 64 chars are supported now.

DWORD JCoder050::MakeCOMString (void)
	{
	WORD org = COM_INDX;
	int j;
	VirtualJCoder050 *jpeg = GetCurrent ();
	BYTE __far *comString = jpeg->comString;


	if (comString == NULL)
		return 0;

	port->OutByte (org, 0xff);
	port->OutByte (org+1, COM_MARKER);

	port->OutByte (org+2, HIBYTE(COM_SIZE + 2));
	port->OutByte (org+3, LOBYTE(COM_SIZE + 2));

	for (j = 0;  j < COM_SIZE;  j++)
		{
		port->OutByte (org+4+j, (BYTE)comString[j]);
		}

	return COM_SIZE + 4;
	};





// Create the marker segments for a given configuration of
// compression/expansion.

void JCoder050::MakeMarkerSegments (void)
	{
	SOFMarkerSize  = MakeSOFMarker ();
	SOSMarkerSize  = MakeSOS1Marker ();
	DRIMarkerSize  = MakeDRIMarker ();
	DQTMarkerSize  = MakeQTable ();
	DHTMarkerSize  = MakeHuffTable ();
	APPMarkerSize  = MakeAPPString ();
	COMMarkerSize  = MakeCOMString ();
	DHTIMarkerSize = 0;
	DQTIMarkerSize = 0;
	DNLMarkerSize  = 0;
	};





// Start the coder operation.

void JCoder050::Start (void)
	{
	port->OutByte (GO, 0);
	};





// Configure the coder's internal registers and marker segments to baseline
// JPEG compression, either two pass mode or not. Return success.

BOOL JCoder050::ConfigureCompression (void)
	{
	BYTE tmp;
	WORD wtmp;
	DWORD dtmp;
	DWORD totalMarkers;
	DWORD timeout;
	int i;
	BYTE *hs, *vs;
	VirtualJCoder050 *jpeg = GetCurrent ();


	switch (jpeg->format)
		{
		case VFMT_YUV_411:
			hs = HSampRatio411;
			vs = VSampRatio411;
			break;
		case VFMT_YUV_420:
			hs = HSampRatio411H2V2;
			vs = VSampRatio411H2V2;
			break;
		default:
		case VFMT_YUV_422:
			hs = HSampRatio422;
			vs = VSampRatio422;
			break;
		}
	for (i = 0;  i < 8;  i++)
		{
		HSampRatio[i] = *hs++;
		VSampRatio[i] = *vs++;
		}

	tmp = MSTR | NO_DMA | CFIS_1_CLK | NO_BELE;
	port->OutByte (HARDWARE, tmp);

	tmp = COMP | NO_ATP | NO_PASS2 | TLM | NO_DCONLY | NO_BRC;
	port->OutByte (MODE, tmp);

	tmp = NSCN_1 | NO_OVF;
	port->OutByte (OPTIONS, tmp);

	tmp = (BYTE) (jpeg->maxBlockCodeVolume / 2);
	port->OutByte (MBCV_050, tmp);

	tmp = 0;
	port->OutByte (INT_REQ_0, tmp);   // no interrupts on markers

	tmp = 3;                          // two LSBs must be one
	port->OutByte (INT_REQ_1, tmp);   // no interrupt enabled

	wtmp = jpeg->scaleFactor;
	port->OutByte (SF_HI, HIBYTE(wtmp));
	port->OutByte (SF_LO, LOBYTE(wtmp));

	port->OutByte (AF_HI, 0xff);
	port->OutByte (AF_M,  0xff);
	port->OutByte (AF_LO, 0xff);

	MakeMarkerSegments ();   // sets the MarkerSize variables

	tmp = APP_OFF | COM_OFF | DRI_OFF  | DQT_OFF | DHT_OFF | DNL_OFF | DQTI_OFF | DHTI_ON;
	port->OutByte (MARKERS_EN, tmp);

	Start ();   // start the coder for Huffman table preload

	// Wait for end of table preload.
	timeout = Timer.GetMilliTicks () + TABLE_PRELOAD_TIMEOUT;
	while (! Check050End ())
		{
		if (timeout < Timer.GetMilliTicks ())
			return FALSE;   // timeout
		}

	// Now the 050 is in idle mode like after a reset and waits for GO.

	if (jpeg->twoPassMode)
		tmp = COMP | ATP | NO_PASS2 | NO_TLM | NO_DCONLY | BRC;
	else
		tmp = COMP | NO_ATP | PASS2 | NO_TLM | NO_DCONLY | BRC;
	port->OutByte (MODE, tmp);

	if (jpeg->stillMode)
		tmp = APP_ON | COM_OFF | DRI_OFF | DQT_ON | DHT_ON | DNL_OFF | DQTI_OFF | DHTI_OFF;
	else
		tmp = APP_ON | COM_OFF | DRI_OFF | DQT_ON | DHT_OFF | DNL_OFF | DQTI_OFF | DHTI_OFF;
	port->OutByte (MARKERS_EN, tmp);

	totalMarkers = SOFMarkerSize + SOSMarkerSize + 4;   // SOF, SOS, SOI, EOI always exist
	if (tmp & APP_MASK) totalMarkers += APPMarkerSize;
	if (tmp & COM_MASK) totalMarkers += COMMarkerSize;
	if (tmp & DRI_MASK) totalMarkers += DRIMarkerSize;
	if (tmp & DQT_MASK) totalMarkers += DQTMarkerSize;
	if (tmp & DHT_MASK) totalMarkers += DHTMarkerSize;
	if (tmp & DNL_MASK) totalMarkers += DNLMarkerSize;
	if (tmp & DQTI_MASK) totalMarkers += DQTIMarkerSize;
	if (tmp & DHTI_MASK) totalMarkers += DHTIMarkerSize;

	dtmp = 8 * (jpeg->totalCodeVolume - totalMarkers);   // bit size !
	port->OutByte (TCV_NET_HI, LBYTE3(dtmp));
	port->OutByte (TCV_NET_MH, LBYTE2(dtmp));
	port->OutByte (TCV_NET_ML, LBYTE1(dtmp));
	port->OutByte (TCV_NET_LO, LBYTE0(dtmp));

	dtmp -= (dtmp / 128);        // minus STUFFING
	dtmp -= ((dtmp * 5) / 64);   // minus EOB
	port->OutByte (TCV_DATA_HI, LBYTE3(dtmp));
	port->OutByte (TCV_DATA_MH, LBYTE2(dtmp));
	port->OutByte (TCV_DATA_ML, LBYTE1(dtmp));
	port->OutByte (TCV_DATA_LO, LBYTE0(dtmp));

	// Preset 050 addressing so that logic holds GO register address.
	port->InByte (GO, tmp);

	return TRUE;
	};





// Configure the coder's internal registers and marker segments to BASELINE
// JPEG expansion. Return success.

BOOL JCoder050::ConfigureExpansion (void)
	{
	BYTE tmp;
	DWORD timeout;
	VirtualJCoder050 *jpeg = GetCurrent ();


	tmp = (cfisExpansion << 2) & CFIS_MASK;
	tmp |= NO_BSWD | MSTR | NO_DMA | NO_BELE;
	port->OutByte (HARDWARE, tmp);

	tmp = NO_COMP | TLM;
	port->OutByte (MODE, tmp);   // table preload mode

	tmp = 0;
	port->OutByte (INT_REQ_0, tmp);   // no interrupts on markers

	tmp = 3;                          // two LSBs must be one
	port->OutByte (INT_REQ_1, tmp);   // no interrupt enabled

	MakeHuffTable ();   // only DHT relevant !!

	tmp = DHTI_ON;
	port->OutByte (MARKERS_EN, tmp);

	Start ();   // start the coder for Huffman table preload

	// Wait for end of table preload.
	timeout = Timer.GetMilliTicks () + TABLE_PRELOAD_TIMEOUT;
	while (! Check050End ())
		{
		if (timeout < Timer.GetMilliTicks ())
			return FALSE;   // timeout
		}

	// Now the 050 is in idle mode like after a reset and waits for GO.

	if (jpeg->thumbnail)
		tmp = NO_COMP | DCONLY;
	else
		tmp = NO_COMP | NO_DCONLY;
	port->OutByte (MODE, tmp);   // expansion

	tmp = APP_OFF | COM_OFF;   // don't accept APP, COM
	port->OutByte (MARKERS_EN, tmp);   // into markers segment

	// Preset 050 addressing so that logic holds GO register address.
	port->InByte (GO, tmp);

	return TRUE;
	};





// Return the scale factor used to compress the last frame.

WORD JCoder050::GetLastScaleFactor (void)
	{
	BYTE lo, hi;
	WORD result;

	port->InByte (SF_LO, lo);
	port->InByte (SF_HI, hi);
	result = ((WORD)hi << 8) + (WORD)lo;
	GetCurrent()->scaleFactor = result;
	return result;
	}










//************************************************************************
// Virtual unit.
//************************************************************************



VirtualJCoder050::VirtualJCoder050 (JCoder050 *physical)
	: VirtualJCoderRated (physical)
	{
	jcoder = physical;

	// Set defaults.
	format     = VFMT_YUV_422;
	fieldOrder = VFLD_SINGLE;
	thumbnail  = FALSE;
	qTable     = NULL;
	huffTable  = NULL;
	comString  = NULL;

	SetDefaultAPP ();
	}





VirtualJCoder050::~VirtualJCoder050 (void)
	{
	}





// Default APP is any non-empty string of 64 bytes length including zero byte.

void VirtualJCoder050::SetDefaultAPP (void)
	{
	int i;

	for (i = 0;  i < APP_LENGTH;  i++)
		appMarker[i] = ' ';
	appMarker[APP_LENGTH-1] = '\0';
	}





Error VirtualJCoder050::Configure (TAG __far *tags)
	{
	int len;
	BYTE __far *src;
	BYTE __far *dst;


	GNREASSERT( VirtualJCoderRated::Configure (tags) );

	PARSE_TAGS_START(tags)
		GETSET (JPG_VIDEO_FORMAT, format);
		GETSET (JPG_THUMBNAIL,    thumbnail);
		GETSET (JPG_QTABLE,       qTable);
		GETSET (JPG_HUFFTABLE,    huffTable);

		GETONLY (JPG_VIDEO_FMT_LIST, VideoFormatBitField);

/*		case CSET_JPG_DEF_APPSTRING:
			src = VAL_JPG_DEF_APPSTRING(tp);
			
			len = 0;
			dst = src;
			while (*dst++) len++;
			
			if (len >= APP_LENGTH)
				GNRAISE(GNR_RANGE_VIOLATION);
			
			SetDefaultAPP ();
			dst = appMarker;
			for (;  len > 0;  len--)
				*dst++ = *src++;
			break;
		case CGET_JPG_DEF_APPSTRING:
			REF_JPG_DEF_APPSTRING(tp) = appMarker;
			break;
		case CQRY_JPG_DEF_APPSTRING:
			QRY_TAG(tp) = TRUE;
			break;
*/
	PARSE_TAGS_END

	GNRAISE_OK;
	}





Error VirtualJCoder050::InitOperation (void)
	{
	BYTE __far *src;
	BYTE __far *dst;
	int i;


	if (compress)
		{
		// Compression.
		twoPassMode = FALSE;
		SetDefaultAPP ();
#if 1
		// Automatically select the field order.
		switch (fieldOrder)
			{
			default:
			case VFLD_SINGLE:	src = JCoder::DefaultAPPStringSingle;	break;
			case VFLD_ODD:		src = JCoder::DefaultAPPStringOdd;		break;
			case VFLD_EVEN:	src = JCoder::DefaultAPPStringEven;	break;
			}
		dst = appMarker;
		while ((*dst++ = *src++) != 0) ;
#endif

		// Estimate a starting value for the scale factor. The formula is
		// sf = (256.0 * ( 1.770625 + x * ( - 0.09240625 + x * (0.001625 - 0.0000090725 * x) ) ) );
//		float x = (float)totalCodeVolume / 1000.0;
//		usedScaleFactor = (long)(453.28 + x * ( -23.656 + x * ( 0.416 - 0.00232256 * x )));
		if (! stillMode)
			{
			long x = (long)totalCodeVolume / 1000L;
			if (x < 10)
				scaleFactor = 1 * 256;
			else if (x > 90)
				scaleFactor = 2;   // 0.01 * 256
			else
				scaleFactor = (WORD)( 453L + ScaleLong (x, 100000L, (-2365600L + x * (41600L - 232L * x))) );
			}

		i = 0;
		while (! jcoder->ConfigureCompression ())
			{
			if (i++ >= 2)
				GNRAISE(GNR_RANGE_VIOLATION);
			}
		}
	else
		{
		// Expansion.
		i = 0;
		while (! jcoder->ConfigureExpansion ())
			{
			if (i++ >= 2)
				GNRAISE(GNR_RANGE_VIOLATION);
			}
		}
	GNRAISE_OK;
	}





void VirtualJCoder050::Stop (void)
	{
	// Actually, you cannot stop the 050 via register. We rely on the JPEG
	// manager class to stop us by resetting via a hardware reset line.
	}
