#include "globals.h"
#include "help_testcases.h"
#include "help_definitions.h"
#include "tests.h"

static REFERENCE_TIME rt0           = 0;
static REFERENCE_TIME rt1           = 1;
static REFERENCE_TIME rt10000       = 10000;
static REFERENCE_TIME rt7770000     = 7770000;
static REFERENCE_TIME rt1000000     = 1000000;
static REFERENCE_TIME rt10000000    = 10000000;
static REFERENCE_TIME rt100000000    = 100000000;
static REFERENCE_TIME rt599990000   = 599990000;
static REFERENCE_TIME rt999990000   = 999990000;
static REFERENCE_TIME rt599999990000 = 599999990000;
static REFERENCE_TIME rt0x7FFFFFFFFFFFFFFF = 0x7FFFFFFFFFFFFFFF;
static REFERENCE_TIME rt208         = 208;
static REFERENCE_TIME rt833         = 833;
static REFERENCE_TIME rt1041        = 1041;

const TESTCASE g_TestCases[] =
{
/******************************************************************************************
BVTTEST CASES
******************************************************************************************/

    //AudioPath Tests
    //------------------
    {"AudioPath_Activate_BVT"               ,"BVT", AudioPath_Activate_BVT,    0, 0},
    {"AudioPath_GetObjectInPath_BVT"   ,     "BVT", AudioPath_GetObjectInPath_BVT, 0, 0},
    {"AudioPath_SetVolume_BVT",              "BVT", AudioPath_SetVolume_BVT, 0, 0},
    {"AudioPath_SetPitch_BVT",               "BVT", AudioPath_SetPitch_BVT, 0, 0},

    //Loader Tests
    //------------
    {"Loader_ClearCache_BVT"            ,     "BVT", Loader_ClearCache_BVT, 0, 0},
    {"Loader_LoadObjectFromFile_BVT"    ,     "BVT", Loader_LoadObjectFromFile_BVT , 0, 0},
    {"Loader_GetObject_BVT"             ,     "BVT", Loader_GetObject_BVT , 0, 0},
    {"Loader_SetObject_BVT"             ,     "BVT", Loader_SetObject_BVT, 0, 0},
    {"Loader_SetSearchDirectory_BVT"    ,     "BVT", Loader_SetSearchDirectory_BVT , 0, 0},
    {"Loader_ReleaseObjectByUnknown_BVT",     "BVT", Loader_ReleaseObjectByUnknown_BVT, 0, 0},

    //Performance8 Tests
    //------------------
    {"Performance8_AddNotificationType_BVT",  "BVT" , Performance8_AddNotificationType_BVT, 0, 0},           //PASS
    {"Performance8_AllocPMsg_BVT",            "BVT" , Performance8_AllocPMsg_BVT, 0, 0},                     //PASS
    {"Performance8_ClonePMsg_BVT",            "BVT" , Performance8_ClonePMsg_BVT, 0, 0},                     //PASS
    {"Performance8_CreateStandardAudioPath_BVT","BVT",Performance8_CreateStandardAudioPath_BVT, 0, 0},      //PASS
    {"Performance8_FreePMsg_BVT",             "BVT" , Performance8_FreePMsg_BVT, 0, 0},                      //PASS 
    {"Performance8_GetGlobalParam_BVT",       "BVT" , Performance8_GetGlobalParam_BVT, 0, 0},                //PASS
    {"Performance8_GetNotificationPMsg_BVT",  "BVT" , Performance8_GetNotificationPMsg_BVT, 0, 0},           //PASS   
    {"Performance8_GetSegmentState_BVT",      "BVT" , Performance8_GetSegmentState_BVT, 0, 0},               //PASS
    {"Performance8_GetSetDefaultAudioPath_BVT","BVT",Performance8_GetSetDefaultAudioPath_BVT, 0, 0},        //PASS

    {"Performance8_GetTime_BVT_ReferenceTime", "BVT" , Performance8_GetTime_BVT_ReferenceTime, 5, 0},                       //PASS
    {"Performance8_GetTime_BVT_MusicTime",     "BVT" , Performance8_GetTime_BVT_MusicTime,     5, 0},                       //PASS


    {"Performance8_InitAudio_BVT",            "BVT" , Performance8_InitAudio_BVT, 0, 0},                     //PASS
    {"Performance8_IsPlaying_BVT",            "BVT" , Performance8_IsPlaying_BVT, 0, 0},                     //PASS
    {"Performance8_MusicToReferenceTime_BVT", "BVT" , Performance8_MusicToReferenceTime_BVT, 0, 0},          //PASS
    {"Performance8_PlaySegmentEx_BVT",        "BVT" , Performance8_PlaySegmentEx_BVT, 0, 0},                 //FAILS 3807, 3841 (listening)
    {"Performance8_ReferenceToMusicTime_BVT", "BVT" , Performance8_ReferenceToMusicTime_BVT, 0, 0},          //PASS
    {"Performance8_RemoveNotificationType_BVT","BVT" , Performance8_RemoveNotificationType_BVT, 0, 0},      //Not implemented yet (partly covered in Add)
    {"Performance8_SendPMsg_BVT",             "BVT" , Performance8_SendPMsg_BVT, 0, 0},                      //FAILS 2968 (listening)
    {"Performance8_SetGlobalParam_BVT",       "BVT" , Performance8_SetGlobalParam_BVT, 0, 0},                //PASS
    {"Performance8_StopEx_BVT",               "BVT" , Performance8_StopEx_BVT, 0, 0},                        //PASS   

    //Segment8 Tests
    //--------------
    {"Segment8_Compose_BVT",                  "BVT" , Segment8_Compose_BVT, 0, 0},    //PASS
//    {"Segment8_Download_BVT",                 "BVT" , Segment8_Download_BVT, 0, 0},   //PASS
    {"Segment8_SetRepeats_BVT",               "BVT" , Segment8_SetRepeats_BVT, 0, 0}, //PASS
    {"Segment8_GetLength_BVT",                "BVT" , Segment8_GetLength_BVT , 0, 0}, //PASS
    {"Segment8_SetLength_BVT",                "BVT" , Segment8_SetLength_BVT , 0, 0}, //PASS

    //SegmentState8 Tests
    //-------------------
    {"SegmentState_GetObjectInPath_BVT",      "BVT" , SegmentState_GetObjectInPath_BVT, 0, 0},   //PASS
    {"SegmentState_GetSegment_BVT",           "BVT" , SegmentState_GetSegment_BVT, 0, 0},        //PASS   
    {"SegmentState_GetStartTime_BVT",         "BVT" , SegmentState_GetStartTime_BVT, 0, 0},      //???
    {"SegmentState_SetPitch_BVT",             "BVT" , SegmentState_SetPitch_BVT, 0, 0},          //FAIL: 3514 (Listening)
    {"SegmentState_SetVolume_BVT",            "BVT" , SegmentState_SetVolume_BVT, 0, 0},         //Same as above.

    //ToolGraph tests
    //---------------
    {"ToolGraph_InsertTool_BVT",              "BVT" , ToolGraph_InsertTool_BVT, 0, 0},   
    {"ToolGraph_StampPMsg_BVT",               "BVT" , ToolGraph_StampPMsg_BVT, 0, 0},
    {"ToolGraph_RemoveTool_BVT",              "BVT" , ToolGraph_RemoveTool_BVT, 0, 0},
    {"ToolGraph_GetTool_BVT",                 "BVT" , ToolGraph_GetTool_BVT, 0, 0},


/******************************************************************************************
VALID TEST CASES
******************************************************************************************/

    {"ADPCM_OneShot_11_1",  "Valid", ADPCM_TestWaveSegment, (DWORD)"OneShot", (DWORD)"OS_11_1"},
    {"ADPCM_OneShot_11_2",  "Valid", ADPCM_TestWaveSegment, (DWORD)"OneShot", (DWORD)"OS_11_2"},
    {"ADPCM_OneShot_22_1",  "Valid", ADPCM_TestWaveSegment, (DWORD)"OneShot", (DWORD)"OS_22_1"},
    {"ADPCM_OneShot_22_2",  "Valid", ADPCM_TestWaveSegment, (DWORD)"OneShot", (DWORD)"OS_22_2"},
    {"ADPCM_OneShot_44_1",  "Valid", ADPCM_TestWaveSegment, (DWORD)"OneShot", (DWORD)"OS_44_1"},
    {"ADPCM_OneShot_44_2",  "Valid", ADPCM_TestWaveSegment, (DWORD)"OneShot", (DWORD)"OS_44_2"},

    {"ADPCM_Looping_11_1",  "Valid", ADPCM_TestWaveSegment, (DWORD)"Looping64", (DWORD)"LP_11_1"},
    {"ADPCM_Looping_11_2",  "Valid", ADPCM_TestWaveSegment, (DWORD)"Looping64", (DWORD)"LP_11_2"},
    {"ADPCM_Looping_22_1",  "Valid", ADPCM_TestWaveSegment, (DWORD)"Looping64", (DWORD)"LP_22_1"},
    {"ADPCM_Looping_22_2",  "Valid", ADPCM_TestWaveSegment, (DWORD)"Looping64", (DWORD)"LP_22_2"},
    {"ADPCM_Looping_44_1",  "Valid", ADPCM_TestWaveSegment, (DWORD)"Looping64", (DWORD)"LP_44_1"},
    {"ADPCM_Looping_44_2",  "Valid", ADPCM_TestWaveSegment, (DWORD)"Looping64", (DWORD)"LP_44_2"},

    {"ADPCM_Streaming_11_1",  "Valid", ADPCM_TestWaveSegment, (DWORD)"Streaming", (DWORD)"ST_11_1"},
    {"ADPCM_Streaming_11_2",  "Valid", ADPCM_TestWaveSegment, (DWORD)"Streaming", (DWORD)"ST_11_2"},
    {"ADPCM_Streaming_22_1",  "Valid", ADPCM_TestWaveSegment, (DWORD)"Streaming", (DWORD)"ST_22_1"},
    {"ADPCM_Streaming_22_2",  "Valid", ADPCM_TestWaveSegment, (DWORD)"Streaming", (DWORD)"ST_22_2"},
    {"ADPCM_Streaming_44_1",  "Valid", ADPCM_TestWaveSegment, (DWORD)"Streaming", (DWORD)"ST_44_1"},
    {"ADPCM_Streaming_44_2",  "Valid", ADPCM_TestWaveSegment, (DWORD)"Streaming", (DWORD)"ST_44_2"},


    //TODO: Run these tests.
    {"ADPCM_DLS_Looping_11_1",    "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegDLS_ADPCM_LP_11_1.sgt"},
    {"ADPCM_DLS_Looping_22_1",    "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegDLS_ADPCM_LP_22_1.sgt"},
    {"ADPCM_DLS_Looping_44_1",    "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegDLS_ADPCM_LP_44_1.sgt"},
    {"ADPCM_DLS_OneShot_11_1",    "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegDLS_ADPCM_OS_11_1.sgt"},
    {"ADPCM_DLS_OneShot_22_1",    "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegDLS_ADPCM_OS_22_1.sgt"},
    {"ADPCM_DLS_OneShot_44_1",    "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegDLS_ADPCM_OS_44_1.sgt"},
    {"ADPCM_DLS_Streaming_11_1",  "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegDLS_ADPCM_ST_11_1.sgt"},
    {"ADPCM_DLS_Streaming_22_1",  "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegDLS_ADPCM_ST_22_1.sgt"},
    {"ADPCM_DLS_Streaming_44_1",  "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegDLS_ADPCM_ST_44_1.sgt"},

    {"ADPCM_WaveSeg_Looping_11_1",     "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_LP_11_1.sgt"},
    {"ADPCM_WaveSeg_Looping_11_2",     "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_LP_11_2.sgt"},
    {"ADPCM_WaveSeg_Looping_22_1",     "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_LP_22_1.sgt"},
    {"ADPCM_WaveSeg_Looping_22_2",     "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_LP_22_2.sgt"},
    {"ADPCM_WaveSeg_Looping_44_1",     "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_LP_44_1.sgt"},
    {"ADPCM_WaveSeg_Looping_44_2",     "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_LP_44_2.sgt"},
    {"ADPCM_WaveSeg_OneShot_11_1",     "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_OS_11_1.sgt"},
    {"ADPCM_WaveSeg_OneShot_11_2",     "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_OS_11_2.sgt"},
    {"ADPCM_WaveSeg_OneShot_22_1",     "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_OS_22_1.sgt"},
    {"ADPCM_WaveSeg_OneShot_22_2",     "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_OS_22_2.sgt"},
    {"ADPCM_WaveSeg_OneShot_44_1",     "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_OS_44_1.sgt"},
    {"ADPCM_WaveSeg_OneShot_44_2",     "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_OS_44_2.sgt"},
    {"ADPCM_WaveSeg_Streaming_11_1",   "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_ST_11_1.sgt"},
    {"ADPCM_WaveSeg_Streaming_11_2",   "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_ST_11_2.sgt"},
    {"ADPCM_WaveSeg_Streaming_22_1",   "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_ST_22_1.sgt"},
    {"ADPCM_WaveSeg_Streaming_22_2",   "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_ST_22_2.sgt"},
    {"ADPCM_WaveSeg_Streaming_44_1",   "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_ST_44_1.sgt"},
    {"ADPCM_WaveSeg_Streaming_44_2",   "Valid",  ADPCM_TestMusicSegment, NULL, (DWORD)"SegWAV_ADPCM_ST_44_2.sgt"},


    //Audiopath Tests
    //---------------
    {"AudioPath_Activate_Valid_Many(STEREOPLUSREVERB,191)",  "Valid", AudioPath_Activate_Valid_Many,  DMUS_APATH_SHARED_STEREOPLUSREVERB, 191},
    {"AudioPath_Activate_Valid_Many(STEREO,191)",            "Valid", AudioPath_Activate_Valid_Many,  DMUS_APATH_SHARED_STEREO,           191},
    {"AudioPath_Activate_Valid_Many(3D,64)",                 "Valid", AudioPath_Activate_Valid_Many,  DMUS_APATH_DYNAMIC_3D,              64},
    {"AudioPath_Activate_Valid_Many(MONO,191)",              "Valid", AudioPath_Activate_Valid_Many,  DMUS_APATH_DYNAMIC_MONO,            191},

    {"AudioPath_Activate_Valid_Many(MONO,32)",              "Valid", AudioPath_Activate_Valid_Many,  DMUS_APATH_DYNAMIC_MONO,            32},
    {"AudioPath_Activate_Valid_Many(MONO,64)",              "Valid", AudioPath_Activate_Valid_Many,  DMUS_APATH_DYNAMIC_MONO,            64},
    {"AudioPath_Activate_Valid_Many(MONO,128)",              "Valid", AudioPath_Activate_Valid_Many,  DMUS_APATH_DYNAMIC_MONO,            128},
    {"AudioPath_Activate_Valid_Many(MONO,150)",              "Valid", AudioPath_Activate_Valid_Many,  DMUS_APATH_DYNAMIC_MONO,            150},
    {"AudioPath_Activate_Valid_Many(MONO,160)",              "Valid", AudioPath_Activate_Valid_Many,  DMUS_APATH_DYNAMIC_MONO,            160},

//    {"AudioPath_Activate_Valid_Many(MONO,191)",              "Valid", AudioPath_Activate_Valid_Many,  DMUS_APATH_DYNAMIC_MONO,            136},


    {"AudioPath_Activate_Valid_Sharing_Shared",      "Valid", AudioPath_Activate_Valid_Sharing,  TRUE, 0},
    {"AudioPath_Activate_Valid_Sharing_Unshared",    "Valid", AudioPath_Activate_Valid_Sharing,  FALSE, 0},
    {"AudioPath_Activate_Valid_OnAndOff",            "Valid", AudioPath_Activate_Valid_OnAndOff, 0, 0},

    {"AudioPath_GetObjectInPath_Valid_GetAll",       "Valid", AudioPath_GetObjectInPath_Valid_GetAll, 0, 0},

    {"AudioPath_GetObjectInPath_Valid_Traverse(STEREOPLUSREVERB)",     "Valid", AudioPath_GetObjectInPath_Valid_Traverse, DMUS_APATH_SHARED_STEREOPLUSREVERB, (DWORD)&g_DefaultPathStereoPlusReverb},
    {"AudioPath_GetObjectInPath_Valid_Traverse(MONO)",     "Valid", AudioPath_GetObjectInPath_Valid_Traverse, DMUS_APATH_DYNAMIC_MONO,          (DWORD)&g_DefaultPathMono},
    {"AudioPath_GetObjectInPath_Valid_Traverse(STEREO)",   "Valid", AudioPath_GetObjectInPath_Valid_Traverse, DMUS_APATH_SHARED_STEREO,         (DWORD)&g_DefaultPathStereo},
    {"AudioPath_GetObjectInPath_Valid_Traverse(3D)",     "Valid", AudioPath_GetObjectInPath_Valid_Traverse, DMUS_APATH_DYNAMIC_3D,              (DWORD)&g_DefaultPath3D},

    {"AudioPath_GetObjectInPath_Valid_SimplyGetBuffer","Valid", AudioPath_GetObjectInPath_Valid_SimplyGetBuffer, 0, 0},


    //Audiopath tests - BUFFERS
    //---------------------------
    {"AudioPath_GetObjectInPath_3DBuf_SetAllParameters(3D)",             "Valid", AudioPath_GetObjectInPath_3DBuf_SetAllParameters,          DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetConeAngles(3D)",                "Valid", AudioPath_GetObjectInPath_3DBuf_SetConeAngles,             DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetConeOrientation_BoundaryCases(3D)","Valid", AudioPath_GetObjectInPath_3DBuf_SetConeOrientation_BoundaryCases, DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetConeOrientation_Listening(3D)", "Valid", AudioPath_GetObjectInPath_3DBuf_SetConeOrientation_Listening, DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetConeOutsideVolume(3D)",         "Valid", AudioPath_GetObjectInPath_3DBuf_SetConeOutsideVolume,      DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetMaxDistance_BoundaryCases(3D)", "Valid", AudioPath_GetObjectInPath_3DBuf_SetMaxDistance_BoundaryCases, DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetMaxDistance_InsideSource(3D)",  "Valid", AudioPath_GetObjectInPath_3DBuf_SetMaxDistance_InsideSource, DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetMinDistance_BoundaryCases(3D)", "Valid", AudioPath_GetObjectInPath_3DBuf_SetMinDistance_BoundaryCases, DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetMinDistance_Listening(3D)",     "Valid", AudioPath_GetObjectInPath_3DBuf_SetMinDistance_Listening,  DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetMode(3D)",                      "Valid", AudioPath_GetObjectInPath_3DBuf_SetMode,                   DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetPosition_BoundaryCases(3D)",    "Valid", AudioPath_GetObjectInPath_3DBuf_SetPosition_BoundaryCases, DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetPosition_Listening(3D)",        "Valid", AudioPath_GetObjectInPath_3DBuf_SetPosition_Listening,     DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetVelocity(3D)",                  "Valid", AudioPath_GetObjectInPath_3DBuf_SetVelocity,               DMUS_APATH_DYNAMIC_3D, 0},
//    {"AudioPath_GetObjectInPath_3DBuf_Test_INFINITY_Bug(3D)",            "Valid", AudioPath_GetObjectInPath_3DBuf_INFINITY_Bug,              DMUS_APATH_DYNAMIC_3D, 0},

    {"AudioPath_GetObjectInPath_3DBuf_SetFrequency_Listening",         "Valid", AudioPath_GetObjectInPath_3DBuf_SetFrequency_Listening,      DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetFrequency_Range",             "Valid", AudioPath_GetObjectInPath_3DBuf_SetFrequency_Range,          DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetVolume_Listening",            "Valid", AudioPath_GetObjectInPath_3DBuf_SetVolume_Listening,         DMUS_APATH_DYNAMIC_3D, 0},
    {"AudioPath_GetObjectInPath_3DBuf_SetVolume_Range",                "Valid", AudioPath_GetObjectInPath_3DBuf_SetVolume_Range,             DMUS_APATH_DYNAMIC_3D, 0},

    {"AudioPath_GetObjectInPath_MonoBuf_SetFrequency_Listening",         "Valid", AudioPath_GetObjectInPath_3DBuf_SetFrequency_Listening,  DMUS_APATH_DYNAMIC_MONO, 0},
    {"AudioPath_GetObjectInPath_MonoBuf_SetFrequency_Range",             "Valid", AudioPath_GetObjectInPath_3DBuf_SetFrequency_Range,      DMUS_APATH_DYNAMIC_MONO, 0},
    {"AudioPath_GetObjectInPath_MonoBuf_SetVolume_Listening",            "Valid", AudioPath_GetObjectInPath_3DBuf_SetVolume_Listening,     DMUS_APATH_DYNAMIC_MONO, 0},
    {"AudioPath_GetObjectInPath_MonoBuf_SetVolume_Range",                "Valid", AudioPath_GetObjectInPath_3DBuf_SetVolume_Range,         DMUS_APATH_DYNAMIC_MONO, 0},



//    {"AudioPath_GetObjectInPath_3DBuf_SetChannelVolume_Channel_Listening","Valid", AudioPath_GetObjectInPath_3DBuf_SetChannelVolume_Channel_Listening, DMUS_APATH_DYNAMIC_3D, 0},
//    {"AudioPath_GetObjectInPath_3DBuf_SetChannelVolume_Mask_Listening","Valid",  AudioPath_GetObjectInPath_3DBuf_SetChannelVolume_Mask_Listening, DMUS_APATH_DYNAMIC_3D, 0},



    {"AudioPath_SetVolume_Valid_Ramp_0ms",            "Valid", AudioPath_SetVolume_Valid_Ramp, 0,            DMUS_APATH_DYNAMIC_MONO},
    {"AudioPath_SetVolume_Valid_Ramp_1ms",            "Valid", AudioPath_SetVolume_Valid_Ramp, 1,            DMUS_APATH_DYNAMIC_MONO},
    {"AudioPath_SetVolume_Valid_Ramp_1000ms",         "Valid", AudioPath_SetVolume_Valid_Ramp, 1000,         DMUS_APATH_DYNAMIC_MONO},
    {"AudioPath_SetVolume_Valid_Ramp_10000ms",        "Valid", AudioPath_SetVolume_Valid_Ramp, 10000,        DMUS_APATH_DYNAMIC_MONO},
    {"AudioPath_SetVolume_Valid_Ramp_ULONG_MAXms",    "Valid", AudioPath_SetVolume_Valid_Ramp, ULONG_MAX,    DMUS_APATH_DYNAMIC_MONO},
    {"AudioPath_SetVolume_Valid_Ramp_0ms_STEREOPLUSREVERB",            "Valid", AudioPath_SetVolume_Valid_Ramp, 0,            DMUS_APATH_SHARED_STEREOPLUSREVERB},
    {"AudioPath_SetVolume_Valid_Ramp_1ms_STEREOPLUSREVERB",            "Valid", AudioPath_SetVolume_Valid_Ramp, 1,            DMUS_APATH_SHARED_STEREOPLUSREVERB},
    {"AudioPath_SetVolume_Valid_Ramp_1000ms_STEREOPLUSREVERB",         "Valid", AudioPath_SetVolume_Valid_Ramp, 1000,         DMUS_APATH_SHARED_STEREOPLUSREVERB},
    {"AudioPath_SetVolume_Valid_Ramp_10000ms_STEREOPLUSREVERB",        "Valid", AudioPath_SetVolume_Valid_Ramp, 10000,        DMUS_APATH_SHARED_STEREOPLUSREVERB},
    {"AudioPath_SetVolume_Valid_Ramp_ULONG_MAXms_STEREOPLUSREVERB",    "Valid", AudioPath_SetVolume_Valid_Ramp, ULONG_MAX,    DMUS_APATH_SHARED_STEREOPLUSREVERB},

    {"AudioPath_SetPitch_Valid_Immediate",           "Valid", AudioPath_SetPitch_Valid_Immediate, 0, 0},
    {"AudioPath_SetPitch_Valid_Ramp_0ms(listen)",    "Valid", AudioPath_SetPitch_Valid_Ramp, 0,      2400},
    {"AudioPath_SetPitch_Valid_Ramp_1000ms(listen)", "Valid", AudioPath_SetPitch_Valid_Ramp, 1000,   2400},
    {"AudioPath_SetPitch_Valid_Ramp_10000ms(listen)","Valid", AudioPath_SetPitch_Valid_Ramp, 10000,  2400},

    {"AudioPath_SetVolume_Valid_Ind(DLS,DYNAMIC)",           "Valid", AudioPath_SetVolume_Valid_Ind, FALSE,  FALSE},
    {"AudioPath_SetVolume_Valid_Ind(WAVE,DYNAMIC)",          "Valid", AudioPath_SetVolume_Valid_Ind, TRUE,   FALSE},
    {"AudioPath_SetVolume_Valid_Ind(DLS,SHARED)",            "Valid", AudioPath_SetVolume_Valid_Ind, FALSE,  TRUE},
    {"AudioPath_SetVolume_Valid_Ind(WAVE,SHARED)",           "Valid", AudioPath_SetVolume_Valid_Ind, TRUE,   TRUE},
    


    //DLS Tests
    //---------     
     //Volume
     {"DLS_TestSegment(0000_Attack_00s)",  "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0000_Attack_00s.sgt", 0},
     {"DLS_TestSegment(0001_Attack_01s)",  "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0001_Attack_01s.sgt", 0},
     {"DLS_TestSegment(0002_Attack_05s)",  "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0002_Attack_05s.sgt", 0},
     {"DLS_TestSegment(0003_Attack_40s)",  "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0003_Attack_40s.sgt", 0},
     {"DLS_TestSegment(0010_Decay_00s)",   "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0010_Decay_00s.sgt", 0},
     {"DLS_TestSegment(0011_Decay_01s)",   "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0011_Decay_01s.sgt", 0},
     {"DLS_TestSegment(0012_Decay_05s)",   "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0012_Decay_05s.sgt", 0},
     {"DLS_TestSegment(0013_Decay_40s)",   "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0013_Decay_40s.sgt", 0},
     {"DLS_TestSegment(0020_Hold_00s)",    "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0020_Hold_00s.sgt", 0},
     {"DLS_TestSegment(0021_Hold_01s)",    "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0021_Hold_01s.sgt", 0},
     {"DLS_TestSegment(0022_Hold_05s)",    "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0022_Hold_05s.sgt", 0},
     {"DLS_TestSegment(0023_Hold_40s)",    "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0023_Hold_40s.sgt", 0},
     {"DLS_TestSegment(0031_Release_01s)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0031_Release_01s.sgt", 0},
     {"DLS_TestSegment(0032_Release_05s)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0032_Release_05s.sgt", 0},
     {"DLS_TestSegment(0033_Release_40s)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0033_Release_40s.sgt", 0},
     {"DLS_TestSegment(0040_Sustain_000)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0040_Sustain_000.sgt", 0},
     {"DLS_TestSegment(0041_Sustain_050)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0041_Sustain_050.sgt", 0},
     {"DLS_TestSegment(0042_Sustain_085)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0042_Sustain_085.sgt", 0},
     {"DLS_TestSegment(0043_Sustain_100)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0043_Sustain_100.sgt", 0},
     {"DLS_TestSegment(0050_Pan_Left_50)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0050_Pan_Left_50.sgt", 0},
     {"DLS_TestSegment(0051_Pan_Left_25)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0051_Pan_Left_25.sgt", 0},
     {"DLS_TestSegment(0052_Pan_Mid)",     "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0052_Pan_Mid.sgt", 0},
     {"DLS_TestSegment(0053_Pan_Right_25)","Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0053_Pan_Right_25.sgt", 0},
     {"DLS_TestSegment(0054_Pan_Right_50)","Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Volume/0054_Pan_Right_50.sgt", 0},

     //Pitch
     {"DLS_TestSegment(0100_Attack_00s)",  "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0100_Attack_00s.sgt", 0},
     {"DLS_TestSegment(0101_Attack_01s)",  "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0101_Attack_01s.sgt", 0},
     {"DLS_TestSegment(0102_Attack_05s)",  "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0102_Attack_05s.sgt", 0},
     {"DLS_TestSegment(0103_Attack_40s)",  "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0103_Attack_40s.sgt", 0},
     {"DLS_TestSegment(0110_Decay_00s)",   "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0110_Decay_00s.sgt", 0},
     {"DLS_TestSegment(0111_Decay_01s)",   "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0111_Decay_01s.sgt", 0},
     {"DLS_TestSegment(0112_Decay_05s)",   "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0112_Decay_05s.sgt", 0},
     {"DLS_TestSegment(0113_Decay_40s)",   "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0113_Decay_40s.sgt", 0},
     {"DLS_TestSegment(0120_Hold_00s)",    "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0120_Hold_00s.sgt", 0},
     {"DLS_TestSegment(0121_Hold_01s)",    "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0121_Hold_01s.sgt", 0},
     {"DLS_TestSegment(0122_Hold_05s)",    "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0122_Hold_05s.sgt", 0},
     {"DLS_TestSegment(0123_Hold_40s)",    "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0123_Hold_40s.sgt", 0},
     {"DLS_TestSegment(0130_Release_00s)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0130_Release_00s.sgt", 0},
     {"DLS_TestSegment(0131_Release_01s)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0131_Release_01s.sgt", 0},
     {"DLS_TestSegment(0132_Release_05s)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0132_Release_05s.sgt", 0},
     {"DLS_TestSegment(0133_Release_40s)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0133_Release_40s.sgt", 0},
     {"DLS_TestSegment(0140_Sustain_000)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0140_Sustain_000.sgt", 0},
     {"DLS_TestSegment(0141_Sustain_050)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0141_Sustain_050.sgt", 0},
     {"DLS_TestSegment(0142_Sustain_085)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0142_Sustain_085.sgt", 0},
     {"DLS_TestSegment(0143_Sustain_100)", "Valid",  DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0143_Sustain_100.sgt", 0},
     {"DLS_TestSegment(0150_Range_-01)","Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0150_Range_-01.sgt", 0},
     {"DLS_TestSegment(0151_Range_-12)","Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0151_Range_-12.sgt", 0},
     {"DLS_TestSegment(0152_Range_01)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0152_Range_01.sgt", 0},
     {"DLS_TestSegment(0153_Range_12)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Pitch/0153_Range_12.sgt", 0},


    //Mod LFO
     {"DLS_TestSegment(0200_ChanPressToFc_00000)",  "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0200_ChanPressToFc_00000.sgt", 0},
     {"DLS_TestSegment(0200_ChanPressToFc_12800)",  "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0200_ChanPressToFc_12800.sgt", 0},
     {"DLS_TestSegment(0200_ChanPressToFc_-12800)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0200_ChanPressToFc_-12800.sgt", 0},
     {"DLS_TestSegment(0210_ChanPressToGain_00db)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0210_ChanPressToGain_00db.sgt", 0},
     {"DLS_TestSegment(0211_ChanPressToGain_12db)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0211_ChanPressToGain_12db.sgt", 0},
     {"DLS_TestSegment(0220_ChanPressToPitch_-06)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0220_ChanPressToPitch_-06.sgt", 0},
     {"DLS_TestSegment(0221_ChanPressToPitch_-12)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0221_ChanPressToPitch_-12.sgt", 0},
     {"DLS_TestSegment(0222_ChanPressToPitch_00)",  "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0222_ChanPressToPitch_00.sgt", 0},
     {"DLS_TestSegment(0223_ChanPressToPitch_06)",  "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0223_ChanPressToPitch_06.sgt", 0},
     {"DLS_TestSegment(0224_ChanPressToPitch_12)",  "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0224_ChanPressToPitch_12.sgt", 0},
     {"DLS_TestSegment(0230_Delay00s)",             "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0230_Delay00s.sgt", 0},
     {"DLS_TestSegment(0231_Delay01s)",             "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0231_Delay01s.sgt", 0},
     {"DLS_TestSegment(0232_Delay05s)",             "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0232_Delay05s.sgt", 0},
     {"DLS_TestSegment(0233_Delay10s)",             "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0233_Delay10s.sgt", 0},
     {"DLS_TestSegment(0240_Frequency_01Hz)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0240_Frequency_01Hz.sgt", 0},
     {"DLS_TestSegment(0241_Frequency_05Hz)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0241_Frequency_05Hz.sgt", 0},
     {"DLS_TestSegment(0242_Frequency_0p1Hz)",      "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0242_Frequency_0p1Hz.sgt", 0},
     {"DLS_TestSegment(0242_Frequency_10Hz)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0242_Frequency_10Hz.sgt", 0},
     {"DLS_TestSegment(0250_MWToPitch_-12)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0250_MWToPitch_-12.sgt", 0},
     {"DLS_TestSegment(0251_MWToPitch_12)",         "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0251_MWToPitch_12.sgt", 0},
     {"DLS_TestSegment(0260_MWToVolume_12)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0260_MWToVolume_12.sgt", 0},
     {"DLS_TestSegment(0270_PitchRange_-01)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0270_PitchRange_-01.sgt", 0},
     {"DLS_TestSegment(0271_PitchRange_-12)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0271_PitchRange_-12.sgt", 0},
     {"DLS_TestSegment(0272_PitchRange_00)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0272_PitchRange_00.sgt", 0},
     {"DLS_TestSegment(0273_PitchRange_01)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0273_PitchRange_01.sgt", 0},
     {"DLS_TestSegment(0274_PitchRange_12)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0274_PitchRange_12.sgt", 0},
     {"DLS_TestSegment(0280_VolRange_00db)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0280_VolRange_00db.sgt", 0},
     {"DLS_TestSegment(0281_VolRange_01db)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0281_VolRange_01db.sgt", 0},
     {"DLS_TestSegment(0282_VolRange_05db)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0282_VolRange_05db.sgt", 0},
     {"DLS_TestSegment(0283_VolRange_12db)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/ModLFO/0283_VolRange_12db.sgt", 0},

    //Vib LFO
     {"DLS_TestSegment(0300_ChanPressToPitch_00)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0300_ChanPressToPitch_00.sgt", 0},
     {"DLS_TestSegment(0301_ChanPressToPitch_-06)","Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0301_ChanPressToPitch_-06.sgt", 0},
     {"DLS_TestSegment(0302_ChanPressToPitch_06)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0302_ChanPressToPitch_06.sgt", 0},
     {"DLS_TestSegment(0303_ChanPressToPitch_-12)","Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0303_ChanPressToPitch_-12.sgt", 0},
     {"DLS_TestSegment(0304_ChanPressToPitch_12)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0304_ChanPressToPitch_12.sgt", 0},
     {"DLS_TestSegment(0310_Delay_00s)",           "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0310_Delay_00s.sgt", 0},
     {"DLS_TestSegment(0311_Delay_01s)",           "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0311_Delay_01s.sgt", 0},
     {"DLS_TestSegment(0312_Delay_05s)",           "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0312_Delay_05s.sgt", 0},
     {"DLS_TestSegment(0313_Delay_10s)",           "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0313_Delay_10s.sgt", 0},
     {"DLS_TestSegment(0320_Frequency_0p01)",      "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0320_Frequency_0p01.sgt", 0},
     {"DLS_TestSegment(0321_Frequency_01)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0321_Frequency_01.sgt", 0},
     {"DLS_TestSegment(0322_Frequency_05)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0322_Frequency_05.sgt", 0},
     {"DLS_TestSegment(0323_Frequency_10)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0323_Frequency_10.sgt", 0},
     {"DLS_TestSegment(0324_Frequency_20)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0324_Frequency_20.sgt", 0},
     {"DLS_TestSegment(0330_MWToPitch_00)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0330_MWToPitch_00.sgt", 0},
     {"DLS_TestSegment(0331_MWToPitch_-06)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0331_MWToPitch_-06.sgt", 0},
     {"DLS_TestSegment(0332_MWToPitch_06)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0332_MWToPitch_06.sgt", 0},
     {"DLS_TestSegment(0333_MWToPitch_-1)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0333_MWToPitch_-1.sgt", 0},
     {"DLS_TestSegment(0334_MWToPitch_12)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0334_MWToPitch_12.sgt", 0},
     {"DLS_TestSegment(0350_PitchRange_00)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0350_PitchRange_00.sgt", 0},
     {"DLS_TestSegment(0351_PitchRange_-06)",      "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0351_PitchRange_-06.sgt", 0},
     {"DLS_TestSegment(0352_PitchRange_06)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0352_PitchRange_06.sgt", 0},
     {"DLS_TestSegment(0353_PitchRange_-12)",      "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0353_PitchRange_-12.sgt", 0},
     {"DLS_TestSegment(0354_PitchRange_12)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/VibLFO/0354_PitchRange_12.sgt", 0},


    //Filter
     {"DLS_TestSegment(0500_EnableFilter_Off)",    "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0500_EnableFilter_Off.sgt", 0},
     {"DLS_TestSegment(0500_EnableFilter_Off1)",   "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0500_EnableFilter_Off1.sgt", 0},
     {"DLS_TestSegment(0510_InitialFc_05535)",     "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0510_InitialFc_05535.sgt", 0},
     {"DLS_TestSegment(0511_InitialFc_11921)",     "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0511_InitialFc_11921.sgt", 0},
     {"DLS_TestSegment(0520_KeyNumberToFc_0)",     "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0520_KeyNumberToFc_0.sgt", 0},
     {"DLS_TestSegment(0521_KeyNumberToFc_-1000)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0521_KeyNumberToFc_-1000.sgt", 0},
     {"DLS_TestSegment(0522_KeyNumberToFc_1000)",  "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0522_KeyNumberToFc_1000.sgt", 0},
     {"DLS_TestSegment(0523_KeyNumberToFc_-12800)","Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0523_KeyNumberToFc_-12800.sgt", 0},
     {"DLS_TestSegment(0524_KeyNumberToFc_12800)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0524_KeyNumberToFc_12800.sgt", 0},
     {"DLS_TestSegment(0530_KeyVelocityToFc_0)",   "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0530_KeyVelocityToFc_0.sgt", 0},
     {"DLS_TestSegment(0531_KeyVelocityToFc_-1000)","Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0531_KeyVelocityToFc_-1000.sgt", 0},
     {"DLS_TestSegment(0532_KeyVelocityToFc_1000)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0532_KeyVelocityToFc_1000.sgt", 0},
     {"DLS_TestSegment(0533_KeyVelocityToFc_-12800)","Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0533_KeyVelocityToFc_-12800.sgt", 0},
     {"DLS_TestSegment(0534_KeyVelocityToFc_12800)", "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0534_KeyVelocityToFc_12800.sgt", 0},
     {"DLS_TestSegment(0540_MODLFOCC1ToFc_0)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0540_MODLFOCC1ToFc_0.sgt", 0},
     {"DLS_TestSegment(0541_MODLFOCC1ToFc_-4000)",   "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0541_MODLFOCC1ToFc_-4000.sgt", 0},
     {"DLS_TestSegment(0542_MODLFOCC1ToFc_4000)",    "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0542_MODLFOCC1ToFc_4000.sgt", 0},
     {"DLS_TestSegment(0543_MODLFOCC1ToFc_-12800)",  "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0543_MODLFOCC1ToFc_-12800.sgt", 0},
     {"DLS_TestSegment(0544_MODLFOCC1ToFc_12800)",   "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0544_MODLFOCC1ToFc_12800.sgt", 0},
     {"DLS_TestSegment(0550_MODLFOToFc_0)",          "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0550_MODLFOToFc_0.sgt", 0},
     {"DLS_TestSegment(0551_MODLFOToFc_-4000)",      "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0551_MODLFOToFc_-4000.sgt", 0},
     {"DLS_TestSegment(0552_MODLFOToFc_4000)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0552_MODLFOToFc_4000.sgt", 0},
     {"DLS_TestSegment(0553_MODLFOToFc_-12800)",     "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0553_MODLFOToFc_-12800.sgt", 0},
     {"DLS_TestSegment(0554_MODLFOToFc_12800)",      "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0554_MODLFOToFc_12800.sgt", 0},
     {"DLS_TestSegment(0561_PitchEnvToFc_-1000)",    "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0561_PitchEnvToFc_-1000.sgt", 0},
     {"DLS_TestSegment(0562_PitchEnvToFc_1000)",     "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0562_PitchEnvToFc_1000.sgt", 0},
     {"DLS_TestSegment(0563_PitchEnvToFc_-6000)",    "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0563_PitchEnvToFc_-6000.sgt", 0},
     {"DLS_TestSegment(0564_PitchEnvToFc_6000)",     "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0564_PitchEnvToFc_6000.sgt", 0},
     {"DLS_TestSegment(0565_PitchEnvToFc_-12800)",   "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0565_PitchEnvToFc_-12800.sgt", 0},
     {"DLS_TestSegment(0566_PitchEnvToFc_12800)",    "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0566_PitchEnvToFc_12800.sgt", 0},
     {"DLS_TestSegment(0570_Resonance_00db)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0570_Resonance_00db.sgt", 0},
     {"DLS_TestSegment(0571_Resonance_22db)",        "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Filter/0571_Resonance_22db.sgt", 0},

    //Regions
     {"DLS_TestSegment(0601_Layers)",                    "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Regions/0601_Layers.sgt", 0},
     {"DLS_TestSegment(0602_Layers_Multiple_Waves)",     "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Regions/0602_Layers_Multiple_Waves.sgt", 0},
     {"DLS_TestSegment(0603_NotesOutsideRegionDontPlay)","Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Regions/0603_NotesOutsideRegionDontPlay.sgt", 0},
     {"DLS_TestSegment(0610_VelocityRange_Exclude)",     "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Regions/0610_VelocityRange_Exclude.sgt", 0},
     {"DLS_TestSegment(0611_VelocityRange_Include)",     "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/Regions/0611_VelocityRange_Include.sgt", 0},


    //Continuous Controllers (CCs)
     {"DLS_TestSegment(CC1_001_ModWheel)",           "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_001_ModWheel.sgt", 0},
     {"DLS_TestSegment(CC1_002_BreathController)",   "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_002_BreathController.sgt", 0},
     {"DLS_TestSegment(CC1_007_Volume)",             "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_007_Volume.sgt", 0},
     {"DLS_TestSegment(CC1_008_Balance)",            "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_008_Balance.sgt", 0},
     {"DLS_TestSegment(CC1_010_Pan)",                "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_010_Pan.sgt", 0},
     {"DLS_TestSegment(CC1_011_Expression)",         "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_011_Expression.sgt", 0},
     {"DLS_TestSegment(CC1_064_Sustain)",            "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_064_Sustain.sgt", 0},
     {"DLS_TestSegment(CC1_069_Hold2)",              "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_069_Hold2.sgt", 0},
     {"DLS_TestSegment(CC1_074_FilterCutoff)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_074_FilterCutoff.sgt", 0},
     {"DLS_TestSegment(CC1_091_ExtEffects)",         "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_091_ExtEffects.sgt", 0},
     {"DLS_TestSegment(CC1_092_Tremelo)",            "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_092_Tremelo.sgt", 0},
     {"DLS_TestSegment(CC1_093_Chorus)",             "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_093_Chorus.sgt", 0},
     {"DLS_TestSegment(CC1_MonoAT)",                 "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_MonoAT.sgt", 0},
     {"DLS_TestSegment(CC1_PBend)",                  "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_PBend.sgt", 0},
     {"DLS_TestSegment(CC1_RPN_CoarseTuning)",       "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_RPN_CoarseTuning.sgt", 0},
     {"DLS_TestSegment(CC1_RPN_FineTuning)",         "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_RPN_FineTuning.sgt", 0},
     {"DLS_TestSegment(CC1_RPN_PitchBendRange)",     "Valid", DLS_TestSegment, (DWORD)"DMusic/DMTest1/DLS/CC/CC1_RPN_PitchBendRange.sgt", 0},


    //Loader Tests
    //------------
    //Move the real tests in here!!
//     {" Loader_ClearCache",  CC1_001_ModWheel.sgt", Loader_ClearCache_Valid(ptPerf8));
     {"Loader_LoadObjectFromFile_Valid",              "Valid",  Loader_LoadObjectFromFile_Valid, 0, 0},     //FAIL 3802
     {"Loader_LoadObjectFromFile_Valid_AbsolutePath(SegEmbedWave)", "Valid",  Loader_LoadObjectFromFile_Valid_AbsolutePath, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegEmbedWave.sgt", 0},
     {"Loader_LoadObjectFromFile_Valid_Bug7707", "Valid",  Loader_LoadObjectFromFile_Valid_Bug7707, 0, 0},
     

//These don't work anyway.
//     {"Loader_LoadObjectFromFile_Container_ByFileName(SegmentDLS)",   "Valid",  Loader_LoadObjectFromFile_Container_ByFileName, FALSE, (DWORD)"ff2end.sgt"},     
//     {"Loader_LoadObjectFromFile_Container_ByFileName(Wave)",         "Valid",  Loader_LoadObjectFromFile_Container_ByFileName,       TRUE, (DWORD)"looping20s.wav"}, 
//     {"Loader_LoadObjectFromFile_Container_ByFileName_Script(SegmentDLS)", "Valid",  Loader_LoadObjectFromFile_Container_ByFileName_Script,       FALSE, (DWORD)"ff2end.sgt"},     
//     {"Loader_LoadObjectFromFile_Container_ByFileName_Script(Wave)",       "Valid",  Loader_LoadObjectFromFile_Container_ByFileName_Script,       TRUE, (DWORD)"looping20s.wav"}, 

     {"Loader_LoadObjectFromFile_Container_ByName(SegmentDLS)",       "Valid",  Loader_LoadObjectFromFile_Container_ByName,     FALSE, (DWORD)"ff2end"},     
     {"Loader_LoadObjectFromFile_Container_ByGUID(SegmentDLS)",       "Valid",  Loader_LoadObjectFromFile_Container_ByGUID,     FALSE, (DWORD)"{C7C88355-14E1-4A6D-B45B-3F099DF28DB5}"},                                                                                                                                        

//WontFixed bug 8444
//     {"Loader_LoadObjectFromFile_Container_ByName(Wave)",             "Valid",  Loader_LoadObjectFromFile_Container_ByName,           TRUE, (DWORD)"looping20s"},     
//     {"Loader_LoadObjectFromFile_Container_ByGUID(Wave)",             "Valid",  Loader_LoadObjectFromFile_Container_ByGUID,           TRUE, (DWORD)"{7589C574-79E4-4564-BA9F-486B99B6F648}"},
     {"Loader_LoadObjectFromFile_Container_ByName_Script(SegmentDLS)",     "Valid",  Loader_LoadObjectFromFile_Container_ByName_Script,           FALSE, (DWORD)"ff2end"},     
     {"Loader_LoadObjectFromFile_Container_ByGUID_Script(SegmentDLS)",     "Valid",  Loader_LoadObjectFromFile_Container_ByGUID_Script,           FALSE, (DWORD)"{C7C88355-14E1-4A6D-B45B-3F099DF28DB5}"},                                                                                                                                        
     {"Loader_LoadObjectFromFile_Container_ByName_Script(Wave)",           "Valid",  Loader_LoadObjectFromFile_Container_ByName_Script,           TRUE, (DWORD)"looping20s"},     
     {"Loader_LoadObjectFromFile_Container_ByGUID_Script(Wave)",           "Valid",  Loader_LoadObjectFromFile_Container_ByGUID_Script,           TRUE, (DWORD)"{7589C574-79E4-4564-BA9F-486B99B6F648}"},

     //{"Loader_LoadObjectFromFile_Container_ByName(Wave)",       "Valid",  Loader_LoadObjectFromFile_Container_ByName, (DWORD)"DMusic/DMTest1/Loader/Cont01/Cont01.con", (DWORD)"Looping20s.wav"},


     //{"Loader_CollectGarbage_Valid",                  "Valid" , Loader_CollectGarbage_Valid, 0, 0},
     {"Loader_GetObject_IStream(test)",                "Valid" , Loader_GetObject_IStream_Test,         (DWORD)"DMusic/DMTest1/test.sgt", 0},
     {"Loader_GetObject_IStream(streaming)",           "Valid" , Loader_GetObject_IStream_Streaming,    (DWORD)"WAV/Streaming10s.wav", 0},

//     {"Loader_GetObject",                           "Valid",  Loader_GetObject_Valid, 0, 0},
//     {"Loader_SetObject",                           "Valid",  Loader_SetObject_Valid, 0, 0},
//     {"Loader_SetSearchDirectory",                  "Valid",  Loader_SetSearchDirectory_Valid, 0, 0},
//     {"Loader_ReleaseObjectByUnknown",              "Valid",  Loader_ReleaseObjectByUnknown_Valid, 0, 0},

    
    //Performance8 Tests
    //------------------
    {"Performance8_AddNotificationType_NoRestart(1,0)",       "Valid", Performance8_AddNotificationType_NoRestart, TESTNOTIFICATION_MEASUREANDBEAT,    0},       //PASS
    {"Performance8_AddNotificationType_NoRestart(2,0)",       "Valid", Performance8_AddNotificationType_NoRestart, TESTNOTIFICATION_PERFORMANCE,       0},       //FAIL, 3240
    {"Performance8_AddNotificationType_NoRestart(4,0)",       "Valid", Performance8_AddNotificationType_NoRestart, TESTNOTIFICATION_SEGMENT,           0},       //FAIL, 3240
    {"Performance8_AddNotificationType_NoRestart(7,0)",       "Valid", Performance8_AddNotificationType_NoRestart, TESTNOTIFICATION_ALL,               0},       //FAIL, 3240

    //Verify that removals work.
    {"Performance8_AddNotificationType_NoRestart(1,1)",       "Valid", Performance8_AddNotificationType_NoRestart, TESTNOTIFICATION_MEASUREANDBEAT,    TESTNOTIFICATION_MEASUREANDBEAT},
    {"Performance8_AddNotificationType_NoRestart(2,2)",       "Valid", Performance8_AddNotificationType_NoRestart, TESTNOTIFICATION_PERFORMANCE,       TESTNOTIFICATION_PERFORMANCE},
    {"Performance8_AddNotificationType_NoRestart(4,4)",       "Valid", Performance8_AddNotificationType_NoRestart, TESTNOTIFICATION_SEGMENT,           TESTNOTIFICATION_SEGMENT},
    {"Performance8_AddNotificationType_NoRestart(7,7)",       "Valid", Performance8_AddNotificationType_NoRestart, TESTNOTIFICATION_ALL,               TESTNOTIFICATION_ALL},

    //Make sure that each removal.is the correct one.
    {"Performance8_AddNotificationType_NoRestart(1,2)",       "Valid", Performance8_AddNotificationType_NoRestart, TESTNOTIFICATION_MEASUREANDBEAT,    TESTNOTIFICATION_PERFORMANCE},
    {"Performance8_AddNotificationType_NoRestart(2,4)",       "Valid", Performance8_AddNotificationType_NoRestart, TESTNOTIFICATION_PERFORMANCE,       TESTNOTIFICATION_SEGMENT},
    {"Performance8_AddNotificationType_NoRestart(4,1)",       "Valid", Performance8_AddNotificationType_NoRestart, TESTNOTIFICATION_SEGMENT,           TESTNOTIFICATION_MEASUREANDBEAT},

    //Make sure that duplicate removals don't hurt anything
    {"Performance8_AddNotificationType_NoRestart(0,1)",       "Valid", Performance8_AddNotificationType_NoRestart, 0, TESTNOTIFICATION_MEASUREANDBEAT},
    {"Performance8_AddNotificationType_NoRestart(0,2)",       "Valid", Performance8_AddNotificationType_NoRestart, 0, TESTNOTIFICATION_PERFORMANCE},
    {"Performance8_AddNotificationType_NoRestart(0,4)",       "Valid", Performance8_AddNotificationType_NoRestart, 0, TESTNOTIFICATION_SEGMENT},

    //Make sure all the notifications work after you restart a segment.
    {"Performance8_AddNotificationType_Restart(1,0)",       "Valid", Performance8_AddNotificationType_Restart, TESTNOTIFICATION_MEASUREANDBEAT,    0},       //PASS
    {"Performance8_AddNotificationType_Restart(2,0)",       "Valid", Performance8_AddNotificationType_Restart, TESTNOTIFICATION_PERFORMANCE,       0},       //FAIL, 3240
    {"Performance8_AddNotificationType_Restart(4,0)",       "Valid", Performance8_AddNotificationType_Restart, TESTNOTIFICATION_SEGMENT,           0},       //FAIL, 3240



     {"Performance8_AllocPMsg_Valid",                 "Valid", Performance8_AllocPMsg_Valid, 0, 0},                      
     {"Performance8_ClonePMsg_Valid",                 "Valid", Performance8_ClonePMsg_Valid, 0, 0},                   

      //Create standard audiopath.
     {"Performance8_CreateStandardAudioPath_AllCombos(FALSE)","Valid", Performance8_CreateStandardAudioPath_AllCombos, FALSE, 0},
     {"Performance8_CreateStandardAudioPath_AllCombos(TRUE)","Valid", Performance8_CreateStandardAudioPath_AllCombos, TRUE,  0},
     {"Performance8_CreateStandardAudioPath_Wait","Valid", Performance8_CreateStandardAudioPath_Wait, 0,  0},
     {"Performance8_Play2AudioPaths",             "Valid", Performance8_Play2AudioPaths, 0,  0},

     {"Performance8_CloseDown(BEFORESTOPPING)",             "Valid", Performance8_CloseDown_WhilePlaying, CLOSEDOWN_BEFORESTOPPING,  0},
//     {"Performance8_CloseDown(BEFOREUNLOADING)",            "Valid", Performance8_CloseDown_WhilePlaying, CLOSEDOWN_BEFOREUNLOADING,  0},
     {"Performance8_CloseDown(BEFORERELEASINGPATH)",        "Valid", Performance8_CloseDown_WhilePlaying, CLOSEDOWN_BEFORERELEASINGPATH,  0},
     {"Performance8_CloseDown(AFTEREVERYTHINGELSE)",        "Valid", Performance8_CloseDown_WhilePlaying, CLOSEDOWN_AFTEREVERYTHINGELSE,  0},
     {"Performance8_CloseDown(TWICE)",                      "Valid", Performance8_CloseDown_WhilePlaying, CLOSEDOWN_TWICE,  0},

     {"Performance8_CreateStandardAudioPath_LimitPChannels","Valid", Performance8_CreateStandardAudioPath_LimitPChannels, 0,  0},

     {"Performance8_FreePMsg_Valid",                  "Valid", Performance8_FreePMsg_Valid, 0, 0},                
     {"Performance8_GetGlobalParam_Valid",            "Valid", Performance8_GetGlobalParam_Valid, 0, 0},          
     {"Performance8_GetNotificationPMsg_Valid_Segment","Valid", Performance8_GetNotificationPMsg_Valid, 0, 0},
     {"Performance8_GetSegmentState_Valid",           "Valid", Performance8_GetSegmentState_Valid, 0, 0},           
     {"Performance8_GetTime_Valid_InOrder",           "Valid", Performance8_GetTime_Valid_InOrder, 0, 0},   
     //{"Performance8_InitAudio_Valid",               "Valid", Performance8_InitAudio_Valid, 0, 0},                            //Not tested.
     {"Performance8_IsPlaying_Valid",                 "Valid", Performance8_IsPlaying_Valid, 0, 0},                              //FAILS due to 3677
     {"Performance8_GetSetDefaultAudioPath_Valid",    "Valid", Performance8_GetSetDefaultAudioPath_Valid, 0, 0},    //PASSES!
     {"Performance8_MusicToReferenceTime_Repeat",     "Valid", Performance8_MusicToReferenceTime_Repeat,      0, 0},
     {"Performance8_MusicToReferenceTime_TempoChange","Valid", Performance8_MusicToReferenceTime_TempoChange, 0, 0},

//     {"Performance8_PlaySegmentEx_Valid",             "Valid", Performance8_PlaySegmentEx_Valid, 0, 0},          //FAIL: 3713, 3714, 3717


    //Multiple copies of the same wave streaming at different points; no repeating.
    {"Performance8_PlaySegmentEx_Multiple(Streaming10s,30,NoRepeat)", "Valid", Performance8_PlaySegmentEx_Multiple_NoRepeats, (DWORD)"WAV/Streaming10s.wav", 30},

    //Multiple copies of the same wave streaming at different points; repeating.
    {"Performance8_PlaySegmentEx_Multiple(Streaming10s,30,Repeat)", "Valid", Performance8_PlaySegmentEx_Multiple_OneRepeat, (DWORD)"WAV/Streaming10s.wav", 30},
    
    //Multiple different waves, streaming.
    {"Performance8_PlaySegmentEx_Multiple(Random,NoRepeat)", "Valid", Performance8_PlaySegmentEx_Multiple_NoRepeats,  NULL, 0},
    {"Performance8_PlaySegmentEx_Multiple(Random,OneRepeat)", "Valid", Performance8_PlaySegmentEx_Multiple_OneRepeat, NULL, 0},  

    //Multiple looping waves, not repeating and repeating", "Valid", it doesn't make a difference).
    {"Performance8_PlaySegmentEx_Multiple(Looping,NoRepeat)", "Valid", Performance8_PlaySegmentEx_Multiple_NoRepeats, (DWORD)"WAV/LoopForever.wav", 5},
    {"Performance8_PlaySegmentEx_Multiple(Looping,OneRepeat)", "Valid", Performance8_PlaySegmentEx_Multiple_OneRepeat, (DWORD)"WAV/LoopForever.wav", 5},


     {"Performance8_PlaySegmentEx_Valid_Play(Default,10)","Valid", Performance8_PlaySegmentEx_Valid_Play,                   (DWORD)"DEFAULT", 10}, //Play for 10 seconds.
     {"Performance8_PlaySegmentEx_Valid_Play(Desert,10)","Valid", Performance8_PlaySegmentEx_Valid_Play,                    (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/Scott01/DesertAmbience.sgt", 10}, //Play for 10 seconds.
     {"Performance8_PlaySegmentEx_Valid_Play(IndSegment,All)",    "Valid", Performance8_PlaySegmentEx_Valid_Play,           (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/IndSegment.sgt",        0xFFFFFFFF}, //Play all the way.
     {"Performance8_PlaySegmentEx_Valid_Play(RefSegment,All)",    "Valid", Performance8_PlaySegmentEx_Valid_Play,           (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/RefSegment.sgt",        0xFFFFFFFF},
//     {"Performance8_PlaySegmentEx_Valid_Play(Seg16MBDLS,5)",      "Valid", Performance8_PlaySegmentEx_Valid_Play,         (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/Seg16MBDLS.sgt",      0xFFFFFFFF},
     {"Performance8_PlaySegmentEx_Valid_Play(SegChordmap,All)",   "Valid", Performance8_PlaySegmentEx_Valid_Play,           (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegChordmap.sgt",       0xFFFFFFFF},
     {"Performance8_PlaySegmentEx_Valid_Play(SegTriggering,All)", "Valid", Performance8_PlaySegmentEx_Valid_Play,           (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegTriggering.sgt",     0xFFFFFFFF},
      {"Performance8_PlaySegmentEx_Valid_Play(SegWaveLoopForever,All)","Valid", Performance8_PlaySegmentEx_Valid_Play,      (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegWaveLoopForever.sgt",0xFFFFFFFF}, 
     {"Performance8_PlaySegmentEx_Valid_Play(SegWaveShort,All)",  "Valid", Performance8_PlaySegmentEx_Valid_Play,           (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegWaveShort.sgt",      0xFFFFFFFF},
     {"Performance8_PlaySegmentEx_Valid_Play(SegWaveStreaming10s,All)","Valid", Performance8_PlaySegmentEx_Valid_Play,      (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegWaveStreaming10s.sgt",0xFFFFFFFF},
     {"Performance8_PlaySegmentEx_Valid_Play(WaveLoopForever,5)", "Valid", Performance8_PlaySegmentEx_Valid_Play,           (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/WaveLoopForever.wav",     10},           //10s
     {"Performance8_PlaySegmentEx_Valid_Play(WaveShort,All)",     "Valid", Performance8_PlaySegmentEx_Valid_Play,           (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/WaveShort.wav",         0xFFFFFFFF},  //Play all the way.
     {"Performance8_PlaySegmentEx_Valid_Play(WaveStreaming10s,All)","Valid", Performance8_PlaySegmentEx_Valid_Play,         (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/WaveStreaming10s.wav",  0xFFFFFFFF},  //Play all the way.
//    {"Performance8_PlaySegmentEx_Valid_Play(16MB_OneShot,5s)",   "Valid", Performance8_PlaySegmentEx_Valid_Play,          (DWORD)"WAV/16MB_OneShot.wav",                          10},           //5s
//     {"Performance8_PlaySegmentEx_Valid_Play(16MB_OneShot,VerifyLoop)",   "Valid", Performance8_PlaySegmentEx_Valid_Play, (DWORD)"WAV/16MB_OneShot.wav", 120},           //2 minutes
     {"Performance8_PlaySegmentEx_Valid_Play(GrayedOutInstrument,5)","Valid", Performance8_PlaySegmentEx_Valid_Play,        (DWORD)"DMusic/DMTest1/loader/Inv_GMRefs/GrayedOutInstrument.sgt", 5}, 
     {"Performance8_PlaySegmentEx_Valid_Play(Timing01,All)","Valid", Performance8_PlaySegmentEx_Valid_Play,                 (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/Timing/Timing.sgt", 0xFFFFFFFF}, 
     {"Performance8_PlaySegmentEx_Valid_Play(PB01,All)","Valid", Performance8_PlaySegmentEx_Valid_Play,                     (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PB01/PB01.sgt", 0xFFFFFFFF}, 
     {"Performance8_PlaySegmentEx_Valid_Play(Fur,5s)","Valid", Performance8_PlaySegmentEx_Valid_Play,                       (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/Fur/Fur.sgt", 5}, 
     {"Performance8_PlaySegmentEx_Valid_Play(MultipleNotes_SamePChannel,All)","Valid", Performance8_PlaySegmentEx_Valid_Play,(DWORD)"DMusic/DMTest1/Perf8/PlaySeg/MultipleNotes_SamePChannel/MultipleNotes_SamePChannel.sgt", 0xFFFFFFFF}, 
     {"Performance8_PlaySegmentEx_Valid_Play(MNSP_Decay2.sgt,All)","Valid", Performance8_PlaySegmentEx_Valid_Play,          (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/MultipleNotes_SamePChannel/MNSP_Decay2.sgt", 0xFFFFFFFF}, 
     {"Performance8_PlaySegmentEx_Valid_Play(Walk.sgt,10s)","Valid", Performance8_PlaySegmentEx_Valid_Play,                 (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/Walk/Walk.sgt", 10}, 
     {"Performance8_PlaySegmentEx_Valid_Play(Harp.sgt,Long)","Valid", Performance8_PlaySegmentEx_Valid_Play,                (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/Harp/Harp.sgt", 30}, 
     {"Performance8_PlaySegmentEx_Valid_Play(Ambience.sgt,20s)","Valid", Performance8_PlaySegmentEx_Valid_Play,             (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/Other/Ambience.sgt", 20}, 
     {"Performance8_PlaySegmentEx_Valid_Play(MusicReverb.sgt,All)","Valid", Performance8_PlaySegmentEx_Valid_Play,          (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/MusicReverb/MusicReverb.sgt", 0xFFFFFFFF}, 
     {"Performance8_PlaySegmentEx_Valid_Play(PCMandADPCM,All)","Valid", Performance8_PlaySegmentEx_Valid_Play,              (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PCMandADPCM/PCMandADPCM.sgt", 27},
     {"Performance8_PlaySegmentEx_Valid_Play(16Streams,All)",  "Valid", Performance8_PlaySegmentEx_Valid_Play,              (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/Streams/16Streams.sgt", 0xFFFFFFFF},
     
     //BUGBUG: Where did this come from?  Why is there no media for it?
     //{"Performance8_PlaySegmentEx_Valid_Play(32Streams,All)",  "Valid", Performance8_PlaySegmentEx_Valid_Play,              (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/Streams/32Streams.sgt", 0xFFFFFFFF},

     {"Performance8_PlaySegment_StartTime_Start(10Ticks)","Valid", Performance8_PlaySegment_StartTime_Start, 10, 0},
     {"Performance8_PlaySegment_StartTime_Start(4Notes)", "Valid", Performance8_PlaySegment_StartTime_Start, DMUS_PPQ * 4, 0},
     {"Performance8_PlaySegment_StartTime_Start(100ms)",  "Valid", Performance8_PlaySegment_StartTime_Start, 0, 1000000},
     {"Performance8_PlaySegment_StartTime_Start(2s)",     "Valid", Performance8_PlaySegment_StartTime_Start, 0, 20000000},
     {"Performance8_PlaySegment_StartTime_Transition(10Ticks)","Valid", Performance8_PlaySegment_StartTime_Transition, 10, 0},
     {"Performance8_PlaySegment_StartTime_Transition(4Notes)", "Valid", Performance8_PlaySegment_StartTime_Transition, DMUS_PPQ * 4, 0},
     {"Performance8_PlaySegment_StartTime_Transition(100ms)",  "Valid", Performance8_PlaySegment_StartTime_Transition, 0, 1000000},
     {"Performance8_PlaySegment_StartTime_Transition(2s)",     "Valid", Performance8_PlaySegment_StartTime_Transition, 0, 20000000},


     {"Performance8_PlaySegment_TransitionType(DMUS_SEGF_QUEUE)",  "Valid", Performance8_PlaySegment_TransitionType, DMUS_SEGF_QUEUE, 0},
     {"Performance8_PlaySegment_TransitionType(DMUS_SEGF_GRID)",   "Valid", Performance8_PlaySegment_TransitionType, DMUS_SEGF_GRID, 0},
     {"Performance8_PlaySegment_TransitionType(DMUS_SEGF_BEAT)",   "Valid", Performance8_PlaySegment_TransitionType, DMUS_SEGF_BEAT, 0},
     {"Performance8_PlaySegment_TransitionType(DMUS_SEGF_MEASURE)","Valid", Performance8_PlaySegment_TransitionType, DMUS_SEGF_MEASURE, 0},
     {"Performance8_PlaySegment_TransitionType(DMUS_SEGF_DEFAULT)","Valid", Performance8_PlaySegment_TransitionType, DMUS_SEGF_DEFAULT, 0},

     {"Performance8_PlaySegment_TransPreAlign(DMUS_SEGF_ALIGN|DMUS_SEGF_VALID_START_BEAT)",   "Valid", Performance8_PlaySegment_TransPreAlign, DMUS_SEGF_ALIGN | DMUS_SEGF_VALID_START_BEAT, 0},
     {"Performance8_PlaySegment_TransPreAlign(DMUS_SEGF_ALIGN|DMUS_SEGF_VALID_START_GRID)",   "Valid", Performance8_PlaySegment_TransPreAlign, DMUS_SEGF_ALIGN | DMUS_SEGF_VALID_START_GRID, 0},
     {"Performance8_PlaySegment_TransPreAlign(DMUS_SEGF_ALIGN|DMUS_SEGF_VALID_START_MEASURE)","Valid", Performance8_PlaySegment_TransPreAlign, DMUS_SEGF_ALIGN | DMUS_SEGF_VALID_START_MEASURE, 0},
     {"Performance8_PlaySegment_TransPreAlign(DMUS_SEGF_ALIGN|DMUS_SEGF_VALID_START_TICK)",   "Valid", Performance8_PlaySegment_TransPreAlign, DMUS_SEGF_ALIGN | DMUS_SEGF_VALID_START_TICK, 0},


     {"Performance8_PlaySegmentEx_Valid_SetFrequency(LoopingWave,10s)","Valid", Performance8_PlaySegmentEx_Valid_SetFrequency, (DWORD)"WAV/LoopForever.wav", 10},    
     {"Performance8_PlaySegmentEx_Valid_SetFrequency(Default,All)","Valid", Performance8_PlaySegmentEx_Valid_SetFrequency, (DWORD)"SGT/test.sgt", 0xFFFFFFFF},    


     {"Performance8_PlaySegmentEx_Valid_Play(Sine,All)","Valid", Performance8_PlaySegmentEx_Segment_WaitFor10Seconds, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/Glitch/PCM_LP_11_1_Short.wav", 0xFFFFFFFF},
     {"Performance8_PlaySegmentEx_Segment_WaitForEnd(Streaming10s.wav,0)",                    "Valid", Performance8_PlaySegmentEx_Segment_WaitForEnd, (DWORD)"WAV/Streaming10s.wav", 0},
     {"Performance8_PlaySegmentEx_Segment_WaitForEnd(Streaming10s.wav,1)",                    "Valid", Performance8_PlaySegmentEx_Segment_WaitForEnd, (DWORD)"WAV/Streaming10s.wav", 1},
     {"Performance8_PlaySegmentEx_Segment_WaitForEnd(ShortWave.wav,0)",                       "Valid", Performance8_PlaySegmentEx_Segment_WaitForEnd, (DWORD)"WAV/ShortWave.wav"   , 0},
     {"Performance8_PlaySegmentEx_Segment_WaitForEnd(ShortWave.wav,1)",                       "Valid", Performance8_PlaySegmentEx_Segment_WaitForEnd, (DWORD)"WAV/ShortWave.wav"   , 1},
     {"Performance8_PlaySegmentEx_Segment_WaitFor10Seconds(Loop3Times.wav,0)","Valid", Performance8_PlaySegmentEx_Segment_WaitFor10Seconds,(DWORD)"WAV/Loop3Times.wav"  , 0},
     {"Performance8_PlaySegmentEx_Segment_WaitFor10Seconds(Loop3Times.wav,1)","Valid", Performance8_PlaySegmentEx_Segment_WaitFor10Seconds,(DWORD)"WAV/Loop3Times.wav"  , 1},
     {"Performance8_PlaySegmentEx_Segment_WaitFor10Seconds(LoopForever.wav,0)","Valid", Performance8_PlaySegmentEx_Segment_WaitFor10Seconds,(DWORD)"WAV/LoopForever.wav" , 0},
     {"Performance8_PlaySegmentEx_Segment_WaitFor10Seconds(LoopForever.wav,1)","Valid", Performance8_PlaySegmentEx_Segment_WaitFor10Seconds,(DWORD)"WAV/LoopForever.wav" , 1},


     {"Performance8_PlaySegmentEx_Valid_PlayBeforeDoWork(Cliff01,All)","Valid", Performance8_PlaySegmentEx_Valid_PlayBeforeDoWork, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/Cliff01/Control.sgt", 0xFFFFFFFF}, //Play entire segment.
     {"Performance8_PlaySegmentEx_Valid_DelayDoWork(Timing,500)","Valid", Performance8_PlaySegmentEx_Valid_DelayDoWork, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/Timing/Timing.sgt", 1500}, //Play entire segment.

     {"Performance8_PlaySegment_Latency",                         "Valid", Performance8_PlaySegment_Latency, 0,0}, 
     {"Performance8_PlaySegment_Latency_Interactive",             "Valid", Performance8_PlaySegment_Latency_Interactive, 0,0}, 
     {"Performance8_PlaySegmentEx_QuickStartStop_ADPCM",          "Valid", Performance8_PlaySegmentEx_QuickStartStop_ADPCM, 0,0}, 
     {"Performance8_PlaySegmentEx_QuickStartStop_WaveFiles",      "Valid", Performance8_PlaySegmentEx_QuickStartStop_WaveFiles, 0,0}, 
     {"Performance8_PlaySegmentEx_QuickStartStop_WaveSegments",   "Valid", Performance8_PlaySegmentEx_QuickStartStop_WaveSegments, 0,0}, 
     {"Performance8_PlaySegmentEx_QuickStartStopOneFile",         "Valid", Performance8_PlaySegmentEx_QuickStartStopOneFile, 0,0},
     {"Performance8_PlaySegmentEx_RelativeVolume",                "Valid", Performance8_PlaySegmentEx_RelativeVolume, 0,0}, 
     {"Performance8_PlaySegmentEx_CompareWithDSound(ADPCM_LP_11_1.wav)",           "Valid", Performance8_PlaySegmentEx_CompareWithDSound, DMUS_APATH_SHARED_STEREO, (DWORD)"WAV/ADPCM_LP_11_1.wav"}, 
     {"Performance8_PlaySegmentEx_CompareWithDSound(PCM_LP_11_1.wav)",             "Valid", Performance8_PlaySegmentEx_CompareWithDSound, DMUS_APATH_SHARED_STEREO, (DWORD)"WAV/PCM_LP_11_1.wav"}, 
     {"Performance8_PlaySegmentEx_CompareWithDSound(Glitch_Sine.wav)",         "Valid", Performance8_PlaySegmentEx_CompareWithDSound, DMUS_APATH_SHARED_STEREO, (DWORD)"WAV/Glitch_Sine.wav"}, 
     {"Performance8_PlaySegmentEx_CompareWithDSound(Glitch_Silence01s.wav)",         "Valid", Performance8_PlaySegmentEx_CompareWithDSound, DMUS_APATH_SHARED_STEREO, (DWORD)"WAV/Glitch_Silence01s.wav"}, 
     {"Performance8_PlaySegmentEx_CompareWithDSound(Glitch_SineSilence.wav)",        "Valid", Performance8_PlaySegmentEx_CompareWithDSound, DMUS_APATH_SHARED_STEREO, (DWORD)"WAV/Glitch_SineSilence.wav"}, 
     {"Performance8_PlaySegmentEx_CompareWithDSound(Glitch_SineSilenceChopped.wav)", "Valid", Performance8_PlaySegmentEx_CompareWithDSound, DMUS_APATH_SHARED_STEREO, (DWORD)"WAV/Glitch_SineSilenceChopped.wav"}, 

     {"Performance8_PlaySegmentEx_Timing(Seg1)",                        "Valid", Performance8_PlaySegmentEx_Timing, (DWORD)"DMusic/DMTest1/script/Play02Trans/seg1.sgt", 12800},
     {"Performance8_PlaySegmentEx_Timing(Seg2)",                        "Valid", Performance8_PlaySegmentEx_Timing, (DWORD)"DMusic/DMTest1/script/Play02Trans/seg2.sgt", 12800},
     {"Performance8_PlaySegmentEx_Timing(10sWave)",                     "Valid", Performance8_PlaySegmentEx_Timing, (DWORD)"WAV/Streaming10s.wav", 10000},

     {"Performance8_PlaySegmentEx_Leak",                                "Valid", Performance8_PlaySegmentEx_Leak, (DWORD)"WAV/LoopForever.wav", NULL},

     #define SEGMENT 0
     #define WAVE 1
     {"Performance8_PlaySegmentEx_ClockTime_Looping_SEGMENT_0_3_1r",  "Valid", Performance8_PlaySegmentEx_ClockTime_Looping, (DWORD)"0_3_1r", SEGMENT},
     {"Performance8_PlaySegmentEx_ClockTime_Looping_SEGMENT_0_3_2r",  "Valid", Performance8_PlaySegmentEx_ClockTime_Looping, (DWORD)"0_3_2r", SEGMENT},
     {"Performance8_PlaySegmentEx_ClockTime_Looping_SEGMENT_0_6_1r",  "Valid", Performance8_PlaySegmentEx_ClockTime_Looping, (DWORD)"0_6_1r", SEGMENT},
     {"Performance8_PlaySegmentEx_ClockTime_Looping_SEGMENT_0_6_2r",  "Valid", Performance8_PlaySegmentEx_ClockTime_Looping, (DWORD)"0_6_2r", SEGMENT},
     {"Performance8_PlaySegmentEx_ClockTime_Looping_SEGMENT_1_5_1r",  "Valid", Performance8_PlaySegmentEx_ClockTime_Looping, (DWORD)"1_5_1r", SEGMENT},
     {"Performance8_PlaySegmentEx_ClockTime_Looping_SEGMENT_1_5_2r",  "Valid", Performance8_PlaySegmentEx_ClockTime_Looping, (DWORD)"1_5_2r", SEGMENT},
     {"Performance8_PlaySegmentEx_ClockTime_Looping_SEGMENT_3_6_1r",  "Valid", Performance8_PlaySegmentEx_ClockTime_Looping, (DWORD)"3_6_1r", SEGMENT},
     {"Performance8_PlaySegmentEx_ClockTime_Looping_SEGMENT_3_6_2r",  "Valid", Performance8_PlaySegmentEx_ClockTime_Looping, (DWORD)"3_6_2r", SEGMENT},

//    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB)","Valid", Performance8_PlaySegmentEx_AudioPath, DMUS_APATH_SHARED_STEREOPLUSREVERB, 0},

#define ONESHOT     0
#define STREAMING   1
#define MUSIC       2
#define MULTIPAN    3
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1,1,OS)",       "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_5DOT1,         ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1,2,OS)",       "Valid", Performance8_PlaySegmentEx_AudioPath_2, DMUS_APATH_MIXBIN_5DOT1,         ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1,6,OS)",       "Valid", Performance8_PlaySegmentEx_AudioPath_6, DMUS_APATH_MIXBIN_5DOT1,         ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_ENV,1,OS)",   "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_5DOT1_ENV,     ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_MUSIC,1,OS)", "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_5DOT1_MUSIC,   ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_MUSIC,2,OS)", "Valid", Performance8_PlaySegmentEx_AudioPath_2, DMUS_APATH_MIXBIN_5DOT1_MUSIC,   ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_MUSIC,4,OS)", "Valid", Performance8_PlaySegmentEx_AudioPath_4, DMUS_APATH_MIXBIN_5DOT1_MUSIC,   ONESHOT},

    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1,1,ST)",       "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_5DOT1,         STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1,2,ST)",       "Valid", Performance8_PlaySegmentEx_AudioPath_2, DMUS_APATH_MIXBIN_5DOT1,         STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1,6,ST)",       "Valid", Performance8_PlaySegmentEx_AudioPath_6, DMUS_APATH_MIXBIN_5DOT1,         STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_ENV,1,ST)",   "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_5DOT1_ENV,     STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_MUSIC,1,ST)", "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_5DOT1_MUSIC,   STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_MUSIC,2,ST)", "Valid", Performance8_PlaySegmentEx_AudioPath_2, DMUS_APATH_MIXBIN_5DOT1_MUSIC,   STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_MUSIC,4,ST)", "Valid", Performance8_PlaySegmentEx_AudioPath_4, DMUS_APATH_MIXBIN_5DOT1_MUSIC,   STREAMING},

    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1,1,MU)",       "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_5DOT1,         MUSIC},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_ENV,1,MU)",   "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_5DOT1_ENV,     MUSIC},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_MUSIC,1,MU)", "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_5DOT1_MUSIC,   MUSIC},

    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1,1,MP)",       "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_5DOT1,         MULTIPAN},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_ENV,1,MP)",   "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_5DOT1_ENV,     MULTIPAN},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_MUSIC,1,MP)", "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_5DOT1_MUSIC,   MULTIPAN},

    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD,1,OS)",        "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_QUAD,          ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD,2,OS)",        "Valid", Performance8_PlaySegmentEx_AudioPath_2, DMUS_APATH_MIXBIN_QUAD,          ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD,4,OS)",        "Valid", Performance8_PlaySegmentEx_AudioPath_4, DMUS_APATH_MIXBIN_QUAD,          ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_ENV,1,OS)",    "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_QUAD_ENV,      ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_MUSIC,1,OS)",  "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_QUAD_MUSIC,    ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_MUSIC,2,OS)",  "Valid", Performance8_PlaySegmentEx_AudioPath_2, DMUS_APATH_MIXBIN_QUAD_MUSIC,    ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_MUSIC,4,OS)",  "Valid", Performance8_PlaySegmentEx_AudioPath_4, DMUS_APATH_MIXBIN_QUAD_MUSIC,    ONESHOT},

    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD,1,ST)",        "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_QUAD,          STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD,2,ST)",        "Valid", Performance8_PlaySegmentEx_AudioPath_2, DMUS_APATH_MIXBIN_QUAD,          STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD,4,ST)",        "Valid", Performance8_PlaySegmentEx_AudioPath_4, DMUS_APATH_MIXBIN_QUAD,          STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_ENV,1,ST)",    "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_QUAD_ENV,      STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_MUSIC,1,ST)",  "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_QUAD_MUSIC,    STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_MUSIC,2,ST)",  "Valid", Performance8_PlaySegmentEx_AudioPath_2, DMUS_APATH_MIXBIN_QUAD_MUSIC,    STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_MUSIC,4,ST)",  "Valid", Performance8_PlaySegmentEx_AudioPath_4, DMUS_APATH_MIXBIN_QUAD_MUSIC,    STREAMING},

    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD,1,MU)",        "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_QUAD,          MUSIC},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_ENV,1,MU)",    "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_QUAD_ENV,      MUSIC},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_MUSIC,1,MU)",  "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_QUAD_MUSIC,    MUSIC},

    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD,1,MP)",        "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_QUAD,          MULTIPAN},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_ENV,1,MP)",    "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_QUAD_ENV,      MULTIPAN},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_MUSIC,1,MP)",  "Valid", Performance8_PlaySegmentEx_AudioPath_1, DMUS_APATH_MIXBIN_QUAD_MUSIC,    MULTIPAN},

#undef STREAMING
#undef ONESHOT
#undef MUSIC
#undef PANNING 

    {"Performance8_PlaySegmentEx_MunchPerf",                                     "Valid", Performance8_PlaySegmentEx_MunchPerf,   0, 0},
    {"Performance8_PlaySegmentEx_AntzLooping",                                   "Valid", Performance8_PlaySegmentEx_AntzLooping, 0, 0},

/*THESE WILL FAIL!!
     {"Performance8_PlaySegmentEx_ClockTime_Looping_WAVE_0_3_1r",     "Valid", Performance8_PlaySegmentEx_ClockTime_Looping, (DWORD)"0_3_1r", WAVE},
     {"Performance8_PlaySegmentEx_ClockTime_Looping_WAVE_0_6_1r",     "Valid", Performance8_PlaySegmentEx_ClockTime_Looping, (DWORD)"0_6_1r", WAVE},
     {"Performance8_PlaySegmentEx_ClockTime_Looping_WAVE_1_5_1r",     "Valid", Performance8_PlaySegmentEx_ClockTime_Looping, (DWORD)"1_5_1r", WAVE},
     {"Performance8_PlaySegmentEx_ClockTime_Looping_WAVE_3_6_1r",     "Valid", Performance8_PlaySegmentEx_ClockTime_Looping, (DWORD)"3_6_1r", WAVE},
*/
     #undef SEGMENT
     #undef WAVE
    
     {"Script_CallRoutine_Valid_TransitionTempo",                 "Valid", Script_CallRoutine_Valid_TransitionTempo, 0,0}, 

     {"Performance8_ReferenceToMusicTime_TempoChange",      "Valid", Performance8_ReferenceToMusicTime_TempoChange, 0, 0},
     //{"Performance8_RemoveNotificationType_Valid",  "Valid", Performance8_RemoveNotificationType_Valid, 0, 0}, 
     {"Performance8_PMsg_Valid_Tempos",              "Valid", Performance8_PMsg_Valid_Tempos, 0, 0},                          
     {"Performance8_PMsg_Valid_Transpose",           "Valid", Performance8_PMsg_Valid_Transpose, 0, 0},                          
     {"Performance8_PMsg_Valid_PitchBend",           "Valid", Performance8_PMsg_Valid_PitchBend, 0, 0},                          
     {"Performance8_PMsg_Valid_Transpose_ChangeSong","Valid", Performance8_PMsg_Valid_Transpose_ChangeSong, 0, 0},                          
     {"Performance8_PMsg_Crossfade(Wave Segments)",                 "Valid", Performance8_PMsg_Crossfade, (DWORD)"WAV/LoopForever.wav", (DWORD)"WAV/08_08_2.wav"},
     {"Performance8_PMsg_Crossfade(DLS Segments)",                  "Valid", Performance8_PMsg_Crossfade, (DWORD)"DMusic/DMTest1/test.sgt", (DWORD)"DMusic/DMTest1/test2.sgt"},  
     {"Performance8_PMsg_Crossfade(Complex DLS Segments)",          "Valid", Performance8_PMsg_Crossfade, (DWORD)"DMusic/DMTest1/script/Play02Trans/seg1.sgt", (DWORD)"DMusic/DMTest1/script/Play02Trans/seg2.sgt"},  

     {"Performance8_SetGlobalParam_Valid",            "Valid", Performance8_SetGlobalParam_Valid, 0, 0},                    

     {"Performance8_StopEx_NoSegments","Valid", Performance8_StopEx_NoSegments, 0, 0},                            
     {"Performance8_StopEx_1Segment_DLS",    "Valid", Performance8_StopEx_1Segment_DLS,   0, 0},                            
     {"Performance8_StopEx_1Segment_Wave",   "Valid", Performance8_StopEx_1Segment_Wave,   0, 0},                            
     {"Performance8_StopEx_2Segments_DLS(1Path,1Seg)",  "Valid", Performance8_StopEx_2Segments_DLS,  TRUE,  TRUE },
     {"Performance8_StopEx_2Segments_DLS(1Path,2Seg)",  "Valid", Performance8_StopEx_2Segments_DLS,  TRUE,  FALSE},                            
     {"Performance8_StopEx_2Segments_DLS(2Path,1Seg)",  "Valid", Performance8_StopEx_2Segments_DLS,  FALSE, TRUE},                             
     {"Performance8_StopEx_2Segments_DLS(2Path,2Seg)",  "Valid", Performance8_StopEx_2Segments_DLS,  FALSE, FALSE},                            
     {"Performance8_StopEx_2Segments_Wave(1Path,1Seg)", "Valid", Performance8_StopEx_2Segments_Wave,  TRUE,  TRUE },
     {"Performance8_StopEx_2Segments_Wave(1Path,2Seg)", "Valid", Performance8_StopEx_2Segments_Wave,  TRUE,  FALSE},                            
     {"Performance8_StopEx_2Segments_Wave(2Path,1Seg)", "Valid", Performance8_StopEx_2Segments_Wave,  FALSE, TRUE},                             
     {"Performance8_StopEx_2Segments_Wave(2Path,2Seg)", "Valid", Performance8_StopEx_2Segments_Wave,  FALSE, FALSE},                            
     {"Performance8_StopEx_TimePast",    "Valid", Performance8_StopEx_TimePast,    0, 0},                            
     {"Performance8_StopEx_TimeFuture",  "Valid", Performance8_StopEx_TimeFuture,  0, 0},
     {"Performance8_StopEx_NoDoWork",    "Valid", Performance8_StopEx_NoDoWork,    0, 0},
     


    // Script Tests
    //-------------------
     {"Script_Init_Valid",                            "Valid",  Script_Init_Valid             , 0, 0},                      
     {"Script_CallRoutine_Valid",                     "Valid",  Script_CallRoutine_Valid      , 0, 0},                      
     
     //Fails by design; bad script.
     //{"Script_CallRoutine_Valid_PlayOneScript(Area51)","Valid",  Script_CallRoutine_Valid_PlayOneScript, (DWORD)"DMusic/DMTest1/script/Bug5337/Area51_005.spt", (DWORD)"ExitAlienRoom"},                      

     {"Script_CallRoutine_Valid_PlayOneScript(Embed,1)",       "Valid",  Script_CallRoutine_Valid_PlayOneScript, (DWORD)"DMusic/DMTest1/script/Embedding/Script1.spt", (DWORD)"PlayMe"},                      
     {"Script_CallRoutine_Valid_PlayOneScript(Embed,2)",       "Valid",  Script_CallRoutine_Valid_PlayOneScript, (DWORD)"DMusic/DMTest1/script/Embedding/Script2.spt", (DWORD)"PlayMe"},                      
     {"Script_CallRoutine_Valid_PlayOneScript(Embed,3)",       "Valid",  Script_CallRoutine_Valid_PlayOneScript, (DWORD)"DMusic/DMTest1/script/Embedding/Script3.spt", (DWORD)"PlayMe"},                      
     {"Script_CallRoutine_Valid_PlayOneScript(Bug6511)", "Valid",  Script_CallRoutine_Valid_PlayOneScript, (DWORD)"DMusic/DMTest1/script/Other/Script1.spt", (DWORD)"PlayMeSecondary"}, 
     {"Script_CallRoutine_Valid_PlayOneScript(Bug6496)", "Valid",  Script_CallRoutine_Valid_PlayOneScript, (DWORD)"DMusic/DMTest1/script/Other/Bug6496/Bug6496.spt", (DWORD)"PlayMe"}, 
     {"Script_CallRoutine_Valid_PlayOneScript(Bug6497)", "Valid",  Script_CallRoutine_Valid_PlayOneScript, (DWORD)"DMusic/DMTest1/script/Other/ScriptA.spt", (DWORD)"DoNothing"}, 

     {"Script_SetVariableNumber_Valid",               "Valid",  Script_SetVariableNumber_Valid, 0, 0},                      
     {"Script_GetVariableNumber_Valid",               "Valid",  Script_GetVariableNumber_Valid, 0, 0},                      
     {"Script_SetvariableObject_Valid",               "Valid",  Script_SetVariableObject_Valid, 0, 0},                      
     {"Script_GetVariableObject_Valid",               "Valid",  Script_GetVariableObject_Valid, 0, 0},                      
     {"Script_SetVariableString_Valid",               "Valid",  Script_SetVariableString_Valid, 0, 0},                      
     {"Script_GetVariableString_Valid",               "Valid",  Script_GetVariableString_Valid, 0, 0},                      

     {"Script_AudioPath_SetGetVolume",                "Valid",  Script_CallRoutine_Valid_TestScript,   0, S_OK},
     {"Script_AudioPath_SetVolumeListeningTest",      "Valid",  Script_CallRoutine_Valid_TestScript,   1, S_OK},
     {"Script_AudioPathConfig_Create",                "Valid",  Script_CallRoutine_Valid_TestScript,  10, S_OK},
     {"Script_AudioPathConfig_Load",                  "Valid",  Script_CallRoutine_Valid_TestScript,  11, S_OK},
     {"Script_Segment_PlayStop",                      "Valid",  Script_CallRoutine_Valid_TestScript,  20, S_OK},
     {"Script_Performance_SetMasterGrooveLevel",      "Valid",  Script_CallRoutine_Valid_TestScript,  30, S_OK},
     {"Script_Performance_SetMasterTempo(DLS)",       "Valid",  Script_CallRoutine_Valid_TestScript,  32, S_OK},
     {"Script_Performance_SetMasterTempo(WAVE)",      "Valid",  Script_CallRoutine_Valid_TestScript,  33, S_OK},
     {"Script_Performance_GetMasterTempo",            "Valid",  Script_CallRoutine_Valid_TestScript,  34, S_OK},
     {"Script_Performance_SetMasterTranspose",        "Valid",  Script_CallRoutine_Valid_TestScript,  35, S_OK},
     {"Script_Performance_GetMasterTranspose",        "Valid",  Script_CallRoutine_Valid_TestScript,  36, S_OK},
     {"Script_Performance_Rand",                      "Valid",  Script_CallRoutine_Valid_TestScript,  37, S_OK},
     {"Script_PlayingSegment_IsPlaying(VerifyTrue)",  "Valid",  Script_CallRoutine_Valid_TestScript,  50, S_OK},
     {"Script_PlayingSegment_IsPlaying(VerifyFalse)", "Valid",  Script_CallRoutine_Valid_TestScript,  51, S_OK},
     {"Script_PlayingSegment_Stop",                   "Valid",  Script_CallRoutine_Valid_TestScript,  52, S_OK},
     {"Script_AutoLoadUnchecked_SegmentNoLoad",       "Valid",  Script_CallRoutine_Valid_TestScript, 100, DMUS_E_SCRIPT_ERROR_IN_SCRIPT},
     {"Script_AutoLoadUnchecked_SegmentLoad",         "Valid",  Script_CallRoutine_Valid_TestScript, 101, S_OK},
  //   {"Script_AutoLoadUnchecked_AudioPathNoLoad",     "Valid",  Script_CallRoutine_Valid_TestScript, 102, DMUS_E_SCRIPT_ERROR_IN_SCRIPT},
//     {"Script_AutoLoadUnchecked_AudioPathLoad",       "Valid",  Script_CallRoutine_Valid_TestScript, 103, S_OK}, //Not valid yet.
     {"Script_AutoLoadChecked_SegmentLoad",           "Valid",  Script_CallRoutine_Valid_TestScript, 110, S_OK},
//     {"Script_AutoLoadChecked_AudioPathLoad",         "Valid",  Script_CallRoutine_Valid_TestScript, 111, S_OK},

     {"Script_AutoDownloadUnchecked_NoDownload",     "Valid",  Script_CallRoutine_Valid_TestScript, 120, S_OK},
     {"Script_AutoDownloadUnchecked_Download",       "Valid",  Script_CallRoutine_Valid_TestScript, 121, S_OK},
     {"Script_AutoDownloadUnchecked_Unload",         "Valid",  Script_CallRoutine_Valid_TestScript, 122, S_OK},

//     {"Script_EmbeddedContent",                      "Valid",  Script_CallRoutine_Valid_TestScript, 150, S_OK},  //Not valid test.
     {"Script_Recompose",                            "Valid",  Script_CallRoutine_Valid_TestScript, 160, S_OK},

     // ************************************************************************************************************************
     // SCRIPT TRANSITION TESTS
     // ************************************************************************************************************************    
     
     //USE SEGMENT'S DEFAULT TRANSITION BOUNDARY
     {"Script_Transition_PlayIntro(Startup)",        "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 500, S_OK},
     {"Script_Transition_PlayIntro(SameSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 501, S_OK},
     {"Script_Transition_PlayIntro(DiffSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 502, S_OK},
     {"Script_Transition_PlayFill (Startup)",        "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 510, S_OK},
     {"Script_Transition_PlayFill (SameSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 511, S_OK},
     {"Script_Transition_PlayFill (DiffSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 512, S_OK},
     {"Script_Transition_PlayBreak(Startup)",        "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 520, S_OK},
     {"Script_Transition_PlayBreak(SameSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 521, S_OK},
     {"Script_Transition_PlayBreak(DiffSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 522, S_OK},
     {"Script_Transition_PlayEnd  (Startup)",        "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 530, S_OK},
     {"Script_Transition_PlayEnd  (SameSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 531, S_OK},
     {"Script_Transition_PlayEnd  (DiffSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 532, S_OK},
     {"Script_Transition_PlayEndAndIntro  (Startup)","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 540, S_OK},
     {"Script_Transition_PlayEndAndIntro  (SameSeg)","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 541, S_OK},
     {"Script_Transition_PlayEndAndIntro  (DiffSeg)","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 542, S_OK},

     
     //USE ATMEASURE TRANSITION BOUNDARY
     {"Script_Transition_AtMeasure_Startup",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 600, S_OK},
     {"Script_Transition_AtMeasure_SameSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 601, S_OK},
     {"Script_Transition_AtMeasure_DiffSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 602, S_OK},
     {"Script_Transition_AtMeasure_Startup_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 605, S_OK},
     {"Script_Transition_AtMeasure_SameSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 606, S_OK},
     {"Script_Transition_AtMeasure_DiffSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 608, S_OK},
     {"Script_Transition_AtMeasure_DiffSeg_PlayBreak","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 609, S_OK},
     {"Script_Transition_AtMeasure_DiffSeg_PlayFill", "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 610, S_OK},
     {"Script_Transition_AtMeasure_DiffSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 611, S_OK},
     {"Script_Transition_AtMeasure_SameSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 612, S_OK},
     {"Script_Transition_AtMeasure_DiffSeg_PlayEnd",  "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 613, S_OK},
     {"Script_Transition_AtMeasure_End_PlayEnd",      "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 615, S_OK},

     //USE ATIMMEDIATE TRANSITION BOUNDARY
     {"Script_Transition_AtImmediate_Startup",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 620, S_OK},
     {"Script_Transition_AtImmediate_SameSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 621, S_OK},
     {"Script_Transition_AtImmediate_DiffSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 622, S_OK},
     {"Script_Transition_AtImmediate_Startup_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 625, S_OK},
     {"Script_Transition_AtImmediate_SameSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 626, S_OK},
     {"Script_Transition_AtImmediate_DiffSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 628, S_OK},
     {"Script_Transition_AtImmediate_DiffSeg_PlayBreak","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 629, S_OK},
     {"Script_Transition_AtImmediate_DiffSeg_PlayFill", "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 630, S_OK},
     {"Script_Transition_AtImmediate_DiffSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 631, S_OK},
     {"Script_Transition_AtImmediate_SameSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 632, S_OK},
     {"Script_Transition_AtImmediate_DiffSeg_PlayEnd",  "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 633, S_OK},
     {"Script_Transition_AtImmediate_End_PlayEnd",      "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 635, S_OK},

     //USE ATBEAT TRANSITION BOUNDARY
     {"Script_Transition_AtBeat_Startup",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 640, S_OK},
     {"Script_Transition_AtBeat_SameSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 641, S_OK},
     {"Script_Transition_AtBeat_DiffSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 642, S_OK},
     {"Script_Transition_AtBeat_Startup_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 645, S_OK},
     {"Script_Transition_AtBeat_SameSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 646, S_OK},
     {"Script_Transition_AtBeat_DiffSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 648, S_OK},
     {"Script_Transition_AtBeat_DiffSeg_PlayBreak","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 649, S_OK},
     {"Script_Transition_AtBeat_DiffSeg_PlayFill", "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 650, S_OK},
     {"Script_Transition_AtBeat_DiffSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 651, S_OK},
     {"Script_Transition_AtBeat_SameSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 652, S_OK},
     {"Script_Transition_AtBeat_DiffSeg_PlayEnd",  "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 653, S_OK},
     {"Script_Transition_AtBeat_End_PlayEnd",      "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 655, S_OK},

     //USE ATGRID TRANSITION BOUNDARY
     {"Script_Transition_AtGrid_Startup",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 660, S_OK},
     {"Script_Transition_AtGrid_SameSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 661, S_OK},
     {"Script_Transition_AtGrid_DiffSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 662, S_OK},
     {"Script_Transition_AtGrid_Startup_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 665, S_OK},
     {"Script_Transition_AtGrid_SameSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 666, S_OK},
     {"Script_Transition_AtGrid_DiffSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 668, S_OK},
     {"Script_Transition_AtGrid_DiffSeg_PlayBreak","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 669, S_OK},
     {"Script_Transition_AtGrid_DiffSeg_PlayFill", "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 670, S_OK},
     {"Script_Transition_AtGrid_DiffSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 671, S_OK},
     {"Script_Transition_AtGrid_SameSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 672, S_OK},
     {"Script_Transition_AtGrid_DiffSeg_PlayEnd",  "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 673, S_OK},
     {"Script_Transition_AtGrid_End_PlayEnd",      "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 675, S_OK},

     //USE ATFINISH TRANSITION BOUNDARY
     {"Script_Transition_AtFinish_Startup",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 680, S_OK},
     {"Script_Transition_AtFinish_SameSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 681, S_OK},
     {"Script_Transition_AtFinish_DiffSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 682, S_OK},
     {"Script_Transition_AtFinish_Startup_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 685, S_OK},
     {"Script_Transition_AtFinish_SameSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 686, S_OK},
     {"Script_Transition_AtFinish_DiffSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 688, S_OK},
     {"Script_Transition_AtFinish_DiffSeg_PlayBreak","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 689, S_OK},
     {"Script_Transition_AtFinish_DiffSeg_PlayFill", "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 690, S_OK},
     {"Script_Transition_AtFinish_DiffSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 691, S_OK},
     {"Script_Transition_AtFinish_SameSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 692, S_OK},
     {"Script_Transition_AtFinish_DiffSeg_PlayEnd",  "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 693, S_OK},
     {"Script_Transition_AtFinish_End_PlayEnd",      "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 695, S_OK},

     //USE AtMarker TRANSITION BOUNDARY
     {"Script_Transition_AtMarker_Startup",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 700, S_OK},
     {"Script_Transition_AtMarker_SameSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 701, S_OK},
     {"Script_Transition_AtMarker_DiffSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 702, S_OK},
     {"Script_Transition_AtMarker_Startup_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 705, S_OK},
     {"Script_Transition_AtMarker_SameSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 706, S_OK},
     {"Script_Transition_AtMarker_DiffSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 708, S_OK},
     {"Script_Transition_AtMarker_DiffSeg_PlayBreak","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 709, S_OK},
     {"Script_Transition_AtMarker_DiffSeg_PlayFill", "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 710, S_OK},
     {"Script_Transition_AtMarker_DiffSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 711, S_OK},
     {"Script_Transition_AtMarker_SameSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_Transition, 712, S_OK},
     {"Script_Transition_AtMarker_DiffSeg_PlayEnd",  "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 713, S_OK},
     {"Script_Transition_AtMarker_End_PlayEnd",      "Valid",  Script_CallRoutine_Valid_TestScript_Transition, 715, S_OK},

     
     //Verify flag boundaries override default segment boundaries on PLAY
     {"Script_SegBoundaries_Play_AtMeasure_AtImmediate_NoTrans","Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 0, S_OK},
     {"Script_SegBoundaries_Play_AtMeasure_AtImmediate_Trans","Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 1, S_OK},
     {"Script_SegBoundaries_Play_AtMeasure_AtGrid_NoTrans",     "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 2, S_OK},
     {"Script_SegBoundaries_Play_AtMeasure_AtGrid_Trans",     "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 3, S_OK},
     {"Script_SegBoundaries_Play_AtMeasure_AtBeat_NoTrans",     "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 4, S_OK},
     {"Script_SegBoundaries_Play_AtMeasure_AtBeat_Trans",     "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 5, S_OK},
     {"Script_SegBoundaries_Play_AtImmediate_AtMeasure_NoTrans","Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 6, S_OK},
     {"Script_SegBoundaries_Play_AtImmediate_AtMeasure_Trans","Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 7, S_OK},
     {"Script_SegBoundaries_Play_AtImmediate_AtFinish_NoTrans", "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 8, S_OK},
     {"Script_SegBoundaries_Play_AtImmediate_AtFinish_Trans", "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 9, S_OK},
     {"Script_SegBoundaries_Play_AtBeat_AtMeasure_NoTrans", "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 10, S_OK},
     {"Script_SegBoundaries_Play_AtBeat_AtMeasure_Trans", "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 11, S_OK},

     //Verify flag boundaries override default segment boundaries on STOP
     {"Script_SegBoundaries_Stop_AtMeasure_AtImmediate_NoTrans","Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 100, S_OK},
     {"Script_SegBoundaries_Stop_AtMeasure_AtImmediate_Trans","Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 101, S_OK},
     {"Script_SegBoundaries_Stop_AtMeasure_AtGrid_NoTrans",     "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 102, S_OK},
     {"Script_SegBoundaries_Stop_AtMeasure_AtGrid_Trans",     "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 103, S_OK},
     {"Script_SegBoundaries_Stop_AtMeasure_AtBeat_NoTrans",     "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 104, S_OK},
     {"Script_SegBoundaries_Stop_AtMeasure_AtBeat_Trans",     "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 105, S_OK},
     {"Script_SegBoundaries_Stop_AtImmediate_AtMeasure_NoTrans","Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 106, S_OK},
     {"Script_SegBoundaries_Stop_AtImmediate_AtMeasure_Trans","Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 107, S_OK},
     {"Script_SegBoundaries_Stop_AtImmediate_AtFinish_NoTrans", "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 108, S_OK},
     {"Script_SegBoundaries_Stop_AtImmediate_AtFinish_Trans", "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 109, S_OK},
     {"Script_SegBoundaries_Stop_AtBeat_AtMeasure_NoTrans", "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 110, S_OK},
     {"Script_SegBoundaries_Stop_AtBeat_AtMeasure_Trans", "Valid",  Script_CallRoutine_Valid_TestScript_SegBoundaries, 111, S_OK},


     //Script TRANSITION tests (with simple wave playback instead of cool music)!!
//     {"Script_TransSimple_PlayIntro(Startup)",        "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 500, S_OK},
//     {"Script_TransSimple_PlayIntro(SameSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 501, S_OK},
//     {"Script_TransSimple_PlayIntro(DiffSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 502, S_OK},
//     {"Script_TransSimple_PlayFill (Startup)",        "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 510, S_OK},
//     {"Script_TransSimple_PlayFill (SameSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 511, S_OK},
//     {"Script_TransSimple_PlayFill (DiffSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 512, S_OK},
//     {"Script_TransSimple_PlayBreak(Startup)",        "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 520, S_OK},
//     {"Script_TransSimple_PlayBreak(SameSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 521, S_OK},
//     {"Script_TransSimple_PlayBreak(DiffSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 522, S_OK},
//     {"Script_TransSimple_PlayEnd  (Startup)",        "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 530, S_OK},
//     {"Script_TransSimple_PlayEnd  (SameSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 531, S_OK},
//     {"Script_TransSimple_PlayEnd  (DiffSeg)",        "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 532, S_OK},
//     {"Script_TransSimple_PlayEndAndIntro  (Startup)","Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 540, S_OK},
//     {"Script_TransSimple_PlayEndAndIntro  (SameSeg)","Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 541, S_OK},
//     {"Script_TransSimple_PlayEndAndIntro  (DiffSeg)","Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 542, S_OK},

//     {"Script_TransSimple_AtMeasure_Startup",          "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 600, S_OK},
//     {"Script_TransSimple_AtMeasure_SameSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 601, S_OK},
//     {"Script_TransSimple_AtMeasure_DiffSeg",          "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 602, S_OK},
//     {"Script_TransSimple_AtMeasure_Startup_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 605, S_OK},
//     {"Script_TransSimple_AtMeasure_SameSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 606, S_OK},
//     {"Script_TransSimple_AtMeasure_DiffSeg_PlayIntro","Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 608, S_OK},
//     {"Script_TransSimple_AtMeasure_DiffSeg_PlayBreak","Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 609, S_OK},
//     {"Script_TransSimple_AtMeasure_DiffSeg_PlayFill", "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 610, S_OK},
//     {"Script_TransSimple_AtMeasure_DiffSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 611, S_OK},
//     {"Script_TransSimple_AtMeasure_SameSeg_PlayEndAndIntro","Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 612, S_OK},
//     {"Script_TransSimple_AtMeasure_DiffSeg_PlayEnd",  "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 613, S_OK},
//     {"Script_TransSimple_AtMeasure_End_PlayEnd",      "Valid",  Script_CallRoutine_Valid_TestScript_TransSimple, 615, S_OK},


    //Segment8 Tests
    //--------------
    {"Segment8_AddNotificationType_NoRestart(1,0)",       "Valid", Segment8_AddNotificationType_NoRestart, TESTNOTIFICATION_MEASUREANDBEAT,    0},  
//    {"Segment8_AddNotificationType_NoRestart(2,0)",       "Valid", Segment8_AddNotificationType_NoRestart, TESTNOTIFICATION_PERFORMANCE,       0},
    {"Segment8_AddNotificationType_NoRestart(4,0)",       "Valid", Segment8_AddNotificationType_NoRestart, TESTNOTIFICATION_SEGMENT,           0},  
//    {"Segment8_AddNotificationType_NoRestart(7,0)",       "Valid", Segment8_AddNotificationType_NoRestart, TESTNOTIFICATION_ALL,               0},

    //Verify that removals work.
    {"Segment8_AddNotificationType_NoRestart(1,1)",       "Valid", Segment8_AddNotificationType_NoRestart, TESTNOTIFICATION_MEASUREANDBEAT,    TESTNOTIFICATION_MEASUREANDBEAT},
//    {"Segment8_AddNotificationType_NoRestart(2,2)",       "Valid", Segment8_AddNotificationType_NoRestart, TESTNOTIFICATION_PERFORMANCE,       TESTNOTIFICATION_PERFORMANCE},
    {"Segment8_AddNotificationType_NoRestart(4,4)",       "Valid", Segment8_AddNotificationType_NoRestart, TESTNOTIFICATION_SEGMENT,           TESTNOTIFICATION_SEGMENT},
//    {"Segment8_AddNotificationType_NoRestart(7,7)",       "Valid", Segment8_AddNotificationType_NoRestart, TESTNOTIFICATION_ALL,               TESTNOTIFICATION_ALL},

    //Make sure that each removal.is the correct one.
//    {"Segment8_AddNotificationType_NoRestart(1,2)",       "Valid", Segment8_AddNotificationType_NoRestart, TESTNOTIFICATION_MEASUREANDBEAT,    TESTNOTIFICATION_PERFORMANCE},
//    {"Segment8_AddNotificationType_NoRestart(2,4)",       "Valid", Segment8_AddNotificationType_NoRestart, TESTNOTIFICATION_PERFORMANCE,       TESTNOTIFICATION_SEGMENT},
    {"Segment8_AddNotificationType_NoRestart(4,1)",       "Valid", Segment8_AddNotificationType_NoRestart, TESTNOTIFICATION_SEGMENT,           TESTNOTIFICATION_MEASUREANDBEAT},

    //Make sure that duplicate removals don't hurt anything
    {"Segment8_AddNotificationType_NoRestart(0,1)",       "Valid", Segment8_AddNotificationType_NoRestart, 0, TESTNOTIFICATION_MEASUREANDBEAT},
//    {"Segment8_AddNotificationType_NoRestart(0,2)",       "Valid", Segment8_AddNotificationType_NoRestart, 0, TESTNOTIFICATION_PERFORMANCE},
    {"Segment8_AddNotificationType_NoRestart(0,4)",       "Valid", Segment8_AddNotificationType_NoRestart, 0, TESTNOTIFICATION_SEGMENT},

    //Make sure all the notifications work after you restart a segment.
    {"Segment8_AddNotificationType_Restart(1,0)",       "Valid", Segment8_AddNotificationType_Restart, TESTNOTIFICATION_MEASUREANDBEAT,    0},  
//    {"Segment8_AddNotificationType_Restart(2,0)",       "Valid", Segment8_AddNotificationType_Restart, TESTNOTIFICATION_PERFORMANCE,       0},
    {"Segment8_AddNotificationType_Restart(4,0)",       "Valid", Segment8_AddNotificationType_Restart, TESTNOTIFICATION_SEGMENT,           0},  




//     {"Segment8_Compose_Valid",                       "Valid", Segment8_Compose_Valid, 0, 0},                                        //NOT A VALID TEST.
//     {"Segment8_Download_Test1_Valid",                "Valid", Segment8_Download_Test1_Valid, 0, 0},                          
//     {"Segment8_Download_Test2_Valid",                "Valid", Segment8_Download_Test2_Valid, 0, 0},                          
//     {"Segment8_Download_Test3_Valid",                "Valid", Segment8_Download_Test3_Valid, 0, 0},                          
//     {"Segment8_Download_Test4_Valid",                "Valid", Segment8_Download_Test4_Valid, 0, 0},                          
//     {"Segment8_Download_Segment8_Download_PreCache_NoCon",   "Valid", Segment8_Download_PreCache, (DWORD)"DMusic/DMTest1/Segment8/Download/Streaming01/Streaming01a.sgt", (DWORD)"DMusic/DMTest1/Segment8/Download/Streaming01/Streaming01b.sgt"},
//     {"Segment8_Download_Segment8_Download_PreCache_Con",     "Valid", Segment8_Download_PreCache, (DWORD)"DMusic/DMTest1/Segment8/Download/Streaming02/Streaming02a.sgt", (DWORD)"DMusic/DMTest1/Segment8/Download/Streaming02/Streaming02b.sgt"},
//     {"Segment8_Download_Segment8_Download_PreCache_Original","Valid", Segment8_Download_PreCache, (DWORD)"DMusic/DMTest1/Segment8/Download/Streaming04/y_shaman.sgt", (DWORD)"DMusic/DMTest1/Segment8/Download/Streaming04/ssR0001.sgt"},
//     {"Segment8_Download_PreCache_Multiple01",   "Valid", Segment8_Download_PreCache_Multiple01, 0, 0},
  //   {"Segment8_Download_PreCache_Multiple02",   "Valid", Segment8_Download_PreCache_Multiple02, 0, 0},

//     {"Segment8_Unload_Test1_Valid",                  "Valid", Segment8_Unload_Test1_Valid, 0, 0},                            
//     {"Segment8_Unload_Test2_Valid",                  "Valid", Segment8_Unload_Test2_Valid, 0, 0},                            
//     {"Segment8_Unload_Test3_Valid",                  "Valid", Segment8_Unload_Test3_Valid, 0, 0},                            
//     {"Segment8_Unload_Test4_Valid",                  "Valid", Segment8_Unload_Test4_Valid, 0, 0},                                         
     {"Segment8_SetRepeats_Test1_Valid",              "Valid", Segment8_SetRepeats_Test1_Valid, 0, 0},
     {"Segment8_SetRepeats_Test2_Valid",              "Valid", Segment8_SetRepeats_Test2_Valid, 0, 0},
     {"Segment8_SetRepeats_Test3_Valid",              "Valid", Segment8_SetRepeats_Test3_Valid, 0, 0},
     {"Segment8_SetRepeats_Test4_Valid",              "Valid", Segment8_SetRepeats_Test4_Valid, 0, 0},
     {"Segment8_SetRepeats_Test5_Valid",              "Valid", Segment8_SetRepeats_Test5_Valid, 0, 0},
     {"Segment8_SetRepeats_Test6_Valid",              "Valid", Segment8_SetRepeats_Test6_Valid, 0, 0},
     {"Segment8_GetLength_Valid",                     "Valid", Segment8_GetLength_Valid, 0, 0},                          //PASSES!
     {"Segment8_SetLength_Test1_Valid",               "Valid", Segment8_SetLength_Test1_Valid, 0, 0},
     {"Segment8_SetLength_Range(DMUS_PPQ-1)",         "Valid", Segment8_SetLength_Range, DMUS_PPQ-1, 0},
     {"Segment8_SetLength_Range(DMUS_PPQ)",           "Valid", Segment8_SetLength_Range, DMUS_PPQ,   0},
     {"Segment8_SetLength_Range(DMUS_PPQ+1)",         "Valid", Segment8_SetLength_Range, DMUS_PPQ+1, 0},
     {"Segment8_SetLength_Range(0x7FFFFFFF)",         "Valid", Segment8_SetLength_Range, 0x7FFFFFFF, 0},

     {"Segment8_GetClockTimeLength_VerifyLength(mt_1Bar.sgt)",            "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/mt_1Bar.sgt",                (DWORD)&rt0}, 
     {"Segment8_GetClockTimeLength_VerifyLength(mt_1BarExtended.sgt)",    "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/mt_1BarExtended.sgt",        (DWORD)&rt0},
     {"Segment8_GetClockTimeLength_VerifyLength(mt_1BarWaveTrack.sgt)",   "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/mt_1BarWaveTrack.sgt",       (DWORD)&rt0},
     {"Segment8_GetClockTimeLength_VerifyLength(mt_2Bars.sgt)",           "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/mt_2Bars.sgt",               (DWORD)&rt0},
     {"Segment8_GetClockTimeLength_VerifyLength(mt_2BarsLoopingInfinite.sgt)", "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/mt_2BarsLoopingInfinite.sgt", (DWORD)&rt0},
     {"Segment8_GetClockTimeLength_VerifyLength(mt_2BarsLoopingOnce.sgt)","Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/mt_2BarsLoopingOnce.sgt",    (DWORD)&rt0},
     {"Segment8_GetClockTimeLength_VerifyLength(rt_000m000s000ms.sgt)",   "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/rt_000m000s000ms.sgt",       (DWORD)&rt0},
     {"Segment8_GetClockTimeLength_VerifyLength(rt_000m000s001ms.sgt)",   "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/rt_000m000s001ms.sgt",       (DWORD)&rt10000},
     {"Segment8_GetClockTimeLength_VerifyLength(rt_000m000s777ms.sgt)",   "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/rt_000m000s777ms.sgt",       (DWORD)&rt7770000},
     {"Segment8_GetClockTimeLength_VerifyLength(rt_000m001s000ms_Ext1.sgt)", "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/rt_000m001s000ms_Ext1.sgt", (DWORD)&rt10000000},
     {"Segment8_GetClockTimeLength_VerifyLength(rt_000m001s000ms_Ext2.sgt)", "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/rt_000m001s000ms_Ext2.sgt", (DWORD)&rt10000000},
     {"Segment8_GetClockTimeLength_VerifyLength(rt_000m001s000ms_Pkup.sgt)","Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/rt_000m001s000ms_Pkup.sgt",  (DWORD)&rt10000000},
     {"Segment8_GetClockTimeLength_VerifyLength(rt_000m059s999ms.sgt)",   "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/rt_000m059s999ms.sgt",       (DWORD)&rt599990000},
     {"Segment8_GetClockTimeLength_VerifyLength(rt_999m059s999ms.sgt)",   "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/rt_999m059s999ms.sgt",       (DWORD)&rt0x7FFFFFFFFFFFFFFF},

//Supposedly these aren't valid cases either!  (see comments in bug 5287)
//     {"Segment8_GetClockTimeLength_VerifyLength(1sample.wav)",            "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/1sample.wav",                (DWORD)&rt208}, //actually 208 1/3 per sample at 48k
//     {"Segment8_GetClockTimeLength_VerifyLength(4samples.wav)",           "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/4samples.wav",               (DWORD)&rt833},
//     {"Segment8_GetClockTimeLength_VerifyLength(5samples.wav)",           "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/5samples.wav",               (DWORD)&rt1041},

     {"Segment8_GetClockTimeLength_VerifyLength(48000Samples.wav)",       "Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/48000Samples.wav",           (DWORD)&rt10000000},
     {"Segment8_GetClockTimeLength_VerifyLength(48000SamplesLooping.wav)","Valid", Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/48000SamplesLooping.wav",    (DWORD)&rt0x7FFFFFFFFFFFFFFF},
     {"Segment8_GetClockTimeLength_VerifyLength(480000Samples.wav)","Valid",Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/480000Samples.wav",                 (DWORD)&rt100000000},
     {"Segment8_GetClockTimeLength_VerifyLength(480000SamplesLooping.wav)","Valid",Segment8_GetClockTimeLength_VerifySegmentLength, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/480000SamplesLooping.wav",   (DWORD)&rt100000000},

     {"Segment8_GetClockTimeLength_GetSet(0,0)",      "Valid", Segment8_GetClockTimeLength_GetSet, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/1Sample.wav",  (DWORD)&rt0},
     {"Segment8_GetClockTimeLength_GetSet(0,1)",      "Valid", Segment8_GetClockTimeLength_GetSet, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/1Sample.wav",  (DWORD)&rt1},
     {"Segment8_GetClockTimeLength_GetSet(0,10000000)","Valid", Segment8_GetClockTimeLength_GetSet, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/1Sample.wav",  (DWORD)&rt10000000},
     {"Segment8_GetClockTimeLength_GetSet(1,0)",      "Valid", Segment8_GetClockTimeLength_GetSet, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/48000Samples.wav",  (DWORD)&rt0},
     {"Segment8_GetClockTimeLength_GetSet(1,1)",      "Valid", Segment8_GetClockTimeLength_GetSet, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/48000Samples.wav",  (DWORD)&rt1},
     {"Segment8_GetClockTimeLength_GetSet(1,10000000)","Valid", Segment8_GetClockTimeLength_GetSet, (DWORD)"DMusic/DMTest1/Segment8/GetClockTimeLength/48000Samples.wav",  (DWORD)&rt10000000},


     //Function naming "DLS" and "1Measure" signifies the type of segment and where the start point is set.
     //The two parameters are used to set the loop points, which must be tested in combination with the start points.
     // -1 for both loop points means "don't call SetLoopPoints"
     
     
     //Start points set, no looping.
     {"Segment8_SetStartPoint_DLS_1Measure_NoLoop",           "Valid", Segment8_SetStartPoint_DLS_1Measure, -1, -1},    //No looping.
     {"Segment8_SetStartPoint_DLS_2Measure_NoLoop",           "Valid", Segment8_SetStartPoint_DLS_2Measure, -1, -1},    
     {"Segment8_SetStartPoint_DLS_5Measure_NoLoop",           "Valid", Segment8_SetStartPoint_DLS_5Measure, -1, -1},    
     {"Segment8_SetStartPoint_DLS_EndMinus1_NoLoop",          "Valid", Segment8_SetStartPoint_DLS_EndMinus1,-1, -1},    
     {"Segment8_SetStartPoint_DLS_1Measure_2_4",              "Valid", Segment8_SetStartPoint_DLS_1Measure, 2, 4},    //Start before the loop starts.
     {"Segment8_SetStartPoint_DLS_1Measure_1_4",              "Valid", Segment8_SetStartPoint_DLS_1Measure, 1, 4},    //Start at the beginning of the loop.
     {"Segment8_SetStartPoint_DLS_2Measure_1_4",              "Valid", Segment8_SetStartPoint_DLS_2Measure, 1, 4},    //Start in the middle of the loop.
     {"Segment8_SetStartPoint_DLS_5Measure_1_5",              "Valid", Segment8_SetStartPoint_DLS_5Measure, 1, 5},    //Start at the end of the loop.
     {"Segment8_SetStartPoint_DLS_5Measure_1_4",              "Valid", Segment8_SetStartPoint_DLS_5Measure, 1, 4},    //Start past the end of the loop.

     {"Segment8_SetStartPoint_WaveSeg_1Measure_NoLoop",           "Valid", Segment8_SetStartPoint_WaveSeg_1Measure, -1, -1},    //No looping.
     {"Segment8_SetStartPoint_WaveSeg_2Measure_NoLoop",           "Valid", Segment8_SetStartPoint_WaveSeg_2Measure, -1, -1},    
     {"Segment8_SetStartPoint_WaveSeg_5Measure_NoLoop",           "Valid", Segment8_SetStartPoint_WaveSeg_5Measure, -1, -1},    
     {"Segment8_SetStartPoint_WaveSeg_EndMinus1_NoLoop",          "Valid", Segment8_SetStartPoint_WaveSeg_EndMinus1,-1, -1},    
     {"Segment8_SetStartPoint_WaveSeg_1Measure_2_4",              "Valid", Segment8_SetStartPoint_WaveSeg_1Measure, 2, 4},    //Start before the loop starts.
     {"Segment8_SetStartPoint_WaveSeg_1Measure_1_4",              "Valid", Segment8_SetStartPoint_WaveSeg_1Measure, 1, 4},    //Start at the beginning of the loop.
     {"Segment8_SetStartPoint_WaveSeg_2Measure_1_4",              "Valid", Segment8_SetStartPoint_WaveSeg_2Measure, 1, 4},    //Start in the middle of the loop.
     {"Segment8_SetStartPoint_WaveSeg_5Measure_1_5",              "Valid", Segment8_SetStartPoint_WaveSeg_5Measure, 1, 5},    //Start at the end of the loop.
     {"Segment8_SetStartPoint_WaveSeg_5Measure_1_4",              "Valid", Segment8_SetStartPoint_WaveSeg_5Measure, 1, 4},    //Start past the end of the loop.

     {"Segment8_GetStartPoint_Generic_Measure1",               "Valid", Segment8_GetStartPoint_Generic, (DWORD)"DMusic/DMTest1/Segment8/GetSetStartPoint/SegStartPoint_Measure1.sgt", 0 * 4 * DMUS_PPQ},
     {"Segment8_GetStartPoint_Generic_Measure2",               "Valid", Segment8_GetStartPoint_Generic, (DWORD)"DMusic/DMTest1/Segment8/GetSetStartPoint/SegStartPoint_Measure2.sgt", 1 * 4 * DMUS_PPQ},
     {"Segment8_GetStartPoint_Generic_Wave",                   "Valid", Segment8_GetStartPoint_Generic, (DWORD)"DMusic/DMTest1/Segment8/GetSetStartPoint/CountTo6.wav",               0 * 4 * DMUS_PPQ},

/*
     {"Segment8_SetClockTimeLoopPoints_DLS(0,1)",   "Valid", Segment8_SetClockTimeLoopPoints_DLS, 0,        10000000},
     {"Segment8_SetClockTimeLoopPoints_DLS(0,6)",   "Valid", Segment8_SetClockTimeLoopPoints_DLS, 0,        60000000},
     {"Segment8_SetClockTimeLoopPoints_DLS(1,6)",   "Valid", Segment8_SetClockTimeLoopPoints_DLS, 10000000, 60000000},
     {"Segment8_SetClockTimeLoopPoints_DLS(5,6)",   "Valid", Segment8_SetClockTimeLoopPoints_DLS, 50000000, 60000000},

     {"Segment8_SetClockTimeLoopPoints_WaveSeg(0,1)",   "Valid", Segment8_SetClockTimeLoopPoints_WaveSeg, 0,        10000000},
     {"Segment8_SetClockTimeLoopPoints_WaveSeg(0,6)",   "Valid", Segment8_SetClockTimeLoopPoints_WaveSeg, 0,        60000000},
     {"Segment8_SetClockTimeLoopPoints_WaveSeg(1,6)",   "Valid", Segment8_SetClockTimeLoopPoints_WaveSeg, 10000000, 60000000},
     {"Segment8_SetClockTimeLoopPoints_WaveSeg(5,6)",   "Valid", Segment8_SetClockTimeLoopPoints_WaveSeg, 50000000, 60000000},

     {"Segment8_SetClockTimeLoopPoints_Wave(0,1)",   "Valid", Segment8_SetClockTimeLoopPoints_Wave, 0,        10000000},
     {"Segment8_SetClockTimeLoopPoints_Wave(0,6)",   "Valid", Segment8_SetClockTimeLoopPoints_Wave, 0,        60000000},
     {"Segment8_SetClockTimeLoopPoints_Wave(1,6)",   "Valid", Segment8_SetClockTimeLoopPoints_Wave, 10000000, 60000000},
     {"Segment8_SetClockTimeLoopPoints_Wave(5,6)",   "Valid", Segment8_SetClockTimeLoopPoints_Wave, 50000000, 60000000},

//     {"Segment8_SetClockTimeLoopPoints_SetsClockTimeMode",   "Valid", Segment8_SetClockTimeLoopPoints_SetsClockTimeMode, 0, 0},
     {"Segment8_SetClockTimeLoopPoints_KeepsLength",         "Valid", Segment8_SetClockTimeLoopPoints_KeepsLength, 0, 0},    
     {"Segment8_GetClockTimeLoopPoints_SetAndGet",          "Valid",  Segment8_GetClockTimeLoopPoints_SetAndGet,  0, 0},    
*/

//     {"Segment8_SetWavePlaybackParams_WAVEF_OFF(normal)",    "Valid",  Segment8_SetWavePlaybackParams_WAVEF_OFF, (DWORD)"WAV/CountTo3.wav", 0},    
//     {"Segment8_SetWavePlaybackParams_WAVEF_OFF(looping)",   "Valid",  Segment8_SetWavePlaybackParams_WAVEF_OFF, (DWORD)"WAV/Loop3Times.wav", 0},    

/*
     {"Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingOFF_RA0", "Valid",  Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingOFF, DMUS_READAHEAD_MIN, 1808322},
     {"Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingON_RA0" , "Valid",  Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingON,  DMUS_READAHEAD_MIN, 934 + 44100 * 2 * 2 / 20 * 2},
     {"Segment8_SetWavePlaybackParams_WAVEF_STREAMING_CountTo3_StreamingOFF_RA0", "Valid",  Segment8_SetWavePlaybackParams_WAVEF_STREAMING_CountTo3_StreamingOFF,         DMUS_READAHEAD_MIN, 546884},
     {"Segment8_SetWavePlaybackParams_WAVEF_STREAMING_CountTo3_StreamingON_RA0" , "Valid",  Segment8_SetWavePlaybackParams_WAVEF_STREAMING_CountTo3_StreamingON,          DMUS_READAHEAD_MIN, 934 + 44100 * 2 * 2 / 20 * 2},
     {"Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingON_RA1s" , "Valid",  Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingON, 1000,   44100 * 2 * 2 * 1 * 2},
     {"Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingON_RA4s" , "Valid",  Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingON, 4000,   44100 * 2 * 2 * 4 * 2},
     {"Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingON_RA10s" , "Valid",  Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingON, 10000, 44100 * 2 * 2 * 10 * 2},
     {"Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingON_RA20s" , "Valid",  Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingON, 20000, 44100 * 2 * 2 * 20 * 2},
*/
     
     {"Segment8_SetWavePlaybackParams_WAVEF_NOINVALIDATE_FALSE" , "Valid",  Segment8_SetWavePlaybackParams_WAVEF_NOINVALIDATE, FALSE, 0},
     {"Segment8_SetWavePlaybackParams_WAVEF_NOINVALIDATE_TRUE"  , "Valid",  Segment8_SetWavePlaybackParams_WAVEF_NOINVALIDATE, TRUE,  0},


#define CUTOFF 0
#define NOCUT 1
#define NOCHANGE 2

     {"Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS_FALSE(CUTOFF,CUTOFF)" ,     "Valid",  Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS, CUTOFF, CUTOFF},
     {"Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS_FALSE(CUTOFF,NOCUT)" ,      "Valid",  Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS, CUTOFF, NOCUT},
     {"Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS_FALSE(CUTOFF,NOCHANGE)" ,   "Valid",  Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS, CUTOFF, NOCHANGE},
     {"Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS_FALSE(NOCUT,CUTOFF)" ,      "Valid",  Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS, NOCUT, CUTOFF},
     {"Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS_FALSE(NOCUT,NOCUT)" ,       "Valid",  Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS, NOCUT, NOCUT},
     {"Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS_FALSE(NOCUT,NOCHANGE)" ,    "Valid",  Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS, NOCUT, NOCHANGE},

#undef NOCHANGE 
#undef CUTOFF 
#undef NOCUT 


     {"Segment8_GetRepeats_Content(0)" ,            "Valid",  Segment8_GetRepeats_Content, 0,          NULL},
     {"Segment8_GetRepeats_Content(1)" ,            "Valid",  Segment8_GetRepeats_Content, 1,          NULL},
     {"Segment8_GetRepeats_Content(2)" ,            "Valid",  Segment8_GetRepeats_Content, 2,          NULL},
     {"Segment8_GetRepeats_Content(32767)" ,        "Valid",  Segment8_GetRepeats_Content, 32767,      NULL},
     {"Segment8_GetRepeats_Content(0xFFFFFFFF)" ,   "Valid",  Segment8_GetRepeats_Content, 0xFFFFFFFF, NULL},
     {"Segment8_GetRepeats_Programmatic" ,          "Valid",  Segment8_GetRepeats_Programmatic, NULL, NULL},


//Invalid test cases
//     {"Segment8_SetClockTimeLoopPoints_DLS(0,8)",   "Valid", Segment8_SetClockTimeLoopPoints_DLS, 0,        80000000},
//     {"Segment8_SetClockTimeLoopPoints_DLS(7,8)",   "Valid", Segment8_SetClockTimeLoopPoints_DLS, 60000000, 80000000},
//     {"Segment8_SetStartPoint_DLS_2Measure_1_4",           "Valid", Segment8_SetStartPoint_DLS_1Measure, 1, 6},    //Start in the middle of the loop.
//     {"Segment8_SetClockTimeLoopPoints_WaveSeg(0,8)",   "Valid", Segment8_SetClockTimeLoopPoints_WaveSeg, 0,        80000000},
//     {"Segment8_SetClockTimeLoopPoints_WaveSeg(7,8)",   "Valid", Segment8_SetClockTimeLoopPoints_WaveSeg, 60000000, 80000000},
//     {"Segment8_SetClockTimeLoopPoints_Wave(0,8)",   "Valid", Segment8_SetClockTimeLoopPoints_Wave, 0,        80000000},
//     {"Segment8_SetClockTimeLoopPoints_Wave(7,8)",   "Valid", Segment8_SetClockTimeLoopPoints_Wave, 60000000, 80000000},



    //SegmentState8 Tests
    //-------------------
     {"SegmentState_GetObjectInPath_Valid",           "Valid", SegmentState_GetObjectInPath_Valid, 0, 0},                  
     {"SegmentState_GetSegment_Valid",                "Valid", SegmentState_GetSegment_Valid, 0, 0},
     {"SegmentState_GetStartTime_Valid",              "Valid", SegmentState_GetStartTime_Valid, 0, 0},

    {"SegmentState_SetVolume_Valid_Ramp_0ms",            "Valid", SegmentState_SetVolume_Valid_Ramp, 0,      DMUS_APATH_DYNAMIC_MONO},
    {"SegmentState_SetVolume_Valid_Ramp_1ms",            "Valid", SegmentState_SetVolume_Valid_Ramp, 1,      DMUS_APATH_DYNAMIC_MONO},
    {"SegmentState_SetVolume_Valid_Ramp_1000ms",         "Valid", SegmentState_SetVolume_Valid_Ramp, 1000,   DMUS_APATH_DYNAMIC_MONO},
    {"SegmentState_SetVolume_Valid_Ramp_10000ms",        "Valid", SegmentState_SetVolume_Valid_Ramp, 10000,  DMUS_APATH_DYNAMIC_MONO},
    {"SegmentState_SetVolume_Valid_Ramp_ULONG_MAXms",    "Valid", SegmentState_SetVolume_Valid_Ramp, ULONG_MAX,DMUS_APATH_DYNAMIC_MONO},
    {"SegmentState_SetVolume_Valid_Ramp_0ms_STEREOPLUSREVERB",            "Valid", SegmentState_SetVolume_Valid_Ramp, 0,      DMUS_APATH_SHARED_STEREOPLUSREVERB},
    {"SegmentState_SetVolume_Valid_Ramp_1ms_STEREOPLUSREVERB",            "Valid", SegmentState_SetVolume_Valid_Ramp, 1,      DMUS_APATH_SHARED_STEREOPLUSREVERB},
    {"SegmentState_SetVolume_Valid_Ramp_1000ms_STEREOPLUSREVERB",         "Valid", SegmentState_SetVolume_Valid_Ramp, 1000,   DMUS_APATH_SHARED_STEREOPLUSREVERB},
    {"SegmentState_SetVolume_Valid_Ramp_10000ms_STEREOPLUSREVERB",        "Valid", SegmentState_SetVolume_Valid_Ramp, 10000,  DMUS_APATH_SHARED_STEREOPLUSREVERB},
    {"SegmentState_SetVolume_Valid_Ramp_ULONG_MAXms_STEREOPLUSREVERB",    "Valid", SegmentState_SetVolume_Valid_Ramp, ULONG_MAX,DMUS_APATH_SHARED_STEREOPLUSREVERB},

    {"SegmentState_SetPitch_Valid_Ramp_0ms(listen)",    "Valid", SegmentState_SetPitch_Valid_Ramp, 0,      2400},
    {"SegmentState_SetPitch_Valid_Ramp_1000ms(listen)", "Valid", SegmentState_SetPitch_Valid_Ramp, 1000,   2400},
    {"SegmentState_SetPitch_Valid_Ramp_10000ms(listen)","Valid", SegmentState_SetPitch_Valid_Ramp, 10000,  2400},

    //ToolGraph tests
    //---------------
    {"ToolGraph_InsertTool_Valid",                    "Valid" , ToolGraph_InsertTool_Valid, 0, 0},   
    {"ToolGraph_StampPMsg_Valid",                     "Valid" , ToolGraph_StampPMsg_Valid, 0, 0},
    {"ToolGraph_RemoveTool_Valid",                    "Valid" , ToolGraph_RemoveTool_Valid, 0, 0},
    {"ToolGraph_GetTool_Valid",                       "Valid" , ToolGraph_GetTool_Valid, 0, 0},

    {"Sleep_Zero",                       "Valid" , Sleep_Zero, 0, 0},



/******************************************************************************************
PERFORMANCE_MEMORY TEST CASES
******************************************************************************************/
//     {"Loader_GetObject_PerfMem_DownloadWaveLeak(one-shot)",  "PerfMem", Loader_GetObject_PerfMem_DownloadWaveLeak, FALSE, 0},
//     {"Loader_GetObject_PerfMem_DownloadWaveLeak(streaming)", "PerfMem", Loader_GetObject_PerfMem_DownloadWaveLeak, TRUE,  0},

     {"Loader_GetObject_PerfMem_Type_Full(IndAudioPath)"    , "PerfMem", Loader_GetObject_PerfMem_Type_Full,            (DWORD)"DMusic/DMTest1/Loader/Perf/IndAudioPath.aud", 0},
//     {"Loader_GetObject_PerfMem_Type_Full(IndBand)"         , "PerfMem", Loader_GetObject_PerfMem_Type_Full,          (DWORD)"DMusic/DMTest1/Loader/Perf/IndBand.bnd", 0},
     {"Loader_GetObject_PerfMem_Type_Full(IndChordmap)"     , "PerfMem", Loader_GetObject_PerfMem_Type_Full,            (DWORD)"DMusic/DMTest1/Loader/Perf/IndChordmap.cdm", 0},
     {"Loader_GetObject_PerfMem_Type_Full(ContainerEmbedIndSegment)"  , "PerfMem", Loader_GetObject_PerfMem_Type_Full,  (DWORD)"DMusic/DMTest1/Loader/Perf/ContainerEmbedIndSegment.con", 0},
     {"Loader_GetObject_PerfMem_Type_Full(ContainerEmbedRefSegment)"  , "PerfMem", Loader_GetObject_PerfMem_Type_Full,  (DWORD)"DMusic/DMTest1/Loader/Perf/ContainerEmbedRefSegment.con", 0},
     {"Loader_GetObject_PerfMem_Type_Full(ContainerRefRefSegment)"    , "PerfMem", Loader_GetObject_PerfMem_Type_Full,  (DWORD)"DMusic/DMTest1/Loader/Perf/ContainerRefRefSegment.con", 0},
     {"Loader_GetObject_PerfMem_Type_Full(DLS)"         ,     "PerfMem", Loader_GetObject_PerfMem_Type_Full,            (DWORD)"DMusic/DMTest1/Loader/Perf/DLS.dls", 0},
     {"Loader_GetObject_PerfMem_Type_Full(IndSegment)"         , "PerfMem", Loader_GetObject_PerfMem_Type_Full,         (DWORD)"DMusic/DMTest1/Loader/Perf/IndSegment.sgt", 0},
     {"Loader_GetObject_PerfMem_Type_Full(RefSegment)"         , "PerfMem", Loader_GetObject_PerfMem_Type_Full,         (DWORD)"DMusic/DMTest1/Loader/Perf/RefSegment.sgt", 0},
     {"Loader_GetObject_PerfMem_Type_Full(ScriptNoRef)"         , "PerfMem", Loader_GetObject_PerfMem_Type_Full,        (DWORD)"DMusic/DMTest1/Loader/Perf/ScriptNoRef.spt", 0},
     {"Loader_GetObject_PerfMem_Type_Full(ScriptEmbedIndSegment)" , "PerfMem", Loader_GetObject_PerfMem_Type_Full,      (DWORD)"DMusic/DMTest1/Loader/Perf/ScriptEmbedIndSegment.spt", 0},
     {"Loader_GetObject_PerfMem_Type_Full(ScriptEmbedRefSegment)" , "PerfMem", Loader_GetObject_PerfMem_Type_Full,      (DWORD)"DMusic/DMTest1/Loader/Perf/ScriptEmbedRefSegment.spt", 0},
     {"Loader_GetObject_PerfMem_Type_Full(ScriptRefRefSegment)"         , "PerfMem", Loader_GetObject_PerfMem_Type_Full,(DWORD)"DMusic/DMTest1/Loader/Perf/ScriptRefRefSegment.spt", 0},
     {"Loader_GetObject_PerfMem_Type_Full(IndStyle)"         , "PerfMem", Loader_GetObject_PerfMem_Type_Full,           (DWORD)"DMusic/DMTest1/Loader/Perf/IndStyle.sty", 0},
     {"Loader_GetObject_PerfMem_Type_Full(Shortwave)"      , "PerfMem", Loader_GetObject_PerfMem_Type_Full,             (DWORD)"WAV/ShortWave.wav", 0},
//     {"Loader_GetObject_PerfMem_Type_Full(16MB_OneShot)"      , "PerfMem", Loader_GetObject_PerfMem_Type_Full,        (DWORD)"WAV/16MB_OneShot.wav", 0}, //Can't load a band anymore
     {"Loader_GetObject_PerfMem_Type_Full(Streaming)"      , "PerfMem", Loader_GetObject_PerfMem_Type_Full,             (DWORD)"WAV/Streaming10s.wav", 0},
     {"Loader_GetObject_PerfMem_Type_Full(LoopForever)"      , "PerfMem", Loader_GetObject_PerfMem_Type_Full,           (DWORD)"WAV/LoopForever.wav", 0},
     {"Loader_GetObject_PerfMem_Type_Full(SegEmbedWave)"      , "PerfMem", Loader_GetObject_PerfMem_Type_Full,          (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegEmbedWave.sgt", 0},



     {"Loader_GetObject_PerfMem_Type_Partial(IndAudioPath)"    , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,      (DWORD)"DMusic/DMTest1/Loader/Perf/IndAudioPath.aud", 0},
//     {"Loader_GetObject_PerfMem_Type_Partial(IndBand)"         , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,    (DWORD)"DMusic/DMTest1/Loader/Perf/IndBand.bnd", 0}, //Can't load a band anymore
     {"Loader_GetObject_PerfMem_Type_Partial(IndChordmap)"     , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,      (DWORD)"DMusic/DMTest1/Loader/Perf/IndChordmap.cdm", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(ContainerEmbedIndSegment)" , "PerfMem", Loader_GetObject_PerfMem_Type_Partial, (DWORD)"DMusic/DMTest1/Loader/Perf/ContainerEmbedIndSegment.con", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(ContainerEmbedRefSegment)" , "PerfMem", Loader_GetObject_PerfMem_Type_Partial, (DWORD)"DMusic/DMTest1/Loader/Perf/ContainerEmbedRefSegment.con", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(ContainerRefRefSegment)"   , "PerfMem", Loader_GetObject_PerfMem_Type_Partial, (DWORD)"DMusic/DMTest1/Loader/Perf/ContainerRefRefSegment.con", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(DLS)"         ,     "PerfMem", Loader_GetObject_PerfMem_Type_Partial,      (DWORD)"DMusic/DMTest1/Loader/Perf/DLS.dls", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(IndSegment)"         , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,   (DWORD)"DMusic/DMTest1/Loader/Perf/IndSegment.sgt", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(RefSegment)"         , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,   (DWORD)"DMusic/DMTest1/Loader/Perf/RefSegment.sgt", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(ScriptNoRef)"         , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,  (DWORD)"DMusic/DMTest1/Loader/Perf/ScriptNoRef.spt", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(ScriptEmbedIndSegment)" , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,(DWORD)"DMusic/DMTest1/Loader/Perf/ScriptEmbedIndSegment.spt", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(ScriptEmbedRefSegment)" , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,(DWORD)"DMusic/DMTest1/Loader/Perf/ScriptEmbedRefSegment.spt", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(ScriptRefRefSegment)"   , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,(DWORD)"DMusic/DMTest1/Loader/Perf/ScriptRefRefSegment.spt", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(IndStyle)"         , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,     (DWORD)"DMusic/DMTest1/Loader/Perf/IndStyle.sty", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(Shortwave)"      , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,       (DWORD)"WAV/ShortWave.wav", 0},
//     {"Loader_GetObject_PerfMem_Type_Partial(16MB_OneShot)"      , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,  (DWORD)"WAV/16MB_OneShot.wav", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(Streaming)"      , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,       (DWORD)"WAV/Streaming10s.wav", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(LoopForever)"      , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,     (DWORD)"WAV/LoopForever.wav", 0},
     {"Loader_GetObject_PerfMem_Type_Partial(SegEmbedWave)"      , "PerfMem", Loader_GetObject_PerfMem_Type_Partial,    (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegEmbedWave.sgt", 0},

    //CreateStandardAudioPath    
     {"Performance8_CreateStandardAudioPath_PerfMem(STEREOPLUSREVERB,FULL)",         "PerfMem", Performance8_CreateStandardAudioPath_PerfMem,    TRUE, DMUS_APATH_SHARED_STEREOPLUSREVERB},
     {"Performance8_CreateStandardAudioPath_PerfMem(STEREOPLUSREVERB,PARTIAL)",      "PerfMem", Performance8_CreateStandardAudioPath_PerfMem,    FALSE, DMUS_APATH_SHARED_STEREOPLUSREVERB},
     {"Performance8_CreateStandardAudioPath_PerfMem(STEREO,FULL)",                   "PerfMem", Performance8_CreateStandardAudioPath_PerfMem,    TRUE, DMUS_APATH_SHARED_STEREO},
     {"Performance8_CreateStandardAudioPath_PerfMem(STEREO,PARTIAL)",                "PerfMem", Performance8_CreateStandardAudioPath_PerfMem,    FALSE, DMUS_APATH_SHARED_STEREO},
     {"Performance8_CreateStandardAudioPath_PerfMem(3D,FULL)",                       "PerfMem", Performance8_CreateStandardAudioPath_PerfMem,    TRUE, DMUS_APATH_DYNAMIC_3D},
     {"Performance8_CreateStandardAudioPath_PerfMem(3D,PARTIAL)",                    "PerfMem", Performance8_CreateStandardAudioPath_PerfMem,    FALSE, DMUS_APATH_DYNAMIC_3D},
     {"Performance8_CreateStandardAudioPath_PerfMem(MONO,FULL)",                     "PerfMem", Performance8_CreateStandardAudioPath_PerfMem,    TRUE, DMUS_APATH_DYNAMIC_MONO},
     {"Performance8_CreateStandardAudioPath_PerfMem(MONO,PARTIAL)",                  "PerfMem", Performance8_CreateStandardAudioPath_PerfMem,    FALSE, DMUS_APATH_DYNAMIC_MONO},

     {"Performance8_CreateStandardAudioPath_PerfMem_Bug10385(20,DMUS_APATH_SHARED_STEREOPLUSREVERB)",   "PerfMem", Performance8_CreateStandardAudioPath_PerfMem_Bug10385, 1, DMUS_APATH_DYNAMIC_MONO},
    
    
    //PlaySegmentEx
//     {"Performance8_PlaySegmentEx_PerfMem(Seg16MBDLS,FULL)",         "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/Seg16MBDLS.sgt",         TRUE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(SegChordmap,FULL)",        "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegChordmap.sgt",        TRUE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(SegTriggering,FULL)",      "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegTriggering.sgt",      TRUE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(SegWaveLoopForever,FULL)", "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegWaveLoopForever.sgt", TRUE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(SegWaveShort,FULL)",       "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegWaveShort.sgt",       TRUE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(SegWaveStreaming10s,FULL)","PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegWaveStreaming10s.sgt",TRUE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(SegEmbedWave,FULL)",       "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegEmbedWave.sgt"       ,TRUE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(IndSegment,FULL)"        , "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/IndSegment.sgt",       TRUE},
     {"Performance8_PlaySegmentEx_PerfMem(RefSegment,FULL)"        , "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/RefSegment.sgt",       TRUE},
     {"Performance8_PlaySegmentEx_PerfMem(Shortwave,FULL)"         , "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"WAV/ShortWave.wav",                            TRUE},
//     {"Performance8_PlaySegmentEx_PerfMem(16MB_OneShot,FULL)"      , "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"WAV/16MB_OneShot.wav",                         TRUE},
     {"Performance8_PlaySegmentEx_PerfMem(Streaming,FULL)"         , "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"WAV/Streaming10s.wav",                         TRUE},
     {"Performance8_PlaySegmentEx_PerfMem(LoopForever,FULL)"       , "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"WAV/LoopForever.wav",                          TRUE},
     {"Performance8_PlaySegmentEx_PerfMem(48_16_2,FULL)"           , "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"WAV/48_16_2.wav",                              TRUE},

//     {"Performance8_PlaySegmentEx_PerfMem(Seg16MBDLS,PARTIAL)",         "PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/Seg16MBDLS.sgt",      FALSE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(SegChordmap,PARTIAL)",        "PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegChordmap.sgt",     FALSE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(SegTriggering,PARTIAL)",      "PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegTriggering.sgt",   FALSE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(SegWaveLoopForever,PARTIAL)", "PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegWaveLoopForever.sgt",FALSE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(SegWaveShort,PARTIAL)",       "PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegWaveShort.sgt",    FALSE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(SegWaveStreaming10s,PARTIAL)","PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegWaveStreaming10s.sgt",FALSE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(SegEmbedWave,PARTIAL)",       "PerfMem", Performance8_PlaySegmentEx_PerfMem,    (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegEmbedWave.sgt"       ,FALSE}, //Play all the way.
     {"Performance8_PlaySegmentEx_PerfMem(IndSegment,PARTIAL)"     ,    "PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/IndSegment.sgt",       FALSE},
     {"Performance8_PlaySegmentEx_PerfMem(RefSegment,PARTIAL)"     ,    "PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/RefSegment.sgt",       FALSE},
     {"Performance8_PlaySegmentEx_PerfMem(Shortwave,PARTIAL)"      ,    "PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"WAV/ShortWave.wav",                            FALSE},
//     {"Performance8_PlaySegmentEx_PerfMem(16MB_OneShot,PARTIAL)"   ,    "PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"WAV/16MB_OneShot.wav",                         FALSE},
     {"Performance8_PlaySegmentEx_PerfMem(Streaming,PARTIAL)"      ,    "PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"WAV/Streaming10s.wav",                         FALSE},
     {"Performance8_PlaySegmentEx_PerfMem(LoopForever,PARTIAL)"    ,    "PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"WAV/LoopForever.wav",                          FALSE},
     {"Performance8_PlaySegmentEx_PerfMem(48_16_2,PARTIAL)"        ,    "PerfMem", Performance8_PlaySegmentEx_PerfMem, (DWORD)"WAV/48_16_2.wav",                              FALSE},

     {"Performance8_PlaySegmentEx_StartStopStreaming_PerfMem",  "PerfMem", Performance8_PlaySegmentEx_StartStopStreaming_PerfMem, 0, 0},

     //SegmentState8
     {"SegmentState_GetSegment_PerfMem(PARTIAL)"                   ,"PerfMem", SegmentState_GetSegment_PerfMem, FALSE,0},
     {"SegmentState_GetSegment_PerfMem(FULL)"                      ,"PerfMem", SegmentState_GetSegment_PerfMem, TRUE, 0},

    //Script
     {"Script_CallRoutine_PerfMem1_Full(Routines,0)",    "PerfMem", Script_CallRoutine_PerfMem1_Full,    (DWORD)"DMusic/DMTest1/script/perfmem/Routines.spt", 0},
     {"Script_CallRoutine_PerfMem1_Full(Routines,1)",    "PerfMem", Script_CallRoutine_PerfMem1_Full,    (DWORD)"DMusic/DMTest1/script/perfmem/Routines.spt", 1},
     {"Script_CallRoutine_PerfMem1_Partial(Routines,0)", "PerfMem", Script_CallRoutine_PerfMem1_Partial, (DWORD)"DMusic/DMTest1/script/perfmem/Routines.spt", 0},
     {"Script_CallRoutine_PerfMem1_Partial(Routines,1)", "PerfMem", Script_CallRoutine_PerfMem1_Partial, (DWORD)"DMusic/DMTest1/script/perfmem/Routines.spt", 1},

    // More script Tests
    //-------------------
     {"Script_PerfMem_AudioPath_SetGetVolume(FULL)",          "PerfMem",  Script_CallRoutine_PerfMem_TestScript,   0, TRUE},
     {"Script_PerfMem_AudioPath_SetVolumeListeningTest(FULL)","PerfMem",  Script_CallRoutine_PerfMem_TestScript,   1, TRUE},
     {"Script_PerfMem_AudioPathConfig_Create(FULL)",          "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  10, TRUE},
     {"Script_PerfMem_AudioPathConfig_Load(FULL)",            "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  11, TRUE},
     {"Script_PerfMem_Segment_PlayStop(FULL)",                "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  20, TRUE},
     {"Script_PerfMem_Performance_SetMasterGrooveLevel(FULL)","PerfMem",  Script_CallRoutine_PerfMem_TestScript,  30, TRUE},
     {"Script_PerfMem_Performance_SetMasterTempo(DLS)(FULL)", "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  32, TRUE},
     {"Script_PerfMem_Performance_SetMasterTempo(WAVE)(FULL)","PerfMem",  Script_CallRoutine_PerfMem_TestScript,  33, TRUE},
     {"Script_PerfMem_Performance_GetMasterTempo(FULL)",      "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  34, TRUE},
     {"Script_PerfMem_Performance_SetMasterTranspose(FULL)",  "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  35, TRUE},
     {"Script_PerfMem_Performance_GetMasterTranspose(FULL)",  "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  36, TRUE},
     {"Script_PerfMem_Performance_Rand(FULL)",                "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  37, TRUE},
     {"Script_PerfMem_PlayingSegment_IsPlaying(VerifyTrue)(FULL)", "PerfMem",Script_CallRoutine_PerfMem_TestScript,50,TRUE},
     {"Script_PerfMem_PlayingSegment_IsPlaying(VerifyFalse)(FULL)","PerfMem",Script_CallRoutine_PerfMem_TestScript,51,TRUE},
     {"Script_PerfMem_PlayingSegment_Stop(FULL)",             "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  52, TRUE},
     {"Script_PerfMem_AutoLoadUnchecked_SegmentNoLoad(FULL)", "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 100, TRUE},
     {"Script_PerfMem_AutoLoadUnchecked_SegmentLoad(FULL)",   "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 101, TRUE},
     {"Script_PerfMem_AutoLoadUnchecked_AudioPathNoLoad(FULL)","PerfMem", Script_CallRoutine_PerfMem_TestScript, 102, TRUE},
     {"Script_PerfMem_AutoLoadUnchecked_AudioPathLoad(FULL)", "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 103, TRUE},
     {"Script_PerfMem_AutoLoadChecked_SegmentLoad(FULL)",     "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 110, TRUE},
     {"Script_PerfMem_AutoLoadChecked_AudioPathLoad(FULL)",   "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 111, TRUE},
     {"Script_PerfMem_AutoDownloadUnchecked_NoDownload(FULL)","PerfMem",  Script_CallRoutine_PerfMem_TestScript, 120, TRUE},
     {"Script_PerfMem_AutoDownloadUnchecked_Download(FULL)",  "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 121, TRUE},
     {"Script_PerfMem_AutoDownloadUnchecked_Unload(FULL)",    "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 122, TRUE},
     {"Script_PerfMem_EmbeddedContent(FULL)",                 "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 150, TRUE},
     {"Script_PerfMem_Recompose(FULL)",                       "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 160, TRUE},

     {"Script_PerfMem_AudioPath_SetGetVolume(PARTIAL)",          "PerfMem",  Script_CallRoutine_PerfMem_TestScript,   0, FALSE},
     {"Script_PerfMem_AudioPath_SetVolumeListeningTest(PARTIAL)","PerfMem",  Script_CallRoutine_PerfMem_TestScript,   1, FALSE},
     {"Script_PerfMem_AudioPathConfig_Create(PARTIAL)",          "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  10, FALSE},
     {"Script_PerfMem_AudioPathConfig_Load(PARTIAL)",            "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  11, FALSE},
     {"Script_PerfMem_Segment_PlayStop(PARTIAL)",                "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  20, FALSE},
     {"Script_PerfMem_Performance_SetMasterGrooveLevel(PARTIAL)","PerfMem",  Script_CallRoutine_PerfMem_TestScript,  30, FALSE},
     {"Script_PerfMem_Performance_SetMasterTempo(DLS)(PARTIAL)", "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  32, FALSE},
     {"Script_PerfMem_Performance_SetMasterTempo(WAVE)(PARTIAL)","PerfMem",  Script_CallRoutine_PerfMem_TestScript,  33, FALSE},
     {"Script_PerfMem_Performance_GetMasterTempo(PARTIAL)",      "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  34, FALSE},
     {"Script_PerfMem_Performance_SetMasterTranspose(PARTIAL)",  "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  35, FALSE},
     {"Script_PerfMem_Performance_GetMasterTranspose(PARTIAL)",  "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  36, FALSE},
     {"Script_PerfMem_Performance_Rand(PARTIAL)",                "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  37, FALSE},
     {"Script_PerfMem_PlayingSegment_IsPlaying(VerifyTrue) (PARTIAL)","PerfMem",Script_CallRoutine_PerfMem_TestScript,50,FALSE},
     {"Script_PerfMem_PlayingSegment_IsPlaying(VerifyFalse)(PARTIAL)","PerfMem",Script_CallRoutine_PerfMem_TestScript,51,FALSE},
     {"Script_PerfMem_PlayingSegment_Stop(PARTIAL)",             "PerfMem",  Script_CallRoutine_PerfMem_TestScript,  52, FALSE},
     {"Script_PerfMem_AutoLoadUnchecked_SegmentNoLoad(PARTIAL)", "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 100, FALSE},
     {"Script_PerfMem_AutoLoadUnchecked_SegmentLoad(PARTIAL)",   "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 101, FALSE},
     {"Script_PerfMem_AutoLoadUnchecked_AudioPathNoLoad(PARTIAL)","PerfMem", Script_CallRoutine_PerfMem_TestScript, 102, FALSE},
     {"Script_PerfMem_AutoLoadUnchecked_AudioPathLoad(PARTIAL)", "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 103, FALSE},
     {"Script_PerfMem_AutoLoadChecked_SegmentLoad(PARTIAL)",     "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 110, FALSE},
     {"Script_PerfMem_AutoLoadChecked_AudioPathLoad(PARTIAL)",   "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 111, FALSE},
     {"Script_PerfMem_AutoDownloadUnchecked_NoDownload(PARTIAL)","PerfMem",  Script_CallRoutine_PerfMem_TestScript, 120, FALSE},
     {"Script_PerfMem_AutoDownloadUnchecked_Download(PARTIAL)",  "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 121, FALSE},
     {"Script_PerfMem_AutoDownloadUnchecked_Unload(PARTIAL)",    "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 122, FALSE},
     {"Script_PerfMem_EmbeddedContent(PARTIAL)",                 "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 150, FALSE},
     {"Script_PerfMem_Recompose(PARTIAL)",                       "PerfMem",  Script_CallRoutine_PerfMem_TestScript, 160, FALSE},



/******************************************************************************************
INVALID TEST CASES
******************************************************************************************/
    {"AudioPath_GetObjectInPath_Invalid",            "Invalid",AudioPath_GetObjectInPath_Invalid, 0, 0},
    {"AudioPath_SetVolume_Invalid",                  "Invalid",AudioPath_SetVolume_Invalid, 0, 0},
    {"Performance8_AllocPMsg_Invalid",               "Invalid",Performance8_AllocPMsg_Invalid, 0, 0},
    {"Performance8_IsPlaying_Invalid",               "Invalid",Performance8_IsPlaying_Invalid, 0, 0},
    {"Performance8_PlaySegmentEx_Invalid(ContainsAP,10)","Invalid", Performance8_PlaySegmentEx_Valid_Play, (DWORD)"DMusic/DMTest1/Perf8/PlaySeg/Other/ContainsAudioPathConfig.sgt", 10}, //Play for 10 seconds.


    {"Loader8_LoadObjectFromFile_Invalid",           "Invalid",Loader8_LoadObjectFromFile_Invalid, 0, 0},
    {"Loader8_LoadObjectFromFile_Invalid_GMRef",     "Invalid",Loader8_LoadObjectFromFile_Invalid_GMRef, (DWORD)"DMusic/DMTest1/Loader/Inv_GMRefs/Inv_GMRefs.sgt", 0},
    {"Loader8_LoadObjectFromFile_TooLongPath",       "Invalid",Loader8_LoadObjectFromFile_TooLongPath, 0, 0},
    {"Loader8_LoadObjectFromFile_Invalid_Content",           "Invalid",Loader8_LoadObjectFromFile_Invalid_Content, 0, 0},

    {"Script_Invalid_NoAudioPath",                   "Invalid",Script_CallRoutine_Invalid_NoAudioPath, 20, DMUS_E_LOADER_FAILEDCREATE},
    {"Script_Invalid_EmbeddedContentMissing",        "Invalid",Script_CallRoutine_Invalid_TestScript, 150, DMUS_E_LOADER_FAILEDCREATE},
    {"Segment8_SetStartPoint_DLS_End_NoLoop",        "Invalid", Segment8_SetStartPoint_DLS_End,-1, -1},    

     {"Segment8_SetStartPoint_Wave_1Measure_NoLoop",           "Invalid", Segment8_SetStartPoint_Wave_1Measure, -1, -1},    //No looping.
     {"Segment8_SetStartPoint_Wave_2Measure_NoLoop",           "Invalid", Segment8_SetStartPoint_Wave_2Measure, -1, -1},    
     {"Segment8_SetStartPoint_Wave_5Measure_NoLoop",           "Invalid", Segment8_SetStartPoint_Wave_5Measure, -1, -1},    
     {"Segment8_SetStartPoint_Wave_EndMinus1_NoLoop",          "Invalid", Segment8_SetStartPoint_Wave_EndMinus1,-1, -1},    
     {"Segment8_SetStartPoint_Wave_1Measure_2_4",              "Invalid", Segment8_SetStartPoint_Wave_1Measure, 2, 4},    //Start before the loop starts.
     {"Segment8_SetStartPoint_Wave_1Measure_1_4",              "Invalid", Segment8_SetStartPoint_Wave_1Measure, 1, 4},    //Start at the beginning of the loop.
     {"Segment8_SetStartPoint_Wave_2Measure_1_4",              "Invalid", Segment8_SetStartPoint_Wave_2Measure, 1, 4},    //Start in the middle of the loop.
     {"Segment8_SetStartPoint_Wave_5Measure_1_5",              "Invalid", Segment8_SetStartPoint_Wave_5Measure, 1, 5},    //Start at the end of the loop.
     {"Segment8_SetStartPoint_Wave_5Measure_1_4",              "Invalid", Segment8_SetStartPoint_Wave_5Measure, 1, 4},    //Start past the end of the loop.

#define ONESHOT     0
#define STREAMING   1
#define MUSIC       2
#define MULTIPAN    3
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1,4,OS)",       "Invalid", Performance8_PlaySegmentEx_AudioPath_4, DMUS_APATH_MIXBIN_5DOT1,         ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_ENV,2,OS)",   "Invalid", Performance8_PlaySegmentEx_AudioPath_2, DMUS_APATH_MIXBIN_5DOT1_ENV,     ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_ENV,4,OS)",   "Invalid", Performance8_PlaySegmentEx_AudioPath_4, DMUS_APATH_MIXBIN_5DOT1_ENV,     ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_ENV,6,OS)",   "Invalid", Performance8_PlaySegmentEx_AudioPath_6, DMUS_APATH_MIXBIN_5DOT1_ENV,     ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_MUSIC,6,OS)", "Invalid", Performance8_PlaySegmentEx_AudioPath_6, DMUS_APATH_MIXBIN_5DOT1_MUSIC,   ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1,4,ST)",       "Invalid", Performance8_PlaySegmentEx_AudioPath_4, DMUS_APATH_MIXBIN_5DOT1,         STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_ENV,2,ST)",   "Invalid", Performance8_PlaySegmentEx_AudioPath_2, DMUS_APATH_MIXBIN_5DOT1_ENV,     STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_ENV,4,ST)",   "Invalid", Performance8_PlaySegmentEx_AudioPath_4, DMUS_APATH_MIXBIN_5DOT1_ENV,     STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_ENV,6,ST)",   "Invalid", Performance8_PlaySegmentEx_AudioPath_6, DMUS_APATH_MIXBIN_5DOT1_ENV,     STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_5DOT1_MUSIC,6,ST)", "Invalid", Performance8_PlaySegmentEx_AudioPath_6, DMUS_APATH_MIXBIN_5DOT1_MUSIC,   STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD,6,OS)",        "Invalid", Performance8_PlaySegmentEx_AudioPath_6, DMUS_APATH_MIXBIN_QUAD,          ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_ENV,2,OS)",    "Invalid", Performance8_PlaySegmentEx_AudioPath_2, DMUS_APATH_MIXBIN_QUAD_ENV,      ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_ENV,4,OS)",    "Invalid", Performance8_PlaySegmentEx_AudioPath_4, DMUS_APATH_MIXBIN_QUAD_ENV,      ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_ENV,6,OS)",    "Invalid", Performance8_PlaySegmentEx_AudioPath_6, DMUS_APATH_MIXBIN_QUAD_ENV,      ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_MUSIC,6,OS)",  "Invalid", Performance8_PlaySegmentEx_AudioPath_6, DMUS_APATH_MIXBIN_QUAD_MUSIC,    ONESHOT},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD,6,ST)",        "Invalid", Performance8_PlaySegmentEx_AudioPath_6, DMUS_APATH_MIXBIN_QUAD,          STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_ENV,2,ST)",    "Invalid", Performance8_PlaySegmentEx_AudioPath_2, DMUS_APATH_MIXBIN_QUAD_ENV,      STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_ENV,4,ST)",    "Invalid", Performance8_PlaySegmentEx_AudioPath_4, DMUS_APATH_MIXBIN_QUAD_ENV,      STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_ENV,6,ST)",    "Invalid", Performance8_PlaySegmentEx_AudioPath_6, DMUS_APATH_MIXBIN_QUAD_ENV,      STREAMING},
    {"Performance8_PlaySegmentEx_AudioPath(DMUS_APATH_MIXBIN_QUAD_MUSIC,6,ST)",  "Invalid", Performance8_PlaySegmentEx_AudioPath_6, DMUS_APATH_MIXBIN_QUAD_MUSIC,    STREAMING},

#undef STREAMING
#undef ONESHOT
#undef MUSIC
#undef PANNING 




    {"Fail_On_Purpose"                           ,   "Invalid",Fail_On_Purpose,    0, 0},


    
//    {"Performance8_InitAudio_Invalid",             "Invalid",Performance8_InitAudio_Invalid, 0, 0},


/******************************************************************************************
FATAL TEST CASES
******************************************************************************************/
    {"Script_CallRoutine_Fatal",                     "Fatal", Script_CallRoutine_Fatal, 0, 0},
    {"Script_Init_Fatal",                            "Fatal", Script_Init_Fatal, 0, 0},
//    {"Script_SetVariableObject_Fatal",             "Fatal", Script_SetVariableObject_Fatal, 0, 0},
//    {"Script_SetVariableNumber_Fatal",             "Fatal", Script_SetVariableNumber_Fatal, 0, 0}
};

DWORD g_dwNumTestCases = AMOUNT(g_TestCases);




/********************************************************************************
Verify our test array is set up without any bugs.  If there are problems,
we'll print them and return E_FAIL;
********************************************************************************/
HRESULT SanityCheckTestCases(void)
{
LPSTR szValidTestTypes[] = {"BVT", "Valid", "Invalid", "Fatal", "PerfMem"};
HRESULT hr = S_OK;
DWORD i=0, j=0;

//Verify no dupes.
for (i=0; i<AMOUNT(g_TestCases)-1; i++)
{
    for (j=i+1; j<AMOUNT(g_TestCases); j++)
    {
        if (_strcmpi(g_TestCases[i].szTestName, g_TestCases[j].szTestName) == 0)
        {
            hr = E_FAIL;
            Log(ABORTLOGLEVEL, "Sanity check error: g_TestCases array elements %d and %d both have %s as the Test Name", i, j, g_TestCases[i].szTestName);
        }
    }
}

//Verify all test "types" are valid.
for (i=0; i<AMOUNT(g_TestCases); i++)
{
    BOOL bFound = FALSE;
    for (j=0; j<AMOUNT(szValidTestTypes); j++)
    {
        if (strcmp(g_TestCases[i].szTestDesc, szValidTestTypes[j]) == 0)
        {
            bFound = TRUE;
            break;
        }
    }

    if (!bFound)
    {
        hr = E_FAIL;
        Log(ABORTLOGLEVEL, "Sanity check error: g_TestCases array element %d had invalid description (%s)", i, g_TestCases[i].szTestDesc);
    }
}



//Verify that a method of a certain type isn't "sandwiched" between other types (for readability)
for (i=0; i<AMOUNT(g_TestCases)-2; i++)
{
    if (
        (_strcmpi(g_TestCases[i].szTestDesc, g_TestCases[i+1].szTestDesc) != 0) &&
        (_strcmpi(g_TestCases[i].szTestDesc, g_TestCases[i+2].szTestDesc) == 0)
        )
    {
        hr = E_FAIL;
        Log(ABORTLOGLEVEL, "Sanity check error: %s is sandwiched between two elements with different descriptions", g_TestCases[i+1]);
    }
}


return hr;
}

