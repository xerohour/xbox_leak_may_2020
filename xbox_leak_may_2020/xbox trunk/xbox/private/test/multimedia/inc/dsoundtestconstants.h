#pragma once

#include <xtl.h>

enum PLAY_MODE { PLAY_MODE_WAIT = 0, PLAY_MODE_POLL = 1, PLAY_MODE_BUFFERED = 2, PLAY_MODE_STREAM = 3 };

static const ULONG MIN_FILE_NUM					= 1;
static const ULONG MAX_FILE_NUM					= 2846; 
static const ULONG MAX_FILE_NAME_LEN			= 10;
static const ULONG BITS_PER_BYTE				= 8;
static const ULONG MILLISEC_PER_SEC				= 1000;
static const ULONG NUM_DEFAULT_BUFFERS			= 4;
static const ULONG INCREMENTAL_AMOUNT			= 3000;
static const ULONG NUM_THREADS                  = 10;
static const ULONG BUFFER_ALLOC_PAD             = 5;
static const DWORD PLAY_TO_COMPLETION			= 0xFFFFFFFF;
static const DWORD PLAY_POLLING					= 0xFFFFFFFE;
static const float CREATE_COMPLETE_BUFFER		= -1.0f;
static const float DEFAULT_BUFFER_DURATION		= 4.0f;

static const LPCSTR MEDIA_DIR					= "t:\\media\\audio\\pcm";
static const LPCSTR WMA_DRIVE                   = "t:\\media\\audio\\wma";
static const LPCSTR DVD_DRIVE                   = "t:\\media\\audio\\pcm";
static const LPCSTR WMA_FILES                   = "t:\\media\\audio\\wma\\*.wma";
static const LPCSTR DVD_FILES                   = "t:\\media\\audio\\pcm\\*.wav";
static const LPCSTR VERY_LARGE_FILE			    = "t:\\media\\audio\\pcm\\2844.wav";
static const LPCSTR SIMPLE_FILE				    = "t:\\media\\audio\\pcm\\2592.wav";
static const LPCSTR BVT_FILE                    = "t:\\media\\audio\\pcm\\2845.wav";
static const LPCSTR WMA_BVT_FILE                = "t:\\media\\audio\\wma\\test.wma";
static const LPCSTR FILE_FORMAT_STRING			= "%s\\%u.wav";

static const LPCSTR BVT_FILES[] = { 
	"t:\\media\\audio\\pcm\\2723.wav",  // 48000, 16, 2												
	"t:\\media\\audio\\pcm\\2615.wav",  // 48000, 16, 1												
	"t:\\media\\audio\\pcm\\2603.wav",  // 48000,  8, 2
	"t:\\media\\audio\\pcm\\2674.wav",  // 48000,  8, 1 
	"t:\\media\\audio\\pcm\\167.wav",   // 44100, 16, 2 
	"t:\\media\\audio\\pcm\\677.wav",   // 44100, 16, 1
	"t:\\media\\audio\\pcm\\682.wav",   // 44100,  8, 2
	"t:\\media\\audio\\pcm\\681.wav",   // 44100,  8, 1
	"t:\\media\\audio\\pcm\\1947.wav",  // 32000, 16, 2
	"t:\\media\\audio\\pcm\\2301.wav",  // 32000, 16, 1
	"t:\\media\\audio\\pcm\\1892.wav",  // 32000,  8, 2
	"t:\\media\\audio\\pcm\\1891.wav",  // 32000,  8, 1
	"t:\\media\\audio\\pcm\\1596.wav",  // 22050, 16, 2
	"t:\\media\\audio\\pcm\\1210.wav",  // 22050, 16, 1
	"t:\\media\\audio\\pcm\\1226.wav",  // 22050,  8, 2
	"t:\\media\\audio\\pcm\\5.wav",     // 22050,  8, 1
	"t:\\media\\audio\\pcm\\664.wav",   // 16000, 16, 2
	"t:\\media\\audio\\pcm\\663.wav",   // 16000, 16, 1
	"t:\\media\\audio\\pcm\\666.wav",   // 16000,  8, 2
	"t:\\media\\audio\\pcm\\665.wav",   // 16000,  8, 1
	"t:\\media\\audio\\pcm\\991.wav",   // 11025, 16, 2
	"t:\\media\\audio\\pcm\\823.wav",   // 11025, 16, 1
	"t:\\media\\audio\\pcm\\831.wav",   // 11025,  8, 2
	"t:\\media\\audio\\pcm\\816.wav",   // 11025,  8, 1
	"t:\\media\\audio\\pcm\\2770.wav",  //  8000, 16, 2
	"t:\\media\\audio\\pcm\\2768.wav",  //  8000, 16, 1
	"t:\\media\\audio\\pcm\\2772.wav",  //  8000,  8, 2
	"t:\\media\\audio\\pcm\\2761.wav"   //  8000,  8, 1
};

static const LPCSTR SIM_FILES[] = { 
	"t:\\media\\audio\\pcm\\1939.wav",
	"t:\\media\\audio\\pcm\\2066.wav",
	"t:\\media\\audio\\pcm\\1240.wav",
	"t:\\media\\audio\\pcm\\755.wav",
	"t:\\media\\audio\\pcm\\190.wav",
	"t:\\media\\audio\\pcm\\2513.wav",
	"t:\\media\\audio\\pcm\\1252.wav",
	"t:\\media\\audio\\pcm\\1308.wav" 
};

static const LPCSTR THREAD_FILES[] = {
	"t:\\media\\audio\\pcm\\1448.wav",
	"t:\\media\\audio\\pcm\\1720.wav",
	"t:\\media\\audio\\pcm\\1958.wav",
	"t:\\media\\audio\\pcm\\1357.wav",
	"t:\\media\\audio\\pcm\\2057.wav",
	"t:\\media\\audio\\pcm\\77.wav",
	"t:\\media\\audio\\pcm\\696.wav",
	"t:\\media\\audio\\pcm\\1413.wav",
	"t:\\media\\audio\\pcm\\2633.wav",
	"t:\\media\\audio\\pcm\\179.wav"
};

static const LPCSTR WMA_THREAD_FILES[] = {
	"t:\\media\\audio\\wma\\fz14_44khz_10secs_WMA2_160k48khzS.wma",
	"t:\\media\\audio\\wma\\dmb3_WMA2_22k22khzS.wma",
	"t:\\media\\audio\\wma\\acos_WMA2_32k44khzM.wma",
	"t:\\media\\audio\\wma\\horn23_2_WMA2_192k44khzS.wma",
	"t:\\media\\audio\\wma\\spme50_1_WMA2_10k11khzM.wma",
	"t:\\media\\audio\\wma\\tough2_44s_WMA2_40k32khzS.wma",
	"t:\\media\\audio\\wma\\trpt21_2_WMA2_48k32khzS.wma",
	"t:\\media\\audio\\wma\\dire_WMA2_48k32khzS.wma",
	"t:\\media\\audio\\wma\\bass47_1_WMA2_80k44khzS.wma",
	"t:\\media\\audio\\wma\\gspi35_1_WMA2_8k8khzM.wma"
};



static const LPCSTR TONE_FILES[] = { 
	"t:\\media\\audio\\pcm\\t1.wav",
    "t:\\media\\audio\\pcm\\t2.wav",
	"t:\\media\\audio\\pcm\\t3.wav",
	"t:\\media\\audio\\pcm\\t4.wav",
	"t:\\media\\audio\\pcm\\t5.wav",
	"t:\\media\\audio\\pcm\\t6.wav",
	"t:\\media\\audio\\pcm\\t7.wav",
	"t:\\media\\audio\\pcm\\t8.wav",
	"t:\\media\\audio\\pcm\\t9.wav",
	"t:\\media\\audio\\pcm\\t10.wav",
	"t:\\media\\audio\\pcm\\t11.wav",
	"t:\\media\\audio\\pcm\\t12.wav",
	"t:\\media\\audio\\pcm\\t13.wav",
	"t:\\media\\audio\\pcm\\t14.wav",
	"t:\\media\\audio\\pcm\\t15.wav",
	"t:\\media\\audio\\pcm\\t16.wav",
	"t:\\media\\audio\\pcm\\t17.wav",
	"t:\\media\\audio\\pcm\\t18.wav",
	"t:\\media\\audio\\pcm\\t19.wav",
	"t:\\media\\audio\\pcm\\t20.wav",
	"t:\\media\\audio\\pcm\\t21.wav",
	"t:\\media\\audio\\pcm\\t22.wav",
	"t:\\media\\audio\\pcm\\t23.wav",
	"t:\\media\\audio\\pcm\\t24.wav",
	"t:\\media\\audio\\pcm\\t25.wav",
	"t:\\media\\audio\\pcm\\t26.wav",
	"t:\\media\\audio\\pcm\\t27.wav",
	"t:\\media\\audio\\pcm\\t28.wav" 
};

static const DWORD g_dwDeviceIds[] = { 0, 1, 2, 0xFFFFFFFE, 0xFFFFFFFF };
static const LPUNKNOWN g_pUnks[] = { NULL, (LPUNKNOWN) 0xDEADBEEF };
static const DWORD g_dwBufferFlags[] = { 
	0, 
//	DSBCAPS_LOCHARDWARE, 
//	DSBCAPS_LOCSOFTWARE, 
	DSBCAPS_CTRL3D, 
	DSBCAPS_CTRLFREQUENCY, 
	DSBCAPS_CTRLVOLUME, 
	DSBCAPS_CTRLPOSITIONNOTIFY, 
//	DSBCAPS_CTRLCHANNELVOLUME, 
	DSBCAPS_LOCDEFER,
	0xFFFFFFFE,
	0xFFFFFFFF,
};

static const DWORD g_dwBufferSizes[] = {
	0,
	1,                // Boundry
	512,              // 2^9
	1024,             // 2^10
	2048,             // 2^11
	4096,             // 2^12
    8000,             // 8k
	8192,             // 2^13
	11025,            // 11k
	16000,            // 16k
	16384,            // 2^14
	22050,            // 22k
	32000,            // 32k
	32768,            // 2^15
	44100,            // 44k
	48000,            // 48k
	64000,            // 64k
	65536,            // 2^16
	88200,            // 88k
	96000,            // 96k
	128000,           // 128k
	131072,           // 2^17
	160000,           // 160k
	176400,           // 176k
	192000,           // 192k
	262144,           // 2^18
	0xFFFFFFFF        // Boundry
};

static WAVEFORMATEX g_waveFormats[] = {
	// NEED TO ADD MORE WAVEFORMATEX's
	// Need to add NULL case
	{ 0,               0, 0,      0,      0, 0,  0 },
	{ WAVE_FORMAT_PCM, 2, 48000,  192000, 4, 16, 0 }
};

#ifndef SILVER

static const DWORD g_dwSpeakerConfigs[] = {
	0,
    DSSPEAKER_COMBINED(DSSPEAKER_SURROUND, 0),
    DSSPEAKER_COMBINED(DSSPEAKER_SURROUND, DSSPEAKER_ENABLE_AC3),
    DSSPEAKER_COMBINED(DSSPEAKER_SURROUND, DSSPEAKER_ENABLE_DTS),
    DSSPEAKER_COMBINED(DSSPEAKER_SURROUND, DSSPEAKER_ENABLE_AC3 | DSSPEAKER_ENABLE_DTS),
    DSSPEAKER_COMBINED(DSSPEAKER_STEREO, 0),
    DSSPEAKER_COMBINED(DSSPEAKER_STEREO, DSSPEAKER_ENABLE_AC3),
    DSSPEAKER_COMBINED(DSSPEAKER_STEREO, DSSPEAKER_ENABLE_DTS),
    DSSPEAKER_COMBINED(DSSPEAKER_STEREO, DSSPEAKER_ENABLE_AC3 | DSSPEAKER_ENABLE_DTS),
    DSSPEAKER_COMBINED(DSSPEAKER_MONO, 0),
    DSSPEAKER_COMBINED(DSSPEAKER_MONO, DSSPEAKER_ENABLE_AC3),
    DSSPEAKER_COMBINED(DSSPEAKER_MONO, DSSPEAKER_ENABLE_DTS),
    DSSPEAKER_COMBINED(DSSPEAKER_MONO, DSSPEAKER_ENABLE_AC3 | DSSPEAKER_ENABLE_DTS),
	0xFFFFFFFE,
	0xFFFFFFFF
};

#endif //SILVER

/*
static DSCAPS g_dsCaps[] = {
	// NEED TO ADD MORE CAPS CASES
	// NEED TO ADD NULL CASE
	{ 0                   , 0, 0, 0, 0, 0, 0, 0 },
	{ sizeof( DSCAPS ) - 1, 0, 0, 0, 0, 0, 0, 0 },
	{ sizeof( DSCAPS )    , 0, 0, 0, 0, 0, 0, 0 },
	{ sizeof( DSCAPS ) + 1, 0, 0, 0, 0, 0, 0, 0 },
	{ 0xFFFFFFFF          , 0, 0, 0, 0, 0, 0, 0 }
};
*/
static const DWORD g_dwApplies[] = { DS3D_IMMEDIATE, DS3D_DEFERRED, 2, 0xFFFFFFFF };
/*
static const D3DXVECTOR3 g_ZeroVec;

static DS3DLISTENER g_ds3dListeners[] = {
	// NEED TO ADD MORE LISTNENERS
	// NEED TO ADD NULL CASE
	{ 0                          , g_ZeroVec, g_ZeroVec, g_ZeroVec, g_ZeroVec, 0, 0, 0 },
	{ sizeof ( DS3DLISTENER ) - 1, g_ZeroVec, g_ZeroVec, g_ZeroVec, g_ZeroVec, 0, 0, 0 },
	{ sizeof ( DS3DLISTENER )    , g_ZeroVec, g_ZeroVec, g_ZeroVec, g_ZeroVec, 0, 0, 0 },
	{ sizeof ( DS3DLISTENER ) + 1, g_ZeroVec, g_ZeroVec, g_ZeroVec, g_ZeroVec, 0, 0, 0 },
	{ 0xFFFFFFFF                 , g_ZeroVec, g_ZeroVec, g_ZeroVec, g_ZeroVec, 0, 0, 0 }
};

static DS3DBUFFER g_ds3dBuffers[] = {
	// NEED TO ADD MORE BUFFERS
	// NEED TO ADD NULL CASE
	{ 0                       , g_ZeroVec, g_ZeroVec, 0, 0, g_ZeroVec, 0, 0, 0, 0 },
	{ sizeof( DS3DBUFFER ) - 1, g_ZeroVec, g_ZeroVec, 0, 0, g_ZeroVec, 0, 0, 0, 0 },
	{ sizeof( DS3DBUFFER )    , g_ZeroVec, g_ZeroVec, 0, 0, g_ZeroVec, 0, 0, 0, 0 },
	{ sizeof( DS3DBUFFER ) + 1, g_ZeroVec, g_ZeroVec, 0, 0, g_ZeroVec, 0, 0, 0, 0 },
	{ 0xFFFFFFFF              , g_ZeroVec, g_ZeroVec, 0, 0, g_ZeroVec, 0, 0, 0, 0 }
};
*/
static FLOAT g_d3dVals[] = { 
	0.00000f, 
	0.00001f, 
	0.10000f, 
	0.90000f, 
	0.99999f, 
	1.00000f, 
	1.00001f,
	1.10000f,
	9.99999f,
	10.00000f,
	10.00001f,
	99.99999f,
	100.00000f,
	100.00001f,
	999.99999f,
	1000.00000f,
	1000.00001f,
	-0.00001f, 
	-0.10000f, 
	-0.90000f, 
	-0.99999f, 
	-1.00000f, 
	-1.00001f,
	-1.10000f,
	-9.99999f,
	-10.00000f,
	-10.00001f,
	-99.99999f,
	-100.00000f,
	-100.00001f,
	-999.99999f,
	-1000.00000f,
	-1000.00001f
};

#ifdef SILVER

static DSCHANNELVOLUME g_dsChannelVolumes[] = {
	// NEED TO ADD MORE CHANNEL VOLUME
	// NEED TO ADD NULL TEST
	{ 0, 0, 0, 0, 0, 0, 0 }
};

#endif //SILVER

static const LONG g_lVolumes[] = {
	DSBVOLUME_MIN - 1,
	DSBVOLUME_MIN,
	DSBVOLUME_MIN + 1,
	-5001,
	-5000,
	-4999,
	DSBVOLUME_MAX - 1,
	DSBVOLUME_MAX,
	DSBVOLUME_MAX + 1,
	4999,
	5000,
	5001,
	-DSBVOLUME_MIN - 1,
	-DSBVOLUME_MIN,
	-DSBVOLUME_MIN + 1,
};

static const DWORD g_dwFrequencies[] = {
	DSBFREQUENCY_ORIGINAL,
	DSBFREQUENCY_MIN - 1,
	DSBFREQUENCY_MIN,
	DSBFREQUENCY_MIN + 1,
	49999,
	50000,
	50001,
	DSBFREQUENCY_MAX - 1,
	DSBFREQUENCY_MAX,
	DSBFREQUENCY_MAX + 1,
	0xFFFFFFFF
};

static const DWORD g_dwModes[] = {
	DS3DMODE_NORMAL,
	DS3DMODE_HEADRELATIVE,    
	DS3DMODE_DISABLE,
	DS3DMODE_DISABLE + 1,
	0xFFFFFFFF
};

static const DWORD g_dwNotifyCounts[] = {
	0,
	1,
	2,
	10,
	100,
	1000,
	0xFFFFFFFF
};

static DSBPOSITIONNOTIFY g_pNotifies[] = {
	// NEED MORE TEST CASES
	// NEED NULL CASE
	{ 0, 0 },
	{ 0xFFFFFFFF, 0 }
};

static const DWORD g_dwPlayCursors[] = {
	0,
	1,
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	0xFFFFFFFF
};

static const LPVOID g_pvBuffers[] = {
	(LPVOID) 0,
	(LPVOID) 1,
	(LPVOID) 10,
	(LPVOID) 100,
	(LPVOID) 1000,
	(LPVOID) 100000,
	(LPVOID) 1000000,
	(LPVOID) 10000000,
	(LPVOID) 100000000,
	(LPVOID) 0xDEADBEEF,
	(LPVOID) 0xFFFFFFFF
};

static const DWORD g_dwConeAngles[] = {
	0,
	1,
	15,
	30,
	45,
	60,
	75,
	90,
	105,
	120,
	135,
	150,
	165,
	180,
	270,
	359,
	360,
	361,
	1000,
	10000,
	100000,
	0xFFFFFFFF
};

static const DWORD g_dwReserves[] = {
	0,
	1,
	0xDEADBEEF,
	0xFFFFFFFF
};

static const DWORD g_dwFlags[] = {
	0,
	DSBPLAY_LOOPING,
	DSBPLAY_LOOPING + 1,
	DSBSTOPEX_ENVELOPE,
	DSBSTOPEX_ENVELOPE + 1,
	0xFFFFFFFF
};

static const DWORD g_dwWriteCursors[] = {
	0,
	1,
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	0xFFFFFFFF
};

static const DWORD g_dwLockFlags[] = {
	0,
	DSBLOCK_FROMWRITECURSOR,
	DSBLOCK_ENTIREBUFFER,
	DSBLOCK_ENTIREBUFFER + 1,
	0xFFFFFFFF
};

static const REFERENCE_TIME g_rtTimeStamps[] = {
	0,
	1,
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	0xFFFFFFFF
};






