#ifndef _WMC_TYPES_H_
#define _WMC_TYPES_H_

/*********  Basic type Definitions  *************/

#ifndef WMC_TYPE_DEFINED
#define WMC_TYPE_DEFINED

typedef void Void_WMC;
typedef long I32_WMC;
typedef unsigned long U32_WMC;
typedef short I16_WMC;
typedef unsigned short U16_WMC;

#if defined(_Embedded_x86)
typedef unsigned short U16Char_WMC;
#elif defined(macintosh)
typedef wchar_t U16Char_WMC;
#else
typedef unsigned short U16Char_WMC;
#endif

typedef char I8_WMC;
typedef unsigned char U8_WMC;
typedef long Bool_WMC;
typedef double Double_WMC;
typedef float Float_WMC;
typedef struct tQWORD_WMC
{
    U32_WMC   dwLo;
    U32_WMC   dwHi;

}   QWORD_WMC;

#ifdef macintosh
typedef unsigned long long QWORD;
#endif

#ifdef _XBOX
typedef unsigned __int64    U64_WMC;
typedef __int64    I64_WMC;
#elif defined(macintosh) || defined(_Embedded_x86)
typedef unsigned long long  U64_WMC;
typedef long long  I64_WMC;
#elif defined(HITACHI)
typedef struct tU64_WMC
{
    U32_WMC   dwLo;
    U32_WMC   dwHi;
}   U64_WMC;
typedef struct tI64_WMC
{
    I32_WMC   dwLo;
    I32_WMC   dwHi;
}   I64_WMC;
#else
typedef unsigned __int64    U64_WMC;
typedef __int64    I64_WMC;
#endif

typedef struct tRect_WMC
{
    I32_WMC iLeft;
    I32_WMC iTop;
    I32_WMC iRight;
    I32_WMC iBottom;
}   Rect_WMC;

#define TRUE_WMC    1
#define FALSE_WMC   0
#define NULL_WMC   0
#define MIN_WANTED 512
typedef void * HWMCFILE;
typedef enum tagMediaType_WMC
{
    Audio_WMC, 
    Video_WMC, 
    Binary_WMC
} tMediaType_WMC;



typedef struct tagStreamIdnMediaType_WMC
{
    U16_WMC wStreamId;
    tMediaType_WMC MediaType;
} tStreamIdnMediaType_WMC;


typedef enum tagOutputType_WMC
{
    Discard_WMC,
    Compressed_WMC, 
    Decompressed_WMC
} tOutputType_WMC;
/************************************************************************/
#ifndef MAKEFOURCC_WMC
#define MAKEFOURCC_WMC(ch0, ch1, ch2, ch3) \
        ((U32_WMC)(U8_WMC)(ch0) | ((U32_WMC)(U8_WMC)(ch1) << 8) |   \
        ((U32_WMC)(U8_WMC)(ch2) << 16) | ((U32_WMC)(U8_WMC)(ch3) << 24 ))

#define mmioFOURCC_WMC(ch0, ch1, ch2, ch3)  MAKEFOURCC_WMC(ch0, ch1, ch2, ch3)
#endif




/******* video output type guids, in preference order  *****/
#define FOURCC_WMV3     mmioFOURCC_WMC('W','M','V','3')
#define FOURCC_WMV2     mmioFOURCC_WMC('W','M','V','2')
#define FOURCC_WMV1     mmioFOURCC_WMC('W','M','V','1')
#define FOURCC_M4S2     mmioFOURCC_WMC('M','4','S','2')
#define FOURCC_MP43     mmioFOURCC_WMC('M','P','4','3')
#define FOURCC_mp43     mmioFOURCC_WMC('m','m','4','3')
#define FOURCC_MP4S     mmioFOURCC_WMC('M','P','4','S')
#define FOURCC_mp4s     mmioFOURCC_WMC('m','p','4','s')
#define FOURCC_MP42     mmioFOURCC_WMC('M','P','4','2')
#define FOURCC_mp42     mmioFOURCC_WMC('m','m','4','2')
#define FOURCC_MSS1     mmioFOURCC_WMC('M','S','S','1')
#define FOURCC_MSS2     mmioFOURCC_WMC('M','S','S','2')
#define FOURCC_WMS2     mmioFOURCC_WMC('W','M','S','2')
#define FOURCC_WMS1     mmioFOURCC_WMC('W','M','S','1')
#define FOURCC_MPG4     mmioFOURCC_WMC('M','P','G','4')
#define FOURCC_MSS1     mmioFOURCC_WMC('M','S','S','1')
#define FOURCC_MSS2     mmioFOURCC_WMC('M','S','S','2')

/***********************************************************/


/******* video intput type guids, in preference order  *****/

#define FOURCC_I420		        0x30323449
#define FOURCC_IYUV		        0x56555949
#define FOURCC_YV12		        0x32315659
#define FOURCC_YUY2		        0x32595559
#define FOURCC_UYVY		        0x59565955
#define FOURCC_YVYU		        0x55595659
#define FOURCC_YVU9		        0x39555659
#define FOURCC_BI_RGB   	    0x00000000
#define FOURCC_BI_BITFIELDS	    0x00000003
#define FOURCC_BI_RLE8   	    0x00000001
#define FOURCC_BI_RLE4   	    0x00000002


////////////////////////////////////////////

#define WAVE_FORMAT_PCM_WMC     0x0001
#define WAVE_FORMAT_WMA1_WMC    0x0160
#define WAVE_FORMAT_WMA8_WMC    0x0161
#define WAVE_FORMAT_WMA9_WMC    0x0162
#define WAVE_FORMAT_WMSP9_WMC    0x000A

///////////////////////////////////////////

typedef struct tagStreamIdPattern_WMC
{
    U16_WMC wStreamId;
    tOutputType_WMC tPattern;
} tStreamIdPattern_WMC;

typedef struct tagPlannedOutputId_WMC
{
    U16_WMC wStreamIndex;
    tMediaType_WMC tMediaType;
	Bool_WMC	bDone;
} tPlannedOutputId_WMC;

typedef struct tagPlannedOutput_WMC
{
    U16_WMC wTotalOutput;
    tPlannedOutputId_WMC tPlannedId[127];
} tPlannedOutputWMC;




typedef enum tagVideoFormat_WMC
{
    // uncompressed
    YUY2_WMV, 
    UYVY_WMV, 
    YVYU_WMV, 
    RGB24_WMV, 
    RGB555_WMV, 
    RGB565_WMV, 
    RGB32_WMV, 
    RGB8_WMV, 
    IYUV_WMV, 
    I420_WMV, 
    YVU9_WMV, 

    // compressed
    WMV2_WMV, 
    WMV1_WMV, 

	// Ignore Audio Only
	IGNORE_VIDEO
	/*
    WMS2_WMV, 
    WMS1_WMV, 
    M4S2_WMV, 
    MP4S_WMV, 
    MP43_WMV, 
    MP42_WMV, 
    MPG4_WMV
*/
} tVideoFormat_WMC;

typedef enum tagWMCDecodeDispRotateDegree
{
    WMC_DispRotate0 = 0,
    WMC_DispRotate90,
    WMC_DispFlip,
    WMC_DispRotate270
} tWMCDecodeDispRotateDegree;



typedef enum tagAudioType_WMC
{
    // uncompressed
    PCM_WMA, 
    WMA3_WMA, 
    WMA2_WMA, 
    WMA1_WMA
} tAudioType_WMC;


typedef struct strAudioStreamInfo_WMC
{
    U16_WMC         u16FormatTag;
    U32_WMC         u32BitsPerSecond;
    U16_WMC         u16BitsPerSample;
    U16_WMC         u16SamplesPerSecond;
    U16_WMC         u16NumChannels;
    U32_WMC         u32BlockSize;
    U16_WMC         u16ValidBitsPerSample; // bits of precision 
    U32_WMC         u32ChannelMask;       // which channels are present in stream 
    U16_WMC         u16StreamId;

} strAudioInfo_WMC;

typedef struct strVideoStreamInfo_WMC
{
    U32_WMC           u32Compression; 
    U32_WMC           u32BitsPerSecond;
    Float_WMC         fltFramesPerSecond; //is always 0
    I32_WMC           i32Width;
    I32_WMC           i32Height;
    U8_WMC            bPalette[3*256]; // Palette
    U16_WMC           u16StreamId;
    U16_WMC           u16VideoOutputBitCount;
    U32_WMC           u32VideoOutputPixelType;
} strVideoInfo_WMC;

typedef struct strBinaryStreamInfo_WMC
{
    Bool_WMC         bNothing;
    U16_WMC          u16StreamId;
} strBinaryInfo_WMC;


typedef struct strHeaderInfo_WMC
{
    U32_WMC     u32PlayDuration;
    U32_WMC     u32Preroll;
    U32_WMC     u32SendDuration;
    U32_WMC     u32MaxBitrate;
    U8_WMC      u8HasDRM;
	U32_WMC		u32PacketSize;
} strHeaderInfo_WMC;


typedef struct tagWMCContentDescription
{
    /* *_len: as [in], they specify how large the corresponding
     *        buffers below are.
     *        as [out], they specify how large the returned
     *        buffers actually are.
     */

    U16_WMC uiTitle_len;
    U16_WMC uiAuthor_len;
    U16_WMC uiCopyright_len;
    U16_WMC uiDescription_len;   /* rarely used */
    U16_WMC uiRating_len;        /* rarely used */

    /* these are two-byte strings
     *   for ASCII-type character set, to convert these
     *   into a single-byte characters, simply skip
     *   over every other bytes.
     */

    U16Char_WMC *pchTitle;
    U16Char_WMC *pchAuthor;
    U16Char_WMC *pchCopyright;
    U16Char_WMC *pchDescription;
    U16Char_WMC *pchRating;

} WMCContentDescription;

/******************************************************************
**   Extended content description types:
**   ECD_STRING -- wchar string
**   ECD_BINARY -- binary (byte) data
**   ECD_BOOL   -- BOOL (int) data
**   ECD_WORD  -- contains one word,
**   ECD_DWORD  -- contains one dword,
**   ECD_QWORD  -- contains one qword,
*******************************************************************/

enum {
    ECD_STRING = 0,
    ECD_BINARY = 1,
    ECD_BOOL = 2,
    ECD_DWORD = 3,
    ECD_QWORD = 4,
    ECD_WORD = 5
};

typedef struct _ECD_DESCRIPTOR {
    U16_WMC         cbName;
    U16Char_WMC     *pwszName;
    U16_WMC         data_type;
    U16_WMC         cbValue;
    union {
        U16Char_WMC *pwszString;
        U8_WMC *pbBinary;
        Bool_WMC *pfBool;
        U32_WMC *pdwDword;
        U64_WMC *pqwQword;
        U16_WMC  *pwWord;
    } uValue;
} ECD_DESCRIPTOR;

typedef struct tagWMCExtendedContentDescription
{
    U16_WMC cDescriptors;             // number of descriptors
    ECD_DESCRIPTOR *pDescriptors;  // pointer to all the descriptors
} WMCExtendedContentDesc;


typedef struct _WMCCommandEntry {
    U32_WMC         time;
    U16_WMC         type;
    U16Char_WMC     *param;
} WMCCommandEntry;

typedef struct tagWMCScriptCommand
{
    U16_WMC num_commands;      // number of script commands
    U16_WMC num_types;         // number of types of commands
    U16Char_WMC **type_names;     // command names, URL, FILENAME, CAPTION, etc
    I32_WMC *type_name_len;     // length of the command name
    I32_WMC *command_param_len; // length of command parameters
    WMCCommandEntry *commands; // pointer to all the commands
} WMCScriptCommand;


typedef struct tagWMCMarkerEntry {
    U64_WMC     m_qOffset;
    U64_WMC     m_qtime;
    U16_WMC     m_wEntryLen;
    U32_WMC     m_dwSendTime;
    U32_WMC     m_dwFlags;
    U32_WMC     m_dwDescLen;
    U16Char_WMC *m_pwDescName;
} WMCMarkerEntry;

typedef struct tagWMCIndexEntries
{
	U32_WMC	dwPacket;
	U16_WMC	wSpan;
} WMCINDEXENTRIES;


typedef struct tagWMCIndexInfo
{
	U16_WMC	nStreamId;
    U32_WMC   time_deltaMs;
    U32_WMC   max_packets;
    U32_WMC   num_entries;
    WMCINDEXENTRIES *pIndexEntries;
} WMCINDEXINFO;


typedef struct tagWMCCodecEntry {
    tMediaType_WMC     m_wCodecType;
    U16_WMC     m_wCodecNameLength;
    U16Char_WMC * m_pwCodecName;
    U16_WMC     m_wCodecDescLength;
    U16Char_WMC * m_pwCodecDescription;
    U16_WMC     m_wCodecInfoLen;
    U8_WMC		*m_pbCodecInfo;
} WMCCodecEntry;

typedef struct tagWMCMetaDataDescRecords {
    U16_WMC     wLangIdIndex;
    U16_WMC     wStreamNumber;
    U16_WMC     wNameLenth;
    U16_WMC     wDataType;
    U32_WMC     wDataLength;
    U16Char_WMC *pwName;
	Void_WMC	*pData;
} WMCMetaDataDescRecords;


typedef struct tagWMCMetaDataEntry {
    U16_WMC     m_wDescRecordsCount;
	WMCMetaDataDescRecords *pDescRec;
} WMCMetaDataEntry;

typedef struct tagBITMAPINFOHEADER_WMC{
        unsigned long      biSize;
        long               biWidth;
        long               biHeight;
        unsigned short     biPlanes;
        unsigned short     biBitCount;
        unsigned long      biCompression;
        unsigned long      biSizeImage;
        long               biXPelsPerMeter;
        long               biYPelsPerMeter;
        unsigned long      biClrUsed;
        unsigned long      biClrImportant;
} BITMAPINFOHEADER_WMC;

#endif
#endif
