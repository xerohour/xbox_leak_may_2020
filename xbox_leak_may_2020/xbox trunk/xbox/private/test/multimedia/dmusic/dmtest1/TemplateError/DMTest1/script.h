#pragma once

#include "globals.h"
#include "cicmusicx.h"

static const LPSTR INIT_SCRIPT           = "T:\\Media\\script\\init.spt";
static const LPSTR INIT_SCRIPT_NOEXIST   = "T:\\Media\\script\\haha.spt";
static const LPSTR INIT_SCRIPT_BAD       = "T:\\Media\\script\\bad.spt";
static const LPSTR INIT_SCRIPT_SYNTAX    = "T:\\Media\\script\\syntax.spt";
static const LPSTR INIT_SCRIPT_RUNTIME   = "T:\\Media\\script\\runtime.spt";
static const LPSTR CALLROUTINE_MASTER    = "T:\\Media\\script\\master.spt";
static const LPSTR VARIABLE_MASTER       = "T:\\Media\\script\\variables.spt";

static const LPSTR ROUTINE_GOOD          = "NoError";
static const LPSTR ROUTINE_NO_EXIST      = "Bingo";
static const LPSTR ROUTINE_RUNTIME_ERROR = "BadObject";
static const LPSTR ROUTINE_EMPTY         = "";

static const LPSTR VARIABLE_INIT         = "Init";
static const LPSTR NUMBER_GOOD           = "Number";
static const LPSTR VARIABLE_EMPTY        = "";
static const LPSTR VARIABLE_NO_EXIST     = "Bingo";
static const LONG NUMBER_VALUE           = 100;
static const LONG NEW_VALUE              = 200;

static const LPSTR SEGMENT_GOOD          = "PlayingSegment1";

HRESULT dmthCreateScript( LPSTR szScriptName, CtIDirectMusicLoader8* pLoader, CtIDirectMusicScript** ppScript );
HRESULT dmthInitScriptErrorInfo( DMUS_SCRIPT_ERRORINFO* pInfo );
void dmthVerifyScriptError( DMUS_SCRIPT_ERRORINFO* pInfo );