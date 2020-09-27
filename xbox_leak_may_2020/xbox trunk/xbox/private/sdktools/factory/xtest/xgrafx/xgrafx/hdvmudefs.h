// Header for structs used for HDVMU communications.

#pragma warning (disable:4200)//Zero-sized array in structs

//BYTE align the data.
#pragma pack(1)

#ifndef _HDVMU_DEFINES_H_
#define _HDVMU_DEFINES_H_

/* Type definitions */
typedef unsigned char	DVMU_BYTE;   // Must be unsigned 8 bit 
typedef unsigned short	DVMU_WORD;   // Must be unsigned 16 bit 
typedef unsigned long	DVMU_DWORD;  // Must be unsigned 32 bit 
typedef short			DVMU_SHORT;  // Must be signed 16 bit 
typedef long			DVMU_LONG;   // Must be signed 32 bit 
typedef BOOL			DVMU_BOOL;

/* Defines */
#define DVMU_SUCCESS 1
#define DVMU_FAILED	0

#define DIGITAL 0
#define ANALOG 1
// SELECT MONITOR SOURCE DEFINES
#define DVI_DIGITAL 0
#define DVI_ANALOG 1
#define VGA 2
#define AUTOMATIC 3

//HDTV mode and Standard Defines
#define HDTV_MODE_RGB					0x0
#define HDTV_MODE_YPRPB					0x1
#define HDTV_MODE_NONINVERT_TRILEVEL	0x2
#define HDTV_MODE_INVERTED_TRILEVEL		0x3

#define HDTV_STD_1080					0x0
#define HDTV_STD_720					0x1
#define HDTV_STD_480					0x2

// SET_SYNC_POLARITY_DEFINES

#define HSYNC_POS_VSYNC_POS  0
#define HSYNC_POS_VSYNC_NEG  1
#define HSYNC_NEG_VSYNC_POS  2
#define HSYNC_NEG_VSYNC_NEG  3

#define HSYNC_NEG 1
#define VSYNC_NEG 2

// SET_2_BITS_CLOCK defines
#define ONE_BIT_PER_CLOCK  0
#define TWO_BITS_PER_CLOCK 0x80

#define MAX_EDID_BUFFER_SIZE 256
// defines the DVMU function codes

// USB standard requests
#define GET_DESCRIPTOR		0x06
#define SET_ADDRESS			0x05
#define SET_CONFIGURATION	0x09

// Vendor Specific Reqeusts
#define UPDATE_FIRMWARE					0x20
#define UPDATE_FPGA						0x21
#define CONNECT_DVI						0x22
#define DISCONNECT_DVI					0x23
#define SET_EDID						0x24
#define SELECT_MONITOR_SOURCE			0x25
#define SET_SYNC_THRESHOLD				0x26
#define SET_COLOR_THRESHOLD				0x27
#define GET_FIRMWARE_REVISION			0x28
#define GET_HARDWARE_REVISION			0x29
#define GET_EDID						0x2A
#define GET_DIGITAL_PIXEL_CLOCK			0x2B
#define GET_TIMINGS						0x2C
#define GET_ANALOG_PIXEL_COUNTS			0x2D
#define GET_DIGITAL_CRC					0x2E
#define GET_ANALOG_COLORS				0x2F
#define SETUP_FOR_ANALOG_DATA			0x2F
#define SEND_ANALOG_COLOR_DATA			0x30
#define GET_UPDATE_STATUS				0x31
#define SET_SYNC_POLARITY				0x32
#define SET_2_BITS_PER_CLOCK			0x33
#define REINITIALIZE_FPGAS				0x34
#define SELECT_HDTV_MODE_AND_STANDARD	0x35


struct HDVMU_SETUP_INFO
{
	int  hSyncPolarity;
	int  vSyncPolarity;
	int  Interlaced;
	WORD wPixelThreshold;
	WORD wHSyncThreshold;
	WORD wVSyncThreshold;
	WORD wStartLine;
	WORD wNumLines;
	WORD wNumSamples;
	BYTE hDelay;
	BYTE Mode;
};


struct GET_TIMINGS_CMD
{
	BYTE function;
	BYTE source;
};

struct TIMINGS_DATA  // for use in calling DLL timings function
{
	BYTE vFrontPorch;
	BYTE vBackPorch;
	WORD vSyncLow;
	WORD vSyncHigh;
	WORD hBackPorch;
	WORD hFrontPorch;
	WORD hSyncLow;
	WORD hSyncHigh;
};

struct GET_TIMINGS_DATA  // for use in calling driver function
{
	BYTE status;
	struct TIMINGS_DATA TimingData1;
	struct TIMINGS_DATA TimingData2;
};

struct GET_DIGITAL_PIXEL_CLOCK_DATA
{
	BYTE status;
	DWORD count;
};

struct GET_ANALOG_PIXEL_COUNT_DATA {
	BYTE status;
	DWORD redPixels[2];
	DWORD greenPixels[2];
	DWORD bluePixels[2];
};

struct SET_EDID_CMD
{
	BYTE function;
	BYTE target;
	WORD BufSize;
	BYTE EDIDInfo[];
};

struct GET_EDID_CMD
{
	BYTE function;
	BYTE target;
};

struct GET_EDID_DATA
{
	BYTE status;
	BYTE EDIDInfo[];
};

struct GET_CRC_DATA
{
	BYTE status;
	DWORD crc;
};

struct REVISION_DATA
{
	BYTE status;
	char revision[8];
};

struct SYNC_THRESHOLD_CMD
{
	BYTE function;
	WORD vertical;
	WORD horizontal;
};

struct COLOR_THRESHOLD_CMD
{
	BYTE function;
	WORD upper;
	WORD lower;
};

struct STATUS_DATA
{
	BYTE status;
	WORD UpdateStatus;
};

struct SYNC_POLARITY_CMD
{
	BYTE function;
	BYTE Polarity;
};

struct TWO_BITS_PER_CLK_CMD
{
	BYTE function;
	BYTE TwoBitsPerClock;
};

struct MONITOR_SRC_CMD
{
	BYTE function;
	BYTE Source;
};

struct HDTV_MODE_N_STD_CMD
{
	BYTE function;
	BYTE ModeStd;
};

struct UPDATE_FIRMWARE_CMD
{
	BYTE function;
	WORD CheckSum;  // checksum of code file
	WORD CodeLen;	// total length of code
	WORD PacketLen; // length of code packet (usually 64 bytes, remainder at end could be less)
	char Packet[64];
};

struct COLOR_SAMPLE
{
	WORD red;	// the buffer this points to should be numlines big
	WORD green; // the buffer this points to should be numlines numlines
	WORD blue; // the buffer this points to should be numlines big
		
};

struct COLOR_SAMPLE_CMD
{
	BYTE function;
	BOOL interlaced;
	BYTE hdelay; 	  // Delay from the horizontal sync in clock counts. A value of 0 means don't sample
	WORD StartLine;  // what line of the frame the sample starts on
	WORD numberOflines;  // how many lines after verticalDelay to measure
	WORD numberOfSamples; // the number of times to measure each pixel. The samples are averaged
};

struct COLOR_SAMPLE_DATA
{
	BYTE status;
	struct COLOR_SAMPLE ColorData[];
};

#pragma pack()

#endif// _HDVMU_DEFINES_H_