@echo off
if not "%_echo%" == "" echo on
if not "%verbose%"=="" echo on

REM Special environment settings for debugger remotes
REM
REM Set the default environment

set DEBUG_REMOTE=1

if "%_NTDRIVE%" == "" set _NTDRIVE=C:
if "%_NTROOT%" == "" set _NTROOT=\NT
set _NTBINDIR=%_NTDRIVE%%_NTROOT%
set _NTUSER=%USERNAME%
path %PATH%;%_NTBINDIR%\PUBLIC\OAK\BIN

net use /PER:NO >nul

REM set up the user specific environment
REM

call %_NTBINDIR%\PUBLIC\TOOLS\ntuser.cmd DebugRemote

REM
REM Optional parameters to this script are command line to execute
REM
%1 %2 %3 %4 %5 %6 %7 %8 %9