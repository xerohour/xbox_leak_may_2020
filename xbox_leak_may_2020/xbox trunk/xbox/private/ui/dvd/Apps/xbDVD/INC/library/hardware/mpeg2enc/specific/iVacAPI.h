//********************* FILE DESCRIPTION *******************************
//
//  THIS SDK INCLUDING CODE, BINARIES, AND INFORMATION IS PROVIDED "AS IS" WITHOUT 
//  WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED 
//  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
// FILE NAME:  DOAPI.H
//
// COPYRIGHT:		    Company Confidential
//		          Copyright (c) iCompression, Inc. 1997,1998,1999
//			             All Rights Reserved
//
// VERSION: Pre-release
//
// FUNCTION: FW Maibox interface prototypes and defines
//
// DEPENDENCIES: NONE
// RESTRICTIONS: NONE
//
// EXTERNAL REFERENCES:  none.
//
// MODIFICATION HISTORY:
// DATE			DEVELOPER		CHANGE DESCRIPTION
// 01/30/98		Deb Chatterjee	Original creation
// 02/10/98		A. Mobini		Translation to 'C' and separation of OS dependencies
// 10/21/99		Martin Stephan, VIONA Development GmbH, several changes to adapt to our software architecture
//**********************************************************************
#ifndef _DOAPI_H
#define _DOAPI_H

#include <fwid.h>

#ifndef _WDM_
#ifndef _EXTERN
#define _EXTERN
#endif
#endif // _WDM_

#define MAX_PARAMS 8
#define MAX_QUEUED_MAILBOX_COMMANDS 10
#define API_CALL_SUCCESS 0
#define API_CALL_FAILURE 0xFFFFFFFF
#define MAILBOX_ADDRESS_INSIDE_FW 0xED00
#define MAILBOX_SIZE_IN_DWORDS 12
#define MAILBOX_SIZE_IN_BYTES 48
#define MAILBOX_FLAG_MAILBOX_FULL 1
#define MAILBOX_FLAG_PARAMETERS_SET 2
#define MAILBOX_FLAG_MAILBOX_READY 3
#define MAILBOX_FLAG_MAILBOX_DONE 7

typedef char CHAR;
typedef unsigned char UCHAR;
typedef unsigned char BYTE;
typedef BYTE * PBYTE;
typedef short SHORT;
typedef long LONG;
typedef unsigned short WORD;
typedef unsigned long DWORD;
#define VOID void
#define HANDLE VOID *	
#define open _open
#define read _read
#define filelength _filelength
#define close _close

#pragma pack(1)

enum eReturnCodes {
	RC_SUCCESS							= 0x00000000,	/* Successful completion */
	RC_ERROR_INVALIDPARAMETER			= 0x88000000,	/* One or more of the formal parameters are out of range */
	RC_ERROR_UNKOWN						= 0x88000001,	/* Unknown error */
	RC_ERROR_ROM_DOWNLOAD_FILEOPEN		= 0x88000010,	/* Error openning the rom file */
	RC_ERROR_ROM_DOWNLOAD_DATATXFER		= 0x88000020,	/* Error transferring the rom image to the device */
	RC_ERROR_DEVICE_UNINITIALIZED		= 0x88000100,	/* Error initializing the device */
	RC_ERROR_DEVICE_MEMWRITE			= 0x88000200,	/* Error writing to iVAC memory */
	RC_ERROR_DEVICE_MEMREAD				= 0x88000400,	/* Error reading from iVAC memory */
};

typedef enum
{
	WDM_DRIVER						= 0x00,
	VXD_DRIVER						= 0x01
} DRIVER_TYPE;

typedef enum
{
	PYTHON							= 0x00,
	PYTHON_DS						= 0x01,
	PYTHON_2						= 0x02
} BOARD_TYPE;

typedef enum
{
	PHILIPS_7111					= 0x00,
	PHILIPS_7114					= 0x01,
	MICRONAS						= 0x02
} DIGITIZER_TYPE;

typedef enum
{
	BOARD_6MEG						= 0x00,
	BOARD_12MEG						= 0x01
} BOARD_MEMORY;

typedef enum
{
	SUPPORTED_AUDIOSAMPLINGRATE_32		= 1,
	SUPPORTED_AUDIOSAMPLINGRATE_44		= 2,
	SUPPORTED_AUDIOSAMPLINGRATE_48		= 4
} SUPPORTED_AUDIO_FREQUENCIES;

typedef enum
{
	VIDEO_INPUT_656						= 0,
	VIDEO_INPUT_601						= 1,
} VIDEO_FORMAT;

typedef struct
{
	BOARD_TYPE		eBoardType;				// Python, Python DS, Python 2
	DIGITIZER_TYPE	eDigitizer;				// 7111, 7114, Micronas
	BOARD_MEMORY	eBoardMemory;			// 6 Meg, 12 Meg
	BYTE			bSupportedAudioFreq;	// Bitmask indicating which of 32, 44.1 and, 
											// 48 KHz frequencies are supported. the value 
											// is a OR between any of the values in 
											// SUPPORTED_AUDIO_FREQUENCIES
} BOARD_DESCRIPTION;

typedef enum
{
	MEMORY_SIZE						= 0x01,
	AUDIO_DIGITIZER					= 0x0E,
	VIDEO_INPUT_FORMAT				= 0x10,
	VIDEO_DIGITIZER					= 0xE0
} SUBVENDOR_ID_MASK;

typedef enum
{
	MEMORY_6						= 0x00,
	MEMORY_12						= 0x01,
	AUDIO_DIG_MIC_32				= 0x00,
	AUDIO_DIG_MIC_48				= 0x02,
	AUDIO_DIG_CRYSTAL				= 0x04,
	VIDEO_INPUT_FORMAT_656			= 0x00,
	VIDEO_INPUT_FORMAT_601			= 0x10,
	VIDEO_DIG_7111					= 0x00,
	VIDEO_DIG_7114					= 0x20,
	VIDEO_DIG_MICRONAS				= 0xE0
} SUBDEVICE_ID_VALUES;

typedef enum
{
	MPEG_COMPLIANT					= 0x01,
	NON_MPEG_COMPLIANT				= 0x00
} MPEG_COMPLIANCE;

typedef enum 
{
	PROP_STREAMOUTPUT_ELEMENTARY	= 0x00,
	PROP_STREAMOUTPUT_PROGRAM		= 0x01,
	PROP_STREAMOUTPUT_TRANSPORT		= 0x02,
	PROP_STREAMOUTPUT_PES			= 0x03,	//Not supported
	PROP_STREAMOUTPUT_MPEG1			= 0x04
} OUTPUT_TYPE;

typedef enum 
{
	PROP_VIDEOINPUT_YC					= 0x00,
	PROP_VIDEOINPUT_COMPOSITE			= 0x01
} VIDEO_INPUT_TYPE;

typedef enum
{
	PROP_VIDEORESOLUTION_720x480		= 0,
	PROP_VIDEORESOLUTION_576x576		= 0, // For PAL
	PROP_VIDEORESOLUTION_480x480		= 1,
	PROP_VIDEORESOLUTION_480x576		= 1, // For PAL
	PROP_VIDEORESOLUTION_352x480		= 2,
	PROP_VIDEORESOLUTION_352x576		= 2, // For PAL
	PROP_VIDEORESOLUTION_352x240		= 2  // For MPEG1
} VIDEO_RESOLUTION;

typedef enum
{
	PROP_TVFORMAT_NTSC		= 0,
	PROP_TVFORMAT_PAL		= 1
} TV_ENCODE_FORMAT;

typedef enum
{
	PROP_ASPECTRATIO_4X3			= 0x00,
	PROP_ASPECTRATIO_PANSCAN		= 0x01,
	PROP_ASPECTRATIO_16X9			= 0X02
} ASPECT_RATIO;

typedef enum
{
	PROP_MPEGAUDIOLAYER_I			= 0x03,
	PROP_MPEGAUDIOLAYER_II			= 0x02,
	PROP_MPEGAUDIOLAYER_III			= 0x01,
} MPEG_AUDIO_LAYER;

typedef enum
{
	PROP_AUDIOSAMPLINGRATE_32		= 0x02,
	PROP_AUDIOSAMPLINGRATE_44		= 0x00,
	PROP_AUDIOSAMPLINGRATE_48		= 0x01,
	PROP_AUDIOSAMPLINGRATE_UNSUPPORTED = 0x10
} SAMPLING_RATE;

typedef enum
{	
	PROP_AUDIOOUTPUTMODE_MONO		= 0x03,
	PROP_AUDIOOUTPUTMODE_STEREO		= 0x00,
	PROP_AUDIOOUTPUTMODE_DUAL		= 0x02,
	PROP_AUDIOOUTPUTMODE_JOINT		= 0x01
} AUDIO_OUTPUT_MODE;

typedef enum
{
	PROP_AUDIOCRC_OFF				= 0x00,
	PROP_AUDIOCRC_ON				= 0x01
} CRC_STATE;

typedef enum
{
	PROP_VIDEOGOPSTRUCT_IBP			= 0x00,
	PROP_VIDEOGOPSTRUCT_IP			= 0x01,
	PROP_VIDEOGOPSTRUCT_I			= 0x02
} GOP_STRUCTURE;

typedef enum
{
	PROP_AUDIOENCODINGMODE_MPEG				= 0x00,
	PROP_AUDIOENCODINGMODE_DOLBY_DIGITAL	= 0x01,
	PROP_AUDIOENCODINGMODE_LPCM				= 0x02
} AUDIO_ENCODING_MODE;

typedef struct
{
	DWORD		dwTime;
	DWORD		dwVideoFrames;
} CAPTURE_STATUS;

typedef enum
{
	RC_VIDEOENCODINGMODE_VBR	= 0x00,
	RC_VIDEOENCODINGMODE_CBR	= 0x01
}  BITRATE_MODE;

typedef struct
{
	BITRATE_MODE	bEncodingMode;	// Variable or Constant bit rate
	WORD			wBitrate;		// Actual bitrate in 1/400 mbits/sec
	BYTE			bVariance;		// 1-100 as a percentage of variance
} VIDEO_BITRATE;

typedef enum
{
	DATARATE_LAYERII_64			= 0x04,
	DATARATE_LAYERII_96			= 0x06,
	DATARATE_LAYERII_112		= 0x07,
	DATARATE_LAYERII_128		= 0x08,
	DATARATE_LAYERII_160		= 0x09,
	DATARATE_LAYERII_192		= 0x0A,
	DATARATE_LAYERII_224		= 0x0B,
	DATARATE_LAYERII_256		= 0x0C,
	DATARATE_LAYERII_320		= 0x0D,
	DATARATE_LAYERII_384		= 0x0E
} AUDIO_DATARATE_LAYER_II;

#ifdef MINIDRIVER
typedef enum 
{
	STATE_STOPPED   = 0x00,
    STATE_PAUSED    = 0x01,
    STATE_RUNNING   = 0x02
}  RUN_STATE;
#endif // #ifdef MINIDRIVER

//
// definition of the full HW device extension structure This is the structure
// that will be allocated in HW_INITIALIZATION by the stream class driver
// Any information that is used in processing a device request (as opposed to
// a STREAM based request) should be in this structure.  A pointer to this
// structure will be passed in all requests to the minidriver. (See
// HW_STREAM_REQUEST_BLOCK in STRMINI.H)
//


typedef struct {
    DWORD mailBoxFlag; /* Bit 0 = 0 -> empty, 1 -> full */
                     /* Bit 1 = 0 -> input parameters have been set,
                              = 1 -> input parameters are not valid */
                     /* Bit 2 = 1 - > if the call has been processed,
                              = 0 - >otherwise */
    DWORD commandCode;  /* Each API function will have a command code */
    DWORD returnCode;   /* Return code = 0 for success, -1 for failure */
    DWORD timeToLive;   /* Explained later */
    DWORD parameterBuffer[MAX_PARAMS];
                      /* Same buffer will be shared between input and
                         output */
} FW_API_MAILBOX;

typedef enum eApiCommandCode
{
	ApiCommand_NoOperation = 0, 
	ApiCommand_StartCapture = 1, 
	ApiCommand_StopCapture = 2, 
	ApiCommand_SetAudioStreamID = 3,
	ApiCommand_GetAudioStreamID = 4,
	ApiCommand_SetVideoStreamID = 5,
	ApiCommand_GetVideoStreamID = 6,
	ApiCommand_SetPrivateStreamID = 7,
	ApiCommand_GetPrivateStreamID = 8,
	ApiCommand_SetAudioPID = 9,
	ApiCommand_GetAudioPID = 10,
	ApiCommand_SetVideoPID = 11,
	ApiCommand_GetVideoPID = 12,
	ApiCommand_SetAuxPID = 13,
	ApiCommand_GetAuxPID = 14,
	ApiCommand_SetVideoResolution = 15,
	ApiCommand_GetVideoResolution = 16,
	ApiCommand_SetVideoBitRate = 17,
	ApiCommand_GetVideoBitRate = 18,
	ApiCommand_SetGOPSize = 19,
	ApiCommand_GetGOPSize = 20,
	ApiCommand_SetPredictionDistance = 21,
	ApiCommand_GetPredictionDistance = 22,
	ApiCommand_SetTVEncodingFormat = 23,
	ApiCommand_GetTVEncodingFormat = 24,
	ApiCommand_SetAspectRatio = 25,
	ApiCommand_GetAspectRatio = 26,
	ApiCommand_SetStreamOutputType = 27,
	ApiCommand_GetStreamOutputType = 28,
	ApiCommand_SetMPEGAudioLayer = 29,
	ApiCommand_GetMPEGAudioLayer = 30,
	ApiCommand_SetAudioSamplingRate = 31,
	ApiCommand_GetAudioSamplingRate = 32,
	ApiCommand_SetAudioDataRate = 33,
	ApiCommand_GetAudioDataRate = 34,
	ApiCommand_SetAudioOutputMode = 35,
	ApiCommand_GetAudioOutputMode = 36,
	ApiCommand_SetAudioCRCState = 37,
	ApiCommand_GetAudioCRCState = 38,
	ApiCommand_ReadIOPort = 39,
	ApiCommand_WriteIOPort = 40,
	ApiCommand_SetupDMATxfer = 41,
	ApiCommand_DownloadROMFile = 42, 
	ApiCommand_SetCroppingFormat = 43,
	ApiCommand_GetCroppingFormat = 44,
	ApiCommand_SetVideoInputType = 45,
	ApiCommand_GetVideoInputType = 46,
	ApiCommand_SetGOPStructure = 47,
	ApiCommand_GetGOPStructure = 48,
	ApiCommand_SetVideoDigitizer = 55,
	ApiCommand_GetVideoDigitizer = 56,
	ApiCommand_SetVideoMemoryMode = 57,
	ApiCommand_GetVideoMemoryMode = 58,
	ApiCommand_SetVideoD1Or601 = 59,
	ApiCommand_GetVideoD1Or601 = 60,
	ApiCommand_SetVideoPSCompliance = 61,
	ApiCommand_GetVideoPSCompliance = 62
} API_COMMAND_CODE;

#pragma pack()
	
#ifdef __cplusplus
extern "C"
{
#endif

_EXTERN FW_IDENTIFICATION_STRUCTURE g_FWId;
_EXTERN DWORD g_dwAPIStatus;
_EXTERN DWORD g_dwMailBoxAddress;
_EXTERN FW_API_MAILBOX g_fwApiMailBox;

_EXTERN VOID InitializeMailBox(VOID *pHwDevExt, int paramA);
#ifdef _WIN32_WCE
DWORD DownloadROMFile(VOID *pHwDevExt, LPCTSTR kszFileName, BYTE bOutputStreamType);
#else
_EXTERN DWORD DownloadROMFile(VOID *pHwDevExt, BYTE *Filename, BYTE bOutputStreamType);
#endif
_EXTERN DWORD NoOp(VOID *pHwDevExt);
_EXTERN DWORD StartCapture(VOID *pHwDevExt);	
_EXTERN DWORD StopCapture(VOID *pHwDevExt);	
_EXTERN DWORD SetAudioStreamID(VOID *pHwDevExt, WORD wStreamNumber, WORD wStreamID );	
_EXTERN DWORD GetAudioStreamID(VOID *pHwDevExt, WORD wStreamNumber, WORD *pbStreamID);
_EXTERN DWORD SetVideoStreamID(VOID *pHwDevExt, WORD wStreamNumber, WORD wStreamID);
_EXTERN DWORD GetVideoStreamID(VOID *pHwDevExt, WORD wStreamNumber, WORD *pwStreamID);
_EXTERN DWORD SetPrivateStreamID(VOID *pHwDevExt, WORD wStreamNumber, WORD wStreamID);
_EXTERN DWORD GetPrivateStreamID(VOID *pHwDevExt, WORD wStreamNumber, WORD *pwStreamID);
_EXTERN DWORD SetAudioPID(VOID *pHwDevExt, WORD wStreamNumber, WORD wPID);
_EXTERN DWORD GetAudioPID(VOID *pHwDevExt, WORD wStreamNumber, WORD *pwPID);
_EXTERN DWORD SetVideoPID(VOID *pHwDevExt, WORD wStreamNumber, WORD wPID);
_EXTERN DWORD GetVideoPID(VOID *pHwDevExt, WORD wStreamNumber, WORD *pwPID);
_EXTERN DWORD SetAuxPID(VOID *pHwDevExt, WORD wStreamNumber, WORD wPID);
_EXTERN DWORD GetAuxPID(VOID *pHwDevExt, WORD wStreamNumber, WORD *pwPID);
_EXTERN DWORD SetVideoResolution(VOID *pHwDevExt, BYTE bResolutionIndex);
_EXTERN DWORD GetVideoResolution(VOID *pHwDevExt, BYTE *pbResolutionIndex);
_EXTERN DWORD SetVideoBitRate(VOID *pHwDevExt, BYTE bEncodingMode, WORD wVideoBitrate, BYTE bVariance);
_EXTERN DWORD GetVideoBitRate(VOID *pHwDevExt, BYTE *pbEncodingMode, WORD *pwVideoBitrate, BYTE *pbVariance);
_EXTERN DWORD SetGOPSize(VOID *pHwDevExt, BYTE bGOPSize);
_EXTERN DWORD GetGOPSize(VOID *pHwDevExt, BYTE *pbGOPSize);
_EXTERN DWORD SetPredictionDistance(VOID *pHwDevExt, WORD wDistance);
_EXTERN DWORD GetPredictionDistance(VOID *pHwDevExt, WORD *pwDistance);
_EXTERN DWORD SetTVEncodingFormat(VOID *pHwDevExt, BYTE bTVFormat);
_EXTERN DWORD GetTVEncodingFormat(VOID *pHwDevExt, BYTE *pbTVFormat);
_EXTERN DWORD SetCroppingFormat(VOID *pHwDevExt, BYTE bCrop);
_EXTERN DWORD GetCroppingFormat(VOID *pHwDevExt, BYTE *pbCrop);
_EXTERN DWORD SetAspectRatio(VOID *pHwDevExt, BYTE bAspectRatio);
_EXTERN DWORD GetAspectRatio(VOID *pHwDevExt, BYTE *pbAspectRatio);
_EXTERN DWORD SetStreamOutputType(VOID *pHwDevExt, BYTE bOutputType);
_EXTERN DWORD GetStreamOutputType(VOID *pHwDevExt, BYTE *pbOutputType);
_EXTERN DWORD SetMPEGAudioLayer(VOID *pHwDevExt, BYTE bMPEGAudLayer);
_EXTERN DWORD GetMPEGAudioLayer(VOID *pHwDevExt, BYTE *pbMPEGAudLayer);
_EXTERN DWORD SetAudioSamplingRate(VOID *pHwDevExt, BYTE bSamplingRate);
_EXTERN DWORD GetAudioSamplingRate(VOID *pHwDevExt, BYTE *pbSamplingRate);
_EXTERN DWORD SetAudioDataRate(VOID *pHwDevExt, BYTE bDataRate);	
_EXTERN DWORD GetAudioDataRate(VOID *pHwDevExt, BYTE *pbDataRate);
_EXTERN DWORD SetAudioOutputMode(VOID *pHwDevExt, BYTE bOutputMode);
_EXTERN DWORD GetAudioOutputMode(VOID *pHwDevExt, BYTE *pbOutputMode);
_EXTERN DWORD SetAudioCRCState(VOID *pHwDevExt, BYTE bCRCState);
_EXTERN DWORD GetAudioCRCState(VOID *pHwDevExt, BYTE *pbCRCState);
_EXTERN DWORD ReadIOPort(VOID *pHwDevExt, BYTE bPortID, DWORD dwReg, DWORD *pdwValue);
_EXTERN DWORD WriteIOPort(VOID *pHwDevExt, BYTE bPortID, DWORD dwReg, DWORD dwValue);
_EXTERN DWORD SetupDMATxfer(VOID *pHwDevExt, DWORD dwSrc, DWORD dwDst, DWORD dwSize);
_EXTERN DWORD PostGenericRequest(VOID *pHwDevExt, FW_API_MAILBOX *data);
_EXTERN DWORD SetVideoInputType(VOID *pHwDevExt, BYTE bVideoInput);
_EXTERN DWORD GetVideoInputType(VOID *pHwDevExt, BYTE * pbVideoInput);
_EXTERN DWORD SetGOPStructure(VOID *pHwDevExt, BYTE eGOPStructure);
_EXTERN DWORD GetGOPStructure(VOID *pHwDevExt, BYTE * eGOPStructure);
_EXTERN DWORD SetVideoDigitizer(VOID *pHwDevExt, BYTE bVideoDigitizer);
_EXTERN DWORD GetVideoDigitizer(VOID *pHwDevExt, BYTE *pbVideoDigitizer);
_EXTERN DWORD SetVideoMemoryMode(VOID *pHwDevExt, BYTE bMemoryMode);
_EXTERN DWORD GetVideoMemoryMode(VOID *pHwDevExt, BYTE *pbMemoryMode);
_EXTERN DWORD SetVideoInputFormat(VOID *pHwDevExt, BYTE bVideoInputFormat);
_EXTERN DWORD GetVideoInputFormat(VOID *pHwDevExt, BYTE *pbVideoInputFormat);
_EXTERN DWORD SetVideoMpegCompliance(VOID *pHwDevExt, BYTE bMpegCompliance);
_EXTERN DWORD GetVideoMpegCompliance(VOID *pHwDevExt, BYTE *pbMpegCompliance);

//PRIVATE FUNCTIONS
_EXTERN DWORD ExecuteApiRequest(VOID *pHwDevExt, FW_API_MAILBOX *pfwApiMailBox);
_EXTERN DWORD GetMailBoxAddress(VOID *pHwDevExt);
//TEMPORARY API - FOR FILTER USE ONLY
//GetVideoResolution should be used
_EXTERN DWORD GetActualResolution(VOID *pHwDevExt, WORD *pwWidth, WORD *pwHeight);

#ifdef __cplusplus
}
#endif

#endif //_DOAPI_H