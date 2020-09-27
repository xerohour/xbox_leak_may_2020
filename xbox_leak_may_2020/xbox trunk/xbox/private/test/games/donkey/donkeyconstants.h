#pragma once

static const float  LEFT_LANE          = 124.0f;
static const float  RIGHT_LANE         = 165.0f;
static const float  MOVE_UP_AMOUNT     = 5.0f;
static const float  MOVE_DOWN_AMOUNT   = 5.0f;
static const float  LEFT_BACK_LEFT_X   = 0.0f;
static const float  LEFT_BACK_LEFT_Y   = 0.0f;
static const float  LEFT_BACK_RIGHT_X  = 119.0f;
static const float  LEFT_BACK_RIGHT_Y  = 239.0f;
static const float  RIGHT_BACK_LEFT_X  = 200.0f;
static const float  RIGHT_BACK_LEFT_Y  = 0.0f;
static const float  RIGHT_BACK_RIGHT_X = 319.0f;
static const float  RIGHT_BACK_RIGHT_Y = 239.0f;
static const float  DONKEY_STRING_X    = 3.0f;
static const float  DONKEY_STRING_Y    = 20.0f;
static const float  DRIVER_STRING_X    = 212.0f;
static const float  DRIVER_STRING_Y    = 20.0f;
static const float  ROAD_INCREMENT     = 24.0f;
static const float  ROAD_X_POS         = 159.0f;
static const float  ROAD_LENGTH        = 12.0f;
static const float  ROAD_ANIMATION_VAL = 6.0f;
static const float  ROAD_START_Y       = 0.0f;
static const float  DONKEY_START_Y     = 30.0f;
static const float  DRIVER_START_Y     = 150.0f;
static const float  FONT_WIDTH         = 16.0f;
static const float  FONT_HEIGHT        = 16.0f; 

static const int    SCREEN_WIDTH       = 320;
static const int    SCREEN_HEIGHT      = 240;

static const ULONG  BUTTON_WAIT_STATE  = 3;
static const ULONG  DONKEY_WIDTH       = 29;
static const ULONG  DONKEY_HEIGHT      = 34;
static const ULONG  DRIVER_WIDTH       = 25;
static const ULONG  DRIVER_HEIGHT      = 39;
static const ULONG  NUM_DRIVER_TURNS   = 10;

static const DWORD  WHITE              = 0xFFFFFFFF;
static const DWORD  BLACK              = 0xFF000000;
static const DWORD  RED                = 0xFFFF0000;
static const DWORD  SLEEP_AMOUNT       = 50;
static const DWORD  SLEEP_FACTOR       = 3;

static TCHAR* DONKEY_TEXT        = TEXT( "DONKEY: %u" );
static TCHAR* DRIVER_TEXT        = TEXT( "DRIVER: %u" );
static const CHAR* BACKGROUND_SOUND   = "t:\\media\\audio\\pcm\\techno.wav";
static const CHAR* CARMOVE_SOUND      = "t:\\media\\audio\\pcm\\screetch.wav";
static const CHAR* DONKEYHIT_SOUND    = "t:\\media\\audio\\pcm\\crash.wav";



